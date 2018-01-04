#ifndef _ZT_SCANNER_H_
#define _ZT_SCANNER_H_
#include "Token.h"
#include <string>
#include <type_traits>
#include <vector>
#include <cassert>
#include <unordered_map>
#include <regex>
#include <vector>
namespace ztCompiler {
	class tokens;
	class token;
	class scanner {
	private:
		token token_;
		std::string::const_iterator cur_;
		const std::string text_;
	public:;
		explicit scanner(const std::string text):text_(text),token_(TokenAttr::END){}
		virtual ~scanner();
		scanner(const scanner& other) = delete;
		scanner& operator=(const scanner& other) = delete;

	public:
		void tokenize(token_collection tokens_) {}
		token* scan();
		int scan_escape_character();	
		token* create_token(int tag);
		token* create_token(TokenAttr type);
		void encode_utf8(uint32_t ch, std::string& out);	//用于解析utf-8字符
		void skip_white_space();
		bool skip_comment();
		token* skip_number();
		token* skip_identifier();
		int scan_ucn(int length);
		bool is_ucn(int alpha) { return alpha == '\\' && (test_next_token('u') || test_next_token('U')); }
		int to_hex(int value);
		//---用于测试下一个token
		bool test_next_token(int tag);	
		//---用于处理字符
		void scan_string();
		//---用于处理数字
		token* scan_number();	
		//---用于处理标识符
		std::string scan_identifier();
		bool in_range(uint32_t target,uint32_t low,uint32_t high);	 //用于判断字符是否属于某个集合
	};
}
#endif
