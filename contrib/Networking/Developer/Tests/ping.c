/*
 * ping.c -- network connectivity testing tool
 *
 * Last modified: Tue Mar  8 00:46:06 1994 ppessi
 */

static const char version[] = "$VER: "
"Copyright © 2000 Adam Chodorowski"
"Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>\n"
"Helsinki University of Technology, Finland.\n"
"Copyright © 1983 The Regents of the University of California.\n"
"All rights reserved.\n";

#ifndef __AROS__
typedef unsigned long IPTR;
#else
#include <netinet/in.h>
#endif

/****** netutil.doc/ping ****************************************************

    NAME
        ping - send ICMP ECHO_REQUEST packets to network hosts

    SYNOPSIS
        ping [-dfnqrvR] [-c count] [-i wait] [-l preload] [-p pattern]
             [-s packetsize] [-L [ hosts ]] host

        ping [HOST] <host> [COUNT <n>] [WAIT <n>] [PACKETSIZE <n>] [PATTERN <p>]
             [FLOOD] [PRELOAD] [DEBUG] [VERBOSE] [QUIET] [RECORDROUTE] [NOROUTE]

    TEMPLATE

        "HOST/A,COUNT/K/N,WAIT/K/N,PACKETSIZE/K/N,PATTERN/K,FLOOD/S,PRELOAD/S," \
        "DEBUG/S,VERBOSE/S,QUIET/S,RECORDROUTE/S,NOROUTE/S"

    DESCRIPTION
        Ping uses the ICMP protocol's mandatory ECHO_REQUEST datagram to
        elicit an ICMP ECHO_RESPONSE from a host or gateway.  ECHO_REQUEST
        datagrams (``pings'') have an IP and ICMP header, followed by a
        ``struct timeval'' and then an arbitrary number of ``pad'' bytes
        used to fill out the packet.  The options are as follows: Other
        options are:

        -c count
                Stop after sending (and receiving) count ECHO_RESPONSE
                packets.

        -d      Set the SO_DEBUG option on the socket being used.

        -f      Flood ping.  Outputs packets as fast as they come back or
                one hundred times per second, whichever is more.  For every
                ECHO_REQUEST sent a period ``.'' is printed, while for ever
                ECHO_REPLY received a backspace is printed.  This provides a
                rapid display of how many packets are being dropped.  Only
                the super-user may use this option.  This can be very hard
                on a network and should be used with caution.

        -i wait
                Wait wait seconds between sending each packet. The default
                is to wait for one second between each packet.  This option
                is incompatible with the -f option.

        -L [hosts]
	        Use loose routing IP option.  Includes IPOPT_LSRR option in
                the ECHO_REQUEST packet with all specified hosts in the
                route.  Many hosts wont support loose routing, such a host
                can either ignore or return the loose routed ICMP packet in
                the middle of the route.

        -l preload
                If preload is specified, ping sends that many packets as
                fast as possible before falling into its normal mode of
                behavior.

        -n      Numeric output only.  No attempt will be made to lookup
                symbolic names for host addresses.

        -p pattern
                You may specify up to 16 ``pad'' bytes to fill out the
                packet you send.  This is useful for diagnosing
                data-dependent problems in a network.  For example, ``-p
                ff'' will cause the sent packet to be filled with all ones.

        -q      Quiet output.  Nothing is displayed except the summary lines
                at startup time and when finished.

        -R      Record route.  Includes the RECORD_ROUTE option in the
                ECHO_REQUEST packet and displays the route buffer on
                returned packets.  Note that the IP header is only large
                enough for nine such routes.  Many hosts ignore or discard
                this option.

        -r      Bypass the normal routing tables and send directly to a host
                on an attached network.  If the host is not on a
                directly-attached network, an error is returned.  This
                option can be used to ping a local host through an interface
                that has no route through it.

        -s packetsize
                Specifies the number of data bytes to be sent.  The default
                is 56, which translates into 64 ICMP data bytes when
                combined with the 8 bytes of ICMP header data.

        -v      Verbose output.  ICMP packets other than ECHO_RESPONSE that
                are received are listed.

        When using ping for fault isolation, it should first be run on the
        local host, to verify that the local network interface is up and
        running.  Then, hosts and gateways further and further away should
        be ``pinged''.  Round-trip times and packet loss statistics are
        computed.  If duplicate packets are received, they are not included
        in the packet loss calculation, although the round trip time of
        these packets is used in calculating the minimum/average/maximum
        round-trip time numbers.  When the specified number of packets have
        been sent (and received) or if the program is terminated with a
        SIGINT, a brief summary is displayed.

        This program is intended for use in network testing, measurement and
        management.  Because of the load it can impose on the network, it is
        unwise to use ping during normal operations or from automated
        scripts.

    ICMP PACKET DETAILS
        An IP header without options is 20 bytes.  An ICMP ECHO_REQUEST
        packet contains an additional 8 bytes worth of ICMP header followed
        by an arbitrary amount of data.  When a packetsize is given, this
        indicated the size of this extra piece of data (the default is 56).
        Thus the amount of data received inside of an IP packet of type ICMP
        ECHO_REPLY will always be 8 bytes more than the requested data space
        (the ICMP header).

        If the data space is at least eight bytes large, ping uses the first
        eight bytes of this space to include a timestamp which it uses in
        the computation of round trip times.  If less than eight bytes of
        pad are specified, no round trip times are given.

    DUPLICATE AND DAMAGED PACKETS
        Ping will report duplicate and damaged packets.  Duplicate packets
        should never occur, and seem to be caused by inappropriate
        link-level retransmissions.  Duplicates may occur in many situations
        and are rarely (if ever) a good sign, although the presence of low
        levels of duplicates may not always be cause for alarm.

        Damaged packets are obviously serious cause for alarm and often
        indicate broken hardware somewhere in the ping packet's path (in the
        network or in the hosts).

    TRYING DIFFERENT DATA PATTERNS
        The (inter)network layer should never treat packets differently
        depending on the data contained in the data portion.  Unfortunately,
        data-dependent problems have been known to sneak into networks and
        remain undetected for long periods of time.  In many cases the
        particular pattern that will have problems is something that doesn't
        have sufficient ``transitions'', such as all ones or all zeros, or a
        pattern right at the edge, such as almost all zeros.  It isn't
        necessarily enough to specify a data pattern of all zeros (for
        example) on the command line because the pattern that is of interest
        is at the data link level, and the relationship between what you
        type and what the controllers transmit can be complicated.

        This means that if you have a data-dependent problem you will
        probably have to do a lot of testing to find it.  If you are lucky,
        you may manage to find a file that either can't be sent across your
        network or that takes much longer to transfer than other similar
        length files.  You can then examine this file for repeated patterns
        that you can test using the -p option of ping.

    TTL DETAILS
        The TTL value of an IP packet represents the maximum number of IP
        routers that the packet can go through before being thrown away.  In
        current practice you can expect each router in the Internet to
        decrement the TTL field by exactly one.

        The TCP/IP specification states that the TTL field for TCP packets
        should be set to 60, but many systems use smaller values (4.3 BSD
        uses 30, 4.2 used 15). The AmiTCP/IP uses normally TTL value 30.

        The maximum possible value of this field is 255, and most systems
        set the TTL field of ICMP ECHO_REQUEST packets to 255.  This is why
        you will find you can ``ping'' some hosts, but not reach them with
        telnet or ftp.

        In normal operation ping prints the ttl value from the packet it re-
        ceives.  When a remote system receives a ping packet, it can do one
        of three things with the TTL field in its response:

        ·   Not change it; this is what Berkeley Unix systems did before the
            4.3BSD-Tahoe release.  In this case the TTL value in the
            received packet will be 255 minus the number of routers in the
            round-trip path.

        ·   Set it to 255; this is what AmiTCP/IP and current Berkeley Unix
            systems do.  In this case the TTL value in the received packet
            will be 255 minus the number of routers in the path from the
            remote system to the pinging host.

        ·   Set it to some other value.  Some machines use the same value
            for ICMP packets that they use for TCP packets, for example
            either 30 or 60.  Others may use completely wild values.

    LOOSE SOURCE ROUTING DETAILS
        When a packet is routed with loose routing in IP, the destination
        address of datagram is originally set to the first address in the
        routing list.  When the datagram reaches its destination, the
        destination address is changed to the next address in the list and
        the datagram is routed to that destination. After the whole routing
        list is exhausted, the datagram is handled to upper-level protocols.

        The loose routing options can be ignored by hosts between the
        gateways in the loose routing list.  However, if the host in the
        list don't understand loose routing, it may think that the datagram
        is destined to it and respond to it.  Also, many hosts simply drop
        the packets with IP options.

    BUGS
        Many Hosts and Gateways ignore the RECORD_ROUTE and
        LOOSE_SOURCE_ROUTING options.

        The maximum IP header length is too small for options like
        RECORD_ROUTE to be completely useful.  There's not much that that
        can be done about this, however.

        Flood pinging is not recommended in general, and flood pinging the
        broadcast address should only be done under very controlled
        conditions.

    SEE ALSO
        netstat,  ifconfig

    AUTHOR
        Mike Muuss, U. S. Army Ballistic Research Laboratory, December, 1983

        The ping command appeared in 4.3BSD.

        The loose routing and working record route options were added by
        Pekka Pessi, AmiTCP/IP Group, Helsinki Univ. of Technology.

*****************************************************************************
* */

