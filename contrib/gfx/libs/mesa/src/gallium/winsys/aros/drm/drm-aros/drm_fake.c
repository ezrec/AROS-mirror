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

int nouveau_firstopen(struct drm_device *dev)
{
    asm("int3");
    return 0;
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

struct drm_ioctl_desc * nouveau_ioctls;

void
nouveau_irq_preinstall(struct drm_device *dev)
{
    asm("int3");
}

int
nouveau_irq_postinstall(struct drm_device *dev)
{
    asm("int3");
    return 0;
}

void
nouveau_irq_uninstall(struct drm_device *dev)
{
    asm("int3");
}

irqreturn_t
nouveau_irq_handler(DRM_IRQ_ARGS)
{
    asm("int3");
}

struct drm_gem_object *drm_gem_object_alloc(struct drm_device *dev, size_t size)
{
    asm("int3");
    return NULL;
}

int ttm_bo_kmap(struct ttm_buffer_object *bo, unsigned long start_page,
               unsigned long num_pages, struct ttm_bo_kmap_obj *map)
{
    asm("int3");
    return 0;
}

void ttm_bo_kunmap(struct ttm_bo_kmap_obj *map)
{
    asm("int3");
}

int ttm_bo_reserve(struct ttm_buffer_object *bo,
              bool interruptible,
              bool no_wait, bool use_sequence, uint32_t sequence)
{
    asm("int3");
    return 0;
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

int ttm_bo_move_accel_cleanup(struct ttm_buffer_object *bo,
                     void *sync_obj,
                     void *sync_obj_arg,
                     bool evict, bool no_wait,
                     struct ttm_mem_reg *new_mem)
{
    asm("int3");
    return 0;
}

int ttm_bo_move_memcpy(struct ttm_buffer_object *bo,
                  bool evict,
                  bool no_wait, struct ttm_mem_reg *new_mem)
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

void ttm_bo_unmap_virtual(struct ttm_buffer_object *bo)
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

int ttm_mem_global_alloc_page(struct ttm_mem_global *glob,
                  struct page *page,
                  bool no_wait, bool interruptible)
{
    asm("int3");
    return 0;
}

void drm_clflush_pages(struct page *pages[], unsigned long num_pages)
{
    asm("int3");
}
