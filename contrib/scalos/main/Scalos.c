// Scalos.c
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
#include <cybergraphx/cybergraphics.h>
#include <datatypes/pictureclass.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/utility.h>
#include <proto/wb.h>
#include <proto/ttengine.h>
#include "debug.h"
#include <proto/scalosgfx.h>
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local functions

static void FreeAppIconNode(struct internalScaWindowTask *iwt,
	struct ScaIconNode **IconList, struct ScaIconNode *in);

//----------------------------------------------------------------------------

// public Data

const int Max_PathLen = 1000;

struct MsgPort *FBlitPort = NULL;		// Result defined in FindPort("FBlit")

SCALOSSEMAPHORE	QuitSemaphore;			// +jl+ 20011130 semaphore to prevent main process from premature quitting
SCALOSSEMAPHORE	MenuSema;			// +jl+ 20010825 semaphore to protect menu access defined in changing windows
SCALOSSEMAPHORE	LayersSema;			// +jl+ 20010806 semaphore to protect OpenWindow/CloseWindow/Layers operations
SCALOSSEMAPHORE	WBStartListSema;		// semaphore to protect _WBStartList
SCALOSSEMAPHORE	PatternSema; 			// +jl+ 20010826 Pattern semaphore
SCALOSSEMAPHORE	ClassListSemaphore;		// +jl+ 20010929 Class list semaphore
SCALOSSEMAPHORE	DoWaitSemaphore;		// +jl+ 20011001 Semaphore for DOWAIT-List

struct List WBStartList;			// list of programs launched by scalos

struct ScalosClass *ScalosClassList = NULL;	// +jl+ 20010929 Scalos Class list
struct DoWait *DoWaitList = NULL;		// +jl+ 20011001 List of Msgs for DOWAIT

struct IClass *DtImageClass = NULL;		// Image Class wrapper for Scalos Datatypes images
struct IClass *TextIconClass = NULL;		// Class for View By Text mode
struct IClass *TextIconHighlightClass = NULL;	// Class for highlighting of text mode icons
struct IClass *IconifyImageClass = NULL;	// +jl+ 20010831 Imageclass for the iconify gadget

UWORD WBLaunchCount = 0;			// number of workbench launched programs
ULONG RandomSeed = 0;				// +jl+ 20011130 RandomSeed to generate good random numbers

APTR PenShareMap = NULL;			// +jl+ 20011118 PenShareMap defined in guigfx

ULONG OldMemSize = 0;				// +jl+ 20010831 Old total memorysize

struct PluginClass *ScalosPluginList = NULL;	// +jl+ 20011009 List of plugin classes
Object *ScreenTitleObject = NULL;		// +jl+ 20010831 Scalos Title Object

struct Locale *ScalosLocale = NULL;		// +jl+ 20010803
struct Catalog *ScalosCatalog = NULL;		// +jl+ 20010829

struct TextFont *IconWindowFont = NULL;		// +jl+ 20010710 Font for icon window icons
struct TextFont *TextWindowFont = NULL;		// +jl+ 20010710 Font for text window icons

struct TTFontFamily IconWindowTTFont;		// +jl+ 20030110 TTengine Font for icon window icons
struct TTFontFamily TextWindowTTFont;		// +jl+ 20030110 TTengine Font for text window icons

struct TTFontFamily ScreenTTFont;		// TTengine screen font

STRPTR LastScreenTitleBuffer = NULL;		// +jl+ 20011226 Scalos ScreenTitle (Last Msg)
STRPTR ScreenTitleBuffer = NULL;		// +jl+ 20010831 Scalos ScreenTitle

UBYTE IconActive = 0;
UBYTE PopupMenuFlag = FALSE;			// Flag: Are popupmenus available +jl+ 20010303


struct MsgPort *wbPort = NULL;			// Scalos main Message port

struct MainTask *MainWindowTask = NULL;		// +jl+ 20011020 MainTask
struct Process *MainTask = NULL;		// Scalos main process +jl+ 20010504

struct extendedScaInternInfos iInfos =
	{
	// ScaInternInfos
	{
	NULL,			// ii_iInfos.xii_iinfos.ii_MainMsgPort
	NULL,			// ii_iInfos.xii_iinfos.ii_MainWindowStruct
	NULL,			// ii_AppWindowStruct
	NULL,			// ii_Screen
	NULL,			// ii_DrawInfo
	NULL			// ii_visualinfo
	},
	// GlobalGadgetUnderPointer
	{
	NULL,			// ggd_iwt
	NULL,			// ggd_Gadget
	NULL,       		// ggd_cgy
	NULL,			// ggd_GadgetTextHook
	SGTT_GADGETID_unknown,	// ggd_GadgetID
	},
	};

