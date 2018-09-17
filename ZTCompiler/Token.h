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

namespace ztCompiler {
	class token;
	using token_list = std::vector<const token*>;	//定义一个token表
	//定义单词的属性
	enum class TokenAttr {
		IDENTIFIER,		//标识符
		STRING,
		/*Punctutors*/
		PLUS,
		SUB,
		MUL,
		DIV,
		LESS,		//<
		GREATER,	//>
		SHARP,		//#
		ADDRDESS,	//&
		DOUBLE_SHARP,//##
		DEC,		//++
		INC,		//--
		DEREFERENCE,//* 解引用
		POINTER,	//* 指针
		PREFIX_DEC,
		POSTFIX_DEC,//--
		POSTFIX_INC,
		PREFIX_INC,	//++
		SUB_ASSIGN,	//-=
		ADD_ASSIGN,	//+=
		LEFT_SHIFT,	//<<
		LESS_EQUAL,	//<=
		MOD_ASSIGN,	//%=
		LEFT_SHIFT_ASSIGN,//<<==
		RIGHT_SHIFT_ASSIGN,//>>==
		EXCLUSIVE_OR_ASSIGN,//^=
		RIGHT_SHIFT,//>>
		GREATER_EQUAL,//>=
		EQUAL,		//==
		NOT_EQUAL,	//!=
		LOGICAL_AND,//&&
		AND_ASSIGN,	//&=
		LOGICAL_OR,	//||
		OR_ASSIGN,	//|=
		XOR_ASSIGN, //^=s
		EXCLUSIVE_OR,//^
		INCLUSIVE_OR,//|
		INCLUSIVE_OR_ASSIGN,//|=
		DIV_ASSIGN, // /=
		MUL_ASSIGN,	//*=
		ELLIPSIS,	//...
		
		/*keyword*/
		KEYWORD,
		VOID,	
		LONG,		
		INT,		
		SHORT,
		FLOAT,	
		SIGNED,		
		CHAR,		
		UNSIGNED,
		BREAK,	
		BOOL,		
		CASE,		
		COMPLEX,
		CONST,		
		RESTRICT,	
		VOLATILE,
		ATOMIC,	
		CONTINUE,	
		DEFAULT,	
		DO,
		WHILE,	
		DOUBLE,		
		IF,			
		ELSE,
		FOR,	
		RETURN,		
		STRUCT,		
		SWITCH,
		UNION,	    
		GOTO,		
		END,	
		ENUM,	
		LITERAL,
		CONSTANT, 
		INTEGER_CONSTANT, 
		DOUBLE_CONSTANT,
		FLOAT_CONSTANT,
		CHARACTER_CONSTANT,  
		INLINE,			
		NORETURN,
		EXTERN,	  
		AUTO,				
		TYPEDEF, 
		REGISTER, 
		STATIC_ASSERT,				
		STATIC, 
		SIZEOF,
		THREAD_LOCAL,  

		CAST,
		NEW_LINE,
		NOTDEFINED	//代表不属于以上任意一种类型
	};

	//token对象应该包含词素和属性值
	class token {
		friend class scanner;
	public:
// 		static memory_pool token_pool;
// 		static token* new_token(const token& other) {
// 			return new(token_pool.allocate(sizeof(other))) token(other);
// 		}
// 		static token* new_token(TokenAttr type) {
// 			return new(token_pool.allocate(sizeof(type))) token(type);
// 		}
		
		static token* new_token(const token& other) {
			char* buffer = new char[sizeof(other)];
			return new(buffer) token(other);
		}
		static token* new_token(TokenAttr type) {
			char* buffer = new char[sizeof(type)];
			return new(buffer) token(type);
		}
 
