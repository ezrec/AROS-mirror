/*
 * Copyright (C) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
 * Copyright (C) 2005 Pavel Fedin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */

#include <conf.h>

#include <sys/param.h>
#include <sys/malloc.h>

#include <kern/amiga_includes.h>

struct SignalSemaphore malloc_semaphore = { 0 };
static BOOL initialized = FALSE;
static APTR mem_pool = NULL;

BOOL
malloc_init(void)
{
#if defined(__AROS__)
D(bug("[AROSTCP](kern_malloc.c) malloc_init()\n"));
#endif
  if (!initialized) {
    mem_pool = CreatePool(MEMF_PUBLIC, 0x4000, 0x2000);
    if(mem_pool == NULL)
      return FALSE;

    /*
     * Initialize malloc_semaphore for use.
     * Do not call bsd_malloc() or bsd_free() before this!
     */
    InitSemaphore(&malloc_semaphore);
    initialized = TRUE;
  }
  return TRUE;
}

VOID
malloc_deinit(void)
{
#if defined(__AROS__)
D(bug("[AROSTCP](kern_malloc.c) malloc_deinit()\n"));
#endif
  DeletePool(mem_pool);
  initialized = FALSE;
  return;
}

void *
bsd_malloc(unsigned long size, int type, int flags)
{
  register void *mem;
#if defined(__AROS__)
D(bug("[AROSTCP](kern_malloc.c) bsd_malloc()\n"));
#endif

#ifdef DEBUG_MEM
  size += 4;
#endif
  ObtainSemaphore(&malloc_semaphore);
  mem = AllocVecPooled(mem_pool, size);
#ifdef DEBUG_MEM
  *((ULONG *)mem)++=0xbaadab00;
#endif
  ReleaseSemaphore(&malloc_semaphore);

  return mem;
}

void
bsd_free(void *addr, int type)
{
#if defined(__AROS__)
D(bug("[AROSTCP](kern_malloc.c) bsd_free()\n"));
#endif

  ObtainSemaphore(&malloc_semaphore);
#ifdef DEBUG_MEM
  if (*--((ULONG *)addr) == 0xbaadab00)
  {
    *((ULONG *)addr) = 0xabadcafe;
#endif
    FreeVecPooled(mem_pool, addr);
#ifdef DEBUG_MEM
  }
  else
    log(LOG_CRIT,"Attempt to free non-allocated memory at 0x%08x!!!", addr);
#endif
  ReleaseSemaphore(&malloc_semaphore);
}

/*
 * bsd_realloc() cannot be used for reallocating
 * last freed block for obvious reasons
 *
 * This function is only called from one place, and there
 * doesn't appear to be any need to copy data from
 * the old buffer in that particular case, so we don't.
 */

void *
bsd_realloc(void * mem, unsigned long size, int type, int flags)
{
  void *new_mem = NULL;
#if defined(__AROS__)
D(bug("[AROSTCP](kern_malloc.c) bsd_realloc()\n"));
#endif

  ObtainSemaphore(&malloc_semaphore);
#ifdef DEBUG_MEM
  {
    ULONG *realmem = mem-4;
    unsigned long realsize = size+4;
    if (*realmem == 0xbaadab00)
    {
#else
#define realmem mem
#define realsize size
#endif
      new_mem = AllocVecPooled(mem_pool, realsize);
      if(new_mem != NULL) {
#ifdef DEBUG_MEM
	*((ULONG *)new_mem)++ = 0xbaadab00;
#endif
	CopyMem(mem, new_mem, size);
	FreeVecPooled(mem_pool, realmem);
      }
#ifdef DEBUG_MEM
    }
    else
      log(LOG_CRIT,"Attempt to reallocate non-allocated memory at 0x%08x!!!", mem);
  }
#endif
  ReleaseSemaphore(&malloc_semaphore);

  return new_mem;
}

