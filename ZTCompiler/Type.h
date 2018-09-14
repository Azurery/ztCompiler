/*!
 * \file   Type.h
 *
 * \author Magicmanoooo
 * \mail   zt838713968@gmail.com
 * \date   2018/08/23 
 *
 * \brief  
 *
 * TODO:   long description
 *
 * // Copyright (c) 2018 The ZTCompiler Authors. All rights reserved.
 * // Use of this source code is governed by a BSD-style license that can be
 * // found in the LICENSE file. See the AUTHORS file for names of contributors.
 * 
 */

#ifndef _ZT_TYPE_H_
#define _ZT_TYPE_H_
#include <cassert>
#include <list>
#include "AST.h"

namespace ztCompiler {
	class value;
	class user;
	class use;
	class type;
	class basic_block;
	class cfg;
	class qualifier_type;
	class arithmetic_type;
	class function_type;
	class array_type;
	class derived_type;
	class struct_union_type;
	class pointer_type;
	class identifier;
	class environment;
	
	/*!
	 * \class value
	 *
	 * \brief 每一个value都有一个use list，用于追踪其他哪些值引用了当前值。
	 *		  它是一个程序所计算的所有的值的base class，可以用作其他值的operands。
	 *		  value是instruction个function等重要的class的super class。所有的values
	 *		  都有一个type。type不是value的subclass。
	 */
	class value {
		friend class use;
		const unsigned short subclass_id;
	public:
		enum class value_type {
			undef_type,
			argurment_type,
			constant_type,
			instruction_type,
			basic_block_type
		};

		// FIXME
		//value(const type* ty, unsigned scid) {}

		value(const value&) = delete;
		value& operator=(const value&) = delete;
		virtual ~value();

		value(type* ty, unsigned scid) 
			:subclass_id(scid),type_(check_type(ty)){}

		using use_iterator = std::list<use*>::iterator;
		using const_use_iterator = std::list<use*>::const_iterator;
		use_iterator		use_begin()			{ return uses_.begin(); }
		const_use_iterator	use_begin() const	{ return uses_.cbegin(); }
		use_iterator		use_end()			{ return uses_.end(); }
		const_use_iterator	use_end()   const	{ return uses_.cend(); }
		size_t				use_size()  const   { return uses_.size(); }
		
		const std::string& get_value_name() const    { return this->value_name_; }
		void set_value_name(std::string name) { this->value_name_ = name; }
		void replace_all_uses_with(value* new_value) {
			assert(new_value
				&& "value::replace_all_uses_with(<null>) is invalid!");
			assert(new_value != this
				&& "this->replace_all_uses_with(this) is invalid!");
			assert(new_value->get_type() == this->get_type()
				&& "replace_all_uses of value with new value of different type!");
			unchecked_replace_all_uses_with(new_value);
		}

		void unchecked_replace_all_uses_with(value* new_value) {
			while (!uses_.empty()) {
				use* u = uses_.front();
				u->set(new_value);
			}
		}
		const type* check_type(const type* ty) {
			assert(ty != nullptr && "value cannot be defined with null type");
			return ty;
		}

		const type* get_type() const { return type_; }
	protected:
		std::list<use*>		uses_;
		std::string	value_name_;
		type*				type_;
	private:
		void add_use_to_list(use* u) { 
			assert(u != nullptr);
			uses_.push_back(u);
		}

		void remove_use_from_list(use* u) {
			assert(u != nullptr);
			uses_.remove(u); 
		}
	};


	/*!
	 * \class use
	 *
	 * \brief 用于表示一个指令的操作数或者一些指向value的user实例。
	 *		  一个use代表了一个value定义与其user之间的边,即use对象一头连接着user
	 *		  （所有instruction都继承自user），一头连接着value。然后这些use对象
	 *		  以链表的形式组织起来。每次创建一个新的use对象时，就会调用构造函数
	 *		  将当前的use对象连接到value对象的use链表上
	 *
	 */
	class use {
		friend class value;
	public:
		use(value* value, user* user) {
			this->value_ = value;
			this->user_ = user;
			if (this->value_ != nullptr) {
				this->value_->add_use_to_list(this);
			}
		}

		use(const use& u)
			:use(u.value_, u.user_) {}
		use(const use&& u)
			:use(std::move(u.value_), std::move(u.user_)) {}
		
		~use() {
			if (this->value_ != nullptr) {
				this->value_->remove_use_from_list(this);
			}
		}

		void set(value* val) {
			if (this->value_)
				this->value_->remove_use_from_list(this);
			this->value_ = val;
			if (this->value_)
				this->value_->add_use_to_list(this);
		}

		value*	get_value()		{ return value_; }
		user*	get_user()		{ return user_; }
		const use&	operator=(const use& other) {
			set(other.value_);
			return *this;
		}

