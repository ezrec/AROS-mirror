#ifndef CLIB_REALTIME_PROTOS_H
#define CLIB_REALTIME_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/realtime/realtime.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <utility/tagitem.h>

__BEGIN_DECLS

AROS_LP1(APTR, LockRealTime,
         AROS_LPA(ULONG, lockType, D0),
         LIBBASETYPEPTR, RealTimeBase, 5, RealTime
);
AROS_LP1(void, UnlockRealTime,
         AROS_LPA(APTR, lockHandle, A0),
         LIBBASETYPEPTR, RealTimeBase, 6, RealTime
);
AROS_LP1(struct Player *, CreatePlayerA,
         AROS_LPA(struct TagItem *, tagList, A0),
         LIBBASETYPEPTR, RealTimeBase, 7, RealTime
);
AROS_LP1(void, DeletePlayer,
         AROS_LPA(struct Player *, player, A0),
         LIBBASETYPEPTR, RealTimeBase, 8, RealTime
);
AROS_LP2(BOOL, SetPlayerAttrsA,
         AROS_LPA(struct Player *, player, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, RealTimeBase, 9, RealTime
);
AROS_LP3(LONG, SetConductorState,
         AROS_LPA(struct Player *, player, A0),
         AROS_LPA(ULONG, state, D0),
         AROS_LPA(LONG, time, D1),
         LIBBASETYPEPTR, RealTimeBase, 10, RealTime
);
AROS_LP3(BOOL, ExternalSync,
         AROS_LPA(struct Player *, player, A0),
         AROS_LPA(LONG, minTime, D0),
         AROS_LPA(LONG, maxTime, D1),
         LIBBASETYPEPTR, RealTimeBase, 11, RealTime
);
AROS_LP1(struct Conductor *, NextConductor,
         AROS_LPA(struct Conductor *, previousConductor, A0),
         LIBBASETYPEPTR, RealTimeBase, 12, RealTime
);
AROS_LP1(struct Conductor *, FindConductor,
         AROS_LPA(STRPTR, name, A0),
         LIBBASETYPEPTR, RealTimeBase, 13, RealTime
);
AROS_LP2(BOOL, GetPlayerAttrsA,
         AROS_LPA(struct Player *, player, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, RealTimeBase, 14, RealTime
);

__END_DECLS

#endif /* CLIB_REALTIME_PROTOS_H */
