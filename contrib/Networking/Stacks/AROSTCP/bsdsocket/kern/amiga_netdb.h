/*
 * Copyright (C) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
 * Copyright (C) 2005 Pavel Fedin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */

#ifndef AMIGA_NETDB_H
#define AMIGA_NETDB_H

#ifndef NETDB_H
#include <netdb.h>
#endif

#if !defined(__AROS__)
#include <libraries/eztcp_private.h>
#else
#include <kern/amiga_netdb_resolver.h>
#endif
#include "net/netdbpaths.h"

#ifndef IN_H
#include <netinet/in.h>
#endif

/* Access control flags */
#define ACF_ALLOW	0x01
#define ACF_LOG		0x02
#define ACF_PRIVONLY	0x04 /* 0 as port matches for privileged ports only */
#define ACF_CONTINUE	(1 << 15)

/* AC table temporary buffer size */
#define TMPACTSIZE	0x4000 

extern struct NetDataBase *NDB;
extern struct SignalSemaphore ndb_Lock;
extern struct DynDataBase DynDB;
extern ULONG ndb_Serial;

/* Locking macros for NDB */
#define LOCK_W_NDB(ndb) (ObtainSemaphore(&ndb_Lock))
#define LOCK_R_NDB(ndb) (ObtainSemaphoreShared(&ndb_Lock))
#define UNLOCK_NDB(ndb) (ReleaseSemaphore(&ndb_Lock))
#define ATTEMPT_NDB(ndb) (AttemptSemaphore(&ndb_Lock))

/*
 * netdatabase calls for some AmiTCP/IP functions
 */
struct ServentNode * findServentNode(struct NetDataBase * ndb,
				     const char * name, const char * proto);

/*
 * Read NetDB...
 */
LONG do_netdb(struct CSource *cs, UBYTE **errstrp, struct CSource *res);
LONG reset_netdb(struct CSource *cs, UBYTE **errstrp, struct CSource *res);
LONG init_netdb(void);
void netdb_deinit(void);
     
#endif /* AMIGA_NETDB_H */
