#include "Token.h"
namespace ztCompiler {
	//symbol table
	const std::unordered_map<std::string, TokenAttr> keyword_map{
			{ "char",TokenAttr::CHAR },
			{ "int",TokenAttr::INT },
			{ "float",TokenAttr::FLOAT },
			{ "char",TokenAttr::CHAR },
			{ "short",TokenAttr::SHORT },
			{ "double",TokenAttr::DOUBLE },
			{ "long",TokenAttr::LONG },
			{ "else",TokenAttr::ELSE },
			{ "int",TokenAttr::INT },
			{ "signed",TokenAttr::SIGNED },
			{ "unsigned",TokenAttr::UNSIGNED },
			{ "break",TokenAttr::BREAK },V
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

}