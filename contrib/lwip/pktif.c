/*
 * Copyright (c) 2001 Florian Schulze.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of the contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * pktif.c - This file is part of lwIPtest
 *
 ****************************************************************************
 *
 * This file is derived from an example in lwIP with the following license:
 *
 * Copyright (c) 2001, Swedish Institute of Computer Science.
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
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "lwip/debug.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/ip.h"

#include "netif/arp.h"
#include "netif/tcpdump.h"

#include "pktdrv.h"

#undef NETIF_DEBUG

/* Define those to better describe your network interface. */
#define IFNAME0 'p'
#define IFNAME1 'k'

struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

static const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};

/* Forward declarations. */
static void  ethernetif_input(struct netif *netif);
static err_t ethernetif_output(struct netif *netif, struct pbuf *p,
			       struct ip_addr *ipaddr);

static struct netif *pktif_netif;

static unsigned char snd_buf[SBUFSIZE];

/*-----------------------------------------------------------------------------------*/
static void
low_level_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  ethernetif = netif->state;
  
	if (pkt_init(0))
	{
		fprintf(stderr,"Couldn't initialize pktdrv!\n");
		exit(1);
	}
	
  /* Obtain MAC address from network interface. */
  bcopy(eth_addr, ethernetif->ethaddr->addr, 6);

#ifdef NETIF_DEBUG
	DEBUGF(NETIF_DEBUG, ("pktif: eth_addr %02X%02X%02X%02X%02X%02X\n",ethernetif->ethaddr->addr[0],ethernetif->ethaddr->addr[1],ethernetif->ethaddr->addr[2],ethernetif->ethaddr->addr[3],ethernetif->ethaddr->addr[4],ethernetif->ethaddr->addr[5]));
#endif /* NETIF_DEBUG */
  /* Do whatever else is needed to initialize interface. */  
	
	pktif_netif=netif;
}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */
/*-----------------------------------------------------------------------------------*/

static err_t
low_level_output(struct ethernetif *ethernetif, struct pbuf *p)
{
  struct pbuf *q;
	send_buffer_t sbuf;
	unsigned char *ptr;

  /* initiate transfer(); */
	sbuf.size=p->tot_len;
	sbuf.retries=1;
	sbuf.eth_header=NULL;
	sbuf.data=(void *)snd_buf;
  
	ptr=(unsigned char *)sbuf.data;
  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
    /* send data from(q->payload, q->len); */
#ifdef NETIF_DEBUG
		DEBUGF(NETIF_DEBUG, ("netif: send ptr %p q->payload %p q->len %i q->next %p\n", ptr, q->payload, (int)q->len, q->next));
#endif
		bcopy(q->payload,ptr,q->len);
		ptr+=q->len;
  }

  /* signal that packet should be sent(); */
	
	if (pkt_send(&sbuf)<0)
		return ERR_BUF;

#ifdef LINK_STATS
  stats.link.xmit++;
#endif /* LINK_STATS */      

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 */
/*-----------------------------------------------------------------------------------*/
static struct pbuf *
low_level_input(struct ethernetif *ethernetif)
{
  struct pbuf *p, *q;
  int start, length;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  length = pkt_get_current_length();
	if (length<=0)
		return NULL;

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_LINK, (u16_t)length, PBUF_POOL);
#ifdef NETIF_DEBUG
	DEBUGF(NETIF_DEBUG, ("netif: recv length %i p->tot_len %i\n", length, (int)p->tot_len));
#endif
	
  if(p != NULL) {
    /* We iterate over the pbuf chain until we have read the entire
       packet into the pbuf. */
		start=0;
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
         avaliable data in the pbuf is given by the q->len
         variable. */
      /* read data into(q->payload, q->len); */
#ifdef NETIF_DEBUG
			DEBUGF(NETIF_DEBUG, ("netif: recv start %i length %i q->payload %p q->len %i q->next %p\n", start, length, q->payload, (int)q->len, q->next));
#endif
			pkt_read_current(q->payload,start,q->len);
			start+=q->len;
			length-=q->len;
			if (length<=0)
				break;
    }
    /* acknowledge that packet has been read(); */
		pkt_drop_current();
#ifdef LINK_STATS
    stats.link.recv++;
#endif /* LINK_STATS */      
  } else {
    /* drop packet(); */
		pkt_drop_current();
#ifdef LINK_STATS
    stats.link.memerr++;
    stats.link.drop++;
#endif /* LINK_STATS */      
  }

  return p;  
}
/*-----------------------------------------------------------------------------------*/
/*
 * ethernetif_output():
 *
 * This function is called by the TCP/IP stack when an IP packet
 * should be sent. It calls the function called low_level_output() to
 * do the actuall transmission of the packet.
 *
 */
