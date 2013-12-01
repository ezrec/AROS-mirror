// ButtonGadgetClass.c
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
#include <scalos/GadgetBar.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// ButtonGadgetClass instance data
struct ButtonGadgetInstance
	{
	ULONG bgi_Hidden;		// Flag: this gadget is hidden
	};

//----------------------------------------------------------------------------

// The functions in this module

static SAVEDS(ULONG) INTERRUPT dispatchButtonGadgetClass(Class *cl, Object *o, Msg msg);
static ULONG ButtonGadgetNew(Class *cl, Object *o, Msg msg);
static ULONG ButtonGadget_Set(Class *cl, Object *o, Msg msg);
static ULONG ButtonGadget_Get(Class *cl, Object *o, Msg msg);
static ULONG ButtonGadget_Render(Class *cl, Object *o, Msg msg);

//----------------------------------------------------------------------------

/***********************************************************/
/**	Make the class and set up the dispatcher's hook	**/
/***********************************************************/
struct ScalosClass *initButtonGadgetClass(const struct PluginClass *plug)
{
	struct ScalosClass *ButtonGadgetClass;

	ButtonGadgetClass =  SCA_MakeScalosClass(plug->plug_classname,
			plug->plug_superclassname,
			sizeof(struct ButtonGadgetInstance),
			NULL);
	d1(KPrintF("%s/%s/%ld  ButtonGadgetClass=%08lx  plug_classname=<%s> plug_superclassname=<%s>\n",
		__FILE__, __FUNC__, __LINE__, ButtonGadgetClass, plug->plug_classname, plug->plug_superclassname));

	if (ButtonGadgetClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(ButtonGadgetClass->sccl_class->cl_Dispatcher, dispatchButtonGadgetClass);
		}

	return ButtonGadgetClass;
}

//----------------------------------------------------------------------------

