// DtImageClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/gfxmacros.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/newmouse.h>
#include <utility/hooks.h>
#include <libraries/gadtools.h>
#include <datatypes/pictureclass.h>
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
#include <proto/cybergraphics.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "DtImageClass.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

struct DtImageClassInst
	{
	struct DatatypesImage *bmicl_DtImage;
	struct DatatypesImage *bmicl_SelDtImage;
	struct DatatypesImage *bmicl_DisabledDtImage;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT DtImageClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG DtImage_New(Class *cl, Object *o, Msg msg);
static ULONG DtImage_Dispose(Class *cl, Object *o, Msg msg);
static ULONG DtImage_Draw(Class *cl, Object *o, Msg msg);
static ULONG DtImage_Get(Class *cl, Object *o, Msg msg);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

Class *initDtImageClass(void)
{
	Class *DtImageClass;

	DtImageClass = MakeClass( NULL,
			(STRPTR) IMAGECLASS,
			NULL,
			sizeof(struct DtImageClassInst),
			0 );

	if (DtImageClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(DtImageClass->cl_Dispatcher, DtImageClassDispatcher);
		}

	return DtImageClass;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT DtImageClassDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = DtImage_New(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = DtImage_Dispose(cl, o, msg);
		break;
	case IM_DRAW:
		Result = DtImage_Draw(cl, o, msg);
		break;
	case OM_GET:
		Result = DtImage_Get(cl, o, msg);
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG DtImage_New(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct DtImageClassInst *inst;
	CONST_STRPTR ImageName;
	ULONG Width, Height;

	o = (Object *) DoSuperMethodA(cl, o, msg);
	d1(KPrintF("%s/%s/%ld: o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
	if (NULL == o)
		return 0;

	inst = INST_DATA(cl, o);

	memset(inst, 0, sizeof(struct DtImageClassInst));

	ImageName = (CONST_STRPTR) GetTagData(DTIMG_ImageName, (ULONG) "", ops->ops_AttrList);
	d1(KPrintF("%s/%s/%ld: o=%08lx  ImageName=<%s>\n", __FILE__, __FUNC__, __LINE__, o, ImageName));

	inst->bmicl_DtImage = CreateDatatypesImage(ImageName, 0);
	d1(KPrintF("%s/%s/%ld: bmicl_DtImage=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->bmicl_DtImage));
	if (NULL == inst->bmicl_DtImage)
		{
		DoMethod(o, OM_DISPOSE);
		return 0;
		}

	// Selected image is optional
	ImageName = (CONST_STRPTR) GetTagData(DTIMG_SelImageName, (ULONG) "", ops->ops_AttrList);
	d1(KPrintF("%s/%s/%ld: o=%08lx  SelImageName=<%s>\n", __FILE__, __FUNC__, __LINE__, o, ImageName));

	if (ImageName && strlen(ImageName) > 0)
		inst->bmicl_SelDtImage = CreateDatatypesImage(ImageName, 0);
	d1(KPrintF("%s/%s/%ld: bmicl_SelDtImage=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->bmicl_SelDtImage));

	// Disabled image is optional
	ImageName = (CONST_STRPTR) GetTagData(DTIMG_DisabledImageName, (ULONG) "", ops->ops_AttrList);
	d1(KPrintF("%s/%s/%ld: o=%08lx  DisabledImageName=<%s>\n", __FILE__, __FUNC__, __LINE__, o, ImageName));

	if (ImageName && strlen(ImageName) > 0)
		inst->bmicl_DisabledDtImage = CreateDatatypesImage(ImageName, 0);
	d1(KPrintF("%s/%s/%ld: bmicl_DisabledDtImage=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->bmicl_DisabledDtImage));


	
        // set dimensions in superclass
	Width = inst->bmicl_DtImage->dti_BitMapHeader->bmh_Width;
	Height = inst->bmicl_DtImage->dti_BitMapHeader->bmh_Height;

	if (inst->bmicl_SelDtImage)
		{
		// use largest dimensions of normal and selected images
		if (inst->bmicl_SelDtImage->dti_BitMapHeader->bmh_Width > Width)
			Width = inst->bmicl_SelDtImage->dti_BitMapHeader->bmh_Width;
		if (inst->bmicl_SelDtImage->dti_BitMapHeader->bmh_Height > Height)
			Height = inst->bmicl_SelDtImage->dti_BitMapHeader->bmh_Height;
		}

	SetAttrs(o,
		IA_Width, Width,
		IA_Height, Height,
		TAG_END);

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG DtImage_Dispose(Class *cl, Object *o, Msg msg)
{
	struct DtImageClassInst *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld: o=%08lx  bmicl_DtImage=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->bmicl_DtImage));

	DisposeDatatypesImage(&inst->bmicl_DtImage);
	DisposeDatatypesImage(&inst->bmicl_SelDtImage);
	DisposeDatatypesImage(&inst->bmicl_DisabledDtImage);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG DtImage_Draw(Class *cl, Object *o, Msg msg)
{
	struct impDraw *imp = (struct impDraw *) msg;
	struct DtImageClassInst *inst = INST_DATA(cl, o);
	struct Image *img = (struct Image *) o;
	struct DatatypesImage *dtImg;

	d1(KPrintF("%s/%s/%ld: o=%08lx  Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, o, img->LeftEdge, img->TopEdge));
	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld  imp_State=%ld\n", __FILE__, __FUNC__, __LINE__, img->Width, img->Height, imp->imp_State));
	d1(KPrintF("%s/%s/%ld: bmicl_SelDtImage=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->bmicl_SelDtImage));

	if ((IDS_SELECTED == imp->imp_State) && inst->bmicl_SelDtImage)
		{
		dtImg = inst->bmicl_SelDtImage;
		}
	else if ((IDS_DISABLED == imp->imp_State) && inst->bmicl_DisabledDtImage)
		{
		dtImg = inst->bmicl_DisabledDtImage;
		}
	else
		{
		dtImg = inst->bmicl_DtImage;
		}

	if (dtImg)
		{
		LONG x, y;

		x = img->LeftEdge + imp->imp_Offset.X;
		y = img->TopEdge  + imp->imp_Offset.Y;

		// if normal and selected images have different dimensions, center image in bounding rectangle
		x += (img->Width - dtImg->dti_BitMapHeader->bmh_Width) / 2;
		y += (img->Height - dtImg->dti_BitMapHeader->bmh_Height) / 2;

		d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

		DtImageDraw(dtImg,
			imp->imp_RPort,
			x,
			y,
			dtImg->dti_BitMapHeader->bmh_Width,
			dtImg->dti_BitMapHeader->bmh_Height
			);
		}

	if ((IDS_DISABLED == imp->imp_State) && NULL == inst->bmicl_DisabledDtImage)
		{
		static UWORD myPattern[] =
			{ 0x8888, 0x2222 };
		LONG x, y;

		x = img->LeftEdge + imp->imp_Offset.X;
		y = img->TopEdge  + imp->imp_Offset.Y;

		// if normal and selected images have different dimensions, center image in bounding rectangle
		x += (img->Width - dtImg->dti_BitMapHeader->bmh_Width) / 2;
		y += (img->Height - dtImg->dti_BitMapHeader->bmh_Height) / 2;

		SetABPenDrMd(imp->imp_RPort, 1, 0, JAM1);
		SetAfPt(imp->imp_RPort, myPattern, 1);

		RectFill(imp->imp_RPort, x, y,
			x + img->Width - 1,
			y + img->Height - 1);
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG DtImage_Get(Class *cl, Object *o, Msg msg)
{
	struct opGet *opg = (struct opGet *) msg;
	struct DtImageClassInst *inst = INST_DATA(cl, o);
	ULONG Result = 1;

	if (NULL == opg->opg_Storage)
		return 0;

	switch (opg->opg_AttrID)
		{
	case IA_SupportsDisable:
		*(opg->opg_Storage) = (ULONG) (NULL != inst->bmicl_DisabledDtImage);
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------
