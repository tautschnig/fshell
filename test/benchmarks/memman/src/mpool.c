#include"mpool.h"
#include"module_config.h"
#include"types.h"
#include"defs.h"

/* req1: pool muß wissen, wieviel noch frei ist
 */

TCOmpool*	tcoMPoolCreate(TCOuint32 nblocks, TCOuint32 blocksize)
{
	TCOmpool* result;

	tcoAssert(nblocks);
	tcoAssert(blocksize);

#if MEM_ALIGN
	{
		TCOuint32 mask = MEM_ALIGN - 1;
		tcoAssert(tcoIsPowerOf2(MEM_ALIGN));
		if (blocksize & mask) blocksize = (blocksize & ~mask) + MEM_ALIGN;
		tcoAssert((blocksize & mask) == 0);
	}
#endif

	{ /* create pool and allocate memory */
		result = tcoMalloc(sizeof(TCOmpool));						/* malloc for the pool */
		result->blocks = tcoMalloc(sizeof(TCOmblock) * nblocks);	/* malloc for blocks */
		/* allocate memory for one more block to assert realloc does not to cross boundarys */
		result->memory = tcoMalloc(blocksize * (nblocks + 1));		/* malloc for blockmem */
		result->nblocksfree =
		result->nblocks = nblocks;
		result->blocksize = blocksize;
		result->mutex = tcoMutexNew();
	}

	{ /* link blocks, set their mem */
		TCOuint32 i;
		TCOptr blockmem = result->memory;
		TCOmblock *blocktmp = result->blocks;
		for (i = 0; i < nblocks; i++)
		{
			blocktmp->nbytes = blocksize;
			blocktmp->next = (i == (nblocks - 1)) ? 0 : blocktmp + 1;
			blocktmp->freeptr = blocktmp->memory = blockmem;

			blockmem += blocksize;
			blocktmp++;
		}
	}

	return result;
}

TCOmblock*	tcoMPoolReserveBlock(TCOmpool* self)
{
	TCOmblock* result;

  __CPROVER_assume(self);
	tcoAssert(self);

	tcoMutexLock(self->mutex);
		result = self->blocks;

		if (result)
		{
			self->blocks = result->next;	/* get first block from free list */
			result->next = NULL;
			tcoAssert(result->freeptr == result->memory);
			self->nblocksfree--;	/*r1*/
		} else {
			tcoError("pool out of blocks");
		}
	tcoMutexUnlock(self->mutex);

	return result;
}


TCOmblock*	tcoMPoolReleaseBlock(TCOmpool* self, TCOmblock* block)
{
	TCOmblock* result;

	tcoAssert(self);
	tcoAssert(block);

	tcoMutexLock(self->mutex);
		result = block->next;

		if (block->nbytes == self->blocksize)
		{
			block->freeptr = block->memory;		/* "free" memory in block */
			block->next = self->blocks;			/* add block to block list in pool */
			self->blocks = block;
			self->nblocksfree++;	/*r1*/
		} else {
			tcoWarn("block does not belong to pool");
		}
	tcoMutexUnlock(self->mutex);

	return result;
}

TCOvoid tcoMPoolShow(TCOmpool* self)
{
	TCOmblock* block;

	tcoAssert(self);

	tcoMutexLock(self->mutex);
		fprintf(stderr, "tcoPoolShow(%08x): nblocks: %u, blocksize: %u, free blocks:\n",
				self, self->nblocks, self->blocksize);

		block = self->blocks;
		while (block)
		{
			tcoMBlockShow(block);
			block = block->next;
		}
	tcoMutexUnlock(self->mutex);

	fprintf(stderr, "\n\n");
}

TCOuint32 tcoMPoolGetBytesLeft(TCOmpool* self)	/*r1*/
{
	tcoAssert(self);
	return self->nblocksfree * self->blocksize;
}


