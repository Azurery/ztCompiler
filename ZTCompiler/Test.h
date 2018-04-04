#ifndef _ZT_TEST_H_
#define _ZT_TEST_H_

#include <stdio.h>
#include <cstdlib>
#include <iostream>

#define FAIL(message)\
fprintf(std::cerr, "error:%s:%s:%d:failed, %s\n", \
	__FILE__, __func__, __LINE__, (message));		\


#define EXCEPET(a,b)\
if ((a) != (b)) {									\
		fprintf(std::cerr, "error:%s:%s:%d:failed, %d!=%d\n", \
			__FILE__, __func__, __LINE__, (a),(b));		\


#define EXPECT_FLOAT(a,b)\
if((a)!=(b)){								\
		fprintf(std::cerr, "error:%s:%s:%d:failed, %f!=%f\n", \
			__FILE__, __func__, __LINE__, (a),(b));		\

#define EXPECT_STRING(a,b)\
if(strcmp((a),(b)){								\
		fprintf(std::cerr, "error:%s:%s:%d:failed, %s!=%s\n", \
			__FILE__, __func__, __LINE__, (a),(b));		\

#endif