/*
 *			P I N G . C
 *
 * Using the InterNet Control Message Protocol (ICMP) "ECHO" facility,
 * measure round-trip-delays and packet loss across network paths.
 *
 * Author -
 *	Mike Muuss
 *	U. S. Army Ballistic Research Laboratory
 *	December, 1983
 *
 * Status -
 *	Public Domain.  Distribution Unlimited.
 * Bugs -
 *	More statistics could always be gathered.
 */

#include <exec/memory.h>
#include <exec/types.h>
#include <exec/ports.h>
#include <exec/io.h>

#include <dos/dos.h>

#include <devices/timer.h>

#include <proto/socket.h>
#include <proto/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>

struct Library *SocketBase;

#define ioctl IoctlSocket

#include <sys/param.h>
#include <sys/socket.h>

#include <fcntl.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_var.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#define MYDEBUG 1
#include "debug.h"

#ifdef __AROS__
#undef inet_ntoa
#endif

char *inet_ntoa( struct in_addr ip )
{
  return Inet_NtoA(ip.s_addr);
}


#define DEFDATALEN  (64 - 8)         /* default data length */
#define MAXIPLEN    60
#define MAXICMPLEN  76
#define MAXPACKET   (65536 - 60 - 8) /* max packet size */
#define MAXWAIT     10               /* max seconds to wait for response */
#define NROUTES     9                /* number of record route slots */

