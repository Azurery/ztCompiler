#include <set>
#include <string>
#include <memory>
namespace ZTCompiler {
		//定义c语言的key word
		enum class keyword {
			BREAK,
			CASE,
			CHAR,
			CONST,
			CONTINUE,
			DEFAULT,
			DO,
			WHILE,
			DOUBLE,
			IF,
			ELSE,
			FOR,
			INT,
			LOMG,
			RETURN,
			SHORT,
			STRUCT,
			SWITCH,
			UNION,
			TYPEDEF,
			VOID
		};
		using keyset = std::set<std::string>;
		class keysetcase {
		private:
			static std::shared_ptr<keyset> ptr;
		public:
			static std::shared_ptr<keyset> get_instance();
		};


	//token对象应该包含
	class token {
		enum class type {
			KEYWORD,
			NUMBER,
			STRING,
			IDENTIFIER,
			SPACE
		};
		
		
	};
}
