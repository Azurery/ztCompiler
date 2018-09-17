#include "Scanner.h"
using namespace ztCompiler;

bool scanner::test_next_token(int tag) {
	if (*cur_ == tag) {
		if (++cur_ != str_.end()) {
			++cur_;
		}
		return true;
	}
	return false;
}

token* scanner::skip_identifier() {
	auto c = *cur_++;
	while (isalnum(c) ||is_ucn(c)|| c == '_' || c == '$' || (c >= 0x80 && c <= 0xfd)) {
		if (is_ucn(c))
			scan_escaped_character();
		cur_++;
	}
	return create_token(TokenAttr::IDENTIFIER);
}

void scanner::tokenize(token_sequence tokens_) {
	while (true) {
		auto tok = scan();
		if (tok->get_token_attr() == TokenAttr::END) {
			if (tokens_.empty() || (*--tokens_.end_)->get_token_attr() != TokenAttr::NEW_LINE) {
				auto other = token::new_token(*tok);
				other->set_token_attr(TokenAttr::NEW_LINE);
				other->set_token_name("\n");
				tokens_.insert_back(other);
			}
		}
		else
			tokens_.insert_back(tok);
	}
}

token* scanner::scan() {
	skip_white_space();
	auto ch = *(cur_++);
	if (ch == '#') {
		return create_token(test_next_token('#') ? static_cast<int>(TokenAttr::DOUBLE_SHARP) : ch);
	}
	else if (ch == '#' || ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '?' || ch == ',' || ch == ';') {
		return create_token(ch);
	}
	else if (ch == '-') {
		if (test_next_token('>')) return create_token(TokenAttr::POINTER);
		if (test_next_token('-')) return create_token(TokenAttr::DEC);
		if (test_next_token('=')) return create_token(TokenAttr::SUB_ASSIGN);
		return create_token(ch);
	}
	else if (ch == '+') {
		if (test_next_token('+')) return create_token(TokenAttr::INC);
		if (test_next_token('=')) return create_token(TokenAttr::ADD_ASSIGN);
		return create_token(ch);
	}
	else if (ch == '<') {
		if (test_next_token('<')) return create_token(TokenAttr::LEFT_SHIFT);
		if (test_next_token('=')) return create_token(TokenAttr::LESS_EQUAL);
		return create_token(ch);
	}
	else if (ch == '>') {
		if (test_next_token('>')) return create_token(TokenAttr::RIGHT_SHIFT);
		if (test_next_token('=')) return create_token(TokenAttr::GREATER_EQUAL);
		return create_token(ch);
	}
	else if (ch == '=') {
		return create_token(test_next_token('=') ? static_cast<int>(TokenAttr::EQUAL) : ch);
	}
	else if (ch == '!') {
		return create_token(test_next_token('=') ? static_cast<int>(TokenAttr::NOT_EQUAL) : ch);
	}
	else if (ch == '&') {
		if (test_next_token('&')) return create_token(TokenAttr::LOGICAL_AND);
		if (test_next_token('=')) return create_token(TokenAttr::AND_ASSIGN);
		return create_token(ch);
	}
	else if (ch == '|') {
		if (test_next_token('|')) return create_token(TokenAttr::LOGICAL_OR);
		if (test_next_token('=')) return create_token(TokenAttr::OR_ASSIGN);
		return create_token(ch);
	}
	else if (ch == '^') {
		return create_token(test_next_token('=') ? static_cast<int>(TokenAttr::XOR_ASSIGN) : ch);
	}
	else if (ch == '*') {
		return create_token(test_next_token('=') ? static_cast<int>(TokenAttr::MUL_ASSIGN) : ch);
	}
	else if (ch == '/') {
		if (test_next_token('/') || test_next_token('*')) {
			skip_comment();
			return scan();
		}
		if (test_next_token('=')) return create_token(TokenAttr::DIV_ASSIGN);
		return create_token(ch);
	}
	else if (ch == '.') {
		if (test_next_token('.')) {
			if (test_next_token('.'))
				return create_token(TokenAttr::ELLIPSIS);
			--cur_;
			return create_token(ch);
		}
		return create_token(ch);
	}
	else if (ch == '\0') {
		return create_token(TokenAttr::END);
	}
	else if (ch == '\\') {
		if (test_next_token('u') || test_next_token('U'))
			return skip_identifier();
	}
	else if (ch == '\"') {
		return skip_literal();
	}
	else if (in_range(ch, 'a', 't') ||
		in_range(ch, 'v', 'z') ||
		in_range(ch, 'A', 'K') ||
		in_range(ch, 'M', 'T') ||
		in_range(ch, 'V', 'Z') ||
		in_range(ch, 0x80, 0xFD) ||
		ch == '_') {
		return scan_identifier();
	}
	else if (ch == 'L' || ch == 'U') {
		int val = (ch == 'L') ? static_cast<int>(character_encode::WCHAR) : static_cast<int>(character_encode::CHAR_32);
		if (test_next_token('"')) 
			return scan_string();
		if (test_next_token('\''))
			return scan_character();
		return scan_identifier();
	}
	else if (ch == 'u') {
		if (test_next_token('"'))
			return scan_string();
		if (test_next_token('\''))
			return scan_character();
		return scan_identifier();
	}
	else {
		return create_token(TokenAttr::NOTDEFINED);
	}
}