/*-----------------------------------------------------------------------------------*/
static err_t
ethernetif_output(struct netif *netif, struct pbuf *p,
		  struct ip_addr *ipaddr)
{
  struct ethernetif *ethernetif;
  struct pbuf *q;
  struct eth_hdr *ethhdr;
  struct eth_addr *dest, mcastaddr;
  struct ip_addr *queryaddr;
  err_t err;
  u8_t i;
  
  ethernetif = netif->state;

  /* Make room for Ethernet header. */
  if(pbuf_header(p, 14) != 0) {
    /* The pbuf_header() call shouldn't fail, but we allocate an extra
       pbuf just in case. */
    q = pbuf_alloc(PBUF_LINK, 14, PBUF_RAM);
    if(q == NULL) {
#ifdef LINK_STATS
      stats.link.drop++;
      stats.link.memerr++;
#endif /* LINK_STATS */      
      return ERR_MEM;
    }
    pbuf_chain(q, p);
    p = q;
  }

  /* Construct Ethernet header. Start with looking up deciding which
     MAC address to use as a destination address. Broadcasts and
     multicasts are special, all other addresses are looked up in the
     ARP table. */
  queryaddr = ipaddr;
  if(ip_addr_isany(ipaddr) ||
     ip_addr_isbroadcast(ipaddr, &(netif->netmask))) {
    dest = (struct eth_addr *)&ethbroadcast;
  } else if(ip_addr_ismulticast(ipaddr)) {
    /* Hash IP multicast address to MAC address. */
    mcastaddr.addr[0] = 0x01;
    mcastaddr.addr[1] = 0x0;
    mcastaddr.addr[2] = 0x5e;
    mcastaddr.addr[3] = ip4_addr2(ipaddr) & 0x7f;
    mcastaddr.addr[4] = ip4_addr3(ipaddr);
    mcastaddr.addr[5] = ip4_addr4(ipaddr);
    dest = &mcastaddr;
  } else {

    if(ip_addr_maskcmp(ipaddr, &(netif->ip_addr), &(netif->netmask))) {
      /* Use destination IP address if the destination is on the same
         subnet as we are. */
      queryaddr = ipaddr;
    } else {
      /* Otherwise we use the default router as the address to send
         the Ethernet frame to. */
      queryaddr = &(netif->gw);
    }
    dest = arp_lookup(queryaddr);
  }


  /* If the arp_lookup() didn't find an address, we send out an ARP
     query for the IP address. */
  if(dest == NULL) {
    q = arp_query(netif, ethernetif->ethaddr, queryaddr);
    if(q != NULL) {
      err = low_level_output(ethernetif, q);
      pbuf_free(q);
      return err;
    }
#ifdef LINK_STATS
    stats.link.drop++;
    stats.link.memerr++;
#endif /* LINK_STATS */          
    return ERR_MEM;
  }
  ethhdr = p->payload;

  for(i = 0; i < 6; i++) {
    ethhdr->dest.addr[i] = dest->addr[i];
    ethhdr->src.addr[i] = ethernetif->ethaddr->addr[i];
  }
  
  ethhdr->type = htons(ETHTYPE_IP);
  
  return low_level_output(ethernetif, p);

}
/*-----------------------------------------------------------------------------------*/
/*
 * ethernetif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
static void
ethernetif_input(struct netif *netif)
{
  struct ethernetif *ethernetif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;


  ethernetif = netif->state;
  
  p = low_level_input(ethernetif);

  if(p != NULL) {

#ifdef LINK_STATS
    stats.link.recv++;
#endif /* LINK_STATS */

    ethhdr = p->payload;
    
    switch(htons(ethhdr->type)) {
    case ETHTYPE_IP:
      arp_ip_input(netif, p);
      pbuf_header(p, -14);
			//if(ip_lookup(p->payload, netif)) {
	      netif->input(p, netif);
			//}
      break;
    case ETHTYPE_ARP:
      p = arp_arp_input(netif, ethernetif->ethaddr, p);
      if(p != NULL) {
				low_level_output(ethernetif, p);
				pbuf_free(p);
      }
      break;
    default:
      pbuf_free(p);
      break;
    }
  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * ethernetif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
void
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;
    
  ethernetif = mem_malloc(sizeof(struct ethernetif));
  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = ethernetif_output;
  
  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  low_level_init(netif);
  arp_init();  
}
/*-----------------------------------------------------------------------------------*/
/*
 * pktif_update():
 *
 * Needs to be called periodically to get new packets. This could
 * be done inside a thread.
 */
/*-----------------------------------------------------------------------------------*/
void pktif_update(void)
{
  if (pkt_get_current_length()>0)
		ethernetif_input(pktif_netif);
}
/*-----------------------------------------------------------------------------------*/
void pktif_release(void)
{
	pkt_release();
}

