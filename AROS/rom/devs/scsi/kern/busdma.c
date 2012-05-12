/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <proto/exec.h>

#include <sys/conf.h>
#include <sys/systm.h>
#include <sys/bus_dma.h>

struct bus_dma_tag {
    bus_size_t dt_size;
};

int bus_dma_tag_create(bus_dma_tag_t parent, bus_size_t alignment, bus_size_t boundary, bus_addr_t lowaddr, bus_addr_t highaddr, bus_dma_filter_t *filter, void *filterarg, bus_size_t maxsize, int nsegments, bus_size_t maxsegsz, int flags, bus_dma_tag_t *dmat)
{
    *dmat = AllocVec(sizeof(**dmat), MEMF_ANY);
    if (*dmat == NULL)
        return -ENOMEM;

    (*dmat)->dt_size = maxsize;
    return 0;
}

int bus_dma_tag_destroy(bus_dma_tag_t tag)
{
    FreeVec(tag);
    return 0;
}

bus_size_t bus_dma_tag_getmaxsize(bus_dma_tag_t tag)
{
    return tag->dt_size;
}

int bus_dmamem_alloc(bus_dma_tag_t tag, void **vaddr, int flags, bus_dmamap_t *map)
{
    /* FIXME: Allocate DMAable memory */
    void *addr = AllocMem(tag->dt_size, MEMF_ANY);
    if (addr == NULL)
        return -ENOMEM;

    if (vaddr)
        *vaddr = addr;

    *map = addr;
    return 0;
}

void bus_dmamem_free(bus_dma_tag_t tag, void *vaddr, bus_dmamap_t map)
{
    FreeMem(map, tag->dt_size);
}


int bus_dmamap_create(bus_dma_tag_t tag, int flags, bus_dmamap_t *map)
{
    bus_dmamem_alloc(tag, NULL, 0, map);
    return 0;
}

int bus_dmamap_destroy(bus_dma_tag_t tag, bus_dmamap_t map)
{
    bus_dmamem_free(tag, NULL, map);
    return 0;
}

int bus_dmamap_load(bus_dma_tag_t tag, bus_dmamap_t map, void *data, size_t len, bus_dmamap_callback_t *callback, void *info, int flags)
{
    bus_dma_segment_t seg = { .ds_addr = (bus_addr_t)data, .ds_len = (bus_size_t)len };
    callback(info, &seg, 1, 0);
    return 0;
}

void _bus_dmamap_sync(bus_dma_tag_t dmat, bus_dmamap_t map, bus_dmasync_op_t op)
{
    /* FIXME */
}

void _bus_dmamap_unload(bus_dma_tag_t dmat, bus_dmamap_t map)
{
    /* FIXME */
}

