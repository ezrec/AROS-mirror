// Class.c
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
#include <proto/cybergraphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/rexxsyslib.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

struct ScaMakeWbArg
	{
	struct ScaIconNode *mwa_IconNode;
	struct WBArg *mwa_Buffer;
	};

struct FormatDateHookData
	{
	STRPTR fdhd_Buffer;		// buffer to write characters to
	size_t fdhd_Length;		// length of buffer
	};

//----------------------------------------------------------------------------

// local functions

static BOOL IsSameWindowIcon(struct internalScaWindowTask *iwtSrc, 
	struct internalScaWindowTask *iwtDest, struct ScaIconNode *inDest);
static SAVEDS(void) FormatDateHookFunc(struct Hook *theHook, struct Locale *locale, char ch);
static BOOL isParentOf(BPTR srcDirLock, struct DragNode *dragList, BPTR dirLock, CONST_STRPTR objName);
static BPTR LockIcon(BPTR dirLock, struct ScaIconNode *in);
static void NotifyAppIconSelected(struct ScaWindowStruct *ws, Object *IconObj, BOOL Selected);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

// iwt is Destination window
// returns BOOL :
//	TRUE	yes, we may drop here
//	FALSE	no, no drop allowed
ULONG ClassDragQuery(struct DragEnter *drge, struct internalScaWindowTask *iwt)
{
	ULONG Result = TRUE;
	struct internalScaWindowTask *iwtSrc = (struct internalScaWindowTask *) drge->drage_Window->ws_WindowTask;
	struct DragNode *dn;
	BOOL SrcIsAppIcon = FALSE;
	BOOL SrcIsGarbage = FALSE;
	BOOL SrcIsKick = FALSE;
	BOOL SrcIsNoDrag = FALSE;
	BOOL SrcIsDiskOrDrawer = FALSE;

	// Check for source icons which may not be dropped on any other icon
	for (dn = iwtSrc->iwt_DragNodeList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
		{
		ULONG SrcIconType;
		STRPTR tt;

		GetAttr(IDTA_Type, dn->drgn_icon, &SrcIconType);

		tt = NULL;
		if (DoMethod(dn->drgn_icon, IDTM_FindToolType, "SCALOS_NODRAG", &tt))
			SrcIsNoDrag = TRUE;

		switch (SrcIconType)
			{
		case WBAPPICON:
			SrcIsAppIcon = TRUE;
			break;
		case WBGARBAGE:
			SrcIsGarbage = TRUE;
			break;
		case WBKICK:
			SrcIsKick = TRUE;
			break;
		case WBDISK:
		case WBDRAWER:
		case WBDEVICE:
		case WB_TEXTICON_DRAWER:
			SrcIsDiskOrDrawer = TRUE;
			break;
			}
		}

	if (drge->drage_Window && iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		{
		// Device icons may not be copied into window on same volume

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		for (dn = iwtSrc->iwt_DragNodeList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: icon=%08lx <%s>  Type=%ld  DeviceIcon=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, drge->drage_Icon, drge->drage_Icon->in_Name, dn->drgn_iconnode->in_DeviceIcon));

			if (dn->drgn_iconnode->in_DeviceIcon)
				{
				BPTR devLock = (BPTR)NULL;

				if (dn->drgn_iconnode->in_DeviceIcon->di_Volume)
					devLock = Lock(dn->drgn_iconnode->in_DeviceIcon->di_Volume, ACCESS_READ);

				if ((BPTR)NULL == devLock && dn->drgn_iconnode->in_DeviceIcon->di_Device)
					devLock = Lock(dn->drgn_iconnode->in_DeviceIcon->di_Device, ACCESS_READ);

				if (devLock)
					{
					LONG sameLockVal = ScaSameLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, devLock);

					debugLock_d1(devLock);
					debugLock_d1(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
					d1(kprintf("%s/%s/%ld: ScaSameLock()=%ld\n", __FILE__, __FUNC__, __LINE__, sameLockVal));

					UnLock(devLock);

					if (LOCK_SAME == sameLockVal)
						return FALSE;
					if (LOCK_SAME_VOLUME == sameLockVal)
						return FALSE;
					}
				}
			}

		}

	if (drge->drage_Icon)
		{
		BOOL SameWindow;

		// Kick, Garbage, AppIcons may not be dropped on other icons
		if (SrcIsKick || SrcIsGarbage || SrcIsAppIcon)
			return FALSE;

		if (drge->drage_Icon->in_Icon)
			{
			// query icon if we may drop here
			ULONG DestIconType;

			GetAttr(IDTA_Type, drge->drage_Icon->in_Icon, &DestIconType);

			d1(KPrintF("%s/%s/%ld: icon=%08lx <%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, drge->drage_Icon, drge->drage_Icon->in_Name, DestIconType));

			switch (DestIconType)
				{
			case WBAPPICON:
			case WBGARBAGE:
				Result = TRUE;
				break;
			case WBTOOL:
				Result = CurrentPrefs.pref_DragStartFlag ? TRUE : FALSE;
				break;
			case WBKICK:
			case WBPROJECT:
			case WB_TEXTICON_TOOL:
				Result = FALSE;
				break;
			case WBDISK:
			case WBDRAWER:
			case WBDEVICE:
			case WB_TEXTICON_DRAWER:
				SameWindow = IsSameWindowIcon(iwtSrc, iwt, drge->drage_Icon);

				d1(KPrintF("%s/%s/%ld: SameWindow=%ld  SrcIsNoDrag=%ld\n", __FILE__, __FUNC__, __LINE__, SameWindow, SrcIsNoDrag));
				if (SameWindow && SrcIsNoDrag)
					{
					Result = FALSE;
					break;
					}

				if (drge->drage_Icon->in_DeviceIcon)
					{
					if (!ClassCheckInfoData(drge->drage_Icon->in_DeviceIcon->di_Info))
						Result = FALSE;
					}
				else
					{
					Result = !(drge->drage_Icon->in_Flags & INF_VolumeWriteProtected);
					}

				if (SrcIsDiskOrDrawer && Result)
					{
					BPTR DestLock;

					if (drge->drage_Icon->in_Lock)
						DestLock = drge->drage_Icon->in_Lock;
					else
						DestLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

					d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
					Result = !isParentOf(iwtSrc->iwt_WindowTask.mt_WindowStruct->ws_Lock, iwtSrc->iwt_DragNodeList, 
						DestLock, GetIconName(drge->drage_Icon));
					d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
					}
				break;
			default:
				Result = TRUE;
				break;
				}
			}

		}
	else
		{
		// query window if we may drop here

		// AppIcons may not be dropped into different windows
		if (SrcIsAppIcon && iwt != iwtSrc)
			return FALSE;

		if (iwt == iwtSrc)
			{
			// drop in same window
			if (SrcIsNoDrag)
				return FALSE;	// SCALOS_NODRAG icons must not be dropped in same window

			return TRUE;	// always may drop in same window (no move/copy)
			}

		if ((BPTR)NULL == iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock && SrcIsNoDrag)
			return FALSE;		// SCALOS_NODRAG icons must not be dropped in desktop window

		if (SrcIsDiskOrDrawer)
			{
			if (isParentOf(iwtSrc->iwt_WindowTask.mt_WindowStruct->ws_Lock, iwtSrc->iwt_DragNodeList, 
					iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, NULL))
				{
				return FALSE;
				}
			}

		Result = ClassIsDiskWritable(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
		}

	return Result;
}


// return TRUE if Disk is writable
ULONG ClassCheckInfoData(const struct InfoData *info)
{
	ULONG Result = TRUE;

	d1(kprintf("%s/%s/%ld: DiskType=%ld  DiskState=%ld\n", __FILE__, __FUNC__, __LINE__, info->id_DiskType, info->id_DiskState));

	switch (info->id_DiskType)
		{
	case ID_NO_DISK_PRESENT:
	case ID_UNREADABLE_DISK:
	case ID_NOT_REALLY_DOS:
		Result = FALSE;
		break;
		}

	if (ID_WRITE_PROTECTED == info->id_DiskState)
		Result = FALSE;

	return Result;
}


// Note: it is VERY IMPORTANT that for every call to ClassDragEnter() (icon or window) you execute
// a corresponding ClassDragLeave() (icon or window) call !!!
ULONG ClassDragEnter(struct DragEnter *drge, struct internalScaWindowTask *iwtDest)
{
	struct internalScaWindowTask *iwtSrc = (struct internalScaWindowTask *) drge->drage_Window->ws_WindowTask;
	struct DragHandle *dh = iwtSrc->iwt_myDragHandle;
	ULONG WasLocked;

	// iwtDest : Window dragged into
	// iwtSrc : Window dragged from (where dragging started)

	d1(kprintf("%s/%s/%ld: Started. \n", __FILE__, __FUNC__, __LINE__));

	d1(kprintf("%s/%s/%ld: iwtDest=%08lx  icon=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtDest, drge->drage_Icon));

	if (dh)
		ClassHideDragBobs(iwtDest, dh);

	WasLocked = SCA_UnlockDrag(dh);

	if (drge->drage_Icon)
		{
		// Pointer entered Icon area
		// and left window area

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (iwtSrc->iwt_DragMayDrop)
			{
			ULONG IconType;

			d1(kprintf("%s/%s/%ld: icon=%08lx <%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, drge->drage_Icon, drge->drage_Icon->in_Name));

			ClassSelectIcon(iwtDest->iwt_WindowTask.mt_WindowStruct, drge->drage_Icon, TRUE);

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			GetAttr(IDTA_Type, drge->drage_Icon->in_Icon, &IconType);
			switch (IconType)
				{
			case WBDRAWER:
			case WBDISK:
			case WBGARBAGE:
			case WB_TEXTICON_DRAWER:
				DoMethod(iwtSrc->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_StartPopOpenTimer,
					iwtDest,
					dh, drge->drage_Icon);
				break;
			default:
				break;
				}

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			DisplayIconDropMark(iwtDest, drge->drage_Icon);
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}
	else
		{

		d1(kprintf("%s/%s/%ld: DisplayWindowDropMark(%s) \n", __FILE__, __FUNC__, __LINE__,  iwtDest->iwt_WinTitle));

		DisplayWindowDropMark(iwtDest);

		DoMethod(iwtDest->iwt_WindowTask.mt_WindowObject, SCCM_Window_LockUpdate, TRUE);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ReLockDrag(dh, iwtSrc, WasLocked);

	d1(kprintf("%s/%s/%ld: Finished. \n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


// Note: it is VERY IMPORTANT that for every call to ClassDragEnter() (icon or window) you execute
// a corresponding ClassDragLeave() (icon or window) call !!!
ULONG ClassDragLeave(struct DragEnter *drge, struct internalScaWindowTask *iwtDest)
{
	struct internalScaWindowTask *iwtSrc = (struct internalScaWindowTask *) drge->drage_Window->ws_WindowTask;
	struct DragHandle *dh = iwtSrc->iwt_myDragHandle;
	ULONG WasLocked;

	// iwtDest : Window dragged into
	// iwtSrc : Window dragged from (where dragging started)

	d1(kprintf("%s/%s/%ld: Started. \n", __FILE__, __FUNC__, __LINE__));
	d1(kprintf("%s/%s/%ld: iwtDest=%08lx  title=<%s> icon=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtDest, iwtDest->iwt_WinTitle, drge->drage_Icon));

	if (dh)
		ClassHideDragBobs(iwtDest, dh);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	WasLocked = SCA_UnlockDrag(dh);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(iwtSrc->iwt_WindowTask.mt_MainObject, SCCM_IconWin_StopPopOpenTimer, dh);

	if (drge->drage_Icon)
		{
		// pointer left icon area
		// and is back in window area
		struct Gadget *gg = (struct Gadget *) drge->drage_Icon->in_Icon;

		if (gg->Flags & GFLG_SELECTED)
			{
			// remove icon drop mark
			d1(kprintf("%s/%s/%ld: icon=%08lx <%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, drge->drage_Icon, drge->drage_Icon->in_Name));

			EraseIconDropMark(iwtDest, drge->drage_Icon);
			ClassSelectIcon(iwtDest->iwt_WindowTask.mt_WindowStruct, drge->drage_Icon, FALSE);
			}
		}
	else
		{
		// Pointer left window area
		d1(kprintf("%s/%s/%ld: CALL EraseWindowDropMark(%s)\n", __FILE__, __FUNC__, __LINE__ , iwtDest->iwt_WinTitle));

		iwtDest->iwt_DragScrollX = iwtDest->iwt_DragScrollY = 0;

		EraseWindowDropMark(iwtDest);

		DoMethod(iwtDest->iwt_WindowTask.mt_WindowObject, SCCM_Window_UnlockUpdate);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ReLockDrag(dh, iwtSrc, WasLocked);

	d1(kprintf("%s/%s/%ld: Finished. \n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


void ClassHideDragBobs(struct internalScaWindowTask *iwt, struct DragHandle *dh)
{
	SCA_DrawDrag(dh, iwt->iwt_WinScreen->Width + 400, 0, SCAF_Drag_Hide);
}


void ClassSelectIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon, BOOL Selected)
{
	d1(kprintf("%s/%s/%ld: swi=%08lx  sIcon=%08lx  Selected=%ld\n", __FILE__, __FUNC__, __LINE__, swi, sIcon, Selected);)

	ClassSelectIconObj(swi, sIcon->in_Icon, Selected);
}


void ClassSelectIconObj(struct ScaWindowStruct *swi, Object *IconObj, BOOL Selected)
{
	struct ExtGadget *gg = (struct ExtGadget *) IconObj;

	d1(KPrintF("%s/%s/%ld: swi=%08lx  IconObj=%08lx  Selected=%ld\n", __FILE__, __FUNC__, __LINE__, swi, IconObj, Selected);)

	if ( (!Selected && (gg->Flags & GFLG_SELECTED)) || (Selected && !(gg->Flags & GFLG_SELECTED)))
		{
		struct Window *win = swi->ws_Window;

		SetAttrs(IconObj,
			GA_Selected, Selected,
			TAG_END);

		if (Selected)
			{
			if (NULL == gg->SelectRender)
				{
				DoMethod(swi->ws_WindowTask->mt_MainObject,
					SCCM_IconWin_LayoutIcon, IconObj,
					IOLAYOUTF_SelectedImage, 0);
				}
			}
		else
			{
			if (NULL == gg->GadgetRender)
				{
				DoMethod(swi->ws_WindowTask->mt_MainObject,
					SCCM_IconWin_LayoutIcon, IconObj,
					IOLAYOUTF_NormalImage);
				}
			}

		if (win)
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) swi->ws_WindowTask;

			d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			DoMethod(swi->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_DrawIcon, IconObj);

			if (iwt->iwt_DragFlags & DRAGFLAGF_WindowMarked)
				{
				// Restore the part of the Window drop mark
				// that has been destroyed by the icon redraw
				struct Region *iconRegion;

				iconRegion = NewRegion();
				d1(kprintf("Restore the part of the Window drop mark\nthat has been destroyed by the icon redraw: \n"));
				d1(kprintf("%s/%s/%ld: iconRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iconRegion));
				if (iconRegion)
					{
					struct Rectangle IconRect;

					IconRect.MinX = IconRect.MaxX = gg->BoundsLeftEdge - iwt->iwt_WindowTask.wt_XOffset + iwt->iwt_InnerLeft;
					IconRect.MinY = IconRect.MaxY = gg->BoundsTopEdge - iwt->iwt_WindowTask.wt_YOffset + iwt->iwt_InnerTop;
					IconRect.MaxX += gg->BoundsWidth - 1;
					IconRect.MaxY += gg->BoundsHeight - 1;

					d1(kprintf("%s/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", \
						__FILE__, __FUNC__, __LINE__, IconRect.MinX, IconRect.MinY, IconRect.MaxX, IconRect.MaxY));

					OrRectRegion(iconRegion, &IconRect);

					d1(kprintf("%s/%s/%ld: CALL  DrawWindowDropMark(iwt, iconRegion)\n"));
					RedrawWindowDropMark(iwt, iconRegion);

					DisposeRegion(iconRegion);
					}
				}
			}

		NotifyAppIconSelected(swi, IconObj, Selected);
		}
}


BOOL ClassIsDiskWritable(BPTR dLock)
{
	struct InfoData *info = ScalosAllocInfoData();
	BOOL Result = TRUE;

	if (info)
		{
		Info(dLock, info);

		if (!ClassCheckInfoData(info))
			Result = FALSE;

		ScalosFreeInfoData(&info);
		}

	return Result;
}


ULONG ClassWinTimerMsg(struct internalScaWindowTask *iwt, struct Message *Msg, APTR p)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>  MoveGadId=%ld  ws_Flags=%04lx\n", __FILE__, __FUNC__, __LINE__, \
		iwt, iwt->iwt_WinTitle, iwt->iwt_MoveGadId, iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags));

	if (iwt->iwt_LockFlag)
		{
		struct SM_Timer *smtm = (struct SM_Timer *) Msg;
		ULONG Seconds, Micros;

		if (smtm->smtm_Time.tv_secs >= iwt->iwt_LockCount.tv_secs)
			Seconds = smtm->smtm_Time.tv_secs - iwt->iwt_LockCount.tv_secs;
		else
			Seconds = 0;

		if (smtm->smtm_Time.tv_micro >= iwt->iwt_LockCount.tv_micro)
			Micros = smtm->smtm_Time.tv_micro - iwt->iwt_LockCount.tv_micro;
		else
			Micros = 0;

		d1(kprintf("%s/%s/%ld: iwt_LockCount: Seconds=%lu  Micros=%lu\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_LockCount.tv_secs, iwt->iwt_LockCount.tv_micro));
		d1(kprintf("%s/%s/%ld: Seconds=%lu  Micros=%lu\n", __FILE__, __FUNC__, __LINE__, Seconds, Micros));

		if (Micros > 500000)
			Seconds++;

		if (Seconds >= 2)
			{
			d1(kprintf("%s/%s/%ld: try to AbortFunctions() \n", __FILE__, __FUNC__, __LINE__));

			AbortFunctions(iwt);

			if (iwt->iwt_WindowTask.wt_Window)
				ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags);

			if (!iwt->iwt_NeedsTimerFlag && !(iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & (WSV_FlagF_CheckUpdatePending | WSV_FlagF_Typing)))
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_NeedsTimerMsg;
			}
		}

	if (!iwt->iwt_LockFlag)
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_Ping);
		d1(KPrintF("%s/%s/%ld:  DOMETHOD mt_MainObject [SCCM_Ping]\n", __FILE__, __FUNC__, __LINE__));
		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Ping);
		d1(KPrintF("%s/%s/%ld:  DOMETHOD mt_WindowObject [SCCM_Ping]\n", __FILE__, __FUNC__, __LINE__));
		}

	d1(kprintf("%s/%s/%ld: Finished \n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


void ClassTimerToolTipMsg(struct internalScaWindowTask *iwt)
{
	struct ScaIconNode *iconUnderPointer;
	struct internalScaWindowTask *iwtUnderPointer;
	struct internalScaWindowTask *iwtActiveWindow = NULL;
	struct Window *foreignWindow;
	struct ScaWindowStruct *ws;
	BOOL LockFlag = FALSE;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

		if (iwtx->iwt_LockFlag)
			LockFlag = TRUE;

		if (ws->ws_Window && ws->ws_Window->Flags & WFLG_WINDOWACTIVE)
			{
			iwtActiveWindow = (struct internalScaWindowTask *) ws->ws_WindowTask;
			break;
			}
		}

	SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: iwtActiveWindow=%08lx \n", __FILE__, __FUNC__, __LINE__, iwtActiveWindow));

	if (NULL == iwtActiveWindow)
		{
		// return if no Scalos window active
		MainWindowTask->miwt_IconUnderPtrCount = 0;
		MainWindowTask->miwt_LastIconUnderPtr = NULL;
		MainWindowTask->miwt_LastGadgetUnderPtr = SGTT_GADGETID_unknown;

		return;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (!LockFlag)
		{
		enum sgttGadgetIDs gadgetUnderPointer = iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetID;

		QueryObjectUnderPointer(&iwtUnderPointer, &iconUnderPointer, NULL, &foreignWindow);

		d1(KPrintF("%s/%s/%ld: iconUnderPointer=%08lx\n", __FILE__, __FUNC__, __LINE__, iconUnderPointer));
		d1(KPrintF("%s/%s/%ld: gadgetUnderPointer=%ld\n", __FILE__, __FUNC__, __LINE__, gadgetUnderPointer));

		if (iwtUnderPointer != iInfos.xii_GlobalGadgetUnderPointer.ggd_iwt)
			gadgetUnderPointer = SGTT_GADGETID_unknown;

		d1(KPrintF("%s/%s/%ld: gadgetUnderPointer=%ld\n", __FILE__, __FUNC__, __LINE__, gadgetUnderPointer));

		if ((SGTT_GADGETID_unknown == MainWindowTask->miwt_LastGadgetUnderPtr
			 || gadgetUnderPointer != MainWindowTask->miwt_LastGadgetUnderPtr)
			&&
			(NULL == iconUnderPointer ||
				iconUnderPointer != MainWindowTask->miwt_LastIconUnderPtr))
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			MainWindowTask->miwt_IconUnderPtrCount = 0;
			MainWindowTask->miwt_LastIconUnderPtr = iconUnderPointer;
			MainWindowTask->miwt_LastGadgetUnderPtr = gadgetUnderPointer;
			}
		else
			{
			d1(KPrintF("%s/%s/%ld: iwt_IconUnderPtrCount=%ld\n", __FILE__, __FUNC__, __LINE__, MainWindowTask->miwt_IconUnderPtrCount));

			if (NULL == iwtUnderPointer || VGADGETID_IDLE != iwtActiveWindow->iwt_MoveGadId)
				{
				d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				MainWindowTask->miwt_IconUnderPtrCount = 0;
				MainWindowTask->miwt_LastIconUnderPtr = NULL;
				MainWindowTask->miwt_LastGadgetUnderPtr = SGTT_GADGETID_unknown;
				}
			else
				{
				if (++MainWindowTask->miwt_IconUnderPtrCount == CurrentPrefs.pref_ToolTipDelaySeconds)
					{
					d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					if (iconUnderPointer)
						{
						d1(KPrintF("%s/%s/%ld: iconUnderPointer=%08lx  <%s>\n", \
							__FILE__, __FUNC__, __LINE__, iconUnderPointer, iconUnderPointer->in_Name));

						DoMethod(iwtUnderPointer->iwt_WindowTask.mt_MainObject, 
							SCCM_IconWin_ShowIconToolTip, iconUnderPointer);
						}
					else if (SGTT_GADGETID_unknown != gadgetUnderPointer)
						{
						d1(KPrintF("%s/%s/%ld: gadgetUnderPointer=%08lx\n", \
							__FILE__, __FUNC__, __LINE__, gadgetUnderPointer));

						DoMethod(iwtUnderPointer->iwt_WindowTask.mt_MainObject, 
							SCCM_IconWin_ShowGadgetToolTip, gadgetUnderPointer);
						}
					}
				}
			}


		if (iwtUnderPointer)
			{
			if (iconUnderPointer)
				ScalosUnLockIconList(iwtUnderPointer);
			SCA_UnLockWindowList();
			}
		}
}


// Check if <inDest> is the disk/drawer icon for <inDest>
static BOOL IsSameWindowIcon(struct internalScaWindowTask *iwtSrc, 
	struct internalScaWindowTask *iwtDest, struct ScaIconNode *inDest)
{
	BPTR iLock = (BPTR)NULL;
	BOOL SameWindow = FALSE;

	d1(KPrintF("%s/%s/%ld: iwtSrc=%08lx  iwtDest=%08lx  inDest=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwtSrc, iwtDest, inDest));

	if (inDest->in_DeviceIcon)
		{
		if (inDest->in_DeviceIcon->di_Volume)
			iLock = Lock(inDest->in_DeviceIcon->di_Volume, ACCESS_READ);
		}
	else
		{
		BPTR oldDir;

		if (inDest->in_Lock)
			{
			// backdrop icon on desktop
			oldDir = CurrentDir(inDest->in_Lock);
			}
		else
			{
			oldDir = CurrentDir(iwtDest->iwt_WindowTask.mt_WindowStruct->ws_Lock);
			}

		iLock = Lock(inDest->in_Name, ACCESS_READ);

		CurrentDir(oldDir);
		}

	if (iLock)
		{
		debugLock_d1(iLock);
		debugLock_d1(iwtSrc->iwt_WindowTask.mt_WindowStruct->ws_Lock);

		SameWindow = LOCK_SAME == ScaSameLock(iwtSrc->iwt_WindowTask.mt_WindowStruct->ws_Lock, iLock);
		UnLock(iLock);
		}

	return SameWindow;
}


// find corresponding (parent) icon for given Scalos window
Object *ClassGetWindowIconObject(struct internalScaWindowTask *iwt, Object **allocIconObj)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	Object *IconObj = NULL;
	LONG DirEntryType;
	BPTR parentLock;
	BPTR oldDir = NOT_A_LOCK;
	CONST_STRPTR iconName;

	do	{
		if (ws->ws_Flags & WSV_FlagF_RootWindow)
			{
			parentLock = DupLock(ws->ws_Lock);

			iconName = "disk";
			DirEntryType = ST_ROOT;

			// first look in main window's device icon list
			if (parentLock)
				{
				struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
				struct ScaDeviceIcon *sdi;
				struct FileLock *fLock = BADDR(parentLock);

				for (sdi=iwtMain->iwt_DevIconList; sdi; sdi = (struct ScaDeviceIcon *) sdi->di_Node.mln_Succ)
					{
					if (sdi->di_Handler == fLock->fl_Task)
						{
						d1(kprintf("%s/%s/%ld: sdi=%08lx  Icon=%08lx\n", __FILE__, __FUNC__, __LINE__, sdi, sdi->di_Icon));
						IconObj = sdi->di_Icon;
						}
					}
				}
			}
		else
			{
			parentLock = ParentDir(ws->ws_Lock);

			iconName = ws->ws_Name;
			DirEntryType = ST_USERDIR;
			}

		if ((BPTR)NULL == parentLock)
			break;

		oldDir = CurrentDir(parentLock);

		if (NULL == IconObj)
			IconObj = *allocIconObj = NewIconObject(iconName, NULL);

		d1(kprintf("%s/%s/%ld: iconName=<%s>  IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconName, IconObj));
		if (NULL == IconObj)
			{
			ULONG Protection = 0L;
			STRPTR IconPath;

			do	{
				IconPath = AllocPathBuffer();
				if (NULL == IconPath)
					break;

				if (!NameFromLock(parentLock, IconPath, Max_PathLen-1))
					break;

				if (!AddPart(IconPath, iconName, Max_PathLen-1))
					break;

				IconObj = *allocIconObj = (Object *) DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_GetDefIcon,
					IconPath, DirEntryType, Protection, ICONTYPE_NONE);
				} while (0);

			if (IconPath)
				FreePathBuffer(IconPath);
			}
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (parentLock)
		UnLock(parentLock);

	return IconObj;
}


void ClassFormatDate(struct DateTime *dt, ULONG DateMaxLen, ULONG TimeMaxLen)
{
	if (ScalosLocale)
		{
		struct FormatDateHookData fd;
		struct Hook fmtHook;

		d1(kprintf("%s/%s/%ld: DateBuff=%08lx Len=%ld  TimeBuff=%08lx Len=%ld\n", \
			__FILE__, __FUNC__, __LINE__, dt->dat_StrDate, DateMaxLen, dt->dat_StrTime, DateMaxLen));

		SETHOOKFUNC(fmtHook, FormatDateHookFunc);
		fmtHook.h_Data = &fd;

		fd.fdhd_Buffer = dt->dat_StrDate;
		fd.fdhd_Length = DateMaxLen;

		FormatDate(ScalosLocale, ScalosLocale->loc_ShortDateFormat,
			&dt->dat_Stamp, &fmtHook);

		fd.fdhd_Buffer = dt->dat_StrTime;
		fd.fdhd_Length = TimeMaxLen;

		FormatDate(ScalosLocale, ScalosLocale->loc_ShortTimeFormat,
			&dt->dat_Stamp, &fmtHook);
		}
	else
		{
		DateToStr(dt);		// no size checking possible here :(
		}
}


static SAVEDS(void) FormatDateHookFunc(struct Hook *theHook, struct Locale *locale, char ch)
{
	struct FormatDateHookData *fd = (struct FormatDateHookData *) theHook->h_Data;

	(void) locale;

	if (fd->fdhd_Length >= 1)
		{
		*(fd->fdhd_Buffer)++ = ch;
		fd->fdhd_Length--;
		}
}


// forbid drag-copying or -moving icon into own sibling window or on sibling icon
// to avoid errors or endless recursion
static BOOL isParentOf(BPTR srcDirLock, struct DragNode *dragList, BPTR dirLock, CONST_STRPTR objName)
{
	BPTR destLock;
	BOOL isParent = FALSE;
	BPTR oldDir;

	d1(kprintf("%s/%s/%ld: objName=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, objName, objName ? objName : ""));

	if (objName)
		{
		BPTR oldDir = CurrentDir(dirLock);

		destLock = Lock((STRPTR) objName, ACCESS_READ);

		CurrentDir(oldDir);
		}
	else
		destLock = DupLock(dirLock);

	debugLock_d1(destLock);

	if ((BPTR)NULL == destLock)
		return FALSE;

	oldDir = CurrentDir(srcDirLock);

	while (dragList && !isParent)
		{
		BPTR srcLock;

		srcLock = LockIcon(srcDirLock, dragList->drgn_iconnode);

		debugLock_d1(srcLock);
		if (srcLock)
			{
			BPTR prevLock = (BPTR)NULL;
			BPTR parentLock;

			switch (ScaSameLock(srcLock, destLock))
				{
			case LOCK_SAME:
				return isParent = TRUE;

			case LOCK_SAME_VOLUME:
				parentLock = destLock;
				do	{
					parentLock = ParentDir(parentLock);
					if (prevLock)
						UnLock(prevLock);

					debugLock_d1(parentLock);

					if (LOCK_SAME == ScaSameLock(srcLock, parentLock))
						isParent = TRUE;

					d1(kprintf("%s/%s/%ld: isParent=%ld\n", __FILE__, __FUNC__, __LINE__, isParent));

					prevLock = parentLock;
					} while (parentLock && !isParent);

				if (prevLock)
					UnLock(prevLock);
				break;

			default:
				break;
				}
			UnLock(srcLock);
			}
		dragList = (struct DragNode *) dragList->drgn_Node.mln_Succ;
		}

	CurrentDir(oldDir);
	UnLock(destLock);

	d1(kprintf("%s/%s/%ld: isParent=%ld\n", __FILE__, __FUNC__, __LINE__, isParent));

	return isParent;
}


static BPTR LockIcon(BPTR dirLock, struct ScaIconNode *in)
{
	BPTR lock;
	BPTR oldDir;

	if (in->in_Lock)
		oldDir = CurrentDir(in->in_Lock);
	else
		{
		if (in->in_DeviceIcon)
			return DiskInfoLock(in);
		else
			oldDir = CurrentDir(dirLock);
		}

	lock = Lock((STRPTR) GetIconName(in), ACCESS_READ);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);

	return lock;
}


ULONG ClassCountSelectedIcons(struct internalScaWindowTask *iwt)
{
	struct ScaIconNode *in;
	ULONG Count = 0;

	ScalosLockIconListShared(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct Gadget *gg = (struct Gadget *) in->in_Icon;

		if (gg->Flags & GFLG_SELECTED)
			Count++;
		}

	ScalosUnLockIconList(iwt);

	return Count;
}


static void NotifyAppIconSelected(struct ScaWindowStruct *ws, Object *IconObj, BOOL Selected)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
	ULONG IconType;
	struct AppObject *appo;

	if (ws != iInfos.xii_iinfos.ii_MainWindowStruct)
		return;

	GetAttr(IDTA_Type, IconObj, &IconType);
	if (WBAPPICON != IconType)
		return;

	ScalosObtainSemaphoreShared(iwt->iwt_AppListSemaphore);

	for (appo=iwt->iwt_AppList; appo; appo = (struct AppObject *) appo->appo_Node.mln_Succ)
		{
		if (APPTYPE_AppIcon == appo->appo_type &&
			appo->appo_object.appoo_IconObject == IconObj &&
			appo->appo_TagList &&
			GetTagData(WBAPPICONA_NotifySelectState, FALSE, appo->appo_TagList))
			{
			SendAppMessage(appo, 
				Selected ? AMCLASSICON_Selected : AMCLASSICON_Unselected,
				0, 0);
			break;
			}
		}

	ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);
}


void ClassDragFinish_IconWin(struct ScalosArg **arglist)
{
	struct ScalosArg *scarg, *argNext;

	for (scarg=*arglist; scarg; scarg=argNext)
		{
		argNext = (struct ScalosArg *) scarg->scarg_Node.mln_Succ;

		if (scarg->scarg_lock)
			UnLock(scarg->scarg_lock);
		if (scarg->scarg_name)
			FreeCopyString(scarg->scarg_name);
		if (scarg->scarg_ext)
			FreeCopyString(scarg->scarg_ext);

		SCA_FreeNode((struct ScalosNodeList *) arglist, &scarg->scarg_Node);
		}
}


void ClassDragBegin_DeviceWin(struct ScalosArg **ArgList, struct DragNode **dnList)
{
	ClassDragBegin_IconWin(ArgList, dnList);
}


void ClassDragBegin_IconWin(struct ScalosArg **ArgList, struct DragNode **dnList)
{
	struct DragNode *dn;
	CONST_STRPTR Extension = NULL;
	Object *tempIconObj;

	for (dn=*dnList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
		{
		struct ScalosArg *scarg = (struct ScalosArg *) SCA_AllocStdNode((struct ScalosNodeList *) ArgList, NTYP_ScalosArg);

		if (scarg)
			{
			ULONG IconType = 0;

			scarg->scarg_xpos = dn->drgn_x;
			scarg->scarg_ypos = dn->drgn_y;

			scarg->scarg_DeltaX = dn->drgn_DeltaX;
			scarg->scarg_DeltaY = dn->drgn_DeltaY;

			GetAttr(IDTA_Type, dn->drgn_icon, &IconType);

			switch (IconType)
				{
			case WBDISK:
			case WBKICK:
				//.disk
				scarg->scarg_icontype = WBDISK;

				scarg->scarg_lock = DiskInfoLock(dn->drgn_iconnode);
				if ((BPTR)NULL == scarg->scarg_lock)
					{
					if (NULL == dn->drgn_iconnode->in_DeviceIcon->di_Volume)
						{
						SCA_FreeNode((struct ScalosNodeList *) ArgList, &dn->drgn_Node);
						break;
						}

					scarg->scarg_name = AllocCopyString(dn->drgn_iconnode->in_DeviceIcon->di_Device);
					}
				else
					{
					scarg->scarg_name = AllocCopyString(dn->drgn_iconnode->in_Name);

					GetAttr(IDTA_Extention, dn->drgn_icon, (APTR) &Extension);
					if (Extension)
						scarg->scarg_ext = AllocCopyString(Extension);
					}
				d1(kprintf("%s/%s/%ld: scarg_name=<%s>\n", __FILE__, __FUNC__, __LINE__, scarg->scarg_name));
				break;

			case WBDRAWER:
			case WBGARBAGE:
			case WBDEVICE:
				//.drawer
				scarg->scarg_icontype = WBDRAWER;
				if (dn->drgn_iconnode->in_Lock)
					scarg->scarg_lock = DupLock(dn->drgn_iconnode->in_Lock);
				else
					scarg->scarg_lock = Lock("", ACCESS_READ);

				if ((BPTR)NULL == scarg->scarg_lock)
					{
					SCA_FreeNode((struct ScalosNodeList *) ArgList, &dn->drgn_Node);
					break;
					}

				scarg->scarg_name = AllocCopyString(GetIconName(dn->drgn_iconnode));

				GetAttr(IDTA_Extention, dn->drgn_icon, (APTR) &Extension);
				if (Extension)
					scarg->scarg_ext = AllocCopyString(Extension);
				break;

			case WBTOOL:
			case WBPROJECT:
				//.tool
				scarg->scarg_icontype = WBTOOL;
				if (dn->drgn_iconnode->in_Lock)
					scarg->scarg_lock = DupLock(dn->drgn_iconnode->in_Lock);
				else
					scarg->scarg_lock = Lock("", ACCESS_READ);

				if ((BPTR)NULL == scarg->scarg_lock)
					{
					SCA_FreeNode((struct ScalosNodeList *) ArgList, &dn->drgn_Node);
					break;
					}

				scarg->scarg_name = AllocCopyString(GetIconName(dn->drgn_iconnode));

				GetAttr(IDTA_Extention, dn->drgn_icon, (APTR) &Extension);
				if (Extension)
					scarg->scarg_ext = AllocCopyString(Extension);
				break;

			case WB_TEXTICON_TOOL:
				//.texttool
				scarg->scarg_icontype = WBTOOL;
				if (dn->drgn_iconnode->in_Lock)
					scarg->scarg_lock = DupLock(dn->drgn_iconnode->in_Lock);
				else
					scarg->scarg_lock = Lock("", ACCESS_READ);

				scarg->scarg_name = AllocCopyString(GetIconName(dn->drgn_iconnode));

				tempIconObj = NewIconObject(GetIconName(dn->drgn_iconnode), NULL);

				if (tempIconObj)
					{
					GetAttr(IDTA_Extention, dn->drgn_icon, (APTR) &Extension);
					if (Extension)
						scarg->scarg_ext = AllocCopyString(Extension);

					DisposeIconObject(tempIconObj);
					}
				break;

			case WB_TEXTICON_DRAWER:
				//.textdrawer
				scarg->scarg_icontype = WBDRAWER;
				if (dn->drgn_iconnode->in_Lock)
					scarg->scarg_lock = DupLock(dn->drgn_iconnode->in_Lock);
				else
					scarg->scarg_lock = Lock("", ACCESS_READ);

				scarg->scarg_name = AllocCopyString(GetIconName(dn->drgn_iconnode));

				tempIconObj = NewIconObject(GetIconName(dn->drgn_iconnode), NULL);

				if (tempIconObj)
					{
					GetAttr(IDTA_Extention, dn->drgn_icon, (APTR) &Extension);
					if (Extension)
						scarg->scarg_ext = AllocCopyString(Extension);

					DisposeIconObject(tempIconObj);
					}
				break;

			case WBAPPICON:
				SCA_FreeNode((struct ScalosNodeList *) ArgList, &dn->drgn_Node);
				break;

			default:
				SCA_FreeNode((struct ScalosNodeList *) ArgList, &dn->drgn_Node);
				break;
				}
			}
		}
}


void ClassSetDefaultIconFlags(struct ScaIconNode *in, BOOL IsDefIcon)
{
	ULONG UserFlags;

	GetAttr(IDTA_UserFlags, in->in_Icon, &UserFlags);

	if (IsDefIcon)
		{
		d1(kprintf("%s/%s/%ld: in=<%s>  supportFlags=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, GetIconName(in), in->in_SupportFlags));

		UserFlags |= ICONOBJ_USERFLAGF_DefaultIcon;
		in->in_Flags |= INF_DefaultIcon;
		}
	else
		{
		UserFlags &= ~ICONOBJ_USERFLAGF_DefaultIcon;
		in->in_Flags &= ~INF_DefaultIcon;
		}

	SetAttrs(in->in_Icon, IDTA_UserFlags, UserFlags, TAG_END);
}


