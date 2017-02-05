/*
 *	bonk.datatype
 *	(c) Fredrik Wikstrom
 */

#include "class.h"
#include <bonk.h>

static uint32 ClassDispatch (Class *cl, Object *obj, Msg msg);

static int32 ConvertBonk (struct ClassBase *libBase, BPTR file, int8 **ChannelsPtr,
	struct VoiceHeader *vhdr);
static int32 GetBonk (struct ClassBase *libBase, Object *obj, struct TagItem *Tags);

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
				error = GetBonk(libBase, (Object *)retval, ((struct opSet *)msg)->ops_AttrList);
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

static int32 ConvertBonk (struct ClassBase *libBase, BPTR file, int8 **ChannelsPtr,
	struct VoiceHeader *vhdr)
{
	decoder *deco = NULL;
	int32 num_channels, chan;
	int32 frames, i;
	int32 error = OK;
	vector<int> samples;
	int8 *dst[2];
	
	deco = new decoder;
	if (!deco) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}
	
	if (!deco->begin(file)) {
		error = ERROR_OBJECT_WRONG_TYPE;
		goto out;
	}

	num_channels = deco->channels;
	if (num_channels != 1 && num_channels != 2) {
		error = ERROR_BAD_NUMBER;
		goto out;
	}
	
	frames = deco->length_remaining / deco->channels;
	for (chan = 0; chan < num_channels; chan++) {
		ChannelsPtr[chan] = (int8 *)IExec->AllocVec(frames, MEMF_SHARED);
		if (!ChannelsPtr[chan]) {
			error = ERROR_NO_FREE_STORE;
			goto out;
		}
	}
	
	dst[0] = ChannelsPtr[0];
	dst[1] = ChannelsPtr[1];
	chan = 0;
	while (deco->length_remaining) {
		deco->read_packet(samples);
		for (i = 0; i < samples.size(); i++) {
			if (samples[i] >= 32767)
				*dst[chan]++ = 127;
			else if (samples[i] <= -32768)
				*dst[chan]++ = -128;
			else
				*dst[chan]++ = samples[i] >> 8;
			if (++chan == num_channels) chan = 0;
		}
	}
	
out:
	if (error == OK) {
		vhdr->vh_OneShotHiSamples = frames;
		vhdr->vh_SamplesPerSec = deco->rate;
		vhdr->vh_Octaves = 1;
		vhdr->vh_Compression = 0;
		vhdr->vh_Volume = 0x10000;
	} else {
		IExec->FreeVec(ChannelsPtr[0]);
		IExec->FreeVec(ChannelsPtr[1]);
		ChannelsPtr[0] = NULL;
		ChannelsPtr[1] = NULL;
	}
	if (deco) delete(deco);

	return error;
}

static int32 GetBonk (struct ClassBase *libBase, Object *obj, struct TagItem *Tags) {
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
		Error = ConvertBonk(libBase, file, Channel, vhdr);
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
