#ifndef _ZT_PARSE_H_
#define _ZT_PARSE_H_

#include "Token.h"
#include "Scanner.h"
#include "Type.h"
#include "AST.h"
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <stack>

namespace ztCompiler {
	class type;
	class arithmetic_type;
	class qualifier_type;
	class constant;
	class function_type;
	class expression;
	class unary_expression;
	class compound_statement;
	class statement;
	class if_statement;
	class jump_statement;
	class return_statement;
	class translate_unit;
	class parser {
	public:
		explicit parser(const tokens* tokens_);
		constant* parse_constant(const token* token_);
		constant* parse_float(const token* token_);
		constant* parse_integer(const token* token_);
		constant* parse_character(const token* token_);
		constant* parse_string_literal( const token* token_);
		expression* parse_generic();

		void parse();
		void parse_translate_unit();


		//parse expression
		function_type* parse_function_type(expression* expression_);
		expression* parse_assignment_expression();
		expression* parse_expression();
		expression* parse_primary_expression();
		expression* parse_unary_expression();
		expression* parse_cast_expression();
		expression* parse_postfix_expression_helper(expression* expression_);
		expression* parse_postfix_expression();
		unary_expression* parse_prefix_inc_dec(const token* token_);
		unary_expression* parse_postfix_inc_dec(const token* token_,expression* operator_);
		expression* parse_multiplicative_expression();
		expression* parse_additive_expression();
		expression* parse_shift_expression();
		expression* parse_relational_expression();
		expression* parse_equality_expression();
		expression* parse_and_expression();
		expression* parse_exclusive_or_expression();
		expression* parse_inclusive_or_expression();
		expression* parse_logical_and_expression();
		expression* parse_logical_or_expression();
		expression* parse_conditional_expression();

		//parse declaration
		void parse_static_assert_declaration();
		qualifier_type parse_pointer(qualifier_type pointer_to_);
		qualifier_type* parse_typedef_name();
		int parse_qualifier();
		qualifier_type* parse_declaration_specifier();
		type* parse_enumerator(arithmetic_type* arithemetic_type_);
		compound_statement* parse_declaration();

		//parse statment
		statement* parse_statement();
		compound_statement* parse_compound_statement();
		if_statement* parse_if_statement();
		compound_statement* parse_for_statement();
		compound_statement* parse_while_statement();
		compound_statement* parse_do_statement();
		compound_statement* parse_switch_statement();
		compound_statement* parse_cast_statement();
		compound_statement* parse_default_statement();
		jump_statement* parse_continue_statement();
		jump_statement* parse_break_statement();
		return_statement* parse_return_statement();
		jump_statement* parse_goto_statement();

	
	private:
		bool is_type(const token* token_) {
			if (token_->is_type_qualifier() || token_->is_type_specifier()) {
				return true;
			}
			return false;
		}

	private:
		translate_unit* translate_unit_;	//	AST的根结点
		token_sequence tokens_;
		environment* enviroment_;
	};
}

#endif 