// windowtask.c 
// $Date$
// $Revision$


#include <exec/types.h>
#include <dos/dostags.h>
#include <workbench/startup.h>
#include <intuition/gadgetclass.h>
#include <datatypes/pictureclass.h>
#include <utility/tagitem.h>

#define	__USE_SYSBASE

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>

#include "scalos_structures.h"
#include "locale.h"
#include "about.h"
#include "functions.h"
#include "Variables.h"


//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT DefaultColumnWidthChangeHookFunc(struct Hook *hook, Object *o, Msg msg);

//----------------------------------------------------------------------------

// local data items

#define	MIN_WINDOW_WIDTH	92
#define	MIN_WINDOW_HEIGHT	65

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT DefaultColumnWidthChangeHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	(void) hook;
	(void) o;
	(void) msg;

	return 0;
}


SAVEDS(void) INTERRUPT WindowTask(void)
{
///
	struct internalScaWindowTask *iwt;
	struct SM_StartWindow *msgStart = NULL;
	BOOL WinSemaLocked = FALSE;
	BOOL wasMainWindow = FALSE;
	struct ScaWindowStruct *dummyList = NULL;

	ScalosObtainSemaphoreShared(&QuitSemaphore);

	d1(kprintf("%s/%s/%ld: Offset iwt_LastIconUpdateTime=%ld\n", __FILE__, __FUNC__, __LINE__, offsetof(struct internalScaWindowTask, iwt_LastIconUpdateTime)));
	d1(kprintf("%s/%s/%ld: Offset iwt_UpdateSemaphore=%ld\n", __FILE__, __FUNC__, __LINE__, offsetof(struct internalScaWindowTask, iwt_UpdateSemaphore)));

	do	{
		struct ScaWindowStruct *ws;
		BOOL Finished = FALSE;
		enum ScanDirResult sdResult;

		iwt = ScalosAlloc(sizeof(struct internalScaWindowTask));
		if (NULL == iwt)
			break;

		d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
		d1(kprintf("%s/%s/%ld: MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMenu));

		memset(iwt, 0, sizeof(struct internalScaWindowTask));

		ScalosInitSemaphore(&iwt->iwt_ThumbnailIconSemaphore);
		ScalosInitSemaphore(&iwt->iwt_ThumbGenerateSemaphore);
		ScalosInitSemaphore(&iwt->iwt_PopChildListSemaphore);
		ScalosInitSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

		iwt->iwt_WindowTask.wt_PatternInfo.ptinf_BgPen = NO_PEN;

		NewList(&iwt->iwt_ThumbnailIconList);
		NewList(&iwt->iwt_IconDropMarkInfoList);
		NewList(&iwt->iwt_ControlBarMemberList);
		NewList(&iwt->iwt_HistoryList);
		NewList(&iwt->iwt_PopChildList);

		iwt->iwt_BottomScrollerScale = iwt->iwt_SideScrollerScale = 0;

		iwt->iwt_OldFileSys = (struct MsgPort *) ~0;

		iwt->iwt_IDCMPFlags = IDCMP_REFRESHWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_GADGETDOWN 
			| IDCMP_GADGETUP | IDCMP_MENUPICK | IDCMP_CLOSEWINDOW | IDCMP_ACTIVEWINDOW 
			| IDCMP_RAWKEY | IDCMP_CHANGEWINDOW | IDCMP_GADGETHELP | IDCMP_IDCMPUPDATE
			| IDCMP_INACTIVEWINDOW;

#if defined(IDCMP_EXTENDEDMOUSE)
		iwt->iwt_IDCMPFlags |= IDCMP_EXTENDEDMOUSE;
#endif //defined(IDCMP_EXTENDEDMOUSE)
#if defined(__MORPHOS__)
		// enable IDCMP events during dynamic window resizing
		iwt->iwt_IDCMPFlags |= IDCMP_NEWSIZE;
#endif

		iwt->iwt_WindowProcess = (struct Process *) FindTask(NULL);

		while (NULL == (msgStart = (struct SM_StartWindow *) GetMsg(&iwt->iwt_WindowProcess->pr_MsgPort)))
			WaitPort(&iwt->iwt_WindowProcess->pr_MsgPort);

		d1(kprintf("%s/%s/%ld: iwt=%08lx  msgStart=%08lx  ReplyPort=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, iwt, msgStart, msgStart->ScalosMessage.sm_Message.mn_ReplyPort));

		iwt->iwt_WindowTask.mt_WindowStruct = dummyList = ws = msgStart->WindowStruct;

		iwt->iwt_ColumnWidthChangeHook.h_Data = iwt;
		SETHOOKFUNC(iwt->iwt_ColumnWidthChangeHook, DefaultColumnWidthChangeHookFunc);

		iwt->iwt_WindowTask.wt_IconSemaphore = ScalosCreateSemaphore();
		if (NULL == iwt->iwt_WindowTask.wt_IconSemaphore)
			break;

		iwt->iwt_AppListSemaphore = ScalosCreateSemaphore();
		if (NULL == iwt->iwt_AppListSemaphore)
			break;

		iwt->iwt_WindowTask.wt_WindowSemaphore = ScalosCreateSemaphore();
		if (NULL == iwt->iwt_WindowTask.wt_WindowSemaphore)
			break;

		ScalosInitSemaphore(&iwt->iwt_UpdateSemaphore);
		ScalosInitSemaphore(&iwt->iwt_ScanDirSemaphore);
		ScalosInitSemaphore(&iwt->iwt_ChildProcessSemaphore);

		ws->ws_WindowTask = (struct ScaWindowTask *) iwt;

		iwt->iwt_WinScreen = iwt->iwt_OrigWinScreen = iInfos.xii_iinfos.ii_Screen;
		iwt->iwt_WinDrawInfo = iInfos.xii_iinfos.ii_DrawInfo;

		iwt->iwt_CheckOverlappingIcons = (ws->ws_Flags & WSV_FlagF_CheckOverlappingIcons) != 0;

		d1(KPrintF("%s/%s/%ld: ws_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, ws->ws_Flags));
		d1(KPrintF("%s/%s/%ld: iwt_CheckOverlappingIcons=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_CheckOverlappingIcons));

		iwt->iwt_WindowTask.wt_IconPort = ws->ws_MessagePort = CreateMsgPort();
		if (NULL == iwt->iwt_WindowTask.wt_IconPort)
			break;

		if (WSV_Type_DeviceWindow != ws->ws_WindowType)
			{
			struct FileLock *fLock = BADDR(ws->ws_Lock);

			ScalosObtainSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
			iwt->iwt_CurrentHistoryEntry = WindowAddHistoryEntry(iwt, ws->ws_Lock);
			ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);

			iwt->iwt_OldFileSys = SetFileSysTask(fLock->fl_Task);

			iwt->iwt_ReadOnly = !ClassIsDiskWritable(ws->ws_Lock);

			UndoWindowSignalOpening(iwt);
			}

		if (ws->ws_Width < MIN_WINDOW_WIDTH)
			ws->ws_Width = MIN_WINDOW_WIDTH;
		if (ws->ws_Height < MIN_WINDOW_HEIGHT)
			ws->ws_Height = MIN_WINDOW_HEIGHT;

		iwt->iwt_WindowTask.wt_PatternInfo.ptinf_width = ws->ws_Width;
		iwt->iwt_WindowTask.wt_PatternInfo.ptinf_height = ws->ws_Height;

		if (ws->ws_Flags & WSV_FlagF_Backdrop)
			{
			// main window
			iwt->iwt_WindowTask.wt_PatternInfo.ptinf_width = iwt->iwt_WinScreen->Width;
			iwt->iwt_WindowTask.wt_PatternInfo.ptinf_height = iwt->iwt_WinScreen->Height;

			iwt->iwt_WindowTask.wt_XOffset = ws->ws_xoffset = 0;
			iwt->iwt_WindowTask.wt_YOffset = ws->ws_yoffset = 0;

			if (!CurrentPrefs.pref_FullBenchFlag)
				iwt->iwt_WindowTask.wt_PatternInfo.ptinf_height -= iwt->iwt_WinScreen->BarHeight + 1;
			}
		SetBackFill(iwt, ws->ws_PatternNode, &iwt->iwt_WindowTask.wt_PatternInfo, 0, iwt->iwt_WinScreen);

		iwt->iwt_TitleObject = SCA_NewScalosObjectTags("Title.sca",
				SCCA_Title_Type, SCCV_Title_Type_Window,
				SCCA_Title_Format, (ULONG) (ws->ws_Title ? ws->ws_Title : (STRPTR) "Scalos"),
				SCCA_WindowTask, (ULONG) iwt,
				TAG_END);
		d1(kprintf("%s/%s/%ld: TitleObject=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_TitleObject));
		if (NULL == iwt->iwt_TitleObject)
			break;

		if (DoMethod(iwt->iwt_TitleObject, SCCM_Title_QueryTitle))
			{
			iwt->iwt_IDCMPFlags |= IDCMP_DISKINSERTED | IDCMP_DISKREMOVED;
			iwt->iwt_NeedsTimerFlag = TRUE;
			ws->ws_Flags |= WSV_FlagF_NeedsTimerMsg;
			}
		iwt->iwt_WinTitle = (STRPTR) DoMethod(iwt->iwt_TitleObject, SCCM_Title_Generate);

		if (IsIwtViewByIcon(iwt))
			{
			iwt->iwt_WindowTask.wt_XOffset = ws->ws_xoffset;
			iwt->iwt_WindowTask.wt_YOffset = ws->ws_yoffset;
			}

		iwt->iwt_WindowTask.mt_WindowObject = SCA_NewScalosObjectTags("Window.sca",
				SCCA_WindowTask, (ULONG) iwt,
				TAG_END);
		d1(kprintf("%s/%s/%ld: WindowObject=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.mt_WindowObject));
		if (NULL == iwt->iwt_WindowTask.mt_WindowObject)
			break;

		iwt->iwt_WindowTask.mt_MainObject = SCA_NewScalosObjectTags(ws->ms_ClassName,
				SCCA_WindowTask, (ULONG) iwt,
				TAG_END);
		d1(kprintf("%s/%s/%ld: MainObject=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.mt_MainObject));
		if (NULL == iwt->iwt_WindowTask.mt_MainObject)
			break;

		if (CurrentPrefs.pref_DisplayStatusBar && (CurrentPrefs.pref_iCandy > 1) && !(ws->ws_Flags & (WSV_FlagF_NoStatusBar | WSV_FlagF_Backdrop)))
			{
			if (!StatusBarAdd(iwt))
				break;
			}

		if (!(ws->ws_Flags & WSV_FlagF_Backdrop) && !(ws->ws_MoreFlags & WSV_MoreFlagF_NoControlBar))
			{
			if (!ControlBarAdd(iwt))
				{
				// Disable browser mode if control bar creation failed.
				ws->ws_Flags &= ~WSV_FlagF_BrowserMode;
				}
			}

		d1(KPrintF("%s/%s/%ld: ws_ThumbnailView=%ld\n", __FILE__, __FUNC__, __LINE__, ws->ws_ThumbnailView));

		SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
			SCCA_IconWin_ShowType, ws->ws_ViewAll,
			SCCA_IconWin_ThumbnailView, ws->ws_ThumbnailView,
			TAG_END);

		iwt->iwt_ThumbnailsLifetimeDays = ws->ws_ThumbnailsLifetimeDays;
		
		SetIconWindowReadOnly(iwt, iwt->iwt_ReadOnly);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (ws->ws_Flags & WSV_FlagF_Iconify)
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			// Reply task startup message
			ReplyMsg(&msgStart->ScalosMessage.sm_Message);
			msgStart = NULL;

			if (!DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Iconify))
				{
				ws->ws_Flags &= ~WSV_FlagF_Iconify;
				iwt->iwt_IconifyFlag = FALSE;

				if (!DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Open))
					{
					d1(kprintf("%s/%s/%ld: SCCM_Window_Open failed.\n", __FILE__, __FUNC__, __LINE__));
					break;
					}

				d1(kprintf("%s/%s/%ld: wt_Window=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

				ws->ws_Window = iwt->iwt_WindowTask.wt_Window;
				}
			}
		else
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			if (!DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Open))
				{
				d1(kprintf("%s/%s/%ld: SCCM_Window_Open failed.\n", __FILE__, __FUNC__, __LINE__));
				break;
				}

			d1(kprintf("%s/%s/%ld: wt_Window=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

			ws->ws_Window = iwt->iwt_WindowTask.wt_Window;

			ReplyMsg(&msgStart->ScalosMessage.sm_Message);
			msgStart = NULL;
			}

		// move <ws> into official window list
		SCA_LockWindowList(SCA_LockWindowList_Exclusiv);
		SCA_MoveWSNode(&dummyList, &winlist.wl_WindowStruct, iwt->iwt_WindowTask.mt_WindowStruct);
		SCA_UnLockWindowList();

		d1(kprintf("%s/%s/%ld: wt_Window=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetInnerSize);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (iwt->iwt_WindowTask.wt_Window)
			{
			Scalos_SetFont(iwt->iwt_WindowTask.wt_Window->RPort, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);

			SetABPenDrMd(iwt->iwt_WindowTask.wt_Window->RPort, 
				FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen,
                                FontPrefs.fprf_TextDrawMode);

			d1(kprintf("%s/%s/%ld: MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMenu));

			SafeSetMenuStrip(iwt->iwt_WindowTask.wt_Window);
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		SetMenuOnOff(iwt);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		iwt->iwt_RemRegion = (struct Region *) DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_InitClipRegion);
		d1(kprintf("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));

		d1(KPrintF("%s/%s/%ld: iwt=%08lx  ws=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.mt_WindowStruct));

		sdResult = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);

		d1(KPrintF("%s/%s/%ld: SCCM_IconWin_Update Result = %08lx\n", __FILE__, __FUNC__, __LINE__, sdResult));
		if (ScanDirIsError(sdResult))
			break;	// error or user abort reading icons

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		UpdateIconCount(iwt);

		SetMenuOnOff(iwt);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_WindowStartComplete);

		do	{
			d1(KPrintF("%s/%s/%ld: iwt=%08lx  Finished=%ld  iwt_CloseWindow=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, Finished, iwt->iwt_CloseWindow));

			if (iwt->iwt_CloseWindow)
				Delay(2);
			else
				WaitPort(iwt->iwt_WindowTask.wt_IconPort);

			if (DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_HandleMessages))
				{
				// Window is going to be closed
				Finished = TRUE;

				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				FlushThumbnailEntries(iwt);

				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				if (iwt->iwt_PopupMenuPending)
					{
					if (iwt->iwt_PopupMenuPending->mpm_Flags & SHOWPOPUPFLGF_WinListLocked)
						SCA_UnLockWindowList();
					if (iwt->iwt_PopupMenuPending->mpm_Flags & SHOWPOPUPFLGF_IconSemaLocked)
						ScalosUnLockIconList(iwt);

					ScalosFree(iwt->iwt_PopupMenuPending);
					}

				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				if (ScaAttemptSemaphoreList(SCA_SemaphoreExclusive, (ULONG) iwt->iwt_WindowTask.wt_WindowSemaphore,
						SCA_SemaphoreExclusive, (ULONG) &iwt->iwt_ChildProcessSemaphore,
						TAG_END))
					{
					WinSemaLocked = TRUE;

					d1(KPrintF("%s/%s/%ld: mt_WindowStruct=%08lx  ii_MainWindowStruct=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.mt_WindowStruct, iInfos.xii_iinfos.ii_MainWindowStruct));

					if (iwt->iwt_WindowTask.mt_WindowStruct == iInfos.xii_iinfos.ii_MainWindowStruct)
						{
						// "Quit Scalos" requested

						if (MainWindowTask->emulation)
							{
							UseAsyncRequestArgs(iwt,
								MSGID_CANNOT_QUIT_WBREPLACEMENT,
								MSGID_OKNAME,
								0);

							ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);
							ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
							WinSemaLocked = FALSE;
							Finished = FALSE;
							iwt->iwt_CloseWindow = FALSE;
							}
						else
							{
							d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, ScalosBase->scb_LibNode.lib_OpenCnt));

							// force flushing of all libraries/devices
							(void) AllocMem(INT_MAX, MEMF_ANY);

							d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, ScalosBase->scb_LibNode.lib_OpenCnt));

							// Add 1 to ScaLibPluginOpenCount because Scalos itself opens the library once!
							while (Finished && (ScalosBase->scb_LibNode.lib_OpenCnt > (1 + ScaLibPluginOpenCount)))
								{
								d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

								UseAsyncRequestArgs(iwt,
									MSGID_CANNOT_QUIT_LIBOPEN, 
									MSGID_GADGETSNAME, 
									1, (ScalosBase->scb_LibNode.lib_OpenCnt - 1) << 16);

								d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

								if (0 == WaitReply(iwt->iwt_WindowTask.wt_IconPort, iwt, MTYP_Requester))
									{
									d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

									ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);
									ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
									WinSemaLocked = FALSE;
									Finished = FALSE;
									iwt->iwt_CloseWindow = FALSE;
									}
								d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
								}

							d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, ScalosBase->scb_LibNode.lib_OpenCnt));
							}
						}
					}
				else
					{
					// Cannot lock semaphores - perhaps some child process is still running
					// defer quit until child process has finished!
					Finished = FALSE;
					}
				d1(KPrintF("%s/%s/%ld: iwt_IconPortOutstanding=%ld  iwt_AsyncLayoutPending=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconPortOutstanding, iwt->iwt_AsyncLayoutPending));

				if (Finished && (iwt->iwt_IconPortOutstanding || iwt->iwt_AsyncLayoutPending))
					{
					ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);
					ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
					}
				}
			} while ((!WinSemaLocked && !Finished) || iwt->iwt_IconPortOutstanding || iwt->iwt_AsyncLayoutPending);
		} while (0);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (iwt)
		{
		struct WindowHistoryEntry *whe;
		struct ScaPopChildWindow *spcw;
		ULONG n;

		UndoWindowSignalClosing(iwt);

		d1(kprintf("%s/%s/%ld: iwt_AsyncLayoutPending=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_AsyncLayoutPending));

		InvalidateLastIwtUnderPointer(iwt);

		while (iwt->iwt_AsyncLayoutPending)
			{
			// we need to continue handling messages because iwt_AsyncLayoutPending 
			// is cleared within message reply handler
			(void) DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_CheckForMessages);
			Delay(2);
			}

		d1(kprintf("%s/%s/%ld: iwt_AsyncLayoutPending=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_AsyncLayoutPending));

		ScalosObtainSemaphore(&iwt->iwt_ChildProcessSemaphore);
		ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);

		// make sure whe_IconList is not freed twice
		if (iwt->iwt_CurrentHistoryEntry)
			iwt->iwt_CurrentHistoryEntry->whe_IconList = NULL;

		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_RemClipRegion, iwt->iwt_RemRegion);
		iwt->iwt_RemRegion = NULL;
		d1(kprintf("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (iInfos.xii_GlobalGadgetUnderPointer.ggd_iwt == iwt)
			{
			iInfos.xii_GlobalGadgetUnderPointer.ggd_iwt = NULL;
			iInfos.xii_GlobalGadgetUnderPointer.ggd_Gadget = NULL;
			iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID = SGTT_GADGETID_unknown;
			}

		// Dispose icon overlays
		for (n = 0; n < Sizeof(iwt->iwt_IconOverlays); n++)
			{
			DisposeDatatypesImage(&iwt->iwt_IconOverlays[n]);
			}
		if (iwt->iwt_PopupMenuPending)
			ScalosFree(iwt->iwt_PopupMenuPending);
		if (iwt->iwt_myAppObj)
			{
			SCA_RemoveAppObject(iwt->iwt_myAppObj);
			DisposeIconObject(iwt->iwt_myAppIcon);
			}
		if (WinSemaLocked)
			{
			ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
			ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);
			}

		// Dispose all entries in window history list
		while ((whe = (struct WindowHistoryEntry *) RemHead(&iwt->iwt_HistoryList)))
			{
			WindowHistoryEntryDispose(iwt, whe);
			}
		iwt->iwt_CurrentHistoryEntry = NULL;

		// Dispose all entries in iwt_PopChildList
		while ((spcw = (struct ScaPopChildWindow *) RemHead(&iwt->iwt_PopChildList)))
			{
			d1(KPrintF("%s/%s/%ld:  spcw=%08lx\n", __FILE__, __FUNC__, __LINE__, spcw));
			PopChildWindowDispose(iwt, spcw);
			}

		if (iwt->iwt_WindowTask.mt_WindowObject)
			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Close);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		wasMainWindow = DisposeScalosWindow(iwt, dummyList ? &dummyList : &winlist.wl_WindowStruct);	// frees ScaWindowStruct !!
		iwt->iwt_WindowTask.mt_WindowStruct = NULL;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (iwt->iwt_WindowTask.mt_WindowObject)
			{
			SCA_DisposeScalosObject(iwt->iwt_WindowTask.mt_WindowObject);
			}

		FreeBackFill(&iwt->iwt_WindowTask.wt_PatternInfo);

		if (iwt->iwt_WindowTask.wt_IconSemaphore)
			{
			ScalosLockIconListExclusive(iwt);

			FreeIconList(iwt, &iwt->iwt_WindowTask.wt_IconList);
			FreeIconList(iwt, &iwt->iwt_WindowTask.wt_LateIconList);

			ScalosUnLockIconList(iwt);
			}

		StatusBarRemove(iwt);
		ControlBarRemove(iwt);

		if (iwt->iwt_WindowTask.mt_MainObject)
			{
			SCA_DisposeScalosObject(iwt->iwt_WindowTask.mt_MainObject);
			iwt->iwt_WindowTask.mt_MainObject = NULL;
			}

		if (iwt->iwt_WindowTask.wt_IconPort)
			{
			struct MsgPort *iconPort = iwt->iwt_WindowTask.wt_IconPort;

			Forbid();

			iwt->iwt_WindowTask.wt_IconPort = NULL;
			if (iwt->iwt_WindowTask.mt_WindowStruct)
				iwt->iwt_WindowTask.mt_WindowStruct->ws_MessagePort = NULL;

			ClearMsgPort(iconPort);
			DeleteMsgPort(iconPort);

			Permit();
			}
		if (iwt->iwt_WindowTask.wt_IconSemaphore)
			{
			ScalosDeleteSemaphore(iwt->iwt_WindowTask.wt_IconSemaphore);
			iwt->iwt_WindowTask.wt_IconSemaphore = NULL;
			}
		if (iwt->iwt_AppListSemaphore)
			{
			ScalosDeleteSemaphore(iwt->iwt_AppListSemaphore);
			iwt->iwt_AppListSemaphore = NULL;
			}
		if (iwt->iwt_WindowTask.wt_WindowSemaphore)
			{
			ScalosDeleteSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
			iwt->iwt_WindowTask.wt_WindowSemaphore = NULL;
			}

		if (((struct MsgPort *) ~0) != iwt->iwt_OldFileSys)
			SetFileSysTask(iwt->iwt_OldFileSys);

		if (iwt->iwt_TitleObject)
			{
			SCA_DisposeScalosObject(iwt->iwt_TitleObject);
			iwt->iwt_TitleObject = NULL;
			}
		if (iwt->iwt_AslFileRequest)
			{
			FreeAslRequest(iwt->iwt_AslFileRequest);
			iwt->iwt_AslFileRequest = NULL;
			}
		if (iwt->iwt_WindowTask.mt_WindowStruct)
			iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowTask = NULL;

		CleanupDropMarkInfo(&iwt->iwt_WindowDropMarkInfo);
		CleanupDropMarkInfoList(&iwt->iwt_IconDropMarkInfoList);

		d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

		ScalosFree(iwt);
		}

	d1(KPrintF("%s/%s/%ld: wasMainWindow=%ld\n", __FILE__, __FUNC__, __LINE__, wasMainWindow));

	if (wasMainWindow)
		{
		struct SM_AsyncRoutine *msg = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine, 0);

		d1(KPrintF("%s/%s/%ld: msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

		if (msg)
			{
			msg->smar_EntryPoint = (ASYNCROUTINEFUNC) CloseWindowMsg;

			PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &msg->ScalosMessage.sm_Message);
			}
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (msgStart)
		ReplyMsg(&msgStart->ScalosMessage.sm_Message);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &dummyList);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ScalosReleaseSemaphore(&QuitSemaphore);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
///
}


