#ifndef _ZT_STATEMENT_H_
#define _ZT_STATEMENT_H_
#include "MemoryPool.h"
#include "Token.h"
namespace ztCompiler {
	class visitor {
	public:
		virtual ~visitor(){}
		
	};
	//抽象语法树结点，所有的语法结点都直接或者间接继承自它
	class ast_node {
	public:
		virtual ~ast_node(){}
		virtual void visit(visitor* vistor_) = 0;
	protected:
		ast_node(){}
		memory_pool* pool_=nullptr;
	};

	//语句
	class statement :public ast_node {
	public:
		virtual ~statement(){}
	protected:
		statement(){}
	};

	/*表达式expression分为：
	*1.单目操作符
	*2.双目操作符
	*3.条件操作符
	*4.函数调用
	*5.标识符
	*6.对象
	*/
	class expresstion : public statement {
		virtual ~expresstion(){}
		virtual bool is_lvalue() = 0;
		virtual void check_type() = 0;
		const token* get_token() const { return ; }
	};


	class empty_statement :public statement {
	public:
		virtual ~empty_statement() {}
		virtual void visit(visitor* vistor_);
		static empty_statement* construct();
	protected:
		empty_statement(){}
	};

	//if语句
	class if_statement :public statement {
	public:
		virtual ~if_statement(){}
		virtual void visit(visitor* vistor_);
		static if_statement* construct();
	protected:
	private:
		
	};
}
#endif