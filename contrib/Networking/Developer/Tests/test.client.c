#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>

#include <proto/exec.h>
#include <proto/socket.h>
#include <proto/dos.h>

static const char version[] = "$VER: test.client 0.5 (21.12.2004)";

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

  if (!(rdargs = ReadArgs("HOST/A",(LONG*)&cli_args,NULL))) host = gethostbyname("localhost");
  else host = gethostbyname(cli_args.host);

  if (!(host))
  {
    Printf("Test.Client : Unknown Host\n");
    return 20;
  }
  else
  {
  src.sin_len = host->h_length;
  memcpy(&src.sin_addr, host->h_addr_list[0], host->h_length);
  }

  int sock = socket(AF_INET,SOCK_STREAM,0);

  if (sock != -1)
  {
      Printf("Client: Client socket at 0x%ld\n",sock);

      src.sin_family = AF_INET;
      src.sin_port = htons(6000);

      Printf("Client: connecting\n");

      if (connect(sock, (struct sockaddr *) &src, sizeof(src)) != -1)
      {
          Printf("Client: Connected to socket\nClient: Now sending a string\n");
          send(sock,"test string",12,0);
          Printf("Client: Sent a string\n");
      } else
      {
          Printf("Client: Connecting failed\n");
      }
      Printf("Client: Close socket\n");

      CloseSocket(sock);
  }
  CloseLibrary(SocketBase);
      
  FreeArgs(rdargs);
  return 0;
}
