#include <exec/exec.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>

#define DEBUG 0
#include <aros/debug.h>

#include "d_net.h"
#include "doomstat.h"
#include "m_argv.h"
#include "i_system.h"

#include "aros_netspecial.h"
#include "aros_net_local.h"

/****************************************************************************/

arosnetdriver_t local_netdriver =
{
    "Local",
    "-netlocal",
    AROSNetLocal_InitNetwork,
    AROSNetLocal_CleanupNetwork,
    AROSNetLocal_NetCmd
};

/****************************************************************************/

static struct ComPort 	*comport;
static struct ComMsg 	sendmsg, *getmsg;
static struct ComMsg 	fastmsg[NUM_FASTMSG];
static struct MsgPort 	fastmsg_freeport;
static struct MsgPort 	*sendport[MAX_PLAYERS];
static WORD 		playerid_to_node[MAX_PLAYERS];
static WORD 		playerid;

/****************************************************************************/

boolean AROSNetLocal_InitNetwork(void)
{
    int i,i2,i3;
    int netgame=0;

    i = M_CheckParm ("-netlocal");

    netgame = 1;

    // parse player number and host list
    doomcom->consoleplayer = consoleplayer = playerid = myargv[i + 1][0] - '1';

    doomcom->numnodes = 1; // this node for sure

    i++;
    while (++i < myargc && myargv[i][0] != '-')
    {
        doomcom->numnodes++;
    }

    doomcom->id = DOOMCOM_ID;
    doomcom->numplayers = doomcom->numnodes;

    Forbid();
    if (!(comport = (struct ComPort *)FindSemaphore(COMNAME)))
    {
	comport = AllocVec(sizeof(struct ComPort), MEMF_PUBLIC | MEMF_CLEAR);
	if (!comport)
	{
	    Permit();
	    I_Error("AROS_Net_Local: Out of memory!");
	}

	InitSemaphore(&comport->sem);
	comport->sem.ss_Link.ln_Name = COMNAME;
	comport->sem.ss_Link.ln_Pri = -128;

	comport->pool = CreatePool(MEMF_PUBLIC, sizeof(struct ComMsg) * 20,
						sizeof(struct ComMsg) * 20);
	if (!comport->pool)
	{
	    Permit();
	    FreeVec(comport);comport=0;

	    I_Error("AROS_Net_Local: Out of memory!");
	}

	AddSemaphore(&comport->sem);

	for(i = 0;i < MAX_PLAYERS; i++)
	{
	    comport->playerport[i].msgport.mp_Node.ln_Type = NT_MSGPORT;
	    comport->playerport[i].msgport.mp_Flags        = PA_IGNORE;
	    comport->playerport[i].msgport.mp_SigBit       = 0;
	    comport->playerport[i].msgport.mp_SigTask      = NULL;
	    
	    NewList(&comport->playerport[i].msgport.mp_MsgList);
	}
    }
    
    if (comport->playerport[playerid].inuse)
    {
        Permit();
	I_Error("AROS_Net_Local: ComPort %ld already in use!\n", playerid + 1);
    }
    
    comport->playerport[playerid].inuse = TRUE;
    comport->users++;

    Permit();

    sendmsg.playerid = playerid;

    for(i = 0;i < doomcom->numplayers; i++)
    {
	playerid_to_node[i] = -1;
    }

    i2 = 1;i3 = 1;
    for(i = 0;i < doomcom->numplayers; i++)
    {
	if (i != playerid)
	{
	    D(bug("Player: %d   playerid_to_node[%d] = %d\n",playerid,i,i2));
	    playerid_to_node[i] = i2++;

	    D(bug("Player: %d   sendport[%d] = %d\n",playerid,i3,i));
	    sendport[i3++] = &comport->playerport[i].msgport;

	}
    }
    
    fastmsg_freeport.mp_Node.ln_Type = NT_MSGPORT;
    fastmsg_freeport.mp_Flags = PA_IGNORE;
    NewList(&fastmsg_freeport.mp_MsgList);
    
    for(i = 0;i < NUM_FASTMSG; i++)
    {
        fastmsg[i].msg.mn_Node.ln_Type = NT_MESSAGE;
	fastmsg[i].msg.mn_ReplyPort    = &fastmsg_freeport;
	fastmsg[i].msg.mn_Length       = sizeof(struct ComMsg);
	
	PutMsg(&fastmsg_freeport, &fastmsg[i].msg);
    }

    return true;
}

/****************************************************************************/

void AROSNetLocal_NetCmd (void)
{
    if (doomcom->command == CMD_SEND) {
        AROSNetLocal_NetSend ();
    } else if (doomcom->command == CMD_GET) {
        AROSNetLocal_NetGet ();
    } else
        I_Error ("AROS_Net_Local: Bad net cmd: %i\n",doomcom->command);
}

/****************************************************************************/

void AROSNetLocal_NetSend(void)
{
    /* static int counter = 0; */
    
    struct ComMsg *msg = (struct ComMsg *)GetMsg(&fastmsg_freeport);
    /* no msg free ==> let's allocate one */
    
    if (!msg) return;
    
/*    {
        Forbid();
	msg = AllocPooled(comport->pool,sizeof(struct ComMsg));
	Permit();
	
	if (!msg) I_Error("AROS_Net_Local: Could not alloc ComMsg!");
	msg->flags = CMF_ALLOCED;
	msg->pool = comport->pool;
    }
*/

    D(bug("Player %d (%06d): send to %d\n",playerid,counter++,doomcom->remotenode));

    msg->dd = *netbuffer;

    msg->datalen = doomcom->datalength;
    msg->playerid = playerid;

    PutMsg(sendport[doomcom->remotenode], &msg->msg);
}

/****************************************************************************/

void AROSNetLocal_NetGet(void)
{

    getmsg = (struct ComMsg *)GetMsg(&comport->playerport[playerid].msgport);

    if (!getmsg)
    {
	doomcom->remotenode = -1; // no packet
	return;
    }

    D(bug("player %d: -------- netget %d (node = %d)\n",
    		playerid,
		getmsg->playerid,
		playerid_to_node[getmsg->playerid]));

    doomcom->remotenode = playerid_to_node[getmsg->playerid];
    doomcom->datalength = getmsg->datalen;

    if (doomcom->remotenode < 0 || doomcom->remotenode >= MAX_PLAYERS)
    {
        doomcom->remotenode = - 1;
    } else {
        *netbuffer = getmsg->dd;
    }

    if (getmsg->flags & CMF_ALLOCED)
    {
        Forbid();
	FreePooled(getmsg->pool, getmsg, sizeof(struct ComMsg));
	Permit();	
    }
    else if (!(getmsg->flags & CMF_DEAD))
    {
  	ReplyMsg(&getmsg->msg);
    }

}

/****************************************************************************/

void AROSNetLocal_CleanupNetwork(void)
{
    WORD i;
    
    Forbid();
    
    comport->playerport[playerid].inuse = FALSE;
    comport->users--;
    
    /* Make sure sent fastmsg's are not replied back */
    
    for(i = 0;i < NUM_FASTMSG; i++)
    {
	fastmsg[i].flags |= CMF_DEAD;
    }

    if (comport->users == 0)
    {
	RemSemaphore(&comport->sem);        
    }
    
    Permit();
}

/****************************************************************************/
