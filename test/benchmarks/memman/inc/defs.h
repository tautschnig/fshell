/**
 *	\file		defs.h
 *	\brief		common definitions and includes.
 *	\author		boris.budweg@diehl-aerospace.de
 *	\date		2009/04/02
*/


#ifndef DEFS_H_
#define DEFS_H_

#include"stdio.h"
#include"stdlib.h"
#include"memory.h"

#define tcoMemcpy(dst, src, n)	memcpy(dst, src, n)
#define tcoMalloc(n)			malloc(n)

#define tcoMutexNew() 		(0)
#define tcoMutexLock(m)
#define tcoMutexUnlock(m)

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#define tcoError(s)		\
{\
	fprintf(stderr, "error '%s' @ %s:%i\n", s, __FILE__, __LINE__);\
  /*assert(0);*/\
	exit(1);\
}

#define tcoWarn(s)		\
{\
	fprintf(stderr, "warning '%s' @ %s:%i\n", s, __FILE__, __LINE__);\
}

#ifndef tcoAssert
#define tcoAssert(x)	{if (!(x)) tcoError(#x);}
//#define tcoAssert(x) assert(x)
#endif

#define tcoIsPowerOf2(x) ( ((x) & ((x) - 1) ) == 0 )


#endif /* DEFS_H_ */
