
#include <sys/types.h>
#include <sys/socket.h>

#include <proto/exec.h>
#include <proto/socket.h>
#include <proto/dos.h>

struct Library *SocketBase;

void main(void)
{
    if ((SocketBase = OpenLibrary("bsdsocket.library",0)))
    {
	int sock = socket(0,SOCK_STREAM,0);
	if (sock != -1)
	{
	   struct sockaddr_in src;

	   src.sin_family = AF_INET;
	   src.sin_addr.s_addr = htonl(INADDR_ANY);
	   src.sin_port = htons(6000);

	   Printf("Server: bind\n");

	   if (bind(sock, (struct sockaddr *)&src, sizeof(src)) != -1)
	   {
		Printf("Server: listen\n");

		if(listen(sock, 4) != -1)
		{
		    char buf[200];
		    int s;

		    int fromlen;
		    struct sockaddr_in fromend;

		    Printf("Server: listen() success\n");

		    s = accept(sock,(struct sockaddr *) &fromend, &fromlen);
		    if (s != -1)
		    {
			Printf("Server: accept() ok\n");
			recv(s, &buf, sizeof(buf),0);
		        Printf("Server: Received %s\n",buf);
		        CloseSocket(s);
		    } else Printf("Server: accept() failed\n");
		}
	    }
	    CloseSocket(sock);
	}
	CloseLibrary(SocketBase);
    }
}
