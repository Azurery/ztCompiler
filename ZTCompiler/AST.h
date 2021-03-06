/*!
 * \file   AST.h
 *
 * \author Magicmanoooo
 * \mail   zt838713968@gmail.com
 * \date   2018/08/23 
 *
 * \brief  
 *
 * TODO:   long description
 *
 * // Copyright (c) 2018 The ztCompiler Authors. All rights reserved.
 * // Use of this source code is governed by a BSD-style license that can be
 * // found in the LICENSE file. See the AUTHORS file for names of contributors.
 * 
 */
#ifndef _ZT_AST_H_
#define _ZT_AST_H_

#include "Token.h"
#include "Parser.h"
#include "Type.h"
#include <list>

namespace ztCompiler {
	class qualifier_type;
	class type;
	class expression;
	class token;
	class visitor;
	class enumerator;
	class identifier;
	class arithmetic_type;
	class pointer_type;
	class constant;
	class environment;
	
	/*!
	 * \class ast_node
	 *
	 * \brief 抽象语法树结点，所有的语法结点都直接或者间接继承自它
	 *
	 */
	class ast_node {
	public:
		virtual ~ast_node() {}
		virtual void accept(visitor* vistor_) = 0;
	protected:
		ast_node() {}
/*		memory_pool* pool_ = nullptr;*/
	};

	/*!
	 * \class statement
	 *
	 * \brief 普通的语句
	 *
	 */
	class statement : public ast_node {
	public:
		virtual ~statement() {}
	protected:
		statement() {}
	};

	/*!
	 * \class expression
	 *
	 * \brief 1.单目操作符（unary operator）
	 *		  2.双目操作符（binary operator）
	 *        3.条件操作符（condition operator）
	 *        4.函数调用（function call）
	 *        5.标识符（identifier）
	 *        6.对象（object）
	 *
	 */

	class expression : public statement {
	public:
		virtual ~expression() {}
		/*virtual bool is_lvalue() = 0;
		virtual void check_type() = 0;*/
		virtual bool is_lvalue() const;
		type* type() {
			return qualifier_type_->get_ptr();
		}
		//const qualifier_type* type() const {
		//	return qualifier_type_->get_ptr();
		//}
		virtual void accept(visitor* visitor_);
		const token* get_token() const { return this->token_; }
	protected:
		expression(const token* token, qualifier_type* type_)
			:token_(token), qualifier_type_(type_) {}
		const token*  token_;
		qualifier_type* qualifier_type_;
	};

	//identifier 标识符
	class identifier :public expression {
	public:
		virtual ~identifier() {}
		//identifier是左值
		virtual bool is_lvalue() const {
			return true;
		}
		virtual void accept(visitor* visitor_);
		virtual enumerator* covert_to_enumerator() { return nullptr; }
		virtual const std::string indentifier_name() const {
			return token_->get_token_name();
		}

		bool operator!=(const identifier* other) const {
			return !(other == this);
		}
		static identifier* create(const token* token_, qualifier_type qualifier_type_);
	public:
		identifier(qualifier_type* qualifier_type_, const token* token_)
			:expression(token_,qualifier_type_){}
	};
 
	/*!
	 * \class labeled-statement:
					attribute-specifier-seqopt identifier : statement
					attribute-specifier-seqopt case constant-expression : statement
					attribute-specifier-seqopt default : statement
	 *
	 * \brief 标号语句
	 *
	 */
	class labeled_statement : public statement {
	private:
		int label_;
		int generate_label() {
			int label = 0;
			return ++label;
		}
	public:
		static labeled_statement * create();
		~labeled_statement() {}
		virtual void accept(visitor* visitor_);
		std::string labeled_wrapper() const { return std::to_string(label_); }
	protected:
		labeled_statement() :label_(generate_label()) {}
	};

