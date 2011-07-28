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

/* Internal routines, defined below */
#define PMF_BOOTPRI(x)          ((x) & 0xff)
#define PMF_BOOTPRI_of(x)       ((BYTE)((x) & 0xff))
#define PMF_AUTOMOUNT           (1 << 8)

#define DNAME(device) (device->dd_Library.lib_Node.ln_Name)

static void internal_InjectMap(struct PartitionDeviceMap *pm, struct PartitionBase *PartitionBase);
static struct DeviceNode *internal_MakeDeviceNode( struct PartitionHandle *ph, struct Device *device, IPTR unit, TEXT dosname[32], struct PartitionBase *PartitionBase);
static BOOL internal_FindDeviceNodes(struct PartitionHandle *table, struct Device *device, IPTR unit, TEXT dosname[32], struct DeviceNode **dnlist, struct PartitionBase *PartitionBase);
static BOOL internal_DeviceNodeCmp(const struct DeviceNode *dn_a, const struct DeviceNode *dn_b);
static BOOL internal_DeviceMapCmp(const struct PartitionDeviceMap *pm_a, const struct DeviceNode *dnlist);
static BOOL internal_IsPresent(struct Device *device, IPTR unit, ULONG flags);

/*****************************************************************************

    NAME */
   AROS_LH3(LONG, UpdateDosPartitions,

/*  SYNOPSIS */
   AROS_LHA(struct Device *, device, A0),
   AROS_LHA(IPTR,            unit,   D0),
   AROS_LHA(CONST_STRPTR,    dosdevice, A1),

/*  LOCATION */
   struct PartitionBase *, PartitionBase, 24, Partition)

