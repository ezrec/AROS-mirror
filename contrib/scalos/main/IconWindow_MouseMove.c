// IconWindow_MouseMove.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/scalosprefs.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//-----------------------------------------------------------------------

// local data definitions

//----------------------------------------------------------------------------

// local functions

static LONG GetSourceDestScaSameLock(struct internalScaWindowTask *iwt);
static BPTR GetIconLock(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static ULONG DragMayCopy(struct internalScaWindowTask *iwt, ULONG Qualifier);
static BOOL IsIconifiedScalosWindow(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static void CheckNeedScroll(struct internalScaWindowTask *iwt, WORD xw, WORD yw);
static void DoDragScroll(struct internalScaWindowTask *iwtSrc, struct internalScaWindowTask *iwtDest);

//----------------------------------------------------------------------------

// local data

static const WORD DRAG_Threshold = 4;

//----------------------------------------------------------------------------

void IDCMPDragMouseMove(struct internalScaWindowTask *iwt, struct IntuiMessage *msg)
{
	struct internalScaWindowTask *swit;
	struct ScaIconNode *in;
	struct ScaIconNode *inOuterBounds;
	struct Window *foreignWin;
	ULONG DrawDragFlags = 0;
	BOOL DestChange = FALSE;	// Flag: mouse is now over different object
	WORD dX, dY;

	dX = msg->MouseX - iwt->iwt_StartDragMouseX;
	if (dX < 0)
		dX = -dX;

	dY = msg->MouseY - iwt->iwt_StartDragMouseY;
	if (dY < 0)
		dY = -dY;

	if (dX >= DRAG_Threshold || dY >= DRAG_Threshold)
		{
		iwt->iwt_StartDragMouseX = (WORD) 0x8000;	// make sure DRAG_Threshold is always hit now
		iwt->iwt_StartDragMouseY = (WORD) 0x8000;

		iwt->iwt_ObjectUnderMouse = OUM_nothing;

		DrawDrag(SCAF_Drag_NoDrawDrag, iwt);	// make sure icons get removed

		if (iwt->iwt_myDragHandle)
			iwt->iwt_myDragHandle->drgh_flags |= DRGF_DragStarted;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		QueryObjectUnderPointer(&swit, &in, &inOuterBounds, &foreignWin);

		d1(kprintf("%s/%s/%ld: swit=%08lx\n", __FILE__, __FUNC__, __LINE__, swit));

		if (swit)
			{
			// Pointer is inside a Scalos Window
			struct ScaIconNode *inOrig = in;
			struct DragNode *dn;
			BOOL IconOverlap = FALSE;
			WORD x, y;

			iwt->iwt_ObjectUnderMouse = OUM_ScalosWindow;

			d1(kprintf("%s/%s/%ld: WinUnderPtr=%08lx  swit=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WinUnderPtr, swit));

			x = msg->MouseX + iwt->iwt_WindowTask.wt_Window->LeftEdge - swit->iwt_WindowTask.wt_Window->LeftEdge - swit->iwt_InnerLeft + swit->iwt_WindowTask.wt_XOffset;
			y = msg->MouseY + iwt->iwt_WindowTask.wt_Window->TopEdge - swit->iwt_WindowTask.wt_Window->TopEdge - swit->iwt_InnerTop + swit->iwt_WindowTask.wt_YOffset;

			if (swit != iwt->iwt_WinUnderPtr)
				{
				// pointer entered different window

				DestChange = TRUE;

				d1(kprintf("%s/%s/%ld: New Scalos Window  swit=%08lx\n", __FILE__, __FUNC__, __LINE__, swit));

				if (iwt->iwt_WinUnderPtr)
					{
					if (iwt->iwt_IconUnderPtr)
						{
						// signal pointer leaving old icon
						DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
							SCCM_IconWin_DragLeave,
							iwt->iwt_WindowTask.mt_WindowStruct,
							iwt->iwt_IconUnderPtr,
							x, y);

						iwt->iwt_IconUnderPtr = NULL;
						}

					// signal pointer leaving old window
					DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_DragLeave,
						iwt->iwt_WindowTask.mt_WindowStruct,
						NULL,
						x, y);

					iwt->iwt_WinUnderPtr = NULL;
					iwt->iwt_IconOuterUBUnderPtr = NULL;
					}

				// query window if icons may be dropped on it
				iwt->iwt_DragMayDrop = DoMethod(swit->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_DragQuery,
					iwt->iwt_WindowTask.mt_WindowStruct,
					NULL,
					x, y);

				// signal pointer entering new window
				DoMethod(swit->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_DragEnter,
					iwt->iwt_WindowTask.mt_WindowStruct,
					NULL,
					x, y);

				iwt->iwt_WinUnderPtr = swit;
				iwt->iwt_IconOuterUBUnderPtr = NULL;
				}
			else
				{
				// same window as before
				if (NULL == inOuterBounds && NULL == in)
					{
					WORD xw = msg->MouseX + iwt->iwt_WindowTask.wt_Window->LeftEdge - swit->iwt_WindowTask.wt_Window->LeftEdge;
					WORD yw = msg->MouseY + iwt->iwt_WindowTask.wt_Window->TopEdge - swit->iwt_WindowTask.wt_Window->TopEdge;

					// we are not over any icon
					CheckNeedScroll(swit, xw, yw);

					DoDragScroll(iwt, swit);
					}
				else
					{
					swit->iwt_DragScrollX = swit->iwt_DragScrollY = 0;
					}
				}

			d1(kprintf("%s/%s/%ld: IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, swit->iwt_WindowTask.wt_IconList));

			// now check icons

			// check if <in> is one of the icons being dragged
			for (dn = iwt->iwt_DragNodeList; in && dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
				{
				if (dn->drgn_icon == in->in_Icon)
					in = NULL;
				}

			d1(kprintf("%s/%s/%ld: in=%08lx  IconUnderPtr=%08lx\n", __FILE__, __FUNC__, __LINE__, \
				in, iwt->iwt_IconUnderPtr));
			d1(kprintf("%s/%s/%ld: inOuterBounds=%08lx  IconOuterUBUnderPtr=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, inOuterBounds, iwt->iwt_IconOuterUBUnderPtr));

			if (!CurrentPrefs.pref_DDIconsMayOverlap && inOuterBounds != iwt->iwt_IconOuterUBUnderPtr)
				{
				d1(kprintf("%s/%s/%ld: WinUnderPtr=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WinUnderPtr));

				d1(kprintf("%s/%s/%ld: inOuterBounds=%08lx <%s>  iwt_IconOuterUBUnderPtr=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
					inOuterBounds, inOuterBounds ? inOuterBounds->in_Name : (STRPTR) "", \
					iwt->iwt_IconOuterUBUnderPtr, \
					iwt->iwt_IconOuterUBUnderPtr ? iwt->iwt_IconOuterUBUnderPtr->in_Name : (STRPTR) ""));


				iwt->iwt_DragMayDrop = (NULL == inOuterBounds);
				IconOverlap = !iwt->iwt_DragMayDrop;

				d1(kprintf("%s/%s/%ld: DragMayDrop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragMayDrop));

				iwt->iwt_IconOuterUBUnderPtr = inOuterBounds;
				}

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			if (in != iwt->iwt_IconUnderPtr)
				{
				d1(kprintf("%s/%s/%ld: WinUnderPtr=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WinUnderPtr));

				DestChange = TRUE;

				d1(kprintf("%s/%s/%ld: in=%08lx <%s>  IconUnderPtr=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
					in, in ? in->in_Name : (STRPTR) "", \
					iwt->iwt_IconUnderPtr, \
					iwt->iwt_IconUnderPtr ? iwt->iwt_IconUnderPtr->in_Name : (STRPTR) ""));

				// query icon or window if icons may be dropped on it
				iwt->iwt_DragMayDrop = !IconOverlap && DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_DragQuery,
					iwt->iwt_WindowTask.mt_WindowStruct,
					in,
					x, y);

				if (iwt->iwt_IconUnderPtr)
					{
					DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_DragLeave,
						iwt->iwt_WindowTask.mt_WindowStruct,
						iwt->iwt_IconUnderPtr,
						x, y);
					}

				d1(kprintf("%s/%s/%ld: DragMayDrop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragMayDrop));

				if (in)
					{
					// enter new icon
					DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_DragEnter,
						iwt->iwt_WindowTask.mt_WindowStruct,
						in,
						x, y);

					iwt->iwt_ObjectUnderMouse = OUM_Icon;
					}

				iwt->iwt_IconUnderPtr = in;
				}

			if (inOrig)
				ScalosUnLockIconList(swit);

			SCA_UnLockWindowList();
			}
		else if (foreignWin)
			{
			// foreign (non-scalos) window found
			struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;

			iwt->iwt_ObjectUnderMouse = OUM_ForeignWindow;

			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			if (iwt->iwt_WinUnderPtr)
				{
				if (iwt->iwt_IconUnderPtr)
					{
					// signal pointer leaving icon
					DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_DragLeave,
						iwt->iwt_WindowTask.mt_WindowStruct,
						iwt->iwt_IconUnderPtr,
						0, 0);
					iwt->iwt_IconUnderPtr = NULL;
					}

				// signal pointer leaving window
				DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_DragLeave,
					iwt->iwt_WindowTask.mt_WindowStruct,
					NULL,
					0, 0);

				iwt->iwt_WinUnderPtr = NULL;
				iwt->iwt_IconOuterUBUnderPtr = NULL;
				}

			if (ScalosAttemptSemaphoreShared(iwtx->iwt_AppListSemaphore))
				{
				struct AppObject *an;

				d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

				for (an=iwtx->iwt_AppList; an; an = (struct AppObject *) an->appo_Node.mln_Succ)
					{
					if (APPTYPE_AppWindow == an->appo_type &&
						an->appo_object.appoo_Window == foreignWin)
						{
						// Mouse is inside AppWindow
						iwt->iwt_DragMayDrop = TRUE;
						iwt->iwt_ObjectUnderMouse = OUM_AppWindow;

						d1(kprintf("%s/%s/%ld:  AppWindow found  AppObject=%08lx\n", __FILE__, __FUNC__, __LINE__, an));
						}
					}

				ScalosReleaseSemaphore(iwtx->iwt_AppListSemaphore);
				}
			}

		if (DestChange)
			{
			iwt->iwt_SameLock = GetSourceDestScaSameLock(iwt);
			}

		if (DRAGTRANSPMODE_TranspAndSolid == CurrentPrefs.pref_DragTranspMode
			|| DRAGTRANSPMODE_SolidAndTransp == CurrentPrefs.pref_DragTranspMode)
			{
			ULONG TranspFlags = 0;

			d1(kprintf("%s/%s/%ld:  ObjectUnderMouse=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_ObjectUnderMouse));

			if (iwt->iwt_IconUnderPtr)
				iwt->iwt_ObjectUnderMouse = OUM_Icon;
			else if (iwt->iwt_WinUnderPtr)
				iwt->iwt_ObjectUnderMouse = OUM_ScalosWindow;

			switch (iwt->iwt_ObjectUnderMouse)
				{
			case OUM_ScalosWindow:
				TranspFlags = DRAGTRANSPF_ScalosWindows;
				break;
			case OUM_AppWindow:
				TranspFlags = DRAGTRANSPF_AppWindows;
				break;
			case OUM_Icon:
				if (iwt->iwt_DragMayDrop && iwt->iwt_IconUnderPtr)
					{
					ULONG IconType;

					GetAttr(IDTA_Type, iwt->iwt_IconUnderPtr->in_Icon, &IconType);

					d1(KPrintF("%s/%s/%ld:  <%s>  IconType=%08lx\n", __FILE__, __FUNC__, __LINE__, GetIconName(iwt->iwt_IconUnderPtr), IconType));
					switch (IconType)
						{
					case WBDISK:
						TranspFlags = DRAGTRANSPF_DiskIcons;
						break;
					case WBDRAWER:
					case WB_TEXTICON_DRAWER:
						TranspFlags = DRAGTRANSPF_DrawerIcons;
						break;
					case WBTOOL:
					case WB_TEXTICON_TOOL:
						TranspFlags = DRAGTRANSPF_ToolIcons;
						break;
					case WBPROJECT:
						TranspFlags = DRAGTRANSPF_ProjectIcons;
						break;
					case WBGARBAGE:
						TranspFlags = DRAGTRANSPF_TrashcanIcons;
						break;
					case WBKICK:
						TranspFlags = DRAGTRANSPF_KickIcons;
						break;
					case WBAPPICON:
						if (IsIconifiedScalosWindow(iwt, iwt->iwt_IconUnderPtr))
							TranspFlags = DRAGTRANSPF_IconifiedWinIcons;
						else
							TranspFlags = DRAGTRANSPF_AppIcons;
						break;
					default:
						break;
						}
					d1(KPrintF("%s/%s/%ld:  pref_DragTranspObjects=%08lx  TranspFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, CurrentPrefs.pref_DragTranspObjects, TranspFlags));
					}
				break;
			default:
				break;
				}

			if (((DRAGTRANSPMODE_SolidAndTransp == CurrentPrefs.pref_DragTranspMode)
				&& (CurrentPrefs.pref_DragTranspObjects & TranspFlags)) ||
				((DRAGTRANSPMODE_TranspAndSolid == CurrentPrefs.pref_DragTranspMode)
				&& !(CurrentPrefs.pref_DragTranspObjects & TranspFlags)) )
				DrawDragFlags |= SCAF_Drag_Transparent;
			}

		d1(kprintf("%s/%s/%ld:  DrawDragFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, DrawDragFlags));

		if (!iwt->iwt_DragMayDrop)
			DrawDragFlags |= SCAF_Drag_NoDropHere;
		else
			DrawDragFlags |= DragMayCopy(iwt, msg->Qualifier);
		}

	d1(kprintf("%s/%s/%ld:  DrawDragFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, DrawDragFlags));

	DrawDrag(DrawDragFlags, iwt);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


void IconWin_EndDrag(struct internalScaWindowTask *iwt)
{
	d1(kprintf("%s/%s/%ld: iwt=%08lx  DragFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_DragFlags));
	d1(kprintf("%s/%s/%ld: WinUnderPtr=%08lx  DragFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_WinUnderPtr, iwt->iwt_WinUnderPtr ? iwt->iwt_WinUnderPtr->iwt_DragFlags : 0l));

	if (iwt->iwt_WinUnderPtr)
		{
		if (iwt->iwt_IconUnderPtr)
			{
			// signal pointer leaving old icon
			DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_DragLeave,
				iwt->iwt_WindowTask.mt_WindowStruct,
				iwt->iwt_IconUnderPtr,
				0, 0);

			iwt->iwt_IconUnderPtr = NULL;
			}

		// signal pointer leaving old window
		DoMethod(iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_DragLeave,
			iwt->iwt_WindowTask.mt_WindowStruct,
			NULL,
			0, 0);

		iwt->iwt_WinUnderPtr = NULL;
		}

	iwt->iwt_IconOuterUBUnderPtr = NULL;
	iwt->iwt_ObjectUnderMouse = OUM_nothing;

	// erase icon shadows
	if (iwt->iwt_IconShadowVisible)
		{
		RemoveIcons(iwt, &iwt->iwt_DragIconList);
		iwt->iwt_IconShadowVisible = FALSE;
		}
}


static LONG GetSourceDestScaSameLock(struct internalScaWindowTask *iwt)
{
	LONG Result;
	BPTR srcLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;
	BPTR destLock = (BPTR)NULL;
	BPTR IconLock = (BPTR)NULL;

	if (iwt->iwt_DragNodeList->drgn_iconnode->in_Lock)
		srcLock = iwt->iwt_DragNodeList->drgn_iconnode->in_Lock;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  ObjUnderMouse=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_ObjectUnderMouse));

	switch (iwt->iwt_ObjectUnderMouse)
		{
	case OUM_ScalosWindow:
		destLock = iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_WindowStruct->ws_Lock;
		d1(kprintf("%s/%s/%ld: ScalosWindow Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, destLock));
		break;

	case OUM_Icon:
		d1(kprintf("%s/%s/%ld: Icon=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconUnderPtr));

		if (iwt->iwt_IconUnderPtr)
			{
			ULONG IconType;

			GetAttr(IDTA_Type, iwt->iwt_IconUnderPtr->in_Icon, &IconType);

			d1(kprintf("%s/%s/%ld: <%s>  Lock=%08lx  Type=%ld\n", __FILE__, __FUNC__, __LINE__, \
				iwt->iwt_IconUnderPtr->in_Name, iwt->iwt_IconUnderPtr->in_Lock, IconType));

			switch (IconType)
				{
			case WBDISK:
			case WBDRAWER:
			case WB_TEXTICON_DRAWER:
			case WBGARBAGE:
				destLock = IconLock = GetIconLock(iwt->iwt_WinUnderPtr, iwt->iwt_IconUnderPtr);
				break;
			default:
				break;
				}
			}
		break;
	default:
		break;
		}

	if (destLock)
		Result = ScaSameLock(destLock, srcLock);
	else
		Result = LOCK_SAME;

	if (IconLock)
		UnLock(IconLock);

	return Result;
}


// get a Lock to an icon
static BPTR GetIconLock(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	BPTR iLock;
	ULONG IconType;

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	if (WBDISK == IconType && in->in_DeviceIcon)
		{
		// this is a Device, i.e. RAM:, DF0:
		iLock = Lock(in->in_DeviceIcon->di_Volume, ACCESS_READ);
		}
	else if (in->in_Lock)
		iLock = DupLock(in->in_Lock);
	else
		{
		BPTR oldDir = CurrentDir(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
		iLock = Lock(in->in_Name, ACCESS_READ);
		CurrentDir(oldDir);
		}

	return iLock;
}


// Mouse moved, MMB pressed
void IDCMPMMBMouseMove(struct internalScaWindowTask *iwt)
{
	WORD oldXOffset, oldYOffset;
	LONG dx, dy;
	ULONG Flags = 0;
	LONG x = iwt->iwt_WindowTask.wt_Window->MouseX;
	LONG y = iwt->iwt_WindowTask.wt_Window->MouseY;

	if (x == iwt->iwt_OldMouse.X && y == iwt->iwt_OldMouse.Y)
		return;

	dx = iwt->iwt_OldMouse.X - x;
	dy = iwt->iwt_OldMouse.Y - y;

	iwt->iwt_OldMouse.X = x;
	iwt->iwt_OldMouse.Y = y;

	if (2 == CurrentPrefs.pref_MMBMoveFlag)
		{
		dx = -dx;
		dy = -dy;
		}

	oldXOffset = iwt->iwt_WindowTask.wt_XOffset;
	oldYOffset = iwt->iwt_WindowTask.wt_YOffset;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, dx, dy);

	if (oldXOffset != iwt->iwt_WindowTask.wt_XOffset)
		Flags |= SETVIRTF_AdjustBottomSlider;
	if (oldYOffset != iwt->iwt_WindowTask.wt_YOffset)
		Flags |= SETVIRTF_AdjustRightSlider;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, Flags);
}


static ULONG DragMayCopy(struct internalScaWindowTask *iwt, ULONG Qualifier)
{
	struct DragNode *dn;
	ULONG DrawDragFlags = 0l;
	ULONG IconType;
	BOOL isBackDropIcon = FALSE;

	DrawDragFlags &= ~(SCAF_Drag_IndicateCopy | SCAF_Drag_IndicateMakeLink | SCAF_Drag_ForceMove);

	switch (iwt->iwt_ObjectUnderMouse)
		{
	case OUM_ScalosWindow:
		for (dn = iwt->iwt_DragNodeList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
			{
			if (dn->drgn_iconnode->in_Lock)
				isBackDropIcon = TRUE;

			if (dn->drgn_iconnode->in_DeviceIcon &&
				iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_WindowStruct->ws_WindowType != WSV_Type_DeviceWindow)
				{
				// dragging device icon to non-main window is allowed - always copy
				if (isMakeLinkQualifier(Qualifier))
					{
					DrawDragFlags |= SCAF_Drag_IndicateMakeLink;
					}
				else
					{
					DrawDragFlags |= SCAF_Drag_IndicateCopy;
					}
				}
			}

		if ((isBackDropIcon || iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType != WSV_Type_DeviceWindow) &&	// no copy from main window
			iwt->iwt_WinUnderPtr->iwt_WindowTask.mt_WindowStruct->ws_WindowType != WSV_Type_DeviceWindow)	// no copy to main window
			{
			if (isMakeLinkQualifier(Qualifier))
				{
				DrawDragFlags |= SCAF_Drag_IndicateMakeLink;
				}
			else if (isMoveQualifier(Qualifier))
				{
				DrawDragFlags |= SCAF_Drag_ForceMove;
				}
			else if (isCopyQualifier(Qualifier))
				{
				DrawDragFlags |= SCAF_Drag_IndicateCopy;
				}
			else if (LOCK_DIFFERENT == iwt->iwt_SameLock)
				{
				// check if Src and Dest are on same volume
				// if different volume then Copy
				DrawDragFlags |= SCAF_Drag_IndicateCopy;
				}
			}
		break;

	case OUM_Icon:
		GetAttr(IDTA_Type, iwt->iwt_IconUnderPtr->in_Icon, &IconType);

		// only copy to drawer, device, disk icons
		switch (IconType)
			{
		case WBDRAWER:
		case WBDISK:
		case WBDEVICE:
		case WB_TEXTICON_DRAWER:
			if (isMakeLinkQualifier(Qualifier))
				{
				DrawDragFlags |= SCAF_Drag_IndicateMakeLink;
				}
			else if (isMoveQualifier(Qualifier))
				{
				DrawDragFlags |= SCAF_Drag_ForceMove;
				}
			else if (isCopyQualifier(Qualifier))
				{
				DrawDragFlags |= SCAF_Drag_IndicateCopy;
				}
			else if (LOCK_DIFFERENT == iwt->iwt_SameLock)
				{
				// check if Src and Dest are on same volume
				// if different volume then Copy
				DrawDragFlags |= SCAF_Drag_IndicateCopy;
				}
			break;
		default:
			break;
			}
		break;
	default:
		break;
		}

	return DrawDragFlags;
}


static BOOL IsIconifiedScalosWindow(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct AppObject *appo;
	BOOL IsIconified = FALSE;

	ScalosObtainSemaphoreShared(iwt->iwt_AppListSemaphore);

	for (appo=iwt->iwt_AppList; appo; appo = (struct AppObject *) appo->appo_Node.mln_Succ)
		{
		if (APPTYPE_AppIcon == appo->appo_type
			&& in->in_Icon == appo->appo_object.appoo_IconObject
			&& MAKE_ID('I','C','F','Y') == appo->appo_id)
			{
			IsIconified = TRUE;
			break;
			}
		}

	ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);

	return IsIconified;
}


static void CheckNeedScroll(struct internalScaWindowTask *iwt, WORD xw, WORD yw)
{
	if (xw >= iwt->iwt_InnerLeft && xw < (iwt->iwt_InnerLeft + iwt->iwt_InnerWidth))
		{
		// xw is inside inner window area

		if (yw < iwt->iwt_InnerTop)
			{
			// Scroll down
			d1(KPrintF("%s/%s/%ld: iwt=%08lx  xw=%ld  yw=%ld  scroll down\n", __FILE__, __FUNC__, __LINE__, iwt, xw, yw));
			iwt->iwt_DragScrollY = yw - iwt->iwt_InnerTop;
			}
		else if (yw >= (iwt->iwt_InnerTop + iwt->iwt_InnerHeight))
			{
			// Scroll up
			d1(KPrintF("%s/%s/%ld: iwt=%08lx  xw=%ld  yw=%ld  scroll up\n", __FILE__, __FUNC__, __LINE__, iwt, xw, yw));
			iwt->iwt_DragScrollY = yw - (iwt->iwt_InnerTop + iwt->iwt_InnerHeight);
			}
		else
			{
			iwt->iwt_DragScrollY = 0;
			}
		}
	if (yw >= iwt->iwt_InnerTop && yw < (iwt->iwt_InnerTop + iwt->iwt_InnerHeight))
		{
		// yw is inside inner window area

		if (xw < iwt->iwt_InnerLeft)
			{
			// Scroll right
			d1(KPrintF("%s/%s/%ld: iwt=%08lx  xw=%ld  yw=%ld  scroll right\n", __FILE__, __FUNC__, __LINE__, iwt, xw, yw));
			iwt->iwt_DragScrollX = xw - iwt->iwt_InnerLeft;
			}
		else if (xw >= (iwt->iwt_InnerLeft + iwt->iwt_InnerWidth))
			{
			// Scroll left
			d1(KPrintF("%s/%s/%ld: iwt=%08lx  xw=%ld  yw=%ld  scroll left\n", __FILE__, __FUNC__, __LINE__, iwt, xw, yw));
			iwt->iwt_DragScrollX = xw - (iwt->iwt_InnerLeft + iwt->iwt_InnerWidth);
			}
		else
			{
			iwt->iwt_DragScrollX = 0;
			}
		}
}


static void DoDragScroll(struct internalScaWindowTask *iwtSrc, struct internalScaWindowTask *iwtDest)
{
	LONG DeltaX = 0, DeltaY = 0;

	d1(KPrintF("%s/%s/%ld: iwtDest=%08lx  iwtDest_DragScrollX=%ld  iwtDest_DragScrollY=%ld\n", \
		__FILE__, __FUNC__, __LINE__, iwtDest, iwtDest->iwt_DragScrollX, iwtDest->iwt_DragScrollY));

	if (iwtDest->iwt_DragScrollY < 0)
		{
		d1(KPrintF("%s/%s/%ld: iwt_MinY=%ld  iwt_InvisibleHeight=%ld  iwt_InnerHeight=%ld  wt_YOffset=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iwtDest->iwt_MinY, iwtDest->iwt_InvisibleHeight, \
                        iwtDest->iwt_InnerHeight, iwtDest->iwt_WindowTask.wt_YOffset));

		if ((iwtDest->iwt_MinY + 1) < iwtDest->iwt_WindowTask.wt_YOffset)
			{
			DeltaY = iwtDest->iwt_DragScrollY;
			}
		}
	else if (iwtDest->iwt_DragScrollY > 0)
		{
		d1(KPrintF("%s/%s/%ld: iwt_MinY=%ld  iwt_InvisibleHeight=%ld  iwt_InnerHeight=%ld  wt_YOffset=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iwtDest->iwt_MinY, iwtDest->iwt_InvisibleHeight, \
                        iwtDest->iwt_InnerHeight, iwtDest->iwt_WindowTask.wt_YOffset));

		if ((iwtDest->iwt_MinY + iwtDest->iwt_InvisibleHeight) > iwtDest->iwt_WindowTask.wt_YOffset)
			{
			DeltaY = iwtDest->iwt_DragScrollY;
			}
		}

	if (iwtDest->iwt_DragScrollX < 0)
		{
		if ((iwtDest->iwt_MinX + 1) < iwtDest->iwt_WindowTask.wt_XOffset)
			{
			DeltaX = iwtDest->iwt_DragScrollX;
			}
		}
	else if (iwtDest->iwt_DragScrollX > 0)
		{
		if ((iwtDest->iwt_MinX + iwtDest->iwt_InvisibleWidth) > iwtDest->iwt_WindowTask.wt_XOffset)
			{
			DeltaX = iwtDest->iwt_DragScrollX;
			}
		}

	if (DeltaX || DeltaY)
		{
		UWORD DeltaMoveFlags = 0;
		BOOL DropMark = (iwtDest->iwt_DragFlags & DRAGFLAGF_WindowMarked);
		BOOL WindowLocked = iwtDest->iwt_DragFlags & DRAGFLAGF_WindowLocked;
		struct DragHandle *dh = iwtSrc->iwt_myDragHandle;
		ULONG WasLocked = FALSE;

		if (dh)
			{
			ClassHideDragBobs(iwtDest, dh);
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			WasLocked = SCA_UnlockDrag(dh);
			}

		// clear lock flag to allow icons to be updated
		iwtDest->iwt_DragFlags &= ~DRAGFLAGF_WindowLocked;

		if (DropMark)
			EraseWindowDropMark(iwtDest);

		if (DeltaY)
			DeltaMoveFlags |= SETVIRTF_AdjustRightSlider;
		if (DeltaX)
			DeltaMoveFlags |= SETVIRTF_AdjustBottomSlider;

		DoMethod(iwtDest->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_DeltaMove, DeltaX, DeltaY);
		DoMethod(iwtDest->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_SetVirtSize, DeltaMoveFlags);

		if (DropMark)
			DisplayWindowDropMark(iwtDest);

		if (WindowLocked)
			iwtDest->iwt_DragFlags |= DRAGFLAGF_WindowLocked;

		ReLockDrag(dh, iwtSrc, WasLocked);
		}
}


void IDCMPDragIntuiTicks(struct internalScaWindowTask *iwt, struct IntuiMessage *msg)
{
	struct internalScaWindowTask *iwtUnderPointer;
	struct ScaIconNode *in;
	struct ScaIconNode *inOuterBounds;
	struct Window *foreignWin;
	struct DragHandle *dh;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	QueryObjectUnderPointer(&iwtUnderPointer, &in, &inOuterBounds, &foreignWin);

	d1(KPrintF("%s/%s/%ld: iwtUnderPointer=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtUnderPointer));

	if (iwtUnderPointer)
		{
		// Pointer is inside a Scalos Window
		DoDragScroll(iwt, iwtUnderPointer);

		if (in)
			ScalosUnLockIconList(iwtUnderPointer);
		SCA_UnLockWindowList();
		}

	dh = iInfos.xii_GlobalDragHandle;

	if (dh && dh->drgh_PopOpenTickCount && dh->drgh_PopOpenDestWindow && dh->drgh_PopOpenIcon)
		{
		d1(KPrintF("%s/%s/%ld: drgh_PopOpenTickCount=%lu\n", __FILE__, __FUNC__, __LINE__, dh->drgh_PopOpenTickCount));

		if (0 == --dh->drgh_PopOpenTickCount)
			{
			d1(KPrintF("%s/%s/%ld:  Task=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__,  FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));
			PopOpenWindow(iwt, dh);
			}
		}
}


