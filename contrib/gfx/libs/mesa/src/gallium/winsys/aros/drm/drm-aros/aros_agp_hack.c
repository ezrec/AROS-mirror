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

/* AGP Bridge Detection */
#include <stdio.h>
BOOL listing_mode = FALSE;

/* Bridge data */
OOP_Object * agp_bridge = NULL;
ULONG mode = 0;
IPTR aperture_base = (IPTR)NULL;
IPTR gatt_base = (IPTR)NULL;
ULONG * gatt_table = NULL;
ULONG aperture_size = 0;
BOOL bridge_present = FALSE;
APTR scratch_memory = NULL;
BOOL isagp3 = FALSE;
UBYTE capptr = 0;
UBYTE vgacapptr = 0;
OOP_Object * vga_card = NULL;

/*
#define LOG_MSG(x, ...)         \
    bug(x, ##__VA_ARGS__);      \
    printf(x, ##__VA_ARGS__);
*/
#define LOG_MSG(x, ...)

/* Accessors */
OOP_Object * aros_agp_hack_get_agp_bridge()
{
    return agp_bridge;
}

BOOL aros_agp_hack_is_agp_bridge_present()
{
    return bridge_present;
}

ULONG aros_agp_hack_get_bridge_mode()
{
    return mode;
}

IPTR aros_agp_hack_get_bridge_aperture_base()
{
    return aperture_base;
}

ULONG aros_agp_hack_get_bridge_aperture_size()
{
    return aperture_size;
}

#define writel(val, addr)               (*(volatile ULONG*)(addr) = (val))
#define readl(addr)                     (*(volatile ULONG*)(addr))
#define min(a,b)                        ((a) < (b) ? (a) : (b))
#define max(a,b)                        ((a) > (b) ? (a) : (b))

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


AROS_UFH3(void, AGPDevicesEnumerator,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(OOP_Object *, pciDevice, A2),
    AROS_UFHA(APTR, message, A1))
{
    AROS_USERFUNC_INIT

    IPTR ProductID;
    IPTR VendorID;
    UBYTE agpcapptr = aros_agp_hack_get_capabilities_ptr(pciDevice, CAPABILITY_AGP);

    OOP_GetAttr(pciDevice, aHidd_PCIDevice_ProductID, &ProductID);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_VendorID, &VendorID);

    if (agpcapptr)
    {
        ULONG status = *((ULONG*)hook->h_Data);
        ULONG mode = status;
        
        mode &= 0x7;
        if (isagp3)
            mode *= 4;
        
        LOG_MSG("Set AGP%d device 0x%x/0x%x to speed %dx\n", 
            isagp3 ? 3 : 2, (ULONG)VendorID, (ULONG)ProductID, mode);
        
        writeconfiglong(pciDevice, agpcapptr + AGP_COMMAND_REG, status);
    } 

    AROS_USERFUNC_EXIT
}

