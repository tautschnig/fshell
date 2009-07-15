
/*
 * $Author: muehlber $ : $RCSfile: pseudo_unlink.c,v $
 * $Revision: 1.11 $, $Date: 2008/04/23 17:33:50 $
 */

int sys_unlink (string path)
 {
  lookup_res_t l;
  dentry_t parent, file;
  inode_t inode = iNULL;
  int wait_done = 0;

  l = path_lookup (path);
  parent = *l.parent;
  file = *l.file;

  if (!file.is_allocated || is_directory (file))
   { dput (file); dput (parent); return (ERROR); }

  down (file.d_inode->i_mutex);
  /* there's only permission checking in here; but is it save? I think it
   * could cause corruption if one process progresses faster and 
   * removes the inode already. Locking iNULL is clearly invalid.
   * But I guess there's not much time for investigating on this...
   */
  up (file.d_inode->i_mutex);


  spin_lock (dcache_lock); /* d_delete () */

  if (! file.d_inode->is_allocated) /* may_delete() */
   {
    dput (file);
    dput (parent);
    spin_unlock (dcache_lock);
    return (SUCCESS); /* Actually an error, but "this file is already gone"
                       * is not really an error in our model, right? */
   }

  /* vfs_unlink() / file->i_op->unlink() */
  inode         = *file.d_inode; /* just saving the reference */
  file.d_inode  = &iNULL;
  file.d_parent = &dNULL;
  update_parent (parent);

  spin_unlock (dcache_lock);


  while (! wait_done ) /* d_delete() / dentry_iput() */
   {
    spin_lock (dcache_lock);
    spin_lock (file.d_lock);
    if (atomic_read(file.d_count) == 2)
     { atomic_write(file.d_count, 0);
       wait_done = 1; }
    spin_unlock (file.d_lock);
    spin_unlock (dcache_lock);
   }

  spin_lock (inode.i_lock); /* iput () */
  atomic_write (inode.i_count, atomic_read(inode.i_count) - 1);
  spin_unlock (inode.i_lock);

  dput (parent);

  return (SUCCESS);
 }

