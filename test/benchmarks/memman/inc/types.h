/**
 *	\file		types.h
 *	\brief		basic types for InteCo.
 *	\author		boris.budweg@diehl-aerospace.de
 *	\date		2009/04/02
*/


#ifndef TYPES_H_
#define TYPES_H_


	typedef void 				TCOvoid;
	typedef unsigned char		TCOboolean;
	typedef unsigned long		TCObitfield;
	typedef unsigned long		TCOenum;
	typedef signed char			TCOint8;
	typedef unsigned char 		TCOuint8;
	typedef signed short		TCOint16;
	typedef unsigned short		TCOuint16;
	typedef signed long			TCOint32;
	typedef unsigned long		TCOuint32;

#ifdef _MSC_VER								/* Micro$oft-Patch */
	typedef signed __int64		TCOint64;
	typedef unsigned __int64	TCOuint64;
#else
	typedef signed long long	TCOint64;
	typedef unsigned long long	TCOuint64;
#endif

	typedef float				TCOfloat32;
	typedef double				TCOfloat64;
/*	typedef long double			TCOfloat96;*/

//	typedef signed int					TCOid;

	typedef char*				TCOptr;


#endif /* TYPES_H_ */
