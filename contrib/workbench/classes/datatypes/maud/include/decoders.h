/*
 *  maud.datatype
 *  (C) Fredrik Wikstrom
 *
 */
 
#ifndef DECODERS_H
#define DECODERS_H

#ifndef MAUD_FORMAT_H
#include "maud_format.h"
#endif

struct Decoder;

struct DecData {
	struct ClassBase *LibBase;
	const struct Decoder *Decoder;
	struct ContextNode *Chunk;
	struct MaudHeader *Header;
	int32 BlockSize;
	int32 BlockFrames;
	APTR State;
};

struct Decoder {
	int32 Compression;
	int32 (*Start) (struct DecData *data);
	void (*Stop) (struct DecData *data);
	int32 (*Decode) (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames);
	int32 (*DecodeFrames) (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames);
};

const struct Decoder *GetDecoder (uint16 compression);

#endif
