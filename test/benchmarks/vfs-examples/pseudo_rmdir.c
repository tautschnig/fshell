
/*
 * $Author: muehlber $ : $RCSfile: pseudo_rmdir.c,v $
 * $Revision: 1.17 $, $Date: 2008/04/21 11:49:56 $
 */

int sys_rmdir (string path)
 {
  lookup_res_t l;
  dentry_t parent, dir, tmp;
  int children = 0, wait_done = 0;

  l = path_lookup (path);
  parent = *l.parent;
  dir = *l.file;

  /* 1. -- sanity checks */
  if (!dir.is_allocated || !is_directory(dir) || !parent.is_allocated)
   { dput (dir); dput (parent); return (ERROR); }

  /* 2. -- lock the node */
  down (dir.d_inode->i_mutex);
  spin_lock (dcache_lock); /* d_delete () */

  /* 3. -- check for subdirectories
   * (needs i_mutex and dcache_lock in order to avoid others changing
   * the state) */
  foreach ("tmp in /list of dentries/");
   {
    if (((dentry_t *)(tmp.d_parent))->id == dir.id &&
        atomic_read (tmp.d_count))
     { children++; }
   }

  if (children != 0)
   {
    spin_unlock (dcache_lock);
    up (dir.d_inode->i_mutex);
    dput (dir); dput (parent); /* no path_release because another process could
                                * have already destroyed the parent relation */
    return (ERROR);
   }

  /* 4. -- mark the node for deletion
   * (uses d_inode->i_mutex and d_inode->i_state) */
  if (dir.d_inode->i_state & DELETING)
   { /* somebody else is already deleting this node -- success */
    dput (dir); dput (parent); /* no path_release because another process
                                * has already destroyed the parent relation */
    spin_unlock (dcache_lock);
    up (dir.d_inode->i_mutex);
    return (SUCCESS);
   }
   else
   { /* we are going to delete this node */
    dir.d_inode->i_state |= DELETING;
    up (dir.d_inode->i_mutex);
   }

  /* ==> dir.d_inode->i_mutex is free now */

  /* 5. -- now remove links to this node so that later path_lookup()s won't
   * return it and we don't get any new processes working on this node */
  dir.d_parent = &dNULL;
  update_parent (parent); /* This can result in clients unsing invalid
                           * working directories. This is okay. */
  dput (parent);
  spin_unlock (dcache_lock);

  /* ==> dcache_lock is free now */
  /* ==> other processes may continue working on this directory here */

  /* 6. -- set dir.d_inode->i_count and dir.d_count to 0
   * as soon as we are the only user of that link */
  while (!wait_done) /* dentry_iput */
   {
    spin_lock (dcache_lock);
    spin_lock (dir.d_lock);
    if (atomic_read(dir.d_count) == 2) /* are we the only user */
     {
      /* inode removal */
      /* remark for Radu: in the absence of hard and symbolic links,
       * this boils down to atomic_write (dir.d_inode->i_count, 0); */
      spin_lock (dir.d_inode->i_lock);
      /* Just decrement. Since there is still at least one dentry pointing to
       * the inode i_count must be >= 1 */
      atomic_write (dir.d_inode->i_count,
                    atomic_read(dir.d_inode->i_count) - 1);
      /* This is handling for hardlinks: if there is still another dentry
       * pointing to the node, we remove the deleting flag. */
      if (atomic_read(dir.d_inode->i_count) >= 1)
       { dir.d_inode->i_state ^= DELETING; }
      spin_unlock (dir.d_inode->i_lock);

      /* scheduling dentry for removal (d_delete()) */
      atomic_write(dir.d_count, 0);
     }

    /* this is not thread-save in C semantics. wait_done has to be
     * stored per thread. But the same goes for dir and parent... */
    if (atomic_read(dir.d_count) == 0)
     { wait_done = 1; }

    spin_unlock (dir.d_lock);
    spin_unlock (dcache_lock);
   }

  return (SUCCESS);
 }

