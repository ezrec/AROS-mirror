/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Discover all mountable partitions
    Lang: english
*/

#include <string.h>
#include <stdlib.h>

#include <aros/debug.h>
#include <exec/alerts.h>
#include <aros/asmcall.h>
#include <aros/bootloader.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/resident.h>
#include <exec/types.h>
#include <libraries/configvars.h>
#include <libraries/expansion.h>
#include <libraries/expansionbase.h>
#include <libraries/partition.h>
#include <utility/tagitem.h>
#include <devices/bootblock.h>
#include <devices/timer.h>
#include <dos/dosextens.h>
#include <resources/filesysres.h>

#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/partition.h>
#include <proto/bootloader.h>

#include LC_LIBDEFS_FILE

#include "dosboot_intern.h"

/* Scan all partitions manually for additional
 * volumes that can be mounted.
 */
LONG dosboot_BootScan(LIBBASETYPEPTR LIBBASE)
{
    struct ExpansionBase *ExpansionBase;
    APTR PartitionBase;

    ExpansionBase = (APTR)OpenLibrary("expansion.library", 0);
    if (ExpansionBase == NULL)
        Alert( AT_DeadEnd | AG_OpenLib | AN_BootStrap | AO_ExpansionLib );

    /* If we have partition.library, we can look for partitions */
    PartitionBase = OpenLibrary("partition.library", 4);
    if (PartitionBase)
    {
        /* Start the partition monitors.
         * This will also populate the Expansion MountList
         * with all the discovered DOS nodes
         */
        D(bug("[Strap] Starting DOS device monitors\n"));
        MonitorDosPartitions(TRUE);
        CloseLibrary(PartitionBase);
    }

    CloseLibrary((APTR)ExpansionBase);

    return RETURN_OK;
}


