/*
    Copyright © 2002-2004, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/
#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H

/* Definitions related to sockets: types, address families, options */

#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif

typedef unsigned char   sa_family_t;
typedef int socklen_t;

/* AmiTCP asynchronous event definitions */

#define FD_ACCEPT	        0x001	        /* there is a connection to accept() */
#define FD_CONNECT	        0x002	        /* connect() completed */
#define FD_OOB		        0x004	        /* socket has out-of-band data */
#define FD_READ		        0x008	        /* socket is readable */
#define FD_WRITE	        0x010	        /* socket is writeable */
#define FD_ERROR	        0x020	        /* asynchronous error on socket */
#define FD_CLOSE	        0x040	        /* connection closed (graceful or not) */

/* Definition for Release(CopyOf)Socket unique id */

#define UNIQUE_ID	        (-1)

/* Types */

#define	SOCK_STREAM	        1		/* stream socket */
#define	SOCK_DGRAM	        2		/* datagram socket */
#define	SOCK_RAW	        3		/* raw-protocol interface */
# define	SOCK_RDM	4		/* reliably-delivered message */
#define	SOCK_SEQPACKET	        5		/* sequenced packet stream */

/* Option flags per-socket. */

#define	SO_DEBUG	        0x0001		/* turn on debugging info recording */
#define	SO_ACCEPTCONN	        0x0002		/* socket has had listen() */
#define	SO_REUSEADDR	        0x0004		/* allow local address reuse */
#define	SO_KEEPALIVE	        0x0008		/* keep connections alive */
#define	SO_DONTROUTE	        0x0010		/* just use interface addresses */
#define	SO_BROADCAST	        0x0020		/* permit sending of broadcast msgs */
# define	SO_USELOOPBACK	0x0040		/* bypass hardware when possible */
#define	SO_LINGER	        0x0080		/* linger on close if data present */
#define	SO_OOBINLINE	        0x0100		/* leave received OOB data in line */
# define        SO_REUSEPORT    0x0200          /* allow local address & port reuse */
# define        SO_TIMESTAMP    0x0400          /* timestamp received dgram traffic */
# define        SO_NOSIGPIPE    0x0800          /* no SIGPIPE from EPIPE */
# define        SO_ACCEPTFILTER 0x1000          /* there is an accept filter */
# define        SO_BINTIME      0x2000          /* timestamp received dgram traffic */

/* Additional options, not kept in so_options. */

#define SO_SNDBUF	        0x1001		/* send buffer size */
#define SO_RCVBUF	        0x1002		/* receive buffer size */
#define SO_SNDLOWAT	        0x1003		/* send low-water mark */
#define SO_RCVLOWAT	        0x1004		/* receive low-water mark */
#define SO_SNDTIMEO	        0x1005		/* send timeout */
#define SO_RCVTIMEO	        0x1006		/* receive timeout */
#define	SO_ERROR	        0x1007		/* get error status and clear */
#define	SO_TYPE		        0x1008		/* get socket type */
# define        SO_LABEL        0x1009          /* socket's MAC label */
# define        SO_PEERLABEL    0x1010          /* socket's peer's MAC label */

/* AmiTCP/IP specific socket options */

#define SO_EVENTMASK	        0x2001		/* socket event mask,     */
                                                /* defaults to no events (0) */

/* Structure used for manipulating linger option. */

struct	linger {
	int	l_onoff;		        /* option on/off */
	int	l_linger;		        /* linger time */
};

struct accept_filter_arg {
        char    af_name[16];
        char    af_arg[256-16];
};


/* Level number for (get/set)sockopt() to apply to socket itself. */

#define	SOL_SOCKET	        0xffff		/* options for socket level */

/* Address families. */

