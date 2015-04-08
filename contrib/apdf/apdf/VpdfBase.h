/*************************************************************************\
 *                                                                       *
 * VpdfBase.h                                                            *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef VPDFBASE_H
#define VPDFBASE_H

#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <exec/types.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <exec/semaphores.h>
#include <exec/execbase.h>
#include <exec/devices.h>

#include "v_plugin.h"

void kprintf(const char *,...);

struct ApdfBase {
    struct Library          apdf_Lib;
    BOOL                    apdf_LoadPrefsFlag;
    struct Library         *apdf_SysBase;
    BPTR                    apdf_SegList;
    struct SignalSemaphore  apdf_Lock;
    union {
	APTR                apdf_MCC;
	struct ApdfBase    *apdf_Base2;
    } u;
    struct vplug_functable *apdf_FuncTable;
};

#endif

