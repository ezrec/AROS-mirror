#ifndef _INLINE_PREFERENCES_H
#define _INLINE_PREFERENCES_H

#ifndef CLIB_PREFERENCES_PROTOS_H
#define CLIB_PREFERENCES_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  SCALOS_PREFERENCES_H
#include <scalos/preferences.h>
#endif

#ifndef PREFERENCES_BASE_NAME
#define PREFERENCES_BASE_NAME PreferencesBase
#endif

#define AllocPrefsHandle(name) \
	LP1(0x1e, APTR, AllocPrefsHandle, CONST_STRPTR, name, a0, \
	, PREFERENCES_BASE_NAME)

#define FreePrefsHandle(prefsHandle) \
	LP1NR(0x24, FreePrefsHandle, APTR, prefsHandle, a0, \
	, PREFERENCES_BASE_NAME)

#define SetPreferences(prefsHandle, iD, prefsTag, a1arg, struct_Size) \
	LP5NR(0x2a, SetPreferences, APTR, prefsHandle, a0, ULONG, iD, d0, ULONG, prefsTag, d1, CONST APTR, a1arg, a1, ULONG, struct_Size, d2, \
	, PREFERENCES_BASE_NAME)

#define GetPreferences(prefsHandle, iD, prefsTag, a1arg, struct_Size) \
	LP5(0x30, ULONG, GetPreferences, APTR, prefsHandle, a0, ULONG, iD, d0, ULONG, prefsTag, d1, APTR, a1arg, a1, ULONG, struct_Size, d2, \
	, PREFERENCES_BASE_NAME)

#define ReadPrefsHandle(prefsHandle, filename) \
	LP2NR(0x36, ReadPrefsHandle, APTR, prefsHandle, a0, CONST_STRPTR, filename, a1, \
	, PREFERENCES_BASE_NAME)

#define WritePrefsHandle(prefsHandle, filename) \
	LP2NR(0x3c, WritePrefsHandle, APTR, prefsHandle, a0, CONST_STRPTR, filename, a1, \
	, PREFERENCES_BASE_NAME)

#define FindPreferences(prefsHandle, iD, prefsTag) \
	LP3(0x42, struct PrefsStruct *, FindPreferences, APTR, prefsHandle, a0, ULONG, iD, d0, ULONG, prefsTag, d1, \
	, PREFERENCES_BASE_NAME)

#define SetEntry(prefsHandle, iD, prefsTag, a1arg, struct_Size, entry) \
	LP6NR(0x48, SetEntry, APTR, prefsHandle, a0, ULONG, iD, d0, ULONG, prefsTag, d1, CONST APTR, a1arg, a1, ULONG, struct_Size, d2, ULONG, entry, d3, \
	, PREFERENCES_BASE_NAME)

#define GetEntry(prefsHandle, iD, prefsTag, a1arg, struct_Size, entry) \
	LP6(0x4e, ULONG, GetEntry, APTR, prefsHandle, a0, ULONG, iD, d0, ULONG, prefsTag, d1, APTR, a1arg, a1, ULONG, struct_Size, d2, ULONG, entry, d3, \
	, PREFERENCES_BASE_NAME)

#define RemEntry(prefsHandle, iD, prefsTag, entry) \
	LP4(0x54, ULONG, RemEntry, APTR, prefsHandle, a0, ULONG, iD, d0, ULONG, prefsTag, d1, ULONG, entry, d2, \
	, PREFERENCES_BASE_NAME)

#endif /*  _INLINE_PREFERENCES_H  */
