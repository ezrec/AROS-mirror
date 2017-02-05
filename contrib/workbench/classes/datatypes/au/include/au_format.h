#ifndef AU_FORMAT_H
#define AU_FORMAT_H 1
/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

/*
Links:
http://astronomy.swin.edu.au/~pbourke/dataformats/au/
http://www.cnpbagwell.com/AudioFormats-11.html
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef ENDIAN_H
#include "endian.h"
#endif

typedef struct {
	uint32 magic; // magic number
	uint32 data_offset; // offset (in file of data)
	uint32 data_size; // length of data (optional)
	uint32 encoding; // data encoding format
	uint32 sample_rate; // samples/sec
	uint32 channels; // number of interleaved channels
} SNDSoundStruct;

#define hdr_size data_offset
#define AU_MAGIC MAKE_ID('.','s','n','d')

enum {
	AU_FMT_ULAW = 1,	// 8-bit mu-law samples
	AU_FMT_8BIT_PCM,	// 8-bit linear samples
	AU_FMT_16BIT_PCM,	// 16-bit linear samples
	AU_FMT_24BIT_PCM,	// 24-bit linear samples
	AU_FMT_32BIT_PCM,	// 32-bit linear samples
	AU_FMT_32BIT_IEEE_FLOAT,	// floating-point samples
	AU_FMT_64BIT_IEEE_FLOAT,	// double-precision float samples
	AU_FMT_FRAGMENT,	// fragmented sampled data
	AU_FMT_NESTED,		// ?
	AU_FMT_DSP_CORE,	// DSP program
	AU_FMT_8BIT_FIXED,	// 8-bit fixed-point samples
	AU_FMT_16BIT_FIXED,	// 16-bit fixed-point samples
	AU_FMT_24BIT_FIXED,	// 24-bit fixed-point samples
	AU_FMT_32BIT_FIXED,	// 32-bit fixed-point samples
	AU_FMT_DISPLAY = 16,	// non-audio display data
	AU_FMT_ULAW_SQUELCH,	// ?
	AU_FMT_EMPHASIZED,	// 16-bit linear with emphasis
	AU_FMT_COMPRESSED,	// 16-bit linear with compression
	AU_FMT_EMP_COMP, // A combination of the two above
	AU_FMT_DSP_COMMANDS,	// Music Kit DSP commands
	AU_FMT_DSP_COMMANDS_SAMPLES, // ?
	AU_FMT_4BIT_G721_ADPCM,
	AU_FMT_G722_ADPCM,	// ?
	AU_FMT_3BIT_G723_ADPCM,
	AU_FMT_5BIT_G723_ADPCM,
	AU_FMT_ALAW
};

#endif