		value* operator=(value* other) {
			set(other);
			return other;
		}

		bool operator==(const use& other) {
			return (this->value_ == other.value_&&
				this->user_ == other.user_);
		}
	private:
		value*	value_;
		user*	user_;
	};

	class user : public value {
		user(const user&) = delete;
	public:
// 		user(unsigned scid)
// 			:value(scid) {}

		virtual ~user() {
			operands_.clear();
		}
		using operand_iterator=std::vector<use>::iterator ;
		using const_operand_iterator = std::vector<use>::const_iterator;
		operand_iterator		operand_bagin()			{ return operands_.begin(); }
		const_operand_iterator	operand_bagin() const	{ return operands_.cbegin(); }
		operand_iterator		operand_end()			{ return operands_.end(); }
		const_operand_iterator	operand_end() const		{ return operands_.cend(); }

		///用于将移除操作数链表中的一个参数
		operand_iterator operand_erase(operand_iterator iter) { return operands_.erase(iter); }
		operand_iterator operand_erase(operand_iterator begin, operand_iterator end) { return operands_.erase(begin,end); }

	protected:
		std::vector<use*> operands_;
	};

	/*!
	 * \class instruction
	 *
	 * \brief 
	 *
	 */
	class instruction : public user {
	public:
		enum class instruction_type {
			undef_type,
			phi_type,
			binary_operand_type
		};
		
		instruction(unsigned value_id)
			: parent_(0)
			, user(static_cast<unsigned>(value::value_type::instruction_type))
			, operand_(value_id) {}

		instruction(unsigned value_id, const char* value_name)
			:instruction(value_id) {
			this->set_value_name(value_name);
		}

// 		instruction(int value_id, const std::string& value_name)
// 			:instruction(value_id, value_name) {}

		/*instruction(const type* ty, int type_categoty, const std::string& name = "");*/
		virtual ~instruction();
		virtual void replace_by(instruction* other) {
			assert(parent_ != nullptr&&other != this);
			replace_all_uses_with(other);
			//parent_->replace_instruction_with();

		}

		basic_block* get_parent() const { return parent_; }
		void set_parent(basic_block* val) { parent_ = val; }
		unsigned get_operand() const { return operand_; }
		bool is_phi() const { return get_operand() == instruction::instruction_type::phi_type; }
	protected:
		basic_block* parent_;
		const unsigned operand_;
	};


	class phi_node : public instruction {
		///禁止对phi_node进行拷贝赋值
		phi_node(const phi_node& pnode) = delete;
	public:
		phi_node(const char* name)
			:instruction(static_cast<unsigned>(instruction::instruction_type::phi_type), name) {}

		phi_node(const std::string& name)
			:instruction(static_cast<unsigned>(instruction::instruction_type::phi_type), name.c_str()) {}

		void append_operand(value* val) {
			use* u = new use(val, dynamic_cast<user*>(this));
			operands_.push_back(u);
		}
	};

	class type: public instruction {
	public:
		static const int machine_width = 4;
		enum class Storage_class_specifier{
			TYPEDEF,
			AUTO,
			STATIC,
			REGISTER,
			EXTERN,
			_THREAD_LOACAL
		};
		enum class Type_qualifier {
			CONST,
			VOLATILE,
			RESTRICT,
			ATOMIC
		};

		bool operator!=(const type& other) const {
			return !(*this == other);
		}

		virtual bool operator==(const type& other) const = 0;
		virtual bool compatible(const type& other) const = 0;
		
		virtual ~type(){}
		virtual int width() const = 0;
		int qualifier() const {
			return qualifier_;
		}

		int set_qualifier(unsigned char qualifier) {
			qualifier_ = qualifier;
		}

		bool is_const() const {
			return qualifier_ & static_cast<unsigned char>(Type_qualifier::CONST);
		}
		/*Integer types（整数类型）
			short int
			unsigned short int
			int
			unsigned int
			long int
			unsigned long int
			long long int
			unsigned long long int
		*/
		virtual bool is_integer() const {return false;}
		virtual bool is_float() const { return false; }
		/*scalar types（标量类型）
			arithmetic types
			pointer types
		*/
		virtual bool is_scalar() const { return false; }
		virtual bool is_bool() const { return false; }
		virtual bool is_real() const { return false; }
		virtual bool is_complex() const { return false; }
		virtual bool is_unsigned() const { return false; }


		virtual arithmetic_type* to_arithmetic_type() { return nullptr; }
		virtual arithmetic_type* to_arithmetic_type() const { return nullptr; }
		virtual function_type* to_function_type() { return nullptr; }
		virtual function_type* to_function_type() const { return nullptr; }
		virtual array_type* to_array_type() { return nullptr; }
		virtual array_type* to_array_type() const { return nullptr; }

