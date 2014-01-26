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
 * Author: Simon Goldschmidt
 *
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/* We will be using the 'socket' API */
#define NO_SYS                          0
#define LWIP_NETCONN                    1
#define LWIP_SOCKET                     1
#define LWIP_IPV6                       1
#define LWIP_ARP                        1
#define LWIP_ETHERNET                   1

/* Enable DHCP */
#define LWIP_DHCP                       1

#define TCP_SND_QUEUELEN                40
#define TCP_SND_BUF                     (12 * TCP_MSS)
#define TCP_WND                         (10 * TCP_MSS)

#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN
#define MEM_LIBC_MALLOC                 1

#define ARP_QUEUEING            1
#define CHAP_SUPPORT            1
#define ETHARP_SUPPORT_STATIC_ENTRIES   1
#define ETHARP_SUPPORT_VLAN     1
#define ETHARP_TRUST_IP_MAC     1
#define ETH_PAD_SIZE            2
#define IN_ADDR_T_DEFINED               1
#define IP_FORWARD              1
#define LCP_ECHOINTERVAL        10
#define LWIP_AUTOIP             1
#define LWIP_BROADCAST_PING     1
#define LWIP_COMPAT_MUTEX               1
#define LWIP_COMPAT_SOCKETS             0
#define LWIP_DHCP               1
#define LWIP_DHCP_AUTOIP_COOP   1
#define LWIP_DNS                1
#define LWIP_HAVE_LOOPIF        1
#define LWIP_HAVE_SLIPIF        1
#define LWIP_IPV6_DHCP6         1
#define LWIP_IPV6_FORWARD       1
#define LWIP_IPV6_FRAG          1
#define LWIP_MULTICAST_PING     1
#define LWIP_NETIF_HOSTNAME     1
#define LWIP_NETIF_HWADDRHINT           1
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_REMOVE_CALLBACK      1
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS      1
//#define LWIP_SO_SNDTIMEO                1     // Needs 'sys_now()'
#define LWIP_TCP_KEEPALIVE              1
//#define LWIP_TCP_TIMESTAMPS     1       // Needs 'sys_now()'
#define MD5_SUPPORT             1
#define NUM_PPP                 4
#define PAP_SUPPORT             1
//#define PPPOE_SUPPORT           1     // Broken
#define PPP_SUPPORT             1
#define SYS_LIGHTWEIGHT_PROT    1
#define TCP_LISTEN_BACKLOG      1
#define VJ_SUPPORT              1

#endif /* __LWIPOPTS_H__ */
