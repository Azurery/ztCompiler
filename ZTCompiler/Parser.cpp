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
		std::cerr << "%s is unexcepted " << tok << std::endl;
	}

	/*(6.5.3) unary-expression:
					postfix-expression
					++ unary-expression
					-- unary-expression
					unary-operator cast-expression
					sizeof unary-expression
					sizeof ( type-name )
					//_Alignof ( type-name )
	(6.5.3) unary-operator: one of
					& * + - ~ !
	*/
	expression* parser::parse_unary_expression() {
		auto token_ = tokens_.test_next_token();
		switch (token_->type_attr) {
		case static_cast<TokenAttr>('&'):
			
		}
	}

	/*(6.5.4) cast-expression:
					unary-expression
					( type-name ) cast-expression
	*/
	expression* parser::parse_cast_expression() {
		auto token_ = tokens_.test_next_token();
		if (token_->type_attr ==static_cast<TokenAttr>('(')&&is_type(tokens_.test_next_token())) {
			auto type_ = parse_typedef_name();
			tokens_.consume_next_token();
			
		}
	}

	unary_operation* parser::parse_prefix_inc_dec(const token* token_, expression* operator_) {
		auto type_ = token_->type_attr;
		assert(type_ == TokenAttr::PREFIX_INC || type_ == TokenAttr::PREFIX_DEC);

		if (type_ == TokenAttr::INC)
			type_ = TokenAttr::PREFIX_INC;
		else
			type_ = TokenAttr::PREFIX_DEC;
	}

	unary_operation* parser::parse_postfix_inc_dec(const token* token_,expression* operator_) {
		auto type_ = token_->type_attr;
		if (type_ == TokenAttr::INC)
			type_ = TokenAttr::POSTFIX_INC;
		else type_ = TokenAttr::POSTFIX_DEC;
		return unary_operation::create(static_cast<int>(type_), operator_);
	}


	/*(6.5.2) postfix-expression:
				primary-expression
				postfix-expression [ expression ]
				postfix-expression ( argument-expression-listopt )
				postfix-expression . identifier
				postfix-expression -> identifier
				postfix-expression ++
				postfix-expression --
				//( type-name ) { initializer-list }
				//( type-name ) { initializer-list , }
	(6.5.2) argument-expression-list:
				assignment-expression
				argument-expression-list , assignment-expression
	*/
	expression* parser::parse_postfix_expression(expression* expression_) {
		while (true) {
			auto token_ = tokens_.consume_next_token();
			switch (token_->type_attr) {
			case static_cast<TokenAttr>('[') : {
				auto rhs_ = parse_expression();
				auto tok_ = tokens_.consume_next_token();
				tokens_.expect(']');
				auto operator_ = binary_operation::create(token_, '+', expression_, rhs_);
				expression_ = unary_operation::create(static_cast<int>(TokenAttr::DEREF), operator_);
				break;
			}
			case static_cast<TokenAttr>('(') :
				//expression_ = parse_function_type(expression_);
				break;
			case static_cast<TokenAttr>('.') : {
				auto member_name_ = token_->str_;
				tokens_.expect(static_cast<int>(TokenAttr::IDENTIFIER));
				auto struct_union_type_ = expression_->type_value();
				if (struct_union_type_ == nullptr)
					std::cerr << "应输入struct/union" << std::endl;
				auto rhs_ = struct_union_type_;
				std::cerr << token_ << "%s不是%s的成员变量" << member_name_.c_str() << std::endl;
				//expression_=binary_operation::create(token_, '.', expression_, rhs_);
				break;
			}
			case TokenAttr::POINTER:
				expression_ = unary_operation::create(static_cast<int>(TokenAttr::POINTER), expression_);
				break;
			case TokenAttr::INC: case TokenAttr::DEC:
				expression_ = parse_postfix_inc_dec(token_, expression_);
				break;
			default:
				expression_ = parse_primary_expression();
				return expression_;
			}
		}
	}
	qualifier_type* parser::parse_typedef_name() {
		auto type_ = parse_declaration_specifier();
		return type_;
	}


	qualifier_type* parser::parse_declaration_specifier() {
		qualifier_type qualifier_type_();
		int storage_class_specifier = 0;
		int function_specifier = 0;
		int alignment_specifier = 0;
		int qualifer_specifier = 0;
		int type_specifier = 0;
		while (true) {
			const token* token_ = tokens_.test_next_token();
			switch (token_->type_attr) {
			//function specifier
			case TokenAttr::INLINE:
				function_specifier = static_cast<int>(function_type::Type_function_specifier::INLINE);
				break;
			case TokenAttr::NORETURN:
				function_specifier = static_cast<int>(function_type::Type_function_specifier::NORETURN);
				break;
			//TODO alignment specifier

			//storage class specifier
			case TokenAttr::TYPEDEF:
				break;
			case TokenAttr::STATIC:
				storage_class_specifier = static_cast<int>(type::Storage_class_specifier::STATIC);
				break;
			case TokenAttr::EXTERN:
				break;
			case TokenAttr::THREAD_LOCAL:
				break;
			case TokenAttr::REGISTER:
				break;
			case TokenAttr::AUTO:
				break;
			//type qualifier
			case TokenAttr::CONST:
				break;
				//qualifer_specifier=
		}
	}


}