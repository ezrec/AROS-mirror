// PNGIconDt.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/resident.h>
#include <intuition/classes.h>
#include <intuition/gadgetclass.h>
#include <graphics/gfxbase.h>
#include <dos/dos.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <utility/tagitem.h>
#include <cybergraphx/cybergraphics.h>
#include <scalos/scalosgfx.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/wb.h>
#include <proto/timer.h>

#include <clib/alib_protos.h>

#include <datatypes/iconobject.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <defs.h>
#include <Year.h>

#include "PNGIconDt.h"
#include "PNGIconProto.h"

//-----------------------------------------------------------------------------

struct DefIconName
	{
	ULONG din_IconType;
	CONST_STRPTR din_Filename;
	};

//-----------------------------------------------------------------------------

void *PngMemPool;

Class *PngIconClass;

const ULONG InstanceSize = sizeof(struct InstanceData);

static struct SignalSemaphore PngMemPoolSemaphore;

#ifndef __AROS__
struct ExecBase *SysBase;
struct Library *WorkbenchBase;
struct IntuitionBase *IntuitionBase;
T_UTILITYBASE UtilityBase;
struct Library *IconBase;
struct GfxBase *GfxBase;
struct DosLibrary *DOSBase;
#endif
struct Library *CyberGfxBase;
static struct Library *IconObjectDTBase;
#ifdef TIMESTAMPS
T_TIMERBASE TimerBase;
#endif /* TIMESTAMPS */
#ifdef __amigaos4__
struct Library *NewlibBase;
struct Interface *INewlib;
struct WorkbenchIFace *IWorkbench;
struct ExecIFace *IExec;
struct IntuitionIFace *IIntuition;
struct UtilityIFace *IUtility;
struct IconIFace *IIcon;
struct GraphicsIFace *IGraphics;
struct DOSIFace *IDOS;
struct CyberGfxIFace *ICyberGfx;
#endif /* __amigaos4__ */

#ifdef __GNUC__
extern T_UTILITYBASE __UtilityBase;
#endif /* __GNUC__ */

//----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT PngIconDispatcher(Class *cl, Object *obj, Msg msg);

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops);
static ULONG DtDispose(Class *cl, Object *o, Msg msg);
static ULONG DtGet(Class *cl, Object *o, struct opGet *opg);
static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *opw);
static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *iopw);
static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio);

static CONST_STRPTR GetFileNameForDefaultIcon(ULONG IconType);
static BOOL IsPiDiskObject(const struct DiskObject *icon);
#if !defined(__SASC) &&!defined(__MORPHOS__)
size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !defined(__SASC) &&!defined(__MORPHOS__)  */
static BOOL GetARGBfromSAC(struct ARGBHeader *argbh,
	const struct ScalosBitMapAndColor *sac);
static void FreePngImage(struct ARGBHeader *argbh, BOOL DoFreeIcon);
static void DoUpdateWb(struct InstanceData *inst, CONST_STRPTR SavePath);
static void SetParentAttributes(Class *cl, Object *o);

//----------------------------------------------------------------------------

char ALIGNED libName[] = "pngiconobject.datatype";
char ALIGNED libIdString[] = "$VER: pngiconobject.datatype "
                STR(LIB_VERSION) "." STR(LIB_REVISION)
		" (25 Jan 2008 22:27:14) "
		COMPILER_STRING " ©2003" CURRENTYEAR
                " The Scalos Team, parts of code ©Elena Novaretti <uni-dea@jumpy.it>";

//----------------------------------------------------------------------------