static void aros_agp_hack_send_command(ULONG status)
{
    struct Hook FindHook = {
    h_Entry:    (IPTR (*)())AGPDevicesEnumerator,
    h_Data:     &status,
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

AROS_UFH3(void, AGPVideoCardsEnumerator,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(OOP_Object *, pciDevice, A2),
    AROS_UFHA(APTR, message, A1))
{
    AROS_USERFUNC_INIT

    IPTR ProductID;
    IPTR VendorID;

    if (vgacapptr != 0)
        return;

    vgacapptr = aros_agp_hack_get_capabilities_ptr(pciDevice, CAPABILITY_AGP);
    if (vgacapptr != 0)
    {
        OOP_GetAttr(pciDevice, aHidd_PCIDevice_ProductID, &ProductID);
        OOP_GetAttr(pciDevice, aHidd_PCIDevice_VendorID, &VendorID);

        vga_card = pciDevice;
        
        LOG_MSG("    Found AGP VGA 0x%x/0x%x\n", (ULONG)VendorID, (ULONG)ProductID);
    }

    AROS_USERFUNC_EXIT
}

static ULONG aros_agp_hack_calibrate(ULONG requestedmode, ULONG bridgemode, ULONG vgamode)
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
static ULONG aros_agp_hack_select_best_mode(ULONG requestedmode, ULONG bridgemode)
{
    struct Hook FindHook = {
    h_Entry:    (IPTR (*)())AGPVideoCardsEnumerator,
    h_Data:     NULL,
    };

    struct TagItem Requirements[] = {
    { tHidd_PCI_Class,      0x03 },
    { TAG_DONE,             0UL }
    };

    struct pHidd_PCI_EnumDevices enummsg = {
    mID:        OOP_GetMethodID(IID_Hidd_PCI, moHidd_PCI_EnumDevices),
    callback:   &FindHook,
    requirements:   (struct TagItem*)&Requirements,
    }, *msg = &enummsg;
    
    ULONG vgamode = 0;

    OOP_DoMethod(pciBus_AGP, (OOP_Msg)msg);
    
    /* Get VGA card capability */
    vgamode = readconfiglong(vga_card, vgacapptr + AGP_STATUS_REG);
    
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
        bridgemode = aros_agp_hack_calibrate(requestedmode, bridgemode, vgamode);
    }
    else
    {
        LOG_MSG("Select best mode: AGP2 mode not supported\n");
    }
    
    return bridgemode;
}

static void aros_agp_hack_enable_agp_generic(ULONG requestedmode)
{
    ULONG bridgemode = 0;
    
    LOG_MSG("Enable AGP:\n");
    LOG_MSG("    Requested mode 0x%x\n", requestedmode);
    
    bridgemode = readconfiglong(agp_bridge, capptr + AGP_STATUS_REG);
    LOG_MSG("    Bridge mode 0x%x\n", requestedmode);
    
    bridgemode = aros_agp_hack_select_best_mode(requestedmode, bridgemode);
    
    bridgemode |= AGP_STATUS_REG_AGP_ENABLED;
    
    LOG_MSG("Mode to write: 0x%x\n", bridgemode);
    
    if (isagp3)
    {
        /* TODO: agp_3_5_enable */
        aros_agp_hack_send_command(bridgemode);
        
        /* TODO: Handling of situation when version is 3 but bridge is not in mode 3 */
    }
    else
    {
        LOG_MSG("Enable AGP: AGP2 mode not supported\n");
    }
}

VOID aros_agp_hack_enable_agp(ULONG mode)
{
    aros_agp_hack_enable_agp_generic(mode);
}



static void aros_agp_hack_tblflush_generic_agp3()
{
    ULONG ctrlreg;
    ctrlreg = readconfiglong(agp_bridge, capptr + AGP_CTRL_REG);
    writeconfiglong(agp_bridge, capptr + AGP_CTRL_REG, ctrlreg & ~AGP_CTRL_REG_GTBLEN);
    writeconfiglong(agp_bridge, capptr + AGP_CTRL_REG, ctrlreg);
}

static inline void clflush(volatile void * ptr)
{
    asm volatile("clflush %0" : "+m" (*(volatile BYTE *) ptr));
}

static void aros_agp_hack_bind_memory_generic(IPTR address, ULONG size, ULONG offset)
{
    ULONG i;
    
//    LOG_MSG("Bind address 0x%x into offset %d, size %d\n", (ULONG)address, offset, size);

    /* TODO: check if offset + size / 4096 ends before gatt_table end */
    
    /* TODO: get mask type */
    
    /* TODO: Check if each entry in GATT to be written in unbound */
    
    /* Flush memory */
    for (i = 0; i < size; i += 32 /* Cache line size on x86 */)
        clflush((APTR)(address + i));
    
    /* Insert entries into GATT table */
    for(i = 0; i < size / 4096; i++)
    {
        /* TODO: mask memory */
        writel(address + (4096 * i), gatt_table + offset + i);
    }
    
    readl(gatt_table + offset + i - 1); /* PCI posting */
    
    /* Flush GATT table */
    if (isagp3)
    {
        aros_agp_hack_tblflush_generic_agp3();

        {
            /* THIS IS A NASTY HACK
               Somehow data written in GATT is not refreshed at video card -
               this probably happens due to some CPU cache problem.
               Executing the code below makes the GATT table have correct data,
               but it will cause memory fragmentation at a long term - a proper
               solution needs to be found
             */
            ULONG sized = 512 * 1024;
            ULONG * d = (ULONG*)AllocVec(sized, MEMF_ANY);
            ULONG i = 0;
            for (i = 0; i < sized / 4; i++)
                *(d+i) = 0xffeeddcc;
            FreeVec((APTR)d);
        }
    }
    else
    {
        LOG_MSG("Bind Memory: AGP2 mode not supported\n");
    }
    

}

VOID aros_agp_hack_bind_memory(IPTR address, ULONG size, ULONG offset)
{
    aros_agp_hack_bind_memory_generic(address, size, offset);
}

static void aros_agp_hack_unbind_memory_generic(ULONG offset, ULONG size)
{
    ULONG i;

//    LOG_MSG("Unbind offset %d, size %d\n", offset, size);
    if (size == 0)
        return;

    /* TODO: get mask type */

    /* Remove entries from GATT table */
    for(i = 0; i < size / 4096; i++)
    {
        writel((ULONG)scratch_memory, gatt_table + offset + i);
    }
    
    readl(gatt_table + offset + i - 1); /* PCI posting */
    
    /* Flush GATT table */
    if (isagp3)
    {
        aros_agp_hack_tblflush_generic_agp3();
    }
    else
    {
        LOG_MSG("Bind Memory: AGP2 mode not supported\n");
    }    
}

VOID aros_agp_hack_unbind_memory(ULONG offset, ULONG size)
{
    aros_agp_hack_unbind_memory_generic(offset, size);
}

/* Init code */
AROS_UFH3(void, AGPBridgeEnumerator,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(OOP_Object *, pciDevice, A2),
    AROS_UFHA(APTR, message, A1))
{
    AROS_USERFUNC_INIT

    IPTR ProductID;
    IPTR VendorID;
    BOOL isAGP = aros_agp_hack_device_is_agp(pciDevice);


    OOP_GetAttr(pciDevice, aHidd_PCIDevice_ProductID, &ProductID);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_VendorID, &VendorID);
    
    if (listing_mode)
    {
        LOG_MSG("Listing bridge: 0x%x/0x%x -> %s\n", 
            (UWORD)VendorID, (UWORD)ProductID, isAGP ? "AGP" : "NOT AGP");
    }
    
    if (isAGP && !agp_bridge && !listing_mode)
    {
        LOG_MSG("Selecting device 0x%x/0x%x as AGP bridge\n",
            (UWORD)VendorID, (UWORD)ProductID);

        agp_bridge = pciDevice;
    }

    AROS_USERFUNC_EXIT
}

