
/*
 * $Author: muehlber $ : $RCSfile: pseudo_misc.c,v $
 * $Revision: 1.16 $, $Date: 2007/08/14 16:26:35 $
 */

/* Defines: */

#define NULL    (void *) 0

#define DIRTY    1
#define DELETING 2
#define TRUE     1
#define FALSE    0

#define ERROR    0
#define SUCCESS  1

/* Types: */
typedef char * string; /* just in order to remove pointers */

typedef int spinlock_t;

typedef int atomic_t;

typedef int mutex_t;

typedef struct inode_t
 {
  int        is_allocated;
  atomic_t   i_count;
  spinlock_t i_lock;
  int        i_state;
  mutex_t    i_mutex;
 } inode_t;

inode_t iNULL = {0, 0, 0, 0};

typedef struct dentry_t
 {
  int        id; /* in order to avoid random bit shifting operations
                  * in the pseudocode I assume dentries to be numbered
                  * by 2^n with n being the decimal number of the entry. */
  int        is_allocated;
  void       *d_parent;
  atomic_t   d_count;
  spinlock_t d_lock;
  int        d_child;
  int        d_subdirs;
  inode_t    *d_inode;
  string     d_iname;
 } dentry_t;

dentry_t dNULL = {0, 0, NULL, 0, 0, 0, 0, NULL, NULL};

typedef struct lookup_res_t /* just in order to remove pointers */
 {
  dentry_t  * parent;
  dentry_t  * file;
 } lookup_res_t;

lookup_res_t lNULL = {&dNULL, &dNULL};


/* Function defs: */
#include "pseudo.h"


/* Hacks: */
static dentry_t *root    = NULL; /* this is a pointer to the root directory;
                                  * should be part of the superblock */
static dentry_t *current = NULL; /* we assume that we have a global pointer to
                                  * the current directory of each process */


/* Global locks: */
spinlock_t inode_lock;
spinlock_t dcache_lock;


/* General comments:
 * - Most of the functions defined here use local variables such as
 *   counters or temporary dentries. Of course this is not thread-save.
 *   A model implementing the functions should either inline them
 *   or use some additional locking around each function in order to
 *   serialise their execution.
 * - This pseudocode models a mixture between a synchronous and an
 *   asynchronous filesystem. Especially the way how we wait until
 *   nobody else uses a particular dentry or inode we want to delete,
 *   is significantly different from the Linux kernel's operation.
 *   The reason for this is that I didn't want to introduce a scheduler,
 *   software interrupts and additional process lists.
 * - There will be a new version of these code snippets containing
 *   cross-references to the kernel's code.
 * - The whole pseudocode is about 10 pages long now. Since I have
 *   no means of compiling or testing it I would not expect it to be
 *   free of errors :-)
 */


/* Functions not defined explicitly:
 * - find_dentry() -- a model specific function that returns a dentry
 *   for a given parent and a filename */
extern dentry_t find_dentry   (dentry_t, string);

/* - sleep() -- waits for some time. */
extern void sleep             (void);

/* - foreach(array) -- do something with every element of array */
extern void foreach           (string);
/* - cont() -- is the continue-statement to be used in the foreach-loop. */
extern void cont              (void);

/* - spin_lock(), spin_unlock() -- spinlock operations */
extern void spin_lock         (spinlock_t);
extern void spin_unlock       (spinlock_t);

/* - first_component(), next_component(), last_component() are
 *   functions "exploding" path strings into it's components separated
 *   by /. If / is the first component of a path, it refers to the
 *   root directory. All other /es handled as delimiters. */
extern string first_component (string);
extern string next_component  (string);
extern string last_component  (string);
extern string concat          (string, string);

/* - atomic_read() and atomic_write() read and write atomic integer
 *   variables. */
extern int  atomic_read       (atomic_t);
extern void atomic_write      (atomic_t, int);
extern void atomic_inc        (atomic_t);
extern void atomic_dec        (atomic_t);

/* - up() and down() set and anset mutexes; we can probably skip those. */
extern void up                (mutex_t);
extern void down              (mutex_t);

