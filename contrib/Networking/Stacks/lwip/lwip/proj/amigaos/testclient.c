#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <proto/exec.h>
#include <proto/socket.h>
#include <proto/dos.h>

struct Library *SocketBase;

void main(void)
{
    if ((SocketBase = OpenLibrary("bsdsocket.library",0)))
    {
	int sock = socket(AF_INET,SOCK_STREAM,0);

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
    }
}
