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
 * test.c - This file is part of libpktdrv
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "pktdrv.h"

int main(void)
{
  int count=0;

  if (pkt_init(0))
  {
    printf("Program halted\n");
    return 1;
  }
  printf("pkt_init: success\n");
  printf("ethernet address: %02x:%02x:%02x:%02x:%02x:%02x\n",eth_addr[0],eth_addr[1],eth_addr[2],eth_addr[3],eth_addr[4],eth_addr[5]);
  printf("pkt_dev_class: %i\n",(int)pkt_dev_class);
  while (!kbhit())
  {
    recv_buffer_t *pkt;
    eth_header_t *eth_header;

    pkt=pkt_received();
    if (pkt)
    {
      eth_header=pkt->eth_header;
      if (eth_header)
      {
        count++;
#if 1
        printf("count: %i\r",(int)count);
#else
        printf("count: %i\n",(int)count);
        printf("size: %i\n",(int)pkt->size);
        printf("to   address: %02x:%02x:%02x:%02x:%02x:%02x\n",eth_header->destination[0],eth_header->destination[1],eth_header->destination[2],eth_header->destination[3],eth_header->destination[4],eth_header->destination[5]);
        printf("from address: %02x:%02x:%02x:%02x:%02x:%02x\n",eth_header->source[0],eth_header->source[1],eth_header->source[2],eth_header->source[3],eth_header->source[4],eth_header->source[5]);
        printf("type: 0x%04x\n",eth_header->type);
#endif
      }
      free(pkt);
    }
  }
  pkt_release();
  printf("pkt_release: success\n");
  return 0;
}

