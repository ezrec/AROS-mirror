#include "rexxsaa.h"

#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/regina.h>
#include <proto/rexxsyslib.h>

#include <exec/ports.h>
#include <intuition/intuition.h>
#include <rexx/storage.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct Library *ReginaBase;
struct Library *RexxSysBase;
struct IntuitionBase *IntuitionBase;

static void StartCommand(struct RexxMsg *);

int main(void)
{
    struct MsgPort *port;
    struct RexxMsg *msg;
    BOOL done = FALSE;
    ULONG mask, signals;
    struct EasyStruct es =
    {
	sizeof(struct EasyStruct),
	0,
	"RexxMast error",
	NULL,
	"OK"
    };
    
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
    ReginaBase = OpenLibrary("regina.library", 2);
    if (ReginaBase == NULL)
    {
	es.es_TextFormat = "Error opening regina.library V2";
	EasyRequest(NULL, &es, NULL);
	return 20;
    }
    RexxSysBase = OpenLibrary("rexxsyslib.library", 44);
    if (RexxSysBase == NULL)
    {
	es.es_TextFormat = "Error opening rexxsyslib.library V44";
	EasyRequest(NULL, &es, NULL);
	CloseLibrary(ReginaBase);
	return 20;
    }

    port = CreatePort("REXX", 0);
    es.es_Title = "RexxMast message";
    mask = SIGBREAKF_CTRL_C | (1<<port->mp_SigBit);
    do
    {
	signals = Wait(mask);
	if (signals & SIGBREAKF_CTRL_C)
	    done = TRUE;
	if (signals & (1<<port->mp_SigBit))
	{
	    while ((msg = (struct RexxMsg *)GetMsg(port)) != NULL)
	    {
		if (!IsRexxMsg(msg))
		{
		    es.es_TextFormat = "Received message that is not a Rexx message";
		    EasyRequest(NULL, &es, NULL);
		}
		else
		{
		    static UBYTE *text[] =
		    { "RXCOMM", "RXFUNC", "RXCLOSE", "RXQUERY", "UNKNOWN1",
			    "UNKNOWN2", "RXADDFH", "RXADDLIB", "RXREMLIB", 
			    "RXADDCON", "RXREMCON", "RXTCOPN", "RXTCCLS",
			    "TOO HIGH"
		    };
		    LONG action = msg->rm_Action & RXCODEMASK;
		    
		    switch (action)
		    {
		    case RXCOMM:
			StartCommand(msg);
			break;
			
		    case RXCLOSE:
			done = TRUE;
		    default:
			es.es_TextFormat = "Rexx message with command \"%s\" received";
			if (action <= RXTCCLS)
			    EasyRequest(NULL, &es, NULL, text[(action >> 24) - 1]);
			else
			    EasyRequest(NULL, &es, NULL, text[(RXTCCLS >> 24)]);
			break;
		    }
		}
		ReplyMsg(msg);
	    }
	}
    } while(!done);

    DeletePort(port);
    CloseLibrary(RexxSysBase);
    CloseLibrary(ReginaBase);
    CloseLibrary((struct Library *)IntuitionBase);

    return 0;
}

void StartCommand(struct RexxMsg *msg)
{
    UBYTE *s, *filename;
    RXSTRING rxarg, rxresult;
    USHORT rc;
    BPTR lock;
    unsigned int len=0;
    
    s = msg->rm_Args[0];
    while (*s != 0 && !isspace(*s)) s++;
    len = s - msg->rm_Args[0];
    filename = malloc(len+6+(msg->rm_FileExt==NULL ? 5 : strlen(msg->rm_FileExt)));
    memcpy(filename, msg->rm_Args[0], len);
    filename[len] = 0;
    if (strchr(filename, ':') == NULL)
    {
	strcpy(filename, "REXX:");
	strncat(filename, msg->rm_Args[0], len);
    }
    lock = Lock(filename, ACCESS_READ);
    if (lock == NULL)
    {
	strcat(filename, msg->rm_FileExt==NULL ? ".rexx" : msg->rm_FileExt);
	lock = Lock(filename, ACCESS_READ);
    }
    if (lock == NULL)
    {
	msg->rm_Result1 = 5;
	msg->rm_Result2 = 1;
	free(filename);
	return;
    }
    UnLock(lock);
    
    len = LengthArgstring(msg->rm_Args[0]) - (s - msg->rm_Args[0]);
    if (len>0)
    {
	MAKERXSTRING(rxarg, s, len);
	RexxStart(1, &rxarg, filename, NULL, msg->rm_CommAddr, RXCOMMAND, NULL, &rc, &rxresult);
    }
    else
    {
	RexxStart(0, NULL, filename, NULL, msg->rm_CommAddr, RXCOMMAND, NULL, &rc, &rxresult);
    }
    fflush(stdout);
    msg->rm_Result1 = rc;
    
    free(filename);
}
