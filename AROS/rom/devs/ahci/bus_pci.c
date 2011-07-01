/*
    Copyright © 2004-2011, The AROS Development Team. All rights reserved.
    $Id: lowlevel.c 34191 2010-08-17 16:19:51Z neil $

    Desc: PCI bus driver for ata.device
    Lang: English
*/

#define DEBUG 1

#include <aros/asmcall.h>
#include <aros/debug.h>
#include <aros/symbolsets.h>
#include <asm/io.h>
#include <exec/lists.h>
#include <hardware/ahci.h>
#include <hidd/irq.h>
#include <hidd/pci.h>
#include <oop/oop.h>
#include <proto/exec.h>
#include <proto/oop.h>

#include <string.h>

#include "ahci_intern.h"
#include "pci.h"

typedef struct 
{
    struct AHCIBase *AHCIBase;
    struct List	    hosts;
    OOP_AttrBase    HiddPCIDeviceAttrBase;
    OOP_MethodID    HiddPCIDriverMethodBase;
} EnumeratorArgs;

#define RANGESIZE0 8
#define RANGESIZE1 4
#define DMASIZE 16

#if 0
static void ahci_Interrupt(HIDDT_IRQ_Handler *irq, HIDDT_IRQ_HwInfo *hw)
{
    /*
     * Our interrupt handler should call this function.
     * It's our problem how to store bus pointer. Here we use h_Data for it.
     */
    ahci_HandleIRQ(irq->h_Data);
}

/* Actually a quick hack. Proper implementation really needs HIDDizing this code. */
static APTR CreateInterrupt(struct ahci_Host *host)
{
    HIDDT_IRQ_Handler *IntHandler = AllocPooled(host->AHCIBase->ahci_MemPool, sizeof(HIDDT_IRQ_Handler));

    if (IntHandler)
    {
        OOP_Object *o;

        /*
            Prepare nice interrupt for our bus. Even if interrupt sharing is enabled,
            it should work quite well
        */
        IntHandler->h_Node.ln_Pri = 10;
        IntHandler->h_Node.ln_Name = "AHCI IRQ";
        IntHandler->h_Code = ahci_Interrupt;
        IntHandler->h_Data = host;

        o = OOP_NewObject(NULL, CLID_Hidd_IRQ, NULL);
        if (o)
        {
            struct pHidd_IRQ_AddHandler msg =
            {
                mID:            OOP_GetMethodID(IID_Hidd_IRQ, moHidd_IRQ_AddHandler),
                handlerinfo:    IntHandler,
                id:             host->ah_IRQ,
            };
            int retval = OOP_DoMethod(o, &msg.mID);

            OOP_DisposeObject(o);
            
            if (retval)
            	return IntHandler;
        }
    }

    FreeMem(IntHandler, sizeof(HIDDT_IRQ_Handler));
    return NULL;
}
#endif

/*
 * PCI BUS ENUMERATOR
 *   collect all SATA devices and spawn concurrent tasks.
 */