//将ch encode成utf-8,并将其加入到ret中
void scanner::encode_utf8(long code_point, std::string& ret) {
	if (code_point < 0) {
		return;
	}
	else if (code_point <= 0x7F) {
		ret.push_back(static_cast<char>(code_point));
	}
	else if (code_point <= 0x7FF) {
		//0xC0 ===> 1100 0000
		//0x80 ===> 1000 0000
		//0x3F ===> 0011 1111

		//U+0080 ~ U+07FF 
		ret.push_back(static_cast<char>(0xC0 | ((code_point >> 6) & 0xFF)));	//将字节1进行补全	
		ret.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));	      //c & 0x3F ===> 获取c的低6位，再|0x80 ====> 得到完整的字节2
	}
	else if (code_point <= 0xFFFF) {
		ret.push_back(static_cast<char>(0xE0 | (code_point >> 12)));
		ret.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
		ret.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
	}
	else {
		//assert(code_point <= 0x10FFFF && "the code out of the range");
		ret.push_back(static_cast<char>(0xF0 | ((code_point >> 18) & 0xFF)));
		ret.push_back(static_cast<char>(0x80 | ((code_point >> 12) & 0xFF)));
		ret.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
		ret.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
	}
}

void scanner::skip_white_space() {
	for (; *cur_ == '\t' || *cur_ == '\n' || *cur_ == '\r' || *cur_ == ' '; ++cur_) 
		;
}

bool scanner::in_range(uint32_t target, uint32_t low, uint32_t high) {
	return (target >= low && target <= high);
}

//创建一个新的token
token* scanner::create_token(int type) {
	return create_token(static_cast<TokenAttr>(type));
}

token* scanner::create_token(TokenAttr type) {
 	token_.set_token_attr(type);
 	std::unordered_map<TokenAttr,const char*>::const_iterator iter = token::lexical_table.find(type);
 	token_.set_token_name(iter->second);
	return token::new_token(token_);
}

bool scanner::skip_comment() {
	bool comment_sign = false;
	//单行注释
	if (*cur_ == '/') {
		cur_++;
		if (cur_ == str_.cend())
			std::cerr << "Unexpected end of input" << std::endl;
		if (*cur_ == '/') {
			cur_++;
			if (cur_ == str_.cend()) {
				std::cerr << "Unexpected end of input" << std::endl;
			}
			for (; ((cur_ != str_.cend()) && (*cur_ != '\n'));) {
				cur_++;
			}
			comment_sign = true;
		}
		else if (*cur_ == '*') {  //多行注释
			cur_++;
			if (std::next(cur_, 2) == str_.cend()) {
				std::cerr << "Unexpected end of input" << std::endl;
			}
			while (!(*cur_ == '*') && (*(++cur_) == '/')) {
				cur_++;
				if (std::next(cur_, 2) == str_.cend())
					std::cerr << "Unexpected end of input" << std::endl;
			}
			std::next(cur_, 2);
			comment_sign = true;
		}
	}
	return comment_sign;
}

void scanner::skip_garbage() {
	skip_white_space();
	bool comment_sign = false;
	do {
		comment_sign = skip_comment();
		skip_white_space();
	} while (comment_sign);
}

token* scanner::scan_string() {
	//如果该字符为raw literal或者宽字符
	std::string buf;
	long code_point = -1;
	skip_garbage();
	cur_++;
	if (*cur_ == '\"') {
		cur_++;
		return {};
	}
	for (;;) {
		if (cur_ == str_.cend())
			std::cerr << "incorrect input in string" << std::endl;
		char ch = *cur_++;
		//普通的ASCII，则不需要进行转换
		if (ch != '\\') {
			encode_utf8(code_point, buf);
			code_point = -1;
			buf.push_back(ch);
			continue;
			//auto ret=create_token(TokenAttr::)
		}
		if (cur_ == str_.cend())
			std::cerr << "incorrect input in string" << std::endl;
		ch = *cur_++;

		if (ch == 'u' || ch == 'U') {
			std::string high_seq = str_.substr((cur_ - str_.begin()), 4);
			long high_surrogate = parse_hex(high_seq);
			//FIXME:暂时先不考虑0xD800~0xD8FF
			encode_utf8(high_surrogate, buf);
		}
		std::next(cur_, 4);
		continue;
	}
	
	for (; cur_ != str_.cend(); cur_++) {
		char ch = *cur_;
		if (ch == '\\') {
			cur_++;
			buf.push_back(scan_escaped_character());
			continue;
		}
	}

	if (*cur_ == '\"') {
		cur_++;
		return create_string(buf);
	}
	else {
		buf.push_back(*cur_);
	}
}

int scanner::scan_escaped_character() {
	auto c = *(cur_++);
	switch (c) {
	case '\\':
	case '\'':
	case '\"':
	case '\?':
		return c;
	case 'a':return '\a';
	case 'b':return '\b';
	case 'f':return '\f';
	case 'r':return '\r';
	case 'n':return '\n';
	case 't':return '\t';
	case 'v':return '\v';
	default:
		;
	}
	return c;
}

