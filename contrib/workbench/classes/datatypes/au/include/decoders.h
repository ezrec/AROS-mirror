#ifndef DECODERS_H
#define DECODERS_H 1
/*
 *	sun_au.datatype by Fredrik Wikstrom
 *
 */

int32 DecoderSetup (struct DecoderData * data);
void DecoderCleanup (struct DecoderData * data, int32 error);

#endif
