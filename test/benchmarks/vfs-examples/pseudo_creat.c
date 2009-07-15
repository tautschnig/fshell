
/*
 * $Author: muehlber $ : $RCSfile: pseudo_creat.c,v $
 * $Revision: 1.11 $, $Date: 2007/11/09 17:51:15 $
 */

/* sys_creat is actually a specific behaviour of sys_open() */
int sys_creat (string path)
 {
  lookup_res_t l;
  inode_t itmp;
  dentry_t parent, file;

  l = path_lookup (path);
  parent = *l.parent;
  file = *l.file;

  if (!parent.is_allocated)
   {
    if (file.is_allocated) /* deals with root look up */
     { dput(file); }
    return (ERROR);
   }

  down (parent.d_inode->i_mutex);

  if (file.is_allocated && !is_directory (file))
   { up (parent.d_inode->i_mutex);
     path_release (file);
     return (SUCCESS); }
  if (file.is_allocated && is_directory (file))
   { up (parent.d_inode->i_mutex);
     path_release (file);
     return (ERROR); }

  spin_lock (dcache_lock);

  file = allocate_dentry(last_component(path), parent);
  if (!file.is_allocated)
   { spin_unlock (dcache_lock);
     up (parent.d_inode->i_mutex);
     dput (parent);
     return (ERROR); }
  dget (file);

  spin_lock (inode_lock);
  itmp = allocate_inode(file);
  file.d_inode = &itmp;
  spin_unlock (inode_lock);
  if (!file.d_inode->is_allocated)
   { atomic_write (file.d_count, 0);
     dput (parent);
     spin_unlock (dcache_lock);
     up (parent.d_inode->i_mutex);
     return (ERROR); }

  update_parent (*((dentry_t *)file.d_parent));
  path_release (file);
  spin_unlock (dcache_lock);

  up (parent.d_inode->i_mutex);

  return (SUCCESS);
 }