#define A(bit)          rcvd_tbl[(bit)>>3]      /* identify byte in array */
#define B(bit)          (1 << ((bit) & 0x07))   /* identify bit in byte */
#define SET(bit)        (A(bit) |= B(bit))
#define CLR(bit)        (A(bit) &= (~B(bit)))
#define TST(bit)        (A(bit) & B(bit))

/* various options */
int options;
#define F_FLOOD         0x001
#define F_INTERVAL      0x002
#define F_NUMERIC       0x004
#define F_PINGFILLED    0x008
#define F_QUIET         0x010
#define F_RROUTE        0x020
#define F_SO_DEBUG      0x040
#define F_SO_DONTROUTE  0x080
#define F_VERBOSE       0x100
#define F_LOOSEROUTE    0x200

#ifndef LONG_MAX
#define LONG_MAX        0xffffffff
#endif

/*
 * MAX_DUP_CHK is the number of bits in received table, i.e. the maximum
 * number of received sequence numbers we can keep track of.  Change 128
 * to 8192 for complete accuracy...
 */
#define	MAX_DUP_CHK	(8 * 128)
int mx_dup_ck = MAX_DUP_CHK;
char rcvd_tbl[MAX_DUP_CHK / 8];

struct sockaddr whereto;        /* who to ping */
int datalen = DEFDATALEN;
int s = -1;                     /* socket file descriptor */
u_char *outpack;
char BSPACE = '\b';             /* characters written for flood */
char DOT = '.';
char *hostname;
long ident;                     /* process id to identify our packets */

/* counters */
long npackets;          /* max packets to transmit */
long nreceived;         /* # of packets we got back */
long nrepeats;          /* number of duplicates */
long ntransmitted;      /* sequence # for outbound packets = #sent */
int  interval = 1;       /* interval between packets */

/* timing */
int  timing;             /* flag to do timing */
long tmin = LONG_MAX;   /* minimum round trip time */
long tmax;              /* maximum round trip time */
u_long tsum;            /* sum of all times, for doing average */

/*** Prototypes **************************************************************/

char *pr_addr(u_long l);
void catcher(void), pinger(void), finish(void);
void pr_pack(char *buf,	int cc,	struct sockaddr_in *from);
void pr_icmph(struct icmp *icp);
void pr_iph(struct ip *ip);
void pr_retip(struct ip *ip);
void fill(char *bp, char *patp);
int in_cksum(u_short *addr, int len);
void clean_time( void );

void Init( void );
void Quit( STRPTR message );

struct MsgPort *timerport = NULL;
struct timerequest *timermsg = NULL;
BOOL notopen = TRUE;
#define TimerBase (timermsg->tr_node.io_Device)

struct RDArgs *rda = NULL;

void clean_timer() {
    if (timermsg) {
        if (!notopen) {
            if (!CheckIO((struct IORequest*)timermsg)) {
                AbortIO((struct IORequest*)timermsg);
                WaitIO((struct IORequest*)timermsg);
            }
            CloseDevice((struct IORequest*)timermsg);
            notopen = TRUE;
        }
        DeleteIORequest( (struct IORequest *) timermsg );
        timermsg = NULL;
    }
    if (timerport) {
        DeleteMsgPort(timerport);
        timerport = NULL;
    }
}

void Init() {
    if( !(SocketBase = OpenLibrary( "bsdsocket.library", 0L )) ) {
        Quit( "Could not open bsdsocket.library!" );
    }
}

void Quit( STRPTR message ) {
    WORD rc;

    if( message ) {
        printf( "Ping: %s\n", message );
        rc = RETURN_WARN;
    } else {
        rc = RETURN_OK;
    }

    if( rda ) FreeArgs( rda );

    clean_timer();

    if( SocketBase ) CloseLibrary( SocketBase );

    exit( rc );
}


#define ARG_TEMPLATE "HOST/A,COUNT/K/N,WAIT/K/N,PACKETSIZE/K/N,PATTERN/K,FLOOD/S,PRELOAD/K/N," \
                     "DEBUG/S,VERBOSE/S,QUIET/S,RECORDROUTE/S,NOROUTE/S,NUMERIC/S"

enum {
    ARG_HOST,
    ARG_COUNT,
    ARG_WAIT,
    ARG_PACKETSIZE,
    ARG_PATTERN,
    ARG_FLOOD,
    ARG_PRELOAD,
    ARG_DEBUG,
    ARG_VERBOSE,
    ARG_QUIET,
    ARG_RECORDROUTE,
    ARG_NOROUTE,
    ARG_NUMERIC
};

