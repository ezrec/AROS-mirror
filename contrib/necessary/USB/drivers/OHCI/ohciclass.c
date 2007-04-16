/*
    Copyright (C) 2006 by Michal Schulz
    $Id$

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License as 
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this program; if not, write to the
    Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define DEBUG 1

#include <inttypes.h>

#include <exec/types.h>
#include <exec/ports.h>
#include <oop/oop.h>
#include <usb/usb.h>
#include <utility/tagitem.h>
#include <aros/debug.h>
#include <aros/symbolsets.h>

#include <devices/timer.h>

#include <hidd/hidd.h>
#include <hidd/pci.h>
#include <hidd/irq.h>

#include <proto/oop.h>
#include <proto/utility.h>

#include "ohci.h"

static const usb_hub_descriptor_t hub_descriptor = {
    bDescLength:        sizeof(usb_hub_descriptor_t) - 31,
    bDescriptorType:    UDESC_HUB,
    bNbrPorts:          2,
    wHubCharacteristics:AROS_WORD2LE(UHD_PWR_NO_SWITCH | UHD_OC_INDIVIDUAL),
    bPwrOn2PwrGood:     50,
    bHubContrCurrent:   0,
    DeviceRemovable:    {0,},
};

OOP_Object *METHOD(OHCI, Root, New)
{
    int success = 0;
    D(bug("[OHCI] OHCI::New()\n"));

    BASE(cl->UserData)->LibNode.lib_OpenCnt++;
    
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);
    if (o)
    {
        OHCIData *ohci = OOP_INST_DATA(cl, o);
        NEWLIST(&ohci->intList);
        
        NEWLIST(&ohci->timerPort.mp_MsgList);
        ohci->timerPort.mp_Flags = PA_SOFTINT;
        ohci->timerPort.mp_Node.ln_Type = NT_MSGPORT;
        ohci->timerPort.mp_SigTask = &ohci->timerInt;
        ohci->timerInt.is_Code = OHCI_HubInterrupt;
        ohci->timerInt.is_Data = ohci;
        
        ohci->timerReq = CreateIORequest(&ohci->timerPort, sizeof(struct timerequest));
        OpenDevice((STRPTR)"timer.device", UNIT_VBLANK, (struct IORequest *)ohci->timerReq, 0);
        
        CopyMem(&hub_descriptor, &ohci->hubDescr, sizeof(usb_hub_descriptor_t));
        
        ohci->hubDescr.bNbrPorts = GetTagData(aHidd_USBHub_NumPorts, 0, msg->attrList);
        ohci->regs = (ohci_registers_t *)GetTagData(aHidd_OHCI_MemBase, 0, msg->attrList);
        ohci->pciDriver = (OOP_Object *)GetTagData(aHidd_OHCI_PCIDriver, 0, msg->attrList);
        ohci->pciDevice = (OOP_Object *)GetTagData(aHidd_OHCI_PCIDevice, 0, msg->attrList);
        ohci->hcca = HIDD_PCIDriver_AllocPCIMem(ohci->pciDriver, 4096);

        D(bug("[OHCI] New(): o=%p, ports=%d, regs=%p, drv=%p, dev=%p, hcca=%p\n", o,
              ohci->hubDescr.bNbrPorts, ohci->regs, ohci->pciDriver, ohci->pciDevice,
              ohci->hcca));
        
        ohci->sd = SD(cl);
        
        if (ohci->tmp)
            AddTail(&ohci->intList, &ohci->tmp->is_Node);
        
        success = TRUE;
    }
    
    if (!success)
    {
        OOP_MethodID mID = OOP_GetMethodID((STRPTR)IID_Root, moRoot_Dispose);
        OOP_CoerceMethod(cl, o, (OOP_Msg)&mID);
        o = NULL;
    }   
    
    D(bug("[OHCI] OHCI::New() = %p\n",o));

    if (!o)
        BASE(cl->UserData)->LibNode.lib_OpenCnt--;

    return o;
}

struct pRoot_Dispose {
    OOP_MethodID        mID;
};

void METHOD(OHCI, Root, Dispose)
{
    OHCIData *ohci = OOP_INST_DATA(cl, o);
    struct Library *base = &BASE(cl->UserData)->LibNode;

    D(bug("[OHCI] OHCI::Dispose\n"));
    
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    
    base->lib_OpenCnt--;
}

void METHOD(OHCI, Root, Get)
{
    uint32_t idx;

    if (IS_USBDEVICE_ATTR(msg->attrID, idx))
    {
        switch (idx)
        {
            case aoHidd_USBDevice_Address:
                *msg->storage = 1;
                break;
            case aoHidd_USBDevice_Hub:
                *msg->storage = 0;
                break;
            case aoHidd_USBDevice_Bus:
                *msg->storage = (intptr_t)o;
                break;
            default:
                OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
        }
    }
    else
        OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

BOOL METHOD(OHCI, Hidd_USBDrv, AddInterrupt)
{
    BOOL retval = FALSE;
    
    if (msg->pipe == (void *)0xdeadbeef)
    {
        OHCIData *ohci = OOP_INST_DATA(cl, o);
        D(bug("[OHCI] AddInterrupt() local for the OHCI. Intr %p, list %p\n", msg->interrupt, &ohci->intList));
        
        if (!ohci->regs)
            ohci->tmp = msg->interrupt;
        else
            AddTail(&ohci->intList, &msg->interrupt->is_Node);
        
        retval = TRUE;    
    }
    else
    {
        D(bug("[OHCI] AddInterrupt()\n"));
#warning TODO: Complete
    }
    D(bug("[OHCI::AddInterrupt] %s\n", retval ? "success":"failure"));
    
    //for(;;);
    
    return retval;
}

BOOL METHOD(OHCI, Hidd_USBDrv, RemInterrupt)
{
    if (msg->pipe == (void *)0xdeadbeef)
    {
        Remove(msg->interrupt);
        return TRUE;
    }
    else
    {
#warning TODO:
        return FALSE;
    }
}


/* Class initialization and destruction */

