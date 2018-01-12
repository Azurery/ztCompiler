#include "Code_generation.h"

/*			|Microsoft		|							|
			|x64 calling	|							|
			|convention[14]	|Windows					|RCX/XMM0,		32 bytes shadow space on stack. 
			|				|(Microsoft Visual C++,		|RDX/XMM1,		The specified 8 registers can only be 
			|				|GCC,						|R8/XMM2,		used for parameters 1 through 4.
			|				|Intel C++ Compiler,		|R9/XMM3		For C++ classes, the hidden "this"
			|				|Delphi),					|				parameter is the first parameter, 
			|				|UEFI						|				and is passed in RCX [22].
			|				|							|																							
	x86-64	|				|Windows					|RCX/XMM0,
			|	vectorcall	|(Microsoft Visual C++)		|RDX/XMM1, 
			|				|							|R8/XMM2, 
			|				|							|R9/XMM3 + 
			|				|							|XMM0-XMM5/YMM0-YMM5
			|				|							|		
			|System V		|							|
			|AMD64 ABI[19]	|Solaris,					|RDI, RSI, RDX,		Caller	Stack aligned on 16 bytes boundary. 
			|				|Linux,						|RCX, R8, R9,		128 bytes red zone below stack. 
			|				|BSD, OS X					|XMM0¨C7				The kernel interface uses RDI,
			|				|GCC, Intel C++ Compiler)	|					RSI, RDX, R10, R8 and R9
*/
														 
																																						
static std::vector<const char*> registers = {
	"%rdi",	"%rsi",	"rdx",
	"rcx",	"r8",	"r9"
};

static std::vector<const char*> sse_registers{
	"xmm0",	"xmm1",	"xmm2",	"xmm3",
	"xmm4", "xmm5",	"xmm6",	"xmm7"
};

