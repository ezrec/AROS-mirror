#ifndef CLIB_OPENURL_PROTOS_H
#define CLIB_OPENURL_PROTOS_H

/*
    *** Automatically generated from 'openurl.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <libraries/openurl.h>

ULONG URL_Open(STRPTR, Tag tags, ...);
struct URL_Prefs *URL_GetPrefs(Tag tags, ...);
VOID URL_FreePrefs(struct URL_Prefs *, Tag tags, ...);
ULONG URL_SetPrefs(struct URL_Prefs *, Tag tags, ...);
ULONG URL_LaunchPrefsApp(Tag tags, ...);
AROS_LP2(ULONG, URL_OpenA,
         AROS_LPA(STRPTR, URL, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, OpenURLBase, 5, Openurl
);
AROS_LP1(struct URL_Prefs *, URL_GetPrefsA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, OpenURLBase, 12, Openurl
);
AROS_LP2(VOID, URL_FreePrefsA,
         AROS_LPA(struct URL_Prefs *, p, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, OpenURLBase, 13, Openurl
);
AROS_LP2(ULONG, URL_SetPrefsA,
         AROS_LPA(struct URL_Prefs *, p, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, OpenURLBase, 14, Openurl
);
AROS_LP1(ULONG, URL_LaunchPrefsAppA,
         AROS_LPA(struct TagItem *, tags, A0),
         LIBBASETYPEPTR, OpenURLBase, 15, Openurl
);
AROS_LP2(ULONG, URL_GetAttr,
         AROS_LPA(ULONG, attr, D0),
         AROS_LPA(IPTR *, storage, A0),
         LIBBASETYPEPTR, OpenURLBase, 16, Openurl
);

#endif /* CLIB_OPENURL_PROTOS_H */
