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
			{ "",TokenAttr::CHAR },
			{ "char",TokenAttr::CHAR },
			{ "char",TokenAttr::CHAR },
			{ "char",TokenAttr::CHAR },
			{ "char",TokenAttr::CHAR },
			{ "char",TokenAttr::CHAR },
			{ "char",TokenAttr::CHAR },
			{ "char",TokenAttr::CHAR },
			{ "char",TokenAttr::CHAR },
			{ "void",TokenAttr::VOID },
			{"if",TokenAttr::IF},
			{}
		};

}