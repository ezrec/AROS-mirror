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

#ifndef _DATA_SOCKET_SOCKET_H
#define _DATA_SOCKET_SOCKET_H 

#include <Generic/Types.h>

/*
 * AmiTCP asynchronous event definitions
 */

enum E_SockFDEvents
{
   FD_ACCEPT    = 0x001, /* there is a connection to accept() */
   FD_CONNECT   = 0x002, /* connect() completed */
   FD_OOB       = 0x004, /* socket has out-of-band data */
   FD_READ      = 0x008, /* socket is readable */
   FD_WRITE     = 0x010, /* socket is writeable */
   FD_ERROR     = 0x020, /* asynchronous error on socket */
   FD_CLOSE     = 0x040, /* connection closed (graceful or not) */
};

/*
 * Definition for Release(CopyOf)Socket unique id
 */
enum E_SocketID
{
   UNIQUE_ID   =  -1,
};

/*
 * Types
 */

enum E_SocketType
{
   SOCK_STREAM = 1,     /* stream socket */
   SOCK_DGRAM,          /* datagram socket */
   SOCK_RAW,            /* raw-protocol interface */
   SOCK_RDM,            /* reliably-delivered message */
   SOCK_SEQPACKET,      /* sequenced packet stream */
};

/*
 * Option flags per-socket.
 */

enum E_SocketOptions
{
   SO_DEBUG       = 0x0001,      /* turn on debugging info recording */
   SO_ACCEPTCONN  = 0x0002,      /* socket has had listen() */
   SO_REUSEADDR   = 0x0004,      /* allow local address reuse */
   SO_KEEPALIVE   = 0x0008,      /* keep connections alive */
   SO_DONTROUTE   = 0x0010,      /* just use interface addresses */
   SO_BROADCAST   = 0x0020,      /* permit sending of broadcast msgs */
   SO_USELOOPBACK = 0x0040,      /* bypass hardware when possible */
   SO_LINGER      = 0x0080,      /* linger on close if data present */
   SO_OOBINLINE   = 0x0100,      /* leave received OOB data in line */

   /*
    * Additional options, not kept in so_options.
    */
   SO_SNDBUF      = 0x1001,      /* send buffer size */
   SO_RCVBUF      = 0x1002,      /* receive buffer size */
   SO_SNDLOWAT    = 0x1003,      /* send low-water mark */
   SO_RCVLOWAT    = 0x1004,      /* receive low-water mark */
   SO_SNDTIMEO    = 0x1005,      /* send timeout */
   SO_RCVTIMEO    = 0x1006,      /* receive timeout */
   SO_ERROR       = 0x1007,      /* get error status and clear */
   SO_TYPE        = 0x1008,      /* get socket type */

   /*
    * AmiTCP/IP specific socket options
    */   
   SO_EVENTMASK   = 0x2001,      /* socket event mask,     */
                                 /* defaults to no events (0) */

   /*
    * Level number for (get/set)sockopt() to apply to socket itself.
    */
   SOL_SOCKET     = 0xffff,      /* options for socket level */
};

/*
 * Structure used for manipulating linger option.
 */
struct linger 
{
   int32    l_onoff;       /* option on/off */
   int32    l_linger;      /* linger time */
};


/*
 * Address families.
 */
enum E_SockAddrFamily
{
   AF_UNSPEC      = 0,     /* unspecified */
   AF_UNIX        = 1,     /* local to host (pipes, portals) */
   AF_INET        = 2,     /* internetwork: UDP, TCP, etc. */
   AF_IMPLINK     = 3,     /* arpanet imp addresses */
   AF_PUP         = 4,     /* pup protocols: e.g. BSP */
   AF_CHAOS       = 5,     /* mit CHAOS protocols */
   AF_NS          = 6,     /* XEROX NS protocols */
   AF_ISO         = 7,     /* ISO protocols */
   AF_OSI         = AF_ISO,
   AF_ECMA        = 8,     /* european computer manufacturers */
   AF_DATAKIT     = 9,     /* datakit protocols */
   AF_CCITT       = 10,    /* CCITT protocols, X.25 etc */
   AF_SNA         = 11,    /* IBM SNA */
   AF_DECnet      = 12,    /* DECnet */
   AF_DLI         = 13,    /* DEC Direct data link interface */
   AF_LAT         = 14,    /* LAT */
   AF_HYLINK      = 15,    /* NSC Hyperchannel */
   AF_APPLETALK   = 16,    /* Apple Talk */
   AF_ROUTE       = 17,    /* Internal Routing Protocol */
   AF_LINK        = 18,    /* Link layer interface */
   pseudo_AF_XTP  = 19,    /* eXpress Transfer Protocol (no AF) */

   AF_MAX         = 20,
};

/*
 * Structure used by kernel to store most
 * addresses.
 */
struct sockaddr 
{
   uint8    sa_len;           /* total length */
   uint8    sa_family;        /* address family */
   char     sa_data[14];      /* actually longer; address value */
};

/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct sockproto 
{
   uint16   sp_family;        /* address family */
   uint16   sp_protocol;      /* protocol */
};

