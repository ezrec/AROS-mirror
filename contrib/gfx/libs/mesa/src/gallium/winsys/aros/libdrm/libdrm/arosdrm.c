#include "arosdrm.h"
#include "drmP.h"
#include "nouveau_drm.h"
#include "nouveau_drv.h"

/* FIXME: This should implement generic approach - not card specific */

struct drm_device global_drm_device;
/* FIXME: Array for now, list maybe in future */
struct drm_file * drm_files[128] = {NULL};

/* FIXME: This should be generic function table, not nouveau */
extern struct drm_ioctl_desc nouveau_ioctls[];

int 
drmCommandNone(int fd, unsigned long drmCommandIndex)
{
    if (!drm_files[fd])
        return -EINVAL;
    
    switch(drmCommandIndex)
    {
        case(DRM_NOUVEAU_CARD_INIT):
            return nouveau_ioctl_card_init(&global_drm_device, NULL, drm_files[fd]);
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
    
    switch(drmCommandIndex)
    {
        case(DRM_NOUVEAU_GROBJ_ALLOC):
            return nouveau_ioctl_grobj_alloc(&global_drm_device, data, drm_files[fd]);
        case(DRM_NOUVEAU_GPUOBJ_FREE):
            return nouveau_ioctl_gpuobj_free(&global_drm_device, data, drm_files[fd]);
        case(DRM_NOUVEAU_MEM_FREE):
            return nouveau_ioctl_mem_free(&global_drm_device, data, drm_files[fd]);
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
    
    switch(drmCommandIndex)
    {
        case(DRM_NOUVEAU_GETPARAM):
            return nouveau_ioctl_getparam(&global_drm_device, data, drm_files[fd]);
        case(DRM_NOUVEAU_CHANNEL_ALLOC):
            return nouveau_ioctls[DRM_NOUVEAU_CHANNEL_ALLOC].func(&global_drm_device, data, drm_files[fd]);
        case(DRM_NOUVEAU_NOTIFIEROBJ_ALLOC):
            return nouveau_ioctl_notifier_alloc(&global_drm_device, data, drm_files[fd]);
        case(DRM_NOUVEAU_MEM_ALLOC):
            return nouveau_ioctl_mem_alloc(&global_drm_device, data, drm_files[fd]);
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
    
    nouveau_preclose(&global_drm_device, f);
    
    FreeVec(f);
    
    return 0;
}

drmVersionPtr
drmGetVersion(int fd)
{
    static drmVersion ver;
    ver.version_patchlevel = NOUVEAU_DRM_HEADER_PATCHLEVEL;
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

