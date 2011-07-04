/*
    Copyright © 2004-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

/*
 * PARTIAL CHANGELOG:
 * DATE        NAME                ENTRY
 * ----------  ------------------  -------------------------------------------------------------------
 * 2006-12-20  T. Wiszkowski       Updated ATA Packet Interface to handle ATAPI/SCSI Commands
 * 2008-01-06  T. Wiszkowski       Corrected and completed ATA Packet Interface handling. PIO transfers fully operational.
 * 2008-01-07  T. Wiszkowski       Added initial DMA support for Direct SCSI commands. Corrected atapi
 *                                 READ and WRITE commands to pass proper transfer size to the atapi_SendPacket
 *                                 as discovered by mschulz
 * 2008-01-25  T. Wiszkowski       Rebuilt, rearranged and partially fixed 60% of the code here
 *                                 Enabled implementation to scan for other PCI IDE controllers
 *                                 Implemented ATAPI Packet Support for both read and write
 *                                 Corrected ATAPI DMA handling
 *                                 Fixed major IDE enumeration bugs severely handicapping transfers with more than one controller
 *                                 Compacted source and implemented major ATA support procedure
 *                                 Improved DMA and Interrupt management
 *                                 Removed obsolete code
 * 2008-01-26  T. Wiszkowski       Restored 32bit io
 *                                 Removed memory dump upon RequestSense
 * 2008-02-08  T. Wiszkowski       Fixed DMA accesses for direct scsi devices,
 *                                 Corrected IO Areas to allow ATA to talk to PCI controllers
 * 2008-03-03  T. Wiszkowski       Added drive reselection + setup delay on Init
 * 2008-03-29  T. Wiszkowski       Restored error on 64bit R/W access to non-64bit capable atapi devices
 *                                 cleared debug flag
 * 2008-03-30  T. Wiszkowski       Added workaround for interrupt collision handling; fixed SATA in LEGACY mode.
 *                                 nForce and Intel SATA chipsets should now be operational (nForce confirmed)
 * 2008-03-31  M. Schulz           The ins/outs function definitions used only in case of x86 and x86_64 architectures.
 *                                 Otherwise, function declaratons are emitted.
 * 2008-04-01  M. Schulz           Use C functions ahci_ins[wl] ahci_outs[wl]
 * 2008-04-03  T. Wiszkowski       Fixed IRQ flood issue, eliminated and reduced obsolete / redundant code
 * 2008-04-05  T. Wiszkowski       Improved IRQ management
 * 2008-04-07  T. Wiszkowski       Changed bus timeout mechanism
 *                                 increased failure timeout values to cover rainy day scenarios
 * 2008-04-20  T. Wiszkowski       Corrected the flaw in drive identification routines leading to ocassional system hangups
 * 2008-05-11  T. Wiszkowski       Remade the ata trannsfers altogether, corrected the pio/irq handling
 *                                 medium removal, device detection, bus management and much more
 * 2008-05-12  P. Fedin	           Explicitly enable multisector transfers on the drive
 * 2008-05-18  T. Wiszkowski       Added extra checks to prevent duplicating drive0 in drive0 only configs
 * 2008-05-18  T. Wiszkowski       Replaced static C/H/S with more accurate calcs, should make HDTB and other tools see right capacity
 * 2008-05-19  T. Wiszkowski       Updated ATA DMA handling and transfer wait operation to allow complete transfer before dma_StopDMA()
 * 2008-05-30  T. Wiszkowski       Corrected CHS calculation for larger disks
 * 2008-06-03  K. Smiechowicz      Added 400ns delay in ahci_WaitBusyTO before read of device status.
 * 2008-06-25  P. Fedin            Added "nomulti" flag
 *                                 PIO works correctly again
 * 2008-11-28  T. Wiszkowski       updated test unit ready to suit individual taste of hw manufacturers
 * 2009-01-20  J. Koivisto         Modified bus reseting scheme
 * 2009-02-04  T. Wiszkowski       Disabled ATA debug on official builds
 * 2009-03-05  T. Wiszkowski       remade timeouts, added timer-based and benchmark-based delays.
 * 2011-05-19  P. Fedin		   The Big Rework. Separated bus-specific code. Made 64-bit-friendly.
 */
/*
 * TODO:
 * - put a critical section around DMA transfers (shared dma channels)
 */

#define DEBUG 0
// use #define xxx(a) D(a) to enable particular sections.
#if DEBUG
#define DIRQ(a) D(a)
#define DIRQ_MORE(a)
#define DUMP(a) D(a)
#define DUMP_MORE(a)
#define DATA(a) D(a)
#define DATAPI(a) D(a)
#define DINIT(a) (a)
#else
#define DIRQ(a)      do { } while (0)
#define DIRQ_MORE(a) do { } while (0)
#define DUMP(a)      do { } while (0)
#define DUMP_MORE(a) do { } while (0)
#define DATA(a)      do { } while (0)
#define DATAPI(a)    do { } while (0)
#define DINIT(a)     do { } while (0)
#endif

#include <aros/debug.h>
#include <exec/types.h>
#include <exec/exec.h>
#include <exec/resident.h>
#include <utility/utility.h>
#include <oop/oop.h>

#include <proto/exec.h>
#include <devices/timer.h>
#include <devices/cd.h>

#include "ahci_intern.h"
#include "ata.h"
#include "timer.h"

#define TIMEOUT 500

#define ATA_IN(a,b) (0)
#define ATA_OUT(a,b,c) do { } while (0)

