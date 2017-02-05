/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

#include "au_class.h"
#include "au_format.h"
#include "au_pcm.h"

DEC_SETUPPROTO(SetupPCM) {
	switch (data->au.encoding) {
		case AU_FMT_8BIT_PCM:
			data->sample.size = 1;
			break;
		case AU_FMT_16BIT_PCM:
			data->sample.size = 2;
			break;
		case AU_FMT_24BIT_PCM:
			data->sample.size = 3;
			break;
		case AU_FMT_32BIT_PCM:
			data->sample.size = 4;
			break;
	}
	data->block.size = data->sample.size * data->au.channels;
	data->block.frames = 1;
	return OK;
}

DECODERPROTO(DecodePCM) {
	int32 fr, ch;
	for (fr = 0; fr < numFrames; fr++) {
		for (ch = 0; ch < data->au.channels; ch++) {
			*Dst[ch]++ = *Src;
			Src+= data->sample.size;
		}
	}
	return frameIndex + numFrames;
}