int main( void ) {
    IPTR           args[] = { NULL,
                              NULL,
                              NULL,
                              NULL,
                              NULL,
                              FALSE,
                              NULL,
                              FALSE,
                              FALSE,
                              FALSE,
                              FALSE,
                              FALSE,
                              FALSE };

    extern int errno;
    struct timeval timeout;
    struct hostent *hp;
    struct sockaddr_in *to;
    struct protoent *proto;
    register int i;
    int fdmask, hold, packlen, preload;
    u_char *datap, *packet;
    char *target, hnamebuf[MAXHOSTNAMELEN];
#ifdef IP_OPTIONS
    u_char rspace[3 + 4 * NROUTES + 1]; /* record route space */
#endif
    ULONG timermask;

    outpack = malloc(MAXPACKET);
    if (outpack == NULL) {
	//perror("ping");
	Quit( "Network error." );
    }
    preload = 0;
    datap = &outpack[8 + sizeof(struct timeval)];

    if( (rda = ReadArgs( ARG_TEMPLATE, args, NULL )) != NULL ) {
	Init();

	if( args[ARG_HOST] != NULL ) {
            target = (STRPTR) args[ARG_HOST];
        }

        if( args[ARG_COUNT] != NULL ) {
            npackets = *(ULONG *)args[ARG_COUNT];
        }
        if( args[ARG_FLOOD] ) {
            options |= F_FLOOD;
            //setbuf( stdout, (char *) NULL );
        }
        if( args[ARG_WAIT] != NULL ) {
            options |= F_INTERVAL;
            interval = *(ULONG *)args[ARG_WAIT];
        }
        if( args[ARG_PRELOAD] != NULL ) {
            preload = *(ULONG *)args[ARG_PRELOAD];
        }

        if( args[ARG_PATTERN] != NULL ) {
            options |= F_PINGFILLED;
            fill( (char *) datap, (STRPTR) args[ARG_PATTERN] );
        }

        if( args[ARG_PACKETSIZE] != NULL ) {
            datalen = *(ULONG *)args[ARG_PACKETSIZE];
        }

        if( args[ARG_DEBUG] )       options |= F_SO_DEBUG;
        if( args[ARG_NUMERIC] )     options |= F_NUMERIC;
        if( args[ARG_QUIET] )       options |= F_QUIET;
        if( args[ARG_RECORDROUTE] ) options |= F_RROUTE;
        if( args[ARG_NOROUTE] )     options |= F_SO_DONTROUTE;
        if( args[ARG_VERBOSE] )     options |= F_VERBOSE;

        /* Not handled: F_LOOSEROUTE */

	/* Sanity checking of the options */

	if( datalen <= 0 )  Quit( "Packet size too large." );
	if( preload < 0 )   Quit( "Bad preload value." );
	if( interval <= 0 ) Quit( "Bad timing interval." );
	if( npackets < 0 ) /* WAS: <= */ Quit( "Bad number of packets to transmit." );

	if( (options & F_LOOSEROUTE) && (options & F_RROUTE) ) {
	    Quit( "LOOSEROUTE and RECORDROUTE options cannot be used concurrently." );
	}

	if( (options & F_FLOOD) && (options & F_INTERVAL) ) {
	    Quit( "FLOOD and WAIT are incompatible options." );
	}

	{
	    u_char *cp = rspace;

	    if (options & F_LOOSEROUTE) {
#ifdef IP_OPTIONS
		rspace[IPOPT_OPTVAL] = IPOPT_LSRR;
		rspace[IPOPT_OLEN] = 3;
		rspace[IPOPT_OFFSET] = IPOPT_MINOFF;
		cp = rspace + IPOPT_OFFSET + 1;
#else
		Quit( "Source routing not available in this implementation." );
#endif				/* IP_OPTIONS */
	    }

	    if( target != NULL ) {
		bzero((char *)&whereto, sizeof(struct sockaddr));
		to = (struct sockaddr_in *)&whereto;
#ifdef _SOCKADDR_LEN
		to->sin_len = sizeof(*to);
#endif
		to->sin_family = AF_INET;
		to->sin_addr.s_addr = inet_addr(target);

		if (to->sin_addr.s_addr != (u_int)-1) {
		    hostname = target;
		} else {
		    hp = gethostbyname(target);
		    if( !hp ) {
			Quit( "Unknown host." );
		    }
		    to->sin_family = hp->h_addrtype;
		    bcopy(hp->h_addr, (caddr_t)&to->sin_addr, hp->h_length);
		    strncpy(hnamebuf, hp->h_name, sizeof(hnamebuf) - 1);
		    hostname = hnamebuf;
		}

#ifdef IP_OPTIONS
		if (options & F_LOOSEROUTE) {
		    if (rspace + sizeof(rspace) - 4 < cp) {
			Quit( "Too many hops for source routing." );
		    }
		}
		*cp++ = (to->sin_addr.s_addr >> 24);
		*cp++ = (to->sin_addr.s_addr >> 16);
		*cp++ = (to->sin_addr.s_addr >> 8);
		*cp++ = (to->sin_addr.s_addr >> 0);
		rspace[IPOPT_OLEN] += 4;
	    }
#endif
	}

	if (datalen >= sizeof(struct timeval)) /* can we time transfer */
	    timing = 1;
	packlen = datalen + MAXIPLEN + MAXICMPLEN;
	if (!(packet = (u_char *)malloc((u_int)packlen))) {
	    Quit( "Out of memory." );
	}
	if (!(options & F_PINGFILLED))
	    for (i = 8; i < datalen; ++i)
		*datap++ = i;

	ident = ((ULONG) FindTask( NULL )) & 0xFFFF;

	/*  if (!(proto = getprotobyname("icmp"))) { */
/*  	    Quit( "Unknown protocol ICMP." ); */
/*  	} */

	timerport = CreateMsgPort();
	if (!timerport) {
	    Quit( "Could not create timer port." );
	}
	timermask = 1<<timerport->mp_SigBit;

	timermsg = (struct timerequest *) CreateIORequest(timerport, sizeof(*timermsg));
	if (!timermsg) {
	    Quit( "Could not create timer message." );
	}

	if( (notopen = OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)timermsg, 0)) ) {
	    Quit( "Could not open timer device.");
	}

	timermsg->tr_node.io_Command = TR_ADDREQUEST;
	timermsg->tr_time.tv_secs = 1L;
	timermsg->tr_time.tv_micro = 0L;
	/* don't confuse CheckIO */
	timermsg->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	SetSocketSignals(timermask | SIGBREAKF_CTRL_C, 0L, 0L);

	if ((s = socket(AF_INET, SOCK_RAW, 1 /*proto->p_proto*/)) < 0) {
	    //perror("ping: socket");
	    Quit( "Could not allocate socket." );
	}

	hold = 1;
	if (options & F_SO_DEBUG)
	    (void)setsockopt(s, SOL_SOCKET, SO_DEBUG, (char *)&hold,
			     sizeof(hold));
	if (options & F_SO_DONTROUTE)
	    (void)setsockopt(s, SOL_SOCKET, SO_DONTROUTE, (char *)&hold,
			     sizeof(hold));

