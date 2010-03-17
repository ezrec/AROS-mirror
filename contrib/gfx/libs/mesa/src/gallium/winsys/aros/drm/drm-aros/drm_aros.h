/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#if !defined(DRM_AROS_H)
#define DRM_AROS_H

#include <hidd/irq.h>
extern struct Library   * OOPBase_Nouveau;
extern OOP_Object       * pciDriver;
extern OOP_Object       * pciBus;
struct drm_driver;
#define OOPBase OOPBase_Nouveau

LONG        drm_aros_pci_find_supported_video_card(struct drm_driver *drv);
void        drm_aros_pci_shutdown(struct drm_driver *drv);

#endif /* DRM_AROS_H */
