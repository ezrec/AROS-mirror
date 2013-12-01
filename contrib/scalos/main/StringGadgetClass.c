// StringGadgetClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <graphics/rpattr.h>
#include <libraries/dos.h>
#include <workbench/startup.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <intuition/screens.h>
#include <intuition/cghooks.h>

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>

#include <defs.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// StringGadgetClass instance data
struct StringGadgetInstance
	{
	Object *sgi_Frame;		// BOOPSI frame object
	Object *sgi_StringGadget;	// BOOPSI string gadget
	LONG sgi_HBorder;
	LONG sgi_VBorder;
	};

//----------------------------------------------------------------------------

// The functions in this module

static SAVEDS(ULONG) INTERRUPT dispatchStringGadgetClass(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetNew(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetDispose(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetRender(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetGoActive(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetHandleInput(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetLayout(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetHitTest(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetHelpTest(Class *cl, Object *o, Msg msg);
static ULONG StringGadgetGet(Class *cl, Object *o, Msg msg);

//----------------------------------------------------------------------------


/***********************************************************/
/**	Make the class and set up the dispatcher's hook	**/
/***********************************************************/
struct ScalosClass *initStringGadgetClass(const struct PluginClass *plug)
{
	struct ScalosClass *StringGadgetClass;

	StringGadgetClass =  SCA_MakeScalosClass(plug->plug_classname,
			plug->plug_superclassname,
			sizeof(struct StringGadgetInstance),
			0 );

	if (StringGadgetClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(StringGadgetClass->sccl_class->cl_Dispatcher, dispatchStringGadgetClass);
		}

	return StringGadgetClass;
}

/**************************************************************************/
/**********	   The StringGadgetCLASS class dispatcher	  *********/
/**************************************************************************/
static SAVEDS(ULONG) INTERRUPT dispatchStringGadgetClass(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld:  Class=%l08x  SuperClass=%08lx  Method=%08lx\n", __FILE__, __FUNC__,__LINE__, cl, cl->cl_Super, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = StringGadgetNew(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = StringGadgetDispose(cl, o, msg);
		break;
	case OM_GET:
		Result = StringGadgetGet(cl, o, msg);
		break;
	case GM_GOACTIVE:
		Result = StringGadgetGoActive(cl, o, msg);
		break;
	case GM_HANDLEINPUT:
		Result = StringGadgetHandleInput(cl, o, msg);
		break;
	case GM_HITTEST:
		Result = StringGadgetHitTest(cl, o, msg);
		break;
	case GM_HELPTEST:
		Result = StringGadgetHelpTest(cl, o, msg);
		break;
	case GM_LAYOUT:
		Result = StringGadgetLayout(cl, o, msg);
		break;
	case GM_RENDER:
		Result = StringGadgetRender(cl, o, msg);
		break;
	default:
		{
		struct StringGadgetInstance *inst = INST_DATA(cl, o);

		DoMethodA(inst->sgi_StringGadget, msg);

		Result =  DoSuperMethodA(cl, o, msg);
		break;
		}
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG StringGadgetNew(Class *cl, Object *o, Msg msg)
{
	BOOL Success = FALSE;

	do	{
		struct opSet *ops = ((struct opSet *) msg);
		struct StringGadgetInstance *inst;

		o  = (Object *) DoSuperMethodA(cl, o, msg);
		if (NULL == o)
			break;

		/* Initial local instance data */
		inst = INST_DATA(cl, o);
		memset(inst, 0, sizeof(struct StringGadgetInstance));

		inst->sgi_StringGadget = NewObject(NULL, STRGCLASS,
			TAG_MORE, ops->ops_AttrList,
			TAG_END);
		if (NULL == inst->sgi_StringGadget)
			break;

		inst->sgi_Frame = NewObject(NULL, FRAMEICLASS,
			IA_FrameType, FRAME_RIDGE,
			IA_EdgesOnly, TRUE,
//			IA_DrawInfo, iInfos.xii_iinfos.ii_DrawInfo,
			TAG_END);
		if (NULL == inst->sgi_Frame)
			break;

		Success = TRUE;
		} while (0);

	if (o && !Success)
		{
		DoMethod(o, OM_DISPOSE);
		o = NULL;
		}

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG StringGadgetDispose(Class *cl, Object *o, Msg msg)
{
	struct StringGadgetInstance *inst = INST_DATA(cl, o);

	if (inst->sgi_Frame)
		{
		DisposeObject(inst->sgi_Frame);
		inst->sgi_Frame = NULL;
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG StringGadgetRender(Class *cl, Object *o, Msg msg)
{
	struct gpRender *gpr = (struct gpRender *) msg;
	struct StringGadgetInstance *inst = INST_DATA(cl, o);
	ULONG Result;

	Result = DoSuperMethodA(cl, o, msg);

	if (GREDRAW_REDRAW == gpr->gpr_Redraw)
		{
		struct StringGadgetInstance *inst = INST_DATA(cl, o);
		struct ExtGadget *gg = (struct ExtGadget *) o;

		DrawImageState(gpr->gpr_RPort,
			(struct Image *) inst->sgi_Frame,
			gg->LeftEdge,
			gg->TopEdge,
			IDS_NORMAL,
			gpr->gpr_GInfo->gi_DrInfo);
		}

	DoMethodA(inst->sgi_StringGadget, msg);

	return Result;
}

//----------------------------------------------------------------------------

static ULONG StringGadgetGoActive(Class *cl, Object *o, Msg msg)
{
	struct gpInput *gpi = (struct gpInput *) msg;
	struct StringGadgetInstance *inst = INST_DATA(cl, o);
	struct gpInput gpiCopy = *gpi;

	gpiCopy.gpi_Mouse.X -= inst->sgi_HBorder;
	gpiCopy.gpi_Mouse.Y -= inst->sgi_VBorder;

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gpiCopy.gpi_Mouse.X, gpiCopy.gpi_Mouse.Y));

	DoSuperMethodA(cl, o, msg);
	
	return DoMethodA(inst->sgi_StringGadget, (Msg)(APTR) &gpiCopy);
}

//----------------------------------------------------------------------------

static ULONG StringGadgetHandleInput(Class *cl, Object *o, Msg msg)
{
	struct gpInput *gpi = (struct gpInput *) msg;
	struct StringGadgetInstance *inst = INST_DATA(cl, o);
	struct gpInput gpiCopy = *gpi;

	gpiCopy.gpi_Mouse.X -= inst->sgi_HBorder;
	gpiCopy.gpi_Mouse.Y -= inst->sgi_VBorder;

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gpiCopy.gpi_Mouse.X, gpiCopy.gpi_Mouse.Y));

	DoSuperMethodA(cl, o, msg);

	return DoMethodA(inst->sgi_StringGadget, (Msg)(APTR) &gpiCopy);
}

//----------------------------------------------------------------------------

static ULONG StringGadgetLayout(Class *cl, Object *o, Msg msg)
{
	struct gpLayout *gpl = (struct gpLayout *) msg;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx gpl_GInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gpl->gpl_GInfo));

	if (gpl->gpl_GInfo)
		{
		struct StringGadgetInstance *inst = INST_DATA(cl, o);
		struct ExtGadget *gg = (struct ExtGadget *) o;

		if (gpl->gpl_Initial)
			{
			struct IBox FrameBox = { 0, 0, 20, 15 };
			struct IBox ContentsBox = { 3, 3, 20 - 2 * 3, 15 - 2 * 3 };

			DoMethod(inst->sgi_Frame,
				IM_FRAMEBOX,
				&ContentsBox,
				&FrameBox,
				gpl->gpl_GInfo->gi_DrInfo,
				0);

			d1(KPrintF("%s/%s/%ld: FrameBox: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, FrameBox.Left, FrameBox.Top, FrameBox.Width, FrameBox.Height));
			d1(KPrintF("%s/%s/%ld: ContentsBox: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, ContentsBox.Left, ContentsBox.Top, ContentsBox.Width, ContentsBox.Height));

			inst->sgi_HBorder = (FrameBox.Width - ContentsBox.Width) / 2;
			inst->sgi_VBorder = (FrameBox.Height - ContentsBox.Height) / 2;

			SetAttrs(inst->sgi_Frame,
				IA_Width, gg->Width,
				IA_Height, gg->Height,
				TAG_END);

			SetAttrs(inst->sgi_StringGadget,
				GA_Left, gg->LeftEdge + inst->sgi_HBorder,
				GA_Top, gg->TopEdge + inst->sgi_VBorder,
				GA_Width, gg->Width - 2 * inst->sgi_HBorder,
				GA_Height, gg->Height - 2 * inst->sgi_VBorder,
				TAG_END);

			DoMethod(inst->sgi_StringGadget,
				GM_LAYOUT,
				gpl->gpl_GInfo,
				TRUE);
			}
		else
			{
			DoMethodA(inst->sgi_StringGadget, msg);
			}
		}

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG StringGadgetHitTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;
	struct StringGadgetInstance *inst = INST_DATA(cl, o);
	struct gpHitTest gphtCopy = *gpht;

	gphtCopy.gpht_Mouse.X -= inst->sgi_HBorder;
	gphtCopy.gpht_Mouse.Y -= inst->sgi_VBorder;

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gphtCopy.gpht_Mouse.X, gphtCopy.gpht_Mouse.Y));

	DoSuperMethodA(cl, o, msg);

	return DoMethodA(inst->sgi_StringGadget, (Msg)(APTR) &gphtCopy);
}

//----------------------------------------------------------------------------

static ULONG StringGadgetHelpTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y));

	if (gpht->gpht_Mouse.X < 0 || gpht->gpht_Mouse.X >= gg->Width)
		return GMR_NOHELPHIT;
	if (gpht->gpht_Mouse.Y < 0 || gpht->gpht_Mouse.Y >= gg->Height)
		return GMR_NOHELPHIT;

	return GMR_HELPHIT;
}

//----------------------------------------------------------------------------

static ULONG StringGadgetGet(Class *cl, Object *o, Msg msg)
{
	struct opGet *opg = (struct opGet *) msg;
	struct Gadget *gg = (struct Gadget *) o;
	ULONG Result = 1;

	if (NULL == opg->opg_Storage)
		return 0;

	*(opg->opg_Storage) = 0;

	switch (opg->opg_AttrID)
		{
	case GA_Left:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->LeftEdge;
		break;
	case GA_Top:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->TopEdge;
		break;
	case GA_Width:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->Width;
		break;
	case GA_Height:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->Height;
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------
