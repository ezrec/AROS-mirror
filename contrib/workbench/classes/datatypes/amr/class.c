/*
 *	amr.datatype
 *	(c) Fredrik Wikstrom
 */

#include "class.h"
#include "interf_dec.h"

static uint32 ClassDispatch (Class *cl, Object *obj, Msg msg);

static int32 ConvertAMR (struct ClassBase *libBase, BPTR file, int8 **ChannelsPtr,
	struct VoiceHeader *vhdr);
static int32 GetAMR (struct ClassBase *libBase, Object *obj, struct TagItem *Tags);

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
				error = GetAMR(libBase, (Object *)retval, ((struct opSet *)msg)->ops_AttrList);
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

static const short block_size[16]={ 12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0 };

static int32 ConvertAMR (struct ClassBase *libBase, BPTR file, int8 **ChannelsPtr,
	struct VoiceHeader *vhdr)
{
	int32 status, error = OK;
	uint64 filesize;
	uint8 *buffer = NULL, *src, *end;
	int16 *pcm = NULL, *ptr;
	int8 *dst;
	int32 frames, i;
	int32 dec_mode, read_size;
	int *destate = NULL;
	
	filesize = IDOS->GetFileSize(file);
	if (filesize == (uint64)-1) {
		error = IDOS->IoErr();
		goto out;
	}
	if (filesize > 0x7fffffff) {
		error = ERROR_OBJECT_TOO_LARGE;
		goto out;
	}
	
	buffer = IExec->AllocVec(filesize, MEMF_VIRTUAL|MEMF_SHARED);
	pcm = IExec->AllocVec(160*2, MEMF_PRIVATE);
	if (!buffer && !pcm) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}
	
	status = IDOS->Read(file, buffer, filesize);
	if (status != filesize) {
		error = ReadError(status);
		goto out;
	}
	
	if (memcmp(buffer, "#!AMR\n", 6)) {
		error = ERROR_OBJECT_WRONG_TYPE;
		goto out;
	}
	
	src = &buffer[6];
	end = &buffer[filesize];
	frames = 0;
	while (src < end) {
		dec_mode = (*src >> 3) & 0xF;
		read_size = block_size[dec_mode];
		src += read_size+1;
		frames += 160;
	}
	
	ChannelsPtr[0] = IExec->AllocVec(frames, MEMF_SHARED);
	destate = Decoder_Interface_init();
	if (!ChannelsPtr[0] || !destate) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	src = &buffer[6];
	dst = ChannelsPtr[0];
	while (src < end) {
		dec_mode = (*src >> 3) & 0xF;
		read_size = block_size[dec_mode];
		Decoder_Interface_Decode(destate, src, pcm, 0);
		src += read_size+1;
		ptr = pcm;
		for (i = 0; i < 160; i++) {
			*dst++ = *(int8 *)ptr; ptr++;
		}
	}

out:
	if (error == OK) {
		vhdr->vh_OneShotHiSamples = frames;
		vhdr->vh_SamplesPerSec = 8000;
		vhdr->vh_Octaves = 1;
		vhdr->vh_Compression = 0; // none
		vhdr->vh_Volume = 0x10000; // max volume
	} else {
		IExec->FreeVec(ChannelsPtr[0]);
		ChannelsPtr[0] = NULL;
	}
	if (destate) Decoder_Interface_exit(destate);
	IExec->FreeVec(buffer);
	IExec->FreeVec(pcm);
	
	return error;
}

static int32 GetAMR (struct ClassBase *libBase, Object *obj, struct TagItem *Tags) {
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
		Error = ConvertAMR(libBase, file, Channel, vhdr);
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
