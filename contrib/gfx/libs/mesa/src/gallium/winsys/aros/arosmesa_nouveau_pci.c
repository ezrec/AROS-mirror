/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_nouveau_pci.h"

#include <aros/libcall.h>

#define DEBUG 1
#include <aros/debug.h>

#include <proto/exec.h>
#include <proto/oop.h>
#include <oop/oop.h>

#include <hidd/pci.h>
#include <hidd/hidd.h>

struct nouveau_device * global_device = NULL;

/* FIXME: should these be global? */
OOP_Object * pci = NULL;
struct Library *OOPBase = NULL;
OOP_AttrBase __IHidd_PCIDev;

/* Taken from nvidia.hidd */

#define NV_ARCH_03  0x03
#define NV_ARCH_04  0x04
#define NV_ARCH_10  0x10
#define NV_ARCH_20  0x20
#define NV_ARCH_30  0x30
#define NV_ARCH_40  0x40

typedef enum {
    NV04 = 0,   NV05,   NV05M64,    NV06,   NV10,
    NV11,       NV11M,  NV15,       NV17,   NV17M,
    NV18,       NV18M,  NV20,       NV25,   NV28,
    NV30,       NV31,   NV34,       NV35,   NV36,
    NV43,

    CardType_Sizeof
} CardType;

static BOOL NV4Init(/*struct staticdata *sd*/)
{
    return TRUE;
}

#define NV10Init NV4Init
#define NV20Init NV4Init