#undef SD
#define SD(x) (&LIBBASE->sd)

static int OHCI_InitClass(LIBBASETYPEPTR LIBBASE)
{
    int i;
    D(bug("[OHCI] InitClass\n"));

    HiddOHCIAttrBase = OOP_ObtainAttrBase((STRPTR)IID_Drv_USB_OHCI);
    LIBBASE->sd.irq = OOP_NewObject(NULL, (STRPTR)CLID_Hidd_IRQ, NULL);
    
    if (HiddOHCIAttrBase)
    {
        struct TagItem tags[] = {
                { aHidd_OHCI_MemBase,           0UL },
                { aHidd_OHCI_PCIDevice,         0UL },
                { aHidd_OHCI_PCIDriver,         0UL },
                { aHidd_USBHub_NumPorts,        0UL },
                { aHidd_USBDevice_Address,      1UL },
                { aHidd_USBHub_IsRoot,          1UL },
                { TAG_DONE, 0UL },
        };

        for (i=0; i < LIBBASE->sd.numDevices; i++)
        {
            tags[0].ti_Data = LIBBASE->sd.ramBase[i];
            tags[1].ti_Data = (intptr_t)LIBBASE->sd.pciDevice[i];
            tags[2].ti_Data = (intptr_t)LIBBASE->sd.pciDriver[i];
            tags[3].ti_Data = (intptr_t)LIBBASE->sd.numPorts[i];
            
            D(bug("[OHCI] Initializing driver object: dev=%p, drv=%p, %d ports @ %p\n",
                  LIBBASE->sd.pciDevice[i], LIBBASE->sd.pciDriver[i], LIBBASE->sd.numPorts[i],
                  LIBBASE->sd.ramBase[i]));

            LIBBASE->sd.ohciDevice[i] = OOP_NewObject(NULL, (STRPTR)CLID_Drv_USB_OHCI, tags);
            HIDD_USB_AttachDriver(LIBBASE->sd.usb, LIBBASE->sd.ohciDevice[i]);
        }
    }

    return TRUE;
}

static int OHCI_ExpungeClass(LIBBASETYPEPTR LIBBASE)
{
    D(bug("[OHCI] ExpungeClass\n"));

    OOP_ReleaseAttrBase((STRPTR)IID_Drv_USB_OHCI);

    return TRUE;
}

ADD2INITLIB(OHCI_InitClass, 0)
ADD2EXPUNGELIB(OHCI_ExpungeClass, 0)
