#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/rexxsyslib.h>
#include <stdio.h>

#include <exec/ports.h>
#include <rexx/storage.h>

struct RxsLib *RexxSysBase;

int main(void)
{
    struct RexxMsg *msg;
    struct MsgPort *port;

    RexxSysBase = (struct RxsLib *)OpenLibrary("rexxsyslib.library", 44);
    if (RexxSysBase == NULL)
    {
	puts("Error opening rexxsyslib.library");
	return 20;
    }
    
    port = CreatePort("TEST", 0);
    if (port == NULL)
    {
	puts("Error creating TEST port");
	CloseLibrary((struct Library *)RexxSysBase);
	return 20;
    }
    
    msg = (struct RexxMsg *)WaitPort(port);
    if (msg == NULL)
    {
	puts("Received NULL message");
    }
    else if (!IsRexxMsg(msg))
    {
	puts("Received message that is not a RexxMsg");
    }
    else
    {
	puts("Received RexxMsg");
	printf("%08lx\n", msg->rm_Action);
	Write(msg->rm_Stdin, "Hello\n", 6);
	puts((STRPTR)msg->rm_Args[0]);
    }
    ReplyMsg((struct Message *)msg);
    DeletePort(port);
    CloseLibrary((struct Library *)RexxSysBase);

    return 0;
}
