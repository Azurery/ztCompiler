#pragma once
#ifndef _ZT_PARSE_H_
#define _ZT_PARSE_H_
#include <string>
#include <vector>
#include <set>
#include <memory>
namespace ztCompiler {
	//statement的base class
	enum class statement_catagory {
		IF_STATEMENT,
		WHILE_STATEMENT,
		FOR_STATEMENT,
		ARITHMETIC_STATEMENT,
		DEFINE_STATEMENT,
		ASSGIN_STATEMENT
	};
	class statement_node {
	public:
		typedef std::shared_ptr<statement_node> ptr;
		std::vector<ptr> node;
		statement_catagory catagory;
	};
	class base_node{
		std::set<char> first_set;
		std::set<char> follow_set;
		std::vector<char> terminal;	//终结符
		std::vector<char> nonterminal;//非终结符
	public:
		int get_terminal_index(char target);	//获取在终结符集合中的下标
		int get_nonterminal_index(char target); //获取在非终结符集合中的下标
		void get_first(char target);	//得到first集合
		void get_follow(char target);	//得到follow集合
		void resolve();		//处理得到的first和follow集合
	};

}
#endif