#define AF_UNSPEC	        0		/* unspecified */
#define AF_UNIX		        1		/* local to host (pipes, portals) */
# define        AF_LOCAL        AF_UNIX         /* local to host (pipes, portals) */
#define AF_INET		        2		/* internetwork: UDP, TCP, etc. */
# define	AF_IMPLINK	3		/* arpanet imp addresses */
# define	AF_PUP		4		/* pup protocols: e.g. BSP */
# define	AF_CHAOS	5		/* mit CHAOS protocols */
# define	AF_NETBIOS	6		/* XEROX NS protocols */
# define	AF_ISO		7		/* ISO protocols */
# define	AF_OSI		AF_ISO
# define	AF_ECMA		8		/* european computer manufacturers */
# define	AF_DATAKIT	9		/* datakit protocols */
# define	AF_CCITT	10		/* CCITT protocols, X.25 etc */
# define	AF_SNA		11		/* IBM SNA */
# define        AF_DECnet	12		/* DECnet */
# define        AF_DLI		13		/* DEC Direct data link interface */
# define        AF_LAT		14		/* LAT */
# define	AF_HYLINK	15		/* NSC Hyperchannel */
# define	AF_APPLETALK	16		/* Apple Talk */
# define	AF_ROUTE	17		/* Internal Routing Protocol */
# define	AF_LINK		18		/* Link layer interface */
# define	pseudo_AF_XTP	19		/* eXpress Transfer Protocol (no AF) */
# define        AF_COIP         20              /* connection-oriented IP, aka ST II */
# define        AF_CNT          21              /* Computer Network Technology */
# define        pseudo_AF_RTIP  22              /* Help Identify RTIP packets */
# define        AF_IPX          23              /* Novell Internet Protocol */
# define        AF_SIP          24              /* Simple Internet Protocol */
# define        pseudo_AF_PIP   25              /* Help Identify PIP packets */
# define        AF_ISDN         26              /* Integrated Services Digital Network*/
# define        AF_E164         AF_ISDN         /* CCITT E.164 recommendation */
# define        pseudo_AF_KEY   27              /* Internal key-management function */
#define AF_INET6                28              /* IPv6 */
# define        AF_NATM         29              /* native ATM access */
# define        AF_ATM          30              /* ATM */
# define        pseudo_AF_HDRCMPLT 31           /* Used by BPF to not rewrite headers
                                                   in interface output routine */
# define        AF_NETGRAPH     32              /* Netgraph sockets */
# define        AF_SLOW         33              /* 802.3ad slow protocol */
# define        AF_SCLUSTER     34              /* Sitara cluster protocol */
# define        AF_ARP          35
# define        AF_BLUETOOTH    36              /* Bluetooth sockets */
# define        AF_MAX          37

/*
  Structure used by kernel to store most
  addresses.
*/

struct sockaddr {
	u_char	sa_len;			        /* total length */
	u_char	sa_family;		        /* address family */
	char	sa_data[14];		        /* actually longer; address value */
};

# define SOCK_MAXADDRLEN 255                    /* longest possible addresses */

/*
  Structure used by kernel to pass protocol
  information in raw sockets.
*/

struct sockproto {
	u_short	sp_family;		        /* address family */
	u_short	sp_protocol;		        /* protocol */
};

/* RFC 2553: protocol-independent placeholder for socket addresses */

#define _SS_MAXSIZE     128U
#define _SS_ALIGNSIZE   (sizeof(__int64_t))
#define _SS_PAD1SIZE    (_SS_ALIGNSIZE - sizeof(unsigned char) - \
                            sizeof(sa_family_t))
#define _SS_PAD2SIZE    (_SS_MAXSIZE - sizeof(unsigned char) - \
                            sizeof(sa_family_t) - _SS_PAD1SIZE - _SS_ALIGNSIZE)

struct sockaddr_storage {
        unsigned char   ss_len;                 /* address length */
        sa_family_t     ss_family;              /* address family */
        char            __ss_pad1[_SS_PAD1SIZE];
        __int64_t       __ss_align;             /* force desired struct alignment */
        char            __ss_pad2[_SS_PAD2SIZE];
};

/* Protocol families, same as address families for now. */

