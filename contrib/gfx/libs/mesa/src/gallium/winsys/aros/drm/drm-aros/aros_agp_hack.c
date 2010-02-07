#include "aros_agp_hack.h"

#include <proto/oop.h>
#include <hidd/pci.h>
#include <hidd/hidd.h>
#include <aros/libcall.h>
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
    /* TODO: Semaphore */
    struct List         devices;        /* Bridges and AGP devices in system */

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

    /* Video card data */
    struct PciAgpDevice *videocard;     /* Selected AGP card */
    
    /* HACK */
    ULONG               *hackbuffer;    /* Use for CPU flush hack */
    ULONG               hackbuffersize;
    /* HACK */
    
    /* Functions */
    VOID    (*create_gatt_table)(struct agp_staticdata * agpsd);
    VOID    (*unbind_memory)(struct agp_staticdata * agpsd, ULONG offset, ULONG size);
    VOID    (*flush_gatt_table)(struct agp_staticdata *agpsd);
    VOID    (*bind_memory)(struct agp_staticdata * agpsd, IPTR address, ULONG size, ULONG offset);
    VOID    (*enable_agp)(struct agp_staticdata * agpsd, ULONG mode);
    VOID    (*initialize_chipset)(struct agp_staticdata * agpsd);
    VOID    (*deinitialize_chipset)(struct agp_staticdata * agpsd);
    VOID    (*free_gatt_table)(struct agp_staticdata * agpsd);
};

#include <stdio.h>

