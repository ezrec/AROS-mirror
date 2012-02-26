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

#ifndef LIB_SLAB_H
#define LIB_SLAB_H

#include "LibC.h"
#include <LowLevel/Atomic.h>

struct SMem_Header
{
   struct _atomic_item  item;
   uint32               smagic;  /* SLAB_MAGIC */
   struct SLAB_Level   *level;
   uint8                block[0];
};

#define SHIFT2BYTES(_shift_)     (1L << _shift_)

#define HEADER2MEM(_header_)     (&(_header_)->block)
#define MEM2HEADER(_mem_)        (((uint8 *) _mem_) - sizeof(struct SMem_Header))

#define SL_PUSH_BLOCK(_sl_, _b_) ATOMIC_SADDHEAD(&(_sl_)->free_list, _b_)
#define SL_POP_BLOCK(_sl_)       ATOMIC_SREMHEAD(&(_sl_)->free_list)

struct SLAB_Level
{
   struct _atomic_stack stk;
   uint8                shift;
   uint8                pad1[3];
   
   uint32               capacity;

   struct _atomic_cnt   used;
   void*                mem_block;
};

/*
 * SLAB_Allocator
 */

struct SLAB_Allocator 
{
   struct Allocator  allocator;
   /* Configuration */
   uint8             first_shift;
   uint8             last_shift;
   uint8             pad1[2];

   struct SLAB_Level *levels; 
};

/*
 * SLAB
 */

#define SLAB_FIRST_SHIFT   (5)

#endif /* LIB_SLAB_H */
