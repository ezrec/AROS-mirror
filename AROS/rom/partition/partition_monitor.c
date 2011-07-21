/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

*/

#include <aros/debug.h>

#include <exec/memory.h>
#include <exec/rawfmt.h>
#include <libraries/expansion.h>
#include <libraries/expansionbase.h>

#include <proto/exec.h>
#include <proto/partition.h>
#include <proto/expansion.h>
#include <proto/dos.h>

#include <string.h>

#include "partition_intern.h"
#include "partition_support.h"
#include "platform.h"

const TEXT task_prefix[] = "Monitor of ";
static void internalPartitionMonitor(IPTR ipm, IPTR parent, IPTR parentsig_ok, IPTR parentsig_err);

/*  FUNCTION
    Watch a PM for disk change events

    Send SIGBREAKF_CTRL_C to kill the task.

    RESULT
    struct Task * on success, NULL on failure

*****************************************************************************/
struct Task *Partition_Monitor(struct PartitionDeviceMap *pm, struct PartitionBase *PartitionBase)
{
    STRPTR task_name, dname;
    struct Task *mgr;
    int i;
    struct TagItem tags[] = {
/* 0 */ { TASKTAG_NAME,      (IPTR)0  },
/* 1 */ { TASKTAG_ARG1,      (IPTR)pm },
/* 2 */ { TASKTAG_ARG2,      (IPTR)0  },
/* 3 */ { TASKTAG_ARG3,      (IPTR)0  },
/* 4 */ { TASKTAG_ARG4,      (IPTR)0  },
        { TASKTAG_PC,        (IPTR)internalPartitionMonitor },
        { TASKTAG_STACKSIZE, AROS_STACKSIZE },
        { TAG_END },
    };
    BYTE sigok, sigerr;

    sigok = AllocSignal(-1);
    if (sigok == -1)
        return NULL;

    sigerr = AllocSignal(-1);
    if (sigerr == -1) {
        FreeSignal(sigok);
        return NULL;
    }

    /* Create a new task to manage the partitions for this device
     */
    task_name = AllocVec(strlen(pm->pm_DosName) + sizeof(task_prefix), MEMF_PUBLIC);
    if (task_name == NULL) {
        FreeSignal(sigok);
        FreeSignal(sigerr);
        return NULL;
    }

    for (i = 0; task_prefix[i] != 0; i++)
        task_name[i] = task_prefix[i];
    dname = &task_name[i];
    for (i = 0; pm->pm_DosName[i] != 0; i++)
        dname[i] = pm->pm_DosName[i];
    dname[i] = 0;

    tags[0].ti_Data = (IPTR)task_name;
    tags[2].ti_Data = (IPTR)FindTask(NULL);
    tags[3].ti_Data = (IPTR)sigok;
    tags[4].ti_Data = (IPTR)sigerr;
    
    mgr = NewCreateTaskA(tags);

    if (mgr != NULL) {
        ULONG sigmask;

        sigmask = Wait((1 << sigok) | (1 << sigerr));
        if (sigmask & sigerr) {
            D(bug("%s: %s failed to run\n", __func__, task_name));
            mgr = NULL;
        } else {
            D(bug("%s: %s has started\n", __func__, task_name));
        }
    }

    FreeVec(task_name);
    FreeSignal(sigok);
    FreeSignal(sigerr);

    return mgr;
}

static AROS_UFH3(VOID, doChangeInt,
        AROS_UFHA(struct Interrupt *, ipm, A1),
        AROS_UFHA(APTR, is_Code, A5),
        AROS_UFHA(struct ExecBase *, SysBase, A6))
{
    AROS_USERFUNC_INIT

    struct Task *task = (APTR)ipm->is_Node.ln_Name;
    BYTE sigbit = ipm->is_Node.ln_Pri;

    D(bug("%s: Signal %s (%d)\n", __func__, task->tc_Node.ln_Name, sigbit));
    Signal(task, (1 << sigbit));

    AROS_USERFUNC_EXIT
}

