#include "Scanner.h"
using namespace ztCompiler;

bool scanner::test_next_token(int tag) {
	if (*cur_ == tag) {
		++cur_;
		return true;
	}
	return false;
}

token* scanner::skip_identifier() {
	auto c = *cur_++;
	while (isalnum || c == '_' || c == '$' || (c >= 0x80 && c <= 0xfd)) {
		++cur_;
	}
	return create_token(TokenAttr::IDENTIFIER);
}

void scanner::tokenize(tokens tokens_) {
	while (true) {
		
	}
}

token* scanner::scan() {
	skip_white_space();
	auto c = *cur_++;
	switch (c)
	{
	case '#':return create_token(test_next_token('#') ? static_cast<int>(TokenAttr::DOUBLE_HASH) : c);
	case '(':case ')':case '[':case ']':case '{':
	case '}':case '?':case ',':case ';':
		return create_token(c);
	case '-':
		if (test_next_token('>')) return create_token(TokenAttr::POINTER);
		if (test_next_token('-')) return create_token(TokenAttr::DEC);
		if (test_next_token('=')) return create_token(TokenAttr::SUB_ASSIGN);
		return create_token(c);
	case '+':
		if (test_next_token('+')) return create_token(TokenAttr::INC);
		if (test_next_token('=')) return create_token(TokenAttr::ADD_ASSIGN);
		return create_token(c);
	case '<':
		if (test_next_token('<')) return create_token(TokenAttr::LEFT_SHIFT);
		if (test_next_token('=')) return create_token(TokenAttr::LEFT_EQUAL);
		return create_token(c);
	case '>':
		if (test_next_token('>')) return create_token(TokenAttr::RIGHT_SHIFT);
		if (test_next_token('=')) return create_token(TokenAttr::RIGHT_EQUAL);
		return create_token(c);
	case '!':
		return create_token(test_next_token('=')? static_cast<int>(TokenAttr::NOT_EQUAL):c);
	case '=':
		return create_token(test_next_token('=') ? static_cast<int>(TokenAttr::EQUAL) : c);
	case '&':
		if(test_next_token('&')) return create_token(TokenAttr::LOGICAL_AND);
		if(test_next_token('=')) return create_token(TokenAttr::AND_ASSIGN);
		return create_token(c);
	case '|':
		if(test_next_token('|')) return create_token(TokenAttr::LOGICAL_OR);
		if(test_next_token('=')) return create_token(TokenAttr::OR_ASSIGN);
		return create_token(c);
	case '*':
		return create_token(test_next_token('=') ? static_cast<int>(TokenAttr::NOT_EQUAL) : c);
	case '/':
		if (test_next_token('/') || test_next_token('*')) {
			skip_comment();
			return scan();
		}
		if (test_next_token('=')) return create_token(TokenAttr::DIV_ASSIGN);
		return create_token(c);
	case '.':
		if (isdigit(*cur_++))
			skip_number();
		if (test_next_token('.')) {
			if(test_next_token('.'))
				return create_token(TokenAttr::ELLIPSIS);
			--cur_;
			return create_token(c);
		}
		return create_token(c);
	case '0':case '1':case '2':case '3':case '4':case '5':case '6':
	case '7':case '8':case '9':
		return skip_number();
	case '\0':
		return create_token(TokenAttr::END);
	default:
		return create_token(TokenAttr::NOTDEFINED);
	}
}
//将ch encode成utf-8,并将其加入到out中
void scanner::encode_utf8(uint32_t ch, std::string& out) {
	if (ch < 0) 
		return;
	if (ch<0x80) {
		out += static_cast<char>(ch);
	}
	else if (ch < 0x800) {
		out += static_cast<char>((ch >> 6) & 0xc0);
		out += static_cast<char>((ch & 0x3F) | 0x80);
	}
	else if (ch < 0x10000) {
		out += static_cast<char>(0xE0 | (ch >> 12));
		out += static_cast<char>(0x80 | (ch >> 6) & 0x3F);
		out += static_cast<char>(0x80 | (ch & 0x3F));
	}
	else {

	}
}
void scanner::skip_white_space() {
	while (cur_ != text_.cend())
		if (!(*cur_ != '\t' || *cur_ != '\n' || *cur_ != '\r' || *cur_ != ' '))
			++cur_;
}

bool scanner::in_range(uint32_t target, uint32_t low, uint32_t high) {
	return (target >= low&&target <= high);
}

//创建一个新的token
token* scanner::create_token(int type) {
	token_.type_ = static_cast<TokenAttr>(type);
	auto& str = token_.str_;
	str.resize(0);
	return token::new_token(token_);
}

token* scanner::create_token(TokenAttr type) {
	return create_token(type);
}
bool scanner::skip_comment() {
	bool comment_found = false;
	//单行注释
	if (*cur_ == '/') {
		++cur_;
		if (cur_ == text_.cend())
			return;
		if (*cur_ == '/') {
			++cur_;
			if (cur_ == text_.cend()) {
				return;
			}
			while (*cur_ != '\n') {
				++cur_;
				if (cur_ == text_.cend())
					return;
			}
			comment_found = true;
		}
		else if (*cur_ == '*') {  //多行注释
			++cur_;
			if (std::next(cur_, 2) == text_.cend()) {
				return;
			}
			while (!(*cur_ == '*') && (*cur_++ == '/')) {
				++cur_;
				if (std::next(cur_, 2) == text_.cend())
					return;
			}
			cur_ += 2;
			if (std::next(cur_, 2) == text_.cend())
				return;
			comment_found = true;
		}
		else return;
	}
	return comment_found;
}
void scanner::scan_string() {
	//如果该字符为raw literal或者宽字符
	std::string out;
	skip_white_space();
	if ((*cur_) == 'U' || (*cur_) == 'u') {
		//将开头的4-byte的转义字符
		std::string begin = text_.substr(*cur_, 4);
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

int scanner::scan_esc() {
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
			return;
	}
	else if (in_range(*cur_, '1', '9')) {
		++cur_;
		while (in_range(*cur_, '0', '9'))
			++cur_;
	}
	else 
		return;

	if ((*cur_ != '.')&&(*cur_ != 'e')&&(*cur_ != 'E')) {
		return create_token(TokenAttr::FLOAT);
	}
}

std::string scanner::scan_identifier() {
	std::string ret;
	while (!text_.empty()) {
		auto c = *++cur_;
		if (c == '\\' && test_next_token('u')||test_next_token('U')) 
			c = scan_esc();
		++cur_;
	}
	return ret;
}

int scanner::scan_ucn(int length) {
	assert(length == 4 || length == 8);	//一个UCN的长度为4或者8
	int ret = 0;
	for (int i = 0; i < length; ++i) {
		auto c = *++cur_;
		if (!isxdigit(c))
			return;
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


