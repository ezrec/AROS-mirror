#include <stdio.h>

#include "lwip/debug.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"

#include "lwip/stats.h"

#include "lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#include "lwip/tcpip.h"
#include "lwip/netif.h"

#include "netif/tcpdump.h"
#include "netif/loopif.h"

#include "lwip/sockets.h"

/* ----------------------------------------------- */

#include "lib_stuff.h"

/* ----------------------------------------------- */

#include <proto/exec.h>
#include <proto/dos.h>

/* ----------------------------------------------- */

void client_test(void *arg)
{
	  int sock = lwip_socket(0,SOCK_STREAM,0);
	  Delay(10);
	  if (sock != -1)
	  {
        struct sockaddr_in src;

        kprintf("Client: Client socket at 0x%ld\n",sock);

        src.sin_family = AF_INET;
        src.sin_addr.s_addr = htonl(0x7f000001);
        src.sin_port = htons(6000);

        kprintf("Client: connects\n");

				if (lwip_connect(sock, (struct sockaddr *) &src, sizeof(src)) != -1)
				{
            kprintf("Client: Connected to socket\n");
            lwip_write(sock,"test string",12);
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

      	sys_thread_new(client_test,NULL);

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

static void tcpip_init_done(void *arg)
{
  sys_sem_t *sem;
  sem = arg;
  sys_sem_signal(*sem);
}

void test(void)
{
}

void main(void)
{
  struct ip_addr ipaddr, netmask, gw;

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
  
    netif_add(&ipaddr, &netmask, &gw, loopif_init, tcpip_input);
  } 

  LIB_add();
  
  test();

//  server_init();
  Wait(4096);
}
