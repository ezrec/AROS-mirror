// BitMapMCC.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <libraries/mui.h>
#include <datatypes/pictureclass.h>
#include <cybergraphx/cybergraphics.h>
#include <scalos/scalosgfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/scalosgfx.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include "debug.h"
#include "BitMapMCC.h"


#define CLASS	   MUIC_BitMappic
#define SUPERCLASS MUIC_Area

struct BitMapMCCInstance
{
	struct BitMap *BitMap;
	PLANEPTR MaskPlane;
	struct Screen *Screen;
	const ULONG *ColorTable;
	ULONG Width;
	ULONG Height;
	const UBYTE *BitMapArray;
	struct ARGB *PixelArray;
	ULONG NumColors;	// number of colors in ColorTable
};

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg);
static ULONG mDispose(struct IClass *cl,Object *obj,Msg msg);
static ULONG mSetup(struct IClass *cl,Object *obj,Msg msg);
static ULONG mCleanup(struct IClass *cl,Object *obj,Msg msg);
static ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg);
static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg);
DISPATCHER_PROTO(BitMapMCC);
static ULONG mGet(Class *cl, Object *o, struct opGet *opg);
static void ClearInstanceData(struct BitMapMCCInstance *inst);

/* ------------------------------------------------------------------------- */

extern struct Library *CyberGfxBase;

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct BitMapMCCInstance *inst;
	struct opSet *ops;
	struct BitMap *TempBM = NULL;
	UBYTE *PixelLine = NULL;
	BOOL Success = FALSE;

	do	{
		ULONG ScreenDepth;

		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		if (NULL == obj)
			break;

		d1(kprintf(__FUNC__ "/%ld:  obj=%08lx\n", __LINE__, obj));

		inst = INST_DATA(cl,obj);
		ClearInstanceData(inst);

		ops = (struct opSet *) msg;

		inst->NumColors = 256;

		inst->ColorTable = (ULONG *) GetTagData(MUIA_ScaBitMappic_ColorTable, (ULONG) NULL, ops->ops_AttrList);
		inst->Screen = (struct Screen *) GetTagData(MUIA_ScaBitMappic_Screen, (ULONG) NULL, ops->ops_AttrList);

		inst->Width  = (ULONG) GetTagData(MUIA_ScaBitMappic_Width, 0, ops->ops_AttrList);
		inst->Height = (ULONG) GetTagData(MUIA_ScaBitMappic_Height, 0, ops->ops_AttrList);

		inst->BitMapArray = (UBYTE *) GetTagData(MUIA_ScaBitmappic_BitMapArray, (ULONG) NULL, ops->ops_AttrList);
		d1(KPrintF(__FUNC__ "/%ld:  inst->BitMapArray=%08lx\n", __LINE__, inst->BitMapArray));

		inst->BitMap = (struct BitMap *) GetTagData(MUIA_ScaBitMappic_BitMap, (ULONG) NULL, ops->ops_AttrList);
		if (NULL == inst->BitMap)
			break;

		d1(KPrintF(__FUNC__ "/%ld:  inst->BitMap=%08lx\n", __LINE__, inst->BitMap));

		inst->PixelArray = ScalosGfxCreateARGB(inst->Width, inst->Height, NULL);
		d1(KPrintF(__FUNC__ "/%ld:  inst->PixelArray=%08lx\n", __LINE__, inst->PixelArray));
		if (NULL == inst->PixelArray)
			break;

		ScreenDepth = GetBitMapAttr(inst->Screen->RastPort.BitMap, BMA_DEPTH);

		if (CyberGfxBase && ScreenDepth > 8)
			{
			if (GetCyberMapAttr(inst->BitMap, CYBRMATTR_ISCYBERGFX))
				{
				d1(KPrintF("%s/%ld: CyberGfx BitMap\n", __FUNC__, __LINE__));
				}
			else
				{
				struct ARGB *pPixelArray;
				ULONG y;

				d1(KPrintF("%s/%ld: Standard BitMap\n", __FUNC__, __LINE__));
				d1(KPrintF("%s/%ld: ColorTable=%08lx\n", __FUNC__, __LINE__, inst->ColorTable));

				// Translate pen numbers from BitMapArray into RGB values
				if (inst->BitMapArray)
					{
					const UBYTE *pBitMapArray = inst->BitMapArray;

					for (y = 0, pPixelArray = inst->PixelArray; y < inst->Height; y++)
						{
						ULONG x;
						const UBYTE *pLineArray = pBitMapArray;

						for (x = 0; x < inst->Width; x++)
							{
							ULONG PenIndex = 3 * *pLineArray++;

							pPixelArray->Alpha = 0;							// AA
							pPixelArray->Red   = (UBYTE) (inst->ColorTable[PenIndex] >> 24);      // RR
							pPixelArray->Green = (UBYTE) (inst->ColorTable[PenIndex + 1] >> 24);  // GG
							pPixelArray->Blue  = (UBYTE) (inst->ColorTable[PenIndex + 2] >> 24);  // BB

							pPixelArray++;
							}

						pBitMapArray += PIXELARRAY8_WIDTH(inst->Width);
						}
					}
				else
					{
					struct RastPort rp;
					struct RastPort TempRp;

					PixelLine = malloc(PIXELARRAY8_BUFFERSIZE(inst->Width, 1));
					if (NULL == PixelLine)
						break;

					InitRastPort(&TempRp);
					TempRp.Layer = NULL;
					TempRp.BitMap = TempBM = AllocBitMap(TEMPRP_WIDTH(inst->Width), 1, 8, 0, NULL);
					if (NULL == TempBM)
						break;

					InitRastPort(&rp);
					rp.BitMap = inst->BitMap;

					for (y = 0, pPixelArray = inst->PixelArray; y < inst->Height; y++)
						{
						ULONG x;

						ReadPixelLine8(&rp,
							0, y,
							inst->Width,
							PixelLine,
							&TempRp);

						for (x = 0; x < inst->Width; x++)
							{
							ULONG PenIndex = 3 * PixelLine[x];

							pPixelArray->Alpha = 0;							// AA
							pPixelArray->Red   = (UBYTE) (inst->ColorTable[PenIndex] >> 24);      // RR
							pPixelArray->Green = (UBYTE) (inst->ColorTable[PenIndex + 1] >> 24);  // GG
							pPixelArray->Blue  = (UBYTE) (inst->ColorTable[PenIndex + 2] >> 24);  // BB

							pPixelArray++;
							}
						}
					}

				d1(KPrintF("%s/%ld: PixelArray: %02lx/%02lx/%02lx %02lx/%02lx/%02lx\n", \
					__FUNC__, __LINE__, \
					inst->PixelArray[0].Red, inst->PixelArray[0].Green, inst->PixelArray[0].Blue, \
					inst->PixelArray[1].Red, inst->PixelArray[1].Green, inst->PixelArray[1].Blue, \
					inst->PixelArray[2].Red, inst->PixelArray[2].Green, inst->PixelArray[2].Blue));
				}
			}

		Success = TRUE;
		} while (0);

	if (TempBM)
		FreeBitMap(TempBM);
	if (PixelLine)
		free(PixelLine);

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