		virtual derived_type* to_derived_type() { return nullptr; }
		virtual derived_type* to_derived_type() const { return nullptr; }
		virtual struct_union_type* to_struct_type() { return nullptr; }
		virtual struct_union_type* to_struct_type() const { return nullptr; }
		virtual pointer_type* to_pointer_type() { return nullptr; }
		virtual pointer_type* to_pointer_type() const { return nullptr; }

	protected:
		int width_;
		unsigned char qualifier_;
		type(int width)
			: width_(width){}
	};

	class qualifier_type {
	public:
		qualifier_type(type* ptr) :ptr_(reinterpret_cast<intptr_t>(ptr)) {}
		const type* get_ptr() const {
			return reinterpret_cast<const type*>(ptr_);
		}
		type* get_ptr() {
			return reinterpret_cast<type*>(ptr_);
		}
		bool is_null() const { return get_ptr() == nullptr; }
		operator bool() const { return !is_null(); }
	private:
		intptr_t ptr_;
	};

	class arithmetic_type : public type {
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
		
		virtual bool operator==(type& other) const {
			const arithmetic_type* other_type = other.to_arithmetic_type();
			return (other_type != nullptr&&(other_type->tag_ == tag_));
		}

		virtual bool campatiable(const type& other) const {
			return false;
		}
		virtual bool is_bool() const {
			return Type_arithmetic_specifier::BOOL == static_cast<Type_arithmetic_specifier>(tag_);
		}
		virtual bool is_integer() const {
			return (Type_arithmetic_specifier::BOOL <= static_cast<Type_arithmetic_specifier>(tag_) &&
				Type_arithmetic_specifier::UNSIGNED_LONG >= static_cast<Type_arithmetic_specifier>(tag_));
		}

		virtual bool is_real() const {
			return (Type_arithmetic_specifier::FLOAT <= static_cast<Type_arithmetic_specifier>(tag_) &&
				Type_arithmetic_specifier::DOUBLE >= static_cast<Type_arithmetic_specifier>(tag_));
		}
		//判断arithmetic type是否有虚数部分
		virtual bool is_complex() const {
			return tag_ & static_cast<int>(TokenAttr::COMPLEX);
		}
		//arithmetic type自然是标量类型
		virtual bool is_scalar() const { return true; }
		//将arithmetic type与float或者double进行&运算。若相同则为floating type
		virtual bool is_float() const {
			return (tag_ & static_cast<int>(TokenAttr::FLOAT)) ||
				(tag_ & static_cast<int>(TokenAttr::DOUBLE));
		}
		static arithmetic_type* create(){}
		//用于返回tag_
		int tag_value() { return tag_; }
		virtual int width() const;
	protected:
		arithmetic_type(int tag):tag_(tag),type(caculate_width(tag)){
			assert((static_cast<Type_arithmetic_specifier>(tag_) >= Type_arithmetic_specifier::BOOL)&&
				(static_cast<Type_arithmetic_specifier>(tag_) <= Type_arithmetic_specifier::COMPLEX));
		}
	private:
		int tag_;
		static int caculate_width(int tag);

	};

	class derived_type : public type {
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


		//virtual const pointer_type* to_pointer_type() const {
		//	return this;
		//}
		static pointer_type* create(qualifier_type qualifier_type_);
		virtual const pointer_type* to_pointer() const { return this; }
		virtual bool operator==(const type& other) const;
		virtual bool compatible(const type& other) const;
		//指针变量的大小为64位（即8字节）
		virtual int width() const { return 8; }
		virtual bool is_scalar() const { return true; }
		//virtual bool is_void_pointer() const { return derived_->to_void(); }
	protected:
		pointer_type(type* derived_)
			:derived_type(derived_,machine_width){}
	private:
		/*....*/
	};

	class array_type : public pointer_type {
		friend class type;
	public:
		virtual ~array_type(){}
		//virtual pointer_type* to_pointer_type() {
		//	return this;
		//}
		bool operator==(const type& other) const {
			auto other_array = to_array_type();
			return (other_array != nullptr
				&&(other_array->len_ == len_)
				/*&&(*(other_array->compatible) == (*derived_))*/);
		}

		bool compatible(const type& other) const {
			auto other_array = to_array_type();
			return (other_array != nullptr
				&&other_array->len_ == len_
				&&derived_->compatible(*other_array->derived_));
		}

		int width() const { return sizeof(array_type*)*len_; }
	protected:
		array_type(size_t len, type* derived) 
				:len_(len),pointer_type(derived){
			//TODO: caculate the width
		}
	private:
		size_t len_;
	};

	class function_type : public derived_type{
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
		/*virtual function_type* to_function_type() {
			return this;
		}
*/
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

	class enum_type : public type {
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
