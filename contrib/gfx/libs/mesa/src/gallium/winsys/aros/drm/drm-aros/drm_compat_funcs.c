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

/* HACK: This a mighty hack :( */
#include <proto/oop.h>

APTR drm_aros_pci_ioremap(OOP_Object *driver, APTR buf, IPTR size);
void drm_aros_pci_iounmap(OOP_Object *driver, APTR buf, IPTR size);
extern OOP_Object * hack_pci_driver_hack;

void  *ioremap_nocache(resource_size_t offset, unsigned long size)
{
    return drm_aros_pci_ioremap(hack_pci_driver_hack, (APTR)offset, size);
}

void iounmap_helper(void * addr, unsigned long size)
{
    return drm_aros_pci_iounmap(hack_pci_driver_hack, (APTR) addr, size);
}
/* HACK ends */

/* Bit operations */
void clear_bit(int nr, volatile void * addr)
{
    unsigned long mask = 1 << nr;
    
    *(unsigned long*)addr &= ~mask;
}

void set_bit(int nr, volatile void *addr)
{
    unsigned long mask = 1 << nr;
    
    *(unsigned long*)addr |= mask;
}


/* Page handling */
void __free_page(struct page * p)
{
    if (p->allocated_buffer)
        FreeVec(p->allocated_buffer);
    p->allocated_buffer = NULL;
    p->address = NULL;
    FreeVec(p);
}

struct page * create_page_helper()
{
    struct page * p;
    p = AllocVec(sizeof(*p), MEMF_PUBLIC | MEMF_CLEAR);
    p->allocated_buffer = AllocVec(PAGE_SIZE + PAGE_SIZE - 1, MEMF_PUBLIC | MEMF_CLEAR);
    p->address = PAGE_ALIGN(p->allocated_buffer);
    return p;
}

/* IDR handling */
int idr_pre_get_internal(struct idr *idp)
{
    if (idp->size == idp->occupied)
    {
        /* Create new table */
        ULONG newsize = idp->size ? idp->size * 2 : 128;
        IPTR * newtab = AllocVec(newsize * sizeof(IPTR), MEMF_PUBLIC | MEMF_CLEAR);
        
        if (newtab == NULL)
            return 0;
        
        
        if (idp->pointers)
        {
            /* Copy old table into new */
            CopyMem(idp->pointers, newtab, idp->size * sizeof(IPTR));
            
            /* Release old table */
            FreeVec(idp->pointers);
        }
        
        idp->pointers = newtab;
        idp->size = newsize;
    }
    
    return 1;
}

int idr_get_new_above(struct idr *idp, void *ptr, int starting_id, int *id)
{
    int i = starting_id;

    for(;i < idp->size;i++)
    {
        if (idp->pointers[i] == (IPTR)NULL)
        {
            *id = i;
            idp->pointers[i] = (IPTR)ptr;
            idp->occupied++;
            return 0;
        }
    }
    
    return -EAGAIN;
}

void *idr_find(struct idr *idp, int id)
{
    if ((id < idp->size) && (id >= 0))
        return (APTR)idp->pointers[id];
    else
        return NULL;
}

void idr_remove(struct idr *idp, int id)
{
    if ((id < idp->size) && (id >= 0))
    {
        idp->pointers[id] = (IPTR)NULL;
        idp->occupied--;
    }
}

void idr_init(struct idr *idp)
{
    idp->size = 0;
    idp->pointers = NULL;
    idp->occupied = 0;
}
