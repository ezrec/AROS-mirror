// FrameButtonMCC.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <libraries/mui.h>
#include <cybergraphx/cybergraphics.h>
#include <scalos/scalosgfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/scalosgfx.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <prefs/popupmenu.h>
#include "PopupMenuPrefs.h"
#include "FrameButtonMCC.h"

/* ------------------------------------------------------------------------- */

#define CLASS	   MUIC_FrameButton
#define SUPERCLASS MUIC_Text

struct FrameButtonMCCInstance
{
	ULONG	FrameType;
	ULONG	Selected;		// show frame for selected menu entries
	ULONG	Raised;			// draw raised selected frames
};

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg);
static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg);
static void DrawBoxMM2(struct RastPort *rp, int x1, int y1, int x2, int y2,
	UBYTE shine, UBYTE shadow, UBYTE halfshine);
static void DrawXENBoxMM2(struct RastPort *rp, int x1, int y1, int x2, int y2,
	UBYTE shine, UBYTE shadow, UBYTE bgplus, UBYTE bgminus, UBYTE halfshine);
static void DrawBox(struct RastPort *rp, int x1, int y1, int x2, int y2, UBYTE shine, UBYTE shadow);
static void DrawIBox(struct RastPort *rp, int x1, int y1, int x2, int y2, UBYTE shine, UBYTE shadow);
static void DrawXENBox(struct RastPort *rp, int x1, int y1, int x2, int y2,
	UBYTE shine, UBYTE shadow, UBYTE bgplus, UBYTE bgminus);
static void DrawDBLBox(struct RastPort *rp, int x1, int y1, int x2, int y2, UBYTE shine, UBYTE shadow);
static void DrawDropBox(struct RastPort *rp, int x1, int y1, int x2, int y2, UBYTE shine, UBYTE shadow);
DISPATCHER_PROTO(FrameButtonClass);
static ULONG mGet(Class *cl, Object *o, struct opGet *opg);
static void ClearInstanceData(struct FrameButtonMCCInstance *inst);

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct FrameButtonMCCInstance *inst;
	struct opSet *ops;
	BOOL Success = FALSE;

	do	{
		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		if (NULL == obj)
			break;

		d1(kprintf(__FUNC__ "/%ld:  obj=%08lx\n", __LINE__, obj));

		inst = INST_DATA(cl,obj);
		ClearInstanceData(inst);

		ops = (struct opSet *) msg;

		inst->FrameType = GetTagData(MUIA_ScaFrameButton_FrameType, PMP_MENUBORDER_THIN, ops->ops_AttrList);
		inst->Selected  = GetTagData(MUIA_ScaFrameButton_Selected, FALSE, ops->ops_AttrList);
		inst->Raised    = GetTagData(MUIA_ScaFrameButton_Raised, FALSE, ops->ops_AttrList);

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		d1(kprintf(__FUNC__ "/%ld:  obj=%08lx\n", __LINE__, obj));
		if (obj)
			{
			DisposeObject(obj);
			obj = NULL;
			}
		}

	return((ULONG)obj);
}

/* ------------------------------------------------------------------------- */

