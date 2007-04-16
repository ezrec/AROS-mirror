#ifndef OHCI_H_
#define OHCI_H_

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

#include <inttypes.h>

#include <aros/asmcall.h>

#include LC_LIBDEFS_FILE

#define mmio(var) (*(volatile uint32_t *)&(var))

typedef struct ohci_registers {
    uint32_t    HcRevision;
    uint32_t    HcControl;
    uint32_t    HcCommandStatus;
    uint32_t    HcInterruptStatus;
    uint32_t    HcInterruptEnable;
    uint32_t    HcInterruptDisable;
    uint32_t    HcHCCA;
    uint32_t    HcPeriodCurrentED;
    uint32_t    HcControlHeadED;
    uint32_t    HcControlCurrentED;
    uint32_t    HcBulkHeadED;
    uint32_t    HcBulkCurrentED;
    uint32_t    HcDoneHead;
    uint32_t    HcFmInterval;
    uint32_t    HcFmRemaining;
    uint32_t    HcFmNumber;
    uint32_t    HcPeriodicStart;
    uint32_t    HcLSThreshold;
    uint32_t    HcRhDescriptorA;
    uint32_t    HcRhDescriptorB;
    uint32_t    HcRhStatus;
    uint32_t    HcRhPortStatus[0];
} ohci_registers_t;

/* HcControl */
#define HC_CTRL_CBSR_MASK       0x00000003
#define HC_CTRL_CBSR_1_1        0x00000000
#define HC_CTRL_CBSR_1_2        0x00000001
#define HC_CTRL_CBSR_1_3        0x00000002
#define HC_CTRL_CBSR_1_4        0x00000003
#define HC_CTRL_PLE             0x00000004
#define HC_CTRL_IE              0x00000008
#define HC_CTRL_CLE             0x00000010
#define HC_CTRL_BLE             0x00000020
#define HC_CTRL_HCFS_MASK       0x000000c0
#define HC_CTRL_HCFS_RESET      0x00000000
#define HC_CTRL_HCFS_RESUME     0x00000040
#define HC_CTRL_HCFS_OPERATIONAL 0x00000080
#define HC_CTRL_HCFS_SUSPENDED  0x000000c0
#define HC_CTRL_IR              0x00000100
#define HC_CTRL_RWC             0x00000200
#define HC_CTRL_RWE             0x00000400

/* HcCommandStatus */
#define HC_CS_HCR               0x00000001
#define HC_CS_CLF               0x00000002
#define HC_CS_BLF               0x00000004
#define HC_CS_OCR               0x00000008
#define HC_CS_SOC_MASK          0x00030000

/* HcInterruptStatus, HcInterruptDisable */
#define HC_INTR_SO              0x00000001
#define HC_INTR_WDH             0x00000002
#define HC_INTR_SF              0x00000004
#define HC_INTR_RD              0x00000008
#define HC_INTR_UE              0x00000010
#define HC_INTR_FNO             0x00000020
#define HC_INTR_RHCS            0x00000040
#define HC_INTR_OC              0x40000000
#define HC_INTR_MIE             0x80000000

/* HcFmInterval */
#define HC_FM_GET_IVAL(v)       ((v) & 0x3fff)
#define HC_FM_GET_FSMPS(v)      (((v) >> 16) & 0x7fff)
#define HC_FM_FIT               0x80000000

/* HcRhDescriptorA */
#define HC_RHA_GET_NDP(v)       ((v) & 0xff)
#define HC_RHA_PSM              0x00000100
#define HC_RHA_NPS              0x00000200
#define HC_RHA_DT               0x00000400
#define HC_RHA_OCPM             0x00000800
#define HC_RHA_NOCP             0x00001000
#define HC_RHA_GET_POTPGT(v)    ((v) >> 24)

/* HcRhStatus */
#define HC_RHS_LPS              0x00000001
#define HC_RHS_OCI              0x00000002
#define HC_RHS_DRWE             0x00008000
#define HC_RHS_LPSC             0x00010000
#define HC_RHS_OCIC             0x00020000
#define HC_RHS_CRWE             0x80000000

/* HcRhPortStatus */
#define HC_PS_CCS               0x00000001
#define HC_PS_PES               0x00000002
#define HC_PS_PSS               0x00000004
#define HC_PS_POCI              0x00000008
#define HC_PS_PRS               0x00000010
#define HC_PS_PPS               0x00000100
#define HC_PS_LSDA              0x00000200
#define HC_PS_CSC               0x00010000
#define HC_PS_PESC              0x00020000
#define HC_PS_PSSC              0x00040000
#define HC_PS_OCIC              0x00080000
#define HC_PS_PRSC              0x00100000