		token& operator = (const token& other) {
			set_token_name(other.get_token_name());
			set_token_attr(other.get_token_attr());
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
		
		bool is_literal() const {
			return get_token_attr() == TokenAttr::LITERAL;
		}
		bool is_keyword() const {
			return is_keyword(static_cast<int>(get_token_attr()));
		}
		static bool is_keyword(int tag) { 
			return static_cast<TokenAttr>(tag) >= TokenAttr::KEYWORD&&static_cast<TokenAttr>(tag) <= TokenAttr::THREAD_LOCAL;
		}
		
		bool is_identifier() const { return get_token_attr() == TokenAttr::IDENTIFIER; }
		
		bool is_type_qualifier() const {
			switch (get_token_attr()) {
			case TokenAttr::CONST:
			case TokenAttr::RESTRICT:
			case TokenAttr::VOLATILE:
			case TokenAttr::ATOMIC:
			case TokenAttr::EXTERN:
				return true;
			default:
				return true;
			}
		}
		bool is_type_specifier() const {
			switch (get_token_attr()){
			case TokenAttr::VOID:
			case  TokenAttr::CHAR:
			case  TokenAttr::SHORT:
			case TokenAttr::INT: 
			case TokenAttr::LONG:
			case  TokenAttr::FLOAT:
			case TokenAttr::DOUBLE:
			case  TokenAttr::SIGNED: 
			case TokenAttr::UNSIGNED:
			case TokenAttr::BOOL: 
			case TokenAttr::COMPLEX: 
			case TokenAttr::STRUCT:
			case TokenAttr::UNION:
				return true;
			default:
				return false;
			}
		}
		bool is_constant() const{
			switch (get_token_attr()) {
			case TokenAttr::CONSTANT: 
			case TokenAttr::INTEGER_CONSTANT: 
			case TokenAttr::DOUBLE_CONSTANT:
			case TokenAttr::FLOAT_CONSTANT:
			case TokenAttr::CHARACTER_CONSTANT:
				return true;
			default:
				return false;
			}
		}

		bool is_punctuator() {
			return get_token_attr() >= TokenAttr::PLUS&&get_token_attr() <= TokenAttr::ELLIPSIS;
		}

		bool is_eof() const { return get_token_attr() == TokenAttr::END; }
		virtual ~token() {}

		static const char* lexical_search(int flag) {
			auto iter = lexical_table.find(static_cast<TokenAttr>(flag));
			if (iter == lexical_table.end())
				return nullptr;
			return iter->second;
		}

	public:
		token(std::string str, TokenAttr type) :str_(str), type_attr_(type) {};
		token(const token& other) { *this = other; }
		explicit token(TokenAttr type) : type_attr_(type) {}
		token(TokenAttr type, std::string str) :type_attr_(type), str_(str) {}

		static const std::unordered_map<std::string, TokenAttr> keyword_table;
		static const std::unordered_map<TokenAttr, const char*> lexical_table;

		std::string get_token_name() const { return str_; }
		void set_token_name(std::string val) { str_ = val; }

		TokenAttr get_token_attr() const { return type_attr_; }
		void set_token_attr(TokenAttr val) { type_attr_ = val; }
	private:
		std::string str_;	//Token的词素
		TokenAttr type_attr_;//Token的属性值
	};

	////此class为一个token集。即由一组token组成
	class token_sequence {
		friend class token;
		friend class scanner;
	private:
		token_list* tokens_;
		token_list::iterator begin_;
		token_list::iterator end_;
	public:
		token_sequence() :tokens_(new token_list()), begin_(tokens_->begin()), end_(tokens_->end()) {}
		explicit token_sequence(token* t) {
			token_sequence();
			insert_back(t);
		}
		explicit token_sequence(token_list* token_list_)
			:tokens_(token_list_), begin_(token_list_->begin()), end_(token_list_->end()) {}
		token_sequence(token_list* token_list_,
			token_list::iterator begin,
			token_list::iterator end)
			:tokens_(token_list_),begin_(begin),end_(end){}
		~token_sequence() {};
		token_sequence(const token_sequence &other) { *this = other; }
		const token_sequence& operator=(const token_sequence& other) {
			tokens_ = other.tokens_;
			begin_ = other.begin_;
			end_ = other.end_;
			return *this;
		}

		void copy(const token_sequence& other) {
			auto ts= new token_list(other.begin_, other.end_);
			this->tokens_ = ts;
			this->begin_ = ts->begin();
			this->end_ = ts->end();
			for (auto iter = begin_; iter != end_; ++iter)
				*iter = token::new_token(**iter);
		}
		//判断当前token序列是否为空
		bool empty() {
			return test_next_token()->get_token_attr() == TokenAttr::END;
		}

		//是否包含type类型的token
		//bool contains(TokenAttr type) { get_next_token()->get_token_attr() == type; }
		
		//返回下一个token(只测试该token，不向前移动token_list的偏移指针)
		const token* test_next_token() const {
			auto flag = token::new_token(TokenAttr::END);
			if (begin_ != end_ && (*begin_)->get_token_attr() == TokenAttr::NEW_LINE) {
				std::next(begin_);
				return test_next_token();
			}
			else if (begin_ == end_) {	//如果token_list为空
				if (end_ != tokens_->begin()) {
					auto back_ = end_;
					*flag = **std::prev(back_);
					flag->set_token_attr(TokenAttr::END);
					return flag;
				}
			}
			else if ((*begin_)->get_token_attr() == TokenAttr::IDENTIFIER) {

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
		//将当前token序列插入到之前的token系列之后
		void insert_back(token_sequence& ts) {
			auto pos = tokens_->insert(end_, ts.begin_, ts.end_);
			if (begin_ == end_)
				begin_ = pos;
		}
		//将当前token插入到之前的token序列之后
		void insert_back(const token* tok) {
			auto pos = tokens_->insert(end_, tok);
			if (begin_ == end_)
				begin_ = pos;
		}
		const token* expect(int expect) {
			auto token_ = consume_next_token();
			if (test_next_token()->get_token_attr()!=static_cast<TokenAttr>(expect)) {
				std::cerr << token_<< "'%s' is expected, but got '%s'"<<
					token::lexical_search(expect)<< token_->get_token_name().c_str() << std::endl;
			}
			return token_;
		}
	};
}
#endif