/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

#include "au_class.h"
#include "endian.h"
#include "au_format.h"
#include "decoders.h"

#include "au_pcm.h"
#include "au_alaw.h"
#include "au_float.h"
#include "au_g72x.h"

static const struct Decoder all_decoders[] = {
	{ AU_FMT_8BIT_PCM,			SetupPCM,	NULL,		DecodePCM	},
	{ AU_FMT_16BIT_PCM,			SetupPCM,	NULL,		DecodePCM	},
	{ AU_FMT_24BIT_PCM,			SetupPCM,	NULL,		DecodePCM	},
	{ AU_FMT_32BIT_PCM,			SetupPCM,	NULL,		DecodePCM	},
	{ AU_FMT_ALAW,				SetupALAW,	NULL,		DecodeALAW	},
	{ AU_FMT_ULAW,				SetupALAW,	NULL,		DecodeULAW	},
	{ AU_FMT_32BIT_IEEE_FLOAT,		SetupFloat,	NULL,		DecodeFloat32	},
	{ AU_FMT_64BIT_IEEE_FLOAT,		SetupFloat,	NULL,		DecodeFloat64 },
	#ifdef DG72X_SUPPORT
	{ AU_FMT_4BIT_G721_ADPCM,	SetupG72x,	NULL,		DecodeG72x	},
	{ AU_FMT_3BIT_G723_ADPCM,	SetupG72x,	NULL,		DecodeG72x	},
	{ AU_FMT_5BIT_G723_ADPCM,	SetupG72x,	NULL,		DecodeG72x	},
	#endif
	{ 0 }
};

#define libBase data->libBase

const struct Decoder * FindDecoder (uint32 enc) {
	const struct Decoder * dec;
	for (dec = all_decoders; dec->encoding; dec++) {
		if (dec->encoding == enc)
			return dec;
	}
	return NULL;
}

int32 DecoderSetup (struct DecoderData * data) {
	int32 Error;

	data->decoder = FindDecoder(data->au.encoding);
	if (!data->decoder) return DTERROR_UNKNOWN_COMPRESSION;
	data->Decode = data->decoder->Decode;

	Error = data->decoder->Setup(data);
	if (Error) return Error;

	if (data->au.channels < 1 || data->au.channels > 2) return ERROR_NOT_IMPLEMENTED;

	data->output.blocks = data->au.data_size / data->block.size;
	data->output.frames = data->output.blocks * data->block.frames;
	data->output.size = data->output.frames;

	data->decode.blocks = MIN_DECODE_BUFFER / data->block.size;
	if (data->decode.blocks >= data->output.blocks) {
		data->decode.blocks = data->output.blocks;
		data->decode.size = data->decode.blocks * data->block.size;
		data->decode.frames = data->output.frames;
	} else {
		data->decode.size = data->decode.blocks * data->block.size;
		if (data->decode.size < MIN_DECODE_BUFFER) {
			data->decode.blocks++;
			data->decode.size+=data->block.size;
		}
		data->decode.frames = data->decode.blocks * data->block.frames;
	}

	data->decode.buffer = IExec->AllocVec(data->decode.size, MEMF_PRIVATE);
	if (!data->decode.buffer) return ERROR_NO_FREE_STORE;

	data->output.buffer[0] = IExec->AllocVec(data->output.size, MEMF_SHARED);
	if (!data->output.buffer[0]) return ERROR_NO_FREE_STORE;
	if (data->au.channels == 2) {
		data->output.buffer[1] = IExec->AllocVec(data->output.size, MEMF_SHARED);
		if (!data->output.buffer[1]) return ERROR_NO_FREE_STORE;
	}

	return OK;
}

void DecoderCleanup (struct DecoderData * data, int32 error) {
	if (error != OK) {
		IExec->FreeVec(data->output.buffer[1]);
		IExec->FreeVec(data->output.buffer[0]);
	}
	IExec->FreeVec(data->decode.buffer);
	if (data->state) {
		if (data->decoder->Cleanup)
			data->decoder->Cleanup(data);
		else
			IExec->FreeVec(data->state);
	}
}
