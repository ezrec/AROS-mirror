/*
 * $Id$
 *
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 */

#ifndef AMITCP_TIME_H
#define AMITCP_TIME_H

#ifndef _SYS_TIME_H_
#include <sys/time.h>
#endif

/* Deleted a bunch of stuff. */

/*
 * We must define the timerequest, because compatible_timeval is not 
 * compatible with old timeval...
 */

#include <devices/timer.h>
/*
#include <proto/timer.h>
*/

/*
 * Operations on timevals.
 *
 */
#define	timerisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define	timerclear(tvp)		(tvp)->tv_sec = (tvp)->tv_usec = 0

#endif /* !AMITCP_TIME_H */

