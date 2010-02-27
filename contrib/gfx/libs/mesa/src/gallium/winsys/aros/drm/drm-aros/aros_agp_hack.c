#include "aros_agp_hack.h"

#include <proto/oop.h>
#include <proto/dos.h>
#include <hidd/pci.h>
#include <hidd/hidd.h>
#include <aros/libcall.h>
#define DEBUG 1
#include <aros/debug.h>
#include <aros/symbolsets.h>

OOP_AttrBase __IHidd_PCIDev_AGP = 0;
struct Library * OOPBase_AGP = NULL;
OOP_Object * pciBus_AGP = NULL;

#define OOPBase OOPBase_AGP
#define __IHidd_PCIDev __IHidd_PCIDev_AGP
#define __IHidd_PCIDrv __IHidd_PCIDrv_AGP

#define CAPABILITY_AGP      0x02
#define CAPABILITY_PCIE     0x10

static UBYTE readconfigbyte(OOP_Object * pciDevice, UBYTE where)
{
    struct pHidd_PCIDevice_ReadConfigByte rcbmsg = {
    mID: OOP_GetMethodID(IID_Hidd_PCIDevice, moHidd_PCIDevice_ReadConfigByte),
    reg: where,
    }, *msg = &rcbmsg;
    
    return (UBYTE)OOP_DoMethod(pciDevice, (OOP_Msg)msg);
}

static UWORD readconfigword(OOP_Object * pciDevice, UBYTE where)
{
    struct pHidd_PCIDevice_ReadConfigWord rcwmsg = {
    mID: OOP_GetMethodID(IID_Hidd_PCIDevice, moHidd_PCIDevice_ReadConfigWord),
    reg: where,
    }, *msg = &rcwmsg;
    
    return (UWORD)OOP_DoMethod(pciDevice, (OOP_Msg)msg);    
}

static ULONG readconfiglong(OOP_Object * pciDevice, UBYTE where)
{
    /* Has to be done like this because of bug in PciDevice ReadConfigLong
       method (return type was UBYTE instead of ULONG) */

    ULONG result = 0;
    
    result = readconfigword(pciDevice, where + 2);
    result <<= 16;
    result |= readconfigword(pciDevice, where);
    return result;
}

static void writeconfiglong(OOP_Object * pciDevice, UBYTE where, ULONG val)
{
    struct pHidd_PCIDevice_WriteConfigLong wclmsg = {
    mID: OOP_GetMethodID(IID_Hidd_PCIDevice, moHidd_PCIDevice_WriteConfigLong),
    reg: where,
    val: val,
    }, *msg = &wclmsg;
    
    OOP_DoMethod(pciDevice, (OOP_Msg)msg); 
}

static void writeconfigbyte(OOP_Object * pciDevice, UBYTE where, UBYTE val)
{
    struct pHidd_PCIDevice_WriteConfigByte wcbmsg = {
    mID: OOP_GetMethodID(IID_Hidd_PCIDevice, moHidd_PCIDevice_WriteConfigByte),
    reg: where,
    val: val,
    }, *msg = &wcbmsg;
    
    OOP_DoMethod(pciDevice, (OOP_Msg)msg); 
}

static void writeconfigword(OOP_Object * pciDevice, UBYTE where, UWORD val)
{
    struct pHidd_PCIDevice_WriteConfigWord wcwmsg = {
    mID: OOP_GetMethodID(IID_Hidd_PCIDevice, moHidd_PCIDevice_WriteConfigWord),
    reg: where,
    val: val,
    }, *msg = &wcwmsg;
    
    OOP_DoMethod(pciDevice, (OOP_Msg)msg); 
}

/* This needs to be added to pci.hidd */
static UBYTE aros_agp_hack_get_capabilities_ptr(OOP_Object * pciDevice, UBYTE cap)
{
    UWORD where = 0x34; /*  First cap list entry */
    UBYTE capid = 0;
    
    while(where < 0xff)
    {
        where = readconfigbyte(pciDevice, (UBYTE)where);

        if (where < 0x40)
            break;
        where &= ~3; /* ?? */
        capid = readconfigbyte(pciDevice, (UBYTE)where);
        if (capid == 0xff)
            break;

        if (capid == cap) return (UBYTE)where;
    
        where += 1; /* next cap */
    }
    
    return (UBYTE)0;
}

BOOL aros_agp_hack_device_is_agp(OOP_Object * pciDevice)
{
    return (aros_agp_hack_get_capabilities_ptr(pciDevice, CAPABILITY_AGP) != (UBYTE)0);
}

BOOL aros_agp_hack_device_is_pcie(OOP_Object * pciDevice)
{
    return (aros_agp_hack_get_capabilities_ptr(pciDevice, CAPABILITY_PCIE) != (UBYTE)0);
}

/* AGP Support */
struct PciAgpDevice {
    struct Node node;
    OOP_Object  *PciDevice;
    UBYTE       AgpCapability;          /* Offset to AGP capability in config */
    UWORD       VendorID;
    UWORD       ProductID;
    UBYTE       Class;
};

struct agp_staticdata {
    struct SignalSemaphore driverlock;  /* Lock for driver operations */
    struct List         devices;        /* Bridges and AGP devices in system */
    BOOL                enabled;        /* Indicates if AGP is enabled */

    /* Bridge data */   
    struct PciAgpDevice *bridge;        /* Selected AGP bridge */
    BOOL                bridgesupported;/* TRUE if detected bridge is supported by driver */
    ULONG               bridgemode;     /* Mode of AGP bridge */
    IPTR                bridgeaperbase; /* Base address for aperture */
    IPTR                bridgeapersize; /* Size of aperture */
    APTR                gatttablebuffer;/* Buffer for gatt table */
    ULONG               *gatttable;     /* 4096 aligned gatt table */
    APTR                scratchmembuffer;/* Buffer for scratch mem */
    ULONG               *scratchmem;    /* 4096 aligned scratch mem */
    ULONG               memmask;        /* Mask for binded memorory */

    /* Video card data */
    struct PciAgpDevice *videocard;     /* Selected AGP card */
    
    /* Functions */
    VOID    (*create_gatt_table)(struct agp_staticdata * agpsd);
    VOID    (*unbind_memory)(struct agp_staticdata * agpsd, ULONG offset, ULONG size);
    VOID    (*flush_gatt_table)(struct agp_staticdata *agpsd);
    VOID    (*bind_memory)(struct agp_staticdata * agpsd, IPTR address, ULONG size, ULONG offset);
    VOID    (*enable_agp)(struct agp_staticdata * agpsd, ULONG mode);
    VOID    (*initialize_chipset)(struct agp_staticdata * agpsd);
    VOID    (*deinitialize_chipset)(struct agp_staticdata * agpsd);
    VOID    (*free_gatt_table)(struct agp_staticdata * agpsd);
    VOID    (*flush_chipset)(struct agp_staticdata * agpsd);
};

#define writel(val, addr)               (*(volatile ULONG*)(addr) = (val))
#define readl(addr)                     (*(volatile ULONG*)(addr))
#define min(a,b)                        ((a) < (b) ? (a) : (b))
#define max(a,b)                        ((a) > (b) ? (a) : (b))

