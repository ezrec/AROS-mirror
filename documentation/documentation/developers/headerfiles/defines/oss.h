#ifndef DEFINES_OSS_H
#define DEFINES_OSS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/arch/all-unix/libs/oss/oss.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for oss
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS

#define OSS_Open ((BOOL (*)(char *filename, BOOL read, BOOL write, BOOL blocking))__AROS_GETVECADDR(OSSBase,5))
#define OSS_Close ((void (*)())__AROS_GETVECADDR(OSSBase,6))
#define OSS_Reset ((void (*)())__AROS_GETVECADDR(OSSBase,7))
#define OSS_SetFragmentSize ((BOOL (*)(int num_fragments, int fragment_size))__AROS_GETVECADDR(OSSBase,8))
#define OSS_GetOutputInfo ((BOOL (*)(int *num_fragments_available, int *num_fragments_allocated, int *fragment_size, int *num_bytes_available))__AROS_GETVECADDR(OSSBase,9))
#define OSS_GetOutputPointer ((BOOL (*)(int *processed_bytes, int *fragment_transitions, int *dmapointer))__AROS_GETVECADDR(OSSBase,10))
#define OSS_FormatSupported_S8 ((BOOL (*)())__AROS_GETVECADDR(OSSBase,11))
#define OSS_FormatSupported_U8 ((BOOL (*)())__AROS_GETVECADDR(OSSBase,12))
#define OSS_FormatSupported_S16LE ((BOOL (*)())__AROS_GETVECADDR(OSSBase,13))
#define OSS_FormatSupported_S16BE ((BOOL (*)())__AROS_GETVECADDR(OSSBase,14))
#define OSS_FormatSupported_U16LE ((BOOL (*)())__AROS_GETVECADDR(OSSBase,15))
#define OSS_FormatSupported_U16BE ((BOOL (*)())__AROS_GETVECADDR(OSSBase,16))
#define OSS_Revision ((int (*)())__AROS_GETVECADDR(OSSBase,17))
#define OSS_Capability_Duplex ((BOOL (*)())__AROS_GETVECADDR(OSSBase,18))
#define OSS_Capability_Realtime ((BOOL (*)())__AROS_GETVECADDR(OSSBase,19))
#define OSS_Capability_Trigger ((BOOL (*)())__AROS_GETVECADDR(OSSBase,20))
#define OSS_Capability_MMap ((BOOL (*)())__AROS_GETVECADDR(OSSBase,21))
#define OSS_SetFormat_S8 ((BOOL (*)())__AROS_GETVECADDR(OSSBase,22))
#define OSS_SetFormat_U8 ((BOOL (*)())__AROS_GETVECADDR(OSSBase,23))
#define OSS_SetFormat_S16LE ((BOOL (*)())__AROS_GETVECADDR(OSSBase,24))
#define OSS_SetFormat_S16BE ((BOOL (*)())__AROS_GETVECADDR(OSSBase,25))
#define OSS_SetFormat_U16LE ((BOOL (*)())__AROS_GETVECADDR(OSSBase,26))
#define OSS_SetFormat_U16BE ((BOOL (*)())__AROS_GETVECADDR(OSSBase,27))
#define OSS_SetStereo ((BOOL (*)())__AROS_GETVECADDR(OSSBase,28))
#define OSS_SetMono ((BOOL (*)())__AROS_GETVECADDR(OSSBase,29))
#define OSS_SetNumChannels ((BOOL (*)(int numchannels))__AROS_GETVECADDR(OSSBase,30))
#define OSS_SetWriteRate ((BOOL (*)(int rate, int *used_rate))__AROS_GETVECADDR(OSSBase,31))
#define OSS_MMap ((BOOL (*)(APTR *mapped_address, int len, BOOL read, BOOL write))__AROS_GETVECADDR(OSSBase,32))
#define OSS_MUnmap ((void (*)(APTR mapped_address, int len))__AROS_GETVECADDR(OSSBase,33))
#define OSS_SetTrigger ((BOOL (*)(BOOL input, BOOL output))__AROS_GETVECADDR(OSSBase,34))
#define OSS_Write ((int (*)(APTR buf, int size))__AROS_GETVECADDR(OSSBase,35))

__END_DECLS

#endif /* DEFINES_OSS_H*/
