#ifndef _PCI_H
#define _PCI_H

/*
    Copyright © 2004-2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/semaphores.h>
#include <dos/bptr.h>

#include <aros/libcall.h>
#include <aros/asmcall.h>

#include <oop/oop.h>

#include <aros/arossupportbase.h>
#include <exec/execbase.h>

#include LC_LIBDEFS_FILE

/* Private data and structures unavailable outside the pci base classes */

struct DriverNode {
    struct Node		node;
    OOP_Class		*driverClass;	/* Driver class */
    OOP_Object		*driverObject;	/* Driver object */
    ULONG		highBus;
    struct List		devices;	/* List of defices behind this node */
};

struct DrvInstData {
    BOOL DirectBus;
};

struct PciDevice {
    struct MinNode	node;
    OOP_Object		*device;
};

typedef struct DeviceData {
    OOP_Object		*driver;
    UBYTE		bus,dev,sub;
    UBYTE		isBridge;
    UBYTE		subbus;
    UWORD		VendorID;
    UWORD		ProductID;
    UBYTE		RevisionID;
    UBYTE		Interface;
    UBYTE		SubClass;
    UBYTE		Class;
    UWORD		SubsysVID;
    UWORD		SubsystemID;
    UBYTE		INTLine;
    UBYTE		IRQLine;
    UBYTE		HeaderType;
    struct {
	IPTR		addr;
	IPTR		size;
    } BaseReg[6];
    ULONG		RomBase;
    ULONG		RomSize;

    STRPTR		strClass;
    STRPTR		strSubClass;
    STRPTR		strInterface;
} tDeviceData;

struct pci_staticdata {
    struct SignalSemaphore driver_lock;
    struct List		drivers;

    APTR		MemPool;
    
    OOP_AttrBase	hiddAB;
    OOP_AttrBase	hiddPCIAB;
    OOP_AttrBase	hiddPCIDriverAB;
    OOP_AttrBase	hiddPCIBusAB;
    OOP_AttrBase	hiddPCIDeviceAB;

    OOP_Class		*pciClass;
    OOP_Class		*pciDeviceClass;
    OOP_Class		*pciDriverClass;

    ULONG		users;

    /* Most commonly used methods have already the mID's stored here */
    OOP_MethodID	mid_RB;
    OOP_MethodID	mid_RW;
    OOP_MethodID	mid_RL;
    OOP_MethodID	mid_WB;
    OOP_MethodID	mid_WW;
    OOP_MethodID	mid_WL;
};

struct pcibase {
    struct Library 		LibNode;
    APTR			MemPool;
    struct pci_staticdata	psd;
};

OOP_Class *init_pcideviceclass(struct pci_staticdata *);
void free_pcideviceclass(struct pci_staticdata *, OOP_Class *cl);

#define BASE(lib) ((struct pcibase*)(lib))

#define PSD(cl) (&BASE(cl->UserData)->psd)

#include "pci_registers.h"

#endif /* _PCI_H */

