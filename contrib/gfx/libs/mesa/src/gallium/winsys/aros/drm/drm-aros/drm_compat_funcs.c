/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "drm_compat_funcs.h"
#include "drm_aros_config.h"

#define PIO_RESERVED                (IPTR)0x40000UL

void iowrite32(u32 val, void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        writel(val, addr);
    else
        IMPLEMENT("PIO\n");
}

unsigned int ioread32(void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        return readl(addr);
    else
        IMPLEMENT("PIO\n");
    
    return 0xffffffff;
}

void iowrite16(u16 val, void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        writew(val, addr);
    else
        IMPLEMENT("PIO\n");
}

unsigned int ioread16(void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        return readw(addr);
    else
        IMPLEMENT("PIO\n");
    
    return 0xffff;
}

void iowrite8(u8 val, void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        writeb(val, addr);
    else
        IMPLEMENT("PIO\n");
}

unsigned int ioread8(void * addr)
{
    if ((IPTR)addr >= PIO_RESERVED)
        return readb(addr);
    else
        IMPLEMENT("PIO\n");
    
    return 0xff;
}

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

#include "drm_aros.h"
#include <proto/oop.h>
#include <hidd/pci.h>
#include <hidd/hidd.h>

void *ioremap(resource_size_t offset, unsigned long size)
{
#if !defined(HOSTED_BUILD)
    if (pciDriver)
    {
        struct pHidd_PCIDriver_MapPCI mappci,*msg = &mappci;
        mappci.mID = OOP_GetMethodID(IID_Hidd_PCIDriver, moHidd_PCIDriver_MapPCI);
        mappci.PCIAddress = (APTR)offset;
        mappci.Length = size;
        return (APTR)OOP_DoMethod(pciDriver, (OOP_Msg)msg);
    }
    else
    {
        bug("BUG: ioremap used without acquiring pciDriver\n");
        return NULL;
    }
#else
    /* For better simulation:
    a) make a list of already "mapped" buffers keyed by APTR buf
    b) check if a request (buf + size) is inside of already mapped region -> return pointer in mapped region
    Why: sometimes the same range is mapped more than once
    */
    return AllocVec(size, MEMF_PUBLIC | MEMF_CLEAR); /* This will leak */
#endif
}

void iounmap(void * addr)
{
#if !defined(HOSTED_BUILD)
    if (pciDriver)
    {
        struct pHidd_PCIDriver_UnmapPCI unmappci,*msg=&unmappci;

        unmappci.mID = OOP_GetMethodID(IID_Hidd_PCIDriver, moHidd_PCIDriver_UnmapPCI);
        unmappci.CPUAddress = addr;
        unmappci.Length = 0; /* This works on i386 but may create problems on other archs */

        OOP_DoMethod(pciDriver, (OOP_Msg)msg);
    }
#else
    /* If "better simulation" is implemented (see ioremap) memory
    can only be freed if there is no other mappings to this buffer */
    FreeVec(addr);
#endif
}

resource_size_t pci_resource_start(void * pdev, unsigned int resource)
{
#if !defined(HOSTED_BUILD)    
    APTR start = (APTR)NULL;
    switch(resource)
    {
        case(0): OOP_GetAttr(pdev, aHidd_PCIDevice_Base0, (APTR)&start); break;
        case(1): OOP_GetAttr(pdev, aHidd_PCIDevice_Base1, (APTR)&start); break;
        case(2): OOP_GetAttr(pdev, aHidd_PCIDevice_Base2, (APTR)&start); break;
        case(3): OOP_GetAttr(pdev, aHidd_PCIDevice_Base3, (APTR)&start); break;
        case(4): OOP_GetAttr(pdev, aHidd_PCIDevice_Base4, (APTR)&start); break;
        case(5): OOP_GetAttr(pdev, aHidd_PCIDevice_Base5, (APTR)&start); break;
        default: bug("ResourceID %d not supported\n", resource);
    }
    
    return (resource_size_t)start;
#else
#if HOSTED_BUILD_CHIPSET >= 0x40
if (resource == 0) return (resource_size_t)0xcf000000;
if (resource == 1) return (resource_size_t)0xb0000000;
if (resource == 3) return (resource_size_t)0xce000000;
#else
if (resource == 0) return (resource_size_t)0xe7000000;
if (resource == 1) return (resource_size_t)0xf0000000;
if (resource == 2) return (resource_size_t)0xef800000;
#endif
return (resource_size_t)0;
#endif
}

unsigned long pci_resource_len(void * pdev, unsigned int resource)
{
#if !defined(HOSTED_BUILD)    
    IPTR len = (IPTR)0;
    
    if (pci_resource_start(pdev, resource) != 0)
    {
        /* 
         * FIXME:
         * The length reading is only correct when the resource actually exists.
         * pci.hidd can however return a non 0 lenght for a resource that does
         * not exsist. Possible fix in pci.hidd needed
         */
        
        switch(resource)
        {
            case(0): OOP_GetAttr(pdev, aHidd_PCIDevice_Size0, (APTR)&len); break;
            case(1): OOP_GetAttr(pdev, aHidd_PCIDevice_Size1, (APTR)&len); break;
            case(2): OOP_GetAttr(pdev, aHidd_PCIDevice_Size2, (APTR)&len); break;
            case(3): OOP_GetAttr(pdev, aHidd_PCIDevice_Size3, (APTR)&len); break;
            case(4): OOP_GetAttr(pdev, aHidd_PCIDevice_Size4, (APTR)&len); break;
            case(5): OOP_GetAttr(pdev, aHidd_PCIDevice_Size5, (APTR)&len); break;
            default: bug("ResourceID %d not supported\n", resource);
        }
    }
    
    return len;
#else
#if HOSTED_BUILD_CHIPSET >= 0x40
if (resource == 0) return (IPTR)0x1000000;
if (resource == 1) return (IPTR)0x10000000;
if (resource == 3) return (IPTR)0x1000000;
#else
if (resource == 0) return (IPTR)0x1000000;
if (resource == 1) return (IPTR)0x8000000;
if (resource == 2) return (IPTR)0x80000;
#endif
return (IPTR)0;
#endif
}

