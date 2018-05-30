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

#ifndef _ATOMIC_H_
#define _ATOMIC_H_

struct _atomic_item
{
   struct _atomic_item*    next;
};

struct _atomic_stack
{
   struct _atomic_item*    head;       // list head
   uint8                   available;  // bit 0 = item available
   uint8                   pad[3];
};

struct _atomic_cnt
{
   uint32 counter;
};

struct _atomic_slock
{
   uint32 status;
};

void inline _atomic_slock_init(struct _atomic_slock* s)
{
   s->status = 0;
}

void inline _atomic_cnt_init(struct _atomic_cnt* c)
{
   c->counter = 0;
}

void inline _atomic_stack_init(struct _atomic_stack* l)
{
   l->available = 0;
   l->head = 0;
}

void inline _atomic_item_init(struct _atomic_item* n)
{
   n->next = 0;
}

#if defined(__AMIGAOS4__) || defined(__MORPHOS__) || (defined(__AROS__) && defined(__PPC__))
   #include "Atomic_ppc.h"
#elif defined(__AROS__) && !defined(mc68000)
   #include "Atomic_x86.h"
#else
   #include "Atomic_m68k.h"
#endif

#endif