#ifdef IP_OPTIONS
	if (options & F_LOOSEROUTE) {
	    // pad to long word
	    rspace[rspace[IPOPT_OLEN]] = IPOPT_EOL;
	    if (setsockopt(s, IPPROTO_IP, IP_OPTIONS, rspace,
			   rspace[IPOPT_OLEN] + 1) < 0) {
		//perror("ping: source routing");
		Quit( "Source routing." );
	    }
	}
#endif

	// record route option
	if (options & F_RROUTE) {
#ifdef IP_OPTIONS
	    rspace[IPOPT_OPTVAL] = IPOPT_RR;
	    rspace[IPOPT_OLEN] = sizeof(rspace)-1;
	    rspace[IPOPT_OFFSET] = IPOPT_MINOFF;
	    rspace[rspace[IPOPT_OLEN]] = IPOPT_EOL;
	    if (setsockopt(s, IPPROTO_IP, IP_OPTIONS, rspace, sizeof(rspace)) < 0) {
		//perror("ping: record route");
		Quit( "Record route." );
	    }
#else
	    Quit( "Record route not available in this implementation." );
#endif				// IP_OPTIONS
	}

	/*
	 * When pinging the broadcast address, you can get a lot of answers.
	 * Doing something so evil is useful if you are trying to stress the
	 * ethernet, or just want to fill the arp cache to get some stuff for
	 * /etc/ethers.
	 */
	hold = 48 * 1024;
	(void)setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&hold,
			 sizeof(hold));

	if (to->sin_family == AF_INET)
	    (void)printf("PING %s (%s): %d data bytes\n", hostname,
			 inet_ntoa(*(struct in_addr *)&to->sin_addr.s_addr),
			 datalen);
	else
	    (void)printf("PING %s: %d data bytes\n", hostname, datalen);

	while (preload--)		/* fire off them quickies */
	    pinger();

	if ((options & F_FLOOD) == 0)
	    catcher();			/* start things going */

	for (;;) {
	    struct sockaddr_in from;
	    register int cc;
	    int fromlen;

	    ULONG sm = SetSignal(0L, timermask | SIGBREAKF_CTRL_C);
	    if (sm & SIGBREAKF_CTRL_C)
		finish();
	    if (sm & timermask && GetMsg(timerport))
		catcher();

	    if (options & F_FLOOD) {
		pinger();
		timeout.tv_secs = 0;
		timeout.tv_micro = 10000;
		fdmask = 1 << s;
		if (WaitSelect(s + 1, (fd_set *)&fdmask, (fd_set *)NULL,
			       (fd_set *)NULL, &timeout,NULL) < 1) /* sba: was select() */
		    continue;
	    }
	    fromlen = sizeof(from);
	    if ((cc = recvfrom(s, (char *)packet, packlen, 0,
			       (struct sockaddr *)&from, &fromlen)) < 0) {
		if (errno == EINTR)
		    continue;
		//perror("ping: recvfrom");
		fprintf(stderr, "ping: recvfrom" );
		continue;
	    }
	    pr_pack((char *)packet, cc, &from);
	    if (npackets && nreceived >= npackets)
		break;
	}
	finish();
	/* NOTREACHED */
    }
    else
    {
	printf("Usage: ping %s\n", ARG_TEMPLATE);
	Quit("invalid arguments");
    }
    return( 0 );
}

/*
 * catcher --
 *	This routine causes another PING to be transmitted, and then
 * schedules another SIGALRM for 1 second from now.
 *
 * bug --
 *	Our sense of time will slowly skew (i.e., packets will not be
 * launched exactly at 1-second intervals).  This does not affect the
 * quality of the delay and loss statistics.
 *
 * notes --
 *      This routine uses timer.device in Amiga implementation instead
 * of SIGALRM.
 */

