/*
    Copyright © 2004-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#define DEBUG 0

#include <aros/debug.h>
#include <exec/types.h>
#include <exec/exec.h>
#include <exec/resident.h>
#include <utility/utility.h>
#include <oop/oop.h>

#include <proto/exec.h>
#include <devices/timer.h>
#include <devices/cd.h>

#include "ahci.h"
#include "ahci_intern.h"
#include "timer.h"

/*
    Prototypes of static functions from lowlevel.c. I do not want to make them
    non-static as I'd like to remove as much symbols from global table as possible.
    Besides some of this functions could conflict with old ide.device or any other
    device.
*/
static BYTE ahci_Read32(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE ahci_Read64(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
static BYTE ahci_Write32(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE ahci_Write64(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
static BYTE ahci_Eject(struct ahci_Unit *);
static BOOL ahci_WaitBusyTO(struct ahci_Unit *unit, UWORD tout, BOOL irq, UBYTE *stout);


#if DEBUG
static void dump(APTR mem, ULONG len)
{
    register int i, j = 0;

    DUMP_MORE(for (j=0; j<(len+15)>>4; ++j))
    {
        bug("[AHCI  ] %06lx: ", j<<4);

        for (i=0; i<len-(j<<4); i++)
        {
            bug("%02lx ", ((unsigned char*)mem)[(j<<4)|i]);
            if (i == 15)
                break;
        }

        for (i=0; i<len-(j<<4); i++)
        {
            unsigned char c = ((unsigned char*)mem)[(j<<4)|i];

            bug("%c", c >= 0x20 ? c<=0x7f ? c : '.' : '.');
            if (i == 15)
                break;
        }
        bug("\n");
    }
}
#endif

static void ahci_strcpy(const UBYTE *str1, UBYTE *str2, ULONG size)
{
    register int i = size;

    while (size--)
        str2[size ^ 1] = str1[size];

    while (i > 0 && str2[--i] <= ' ')
        str2[i] = '\0';
}

/*
 * a STUB function for commands not supported by this particular device
 */
static BYTE ahci_STUB(struct ahci_Unit *au)
{
    bug("[AHCI%02ld] CALLED STUB FUNCTION (GENERIC). THIS OPERATION IS NOT "
        "SUPPORTED BY DEVICE\n", au->au_UnitNum);
    return CDERR_NOCMD;
}

static BYTE ahci_STUB_IO32(struct ahci_Unit *au, ULONG blk, ULONG len,
    APTR buf, ULONG* act)
{
    bug("[AHCI%02ld] CALLED STUB FUNCTION (IO32). THIS OPERATION IS NOT "
        "SUPPORTED BY DEVICE\n", au->au_UnitNum);
    return CDERR_NOCMD;
}

static BYTE ahci_STUB_IO64(struct ahci_Unit *au, UQUAD blk, ULONG len,
    APTR buf, ULONG* act)
{
    bug("[AHCI%02ld] CALLED STUB FUNCTION -- IO ACCESS TO BLOCK %08lx:%08lx, LENGTH %08lx. THIS OPERATION IS NOT SUPPORTED BY DEVICE\n", au->au_UnitNum, (blk >> 32), (blk & 0xffffffff), len);
    return CDERR_NOCMD;
}

static BYTE ahci_STUB_SCSI(struct ahci_Unit *au, struct SCSICmd* cmd)
{
    bug("[AHCI%02ld] CALLED STUB FUNCTION. THIS OPERATION IS NOT SUPPORTED BY DEVICE\n", au->au_UnitNum);
    return CDERR_NOCMD;
}

/*
 * handle IRQ; still fast and efficient, supposed to verify if this irq is for us and take adequate steps
 * part of code moved here from ata.c to reduce containment
 */
void ahci_IRQSignalTask(struct ahci_Port *ap)
{
    ap->ap_IntCnt++;
    Signal(ap->ap_Task, 1UL << ap->ap_SleepySignal);
}

void ahci_IRQSetHandler(struct ahci_Unit *unit, void (*handler)(struct ahci_Unit*, UBYTE), APTR piomem, ULONG blklen, ULONG piolen)
{
    unit->au_Port->ap_HandleIRQ = handler;
}

void ahci_IRQNoData(struct ahci_Unit *unit, UBYTE status)
{
    D(bug("[AHCI%02ld] IRQ: NoData - done; status %02lx.\n", unit->au_UnitNum, status));
    ahci_IRQSetHandler(unit, NULL, NULL, 0, 0);
    ahci_IRQSignalTask(unit->au_Port);
}

void ahci_IRQPIORead(struct ahci_Unit *unit, UBYTE status)
{
	D(bug("[AHCI  ] IRQ: PIOReadData - DRQ.\n"));
	D(bug("[AHCI  ] IRQ: PIOReadData - transfer completed.\n"));
    ahci_IRQNoData(unit, status);
}

void ahci_IRQReadWrite(struct ahci_Unit *unit, UBYTE status)
{
    D(bug("[AHCI%02ld] IRQ: IO status %02lx, DMA status %02lx\n", unit->au_UnitNum, status, stat));
    D(bug("[AHCI  ] IRQ: DMA Done.\n"));
    ahci_IRQNoData(unit, status);
}

/*
 * wait for timeout or drive ready
 */
BOOL ahci_WaitBusyTO(struct ahci_Unit *unit, UWORD tout, BOOL irq, UBYTE *stout)
{
    UBYTE status;
    ULONG sigs = SIGBREAKF_CTRL_C;
    ULONG step = 0;
    BOOL res = TRUE;

    /*
     * set up bus timeout
     */
    Disable();
    unit->au_Port->ap_Timeout = tout;
    Enable();

    sigs |= (irq ? (1 << unit->au_Port->ap_SleepySignal) : 0);
    status = 0; /* TODO: Get status */

    /*
     * wait for either IRQ or TIMEOUT (unless device seems to be a
     * phantom SAHCIPI drive, in which case we fake a timeout)
     */
    D(bug("[AHCI%02ld] Waiting (Current status: %02lx)...\n",
        unit->au_UnitNum, status));
    if (status != 0)
        step = Wait(sigs);
    else
        step = SIGBREAKF_CTRL_C;

    /*
     * now if we did reach timeout, then there's no point in going ahead.
     */
    if (SIGBREAKF_CTRL_C & step)
    {
        bug("[AHCI%02ld] Timeout while waiting for device to complete"
            " operation\n", unit->au_UnitNum);
        res = FALSE;

        /*
         * do nothing if the interrupt eventually arrives
         */
        Disable();
        ahci_IRQSetHandler(unit, NULL, NULL, 0, 0);
        Enable();
    }

    /*
     * clear up all our expectations
     */
    Disable();
    unit->au_Port->ap_Timeout = -1;
    Enable();

    /*
     * get final status and clear any interrupt (may be neccessary if we
     * were polling, for example)
     */
    status = 0; /* TODO: Get status */

    /*
     * be nice to frustrated developer
     */
    D(bug("[AHCI%02ld] WaitBusy status: %lx / %ld\n", unit->au_UnitNum,
        status, res));

    /*
     * release old junk
     */
    SetSignal(0, sigs);

    /*
     * and say it went fine (i mean it)
     */
    if (stout)
        *stout = status;
    return res;
}

/*
 * Initial device configuration that suits *all* cases
 */
BOOL ahci_init_unit(struct ahci_Port *ap, ULONG unitnum)
{
    struct ahci_Unit *unit = &ap->ap_Unit;

    D(bug("[AHCI  ] ahci_init_unit(%ld)\n", u));

    unit = &ap->ap_Unit;
    if (NULL == unit)
        return FALSE;

    unit->au_Port       = ap;
    unit->au_Drive      = AllocPooled(ap->ap_Base->ahci_MemPool, sizeof(struct DriveIdent));
    unit->au_UnitNum    = unitnum;

    unit->au_SectorShift= 9;    /* this really has to be set here. */

    NEWLIST(&unit->au_SoftList);

    /*
     * since the stack is always handled by caller
     * it's safe to stub all calls with one function
     */
    unit->au_Read32                 = ahci_STUB_IO32;
    unit->au_Read64                 = ahci_STUB_IO64;
    unit->au_Write32                = ahci_STUB_IO32;
    unit->au_Write64                = ahci_STUB_IO64;
    unit->au_Eject                  = ahci_STUB;
    unit->au_DirectSCSI             = ahci_STUB_SCSI;
    unit->au_Identify               = ahci_STUB;
    return TRUE;
}

BOOL ahci_setup_unit(struct ahci_Port *ap)
{
    struct ahci_Unit *unit=NULL;

    /*
     * this stuff always goes along the same way
     * WARNING: NO INTERRUPTS AT THIS POINT!
     */
    D(bug("[AHCI  ] ahci_setup_unit(%ld,%ld)\n", ap->ap_PortNum, u));

    unit = &ap->ap_Unit;
    if (NULL == unit)
        return FALSE;

    ahci_SelectUnit(unit);

    if (FALSE == ahci_WaitBusyTO(unit, 1, FALSE, NULL))
    {
        D(bug("[AHCI%02ld] ahci_setup_unit: ERROR: Drive not ready for use. Keeping functions stubbed\n", unit->au_UnitNum));
        FreePooled(ap->ap_Base->ahci_MemPool, unit->au_Drive, sizeof(struct DriveIdent));
        unit->au_Drive = 0;
        return FALSE;
    }

    switch (ap->ap_Dev)
    {
        /*
         * safe fallback settings
         */
        case DEV_SATAPI:
        case DEV_ATAPI:
        case DEV_SATA:
        case DEV_ATA:
            unit->au_Identify = ahci_Identify;
            break;

        default:
            D(bug("[AHCI%02ld] ahci_setup_unit: Unsupported device %lx. All functions will remain stubbed.\n", unit->au_UnitNum, ap->ap_Dev[u]));
            FreePooled(ap->ap_Base->ahci_MemPool, unit->au_Drive, sizeof(struct DriveIdent));
            unit->au_Drive = 0;
            return FALSE;
    }

    D(bug("[AHCI  ] ahci_setup_unit: Enabling IRQs\n"));
    /* TODO: Unmask the IRQs for this port */

    /*
     * now make unit self diagnose
     */
    if (unit->au_Identify(unit) != 0)
    {
        FreePooled(ap->ap_Base->ahci_MemPool, unit->au_Drive, sizeof(struct DriveIdent));
        unit->au_Drive = NULL;
        return FALSE;
    }

    return TRUE;
}

#define SWAP_LE_WORD(x) (x) = AROS_LE2WORD((x))
#define SWAP_LE_LONG(x) (x) = AROS_LE2LONG((x))
#define SWAP_LE_QUAD(x) (x) = AROS_LE2LONG((x) >> 32) | (((QUAD)(AROS_LE2LONG((x) & 0xffffffff))) << 32)

BYTE ahci_Identify(struct ahci_Unit* unit)
{
    BOOL atapi = unit->au_Port->ap_Dev & 0x80;
    ahci_CommandBlock acb = { };

    /* If the right command fails, try the wrong one. If both fail, abort */
    D(bug("[AHCI%02ld] ahci_Identify: Executing AHCI_IDENTIFY_%s command\n",
        unit->au_UnitNum, atapi ? "ATAPI" : "DEVICE"));
    if (ahci_exec_cmd(unit, &acb))
    {
        acb.command = atapi ? AHCI_IDENTIFY_DEVICE : AHCI_IDENTIFY_AHCIPI;
        D(bug("[AHCI%02ld] ahci_Identify: Executing AHCI_IDENTIFY_%s command"
            " instead\n", unit->au_UnitNum, atapi ? "DEVICE" : "AHCIPI"));
        if (ahci_exec_cmd(unit, &acb))
        {
            D(bug("[AHCI%02ld] ahci_Identify: Both command variants failed\n",
                unit->au_UnitNum));
            return IOERR_OPENFAIL;
        }
        unit->au_Port->ap_Dev ^= 0x82;
        atapi = unit->au_Port->ap_Dev & 0x80;
        D(bug("[AHCI%02ld] ahci_Identify:"
            " Incorrect device signature detected."
            unit->au_UnitNum));
    }

#if (AROS_BIG_ENDIAN != 0)
    SWAP_LE_WORD(unit->au_Drive->id_General);
    SWAP_LE_WORD(unit->au_Drive->id_OldCylinders);
    SWAP_LE_WORD(unit->au_Drive->id_SpecificConfig);
    SWAP_LE_WORD(unit->au_Drive->id_OldHeads);
    SWAP_LE_WORD(unit->au_Drive->id_OldSectors);
    SWAP_LE_WORD(unit->au_Drive->id_RWMultipleSize);
    SWAP_LE_WORD(unit->au_Drive->id_Capabilities);
    SWAP_LE_WORD(unit->au_Drive->id_OldCaps);
    SWAP_LE_WORD(unit->au_Drive->id_OldPIO);
    SWAP_LE_WORD(unit->au_Drive->id_ConfigAvailable);
    SWAP_LE_WORD(unit->au_Drive->id_OldLCylinders);
    SWAP_LE_WORD(unit->au_Drive->id_OldLHeads);
    SWAP_LE_WORD(unit->au_Drive->id_OldLSectors);
    SWAP_LE_WORD(unit->au_Drive->id_RWMultipleTrans);
    SWAP_LE_WORD(unit->au_Drive->id_MWDMASupport);
    SWAP_LE_WORD(unit->au_Drive->id_PIOSupport);
    SWAP_LE_WORD(unit->au_Drive->id_MWDMA_MinCycleTime);
    SWAP_LE_WORD(unit->au_Drive->id_MWDMA_DefCycleTime);
    SWAP_LE_WORD(unit->au_Drive->id_PIO_MinCycleTime);
    SWAP_LE_WORD(unit->au_Drive->id_PIO_MinCycleTImeIORDY);
    SWAP_LE_WORD(unit->au_Drive->id_QueueDepth);
    SWAP_LE_WORD(unit->au_Drive->id_AHCIVersion);
    SWAP_LE_WORD(unit->au_Drive->id_AHCIRevision);
    SWAP_LE_WORD(unit->au_Drive->id_Commands1);
    SWAP_LE_WORD(unit->au_Drive->id_Commands2);
    SWAP_LE_WORD(unit->au_Drive->id_Commands3);
    SWAP_LE_WORD(unit->au_Drive->id_Commands4);
    SWAP_LE_WORD(unit->au_Drive->id_Commands5);
    SWAP_LE_WORD(unit->au_Drive->id_Commands6);
    SWAP_LE_WORD(unit->au_Drive->id_UDMASupport);
    SWAP_LE_WORD(unit->au_Drive->id_SecurityEraseTime);
    SWAP_LE_WORD(unit->au_Drive->id_ESecurityEraseTime);
    SWAP_LE_WORD(unit->au_Drive->id_CurrentAdvPowerMode);
    SWAP_LE_WORD(unit->au_Drive->id_MasterPwdRevision);
    SWAP_LE_WORD(unit->au_Drive->id_HWResetResult);
    SWAP_LE_WORD(unit->au_Drive->id_AcousticManagement);
    SWAP_LE_WORD(unit->au_Drive->id_StreamMinimunReqSize);
    SWAP_LE_WORD(unit->au_Drive->id_StreamingTimeDMA);
    SWAP_LE_WORD(unit->au_Drive->id_StreamingLatency);
    SWAP_LE_WORD(unit->au_Drive->id_StreamingTimePIO);
    SWAP_LE_WORD(unit->au_Drive->id_PhysSectorSize);
    SWAP_LE_WORD(unit->au_Drive->id_RemMediaStatusNotificationFeatures);
    SWAP_LE_WORD(unit->au_Drive->id_SecurityStatus);

    SWAP_LE_LONG(unit->au_Drive->id_WordsPerLogicalSector);
    SWAP_LE_LONG(unit->au_Drive->id_LBASectors);
    SWAP_LE_LONG(unit->au_Drive->id_StreamingGranularity);

    SWAP_LE_QUAD(unit->au_Drive->id_LBA48Sectors);
#endif

    DUMP(dump(unit->au_Drive, sizeof(struct DriveIdent)));

    if (atapi)
    {
        unit->au_SectorShift    = 11;
        unit->au_Read32         = atapi_Read;
        unit->au_Write32        = atapi_Write;
        unit->au_DirectSCSI     = atapi_DirectSCSI;
        unit->au_Eject          = atapi_Eject;
        unit->au_Flags          |= AF_DiscChanged;
        unit->au_DevType        = (unit->au_Drive->id_General >>8) & 0x1f;
        unit->au_XferModes      = AF_XFER_PACKET;
    }
    else
    {
        unit->au_SectorShift    = 9;
        unit->au_DevType        = DG_DIRECT_ACCESS;
        unit->au_Read32         = ahci_ReadSector32;
        unit->au_Write32        = ahci_WriteSector32;
        unit->au_Eject          = ahci_Eject;
        unit->au_XferModes      = 0;
        unit->au_Flags         |= AF_DiscPresent | AF_DiscChanged;
    }

    ahci_strcpy(unit->au_Drive->id_Model, unit->au_Model, 40);
    ahci_strcpy(unit->au_Drive->id_SerialNumber, unit->au_SerialNumber, 20);
    ahci_strcpy(unit->au_Drive->id_FirmwareRev, unit->au_FirmwareRev, 8);

    bug("[AHCI%02ld] ahci_Identify: Unit info: %s / %s / %s\n", unit->au_UnitNum, unit->au_Model, unit->au_SerialNumber, unit->au_FirmwareRev);
    common_DetectXferModes(unit);
    common_SetBestXferMode(unit);

    if (unit->au_Drive->id_General & 0x80)
    {
        D(bug("[AHCI%02ld] ahci_Identify: Device is removable.\n", unit->au_UnitNum));
        unit->au_Flags |= AF_Removable;
    }

    unit->au_Capacity   = unit->au_Drive->id_LBASectors;
    unit->au_Capacity48 = unit->au_Drive->id_LBA48Sectors;
    bug("[AHCI%02ld] ahci_Identify: Unit info: %07lx 28bit / %04lx:%08lx 48bit addressable blocks\n", unit->au_UnitNum, unit->au_Capacity, (ULONG)(unit->au_Capacity48 >> 32), (ULONG)(unit->au_Capacity48 & 0xfffffffful));

    if (atapi)
    {
        /*
         * ok, this is not very original, but quite compatible :P
         */
        switch (unit->au_DevType)
        {
            case DG_CDROM:
            case DG_WORM:
            case DG_OPTICAL_DISK:
                unit->au_SectorShift    = 11;
                unit->au_Heads          = 1;
                unit->au_Sectors        = 75;
                unit->au_Cylinders      = 4440;
                break;

            case DG_DIRECT_ACCESS:
                unit->au_SectorShift = 9;
                if (!strcmp("LS-120", &unit->au_Model[0]))
                {
                    unit->au_Heads      = 2;
                    unit->au_Sectors    = 18;
                    unit->au_Cylinders  = 6848;
                }
                else if (!strcmp("ZIP 100 ", &unit->au_Model[8]))
                {
                    unit->au_Heads      = 1;
                    unit->au_Sectors    = 64;
                    unit->au_Cylinders  = 3072;
                }
                break;
        }

        atapi_TestUnitOK(unit);
    }
    else
    {
        /*
           For drive capacities > 8.3GB assume maximal possible layout.
           It really doesn't matter here, as BIOS will not handle them in
           CHS way anyway :)
           i guess this just solves that weirdo div-by-zero crash, if nothing
           else...
           */
        if ((unit->au_Drive->id_LBA48Sectors > (63 * 255 * 1024)) ||
            (unit->au_Drive->id_LBASectors > (63 * 255 * 1024)))
        {
            ULONG div = 1;
            /*
             * TODO: this shouldn't be casted down here.
             */
            ULONG sec = unit->au_Capacity48;

            if (sec < unit->au_Capacity48)
                sec = ~((ULONG)0);

            if (sec < unit->au_Capacity)
                sec = unit->au_Capacity;

            unit->au_Sectors = 63;
            sec /= 63;
            /*
             * keep dividing by 2
             */
            do
            {
                if (((sec >> 1) << 1) != sec)
                    break;
                if ((div << 1) > 255)
                    break;
                div <<= 1;
                sec >>= 1;
            } while (1);

            do
            {
                if (((sec / 3) * 3) != sec)
                    break;
                if ((div * 3) > 255)
                    break;
                div *= 3;
                sec /= 3;
            } while (1);

            unit->au_Cylinders  = sec;
            unit->au_Heads      = div;
        }
        else
        {
            unit->au_Cylinders  = unit->au_Drive->id_OldLCylinders;
            unit->au_Heads      = unit->au_Drive->id_OldLHeads;
            unit->au_Sectors    = unit->au_Drive->id_OldLSectors;
        }
    }

    return 0;
}

/*
 * ahci read32 commands
 */
static BYTE ahci_ReadSector32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    ahci_CommandBlock acb = { };
    BYTE err;

    D(bug("[AHCI%02ld] ahci_ReadSector32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_ReadMultiple32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    ahci_CommandBlock acb = { };
    BYTE err;

    D(bug("[AHCI%02ld] ahci_ReadMultiple32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_Read32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    BYTE err;
    ahci_CommandBlock acb = { };

    D(bug("[AHCI%02ld] ahci_Read32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_Read64(struct ahci_Unit *unit, UQUAD block,
    ULONG count, APTR buffer, ULONG *act)
{
    ahci_CommandBlock acb = {};
    BYTE err;

    D(bug("[AHCI%02ld] ahci_Read64()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

/*
 * ahci write32 commands
 */
static BYTE ahci_Write32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    ahci_CommandBlock acb = {};
    BYTE err;

    D(bug("[AHCI%02ld] ahci_Write32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

/*
 * ahci write64 commands
 */
static BYTE ahci_Write64(struct ahci_Unit *unit, UQUAD block,
    ULONG count, APTR buffer, ULONG *act)
{
    ahci_CommandBlock acb = { };
    BYTE err;

    D(bug("[AHCI%02ld] ahci_Write64()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

/*
 * ahci miscellaneous commands
 */
static BYTE ahci_Eject(struct ahci_Unit *unit)
{
    ahci_CommandBlock acb;

    D(bug("[AHCI%02ld] ahci_Eject()\n", unit->au_UnitNum));

    return ahci_exec_cmd(unit, &acb);
}

ULONG ahci_ReadSignature(struct ahci_Port *bus, int unit)
{
    D(bug("[AHCI  ] ahci_ReadSignature(%02ld)\n", unit));

    return DEV_NONE;
}

void ahci_ResetPort(struct ahci_Port *ap)
{
    /* Set and then reset the soft reset bit in the Device Control
     * register.  This causes device 0 be selected */
    D(bug("[AHCI  ] ahci_ResetPort(%d)\n", ap->ap_PortNum));
}

void ahci_InitPort(struct ahci_Port *ap)
{
    /*
     * initialize timer for the sake of scanning
     */
    ap->ap_Timer = ahci_OpenTimer();

    D(bug("[AHCI  ] ahci_InitPort(%d)\n", ap->ap_PortNum));

    ap->ap_Dev = DEV_NONE;

    ahci_ResetPort(ap);
    ahci_CloseTimer(ap->ap_Timer);
    D(bug("[AHCI  ] ahci_InitPort: Finished\n"));
}

/*
 * vim: ts=4 et sw=4 fdm=marker fmr={,}
 */
