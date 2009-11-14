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

void ttm_bo_unref(struct ttm_buffer_object **bo)
{
    asm("int3");
}

int ttm_buffer_object_init(struct ttm_bo_device *bdev,
                  struct ttm_buffer_object *bo,
                  unsigned long size,
                  enum ttm_bo_type type,
                  uint32_t flags,
                  uint32_t page_alignment,
                  unsigned long buffer_start,
                  bool interrubtible,
                  struct file *persistant_swap_storage,
                  size_t acc_size,
                  void (*destroy) (struct ttm_buffer_object *))
{
    asm("int3");
    return 0;
}

int ttm_buffer_object_validate(struct ttm_buffer_object *bo,
                      uint32_t proposed_placement,
                      bool interruptible, bool no_wait)
{
    asm("int3");
    return 0;
}

int ttm_bo_reserve(struct ttm_buffer_object *bo,
              bool interruptible,
              bool no_wait, bool use_sequence, uint32_t sequence)
{
    asm("int3");
    return 0;
}

void ttm_bo_unreserve(struct ttm_buffer_object *bo)
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

int ttm_bo_device_init(struct ttm_bo_device *bdev,
                  struct ttm_bo_global *glob,
                  struct ttm_bo_driver *driver,
                  uint64_t file_page_offset, bool need_dma32)
{
    asm("int3");
    return 0;
}

int ttm_bo_init_mm(struct ttm_bo_device *bdev, unsigned type,
              unsigned long p_offset, unsigned long p_size)
{
    asm("int3");
    return 0;
}

int ttm_bo_mem_space(struct ttm_buffer_object *bo,
                uint32_t proposed_placement,
                struct ttm_mem_reg *mem,
                bool interruptible, bool no_wait)
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

int ttm_tt_bind(struct ttm_tt *ttm, struct ttm_mem_reg *bo_mem)
{
    asm("int3");
    return 0;
}

int nouveau_ttm_global_init(struct drm_nouveau_private * nvpriv)
{
    asm("int3");
    return 0;
}

int nouveau_sgdma_init(struct drm_device * dev)
{
    asm("int3");
    return 0;
}

struct ttm_backend *nouveau_sgdma_init_ttm(struct drm_device * dev)
{
    asm("int3");
    return NULL;
}

void drm_mm_put_block(struct drm_mm_node * cur)
{
    asm("int3");
}