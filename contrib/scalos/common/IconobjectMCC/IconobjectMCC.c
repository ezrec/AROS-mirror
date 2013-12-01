// IconobjectMCC.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <libraries/mui.h>
#include <datatypes/pictureclass.h>
#include <datatypes/iconobject.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <proto/iconobject.h>

#include <clib/alib_protos.h>

#include "defs.h"
#include "IconobjectMCC.h"
#include "debug.h"


#define CLASS	   MUIC_Iconobject
#define SUPERCLASS MUIC_Area


struct DtMCCInstance
{
	Object *Iconobj;
	ULONG Selected;
};

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg);
static ULONG mDispose(struct IClass *cl,Object *obj,Msg msg);
static ULONG mSetup(struct IClass *cl,Object *obj,Msg msg);
static ULONG mCleanup(struct IClass *cl,Object *obj,Msg msg);
static ULONG mAskMinMax(struct IClass *cl,Object *obj, struct MUIP_AskMinMax *msg);
static ULONG mDraw(struct IClass *cl,Object *obj, struct MUIP_Draw *msg);
static ULONG mSet(struct IClass *cl, Object *obj, Msg msg);
static ULONG mGet(struct IClass *cl, Object *obj, Msg msg);

DISPATCHER_PROTO(MUI);
static void ClearInstanceData(struct DtMCCInstance *inst);
static void ForceRelayout(struct IClass *cl, Object *obj);

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct DtMCCInstance *inst;
	struct opSet *ops;

	if (!(obj=(Object *) DoSuperMethodA(cl,obj,msg)))
		return(0);

	inst = INST_DATA(cl,obj);
	ClearInstanceData(inst);

	set(obj, MUIA_FillArea, TRUE);

	ops = (struct opSet *) msg;

	inst->Iconobj = (Object *) GetTagData(MUIA_Iconobj_Object, (ULONG)NULL, ops->ops_AttrList);
	inst->Selected = GetTagData(MUIA_Selected, FALSE, ops->ops_AttrList);

	return (ULONG) obj;
}


static ULONG mDispose(struct IClass *cl,Object *obj,Msg msg)
{
//	struct DtMCCInstance *inst = INST_DATA(cl,obj);

	return DoSuperMethodA(cl, obj, msg);
}


static ULONG mSetup(struct IClass *cl,Object *obj,Msg msg)
{
//	struct DtMCCInstance *inst = INST_DATA(cl,obj);

	if (!DoSuperMethodA(cl,obj,msg))
		return(FALSE);

	return(TRUE);
}


static ULONG mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
//	struct DtMCCInstance *inst = INST_DATA(cl,obj);

	return DoSuperMethodA(cl,obj,msg);
}


static ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);
	struct MUI_MinMax *mi;

	DoSuperMethodA(cl, obj, (Msg) msg);

	mi = msg->MinMaxInfo;

	if (inst->Iconobj)
		{
		const struct ExtGadget *gg = (const struct ExtGadget *) inst->Iconobj;

		d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

		mi->MinWidth  += gg->Width;
		mi->MinHeight += gg->Height;
		mi->DefWidth  += gg->Width;
		mi->DefHeight += gg->Height;
		mi->MaxWidth  += gg->Width;
		mi->MaxHeight += gg->Height;
		}

	/* if we have no Iconobject, our object's size will be 0 */

	return(0);
}


static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	DoSuperMethodA(cl, obj, (Msg) msg);

	if (msg->flags & MADF_DRAWOBJECT)
		{
		if (inst->Iconobj)
			{
			struct ExtGadget *gg = (struct ExtGadget *) inst->Iconobj;

			if (inst->Selected)
				{
				gg->Flags |= GFLG_SELECTED;

				if (NULL == gg->SelectRender)
					{
					DoMethod(inst->Iconobj, IDTM_Layout,
						_screen(obj), _window(obj), _rp(obj), _dri(obj),
						IOLAYOUTF_SelectedImage);
					}
				}
			else
				{
				gg->Flags &= ~GFLG_SELECTED;

				if (NULL == gg->GadgetRender)
					{
					DoMethod(inst->Iconobj, IDTM_Layout,
						_screen(obj), _window(obj), _rp(obj), _dri(obj),
						IOLAYOUTF_NormalImage);
					}
				}

			DoMethod(inst->Iconobj, IDTM_Draw, 
				_screen(obj), _window(obj), _rp(obj), _dri(obj),
				_mleft(obj),_mtop(obj),
				IODRAWF_AbsolutePos | IODRAWF_NoText | IODRAWF_NoEraseBg);
			}
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return(0);
}


