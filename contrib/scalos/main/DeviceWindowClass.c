// DeviceWindowClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <cybergraphx/cybergraphics.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/cybergraphics.h>
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
#include "FsAbstraction.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

struct DeviceWindowClassInstance
	{
	ULONG dwci_Dummy;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT DeviceWindowClass_Dispatcher(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_ReadIconList(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_ReadIcon(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_RemIcon(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_RemIcon2(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_Ping(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_Message(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_New(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_Dispose(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_CheckUpdate(Class *cl, Object *o, Msg msg);
static ULONG DeviceWindowClass_Update(Class *cl, Object *o, Msg msg);
static struct ScaIconNode *ReadDevIcon_Device(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di);
static struct ScaIconNode *ReadDevIcon_Volume(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di);
static struct ScaIconNode *DevIconOk2(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, Object *iconObj);
static LONG ReadDevBackdrop(struct internalScaWindowTask *iwt, struct ScaIconNode *DevIn);
static struct ScaIconNode *CreateBackdropIcon(struct internalScaWindowTask *iwt, 
	struct BackDropList *bdl, struct ScaIconNode *DevIn, 
	CONST_STRPTR BackdropIconName, STRPTR fName);
static Object *ReadDefaultVolumeIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, STRPTR DevName);
static Object *ReadDefaultDeviceIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, STRPTR DevName);
static Object *ReadDiskIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, STRPTR DevName);
static Object *ReadDefaultIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, STRPTR DevName);
static void FreeBackdropIcon(struct ScaBackdropIcon *bdi);
static BOOL CreateSbi(struct ScaIconNode *DevIn, struct ScaIconNode *in);
static struct ScaIconNode *IconAlreadyExists(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di);
static BOOL FindIconInDrawerWindows(BPTR dirLock, CONST_STRPTR IconName);
static Object *ReadDefaultIconByName(struct internalScaWindowTask *iwt, CONST_STRPTR iconName, STRPTR DevName);
static BOOL StartDeviceIconNotify(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di);
static void SetDeviceIconSupportsFlags(struct ScaIconNode *in);
static void SetBackdropIconSupportsFlags(struct internalScaWindowTask *iwt, 
	struct BackDropList *bdl, struct ScaIconNode *in);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------


struct ScalosClass *initDeviceWindowClass(const struct PluginClass *plug)
{
	struct ScalosClass *DeviceWindowClass;

	DeviceWindowClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct DeviceWindowClassInstance),
			NULL);

	if (DeviceWindowClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(DeviceWindowClass->sccl_class->cl_Dispatcher, DeviceWindowClass_Dispatcher);
		}

	return DeviceWindowClass;
}


static SAVEDS(ULONG) INTERRUPT DeviceWindowClass_Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = DeviceWindowClass_New(cl, o, msg);
		break;

	case OM_DISPOSE:
		Result = DeviceWindowClass_Dispose(cl, o, msg);
		break;

	case SCCM_IconWin_ReadIconList:
		Result = DeviceWindowClass_ReadIconList(cl, o, msg);
		break;

	case SCCM_DeviceWin_ReadIcon:
		Result = DeviceWindowClass_ReadIcon(cl, o, msg);
		break;

	case SCCM_Ping:
		Result = DeviceWindowClass_Ping(cl, o, msg);
		break;

	case SCCM_Message:
		Result = DeviceWindowClass_Message(cl, o, msg);
		break;

	case SCCM_IconWin_NewViewMode:
		Result = 0;
		break;

	case SCCM_IconWin_Update:
		Result = DeviceWindowClass_Update(cl, o, msg);
		break;

	case SCCM_IconWin_ImmediateCheckUpdate:
		Result = DeviceWindowClass_CheckUpdate(cl, o, msg);
		break;

	case SCCM_DeviceWin_RemIcon:
		Result = DeviceWindowClass_RemIcon(cl, o, msg);
		break;

	case SCCM_IconWin_RemIcon:
		// Only supports left-out icons
		Result = DeviceWindowClass_RemIcon2(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}


static ULONG DeviceWindowClass_ReadIconList(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaDeviceIcon *sdi, *diNext;
	struct ScaIconNode *NotShownIconList = NULL;

	d1(kprintf("%s/%s/%ld: Start iwt=%08lx  ws=%08lx  <%s>\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.mt_WindowStruct, iwt->iwt_WinTitle));

	SplashAddUser();

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SplashDisplayProgress(GetLocString(MSGID_PROGRESS_READDEVICEICONS), 0);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(iwt->iwt_DevListObject, SCCM_DeviceList_Generate, &iwt->iwt_DevIconList);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (sdi = iwt->iwt_DevIconList; sdi; sdi = diNext)
		{
		diNext = (struct ScaDeviceIcon *) sdi->di_Node.mln_Succ;

		d1(kprintf("%s/%s/%ld: di=%08lx  <%s>  Flags=%04lx\n", __FILE__, __FUNC__, __LINE__, \
			sdi, sdi->di_Volume, sdi->di_Flags));

		if (DIBF_DontDisplay & sdi->di_Flags)
			{
			// remove icon from iwt_DevIconList
			d1(kprintf("%s/%s/%ld: Remove di=%08lx  <%s>  di_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, sdi, sdi->di_Device, sdi->di_Flags));
			RemoveDeviceIcon(iwt, sdi, &NotShownIconList);
			}
		else
			{
			d1(kprintf("%s/%s/%ld: Add di=%08lx  <%s>  di_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, sdi, sdi->di_Device, sdi->di_Flags));
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_DeviceWin_ReadIcon, sdi);
			}
		}
	FreeIconList(iwt, &NotShownIconList);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SplashDisplayProgress(GetLocString(MSGID_PROGRESS_ENDREADDEVICEICONS), 0);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SplashRemoveUser();

	d1(kprintf("%s/%s/%ld: Finished iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 0;
}


static ULONG DeviceWindowClass_ReadIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DevWinReadIcon *dri = (struct msg_DevWinReadIcon *) msg;
	struct ScaIconNode *in;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: iwt=%08lx  di=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, dri->dri_DeviceNode, dri->dri_DeviceNode->di_Device));
	d1(KPrintF("%s/%s/%ld: deviceNode=%08lx  <%s>  diskType=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		dri->dri_DeviceNode, dri->dri_DeviceNode->di_Device, dri->dri_DeviceNode->di_Info->id_DiskType));

	in = IconAlreadyExists(iwt, dri->dri_DeviceNode);
	if (in)
		{
		ScalosUnLockIconList(iwt);

		return 1;
		}

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>  di_Volume=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, dri->dri_DeviceNode->di_Volume));

	if (dri->dri_DeviceNode->di_Volume)
		ReadDevIcon_Volume(iwt, dri->dri_DeviceNode);
	else
		ReadDevIcon_Device(iwt, dri->dri_DeviceNode);

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 1;
}


static ULONG DeviceWindowClass_RemIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DevWinRemIcon *dri = (struct msg_DevWinRemIcon *) msg;
	struct ScaIconNode *dummyIconList = NULL;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  lock=%08lx  IconName=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, iwt, dri->dri_IconNode, GetIconName(dri->dri_IconNode)));

	ScalosLockIconListExclusive(iwt);

	SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &dummyIconList, dri->dri_IconNode);

	RemoveIcons(iwt, &dummyIconList);	// visually erase icons

	debugLock_d1(dri->dri_IconNode->in_Lock);

	FreeIconNode(iwt, &dummyIconList, dri->dri_IconNode);

	ScalosUnLockIconList(iwt);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
		SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);

	return 1;
}


static ULONG DeviceWindowClass_RemIcon2(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_RemIcon *mri = (struct msg_RemIcon *) msg;
	struct ScaIconNode *in, *inNext;
	BOOL Found = FALSE;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  lock=%08lx  IconName=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, iwt, mri->mri_Lock, mri->mri_Name));

	ScalosLockIconListExclusive(iwt);

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in=inNext)
		{
		d1(kprintf("%s/%s/%ld: in_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, in->in_Name));

		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		if (in->in_Lock && (LOCK_SAME == SameLock(in->in_Lock, mri->mri_Lock))
			&& in->in_Name && 0 == Stricmp((STRPTR) mri->mri_Name, in->in_Name))
			{
			struct ScaIconNode *dummyIconList = NULL;

			d1(KPrintF("%s/%s/%ld: FOUND in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

			Found = TRUE;

			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &dummyIconList, in);

			RemoveIcons(iwt, &dummyIconList);	// visually erase icons

			debugLock_d1(in->in_Lock);
			RewriteBackdrop(in);

			FreeIconNode(iwt, &dummyIconList, in);
			}
		}

	ScalosUnLockIconList(iwt);

	if (Found)
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
			SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
		}

	return 1;
}


static ULONG DeviceWindowClass_Ping(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (++iwt->iwt_dRefreshCount >= CurrentPrefs.pref_DiskRefresh)
		{
		iwt->iwt_dRefreshCount = 0;

		IDCMPDiskInserted(iwt, NULL);
		}

	return DoSuperMethodA(cl, o, msg);
}


static ULONG DeviceWindowClass_Message(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_Message *msm = (struct msg_Message *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	switch (msm->msm_iMsg->Class)
		{
	case IDCMP_DISKINSERTED:
	case IDCMP_DISKREMOVED:
		IDCMPDiskInserted(iwt, NULL);
		break;
		}

	return DoSuperMethodA(cl, o, msg);
}


static ULONG DeviceWindowClass_New(Class *cl, Object *o, Msg msg)
{
	d1(kprintf("%s/%s/%ld:  START  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	o = (Object *) DoSuperMethodA(cl, o, msg);

	if (o)
		{
		struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
		struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
		struct opSet *ops = (struct opSet *) msg;
		ULONG n;

		d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  o=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, o));

		SplashDisplayProgress(GetLocString(MSGID_PROGRESS_INITDEVICEWINDOW), 0);

		// desktop window is ALWAYS displayed as icons
		ws->ws_Viewmodes = IDTV_ViewModes_Icon;

		iwt->iwt_DevListObject = (Object *) SCA_NewScalosObject("DeviceList.sca", ops->ops_AttrList);

		iwt->iwt_IDCMPFlags |= IDCMP_DISKINSERTED | IDCMP_DISKREMOVED;
		iwt->iwt_NeedsTimerFlag = TRUE;
		ws->ws_Flags |= WSV_FlagF_NeedsTimerMsg;

		iwt->iwt_IconFont = IconWindowFont;
		iwt->iwt_IconTTFont = IconWindowTTFont;

		// initialize iwt_IconLayoutInfo
		memset(&iwt->iwt_IconLayoutInfo.ili_LayoutArea, 0, sizeof(iwt->iwt_IconLayoutInfo.ili_LayoutArea));
		memset(&iwt->iwt_IconLayoutInfo.ili_Restricted, 0, sizeof(iwt->iwt_IconLayoutInfo.ili_Restricted));

		for (n = 0; n < Sizeof(iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes); n++)
			{
			if (n < Sizeof(CurrentPrefs.pref_DeviceWindowLayoutModes))
				iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes[n] = CurrentPrefs.pref_DeviceWindowLayoutModes[n];
			else
				iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes[n] = ICONLAYOUT_Columns; //default
			}

		d1(kprintf("%s/%s/%ld:  DevListObject=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DevListObject));

		if (NULL == iwt->iwt_DevListObject)
			{
			DoMethod(o, OM_DISPOSE);
			o = NULL;
			}
		}

	d1(kprintf("%s/%s/%ld:  END o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	return (ULONG) o;
}


static ULONG DeviceWindowClass_Dispose(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	SCA_DisposeScalosObject(iwt->iwt_DevListObject);
	iwt->iwt_DevListObject = NULL;

	return DoSuperMethodA(cl, o, msg);
}


static ULONG DeviceWindowClass_CheckUpdate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return DeviceWindowCheckUpdate(iwt);
}


static ULONG DeviceWindowClass_Update(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	ULONG Result, ReadResult;
	struct ScaIconNode *in, *inNext;
	ULONG IconType;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	UpdateIconOverlays(iwt);

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return 0;

	ScalosLockIconListExclusive(iwt);

	d1(Forbid(); KPrintF("%s/%s/%ld: iwt=%08lx  MsgList Head=%08lx  TailPred=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.wt_IconPort->mp_MsgList.lh_Head, \
		iwt->iwt_WindowTask.wt_IconPort->mp_MsgList.lh_TailPred); Permit());

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = inNext)
		{
		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		GetAttr(IDTA_Type, in->in_Icon, &IconType);

		if (WBAPPICON != IconType)
			FreeIconNode(iwt, &iwt->iwt_WindowTask.wt_IconList, in);
		}

	d1(Forbid(); KPrintF("%s/%s/%ld: iwt=%08lx  MsgList Head=%08lx  TailPred=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.wt_IconPort->mp_MsgList.lh_Head, \
		iwt->iwt_WindowTask.wt_IconPort->mp_MsgList.lh_TailPred); Permit());

	for (in=iwt->iwt_WindowTask.wt_LateIconList; in; in = inNext)
		{
		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		GetAttr(IDTA_Type, in->in_Icon, &IconType);

		if (WBAPPICON != IconType)
			FreeIconNode(iwt, &iwt->iwt_WindowTask.wt_LateIconList, in);
		}

	d1(Forbid(); KPrintF("%s/%s/%ld: iwt=%08lx  MsgList Head=%08lx  TailPred=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.wt_IconPort->mp_MsgList.lh_Head, \
		iwt->iwt_WindowTask.wt_IconPort->mp_MsgList.lh_TailPred); Permit());

	ScalosUnLockIconList(iwt);

	IconActive = FALSE;

	SetAttrs(iwt->iwt_WindowTask.mt_MainObject, 
		SCCA_IconWin_Reading, TRUE,
		TAG_END);

	SetMenuOnOff(iwt);

	if (iwt->iwt_WindowTask.wt_Window)
		{
		EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
			0, 0,
			iwt->iwt_WindowTask.wt_Window->Width - 1,
			iwt->iwt_WindowTask.wt_Window->Height - 1);
		}

	RefreshIcons(iwt, NULL);

	ReadResult = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ReadIconList, 0L);

	SetAttrs(iwt->iwt_WindowTask.mt_MainObject, 
		SCCA_IconWin_Reading, FALSE,
		TAG_END);

	SetMenuOnOff(iwt);

	if (ScanDirIsError(ReadResult))
		{
		Result = 1;
		iwt->iwt_CloseWindow = TRUE;
		}
	else
		{
		Result = 0;
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);
		}

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return Result;
}


static struct ScaIconNode *ReadDevIcon_Device(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di)
{
	struct ScaIconNode *in = NULL;
	STRPTR DevName;
	Object *iconObj;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  di=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, di, di->di_Device));

	do	{
		DevName = AllocPathBuffer();
		if (NULL == DevName)
			break;

		stccpy(DevName, di->di_Device, Max_PathLen - 4);

		d1(kprintf("%s/%s/%ld: DevName=<%s>\n", __FILE__, __FUNC__, __LINE__, DevName));

		if (MAKE_ID('B','A','D',0) == di->di_Info->id_DiskType)
			SafeStrCat(DevName, GetLocString(MSGID_DISK_UNREADABLE), Max_PathLen);
		else if (MAKE_ID('B','U','S','Y') == di->di_Info->id_DiskType)
			SafeStrCat(DevName, GetLocString(MSGID_DISK_BUSY), Max_PathLen);
		else if (MAKE_ID('N','D','O','S') == di->di_Info->id_DiskType)			// +jmc+
			SafeStrCat(DevName, GetLocString(MSGID_DISK_NDOS), Max_PathLen);
		else
			strncat(DevName, (STRPTR) &di->di_Info->id_DiskType, 4);

		// GetDefIconObject()
		iconObj = GetDefIconObjectTags(WBKICK,
				IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
				IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
				IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
				IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
				IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
				IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
				IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
				IDTA_FrameType, CurrentPrefs.pref_FrameType,
				IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
				IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
				IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
				IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
				IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
				IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
				IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
				IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
				IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
				IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
				IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
				IDTA_TextMode, CurrentPrefs.pref_TextMode,
				IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
				IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
				DTA_Name, (ULONG) di->di_Device,
				IDTA_Text, (ULONG) DevName,
				IDTA_Font, (ULONG) iwt->iwt_IconFont,
				IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
				IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
				IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
				IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
				TAG_END);
		if (NULL == iconObj)
			break;

		in = DevIconOk2(iwt, di, iconObj);
		} while (0);

	if (DevName)
		FreePathBuffer(DevName);

	return in;
}


static struct ScaIconNode *ReadDevIcon_Volume(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di)
{
	BPTR dLock;
	BPTR oldDir = NOT_A_LOCK;
	STRPTR DevName = NULL;
	struct ScaIconNode *in = NULL;
	Object *iconObj = NULL;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  di=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, di, di->di_Volume));

	do	{
		dLock = Lock(di->di_Device, ACCESS_READ);
		if ((BPTR)NULL == dLock)
			break;

		debugLock_d1(dLock);

		oldDir = CurrentDir(dLock);

		DevName = AllocPathBuffer();
		if (NULL == DevName)
			break;

		stccpy(DevName, di->di_Volume, Max_PathLen - 1);
		StripTrailingColon(DevName);

		d1(KPrintF("%s/%s/%ld: DevName=<%s>\n", __FILE__, __FUNC__, __LINE__, DevName));
		d1(KPrintF("%s/%s/%ld: pref_DefIconsFirst=%ld\n", __FILE__, __FUNC__, __LINE__, CurrentPrefs.pref_DefIconsFirst));

		if (CurrentPrefs.pref_DefIconsFirst)
			{
			iconObj = ReadDefaultVolumeIcon(iwt, di, DevName);
			d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

			if (NULL == iconObj)
				iconObj = ReadDefaultDeviceIcon(iwt, di, DevName);

			d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));
			if (NULL == iconObj)
				iconObj = ReadDiskIcon(iwt, di, DevName);
			}
		else
			{
			iconObj = ReadDiskIcon(iwt, di, DevName);

			d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));
			if (NULL == iconObj)
				iconObj = ReadDefaultVolumeIcon(iwt, di, DevName);

			d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));
			if (NULL == iconObj)
				iconObj = ReadDefaultDeviceIcon(iwt, di, DevName);
			}

		d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));
		if (NULL == iconObj)
			{
			iconObj = ReturnDefIconObjTags(dLock, "",
				IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
				IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
				IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
				IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
				IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
				IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
				IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
				IDTA_FrameType, CurrentPrefs.pref_FrameType,
				IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
				IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
				IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
				IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
				IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
				IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
				IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
				IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
				IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
				IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
				IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
				IDTA_TextMode, CurrentPrefs.pref_TextMode,
				IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
				IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
				DTA_Name, (ULONG) di->di_Device,
				IDTA_Text, (ULONG) DevName,
				IDTA_Font, (ULONG) iwt->iwt_IconFont,
				IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
				IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
				IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
				IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
				TAG_END);
			}

		d1(KPrintF("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));
		if (NULL == iconObj)
			iconObj = ReadDefaultIcon(iwt, di, DevName);
		} while (0);

	if (iconObj)
		in = DevIconOk2(iwt, di, iconObj);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (dLock)
		UnLock(dLock);
	if (DevName)
		FreePathBuffer(DevName);

	return in;
}


