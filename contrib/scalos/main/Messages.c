// Messages.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <rexx/rxslib.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include <proto/rexxsyslib.h>
#include <proto/gadtools.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

static ULONG WinRunMenuCmdMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG AsyncRoutineMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG ShowTitleMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG AppSleepMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG AppWakeupMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG IconifyMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG UnIconifyMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG UpdateMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG RedrawMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG UpdateIconMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG AddIconMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG RemIconMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG WindowSleepMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG WindowWakeupMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG TimerReplyMsg(struct internalScaWindowTask *iwt, 	struct Message *msg, APTR p);
static ULONG ARexxRoutineMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG ShowPopupMenuMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG ShowStatusBarMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG ShowControlBarMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG RedrawIconMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG RedrawIconObjectMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG DoPopupMenuMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG PrefsChangedMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG DeltaMoveMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG SetThumbnailImageRemappedMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG SetThumbnailImageARGBMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG NewWindowPathMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG WindowPrefsChangedMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);
static ULONG WindowRootEventMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p);

//----------------------------------------------------------------------------

// public data items

struct MsgTableEntry iMsgTable[] =
	{
	{ NULL,				NULL		},	// MTYP_DropProc	DropTask startup
	{ CloseWindowMsg,		NULL		},	// MTYP_CloseWindow	Window
	{ NULL,				NULL		},	// MTYP_StartWindow	Get+Reply on ProcStart
	{ ClassWinTimerMsg,		TimerReplyMsg	},	// MTYP_Timer		Timer pulse
	{ NULL,				NULL		},	// MTYP_Requester	AsyncRequester Msg
	{ NewWindowPatternMsg,		NULL		},	// MTYP_NewPattern	set backfill pattern
	{ WindowSleepMsg,		NULL		},	// MTYP_Sleep		WindowTask sleep
	{ WindowWakeupMsg,		NULL		},	// MTYP_Wakeup		WindowTask wakeup
	{ AsyncRoutineMsg,		NULL		},	// MTYP_AsyncRoutine	arg = routine
	{ NULL,				NULL		},	// MTYP_RunProcess	run routine with own process
	{ AppSleepMsg,			NULL		},	// MTYP_AppSleep	MainProg sleep
	{ AppWakeupMsg,			NULL		},	// MTYP_AppWakeup	MainProg wakeup
	{ RedrawMsg,			NULL		},	// MTYP_Redraw		Redraw the windows icons
	{ UpdateMsg,			NULL		},	// MTYP_Update		reread the windows icons
	{ UpdateIconMsg,		NULL		},	// MTYP_UpdateIcon	Reread an icon
	{ AddIconMsg,			NULL		},	// MTYP_AddIcon		Add an icon to the iconlist
	{ RemIconMsg,			NULL		},	// MTYP_RemIcon		Remove an icon from the iconlist
	{ ARexxRoutineMsg,		NULL		},	// MTYP_ARexxRoutine	Run a routine and reply arexx msg
	{ IconifyMsg,			NULL		},	// MTYP_Iconify		Iconify window
	{ UnIconifyMsg,			NULL		},	// MTYP_UnIconify	UnIconify window
	{ NULL,				AsyncReplyMsg	},	// MTYP_AsyncBackFill	Asynchronous Backfill
	{ ShowTitleMsg,			NULL		},	// MTYP_ShowTitle	Showtitle
	{ WinRunMenuCmdMsg,		NULL		},	// MTYP_RunMenuCmd	Run Menu Cmd
	{ ShowPopupMenuMsg,		NULL		},	// MTYP_ShowPopupMenu	Showtitle
	{ ShowStatusBarMsg,		NULL		},	// MTYP_ShowStatusBar	Show/Hide status bar
	{ RedrawIconMsg,		NULL		},	// MTYP_RedrawIcon	Redraw one icon
	{ DoPopupMenuMsg,		NULL		},	// MTYP_DoPopupMenu	open popup menu
	{ RedrawIconObjectMsg,		NULL		},	// MTYP_RedrawIconObj	Redraw one iconobject
	{ PrefsChangedMsg,		NULL		},	// MTYP_NewPreferences	some preferences have changed
	{ DeltaMoveMsg,			NULL		},	// MTYP_DeltaMove	scroll window contents deltax/deltay
	{ SetThumbnailImageRemappedMsg,	NULL		},	// MTYP_SetThumbnailImage_Remapped set color-mapped thumbnail image for icon
	{ SetThumbnailImageARGBMsg,	NULL		},	// MTYP_SetThumbnailImage_ARGB		set ARGB thumbnail image for icon
	{ NewWindowPathMsg,		NULL		},	// MTYP_NewWindowPath	switch window to display new drawer
	{ WindowPrefsChangedMsg,	NULL		},	// MTYP_PrefsChanged	 notifies window about changed preferences
	{ NULL,				NULL		},	// MTYP_StartChildProcess
	{ WindowRootEventMsg,		NULL		},	// MTYP_RootEvent
	{ ShowControlBarMsg,		NULL		},	// MTYP_ShowControlBar
	};

