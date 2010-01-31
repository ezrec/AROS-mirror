#include "aros_agp_hack.h"

#include <proto/oop.h>
#include <hidd/pci.h>
#include <hidd/hidd.h>
#include <aros/libcall.h>
#include <aros/debug.h>
#include <aros/symbolsets.h>

OOP_AttrBase __IHidd_PCIDev_AGP = 0;
OOP_AttrBase __IHidd_PCIDrv_AGP = 0;
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
    /* Has to be done using driver because of bug in PciDevice ReadConfigLong
       method (return type was UBYTE instead of ULONG */

    OOP_Object * driver;
    UBYTE bus, dev, sub;
    struct pHidd_PCIDriver_ReadConfigLong rclmsg;
    struct pHidd_PCIDriver_ReadConfigLong * msg = &rclmsg;
    
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Driver, (APTR)&driver);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Bus, (APTR)&bus);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Dev, (APTR)&dev);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_Sub, (APTR)&sub);
    
    rclmsg.mID = OOP_GetMethodID(IID_Hidd_PCIDriver, moHidd_PCIDriver_ReadConfigLong);
    rclmsg.bus = bus;
    rclmsg.dev = dev;
    rclmsg.sub = sub;
    rclmsg.reg = where;
    
    return (ULONG)OOP_DoMethod(driver, (OOP_Msg)msg);
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

#define LOG_MSG(x, ...)         \
    bug(x, ##__VA_ARGS__);      \
    printf(x, ##__VA_ARGS__);

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

#define writel(val, addr)               (*(volatile ULONG*)(addr) = (val))
#define readl(addr)                     (*(volatile ULONG*)(addr))
#define ALIGN(val, align)               (val + align - 1) & (~(align - 1))

static void aros_agp_hack_create_gatt_table()
{
    /* Create a table that will hold a certain number of 32bit pointers */
    ULONG entries = aperture_size * 1024 * 1024 / 4096;
    ULONG tablesize =  entries * 4;
    ULONG i = 0;
    
    scratch_memory = AllocVec(4096 * 2, MEMF_PUBLIC | MEMF_CLEAR);
    scratch_memory = (APTR)(ALIGN((IPTR)scratch_memory, 4096));
    LOG_MSG("Created scratch memory at 0x%x\n", (ULONG)scratch_memory);
    
    gatt_table = AllocVec(tablesize + 4096, MEMF_PUBLIC | MEMF_CLEAR);
    gatt_table = (APTR)(ALIGN((ULONG)gatt_table, 4096));
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
    UBYTE capptr = aros_agp_hack_get_capabilities_ptr(agp_bridge, CAPABILITY_AGP);
    BOOL agp3 = FALSE;
    UBYTE agp_speed = 1;
    ULONG major, minor = 0;
    BOOL isagp3 = FALSE;
    
    /* Getting version info */ 
    major = (readconfigbyte(agp_bridge, capptr + 2 /* PCI_AGP_VERSION */) >> 4) & 0xf;
    minor = readconfigbyte(agp_bridge, capptr + 2 /* PCI_AGP_VERSION */) & 0xf;
    
    LOG_MSG("Read config: AGP version %d.%d\n", major, minor);
    isagp3 = (major == 3 && minor >= 5);
        
    /* Getting mode */
    mode = readconfiglong(agp_bridge, capptr + 0x04 /* PCI_AGP_STATUS */);
    
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
        aros_agp_hack_create_gatt_table();
        
        /* TODO: Setting GART size (is it needed at all?) */
        
        /* Set GATT pointer */
        writeconfiglong(agp_bridge, capptr + 0x18 /* AGPGARTLO */,
            (ULONG)gatt_base);
        LOG_MSG("Set GATT pointer to 0x%x\n", (ULONG)gatt_base);
        
        /* Enabled GART and GATT */
        ctrlreg = readconfiglong(agp_bridge, capptr + 0x10 /* AGPCTRL */);
        writeconfiglong(agp_bridge, capptr + 0x10 /* AGPCTRL */,
            ctrlreg | (1<<8) /* AGPCTRL_APERENB */ | (1<<7) /* AGPCTRL_GTLBEN */);
        
        LOG_MSG("Enabled GART and GATT\n");
        
        bridge_present = TRUE;
    }
    else
    {
        LOG_MSG("AGP2 mode not supported\n");
    }
}

static int aros_agp_hack_init_agp(void)
{
    if (!OOPBase_AGP)
    {
        if ((OOPBase_AGP=OpenLibrary("oop.library", 0)) != NULL)
        {
            __IHidd_PCIDev_AGP = OOP_ObtainAttrBase(IID_Hidd_PCIDevice);
            __IHidd_PCIDrv_AGP = OOP_ObtainAttrBase(IID_Hidd_PCIDriver);
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
    
    if (__IHidd_PCIDrv_AGP != 0)
    {
        OOP_ReleaseAttrBase(IID_Hidd_PCIDriver);
        __IHidd_PCIDrv_AGP = 0;
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
