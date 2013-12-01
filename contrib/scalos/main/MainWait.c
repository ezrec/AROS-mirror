// MainWait.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <devices/timer.h>
#include <rexx/errors.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <datatypes/pictureclass.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/rexxsyslib.h>
#include <proto/timer.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/palette.h>
#include <scalos/pattern.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

#define	TIMER_PREFS_CHANGE	2
#define	TIMER_MAINTASK_COUNT	2

//----------------------------------------------------------------------------

// local functions

static void ProcessWBMessages(struct MainTask *mainTask);
static void ProcessTimerMessage(struct MainTask *mainTask);
static void ProcessScalosMessage(struct MainTask *mainTask, struct ScalosMessage *sMsg);
static void ProcessWblMessage(struct MainTask *mainTask, struct WblMessage *wMsg);
static void ProcessRexxMessage(struct MainTask *mainTask, struct RexxMsg *RxMsg);
static void ProcessPrefsChanges(struct MainTask *mainTask);
static void UpdatePrefs(void);
static void QuitScalos(struct ScaWindowTask *iwtMain, struct RexxMsg *msg);

//----------------------------------------------------------------------------

// local data items

// Notice: ARexx functions are called with mainTask->mwt.iwt_WindowTask as wtMain
static struct ARexxCmdEntry ARexxCommandTable[] =
	{
	{ "about", 	ACEFLAGF_RunFromMainTask, 	(AREXXFUNC) AboutProg	},
	{ "quit",	ACEFLAGF_RunFromMainTask, 	(AREXXFUNC) QuitScalos 	},
	{ NULL, 	0, 				NULL			},
	};

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------


