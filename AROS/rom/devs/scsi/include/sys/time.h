/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_TIME_H
#define SYS_TIME_H

#include <devices/timer.h>

#define min(a,b)        ((a) < (b) ? (a) : (b))
#define max(a,b)        ((a) > (b) ? (a) : (b))

static inline void timevalfix(struct timeval *t1)
{
    if (t1->tv_usec < 0) {
        t1->tv_sec--;
        t1->tv_usec += 1000000;
    }
    if (t1->tv_usec >= 1000000) {
        t1->tv_sec++;
        t1->tv_usec -= 1000000;
    }
}

static inline void timevalsub(struct timeval *t1, const struct timeval *t2)
{
    t1->tv_sec -= t2->tv_sec;
    t1->tv_usec -= t2->tv_usec;
    timevalfix(t1);
}

#define timevalclear(tvp)       (tvp)->tv_sec = (tvp)->tv_usec = 0
#define timevalcmp(tvp, uvp, cmp)                                       \
        (((tvp)->tv_sec == (uvp)->tv_sec) ?                             \
            ((tvp)->tv_usec cmp (uvp)->tv_usec) :                       \
            ((tvp)->tv_sec cmp (uvp)->tv_sec))


static inline void microuptime(struct timeval *tvp)
{
    /* FIXME: get uptime into *tvp */
    tvp->tv_sec = 0;
    tvp->tv_usec = 0;
}

#endif /* SYS_TIME_H */
