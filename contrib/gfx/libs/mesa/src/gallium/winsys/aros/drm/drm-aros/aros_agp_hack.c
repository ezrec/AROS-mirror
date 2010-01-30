#include "aros_agp_hack.h"

#include <proto/oop.h>
#include <hidd/pci.h>
#include <hidd/hidd.h>
#include <aros/libcall.h>
#include <aros/debug.h>

static UBYTE readconfigbyte(OOP_Object * pciDevice, UBYTE where)
{
    struct pHidd_PCIDevice_ReadConfigLong rcbmsg = {
    mID: OOP_GetMethodID(IID_Hidd_PCIDevice, moHidd_PCIDevice_ReadConfigByte),
    reg: where,
    }, *msg = &rcbmsg;
    
    return (UBYTE)OOP_DoMethod(pciDevice, (OOP_Msg)msg);
}

static BOOL aros_agp_hack_read_capabilities(OOP_Object * pciDevice, UBYTE cap)
{
    LONG maxcaps = 48;
    UBYTE where = 0x34; /*  First cap list entry */
    UBYTE capid = 0;
    
    while(maxcaps > 0)
    {
        where = readconfigbyte(pciDevice, where);

        if (where < 0x40)
            break;
        where &= ~3; /* ?? */
        capid = readconfigbyte(pciDevice, where);
        if (capid == 0xff)
            break;
        if (capid == cap /* AGP */) return TRUE;
    
        where += 1; /* next cap */
        
        maxcaps--;
    }
    
    return FALSE;
}

BOOL aros_agp_hack_device_is_agp(OOP_Object * pciDevice)
{
    return aros_agp_hack_read_capabilities(pciDevice, 0x02);
}

BOOL aros_agp_hack_device_is_pcie(OOP_Object * pciDevice)
{
    return aros_agp_hack_read_capabilities(pciDevice, 0x10);
}

/* AGP Bridge Detection */
#include <stdio.h>
OOP_Object * agp_bridge = NULL;
BOOL listing_mode = FALSE;
BOOL detection_done = FALSE;

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
        bug("Listing bridge: 0x%x/0x%x -> %s\n", 
            (UWORD)VendorID, (UWORD)ProductID, isAGP ? "AGP" : "NOT AGP");

        printf("Listing bridge: 0x%x/0x%x -> %s\n", 
            (UWORD)VendorID, (UWORD)ProductID, isAGP ? "AGP" : "NOT AGP");
    }
    
    if (isAGP && !agp_bridge && !listing_mode)
    {
        bug("Selecting device 0x%x/0x%x as AGP bridge\n",
            (UWORD)VendorID, (UWORD)ProductID);
        printf("Selecting device 0x%x/0x%x as AGP bridge\n",
            (UWORD)VendorID, (UWORD)ProductID);

        agp_bridge = pciDevice;
    }

    AROS_USERFUNC_EXIT
}

static BOOL aros_agp_hack_find_supported_agp_bridge(UWORD VendorID)
{
    OOP_Object * pciBus = NULL;
    listing_mode = FALSE;
    
    pciBus = OOP_NewObject(NULL, CLID_Hidd_PCI, NULL);

    if (pciBus)
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

        OOP_DoMethod(pciBus, (OOP_Msg)msg);
    }
    
    return agp_bridge != NULL;
}

static void aros_agp_hack_list_all_bridges()
{
    OOP_Object * pciBus = NULL;
    listing_mode = TRUE;
   
    pciBus = OOP_NewObject(NULL, CLID_Hidd_PCI, NULL);

    if (pciBus)
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

        OOP_DoMethod(pciBus, (OOP_Msg)msg);
    }   
}

OOP_Object * aros_agp_hack_find_agp_bridge()
{
    if (!detection_done)
    {
        detection_done = TRUE; /* run only once */
        
        aros_agp_hack_list_all_bridges();
        
        if (aros_agp_hack_find_supported_agp_bridge(0x1039)) /* SiS */
            return agp_bridge;

        bug("No supported AGP bridge found\n");
        printf("No supported AGP bridge found\n");
    }
    
    return agp_bridge;
}
