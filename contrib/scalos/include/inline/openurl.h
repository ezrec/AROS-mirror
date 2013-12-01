#ifndef _INLINE_OPENURL_H
#define _INLINE_OPENURL_H

#ifndef CLIB_OPENURL_PROTOS_H
#define CLIB_OPENURL_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef OPENURL_BASE_NAME
#define OPENURL_BASE_NAME OpenURLBase
#endif

#define URL_OpenA(url, tags) \
	LP2(0x1e, ULONG, URL_OpenA, STRPTR, url, a0, struct TagItem *, tags, a1, \
	, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_Open(url, tags...) \
	({ULONG _tags[] = {tags}; URL_OpenA((url), (struct TagItem *) _tags);})
#endif

#define URL_OldGetPrefs() \
	LP0(0x24, struct URL_Prefs *, URL_OldGetPrefs, \
	, OPENURL_BASE_NAME)

#define URL_OldFreePrefs(up) \
	LP1NR(0x2a, URL_OldFreePrefs, struct URL_Prefs *, up, a0, \
	, OPENURL_BASE_NAME)

#define URL_OldSetPrefs(up, permanent) \
	LP2(0x30, ULONG, URL_OldSetPrefs, struct URL_Prefs *, up, a0, LONG, permanent, d0, \
	, OPENURL_BASE_NAME)

#define URL_OldGetDefaultPrefs() \
	LP0(0x36, struct URL_Prefs *, URL_OldGetDefaultPrefs, \
	, OPENURL_BASE_NAME)

#define URL_OldLaunchPrefsApp() \
	LP0(0x3c, ULONG, URL_OldLaunchPrefsApp, \
	, OPENURL_BASE_NAME)

#define URL_GetPrefsA(tags) \
	LP1(0x48, struct URL_Prefs *, URL_GetPrefsA, struct TagItem *, tags, a0, \
	, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_GetPrefs(tags...) \
	({ULONG _tags[] = {tags}; URL_GetPrefsA((struct TagItem *) _tags);})
#endif

#define URL_FreePrefsA(prefs, tags) \
	LP2NR(0x4e, URL_FreePrefsA, struct URL_Prefs *, prefs, a0, struct TagItem *, tags, a1, \
	, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_FreePrefs(prefs, tags...) \
	({ULONG _tags[] = {tags}; URL_FreePrefsA((prefs), (struct TagItem *) _tags);})
#endif

#define URL_SetPrefsA(up, tags) \
	LP2(0x54, ULONG, URL_SetPrefsA, struct URL_Prefs *, up, a0, struct TagItem *, tags, a1, \
	, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_SetPrefs(up, tags...) \
	({ULONG _tags[] = {tags}; URL_SetPrefsA((up), (struct TagItem *) _tags);})
#endif

#define URL_LaunchPrefsAppA(tags) \
	LP1(0x5a, ULONG, URL_LaunchPrefsAppA, struct TagItem *, tags, a0, \
	, OPENURL_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define URL_LaunchPrefsApp(tags...) \
	({ULONG _tags[] = {tags}; URL_LaunchPrefsAppA((struct TagItem *) _tags);})
#endif

#define URL_GetAttr(attr, storage) \
	LP2(0x60, ULONG, URL_GetAttr, ULONG, attr, d0, ULONG *, storage, a0, \
	, OPENURL_BASE_NAME)

#endif /*  _INLINE_OPENURL_H  */
