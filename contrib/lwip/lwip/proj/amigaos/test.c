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

#include "netif/tcpdump.h"
#include "netif/loopif.h"

#include "lwip/sockets.h"

void server_init(void)
{
  int sock = lwip_socket(0,SOCK_STREAM,0);
  if (sock != -1)
  {
    struct sockaddr_in src;

    src.sin_family = AF_INET;
    src.sin_addr.s_addr = htonl(INADDR_ANY);
    src.sin_port = htons(6000);

    if(lwip_bind(sock, (struct sockaddr *)&src, sizeof(src)) != -1)
    {
      if(lwip_listen(sock, 10) != -1)
      {
      	printf("listen() success\n");
      }
    }
  	lwip_close(sock);
  }
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

  IP4_ADDR(&gw, 127,0,0,1);
  IP4_ADDR(&ipaddr, 127,0,0,1);
  IP4_ADDR(&netmask, 255,0,0,0);
  
  netif_add(&ipaddr, &netmask, &gw, loopif_init, tcpip_input);

  printf("System initialized.\n");

  server_init();
}