/*
 * Protocol families, same as address families for now.
 */

enum E_SockProtoFamily
{
   PF_UNSPEC      = AF_UNSPEC,
   PF_UNIX        = AF_UNIX,
   PF_INET        = AF_INET,
   PF_IMPLINK     = AF_IMPLINK,
   PF_PUP         = AF_PUP,
   PF_CHAOS       = AF_CHAOS,
   PF_NS          = AF_NS,
   PF_ISO         = AF_ISO,
   PF_OSI         = AF_ISO,
   PF_ECMA        = AF_ECMA,
   PF_DATAKIT     = AF_DATAKIT,
   PF_CCITT       = AF_CCITT,
   PF_SNA         = AF_SNA,
   PF_DECnet      = AF_DECnet,
   PF_DLI         = AF_DLI,
   PF_LAT         = AF_LAT,
   PF_HYLINK      = AF_HYLINK,
   PF_APPLETALK   = AF_APPLETALK,
   PF_ROUTE       = AF_ROUTE,
   PF_LINK        = AF_LINK,
   PF_XTP         = pseudo_AF_XTP,  /* really just proto family, no AF */

   PF_MAX         =AF_MAX,
};

/*
 * Maximum queue length specifiable by listen.
 */
enum 
{
   SOMAXCONN      = 5,
};

/*
 * Message header for recvmsg and sendmsg calls.
 * Used value-result for recvmsg, value only for sendmsg.
 */
struct iovec 
{
   char*          iov_base;
   int            iov_len;
};

struct msghdr 
{
   char*          msg_name;         /* optional address */
   uint32         msg_namelen;      /* size of address */
   struct iovec*  msg_iov;          /* scatter/gather array */
   uint32         msg_iovlen;       /* # elements in msg_iov */
   char*          msg_control;      /* ancillary data, see below */
   uint32         msg_controllen;   /* ancillary data buffer len */
   int32          msg_flags;        /* flags on received message */
};

enum E_SockMsgType
{
   MSG_OOB        = 0x1,      /* process out-of-band data */
   MSG_PEEK       = 0x2,      /* peek at incoming message */
   MSG_DONTROUTE  = 0x4,      /* send without using routing tables */
   MSG_EOR        = 0x8,      /* data completes record */
   MSG_TRUNC      = 0x10,     /* data discarded before delivery */
   MSG_CTRUNC     = 0x20,     /* control data lost before delivery */
   MSG_WAITALL    = 0x40,     /* wait for full request or error */
};

/*
 * Header for ancillary data objects in msg_control buffer.
 * Used for additional information with/about a datagram
 * not expressible by flags.  The format is a sequence
 * of message elements headed by cmsghdr structures.
 */
struct cmsghdr 
{
   uint32         cmsg_len;      /* data byte count, including hdr */
   int32          cmsg_level;    /* originating protocol */
   int32          cmsg_type;     /* protocol-specific type */
/* followed by u_char  cmsg_data[]; */
};

/* given pointer to struct adatahdr, return pointer to data */
#define  CMSG_DATA(cmsg)      ((u_char *)((cmsg) + 1))

/* given pointer to struct adatahdr, return pointer to next adatahdr */
#define  CMSG_NXTHDR(mhdr, cmsg) \
   (((caddr_t)(cmsg) + (cmsg)->cmsg_len + sizeof(struct cmsghdr) > \
       (mhdr)->msg_control + (mhdr)->msg_controllen) ? \
       (struct cmsghdr *)NULL : \
       (struct cmsghdr *)((caddr_t)(cmsg) + ALIGN((cmsg)->cmsg_len)))

#define  CMSG_FIRSTHDR(mhdr)  ((struct cmsghdr *)(mhdr)->msg_control)

/* "Socket"-level control message types: */
#define  SCM_RIGHTS  0x01     /* access rights (array of int) */

/*
 * 4.3 compat sockaddr, move to compat file later
 */
struct osockaddr 
{
   uint16         sa_family;        /* address family */
   char           sa_data[14];      /* up to 14 bytes of direct address */
};

/*
 * 4.3-compat message header (move to compat file later).
 */
struct omsghdr 
{
   char*          msg_name;         /* optional address */
   int32          msg_namelen;      /* size of address */
   struct iovec*  msg_iov;          /* scatter/gather array */
   int32          msg_iovlen;       /* # elements in msg_iov */
   char*          msg_accrights;    /* access rights sent/received */
   int32          msg_accrightslen;
};

/*** Defines and macros for select() *****************************************/

typedef struct fd_set 
{
        uint32    fds_bits[2];
} fd_set;

#define FD_SET(n, p)    ((p)->fds_bits[(n)>>5] |=  (1 << ((n) & 31)))
#define FD_CLR(n, p)    ((p)->fds_bits[(n)>>5] &= ~(1 << ((n) & 31)))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)>>5] &   (1 << ((n) & 31)))
#define FD_COPY(f, t)   memcpy(t, f, sizeof(*(f)))
#define FD_ZERO(p)      memset(p, 0, sizeof(*(p)))

#endif /* _DATA_SOCKET_SOCKET_H */
