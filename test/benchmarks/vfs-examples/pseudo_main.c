
/*
 * $Author: muehlber $ : $RCSfile: pseudo_main.c,v $
 * $Revision: 1.3 $, $Date: 2007/07/13 17:12:16 $
 */

#include "pseudo_misc.c"
#include "pseudo_creat.c"
#include "pseudo_unlink.c"
#include "pseudo_mkdir.c"
#include "pseudo_rmdir.c"
#include "pseudo_rename.c"
#include "pseudo_cd.c"


int main (void)
 {

  init_fs ();

  return (0);
 }

