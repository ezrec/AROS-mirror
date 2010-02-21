/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "drmP.h"

#include "nouveau_drv.h"
#include "nouveau_pciids.h"

/* Whether pushbuf and notifer are to be put in VRAM (access via 
   pci mapping) or in GART (accessed by card - SGDMA or AGP) 
   Setting nouveau_vram_pushbuf to 1 causes problems */
int nouveau_vram_pushbuf = 0;
int nouveau_vram_notify = 0;
int nouveau_noagp = 0;
int nouveau_noaccel = 0;
int nouveau_ctxfw = 0; /* Use external ctxprog for NV40 */

extern struct drm_ioctl_desc nouveau_ioctls[];

static struct drm_driver driver =
{
    .VendorID = 0x10de,
    .ProductID = 0x0,
    .PciIDs = nouveau_pciids,
    .pciDevice = NULL,
    .IsAGP = FALSE,
    .IsPCIE = FALSE,
    .dev = NULL,
    .driver_features = DRIVER_MODESET | DRIVER_GEM | DRIVER_USE_AGP,
    .load = nouveau_load,
    .firstopen = nouveau_firstopen,
    .open = NULL,
    .preclose = nouveau_preclose,
    .postclose = NULL,
    .lastclose = nouveau_lastclose,
    .unload = nouveau_unload,
    .irq_handler = nouveau_irq_handler,
    .irq_preinstall = nouveau_irq_preinstall,
    .irq_postinstall = nouveau_irq_postinstall,
    .irq_uninstall = nouveau_irq_uninstall,
    .version_patchlevel = NOUVEAU_DRM_HEADER_PATCHLEVEL,
    .ioctls = nouveau_ioctls,
    .gem_init_object = nouveau_gem_object_new,
    .gem_free_object = nouveau_gem_object_del,
};

void nouveau_exit(void)
{
    drm_exit(&driver);
}

int nouveau_init(void)
{
    return drm_init(&driver);
}
