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

#ifndef _DATA_SOCKET_NET_IN_H
#define _DATA_SOCKET_NET_IN_H 

enum ENet_Protocol
{
   IPPROTO_IP     = 0,     /* dummy for IP */
   IPPROTO_ICMP   = 1,     /* control message protocol */
   IPPROTO_GGP    = 3,     /* gateway^2 (deprecated) */
   IPPROTO_TCP    = 6,     /* tcp */
   IPPROTO_EGP    = 8,     /* exterior gateway protocol */
   IPPROTO_PUP    = 12,    /* pup */
   IPPROTO_UDP    = 17,    /* user datagram protocol */
   IPPROTO_IDP    = 22,    /* xns idp */
   IPPROTO_TP     = 29,    /* tp-4 w/ class negotiation */
   IPPROTO_EON    = 80,    /* ISO cnlp */

   IPPROTO_RAW    = 255,   /* raw IP packet */
   IPPROTO_MAX    = 256,
};


/*
 * Local port number conventions:
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 * Ports > IPPORT_USERRESERVED are reserved
 * for servers, not necessarily privileged.
 */
enum ENet_Ports
{
   IPPORT_RESERVED      = 1024,
   IPPORT_USERRESERVED  = 5000,
};

/*
 * Internet address (a structure for historical reasons)
 */
struct in_addr 
{
   uint32   s_addr;
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define  IN_CLASSA(i)         (((long)(i) & 0x80000000) == 0)
#define  IN_CLASSA_NET        0xff000000
#define  IN_CLASSA_NSHIFT     24
#define  IN_CLASSA_HOST       0x00ffffff
#define  IN_CLASSA_MAX        128

#define  IN_CLASSB(i)         (((long)(i) & 0xc0000000) == 0x80000000)
#define  IN_CLASSB_NET        0xffff0000
#define  IN_CLASSB_NSHIFT     16
#define  IN_CLASSB_HOST       0x0000ffff
#define  IN_CLASSB_MAX        65536

#define  IN_CLASSC(i)         (((long)(i) & 0xe0000000) == 0xc0000000)
#define  IN_CLASSC_NET        0xffffff00
#define  IN_CLASSC_NSHIFT     8
#define  IN_CLASSC_HOST       0x000000ff

#define  IN_CLASSD(i)         (((long)(i) & 0xf0000000) == 0xe0000000)
#define  IN_MULTICAST(i)      IN_CLASSD(i)

#define  IN_EXPERIMENTAL(i)   (((long)(i) & 0xe0000000) == 0xe0000000)
#define  IN_BADCLASS(i)       (((long)(i) & 0xf0000000) == 0xf0000000)

#define  INADDR_ANY           0x00000000
#define  INADDR_BROADCAST     0xffffffff           /* must be masked */
#define  INADDR_NONE          0xffffffff           /* -1 return */

#define  IN_LOOPBACKNET       127                  /* official! */

/*
 * Socket address, internet style.
 */
struct sockaddr_in 
{
   uint8          sin_len;
   uint8          sin_family;
   uint16         sin_port;
   struct in_addr sin_addr;
   char           sin_zero[8];
};



/*
 * Structure used to describe IP options.
 * Used to store options internally, to pass them to a process,
 * or to restore options retrieved earlier.
 * The ip_dst is used for the first-hop gateway when using a source route
 * (this gets put into the header proper).
 */
struct ip_opts 
{
   struct in_addr ip_dst;        /* first hop, 0 w/o src rt */
   char           ip_opt[40];    /* actually variable in size */
};

/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
enum ENet_IPOptions
{
   IP_OPTIONS     = 1,  /* buf/ip_opts; set/get IP per-packet options */
   IP_HDRINCL     = 2,  /* int; header is included with data (raw) */
   IP_TOS         = 3,  /* int; IP type of service and precedence */
   IP_TTL         = 4,  /* int; IP time to live */
   IP_RECVOPTS    = 5,  /* bool; receive all IP options w/datagram */
   IP_RECVRETOPTS = 6,  /* bool; receive IP options for response */
   IP_RECVDSTADDR = 7,  /* bool; receive IP dst addr w/datagram */
   IP_RETOPTS     = 8,  /* ip_opts; set/get IP per-packet options */
};

#endif /* !IN_H */
