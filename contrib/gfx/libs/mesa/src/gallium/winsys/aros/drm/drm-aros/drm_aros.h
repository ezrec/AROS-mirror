/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#if !defined(DRM_AROS_H)
#define DRM_AROS_H

#include <hidd/irq.h>
extern struct Library   * OOPBase;
extern OOP_Object       * pciDriver; 
struct drm_device;

LONG        drm_aros_find_supported_video_card(struct drm_device *dev);
void        drm_aros_pci_shutdown(struct drm_device *dev);

#endif /* DRM_AROS_H */