/**************************************************************************/
/**********	   The ButtonGadgetCLASS class dispatcher	  *********/
/**************************************************************************/
static SAVEDS(ULONG) INTERRUPT dispatchButtonGadgetClass(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(KPrintF("%s/%s/%ld  Class=%l08x  SuperClass=%08lx  Method=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, cl->cl_Super, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = ButtonGadgetNew(cl, o, msg);
		break;
	case OM_SET:
		Result = ButtonGadget_Set(cl, o, msg);
		break;
	case OM_GET:
		Result = ButtonGadget_Get(cl, o, msg);
		break;
	case GM_RENDER:
		Result = ButtonGadget_Render(cl, o, msg);
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG ButtonGadgetNew(Class *cl, Object *o, Msg msg)
{
	BOOL Success = FALSE;

	do	{
		struct opSet *ops = ((struct opSet *) msg);
		struct ButtonGadgetInstance *inst;

		o  = (Object *) DoSuperMethodA(cl, o, msg);
		d1(KPrintF("%s/%s/%ld:  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		if (NULL == o)
			break;

		/* Initial local instance data */
		inst = INST_DATA(cl, o);
		memset(inst, 0, sizeof(struct ButtonGadgetInstance));

		inst->bgi_Hidden = GetTagData(GBDTA_Hidden, FALSE, ops->ops_AttrList);

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

static ULONG ButtonGadget_Get(Class *cl, Object *o, Msg msg)
{
	struct ButtonGadgetInstance *inst = INST_DATA(cl, o);
	struct opGet *opg = (struct opGet *) msg;
	struct Gadget *gg = (struct Gadget *) o;
	ULONG Success = TRUE;

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
	case GBDTA_Hidden:
		*(opg->opg_Storage) = (ULONG) inst->bgi_Hidden;
		break;
	default:
		Success = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Success;
}

//----------------------------------------------------------------------------

static ULONG ButtonGadget_Set(Class *cl, Object *o, Msg msg)
{
	struct ButtonGadgetInstance *inst = INST_DATA(cl, o);
	struct opSet *ops = (struct opSet *) msg;
	struct TagItem *TagList = ops->ops_AttrList;
	struct TagItem *tag;
	struct Gadget *gg = (struct Gadget *) o;
	BOOL NeedRerender = FALSE;
	ULONG Result;

	while ((tag = NextTagItem(&TagList)))
		{
		switch (tag->ti_Tag)
			{
		case GA_Disabled:
			if ((tag->ti_Data && !(gg->Flags & GFLG_DISABLED)) ||
				(!tag->ti_Data && (gg->Flags & GFLG_DISABLED)))
				{
				d1(KPrintF("%s/%s/%ld  o=%l08x  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->Flags));
				NeedRerender = TRUE;
				}
			break;
		case GBDTA_Hidden:
			inst->bgi_Hidden = tag->ti_Data;
			break;
		default:
			break;
			}
		}

	Result = DoSuperMethodA(cl, o, msg);

	if (NeedRerender && !inst->bgi_Hidden)
		{
		struct RastPort *rp;

		rp = ObtainGIRPort(ops->ops_GInfo);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  rp=%08lx\n", __FILE__, __FUNC__, __LINE__, o, rp));
		if (rp)
			{
			DoMethod(o,
				GM_RENDER,
				ops->ops_GInfo,
				rp,
				GREDRAW_UPDATE);
			ReleaseGIRPort(rp);
			}
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG ButtonGadget_Render(Class *cl, Object *o, Msg msg)
{
	struct gpRender *gpr = (struct gpRender *) msg;
	struct Gadget *gg = (struct Gadget *) o;
	ULONG Result;

	d1(KPrintF("%s/%s/%ld  o=%l08x  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->Flags));

	if (gg->Flags & GFLG_DISABLED)
		{
		Object *Image = NULL;

		GetAttr(GA_Image, o, (APTR) &Image);

		if (NULL == Image && (gg->Flags & GFLG_GADGIMAGE))
			Image = (Object *) gg->GadgetRender;

		d1(KPrintF("%s/%s/%ld:  Image=%l08x\n", __FILE__, __FUNC__, __LINE__, Image));

		if (Image)
			{
			ULONG SupportsDisable = FALSE;

			Result = 0;

			GetAttr(IA_SupportsDisable, Image, &SupportsDisable);
			d1(KPrintF("%s/%s/%ld:  SupportsDisable=%ld\n", __FILE__, __FUNC__, __LINE__, SupportsDisable));
			if (SupportsDisable)
				{
				DrawImageState(gpr->gpr_RPort,
					(struct Image *) Image,
					gg->LeftEdge,
					gg->TopEdge,
					IDS_DISABLED,
					gpr->gpr_GInfo->gi_DrInfo);
				}
			else
				{
				static UWORD Pattern[] =
					{ 0x8888, 0x2222 };

				DrawImageState(gpr->gpr_RPort,
					(struct Image *) Image,
					gg->LeftEdge,
					gg->TopEdge,
					IDS_NORMAL,
					gpr->gpr_GInfo->gi_DrInfo);


				d1(kprintf("%s/%s/%ld: DrawGhosted\n", __FILE__, __FUNC__, __LINE__));

				SetABPenDrMd(gpr->gpr_RPort,
					gpr->gpr_GInfo->gi_DrInfo->dri_Pens[SHADOWPEN], 0, JAM1);
				SetAfPt(gpr->gpr_RPort, Pattern, 1);
				RectFill(gpr->gpr_RPort,
					gg->LeftEdge,
					gg->TopEdge,
					gg->LeftEdge + gg->Width - 1,
					gg->TopEdge + gg->Height - 1);
				}
			}
		else
			Result = DoSuperMethodA(cl, o, msg);
		}
	else
		{
		Result = DoSuperMethodA(cl, o, msg);
		}

	return Result;
}

//----------------------------------------------------------------------------