/* - allocate_inode(dentry) returns a new inode; i_count is set to 1,
 *   i_dentry points to dentry, all locks are released, state is dirty. */
extern inode_t allocate_inode (dentry_t);

/* - allocate_dentry(filename, parent) returns a new dentry; d_iname is
 *   set to filename, d_parent is set to parent, all locks are released,
 *   all lists are empty, d_count is 1. */
extern dentry_t allocate_dentry (string, dentry_t);

/* - deallocate_dentry() and deallocate_inode() set every data field
 *   in a given dentry or inode to 0. */
extern void deallocate_dentry (dentry_t);
extern void deallocate_inode  (inode_t);


/* Initialisation */

int init_fs (void)
 {
  dentry_t my_root;
  inode_t  itmp;

  spin_lock (dcache_lock);
  spin_lock (inode_lock);

  /* get dentry for / */
  my_root = allocate_dentry ("/", dNULL);
  if (!my_root.is_allocated) { goto FIN; }

  /* get inode for / */
  itmp = allocate_inode (my_root);
  if (!itmp.is_allocated) { goto FIN; }
  my_root.d_inode   = &itmp;      /* set inode */
  my_root.d_parent  = &my_root;   /* root is its own parent  */
  my_root.d_subdirs = my_root.id; /* root is its own subdir  */
  my_root.d_child   = my_root.id; /* root is its own sibling */

  /* set up "superblock" */
  root = &my_root;

FIN:
  spin_unlock (inode_lock);
  spin_unlock (dcache_lock);

/* /lost+found is optional */
#ifdef __HAVE_LOSTANDFOUND
  if (root && sys_mkdir ("/lost+found") == SUCCESS)
#else
  if (root)
#endif
   { return (SUCCESS); }
   else
   { return (ERROR); }
 }


/* Cleanup process: */

void cleanup (void)
 {
  inode_t  inode;
  dentry_t dentry;

  while (1) /* This should only be an endless loop if it's actually
             * running as a separate process. Otherwise it must
             * terminate in order to avoid deadlock situations. */
   {
    spin_lock (dcache_lock);
    foreach ("dentry in /list of dentries/");
     {
      if (! atomic_read(dentry.d_count))
       { deallocate_dentry (dentry); }
     } 
    spin_unlock (dcache_lock);

    spin_lock (inode_lock);
    foreach ("inode in /list of inodes/");
     {
      if (! atomic_read(inode.i_count))
       { deallocate_inode (inode); }
      if (inode.i_state == DIRTY) /* sync operation */
       {
        spin_lock (inode.i_lock);
        inode.i_state = 0;
        spin_unlock (inode.i_lock);
       }
     } 
    spin_unlock (inode_lock);

    sleep();
   }

  return;
 }


/* Helper functions: */

/* is a given dentry a directory? */
int is_directory (dentry_t dentry)
 {
  if (dentry.is_allocated && dentry.d_subdirs != 0)
   { return (TRUE); }

  return (FALSE);
 }

/* increment d_count */
void dget (dentry_t dentry)
 {
  if (dentry.is_allocated)
   {
    spin_lock (dentry.d_lock);
    if (atomic_read (dentry.d_count))
     { atomic_inc(dentry.d_count); }
    spin_unlock (dentry.d_lock);
   }

  return;
 }

/* decrement d_count */
void dput (dentry_t dentry)
 {
  if (dentry.is_allocated)
   {
    spin_lock (dentry.d_lock);
    if (atomic_read (dentry.d_count) > 1)
     { atomic_dec (dentry.d_count); }
    spin_unlock (dentry.d_lock);
   }

  return;
 }

/* returns dentry for parent/path if it exists, NULL otherwise */
dentry_t get_dentry (string path, dentry_t parent)
 {
  dentry_t dtmp;

  spin_lock (dcache_lock);
  dtmp = find_dentry (parent, path);  /* model specific function */
  if (dtmp.is_allocated)
   {
    dget (dtmp);                     /* mark entry as "in use" */
    if (!atomic_read (dtmp.d_count)) /* did it work? */
     { dtmp = dNULL; }               /* error */
   }
  spin_unlock (dcache_lock);

  return (dtmp);
 }

