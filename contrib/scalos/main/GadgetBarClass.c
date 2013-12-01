// GadgetBarClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <datatypes/pictureclass.h>
#include <libraries/gadtools.h>
#include <utility/pack.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <cybergraphx/cybergraphics.h>
#include <dos/dostags.h>
#include <dos/datetime.h>
#include <intuition/cghooks.h>

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

struct GadgetBarMember
	{
	struct Node gbm_Node;
	Object *gbm_Object;
	LONG gbm_Weight;
	LONG gbm_LeftOffset;    		// LeftEdge of member relative to GadgetBar
	LONG gbm_TopOffset;                     // TopEdge of member relative to GadgetBa
	struct Rectangle gbm_Bounds;            // Cached bounds rectangle of member gadget
	ULONG gbm_Flags;
	};

// Values in gbm_Flags
#define	GBMFLAGB_Skipped	0		// Gadget has been skipped since it doesn't fit

#define GBMFLAGF_Skipped	(1L << GBMFLAGB_Skipped)


struct GadgetBarClassInst
	{
	struct internalScaWindowTask *gbcl_WindowTask;
	enum GBposition gbcl_Position;
	struct List gbcl_MemberList;

	struct DatatypesImage *gbcl_Background;	// optional backfill image
	UWORD gbcl_BGPen;			// background pen (used if no backfill image available)

	WORD gbcl_TopSpace;			// space at top edge
	WORD gbcl_BottomSpace;			// space at bottom edge
	WORD gbcl_LeftSpace;			// space at left edge
	WORD gbcl_RightSpace;			// space at right edge
	WORD gbcl_BetweenSpace;			// space between images

	WORD gbcl_BorderLeft;			// Window->BorderLeft at Layout time
	WORD gbcl_BorderRight;

	WORD gbcl_WindowLeftBorder;		// Window->BorderLeft

	struct GadgetBarMember *gbcl_ActiveMember;	// this Gadget is currently active
	struct GadgetBarMember *gbcl_MemberHit;

	struct GadgetBarMember *gbcl_LastActiveMember;	// this member Gadget was the most recent active one

	struct Hook gbcl_BackFillHook;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT GadgetBarClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_New(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_Dispose(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_Set(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_Get(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_Layout(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_Render(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_AddMember(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_RemMember(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_UpdateMember(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_HelpTest(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_HitTest(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_GoActive(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_GoInactive(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_HandleInput(Class *cl, Object *o, Msg msg);
static ULONG GadgetBar_HandleMouseWheel(Class *cl, Object *o, Msg msg);

static BOOL GadgetBar_PointInGadget(struct ExtGadget *Parent, struct ExtGadget *gg, WORD x, WORD y);
static void EraseGadgetBarBackground(struct GadgetBarClassInst *inst, 
	struct RastPort *rp, struct ExtGadget *gg);
static SAVEDS(ULONG) GadgetBar_BackFillFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------


struct ScalosClass *initGadgetBarClass(const struct PluginClass *plug)
{
	struct ScalosClass *GadgetBarClass;

	GadgetBarClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct GadgetBarClassInst),
			NULL);

	if (GadgetBarClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(GadgetBarClass->sccl_class->cl_Dispatcher, GadgetBarClassDispatcher);
		}

	return GadgetBarClass;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT GadgetBarClassDispatcher(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarMember *Member;
	struct GadgetBarClassInst *inst;
	ULONG Result;

	d1(KPrintF("%s/%s/%ld: o=%08lx  MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, o, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = GadgetBar_New(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = GadgetBar_Dispose(cl, o, msg);
		break;
	case OM_SET:
		Result = GadgetBar_Set(cl, o, msg);
		break;
	case OM_GET:
		Result = GadgetBar_Get(cl, o, msg);
		break;
	case GM_HELPTEST:
		Result = GadgetBar_HelpTest(cl, o, msg);
		break;
	case GM_HITTEST:
		Result = GadgetBar_HitTest(cl, o, msg);
		break;
	case GM_GOACTIVE:
		Result = GadgetBar_GoActive(cl, o, msg);
		break;
	case GM_GOINACTIVE:
		Result = GadgetBar_GoInactive(cl, o, msg);
		break;
	case GM_HANDLEINPUT:
		Result = GadgetBar_HandleInput(cl, o, msg);
		break;
	case GM_LAYOUT:
		Result = GadgetBar_Layout(cl, o, msg);
		break;
	case GM_RENDER:
		Result = GadgetBar_Render(cl, o, msg);
		break;
	case OM_ADDMEMBER:
		Result = GadgetBar_AddMember(cl, o, msg);
		break;
	case OM_REMMEMBER:
		Result = GadgetBar_RemMember(cl, o, msg);
		break;
	case GBCL_UPDATEMEMBER:
		Result = GadgetBar_UpdateMember(cl, o, msg);
		break;
	case GBCL_HANDLEMOUSEWHEEL:
		Result = GadgetBar_HandleMouseWheel(cl, o, msg);
		break;

	default:
		inst = INST_DATA(cl, o);

		for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
			Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
			Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: Member=%08lx  Object=%08lx\n", __FILE__, __FUNC__, __LINE__, Member, Member->gbm_Object));

			DoMethodA(Member->gbm_Object, msg);
			}

		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	d1(KPrintF("%s/%s/%ld: END  o=%08lx  Result=%08lx\n", __FILE__, __FUNC__, __LINE__, o, Result));

	return Result;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_New(Class *cl, Object *o, Msg msg)
{
	struct opSet *ops = (struct opSet *) msg;
	struct GadgetBarClassInst *inst;
	struct ExtGadget *gg;

	o = (Object *) DoSuperMethodA(cl, o, msg);
	if (NULL == o)
		return 0;

	inst = INST_DATA(cl, o);

	memset(inst, 0, sizeof(struct GadgetBarClassInst));

	NewList(&inst->gbcl_MemberList);

	inst->gbcl_WindowTask = (struct internalScaWindowTask *) GetTagData(GBDTA_WindowTask, (ULONG)NULL, ops->ops_AttrList);

	if (NULL == inst->gbcl_WindowTask)
		{
		CoerceMethod(cl, o, OM_DISPOSE);
		return 0;
		}

	gg = (struct ExtGadget *) o;

	inst->gbcl_Position = GetTagData(GBDTA_Position, GBPOS_Top, ops->ops_AttrList);
	inst->gbcl_BGPen = GetTagData(GBDTA_BGPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN], ops->ops_AttrList);
	inst->gbcl_TopSpace = GetTagData(GBDTA_TopSpace, 2, ops->ops_AttrList);
	inst->gbcl_BottomSpace = GetTagData(GBDTA_BottomSpace, 2, ops->ops_AttrList);
	inst->gbcl_RightSpace = GetTagData(GBDTA_RightSpace, 2, ops->ops_AttrList);
	inst->gbcl_LeftSpace = GetTagData(GBDTA_LeftSpace, 2, ops->ops_AttrList);
	inst->gbcl_BetweenSpace = GetTagData(GBDTA_BetweenSpace, 5, ops->ops_AttrList);

	inst->gbcl_Background = CreateDatatypesImage((CONST_STRPTR) GetTagData(GBDTA_BackgroundImageName, (ULONG) "", ops->ops_AttrList), 0);

	gg->Height = gg->BoundsHeight = 0;
	inst->gbcl_WindowLeftBorder = 0;

	inst->gbcl_ActiveMember = NULL;
	inst->gbcl_MemberHit = NULL;

	SETHOOKFUNC(inst->gbcl_BackFillHook, GadgetBar_BackFillFunc);
	inst->gbcl_BackFillHook.h_Data = inst;

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_Dispose(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarMember *Member;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);

	// Free all members
	while ((Member = (struct GadgetBarMember *) RemHead(&inst->gbcl_MemberList)))
		{
		DoMethod(Member->gbm_Object, OM_DISPOSE);

		ScalosFree(Member);
		}

	DisposeDatatypesImage(&inst->gbcl_Background);

	if (inst->gbcl_WindowTask)
		{
		switch (inst->gbcl_Position)
			{
		case GBPOS_Top:
			inst->gbcl_WindowTask->iwt_GadgetBarHeightTop = 0;
			break;
		case GBPOS_Bottom:
			inst->gbcl_WindowTask->iwt_GadgetBarHeightBottom = 0;
			break;
			}
		}

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_Set(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct opSet *ops = (struct opSet *) msg;
	static const ULONG packTable[] =
		{
		PACK_STARTTABLE(DTA_Dummy),
		PACK_ENTRY(DTA_Dummy, GBDTA_BGPen, 	  GadgetBarClassInst, gbcl_BGPen, PKCTRL_UWORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(DTA_Dummy, GBDTA_TopSpace, 	  GadgetBarClassInst, gbcl_TopSpace, PKCTRL_WORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(DTA_Dummy, GBDTA_BottomSpace,  GadgetBarClassInst, gbcl_BottomSpace, PKCTRL_WORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(DTA_Dummy, GBDTA_RightSpace,   GadgetBarClassInst, gbcl_RightSpace, PKCTRL_WORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(DTA_Dummy, GBDTA_LeftSpace, 	  GadgetBarClassInst, gbcl_LeftSpace, PKCTRL_WORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(DTA_Dummy, GBDTA_BetweenSpace, GadgetBarClassInst, gbcl_BetweenSpace, PKCTRL_WORD | PKCTRL_PACKUNPACK),
		PACK_ENDTABLE
		};

	DoSuperMethodA(cl, o, msg);

	PackStructureTags(inst, packTable, ops->ops_AttrList);
/*
	inst->gbcl_BGPen = GetTagData(GBDTA_BGPen, inst->gbcl_BGPen, ops->ops_AttrList);
	inst->gbcl_TopSpace = GetTagData(GBDTA_TopSpace, inst->gbcl_TopSpace, ops->ops_AttrList);
	inst->gbcl_BottomSpace = GetTagData(GBDTA_BottomSpace, inst->gbcl_BottomSpace, ops->ops_AttrList);
	inst->gbcl_RightSpace = GetTagData(GBDTA_RightSpace, inst->gbcl_RightSpace, ops->ops_AttrList);
	inst->gbcl_LeftSpace = GetTagData(GBDTA_LeftSpace, inst->gbcl_LeftSpace, ops->ops_AttrList);
	inst->gbcl_BetweenSpace = GetTagData(GBDTA_BetweenSpace, inst->gbcl_BetweenSpace, ops->ops_AttrList);
*/
	if (FindTagItem(GBDTA_BackgroundImageName, ops->ops_AttrList))
		{
		DisposeDatatypesImage(&inst->gbcl_Background);
		inst->gbcl_Background = CreateDatatypesImage((CONST_STRPTR) GetTagData(GBDTA_BackgroundImageName, (ULONG) "", ops->ops_AttrList), 0);
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_Get(Class *cl, Object *o, Msg msg)
{
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct opGet *opg = (struct opGet *) msg;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	ULONG Success = TRUE;

	if (NULL == opg->opg_Storage)
		return 0;

	*(opg->opg_Storage) = 0;

	switch (opg->opg_AttrID)
		{
	case GA_Left:	// required since gadgetclass attribute is [IS]
		*(opg->opg_Storage) = (ULONG) gg->LeftEdge;
		break;
	case GA_Top:	// required since gadgetclass attribute is [IS]
		*(opg->opg_Storage) = (ULONG) gg->TopEdge;
		break;
	case GA_Width:	// required since gadgetclass attribute is [IS]
		*(opg->opg_Storage) = (ULONG) gg->Width;
		break;
	case GA_Height:	// required since gadgetclass attribute is [IS]
		*(opg->opg_Storage) = (ULONG) gg->Height;
		break;
	case GBDTA_BGPen:
		*(opg->opg_Storage) = inst->gbcl_BGPen;
		break;
	case GBDTA_TopSpace:
		*(opg->opg_Storage) = inst->gbcl_TopSpace;
		break;
	case GBDTA_BottomSpace:
		*(opg->opg_Storage) = inst->gbcl_BottomSpace;
		break;
	case GBDTA_RightSpace:
		*(opg->opg_Storage) = inst->gbcl_RightSpace;
		break;
	case GBDTA_LeftSpace:
		*(opg->opg_Storage) = inst->gbcl_LeftSpace;
		break;
	case GBDTA_BetweenSpace:
		*(opg->opg_Storage) = inst->gbcl_BetweenSpace;
		break;
	case GBDTA_LastActive:
		if (inst->gbcl_LastActiveMember)
			{
			struct ExtGadget *gg = (struct ExtGadget *) inst->gbcl_LastActiveMember->gbm_Object;
			
			*(opg->opg_Storage) = gg->GadgetID;
			}
		else
			{
			*(opg->opg_Storage) = 0;
			}
		break;

	default:
		Success = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Success;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_Layout(Class *cl, Object *o, Msg msg)
{
	struct gpLayout *gpl = (struct gpLayout *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct GadgetBarMember *Member;
	struct ExtGadget *gg = (struct ExtGadget *) o;
	WORD Left, Height;
	WORD FreeSpace;
	LONG TotalWeight;

	d1(KPrintF("%s/%s/%ld: START  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	if (NULL == gpl->gpl_GInfo || NULL == gpl->gpl_GInfo->gi_Window)
		{
		for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
			Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
			Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
			{
			DoMethod(Member->gbm_Object,
				GM_LAYOUT,
				gpl->gpl_GInfo,
				TRUE);

			d1(KPrintF("%s/%s/%ld: after GM_LAYOUT:  Width=%ld  BoundsWidth=%ld\n", __FILE__, __FUNC__, __LINE__, \
				((struct ExtGadget *) Member->gbm_Object)->Width, \
				((struct ExtGadget *) Member->gbm_Object)->BoundsWidth));
			}

		return 0;
		}

	inst->gbcl_BorderLeft = gpl->gpl_GInfo->gi_Window->BorderLeft;
	inst->gbcl_BorderRight = gpl->gpl_GInfo->gi_Window->BorderRight;

	gg->LeftEdge = gg->BoundsLeftEdge = gpl->gpl_GInfo->gi_Window->BorderLeft;
	gg->Width = gg->BoundsWidth = gpl->gpl_GInfo->gi_Window->Width - 
		gpl->gpl_GInfo->gi_Window->BorderLeft - gpl->gpl_GInfo->gi_Window->BorderRight;

	d1(kprintf("%s/%s/%ld: gg=%08lx  gg_Width=%ld  gg_Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg, gg->Width, gg->Height));

	Height = 0;

	d1(KPrintF("%s/%s/%ld: win=%08lx  winWidth=%ld  BorderLeft=%ld  BorderRight=%ld\n", \
			__FILE__, __FUNC__, __LINE__, gpl->gpl_GInfo->gi_Window, \
			gpl->gpl_GInfo->gi_Window->Width, \
                        gpl->gpl_GInfo->gi_Window->BorderLeft,\
			gpl->gpl_GInfo->gi_Window->BorderRight));

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
		Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
		{
		ULONG MemberHeight = 0;
		ULONG TotalHeight;

		Member->gbm_Flags &= ~GBMFLAGF_Skipped;

		SetAttrs(Member->gbm_Object,
			GBDTA_Hidden, Member->gbm_Flags & GBMFLAGF_Skipped,
			TAG_END);
/*
		{
		struct IBox MinimumBox = { 0, 0, 0, 0 };

		DoMethod(Member->gbm_Object,
			GM_DOMAIN,
			gpl->gpl_GInfo,
			gpl->gpl_GInfo->gi_RastPort,
			&MinimumBox,
			GDOMAIN_MINIMUM,
			NULL);

		d1(KPrintF("%s/%s/%ld: GDOMAIN_MINIMUM gg=%08lx  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, Member->gbm_Object, MinimumBox.Left, MinimumBox.Top, MinimumBox.Width, MinimumBox.Height));
		}
*/
		DoMethod(Member->gbm_Object,
			GM_LAYOUT,
			gpl->gpl_GInfo,
			gpl->gpl_Initial);

		GetAttr(GA_Height, Member->gbm_Object, &MemberHeight);

		TotalHeight = MemberHeight + inst->gbcl_TopSpace + inst->gbcl_BottomSpace;

		d1(kprintf("%s/%s/%ld: Member=%08lx  Object=%08lx  Height=%ld\n", __FILE__, __FUNC__, __LINE__, Member, Member->gbm_Object, MemberHeight));
		d1(kprintf("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, MemberHeight));

		if (TotalHeight > Height)
			Height = TotalHeight;
		}

	Left = inst->gbcl_LeftSpace;
	FreeSpace = gg->Width - inst->gbcl_LeftSpace - inst->gbcl_RightSpace;
	TotalWeight = 0;
	d1(KPrintF("%s/%s/%ld: gg->Width=%ld  gbcl_LeftSpace=%ld  gbcl_RightSpace=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, inst->gbcl_LeftSpace, inst->gbcl_RightSpace));
	d1(KPrintF("%s/%s/%ld: Height=%ld  FreeSpace=%ld\n", __FILE__, __FUNC__, __LINE__, Height, FreeSpace));

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
		Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
		{
		ULONG imgWidth = 0;

		GetAttr(GA_Width, Member->gbm_Object, &imgWidth);

		if (GB_FIXED_WIDTH == Member->gbm_Weight)
			FreeSpace -= imgWidth + inst->gbcl_BetweenSpace;
		else
			TotalWeight += Member->gbm_Weight;

		d1(KPrintF("%s/%s/%ld: FreeSpace=%ld  imgWidth=%ld\n", __FILE__, __FUNC__, __LINE__, FreeSpace, imgWidth));
		}
	d1(KPrintF("%s/%s/%ld: FreeSpace=%ld\n", __FILE__, __FUNC__, __LINE__, FreeSpace));

	if (FreeSpace < 0)
		{
		// We don't have enough width to fit all gadgets.
		// Now we attempt so skip some gadgets, in right-to-left order,
		// until all remaining gadgets fit into the available width.
		for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_TailPred;
			FreeSpace < 0 && Member->gbm_Node.ln_Pred;
			Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Pred)
			{
			if (GB_FIXED_WIDTH == Member->gbm_Weight)
				{
				ULONG imgWidth = 0;

				GetAttr(GA_Width, Member->gbm_Object, &imgWidth);

				Member->gbm_Flags |= GBMFLAGF_Skipped;

				SetAttrs(Member->gbm_Object,
					GBDTA_Hidden, Member->gbm_Flags & GBMFLAGF_Skipped,
					TAG_END);

                                FreeSpace += imgWidth + inst->gbcl_BetweenSpace;
				d1(KPrintF("%s/%s/%ld: SKIP Member=%08lx  FreeSpace=%ld\n", __FILE__, __FUNC__, __LINE__, Member, FreeSpace));
				}
			}

		// Skip variable-width gadgets when they will get too narrow
		for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_TailPred;
			Member->gbm_Node.ln_Pred;
			Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Pred)
			{
			if (GB_FIXED_WIDTH != Member->gbm_Weight)
				{
				LONG imgWidth = (FreeSpace * Member->gbm_Weight) / TotalWeight;

				if (imgWidth < 5)
					{
					Member->gbm_Flags |= GBMFLAGF_Skipped;

					SetAttrs(Member->gbm_Object,
						GBDTA_Hidden, Member->gbm_Flags & GBMFLAGF_Skipped,
						TAG_END);

					
                                        TotalWeight -= Member->gbm_Weight;
					d1(KPrintF("%s/%s/%ld: SKIP Member=%08lx  FreeSpace=%ld\n", __FILE__, __FUNC__, __LINE__, Member, FreeSpace));
					}
				}
			}
		}

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
		Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
		{
		if (!(Member->gbm_Flags & GBMFLAGF_Skipped))
			{
			ULONG imgWidth = 0;
			ULONG imgHeight = 0;

			GetAttr(GA_Height, Member->gbm_Object, &imgHeight);
			GetAttr(GA_Width, Member->gbm_Object, &imgWidth);

			Member->gbm_LeftOffset = Left;
			d1(KPrintF("%s/%s/%ld: Left=%ld  imgWidth=%ld\n", __FILE__, __FUNC__, __LINE__, Left, imgWidth));

			if (GB_FIXED_WIDTH != Member->gbm_Weight)
				{
				struct ExtGadget *ggm = (struct ExtGadget *) Member->gbm_Object;

				// Calculate member width relative to parent width
				imgWidth = (FreeSpace * Member->gbm_Weight) / TotalWeight;

				d1(KPrintF("%s/%s/%ld: totalWeight=%ld  imgWidth=%ld  FreeSpace=%ld  gbm_Weight=%ld\n", \
					__FILE__, __FUNC__, __LINE__, TotalWeight, imgWidth, FreeSpace, Member->gbm_Weight));

				if (imgWidth > FreeSpace)
					imgWidth = FreeSpace;

				SetAttrs(Member->gbm_Object,
					GA_Width, imgWidth,
					TAG_END);

				ggm->BoundsWidth = ggm->Width;
				ggm->BoundsHeight = ggm->Height;
				ggm->BoundsLeftEdge = ggm->LeftEdge;
				ggm->BoundsTopEdge = ggm->TopEdge;

				d1(KPrintF("%s/%s/%ld: imgWidth=%ld  gg->Width=%ld  BoundsWidth=%ld\n", __FILE__, __FUNC__, __LINE__, \
					imgWidth, ggm->Width, ggm->BoundsWidth));
				d1(KPrintF("%s/%s/%ld: bx=%ld  by=%ld  bw=%ld  bh=%ld\n", \
					__FILE__, __FUNC__, __LINE__, ggm->BoundsLeftEdge, ggm->BoundsTopEdge, ggm->BoundsWidth, ggm->BoundsHeight));
				}

			Left += imgWidth + inst->gbcl_BetweenSpace;
			}
		}

	if (Height > gg->Height)
		{
		struct internalScaWindowTask *iwt = inst->gbcl_WindowTask;

		switch (inst->gbcl_Position)
			{
		case GBPOS_Top:
			iwt->iwt_GadgetBarHeightTop = Height;
			break;
		case GBPOS_Bottom:
			iwt->iwt_GadgetBarHeightBottom = Height;
			break;
			}

		gg->Height = gg->BoundsHeight = Height;
		d1(KPrintF("%s/%s/%ld: Height=%ld  gg->Height=%ld\n", __FILE__, __FUNC__, __LINE__, Height, gg->Height));

		if (iwt->iwt_WindowTask.mt_WindowObject)
			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetInnerSize);
		}
	d1(KPrintF("%s/%s/%ld: Height=%ld  gg->Height=%ld\n", __FILE__, __FUNC__, __LINE__, Height, gg->Height));

	switch (inst->gbcl_Position)
		{
	case GBPOS_Top:
		gg->TopEdge = gg->BoundsTopEdge = gpl->gpl_GInfo->gi_Window->BorderTop;
		break;
	case GBPOS_Bottom:
		gg->TopEdge = gg->BoundsTopEdge = gpl->gpl_GInfo->gi_Window->Height - gpl->gpl_GInfo->gi_Window->BorderBottom - gg->Height;
		break;
		}

	d1(kprintf("%s/%s/%ld: gpl_GInfo=%08lx  gi_Window=%08lx\n", __FILE__, __FUNC__, __LINE__, gpl->gpl_GInfo, gpl->gpl_GInfo->gi_Window));
	d1(kprintf("%s/%s/%ld: gg=%08lx  LeftEdge=%ld  TopEdge=%ld\n", __FILE__, __FUNC__, __LINE__, gg, gg->LeftEdge, gg->TopEdge));

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
		Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
		{
		if (!(Member->gbm_Flags & GBMFLAGF_Skipped))
			{
			ULONG imgWidth = 0, imgHeight = 0;

			GetAttr(GA_Height, Member->gbm_Object, &imgHeight);
			GetAttr(GA_Width, Member->gbm_Object, &imgWidth);

			Member->gbm_TopOffset = (gg->Height - imgHeight) / 2;

			Member->gbm_Bounds.MinX = Member->gbm_Bounds.MaxX = gg->LeftEdge + Member->gbm_LeftOffset;
			Member->gbm_Bounds.MinY = Member->gbm_Bounds.MaxY = gg->TopEdge + Member->gbm_TopOffset;
			Member->gbm_Bounds.MaxX += imgWidth;
			Member->gbm_Bounds.MaxY += imgHeight;

			SetAttrs(Member->gbm_Object, 
				GA_Left, Member->gbm_Bounds.MinX,
				GA_Top,  Member->gbm_Bounds.MinY,
				TAG_END);

			d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
				Member->gbm_Bounds.MinX, Member->gbm_Bounds.MinY, imgWidth, imgHeight));
			d1(KPrintF("%s/%s/%ld: gbm_LeftOffset=%ld\n", __FILE__, __FUNC__, __LINE__, Member->gbm_LeftOffset));

			// Re-layout with final position
			DoMethod(Member->gbm_Object,
				GM_LAYOUT,
				gpl->gpl_GInfo,
				TRUE);
			}
		}

	if (inst->gbcl_WindowLeftBorder != gpl->gpl_GInfo->gi_Window->BorderLeft)
		{
		if (inst->gbcl_WindowLeftBorder)
			{
			// window border changed since last call
			RefreshWindowFrame(gpl->gpl_GInfo->gi_Window);
			}

		inst->gbcl_WindowLeftBorder = gpl->gpl_GInfo->gi_Window->BorderLeft;
		}

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_Render(Class *cl, Object *o, Msg msg)
{
	struct gpRender *gpr = (struct gpRender *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct GadgetBarMember *Member;
	struct Region *OldClipRegion;

	d1(KPrintF("%s/%s/%ld: START  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
	d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld  gpr_Redraw=%ld\n", __FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge, gpr->gpr_Redraw));

	OldClipRegion = InstallClipRegion(gpr->gpr_RPort->Layer, NULL);

	if (inst->gbcl_BorderLeft != gpr->gpr_GInfo->gi_Window->BorderLeft
		|| inst->gbcl_BorderRight != gpr->gpr_GInfo->gi_Window->BorderRight)
		{
		// Re-Layout gadget if window border has changed
		DoMethod(o, GM_LAYOUT, gpr->gpr_GInfo, FALSE);
		}

	if (gg->Height != 0 && gpr->gpr_GInfo)
		{
		EraseGadgetBarBackground(inst, gpr->gpr_RPort, gg);

		// Draw border around GadgetBar
		SetAPen(gpr->gpr_RPort, gpr->gpr_GInfo->gi_DrInfo->dri_Pens[SHINEPEN]);
		Move(gpr->gpr_RPort, gg->LeftEdge, gg->TopEdge + gg->Height - 1);
		Draw(gpr->gpr_RPort, gg->LeftEdge, gg->TopEdge);
		Draw(gpr->gpr_RPort, gg->LeftEdge + gg->Width - 1, gg->TopEdge);

		SetAPen(gpr->gpr_RPort, gpr->gpr_GInfo->gi_DrInfo->dri_Pens[SHADOWPEN]);
		Move(gpr->gpr_RPort, gg->LeftEdge + gg->Width - 1, gg->TopEdge);
		Draw(gpr->gpr_RPort, gg->LeftEdge + gg->Width - 1, gg->TopEdge + gg->Height - 1);
		Draw(gpr->gpr_RPort, gg->LeftEdge, gg->TopEdge + gg->Height - 1);

		// Render all members
		for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
			Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
			Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
			{
			d1(kprintf("%s/%s/%ld: Member=%08lx  Object=%08lx\n", __FILE__, __FUNC__, __LINE__, Member, Member->gbm_Object));

			if (!(Member->gbm_Flags & GBMFLAGF_Skipped))
				{
				DoMethod(Member->gbm_Object,
					GM_RENDER,
					gpr->gpr_GInfo,
					gpr->gpr_RPort,
					GREDRAW_REDRAW);
				}
			}
		}

	InstallClipRegion(gpr->gpr_RPort->Layer, OldClipRegion);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_AddMember(Class *cl, Object *o, Msg msg)
{
	struct opGBAddMember *opam = (struct opGBAddMember *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct GadgetBarMember *Member;

	if (NULL == opam->opam_Object)
		return 0;

	Member = ScalosAlloc(sizeof(struct GadgetBarMember));
	if (NULL == Member)
		return 0;

	Member->gbm_Object = opam->opam_Object;
	Member->gbm_Weight = GetTagData(GBDTA_Weight, GB_FIXED_WIDTH, opam->opam_AttrList);

	d1(kprintf("%s/%s/%ld: Object=%08lx  Weight=%ld\n", __FILE__, __FUNC__, __LINE__, Member->gbm_Object, Member->gbm_Weight));

	AddHead(&inst->gbcl_MemberList, &Member->gbm_Node); 
	DoMethod(o, GM_LAYOUT, opam->opam_GInfo, FALSE);

	// Forward backfill hook to child
	SetAttrs(Member->gbm_Object,
		GBDTA_BackfillHook, &inst->gbcl_BackFillHook,
		TAG_END);

	d1(KPrintF("%s/%s/%ld:  o=%08lx  gbcl_BackFillHook=%08lx  h_Entry=%08lx  h_SubEntry=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, o, &inst->gbcl_BackFillHook, \
		inst->gbcl_BackFillHook.h_Entry, inst->gbcl_BackFillHook.h_SubEntry));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_RemMember(Class *cl, Object *o, Msg msg)
{
	struct opGBRemMember *oprm = (struct opGBRemMember *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct GadgetBarMember *Member, *NextMember;

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
		Member = NextMember)
		{
		NextMember = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ;

		if (Member->gbm_Object == oprm->oprm_Object)
			{
			Remove(&Member->gbm_Node);

			if (inst->gbcl_MemberHit == Member)
				inst->gbcl_MemberHit = NULL;
			if (inst->gbcl_ActiveMember == Member)
				inst->gbcl_ActiveMember = NULL;

			DoMethod(Member->gbm_Object, DTM_REMOVEDTOBJECT, oprm->oprm_GInfo);

			DisposeObject(Member->gbm_Object);
			ScalosFree(Member);

			DoMethod(o, GM_LAYOUT, oprm->oprm_GInfo, FALSE);

			return 1;
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_UpdateMember(Class *cl, Object *o, Msg msg)
{
	struct opGBUpdateMember *opum = (struct opGBUpdateMember *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct GadgetBarMember *Member;

	d1(kprintf("%s/%s/%ld: Object=%08lx  Member=%08lx\n", __FILE__, __FUNC__, __LINE__, o, opum->opum_Object));

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
		Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
		{
		if (Member->gbm_Object == opum->opum_Object && !(Member->gbm_Flags & GBMFLAGF_Skipped))
			{
			struct RastPort *rp;

			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
			rp = ObtainGIRPort(opum->opum_GInfo);

			if (rp)
				{
				EraseGadgetBarBackground(inst, rp, (struct ExtGadget *) Member->gbm_Object);

				DoMethod(Member->gbm_Object,
					GM_LAYOUT,
					opum->opum_GInfo,
					FALSE);
				DoMethod(Member->gbm_Object,
					GM_RENDER,
					opum->opum_GInfo,
					rp,
					GREDRAW_REDRAW);

				ReleaseGIRPort(rp);
				}
			return 1;
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_HelpTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct GadgetBarMember *Member;

	d1(KPrintF("%s/%s/%ld: Object=%08lx  bx=%ld by=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, gg->BoundsLeftEdge, gg->BoundsTopEdge));
	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y));

	inst->gbcl_MemberHit = NULL;

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
		Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
		{
		struct ExtGadget *ggm = (struct ExtGadget *) Member->gbm_Object;

		d1(KPrintF("%s/%s/%ld: x=%ld y=%ld  w=%ld  h=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ggm->LeftEdge, ggm->TopEdge, ggm->Width, ggm->Height));
		d1(kprintf("%s/%s/%ld: bx=%ld by=%ld  bw=%ld  bh=%ld\n", \
			__FILE__, __FUNC__, __LINE__, ggm->BoundsLeftEdge, ggm->BoundsTopEdge,\
			ggm->BoundsWidth, ggm->BoundsHeight));

		if (!(Member->gbm_Flags & GBMFLAGF_Skipped) &&
			GadgetBar_PointInGadget(gg, ggm, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y))
			{
			ULONG Result;
			struct gpHitTest gphtCopy = *gpht;

			gphtCopy.gpht_Mouse.X -= Member->gbm_LeftOffset;
			gphtCopy.gpht_Mouse.Y -= Member->gbm_TopOffset;

			d1(KPrintF("%s/%s/%ld: x=%ld y=%ld  w=%ld  h=%ld\n", \
				__FILE__, __FUNC__, __LINE__, ggm->LeftEdge, ggm->TopEdge, ggm->Width, ggm->Height));

			Result = DoMethodA(Member->gbm_Object, (Msg)(APTR) &gphtCopy);

			if (GMR_HELPHIT == Result)
				{
				inst->gbcl_MemberHit = Member;
				Result = GMR_HELPCODE | ggm->GadgetID;
				}

			d1(KPrintF("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

			return Result;
			}
		}

	d1(KPrintF("%s/%s/%ld: Result=GMR_NOHELPHIT\n", __FILE__, __FUNC__, __LINE__));

	return GMR_NOHELPHIT;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_HitTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct GadgetBarMember *Member;
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld: START Object=%08lx  bx=%ld by=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, gg->BoundsLeftEdge, gg->BoundsTopEdge));
	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y));

	inst->gbcl_MemberHit = NULL;

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail; 
		Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
		{
		struct ExtGadget *ggm = (struct ExtGadget *) Member->gbm_Object;

		d1(KPrintF("%s/%s/%ld: Member=%08lx  gbm_LeftOffset=%ld  gbm_TopOffset=%ld\n", \
			__FILE__, __FUNC__, __LINE__, Member, Member->gbm_LeftOffset, Member->gbm_TopOffset));

		if (!(Member->gbm_Flags & GBMFLAGF_Skipped) &&
			!(ggm->Flags & GFLG_DISABLED) &&
			GadgetBar_PointInGadget(gg, ggm, gpht->gpht_Mouse.X, gpht->gpht_Mouse.Y))
			{
			ULONG Result;
			struct gpHitTest gphtCopy = *gpht;

			d1(KPrintF("%s/%s/%ld: PointInGadget  Member=%08lx\n", __FILE__, __FUNC__, __LINE__, Member));

			// gpht_Mouse is always relative to Gadget left/top
			gphtCopy.gpht_Mouse.X -= Member->gbm_LeftOffset;
			gphtCopy.gpht_Mouse.Y -= Member->gbm_TopOffset;

			Result = DoMethodA(Member->gbm_Object, (Msg)(APTR) &gphtCopy);

			if (GMR_GADGETHIT == Result)
				inst->gbcl_MemberHit = Member;

			d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

			return Result;
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_GoActive(Class *cl, Object *o, Msg msg)
{
	struct gpInput *gpi = (struct gpInput *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	ULONG Result = 0;

	d1(KPrintF("%s/%s/%ld: START Object=%08lx  gbcl_ActiveMember=%08lx  gbcl_MemberHit=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, o, inst->gbcl_ActiveMember, inst->gbcl_MemberHit));
	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y));

	if (inst->gbcl_ActiveMember)
		{
		DoMethod(inst->gbcl_ActiveMember->gbm_Object, 
			GM_GOINACTIVE, gpi->gpi_GInfo, TRUE);
		inst->gbcl_ActiveMember = NULL;
		}

	if (inst->gbcl_MemberHit)
		{
		struct gpInput gpiCopy = *gpi;
		struct ExtGadget *ggm = (struct ExtGadget *) inst->gbcl_MemberHit->gbm_Object;

		if (!(ggm->Flags & GFLG_DISABLED))
			{
			gpiCopy.gpi_Mouse.X -= inst->gbcl_MemberHit->gbm_LeftOffset;
			gpiCopy.gpi_Mouse.Y -= inst->gbcl_MemberHit->gbm_TopOffset;

			inst->gbcl_LastActiveMember = inst->gbcl_MemberHit;

			d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", \
				__FILE__, __FUNC__, __LINE__, gpiCopy.gpi_Mouse.X, gpiCopy.gpi_Mouse.Y));

			Result = DoMethodA(inst->gbcl_MemberHit->gbm_Object, (Msg)(APTR) &gpiCopy);

			inst->gbcl_ActiveMember = inst->gbcl_MemberHit;
			}
		inst->gbcl_MemberHit = NULL;
		}

	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_GoInactive(Class *cl, Object *o, Msg msg)
{
//	struct gpGoInactive *gpgi = (struct gpGoInactive *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	ULONG Result = 0;

	d1(KPrintF("%s/%s/%ld: START Object=%08lx  gbcl_ActiveMember=%08lx  gbcl_MemberHit=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, o, inst->gbcl_ActiveMember, inst->gbcl_MemberHit));

	if (inst->gbcl_ActiveMember)
		{
		Result = DoMethodA(inst->gbcl_ActiveMember->gbm_Object, msg);
		inst->gbcl_ActiveMember = NULL;
		}

	inst->gbcl_MemberHit = NULL;

	return Result;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_HandleInput(Class *cl, Object *o, Msg msg)
{
	struct gpInput *gpi = (struct gpInput *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	ULONG Result = 0;

	inst->gbcl_MemberHit = NULL;

	d1(KPrintF("%s/%s/%ld: START Object=%08lx  gbcl_ActiveMember=%08lx  gbcl_MemberHit=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, o, inst->gbcl_ActiveMember, inst->gbcl_MemberHit));
	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y));

	if (inst->gbcl_ActiveMember)
		{
		struct gpInput gpiCopy = *gpi;

		gpiCopy.gpi_Mouse.X -= inst->gbcl_ActiveMember->gbm_LeftOffset;
		gpiCopy.gpi_Mouse.Y -= inst->gbcl_ActiveMember->gbm_TopOffset;

		d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", \
			__FILE__, __FUNC__, __LINE__, gpiCopy.gpi_Mouse.X, gpiCopy.gpi_Mouse.Y));

		Result = DoMethodA(inst->gbcl_ActiveMember->gbm_Object, (Msg)(APTR) &gpiCopy);
		}

	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------

static ULONG GadgetBar_HandleMouseWheel(Class *cl, Object *o, Msg msg)
{
	struct gpInput *gpi = (struct gpInput *) msg;
	struct GadgetBarClassInst *inst = INST_DATA(cl, o);
	struct GadgetBarMember *Member;
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld: START Object=%08lx  bx=%ld by=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, gg->BoundsLeftEdge, gg->BoundsTopEdge));
	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y));

	for (Member = (struct GadgetBarMember *) inst->gbcl_MemberList.lh_Head;
		Member != (struct GadgetBarMember *) &inst->gbcl_MemberList.lh_Tail;
		Member = (struct GadgetBarMember *) Member->gbm_Node.ln_Succ)
		{
		struct ExtGadget *ggm = (struct ExtGadget *) Member->gbm_Object;

		d1(KPrintF("%s/%s/%ld: Member=%08lx  gbm_LeftOffset=%ld  gbm_TopOffset=%ld\n", \
			__FILE__, __FUNC__, __LINE__, Member, Member->gbm_LeftOffset, Member->gbm_TopOffset));

		if (!(Member->gbm_Flags & GBMFLAGF_Skipped) &&
			GadgetBar_PointInGadget(gg, ggm, gpi->gpi_Mouse.X, gpi->gpi_Mouse.Y))
			{
			struct gpInput gpiCopy = *gpi;

			d1(KPrintF("%s/%s/%ld: Member=%08lx\n", __FILE__, __FUNC__, __LINE__, Member));

			gpiCopy.gpi_Mouse.X -= Member->gbm_LeftOffset;
			gpiCopy.gpi_Mouse.Y -= Member->gbm_TopOffset;

			d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld\n", \
				__FILE__, __FUNC__, __LINE__, gpiCopy.gpi_Mouse.X, gpiCopy.gpi_Mouse.Y));

			DoMethodA(Member->gbm_Object, (Msg)(APTR) &gpiCopy);
			break;
			}
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return 0;
}


//----------------------------------------------------------------------------

static BOOL GadgetBar_PointInGadget(struct ExtGadget *Parent, struct ExtGadget *gg, WORD x, WORD y)
{
	ULONG Left, Top, Width, Height;

	GetAttr(GA_Left, (Object *) Parent, &Left);
	GetAttr(GA_Top, (Object *) Parent, &Top);

	x += Left;
	y += Top;

	GetAttr(GA_Left, (Object *) gg, &Left);
	GetAttr(GA_Top, (Object *) gg, &Top);
	GetAttr(GA_Width, (Object *) gg, &Width);
	GetAttr(GA_Height, (Object *) gg, &Height);

	d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, x, y, Left, Top, Width, Height));

	if (x < Left || x >= Left + Width)
		return FALSE;
	if (y < Top || y >= Top + Height)
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------

static void EraseGadgetBarBackground(struct GadgetBarClassInst *inst, 
	struct RastPort *rp, struct ExtGadget *gg)
{
	d1(kprintf("%s/%s/%ld: bx=%ld  by=%ld  bw=%ld  bh=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gg->BoundsLeftEdge, gg->BoundsTopEdge, gg->BoundsWidth, gg->BoundsHeight));

	if (inst->gbcl_Background)
		{
		FillBackground(rp, inst->gbcl_Background,
			gg->BoundsLeftEdge + 1, gg->BoundsTopEdge + 1,
			gg->BoundsLeftEdge + gg->BoundsWidth - 1 - 1,
			gg->BoundsTopEdge + gg->BoundsHeight - 1 - 1,
			(gg->BoundsLeftEdge + 1) % (gg->BoundsWidth - 1),
			(gg->BoundsTopEdge + 1) % (gg->BoundsHeight - 1));
		}
	else
		{
		SetAPen(rp, inst->gbcl_BGPen);
		RectFill(rp, 
			gg->BoundsLeftEdge + 1, gg->BoundsTopEdge + 1,
			gg->BoundsLeftEdge + gg->BoundsWidth - 1 - 1,
			gg->BoundsTopEdge + gg->BoundsHeight - 1 - 1);
		}
}

//----------------------------------------------------------------------------

// object == (struct RastPort *) result->RastPort
// message == [ (Layer *) layer, (struct Rectangle) bounds,
//              (LONG) offsetx, (LONG) offsety ]

static SAVEDS(ULONG) GadgetBar_BackFillFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg)
{
	struct GadgetBarClassInst *inst = (struct GadgetBarClassInst *) bfHook->h_Data;

	d1(KPrintF("%s/%s/%ld: START RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, rp));
	d1(kprintf("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
	d1(kprintf("%s/%s/%ld: Layer=%08lx  OffsetX=%ld  OffsetY=%ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Layer, msg->bfm_OffsetX, msg->bfm_OffsetY));

	if (inst->gbcl_Background)
		{
		WindowBackFill(rp, msg, inst->gbcl_Background->dti_BitMap,
			inst->gbcl_Background->dti_BitMapHeader->bmh_Width,
			inst->gbcl_Background->dti_BitMapHeader->bmh_Height,
			iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[BACKGROUNDPEN],
			0, 0,
			NULL);
		}
	else
		{
		WindowBackFill(rp, msg, NULL,
			0, 0,
			iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[BACKGROUNDPEN],
			0, 0,
			NULL);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//----------------------------------------------------------------------------


