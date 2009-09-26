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

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define DRM_CURRENTPID 1
#define DRM_IRQ_ARGS void *args
#define DRM_MEM_DRIVER     2
#define DRM_MEM_BUFS       7

extern int drm_debug_flag;

/* FIXME: real code needed*/
#define DRM_READ32(map, offset)   0
#define DRM_WRITE32(map, offset, val)  

#define DRM_ERROR(fmt, ...) \
    printf("error: [" DRM_NAME ":pid%d:%s] *ERROR* " fmt,       \
        DRM_CURRENTPID, __func__ , ##__VA_ARGS__)

#define DRM_INFO(fmt, ...)  printf("info: [" DRM_NAME "] " fmt , ##__VA_ARGS__)

#define DRM_DEBUG(fmt, ...) do {                    \
    if (drm_debug_flag)                     \
        printf("[" DRM_NAME ":pid%d:%s] " fmt, DRM_CURRENTPID,  \
            __func__ , ##__VA_ARGS__);          \
} while (0)

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
};

struct drm_file;

struct file;

typedef struct drm_map drm_local_map_t;

typedef void            irqreturn_t;

int drm_irq_install(struct drm_device *dev);
unsigned long drm_get_resource_len(struct drm_device *dev,
                      unsigned int resource);
int drm_order(unsigned long size);
                          
void *drm_calloc(size_t nmemb, size_t size, int area);
/* FIXME: make them inline */
void *drm_alloc(size_t size, int area);
void drm_free(void *pt, size_t size, int area);

#endif