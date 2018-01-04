#ifndef _ZT_TYPE_H_
#define _ZT_TYPE_H_
#include <cassert>
#include <list>
namespace ztCompiler {
class arithmetic_type;
class variable;
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
		enum class Qualifier{
			//Storage class specifiers
			TYPEDEF=0x00,
			CONST = 0x01,
			VOLATILE = 0x02,
			RESTRICT = 0x03,
			AUTO=0X04,
			STATIC=0X05,
			REGISTER=0X06,

			//Type specifiers

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
			return qualifier_ & static_cast<unsigned char>(Qualifier::CONST);
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
		enum class Arithmetic_qualifier {
			BOOL=0,CHAR,UNSIGNED_CHAR,
			SHORT,UNSIGNED_SHORT,
			INT,UNSIGNED_INT,LONG,UNSIGNED_LONG,LONG_LONG,
			UNSIGNED_LONG_LONG,FLOAT,DOUBLE,LONG_DOUBLE, COMPLEX
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
			return Arithmetic_qualifier::BOOL == static_cast<Arithmetic_qualifier>(tag_);
		}
		bool is_integer() const {
			return (Arithmetic_qualifier::BOOL <= static_cast<Arithmetic_qualifier>(tag_) &&
				Arithmetic_qualifier::UNSIGNED_LONG >= static_cast<Arithmetic_qualifier>(tag_));
		}

		bool is_real() const {
			return (Arithmetic_qualifier::FLOAT <= static_cast<Arithmetic_qualifier>(tag_) &&
				Arithmetic_qualifier::DOUBLE >= static_cast<Arithmetic_qualifier>(tag_));
		}

	protected:
		arithmetic_type(int tag):tag_(tag),type(caculate_width(tag)){
			assert((static_cast<Arithmetic_qualifier>(tag_) >= Arithmetic_qualifier::BOOL)&&
				(static_cast<Arithmetic_qualifier>(tag_) <= Arithmetic_qualifier::COMPLEX));
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
			:derived_type(derived,machine_word){}
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

	using symbol = variable;
	class struct_union_type :public type {
		friend class type;
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

		variable* find(const char* name);
		const variable* find(const char* name) const;
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
		symbol* find(const char* name);
		const symbol* find(const char*name) const;

		type* find_type(const char* name);
		const type* find_type(const char* name) const;

		variable* find_variable(const char* name);
		const variable* find_variable(const char* name) const;

		void insert_type(const char* name);
		void insert_variable(const char* name);

		bool operator==(const environment& other) const;
	private:
		environment* parent_;
		int offset_;
	};

	
	
}

#endif
