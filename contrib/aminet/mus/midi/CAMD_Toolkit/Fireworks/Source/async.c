
/*--------------------------------------------------*/
/* General routines for starting asynchronous tasks */
/*--------------------------------------------------*/

#include "fireworks.h"
#include "fireworks_protos.h"


struct AsyncData
{
	struct Message Message;
	struct Globals *glob;
	struct Prefs *pref;
	TaskFlag flg;
	void (*AsyncFunction)(struct Globals *glob, struct Prefs *pref, APTR UserData);
};

SAVEDS void AsyncTask(void)
{
	struct AsyncData *ad;
	struct Process *MyProc = (struct Process*)FindTask(NULL);
	struct Globals *glob;
	struct Prefs *pref;

	WaitPort(&MyProc->pr_MsgPort);

	if ((ad = (struct AsyncData*)GetMsg(&MyProc->pr_MsgPort)))
	{
		TaskFlag flg = ad->flg;
		
		glob = ad->glob;
		pref = ad->pref;
		
		(ad->AsyncFunction)(glob, pref, (APTR)(ad+1));

		FreeVec(ad);
		
		Forbid();
		FreeAsync(glob, flg);
	}
}

BOOL StartAsyncTask(struct Globals *glob, struct Prefs *pref, UBYTE *TaskName, TaskFlag flg, APTR AsyncFunction, APTR UserData, ULONG UDLength)
{
	BOOL Success = FALSE;
	struct Process *proc;
	struct AsyncData *ad;
	
	if (AllocAsync(glob, flg))
	{
		if (!(ad = AllocVec(sizeof(struct AsyncData)+UDLength, MEMF_ANY|MEMF_CLEAR)))
		{
			Message("No memory for asynchronous task message.", NULL);
		}
		else
		{
			if (!(proc = CreateNewProcTags(
					NP_Entry, &AsyncTask,
					NP_Name, TaskName,
					NP_Priority, 0,
					NP_StackSize, 8192,
					NP_Input, Input(),
					NP_Output, Output(),
					NP_CloseInput, FALSE,
					NP_CloseOutput, FALSE,
					TAG_DONE)))
			{
				Message("Unable to create asynchronous task.\n", NULL);
			}
			else
			{
				ad->Message.mn_ReplyPort = NULL;
				ad->Message.mn_Length = sizeof(struct AsyncData)+UDLength;
				
				ad->glob = glob;
				ad->pref = pref;
				ad->flg = flg;
				ad->AsyncFunction = ( void (*)() )AsyncFunction;
				
				if (UserData && UDLength)
				{
					CopyMem(UserData, (APTR)(ad+1), UDLength);
				}
				
				PutMsg(&proc->pr_MsgPort, (struct Message*)ad);
				
				Success = TRUE;
			}
			if (!Success) FreeVec(ad);
		}
		if (!Success) FreeAsync(glob, flg);
	} else Success = TRUE;
	
	return(Success);
}


/*-----------------------------------------*/
/* Keep track of the number of Async tasks */
/*-----------------------------------------*/

BOOL AllocAsync(struct Globals *glob, TaskFlag flg)
{
	BOOL success = FALSE;
	
	Forbid();
	if (!(glob->TaskAlloc & flg))
	{
		glob->TaskAlloc |= flg;
		success = TRUE;
	}
	Permit();
	
	if (!success) if (IntuitionBase) DisplayBeep(NULL);
	
	return(success);
}

void FreeAsync(struct Globals *glob, TaskFlag flg)
{
	Forbid();
	if (glob->TaskAlloc & flg)
	{
		glob->TaskAlloc &= (~flg);
	}
	Permit();
	
	if (!glob->TaskAlloc) Signal(MyTask, SIGBREAKF_CTRL_F);
}

BOOL AskAsync(struct Globals *glob)
{
	BOOL result = FALSE;
	
	if (!glob->TaskAlloc) result = TRUE;
	
	return(result);
}

void WaitAsync(struct Globals *glob)
{
	while (glob->TaskAlloc)
	{
		Wait(SIGBREAKF_CTRL_F);
	}
}
