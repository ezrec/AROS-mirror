/*
 * Copyright (C) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
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

/*
 * Copyright © 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/****** netutil.doc/ifconfig ************************************************

   NAME
        ifconfig - configure network interface parameters

   VERSION
        $Id$

   SYNOPSIS
        ifconfig interface address_family [address [dest_address]] [params]
        ifconfig interface [address_family]

   DESCRIPTION
        ifconfig is used to assign an address to a network interface and/or
        configure network interface parameters.  ifconfig must be used at
        boot time to define the network address of each interface present on
        a machine.  It can also be used at other times to redefine an
        interface's address or other operating parameters.

   PARAMETERS
        interface     A string of the interface name concatenated with unit
                      numver, for example `eth0'.  The AROSTCP network
                      interfaces are defined in the `db/interfaces'
                      file.  For example, an interface sl corresponds by
                      default to `Devs:networks/rhcslip.device'.

        address_family

                      Name of protocol on which naming scheme is based.  An
                      interface can receive transmissions in differing
                      protocols, each of which may require separate naming
                      schemes.  Therefore, it is necessary to specify the
                      address_family, which may affect interpretation of the
                      remaining parameters on the command line.  The only
                      address family currently supported is inet (DARPA-
                      Internet family).

        address       Either a host name present in the host name database,
                      (SEE hosts), or a DARPA Internet address
                      expressed in Internet standard "dot notation".  The
                      host number can be omitted on 10-Mbyte/second Ethernet
                      interfaces (which use the hardware physical address),
                      and on interfaces other than the first.

        dest_address  Address of destination system.  Consists of either a
                      host name present in the host name database, hosts(4),
                      or a DARPA Internet address expressed in Internet
                      standard "dot notation".

   SWITCHES
        The following operating parameters can be specified:

         up           Mark an interface "up". Enables interface after an
                      "ifconfig down."  Occurs automatically when setting the
                      address on an interface.  Setting this flag has no
                      effect if the hardware is "down".

         down         Mark an interface "down".  When an interface is marked
                      "down", the system will not attempt to transmit
                      messages through that interface. If possible, the
                      interface will be reset to disable reception as well.
                      This action does not automatically disable routes
                      using the interface.

        arp           Enable the use of Address Resolution Protocol in
                      mapping between network level addresses and link-level
                      addresses (default).

        -arp          Disable the use of Address Resolution Protocol.

         metric n     Set the routing metric of the interface to n, default
                      0.  The routing metric is used by the routing protocol
                      (see gated).  Higher metrics have the effect of making
                      a route less favorable; metrics are counted as
                      additional hops to the destination network or host.

         debug        Enable driver-dependent debugging code. This usually
                      turns on extra console error logging.

        -debug        Disable driver-dependent debugging code.

         netmask mask (Inet only) Specify how much of the address to reserve
                      for subdividing networks into sub-networks.  mask
                      includes the network part of the local address, and
                      the subnet part which is taken from the host field of
                      the address.  mask can be specified as a single hexa-
                      decimal number with a leading 0x, with a dot-notation
                      Internet address, or with a pseudo-network name listed
                      in the file db/networks.  `mask' contains 1's
                      for each bit position in the 32-bit address that are
                      to be used for the network and subnet parts, and 0's
                      for the host part.  mask should contain at least the
                      standard network portion, and the subnet field should
                      be contiguous with the network portion.

        broadcast    (Inet only) Specify the address that represents
                      broadcasts to the network.  The default broadcast
                      address is the address with a host part of all 1's.

        The command:

             ifconfig interface/unit

        with no optional command arguments supplied displays the current
        configuration for interface.  If address_family is specified,
        ifconfig reports only the details specific to that address family.

   DIAGNOSTICS
        Messages indicating that the specified interface does not exist, the
        requested address is unknown, or the user is not privileged and
        tried to alter an interface's configuration.

   EXAMPLES
        ifconfig lo0 127.0.0.1

                This command marks internal loopback device "UP", and
                attach an inet address 127.0.0.1 to it.

        ifconfig cslip0 inet 193.102.4.144 193.102.4.129

                This command starts the CSLIP driver, attach an
                address 193.102.4.144 (our internet address) and a
                destination address 193.102.4.129 (the internet
                address of the host you are connecting) to it.

        ifconfig eth0 inet 193.124.100.64 netmask 255.255.255.192 -arp

                This command loads an ethernet driver (by default for the
                Commodore A2065 Ethernet adapter unit 0), marks it "up",
                disables ARP protocol for it, attaches an inet address
                193.124.100.65 to it and sets its netmask to
                255.255.255.192.  A bitwise logical and of netmask and
                address for the interface forms a subnet address, in this
                case 193.124.100.64.  All packets aimed to hosts with same
                subnet address (that is hosts 193.124.100.64 -
                193.124.100.127) are routed to this interface.

   FILES
        db/interfaces

   SEE ALSO
        netstat, hosts, arp, "net/if.h", "net/sana2tags.h"

*****************************************************************************
*
*/

