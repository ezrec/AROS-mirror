#include "drmP.h"
#include "ttm/ttm_bo_api.h"
#include "ttm/ttm_bo_driver.h"
#include "ttm/ttm_placement.h"
#include "ttm/ttm_memory.h"
#include "ttm/ttm_module.h"

void drm_sg_cleanup(struct drm_sg_mem * entry)
{
    asm("int3");
}

void nouveau_preclose(struct drm_device *dev, struct drm_file *file_priv)
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
    return 0;
}

void ttm_bo_kunmap(struct ttm_bo_kmap_obj *map)
{
    asm("int3");
}

struct ttm_backend *ttm_agp_backend_init(struct ttm_bo_device *bdev,
                        struct agp_bridge_data *bridge)
{
    asm("int3");
    return NULL;
}
int
nouveau_fence_wait(void *sync_obj, void *sync_arg, bool lazy, bool intr)
{
    asm("int3");
    return 0;
}

int ttm_bo_move_ttm(struct ttm_buffer_object *bo,
               bool evict, bool no_wait,
               struct ttm_mem_reg *new_mem)
{
    asm("int3");
    return 0;
}

void ttm_bo_global_release(struct ttm_global_reference *ref)
{
    asm("int3");
}

void ttm_mem_global_release(struct ttm_mem_global *glob)
{
    asm("int3");
}

void ttm_tt_destroy(struct ttm_tt *ttm)
{
    asm("int3");
}

int ttm_bo_swapout(struct ttm_mem_shrink *shrink)
{
    asm("int3");;
    return 0;
}

int ttm_tt_swapin(struct ttm_tt *ttm)
{
    asm("int3");
    return 0;
}

void drm_clflush_pages(struct page *pages[], unsigned long num_pages)
{
    asm("int3");
}

struct drm_gem_object *
drm_gem_object_lookup(struct drm_device *dev, struct drm_file *filp,
              u32 handle)
{
    asm("int3");
    return NULL;
}

int
drm_gem_handle_create(struct drm_file *file_priv,
               struct drm_gem_object *obj,
               u32 *handlep)
{
    asm("int3");
    return 0;
}

int ttm_bo_wait_cpu(struct ttm_buffer_object *bo, bool no_wait)
{
    asm("int3");
    return 0;
}

int ttm_bo_wait_unreserved(struct ttm_buffer_object *bo, bool interruptible)
{
    asm("int3");
    return 0;
}

int ttm_bo_synccpu_write_grab(struct ttm_buffer_object *bo, bool no_wait)
{
    asm("int3");
    return 0;
}

void ttm_bo_synccpu_write_release(struct ttm_buffer_object *bo)
{
    asm("int3");
}
