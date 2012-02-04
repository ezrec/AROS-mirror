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

#ifndef ATOMIC_PPC_H
#define ATOMIC_PPC_H


static inline void _atomic_stack_push(register struct _atomic_stack* list, register struct _atomic_item* item)
{
   asm volatile (
         ".atomic_saddhead_loop_%=:\n"
         "	lwarx	%0,0,%1\n"
         "	stw	%0,0(%2)\n"
         "	sync\n"
         "	stwcx.	%2,0,%1\n"
         "	bne-	.atomic_saddhead_loop_%=\n"
         : "=&r" (item)
         : "r" (list), "b" (item)
         : "memory", "cr0");
}

static inline struct _atomic_item* _atomic_stack_pop(register struct _atomic_stack* list)
{
   register struct _atomic_item* item;
   asm volatile (
         "\n"
         ".atomic_sremhead_loop_%=:\n"
         "	lwarx	%0,0,%2\n"
         "	mr.	%1,%0\n"
         "	beq-	.atomic_sremhead_skip_%=\n"
         "	lwz	%1,0(%0)\n"
         ".atomic_sremhead_skip_%=:\n"
         "	stwcx.	%1,0,%2\n"
         "	bne-	.atomic_sremhead_loop_%=\n"
         "	isync\n"
         : "=&b" (item), "=&r" (item)
         : "r" (list)
         : "memory", "cr0");
   return item;
}

static inline void _atomic_cnt_inc(struct _atomic_cnt* cnt)
{
	asm volatile (
         ".atomic_inc%=:\n"
         "	lwarx	%1,0,%0\n"
         "	addi	%1,%1,1\n"
         "	stwcx. %1,0,%0\n"
         "	bne-	.atomic_inc%=\n"
         : 
         : "r" (cnt), "b" (0)
         : "r0", "memory", "cr0");
}

static inline void _atomic_cnt_dec(struct _atomic_cnt* cnt)
{
	asm volatile (
         ".atomic_dec%=:\n"
         "	lwarx	%1,0,%0\n"
         "	subi	%1,%1,1\n"
         "	stwcx. %1,0,%0\n"
         "	bne-	.atomic_dec%=\n"
         :
         : "r" (cnt), "b" (0)
         : "r0", "memory", "cr0");
}

/*
 * following are broken and useless.
 *
static inline int32 _atomic_slock_trywrite(struct _atomic_slock* lock)
{
   register int32 res;

   asm volatile(        "   move.l %1,a0\n"
               "   moveq #0,d0\n"
               "   bset.b #7,(a0)\n"
               "   bne 1f\n"
               "   cmp.l #$80000000,(a0)\n"
               "   seq d0\n"
               "   beq 1f\n"
               "   bclr.b #7,(a0)\n"
               "1: move.l d0,%0\n"
               : "=g"(res)
               : "m"(lock)
               : "a0", "d0", "cc", "memory");
   return res;
}

static inline void _atomic_slock_unlockwrite(struct _atomic_slock* lock)
{
   asm volatile(        "   move.l %0,a0\n"
               "   clr.l (a0)\n"
               :
               : "m"(lock)
               : "a0", "cc", "memory");
}

static inline int32 _atomic_slock_tryread(struct _atomic_slock* lock)
{
   register int32 res;

   asm volatile(        "   move.l %1,a0\n"
               "   moveq #0,d0\n"
               "   addq.l #1,(a0)\n"
               "   spl d0\n"
               "   bpl 1f\n"
               "   subq.l #1,(a0)\n"
               "1: move.l d0,%0\n"
               : "=g"(res)
               : "m"(lock)
               : "a0", "d0", "cc", "memory");
   return res;
}

static inline void _atomic_slock_unlockread(struct _atomic_slock* lock)
{
   asm volatile(        "   move.l %0,a0\n"
               "   subq.l #1,(a0)\n"
               :
               : "m"(lock)
               : "a0", "d0", "cc", "memory");
}
 */
#endif

