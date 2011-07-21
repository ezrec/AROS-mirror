/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

*/

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
#include "debug.h"

static LONG internal_EjectDeviceMap(struct PartitionDeviceMap *pm, struct PartitionBase *PartitionBase);

/*****************************************************************************

    NAME */
   AROS_LH2(LONG, EjectDosPartitions,

/*  SYNOPSIS */
   AROS_LHA(struct Device *, device, A0),
   AROS_LHA(IPTR,            unit,   D0),

/*  LOCATION */
   struct PartitionBase *, PartitionBase, 25, Partition)

/*  FUNCTION
    Eject the Dos nodes for a trackdisk style device

    This function valiantly attempts to dismount the
    DOS handlers for this device.

    Only those handlers that were detected by 
    Partition/UpdateDosPartitions will be affected.

    INPUTS
    device    - Exec block device
    unit      - unit of the block device

    RESULT
    RETURN_OK            - no errors, DOS Devices updated
    ERROR_UNLOCK_ERROR   - Handler(s) could not be stopped.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
    05-07-2011  first version

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct PartitionBase_intern *pbi = (struct PartitionBase_intern *)PartitionBase;
    struct PartitionDeviceMap *pm;
    LONG ret = RETURN_OK;

    ObtainSemaphore(&pbi->pb_DeviceMapSemaphore);

    /* Scan the pb_DeviceMaps list */
    ForeachNode(&pbi->pb_DeviceMaps, pm) {
        if ((pm->pm_Device == device) &&
            (pm->pm_Unit == unit)) {
            ret = internal_EjectDeviceMap(pm, PartitionBase);
            break;
        }
    }

    ReleaseSemaphore(&pbi->pb_DeviceMapSemaphore);

    return ret;

    AROS_LIBFUNC_EXIT
}

/* Offline the DOS DeviceNodes in the PartitionDeviceMap
 */
static LONG internal_EjectDeviceMap(struct PartitionDeviceMap *pm, struct PartitionBase *PartitionBase)
{
    APTR DOSBase;
    LONG ret = RETURN_OK;
    int i;

    /* Remove all the BootNodes from ExpansionBase->MountList
     */
    for (i = 0; i < pm->pm_DeviceNodes; i++) {
        struct BootNode *bn, *tmp;
        struct DeviceNode *dn = pm->pm_DeviceNode[i];

        /* Remove the DeviceNode from the Mountlist
         *
         * NOTE: Maybe a private Expansion/RemBootNode() function
         *       would be a better idea?
         */
        Forbid();
        ForeachNodeSafe(&ExpansionBase->MountList, bn, tmp) {
            if (bn->bn_DeviceNode == dn) {
                Remove((struct Node *)bn);
                /* WARNING: THIS MUST AGREE WITH Expansion/AddBootNode()'s
                 *          ALLOCATION MECHANISM!
                 */
                FreeMem(bn, sizeof(*bn));
            }
        }
        Permit();
    }

    /* DOS can fail to open, and that's ok, as we may
     * be handling ejects before DOS is started.
     */
    DOSBase = TaggedOpenLibrary(TAGGEDOPEN_DOS);
    if (!DOSBase)
        goto dispose;

    /* From now on, we're going to assume it's dangling */
    pm->pm_State = PDMS_DANGLING;

    /* We must be careful here, as the context of this
     * routine is usually a task. We cannot call
     * any DOS routines that attempt to modify
     * any pr_* fields.
     */

    /* Try to ACTION_INHIBIT all the handlers
     */
    for (i = 0; i < pm->pm_DeviceNodes; i++) {
        struct DeviceNode *dn = pm->pm_DeviceNode[i];
        SIPTR err;

        /* No task? Then the device hasn't started */
        if (dn->dn_Task == NULL)
            continue;

        err = DoPkt(dn->dn_Task, ACTION_INHIBIT, DOSTRUE, 0, 0, 0, 0);
        if (err != RETURN_OK) {
            D(bug("%s: Handler for %b will not inhibit\n", __func__, dn->dn_Name));
            ret = (LONG)err;
        }
    }

    /* Inhibition failed. It's a dangling map. */
    if (ret != RETURN_OK)
        goto exit;

    /* Ok, all handlers are INHIBITed. Kill them. */
    for (i = 0; i < pm->pm_DeviceNodes; i++) {
        struct DeviceNode *dn = pm->pm_DeviceNode[i];
        SIPTR err;

        /* No task? Then the device hasn't started */
        if (dn->dn_Task == NULL)
            continue;

        err = DoPkt(dn->dn_Task, ACTION_DIE, 0, 0, 0, 0, 0);
        if (err == RETURN_OK) {
            dn->dn_Task = NULL;
            D(bug("%s: Handler for %b has died\n", __func__, dn->dn_Name));
        } else {
            D(bug("%s: Handler for %b will not die\n", __func__, dn->dn_Name));
            ret = (LONG)err;
        }
    }

    /* Death failed. It's a dangling map. */
    if (ret != RETURN_OK)
        goto exit;

dispose:
    /* We can safely remove it from the list at this point */
    pm->pm_State = PDMS_EMPTY;

    /* Ok, everything is stopped. Clean it up! */
    for (i = 0; i < pm->pm_DeviceNodes; i++) {
        struct DeviceNode *dn = pm->pm_DeviceNode[i];

        /* Remove the DeviceNode from the DOS list */
        RemDosEntry((struct DosList *)dn);

        /* WARNING! This must match the allocation in
         * Expansion/MakeDosNode
         */
        FreeVec(dn);
    }

    /* This must match the allocations in UpdateDosPartitions */
    FreeMem(pm->pm_DeviceNode, sizeof(struct DeviceNode *)*pm->pm_DeviceNodes);
    pm->pm_DeviceNode = NULL;
    pm->pm_DeviceNodes = 0;

exit:
    CloseLibrary(DOSBase);
    return ret;
}
