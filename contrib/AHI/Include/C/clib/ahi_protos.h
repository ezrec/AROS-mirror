#ifndef CLIB_AHI_PROTOS_H
#define CLIB_AHI_PROTOS_H

/*
**      $VER: ahi_protos.h 4.1 (2.4.97)
**      :ts=8 (TAB SIZE: 8)
**
**      C prototypes. For use with 32 bit integers only.
**
**      (C) Copyright 1994-2003 Martin Blom
**      All Rights Reserved.
**
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef  DEVICES_AHI_H
#include <devices/ahi.h>
#endif

struct AHIAudioCtrl *AHI_AllocAudioA( struct TagItem * );
struct AHIAudioCtrl *AHI_AllocAudio( Tag, ... );
void AHI_FreeAudio( struct AHIAudioCtrl * );
void AHI_KillAudio( void );
ULONG AHI_ControlAudioA( struct AHIAudioCtrl *, struct TagItem * );
ULONG AHI_ControlAudio( struct AHIAudioCtrl *, Tag, ... );
void AHI_SetVol( UWORD, Fixed, sposition, struct AHIAudioCtrl *, ULONG );
void AHI_SetFreq( UWORD, ULONG, struct AHIAudioCtrl *, ULONG );
void AHI_SetSound( UWORD, UWORD, ULONG, LONG, struct AHIAudioCtrl *, ULONG );
ULONG AHI_SetEffect( APTR, struct AHIAudioCtrl * );
ULONG AHI_LoadSound( UWORD, ULONG, APTR, struct AHIAudioCtrl * );
void AHI_UnloadSound( UWORD, struct AHIAudioCtrl * );
ULONG AHI_NextAudioID( ULONG );
BOOL AHI_GetAudioAttrsA( ULONG, struct AHIAudioCtrl *, struct TagItem * );
BOOL AHI_GetAudioAttrs( ULONG, struct AHIAudioCtrl *, Tag, ... );
ULONG AHI_BestAudioIDA( struct TagItem * );
ULONG AHI_BestAudioID( Tag, ... );
struct AHIAudioModeRequester *AHI_AllocAudioRequestA( struct TagItem * );
struct AHIAudioModeRequester *AHI_AllocAudioRequest( Tag, ... );
BOOL AHI_AudioRequestA( struct AHIAudioModeRequester *, struct TagItem * );
BOOL AHI_AudioRequest( struct AHIAudioModeRequester *, Tag, ... );
void AHI_FreeAudioRequest( struct AHIAudioModeRequester * );
/* --- New for V4 --- */
void AHI_PlayA( struct AHIAudioCtrl *, struct TagItem * );
void AHI_Play( struct AHIAudioCtrl *, Tag, ... );
ULONG AHI_SampleFrameSize( ULONG );
ULONG AHI_AddAudioMode(struct TagItem * );
ULONG AHI_RemoveAudioMode( ULONG );
ULONG AHI_LoadModeFile( STRPTR );
#endif /* CLIB_AHI_PROTOS_H */