#define AGP_APER_BASE                   0x10                /* BAR0 */
#define AGP_VERSION_REG                 0x02
#define AGP_STATUS_REG                  0x04
#define AGP_STATUS_REG_AGP_3_0          (1<<3)
#define AGP_STATUS_REG_FAST_WRITES      (1<<4)
#define AGP_STATUS_REG_AGP_ENABLED      (1<<8)
#define AGP_STATUS_REG_SBA              (1<<9)
#define AGP_STATUS_REG_CAL_MASK         (1<<12|1<<11|1<<10)
#define AGP_STATUS_REG_ARQSZ_MASK       (1<<15|1<<14|1<<13)
#define AGP_STATUS_REG_RQ_DEPTH_MASK    0xff000000
#define AGP_STATUS_REG_AGP2_X1          (1<<0)
#define AGP_STATUS_REG_AGP2_X2          (1<<1)
#define AGP_STATUS_REG_AGP2_X4          (1<<2)
#define AGP_STATUS_REG_AGP3_X4          (1<<0)
#define AGP_STATUS_REG_AGP3_X8          (1<<1)
#define AGP_COMMAND_REG                 0x08
#define AGP_CTRL_REG                    0x10
#define AGP_CTRL_REG_GTBLEN             (1<<7)
#define AGP_CTRL_REG_APEREN             (1<<8)
#define AGP_APER_SIZE_REG               0x14
#define AGP_GATT_CTRL_LO_REG            0x18

#define AGP2_RESERVED_MASK              0x00fffcc8
#define AGP3_RESERVED_MASK              0x00ff00c4


#define ALIGN(val, align)               (val + align - 1) & (~(align - 1))

/* Generic AGP3 functions */
static VOID agp3_flush_gatt_table(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    ULONG ctrlreg;
    ctrlreg = readconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG);
    writeconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG, ctrlreg & ~AGP_CTRL_REG_GTBLEN);
    writeconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG, ctrlreg);
}

static VOID agp3_initialize_chipset(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    UWORD aperture_size_value = 0;
    ULONG ctrlreg = 0;
    
    /* Getting GART size */
    aperture_size_value = readconfigword(bridgedev, bridgeagpcap + AGP_APER_SIZE_REG);
    D(bug("[AGP] Reading aperture size value: %x\n", aperture_size_value));
    
    switch(aperture_size_value)
    {
        case(0xf3f): agpsd->bridgeapersize = 4; break;
        case(0xf3e): agpsd->bridgeapersize = 8; break;
        case(0xf3c): agpsd->bridgeapersize = 16; break;
        case(0xf38): agpsd->bridgeapersize = 32; break;
        case(0xf30): agpsd->bridgeapersize = 64; break;
        case(0xf20): agpsd->bridgeapersize = 128; break;
        case(0xf00): agpsd->bridgeapersize = 256; break;
        case(0xe00): agpsd->bridgeapersize = 512; break;
        case(0xc00): agpsd->bridgeapersize = 1024; break;
        case(0x800): agpsd->bridgeapersize = 2048; break;
        default: agpsd->bridgeapersize = 0; break;
    }
    
    D(bug("[AGP] Calculated aperture size: %d MB\n", (ULONG)agpsd->bridgeapersize));

    /* Creation of GATT table */
    agpsd->create_gatt_table(agpsd);
    
    /* Getting GART base */
    agpsd->bridgeaperbase = (IPTR)readconfiglong(bridgedev, AGP_APER_BASE);
    agpsd->bridgeaperbase &= (~0x0fUL) /* PCI_BASE_ADDRESS_MEM_MASK */;
    D(bug("[AGP] Reading aperture base: 0x%x\n", (ULONG)agpsd->bridgeaperbase));

    /* TODO: Setting GART size (is it needed at all?) */
    
    /* Set GATT pointer */
    writeconfiglong(bridgedev, bridgeagpcap + AGP_GATT_CTRL_LO_REG,
        (ULONG)agpsd->gatttable);
    D(bug("[AGP] Set GATT pointer to 0x%x\n", (ULONG)agpsd->gatttable));
    
    /* Enabled GART and GATT */
    ctrlreg = readconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG);
    writeconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG,
        ctrlreg | AGP_CTRL_REG_APEREN | AGP_CTRL_REG_GTBLEN);
    
    D(bug("[AGP] Enabled GART and GATT\n"));
    
    agpsd->bridgesupported = TRUE;
}

static VOID agp3_deinitialize_chipset(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    ULONG ctrlreg;
    ctrlreg = readconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG);
    writeconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG, ctrlreg & ~AGP_CTRL_REG_APEREN);
}

/* Generic functions */
VOID HACK_Wbinvd(); /* Implemented in assembler */

static VOID flush_cpu_cache()
{
    /* Don't use clflush here. Both linux and BSD codes use full wbinvd */
    Supervisor(HACK_Wbinvd);
}

static VOID generic_flush_chipset(struct agp_staticdata * agpsd)
{
    /* This function is a NOOP */
}

static VOID generic_create_gatt_table(struct agp_staticdata * agpsd)
{
    /* Create a table that will hold a certain number of 32bit pointers */
    ULONG entries = agpsd->bridgeapersize * 1024 * 1024 / 4096;
    ULONG tablesize =  entries * 4;
    ULONG i = 0;
    
    agpsd->scratchmembuffer = AllocVec(4096 * 2, MEMF_PUBLIC | MEMF_CLEAR);
    agpsd->scratchmem = (ULONG*)(ALIGN((IPTR)agpsd->scratchmembuffer, 4096));
    D(bug("[AGP] Created scratch memory at 0x%x\n", (ULONG)agpsd->scratchmem));

    
    agpsd->gatttablebuffer = AllocVec(tablesize + 4096, MEMF_PUBLIC | MEMF_CLEAR);
    agpsd->gatttable = (ULONG *)(ALIGN((ULONG)agpsd->gatttablebuffer, 4096));
    
    D(bug("[AGP] Created GATT table size %d at 0x%x\n", tablesize, 
        (ULONG)agpsd->gatttable));
    
    for (i = 0; i < entries; i++)
    {
        writel((ULONG)agpsd->scratchmem, agpsd->gatttable + i);
        readl(agpsd->gatttable + i);	/* PCI Posting. */
    }
    
    flush_cpu_cache();
}

static VOID generic_free_gatt_table(struct agp_staticdata * agpsd)
{
    D(bug("[AGP] Freeing GATT table 0x%x, scratch memory 0x%x\n",
        (ULONG)agpsd->gatttable, (ULONG)agpsd->scratchmem));

    if (agpsd->scratchmembuffer)
        FreeVec(agpsd->scratchmembuffer);
    agpsd->scratchmembuffer = NULL;
    agpsd->scratchmem = NULL;
    
    if (agpsd->gatttablebuffer)
        FreeVec(agpsd->gatttablebuffer);
    agpsd->gatttablebuffer = NULL;
    agpsd->gatttable = NULL;
}

