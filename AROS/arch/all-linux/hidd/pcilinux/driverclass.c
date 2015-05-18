/*
    Copyright © 2004-2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: PCI direct driver for x86 linux.
    Lang: English
*/

#include <fcntl.h>      // O_RDWR

#include <exec/interrupts.h>
#include <exec/rawfmt.h>
#include <hidd/hidd.h>
#include <hidd/pci.h>
#include <oop/oop.h>

#include <utility/tagitem.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/oop.h>

#include <aros/symbolsets.h>

#include "pci.h"
#include "syscall.h"

#define DEBUG 1
#include <aros/debug.h>

#include <unistd.h>

#ifdef HiddPCIDriverAttrBase
#undef HiddPCIDriverAttrBase
#endif // HiddPCIDriverAttrBase
#undef  HiddAttrBase

#define HiddPCIDriverAttrBase   (PSD(cl)->hiddPCIDriverAB)
#define HiddAttrBase            (PSD(cl)->hiddAB)

#define CFGADD(bus,dev,func,reg)    \
    ( 0x80000000 | ((bus)<<16) |    \
    ((dev)<<11) | ((func)<<8) | ((reg)&~3))

typedef union _pcicfg
{
    ULONG   ul;
    UWORD   uw[2];
    UBYTE   ub[4];
} pcicfg;

/*
    We overload the New method in order to introduce the Hidd Name and
    HardwareName attributes.
*/
OOP_Object *PCILx__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{
    struct pRoot_New mymsg;
    
    struct TagItem mytags[] = {
        { aHidd_Name, (IPTR)"PCILinux" },
        { aHidd_HardwareName, (IPTR)"Linux direct access PCI driver" },
        { aHidd_PCIDriver_DirectBus, FALSE },
        { TAG_DONE, 0 }
    };

    mymsg.mID = msg->mID;
    mymsg.attrList = (struct TagItem *)&mytags;

    if (msg->attrList)
    {
        mytags[3].ti_Tag = TAG_MORE;
        mytags[3].ti_Data = (IPTR)msg->attrList;
    }
 
    msg = &mymsg;
 
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    return o;
}

#ifdef PCI_CONFIG_IO
/*
    Some in/out magic to access PCI bus on PCs
*/
static inline ULONG inl(UWORD port)
{
    ULONG val;

    asm volatile ("inl %w1,%0":"=a"(val):"Nd"(port));

    return val;
}

static inline void outl(ULONG val, UWORD port)
{
    asm volatile ("outl %0,%w1"::"a"(val),"Nd"(port));
}
#endif

#ifdef PCI_CONFIG_SYS
static inline int pci_config_sys_open(OOP_Class *cl, int domain,
                                      int bus, int dev, int sub)
{
    char path[64];

    NewRawDoFmt("/sys/bus/pci/devices/%04X:%02X:%02X.%d/config",
                RAWFMTFUNC_STRING, path, 
                domain, bus, dev, sub);
    return Hidd_UnixIO_OpenFile(PSD(cl)->hiddUnixIO, path, O_RDWR, 0, NULL);
}

static inline int pci_config_sys_close(OOP_Class *cl, int fd)
{
    return Hidd_UnixIO_CloseFile(PSD(cl)->hiddUnixIO, fd, NULL);
}
#endif

IPTR PCILx__Hidd_PCIDriver__HasExtendedConfig(OOP_Class *cl, OOP_Object *o,
                                              struct pHidd_PCIDriver_HasExtendedConfig *msg)
{
    IPTR mmio = 0;

    return mmio;
}

#ifdef PCI_CONFIG_IO
ULONG PCILx__Hidd_PCIDriver__ReadConfigLong(OOP_Class *cl, OOP_Object *o, 
    struct pHidd_PCIDriver_ReadConfigLong *msg)
{
    ULONG orig, temp;
    Disable();    
    orig=inl(PCI_AddressPort);
    outl(CFGADD(msg->bus, msg->dev, msg->sub, msg->reg),PCI_AddressPort);
    temp=inl(PCI_DataPort);
    outl(orig, PCI_AddressPort);
    Enable();
}

void PCILx__Hidd_PCIDriver__WriteConfigLong(OOP_Class *cl, OOP_Object *o,
    struct pHidd_PCIDriver_WriteConfigLong *msg)
{
    ULONG orig;
   
    Disable();
    orig=inl(PCI_AddressPort);
    outl(CFGADD(msg->bus, msg->dev, msg->sub, msg->reg),PCI_AddressPort);
    outl(msg->val,PCI_DataPort);
    outl(orig, PCI_AddressPort);
    Enable();
}
#endif /* PCI_CONFIG_IO */

#ifdef PCI_CONFIG_SYS
ULONG PCILx__Hidd_PCIDriver__ReadConfigLong(OOP_Class *cl, OOP_Object *o, 
    struct pHidd_PCIDriver_ReadConfigLong *msg)
{
    ULONG orig, temp;
    int fd;

