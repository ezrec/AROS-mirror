/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef MACHINE_BUS_DMA_H
#define MACHINE_BUS_DMA_H

#include <machine/aros.h>

typedef IPTR    bus_addr_t;
typedef IPTR    bus_size_t;

typedef IPTR    bus_space_tag_t;
typedef IPTR    bus_space_handle_t;

#define BUS_SPACE_MAXADDR       (~((IPTR)0))
#define BUS_SPACE_MAXADDR_32BIT 0xFFFFFFFF

#define BUS_SPACE_BARRIER_READ  0x01
#define BUS_SPACE_BARRIER_WRITE 0x02

static inline void bus_space_barrier(bus_space_tag_t tag __unused, bus_space_handle_t bsh __unused, bus_size_t offset __unused, bus_size_t len __unused, int flags)
{
}

static inline void *bus_space_kva(bus_space_tag_t tag, bus_space_handle_t handle, bus_size_t offset)
{
    return ((void *)handle + offset);
}

static inline int bus_space_subregion(bus_space_tag_t t, bus_space_handle_t bsh, bus_size_t offset, bus_size_t size, bus_space_handle_t *nbshp)
{
    *nbshp = bsh + offset;
    return 0;
}

static inline u_int32_t bus_space_read_4(bus_space_tag_t tag, bus_space_handle_t handle, bus_size_t offset)
{
    return AROS_LE2LONG(*(volatile u_int32_t *)(handle + offset));
}

static inline void bus_space_write_4(bus_space_tag_t tag, bus_space_handle_t bsh, bus_size_t offset, u_int32_t value)
{
    *(volatile u_int32_t *)(bsh + offset) = AROS_LONG2LE(value);
}

#endif /* MACHINE_BUS_DMA_H */
