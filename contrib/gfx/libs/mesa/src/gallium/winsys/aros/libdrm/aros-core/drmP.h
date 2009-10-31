/*-
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#if !defined(DRMP_H)
#define DRMP_H

#include "drm.h"
#include "drm_linux_list.h"

#include "drm_redefines.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>


#define DEBUG 0
#include <aros/debug.h>

#include <hidd/irq.h>

#define max(a,b) (a > b) ? a : b

struct drm_device;
struct drm_file;

/* Enable hacks for running under hosted AROS */
/* FIXME: THIS AND ALL "HOSTED_BUILD" MARKED CODE MUST BE DELETED IN FINAL VERSION */
//#define HOSTED_BUILD

#if defined(HOSTED_BUILD)
/* FIXME: These defines simulate certain gfx cards. Need to be removed in final version. Works only with HOSTED_BUILD */
//#define HOSTED_BUILD_ARCH       0x04    /* NV04 family */
//#define HOSTED_BUILD_CHIPSET    5       /* NV05 chip Riva TNT 2 */
//#define HOSTED_BUILD_ARCH       0x10    /* NV10 family */
//#define HOSTED_BUILD_CHIPSET    16      /* NV10 chip GeForce 256 */
//#define HOSTED_BUILD_ARCH       0x10    /* NV10 family */
//#define HOSTED_BUILD_CHIPSET    21      /* NV15 chip GeForce 2 GTS */
//#define HOSTED_BUILD_ARCH       0x20    /* NV20 family */
//#define HOSTED_BUILD_CHIPSET    32      /* NV20 chip GeForce 3 Ti 200 */
//#define HOSTED_BUILD_ARCH       0x20    /* NV20 family */
//#define HOSTED_BUILD_CHIPSET    37      /* NV25 chip GeForce Ti 4200 */
#define HOSTED_BUILD_ARCH       0x30    /* NV30 family */
#define HOSTED_BUILD_CHIPSET    52      /* NV34 chip GeForce FX 5200 */
//#define HOSTED_BUILD_ARCH       0x40    /* NV40 family */
//#define HOSTED_BUILD_CHIPSET    67      /* NV43 chip GeForce 6600 */
//#define HOSTED_BUILD_ARCH       0x80    /* NV50 family */
//#define HOSTED_BUILD_CHIPSET    132     /* G84 chip GeForce 8600 GT */
#endif

/* FIXME: Need to find a way to remove the need for these defines */
#define PAGE_SHIFT  12
#define PAGE_SIZE   ((1UL) << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))
#define EXPORT_SYMBOL(a)
#define DRM_CURRENTPID 1

#define DRM_IRQ_ARGS        void *arg
typedef void                irqreturn_t;
#define IRQ_HANDLED         /* nothing */
#define IRQ_NONE            /* nothing */

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define DRM_ARRAY_SIZE(x) ARRAY_SIZE(x)

struct Library          *OOPBase;

    
#define DRM_MEM_DRIVER     2
#define DRM_MEM_MAPS       5
#define DRM_MEM_BUFS       7
#define DRM_MEM_PAGES      9
#define DRM_MEM_SGLISTS   20
/* FIXME: Implement missing */


/* IOCTL table emulation */
/**
 * Ioctl function type.
 *
 * \param dev DRM device structure
 * \param data pointer to kernel-space stored data, copied in and out according
 *         to ioctl description.
 * \param file_priv DRM file private pointer.
 */
typedef int drm_ioctl_t(struct drm_device *dev, void *data,
            struct drm_file *file_priv);

#define DRM_AUTH        0x1
#define DRM_MASTER      0x2
#define DRM_ROOT_ONLY   0x4

struct drm_ioctl_desc {
    unsigned int cmd;
    drm_ioctl_t *func;
    int flags;
};
/**
 * Creates a driver or general drm_ioctl_desc array entry for the given
 * ioctl, for use by drm_ioctl().
 */
#define DRM_IOCTL_NR(n)     ((n) & 0xff)
#define DRM_IOCTL_DEF(ioctl, func, flags) \
    [DRM_IOCTL_NR(ioctl)] = {ioctl, func, flags}