static VOID generic_unbind_memory(struct agp_staticdata * agpsd, ULONG offset, ULONG size)
{
    ULONG i;

    D(bug("Unbind offset %d, size %d\n", offset, size));

    if (!agpsd->enabled)
        return;

    if (size == 0)
        return;

    ObtainSemaphore(&agpsd->driverlock);

    /* TODO: get mask type */

    /* Remove entries from GATT table */
    for(i = 0; i < size / 4096; i++)
    {
        writel((ULONG)agpsd->scratchmem, agpsd->gatttable + offset + i);
    }
    
    readl(agpsd->gatttable + offset + i - 1); /* PCI posting */
    
    /* Flush CPU cache - make sure data in GATT is up to date */
    flush_cpu_cache();

    /* Flush GATT table */
    agpsd->flush_gatt_table(agpsd);
    
    ReleaseSemaphore(&agpsd->driverlock);
}

static VOID generic_bind_memory(struct agp_staticdata * agpsd, IPTR address, ULONG size, ULONG offset)
{
    ULONG i;

    D(bug("Bind address 0x%x into offset %d, size %d\n", (ULONG)address, offset, size));

    if (!agpsd->enabled)
        return;

    ObtainSemaphore(&agpsd->driverlock);

    /* TODO: check if offset + size / 4096 ends before gatt_table end */
    
    /* TODO: get mask type */
    
    /* TODO: Check if each entry in GATT to be written in unbound (might not work due to CPU caches problem - see HACK*/
    
    /* Flush incomming memory - will be done in flush_cpu_cache below */
    
    /* Insert entries into GATT table */
    for(i = 0; i < size / 4096; i++)
    {
        /* Write masked memory address into GATT */
        writel((address + (4096 * i)) | agpsd->memmask, 
            agpsd->gatttable + offset + i);
    }
    
    readl(agpsd->gatttable + offset + i - 1); /* PCI posting */

    /* Flush CPU cache - make sure data in GATT is up to date */
    flush_cpu_cache();

    /* Flush GATT table at card*/
    agpsd->flush_gatt_table(agpsd);

    ReleaseSemaphore(&agpsd->driverlock);
}

static ULONG generic_agp3_calibrate_modes(ULONG requestedmode, ULONG bridgemode, ULONG vgamode)
{
    ULONG calibratedmode = bridgemode;
    ULONG temp = 0;
    
    /* Apply reserved mask to requestedmode */
    requestedmode &= ~AGP3_RESERVED_MASK;
    
    /* Select a speed for requested mode */
    temp = requestedmode & 0x07;
    requestedmode &= ~0x07; /* Clear any speed */
    if (temp & AGP_STATUS_REG_AGP3_X8)
        requestedmode |= AGP_STATUS_REG_AGP3_X8;
    else
        requestedmode |= AGP_STATUS_REG_AGP3_X4;
    
    /* Set ARQSZ as max value. Ignore requestedmode */
    calibratedmode = ((calibratedmode & ~AGP_STATUS_REG_ARQSZ_MASK) |
        max(calibratedmode & AGP_STATUS_REG_ARQSZ_MASK, vgamode & AGP_STATUS_REG_ARQSZ_MASK));
    
    /* Set calibration cycle. Ignore requestedmode */
    calibratedmode = ((calibratedmode & ~AGP_STATUS_REG_CAL_MASK) |
        min(calibratedmode & AGP_STATUS_REG_CAL_MASK, vgamode & AGP_STATUS_REG_CAL_MASK));
    
    /* Set SBA for AGP3 (always) */
    calibratedmode |= AGP_STATUS_REG_SBA;
    
    /* Select speed based on request and capabilities of bridge and vgacard */
    calibratedmode &= ~0x07; /* Clear any mode */
    if ((requestedmode & AGP_STATUS_REG_AGP3_X8) &&
        (bridgemode & AGP_STATUS_REG_AGP3_X8) &&
        (vgamode & AGP_STATUS_REG_AGP3_X8))
        calibratedmode |= AGP_STATUS_REG_AGP3_X8;
    else
        calibratedmode |= AGP_STATUS_REG_AGP3_X4;

    return calibratedmode;
}

static ULONG generic_agp2_calibrate_modes(ULONG requestedmode, ULONG bridgemode, ULONG vgamode)
{
    ULONG calibratedmode = bridgemode;
    ULONG temp = 0;

    /* Apply reserved mask to requestedmode */
    requestedmode &= ~AGP2_RESERVED_MASK;
    
    /* Fix for some bridges reporting only one speed instead of all */
    if (bridgemode & AGP_STATUS_REG_AGP2_X4)
        bridgemode |= (AGP_STATUS_REG_AGP2_X2 | AGP_STATUS_REG_AGP2_X1);
    if (bridgemode & AGP_STATUS_REG_AGP2_X2)
        bridgemode |= AGP_STATUS_REG_AGP2_X1;

    /* Select speed for requested mode */
    temp = requestedmode & 0x07;
    requestedmode &= ~0x07; /* Clear any speed */
    if (temp & AGP_STATUS_REG_AGP2_X4)
        requestedmode |= AGP_STATUS_REG_AGP2_X4;
    else 
    {
        if (temp & AGP_STATUS_REG_AGP2_X2)
            requestedmode |= AGP_STATUS_REG_AGP2_X2;
        else
            requestedmode |= AGP_STATUS_REG_AGP2_X1;
    }
    
    /* Disable SBA if not supported/requested */
    if (!((bridgemode & AGP_STATUS_REG_SBA) && (requestedmode & AGP_STATUS_REG_SBA)
            && (vgamode & AGP_STATUS_REG_SBA)))
        calibratedmode &= ~AGP_STATUS_REG_SBA;

    /* Select speed based on request and capabilities of bridge and vgacard */
    calibratedmode &= ~0x07; /* Clear any mode */
    if ((requestedmode & AGP_STATUS_REG_AGP2_X4) &&
        (bridgemode & AGP_STATUS_REG_AGP2_X4) &&
        (vgamode & AGP_STATUS_REG_AGP2_X4))
        calibratedmode |= AGP_STATUS_REG_AGP2_X4;
    else
    {
        if ((requestedmode & AGP_STATUS_REG_AGP2_X2) &&
            (bridgemode & AGP_STATUS_REG_AGP2_X2) &&
            (vgamode & AGP_STATUS_REG_AGP2_X2))
            calibratedmode |= AGP_STATUS_REG_AGP2_X2;
        else
            calibratedmode |= AGP_STATUS_REG_AGP2_X1;
    }

    /* Disable fast writed if in X1 mode */
    if (calibratedmode & AGP_STATUS_REG_AGP2_X1)
        calibratedmode &= ~AGP_STATUS_REG_FAST_WRITES;
    
    return calibratedmode;
}

