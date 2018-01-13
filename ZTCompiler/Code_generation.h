#ifndef _ZT_CODE_GENERATION_H_
#define _ZT_CODE_GENERATION_H_
#include "AST.h"

class parser;
class token;

namespace ztCompiler {
	class generator : public visitor {
		//System V的参数传递方式
	public:
		enum class Parameter_passing {
			INTEGER,
			SSE,
			SSE_UP,
			X87,
			X87_UP,
			COMPLEX_87,
			MEMORY,
			NO_CLASS
		};
	public:

	protected:
		void emit(std::string& str){}

		static Parameter_passing classify(type* parameter_type_);
		void generate_comma_operator(binary_expression* comma_);
		void generate_member_reference_operator(binary_expression* binary_operator_);


	private:


	};
	
}



#endif
