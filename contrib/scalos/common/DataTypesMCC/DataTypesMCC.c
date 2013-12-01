// DataTypesMCC.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <libraries/mui.h>
#include <datatypes/pictureclass.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include "DataTypesMCC.h"

/*
** Make sure to *always* use V43 datatype tags, even if you dont
** have a gfxboard yourself and still use older datatypes.
** If you don't, your program will annoy many gfxboard-users!
** Check aminet for developer material!
*/


#define CLASS	   MUIC_Dtpic
#define SUPERCLASS MUIC_Area

#define DataTypesBase	(inst->dtbase)
#ifdef __amigaos4__
#define IDataTypes	(inst->dtiface)
#endif

struct DtMCCInstance
{
	struct Library *dtbase;
#ifdef __amigaos4__
	struct DataTypesIFace *dtiface;
#endif
	STRPTR name;
	APTR dto;
	struct BitMapHeader *bmhd;
	struct BitMap *bitmap;
	PLANEPTR MaskPlane;
	BOOL FailIfUnavailable;
	BOOL Tiled;
};

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg);
static ULONG mDispose(struct IClass *cl,Object *obj,Msg msg);
static BOOL CreateDto(struct DtMCCInstance *inst);
static VOID freedto(struct DtMCCInstance *inst);
static BOOL SetupDto(Class *cl, Object *obj);
static ULONG mSet(struct IClass *cl, Object *obj, Msg msg);
static ULONG mSetup(struct IClass *cl,Object *obj,Msg msg);
static ULONG mCleanup(struct IClass *cl,Object *obj,Msg msg);
static ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg);
static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg);
DISPATCHER_PROTO(MUI_DataTypesMCC);
static void ClearInstanceData(struct DtMCCInstance *inst);
static void ForceRelayout(struct IClass *cl, Object *obj);

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct DtMCCInstance *inst;
	struct opSet *ops;
	CONST_STRPTR FileName;
	BOOL Success = FALSE;

	do	{
		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		if (NULL == obj)
			break;

		d1(kprintf(__FUNC__ "/%ld:  obj=%08lx\n", __LINE__, obj));

		inst = INST_DATA(cl,obj);
		ClearInstanceData(inst);

		ops = (struct opSet *) msg;

		inst->dtbase = OpenLibrary("datatypes.library",39);
		d1(KPrintF(__FUNC__ "/%ld:  dtbase=%08lx", __LINE__, inst->dtbase));
		if (NULL == inst->dtbase)
			break;
#ifdef __amigaos4__
		inst->dtiface = (struct DataTypesIFace *)GetInterface(inst->dtbase, "main", 1, NULL);
		if (NULL == inst->dtiface)
			break;
#endif
		inst->FailIfUnavailable = GetTagData(MUIA_ScaDtpic_FailIfUnavailable, FALSE, ops->ops_AttrList);
		inst->Tiled = GetTagData(MUIA_ScaDtpic_Tiled, FALSE, ops->ops_AttrList);

		FileName = (CONST_STRPTR) GetTagData(MUIA_ScaDtpic_Name, (ULONG) NULL, ops->ops_AttrList);
		if (NULL == FileName)
			break;

		inst->name = strdup(FileName);
		if (NULL == inst->name)
			break;

		d1(KPrintF(__FUNC__ "/%ld:  inst->name=<%s>\n", __LINE__, inst->name));

		if (!inst->FailIfUnavailable)
			Success = TRUE;

		d1(KPrintF("%s/%ld: FileName=<%s>  name=<%s>\n", __FUNC__, __LINE__, FileName, inst->name));
		d1(KPrintF(__FUNC__ "/%ld:  DataTypesBase=%08lx", __LINE__, DataTypesBase));

		// create the datatypes object
		if (!CreateDto(inst))
			break;

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

static ULONG mDispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);

	d1(kprintf(__FUNC__ "/%ld:  \n", __LINE__));

	freedto(inst);

	d1(kprintf(__FUNC__ "/%ld:  \n", __LINE__));

	if (inst->name)
		{
		free(inst->name);
		inst->name = NULL;
		}

	d1(kprintf(__FUNC__ "/%ld:  \n", __LINE__));

#ifdef __amigaos4__
	if (inst->dtiface)
		{
		DropInterface((struct Interface *)inst->dtiface);
		inst->dtiface = NULL;
		}
#endif
	if (inst->dtbase)
		{
		CloseLibrary(inst->dtbase);
		inst->dtbase = NULL;
		}

	d1(kprintf(__FUNC__ "/%ld:  \n", __LINE__));

	return DoSuperMethodA(cl, obj, msg);
}

/* ------------------------------------------------------------------------- */

