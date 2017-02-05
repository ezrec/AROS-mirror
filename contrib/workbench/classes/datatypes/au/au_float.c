/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

#include "au_class.h"
#include "au_format.h"
#include "au_float.h"
#include "endian.h"

#if !defined(LITTLE_ENDIAN)
#define read_float(PTR) (*(float *)PTR)
#define read_double(PTR) (*(double *)PTR)
#endif

DEC_SETUPPROTO(SetupFloat) {
	if (data->au.encoding == AU_FMT_32BIT_IEEE_FLOAT)
		data->sample.size = 4;
	else
		data->sample.size = 8;
	data->block.size = data->sample.size * data->au.channels;
	data->block.frames = 1;
	return OK;
}

#ifndef read_float

static float read_float (uint8 *ptr) {
	int32 exp, mant;
	float f_val;

	exp = ((ptr[0] & 0x7F) << 1) | ((ptr[1] & 0x80) ? 1 : 0);
	mant = ((ptr[1] & 0x7F) << 16) | (ptr[2] << 8) | ptr[3];

	if (!exp && !mant) return 0.0;

	mant |= 0x800000;
	exp = exp ? exp-127 : 0;

	f_val = (mant) ? ((float)mant / ((float)0x800000)) : 0.0;

	if (ptr[0] & 0x80)
		f_val *= -1;

	if (exp > 0)
		f_val *= (1<<exp);
	else if (exp < 0)
		f_val /= (1<<(-exp));

	return f_val;
}

#endif

#ifndef read_double

static double read_double (uint8 *ptr) {
	int32 exp;
	double mant;
	double d_val;

	exp = ((ptr[0] & 0x7F) << 4) | (ptr[1] >> 4);
	mant = ((ptr[1] & 0xF) << 24) | (ptr[2] << 16) | (ptr[3] << 8) | ptr[4];
	mant += ((ptr[5] << 16) | (ptr[6] << 8) | ptr[7]) / ((double)0x1000000);

	if (!exp && !mant) return 0.0;

	mant += 0x10000000;
	exp = exp - 0x3FF;

	d_val = mant / ((double) 0x10000000);

	if (ptr[0] & 0x80)
		d_val *= -1;

	if (exp > 0)
		d_val *= (1<<exp);
	else if (exp < 0)
		d_val /= (1<<(-exp));

	return d_val;
}

#endif

DECODERPROTO(DecodeFloat32) {
	int32 fr, ch;
	for (fr = 0; fr < numFrames; fr++) {
		for (ch = 0; ch < data->au.channels; ch++) {
			*Dst[ch]++ = (int8)(read_float(Src) * 127);
			Src += 4;
		}
	}
	return frameIndex + numFrames;
}

DECODERPROTO(DecodeFloat64) {
	int32 fr, ch;
	for (fr = 0; fr < numFrames; fr++) {
		for (ch = 0; ch < data->au.channels; ch++) {
			*Dst[ch]++ = (int8)(read_double(Src) * 127);
			Src += 8;
		}
	}
	return frameIndex + numFrames;
}
