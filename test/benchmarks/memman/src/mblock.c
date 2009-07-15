/*
 * mblock.c
 *
 *  Created on: 31.03.2009
 *      Author: budweg
 */

#include"mblock.h"
#include"module_config.h"
#include"defs.h"

TCOptr	tcoMBlockAllocate(TCOmblock* self, TCOuint32 nbytes)
{
	TCOptr result = 0;

	//tcoAssert(self);
	tcoAssert(nbytes);

#if MEM_ALIGN
	{
		TCOuint32 mask = MEM_ALIGN - 1;
		tcoAssert(tcoIsPowerOf2(MEM_ALIGN));
		if (nbytes & mask) nbytes = (nbytes & ~mask) + MEM_ALIGN;
		tcoAssert((nbytes & mask) == 0);
	}
#endif

	if (nbytes <= self->nbytes - (self->freeptr - self->memory))
	{
		result = self->freeptr;
		self->freeptr += nbytes;
	} else {
		if (self->nbytes < nbytes)
		{
			tcoError("nbytes requested > block mem size");
		}
	}
	return result;
}

TCOvoid tcoMBlockShow(TCOmblock* self)
{
	fprintf(stderr, "tcoMBlockShow(%08x): next:%08x, bytes free: %i of %i, mem: %08x\n",
			self, self->next, self->nbytes - (self->freeptr - self->memory), self->nbytes, self->memory);
}




