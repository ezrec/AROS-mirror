#ifndef DEFINES_TIMER_H
#define DEFINES_TIMER_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/timer/timer.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for timer
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __AddTime_WB(__TimerBase, __arg1, __arg2) \
        AROS_LC2NR(void, AddTime, \
                  AROS_LCA(struct timeval *,(__arg1),A0), \
                  AROS_LCA(struct timeval *,(__arg2),A1), \
        struct Device *, (__TimerBase), 7, Timer)

#define AddTime(arg1, arg2) \
    __AddTime_WB(TimerBase, (arg1), (arg2))

#define __SubTime_WB(__TimerBase, __arg1, __arg2) \
        AROS_LC2NR(void, SubTime, \
                  AROS_LCA(struct timeval *,(__arg1),A0), \
                  AROS_LCA(struct timeval *,(__arg2),A1), \
        struct Device *, (__TimerBase), 8, Timer)

#define SubTime(arg1, arg2) \
    __SubTime_WB(TimerBase, (arg1), (arg2))

#define __CmpTime_WB(__TimerBase, __arg1, __arg2) \
        AROS_LC2(LONG, CmpTime, \
                  AROS_LCA(struct timeval *,(__arg1),A0), \
                  AROS_LCA(struct timeval *,(__arg2),A1), \
        struct Device *, (__TimerBase), 9, Timer)

#define CmpTime(arg1, arg2) \
    __CmpTime_WB(TimerBase, (arg1), (arg2))

#define __ReadEClock_WB(__TimerBase, __arg1) \
        AROS_LC1(ULONG, ReadEClock, \
                  AROS_LCA(struct EClockVal *,(__arg1),A0), \
        struct Device *, (__TimerBase), 10, Timer)

#define ReadEClock(arg1) \
    __ReadEClock_WB(TimerBase, (arg1))

#define __GetSysTime_WB(__TimerBase, __arg1) \
        AROS_LC1NR(void, GetSysTime, \
                  AROS_LCA(struct timeval *,(__arg1),A0), \
        struct Device *, (__TimerBase), 11, Timer)

#define GetSysTime(arg1) \
    __GetSysTime_WB(TimerBase, (arg1))

__END_DECLS

#endif /* DEFINES_TIMER_H*/