static BOOL aros_agp_hack_find_supported_agp_bridge(UWORD VendorID)
{
    if (pciBus_AGP)
    {
        struct Hook FindHook = {
        h_Entry:    (IPTR (*)())AGPBridgeEnumerator,
        h_Data:     NULL,
        };

        struct TagItem Requirements[] = {
        { tHidd_PCI_Class,      0x06 },
        { tHidd_PCI_VendorID,   VendorID },
        { TAG_DONE,             0UL }
        };
    
        struct pHidd_PCI_EnumDevices enummsg = {
        mID:        OOP_GetMethodID(IID_Hidd_PCI, moHidd_PCI_EnumDevices),
        callback:   &FindHook,
        requirements:   (struct TagItem*)&Requirements,
        }, *msg = &enummsg;

        listing_mode = FALSE;
        OOP_DoMethod(pciBus_AGP, (OOP_Msg)msg);
    }
    
    return agp_bridge != NULL;
}

static void aros_agp_hack_list_all_bridges()
{
    if (pciBus_AGP)
    {
        struct Hook FindHook = {
        h_Entry:    (IPTR (*)())AGPBridgeEnumerator,
        h_Data:     NULL,
        };

        struct TagItem Requirements[] = {
        { tHidd_PCI_Class,      0x06 },
        { TAG_DONE,             0UL }
        };
    
        struct pHidd_PCI_EnumDevices enummsg = {
        mID:        OOP_GetMethodID(IID_Hidd_PCI, moHidd_PCI_EnumDevices),
        callback:   &FindHook,
        requirements:   (struct TagItem*)&Requirements,
        }, *msg = &enummsg;

        listing_mode = TRUE;
        OOP_DoMethod(pciBus_AGP, (OOP_Msg)msg);
    }   
}


#define ALIGN(val, align)               (val + align - 1) & (~(align - 1))

static void aros_agp_hack_create_gatt_table_generic()
{
    /* Create a table that will hold a certain number of 32bit pointers */
    ULONG entries = aperture_size * 1024 * 1024 / 4096;
    ULONG tablesize =  entries * 4;
    ULONG i = 0;
    
    scratch_memory = AllocVec(4096 * 2, MEMF_PUBLIC | MEMF_CLEAR);
    scratch_memory = (APTR)(ALIGN((IPTR)scratch_memory, 4096));
    LOG_MSG("Created scratch memory at 0x%x\n", (ULONG)scratch_memory);

    
    gatt_table = AllocVec(tablesize + 4096, MEMF_PUBLIC | MEMF_CLEAR);
    gatt_table = (ULONG *)(ALIGN((ULONG)gatt_table, 4096));
    gatt_base = (IPTR)gatt_table;
    
    LOG_MSG("Created GATT table size %d at 0x%x\n", tablesize, (ULONG)gatt_base);
    
    for (i = 0; i < entries; i++)
    {
        writel((ULONG)scratch_memory, gatt_table + i);
        readl(gatt_table + i);	/* PCI Posting. */
    }
}