void MainWait(struct MainTask *mainTask)
{
	ULONG WBPortMask = (1L << wbPort->mp_SigBit);
	ULONG MainPortMask = (1L << iInfos.xii_iinfos.ii_MainMsgPort->mp_SigBit);

	d1(KPrintF("%s/%s/%ld: START  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, &mainTask->mwt));

	do	{
		ULONG SigsRcvd = Wait(WBPortMask | MainPortMask);

		d1(KPrintF("%s/%s/%ld: SigsRcvd=%08lx\n", __FILE__, __FUNC__, __LINE__, SigsRcvd));

		if (SigsRcvd & WBPortMask)
			{
			//.wbmessage
			ProcessWBMessages(mainTask);
			}
		if (SigsRcvd & MainPortMask)
			{
			//.mainwait2
			struct Message *msg;

			while (!mainTask->mwt.iwt_CloseWindow && (msg = GetMsg(iInfos.xii_iinfos.ii_MainMsgPort)))
				{
				struct NotifyMessage *nMsg = (struct NotifyMessage *) msg;
				struct RexxMsg *RxMsg = (struct RexxMsg *) msg;
				struct ScalosMessage *sMsg = (struct ScalosMessage *) msg;
				struct WblMessage *wMsg = (struct WblMessage *) msg;
				struct AppMessage *appMsg = (struct AppMessage *) msg;

				d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

				if (msg == &mainTask->iorequest->tr_node.io_Message)
					{
					//.mytimermsg
					d1(KPrintF("%s/%s/%ld: Timer Message msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));
					ProcessTimerMessage(mainTask);
					continue;
					}

				if (NOTIFY_CLASS == nMsg->nm_Class && NOTIFY_CODE == nMsg->nm_Code)
					{
					const struct NotifyNode *non;
					BOOL Found = FALSE;

					d1(KPrintF("%s/%s/%ld: Notify Msg  UserData=%08lx\n", __FILE__, __FUNC__, __LINE__, nMsg->nm_NReq->nr_UserData));

					ScalosObtainSemaphoreShared(&mainTask->mwt_MainNotifyListSema);
					for (non = mainTask->mainnotifylist; non; non = (const struct NotifyNode *) non->non_Node.mln_Succ)
						{
						if (non->non_NotifyRequest.nr_UserData == nMsg->nm_NReq->nr_UserData)
							{
							struct NotifyTab *nft = (struct NotifyTab *) nMsg->nm_NReq->nr_UserData;

							ScalosReleaseSemaphore(&mainTask->mwt_MainNotifyListSema);
							Found = TRUE;

							d1(KPrintF("%s/%s/%ld: Notify Msg nft=%08lx  nft_FileName=<%s>  nft_Entry=%08lx\n", \
								__FILE__, __FUNC__, __LINE__, nft, nft->nft_FileName, nft->nft_Entry));

							(*nft->nft_Entry)(&mainTask->mwt, nMsg);
							break;
							}
						}
					if (!Found)
						ScalosReleaseSemaphore(&mainTask->mwt_MainNotifyListSema);

					d1(KPrintF("%s/%s/%ld: Notify Message\n", __FILE__, __FUNC__, __LINE__));
					ReplyMsg(&nMsg->nm_ExecMessage);

					continue;
					}

				if (ID_IMSG == sMsg->sm_Signature)
					{
					d1(KPrintF("%s/%s/%ld: ScalosMessage\n", __FILE__, __FUNC__, __LINE__));
					ProcessScalosMessage(mainTask, sMsg);
					continue;
					}

				if (ID_WBLM == wMsg->wbl_Magic)
					{
					d1(KPrintF("%s/%s/%ld: WBL Message\n", __FILE__, __FUNC__, __LINE__));
					ProcessWblMessage(mainTask, wMsg);
					continue;
					}

				if (msg->mn_Length >= sizeof(struct AppMessage)
					&& ID_APPM == appMsg->am_Reserved[0])
					{
					//freeappmsg
					d1(KPrintF("%s/%s/%ld: AppMessage\n", __FILE__, __FUNC__, __LINE__));

					SCA_FreeWBArgs(appMsg->am_ArgList, 
						appMsg->am_NumArgs, 
						SCAF_FreeLocks | SCAF_FreeNames);

					FreeVec(appMsg);
					continue;
					}

				if (IsRexxMsg((struct RexxMsg *)RxMsg))
					{
					//rexxmsggot
					d1(KPrintF("%s/%s/%ld: Rexx Message\n", __FILE__, __FUNC__, __LINE__));
					ProcessRexxMessage(mainTask, RxMsg);
					continue;
					}

				d1(KPrintF("%s/%s/%ld: unknown Msg Type\n", __FILE__, __FUNC__, __LINE__));

				// unknown message type
				ReplyMsg(msg);
				}
			}

		d1(KPrintF("%s/%s/%ld: SigsRcvd=%08lx\n", __FILE__, __FUNC__, __LINE__, SigsRcvd));

		} while (!mainTask->mwt.iwt_CloseWindow);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void ProcessWBMessages(struct MainTask *mainTask)
{
	struct WBStartup *sMsg;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	while ((sMsg = (struct WBStartup *) GetMsg(wbPort)))
		{
		ULONG n;
		struct DoWait *dw, *dwNext;

		d1(KPrintF("%s/%s/%ld: sMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, sMsg));

		ScalosObtainSemaphore(&DoWaitSemaphore);

		for (dw = DoWaitList; dw; dw = dwNext)
			{
			dwNext = (struct DoWait *) dw->dwai_Node.mln_Succ;

			d1(KPrintF("%s/%s/%ld: dw=%08lx  wbmsg=%08lx\n", __FILE__, __FUNC__, __LINE__, dw, dw->dwai_wbmsg));

			if (sMsg == dw->dwai_wbmsg)
				{
				struct DoWaitFinished *dwf = AllocVec(sizeof(struct DoWaitFinished), MEMF_PUBLIC | MEMF_CLEAR);

				d1(KPrintF("%s/%s/%ld: dw=%08lx  dwf=%08lx\n", __FILE__, __FUNC__, __LINE__, dw, dwf));

				if (dwf)
					{
					dwf->dwf_DoWait = dw;
					PutMsg(dw->dwai_msgport, &dwf->dwf_Message);
					}
				break;
				}
			}

		ScalosReleaseSemaphore(&DoWaitSemaphore);

		d1(KPrintF("%s/%s/%ld: numArgs=%ld\n", __FILE__, __FUNC__, __LINE__, sMsg->sm_NumArgs));

		for (n=0; n<sMsg->sm_NumArgs; n++)
			{
			d1(KPrintF("%s/%s/%ld: n=%ld Name=%08lx <%s>  Lock=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, n, sMsg->sm_ArgList[n].wa_Name, \
				sMsg->sm_ArgList[n].wa_Name, \
				sMsg->sm_ArgList[n].wa_Lock));
			debugLock_d1(sMsg->sm_ArgList[n].wa_Lock);

			if (NULL == sMsg->sm_ArgList[n].wa_Name && BNULL == sMsg->sm_ArgList[n].wa_Lock)
				break;

			UnLock(sMsg->sm_ArgList[n].wa_Lock);
			FreeCopyString(sMsg->sm_ArgList[n].wa_Name);
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		RemWBProgram(sMsg);
		}
}


static void ProcessTimerMessage(struct MainTask *mainTask)
{
	d1(KPrintF("%s/%s/%ld: Start\n", __FILE__, __FUNC__, __LINE__));

	// called every 0.25s

	if (mainTask->mt_TimerCounter++ >= TIMER_MAINTASK_COUNT)
		{
		T_TIMEVAL Now;

		// called every second

		mainTask->mt_TimerCounter = 0;

		GetSysTime(&Now);

		ClassTimerToolTipMsg(&mainTask->mwt);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (SCA_LockWindowList(SCA_LockWindowList_AttemptShared))
			{
			struct ScaWindowStruct *ws;

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
				{
				d1(KPrintF("%s/%s/%ld: ws=%08lx  ws_Flags=%08lx  iwt_MoveGadId=%ld\n", __FILE__, __FUNC__, __LINE__, ws, ws->ws_Flags, \
					((struct internalScaWindowTask *) ws->ws_WindowTask)->iwt_MoveGadId));

				if ((ws->ws_Flags & (WSV_FlagF_CheckUpdatePending | WSV_FlagF_Typing | WSV_FlagF_NeedsTimerMsg))
					&& !(ws->ws_Flags & (WSV_FlagF_TaskSleeps | WSV_FlagF_TimerMsgSent)))
					{
					struct SM_Timer *msg;

					ws->ws_Flags |= WSV_FlagF_TimerMsgSent;

					msg = (struct SM_Timer *) SCA_AllocMessage(MTYP_Timer, 0);
					d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));
					if (msg)
						{
						msg->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
						msg->smtm_WindowStruct = ws;
						msg->smtm_Time = Now;

						PutMsg(ws->ws_MessagePort, &msg->ScalosMessage.sm_Message);
						}
					}
				}

			SCA_UnLockWindowList();
			}
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ProcessPrefsChanges(mainTask);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	// Restart timer
	StartMainTaskTimer(mainTask);

	d1(KPrintF("%s/%s/%ld: Finished\n", __FILE__, __FUNC__, __LINE__));
}


static void ProcessScalosMessage(struct MainTask *mainTask, struct ScalosMessage *sMsg)
{
	APTR ArgPtr = (APTR) (((UBYTE *) &sMsg->sm_MessageType) + sizeof(sMsg->sm_MessageType));
	short mIndex = sMsg->sm_MessageType - 1;

	d1(KPrintF("%s/%s/%ld: Msg=%08lx  MsgType=%ld, ln_Type=%ld\n", \
		__FILE__, __FUNC__, __LINE__, sMsg, sMsg->sm_MessageType, sMsg->sm_Message.mn_Node.ln_Type));

	if (NT_REPLYMSG == sMsg->sm_Message.mn_Node.ln_Type)
		{
		if (mIndex > 0 && mIndex < MTYP_MAX)
			{
			d1(KPrintF("%s/%s/%ld: ReplyFunc=%08lx\n", __FILE__, __FUNC__, __LINE__, iMsgTable[mIndex].ReplyFunc));

			if (iMsgTable[mIndex].ReplyFunc)
				(*iMsgTable[mIndex].ReplyFunc)(&mainTask->mwt, &sMsg->sm_Message, ArgPtr);
			}

		SCA_FreeMessage(sMsg);
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: MsgFunc=%08lx\n", __FILE__, __FUNC__, __LINE__, iMsgTable[mIndex].MsgFunc));

		if (mIndex > 0 && mIndex < MTYP_MAX)
			{
			if (iMsgTable[mIndex].MsgFunc)
				(*iMsgTable[mIndex].MsgFunc)(&mainTask->mwt, &sMsg->sm_Message, ArgPtr);
			}

		if (sMsg->sm_Message.mn_ReplyPort)
			ReplyMsg(&sMsg->sm_Message);
		else
			SCA_FreeMessage(sMsg);
		}

}


static void ProcessWblMessage(struct MainTask *mainTask, struct WblMessage *wMsg)
{
	struct DoWait *dw, *dwNext;

	d1(KPrintF("%s/%s/%ld: wMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, wMsg));

	ScalosObtainSemaphore(&DoWaitSemaphore);

	for (dw = DoWaitList; dw; dw = dwNext)
		{
		dwNext = (struct DoWait *) dw->dwai_Node.mln_Succ;

		if (wMsg == dw->dwai_wblmsg)
			{
			d1(KPrintF("%s/%s/%ld: dw=%08lx\n", __FILE__, __FUNC__, __LINE__, dw));

			dw->dwai_wbmsg = wMsg->wbl_IO.wbl_Return.wblr_WBStartup;
			break;
			}
		}

	ScalosReleaseSemaphore(&DoWaitSemaphore);
	FreeVec(wMsg);

}


static void ProcessRexxMessage(struct MainTask *mainTask, struct RexxMsg *RxMsg)
{
	struct ARexxCmdEntry *ace;
	BOOL Found = FALSE;

	d1(KPrintF("%s/%s/%ld: RxMsg=%08lx  Arg[0]=<%s>\n", __FILE__, __FUNC__, __LINE__, RxMsg, RxMsg->rm_Args[0]));

	for (ace=ARexxCommandTable; !Found && ace->ace_CmdName; ace++)
		{
		if (0 == Stricmp(ace->ace_CmdName, (STRPTR) RxMsg->rm_Args[0]))
			{
			Found = TRUE;

			RxMsg->rm_Result1 = RC_OK;
			RxMsg->rm_Result2 = 0;

			d1(KPrintF("%s/%s/%ld: EntryPoint=%08lx  Flags=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, ace->ace_EntryPoint, ace->ace_Flags));

			if (ace->ace_Flags & ACEFLAGF_RunFromMainTask)
				{
				struct SM_ARexxRoutine *smrx;

				smrx = (struct SM_ARexxRoutine *) SCA_AllocMessage(MTYP_ARexxRoutine, 0);
				if (smrx)
					{
					smrx->smrx_RexxMsg = RxMsg;
					smrx->smrx_EntryPoint = ace->ace_EntryPoint;

					PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smrx->ScalosMessage.sm_Message);
					}
				}
			else
				{
				(*ace->ace_EntryPoint)(&mainTask->mwt.iwt_WindowTask, RxMsg);
				ReplyMsg(&RxMsg->rm_Node);
				}
			}
		}

	if (!Found)
		{
		ReplyMsg(&RxMsg->rm_Node);

		RxMsg->rm_Result1 = RC_FATAL;
		}
}


static void ProcessPrefsChanges(struct MainTask *mainTask)
{
	if (mainTask->mt_PrefsChangedTimer++ >= TIMER_PREFS_CHANGE)
		{
		if (mainTask->mt_PrefsChangedFlags)
			{
			BOOL Changed = FALSE;
			BOOL PaletteChanged = FALSE;

			d1(KPrintF("%s/%s/%ld: mt_PrefsChangedFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, MainWindowTask->mt_PrefsChangedFlags));

			if (mainTask->mt_PrefsChangedFlags & SMNPFLAGF_FONTPREFS)
				Changed |= ChangedFontPrefs(mainTask);
			if (mainTask->mt_PrefsChangedFlags & SMNPFLAGF_FILETYPESPREFS)
				Changed |= ChangedFileTypesPrefs(mainTask);
			if (mainTask->mt_PrefsChangedFlags & SMNPFLAGF_DEFICONSPREFS)
				Changed |= ChangedDefIconsPrefs();
			if (mainTask->mt_PrefsChangedFlags & SMNPFLAGF_LOCALEPREFS)
				Changed |= ChangedLocalePrefs(mainTask);
			if (mainTask->mt_PrefsChangedFlags & SMNPFLAGF_PATTERNPREFS)
				Changed |= ChangedPatternPrefs(mainTask);
			if (mainTask->mt_PrefsChangedFlags & SMNPFLAGF_SCALOSPREFS)
				Changed |= ChangedMainPrefs(mainTask);
			if (mainTask->mt_PrefsChangedFlags & (SMNPFLAGF_MENUPREFS))
				Changed |= ChangedMenuPrefs(mainTask);
			if (mainTask->mt_PrefsChangedFlags & SMNPFLAGF_PALETTEPREFS)
				Changed |= PaletteChanged = ChangedPalettePrefs(mainTask);

			d1(KPrintF("%s/%s/%ld: Changed=%lu\n", __FILE__, __FUNC__, __LINE__, Changed));

			if (Changed && !PaletteChanged)
				UpdatePrefs();

			if (mainTask->mt_ChangedMenuPrefsName)
				{
				FreeCopyString(mainTask->mt_ChangedMenuPrefsName);
				mainTask->mt_ChangedMenuPrefsName = NULL;
				}
			if (mainTask->mt_ChangedPalettePrefsName)
				{
				FreeCopyString(mainTask->mt_ChangedPalettePrefsName);
				mainTask->mt_ChangedPalettePrefsName = NULL;
				}

			mainTask->mt_PrefsChangedFlags = 0;
			}
		mainTask->mt_PrefsChangedTimer = 0;
		}
}


static void UpdatePrefs(void)
{
	struct ScaWindowStruct *ws;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		d1(KPrintF("%s/%s/%ld:  Name=<%s>  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, ws->ws_Name, ws->ws_Flags));

		if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps))
			{
			struct SM_PrefsChanged *smpc;

			smpc = (struct SM_PrefsChanged *) SCA_AllocMessage(MTYP_PrefsChanged, 0);

			if (smpc)
				{
				smpc->smpc_Flags = 0;
				PutMsg(ws->ws_MessagePort, &smpc->ScalosMessage.sm_Message);
				}
			}
		}

	SCA_UnLockWindowList();

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

}




void RememberPrefsChanges(struct internalScaWindowTask *iwt, ULONG PrefsChangeFlags)
{
	struct MainTask *mainTask = MainWindowTask;

	if (iwt == &mainTask->mwt)
		{
		mainTask->mt_PrefsChangedFlags |= PrefsChangeFlags;
		d1(KPrintF("%s/%s/%ld: PrefsChangeFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, PrefsChangeFlags));
		d1(KPrintF("%s/%s/%ld: mt_PrefsChangedFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, mainTask->mt_PrefsChangedFlags));
		mainTask->mt_PrefsChangedTimer = 0;
		}
}


void StartMainTaskTimer(struct MainTask *mt)
{
	mt->iorequest->tr_node.io_Command = TR_ADDREQUEST;
	mt->iorequest->tr_time.tv_secs = 0;
	mt->iorequest->tr_time.tv_micro = 250000;	// 0.25s period

	SendIO(&mt->iorequest->tr_node);
}


struct NotifyNode *AddToMainNotifyList(struct NotifyTab *nft, ULONG Flags)
{
	struct NotifyNode *non;

	do	{
                struct MainTask *mainTask = MainWindowTask;

		ScalosObtainSemaphore(&MainWindowTask->mwt_MainNotifyListSema);
		non = (struct NotifyNode *) SCA_AllocNode((struct ScalosNodeList *) &mainTask->mainnotifylist, sizeof(struct NotifyNode));
		ScalosReleaseSemaphore(&MainWindowTask->mwt_MainNotifyListSema);
		d1(KPrintF("%s/%s/%ld:  non=%08lx\n", __FILE__, __FUNC__, __LINE__, non));
		if (NULL == non)
			break;

		non->non_NotifyRequest.nr_UserData = (ULONG) nft;
		non->non_NotifyRequest.nr_Name = (STRPTR) nft->nft_FileName;
		non->non_NotifyRequest.nr_stuff.nr_Msg.nr_Port = iInfos.xii_iinfos.ii_MainMsgPort;
		non->non_NotifyRequest.nr_Flags = NRF_SEND_MESSAGE | Flags;

		if (!StartNotify(&non->non_NotifyRequest))
			{
			// free ffi_NotifyNode, but continue and add ffi!
			d1(kprintf("%s/%s/%ld:  StartNotify() failed\n", __FILE__, __FUNC__, __LINE__));

			SCA_FreeNode((struct ScalosNodeList *) &mainTask->mainnotifylist, &non->non_Node);
			non = NULL;
			}
		d1(kprintf("%s/%s/%ld:  StartNotify() OK\n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	d1(KPrintF("%s/%s/%ld: AddToMainNotifyList return=%8lx\n",
		__FILE__, __FUNC__, __LINE__, non));

	return non;
}


void RemFromMainNotifyList(struct NotifyNode *nonOld)
{
	struct MainTask *mainTask = MainWindowTask;
	struct NotifyNode *non;

	d1(KPrintF("%s/%s/%ld: RemFromMainNotifyList %8lx\n",
		__FILE__, __FUNC__, __LINE__, nonOld));

	if (MainWindowTask)
		{
		ScalosObtainSemaphore(&MainWindowTask->mwt_MainNotifyListSema);

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		// make sure nonOld is still present in mainnotifylist
		for (non = mainTask->mainnotifylist;
			non; 
			non = (struct NotifyNode *) non->non_Node.mln_Succ)
			{
			d1(KPrintF("%s/%s/%ld: non(%08lx) nonOld(%08lx)\n", __FILE__, __FUNC__, __LINE__,
				non, nonOld));
			if (non == nonOld)
				{
				ScalosEndNotify(&nonOld->non_NotifyRequest);
				SCA_FreeNode((struct ScalosNodeList *) &mainTask->mainnotifylist, &nonOld->non_Node);
				break;
				}
			}

		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		ScalosReleaseSemaphore(&MainWindowTask->mwt_MainNotifyListSema);
		}
}


static void QuitScalos(struct ScaWindowTask *wtMain, struct RexxMsg *msg)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

	(void) wtMain;
	(void) msg;

	iwtMain->iwt_CloseWindow = TRUE;
}


