#ifndef CLIB_TIMER_PROTOS_H
#define CLIB_TIMER_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/timer/timer.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <devices/timer.h>

__BEGIN_DECLS

AROS_LP2(void, AddTime,
         AROS_LPA(struct timeval *, dest, A0),
         AROS_LPA(struct timeval *, src, A1),
         LIBBASETYPEPTR, TimerBase, 7, Timer
);
AROS_LP2(void, SubTime,
         AROS_LPA(struct timeval *, dest, A0),
         AROS_LPA(struct timeval *, src, A1),
         LIBBASETYPEPTR, TimerBase, 8, Timer
);
AROS_LP2(LONG, CmpTime,
         AROS_LPA(struct timeval *, dest, A0),
         AROS_LPA(struct timeval *, src, A1),
         LIBBASETYPEPTR, TimerBase, 9, Timer
);
AROS_LP1(ULONG, ReadEClock,
         AROS_LPA(struct EClockVal *, dest, A0),
         LIBBASETYPEPTR, TimerBase, 10, Timer
);
AROS_LP1(void, GetSysTime,
         AROS_LPA(struct timeval *, dest, A0),
         LIBBASETYPEPTR, TimerBase, 11, Timer
);

__END_DECLS

#endif /* CLIB_TIMER_PROTOS_H */