#include <dos/dos.h>

#include <proto/exec.h>
#include <proto/socket.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef NS
#define	NSIP
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#include <netdb.h>

#ifdef ISO
#define EON
#include <netiso/iso.h>
#include <netiso/iso_var.h>
#endif

#ifdef notyet
#include <unistd.h>
#endif
#include <stdio.h>
#include <sys/errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct	ifreq		ifr, ridreq;
struct	ifaliasreq	addreq;
#ifdef ISO
struct	iso_ifreq	iso_ridreq;
struct	iso_aliasreq	iso_addreq;
#endif
struct	sockaddr_in	netmask;

char	name[1024];
int	flags;
int	metric;
int	setaddr;
int	setipdst;
int	doalias;
int	clearaddr;
int	newaddr = 1;
int	s;

#ifdef NS
int	nsellength = 1;
#endif

void Perror(char *cmd);
void status(void);
void printb(char *s, register unsigned short v, register char *bits);

void setifflags(char *, short);
void setifaddr(char *, short);
void setifdstaddr(char *, short);
void setifnetmask(char *, short);
void setifmetric(char *, short);
void setifbroadaddr(char *, short);
void setifipdst(char *, short);
void notealias(char *, short);
void setsnpaoffset(char *, short);
void setnsellength(char *, short);
VOID CleanUpExit(LONG error);

#define	NEXTARG		0xffffff

struct	cmd {
	char	*c_name;
	int	c_parameter;		/* NEXTARG means next argv */
	void 	(*c_func)(char *, short);
} cmds[] = {
	{ "up",		IFF_UP,		setifflags } ,
	{ "down",	-IFF_UP,	setifflags },
	{ "trailers",	-IFF_NOTRAILERS,setifflags },
	{ "-trailers",	IFF_NOTRAILERS,	setifflags },
	{ "arp",	-IFF_NOARP,	setifflags },
	{ "-arp",	IFF_NOARP,	setifflags },
	{ "debug",	IFF_DEBUG,	setifflags },
	{ "-debug",	-IFF_DEBUG,	setifflags },
	{ "alias",	IFF_UP,		notealias },
	{ "-alias",	-IFF_UP,	notealias },
	{ "delete",	-IFF_UP,	notealias },
#ifdef notdef
#define	EN_SWABIPS	0x1000
	{ "swabips",	EN_SWABIPS,	setifflags },
	{ "-swabips",	-EN_SWABIPS,	setifflags },
#endif
	{ "netmask",	NEXTARG,	setifnetmask },
	{ "metric",	NEXTARG,	setifmetric },
	{ "broadcast",	NEXTARG,	setifbroadaddr },
	{ "ipdst",	NEXTARG,	setifipdst },
#ifdef ISO
	{ "snpaoffset",	NEXTARG,	setsnpaoffset },
	{ "nsellength",	NEXTARG,	setnsellength },
#endif
	{ 0,		0,		setifaddr },
	{ 0,		0,		setifdstaddr },
};

void in_status(int);
void in_getaddr(char *addr, int which);
#ifdef NS
/*
 * XNS support liberally adapted from
 * code written at the University of Maryland
 * principally by James O'Toole and Chris Torek.
 */
void xns_status(int);
void xns_getaddr(char *addr, int which);
#endif
#ifdef ISO
void iso_status(int);
void iso_getaddr(char *addr, int which);
#endif

/* Known address families */
struct afswtch {
	char *af_name;
	short af_af;
	void (*af_status)(int);
	void (*af_getaddr)(char *, int);
	int af_difaddr;
	int af_aifaddr;
	caddr_t af_ridreq;
	caddr_t af_addreq;
} afs[] = {
#define C(x) ((caddr_t) &x)
	{ "inet", AF_INET, in_status, in_getaddr,
	     SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq) },
