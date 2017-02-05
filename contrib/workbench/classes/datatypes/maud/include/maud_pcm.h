#ifndef MAUD_PCM_H
#define MAUD_PCM_H

#ifndef DECODERS_H
#include "decoders.h"
#endif

int32 StartPCM (struct DecData *data);
int32 DecodePCM (struct DecData *data, uint8 *src, int8 **dst, int32 num_blocks, int32 num_frames);

#endif
