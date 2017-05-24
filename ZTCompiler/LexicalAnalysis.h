#ifndef _ZT_LECICAL_ANALYSIS_H_
#define _ZT_LECICAL_ANALYSIS_H_
#include <string>

namespace ztCompiler {
	class LexicalAnalysis {
	private:
		enum class State {
			START,	//START作为状态机的初始状态
			IDENTIFIER,	
			SIGN,
			COMMENT,//在START状态下读到/*将使状态转换为COMMENT
			SPACE,
			STRING,
			REGEX
		};
	};
}
#endif
