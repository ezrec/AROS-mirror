/*
    Copyright © 2004-2011, The AROS Development Team. All rights reserved
    $Id$

    Desc:
    Lang: English
*/

/* Maintainer: Jason S. McMullan <jason.mcmullan@gmail.com>
 */

#include <aros/debug.h>
#include <aros/symbolsets.h>
#include <exec/exec.h>
#include <exec/resident.h>
#include <exec/tasks.h>
#include <exec/memory.h>
#include <exec/nodes.h>
#include <utility/utility.h>
#include <libraries/expansion.h>
#include <libraries/configvars.h>
#include <devices/trackdisk.h>
#include <dos/bptr.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>

#include <proto/exec.h>
#include <proto/timer.h>
#include <proto/bootloader.h>
#include <proto/expansion.h>

#include <string.h>

#include "ahci_intern.h"
#include "timer.h"

#include LC_LIBDEFS_FILE

/* Add a bootnode using expansion.library */
BOOL ahci_RegisterVolume(ULONG StartCyl, ULONG EndCyl, struct ahci_Unit *unit)
{
    struct ExpansionBase *ExpansionBase;
    struct DeviceNode *devnode;
    TEXT dosdevname[4] = "HA0";
    const ULONG DOS_ID = AROS_MAKE_ID('D','O','S','\001');
    const ULONG CDROM_ID = AROS_MAKE_ID('C','D','V','D');

    ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library",
                                                        40L);

    if (ExpansionBase)
    {
        IPTR pp[4 + DE_BOOTBLOCKS + 1];

        /* This should be dealt with using some sort of volume manager or such. */
        switch (unit->au_DevType)
        {
            case DG_DIRECT_ACCESS:
                break;
            case DG_CDROM:
                dosdevname[0] = 'C';
                break;
            default:
                D(bug("[AHCI>>]:-ahci_RegisterVolume called on unknown devicetype\n"));
        }

        if (unit->au_UnitNum < 10)
            dosdevname[2] += unit->au_UnitNum % 10;
        else
            dosdevname[2] = 'A' - 10 + unit->au_UnitNum;
    
        pp[0] 		    = (IPTR)dosdevname;
        pp[1]		    = (IPTR)MOD_NAME_STRING;
        pp[2]		    = unit->au_UnitNum;
        pp[DE_TABLESIZE    + 4] = DE_BOOTBLOCKS;
        pp[DE_SIZEBLOCK    + 4] = 1 << (unit->au_SectorShift - 2);
        pp[DE_NUMHEADS     + 4] = unit->au_Heads;
        pp[DE_SECSPERBLOCK + 4] = 1;
        pp[DE_BLKSPERTRACK + 4] = unit->au_Sectors;
        pp[DE_RESERVEDBLKS + 4] = 2;
        pp[DE_LOWCYL       + 4] = StartCyl;
        pp[DE_HIGHCYL      + 4] = EndCyl;
        pp[DE_NUMBUFFERS   + 4] = 10;
        pp[DE_BUFMEMTYPE   + 4] = MEMF_PUBLIC;
        pp[DE_MAXTRANSFER  + 4] = 0x00200000;
        pp[DE_MASK         + 4] = ~3;
        pp[DE_BOOTPRI      + 4] = (unit->au_DevType == DG_DIRECT_ACCESS) ? 0 : 10;
        pp[DE_DOSTYPE      + 4] = (unit->au_DevType == DG_DIRECT_ACCESS) ? DOS_ID : CDROM_ID;
        pp[DE_BOOTBLOCKS   + 4] = 2;
    
        devnode = MakeDosNode(pp);

        if (devnode) {
            D(bug("[AHCI>>]:-ahci_RegisterVolume: '%s' with StartCyl=%d, EndCyl=%d .. ",
                        AROS_DOSDEVNAME(devnode), StartCyl, EndCyl));
            AddBootNode(pp[DE_BOOTPRI + 4], 0, devnode, 0);
            D(bug("done\n"));
            return TRUE;
        }

        CloseLibrary((struct Library *)ExpansionBase);
    }

    return FALSE;
}

/*
 * This routine needs to be called by bus probe code in order to register a device.
 */
void ahci_RegisterHost(struct ahci_Host *ah)
{
    struct ahci_Port *ap;
    int i, port;
    ULONG pi;

    /*
     * initialize structure
     */
    ah->ah_HostNum = ah->ah_Base->ahci_HostCount++;

    pi = mmio_inl_le(&ah->ah_MMIO->pi);

    for (port = i = 0; port < 32 && i < ah->ah_Ports; port++) {
        if (!((1 << port) & pi))
            continue;

        ap = &ah->ah_Port[i++];

        ap->ap_Base         = ah->ah_Base;
        ap->ap_Host         = ah;
        ap->ap_MMIO         = ((void *)ah->ah_MMIO) + 0x100 + (0x80 * port);
        ap->ap_Dev          = DEV_NONE;
        ap->ap_Flags        = 0;
        ap->ap_SleepySignal = 0;
        ap->ap_Timeout      = 0;
        ap->ap_Task         = NULL;
        ap->ap_HandleIRQ    = NULL;
        ap->ap_PortNum      = i;

        /* Set up base (non-operating) parameters */
        ahci_init_unit(ap,ah->ah_Base->ahci_PortCount++);
    }

    D(bug("[AHCI>>] ahci_RegisterHost: Analysing host %d, units %d..%d\n",
                ah->ah_HostNum,
                ah->ah_Port[0].ap_Unit.au_UnitNum, 
                ah->ah_Port[ah->ah_HostNum->ah_Ports-1].ap_Unit.au_UnitNum));

    D(bug("[AHCI>>] ahci_RegisterHost: IRQ %d, MMIO %p\n", ah->ah_IRQ, ah->ah_MMIO));

    /*
     * scan bus - try to locate all devices (no irq)
     */
    ahci_InitHost(ah);

    /*
     * start things up :)
     * note: this happens no matter there are devices or not 
     * sort of almost-ready-for-hotplug ;)
     */
    AddTail((struct List*)&ah->ah_Base->ahci_Hosts, (struct Node*)ah);

    for (i = 0; i < ah->ah_Ports; i++) {
        AddTail((struct List*)&ah->ah_Base->ahci_Ports, (struct Node*)&ah->ah_Port[i]);
    }
}

