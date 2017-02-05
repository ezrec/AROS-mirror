/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

#include "au_class.h"
#include "au_format.h"
#include "au_alaw.h"

static int16 decode_alaw (uint8 a_val);
static int16 decode_mulaw(uint8 u_val);

DEC_SETUPPROTO(SetupALAW) {
	data->sample.size = 1;
	data->block.size = data->sample.size * data->au.channels;
	data->block.frames = 1;
	return OK;
}

DECODERPROTO(DecodeALAW) {
	int32 fr, ch;
	for (fr = 0; fr < numFrames; fr++) {
		for (ch = 0; ch < data->au.channels; ch++) {
			*Dst[ch]++ = decode_alaw(*Src++) >> 8;
		}
	}
	return frameIndex + numFrames;
}

DECODERPROTO(DecodeULAW) {
	int32 fr, ch;
	for (fr = 0; fr < numFrames; fr++) {
		for (ch = 0; ch < data->au.channels; ch++) {
			*Dst[ch]++ = decode_mulaw(*Src++) >> 8;
		}
	}
	return frameIndex + numFrames;
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
