#ifndef ARPA_INET_H
#define ARPA_INET_H
/*
 * $Id$
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * Inet Library Functions 
 *
 */

#ifndef KERNEL

#ifndef IN_H
#include <netinet/in.h>
#endif

/*
 * Include socket protos/inlines/pragmas
 */
#ifndef BSDSOCKET_H
#include <bsdsocket.h>
#endif

#endif /* !KERNEL */

#endif /* ARPA_INET_H */
