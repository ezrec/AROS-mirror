#ifndef CLIB_MYSTICVIEW_PROTOS_H
#define CLIB_MYSTICVIEW_PROTOS_H

/*
    *** Automatically generated from 'mysticview.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <utility/tagitem.h>
#include <intuition/screens.h>
AROS_LP3(APTR, MV_CreateA,
         AROS_LPA(struct Screen *, screen, A0),
         AROS_LPA(struct RastPort *, rastport, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, MysticBase, 5, MysticView
);
AROS_LP1(void, MV_Delete,
         AROS_LPA(APTR, mview, A0),
         LIBBASETYPEPTR, MysticBase, 6, MysticView
);
AROS_LP2(void, MV_SetAttrsA,
         AROS_LPA(APTR, mview, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, MysticBase, 7, MysticView
);
AROS_LP1(BOOL, MV_DrawOn,
         AROS_LPA(APTR, mview, A0),
         LIBBASETYPEPTR, MysticBase, 8, MysticView
);
AROS_LP1(void, MV_DrawOff,
         AROS_LPA(APTR, mview, A0),
         LIBBASETYPEPTR, MysticBase, 9, MysticView
);
AROS_LP1(void, MV_Refresh,
         AROS_LPA(APTR, mview, A0),
         LIBBASETYPEPTR, MysticBase, 10, MysticView
);
AROS_LP2(void, MV_GetAttrsA,
         AROS_LPA(APTR, mview, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, MysticBase, 11, MysticView
);
AROS_LP3(void, MV_SetViewStart,
         AROS_LPA(APTR, mview, A0),
         AROS_LPA(LONG, x, D0),
         AROS_LPA(LONG, y, D1),
         LIBBASETYPEPTR, MysticBase, 12, MysticView
);
AROS_LP3(void, MV_SetViewRelative,
         AROS_LPA(APTR, mview, A0),
         AROS_LPA(LONG, x, D0),
         AROS_LPA(LONG, y, D1),
         LIBBASETYPEPTR, MysticBase, 13, MysticView
);
AROS_LP2(void, MV_Lock,
         AROS_LPA(APTR, mview, A0),
         AROS_LPA(BOOL, lock, D0),
         LIBBASETYPEPTR, MysticBase, 14, MysticView
);

#endif /* CLIB_MYSTICVIEW_PROTOS_H */
