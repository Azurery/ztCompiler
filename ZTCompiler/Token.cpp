#include "Token.h"
namespace ZTCompiler {
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