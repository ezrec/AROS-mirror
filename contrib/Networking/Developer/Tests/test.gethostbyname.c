#include <exec/memory.h>
#include <exec/types.h>
#include <exec/ports.h>
#include <exec/io.h>

#include <dos/dos.h>

#include <devices/timer.h>

#include <proto/socket.h>
#include <proto/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>

struct Library *SocketBase;

#define ioctl IoctlSocket

#include <sys/param.h>
#include <sys/socket.h>

#include <fcntl.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_var.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

struct Library *SocketBase;

void main(int argc,char **argv)
{
  struct hostent *host;

  if (argc < 2) return;

  SocketBase = OpenLibrary("bsdsocket.library",0);
  if (!SocketBase) return;

  host = gethostbyname(argv[1]);

  printf("%s\n",Inet_NtoA((*(int*)host->h_addr)));

  CloseLibrary(SocketBase);
}