#define LOG_MSG(x, ...)             \
    {                               \
        bug("[AGP] "x, ##__VA_ARGS__);      \
        printf("[AGP] "x, ##__VA_ARGS__);   \
    }

//#define LOG_MSG(x, ...)

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
#define AGP_STATUS_REG_AGP3_X4          (1<<0)
#define AGP_STATUS_REG_AGP3_X8          (1<<1)
#define AGP_COMMAND_REG                 0x08
#define AGP_CTRL_REG                    0x10
#define AGP_CTRL_REG_GTBLEN             (1<<7)
#define AGP_CTRL_REG_APEREN             (1<<8)
#define AGP_APER_SIZE_REG               0x14
#define AGP_GART_CTRL_LO_REG            0x18

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
    
    /* Getting GART base */
    agpsd->bridgeaperbase = (IPTR)readconfiglong(bridgedev, AGP_APER_BASE);
    agpsd->bridgeaperbase &= (~0x0fUL) /* PCI_BASE_ADDRESS_MEM_MASK */;
    LOG_MSG("Reading aperture base: 0x%x\n", (ULONG)agpsd->bridgeaperbase);

    /* Getting GART size */
    aperture_size_value = readconfigword(bridgedev, bridgeagpcap + AGP_APER_SIZE_REG);
    LOG_MSG("Reading aperture size value: %d\n", aperture_size_value);
    
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
    
    LOG_MSG("Calculated aperture size: %d MB\n", (ULONG)agpsd->bridgeapersize);

    /* Creation of GATT table */
    agpsd->create_gatt_table(agpsd);
    
    /* TODO: Setting GART size (is it needed at all?) */
    
    /* Set GATT pointer */
    writeconfiglong(bridgedev, bridgeagpcap + AGP_GART_CTRL_LO_REG,
        (ULONG)agpsd->gatttable);
    LOG_MSG("Set GATT pointer to 0x%x\n", (ULONG)agpsd->gatttable);
    
    /* Enabled GART and GATT */
    ctrlreg = readconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG);
    writeconfiglong(bridgedev, bridgeagpcap + AGP_CTRL_REG,
        ctrlreg | AGP_CTRL_REG_APEREN | AGP_CTRL_REG_GTBLEN);
    
    LOG_MSG("Enabled GART and GATT\n");
    
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
static VOID generic_create_gatt_table(struct agp_staticdata * agpsd)
{
    /* Create a table that will hold a certain number of 32bit pointers */
    ULONG entries = agpsd->bridgeapersize * 1024 * 1024 / 4096;
    ULONG tablesize =  entries * 4;
    ULONG i = 0;
    
    agpsd->scratchmembuffer = AllocVec(4096 * 2, MEMF_PUBLIC | MEMF_CLEAR);
    agpsd->scratchmem = (ULONG*)(ALIGN((IPTR)agpsd->scratchmembuffer, 4096));
    LOG_MSG("Created scratch memory at 0x%x\n", (ULONG)agpsd->scratchmem);

    
    agpsd->gatttablebuffer = AllocVec(tablesize + 4096, MEMF_PUBLIC | MEMF_CLEAR);
    agpsd->gatttable = (ULONG *)(ALIGN((ULONG)agpsd->gatttablebuffer, 4096));
    
    LOG_MSG("Created GATT table size %d at 0x%x\n", tablesize, 
        (ULONG)agpsd->gatttable);
    
    for (i = 0; i < entries; i++)
    {
        writel((ULONG)agpsd->scratchmem, agpsd->gatttable + i);
        readl(agpsd->gatttable + i);	/* PCI Posting. */
    }
}

static VOID generic_free_gatt_table(struct agp_staticdata * agpsd)
{
    LOG_MSG("Freeing GATT table 0x%x, scratch memory 0x%x\n",
        (ULONG)agpsd->gatttable, (ULONG)agpsd->scratchmem);

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

#if 0
    LOG_MSG("Unbind offset %d, size %d\n", offset, size);
#endif
    if (size == 0)
        return;

    /* TODO: get mask type */

    /* Remove entries from GATT table */
    for(i = 0; i < size / 4096; i++)
    {
        writel((ULONG)agpsd->scratchmem, agpsd->gatttable + offset + i);
    }
    
    readl(agpsd->gatttable + offset + i - 1); /* PCI posting */
    
    /* Flush GATT table */
    agpsd->flush_gatt_table(agpsd);
}

static inline VOID clflush(volatile void * ptr)
{
    asm volatile("clflush %0" : "+m" (*(volatile BYTE *) ptr));
}

static VOID generic_bind_memory(struct agp_staticdata * agpsd, IPTR address, ULONG size, ULONG offset)
{
    ULONG i;

#if 0    
    LOG_MSG("Bind address 0x%x into offset %d, size %d\n", (ULONG)address, offset, size);
#endif

    /* TODO: check if offset + size / 4096 ends before gatt_table end */
    
    /* TODO: get mask type */
    
    /* TODO: Check if each entry in GATT to be written in unbound (might not work due to CPU caches problem - see HACK*/
    
    /* Flush memory */
    for (i = 0; i < size; i += 32 /* Cache line size on x86 */)
        clflush((APTR)(address + i));
    
    /* Insert entries into GATT table */
    for(i = 0; i < size / 4096; i++)
    {
        /* TODO: mask memory */
        writel(address + (4096 * i), agpsd->gatttable + offset + i);
    }
    
    readl(agpsd->gatttable + offset + i - 1); /* PCI posting */
    
    /* Flush GATT table */
    agpsd->flush_gatt_table(agpsd);

    {
        /* THIS IS A NASTY HACK
           Somehow data written in GATT is not refreshed at video card -
           this probably happens due to some CPU cache problem.
           Executing the code below makes the GATT table have correct data.
         */
        ULONG i = 0;
        for (i = 0; i < agpsd->hackbuffersize / 4; i++)
            *(agpsd->hackbuffer + i) = 0xdeadbeef;
    }
}

static ULONG generic_calibrate_modes(ULONG requestedmode, ULONG bridgemode, ULONG vgamode)
{
    ULONG calibratedmode = bridgemode;
    ULONG temp = 0;
    
    /* TODO: Apply reserved mask to requestedmode */
    
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

static ULONG generic_select_best_mode(struct agp_staticdata * agpsd, ULONG requestedmode, ULONG bridgemode)
{
    OOP_Object * videodev = agpsd->videocard->PciDevice;
    UBYTE videoagpcap = agpsd->videocard->AgpCapability;    
    ULONG vgamode = 0;

    /* Get VGA card capability */
    vgamode = readconfiglong(videodev, videoagpcap + AGP_STATUS_REG);
    
    LOG_MSG("    VGA mode 0x%x\n", vgamode);
    
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
        
    if (bridgemode & AGP_STATUS_REG_AGP_3_0)
    {
        bridgemode = generic_calibrate_modes(requestedmode, bridgemode, vgamode);
    }
    else
    {
        LOG_MSG("Select best mode: AGP2 mode not supported\n");
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
            if (1) /* FIXME: detection of AGP3 */
                mode *= 4;
            
            LOG_MSG("Set AGP%d device 0x%x/0x%x to speed %dx\n", 
                TRUE /* FIXME: detection of AGP3 */ ? 3 : 2, 
                pciagpdev->VendorID, pciagpdev->ProductID, mode);
            
            writeconfiglong(pciagpdev->PciDevice, pciagpdev->AgpCapability + AGP_COMMAND_REG, status);            
        }
    }
}

static VOID generic_enable_agp(struct agp_staticdata * agpsd, ULONG requestedmode)
{
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    ULONG bridgemode = 0;
    
    LOG_MSG("Enable AGP:\n");
    LOG_MSG("    Requested mode 0x%x\n", requestedmode);
    
    bridgemode = readconfiglong(bridgedev, bridgeagpcap + AGP_STATUS_REG);
    LOG_MSG("    Bridge mode 0x%x\n", requestedmode);
    
    bridgemode = generic_select_best_mode(agpsd, requestedmode, bridgemode);
    
    bridgemode |= AGP_STATUS_REG_AGP_ENABLED;
    
    LOG_MSG("Mode to write: 0x%x\n", bridgemode);
    
    if (1) /* FIXME: detection of AGP3 */
    {
        /* TODO: agp_3_5_enable */
        generic_send_command(agpsd, bridgemode);
        
        /* TODO: Handling of situation when version is 3 but bridge is not in mode 3 */
    }
    else
    {
        LOG_MSG("Enable AGP: AGP2 mode not supported\n");
    }
}

/* SiS functions */
static VOID sis_initialize_agp(struct agp_staticdata * agpsd)
{
    ULONG major, minor = 0;
    OOP_Object * bridgedev = agpsd->bridge->PciDevice;
    UBYTE bridgeagpcap = agpsd->bridge->AgpCapability;
    BOOL bridgeisagp3 = FALSE;
    
    /* Set default function pointers */
    agpsd->create_gatt_table = generic_create_gatt_table;
    agpsd->unbind_memory = generic_unbind_memory;
    agpsd->flush_gatt_table = NULL;
    agpsd->bind_memory = generic_bind_memory;
    agpsd->enable_agp = generic_enable_agp;
    agpsd->initialize_chipset = NULL;
    agpsd->deinitialize_chipset = NULL;
    agpsd->free_gatt_table = generic_free_gatt_table;
    
    /* Getting version info */ 
    major = (readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) >> 4) & 0xf;
    minor = readconfigbyte(bridgedev, bridgeagpcap + AGP_VERSION_REG) & 0xf;
    
    LOG_MSG("Read config: AGP version %d.%d\n", major, minor);
    bridgeisagp3 = (major == 3 && minor >= 5);
        
    /* Getting mode */
    agpsd->bridgemode = readconfiglong(bridgedev, bridgeagpcap + AGP_STATUS_REG);
    
    LOG_MSG("Reading mode: 0x%x\n", agpsd->bridgemode);

    if (bridgeisagp3)
    {
        /* Set specific function pointers */
        agpsd->flush_gatt_table = agp3_flush_gatt_table;
        agpsd->initialize_chipset = agp3_initialize_chipset;
        agpsd->deinitialize_chipset = agp3_deinitialize_chipset;
    }
    else
    {
        /* TODO: Set specific function pointers */
        LOG_MSG("SiS Init: AGP2 mode not supported\n");
        return;
    }
    
    agpsd->initialize_chipset(agpsd);
}

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
    LOG_MSG("    VendorID:0x%x/ProductID:0x%x AGP:%s\n", 
        pciagpdev->VendorID, pciagpdev->ProductID, 
        pciagpdev->AgpCapability ? "YES" : "NO");
}