static ULONG generic_select_best_mode(struct agp_staticdata * agpsd, ULONG requestedmode, ULONG bridgemode)
{
    OOP_Object * videodev = agpsd->videocard->PciDevice;
    UBYTE videoagpcap = agpsd->videocard->AgpCapability;    
    ULONG vgamode = 0;

    /* Get VGA card capability */
    vgamode = readconfiglong(videodev, videoagpcap + AGP_STATUS_REG);
    
    D(bug("[AGP]     VGA mode 0x%x\n", vgamode));
    
    /* Set Request Queue */
    bridgemode = ((bridgemode & ~AGP_STATUS_REG_RQ_DEPTH_MASK) |
        min(requestedmode & AGP_STATUS_REG_RQ_DEPTH_MASK,
        min(bridgemode & AGP_STATUS_REG_RQ_DEPTH_MASK, vgamode & AGP_STATUS_REG_RQ_DEPTH_MASK)));
    
    /* Fast Writes */
    if (!(
        (bridgemode & AGP_STATUS_REG_FAST_WRITES) &&
        (requestedmode & AGP_STATUS_REG_FAST_WRITES) &&
        (vgamode & AGP_STATUS_REG_FAST_WRITES)))
    {
        bridgemode &= ~AGP_STATUS_REG_FAST_WRITES;
    }
        
    if (agpsd->bridgemode & AGP_STATUS_REG_AGP_3_0)
    {
        bridgemode = generic_agp3_calibrate_modes(requestedmode, bridgemode, vgamode);
    }
    else
    {
        bridgemode = generic_agp2_calibrate_modes(requestedmode, bridgemode, vgamode);
    }
    
    return bridgemode;
}

static VOID generic_send_command(struct agp_staticdata * agpsd, ULONG status)
{
    struct PciAgpDevice * pciagpdev = NULL;

    /* Send command to all AGP capable devices */
    ForeachNode(&agpsd->devices, pciagpdev)
    {
        if(pciagpdev->AgpCapability)
        {
            ULONG mode = status;
            
            mode &= 0x7;
            if (status & AGP_STATUS_REG_AGP_3_0)
                mode *= 4;
            
            D(bug("[AGP] Set AGP%d device 0x%x/0x%x to speed %dx\n", 
                (status & AGP_STATUS_REG_AGP_3_0) ? 3 : 2, 
                pciagpdev->VendorID, pciagpdev->ProductID, mode));
            
            writeconfiglong(pciagpdev->PciDevice, pciagpdev->AgpCapability + AGP_COMMAND_REG, status);

            /* Keep this delay here. Some bridges need it. */
            Delay(10);
        }
    }
}

static VOID generic_enable_agp(struct agp_staticdata * agpsd, ULONG requestedmode)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    ULONG bridgemode = 0;
    ULONG major = 0;
    
    if (!agpsd->bridgesupported)
        return;
    
    ObtainSemaphore(&agpsd->driverlock);
    
    if (agpsd->enabled)
    {
        ReleaseSemaphore(&agpsd->driverlock);
        return;
    }
    
    D(bug("[AGP] Enable AGP:\n"));
    D(bug("[AGP]     Requested mode 0x%x\n", requestedmode));
    
    bridgemode = readconfiglong(bridgedev, bridgeagpcap + AGP_STATUS_REG);
    D(bug("[AGP]     Bridge mode 0x%x\n", requestedmode));
    
    bridgemode = generic_select_best_mode(agpsd, requestedmode, bridgemode);
    
    bridgemode |= AGP_STATUS_REG_AGP_ENABLED;
    
    major = (readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) >> 4) & 0xf;

    if (major >= 3)
    {
        /* Bridge supports version 3 or greater) */
        if (agpsd->bridgemode & AGP_STATUS_REG_AGP_3_0)
        {
            /* Bridge is operating in mode 3.0 */
        }
        else
        {
            /* Bridge is operating in legacy mode */
            /* Disable calibration cycle */
            ULONG temp = 0;
            bridgemode &= ~(7 << 10);
            temp = readconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG);
            temp |= (1 << 9);
            writeconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG, temp);           
        }
    }

    D(bug("[AGP] Mode to write: 0x%x\n", bridgemode));
    
    generic_send_command(agpsd, bridgemode);
    agpsd->enabled = TRUE;
    
    ReleaseSemaphore(&agpsd->driverlock);
}

/* SiS functions */
#define AGP_SIS_GATT_BASE   0x90
#define AGP_SIS_APER_SIZE   0x94
#define AGP_SIS_GATT_CNTRL  0x97
#define AGP_SIS_GATT_FLUSH  0x98

static VOID sis_flush_gatt_table(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    writeconfigbyte(bridgedev, AGP_SIS_GATT_FLUSH, 0x02);    
}

static VOID sis_initialize_chipset(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE aperture_size_value = 0;
    UBYTE temp = 0;
    
    /* Getting GART size */
    aperture_size_value = readconfigbyte(bridgedev, AGP_SIS_APER_SIZE);
    D(bug("[AGP] [SIS] Reading aperture size value: %x\n", aperture_size_value));
    
    switch(aperture_size_value & ~(0x07))
    {
        case( 3 & ~(0x07)): agpsd->bridgeapersize = 4; break;
        case(19 & ~(0x07)): agpsd->bridgeapersize = 8; break;
        case(35 & ~(0x07)): agpsd->bridgeapersize = 16; break;
        case(51 & ~(0x07)): agpsd->bridgeapersize = 32; break;
        case(67 & ~(0x07)): agpsd->bridgeapersize = 64; break;
        case(83 & ~(0x07)): agpsd->bridgeapersize = 128; break;
        case(99 & ~(0x07)): agpsd->bridgeapersize = 256; break;
        default: agpsd->bridgeapersize = 0; break;
    }
    
    D(bug("[AGP] [SIS] Calculated aperture size: %d MB\n", (ULONG)agpsd->bridgeapersize));

    /* Creation of GATT table */
    agpsd->create_gatt_table(agpsd);
    
    /* Getting GART base */
    agpsd->bridgeaperbase = (IPTR)readconfiglong(bridgedev, AGP_APER_BASE);
    agpsd->bridgeaperbase &= (~0x0fUL) /* PCI_BASE_ADDRESS_MEM_MASK */;
    D(bug("[AGP] [SiS] Reading aperture base: 0x%x\n", (ULONG)agpsd->bridgeaperbase));
    
    /* Set GATT pointer */
    writeconfiglong(bridgedev, AGP_SIS_GATT_BASE, (ULONG)agpsd->gatttable);
    D(bug("[AGP] [SiS] Set GATT pointer to 0x%x\n", (ULONG)agpsd->gatttable));
    
    /* Enable GART */
    temp = readconfigbyte(bridgedev, AGP_SIS_APER_SIZE);
    writeconfigbyte(bridgedev, AGP_SIS_APER_SIZE, temp | 3);

    /* Enable GATT */
    writeconfigbyte(bridgedev, AGP_SIS_GATT_CNTRL, 0x05);

    agpsd->bridgesupported = TRUE;
}

static VOID sis_deinitialize_chipset(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE temp = 0;

    /* Disable GART */
    temp = readconfigbyte(bridgedev, AGP_SIS_APER_SIZE);
    writeconfigbyte(bridgedev, AGP_SIS_APER_SIZE, temp & ~3);
    
    /* Disable GATT */
    writeconfigbyte(bridgedev, AGP_SIS_GATT_CNTRL, 0x00);
}

