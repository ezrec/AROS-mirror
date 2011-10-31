/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef HOSTGL_FUNCS_H
#define HOSTGL_FUNCS_H

#include "hostgl_types.h"

BOOL HostGL_FillFBAttributes(LONG * fbattributes, LONG size, struct TagItem *tagList);
VOID HostGL_CheckAndUpdateBufferSize(AROSMesaContext amesa);
#if defined(RENDERER_PBUFFER_WPA)
VOID HostGL_AllocatePBuffer(AROSMesaContext amesa);
VOID HostGL_DeAllocatePBuffer(AROSMesaContext amesa);
#endif

#endif /* HOSTGL_FUNCS_H */
