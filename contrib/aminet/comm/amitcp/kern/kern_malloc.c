/*
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 1.9  1994/03/26  09:36:29  too
 * Moved bsd_malloc(), bsd_free() inlines here as normal functions.
 * Added bsd_realloc()
 *
 * Revision 1.8  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 * Revision 1.7  1993/05/17  01:07:47  ppessi
 * Changed RCS version.
 *
 * Revision 1.6  1993/05/04  12:48:38  jraja
 * Added tuning for SASC malloc.
 *
 * Revision 1.5  93/04/06  15:15:50  15:15:50  jraja (Jarno Tapio Rajahalme)
 * Changed spl function return value storage to spl_t,
 * changed bcopys and bzeros to aligned and/or const when possible,
 * added inclusion of conf.h to every .c file.
 * 
 * Revision 1.4  93/03/11  19:41:03  19:41:03  jraja (Jarno Tapio Rajahalme)
 * Changed mallocSemaphore to malloc_semaphore.
 * 
 * Revision 1.3  93/03/05  21:11:15  21:11:15  jraja (Jarno Tapio Rajahalme)
 * Fixed includes (again).
 * 
 * Revision 1.2  93/02/24  12:54:36  12:54:36  jraja (Jarno Tapio Rajahalme)
 * Changed init to remember if initialized.
 * 
 * Revision 1.1  93/02/04  18:29:36  18:29:36  jraja (Jarno Tapio Rajahalme)
 * Initial revision
 * 
 */

#include <conf.h>

#include <sys/param.h>
#include <sys/malloc.h>

#include <kern/amiga_includes.h>

#if __SASC
/* 
 * Change the minimum chuck size of the SASC memory allocator.
 * The default value is 16.384 bytes (SASC6.2), but smaller value
 * is more efficient, since it takes less time to search the free
 * memory pool when it is smaller.
 */
long _MSTEP = 4096;

/*
 * Set the memory type allocated by malloc() to public, since the memory
 * is used by many tasks (including all library users)
 */
unsigned long _MemType = MEMF_PUBLIC;

#endif /* __SASC */

struct SignalSemaphore malloc_semaphore = { 0 };
static BOOL initialized = FALSE;

BOOL
malloc_init(void)
{
  if (!initialized) {
    /*
     * Initialize malloc_semaphore for use.
     * Do not call bsd_malloc() or bsd_free() before this!
     */
    InitSemaphore(&malloc_semaphore);
    initialized = TRUE;
  }
  return TRUE;
}

void *
bsd_malloc(unsigned long size, int type, int flags)
{
  register void *mem;

  extern void *malloc(size_t);
  extern struct SignalSemaphore malloc_semaphore; 

  ObtainSemaphore(&malloc_semaphore);
  mem = malloc((size_t)size);
  ReleaseSemaphore(&malloc_semaphore);

  return mem;
}

void
bsd_free(void *addr, int type)
{
  extern void free(void *);
  extern struct SignalSemaphore malloc_semaphore; 

  ObtainSemaphore(&malloc_semaphore);
  free(addr);
  ReleaseSemaphore(&malloc_semaphore);
}

/*
 * bsd_realloc() cannot be used for reallocating
 * last freed block for obvious reasons
 */

void *
bsd_realloc(void * mem, unsigned long size, int type, int flags)
{
  extern void *realloc(void *, size_t);
  extern struct SignalSemaphore malloc_semaphore; 

  ObtainSemaphore(&malloc_semaphore);
  mem = realloc(mem, (size_t)size);
  ReleaseSemaphore(&malloc_semaphore);

  return mem;
}