typedef struct ohci_hcca {
    uint32_t    hccaIntrTab[32];
    uint16_t    hccaFrNum;
    uint32_t    hccaDoneHead;
    uint8_t     hccaRsvd[116];
} __attribute__((packed)) ohci_hcca_t;

#define HC_FSMPS(i) (((i-210) * 6 / 7)<<16)
#define HC_PERIODIC(i) ((i) * 9 / 10)

#define CLID_Drv_USB_OHCI "Bus::Drv::OHCI"
#define IID_Drv_USB_OHCI  "Bus::Drv::OHCI"

#undef HiddPCIDeviceAttrBase
#undef HiddUSBDeviceAttrBase
#undef HiddUSBHubAttrBase
#undef HiddUSBDrvAttrBase
#undef HiddOHCIAttrBase
#undef HiddAttrBase

#define HiddPCIDeviceAttrBase (SD(cl)->HiddPCIDeviceAB)
#define HiddUSBDeviceAttrBase (SD(cl)->HiddUSBDeviceAB)
#define HiddUSBHubAttrBase (SD(cl)->HiddUSBHubAB)
#define HiddUSBDrvAttrBase (SD(cl)->HiddUSBDrvAB)
#define HiddOHCIAttrBase (SD(cl)->HiddOHCIAB)
#define HiddAttrBase (SD(cl)->HiddAB)

#define MAX_OHCI_DEVICES        8

struct ohci_staticdata
{
    OOP_Class           *ohciClass;

    OOP_Object          *irq;
    OOP_Object          *usb;
    OOP_Object          *pci;
    
    OOP_AttrBase        HiddPCIDeviceAB;
    OOP_AttrBase        HiddUSBDeviceAB;
    OOP_AttrBase        HiddUSBHubAB;
    OOP_AttrBase        HiddUSBDrvAB;
    OOP_AttrBase        HiddOHCIAB;
    OOP_AttrBase        HiddAB;
    
    void                *memPool;
    
    uint8_t             numDevices;
    intptr_t            ramBase[MAX_OHCI_DEVICES];
    uint8_t             numPorts[MAX_OHCI_DEVICES];
    OOP_Object          *pciDevice[MAX_OHCI_DEVICES];
    OOP_Object          *pciDriver[MAX_OHCI_DEVICES];
    OOP_Object          *ohciDevice[MAX_OHCI_DEVICES];
};

struct ohcibase
{
    struct Library          LibNode;
    struct ExecBase         *sysbase;
    BPTR                    seglist;
    struct ohci_staticdata  sd;
};

typedef struct OHCIData {
    struct ohci_staticdata      *sd;
    volatile ohci_registers_t   *regs;
    ohci_hcca_t                 *hcca;
    usb_hub_descriptor_t        hubDescr;
    struct List                 intList;
    struct Interrupt            *tmp;
    
    struct MsgPort              timerPort;
    struct Interrupt            timerInt;
    struct timerequest          *timerReq;

} OHCIData;

#define BASE(lib)((struct ohcibase*)(lib))
#define SD(cl) (&BASE(cl->UserData)->sd)

#define METHOD(base, id, name) \
    base ## __ ## id ## __ ## name (OOP_Class *cl, OOP_Object *o, struct p ## id ## _ ## name *msg)

enum {
    aoHidd_OHCI_MemBase,
    aoHidd_OHCI_PCIDriver,
    aoHidd_OHCI_PCIDevice,

    num_Hidd_OHCI_Attrs
};

#define aHidd_OHCI_MemBase      (HiddOHCIAttrBase + aoHidd_OHCI_MemBase)
#define aHidd_OHCI_PCIDriver    (HiddOHCIAttrBase + aoHidd_OHCI_PCIDriver)
#define aHidd_OHCI_PCIDevice    (HiddOHCIAttrBase + aoHidd_OHCI_PCIDevice)
#define IS_OHCI_ATTR(attr, idx) (((idx)=(attr)-HiddOHCIAttrBase) < num_Hidd_OHCI_Attrs)

#define PCI_BASE_CLASS_SERIAL   0x0c
#define PCI_SUB_CLASS_USB               0x03
#define PCI_INTERFACE_OHCI      0x10

AROS_UFP3(void, OHCI_HubInterrupt,
          AROS_UFPA(APTR, interruptData, A1),
          AROS_UFPA(APTR, interruptCode, A5),
          AROS_UFPA(struct ExecBase *, SysBase, A6));

#endif /*OHCI_H_*/
