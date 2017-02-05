/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

#include "au_class.h"
#include "au_format.h"
#include "au_pcm.h"
#include "bitpack.h"
#include "endian.h"

#include "g72x/g72x.h"

typedef struct {
	struct g72x_state stat[2];
	int (*decoder)(int code, int out_coding, struct g72x_state *state_ptr);
} AU_G72x_state;

DEC_SETUPPROTO(SetupG72x) {
	struct ClassBase * libBase;
	AU_G72x_state * state;
	libBase = data->libBase;
	int32 ch;
	data->state = state = (AU_G72x_state *)IExec->AllocVec(sizeof(AU_G72x_state), MEMF_PRIVATE|MEMF_CLEAR);
	if (!state)
		return ERROR_NO_FREE_STORE;
	for (ch = 0; ch < data->au.channels; ch++)
		g72x_init_state(&state->stat[ch]);
	switch (data->au.encoding) {

		case AU_FMT_4BIT_G721_ADPCM:
			data->block.size = 64;
			state->decoder = g721_decoder;
			break;

		case AU_FMT_3BIT_G723_ADPCM:
			data->block.size = 48;
			state->decoder = g723_24_decoder;
			break;

		case AU_FMT_5BIT_G723_ADPCM:
			data->block.size = 80;
			state->decoder = g723_40_decoder;
			break;

	}
	data->block.frames = 128;
	return OK;
}

/*DEC_CLEANUPPROTO(CleanupG72x) {
	struct ClassBase * libBase;
	libBase = data->libBase;
	IExec->FreeVec(data->state);
}*/

DECODERPROTO(DecodeG72x) {
	AU_G72x_state * state;
	BitPack_buffer bp;
	int32 fr, ch;
	state = (AU_G72x_state *)data->state;
	bitpack_init(&bp, Src, 0);
	for (fr = 0;  fr < numFrames; fr++) {
		for (ch = 0; ch < data->au.channels; ch++) {
			*Dst[ch]++ =
				(state->decoder(bitpack_read_msb(&bp, data->sample.size),
				AUDIO_ENCODING_LINEAR, &state->stat[ch]) >> 8);
		}
	}
	return frameIndex + numFrames;
}