static VOID sis_initialize_agp(struct agp_staticdata * agpsd)
{
    ULONG major, minor = 0;
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    
    /* Set default function pointers */
    agpsd->create_gatt_table = generic_create_gatt_table;
    agpsd->unbind_memory = generic_unbind_memory;
    agpsd->flush_gatt_table = NULL;
    agpsd->bind_memory = generic_bind_memory;
    agpsd->enable_agp = generic_enable_agp;
    agpsd->initialize_chipset = NULL;
    agpsd->deinitialize_chipset = NULL;
    agpsd->free_gatt_table = generic_free_gatt_table;
    agpsd->memmask = 0x00000000;
    agpsd->flush_chipset = generic_flush_chipset;
    
    /* Getting version info */ 
    major = (readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) >> 4) & 0xf;
    minor = readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) & 0xf;
    
    D(bug("[AGP] [SiS] Read config: AGP version %d.%d\n", major, minor));
        
    /* Getting mode */
    agpsd->bridgemode = readconfiglong(bridgedev, bridgeagpcap + AGP_STATUS_REG);
    
    D(bug("[AGP] [SiS] Reading mode: 0x%x\n", agpsd->bridgemode));

    /* In case of SiS only 3.5 bridges are guaranteed to be AGP3 compliant */
    if ((major == 3) && (minor >= 5))
    {
        /* Set specific function pointers */
        agpsd->flush_gatt_table = agp3_flush_gatt_table;
        agpsd->initialize_chipset = agp3_initialize_chipset;
        agpsd->deinitialize_chipset = agp3_deinitialize_chipset;
    }
    else
    {
        /* Set specific function pointers */
        agpsd->flush_gatt_table = sis_flush_gatt_table;
        agpsd->initialize_chipset = sis_initialize_chipset;
        agpsd->deinitialize_chipset = sis_deinitialize_chipset;
    }
    
    agpsd->initialize_chipset(agpsd);
}

/* VIA functions */
#define AGP_VIA_SEL             0xfd
#define AGP_VIA_GART_CTRL       0x80
#define AGP_VIA_APER_SIZE       0x84
#define AGP_VIA_GATT_BASE       0x88

#define AGP_VIA_AGP3_GART_CTRL  0x90
#define AGP_VIA_AGP3_APER_SIZE  0x94
#define AGP_VIA_AGP3_GATT_BASE  0x98

static VOID via_deinitialize_chipset(struct agp_staticdata * agpsd)
{
    /* NO OP (or set previous size of aperture?) */
}

static VOID via_flush_gatt_table(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    ULONG ctrlreg;
    ctrlreg = readconfiglong(bridgedev, AGP_VIA_GART_CTRL);
    ctrlreg |= (1<<7);
    writeconfiglong(bridgedev, AGP_VIA_GART_CTRL, ctrlreg);
    ctrlreg &= ~(1<<7);
    writeconfiglong(bridgedev, AGP_VIA_GART_CTRL, ctrlreg);
}

static VOID via_agp3_flush_gatt_table(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    ULONG ctrlreg;
    ctrlreg = readconfiglong(bridgedev, AGP_VIA_AGP3_GART_CTRL);
    writeconfiglong(bridgedev, AGP_VIA_AGP3_GART_CTRL, ctrlreg & ~(1<<7));
    writeconfiglong(bridgedev, AGP_VIA_AGP3_GART_CTRL, ctrlreg);
}

static VOID via_initialize_chipset(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE aperture_size_value = 0;
    
    /* Getting GART size */
    aperture_size_value = readconfigbyte(bridgedev, AGP_VIA_APER_SIZE);
    D(bug("[AGP] [VIA] Reading aperture size value: %x\n", aperture_size_value));
    
    switch(aperture_size_value)
    {
        case(255): agpsd->bridgeapersize = 1; break;
        case(254): agpsd->bridgeapersize = 2; break;
        case(252): agpsd->bridgeapersize = 4; break;
        case(248): agpsd->bridgeapersize = 8; break;
        case(240): agpsd->bridgeapersize = 16; break;
        case(224): agpsd->bridgeapersize = 32; break;
        case(192): agpsd->bridgeapersize = 64; break;
        case(128): agpsd->bridgeapersize = 128; break;
        case(0): agpsd->bridgeapersize = 256; break;
        default: agpsd->bridgeapersize = 0; break;
    }
    
    D(bug("[AGP] [VIA] Calculated aperture size: %d MB\n", (ULONG)agpsd->bridgeapersize));

    /* Creation of GATT table */
    agpsd->create_gatt_table(agpsd);
    
    /* Getting GART base */
    agpsd->bridgeaperbase = (IPTR)readconfiglong(bridgedev, AGP_APER_BASE);
    agpsd->bridgeaperbase &= (~0x0fUL) /* PCI_BASE_ADDRESS_MEM_MASK */;
    D(bug("[AGP] [VIA] Reading aperture base: 0x%x\n", (ULONG)agpsd->bridgeaperbase));

    /* TODO: Setting GART size (is it needed at all?) */

    /* GART control register */
    writeconfiglong(bridgedev, AGP_VIA_GART_CTRL, 0x0000000f);
        
    /* Set GATT pointer */
    writeconfiglong(bridgedev, AGP_VIA_GATT_BASE, 
        (((ULONG)agpsd->gatttable) & 0xfffff000) | 3);
    D(bug("[AGP] [VIA] Set GATT pointer to 0x%x\n", 
        (((ULONG)agpsd->gatttable) & 0xfffff000) | 3));
    
    agpsd->bridgesupported = TRUE;
}

static VOID via_agp3_initialize_chipset(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UWORD aperture_size_value = 0;
    ULONG ctrlreg = 0;
    
    /* Getting GART size */
    aperture_size_value = readconfigword(bridgedev, AGP_VIA_AGP3_APER_SIZE);
    aperture_size_value &= 0xfff;
    D(bug("[AGP] [VIA] Reading aperture size value: %x\n", aperture_size_value));
    
    switch(aperture_size_value)
    {
        case(0xf3f): agpsd->bridgeapersize = 4; break;
        case(0xf3e): agpsd->bridgeapersize = 8; break;
        case(0xf3c): agpsd->bridgeapersize = 16; break;
        case(0xf38): agpsd->bridgeapersize = 32; break;
        case(0xf30): agpsd->bridgeapersize = 64; break;
        case(0xf20): agpsd->bridgeapersize = 128; break;
        case(0xf00): agpsd->bridgeapersize = 256; break;
        case(0xe00): agpsd->bridgeapersize = 512; break;
        case(0xc00): agpsd->bridgeapersize = 1024; break;
        case(0x800): agpsd->bridgeapersize = 2048; break;
        default: agpsd->bridgeapersize = 0; break;
    }
    
    D(bug("[AGP] [VIA] Calculated aperture size: %d MB\n", (ULONG)agpsd->bridgeapersize));

    /* Creation of GATT table */
    agpsd->create_gatt_table(agpsd);
    
    /* Getting GART base */
    agpsd->bridgeaperbase = (IPTR)readconfiglong(bridgedev, AGP_APER_BASE);
    agpsd->bridgeaperbase &= (~0x0fUL) /* PCI_BASE_ADDRESS_MEM_MASK */;
    D(bug("[AGP] [VIA] Reading aperture base: 0x%x\n", (ULONG)agpsd->bridgeaperbase));

    /* TODO: Setting GART size (is it needed at all?) */
    
    /* Set GATT pointer */
    writeconfiglong(bridgedev, AGP_VIA_AGP3_GATT_BASE, 
        ((ULONG)agpsd->gatttable) & 0xfffff000);
    D(bug("[AGP] [VIA] Set GATT pointer to 0x%x\n", 
        ((ULONG)agpsd->gatttable) & 0xfffff000));
    
    /* Enabled GART and GATT */
    /* 1. Enable GTLB in RX90<7>, all AGP aperture access needs to fetch
     *    translation table first.
     * 2. Enable AGP aperture in RX91<0>. This bit controls the enabling of the
     *    graphics AGP aperture for the AGP3.0 port.
     */

    ctrlreg = readconfiglong(bridgedev, AGP_VIA_AGP3_GART_CTRL);
    writeconfiglong(bridgedev, AGP_VIA_AGP3_GART_CTRL,
        ctrlreg | (3<<7));
    
    D(bug("[AGP] [VIA] Enabled GART and GATT\n"));
    
    agpsd->bridgesupported = TRUE;
}

