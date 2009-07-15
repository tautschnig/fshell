
/*
 * $Author: muehlber $ : $RCSfile: pseudo_mkdir.c,v $
 * $Revision: 1.10 $, $Date: 2007/08/13 17:22:05 $
 */

int sys_mkdir (string path)
 {
  lookup_res_t l;
  inode_t itmp;
  dentry_t parent, dir;

  l = path_lookup(path);
  parent = *l.parent;
  dir = *l.file;

  if (dir.is_allocated)
   { path_release (dir); return (ERROR); }
  if (!parent.is_allocated)
   {
    if (dir.is_allocated) /* deals with root look up */
     { dput(dir); }
    return (ERROR);
   }

  spin_lock (dcache_lock);
  dir = allocate_dentry(last_component(path), parent);
  if (!dir.is_allocated)
   { spin_unlock (dcache_lock);
     dput(parent);
     return (ERROR); }

  dget(dir);

  spin_lock (inode_lock);
  itmp = allocate_inode(dir);
  dir.d_inode = &itmp;
  if (dir.d_inode->is_allocated) { down(dir.d_inode->i_mutex); }
  spin_unlock (inode_lock);

  if (!dir.d_inode->is_allocated)
   { atomic_write (dir.d_count, 0);
     dput (parent);
     spin_unlock (dcache_lock);
     return (ERROR); }

  dir.d_subdirs  = dir.id;
  update_parent (*((dentry_t *)dir.d_parent));
  path_release (dir);
  spin_unlock (dcache_lock);
  
  up(dir.d_inode->i_mutex);

  return (SUCCESS);
 }

