#include <dos/dos.h>
#include <libraries/commodities.h>
#include <exec/memory.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/commodities_lib.h>
#else
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/commodities.h>
#endif

#include <clib/alib_protos.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "cx.h"
#include "asmmisc.h"

ULONG cxmask;
BOOL cxinactive;
char *rap;

static struct MsgPort brokerport;
static CxObj *broker;

void InitCX(void)
{
	struct NewBroker nbr;

	if (CxBase)
	{
		brokerport.mp_Node.ln_Type = NT_MSGPORT;
		brokerport.mp_Flags = PA_SIGNAL;
		brokerport.mp_SigBit = AllocSignal(-1);
		brokerport.mp_SigTask = FindTask(0);
		NewList(&brokerport.mp_MsgList);

		cxmask = 1L << brokerport.mp_SigBit;

		if (cxmask)
		{
			nbr.nb_Version = NB_VERSION;
			nbr.nb_Name = "ReqAttack";
			nbr.nb_Title = "ReqAttack " VERSION_STRING " ©2003 by Morgoth/D-CAPS";
			nbr.nb_Descr = "Better look and feeling of requesters";
			nbr.nb_Unique = NBU_UNIQUE;
			nbr.nb_Flags = COF_SHOW_HIDE;
			nbr.nb_Pri = 0;
			nbr.nb_Port = &brokerport;

			if ((broker = CxBroker(&nbr,0)))
			{
				ActivateCxObj(broker,TRUE);
			}

		} /* if (cxmask) */

	} /* if (CxBase) */
}

void CleanupCX(void)
{
	CxMsg *msg;

	if (broker)
	{
		DeleteCxObjAll(broker);
	}

	if (brokerport.mp_SigBit)
	{
		while((msg = (CxMsg *)GetMsg(&brokerport)))
		{
			ReplyMsg((struct Message *)msg);
		}

		FreeSignal(brokerport.mp_SigBit);
	}

}

void HandleCX(void)
{
	LONG msgid;
	ULONG msgtype;
	CxMsg *msg;
	struct MsgPort *p;

	while ((msg = (CxMsg *)GetMsg(&brokerport)))
	{
		msgid = CxMsgID(msg);
		msgtype = CxMsgType(msg);
		ReplyMsg((struct Message *)msg);

		switch (msgtype)
		{
			case CXM_COMMAND:
				switch (msgid)
				{
					case CXCMD_DISABLE:
						ActivateCxObj(broker,FALSE);
						cxinactive = TRUE;
						break;

					case CXCMD_ENABLE:
						ActivateCxObj(broker,TRUE);
						cxinactive = FALSE;
						break;

					case CXCMD_KILL:
						Signal(FindTask(0),SIGBREAKF_CTRL_C);
						break;

					case CXCMD_APPEAR:
						Forbid();
						p = FindPort(RAPREFS);
						if (!(p))
							{
								Permit();
								rap = AllocVec(strlen(raprefspath)+13,MEMF_PUBLIC+MEMF_CLEAR);
								if (rap)
								{
									ULONG argt[] = {(ULONG)raprefspath};
								    #ifdef NO_ASM
								    {
								    	STRPTR strptr = rap;
									RawDoFmt("Run >NIL: %s",argt,
										 (APTR)Copy_PutChFunc,&strptr);
									
								    }
								    #else
									RawDoFmt("Run >NIL: %s",argt,
										 (APTR)Copy_PutChFunc,rap);
								    #endif
									//sprintf(rap,"Run >NIL: %s",raprefspath);
									Execute(rap,0,0);
									FreeVec(rap);
								}
							} else {
								Signal(p->mp_SigTask,1);
							}
						Permit();
						break;

					case CXCMD_DISAPPEAR:
						Forbid();
						p = FindPort(RAPREFS);
						if (p)
							{
								Signal(p->mp_SigTask,2);
							}
						Permit();
						break;

				} /* switch (msgid) */
				break;

		} /* switch (msgtype) */

	} /* while ((msg = (CxMsg *)GetMsg(&brokerport))) */
}

