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
 * pktdrv.h - This file is part of libpktdrv
 *
 */

#ifndef __PKTDRV_H__
#define __PKTDRV_H__

#define RBUFSIZE   1514 /* receive buffer size */
#define SBUFSIZE   1514 /* send buffer size */

#define PD_ETHER 1
#define PD_SLIP  6

typedef unsigned char eth_address_t[6];

typedef struct {
  eth_address_t destination __attribute__ ((packed));
  eth_address_t source __attribute__ ((packed));
  unsigned short type __attribute__ ((packed));
} eth_header_t;

typedef struct {
  int size;
  eth_header_t *eth_header;
  void *data;
  unsigned char buffer[0];
} recv_buffer_t;

typedef struct {
  int size;
  int retries;
  eth_header_t *eth_header;
  void *data;
} send_buffer_t;

#undef pack

extern unsigned short pkt_dev_class;
extern eth_address_t eth_addr;

int pkt_init(int pkt_intr_arg);
int pkt_release();
int pkt_send(send_buffer_t *buffer);
int pkt_get_current_length();
void pkt_drop_current();
int pkt_read_current(unsigned char *buffer, unsigned int start, unsigned int length);
recv_buffer_t *pkt_received();

#endif /* __PKTDRV_H__ */
