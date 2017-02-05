/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

#ifndef AU_CLASS_H
#define AU_CLASS_H 1

#define GLOBAL_IFACES			(FALSE)
#define SUPPORTS_WRITING		(TRUE)
#define MIN_DECODE_BUFFER	(16 * 1024)
#define MIN_ENCODE_BUFFER		MIN_DECODE_BUFFER
#define MAX_CHANNELS			(2)

#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/classes.h>
#include <intuition/cghooks.h>

#include <dos/dosextens.h>

#include <exec/libraries.h>
#include <exec/interfaces.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/memory.h>

#include <devices/clipboard.h>

#include <datatypes/soundclass.h>

#define __NOLIBBASE__ 1
#if GLOBAL_IFACES == FALSE
#define __NOGLOBALIFACE__ 1
#endif

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/dtclass.h>

struct ClassBase {
	struct Library	libNode;		// Exec link
	uint16		Pad;			// 32-bit alignment
	Class *		DTClass;	// The class this library implements

	struct Library *	DOSLib;
	struct Library *	IntuitionLib;
	struct Library *	UtilityLib;
	struct Library *	DataTypesLib;
	struct Library *	SoundDTLib;

	#if GLOBAL_IFACES == FALSE
	struct ExecIFace *		IExec;
	struct DOSIFace *		IDOS;
	struct IntuitionIFace *	IIntuition;
	struct UtilityIFace *		IUtility;
	struct DataTypesIFace *	IDataTypes;
	#endif

	//struct SignalSemaphore	LockSemaphore;	// Shared access semaphore
	BPTR				SegList;
};

#define DOSLib			libBase->DOSLib
#define IntuitionLib	libBase->IntuitionLib
#define UtilityLib		libBase->UtilityLib
#define DataTypesLib	libBase->DataTypesLib
#define SoundDTLib		libBase->SoundDTLib

#if GLOBAL_IFACES == FALSE
#define IExec		libBase->IExec
#define IDOS		libBase->IDOS
#define IIntuition	libBase->IIntuition
#define IUtility		libBase->IUtility
#define IDataTypes	libBase->IDataTypes
#endif

#include <SDI_hook.h>

#if defined(__VBCC__) && defined(__amigaos4__)
#undef REG
#define REG(reg,arg) arg
#endif

Class * initDTClass (struct ClassBase *libBase);
BOOL freeDTClass (struct ClassBase *libBase);

#include "au_format.h"

#define DEC_SETUPPROTO(NAME) \
int32 NAME (struct DecoderData *data)

#define DEC_CLEANUPPROTO(NAME) \
void NAME (struct DecoderData *data)

#define DECODERPROTO(NAME) \
int32 NAME (uint8 *Src, int32 frameIndex, int32 numFrames, int8 **Dst, struct DecoderData *data)

struct DecoderData {
	struct ClassBase *	libBase;
	SNDSoundStruct		au;

	struct {
		int32 size; // decoder private
	} sample;

	struct {
		int32	size, frames;
	} block;

	struct {
		uint8 *	buffer;
		int32	size, blocks, frames;
	} decode;

	struct {
		int8 *	buffer[2];
		int32	size, blocks, frames;
	} output;

	const struct Decoder * decoder;
	DECODERPROTO((*Decode));
	void *	state; // decoder private
};

struct Decoder {
	uint32	encoding;
	DEC_SETUPPROTO((*Setup));
	DEC_CLEANUPPROTO((*Cleanup));
	DECODERPROTO((*Decode));
};

#define OK (0)
#define NOTOK DTERROR_INVALID_DATA
#define ERROR_EOF DTERROR_NOT_ENOUGH_DATA

#define ReadError(C) ((C == -1) ? IDOS->IoErr() : ERROR_EOF)
#define WriteError(C) IDOS->IoErr()

#endif