#ifdef NS
	{ "ns", AF_NS, xns_status, xns_getaddr,
	     SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq) },
#endif
#ifdef ISO
	{ "iso", AF_ISO, iso_status, iso_getaddr,
	     SIOCDIFADDR_ISO, SIOCAIFADDR_ISO, C(iso_ridreq), C(iso_addreq) },
#endif
	{ 0,	0,	    0,		0 }
};

struct afswtch *af_p;	/*the address family being set or asked about*/

#define SOCKET_VERSION 3
struct Library *SocketBase;
const TEXT version[] = "ifconfig 3.2 (29.12.2004)";
const TEXT socket_name[] = "bsdsocket.library";
#define ioctl IoctlSocket

main(argc, argv)
	int argc;
	char *argv[];
{
	int af = AF_INET;
	register struct afswtch *raf_p;

	SocketBase = OpenLibrary(socket_name, SOCKET_VERSION);
	if(SocketBase == NULL)
		return RETURN_FAIL;

	if (argc < 2) {
		fprintf(stderr, "usage: ifconfig interface\n%s%s%s%s%s",
		    "\t[ [ af ] [ address [ dest_addr ] ] [ up ] [ down ]",
			    "[ netmask mask ] ]\n",
		    "\t[ metric n ]\n",
		    "\t[ trailers | -trailers ]\n",
		    "\t[ arp | -arp ]\n");
		CleanUpExit(1);
	}
	argc--, argv++;
	strncpy(name, *argv, sizeof(name));
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	argc--, argv++;
	if (argc > 0) {
		for (af_p = raf_p = afs; raf_p->af_name; raf_p++)
			if (strcmp(raf_p->af_name, *argv) == 0) {
				af_p = raf_p; argc--; argv++;
				break;
			}
		raf_p = af_p;
		af = ifr.ifr_addr.sa_family = raf_p->af_af;
	}
	s = socket(af, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("ifconfig: socket");
		CleanUpExit(1);
	}
	if (ioctl(s, SIOCGIFFLAGS, (caddr_t)&ifr) < 0) {
		Perror("ioctl (SIOCGIFFLAGS)");
		CleanUpExit(1);
	}
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	flags = ifr.ifr_flags;
	if (ioctl(s, SIOCGIFMETRIC, (caddr_t)&ifr) < 0)
		perror("ioctl (SIOCGIFMETRIC)");
	else
		metric = ifr.ifr_metric;
	if (argc == 0) {
		status();
		CleanUpExit(0);
	}
	while (argc > 0) {
		register struct cmd *p;

		for (p = cmds; p->c_name; p++)
			if (strcmp(*argv, p->c_name) == 0)
				break;
		if (p->c_name == 0 && setaddr)
			p++;	/* got src, do dst */
		if (p->c_func) {
			if (p->c_parameter == NEXTARG) {
				(*p->c_func)(argv[1], 0);
				argc--, argv++;
			} else
				(*p->c_func)(*argv, p->c_parameter);
		}
		argc--, argv++;
	}
#ifdef ISO
	if (af == AF_ISO)
		adjust_nsellength();
#endif
#ifdef NS
	if (setipdst && af==AF_NS) {
		struct nsip_req rq;
		int size = sizeof(rq);

		rq.rq_ns = addreq.ifra_addr;
		rq.rq_ip = addreq.ifra_dstaddr;

		if (setsockopt(s, 0, SO_NSIP_ROUTE, &rq, size) < 0)
			Perror("Encapsulation Routing");
	}
#endif
	if (clearaddr) {
		int ret;
		strncpy(raf_p->af_ridreq, name, sizeof ifr.ifr_name);
		if ((ret = ioctl(s, raf_p->af_difaddr, raf_p->af_ridreq)) < 0) {
			if (Errno() == EADDRNOTAVAIL && (doalias >= 0)) {
				/* means no previous address for interface */
			} else
				Perror("ioctl (SIOCDIFADDR)");
		}
	}
	if (newaddr) {
		strncpy(raf_p->af_addreq, name, sizeof ifr.ifr_name);
		if (ioctl(s, raf_p->af_aifaddr, raf_p->af_addreq) < 0)
			Perror("ioctl (SIOCAIFADDR)");
	}
	CleanUpExit(0);
}
#define RIDADDR 0
#define ADDR	1
#define MASK	2
#define DSTADDR	3

