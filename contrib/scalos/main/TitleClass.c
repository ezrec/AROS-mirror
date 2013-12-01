// TitleClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#if defined(__MORPHOS__)
#include <exec/system.h>
#endif /* __MORPHOS__ */
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
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
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "int64.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

// PowerPC CPU versions
#define	CPU_601		1
#define	CPU_603		3
#define	CPU_604		4
#define	CPU_602		5
#define	CPU_603e	6
#define	CPU_603p	7
#define	CPU_750		8
#define	CPU_604e	9
#define	CPU_604ev	10
#define	CPU_7400	12
#define	CPU_7410	12	/* revision 0x1xxx */
#define	CPU_7450	0x8000
#define	CPU_7455	0x8001
#define	CPU_7447	0x8002
#define	CPU_7447A	0x8003

struct TitleClassInstance
	{
	STRPTR tci_Buffer;
	CONST_STRPTR tci_TitleFormat;
	ULONG tci_BufferSize;
	};

#define	MAX_WindowTitleSize	600
#define	MAX_ScreenTitleSize	1000

#define	PARSEID(c1,c2)	(((c1) << 8) + (c2))

typedef void (*TITLEFUNC)(struct internalScaWindowTask *iwt, struct InfoData *id, 
	struct msg_Translate *mxl);

struct TitleFuncEntry
	{
	UWORD tfe_ParseID;
	TITLEFUNC tfe_Function;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) TitleClass_Dispatcher(Class *cl, Object *o, Msg msg);
static ULONG TitleClass_New(Class *cl, Object *o, Msg msg);
static ULONG TitleClass_Dispose(Class *cl, Object *o, Msg msg);
static ULONG TitleClass_Set(Class *cl, Object *o, Msg msg);
static ULONG TitleClass_Generate(Class *cl, Object *o, Msg msg);
static ULONG TitleClass_Translate(Class *cl, Object *o, Msg msg);
static ULONG TitleClass_QueryTitle(Class *cl, Object *o, Msg msg);
static ULONG TitleClass_Query(Class *cl, Object *o, Msg msg);

static void TTLFN_WBVersion(struct internalScaWindowTask *iwt, struct InfoData *id, 
	struct msg_Translate *mxl);
