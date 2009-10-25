/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

/* FIXME: This should implement generic approach - not card specific */

#include "drmP.h"

/* FIXME: This should not be here */
#include "nouveau_drv.h"

int drm_lastclose(struct drm_device * dev)
{
    nouveau_lastclose(dev);
    
    if (dev->irq_enabled)
        drm_irq_uninstall(dev);
    
    if (dev->sg)
    {
        drm_sg_cleanup(dev->sg);
        dev->sg = NULL;
    }
    
    return 0;
}

static void drm_cleanup(struct drm_device * dev)
{
    drm_lastclose(dev);
    
    /* FIXME: Should be generic, not card specific */
    nouveau_unload(dev);
    
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
    
    /* Init fields */
    
    /* FIXME: drm_device should not be hardcoded */
    INIT_LIST_HEAD(&global_drm_device.maplist);
    global_drm_device.sg = NULL;
    global_drm_device.irq_enabled = 0;
    
    DRM_DEBUG("%s, %s\n", name, busid);
#if !defined(HOSTED_BUILD)    
    ret = drm_aros_find_supported_video_card(&global_drm_device);
    if (ret)
        return -1;
#endif

    ret = nouveau_load(&global_drm_device, 0);
    if (ret)
        return -1;

    ret = nouveau_firstopen(&global_drm_device);
    if (ret)
        return -1;
    
    return 0;
}
