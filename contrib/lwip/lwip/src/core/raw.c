/*
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
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * This file: Sebastian Bauer <sebauer@t-online.de>
 *
 * $Id$
 */

/*-----------------------------------------------------------------------------------*/
/* udp.c
 *
 * The code for the RAW stuff
 *
 */
/*-----------------------------------------------------------------------------------*/
#include "lwip/debug.h"

#include "lwip/def.h"
#include "lwip/memp.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"

#include "lwip/stats.h"

#include "arch/perf.h"

/*-----------------------------------------------------------------------------------*/

/* The list of RAW PCBs. */
static struct raw_pcb *raw_pcbs = NULL;

/*-----------------------------------------------------------------------------------*/
void
raw_init(void)
{
}
/*-----------------------------------------------------------------------------------*/
/* This function will not free the pbuf */
void
raw_input(struct pbuf *p, struct netif *inp)
{
  struct raw_pcb *pcb;
  struct ip_hdr *iphdr;
  int protocol;
  
  PERF_START;

  iphdr = p->payload;

#ifdef IPv6
  protocol = iphdr->nexthdr;
#else
  protocol = IPH_PROTO(iphdr);
#endif /* IPv4 */

  /* Demultiplex packet */
  for (pcb = raw_pcbs; pcb != NULL; pcb = pcb->next) {
    if (pcb->protocol == protocol)
    {
      if (pcb->recv)
      {
	pcb->recv(pcb->recv_arg, pcb, p, &(iphdr->src));
      }
    }
  }

  if (!pcb) goto end;

  end:
    
  PERF_STOP("raw_input");
}
/*-----------------------------------------------------------------------------------*/
err_t
raw_send(struct raw_pcb *pcb, struct pbuf *p)
{
  struct netif *netif;
  struct ip_addr *src_ip;
  err_t err;
  struct pbuf *q;
  
  if((netif = ip_route(&(pcb->remote_ip))) == NULL) {
    DEBUGF(RAW_DEBUG, ("raw_send: No route to 0x%lx\n", pcb->remote_ip.addr));
#if 0
#ifdef UDP_STATS
    ++stats.udp.rterr;
#endif /* UDP_STATS */
#endif
    return ERR_RTE;
  }

  src_ip = &(netif->ip_addr);
  
  DEBUGF(RAW_DEBUG, ("raw_send: sending datagram of length %d\n", p->tot_len));
  
  err = ip_output_if(p, src_ip, &pcb->remote_ip, 255 /*UDP_TTL*/, pcb->protocol, netif);

#if 0
#ifdef UDP_STATS
  ++stats.udp.xmit;
#endif /* UDP_STATS */
#endif
  return err;
}
/*-----------------------------------------------------------------------------------*/
#if 0
err_t
udp_bind(struct udp_pcb *pcb, struct ip_addr *ipaddr, u16_t port)
{
  struct udp_pcb *ipcb;
  ip_addr_set(&pcb->local_ip, ipaddr);
  pcb->local_port = port;

  /* Insert UDP PCB into the list of active UDP PCBs. */
  for(ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next) {
    if(pcb == ipcb) {
      /* Already on the list, just return. */
      return ERR_OK;
    }
  }
  /* We need to place the PCB on the list. */
  pcb->next = udp_pcbs;
  udp_pcbs = pcb;

  DEBUGF(UDP_DEBUG, ("udp_bind: bound to port %d\n", port));
  return ERR_OK;
}
#endif
/*-----------------------------------------------------------------------------------*/
err_t
raw_connect(struct raw_pcb *pcb, struct ip_addr *ipaddr)
{
  struct raw_pcb *ipcb;
  ip_addr_set(&pcb->remote_ip, ipaddr);

  /* Insert RAW PCB into the list of active RAW PCBs. */
  for(ipcb = raw_pcbs; ipcb != NULL; ipcb = ipcb->next) {
    if(pcb == ipcb) {
      /* Already on the list, just return. */
      return ERR_OK;
    }
  }
  /* We need to place the PCB on the list. */
  pcb->next = raw_pcbs;
  raw_pcbs = pcb;
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
void
raw_recv(struct raw_pcb *pcb,
	 void (* recv)(void *arg, struct raw_pcb *upcb, struct pbuf *p,
		       struct ip_addr *addr),
	 void *recv_arg)
{
  pcb->recv = recv;
  pcb->recv_arg = recv_arg;
}
/*-----------------------------------------------------------------------------------*/
void
raw_remove(struct raw_pcb *pcb)
{
  struct raw_pcb *pcb2;
  
  if(raw_pcbs == pcb) {
    raw_pcbs = raw_pcbs->next;
  } else for(pcb2 = raw_pcbs; pcb2 != NULL; pcb2 = pcb2->next) {
    if(pcb2->next != NULL && pcb2->next == pcb) {
      pcb2->next = pcb->next;
    }
  }

  memp_free(MEMP_RAW_PCB, pcb);  
}
/*-----------------------------------------------------------------------------------*/
struct raw_pcb *
raw_new(int protocol) {
  struct raw_pcb *pcb;
  pcb = memp_malloc(MEMP_RAW_PCB);
  if(pcb != NULL) {
    bzero(pcb, sizeof(struct raw_pcb));
    pcb->protocol = protocol;
    return pcb;
  }
  return NULL;

}
/*-----------------------------------------------------------------------------------*/


