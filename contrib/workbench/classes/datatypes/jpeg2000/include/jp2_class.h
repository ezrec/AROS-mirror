/*
 * jpeg2000.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef JP2_CLASS_H
#define JP2_CLASS_H

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

/* jp2_class.c */
Class *initDTClass (struct ClassBase *libBase);
BOOL freeDTClass (struct ClassBase *libBase, Class *cl);

#define OK (0)
#define NOTOK DTERROR_INVALID_DATA
#define ERROR_EOF DTERROR_NOT_ENOUGH_DATA

#define ReadError(C) ((C == -1) ? IDOS->IoErr() : ERROR_EOF)
#define WriteError(C) IDOS->IoErr()

#endif