static ULONG mDispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct BitMapMCCInstance *inst = INST_DATA(cl,obj);

	if (inst->PixelArray)
		{
		ScalosGfxFreeARGB(&inst->PixelArray);
		}

	d1(kprintf(__FUNC__ "/%ld:  \n", __LINE__));

	return DoSuperMethodA(cl, obj, msg);
}


static ULONG mSetup(struct IClass *cl,Object *obj,Msg msg)
{
	struct BitMapMCCInstance *inst = INST_DATA(cl,obj);
	BOOL Success = FALSE;

	do	{
		if (!DoSuperMethodA(cl,obj,msg))
			break;

		d1(KPrintF("%s/%ld: BitMap=%08lx\n", __FUNC__, __LINE__, inst->BitMap));

		if (NULL == inst->BitMap)
			break;

		if (NULL == inst->MaskPlane)
			{
			/* tell MUI not to care about filling our background during MUIM_Draw */
			set(obj, MUIA_FillArea, FALSE);
			}

		Success = TRUE;
		} while (0);

	return Success;
}


static ULONG mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
//	struct BitMapMCCInstance *inst = INST_DATA(cl,obj);

	return DoSuperMethodA(cl,obj,msg);
}


static ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	struct BitMapMCCInstance *inst = INST_DATA(cl,obj);
	struct MUI_MinMax *mi;

	DoSuperMethodA(cl, obj, (Msg) msg);

	mi = msg->MinMaxInfo;

	if (inst->BitMap)
		{
		mi->MinWidth  += inst->Width ;
		mi->MinHeight += inst->Height;
		mi->DefWidth  += inst->Width ;
		mi->DefHeight += inst->Height;
//		  mi->MaxWidth  += inst->Width ;
//		  mi->MaxHeight += inst->Height;
		mi->MaxWidth = 10000;
		mi->MaxHeight = 10000;
		}

	/* if we have no bitmap, our object's size will be 0 */

	return(0);
}


