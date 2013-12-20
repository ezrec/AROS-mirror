// VolumeGauge.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <graphics/rpattr.h>
#include <libraries/dos.h>
#include <libraries/iffparse.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/cghooks.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include <scalos/scalos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <math.h>


#include <defs.h>

#include "volumegauge.h"

// aus Amiga.lib
#if !defined(__amigaos4__) && !defined(__AROS__)
extern ULONG HookEntry(struct Hook *, Object *, APTR);
#endif


static struct RastPort *NewObtainGIRPort(struct GadgetInfo *gInfo);


/***********************************************************/
/****************	  Class specifics	  ****************/
/***********************************************************/

// The functions in this module
static ULONG INTERRUPT InitVolumeGauge(Class *cl, struct Gadget *g, struct opSet *ops);
static ULONG LayoutVolumeGauge(Class *cl, struct Gadget *g, struct gpLayout *gpr);
static ULONG RenderVolumeGauge(Class *, struct Gadget *, struct gpRender *);
static ULONG UpdateVolumeGauge(Class *cl, struct Gadget *g, struct opSet *msg);
static void DrawGauge(struct RastPort *rp, struct VolumeGaugeInst *inst,
		struct Gadget *g, struct GadgetInfo *GInfo, short BarHeight);
static void DrawTick(struct RastPort *rp, struct VolumeGaugeInst *inst,
		short x, short y, short TickSize, BOOL active);
static short GetVolumeGaugeHeight(struct VolumeGaugeInst *inst, struct Gadget *g, struct GadgetInfo *GInfo);
static SAVEDS(ULONG) dispatchVolumeGaugeGad(Class *cl, Object *o, Msg msg);
static void ScaleValue(struct VolumeGaugeInst *inst);
static long GetLeftEdge(const struct Gadget *g, const struct GadgetInfo *gi);
static long GetTopEdge(const struct Gadget *g, const struct GadgetInfo *gi);
static long GetWidth(const struct Gadget *g, const struct GadgetInfo *gi);
static long GetHeight(const struct Gadget *g, const struct GadgetInfo *gi);


static Class *VolumeGaugecl;


/***********************************************************/
/**	Make the class and set up the dispatcher's hook	**/
/***********************************************************/
Class *VOLUMEGAUGE_GetClass(void)
{
	if (VolumeGaugecl)
		return VolumeGaugecl;

	VolumeGaugecl = MakeClass( NULL, (STRPTR) GADGETCLASS, NULL, sizeof(struct VolumeGaugeInst), 0);

	if (VolumeGaugecl)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(VolumeGaugecl->cl_Dispatcher, dispatchVolumeGaugeGad);
		}

	return VolumeGaugecl;
}

/***********************************************************/
/******************	 Free the class	  ****************/
/***********************************************************/
BOOL freeVolumeGaugeGadClass(void)
{
	return (FreeClass(VolumeGaugecl));
}

/***********************************************************/
/**********	   The VolumeGauge class dispatcher	  *********/
/***********************************************************/
static SAVEDS(ULONG) INTERRUPT dispatchVolumeGaugeGad(Class *cl, Object *o, Msg msg)
{
	struct VolumeGaugeInst *inst;
	ULONG retval = FALSE;
	Object *object;

	switch (msg->MethodID)
		{
	case OM_NEW:	   /* First, pass up to superclass */
		if (object = (Object *) DoSuperMethodA(cl, o, msg))
			retval = InitVolumeGauge(cl, (struct Gadget *) object, (struct opSet *) msg);
		break;

	case OM_DISPOSE:
//		inst = INST_DATA(cl, o);
		DoSuperMethodA(cl, o, msg);
		break;

	case GM_HITTEST:
		retval = 0;
		break;

	case GM_LAYOUT:
		retval = LayoutVolumeGauge(cl, (struct Gadget *)o, (struct gpLayout *)msg);
		break;

	case GM_RENDER:
		retval = RenderVolumeGauge(cl, (struct Gadget *)o, (struct gpRender *)msg);
		break;

	case OM_SET:
		inst = INST_DATA(cl, o);

		{
		DoSuperMethodA(cl, o, msg);	/* Größe & Ort ändern */

		inst->MinValue = GetTagData(VOLUMEGAUGE_Min, inst->MinValue, ((struct opSet *)msg)->ops_AttrList);
		inst->MaxValue = GetTagData(VOLUMEGAUGE_Max, inst->MaxValue, ((struct opSet *)msg)->ops_AttrList);
		inst->Level = GetTagData(VOLUMEGAUGE_Level, inst->Level, ((struct opSet *)msg)->ops_AttrList);

		ScaleValue(inst);

		retval = UpdateVolumeGauge(cl, (struct Gadget *)o, (struct opSet *)msg);
		}
		break;

	default:
		retval = DoSuperMethodA(cl, o, msg);
		break;
		}

	return(retval);
}


