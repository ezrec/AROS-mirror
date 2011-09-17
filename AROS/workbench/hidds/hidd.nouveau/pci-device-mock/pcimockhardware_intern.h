#ifndef PCIMOCKHARDWARE_INTERN_H
#define PCIMOCKHARDWARE_INTERN_H

/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>

#define PCI_BAR0            0
#define PCI_BAR1            1
#define PCI_BAR2            2
#define PCI_BAR3            3
#define PCI_BAR4            4
#define PCI_BAR5            5
#define PCI_CONFIG_SPACE    6
#define PCI_REGIONS_COUNT   7

struct AddressSpaceRegion
{
    ULONG   Size;
    IPTR    Address;
};

struct HIDDPCIMockHardwareData
{
    struct AddressSpaceRegion   Regions[PCI_REGIONS_COUNT];
};


#define GET_PCIMOCKHWDATA \
    struct HIDDPCIMockHardwareData * hwdata = OOP_INST_DATA((SD(cl))->pciMockHardwareClass, o);

#define ALLOC_ADDRESSSPACEREGION(hwdata, n, size)                               \
    hwdata->Regions[n].Size = size;                                             \
    hwdata->Regions[n].Address = (IPTR)AllocVec(size, MEMF_ANY | MEMF_CLEAR)    \

#define SET_ASR_DWORD(hwdata, n, address, value) \
    ((ULONG *)hwdata->Regions[n].Address)[address] = value;

#define CLID_Hidd_PCIMockHardware_NV44A   "hidd.pcimockhardware.nv44a"

struct HIDDNV44AMockHardwareData
{
};


#endif /* PCIMOCKHARDWARE_INTERN_H */

