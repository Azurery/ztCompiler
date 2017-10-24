#ifndef _ZT_ERROR_H_
#define _ZT_ERROR_H_
namespace ztCompiler {
	class token;
	class statement;
	
	template<typename...Types>
	void throw_error(const char* info,Types...);

	template<typename...Types>
	void throw_error(const token* tok,const char* info,Types...);

	template<typename...Types>
	void throw_error(const statement* state, const char* info, Types...);

}


#endif