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

#ifndef _MIXER_H_
#define _MIXER_H_

#include <config.h>
#include <CompilerSpecific.h>
#include "ahi_def.h"

#include "addroutines.h"

ULONG
InternalSampleFrameSize( ULONG sampletype );

BOOL
InitMixroutine ( struct AHIPrivAudioCtrl *audioctrl );

void
CleanUpMixroutine ( struct AHIPrivAudioCtrl *audioctrl );

void
SelectAddRoutine ( Fixed     VolumeLeft,
                   Fixed     VolumeRight,
                   ULONG     SampleType,
                   struct    AHIPrivAudioCtrl *audioctrl,
                   LONG     *ScaleLeft,
                   LONG     *ScaleRight,
                   ADDFUNC **AddRoutine );

#if !defined( VERSIONPPC )

void ASMCALL
MixM68K ( REG(a0, struct Hook *Hook),
             REG(a1, void *dst),
             REG(a2, struct AHIPrivAudioCtrl *audioctrl) );

void ASMCALL
MixPowerPC( REG(a0, struct Hook *Hook), 
            REG(a1, void *dst), 
            REG(a2, struct AHIPrivAudioCtrl *audioctrl) );

void
DoOutputBuffer ( void *buffer,
                 struct AHIPrivAudioCtrl *audioctrl );

void
DoChannelInfo ( struct AHIPrivAudioCtrl *audioctrl );

#endif /* !defined( VERSIONPPC ) */

void
Mix( struct Hook *Hook,
     void *dst,
     struct AHIPrivAudioCtrl *audioctrl );

LONG
CalcSamples ( Fixed64 Add,
              ULONG   Type,
              Fixed64 LastOffset,
              Fixed64 Offset );

void
DoMasterVolume ( void *buffer,
                 struct AHIPrivAudioCtrl *audioctrl );

#endif /* _MIXER_H_ */