	/*!
	 * \class selection_statement 
	 *
	 * \brief selection-statement 
	 *				if ( expression ) statement
	 *				if ( expression ) statement else statement
	 *				switch ( expression ) statement
	 */
	class selection_statement : public statement {
	private:
		labeled_statement * select_label_;
	protected:
		selection_statement(labeled_statement* label_state){}
	public:
		virtual const void accpet() = 0;
		virtual const selection_statement* create() const = 0;
		virtual ~selection_statement(){}

	};
	//compound-statement 复合语句
	/*(6.8.2) compound-statement:
				{ block-item-listopt }
		(6.8.2) block-item-list:
				block-item
				block-item-list block-item
		(6.8.2) block-item:
				declaration
				statement
	*/
	using statement_list = std::list<statement*>;
	class compound_statement :public statement {
	private:
		statement_list statements_;
	public:
			compound_statement(const statement_list& statements):
			statements_(statements){}
	public:
		static compound_statement * create(statement_list& statements);
		virtual ~compound_statement(){}
		virtual void accept(visitor* visitor_);
		statement_list& statements_value(){ return statements_; }
	};

	/*(6.8.5) iteration-statement:
				while ( expression ) statement
				do statement while ( expression ) ;
				for ( expressionopt ; expressionopt ; expressionopt ) statement
				for ( declaration expressionopt ; expressionopt ) statement
	*/

	class iteration_statement : public statement {

	};


	//jump-statement 跳转语句
	/*	jump-statement:
			goto identifier ;
			continue ;
			break ;
			return expressionopt ;
	*/
	class jump_statement :public statement {
	private:
		labeled_statement * label_;
	protected:
		jump_statement(labeled_statement* label) :
			label_(label){}
	public:
		static jump_statement* create(statement* statement_);
		~jump_statement();
		virtual void accept(visitor* visitor_);
		const labeled_statement* jump_wrapper() const { return this->label_; }
	};

	//if statements
	class if_statement : public selection_statement {
	private:
		expression* condition_;
		statement* then_;
		statement* else_;
	protected:
		/*if_statement(expression* condition, statement* then, statement* elses = nullptr) :
			condition_(condition),then_(then),else_(elses){}
		*/
	public:
		virtual ~if_statement(){}
		static if_statement* create(expression* condition, statement* then, statement* elses = nullptr);
		virtual void accept(visitor* vistor_);
		expression* if_statement_condotion_wrapper() { return this->condition_; }
	};

	//integer or float
	class constant : public expression {
		friend class translate_unit;
	public:
		~constant(){}
		virtual bool is_lvalue() const {
			return false;
		}
		static constant* create(const token* token_, int flag, long value);
		static constant* create(const token* token_, int flag, double value);
		static constant* create(const token* token_, int flag, const std::string& value);
	protected:
		size_t value_;
		constant(const token* type, qualifier_type* qualifier_type,long value)
			:expression(type,qualifier_type),value_(value) {}
		constant(const token* type, qualifier_type* qualifier_type, double value)
			:expression(type, qualifier_type), value_(value) {}
		/*constant(const token* type, qualifier_type* qualifier_type, std::string value)
			:expression(type, qualifier_type_), value_(value) {}*/
	};

	
	/*二元操作符
	*+ - *  / % < > << >> | & ^ =
	*== != <= >=
	*&& ||
	*.(成员运算符）
	*/
	class binary_expression : public expression {
		friend class translate_unit;
		friend class visitor;
	public:
		static binary_expression* create(const token* token_, expression* lhs, expression* rhs);
		static binary_expression* create(const token* token_, int token_attr_,expression* lhs, expression* rhs);
		virtual ~binary_expression() {}
		virtual void accept(visitor* visitor_);
		int width() const;
		int operation_wrapper() const { return this->op_; }
		expression* lhs_wrapper() const { return this->lhs_; }
		expression* rhs_wrapper() const { return this->rhs_; }
		virtual bool is_lvalue() const {
			//TODO:switch(op_)
			//算数表达式不是左值，但一些operator是左值，如operator-
			return false;
		}
	protected:
		binary_expression(qualifier_type* type,int op, expression* lhs, expression* rhs)
			:expression(nullptr, type ), op_(op),lhs_(lhs), rhs_(rhs) {}
	protected:
		int op_;
		expression* lhs_;
		expression* rhs_;
	};

	class unary_expression :public expression {
		friend class translate_unit;
	public:
		~unary_expression(){}
		
