#include "Token.h"
namespace ztCompiler {
	const std::unordered_map<std::string, TokenAttr> keyword_map{
			{ "char",TokenAttr::CHAR },
			{ "int",TokenAttr::INT },
			{ "float",TokenAttr::FLOAT },
			{ "void",TokenAttr::VOID },
			{"if",TokenAttr::IF}
		};

}