static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct BitMapMCCInstance *inst = INST_DATA(cl,obj);

	DoSuperMethodA(cl, obj, (Msg) msg);

	if (msg->flags & MADF_DRAWOBJECT)
		{
		d1(KPrintF("%s/%ld: bitmap=%08lx\n", __FUNC__, __LINE__, inst->BitMap));
		if (inst->BitMap)
			{
			ULONG Left = _mleft(obj);
			ULONG Top = _mtop(obj);
			ULONG Width = _mwidth(obj);
			ULONG Height = _mheight(obj);

			d1(KPrintF("%s/%ld: PixelArray: %02lx/%02lx/%02lx %02lx/%02lx/%02lx\n", \
				__FUNC__, __LINE__, \
				inst->PixelArray[0].Red, inst->PixelArray[0].Green, inst->PixelArray[0].Blue, \
				inst->PixelArray[1].Red, inst->PixelArray[1].Green, inst->PixelArray[1].Blue, \
				inst->PixelArray[2].Red, inst->PixelArray[2].Green, inst->PixelArray[2].Blue));

			if (inst->Width < Width)
				{
				Left += (Width - inst->Width) / 2;
				Width = inst->Width;
				}
			if (inst->Height < Height)
				{
				Top += (Height - inst->Height) / 2;
				Height = inst->Height;
				}

			WritePixelArray(inst->PixelArray,
				0, 0,
				sizeof(struct ARGB) * inst->Width,
				_rp(obj),
				Left, Top,
				Width, Height,
				RECTFMT_ARGB);
			}
		}

	return(0);
}


static ULONG mGet(Class *cl, Object *o, struct opGet *opg)
{
	struct BitMapMCCInstance *inst = INST_DATA(cl, o);
	ULONG Result = 0;

	switch (opg->opg_AttrID)
		{
	case MUIA_ScaBitMappic_ColorTable:
		*(opg->opg_Storage) = (ULONG) inst->ColorTable;
		Result++;
		break;
	case MUIA_ScaBitMappic_Screen:
		*(opg->opg_Storage) = (ULONG) inst->Screen;
		Result++;
		break;
	case MUIA_ScaBitMappic_Width:
		*(opg->opg_Storage) = inst->Width;
		Result++;
		break;
	case MUIA_ScaBitMappic_Height:
		*(opg->opg_Storage) = inst->Height;
		Result++;
		break;
	case MUIA_ScaBitmappic_BitMapArray:
		*(opg->opg_Storage) = (ULONG) inst->BitMapArray;
		Result++;
		break;
	case MUIA_ScaBitMappic_BitMap:
		*(opg->opg_Storage) = (ULONG) inst->BitMap;
		Result++;
		break;
	default:
		Result = DoSuperMethodA(cl, o, (Msg) opg);
		}

	return Result;
}


DISPATCHER(BitMapMCC)
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

	case OM_GET:
		d1(KPrintF("%s/%ld: OM_GET\n", __FUNC__, __LINE__));
		Result = mGet(cl, obj, (struct opGet *) msg);
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
		d1(KPrintF("%s/%ld: MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl,obj,msg);
		break;
		}

	return Result;
}
DISPATCHER_END

static void ClearInstanceData(struct BitMapMCCInstance *inst)
{
	memset(inst, 0, sizeof(struct BitMapMCCInstance));
}

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitBitMappicClass(void)
{
	return MUI_CreateCustomClass(NULL, SUPERCLASS, NULL, sizeof(struct BitMapMCCInstance), DISPATCHER_REF(BitMapMCC));
}

void CleanupBitMappicClass(struct MUI_CustomClass *mcc)
{
	MUI_DeleteCustomClass(mcc);
}