//----------------------------------------------------------------------------

LONG SendAppMessage(struct AppObject *appo, ULONG AmClass, WORD x, WORD y)
{
	LONG Result = RETURN_OK;
	struct AppMessage *am;

	d1(kprintf("%s/%s/%ld: appo=%08lx\n", __FILE__, __FUNC__, __LINE__, appo));

	am = AllocVec(sizeof(struct AppMessage), MEMF_CLEAR | MEMF_PUBLIC);
	if (am)
		{
		switch (appo->appo_type)
			{
		case APPTYPE_AppIcon:
			am->am_Type = AMTYPE_APPICON;
			break;
		case APPTYPE_AppWindow:
			am->am_Type = AMTYPE_APPWINDOW;
			break;
		case APPTYPE_AppMenuItem:
			am->am_Type = AMTYPE_APPMENUITEM;
			break;
			}

		am->am_NumArgs = 0;

		am->am_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
		am->am_Message.mn_Length = sizeof(struct AppMessage);
		am->am_UserData = appo->appo_userdata;
		am->am_ID = appo->appo_id;
		am->am_Version = AM_VERSION;
		am->am_Class = AmClass;
		am->am_MouseX = x;
		am->am_MouseY = y;
		am->am_Reserved[0] = ID_APPM;

		CurrentTime(&am->am_Seconds, &am->am_Micros);

		PutMsg(appo->appo_msgport, &am->am_Message);
		}
	else
		{
		Result = ERROR_NO_FREE_STORE;
		}

	return Result;
}


