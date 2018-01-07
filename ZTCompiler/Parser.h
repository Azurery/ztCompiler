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
		explicit parser(const tokens* tokens_) {

		}
		constant* parse_constant(const token* token_);
		constant* parse_float(const token* token_);
		constant* parse_integer(const token* token_);
		constant* parse_character(const token* token_);
		constant* parse_literal(std::string& str, const token* token_);
		expression* parse_generic();

		void parse();
		void parse_translate_unit();


		expression* parse_conditional_expression();
		expression* parse_assignment_expression();
		expression* parse_expression();
		expression* parse_primary_expression();
		unary_operation* parse_postfix_inc_dec(const token* token_,expression* operator_);

	private:
		bool is_type(const token* token) {
			if (token->is_type_qualifier() || token->is_type_specifier()) {
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
