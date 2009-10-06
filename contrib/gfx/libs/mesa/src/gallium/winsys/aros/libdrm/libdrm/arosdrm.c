#include "arosdrm.h"
#include "drmP.h"
#include "nouveau_drm.h"
#include "nouveau_drv.h"

/* FIXME: This should implement generic approach - not card specific */

struct drm_device global_drm_device;

int 
drmCommandNone(int fd, unsigned long drmCommandIndex)
{
    switch(drmCommandIndex)
    {
        case(DRM_NOUVEAU_CARD_INIT):
            return nouveau_ioctl_card_init(&global_drm_device, NULL, NULL);
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
            return nouveau_ioctl_grobj_alloc(&global_drm_device, data, NULL);
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
}

/* FIXME: temporary, should be in some header, later dynamically invoked from driver object */
int exported_nouveau_ioctl_fifo_alloc(struct drm_device *dev, void *data,
                    struct drm_file *file_priv);
int
drmCommandWriteRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    switch(drmCommandIndex)
    {
        case(DRM_NOUVEAU_GETPARAM):
            return nouveau_ioctl_getparam(&global_drm_device, data, NULL);
        case(DRM_NOUVEAU_CHANNEL_ALLOC):
            /* FIXME: What to pass as third argument? */
            return exported_nouveau_ioctl_fifo_alloc(&global_drm_device, data, NULL);
        case(DRM_NOUVEAU_NOTIFIEROBJ_ALLOC):
            return nouveau_ioctl_notifier_alloc(&global_drm_device, data, NULL);
        case(DRM_NOUVEAU_MEM_ALLOC):
            /* FIXME: What to pass as third argument? */
            return nouveau_ioctl_mem_alloc(&global_drm_device, data, NULL);
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
}
 
int           
drmMap(int fd, drm_handle_t handle, drmSize size, drmAddressPtr address)
{
    struct drm_map_list *entry;
    
    /* This function should call mmap - map a portion of user space onto
    kernel allocated buffer. Since AROS does not distinguish kernel-user, 
    we just return to the caller the address of "kernel" allocated memory */
    
    /* FIXME: will become extreemly slow with large number of allocations
    Original implementeation used hastabled to get quicker access to the
    needed drm_map_list */
    
    for (entry = (struct drm_map_list *)global_drm_device.maplist.next; 
    entry != (struct drm_map_list *)&global_drm_device.maplist; 
    entry = (struct drm_map_list *)entry->head.next)
    {
        if (entry->map && entry->user_token == handle)
        {
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
    DRM_IMPL("\n");
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