ULONG AsyncReplyMsg(struct internalScaWindowTask *iwt, 	struct Message *msg, APTR p)
{
	struct SM_AsyncBackFill *smab = (struct SM_AsyncBackFill *) msg;
	struct PatternInfo *pti = (struct PatternInfo *) smab->smab_PatternInfoCopy;

	smab->smab_PatternInfo->ptinf_message = NULL;
	smab->smab_PatternInfo->ptinf_width = pti->ptinf_width;
	smab->smab_PatternInfo->ptinf_height = pti->ptinf_height;
	smab->smab_PatternInfo->ptinf_bitmap = pti->ptinf_bitmap;
	smab->smab_PatternInfo->ptinf_flags = pti->ptinf_flags;

	d1(KPrintF("%s/%s/%ld: smab=%08lx  PatternInfoCopy=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, smab, pti));
	d1(KPrintF("%s/%s/%ld: iwt=%08lx  ptinf=%08lx  bitmap=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, smab->smab_PatternInfo, smab->smab_PatternInfo->ptinf_bitmap));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Redraw, REDRAWF_DontRefreshWindowFrame);

	iwt->iwt_AsyncLayoutPending = FALSE;
	d1(KPrintF("%s/%s/%ld: iwt=%08lx  AsyncLayoutPending=%lu\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_AsyncLayoutPending));

	return 0;
}


static ULONG WinRunMenuCmdMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_RunMenuCmd *smrm = (struct SM_RunMenuCmd *) msg;

	d1(kprintf("%s/%s/%ld: MenuItem=%08lx Next=%08lx  IconNode=%08lx  Flags=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, smrm->smrm_MenuItem, smrm->smrm_MenuItem->mtre_Next, smrm->smrm_IconNode, smrm->smrm_Flags));

	RunMenuCommand(iwt, smrm->smrm_MenuItem, smrm->smrm_IconNode, smrm->smrm_Flags);

	DisposeMenuTree(smrm->smrm_MenuItem);

	return 0;
}


static ULONG AsyncRoutineMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_AsyncRoutine *smar = (struct SM_AsyncRoutine *) msg;

	d1(KPrintF("%s/%s/%ld: smar=%08lx  args=%08lx \n", __FILE__, __FUNC__, __LINE__, smar, smar->smar_Args));

	(smar->smar_EntryPoint)(&iwt->iwt_WindowTask, smar->smar_Args);

	return 0;
}


ULONG CloseWindowMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	d1(KPrintF("\n" "%s/%s/%ld: iwt=%08lx  ws=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.mt_WindowStruct));

	iwt->iwt_CloseWindow = TRUE;

	return 0;
}


static ULONG ShowTitleMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_ShowTitle *smst = (struct SM_ShowTitle *) msg;

	d1(KPrintF("\n" "%s/%s/%ld: iwt=%08lx  ws=%08lx  ShowTitle=%ld\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.mt_WindowStruct, smst->smst_showTitle));

	ShowTitle(iInfos.xii_iinfos.ii_Screen, smst->smst_showTitle);

	return 0;
}


static ULONG AppSleepMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct ScaWindowStruct *ws;
	ULONG WaitCount = 0;
	struct Hook *hook;

	d1(kprintf("\n" "%s/%s/%ld: iwt=%08lx  ws=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.mt_WindowStruct));

	ResetToolTips(iwt);	// make sure no Tooltip is open

	ScalosObtainSemaphoreShared(&CloseWBHookListSemaphore);

	for (hook = (struct Hook *) globalCloseWBHookList.lh_Head;
		hook != (struct Hook *) &globalCloseWBHookList.lh_Tail;
		hook = (struct Hook *) hook->h_MinNode.mln_Succ)
		{
		struct SetupCleanupHookMsg schm;

		d1(kprintf("%s/%s/%ld: hook=%08lx  h_Entry=%08lx\n", __FILE__, __FUNC__, __LINE__, hook, hook->h_Entry));

		schm.schm_Length = sizeof(schm);
		schm.schm_State = SCHMSTATE_Cleanup;

		CallHookPkt(hook, NULL, &schm);
		}

	ScalosReleaseSemaphore(&CloseWBHookListSemaphore);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: ws=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, ws, ws->ws_Title));

		if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps))
			{
			d1(kprintf("%s/%s/%ld: ws=%08lx\n", __FILE__, __FUNC__, __LINE__, ws));

			if ((NULL == iwt->iwt_WindowTask.mt_WindowStruct && ws == iInfos.xii_iinfos.ii_MainWindowStruct)
				|| ws == iwt->iwt_WindowTask.mt_WindowStruct)
				{
				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_Sleep);
				}
			else
				{
				struct SM_Sleep *msg = (struct SM_Sleep *) SCA_AllocMessage(MTYP_Sleep, 0);

				d1(kprintf("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

				if (msg)
					{
					msg->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;

					PutMsg(ws->ws_MessagePort, &msg->ScalosMessage.sm_Message);
					WaitCount++;
					}
				}
			}
		}

	SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	while (WaitCount)
		{
		d1(kprintf("%s/%s/%ld: WaitCount=%lu\n", __FILE__, __FUNC__, __LINE__, WaitCount));

		WaitReply(iInfos.xii_iinfos.ii_MainMsgPort, iwt, MTYP_Sleep);
		WaitCount--;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	Delay(30);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static ULONG AppWakeupMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_AppWakeup *smaw = (struct SM_AppWakeup *) msg;
	struct ScaWindowStruct *ws;
	struct Hook *hook;
	ULONG WaitCount = 0;

	if (smaw->smaw_ReLayout)
		{
		if (NULL == iInfos.xii_iinfos.ii_visualinfo)
			iInfos.xii_iinfos.ii_visualinfo = GetVisualInfoA(iInfos.xii_iinfos.ii_Screen, NULL);

		LayoutMenus(MainMenu, iInfos.xii_iinfos.ii_visualinfo,
			GTMN_NewLookMenus, TRUE,
			TAG_END);
		}

	SCA_LockWindowList(SCA_LockWindowList_Exclusiv);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws->ws_Flags & WSV_FlagF_TaskSleeps)
			{
			if ((NULL == iwt->iwt_WindowTask.mt_WindowStruct && ws == iInfos.xii_iinfos.ii_MainWindowStruct)
				|| ws == iwt->iwt_WindowTask.mt_WindowStruct)
				{
				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
				DoMethod(ws->ws_WindowTask->mt_MainObject, SCCM_IconWin_WakeUp);
				}
			else
				{
				struct SM_Wakeup *msg = (struct SM_Wakeup *) SCA_AllocMessage(MTYP_Wakeup, 0);

				if (msg)
					{
					msg->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
					msg->smwu_ReLayout = smaw->smaw_ReLayout;

					PutMsg(ws->ws_MessagePort, &msg->ScalosMessage.sm_Message);
					WaitCount++;
					}
				}
			}
		}

	while (WaitCount)
		{
		WaitReply(iInfos.xii_iinfos.ii_MainMsgPort, iwt, MTYP_Wakeup);
		WaitCount--;
		}

	SCA_UnLockWindowList();

	ScalosObtainSemaphoreShared(&CloseWBHookListSemaphore);

	for (hook = (struct Hook *) globalCloseWBHookList.lh_Head;
		hook != (struct Hook *) &globalCloseWBHookList.lh_Tail;
		hook = (struct Hook *) hook->h_MinNode.mln_Succ)
		{
		struct SetupCleanupHookMsg schm;

		d1(kprintf("%s/%s/%ld: hook=%08lx  h_Entry=%08lx\n", __FILE__, __FUNC__, __LINE__, hook, hook->h_Entry));

		schm.schm_Length = sizeof(schm);
		schm.schm_State = SCHMSTATE_Setup;

		CallHookPkt(hook, NULL, &schm);
		}

	ScalosReleaseSemaphore(&CloseWBHookListSemaphore);

	return 0;
}


static ULONG IconifyMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Iconify);

	return 0;
}


static ULONG UnIconifyMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_UnIconify);

	return 0;
}


