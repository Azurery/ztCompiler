#ifndef _ZT_TYPE_H_
#define _ZT_TYPE_H_
#include <cassert>
#include <list>
namespace ztCompiler {
class arithmetic_type;
class type;
class function_type;
class array_type;
class pointer_type;
class struct_union_type;
class enum_type;
	class type {
		friend class arithmetic_type;
	public:
		static const int machine_width = 4;
		enum class Storage_class_specifier{
			TYPEDEF=0x00,
			CONST = 0x01,
			VOLATILE = 0x02,
			RESTRICT = 0x03,
			AUTO=0X04,
			STATIC=0X05,
			REGISTER=0X06,

		};
		bool operator!=(const type& other) const {
			return !(*this == other);
		}

		virtual bool operator==(const type& other) const = 0;
		virtual bool compatible(const type& other) const = 0;

		virtual ~type(){}
		int width() const {
			return width_;
		}

		int qualifier() const {
			return qualifier_;
		}

		int set_qualifier(unsigned char qualifier) {
			qualifier_ = qualifier;
		}

		bool is_const() const {
			return qualifier_ & static_cast<unsigned char>(Storage_class_specifier::CONST);
		}

		virtual arithmetic_type* to_arithmetic_type() {
			return nullptr;
		}
	protected:
		int width_;
		unsigned char qualifier_;
		type(int width):width_(width){}
	};

	
	class arithmetic_type :public type {
		friend class type;
	public:
		enum class Type_arithmetic_specifier {
			BOOL,			CHAR,				UNSIGNED_CHAR,
			SHORT,			UNSIGNED_SHORT,		INT,
			UNSIGNED_INT,	LONG,				UNSIGNED_LONG,
			LONG_LONG,		UNSIGNED_LONG_LONG,	FLOAT,
			DOUBLE,			LONG_DOUBLE,		COMPLEX,
			UNSIGNED
		};
		virtual ~arithmetic_type(){}
		
		virtual const arithmetic_type* to_arithmetic_type() const {
			return this;
		}
		virtual bool operator==(type& other) const {
			const arithmetic_type* other_type = other.to_arithmetic_type();
			return (other_type != nullptr&&(other_type->tag_ == tag_));
		}

		virtual bool campatiable(const type& other) const {
			return false;
		}
		bool is_bool() const {
			return Type_arithmetic_specifier::BOOL == static_cast<Type_arithmetic_specifier>(tag_);
		}
		bool is_integer() const {
			return (Type_arithmetic_specifier::BOOL <= static_cast<Type_arithmetic_specifier>(tag_) &&
				Type_arithmetic_specifier::UNSIGNED_LONG >= static_cast<Type_arithmetic_specifier>(tag_));
		}

		bool is_real() const {
			return (Type_arithmetic_specifier::FLOAT <= static_cast<Type_arithmetic_specifier>(tag_) &&
				Type_arithmetic_specifier::DOUBLE >= static_cast<Type_arithmetic_specifier>(tag_));
		}

	protected:
		arithmetic_type(int tag):tag_(tag),type(caculate_width(tag)){
			assert((static_cast<Type_arithmetic_specifier>(tag_) >= Type_arithmetic_specifier::BOOL)&&
				(static_cast<Type_arithmetic_specifier>(tag_) <= Type_arithmetic_specifier::COMPLEX));
		}
	private:
		int tag_;
		static int caculate_width(int tag);

	};

	class derived_type :public type {
	public:
		type* derived() {
			return derived_;
		}
		const type* derived() const {
			return derived_;
		}
	protected:
		type* derived_;
		derived_type(type* derived,int width)
			:derived_(derived),type(width){}
	};

	class pointer_type :public derived_type {
		friend class type;
	public:
		~pointer_type(){}

		virtual pointer_type* to_pointer_type(){
			return this;
		}
		virtual const pointer_type* to_pointer_type() const {
			return this;
		}

		virtual bool operator==(const type& other) const;
		virtual bool compatible(const type& other) const;

	protected:
		pointer_type(type* derived)
			:derived_type(derived,machine_width){}
	private:
		/*....*/
	};

	class array_type :public pointer_type {
		friend class type;
	public:
		virtual ~array_type(){}
		virtual array_type* to_array_type() {
			return this;
		}
		virtual const array_type* to_array_type() const {
			return this;
		}

		virtual pointer_type* to_pointer_type() {
			return this;
		}
		virtual const pointer_type* to_pointer_type() const {
			return this;
		}

		bool operator==(const type& other) const {
			auto other_array = to_array_type();
			return (other_array != nullptr
				&&other_array->len_ == len_
				&&*other_array->compatible == *derived_);
		}

		bool compatible(const type& other) const {
			auto other_array = to_array_type();
			return (other_array != nullptr
				&&other_array->len_ == len_
				&&derived_->compatible(*other_array->derived_));
		}
	protected:
		array_type(size_t len, type* derived) 
				:len_(len),pointer_type(derived){
			//TODO: caculate the width
		}
	private:
		size_t len_;
	};

	class function_type:public derived_type{
		friend class type;
	public:
		enum class Type_function_specifier {
			INLINE,
			NORETURN
			/*
			[noreturn]是C++11的attribute specifier sequence

			Indicates that the function does not return.
			This attribute applies to function declarations only.The behavior is undefined if the function with this attribute actually returns.

			该specifier用来指示函数永不返回，
			有助于编译器进行编译优化（如尾递归等），
			也可以用于抑制编译器给出不必要的警告（如int f(); f(); ，不加[[noreturn]]的话，编译器会警告f()的返回值被忽略）

			但是，若函数的确有返回值，而你却指定[[noreturn]]的话，这就是未定义行为了
			*/
		};
	public:
		virtual ~function_type(){}
		virtual function_type* to_function_type() {
			return this;
		}
		virtual const function_type* to_function_type() const {
			return this;
		}

		virtual bool operator==(const type& other) const;
		virtual bool compatible(const type& other) const;

	protected:
		function_type(type* derived,const std::list<type*>& params)
			:derived_type(derived,-1),params_(params){}
	private:
		std::list<type*> params_;
	};

	class struct_union_type :public type {
		friend class type;
	public:
		enum class Type_struct_union_specifier {
			STRUCT_UNION
		};
	public:
		virtual ~struct_union_type() {
			//TODO
		}
		virtual struct_union_type* to_struct_union_type() {
			return this;
		}
		virtual const struct_union_type* to_struct_union_type() const {
			return this;
		}

		virtual bool operator==(const type& other);
		virtual bool compatible(const type& other);

		identifier* find(const char* name);
		const identifier* find(const char* name) const;
	protected:
		explicit struct_union_type(environment* env)
			:env_(env),type(caculate_width(env)){}
	private:
		static int caculate_width(const environment* env);
		environment* env_;
	};

	class enum_type :public type {
	private:
		//TODO
	};

	class environment {
		friend class struct_union_type;
	public:
		explicit environment(environment* parent=nullptr)
			:parent_(parent),offset_(0){}
		identifier* find(const char* name);
		const identifier* find(const char*name) const;

		type* find_type(const char* name);
		const type* find_type(const char* name) const;

		identifier* find_variable(const char* name);
		const identifier* find_variable(const char* name) const;

		void insert_type(const char* name);
		void insert_variable(const char* name);

		bool operator==(const environment& other) const;
	private:
		environment* parent_;
		int offset_;
	};

	
}

#endif