/*************************************************************************************************/
/*******************************   Initialize the gadget.   **************************************/
/*************************************************************************************************/
static ULONG INTERRUPT InitVolumeGauge(Class *cl, struct Gadget *g, struct opSet *ops)
{
	struct VolumeGaugeInst *inst = INST_DATA(cl, (Object *)g);

	g->Width = 0;

	inst->LastLevel = ~0;
	inst->ScaleFactor = 0;

	inst->MinValue  = GetTagData(VOLUMEGAUGE_Min, 0, ops->ops_AttrList);
	inst->MaxValue  = GetTagData(VOLUMEGAUGE_Max, 0, ops->ops_AttrList);
	inst->Level     = GetTagData(VOLUMEGAUGE_Level, 0, ops->ops_AttrList);
	inst->Ticks	= GetTagData(VOLUMEGAUGE_Ticks, 0, ops->ops_AttrList);
	inst->ShortTicks= GetTagData(VOLUMEGAUGE_ShortTicks, 0, ops->ops_AttrList);
	inst->TickSize	= GetTagData(VOLUMEGAUGE_TickSize, 6, ops->ops_AttrList);

	ScaleValue(inst);

	return (ULONG) g;
}


/*************************************************************************************************/
/*************************************   Layout the gadget.   ************************************/
/*************************************************************************************************/
static ULONG LayoutVolumeGauge(Class *cl, struct Gadget *g, struct gpLayout *gpl)
{
	struct VolumeGaugeInst *inst = INST_DATA(cl, (Object *)g);
	ULONG retval = TRUE;

	d1(kprintf("%s/%s/%ld:  GInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, gpl->gpl_GInfo));

	if (gpl->gpl_GInfo && gpl->gpl_GInfo->gi_Window)
		{
		Object *CloseImage;
		ULONG GWidth = 14;

		d1(kprintf("%s/%s/%ld:  Window=%08lx\n", __FILE__, __FUNC__, __LINE__, gpl->gpl_GInfo->gi_Window));
		d1(kprintf("%s/%s/%ld:  BorderLeft=%ld  BorderRight=%ld\n", __FILE__, __FUNC__, __LINE__, gpl->gpl_GInfo->gi_Window->BorderLeft, gpl->gpl_GInfo->gi_Window->BorderRight));

		CloseImage = NewObject(NULL, SYSICLASS,
			SYSIA_Which, CLOSEIMAGE,
			SYSIA_DrawInfo, gpl->gpl_GInfo->gi_DrInfo,
			TAG_END);
		if (CloseImage)
			{
			GetAttr(IA_Width, CloseImage, &GWidth);
			GWidth--;

			DisposeObject(CloseImage);
			}

		if (gpl->gpl_GInfo->gi_Window->BorderLeft < GWidth)
			gpl->gpl_GInfo->gi_Window->BorderLeft = GWidth;
		if (gpl->gpl_GInfo->gi_Window->BorderLeft < gpl->gpl_GInfo->gi_Window->BorderRight)
			gpl->gpl_GInfo->gi_Window->BorderLeft = gpl->gpl_GInfo->gi_Window->BorderRight;

		SetAttrs(g,
			GA_Left, 4,
			GA_Top, gpl->gpl_GInfo->gi_Window->BorderTop + 2,
			GA_Width, gpl->gpl_GInfo->gi_Window->BorderLeft - 4 - 4,
			GA_RelHeight, - (gpl->gpl_GInfo->gi_Window->BorderTop + 2) - gpl->gpl_GInfo->gi_Window->BorderBottom,
			TAG_END);

		// Get the pens for drawing
		inst->InactiveShinePen = inst->ActiveShinePen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[SHINEPEN];
		inst->InactiveShadowPen = inst->ActiveShadowPen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[SHADOWPEN];
		inst->ActiveFillPen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[FILLPEN];
		inst->InactiveFillPen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[DETAILPEN];

#if DRI_VERSION >= 3 && !defined(__MORPHOS__) && !defined(__AROS__)
		if (gpl->gpl_GInfo->gi_DrInfo->dri_Version >= 3)
			{
			inst->ActiveShinePen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[FILLSHINEPEN];
			inst->ActiveShadowPen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[FILLSHADOWPEN];
		        inst->InactiveFillPen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[INACTIVEFILLPEN];
		        inst->InactiveShinePen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[INACTIVEFILLSHINEPEN];
		        inst->InactiveShadowPen = gpl->gpl_GInfo->gi_DrInfo->dri_Pens[INACTIVEFILLSHADOWPEN];
			}
#endif
		}

	return(retval);
}


/*************************************************************************************************/
/*******************************   Erase and rerender the gadget.   ******************************/
/*************************************************************************************************/
static ULONG INTERRUPT RenderVolumeGauge(Class *cl, struct Gadget *g, struct gpRender *gpr)
{
	struct VolumeGaugeInst *inst = INST_DATA(cl, (Object *)g);
	ULONG retval = TRUE;
	BOOL active = (gpr->gpr_GInfo->gi_Window->Flags & WFLG_WINDOWACTIVE) != 0;

	d1(kprintf("%s/%s/%ld:  RPort=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		gpr->gpr_RPort));

	if (gpr->gpr_RPort)
		{
		short x = GetLeftEdge(g, gpr->gpr_GInfo);
		short y = GetTopEdge(g, gpr->gpr_GInfo);
		short w = GetWidth(g, gpr->gpr_GInfo);
		short h = GetHeight(g, gpr->gpr_GInfo);
		short y2 = y + h - 1;
		short x2 = x + w - 1;

		SetAPen(gpr->gpr_RPort, active ? inst->ActiveShadowPen : inst->InactiveShadowPen);

		Move(gpr->gpr_RPort, x2, y);
		Draw(gpr->gpr_RPort, x, y);
		Draw(gpr->gpr_RPort, x, y2);

		SetAPen(gpr->gpr_RPort, active ? inst->ActiveShinePen : inst->InactiveShinePen);

		Move(gpr->gpr_RPort, x2, 1 + y);
		Draw(gpr->gpr_RPort, x2, y2+1);
		Draw(gpr->gpr_RPort, x + 1, y2+1);

		DrawGauge(gpr->gpr_RPort, inst, g, 
			gpr->gpr_GInfo, 
			GetVolumeGaugeHeight(inst, g, gpr->gpr_GInfo));
		}

	return(retval);
}


static ULONG INTERRUPT UpdateVolumeGauge(Class *cl, struct Gadget *g, struct opSet *ops)
{
	struct VolumeGaugeInst *inst = INST_DATA(cl, (Object *)g);
	struct RastPort *rp;
	ULONG retval = TRUE;

	rp = NewObtainGIRPort(ops->ops_GInfo);
	if (rp)
		{
		short BarHeight;

		BarHeight = GetVolumeGaugeHeight(inst, g, ops->ops_GInfo);

		if (BarHeight != inst->LastLevel)
			{
			DrawGauge(rp, inst, g, ops->ops_GInfo, BarHeight);

			inst->LastLevel = BarHeight;
			}

		ReleaseGIRPort(rp);
		}

	return(retval);
}


static void DrawGauge(struct RastPort *rp, struct VolumeGaugeInst *inst, 
		struct Gadget *g, struct GadgetInfo *GInfo, short BarHeight)
{
	short x = GetLeftEdge(g, GInfo) + 1;
	short y1, y = GetTopEdge(g, GInfo) + 1;
	short w = GetWidth(g, GInfo) - 2;
	short h = GetHeight(g, GInfo) - 2;
	short y2 = y + h - 1;
	short x2 = x + w - 1;
	short n;
	BOOL active = (GInfo->gi_Window->Flags & WFLG_WINDOWACTIVE) != 0;

	y1 = y;
	y = y + h - BarHeight;

	SetAPen(rp, active ? inst->ActiveFillPen : inst->InactiveFillPen);

	RectFill(rp, x, y1, x2, y2);

	SetAPen(rp, active ? inst->ActiveShinePen : inst->InactiveShinePen);

	Move(rp, x2, y);
	Draw(rp, x, y);
	Draw(rp, x, y + BarHeight);

	SetAPen(rp, active ? inst->ActiveShadowPen : inst->InactiveShadowPen);

	Move(rp, x2, 1 + y);
	Draw(rp, x2, y + BarHeight);
	Draw(rp, x + 1, y + BarHeight);

	for (n=0; n<=inst->Ticks; n++)
		{
		short yTick = y1 + h - ((n * h) / inst->Ticks);

		if (yTick < y && yTick > y1)
			{
			DrawTick(rp, inst, x, yTick, inst->TickSize, active);
			}

		if (inst->ShortTicks && yTick < y)
			{
			yTick += h / (2 * inst->Ticks);

			if (yTick < y)
				{
				DrawTick(rp, inst, x, yTick, inst->TickSize / 2, active);
				}
			}
		}
}


static void DrawTick(struct RastPort *rp, struct VolumeGaugeInst *inst, short x, short y, short TickSize, BOOL active)
{
	SetAPen(rp, active ? inst->ActiveShinePen : inst->InactiveShinePen);
	Move(rp, x, y);
	Draw(rp, x + TickSize, y);

	SetAPen(rp, active ? inst->ActiveShadowPen : inst->InactiveShadowPen);
	Move(rp, x, y-1);
	Draw(rp, x + TickSize, y-1);
}


static short GetVolumeGaugeHeight(struct VolumeGaugeInst *inst, struct Gadget *g, struct GadgetInfo *GInfo)
{
	short BarHeight;

	if (inst->MaxValue - inst->MinValue)
		{
		BarHeight = ((long)(GetHeight(g, GInfo) - 2l) * 
			((inst->Level >> inst->ScaleFactor) - (inst->MinValue  >> inst->ScaleFactor))) /
			((inst->MaxValue  >> inst->ScaleFactor) - (inst->MinValue  >> inst->ScaleFactor));
		}
	else
		BarHeight = 0;

	return BarHeight;
}


static void ScaleValue(struct VolumeGaugeInst *inst)
{
	ULONG val = inst->MaxValue;

	inst->ScaleFactor = 0;
	while (val & 0xffff0000)
		{
		val = (val >> 1) & 0x7fffffff;
		inst->ScaleFactor++;
		}
}


static long GetLeftEdge(const struct Gadget *g, const struct GadgetInfo *gi)
{
	if (g->Flags & GFLG_RELRIGHT)
		return gi->gi_Window->Width + g->LeftEdge;

	return g->LeftEdge;
}


static long GetTopEdge(const struct Gadget *g, const struct GadgetInfo *gi)
{
	if (g->Flags & GFLG_RELBOTTOM)
		return gi->gi_Window->Height + g->TopEdge;

	return g->TopEdge;
}


static long GetWidth(const struct Gadget *g, const struct GadgetInfo *gi)
{
	if (g->Flags & GFLG_RELWIDTH)
		return gi->gi_Window->Width + g->Width;

	return g->Width;
}


static long GetHeight(const struct Gadget *g, const struct GadgetInfo *gi)
{
	if (g->Flags & GFLG_RELHEIGHT)
		return gi->gi_Window->Height + g->Height;

	return g->Height;
}


static struct RastPort *NewObtainGIRPort(struct GadgetInfo *gInfo)
{
	struct RastPort *Result;

	if (gInfo && gInfo->gi_Layer)
		{
		if (AttemptLockLayerRom(gInfo->gi_Layer))
			{
			Forbid();	// no chance for anyone to lock Layers inbetween...

			UnlockLayerRom(gInfo->gi_Layer);	// Got a lock, free it
			Result = ObtainGIRPort(gInfo);		// call original function

			Permit();
			}
		else
			{
			// Attempt to lock the layer failed, return NULL
			Result = NULL;
			}
		}
	else
		Result = ObtainGIRPort(gInfo);

	return Result;
}
