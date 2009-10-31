#include "arosdrm.h"
#include "drmP.h"

struct drm_device global_drm_device;
/* FIXME: Array for now, list maybe in future */
struct drm_file * drm_files[128] = {NULL};

int 
drmCommandNone(int fd, unsigned long drmCommandIndex)
{
    if (!drm_files[fd])
        return -EINVAL;

    if (!global_drm_device.driver || !global_drm_device.driver->ioctls)
        return -EINVAL;

    /* FIXME: Remove switch when all paths are tested */
    switch(drmCommandIndex)
    {
        case(0x0 /*DRM_NOUVEAU_CARD_INIT*/):
            return global_drm_device.driver->ioctls[drmCommandIndex].func(&global_drm_device, NULL, drm_files[fd]);
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
}

int
drmCommandRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    if (!drm_files[fd])
        return -EINVAL;

    if (!global_drm_device.driver || !global_drm_device.driver->ioctls)
        return -EINVAL;
    
    /* FIXME: Remove switch when all paths are tested */
    switch(drmCommandIndex)
    {
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
}

int
drmCommandWrite(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    if (!drm_files[fd])
        return -EINVAL;

    if (!global_drm_device.driver || !global_drm_device.driver->ioctls)
        return -EINVAL;
    
    /* FIXME: Remove switch when all paths are tested */
    switch(drmCommandIndex)
    {
        case(0x5 /*DRM_NOUVEAU_GROBJ_ALLOC*/):
        case(0x7 /*DRM_NOUVEAU_GPUOBJ_FREE*/):
        case(0x9 /*DRM_NOUVEAU_MEM_FREE*/):
            return global_drm_device.driver->ioctls[drmCommandIndex].func(&global_drm_device, data, drm_files[fd]);
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
}

int
drmCommandWriteRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    if (!drm_files[fd])
        return -EINVAL;
    
    if (!global_drm_device.driver || !global_drm_device.driver->ioctls)
        return -EINVAL;
    
    /* FIXME: Remove switch when all paths are tested */
    switch(drmCommandIndex)
    {
        case(0x1 /*DRM_NOUVEAU_GETPARAM*/):
        case(0x3 /*DRM_NOUVEAU_CHANNEL_ALLOC*/):
        case(0x6 /*DRM_NOUVEAU_NOTIFIEROBJ_ALLOC*/):
        case(0x8 /*DRM_NOUVEAU_MEM_ALLOC*/):
            return global_drm_device.driver->ioctls[drmCommandIndex].func(&global_drm_device, data, drm_files[fd]);
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
}
 
int           
drmMap(int fd, drm_handle_t handle, drmSize size, drmAddressPtr address)
{
    struct drm_map_list *entry;
    
    /* FIXME: will become extreemly slow with large number of allocations
    Original implementeation used hastabled to get quicker access to the
    needed drm_map_list */
  
    for (entry = (struct drm_map_list *)global_drm_device.maplist.next; 
    entry != (struct drm_map_list *)&global_drm_device.maplist; 
    entry = (struct drm_map_list *)entry->head.next)
    {
        if (entry->map && entry->user_token == handle)
        {
            switch(entry->map->type)
            {
                case(_DRM_FRAME_BUFFER):
                    /* HACK ? - map of this type was not ioremaped before */
                    drm_core_ioremap(entry->map, &global_drm_device);
                    /* FALLTHROUGH */
                case(_DRM_REGISTERS):
                    *address = entry->map->handle;
                    break;
                case(_DRM_SCATTER_GATHER):
                    *address = (void *)entry->map->offset;
                    break;
                default:
                    DRM_IMPL("Unhandled map->type\n");
                    *address = NULL;
            }
            
            if(*address)
                return 0;
            else
                return -EINVAL;
        }
    }
    
    return -EINVAL;
}

int
drmUnmap(drmAddress address, drmSize size)
{
    /* FIXME: Mappings of type _DRM_FRAME_BUFFER need unmapping here */
    /* Commented out so that it does not litter the debug output */
    /* DRM_IMPL("\n"); */
    return 0;
}

int
drmOpen(const char *name, const char *busid)
{
    int i;
    
    for (i = 0; i < 128; i++)
    {
        if (drm_files[i] == NULL)
        {
            drm_files[i] = AllocVec(sizeof(struct drm_file), MEMF_PUBLIC | MEMF_CLEAR);
            return i;
        }
    }
    
    return -EINVAL;
}

int
drmClose(int fd)
{
    struct drm_file * f = NULL;

    if (!(f = drm_files[fd]))
        return 0;
    
    drm_files[fd] = NULL;
    
    if (global_drm_device.driver->preclose)
        global_drm_device.driver->preclose(&global_drm_device, f);
    
    FreeVec(f);
    
    return 0;
}

drmVersionPtr
drmGetVersion(int fd)
{
    static drmVersion ver;
    if (global_drm_device.driver)
        ver.version_patchlevel = global_drm_device.driver->version_patchlevel;
    else
        ver.version_patchlevel = 0;
    
    return &ver;
}

void
drmFreeVersion(drmVersionPtr ptr)
{
    /* This is a no-op for now */
}

int
drmCreateContext(int fd, drm_context_t * handle)
{
    /* No Op */
    return 0;
}

int
drmDestroyContext(int fd, drm_context_t handle)
{
    /* No Op */
    return 0;
}

