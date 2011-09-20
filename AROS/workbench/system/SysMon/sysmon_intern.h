/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef _SYSMON_INTERN_H
#define _SYSMON_INTERN_H

#include <libraries/mui.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#define MEMORY_RAM  0
#define MEMORY_CHIP 1
#define MEMORY_FAST 2
#define MEMORY_VRAM 3
#define MEMORY_GART 4

struct SysMonData
{
    Object * memorysize[5];
    Object * memoryfree[5];
};

BOOL InitMemory();
VOID DeInitMemory();
VOID UpdateMemoryInformation(struct SysMonData * smdata);
VOID UpdateMemoryStaticInformation(struct SysMonData * smdata);

BOOL InitVideo();
VOID DeInitVideo();
VOID UpdateVideoInformation(struct SysMonData * smdata);
VOID UpdateVideoStaticInformation(struct SysMonData * smdata);

#endif
