#ifndef PARTITION_INTERN_H
#define PARTITION_INTERN_H

/*
    Copyright © 2001-2011, The AROS Development Team. All rights reserved.
   $Id$

   Desc: Internal definitions for partition.library
   Lang: english
*/

#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <libraries/partition.h>

#include <aros/libcall.h>
#include LC_LIBDEFS_FILE

struct PartitionBase_intern
{
    struct PartitionBase partbase;
    BPTR segList;
    struct List bootList;
    struct Library *pb_DOSBase;

    /* REMOVE ONCE ABIv1 HAS STABALIZED */
    struct Library *pb_UtilityBase;
    struct ExpansionBase *pb_ExpansionBase;

    /* DOS device handling service */

    /* Maps for inserted and 'dangling' DOS Devices */
    struct SignalSemaphore pb_DeviceMapSemaphore;
    struct List  pb_DeviceMaps; /* List of PartitionDeviceMap entries */

};

/* This is a map of running/ejected device nodes */
struct PartitionDeviceMap {
    struct MinNode      pm_Node;

    struct Device      *pm_Device;      /* Exec Device */
    IPTR                pm_Unit;        /* Unit of the Exec Device */
    STRPTR              pm_DosName;     /* Name of the DOS Device */

    ULONG               pm_State;       /* Current state of the map */
    struct Task        *pm_Monitor;     /* (optional) monitor */

    /* Array of DeviceNode pointers */
    ULONG               pm_DeviceNodes;
    struct DeviceNode **pm_DeviceNode;
};

/* PartitionDeviceMap states */
#define PDMS_EMPTY      0       /* Empty map */
#define PDMS_ACTIVE     1       /* Active map */
#define PDMS_DANGLING   2       /* Ejected map that has active DOS Device handlers */

#define PTYPE(x) ((struct PartitionType *)x)

/* We do NOT define DOSBase, because we want to be
 * explicit about all uses of DOSBase, since it may
 * be NULL.
 */

/* REMOVE ONCE ABIv1 HAS STABALIZED */
#define UtilityBase (((struct PartitionBase_intern *)PartitionBase)->pb_UtilityBase)
#define ExpansionBase (((struct PartitionBase_intern *)PartitionBase)->pb_ExpansionBase)

#endif /* PARTITION_INTERN_H */