		//TODO:像'++i'就是左值，'~i'便不是左值
		virtual bool is_lvalue() const {
			//解引用deref('*') op是左值
			return (TokenAttr::DEREFERENCE == static_cast<TokenAttr>(op_));
		}
		expression* unary_expression_wrapper() const { return this->expression_; }
		static unary_expression* create(int flag, expression* operand);
		static unary_expression* create(int flag, expression* operand, qualifier_type*);
	protected:
		unary_expression(qualifier_type* type, int op, expression* expression)
			:expression( nullptr, type), expression_(expression) {

		}
	private:
		int op_;
		expression* expression_;
	};

	
	// cond ? true ： false
	class conditional_expression : public expression {
	private:
		expression* condition_;
		expression* true_;
		expression* false_;
	public:
		~conditional_expression(){}
		static conditional_expression* create(const token* token_,expression* condition_, expression* true_, expression* false_);
		void accept(visitor* visitor_);
		expression* condition_wrapper() { return this->condition_; }
		expression* true_wrapper() { return this->true_; }
		expression* false_wrapper() { return this->false_; }
		virtual bool is_lvalue() { return false; }
	protected:
		conditional_expression(expression* condition, expression* true_expression, expression* false_expression)
			:expression(condition->get_token(),nullptr), true_(true_expression), false_(false_expression){}
	};

	//empty语句
	class empty_statement :public statement {
	public:
		virtual ~empty_statement() {}
		virtual void accept(visitor* vistor_);
		static empty_statement* create();
	protected:
		empty_statement() {}
	};

	//return语句
	class return_statement :public statement {
	public:
		virtual ~return_statement();
		virtual void accept(visitor* visitor_);
		static return_statement* construct();
	protected:
		return_statement(expression* expr):expr_(expr){}
	private:
		expression* expr_;
	};

	
	//fuction call
	class function_call :public expression {
		friend class translate_unit;
	public:
		~function_call(){}
		//function call不是lvalue
		virtual bool is_lvalue() const {
			return false;
		}

	protected:
		function_call(qualifier_type* type,expression* caller,std::list<expression*> args)
			:expression(nullptr,type),caller_(caller),args_(args){}
	private:
		expression* caller_;
		std::list<expression*> args_;
	};

	//enumerator
	class enumerator :public identifier {
	public:

	protected:
		//enumerator(const token* token_, int value) :identifier(arithmetic_type::create(),token_){}
	private:
		constant * constant_;
	};

	using declaration = ast_node;
	class translate_unit :public ast_node {
	public:
		virtual ~translate_unit() {
			auto iter = declaration_.begin();
			while (iter != declaration_.end()) {
				delete *iter;
				iter++;
			}
		}

		void add(declaration* declaration) {
			declaration_.push_back(declaration);
		}

		//static translate_unit* new_translate_unit() {
		////	return new translate_unit();
		////}

		static binary_expression* new_binary_operation(qualifier_type* type,int op,expression* lhs,expression* rhs) {
			return new binary_expression(type,op,lhs,rhs);
		}

		static unary_expression* new_unary_operation(qualifier_type* ty, int op, expression* expr) {
			return new unary_expression(ty,op,expr);
		}

		static function_call* new_function_call(qualifier_type* type,expression* caller,const std::list<expression*>& args) {
			return new function_call(type,caller,args);
		}

		static identifier* new_identifier(qualifier_type* type,token* token_) {
			return new identifier(type,token_);
		}

		static constant* new_constant(qualifier_type* type_,long value) {
			return new constant(nullptr,type_,value);
		}

		static constant* new_constant(qualifier_type* type_) {
			return new constant(nullptr,type_,static_cast<double>(sizeof(type_)));
		}

	private:
		translate_unit(){}
		std::list<declaration*> declaration_;
	};

	/*!
	 * \class visitor
	 *
	 * \brief 采用visitor模式
	 *
	 */
	class visitor {
		friend class binary_expression;
	public:
		virtual ~visitor() {}
		virtual void visit_binary_operation(binary_expression* binary_) = 0;
		virtual void visit_unary_operation(unary_expression* binary_) = 0;
		virtual void visit_jump_statement(jump_statement* jump_statement_) = 0;
		virtual void visit_if_statement(if_statement* if_statement_) = 0;
		virtual void visit_label_statement(labeled_statement* label_statement_) = 0;
		virtual void visit_compound_statement(compound_statement* compound_statement_) = 0;
	};
}
#endif 