static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct FrameButtonMCCInstance *inst = INST_DATA(cl,obj);

	DoSuperMethodA(cl, obj, (Msg) msg);

	if (msg->flags & MADF_DRAWOBJECT)
		{
		d1(KPrintF("%s/%ld: bitmap=%08lx\n", __FUNC__, __LINE__, inst->BitMap));

		if(inst->Selected)
			{
			if (PMP_MENUBORDER_MM == inst->FrameType)
				{
				if (inst->Raised)
					{
					DrawBoxMM2(_rp(obj), _mleft(obj), _mtop(obj),
						_mright(obj), _mbottom(obj),
                                                _dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN],
						_dri(obj)->dri_Pens[BACKGROUNDPEN]);
					}
				else
					{
					DrawBoxMM2(_rp(obj), _mleft(obj), _mtop(obj),
						_mright(obj), _mbottom(obj),
						_dri(obj)->dri_Pens[HALFSHADOWPEN], _dri(obj)->dri_Pens[HALFSHINEPEN],
						_dri(obj)->dri_Pens[BACKGROUNDPEN]);
					}
				}
			else if (PMP_MENUBORDER_NONE != inst->FrameType)
				{
				if (inst->Raised)
					{
					DrawBox(_rp(obj), _mleft(obj), _mtop(obj),
						_mright(obj), _mbottom(obj),
						_dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN]);
					}
				else
					{
					DrawBox(_rp(obj), _mleft(obj), _mtop(obj),
						_mright(obj), _mbottom(obj),
						_dri(obj)->dri_Pens[HALFSHADOWPEN], _dri(obj)->dri_Pens[HALFSHINEPEN]);
					}
				}
			}
		else
			{
			switch(inst->FrameType)
				{
			case PMP_MENUBORDER_NONE:
				break;
			case PMP_MENUBORDER_THIN:
				DrawBox(_rp(obj), _mleft(obj), _mtop(obj),
					_mright(obj), _mbottom(obj),
					_dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN]);
				break;
			case PMP_MENUBORDER_MM:
				DrawXENBoxMM2(_rp(obj), _mleft(obj), _mtop(obj),
					_mright(obj),  _mbottom(obj),
					_dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN],
					_dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN],
					_dri(obj)->dri_Pens[BACKGROUNDPEN]);
				break;
			case PMP_MENUBORDER_THICK:
				DrawXENBox(_rp(obj), _mleft(obj), _mtop(obj),
					_mright(obj),  _mbottom(obj),
					_dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN],
					_dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN]);
				break;
			case PMP_MENUBORDER_RIDGE:
				DrawDBLBox(_rp(obj), _mleft(obj), _mtop(obj),
					_mright(obj),  _mbottom(obj),
					_dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN]);
				break;
			case PMP_MENUBORDER_DROPBOX:
				DrawDropBox(_rp(obj), _mleft(obj), _mtop(obj),
					_mright(obj),  _mbottom(obj),
					_dri(obj)->dri_Pens[HALFSHINEPEN], _dri(obj)->dri_Pens[HALFSHADOWPEN]);
				break;
			case PMP_MENUBORDER_OLDSTYLE:
				DrawIBox(_rp(obj), _mleft(obj), _mtop(obj),
					_mright(obj),  _mbottom(obj),
					0, _dri(obj)->dri_Pens[HALFSHADOWPEN]);
				break;
				}
			}
		}

	return(0);
}

static void DrawBoxMM2(struct RastPort *rp, int x1, int y1, int x2, int y2,
	UBYTE shine, UBYTE shadow, UBYTE halfshine)
{
	SetAPen(rp, halfshine);
	WritePixel(rp, x1, y2);
	WritePixel(rp, x2, y1);
	SetAPen(rp, shine);
        Move(rp, x1, y2-1);
        Draw(rp, x1, y1);
        Draw(rp, x2-1, y1);
	SetAPen(rp, shadow);
        Move(rp, x1+1, y2);
        Draw(rp, x2, y2);
        Draw(rp, x2, y1+1);
}

static void DrawXENBoxMM2(struct RastPort *rp, int x1, int y1, int x2, int y2,
	UBYTE shine, UBYTE shadow, UBYTE bgplus, UBYTE bgminus, UBYTE halfshine)
{
	SetAPen(rp, shine);
        Move(rp, x1, y2);
        Draw(rp, x1, y1);
        Draw(rp, x2, y1);
        SetAPen(rp, bgplus);
        Move(rp, x1+1, y2-1);
        Draw(rp, x1+1, y1+1);
        Draw(rp, x2-1, y1+1);
        SetAPen(rp, shadow);
        Move(rp, x1, y2);
        Draw(rp, x2, y2);
        Draw(rp, x2, y1);
        SetAPen(rp, bgminus);
        Move(rp, x1+1, y2-1);
        Draw(rp, x2-1, y2-1);
        Draw(rp, x2-1, y1+1);
	SetAPen(rp, halfshine);
	WritePixel(rp, x1, y2);
	WritePixel(rp, x2, y1);
}

static void DrawBox(struct RastPort *rp, int x1, int y1, int x2, int y2, UBYTE shine, UBYTE shadow)
{
        SetAPen(rp, shine);
        Move(rp, x1, y2-1);
        Draw(rp, x1, y1);
        Draw(rp, x2-1, y1);
        SetAPen(rp, shadow);
        Move(rp, x1, y2);
        Draw(rp, x2, y2);
        Draw(rp, x2, y1);

}

