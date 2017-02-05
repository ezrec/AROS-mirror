/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

#include "au_class.h"
#include "endian.h"
#include "au_format.h"
#include "decoders.h"

#define min(a,b) ((a <= b) ? (a) : (b))
#define max(a,b) ((a <= b) ? (b) : (a))

DISPATCHERPROTO(ClassDispatch);

static int32 WriteAU (struct ClassBase * libBase, Object * obj, struct dtWrite * msg);
static int32 ConvertAU (struct ClassBase * libBase, BPTR FileHandle, int8 ** ChannelsPtr, struct VoiceHeader * vhdr);
static int32 GetAU (struct ClassBase * libBase, Object * obj, struct TagItem * Tags);

static int32 FileSize (struct ClassBase *libBase, BPTR FileHandle, int32 *Error);
static int32 ReadAUFragments (struct ClassBase *libBase, BPTR FileHandle, struct DecoderData *data);

Class * initDTClass (struct ClassBase * libBase) {
	libBase->DTClass = NULL;

	SoundDTLib = IExec->OpenLibrary("datatypes/sound.datatype", 44);
	if (SoundDTLib) {

		libBase->DTClass = IIntuition->MakeClass(libBase->libNode.lib_Node.ln_Name, SOUNDDTCLASS, NULL, 0, 0);
		if (libBase->DTClass) {
			libBase->DTClass->cl_Dispatcher.h_Entry = (HOOKFUNC)ClassDispatch;
			libBase->DTClass->cl_UserData = (uint32)libBase;
			IIntuition->AddClass(libBase->DTClass);
		} else {
			IExec->CloseLibrary(SoundDTLib);
			SoundDTLib = NULL;
		}

	}

	return(libBase->DTClass);
}

BOOL freeDTClass (struct ClassBase * libBase) {
	BOOL result = TRUE;
	if (libBase->DTClass) {
		if (!IIntuition->FreeClass(libBase->DTClass)) {
			result = FALSE;
		} else {
			libBase->DTClass = NULL;
			IExec->CloseLibrary(SoundDTLib);
			SoundDTLib = NULL;
		}
	}
	return(result);
}

DISPATCHERPROTO(ClassDispatch) {
	struct ClassBase * libBase;
	uint32 retv;

	libBase = (struct ClassBase *)cl->cl_UserData;

	switch(msg->MethodID) {

		case OM_NEW:
			retv = (uint32)IIntuition->IDoSuperMethodA(cl, obj, msg);
			if (retv) {
				int32 error;
				error = GetAU(libBase, (Object *)retv, ((struct opSet *)msg)->ops_AttrList);
				if (error != OK) {
					IIntuition->ICoerceMethod(cl, (Object *)retv, OM_DISPOSE);
					retv = (uint32)NULL;
					IDOS->SetIoErr(error);
				}
			}
			break;

		case DTM_WRITE:
			/* check if dt's native format should be used */
			if (((struct dtWrite *)msg)->dtw_Mode == DTWM_RAW) {
				int32 error;
				error = WriteAU(libBase, obj, (struct dtWrite *)msg);
				if (error == OK) {
					retv = TRUE;
				} else {
					IDOS->SetIoErr(error);
					retv = FALSE;
				}
				break;
			}
			/* fall through (let superclass handle it) */

		default:
			retv = (uint32)IIntuition->IDoSuperMethodA(cl, obj, msg);
			break;

	}

	return retv;
}

static int32 WriteAU (struct ClassBase * libBase, Object * obj, struct dtWrite * msg) {
	return ERROR_NOT_IMPLEMENTED;
}

