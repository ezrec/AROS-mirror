/*
 * $Id$
 *
 * Compiler dependent prototypes and inlines for bsdsocket.library
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 */
#ifndef BSDSOCKET_H
#define BSDSOCKET_H


/* These are compiler independent */
#include <unistd.h>
#include <clib/netlib_protos.h>

/* these need gcc, but shouldn't be included when compiling
   AmiTCP itself, no way! */
#ifndef KERNEL
#include <proto/socket.h>
#include <proto/usergroup.h>
#endif /* KERNEL */

#endif /* !BSDSOCKET_H */
