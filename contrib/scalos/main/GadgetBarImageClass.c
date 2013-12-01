// GadgetBarImageClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <intuition/cghooks.h>
#include <utility/hooks.h>
#include <libraries/gadtools.h>
#include <datatypes/pictureclass.h>
#include <datatypes/animationclass.h>
#include <datatypes/animationclassext.h>
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
#include <proto/gadtools.h>
#include <proto/datatypes.h>
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

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

struct GadgetBarImageClassInst
	{
	struct internalScaWindowTask *gbicl_iwt;
	struct DatatypesImage *gbicl_DtImage;   // Picture DataTypes object
	Object *gbicl_AnimDTObject;		// Animation DataTypes object
	BOOL gbicl_LayoutOk;
	struct Window *gbicl_Window;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT GadgetBarImageClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_New(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_Dispose(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_Set(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_Get(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_Update(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_Layout(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_Render(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_Remove(Class *cl, Object *o, Msg msg);
static ULONG GadgetBarImage_HelpTest(Class *cl, Object *o, Msg msg);
static BOOL GadgetBarImage_PointInGadget(Object *o, WORD x, WORD y);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

struct ScalosClass *initGadgetBarImageClass(const struct PluginClass *plug)
{
	struct ScalosClass *GadgetBarImageClass;

	GadgetBarImageClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct GadgetBarImageClassInst),
			NULL);

	if (GadgetBarImageClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(GadgetBarImageClass->sccl_class->cl_Dispatcher, GadgetBarImageClassDispatcher);
		}

	return GadgetBarImageClass;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT GadgetBarImageClassDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld: MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = GadgetBarImage_New(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = GadgetBarImage_Dispose(cl, o, msg);
		break;
	case OM_SET:
		Result = GadgetBarImage_Set(cl, o, msg);
		break;
	case OM_GET:
		Result = GadgetBarImage_Get(cl, o, msg);
		break;
	case OM_UPDATE:
		Result = GadgetBarImage_Update(cl, o, msg);
		break;
	case GM_LAYOUT:
		Result = GadgetBarImage_Layout(cl, o, msg);
		break;
	case GM_RENDER:
		Result = GadgetBarImage_Render(cl, o, msg);
		break;
	case DTM_REMOVEDTOBJECT:
		Result = GadgetBarImage_Remove(cl, o, msg);
		break;
	case GM_HELPTEST:
		Result = GadgetBarImage_HelpTest(cl, o, msg);
		break;
	case GM_HITTEST:
		return 0;
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_New(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct GadgetBarImageClassInst *inst;
	struct ExtGadget *gg;

	o = (Object *) DoSuperMethodA(cl, o, msg);
	if (NULL == o)
		return 0;

	inst = INST_DATA(cl, o);

	memset(inst, 0, sizeof(struct GadgetBarImageClassInst));

	inst->gbicl_LayoutOk = FALSE;

	inst->gbicl_iwt = (struct internalScaWindowTask *) GetTagData(GBIDTA_WindowTask, (ULONG)NULL, ops->ops_AttrList);

	inst->gbicl_DtImage = CreateDatatypesImage((STRPTR) GetTagData(DTA_Name, (ULONG)NULL, ops->ops_AttrList), 0);

	d1(KPrintF("%s/%s/%ld: gbicl_DtImage=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->gbicl_DtImage));

#if 0
// animations are not supported ATM
	if (NULL == inst->gbicl_DtImage)
		{
		inst->gbicl_AnimDTObject = NewDTObject((STRPTR) GetTagData(DTA_Name, NULL, ops->ops_AttrList),
				DTA_SourceType, DTST_FILE,
				DTA_GroupID, GID_ANIMATION,
				ADTA_Remap, TRUE,
				ADTA_Screen, iInfos.xii_iinfos.ii_Screen,
				DTA_Immediate, TRUE,
				DTA_Repeat, TRUE,
				DTA_ControlPanel, FALSE,
				ADTA_OvertakeScreen, FALSE,
				OBP_Precision, PRECISION_GUI,
				TAG_END);

		d1(kprintf("%s/%s/%ld: Animation DTObject=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->gbicl_AnimDTObject));
		}
#endif

	if (NULL == inst->gbicl_DtImage && NULL == inst->gbicl_AnimDTObject)
		{
		CoerceMethod(cl, o, OM_DISPOSE);
		return 0;
		}

	gg = (struct ExtGadget *) o;

	if (inst->gbicl_DtImage)
		{
		gg->Width = gg->BoundsWidth = inst->gbicl_DtImage->dti_BitMapHeader->bmh_Width;
		gg->Height = gg->BoundsHeight = inst->gbicl_DtImage->dti_BitMapHeader->bmh_Height;
		}
	if (inst->gbicl_AnimDTObject)
		{
		ULONG Width = 0, Height = 0;

		DoDTMethod(inst->gbicl_AnimDTObject, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE);

		GetDTAttrs(inst->gbicl_AnimDTObject, 
			ADTA_Width, (ULONG) &Width,
			ADTA_Height, (ULONG) &Height,
			TAG_END);

		gg->Width = gg->BoundsWidth = Width;
		gg->Height = gg->BoundsHeight = Height;
		}

	d1(kprintf("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_Dispose(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarImageClassInst *inst = INST_DATA(cl, o);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	d1(kprintf("%s/%s/%ld: Task=<%s>\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL)->tc_Node.ln_Name));
#if 1
	if (inst->gbicl_DtImage)
		{
		DisposeDatatypesImage(&inst->gbicl_DtImage);
		inst->gbicl_DtImage = NULL;
		}
#endif
	if (inst->gbicl_AnimDTObject)
		{
		DisposeDTObject(inst->gbicl_AnimDTObject);
		inst->gbicl_AnimDTObject = NULL;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_Set(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarImageClassInst *inst = INST_DATA(cl, o);
	struct opSet *ops = (struct opSet *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	inst->gbicl_LayoutOk = FALSE;

	if (inst->gbicl_AnimDTObject)
		{
		DoMethodA(inst->gbicl_AnimDTObject, msg);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	gg->BoundsLeftEdge = GetTagData(GA_Left, gg->BoundsLeftEdge, ops->ops_AttrList);
	gg->BoundsTopEdge = GetTagData(GA_Top, gg->BoundsTopEdge, ops->ops_AttrList);
	gg->BoundsWidth = GetTagData(GA_Width, gg->BoundsWidth, ops->ops_AttrList);
	gg->BoundsHeight = GetTagData(GA_Height, gg->BoundsHeight, ops->ops_AttrList);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_Update(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarImageClassInst *inst = INST_DATA(cl, o);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (inst->gbicl_AnimDTObject)
		{
		DoMethodA(inst->gbicl_AnimDTObject, msg);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_Get(Class *cl, Object *o, Msg msg)
{
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
	default:
		Success = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Success;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_Layout(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarImageClassInst *inst = INST_DATA(cl, o);
//	struct Gadget *gg = (struct Gadget *) o;
	struct gpLayout *gpl = (struct gpLayout *) msg;

	inst->gbicl_Window = gpl->gpl_GInfo->gi_Window;

	if (inst->gbicl_AnimDTObject)
		{
		// SetDTAttrsA()
		DoMethodA(inst->gbicl_AnimDTObject, msg);

		d1(kprintf("%s/%s/%ld: Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge));
		d1(kprintf("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));
		}

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_Render(Class *cl, Object *o, Msg msg)
{
	struct gpRender *gpr = (struct gpRender *) msg;
	struct GadgetBarImageClassInst *inst = INST_DATA(cl, o);
	struct Gadget *gg = (struct Gadget *) o;

	d1(kprintf("%s/%s/%ld: Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge));

	if (inst->gbicl_DtImage)
		{
		DtImageDraw(inst->gbicl_DtImage,
			gpr->gpr_RPort,
			gg->LeftEdge,
			gg->TopEdge,
			gg->Width, gg->Height
			);
		}
	if (inst->gbicl_AnimDTObject)
		{
		if (!inst->gbicl_LayoutOk)
			{
			DoDTMethod(inst->gbicl_AnimDTObject, gpr->gpr_GInfo->gi_Window, NULL, 
				GM_LAYOUT,
				(ULONG) gpr->gpr_GInfo,
				FALSE);
			inst->gbicl_LayoutOk = TRUE;
			}

		d1(kprintf("%s/%s/%ld: Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, \
			((struct Gadget *) inst->gbicl_AnimDTObject)->LeftEdge, ((struct Gadget *) inst->gbicl_AnimDTObject)->TopEdge));

		DoDTMethodA(inst->gbicl_AnimDTObject, gpr->gpr_GInfo->gi_Window, NULL, msg);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_Remove(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarImageClassInst *inst = INST_DATA(cl, o);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	d1(kprintf("%s/%s/%ld: Task=<%s>\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL)->tc_Node.ln_Name));

	if (inst->gbicl_AnimDTObject)
		{
		DoDTMethod(inst->gbicl_AnimDTObject, inst->gbicl_Window, NULL,
			ADTM_STOP, 0,
			TAG_END);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

//		DoMethodA(inst->gbicl_AnimDTObject, msg);

		RemoveDTObject(inst->gbicl_Window, inst->gbicl_AnimDTObject);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBarImage_HelpTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;

	d1(KPrintF("%s/%s/%ld: Object=%08lx  x=%ld  y=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y));

	if (GadgetBarImage_PointInGadget(o, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y))
		{
		d1(KPrintF("%s/%s/%ld: Result=GMR_HELPHIT\n", __FILE__, __FUNC__, __LINE__));
		return GMR_HELPHIT;
		}

	d1(KPrintF("%s/%s/%ld: Result=GMR_NOHELPHIT\n", __FILE__, __FUNC__, __LINE__));

	return GMR_NOHELPHIT;
}

//----------------------------------------------------------------------------

static BOOL GadgetBarImage_PointInGadget(Object *o, WORD x, WORD y)
{
	ULONG Width, Height;

	GetAttr(GA_Width, o, &Width);
	GetAttr(GA_Height, o, &Height);

	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, Width, Height));

	if (x < 0 || x >= Width)
		return FALSE;
	if (y < 0 || y >= Height)
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------