static int32 ConvertAU (struct ClassBase * libBase, BPTR FileHandle, int8 ** ChannelsPtr, struct VoiceHeader * vhdr) {
	int32 Status, Error = OK;
	struct DecoderData data = {0};

	data.libBase = libBase;

	/* read header */
	Status = IDOS->Read(FileHandle, &data.au, sizeof(SNDSoundStruct));
	if (Status != sizeof(SNDSoundStruct)) {
		Error = ReadError(Status);
		goto out;
	}
	if (data.au.magic != AU_MAGIC) {
		Error = NOTOK;
		goto out;
	}

	if (data.au.encoding == AU_FMT_FRAGMENT) {
		Error = ReadAUFragments(libBase, FileHandle, &data);
		goto out;
	}

	if (data.au.data_offset < 24) data.au.data_offset = 24;
	//if (data.au.data_size == ~0)
	data.au.data_size = max(FileSize(libBase, FileHandle, &Error) - data.au.data_offset, 0);
	if (Error) goto out;
	Error = DecoderSetup(&data);
	if (Error) goto out;

	/* skip extra info */
	if (IDOS->Seek(FileHandle, data.au.data_offset, OFFSET_BEGINNING) == -1) {
		Error = IDOS->IoErr();
		goto out;
	}

	/* read sample data */
	{

		int8 * dst[2];
		int32 bytes_left, bytes_read = 0;
		int32 fr_left, fr_read = 0, fr = 0;
		dst[0] = data.output.buffer[0];
		dst[1] = data.output.buffer[1];
		fr_left = data.output.frames;
		for (bytes_left = data.au.data_size; bytes_left > 0; bytes_left -=bytes_read) {

			bytes_read = (data.decode.size > bytes_left) ?
				bytes_left : data.decode.size;

			Status = IDOS->Read(FileHandle, data.decode.buffer, bytes_read);
			if (Status != bytes_read) {
				Error = ReadError(Status);
				break;
			}

			fr_read = (data.decode.frames > fr_left) ?
				fr_left : data.decode.frames;

			Status = data.Decode(data.decode.buffer, fr, fr_read, dst, &data);

			fr_left -= fr_read;
			fr += fr_read;

			if (Status != fr) {
				Error = NOTOK;
				break;
			}

		}

	}

out:
	if (Error == OK) {
		vhdr->vh_OneShotHiSamples = data.output.frames;
		vhdr->vh_SamplesPerSec = data.au.sample_rate;
		vhdr->vh_Octaves = 1;
		vhdr->vh_Compression = 0; // none
		vhdr->vh_Volume = 0x10000; // max volume
		ChannelsPtr[0] = data.output.buffer[0];
		ChannelsPtr[1] = data.output.buffer[1];
	}
	DecoderCleanup(&data, Error);
	return Error;
}

static int32 GetAU (struct ClassBase * libBase, Object * obj, struct TagItem * Tags) {
	struct VoiceHeader * vhdr = NULL;
	char *	fileName;
	int32	srcType = -1;
	int32	Error = ERROR_OBJECT_NOT_FOUND;
	BPTR	FileHandle = (BPTR)NULL;

	fileName = (char *)IUtility->GetTagData(DTA_Name, (uint32)"Untitled", Tags);

	IDataTypes->GetDTAttrs(obj,
		SDTA_VoiceHeader,	&vhdr,
		DTA_Handle,		&FileHandle,
		DTA_SourceType,	&srcType,
	TAG_END);

	/* Do we have everything we need? */
	if (vhdr && FileHandle && srcType == DTST_FILE) {
		int8 *	Channel[MAX_CHANNELS] = {0};
		/* Convert the audio file */
		Error = ConvertAU(libBase, FileHandle, Channel, vhdr);
		if (Error == OK) {
			int8 * Sample = NULL;

			if (Channel[0] == NULL || Channel[1] == NULL) {
				if (Channel[0] != NULL)
					Sample = Channel[0];
				else
					Sample = Channel[1];
			}

			/* Fill in the remaining information */
			IDataTypes->SetDTAttrs(obj, NULL, NULL,
				DTA_ObjName,			IDOS->FilePart(fileName),
				SDTA_Sample,			Sample,
				SDTA_LeftSample,		Channel[0],
				SDTA_RightSample,		Channel[1],
				SDTA_SampleLength,	vhdr->vh_OneShotHiSamples,
				SDTA_SamplesPerSec,	vhdr->vh_SamplesPerSec,
				SDTA_Volume,			64,
				SDTA_Cycles,			1,
			TAG_END);
		}
	}

	return Error;
}

static int32 FileSize (struct ClassBase *libBase, BPTR FileHandle, int32 *Error) {
	struct FileInfoBlock *fib;
	int32 filesize = 0;
	fib = IDOS->AllocDosObject(DOS_FIB, NULL);
	if (fib) {
		if (IDOS->ExamineFH(FileHandle, fib)) {
			filesize = fib->fib_Size;
		} else
			*Error = IDOS->IoErr();
		IDOS->FreeDosObject(DOS_FIB, fib);
	} else
		*Error = ERROR_NO_FREE_STORE;
	return filesize;
}

static int32 ReadAUFragments (struct ClassBase *libBase, BPTR FileHandle, struct DecoderData *data) {
	return ERROR_NOT_IMPLEMENTED;
}
