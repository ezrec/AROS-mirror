// McpFrameMCC.c
// $Date$


#include <stdlib.h>
#include <string.h>
#include <libraries/mui.h>
#include <intuition/imageclass.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>
#include <proto/intuition.h>

#include <defs.h>
#include "McpFrameMCC.h"
#include <ScalosMcpGfx.h>
#include "ScalosPrefs.h"

#ifdef __AROS__
// FIXME: temporary fix until we have figured out
// how to deal with these deprecated defines.
#define IA_ShadowPen    (IA_Dummy + 0x09)
#define IA_HighlightPen (IA_Dummy + 0x0A)
#endif

// -------------------------------------------------------------------------

#define CLASS	   	MUIC_MCPFrame
#define SUPERCLASS 	MUIC_Area

struct McpFrameMCCInstance
{
	struct Library *dtbase;

	UWORD FrameType;
	BOOL Recessed;
};

// -------------------------------------------------------------------------

static ULONG mNew(Class *cl, Object *o, struct opSet *ops);
static ULONG mDispose(Class *cl, Object *o,Msg msg);
static ULONG mSet(Class *cl, Object *o, struct opSet *ops);
static ULONG mGet(Class *cl, Object *o, struct opGet *opg);
static ULONG mAskMinMax(Class *cl, Object *o,struct MUIP_AskMinMax *msg);
static ULONG mDraw(Class *cl, Object *o,struct MUIP_Draw *msg);
static ULONG mDragQuery(Class *cl, Object *o, struct MUIP_DragQuery *msg);
static ULONG mDragDrop(Class *cl, Object *o, struct MUIP_DragDrop *msg);
DISPATCHER_PROTO(McpFrameMCC);

// -------------------------------------------------------------------------

static ULONG mNew(Class *cl, Object *o, struct opSet *ops)
{
	struct McpFrameMCCInstance *inst = NULL;
	BOOL Success = FALSE;

	do	{
		o = (Object *) DoSuperMethodA(cl, o, (Msg) ops);
		if (NULL == o)
			break;

		inst = INST_DATA(cl,o);

		memset(inst, 0, sizeof(struct McpFrameMCCInstance));

		inst->FrameType = GetTagData(MUIA_MCPFrame_FrameType, MCP_FRAME_NONE, ops->ops_AttrList);

		inst->Recessed = (inst->FrameType & MCP_FRAME_RECESSED) ? TRUE : FALSE;
		inst->FrameType &= ~MCP_FRAME_RECESSED;

		d1(KPrintF(__FUNC__ "/%ld: FrameType=%ld  Recessed=%ld\n", __LINE__, inst->FrameType, inst->Recessed));

		Success = TRUE;
		} while (0);

	if (!Success && o)
		{
		mDispose(cl, o, (Msg) ops);
		o = NULL;
		}

	return((ULONG)o);
}

// -------------------------------------------------------------------------

static ULONG mDispose(Class *cl, Object *o, Msg msg)
{
//	struct McpFrameMCCInstance *inst = INST_DATA(cl,o);

	return DoSuperMethodA(cl, o, msg);
}

// -------------------------------------------------------------------------

static ULONG mSet(Class *cl, Object *o, struct opSet *ops)
{
	struct McpFrameMCCInstance *inst = INST_DATA(cl,o);

	if (FindTagItem(MUIA_MCPFrame_FrameType, ops->ops_AttrList))
		{
		inst->FrameType = GetTagData(MUIA_MCPFrame_FrameType, 
			inst->FrameType | (inst->Recessed ? MCP_FRAME_RECESSED : 0), 
			ops->ops_AttrList);

		inst->Recessed = (inst->FrameType & MCP_FRAME_RECESSED) ? TRUE : FALSE;
		inst->FrameType &= ~MCP_FRAME_RECESSED;
		
		MUI_Redraw(o, MADF_DRAWOBJECT);
		}

	return DoSuperMethodA(cl, o, (Msg) ops);
}

// -------------------------------------------------------------------------

static ULONG mGet(Class *cl, Object *o, struct opGet *opg)
{
	struct McpFrameMCCInstance *inst = INST_DATA(cl,o);
	ULONG Result;

	switch (opg->opg_AttrID)
		{
	case MUIA_MCPFrame_FrameType:
		*opg->opg_Storage = inst->FrameType | (inst->Recessed ? MCP_FRAME_RECESSED : 0);
		Result = 1;
		break;
	default:
		Result = DoSuperMethodA(cl, o, (Msg) opg);
		break;
		}

	return Result;
}

// -------------------------------------------------------------------------

