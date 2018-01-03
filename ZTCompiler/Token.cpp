#include "Token.h"
namespace ztCompiler {
	//symbol table
	const std::unordered_map<std::string, TokenAttr> token::keyword_table{
			{ "char",TokenAttr::CHAR },
			{ "int",TokenAttr::INT },
			{ "switch",TokenAttr::SWITCH },
			{ "float",TokenAttr::FLOAT },
			{ "if",TokenAttr::IF },
			{ "char",TokenAttr::CHAR },
			{ "short",TokenAttr::SHORT },
			{ "double",TokenAttr::DOUBLE },
			{ "long",TokenAttr::LONG },
			{ "else",TokenAttr::ELSE },
			{ "int",TokenAttr::INT },
			{ "signed",TokenAttr::SIGNED },
			{ "unsigned",TokenAttr::UNSIGNED },
			{ "break",TokenAttr::BREAK },
			{ "bool",TokenAttr::BOOL },
			{ "case",TokenAttr::CASE },
			{ "complex",TokenAttr::COMPLEX },
			{ "const",TokenAttr::CONST },
			{ "restrict",TokenAttr::RESTRICT },
			{ "do",TokenAttr::DO },
			{"atomic",TokenAttr::ATOMIC},
			{ "volatile",TokenAttr::VOLATILE },
			{ "continue",TokenAttr::CONTINUE },
			{ "default",TokenAttr::DEFAULT },
			{ "while",TokenAttr::WHILE },
			{ "union",TokenAttr::UNION },
			{ "typedef",TokenAttr::TYPEDEF },
			{ "void",TokenAttr::VOID },
			{ "goto",TokenAttr::GOTO },
			{ "extern",TokenAttr::EXTERN },
			{ "auto",TokenAttr::AUTO },
			{ "static_assert",TokenAttr::STATIC_ASSERT },
			{ "thread_local",TokenAttr::THREAD_LOCAL },
	};

	const std::unordered_map<TokenAttr, const char*> lexical_table{
		{TokenAttr::INT,"int"},
		{TokenAttr::SHORT,"short"},
		{TokenAttr::FLOAT,"float"},
		{TokenAttr::UNSIGNED,"unsigned"},
		{TokenAttr::LONG,"long"},
		{TokenAttr::DOUBLE,"double"},
		{TokenAttr::CHAR,"char"},
		{TokenAttr::BOOL,"bool"}
	};

}