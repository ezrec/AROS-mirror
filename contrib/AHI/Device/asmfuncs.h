/* $Id$ */

/*
     AHI - Hardware independent audio subsystem
     Copyright (C) 1996-1999 Martin Blom <martin@blom.org>
     
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
     
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
     
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the
     Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
     MA 02139, USA.
*/

#ifndef _ASMFUNCS_H_
#define _ASMFUNCS_H_

#include <config.h>
#include <CompilerSpecific.h>
#include "ahi_def.h"

char STDARGS *Sprintf(char *dst, const char *fmt, ...);
BOOL ASMCALL PreTimer ( void );
void ASMCALL PostTimer ( void );
BOOL ASMCALL DummyPreTimer ( void );
void ASMCALL DummyPostTimer ( void );

void ASMCALL MixEntry( REG(a0, struct Hook *Hook), 
                       REG(a1, void *dst), 
                       REG(a2, struct AHIPrivAudioCtrl *audioctrl) );

#endif /* _ASMFUNCS_H_ */
