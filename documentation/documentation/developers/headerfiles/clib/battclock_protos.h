#ifndef CLIB_BATTCLOCK_PROTOS_H
#define CLIB_BATTCLOCK_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/battclock/battclock.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>


__BEGIN_DECLS

AROS_LP0(void, ResetBattClock,
         LIBBASETYPEPTR, BattClockBase, 1, Battclock
);
AROS_LP0(ULONG, ReadBattClock,
         LIBBASETYPEPTR, BattClockBase, 2, Battclock
);
AROS_LP1(void, WriteBattClock,
         AROS_LPA(ULONG, time, D0),
         LIBBASETYPEPTR, BattClockBase, 3, Battclock
);

__END_DECLS

#endif /* CLIB_BATTCLOCK_PROTOS_H */
