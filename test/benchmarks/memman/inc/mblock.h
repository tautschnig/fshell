/**
 *	\file		mblock.h
 *	\brief		memory blocks used by mpool and mman.
 *	\author		boris.budweg@diehl-aerospace.de
 *	\date		2009/04/02
*/

/*\b Usage:

	\verbatim
	\endverbatim
 */


#ifndef BLOCK_H_
#define BLOCK_H_

#include"types.h"

/** type of memory block */
typedef struct _sTCOmblock
{
	TCOuint32	nbytes;		/**< number of bytes / size of the block */
	TCOptr		memory;		/**< pointer to start of memory block */
	TCOptr		freeptr;	/**< pointer to unused section of block */
	struct _sTCOmblock*	next;		/**< next-pointer for linked block */
} TCOmblock;

/** allocate memory from block.
 * @param[in]	self	self
 * @param[in]	nbytes	number of bytes to allocate
 * free block by calling tcoMPoolReleaseBlock()
 */
TCOptr	tcoMBlockAllocate(TCOmblock* self, TCOuint32 nbytes);

/* show contents.
 * @param[in]	self	self
 */
TCOvoid tcoMBlockShow(TCOmblock* self);

#endif /* BLOCK_H_ */
