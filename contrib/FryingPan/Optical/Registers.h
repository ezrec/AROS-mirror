/*
 * FryingPan - Amiga CD/DVD Recording Software (User Interface and supporting Libraries only)
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __REGISTERS
#define __REGISTERS

// what U see here sucks allot but I can't help it.
// so far it is the only way I know...

#if defined( __AMIGA__ )

   #define GETA0(var) asm("mov.l a0,%0\n\t" : "=g"(var))
   #define GETA1(var) asm("mov.l a1,%0\n\t" : "=g"(var))
   #define GETA2(var) asm("mov.l a2,%0\n\t" : "=g"(var))
   #define GETA3(var) asm("mov.l a3,%0\n\t" : "=g"(var))
   #define GETA4(var) asm("mov.l a4,%0\n\t" : "=g"(var))
   #define GETA5(var) asm("mov.l a5,%0\n\t" : "=g"(var))
   #define GETA6(var) asm("mov.l a6,%0\n\t" : "=g"(var))
   #define GETA7(var) asm("mov.l a7,%0\n\t" : "=g"(var))
   #define GETD0(var) asm("mov.l d0,%0\n\t" : "=g"(var))
   #define GETD1(var) asm("mov.l d1,%0\n\t" : "=g"(var))
   #define GETD2(var) asm("mov.l d2,%0\n\t" : "=g"(var))
   #define GETD3(var) asm("mov.l d3,%0\n\t" : "=g"(var))
   #define GETD4(var) asm("mov.l d4,%0\n\t" : "=g"(var))
   #define GETD5(var) asm("mov.l d5,%0\n\t" : "=g"(var))
   #define GETD6(var) asm("mov.l d6,%0\n\t" : "=g"(var))
   #define GETD7(var) asm("mov.l d7,%0\n\t" : "=g"(var))

#elif defined(__MORPHOS__)
 
   #define GETA0(var) asm("mov.l r24,%0\n\t" : "=g"(var))
   #define GETA1(var) asm("mov.l r25,%0\n\t" : "=g"(var))
   #define GETA2(var) asm("mov.l r26,%0\n\t" : "=g"(var))
   #define GETA3(var) asm("mov.l r27,%0\n\t" : "=g"(var))
   #define GETA4(var) asm("mov.l r28,%0\n\t" : "=g"(var))
   #define GETA5(var) asm("mov.l r29,%0\n\t" : "=g"(var))
   #define GETA6(var) asm("mov.l r30,%0\n\t" : "=g"(var))
   #define GETA7(var) asm("mov.l r31,%0\n\t" : "=g"(var))
   #define GETD0(var) asm("mov.l r16,%0\n\t" : "=g"(var))
   #define GETD1(var) asm("mov.l r17,%0\n\t" : "=g"(var))
   #define GETD2(var) asm("mov.l r18,%0\n\t" : "=g"(var))
   #define GETD3(var) asm("mov.l r19,%0\n\t" : "=g"(var))
   #define GETD4(var) asm("mov.l r20,%0\n\t" : "=g"(var))
   #define GETD5(var) asm("mov.l r21,%0\n\t" : "=g"(var))
   #define GETD6(var) asm("mov.l r22,%0\n\t" : "=g"(var))
   #define GETD7(var) asm("mov.l r23,%0\n\t" : "=g"(var))
/*
   #define R_A0 ({ register APTR ___R_A0 __asm("r24"); ___R_A0; })
   #define R_A1 ({ register APTR ___R_A1 __asm("r25"); ___R_A1; })
   #define R_A2 ({ register APTR ___R_A2 __asm("r26"); ___R_A2; })
   #define R_A3 ({ register APTR ___R_A3 __asm("r27"); ___R_A3; })
   #define R_A4 ({ register APTR ___R_A4 __asm("r28"); ___R_A4; })
   #define R_A5 ({ register APTR ___R_A5 __asm("r29"); ___R_A5; })
   #define R_A6 ({ register APTR ___R_A6 __asm("r30"); ___R_A6; })
   #define R_A7 ({ register APTR ___R_A7 __asm("r31"); ___R_A7; })
   #define R_D0 ({ register APTR ___R_D0 __asm("r16"); ___R_D0; })
   #define R_D1 ({ register APTR ___R_D1 __asm("r17"); ___R_D1; })
   #define R_D2 ({ register APTR ___R_D2 __asm("r18"); ___R_D2; })
   #define R_D3 ({ register APTR ___R_D3 __asm("r19"); ___R_D3; })
   #define R_D4 ({ register APTR ___R_D4 __asm("r20"); ___R_D4; })
   #define R_D5 ({ register APTR ___R_D5 __asm("r21"); ___R_D5; })
   #define R_D6 ({ register APTR ___R_D6 __asm("r22"); ___R_D6; })
   #define R_D7 ({ register APTR ___R_D7 __asm("r23"); ___R_D7; })
*/
#elif defined(__AROS__)

#endif


#endif
