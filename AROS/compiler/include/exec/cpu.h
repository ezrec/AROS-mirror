/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef EXEC_CPU_H
#define EXEC_CPU_H

#include <aros/config.h>

#if AROS_SMP
#include <proto/kernel.h>

struct ExecCPUInfo {
    struct Node ec_Node;
    ULONG       ec_CPUNumber;
    struct Task *ThisTask;       /* Pointer to currently running task (readable) */
    ULONG       IdleCount;      /* Incremented when system goes idle            */
    ULONG       DispCount;      /* Incremented when a task is dispatched        */
    UWORD       Elapsed;        /* # of ticks, the current task has run         */
    UWORD       AttnResched;    /* Private scheduler flags                      */
    UWORD       SysFlags;       /* Private flags                                */
};

#define THISCPU (KernelBase ? (struct ExecCPUInfo *)KrnGetCPUStorage() : NULL)
#else
#define ExecCPUInfo ExecBase
#define THISCPU  SysBase
#endif

#endif /* EXEC_CPU_H */