void catcher() {
  static int waittime = 0;

  if (waittime)
    finish();

  pinger();

  if (!npackets || ntransmitted < npackets) {
    timermsg->tr_time.tv_secs = interval;
    SendIO((struct IORequest*)timermsg);
  } else {
    if (nreceived) {
      waittime = 2 * tmax / 1000;
      if (!waittime)
        waittime = 1;
    } else
      waittime = MAXWAIT;
    timermsg->tr_time.tv_secs = waittime;
    SendIO((struct IORequest*)timermsg);
  }
}

/*
 * pinger --
 * 	Compose and transmit an ICMP ECHO REQUEST packet.  The IP packet
 * will be added on by the kernel.  The ID field is our process ID,
 * and the sequence number is an ascending integer.  The first 8 bytes
 * of the data portion are used to hold a UNIX "timeval" struct in native
 * byte-order, to compute the round-trip time.
 */
void pinger() {
	register struct icmp *icp;
	register int cc;
	int i;

	icp = (struct icmp *)outpack;
	icp->icmp_type = ICMP_ECHO;
	icp->icmp_code = 0;
	icp->icmp_cksum = 0;
	icp->icmp_seq = ntransmitted++;
	icp->icmp_id = ident;			/* ID */

	CLR(icp->icmp_seq % mx_dup_ck);

	if (timing) {
		GetSysTime( (struct timeval *) &outpack[8] );
    }
    cc = datalen + 8;			/* skips ICMP portion */

	/* compute ICMP checksum here */
	icp->icmp_cksum = in_cksum((u_short *)icp, cc);

	i = sendto(s, (char *)outpack, cc, 0, &whereto,
	    sizeof(struct sockaddr));

	if (i < 0 || i != cc)  {
		if (i < 0) {
			//perror("ping: sendto");
            fprintf( stderr, "ping: sendto" );
        }
		(void)printf("ping: wrote %s %d chars, ret=%d\n",
		    hostname, cc, i);
	}
	if (!(options & F_QUIET) && options & F_FLOOD)
		(void)write(1, &DOT, 1);
}

/*
 * pr_pack --
 *	Print out the packet, if it came from us.  This logic is necessary
 * because ALL readers of the ICMP socket get a copy of ALL ICMP packets
 * which arrive ('tis only fair).  This permits multiple copies of this
 * program to be run without having intermingled output (or statistics!).
 */
void pr_pack( char *buf, int cc, struct sockaddr_in *from ) {
	register struct icmp *icp;
	register u_long l;
	register int i, j;
	register u_char *cp,*dp;
	static int old_rrlen;
	static char old_rr[MAX_IPOPTLEN];
	struct ip *ip;
	struct timeval tv, *tp;
	long triptime; /* in milliseconds */
	int hlen, dupflag;

    GetSysTime( &tv );

    /* Check the IP header */
	ip = (struct ip *)buf;
	hlen = ip->ip_hl << 2;
	if (cc < hlen + ICMP_MINLEN) {
		if (options & F_VERBOSE)
			(void)fprintf(stderr,
			  "ping: packet too short (%d bytes) from %s\n", cc,
			  inet_ntoa(*(struct in_addr *)&from->sin_addr.s_addr));
		return;
	}

	/* Now the ICMP part */
	cc -= hlen;
	icp = (struct icmp *)(buf + hlen);
	if (icp->icmp_type == ICMP_ECHOREPLY) {
		if (icp->icmp_id != ident)
			return;			/* 'Twas not our ECHO */
		++nreceived;
		if (timing) {
#ifndef icmp_data
			tp = (struct timeval *)&icp->icmp_ip;
#else
			tp = (struct timeval *)icp->icmp_data;
#endif
            SubTime( &tv, tp ); /* "tv = tv - tp" */

            /* Calculate the triptime in milliseconds */
            triptime = tv.tv_secs * 1000 + tv.tv_micro / 1000;

            tsum += triptime;
			if (triptime < tmin)
				tmin = triptime;
			if (triptime > tmax)
				tmax = triptime;
		}

		if (TST(icp->icmp_seq % mx_dup_ck)) {
			++nrepeats;
			--nreceived;
			dupflag = 1;
		} else {
			SET(icp->icmp_seq % mx_dup_ck);
			dupflag = 0;
		}

		if (options & F_QUIET)
			return;

		if (options & F_FLOOD)
			(void)write(1, &BSPACE, 1);
		else {
			(void)printf("%d bytes from %s: icmp_seq=%u", cc,
			   inet_ntoa(*(struct in_addr *)&from->sin_addr.s_addr),
			   icp->icmp_seq);
			(void)printf(" ttl=%d", ip->ip_ttl);
			if (timing)
				(void)printf(" time=%ld ms", triptime);
			if (dupflag)
				(void)printf(" (DUP!)");
			/* check the data */
			cp = (u_char*)&icp->icmp_data[8];
			dp = &outpack[8 + sizeof(struct timeval)];
			for (i = 8; i < datalen; ++i, ++cp, ++dp) {
				if (*cp != *dp) {
	(void)printf("\nwrong data byte #%d should be 0x%x but was 0x%x",
	    i, *dp, *cp);
					cp = (u_char*)&icp->icmp_data[0];
					for (i = 8; i < datalen; ++i, ++cp) {
						if ((i % 32) == 8)
							(void)printf("\n\t");
						(void)printf("%x ", *cp);
					}
					break;
				}
			}
		}
	} else {
		/* We've got something other than an ECHOREPLY */
		if (!(options & F_VERBOSE))
			return;
		(void)printf("%d bytes from %s: ", cc,
		    pr_addr(from->sin_addr.s_addr));
		pr_icmph(icp);
	}

	/* Display any IP options */
	/* The LSRR and RR len handling was broken (?) //ppessi */
	cp = (u_char *)buf + sizeof(struct ip);

	for (; hlen > (int)sizeof(struct ip); --hlen, ++cp)
		switch (*cp) {
		case IPOPT_EOL:
			hlen = 0;
			break;
		case IPOPT_LSRR:
		case IPOPT_SSRR:
			(void)printf(*cp == IPOPT_LSRR ?
				     "\nLSRR: " : "\nSSRR: ");
			j = *++cp;
			++cp;
			hlen -= j;
			if (j > IPOPT_MINOFF)
				for (;;) {
					l = *++cp;
					l = (l<<8) + *++cp;
					l = (l<<8) + *++cp;
					l = (l<<8) + *++cp;
					if (l == 0)
						(void)printf("\t0.0.0.0");
					else
						(void)printf("\t%s", pr_addr(ntohl(l)));
					j -= 4;
					if (j < IPOPT_MINOFF)
						break;
					(void)putchar('\n');
				}
			break;
		case IPOPT_RR:
			j = *++cp;		/* get length */
			i = *++cp;		/* and pointer */
			hlen -= j;
			if (i > j)
				i = j;
			i -= IPOPT_MINOFF;
			if (i <= 0)
				continue;
			if (i == old_rrlen
			    && cp == (u_char *)buf + sizeof(struct ip) + 2
			    && !memcmp((char *)cp, old_rr, i)
			    && !(options & F_FLOOD)) {
				(void)printf("\t(same route)");
				i = ((i + 3) / 4) * 4;
				cp += i;
				break;
			}
			old_rrlen = i;
			bcopy((char *)cp, old_rr, i);
			(void)printf("\nRR: ");
			for (;;) {
				l = *++cp;
				l = (l<<8) + *++cp;
				l = (l<<8) + *++cp;
				l = (l<<8) + *++cp;
				if (l == 0)
					(void)printf("\t0.0.0.0");
				else
					(void)printf("\t%s", pr_addr(ntohl(l)));
				i -= 4;
				if (i <= 0)
					break;
				(void)putchar('\n');
			}
			break;
		case IPOPT_NOP:
			(void)printf("\nNOP");
			break;
		default:
			(void)printf("\nunknown option %x", *cp);
			break;
		}
	if (!(options & F_FLOOD)) {
		(void)putchar('\n');
		(void)fflush(stdout);
	}
}