static ULONG UpdateMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	UBYTE ActualViewByType;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  iwt_WinTitle=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: ReplyTask=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->mn_ReplyPort->mp_SigTask));
	d1(KPrintF("%s/%s/%ld: ClassName=<%s>\n", __FILE__, __FUNC__, __LINE__, ws->ms_ClassName));

	if (WSV_Type_DeviceWindow != iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType
			&& !(ws->ws_Flags & WSV_FlagF_TaskSleeps))
		{
		if (0 == strcmp(ws->ms_ClassName, (STRPTR) "IconWindow.sca"))
			ActualViewByType = IDTV_ViewModes_Icon;
		else
			ActualViewByType = IDTV_ViewModes_Name;

		d1(KPrintF("%s/%s/%ld: ActualViewByType=%ld  ws_Viewmodes=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ActualViewByType, ws->ws_Viewmodes));

		if ((IsIwtViewByIcon(iwt) && !IsViewByIcon(ActualViewByType))
			|| (!IsIwtViewByIcon(iwt) && IsViewByIcon(ActualViewByType)))
			{
			// View mode has changed
			UBYTE NewViewByType = ws->ws_Viewmodes;

			d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

			ws->ws_Viewmodes = ActualViewByType;
			ViewWindowBy(iwt, NewViewByType);

			return 0;
			}
		}

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 0;
}


static ULONG RedrawMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_Redraw *smmr = (struct SM_Redraw *) msg;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Redraw, smmr->smmr_Flags);

	return 0;
}


