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

#ifndef ATOMIC_X86_H
#define ATOMIC_X86_H

#ifdef __AROS__
#include <aros/atomic.h>
#endif


static inline void _atomic_stack_push(struct _atomic_stack* list, struct _atomic_item* item)
{
   asm volatile("   mov %%eax,%1\n"
                "1: lock bts $31,4(%%eax)\n"
                "   jb 1b\n"
                "   mov %%ebx,%0\n"
                "   mov %%ecx,(%%eax)\n"
                "   mov (%%ebx),%%ecx\n"
                "   mov (%%eax),%%ebx\n"
                "   lock btr $31,4(%%eax)\n"
                :
                : "m" (item), "m" (list)
                : "eax", "ebx", "ecx", "cc", "memory");
}

static inline struct _atomic_item* _atomic_stack_pop(struct _atomic_stack* list)
{
   register struct _atomic_item *n;
   asm volatile("   mov %%eax,%1\n"
                "1: lock bts $31,4(%%eax)\n"
                "   jb 1b\n"
                "   mov %0,(%%eax)\n"
                "   je 2f\n"
                "   mov %%ebx,(%0)\n"
                "   mov (%%eax),%%ebx\n"
                "2: lock btr $31,4(%%eax)\n"
                : "=g"(n)
                : "m"(list)
                : "eax", "ebx", "cc", "memory");

   return n;
}

static inline void _atomic_cnt_inc(struct _atomic_cnt* cnt)
{
#ifdef __AROS__
   AROS_ATOMIC_INC(cnt->counter);
#else
   asm volatile("   lock incl (%0)\n"
         :
         : "g"(cnt)
         : "cc", "memory");
#endif
}

static inline void _atomic_cnt_dec(struct _atomic_cnt* cnt)
{
#ifdef __AROS__
   AROS_ATOMIC_DEC(cnt->counter);
#else
   asm volatile("   lock decl (%0)\n"
         :
         : "g"(cnt)
         : "cc", "memory");
#endif
}

static inline int32 _atomic_slock_trywrite(struct _atomic_slock* lock)
{
   register int32 res;
/*

   asm volatile("   move.l %1,a0\n"
                "1: bset.b #7,(a0)\n"
                "   bne.b 1b\n"
                "   move.l (a0),d0\n"
                "   bclr.b #31,d0\n"
                "   moveq #0,d1\n"
                "   tst.l d0\n"
                "   bne.b 2f\n"
                "   moveq #-1,d1\n"
                "   move.l d1,(a0)\n"
                "2: bclr.b #7,(a0)\n"
                "   move.l d1,%0\n"
               : "=g"(res)
               : "m"(lock)
               : "a0", "d0", "d1", "cc", "memory");
*/
   return res;
}

static inline void _atomic_slock_unlockwrite(struct _atomic_slock* lock)
{
/*
   asm volatile("   move.l %0,a0\n"
                "1: bset.b #7,(a0)\n"
                "   bne.b 1b\n"
                "   move.l (a0),d0\n"
                "   moveq #-1,d1\n"
                "   cmp.l d0,d1\n"
                "   bne.b 2f\n"
                "   moveq #0,d0\n"
                "   move.l d0,(a0)\n"
                "2: bclr.b #7,(a0)\n"
                : 
                : "m"(lock)
                : "a0", "d0", "d1", "cc", "memory");
*/
}

static inline int32 _atomic_slock_tryread(struct _atomic_slock* lock)
{
   register int32 res;
/*

   asm volatile("   move.l %1,a0\n"
                "1: bset.b #7,(a0)\n"
                "   bne.b 1b\n"
                "   move.l (a0),d0\n"
                "   moveq.l #-1,d1\n"
                "   cmp.l d0,d1\n"
                "   beq.b 2f\n"
                "   addq.l #1,(a0)\n"
                "   bclr.b #31,d0\n"
                "2: bclr.b #7,(a0)\n"
                "   move.l d0,%0\n"
               : "=g"(res)
               : "m"(lock)
               : "a0", "d0", "d1", "cc", "memory");
*/
   return res;
}

static inline int32 _atomic_slock_unlockread(struct _atomic_slock* lock)
{
   register int32 res;
/*

   asm volatile("   move.l %1,a0\n"
                "1: bset.b #7,(a0)\n"
                "   bne.b 1b\n"
                "   move.l (a0),d0\n"
                "   moveq.l #-1,d0\n"
                "   cmp.l d0,d1\n"
                "   beq.b 2f\n"
                "   subq.l #1,(a0)\n"
                "   bclr.b #31,d0\n"
                "   bclr.b #7,(a0)\n"
                "2: move.l d0,%0\n"
               : "=g"(res)
               : "m"(lock)
               : "a0", "d0", "d1", "cc", "memory");
*/
   return res;
}

#endif

