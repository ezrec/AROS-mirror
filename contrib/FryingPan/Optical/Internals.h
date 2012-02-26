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

#ifndef __Optical_Internals
#define __Optical_Internals

#include <exec/libraries.h>
#include <exec/execbase.h>
#include <libclass/exec.h>
#include <utility/hooks.h>
#include <exec/ports.h>
#include <exec/semaphores.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <devices/scsidisk.h>


   extern APTR Pool;

#include "Optical.h"
#include <Generic/Debug.h>

#undef Exception

   #ifndef AMIGA
      #pragma pack(1)
   #endif 
 
 
   #ifdef __amigaos4__
      #define __UtilBase Library
      #define IPTR LONG
      #define MEMF_SEM_PROTECTED 0
      #define NP_CodeType TAG_IGNORE
      #define CODETYPE_PPC 0
   #elif defined( __MORPHOS__ )
      #define __UtilBase Library
      #define IPTR LONG 
      #define MEMF_SHARED MEMF_PUBLIC
   #else
      #define __UtilBase     UtilityBase
      #define NP_CodeType    TAG_IGNORE 
      #define MEMF_SHARED    0
      #define CODETYPE_PPC   0
      #ifdef __AROS__
         #undef Printf
         #define Printf(a, b...) VPrintf(a, (IPTR*)ARRAY(b))
         #undef FPrintf
         #define FPrintf(a, b, c...) VFPrintf(a, b, (IPTR*)ARRAY(c))
      #else
         #define MEMF_SEM_PROTECTED 0
         #define IPTR LONG
      #endif
   #endif

   #ifndef __MORPHOS__
      #define R_A0 ({ register APTR ___R_A0 __asm("a0"); ___R_A0; })
   #define R_A1 ({ register APTR ___R_A1 __asm("a1"); ___R_A1; })
   #define R_A2 ({ register APTR ___R_A2 __asm("a2"); ___R_A2; })
   #define R_A3 ({ register APTR ___R_A3 __asm("a3"); ___R_A3; })
   #define R_A4 ({ register APTR ___R_A4 __asm("a4"); ___R_A4; })
   #define R_A5 ({ register APTR ___R_A5 __asm("a5"); ___R_A5; })
   #define R_A6 ({ register APTR ___R_A6 __asm("a6"); ___R_A6; })
   #define R_A7 ({ register APTR ___R_A7 __asm("a7"); ___R_A7; })
   #define R_D0 ({ register APTR ___R_D0 __asm("d0"); ___R_D0; })
   #define R_D1 ({ register APTR ___R_D1 __asm("d1"); ___R_D1; })
   #define R_D2 ({ register APTR ___R_D2 __asm("d2"); ___R_D2; })
   #define R_D3 ({ register APTR ___R_D3 __asm("d3"); ___R_D3; })
   #define R_D4 ({ register APTR ___R_D4 __asm("d4"); ___R_D4; })
   #define R_D5 ({ register APTR ___R_D5 __asm("d5"); ___R_D5; })
   #define R_D6 ({ register APTR ___R_D6 __asm("d6"); ___R_D6; })
   #define R_D7 ({ register APTR ___R_D7 __asm("d7"); ___R_D7; })

   #define REG_A0 ({ register APTR ___R_A0 __asm("a0"); ___R_A0; })
   #define REG_A1 ({ register APTR ___R_A1 __asm("a1"); ___R_A1; })
   #define REG_A2 ({ register APTR ___R_A2 __asm("a2"); ___R_A2; })
   #define REG_A3 ({ register APTR ___R_A3 __asm("a3"); ___R_A3; })
   #define REG_A4 ({ register APTR ___R_A4 __asm("a4"); ___R_A4; })
   #define REG_A5 ({ register APTR ___R_A5 __asm("a5"); ___R_A5; })
   #define REG_A6 ({ register APTR ___R_A6 __asm("a6"); ___R_A6; })
   #define REG_A7 ({ register APTR ___R_A7 __asm("a7"); ___R_A7; })
   #define REG_D0 ({ register APTR ___R_D0 __asm("d0"); ___R_D0; })
   #define REG_D1 ({ register APTR ___R_D1 __asm("d1"); ___R_D1; })
   #define REG_D2 ({ register APTR ___R_D2 __asm("d2"); ___R_D2; })
   #define REG_D3 ({ register APTR ___R_D3 __asm("d3"); ___R_D3; })
   #define REG_D4 ({ register APTR ___R_D4 __asm("d4"); ___R_D4; })
   #define REG_D5 ({ register APTR ___R_D5 __asm("d5"); ___R_D5; })
   #define REG_D6 ({ register APTR ___R_D6 __asm("d6"); ___R_D6; })
   #define REG_D7 ({ register APTR ___R_D7 __asm("d7"); ___R_D7; })
#else
   #define R_A0 REG_A0
   #define R_A1 REG_A1
   #define R_A2 REG_A2
   #define R_A3 REG_A3
   #define R_A4 REG_A4
   #define R_A5 REG_A5
   #define R_A6 REG_A6
   #define R_A7 REG_A7
   #define R_D0 REG_D0
   #define R_D1 REG_D1
   #define R_D2 REG_D2
   #define R_D3 REG_D3
   #define R_D4 REG_D4
   #define R_D5 REG_D5
   #define R_D6 REG_D6
   #define R_D7 REG_D7
#endif

//   extern   APTR                 Pool;

#ifdef __AMIGAOS4__
   extern   struct UtilityIFace *IUtility;
#endif


#define MEM_FLAGS (MEMF_PUBLIC | MEMF_CLEAR | MEMF_SEM_PROTECTED | MEMF_SHARED)




#endif
