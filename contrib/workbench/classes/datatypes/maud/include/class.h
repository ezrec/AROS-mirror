/*
 * maud.datatype
 * (C) Fredrik Wikstrom
 */

#ifndef CLASS_H
#define CLASS_H

#include <exec/exec.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <datatypes/soundclass.h>
#include <libraries/iffparse.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/iffparse.h>
#include <proto/dtclass.h>

struct ClassBase;

#include "maud_format.h"
#include "decoders.h"

struct ClassBase {
	struct Library libNode;
	uint16 Pad;
	Class *DTClass;
	BPTR SegList;
	
	struct ExecIFace *IExec;
	struct DOSIFace *IDOS;
	struct IntuitionIFace *IIntuition;
	struct UtilityIFace *IUtility;
	struct DataTypesIFace *IDataTypes;
	struct IFFParseIFace *IIFFParse;
	
	struct Library *DOSBase;
	struct Library *IntuitionBase;
	struct Library *UtilityBase;
	struct Library *DataTypesBase;
	struct Library *IFFParseBase;
	struct Library *SoundDTBase;
};

#define DOSBase libBase->DOSBase
#define IntuitionBase libBase->IntuitionBase
#define UtilityBase libBase->UtilityBase
#define DataTypesBase libBase->DataTypesBase
#define IFFParseBase libBase->IFFParseBase
#define SoundDTBase libBase->SoundDTBase

#define IExec libBase->IExec
#define IDOS libBase->IDOS
#define IIntuition libBase->IIntuition
#define IUtility libBase->IUtility
#define IDataTypes libBase->IDataTypes
#define IIFFParse libBase->IIFFParse

#define SND_BUFFER_SIZE (16 << 10)
#define MAX_CHANNELS 2
#define OK 0
#define NOTOK DTERROR_INVALID_DATA
#define ERROR_EOF DTERROR_NOT_ENOUGH_DATA

#define IFFErr(error) (((error) < 0) ? (error) : DTERROR_NOT_ENOUGH_DATA)

/* class.c */
Class *initDTClass (struct ClassBase *libBase);
BOOL freeDTClass (struct ClassBase *libBase, Class *cl);

#endif
