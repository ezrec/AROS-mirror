/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "drmP.h"

#include "nouveau_drv.h"
#include "nouveau_pciids.h"

/* FIXME: Check what impact these settings have */
int nouveau_vram_pushbuf = 0;
int nouveau_vram_notify = 0;

extern struct drm_ioctl_desc nouveau_ioctls[];

static struct drm_driver driver =
{
    .VendorID = 0x10de,
    .PciIDs = nouveau_pciids,
    .driver_features = DRIVER_MODESET | DRIVER_GEM,
    .load = nouveau_load,
    .firstopen = nouveau_firstopen,
    .preclose = nouveau_preclose,
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
