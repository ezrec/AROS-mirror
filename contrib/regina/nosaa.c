/* There is no need for rexxsaa.c or client.c with static binding.
 * We support stub routines for this reason here.
 * FIXME, FGC: We should move stuff from rexx.c to this file.
 */

#include "rexx.h"
int init_rexxsaa( tsd_t *TSD )
{
   TSD = TSD; /* keep compiler happy */
   return(1);
}

void deinit_rexxsaa( tsd_t *TSD )
{
   TSD = TSD; /* keep compiler happy */
}

void *IfcAllocateMemory( unsigned long size )
{
   size = size; /* keep compiler happy */
   return(NULL); /* should never happen! */
}

int init_client( tsd_t *TSD )
{
   TSD = TSD; /* keep compiler happy */
   return(1);
}