    Disable();
    orig = temp = 0;
    fd = pci_config_sys_open(cl, 0, msg->bus, msg->dev, msg->sub);
    if (fd >= 0) {
        Hidd_UnixIO_SeekFile(PSD(cl)->hiddUnixIO, fd,
                             msg->reg, SEEK_SET, NULL);
        Hidd_UnixIO_ReadFile(PSD(cl)->hiddUnixIO, fd,
                              &orig, sizeof(orig), NULL);
        temp = AROS_LE2LONG(orig);
        pci_config_sys_close(cl, fd);
    }
    Enable();

    return temp;
}

void PCILx__Hidd_PCIDriver__WriteConfigLong(OOP_Class *cl, OOP_Object *o,
    struct pHidd_PCIDriver_WriteConfigLong *msg)
{
    int fd;

    Disable();
    fd = pci_config_sys_open(cl, 0, msg->bus, msg->dev, msg->sub);
    if (fd >= 0) {
        ULONG val = AROS_LONG2LE(msg->val);
        Hidd_UnixIO_SeekFile(PSD(cl)->hiddUnixIO, fd,
                             msg->reg, SEEK_SET, NULL);
        Hidd_UnixIO_WriteFile(PSD(cl)->hiddUnixIO, fd,
                              &val, sizeof(val), NULL);
        pci_config_sys_close(cl, fd);
    }
    Enable();
}
#endif /* PCI_CONFIG_SYS */

IPTR PCILx__Hidd_PCIDriver__MapPCI(OOP_Class *cl, OOP_Object *o,
    struct pHidd_PCIDriver_MapPCI *msg)
{
    void *addr;

    D(bug("[PCILinux] PCIDriver::MapPCI(%p, %x)\n",
                (void *)msg->PCIAddress, (unsigned)msg->Length));

    addr = Hidd_UnixIO_MemoryMap(PSD(cl)->hiddUnixIO, NULL,
                                 msg->Length, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, PSD(cl)->fd,
                                 (IPTR)msg->PCIAddress, NULL);

    D(bug("[PCILinux] mmap syscall returned %p\n", addr));

    return (IPTR)addr;
}

VOID PCILx__Hidd_PCIDriver__UnmapPCI(OOP_Class *cl, OOP_Object *o,
    struct pHidd_PCIDriver_UnmapPCI *msg)
{
    Hidd_UnixIO_MemoryUnMap(PSD(cl)->hiddUnixIO, msg->CPUAddress, 
                            msg->Length, NULL);
}

BOOL PCILx__Hidd_PCIDriver__AddInterrupt(OOP_Class *cl, OOP_Object *o,
     struct pHidd_PCIDriver_AddInterrupt *msg)
{
    /* We cannot support interrupts, sorry */
    return FALSE;
}

VOID PCILx__Hidd_PCIDriver__RemoveInterrupt(OOP_Class *cl, OOP_Object *o,
     struct pHidd_PCIDriver_RemoveInterrupt *msg)
{

}

/* Class initialization and destruction */

#define psd (&LIBBASE->psd)

static int PCILx_ExpungeClass(LIBBASETYPEPTR LIBBASE)
{
    D(bug("[PCILinux] deleting classes\n"));
    
    OOP_ReleaseAttrBase(IID_Hidd_PCIDriver);
    OOP_ReleaseAttrBase(IID_Hidd);

    return TRUE;
}
        
static int PCILx_InitClass(LIBBASETYPEPTR LIBBASE)
{
    OOP_Object *pci = NULL;

    D(bug("LinuxPCI: Driver initialization\n"));

    psd->hiddPCIDriverAB = OOP_ObtainAttrBase(IID_Hidd_PCIDriver);
    psd->hiddAB = OOP_ObtainAttrBase(IID_Hidd);

    if (psd->hiddPCIDriverAB)
    {
        /*
         * The class may be added to the system. Add the driver
         * to PCI subsystem as well
         */
        struct pHidd_PCI_AddHardwareDriver msg;
                
        /*
         * PCI is suppose to destroy the class on its Dispose
         */
        msg.driverClass = psd->driverClass;
        msg.mID = OOP_GetMethodID(IID_Hidd_PCI, moHidd_PCI_AddHardwareDriver);

        // Add it for God's sake! ;)
        pci = OOP_NewObject(NULL, CLID_Hidd_PCI, NULL);
        OOP_DoMethod(pci, (OOP_Msg)&msg);
        OOP_DisposeObject(pci);
    }
    else
        return FALSE;

    D(bug("LinuxPCI: Driver ClassPtr = %x\n", psd->driverClass));

    return TRUE;
}

ADD2INITLIB(PCILx_InitClass, 0)
ADD2EXPUNGELIB(PCILx_ExpungeClass, 0)
