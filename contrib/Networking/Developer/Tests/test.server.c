#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>

#include <proto/exec.h>
#include <proto/socket.h>
#include <proto/dos.h>

static const char version[] = "$VER: test.server 0.5 (21.12.2004)";

struct Library *SocketBase;

int main(void)
{
  struct hostent *host;
  struct sockaddr_in src;

  struct RDArgs *rdargs;
  struct
  {
    STRPTR host;
  } cli_args;

  memset(&cli_args,0,sizeof(cli_args));

  if (!(SocketBase = OpenLibrary("bsdsocket.library",0)))
  {
    Printf("Couldn't open bsdsocket.library!\n");
    return 20;
  }

  if (!(rdargs = ReadArgs("HOST/A",(LONG*)&cli_args,NULL)))
  {
    src.sin_addr.s_addr = htonl(INADDR_ANY);
  }
  else
  {
    if (host = gethostbyname(cli_args.host))
    {
    src.sin_len = sizeof(struct sockaddr_in);
    memcpy(&src.sin_addr, host->h_addr_list[0], host->h_length);
    }
    else
    {
      Printf("Test.Server : Unknown Host\n");
      return 20;
    }
  }

  int sock = socket(AF_INET,SOCK_STREAM,0);
  if (sock != -1)
  {
     Printf("Server: Server socket at 0x%ld\n",sock);

     src.sin_family = AF_INET;
     src.sin_port = htons(6000);

     Printf("Server: bind\n");

     if (bind(sock, (struct sockaddr *)&src, sizeof(src)) != -1)
     {
          Printf("Server: listen\n");

          if(listen(sock, 4) != -1)
          {
              char buf[200];
              int s;

              long fromlen;
              struct sockaddr_in fromend;

              Printf("Server: listen() success\n");
              Printf("Server: waiting for a connection\n");

              s = accept(sock,(struct sockaddr *) &fromend, &fromlen);
              if (s != -1)
              {
                  Printf("Server: accept() ok\n");
                  recv(s, buf, sizeof(buf),0);
                  Printf("Server: Received %s\n",buf);
                  CloseSocket(s);
              } else Printf("Server: accept() failed\n");
          }
      }
      CloseSocket(sock);
  }
  CloseLibrary(SocketBase);
      
  FreeArgs(rdargs);
  return 0;
}