/* List of DeviceID's of supported nvidia cards */
static const struct NVDevice {
    UWORD VendorID, ProductID;
    CardType Type;
    UWORD Arch;
    BOOL (*Init)(/*struct staticdata**/);
    BOOL masked_check;
    
} support[] = {

    /* Known: match ProductID exactly */
    { 0x10de, 0x0020, NV04, NV_ARCH_04, NV4Init, FALSE }, /* RIVA TNT */
    { 0x10de, 0x0028, NV05, NV_ARCH_04, NV4Init, FALSE }, /* RIVA TNT2 */
    { 0x10de, 0x0029, NV05, NV_ARCH_04, NV4Init, FALSE }, /* RIVA TNT2 Ultra */
    { 0x10de, 0x002a, NV05, NV_ARCH_04, NV4Init, FALSE }, /* Unknown TNT2 */
    { 0x10de, 0x002b, NV05, NV_ARCH_04, NV4Init, FALSE },
    { 0x10de, 0x002c, NV05, NV_ARCH_04, NV4Init, FALSE }, /* Vanta */
    { 0x10de, 0x002d, NV05M64,  NV_ARCH_04, NV4Init, FALSE }, /* RIVA TNT2 Model 64 */
    { 0x10de, 0x002e, NV06, NV_ARCH_04, NV4Init, FALSE },
    { 0x10de, 0x002f, NV06, NV_ARCH_04, NV4Init, FALSE },
    { 0x10de, 0x00a0, NV05, NV_ARCH_04, NV4Init, FALSE }, /* Aladdin TNT2 */
    { 0x10de, 0x0100, NV10, NV_ARCH_10, NV10Init, FALSE }, /* GeForce 256 */
    { 0x10de, 0x0101, NV10, NV_ARCH_10, NV10Init, FALSE }, /* GeForce DDR */
    { 0x10de, 0x0102, NV10, NV_ARCH_10, NV10Init, FALSE },
    { 0x10de, 0x0103, NV10, NV_ARCH_10, NV10Init, FALSE }, /* Quadro */
    { 0x10de, 0x0110, NV11, NV_ARCH_10, NV10Init, FALSE }, /* GeForce2 MX/MX 400 */
    { 0x10de, 0x0111, NV11, NV_ARCH_10, NV10Init, FALSE }, /* GeForce2 MX 100/200 */
    { 0x10de, 0x0112, NV11, NV_ARCH_10, NV10Init, FALSE }, /* GeForce2 Go */
    { 0x10de, 0x0113, NV11, NV_ARCH_10, NV10Init, FALSE }, /* Quadro2 MXR/EX/Go */
    { 0x10de, 0x0150, NV15, NV_ARCH_10, NV10Init, FALSE }, /* GeForce2 GTS */
    { 0x10de, 0x0151, NV15, NV_ARCH_10, NV10Init, FALSE }, /* GeForce2 Ti */
    { 0x10de, 0x0152, NV15, NV_ARCH_10, NV10Init, FALSE }, /* GeForce2 Ultra */
    { 0x10de, 0x0153, NV15, NV_ARCH_10, NV10Init, FALSE }, /* Quadro2 Pro */
    { 0x10de, 0x0170, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX 460 */
    { 0x10de, 0x0171, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX 440 */
    { 0x10de, 0x0172, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX 420 */
    { 0x10de, 0x0173, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX 440-SE */
    { 0x10de, 0x0174, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 440 Go */
    { 0x10de, 0x0175, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 420 Go */
    { 0x10de, 0x0176, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 420 Go 32M */
    { 0x10de, 0x0177, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 460 Go */
    { 0x10de, 0x0178, NV17, NV_ARCH_10, NV10Init, FALSE }, /* Quadro4 550 XGL */
    { 0x10de, 0x0179, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 440 Go 64M / GeForce4 Mx (Mac) */
    { 0x10de, 0x017a, NV17, NV_ARCH_10, NV10Init, FALSE }, /* Quadro4 NVS */
    { 0x10de, 0x017c, NV17, NV_ARCH_10, NV10Init, FALSE }, /* Quadro4 500 GoGL */
    { 0x10de, 0x017d, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 410 Go 16M */
    { 0x10de, 0x0180, NV18, NV_ARCH_10, NV10Init, FALSE },
    { 0x10de, 0x0181, NV18, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX 440 with AGP8x */
    { 0x10de, 0x0182, NV18, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX 440SE with AGP8x */
    { 0x10de, 0x0183, NV18, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX 420 with AGP8x */
    { 0x10de, 0x0186, NV18, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 448 Go */
    { 0x10de, 0x0187, NV18, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 488 Go */
    { 0x10de, 0x0188, NV18, NV_ARCH_10, NV10Init, FALSE }, /* Quadro4 580 XGL */
    { 0x10de, 0x0189, NV18, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX with AGP8X (Mac) */
    { 0x10de, 0x018a, NV18, NV_ARCH_10, NV10Init, FALSE }, /* Quadro4 280 NVS */
    { 0x10de, 0x018b, NV18, NV_ARCH_10, NV10Init, FALSE }, /* Quadro4 380 XGL */
    { 0x10de, 0x01a0, NV11, NV_ARCH_10, NV10Init, FALSE }, /* GeForce2 Integrated GPU */
    { 0x10de, 0x01f0, NV17, NV_ARCH_10, NV10Init, FALSE }, /* GeForce4 MX Integerated GPU */
    { 0x10de, 0x0200, NV20, NV_ARCH_20, NV20Init, FALSE }, /* GeForce3 */
    { 0x10de, 0x0201, NV20, NV_ARCH_20, NV20Init, FALSE }, /* GeForce3 Ti 200 */
    { 0x10de, 0x0202, NV20, NV_ARCH_20, NV20Init, FALSE }, /* GeForce3 Ti 500 */
    { 0x10de, 0x0203, NV20, NV_ARCH_20, NV20Init, FALSE }, /* Quadro DCC */
    { 0x10de, 0x0250, NV25, NV_ARCH_20, NV20Init, FALSE }, /* GeForce4 Ti 4600 */
    { 0x10de, 0x0251, NV25, NV_ARCH_20, NV20Init, FALSE }, /* GeForce4 Ti 4400 */
    { 0x10de, 0x0252, NV25, NV_ARCH_20, NV20Init, FALSE }, /* 0x252 */
    { 0x10de, 0x0253, NV25, NV_ARCH_20, NV20Init, FALSE }, /* GeForce4 Ti 4200 */
    { 0x10de, 0x0258, NV25, NV_ARCH_20, NV20Init, FALSE }, /* Quadro4 900 XGL */
    { 0x10de, 0x0259, NV25, NV_ARCH_20, NV20Init, FALSE }, /* Quadro4 750 XGL */
    { 0x10de, 0x025b, NV25, NV_ARCH_20, NV20Init, FALSE }, /* Quadro4 700 XGL */
    { 0x10de, 0x0280, NV28, NV_ARCH_20, NV20Init, FALSE }, /* GeForce4 Ti 4800 */
    { 0x10de, 0x0281, NV28, NV_ARCH_20, NV20Init, FALSE }, /* GeForce4 Ti 4200 with AGP8X */
    { 0x10de, 0x0282, NV28, NV_ARCH_20, NV20Init, FALSE }, /* GeForce4 Ti 4800 SE */
    { 0x10de, 0x0286, NV28, NV_ARCH_20, NV20Init, FALSE }, /* GeForce4 4200 Go */
    { 0x10de, 0x0288, NV28, NV_ARCH_20, NV20Init, FALSE }, /* Quadro4 980 XGL */
    { 0x10de, 0x0289, NV28, NV_ARCH_20, NV20Init, FALSE }, /* Quadro4 780 XGL */
    { 0x10de, 0x028c, NV28, NV_ARCH_20, NV20Init, FALSE }, /* Quadro4 700 GoGL */
    { 0x10de, 0x02a0, NV20, NV_ARCH_20, NV20Init, FALSE }, 
    { 0x10de, 0x0301, NV30, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5800 Ultra */
    { 0x10de, 0x0302, NV30, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5800 */
    { 0x10de, 0x0308, NV30, NV_ARCH_30, NV20Init, FALSE }, /* Quadro FX 2000 */
    { 0x10de, 0x0309, NV30, NV_ARCH_30, NV20Init, FALSE }, /* Quadro FX 1000 */
    { 0x10de, 0x0311, NV31, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5600 Ultra */
    { 0x10de, 0x0312, NV31, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5600 */
    { 0x10de, 0x0313, NV31, NV_ARCH_30, NV20Init, FALSE }, /* 0x313 */
    { 0x10de, 0x0314, NV31, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5600 SE */
    { 0x10de, 0x0316, NV31, NV_ARCH_30, NV20Init, FALSE }, /* 0x316 */
    { 0x10de, 0x0317, NV31, NV_ARCH_30, NV20Init, FALSE }, /* 0x317 */
    { 0x10de, 0x031a, NV31, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go5600 */
    { 0x10de, 0x031b, NV31, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go5650 */
    { 0x10de, 0x031c, NV31, NV_ARCH_30, NV20Init, FALSE }, /* Quadro FX Go700 */
    { 0x10de, 0x031d, NV31, NV_ARCH_30, NV20Init, FALSE }, /* 0x31d */
    { 0x10de, 0x031e, NV31, NV_ARCH_30, NV20Init, FALSE }, /* 0x31e */
    { 0x10de, 0x031f, NV31, NV_ARCH_30, NV20Init, FALSE }, /* 0x31f */
    { 0x10de, 0x0320, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5200 */
    { 0x10de, 0x0321, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5200 Ultra */
    { 0x10de, 0x0322, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5200 */
    { 0x10de, 0x0323, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5200SE */
    { 0x10de, 0x0324, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go5200 */
    { 0x10de, 0x0325, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go5250 */
    { 0x10de, 0x0326, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5500 */
    { 0x10de, 0x0327, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5100 */
    { 0x10de, 0x0328, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go5200 32M/64M */
    { 0x10de, 0x0329, NV34, NV_ARCH_30, NV20Init, FALSE }, /* 0x329 / GeForce FX 5200 (Mac) */
    { 0x10de, 0x032a, NV34, NV_ARCH_30, NV20Init, FALSE }, /* Quadro NVS 280 PCI */
    { 0x10de, 0x032b, NV34, NV_ARCH_30, NV20Init, FALSE }, /* Quadro FX 500/600 PCI */
    { 0x10de, 0x032c, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go53xx Series */
    { 0x10de, 0x032d, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go5100 */
    { 0x10de, 0x032e, NV34, NV_ARCH_30, NV20Init, FALSE },
    { 0x10de, 0x032f, NV34, NV_ARCH_30, NV20Init, FALSE }, /* 0x32F */
    { 0x10de, 0x0330, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce Fx 5900 Ultra */
    { 0x10de, 0x0331, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5900 */
    { 0x10de, 0x0332, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5900XT */
    { 0x10de, 0x0333, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5950 Ultra */
    { 0x10de, 0x0334, NV34, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5900ZT */
    { 0x10de, 0x0338, NV34, NV_ARCH_30, NV20Init, FALSE }, /* Quadro FX 3000 */
    { 0x10de, 0x033F, NV34, NV_ARCH_30, NV20Init, FALSE }, /* Quadro FX 700 */
    { 0x10de, 0x0341, NV36, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5700 Ultra */
    { 0x10de, 0x0342, NV36, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5700 */
    { 0x10de, 0x0343, NV36, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5700LE */
    { 0x10de, 0x0344, NV36, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX 5700VE */
    { 0x10de, 0x0345, NV36, NV_ARCH_30, NV20Init, FALSE }, /* 0x345 */
    { 0x10de, 0x0347, NV36, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go5700 */
    { 0x10de, 0x0348, NV36, NV_ARCH_30, NV20Init, FALSE }, /* GeForce FX Go5700 */
    { 0x10de, 0x0349, NV36, NV_ARCH_30, NV20Init, FALSE }, /* 0x349 */
    { 0x10de, 0x034B, NV36, NV_ARCH_30, NV20Init, FALSE }, /* 0x34B */
    { 0x10de, 0x034C, NV36, NV_ARCH_30, NV20Init, FALSE }, /* Quadro FX Go1000 */
    { 0x10de, 0x034E, NV36, NV_ARCH_30, NV20Init, FALSE }, /* Quadro FX 1100 */
    { 0x10de, 0x034F, NV36, NV_ARCH_30, NV20Init, FALSE }, /* 0x34F */
    { 0x10de, 0x0040, NV36, NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6800 Ultra */
    { 0x10de, 0x0041, NV36, NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6800 */
    { 0x10de, 0x0042, NV36, NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6800 LE */
    { 0x10de, 0x0043, NV36, NV_ARCH_40, NV20Init, FALSE }, /* 0x043 */
    { 0x10de, 0x0045, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6800 GT */
    { 0x10de, 0x0049, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x049 */
    { 0x10de, 0x004E, NV36, NV_ARCH_40, NV20Init, FALSE }, /* Quadro FX 4000 */
    { 0x10de, 0x00C0, NV36, NV_ARCH_40, NV20Init, FALSE }, /* 0x0C0 */
    { 0x10de, 0x00C1, NV36, NV_ARCH_40, NV20Init, FALSE }, /* 0x0C1 */
    { 0x10de, 0x00C2, NV36, NV_ARCH_40, NV20Init, FALSE }, /* 0x0C2 */
    { 0x10de, 0x00C8, NV36, NV_ARCH_40, NV20Init, FALSE }, /* 0x0C8 */
    { 0x10de, 0x00C9, NV36, NV_ARCH_40, NV20Init, FALSE }, /* 0x0C9 */
    { 0x10de, 0x00CC, NV36, NV_ARCH_40, NV20Init, FALSE }, /* 0x0CC */
    { 0x10de, 0x00CE, NV36, NV_ARCH_40, NV20Init, FALSE }, /* 0x0CE */
    { 0x10de, 0x00F1, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6600 / GeForce 6600 GT (Verified: LeadTek GeForce 6600 GT) */
    { 0x10de, 0x00F2, NV43,     NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6600 / GeForce 6600 GT (Verified: Club3D GeForce 6600) */
    { 0x10de, 0x0140, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6600 GT */
    { 0x10de, 0x0141, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6600 */
    { 0x10de, 0x0142, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0142 */
    { 0x10de, 0x0143, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0143 */
    { 0x10de, 0x0144, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0144 */
    { 0x10de, 0x0145, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6610 XL */
    { 0x10de, 0x0146, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0146 */
    { 0x10de, 0x0147, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0147 */
    { 0x10de, 0x0148, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0148 */
    { 0x10de, 0x0149, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0149 */
    { 0x10de, 0x014B, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x014B */
    { 0x10de, 0x014C, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x014C */
    { 0x10de, 0x014D, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x014D */
    { 0x10de, 0x014E, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* Quadro FX 540 */
    { 0x10de, 0x014F, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* GeForce 6200 */
    { 0x10de, 0x0160, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0160 */
    { 0x10de, 0x0166, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0166 */
    { 0x10de, 0x0210, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0210 */
    { 0x10de, 0x0211, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x0211 */
    { 0x10de, 0x021D, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x021D */
    { 0x10de, 0x021E, NV36,     NV_ARCH_40, NV20Init, FALSE }, /* 0x021E */
    
    /* Unknown: Match ProductID & 0xFFF0 */
    { 0x10de, 0x0170, NV17, NV_ARCH_10, NV10Init, TRUE },
    { 0x10de, 0x0180, NV18, NV_ARCH_10, NV10Init, TRUE },
    { 0x10de, 0x0250, NV25, NV_ARCH_20, NV20Init, TRUE },
    { 0x10de, 0x0280, NV28, NV_ARCH_20, NV20Init, TRUE },
    { 0x10de, 0x0300, NV30, NV_ARCH_30, NV20Init, TRUE },
    { 0x10de, 0x0310, NV31, NV_ARCH_30, NV20Init, TRUE },
    { 0x10de, 0x0320, NV34, NV_ARCH_30, NV20Init, TRUE },
    { 0x10de, 0x0340, NV34, NV_ARCH_30, NV20Init, TRUE },
    { 0x10de, 0x0040, NV36, NV_ARCH_40, NV20Init, TRUE },
    { 0x10de, 0x00C0, NV36, NV_ARCH_40, NV20Init, TRUE },
    { 0x10de, 0x0120, NV36, NV_ARCH_40, NV20Init, TRUE },
    { 0x10de, 0x0140, NV36, NV_ARCH_40, NV20Init, TRUE },
    { 0x10de, 0x0160, NV36, NV_ARCH_40, NV20Init, TRUE },
    { 0x10de, 0x0130, NV36, NV_ARCH_40, NV20Init, TRUE },
    { 0x10de, 0x01D0, NV36, NV_ARCH_40, NV20Init, TRUE },
    { 0x10de, 0x0090, NV36, NV_ARCH_40, NV20Init, TRUE },
    { 0x10de, 0x0210, NV36, NV_ARCH_40, NV20Init, TRUE },
    
    { 0x0000, 0x0000, }
};



AROS_UFH3(void, Enumerator,
    AROS_UFHA(struct Hook *,    hook,       A0),
    AROS_UFHA(OOP_Object *, pciDevice,  A2),
    AROS_UFHA(APTR,     message,    A1))
{
    AROS_USERFUNC_INIT

    IPTR ProductID;
    IPTR VendorID;
    struct NVDevice *sup = (struct NVDevice *)support;

    if (global_device)
    {
        /* Already allocated */
        D(bug("[NVidia-gallium] global_device already allocated\n"));
        return;
    }
    
    /* Get the Device's ProductID */
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_ProductID, &ProductID);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_VendorID, &VendorID);

    D(bug("[NVidia-gallium] VendorID: %x, ProductID: %x\n", VendorID, ProductID));
    
    while (sup->VendorID)
    {
        BOOL found = FALSE;
    
        if (sup->VendorID == VendorID)
        {
            if (!sup->masked_check && (sup->ProductID == ProductID))
            {
                found = TRUE;
            }
            else if (sup->masked_check && (sup->ProductID == (ProductID & 0xFFF0)))
            {
                found = TRUE;
            }
        }

        if (found)
        {
            D(bug("[NVidia-gallium] Found!\n"));
            global_device = AllocVec(sizeof(struct nouveau_device), MEMF_PUBLIC | MEMF_CLEAR);
            
            /* TODO: drmCommandNone(nvdev->fd, DRM_NOUVEAU_CARD_INIT); */
            
            
            
            /* TODO: Implement filling out device properties */
            return;
        }
        
        sup++;
    }
    
    AROS_USERFUNC_EXIT
}

static void 
Find_NV_Card()
{
    D(bug("[NVidia-gallium] Find_NV_Card\n"));
    
    if (!OOPBase)
    {
        if ((OOPBase=OpenLibrary("oop.library", 0)) != NULL)
        {
            __IHidd_PCIDev = OOP_ObtainAttrBase(IID_Hidd_PCIDevice);
        }
        else
        {
            /* Failure */
            return;
        }
    }

    pci = OOP_NewObject(NULL, CLID_Hidd_PCI, NULL);
    
    D(bug("[NVidia-gallium] Creating PCI object\n"));

    if (pci)
    {
        struct Hook FindHook = {
        h_Entry:    (IPTR (*)())Enumerator,
        h_Data:     0UL/* FIXME; was: sd */,
        };

        struct TagItem Requirements[] = {
        { tHidd_PCI_Interface,  0x00 },
        { tHidd_PCI_Class,  0x03 },
        { tHidd_PCI_SubClass,   0x00 },
        { tHidd_PCI_VendorID,   0x10de }, // NVidia VendorID. May require more of them
        { TAG_DONE, 0UL }
        };
    
        struct pHidd_PCI_EnumDevices enummsg = {
        mID:        OOP_GetMethodID(IID_Hidd_PCI, moHidd_PCI_EnumDevices),
        callback:   &FindHook,
        requirements:   (struct TagItem*)&Requirements,
        }, *msg = &enummsg;
        D(bug("[NVidia-gallium] Calling search Hook\n"));
        OOP_DoMethod(pci, (OOP_Msg)msg);
    }
}

static void
arosmesa_create_nouveau_device_global()
{
    if (global_device)
    {
        /* Device alrady created */
        return;
    }

    Find_NV_Card();
}

struct nouveau_device *
arosmesa_create_nouveau_device()
{
    struct nouveau_device * device = NULL;

    arosmesa_create_nouveau_device_global();
    
    /* FIXME: Temp so that initialization does not continue */
    //return device;
    global_device = AllocVec(sizeof(struct nouveau_device), MEMF_PUBLIC | MEMF_CLEAR);
    global_device->chipset = 0x0148;
    /* FIXME: Temp so that initialization does not continue */
    
    if (global_device)
    {
        device = AllocVec(sizeof(struct nouveau_device), MEMF_PUBLIC | MEMF_CLEAR);
        
        if (!device)
            return NULL;
        
        /* Copy device information */
        *device = *global_device;
    }
    
    return device;
}