static ULONG UpdateIconMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_UpdateIcon *smui = (struct SM_UpdateIcon *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: Lock=%08lx  IconName=<%s>\n",\
		__FILE__, __FUNC__, __LINE__, smui->smui_DirLock, smui->smui_IconName));
	debugLock_d1(smui->smui_DirLock);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_UpdateIconTags,
		smui->smui_DirLock, smui->smui_IconName,
		UPDATEICON_IconType, smui->smui_IconType,
		TAG_END);

	UnLock(smui->smui_DirLock);
	FreeVec((STRPTR) smui->smui_IconName);

	return 0;
}


static ULONG AddIconMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_AddIcon *smai = (struct SM_AddIcon *) msg;
	ULONG Result;

	Result = DoMethod(iwt->iwt_WindowTask.mt_MainObject,
		SCCM_IconWin_AddIcon,
		SCCM_ADDICON_MAKEXY(smai->smai_x, smai->smai_y),
		smai->smai_DirLock, smai->smai_IconName);

	*((ULONG *) &(smai->smai_x)) = Result;

	return 0;
}


static ULONG RemIconMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_RemIcon *smri = (struct SM_RemIcon *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: Lock=%08lx  IconName=<%s>\n",\
		__FILE__, __FUNC__, __LINE__, smri->smri_DirLock, smri->smri_IconName));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject,
		SCCM_IconWin_RemIcon,
		smri->smri_DirLock,
                smri->smri_IconName);

	return 0;
}


static ULONG WindowSleepMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Sleep);

	return 0;
}


static ULONG WindowWakeupMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_Wakeup *smwu = (struct SM_Wakeup *) msg;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_WakeUp, smwu->smwu_ReLayout);

	return 0;
}


static ULONG TimerReplyMsg(struct internalScaWindowTask *iwt, 	struct Message *msg, APTR p)
{
	struct SM_Timer *smtm = (struct SM_Timer *) msg;

	smtm->smtm_WindowStruct->ws_Flags &= ~WSV_FlagF_TimerMsgSent;

	return 0;
}


static ULONG ARexxRoutineMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_ARexxRoutine *smrx = (struct SM_ARexxRoutine *) msg;

	(*smrx->smrx_EntryPoint)(&iwt->iwt_WindowTask, smrx->smrx_RexxMsg);

	ReplyMsg(&smrx->smrx_RexxMsg->rm_Node);

	return 0;
}


static ULONG ShowPopupMenuMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_ShowPopupMenu *smpm = (struct SM_ShowPopupMenu *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  smpm_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, smpm->smpm_Flags));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ShowPopupMenu,
		smpm->smpm_PopupMenu, smpm->smpm_IconNode, smpm->smpm_Flags, 
		(ULONG) smpm->smpm_Qualifier, smpm->smpm_FileType);

	return 0;
}