// try to read <pref_DefIconPath>/<VolumeName>.info
static Object *ReadDefaultVolumeIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, STRPTR DevName)
{
	Object *iconObj;
	STRPTR iconName = DevName + strlen(DevName) + 1;
	size_t iconNameLen = Max_PathLen - strlen(DevName) - 2;

	stccpy(iconName, CurrentPrefs.pref_DefIconPath, iconNameLen);
	d1(kprintf("%s/%s/%ld: iconName=<%s>  Len=%ld\n", __FILE__, __FUNC__, __LINE__, iconName, iconNameLen));
	AddPart(iconName, "", iconNameLen);
	d1(kprintf("%s/%s/%ld: iconName=<%s>  Len=%ld\n", __FILE__, __FUNC__, __LINE__, iconName, iconNameLen));
	SafeStrCat(iconName, "def_", iconNameLen);
	d1(kprintf("%s/%s/%ld: iconName=<%s>  Len=%ld\n", __FILE__, __FUNC__, __LINE__, iconName, iconNameLen));
	SafeStrCat(iconName, di->di_Volume, iconNameLen);
	d1(kprintf("%s/%s/%ld: iconName=<%s>  Len=%ld\n", __FILE__, __FUNC__, __LINE__, iconName, iconNameLen));
	StripTrailingColon(iconName);

	d1(kprintf("%s/%s/%ld: iconName=<%s>\n", __FILE__, __FUNC__, __LINE__, iconName));

	iconObj = ReadDefaultIconByName(iwt, iconName, DevName);

	if (NULL == iconObj)
		{
		// Try again, but remove spaces from di->di_Volume 
		STRPTR dlp;
		CONST_STRPTR slp;
		size_t Len;

		stccpy(iconName, CurrentPrefs.pref_DefIconPath, iconNameLen);
		AddPart(iconName, "", iconNameLen);
		SafeStrCat(iconName, "def_", iconNameLen);

		Len = iconNameLen - 1 - strlen(iconName);

		dlp = iconName + strlen(iconName);
		slp = di->di_Volume;
		while (Len && *slp)
			{
			if (' ' != *slp)
				{
				*dlp++ = *slp;
				Len--;
				}
			slp++;
			}
		StripTrailingColon(iconName);

		d1(kprintf("%s/%s/%ld: iconName=<%s>\n", __FILE__, __FUNC__, __LINE__, iconName));

		iconObj = ReadDefaultIconByName(iwt, iconName, DevName);
		}

	d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

	return iconObj;
}


