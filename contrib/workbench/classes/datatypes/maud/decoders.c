/*
 *
 *  maud.datatype
 *  (C) Fredrik Wikstrom
 *
 */

#include "decoders.h"
#include "maud_pcm.h"
#include "maud_alaw.h"

int32 DecodeBlocks (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames);

static const struct Decoder all_decoders[] = {
	{MCOMP_NONE, StartPCM,  NULL, DecodePCM,   NULL},
	{MCOMP_ALAW, StartALaw, NULL, DecodeALaw,  NULL},
	{MCOMP_ULAW, StartALaw, NULL, DecodeMuLaw, NULL},
	{-1}
};

const struct Decoder *GetDecoder (uint16 compression) {
	const struct Decoder *dec;
	for (dec = all_decoders; dec->Compression != -1; dec++) {
		if (dec->Compression == compression) {
			return dec;
		}
	}
	return NULL;
}

int32 DecodeBlocks (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames) {
	const struct Decoder *dec = data->Decoder;
	int32 status;
	int32 frames_left;
	int32 frames;
	int32 block_size;
	
	frames_left = num_frames;
	frames = data->BlockFrames;
	block_size = data->BlockSize;
	while (frames_left > 0) {
		if (frames_left < frames) frames = frames_left;
		status = dec->DecodeFrames(data, src, dst, 1, frames);
		if (status != frames) {
			return num_frames - frames_left + status;
		}
		src += block_size;
		frames_left -= frames;
	}
	return num_frames;
}
