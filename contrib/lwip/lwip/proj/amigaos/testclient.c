#include <sys/types.h>
#include <sys/socket.h>

#include <proto/exec.h>
#include <proto/socket.h>

struct Library *SocketBase;

void main(void)
{
    if ((SocketBase = OpenLibrary("bsdsocket.library",0)))
    {
	int sock = socket(0,SOCK_STREAM,0);

	if (sock != -1)
	{
	    struct sockaddr_in src;
	    Printf("Client: Client socket at 0x%ld\n",sock);

	    src.sin_family = AF_INET;
	    src.sin_addr.s_addr = htonl(0x7f000001);
	    src.sin_port = htons(6000);

	    Printf("Client: connecting\n");

	    if (connect(sock, (struct sockaddr *) &src, sizeof(src)) != -1)
	    {
		Printf("Client: Connected to socket\n");
		send(sock,"test string",12,0);
	    }

	    CloseSocket(sock);
	}
	CloseLibrary(SocketBase);
    }
}
