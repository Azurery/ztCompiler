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
#include "CFG.h"
namespace ztCompiler {
	class value;
	class user;
	class use;
	class type;
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
	 * \brief ÿһ��value����һ��use list������׷��������Щֵ�����˵�ǰֵ��
	 *		  ����һ����������������е�ֵ��base class��������������ֵ��operands��
	 *		  value��instruction��function����Ҫ��class��super class�����е�values
	 *		  ����һ��type��type����value��subclass��
	 */
	class value {
		friend class use;
	public:
// 		enum class value_type {
// 			undef_value,
// 			type_value,
// 			argurment_type,
// 			instruction_type,
// 			basic_block_type
// 		};
		value(unsigned id);
		value(const value&) = delete;
		value& operator=(const value&) = delete;
		virtual ~value();

		using use_iterator = std::list<use*>::iterator;
		using const_use_iterator = std::list<use*>::const_iterator;
		use_iterator		use_begin()			{ return uses_.begin(); }
		const_use_iterator	use_begin() const	{ return uses_.cbegin(); }
		use_iterator		use_end()			{ return uses_.end(); }
		const_use_iterator	use_end()   const	{ return uses_.cend(); }
		size_t				use_size()  const   { return uses_.size(); }
		
		const std::string& get_value_name() const    { return this->value_name_; }
		void set_value_name(const std::string& name) { this->value_name_ = name; }
	protected:
		std::list<use*>		uses_;
		const std::string&	value_name_;
	private:
		void add_use_to_list(use* u) { 
			static_assert(u);
			uses_.push_back(u);
		}

		void remove_use_from_list(use* u) {
			static_assert(u);
			uses_.remove(u); 
		}
	};


	/*!
	 * \class use
	 *
	 * \brief ���ڱ�ʾһ��ָ��Ĳ���������һЩָ��value��userʵ����
	 *		  һ��use������һ��value��������user֮��ı�,��use����һͷ������user
	 *		  ������instruction���̳���user����һͷ������value��Ȼ����Щuse����
	 *		  ����������ʽ��֯������ÿ�δ���һ���µ�use����ʱ���ͻ���ù��캯��
	 *		  ����ǰ��use�������ӵ�value�����use������
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

		value*	get_value()		{ return value_; }
		user*	get_user()		{ return user_; }
		user&	operator=(const use& other) {
			this->user_ = other.user_;
			if (this->value_ != nullptr) {
				this->value_->remove_use_from_list(this);
			}
			this->value_ = other.value_;
			this->value_->add_use_to_list(this);
		}

		bool	operator==(const use& other) {
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
		virtual ~user() {
			operands_.clear();
		}
		using operand_iterator=std::vector<use>::iterator ;
		using const_operand_iterator = std::vector<use>::const_iterator;
		operand_iterator		operand_bagin()			{ return operands_.begin(); }
		const_operand_iterator	operand_bagin() const	{ return operands_.cbegin(); }
		operand_iterator		operand_end()			{ return operands_.end(); }
		const_operand_iterator	operand_end() const		{ return operands_.cend(); }

		///���ڽ��Ƴ������������е�һ������
		operand_iterator operand_erase(operand_iterator iter) { return operands_.erase(iter); }
		operand_iterator operand_erase(operand_iterator begin, operand_iterator end) { return operands_.erase(begin,end); }

	private:
		std::vector<use> operands_;
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
			phi_type,
			binary_operand_type,

		};
		
		instruction(int value_id);
		instruction(int value_id, const char* value_name);
		instruction(int value_id, const std::string& value_name);
		instruction(const type* ty, int type_categoty, const std::string& name = "");
		virtual ~instruction();

		basic_block* get_parent() const { return parent_; }
		void set_parent(basic_block* val) { parent_ = val; }
	protected:
		int type_categoty;
		basic_block* parent_;
	};


	class phi_node : public instruction {
		///��ֹ��phi_node���п�����ֵ
		phi_node(const phi_node& pnode) = delete;
	public:
		phi_node(const type* ty, const std::string& name = "")
			:instruction(ty, instruction_type::phi_type, name) {}
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
		/*Integer types���������ͣ�
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
		/*scalar types���������ͣ�
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
		type(int width):width_(width){}
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
		//�ж�arithmetic type�Ƿ�����������
		virtual bool is_complex() const {
			return tag_ & static_cast<int>(TokenAttr::COMPLEX);
		}
		//arithmetic type��Ȼ�Ǳ�������
		virtual bool is_scalar() const { return true; }
		//��arithmetic type��float����double����&���㡣����ͬ��Ϊfloating type
		virtual bool is_float() const {
			return (tag_ & static_cast<int>(TokenAttr::FLOAT)) ||
				(tag_ & static_cast<int>(TokenAttr::DOUBLE));
		}
		static arithmetic_type* create(){}
		//���ڷ���tag_
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
		//ָ������Ĵ�СΪ64λ����8�ֽڣ�
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
			[noreturn]��C++11��attribute specifier sequence

			Indicates that the function does not return.
			This attribute applies to function declarations only.The behavior is undefined if the function with this attribute actually returns.

			��specifier����ָʾ�����������أ�
			�����ڱ��������б����Ż�����β�ݹ�ȣ���
			Ҳ�����������Ʊ�������������Ҫ�ľ��棨��int f(); f(); ������[[noreturn]]�Ļ����������ᾯ��f()�ķ���ֵ�����ԣ�

			���ǣ���������ȷ�з���ֵ������ȴָ��[[noreturn]]�Ļ��������δ������Ϊ��
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