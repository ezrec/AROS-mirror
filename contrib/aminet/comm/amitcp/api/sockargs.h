/*
 * $Id$
 * 
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * Created: Wed Jan  5 19:25:16 1994
 * Last modified: Wed Jan  5 19:27:36 1994 too
 * 
 * $Log$
 * Revision 1.2  2001/12/26 19:30:19  henrik
 * Aros version
 *
 * Revision 1.1  2001/12/25 22:21:39  henrik
 *
 * Revision 3.1  1994/01/06  13:39:42  too
 * extern prototype for sockArgs() which resides in amiga_syscalls.c
 *
 */

#ifndef API_SOCKARGS_H
#define API_SOCKARGS_H


#ifndef _SYS_TYPES_H_ 
#include <sys/types.h>
#endif

#ifndef  _SYS_MBUF_H_
#include <sys/mbuf.h>
#endif


/*
 * sockArgs code in amiga_syscalls.c
 */
long sockArgs(struct mbuf **mp, caddr_t buf, long buflen, long type);

#endif /* API_SOCKARGS_H */
