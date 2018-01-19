#ifndef _ZT_CODE_GENERATION_H_
#define _ZT_CODE_GENERATION_H_
#include "AST.h"

class parser;
class token;
class binary_operation;
namespace ztCompiler {
	class code_generator : public visitor {
		//System V的参数传递方式
	public:
		enum class Parameter_passing {
			INTEGER,
			SSE,
			SSE_UP,
			X87,
			X87_UP,
			COMPLEX_X87,
			MEMORY,
			NO_CLASS
		};
	public:

	protected:
		void emit(const std::string& str);
		void emit(const std::string& instruction_, const std::string& source_, const std::string& destination_);
		// immediate表示immediate date，立即数 常数 
		void emit(const std::string& instruction_,int immediate);
		void emit(const std::string& instruction_, const labeled_statement* label_statement_);


		static Parameter_passing classify(type* parameter_type_);
		
		static std::string get_instruction(const std::string& instruction_, int width_);
		static std::string get_instruction(const std::string& instruction_, type* type_);
		static std::string get_register(int width_);
		static std::string get_load_instruction(int width_);
		static std::string get_destination(int width_);
		static std::string get_source(int width_);

		int push(const std::string& register_);
		int push(type* type_);
		int pop(const std::string& register_);

		virtual void visit_binary_operation(binary_expression* binary_expression_);
		virtual void visit_jump_statement(jump_statement* jump_statement_);
		virtual void visit_if_statement(if_statement* if_statement_);
		virtual void visit_expression(statement* statement_);


		void generate_comma_operator(binary_expression* comma_);
		void generate_member_reference_operator(binary_expression* binary_operator_);


	private:
		static int stack_position_;
		static FILE* output_file_;
	};
	
}



#endif