#define PF_UNSPEC               AF_UNSPEC
#define PF_LOCAL                AF_LOCAL
#define PF_UNIX                 PF_LOCAL        /* backward compatibility */
#define PF_INET                 AF_INET
#define PF_IMPLINK              AF_IMPLINK
#define PF_PUP                  AF_PUP
#define PF_CHAOS                AF_CHAOS
#define PF_NETBIOS              AF_NETBIOS
#define PF_ISO                  AF_ISO
#define PF_OSI                  AF_ISO
#define PF_ECMA                 AF_ECMA
#define PF_DATAKIT              AF_DATAKIT
#define PF_CCITT                AF_CCITT
#define PF_SNA                  AF_SNA
#define PF_DECnet               AF_DECnet
#define PF_DLI                  AF_DLI
#define PF_LAT                  AF_LAT
#define PF_HYLINK               AF_HYLINK
#define PF_APPLETALK            AF_APPLETALK
#define PF_ROUTE                AF_ROUTE
#define PF_LINK                 AF_LINK
#define PF_XTP                  pseudo_AF_XTP   /* really just proto family, no AF */
#define PF_COIP                 AF_COIP
#define PF_CNT                  AF_CNT
#define PF_SIP                  AF_SIP
#define PF_IPX                  AF_IPX
#define PF_RTIP                 pseudo_AF_RTIP  /* same format as AF_INET */
#define PF_PIP                  pseudo_AF_PIP
#define PF_ISDN                 AF_ISDN
#define PF_KEY                  pseudo_AF_KEY
#define PF_INET6                AF_INET6
#define PF_NATM                 AF_NATM
#define PF_ATM                  AF_ATM
#define PF_NETGRAPH             AF_NETGRAPH
#define PF_SLOW                 AF_SLOW
#define PF_SCLUSTER             AF_SCLUSTER
#define PF_ARP                  AF_ARP
#define PF_BLUETOOTH            AF_BLUETOOTH

#define PF_MAX                  AF_MAX

/* Definitions for network related sysctl, CTL_NET. */

#define NET_MAXID       AF_MAX

/* PF_ROUTE - Routing table */

#define NET_RT_DUMP             1               /* dump; may limit to a.f. */
#define NET_RT_FLAGS            2               /* by flags, e.g. RESOLVING */
#define NET_RT_IFLIST           3               /* survey interface list */
#define NET_RT_IFMALIST         4               /* return multicast address list */
#define NET_RT_MAXID            5

/* Maximum queue length specifiable by listen. */

#define	SOMAXCONN	        128

/*
  Message header for recvmsg and sendmsg calls.
  Used value-result for recvmsg, value only for sendmsg.
*/

struct iovec {
	caddr_t	iov_base;
	int	iov_len;
};

struct msghdr {
	caddr_t	msg_name;		        /* optional address */
	u_int	msg_namelen;		        /* size of address */
	struct	iovec *msg_iov;		        /* scatter/gather array */
	u_int	msg_iovlen;		        /* # elements in msg_iov */
	caddr_t	msg_control;		        /* ancillary data, see below */
	u_int	msg_controllen;		        /* ancillary data buffer len */
	int	msg_flags;		        /* flags on received message */
};

#define	MSG_OOB		        0x1		/* process out-of-band data */
#define	MSG_PEEK	        0x2		/* peek at incoming message */
#define	MSG_DONTROUTE	        0x4		/* send without using routing tables */
#define	MSG_EOR		        0x8		/* data completes record */
#define	MSG_TRUNC	        0x10		/* data discarded before delivery */
#define	MSG_CTRUNC	        0x20		/* control data lost before delivery */
#define	MSG_WAITALL	        0x40		/* wait for full request or error */
# define        MSG_DONTWAIT    0x80            /* this message should be nonblocking */
# define        MSG_EOF         0x100           /* data completes connection */
# define        MSG_NBIO        0x4000          /* FIONBIO mode, used by fifofs */
# define        MSG_COMPAT      0x8000          /* used in sendit() */

