// Functions.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local data items

// remapping of SCA_OpenIconWindow() tags to ws_Flags values
static const struct TagItem FlagMapTable[] =
	{
	{ SCA_Iconify, WSV_FlagF_Iconify },
	{ SCA_NoStatusBar, WSV_FlagF_NoStatusBar },
	{ SCA_NoActivateWindow, WSV_FlagF_NoActivateWindow },
	{ SCA_BrowserMode, WSV_FlagF_BrowserMode },
	{ SCA_CheckOverlappingIcons, WSV_FlagF_CheckOverlappingIcons },
	{ SCA_DdPopupWindow, WSV_FlagF_DdPopupWindow },
	{ TAG_END, 0 },
	};

// remapping of SCA_OpenIconWindow() tags to ws_MoreFlags values
static const struct TagItem MoreFlagMapTable[] =
	{
	{ SCA_NoControlBar, WSV_MoreFlagF_NoControlBar },
	{ TAG_END, 0 },
	};

//----------------------------------------------------------------------------

// local functions

static Object *ReadDeviceIconObject(CONST_STRPTR Path);
static Object *FindIconInScalosWindows(BPTR Lock, CONST_STRPTR IconName,
	struct ScaWindowStruct **wsLocked, BOOL *windowListLocked);
static Object *FindIconInDeviceWindow(CONST_STRPTR IconName,
	struct ScaWindowStruct **wsLocked, BOOL *windowListLocked);
static BOOL MatchDeviceIcon(CONST_STRPTR SearchPath, struct ScaIconNode *in);
static BOOL MatchPath(CONST_STRPTR VolPath, CONST_STRPTR DevIconPath);

//----------------------------------------------------------------------------

// implementation of SCA_OpenIconWindow()

