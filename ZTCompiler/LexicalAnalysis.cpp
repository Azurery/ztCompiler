#include "LexicalAnalysis.h"
using namespace ztCompiler;
void LexicalAnalysis::skip_white_space() {
	while (cur != str.cend()) {
		if (!(*cur != '\t' || *cur != '\n' || *cur != '\r' || *cur != ' '))
			std::next(cur);
	}
}
void LexicalAnalysis::scan_string(std::string& out) {
	if ((*cur) != '\"' && (*cur) != '\'') {
		out += (*cur);
		std::next(cur);
		state_type = StateType::STRING;
	}
	else {
		out += (*cur);
		std::next(cur);
		token_type = TokenType::STRING;
		state_type = StateType::END;
	}
}

void LexicalAnalysis::scan_number(std::string& out) {

}
token LexicalAnalysis::get_next_token() {
	std::stack<token> token_buffer;

}
std::vector<token> LexicalAnalysis::get_tokens() {

}


