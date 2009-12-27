/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "drmP.h"

#include "i915_drv.h"
#include "i915_pciids.h"

/* FIXME: What this does? */
unsigned int i915_powersave = 0;

//extern struct drm_ioctl_desc 915_ioctls[];

static struct drm_driver driver =
{
    .VendorID = 0x8086,
    .PciIDs = i915_pciids,
    .driver_features = DRIVER_MODESET | DRIVER_GEM | DRIVER_USE_AGP | DRIVER_REQUIRE_AGP,
    .load = i915_driver_load,
/*    .firstopen = nouveau_firstopen,
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
    .gem_free_object = nouveau_gem_object_del,*/
};

void i915_exit(void)
{
/* FIXME */
//    drm_exit(&driver);
}

int i915_init(void)
{
    return drm_init(&driver);
}
