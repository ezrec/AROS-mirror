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

#include <aros/symbolsets.h>

#include <exec/types.h>
#include <oop/oop.h>

#include <hidd/hidd.h>
#include <hidd/pci.h>

#include <usb/usb.h>

#define DEBUG 1

#include <proto/exec.h>
#include <proto/oop.h>
#include <aros/debug.h>

#include "uhci.h"

OOP_AttrBase HiddPCIDeviceAttrBase;
OOP_AttrBase HiddUSBDeviceAttrBase;
OOP_AttrBase HiddUSBHubAttrBase;
OOP_AttrBase HiddUSBDrvAttrBase;
OOP_AttrBase HiddAttrBase;

AROS_UFH3(void, Enumerator,
        AROS_UFHA(struct Hook *,        hook,           A0),
        AROS_UFHA(OOP_Object *,         pciDevice,      A2),
        AROS_UFHA(APTR,                 message,        A1))
{
    AROS_USERFUNC_INIT

    static int counter;

    if (counter == MAX_DEVS)
        return;

    LIBBASETYPE *LIBBASE = (LIBBASETYPE *)hook->h_Data;

    struct TagItem attrs[] = {
            { aHidd_PCIDevice_isIO,     TRUE },
            { aHidd_PCIDevice_isMaster, TRUE },
            { TAG_DONE, 0UL },
    };

    OOP_SetAttrs(pciDevice, (struct TagItem *)attrs);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Base4, &LIBBASE->sd.iobase[counter]);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Driver, (APTR)&LIBBASE->sd.uhciPCIDriver[counter]);
    LIBBASE->sd.uhciDevice[counter] = pciDevice;

    D(bug("[UHCI]   Device %d @ %08x with IO @ %08x\n", counter, pciDevice, LIBBASE->sd.iobase[counter]));

    D({
        struct pHidd_PCIDevice_ReadConfigWord __msg = {
                OOP_GetMethodID(CLID_Hidd_PCIDevice, moHidd_PCIDevice_ReadConfigWord), 0xc0
        }, *msg = &__msg;

        bug("[UHCI]    0xc0: %04x\n", OOP_DoMethod(pciDevice, msg));
    });

    LIBBASE->sd.num_devices = ++counter;

    AROS_USERFUNC_EXIT
}

static int UHCI_Init(LIBBASETYPEPTR LIBBASE)
{
    D(bug("[UHCI] UHCI_Init()\n"));

    LIBBASE->sd.usb = OOP_NewObject(NULL, (STRPTR)CLID_Hidd_USB, NULL);

    if (!LIBBASE->sd.usb)
    {
        bug("[UHCI] Cannot create the instance of base USB class\n");
        return FALSE;
    }

    if ((LIBBASE->sd.pci=OOP_NewObject(NULL, (STRPTR)CLID_Hidd_PCI, NULL)))
    {
        struct TagItem tags[] = {
                { tHidd_PCI_Class,      PCI_BASE_CLASS_SERIAL },
                { tHidd_PCI_SubClass,   PCI_SUB_CLASS_USB },
                { tHidd_PCI_Interface,  PCI_INTERFACE_UHCI },
                { TAG_DONE, 0UL }
        };

        struct OOP_ABDescr attrbases[] = {
                { (STRPTR)IID_Hidd,             &HiddAttrBase },
                { (STRPTR)IID_Hidd_PCIDevice,   &HiddPCIDeviceAttrBase },
                { (STRPTR)IID_Hidd_USBDevice,   &HiddUSBDeviceAttrBase },
                { (STRPTR)IID_Hidd_USBHub,      &HiddUSBHubAttrBase },
                { (STRPTR)IID_Hidd_USBDrv,      &HiddUSBDrvAttrBase },
                { NULL, NULL }
        };

        struct Hook FindHook = {
                h_Entry:        (IPTR (*)())Enumerator,
                h_Data:         LIBBASE,
        };

        NEWLIST(&LIBBASE->sd.td_list);
        InitSemaphore(&LIBBASE->sd.global_lock);
        InitSemaphore(&LIBBASE->sd.td_lock);

        OOP_ObtainAttrBases(attrbases);

        D(bug("[UHCI] Searching for UHCI devices...\n"));

        HIDD_PCI_EnumDevices(LIBBASE->sd.pci, &FindHook, (struct TagItem *)&tags);

        D(bug("[UHCI] Done. UHCI devices found: %d\n", LIBBASE->sd.num_devices));

        if (LIBBASE->sd.num_devices > 0)
        {
            LIBBASE->sd.MemPool = CreatePool(MEMF_CLEAR | MEMF_PUBLIC | MEMF_SEM_PROTECTED, 8192, 4096);

            if (LIBBASE->sd.MemPool)
                return TRUE;
        }
    }

    return FALSE;
}

static int UHCI_Expunge(LIBBASETYPEPTR LIBBASE)
{
    struct OOP_ABDescr attrbases[] = {
            { (STRPTR)IID_Hidd,                 &HiddAttrBase },
            { (STRPTR)IID_Hidd_PCIDevice,       &HiddPCIDeviceAttrBase },
            { (STRPTR)IID_Hidd_USBDevice,       &HiddUSBDeviceAttrBase },
            { (STRPTR)IID_Hidd_USBHub,          &HiddUSBHubAttrBase },
            { (STRPTR)IID_Hidd_USBDrv,          &HiddUSBDrvAttrBase },
            { NULL, NULL }
    };

    OOP_ReleaseAttrBases(attrbases);

    return TRUE;	
}

ADD2INITLIB(UHCI_Init, 0)
ADD2EXPUNGELIB(UHCI_Expunge, 0)
ADD2LIBS((STRPTR)"usb.hidd", 0, static struct Library *, __usbbase)
