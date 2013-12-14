// WindowClass.c
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
#if defined(__MORPHOS__)
#include <intuition/extensions.h>
#else
#include <images/titlebar.h>
#endif /* __MORPHOS__ */
#include <libraries/pm.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/titlebarimage.h>
#include "debug.h"
#include <proto/scalos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

#define	NO_ICON_POSITION_WORD	((WORD) 0x8000)

#define	THEME_ICONIFIED_PATH	"THEME:window"
#define	ICONIFIED_ICON_NAME	"def_iconify"

#if defined(__MORPHOS__)

// Set this undocumented tag with non-zero argument to allow dynamic window resize on Morphos 2.x
// When set, we will receive lots of IDCMP_NEWSIZE messages during window resize.
#define	WA_DynamicResize1	(WA_Dummy + 150)

// Set this undocumented tag with non-zero argument to allow dynamic window resize on
// Morphos 2.3 with Enhanced Display Engine.
// Then we will receive IDCMP_CHANGEWINDOW messages with Code=2 or Code=3
// and need to react by actively calling ChangeWindowBox()
#define	WA_DynamicResize2	(WA_Dummy + 147)
#endif /* __MORPHOS__ */

struct WindowClassInstance
	{
	ULONG wci_Transparency;
	ULONG wci_Dummy;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) WindowClass_Dispatcher(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_Iconify(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_UnIconify(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_RemClipRegion(Class *cl, Object *o, Msg msg);
static struct Region * WindowClass_InitClipRegion(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_SetTitle(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_LockUpdate(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_UnlockUpdate(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_SetInnerSize(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_Close(Class *cl, Object *o, Msg msg);
static void FreeWindowGadgets(struct internalScaWindowTask *iwt);
static ULONG WindowClass_Ping(Class *cl, Object *o, Msg msg);
static struct Window *CheckWindowPing(Class *cl, Object *o, Msg msg); // +jmc+
static struct Window *WindowClass_Open(Class *cl, Object *o, Msg msg);
static struct Window *ScaOpenWindow(struct internalScaWindowTask *iwt, Class *cl, Object *o);
static struct Gadget *InitWindowGadgets(struct internalScaWindowTask *iwt);
static struct Image *NewArrow(struct internalScaWindowTask *iwt, ULONG ImageType);
static ULONG WindowClass_ChangeWindow(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_DynamicResizeWindow(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_Set(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_Get(Class *cl, Object *o, Msg msg);
static ULONG WindowClass_SetTransparency(Class *cl, Object *o, ULONG NewTransparency);
static void WindowClass_ReCleanup(struct internalScaWindowTask *iwt, WORD OldInnerWidth, WORD OldInnerHeight);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------


struct ScalosClass *initWindowClass(const struct PluginClass *plug)
{
	struct ScalosClass *WindowClass;

	WindowClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct WindowClassInstance),
			NULL);

	if (WindowClass && WindowClass->sccl_class)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(WindowClass->sccl_class->cl_Dispatcher, WindowClass_Dispatcher);
		}

	return WindowClass;
}


static SAVEDS(ULONG) WindowClass_Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case SCCM_Window_Open:
		Result = (ULONG) WindowClass_Open(cl, o, msg);
		break;

	case SCCM_Window_Close:
		Result = WindowClass_Close(cl, o, msg);
		break;

	case SCCM_Window_Iconify:
		Result = WindowClass_Iconify(cl, o, msg);
		break;

	case SCCM_Window_UnIconify:
		Result = WindowClass_UnIconify(cl, o, msg);
		break;

	case SCCM_Window_InitClipRegion:
		Result = (ULONG) WindowClass_InitClipRegion(cl, o, msg);
		break;

	case SCCM_Window_RemClipRegion:
		Result = WindowClass_RemClipRegion(cl, o, msg);
		break;

	case SCCM_Window_SetTitle:
		Result = WindowClass_SetTitle(cl, o, msg);
		break;

	case SCCM_Ping:
		Result = WindowClass_Ping(cl, o, msg);
		break;

	case SCCM_Window_SetInnerSize:
		Result = WindowClass_SetInnerSize(cl, o, msg);
		break;

	case SCCM_Window_LockUpdate:
		Result = WindowClass_LockUpdate(cl, o, msg);
		break;

	case SCCM_Window_UnlockUpdate:
		Result = WindowClass_UnlockUpdate(cl, o, msg);
		break;

	case SCCM_Window_ChangeWindow:
		Result = WindowClass_ChangeWindow(cl, o, msg);
		break;

	case SCCM_Window_DynamicResize:
		Result = WindowClass_DynamicResizeWindow(cl, o, msg);
		break;

	case SCCM_Window_NewPath:
		// No-op
		Result = 0;
		break;

	case SCCM_Window_WBStartupFinished:
		// currently No-Op
		Result = 0;
		break;

	case OM_SET:
		Result = WindowClass_Set(cl, o, msg);
		break;

	case OM_GET:
		Result = WindowClass_Get(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}


static ULONG WindowClass_Iconify(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	Object *WindowIconObj;
	Object *iconifiedIconObj, *allocIconObj = NULL;
	struct ExtGadget *gg;
	STRPTR IconPath;
	BOOL Result = FALSE;

	do	{
		IconPath = AllocPathBuffer();
		if (NULL == IconPath)
			break;

		if (iwt->iwt_IconifyFlag)
			{
			// window is already iconified
			Result = TRUE;
			break;
			}

		ClearSelectionProg(iwt, NULL);	// unselect all icons

		iwt->iwt_IconifyFlag = TRUE;

		stccpy(IconPath, THEME_ICONIFIED_PATH, Max_PathLen - 1);
		AddPart(IconPath, ICONIFIED_ICON_NAME, Max_PathLen - 1);

		iconifiedIconObj = NewIconObjectTags(IconPath,
			IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
			IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
			TAG_END);

		if (NULL == iconifiedIconObj)
			{
			stccpy(IconPath, CurrentPrefs.pref_DefIconPath, Max_PathLen - 1);
			AddPart(IconPath, ICONIFIED_ICON_NAME, Max_PathLen - 1);

			iconifiedIconObj = NewIconObjectTags(IconPath,
				IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
				IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
				TAG_END);
			}

		if (NULL == iconifiedIconObj)
			iconifiedIconObj = GetDefIconObjectTags(WBPROJECT,
				IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
				IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
				TAG_END);

		if (NULL == iconifiedIconObj)
			break;

		WindowIconObj = ClassGetWindowIconObject(iwt, &allocIconObj);

		gg = (struct ExtGadget *) iconifiedIconObj;

		iwt->iwt_myAppIcon = iconifiedIconObj;

		gg->TopEdge = gg->LeftEdge = NO_ICON_POSITION_WORD;

		d1(kprintf("%s/%s/%ld: LeftEdge=%ld  TopEdge=%ld\n", __FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge));

		if (WindowIconObj)
			{
			STRPTR tt;
			ULONG lVal;

			tt = NULL;
			if (DoMethod(WindowIconObj, IDTM_FindToolType, "SCALOS_ICONIFIED_XPOS", &tt))
				{
				lVal = NO_ICON_POSITION_WORD;
				DoMethod(WindowIconObj, IDTM_GetToolTypeValue, tt, &lVal);
				gg->LeftEdge = (WORD) lVal;
				}

			tt = NULL;
			if (DoMethod(WindowIconObj, IDTM_FindToolType, "SCALOS_ICONIFIED_YPOS", &tt))
				{
				lVal = NO_ICON_POSITION_WORD;
				DoMethod(WindowIconObj, IDTM_GetToolTypeValue, tt, &lVal);
				gg->TopEdge = (WORD) lVal;
				}
			}

		d1(kprintf("%s/%s/%ld: LeftEdge=%ld  TopEdge=%ld\n", __FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge));

		SetAttrs(iconifiedIconObj, 
			IDTA_Text, (ULONG) iwt->iwt_WindowTask.mt_WindowStruct->ws_Name,
			TAG_END);

		// SCA_NewAddAppIcon()
		iwt->iwt_myAppObj = SCA_NewAddAppIconTags(MAKE_ID('I','C','F','Y'), (ULONG) iwt->iwt_WindowTask.mt_WindowStruct,
			iconifiedIconObj, iwt->iwt_WindowTask.wt_IconPort,
			WBAPPICONA_SupportsOpen, TRUE,
			WBAPPICONA_SupportsSnapshot, NULL != WindowIconObj,
			WBAPPICONA_SupportsUnSnapshot, NULL != WindowIconObj,
			TAG_END);

		if (NULL == iwt->iwt_myAppObj)
			{
			DisposeIconObject(iconifiedIconObj);
			iwt->iwt_myAppIcon = NULL;

			break;
			}

		if (iwt->iwt_WindowTask.wt_Window)
			{
			iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_Iconify;

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Sleep);
			}

		Result = TRUE;
		} while (0);

	if (IconPath)
		FreePathBuffer(IconPath);
	if (allocIconObj)
		DisposeIconObject(allocIconObj);

	return Result;
}


static ULONG WindowClass_UnIconify(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(kprintf("%s/%s/%ld: IconList=%08lx  LateIconList=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_IconList, iwt->iwt_WindowTask.wt_LateIconList));

	if (iwt->iwt_myAppObj)
		{
		SCA_RemoveAppObject(iwt->iwt_myAppObj);
		iwt->iwt_myAppObj = NULL;
		}
	if (iwt->iwt_myAppIcon)
		{
		DisposeIconObject(iwt->iwt_myAppIcon);
		iwt->iwt_myAppIcon = NULL;
		}

	iwt->iwt_IconifyFlag = FALSE;
	iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_Iconify;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_WakeUp, 0);

	return TRUE;
}


static struct Region *WindowClass_InitClipRegion(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct Region *clipRegion;
	struct Region *origClipRegion;

	d1(KPrintF("%s/%s/%ld: wt_Window=%08lx  xOffset=%ld  yOffset=%ld\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_WindowTask.wt_Window, iwt->iwt_WindowTask.wt_XOffset, \
		iwt->iwt_WindowTask.wt_YOffset));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return NULL;

	clipRegion = NewRegion();
	if (NULL == clipRegion)		
		return NULL;

	if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
		{
		d1(kprintf("%s/%s/%ld: DRAGFLAGF_WindowLocked\n", __FILE__, __FUNC__, __LINE__));
		iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;
		}
	else
		{
		struct Rectangle WindowRect;

		WindowRect.MinX = WindowRect.MaxX = iwt->iwt_InnerLeft;
		WindowRect.MinY = WindowRect.MaxY = iwt->iwt_InnerTop;
		WindowRect.MaxX += iwt->iwt_InnerWidth - 1;
		WindowRect.MaxY += iwt->iwt_InnerHeight - 1;

		d1(KPrintF("%s/%s/%ld: iwt_InnerTop=%ld\n", \
			__FILE__, __FUNC__, __LINE__, iwt->iwt_InnerTop));
		d1(KPrintF("%s/%s/%ld: iwt_InnerBottom=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerBottom));

		d1(kprintf("%s/%s/%ld: MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, \
			WindowRect.MinX, WindowRect.MinY, WindowRect.MaxX, WindowRect.MaxY));

		OrRectRegion(clipRegion, &WindowRect);
		}

	origClipRegion = InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, clipRegion);

	d1(kprintf("%s/%s/%ld: origClipRegion=%08lx  clipRegion=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, origClipRegion, clipRegion));

	return origClipRegion;
}


static ULONG WindowClass_RemClipRegion(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_RemClipRegion *mrc = (struct msg_RemClipRegion *) msg;
	struct Region *oldClipRegion;

	d1(kprintf("%s/%s/%ld: mrc_oldClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, mrc->mrc_oldClipRegion));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return 0;

	oldClipRegion = InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, mrc->mrc_oldClipRegion);

	d1(kprintf("%s/%s/%ld: oldClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, oldClipRegion));
	if (oldClipRegion)
		DisposeRegion(oldClipRegion);

	return 0;
}


static ULONG WindowClass_SetTitle(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_SetTitle *mst = (struct msg_SetTitle *) msg;

	if (iwt->iwt_WindowTask.wt_Window)
		{
		d1(kprintf("%s/%s/%ld: LockedSetWindowTitles(%08lx) mst->mst_Title<%s>\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window, mst->mst_Title));
		LockedSetWindowTitles(iwt->iwt_WindowTask.wt_Window, mst->mst_Title, (STRPTR) ~0);
		}

	return 0;
}


static ULONG WindowClass_LockUpdate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	iwt->iwt_DragFlags |= DRAGFLAGF_WindowLocked;
	d1(kprintf("%s/%s/%ld: iwt_DragFlags [ |= DRAGFLAGF_WindowLocked(%08lx) ]\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragFlags));
	iwt->iwt_DragFlags &= ~DRAGFLAGF_UpdatePending;
	d1(kprintf("%s/%s/%ld: iwt_DragFlags [ &= ~DRAGFLAGF_UpdatePending(%08lx) ]\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragFlags));

	return 0;
}


static ULONG WindowClass_UnlockUpdate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	iwt->iwt_DragFlags &= ~DRAGFLAGF_WindowLocked;
	d1(kprintf("%s/%s/%ld: iwt_DragFlags [ &= ~DRAGFLAGF_WindowLocked(%08lx) ]\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragFlags));

	if (iwt->iwt_DragFlags & DRAGFLAGF_UpdatePending)
		{
		IDCMPRefreshWindow(iwt, NULL);
		d1(kprintf("%s/%s/%ld: IDCMPRefreshWindow(%08lx, NULL)\n", __FILE__, __FUNC__, __LINE__, iwt));
		}

	return 0;
}


static ULONG WindowClass_SetInnerSize(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(kprintf("%s/%s/%ld: START iwt=%08lx  <%s>  wt_Window=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iwt->iwt_WindowTask.wt_Window));

	if (ScalosAttemptSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore))
		{
		if (iwt->iwt_WindowTask.wt_Window)
			{
			struct Region *origClipRegion = iwt->iwt_RemRegion;

			iwt->iwt_InnerLeft   = iwt->iwt_WindowTask.wt_Window->BorderLeft;
			iwt->iwt_InnerTop    = iwt->iwt_WindowTask.wt_Window->BorderTop    + iwt->iwt_TextWindowGadgetHeight + iwt->iwt_GadgetBarHeightTop;
			iwt->iwt_InnerRight  = iwt->iwt_WindowTask.wt_Window->BorderRight;
			iwt->iwt_InnerBottom = iwt->iwt_WindowTask.wt_Window->BorderBottom + iwt->iwt_GadgetBarHeightBottom;

			iwt->iwt_InnerWidth = iwt->iwt_WindowTask.wt_Window->Width - iwt->iwt_InnerLeft - iwt->iwt_InnerRight;
			iwt->iwt_InnerHeight = iwt->iwt_WindowTask.wt_Window->Height - iwt->iwt_InnerTop - iwt->iwt_InnerBottom;

			d1(kprintf("%s/%s/%ld: iwt_InnerWidth=%ld  iwt_InnerHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerWidth, iwt->iwt_InnerHeight));
			d1(kprintf("%s/%s/%ld: GZZWidth=%ld  GZZHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window->GZZWidth, iwt->iwt_WindowTask.wt_Window->GZZHeight));
			d1(kprintf("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window->Width, iwt->iwt_WindowTask.wt_Window->Height));
			d1(kprintf("%s/%s/%ld: BorderLeft=%ld  BorderRight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window->BorderLeft, iwt->iwt_WindowTask.wt_Window->BorderRight));

			iwt->iwt_RemRegion = NULL;

			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_RemClipRegion, origClipRegion);
			}

		// create a new ClipRegion with the actual window size
		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
		iwt->iwt_RemRegion = (struct Region *) DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_InitClipRegion);

		d1(kprintf("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static ULONG WindowClass_Close(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  wt_Window=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iwt->iwt_WindowTask.wt_Window));

	if (iwt->iwt_WindowTask.wt_Window)
		{
		struct Window *win;

		iwt->iwt_DragFlags &= ~(DRAGFLAGF_WindowLocked | DRAGFLAGF_UpdatePending);
		d1(kprintf("%s/%s/%ld: &= ~(DRAGFLAGF_WindowLocked | DRAGFLAGF_UpdatePending)=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragFlags));

		Forbid();
		d1(kprintf("%s/%s/%ld: Forbid() \n", __FILE__, __FUNC__, __LINE__));

		ClearMsgPort(iwt->iwt_WindowTask.wt_Window->UserPort);
		d1(kprintf("%s/%s/%ld: ClearMsgPort(%08lx)\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window->UserPort));
		iwt->iwt_WindowTask.wt_Window->UserPort = NULL;
		d1(kprintf("%s/%s/%ld: ClearMsgPort(%08lx)\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window->UserPort));

		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, 0L);
		d1(kprintf("%s/%s/%ld: ModifyIDCMP(%08lx, 0L)\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

		Permit();

		d1(kprintf("%s/%s/%ld: Permit() \n", __FILE__, __FUNC__, __LINE__));

		win = iwt->iwt_WindowTask.wt_Window;

		d1(kprintf("%s/%s/%ld: win = %08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

		iwt->iwt_WindowTask.wt_Window = NULL;

		d1(kprintf("%s/%s/%ld: NULL = %08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

		if (iwt->iwt_WindowTask.mt_WindowStruct)
			iwt->iwt_WindowTask.mt_WindowStruct->ws_Window = NULL;

		if (win->MenuStrip)
			SafeClearMenuStrip(win);

		LockedCloseWindow(win);
		d1(kprintf("%s/%s/%ld: LockedCloseWindow(%08lx)\n", __FILE__, __FUNC__, __LINE__, win));

		FreeWindowGadgets(iwt);

		d1(kprintf("%s/%s/%ld: FreeWindowGadgets(%08lx)\n", __FILE__, __FUNC__, __LINE__, iwt));

		}

	d1(kprintf("%s/%s/%ld: WINDOW CLOSED\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static void FreeWindowGadgets(struct internalScaWindowTask *iwt)
{
	if (iwt->iwt_GadImageRightArrow)
		{
		DisposeObject((Object *) iwt->iwt_GadImageRightArrow);
		iwt->iwt_GadImageRightArrow = NULL;
		}
	if (iwt->iwt_GadImageLeftArrow)
		{
		DisposeObject((Object *) iwt->iwt_GadImageLeftArrow);
		iwt->iwt_GadImageLeftArrow = NULL;
		}
	if (iwt->iwt_GadImageDownArrow)
		{
		DisposeObject((Object *) iwt->iwt_GadImageDownArrow);
		iwt->iwt_GadImageDownArrow = NULL;
		}
	if (iwt->iwt_GadImageUpArrow)
		{
		DisposeObject((Object *) iwt->iwt_GadImageUpArrow);
		iwt->iwt_GadImageUpArrow = NULL;
		}

	if (iwt->iwt_PropBottom)
		{
		DisposeObject((Object *) iwt->iwt_PropBottom);
		iwt->iwt_PropBottom = NULL;
		}
	if (iwt->iwt_PropSide)
		{
		DisposeObject((Object *) iwt->iwt_PropSide);
		iwt->iwt_PropSide = NULL;
		}

	if (iwt->iwt_GadgetRightArrow)
		{
		DisposeObject((Object *) iwt->iwt_GadgetRightArrow);
		iwt->iwt_GadgetRightArrow = NULL;
		}
	if (iwt->iwt_GadgetLeftArrow)
		{
		DisposeObject((Object *) iwt->iwt_GadgetLeftArrow);
		iwt->iwt_GadgetLeftArrow = NULL;
		}
	if (iwt->iwt_GadgetDownArrow)
		{
		DisposeObject((Object *) iwt->iwt_GadgetDownArrow);
		iwt->iwt_GadgetDownArrow = NULL;
		}
	if (iwt->iwt_GadgetUpArrow)
		{
		DisposeObject((Object *) iwt->iwt_GadgetUpArrow);
		iwt->iwt_GadgetUpArrow = NULL;
		}

	iwt->iwt_Gadgets = NULL;

	if (iwt->iwt_IconifyGadget)
		{
		DisposeObject((Object *) iwt->iwt_IconifyGadget);
		iwt->iwt_IconifyGadget = NULL;
		}
	if (iwt->iwt_IconifyImage)
		{
		DisposeObject((Object *) iwt->iwt_IconifyImage);
		iwt->iwt_IconifyImage = NULL;
		}

}


static ULONG WindowClass_Ping(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	if (++iwt->iwt_tRefreshCount >= CurrentPrefs.pref_TitleRefreshSec)
		{
		iwt->iwt_tRefreshCount = 0;

		if (iInfos.xii_iinfos.ii_MainWindowStruct == iwt->iwt_WindowTask.mt_WindowStruct)
			{
			BOOL fNewTitle = FALSE;

			d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

			if (CurrentPrefs.pref_RefreshOnMemChg)
				{
				ULONG MemSize = AvailMem(MEMF_ANY);

				if (OldMemSize != MemSize)
					{
					fNewTitle = TRUE;
					OldMemSize = MemSize;
					}
				}
			else
				fNewTitle = TRUE;

			d1(KPrintF("%s/%s/%ld: iwt=%08lx  fNewTitle=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, fNewTitle));

			if (fNewTitle)
				{
				struct ScaWindowStruct *ws;

				SCA_LockWindowList(SCA_LockWindowList_Shared);

				ScreenTitleBuffer = (STRPTR) DoMethod(ScreenTitleObject, SCCM_Title_Generate);

				for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
					{
					if (ws->ws_WindowTask && ws->ws_WindowTask->wt_Window)
						{
						LockedSetWindowTitles(ws->ws_Window, (STRPTR) ~0, ScreenTitleBuffer);
						}
					}

				SCA_UnLockWindowList();
				}
			d1(KPrintF("%s/%s/%ld: SCREEN: fNewTitle=[%ld] iwt->iwt_tRefreshCount[%ld] pref_TitleRefreshSec[%ld]\n", \
				__FILE__, __FUNC__, __LINE__, fNewTitle, iwt->iwt_tRefreshCount, CurrentPrefs.pref_TitleRefreshSec));
			}
		}

	if (!iwt->iwt_BackDrop)
		{
		d1(kprintf("%s/%s/%ld: CheckWindowPing() CALL \n", __FILE__, __FUNC__, __LINE__));
		CheckWindowPing(cl, o, msg);
		}

	return 0;
}

// +jmc+
static struct Window *CheckWindowPing(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	// iwt->iwt_WinLockCount++;

	iwt->iwt_wRefreshCount++;

	if (iwt->iwt_wRefreshCount >= CurrentPrefs.pref_WinTitleRefreshSec)
		{
		BOOL fNewTitle = FALSE;
		iwt->iwt_wRefreshCount = 0;

		if (CurrentPrefs.pref_WinRefreshOnMemChg)
			{
			ULONG MemSize = AvailMem(MEMF_ANY);

			d1(KPrintF("%s/%s/%ld: pref_WinRefreshOnMemChg=[%ld] OldMemSize[%ld] MemSize[%ld] \n", __FILE__, __FUNC__, __LINE__, CurrentPrefs.pref_WinRefreshOnMemChg, OldMemSize, MemSize));

			if (OldMemSize != MemSize)
				{
				fNewTitle = TRUE;
				OldMemSize = MemSize;
				}
			}
		else
			fNewTitle = TRUE;

		if (fNewTitle)
			{
			iwt->iwt_WinTitle = (STRPTR) DoMethod(iwt->iwt_TitleObject, SCCM_Title_Generate);
			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetTitle, iwt->iwt_WinTitle);
			}
		d1(KPrintF("%s/%s/%ld:    WIN: fNewTitle=[%ld] iwt->iwt_wRefreshCount[%ld] pref_WinTitleRefreshSec[%ld]\n", __FILE__, __FUNC__, __LINE__, fNewTitle, iwt->iwt_wRefreshCount, CurrentPrefs.pref_WinTitleRefreshSec));
		}

	d1(kprintf("%s/%s/%ld: CheckWindowPing() END \n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

static struct Window *WindowClass_Open(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  wt_Window=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iwt->iwt_WindowTask.wt_Window));

	iwt->iwt_LastUnCleanupInnerWidth = iwt->iwt_LastUnCleanupInnerHeight = 0;

	iwt->iwt_WindowTask.wt_Window = ScaOpenWindow(iwt, cl, o);

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  wt_Window=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iwt->iwt_WindowTask.wt_Window));

	if (iwt->iwt_WindowTask.wt_Window)
		{
		iwt->iwt_WindowTask.wt_Window->UserPort = iwt->iwt_WindowTask.wt_IconPort;

		iwt->iwt_DragFlags &= ~(DRAGFLAGF_WindowLocked | DRAGFLAGF_UpdatePending);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ModifyIDCMP(iwt->iwt_WindowTask.wt_Window, iwt->iwt_IDCMPFlags);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetInnerSize);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}

	d1(kprintf("%s/%s/%ld: wt_Window=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

	return iwt->iwt_WindowTask.wt_Window;
}


static struct Window *ScaOpenWindow(struct internalScaWindowTask *iwt, Class *cl, Object *o)
{
	struct WindowClassInstance *inst = (struct WindowClassInstance *) INST_DATA(cl, o);
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	struct Window *win;
	ULONG WindowFlags;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	iwt->iwt_WindowTask.wt_XOffset = ws->ws_xoffset;
	iwt->iwt_WindowTask.wt_YOffset = ws->ws_yoffset;

	iwt->iwt_BackDrop = FALSE;

	if (ws->ws_Flags & WSV_FlagF_Backdrop)
		{
		iwt->iwt_BackDrop = TRUE;

		ws->ws_xoffset = 0;
		ws->ws_yoffset = 0;
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (iInfos.xii_iinfos.ii_MainWindowStruct == ws)
		{
		ws->ws_Left = CurrentPrefs.pref_WBWindowBox.Left;
		ws->ws_Top = CurrentPrefs.pref_WBWindowBox.Top;
		ws->ws_Width = CurrentPrefs.pref_WBWindowBox.Width;
		ws->ws_Height = CurrentPrefs.pref_WBWindowBox.Height;

		ws->ws_xoffset = CurrentPrefs.pref_WBWindowXOffset;
		ws->ws_yoffset = CurrentPrefs.pref_WBWindowYOffset;

		if (iwt->iwt_BackDrop)
			{
			ws->ws_xoffset = 0;
			ws->ws_yoffset = 0;

			ws->ws_Left = 0;         // +dm+ 20011106

			if (CurrentPrefs.pref_FullBenchFlag)
				ws->ws_Top = 0;
			else
				ws->ws_Top = iwt->iwt_WinScreen->BarHeight + 1;

			ws->ws_Height = iwt->iwt_WinScreen->Height - ws->ws_Top;
			ws->ws_Width = iwt->iwt_WinScreen->Width;
			}
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (!iwt->iwt_BackDrop)
		{
		struct Gadget *FirstGad = InitWindowGadgets(iwt);

		if (iwt->iwt_Gadgets)
			iwt->iwt_Gadgets->NextGadget = FirstGad;
		else
			iwt->iwt_Gadgets = FirstGad;
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (iwt->iwt_BackDrop)
		{
		WindowFlags = WFLG_HASZOOM  | WFLG_REPORTMOUSE |
			WFLG_BACKDROP | WFLG_BORDERLESS;
		}
	else
		{
		WindowFlags = WFLG_HASZOOM | WFLG_REPORTMOUSE |
			WFLG_SIZEBRIGHT | WFLG_SIZEBBOTTOM | 
			WFLG_SIZEGADGET | WFLG_DRAGBAR | 
			WFLG_DEPTHGADGET | WFLG_CLOSEGADGET;
		}

	if (((struct MainTask *) iwt)->emulation)
		WindowFlags |= WFLG_WBENCHWINDOW;

	if (CurrentPrefs.pref_SmartRefresh)
		WindowFlags |= WFLG_SMART_REFRESH;
	else
		WindowFlags |= WFLG_SIMPLE_REFRESH;

	d1(KPrintF("%s/%s/%ld: iwt_BackDrop=%ld  ws_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_BackDrop, ws->ws_Flags));
	d1(KPrintF("%s/%s/%ld: WA_Activate=%ld\n", __FILE__, __FUNC__, __LINE__, (ws->ws_Flags & WSV_FlagF_NoActivateWindow) ? 0 : 1));

	if (ws->ws_Flags & WSV_FlagF_NoActivateWindow)
		inst->wci_Transparency = ws->ws_WindowOpacityInactive;
	else
		inst->wci_Transparency = ws->ws_WindowOpacityActive;

	d1(KPrintF("%s/%s/%ld: WA_Width=%ld  WA_Height=%ld  wci_Transparency=%ld\n", __FILE__, __FUNC__, __LINE__, \
		ws->ws_Width + iwt->iwt_ExtraWidth, ws->ws_Height + iwt->iwt_ExtraHeight, inst->wci_Transparency));

	// OpenWindowTags()
	win = LockedOpenWindowTags(NULL,
		WA_ScreenTitle, ScreenTitleBuffer,
		WA_NewLookMenus, TRUE,
		WA_AutoAdjust, TRUE,
		WA_BackFill, iwt,
		WA_MaxHeight, ~0,
		WA_MaxWidth, ~0,
		WA_MinHeight, 65,
		WA_MinWidth, 92+20,
		WA_Activate, (ws->ws_Flags & WSV_FlagF_NoActivateWindow) ? 0 : 1,
		WA_CustomScreen, iwt->iwt_WinScreen,
		WA_IDCMP, 0L,
		WA_HelpGroup, HelpGroupID,
		WA_Flags, WindowFlags,
		iwt->iwt_BackDrop ? TAG_IGNORE : WA_Gadgets, iwt->iwt_Gadgets,
		iwt->iwt_BackDrop ? TAG_IGNORE : WA_Title, iwt->iwt_WinTitle,
		WA_ExtraGadget_Iconify, !iwt->iwt_BackDrop,
		WA_Width, ws->ws_Width + iwt->iwt_ExtraWidth,
		WA_Height, ws->ws_Height + iwt->iwt_ExtraHeight,
		WA_Top, ws->ws_Top,
		WA_Left, ws->ws_Left,
#if defined(__MORPHOS__)
		WA_DynamicResize1, 1,
		WA_DynamicResize2, 1,
#endif //defined(__MORPHOS__)
		WA_SCA_Opaqueness, SCALOS_OPAQUENESS(inst->wci_Transparency),
		TAG_END);

	d1(KPrintF("%s/%s/%ld: win=%08lx\n", __FILE__, __FUNC__, __LINE__, win));
	if (win)
		{
		WindowToFront(win);

#if !defined(__MORPHOS__) && !defined(__AROS__)
		if (!iwt->iwt_BackDrop && ws != iInfos.xii_iinfos.ii_AppWindowStruct)
			{
			// Add iconify Gadget
			struct Gadget *gad;
			struct Gadget *LeftMostGadget = NULL;
			WORD LeftMostPos = 0;

			for (gad=win->FirstGadget; gad; gad=gad->NextGadget)
				{
				if (0 == gad->TopEdge && (gad->Flags & GFLG_RELRIGHT) && gad->LeftEdge < 0)
					{
					d1(kprintf("%s/%s/%ld: LeftMostPos=%ld  gad->LeftEdge=%ld\n", \
						__FILE__, __FUNC__, __LINE__, LeftMostPos, gad->LeftEdge));

					if (LeftMostPos > gad->LeftEdge)
						{
						LeftMostPos = gad->LeftEdge;
						LeftMostGadget = gad;
						}
					}
				}

			if (LeftMostGadget)
				{
				// First, try to use "sysiclass" gadget
				iwt->iwt_IconifyImage = (struct Image *) NewObject(NULL, "sysiclass",
					SYSIA_DrawInfo, (ULONG) iwt->iwt_WinDrawInfo,
					SYSIA_Which, ICONIFYIMAGE,
					TAG_END);

				// Second, try to use "tbiclass" gadget
				if (NULL == iwt->iwt_IconifyImage)
					{
					iwt->iwt_IconifyImage = (struct Image *) NewObject(NULL, "tbiclass",
						SYSIA_DrawInfo, (ULONG) iwt->iwt_WinDrawInfo,
						SYSIA_Which, ICONIFYIMAGE,
						TAG_END);
					}

				d1(kprintf("%s/%s/%ld: iwt_IconifyImage=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconifyImage));

				if (NULL == iwt->iwt_IconifyImage)
					{
					// if titlebarimageclass" not available, use built-in image class.
					iwt->iwt_IconifyImage = (struct Image *) NewObject(IconifyImageClass, NULL,
						IA_Height, LeftMostGadget->Height,
						SYSIA_DrawInfo, (ULONG) iwt->iwt_WinDrawInfo,
						SYSIA_Which, ICONIFYIMAGE,
						TAG_END);
					}

				d1(kprintf("%s/%s/%ld: iwt_IconifyImage=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconifyImage));

				if (iwt->iwt_IconifyImage)
					{
					LONG ImageWidth;

					GetAttr(IA_Width, (Object *) iwt->iwt_IconifyImage, (ULONG *) &ImageWidth);

					d1(kprintf("%s/%s/%ld: gad->LeftEdge=%ld  Width=%ld\n", \
					__FILE__, __FUNC__, __LINE__, LeftMostGadget->LeftEdge - ImageWidth, ImageWidth));

					iwt->iwt_IconifyGadget = (struct Gadget *) NewObject(NULL, BUTTONGCLASS,
						GA_TopBorder, TRUE,
						GA_Top, 0,
//+++						GA_RelRight, TBI_RELPOS(iwt->iwt_IconifyImage,2),
						GA_RelRight, LeftMostGadget->LeftEdge - ImageWidth + 1,
						GA_RelVerify, TRUE,
						GA_GadgetHelp, TRUE,
						GA_TopBorder, TRUE,
						GA_ID, MAKE_ID(0,0,'F','Y'),
						GA_Image, (ULONG) iwt->iwt_IconifyImage,
						TAG_END);
					}
				if (iwt->iwt_IconifyGadget)
					{
					AddGadget(win, iwt->iwt_IconifyGadget, 0);
					RefreshWindowFrame(win);
					}
				else
					{
					if (iwt->iwt_IconifyImage)
						{
						DisposeObject((Object *) iwt->iwt_IconifyImage);
						iwt->iwt_IconifyImage = NULL;
						}
					}
				}
			}
#endif /* __MORPHOS__ */

		HelpControl(win, HC_GADGETHELP);	// Turn on gadget help

		if (win->Flags & WFLG_WINDOWACTIVE)
			{
			// If the new window is now active, set transparency for
			// all other Scalos windows to inactive value.
			if (SCA_LockWindowList(SCA_LockWindowList_AttemptShared))
				{
				struct ScaWindowStruct *ws2;

				d1(KPrintF("%s/%s/%ld: SCA_LockWindowList success!\n", __FILE__, __FUNC__, __LINE__));

				for (ws2=winlist.wl_WindowStruct; ws2; ws2 = (struct ScaWindowStruct *) ws2->ws_Node.mln_Succ)
					{
					if (ws2 != ws)
						{
						SetAttrs(ws2->ws_WindowTask->mt_WindowObject,
							SCCA_Window_Transparency, ws2->ws_WindowOpacityInactive,
							TAG_END);
						}
					}

				SCA_UnLockWindowList();
				}
			}
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return win;
}


static struct Gadget *InitWindowGadgets(struct internalScaWindowTask *iwt)
{
	struct Gadget *Result = NULL;
	WORD w, h, w2, h2, Top, Left;

	d1(kprintf("%s/%s/%ld: START iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

	do	{
		d1(kprintf("%s/%s/%ld: iwt_WinDrawInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WinDrawInfo));

		iwt->iwt_GadImageRightArrow = NewArrow(iwt, RIGHTIMAGE);
		d1(kprintf("%s/%s/%ld: iwt_GadImageRightArrow=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadImageRightArrow));
		if (NULL == iwt->iwt_GadImageRightArrow)
			break;

		iwt->iwt_GadImageLeftArrow = NewArrow(iwt, LEFTIMAGE);
		d1(kprintf("%s/%s/%ld: iwt_GadImageLeftArrow=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadImageLeftArrow));
		if (NULL == iwt->iwt_GadImageLeftArrow)
			break;

		iwt->iwt_GadImageDownArrow = NewArrow(iwt, DOWNIMAGE);
		d1(kprintf("%s/%s/%ld: iwt_GadImageDownArrow=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadImageDownArrow));
		if (NULL == iwt->iwt_GadImageDownArrow)
			break;

		iwt->iwt_GadImageUpArrow = NewArrow(iwt, UPIMAGE);
		d1(kprintf("%s/%s/%ld: iwt_GadImageUpArrow=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadImageUpArrow));
		if (NULL == iwt->iwt_GadImageUpArrow)
			break;

		w = iwt->iwt_GadImageDownArrow->Width;
		h = iwt->iwt_GadImageRightArrow->Height;
		w2 = w - 1;
		h2 = h - 1;

		w2 += iwt->iwt_GadImageRightArrow->Width;

		if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText])
			{
			// Update status bar font
			SetAttrs(iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText],
				GBTDTA_TextFont, (ULONG) iInfos.xii_iinfos.ii_Screen->RastPort.Font,
				TAG_END);
			}

		// Right Button
		iwt->iwt_GadgetRightArrow = (struct Gadget *) SCA_NewScalosObjectTags("ButtonGadget.sca",
			GA_Immediate, TRUE,
			GA_BottomBorder, TRUE,
			GA_ID, GADGETID_RIGHTBUTTON,
			GA_RelVerify, TRUE,
			GA_RelRight, -w2,
			GA_RelBottom, -(h - 1),
			GA_GadgetHelp, TRUE,
			GA_Image, (ULONG) iwt->iwt_GadImageRightArrow,
			TAG_END);
		d1(kprintf("%s/%s/%ld: iwt_GadgetRightArrow=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadgetRightArrow));
		if (NULL == iwt->iwt_GadgetRightArrow)
			break;

		w2 += iwt->iwt_GadImageLeftArrow->Width;

		// Left Button
		iwt->iwt_GadgetLeftArrow = (struct Gadget *) SCA_NewScalosObjectTags("ButtonGadget.sca",
			GA_Immediate, TRUE,
			GA_BottomBorder, TRUE,
			GA_ID, GADGETID_LEFTBUTTON,
			GA_RelVerify, TRUE,
			GA_RelRight, -w2,
			GA_RelBottom, -(h - 1),
			GA_GadgetHelp, TRUE,
			GA_Image, (ULONG) iwt->iwt_GadImageLeftArrow,
			GA_Next, (ULONG) iwt->iwt_GadgetRightArrow,
			TAG_END);
		d1(kprintf("%s/%s/%ld: iwt_GadgetLeftArrow=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadgetLeftArrow));
		if (NULL == iwt->iwt_GadgetLeftArrow)
			break;

		Left = iwt->iwt_WinScreen->WBorLeft - 1;

		// Bottom/horizontal slider
		iwt->iwt_PropBottom = (struct Gadget *) NewObject(NULL, PROPGCLASS,
			GA_RelWidth, -(w2 + Left + 3),
			GA_Left, Left,
			GA_RelBottom, (h >= 12) ? (-h + 4) : (-h + 3),
			GA_Height, (h >= 12) ? (h - 6) : (h - 4),
			GA_GadgetHelp, TRUE,
			GA_ID, GADGETID_HORIZSLIDER,
			GA_FollowMouse, TRUE,
			PGA_Visible, 1,
			PGA_Total, 1,
			PGA_Borderless, TRUE,
			PGA_NewLook, TRUE,
			PGA_Freedom, FREEHORIZ,
			GA_BottomBorder, TRUE,
			GA_RelVerify, TRUE,
			GA_Immediate, TRUE,
			GA_Next, (ULONG) iwt->iwt_GadgetLeftArrow,
			TAG_END);
		d1(kprintf("%s/%s/%ld: iwt_PropBottom=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_PropBottom));
		if (NULL == iwt->iwt_PropBottom)
			break;

		h2 += iwt->iwt_GadImageDownArrow->Height;

		// Down button
		iwt->iwt_GadgetDownArrow = (struct Gadget *) SCA_NewScalosObjectTags("ButtonGadget.sca",
			GA_Immediate, TRUE,
			GA_RightBorder, TRUE,
			GA_ID, GADGETID_DOWNBUTTON,
			GA_RelVerify, TRUE,
			GA_RelBottom, -h2,
			GA_RelRight, -(w - 1),
			GA_GadgetHelp, TRUE,
			GA_Image, (ULONG) iwt->iwt_GadImageDownArrow,
			GA_Next, (ULONG) iwt->iwt_PropBottom,
			TAG_END);
		d1(kprintf("%s/%s/%ld: iwt_GadgetDownArrow=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadgetDownArrow));
		if (NULL == iwt->iwt_GadgetDownArrow)
			break;

		d1(kprintf("%s/%s/%ld: Down Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_GadgetDownArrow->LeftEdge, iwt->iwt_GadgetDownArrow->TopEdge, iwt->iwt_GadgetDownArrow->Width, iwt->iwt_GadgetDownArrow->Height));

		h2 += iwt->iwt_GadImageUpArrow->Height;

		// Up button
		iwt->iwt_GadgetUpArrow = (struct Gadget *) SCA_NewScalosObjectTags("ButtonGadget.sca",
			GA_Immediate, TRUE,
			GA_RightBorder, TRUE,
			GA_ID, GADGETID_UPBUTTON,
			GA_RelVerify, TRUE,
			GA_RelBottom, -h2,
			GA_RelRight, -w + 1,
			GA_GadgetHelp, TRUE,
			GA_Image, (ULONG) iwt->iwt_GadImageUpArrow,
			GA_Next, (ULONG) iwt->iwt_GadgetDownArrow,
			TAG_END);
		d1(kprintf("%s/%s/%ld: iwt_GadgetUpArrow=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_GadgetUpArrow));
		if (NULL == iwt->iwt_GadgetUpArrow)
			break;

		d1(kprintf("%s/%s/%ld: Up Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_GadgetUpArrow->LeftEdge, iwt->iwt_GadgetUpArrow->TopEdge, iwt->iwt_GadgetUpArrow->Width, iwt->iwt_GadgetUpArrow->Height));

		Top = iwt->iwt_WinScreen->WBorTop + iwt->iwt_WinScreen->Font->ta_YSize + 2;

		// Right/vertical slider
		iwt->iwt_PropSide = (struct Gadget *) NewObject(NULL, PROPGCLASS,
			GA_Top, Top,
			GA_RelHeight, -(Top + h2 + 2),
			GA_Width, w - 8,
			GA_RelRight, -(w - 5),
			GA_GadgetHelp, TRUE,
			GA_ID, GADGETID_VERTSLIDER,
			GA_FollowMouse, TRUE,
			PGA_Visible, 1,
			PGA_Total, 1,
			PGA_Borderless, TRUE,
			PGA_NewLook, TRUE,
			PGA_Freedom, FREEVERT,
			GA_RightBorder, TRUE,
			GA_RelVerify, TRUE,
			GA_Immediate, TRUE,
			GA_Next, (ULONG) iwt->iwt_GadgetUpArrow,
			TAG_END);
		d1(kprintf("%s/%s/%ld: iwt_PropSide=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_PropSide));
		if (NULL == iwt->iwt_PropSide)
			break;

		d1(kprintf("%s/%s/%ld: PropSide Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_PropSide->LeftEdge, iwt->iwt_PropSide->TopEdge, iwt->iwt_PropSide->Width, iwt->iwt_PropSide->Height));

		Result = iwt->iwt_PropSide;

		d1(kprintf("%s/%s/%ld: StatusBar=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_StatusBar));
		if (iwt->iwt_StatusBar)
			{
			iwt->iwt_StatusBar->NextGadget = Result;
			Result = iwt->iwt_StatusBar;

			d1(kprintf("%s/%s/%ld: StatusBar BgPen=%ld\n", __FILE__, __FUNC__, __LINE__, PalettePrefs.pal_PensList[PENIDX_STATUSBAR_BG]));

			SetAttrs(iwt->iwt_StatusBar,
				GBDTA_BGPen, PalettePrefs.pal_PensList[PENIDX_STATUSBAR_BG],
				TAG_END);
			}
		d1(KPrintF("%s/%s/%ld: iwt_ControlBar=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_ControlBar));
		if (iwt->iwt_ControlBar)
			{
			iwt->iwt_ControlBar->NextGadget = Result;
			Result = iwt->iwt_ControlBar;

			d1(kprintf("%s/%s/%ld: StatusBar BgPen=%ld\n", __FILE__, __FUNC__, __LINE__, PalettePrefs.pal_PensList[PENIDX_STATUSBAR_BG]));

			SetAttrs(iwt->iwt_ControlBar,
				GBDTA_BGPen, PalettePrefs.pal_PensList[PENIDX_STATUSBAR_BG],
				TAG_END);
			}
		} while (0);

	if (NULL == Result)
		FreeWindowGadgets(iwt);

	d1(kprintf("%s/%s/%ld: Finished iwt=%08lx  Result=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, Result));

	return Result;
}


static struct Image *NewArrow(struct internalScaWindowTask *iwt, ULONG ImageType)
{
	return (struct Image *) NewObject(NULL, SYSICLASS,
		SYSIA_Which, ImageType,
		SYSIA_DrawInfo, (ULONG) iwt->iwt_WinDrawInfo,
		TAG_END);
}


static ULONG WindowClass_ChangeWindow(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	WORD newWidth, newHeight;
	WORD OldInnerHeight = iwt->iwt_InnerHeight;
	WORD OldInnerWidth = iwt->iwt_InnerWidth;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (ws->ws_Left != iwt->iwt_WindowTask.wt_Window->LeftEdge || ws->ws_Top != iwt->iwt_WindowTask.wt_Window->TopEdge)
		{
		ws->ws_Left = iwt->iwt_WindowTask.wt_Window->LeftEdge;
		ws->ws_Top = iwt->iwt_WindowTask.wt_Window->TopEdge;
		}

	newWidth = iwt->iwt_WindowTask.wt_Window->Width - iwt->iwt_ExtraWidth;
	newHeight = iwt->iwt_WindowTask.wt_Window->Height - iwt->iwt_ExtraHeight;

	if (ws->ws_Width != newWidth || ws->ws_Height != newHeight)
		{
		struct PatternNode *ptNode;

		ws->ws_Width = newWidth;
		ws->ws_Height = newHeight;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetInnerSize);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
			SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);

		ptNode = ws->ws_PatternNode;

		d1(KPrintF("%s/%s/%ld: type=%ld\n", __FILE__, __FUNC__, __LINE__, ptNode->ptn_type));

		 if (PatternPrefs.patt_NewRenderFlag && ptNode &&
			((SCP_RenderType_FitSize == ptNode->ptn_type)
				|| (SCP_RenderType_ScaledMin == ptNode->ptn_type)
				|| (SCP_RenderType_ScaledMax == ptNode->ptn_type)
				|| (SCP_RenderType_Centered == ptNode->ptn_type))
			&& iwt->iwt_WindowTask.wt_PatternInfo.ptinf_bitmap)
			{
			d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			NewWindowPatternMsg(iwt, NULL, (struct PatternNode *) &ws->ws_PatternNode);
			}

		if (iwt->iwt_Reading)
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			Forbid();
			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_RemClipRegion, iwt->iwt_RemRegion);
			iwt->iwt_RemRegion = (struct Region *) DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_InitClipRegion);
			d1(kprintf("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));
			Permit();
			}

		if (CurrentPrefs.pref_AutoCleanupOnResize && IsIwtViewByIcon(iwt))
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			WindowClass_ReCleanup(iwt, OldInnerWidth, OldInnerHeight);
			}
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetInnerSize);

	RedrawResizedWindow(iwt, OldInnerWidth, OldInnerHeight);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static ULONG WindowClass_DynamicResizeWindow(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	WORD newWidth, newHeight;
	WORD OldInnerHeight = iwt->iwt_InnerHeight;
	WORD OldInnerWidth = iwt->iwt_InnerWidth;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	newWidth = iwt->iwt_WindowTask.wt_Window->Width - iwt->iwt_ExtraWidth;
	newHeight = iwt->iwt_WindowTask.wt_Window->Height - iwt->iwt_ExtraHeight;

	if (ws->ws_Width != newWidth || ws->ws_Height != newHeight)
		{
		struct PatternNode *ptNode;

		ws->ws_Width = newWidth;
		ws->ws_Height = newHeight;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
			SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);

		d1(kprintf("%s/%s/%ld: type=%ld\n", __FILE__, __FUNC__, __LINE__, ptNode->ptn_type));

		ptNode = ws->ws_PatternNode;

		 if (PatternPrefs.patt_NewRenderFlag && ptNode &&
			((SCP_RenderType_FitSize == ptNode->ptn_type)
				|| (SCP_RenderType_ScaledMin == ptNode->ptn_type)
				|| (SCP_RenderType_ScaledMax == ptNode->ptn_type)
				|| (SCP_RenderType_Centered == ptNode->ptn_type))
			&& iwt->iwt_WindowTask.wt_PatternInfo.ptinf_bitmap)
			{
			d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			NewWindowPatternMsg(iwt, NULL, (struct PatternNode *) &ws->ws_PatternNode);
			}

		if (iwt->iwt_Reading)
			{
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			Forbid();
			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_RemClipRegion, iwt->iwt_RemRegion);
			iwt->iwt_RemRegion = (struct Region *) DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_InitClipRegion);
			d1(kprintf("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));
			Permit();
			}

		if (CurrentPrefs.pref_AutoCleanupOnResize && IsIwtViewByIcon(iwt))
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			WindowClass_ReCleanup(iwt, OldInnerWidth, OldInnerHeight);
			}
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


static ULONG WindowClass_Set(Class *cl, Object *o, Msg msg)
{
	struct WindowClassInstance *inst = INST_DATA(cl, o);
	struct opSet *ops = (struct opSet *) msg;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (FindTagItem(SCCA_Window_Transparency, ops->ops_AttrList))
		{
		WindowClass_SetTransparency(cl, o,
			GetTagData(SCCA_Window_Transparency, inst->wci_Transparency, ops->ops_AttrList));
		}

	return DoSuperMethodA(cl, o, msg);
}


static ULONG WindowClass_Get(Class *cl, Object *o, Msg msg)
{
	struct WindowClassInstance *inst = INST_DATA(cl, o);
	struct opGet *opg = (struct opGet *) msg;
	ULONG Result = 0;

	if (NULL == opg->opg_Storage)
		return 0;

	*(opg->opg_Storage) = 0;

	switch (opg->opg_AttrID)
		{
	case SCCA_Window_Transparency:
		*(opg->opg_Storage) = inst->wci_Transparency;
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}


static ULONG WindowClass_SetTransparency(Class *cl, Object *o, ULONG NewTransparency)
{
	struct WindowClassInstance *inst = INST_DATA(cl, o);
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;

	d1(KPrintF("%s/%s/%ld: START NewTransparency=%ld\n", __FILE__, __FUNC__, __LINE__, NewTransparency));

	if (ws->ws_Window && !iwt->iwt_BackDrop)
		{
		ULONG Transparency;
		const ULONG Step = 10;

		for (Transparency = inst->wci_Transparency; Transparency != NewTransparency; )
			{
			d1(KPrintF("%s/%s/%ld: Transparency=%ld\n", __FILE__, __FUNC__, __LINE__, Transparency));

			if (Transparency < NewTransparency)
				{
				if (( NewTransparency - Transparency) > Step)
					Transparency += Step;
				else
					Transparency = NewTransparency;
				}
			else
				{
				if ((Transparency - NewTransparency) > Step)
					Transparency -= Step;
				else
					Transparency = NewTransparency;
				}

#if defined(__MORPHOS__) && defined(WA_Opacity)
			SetAttrs(ws->ws_Window,
				WA_SCA_Opaqueness, SCALOS_OPAQUENESS(Transparency),
				TAG_END);
#elif defined(__amigaos4__) && defined(WA_Opaqueness)
			SetWindowAttrs(ws->ws_Window,
				WA_OverrideOpaqueness, TRUE,
				WA_SCA_Opaqueness, SCALOS_OPAQUENESS(Transparency),
				TAG_END);
#endif
			if (Transparency != NewTransparency)
				Delay(1);
			}
		}
	inst->wci_Transparency = NewTransparency;

	d1(KPrintF("%s/%s/%ld: END wci_Transparency=%ld\n", __FILE__, __FUNC__, __LINE__, inst->wci_Transparency));

	return 0;
}


static void WindowClass_ReCleanup(struct internalScaWindowTask *iwt, WORD OldInnerWidth, WORD OldInnerHeight)
{
	BOOL UnCleanup = FALSE;
	struct Region *UnCleanupRegion = NULL;
	struct Rect32 AllIconsBBox = iwt->iwt_IconBBox;

	d1(KPrintF("%s/%s/%ld: MaxX=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconBBox.MaxX, iwt->iwt_IconBBox.MaxY));
	d1(KPrintF("%s/%s/%ld: iwt_InnerWidth=%ld  iwt_InnerHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerWidth, iwt->iwt_InnerHeight));
	d1(KPrintF("%s/%s/%ld: OldInnerWidth=%ld  OldInnerHeight=%ld\n", __FILE__, __FUNC__, __LINE__, OldInnerWidth, OldInnerHeight));
	d1(KPrintF("%s/%s/%ld: wt_XOffset=%ld wt_YOffse=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_XOffset, iwt->iwt_WindowTask.wt_YOffset));

	AllIconsBBox.MaxX -= iwt->iwt_WindowTask.wt_XOffset;
	AllIconsBBox.MaxY -= iwt->iwt_WindowTask.wt_YOffset;

	if ((AllIconsBBox.MaxX <= iwt->iwt_InnerWidth && AllIconsBBox.MaxX > OldInnerWidth)
		|| (AllIconsBBox.MaxY <= iwt->iwt_InnerHeight && AllIconsBBox.MaxY > OldInnerHeight) )
		{
		d1(KPrintF("%s/%s/%ld: SCCM_IconWin_UnCleanUp\n", __FILE__, __FUNC__, __LINE__));
		UnCleanup = TRUE;
		}

	if (!UnCleanup && AllIconsBBox.MaxX > iwt->iwt_InnerWidth && AllIconsBBox.MaxX > OldInnerWidth
			&& AllIconsBBox.MaxY <= iwt->iwt_InnerHeight)
		{
		d1(KPrintF("%s/%s/%ld: iwt_LastUnCleanupInnerWidth=%ld  iwt_InnerWidt=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_LastUnCleanupInnerWidth, iwt->iwt_InnerWidth));
		d1(KPrintF("%s/%s/%ld: iwt_LastUnCleanupInnerHeight=%ld  iwt_InnerHeight=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_LastUnCleanupInnerHeight, iwt->iwt_InnerHeight));

		if (abs(iwt->iwt_LastUnCleanupInnerWidth - iwt->iwt_InnerWidth) >= 10
			|| abs(iwt->iwt_LastUnCleanupInnerHeight - iwt->iwt_InnerHeight) >= 10)
			{
			d1(KPrintF("%s/%s/%ld: SCCM_IconWin_UnCleanUp\n", __FILE__, __FUNC__, __LINE__));
			UnCleanup = TRUE;

			UnCleanupRegion = NewRegion();
			if (UnCleanupRegion)
				{
				struct Rectangle Rect;
				
				Rect.MinX = iwt->iwt_InnerWidth;
				Rect.MinY = iwt->iwt_IconBBox.MinY;
				Rect.MaxX = iwt->iwt_IconBBox.MaxX;
				Rect.MaxY = iwt->iwt_IconBBox.MaxY;

				d1(KPrintF("%s/%s/%ld: Rect: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, Rect.MinX, Rect.MinY, Rect.MaxX, Rect.MaxY));

				OrRectRegion(UnCleanupRegion, &Rect);
				}
			}
		}

	if (!UnCleanup && AllIconsBBox.MaxY > iwt->iwt_InnerHeight && AllIconsBBox.MaxY > OldInnerHeight
			&& AllIconsBBox.MaxX <= iwt->iwt_InnerWidth)
		{
		d1(KPrintF("%s/%s/%ld: iwt_LastUnCleanupInnerWidth=%ld  iwt_InnerWidt=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_LastUnCleanupInnerWidth, iwt->iwt_InnerWidth));
		d1(KPrintF("%s/%s/%ld: iwt_LastUnCleanupInnerHeight=%ld  iwt_InnerHeight=%ld\n", __FILE__, __FUNC__, __LINE__, \
			iwt->iwt_LastUnCleanupInnerHeight, iwt->iwt_InnerHeight));

		if (abs(iwt->iwt_LastUnCleanupInnerWidth - iwt->iwt_InnerWidth) >= 10
			|| abs(iwt->iwt_LastUnCleanupInnerHeight - iwt->iwt_InnerHeight) >= 10)
			{
			d1(KPrintF("%s/%s/%ld: SCCM_IconWin_UnCleanUp\n", __FILE__, __FUNC__, __LINE__));
			UnCleanup = TRUE;

			UnCleanupRegion = NewRegion();
			if (UnCleanupRegion)
				{
				struct Rectangle Rect;

				Rect.MinX = iwt->iwt_IconBBox.MinX;
				Rect.MinY = iwt->iwt_InnerHeight;
				Rect.MaxX = iwt->iwt_IconBBox.MaxX;
				Rect.MaxY = iwt->iwt_IconBBox.MaxY;

				d1(KPrintF("%s/%s/%ld: Rect: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, Rect.MinX, Rect.MinY, Rect.MaxX, Rect.MaxY));

				OrRectRegion(UnCleanupRegion, &Rect);
				}
			}
		}

	if (UnCleanup)
		{
		d1(KPrintF("%s/%s/%ld: SCCM_IconWin_UnCleanUp\n", __FILE__, __FUNC__, __LINE__));

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_UnCleanUpRegion,
			UnCleanupRegion);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
			SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);

		iwt->iwt_LastUnCleanupInnerWidth = iwt->iwt_InnerWidth;
		iwt->iwt_LastUnCleanupInnerHeight = iwt->iwt_InnerHeight;
		}

	if (UnCleanupRegion)
		DisposeRegion(UnCleanupRegion);
}