// try to read <pref_DefIconPath>/<DeviceName>.info
static Object *ReadDefaultDeviceIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, STRPTR DevName)
{
	Object *iconObj;
	STRPTR iconName = DevName + strlen(DevName) + 1;
	size_t iconNameLen = Max_PathLen - strlen(DevName) - 2;

	stccpy(iconName, CurrentPrefs.pref_DefIconPath, iconNameLen);
	AddPart(iconName, "", iconNameLen);
	SafeStrCat(iconName, "def_", iconNameLen);
	SafeStrCat(iconName, di->di_Device, iconNameLen);
	StripTrailingColon(iconName);

	d1(kprintf("%s/%s/%ld: iconName=<%s>\n", __FILE__, __FUNC__, __LINE__, iconName));

	iconObj = ReadDefaultIconByName(iwt, iconName, DevName);

	d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

	return iconObj;
}


// try to read "disk.info" icon from volume
static Object *ReadDiskIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, STRPTR DevName)
{
	Object *iconObj;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  di=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, di, di->di_Volume));

	// NewIconObject()
	iconObj = NewIconObjectTags("disk",
			IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
			IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
			IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
			IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
			IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
			IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
			IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
			IDTA_FrameType, CurrentPrefs.pref_FrameType,
			IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
			IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
			IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
			IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
			IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
			IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
			IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
			IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
			IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
			IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
			IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
			IDTA_TextMode, CurrentPrefs.pref_TextMode,
			IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
			IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
			IDTA_Font, (ULONG) iwt->iwt_IconFont,
			IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
			IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
			IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes,
			IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
			IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
			TAG_END);

	d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

	if (iconObj)
		{
		ULONG IconType;

		GetAttr(IDTA_Type, iconObj, &IconType);

		d1(kprintf("%s/%s/%ld: iconObj=%08lx  IconType=%ld\n", __FILE__, __FUNC__, __LINE__, iconObj, IconType));

		if (WBDISK == IconType)
			{
			SetAttrs(iconObj, 
				IDTA_Text, (ULONG) DevName,
				TAG_END);
			}
		else
			{
			DisposeIconObject(iconObj);
			iconObj = NULL;
			}
		}

	d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

	return iconObj;
}