static VOID via_initialize_agp(struct agp_staticdata * agpsd)
{
    ULONG major, minor = 0;
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    
    /* Set default function pointers */
    agpsd->create_gatt_table = generic_create_gatt_table;
    agpsd->unbind_memory = generic_unbind_memory;
    agpsd->flush_gatt_table = NULL;
    agpsd->bind_memory = generic_bind_memory;
    agpsd->enable_agp = generic_enable_agp;
    agpsd->initialize_chipset = NULL;
    agpsd->deinitialize_chipset = NULL;
    agpsd->free_gatt_table = generic_free_gatt_table;
    agpsd->memmask = 0x00000000;
    agpsd->flush_chipset = generic_flush_chipset;

    /* Getting version info */ 
    major = (readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) >> 4) & 0xf;
    minor = readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) & 0xf;
    
    D(bug("[AGP] [VIA] Read config: AGP version %d.%d\n", major, minor));
        
    /* Getting mode */
    agpsd->bridgemode = readconfiglong(bridgedev, bridgeagpcap + AGP_STATUS_REG);
    
    D(bug("[AGP] [VIA] Reading mode: 0x%x\n", agpsd->bridgemode));

    /* By deafult set specific function pointer to pre 3.0 version */
    agpsd->flush_gatt_table = via_flush_gatt_table;
    agpsd->initialize_chipset = via_initialize_chipset;
    agpsd->deinitialize_chipset = via_deinitialize_chipset;
    
    if (major >= 3)
    {
        /* VIA AGP3 chipsets emulate pre 3.0 if in legacy mode */
        
        UBYTE reg = 0;
        reg = readconfigbyte(bridgedev, AGP_VIA_SEL);
        /* Set 3.0 functions if not in 2.0 compatibility mode */
        if ((reg & (1<<1)) == 0)
        {
            D(bug("[AGP] [VIA] 3.0 chipset working in 3.0 mode\n"));
            /* Set specific function pointers for 3.0 version*/
            agpsd->flush_gatt_table = via_agp3_flush_gatt_table;
            agpsd->initialize_chipset = via_agp3_initialize_chipset;
            agpsd->deinitialize_chipset = via_deinitialize_chipset;
        }
        else
        {
            D(bug("[AGP] [VIA] 3.0 chipset working in 2.0 emulation mode\n"));
        }
    }
        
    agpsd->initialize_chipset(agpsd);
}

/* Intel functions */
#define AGP_INTEL_NBXCFG        0x50
#define AGP_INTEL_CTRL          0xb0
#define AGP_INTEL_APER_SIZE     0xb4
#define AGP_INTEL_GATT_BASE     0xb8

#define AGP_INTEL_I845_AGPM     0x51
#define AGP_INTEL_I845_ERRSTS   0xc8

static VOID intel_845_initialize_chipset(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE aperture_size_value = 0;
    UBYTE agpm = 0;
    
    /* Getting GART size */
    aperture_size_value = readconfigbyte(bridgedev, AGP_INTEL_APER_SIZE);
    D(bug("[AGP] [Intel 845] Reading aperture size value: %x\n", aperture_size_value));
    
    switch(aperture_size_value)
    {
        case(63): agpsd->bridgeapersize = 4; break;
        case(62): agpsd->bridgeapersize = 8; break;
        case(60): agpsd->bridgeapersize = 16; break;
        case(56): agpsd->bridgeapersize = 32; break;
        case(48): agpsd->bridgeapersize = 64; break;
        case(32): agpsd->bridgeapersize = 128; break;
        case(0): agpsd->bridgeapersize = 256; break;
        default: agpsd->bridgeapersize = 0; break;
    }
    
    D(bug("[AGP] [Intel 845] Calculated aperture size: %d MB\n", (ULONG)agpsd->bridgeapersize));

    /* Creation of GATT table */
    agpsd->create_gatt_table(agpsd);
    
    /* Getting GART base */
    agpsd->bridgeaperbase = (IPTR)readconfiglong(bridgedev, AGP_APER_BASE);
    agpsd->bridgeaperbase &= (~0x0fUL) /* PCI_BASE_ADDRESS_MEM_MASK */;
    D(bug("[AGP] [Intel 845] Reading aperture base: 0x%x\n", (ULONG)agpsd->bridgeaperbase));

    /* Set GATT pointer */
    writeconfiglong(bridgedev, AGP_INTEL_GATT_BASE, (ULONG)agpsd->gatttable);
    D(bug("[AGP] [Intel 845] Set GATT pointer to 0x%x\n", (ULONG)agpsd->gatttable));
    
    /* Control register */
    writeconfiglong(bridgedev, AGP_INTEL_CTRL, 0x00000000);
    
    /* AGPM */
    agpm = readconfigbyte(bridgedev, AGP_INTEL_I845_AGPM);
    writeconfigbyte(bridgedev, AGP_INTEL_I845_AGPM, agpm | (1 << 1));
    
    /* Clear error conditions */
    writeconfigword(bridgedev, AGP_INTEL_I845_ERRSTS, 0x001c);
    
    agpsd->bridgesupported = TRUE;
}

static VOID intel_8XX_deinitialize_chipset(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UWORD cfgreg;
    
    cfgreg = readconfigword(bridgedev, AGP_INTEL_NBXCFG);
    writeconfigword(bridgedev, AGP_INTEL_NBXCFG, cfgreg & ~(1 << 9));
    /* TODO: set previous size of aperture */
}

