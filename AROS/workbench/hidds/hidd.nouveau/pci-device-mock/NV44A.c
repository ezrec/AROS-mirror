/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <proto/oop.h>

#include "pcimockhardware.h"
#include "pcimockhardware_intern.h"
#include "pcimock_intern.h"

OOP_Object * METHOD(NV44AMockHardware, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    GET_PCIMOCKHWDATA
    
    /* Allocate address space regions */
    ALLOC_ADDRESSSPACEREGION(hwdata, PCI_CONFIG_SPACE, 1024);

    /* Fill in pci config space */
    SET_ASR_DWORD(hwdata, PCI_CONFIG_SPACE, 0x00, 0x022110de); /* PCICS_PRODUCT, PCICS_VENDOR */
    SET_ASR_DWORD(hwdata, PCI_CONFIG_SPACE, 0x08, 0x030000a1); /* PCICS_CLASS, PCICS_SUBCLASS, PCICS_PROGIF, PCICS_REVISION */
    SET_ASR_DWORD(hwdata, PCI_CONFIG_SPACE, 0x0c, 0x00000000); /* PCICS_BIST, PCICS_HEADERTYPE, PCICS_LATENCY, PCICS_CACHELS */
    SET_ASR_DWORD(hwdata, PCI_CONFIG_SPACE, 0x2c, 0x81c71043); /* PCICS_SUBSYSTEM, PCICS_SUBVENDOR */
    SET_ASR_DWORD(hwdata, PCI_CONFIG_SPACE, 0x3c, 0x0000010b); /* PCICS_INT_PIN, PCICS_INT_LINE */

    return o;
}

