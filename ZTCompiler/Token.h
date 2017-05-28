#ifndef _ZT_TOKEN_H_
#define _ZT_TOKEN_H_
#include <set>
#include <string>
#include <memory>
#include <type_traits>
namespace ztCompiler {
	//定义c语言的key word
	enum class keyword {
		BREAK,
		CASE,
		CHAR,
		CONST,
		CONTINUE,
		DEFAULT,
		DO,
		WHILE,
		DOUBLE,
		IF,
		ELSE,
		FOR,
		INT,
		LONG,
		RETURN,
		SHORT,
		STRUCT,
		SWITCH,
		UNION,
		TYPEDEF,
		VOID
	};

	using keyset = std::set<std::string>;

	class keyset_instance {
	private:
		static std::shared_ptr<keyset> ptr;
	public:
		static std::shared_ptr<keyset> get_instance();
	};

	enum class TokenType {
		IDENTIFIER,//标识符类型
		KEYWORD,//关键字类型
		NUMBER,	//数字类型
		SIGN,
		STRING,	//字符串类型
		COMMENT	//注释类型
	};
	//token对象应该包含
	class token {
		friend class LexicalAnalysis;
	public:
		token() = default;
		token(TokenType type, const std::string& value);
		token(TokenType type, const std::string&& value);
	private:
		const std::string value;
		TokenType type;
	};
}
#endif