
/*
 * $Author: muehlber $ : $RCSfile: pseudo_rename.c,v $
 * $Revision: 1.9 $, $Date: 2007/08/03 13:43:06 $
 */

int sys_rename (string src, string dst)
 {
  lookup_res_t l;
  dentry_t src_parent, src_file;
  dentry_t dst_parent = dNULL, dst_file = dNULL;

  l = path_lookup (src);
  src_parent = *l.parent;
  src_file = *l.file;
  
  if (!src_file.is_allocated)
   { goto MVERROR; }

  l = path_lookup (dst);
  dst_parent = *l.parent;
  dst_file = *l.file; 
  if (!dst_parent.is_allocated || src_file.id == dst_file.id)
   { goto MVERROR; }

  if (is_directory(dst_file)) /* target is directory; move file into it */
   { dput (dst_parent);
     dst_parent = dst_file;
     dst_file = get_dentry(last_component(src), dst_parent);
     /* the implementation uses a temporary dentry; but since we
      * probably don't care about filenames at all, I use a string
      * operation. */
     dst = concat (dst, last_component(src)); }

  if (is_directory(dst_file))
   { goto MVERROR; } /* EFAULT */

  if (!is_directory(dst_file) && is_directory(src_file))
   { goto MVERROR; } /* EFAULT */

  if (is_directory(src_file) && atomic_read(src_file.d_count > 2))
   { goto MVERROR; } /* EBUSY */

  /* the implementation follows dst_file.d_parent.d_parent... and
   * checks whether there is a parent that equals src_file */
  if ("the new pathname contained a path prefix of the  old")
   { goto MVERROR; } /* EINVAL; */

  down (dst_parent.d_inode->i_mutex);

  spin_lock (dcache_lock);

  if (dst_file.is_allocated) /* remove dst_file */
   {
    while (atomic_read(dst_file.d_count) != 0) /* dentry_iput */
     {
      spin_lock (dst_file.d_lock);
      if (atomic_read(dst_file.d_count) == 2)
       { atomic_write(dst_file.d_count, 0); }
      spin_unlock (dst_file.d_lock);
     }

     /* there may be a bug in this line; I'm not sure when exactly
      * i_count is decremented or incremented. */
     spin_lock (inode_lock);
     while (atomic_read(dst_file.d_inode->i_count) != 0) /* iput () */
      {
       spin_lock (dst_file.d_inode->i_lock);
       if (atomic_read(dst_file.d_inode->i_count) == 1)
        { atomic_write (dst_file.d_inode->i_count, 0); }
       spin_unlock (dst_file.d_inode->i_lock);
      }
     spin_unlock (inode_lock);

    update_parent (*((dentry_t *)dst_file.d_parent));
    dst_file = dNULL; /* we are done with it */
   }

  /* rename */
  src_file.d_parent = &dst_parent;
  src_file.d_iname = last_component (dst);
  update_parent (src_parent);
  update_parent (dst_parent);
  dput (dst_parent);
  dput (src_parent);
  dput (src_file);

  spin_unlock (dcache_lock);

  up (dst_parent.d_inode->i_mutex);

  return (SUCCESS);

MVERROR:
  dput (dst_file); dput (dst_parent);
  dput (src_file); dput (src_parent);
  return (ERROR);
 }