static
AROS_UFH3(void, ahci_PCIEnumerator_h,
    AROS_UFHA(struct Hook *,    hook,   A0),
    AROS_UFHA(OOP_Object *,     Device, A2),
    AROS_UFHA(APTR,             message,A1))
{
    AROS_USERFUNC_INIT

    struct ahci_Host *host;
    APTR hba_phys = NULL;
    IPTR hba_size = 0;
    OOP_Object *Driver = NULL;
    volatile struct ahci_hwhba *hwhba;
    struct pHidd_PCIDriver_MapPCI map;
    struct pHidd_PCIDriver_UnmapPCI unmap;
    ULONG ghc, cap;
    int nr_ports;
    EnumeratorArgs *a = hook->h_Data;
    struct AHCIBase *AHCIBase = a->AHCIBase;

    /*
     * parameters we will want to acquire
     */
    IPTR	ProductID = 0,
		VendorID = 0,
                INTLine = 0;

    /*
     * the PCI Attr Base
     */
    OOP_AttrBase HiddPCIDeviceAttrBase = a->HiddPCIDeviceAttrBase;

    /*
     * obtain more or less useful data
     */
    OOP_GetAttr(Device, aHidd_PCIDevice_VendorID,           &VendorID);
    OOP_GetAttr(Device, aHidd_PCIDevice_ProductID,          &ProductID);

    D(bug("[PCI-AHCI] ahci_PCIEnumerator_h: Found device %04x:%04x\n", VendorID, ProductID));

    /*
     * SATA controllers may need a special treatment before becoming usable.
     * The machine's firmware may operate them in Legacy mode.
     * In this case we ignore them, and let ata.device handle them.
     * 
     * CHECKME: In order to work on PPC it uses explicit little-endian I/O,
     * assuning AHCI register file is always little-endian. Is it correct ?
     */
    OOP_GetAttr(Device, aHidd_PCIDevice_Base5, (IPTR *)&hba_phys);
    OOP_GetAttr(Device, aHidd_PCIDevice_Size5, &hba_size);

    D(bug("[PCI-AHCI] Device %04x:%04x is a SATA device, HBA 0x%p, size 0x%p\n", VendorID, ProductID, hba_phys, hba_size));

    OOP_GetAttr(Device, aHidd_PCIDevice_Driver, (IPTR *)&Driver);

    /*
     * Obtain PCIDriver method base (lazy).
     * Methods are numbered subsequently, the same as attributes. This means
     * we can use the same mechanism for them (get base value and add offsets).
     */
    if (!a->HiddPCIDriverMethodBase)
    	a->HiddPCIDriverMethodBase = OOP_GetMethodID(IID_Hidd_PCIDriver, 0);

    map.mID        = a->HiddPCIDriverMethodBase + moHidd_PCIDriver_MapPCI;
    map.PCIAddress = hba_phys;
    map.Length     = hba_size;

    hwhba = (struct ahci_hwhba *)OOP_DoMethod(Driver, (OOP_Msg)&map);
    D(bug("[PCI-AHCI] Mapped at 0x%p\n", hwhba));

    if (!hwhba)
    {
    	D(bug("[PCI-AHCI] Mapping failed, device will be ignored\n"));

    	return;
    }

    cap = mmio_inl_le(&hwhba->cap);
    ghc = mmio_inl_le(&hwhba->ghc);
    D(bug("[PCI-AHCI] Capabilities: 0x%08X, host control: 0x%08X\n", cap, ghc));

    nr_ports = cap & CAP_NP_MASK;

    if (!(ghc & GHC_AE) || (nr_ports == 0))
    {
    	D(bug("[PCI-AHCI] AHCI not enabled, will be handled by ata.device\n"));

    	goto failure;
    }

    host = AllocPooled(AHCIBase->ahci_MemPool, (sizeof(*host) + sizeof(host->ah_Port[0]) * (nr_ports)));
    if (!host)
    	goto failure;

    OOP_GetAttr(Device, aHidd_PCIDevice_INTLine, &INTLine);

    host->ah_Ports = nr_ports;
    host->ah_MMIO  = hwhba;
    host->ah_IRQ   = INTLine;

    AddTail(&a->hosts, (struct Node *)host);

    OOP_SetAttrsTags(Device, aHidd_PCIDevice_isMEM, TRUE,
                     aHidd_PCIDevice_isMaster, TRUE,
                     TAG_DONE);

    return;

failure:
    unmap.mID        = a->HiddPCIDriverMethodBase + moHidd_PCIDriver_UnmapPCI;
    unmap.CPUAddress = (APTR)hwhba;
    unmap.Length     = hba_size;

    OOP_DoMethod(Driver, &unmap.mID);

    AROS_USERFUNC_EXIT
}

static const struct TagItem Requirements[] =
{
    {tHidd_PCI_Class,     PCI_CLASS_MASSSTORAGE},
    {tHidd_PCI_SubClass,  PCI_SUBCLASS_SATA},
    {tHidd_PCI_Interface, 1},
    {TAG_DONE }
};

static int ahci_pci_scan(struct AHCIBase *base)
{
    OOP_Object *pci;
    EnumeratorArgs Args;
    struct ahci_Host *host;

    D(bug("[PCI-AHCI] ahci_scan: Enumerating devices\n"));

    Args.AHCIBase                 = base;
    NEWLIST(&Args.hosts);
    Args.HiddPCIDriverMethodBase = 0;
    Args.HiddPCIDeviceAttrBase   = OOP_ObtainAttrBase(IID_Hidd_PCIDevice);

    if (Args.HiddPCIDeviceAttrBase)
    {
	pci = OOP_NewObject(NULL, CLID_Hidd_PCI, NULL);

	if (pci)
	{
	    struct Hook FindHook =
	    {
		h_Entry:    (IPTR (*)())ahci_PCIEnumerator_h,
		h_Data:     &Args
	    };

	    struct pHidd_PCI_EnumDevices enummsg =
	    {
		mID:            OOP_GetMethodID(IID_Hidd_PCI, moHidd_PCI_EnumDevices),
		callback:       &FindHook,
		requirements:   Requirements,
	    };

	    OOP_DoMethod(pci, &enummsg.mID);
	    OOP_DisposeObject(pci);
	}

	OOP_ReleaseAttrBase(IID_Hidd_PCIDevice);
    }

    D(bug("[PCI-AHCI] ahci_scan: Registering Probed Hosts..\n"));

    while ((host = (struct ahci_Host *)RemHead(&Args.hosts)) != NULL) {
    	ahci_RegisterHost(host);
    }

    return TRUE;
}

/*
 * ahci.device main code has two init routines with 0 and 127 priorities.
 * All bus scanners must run between them.
 */
ADD2INITLIB(ahci_pci_scan, 30)
ADD2LIBS("irq.hidd", 0, static struct Library *, __irqhidd)
