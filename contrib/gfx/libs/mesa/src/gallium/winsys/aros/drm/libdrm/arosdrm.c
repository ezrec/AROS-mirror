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
        case(0x4 /*DRM_NOUVEAU_CHANNEL_FREE*/):
        case(0x5 /*DRM_NOUVEAU_GROBJ_ALLOC*/):
        case(0x7 /*DRM_NOUVEAU_GPUOBJ_FREE*/):
        case(0x45 /*DRM_NOUVEAU_GEM_CPU_PREP*/):
        case(0x46 /*DRM_NOUVEAU_GEM_CPU_FINI*/):
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
        case(0x40 /*DRM_NOUVEAU_GEM_NEW*/):
        case(0x42 /*DRM_NOUVEAU_GEM_PUSHBUF_CALL*/):
        case(0x43 /*DRM_NOUVEAU_GEM_PIN*/):
        case(0x47 /*DRM_NOUVEAU_GEM_INFO*/):
        case(0x48 /* DRM_NOUVEAU_GEM_PUSHBUF_CALL2 */):
            return global_drm_device.driver->ioctls[drmCommandIndex].func(&global_drm_device, data, drm_files[fd]);
        default:
            DRM_IMPL("COMMAND %d\n", drmCommandIndex);
    }
    
    return 0;
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
//    entry != (struct drm_map_list *)&global_drm_device.maplist; 
//    entry = (struct drm_map_list *)entry->head.next)
//    {
//        if (entry->map && entry->user_token == handle)
//        {
//            switch(entry->map->type)
//            {
//                case(_DRM_FRAME_BUFFER):
//                    /* HACK ? - map of this type was not ioremaped before */
//                    drm_core_ioremap(entry->map, &global_drm_device);
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
            if (global_drm_device.driver->open)
                global_drm_device.driver->open(&global_drm_device, drm_files[i]);
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

    if (global_drm_device.driver->postclose)
        global_drm_device.driver->postclose(&global_drm_device, f);

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
                ret = drm_gem_close_ioctl(&global_drm_device, arg, drm_files[fd]);
                break;
            case(DRM_IOCTL_GEM_OPEN):
                ret = drm_gem_open_ioctl(&global_drm_device, arg, drm_files[fd]);
                break;
            case(DRM_IOCTL_GEM_FLINK):
                ret = drm_gem_flink_ioctl(&global_drm_device, arg, drm_files[fd]);
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
    gem_object = drm_gem_object_lookup(&global_drm_device, f, handle);
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
    mutex_lock(&global_drm_device.struct_mutex);
    drm_gem_object_unreference(gem_object);
    mutex_unlock(&global_drm_device.struct_mutex);    
    
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
    gem_object = drm_gem_object_lookup(&global_drm_device, f, handle);
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
    mutex_lock(&global_drm_device.struct_mutex);
    drm_gem_object_unreference(gem_object);
    mutex_unlock(&global_drm_device.struct_mutex);
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
        return i915_gem_get_aperture_ioctl(&global_drm_device, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GETPARAM):
        return i915_getparam(&global_drm_device, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_CREATE):
        return i915_gem_create_ioctl(&global_drm_device, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_MMAP):
        return i915_gem_mmap_ioctl(&global_drm_device, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_SET_DOMAIN):
        return i915_gem_set_domain_ioctl(&global_drm_device, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_SW_FINISH):
        return i915_gem_sw_finish_ioctl(&global_drm_device, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_SET_TILING):
        return i915_gem_set_tiling(&global_drm_device, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_PWRITE):
        return i915_gem_pwrite_ioctl(&global_drm_device, arg, drm_files[fildes]);
    case(DRM_IOCTL_I915_GEM_EXECBUFFER):
        return i915_gem_execbuffer(&global_drm_device, arg, drm_files[fildes]);
    default:
        asm("int3");
        DRM_IMPL("IOCTL: %d -> %d\n", fildes, request);
        break;
    }
    
    return ret;
}

