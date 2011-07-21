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

/*****************************************************************************

    NAME */
   AROS_LH3(LONG, ManageDosPartitions,

/*  SYNOPSIS */
   AROS_LHA(struct Device *, device, A0),
   AROS_LHA(IPTR,            unit,   D0),
   AROS_LHA(CONST_STRPTR,    dosname, A1),

/*  LOCATION */
   struct PartitionBase *, PartitionBase, 26, Partition)

/*  FUNCTION
    Enable automatic partition management support for a Exec Device

    INPUTS
    device    - Exec block device
    unit      - unit of the block device
    dosdevice - base name for DOS volumes. Use NULL to unregister.

    RESULT
    0 on success, DOS error code on error.

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

    /* All this function does is add an empty partition map to
     * the partition list.
     */
    LONG ret = RETURN_OK;
    struct PartitionBase_intern *pbi = (struct PartitionBase_intern *)PartitionBase;
    struct PartitionDeviceMap *pm;

    D(bug("%s: register %s.%d %s\n", __func__, device->dd_Library.lib_Node.ln_Name, unit, dosname));

    ObtainSemaphore(&pbi->pb_DeviceMapSemaphore);
    ForeachNode(&pbi->pb_DeviceMaps, pm) {
        if (pm->pm_Device == device &&
            pm->pm_Unit == unit) {
            D(bug("%s:   Duplicate DOS object\n", __func__));
            ret = ERROR_OBJECT_EXISTS;
            break;
        }
    }

    if (dosname == NULL) {
        /* Attempt to unmanage the device/unit */
        D(bug("%s:   Unregister %s.%d %s\n", __func__, device->dd_Library.lib_Node.ln_Name, unit, dosname));
        if (ret == ERROR_OBJECT_EXISTS) {
            EjectDosPartitions(pm->pm_Device, pm->pm_Unit);
            if (pm->pm_State == PDMS_EMPTY) {
                REMOVE((struct Node *)pm);
                D(bug("%s:   Remove %s.%d %s\n", __func__, device->dd_Library.lib_Node.ln_Name, unit, dosname));
                FreeVec(pm);
                ret = RETURN_OK;
            } else {
                ret = ERROR_OBJECT_IN_USE;
            }
        } else {
            ret = RETURN_OK;
        }
    } else if (ret == RETURN_OK) {
        pm = AllocVec(sizeof(*pm) + strlen(dosname) + 1, MEMF_PUBLIC | MEMF_CLEAR);
        if (pm == NULL) {
            ret = ERROR_NO_FREE_STORE;
        } else {
            D(bug("%s:   Add empty %s.%d %s\n", __func__, device->dd_Library.lib_Node.ln_Name, unit, dosname));
            pm->pm_Device = device;
            pm->pm_Unit   = unit;
            pm->pm_State  = PDMS_EMPTY;
            pm->pm_DosName = (APTR)&pm[1];
            strcpy(pm->pm_DosName, dosname);

            AddTail(&pbi->pb_DeviceMaps, (struct Node *)pm);
        }
    }
    ReleaseSemaphore(&pbi->pb_DeviceMapSemaphore);

    return ret;

    AROS_LIBFUNC_EXIT
}