token* scanner::scan_number() {
	skip_white_space();
	std::string buf;
	auto start_pos = cur_;
	if (*cur_ == '-')
		++cur_;
	//整数部分
	if (*cur_ == '0') {
		++cur_;
		//leading 0是不允许的
		if (in_range(*cur_, '0', '9'))
			std::cerr << "leading zero is not allowed" << std::endl;
	}
	else if (in_range(*cur_, '1', '9')) {
		++cur_;
		while (in_range(*cur_, '0', '9'))
			++cur_;
	}
	else
		std::cerr << "invalid number" << *cur_ << std::endl;

	if (!(*cur_ != '.'&& *cur_ != 'e'&& *cur_ != 'E')) {
		buf.push_back(std::atoi(str_.c_str() + (start_pos-str_.begin())));
		return create_number(buf);
	}

	//小数部分
	if (*cur_ == '.') {
		cur_++;
		if (!isdigit(*cur_))
			std::cerr << "Unexpected digit at position" << std::endl;
		while (isdigit(*cur_))
			cur_++;
	}
	//指数部分
	if (*cur_ == 'e' || *cur_ == 'E') {
		cur_++;
		if (*cur_ == '-' || *cur_ == '+')
			cur_++;
		if (!isdigit(*cur_))
			std::cerr << "Unexpected digit at position" << (cur_ - str_.begin()) << std::endl;
		while (isdigit(*cur_))
			cur_++;
	}
	buf.push_back(strtod(str_.c_str() + (start_pos - str_.begin()), nullptr));
	return create_number(buf);
}

token* scanner::scan_identifier() {
	std::string buf;
	cur_--;
	while (!str_.empty() && (cur_ != str_.cend())) {
		auto ch = *(cur_++);
		if (isalnum(ch) || (ch & 0x80) || ch == '_') {
			buf.push_back(ch);
			continue;
		}
		if (ch == '\\' && test_next_token('u') || test_next_token('U')) {
			scan_escaped_character();
			buf.push_back(ch);
			continue;
		}	
		++cur_;
	}
	return create_identifier(buf);
}

//UCN:Universal character names
/*
	Syntax
		universal-character-name:
			\u hex-quad
			\U hex-quad hex-quad
		hex-quad:
			hexadecimal-digit hexadecimal-digit
			hexadecimal-digit hexadecimal-digit
*/

int scanner::scan_ucn(int length) {
	assert(length == 4 || length == 8);	//一个UCN的长度为4或者8
	int ret = 0;
	for (int i = 0; i < length; ++i) {
		auto c = *++cur_;
		ret = (ret << 4) + to_hex(c);
	}
	return ret;
}

long scanner::to_hex(int ch) {
	if (in_range(ch, '0', '9'))
		ch -= '0';
	else if (in_range(ch, 'a', 'f'))
		ch = ch - 'a' + 10;
	else if (in_range(ch, 'A', 'F'))
		ch = ch - 'A' + 10;	
	return ch;
}

long scanner::parse_hex(const std::string& source) {
	long ret = 0;
	for (int i = 0; i < 4; i++) {
		char ch = source[i];
// 		if (!isxdigit(ch))
// 			std::cerr << "%c is not hex digit" << ch << std::endl;
// 			
		assert(in_range(ch, '0', '9') ||
			in_range(ch, 'a', 'f') ||
			in_range(ch, 'A', 'F'));
		ret = ret << 4 + to_hex(ch);
	}
	return ret;
}

token* scanner::skip_literal() {
	char ch = *std::next(cur_);
	for (char ch = *std::next(cur_); ch != '\n' || ch != '\"' || ch != '\0'; std::next(cur_)) {
		;
	}
	return create_token(TokenAttr::LITERAL);
}

bool scanner::is_ucn(int ch) {
	return ch == '\\' && (test_next_token('u') || test_next_token('U'));
}

token* scanner::scan_character() {
// 	auto encoding = test_next_token('\'') ? scanner::character_encode::NONE
// 		: detect_encode(*(++cur_));
// 	cur_++;
// 	
		char ch = *cur_++;
		if (ch == '\\')
			ch = scan_escaped_character();
// 		if (encoding == character_encode::NONE) {
// 			return create_char(ch);
// 		}
	return create_char(ch);
}

scanner::character_encode scanner::detect_encode(char ch) {
	if (ch == 'u')
		return test_next_token('8') ? scanner::character_encode::UTF_8 : scanner::character_encode::CHAR_16;
	else if (ch == 'U')
		return scanner::character_encode::CHAR_32;
	else if (ch == 'L')
		return scanner::character_encode::WCHAR;
	else 
		return scanner::character_encode::NONE;
}

// token* scanner::skip_number() {
// 	int tag = static_cast<int>(TokenAttr::CONSTANT);
// 	auto ch = *(++cur_);
// 	while (ch == '.' || isdigit(ch) || isalpha(ch) || ch == '_') {
// 		if(ch=='e'||ch=='E'||ch)
// 	}
// }


