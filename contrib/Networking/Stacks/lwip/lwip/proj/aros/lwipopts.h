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
 * $Id$
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/* ---------- Memory options ---------- */
#define MEM_ALIGNMENT           4

#define MEM_SIZE                64000
//#define MEM_SIZE                1500

#define MEM_RECLAIM             1
#define MEMP_RECLAIM            1

#define MEMP_NUM_PBUF           1600 
#define MEMP_NUM_UDP_PCB        10
#define MEMP_NUM_TCP_PCB        10
#define MEMP_NUM_TCP_PCB_LISTEN 10
#define MEMP_NUM_TCP_SEG        160
#define MEMP_NUM_NETBUF         200 
#define MEMP_NUM_NETCONN        200 
#define MEMP_NUM_API_MSG        200
#define MEMP_NUM_TCPIP_MSG      200

#define MEMP_NUM_SYS_TIMEOUT    20

/* ---------- Pbuf options ---------- */

#define PBUF_POOL_SIZE          128
#define PBUF_POOL_BUFSIZE       512

#define PBUF_LINK_HLEN          16

/* ---------- TCP options ---------- */
#define TCP_TTL                 255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 1024

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             2048

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN        2 * TCP_SND_BUF/TCP_MSS

/* TCP receive window. */
#define TCP_WND                 2048

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           4


/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD              0

/* If defined to 1, IP options are allowed (but not parsed). If
   defined to 0, all packets with IP options are dropped. */
#define IP_OPTIONS              1

/* ---------- ICMP options ---------- */
#define ICMP_TTL                255


/* ---------- UDP options ---------- */
#define UDP_TTL                 255


/* ---------- Statistics options ---------- */
#define STATS

#ifdef STATS
#define LINK_STATS
#define IP_STATS
#define ICMP_STATS
#define UDP_STATS
#define TCP_STATS
#define MEM_STATS
#define MEMP_STATS
#define PBUF_STATS
#define SYS_STATS
#endif /* STATS */

#endif /* __LWIPOPTS_H__ */
