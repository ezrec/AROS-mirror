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
