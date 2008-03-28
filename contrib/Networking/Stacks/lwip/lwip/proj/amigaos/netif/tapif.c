/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>,
 *         Sebastian Bauer <sebauer@t-online.de>
 *
 */

#include "lwip/debug.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/ip.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"

#include "netif/etharp.h"

#if defined(LWIP_DEBUG) && defined(LWIP_TCPDUMP)
#include "netif/tcpdump.h"
#endif /* LWIP_DEBUG && LWIP_TCPDUMP */

#include <proto/exec.h>
#include <proto/tapinterface.h>

struct Library *TapInterfaceBase;

#define IFNAME0 't'
#define IFNAME1 'p'

struct tapif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  APTR interface;
};

/* Forward declarations. */
static void  tapif_input(struct netif *netif);
static err_t tapif_output(struct netif *netif, struct pbuf *p,
			       struct ip_addr *ipaddr);

static void tapif_thread(void *data);

/*-----------------------------------------------------------------------------------*/
static void
low_level_init(struct netif *netif)
{
  struct tapif *tapif;

  tapif = netif->state;

  /* Obtain MAC address from network interface. */

  /* (We just fake an address...) */
  tapif->ethaddr->addr[0] = 0x1;
  tapif->ethaddr->addr[1] = 0x2;
  tapif->ethaddr->addr[2] = 0x3;
  tapif->ethaddr->addr[3] = 0x4;
  tapif->ethaddr->addr[4] = 0x5;
  tapif->ethaddr->addr[5] = 0x6;

  /* Do whatever else is needed to initialize interface. */
  if (!(tapif->interface = OpenTapInterface(ntohl(netif->gw.addr))))
  {
    D(bug("Unable to open tap interface\n"));
    return;
  }

  sys_thread_new(tapif_thread, netif, DEFAULT_THREAD_PRIO);
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
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  char buf[1514];
  char *bufptr;
  struct tapif *tapif;

  tapif = netif->state;
#if 0
    if(((double)rand()/(double)RAND_MAX) < 0.2) {
    printf("drop output\n");
    return ERR_OK;
    }
#endif
  /* initiate transfer(); */

  bufptr = &buf[0];

  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
    /* send data from(q->payload, q->len); */
    memcpy(bufptr, q->payload, q->len);
    bufptr += q->len;
  }

  if (WriteTapInterface(tapif->interface, buf, p->tot_len) == -1)
  {
    D(bug("Error while writing to tapif\n"));
  }
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
low_level_input(struct tapif *tapif)
{
  struct pbuf *p, *q;
  u16_t len;
  char buf[1514];
  char *bufptr;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = ReadTapInterface(tapif->interface, buf, sizeof(buf));

#if 0
    if(((double)rand()/(double)RAND_MAX) < 0.2) {
    printf("drop\n");
    return NULL;
    }
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if(p != NULL) {
    /* We iterate over the pbuf chain until we have read the entire
       packet into the pbuf. */
    bufptr = &buf[0];
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
         available data in the pbuf is given by the q->len
         variable. */
      /* read data into(q->payload, q->len); */
      memcpy(q->payload, bufptr, q->len);
      bufptr += q->len;
    }
    /* acknowledge that packet has been read(); */
  } else {
    /* drop packet(); */
  }

  return p;
}
/*-----------------------------------------------------------------------------------*/
static void
tapif_thread(void *arg)
{
    struct netif *netif;
    struct tapif *tapif;

    netif = arg;
    tapif = netif->state;

    while (1)
    {
        if (WaitTapInterface(tapif->interface))
	    tapif_input(netif);
    }
}
/*-----------------------------------------------------------------------------------*/
/*
 * tapif_output():
 *
 * This function is called by the TCP/IP stack when an IP packet
 * should be sent. It calls the function called low_level_output() to
 * do the actuall transmission of the packet.
 *
 */
/*-----------------------------------------------------------------------------------*/
static err_t
tapif_output(struct netif *netif, struct pbuf *p,
		  struct ip_addr *ipaddr)
{
  p = etharp_output(netif, ipaddr, p);
  if(p != NULL) {
    low_level_output(netif, p);
    p = NULL;
  }
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/*
 * tapif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
static void
tapif_input(struct netif *netif)
{
  struct tapif *tapif;
  struct eth_hdr *ethhdr;
  struct pbuf *p, *q;


  tapif = netif->state;

  p = low_level_input(tapif);

  if(p == NULL) {
    LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_input: low_level_input returned NULL\n"));
    return;
  }
  ethhdr = p->payload;

  LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_input: got packet with type %x\n",htons(ethhdr->type)));

  q = NULL;
  switch(htons(ethhdr->type)) {
  case ETHTYPE_IP:
    LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_input: IP packet\n"));
    q = etharp_ip_input(netif, p);
    pbuf_header(p, -14);
#if defined(LWIP_DEBUG) && defined(LWIP_TCPDUMP)
    tcpdump(p);
#endif /* LWIP_DEBUG && LWIP_TCPDUMP */
    netif->input(p, netif);
    break;
  case ETHTYPE_ARP:
    LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_input: ARP packet\n"));
    q = etharp_arp_input(netif, tapif->ethaddr, p);
    break;
  default:
    pbuf_free(p);
    break;
  }
  if(q != NULL) {
    low_level_output(netif, q);
    pbuf_free(q);
  }

}
/*-----------------------------------------------------------------------------------*/
static void
arp_timer(void *arg)
{
  etharp_tmr();
  sys_timeout(ARP_TMR_INTERVAL, (sys_timeout_handler)arp_timer, NULL);
}
/*-----------------------------------------------------------------------------------*/
/*
 * tapif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
err_t
tapif_init(struct netif *netif)
{
    struct tapif *tapif;

    if (!(TapInterfaceBase = OpenLibrary("tapinterface.library",0)))
    {
	fprintf(stderr,"Unable to open tapinterface.library\n");
	return ERR_MEM;
    }


    tapif = mem_malloc(sizeof(struct tapif));
    if (!tapif)
	return ERR_MEM;
    netif->state = tapif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    netif->output = tapif_output;
    netif->linkoutput = low_level_output;
    netif->mtu = 1500;
    /* hardware address length */
    netif->hwaddr_len = 6;

    tapif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
    low_level_init(netif);
    etharp_init();

    sys_timeout(ARP_TMR_INTERVAL, (sys_timeout_handler)arp_timer, NULL);
    return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
