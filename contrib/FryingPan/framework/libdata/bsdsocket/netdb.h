/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _DATA_SOCKET_NETDB_H
#define _DATA_SOCKET_NETDB_H

/*
 * Structures returned by network data base library.  All addresses are
 * supplied in host order, and returned in network order (suitable for
 * use in system calls).
 */
struct	hostent 
{
	char*    h_name;	      /* official name of host */
	char**   h_aliases;	   /* alias list */
	int32	   h_addrtype;	   /* host address type */
	int32	   h_length;	   /* length of address */
	char**   h_addr_list;	/* list of addresses from name server */
                           /* #define	h_addr	h_addr_list[0]	address, for backward compatiblity */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 */
struct	netent 
{
	char*    n_name;	/* official name of net */
	char**   n_aliases;	/* alias list */
	int32		n_addrtype;	/* net address type */
	uint32	n_net;		/* network # */
};

struct	servent 
{
	char*    s_name;	/* official service name */
	char**   s_aliases;	/* alias list */
	int32	   s_port;		/* port # */
	char*    s_proto;	/* protocol to use */
};

struct	protoent 
{
	char*    p_name;	/* official protocol name */
	char**   p_aliases;	/* alias list */
	int32    p_proto;	/* protocol # */
};

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */
enum ENetDB_Error
{
   HOST_NOT_FOUND	   = 1,           /* Authoritative Answer Host not found */
   TRY_AGAIN	      = 2,           /* Non-Authoritive Host not found, or SERVERFAIL */
   NO_RECOVERY	      = 3,           /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
   NO_DATA		      = 4,           /* Valid name, no data record of requested type */
   NO_ADDRESS	      = NO_DATA,		/* no address, look for MX record */
};

#endif /* !NETDB_H */
