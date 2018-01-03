#ifndef _ZT_TOKEN_H_
#define _ZT_TOKEN_H_
#include <set>
#include <string>
#include <iostream>
#include <cassert>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "MemoryPool.h"

namespace ztCompiler {
	class token;
	using token_list = std::vector<const token*>;	//定义一个token表
	//定义单词的属性
	enum class TokenAttr {
		IDENTIFIER,		//标识符
		/*运算符*/
		ADD = '+',
		SUB = '-',
		MUL = '*',
		DIV = '/',
		LESS = '<',
		GREATER = '>',
		HASH = '#',
		DOUBLE_HASH = '##',
		DEC, INC,
		/*声明类型*/
		DEREF,//解引用
		POINTER,	//指针
		PREFIX_DEC,POST_DEC,	//--
		POST_INC, PREFIX_INC,//前置++	//++
		SUB_ASSIGN,	//-=
		ADD_ASSIGN,	//+=
		LEFT_SHIFT,	//<<
		LEFT_EQUAL,	//<=
		RIGHT_SHIFT,//>>
		RIGHT_EQUAL,//>=
		EQUAL,		//==
		NOT_EQUAL,	//!=
		LOGICAL_AND,//&&
		AND_ASSIGN,	//&=
		LOGICAL_OR,	//||
		OR_ASSIGN,	//|=
		LOGICAL_XOR,
		DIV_ASSIGN, // /=
		MUL_ASSIGN,	//*=
		ELLIPSIS,	//...
		
		/*keyword*/
		VOID,	LONG,		INT,		SHORT,
		FLOAT,	SIGNED,		CHAR,		UNSIGNED,
		BREAK,	BOOL,		CASE,		COMPLEX,
		CHAR,	CONST,		RESTRICT,	VOLATILE,
		ATOMIC,	CONTINUE,	DEFAULT,	DO,
		WHILE,	DOUBLE,		IF,			ELSE,
		FOR,	RETURN,		STRUCT,		SWITCH,
		UNION,	TYPEDEF,    GOTO,		END,
		GOTO,   EXTERN,		AUTO,STATIC_ASSERT,
		THREAD_LOCAL,  

		NEW_LINE,
		NOTDEFINED	//代表不属于以上任意一种类型
	};

	//token对象应该包含词素和属性值
	class token {
		friend class scanner;
	public:
		static memory_pool token_pool;
		static token* new_token(const token& other) {
			return new(token_pool.allocate(sizeof(other))) token(other);
		}
		static token* new_token(TokenAttr type) {
			return new(token_pool.allocate(sizeof(type))) token(type);
		}
		token& operator = (const token& other) {
			str_ = other.str_;
			type_attr = other.type_attr;
			return *this;
		}
		//根据key获得其type
		static TokenAttr get_key_type(const std::string& key) {
			auto iter = keyword_table.find(key);
			if (iter != keyword_table.end()) {
				return TokenAttr::NOTDEFINED;
			}
			return iter->second;
		}
		
		bool is_keyword() const {
			return is_keyword(static_cast<int>(type_attr));
		}
		static bool is_keyword(int tag) { 
			return static_cast<TokenAttr>(tag) >= TokenAttr::VOID&&static_cast<TokenAttr>(tag) <= TokenAttr::THREAD_LOCAL; }
		
		bool is_identifier() const { return type_attr == TokenAttr::IDENTIFIER; }
		
		bool is_type_qualifier() const {
			switch (type_attr) {
			case TokenAttr::CONST:
			case TokenAttr::RESTRICT:
			case TokenAttr::VOLATILE:
			case TokenAttr::ATOMIC:
				return true;
			default:
				return true;
			}
		}
		bool is_type_specifier() const {
			switch (type_attr){
			case TokenAttr::VOID:case  TokenAttr::CHAR:case  TokenAttr::SHORT :
			case TokenAttr::INT : case TokenAttr::LONG :case  TokenAttr::FLOAT :
			case TokenAttr::DOUBLE :case  TokenAttr::SIGNED : case TokenAttr::UNSIGNED :
			case TokenAttr::BOOL : case TokenAttr::COMPLEX : case TokenAttr::STRUCT :
			case TokenAttr::UNION:
				return true;
			default:
				return false;
			}
		}
		bool is_punctuator() {
			return type_attr >= TokenAttr::ADD&&type_attr <= TokenAttr::ELLIPSIS;
		}
		bool is_eof() const { return type_attr == TokenAttr::END; }
		virtual ~token() {}
	private:
		token(std::string str, TokenAttr type) :str_(str), type_attr(type) {};
		token(const token& other) { *this = other; }
		explicit token(TokenAttr type) :type_attr(type) {}
		token(TokenAttr type, std::string str) :type_attr(type), str_(str) {}
		static const std::unordered_map<std::string, TokenAttr> keyword_table;

	public:
		std::string str_;	//Token的词素
		TokenAttr type_attr;		//Token的属性值

	};

	////此class为一个token集。即由一组token组成
	class token_collection {
		friend class token;
		friend class scanner;
	private:
		token_list* tokens_;
		token_list::iterator begin_;
		token_list::iterator end_;
	public:
		token_collection() :tokens_(new token_list()), begin_(tokens_->begin()), end_(tokens_->end()) {}
		explicit token_collection(token* t) {
			token_collection();
			insert_back(t);
		}
		explicit token_collection(token_list* token_list_)
			:tokens_(token_list_), begin_(token_list_->begin()), end_(token_list_->end()) {}
		token_collection(token_list* token_list_,
			token_list::iterator begin,
			token_list::iterator end)
			:tokens_(token_list_),begin_(begin),end_(end){}
		~token_collection() {};
		token_collection(const token_collection &other) { *this = other; }
		const token_collection& operator=(const token_collection& other) {
			tokens_ = other.tokens_;
			begin_ = other.begin_;
			end_ = other.end_;
			return *this;
		}

		void copy(const token_collection& other) {
			auto ts= new token_list(other.begin_, other.end_);
			this->tokens_ = ts;
			this->begin_ = ts->begin();
			this->end_ = ts->end();
			for (auto iter = begin_; iter != end_; ++iter)
				*iter = token::new_token(**iter);
		}
		bool empty() {
			return test_next_token()->type_attr == TokenAttr::END;
		}

		//是否包含type类型的token
		//bool contains(TokenAttr type) { get_next_token()->type_attr == type; }
		
		//返回下一个token(只测试该token，不向前移动token_list的偏移指针)
		const token* test_next_token() const {
			auto flag = token::new_token(TokenAttr::END);
			if (begin_ != end_ && (*begin_)->type_attr == TokenAttr::NEW_LINE) {
				std::next(begin_);
				return test_next_token();
			}
			else if (begin_ == end_) {	//如果token_list为空
				if (end_ != tokens_->begin()) {
					auto back_ = end_;
					*flag = **std::prev(back_);
					flag->type_attr = TokenAttr::END;
					return flag;
				}
			}
			else if ((*begin_)->type_attr == TokenAttr::IDENTIFIER) {

			}
			return *begin_;
		}
		const token* consume_next_token() {//消费下一个Token
			auto ret = test_next_token();
			if (!ret->is_eof()) {
				++begin_;
				test_next_token();
			}
			return ret;
		}
		token_list::iterator get_insert_pos() {
			auto pos = begin_;
			if (pos == tokens_->begin())
				return begin_;
		}
		void insert_back(token_collection& ts) {
			auto pos = tokens_->insert(end_, ts.begin_, ts.end_);
			if (begin_ == end_)
				begin_ = pos;
		}
	};
}
#endif