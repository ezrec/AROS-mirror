#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/rexxsyslib.h>
#include <stdio.h>
#include <string.h>

#include <exec/ports.h>
#include <rexx/storage.h>

struct Library *RexxSysBase;

int main(void)
{
    struct RexxMsg *msg, *reply;
    struct MsgPort *port, *replyport;
    const char *command = "rexxdir een test  ";
    
    RexxSysBase = OpenLibrary("rexxsyslib.library", 44);
    if (RexxSysBase == NULL)
    {
	puts("Error opening rexxsyslib.library");
	return 20;
    }
    
    port = FindPort("REXX");
    if (port == NULL)
    {
	puts("REXX port not found");
	CloseLibrary(RexxSysBase);
	return 20;
    }

    replyport = CreatePort(NULL, 0);
    if (replyport == NULL)
    {
	puts("Could not create replyport");
	CloseLibrary(RexxSysBase);
	return 20;
    }
    
    msg = CreateRexxMsg(replyport, ".rexx", NULL);
    if (msg == NULL)
    {
	puts("Error creating RexxMsg");
	DeletePort(replyport);
	CloseLibrary(RexxSysBase);
	return 20;
    }
    msg->rm_Action = RXCOMM | RXFF_RESULT;
    msg->rm_Args[0] = CreateArgstring(command, strlen(command));
    PutMsg(port, (struct Message *)msg);
    reply = (struct RexxMsg *)WaitPort(replyport);
    printf("Result1: %d\n", reply->rm_Result1);
    if (reply->rm_Result1==0 && reply->rm_Result2!=NULL)
    {
	printf("Result2: %s\n", reply->rm_Result2);
	DeleteArgstring(reply->rm_Result2);
    }
    DeleteArgstring(msg->rm_Args[0]);
    DeleteRexxMsg(msg);
    DeletePort(replyport);
    CloseLibrary(RexxSysBase);

    if (reply != msg)
    {
	puts("Returned message is different then sent message");
	return 20;
    }
    
    puts("All OK");
    return 0;
}