/*
  Header for ancillary data objects in msg_control buffer.
  Used for additional information with/about a datagram
  not expressible by flags.  The format is a sequence
  of message elements headed by cmsghdr structures.
*/

struct cmsghdr {
	socklen_t 	cmsg_len;		/* data byte count, including hdr */
	int	        cmsg_level;		/* originating protocol */
	int	        cmsg_type;		/* protocol-specific type */
/* followed by	u_char  cmsg_data[]; */
};

/*
  While we may have more groups than this, the cmsgcred struct must
  be able to fit in an mbuf, and NGROUPS_MAX is too large to allow
  this.
*/

#define CMGROUP_MAX 16

/* given pointer to struct adatahdr, return pointer to data */

#define CMSG_DATA(cmsg)         ((unsigned char *)(cmsg) + \
                                 _ALIGN(sizeof(struct cmsghdr)))


/* given pointer to struct adatahdr, return pointer to next adatahdr */


#define CMSG_NXTHDR(mhdr, cmsg) \
        (((char *)(cmsg) + _ALIGN((cmsg)->cmsg_len) + \
          _ALIGN(sizeof(struct cmsghdr)) > \
            (char *)(mhdr)->msg_control + (mhdr)->msg_controllen) ? \
            (struct cmsghdr *)0 : \
            (struct cmsghdr *)((char *)(cmsg) + _ALIGN((cmsg)->cmsg_len)))

/*
  RFC 2292 requires to check msg_controllen, in case that the kernel returns
  an empty list for some reasons.
*/

#define CMSG_FIRSTHDR(mhdr) \
        ((mhdr)->msg_controllen >= sizeof(struct cmsghdr) ? \
         (struct cmsghdr *)(mhdr)->msg_control : \
         (struct cmsghdr *)NULL)

/* RFC 2292 additions */

#define CMSG_SPACE(l)           (_ALIGN(sizeof(struct cmsghdr)) + _ALIGN(l))
#define CMSG_LEN(l)             (_ALIGN(sizeof(struct cmsghdr)) + (l))

/* "Socket"-level control message types: */

#define SCM_RIGHTS              0x01            /* access rights (array of int) */
#define SCM_TIMESTAMP           0x02            /* timestamp (struct timeval) */
#define SCM_CREDS               0x03            /* process creds (struct cmsgcred) */
#define SCM_BINTIME             0x04            /* timestamp (struct bintime) */

/* 4.3 compat sockaddr, move to compat file later */

struct osockaddr {
	u_short	sa_family;		        /* address family */
	char	sa_data[14];		        /* up to 14 bytes of direct address */
};

/* 4.3-compat message header (move to compat file later). */

struct omsghdr {
	caddr_t	msg_name;		        /* optional address */
	int	msg_namelen;		        /* size of address */
	struct	iovec *msg_iov;		        /* scatter/gather array */
	int	msg_iovlen;		        /* # elements in msg_iov */
	caddr_t	msg_accrights;		        /* access rights sent/received */
	int	msg_accrightslen;
};

/* howto arguments for shutdown(2), specified by Posix.1g. */

#define SHUT_RD                 0               /* shut down the reading side */
#define SHUT_WR                 1               /* shut down the writing side */
#define SHUT_RDWR               2               /* shut down both sides */

/* sendfile(2) header/trailer struct */

struct sf_hdtr {
        struct iovec *headers;                  /* pointer to an array of header struct iovec's */
        int hdr_cnt;                            /* number of header iovec's */
        struct iovec *trailers;                 /* pointer to an array of trailer struct iovec's */
        int trl_cnt;                            /* number of trailer iovec's */
};

/* Sendfile-specific flag(s) */

#define        SF_NODISKIO     0x00000001

#ifndef KERNEL

/*
 * Include socket protos/inlines/pragmas
 */
#if !defined(BSDSOCKET_H) && !defined(CLIB_SOCKET_PROTOS_H)
/* #include <bsdsocket.h> AROS NO */
#endif

#endif /* !KERNEL */

#endif /* !SYS_SOCKET_H */
