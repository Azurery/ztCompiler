#include "Parser.h"

namespace ztCompiler {
	expression* parser::parse_primary_expression() {
		if (tokens_.empty()) {
			std::cerr << "输如结束过早" << std::endl;
		}
		auto tok = tokens_.consume_next_token();
		if (tok->type_attr == static_cast<TokenAttr>('(')) {

		}
	}

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
			flag = static_cast<int>(TokenAttr::LONG_CONSTANT) | static_cast<int>(TokenAttr::DOUBLE_CONSTANT);
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
		long long value;
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

		}
	}

}