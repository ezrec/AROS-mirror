#include "arosdrm.h"
#include "drmP.h"

extern struct drm_driver * current_drm_driver;

/* FIXME: Array for now, list maybe in future */
struct drm_file * drm_files[128] = {NULL};

int 
drmCommandNone(int fd, unsigned long drmCommandIndex)
{
    if (!drm_files[fd])
        return -EINVAL;

    if (!current_drm_driver || !current_drm_driver->ioctls)
        return -EINVAL;

    return current_drm_driver->ioctls[drmCommandIndex].func(current_drm_driver->dev, NULL, drm_files[fd]);
}

int
drmCommandRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    if (!drm_files[fd])
        return -EINVAL;

    if (!current_drm_driver || !current_drm_driver->ioctls)
        return -EINVAL;

    return current_drm_driver->ioctls[drmCommandIndex].func(current_drm_driver->dev, data, drm_files[fd]);
}

int
drmCommandWrite(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    if (!drm_files[fd])
        return -EINVAL;

    if (!current_drm_driver || !current_drm_driver->ioctls)
        return -EINVAL;
    
    return current_drm_driver->ioctls[drmCommandIndex].func(current_drm_driver->dev, data, drm_files[fd]);
}

int
drmCommandWriteRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    if (!drm_files[fd])
        return -EINVAL;
    
    if (!current_drm_driver || !current_drm_driver->ioctls)
        return -EINVAL;
    
    return current_drm_driver->ioctls[drmCommandIndex].func(current_drm_driver->dev, data, drm_files[fd]);
}
 
//int           
//drmMap(int fd, drm_handle_t handle, drmSize size, drmAddressPtr address)
//{
//    struct drm_map_list *entry;
//    
//    /* FIXME: will become extreemly slow with large number of allocations
//    Original implementeation used hastabled to get quicker access to the
//    needed drm_map_list */
//  
//    for (entry = (struct drm_map_list *)global_drm_device.maplist.next; 
//    entry != (struct drm_map_list *)current_drm_driver->dev.maplist; 
//    entry = (struct drm_map_list *)entry->head.next)
//    {
//        if (entry->map && entry->user_token == handle)
//        {
//            switch(entry->map->type)
//            {
//                case(_DRM_FRAME_BUFFER):
//                    /* HACK ? - map of this type was not ioremaped before */
//                    drm_core_ioremap(entry->map, current_drm_driver->dev);
//                    /* FALLTHROUGH */
//                case(_DRM_REGISTERS):
//                    *address = entry->map->handle;
//                    break;
//                case(_DRM_SCATTER_GATHER):
//                    *address = (void *)entry->map->offset;
//                    break;
//                default:
//                    DRM_IMPL("Unhandled map->type\n");
//                    *address = NULL;
//            }
//            
//            if(*address)
//                return 0;
//            else
//                return -EINVAL;
//        }
//    }
//    
//    return -EINVAL;

//int
//drmUnmap(drmAddress address, drmSize size)
//{
//    /* FIXME: Mappings of type _DRM_FRAME_BUFFER need unmapping here */
//    /* Commented out so that it does not litter the debug output */
//    /* DRM_IMPL("\n"); */
//    return 0;
//}