#if 0
/*
 * This init routine has +127 priority, so it runs after all
 * bus scanners have done their job.
 * It initializes all discovered units.
 */
static int ahci_scan(struct AHCIBase *base)
{
    struct SignalSemaphore ssem;
    struct ahci_Port* node;
    struct Task *parent = FindTask(NULL);

    D(bug("[AHCI--] ahci_scan: Initialising Bus Tasks..\n"));
    InitSemaphore(&ssem);
    ForeachNode(&base->ahci_Ports, node)
    {
    	NewCreateTask(TASKTAG_PC	 , BusTaskCode,
    		      TASKTAG_NAME	 , "AHCI Subsystem",
    		      TASKTAG_STACKSIZE  , AROS_STACK_SIZE,
    		      TASKTAG_PRI	 , 10,
    		      TASKTAG_TASKMSGPORT, &node->ab_MsgPort,
    		      TASKTAG_ARG1 	 , node,
        	      TASKTAG_ARG2	 , parent,
        	      TASKTAG_ARG3	 , &ssem,
        	      TAG_DONE);

	/* Initial handshake */
        Wait(SIGBREAKF_CTRL_C);
    }

    /*
     * wait for all buses to complete their init
     */
    D(bug("[AHCI--] ahci_scan: Waiting for Buses to finish Initialising\n"));
    ObtainSemaphore(&ssem);

    /*
     * and leave.
     */
    ReleaseSemaphore(&ssem);
    D(bug("[AHCI--] ahci_scan: Finished\n"));

    /* Try to setup daemon task looking for diskchanges */
    NewCreateTask(TASKTAG_PC       , DaemonCode,
                  TASKTAG_NAME     , "AHCI.daemon",
                  TASKTAG_STACKSIZE, AROS_STACK_SIZE,
                  TASKTAG_PRI      , TASK_PRI - 1,	/* The daemon should have a little bit lower Pri as handler tasks */
                  TASKTAG_ARG1     , base,
                  TAG_DONE);

    return TRUE;
}
ADD2INITLIB(ahci_scan, 127)
#endif

/*
    Here shall we start. Make function static as it shouldn't be visible from
    outside.
*/
static int ahci_init(LIBBASETYPEPTR LIBBASE)
{
    D(bug("[AHCI--] ahci_init: ahci.device Initialization\n"));

    /*
     * I've decided to use memory pools again. Alloc everything needed from 
     * a pool, so that we avoid memory fragmentation.
     */
    LIBBASE->ahci_MemPool = CreatePool(MEMF_CLEAR | MEMF_PUBLIC | MEMF_SEM_PROTECTED , 8192, 4096);
    if (LIBBASE->ahci_MemPool == NULL)
        return FALSE;

    D(bug("[AHCI--] ahci_init: MemPool @ %p\n", LIBBASE->ahci_MemPool));

    /* Initialize lists */
    NEWLIST(&LIBBASE->ahci_Hosts);
    NEWLIST(&LIBBASE->ahci_Ports);

    return TRUE;
}

static int open
(
    LIBBASETYPEPTR LIBBASE,
    struct IORequest *iorq,
    ULONG unitnum,
    ULONG flags
)
{
    /* 
     * Assume it failed 
     */
    iorq->io_Error = IOERR_OPENFAIL;

    /*
     * actual port
     */
    struct ahci_Port *p = (struct ahci_Port *)GetHead(&LIBBASE->ahci_Ports);

    /*
     * locate bus
     */
    for (; p && p->ap_Unit.au_UnitNum != unitnum; p = (struct ahci_Port *)GetSucc(p));

    if (!p)
        return FALSE;

    /*
     * set up iorequest
     */
    iorq->io_Device     = &LIBBASE->ahci_Device;
    iorq->io_Unit       = &p->ap_Unit.au_Unit;
    iorq->io_Error      = 0;

    p->ap_Unit.au_Unit.unit_OpenCnt++;

    return TRUE;
}

/* Close given device */
static int close
(
    LIBBASETYPEPTR LIBBASE,
    struct IORequest *iorq
)
{
    struct ahci_Unit *unit = (struct ahci_Unit *)iorq->io_Unit;

    /* First of all make the important fields of struct IORequest invalid! */
    iorq->io_Unit = (struct Unit *)~0;
    
    /* Decrease use counters of unit */
    unit->au_Unit.unit_OpenCnt--;

    return TRUE;
}

ADD2INITLIB(ahci_init, 0)
ADD2OPENDEV(open, 0)
ADD2CLOSEDEV(close, 0)
/* vim: set ts=8 sts=4 et : */