static BOOL CreateDto(struct DtMCCInstance *inst)
{
	struct Process *myproc;
	APTR oldwindowptr;

	/* tell DOS not to bother us with requesters */
	myproc = (struct Process *) FindTask(NULL);
	oldwindowptr = myproc->pr_WindowPtr;
	myproc->pr_WindowPtr = (APTR) -1;

	freedto(inst);	// make sure any old datatypes object is freed

	// create the datatypes object
	inst->dto = NewDTObject((APTR) inst->name,
		DTA_GroupID, GID_PICTURE,
		OBP_Precision, PRECISION_EXACT,
		PDTA_FreeSourceBitMap, TRUE,
		PDTA_DestMode, PMODE_V43,
		PDTA_UseFriendBitMap, TRUE,
		TAG_DONE);

	d1(KPrintF("%s/%ld: dto=%08lx\n", __FUNC__, __LINE__, inst->dto));

	myproc->pr_WindowPtr = oldwindowptr;

	return (BOOL) (NULL != inst->dto);
}

/* ------------------------------------------------------------------------- */

/* do all the setup/layout stuff that's necessary to get a bitmap from the dto	 */
/* note that when using V43 datatypes, this might not be a real "struct BitMap *" */

static BOOL SetupDto(Class *cl, Object *obj)
{
	BOOL Success = FALSE;

	do	{
		struct FrameInfo fri = { 0 };
		struct DtMCCInstance *inst = INST_DATA(cl,obj);

		if (NULL == inst->dto)
			break;

		d1(KPrintF("%s/%ld: muiRenderInfo=%08lx\n", __FUNC__, __LINE__, muiRenderInfo(obj)));
		if (NULL == muiRenderInfo(obj))
			break;

		d1(KPrintF("%s/%ld: screen=%08lx\n", __FUNC__, __LINE__, _screen(obj)));
		d1(KPrintF("%s/%ld: win=%08lx\n", __FUNC__, __LINE__, _win(obj)));

		SetAttrs(inst->dto,
			PDTA_Screen, (ULONG) _screen(obj),
			TAG_END);

		DoMethod(inst->dto,
			DTM_FRAMEBOX, NULL, &fri, &fri, sizeof(struct FrameInfo), 0);

		d1(KPrintF("%s/%ld: Depth=%ld\n", __FUNC__, __LINE__, fri.fri_Dimensions.Depth));
		if (fri.fri_Dimensions.Depth <= 0)
			break;

		if (!DoDTMethod(inst->dto, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE))
			break;

		get(inst->dto, PDTA_BitMapHeader, &inst->bmhd);
		d1(KPrintF("%s/%ld: bmhd=%08lx\n", __FUNC__, __LINE__, inst->bmhd));
		if (NULL == inst->bmhd)
			break;

		GetDTAttrs(inst->dto, PDTA_DestBitMap, (ULONG) &inst->bitmap, TAG_DONE);
		d1(KPrintF("%s/%ld: bitmap=%08lx\n", __FUNC__, __LINE__, inst->bitmap));
		d1(KPrintF("%s/%ld: bmh_Masking=%ld\n", __FUNC__, __LINE__, inst->bmhd->bmh_Masking));

		GetDTAttrs(inst->dto,
			PDTA_MaskPlane, (ULONG) &inst->MaskPlane,
			TAG_END);
		d1(KPrintF("%s/%ld: MaskPlane=%08lx\n", __FUNC__, __LINE__, inst->MaskPlane));

		if (NULL == inst->bitmap)
			GetDTAttrs(inst->dto, PDTA_BitMap, (ULONG) &inst->bitmap, TAG_DONE);

		if (NULL == inst->bitmap)
			break;

		/* tell MUI not to care about filling our background during MUIM_Draw */
		set(obj, MUIA_FillArea, NULL != inst->MaskPlane);

		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%ld: Success=%ld\n", __FUNC__, __LINE__, Success));

	return Success;
}

/* ------------------------------------------------------------------------- */

/*
** free the datatypes object.
** called from cleanup method or when setup failed somehow
*/

static VOID freedto(struct DtMCCInstance *inst)
{
	inst->bitmap = NULL;
	inst->bmhd = NULL;

	if (inst->dto)
		{
		DisposeDTObject(inst->dto);
		inst->dto = NULL;
		}
}

/* ------------------------------------------------------------------------- */

static ULONG mSet(struct IClass *cl, Object *obj, Msg msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);
	struct opSet *ops = (struct opSet *) msg;
	CONST_STRPTR FileName;
	STRPTR NewFileName;
	struct TagItem *ti;

	inst->FailIfUnavailable = GetTagData(MUIA_ScaDtpic_FailIfUnavailable, inst->FailIfUnavailable, ops->ops_AttrList);
	FileName = (CONST_STRPTR) GetTagData(MUIA_ScaDtpic_Name, (ULONG) inst->name, ops->ops_AttrList);

	if (FileName)
		NewFileName = strdup(FileName);
	else
		NewFileName = NULL;

	if (inst->name)
		free(inst->name);
	inst->name = NewFileName;
	d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));

	if (FindTagItem(MUIA_ScaDtpic_Name, ops->ops_AttrList))
		{
		WORD OldWidth, OldHeight;
		WORD NewWidth, NewHeight;

		d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));

		OldWidth  = inst->bmhd ? inst->bmhd->bmh_Width : 0;
		OldHeight = inst->bmhd ? inst->bmhd->bmh_Height : 0;

		d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));

		(void) CreateDto(inst);
		d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));
		(void) SetupDto(cl, obj);
		d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));

		NewWidth  = inst->bmhd ? inst->bmhd->bmh_Width : 0;
		NewHeight = inst->bmhd ? inst->bmhd->bmh_Height : 0;

		d1(KPrintF("%s/%ld: OldWidth=%ld  OldHeight=%ld\n", __FUNC__, __LINE__, OldWidth, OldHeight));
		d1(KPrintF("%s/%ld: NewWidth=%ld  NewHeight=%ld\n", __FUNC__, __LINE__, NewWidth, NewHeight));

		if (OldWidth != NewWidth || OldHeight != NewHeight)
			{
			// Setting an image with different size requires relayout
			d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));
			ForceRelayout(cl, obj);
			}
		else
			{
			// Image has changed - redraw ourself
			d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));
			MUI_Redraw(obj, MADF_DRAWOBJECT);
			}
		d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));
		}
	ti = FindTagItem(MUIA_ScaDtpic_Tiled, ops->ops_AttrList);
	if (ti)
		{
		if (inst->Tiled != ti->ti_Data)
			{
			inst->Tiled = ti->ti_Data;
			ForceRelayout(cl, obj);
			}
		}


	return DoSuperMethodA(cl, obj, msg);
}

