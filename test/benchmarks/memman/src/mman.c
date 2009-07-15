#include"mman.h"
#include"mpool.h"
#include"types.h"
#include"defs.h"

TCOmman*	tcoMmanCreate(TCOmpool* mpool)
{
	TCOmblock* block;
	TCOmman* result;

	tcoAssert(mpool);
	tcoAssert(mpool->blocksize >= sizeof(TCOmman)); /* blocks must be able to hold blockman */

	block = tcoMPoolReserveBlock(mpool);	/* get a block to place self into */
	result = (TCOmman*) tcoMBlockAllocate(block, sizeof(TCOmman));
  result->nbytes = block->nbytes; /*r1*/

	result->block = block;					/* register block as first block of blockman */
	result->nallocs = result->nfrees = 0;
	result->pool = mpool;
	result->mutex = tcoMutexNew();

	return result;
}

TCOptr			tcoMmanAllocate(TCOmman* self, TCOuint32 nbytes)
{
	TCOptr result;

	tcoAssert(self);
	tcoAssert(nbytes);

	tcoMutexLock(self->mutex);
		result = tcoMBlockAllocate(self->block, nbytes);

		if (!result)  /* if alloc did not succeede, attach fresh block */
		{
			TCOmblock* block = tcoMPoolReserveBlock(self->pool);
			block->next = self->block;
			self->block = block;
			self->nbytes += block->nbytes;
			result = tcoMBlockAllocate(self->block, nbytes);
			tcoAssert(result);
		}

		self->nallocs++;
	tcoMutexUnlock(self->mutex);

	return result;
}

TCOboolean		tcoMmanFree(TCOmman* self, TCOptr dummy)
{
	tcoAssert(self);
	tcoAssert(dummy);

	tcoMutexLock(self->mutex);
		self->nfrees++;
	tcoMutexUnlock(self->mutex);

	return TRUE;
}


TCOptr			tcoMmanReallocate(TCOmman* self, TCOptr oldmem, TCOuint32 nbytes)
{
	TCOptr result;

	tcoAssert(self);
	tcoAssert(nbytes);

	result = tcoMmanAllocate(self, nbytes);

	if (oldmem) /* dont copy anything if old ptr is NULL */
	{
		/* this is always possible without leaving the block memory beacuse there is always one
		 * unused block at the end of the memory */
		tcoMemcpy(result, oldmem, nbytes);

		tcoMmanFree(self, oldmem);
	}

	return result;

}

TCOboolean		tcoMmanDelete(TCOmman* self)
{
	tcoAssert(self);

	tcoMutexLock(self->mutex);
		while (self->block)		/* release all blocks managed by the blockman */
		{
			self->block = tcoMPoolReleaseBlock(self->pool, self->block);
		}
	tcoMutexUnlock(self->mutex);

	return TRUE;
}

TCOvoid	tcoMmanShow(TCOmman* self)
{
	TCOmblock* block;

	tcoAssert(self);

	tcoMutexLock(self->mutex);
		block = self->block;
		fprintf(stderr, "tcoMBlockmanShow(%p): allocs/frees: %u/%u\n", self, self->nallocs, self->nfrees);
		fprintf(stderr, "pool/block/mutex: %p/%p/%p, used blocks:\n", self->pool, self->block, self->mutex);
		while (block)
		{
			tcoMBlockShow(block);
			block = block->next;
		}
		tcoMPoolShow(self->pool);
	tcoMutexUnlock(self->mutex);
}

TCOuint32 tcoMmanGetSize(TCOmman* self)
{
	tcoAssert(self);
	return self->nbytes;
}

