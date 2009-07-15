/*
 * environment.h
 *
 *  Created on: 30.03.2009
 *      Author: budweg
 */

#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_


#ifdef CFG_PCDESK
	#define OS_WINDOWS
#endif

#ifdef CFG_PGM
	#define OS_LINUX
	#define ABST_POSIX
#endif



#endif /* ENVIRONMENT_H_ */