/* Partition monitor task 
 *
 * NOTE: Error handling is a little special, since we don't
 *       want to die until someone explicitly kills us.
 */
static void internalPartitionMonitor(IPTR ipm, IPTR iparent, IPTR iparentok, IPTR iparenterr)
{
    struct Task *parent = (APTR)iparent;
    BYTE parentok = (BYTE)iparentok;
    BYTE parenterr = (BYTE)iparenterr;
    struct PartitionDeviceMap *pm = (APTR)ipm;
    struct Device *device = pm->pm_Device;
    IPTR unit = pm->pm_Unit;
    CONST_STRPTR dosname = pm->pm_DosName;
    struct IOExtTD ioreq;
    struct Interrupt changeint;
    APTR PartitionBase;
    BYTE sigbit;

    PartitionBase = OpenLibrary("partition.library", 0);
    if (PartitionBase == NULL) {
        D(bug("%s: Impossible! No partition.library?\n", __func__));
        Signal(parent, 1 << parenterr);
        return;
    }

    sigbit = AllocSignal(-1);
    if (sigbit == -1) {
        CloseLibrary(PartitionBase);
        D(bug("%s: Impossible! No signal allocated!\n", __func__));
        Signal(parent, 1 << parenterr);
        return;
    }

    changeint.is_Node.ln_Pri  = sigbit;
    changeint.is_Node.ln_Name = (APTR)FindTask(NULL);
    changeint.is_Code = doChangeInt;
    changeint.is_Data = &changeint;

    if (OpenDevice(device->dd_Library.lib_Node.ln_Name, unit, (struct IORequest *)&ioreq, 0) == 0) {
        ioreq.iotd_Req.io_Command = TD_ADDCHANGEINT;
        ioreq.iotd_Req.io_Flags = 0;
        ioreq.iotd_Req.io_Length = sizeof(struct Interrupt);
        ioreq.iotd_Req.io_Data = &changeint;
        ioreq.iotd_Req.io_Error = 0;
        SendIO((struct IORequest *)&ioreq);
        if (ioreq.iotd_Req.io_Error == 0) {
            ULONG sigs;

            D(bug("%s: Partition Monitor for %s on %s.%d started\n",
                        __func__, dosname,
                        device->dd_Library.lib_Node.ln_Name, unit));

            /* Initial update */
            UpdateDosPartitions(device, unit, dosname);

            /* Ok, we're running now. */
            Signal(parent, 1 << parentok);

            /* Handle any eject/inject notifications */
            do {
                D(bug("%s: Waiting for media changes on %s.%d\n", __func__, device->dd_Library.lib_Node.ln_Name, (int)unit));
                sigs = Wait(SIGBREAKF_CTRL_C | (1 << sigbit));

                if (sigs & (1 << sigbit)) {
                    D(bug("%s: Detected for media change on %s.%d\n", __func__, device->dd_Library.lib_Node.ln_Name, (int)unit));
                    UpdateDosPartitions(device, unit, dosname);
                }
            } while (!(sigs & SIGBREAKF_CTRL_C));

            /* Remove the handler */
            ioreq.iotd_Req.io_Command = TD_REMCHANGEINT;
            SendIO((struct IORequest *)&ioreq);

            /* Eject any DOS partitions we manage */
            D(bug("%s: Ejecting %s.%d\n", __func__, device->dd_Library.lib_Node.ln_Name, (int)unit));
            EjectDosPartitions(device, unit);
        } else {
            D(bug("%s: Can't attach the change interrupt\n"));
            /* Signal death */
            Signal(parent, 1 << parenterr);
        }

        /* Close the device */
        CloseDevice((struct IORequest *)&ioreq);
    } else {
        D(bug("%s: Can't open device %s.%d\n", __func__, device->dd_Library.lib_Node.ln_Name, (int)unit));
        /* Signal death */
        Signal(parent, 1 << parenterr);
    }

    CloseLibrary(PartitionBase);

    D(bug("%s: Halted for %s.%d\n", __func__, device->dd_Library.lib_Node.ln_Name, (int)unit));
}
