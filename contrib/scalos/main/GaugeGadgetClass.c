// GaugeGadgetClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <graphics/rpattr.h>
#include <libraries/dos.h>
#include <libraries/iffparse.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/cghooks.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include <proto/diskfont.h>
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
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

#include "GaugeGadgetClass.h"


//----------------------------------------------------------------------------

// local data definitions

struct GaugeINST
{
	struct Window *GaugeWindow;
	short BarPen;			// Pen-Nummer for bar
	short Left, Top, Width, Height;

	short LastLevel;
	long Level;
	long MinValue, MaxValue;

	long ScaleFactor;

	BOOL Vertical;

	struct RastPort myRastPort;
	struct Layer_Info *LayerInfo;

	APTR vi;

	BOOL WantNum;			/* Flag: Text display wanted? */
	struct TextFont *TextFont;	// Font for text display or NULL
	struct TTFontFamily TTFont;	// TTengine Font 
	char NumFormat[20];		/* Format string for text display */
	short TextPen;			// Pen number for text

	char NumText[20];		/* Text buffer for text display */
};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT GaugeGadgetClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG  RenderGauge(Class *, struct Gadget *, struct gpRender *);
static ULONG INTERRUPT UpdateGauge(Class *cl, struct Gadget *g, struct opSet *msg);
static short GetGaugeWidth(struct GaugeINST *inst, struct Gadget *g);
static void DrawGauge(struct GaugeINST *inst, struct Gadget *g, struct RastPort *rp,
		struct GadgetInfo *gInfo, short GaugeWidth);
static void ScaleValue(struct GaugeINST *inst);
static void GaugeDrawText(struct GaugeINST *inst, struct RastPort *rp, struct Gadget *g,
		const char *Text, struct GadgetInfo *gInfo);
static struct RastPort *NewObtainGIRPort(struct GadgetInfo *gInfo);

//----------------------------------------------------------------------------


struct ScalosClass *initGaugeGadgetClass(const struct PluginClass *plug)
{
	struct ScalosClass *GaugeGadgetClass;

	GaugeGadgetClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct GaugeINST),
			NULL);

	if (GaugeGadgetClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(GaugeGadgetClass->sccl_class->cl_Dispatcher, GaugeGadgetClassDispatcher);
		}

	return GaugeGadgetClass;
}


