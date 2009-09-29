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

static int drm_addmap_core(struct drm_device *dev, unsigned int offset,
               unsigned int size, enum drm_map_type type,
               enum drm_map_flags flags,
               struct drm_map_list **maplist)
{
    struct drm_map *map;
    struct drm_map_list *list;
    unsigned long user_token;
    int ret;    
    
    /* FIXME: Add support for other types */
    if ((type != _DRM_REGISTERS) && (type != _DRM_FRAME_BUFFER) && (type != _DRM_SCATTER_GATHER))
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

    switch (map->type) {
    case _DRM_FRAME_BUFFER:
        /* FIXME: In original codes _DRM_FRAME_BUFFER is only mapped if drm_core_has_MTRR(dev) */
    case _DRM_REGISTERS:
        map->handle = drm_pci_ioremap(dev->pcidriver, (APTR)map->offset, (IPTR)map->size);
        if (!map->handle) {
            drm_free(map, sizeof(*map), DRM_MEM_MAPS);
            return -ENOMEM;
        }
        break;
    case _DRM_SCATTER_GATHER:
        if (!dev->sg) {
            drm_free(map, sizeof(*map), DRM_MEM_MAPS);
            return -EINVAL;
        }
        map->offset += (unsigned long)dev->sg->virtual;
        break;
    default:
        drm_free(map, sizeof(*map), DRM_MEM_MAPS);
        return -EINVAL;
    }


    list = drm_alloc(sizeof(*list), DRM_MEM_MAPS);
    if (!list) {
        if (map->type == _DRM_REGISTERS)
            drm_pci_iounmap(dev->pcidriver, map->handle, map->size);
        drm_free(map, sizeof(*map), DRM_MEM_MAPS);
        return -EINVAL;
    }
    memset(list, 0, sizeof(*list));
    list->map = map;

    list_add(&list->head, &dev->maplist);

    /* Assign a 32-bit handle */
#if !defined(__AROS__)
    user_token = (map->type == _DRM_SHM) ? (unsigned long) map->handle :
        map->offset;
    ret = drm_map_handle(dev, &list->hash, user_token, 0);

    if (ret) {
        if (map->type == _DRM_REGISTERS)
            drm_pci_iounmap(dev->pcidriver, map->handle, map->size);
        drm_free(map, sizeof(*map), DRM_MEM_MAPS);
        drm_free(list, sizeof(*list), DRM_MEM_MAPS);
        return ret;
    }

    list->user_token = list->hash.key << PAGE_SHIFT;
#else
DRM_ERROR("IMPLEMENT assigning token?\n");
#warning IMPLEMENT assigning token?
#endif
    *maplist = list;
    return 0;
}

int drm_addmap(struct drm_device *dev, unsigned int offset,
           unsigned int size, enum drm_map_type type,
           enum drm_map_flags flags, drm_local_map_t ** map_ptr)
{
    struct drm_map_list *list;
    int rc;

    rc = drm_addmap_core(dev, offset, size, type, flags, &list);
    if (!rc)
        *map_ptr = list->map;
    return rc;
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

struct drm_map_list *drm_find_matching_map(struct drm_device *dev, drm_local_map_t *map)
{
    struct drm_map_list *entry;
//     list_for_each_entry(entry, &dev->maplist, head) {
    for (entry = (struct drm_map_list *)dev->maplist.next; entry != (struct drm_map_list *)&dev->maplist; entry = (struct drm_map_list *)entry->head.next){
        if (entry->map && map->type == entry->map->type &&
            ((entry->map->offset == map->offset) ||
             (map->type == _DRM_SHM && map->flags==_DRM_CONTAINS_LOCK))) {
            return entry;
        }
    }

    return NULL;
}