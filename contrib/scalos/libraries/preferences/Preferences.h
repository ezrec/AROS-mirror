// Preferences.h
// $Date$
// $Revision$


#ifndef PREFERENCES_H
#define	PREFERENCES_H

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/libraries.h>

#include <defs.h>

#define	LIB_VERSION	41
#define	LIB_REVISION	2

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

extern struct ExecBase *SysBase;

//----------------------------------------------------------------------------

BOOL PreferencesInit(struct PreferencesBase *PreferencesBase);
BOOL PreferencesOpen(struct PreferencesBase *PreferencesBase);
void PreferencesCleanup(struct PreferencesBase *PreferencesBase);

LIBFUNC_P2_PROTO(APTR, LIBAllocPrefsHandle,
	A0, CONST_STRPTR, Name,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P2_PROTO(void, LIBFreePrefsHandle,
	A0, APTR, PrefsHandle,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P6_PROTO(void, LIBSetPreferences,
	A0, APTR, PrefsHandle,
	D0, ULONG, id,
	D1, ULONG, tag,
	A1, const APTR, Struct,
	D2, UWORD, StructSize,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P6_PROTO(ULONG, LIBGetPreferences,
	A0, APTR, PrefsHandle,
	D0, ULONG, id,
	D1, ULONG, tag,
	A1, APTR, Struct,
	D2, UWORD, StructSize,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P3_PROTO(void, LIBReadPrefsHandle,
	A0, APTR, PrefsHandle,
	A1, CONST_STRPTR, Filename,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P3_PROTO(void, LIBWritePrefsHandle,
	A0, APTR, PrefsHandle,
	A1, CONST_STRPTR, Filename,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P4_PROTO(struct PrefsStruct *, LIBFindPreferences,
	A0, APTR, PrefsHandle,
	D0, ULONG, ID,
	D1, ULONG, tag,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P7_PROTO(void, LIBSetEntry,
	A0, APTR, PrefsHandle,
	D0, ULONG, ID,
	D1, ULONG, tag,
	A1, const APTR, Struct,
	D2, UWORD, StructSize,
	D3, ULONG, Entry,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P7_PROTO(ULONG, LIBGetEntry,
	A0, APTR, PrefsHandle,
	D0, ULONG, ID,
	D1, ULONG, tag,
	A1, const APTR, Struct,
	D2, UWORD, StructSize,
	D3, ULONG, Entry,
	A6, struct PreferencesBase *, PreferencesBase);
LIBFUNC_P5_PROTO(ULONG, LIBRemEntry,
	A0, APTR, PrefsHandle,
	D0, ULONG, ID,
	D1, ULONG, tag,
	D2, ULONG, Entry,
	A6, struct PreferencesBase *, PreferencesBase);

//----------------------------------------------------------------------------

#endif /* PREFERENCES_H */
