/* Automatically generated header! Do not edit! */

#ifndef _INLINE_PREFERENCES_LIB_SFD_H
#define _INLINE_PREFERENCES_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef PREFERENCES_LIB_SFD_BASE_NAME
#define PREFERENCES_LIB_SFD_BASE_NAME PreferencesBase
#endif /* !PREFERENCES_LIB_SFD_BASE_NAME */

#define AllocPrefsHandle(___name) __AllocPrefsHandle_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___name)
#define __AllocPrefsHandle_WB(___base, ___name) \
	AROS_LC1(APTR, AllocPrefsHandle, \
	AROS_LCA(CONST_STRPTR, (___name), A0), \
	struct Library *, (___base), 5, Preferences_lib_sfd)

#define FreePrefsHandle(___prefsHandle) __FreePrefsHandle_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle)
#define __FreePrefsHandle_WB(___base, ___prefsHandle) \
	AROS_LC1NR(VOID, FreePrefsHandle, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	struct Library *, (___base), 6, Preferences_lib_sfd)

#define SetPreferences(___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size) __SetPreferences_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size)
#define __SetPreferences_WB(___base, ___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size) \
	AROS_LC5NR(VOID, SetPreferences, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___prefsTag), D1), \
	AROS_LCA(const APTR, (___a1arg), A1), \
	AROS_LCA(UWORD, (___struct_Size), D2), \
	struct Library *, (___base), 7, Preferences_lib_sfd)

#define GetPreferences(___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size) __GetPreferences_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size)
#define __GetPreferences_WB(___base, ___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size) \
	AROS_LC5(ULONG, GetPreferences, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___prefsTag), D1), \
	AROS_LCA(APTR, (___a1arg), A1), \
	AROS_LCA(UWORD, (___struct_Size), D2), \
	struct Library *, (___base), 8, Preferences_lib_sfd)

#define ReadPrefsHandle(___prefsHandle, ___filename) __ReadPrefsHandle_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle, ___filename)
#define __ReadPrefsHandle_WB(___base, ___prefsHandle, ___filename) \
	AROS_LC2NR(VOID, ReadPrefsHandle, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	AROS_LCA(CONST_STRPTR, (___filename), A1), \
	struct Library *, (___base), 9, Preferences_lib_sfd)

#define WritePrefsHandle(___prefsHandle, ___filename) __WritePrefsHandle_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle, ___filename)
#define __WritePrefsHandle_WB(___base, ___prefsHandle, ___filename) \
	AROS_LC2NR(VOID, WritePrefsHandle, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	AROS_LCA(CONST_STRPTR, (___filename), A1), \
	struct Library *, (___base), 10, Preferences_lib_sfd)

#define FindPreferences(___prefsHandle, ___iD, ___prefsTag) __FindPreferences_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle, ___iD, ___prefsTag)
#define __FindPreferences_WB(___base, ___prefsHandle, ___iD, ___prefsTag) \
	AROS_LC3(struct PrefsStruct  *, FindPreferences, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___prefsTag), D1), \
	struct Library *, (___base), 11, Preferences_lib_sfd)

#define SetEntry(___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size, ___entry) __SetEntry_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size, ___entry)
#define __SetEntry_WB(___base, ___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size, ___entry) \
	AROS_LC6NR(VOID, SetEntry, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___prefsTag), D1), \
	AROS_LCA(const APTR, (___a1arg), A1), \
	AROS_LCA(UWORD, (___struct_Size), D2), \
	AROS_LCA(ULONG, (___entry), D3), \
	struct Library *, (___base), 12, Preferences_lib_sfd)

#define GetEntry(___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size, ___entry) __GetEntry_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size, ___entry)
#define __GetEntry_WB(___base, ___prefsHandle, ___iD, ___prefsTag, ___a1arg, ___struct_Size, ___entry) \
	AROS_LC6(ULONG, GetEntry, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___prefsTag), D1), \
	AROS_LCA(APTR, (___a1arg), A1), \
	AROS_LCA(UWORD, (___struct_Size), D2), \
	AROS_LCA(ULONG, (___entry), D3), \
	struct Library *, (___base), 13, Preferences_lib_sfd)

#define RemEntry(___prefsHandle, ___iD, ___prefsTag, ___entry) __RemEntry_WB(PREFERENCES_LIB_SFD_BASE_NAME, ___prefsHandle, ___iD, ___prefsTag, ___entry)
#define __RemEntry_WB(___base, ___prefsHandle, ___iD, ___prefsTag, ___entry) \
	AROS_LC4(ULONG, RemEntry, \
	AROS_LCA(APTR, (___prefsHandle), A0), \
	AROS_LCA(ULONG, (___iD), D0), \
	AROS_LCA(ULONG, (___prefsTag), D1), \
	AROS_LCA(ULONG, (___entry), D2), \
	struct Library *, (___base), 14, Preferences_lib_sfd)

#endif /* !_INLINE_PREFERENCES_LIB_SFD_H */
