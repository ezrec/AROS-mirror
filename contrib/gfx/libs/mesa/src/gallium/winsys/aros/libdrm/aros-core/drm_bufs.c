#include "drmP.h"

unsigned long drm_get_resource_len(struct drm_device *dev, unsigned int resource)
{
    return (unsigned long)drm_pci_resource_len(dev->pciDevice, resource);
}

unsigned long drm_get_resource_start(struct drm_device *dev,
                        unsigned int resource)
{
    return (unsigned long)drm_pci_resource_start(dev->pciDevice, resource);
}

int drm_addmap(struct drm_device *dev, unsigned int offset,
              unsigned int size, enum drm_map_type type,
              enum drm_map_flags flags, drm_local_map_t ** map_ptr)
{
    struct drm_map *map;
    
    /* FIXME: Add support for other types */
    if (type != _DRM_REGISTERS)
    {
        DRM_ERROR("Type %d UNHANDLED\n", type);
        return -EINVAL;
    }

    map = drm_alloc(sizeof(*map), DRM_MEM_MAPS);
    if (!map)
        return -ENOMEM;

    map->offset = offset;
    map->size = size;
    map->flags = flags;
    map->type = type;

    if (map->type == _DRM_REGISTERS) {
        map->handle = drm_pci_ioremap(dev->pcidriver, (APTR)map->offset, (IPTR)map->size);
        if (!map->handle) {
            drm_free(map, sizeof(*map), DRM_MEM_MAPS);
            return -ENOMEM;
        }
    }

    *map_ptr = map;
    
    return 0;
}
              
/**
 * Compute size order.  Returns the exponent of the smaller power of two which
 * is greater or equal to given number.
 *
 * \param size size.
 * \return order.
 *
 * \todo Can be made faster.
 */
int drm_order(unsigned long size)
{
    int order;
    unsigned long tmp;

    for (order = 0, tmp = size >> 1; tmp; tmp >>= 1, order++) ;

    if (size & (size - 1))
        ++order;

    return order;
}