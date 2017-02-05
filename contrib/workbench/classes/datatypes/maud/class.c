/*
 *	maud.datatype
 *	(c) Fredrik Wikstrom
 */

#include "class.h"

static uint32 ClassDispatch (Class *cl, Object *obj, Msg msg);

static int32 WriteMAUD (struct ClassBase *libBase, Object *obj, struct dtWrite *msg);
static int32 ConvertMAUD (struct ClassBase *libBase, struct IFFHandle *iff, int8 **ChannelsPtr,
	struct VoiceHeader *vhdr);
static int32 GetMAUD (struct ClassBase *libBase, Object *obj, struct TagItem *Tags);

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
				error = GetMAUD(libBase, (Object *)retval, ((struct opSet *)msg)->ops_AttrList);
				if (error != OK) {
					IIntuition->ICoerceMethod(cl, (Object *)retval, OM_DISPOSE);
					retval = (uint32)NULL;
					IDOS->SetIoErr(error);
				}
			}
			break;

#if 0
		case DTM_WRITE:
			/* check if dt's native format should be used */
			if (((struct dtWrite *)msg)->dtw_Mode == DTWM_RAW) {
				int32 error;
				error = WriteMAUD(libBase, obj, (struct dtWrite *)msg);
				if (error == OK) {
					retval = TRUE;
				} else {
					IDOS->SetIoErr(error);
					retval = FALSE;
				}
				break;
			}
			/* fall through (let superclass handle it) */
#endif

		default:
			retval = (uint32)IIntuition->IDoSuperMethodA(cl, obj, msg);
			break;

	}

	return(retval);
}

#if 0
static int32 WriteMAUD (struct ClassBase *libBase, Object *obj, struct dtWrite *msg) {
	return ERROR_NOT_IMPLEMENTED;
}
#endif

static int32 ConvertMAUD (struct ClassBase *libBase, struct IFFHandle *iff, int8 **ChannelsPtr,
	struct VoiceHeader *vhdr)
{
	struct DecData data = {0};
	static uint32 stop_chunks[] = {
		ID_MAUD,	ID_MHDR,
		ID_MAUD,	ID_MDAT
	};
	int32 error = OK;
	BOOL done = FALSE;
	struct MaudHeader mhdr = {0};
	int32 status;
	int32 i;
	int32 total_frames = 0;
	int32 num_blocks = 0;
	int32 decodeBufferSize = 0;
	uint8 *decodeBuffer = NULL;
	int32 frequency = 0;
	
	error = IIFFParse->StopChunks(iff, stop_chunks, sizeof(stop_chunks)/8);
	if (error) return error;
	