// try to get default WBDISK icon
static Object *ReadDefaultIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, STRPTR DevName)
{
	Object *iconObj;
	ULONG IconType = WBDISK;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  di=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, di, di->di_Volume));

	switch (di->di_Info->id_DiskType)
		{
	case ID_NOT_REALLY_DOS:
	case ID_KICKSTART_DISK:
		IconType = WBKICK;
		break;
		}
	
	// GetDefIconObject()
	iconObj = GetDefIconObjectTags(IconType,
			IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
			IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
			IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
			IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
			IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
			IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
			IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
			IDTA_FrameType, CurrentPrefs.pref_FrameType,
			IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
			IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
			IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
			IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
			IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
			IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
			IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
			IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
			IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
			IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
			IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
			IDTA_TextMode, CurrentPrefs.pref_TextMode,
			IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
			IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
			DTA_Name, (ULONG) di->di_Device,
			IDTA_Text, (ULONG) DevName,
			IDTA_Font, (ULONG) iwt->iwt_IconFont,
			IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
			IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
			TAG_END);

	if (iconObj)
		{
		SetAttrs(iconObj, 
			IDTA_Type, WBDISK,
			TAG_END);
		}

	d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

	return iconObj;
}


static struct ScaIconNode *DevIconOk2(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di, Object *iconObj)
{
	struct ScaIconNode *in;
	STRPTR tt;
	ULONG IconType;
	struct ExtGadget *gg = (struct ExtGadget *) iconObj;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  di=%08lx iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, di, iconObj));

	tt = NULL;
	if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NOTEXT", &tt))
		{
		d1(kprintf("%s/%s/%ld: SCALOS_NOTEXT\n", __FILE__, __FUNC__, __LINE__));

		SetAttrs(iconObj,
			IDTA_Text, NULL,
			TAG_END);
		}

	d1(kprintf("%s/%s/%ld: iwt=%08lx  di=%08lx iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, di, iconObj));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_LayoutIcon, iconObj, IOLAYOUTF_NormalImage);

	ScalosLockIconListExclusive(iwt);

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  di=%08lx iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, di, iconObj));

	if (IsNoIconPosition(gg))
		in = SCA_AllocIconNode(&iwt->iwt_WindowTask.wt_LateIconList);
	else
		in = SCA_AllocIconNode(&iwt->iwt_WindowTask.wt_IconList);
	if (NULL == in)
		{
		ScalosUnLockIconList(iwt);
		return in;
		}

	SetAttrs(iconObj, 
		IDTA_Type, WBDISK,
		TAG_END);

	StartDeviceIconNotify(iwt, di);

	SetIconName(iconObj, in);

	in->in_Icon = di->di_Icon = iconObj;
	in->in_DeviceIcon = di;
