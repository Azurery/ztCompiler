#include "Parser.h"
namespace ztCompiler {
	int base_node::get_terminal_index(char target){
		for (int i = 0; i < terminal.size(); ++i) {
			if (target == terminal[i])
				return i;
		}
		return -1;
	}

	int base_node::get_nonterminal_index(char target) {
		for (int i = 0; i < nonterminal.size(); ++i) {
			if (target ==nonterminal[i])
				return i;
		}
		return -1;
	}

	void base_node
}