/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef _DRM_COMPAT_FUNCS_
#define _DRM_COMPAT_FUNCS_

#include <proto/exec.h>
#include <aros/debug.h>

#include "drm_compat_types.h"

#define writeq(val, addr)               (*(volatile UQUAD*)(addr) = (val))
#define readq(addr)                     (*(volatile UQUAD*)(addr)
#define writel(val, addr)               (*(volatile ULONG*)(addr) = (val))
#define readl(addr)                     (*(volatile ULONG*)(addr))
#define writew(val, addr)               (*(volatile UWORD*)(addr) = (val))
#define readw(addr)                     (*(volatile UWORD*)(addr))
#define writeb(val, addr)               (*(volatile UBYTE*)(addr) = (val))
#define readb(addr)                     (*(volatile UBYTE*)(addr))
#define kzalloc(size, flags)            AllocVec(size, MEMF_ANY | MEMF_CLEAR)
#define kcalloc(count, size, flags)     AllocVec(count * size, MEMF_ANY | MEMF_CLEAR);
#define kmalloc(size, flags)            AllocVec(size, MEMF_ANY)
#define vmalloc_user(size)              AllocVec(size, MEMF_ANY | MEMF_CLEAR)
#define vmalloc(size)                   AllocVec(size, MEMF_ANY)
#define kfree(objp)                     FreeVec(objp)
#define vfree(objp)                     FreeVec(objp)
#define capable(p)                      TRUE
#define roundup(x, y)                   ((((x) + ((y) - 1)) / (y)) * (y))
#define lower_32_bits(n)                ((u32)(n))
#define upper_32_bits(n)                ((u32)(((n) >> 16) >> 16))
#define memcpy_toio(dest, src, count)   memcpy(dest, src, count)
#define memcpy_fromio(dest, src, count) memcpy(dest, src, count)
#define mutex_lock(x)                   ObtainSemaphore(x.semaphore)
#define mutex_unlock(x)                 ReleaseSemaphore(x.semaphore)
#define mutex_init(x)                   InitSemaphore(x.semaphore);
#define likely(x)                       __builtin_expect((x),1)
#define unlikely(x)                     __builtin_expect((x),0)
#define mb()                            __asm __volatile("lock; addl $0,0(%%esp)" : : : "memory");
#define ffs(x)                          __builtin_ffs(x)



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

static inline void clflush(volatile void * ptr)
{
    asm volatile("clflush %0" : "+m" (*(volatile BYTE *) ptr));
}


#define BUG_ON(condition)           do { if (unlikely(condition)) bug("BUG: %s:%d\n", __FILE__, __LINE__); } while(0)
#define WARN_ON(condition)          do { if (unlikely(condition)) bug("WARN: %s:%d\n", __FILE__, __LINE__); } while(0)
#define EXPORT_SYMBOL(x)
#define PTR_ERR(addr)               (SIPTR)addr
#define ERR_PTR(error)              (APTR)error
static inline IPTR IS_ERR(APTR ptr)
{
    return (IPTR)(ptr) >= (IPTR)-MAX_ERRNO;
}

/* Kernel debug */
#define KERN_ERR
#define printk(fmt, ...)            bug(fmt, ##__VA_ARGS__)
#define IMPLEMENT(fmt, ...)         bug("------IMPLEMENT(%s): " fmt, __func__ , ##__VA_ARGS__)
#define TRACE(fmt, ...)             D(bug("[TRACE](%s): " fmt, __func__ , ##__VA_ARGS__))
#define BUG(x)                      bug("BUG:(%s)\n", __func__)
#define WARN(condition, message)    do { if (unlikely(condition)) bug("WARN: %s:%d %s\n", __FILE__, __LINE__, message); } while(0)

/* PCI handling */
void * ioremap(resource_size_t offset, unsigned long size);
#define pci_map_page(a, b, c, d, e)     (dma_addr_t)(b->address + c)
#define pci_dma_mapping_error(a, b)     FALSE
#define pci_unmap_page(a, b, c, d)      
#define ioremap_nocache                 ioremap
#define ioremap_wc                      ioremap
void iounmap(void * addr);
resource_size_t pci_resource_start(void * pdev, unsigned int barnum);
unsigned long pci_resource_len(void * pdev, unsigned int barnum);
#define PCI_DEVFN(dev, fun)             dev, fun
void * pci_get_bus_and_slot(unsigned int bus, unsigned int dev, unsigned int fun);
int pci_read_config_word(void *dev, int where, u16 *val);
int pci_read_config_dword(void *dev, int where, u32 *val);
int pci_write_config_dword(void *dev, int where, u32 val);



