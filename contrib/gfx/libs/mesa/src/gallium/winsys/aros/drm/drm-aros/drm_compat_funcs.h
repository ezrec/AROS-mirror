/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef _DRM_COMPAT_FUNCS_
#define _DRM_COMPAT_FUNCS_

#include <proto/exec.h>
#include <aros/debug.h>

#include "drm_compat_types.h"

#define writel(val, addr)               (*(volatile ULONG*)(addr) = (val))
#define readl(addr)                     (*(volatile ULONG*)(addr))
#define writew(val, addr)               (*(volatile UWORD*)(addr) = (val))
#define readw(addr)                     (*(volatile UWORD*)(addr))
#define writeb(val, addr)               (*(volatile BYTE*)(addr) = (val))
#define readb(addr)                     (*(volatile BYTE*)(addr))
#define kzalloc(size, flags)            AllocVec(size, MEMF_ANY | MEMF_CLEAR)
#define kmalloc(size, flags)            AllocVec(size, MEMF_ANY)
#define vmalloc_user(size)              AllocVec(size, MEMF_ANY)
#define kfree(objp)                     FreeVec(objp)
#define roundup(x, y)                   ((((x) + ((y) - 1)) / (y)) * (y))
#define lower_32_bits(n)                ((u32)(n))
#define upper_32_bits(n)                ((u32)(((n) >> 16) >> 16))
#define memcpy_toio(dest, src, count)   memcpy(dest, src, count)
#define mutex_lock(x)                   ObtainSemaphore(x.semaphore)
#define mutex_unlock(x)                 ReleaseSemaphore(x.semaphore)
#define mutex_init(x)                   InitSemaphore(x.semaphore);
#define likely(x)                       __builtin_expect((x),1)
#define unlikely(x)                     __builtin_expect((x),0)
/* TODO: Implement spinlocks in busy-cpu way? */
#define spin_lock_init(x)               InitSemaphore(x)
#define spin_lock(x)                    ObtainSemaphore(x)
#define spin_unlock(x)                  ReleaseSemaphore(x)
#define spin_lock_irq(x)                ObtainSemaphore(x)
#define spin_unlock_irq(x)              ReleaseSemaphore(x)
#define spin_lock_irqsave(x,y)          ObtainSemaphore(x)
#define spin_unlock_irqrestore(x,y)     ReleaseSemaphore(x)
#define pci_map_page(a, b, c, d, e)     drm_aros_dma_map_buf(b->address, c, d)
#define pci_dma_mapping_error(a, b)     FALSE
#define pci_unmap_page(a, b, c, d)      drm_aros_dma_unmap_buf(b, c)
#define ioremap_wc                      ioremap_nocache
#define idr_pre_get(a, b)               idr_pre_get_internal(a)




void  *ioremap_nocache(resource_size_t offset, unsigned long size);

void iowrite32(u32 val, void * addr);
unsigned int ioread32(void * addr);
void iowrite16(u16 val, void * addr);
unsigned int ioread16(void * addr);
void iowrite8(u8 val, void * addr);
unsigned int ioread8(void * addr);

static inline void udelay(unsigned long usecs)
{
    /*FIXME: THIS IS PROBABLY A VERY BAD IDEA */
    /*FIXME: IT WILL GET OPTIMIZED OUT ANYWAY */
}

static inline ULONG copy_from_user(APTR to, APTR from, IPTR size)
{
    memcpy(to, from, size);
    return 0;
}

static inline ULONG copy_to_user(APTR to, APTR from, IPTR size)
{
    memcpy(to, from, size);
    return 0;
}

#define BUG_ON(condition)           do { if (unlikely(condition)) bug("%s:%d\n", __FILE__, __LINE__); } while(0)
#define EXPORT_SYMBOL(x)
#define PTR_ERR(addr)               (SIPTR)addr
#define ERR_PTR(error)              (APTR)error
static inline IPTR IS_ERR(APTR ptr)
{
    return (IPTR)(ptr) >= (IPTR)-MAX_ERRNO;
}

/* Reference counted objects implementation */
void kref_init(struct kref *kref);
void kref_get(struct kref *kref);
int kref_put(struct kref *kref, void (*release) (struct kref *kref));

/* Kernel debug */
#define KERN_ERR
#define printk(fmt, ...)            bug(fmt, ##__VA_ARGS__)
#define IMPLEMENT(fmt, ...)         bug("------IMPLEMENT(%s): " fmt, __func__ , ##__VA_ARGS__)
#define BUG(x)                      bug("BUG:(%s)\n", __func__)

/* Bit operations */
void clear_bit(int nr, volatile void * addr);
void set_bit(int nr, volatile void *addr);

/* Page handling */
void __free_page(struct page * p);
struct page * my_create_page(); /* Helper function - not from compat */
#define PageHighMem(p)              FALSE
#define put_page(p)                 __free_page(p)
#define page_to_pfn(p)              p->address /*FIXME: This might be wrong */
#define kmap(p)                     p->address
#define vmap(p, count, flags, prot) (p)[0]->address
#define kunmap(addr)
#define vunmap(addr)

/* Atomic handling */
/* FIXME NOT REALLY ATOMIC */
static inline int atomic_add_return(int i, atomic_t *v)
{
    (*v) += i;
    return (*v);
}

static inline void atomic_inc(atomic_t *v)
{
    (*v)++;
}

static inline void atomic_set(atomic_t *v, int i)
{
    (*v) = i;
}

static inline int atomic_read(atomic_t *v)
{
    return (*v);
}

static inline int atomic_dec_and_test(atomic_t *v)
{
    (*v)--;
    return (*v) == 0;
}


/* IDR handling */
int idr_pre_get_internal(struct idr *idp);
int idr_get_new_above(struct idr *idp, void *ptr, int starting_id, int *id);
void *idr_find(struct idr *idp, int id);


#endif /* _DRM_COMPAT_FUNCS_ */
