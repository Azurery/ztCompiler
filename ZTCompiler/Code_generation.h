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

		struct object_address {
		private:
			std::string base_address_;
			int offset_;
			std::string label_;

			explicit object_address(int offset)
				:object_address("%rbp", offset, ""){}

			object_address(const std::string& base_address, int offset,const std::string& label)
				:base_address_(base_address), offset_(offset), label_(label){}
		public:
			//address=label_+offset_+(base_address)
			std::string clone() const {
				std::string ret;
				if (base_address_.empty())
					ret = "(" + base_address_ + ")";
				else ret = "";

				if (label_.empty()) {
					if (offset_ == 0) return ret;
					return std::to_string(offset_) + ret;
				}
				else {
					if (offset_ == 0)	return label_ + ret;
					return label_ + "+" + std::to_string(offset_) + ret;
				}
			}
		};

	protected:
		void emit(const std::string& str);
		void emit(const std::string& instruction_, const std::string& source_);
		void emit(const std::string& instruction_, const std::string& source_, const std::string& destination_);
		// immediate表示immediate date，立即数 常数 
		void emit(const std::string& instruction_,int immediate_);
		void emit(const std::string& instruction_, const labeled_statement* label_statement_);
		void emit_label(const std::string& label_);
		void emit_load(const std::string& address_, int width_, bool is_float_);
		void emit_load(const std::string& address_, type* type_);

		static Parameter_passing classify(type* parameter_type_);
		
		static std::string get_instruction(const std::string& instruction_, int width_,bool is_float_);
		static std::string get_instruction(const std::string& instruction_, type* type_);
		static std::string load_register(int width_);
		static std::string get_load_instruction(int width_);
		static std::string get_destination(int width_);
		static std::string get_source(int width_,bool is_float_);
		

		int push(const std::string& register_);
		int push(type* type_);
		int pop(const std::string& register_);
		void save(bool is_float_);

		virtual void visit(ast_node* ast_node_) const { return ast_node_->accept(this); }
		virtual void visit_binary_operation(binary_expression* binary_expression_);
		virtual void visit_jump_statement(jump_statement* jump_statement_);
		virtual void visit_if_statement(if_statement* if_statement_);
		virtual void visit_conditional_expression(conditional_expression* conditional_expression_);
		virtual void visit_identifier(identifier* identifier_);
		void visit_expression(expression* expression_) { expression_->accept(this); }

		void generate_comma_operator(binary_expression* comma_);
		void generate_member_reference_operator(binary_expression* binary_operator_);
		void generate_comparation_operator(int width_, const char* instruction_, bool is_float_);
		void generate_or_operator(binary_expression* or_opetator_);
		void generate_and_operator(binary_expression* and_operator_);
		void generate_mul_operator(int width, bool sign,bool is_float);
		void generate_assignment_operator(binary_expression* binary_expression_);
		void generate_div_operator(int width, bool sign, int operation_,bool is_float_);
		void generate_comparation_zero(type* type_);
		void generate_minus_operator(unary_expression* minus_operator_);
		void generate_cast_operator(unary_expression* cast_operator_);
	private:
		static int stack_position_;
		static FILE* output_file_;
	};
	
}
#endif