static ULONG ShowStatusBarMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_ShowStatusBar *smsb = (struct SM_ShowStatusBar *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  smsb_Visible=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, smsb->smsb_Visible));

	if (smsb->smsb_Visible && NULL == iwt->iwt_StatusBar && !iwt->iwt_BackDrop)
		{
		WORD ExtraHeight = iwt->iwt_GadgetBarHeightBottom;

		d1(kprintf("%s/%s/%ld: StatusBar ON\n", __FILE__, __FUNC__, __LINE__));

		if (StatusBarAdd(iwt))
			{
			// increase window height to make room for the status bar
			SizeWindow(iwt->iwt_WindowTask.wt_Window, 0, iwt->iwt_ExtraHeight - ExtraHeight);
			}
		}
	else if (!smsb->smsb_Visible && NULL != iwt->iwt_StatusBar)
		{
		WORD ExtraHeight = iwt->iwt_GadgetBarHeightBottom;

		d1(kprintf("%s/%s/%ld: StatusBar OFF\n", __FILE__, __FUNC__, __LINE__));

		StatusBarRemove(iwt);

		if (iwt->iwt_WindowTask.wt_Window)
			{
			// Decrease window height 
			SizeWindow(iwt->iwt_WindowTask.wt_Window, 0, iwt->iwt_ExtraHeight - ExtraHeight);
			}
		}

	return 0;
}


static ULONG ShowControlBarMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_ShowControlBar *smcb = (struct SM_ShowControlBar *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  smcb_Visible=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, smsb->smcb_Visible));

	if (smcb->smcb_Visible)
		iwt->iwt_WindowTask.mt_WindowStruct->ws_MoreFlags &= ~WSV_MoreFlagF_NoControlBar;
	else
		iwt->iwt_WindowTask.mt_WindowStruct->ws_MoreFlags |= WSV_MoreFlagF_NoControlBar;

	ControlBarRebuild(iwt);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);

	return 0;
}


static ULONG RedrawIconMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_RedrawIcon *smri = (struct SM_RedrawIcon *) msg;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>  in=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, smri->smri_Icon));

	// make sure the specified icon is still present
	if (ScalosAttemptLockIconListShared(iwt))
		{
		struct ScaIconNode *in;

		for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (in == smri->smri_Icon)
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_DrawIcon, in->in_Icon);
				break;
				}
			}

		ScalosUnLockIconList(iwt);
		}

	return 0;
}