//	  in->in_FileType = (struct TypeNode *) WBDISK;
	in->in_FileType = DefIconsIdentify(BNULL, di->di_Volume, WBDISK);
	in->in_Flags |= INF_Identified;

	d1(KPrintF("%s/%s/%ld: <%s>  in_FileType=%08lx\n", __FILE__, __FUNC__, __LINE__, GetIconName(in), in->in_FileType));
	DateStamp(&in->in_DateStamp);

	if (!ClassCheckInfoData(di->di_Info))
		{
		in->in_Flags |= INF_VolumeWriteProtected;
		AddIconOverlay(in->in_Icon, ICONOVERLAYF_ReadOnly);
		}

	SetDeviceIconSupportsFlags(in);

	if (!(in->in_SupportFlags & INF_SupportsOpen))
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		gg->Flags |= GFLG_DISABLED;
		}

	d1(kprintf("%s/%s/%ld: <%s>  SupportFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		GetIconName(in), in->in_SupportFlags));

	tt = NULL;
	if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NODRAG", &tt))
		{
		in->in_Flags |= INF_Sticky;
		}

	ScalosUnLockIconList(iwt);

	if (!IsNoIconPosition(gg))
		{
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if ((!iwt->iwt_BackDrop || !CheckPosition(iwt, in)) && !CheckOverlap(iwt, in))
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DrawIcon, iconObj);
		}

	GetAttr(IDTA_Type, iconObj, &IconType);

	if (WBDISK == IconType)
		{
		// Read and display backdrop icons
		ReadDevBackdrop(iwt, in);
		}

	return in;
}


