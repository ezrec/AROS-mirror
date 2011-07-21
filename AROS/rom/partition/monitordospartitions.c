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
#include "partition_monitor.h"
#include "partition_support.h"
#include "platform.h"
#include "debug.h"

/*****************************************************************************

    NAME */
   AROS_LH1(LONG, MonitorDosPartitions,

/*  SYNOPSIS */
   AROS_LHA(BOOL,    is_enabled,   D0),

/*  LOCATION */
   struct PartitionBase *, PartitionBase, 27, Partition)

/*  FUNCTION
    Monitor all the managed block devices

    INPUTS
    is_enabled- Enable (TRUE) or disable (FALSE) all monitors

    RESULT
    RETURN_OK            - no errors, monitors started

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
    struct PartitionDeviceMap *pm, *tmp;
    LONG ret = RETURN_OK;

    /* Enable/Disable the monitors for all the devices */
    ForeachNodeSafe(&pbi->pb_DeviceMaps, pm, tmp) {
        if (is_enabled && pm->pm_Monitor == NULL) {
            D(bug("%s: Monitoring pm %p\n", __func__, pm));
            pm->pm_Monitor = Partition_Monitor(pm, PartitionBase);
            if (pm->pm_Monitor == NULL)
                ret = ERROR_NO_FREE_STORE;
        } else if (!is_enabled && pm->pm_Monitor != NULL) {
            D(bug("%s: Unmonitoring pm %p\n", __func__, pm));
            Signal(pm->pm_Monitor, SIGBREAKF_CTRL_C);
            pm->pm_Monitor = NULL;
        }
    }

    return ret;

    AROS_LIBFUNC_EXIT
}
