
/* (C) Copyright by Dirk Heckmann and Aaron Digulla in D-7750 Konstanz

     $Id$

	  This module provides everything to write save and clean
	  programs that allocate memory. This routine handles all
	  that's needed for allocating and freeing memory and de-
	  bugging the programm itself.

	  Take care to include "debug_mem.h" in your module, where
	  you allocate memory and re-compile it !

	  I use here code from the allround.lib by Dirk Heckmann.
	  Thanks for work, Dirk.

     TODO
	  The method of replacing Message() by printf() has to be replaced.
	  We need a Requester or Alert here (especially for WB-Startup).

*/

     /* Some defs and things we need in this module */

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>

#define DEBUG_C
#include "debug_mem.h"

extern struct ExecBase * SysBase;
#define ALLOCERROR	 (1L << 31)
#include <stdio.h>


char message[256];	 /* Place for the Output */

     /* _debug_AllocMem ()

     PARAMETER
	  long	       size		     ; like AllocMem ()
	  long	       requirements	     ; like AllocMem ()
	  const char * infotext 	     ; NEW !

     RETURN
	  void *

     DESCRIPTION

	  This Function provides what every Amiga-Programmer always
	  wanted: a SAVE way of allocating memory. _debug_AllocMem
	  will write the size and position and name of the memory-
	  block in the TaskMem-list (which is tracked by Exec). So
	  we can check the block in _debug_FreeMem (no more gurus
	  8100000[59] !) and look whether there is memory left at
	  exitus.

	  BEWARE: The infotext is NOT copied. YOU MUST NOT MODIFY
	  IT. It is defined in debug_mem.h.

	  Because of the wonderful ability of DICE to create __auto-
	  exit-functions, we can then check for all non-freed mem-
	  ory ! */

void * _debug_AllocMem (long size, long reqs, const char * infotext)

{
     struct MemList * ml;		/* Received from Exec via AllocEntry */
     struct MemList   block;		/* That's what we want from Exec */
     struct List    * tmem_list;	/* Pointer to the TaskMem-list */

	  /* Initialize tmem_list */

     tmem_list = &SysBase->ThisTask->tc_MemEntry;

	  /* Fill in the block with the requirements */

     block.ml_NumEntries	   = 1;      /* just one entry */
     block.ml_ME[0].me_Reqs	   = reqs;   /* the requirements (MEMF_xxxx) */
     block.ml_ME[0].me_Length	   = size;   /* the size we want */

#ifdef __AROS__
    if (!NewAllocEntry(&block, &ml, NULL))
    	return (NULL);
#else
     ml = AllocEntry (&block);          /* get it */

     if ((ULONG)ml & ALLOCERROR)        /* Success ? */
	  return (NULL);                     /* sad */
#endif

	  /* Add block to TaskMem-list */
     AddTail (tmem_list, &ml->ml_Node);

	  /* Add the infotext, so _debug_FreeAllMem() can find it */
     ml->ml_Node.ln_Name = infotext;

     return (ml->ml_ME[0].me_Addr);   /* Give back address */
} /* _debug_AllocMem () */


     /* _debug_FreeMem ()

     PARAMETER
	  void	     * memorypointer
	  long	       blocksize
	  const char * infotext

     RETURN
	  - none -

     DESCRIPTION

	  This is the opposite to _debug_AllocMem(). Here we give the
	  memory back to the system ... AFTER SOME CHECKS. So we will
	  never get any gurus for this !

	  If _debug_FreeMem fails, we will get a message with more
	  information, why. */

void _debug_FreeMem (void * memptr, long size, const char * infotext)

{
     struct MemList * ml;		/* This is what we found */
     struct List    * tmem_list;	/* and here we look for it */

	  /* Initialize tmem_list */

     tmem_list = &SysBase->ThisTask->tc_MemEntry;

	  /* Get first entry out of the TaskMem-list */

     ml = (struct MemList *)tmem_list->lh_Head;

     while (ml->ml_Node.ln_Succ)
     {
	  if (ml->ml_ME[0].me_Addr == memptr)
	  { /* found it ? */
	       struct MemEntry * me = ml->ml_ME;

	       if (me->me_Length != size)
	       {
		    sprintf (message, "FreeMem(): Warning: Size mismatch error (old %d new %d)\n\t%s\n",
			 me->me_Length, size, infotext);
		    Write (Output(), message, strlen(message));
		    Delay (50);
	       }

	       Remove (&ml->ml_Node);
	       FreeEntry (ml);
	       return ;
	  }

	  ml = (struct MemList *)ml->ml_Node.ln_Succ;
     } /* while ((struct Node *)ml->ln_Succ) */

     sprintf (message, "FreeMem(): Error: Memory not found (Addr $%08x Size %d)\n\t%s\n",
	  memptr, size, infotext);
     Write (Output(), message, strlen(message));
    Delay (50);
} /* _debug_FreeMem () */


void _debug_FreeAllMem (void)

{
     struct MemList * ml;		/* This is what we found */
     struct List    * tmem_list;	/* and here we look for it */

	  /* Initialize tmem_list */

     tmem_list = &SysBase->ThisTask->tc_MemEntry;

	  /* Get first entry out of the TaskMem-list */

     ml = (struct MemList *)tmem_list->lh_Head;

     while (ml->ml_Node.ln_Succ)
     {
	  if (strncmp (ml->ml_Node.ln_Name, "AllocMem()", 10) == 0)
	  {
			 /* found an entry */
	       struct MemEntry * me = ml->ml_ME;

	       sprintf (message, "FreeAllMem(): Error: Forgotten Memory (Addr $%08x Size %d)\n\t%s\n",
		    me->me_Addr, me->me_Length, ml->ml_Node.ln_Name);
	       Write (Output(), message, strlen(message));
	       Delay (50);

	       Remove (&ml->ml_Node);
	       FreeEntry (ml);
	  }

	  ml = (struct MemList *)ml->ml_Node.ln_Succ;
     } /* while ((struct Node *)ml->ln_Succ) */
} /* FreeAllMem () */


