#include "tco.h"

void memman() {
  TCOmpool * _pool = 0;
  TCOmman * _mman = 0;
  TCOptr _ptr;
  unsigned __nblocks;
  unsigned __blocksize;
  unsigned __nalloc;
  unsigned _nblocks = __nblocks;
  unsigned _blocksize = __blocksize;
  unsigned _nalloc = __nalloc;

  __CPROVER_assume(_nblocks > 0 && _nblocks < 4);
  __CPROVER_assume(_blocksize >= 24);
  __CPROVER_assume(_nalloc > 0 && _nalloc <= (_nblocks * _blocksize));

  _pool = tcoMPoolCreate(_nblocks, _blocksize);
  _mman = tcoMmanCreate(_pool);
  _ptr = tcoMmanAllocate(_mman, _nalloc);
  assert(tcoMPoolGetBytesLeft(_pool) == (_nblocks -1) * _blocksize);
  tcoMmanDelete(_mman);
}