/*  FUNCTION
    Update the Dos nodes for a trackdisk style device

    This function wraps the necessary functionality for a trackdisk
    device to handle hotplug support:

     * Creation and maintainance of DOS DeviceNodes for a trackdisk
     * Automatic dismounting of DeviceNodes when a disk change occurs
     * Automatic remounting of the same DeviceNodes when a disk 
       reappears on *any* trackdisk device.

    INPUTS
    dosdevice - *suggested* base name for DOS volumes
    device    - pointer to the Exec block device
    unit      - unit of the block device

    RESULT
    0 on success, ERROR_* code on failure.

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

    struct PartitionDeviceMap *pm, *pm_old;
    struct PartitionHandle *ph;
    struct DeviceNode *dnlist = NULL, *dn;
    int i, dncount;
    TEXT dosname[32 + 1];
    LONG ret = RETURN_OK;

    if (!internal_IsPresent(device, unit, 0))
        return EjectDosPartitions(device, unit);

    D(bug("%s: Exec %s(%d), Dos '%s'\n", __func__, DNAME(device), (int)unit, dosdevice));

    pm = AllocVec(sizeof(struct PartitionDeviceMap) + strlen(dosdevice) + 1, MEMF_PUBLIC | MEMF_CLEAR);
    if (!pm)
        return ERROR_NO_FREE_STORE;

    pm->pm_Unit = unit;
    pm->pm_Device = device;
    pm->pm_DosName = (APTR)&pm[1];
    strcpy(pm->pm_DosName, dosdevice);

    /* Truncate the incoming dos name to 32 characters */
    strncpy(dosname, dosdevice, sizeof(dosname));
    dosname[sizeof(dosname)-1] = 0;

    ph = OpenRootPartition(DNAME(device), unit);
    if (!ph) {
        /* Ok.. broken device. Eject any partitions that may be hanging around */
        D(bug("%s:   No device detected - ejecting\n", __func__));
        ret = EjectDosPartitions(device, unit);

        FreeVec(pm);

        return ret;
    }

    /* Create a new list of DOS DeviceNodes
     *  - but don't register it just yet!
     *
     * Also, we don't need to check for conflicts yet with existing DOS device names.
     */
    if (internal_FindDeviceNodes(ph, device, unit, dosname, &dnlist, PartitionBase)) {
        /* Count the number of entries in the list we got */
        for (dncount = 0, dn = dnlist; dn ; dncount++, dn = BADDR(dn->dn_Next));
        D(bug("%s:   Partitions detected - %d total\n", __func__, dncount));
    } else {
        /* No device nodes. Just create a whole-disk node */
        D(bug("%s:   No partitions detected - whole disk\n", __func__));
        dnlist = internal_MakeDeviceNode(ph, device, unit, dosname, PartitionBase);
        dncount = 1;
    }

    CloseRootPartition(ph);

    /* We'll need the PartitionBase's PartitionDeviceMap list locked for the
     * rest of this process.
     */
    D(bug("%s: Waiting for DeviceMap semaphore...\n", __func__));
    ObtainSemaphore(&((struct PartitionBase_intern *)PartitionBase)->pb_DeviceMapSemaphore);

    /* Do we have an active Device Map for this device? */
    D(bug("%s: Scanning for stale DeviceMap...\n", __func__));
    ForeachNode(&((struct PartitionBase_intern *)PartitionBase)->pb_DeviceMaps, pm_old) {
        BOOL did_match;

        D(bug("%s: Testing against %s.%d..\n", __func__, pm_old->pm_Device->dd_Library.lib_Node.ln_Name, (int)pm_old->pm_Unit));
        if (pm_old->pm_Device != device ||
            pm_old->pm_Unit != unit)
            continue;

        /* Ok. An existing map. Does it match what we just found? */
        did_match = internal_DeviceMapCmp(pm_old, dnlist);
        D(bug("%s:   Existing %smatching partition map registered\n", __func__, did_match ? "" : "non-"));

        /* If the pm is PDMS_EMPTY, use it.
         */
        if (pm_old->pm_State == PDMS_EMPTY) {
            FreeVec(pm);
            pm = pm_old;
            REMOVE((struct Node *)pm);
            break;
        }

        /* Luckily, these were ever only allocated privately, and
         * never put on any Expansion or DOS lists.
         */
        for (dn = dnlist; dn; ) {
            struct DeviceNode *tmp = BADDR(dn->dn_Next);

            /* Ah, if only we could call an Expansion/FreeDosNode function...
             */
            /* WARNING: THIS MUST AGREE WITH Expansion/MakeDosNode()'s
             *          ALLOCATION MECHANISM!
             */
            FreeVec(dn);

            /* Move to the next one */
            dn = tmp;
        }

        FreeVec(pm);
        pm = NULL;

        /* If we didn't match, the old one is still
         * waiting for it's medium to return.
         * Don't start any new handlers.
         */
        if (!did_match) {
            ret = ERROR_LOCK_COLLISION;
            break;
        }

        /* If we did have a match, this implies that the
         * old one is either still active, or dangling.
         */
        if (pm_old->pm_State == PDMS_ACTIVE) {
            /* If active, leave it active. */
            ret = RETURN_OK;
        } else {
            int i;
            APTR DOSBase;

            if ((DOSBase = OpenLibrary("dos.library", 0))) {

                D(bug("%s:   Restarting dangling IOs\n", __func__));

                /* Dangling. Restart them. */
                for (i = 0; i < pm_old->pm_DeviceNodes; i++) {
                    struct MsgPort *mp = pm_old->pm_DeviceNode[i]->dn_Task;

                    if (mp != NULL)
                        DoPkt(mp, ACTION_INHIBIT, DOSFALSE, 0, 0, 0, 0);
                }

                CloseLibrary(DOSBase);
            }
            ret = RETURN_OK;
        }

        break;
    }

    D(bug("%s: pm=%p\n", __func__, pm));
    if (pm != NULL) {
        /* New set of DeviceNodes. Store them away */
        pm->pm_DeviceNodes = dncount;
        pm->pm_DeviceNode = AllocMem(sizeof(struct DeviceNode *)*dncount, MEMF_PUBLIC);
        for (i = 0, dn = dnlist; dn ; dn = BADDR(dn->dn_Next), i++)
            pm->pm_DeviceNode[i] = dn;

        /* Ok, now we can safely add them to the system */
        D(bug("%s:   Injecting new DOS DeviceNodes...\n", __func__));
        internal_InjectMap(pm, PartitionBase);
    }

    D(bug("%s: Release the DeviceMap semaphore\n", __func__));
    ReleaseSemaphore(&((struct PartitionBase_intern *)PartitionBase)->pb_DeviceMapSemaphore);
    return ret;

    AROS_LIBFUNC_EXIT
}

