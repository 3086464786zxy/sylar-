#ifndef __SYLAR_MACRO_H__ 
#define __SYLAR_MACRO_H__ 

#include <string.h> 
#include<assert.h>
#include "util.h"

//检测编译器
#if defined __GNUC__ || defined __llvm__ 
//上面成功几率大， 下边几率小 __builtin_expect(!!(x), 1)第二个参数代表预期值，UNLICKLY预期值为0, 代表前面条件为false的几率大
#	define SYLAR_LICKLY(x)		__builtin_expect(!!(x), 1) 
#	define SYLAR_UNLICKLY(x)	__builtin_expect(!!(x), 0) 
#else 
#	define SYLAR_LICKLY(x)		(x) 
#	define SYLAR_UNLICKLY(x)    (x)
#endif

//断言很小几率触发, UNLICKLY
#define SYLAR_ASSERT(x) \
	if (SYLAR_UNLICKLY(!(x))) { \
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
			<< "\nbacktrace:\n" \
			<< sylar::BacktraceToString(100, 2, "    "); \
		assert(x);\
	} 

#define SYLAR_ASSERT2(x, w) \
	if (SYLAR_UNLICKLY(!(x))) { \
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
			<< '\n' << w  \
			<< "\nbacktrace:\n" \
			<< sylar::BacktraceToString(100, 2, "    "); \
		assert(x); \
	}

#endif 
