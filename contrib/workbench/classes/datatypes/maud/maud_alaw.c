#include "class.h"
#include "maud_alaw.h"

static int16 decode_alaw (uint8 a_val);
static int16 decode_mulaw(uint8 u_val);

int32 StartALaw (struct DecData *data) {
	struct MaudHeader *mhdr = data->Header;
	if (mhdr->mhdr_SampleSizeC != 8) {
		return DTERROR_UNKNOWN_COMPRESSION;
	}
	data->BlockSize = mhdr->mhdr_Channels;
	data->BlockFrames = 1;
	return OK;
}

int32 DecodeALaw (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames) {
	struct MaudHeader *mhdr = data->Header;
	int32 frame, chan;
	for (frame = 0; frame < num_frames; frame++) {
		for (chan = 0; chan < mhdr->mhdr_Channels; chan++) {
			*dst[chan]++ = decode_alaw(*src++) >> 8;
		}
	}
	return num_frames;
}

int32 DecodeMuLaw (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames) {
	struct MaudHeader *mhdr = data->Header;
	int32 frame, chan;
	for (frame = 0; frame < num_frames; frame++) {
		for (chan = 0; chan < mhdr->mhdr_Channels; chan++) {
			*dst[chan]++ = decode_mulaw(*src++) >> 8;
		}
	}
	return num_frames;
}

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */
#define	BIAS		(0x84)		/* Bias for linear code. */

static int16 decode_alaw (uint8 a_val) {
	int16 t;
	int16 seg;

	a_val ^= 0x55;

	t = (a_val & QUANT_MASK) << 4;
	seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
	switch (seg) {
	case 0:
		t += 8;
		break;
	case 1:
		t += 0x108;
		break;
	default:
		t += 0x108;
		t <<= seg - 1;
	}
	return ((a_val & SIGN_BIT) ? t : -t);
}

static int16 decode_mulaw(uint8 u_val) {
	int16 t;

	/* Complement to obtain normal u-law value. */
	u_val = ~u_val;

	/*
	 * Extract and bias the quantization bits. Then
	 * shift up by the segment number and subtract out the bias.
	 */
	t = ((u_val & QUANT_MASK) << 3) + BIAS;
	t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

	return ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}
