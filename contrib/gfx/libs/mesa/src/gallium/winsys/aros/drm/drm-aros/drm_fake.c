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

struct ttm_backend *ttm_agp_backend_init(struct ttm_bo_device *bdev,
                        struct agp_bridge_data *bridge)
{
    asm("int3");
    return NULL;
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