/*
 * in_cksum --
 *	Checksum routine for Internet Protocol family headers (C Version)
 */
int in_cksum( u_short *addr, int len ) {
	register int nleft = len;
	register u_short *w = addr;
	register int sum = 0;
	u_short answer = 0;

	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1) {
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return(answer);
}

/*
 * finish --
 *	Print out statistics, and give up.
 */
void finish() {
    putchar('\n');
    fflush(stdout);
    printf("--- %s ping statistics ---\n", hostname);
    printf("%ld packets transmitted, ", ntransmitted);
    printf("%ld packets received, ", nreceived);
    if( nrepeats ) {
        printf( "+%ld duplicates, ", nrepeats );
    }
    if( ntransmitted ) {
        if (nreceived > ntransmitted) {
            printf("-- somebody's printing up packets!");
        } else {
            printf("%ld%% packet loss", (((ntransmitted - nreceived) * 100) / ntransmitted));
        }
    }
    putchar('\n');
    if (nreceived && timing) {
        printf("round-trip min/avg/max = %ld/%lu/%ld ms\n",
                tmin, tsum / (nreceived + nrepeats), tmax);
    }

    Quit( NULL );
}

/*
 * pr_icmph --
 *	Print a descriptive string about an ICMP header.
 */
void pr_icmph( struct icmp *icp ) {
	switch(icp->icmp_type) {
	case ICMP_ECHOREPLY:
		(void)printf("Echo Reply\n");
		/* XXX ID + Seq + Data */
		break;
	case ICMP_UNREACH:
		switch(icp->icmp_code) {
		case ICMP_UNREACH_NET:
			(void)printf("Destination Net Unreachable\n");
			break;
		case ICMP_UNREACH_HOST:
			(void)printf("Destination Host Unreachable\n");
			break;
		case ICMP_UNREACH_PROTOCOL:
			(void)printf("Destination Protocol Unreachable\n");
			break;
		case ICMP_UNREACH_PORT:
			(void)printf("Destination Port Unreachable\n");
			break;
		case ICMP_UNREACH_NEEDFRAG:
			(void)printf("frag needed and DF set\n");
			break;
		case ICMP_UNREACH_SRCFAIL:
			(void)printf("Source Route Failed\n");
			break;
		default:
			(void)printf("Dest Unreachable, Bad Code: %d\n",
			    icp->icmp_code);
			break;
		}
		/* Print returned IP header information */
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_SOURCEQUENCH:
		(void)printf("Source Quench\n");
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_REDIRECT:
		switch(icp->icmp_code) {
		case ICMP_REDIRECT_NET:
			(void)printf("Redirect Network");
			break;
		case ICMP_REDIRECT_HOST:
			(void)printf("Redirect Host");
			break;
		case ICMP_REDIRECT_TOSNET:
			(void)printf("Redirect Type of Service and Network");
			break;
		case ICMP_REDIRECT_TOSHOST:
			(void)printf("Redirect Type of Service and Host");
			break;
		default:
			(void)printf("Redirect, Bad Code: %d", icp->icmp_code);
			break;
		}
		(void)printf("(New addr: 0x%08lx)\n", icp->icmp_gwaddr.s_addr);
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_ECHO:
		(void)printf("Echo Request\n");
		/* XXX ID + Seq + Data */
		break;
	case ICMP_TIMXCEED:
		switch(icp->icmp_code) {
		case ICMP_TIMXCEED_INTRANS:
			(void)printf("Time to live exceeded\n");
			break;
		case ICMP_TIMXCEED_REASS:
			(void)printf("Frag reassembly time exceeded\n");
			break;
		default:
			(void)printf("Time exceeded, Bad Code: %d\n",
			    icp->icmp_code);
			break;
		}
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_PARAMPROB:
		(void)printf("Parameter problem: pointer = 0x%02x\n",
		    icp->icmp_hun.ih_pptr);
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_TSTAMP:
		(void)printf("Timestamp\n");
		/* XXX ID + Seq + 3 timestamps */
		break;
	case ICMP_TSTAMPREPLY:
		(void)printf("Timestamp Reply\n");
		/* XXX ID + Seq + 3 timestamps */
		break;
	case ICMP_IREQ:
		(void)printf("Information Request\n");
		/* XXX ID + Seq */
		break;
	case ICMP_IREQREPLY:
		(void)printf("Information Reply\n");
		/* XXX ID + Seq */
		break;
#ifdef ICMP_MASKREQ
	case ICMP_MASKREQ:
		(void)printf("Address Mask Request\n");
		break;
#endif
#ifdef ICMP_MASKREPLY
	case ICMP_MASKREPLY:
		(void)printf("Address Mask Reply\n");
		break;
#endif
	default:
		(void)printf("Bad ICMP type: %d\n", icp->icmp_type);
	}
}

