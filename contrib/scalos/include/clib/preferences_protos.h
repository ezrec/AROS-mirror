#ifndef  CLIB_PREFERENCES_PROTOS_H
#define  CLIB_PREFERENCES_PROTOS_H

/*
**	$Id$
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  SCALOS_PREFERENCES_H
#include <scalos/preferences.h>
#endif
APTR AllocPrefsHandle( CONST_STRPTR name );
VOID FreePrefsHandle( APTR prefsHandle );
VOID SetPreferences( APTR prefsHandle, ULONG iD, ULONG prefsTag, CONST APTR a1arg, ULONG struct_Size );
ULONG GetPreferences( APTR prefsHandle, ULONG iD, ULONG prefsTag, APTR a1arg, ULONG struct_Size );
VOID ReadPrefsHandle( APTR prefsHandle, CONST_STRPTR filename );
VOID WritePrefsHandle( APTR prefsHandle, CONST_STRPTR filename );
struct PrefsStruct *FindPreferences( APTR prefsHandle, ULONG iD, ULONG prefsTag );
VOID SetEntry( APTR prefsHandle, ULONG iD, ULONG prefsTag, CONST APTR a1arg, ULONG struct_Size, ULONG entry );
ULONG GetEntry( APTR prefsHandle, ULONG iD, ULONG prefsTag, APTR a1arg, ULONG struct_Size, ULONG entry );
ULONG RemEntry( APTR prefsHandle, ULONG iD, ULONG prefsTag, ULONG entry );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_PREFERENCES_PROTOS_H */
