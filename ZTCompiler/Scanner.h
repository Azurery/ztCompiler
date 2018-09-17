#ifndef _ZT_SCANNER_H_
#define _ZT_SCANNER_H_
#include "Token.h"
#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>
#include <regex>
#include <vector>

#define CREATE_TOKEN(token_name, attr)\
	do{\
		auto ret = create_token(attr);\
		token_.set_token_attr((ret->get_token_attr()));\
		token_.set_token_name(token_name);\
		return ret;\
	} while(0)

namespace ztCompiler {
	class tokens;
	class token;
	class scanner {
	public:
		enum class character_encode {
			UTF_8,
			CHAR_16,
			CHAR_32,
			WCHAR,
			NONE
		};
	private:
		token token_;
		std::string::const_iterator cur_;
		const std::string& str_;
	public:;
		scanner(const std::string& str) :str_(str), cur_(str.begin()), token_(TokenAttr::END) {}
		explicit scanner(const token* token_) :scanner(token_->str_){}
//		virtual ~scanner();
		scanner(const scanner& other) = delete;
		scanner& operator=(const scanner& other) = delete;

	public:
		token* create_identifier(std::string str) {
			CREATE_TOKEN(str,TokenAttr::IDENTIFIER);
		}

		token* create_keyword(int tag) {
			CREATE_TOKEN(std::to_string(tag), TokenAttr::STRING);
		}
		
		token* create_string(std::string str) {
			CREATE_TOKEN(str, TokenAttr::STRING);
		}

		token* create_number(std::string num) {
			token* ret = nullptr;
			std::regex int_regex("^[1 - 9]\d*$");
			std::regex float_regex("^-?([1-9]\d*\.\d*|0\.\d*[1-9]\d*|0?\.0+|0)$");
			if (std::regex_match(num, int_regex)) {
				ret = create_token(TokenAttr::INT);
			}
			else if (std::regex_match(num, float_regex)) {
				ret = create_token(TokenAttr::FLOAT);
			}
			token_.set_token_attr(ret->get_token_attr());
			token_.set_token_name(ret->get_token_name());
			return ret;
		}

		token* create_char(char ch) {
			CREATE_TOKEN(std::to_string(ch), TokenAttr::STRING);
		}

		void tokenize(token_sequence tokens_);
		token* scan();
		int scan_escaped_character();	
		token* create_token(int tag);
		token* create_token(TokenAttr type);

		character_encode detect_encode(char ch);
		void encode_utf8(long code_point, std::string& ret);	//用于解析utf-8字符
		void skip_white_space();
		bool skip_comment();
		void skip_garbage();
		token* skip_literal();
		//token* skip_number();
		token* skip_identifier();
		int scan_ucn(int length);
		bool is_ucn(int alpha); 
		long to_hex(int ch);
		long parse_hex(const std::string& source);
		
		//************************************
		// \method name:    test_next_token
		//
		// \brief:			用于测试下一个token
		//************************************
		bool test_next_token(int tag);	

		//************************************
		// \method name:    scan_string
		//
		// \brief:			用于处理字符
		//************************************


		token* scan_character();
		token* scan_string();
		//---用于处理数字
		token* scan_number();	
		//---用于处理标识符
		token* scan_identifier();

		bool in_range(uint32_t target,uint32_t low,uint32_t high);	 //用于判断字符是否属于某个集合
	};
}
#endif