/***********************************************************/
/**********	   The Gauge class dispatcher	  *********/
/***********************************************************/
static SAVEDS(ULONG) INTERRUPT GaugeGadgetClassDispatcher(Class *cl, Object *o, Msg msg)
{
	struct GaugeINST *inst;
	ULONG retval = FALSE;
	Object *object;

	switch (msg->MethodID)
		{
	case OM_NEW:	   /* First, pass up to superclass */
		if ((object = (Object *) DoSuperMethodA(cl, o, msg)))
			{
			struct opSet *ops = ((struct opSet *)msg);
			struct TextAttr *PBTextAttr;
			CONST_STRPTR PBTTTextDesc;

			/* Initial local instance data */
			inst = INST_DATA(cl, object);

			inst->LastLevel = ~0;
			inst->ScaleFactor = 0;

			inst->MinValue  = GetTagData(SCAGAUGE_MinValue, 0, ops->ops_AttrList);
			inst->MaxValue  = GetTagData(SCAGAUGE_MaxValue, 0, ops->ops_AttrList);
			inst->Level     = GetTagData(SCAGAUGE_Level, 0,  ops->ops_AttrList);
			inst->Vertical  = GetTagData(SCAGAUGE_Vertical, FALSE,  ops->ops_AttrList);
			inst->WantNum   = GetTagData(SCAGAUGE_WantNumber, FALSE, ops->ops_AttrList);

			inst->BarPen    = GetTagData(SCAGAUGE_BarPen, FILLPEN, ops->ops_AttrList);
			inst->TextPen   = GetTagData(SCAGAUGE_TextPen, TEXTPEN, ops->ops_AttrList);

			PBTextAttr      = (struct TextAttr *) GetTagData(SCAGAUGE_TextFont, (ULONG) NULL, ops->ops_AttrList);
			PBTTTextDesc	= (CONST_STRPTR) GetTagData(SCAGAUGE_TTFont, (ULONG) NULL, ops->ops_AttrList);

			stccpy(inst->NumFormat, (char *) GetTagData(SCAGAUGE_NumFormat, (ULONG) "%3d%%", ops->ops_AttrList),
					sizeof(inst->NumText));

			if (PBTTTextDesc)
				Scalos_OpenTTFont(PBTTTextDesc, &inst->TTFont);
			if (PBTextAttr)
				inst->TextFont = OpenDiskFont(PBTextAttr);

			ScaleValue(inst);

			retval = (ULONG)object;
			}
		break;

	case OM_DISPOSE:
		inst = INST_DATA(cl, o);
		
		Scalos_CloseFont(&inst->TextFont, &inst->TTFont);

		if (inst->vi)
			{
			FreeVisualInfo(inst->vi);
			inst->vi = NULL;
			}
		DoSuperMethodA(cl, o, msg);
		break;

	case GM_HITTEST:
		retval = 0;
		break;

	case GM_RENDER:
		retval = RenderGauge(cl, (struct Gadget *)o, (struct gpRender *)msg);
		break;

	case OM_SET:
		inst = INST_DATA(cl, o);

		{
		DoSuperMethodA(cl, o, msg);	// set size and position

		inst->MinValue = GetTagData(SCAGAUGE_MinValue, inst->MinValue, ((struct opSet *)msg)->ops_AttrList);
		inst->MaxValue = GetTagData(SCAGAUGE_MaxValue, inst->MaxValue, ((struct opSet *)msg)->ops_AttrList);
		inst->Level = GetTagData(SCAGAUGE_Level, inst->Level, ((struct opSet *)msg)->ops_AttrList);

		ScaleValue(inst);

		retval = UpdateGauge(cl, (struct Gadget *)o, (struct opSet *)msg);
		}
		break;

	default:
		retval = DoSuperMethodA(cl, o, msg);
		break;
		}

	return(retval);
}



/*************************************************************************************************/
/*******************************   Erase and rerender the gadget.   ******************************/
/*************************************************************************************************/
static ULONG INTERRUPT RenderGauge(Class *cl, struct Gadget *g, struct gpRender *msg)
{
	struct GaugeINST *inst = INST_DATA(cl, (Object *)g);
	ULONG retval = TRUE;

	if (inst->vi == NULL)
		{
		inst->vi = GetVisualInfo(msg->gpr_GInfo->gi_Screen, TAG_END);

		if (inst->vi == NULL)
			return FALSE;
		}

	if (msg->gpr_RPort)
		{
		DrawGauge(inst, g, msg->gpr_RPort, msg->gpr_GInfo, GetGaugeWidth(inst, g));	// Draw inside area according to Level

		if (inst->WantNum)
			{
			ScaFormatStringMaxLength(inst->NumText, sizeof(inst->NumText),
				inst->NumFormat, 
				100 * ((inst->Level >> inst->ScaleFactor) - (inst->MinValue  >> inst->ScaleFactor)) /
					((inst->MaxValue  >> inst->ScaleFactor) - (inst->MinValue  >> inst->ScaleFactor))
				);

			GaugeDrawText(inst, msg->gpr_RPort, g, inst->NumText, msg->gpr_GInfo);		// Draw text
			}

		/* draw border */
		DrawBevelBox(msg->gpr_RPort, g->LeftEdge, g->TopEdge, g->Width, g->Height,
			GTBB_FrameType, BBFT_BUTTON,
			GTBB_Recessed, TRUE,
			GT_VisualInfo, (ULONG) inst->vi,
			TAG_END );
		}

	return(retval);
}


