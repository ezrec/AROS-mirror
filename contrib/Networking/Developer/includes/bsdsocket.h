#ifndef BSDSOCKET_H
#define BSDSOCKET_H
/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

*/

#ifndef UNISTD_H
#include <unistd.h>		/* These should be compiler independent */
#endif

#ifndef CLIB_NETLIB_PROTOS_H
#include <clib/netlib_protos.h>
#endif

#ifndef CLIB_SOCKET_PROTOS_H
#if defined(__SASC) || defined(__GNUC__)
#include <proto/bsdsocket.h>
#else
#include <clib/bsdsocket_protos.h>
#endif
#endif

#ifndef CLIB_USERGROUP_PROTOS_H
#if defined(__SASC) || defined(__GNUC__)
#ifndef __AROS__
#include <proto/usergroup.h>
#else
#include <clib/usergroup_protos.h>
#endif
#endif
#endif /* __AROS__ */
#endif /* !BSDSOCKET_H */
