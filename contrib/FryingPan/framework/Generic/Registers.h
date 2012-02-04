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

#ifndef __REGISTERS
#define __REGISTERS

// what U see here sucks allot but I can't help it.
// so far it is the only way I know...

#if defined( __mc68000 )

   #define XGETA0(var) asm("mov.l a0,%0\n\t" : "=g"(var))
   #define XGETA1(var) asm("mov.l a1,%0\n\t" : "=g"(var))
   #define XGETA2(var) asm("mov.l a2,%0\n\t" : "=g"(var))
   #define XGETA3(var) asm("mov.l a3,%0\n\t" : "=g"(var))
   #define XGETA4(var) asm("mov.l a4,%0\n\t" : "=g"(var))
   #define XGETA5(var) asm("mov.l a5,%0\n\t" : "=g"(var))
   #define XGETA6(var) asm("mov.l a6,%0\n\t" : "=g"(var))
   #define XGETA7(var) asm("mov.l a7,%0\n\t" : "=g"(var))
   #define XGETD0(var) asm("mov.l d0,%0\n\t" : "=g"(var))
   #define XGETD1(var) asm("mov.l d1,%0\n\t" : "=g"(var))
   #define XGETD2(var) asm("mov.l d2,%0\n\t" : "=g"(var))
   #define XGETD3(var) asm("mov.l d3,%0\n\t" : "=g"(var))
   #define XGETD4(var) asm("mov.l d4,%0\n\t" : "=g"(var))
   #define XGETD5(var) asm("mov.l d5,%0\n\t" : "=g"(var))
   #define XGETD6(var) asm("mov.l d6,%0\n\t" : "=g"(var))
   #define XGETD7(var) asm("mov.l d7,%0\n\t" : "=g"(var))
   
   #define GETA0(var) ({register long* ___varA0 asm("a0"); ((long*)var) = ___varA0;})
   #define GETA1(var) ({register long* ___varA1 asm("a1"); ((long*)var) = ___varA1;})
   #define GETA2(var) ({register long* ___varA2 asm("a2"); ((long*)var) = ___varA2;})
   #define GETA3(var) ({register long* ___varA3 asm("a3"); ((long*)var) = ___varA3;})
   #define GETA4(var) ({register long* ___varA4 asm("a4"); ((long*)var) = ___varA4;})
   #define GETA5(var) ({register long* ___varA5 asm("a5"); ((long*)var) = ___varA5;})
   #define GETA6(var) ({register long* ___varA6 asm("a6"); ((long*)var) = ___varA6;})
   #define GETA7(var) ({register long* ___varA7 asm("a7"); ((long*)var) = ___varA7;})
   #define GETD0(var) ({register long ___varD0 asm("d0"); ((long*)var) = ___varD0;})
   #define GETD1(var) ({register long ___varD1 asm("d1"); ((long*)var) = ___varD1;})
   #define GETD2(var) ({register long ___varD2 asm("d2"); ((long*)var) = ___varD2;})
   #define GETD3(var) ({register long ___varD3 asm("d3"); ((long*)var) = ___varD3;})
   #define GETD4(var) ({register long ___varD4 asm("d4"); ((long*)var) = ___varD4;})
   #define GETD5(var) ({register long ___varD5 asm("d5"); ((long*)var) = ___varD5;})
   #define GETD6(var) ({register long ___varD6 asm("d6"); ((long*)var) = ___varD6;})
   #define GETD7(var) ({register long ___varD7 asm("d7"); ((long*)var) = ___varD7;})

#elif defined(__MORPHOS__)
#elif defined(__AROS__)
#endif


#endif