static ULONG mSet(struct IClass *cl, Object *obj, Msg msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);
	struct opSet *ops = (struct opSet *) msg;

	inst->Iconobj = (Object *) GetTagData(MUIA_Iconobj_Object, (ULONG) inst->Iconobj, ops->ops_AttrList);
	inst->Selected = GetTagData(MUIA_Selected, inst->Selected, ops->ops_AttrList);

	// Setting a different IconObj requires relayout
	if (FindTagItem(MUIA_Iconobj_Object, ops->ops_AttrList))
		ForceRelayout(cl, obj);

	return DoSuperMethodA(cl, obj, msg);
}


static ULONG mGet(struct IClass *cl, Object *obj, Msg msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);
	struct opGet *opg = (struct opGet *) msg;

	switch (opg->opg_AttrID)
		{
	case MUIA_Selected:
		*(opg->opg_Storage) = inst->Selected;
		break;
		}

	return DoSuperMethodA(cl, obj, msg);
}


DISPATCHER(MUI)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		d1(KPrintF("%s/%s/%ld: OM_NEW\n", __FILE__, __FUNC__, __LINE__));
		Result = mNew(cl, obj, msg);
		break;

	case OM_DISPOSE:
		d1(KPrintF("%s/%s/%ld: OM_DISPOSE\n", __FILE__, __FUNC__, __LINE__));
		Result = mDispose(cl, obj, msg);
		break;

	case OM_SET:
		d1(KPrintF("%s/%s/%ld: OM_SET\n", __FILE__, __FUNC__, __LINE__));
		Result = mSet(cl,obj, msg);
		break;

	case OM_GET:
		d1(KPrintF("%s/%s/%ld: OM_GET\n", __FILE__, __FUNC__, __LINE__));
		Result = mGet(cl, obj, msg);
		break;

	case MUIM_Setup:
		d1(KPrintF("%s/%s/%ld: MUIM_Setup\n", __FILE__, __FUNC__, __LINE__));
		Result = mSetup(cl, obj, msg);
		break;

	case MUIM_Cleanup:
		d1(KPrintF("%s/%s/%ld: MUIM_Cleanup\n", __FILE__, __FUNC__, __LINE__));
		Result = mCleanup(cl, obj, msg);
		break;

	case MUIM_AskMinMax:
		d1(KPrintF("%s/%s/%ld: MUIM_AskMinMax\n", __FILE__, __FUNC__, __LINE__));
		Result = mAskMinMax(cl, obj, (struct MUIP_AskMinMax *) msg);
		break;

	case MUIM_Draw:
		d1(KPrintF("%s/%s/%ld: MUIM_Draw\n", __FILE__, __FUNC__, __LINE__));
		Result = mDraw(cl, obj, (struct MUIP_Draw *) msg);
		break;

	default:
		d1(KPrintF("%s/%s/%ld: MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END


static void ClearInstanceData(struct DtMCCInstance *inst)
{
	memset(inst, 0, sizeof(struct DtMCCInstance));
}

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitIconobjectClass(void)
{
	return MUI_CreateCustomClass(NULL, SUPERCLASS, NULL,
		sizeof(struct DtMCCInstance), DISPATCHER_REF(MUI));
}

void CleanupIconobjectClass(struct MUI_CustomClass *mcc)
{
	MUI_DeleteCustomClass(mcc);
}

/* ------------------------------------------------------------------------- */

static void ForceRelayout(struct IClass *cl, Object *obj)
{
	Object *WindowObj;
	Object *RootObj = NULL;

	if (muiRenderInfo(obj))
		{
		WindowObj = _win(obj);
		get(WindowObj, MUIA_Window_RootObject, &RootObj);

		if (RootObj)
			{
			// force relayout
			DoMethod(RootObj, MUIM_Group_InitChange);
			DoMethod(RootObj, MUIM_Group_ExitChange);
			}
		}
}

/* ------------------------------------------------------------------------- */

