#include "Token.h"
namespace ztCompiler {

	token::token(TokenType type, const std::string& value) {
		if (type == TokenType::IDENTIFIER) {
			char first_char = value.at(0);
			if (first_char >= '0'&&first_char <= '9')
				type = TokenType::NUMBER;
			else
				if (keyset_instance::get_instance()->find(value) != keyset_instance::get_instance()->end()) {
					type = TokenType::KEYWORD;
				}
		}
	}

	token::token(TokenType type, const std::string&& value) {
		
	}
	std::shared_ptr<keyset> keyset_instance::ptr = nullptr;
	std::shared_ptr<keyset> keyset_instance::get_instance() {
		if (!ptr) {
			ptr->insert("if");
			ptr->insert("else");
			ptr->insert("when");
			ptr->insert("while");
			ptr->insert("return");
			ptr->insert("int");
			ptr->insert("double");
			ptr->insert("string");
			ptr->insert("float");
			ptr->insert("long");
			ptr->insert("union");
			ptr->insert("typedef");
		}
		return ptr;
	}
}