LIBFUNC_P2(BOOL, sca_OpenIconWindow,
	A0, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase, 0)
{
	Object *IconObj = NULL;
	Object *allocIconObj = NULL;
	struct MsgPort *allocMsgPort = NULL;
	struct ScaWindowStruct *wsNew = NULL;
	struct ScaWindowStruct *dummyList = NULL;
	struct SM_StartWindow *msgStart = NULL;
	BOOL Success = FALSE;
	struct ScaWindowStruct *wsIconListLocked = NULL;
	BOOL WindowListLocked = FALSE;

	(void)ScalosBase;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		STATIC_PATCHFUNC(WindowTask)
		struct ScaWindowStruct *ws;
		struct ScaWindowStruct **ReturnWs;
		struct IBox *WindowRect;
		struct ScaIconNode *in;
		struct MsgPort *ReplyPort;
		STRPTR Path;
		STRPTR TaskName;
		ULONG WindowFlags;
		CONST_STRPTR clp;
		BOOL Found;
		size_t length;

		ReturnWs = (struct ScaWindowStruct **) GetTagData(SCA_WindowStruct, (ULONG)NULL, TagList);
		d1(KPrintF("%s/%s/%ld: ReturnWs=%08lx\n", __FILE__, __FUNC__, __LINE__, ReturnWs));

		in = (struct ScaIconNode *) GetTagData(SCA_IconNode, (ULONG)NULL, TagList);
		if (in)
			IconObj = in->in_Icon;

		IconObj = (Object *) GetTagData(SCA_IconObject, (ULONG) IconObj, TagList);

		d1(KPrintF("%s/%s/%ld: in=%08lx  IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, in, IconObj));

		WindowFlags = GetTagData(SCA_Flags, 0, TagList);

		ReplyPort = (struct MsgPort *) GetTagData(SCA_MessagePort, (ULONG)NULL, TagList);
		d1(KPrintF("%s/%s/%ld: ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, ReplyPort));
		if (NULL == ReplyPort)
			ReplyPort = allocMsgPort = CreateMsgPort();
		if (NULL == ReplyPort)
			break;

		wsNew = (struct ScaWindowStruct *) SCA_AllocStdNode((struct ScalosNodeList *)(APTR) &dummyList, NTYP_WindowStruct);
		if (NULL == wsNew)
			break;

		wsNew->ws_Left = CurrentPrefs.pref_DefWindowBox.Left;
		wsNew->ws_Top = CurrentPrefs.pref_DefWindowBox.Top;
		wsNew->ws_Width = CurrentPrefs.pref_DefWindowBox.Width;
		wsNew->ws_Height = CurrentPrefs.pref_DefWindowBox.Height;

		wsNew->ws_WindowOpacityActive = CurrentPrefs.pref_ActiveWindowTransparency;
		wsNew->ws_WindowOpacityInactive = CurrentPrefs.pref_InactiveWindowTransparency;

		wsNew->ws_IconSizeConstraints = CurrentPrefs.pref_IconSizeConstraints;
		wsNew->ws_IconScaleFactor = CurrentPrefs.pref_IconScaleFactor;

		Path = (STRPTR) GetTagData(SCA_Path, (ULONG)NULL, TagList);
		d1(KPrintF("%s/%s/%ld: Path=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, Path, Path ? Path : (STRPTR) ""));
		if (Path)
			{
			BPTR parentLock;

			wsNew->ws_Lock = Lock(Path, ACCESS_READ);
			debugLock_d1(wsNew->ws_Lock);
			if ((BPTR)NULL == wsNew->ws_Lock)
				break;

			wsNew->ws_Name = GetWsNameFromLock(wsNew->ws_Lock);
			d1(KPrintF("%s/%s/%ld: ws_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, wsNew->ws_Name));

			IconObj = FunctionsFindIconObjectForPath(Path, &WindowListLocked,
				&wsIconListLocked, &allocIconObj);

			parentLock = ParentDir(wsNew->ws_Lock);
			debugLock_d1(parentLock);
			if (parentLock)
				{
				UnLock(parentLock);
				}
			else
				{
				// this is a root window
				wsNew->ws_Flags |= WSV_FlagF_RootWindow;
				}
			}

		d1(KPrintF("%s/%s/%ld: IconObj=%08lx  ws_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj, wsNew->ws_Name));

		WindowRect = (struct IBox *) GetTagData(SCA_WindowRect, (ULONG)NULL, TagList);
		d1(KPrintF("%s/%s/%ld: WindowRect=%08lx\n", __FILE__, __FUNC__, __LINE__, WindowRect));
		if (NULL == WindowRect)
			GetAttr(IDTA_WindowRect, IconObj, (APTR) &WindowRect);

		d1(kprintf("%s/%s/%ld: WindowRect=%08lx\n", __FILE__, __FUNC__, __LINE__, WindowRect));
		if (WindowRect)
			{
			wsNew->ws_Left = WindowRect->Left;
			wsNew->ws_Top = WindowRect->Top;
			wsNew->ws_Width = WindowRect->Width;
			wsNew->ws_Height = WindowRect->Height;
			}

		wsNew->ws_xoffset = wsNew->ws_yoffset = 0;
		wsNew->ws_Viewmodes = GetTagData(SCA_ViewModes, IDTV_ViewModes_Default, TagList);

		wsNew->ws_ThumbnailView = CurrentPrefs.pref_ShowThumbnails;

		// determine default pattern number
		if (IsViewByIcon(wsNew->ws_Viewmodes))
			wsNew->ws_PatternNumber	= PATTERNNR_IconWindowDefault;	 // default icon window pattern no.
		else
			wsNew->ws_PatternNumber	= PATTERNNR_TextWindowDefault;	 // default text window pattern no.

		wsNew->ws_ThumbnailsLifetimeDays = CurrentPrefs.pref_ThumbnailMaxAge;

		wsNew->ws_Flags = PackBoolTags(wsNew->ws_Flags, TagList, (struct TagItem *) FlagMapTable);
		wsNew->ws_MoreFlags = PackBoolTags(wsNew->ws_MoreFlags, TagList, (struct TagItem *) MoreFlagMapTable);

		d1(KPrintF("%s/%s/%ld: ws_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, wsNew->ws_Flags));

		if (IconObj)
			{
			struct IconWindowProperties iwp;

			iwp.iwp_SortOrder = wsNew->ws_SortOrder;
			iwp.iwp_ThumbnailView = wsNew->ws_ThumbnailView;
			iwp.iwp_ThumbnailsLifetimeDays = wsNew->ws_ThumbnailsLifetimeDays;
			iwp.iwp_ShowAll = wsNew->ws_ViewAll;
			iwp.iwp_Viewmodes = wsNew->ws_Viewmodes;
			iwp.iwp_PatternNumber = wsNew->ws_PatternNumber;
			iwp.iwp_CheckOverlap = (wsNew->ws_Flags & WSV_FlagF_CheckOverlappingIcons) != 0;
			iwp.iwp_XOffset = iwp.iwp_YOffset = 0;
			iwp.iwp_OpacityActive = wsNew->ws_WindowOpacityActive;
			iwp.iwp_OpacityInactive = wsNew->ws_WindowOpacityInactive;
			iwp.iwp_IconSizeConstraints = wsNew->ws_IconSizeConstraints;
			iwp.iwp_IconScaleFactor = wsNew->ws_IconScaleFactor;

			FunctionsGetSettingsFromIconObject(&iwp, IconObj);

			wsNew->ws_SortOrder = iwp.iwp_SortOrder;
			wsNew->ws_ViewAll = iwp.iwp_ShowAll;
			wsNew->ws_ThumbnailsLifetimeDays = iwp.iwp_ThumbnailsLifetimeDays;
			wsNew->ws_PatternNumber = iwp.iwp_PatternNumber;
			wsNew->ws_Viewmodes = iwp.iwp_Viewmodes;
			wsNew->ws_ThumbnailView = iwp.iwp_ThumbnailView;
			wsNew->ws_WindowOpacityActive = iwp.iwp_OpacityActive;
			wsNew->ws_WindowOpacityInactive = iwp.iwp_OpacityInactive;
			wsNew->ws_IconSizeConstraints = iwp.iwp_IconSizeConstraints;
			wsNew->ws_IconScaleFactor = iwp.iwp_IconScaleFactor;

			if (iwp.iwp_NoStatusBar)
				wsNew->ws_Flags |= WSV_FlagF_NoStatusBar;
			if (iwp.iwp_NoControlBar)
				wsNew->ws_MoreFlags |= WSV_MoreFlagF_NoControlBar;
			if (iwp.iwp_BrowserMode)
				wsNew->ws_Flags |= WSV_FlagF_BrowserMode;

			if (iwp.iwp_CheckOverlap)
				wsNew->ws_Flags |= WSV_FlagF_CheckOverlappingIcons;
			else
				wsNew->ws_Flags &= ~WSV_FlagF_CheckOverlappingIcons;
			}

		// SCA_ShowAllMode and SCA_ShowAllFiles tags explicitly given
		// take precedence over window/icon settings
		wsNew->ws_ViewAll = GetTagData(SCA_ShowAllMode, wsNew->ws_ViewAll, TagList);
		if (NULL == FindTagItem(SCA_ShowAllMode, TagList)
				&& FindTagItem(SCA_ShowAllFiles, TagList))
			{
			wsNew->ws_ViewAll = GetTagData(SCA_ShowAllFiles, FALSE, TagList)
				? DDFLAGS_SHOWALL : DDFLAGS_SHOWDEFAULT;
			}

		if (IsShowAll(wsNew))
			wsNew->ws_Flags |= WSV_FlagF_ShowAllFiles;
		else
			wsNew->ws_Flags &= ~WSV_FlagF_ShowAllFiles;

		wsNew->ws_xoffset = GetTagData(SCA_XOffset, wsNew->ws_xoffset, TagList);
		wsNew->ws_yoffset = GetTagData(SCA_YOffset, wsNew->ws_yoffset, TagList);

		wsNew->ws_PatternNumber = GetTagData(SCA_PatternNumber, wsNew->ws_PatternNumber, TagList);
		wsNew->ws_PatternNode = GetPatternNode(wsNew->ws_PatternNumber, NULL);

		wsNew->ws_Viewmodes = GetTagData(SCA_ViewModes, wsNew->ws_Viewmodes, TagList);

		wsNew->ws_WindowOpacityActive = GetTagData(SCA_TransparencyActive, wsNew->ws_WindowOpacityActive, TagList);
		wsNew->ws_WindowOpacityInactive = GetTagData(SCA_TransparencyInactive, wsNew->ws_WindowOpacityInactive, TagList);

		d1(kprintf("%s/%s/%ld: ws_ViewModes=%ld\n", __FILE__, __FUNC__, __LINE__, wsNew->ws_Viewmodes));
		d1(KPrintF("%s/%s/%ld: ws_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, wsNew->ws_Flags));

		if (IsViewByIcon(wsNew->ws_Viewmodes))
			wsNew->ms_ClassName = (STRPTR) "IconWindow.sca";
		else
			wsNew->ms_ClassName = (STRPTR) "TextWindow.sca";

		wsNew->ms_ClassName = (STRPTR) GetTagData(SCA_ClassName, (ULONG) wsNew->ms_ClassName, TagList);

		wsNew->ms_Class = (Class *) GetTagData(SCA_Class, (ULONG)NULL, TagList);
		if (wsNew->ms_Class)
			wsNew->ms_ClassName = NULL;

		if ((BPTR)NULL == wsNew->ws_Lock)
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			if (in)
				{
				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				if (in->in_Lock)
					{
					// .withlock:
					BPTR oldDir;

					if (NULL == in->in_Name)
						break;

					wsNew->ws_Name = AllocCopyString(in->in_Name);
					if (NULL == wsNew->ws_Name)
						break;

					oldDir = CurrentDir(in->in_Lock);

					wsNew->ws_Lock = Lock(in->in_Name, ACCESS_READ);

					CurrentDir(oldDir);

					if ((BPTR)NULL == wsNew->ws_Lock)
						break;
					}
				else
					{
					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					if (in->in_DeviceIcon)
						{
						wsNew->ws_Lock = DiskInfoLock(in);
						debugLock_d1(wsNew->ws_Lock);
						if ((BPTR)NULL == wsNew->ws_Lock)
							break;

						wsNew->ws_Name = AllocCopyString(in->in_DeviceIcon->di_Volume);
						d1(kprintf("%s/%s/%ld: ws_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, wsNew->ws_Name));
						if (NULL == wsNew->ws_Name)
							break;

						StripTrailingColon(wsNew->ws_Name);

						wsNew->ws_Flags |= WSV_FlagF_RootWindow;
						}
					else
						{
						// .onlyicon:
						if (NULL == in->in_Name)
							break;

						wsNew->ws_Lock = Lock(in->in_Name, ACCESS_READ);
						debugLock_d1(wsNew->ws_Lock);
						if ((BPTR)NULL == wsNew->ws_Lock)
							break;

						wsNew->ws_Name = AllocCopyString(in->in_Name);
						d1(kprintf("%s/%s/%ld: ws_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, wsNew->ws_Name));
						if (NULL == wsNew->ws_Name)
							break;
						}
					}
				}
			else
				{
				// .noiconnode:
				STRPTR iconName = NULL;
				ULONG IconType;

				d1(kprintf("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

				if (NULL == IconObj)
					break;

				GetAttr(DTA_Name, IconObj, (APTR) &iconName);
				if (NULL == iconName)
					break;
				if ('\0' == *iconName)
					break;

				d1(kprintf("%s/%s/%ld: iconName=<%s> \n", __FILE__, __FUNC__, __LINE__, iconName));

				GetAttr(IDTA_Type, IconObj, &IconType);

				if (':' == iconName[strlen(iconName) - 1])
					IconType = WBDISK;

				d1(kprintf("%s/%s/%ld: IconType=%ld\n", __FILE__, __FUNC__, __LINE__, IconType));

				if (WBDISK == IconType)
					{
					wsNew->ws_Name = AllocCopyString(iconName);

					if (NULL == wsNew->ws_Name)
						break;

					StripTrailingColon(wsNew->ws_Name);

					wsNew->ws_Flags |= WSV_FlagF_RootWindow;
					}
				else
					{
					wsNew->ws_Name = AllocCopyString(FilePart(iconName));
					if (NULL == wsNew->ws_Name)
						break;
					}

				wsNew->ws_Lock = Lock(iconName, ACCESS_READ);
				debugLock_d1(wsNew->ws_Lock);
				if ((BPTR)NULL == wsNew->ws_Lock)
					break;
				}
			}
		// .lockok:
		d1(kprintf("%s/%s/%ld: ws_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, wsNew->ws_Name));

		if (wsIconListLocked)
			{
			d1(kprintf("\n" "%s/%s/%ld:  wsLocked=%08lx\n", __FILE__, __FUNC__, __LINE__, wsIconListLocked));
			ScalosReleaseSemaphore(wsIconListLocked->ws_WindowTask->wt_IconSemaphore);
			wsIconListLocked = NULL;
			}
		if (WindowListLocked)
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			SCA_UnLockWindowList();
			WindowListLocked = FALSE;
			}

		if (wsNew->ws_Flags & WSV_FlagF_RootWindow)
			clp = CurrentPrefs.pref_MainWindowTitle;
		else
			clp = CurrentPrefs.pref_StandardWindowTitle;

		wsNew->ws_Title = AllocCopyString((STRPTR) GetTagData(SCA_WindowTitle, (ULONG) clp, TagList));
		if (NULL == wsNew->ws_Title)
			break;

		Found = FALSE;
		SCA_LockWindowList(SCA_LockWindowList_Shared);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		debugLock_d1(wsNew->ws_Lock);

		for (ws=winlist.wl_WindowStruct; !Found && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (ws != wsNew && ws->ws_Lock &&
				LOCK_SAME == ScaSameLock(wsNew->ws_Lock, ws->ws_Lock))
				{
				Found = TRUE;

				if (ws->ws_Flags & WSV_FlagF_Iconify)
					{
					struct ScalosMessage *msg = SCA_AllocMessage(MTYP_UnIconify, 0);

					if (msg)
						{
						d1(kprintf("%s/%s/%ld: PutMsg iwt=%08lx  Msg=%08lx \n", __FILE__, __FUNC__, __LINE__, ws->ws_WindowTask, &msg->sm_Message));
						PutMsg(ws->ws_MessagePort, &msg->sm_Message);
						}
					}
				else
					{
					if (ws->ws_Window)
						{
						ActivateWindow(ws->ws_Window);
						WindowToFront(ws->ws_Window);
						}
					}
				}
			}

		d1(kprintf("%s/%s/%ld: Found=%ld\n", __FILE__, __FUNC__, __LINE__, Found));

		SCA_UnLockWindowList();

		if (Found)
			{
			// requested Scalos window for drawer is already open
			Success = TRUE;
			break;
			}

		msgStart = (struct SM_StartWindow *) SCA_AllocMessage(MTYP_StartWindow, 0);
		d1(kprintf("%s/%s/%ld: msgStart=%08lx\n", __FILE__, __FUNC__, __LINE__, msgStart));
		if (NULL == msgStart)
			break;

		msgStart->ScalosMessage.sm_Message.mn_ReplyPort = ReplyPort;
		msgStart->WindowStruct = wsNew;

		length = 50 + strlen(wsNew->ws_Name);

		wsNew->ws_WindowTaskName = ScalosAlloc(length);
		if (wsNew->ws_WindowTaskName)
			{
			TaskName = wsNew->ws_WindowTaskName;

			ScaFormatStringMaxLength(TaskName, length,
				"Scalos_Window_Task <%s>", (ULONG) wsNew->ws_Name);
			}
		else
			{
			TaskName = (STRPTR) "Scalos_Window_Task";
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		wsNew->ws_Task = CreateNewProcTags(
				NP_WindowPtr, ~0,
				NP_StackSize, CurrentPrefs.pref_DefaultStackSize,
				NP_Priority, 1,
				NP_Cli, TRUE,
				NP_CommandName, (ULONG) TaskName,
				NP_Name, (ULONG) TaskName,
				NP_Entry, (ULONG) PATCH_NEWFUNC(WindowTask),
				NP_CurrentDir, wsNew->ws_Lock,
				NP_Path, DupWBPathList(),
				TAG_END);

		d1(kprintf("%s/%s/%ld: ws_Task=%08lx\n", __FILE__, __FUNC__, __LINE__, wsNew->ws_Task));
		if (NULL == wsNew->ws_Task)
			break;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		PutMsg(&wsNew->ws_Task->pr_MsgPort, &msgStart->ScalosMessage.sm_Message);
		msgStart = NULL;	// no SCA_FreeMessage()

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (WindowFlags & SCAF_OpenWindow_ScalosPort)
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			WaitReply(ReplyPort, NULL, MTYP_StartWindow);
			}
		else
			{
			BOOL Finished = FALSE;
			struct ScalosMessage *ReplyMsg;

			do	{
				WaitPort(ReplyPort);

				ReplyMsg = (struct ScalosMessage *) GetMsg(ReplyPort);

				d1(kprintf("%s/%s/%ld: MessageType=%ld\n", __FILE__, __FUNC__, __LINE__, ReplyMsg->sm_MessageType));

				if (ReplyMsg && ID_IMSG == ReplyMsg->sm_Signature
					&& MTYP_StartWindow == ReplyMsg->sm_MessageType)
					{
					SCA_FreeMessage(ReplyMsg);
					Finished = TRUE;
					}
				} while (!Finished);
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (ReturnWs)
			*ReturnWs = wsNew;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		wsNew = NULL;
		Success = TRUE;
		} while (0);

	if (wsIconListLocked)
		{
		d1(kprintf("\n" "%s/%s/%ld:  wsLocked=%08lx\n", __FILE__, __FUNC__, __LINE__, wsIconListLocked));
		ScalosReleaseSemaphore(wsIconListLocked->ws_WindowTask->wt_IconSemaphore);
		}
	if (WindowListLocked)
		{
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		SCA_UnLockWindowList();
		}

	if (msgStart)
		SCA_FreeMessage(&msgStart->ScalosMessage);
	if (allocIconObj)
		DisposeIconObject(allocIconObj);
	if (wsNew)
		{
		if (wsNew->ws_Lock)
			UnLock(wsNew->ws_Lock);
		if (wsNew->ws_Name)
			FreeCopyString(wsNew->ws_Name);

		SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &dummyList);
		}
	if (allocMsgPort)
		DeleteMsgPort(allocMsgPort);

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}
LIBFUNC_END


BPTR DiskInfoLock(const struct ScaIconNode *in)
{
	return Lock(in->in_DeviceIcon->di_Device, ACCESS_READ);
}


static Object *ReadDeviceIconObject(CONST_STRPTR Path)
{
	Object *IconObj = NULL;
	STRPTR internalName;
	struct DosList *dList;

	do	{
		internalName = AllocPathBuffer();
		if (NULL == internalName)
			break;

		stccpy(internalName, Path, Max_PathLen);
		StripTrailingColon(internalName);

		d1(kprintf("%s/%s/%ld:  Path=<%s>  internalName=<%s>\n", __FILE__, __FUNC__, __LINE__, Path, internalName));

		dList = LockDosList(LDF_ASSIGNS | LDF_DEVICES | LDF_VOLUMES | LDF_READ);
		if ((dList = FindDosEntry(dList, internalName, LDF_ASSIGNS | LDF_DEVICES | LDF_VOLUMES)))
			{
			// First, try to get "VOLNAME:disk" icon

			d1(kprintf("%s/%s/%ld:  FOUND internalName=<%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, internalName, dList->dol_Type));

			stccpy(internalName, Path, Max_PathLen - 5);
			AddPart(internalName, "disk", Max_PathLen);

			d1(kprintf("%s/%s/%ld: internalName=<%s>\n", __FILE__, __FUNC__, __LINE__, internalName));

			IconObj = NewIconObjectTags(internalName,
				IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints,
				IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor,
				TAG_END);
			d1(kprintf("%s/%s/%ld:  IconObj=%08lx  internalName=<%s>\n", __FILE__, __FUNC__, __LINE__, IconObj, internalName));

			if (NULL == IconObj && CurrentPrefs.pref_DefIconPath)
				{
				CONST_STRPTR DeviceName = NULL;

				switch (dList->dol_Type)
					{
				case DLT_DIRECTORY:
				case DLT_LATE:
				case DLT_NONBINDING:
					IconObj = NewIconObjectTags(Path,
						IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints,
						IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor,
						TAG_END);
					break;

				case DLT_DEVICE:
					DeviceName = Path;
					break;

				case DLT_VOLUME:
					if (dList->dol_Task)
						{
						// no "VOLNAME:disk" icon found
						// try "ENV:sys/def_DEVNAME"

						// get Device name from volume name, e.g. "RAM" from "Ram Disk"
						struct Task *SigTask = (struct Task *) dList->dol_Task->mp_SigTask;

						if (SigTask)
							DeviceName = SigTask->tc_Node.ln_Name;
						}
					break;
					}

				if (NULL == IconObj && DeviceName)
					{
					stccpy(internalName, CurrentPrefs.pref_DefIconPath, Max_PathLen);

					AddPart(internalName, "def_", Max_PathLen - 1);

					SafeStrCat(internalName, DeviceName, Max_PathLen - 1);
					StripTrailingColon(internalName);
					SafeStrCat(internalName, "disk", Max_PathLen - 1);

					// if XXX is our device name, then
					// look for "ENVARC:sys/def_XXXdisk"

					d1(kprintf("%s/%s/%ld: DeviceName=<%s>\n", __FILE__, __FUNC__, __LINE__, internalName));

					IconObj = NewIconObjectTags(internalName,
						IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints,
						IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor,
						TAG_END);
					}
				}
			}

		UnLockDosList(LDF_ASSIGNS | LDF_VOLUMES | LDF_DEVICES | LDF_READ);
		} while (0);

	if (internalName)
		FreePathBuffer(internalName);

	return IconObj;
}


static Object *FindIconInScalosWindows(BPTR Lock, CONST_STRPTR IconName,
	struct ScaWindowStruct **wsLocked, BOOL *windowListLocked)
{
	struct ScaWindowStruct *ws;

	*wsLocked = NULL;
	*windowListLocked = FALSE;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		d1(kprintf("\n" "%s/%s/%ld: ws=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, ws, ws->ws_Name));
		debugLock_d1(ws->ws_Lock);

		if (LOCK_SAME == SameLock(ws->ws_Lock, Lock))
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
			struct ScaIconNode *in;

			d1(kprintf("\n" "%s/%s/%ld: found ws=%08lx\n", __FILE__, __FUNC__, __LINE__, ws));

			ScalosLockIconListShared(iwt);

			for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				CONST_STRPTR Name = GetIconName(in);

				if (Name && 0 == Stricmp((STRPTR) IconName, (STRPTR) Name))
					{
					*wsLocked = ws;
					*windowListLocked = TRUE;

					d1(kprintf("\n" "%s/%s/%ld: ws=%08lx  in=%08lx <%s>  Icon=%08lx\n", __FILE__, __FUNC__, __LINE__, ws, in, Name, in->in_Icon));

					return in->in_Icon;
					}
				}

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			ScalosUnLockIconList(iwt);
			}

		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SCA_UnLockWindowList();

	d1(kprintf("\n" "%s/%s/%ld: not found\n", __FILE__, __FUNC__, __LINE__));

	return NULL;
}


static Object *FindIconInDeviceWindow(CONST_STRPTR IconName,
	struct ScaWindowStruct **wsLocked, BOOL *windowListLocked)
{
	struct internalScaWindowTask *iwt;
	struct ScaIconNode *in;

	*windowListLocked = FALSE;

	d1(KPrintF("\n" "%s/%s/%ld: IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, IconName));
	d1(KPrintF("\n" "%s/%s/%ld: ii_MainWindowStruct=%08lx\n", __FILE__, __FUNC__, __LINE__, iInfos.xii_iinfos.ii_MainWindowStruct));

	while (NULL == iInfos.xii_iinfos.ii_MainWindowStruct
		|| NULL == iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask)
		{
		d1(KPrintF("\n" "%s/%s/%ld: Wait for ii_MainWindowStruct=%08lx\n", __FILE__, __FUNC__, __LINE__, iInfos.xii_iinfos.ii_MainWindowStruct));
		Delay(10);
		}
	iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

	d1(KPrintF("\n" "%s/%s/%ld: wt_IconSemaphore=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_IconSemaphore));

	while (NULL == iwt->iwt_WindowTask.wt_IconSemaphore && !iwt->iwt_CloseWindow)
		{
		d1(KPrintF("\n" "%s/%s/%ld: Wait for wt_IconSemaphore=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_IconSemaphore));
		Delay(10);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	ScalosLockIconListShared(iwt);

	*wsLocked = iwt->iwt_WindowTask.mt_WindowStruct;

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (MatchDeviceIcon(IconName, in))
			{
			d1(KPrintF("\n" "%s/%s/%ld: iwt=%08lx  in=%08lx <%s>  Icon=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, in, IconName, in->in_Icon));
			return in->in_Icon;
			}
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ScalosUnLockIconList(iwt);

	*wsLocked = NULL;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	d1(kprintf("\n" "%s/%s/%ld: not found\n", __FILE__, __FUNC__, __LINE__));

	return NULL;
}

// ----------------------------------------------------------

STRPTR GetWsNameFromLock(BPTR WsLock)
{
	STRPTR allocPath;
	STRPTR WsName = NULL;

	do	{
		STRPTR lp;

		allocPath = AllocPathBuffer();
		if (NULL == allocPath)
			break;

		if (!NameFromLock(WsLock, allocPath, Max_PathLen - 1))
			break;

		lp = FilePart(allocPath);
		d1(kprintf("%s/%s/%ld: FilePart=<%s>\n", __FILE__, __FUNC__, __LINE__, lp));
		if (*lp)
			{
			WsName = AllocCopyString(lp);
			if (NULL == WsName)
				break;
			}
		else
			{
			WsName = AllocCopyString(allocPath);
			if (NULL == WsName)
				break;

			StripTrailingColon(WsName);
			}
		} while (0);

	if (allocPath)
		FreePathBuffer(allocPath);

	return WsName;
}

// ----------------------------------------------------------

static BOOL MatchDeviceIcon(CONST_STRPTR SearchPath, struct ScaIconNode *in)
{
	if (NULL == in->in_DeviceIcon)
		return FALSE;

	return (BOOL) (MatchPath(SearchPath, in->in_DeviceIcon->di_Volume)
			||  MatchPath(SearchPath, in->in_DeviceIcon->di_Device)
			|| MatchPath(SearchPath, GetIconName(in)));
}


static BOOL MatchPath(CONST_STRPTR VolPath, CONST_STRPTR DevIconPath)
{
	BOOL Match = FALSE;

	if (DevIconPath)
		{
		size_t len = strlen(VolPath);
		size_t diLen = strlen(DevIconPath);

		if (':' == DevIconPath[diLen - 1])
			Match = 0 == Stricmp(VolPath, DevIconPath);
		else
			Match = (diLen + 1 == len) && 0 == Strnicmp(VolPath, DevIconPath, len);
		}

	return Match;
}

// ----------------------------------------------------------

Object *FunctionsFindIconObjectForPath(CONST_STRPTR Path, BOOL *WindowListLocked,
	struct ScaWindowStruct **wsIconListLocked, Object **allocIconObj)
{
	Object *IconObj = NULL;
	BPTR pLock;
	BPTR parentLock = BNULL;
	BPTR oldDir = NOT_A_LOCK;
	STRPTR FullPath = NULL;

	do	{
		d1(KPrintF("%s/%s/%ld: START  Path=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

		pLock = Lock(Path, ACCESS_READ);
		debugLock_d1(pLock);
		if (BNULL == pLock)
			break;

		FullPath = AllocPathBuffer();
		if (NULL == FullPath)
			break;

		if (!NameFromLock(pLock, FullPath, Max_PathLen - 1))
			break;

		d1(KPrintF("%s/%s/%ld: FullPath=<%s>\n", __FILE__, __FUNC__, __LINE__, FullPath));

		oldDir = CurrentDir(pLock);
		parentLock = ParentDir(pLock);
		debugLock_d1(parentLock);
		if (parentLock)
			{
			d1(KPrintF("%s/%s/%ld: Drawer window\n", __FILE__, __FUNC__, __LINE__));

			IconObj = FindIconInScalosWindows(parentLock, FullPath,
					wsIconListLocked, WindowListLocked);

			d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
			if (NULL == IconObj)
				{
				CurrentDir(parentLock);

				IconObj = *allocIconObj = NewIconObjectTags(FullPath,
					IDTA_SizeConstraints, (ULONG) &CurrentPrefs.pref_IconSizeConstraints,
					IDTA_ScalePercentage, CurrentPrefs.pref_IconScaleFactor,
					TAG_END);

				d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
				if (NULL == IconObj)
					{
					IconObj = (Object *) DoMethod(iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask->mt_MainObject,
						SCCM_IconWin_GetDefIcon,
						FullPath,
						ST_USERDIR,
						0,
						WBDRAWER);

					d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
					}
				}
			}
		else
			{
			// this is a root window
			d1(KPrintF("%s/%s/%ld: Root window\n", __FILE__, __FUNC__, __LINE__));

			IconObj = FindIconInDeviceWindow(FullPath,
					wsIconListLocked, WindowListLocked);

			d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

			if (NULL == IconObj)
				IconObj = *allocIconObj = ReadDeviceIconObject(FullPath);
			}
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (parentLock)
		UnLock(parentLock);
	if (FullPath)
		FreePathBuffer(FullPath);
	if (pLock)
		UnLock(pLock);

	d1(KPrintF("%s/%s/%ld: END IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	return IconObj;
}

// ----------------------------------------------------------

void FunctionsGetSettingsFromIconObject(struct IconWindowProperties *iwp, Object *IconObj)
{
	ULONG ddFlags;
	ULONG ViewBy;
	ULONG lDummy;
	STRPTR tt;

	GetAttr(IDTA_ViewModes, IconObj, &ViewBy);
	GetAttr(IDTA_Flags, IconObj, &ddFlags);

	iwp->iwp_Viewmodes = TranslateViewModesFromIcon(ViewBy);

	d1(KPrintF("%s/%s/%ld: ddFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, ddFlags));

	switch (ddFlags & DDFLAGS_SHOWMASK)
		{
	case DDFLAGS_SHOWDEFAULT:
		iwp->iwp_ShowAll = DDFLAGS_SHOWDEFAULT;
		break;
	case DDFLAGS_SHOWICONS:
		iwp->iwp_ShowAll = DDFLAGS_SHOWICONS;
		break;
	case DDFLAGS_SHOWALL:
	default:
		iwp->iwp_ShowAll = DDFLAGS_SHOWALL;
		break;
		}

	switch (ddFlags & DDFLAGS_SORTMASK)
		{
	case DDFLAGS_SORTDEFAULT:
	default:
		iwp->iwp_SortOrder = SortOrder_Default;
		break;
	case DDFLAGS_SORTASC:
		iwp->iwp_SortOrder = SortOrder_Ascending;
		break;
	case DDFLAGS_SORTDESC:
		iwp->iwp_SortOrder = SortOrder_Descending;
		break;
		}

	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_PATTERNNO", &tt))
		{
		DoMethod(IconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &lDummy);
		iwp->iwp_PatternNumber = (UWORD) lDummy;
		d1(KPrintF("%s/%s/%ld: iwp->iwp_PatternNumber=%lu\n", __FILE__, __FUNC__, __LINE__, iwp->iwp_PatternNumber));
		}
	else
		{
		// determine default pattern number
		if (IsViewByIcon(iwp->iwp_Viewmodes ))
			iwp->iwp_PatternNumber 	= PATTERNNR_IconWindowDefault;	// default icon window pattern no.
		else
			iwp->iwp_PatternNumber 	= PATTERNNR_TextWindowDefault;	// default text window pattern no.

		}

	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_CHECKOVERLAP", &tt))
		{
		DoMethod(IconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &lDummy);
		iwp->iwp_CheckOverlap = lDummy;
		d1(KPrintF("%s/%s/%ld: iwp_CheckOverlap=%ld\n", __FILE__, __FUNC__, __LINE__, iwp->iwp_CheckOverlap));
		}

	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_THUMBNAILS", &tt))
		{
		d1(KPrintF("%s/%s/%ld: tt=<%s>\n", __FILE__, __FUNC__, __LINE__, tt));
		while (*tt && '=' !=  *tt)
			tt++;

		if ('=' ==  *tt)
			{
			++tt;	// Skip "="
			if (0 == Stricmp(tt, "NEVER"))
				iwp->iwp_ThumbnailView = THUMBNAILS_Never;
			else if (0 == Stricmp(tt, "ALWAYS"))
				iwp->iwp_ThumbnailView = THUMBNAILS_Always;
			else if (0 == Stricmp(tt, "ASDEFAULT"))
				iwp->iwp_ThumbnailView = THUMBNAILS_AsDefault;
			}
		d1(KPrintF("%s/%s/%ld: iwp->iwp_ThumbnailView=%lu\n", __FILE__, __FUNC__, __LINE__, iwp->iwp_ThumbnailView));
		}

	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_THUMBNAIL_LIFETIME", &tt))
		{
		DoMethod(IconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &lDummy);
		iwp->iwp_ThumbnailsLifetimeDays = lDummy;
		}

	iwp->iwp_NoStatusBar = FALSE;
	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_NOSTATUSBAR", &tt))
		{
		iwp->iwp_NoStatusBar = TRUE;
		}

	iwp->iwp_NoControlBar = FALSE;
	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_NOCONTROLBAR", &tt))
		{
		iwp->iwp_NoControlBar = TRUE;
		}

	iwp->iwp_BrowserMode = FALSE;
	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_BROWSERMODE", &tt))
		{
		iwp->iwp_BrowserMode = TRUE;
		}

	GetAttr(IDTA_WinCurrentX, IconObj, (ULONG *) &lDummy);
	iwp->iwp_XOffset = (WORD) lDummy;

	GetAttr(IDTA_WinCurrentY, IconObj, (ULONG *) &lDummy);
	iwp->iwp_YOffset = (WORD) lDummy;

	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_ACTIVETRANSPARENCY", &tt))
		{
		DoMethod(IconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &lDummy);
		iwp->iwp_OpacityActive = (UWORD) lDummy;
		d1(KPrintF("%s/%s/%ld: iwp->iwp_OpacityActive=%lu\n", __FILE__, __FUNC__, __LINE__, iwp->iwp_OpacityActive));
		}

	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_INACTIVETRANSPARENCY", &tt))
		{
		DoMethod(IconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &lDummy);
		iwp->iwp_OpacityInactive = (UWORD) lDummy;
		d1(KPrintF("%s/%s/%ld: iwp->iwp_OpacityInactive=%lu\n", __FILE__, __FUNC__, __LINE__, iwp->iwp_OpacityInactive));
		}

	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_ICONSCALEFACTOR", &tt))
		{
		DoMethod(IconObj, IDTM_GetToolTypeValue, tt, (ULONG *) &lDummy);
		iwp->iwp_IconScaleFactor = lDummy;

		if (iwp->iwp_IconScaleFactor < IDTA_ScalePercentage_MIN)
			iwp->iwp_IconScaleFactor = IDTA_ScalePercentage_MIN;
		else if (iwp->iwp_IconScaleFactor > IDTA_ScalePercentage_MAX)
			iwp->iwp_IconScaleFactor = IDTA_ScalePercentage_MAX;

		d1(KPrintF("%s/%s/%ld: iwp_IconScaleFactor=%lu\n", __FILE__, __FUNC__, __LINE__, iwp->iwp_IconScaleFactor));
		}

	tt = NULL;
	if (DoMethod(IconObj, IDTM_FindToolType, "SCALOS_ICONSIZECONSTRAINTS", &tt))
		{
		LONG IconSizeMin, IconSizeMax;
		long long1, long2;

		d1(KPrintF("%s/%s/%ld: tt=<%s>\n", __FILE__, __FUNC__, __LINE__, tt));
		while (*tt && '=' !=  *tt)
			tt++;

		if (2 == sscanf(tt, "=%ld,%ld", &long1, &long2))
			{
			IconSizeMin = long1;
			IconSizeMax = long2;
			if ((IconSizeMax > 128) || (IconSizeMax < 0))
				IconSizeMax = SHRT_MAX;
			if ((IconSizeMin < 0) || (IconSizeMin >= IconSizeMax))
				IconSizeMin = 0;

			iwp->iwp_IconSizeConstraints.MinX = iwp->iwp_IconSizeConstraints.MinY = IconSizeMin;
			iwp->iwp_IconSizeConstraints.MaxX = iwp->iwp_IconSizeConstraints.MaxY = IconSizeMax;
			}
		d1(KPrintF("%s/%s/%ld: IconSizeMin=%ld  IconSizeMax=%ld\n", __FILE__, __FUNC__, __LINE__, IconSizeMin, IconSizeMax));
		}
}

// ----------------------------------------------------------