/* */


#define writel(val, addr)       (*(volatile ULONG*)(addr) = (val))
#define readl(addr)             (*(volatile ULONG*)(addr))

/* FIXME: Implement missing */
/** Read a byte from a MMIO region */
//#define DRM_READ8(map, offset)      readb((map)->handle + (offset))
/** Read a word from a MMIO region */
//#define DRM_READ16(map, offset)     readw((map)->handle + (offset))
/** Read a dword from a MMIO region */
#define DRM_READ32(map, offset)         readl((map)->handle + (offset))
/** Write a byte into a MMIO region */
//#define DRM_WRITE8(map, offset, val)    writeb(val, (map)->handle + (offset))
/** Write a word into a MMIO region */
//#define DRM_WRITE16(map, offset, val)   writew(val, (map)->handle + (offset))
/** Write a dword into a MMIO region */
#define DRM_WRITE32(map, offset, val)   writel(val, (map)->handle + (offset))



#define DRM_ERROR(fmt, ...) bug("[" DRM_NAME "(ERROR):%s] " fmt, __func__ , ##__VA_ARGS__)
//#define DRM_INFO(fmt, ...) bug("[" DRM_NAME "(INFO)] " fmt, ##__VA_ARGS__)
#define DRM_INFO(fmt, ...)
#define DRM_DEBUG(fmt, ...) D(bug("[" DRM_NAME "(DEBUG):%s] " fmt, __func__ , ##__VA_ARGS__))
//#define printk(fmt, ...) bug(fmt, ##__VA_ARGS__)
#define printk(fmt, ...)

#define DRM_IMPL(fmt, ...) bug("------IMPLEMENT(%s): " fmt, __func__ , ##__VA_ARGS__)

/* DRM_READMEMORYBARRIER() prevents reordering of reads.
 * DRM_WRITEMEMORYBARRIER() prevents reordering of writes.
 * DRM_MEMORYBARRIER() prevents reordering of reads and writes.
 */
/* FIXME: Implementation for other architextures */
#define DRM_READMEMORYBARRIER()     __asm __volatile( \
                    "lock; addl $0,0(%%esp)" : : : "memory");
#define DRM_WRITEMEMORYBARRIER()    __asm __volatile("" : : : "memory");
#define DRM_MEMORYBARRIER()     __asm __volatile( \
                    "lock; addl $0,0(%%esp)" : : : "memory");


typedef unsigned long dma_addr_t;

struct drm_sg_mem {
    unsigned long handle;
    void *virtual;
    int pages;
#if !defined(__AROS__)    
    struct page **pagelist;
#else
    void * buffer;
#endif    
    dma_addr_t *busaddr;
};

/* Contains a collection of functions common to each drm driver */
struct drm_driver
{
    int         (*load)(struct drm_device *, unsigned long);
    int         (*firstopen)(struct drm_device *);
    void        (*preclose)(struct drm_device *dev, struct drm_file *);
    void        (*lastclose)(struct drm_device *);
    int         (*unload)(struct drm_device *);
    
    /* IRQ */
    irqreturn_t (*irq_handler)(DRM_IRQ_ARGS);
    void        (*irq_preinstall)(struct drm_device *);
    int         (*irq_postinstall)(struct drm_device *);
    void        (*irq_uninstall)(struct drm_device *);

    int                     version_patchlevel;
    struct drm_ioctl_desc   *ioctls;
};

struct drm_device {
    struct list_head maplist;       /* Linked list of regions */
    
    int irq_enabled;                /* True if irq handler is enabled */
    int pci_vendor;                 /* PCI vendor id */
    int pci_device;                 /* PCI device id */
    struct drm_sg_mem *sg;          /* Scatter gather memory */
    void *dev_private;              /* Device private data */
    
    struct drm_driver *driver;      /* Driver functions */

    /* AROS specific fields */
    OOP_Object              *pci;
    OOP_Object              *pciDevice;
    OOP_Object              *pcidriver;
    HIDDT_IRQ_Handler       *IntHandler;
};

struct drm_file
{
    /* Don't remove it */
    int dummy;
};