#ifdef __AROS__
AROS_LH0(ULONG, ObtainInfoEngine,
    struct Library *, libBase, 5, PNGIconobject
)
{
	AROS_LIBFUNC_INIT
	return (ULONG) PngIconClass;
	AROS_LIBFUNC_EXIT
}
#else
LIBFUNC(ObtainInfoEngine, libbase, ULONG)
{
	d1(KPrintF("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));
	(void) libbase;
	return (ULONG) PngIconClass;
}
LIBFUNC_END
#endif

//-----------------------------------------------------------------------------

void _XCEXIT(long x)
{
}

//----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT PngIconDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(KPrintF("%s/%s/%ld:  cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));

	switch (msg->MethodID)
		{
	case OM_NEW:
		o = (Object *) DoSuperMethodA(cl, o, msg);
		d1(KPrintF("%s/%s/%ld:  OM_NEW  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		if (o)
			{
			if (!DtNew(cl, o, (struct opSet *) msg))
				{
				DoMethod(o, OM_DISPOSE);
				o = NULL;
				}
			}
		Result = (ULONG) o;
		break;

	case OM_DISPOSE:
		Result = DtDispose(cl, o, msg);
		break;

	case OM_GET:
		Result = DtGet(cl, o, (struct opGet *) msg);
		break;

	case IDTM_Write:
		Result = DtWrite(cl, o, (struct iopWrite *) msg);
		break;

	case IDTM_NewImage:
		Result = DtNewImage(cl, o, (struct iopNewImage *) msg);
		break;

	case IDTM_CloneIconObject:
		Result = DtClone(cl, o, (struct iopCloneIconObject *) msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//-----------------------------------------------------------------------------

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops)
{
	BOOL Success = FALSE;
	struct InstanceData *inst = INST_DATA(cl, o);

	memset(inst, 0, sizeof(struct InstanceData));

	NewList(&inst->id_ToolTypesList);
	inst->id_StackSize = MINSTACKSIZE;
	inst->id_DoNotFreeIcon = FALSE;

	do	{
		struct ExtGadget *gg = (struct ExtGadget *) o;

		d1(KPrintF("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));
		TIMESTAMP_d1();

		if (FindTagItem(IDTA_CloneIconObject, ops->ops_AttrList))
			{
			d1(KPrintF("%s/%s/%ld:  IDTA_CloneIconObject\n", __FILE__, __FUNC__, __LINE__));
			}
		else
			{
			CONST_STRPTR FileName;
			const struct WBArg *wba;
			BPTR IconFh;

			// We don't support user-rendered icons
			if (FindTagItem(IDTA_RenderHook, ops->ops_AttrList))
				break;

			if (FindTagItem(IDTA_DefType, ops->ops_AttrList))
				{
				ULONG IconType;

				IconType = GetTagData(IDTA_DefType, 0, ops->ops_AttrList);
				d1(KPrintF("%s/%s/%ld:  IDTA_DefType=%ld\n", __FILE__, __FUNC__, __LINE__, IconType));
				FileName = GetFileNameForDefaultIcon(IconType);
				}
			else
				{
				FileName = (CONST_STRPTR) GetTagData(DTA_Name, (ULONG)NULL, (struct TagItem *)ops->ops_AttrList);
				}

			wba = (const struct WBArg *) GetTagData(IDTA_IconLocation, (ULONG) NULL, ops->ops_AttrList);

			inst->id_CurrentX = inst->id_CurrentY = NO_ICON_POSITION;

			stccpy(inst->id_FileName, FileName, sizeof(inst->id_FileName));

			if (FindTagItem(AIDTA_Icon, ops->ops_AttrList))
				{
				struct DiskObject *icon = (struct DiskObject *) GetTagData(AIDTA_Icon, (ULONG)NULL, (struct TagItem *)ops->ops_AttrList);
				struct PiIconSpecial *ispc;

				d1(kprintf("%s/%s/%ld:  DiskObject=%08lx\n", __FILE__, __FUNC__, __LINE__, icon));

				if (!IsPiDiskObject(icon))
					// We only support "cloning of icons" for PowerIcons
					break;

				ispc = (struct PiIconSpecial *) icon->do_Gadget.MutualExclude;

				if (!(ispc->Flags & ISF_PNGICON))
					break;

				inst->id_CurrentX = icon->do_CurrentX;
				inst->id_CurrentY = icon->do_CurrentX;
				inst->id_NormalImage.argb_Width = ispc->Width;
				inst->id_NormalImage.argb_Height = ispc->Height;
				inst->id_NormalImage.argb_ImageData = (struct ARGB *) ispc->ImgData;
				inst->id_Type = icon->do_Type;
				inst->id_StackSize = icon->do_StackSize;
				inst->id_ToolTypes = icon->do_ToolTypes;
				inst->id_DefaultTool = icon->do_DefaultTool;
				inst->id_ToolWindow = icon->do_ToolWindow;

				d1(kprintf("%s/%s/%ld:  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_NormalImage.argb_Width, inst->id_NormalImage.argb_Height));
				d1(kprintf("%s/%s/%ld:  ImgData=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_NormalImage.argb_ImageData));
				d1(kprintf("%s/%s/%ld:  ToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_ToolTypes));

				if (icon->do_DrawerData)
					{
					inst->id_DrawerData = *icon->do_DrawerData;
					}

				inst->id_DoNotFreeIcon = TRUE;

				d1(kprintf("%s/%s/%ld:  use existing DiskObject\n", __FILE__, __FUNC__, __LINE__));
				}
			else
				{
				if (NULL == FileName)
					break;

				if ('\0' == *FileName)
					break;

				IconFh = GetTagData(DTA_Handle, 0, ops->ops_AttrList);

				d1(KPrintF("%s/%s/%ld:  FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));
				TIMESTAMP_d1();

				if (!LoadPngIcon(inst, FileName, IconFh, wba))
					break;

				inst->id_Type = GetTagData(IDTA_DefType, inst->id_Type, ops->ops_AttrList);
				d1(KPrintF("%s/%s/%ld:  id_Type=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));

				d1(kprintf("%s/%s/%ld:  LoadPngIcon succeeded\n", __FILE__, __FUNC__, __LINE__));
				TIMESTAMP_d1();
				}

			if (NO_ICON_POSITION == inst->id_CurrentX)
				{
				gg->LeftEdge = gg->TopEdge = (WORD) 0x8000;
				}
			else
				{
				gg->LeftEdge = inst->id_CurrentX;
				gg->TopEdge = inst->id_CurrentY;
				}

			TIMESTAMP_d1();
			SetParentAttributes(cl, o);
			}

		TIMESTAMP_d1();
		Success = TRUE;
		d1(KPrintF("%s/%s/%ld:  o=%08lx  ImgData=%08lx", \
			__FILE__, __FUNC__, __LINE__, o, inst->id_NormalImage.argb_ImageData));
		d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
		} while (0);

	d1(KPrintF("%s/%s/%ld:  o=%08lx  Success=%ld\n", __FILE__, __FUNC__, __LINE__, o, Success));
	TIMESTAMP_d1();

	return Success;
}

//----------------------------------------------------------------------------------------

static ULONG DtGet(Class *cl, Object *o, struct opGet *opg)
{
	ULONG result = 1;

	switch (opg->opg_AttrID)
		{
	case IDTA_IconType:
		*opg->opg_Storage = ioICONTYPE_PngIcon;
		break;

	case IDTA_NumberOfColorsSupported:
		*opg->opg_Storage = 1 + 0x00ffffff;
		break;

	default:
		result = DoSuperMethodA(cl, o, (Msg) opg);
		break;
		}

	return result;
}

//----------------------------------------------------------------------------------------

static ULONG DtDispose(Class *cl, Object *o, Msg msg)
{
	struct TTNode *ttnode;
	struct InstanceData *inst = INST_DATA(cl, o);

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	if (inst->id_DefaultTool && !inst->id_DoNotFreeIcon)
		{
		d1(kprintf("%s/%s/%ld:  id_DefaultTool=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_DefaultTool));
		MyFreeVecPooled(inst->id_DefaultTool);
		inst->id_DefaultTool = NULL;
		}
	if (inst->id_ToolWindow && !inst->id_DoNotFreeIcon)
		{
		d1(KPrintF("%s/%s/%ld:  id_ToolWindow=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_ToolWindow));
		MyFreeVecPooled(inst->id_ToolWindow);
		inst->id_ToolWindow = NULL;
		}
	if (inst->id_ToolTypes && !inst->id_DoNotFreeIcon)
		{
		d1(kprintf("%s/%s/%ld:  id_ToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_ToolTypes));
		MyFreeVecPooled(inst->id_ToolTypes);
		inst->id_ToolTypes = NULL;
		}

	while (ttnode = (struct TTNode *) RemTail(&inst->id_ToolTypesList))
		{
		d1(kprintf("%s/%s/%ld:  ttnode=%08lx\n", __FILE__, __FUNC__, __LINE__, ttnode));
		MyFreeVecPooled(ttnode);
		}

	FreePngImage(&inst->id_NormalImage, !inst->id_DoNotFreeIcon);
	FreePngImage(&inst->id_SelectedImage, !inst->id_DoNotFreeIcon);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------------------

static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *opw)
{
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct InstanceData *inst = INST_DATA(cl, o);
	STRPTR origDefaultTool = inst->id_DefaultTool;
	STRPTR origToolWindow = inst->id_ToolWindow;
	STRPTR *origTooltypes = inst->id_ToolTypes;
	struct IBox *WindowRect = NULL;
	struct ARGBHeader *NrmARGBImage = &inst->id_NormalImage;
	struct ARGBHeader *SelARGBImage = &inst->id_SelectedImage;
	ULONG ViewModes;
	ULONG NeedUpdateWB;
	LONG Result = RETURN_OK;

	d1(KPrintF("%s/%s/%ld:  id_Type=%lu\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));
	d1(KPrintF("%s/%s/%ld:  ImgData=%08lx  WorkBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_NormalImage.argb_ImageData));

	GetAttr(IDTA_Type, 		o, &inst->id_Type);
	GetAttr(IDTA_ViewModes, 	o, &ViewModes);
	GetAttr(IDTA_Flags, 		o, &inst->id_DrawerData.dd_Flags);
	GetAttr(IDTA_WinCurrentX, 	o, (ULONG *) &inst->id_DrawerData.dd_CurrentX);
	GetAttr(IDTA_WinCurrentY, 	o, (ULONG *) &inst->id_DrawerData.dd_CurrentY);
	GetAttr(IDTA_WindowRect, 	o, (APTR) &WindowRect);
	GetAttr(IDTA_Stacksize, 	o, &inst->id_StackSize);
	GetAttr(IDTA_ToolTypes, 	o, (ULONG *) &inst->id_ToolTypes);
	GetAttr(IDTA_DefaultTool, 	o, (ULONG *) &inst->id_DefaultTool);
	GetAttr(IDTA_ToolWindow,	o, (ULONG *) &inst->id_ToolWindow);
	GetAttr(IDTA_ARGBImageData,	o, (APTR) &NrmARGBImage);
	GetAttr(IDTA_SelARGBImageData,	o, (APTR) &SelARGBImage);

	inst->id_DrawerData.dd_ViewModes = ViewModes;

	d1(KPrintF("%s/%s/%ld:  id_Type=%lu\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));
	d1(KPrintF("%s/%s/%ld:  NrmImgData=%08lx  SelImgData=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, NrmARGBImage->argb_ImageData, SelARGBImage->argb_ImageData));

	NeedUpdateWB = GetTagData(ICONA_NotifyWorkbench, FALSE, opw->iopw_Tags);

	if (WindowRect)
		{
		inst->id_DrawerData.dd_NewWindow.LeftEdge = WindowRect->Left;
		inst->id_DrawerData.dd_NewWindow.TopEdge  = WindowRect->Top;
		inst->id_DrawerData.dd_NewWindow.Width    = WindowRect->Width;
		inst->id_DrawerData.dd_NewWindow.Height   = WindowRect->Height;
		}

	if (GetTagData(ICONA_NoPosition, 0, opw->iopw_Tags))
		{
		inst->id_CurrentX = inst->id_CurrentY = NO_ICON_POSITION;
		}
	else
		{
		if ((WORD) 0x8000 == gg->LeftEdge)
			{
			inst->id_CurrentX = inst->id_CurrentY = NO_ICON_POSITION;
			}
		else
			{
			inst->id_CurrentX = gg->LeftEdge;
			inst->id_CurrentY = gg->TopEdge;
			}
		}

	if (!SavePngIcon(inst, opw->iopw_Path, NrmARGBImage, SelARGBImage))
		Result = IoErr();

	d1(KPrintF("%s/%s/%ld:  ImgData=%08lx  WorkBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_NormalImage.argb_ImageData));

	// Restore DefaultTool & ToolTypes
	inst->id_ToolWindow = origToolWindow;
	inst->id_DefaultTool = origDefaultTool;
	inst->id_ToolTypes = origTooltypes;

	if ((RETURN_OK == Result) && NeedUpdateWB)
		DoUpdateWb(inst, opw->iopw_Path);

	return (ULONG) Result;
}

//-----------------------------------------------------------------------------

static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *ioni)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ARGBHeader argbh;

	d1(KPrintF("%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	do	{
		memset(&argbh, 0, sizeof(argbh));

		if (GetARGBfromSAC(&argbh, ioni->ioni_NormalImage))
			{
			if (inst->id_NormalImage.argb_ImageData && !inst->id_DoNotFreeIcon)
				{
				d1(kprintf("%s/%s/%ld:  id_NormalImage.argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_NormalImage.argb_ImageData));
				MyFreeVecPooled(inst->id_NormalImage.argb_ImageData);
				inst->id_NormalImage.argb_ImageData = NULL;
				}
			inst->id_NormalImage = argbh;
			}
		if (GetARGBfromSAC(&argbh, ioni->ioni_SelectedImage))
			{
			if (inst->id_SelectedImage.argb_ImageData && !inst->id_DoNotFreeIcon)
				{
				d1(kprintf("%s/%s/%ld:  id_NormalImage.argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_NormalImage.argb_ImageData));
				MyFreeVecPooled(inst->id_SelectedImage.argb_ImageData);
				inst->id_SelectedImage.argb_ImageData = NULL;
				}
			inst->id_SelectedImage = argbh;
			}
		} while (0);

	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	Object *oClone;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%ld:  START o=%08lx  inst=%08lx  id_FileName=<%s>\n", __FUNC__, __LINE__, o, inst, inst->id_FileName));

	do	{
		struct InstanceData *instClone;


		oClone = (Object *) NewObject(cl, NULL,
			DTA_Name, inst->id_FileName,    // REQUIRED, otherwise NewObject() will fail
			IDTA_CloneIconObject, o,
			TAG_MORE, iocio->iocio_TagList,
			TAG_END);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  oClone=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst, oClone));
		if (NULL == oClone)
			break;

		instClone = INST_DATA(cl, oClone);

		*instClone = *inst;

		if (inst->id_DefaultTool && !inst->id_DoNotFreeIcon)
			{
			// Clone Default Tool
			instClone->id_DefaultTool = MyAllocVecPooled(1 + strlen(inst->id_DefaultTool));
			d1(KPrintF("%s/%s/%ld:  id_DefaultTool=%08lx\n", __FILE__, __FUNC__, __LINE__, instClone->id_DefaultTool));
			if (NULL == instClone->id_DefaultTool)
				break;

			strcpy(instClone->id_DefaultTool, inst->id_DefaultTool);
			}
		if (inst->id_ToolWindow && !inst->id_DoNotFreeIcon)
			{
			// Clone Tool Window
			instClone->id_ToolWindow = MyAllocVecPooled(1 + strlen(inst->id_ToolWindow));
			d1(KPrintF("%s/%s/%ld:  id_ToolWindow=%08lx\n", __FILE__, __FUNC__, __LINE__, instClone->id_ToolWindow));
			if (NULL == instClone->id_ToolWindow)
				break;

			strcpy(instClone->id_ToolWindow, inst->id_ToolWindow);
			}

		NewList(&instClone->id_ToolTypesList);

		// Clone Tooltypes array

		instClone->id_ToolTypes = NULL;
		if (instClone->id_ToolTypesLength)
			{
			instClone->id_ToolTypes = MyAllocVecPooled(instClone->id_ToolTypesLength);
			if (NULL == instClone->id_ToolTypes)
				break;

			memcpy(instClone->id_ToolTypes, inst->id_ToolTypes, instClone->id_ToolTypesLength);
			}

		d1(KPrintF("%s/%s/%ld:  id_DoNotFreeIcon=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DoNotFreeIcon));

		if (!inst->id_DoNotFreeIcon)
			{
			struct ARGBHeader *NrmARGBImage = &inst->id_NormalImage;
			struct ARGBHeader *SelARGBImage = &inst->id_SelectedImage;
			size_t ImgSize;

			d1(KPrintF("%s/%s/%ld:  id_NormalImage=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, NrmARGBImage, NrmARGBImage->argb_Width, NrmARGBImage->argb_Height));
			d1(KPrintF("%s/%s/%ld:  id_SelectedImage=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, NrmARGBImage, SelARGBImage->argb_Width, SelARGBImage->argb_Height));

			GetAttr(IDTA_ARGBImageData,     o, (APTR) &NrmARGBImage);
			GetAttr(IDTA_SelARGBImageData,  o, (APTR) &SelARGBImage);

			d1(KPrintF("%s/%s/%ld:  NrmARGBImage=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, NrmARGBImage, NrmARGBImage->argb_Width, NrmARGBImage->argb_Height));
			d1(KPrintF("%s/%s/%ld:  SelARGBImage=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, NrmARGBImage, SelARGBImage->argb_Width, SelARGBImage->argb_Height));

			ImgSize = sizeof(struct ARGB) * NrmARGBImage->argb_Width * NrmARGBImage->argb_Height;

			// Clone Normal image
			instClone->id_NormalImage = *NrmARGBImage;

			instClone->id_NormalImage.argb_ImageData = MyAllocVecPooled(ImgSize);
			if (NULL == instClone->id_NormalImage.argb_ImageData)
				break;

			memcpy(instClone->id_NormalImage.argb_ImageData,
				NrmARGBImage->argb_ImageData, ImgSize);

			if (SelARGBImage && SelARGBImage->argb_ImageData)
				{
				// Clone Selected Image
				ImgSize = sizeof(struct ARGB) * SelARGBImage->argb_Width * SelARGBImage->argb_Height;

				instClone->id_SelectedImage = *SelARGBImage;

				instClone->id_SelectedImage.argb_ImageData = MyAllocVecPooled(ImgSize);
				if (NULL == instClone->id_SelectedImage.argb_ImageData)
					break;

				memcpy(instClone->id_SelectedImage.argb_ImageData,
					SelARGBImage->argb_ImageData, ImgSize);
				}

			}

		SetAttrsA(oClone, iocio->iocio_TagList);

		TIMESTAMP_d1();
		SetParentAttributes(cl, oClone);

		Success = TRUE;
		} while (0);

	if (oClone && !Success)
		{
		DoMethod(oClone, OM_DISPOSE);
		oClone = NULL;
		}

	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx  oClone=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst, oClone));

	return (ULONG) oClone;
}

//-----------------------------------------------------------------------------

APTR MyAllocVecPooled(size_t Size)
{
	APTR ptr;

	if (PngMemPool)
		{
		ObtainSemaphore(&PngMemPoolSemaphore);
		ptr = AllocPooled(PngMemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&PngMemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, PngMemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, PngMemPool, Size));

	return NULL;
}


void MyFreeVecPooled(APTR mem)
{
	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, PngMemPool, mem));
	if (PngMemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		ObtainSemaphore(&PngMemPoolSemaphore);
		FreePooled(PngMemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&PngMemPoolSemaphore);
		}
}

//-----------------------------------------------------------------------------

static CONST_STRPTR GetFileNameForDefaultIcon(ULONG IconType)
{
	static const struct DefIconName DefIconNames[] =
		{
#if defined(__AROS__)
		{ WBDISK,	"ENVARC:sys/def_disk" 		},
		{ WBDRAWER,	"ENVARC:sys/def_drawer" 	},
		{ WBTOOL,	"ENVARC:sys/def_tool" 		},
		{ WBPROJECT,	"ENVARC:sys/def_project" 	},
		{ WBGARBAGE,	"ENVARC:sys/def_trashcan" 	},
		{ WBDEVICE,	"ENVARC:sys/def_device" 	},
		{ WBKICK,	"ENVARC:sys/def_kick" 		},
		{ WBAPPICON,	"ENVARC:sys/def_appicon"	}
#else
		{ WBDISK,	"ENV:sys/def_disk" 	},
		{ WBDRAWER,	"ENV:sys/def_drawer" 	},
		{ WBTOOL,	"ENV:sys/def_tool" 	},
		{ WBPROJECT,	"ENV:sys/def_project" 	},
		{ WBGARBAGE,	"ENV:sys/def_trashcan" 	},
		{ WBDEVICE,	"ENV:sys/def_device" 	},
		{ WBKICK,	"ENV:sys/def_kick" 	},
		{ WBAPPICON,	"ENV:sys/def_appicon"	}
#endif
		};

	ULONG n;

	for (n=0; n<Sizeof(DefIconNames); n++)
		{
		if (IconType == DefIconNames[n].din_IconType)
			return DefIconNames[n].din_Filename;
		}

	// icontype not found
	return NULL;
}

//-----------------------------------------------------------------------------

static BOOL IsPiDiskObject(const struct DiskObject *icon)
{
	const struct PiIconSpecial *ispc;

	return (BOOL) ((icon->do_Gadget.NextGadget == (struct Gadget *) icon) 
		&& (ispc = (const struct PiIconSpecial *) icon->do_Gadget.MutualExclude) 
		&& (ispc->Myself == icon));
}

//-----------------------------------------------------------------------------

// return 0 if error occurred
ULONG InitDatatype(struct PngIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	dtLib->nib_Initialized = FALSE;

	return 1;
}

// return 0 if error occurred
ULONG OpenDatatype(struct PngIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (!dtLib->nib_Initialized)
		{
		ULONG WbScreenDepth;
		struct Screen *WbScreen;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		dtLib->nib_Initialized = TRUE;

		InitSemaphore(&PngMemPoolSemaphore);

		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
#ifdef __amigaos4__
		if (IntuitionBase != NULL)
			{
			IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
			if (IIntuition == NULL)
				{
				CloseLibrary((struct Library *)IntuitionBase);
				IntuitionBase = NULL;
				}
			}
#endif /* __amigaos4__ */
		if (NULL == IntuitionBase)
			return 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
#ifdef __amigaos4__
		if (UtilityBase != NULL)
			{
			IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
			if (IUtility == NULL)
				{
				CloseLibrary((struct Library *)UtilityBase);
				UtilityBase = NULL;
				}
			}
#endif /* __amigaos4__ */
		if (NULL == UtilityBase)
			return 0;

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
		__UtilityBase = UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) */

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		GfxBase = (struct GfxBase *) OpenLibrary(GRAPHICSNAME, 39);
#ifdef __amigaos4__
		if (GfxBase != NULL)
			{
			IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
			if (IGraphics == NULL)
				{
				CloseLibrary((struct Library *)GfxBase);
				GfxBase = NULL;
				}
			}
#endif /* __amigaos4__ */
		if (NULL == GfxBase)
			return 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		WorkbenchBase = OpenLibrary("workbench.library", 39);
		d1(KPrintF("%s/%s/%ld: WorkbenchBase=%08lx\n", __FILE__, __FUNC__, __LINE__, WorkbenchBase));
		if (NULL == WorkbenchBase)
			return 0;
#ifdef __amigaos4__
		IWorkbench = (struct WorkbenchIFace *)GetInterface((struct Library *)WorkbenchBase, "main", 1, NULL);
		if (NULL == IWorkbench)
			return 0;
#endif /* __amigaos4__ */

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		DOSBase = (struct DosLibrary *) OpenLibrary(DOSNAME, 39);
#ifdef __amigaos4__
		if (DOSBase != NULL)
			{
			IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
			if (IDOS == NULL)
				{
				CloseLibrary((struct Library *)DOSBase);
				DOSBase = NULL;
				}
			}
#endif /* __amigaos4__ */
		if (NULL == DOSBase)
			return 0;

		IconBase = OpenLibrary(ICONNAME, 39);
#ifdef __amigaos4__
		if (IconBase != NULL)
			{
			IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
			if (IIcon == NULL)
				{
				CloseLibrary((struct Library *)IconBase);
				IconBase = NULL;
				}
			}
#endif /* __amigaos4__ */
		if (NULL == IconBase)
			return 0;

		CyberGfxBase = OpenLibrary(CYBERGFXNAME, 0);
#ifdef __amigaos4__
		if (CyberGfxBase != NULL)
			{
			ICyberGfx = (struct CyberGfxIFace *)GetInterface((struct Library *)CyberGfxBase, "main", 1, NULL);
			if (ICyberGfx == NULL)
				{
				CloseLibrary((struct Library *)CyberGfxBase);
				CyberGfxBase = NULL;
				}
			}
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			return 0;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			return 0;
#endif /* __amigaos4__ */
		if (NULL == CyberGfxBase)
			return 0;

		WbScreen = LockPubScreen("Workbench");
		d1(kprintf("%s/%s/%ld: WbScreen=%08lx\n", __FILE__, __FUNC__, __LINE__, WbScreen));
		if (NULL == WbScreen)
			return 0;

		// We REQUIRE a high-color or full-color screen (depth > 8) !!
		WbScreenDepth = GetBitMapAttr(WbScreen->RastPort.BitMap, BMA_DEPTH);
		UnlockPubScreen(NULL, WbScreen);
		d1(kprintf("%s/%s/%ld: WbScreenDepth=%lu\n", __FILE__, __FUNC__, __LINE__, WbScreenDepth));
		if (WbScreenDepth <= 8)
			return 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// we explicitely need to open our parent datatype - otherwise MakeClass() will fail!
		IconObjectDTBase = OpenLibrary("datatypes/iconobject.datatype", 39);
		if (NULL == IconObjectDTBase)
			return 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

#ifdef TIMESTAMPS
		{
		struct timerequest *iorequest;

		iorequest = (struct timerequest *) CreateIORequest(CreateMsgPort(), sizeof(struct timerequest));
		OpenDevice("timer.device", UNIT_VBLANK, &iorequest->tr_node, 0);
		TimerBase = (T_TIMERBASE) iorequest->tr_node.io_Device;
		}
#endif /* TIMESTAMPS */

#if !defined(__amigaos4__) && !defined(__AROS__)
		if (_STI_240_InitMemFunctions())
			return 0;
#endif /* __amigaos4__ */

		PngMemPool = CreatePool(MEMPOOL_MEMFLAGS, MEMPOOL_PUDDLESIZE, MEMPOOL_THRESHSIZE);
		if (NULL == PngMemPool)
			return 0;

		PngIconClass = dtLib->nib_ClassLibrary.cl_Class = MakeClass(libName,
			"iconobject.datatype", NULL, sizeof(struct InstanceData), 0);
		d1(kprintf("%s/%s/%ld:  PngIconClass=%08lx\n", __FILE__, __FUNC__, __LINE__, PngIconClass));
		if (NULL == PngIconClass)
			return 0;

		SETHOOKFUNC(PngIconClass->cl_Dispatcher, PngIconDispatcher);
		PngIconClass->cl_Dispatcher.h_Data = NULL;

		// Make class available for the public
		AddClass(PngIconClass);
#ifdef __AROS__
		dtLib->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = 12;
#endif
		}

	d1(kprintf("%s/%s/%ld:  Open Success!\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}


ULONG CloseDatatype(struct PngIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt < 1)
		{
		if (PngIconClass)
			{
			RemoveClass(PngIconClass);
			FreeClass(PngIconClass);
			PngIconClass = dtLib->nib_ClassLibrary.cl_Class = NULL;
			}

#if !defined(__amigaos4__) && !defined(__AROS__)
		_STD_240_TerminateMemFunctions();
#endif /* __amigaos4__ */

		if (NULL != PngMemPool)
			{
			DeletePool(PngMemPool);
			PngMemPool = NULL;
			}

#ifdef __amigaos4__
		if (INewlib)
			{
			DropInterface(INewlib);
			INewlib = NULL;
			}
		if (NewlibBase)
			{
			CloseLibrary(NewlibBase);
			NewlibBase = NULL;
			}
#endif
		if (NULL != CyberGfxBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)ICyberGfx);
#endif /* __amigaos4__ */
			CloseLibrary(CyberGfxBase);
			CyberGfxBase = NULL;
			}
		if (NULL != IconObjectDTBase)
			{
			CloseLibrary(IconObjectDTBase);
			IconObjectDTBase = NULL;
			}
#ifdef __amigaos4__
		if (NULL != IWorkbench)
			{
			DropInterface((struct Interface *)IWorkbench);
			IWorkbench = NULL;
			}
#endif /* __amigaos4__ */
		if (NULL != WorkbenchBase)
			{
			CloseLibrary(WorkbenchBase);
			WorkbenchBase = NULL;
			}
		if (NULL != IntuitionBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IIntuition);
#endif /* __amigaos4__ */
			CloseLibrary((struct Library *) IntuitionBase);
			IntuitionBase = NULL;
			}
		if (NULL != UtilityBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IUtility);
#endif /* __amigaos4__ */
			CloseLibrary((struct Library *) UtilityBase);
			UtilityBase = NULL;
			}
		if (NULL != GfxBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IGraphics);
#endif /* __amigaos4__ */
			CloseLibrary((struct Library *) GfxBase);
			GfxBase = NULL;
			}
		if (NULL != IconBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IIcon);
#endif /* __amigaos4__ */
			CloseLibrary(IconBase);
			IconBase = NULL;
			}
		if (NULL != DOSBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IDOS);
#endif /* __amigaos4__ */
			CloseLibrary((struct Library *) DOSBase);
			DOSBase = NULL;
			}
		}
	return 1;
}

//-----------------------------------------------------------------------------

#if !defined(__SASC)
// Replacement for SAS/C library functions

#if !defined(__MORPHOS__)
// Replacement for SAS/C library functions

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
#endif /*__MORPHOS__*/

#ifndef __amigaos4__
void exit(int x)
{
	(void) x;
	while (1)
		;
}
#endif /* __amigaos4__ */

#endif /* !defined(__SASC) */
//-----------------------------------------------------------------------------

static BOOL GetARGBfromSAC(struct ARGBHeader *argbh,
	const struct ScalosBitMapAndColor *sac)
{
	UBYTE *ImageLine = NULL;
	struct BitMap *TempBM;
	BOOL Success = FALSE;

	do	{
		struct ARGB *argb;
		ULONG y;
		struct RastPort rp;
		struct RastPort TempRp;
		size_t ImgSize;

		InitRastPort(&rp);
		InitRastPort(&TempRp);

		rp.BitMap = sac->sac_BitMap;
		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: rp.BitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, rp.BitMap));

		// setup temp. RastPort for use by WritePixelLine8()
		TempRp.Layer = NULL;
		TempRp.BitMap = TempBM = AllocBitMap(TEMPRP_WIDTH(sac->sac_Width), 1, 8, 0, NULL);
		if (NULL == TempBM)
			break;

		ImageLine = MyAllocVecPooled(PIXELARRAY8_BUFFERSIZE(sac->sac_Width, 1));
		if (NULL == ImageLine)
			break;

		argbh->argb_Width = sac->sac_Width;
		argbh->argb_Height = sac->sac_Height;

		ImgSize = argbh->argb_Width * argbh->argb_Height;

		argbh->argb_ImageData = MyAllocVecPooled(ImgSize * sizeof(struct ARGB));
		if (NULL == argbh->argb_ImageData)
			break;

		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: sac_TransparentColor=%ld\n", __FILE__, __FUNC__, __LINE__, sac->sac_TransparentColor));

		// nothing can go wrong from here on
		Success = TRUE;

		d1(KPrintF("%s/%s/%ld: NumColors=%lu\n", __FILE__, __FUNC__, __LINE__, sac->sac_NumColors));

		for (y = 0, argb = argbh->argb_ImageData; y < argbh->argb_Height; y++)
			{
			ULONG x;
			const UBYTE *ImgLinePtr;

			d1(KPrintF("%s/%s/%ld: y=%ld\n", __FILE__, __FUNC__, __LINE__, y));

			ReadPixelLine8(&rp, 0, y, sac->sac_Width, ImageLine, &TempRp);

			ImgLinePtr = ImageLine;
			for (x = 0; x < argbh->argb_Width; x++, argb++, ImgLinePtr++)
				{
				ULONG *PalettePtr = sac->sac_ColorTable + 3 * *ImgLinePtr;

				argb->Red   = PalettePtr[0] >> 24;
				argb->Green = PalettePtr[1] >> 24;
				argb->Blue  = PalettePtr[2] >> 24;

				if (SAC_TRANSPARENT_NONE == sac->sac_TransparentColor)
					argb->Alpha = 0xff;
				else
					argb->Alpha = (*ImgLinePtr == sac->sac_TransparentColor) ? 0 : 0xff;
				}
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	if (ImageLine)
		MyFreeVecPooled(ImageLine);
	if (TempBM)
		FreeBitMap(TempBM);

	return Success;
}

//-----------------------------------------------------------------------------

static void FreePngImage(struct ARGBHeader *argbh, BOOL DoFreeIcon)
{
	if (argbh->argb_ImageData && DoFreeIcon)
		{
		d1(KPrintF("%s/%s/%ld:  argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, argbh->argb_ImageData));
		MyFreeVecPooled(argbh->argb_ImageData);
		argbh->argb_ImageData = NULL;
		}
}

//-----------------------------------------------------------------------------

static void DoUpdateWb(struct InstanceData *inst, CONST_STRPTR SavePath)
{
	BPTR fLock;
	BPTR pLock = (BPTR)NULL;

	do	{
		CONST_STRPTR fName;

		fLock = Lock(SavePath, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			break;

		pLock = ParentDir(fLock);
		if ((BPTR)NULL == pLock)
			break;

		fName = FilePart(SavePath);

		UpdateWorkbench(fName, pLock, UPDATEWB_ObjectAdded);
		} while (0);

	if (pLock)
		UnLock(pLock);
	if (fLock)
		UnLock(fLock);
}

//-----------------------------------------------------------------------------

static void SetParentAttributes(Class *cl, Object *o)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	SetAttrs(o,
		GA_Width, inst->id_NormalImage.argb_Width,
		GA_Height, inst->id_NormalImage.argb_Height,
		TAG_END);

	d1(KPrintF("%s/%s/%ld:  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__,\
		inst->id_NormalImage.argb_Width, inst->id_NormalImage.argb_Height));
	d1(KPrintF("%s/%s/%ld:  id_Type=%lu\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));

	SetAttrs(o,
		IDTA_CopyARGBImageData,	FALSE,
		IDTA_ARGBImageData,	(ULONG) &inst->id_NormalImage,
		IDTA_ToolTypes,		(ULONG) inst->id_ToolTypes,
		IDTA_Stacksize,		inst->id_StackSize,
		IDTA_DefaultTool,	(ULONG) inst->id_DefaultTool,
		IDTA_ToolWindow,	(ULONG) inst->id_ToolWindow,
		IDTA_Type, 		inst->id_Type,
		TAG_END);

	if (inst->id_SelectedImage.argb_ImageData)
		{
		SetAttrs(o,
			IDTA_CopySelARGBImageData, FALSE,
			IDTA_SelARGBImageData,	   (ULONG) &inst->id_SelectedImage,
			TAG_END);
		}

	switch (inst->id_Type)
		{
	case WBDRAWER:
	case WBDISK:
	case WBGARBAGE:
		d1(KPrintF("%s/%s/%ld:  dd_CurrentX=%ld  dd_CurrentY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_CurrentX, \
                        inst->id_DrawerData.dd_CurrentY));
		d1(KPrintF("%s/%s/%ld:  dd_Flags=%08lx  dd_ViewModes=%08lx\n", __FILE__, \
			__FUNC__, __LINE__, inst->id_DrawerData.dd_Flags, inst->id_DrawerData.dd_ViewModes));

		SetAttrs(o,
			IDTA_WindowRect,	(ULONG) &inst->id_DrawerData.dd_NewWindow,
			IDTA_WinCurrentX,	inst->id_DrawerData.dd_CurrentX,
			IDTA_WinCurrentY,	inst->id_DrawerData.dd_CurrentY,
			IDTA_Flags, 		inst->id_DrawerData.dd_Flags,
			IDTA_ViewModes,		inst->id_DrawerData.dd_ViewModes,
			TAG_END);
		break;
	default:
		break;
		}
}

//-----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2INITLIB(InitDatatype, 0);
ADD2EXPUNGELIB(CloseDatatype, 0);
ADD2OPENLIB(OpenDatatype, 0);

#endif
//-----------------------------------------------------------------------------
