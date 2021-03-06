#include "Parser.h"
using namespace ztCompiler;

	constant* parser::parse_float(const token* token_) {
		const auto& c = token_->get_token_name();
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
		int value = 0;
		auto ch = scanner(token_).detect_encode(*((token_->get_token_name()).substr(0, 1).c_str()));
		int flag;
		switch (ch) {
		case scanner::character_encode::NONE:
			value = static_cast<char>(value);
			flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::INT);
			break;
		case scanner::character_encode::CHAR_16:
			value = static_cast<char>(value);
			flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED) |
				static_cast<int>(arithmetic_type::Type_arithmetic_specifier::SHORT);
			break;
		case scanner::character_encode::WCHAR:
		case scanner::character_encode::CHAR_32:
			flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED) |
				static_cast<int>(arithmetic_type::Type_arithmetic_specifier::INT);
			break;
		default:
			assert(false);
		}
		return constant::create(token_, flag, static_cast<long>(value));
	}

// 	constant* parser::parse_string_literal(const token* token_) {
// 		
// 	}

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
		const auto& c = token_->get_token_name();
		long value;
		size_t end = 0;
		try {
			value = stoll(c);
		}
		catch (const std::out_of_range& out_err) {
			std::cerr << "integer越界" << std::endl;
		}
		int flag = 0;
		for (; c[end]; end++) {
			if (c[end] == 'u' || c[end] == 'U') {
				if (flag != static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED))
					std::cerr << "无效的后缀" << std::endl;
				flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED);
			}
			else {
				if (flag != static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG) ||
					flag != static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG_LONG))
					std::cerr << "无效的后缀" << std::endl;
				if (c[end++] == 'l' || c[end++] == 'L') {
					flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG_LONG);
					++end;
				}
				else {
					flag = static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG);
				}
			}
		}

		if (c[0] > '0'&&c[0] < '9') {
			switch (flag) {
			case static_cast<int>(arithmetic_type::Type_arithmetic_specifier::LONG) :
				break;
			case static_cast<int>(arithmetic_type::Type_arithmetic_specifier::UNSIGNED) :
				;
			}
		}
		else {

		}
		return constant::create(token_, flag, value);
	}

	constant* parser::parse_constant(const token* token_) {
		assert(token_->is_constant());

		if (token_->get_token_attr() == TokenAttr::INTEGER_CONSTANT)
			return parse_integer(token_);
		else if (token_->get_token_attr() == TokenAttr::CHARACTER_CONSTANT)
			return parse_character(token_);
		else
			return parse_float(token_);
	}


	/*
	(6.5.17) expression:
				assignment-expression
				expression , assignment-expression
	*/
	expression* parser::parse_expression() {
		auto token_ = tokens_.consume_next_token();
		auto lhs_ = parse_assignment_expression();
		while (tokens_.test_next_token()->get_token_attr() ==static_cast<TokenAttr>( ',')) {
			auto rhs_ = parse_assignment_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.consume_next_token();
		}
		return lhs_;
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
		if (tok->get_token_attr() == static_cast<TokenAttr>('(')) {
			auto expression_ = parse_expression();
			tokens_.consume_next_token();
			return expression_;
		}

		if (tok->is_identifier()) {

		}
		else if (tok->is_constant()) {
			//return parse_constant(tok);
		}
		else if (tok->is_literal()) {
			//return;
		}
// 		else if (tok->get_token_attr() == TokenAttr::GENERIC) {
// 			return parse_generic();
// 		}
		std::cerr << "%s is unexcepted " << tok << std::endl;
	}

	/*(6.5.3) unary-expression:
					postfix-expression
					++ unary-expression
					-- unary-expression
					unary-operator cast-expression
					//sizeof unary-expression
					//sizeof ( type-name )
					//_Alignof ( type-name )
	(6.5.3) unary-operator: one of
					& * + - ~ !
	*/
	expression* parser::parse_unary_expression() {
		auto token_ = tokens_.test_next_token();
		switch (token_->get_token_attr()) {
		case static_cast<TokenAttr>('&') : {
			auto operator_ = parse_cast_expression();
			return unary_expression::create(static_cast<int>(TokenAttr::ADDRDESS), operator_);
		}
		case static_cast<TokenAttr>('*') : {
			auto operator_ = parse_cast_expression();
			return unary_expression::create(static_cast<int>(TokenAttr::POINTER), operator_);
		}

		case static_cast<TokenAttr>('~') : {
			auto operator_ = parse_cast_expression();
			return unary_expression::create('~', operator_);
		}
		case static_cast<TokenAttr>('!') : {
			auto operator_ = parse_cast_expression();
			return unary_expression::create('!', operator_);
		}
		case static_cast<TokenAttr>('+') : {
			auto operator_ = parse_cast_expression();
			return unary_expression::create(static_cast<int>(TokenAttr::PLUS), operator_);
		}
		case static_cast<TokenAttr>('-') : {
			auto operator_ = parse_cast_expression();
			return unary_expression::create(static_cast<int>(TokenAttr::SUB), operator_);
		}
		case TokenAttr::INC:
			return parse_prefix_inc_dec(token_);
		case TokenAttr::DEC:
			return parse_prefix_inc_dec(token_);
		default:
			return parse_postfix_expression();
		}
	}

	/*(6.5.4) cast-expression:
					unary-expression
					( type-name ) cast-expression
	*/
	expression* parser::parse_cast_expression() {
		auto token_ = tokens_.test_next_token();
		if (token_->get_token_attr() == static_cast<TokenAttr>('(') && is_type(tokens_.test_next_token())) {
			auto ty= parse_typedef_name();
			tokens_.consume_next_token();
			auto operand = parse_cast_expression();
			return unary_expression::create(static_cast<int>(TokenAttr::CAST), operand, ty);
		}
		return parse_unary_expression();
	}

	/*(6.5.5) multiplicative-expression:
				cast-expression
				multiplicative-expression * cast-expression
				multiplicative-expression / cast-expression
				multiplicative-expression % cast-expression
	*/
	expression* parser::parse_multiplicative_expression() {
		auto lhs_ = parse_cast_expression();
		auto token_ = tokens_.consume_next_token();
		while (token_->get_token_attr() == static_cast<TokenAttr>('*')
			|| token_->get_token_attr() == static_cast<TokenAttr>('/')
			|| token_->get_token_attr() == static_cast<TokenAttr>('%')) {
			auto rhs_ = parse_cast_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.consume_next_token();
		}
		return lhs_;
	}

	/*(6.5.6) additive-expression:
				multiplicative-expression
				additive-expression + multiplicative-expression
				additive-expression - multiplicative-expressi
	*/
	expression* parser::parse_additive_expression() {
		auto lhs_ = parse_multiplicative_expression();
		auto token_ = tokens_.consume_next_token();
		while (token_->get_token_attr() == static_cast<TokenAttr>('+')
			|| token_->get_token_attr() == static_cast<TokenAttr>('-')) {
			auto rhs_ = parse_multiplicative_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.consume_next_token();
		}

		return lhs_;
	}

	/*(6.5.7) shift-expression:
		additive-expression
		shift-expression << additive-expression
		shift-expression >> additive-expression
	*/
	expression* parser::parse_shift_expression() {
		auto token_ = tokens_.consume_next_token();
		auto lhs_ = parse_additive_expression();
		while (token_->get_token_attr() == TokenAttr::LEFT_SHIFT || token_->get_token_attr() == TokenAttr::RIGHT_SHIFT) {
			auto rhs_ = parse_additive_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.consume_next_token();
		}
		return lhs_;
	}

	/*(6.5.8) relational-expression:
					shift-expression
					relational-expression < shift-expression
					relational-expression > shift-expression
					relational-expression <= shift-expression
					relational-expression >= shift-expression
	*/

	expression* parser::parse_relational_expression() {
		auto token_ = tokens_.consume_next_token();
		auto lhs_ = parse_shift_expression();
		while (token_->get_token_attr() == TokenAttr::LESS
			|| token_->get_token_attr() == TokenAttr::GREATER
			|| token_->get_token_attr() == TokenAttr::LESS_EQUAL
			|| token_->get_token_attr() == TokenAttr::GREATER_EQUAL) {
			auto rhs_ = parse_shift_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.consume_next_token();
		}
		return lhs_;
	}


	/*(6.5.9) equality-expression:
					relational-expression
					equality-expression == relational-expression
					equality-expression != relational-expression
	*/
	expression* parser::parse_equality_expression() {
		auto token_ = tokens_.consume_next_token();
		auto lhs_ = parse_relational_expression();
		while (token_->get_token_attr() == TokenAttr::EQUAL
			|| token_->get_token_attr() == TokenAttr::NOT_EQUAL) {
			auto rhs_ = parse_relational_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.consume_next_token();
		}
		return lhs_;
	}

	/*6.5.10) AND-expression:
				equality-expression
				AND-expression & equality-expression
	*/
	expression* parser::parse_and_expression() {
		auto lhs_ = parse_equality_expression();
		auto token_ = tokens_.test_next_token();
		while (tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>('&')) {
			auto rhs_ = parse_equality_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.test_next_token();
		}
		return lhs_;
	}

	/*(6.5.11) exclusive-OR-expression:
					AND-expression
					exclusive-OR-expression ^ AND-expression
	*/
	expression* parser::parse_exclusive_or_expression() {
		auto lhs_ = parse_and_expression();
		auto token_ = tokens_.test_next_token();
		while (tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>('^')) {
			auto rhs_ = parse_equality_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.test_next_token();
		}
		return lhs_;
	}

	/*(6.5.12) inclusive-OR-expression:
					exclusive-OR-expression
					inclusive-OR-expression | exclusive-OR-expression
	*/
	expression* parser::parse_inclusive_or_expression() {
		auto lhs_ = parse_exclusive_or_expression();
		auto token_ = tokens_.test_next_token();
		while (tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>('|')) {
			auto rhs_ = parse_equality_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.test_next_token();
		}
		return lhs_;
	}

	/*(6.5.13) logical-AND-expression:
					inclusive-OR-expression
					logical-AND-expression && inclusive-OR-expression
	*/
	expression* parser::parse_logical_and_expression() {
		auto lhs_ = parse_inclusive_or_expression();
		auto token_ = tokens_.test_next_token();
		while (tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>('&&')) {
			auto rhs_ = parse_equality_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.test_next_token();
		}
		return lhs_;
	}

	/*(6.5.14) logical-OR-expression:
					logical-AND-expression
					logical-OR-expression || logical-AND-expression
	*/
	expression* parser::parse_logical_or_expression() {
		auto lhs_ = parse_logical_and_expression();
		auto token_ = tokens_.test_next_token();
		while (tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>('&&')) {
			auto rhs_ = parse_equality_expression();
			lhs_ = binary_expression::create(token_, lhs_, rhs_);
			token_ = tokens_.test_next_token();
		}
		return lhs_;
	}

	/*(6.5.15) conditional-expression:
					logical-OR-expression
					logical-OR-expression ? expression : conditional-expression
	*/
	expression* parser::parse_conditional_expression() {
		auto token_ = tokens_.test_next_token();
		auto condition_expression_ = parse_logical_or_expression();
		if (tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>('?')) {
			expression* condition_true_;
			if (tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>(':')) {
				condition_true_ = condition_expression_;
			}else {
				condition_expression_ = parse_expression();
			}
			auto condition_false_ = parse_conditional_expression();
			return conditional_expression::create(token_, condition_expression_, condition_true_, condition_false_);
			token_ = tokens_.consume_next_token();
		}
		return condition_expression_;
	}

	/*(6.5.16) assignment-expression:
					conditional-expression
					unary-expression assignment-operator assignment-expression
	 (6.5.16) assignment-operator: one of
					=  *=  /=  %=  +=  -=  <<=  >>=  &=  ^=  |=
	*/
	expression* parser::parse_assignment_expression() {
		expression* lhs_ = parse_conditional_expression();
		expression* rhs_;
		auto token_ = tokens_.consume_next_token();
		switch (token_->get_token_attr()) {
		case static_cast<TokenAttr>('=') :
			rhs_ = parse_assignment_expression();
			break;
		case TokenAttr::MUL_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, '*', lhs_, rhs_);
			break;
		case TokenAttr::DIV_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, '/', lhs_, rhs_);
			break;
		case TokenAttr::MOD_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, '%', lhs_, rhs_);
			break;
		case TokenAttr::ADD_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, '+', lhs_, rhs_);
			break;
		case TokenAttr::SUB_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, '-', lhs_, rhs_);
			break;
		case TokenAttr::LEFT_SHIFT_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, static_cast<int>(TokenAttr::LEFT_SHIFT), lhs_, rhs_);
			break;
		case TokenAttr::RIGHT_SHIFT_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, static_cast<int>(TokenAttr::RIGHT_SHIFT), lhs_, rhs_);
			break;
		case TokenAttr::AND_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, '&', lhs_, rhs_);
			break;
		case TokenAttr::EXCLUSIVE_OR_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, '^', lhs_, rhs_);
			break;
		case TokenAttr::INCLUSIVE_OR_ASSIGN:
			rhs_ = parse_assignment_expression();
			rhs_ = binary_expression::create(token_, '|', lhs_, rhs_);
			break;
		default:
			return lhs_;
		}
		return binary_expression::create(token_, lhs_, rhs_);
	}



	unary_expression* parser::parse_prefix_inc_dec(const token* token_) {
		auto type_ = token_->get_token_attr();
		assert(type_ == TokenAttr::PREFIX_INC || type_ == TokenAttr::PREFIX_DEC);

		if (type_ == TokenAttr::INC)
			type_ = TokenAttr::PREFIX_INC;
		else
			type_ = TokenAttr::PREFIX_DEC;
	}

	unary_expression* parser::parse_postfix_inc_dec(const token* token_, expression* operator_) {
		auto type_ = token_->get_token_attr();
		if (type_ == TokenAttr::INC)
			type_ = TokenAttr::POSTFIX_INC;
		else type_ = TokenAttr::POSTFIX_DEC;
		return unary_expression::create(static_cast<int>(type_), operator_);
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
	expression* parser::parse_postfix_expression_helper(expression* expression_) {
		while (true) {
			auto token_ = tokens_.consume_next_token();
			switch (token_->get_token_attr()) {
			case static_cast<TokenAttr>('[') : {
				auto rhs_ = parse_expression();
				auto tok_ = tokens_.consume_next_token();
				tokens_.expect(']');
				auto operator_ = binary_expression::create(token_, '+', expression_, rhs_);
				expression_ = unary_expression::create(static_cast<int>(TokenAttr::DEREFERENCE), operator_);
				break;
			}
			case static_cast<TokenAttr>('(') :
				//expression_ = parse_function_type(expression_);
				break;
			case static_cast<TokenAttr>('.') : {
				auto member_name_ = token_->get_token_name();
				tokens_.expect(static_cast<int>(TokenAttr::IDENTIFIER));
				auto struct_union_type_ = expression_->type();
				if (struct_union_type_ == nullptr)
					std::cerr << "应输入struct/union" << std::endl;
				auto rhs_ = struct_union_type_;
				std::cerr << token_ << "%s不是%s的成员变量" << member_name_.c_str() << std::endl;
				//expression_=binary_operation::create(token_, '.', expression_, rhs_);
				break;
			}
			case TokenAttr::POINTER:
				expression_ = unary_expression::create(static_cast<int>(TokenAttr::POINTER), expression_);
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


	//parse declaration

	/*(6.7.10) static_assert-declaration:
				_Static_assert ( constant-expression , string-literal ) ;
	*/
	void parser::parse_static_assert_declaration() {
		tokens_.expect('(');	
		/*(6.6) constant-expression:
					conditional-expression
		*/
		auto constant_expression_ = parse_conditional_expression();
		tokens_.expect(',');
		//auto string_literal_ = parse_string_literal();
		tokens_.expect(')');
		tokens_.expect(';');
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
			switch (token_->get_token_attr()) {
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
			case TokenAttr::SIGNED:
				if(type_specifier)
				break;
				//qualifer_specifier=
			}
		}
	}

	/*(6.7.2.2) enumerator:
					enumeration-constant
					enumeration-constant = constant-expression
	*/
	type* parser::parse_enumerator(arithmetic_type* arithmetic_type_) {
		//arithmetic_type_不为空并且是Integer类型
		assert(arithmetic_type_&&arithmetic_type_->is_integer());

		auto token_ = tokens_.expect(static_cast<int>(TokenAttr::IDENTIFIER));
		if (tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>('=')) {
			auto expression_ = parse_assignment_expression();

		}
	}

	int parser::parse_qualifier() {
		int qualifier_ = 0;
		while (true) {
			auto token_ = tokens_.consume_next_token();
			switch (token_->get_token_attr()) {
			case TokenAttr::CONST:
				qualifier_ |= static_cast<int>(type::Type_qualifier::CONST);
			case TokenAttr::VOLATILE:
				qualifier_ |= static_cast<int>(type::Type_qualifier::VOLATILE);
			case TokenAttr::ATOMIC:
				qualifier_ |= static_cast<int>(type::Type_qualifier::ATOMIC);
			case TokenAttr::RESTRICT:
				qualifier_ |= static_cast<int>(type::Type_qualifier::RESTRICT);
			default:;
				return qualifier_;
			}
		}
	}

	//e.g. int** int*
	qualifier_type parser::parse_pointer(qualifier_type pointed_to_) {
		while (tokens_.expect('*')) {
			auto type_ = pointer_type::create(pointed_to_);
			pointed_to_ = qualifier_type(type_);
		}
		return pointed_to_;
	}

	compound_statement* parser::parse_declaration() {
		std::list<statement*> statements_;
		if (tokens_.test_next_token()->get_token_attr() == TokenAttr::STATIC_ASSERT) {
			parse_static_assert_declaration();
		}else {
			//auto type_ = parse_declaration_specifier();
			////如果没有遇见；说明该声明认为结束
			//if (!(tokens_.test_next_token()->get_token_attr() == static_cast<TokenAttr>(';')))
			//	while (tokens_.consume_next_token()->get_token_attr() == static_cast<TokenAttr>(','))
					//TODO:intializer value
					//e.g. int i=0;
					//for(;i<n;i++)
					//auto direct_declarator_ = parse_direct_declarator();
		}
	}


	//parse statement


	//compound - statement:
	//		{ block - item - listopt }
	//block - item - list :
	//		block - item
	//		block - item - list block - item
	//	block - item :
	//		declaration
	//		statement
	compound_statement* parser::parse_compound_statement() {
		std::list<statement*> statements_;
		
		tokens_.expect('{');
		auto token_ = tokens_.consume_next_token();
		while (token_->get_token_attr() != static_cast<TokenAttr>('}')) {
			if (token_->is_type_specifier()) {
				auto declaration_ = parse_declaration();
				statements_.push_back(declaration_);
			}else {
				auto statement_ = parse_statement();
				statements_.push_back(statement_);
			}
		}

		return compound_statement::create(statements_);
	}


	//if (expression) statement
	//if (expression) statement else statement
	if_statement* parser::parse_if_statement() {
		tokens_.expect('(');
		auto token_ = tokens_.consume_next_token();
		auto condition_ = parse_expression();
		//condition_的类型必须是scalar类型（算数类型和指针类型）
		if (!condition_->type()->is_scalar()) {
			std::cerr << "expect scalar" << std::endl;
		}
		tokens_.expect(')');

		auto then_ = parse_statement();
		statement* else_ = nullptr;
		//如果存在else语句，则parse else语句
		if (tokens_.test_next_token()->get_token_attr() == TokenAttr::ELSE)
			else_ = parse_statement();
		return if_statement::create(condition_, then_, else_);

	}

	//for循环结构
	//		for (expressionopt; expressionopt; expressionopt) statement
	//		for (declaration ; test_expression; update_expression) statement
	//展开后的结构：
	//	declaration;
	//t = test_expression;
	//if (!t)
	//	goto done;
	//loop:
	//	body_statement;
	//	update_statement;
	//	t = test_expression;
	//	if (t)
	//		goto loop;
	//done:
	compound_statement* parser::parse_for_statement() {
		tokens_.expect('(');
		//用于存储for语句（）中的复合语句
		std::list<statement*> statement_parameter_list;
		auto type_ = tokens_.consume_next_token();
		//parse for语句的declaration
		if (type_->is_type_specifier()) {
			statement_parameter_list.push_back(parse_declaration());
		}else if (tokens_.consume_next_token()->get_token_attr() != static_cast<TokenAttr>(';')) {
			auto statement_ = parse_statement();
			statement_parameter_list.push_back(statement_);
			tokens_.expect(';');
		}
		//parse condition expression
		expression* test_expression_ = nullptr;
		if (tokens_.consume_next_token()->get_token_attr() != static_cast<TokenAttr>(';')) {
			test_expression_ = parse_expression();
			tokens_.expect(';');
		}

		expression* update_expression_ = nullptr;
		if (tokens_.consume_next_token()->get_token_attr() != static_cast<TokenAttr>(')')) {
			update_expression_ = parse_expression();
			tokens_.expect(')');
		 }

		auto test_label_ = labeled_statement::create();
		auto loop_label_ = labeled_statement::create();
		auto done_label_ = labeled_statement::create();
		statement_parameter_list.push_back(test_label_);
		if (!test_expression_) {
			auto goto_done_statement_ = jump_statement::create(done_label_);
			statement_parameter_list.push_back(goto_done_statement_);
		}

		statement* body_statement_ = parse_statement();
		statement_parameter_list.push_back(body_statement_);
		statement_parameter_list.push_back(loop_label_);

		if (test_expression_) {
			statement_parameter_list.push_back(update_expression_);
		}
		auto goto_loop_statement_ = jump_statement::create(loop_label_);
		statement_parameter_list.push_back(goto_loop_statement_);
		statement_parameter_list.push_back(done_label_);

		return compound_statement::create(statement_parameter_list);

	}

	//while循环结构：
	//while (expression) statement
	//展开之后：
	//			t=test_expressoin
	//condition: if(!test_expression) 
	//				goto done
	//loop:
	//			body_statement
	//			t=test_expression
	//			if(t)
	//			goto loop
	//done:
	
	compound_statement* parser::parse_while_statement() {
		std::list<statement*> statement_parameter_list;
		tokens_.expect('(');

		auto token_ = tokens_.consume_next_token();
		auto test_expression_ = parse_expression();
		tokens_.expect(')');

		if (test_expression_->type()->is_scalar())
			std::cerr << "scalar expression expected" << std::endl;

		auto test_label_ = labeled_statement::create();
		auto done_label_ = labeled_statement::create();
		auto loop_label_ = labeled_statement::create();
		auto goto_done_statement_ = jump_statement::create(done_label_);
		auto goto_loop_statement_ = jump_statement::create(loop_label_);

		auto if_statement_ = if_statement::create(test_expression_, empty_statement::create(), goto_done_statement_);
		statement_parameter_list.push_back(test_label_);
		statement_parameter_list.push_back(if_statement_);

		statement* body_statement_ = parse_statement();
		statement_parameter_list.push_back(loop_label_);
		statement_parameter_list.push_back(body_statement_);
		if (!test_expression_)
			statement_parameter_list.push_back(goto_loop_statement_);
		statement_parameter_list.push_back(done_label_);
		
		return compound_statement::create(statement_parameter_list);
	}

	//do-while循环
	//do
	//	body_statement;
	//	while(test_expression);
	//转换成条件和goto语句：
	//loop:
	//	body_statement;
	//	t = test_expression;
	//	if (t)
	//		goto_loop;
	compound_statement* parser::parse_do_statement() {
		std::list<statement*> statement_parameter_list;
		statement* body_statemnet_ = parse_statement();

		tokens_.expect(static_cast<int>(TokenAttr::WHILE));
		tokens_.expect('(');
		auto test_expression_ = parse_expression();
		tokens_.expect(')');
		tokens_.expect(';');

		auto loop_label_ = labeled_statement::create();
		auto goto_loop_statement_ = jump_statement::create(loop_label_);
		auto if_statement_ = if_statement::create(test_expression_, empty_statement::create(), goto_loop_statement_);

		statement_parameter_list.push_back(loop_label_);
		statement_parameter_list.push_back(body_statemnet_);
		statement_parameter_list.push_back(if_statement_);
		
		return compound_statement::create(statement_parameter_list);
	}

	//switch语句：
	//	swithch
	//		case labels;
	//		jump statements;
	//		default jump statement;
	compound_statement* parser::parse_switch_statement() {
		std::list<statement*> statement_parameter_list;
		tokens_.expect('(');
		auto test_expression_ = parse_expression();
		tokens_.expect(')');

		//switch语句值的类型必须是整数类型（包括字符型）
		if (!(test_expression_->type()->is_integer()))
			std::cerr << "integer type is expected" << std::endl;
		auto test_label_ = labeled_statement::create();
		auto done_label_ = labeled_statement::create();
		return compound_statement::create(statement_parameter_list);
	}
