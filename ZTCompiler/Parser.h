#pragma once
#ifndef _ZT_PARSE_H_
#define _ZT_PARSE_H_

#include "AST.h"
#include "Token.h"
#include "Scanner.h"

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <stack>
namespace ztCompiler {
	class parser {
	public:
		explicit parser(const tokens* tokens_) {}
		constant* parse_constant(const token* token_);
		constant* parse_float(const token* token_);
		constant* parse_integer(const token* token_);
		constant* parse_character(const token* token_);
		constant* parse_literal(std::string& str, const token* token_);
		expression* parse_generic();

		void parse();
		void parse_translate_unit();


		function_type* parse_function_type(expression* expression_);
		expression* parse_conditional_expression();
		expression* parse_assignment_expression();
		expression* parse_expression();
		expression* parse_primary_expression();
		expression* parse_unary_expression();
		expression* parse_cast_expression();
		expression* parse_postfix_expression(expression* expression_);
		unary_operation* parse_prefix_inc_dec(const token* token_, expression* operator_);
		unary_operation* parse_postfix_inc_dec(const token* token_,expression* operator_);


		qualifier_type* parse_typedef_name();
		//qualifier_type* parse_declaration_specifier(int* storage_class_specifier_, int* function_specifier, int* alignment_specifier);
		qualifier_type* parse_declaration_specifier();
	private:
		bool is_type(const token* token_) {
			if (token_->is_type_qualifier() || token_->is_type_specifier()) {
				return true;
			}
			return false;
		}

	private:
		translate_unit* translate_unit_;	//	AST的根结点
		token_collection tokens_;
		environment* enviroment_;
	};

}
#endif