/*ARGSUSED*/
void
setifaddr(char *addr, short param)
{
	/*
	 * Delay the ioctl to set the interface addr until flags are all set.
	 * The address interpretation may depend on the flags,
	 * and the flags may change when the address is set.
	 */
	setaddr++;
	if (doalias == 0)
		clearaddr = 1;
	(*af_p->af_getaddr)(addr, (doalias >= 0 ? ADDR : RIDADDR));
}

void
setifnetmask(char *addr, short dummy)
{
	(*af_p->af_getaddr)(addr, MASK);
}

void
setifbroadaddr(char *addr, short summy)
{
	(*af_p->af_getaddr)(addr, DSTADDR);
}

void
setifipdst(char *addr, short dummy)
{
	in_getaddr(addr, DSTADDR);
	setipdst++;
	clearaddr = 0;
	newaddr = 0;
}

#define rqtosa(x) (&(((struct ifreq *)(af_p->x))->ifr_addr))

void
notealias(char *addr, short param)
{
	if (setaddr && doalias == 0 && param < 0)
		bcopy((caddr_t)rqtosa(af_addreq),
		      (caddr_t)rqtosa(af_ridreq),
		      rqtosa(af_addreq)->sa_len);
	doalias = param;
	if (param < 0) {
		clearaddr = 1;
		newaddr = 0;
	} else
		clearaddr = 0;
}

/*ARGSUSED*/
void
setifdstaddr(char *addr, short param)
{
	(*af_p->af_getaddr)(addr, DSTADDR);
}

void
setifflags(char *vname, short value)
{
 	if (ioctl(s, SIOCGIFFLAGS, (caddr_t)&ifr) < 0) {
 		Perror("ioctl (SIOCGIFFLAGS)");
 		CleanUpExit(1);
 	}
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
 	flags = ifr.ifr_flags;

	if (value < 0) {
		value = -value;
		flags &= ~value;
	} else
		flags |= value;
	ifr.ifr_flags = flags;
	if (ioctl(s, SIOCSIFFLAGS, (caddr_t)&ifr) < 0)
		Perror(vname);
}

void
setifmetric(char *val, short dummy)
{
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	ifr.ifr_metric = atoi(val);
	if (ioctl(s, SIOCSIFMETRIC, (caddr_t)&ifr) < 0)
		perror("ioctl (set metric)");
}
#ifdef ISO
void
setsnpaoffset(char *val, short dummy)
{
	iso_addreq.ifra_snpaoffset = atoi(val);
}
#endif
#define	IFFBITS \
"\020\1UP\2BROADCAST\3DEBUG\4LOOPBACK\5POINTOPOINT\6NOTRAILERS\7RUNNING\10NOARP\
"

/*
 * Print the status of the interface.  If an address family was
 * specified, show it and it only; otherwise, show them all.
 */
void
status(void)
{
	register struct afswtch *p = af_p;
	short af = ifr.ifr_addr.sa_family;

	printf("%s: ", name);
	printb("flags", flags, IFFBITS);
	if (metric)
		printf(" metric %d", metric);
	putchar('\n');
	if ((p = af_p) != NULL) {
		(*p->af_status)(1);
	} else for (p = afs; p->af_name; p++) {
		ifr.ifr_addr.sa_family = p->af_af;
		(*p->af_status)(0);
	}
}

