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
 * Changes by Sebastian Bauer <sebauer@t-online.de> 
 *
 * $Id$
 */

#include "lwip/debug.h"
#include "lwip/def.h"
#include "netif/sio.h" 
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#define MYDEBUG 1
#include "debug.h"

#include <devices/serial.h>
#include <proto/exec.h>
#include <proto/dos.h>

/* Resources */
static struct MsgPort *Serial_ReadPort;
static struct IOExtSer *Serial_ReadIO;
static struct MsgPort *Serial_WritePort;
static struct IOExtSer *Serial_WriteIO;

/** array of ((siostruct*)netif->state)->sio structs */
static sio_status_t statusar[2];

/* --public-functions----------------------------------------------------------------------------- */
void sio_send( u8_t c, sio_status_t * siostat )
{
    char tosend = c;
    Serial_WriteIO->IOSer.io_Command  = CMD_WRITE;
    Serial_WriteIO->IOSer.io_Length   = 1;
    Serial_WriteIO->IOSer.io_Data     = &tosend;
    DoIO((struct IORequest*)Serial_WriteIO);
}

u8_t sio_recv( sio_status_t * siostat )
{
	u8_t toget;

  if (!(Serial_ReadPort = CreateMsgPort())) return 0;
  if (!(Serial_ReadIO = (struct IOExtSer*)CreateIORequest(Serial_ReadPort,sizeof(struct IOExtSer))))
  {
    DeleteMsgPort(Serial_ReadPort);
    return 0;
  }
  *Serial_ReadIO = *Serial_WriteIO;
  Serial_ReadIO->IOSer.io_Message.mn_ReplyPort = Serial_ReadPort;
	Serial_ReadIO->IOSer.io_Command  = CMD_READ;
	Serial_ReadIO->IOSer.io_Length   = 1;
	Serial_ReadIO->IOSer.io_Data     = &toget;
	DoIO((struct IORequest*)Serial_ReadIO);

	return toget;
}

sio_status_t * sio_open( int devnum )
{
  if (!(Serial_WritePort = CreateMsgPort())) return NULL;
  if (!(Serial_WriteIO = (struct IOExtSer*)CreateIORequest(Serial_WritePort,sizeof(struct IOExtSer))))
  {
    DeleteMsgPort(Serial_WritePort);
    return NULL;
  }

  if (OpenDevice(SERIALNAME,0,(struct IORequest *)Serial_WriteIO,0L))
  {
    D(bug("Opening of " SERIALNAME " failed\n"));
    DeleteIORequest(Serial_WriteIO);
    DeleteMsgPort(Serial_WritePort);
    return NULL;
  }

  D(bug("Opening of " SERIALNAME " succeded being in Task at 0x%lx\n",FindTask(NULL)));

  Serial_WriteIO->IOSer.io_Command  = SDCMD_SETPARAMS;
  Serial_WriteIO->io_SerFlags      &= ~SERF_PARTY_ON;
  Serial_WriteIO->io_SerFlags      |= SERF_XDISABLED;
  Serial_WriteIO->io_Baud           = 57600;

  if (DoIO((struct IORequest*)Serial_WriteIO))
    D(bug("Error setting parameters!\n"));

	return &statusar[0];
}

#if 0

/* Resources */

static struct MsgPort *Serial_ReadPort;
static struct IOExtSer *Serial_ReadIO;

static struct MsgPort *Serial_WritePort;
static struct IOExtSer *Serial_WriteIO;

/* The maximum size that an incoming packet can have. */
#define MAX_SIZE     1500

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

/* Define those to whatever is needed to send and receive one byte of
   data. */
#define SIO_SEND(c)  \
 { \
 	char tosend = c;\
    Serial_WriteIO->IOSer.io_Command  = CMD_WRITE;\
    Serial_WriteIO->IOSer.io_Length   = 1;\
    Serial_WriteIO->IOSer.io_Data     = &tosend;\
    DoIO((struct IORequest*)Serial_WriteIO);\
 }

#define SIO_RECV(c) \
 { \
 	char toget;\
    Serial_ReadIO->IOSer.io_Command  = CMD_READ;\
    Serial_ReadIO->IOSer.io_Length   = 1;\
    Serial_ReadIO->IOSer.io_Data     = &toget;\
    DoIO((struct IORequest*)Serial_ReadIO);\
    c = toget;\
 }

static const unsigned char slip_end = SLIP_END, 
                           slip_esc = SLIP_ESC, 
                           slip_esc_end = SLIP_ESC_END, 
                           slip_esc_esc = SLIP_ESC_ESC;