static ULONG INTERRUPT UpdateGauge(Class *cl, struct Gadget *g, struct opSet *msg)
{
	struct GaugeINST *inst = INST_DATA(cl, (Object *)g);
	struct RastPort *rp;
	ULONG retval = TRUE;
	short GaugeWidth;

	rp = NewObtainGIRPort(msg->ops_GInfo);
	if (rp)
		{
		GaugeWidth = GetGaugeWidth(inst, g);

		if (GaugeWidth != inst->LastLevel)
			{
			DrawGauge(inst, g, rp, msg->ops_GInfo, GaugeWidth);

			inst->LastLevel = GaugeWidth;

			if (inst->WantNum)
				{
				short GaugePercent;

				GaugePercent = 100 * ((inst->Level >> inst->ScaleFactor) - (inst->MinValue  >> inst->ScaleFactor)) /
					((inst->MaxValue  >> inst->ScaleFactor) - (inst->MinValue  >> inst->ScaleFactor));

				ScaFormatStringMaxLength(inst->NumText, sizeof(inst->NumText),
					inst->NumFormat, GaugePercent);

				GaugeDrawText(inst, rp, g, inst->NumText, msg->ops_GInfo);		// draw updated text
				}
			}

		ReleaseGIRPort(rp);
		}

	return(retval);
}


static short GetGaugeWidth(struct GaugeINST *inst, struct Gadget *g)
{
	short GaugeWidth;

	if (inst->MaxValue - inst->MinValue)
		{
		GaugeWidth = ((long)(g->Width - 4l) * 
			((inst->Level >> inst->ScaleFactor) - (inst->MinValue  >> inst->ScaleFactor))) /
			((inst->MaxValue  >> inst->ScaleFactor) - (inst->MinValue  >> inst->ScaleFactor));
		}
	else
		GaugeWidth = 0;

	return GaugeWidth;
}


static void DrawGauge(struct GaugeINST *inst, struct Gadget *g, struct RastPort *rp,
		struct GadgetInfo *gInfo, short GaugeWidth)
{
	GaugeWidth = min(max(0, GaugeWidth), g->Width - 4);

	SetAfPt(rp, NULL, 0);

	// draw left edge to level
	SetAPen(rp, inst->BarPen);
	RectFill(rp,
		g->LeftEdge+2, g->TopEdge+1,
		g->LeftEdge+2 + GaugeWidth - 1,
		g->TopEdge+1 + g->Height-2 - 1);

	// Erase level to right edge
	SetAPen(rp, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[BACKGROUNDPEN]);
	RectFill(rp,
		g->LeftEdge+2 + GaugeWidth, g->TopEdge+1,
		g->LeftEdge + g->Width - 2 - 1,
		g->TopEdge+1 + g->Height-2 - 1);
}


static void ScaleValue(struct GaugeINST *inst)
{
	ULONG val = inst->MaxValue;

	inst->ScaleFactor = 0;
	while (val & 0xffff0000)
		{
		val = (val >> 1) & 0x7fffffff;
		inst->ScaleFactor++;
		}
}


static void GaugeDrawText(struct GaugeINST *inst, struct RastPort *rp, struct Gadget *g,
		const char *PBText, struct GadgetInfo *gInfo)
{
	struct TextExtent tExt;

	SetDrMd(rp, JAM1);
	SetAPen(rp, inst->TextPen);

	Scalos_SetFont(rp, inst->TextFont, &inst->TTFont);

	Scalos_TextExtent(rp, (STRPTR) PBText, strlen(PBText), &tExt);

	Move(rp, g->LeftEdge + 2 + (g->Width - tExt.te_Width) / 2,
		 g->TopEdge + 1 + (g->Height - tExt.te_Height) / 2 + Scalos_GetFontBaseline(rp));
	Scalos_Text(rp, (STRPTR) PBText, strlen(PBText));

//	SetDrMd(rp, JAM1);
}


static struct RastPort *NewObtainGIRPort(struct GadgetInfo *gInfo)
{
	struct RastPort *Result;

	if (gInfo && gInfo->gi_Layer)
		{
		if (AttemptLockLayerRom(gInfo->gi_Layer))
			{
			UnlockLayerRom(gInfo->gi_Layer);	// Got a lock, free it
			Result = ObtainGIRPort(gInfo);		// call original function
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
