/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "drm_compat_funcs.h"

#define PIO_RESERVED                (IPTR)0x40000UL

void iowrite32(u32 val, void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        writel(val, addr);
    else
        asm("int3");
}

unsigned int ioread32(void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        return readl(addr);
    else
        asm("int3");
    
    return 0xffffffff;
}

void iowrite16(u16 val, void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        writew(val, addr);
    else
        asm("int3");
}

unsigned int ioread16(void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        return readw(addr);
    else
        asm("int3");
    
    return 0xffff;
}

void iowrite8(u8 val, void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        writeb(val, addr);
    else
        asm("int3");
}

unsigned int ioread8(void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        return readb(addr);
    else
        asm("int3");
    
    return 0xff;
}

/* FIXME:These functions should guarantee atomic access to counter */
void kref_init(struct kref *kref)
{
    kref->count = 1;
}

void kref_get(struct kref *kref)
{
    kref->count++;
}

int kref_put(struct kref *kref, void (*release) (struct kref *kref))
{
    kref->count--;
    if (kref->count < 1)
    {
        release(kref);
        return 0;
    }
        return kref->count;
}