/* Bit operations */
void clear_bit(int nr, volatile void * addr);
void set_bit(int nr, volatile void *addr);
int test_bit(int nr, volatile void *addr);
#define __set_bit(nr, addr)         set_bit(nr, addr)
#define __clear_bit(nr, addr)       clear_bit(nr, addr)

/* Page handling */
void __free_page(struct page * p);
struct page * create_page_helper();                     /* Helper function - not from compat */
#define PageHighMem(p)              FALSE
#define put_page(p)                 __free_page(p)  /*FIXME: This might be wrong */
#define page_to_phys(p)             (dma_addr_t)p->address
#define kmap(p)                     p->address
#define kmap_atomic(p, type)        p->address
#define vmap(p, count, flags, prot) (p)[0]->address
#define kunmap_atomic(addr, type)
#define kunmap(addr)
#define vunmap(addr)
#define set_page_dirty(p)

/* Atomic handling */
static inline int atomic_add_return(int i, atomic_t *v)
{
    return __sync_add_and_fetch(&v->count, i);
}

static inline void atomic_add(int i, atomic_t *v)
{
    (void)__sync_add_and_fetch(&v->count, i);
}

static inline void atomic_inc(atomic_t *v)
{
    __sync_add_and_fetch(&v->count, 1);
}

static inline void atomic_set(atomic_t *v, int i)
{
    v->count = i;
}

static inline int atomic_read(atomic_t *v)
{
    return v->count;
}

static inline void atomic_sub(int i, atomic_t *v)
{
    (void)__sync_sub_and_fetch(&v->count, i);
}

static inline void atomic_dec(atomic_t *v)
{
    __sync_sub_and_fetch(&v->count, 1);
}

static inline int atomic_dec_and_test(atomic_t *v)
{
    return (__sync_sub_and_fetch(&v->count, 1) == 0);
}

static inline int atomic_cmpxchg(atomic_t *v, int old, int new)
{
    return __sync_val_compare_and_swap(&v->count, old, new);
}

/* Lock handling */
static inline void spin_lock_init(spinlock_t * lock)
{
    atomic_set(&lock->lock, 0);
}
static inline void spin_lock(spinlock_t * lock)
{
    while(atomic_cmpxchg(&lock->lock, 0, 1) != 0);
}
static inline void spin_unlock(spinlock_t * lock)
{
    atomic_set(&lock->lock, 0);
}
static inline void spin_lock_irq(spinlock_t * lock)
{
    Disable();
    
    while(atomic_cmpxchg(&lock->lock, 0, 1) != 0);
}
static inline void spin_unlock_irq(spinlock_t * lock)
{
    atomic_set(&lock->lock, 0);

    Enable();
}
#define spin_lock_irqsave(x,y)          spin_lock_irq(x)
#define spin_unlock_irqrestore(x,y)     spin_unlock_irq(x)
/* TODO: This may work incorreclty if write_lock and read_lock are used for the same lock
   read_lock allows concurent readers as lock as there is no writer */
static inline void rwlock_init(rwlock_t * lock)
{
    atomic_set(&lock->lock, 0);
}
static inline void write_lock(rwlock_t * lock)
{
    while(atomic_cmpxchg(&lock->lock, 0, 1) != 0);
}
static inline void write_unlock(rwlock_t * lock)
{
    atomic_set(&lock->lock, 0);
}

/* Reference counted objects implementation */
static inline void kref_init(struct kref *kref)
{
    atomic_set(&kref->refcount, 1);
}

static inline void kref_get(struct kref *kref)
{
    atomic_inc(&kref->refcount);
}

static inline int kref_put(struct kref *kref, void (*release) (struct kref *kref))
{
    if (atomic_dec_and_test(&kref->refcount)) release(kref);
    return atomic_read(&kref->refcount);
}

/* IDR handling */
#define idr_pre_get(a, b)               idr_pre_get_internal(a)
int idr_pre_get_internal(struct idr *idp);
int idr_get_new_above(struct idr *idp, void *ptr, int starting_id, int *id);
void *idr_find(struct idr *idp, int id);
void idr_remove(struct idr *idp, int id);
void idr_init(struct idr *idp);

#endif /* _DRM_COMPAT_FUNCS_ */