static void aros_agp_hack_init_sis()
{
    BOOL agp3 = FALSE;
    UBYTE agp_speed = 1;
    ULONG major, minor = 0;
    capptr = aros_agp_hack_get_capabilities_ptr(agp_bridge, CAPABILITY_AGP);
    
    
    /* Getting version info */ 
    major = (readconfigbyte(agp_bridge, capptr + 2 /* PCI_AGP_VERSION */) >> 4) & 0xf;
    minor = readconfigbyte(agp_bridge, capptr + 2 /* PCI_AGP_VERSION */) & 0xf;
    
    LOG_MSG("Read config: AGP version %d.%d\n", major, minor);
    isagp3 = (major == 3 && minor >= 5);
        
    /* Getting mode */
    mode = readconfiglong(agp_bridge, capptr + AGP_STATUS_REG);
    
    agp3 = mode & (1 << 3); /* MODE AGP3 */
    
    if (agp3)
    {
        agp_speed = 4;
        if (mode & (1 << 1)) agp_speed = 8; /* MODE AGP3 x8 */
    }
    else
    {
        agp_speed = 1;
        if (mode & (1 << 1)) agp_speed = 2; /* MODE AGP2 x2 */
        if (mode & (1 << 2)) agp_speed = 4; /* MODE AGP2 x4 */
    }
    
    LOG_MSG("Reading mode: 0x%x -> %s, SPEEDx%d\n", 
        mode, agp3 ? "AGP3" : "AGP2", agp_speed);

    if (isagp3)
    {
        UWORD aperture_size_value = 0;
        ULONG ctrlreg = 0;
        
        /* Getting GART base */
        aperture_base = (IPTR)readconfiglong(agp_bridge, 0x10 /* AGP_APBASE == BAR0 */);
        aperture_base &= (~0x0fUL) /* PCI_BASE_ADDRESS_MEM_MASK */;
        LOG_MSG("Reading aperture base: 0x%x\n", (ULONG)aperture_base);

        /* Getting GART size */
        aperture_size_value = readconfigword(agp_bridge, capptr + 0x14 /* AGPAPSIZE */);
        LOG_MSG("Reading aperture size value: %d\n", aperture_size_value);
        
        switch(aperture_size_value)
        {
            case(0xf3f): aperture_size = 4; break;
            case(0xf3e): aperture_size = 8; break;
            case(0xf3c): aperture_size = 16; break;
            case(0xf38): aperture_size = 32; break;
            case(0xf30): aperture_size = 64; break;
            case(0xf20): aperture_size = 128; break;
            case(0xf00): aperture_size = 256; break;
            case(0xe00): aperture_size = 512; break;
            case(0xc00): aperture_size = 1024; break;
            case(0x800): aperture_size = 2048; break;
            default: aperture_size = 0; break;
        }
        
        LOG_MSG("Calculated aperture size: %d MB\n", aperture_size);

        /* Creation of GATT table */
        aros_agp_hack_create_gatt_table_generic();
        
        /* TODO: Setting GART size (is it needed at all?) */
        
        /* Set GATT pointer */
        writeconfiglong(agp_bridge, capptr + 0x18 /* AGPGARTLO */,
            (ULONG)gatt_base);
        LOG_MSG("Set GATT pointer to 0x%x\n", (ULONG)gatt_base);
        
        /* Enabled GART and GATT */
        ctrlreg = readconfiglong(agp_bridge, capptr + AGP_CTRL_REG);
        writeconfiglong(agp_bridge, capptr + AGP_CTRL_REG,
            ctrlreg | (1<<8) /* AGPCTRL_APERENB */ | AGP_CTRL_REG_GTBLEN);
        
        LOG_MSG("Enabled GART and GATT\n");
        
        bridge_present = TRUE;
    }
    else
    {
        LOG_MSG("SiS Init: AGP2 mode not supported\n");
    }
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
            LOG_MSG("Could not open oop.library\n");
            return 1;
        }
    }
    
    aros_agp_hack_list_all_bridges();
    
    if (aros_agp_hack_find_supported_agp_bridge(0x1039)) /* SiS */
    {
        aros_agp_hack_init_sis();
    }

    if (!aros_agp_hack_is_agp_bridge_present())
    {
        LOG_MSG("No supported AGP bridge found\n");
    }

    return 1;
}

static int aros_agp_hack_deinit_agp()
{
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

ADD2INIT(aros_agp_hack_init_agp, 4);
ADD2EXIT(aros_agp_hack_deinit_agp, 4);
