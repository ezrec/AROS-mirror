#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <exec/memory.h>
#include <exec/semaphores.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>
#include <intuition/intuition.h>

#include <clib/alib_protos.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "reqlist.h"
#include "misc.h"
#include "reqoffexec.h"
#include "asmmisc.h"

#define SIGF_ROGER SIGBREAKF_CTRL_D
#define TASKNAME "ReqOFFExec SubProcess"

#define REQATTACK_DISPLAYREQ 127

void ReqOFFExecute(struct ReqNode *reqnode)
{
	struct Process *task;
	struct ReqOFFData reqoffdata;
	
	reqnode->cfg.reqoff = -1;

	if (reqnode->cfg.reqoffexec[0])
	{
		if ((task= CreateNewProcTags(NP_Entry,(LONG)ReqOFFExecProcess,
									NP_StackSize,8192,
									NP_WindowPtr,-1,
									NP_Name,(ULONG)TASKNAME,
									TAG_DONE)))
		{

			reqoffdata.task = FindTask(NULL);
			reqoffdata.reqnode = reqnode;
			
			task->pr_Task.tc_UserData = (APTR)&reqoffdata;
			//everything is set up - now make it work!
			Signal((struct Task *)task,SIGF_ROGER);
			
			Wait(SIGF_ROGER);
		}
	}
}

void ReqOFFExecProcess(void)
{
	struct ReqOFFData *reqoffdata;
	struct ReqNode *reqnode;
	struct Process *task;
	char   *exec;
	struct ReplacerData *repdata;
	struct EasyStruct *eareq;
		
	//wait for the father task to set up everything...
	Wait(SIGF_ROGER);

	task = (struct Process *)FindTask(NULL);
	reqoffdata = (struct ReqOFFData *)task->pr_Task.tc_UserData;
	reqnode = reqoffdata->reqnode;
	
	if ((repdata = (struct ReplacerData *)MiscAllocVec(sizeof(struct ReplacerData))))
	{
		if ((exec=MiscAllocVec(strlen(reqnode->cfg.reqoffexec)+14)))
		{
			if ((eareq=MiscAllocVec(sizeof(struct EasyStruct))))
			{
				ULONG argt[] = {(ULONG)repdata};

				repdata->window = 0;//fuck it!
				repdata->idcmp = 0;//fuck it too!
				repdata->easystr = eareq;
				eareq->es_StructSize = sizeof(struct EasyStruct);
				eareq->es_Flags = 0;
				eareq->es_Title = (UBYTE *)reqnode->title;
				eareq->es_TextFormat = (UBYTE *)reqnode->bodytext;
				eareq->es_GadgetFormat = (UBYTE *)reqnode->buttontext;
				repdata->args = 0; //everything is set up... no RawDoFmt!
				
			    #ifdef NO_ASM
			    {
			    	STRPTR strptr = exec;
				RawDoFmt(reqnode->cfg.reqoffexec,argt,(APTR)Copy_PutChFunc,&strptr);
			    }
			    #else

				RawDoFmt(reqnode->cfg.reqoffexec,argt,(APTR)Copy_PutChFunc,exec);
			    #endif
	
				reqnode->cfg.reqoff = SystemTagList(exec,NULL);
				if (reqnode->cfg.reqoff == REQATTACK_DISPLAYREQ) reqnode->cfg.reqoff = -1;

				MiscFreeVec(eareq);
			}
			MiscFreeVec(exec);
		}
		MiscFreeVec(repdata);
	}

	Signal(reqoffdata->task,SIGF_ROGER);
}