struct file;

/*
 * Generic memory manager structs
 */

struct drm_mm_node {
    struct list_head fl_entry;
    struct list_head ml_entry;
    int free;
    unsigned long start;
    unsigned long size;
    struct drm_mm *mm;
    void *private;
};

struct drm_mm {
    struct list_head fl_entry;
    struct list_head ml_entry;
};


/**
 * Mappings list
 */
struct drm_map_list {
    struct list_head head;      /**< list head */
/* FIXME: commented out fields*/    
//    struct drm_hash_item hash;
    struct drm_map *map;            /**< mapping */
    uint64_t user_token;
    struct drm_mm_node *file_offset_node;
};

typedef struct drm_map drm_local_map_t;

/* MTRR */
static inline int drm_mtrr_add(unsigned long offset, unsigned long size,
                   unsigned int flags)
{
    return -ENODEV;
}

static inline int drm_mtrr_del(int handle, unsigned long offset,
                   unsigned long size, unsigned int flags)
{
    return -ENODEV;
}

#define drm_core_has_MTRR(dev) (0)
#define DRM_MTRR_WC     0
static __inline__ int mtrr_add(unsigned long base, unsigned long size,
                   unsigned int type, char increment)
{
    return -ENODEV;
}

static __inline__ int mtrr_del(int reg, unsigned long base, unsigned long size)
{
    return -ENODEV;
}

#define MTRR_TYPE_WRCOMB     1
/*  */

/* drm_irq.c */
int drm_irq_install(struct drm_device *dev);
int drm_irq_uninstall(struct drm_device *dev);

/* drm_bufs.c */
int drm_order(unsigned long size);
unsigned long drm_get_resource_len(struct drm_device *dev,
                      unsigned int resource);
unsigned long drm_get_resource_start(struct drm_device *dev,
                        unsigned int resource);
int drm_addmap(struct drm_device *dev, unsigned int offset,
              unsigned int size, enum drm_map_type type,
              enum drm_map_flags flags, drm_local_map_t ** map_ptr);
int drm_rmmap(struct drm_device *dev, drm_local_map_t *map);              
struct drm_map_list *drm_find_matching_map(struct drm_device *dev,
                          drm_local_map_t *map);              

/* drm_scatter.c */
int drm_sg_alloc(struct drm_device *dev, struct drm_scatter_gather * request);
void drm_sg_cleanup(struct drm_sg_mem * entry);

/* drm_memory.c */
void drm_core_ioremap(struct drm_map *map, struct drm_device *dev);
void drm_core_ioremapfree(struct drm_map *map, struct drm_device *dev);
void *drm_calloc(size_t nmemb, size_t size, int area);
/* FIXME: make them inline? */
void *drm_alloc(size_t size, int area);
void drm_free(void *pt, size_t size, int area);

static __inline__ int drm_device_is_agp(struct drm_device *dev)
{
    /* FIXME: Implement */
    DRM_IMPL("\n");
    return 0;
}

static __inline__ int drm_device_is_pcie(struct drm_device *dev)
{
    /* FIXME: Implement */
    DRM_IMPL("\n");
    return 0;
}

/* drm_drv.c */
int drm_lastclose(struct drm_device *dev);
void drm_exit(struct drm_driver *driver);
int drm_init(struct drm_driver *driver);

/* AROS specific functions */
/* drm_aros.c */
LONG        drm_aros_find_supported_video_card(struct drm_device *dev);
void        drm_aros_pci_shutdown(struct drm_device *dev);
APTR        drm_aros_pci_ioremap(OOP_Object *driver, APTR buf, IPTR size);
void        drm_aros_pci_iounmap(OOP_Object *driver, APTR buf, IPTR size);
APTR        drm_aros_pci_resource_start(OOP_Object *pciDevice,  unsigned int resource);
IPTR        drm_aros_pci_resource_len(OOP_Object *pciDevice,  unsigned int resource);
dma_addr_t  drm_aros_dma_map_buf(APTR buf, IPTR offset, IPTR size);
void        drm_aros_dma_unmap_buf(dma_addr_t dma_address, IPTR size);
#endif