static BOOL internal_IsPresent(struct Device *device, IPTR unit, ULONG flags)
{
    struct IOExtTD ioreq;
    BOOL ret = FALSE;

    if (OpenDevice(device->dd_Library.lib_Node.ln_Name,unit,(struct IORequest *)&ioreq, flags) == 0) {
        ioreq.iotd_Req.io_Actual = ~0;
        ioreq.iotd_Req.io_Command = TD_CHANGESTATE;
        DoIO((struct IORequest *)&ioreq);
        ret = (ioreq.iotd_Req.io_Actual == 0);
        CloseDevice((struct IORequest *)&ioreq);
    }

    return ret;
}


/* Online a partition map */
static void internal_InjectMap(struct PartitionDeviceMap *pm, struct PartitionBase *PartitionBase)
{
    int i;

    /* Add all the detected Dos Devices */
    for (i = 0; i < pm->pm_DeviceNodes; i++) {
        struct DeviceNode *dn = pm->pm_DeviceNode[i];
        struct FileSysStartupMsg *fssm = BADDR(dn->dn_Startup);
        BYTE  pri = -128;
        ULONG flags = 0;

        /* Add the DeviceNode to the Expansion Mountlist
         */
        pri = PMF_BOOTPRI_of(fssm->fssm_Flags);
        flags |= (fssm->fssm_Flags & PMF_AUTOMOUNT) ? ADNF_STARTPROC : 0;

        /* Clear fssm_Flags */
        fssm->fssm_Flags = 0;

        /* If DOS is available, AddBootNode() will take
         * care of starting the DOS Device.
         */
        D(bug("%s:     DOS DeviceNode: '%b', pri = %d%s\n", __func__, dn->dn_Name, pri, (flags & ADNF_STARTPROC) ? ", automount" : ""));
        AddBootNode(pri, flags, dn, NULL);
    }

    pm->pm_State = PDMS_ACTIVE;

    /* Add to the DeviceMap list.
     */
    AddTail(&((struct PartitionBase_intern *)PartitionBase)->pb_DeviceMaps, (struct Node *)pm);
}

static struct DeviceNode *internal_MakeDeviceNode(
        struct PartitionHandle *ph,
        struct Device *device,
        IPTR unit,
        TEXT dosname[32],
        struct PartitionBase *PartitionBase)
{
    IPTR pp[4 + DE_BOOTBLOCKS + 1];
    BOOL bootable, automount;
    ULONG position;
    TEXT name[32];
    struct TagItem tags[] = {
        { PT_AUTOMOUNT,(IPTR)&automount },
        { PT_BOOTABLE, (IPTR)&bootable },
        { PT_POSITION, (IPTR)&position },
        { PT_NAME,     (IPTR)&name[0] },
        { PT_DOSENVEC, (IPTR)&pp[4] },
        { TAG_END }};

    GetPartitionAttrs(ph, tags);

    if (name[0] == 0) {
        /* Ugh. We have to fake a name. 
         * The rules:
         *   - If dosname has no '-' in it, use it, then add a '-' before
         *     position 28 in the dosname.
         *   - If dosname has a '-' in it, replace it with -%u = PT_POSITION
         */
        char *cp = strrchr(dosname, '-');
        if (cp == NULL) {
            int len = strlen(dosname);
            CopyMem(dosname, name, len + 1);

            if (len >= 28)
                len = 27;

            dosname[len++] = '-';
            dosname[len++] = 0;
        } else {
            IPTR args[2] = { (IPTR)dosname, (IPTR)position };
            *cp = 0;
            RawDoFmt("%s-%u", args, RAWFMTFUNC_STRING, name);
            *cp = '.';
        }
    }

