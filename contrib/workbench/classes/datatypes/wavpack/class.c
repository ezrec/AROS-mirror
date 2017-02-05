/*
 *	wavpack.datatype
 *	(c) Fredrik Wikstrom
 */

#include "class.h"
#include "endian.h"
#include <stdio.h>
#include <math.h>
#include <wavpack.h>

static uint32 ClassDispatch (Class *cl, Object *obj, Msg msg);

static int32 ConvertWavPack (struct ClassBase *libBase, BPTR file, int8 **ChannelsPtr,
	struct VoiceHeader *vhdr);
static int32 GetWavPack (struct ClassBase *libBase, Object *obj, struct TagItem *Tags);

Class * initDTClass (struct ClassBase *libBase) {
	Class *cl = NULL;
	SoundDTBase = IExec->OpenLibrary("datatypes/sound.datatype", 44);
	if (SoundDTBase) {
		cl = IIntuition->MakeClass(libBase->libNode.lib_Node.ln_Name,
			SOUNDDTCLASS, NULL, 0, 0);
		if (cl) {
			cl->cl_Dispatcher.h_Entry = (HOOKFUNC)ClassDispatch;
			cl->cl_UserData = (uint32)libBase;
			IIntuition->AddClass(cl);
		} else {
			IExec->CloseLibrary(SoundDTBase);
			SoundDTBase = NULL;
		}
	}

	return cl;
}

BOOL freeDTClass (struct ClassBase *libBase, Class *cl) {
	BOOL result = TRUE;
	if (cl) {
		if (!IIntuition->FreeClass(cl)) {
			result = FALSE;
		} else {
			IExec->CloseLibrary(SoundDTBase);
			SoundDTBase = NULL;
		}
	}
	return(result);
}

static uint32 ClassDispatch (Class *cl, Object *obj, Msg msg) {
	struct ClassBase * libBase;
	uint32 retval;

	libBase = (struct ClassBase *)cl->cl_UserData;

	switch(msg->MethodID) {

		case OM_NEW:
			retval = (uint32)IIntuition->IDoSuperMethodA(cl, obj, msg);
			if (retval) {
				int32 error;
				error = GetWavPack(libBase, (Object *)retval, ((struct opSet *)msg)->ops_AttrList);
				if (error != OK) {
					IIntuition->ICoerceMethod(cl, (Object *)retval, OM_DISPOSE);
					retval = (uint32)NULL;
					IDOS->SetIoErr(error);
				}
			}
			break;

		default:
			retval = (uint32)IIntuition->IDoSuperMethodA(cl, obj, msg);
			break;

	}

	return(retval);
}

static int32_t ReadBytes (void *id, void *data, int32_t len) {
	BPTR file = (BPTR)id;
	int32_t res;
	res = IDOS->FRead(file, data, 1, len);
	return res == -1 ? 0 : res;
}

static uint32_t GetPos (void *id) {
	BPTR file = (BPTR)id;
	uint64_t res;
	res = IDOS->GetFilePosition(file);
	return (res > 0x7fffffff) ? -1 : res;
}

static int SetPosAbs (void *id, uint32_t pos) {
	BPTR file = (BPTR)id;
	return !IDOS->ChangeFilePosition(file, pos, OFFSET_BEGINNING);
}

static int SetPosRel (void *id, int32_t delta, int mode) {
	BPTR file = (BPTR)id;
	switch (mode) {
		case SEEK_SET: mode = OFFSET_BEGINNING; break;
		case SEEK_CUR: mode = OFFSET_CURRENT; break;
		case SEEK_END: mode = OFFSET_END; break;
		default: return -1;
	}
	return !IDOS->ChangeFilePosition(file, delta, mode);
}

static int PushBackByte (void *id, int c) {
	BPTR file = (BPTR)id;
	return IDOS->UnGetC(file, c) ? c : EOF;
}

static uint32_t GetLength (void *id) {
	BPTR file = (BPTR)id;
	uint64_t res;
	if (!file) return 0;
	res = IDOS->GetFileSize(file);
	return (res > 0x7fffffff) ? 0 : res;
}

static int CanSeek (void *id) {
	return TRUE;
}

static WavpackStreamReader wpc_reader = {
	ReadBytes,
	GetPos,
	SetPosAbs,
	SetPosRel,
	PushBackByte,
	GetLength,
	CanSeek,
	NULL
};

static int32 ConvertWavPack (struct ClassBase *libBase, BPTR file, int8 **ChannelsPtr,
	struct VoiceHeader *vhdr)
{
	WavpackContext *wpc = NULL;
	int32 frames, frequency, sample_size, length, mode;
	int32 num_channels, chan, getlen, buflen;
	void *buffer = NULL;
	char error_str[80];
	int32 error = OK;
	int8 *dst[2];
	