static ULONG mAskMinMax(Class *cl, Object *o,struct MUIP_AskMinMax *msg)
{
	struct McpFrameMCCInstance *inst = INST_DATA(cl,o);
	struct MUI_MinMax *mi;
	const struct FrameSize *fs;

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	DoSuperMethodA(cl, o, (Msg) msg);

	fs = McpGetFrameSize(inst->FrameType);

	mi = msg->MinMaxInfo;

	d1(KPrintF(__FUNC__ "/%ld: mi=%08lx  fs=%08lx FrameType=%04lx\n", __LINE__, mi, fs, inst->FrameType));

	if (fs)
		{
		LONG fsWidth;
		LONG fsHeight;

		if (inst->Recessed)
			{
			fsWidth = fs->fs_Sizes[FRAMESIZE_RELEFT] + fs->fs_Sizes[FRAMESIZE_RERIGHT] + 6;
			fsHeight = fs->fs_Sizes[FRAMESIZE_RETOP] + fs->fs_Sizes[FRAMESIZE_REBOTTOM] + 6;
			}
		else
			{
			fsWidth = fs->fs_Sizes[FRAMESIZE_LEFT] + fs->fs_Sizes[FRAMESIZE_RIGHT] + 6;
			fsHeight = fs->fs_Sizes[FRAMESIZE_TOP] + fs->fs_Sizes[FRAMESIZE_BOTTOM] + 6;
			}

		d1(KPrintF(__FUNC__ "/%ld: fsWidth=%ld  fsHeight=%ld\n", __LINE__, fsWidth, fsHeight));

		mi->MaxWidth = MUI_MAXMAX;
		mi->MaxHeight = MUI_MAXMAX;

		mi->MinWidth += fsWidth;
		mi->MaxWidth -= fsWidth;
		mi->DefWidth += fsWidth;

		mi->MinHeight += fsHeight;
		mi->MaxHeight -= fsHeight;
		mi->DefHeight += fsHeight;
		}

	d1(KPrintF(__FUNC__ "/%ld: fd=%08lx  MinW=%ld  MaxW=%ld  MinH=%ld  MaxH=%ld  DefW=%ld  DefH=%ld\n", \
		__LINE__, fs, mi->MinWidth, mi->MaxWidth, mi->MinHeight, mi->MaxHeight, mi->DefWidth, mi->DefHeight));
	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));

	return 0;
}

// -------------------------------------------------------------------------

static ULONG mDraw(Class *cl, Object *o,struct MUIP_Draw *msg)
{
	struct McpFrameMCCInstance *inst = INST_DATA(cl,o);

	DoSuperMethodA(cl, o, (Msg) msg);

	if (msg->flags & MADF_DRAWOBJECT)
		{
		McpGfxDrawFrame(_rp(o),
			_mleft(o), _mtop(o),
			_mright(o), _mbottom(o),
			IA_FrameType, inst->FrameType,
			IA_Recessed, inst->Recessed,
			IA_HighlightPen, _pens(o)[MPEN_SHINE],
			IA_ShadowPen, _pens(o)[MPEN_SHADOW],
			IA_BGPen, _pens(o)[MPEN_BACKGROUND],
			IA_HalfShinePen, _pens(o)[MPEN_HALFSHINE],
			IA_HalfShadowPen, _pens(o)[MPEN_HALFSHADOW],
			TAG_END);
		}

	return 0;
}

// -------------------------------------------------------------------------

static ULONG mDragQuery(Class *cl, Object *o, struct MUIP_DragQuery *drq)
{
//	struct McpFrameMCCInstance *inst = INST_DATA(cl,o);

	if (drq->obj == o)
		return DoSuperMethodA(cl, o, (Msg) drq);

	if (OCLASS(o) == cl)
		{
		return MUIV_DragQuery_Accept;
		}

	return DoSuperMethodA(cl, o, (Msg) drq);
}

// -------------------------------------------------------------------------

static ULONG mDragDrop(Class *cl, Object *o, struct MUIP_DragDrop *msg)
{
//	struct McpFrameMCCInstance *inst = INST_DATA(cl,o);

	if (msg->obj != o)
		{
		ULONG FrameType = 0;

		get(msg->obj, MUIA_MCPFrame_FrameType, &FrameType);
		SetAttrs(o,
			MUIA_MCPFrame_FrameType, FrameType,
			TAG_END);
		}

	return DoSuperMethodA(cl, o, (Msg) msg);
}

// -------------------------------------------------------------------------

DISPATCHER(McpFrameMCC)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		d1(kprintf(__FUNC__ "/%ld: OM_NEW\n", __LINE__));
		Result = mNew(cl, obj, (struct opSet *) msg);
		break;

	case OM_DISPOSE:
		d1(kprintf(__FUNC__ "/%ld: OM_DISPOSE\n", __LINE__));
		Result = mDispose(cl, obj, msg);
		break;

	case OM_SET:
		d1(kprintf(__FUNC__ "/%ld: OM_SET\n", __LINE__));
		Result = mSet(cl, obj, (struct opSet *) msg);
		break;

	case OM_GET:
		d1(kprintf(__FUNC__ "/%ld: OM_GET\n", __LINE__));
		Result = mGet(cl, obj, (struct opGet *) msg);
		break;

	case MUIM_AskMinMax:
		d1(kprintf(__FUNC__ "/%ld: MUIM_AskMinMax\n", __LINE__));
		Result = mAskMinMax(cl, obj, (struct MUIP_AskMinMax *) msg);
		break;

	case MUIM_Draw:
		d1(kprintf(__FUNC__ "/%ld: MUIM_Draw\n", __LINE__));
		Result = mDraw(cl, obj, (struct MUIP_Draw *) msg);
		break;

	case MUIM_DragQuery:
		d1(KPrintF(__FUNC__ "/%ld: MUIM_DragQuery\n", __LINE__));
		Result = mDragQuery(cl, obj, (struct MUIP_DragQuery *) msg);
		break;

	case MUIM_DragDrop:
		d1(KPrintF(__FUNC__ "/%ld: MUIM_DragDrop\n", __LINE__));
		Result = mDragDrop(cl, obj, (struct MUIP_DragDrop *) msg);
		break;

	default:
		d1(KPrintF(__FUNC__ "/%ld: MethodID=%08lx\n", __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
DISPATCHER_END

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitMcpFrameClass(void)
{
	return MUI_CreateCustomClass(NULL, SUPERCLASS, NULL, sizeof(struct McpFrameMCCInstance), DISPATCHER_REF(McpFrameMCC));
}

void CleanupMcpFrameClass(struct MUI_CustomClass *mcc)
{
	MUI_DeleteCustomClass(mcc);
}

// -------------------------------------------------------------------------