void
in_status(int force)
{
	struct sockaddr_in *s_in;

	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR, (caddr_t)&ifr) < 0) {
		if (Errno() == EADDRNOTAVAIL || Errno() == EAFNOSUPPORT) {
			if (!force)
				return;
			bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
		} else
			perror("ioctl (SIOCGIFADDR)P1");
	}
	s_in = (struct sockaddr_in *)&ifr.ifr_addr;
	printf("\tinet %s ", Inet_NtoA(s_in->sin_addr.s_addr));
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	if (ioctl(s, SIOCGIFNETMASK, (caddr_t)&ifr) < 0) {
		if (Errno() != EADDRNOTAVAIL)
			perror("ioctl (SIOCGIFNETMASK)");
		bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
	} else
		netmask.sin_addr =
		    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
	if (flags & IFF_POINTOPOINT) {
		if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
			if (Errno() == EADDRNOTAVAIL)
			    bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
			else
			    perror("ioctl (SIOCGIFDSTADDR)");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		s_in = (struct sockaddr_in *)&ifr.ifr_dstaddr;
		printf("--> %s ", Inet_NtoA(s_in->sin_addr.s_addr));
	}
	printf("netmask %x ", ntohl(netmask.sin_addr.s_addr));
	if (flags & IFF_BROADCAST) {
		if (ioctl(s, SIOCGIFBRDADDR, (caddr_t)&ifr) < 0) {
			if (Errno() == EADDRNOTAVAIL)
			    bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
			else
			    perror("ioctl (SIOCGIFADDR)");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		s_in = (struct sockaddr_in *)&ifr.ifr_addr;
		if (s_in->sin_addr.s_addr != 0)
			printf("broadcast %s", Inet_NtoA(s_in->sin_addr.s_addr));
	}
	putchar('\n');
}

#ifdef NS
xns_status(force)
	int force;
{
	struct sockaddr_ns *sns;

	close(s);
	s = socket(AF_NS, SOCK_DGRAM, 0);
	if (s < 0) {
		if (Errno() == EPROTONOSUPPORT)
			return;
		perror("ifconfig: socket");
		CleanUpExit(1);
	}
	if (ioctl(s, SIOCGIFADDR, (caddr_t)&ifr) < 0) {
		if (Errno() == EADDRNOTAVAIL || Errno() == EAFNOSUPPORT) {
			if (!force)
				return;
			bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
		} else
			perror("ioctl (SIOCGIFADDR)");
	}
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	sns = (struct sockaddr_ns *)&ifr.ifr_addr;
	printf("\tns %s ", ns_ntoa(sns->sns_addr));
	if (flags & IFF_POINTOPOINT) { /* by W. Nesheim@Cornell */
		if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
			if (Errno() == EADDRNOTAVAIL)
			    bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
			else
			    Perror("ioctl (SIOCGIFDSTADDR)");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		sns = (struct sockaddr_ns *)&ifr.ifr_dstaddr;
		printf("--> %s ", ns_ntoa(sns->sns_addr));
	}
	putchar('\n');
}
#endif
#ifdef ISO
iso_status(force)
	int force;
{
	struct sockaddr_iso *siso;
	struct iso_ifreq ifr;

	close(s);
	s = socket(AF_ISO, SOCK_DGRAM, 0);
	if (s < 0) {
		if (Errno() == EPROTONOSUPPORT)
			return;
		perror("ifconfig: socket");
		CleanUpExit(1);
	}
	bzero((caddr_t)&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR_ISO, (caddr_t)&ifr) < 0) {
		if (Errno() == EADDRNOTAVAIL || Errno() == EAFNOSUPPORT) {
			if (!force)
				return;
			bzero((char *)&ifr.ifr_Addr, sizeof(ifr.ifr_Addr));
		} else {
			perror("ioctl (SIOCGIFADDR_ISO)");
			CleanUpExit(1);
		}
	}
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	siso = &ifr.ifr_Addr;
	printf("\tiso %s ", iso_ntoa(&siso->siso_addr));
	if (ioctl(s, SIOCGIFNETMASK_ISO, (caddr_t)&ifr) < 0) {
		if (Errno() != EADDRNOTAVAIL)
			perror("ioctl (SIOCGIFNETMASK_ISO)");
	} else {
		printf(" netmask %s ", iso_ntoa(&siso->siso_addr));
	}
	if (flags & IFF_POINTOPOINT) {
		if (ioctl(s, SIOCGIFDSTADDR_ISO, (caddr_t)&ifr) < 0) {
			if (Errno() == EADDRNOTAVAIL)
			    bzero((char *)&ifr.ifr_Addr, sizeof(ifr.ifr_Addr));
			else
			    Perror("ioctl (SIOCGIFDSTADDR_ISO)");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		siso = &ifr.ifr_Addr;
		printf("--> %s ", iso_ntoa(&siso->siso_addr));
	}
	putchar('\n');
}
#endif

