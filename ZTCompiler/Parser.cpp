#include "Parser.h"

namespace ztCompiler {

	constant* parser::parse_float(const token* token_) {
		const auto& c = token_->str_;
		size_t end = 0;
		double value = 0.0;
		//先将字符串转换为都变了类型
		try {
			value = stod(c, &end);
		}
		catch (const std::out_of_range& out_err) {
			std::cerr << "float越界" << std::endl;
		}

		int flag = static_cast<int>(TokenAttr::DOUBLE_CONSTANT);
		if (c[end] == 'f' || c[end] == 'F') {
			flag = static_cast<int>(TokenAttr::FLOAT_CONSTANT);
			++end;
		}
		else if (c[end] == 'l' || c[end] == 'L') {
			flag = static_cast<int>(TokenAttr::INTEGER_CONSTANT) | static_cast<int>(TokenAttr::DOUBLE_CONSTANT);
			++end;
		}
		if (c[end] != 0)
			std::cerr << "无效的后缀" << std::endl;
		return constant::create(token_, static_cast<int>(flag), value);
	}

	constant* parser::parse_character(const token* token_) {
		int value;
		auto c = scanner(token_).scan_character(value);

		int flag;
		switch (c) {
		case scanner::Character_encoding::NONE:
			value = static_cast<char>(value);
			flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::INT);
			break;
		case scanner::Character_encoding::CHAR_16:
			value = static_cast<char>(value);
			flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED) |
				static_cast<int>(arithmetic_type::Type_arithmetic_specifier::SHORT);
			break;
		case scanner::Character_encoding::WCHAR:
		case scanner::Character_encoding::CHAR_32:
			flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED) |
				static_cast<int>(arithmetic_type::Type_arithmetic_specifier::INT);
			break;
		default:
			assert(false);
		}
		return constant::create(token_, flag, static_cast<long>(value));
	}

	/*
	(6.4.4.1) integer-suffix:
				unsigned-suffix long-suffixopt
				unsigned-suffix long-long-suffix
				long-suffix unsigned-suffixopt
				long-long-suffix unsigned-suffixopt
	(6.4.4.1) unsigned-suffix: one of
				u U
	(6.4.4.1) long-suffix: one of
				l L
	(6.4.4.1) long-long-suffix: one of
				ll LL
*/
	constant* parser::parse_integer(const token* token_) {
		const auto& c = token_->str_;
		long value;
		size_t end = 0;
		try {
			value = stoll(c);
		}catch (const std::out_of_range& out_err) {
			std::cerr << "integer越界" << std::endl;
		}
		int flag = 0;
		for (; c[end]; end++) {
			if (c[end] == 'u' || c[end] == 'U') {
				if (flag != static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED))
					std::cerr << "无效的后缀" << std::endl;
				flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED);
			}else {
				if (flag != static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG) ||
					flag != static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG_LONG))
					std::cerr << "无效的后缀" << std::endl;
				if (c[end++] == 'l' || c[end++] == 'L') {
					flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG_LONG);
					++end;
				}else {
					flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG);
				}	
			}
		}

		if (c[0] > '0'&&c[0] < '9') {
			switch (flag) {
			case static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG):
				break;
			case static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED) :
				;
			}
		}else {

		}
		return constant::create(token_, flag, value);
	}

	constant* parser::parse_constant(const token* token_) {
		assert(token_->is_constant());

		if (token_->type_attr == TokenAttr::INTEGER_CONSTANT)
			return parse_integer(token_);
		else if (token_->type_attr == TokenAttr::CHARACTER_CONSTANT)
			return parse_character(token_);
		else
			return parse_float(token_);
	}


	/*(6.5.15) conditional-expression:
					logical-OR-expression
					logical-OR-expression ? expression : conditional-expression
*/
	expression* parser::parse_conditional_expression() {

	}


	/*(6.5.16) assignment-expression:
					conditional-expression
					unary-expression assignment-operator assignment-expression
	*/
	expression* parser::parse_assignment_expression() {

	}



	/*
parse_expression ()
	return parse_expression_1 (parse_primary (), 0)
parse_expression_1 (lhs, min_precedence)
    lookahead := peek next token
    while lookahead is a binary operator whose precedence is >= min_precedence
        op := lookahead
        advance to next token
        rhs := parse_primary ()
        lookahead := peek next token
        while lookahead is a binary operator whose precedence is greater
                 than op's, or a right-associative operator
                 whose precedence is equal to op's
            rhs := parse_expression_1 (rhs, lookahead's precedence)
            lookahead := peek next token
        lhs := the result of applying op with operands lhs and rhs
    return lhs
	*/
	expression* parser::parse_expression() {
		
	}
	/*(6.5.1) primary-expression:
					identifier
					constant
					string-literal
					( expression )
					generic-selection
	(6.5.1.1) generic-selection:
					_Generic ( assignment-expression , generic-assoc-list )
	(6.5.1.1) generic-assoc-list:
					generic-association
					generic-assoc-list , generic-association
	(6.5.1.1) generic-association:
					type-name : assignment-expression
					default : assignment-expression*/
	expression* parser::parse_primary_expression() {
		if (tokens_.empty()) {
			std::cerr << "输如结束过早" << std::endl;
		}
		auto tok = tokens_.test_next_token();
		if (tok->type_attr == static_cast<TokenAttr>('(')) {
			auto expression_ = parse_expression();
			tokens_.consume_next_token();
			return expression_;
		}

		if (tok->is_identifier()) {
		
		}else if (tok->is_constant()) {
			return parse_constant(tok);
		}else if (tok->is_literal()) {
			return;
		}else if (tok->type_attr == TokenAttr::GENERIC) {
			return parse_generic();
		}
	}

	unary_operation* parser::parse_postfix_inc_dec(const token* token_,expression* operator_) {
		auto type_ = token_->type_attr;
		if (type_ == TokenAttr::INC)
			type_ = TokenAttr::POSTFIX_INC;
		else type_ = TokenAttr::POSTFIX_DEC;
		return unary_operation::create(static_cast<int>(type_), operator_);
	}


}