#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

/*
    Copyright © 2004, The AROS Development Team. All rights reserved.
    $Id: times.h 30792 2009-03-07 22:40:04Z neil $
*/

#include <sys/types.h>

struct tms 
{
    clock_t tms_utime;  /* User time */
    clock_t tms_stime;  /* System time */
    clock_t tms_cutime; /* User time of children */
    clock_t tms_cstime; /* System time of children */
};

__BEGIN_DECLS
clock_t times(struct tms *buffer);
__END_DECLS

#endif /* _SYS_TIMES_H */