    pp[0] = (IPTR)name;
    pp[1] = (IPTR)device->dd_Library.lib_Node.ln_Name;
    pp[2] = (IPTR)unit;

    D(bug("%s: Partition %d of %s.%d: %s type=0x%08x\n", __func__, position, device->dd_Library.lib_Node.ln_Name, (int)unit, name, pp[4 + DE_DOSTYPE]));

    /* We abuse the flags a bit here, but since
     * partition.library won't fill them in, 
     * and we're going to clear them before 
     * starting the Dos node, that's ok.
     */
    pp[3] = PMF_BOOTPRI(bootable ? pp[4 + DE_BOOTPRI] : -128) |
            ((automount) ? PMF_AUTOMOUNT : 0);
   
    return MakeDosNode(pp);
} 
    
static BOOL internal_FindDeviceNodes(struct PartitionHandle *table, struct Device *device, IPTR unit, TEXT dosname[32], struct DeviceNode **dnlist, struct PartitionBase *PartitionBase)
{
    BOOL retval = FALSE;

    if (OpenPartitionTable(table) == 0) {
        struct PartitionHandle *ph;

        ForeachNode(&table->table->list, ph) {
            if (!internal_FindDeviceNodes(ph, device, unit, dosname, dnlist, PartitionBase)) {
                struct DeviceNode *dn;

                dn = internal_MakeDeviceNode(ph, device, unit, dosname, PartitionBase);
                if (dn != NULL) {
                    dn->dn_Next = MKBADDR(*dnlist);
                    *dnlist = BADDR(dn);
                }
            }
        }
        retval = TRUE;
        ClosePartitionTable(table);
    }

    return retval;
}

/* Are two DeviceNodes 'the same'?
 *
 *  - Same dn_Name
 *  - Same fssm->fssm_Environ contents
 */
static BOOL internal_DeviceNodeCmp(const struct DeviceNode *dn_a, const struct DeviceNode *dn_b)
{
    IPTR len_a;
    IPTR len_b;
    struct FileSysStartupMsg *fssm_a, *fssm_b;
    struct DosEnvec *de_a, *de_b;

    len_a = AROS_BSTR_strlen(dn_a->dn_Name);
    len_b = AROS_BSTR_strlen(dn_b->dn_Name);

    if (len_a != len_b)
        return FALSE;

    if (memcmp(AROS_BSTR_ADDR(dn_a->dn_Name),AROS_BSTR_ADDR(dn_b->dn_Name),len_a)!=0)
        return FALSE;

    fssm_a = BADDR(dn_a->dn_Startup);
    fssm_b = BADDR(dn_b->dn_Startup);

    de_a = BADDR(fssm_a->fssm_Environ);
    de_b = BADDR(fssm_b->fssm_Environ);
    if (de_a->de_TableSize != de_b->de_TableSize)
        return FALSE;

    return (memcmp(de_a, de_b, de_a->de_TableSize * sizeof(IPTR)) == 0) ? TRUE : FALSE;
}

/*
 * Find an existing ejected PartitionDeviceMap that matches the
 * partition we just parsed.
 */
static BOOL internal_DeviceMapCmp(const struct PartitionDeviceMap *pm, const struct DeviceNode *dnlist)
{
    int i;
    const struct DeviceNode *dn;

    for (i = 0; i < pm->pm_DeviceNodes && dn != NULL; i++, dn = BADDR(dn->dn_Next)) {
        if (internal_DeviceNodeCmp(pm->pm_DeviceNode[i], dn) != 0)
            break;
    }
    
    /* Did they all match? */
    return (i == pm->pm_DeviceNodes) ? TRUE : FALSE;
}
