#include <stdio.h>
#include <string.h>

#define MYDEBUG 1
#include "debug.h"

#include "lwip/debug.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/pbuf.h"

#include "lwip/stats.h"

#include "lwip/ip.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/icmp.h"

#include "lwip/tcpip.h"
#include "lwip/netif.h"

#include "netif/tcpdump.h"
#include "netif/loopif.h"
#include "netif/tapif.h"


#include "lwip/sockets.h"

/* ----------------------------------------------- */

#include "netif/slipif.h"

/* ----------------------------------------------- */

#include "lib_stuff.h"
#include "calling.h"

/* ----------------------------------------------- */

/* We redefine the timeval here, because lwip defines an own timeval struct,
   should be changed in the future */
#define timeval timeval_amiga

#include <proto/exec.h>
#include <proto/dos.h>

#undef timeval

/* ----------------------------------------------- */

/* Some tests when not using as bsdsocket.library */

#if 0
void client_test(void *arg)
{
	  int sock = lwip_socket(0,SOCK_STREAM,0);
	  Delay(10);
	  if (sock != -1)
	  {
        struct sockaddr_in src;

        kprintf("CLIENT: Client socket at 0x%ld\n",sock);

        src.sin_family = AF_INET;
        src.sin_addr.s_addr = htonl(0x7f000001);
        src.sin_port = htons(6000);

				kprintf("CLIENT: waiting 3 seconds\n");
				Delay(150);
        kprintf("CLIENT: connects\n");

				if (lwip_connect(sock, (struct sockaddr *) &src, sizeof(src)) != -1)
				{
            kprintf("CLIENT: Connected to socket...after waiting 3 secs\n");
            Delay(150);
            kprintf("CLIENT: sending test string\n");
            lwip_write(sock,"test string",12);
            kprintf("CLIENT: Test string written\n");
				}

				lwip_close(sock);
    }
}

void server_init(void)
{
  int sock = lwip_socket(0,SOCK_STREAM,0);
  if (sock != -1)
  {
    struct sockaddr_in src;

    src.sin_family = AF_INET;
    src.sin_addr.s_addr = htonl(INADDR_ANY);
    src.sin_port = htons(6000);

    Printf("Server: bind\n");

    if(lwip_bind(sock, (struct sockaddr *)&src, sizeof(src)) != -1)
    {
    	Printf("Server: listen\n");

      if(lwip_listen(sock, 4) != -1)
      {
      	char buf[200];
      	int s;

        int fromlen;
        struct sockaddr_in fromend;

      	Printf("Server: listen() success\n");

      	sys_thread_new(client_test,NULL,0);

        s = lwip_accept(sock,(struct sockaddr *) &fromend, &fromlen);
        if (s != -1)
        {
					Printf("Server: accept() ok\n");
					lwip_read(s, &buf, sizeof(buf));
          Printf("Server: Received %s\n",buf);
          lwip_close(s);
        } else Printf("Server: accept() failed\n");
      }
    }
  	lwip_close(sock);
  }
}
#endif

static void tcpip_init_done(void *arg)
{

  sys_sem_t *sem;
  sem = arg;

#if 0
#ifndef __AROS__
  /* No point inistialising if we arent going to use them .. */
  struct ip_addr ipaddr, netmask, gw;
  struct netif slipNetif;                   /* NicJA - TEST CODE for the new source */

  /* We must add the interface here because this is the task where the output happens and we create a message port in
   * sioslipif_input. This needs of course improvements */
  IP4_ADDR(&gw, 192,168,6,100);
  IP4_ADDR(&ipaddr, 192,168,6,1);
  IP4_ADDR(&netmask, 255,255,255,0);
  
  netif_set_default(netif_add(&slipNetif, &ipaddr, &netmask, &gw, NULL /*state*/, slipif_init,
			      tcpip_input));
#endif
#endif
  sys_sem_signal(*sem);
}

void start(void)
{
  LIB_add();

  while (1)
  {
    LONG sigs = Wait(4096);

		if (sigs & 4096) break;
  }

  LIB_remove();
}

#if 0
void raw_recv_func(void *arg, struct raw_pcb *upcb, struct pbuf *p, struct ip_addr *addr)
{
  struct ip_hdr *hdr = (struct ip_hdr*)p->payload;

  kprintf("received raw packet: protocol %ld\n",IPH_PROTO(hdr));
  
}
#endif

void main(void)
{
  struct ip_addr ipaddr, netmask, gw;
  struct netif loopNetif, tapNetif;  /* NicJA - TEST CODE for the new source */

#ifdef PERF
  perf_init("T:test.perf");
#endif /* PERF */
#ifdef STATS
  stats_init();
#endif /* STATS */
  sys_init();
  mem_init();
  memp_init();
  pbuf_init();

  tcpdump_init();

  printf("System initialized.\n");

  {
    sys_sem_t sem;

    netif_init();

    sem = sys_sem_new(0);
    tcpip_init(tcpip_init_done, &sem);
    sys_sem_wait(sem);
    sys_sem_free(sem);
    printf("TCP/IP initialized.\n");

    IP4_ADDR(&gw, 127,0,0,1);
    IP4_ADDR(&ipaddr, 127,0,0,1);
    IP4_ADDR(&netmask, 255,0,0,0);

    netif_add(&loopNetif ,&ipaddr, &netmask, &gw, NULL, loopif_init, tcpip_input);
    printf("Added loopback interface 127.0.0.1\n");

#ifdef __AROS__
    IP4_ADDR(&gw, 192,168,10,1);
    IP4_ADDR(&ipaddr, 192,168,10,2);
    IP4_ADDR(&netmask, 255,255,255,0);

    netif_set_default(netif_add(&tapNetif ,&ipaddr, &netmask, &gw, NULL, tapif_init, tcpip_input));
#endif
  }

  start();

  /* TODO: If the program would be finished it would crash */
  Wait(4096);
}
