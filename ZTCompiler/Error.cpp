#include "Error.h"
#include "AST.h"
#include "Token.h"
#include <string>
#include <cstdio>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MARGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[31m"

extern std::string program;

void throw_error(const char* format) {
	fprintf(stderr, "\n");
}
//sprintf(RED "RED " L_RED "L_RED\n" NONE);
template<typename...Args>
void throw_error(const char* format, Args...args) {
	fprintf(stderr, "%s:" ANSI_COLOR_RED "error: " ANSI_COLOR_RESET,
		program.c_str());
	throw_error(++format, args...);
	exit(-1);
}