/*-----------------------------------------------------------------------------------*/
static err_t
sioslipif_output(struct netif *netif, struct pbuf *p, struct ip_addr *ipaddr)
{
  struct pbuf *q;
  int i;
  unsigned char *ptr;
  u8_t c;

  D(bug("Outputting stuff while being in Task at 0x%lx\n",FindTask(NULL)));

  /* Send pbuf out on the serial I/O device. */
  SIO_SEND(slip_end);
  
  for(q = p; q != NULL; q = q->next) {
    ptr = q->payload;
    for(i = 0; i < q->len; i++) {
      c = *ptr++;
      switch(c) {
      case SLIP_END:
        SIO_SEND(slip_esc);
        SIO_SEND(slip_esc_end);
        break;
      case SLIP_ESC:
        SIO_SEND(slip_esc);
        SIO_SEND(slip_esc_esc);
        break;
      default:
        SIO_SEND(c);
        break;
      }
    }
  }
#ifdef LINK_STATS
  lwip_stats.link.xmit++;
#endif /* LINK_STATS */  
  SIO_SEND(slip_end);

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static struct pbuf *
sioslipif_input(void)
{
  u8_t c;
  struct pbuf *p, *q;
  int recved;
  int i;
  
  q = p = NULL;
  recved = i = 0;
  c = 0;
  
  while(1) {
    SIO_RECV(c);
    switch(c) {
    case SLIP_END:
      if(p == NULL) {
        return sioslipif_input();
      }
      if(recved > 0) {
        /* Received whole packet. */
        pbuf_realloc(q, recved);
#ifdef LINK_STATS
        lwip_stats.link.recv++;
#endif /* LINK_STATS */         
        return q;
      }
      break;
    case SLIP_ESC:
      SIO_RECV(c);
      switch(c) {
      case SLIP_ESC_END:
        c = SLIP_END;
        break;
      case SLIP_ESC_ESC:
        c = SLIP_ESC;
        break;
      }
      /* FALLTHROUGH */
    default:
      if(p == NULL) {      
        p = pbuf_alloc(PBUF_LINK, 128, PBUF_POOL);
#ifdef LINK_STATS           
        if(p == NULL) {
          lwip_stats.link.drop++;
        }
#endif /* LINK_STATS */                  
        if(q != NULL) {
          pbuf_chain(q, p);
        } else {
          q = p;
        }
      }
      if(p != NULL && recved < MAX_SIZE) {
        ((u8_t *)p->payload)[i] = c;
        recved++;
        i++;
        if(i >= p->len) {
          i = 0;
          p = NULL;
        }
      }
      break;
    }
    
  }
  return NULL;
}
/*-----------------------------------------------------------------------------------*/
static void sioslipif_loop(void *arg)
{
  struct pbuf *p;
  struct netif *netif;

  netif = arg;

  if (!(Serial_ReadPort = CreateMsgPort())) return;
  if (!(Serial_ReadIO = (struct IOExtSer*)CreateIORequest(Serial_ReadPort,sizeof(struct IOExtSer))))
  {
    DeleteMsgPort(Serial_ReadPort);
    return;
  }

  *Serial_ReadIO = *Serial_WriteIO;
  Serial_ReadIO->IOSer.io_Message.mn_ReplyPort = Serial_ReadPort;

  D(bug("Now waiting for serial input\n"));

  while(1)
  {
    p = sioslipif_input();    
    D(bug("Received a packet\n"));
    netif->input(p, netif);
  }
}
/*-----------------------------------------------------------------------------------*/

void sioslipif_init(struct netif *netif)
{
  netif->state = NULL;
  netif->name[0] = 's';
  netif->name[1] = 'l';
  netif->output = sioslipif_output;

  if (!(Serial_WritePort = CreateMsgPort())) return;
  if (!(Serial_WriteIO = (struct IOExtSer*)CreateIORequest(Serial_WritePort,sizeof(struct IOExtSer))))
  {
    DeleteMsgPort(Serial_WritePort);
    return;
  }

  if (OpenDevice(SERIALNAME,0,(struct IORequest *)Serial_WriteIO,0L))
  {
    D(bug("Opening of " SERIALNAME " failed\n"));
    DeleteIORequest(Serial_WriteIO);
    DeleteMsgPort(Serial_WritePort);
    return;
  }

  D(bug("Opening of " SERIALNAME " succeded being in Task at 0x%lx\n",FindTask(NULL)));

  Serial_WriteIO->IOSer.io_Command  = SDCMD_SETPARAMS;
  Serial_WriteIO->io_SerFlags      &= ~SERF_PARTY_ON;
  Serial_WriteIO->io_SerFlags      |= SERF_XDISABLED;
  Serial_WriteIO->io_Baud           = 57600;

  if (DoIO((struct IORequest*)Serial_WriteIO))
    D(bug("Error setting parameters!\n"));

  sys_thread_new((void *)sioslipif_loop, netif);

  /* Some delay because Serial_WriteIO is used in sioslipif_loop() - this way of doing this is really hacky */

  Delay(5);
}
/*-----------------------------------------------------------------------------------*/

#endif
