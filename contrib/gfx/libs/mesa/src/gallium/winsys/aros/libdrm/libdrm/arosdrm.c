#include "arosdrm.h"
#include "drmP.h"
#include "nouveau_drm.h"
#include "nouveau_drv.h"

/* FIXME: This should implement generic approach - not card specific */

struct drm_device global_drm_device;
struct drm_file global_drm_file;

/* FIXME: This should be generic function table, not nouveau */
extern struct drm_ioctl_desc nouveau_ioctls[];

int 
drmCommandNone(int fd, unsigned long drmCommandIndex)
{
    switch(drmCommandIndex)
    {
        case(DRM_NOUVEAU_CARD_INIT):
            return nouveau_ioctl_card_init(&global_drm_device, NULL, &global_drm_file);
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
}

int
drmCommandRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
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
    switch(drmCommandIndex)
    {
        case(DRM_NOUVEAU_GROBJ_ALLOC):
            return nouveau_ioctl_grobj_alloc(&global_drm_device, data, &global_drm_file);
        case(DRM_NOUVEAU_GPUOBJ_FREE):
            return nouveau_ioctl_gpuobj_free(&global_drm_device, data, &global_drm_file);
        case(DRM_NOUVEAU_MEM_FREE):
            return nouveau_ioctl_mem_free(&global_drm_device, data, &global_drm_file);
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
}

int
drmCommandWriteRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    switch(drmCommandIndex)
    {
        case(DRM_NOUVEAU_GETPARAM):
            return nouveau_ioctl_getparam(&global_drm_device, data, &global_drm_file);
        case(DRM_NOUVEAU_CHANNEL_ALLOC):
            return nouveau_ioctls[DRM_NOUVEAU_CHANNEL_ALLOC].func(&global_drm_device, data, &global_drm_file);
        case(DRM_NOUVEAU_NOTIFIEROBJ_ALLOC):
            return nouveau_ioctl_notifier_alloc(&global_drm_device, data, &global_drm_file);
        case(DRM_NOUVEAU_MEM_ALLOC):
            return nouveau_ioctl_mem_alloc(&global_drm_device, data, &global_drm_file);
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
            
            if (entry->map->type == _DRM_FRAME_BUFFER)
            {
                /* HACK ? - map of this type was not ioremaped before */
                drm_core_ioremap(entry->map, &global_drm_device);
            }
            *address = entry->map->handle;
            return 0;
        }
    }
    
    return -EINVAL;
}

int
drmUnmap(drmAddress address, drmSize size)
{
    DRM_IMPL("\n");
    return 0;
}

int
drmOpen(const char *name, const char *busid)
{
    int ret;
    
    /* FIXME: Calling this the second time will most likelly crash. Fix it. */
    
    /* FIXME: Init code - should be moved to separate function */
    INIT_LIST_HEAD(&global_drm_device.maplist);
    global_drm_device.sg = NULL;
    global_drm_device.irq_enabled = 0;
    
    DRM_DEBUG("%s, %s\n", name, busid);
#if !defined(HOSTED_BUILD)    
    ret = drm_pci_find_supported_video_card(&global_drm_device);
    if (ret)
        return -1;
#endif

    ret = nouveau_load(&global_drm_device, 0);
    if (ret)
        return -1;

    ret = nouveau_firstopen(&global_drm_device);
    if (ret)
        return -1;

    return 4242; /*FIXME: some id just for now */
}

int
drmClose(int fd)
{
    /* FIXME: Calling this the second time will most likelly crash. Fix it. */
    
    nouveau_preclose(&global_drm_device, &global_drm_file);
    
    drm_lastclose(&global_drm_device);
    
    nouveau_unload(&global_drm_device);
    
    drm_pci_shutdown(&global_drm_device);
    
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
    DRM_IMPL("\n");
    return 0;
}

int
drmDestroyContext(int fd, drm_context_t handle)
{
    DRM_IMPL("\n");
    return 0;
}