void
Perror(char *cmd)
{
/*	extern int errno;*/

	fprintf(stderr, "ifconfig: ");
	switch (Errno()) {

	case ENXIO:
		fprintf(stderr, "%s: no such interface\n", cmd);
		break;

	case EPERM:
		fprintf(stderr, "%s: permission denied\n", cmd);
		break;

	default:
		perror(cmd);
	}
	CleanUpExit(1);
}

#define SIN(x) ((struct sockaddr_in *) &(x))
struct sockaddr_in *sintab[] = {
SIN(ridreq.ifr_addr), SIN(addreq.ifra_addr),
SIN(addreq.ifra_mask), SIN(addreq.ifra_broadaddr)};

void
in_getaddr(char *s, int which)
{
	register struct sockaddr_in *s_in = sintab[which];
	struct hostent *hp;
	struct netent *np;
	int val;

	s_in->sin_len = sizeof(*s_in);
	if (which != MASK)
		s_in->sin_family = AF_INET;

	if ((val = inet_addr(s)) != -1)
		s_in->sin_addr.s_addr = val;
	else if (hp = gethostbyname(s))
		bcopy(hp->h_addr, (char *)&s_in->sin_addr, hp->h_length);
#if 0
	else if (np = getnetbyname(s))
		s_in->sin_addr.s_addr = Inet_MakeAddr(np->n_net, INADDR_ANY);
#endif
	else {
		fprintf(stderr, "%s: bad value\n", s);
		CleanUpExit(1);
	}
}

/*
 * Print a value a la the %b format of the kernel's printf
 */
void
printb(char *s, register unsigned short v, register char *bits)
{
	register int i, any = 0;
	register char c;

	if (bits && *bits == 8)
		printf("%s=%o", s, v);
	else
		printf("%s=%x", s, v);
	bits++;
	if (bits) {
		putchar('<');
		while (i = *bits++) {
			if (v & (1 << (i-1))) {
				if (any)
					putchar(',');
				any = 1;
				for (; (c = *bits) > 32; bits++)
					putchar(c);
			} else
				for (; *bits > 32; bits++)
					;
		}
		putchar('>');
	}
}

#ifdef NS
#define SNS(x) ((struct sockaddr_ns *) &(x))
struct sockaddr_ns *snstab[] = {
SNS(ridreq.ifr_addr), SNS(addreq.ifra_addr),
SNS(addreq.ifra_mask), SNS(addreq.ifra_broadaddr)};

xns_getaddr(addr, which)
char *addr;
{
	struct sockaddr_ns *sns = snstab[which];
	struct ns_addr ns_addr();

	sns->sns_family = AF_NS;
	sns->sns_len = sizeof(*sns);
	sns->sns_addr = ns_addr(addr);
	if (which == MASK)
		printf("Attempt to set XNS netmask will be ineffectual\n");
}
#endif

#ifdef ISO
#define SISO(x) ((struct sockaddr_iso *) &(x))
struct sockaddr_iso *sisotab[] = {
SISO(iso_ridreq.ifr_Addr), SISO(iso_addreq.ifra_addr),
SISO(iso_addreq.ifra_mask), SISO(iso_addreq.ifra_dstaddr)};

iso_getaddr(addr, which)
char *addr;
{
	register struct sockaddr_iso *siso = sisotab[which];
	struct iso_addr *iso_addr();
	siso->siso_addr = *iso_addr(addr);

	if (which == MASK) {
		siso->siso_len = TSEL(siso) - (caddr_t)(siso);
		siso->siso_nlen = 0;
	} else {
		siso->siso_len = sizeof(*siso);
		siso->siso_family = AF_ISO;
	}
}

void
setnsellength(char *val, short dummy)
{
	nsellength = atoi(val);
	if (nsellength < 0) {
		fprintf(stderr, "Negative NSEL length is absurd\n");
		CleanUpExit (1);
	}
	if (af_p == 0 || af_p->af_af != AF_ISO) {
		fprintf(stderr, "Setting NSEL length valid only for iso\n");
		CleanUpExit (1);
	}
}

fixnsel(s)
register struct sockaddr_iso *s;
{
	if (s->siso_family == 0)
		return;
	s->siso_tlen = nsellength;
}

adjust_nsellength()
{
	fixnsel(sisotab[RIDADDR]);
	fixnsel(sisotab[ADDR]);
	fixnsel(sisotab[DSTADDR]);
}
#endif

VOID CleanUpExit(LONG error)
{
	CloseLibrary(SocketBase);
	exit(error);
}