/*
 * pr_iph --
 *	Print an IP header with options.
 */
void pr_iph( struct ip *ip ) {
	int hlen;
	u_char *cp;

	hlen = ip->ip_hl << 2;
	cp = (u_char *)ip + 20;		/* point to options */

	(void)printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst Data\n");
	(void)printf(" %1x  %1x  %02x %04x %04x",
	    ip->ip_v, ip->ip_hl, ip->ip_tos, ip->ip_len, ip->ip_id);
	(void)printf("   %1x %04x", ((ip->ip_off) & 0xe000) >> 13,
	    (ip->ip_off) & 0x1fff);
	(void)printf("  %02x  %02x %04x", ip->ip_ttl, ip->ip_p, ip->ip_sum);
	(void)printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->ip_src.s_addr));
	(void)printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->ip_dst.s_addr));
	/* dump and option bytes */
	while (hlen-- > 20) {
		(void)printf("%02x", *cp++);
	}
	(void)putchar('\n');
}

/*
 * pr_addr --
 *	Return an ascii host address as a dotted quad and optionally with
 * a hostname.
 */
char *pr_addr( u_long l ) {
	struct hostent *hp;
	static char buf[80];

	if ((options & F_NUMERIC) ||
	    !(hp = gethostbyaddr((char *)&l, 4, AF_INET)))
		(void)sprintf(buf, "%s", inet_ntoa(*(struct in_addr *)&l));
	else
		(void)sprintf(buf, "%s (%s)", hp->h_name,
		    inet_ntoa(*(struct in_addr *)&l));
	return(buf);
}

/*
 * pr_retip --
 *	Dump some info on a returned (via ICMP) IP packet.
 */
void pr_retip( struct ip *ip ) {
	int hlen;
	u_char *cp;

	pr_iph(ip);
	hlen = ip->ip_hl << 2;
	cp = (u_char *)ip + hlen;

	if (ip->ip_p == 6)
		(void)printf("TCP: from port %u, to port %u (decimal)\n",
		    (*cp * 256 + *(cp + 1)), (*(cp + 2) * 256 + *(cp + 3)));
	else if (ip->ip_p == 17)
		(void)printf("UDP: from port %u, to port %u (decimal)\n",
			(*cp * 256 + *(cp + 1)), (*(cp + 2) * 256 + *(cp + 3)));
}

void fill( char *bp, char *patp) {
	register int ii, jj, kk;
	int pat[16];
	char *cp;

	for (cp = patp; *cp; cp++)
		if (!isxdigit(*cp)) {
			Quit( "Patterns must be specified as hex digits.");
		}
	ii = sscanf(patp,
	    "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
	    &pat[0], &pat[1], &pat[2], &pat[3], &pat[4], &pat[5], &pat[6],
	    &pat[7], &pat[8], &pat[9], &pat[10], &pat[11], &pat[12],
	    &pat[13], &pat[14], &pat[15]);

	if (ii > 0)
		for (kk = 0; kk <= MAXPACKET - (8 + ii); kk += ii)
			for (jj = 0; jj < ii; ++jj)
				bp[jj + kk] = pat[jj];
	if (!(options & F_QUIET)) {
		(void)printf("PATTERN: 0x");
		for (jj = 0; jj < ii; ++jj)
			(void)printf("%02x", bp[jj] & 0xFF);
		(void)printf("\n");
	}
}
