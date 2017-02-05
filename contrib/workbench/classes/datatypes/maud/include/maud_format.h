#ifndef MAUD_FORMAT_H
#define MAUD_FORMAT_H

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

#define ID_MAUD MAKE_ID('M','A','U','D')
#define ID_MHDR MAKE_ID('M','H','D','R')
#define ID_MDAT MAKE_ID('M','D','A','T')
#define ID_MINF MAKE_ID('M','I','N','F') // unused

#pragma pack(2)

struct MaudHeader {
	uint32 mhdr_Samples;
	uint16 mhdr_SampleSizeC;
	uint16 mhdr_SampleSizeU;
	uint32 mhdr_RateSource;
	uint16 mhdr_RateDivide;
	uint16 mhdr_ChannelInfo;
	uint16 mhdr_Channels;
	uint16 mhdr_Compression;
	uint32 mhdr_Reserved[3];
};

#pragma pack()

// mhdr_ChannelInfo
enum {
	MCI_MONO = 0,
	MCI_STEREO,
	MCI_MULTIMONO,
	MCI_MULTISTEREO,
	MCI_MULTICHANNEL // unused
};

// mhdr_Compression
enum {
	MCOMP_NONE = 0,
	MCOMP_FIBDELTA,
	MCOMP_ALAW,
	MCOMP_ULAW,
	MCOMP_ADPCM2,
	MCOMP_ADPCM3,
	MCOMP_ADPCM4,
	MCOMP_ADPCM5,
	MCOMP_LONGDAT
};

#endif