int
drmOpen(const char *name, const char *busid)
{
    int i;

    for (i = 0; i < 128; i++)
    {
        if (drm_files[i] == NULL)
        {
            drm_files[i] = AllocVec(sizeof(struct drm_file), MEMF_PUBLIC | MEMF_CLEAR);
            spin_lock_init(&drm_files[i]->table_lock);
            if (current_drm_driver->open)
                current_drm_driver->open(current_drm_driver->dev, drm_files[i]);
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
    
    if (current_drm_driver->preclose)
        current_drm_driver->preclose(current_drm_driver->dev, f);

    if (current_drm_driver->postclose)
        current_drm_driver->postclose(current_drm_driver->dev, f);

    FreeVec(f);
    
    return 0;
}

drmVersionPtr
drmGetVersion(int fd)
{
    static drmVersion ver;
    if (current_drm_driver)
        ver.version_patchlevel = current_drm_driver->version_patchlevel;
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

int drmIoctl(int fd, unsigned long request, void *arg)
{
    int ret = -EINVAL;
    
    if (!drm_files[fd])
        return ret;

    do
    {
        switch(request)
        {
            case(DRM_IOCTL_GEM_CLOSE):
                ret = drm_gem_close_ioctl(current_drm_driver->dev, arg, drm_files[fd]);
                break;
            case(DRM_IOCTL_GEM_OPEN):
                ret = drm_gem_open_ioctl(current_drm_driver->dev, arg, drm_files[fd]);
                break;
            case(DRM_IOCTL_GEM_FLINK):
                ret = drm_gem_flink_ioctl(current_drm_driver->dev, arg, drm_files[fd]);
                break;
            default:
                DRM_IMPL("GEM COMMAND %d\n", request);
        }
        /* FIXME: It is possible that -ERESTARTSYS needs to be translated to -EAGAIN here */
    } while (ret == -EINTR || ret == -EAGAIN);
    
    return ret;
}

/* FIXME: this should be generic, not nouveau specific */
#include "nouveau_drv.h"
void * drmMMap(int fd, uint32_t handle)
{
    struct drm_file * f = drm_files[fd];
    struct drm_gem_object * gem_object = NULL;
    struct nouveau_bo * nvbo = NULL;
    void * addr = NULL;
    
    if (!f)
        return NULL;
    
    /* Get GEM objects from handle */
    gem_object = drm_gem_object_lookup(current_drm_driver->dev, f, handle);
    if (!gem_object)
        return NULL;
    
    /* Translate to nouveau_bo */
    nvbo = nouveau_gem_object(gem_object);
    
    if (nvbo)
    {
        /* Perform mapping if not already done */
        if (!nvbo->kmap.virtual)
            nouveau_bo_map(nvbo);
        
        addr = nvbo->kmap.virtual;
    }
    
    /* Release the acquired reference */
    mutex_lock(&current_drm_driver->dev->struct_mutex);
    drm_gem_object_unreference(gem_object);
    mutex_unlock(&current_drm_driver->dev->struct_mutex);    
    
    /* Return virtual address */
    return addr;
}

void drmMUnmap(int fd, uint32_t handle)
{
    struct drm_file * f = drm_files[fd];
    struct drm_gem_object * gem_object = NULL;
    struct nouveau_bo * nvbo = NULL;
   
    if (!f) return ;
    
    /* Get GEM objects from handle */
    gem_object = drm_gem_object_lookup(current_drm_driver->dev, f, handle);
    if (!gem_object) return;
    
    /* Translate to nouveau_bo */
    nvbo = nouveau_gem_object(gem_object);
    if (nvbo)
    {
        /* Perform unmapping if not already done */
        if (nvbo->kmap.virtual)
            nouveau_bo_unmap(nvbo);
    }
    
    /* Release the acquired reference */
    mutex_lock(&current_drm_driver->dev->struct_mutex);
    drm_gem_object_unreference(gem_object);
    mutex_unlock(&current_drm_driver->dev->struct_mutex);
}

/* HACK */
#undef DRIVER_NAME
#undef DRIVER_DESC
#undef DRIVER_AUTHOR
#undef DRIVER_DATE
#undef DRIVER_MINOR
#undef DRIVER_MAJOR
#undef DRIVER_PATCHLEVEL
#define mem_block mem_blockr /* HACK: Really nasty hack - nouvea_drv.h also contains defintion of mem_block */
#include "i915_drm.h"
#include "i915_drv.h"

int i915_getparam(struct drm_device *dev, void *data,
			 struct drm_file *file_priv);

int drmIntelIoctlEmul(int fildes, int request, void * arg)
{
    int ret = -EINVAL;
    
    if (!drm_files[fildes])
        return ret;

    switch(request)
    {
    case(DRM_IOCTL_I915_GEM_GET_APERTURE):
        return i915_gem_get_aperture_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GETPARAM):
        return i915_getparam(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_CREATE):
        return i915_gem_create_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_MMAP):
        return i915_gem_mmap_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_SET_DOMAIN):
        return i915_gem_set_domain_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_SW_FINISH):
        return i915_gem_sw_finish_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_SET_TILING):
        return i915_gem_set_tiling(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_PWRITE):
        return i915_gem_pwrite_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_EXECBUFFER):
        return i915_gem_execbuffer(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_MADVISE):
        return i915_gem_madvise_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_BUSY):
        return i915_gem_busy_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    case(DRM_IOCTL_GEM_CLOSE):
        return drm_gem_close_ioctl(current_drm_driver->dev, arg, drm_files[fildes]);
    default:
        asm("int3");
        DRM_IMPL("IOCTL: %d -> %d\n", fildes, request);
        break;
    }
    
    return ret;
}