/* ------------------------------------------------------------------------- */

static ULONG mSetup(struct IClass *cl, Object *obj, Msg msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);
	BOOL Success = FALSE;

	do	{
		if (!DoSuperMethodA(cl, obj, msg))
			break;

		d1(KPrintF("%s/%ld: name=<%s>\n", __FUNC__, __LINE__, inst->name));

		if (!inst->FailIfUnavailable)
			Success = TRUE;

		// inst->dto can be NULL after mCleanup (e.g. window was iconified)
		if (NULL == inst->dto)
			{
			// re-create the datatypes object
			inst->dto = NewDTObject((APTR) inst->name,
				DTA_GroupID, GID_PICTURE,
				OBP_Precision, PRECISION_EXACT,
				PDTA_FreeSourceBitMap, TRUE,
				PDTA_DestMode, PMODE_V43,
				PDTA_UseFriendBitMap, TRUE,
				TAG_DONE);

			d1(KPrintF("%s/%ld: dto=%08lx\n", __FUNC__, __LINE__, inst->dto));
			}

		if (NULL == inst->dto)
			break;
		d1(KPrintF("%s/%ld: dto=%08lx\n", __FUNC__, __LINE__, inst->dto));

		if (!SetupDto(cl, obj))
			break;

		Success = TRUE;
		} while (0);

	return Success;
}

/* ------------------------------------------------------------------------- */

static ULONG mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);

	freedto(inst);

	return DoSuperMethodA(cl,obj,msg);
}

/* ------------------------------------------------------------------------- */

static ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);
	struct MUI_MinMax *mi;

	DoSuperMethodA(cl, obj, (Msg) msg);

	mi = msg->MinMaxInfo;

	if (inst->bmhd)
		{
		mi->MinWidth  += inst->bmhd->bmh_Width ;
		mi->MinHeight += inst->bmhd->bmh_Height;
		mi->DefWidth  += inst->bmhd->bmh_Width ;
		mi->DefHeight += inst->bmhd->bmh_Height;
		mi->MaxWidth  += inst->bmhd->bmh_Width ;
		mi->MaxHeight += inst->bmhd->bmh_Height;
		}

	d1(KPrintF("%s/%ld: bmhd=%08lx\n", __FUNC__, __LINE__, inst->bmhd));
	d1(KPrintF("%s/%ld: DefWidth=%ld  DefHeight=%ld\n", __FUNC__, __LINE__, mi->DefWidth, mi->DefHeight));

	/* if we have no bitmap header, our object's size will be 0 */

	return(0);
}

/* ------------------------------------------------------------------------- */

