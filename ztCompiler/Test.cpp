/*!
 * \file   Test.cpp
 *
 * \author Magicmanoooo
 * \mail   zt838713968@gmail.com
 * \date   2018/09/11 
 *
 * \brief  
 *
 * TODO:   long description
 *
 * // Copyright (c) 2018 The ZTCompiler Authors. All rights reserved.
 * // Use of this source code is governed by a BSD-style license that can be
 * // found in the LICENSE file. See the AUTHORS file for names of contributors.
 * 
 */

#include "Token.h"
#include "Scanner.h"

using namespace ztCompiler;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format)\
	do{\
		test_count++;\
		if(equality)\
			test_pass++;\
		else{\
			fprintf(stderr, "%s:%d: expect: " format " , actual: " format "\n", __FILE__, __LINE__, expect, actual);\
			main_ret = 1;\
		}\
	} while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) ==(actual), expect, actual, "%.17g")
#define EXPECT_EQ_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false", "%s")
#define EXPECT_EQ_FALSE(actual) EXPECT_EQ_BASE((actual) ==0, "false", "true", "%s")
#define EXPECT_EQ_STRING(expect, actual)\
	EXPECT_EQ_BASE(strcmp(expect, actual.data())==0, expect, actual.data(), "%s")


#define TEST_PUNCTUATOR_HELPER(input)\
	do{\
		std::string& str = std::string(input);\
		scanner s(str);\
		auto ret = s.scan();\
		EXPECT_EQ_TRUE(ret->is_punctuator());\
	} while(0)

#define TEST_KEYWORD_HELPER(input)\
	do{\
		std::string& str = std::string(input);\
		scanner s(str);\
		auto ret = s.scan();\
		EXPECT_EQ_TRUE(ret->is_keyword());\
	} while(0)

#define TEST_IDENTIFIER_HELPER(input)\
	do{\
		std::string& str = std::string(input);\
		scanner s(str);\
		auto ret = s.scan();\
		EXPECT_EQ_TRUE(ret->is_identifier());\
	} while(0)

static void test_parse_punctuator() {
	TEST_PUNCTUATOR_HELPER("  ++");
	TEST_PUNCTUATOR_HELPER("--");
}

static void test_parse_keyword() {
// 	TEST_KEYWORD_HELPER("void");
// 	TEST_KEYWORD_HELPER("int");
//	TEST_KEYWORD_HELPER("sjid");
// 	TEST_KEYWORD_HELPER("static");
// 	TEST_KEYWORD_HELPER("");
}

static void test_parse_identifier() {
	TEST_IDENTIFIER_HELPER("SJJ");
}
static void test_parse() {
//	test_parse_LITERAL();
//	test_parse_keyword();
	test_parse_identifier();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass*100.0 / test_count);
	return 0;
}