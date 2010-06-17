#ifndef CLIB_OSS_PROTOS_H
#define CLIB_OSS_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/arch/all-unix/libs/oss/oss.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>


__BEGIN_DECLS

BOOL OSS_Open(char *filename, BOOL read, BOOL write, BOOL blocking);
void OSS_Close();
void OSS_Reset();
BOOL OSS_SetFragmentSize(int num_fragments, int fragment_size);
BOOL OSS_GetOutputInfo(int *num_fragments_available, int *num_fragments_allocated, int *fragment_size, int *num_bytes_available);
BOOL OSS_GetOutputPointer(int *processed_bytes, int *fragment_transitions, int *dmapointer);
BOOL OSS_FormatSupported_S8();
BOOL OSS_FormatSupported_U8();
BOOL OSS_FormatSupported_S16LE();
BOOL OSS_FormatSupported_S16BE();
BOOL OSS_FormatSupported_U16LE();
BOOL OSS_FormatSupported_U16BE();
int OSS_Revision();
BOOL OSS_Capability_Duplex();
BOOL OSS_Capability_Realtime();
BOOL OSS_Capability_Trigger();
BOOL OSS_Capability_MMap();
BOOL OSS_SetFormat_S8();
BOOL OSS_SetFormat_U8();
BOOL OSS_SetFormat_S16LE();
BOOL OSS_SetFormat_S16BE();
BOOL OSS_SetFormat_U16LE();
BOOL OSS_SetFormat_U16BE();
BOOL OSS_SetStereo();
BOOL OSS_SetMono();
BOOL OSS_SetNumChannels(int numchannels);
BOOL OSS_SetWriteRate(int rate, int *used_rate);
BOOL OSS_MMap(APTR *mapped_address, int len, BOOL read, BOOL write);
void OSS_MUnmap(APTR mapped_address, int len);
BOOL OSS_SetTrigger(BOOL input, BOOL output);
int OSS_Write(APTR buf, int size);

__END_DECLS

#endif /* CLIB_OSS_PROTOS_H */
