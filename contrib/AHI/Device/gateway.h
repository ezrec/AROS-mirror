/* $Id$ */

/*
     AHI - Hardware independent audio subsystem
     Copyright (C) 1996-2003 Martin Blom <martin@blom.org>
     
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

#ifndef ahi_gateway_h
#define ahi_gateway_h

#include <config.h>
#include <CompilerSpecific.h>

/* Library entry points (native) */

void gw_initRoutine( void );
void gw_DevExpunge( void );
void gw_DevOpen( void );
void gw_DevClose( void );
void gw_DevBeginIO( void );
void gw_DevAbortIO( void );
void gw_AllocAudioA( void );
void gw_FreeAudio( void );
void gw_KillAudio( void );
void gw_ControlAudioA( void );
void gw_SetVol( void );
void gw_SetFreq( void );
void gw_SetSound( void );
void gw_SetEffect( void );
void gw_LoadSound( void );
void gw_UnloadSound( void );
void gw_PlayA( void );
void gw_SampleFrameSize( void );
void gw_GetAudioAttrsA( void );
void gw_BestAudioIDA( void );
void gw_AllocAudioRequestA( void );
void gw_AudioRequestA( void );
void gw_FreeAudioRequest( void );
void gw_NextAudioID( void );
void gw_AddAudioMode( void );
void gw_RemoveAudioMode( void );
void gw_LoadModeFile( void );

void m68k_IndexToFrequency( void );
void m68k_DevProc( void );

/* Special hook entry points */

void HookEntryPreserveAllRegs( void );

/* (Pre|Post)Timer entry points */

void PreTimerPreserveAllRegs( void );
void PostTimerPreserveAllRegs( void );

#endif /* ahi_gateway_h */
