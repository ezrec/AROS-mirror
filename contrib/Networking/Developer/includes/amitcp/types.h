#ifndef AMITCP_TYPES_H
#define AMITCP_TYPES_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
 */

#ifndef _UID_T
#define _UID_T long
typedef _UID_T uid_t;
#endif

#ifndef _GID_T
#define _GID_T long
typedef _GID_T gid_t;
#endif

#ifndef _PID_T
#define _PID_T struct Task *
typedef	_PID_T pid_t;			/* process id */
#endif

#ifndef _MODE_T
#define _MODE_T unsigned short 
typedef _MODE_T mode_t;
#endif

#ifndef _TIME_T
#define _TIME_T long
typedef _TIME_T time_t;
#endif

#ifndef NULL
#if __SASC && (__VERSION__ > 6 || __REVISION__ >= 50)
#include <sys/commnull.h>
#else
#define	NULL 0L
#endif
#endif

#endif /* !AMITCP_TYPES_H */
