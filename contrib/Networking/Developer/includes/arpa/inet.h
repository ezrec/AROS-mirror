#ifndef ARPA_INET_H
#define ARPA_INET_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef KERNEL
#ifndef NETINET_IN_H
#include <netinet/in.h>
#endif

/*
 * Include socket protos/inlines/pragmas
 */
#if !defined(BSDSOCKET_H) && !defined(CLIB_SOCKET_PROTOS_H)
#include <bsdsocket.h>
#endif

#endif /* !KERNEL */

#endif /* ARPA_INET_H */
