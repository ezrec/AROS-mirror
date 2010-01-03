/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "drmP.h"
#include "drm_compat_funcs.h"

struct drm_driver *current_drm_driver = NULL;

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
}

void drm_exit(struct drm_driver * driver)
{
    drm_cleanup(driver->dev);
    
    drm_aros_pci_shutdown(driver);
    
    FreeVec(driver->dev);
    driver->dev = NULL;
    current_drm_driver = NULL;
}

static int drm_init_device(struct drm_driver * driver)
{
    /* If this function is called, the card was already found */
    driver->dev = AllocVec(sizeof(struct drm_device), MEMF_ANY | MEMF_CLEAR);
    struct drm_device * dev = driver->dev;
    
    /* Init fields */
    INIT_LIST_HEAD(&dev->maplist);
    dev->irq_enabled = 0;
    InitSemaphore(&dev->struct_mutex.semaphore);
    dev->driver = driver;
    dev->pci_vendor = driver->VendorID;
    dev->pci_device = driver->ProductID;
    dev->pdev = driver->pciDevice;
    int ret;

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

int drm_init(struct drm_driver * driver)
{
#if !defined(HOSTED_BUILD)
    if (drm_aros_pci_find_supported_video_card(driver))
        return -1;
#else
#if HOSTED_BUILD_HARDWARE == HOSTED_BUILD_HARDWARE_I915
    driver->ProductID = HOSTED_BUILD_PRODUCT_ID;
#endif
#endif
    if (drm_init_device(driver))
    {
        drm_exit(driver);
        return -1;
    }
    
    current_drm_driver = driver;
    
    return 0;
}
