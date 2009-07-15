/**
 *	\file		mpool.h
 *	\brief		memory pool provides mblocks to mman.
 *	\author		boris.budweg@diehl-aerospace.de
 *	\date		2009/04/02
*/


#ifndef POOL_H_
#define POOL_H_

#include"types.h"
#include"mblock.h"

/** type of memory pool */
typedef struct
{
	TCOmblock	*blocks;		/**< pointer to block array */
	TCOuint32	nblocks;		/**< number of blocks in array */
	TCOuint32	nblocksfree;	/**< number of free blocks in array */
	TCOuint32	blocksize;		/**< size of memory per block */
	TCOptr		memory;			/**< pointer to memory containing all blocks */
	TCOuint32	mutex;			/**< mutex for pool operations */
} TCOmpool;

/** create a memory pool.
 * @param[in]	nblocks		number of blocks to create
 * @param[in]	blocksize	size of memory per block
 * @return		pointer to memory pool
 */
TCOmpool*	tcoMPoolCreate(TCOuint32 nblocks, TCOuint32 blocksize);

/** get a block from memory pool.
 * @param[in]	self	self
 * @return		memory block
 */
TCOmblock*	tcoMPoolReserveBlock(TCOmpool* self);

/** return a block to memory pool.
 * @param[in]	self	self
 * @param[in]	block	block to release
 * @return		next pointer of released block, -1 if block was not from pool self
 */
TCOmblock*	tcoMPoolReleaseBlock(TCOmpool* self, TCOmblock* block);

/** show contents of pool.
 * @param[in]	self	self
 */
TCOvoid tcoMPoolShow(TCOmpool* self);

/** get amount of bytes left in pool.
 * @param	self	self
 * @return 	number of bytes free
 */
TCOuint32 tcoMPoolGetBytesLeft(TCOmpool* self);

#endif /* POOL_H_ */
