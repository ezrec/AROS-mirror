#include <stdio.h>
#include <string.h>

#define MYDEBUG
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

#include "lwip/sockets.h"

/* ----------------------------------------------- */

#include "netif/sioslipif.h"

/* ----------------------------------------------- */

#include "lib_stuff.h"
#include "calling.h"

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
  struct ip_addr ipaddr, netmask, gw;

  sys_sem_t *sem;
  sem = arg;

  /* We must add the interface here because this is the task where the output happens and we create a message port in
   * sioslipif_input. This needs of course improvements */
  IP4_ADDR(&gw, 192,168,6,100);
  IP4_ADDR(&ipaddr, 192,168,6,1);
  IP4_ADDR(&netmask, 255,255,255,0);
  
  netif_set_default(netif_add(&ipaddr, &netmask, &gw, sioslipif_init,
			      tcpip_input));

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

void raw_recv_func(void *arg, struct raw_pcb *upcb, struct pbuf *p, struct ip_addr *addr)
{
  struct ip_hdr *hdr = (struct ip_hdr*)p->payload;

  kprintf("received raw packet: protocol %ld\n",IPH_PROTO(hdr));
  
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

    {
      struct icmp_echo_hdr echo;
      struct pbuf *p;
      int s = lwip_socket(0,SOCK_RAW,1);
      struct sockaddr_in to;
      struct sockaddr_in from;
      int buf[100];
      int from_len = sizeof(from);

      to.sin_len =  sizeof(to);
      to.sin_family = AF_INET;
      to.sin_port = 1; /* protocol */
      to.sin_addr.s_addr = (193 << 24)|(168 << 16)|(6<<8)|100;
      memset(&to.sin_zero,0,sizeof(to));

      ICMPH_TYPE_SET(&echo, ICMP_ECHO);
      ICMPH_CODE_SET(&echo,0);
      echo.chksum = 0;
      echo.seqno = 100;
      echo.id = 100;			/* ID */

      lwip_sendto(s,&echo,sizeof(echo),0,&to,sizeof(to));
      printf("sent some stuff\n");

      lwip_recvfrom(s,buf,sizeof(buf),0,&from,&from_len);
      printf("received something from 0x%lx: 0x%x 0x%x 0x%x 0x%x 0x%x\n",from.sin_addr.s_addr,buf[0],buf[1],buf[2],buf[3],buf[4]);

//      lwip_recvfrom(s,buf,sizeof(buf),0,&from,&from_len);
//      printf("received something from 0x%lx: 0x%x 0x%x 0x%x 0x%x 0x%x\n",from.sin_addr.s_addr,buf[0],buf[1],buf[2],buf[3],buf[4]);
    }
  } 

  start();

  /* TODO: If the program would be finished it would crash */
  Wait(4069);
}
