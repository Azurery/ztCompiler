#ifndef _ZT_TOKEN_H_
#define _ZT_TOKEN_H_
#include <set>
#include <string>
#include <memory>
namespace ZTCompiler {
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
		friend class Token;
	private:
		static std::shared_ptr<keyset> ptr;
	public:
		static std::shared_ptr<keyset> get_instance();
	};


	//token对象应该包含
	class token {
		enum class Type {
			IDENTIFIER,
			KEYWORD,
			NUMBER,
			STRING,
			SPACE
		};
	private:
		token() {}
		token(Type type, std::string value)	{
			std::set<std::string>::const_iterator key_end = keyset_instance::get_instance()->end();
			if (type == Type::IDENTIFIER) {
				char first_char = value.at(0);
				if (first_char >= '0'&&first_char <= '9')
					type = Type::NUMBER;
				else
					if (keyset_instance::get_instance()->find(value) != key_end) {
						type = Type::KEYWORD;
					}
			}
		}

	private:
		std::string value;
		Type type;
	};


#endif