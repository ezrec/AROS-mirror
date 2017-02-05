#ifndef MAUD_ALAW_H
#define MAUD_ALAW_H

#ifndef DECODERS_H
#include "decoders.h"
#endif

int32 StartALaw (struct DecData *data);
int32 DecodeALaw (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames);
int32 DecodeMuLaw (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames);

#endif
