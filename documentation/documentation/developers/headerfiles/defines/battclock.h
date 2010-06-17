#ifndef DEFINES_BATTCLOCK_H
#define DEFINES_BATTCLOCK_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/battclock/battclock.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for battclock
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __ResetBattClock_WB(__BattClockBase) \
        AROS_LC0NR(void, ResetBattClock, \
        struct Library *, (__BattClockBase), 1, Battclock)

#define ResetBattClock() \
    __ResetBattClock_WB(BattClockBase)

#define __ReadBattClock_WB(__BattClockBase) \
        AROS_LC0(ULONG, ReadBattClock, \
        struct Library *, (__BattClockBase), 2, Battclock)

#define ReadBattClock() \
    __ReadBattClock_WB(BattClockBase)

#define __WriteBattClock_WB(__BattClockBase, __arg1) \
        AROS_LC1NR(void, WriteBattClock, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct Library *, (__BattClockBase), 3, Battclock)

#define WriteBattClock(arg1) \
    __WriteBattClock_WB(BattClockBase, (arg1))

__END_DECLS

#endif /* DEFINES_BATTCLOCK_H*/