	wpc = WavpackOpenFileInputEx(&wpc_reader, (APTR)file, NULL, error_str, OPEN_2CH_MAX|OPEN_NORMALIZE, 0);
	if (!wpc) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}
	
	num_channels = WavpackGetReducedChannels(wpc);
	frequency = WavpackGetSampleRate(wpc);
	sample_size = WavpackGetBytesPerSample(wpc);
	frames = length = WavpackGetNumSamples(wpc);
	mode = WavpackGetMode(wpc);
	if (length == -1) {
		error = ERROR_REQUIRED_ARG_MISSING;
		goto out;
	}
	
	for (chan = 0; chan < num_channels; chan++) {
		ChannelsPtr[chan] = IExec->AllocVec(length, MEMF_SHARED);
		if (!ChannelsPtr[chan]) {
			error = ERROR_NO_FREE_STORE;
			goto out;
		}
	}
	
	buffer = IExec->AllocVec(16 << 10, MEMF_SHARED);
	if (!buffer) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}
	buflen = 4096 / num_channels;
	
	IDOS->SetIoErr(0);
	dst[0] = ChannelsPtr[0];
	dst[1] = ChannelsPtr[1];
	while (length > 0) {
		getlen = MIN(length, buflen);
		if (WavpackUnpackSamples(wpc, buffer, getlen) != getlen) {
			error = IDOS->IoErr();
			goto out;
		}
		length -= getlen;
		if (mode & MODE_FLOAT) {
			float32 *src = buffer;
			while (getlen--) {
				for (chan = 0; chan < num_channels; chan++) {
					if (*src >= 1.0)
						*dst[chan]++ = 127;
					else if (*src <= -1.0)
						*dst[chan]++ = -128;
					else
						*dst[chan]++ = floor(*src * 128.0);
					src++;
				}
			}
		} else {
			int8 *src = (int8 *)buffer + (4 - sample_size);
			while (getlen--) {
				for (chan = 0; chan < num_channels; chan++) {
					*dst[chan]++ = *src;
					src += 4;
				}
			}
		}
	}
	
out:
	if (wpc) WavpackCloseFile(wpc);
	IExec->FreeVec(buffer);
	if (error == OK) {
		vhdr->vh_OneShotHiSamples = frames;
		vhdr->vh_SamplesPerSec = frequency;
		vhdr->vh_Octaves = 1;
		vhdr->vh_Compression = 0;
		vhdr->vh_Volume = 0x10000;
	} else {
		IExec->FreeVec(ChannelsPtr[0]);
		IExec->FreeVec(ChannelsPtr[1]);
		ChannelsPtr[0] = NULL;
		ChannelsPtr[1] = NULL;
	}

	return error;
}

static int32 GetWavPack (struct ClassBase *libBase, Object *obj, struct TagItem *Tags) {
	struct VoiceHeader	*vhdr = NULL;
	char	*FileName;
	int32	SourceType = -1;
	int32	Error = ERROR_REQUIRED_ARG_MISSING;
	BPTR	file = (BPTR)NULL;

	FileName = (char *)IUtility->GetTagData(DTA_Name, (uint32)"Untitled", Tags);

	IDataTypes->GetDTAttrs(obj,
		SDTA_VoiceHeader,	&vhdr,
		DTA_Handle,			&file,
		DTA_SourceType,		&SourceType,
	TAG_END);

	/* Do we have everything we need? */
	if (vhdr && file && SourceType == DTST_FILE) {
		int8 *Channel[MAX_CHANNELS] = {0};
		/* Convert the audio file */
		Error = ConvertWavPack(libBase, file, Channel, vhdr);
		if (Error == OK) {
			int8 *Sample = NULL;

			if (!Channel[0] || !Channel[1]) {
				if (Channel[0])
					Sample = Channel[0];
				else
					Sample = Channel[1];
			}

			/* Fill in the remaining information */
			IDataTypes->SetDTAttrs(obj, NULL, NULL,
				DTA_ObjName,		IDOS->FilePart(FileName),
				SDTA_Sample,		Sample,
				SDTA_LeftSample,	Channel[0],
				SDTA_RightSample,	Channel[1],
				SDTA_SampleLength,	vhdr->vh_OneShotHiSamples,
				SDTA_SamplesPerSec,	vhdr->vh_SamplesPerSec,
				SDTA_Volume,		64,
				SDTA_Cycles,		1,
				TAG_END);
		}
	} else
		if (SourceType != DTST_FILE) Error = ERROR_NOT_IMPLEMENTED;

	return (Error);
}
