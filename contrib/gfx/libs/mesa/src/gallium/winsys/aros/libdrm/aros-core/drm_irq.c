#include "drmP.h"

/* FIXME: This should not be here */
#include "nouveau_drv.h"

#include <proto/oop.h>
#include <hidd/pci.h>

void
nouveau_fence_handler(struct drm_device *dev, int channel)
{
    DRM_IMPL("\n");
}

static void interrupt_handler(HIDDT_IRQ_Handler * irq, HIDDT_IRQ_HwInfo *hw)
{
    struct drm_device *dev = (struct drm_device*)irq->h_Data;
    
    /*FIXME: Should be generic, not nouveau */
    nouveau_irq_handler(dev);
}

int drm_irq_install(struct drm_device *dev)
{
#if defined(HOSTED_BUILD)
    return 0;
#else    
    struct OOP_Object *o = NULL;
    IPTR INTLine = 0;
    int retval = -EINVAL;
    
    /* FIXME: This should be generic (dependent of driver) and not hardcoded to nouveau */
    if (dev->irq_enabled) {
        return -EBUSY;
    }
    
    dev->irq_enabled = 1;
    
    nouveau_irq_preinstall(dev);

    dev->IntHandler = (HIDDT_IRQ_Handler *)AllocVec(sizeof(HIDDT_IRQ_Handler), MEMF_PUBLIC | MEMF_CLEAR);
    
    if (dev->IntHandler)
    {
        dev->IntHandler->h_Node.ln_Pri = 10;
        dev->IntHandler->h_Node.ln_Name = "Nvidia-gallium handler";
        dev->IntHandler->h_Code = interrupt_handler;
        dev->IntHandler->h_Data = dev;

        OOP_GetAttr(dev->pciDevice, aHidd_PCIDevice_INTLine, &INTLine);
        DRM_DEBUG("INTLine: %d\n", INTLine);
        
        o = OOP_NewObject(NULL, CLID_Hidd_IRQ, NULL);
        
        if (o)
        {
            struct pHidd_IRQ_AddHandler __msg__ = {
                mID:            OOP_GetMethodID(IID_Hidd_IRQ, moHidd_IRQ_AddHandler),
                handlerinfo:    dev->IntHandler,
                id:             INTLine,
            }, *msg = &__msg__;

            if (OOP_DoMethod((OOP_Object *)o, (OOP_Msg)msg))
                retval = 0;

            OOP_DisposeObject((OOP_Object *)o);
        }
    }

    if (retval != 0)
    {
        dev->irq_enabled = 0;
        return retval;
    }
    
    nouveau_irq_postinstall(dev);
    
    return retval;
#endif    
}
