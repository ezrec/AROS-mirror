/*
 * $Id$
 *
 * Common types previously defined in multiple headers.
 *
 * Copyright (c) 1994 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * 
 *       Created: Thu Apr  7 22:50:12 1994 jraja
 * Last modified: Thu Apr  7 23:19:57 1994 jraja
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:17:12  henrik
 * *** empty log message ***
 *
 * Revision 3.1  1994/04/07  20:20:16  jraja
 * Initial revision.
 *
 */
#ifndef AMITCP_TYPES_H
#define AMITCP_TYPES_H


#ifndef _SYS_TYPES_H_ 
#include <sys/types.h>
#endif

/* I think these should probably be fixed to conform to ixemul if
   possible
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

/*

#ifndef NULL
#if __SASC && (__VERSION__ > 6 || __REVISION__ >= 50)
#include <sys/commnull.h>
#else
#define	NULL 0L
#endif
#endif

*/

#endif /* !AMITCP_TYPES_H */

