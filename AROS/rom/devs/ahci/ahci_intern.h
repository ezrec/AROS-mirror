/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef AHCI_INTERN_H
#define AHCI_INTERN_H

#include <exec/types.h>
#include <asm/io.h>

#include <devices/scsidisk.h>
#include <exec/devices.h>

#include <hardware/ahci.h>

#define Unit(io) ((struct ahci_Unit *)(io)->io_Unit)
#define IOStdReq(io) ((struct IOStdReq *)io)

/* Device types */
#define DEV_NONE        0x00
#define DEV_UNKNOWN     0x01
#define DEV_ATA         0x02
#define DEV_SATA        0x03
#define DEV_ATAPI       0x80
#define DEV_SATAPI      0x81
#define DEV_BROKEN      0xff


/* ahci.device base */
struct AHCIBase
{
   /*
    * Device structure - used to manage devices by Exec
    */
   struct Device           ahci_Device;

   /*
    * master task pointer
    */
   struct Task            *ahci_Daemon;

   /*
    * list of all hosts
    */
   struct MinList          ahci_Hosts;
   ULONG                   ahci_HostCount;

   /*
    * list of all ports
    */
   struct MinList          ahci_Ports;
   ULONG                   ahci_PortCount;

   /*
    * Arguments and flags
    */

   /*
    * memory pool
    */
   APTR                    ahci_MemPool;
};

/*
   Unit structure describing given device on the bus. It contains all the
   necessary information unit/device may need.
   */
struct ahci_Unit
{
   struct Unit         au_Unit;   /* exec's unit */
   struct DriveIdent  *au_Drive;  /* Drive Ident after IDENTIFY command */
   struct ahci_Port   *au_Port;   /* Port on which this unit is */

   ULONG               au_Capacity;    /* Highest sector accessible through LBA28 */
   UQUAD               au_Capacity48;  /* Highest sector accessible through LBA48 */
   ULONG               au_Cylinders;
   UBYTE               au_Heads;
   UBYTE               au_Sectors;
   UBYTE               au_Model[41];
   UBYTE               au_FirmwareRev[9];
   UBYTE               au_SerialNumber[21];

   /*
      Here are stored pointers to functions responsible for handling this
      device. They are set during device initialization and point to most
      effective functions for this particular unit.
      */
   BYTE        (*au_Read32)(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
   BYTE        (*au_Write32)(struct ahci_Unit *, ULONG, ULONG, APTR, ULONG *);
   BYTE        (*au_Read64)(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
   BYTE        (*au_Write64)(struct ahci_Unit *, UQUAD, ULONG, APTR, ULONG *);
   BYTE        (*au_Eject)(struct ahci_Unit *);
   BYTE        (*au_DirectSCSI)(struct ahci_Unit *, struct SCSICmd*);
   BYTE        (*au_Identify)(struct ahci_Unit *);

   ULONG               au_UnitNum;     /* Unit number as coded by device */
   ULONG               au_Flags;       /* Unit flags, see below */
   ULONG               au_ChangeNum;   /* Number of disc changes */

   struct Interrupt   *au_RemoveInt;  /* Raise this interrupt on a disc change */
   struct List         au_SoftList;    /* Raise even more interrupts from this list on disc change */

   UBYTE               au_SectorShift;         /* Sector shift. 9 here is 512 bytes sector */
   UBYTE               au_DevMask;             /* device mask used to simplify device number coding */
   UBYTE               au_SenseKey;            /* Sense key from ATAPI devices */
   UBYTE               au_DevType;
};


/*
 * The single AHCI port (channel)
 */
struct ahci_Port
{
   struct MinNode          ap_Node;    /* exec node */
   struct AHCIBase         *ap_Base;   /* device self */
   struct ahci_Host        *ap_Host;   /* Host device */
   volatile struct ahci_hwport *ap_MMIO;  /* MMIO for port used */
   UBYTE                   ap_Dev;     /* Master/Slave type, see below */
   UBYTE                   ap_Flags;   /* Bus flags similar to unit flags */
   BYTE                    ap_SleepySignal; /* Signal used to wake the task up, when it's waiting */
   /* for data requests/DMA */
   UBYTE                   ap_PortNum;  /* bus id - used to calculate device id */
   LONG                    ap_Timeout; /* in seconds; please note that resolution is low (1sec) */

   struct ahci_Unit         ap_Unit;  /* Only one unit per port */

   APTR			   ap_IntHandler;
   ULONG                   ap_IntCnt;

   struct Task             *ap_Task;       /* Bus task handling all not-immediate transactions */
   struct MsgPort          *ap_MsgPort;    /* Task's message port */
   struct PRDEntry         *ap_PRD;
   struct IORequest	   *ap_Timer;	   /* timer stuff */

   /* functions go here */
   void                   (*ap_HandleIRQ)(struct ahci_Unit* unit, UBYTE status);
   
   /* Bookkeeping pointer, for later freeing */
   APTR                     ap_RAWMemory;
   APTR                     ap_DMAMemory;
};

/* A collection of ports, driven by a single IRQ
 */
struct ahci_Host {
    struct AHCIBase *ah_Base;
    volatile struct ahci_hwhba *ah_MMIO;
    IPTR	ah_IRQ;
    ULONG	ah_MemFlags;
    ULONG	ah_HostNum;
    int		ah_Ports;
    struct ahci_Port ah_Port[];
};

BOOL ahci_InitHost(struct ahci_Host *ah);
BOOL ahci_init_unit(struct ahci_Port *ap, ULONG unitnum);
BOOL ahci_setup_unit(struct ahci_Port *ap);
BOOL ahci_RegisterVolume(ULONG StartCyl, ULONG EndCyl, struct ahci_Unit *unit);
void ahci_RegisterHost(struct ahci_Host *ah);

#endif /* AHCI_INTERN_H */