static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct DtMCCInstance *inst = INST_DATA(cl,obj);

	DoSuperMethodA(cl, obj, (Msg) msg);

	if (msg->flags & MADF_DRAWOBJECT)
		{
		d1(KPrintF("%s/%ld: bitmap=%08lx\n", __FUNC__, __LINE__, inst->bitmap));
#if 1
		if (inst->bitmap)
			{
			WORD Width, Height;
			WORD MinX, MinY;

			MinX   =   _mleft(obj);
			MinY   =    _mtop(obj);
			Width  =  _mwidth(obj);
			Height = _mheight(obj);

			if (inst->Tiled)
				{
				ULONG StartOffset;
				WORD MaxX, MaxY;

				StartOffset = 0;
				MaxX = MinX + Width - 1;
				MaxY = MinY + Height - 1;

				while (MinY <= MaxY)
					{
					WORD SizeY;
					WORD x;

					SizeY = MaxY - MinY + 1;
					if (SizeY > inst->bmhd->bmh_Height)
						SizeY = inst->bmhd->bmh_Height;

					d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: MinY=%ld  SizeY=%ld\n", __LINE__, MinY, SizeY));

					for (x=MinX; x<=MaxX; )
						{
						WORD SizeX = MaxX - x + 1;

						if (SizeX > inst->bmhd->bmh_Width)
							SizeX = inst->bmhd->bmh_Width;

						d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: x=%ld  SizeX=%ld\n", __LINE__, x, SizeX));

						BltBitMapRastPort(inst->bitmap,
							0, StartOffset,
							_rp(obj),
							x, MinY,
							SizeX, SizeY,
							0xC0);

						x += SizeX;
						}

					MinY += SizeY;
					}
				}
			else
				{
				if (inst->MaskPlane)
					{
					BltMaskBitMapRastPort(inst->bitmap,
						0, 0,
						_rp(obj),
						_mleft(obj),_mtop(obj),
						_mwidth(obj),_mheight(obj),
						ABC | ABNC | ANBC,
						inst->MaskPlane);
					}
				else
					{
					BltBitMapRastPort(inst->bitmap,
						0, 0,
						_rp(obj),
						_mleft(obj),_mtop(obj),
						_mwidth(obj),_mheight(obj),
						0xc0);
					}
				}
			}
#else
		if (inst->dto)
			{
			SetAttrs(inst->dto,
				PDTA_Screen, (ULONG) _screen(obj),
				TAG_END);
			DoDTMethod(inst->dto,
				_window(obj), NULL,
				DTM_DRAW,
				_rp(obj),
				_mleft(obj), _mtop(obj),
				_mwidth(obj),_mheight(obj),
				5, 5,
				NULL
				);
			}
#endif
		}

	return(0);
}

/* ------------------------------------------------------------------------- */

DISPATCHER(MUI_DataTypesMCC)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		d1(KPrintF("%s/%ld: OM_NEW\n", __FUNC__, __LINE__));
		Result = mNew(cl,obj,(APTR)msg);
		break;

	case OM_DISPOSE:
		d1(KPrintF("%s/%ld: OM_DISPOSE\n", __FUNC__, __LINE__));
		Result = mDispose(cl,obj,(APTR)msg);
		break;

	case OM_SET:
		d1(KPrintF("%s/%ld: OM_SET", __FUNC__, __LINE__));
		Result = mSet(cl,obj,(APTR)msg);
		break;

	case MUIM_Setup:
		d1(KPrintF("%s/%ld: MUIM_Setup\n", __FUNC__, __LINE__));
		Result = mSetup(cl,obj,(APTR)msg);
		break;

	case MUIM_Cleanup:
		d1(KPrintF("%s/%ld: MUIM_Cleanup\n", __FUNC__, __LINE__));
		Result = mCleanup(cl,obj,(APTR)msg);
		break;

	case MUIM_AskMinMax:
		d1(KPrintF("%s/%ld: MUIM_AskMinMax\n", __FUNC__, __LINE__));
		Result = mAskMinMax(cl,obj,(APTR)msg);
		break;

	case MUIM_Draw:
		d1(KPrintF("%s/%ld: MUIM_Draw\n", __FUNC__, __LINE__));
		Result = mDraw(cl,obj,(APTR)msg);
		break;

	default:
		d1(kprintf("%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl,obj,msg);
		break;
		}

	return Result;
}
DISPATCHER_END

/* ------------------------------------------------------------------------- */

static void ClearInstanceData(struct DtMCCInstance *inst)
{
	memset(inst, 0, sizeof(struct DtMCCInstance));
}

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitDtpicClass(void)
{
	return MUI_CreateCustomClass(NULL, SUPERCLASS, NULL, sizeof(struct DtMCCInstance), DISPATCHER_REF(MUI_DataTypesMCC));
}

void CleanupDtpicClass(struct MUI_CustomClass *mcc)
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
