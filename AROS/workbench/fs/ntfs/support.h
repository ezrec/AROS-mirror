/*
 * ntfs.handler - New Technology FileSystem handler
 *
 * Copyright © 2012 The AROS Development Team
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the same terms as AROS itself.
 *
 * $Id $
 */

#ifndef NTFS_HANDLER_SUPPORT_H
#define NTFS_HANDLER_SUPPORT_H

#include <exec/types.h>
#include <dos/dosextens.h>

#if 0
// AROS ABI v1
#define BSTR_Str AROS_BSTR_ADDR
#define BSTR_Len AROS_BSTR_strlen
#else
// AROS ABI v0
#define BSTR_Str(bstrptr) &bstrptr[1]
#define BSTR_Len(bstrptr) bstrptr[0]
#endif

void SendEvent(LONG event);
void ErrorMessageArgs(char *fmt, IPTR *args);
APTR _AllocVecPooled(APTR mem_pool, ULONG size);
void _FreeVecPooled(APTR mem_pool, APTR vecaddr);

#define ErrorMessage(fmt, ...)		\
{					\
    IPTR __args[] = {__VA_ARGS__};	\
    ErrorMessageArgs(fmt, __args);	\
}

//#define log2 ilog2

#endif