/*
    Prototypes of static functions from lowlevel.c. I do not want to make them
    non-static as I'd like to remove as much symbols from global table as possible.
    Besides some of this functions could conflict with old ide.device or any other
    device.
*/
static BYTE ahci_ReadSector32(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE ahci_ReadSector64(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
static BYTE ahci_ReadMultiple32(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE ahci_ReadMultiple64(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
static BYTE ahci_ReadDMA32(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE ahci_ReadDMA64(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
static BYTE ahci_WriteSector32(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE ahci_WriteSector64(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
static BYTE ahci_WriteMultiple32(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE ahci_WriteMultiple64(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
static BYTE ahci_WriteDMA32(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE ahci_WriteDMA64(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
static BYTE ahci_Eject(struct ahci_Unit *);
static BOOL ahci_WaitBusyTO(struct ahci_Unit *unit, UWORD tout, BOOL irq, UBYTE *stout);

static BYTE atapi_EndCmd(struct ahci_Unit *unit);

static BYTE atapi_Read(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE atapi_Write(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
static BYTE atapi_Eject(struct ahci_Unit *);

static void common_SetBestXferMode(struct ahci_Unit* unit);

ULONG atapi_RequestSense(struct ahci_Unit* unit, UBYTE* sense, ULONG senselen);

#if DEBUG
static void dump(APTR mem, ULONG len)
{
    register int i, j = 0;

    DUMP_MORE(for (j=0; j<(len+15)>>4; ++j))
    {
        bug("[ATA  ] %06lx: ", j<<4);

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
    bug("[ATA%02ld] CALLED STUB FUNCTION (GENERIC). THIS OPERATION IS NOT "
        "SUPPORTED BY DEVICE\n", au->au_UnitNum);
    return CDERR_NOCMD;
}

static BYTE ahci_STUB_IO32(struct ahci_Unit *au, ULONG blk, ULONG len,
    APTR buf, ULONG* act)
{
    bug("[ATA%02ld] CALLED STUB FUNCTION (IO32). THIS OPERATION IS NOT "
        "SUPPORTED BY DEVICE\n", au->au_UnitNum);
    return CDERR_NOCMD;
}

static BYTE ahci_STUB_IO64(struct ahci_Unit *au, UQUAD blk, ULONG len,
    APTR buf, ULONG* act)
{
    bug("[ATA%02ld] CALLED STUB FUNCTION -- IO ACCESS TO BLOCK %08lx:%08lx, LENGTH %08lx. THIS OPERATION IS NOT SUPPORTED BY DEVICE\n", au->au_UnitNum, (blk >> 32), (blk & 0xffffffff), len);
    return CDERR_NOCMD;
}

static BYTE ahci_STUB_SCSI(struct ahci_Unit *au, struct SCSICmd* cmd)
{
    bug("[ATA%02ld] CALLED STUB FUNCTION. THIS OPERATION IS NOT SUPPORTED BY DEVICE\n", au->au_UnitNum);
    return CDERR_NOCMD;
}

static inline struct ahci_Unit* ahci_GetSelectedUnit(struct ahci_Port* bus)
{
    return &bus->ap_Unit;
}

static inline UBYTE ahci_ReadStatus(struct ahci_Port *port)
{
    return 0; /* FIXME */
}

static inline UBYTE ahci_ReadAltStatus(struct ahci_Port *port)
{
    return 0; /* FIXME */
}

static inline BOOL ahci_SelectUnit(struct ahci_Unit* unit)
{
    return (&unit->au_Port->ap_Unit == unit);
}

/*
 * handle IRQ; still fast and efficient, supposed to verify if this irq is for us and take adequate steps
 * part of code moved here from ata.c to reduce containment
 */
void ahci_IRQSignalTask(struct ahci_Port *port)
{
    port->ap_IntCnt++;
    Signal(port->ap_Task, 1UL << port->ap_SleepySignal);
}

/* PIO-only: IDE doublers don't have real AltStatus register.
 * Do not assume reading AltStatus does not clear interrupt request.
 */
void ahci_HandleIRQ(struct ahci_Port *port)
{
    BOOL for_us = TRUE; /* FIXME: Really? */
    ULONG status = 0;

    if (for_us)
    {
        /*
         * Acknowledge interrupt (note that the DMA interrupt bit should be
         * cleared for all interrupt types)
         */
        /* FIXME: Do this! */

        /*
         * ok, we have a routine to handle any form of transmission etc.
         */
        DIRQ(bug("[ATA  ] IRQ: Calling dedicated handler... \n"));
        port->ap_HandleIRQ(&port->ap_Unit, status);

        return;
    }
}

void ahci_IRQSetHandler(struct ahci_Unit *unit, void (*handler)(struct ahci_Unit*, UBYTE), APTR piomem, ULONG blklen, ULONG piolen)
{
    if (NULL != handler)
        unit->au_cmd_error = 0;

    unit->au_cmd_data = piomem;
    unit->au_cmd_length = (piolen < blklen) ? piolen : blklen;
    unit->au_cmd_total = piolen;
    unit->au_Port->ap_HandleIRQ = handler;
}

void ahci_IRQNoData(struct ahci_Unit *unit, UBYTE status)
{
    if (status & ATAF_BUSY)
        return;

    if ((unit->au_cmd_error == 0) && (status & ATAF_ERROR))
        unit->au_cmd_error = HFERR_BadStatus;

    DIRQ(bug("[ATA%02ld] IRQ: NoData - done; status %02lx.\n", unit->au_UnitNum, status));
    ahci_IRQSetHandler(unit, NULL, NULL, 0, 0);
    ahci_IRQSignalTask(unit->au_Port);
}

void ahci_IRQPIORead(struct ahci_Unit *unit, UBYTE status)
{
	DIRQ(bug("[ATA  ] IRQ: PIOReadData - DRQ.\n"));
	/* TODO: Handle PIO finish IRQ */

	/*
	 * indicate it's all done here
	 */
	unit->au_cmd_data += unit->au_cmd_length;
	unit->au_cmd_total -= unit->au_cmd_length;
	if (unit->au_cmd_total) {
	    if (unit->au_cmd_length > unit->au_cmd_total)
		unit->au_cmd_length = unit->au_cmd_total;
	    return;
	}
	DIRQ(bug("[ATA  ] IRQ: PIOReadData - transfer completed.\n"));
    ahci_IRQNoData(unit, status);
}

void ahci_PIOWriteBlk(struct ahci_Unit *unit)
{
    /* TODO: Handle PIO Block Write IRQ */

    /*
     * indicate it's all done here
     */
    unit->au_cmd_data += unit->au_cmd_length;
    unit->au_cmd_total -= unit->au_cmd_length;
    if (unit->au_cmd_length > unit->au_cmd_total)
        unit->au_cmd_length = unit->au_cmd_total;
}

void ahci_IRQPIOWrite(struct ahci_Unit *unit, UBYTE status)
{
    if (status & ATAF_DATAREQ) {
	DIRQ(bug("[ATA  ] IRQ: PIOWriteData - DRQ.\n"));
	ahci_PIOWriteBlk(unit);
	return;
    }
    DIRQ(bug("[ATA  ] IRQ: PIOWriteData - done.\n"));
    ahci_IRQNoData(unit, status);
}

void ahci_IRQDMAReadWrite(struct ahci_Unit *unit, UBYTE status)
{
    UBYTE stat = ATA_IN(dma_Status, unit->au_DMAPort);

    DIRQ(bug("[ATA%02ld] IRQ: IO status %02lx, DMA status %02lx\n", unit->au_UnitNum, status, stat));

    if ((status & ATAF_ERROR) || (stat & DMAF_Error))
    {
        /* This is turned on in order to help Phantom - Pavel Fedin <sonic_amiga@rambler.ru> */
        bug("[ATA%02ld] IRQ: IO status %02lx, DMA status %02lx\n", unit->au_UnitNum, status, stat);
        bug("[ATA%02ld] IRQ: ERROR %02lx\n", unit->au_UnitNum, ATA_IN(atapi_Error, unit->au_Port->ap_PortNum));
        bug("[ATA  ] IRQ: DMA Failed.\n");

        unit->au_cmd_error = HFERR_DMA;
        ahci_IRQNoData(unit, status);
    }
    else if (0 == (status & (ATAF_BUSY | ATAF_DATAREQ)))
    {
        DIRQ(bug("[ATA  ] IRQ: DMA Done.\n"));
        ahci_IRQNoData(unit, status);
    }
}

void ahci_IRQPIOReadAtapi(struct ahci_Unit *unit, UBYTE status)
{
    //ULONG port = unit->au_Port->ap_PortNum;
    ULONG size = 0;
    LONG remainder = 0;
    UBYTE reason = ATA_IN(atapi_Reason, port);
    DIRQ(bug("[DSCSI] Current status: %ld during READ\n", reason));

    /* have we failed yet? */
    if (0 == (status & (ATAF_BUSY | ATAF_DATAREQ)))
        ahci_IRQNoData(unit, status);
    if (status & ATAF_ERROR)
    {
        ahci_IRQNoData(unit, status);
        return;
    }

    /* anything for us please? */
    if (ATAPIF_READ != (reason & ATAPIF_MASK))
        return;

    size = ATA_IN(atapi_ByteCntH, port) << 8 | ATA_IN(atapi_ByteCntL, port);
    DIRQ(bug("[ATAPI] IRQ: data available for read (%ld bytes, max: %ld bytes)\n", size, unit->au_cmd_total));

    if (size > unit->au_cmd_total)
    {
        bug("[ATAPI] IRQ: CRITICAL! MORE DATA OFFERED THAN STORAGE CAN TAKE: %ld bytes vs %ld bytes left!\n", size, unit->au_cmd_total);
        remainder = size - unit->au_cmd_total;
        size = unit->au_cmd_total;
    }

    /* FIXME: unit->au_ins(unit->au_cmd_data, port, size, unit->au_Port->ap_DriverData); */
    unit->au_cmd_data = &((UBYTE*)unit->au_cmd_data)[size];
    unit->au_cmd_total -= size;

    DIRQ(bug("[ATAPI] IRQ: %lu bytes read.\n", size));

    for (; remainder > 0; remainder -= 2) {
        /* FIXME: unit->au_ins(&size, port, 2, unit->au_Port->ap_DriverData);
         */
    }

    if (unit->au_cmd_total == 0)
        ahci_IRQSetHandler(unit, &ahci_IRQNoData, NULL, 0, 0);
}

void ahci_IRQPIOWriteAtapi(struct ahci_Unit *unit, UBYTE status)
{
    //ULONG port = unit->au_Port->ap_PortNum;
    ULONG size = 0;
    UBYTE reason = ATA_IN(atapi_Reason, port);
    DIRQ(bug("[ATAPI] IRQ: Current status: %ld during WRITE\n", reason));

    /* have we failed yet? */
    if (0 == (status & (ATAF_BUSY | ATAF_DATAREQ)))
        ahci_IRQNoData(unit, status);
    if (status & ATAF_ERROR)
    {
        ahci_IRQNoData(unit, status);
        return;
    }

    /* anything for us please? */
    if (ATAPIF_WRITE != (reason & ATAPIF_MASK))
        return;

    size = ATA_IN(atapi_ByteCntH, port) << 8 | ATA_IN(atapi_ByteCntL, port);
    DIRQ(bug("[ATAPI] IRQ: data requested for write (%ld bytes, max: %ld bytes)\n", size, unit->au_cmd_total));

    if (size > unit->au_cmd_total)
    {
        bug("[ATAPI] IRQ: CRITICAL! MORE DATA REQUESTED THAN STORAGE CAN GIVE: %ld bytes vs %ld bytes left!\n", size, unit->au_cmd_total);
        size = unit->au_cmd_total;
    }

    /* FIXME: unit->au_outs(unit->au_cmd_data, port, size, unit->au_Port->ap_DriverData);
     */
    unit->au_cmd_data = &((UBYTE*)unit->au_cmd_data)[size];
    unit->au_cmd_total -= size;

    DIRQ(bug("[ATAPI] IRQ: %lu bytes written.\n", size));

    if (unit->au_cmd_total == 0)
        ahci_IRQSetHandler(unit, &ahci_IRQNoData, NULL, 0, 0);
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
    status = ATA_IN(ahci_AltStatus, unit->au_Port->ap_Alt);

    if (irq)
    {
        /*
         * wait for either IRQ or TIMEOUT (unless device seems to be a
         * phantom SATAPI drive, in which case we fake a timeout)
         */
        DIRQ(bug("[ATA%02ld] Waiting (Current status: %02lx)...\n",
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
            bug("[ATA%02ld] Timeout while waiting for device to complete"
                " operation\n", unit->au_UnitNum);
            res = FALSE;

            /*
             * do nothing if the interrupt eventually arrives
             */
            Disable();
            ahci_IRQSetHandler(unit, NULL, NULL, 0, 0);
            Enable();
        }
    }
    else
    {
        while (status & ATAF_BUSY)
        {
            ++step;

            /*
             * every 16n rounds do some extra stuff
             */
            if ((step & 16) == 0)
            {
                /*
                 * huhm. so it's been 16n rounds already. any timeout yet?
                 */
                if (SetSignal(0, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
                {
                    DIRQ(bug("[ATA%02ld] Device still busy after timeout."
                        " Aborting\n", unit->au_UnitNum));
                    res = FALSE;
                    break;
                }

                /*
                 * no timeout just yet, but it's not a good idea to keep
                 * spinning like that. let's give the system some time.
                 */
                ahci_WaitNano(400);
            }

            status = ATA_IN(ahci_AltStatus, unit->au_Port->ap_Alt);
        }
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
    status = ATA_IN(ahci_Status, unit->au_Port->ap_PortNum);

    /*
     * be nice to frustrated developer
     */
    DIRQ(bug("[ATA%02ld] WaitBusy status: %lx / %ld\n", unit->au_UnitNum,
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
 * Procedure for sending ATA command blocks
 * it appears LARGE but there's a lot of COMMENTS here :)
 * handles *all* ata commands (no data, pio and dma)
 * naturally could be split at some point in the future
 * depends if anyone believes that the change for 50 lines
 * would make slow ATA transfers any faster
 */
static BYTE ahci_exec_cmd(struct ahci_Unit* unit, ata_CommandBlock *block)
{
    //ULONG port = unit->au_Port->ap_PortNum;
    BYTE err = 0;
    APTR mem = block->buffer;
    UBYTE status;

    if (FALSE == ahci_SelectUnit(unit))
        return IOERR_UNITBUSY;

    switch (block->type)
    {
        case CT_LBA28:
            if (block->sectors > 256)
            {
                bug("[ATA%02ld] ahci_exec_cmd: ERROR: Transfer length (%ld) exceeds 256 sectors. Aborting.\n", unit->au_UnitNum, block->sectors);
                return IOERR_BADLENGTH;
            }

            /* note:
             * we want the above to fall in here!
             * we really do (checking for secmul)
             */

        case CT_LBA48:
            if (block->sectors > 65536)
            {
                bug("[ATA%02ld] ahci_exec_cmd: ERROR: Transfer length (%ld) exceeds 65536 sectors. Aborting.\n", unit->au_UnitNum, block->sectors);
                return IOERR_BADLENGTH;
            }
            if (block->secmul == 0)
            {
                bug("[ATA%02ld] ahci_exec_cmd: ERROR: Invalid transfer multiplier. Should be at least set to 1 (correcting)\n", unit->au_UnitNum);
                block->secmul = 1;
            }
           break;

        case CT_NoBlock:
            break;

        default:
            bug("[ATA%02ld] ahci_exec_cmd: ERROR: Invalid command type %lx. Aborting.\n", unit->au_UnitNum, block->type);
            return IOERR_NOCMD;
    }

    block->actual = 0;
    D(bug("[ATA%02ld] ahci_exec_cmd: Executing command %02lx\n", unit->au_UnitNum, block->command));

    if (block->feature != 0)
        ATA_OUT(block->feature, ahci_Feature, port);

    /*
     * - set LBA and sector count
     */
    switch (block->type)
    {
        case CT_LBA28:
            DATA(bug("[ATA%02ld] ahci_exec_cmd: Command uses 28bit LBA addressing (OLD)\n", unit->au_UnitNum));

            ATA_OUT(((block->blk >> 24) & 0x0f) | 0x40 | unit->au_DevMask, ahci_DevHead, port);
            ATA_OUT(block->blk >> 16, ahci_LBAHigh, port);
            ATA_OUT(block->blk >> 8, ahci_LBAMid, port);
            ATA_OUT(block->blk, ahci_LBALow, port);
            ATA_OUT(block->sectors, ahci_Count, port);
            break;

        case CT_LBA48:
            DATA(bug("[ATA%02ld] ahci_exec_cmd: Command uses 48bit LBA addressing (NEW)\n", unit->au_UnitNum));

            ATA_OUT(0x40 | unit->au_DevMask, ahci_DevHead, port);
            ATA_OUT(block->blk >> 40, ahci_LBAHigh, port);
            ATA_OUT(block->blk >> 32, ahci_LBAMid, port);
            ATA_OUT(block->blk >> 24, ahci_LBALow, port);

            ATA_OUT(block->blk >> 16, ahci_LBAHigh, port);
            ATA_OUT(block->blk >> 8, ahci_LBAMid, port);
            ATA_OUT(block->blk, ahci_LBALow, port);

            ATA_OUT(block->sectors >> 8, ahci_Count, port);
            ATA_OUT(block->sectors, ahci_Count, port);
            break;

        case CT_NoBlock:
            DATA(bug("[ATA%02ld] ahci_exec_cmd: Command does not address any block\n", unit->au_UnitNum));
            break;
    }

    switch (block->method)
    {
        case CM_PIOWrite:
            ahci_IRQSetHandler(unit, &ahci_IRQPIOWrite, mem, block->secmul << unit->au_SectorShift, block->length);
            break;

        case CM_PIORead:
            ahci_IRQSetHandler(unit, &ahci_IRQPIORead, mem, block->secmul << unit->au_SectorShift, block->length);
            break;

#ifdef FIXME_DMA
        case CM_DMARead:
            if (FALSE == dma_SetupPRDSize(unit, mem, block->length, TRUE))
                return IOERR_ABORTED;
            ahci_IRQSetHandler(unit, &ahci_IRQDMAReadWrite, NULL, 0, 0);
            dma_StartDMA(unit);
            break;

        case CM_DMAWrite:
            if (FALSE == dma_SetupPRDSize(unit, mem, block->length, FALSE))
                return IOERR_ABORTED;
            ahci_IRQSetHandler(unit, &ahci_IRQDMAReadWrite, NULL, 0, 0);
            dma_StartDMA(unit);
            break;
#endif

        case CM_NoData:
            ahci_IRQSetHandler(unit, &ahci_IRQNoData, NULL, 0, 0);
            break;

        default:
            return IOERR_NOCMD;
            break;
    };

    /*
     * send command now
     * let drive propagate its signals
     */
    DATA(bug("[ATA%02ld] ahci_exec_cmd: Sending command\n", unit->au_UnitNum));
    ATA_OUT(block->command, ahci_Command, port);
    ahci_WaitNano(400);
    //ahci_WaitTO(unit->au_Port->ap_Timer, 0, 1, 0);

    /*
     * In case of PIO write the drive won't issue an IRQ before first
     * data transfer, so we should poll the status and send the first
     * block upon request.
     */
    if (block->method == CM_PIOWrite)
    {
	if (FALSE == ahci_WaitBusyTO(unit, TIMEOUT, FALSE, &status)) {
	    D(bug("[ATA%02ld] ahci_exec_cmd: PIOWrite - no response from device\n", unit->au_UnitNum));
	    return IOERR_UNITBUSY;
	}
	if (status & ATAF_DATAREQ) {
	    DATA(bug("[ATA%02ld] ahci_exec_cmd: PIOWrite - DRQ.\n", unit->au_UnitNum));
	    ahci_PIOWriteBlk(unit);
	}
	else
	{
	    D(bug("[ATA%02ld] ahci_exec_cmd: PIOWrite - bad status: %02X\n", status));
	    return HFERR_BadStatus;
	}
    }

    /*
     * wait for drive to complete what it has to do
     */
    if (FALSE == ahci_WaitBusyTO(unit, TIMEOUT, TRUE, NULL))
    {
        bug("[ATA%02ld] ahci_exec_cmd: Device is late - no response\n", unit->au_UnitNum);
        err = IOERR_UNITBUSY;
    }
    else
        err = unit->au_cmd_error;

    DATA(bug("[ATA%02ld] ahci_exec_cmd: Command done\n", unit->au_UnitNum));
    /*
     * clean up DMA
     * don't use 'mem' pointer here as it's already invalid.
     */
#ifdef FIXME_DMA
    if (block->method == CM_DMARead)
    {
        dma_StopDMA(unit);
        dma_Cleanup(block->buffer, block->length, TRUE);
    }
    else if (block->method == CM_DMAWrite)
    {
        dma_StopDMA(unit);
        dma_Cleanup(block->buffer, block->length, FALSE);
    }
#endif

    D(bug("[ATA%02ld] ahci_exec_cmd: return code %ld\n", unit->au_UnitNum, err));
    return err;
}

/*
 * atapi packet iface
 */
BYTE atapi_SendPacket(struct ahci_Unit *unit, APTR packet, APTR data, LONG datalen, BOOL *dma, BOOL write)
{
    *dma = *dma && (unit->au_XferModes & AF_XFER_DMA) ? TRUE : FALSE;
    LONG err = 0;

    UBYTE cmd[12] = {
        0
    };
    register int t=5,l=0;
    //ULONG port = unit->au_Port->ap_PortNum;

    if (((UBYTE*)packet)[0] > 0x1f)
        t+= 4;
    if (((UBYTE*)packet)[0] > 0x5f)
        t+= 2;

    switch (((UBYTE*)packet)[0])
    {
        case 0x28:  // read10
        case 0xa8:  // read12
        case 0xbe:  // readcd
        case 0xb9:  // readcdmsf
        case 0x2f:  // verify
        case 0x2a:  // write
        case 0xaa:  // write12
        case 0x2e:  // writeverify
        case 0xad:  // readdvdstructure
        case 0xa4:  // reportkey
        case 0xa3:  // sendkey
            break;
        default:
            *dma = FALSE;
    }

    while (l<=t)
    {
        cmd[l] = ((UBYTE*)packet)[l];
        ++l;
    }

    DATAPI({
        bug("[ATA%02lx] Sending %s ATA packet: ", unit->au_UnitNum, (*dma) ? "DMA" : "PIO");
        l=0;
        while (l<=t)
        {
            bug("%02lx ", ((UBYTE*)cmd)[l]);
            ++l;
        }
        bug("\n");

        if (datalen & 1)
            bug("[ATAPI] ERROR: DATA LENGTH NOT EVEN! Rounding Up! (%ld bytes requested)\n", datalen);
    });

    datalen = (datalen+1)&~1;

    if (FALSE == ahci_SelectUnit(unit))
    {
        DATAPI(bug("[ATAPI] WaitBusy failed at first check\n"));
        return IOERR_UNITBUSY;
    }

    /*
     * tell device whether we want to read or write and if we want a dma transfer
     */
    ATA_OUT(((*dma) ? 1 : 0) |
        (((unit->au_Drive->id_DMADir & 0x8000) && !write) ? 4 : 0),
        atapi_Features, port);
    ATA_OUT((datalen & 0xff), atapi_ByteCntL, port);
    ATA_OUT((datalen >> 8) & 0xff, atapi_ByteCntH, port);

    /*
     * once we're done with that, we can go ahead and inform device that we're about to send atapi packet
     * after command is dispatched, we are obliged to give 400ns for the unit to parse command and set status
     */
    DATAPI(bug("[ATAPI] Issuing ATA_PACKET command.\n"));
    ahci_IRQSetHandler(unit, &ahci_IRQNoData, 0, 0, 0);
    ATA_OUT(ATA_PACKET, atapi_Command, port);
    ahci_WaitNano(400);
    //ahci_WaitTO(unit->au_Port->ap_Timer, 0, 1, 0);

    ahci_WaitBusyTO(unit, TIMEOUT, (unit->au_Drive->id_General & 0x60) == 0x20,
        NULL);
    if (0 == (ahci_ReadStatus(unit->au_Port) & ATAF_DATAREQ))
        return HFERR_BadStatus;

    /*
     * setup appropriate hooks
     */
    if (datalen == 0)
        ahci_IRQSetHandler(unit, &ahci_IRQNoData, 0, 0, 0);
    else if (*dma)
        ahci_IRQSetHandler(unit, &ahci_IRQDMAReadWrite, NULL, 0, 0);
    else if (write)
        ahci_IRQSetHandler(unit, &ahci_IRQPIOWriteAtapi, data, 0, datalen);
    else
        ahci_IRQSetHandler(unit, &ahci_IRQPIOReadAtapi, data, 0, datalen);

#ifdef FIXME_DMA
    if (*dma)
    {
        DATAPI(bug("[ATAPI] Starting DMA\n"));
        dma_StartDMA(unit);
    }
#endif

    DATAPI(bug("[ATAPI] Sending packet\n"));
    // FIXME: unit->au_outs(cmd, unit->au_Port->ap_PortNum, 12, unit->au_Port->ap_DriverData);
    ahci_WaitNano(400);
    DATAPI(bug("[ATAPI] Status after packet: %lx\n",
        ahci_ReadAltStatus(unit->au_Port)));

    /*
     * Wait for command to complete. Note that two interrupts will occur
     * before we wake up if this is a PIO data transfer
     */
    if (ahci_WaitTO(unit->au_Port->ap_Timer, TIMEOUT, 0,
        1 << unit->au_Port->ap_SleepySignal) == 0)
    {
        DATAPI(bug("[DSCSI] Command timed out.\n"));
        err = IOERR_UNITBUSY;
    }
    else
        err = atapi_EndCmd(unit);

#ifdef FIXME_DMA
    if (*dma)
    {
        dma_StopDMA(unit);
        dma_Cleanup(data, datalen, !write);
    }
#endif

    DATAPI(bug("[ATAPI] IO error code %ld\n", err));
    return err;
}

BYTE atapi_DirectSCSI(struct ahci_Unit *unit, struct SCSICmd *cmd)
{
    //APTR buffer = cmd->scsi_Data;
    //ULONG length = cmd->scsi_Length;
    //BOOL read = FALSE;
    BYTE err = 0;
    BOOL dma = FALSE;

    cmd->scsi_Actual = 0;

    DATAPI(bug("[DSCSI] Sending packet!\n"));

#ifdef FIXME_DMA
    /*
     * setup DMA & push command
     * it does not really mean we will use dma here btw
     */
    if ((unit->au_XferModes & AF_XFER_DMA) && (length !=0) && (buffer != 0))
    {
        dma = TRUE;
        if ((cmd->scsi_Flags & SCSIF_READ) != 0)
        {
            read = TRUE;
            if (FALSE == dma_SetupPRDSize(unit, buffer, length, TRUE))
                dma = FALSE;
        }
        else
        {
            if (FALSE == dma_SetupPRDSize(unit, buffer, length, FALSE))
                dma = FALSE;
        }
    }
#endif

    err = atapi_SendPacket(unit, cmd->scsi_Command, cmd->scsi_Data, cmd->scsi_Length, &dma, (cmd->scsi_Flags & SCSIF_READ) == 0);

    DUMP({ if (cmd->scsi_Data != 0) dump(cmd->scsi_Data, cmd->scsi_Length); });

    /*
     * on check condition - grab sense data
     */
    DATAPI(bug("[ATA%02lx] atapi_DirectSCSI: SCSI Flags: %02lx / Error: %ld\n", unit->au_UnitNum, cmd->scsi_Flags, err));
    if ((err != 0) && (cmd->scsi_Flags & SCSIF_AUTOSENSE))
    {
        DATAPI(bug("[DSCSI] atapi_DirectSCSI: Packet Failed. Calling atapi_RequestSense\n"));
        atapi_RequestSense(unit, cmd->scsi_SenseData, cmd->scsi_SenseLength);
        DUMP(dump(cmd->scsi_SenseData, cmd->scsi_SenseLength));
    }

    return err;
}

/*
 * chops the large transfers into set of smaller transfers
 * specifically useful when requested transfer size is >256 sectors for 28bit commands
 */
static BYTE ahci_exec_blk(struct ahci_Unit *unit, ata_CommandBlock *blk)
{
    BYTE err = 0;
    ULONG part;
    ULONG max=256;
    ULONG count=blk->sectors;

    if (blk->type == CT_LBA48)
        max <<= 8;

    DATA(bug("[ATA%02ld] ahci_exec_blk: Accessing %ld sectors starting from %x%08x\n", unit->au_UnitNum, count, (ULONG)(blk->blk >> 32), (ULONG)blk->blk));
    while ((count > 0) && (err == 0))
    {
        part = (count > max) ? max : count;
        blk->sectors = part;
        blk->length  = part << unit->au_SectorShift;

        DATA(bug("[ATA%02ld] Transfer of %ld sectors from %x%08x\n", unit->au_UnitNum, part, (ULONG)(blk->blk >> 32), (ULONG)blk->blk));
        err = ahci_exec_cmd(unit, blk);
        DATA(bug("[ATA%02ld] ahci_exec_blk: ahci_exec_cmd returned %lx\n", unit->au_UnitNum, err));

        blk->blk    += part;
        blk->buffer  = &((char*)blk->buffer)[part << unit->au_SectorShift];
        count -= part;
    }
    return err;
}

/*
 * Initial device configuration that suits *all* cases
 */
BOOL ahci_init_unit(struct ahci_Port *port)
{
    struct ahci_Unit *unit=NULL;

    DINIT(bug("[ATA  ] ahci_init_unit(%ld)\n"));

    unit = &port->ap_Unit;
    if (NULL == unit)
        return FALSE;

    unit->au_Port        = bus;
    unit->au_Drive      = AllocPooled(port->ap_Base->ahci_MemPool, sizeof(struct DriveIdent));
    unit->au_UnitNum    = port->ap_BusNum << 1 | u;      // b << 8 | u
    unit->au_DevMask    = 0xa0 | (u << 4);

    /*
     * 32-bit transfer routines are options.
     * If the bus doesn't support this, they will be NULLs.
     */
    if (port->ap_Base->ahci_32bit && port->ap_Driver->ahci_insl)
    {
        unit->au_ins        = port->ap_Driver->ahci_insl;
        unit->au_outs       = port->ap_Driver->ahci_outsl;
    }
    else
    {
        unit->au_ins        = port->ap_Driver->ahci_insw;
        unit->au_outs       = port->ap_Driver->ahci_outsw;
    }
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

BOOL ahci_setup_unit(struct ahci_Port *port, UBYTE u)
{
    struct ahci_Unit *unit=NULL;

    /*
     * this stuff always goes along the same way
     * WARNING: NO INTERRUPTS AT THIS POINT!
     */
    DINIT(bug("[ATA  ] ahci_setup_unit(%ld,%ld)\n", port->ap_BusNum, u));

    unit = port->ap_Units[u];
    if (NULL == unit)
        return FALSE;

    ahci_SelectUnit(unit);

    if (unit->au_DMAPort != 0
        && (ATA_IN(dma_Status, unit->au_DMAPort) & 0x80) != 0)
        bug("[ATA%02ld] ahci_setup_unit: WARNING: Controller only supports "
            "DMA on one bus at a time. DMAStatus=%lx\n", unit->au_UnitNum,
            ATA_IN(dma_Status, unit->au_DMAPort));

    if (FALSE == ahci_WaitBusyTO(unit, 1, FALSE, NULL))
    {
        DINIT(bug("[ATA%02ld] ahci_setup_unit: ERROR: Drive not ready for use. Keeping functions stubbed\n", unit->au_UnitNum));
        FreePooled(port->ap_Base->ahci_MemPool, unit->au_Drive, sizeof(struct DriveIdent));
        unit->au_Drive = 0;
        return FALSE;
    }

    switch (port->ap_Dev[u])
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
            DINIT(bug("[ATA%02ld] ahci_setup_unit: Unsupported device %lx. All functions will remain stubbed.\n", unit->au_UnitNum, port->ap_Dev[u]));
            FreePooled(port->ap_Base->ahci_MemPool, unit->au_Drive, sizeof(struct DriveIdent));
            unit->au_Drive = 0;
            return FALSE;
    }

    DINIT(bug("[ATA  ] ahci_setup_unit: Enabling IRQs\n"));
    ATA_OUT(0x0, ahci_AltControl, port->ap_Alt);

    /*
     * now make unit self diagnose
     */
    if (unit->au_Identify(unit) != 0)
    {
        FreePooled(port->ap_Base->ahci_MemPool, unit->au_Drive, sizeof(struct DriveIdent));
        unit->au_Drive = NULL;
        return FALSE;
    }

    return TRUE;
}

/*
 * ata[pi] identify
 */
static void common_SetXferMode(struct ahci_Unit* unit, ahci_XferMode mode)
{
    UBYTE type=0;
    BOOL dma=FALSE;
#if 0 // We can't set drive modes unless we also set the controller's timing registers
    ata_CommandBlock acb =
    {
        ATA_SET_FEATURES,
        0x03,
        0x01,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        CM_NoData,
        CT_LBA28
    };
#endif
    DINIT(bug("[ATA%02ld] common_SetXferMode: Trying to set mode %d\n", unit->au_UnitNum, mode));

    if ((unit->au_DMAPort == 0) && (mode >= AB_XFER_MDMA0))
    {
        DINIT(bug("[ATA%02ld] common_SetXferMode: This controller does not own DMA port! Will set best PIO\n", unit->au_UnitNum));
        common_SetBestXferMode(unit);
        return;
    }

    /*
     * first, ONLY for ATA devices, set new commands
     */
    if (0 == (unit->au_XferModes & AF_XFER_PACKET))
    {
        if ((mode >= AB_XFER_PIO0) && (mode <= AB_XFER_PIO4))
        {
            if ((!unit->au_Port->ap_Base->ahci_NoMulti) && (unit->au_XferModes & AF_XFER_RWMULTI))
            {
                ahci_IRQSetHandler(unit, ahci_IRQNoData, NULL, 0, 0);
                ATA_OUT(unit->au_Drive->id_RWMultipleSize & 0xFF, ahci_Count, unit->au_Port->ap_PortNum);
                ATA_OUT(ATA_SET_MULTIPLE, ahci_Command, unit->au_Port->ap_PortNum);
                ahci_WaitBusyTO(unit, -1, TRUE, NULL);

                unit->au_Read32         = ahci_ReadMultiple32;
                unit->au_Write32        = ahci_WriteMultiple32;
                if (unit->au_XferModes & AF_XFER_48BIT)
                {
                    unit->au_Read64         = ahci_ReadMultiple64;
                    unit->au_Write64        = ahci_WriteMultiple64;
                }
            }
            else
            {
                unit->au_Read32         = ahci_ReadSector32;
                unit->au_Write32        = ahci_WriteSector32;
                if (unit->au_XferModes & AF_XFER_48BIT)
                {
                    unit->au_Read64         = ahci_ReadSector64;
                    unit->au_Write64        = ahci_WriteSector64;
                }
            }
        }
        else if ((mode >= AB_XFER_MDMA0) && (mode <= AB_XFER_MDMA2))
        {
            unit->au_Read32         = ahci_ReadDMA32;
            unit->au_Write32        = ahci_WriteDMA32;
            if (unit->au_XferModes & AF_XFER_48BIT)
            {
                unit->au_Read64         = ahci_ReadDMA64;
                unit->au_Write64        = ahci_WriteDMA64;
            }
        }
        else if ((mode >= AB_XFER_UDMA0) && (mode <= AB_XFER_UDMA6))
        {
            unit->au_Read32         = ahci_ReadDMA32;
            unit->au_Write32        = ahci_WriteDMA32;
            if (unit->au_XferModes & AF_XFER_48BIT)
            {
                unit->au_Read64         = ahci_ReadDMA64;
                unit->au_Write64        = ahci_WriteDMA64;
            }
        }
        else
        {
            unit->au_Read32         = ahci_ReadSector32;
            unit->au_Write32        = ahci_WriteSector32;
            if (unit->au_XferModes & AF_XFER_48BIT)
            {
                unit->au_Read64         = ahci_ReadSector64;
                unit->au_Write64        = ahci_WriteSector64;
            }
        }
    }

    if ((mode >= AB_XFER_PIO0) && (mode <= AB_XFER_PIO4))
    {
        type = 8 + (mode - AB_XFER_PIO0);
    }
    else if ((mode >= AB_XFER_MDMA0) && (mode <= AB_XFER_MDMA2))
    {
        type = 32 + (mode - AB_XFER_MDMA0);
        dma=TRUE;
    }
    else if ((mode >= AB_XFER_UDMA0) && (mode <= AB_XFER_UDMA6))
    {
        type = 64 + (mode - AB_XFER_UDMA0);
        dma=TRUE;
    }
    else
    {
        type = 0;
    }

#if 0 // We can't set drive modes unless we also set the controller's timing registers
    acb.sectors = type;
    if (0 != ahci_exec_cmd(unit, &acb))
    {
        DINIT(bug("[ATA%02ld] common_SetXferMode: ERROR: Failed to apply new xfer mode.\n", unit->au_UnitNum));
    }

    if (unit->au_DMAPort)
    {
        type = ATA_IN(dma_Status, unit->au_DMAPort);
        type &= 0x60;
        if (dma)
        {
            type |= 1 << (5 + (unit->au_UnitNum & 1));
        }
        else
        {
            type &= ~(1 << (5 + (unit->au_UnitNum & 1)));
        }

        DINIT(bug("[DSCSI] common_SetXferMode: Trying to apply new DMA (%lx) status: %02lx (unit %ld)\n", unit->au_DMAPort, type, unit->au_UnitNum & 1));

        ahci_SelectUnit(unit);
        ATA_OUT(type, dma_Status, unit->au_DMAPort);
        if (type == (ATA_IN(dma_Status, unit->au_DMAPort) & 0x60))
        {
            DINIT(bug("[DSCSI] common_SetXferMode: New DMA Status: %02lx\n", type));
        }
        else
        {
            DINIT(bug("[DSCSI] common_SetXferMode: Failed to modify DMA state for this device\n"));
            dma = FALSE;
        }
    }
#endif

    if (dma)
        unit->au_XferModes |= AF_XFER_DMA;
    else
        unit->au_XferModes &= ~AF_XFER_DMA;
}

static void common_SetBestXferMode(struct ahci_Unit* unit)
{
    int iter;
    int max = AB_XFER_UDMA6;

    if (unit->au_Port->ap_Base->ahci_NoDMA || (unit->au_DMAPort == 0)
        || (   !(unit->au_Drive->id_MWDMASupport & 0x0700)
            && !(unit->au_Drive->id_UDMASupport  & 0x7f00)))
    {
        /*
         * make sure you reduce scan search to pio here!
         * otherwise this and above function will fall into infinite loop
         */
        DINIT(bug("[ATA%02ld] common_SetBestXferMode: DMA is disabled for"
            " this drive.\n", unit->au_UnitNum));
        max = AB_XFER_PIO4;
    }
    else if (!(unit->au_Flags & AF_80Wire))
    {
        DINIT(bug("[ATA%02ld] common_SetBestXferMode: "
            "An 80-wire cable has not been detected for this drive. "
            "Disabling modes above UDMA2.\n", unit->au_UnitNum));
        max = AB_XFER_UDMA2;
    }

    for (iter=max; iter>=AB_XFER_PIO0; --iter)
    {
        if (unit->au_XferModes & (1<<iter))
        {
            common_SetXferMode(unit, iter);
            return;
        }
    }
    bug("[ATA%02ld] common_SetBestXferMode: ERROR: device never reported any valid xfer modes. will continue at default\n", unit->au_UnitNum);
    common_SetXferMode(unit, AB_XFER_PIO0);
}

void common_DetectXferModes(struct ahci_Unit* unit)
{
    int iter;

    DINIT(bug("[ATA%02ld] common_DetectXferModes: Supports\n", unit->au_UnitNum));

    if (unit->au_Drive->id_Commands4 & (1 << 4))
    {
        DINIT(bug("[ATA%02ld] common_DetectXferModes: - Packet interface\n", unit->au_UnitNum));
        unit->au_XferModes     |= AF_XFER_PACKET;
        unit->au_DirectSCSI     = atapi_DirectSCSI;
    }
    else if (unit->au_Drive->id_Commands5 & (1 << 10))
    {
        /* ATAPI devices do not use this bit. */
        DINIT(bug("[ATA%02ld] common_DetectXferModes: - 48bit I/O\n", unit->au_UnitNum));
        unit->au_XferModes     |= AF_XFER_48BIT;
    }

    if ((unit->au_XferModes & AF_XFER_PACKET) || (unit->au_Drive->id_Capabilities & (1<< 9)))
    {
        DINIT(bug("[ATA%02ld] common_DetectXferModes: - LBA Addressing\n", unit->au_UnitNum));
        unit->au_XferModes     |= AF_XFER_LBA;
    }
    else
    {
        DINIT(bug("[ATA%02ld] common_DetectXferModes: - DEVICE DOES NOT SUPPORT LBA ADDRESSING >> THIS IS A POTENTIAL PROBLEM <<\n", unit->au_UnitNum));
    }

    if (unit->au_Drive->id_RWMultipleSize & 0xff)
    {
        DINIT(bug("[ATA%02ld] common_DetectXferModes: - R/W Multiple (%ld sectors per xfer)\n", unit->au_UnitNum, unit->au_Drive->id_RWMultipleSize & 0xff));
        unit->au_XferModes     |= AF_XFER_RWMULTI;
    }

    DINIT(bug("[ATA%02ld] common_DetectXferModes: - PIO0 PIO1 PIO2 ",
        unit->au_UnitNum));
    unit->au_XferModes |= AF_XFER_PIO(0) | AF_XFER_PIO(1) | AF_XFER_PIO(2);
    if (unit->au_Drive->id_ConfigAvailable & (1 << 1))
    {
        for (iter = 0; iter < 2; iter++)
        {
            if (unit->au_Drive->id_PIOSupport & (1 << iter))
            {
                DINIT(bug("PIO%ld ", 3 + iter));
                unit->au_XferModes |= AF_XFER_PIO(3 + iter);
            }
        }
        DINIT(bug("\n"));
    }

    if ((unit->au_Drive->id_ConfigAvailable & (1 << 1)) &&
        (unit->au_Drive->id_Capabilities & (1<<8)))
    {
        DINIT(bug("[ATA%02ld] common_DetectXferModes: DMA:\n", unit->au_UnitNum));
        if (unit->au_Drive->id_MWDMASupport & 0xff)
        {
            DINIT(bug("[ATA%02ld] common_DetectXferModes: - ", unit->au_UnitNum));
            for (iter = 0; iter < 3; iter++)
            {
                if (unit->au_Drive->id_MWDMASupport & (1 << iter))
                {
                    unit->au_XferModes |= AF_XFER_MDMA(iter);
                    if (unit->au_Drive->id_MWDMASupport & (256 << iter))
                    {
                        DINIT(bug("[MDMA%ld] ", iter));
                    }
                    else
                    {
                        DINIT(bug("MDMA%ld ", iter));
                    }
                }
            }
            DINIT(bug("\n"));
        }

        if (unit->au_Drive->id_UDMASupport & 0xff)
        {
            DINIT(bug("[ATA%02ld] common_DetectXferModes: - ", unit->au_UnitNum));
            for (iter = 0; iter < 7; iter++)
            {
                if (unit->au_Drive->id_UDMASupport & (1 << iter))
                {
                    unit->au_XferModes |= AF_XFER_UDMA(iter);
                    if (unit->au_Drive->id_UDMASupport & (256 << iter))
                    {
                        DINIT(bug("[UDMA%ld] ", iter));
                    }
                    else
                    {
                        DINIT(bug("UDMA%ld ", iter));
                    }
                }
            }
            DINIT(bug("\n"));
        }
    }
}

#define SWAP_LE_WORD(x) (x) = AROS_LE2WORD((x))
#define SWAP_LE_LONG(x) (x) = AROS_LE2LONG((x))
#define SWAP_LE_QUAD(x) (x) = AROS_LE2LONG((x) >> 32) | (((QUAD)(AROS_LE2LONG((x) & 0xffffffff))) << 32)

BYTE ahci_Identify(struct ahci_Unit* unit)
{
    BOOL atapi = unit->au_Port->ap_Dev[unit->au_UnitNum & 1] & 0x80;
    ata_CommandBlock acb =
    {
        atapi ? ATA_IDENTIFY_ATAPI : ATA_IDENTIFY_DEVICE,
        0,
        1,
        0,
        0,
        0,
        unit->au_Drive,
        sizeof(struct DriveIdent),
        0,
        CM_PIORead,
        CT_NoBlock
    };

    /* If the right command fails, try the wrong one. If both fail, abort */
    DINIT(bug("[ATA%02ld] ahci_Identify: Executing ATA_IDENTIFY_%s command\n",
        unit->au_UnitNum, atapi ? "ATAPI" : "DEVICE"));
    if (ahci_exec_cmd(unit, &acb))
    {
        acb.command = atapi ? ATA_IDENTIFY_DEVICE : ATA_IDENTIFY_ATAPI;
        DINIT(bug("[ATA%02ld] ahci_Identify: Executing ATA_IDENTIFY_%s command"
            " instead\n", unit->au_UnitNum, atapi ? "DEVICE" : "ATAPI"));
        if (ahci_exec_cmd(unit, &acb))
        {
            DINIT(bug("[ATA%02ld] ahci_Identify: Both command variants failed\n",
                unit->au_UnitNum));
            return IOERR_OPENFAIL;
        }
        unit->au_Port->ap_Dev[unit->au_UnitNum & 1] ^= 0x82;
        atapi = unit->au_Port->ap_Dev[unit->au_UnitNum & 1] & 0x80;
        DINIT(bug("[ATA%02ld] ahci_Identify:"
            " Incorrect device signature detected."
            " Switching device type to %lx.\n", unit->au_UnitNum,
            unit->au_Port->ap_Dev[unit->au_UnitNum & 1]));
    }

    /*
     * If every second word is zero with 32-bit reads, switch to 16-bit
     * accesses for this drive and try again
     */
    if (unit->au_Port->ap_Base->ahci_32bit)
    {
        UWORD n = 0, *p, *limit;

        for (p = (UWORD *)unit->au_Drive, limit = p + 256; p < limit; p++)
            n |= *++p;

        if (n == 0)
        {
            DINIT(bug("[ATA%02ld] Identify data was invalid with 32-bit reads."
                " Switching to 16-bit mode.\n", unit->au_UnitNum));

            unit->au_ins  = unit->au_Port->ap_Driver->ahci_insw;
            unit->au_outs = unit->au_Port->ap_Driver->ahci_outsw;

            if (ahci_exec_cmd(unit, &acb))
                return IOERR_OPENFAIL;
        }
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
    SWAP_LE_WORD(unit->au_Drive->id_ATAVersion);
    SWAP_LE_WORD(unit->au_Drive->id_ATARevision);
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

    bug("[ATA%02ld] ahci_Identify: Unit info: %s / %s / %s\n", unit->au_UnitNum, unit->au_Model, unit->au_SerialNumber, unit->au_FirmwareRev);
    common_DetectXferModes(unit);
    common_SetBestXferMode(unit);

    if (unit->au_Drive->id_General & 0x80)
    {
        DINIT(bug("[ATA%02ld] ahci_Identify: Device is removable.\n", unit->au_UnitNum));
        unit->au_Flags |= AF_Removable;
    }

    unit->au_Capacity   = unit->au_Drive->id_LBASectors;
    unit->au_Capacity48 = unit->au_Drive->id_LBA48Sectors;
    bug("[ATA%02ld] ahci_Identify: Unit info: %07lx 28bit / %04lx:%08lx 48bit addressable blocks\n", unit->au_UnitNum, unit->au_Capacity, (ULONG)(unit->au_Capacity48 >> 32), (ULONG)(unit->au_Capacity48 & 0xfffffffful));

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
 * ata read32 commands
 */
static BYTE ahci_ReadSector32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_READ,
        0,
        1,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_PIORead,
        CT_LBA28
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_ReadSector32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_ReadMultiple32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_READ_MULTIPLE,
        0,
        unit->au_Drive->id_RWMultipleSize & 0xff,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_PIORead,
        CT_LBA28
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_ReadMultiple32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_ReadDMA32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    BYTE err;
    ata_CommandBlock acb =
    {
        ATA_READ_DMA,
        0,
        1,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_DMARead,
        CT_LBA28
    };

    D(bug("[ATA%02ld] ahci_ReadDMA32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

/*
 * ata read64 commands
 */
static BYTE ahci_ReadSector64(struct ahci_Unit *unit, UQUAD block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_READ64,
        0,
        1,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_PIORead,
        CT_LBA48
    };
    BYTE err = 0;

    D(bug("[ATA%02ld] ahci_ReadSector64()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_ReadMultiple64(struct ahci_Unit *unit, UQUAD block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_READ_MULTIPLE64,
        0,
        unit->au_Drive->id_RWMultipleSize & 0xff,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_PIORead,
        CT_LBA48
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_ReadMultiple64()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_ReadDMA64(struct ahci_Unit *unit, UQUAD block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_READ_DMA64,
        0,
        1,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_DMARead,
        CT_LBA48
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_ReadDMA64()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

/*
 * ata write32 commands
 */
static BYTE ahci_WriteSector32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_WRITE,
        0,
        1,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_PIOWrite,
        CT_LBA28
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_WriteSector32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_WriteMultiple32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_WRITE_MULTIPLE,
        0,
        unit->au_Drive->id_RWMultipleSize & 0xff,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_PIOWrite,
        CT_LBA28
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_WriteMultiple32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_WriteDMA32(struct ahci_Unit *unit, ULONG block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_WRITE_DMA,
        0,
        1,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_DMAWrite,
        CT_LBA28
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_WriteDMA32()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

/*
 * ata write64 commands
 */
static BYTE ahci_WriteSector64(struct ahci_Unit *unit, UQUAD block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_WRITE64,
        0,
        1,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_PIOWrite,
        CT_LBA48
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_WriteSector64()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_WriteMultiple64(struct ahci_Unit *unit, UQUAD block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_WRITE_MULTIPLE64,
        0,
        unit->au_Drive->id_RWMultipleSize & 0xff,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_PIOWrite,
        CT_LBA48
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_WriteMultiple64()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

static BYTE ahci_WriteDMA64(struct ahci_Unit *unit, UQUAD block,
    ULONG count, APTR buffer, ULONG *act)
{
    ata_CommandBlock acb =
    {
        ATA_WRITE_DMA64,
        0,
        1,
        0,
        block,
        count,
        buffer,
        count << unit->au_SectorShift,
        0,
        CM_DMAWrite,
        CT_LBA48
    };
    BYTE err;

    D(bug("[ATA%02ld] ahci_WriteDMA64()\n", unit->au_UnitNum));

    *act = 0;
    if (0 != (err = ahci_exec_blk(unit, &acb)))
        return err;

    *act = count << unit->au_SectorShift;
    return 0;
}

/*
 * ata miscellaneous commands
 */
static BYTE ahci_Eject(struct ahci_Unit *unit)
{
    ata_CommandBlock acb =
    {
        ATA_MEDIA_EJECT,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        CM_NoData,
        CT_NoBlock
    };

    D(bug("[ATA%02ld] ahci_Eject()\n", unit->au_UnitNum));

    return ahci_exec_cmd(unit, &acb);
}

/*
 * atapi commands
 */
int atapi_TestUnitOK(struct ahci_Unit *unit)
{
    UBYTE cmd[6] = {
        0
    };
    UBYTE sense[16] = {
        0
    };
    struct SCSICmd sc = {
       0
    };

    D(bug("[ATA%02ld] atapi_TestUnitOK()\n", unit->au_UnitNum));

    sc.scsi_Command = (void*) &cmd;
    sc.scsi_CmdLength = sizeof(cmd);
    sc.scsi_SenseData = (void*)&sense;
    sc.scsi_SenseLength = sizeof(sense);
    sc.scsi_Flags = SCSIF_AUTOSENSE;

    DATAPI(bug("[ATA%02ld] atapi_TestUnitOK: Testing Unit Ready sense...\n", unit->au_UnitNum));
    unit->au_DirectSCSI(unit, &sc);
    unit->au_SenseKey = sense[2];

    /*
     * we may have just lost the disc...?
     */
    /*
     * per MMC, drives are expected to return 02-3a-0# status, when disc is not present
     * that would translate into following code:
     *    int p1 = ((sense[2] == 2) && (sense[12] == 0x3a)) ? 1 : 0;
     * unfortunately, it's what MMC says, not what vendors code.
     */
    int p1 = (sense[2] == 2) ? 1 : 0;
    int p2 = (0 != (AF_DiscPresent & unit->au_Flags)) ? 1 : 0;

    if (p1 == p2)
    {
        //unit->au_Flags ^= AF_DiscPresent;
        if (p1 == 0)
            unit->au_Flags |= AF_DiscPresent;
        else
            unit->au_Flags &= ~AF_DiscPresent;

        unit->au_Flags |= AF_DiscChanged;
    }

    DATAPI(bug("[ATA%02ld] atapi_TestUnitOK: Test Unit Ready sense: %02lx, Media %s\n", unit->au_UnitNum, sense[2], unit->au_Flags & AF_DiscPresent ? "PRESENT" : "ABSENT"));
    return sense[2];
}

static BYTE atapi_Read(struct ahci_Unit *unit, ULONG block, ULONG count,
    APTR buffer, ULONG *act)
{
    UBYTE cmd[] = {
       SCSI_READ10, 0, block>>24, block>>16, block>>8, block, 0, count>>8, count, 0
    };
    struct SCSICmd sc = {
       0
    };

    D(bug("[ATA%02ld] atapi_Read()\n", unit->au_UnitNum));

    sc.scsi_Command = (void*) &cmd;
    sc.scsi_CmdLength = sizeof(cmd);
    sc.scsi_Data = buffer;
    sc.scsi_Length = count << unit->au_SectorShift;
    sc.scsi_Flags = SCSIF_READ;

    return unit->au_DirectSCSI(unit, &sc);
}

static BYTE atapi_Write(struct ahci_Unit *unit, ULONG block, ULONG count,
    APTR buffer, ULONG *act)
{
    UBYTE cmd[] = {
       SCSI_WRITE10, 0, block>>24, block>>16, block>>8, block, 0, count>>8, count, 0
    };
    struct SCSICmd sc = {
       0
    };

    D(bug("[ATA%02ld] atapi_Write()\n", unit->au_UnitNum));

    sc.scsi_Command = (void*) &cmd;
    sc.scsi_CmdLength = sizeof(cmd);
    sc.scsi_Data = buffer;
    sc.scsi_Length = count << unit->au_SectorShift;
    sc.scsi_Flags = SCSIF_WRITE;

    return unit->au_DirectSCSI(unit, &sc);
}

static BYTE atapi_Eject(struct ahci_Unit *unit)
{
    struct atapi_StartStop cmd = {
        command: SCSI_STARTSTOP,
        immediate: 1,
        flags: ATAPI_SS_EJECT,
    };

    struct SCSICmd sc = {
       0
    };

    D(bug("[ATA%02ld] atapi_Eject()\n", unit->au_UnitNum));

    sc.scsi_Command = (void*) &cmd;
    sc.scsi_CmdLength = sizeof(cmd);
    sc.scsi_Flags = SCSIF_READ;

    return unit->au_DirectSCSI(unit, &sc);
}

ULONG atapi_RequestSense(struct ahci_Unit* unit, UBYTE* sense, ULONG senselen)
{
    UBYTE cmd[] = {
       3, 0, 0, 0, senselen & 0xfe, 0
    };
    struct SCSICmd sc = {
       0
    };

    D(bug("[ATA%02ld] atapi_RequestSense()\n", unit->au_UnitNum));

    if ((senselen == 0) || (sense == 0))
    {
       return 0;
    }
    sc.scsi_Data = (void*)sense;
    sc.scsi_Length = senselen & 0xfe;
    sc.scsi_Command = (void*)&cmd;
    sc.scsi_CmdLength = 6;
    sc.scsi_Flags = SCSIF_READ;

    unit->au_DirectSCSI(unit, &sc);

    DATAPI(dump(sense, senselen));
    DATAPI(bug("[SENSE] atapi_RequestSense: sensed data: %lx %lx %lx\n", sense[2]&0xf, sense[12], sense[13]));
    return ((sense[2]&0xf)<<16) | (sense[12]<<8) | (sense[13]);
}

ULONG ahci_ReadSignature(struct ahci_Port *port, int unit)
{
    ULONG port = port->ap_PortNum;
    UBYTE tmp1, tmp2;

    D(bug("[ATA  ] ahci_ReadSignature(%02ld)\n", unit));

    BUS_OUT(0xa0 | (unit << 4), ahci_DevHead, port);
    ahci_WaitNano(400);
    //ahci_WaitTO(port->ap_Timer, 0, 1, 0);

    DINIT(bug("[ATA  ] ahci_ReadSignature: Status %02lx Device %02lx\n",
    ahci_ReadStatus(port), BUS_IN(ahci_DevHead, port)));

    /* Ok, ATA/ATAPI device. Get detailed signature */
    DINIT(bug("[ATA  ] ahci_ReadSignature: ATA[PI] device present. Attempting to detect specific subtype\n"));

    tmp1 = BUS_IN(ahci_LBAMid, port);
    tmp2 = BUS_IN(ahci_LBAHigh, port);

    DINIT(bug("[ATA  ] ahci_ReadSignature: Subtype check returned %02lx:%02lx (%04lx)\n", tmp1, tmp2, (tmp1 << 8) | tmp2));

    switch ((tmp1 << 8) | tmp2)
    {
        case 0x14eb:
            DINIT(bug("[ATA  ] ahci_ReadSignature: Found signature for ATAPI device\n"));
            return DEV_ATAPI;

        case 0x3cc3:
            DINIT(bug("[ATA  ] ahci_ReadSignature: Found signature for SATA device\n"));
            return DEV_SATA;

        case 0x6996:
            DINIT(bug("[ATA  ] ahci_ReadSignature: Found signature for SATAPI device\n"));
            return DEV_SATAPI;

        default:
            if (0 == (ahci_ReadStatus(port) & 0xfe))
                return DEV_NONE;
            /* ATA_EXECUTE_DIAG is executed by both devices, do it only once */
            if (port->ap_Dev[0] == DEV_UNKNOWN || port->ap_Dev[0] >= DEV_ATAPI)
                BUS_OUT(ATA_EXECUTE_DIAG, ahci_Command, port);

            ahci_WaitTO(port->ap_Timer, 0, 2000, 0);
            while (ahci_ReadStatus(port) & ATAF_BUSY)
                ahci_WaitNano(400);
                //ahci_WaitTO(port->ap_Timer, 0, 1, 0);

            BUS_OUT(0xa0 | (unit << 4), ahci_DevHead, port);
            do
            {
                ahci_WaitNano(400);
                //ahci_WaitTO(unit->au_Port->ap_Timer, 0, 1, 0);
            }
            while (0 != (ATAF_BUSY & ahci_ReadStatus(port)));
            DINIT(bug("[ATA  ] ahci_ReadSignature: Further validating ATA signature: %lx & 0x7f = 1, %lx & 0x10 = unit\n", BUS_IN(ahci_Error, port), BUS_IN(ahci_DevHead, port)));

            if ((BUS_IN(ahci_Error, port) & 0x7f) == 1)
            {
                DINIT(bug("[ATA  ] ahci_ReadSignature: Found *valid* signature for ATA device\n"));
                /* this might still be an (S)ATAPI device, but we correct that in ahci_Identify */
                return DEV_ATA;
            }
            bug("[ATA  ] ahci_ReadSignature: Found signature for ATA "
                "device, but further validation failed\n");
            return DEV_NONE;
    }
}

void ahci_ResetBus(struct ahci_Port *port)
{
    ULONG alt = port->ap_Alt;
    ULONG port = port->ap_PortNum;
    ULONG TimeOut;

    /* Set and then reset the soft reset bit in the Device Control
     * register.  This causes device 0 be selected */
    DINIT(bug("[ATA  ] ahci_ResetBus(%d)\n", port->ap_BusNum));
    BUS_OUT(0xa0 | (0 << 4), ahci_DevHead, port);    /* Select it never the less */
    ahci_WaitNano(400);
    //ahci_WaitTO(port->ap_Timer, 0, 1, 0);

    BUS_OUT(0x04, ahci_AltControl, alt);
    ahci_WaitTO(port->ap_Timer, 0, 10, 0);    /* sleep 10us; min: 5us */
    BUS_OUT(0x02, ahci_AltControl, alt);
    ahci_WaitTO(port->ap_Timer, 0, 20000, 0); /* sleep 20ms; min: 2ms */

    /* If there is a device 0, wait for device 0 to clear BSY */
    if (DEV_NONE != port->ap_Dev[0]) {
        DINIT(bug("[ATA%02ld] ahci_ResetBus: Wait for Device to clear BSY\n",
            ((port->ap_BusNum << 1 ) + 0)));
        TimeOut = 1000;     /* Timeout 1s (1ms x 1000) */
        while ( 1 ) {
            if ((ahci_ReadStatus(port) & ATAF_BUSY) == 0)
                break;
            ahci_WaitTO(port->ap_Timer, 0, 1000, 0);
            if (!(--TimeOut)) {
                DINIT(bug("[ATA%02ld] ahci_ResetBus: Device Timed Out!\n",
                    ((port->ap_BusNum << 1 ) + 0)));
                port->ap_Dev[0] = DEV_NONE;
                break;
            }
        }
        DINIT(bug("[ATA%02ld] ahci_ResetBus: Wait left after %d ms\n",
            ((port->ap_BusNum << 1 ) + 0), (1000 - TimeOut)));
    }

    /* If there is a device 1, wait some time until device 1 allows
     * register access, but fail only if BSY isn't cleared */
    if (DEV_NONE != port->ap_Dev[1]) {
        DINIT(bug("[ATA  ] ahci_ResetBus: Wait DEV1 to allow access\n"));
        BUS_OUT(0xa0 | (1 << 4), ahci_DevHead, port);
        ahci_WaitNano(400);
        //ahci_WaitTO(port->ap_Timer, 0, 1, 0);

        TimeOut = 50;     /* Timeout 50ms (1ms x 50) */
        while ( 1 ) {
            if ( (BUS_IN(ahci_Count, port) == 0x01) && (BUS_IN(ahci_LBALow, port) == 0x01) )
                break;
            ahci_WaitTO(port->ap_Timer, 0, 1000, 0);
            if (!(--TimeOut)) {
                DINIT(bug("[ATA  ] ahci_ResetBus: DEV1 1/2 TimeOut!\n"));
                break;
            }
        }
        DINIT(bug("[ATA  ] ahci_ResetBus: DEV1 1/2 Wait left after %d ms\n",
            (1000 - TimeOut)));

        if (DEV_NONE != port->ap_Dev[1]) {
            DINIT(bug("[ATA%02ld] ahci_ResetBus: Wait for Device to clear BSY\n",
                ((port->ap_BusNum << 1 ) + 1)));
            TimeOut = 1000;     /* Timeout 1s (1ms x 1000) */
            while ( 1 ) {
                if ((ahci_ReadStatus(port) & ATAF_BUSY) == 0)
                    break;
                ahci_WaitTO(port->ap_Timer, 0, 1000, 0);
                if (!(--TimeOut)) {
                    DINIT(bug("[ATA%02ld] ahci_ResetBus: Device Timed Out!\n",
                        ((port->ap_BusNum << 1 ) + 1)));
                    port->ap_Dev[1] = DEV_NONE;
                    break;
                }
            }
            DINIT(bug("[ATA%02ld] ahci_ResetBus: Wait left after %d ms\n",
                ((port->ap_BusNum << 1 ) + 1), 1000 - TimeOut));
        }
    }

    if (DEV_NONE != port->ap_Dev[0])
        port->ap_Dev[0] = ahci_ReadSignature(bus, 0);
    if (DEV_NONE != port->ap_Dev[1])
        port->ap_Dev[1] = ahci_ReadSignature(bus, 1);
}

void ahci_InitBus(struct ahci_Port *port)
{
    ULONG port = port->ap_PortNum;
    UBYTE tmp1, tmp2;
    UWORD i;

    /*
     * initialize timer for the sake of scanning
     */
    port->ap_Timer = ahci_OpenTimer();

    DINIT(bug("[ATA  ] ahci_InitBus(%d)\n", port->ap_BusNum));

    port->ap_Dev[0] = DEV_NONE;
    port->ap_Dev[1] = DEV_NONE;

    for (i = 0; i < MAX_BUSUNITS; i++)
    {
        /* Select device and disable IRQs */
        BUS_OUT(0xa0 | (i << 4), ahci_DevHead, port);
        ahci_WaitTO(port->ap_Timer, 0, 100, 0);
        BUS_OUT(0x2, ahci_AltControl, port->ap_Alt);

        /* Write some pattern to registers */
        BUS_OUT(0x55, ahci_Count, port);
        BUS_OUT(0xaa, ahci_LBALow, port);
        BUS_OUT(0xaa, ahci_Count, port);
        BUS_OUT(0x55, ahci_LBALow, port);
        BUS_OUT(0x55, ahci_Count, port);
        BUS_OUT(0xaa, ahci_LBALow, port);

        tmp1 = BUS_IN(ahci_Count, port);
        tmp2 = BUS_IN(ahci_LBALow, port);

        if ((tmp1 == 0x55) && (tmp2 == 0xaa))
            port->ap_Dev[i] = DEV_UNKNOWN;
        DINIT(bug("[ATA%02ld] ahci_InitBus: Device type = %x\n",
            (port->ap_BusNum << 1 ) + i, port->ap_Dev[i]));
    }

    ahci_ResetBus(port);
    ahci_CloseTimer(port->ap_Timer);
    DINIT(bug("[ATA  ] ahci_InitBus: Finished\n"));
}

/*
 * not really sure what this is meant to be - TO BE REPLACED
 */
static const ULONG ErrorMap[] = {
    CDERR_NotSpecified,
    CDERR_NoSecHdr,
    CDERR_NoDisk,
    CDERR_NoSecHdr,
    CDERR_NoSecHdr,
    CDERR_NOCMD,
    CDERR_NoDisk,
    CDERR_WriteProt,
    CDERR_NotSpecified,
    CDERR_NotSpecified,
    CDERR_NotSpecified,
    CDERR_ABORTED,
    CDERR_NotSpecified,
    CDERR_NotSpecified,
    CDERR_NoSecHdr,
    CDERR_NotSpecified,
};

static BYTE atapi_EndCmd(struct ahci_Unit *unit)
{
    UBYTE status;

    DATAPI(bug("[ATA%02ld] atapi_EndCmd()\n", unit->au_UnitNum));

    /*
     * read alternate status register (per specs)
     */
    status = ATA_IN(ahci_AltStatus, unit->au_Port->ap_Alt);
    DATAPI(bug("[ATA%02ld] atapi_EndCmd: Alternate status: %lx\n", unit->au_UnitNum, status));

    status = ATA_IN(atapi_Status, unit->au_Port->ap_PortNum);

    DATAPI(bug("[ATA%02ld] atapi_EndCmd: Command complete. Status: %lx\n",
        unit->au_UnitNum, status));

    if (!(status & ATAPIF_CHECK))
    {
        return 0;
    }
    else
    {
       status = ATA_IN(atapi_Error, unit->au_Port->ap_PortNum);
       DATAPI(bug("[ATA%02ld] atapi_EndCmd: Error code 0x%lx\n", unit->au_UnitNum, status >> 4));
       return ErrorMap[status >> 4];
    }
}

/*
 * vim: ts=4 et sw=4 fdm=marker fmr={,}
 */