static ULONG RedrawIconObjectMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_RedrawIconObj *smrio = (struct SM_RedrawIconObj *) msg;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>  IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, smrio->smrio_IconObject));

	// make sure the specified icon is still present
	if ((smrio->smrio_Flags & SMRIOFLAGF_IconListLocked) || ScalosAttemptLockIconListShared(iwt))
		{
		struct ScaIconNode *in;

		d1(KPrintF("%s/%s/%ld: smrio_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, smrio->smrio_Flags));

		for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (in->in_Icon == smrio->smrio_IconObject)
				{
				if (smrio->smrio_Flags & SMRIOFLAGF_HightlightOn)
					{
					ULONG IconUserFlags;

					d1(KPrintF("%s/%s/%ld: SMRIOFLAGF_HightlightOn\n", __FILE__, __FUNC__, __LINE__));

					GetAttr(IDTA_UserFlags, in->in_Icon, &IconUserFlags);
					if (IconUserFlags & ICONOBJ_USERFLAGF_DrawHighlite)
						break;

					IconUserFlags |= ICONOBJ_USERFLAGF_DrawHighlite;
					SetAttrs(in->in_Icon,
						IDTA_UserFlags, IconUserFlags,
						TAG_END);
					}
				else if (smrio->smrio_Flags & SMRIOFLAGF_HightlightOff)
					{
					ULONG IconUserFlags;

					d1(KPrintF("%s/%s/%ld: SMRIOFLAGF_HightlightOff\n", __FILE__, __FUNC__, __LINE__));

					GetAttr(IDTA_UserFlags, in->in_Icon, &IconUserFlags);
					if (!(IconUserFlags & ICONOBJ_USERFLAGF_DrawHighlite))
						break;

					IconUserFlags &= ~ICONOBJ_USERFLAGF_DrawHighlite;
					SetAttrs(in->in_Icon,
						IDTA_UserFlags, IconUserFlags,
						TAG_END);
					}

				if ((smrio->smrio_Flags & SMRIOFLAGF_EraseIcon)
					&& IsViewByIcon(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes)
					&& iwt->iwt_WindowTask.wt_Window)
					{
					struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
					ULONG BoundsLeft, BoundsTop, BoundsRight, BoundsBottom;

					d1(KPrintF("%s/%s/%ld: SMRIOFLAGF_EraseIcon\n", __FILE__, __FUNC__, __LINE__));

					BoundsLeft =  gg->BoundsLeftEdge + iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset;
					BoundsTop  = gg->BoundsTopEdge + iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset;
					BoundsRight = BoundsLeft + gg->BoundsWidth - 1;
					BoundsBottom = BoundsTop + gg->BoundsHeight - 1;

					EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
						BoundsLeft, BoundsTop,
						BoundsRight, BoundsBottom);
					}
				if (smrio->smrio_Flags & SMRIOFLAGF_FreeLayout)
					{
					struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

					d1(KPrintF("%s/%s/%ld: SMRIOFLAGF_FreeLayout\n", __FILE__, __FUNC__, __LINE__));
					d1(KPrintF("%s/%s/%ld: LeftEdge=%ld  BoundsLeftEdge=%ld  Flags=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->BoundsLeftEdge, gg->Flags));

					DoMethod(in->in_Icon,
						IDTM_FreeLayout,
						IOFREELAYOUTF_ScreenAvailable);
					DoMethod(iwt->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_LayoutIcon,
						in->in_Icon,
						(gg->Flags & GFLG_SELECTED) 
							? IOLAYOUTF_SelectedImage | IOLAYOUTF_NormalImage
							: IOLAYOUTF_NormalImage);
					}

				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_DrawIcon, in->in_Icon);
				break;
				}
			}

		if (!(smrio->smrio_Flags & SMRIOFLAGF_IconListLocked))
			ScalosUnLockIconList(iwt);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static ULONG DoPopupMenuMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_DoPopupMenu *smdpm = (struct SM_DoPopupMenu *) msg;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_WindowTask.wt_Window)
		{
		WORD MouseX, MouseY;

		MouseX = iInfos.xii_iinfos.ii_Screen->MouseX - iwt->iwt_WindowTask.wt_Window->LeftEdge;
		MouseY = iInfos.xii_iinfos.ii_Screen->MouseY - iwt->iwt_WindowTask.wt_Window->TopEdge;

		d1(KPrintF("%s/%s/%ld: MouseX=%ld  MouseY=%ld\n", __FILE__, __FUNC__, __LINE__, MouseX, MouseY));

		(void) TestPopup(iwt, MouseX, MouseY, smdpm->smdpm_InputEvent.ie_Qualifier);
		}

	return 0;
}


static ULONG PrefsChangedMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_NewPreferences *smnp = (struct SM_NewPreferences *) msg;

	(void) p;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	RememberPrefsChanges(iwt, smnp->smnp_PrefsFlags);

	return 0;
}


static ULONG DeltaMoveMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_DeltaMove *smdm = (struct SM_DeltaMove *) msg;

	(void) p;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
		SCCM_IconWin_DeltaMove, smdm->smdm_DeltaX, smdm->smdm_DeltaY);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, smdm->smdm_AdjustSlider);

	return 0;
}


static ULONG SetThumbnailImageRemappedMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_SetThumbnailImage_Remapped *smtir = (struct SM_SetThumbnailImage_Remapped *) msg;

	(void) p;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	SetIconThumbnailRemapped(iwt, smtir);

	return 0;
}


static ULONG SetThumbnailImageARGBMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_SetThumbnailImage_ARGB *smtia = (struct SM_SetThumbnailImage_ARGB *) msg;

	(void) p;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	SetIconThumbnailARGB(iwt, smtia);

	return 0;
}


