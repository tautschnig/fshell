
/*
 * $Author: muehlber $ : $RCSfile: pseudo_cd.c,v $
 * $Revision: 1.2 $, $Date: 2007/08/13 17:22:05 $
 */

/* cd () -- change the current working directory. This requires that
 * each process maintains a pointer to the dentry of its own current
 * working directory. */
int cd (string path)
 {
  lookup_res_t l;
  dentry_t parent, file;

  l = path_lookup (path);
  parent = *l.parent;
  file = *l.file;

  dput (parent); /* this is a hack. */ 

  if (!file.is_allocated || is_directory (file))
   { dput (file);
     return (ERROR); }

  if (current != NULL)
   { dput (*current); };

  current = &file;

  return (SUCCESS);
 }