static LONG ReadDevBackdrop(struct internalScaWindowTask *iwt, struct ScaIconNode *DevIn)
{
	STRPTR fName = NULL;
	struct BackDropList bdl;
	LONG Result;
	BPTR oldDir = NOT_A_LOCK;
	BPTR vLock;

	d1(kprintf("%s/%s/%ld: START iwt=%08lx  DevIn=%08lx <%s>  in_DeviceIcon=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, DevIn, DevIn->in_Name, DevIn->in_DeviceIcon));

	BackDropInitList(&bdl);

	do	{
		struct ChainedLine *cnl;

		d1(kprintf("%s/%s/%ld: di_Device=<%s>\n", __FILE__, __FUNC__, __LINE__, DevIn->in_DeviceIcon->di_Device));

		vLock = Lock(DevIn->in_DeviceIcon->di_Device, ACCESS_READ);
		d1(kprintf("%s/%s/%ld: vLock=%08lx\n", __FILE__, __FUNC__, __LINE__, vLock));
		debugLock_d1(vLock);
		if (BNULL == vLock)
			{
			Result = IoErr();
			d1(kprintf("%s/%s/%ld: vLock=NULL,  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		oldDir = CurrentDir(vLock);

		Result = BackdropLoadList(&bdl);
		if (RETURN_OK != Result)
			{
			d1(kprintf("%s/%s/%ld: BackdropLoadList() failed,  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		fName = AllocPathBuffer();
		if (NULL == fName)
			{
			Result = ERROR_NO_FREE_STORE;
			d1(kprintf("%s/%s/%ld: AllocPathBuffer() failed,  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			break;
			}

		for (cnl = (struct ChainedLine *) bdl.bdl_LinesList.lh_Head;
			RETURN_OK == Result && cnl != (struct ChainedLine *) &bdl.bdl_LinesList.lh_Tail;
			cnl = (struct ChainedLine *) cnl->cnl_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, cnl->cnl_Line));
			CreateBackdropIcon(iwt, &bdl, DevIn, cnl->cnl_Line, fName);
			}
		} while (0);

	if (DevIn && DevIn->in_DeviceIcon)
		{
		d1(kprintf("%s/%s/%ld: setting DIBF_BackdropReadComplete flag\n", __FILE__, __FUNC__, __LINE__));
		DevIn->in_DeviceIcon->di_Flags |= DIBF_BackdropReadComplete;
		}

	BackdropFreeList(&bdl);

	d1(kprintf("%s/%s/%ld: vLock=%08lx\n", __FILE__, __FUNC__, __LINE__, vLock));

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (vLock)
		UnLock(vLock);
	if (fName)
		FreePathBuffer(fName);

	d1(kprintf("%s/%s/%ld: END  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static struct ScaIconNode *CreateBackdropIcon(struct internalScaWindowTask *iwt, 
	struct BackDropList *bdl, struct ScaIconNode *DevIn, 
	CONST_STRPTR BackdropIconName, STRPTR fName)
{
	char ch;
	STRPTR lp;
	STRPTR tt;
	ULONG IconType;
	Object *iconObj = NULL;
	struct ScaIconNode *in = NULL;
	struct ExtGadget *gg;
	BPTR dirLock = (BPTR)NULL;
	BPTR oldDir = NOT_A_LOCK;
	BPTR iLock;				// +++ JMC 28.11.2004
	T_ExamineData *fib = NULL;		// +++ JMC 28.11.2004


	d1(kprintf("%s/%s/%ld: START BackdropIconName=<%s>\n", __FILE__, __FUNC__, __LINE__, BackdropIconName));

	do	{

		if (strlen(BackdropIconName) < 1)
			break;

		stccpy(fName, BackdropIconName, Max_PathLen - 1);

		lp = PathPart(fName);
		ch = *lp;
		*lp = '\0';

		d1(kprintf("%s/%s/%ld: PathPart=<%s>\n", __FILE__, __FUNC__, __LINE__, fName));

		dirLock = Lock(fName, ACCESS_READ);


		*lp = ch;
		debugLock_d1(dirLock);
		if ((BPTR)NULL == dirLock)
			break;

		oldDir = CurrentDir(dirLock);

		lp = FilePart(fName);
		d1(kprintf("%s/%s/%ld: FilePart=<%s>\n", __FILE__, __FUNC__, __LINE__, lp));

//-----------------------------------------------------------------------------
// +++ JMC 28.11.2004
// Check if the object got
// a Hidden protection bit activated before to display it on desktop.

		if (CurrentPrefs.pref_HideProtectHiddenFlag)
			{
			if (!ScalosExamineBegin(&fib))
			if (NULL == fib)
				{
				d1(kprintf("%s/%s/%ld: AllocDosObject failed\n", __FILE__, __FUNC__, __LINE__));
				break;
				}
			iLock = Lock(PathPart(fName), SHARED_LOCK);
			if (iLock)
				{
				ScalosExamineLock(iLock, &fib);
				if (ScalosExamineGetProtection(fib) & FIBF_HIDDEN)
					{
					d1(kprintf("%s/%s/%ld: <%s> FIBF_HIDDEN found!\n", __FILE__, __FUNC__, __LINE__, PathPart(fName)));
					UnLock(iLock);
					break;
					}
				UnLock(iLock);
				}
			}

//-----------------------------------------------------------------------------

		if (FindIconInDrawerWindows(dirLock, lp))
			{
			d1(kprintf("%s/%s/%ld: <%s> Icon found in drawer window!\n", __FILE__, __FUNC__, __LINE__, PathPart(fName)));
			break;
			}

		iconObj = NewIconObjectTags(lp,
			IDTA_Text, (ULONG) lp,
			IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
			IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
			IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
			IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
			IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
			IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
			IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
			IDTA_FrameType, CurrentPrefs.pref_FrameType,
			IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
			IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
			IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
			IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
			IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
			IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
			IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
			IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
			IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
			IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
			IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
			IDTA_TextMode, CurrentPrefs.pref_TextMode,
			IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
			IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
			IDTA_Font, (ULONG) iwt->iwt_IconFont,
			IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
			IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
			IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes,
			IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
			IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
			IDTA_OverlayType, ICONOVERLAYF_LeftOut,
			TAG_END);

		d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));
		if (NULL == iconObj)
			break;

		gg = (struct ExtGadget *) iconObj;

		GetAttr(IDTA_Type, iconObj, &IconType);

		d1(kprintf("%s/%s/%ld: IconType=%ld\n", __FILE__, __FUNC__, __LINE__, IconType));
		if (WBDISK == IconType)
			break;

		tt = NULL;
		if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NOTEXT", &tt))
			{
			d1(kprintf("%s/%s/%ld: SCALOS_NOTEXT\n", __FILE__, __FUNC__, __LINE__));
			SetAttrs(iconObj,
				IDTA_Text, NULL,
				TAG_END);
			}

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_LayoutIcon, iconObj, IOLAYOUTF_NormalImage);

		ScalosLockIconListExclusive(iwt);

		if (IsNoIconPosition(gg))
			in = SCA_AllocIconNode(&iwt->iwt_WindowTask.wt_LateIconList);
		else
			in = SCA_AllocIconNode(&iwt->iwt_WindowTask.wt_IconList);
		if (NULL == in)
			break;

		SetIconName(iconObj, in);

		DateStamp(&in->in_DateStamp);

		in->in_Icon = iconObj;
		in->in_Lock = CurrentDir(oldDir);
		oldDir = NOT_A_LOCK;
		dirLock = (BPTR)NULL;	      // do not UnLock() !!!s

		if (WBTOOL == IconType || WBPROJECT == IconType)
			{
			in->in_Flags |= INF_File;
			}

		SetBackdropIconSupportsFlags(iwt, bdl, in);

		tt = NULL;
		if (DoMethod(iconObj, IDTM_FindToolType, "SCALOS_NODRAG", &tt))
			{
			in->in_Flags |= INF_Sticky;
			}

		if (!ClassCheckInfoData(DevIn->in_DeviceIcon->di_Info))
			{
			in->in_Flags |= INF_VolumeWriteProtected;
			AddIconOverlay(in->in_Icon, ICONOVERLAYF_ReadOnly);
			}

		ScalosUnLockIconList(iwt);

		if (!IsNoIconPosition(gg))
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			if ((!iwt->iwt_BackDrop || !CheckPosition(iwt, in)) && !CheckOverlap(iwt, in))
				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DrawIcon, iconObj);
			}

		CreateSbi(DevIn, in);

		iconObj = NULL;
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (dirLock)
		UnLock(dirLock);
	if (iconObj)
		DisposeIconObject(iconObj);
	ScalosExamineEnd(&fib);

	d1(kprintf("%s/%s/%ld: END  in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

	return in;
}


// Frees everything inside the ScaBackdropIcon, but does not free the node itself !!
static void FreeBackdropIcon(struct ScaBackdropIcon *bdi)
{
	d1(kprintf("%s/%s/%ld: bdi=%08lx  sbi_NotifyReq=%08lx  sbi_DrawerName=%08lx <%s>\n", \
		__FILE__, __FUNC__, __LINE__, bdi, bdi->sbi_NotifyReq, bdi->sbi_DrawerName, bdi->sbi_DrawerName));

	if (NULL == bdi)
		return;

	if (bdi->sbi_NotifyReq)
		{
		ScalosEndNotify(bdi->sbi_NotifyReq);

		ScalosFree(bdi->sbi_NotifyReq);
		bdi->sbi_NotifyReq = NULL;
		}
	if (bdi->sbi_DrawerName)
		{
		FreePathBuffer(bdi->sbi_DrawerName);
		bdi->sbi_DrawerName = NULL;
		}
}


void FreeBackdropIconList(struct ScaBackdropIcon **bdiList)
{
	struct ScaBackdropIcon *bdi, *bdiNext;

	d1(kprintf("%s/%s/%ld: START *bdiList=%08lx\n", __FILE__, __FUNC__, __LINE__, *bdiList));

	for (bdi=*bdiList; bdi; bdi = bdiNext)
		{
		bdiNext = (struct ScaBackdropIcon *) bdi->sbi_Node.mln_Succ;

		d1(kprintf("%s/%s/%ld: bdi=%08lx  bdiNext=%08lx\n", __FILE__, __FUNC__, __LINE__, bdi, bdiNext));

		FreeBackdropIcon(bdi);
		SCA_FreeNode((struct ScalosNodeList *) bdiList, &bdi->sbi_Node);
		}

	d1(kprintf("%s/%s/%ld: END *bdiList=%08lx\n", __FILE__, __FUNC__, __LINE__, *bdiList));
}


// remove ScalosBackdropIcon from in_IconList of the parent device icon
void RemoveScalosBackdropIcon(struct ScaIconNode *in)
{
	struct internalScaWindowTask *iwtMain;
	struct ScaBackdropIcon *bdiList = NULL;
	struct ScaIconNode *inDev;

	d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	if (NULL== iInfos.xii_iinfos.ii_MainWindowStruct)
		return;

	iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;

	ScalosLockIconListExclusive(iwtMain);

	for (inDev=iwtMain->iwt_WindowTask.wt_IconList; inDev; inDev = (struct ScaIconNode *) inDev->in_Node.mln_Succ)
		{
		if (inDev->in_IconList)
			{
			struct ScaBackdropIcon *sbi, *sbiNext;

			for (sbi=inDev->in_IconList; sbi; sbi = sbiNext)
				{
				sbiNext = (struct ScaBackdropIcon *) sbi->sbi_Node.mln_Succ;

				if (sbi->sbi_Icon == in->in_Icon)
					{
					d1(kprintf("%s/%s/%ld: sbi=%08lx  sbi_NotifyReq=%08lx  sbi_DrawerName=%08lx <%s>\n",\
						 __FILE__, __FUNC__, __LINE__, sbi, sbi->sbi_NotifyReq, sbi->sbi_DrawerName, sbi->sbi_DrawerName));

					SCA_MoveNode((struct ScalosNodeList *)(APTR) &inDev->in_IconList,
						(struct ScalosNodeList *)(APTR) &bdiList,
						&sbi->sbi_Node);

					d1(kprintf("%s/%s/%ld: sbi=%08lx  sbi_NotifyReq=%08lx  sbi_DrawerName=%08lx <%s>\n",\
						 __FILE__, __FUNC__, __LINE__, sbi, sbi->sbi_NotifyReq, sbi->sbi_DrawerName, sbi->sbi_DrawerName));
					}
				}
			}
		}

	d1(kprintf("%s/%s/%ld: bdiList=%08lx\n", __FILE__, __FUNC__, __LINE__, bdiList));

	FreeBackdropIconList(&bdiList);

	ScalosUnLockIconList(iwtMain);
}


// Create <ScaBackdropIcon> entry for <in> in <DevIn->in_IconList>
static BOOL CreateSbi(struct ScaIconNode *DevIn, struct ScaIconNode *in)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	struct ScaBackdropIcon *bdi;
	BOOL NotifySuccess = FALSE;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld: DevIn=%08lx <%s>  in=%08lx <%s>\n", \
		__FILE__, __FUNC__, __LINE__, DevIn, GetIconName(DevIn), in, GetIconName(in)));

	do	{
		bdi = (struct ScaBackdropIcon *) SCA_AllocNode((struct ScalosNodeList *) &DevIn->in_IconList, 
			sizeof(struct ScaBackdropIcon) - sizeof(struct MinNode));
		if (NULL == bdi)
			break;

		bdi->sbi_Icon = in->in_Icon;

		bdi->sbi_NotifyReq = ScalosAlloc(sizeof(struct NotifyRequest));
		if (NULL == bdi->sbi_NotifyReq)
			break;

		memset(bdi->sbi_NotifyReq, 0, sizeof(struct NotifyRequest));

		d1(kprintf("%s/%s/%ld: bdi=%08lx  NotifyReq=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, bdi, bdi->sbi_NotifyReq));

		bdi->sbi_DrawerName = AllocPathBuffer();
		if (NULL == bdi->sbi_DrawerName)
			break;

		d1(kprintf("%s/%s/%ld: sbi=%08lx  sbi_DrawerName=%08lx\n", __FILE__, __FUNC__, __LINE__, bdi, bdi->sbi_DrawerName));

		if (!NameFromLock(in->in_Lock, bdi->sbi_DrawerName, MAX_FileName-1))
			break;

		bdi->sbi_NotifyReq->nr_Name = bdi->sbi_DrawerName;
		bdi->sbi_NotifyReq->nr_stuff.nr_Msg.nr_Port = iwtMain->iwt_WindowTask.wt_IconPort;
		bdi->sbi_NotifyReq->nr_Flags = NRF_SEND_MESSAGE;
		bdi->sbi_NotifyReq->nr_UserData = (ULONG)NULL;

		d1(kprintf("%s/%s/%ld: starting notification on <%s>\n", \
			__FILE__, __FUNC__, __LINE__, bdi->sbi_NotifyReq->nr_Name));

		Success = TRUE;

		if (!StartNotify(bdi->sbi_NotifyReq))
			break;

		d1(kprintf("%s/%s/%ld: started notification on <%s>\n", \
			__FILE__, __FUNC__, __LINE__, bdi->sbi_NotifyReq->nr_Name));

		NotifySuccess = TRUE;
		} while (0);

	if (Success)
		{
		if (!NotifySuccess)
			{
			if (bdi->sbi_NotifyReq)
				{
				ScalosFree(bdi->sbi_NotifyReq);
				bdi->sbi_NotifyReq = NULL;
				}
			}

		d1(kprintf("%s/%s/%ld: sbi=%08lx  sbi_DrawerName=%08lx\n", __FILE__, __FUNC__, __LINE__, bdi, bdi->sbi_DrawerName));
		}
	else
		{
		if (bdi)
			{
			if (bdi->sbi_DrawerName)
				{
				FreePathBuffer(bdi->sbi_DrawerName);
				bdi->sbi_DrawerName = NULL;
				}
			if (bdi->sbi_NotifyReq)
				{
				ScalosFree(bdi->sbi_NotifyReq);
				bdi->sbi_NotifyReq = NULL;
				}

			SCA_FreeNode((struct ScalosNodeList *) &DevIn->in_IconList, &bdi->sbi_Node);
			}
		}

	d1(kprintf("%s/%s/%ld: Success=%ld  NotifySuccess=%ld\n", __FILE__, __FUNC__, __LINE__, Success, NotifySuccess));

	return NotifySuccess;
}


// Look for Device icon for <in>, 
// and create <ScaBackdropIcon> entry for <in>
void CreateSbiForIcon(struct ScaIconNode *in)
{
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	struct ScaIconNode *inDev;
	BOOL Finished = FALSE;

	d1(kprintf("%s/%s/%ld: START in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
	debugLock_d1(in->in_Lock);

	if ((BPTR)NULL == in->in_Lock)
		return;		// only useful for left-out icons

	ScalosLockIconListExclusive(iwtMain);

	for (inDev=iwtMain->iwt_WindowTask.wt_IconList; !Finished && inDev; inDev = (struct ScaIconNode *) inDev->in_Node.mln_Succ)
		{
		if (inDev->in_DeviceIcon)
			{
			BPTR DeviceIconLock = DiskInfoLock(inDev);

			if (DeviceIconLock)
				{
				LONG IsSameLock;

				d1(kprintf("%s/%s/%ld: inDev=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, inDev, GetIconName(inDev)));

				IsSameLock = ScaSameLock(DeviceIconLock, in->in_Lock);
				d1(kprintf("%s/%s/%ld: SameLock()=%ld\n", __FILE__, __FUNC__, __LINE__, IsSameLock));

				switch (IsSameLock)
					{
				case LOCK_SAME_VOLUME:
				case LOCK_SAME:
					d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					CreateSbi(inDev, in);
					Finished = TRUE;
					break;
				default:
					break;
					}

				UnLock(DeviceIconLock);
				}
			}
		}

	ScalosUnLockIconList(iwtMain);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}



static struct ScaIconNode *IconAlreadyExists(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di)
{
	struct ScaIconNode *in;
	struct ScaIconNode *inFound = NULL;

	ScalosLockIconListShared(iwt);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	for (in=iwt->iwt_WindowTask.wt_IconList; NULL == inFound && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		if (in->in_DeviceIcon && in->in_DeviceIcon->di_Handler == di->di_Handler)
			{
			d1(kprintf("%s/%s/%ld:  Found in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
			inFound = in;
			}
		}

	if (NULL == inFound)
		ScalosUnLockIconList(iwt);

	return inFound;
}


static BOOL FindIconInDrawerWindows(BPTR dirLock, CONST_STRPTR IconName)
{
	struct ScaWindowStruct *ws;
	BOOL Found = FALSE;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; !Found && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

		if (ws->ws_Lock && LOCK_SAME == SameLock(dirLock, ws->ws_Lock))
			{
			struct ScaIconNode *in;

			ScalosLockIconListShared(iwtx);

			for (in=iwtx->iwt_WindowTask.wt_IconList; !Found && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				if (0 == Stricmp((STRPTR) IconName, (STRPTR) GetIconName(in)))
					Found = TRUE;
				}

			ScalosUnLockIconList(iwtx);
			}
		}

	if (!Found)
		{
		// Search in desktop window if backdrop icon is already present
		struct internalScaWindowTask *iwtx;
		struct ScaIconNode *in;

		ws = iInfos.xii_iinfos.ii_MainWindowStruct;
		iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

		ScalosLockIconListShared(iwtx);

		for (in=iwtx->iwt_WindowTask.wt_IconList; !Found && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (in->in_Lock && LOCK_SAME == SameLock(dirLock, in->in_Lock)
				&& (0 == Stricmp((STRPTR) IconName, (STRPTR) GetIconName(in))))
				Found = TRUE;
			}

		ScalosUnLockIconList(iwtx);
		}
	SCA_UnLockWindowList();

	return Found;
}


static Object *ReadDefaultIconByName(struct internalScaWindowTask *iwt, CONST_STRPTR iconName, STRPTR DevName)
{
	Object *iconObj;

	d1(kprintf("%s/%s/%ld: iconName=<%s>\n", __FILE__, __FUNC__, __LINE__, iconName));

	iconObj = NewIconObjectTags(iconName,
		IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
		IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
		IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
		IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
		IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
		IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
		IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
		IDTA_FrameType, CurrentPrefs.pref_FrameType,
		IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
		IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
		IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
		IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
		IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
		IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
		IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
		IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
		IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
		IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
		IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
		IDTA_TextMode, CurrentPrefs.pref_TextMode,
		IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
		IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
		IDTA_Font, (ULONG) iwt->iwt_IconFont,
		IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
		IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
		IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes,
		IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
		IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
		TAG_END);

	if (iconObj)
		{
		ULONG IconType;

		GetAttr(IDTA_Type, iconObj, &IconType);

		d1(kprintf("%s/%s/%ld: iconObj=%08lx  IconType=%ld\n", __FILE__, __FUNC__, __LINE__, iconObj, IconType));

		SetAttrs(iconObj, 
			IDTA_Type, WBDISK,
			IDTA_Text, (ULONG) DevName,
			TAG_END);
		}

	d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

	return iconObj;
}


static BOOL StartDeviceIconNotify(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di)
{
	BOOL Success = FALSE;

	if (di->di_NotifyReq.nr_Name)
		{
		ScalosEndNotify(&di->di_NotifyReq);
		memset(&di->di_NotifyReq, 0, sizeof(di->di_NotifyReq));
		}
	if (di->di_DiskIconName)
		{
		FreePathBuffer(di->di_DiskIconName);
		di->di_DiskIconName = NULL;
		}

	do	{
		BPTR fLock;
		CONST_STRPTR NotifyReqName = "disk.info";

		di->di_DiskIconName = AllocPathBuffer();
		if (NULL == di->di_DiskIconName)
			break;

		fLock = Lock(NotifyReqName, ACCESS_READ);
		if (BNULL != fLock)
			{
			if (!NameFromLock(fLock, di->di_DiskIconName, Max_PathLen))
				{
				FreePathBuffer(di->di_DiskIconName);
				di->di_DiskIconName = NULL;
				}

			UnLock(fLock);
			}
		else
			{
			stccpy(di->di_DiskIconName, di->di_Device, Max_PathLen);
			AddPart(di->di_DiskIconName, NotifyReqName, Max_PathLen);
			}

		d1(kprintf("%s/%s/%ld: start notify for <%s>\n", __FILE__, __FUNC__, __LINE__, di->di_Device));

		di->di_NotifyReq.nr_Name = di->di_Device;
		di->di_NotifyReq.nr_stuff.nr_Msg.nr_Port = iwt->iwt_WindowTask.wt_IconPort;
		di->di_NotifyReq.nr_Flags = NRF_SEND_MESSAGE;

		Success = StartNotify(&di->di_NotifyReq);
		} while (0);

	return Success;
}


static void SetDeviceIconSupportsFlags(struct ScaIconNode *in)
{
	if (NULL == in)
		return;

	in->in_SupportFlags |= INF_SupportsOpen | INF_SupportsCopy | INF_SupportsInformation
		| INF_SupportsSnapshot | INF_SupportsUnSnapshot | INF_SupportsRename;

	if (in->in_DeviceIcon)
		{
		if (ID_WRITE_PROTECTED != in->in_DeviceIcon->di_Info->id_DiskState)
			in->in_SupportFlags |= INF_SupportsFormatDisk;

		d1(kprintf("%s/%s/%ld: <%s> DiskType=%08lx  DiskState=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			GetIconName(in), in->in_DeviceIcon->di_Info->id_DiskType, in->in_DeviceIcon->di_Info->id_DiskState));

		SetIconSupportsFlags(in, ClassCheckInfoData(in->in_DeviceIcon->di_Info));

		if (NULL == in->in_DeviceIcon->di_Volume)
			{
			in->in_SupportFlags |= INF_SupportsFormatDisk;
			in->in_SupportFlags &= ~(INF_SupportsOpen | INF_SupportsCopy | INF_SupportsInformation
				| INF_SupportsSnapshot | INF_SupportsUnSnapshot | INF_SupportsRename);
			}
		}
}


static void SetBackdropIconSupportsFlags(struct internalScaWindowTask *iwt, 
	struct BackDropList *bdl, struct ScaIconNode *in)
{
	BOOL isWriteable;

	if (NULL == in)
		return;

	isWriteable = ClassIsDiskWritable(in->in_Lock);

	in->in_SupportFlags |= INF_SupportsOpen | INF_SupportsCopy | INF_SupportsInformation;

	d1(kprintf("%s/%s/%ld: <%s>  isWriteable=%ld\n", __FILE__, __FUNC__, __LINE__, GetIconName(in), isWriteable));
	if (isWriteable)
		{
		in->in_SupportFlags |= INF_SupportsSnapshot | INF_SupportsUnSnapshot 
			| INF_SupportsDelete | INF_SupportsRename;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (IsPermanentBackDropIcon(iwt, bdl, in->in_Lock, GetIconName(in)))
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			in->in_SupportFlags |= INF_SupportsPutAway;
			in->in_SupportFlags &= ~INF_SupportsLeaveOut;
			}
		else
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			in->in_SupportFlags &= ~INF_SupportsPutAway;
			in->in_SupportFlags |= INF_SupportsLeaveOut;
			}
		}

	SetIconSupportsFlags(in, isWriteable);
}


