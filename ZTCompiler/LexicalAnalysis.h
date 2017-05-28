#ifndef _ZT_LECICAL_ANALYSIS_H_
#define _ZT_LECICAL_ANALYSIS_H_
#include "Token.h"
#include <string>
#include <vector>
#include <cassert>
#include <stack>
#include <unordered_map>
namespace ztCompiler {
	//此法分析器其实是一个DNF
	class LexicalAnalysis {
	private:
		//状态机的输入
		enum class StateType {
			START,	//START作为状态机的初始状态
			IDENTIFIER,	//标识符状态
			COMMENT,//在START状态下读到/*将使状态转换为COMMENT
			STRING,	//字符状态
			END		//状态机的接受状态
		};
	protected:
		void skip_white_space();
		void scan_string(std::string& out);//用于处理字符串
		void scan_number(std::string& out);	//用于处理数字
		void scan_identifier(std::string& out);//用于处理标识符
		bool contains();	 //用于判断字符是否属于某个集合
	private:
		size_t line_num;	 //用于记录当前读取的token的行号
		std::string::const_iterator cur;	//用于记录下一个待读取的token的位置
		StateType state_type;	 //状态机的各种状态
		TokenType token_type;//单词类型
		std::string str;
	public:
		token get_next_token();
		//返回所有的tokens
		std::vector<token> get_tokens();
	};
}
#endif
