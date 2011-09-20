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
    Object * application;
    Object * mainwindow;

    CONST_STRPTR tabs [4];

    struct Hook tasklistdisplayhook;
    struct Hook tasklistrefreshbuttonhook;

    Object * tasklist;

    Object ** cpuusagegauges;
    Object ** cpufreqlabels;
    Object ** cpufreqvalues;

    Object * memorysize[5];
    Object * memoryfree[5];
    
    IPTR tasklistautorefresh;
};


BOOL InitMemory();
VOID DeInitMemory();
VOID UpdateMemoryInformation(struct SysMonData * smdata);
VOID UpdateMemoryStaticInformation(struct SysMonData * smdata);

BOOL InitVideo();
VOID DeInitVideo();
VOID UpdateVideoInformation(struct SysMonData * smdata);
VOID UpdateVideoStaticInformation(struct SysMonData * smdata);

BOOL InitProcessor();
VOID DeInitProcessor();
ULONG GetProcessorCount();
VOID UpdateProcessorInformation(struct SysMonData * smdata);
VOID UpdateProcessorStaticInformation(struct SysMonData * smdata);

struct TaskInfo;

BOOL InitTasks();
VOID DeInitTasks();
VOID UpdateTasksInformation(struct SysMonData * smdata);
VOID UpdateTasksStaticInformation(struct SysMonData * smdata);

AROS_UFP3(VOID, TasksListDisplayFunction,
    AROS_UFHA(struct Hook *, h,  A0),
    AROS_UFHA(STRPTR *, strings, A2),
    AROS_UFHA(struct TaskInfo *, obj, A1));

ULONG GetSIG_TIMER();
BOOL InitTimer();
VOID DeInitTimer();
VOID SignalMeAfter(ULONG msecs);
#endif
