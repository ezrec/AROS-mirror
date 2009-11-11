#include "drmP.h"

void drm_sg_cleanup(struct drm_sg_mem * entry)
{
    asm("int3");
}

void nouveau_preclose(struct drm_device *dev, struct drm_file *file_priv)
{
    asm("int3");
}

int nouveau_firstopen(struct drm_device *dev)
{
    asm("int3");
}

void nouveau_lastclose(struct drm_device *dev)
{
    asm("int3");
}

int nouveau_unload(struct drm_device *dev)
{
    asm("int3");
}

struct drm_ioctl_desc * nouveau_ioctls;

void
nouveau_irq_preinstall(struct drm_device *dev)
{
    asm("int3");
}

int
nouveau_irq_postinstall(struct drm_device *dev)
{
    asm("int3");
}

void
nouveau_irq_uninstall(struct drm_device *dev)
{
    asm("int3");
}

irqreturn_t
nouveau_irq_handler(DRM_IRQ_ARGS)
{
    asm("int3");
}

void nouveau_fence_handler(struct drm_device * dev, int channel)
{
    asm("int3");
}