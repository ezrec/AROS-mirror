#ifndef RPC_SVC_AUTH_H
#define RPC_SVC_AUTH_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

/* @(#)svc_auth.h	2.1 88/07/29 4.0 RPCSRC */
/*      @(#)svc_auth.h 1.6 86/07/16 SMI      */

/*
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

/*
 * Server side authenticator
 */
extern enum auth_stat _authenticate(struct svc_req * rqst,
				    struct rpc_msg * msg);

#endif /* !RPC_SVC_AUTH_H */