	while (!done && (error = IIFFParse->ParseIFF(iff, IFFPARSE_SCAN)) == OK) {
		data.Chunk = IIFFParse->CurrentChunk(iff);
		switch (data.Chunk->cn_ID) {
			case ID_MHDR:
				if (data.Chunk->cn_Size != sizeof(mhdr)) {
					error = ERROR_BAD_NUMBER;
					break;
				}
				status = IIFFParse->ReadChunkBytes(iff, &mhdr, sizeof(mhdr));
				if (status != sizeof(mhdr)) {
					error = IFFErr(status);
					break;
				}
				data.Header = &mhdr;
				data.Decoder = GetDecoder(mhdr.mhdr_Compression);
				if (!data.Decoder) {
					error = DTERROR_UNKNOWN_COMPRESSION;
					break;
				}
				if (!mhdr.mhdr_RateSource || !mhdr.mhdr_RateDivide) {
					error = ERROR_BAD_NUMBER;
					break;
				}
				frequency = (mhdr.mhdr_RateSource + ((uint32)mhdr.mhdr_RateDivide-1)) /
					(uint32)mhdr.mhdr_RateDivide;
				switch (mhdr.mhdr_ChannelInfo) {
					case MCI_MONO:
						if (mhdr.mhdr_Channels != 1) error = ERROR_BAD_NUMBER;
						break;
					case MCI_STEREO:
						if (mhdr.mhdr_Channels != 2) error = ERROR_BAD_NUMBER;
						break;
					default:
						error = DTERROR_UNKNOWN_COMPRESSION;
						break;
				}
				if (error) break;
				error = data.Decoder->Start(&data);
				break;

			case ID_MDAT:
				total_frames = data.Chunk->cn_Size / data.BlockSize * data.BlockFrames;
				if (!data.Header) {
					error = NOTOK;
					break;
				}
				for (i = 0; i < mhdr.mhdr_Channels; i++) {
					ChannelsPtr[i] = IExec->AllocVec(total_frames, MEMF_SHARED);
					if (!ChannelsPtr[i]) {
						error = ERROR_NO_FREE_STORE;
						break;
					}
				}
				if (error) break;
				num_blocks = (SND_BUFFER_SIZE + (data.BlockSize >> 1)) / data.BlockSize;
				if (!num_blocks) num_blocks++;
				do {
					decodeBufferSize = num_blocks * data.BlockSize;
					decodeBuffer = IExec->AllocVec(decodeBufferSize, MEMF_SHARED);
				} while (!decodeBuffer && num_blocks > 0);
				if (!decodeBuffer) {
					error = ERROR_NO_FREE_STORE;
					break;
				}
				{
					int8 *ChanPtr[MAX_CHANNELS];
					int32 read_size;
					int32 frames_left;
					int32 frames;
					int32 blocks_left;
					int32 blocks;
					
					for (i = 0; i < mhdr.mhdr_Channels; i++) {
						ChanPtr[i] = ChannelsPtr[i];
					}
					
					read_size = decodeBufferSize;
					frames_left = total_frames;
					frames = num_blocks * data.BlockFrames;
					blocks_left = data.Chunk->cn_Size / data.BlockSize;
					blocks = num_blocks;
					
					while (blocks_left > 0 && frames_left > 0) {
						if (blocks_left < blocks) {
							blocks = blocks_left;
							read_size = blocks * data.BlockSize;
						}
						if (frames_left < frames) {
							frames = frames_left;
						}
						
						status = IIFFParse->ReadChunkBytes(iff, decodeBuffer, read_size);
						if (status != read_size) {
							error = IFFErr(status);
							break;
						}
						status = data.Decoder->Decode(&data, decodeBuffer, ChanPtr, blocks, frames);
						if (status != frames) {
							error = NOTOK;
							break;
						}
						
						blocks_left -= blocks;
						frames_left -= frames;
					}
				}
				done = TRUE;
				break;
		}
	}
	
	if (data.Decoder) {
		if (data.State && data.Decoder->Stop)
			data.Decoder->Stop(&data);
		else
			IExec->FreeVec(data.State);
	}
	
	if (error == OK) {
		vhdr->vh_OneShotHiSamples = total_frames;
		vhdr->vh_SamplesPerSec = frequency;
		vhdr->vh_Octaves = 1;
		vhdr->vh_Compression = 0;
		vhdr->vh_Volume = 0x10000;
	} else {
		for (i = 0; i < MAX_CHANNELS; i++) {
			IExec->FreeVec(ChannelsPtr[i]);
			ChannelsPtr[i] = NULL;
		}
	}

	return error;
}

static int32 GetMAUD (struct ClassBase *libBase, Object *obj, struct TagItem *Tags) {
	struct VoiceHeader	*vhdr = NULL;
	char	*FileName;
	int32	SourceType = -1;
	int32	error = ERROR_REQUIRED_ARG_MISSING;
	struct IFFHandle *iff = NULL;

	FileName = (char *)IUtility->GetTagData(DTA_Name, (uint32)"Untitled", Tags);

	IDataTypes->GetDTAttrs(obj,
		SDTA_VoiceHeader,	&vhdr,
		DTA_Handle,			&iff,
		DTA_SourceType,		&SourceType,
	TAG_END);

	/* Do we have everything we need? */
	if (vhdr && iff && (SourceType == DTST_FILE || SourceType == DTST_CLIPBOARD)) {
		int8 *Channel[MAX_CHANNELS] = {0};
		/* Convert the audio file */
		error = ConvertMAUD(libBase, iff, Channel, vhdr);
		if (error == OK) {
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
		if (SourceType != DTST_FILE &&  SourceType != DTST_CLIPBOARD)
			error = ERROR_NOT_IMPLEMENTED;

	return error;
}
