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
			scan_escape_character();
		cur_++;
	}
	return create_token(TokenAttr::IDENTIFIER);
}

void scanner::tokenize(token_sequence tokens_) {
	while (true) {
		auto tok = scan();
		if (tok->type_attr == TokenAttr::END) {
			if (tokens_.empty() || (*--tokens_.end_)->type_attr != TokenAttr::NEW_LINE) {
				auto other = token::new_token(*tok);
				other->type_attr = TokenAttr::NEW_LINE;
				other->str_ = "\n";
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
	return (target >= low&&target <= high);
}

//创建一个新的token
token* scanner::create_token(int type) {
	token_.type_attr = static_cast<TokenAttr>(type);
	auto iter= token::lexical_table.find(static_cast<TokenAttr>(type));
	token_.str_ = iter->second;
 	return token::new_token(token_);
}

token* scanner::create_token(TokenAttr type) {
	return create_token(static_cast<int>(type));
}
bool scanner::skip_comment() {
	bool comment_found = false;
	//单行注释
	if (*cur_ == '/') {
		++cur_;
		if (cur_ == str_.cend())
			return false;
		if (*cur_ == '/') {
			++cur_;
			if (cur_ == str_.cend()) {
				return false;
			}
			while (*cur_ != '\n') {
				++cur_;
				if (cur_ == str_.cend())
					return false;
			}
			comment_found = true;
		}
		else if (*cur_ == '*') {  //多行注释
			++cur_;
			if (std::next(cur_, 2) == str_.cend()) {
				return false;
			}
			while (!(*cur_ == '*') && (*cur_++ == '/')) {
				++cur_;
				if (std::next(cur_, 2) == str_.cend())
					return false;
			}
			cur_ += 2;
			if (std::next(cur_, 2) == str_.cend())
				return false;
			comment_found = true;
		}
		else return false;
	}
	return comment_found;
}
void scanner::scan_string() {
	//如果该字符为raw literal或者宽字符
	std::string out;
	skip_white_space();
	if ((*cur_) == 'U' || (*cur_) == 'u') {
		//将开头的4-byte的转义字符
		std::string begin = str_.substr(*cur_, 4);
		if (in_range(*cur_, 0, 0x1f));
			//print_err("escape characters error" ,out);
		else if (in_range((*cur_), 'a', 'f') ){

		}
	}
	else {
		if ((*cur_) != '\"' || (*cur_) != '\'') {
			out += *cur_;
			++cur_;
			
		}
		else {

		}
	}
}

int scanner::scan_escape_character() {
	auto c = *cur_;
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
	auto begin_pos = cur_;
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
		//return ;

	if ((*cur_ != '.')&&(*cur_ != 'e')&&(*cur_ != 'E')) {
		return create_token(TokenAttr::FLOAT);
	}
}

std::string scanner::scan_identifier() {
	std::string ret;
	while (!str_.empty()) {
		auto c = *++cur_;
		if (c == '\\' && test_next_token('u') || test_next_token('U')) 
			c = scan_escape_character();
		++cur_;
	}
	return ret;
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
		if (!isxdigit(c))
			std::cerr << "%c is not hex digit" << c << std::endl;
		ret = (ret << 4) + to_hex(c);
	}
	return ret;
}

int scanner::to_hex(int value) {
	if (in_range(value, '0', '9'))
		return value - '0';
	else if (in_range(value, 'a', 'z'))
		return value - 'a' + 10;
	else if (in_range(value, 'A', 'Z'))
		return value - 'A' + 10;
	return value;
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

// token* scanner::skip_number() {
// 	int tag = static_cast<int>(TokenAttr::CONSTANT);
// 	auto ch = *(++cur_);
// 	while (ch == '.' || isdigit(ch) || isalpha(ch) || ch == '_') {
// 		if(ch=='e'||ch=='E'||ch)
// 	}
// }