//----------------------------------------------------------------------------

// local Data

//----------------------------------------------------------------------------

#if 0
void SelectIcons(WORD StartX, WORD StartY, WORD StopX, WORD StopY, struct internalScaWindowTask *iwt)
{
	struct Region *Region1, *Region2;
	struct Rectangle myRect;
	struct Layer *layer;
	WORD x, y;

	if (StartX > StopX)
		{
		x = StopX;
		StopX = StartX;
		StartX = x;
		}
	if (StartY > StopY)
		{
		y = StopY;
		StopY = StartY;
		StartY = y;
		}

	Region1 = NewRegion();
	if (NULL == Region1)
		return;
	Region2 = NewRegion();
	if (NULL == Region1)
		{
		DisposeRegion(Region1);
		return;
		}

	myRect.MinX = StartX;
	myRect.MinY = StartY;
	myRect.MaxX = StopX;
	myRect.MaxY = StopY;

	OrRectRegion(Region1, &myRect);

	d1(kprintf("%s/%s/%ld: Region1: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld  RegionRectangle=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, Region1->bounds.MinX, Region1->bounds.MinY, Region1->bounds.MaxX, \
		Region1->bounds.MaxY, Region1->RegionRectangle));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (layer = iwt->iwt_WindowTask.wt_Window->WLayer->LayerInfo->top_layer; layer; layer=layer->back)
		{
		struct Window *win = layer->Window;
		struct ScaWindowStruct *ws;

		d1(kprintf("%s/%s/%ld: layer=%08lx\n", __FILE__, __FUNC__, __LINE__, layer));

		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: ws=%08lx  ws_Window=%08lx  win=%08lx\n", __FILE__, __FUNC__, __LINE__, ws, ws->ws_Window, win));

			if (win == ws->ws_Window)
				{
				struct Region *Region3;
				struct ScaIconNode *in;
				struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

				myRect.MinX = myRect.MaxX = iwtx->iwt_InnerLeft + win->LeftEdge;
				myRect.MaxX += win->Width - iwtx->iwt_InnerRight - 1;

				myRect.MinY = myRect.MaxY = iwtx->iwt_InnerTop + win->TopEdge;
				myRect.MaxY += win->Height + iwtx->iwt_InnerBottom - 1;

				OrRectRegion(Region2, &myRect);

				d1(kprintf("%s/%s/%ld: Region2: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld  RegionRectangle=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, Region2->bounds.MinX, Region2->bounds.MinY, Region2->bounds.MaxX, \
					Region2->bounds.MaxY, Region2->RegionRectangle));

				AndRegionRegion(Region1, Region2);

				d1(kprintf("%s/%s/%ld: Region2: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld  RegionRectangle=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, Region2->bounds.MinX, Region2->bounds.MinY, Region2->bounds.MaxX, \
					Region2->bounds.MaxY, Region2->RegionRectangle));

				d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

				if (!TestRegionNotEmpty(Region2))
					break;

				ScalosLockIconListShared(iwtx);

				Region3 = NewRegion();

				d1(kprintf("%s/%s/%ld: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld\n", \
					__FILE__, __FUNC__, __LINE__, myRect.MinX, myRect.MinY, myRect.MaxX, myRect.MaxY));

				for (in=iwtx->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					struct Rectangle IconRect;
					struct Gadget *gg = (struct Gadget *) in->in_Icon;

					d1(kprintf("%s/%s/%ld: in=%08lx <%s>\n", \
						__FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

					IconRect.MinX = IconRect.MaxX = win->LeftEdge + gg->LeftEdge - iwtx->iwt_WindowTask.wt_XOffset;
					IconRect.MinY = IconRect.MaxY = win->TopEdge + gg->TopEdge - iwtx->iwt_WindowTask.wt_YOffset;
					IconRect.MaxX += gg->Width - 1;
					IconRect.MaxY += gg->Height - 1;

					d1(kprintf("%s/%s/%ld: xmin=%ld  ymin=%ld  xmax=%ld  ymax=%ld\n", \
						__FILE__, __FUNC__, __LINE__, IconRect.MinX, IconRect.MinY, IconRect.MaxX, IconRect.MaxY));

					if (IconRect.MinX < myRect.MaxX && IconRect.MinY < myRect.MaxY && 
						IconRect.MaxX >= myRect.MinX &&
						IconRect.MaxY >= myRect.MinY)
						{
						OrRectRegion(Region3, &IconRect);
						AndRegionRegion(Region2, Region3);

						if (TestRegionNotEmpty(Region3))
							{
							gg->Flags |= GFLG_SELECTED;
							IconActive = TRUE;

							if (NULL == gg->SelectRender)
								{
								DoMethod(iwtx->mt_MainObject, SCCM_IconWin_LayoutIcon, 
									gg, IOLAYOUTF_SelectedImage);
								}

							DoMethod(iwtx->mt_MainObject, SCCM_IconWin_DrawIcon, gg);
							}
						}

					ClearRegion(Region3);
					}
				DisposeRegion(Region3);

				ScalosUnLockIconList(iwtx);
				}
			}

		ClearRegion(Region2);
		ClearRectRegion(Region1, &layer->bounds);
		}

	SCA_UnLockWindowList();

	DisposeRegion(Region2);
	DisposeRegion(Region1);
}
#endif


// Test if Region is empty
BOOL TestRegionNotEmpty(const struct Region *region)
{
	if ((region->bounds.MaxX != region->bounds.MinX ||
		region->bounds.MaxY != region->bounds.MinY) &&
		region->RegionRectangle)
		return TRUE;

	return FALSE;
}


void RefreshIconList(struct internalScaWindowTask *iwt, struct ScaIconNode *in, struct Region *DrawingRegion)
{
	struct RastPort *rp;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
		{
		iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;
		return;
		}

	if (iwt->iwt_HoldUpdate)
		return;

	d1(kprintf("\n%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

	rp = iwt->iwt_WindowTask.wt_Window->RPort;

	SetABPenDrMd(rp, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, FontPrefs.fprf_TextDrawMode);

	while (in)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
		struct Rectangle IconRect;

		d1(kprintf("%s/%s/%ld: in=%08lx  in_Icon=%08lx  <%s>\n", \
			__FILE__, __FUNC__, __LINE__, in, in->in_Icon, GetIconName(in)));

		IconRect.MinX = gg->BoundsLeftEdge + iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset;
		IconRect.MinY = gg->BoundsTopEdge + iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset;
		IconRect.MaxX = IconRect.MinX + gg->BoundsWidth - 1;
		IconRect.MaxY = IconRect.MinY + gg->BoundsHeight - 1;

		if (NULL == DrawingRegion || ScaRectInRegion(DrawingRegion, &IconRect))
			{
			d1(kprintf("%s/%s/%ld: in=%08lx  in_Icon=%08lx  <%s>\n", \
				__FILE__, __FUNC__, __LINE__, in, in->in_Icon, GetIconName(in)));

			if (gg->Flags & GFLG_SELECTED)
				{
				if (NULL == gg->SelectRender)
					{
					DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
						SCCM_IconWin_LayoutIcon,
						in->in_Icon,
						IOLAYOUTF_SelectedImage);
					}
				}
			else
				{
				if (NULL == gg->GadgetRender)
					{
					DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
						SCCM_IconWin_LayoutIcon,
						in->in_Icon,
						IOLAYOUTF_NormalImage);
					}
				}

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, 
				SCCM_IconWin_DrawIcon,
				in->in_Icon);
			}

		in = (struct ScaIconNode *) in->in_Node.mln_Succ;
		}

	d1(kprintf("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
}


void RefreshIcons(struct internalScaWindowTask *iwt, struct Region *DrawingRegion)
{
	d1(kprintf("\n%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

	if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
		{
		iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;
		d1(kprintf("\n%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
		return;
		}

	if (iwt->iwt_HoldUpdate)
		{
		d1(kprintf("\n%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
		return;
		}

	RefreshIconList(iwt, iwt->iwt_WindowTask.wt_IconList, DrawingRegion);
}


ULONG ChipMemAttr(void)
{
	if (CyberGfxBase && IsCyberModeID(GetVPModeID(&iInfos.xii_iinfos.ii_Screen->ViewPort)))
		return MEMF_ANY;

	if (FBlitPort)
		return MEMF_ANY;

	return MEMF_CHIP;
}


void FreeIconList(struct internalScaWindowTask *iwt, struct ScaIconNode **IconList)
{
	while (*IconList)
		{
		struct ScaIconNode *in = *IconList;
		ULONG IconType;

		GetAttr(IDTA_Type, in->in_Icon, &IconType);

		if (WBAPPICON == IconType)
			FreeAppIconNode(iwt, IconList, in);
		else
			FreeIconNode(iwt, IconList, in);

		}

	*IconList = NULL;
}


// Don't free ScaIconNode for AppIcons
static void FreeAppIconNode(struct internalScaWindowTask *iwt,
	struct ScaIconNode **IconList, struct ScaIconNode *in)
{
	if (in->in_Lock)
		UnLock(in->in_Lock);

	if (in->in_DeviceIcon)
		{
		DoMethod(iwt->iwt_DevListObject, SCCM_DeviceList_FreeDevNode, in->in_DeviceIcon);
		SCA_FreeNode((struct ScalosNodeList *) &iwt->iwt_DevIconList, &in->in_DeviceIcon->di_Node);
		}

	SCA_FreeNode((struct ScalosNodeList *) IconList, &in->in_Node);
}


void FreeIconNode(struct internalScaWindowTask *iwt,
	struct ScaIconNode **IconList, struct ScaIconNode *in)
{
	d1(kprintf("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	if (in->in_Lock)
		{
		d1(kprintf("%s/%s/%ld: in_IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, in->in_IconList));

		// remove ScalosBackdropIcon from in_IconList of the parent device icon
		RemoveScalosBackdropIcon(in);

		UnLock(in->in_Lock);
		in->in_Lock = (BPTR)NULL;
		}

	RemoveThumbnailEntry(iwt, in->in_Icon);

	DisposeIconObject(in->in_Icon);
	in->in_Icon = NULL;

	if (in->in_DeviceIcon)
		{
		DoMethod(iwt->iwt_DevListObject, SCCM_DeviceList_FreeDevNode, in->in_DeviceIcon);
		SCA_FreeNode((struct ScalosNodeList *) &iwt->iwt_DevIconList, &in->in_DeviceIcon->di_Node);
		in->in_DeviceIcon = NULL;
		}

	d1(kprintf("%s/%s/%ld: in_IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, in->in_IconList));

	FreeBackdropIconList(&in->in_IconList);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SCA_FreeNode((struct ScalosNodeList *) IconList, &in->in_Node);
}


LONG ScaSameLock(BPTR Lock1, BPTR Lock2)
{
	if (BNULL == Lock1 && BNULL == Lock2)
		return LOCK_SAME;

	if (BNULL == Lock1 || BNULL == Lock2)
		return LOCK_DIFFERENT;

	return SameLock(Lock1, Lock2);
}

// ----------------------------------------------------------

void DisplayScreenTitleError(struct internalScaWindowTask *iwt, ULONG MsgId)
{
	struct SM_ShowTitle *sMsg;

	SCA_ScreenTitleMsg(GetLocString(MsgId), NULL);

	// Make sure screen title is visible
	sMsg = (struct SM_ShowTitle *) SCA_AllocMessage(MTYP_ShowTitle, 0);
	if (sMsg)
		{
		sMsg->smst_showTitle = TRUE;
		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &sMsg->ScalosMessage.sm_Message);
		}

	DisplayBeep(iwt ? iwt->iwt_WinScreen : NULL);
}


BOOL ExistsAssign(CONST_STRPTR AssignName)
{
	STRPTR assignNameCopy;
	BOOL exists = FALSE;

	assignNameCopy = AllocCopyString(AssignName);

	if (assignNameCopy)
		{
		struct DosList *dList;

		StripTrailingColon(assignNameCopy);

		dList = LockDosList(LDF_ASSIGNS | LDF_READ);

		exists = NULL != FindDosEntry(dList, assignNameCopy, LDF_ASSIGNS);

		UnLockDosList(LDF_ASSIGNS | LDF_READ);

		FreeCopyString(assignNameCopy);
		}

	return exists;
}


LONG ScalosTagListInit(struct ScalosTagList *tagList)
{
	LONG Result = RETURN_OK;

	tagList->stl_Index = 0;
	tagList->stl_MaxEntries = SCALOSTAGLIST_STEP;

	tagList->stl_TagList = ScalosAlloc(sizeof(struct TagItem) * tagList->stl_MaxEntries);
	d1(kprintf("%s/%s/%ld: ALLOC TagList=%08lx\n", __FILE__, __FUNC__, __LINE__, tagList->stl_TagList));
	if (NULL == tagList->stl_TagList)
		Result = ERROR_NO_FREE_STORE;
	else
		tagList->stl_TagList[0].ti_Tag = TAG_END;

	return Result;
}


void ScalosTagListCleanup(struct ScalosTagList *tagList)
{
	if (tagList)
		{
		if (tagList->stl_TagList)
			{
			d1(kprintf("%s/%s/%ld: FREE TagList=%08lx\n", __FILE__, __FUNC__, __LINE__, tagList->stl_TagList));
			ScalosFree(tagList->stl_TagList);
			tagList->stl_TagList = NULL;
			}
		tagList->stl_MaxEntries = 0;
		}
}


LONG ScalosTagListNewEntry(struct ScalosTagList *tagList, ULONG tag, ULONG data)
{
	if (NULL == tagList)
		return RETURN_ERROR;

	tagList->stl_TagList[tagList->stl_Index].ti_Tag = tag;
	tagList->stl_TagList[tagList->stl_Index].ti_Data = data;

	if (++tagList->stl_Index >= tagList->stl_MaxEntries)
		{
		struct TagItem *NewTagList;
		ULONG NewEntries = tagList->stl_MaxEntries * 2;

		tagList->stl_MaxEntries *= 2;

		NewTagList = ScalosAlloc(sizeof(struct TagItem) * NewEntries);
		d1(kprintf("%s/%s/%ld: ALLOC TagList=%08lx\n", __FILE__, __FUNC__, __LINE__, NewTagList));
		if (NULL == NewTagList)
			return RETURN_ERROR;

		memcpy(NewTagList, tagList->stl_TagList, sizeof(struct TagItem) * tagList->stl_MaxEntries);

		d1(kprintf("%s/%s/%ld: FREE TagList=%08lx\n", __FILE__, __FUNC__, __LINE__, tagList->stl_TagList));
		ScalosFree(tagList->stl_TagList);

		tagList->stl_TagList = NewTagList;
		tagList->stl_MaxEntries = NewEntries;
		}

	return RETURN_OK;
}


void ScalosTagListEnd(struct ScalosTagList *tagList)
{
	if (tagList)
		tagList->stl_TagList[tagList->stl_Index].ti_Tag = TAG_END;
}

struct TagItem *ScalosVTagList(ULONG FirstTag, va_list args)
{
	struct TagItem *ClonedTagList;
	ULONG AllocatedSize = 10;

	d1(KPrintF("%s/%s/%ld: FirstTag=%08lx  args=%08lx\n", __FILE__, __FUNC__, __LINE__, FirstTag, args));

	do	{
		ULONG NumberOfTags = 1;
		BOOL Finished = FALSE;
		struct TagItem *ti;

		ClonedTagList = ti = AllocateTagItems(AllocatedSize);
		if (NULL == ClonedTagList)
			break;

		ClonedTagList->ti_Tag = FirstTag;
		while (!Finished)
			{
			switch (ti->ti_Tag)
				{
			case TAG_MORE:
				ti->ti_Data = va_arg(args, ULONG);
				Finished = TRUE;
				break;
			case TAG_END:
				Finished = TRUE;
				break;
			default:
				ti->ti_Data = va_arg(args, ULONG);
				break;
				}

			d1(KPrintF("%s/%s/%ld  ti=%08lx  Tag=%08lx  Data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti, ti->ti_Tag, ti->ti_Data));

			if (!Finished)
				{
				if (++NumberOfTags >= AllocatedSize)
					{
					// we need to extend our allocated taglist
					struct TagItem *ExtendedTagList;
					ULONG ExtendedSize = 2 * AllocatedSize;

					ExtendedTagList	= AllocateTagItems(ExtendedSize);
					if (NULL == ExtendedTagList)
						{
						FreeTagItems(ClonedTagList);
						ClonedTagList = NULL;
						break;
						}

					d1(KPrintF("%s/%s/%ld: ExtendedTagList=%08lx  ClonedTagList=%08lx\n", __FILE__, __FUNC__, __LINE__, ExtendedTagList, ClonedTagList));

					// copy contents from old to extended taglist
					memcpy(ExtendedTagList, ClonedTagList, sizeof(struct TagItem) * AllocatedSize);

					// free old taglist
					FreeTagItems(ClonedTagList);
					ClonedTagList = ExtendedTagList;

					ti = ClonedTagList + NumberOfTags - 1;
					AllocatedSize = ExtendedSize;
					}
				else
					{
					ti++;
					}

				ti->ti_Tag = va_arg(args, ULONG);
				}
			}

		d1(KPrintF("%s/%s/%ld: NumberOfTags=%lu  AllocatedSize=%lu\n", __FILE__, __FUNC__, __LINE__, NumberOfTags, AllocatedSize));
		} while (0);

	d1(KPrintF("%s/%s/%ld: ClonedTagList=%08lx\n", __FILE__, __FUNC__, __LINE__, ClonedTagList));

	d1({ ULONG n; \
		for (n = 0; ClonedTagList; n++) \
			{ \
			KPrintF("%s/%s/%ld: TagList[%ld]: Tag=%08lx  Data=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, n, ClonedTagList[n].ti_Tag, ClonedTagList[n].ti_Data); \
			if (TAG_END == ClonedTagList[n].ti_Tag || TAG_MORE == ClonedTagList[n].ti_Tag)
				break;
			} \
		})

	return ClonedTagList;
}

// ----------------------------------------------------------

ULONG TranslateViewModesFromIcon(ULONG IconViewMode)
{
	ULONG ScalosViewMode;

	switch (IconViewMode)
		{
	case DDVM_BYICON:
		ScalosViewMode = IDTV_ViewModes_Icon;
		break;
	case DDVM_BYNAME:
		ScalosViewMode = IDTV_ViewModes_Name;
		break;
	case DDVM_BYDATE:
		ScalosViewMode = IDTV_ViewModes_Date;
		break;
	case DDVM_BYSIZE:
		ScalosViewMode = IDTV_ViewModes_Size;
		break;
	case DDVM_BYTYPE:
		ScalosViewMode = IDTV_ViewModes_Type;
		break;
	case DDVM_BYDEFAULT:
	default:
		ScalosViewMode = IDTV_ViewModes_Default;
		break;
		}

	return ScalosViewMode;
}

// ----------------------------------------------------------

ULONG TranslateViewModesToIcon(ULONG ScalosViewMode)
{
	ULONG IconViewMode;

	switch (ScalosViewMode)
		{
	case IDTV_ViewModes_Icon:
		IconViewMode = DDVM_BYICON;
		break;
	case IDTV_ViewModes_Name:
	case IDTV_ViewModes_Comment:
	case IDTV_ViewModes_Protection:
	case IDTV_ViewModes_Owner:
	case IDTV_ViewModes_Group:
	case IDTV_ViewModes_Version:
	case IDTV_ViewModes_MiniIcon:
		IconViewMode = DDVM_BYNAME;
		break;
	case IDTV_ViewModes_Date:
	case IDTV_ViewModes_Time:
		IconViewMode = DDVM_BYDATE;
		break;
	case IDTV_ViewModes_Size:
		IconViewMode = DDVM_BYSIZE;
		break;
	case IDTV_ViewModes_Type:
		IconViewMode = DDVM_BYTYPE;
		break;
	default:
		IconViewMode = DDVM_BYDEFAULT;
		break;
		}

	return IconViewMode;
}

// ----------------------------------------------------------

ULONG TranslateScalosViewMode(ULONG ScalosViewMode)
{
	if (IDTV_ViewModes_Default == ScalosViewMode)
		return CurrentPrefs.pref_DefaultViewBy;

	return ScalosViewMode;
}

// ----------------------------------------------------------

BOOL IsIwtViewByIcon(struct internalScaWindowTask *iwt)
{
	return (BOOL) (WSV_Type_DeviceWindow == iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType
		|| IsViewByIcon(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes));
}

// ----------------------------------------------------------

BOOL IsViewByIcon(ULONG ScalosViewMode)
{
	BOOL ViewByIcon;

	switch (ScalosViewMode)
		{
	case IDTV_ViewModes_Default:
		ViewByIcon = IDTV_ViewModes_Icon == CurrentPrefs.pref_DefaultViewBy;
		break;
	case IDTV_ViewModes_Icon:
		ViewByIcon = TRUE;
		break;
	default:
		ViewByIcon = FALSE;
		break;
		}

	return ViewByIcon;
}

// ----------------------------------------------------------

BOOL IsShowAll(const struct ScaWindowStruct *ws)
{
	return IsShowAllType(ws->ws_ViewAll);
}

// ----------------------------------------------------------

BOOL IsShowAllType(UWORD ShowType)
{
	BOOL ViewAll;

	switch (ShowType & DDFLAGS_SHOWMASK)
		{
	case DDFLAGS_SHOWDEFAULT:
		ViewAll = CurrentPrefs.pref_DefaultShowAllFiles;
		break;
	case DDFLAGS_SHOWICONS:
		ViewAll = FALSE;
		break;
	case DDFLAGS_SHOWALL:
		ViewAll = TRUE;
		break;
	default:
		ViewAll = CurrentPrefs.pref_DefaultShowAllFiles;
		break;
		}

	return ViewAll;
}

// ----------------------------------------------------------

void ScalosEndNotify(struct NotifyRequest *nr)
{
	d1(KPrintF("%s/%s/%ld: START  nr=%08lx\n", __FILE__, __FUNC__, __LINE__, nr));
	if (nr)
		{
#if defined(__MORPHOS__)
		struct MsgPort *DummyPort = NULL;
		struct MsgPort *NotifyPort = NULL;

		d1(KPrintF("%s/%s/%ld: nr_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, nr->nr_Name));

		Forbid();

		if (nr->nr_Flags & NRF_SEND_MESSAGE)
			{
			NotifyPort = nr->nr_stuff.nr_Msg.nr_Port;

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			if (!IsListEmpty(&NotifyPort->mp_MsgList))
				{
				DummyPort = CreateMsgPort();

				if (DummyPort)
					{
					struct Node *MsgNode, *NextNode;

					nr->nr_stuff.nr_Msg.nr_Port = DummyPort;
					d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					for (MsgNode = NotifyPort->mp_MsgList.lh_Head;
						nr->nr_MsgCount && MsgNode != (struct Node *) &NotifyPort->mp_MsgList.lh_Tail;
						MsgNode = NextNode)
						{
						struct NotifyMessage *nMsg = (struct NotifyMessage *) MsgNode;

						NextNode = MsgNode->ln_Succ;

						d1(KPrintF("%s/%s/%ld: nMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, nMsg));

						if (NOTIFY_CLASS == nMsg->nm_Class
							&& NOTIFY_CODE == nMsg->nm_Code
							&& nr == nMsg->nm_NReq)
							{
							d1(KPrintF("%s/%s/%ld: nMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, nMsg));
							Remove(MsgNode);
							ReplyMsg((struct Message *) MsgNode);
							d1(KPrintF("%s/%s/%ld: nMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, nMsg));
							}
						}

					d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
					}
				}
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}

		Permit();
#endif /* __MORPHOS__ */
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		EndNotify(nr);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

#if defined(__MORPHOS__)
		if (NotifyPort)
			nr->nr_stuff.nr_Msg.nr_Port = NotifyPort;
		if (DummyPort)
			{
			struct NotifyMessage *nMsg;

			while ((nMsg = (struct NotifyMessage *) GetMsg(DummyPort)))
				{
				d1(KPrintF("%s/%s/%ld: nMsg=%08lx  nm_Class=%08lx  nm_Code=%04lx\n", __FILE__, __FUNC__, __LINE__, nMsg, nMsg->nm_Class, nMsg->nm_Code));

				if (NOTIFY_CLASS == nMsg->nm_Class
					&& NOTIFY_CODE == nMsg->nm_Code
					&& nr == nMsg->nm_NReq)
					{
					d1(KPrintF("%s/%s/%ld: nMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, nMsg));
					ReplyMsg(&nMsg->nm_ExecMessage);
					d1(KPrintF("%s/%s/%ld: nMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, nMsg));
					}
				}

			DeleteMsgPort(DummyPort);
			}
#endif /* __MORPHOS__ */
		}
	d1(KPrintF("%s/%s/%ld: END  nr=%08lx\n", __FILE__, __FUNC__, __LINE__, nr));
}

// ----------------------------------------------------------

void SubtractDateStamp(struct DateStamp *from, const struct DateStamp *to)
{
	from->ds_Tick -= to->ds_Tick;
	if (from->ds_Tick < 0)
		{
		from->ds_Minute--;
		from->ds_Tick += 60 * TICKS_PER_SECOND;
		}
	from->ds_Minute -= to->ds_Minute;
	if (from->ds_Minute < 0)
		{
		from->ds_Days--;
		from->ds_Minute += 60 * 24;
		}
	from->ds_Days -= to->ds_Days;
}

//----------------------------------------------------------------------------

Object *CloneIconObject(Object *OrigIconObj)
{
	Object *IconObjClone = NULL;
	BPTR oldDir = BNULL;
	BPTR TempDirLock;
	STRPTR TempNameBuffer = NULL;

	d1(kprintf("%s/%s/%ld: START  OrigIconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, OrigIconObj));

	do	{
		LONG rc;

		TempDirLock = Lock("t:", ACCESS_READ);
		if (BNULL == TempDirLock)
			break;

		oldDir = CurrentDir(TempDirLock);

		TempNameBuffer = AllocPathBuffer();
		d1(kprintf("%s/%s/%ld: TempNameBuffer=%08lx  \n", __FILE__, __FUNC__, __LINE__, TempNameBuffer));
		if (NULL == TempNameBuffer)
			break;

		if (!TempName(TempNameBuffer, Max_PathLen))
			break;

		// PutIconObject()
		rc = PutIconObjectTags(OrigIconObj, TempNameBuffer,
			TAG_END);
		d1(kprintf("%s/%s/%ld: PutIconObjectTags returned rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (RETURN_OK != rc)
			break;

		IconObjClone = NewIconObjectTags(TempNameBuffer,
			TAG_END);
		} while (0);

	if (TempNameBuffer)
		{
		SafeStrCat(TempNameBuffer, ".info", Max_PathLen);
		(void) DeleteFile(TempNameBuffer);
		FreePathBuffer(TempNameBuffer);
		}
	if (oldDir)
		CurrentDir(oldDir);
	if (TempDirLock)
		UnLock(TempDirLock);

	d1(kprintf("%s/%s/%ld: END  IconObjClone=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObjClone));

	return IconObjClone;
}

// ----------------------------------------------------------

#if !defined(__SASC) &&!defined(__MORPHOS__)
// Replacement for SAS/C library functions

char *stpblk(const char *q)
{
	while (q && *q && isspace((int) *q))
		q++;

	return (char *) q;
}

size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}

void strins(char *to, const char *from)
{
	size_t fromLength = strlen(from);
	char *dest = to + fromLength;

	memmove(dest, to, 1 + strlen(to));
	strncpy(to, from, fromLength);
}
#endif /* __SASC */



#ifdef	DEBUG_LOCKS

#undef	Lock
#undef	UnLock
#undef	DupLock
#undef	ParentDir
#undef	ParentOfFH
#undef	DupLockFromFH

BPTR Lock_Debug(CONST_STRPTR name, LONG mode,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	BPTR lock;

	kprintf("%s/%s/%ld: Begin Lock(%s / <%s>, %ld)  Task=%08lx <%s>\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, name, mode, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name);

	lock = Lock(name, mode);

	kprintf("%s/%s/%ld: End Lock(%s / <%s>, %ld) = %08lx\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, name, mode, lock);

	return lock;
}

void UnLock_Debug(BPTR lock,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	char xxName[200];

	NameFromLock(lock, xxName, sizeof(xxName));

	kprintf("%s/%s/%ld: UnLock(%s / %08lx <%s>)  Task=%08lx <%s>\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, 
		lock, xxName, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name);
	UnLock(lock);
}

BPTR DupLock_Debug(BPTR lock,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	BPTR newLock;
	char xxName[200];

	kprintf("%s/%s/%ld: Begin DupLock(%s %08lx)  Task=%08lx <%s>\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, 
		lock, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name);

	NameFromLock(lock, xxName, sizeof(xxName));

	newLock = DupLock(lock);

	kprintf("%s/%s/%ld: End DupLock(%s %08lx <%s>) = %08lx\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, lock, xxName, newLock);

	return newLock;
}

BPTR ParentDir_Debug(BPTR lock,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	BPTR newLock;
	char xxName[200];

	kprintf("%s/%s/%ld: Begin ParentDir(%s %08lx)  Task=%08lx <%s>\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, lock, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name);

	NameFromLock(lock, xxName, sizeof(xxName));

	newLock = ParentDir(lock);

	kprintf("%s/%s/%ld: End ParentDir(%s %08lx <%s>) = %08lx\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, lock, xxName, newLock);

	return newLock;
}

BPTR ParentOfFH_Debug(BPTR fh,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	BPTR newLock;

	kprintf("%s/%s/%ld: Begin ParentOfFH(%s %08lx)  Task=%08lx <%s>\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, fh, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name);

	newLock = ParentOfFH(fh);

	kprintf("%s/%s/%ld: End ParentOfFH(%s %08lx) = %08lx\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, fh, newLock);

	return newLock;
}

BPTR DupLockFromFH_Debug(BPTR fh,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	BPTR newLock;

	kprintf("%s/%s/%ld: Begin DupLockFromFH(%s %08lx)  Task=%08lx <%s>\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, fh, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name);

	newLock = DupLockFromFH(fh);

	kprintf("%s/%s/%ld: End DupLockFromFH(%s %08lx) = %08lx\n", 
		CallingFile, CallingFunc, CallingLine, CallingArg, fh, newLock);

	return newLock;
}

#endif	/* DEBUG_LOCKS */

// ----------------------------------------------------------


