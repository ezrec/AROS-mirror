/*
 * Copyright (C) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
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
  DeletePool(mem_pool);
  initialized = FALSE;
  return;
}

void *
bsd_malloc(unsigned long size, int type, int flags)
{
  register void *mem;

  ObtainSemaphore(&malloc_semaphore);
  mem = AllocVecPooled(mem_pool, size);
  ReleaseSemaphore(&malloc_semaphore);

  return mem;
}

void
bsd_free(void *addr, int type)
{
  ObtainSemaphore(&malloc_semaphore);
  FreeVecPooled(mem_pool, addr);
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
  void *new_mem;

  ObtainSemaphore(&malloc_semaphore);
  new_mem = AllocVecPooled(mem_pool, size);
  if(new_mem != NULL)
    FreeVecPooled(mem_pool, mem);
  ReleaseSemaphore(&malloc_semaphore);

  return new_mem;
}

