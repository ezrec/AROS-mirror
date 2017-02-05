/*
 *	maud.datatype
 *	(C) Fredrik Wikstrom
 */

#include "class.h"
#include "maud_pcm.h"
#include "bitpack.h"

int32 StartPCM (struct DecData *data) {
	struct MaudHeader *mhdr = data->Header;
	uint32 bytes;
	uint32 frames;
	if (mhdr->mhdr_SampleSizeC == 0) {
		return ERROR_BAD_NUMBER;
	}
	bytes = mhdr->mhdr_SampleSizeC * mhdr->mhdr_Channels;
	frames = mhdr->mhdr_Channels << 3;
	while (!(bytes & 1) && !(frames & 1)) {
		bytes >>= 1;
		frames >>= 1;
	}
	data->BlockSize = bytes;
	data->BlockFrames = frames;
	return OK;
}

int32 DecodePCM (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames) {
	struct MaudHeader *mhdr = data->Header;
	struct BitPack_buffer_struct b;
	int32 chan, frame;
	int32 bits = mhdr->mhdr_SampleSizeC;
	
	bitpack_init_msb(&b, src, 0);
	if (bits > 8) {
		int32 skip = bits - 8;
		for (frame = 0; frame < num_frames; frame++) {
			for (chan = 0; chan < mhdr->mhdr_Channels; chan++) {
				*dst[chan]++ = bitpack_read_msb(&b, 8);
				bitpack_seek_msb(&b, skip, OFFSET_CURRENT);
			}
		}
	} else {
		int32 shift = 8 - bits;
		for (frame = 0; frame < num_frames; frame++) {
			for (chan = 0; chan < mhdr->mhdr_Channels; chan++) {
				*dst[chan]++ = bitpack_read_msb(&b, bits) << shift;
			}
		}
	}
	return num_frames;
}