static ULONG NewWindowPathMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_NewWindowPath *smnwp = (struct SM_NewWindowPath *) msg;

	(void) p;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject,
		SCCM_IconWin_NewPath,
		smnwp->smnwp_Path,
		smnwp->smnwp_TagList ? TAG_MORE : TAG_IGNORE, smnwp->smnwp_TagList,
		TAG_END);

	FreeCopyString(smnwp->smnwp_Path);
	if (smnwp->smnwp_TagList)
		FreeTagItems(smnwp->smnwp_TagList);

	return 0;
}


static ULONG WindowPrefsChangedMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	UBYTE ActualViewByType;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  iwt_WinTitle=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: ReplyTask=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->mn_ReplyPort->mp_SigTask));
	d1(KPrintF("%s/%s/%ld: ClassName=<%s>\n", __FILE__, __FUNC__, __LINE__, ws->ms_ClassName));

	// Rebuild control bar is necessary
	ControlBarRebuild(iwt);

	if (WSV_Type_DeviceWindow != iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType
			&& !(ws->ws_Flags & WSV_FlagF_TaskSleeps))
		{
		if (0 == strcmp(ws->ms_ClassName, (STRPTR) "IconWindow.sca"))
			ActualViewByType = IDTV_ViewModes_Icon;
		else
			ActualViewByType = IDTV_ViewModes_Name;

		d1(KPrintF("%s/%s/%ld: ActualViewByType=%ld  ws_Viewmodes=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ActualViewByType, ws->ws_Viewmodes));

		if ((IsIwtViewByIcon(iwt) && !IsViewByIcon(ActualViewByType))
			|| (!IsIwtViewByIcon(iwt) && IsViewByIcon(ActualViewByType)))
			{
			// View mode has changed
			UBYTE NewViewByType = ws->ws_Viewmodes;

			d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

			ws->ws_Viewmodes = ActualViewByType;
			ViewWindowBy(iwt, NewViewByType);

			return 0;
			}
		}

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 0;
}


static ULONG WindowRootEventMsg(struct internalScaWindowTask *iwt, struct Message *msg, APTR p)
{
	struct SM_RootEvent *smre = (struct SM_RootEvent *) msg;

	(void) p;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  iwt_WinTitle=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (SCCM_WindowStartComplete == smre->smre_MethodID)
		{
		struct ScaPopChildWindow *spcw;

		d1(KPrintF("%s/%s/%ld:  SCCM_WindowStartComplete  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

		ScalosObtainSemaphore(&iwt->iwt_PopChildListSemaphore);

		for (spcw = (struct ScaPopChildWindow *) iwt->iwt_PopChildList.lh_Head;
			spcw != (struct ScaPopChildWindow *) &iwt->iwt_PopChildList.lh_Tail;
			spcw = (struct ScaPopChildWindow *) spcw->spcw_Node.ln_Succ)
			{
			if (spcw->spcw_EventHandle == smre->smre_EventHandle)
				{
				d1(KPrintF("%s/%s/%ld:  Found EventHandle %08lx\n", __FILE__, __FUNC__, __LINE__, spcw->spcw_EventHandle));
				Remove(&spcw->spcw_Node);

				PopChildWindowDispose(iwt, spcw);
				break;
				}
			}
		
		ScalosReleaseSemaphore(&iwt->iwt_PopChildListSemaphore);
		}

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 0;
}


// return shared Lock to Scalos modules directory
BPTR LockScaModsDir(void)
{
	BPTR ScalosDirLock;
	BPTR oldDir;
	BPTR modsDirLock;

	ScalosDirLock = Lock((STRPTR) CurrentPrefs.pref_ScalosHomeDir, ACCESS_READ);
	if (BNULL == ScalosDirLock)
		return BNULL;

	oldDir = CurrentDir(ScalosDirLock);

	modsDirLock = Lock("Modules", ACCESS_READ);

	CurrentDir(oldDir);
	UnLock(ScalosDirLock);

	return modsDirLock;
}

