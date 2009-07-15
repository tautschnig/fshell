/**
 *	\file		mman.h
 *	\brief		memory manager to manage mblocks from mpools.
 *	\author		boris.budweg@diehl-aerospace.de
 *	\date		2009/04/02
*/


#ifndef BLOCKMAN_H_
#define BLOCKMAN_H_

#include"mblock.h"
#include"mpool.h"

/** type of memory block manager */
typedef struct _sTCOmman
{
	TCOmpool*	pool;		/**< pool where the blocks reside */
	TCOmblock*	block;		/**< current block (others will be linked in block->next) */
	TCOuint32	nbytes;		/**< number of bytes of all blocks in list */
	TCOuint32	nallocs;	/**< Allocate-counter for statistics */
	TCOuint32	nfrees;		/**< Pseudofree-counter for statistics */
	TCOuint32	mutex;		/**< mutex for blockman operations */
} TCOmman;

/** creates a memory block manager.
 * @param[in]	mpool	existing memory pool this blockman shall work with
 * @return		memory block manager
 */
TCOmman*	tcoMmanCreate(TCOmpool* mpool);

/** allocate memory from the block manager.
 * @param[in]	self	self
 * @param[in]	nbytes	number of bytes to allocate
 * @return		pointer to allocated bytes (will only fail if pool exhausted)
 */
TCOptr			tcoMmanAllocate(TCOmman* self, TCOuint32 nbytes);

/** reallocate memory from the block manager.
 * @param[in]	self	self
 * @param[in]	oldmem	old memory pointer
 * @param[in]	nbytes	number of bytes to allocate
 * @return		pointer to allocated bytes (will only fail if pool exhausted)
 */
TCOptr			tcoMmanReallocate(TCOmman* self, TCOptr oldmem, TCOuint32 nbytes);

/** "free" an allocated area.
 * @param[in]	self	self
 * @param[in]	dummy	pointer to allocated area to free
 * @return		false if adress is within the blocks of the block manager
 * actually only increments nallocs, no freeing of anything.
 */
TCOboolean		tcoMmanFree(TCOmman* self, TCOptr dummy);

/* self-destruction.
 * @param[in]	self	self
 * @return		true
 * deletes its blocks as well, returning the memory to the pool.
 */
TCOboolean		tcoMmanDelete(TCOmman* self);

/** show content.
 * @param[in]	self	self
 */
TCOvoid	tcoMmanShow(TCOmman* self);

/** get combined size of memory in reserved blocks.
 * @param[in]	self	self
 * @return amount of bytes
 */
TCOuint32 tcoMmanGetSize(TCOmman* self);

#endif /* BLOCKMAN_H_ */