static void DrawIBox(struct RastPort *rp, int x1, int y1, int x2, int y2, UBYTE shine, UBYTE shadow)
{
        SetAPen(rp, shadow);
        Move(rp, x1, y2);
        Draw(rp, x1, y1);
        Draw(rp, x2, y1);

        Move(rp, x1, y2);
        Draw(rp, x2, y2);
        Draw(rp, x2, y1);

	Move(rp, x1+1, y1);
	Draw(rp, x1+1, y2);

	Move(rp, x2-1, y1);
	Draw(rp, x2-1, y2);
}

static void DrawXENBox(struct RastPort *rp, int x1, int y1, int x2, int y2,
	UBYTE shine, UBYTE shadow, UBYTE bgplus, UBYTE bgminus)
{
        SetAPen(rp, shine);
        Move(rp, x1, y2);
        Draw(rp, x1, y1);
        Draw(rp, x2, y1);
        SetAPen(rp, bgplus);
        Move(rp, x1+1, y2-1);
        Draw(rp, x1+1, y1+1);
        Draw(rp, x2-1, y1+1);
        SetAPen(rp, shadow);
        Move(rp, x1, y2);
        Draw(rp, x2, y2);
        Draw(rp, x2, y1);
        SetAPen(rp, bgminus);
        Move(rp, x1+1, y2-1);
        Draw(rp, x2-1, y2-1);
        Draw(rp, x2-1, y1+1);
}

static void DrawDBLBox(struct RastPort *rp, int x1, int y1, int x2, int y2, UBYTE shine, UBYTE shadow)
{
	DrawBox(rp, x1, y1, x2, y2, shine, shadow);
	DrawBox(rp, x1+1, y1+1, x2-1, y2-1, shine, shadow);
}

static void DrawDropBox(struct RastPort *rp, int x1, int y1, int x2, int y2, UBYTE shine, UBYTE shadow)
{
        SetAPen(rp, shine);
        Move(rp, x1, y2-1);
        Draw(rp, x1, y1);
        Draw(rp, x2-1, y1);
        Move(rp, x1+4, y2-3);
        Draw(rp, x2-3, y2-3);
        Draw(rp, x2-3, y1+4);
        SetAPen(rp, shadow);
        Move(rp, x1+3, y2-3);
        Draw(rp, x1+3, y1+3);
        Draw(rp, x2-4, y1+3);
        Move(rp, x1, y2);
        Draw(rp, x2, y2);
        Draw(rp, x2, y1);
}

/* ------------------------------------------------------------------------- */

static ULONG mGet(Class *cl, Object *o, struct opGet *opg)
{
	struct FrameButtonMCCInstance *inst = INST_DATA(cl, o);
	ULONG Result = 0;

	switch (opg->opg_AttrID)
		{
	case MUIA_ScaFrameButton_FrameType:
		*(opg->opg_Storage) = (ULONG) inst->FrameType;
		Result++;
		break;
	case MUIA_ScaFrameButton_Selected:
		*(opg->opg_Storage) = (ULONG) inst->Selected;
		Result++;
		break;
	case MUIA_ScaFrameButton_Raised:
		*(opg->opg_Storage) = (ULONG) inst->Raised;
		Result++;
		break;
	default:
		Result = DoSuperMethodA(cl, o, (Msg) opg);
		}

	return Result;
}

/* ------------------------------------------------------------------------- */

DISPATCHER(FrameButtonClass)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		d1(KPrintF("%s/%ld: OM_NEW\n", __FUNC__, __LINE__));
		Result = mNew(cl,obj,(APTR)msg);
		break;

	case OM_GET:
		d1(KPrintF("%s/%ld: OM_GET\n", __FUNC__, __LINE__));
		Result = mGet(cl, obj, (struct opGet *) msg);
		break;

	case MUIM_Draw:
		d1(KPrintF("%s/%ld: MUIM_Draw\n", __FUNC__, __LINE__));
		Result = mDraw(cl,obj,(APTR)msg);
		break;

	default:
		d1(KPrintF("%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl,obj,msg);
		break;
		}

	return Result;
}
DISPATCHER_END


static void ClearInstanceData(struct FrameButtonMCCInstance *inst)
{
	memset(inst, 0, sizeof(struct FrameButtonMCCInstance));
}

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitFrameButtonClass(void)
{
	return MUI_CreateCustomClass(NULL, SUPERCLASS, NULL,
		sizeof(struct FrameButtonMCCInstance), DISPATCHER_REF(FrameButtonClass));
}

void CleanupFrameButtonClass(struct MUI_CustomClass *mcc)
{
	MUI_DeleteCustomClass(mcc);
}

/* ------------------------------------------------------------------------- */


