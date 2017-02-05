/*
 * icns.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef ICNS_CLASS_H
#define ICNS_CLASS_H

#include <exec/exec.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <utility/utility.h>
#include <datatypes/pictureclass.h>
#include <graphics/gfx.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/graphics.h>

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

	struct Library	*DOSBase;
	struct Library	*IntuitionBase;
	struct Library	*UtilityBase;
	struct Library	*DataTypesBase;
	struct Library	*GraphicsBase;
	struct Library	*PictureDTBase;
};

#define DOSBase			libBase->DOSBase
#define IntuitionBase	libBase->IntuitionBase
#define UtilityBase		libBase->UtilityBase
#define DataTypesBase	libBase->DataTypesBase
#define GraphicsBase	libBase->GraphicsBase
#define PictureDTBase	libBase->PictureDTBase
#define SuperClassBase	PictureDTBase

#define IExec		libBase->IExec
#define IDOS		libBase->IDOS
#define IIntuition	libBase->IIntuition
#define IUtility	libBase->IUtility
#define IDataTypes	libBase->IDataTypes
#define IGraphics	libBase->IGraphics

/* icns_class.c */
Class *initDTClass (struct ClassBase *libBase);
BOOL freeDTClass (struct ClassBase *libBase, Class *cl);

#include "icns_format.h"

#define OK (0)
#define NOTOK DTERROR_INVALID_DATA
#define ERROR_EOF DTERROR_NOT_ENOUGH_DATA

#define ReadError(C) ((C == -1) ? IDOS->IoErr() : ERROR_EOF)
#define WriteError(C) IDOS->IoErr()

/* icns_elements.c */
int32 ParseElements (Class *cl, BPTR file, struct ElementHeader *elem, struct IconInfo *info,
	uint32 type, uint32 index, uint32 *total);
BOOL GetIconInfo (uint32 type, struct IconInfo *info);
uint32 GetMaskType (uint32 type);

/* colormaps.c */
void SetCMAP (Class *cl, Object *o, int32 ncolors);

/* rle24_decode.c */
int32 rle24_decode (uint8 *in_ptr, int32 in_size, uint8 *out_ptr, int32 out_size, int32 pixel_count);

/* jp2_decode.c */
int32 icns_jp2_decode (struct IconInfo *info, uint8 *out);

#endif
