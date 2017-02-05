/*
 *	wavpack.datatype
 *	(c) Fredrik Wikstrom
 */

#ifndef WAVPACK_CLASS_H
#define WAVPACK_CLASS_H 1

#define SND_BUFFER_SIZE 		(16 << 10) // size of buffer used for decoding/encoding
#define MAX_CHANNELS			(2)

#include <exec/exec.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/classes.h>
#include <intuition/cghooks.h>
#include <datatypes/soundclass.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/dtclass.h>

#include "endian.h"
#include "debug.h"

struct ClassBase {
	struct Library	libNode;		// Exec link
	uint16			Pad;			// 32-bit alignment
	Class *			DTClass;	// The class this library implements

	BPTR			SegList;

	struct ExecIFace		*IExec;
	struct IntuitionIFace	*IIntuition;
	struct UtilityIFace		*IUtility;
	struct DataTypesIFace	*IDataTypes;

	struct Library	*IntuitionBase;
	struct Library	*UtilityBase;
	struct Library	*DataTypesBase;
	struct Library	*SoundDTBase;
};

#define IntuitionBase	libBase->IntuitionBase
#define UtilityBase		libBase->UtilityBase
#define DataTypesBase	libBase->DataTypesBase
#define SoundDTBase		libBase->SoundDTBase

#define IExec		libBase->IExec
#define IIntuition	libBase->IIntuition
#define IUtility	libBase->IUtility
#define IDataTypes	libBase->IDataTypes

/* wave_class.c */
Class *initDTClass (struct ClassBase *libBase);
BOOL freeDTClass (struct ClassBase *libBase, Class *cl);

#define OK (0)
#define NOTOK DTERROR_INVALID_DATA
#define ERROR_EOF DTERROR_NOT_ENOUGH_DATA

#define ReadError(C) ((C == -1) ? IDOS->IoErr() : ERROR_EOF)
#define WriteError(C) IDOS->IoErr()

#endif
