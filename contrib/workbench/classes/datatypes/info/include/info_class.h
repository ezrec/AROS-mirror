/*
 * info.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef INFO_CLASS_H
#define INFO_CLASS_H

#include <exec/exec.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <utility/utility.h>
#include <datatypes/pictureclass.h>
#include <graphics/gfx.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/graphics.h>
#include <proto/icon.h>

struct ClassBase {
	struct Library libNode;
	uint16	pad;
	Class	*DTClass;
	BPTR	SegList;

	struct ExecIFace		*IExec;
	struct DOSIFace			*IDOS;
	struct IntuitionIFace	*IIntuition;
	struct UtilityIFace		*IUtility;
	struct DataTypesIFace	*IDataTypes;
	struct GraphicsIFace	*IGraphics;
	struct IconIFace		*IIcon;

	struct Library	*DOSBase;
	struct Library	*IntuitionBase;
	struct Library	*UtilityBase;
	struct Library	*DataTypesBase;
	struct Library	*GraphicsBase;
	struct Library	*IconBase;
	struct Library	*PictureDTBase;
};

#define DOSBase			libBase->DOSBase
#define IntuitionBase	libBase->IntuitionBase
#define UtilityBase		libBase->UtilityBase
#define DataTypesBase	libBase->DataTypesBase
#define GraphicsBase	libBase->GraphicsBase
#define IconBase		libBase->IconBase
#define PictureDTBase	libBase->PictureDTBase
#define SuperClassBase	PictureDTBase

#define IExec		libBase->IExec
#define IDOS		libBase->IDOS
#define IIntuition	libBase->IIntuition
#define IUtility	libBase->IUtility
#define IDataTypes	libBase->IDataTypes
#define IGraphics	libBase->IGraphics
#define IIcon		libBase->IIcon

struct IconInfo {
	uint32 Width;
	uint32 Height;
	uint32 Format;
	uint32 NumColors;
	const struct ColorRegister *CMap;
	uint8 *Image;
	uint32 Which;
	uint32 NumImages;
};

/* info_class.c */
Class *initDTClass (struct ClassBase *libBase);
BOOL freeDTClass (struct ClassBase *libBase, Class *cl);

#define OK (0)
#define NOTOK DTERROR_INVALID_DATA
#define ERROR_EOF DTERROR_NOT_ENOUGH_DATA

#define ReadError(C) ((C == -1) ? IDOS->IoErr() : ERROR_EOF)
#define WriteError(C) IDOS->IoErr()

#ifndef ICONCTRLA_GetImageDataFormat

#define ICONCTRLA_SetImageDataFormat        (ICONA_Dummy + 0x67) /*103*/
#define ICONCTRLA_GetImageDataFormat        (ICONA_Dummy + 0x68) /*104*/

/* Values for ICONCTRLA_Set/GetImageDataFormat */
#define IDFMT_BITMAPPED     (0)  /* Bitmapped icon (planar, legacy) */
#define IDFMT_PALETTEMAPPED (1)  /* Palette mapped icon (chunky, V44+) */
#define IDFMT_DIRECTMAPPED  (2)  /* Direct mapped icon (truecolor 0xAARRGGBB, V51+) */

#endif

#endif
