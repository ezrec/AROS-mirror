
#include <libraries/iffparse.h>
#include "Studio16file.h"

struct filesave {
	UBYTE			 fs_Flags;
	UBYTE			 fs_Pad1;
	UWORD			 fs_DisableCount;
	BYTE			 fs_EnableSignal;
	BYTE			 fs_DisableSignal;
	BYTE			 fs_MasterSignal;
	BYTE			 fs_SlaveSignal;
	struct Process		*fs_MasterTask;
	struct Process		*fs_SlaveTask;
	struct FileRequester	*fs_FileReq;
	struct Library		*fs_AHIsubBase;
	ULONG			 fs_Format;
	APTR			 fs_MixBuffer;
	APTR			 fs_SaveBuffer;
	APTR			 fs_SaveBuffer2;
	ULONG			 fs_SaveBufferSize;

	BYTE			 fs_RecMasterSignal;
	BYTE			 fs_RecSlaveSignal;
	struct Process		*fs_RecSlaveTask;
	struct FileRequester	*fs_RecFileReq;
	WORD			*fs_RecBuffer;

};


#define AHIDB_FileSaveFormat	(AHIDB_UserBase+0)	/* Private tag */
#define FORMAT_8SVX		0
#define FORMAT_AIFF		1
#define FORMAT_AIFC		2
#define FORMAT_S16		3
#define FORMAT_WAVE		4

/* AIFF, AIFC and 8SVX defs
   AIFF and AIFC defines was taken from Olaf `Olsen' Barthel's AIFF DataType. */

	// 80 bit IEEE Standard 754 floating point number

typedef struct {
	unsigned short	exponent;		// Exponent, bit #15 is sign bit for mantissa
	unsigned long	mantissa[2];		// 64 bit mantissa
} extended;

	// Audio Interchange Format chunk data

#define ID_AIFF MAKE_ID('A','I','F','F')
#define ID_AIFC MAKE_ID('A','I','F','C')

#define ID_FVER MAKE_ID('F','V','E','R')
#define ID_COMM MAKE_ID('C','O','M','M')
#define ID_SSND MAKE_ID('S','S','N','D')

	// "COMM" chunk header

typedef struct {
	short		numChannels;		// Number of channels
	unsigned long	numSampleFrames;	// Number of sample frames
	short		sampleSize;		// Number of bits per sample point
	extended	sampleRate;		// Replay rate in samples per second
} CommonChunk;

	// The same for "AIFC" type files

#define NO_COMPRESSION MAKE_ID('N','O','N','E') // No sound compression

typedef struct {
	short		numChannels;		// Number of channels
	unsigned long	numSampleFrames;	// Number of sample frames
	short		sampleSize;		// Number of bits per sample point
	extended	sampleRate;		// Replay rate in samples per second
	unsigned long	compressionType;	// Compression type
	char		compressionName[(sizeof("not compressed")+1)&(~1)];
} ExtCommonChunk;


	// "SSND" chunk header

typedef struct {
	unsigned long	offset, 		// Offset to sound data, for block alignment
			blockSize;		// Size of block data is aligned to
} SampledSoundHeader;

	// "FVER" chunk header

typedef struct {
	long		timestamp;		// Format version creation date
} FormatVersionHeader;

#define AIFCVersion1 0xA2805140 		// "AIFC" file format version #1


/* WAV file format */

#define ID_RIFF MAKE_ID('R','I','F','F')
#define ID_WAVE MAKE_ID('W','A','V','E')
#define ID_fmt  MAKE_ID('f','m','t',' ')
#define ID_data MAKE_ID('d','a','t','a')


typedef struct {
	unsigned short	formatTag;
	unsigned short	numChannels;
	unsigned long	samplesPerSec;
	unsigned long	avgBytesPerSec;
	unsigned short	blockAlign;
	unsigned short	bitsPerSample;
} FormatChunk;

#define WAVE_PCM 1
