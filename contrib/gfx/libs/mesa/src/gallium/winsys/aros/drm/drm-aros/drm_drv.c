/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "drmP.h"
#include "drm_compat_funcs.h"

int drm_lastclose(struct drm_device * dev)
{
    if (dev->driver->lastclose)
        dev->driver->lastclose(dev);
    
    if (dev->irq_enabled)
        drm_irq_uninstall(dev);
    
    return 0;
}

static void drm_cleanup(struct drm_device * dev)
{
    drm_lastclose(dev);
    
    if (dev->driver->unload)
        dev->driver->unload(dev);
    
    drm_aros_pci_shutdown(dev);
}

/* HACK to get access to device */
extern struct drm_device global_drm_device;

void drm_exit(struct drm_driver * driver)
{
    /* FIXME: drm_device should not be hardcoded */
    drm_cleanup(&global_drm_device);
}

int drm_init(struct drm_driver * driver)
{
    int ret;
    /* FIXME: drm_device should not be hardcoded */
    struct drm_device * dev = &global_drm_device;
    dev->driver = driver;
    
#if !defined(HOSTED_BUILD)    
    ret = drm_aros_find_supported_video_card(dev);
    if (ret)
        return -1;
#else
#if HOSTED_BUILD_HARDWARE == HOSTED_BUILD_HARDWARE_I915
    dev->pci_device = HOSTED_BUILD_PRODUCT_ID;
#endif
#endif
    
    /* Init fields */
    INIT_LIST_HEAD(&dev->maplist);
    dev->irq_enabled = 0;
    InitSemaphore(&dev->struct_mutex.semaphore);

    if (drm_core_has_AGP(dev)) {
//FIXME        if (drm_device_is_agp(dev))
            dev->agp = drm_agp_init(dev);
//FIXME        if (drm_core_check_feature(dev, DRIVER_REQUIRE_AGP)
//FIXME            && (dev->agp == NULL)) {
//FIXME            DRM_ERROR("Cannot initialize the agpgart module.\n");
//FIXME            return -1;
//FIXME        }
    }

    
    if (driver->driver_features & DRIVER_GEM) {
        if (drm_gem_init(dev)) {
            DRM_ERROR("Cannot initialize graphics execution "
                  "manager (GEM)\n");
            return -1;
        }
    }
    
    if (!dev->driver->load)
        return -1;

    ret = dev->driver->load(dev, 0);
    if (ret)
        return -1;

    if (dev->driver->firstopen)
    {    
        ret = dev->driver->firstopen(dev);
        if (ret)
            return -1;
    }
    
    return 0;
}