/* THIS IS THE NEW VERSION OF get_dentry() AS DISCUSSED WITH ANDY
 * VIA MAIL. */
/* returns dentry for parent/path if it exists, NULL otherwise */
dentry_t get_dentry_NEW (string path, dentry_t parent)
{
 dentry_t dent;

 spin_lock (dcache_lock);

 foreach ("dent in /list of dentries/");
  {
   if (!dent.is_allocated) { cont(); }  /* dent is not allocated */
   if (((dentry_t *)dent.d_parent)->id == parent.id &&  /* correct path and */
       dent.d_iname == path)          /* correct filename? */
    {
     dget (dent);                     /* mark entry as "in use" */
     if (!atomic_read (dent.d_count)) /* did it work? */
      { cont(); }                   /*  no. check next entry. */
      else
      { spin_unlock (dcache_lock);
        return (dent); }              /*  yes! return entry. */
    }
  }
 spin_unlock (dcache_lock);           /* no matching entry found. */

 return (dNULL);
}

/* path traversal, returns parent's and child's dentries */
lookup_res_t path_lookup (string path)
 {
  lookup_res_t result;
  dentry_t parent = dNULL, dtmp;
  string tmp;

  if (path[0] != '/')     /* if path does not start with / */
   {
    if (current != NULL)
     { parent = *current; } /* get current working dir */
     else
     { return (lNULL); } /* error */
   }

  tmp = first_component (path); /* this will return "/" */
  while (tmp)
   {
    /* get dentry for current path component */
    dtmp = get_dentry (tmp, parent);

    if (!dtmp.is_allocated) /* --------------- current path does not exist */
     {
      if (!parent.is_allocated)
       { return (lNULL); } /* error */
      if (tmp != last_component (path))
       { dput (parent); return (lNULL); } /* error */
       else
       { result.parent = &parent; result.file = &dNULL;
         return (result); } 
     }

    /* ------------------------   current path does exist */
    if (tmp != last_component (path))
     {
      if (is_directory (dtmp))
       { /* continue path traversal */
        dput (parent); /* ! this line may be erroneous */
        parent = dtmp;
        tmp = next_component (path);
       }
       else 
       { /* further traversal not possible because one middle
          * component is regular file */
        dput (parent);
        dput (dtmp);
        return (lNULL); /* error */
       }
     }
     else
     { /* this is the last component; we are done. */
      tmp = NULL;
     }
   } /* while (tmp) */

  result.parent = &parent; result.file = &dtmp;
  return (result);
 }

/* release usage counters for an open file */
void path_release (dentry_t dentry)
 {
  if (dentry.is_allocated)
   {
    dput (*((dentry_t *)(dentry.d_parent)));
    dput (dentry);
   }

  return;
 }

/* update sibling list for each child of parent;
 * update parent.d_subdirs */
void update_parent (dentry_t parent)
 {
  dentry_t dent;
  int siblings = 0, subdirs = 0;

  if (!parent.is_allocated) { return; }
  if (!is_directory(parent)) { return; }

  /* find subdirs and siblings */
  foreach ("dent in /list of dentries/");
   {
    if (dent.id != root->id && /* exclude root, in the models this should
                                * become something like (dent.id != 0) */
        ((dentry_t *) dent.d_parent)->id == parent.id &&
        atomic_read(dent.d_count) )
     { siblings |= dent.id;  /* refers to dent's ID */
       if (is_directory(dent))
        { subdirs |= dent.id; }
     }
   }

  /* update siblings */
  foreach ("dent in /list of dentries/");
   {
    if (dent.id != root->id && /* exclude root, in the models this should
                                * become something like (dent.id != 0) */
        ((dentry_t *) dent.d_parent)->id == parent.id &&
        atomic_read(dent.d_count) )
     { dent.d_child = siblings; }
   }

  /* update subdirs; make sure that / is a subdir of / */
  parent.d_subdirs = subdirs | parent.id;

  return;
 }

