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
 * This File: Sebastian Bauer <sebauer@t-online.de>
 *
 * $Id: raw.h,v 1.2 2002/07/23 17:25:53 sebauer Exp $
 */
#ifndef __LWIP_RAW_H__
#define __LWIP_RAW_H__

#include "lwip/arch.h"

#include "lwip/pbuf.h"
#include "lwip/inet.h"
#include "lwip/ip.h"

#include "lwip/err.h"

struct raw_pcb {
  struct raw_pcb *next;
  int protocol;

  struct ip_addr /*local_ip, */remote_ip;
  
  void (* recv)(void *arg, struct raw_pcb *pcb, struct pbuf *p,
		struct ip_addr *addr);
  void *recv_arg;  
};

/* The following functions is the application layer interface to the
   RAW code. */
struct raw_pcb * raw_new        (void);
void             raw_remove     (struct raw_pcb *pcb);
err_t            raw_connect    (struct raw_pcb *pcb, struct ip_addr *ipaddr, u16_t protocol);
void             raw_recv       (struct raw_pcb *pcb,
				 void (* recv)(void *arg, struct raw_pcb *upcb,
					       struct pbuf *p,
					       struct ip_addr *addr),
				 void *recv_arg);
err_t            raw_send       (struct raw_pcb *pcb, struct pbuf *p);

/* The following functions is the lower layer interface to UDP. */
void             raw_input      (struct pbuf *p, struct netif *inp);
void             raw_init       (void);


#endif /* __LWIP_UDP_H__ */