static VOID intel_8XX_flush_gatt_table(struct agp_staticdata * agpsd)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    ULONG ctrlreg;
    
    ctrlreg = readconfiglong(bridgedev, AGP_INTEL_CTRL);
    writeconfiglong(bridgedev, AGP_INTEL_CTRL, ctrlreg & ~(1 << 7));
    ctrlreg = readconfiglong(bridgedev, AGP_INTEL_CTRL);
    writeconfiglong(bridgedev, AGP_INTEL_CTRL, ctrlreg | (1 << 7));
}

static VOID intel_830_flush_chipset(struct agp_staticdata * agpsd)
{
    flush_cpu_cache();   
}

static VOID intel_830_free_gatt_table(struct agp_staticdata * agpsd)
{
    /* This function is a NOOP */ 
}

static VOID intel_810_flush_gatt_table(struct agp_staticdata * agpsd)
{
    /* This function is a NOOP */
}

static VOID intel_810_enable_agp(struct agp_staticdata * agpsd, ULONG requestedmode)
{
    /* This function is a NOOP */
}


#define IS_845(x)                       \
(                                       \
    (x == 0x2570) || /* 82865_HB */     \
    (x == 0x1a30) || /* 82845_HB */     \
    (x == 0x2560) || /* 82845G_HB */    \
    (x == 0x358c) || /* 82854_HB */     \
    (x == 0x3340) || /* 82855PM_HB */   \
    (x == 0x3580) || /* 82855GM_HB */   \
    (x == 0x2578)    /* 82875_HB */     \
)                                       \

#define IS_915(x)                       \
(                                       \
    (x == 0x2588) || /* 915 */          \
    (x == 0x2580) || /* 82915G_HB */    \
    (x == 0x2590) || /* 82915GM_HB */   \
    (x == 0x2770) || /* 82945G_HB */    \
    (x == 0x27A0) || /* 82945GM_HB */   \
    (x == 0x27AC)    /* 82945GME_HB */  \
)                                       \

static VOID intel_initialize_agp(struct agp_staticdata * agpsd)
{
    ULONG major, minor = 0;
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    BOOL supporteddevicefound = FALSE;
    
    /* Set default function pointers */
    agpsd->create_gatt_table = generic_create_gatt_table;
    agpsd->unbind_memory = generic_unbind_memory;
    agpsd->flush_gatt_table = NULL;
    agpsd->bind_memory = generic_bind_memory;
    agpsd->enable_agp = generic_enable_agp;
    agpsd->initialize_chipset = NULL;
    agpsd->deinitialize_chipset = NULL;
    agpsd->free_gatt_table = generic_free_gatt_table;
    agpsd->memmask = 0x00000000;
    agpsd->flush_chipset = NULL;
    
    /* Getting version info */ 
    major = (readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) >> 4) & 0xf;
    minor = readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) & 0xf;
    
    D(bug("[AGP] [Intel] Read config: AGP version %d.%d\n", major, minor));
        
    /* Getting mode */
    agpsd->bridgemode = readconfiglong(bridgedev, bridgeagpcap + AGP_STATUS_REG);
    
    D(bug("[AGP] [Intel] Reading mode: 0x%x\n", agpsd->bridgemode));
    
    if (IS_845(agpsd->bridge->ProductID))
    {
        D(bug("[AGP] [Intel 845] Setting up pointers for 0x%x\n", agpsd->bridge->ProductID));
        agpsd->memmask = 0x00000017;
        agpsd->flush_gatt_table = intel_8XX_flush_gatt_table;
        agpsd->initialize_chipset = intel_845_initialize_chipset;
        agpsd->deinitialize_chipset = intel_8XX_deinitialize_chipset;
        agpsd->flush_chipset = intel_830_flush_chipset;
        supporteddevicefound = TRUE;
    }
    
    if (IS_915(agpsd->bridge->ProductID))
    {
        D(bug("[AGP] [Intel 915] Setting up pointers for 0x%x\n", agpsd->bridge->ProductID));
        /* TODO: fill functions */
        agpsd->create_gatt_table = NULL;
        agpsd->unbind_memory = NULL;
        agpsd->flush_gatt_table = intel_810_flush_gatt_table;
        agpsd->bind_memory = NULL;
        agpsd->enable_agp = intel_810_enable_agp;
        agpsd->initialize_chipset = NULL;
        agpsd->deinitialize_chipset = NULL;
        agpsd->free_gatt_table = intel_830_free_gatt_table;
        agpsd->memmask = 0x00000000;
        agpsd->flush_chipset = NULL;
        supporteddevicefound = TRUE;
    }
    
    if (supporteddevicefound)
    {
        agpsd->initialize_chipset(agpsd);
    }
    else
    {
        D(bug("[AGP] [Intel] Bridge 0x%x is not supported\n", agpsd->bridge->ProductID));
    }
}

/* Generic init functions */
AROS_UFH3(void, PciDevicesEnumerator,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(OOP_Object *, pciDevice, A2),
    AROS_UFHA(APTR, message, A1))
{
    AROS_USERFUNC_INIT

    IPTR class;
    UBYTE agpcapptr = aros_agp_hack_get_capabilities_ptr(pciDevice, CAPABILITY_AGP);
    struct agp_staticdata * agpsd = (struct agp_staticdata*)hook->h_Data;

    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Class, &class);
    
    /* Select bridges and AGP devices */
    if ((class == 0x06) || (agpcapptr))
    {
        struct PciAgpDevice * pciagpdev = 
            (struct PciAgpDevice *)AllocVec(sizeof(struct PciAgpDevice), MEMF_ANY | MEMF_CLEAR);
        IPTR temp;
        pciagpdev->PciDevice = pciDevice;
        pciagpdev->AgpCapability = agpcapptr;
        pciagpdev->Class = (UBYTE)class;
        OOP_GetAttr(pciDevice, aHidd_PCIDevice_VendorID, &temp);
        pciagpdev->VendorID = (UWORD)temp;
        OOP_GetAttr(pciDevice, aHidd_PCIDevice_ProductID, &temp);
        pciagpdev->ProductID = (UWORD)temp;
        
        AddTail(&agpsd->devices, (struct Node *)pciagpdev);
    }
    

    AROS_USERFUNC_EXIT
}

static VOID scan_pci_devices(struct agp_staticdata * agpsd)
{
    if (pciBus_AGP)
    {
        struct Hook FindHook = {
        h_Entry:    (IPTR (*)())PciDevicesEnumerator,
        h_Data:     agpsd,
        };

        struct TagItem Requirements[] = {
        { TAG_DONE,             0UL }
        };
    
        struct pHidd_PCI_EnumDevices enummsg = {
        mID:        OOP_GetMethodID(IID_Hidd_PCI, moHidd_PCI_EnumDevices),
        callback:   &FindHook,
        requirements:   (struct TagItem*)&Requirements,
        }, *msg = &enummsg;

        OOP_DoMethod(pciBus_AGP, (OOP_Msg)msg);
    }         
}

static VOID display_device(struct PciAgpDevice * pciagpdev)
{
    bug("[AGP]     VendorID:0x%x/ProductID:0x%x AGP:%s\n", 
        pciagpdev->VendorID, pciagpdev->ProductID, 
        pciagpdev->AgpCapability ? "YES" : "NO");
}

