/* $Id$
 * 
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>,
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * sana2arp.h - Internal Interfaces to the ARP Protocol
 *
 * Last modified: Fri Jun  4 00:40:05 1993 jraja
 *
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 1.8  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 * Revision 1.8  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 * Revision 1.7  1993/05/16  21:09:43  ppessi
 * RCS version changed.
 *
 * Revision 1.5  93/04/21  00:24:23  00:24:23  ppessi (Pekka Pessi)
 * Minor modifications. Hardware address size is now a macro.
 * 
 * Revision 1.4  93/04/19  02:40:42  02:40:42  ppessi (Pekka Pessi)
 * Test version. In principle this is not ethernet dependet...
 * 
 * Revision 1.3  93/03/10  21:58:53  21:58:53  jraja (Jarno Tapio Rajahalme)
 * Added some intelligence in including needed files.
 * 
 * Revision 1.2  93/03/03  20:49:08  20:49:08  jraja (Jarno Tapio Rajahalme)
 * Cleanup.
 */

#ifndef IF_ARPSANA_H
#define IF_ARPSANA_H

#ifndef IF_H
#include <net/if.h>
#endif

#ifndef IF_ARP_H
#include <net/if_arp.h>
#endif

#ifndef IF_SANA_H
#include <net/if_sana.h>
#endif

/*
 * Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  Structure below is adapted
 * to resolving  addresses.  Field names used correspond to 
 * RFC 826.
 */
struct s2_arppkt {
  struct	arphdr s2a_hdr;	  /* fixed-size header */
  struct {
      u_char sha_dum[MAXADDRARP]; /* space for sender hardware address */
      u_char spa_dum[MAXADDRARP]; /* space for sender protocol address */
      u_char tha_dum[MAXADDRARP]; /* space for target hardware address */
      u_char tpa_dum[MAXADDRARP]; /* space for target protocol address */
  } arpdata;
};
#define	arp_hrd	s2a_hdr.ar_hrd
#define	arp_pro	s2a_hdr.ar_pro
#define	arp_hln	s2a_hdr.ar_hln
#define	arp_pln	s2a_hdr.ar_pln
#define	arp_op	s2a_hdr.ar_op

#define	ARPTAB_HSIZE	11	/* hash table size */
#define ARPENTRIES      11*15	/* normal amount of ARP entries to allocate */
#define ARPENTRIES_MIN  11	/* minimum # of ARP entries to allocate */

/*
 * timer values
 */
#define	ARPT_AGE	(60*1)	/* aging timer, 1 min. */
#define	ARPT_KILLC	20	/* kill completed entry in 20 mins. */
#define	ARPT_KILLI	3	/* kill incomplete entry in 3 minutes */

#ifdef	KERNEL
void alloc_arptable(struct sana_softc* ssc, int to_allocate);
void arptimer(void);
int arpresolve(register struct sana_softc *ssc, struct mbuf * m,
               register struct in_addr *destip, register u_char * desten,
               int * error);
void arpinput(struct sana_softc *ssc, struct mbuf *m, caddr_t srcaddr);
int arpioctl(int cmd, caddr_t data);
#endif

#endif /* !IF_ARPSANA_H */