static VOID display_found_devices(struct agp_staticdata * agpsd)
{
    struct PciAgpDevice * pciagpdev = NULL;
    
    LOG_MSG("Found bridges:\n");
    ForeachNode(&agpsd->devices, pciagpdev)
    {
        if (pciagpdev->Class == 0x06)
            display_device(pciagpdev);
    }
    
    LOG_MSG("Found AGP devices:\n"); 
    ForeachNode(&agpsd->devices, pciagpdev)
    {
        if (pciagpdev->AgpCapability)
            display_device(pciagpdev);
    }
    
    LOG_MSG("Selected AGP bridge:\n");
    if (agpsd->bridge)
        display_device(agpsd->bridge);
    else
        LOG_MSG("    NONE\n");

    LOG_MSG("Selected AGP video card:\n");
    if (agpsd->videocard)
        display_device(agpsd->videocard);
    else
        LOG_MSG("    NONE\n");

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
    }
}

static VOID initialize_static_data(struct agp_staticdata * agpsd)
{
    agpsd->bridge = NULL;
    agpsd->videocard = NULL;
    NEWLIST(&agpsd->devices);
    agpsd->bridgesupported = FALSE;
    agpsd->bridgemode = 0;
    agpsd->bridgeaperbase = 0x0;
    agpsd->bridgeapersize = 0;
    agpsd->gatttablebuffer = NULL;
    agpsd->gatttable = NULL;
    agpsd->scratchmembuffer = NULL;
    agpsd->scratchmem = NULL;
    agpsd->create_gatt_table = NULL;
    agpsd->unbind_memory = NULL;
    agpsd->flush_gatt_table = NULL;
    agpsd->bind_memory = NULL;
    agpsd->enable_agp = NULL;
    agpsd->initialize_chipset = NULL;
    agpsd->deinitialize_chipset = NULL;
    agpsd->free_gatt_table = NULL;
    
    /* HACK */
    agpsd->hackbuffersize = 512 * 1024;
    agpsd->hackbuffer = AllocMem(agpsd->hackbuffersize, MEMF_ANY);
    /* HACK */
}

/* Init/deinit */
struct agp_staticdata agpsd_global;

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
            LOG_MSG("Could not open oop.library\n");
            return 1;
        }
    }
    
    initialize_static_data(&agpsd_global);
    
    scan_pci_devices(&agpsd_global);
    
    select_bridge_and_videocard(&agpsd_global);
    
    display_found_devices(&agpsd_global);
    
    if (agpsd_global.bridge)
    {
        switch(agpsd_global.bridge->VendorID)
        {
            case(0x1039): /*SiS */
            sis_initialize_agp(&agpsd_global);
            break;
        }
    }
    
    
    if (!aros_agp_hack_is_agp_bridge_present())
    {
        LOG_MSG("No supported AGP bridge found\n");
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

    /* HACK */
    if (agpsd_global.hackbuffer)
        FreeMem(agpsd_global.hackbuffer, agpsd_global.hackbuffersize);
    /* HACK */
        
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

ADD2INIT(aros_agp_hack_init_agp, 4);
ADD2EXIT(aros_agp_hack_deinit_agp, 4);
