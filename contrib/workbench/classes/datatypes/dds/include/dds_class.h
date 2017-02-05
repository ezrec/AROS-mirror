/*
 * dds.datatype
 * (c) Fredrik Wikstrom
 */

#ifndef DDS_CLASS_H
#define DDS_CLASS_H

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

	struct Library	*DOSLib;
	struct Library	*IntuitionLib;
	struct Library	*UtilityLib;
	struct Library	*DataTypesLib;
	struct Library	*GraphicsLib;
	struct Library	*PictureDTLib;
};

#define DOSLib			libBase->DOSLib
#define IntuitionLib	libBase->IntuitionLib
#define UtilityLib		libBase->UtilityLib
#define DataTypesLib	libBase->DataTypesLib
#define GraphicsLib		libBase->GraphicsLib
#define PictureDTLib	libBase->PictureDTLib
#define SuperClassLib	PictureDTLib

#define IExec		libBase->IExec
#define IDOS		libBase->IDOS
#define IIntuition	libBase->IIntuition
#define IUtility	libBase->IUtility
#define IDataTypes	libBase->IDataTypes
#define IGraphics	libBase->IGraphics

/* dds_class.c */
Class *initDTClass (struct ClassBase *libBase);
BOOL freeDTClass (struct ClassBase *libBase, Class *cl);

#include "dds_format.h"

#define OK (0)
#define NOTOK DTERROR_INVALID_DATA
#define ERROR_EOF DTERROR_NOT_ENOUGH_DATA

#define ReadError(C) ((C == -1) ? IDOS->IoErr() : ERROR_EOF)
#define WriteError(C) IDOS->IoErr()

/* rgb_read.c */
int32 ReadRGB (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct DDS_Header *dds);

/* dxt_read.c */
int32 ReadDXT1 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct DDS_Header *dds);
int32 ReadDXT2_3_4_5 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct DDS_Header *dds);

#endif