static VOID display_found_devices(struct agp_staticdata * agpsd)
{
    struct PciAgpDevice * pciagpdev = NULL;
    
    bug("[AGP] Found bridges:\n");
    ForeachNode(&agpsd->devices, pciagpdev)
    {
        if (pciagpdev->Class == 0x06)
            display_device(pciagpdev);
    }
    
    bug("[AGP] Found AGP devices:\n"); 
    ForeachNode(&agpsd->devices, pciagpdev)
    {
        if (pciagpdev->AgpCapability)
            display_device(pciagpdev);
    }
    
    bug("[AGP] Selected AGP bridge:\n");
    if (agpsd->bridge)
        display_device(agpsd->bridge);
    else
        bug("[AGP]     NONE\n");

    bug("[AGP] Selected AGP video card:\n");
    if (agpsd->videocard)
        display_device(agpsd->videocard);
    else
        bug("[AGP]     NONE\n");

}

static VOID select_bridge_and_videocard(struct agp_staticdata * agpsd)
{
    struct PciAgpDevice * pciagpdev = NULL;
    
    ForeachNode(&agpsd->devices, pciagpdev)
    {
        /* Select bridge */
        if ((!agpsd->bridge) && (pciagpdev->Class == 0x06) && 
            (pciagpdev->AgpCapability))
        {
            agpsd->bridge = pciagpdev;
        }

        /* Select video card */
        if ((!agpsd->videocard) && (pciagpdev->Class == 0x03) &&
            (pciagpdev->AgpCapability))
        {
            agpsd->videocard = pciagpdev;
        }

        /* FIXME: HACK for selecting Intel integrated bridge */
        if ((!agpsd->bridge) && (IS_915(pciagpdev->ProductID)))
        {
            agpsd->bridge = pciagpdev;
        }
    }
}

static VOID initialize_static_data(struct agp_staticdata * agpsd)
{
    agpsd->bridge = NULL;
    agpsd->videocard = NULL;
    NEWLIST(&agpsd->devices);
    InitSemaphore(&agpsd->driverlock);
    agpsd->enabled = FALSE;
    agpsd->bridgesupported = FALSE;
    agpsd->bridgemode = 0;
    agpsd->bridgeaperbase = 0x0;
    agpsd->bridgeapersize = 0;
    agpsd->gatttablebuffer = NULL;
    agpsd->gatttable = NULL;
    agpsd->scratchmembuffer = NULL;
    agpsd->scratchmem = NULL;
    agpsd->memmask = 0x00000000;
    
    agpsd->create_gatt_table = NULL;
    agpsd->unbind_memory = NULL;
    agpsd->flush_gatt_table = NULL;
    agpsd->bind_memory = NULL;
    agpsd->enable_agp = NULL;
    agpsd->initialize_chipset = NULL;
    agpsd->deinitialize_chipset = NULL;
    agpsd->free_gatt_table = NULL;
    agpsd->flush_chipset = NULL;
}

/* Init/deinit */
struct agp_staticdata agpsd_global;

static BOOL is_agp_disabled_by_env()
{
    TEXT buffer[128] = {0};
    
    if (GetVar("NOAGP", buffer, 128, LV_VAR) == -1)
        return FALSE;
    else
        return TRUE;
}

static int aros_agp_hack_init_agp(void)
{
    if (!OOPBase_AGP)
    {
        if ((OOPBase_AGP=OpenLibrary("oop.library", 0)) != NULL)
        {
            __IHidd_PCIDev_AGP = OOP_ObtainAttrBase(IID_Hidd_PCIDevice);
            pciBus_AGP = OOP_NewObject(NULL, CLID_Hidd_PCI, NULL);
        }
        else
        {
            D(bug("[AGP] Could not open oop.library\n"));
            return 1;
        }
    }

    initialize_static_data(&agpsd_global);
    
    scan_pci_devices(&agpsd_global);
    
    select_bridge_and_videocard(&agpsd_global);
    
    display_found_devices(&agpsd_global);

    if (is_agp_disabled_by_env())
    {
        bug("[AGP] Disabled by environment setting\n");
        return 1;
    }
    
    if (agpsd_global.bridge)
    {
        switch(agpsd_global.bridge->VendorID)
        {
            case(0x1039): /* SiS */
            sis_initialize_agp(&agpsd_global);
            break;
            case(0x1106): /* VIA */
            via_initialize_agp(&agpsd_global);
            break;
            case(0x8086): /* Intel */
            intel_initialize_agp(&agpsd_global);
            break;
        }
    }
    
    
    if (!aros_agp_hack_is_agp_bridge_present())
    {
        bug("[AGP] No supported AGP bridge found\n");
    }

    return 1;
}

static int aros_agp_hack_deinit_agp()
{
    struct PciAgpDevice * pciagpdev = NULL;

    if (agpsd_global.deinitialize_chipset)
        agpsd_global.deinitialize_chipset(&agpsd_global);

    if (agpsd_global.free_gatt_table)
        agpsd_global.free_gatt_table(&agpsd_global);
    
    /* Free scanned device information */
    while((pciagpdev = (struct PciAgpDevice *)RemHead(&agpsd_global.devices)) != NULL)
        FreeVec(pciagpdev);

    if (pciBus_AGP)
    {
        OOP_DisposeObject(pciBus_AGP);
        pciBus_AGP = NULL;
    }

    if (__IHidd_PCIDev_AGP != 0)
    {
        OOP_ReleaseAttrBase(IID_Hidd_PCIDevice);
        __IHidd_PCIDev_AGP = 0;
    }
    
    if (OOPBase_AGP)
    {
        CloseLibrary(OOPBase_AGP);
        OOPBase_AGP = NULL;
    }
    
    return 1;
}

/* (Possible) public interface of (future) agp.hidd */
OOP_Object * aros_agp_hack_get_agp_bridge()
{
    /* TODO: This method might not be needed as returned object is never passed back */
    return agpsd_global.bridge->PciDevice;
}

BOOL aros_agp_hack_is_agp_bridge_present()
{
    return agpsd_global.bridgesupported;
}

ULONG aros_agp_hack_get_bridge_mode()
{
    return agpsd_global.bridgemode;
}

IPTR aros_agp_hack_get_bridge_aperture_base()
{
    return agpsd_global.bridgeaperbase;
}

ULONG aros_agp_hack_get_bridge_aperture_size()
{
    return (ULONG)agpsd_global.bridgeapersize;
}

VOID aros_agp_hack_unbind_memory(ULONG offset, ULONG size)
{
    agpsd_global.unbind_memory(&agpsd_global, offset, size);
}

VOID aros_agp_hack_bind_memory(IPTR address, ULONG size, ULONG offset)
{
    agpsd_global.bind_memory(&agpsd_global, address, size, offset);
}

VOID aros_agp_hack_enable_agp(ULONG mode)
{
    agpsd_global.enable_agp(&agpsd_global, mode);
}

VOID aros_agp_hack_flush_chipset()
{
    agpsd_global.flush_chipset(&agpsd_global);
}

ADD2INIT(aros_agp_hack_init_agp, 4);
ADD2EXIT(aros_agp_hack_deinit_agp, 4);