static void TTLFN_WBRealVersion(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_OSVersion(struct internalScaWindowTask *iwt, struct InfoData *id, 
	struct msg_Translate *mxl);
static void TTLFN_OSRealVersion(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_Path(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_ChipRAM(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_FastRAM(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_TotalRAM(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_DiskFree(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_DiskFreeKMG(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_DiskUsed(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_DiskUsedKMG(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_DiskInsertedStart(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_DiskInsertedEnd(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_DiskPercent(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_ChipSet(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_Processor(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_CoProcessor(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_NumTasks(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_NumLibraries(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_NumPorts(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_NumDevices(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);
static void TTLFN_NumScreens(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl);

static ULONG GetNodeCount(struct List *list);
static void TitleClassGetDiskInfo(struct InfoData *id);

//----------------------------------------------------------------------------

// external data items :

extern struct ExecBase * SysBase;

//----------------------------------------------------------------------------

// local data items :

static const struct TitleFuncEntry TitleFunctionTable[] =
	{
	{ PARSEID('o','s'), TTLFN_OSVersion },
	{ PARSEID('w','b'), TTLFN_WBVersion },
	{ PARSEID('o','v'), TTLFN_OSRealVersion },
	{ PARSEID('w','v'), TTLFN_WBRealVersion },
	{ PARSEID('f','c'), TTLFN_ChipRAM },
	{ PARSEID('f','f'), TTLFN_FastRAM },
	{ PARSEID('f','t'), TTLFN_TotalRAM },
	{ PARSEID('p','a'), TTLFN_Path },
	{ PARSEID('d','f'), TTLFN_DiskFree },
	{ PARSEID('D','F'), TTLFN_DiskFreeKMG },
	{ PARSEID('d','u'), TTLFN_DiskUsed },
	{ PARSEID('D','U'), TTLFN_DiskUsedKMG },
	{ PARSEID('d','('), TTLFN_DiskInsertedStart },
	{ PARSEID('d',')'), TTLFN_DiskInsertedEnd },
	{ PARSEID('d','p'), TTLFN_DiskPercent },
	{ PARSEID('c','s'), TTLFN_ChipSet },
	{ PARSEID('c','p'), TTLFN_CoProcessor },
	{ PARSEID('p','r'), TTLFN_Processor },
	{ PARSEID('n','t'), TTLFN_NumTasks },
	{ PARSEID('n','l'), TTLFN_NumLibraries },
	{ PARSEID('n','p'), TTLFN_NumPorts },
	{ PARSEID('n','d'), TTLFN_NumDevices },
	{ PARSEID('n','s'), TTLFN_NumScreens },
	{ 0, NULL },
	};

static UWORD TitleQueryTable[] =
	{
	PARSEID('f','c'),
	PARSEID('f','f'),
	PARSEID('f','t'),
	PARSEID('d','f'),
	PARSEID('D','F'),
	PARSEID('d','u'),
	PARSEID('D','U'),
	PARSEID('d','('),
	PARSEID('d',')'),
	PARSEID('d','p'),
	PARSEID('n','t'),
	PARSEID('n','l'),
	PARSEID('n','p'),
	PARSEID('n','d'),
	PARSEID('n','s'),
	0
	};

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------


struct ScalosClass *initTitleClass(const struct PluginClass *plug)
{
	struct ScalosClass *TitleClass;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	TitleClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct TitleClassInstance),
			NULL);

	if (TitleClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(TitleClass->sccl_class->cl_Dispatcher,  TitleClass_Dispatcher);

		d1(kprintf("%s/%s/%ld: dispatcher: entry=%08lx  subentry=%08lx  data=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, TitleClass->sccl_class->cl_Dispatcher.h_Entry, \
			TitleClass->sccl_class->cl_Dispatcher.h_SubEntry, \
			TitleClass->sccl_class->cl_Dispatcher.h_Data));
		}

	d1(kprintf("%s/%s/%ld: TitleClass=%08lx\n", __FILE__, __FUNC__, __LINE__, TitleClass));

	return TitleClass;
}


static SAVEDS(ULONG) TitleClass_Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(KPrintF("%s/%s/%ld: cl=%08lx  o=%08lx  msg=%08lx  MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = TitleClass_New(cl, o, msg);
		break;

	case OM_DISPOSE:
		Result = TitleClass_Dispose(cl, o, msg);
		break;

	case OM_SET:
		Result = TitleClass_Set(cl, o, msg);
		break;

	case SCCM_Title_Generate:
		Result = TitleClass_Generate(cl, o, msg);
		break;

	case SCCM_Title_Translate:
		Result = TitleClass_Translate(cl, o, msg);
		break;

	case SCCM_Title_QueryTitle:
		Result = TitleClass_QueryTitle(cl, o, msg);
		break;

	case SCCM_Title_Query:
		Result = TitleClass_Query(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	d1(KPrintF("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static ULONG TitleClass_New(Class *cl, Object *o, Msg msg)
{
	o = (Object *) DoSuperMethodA(cl, o, msg);

	d1(kprintf("%s/%s/%ld: o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
	d1(kprintf("%s/%s/%ld: InstOffset=%ld  InstSize=%ld\n", __FILE__, __FUNC__, __LINE__, \
		cl->cl_InstOffset, cl->cl_InstSize));

	if (o)	
		{
		struct opSet *ops = (struct opSet *) msg;
		struct TitleClassInstance *inst = INST_DATA(cl, o);
		ULONG TitleType;

		memset(inst, 0, sizeof(struct TitleClassInstance));

		inst->tci_TitleFormat = (CONST_STRPTR) GetTagData(SCCA_Title_Format, 0, ops->ops_AttrList);

		TitleType = GetTagData(SCCA_Title_Type, SCCV_Title_Type_Screen, ops->ops_AttrList);
		switch (TitleType)
			{
		case SCCV_Title_Type_Window:
			inst->tci_BufferSize = MAX_WindowTitleSize;
			break;
		case SCCV_Title_Type_Screen:
			inst->tci_BufferSize = MAX_ScreenTitleSize;
			break;
			}

		d1(kprintf("%s/%s/%ld: tci_TitleFormat=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->tci_TitleFormat));

		inst->tci_Buffer = ScalosAlloc(inst->tci_BufferSize);

		d1(kprintf("%s/%s/%ld: tci_Buffer=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, inst->tci_Buffer, inst->tci_BufferSize));
		if (NULL == inst->tci_Buffer)
			{
			DoMethod(o, OM_DISPOSE);
			o = NULL;
			}

		}

	return (ULONG) o;
}


static ULONG TitleClass_Dispose(Class *cl, Object *o, Msg msg)
{
	struct TitleClassInstance *inst = INST_DATA(cl, o);

	d1(kprintf("%s/%s/%ld: o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	if (inst->tci_Buffer)
		{
		ScalosFree(inst->tci_Buffer);
		inst->tci_Buffer = NULL;
		}

	return DoSuperMethodA(cl, o, msg);
}


static ULONG TitleClass_Set(Class *cl, Object *o, Msg msg)
{
	struct TitleClassInstance *inst = INST_DATA(cl, o);
	struct opSet *ops = (struct opSet *) msg;

	d1(kprintf("%s/%s/%ld: o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	inst->tci_TitleFormat = (CONST_STRPTR) GetTagData(SCCA_Title_Format, (ULONG) inst->tci_TitleFormat, ops->ops_AttrList);

	d1(kprintf("%s/%s/%ld: tci_TitleFormat=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->tci_TitleFormat));

	return 1;
}


static ULONG TitleClass_Generate(Class *cl, Object *o, Msg msg)
{
	struct TitleClassInstance *inst = INST_DATA(cl, o);
	CONST_STRPTR tlp;
	STRPTR blp;
	size_t Len = inst->tci_BufferSize - 20;

	d1(kprintf("%s/%s/%ld: o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	memset(inst->tci_Buffer, 0, Len);

	for (tlp=inst->tci_TitleFormat, blp=inst->tci_Buffer; Len && *tlp; )
		{
		if ('%' == tlp[0] && tlp[1] && tlp[2])
			{
			d1(kprintf("%s/%s/%ld: tlp=<%s>\n", __FILE__, __FUNC__, __LINE__, tlp));

			tlp++;		// skip "%"

			if ('%' == *tlp)
				{
				// Generate "%"
				*blp++ = *tlp++;
				Len--;
				}
			else
				{
				struct msg_Translate mxl;

				mxl.mxl_ParseID = PARSEID(tlp[0], tlp[1]);
				tlp += 2;
				mxl.mxl_MethodID = SCCM_Title_Translate;
				mxl.mxl_Buffer = blp;
				mxl.mxl_TitleFormat = tlp;
				mxl.mxl_BuffLen = Len;

				if (DoMethodA(o, (Msg)(APTR) &mxl))
					{
					Len -= mxl.mxl_Buffer - blp;
					blp = mxl.mxl_Buffer;
					tlp = mxl.mxl_TitleFormat;
					}
				else
					{
					d1(kprintf("%s/%s/%ld: FAIL\n", __FILE__, __FUNC__, __LINE__));
					if (Len >= 3)
						{
						*blp++ = '%';
						*blp++ = (char) (mxl.mxl_ParseID >> 8);
						*blp++ = (char) (mxl.mxl_ParseID & 0xff);
						Len -= 3;
						}
					}
				}
			d1(kprintf("%s/%s/%ld: Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->tci_Buffer));
			}
		else
			{
			*blp++ = *tlp++;
			Len--;
			}
		}

	*blp = '\0';

	d1(kprintf("%s/%s/%ld: Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->tci_Buffer));

	return (ULONG) inst->tci_Buffer;
}


static ULONG TitleClass_Translate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_Translate *mxl = (struct msg_Translate *) msg;
	const struct TitleFuncEntry *tfe;
	struct InfoData *id;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: mxl_ParseID=%08lx msg=%08lx\n", __FILE__, __FUNC__, __LINE__, mxl->mxl_ParseID, msg));

	id = ScalosAllocInfoData();
	d1(KPrintF("%s/%s/%ld: id=%08lx\n", __FILE__, __FUNC__, __LINE__, id));
	if (NULL == id)
		return FALSE;

	for (tfe=TitleFunctionTable; tfe->tfe_ParseID; tfe++)
		{
		if (tfe->tfe_ParseID == mxl->mxl_ParseID)
			{
			d1(KPrintF("%s/%s/%ld: tfe=%08lx\n", __FILE__, __FUNC__, __LINE__, tfe));
			(*tfe->tfe_Function)(iwt, id, mxl);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			Success = TRUE;
			break;
			}
		}

	// variable name not found

	ScalosFreeInfoData(&id);

	d1(KPrintF("%s/%s/%ld: Success=%lu\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}


static ULONG TitleClass_QueryTitle(Class *cl, Object *o, Msg msg)
{
	struct TitleClassInstance *inst = INST_DATA(cl, o);
	CONST_STRPTR tlp;
	BOOL NeedRefresh = FALSE;

	for (tlp=inst->tci_TitleFormat; *tlp; tlp++)
		{
		if ('%' == tlp[0] && tlp[1] && tlp[2])
			{
			if ('%' == tlp[1])
				{
				tlp += 2;
				}
			else
				{
				struct msg_Query mqy;

				mqy.mqy_MethodID = SCCM_Title_Query;
				mqy.mqy_ParseID = (tlp[1] << 8) + tlp[2];

				tlp += 3;		// skip "%" + 2-char variable name

				if (DoMethodA(o, (Msg)(APTR) &mqy))
					NeedRefresh = TRUE;
				}
			}
		}

	return NeedRefresh;
}


static ULONG TitleClass_Query(Class *cl, Object *o, Msg msg)
{
	struct msg_Query *mqy = (struct msg_Query *) msg;
	UWORD *wp;

	for (wp=TitleQueryTable; *wp; wp++)
		{
		if (*wp == mqy->mqy_ParseID)
			return TRUE;
		}

	return FALSE;
}


static void TTLFN_WBVersion(struct internalScaWindowTask *iwt, struct InfoData *id, 
	struct msg_Translate *mxl)
{
	stccpy(mxl->mxl_Buffer, ScalosRevision, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_WBRealVersion(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ScaFormatStringMaxLength(Line, sizeof(Line),
		"%ld.%ld", (LONG) ScalosBase->scb_LibNode.lib_Version, (LONG) ScalosBase->scb_LibNode.lib_Revision);

	d1(KPrintF("%s/%s/%ld: Line=<%s>\n", __FILE__, __FUNC__, __LINE__, Line));

	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_OSVersion(struct internalScaWindowTask *iwt, struct InfoData *id, 
	struct msg_Translate *mxl)
{
	CONST_STRPTR OSVersion;

	switch (SysBase->LibNode.lib_Version)
		{
	case 45:
		OSVersion = "3.9";
		break;
	case 44:
		OSVersion = "3.5";
		break;
	case 43:
		OSVersion = "3.2";
		break;
	case 40:
		OSVersion = "3.1";
		break;
	case 39:
		OSVersion = "3.0";
		break;
	default:
		OSVersion = "?.?";
		break;
		}

	stccpy(mxl->mxl_Buffer, OSVersion, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_OSRealVersion(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];

	ScaFormatStringMaxLength(Line, sizeof(Line),
		"%ld.%ld", (LONG) SysBase->LibNode.lib_Version, (LONG) SysBase->LibNode.lib_Revision);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_Path(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	if (iwt && iwt->iwt_WindowTask.mt_WindowStruct->ws_Name)
		{
		stccpy(mxl->mxl_Buffer, iwt->iwt_WindowTask.mt_WindowStruct->ws_Name, mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
}


static void TTLFN_ChipRAM(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];
	LONG Scale = 0;
	ULONG Mem;

	if (*mxl->mxl_TitleFormat >= '0' && *mxl->mxl_TitleFormat <= '6')
		{
		Scale = (*mxl->mxl_TitleFormat - '0') * 10;
		mxl->mxl_TitleFormat++;
		}

	Mem = AvailMem(MEMF_CHIP) >> Scale;

	ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", Mem);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_FastRAM(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];
	LONG Scale = 0;
	ULONG Mem;

	if (*mxl->mxl_TitleFormat >= '0' && *mxl->mxl_TitleFormat <= '6')
		{
		Scale = (*mxl->mxl_TitleFormat - '0') * 10;
		mxl->mxl_TitleFormat++;
		}

	Mem = AvailMem(MEMF_FAST) >> Scale;

	ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", Mem);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_TotalRAM(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];
	LONG Scale = 0;
	ULONG Mem;

	if (*mxl->mxl_TitleFormat >= '0' && *mxl->mxl_TitleFormat <= '6')
		{
		Scale = (*mxl->mxl_TitleFormat - '0') * 10;
		mxl->mxl_TitleFormat++;
		}

	Mem = AvailMem(MEMF_PUBLIC) >> Scale;

	ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", Mem);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_DiskFree(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	LONG Scale = 0;

	if (*mxl->mxl_TitleFormat >= '0' && *mxl->mxl_TitleFormat <= '6')
		{
		Scale = (*mxl->mxl_TitleFormat - '0') * 10;
		mxl->mxl_TitleFormat++;
		}

	TitleClassGetDiskInfo(id);

	if (ID_NO_DISK_PRESENT == id->id_DiskType)
		{
		stccpy(mxl->mxl_Buffer, "?", mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
	else
		{
		ULONG64 BytesFree = Mul64(MakeU64(id->id_NumBlocks - id->id_NumBlocksUsed), MakeU64(id->id_BytesPerBlock), NULL);

		ShiftR64(&BytesFree, Scale);

		d1(kprintf("%s/%s/%ld: id_NumBlocksUsed=%lu  id_BytesPerBlock=%lu\n", __FILE__, __FUNC__, __LINE__, id->id_NumBlocksUsed, id->id_BytesPerBlock));
		d1(kprintf("%s/%s/%ld: BytesFree=%lu %lu\n", __FILE__, __FUNC__, __LINE__, ULONG64_HIGH(BytesFree), ULONG64_LOW(BytesFree)));

		Convert64(ScalosLocale, BytesFree, mxl->mxl_Buffer, mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
}


static void TTLFN_DiskFreeKMG(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	TitleClassGetDiskInfo(id);

	if (ID_NO_DISK_PRESENT == id->id_DiskType)
		{
		stccpy(mxl->mxl_Buffer, "?", mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
	else
		{
		ULONG64 BytesFree = Mul64(MakeU64(id->id_NumBlocks - id->id_NumBlocksUsed), MakeU64(id->id_BytesPerBlock), NULL);

		d1(kprintf("%s/%s/%ld: id_NumBlocksUsed=%lu  id_BytesPerBlock=%lu\n", __FILE__, __FUNC__, __LINE__, id->id_NumBlocksUsed, id->id_BytesPerBlock));
		d1(kprintf("%s/%s/%ld: BytesFree=%lu %lu\n", __FILE__, __FUNC__, __LINE__, ULONG64_HIGH(BytesFree), ULONG64_LOW(BytesFree)));

		TitleClass_Convert64KMG(BytesFree, mxl->mxl_Buffer,mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
}


static void TTLFN_DiskUsed(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	LONG Scale = 0;

	if (*mxl->mxl_TitleFormat >= '0' && *mxl->mxl_TitleFormat <= '6')
		{
		Scale = (*mxl->mxl_TitleFormat - '0') * 10;
		mxl->mxl_TitleFormat++;
		}

	TitleClassGetDiskInfo(id);

	if (ID_NO_DISK_PRESENT == id->id_DiskType)
		{
		stccpy(mxl->mxl_Buffer, "?", mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
	else
		{
		ULONG64 BytesUsed = Mul64(MakeU64(id->id_NumBlocksUsed), MakeU64(id->id_BytesPerBlock), NULL);

		ShiftR64(&BytesUsed, Scale);

		d1(kprintf("%s/%s/%ld: id_NumBlocksUsed=%lu  id_BytesPerBlock=%lu\n", __FILE__, __FUNC__, __LINE__, id->id_NumBlocksUsed, id->id_BytesPerBlock));
		d1(kprintf("%s/%s/%ld: BytesUsed=%lu %lu\n", __FILE__, __FUNC__, __LINE__, ULONG64_HIGH(BytesUsed), ULONG64_LOW(BytesUsed)));

		Convert64(ScalosLocale, BytesUsed, mxl->mxl_Buffer,mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
}


static void TTLFN_DiskUsedKMG(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	TitleClassGetDiskInfo(id);

	if (ID_NO_DISK_PRESENT == id->id_DiskType)
		{
		stccpy(mxl->mxl_Buffer, "?", mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
	else
		{
		ULONG64 BytesUsed = Mul64(MakeU64(id->id_NumBlocksUsed), MakeU64(id->id_BytesPerBlock), NULL);

		d1(kprintf("%s/%s/%ld: id_NumBlocksUsed=%lu  id_BytesPerBlock=%lu\n", __FILE__, __FUNC__, __LINE__, id->id_NumBlocksUsed, id->id_BytesPerBlock));
		d1(kprintf("%s/%s/%ld: BytesUsed=%lu %lu\n", __FILE__, __FUNC__, __LINE__, ULONG64_HIGH(BytesUsed), ULONG64_LOW(BytesUsed)));

		TitleClass_Convert64KMG(BytesUsed, mxl->mxl_Buffer,mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
}


static void TTLFN_DiskInsertedStart(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	TitleClassGetDiskInfo(id);

	if (ID_NO_DISK_PRESENT == id->id_DiskType)
		{
		while (mxl->mxl_TitleFormat[0] && mxl->mxl_TitleFormat[1] && mxl->mxl_TitleFormat[2])
			{
			if (0 == strncmp(mxl->mxl_TitleFormat, "%d)", 3))
				break;

			mxl->mxl_TitleFormat++;
			}
		}
}


static void TTLFN_DiskInsertedEnd(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	// no-op
}


static void TTLFN_DiskPercent(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	TitleClassGetDiskInfo(id);

	if (ID_NO_DISK_PRESENT == id->id_DiskType)
		{
		stccpy(mxl->mxl_Buffer, "?", mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
	else
		{
		char Line[10];
		ULONG64 BlocksTotal = MakeU64(id->id_NumBlocks);
		ULONG64 BlocksUsed100 = Mul64(MakeU64(id->id_NumBlocksUsed), MakeU64(100), NULL);
		ULONG64 PercentUsed;

		d1(kprintf("%s/%s/%ld: Total=%lu %lu\n", __FILE__, __FUNC__, __LINE__, ULONG64_HIGH(BlocksTotal), ULONG64_LOW(BlocksTotal)));
		d1(kprintf("%s/%s/%ld: Used100=%lu %lu\n", __FILE__, __FUNC__, __LINE__, ULONG64_HIGH(BlocksUsed100), ULONG64_LOW(BlocksUsed100)));

		if (0 == Cmp64(BlocksUsed100, MakeU64(0)))
			PercentUsed = MakeU64(0);
		else
			PercentUsed = Div64(BlocksUsed100, BlocksTotal, NULL);

		d1(kprintf("%s/%s/%ld: PercentUsed=%lu %lu\n", __FILE__, __FUNC__, __LINE__, ULONG64_HIGH(PercentUsed), ULONG64_LOW(PercentUsed)));

		ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", ULONG64_LOW(PercentUsed));

		stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

		mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
		mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
		}
}


static void TTLFN_ChipSet(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	CONST_STRPTR ChipSet;

	if (GFXF_AA_ALICE & GfxBase->ChipRevBits0)
		ChipSet = "AGA";
	else if (GFXF_HR_DENISE & GfxBase->ChipRevBits0)
		ChipSet = "ECS";
	else
		ChipSet = "OCS";

	stccpy(mxl->mxl_Buffer, ChipSet, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_Processor(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	static CONST_STRPTR Processor = NULL;

#if defined(__MORPHOS__)
	static ULONG CPUVersion = 0;
	static ULONG CPURevision = 0;

	if (NULL == Processor)
		{
		NewGetSystemAttrs(&CPUVersion, sizeof(CPUVersion),
			SYSTEMINFOTYPE_PPC_CPUVERSION, 0,
			TAG_END);
		NewGetSystemAttrs(&CPURevision, sizeof(CPURevision),
			SYSTEMINFOTYPE_PPC_CPUREVISION, 0,
			TAG_END);
		}

	d1(KPrintF("%s/%s/%ld: CPUVersion=%lu  CPURevision=%04lx\n", __FILE__, __FUNC__, __LINE__, CPUVersion, CPURevision));

	switch (CPUVersion)
		{
	case CPU_7455:
	      Processor = "7455";
              break;
	case CPU_7450:
		if (CPURevision > 0x0200)
			{
			Processor = "7451";
			}
		else
			{
			Processor = "7450";
			}
		break;
	case CPU_7400:
		if ((CPURevision & 0x1000)==0x1000)
			{
			Processor = "7410";
			}
		else
			{
			Processor = "7400";
			}
		break;
	case CPU_750:
		if ((CPURevision & 0xf000)==0x2000)
			{
			if (CPURevision >= 0x2214)
				{
				Processor = "750CXE";
				}
			else
				{
				Processor = "750CX";
				}
			}
		else
			{
			Processor = "750";
			}
		break;
	case CPU_604ev:
		Processor = "604EV";
		break;
	case CPU_604e:
		Processor = "604E";
		break;
	case CPU_604:
		Processor = "604";
		break;
	case CPU_603:
		Processor = "603";
		break;
	case CPU_603e:
		Processor = "603E";
		break;
	case CPU_603p:
		Processor = "603P";
		break;
	case CPU_602:
		Processor = "602";
		break;
	case CPU_601:
		Processor = "601";
		break;
	case CPU_7447:
		Processor = "7447";
		break;
	case CPU_7447A:
		Processor = "7447A";
		break;
        default:
		Processor = "Unknown";
		break;
      }
#elif defined(__amigaos4__)
	GetCPUInfoTags(GCIT_ModelString, &Processor,
		TAG_END);
#else //defined(__amigaos4__)
	if (NULL == Processor)
		{
		if (SysBase->AttnFlags & AFF_68060)
			Processor = "68060";
		else if (SysBase->AttnFlags & AFF_68040)
			Processor = "68040";
		else if (SysBase->AttnFlags & AFF_68030)
			Processor = "68030";
		else if (SysBase->AttnFlags & AFF_68020)
			Processor = "68020";
		else if (SysBase->AttnFlags & AFF_68010)
			Processor = "68010";
		else
			Processor = "68000";
		}
#endif /* __MORPHOS__ */

	stccpy(mxl->mxl_Buffer, Processor, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_CoProcessor(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	CONST_STRPTR CoProcessor;

	if ((SysBase->AttnFlags & AFF_68060) && (SysBase->AttnFlags & AFF_FPU40))
		CoProcessor = "060";
	else if ((SysBase->AttnFlags & AFF_68040) && (SysBase->AttnFlags & AFF_FPU40))
		CoProcessor = "040";
	else if (SysBase->AttnFlags & AFF_68882)
		CoProcessor = "882";
	else if (SysBase->AttnFlags & AFF_68881)
		CoProcessor = "881";
	else
		CoProcessor = "---";

	stccpy(mxl->mxl_Buffer, CoProcessor, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_NumTasks(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];
	ULONG Count;

	Forbid();
	Count = GetNodeCount(&SysBase->TaskReady) + GetNodeCount(&SysBase->TaskWait);
	Permit();

	ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", Count);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_NumLibraries(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];
	ULONG Count;

	Forbid();
	Count = GetNodeCount(&SysBase->LibList);
	Permit();

	ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", Count);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_NumPorts(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];
	ULONG Count;

	Forbid();
	Count = GetNodeCount(&SysBase->PortList);
	Permit();

	ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", Count);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_NumDevices(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	char Line[20];
	ULONG Count;

	Forbid();
	Count = GetNodeCount(&SysBase->DeviceList);
	Permit();

	ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", Count);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static void TTLFN_NumScreens(struct internalScaWindowTask *iwt, struct InfoData *id,
	struct msg_Translate *mxl)
{
	ULONG iLock;
	char Line[20];
	ULONG Count;
	struct Screen *scr;

	iLock = ScaLockIBase(0);

	for (Count=0, scr=IntuitionBase->FirstScreen; scr; scr=scr->NextScreen)
		Count++;

	ScaUnlockIBase(iLock);

	ScaFormatStringMaxLength(Line, sizeof(Line), "%lU", Count);
	stccpy(mxl->mxl_Buffer, Line, mxl->mxl_BuffLen);

	mxl->mxl_BuffLen -= strlen(mxl->mxl_Buffer);
	mxl->mxl_Buffer += strlen(mxl->mxl_Buffer);
}


static ULONG GetNodeCount(struct List *list)
{
	ULONG Count = 0;
	struct Node *node;

	Forbid();
	for (node=list->lh_Head; node != (struct Node *) &list->lh_Tail; node=node->ln_Succ)
		{
		Count++;
		}
	Permit();

	return Count;
}


static void TitleClassGetDiskInfo(struct InfoData *id)
{
	if (0 == id->id_DiskType)
		{
		if (!DoPkt(GetFileSysTask(), ACTION_DISK_INFO, (SIPTR) MKBADDR(id), 0, 0, 0, 0))
			id->id_DiskType = ID_NO_DISK_PRESENT;
		}
}


void TitleClass_Convert64KMG(ULONG64 Number, STRPTR Buffer, size_t MaxLen)
{
	TitleClass_Convert64KMGRounded(Number, Buffer, MaxLen, 0);
}


void TitleClass_Convert64KMGRounded(ULONG64 Number, STRPTR Buffer, size_t MaxLen, ULONG Round)
{
	ULONG MsgID = MSGID_BYTENAME;

	while (Cmp64(Number, MakeU64(10000)) >= 0 && MsgID < MSGID_HBYTENAME)
		{
		ShiftR64(&Number, 10);
		MsgID++;

		d1(kprintf("%s/%s/%ld: Number=%lu %lu  MsgID=%ld\n", __FILE__, __FUNC__, __LINE__, ULONG64_HIGH(Number), ULONG64_LOW(Number), MsgID));
		}

	if (Round)
		{
		ULONG64 ten = MakeU64(10);
		ULONG n;

		for (n = 0; Cmp64(Number, MakeU64(Round)) > 0; n++)
			{
			Number = Div64(Number, ten, NULL);
			}
		while (n--)
			{
			Number = Mul64(Number, ten, NULL);
			}
		}

	Convert64(ScalosLocale, Number, Buffer, MaxLen);

	MaxLen -= strlen(Buffer);
	Buffer += strlen(Buffer);

	SafeStrCat(Buffer, " ", MaxLen);

	MaxLen -= strlen(Buffer);
	Buffer += strlen(Buffer);

	SafeStrCat(Buffer,GetLocString(MsgID), MaxLen);
}


