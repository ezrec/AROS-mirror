#ifndef CLIB_AHISUB_PROTOS_H
#define CLIB_AHISUB_PROTOS_H

/*
**	$VER: ahi_sub_protos.h 4.1 (2.4.97)
**	:ts=8 (TAB SIZE: 8)
**
**	C prototypes. For use with 32 bit integers only.
**
**	(C) Copyright 1994-2003 Martin Blom
**	All Rights Reserved.
**
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef  LIBRARIES_AHISUB_H
#include <libraries/ahi_sub.h>
#endif

ULONG AHIsub_AllocAudio( struct TagItem *, struct AHIAudioCtrlDrv * );
void AHIsub_FreeAudio( struct AHIAudioCtrlDrv * );
void AHIsub_Disable( struct AHIAudioCtrlDrv * );
void AHIsub_Enable( struct AHIAudioCtrlDrv * );
ULONG AHIsub_Start(ULONG, struct AHIAudioCtrlDrv * );
ULONG AHIsub_Update(ULONG, struct AHIAudioCtrlDrv * );
ULONG AHIsub_Stop( ULONG, struct AHIAudioCtrlDrv * );
ULONG AHIsub_SetVol( UWORD, Fixed, sposition, struct AHIAudioCtrlDrv *, ULONG );
ULONG AHIsub_SetFreq( UWORD, ULONG, struct AHIAudioCtrlDrv *, ULONG );
ULONG AHIsub_SetSound( UWORD, UWORD, ULONG, LONG, struct AHIAudioCtrlDrv *, ULONG );
ULONG AHIsub_SetEffect( APTR, struct AHIAudioCtrlDrv * );
ULONG AHIsub_LoadSound( UWORD, ULONG, APTR, struct AHIAudioCtrlDrv * );
ULONG AHIsub_UnloadSound( UWORD, struct AHIAudioCtrlDrv * );
LONG AHIsub_GetAttr( ULONG, LONG, LONG, struct TagItem *, struct AHIAudioCtrlDrv * );
LONG AHIsub_HardwareControl( ULONG, LONG, struct AHIAudioCtrlDrv *);
#endif /* CLIB_AHI_PROTOS_H */
