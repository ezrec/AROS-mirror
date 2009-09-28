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

#define DEBUG 1
#include <aros/debug.h>
#include <oop/oop.h>

/* Enable hacks for running under hosted AROS */
/* FIXME: THIS AND ALL "HOSTED_BUILD" MARKED CODE MUST BE DELETED IN FINAL VERSION */
//#define HOSTED_BUILD 


#define DRM_CURRENTPID 1
#define DRM_IRQ_ARGS void *args
#define DRM_MEM_DRIVER     2
#define DRM_MEM_BUFS       7

/* FIXME: What should this be? */
#define PAGE_SIZE 4096

#if defined(HOSTED_BUILD)
#define readl(addr) 0
#define writel(val, addr)
#else
#define writel(val, addr)  (*(volatile ULONG*)(addr) = (val))
#define readl(addr)    (*(volatile ULONG*)(addr))
#endif

/** Read a byte from a MMIO region */
//#define DRM_READ8(map, offset)      readb((map)->handle + (offset))
/** Read a word from a MMIO region */
//#define DRM_READ16(map, offset)     readw((map)->handle + (offset))
/** Read a dword from a MMIO region */
#define DRM_READ32(map, offset)     readl((map)->handle + (offset))
/** Write a byte into a MMIO region */
//#define DRM_WRITE8(map, offset, val)    writeb(val, (map)->handle + (offset))
/** Write a word into a MMIO region */
//#define DRM_WRITE16(map, offset, val)   writew(val, (map)->handle + (offset))
/** Write a dword into a MMIO region */
#define DRM_WRITE32(map, offset, val)   writel(val, (map)->handle + (offset))



#define DRM_ERROR(fmt, ...) bug("[" DRM_NAME "(ERROR):%s] " fmt, __func__ , ##__VA_ARGS__)
#define DRM_INFO(fmt, ...) bug("[" DRM_NAME "(INFO)] " fmt, ##__VA_ARGS__)
#define DRM_DEBUG(fmt, ...) D(bug("[" DRM_NAME "(DEBUG):%s] " fmt, __func__ , ##__VA_ARGS__))


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
//    struct page **pagelist;
    dma_addr_t *busaddr;
};

struct drm_device {
    int irq_enabled;        /**< True if irq handler is enabled */
    int pci_vendor;         /**< PCI vendor id */
    int pci_device;         /**< PCI device id */
    struct drm_sg_mem *sg;      /**< Scatter gather memory */
    void *dev_private;      /**< device private data */
    /* FIXME: other fields */
    /* AROS specific fields */
    OOP_Object      *pci;
    OOP_Object      *pciDevice;
    OOP_Object      *pcidriver;
};

struct drm_file;

struct file;

typedef struct drm_map drm_local_map_t;

typedef void            irqreturn_t;

int drm_irq_install(struct drm_device *dev);
unsigned long drm_get_resource_len(struct drm_device *dev,
                      unsigned int resource);
int drm_order(unsigned long size);
                          
unsigned long drm_get_resource_start(struct drm_device *dev,
                        unsigned int resource);
                        
void *drm_calloc(size_t nmemb, size_t size, int area);
/* FIXME: make them inline? */
void *drm_alloc(size_t size, int area);
void drm_free(void *pt, size_t size, int area);

/* AROS specific functions */
int drm_pci_find_supported_video_card(struct drm_device *dev);
APTR drm_pci_ioremap(OOP_Object *driver, APTR buf, IPTR size);
void drm_pci_iounmap(OOP_Object *driver, APTR buf, IPTR size);
APTR drm_pci_resource_start(OOP_Object *pciDevice,  unsigned int resource);
IPTR drm_pci_resource_len(OOP_Object *pciDevice,  unsigned int resource);
#endif