/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "LibC.h"
#include <Generic/Types.h>
#include <LowLevel/Atomic.h>
#include <proto/exec.h>
#include "slab.h"

/*
 * Private
 */
enum
{
   SLAB_USED_MAGIC = (('S'<<24) || ('L'<<16) || ('A'<<8) || ('U')),
   SLAB_FREE_MAGIC = (('S'<<24) || ('L'<<16) || ('A'<<8) || ('F'))
};

/*
 * Private
 */

static inline int32 header_check(const struct SMem_Header *header, const uint32 magic)
{
#ifndef DEBUG
   return (header->smagic == magic);
#else
   if(header->smagic == magic)
      return 1;

   return 0;
#endif
}

static inline struct SLAB_Level *level_classify(const struct SLAB_Allocator *sa, uint32 bytes)
{
   struct SLAB_Level *res = sa->levels;

   /* Never try to pass 'bytes == 0' to this function ! */
   --bytes;

   /* We will use one extra level to stop scan */
   while(res->shift)
   {
      if((bytes >> res->shift) == 0)
         return res;

      /* Next */
      ++res;
   }

   /* End */
   return 0;
}

static int32 level_dispose(struct SLAB_Level *sl)
{
   int32 res = 1;

   if(sl)
   {
      if(sl->mem_block)
      {
         if (sl->used.counter)
         {
            res = 0;
         }

#ifndef __amigaos4__
         FreeVec(sl->mem_block);
#else
         IExec->FreeVec(sl->mem_block);
#endif
         sl->mem_block = NULL;
      }
      else
         res = 0;
   }
   else
      res = 0;

   return(res);
}

static void memblock_init(struct SLAB_Level  *sl, uint8 *mem_block)
{
   uint32 block_bytes = SHIFT2BYTES(sl->shift) + sizeof(struct SMem_Header);
   struct SMem_Header *block_header;
   uint32 x;

   for(x = 0; x < sl->capacity; ++x)
   {
      block_header = (struct SMem_Header *) mem_block;

      _atomic_item_init(&block_header->item);
      block_header->smagic = SLAB_FREE_MAGIC;
      block_header->level  = sl;

      _atomic_stack_push(&sl->stk, &block_header->item);

      mem_block += block_bytes;
   }

   return;
}

static int32 level_init(struct SLAB_Level *sl, uint8 shift, uint32 capacity)
{
   int32 res = 0;

   uint32 bytes;

   sl->shift      = shift;

   sl->capacity   = capacity;
   _atomic_cnt_init(&sl->used);

   bytes = (SHIFT2BYTES(shift) + sizeof(struct SMem_Header)) * capacity;

#ifndef __amigaos4__
   sl->mem_block  = AllocVec(bytes, MEMF_PUBLIC);
#else
   sl->mem_block  = IExec->AllocVec(bytes, MEMF_PUBLIC);
#endif
   _atomic_stack_init(&sl->stk);

   if(sl->mem_block)
   {
      memblock_init(sl, (uint8*)sl->mem_block);
      res = 1;
   }

   return(res);
}



/*
 * SLAB_Init
 * already fixed. returns bool
 */

const struct Allocator* SLAB_Init(uint8 shifts, const uint32 config_array[])
{
#ifndef __amigaos4__
   struct SLAB_Allocator *sa = (struct SLAB_Allocator*)AllocVec(sizeof(struct SLAB_Allocator), MEMF_ANY | MEMF_CLEAR);
#else
   struct SLAB_Allocator *sa = (struct SLAB_Allocator*)IExec->AllocVec(sizeof(struct SLAB_Allocator), MEMF_ANY | MEMF_CLEAR);
#endif

   sa->allocator.alloc         = &SLAB_Alloc;
   sa->allocator.free          = &SLAB_Free;
   sa->first_shift   = SLAB_FIRST_SHIFT;
   sa->last_shift    = SLAB_FIRST_SHIFT + shifts;

#ifndef __amigaos4__
   sa->levels = (struct SLAB_Level*)AllocVec((shifts + 1) * sizeof(struct SLAB_Level), MEMF_ANY|MEMF_CLEAR);
#else
   sa->levels = (struct SLAB_Level*)IExec->AllocVec((shifts + 1) * sizeof(struct SLAB_Level), MEMF_ANY|MEMF_CLEAR);
#endif

   if(sa->levels)
   {
      uint8 x;

      for(x = 0; x < shifts; ++x)
      {
         if(!level_init(&sa->levels[x], sa->first_shift + x, config_array[x]))
         {
            SLAB_Cleanup(&sa->allocator);
            sa = 0;
            break;
         }
      }
   }
   else
   {
      SLAB_Cleanup(&sa->allocator);
      sa = 0;
   }

   return &sa->allocator;
}

/*
 * SLAB_Cleanup
 * already fixed. returns bool
 */

void SLAB_Cleanup(const struct Allocator* s)
{
   struct SLAB_Allocator *sa = (struct SLAB_Allocator*)s;

   if(sa)
   {
      uint8 x;
      uint8 shifts = sa->last_shift - sa->first_shift;

      for(x = 0; x < shifts; ++x)
         level_dispose(&sa->levels[x]);

#ifndef __amigaos4__
      if (sa->levels != 0)
         FreeVec(sa->levels);
      FreeVec(sa);
#else
      if (sa->levels != 0)
         IExec->FreeVec(sa->levels);
      IExec->FreeVec(sa);
#endif
   }
}

/*
 * SLAB_Alloc
 * already fixed.
 */

void* SLAB_Alloc(const struct Allocator* s, size_t bytes)
{
   void* res = 0;
   struct SLAB_Allocator *sa = (struct SLAB_Allocator*)s;

   if (bytes)
   {
      struct SLAB_Level *sl = level_classify(sa, bytes);

      if (sl)
      {
         struct SMem_Header *header;
         header = (struct SMem_Header*)_atomic_stack_pop(&sl->stk);

         if (header && header_check(header, SLAB_FREE_MAGIC))
         {
            _atomic_cnt_inc(&sl->used);
            header->smagic = SLAB_USED_MAGIC;
            res = HEADER2MEM(header);
         }
         else
         {
#ifndef __amigaos4__
            header = (struct SMem_Header*)AllocVec(bytes + sizeof(struct SMem_Header), MEMF_PUBLIC);
#else
            header = (struct SMem_Header*)IExec->AllocVec(bytes + sizeof(struct SMem_Header), MEMF_PUBLIC);
#endif

            if(header)
            {
               _atomic_item_init(&header->item);
               header->smagic = SLAB_USED_MAGIC;
               header->level  = 0;
               res = HEADER2MEM(header);
            }
         }
      }
   }

   /* End */
   return(res);
}

/*
 * SLAB_Free
 * already fixed
 */

void SLAB_Free(const struct Allocator* a, void* mem)
{
   if (mem)
   {
      struct SMem_Header *header = (struct SMem_Header *) MEM2HEADER(mem);

      if(header_check(header, SLAB_USED_MAGIC))
      {
         header->smagic = SLAB_FREE_MAGIC;

         if (header->level)
         {
            _atomic_stack_push(&header->level->stk, &header->item);
            _atomic_cnt_dec(&header->level->used);
         }
         else
         {
#ifndef __amigaos4__
            FreeVec(header);
#else
            IExec->FreeVec(header);
#endif
         }
      }
   }
}


