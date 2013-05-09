/*
    Copyright � 2010-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Common memory utility functions
    Lang: english
 */

#include <aros/debug.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/memheaderext.h>
#include <proto/exec.h>

#include <kernel_debug.h>
#include <kernel_base.h>
#include "kernel_globals.h"
#include "kernel_intern.h"


/*
 * Two Levels Segregate Fit memory allocator (TLSF)
 * Version 2.4.6
 *
 * Written by Miguel Masmano Tello <mimastel@doctor.upv.es>
 *
 * Thanks to Ismael Ripoll for his suggestions and reviews
 *
 * Copyright (C) 2008, 2007, 2006, 2005, 2004
 *
 * This code is released using a dual license strategy: GPL/LGPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of the GNU General Public License Version 2.0
 * Released under the terms of the GNU Lesser General Public License Version 2.1
 *
 */

/*
 * Code contributions:
 *
 * (May 9 2013) Michal Schulz <michal.schulz@gmx.de>
 *
 * - Add tlsf_allocabs function. It allows allocation in a fixed place in memory
 *
 * (Jul 28 2007)  Herman ten Brugge <hermantenbrugge@home.nl>:
 *
 * - Add 64 bit support. It now runs on x86_64 and solaris64.
 * - I also tested this on vxworks/32and solaris/32 and i386/32 processors.
 * - Remove assembly code. I could not measure any performance difference
 *   on my core2 processor. This also makes the code more portable.
 * - Moved defines/typedefs from tlsf.h to tlsf.c
 * - Changed MIN_BLOCK_SIZE to sizeof (free_ptr_t) and BHDR_OVERHEAD to
 *   (sizeof (bhdr_t) - MIN_BLOCK_SIZE). This does not change the fact
 *    that the minumum size is still sizeof
 *   (bhdr_t).
 * - Changed all C++ comment style to C style. (// -> /.* ... *./)
 * - Used ls_bit instead of ffs and ms_bit instead of fls. I did this to
 *   avoid confusion with the standard ffs function which returns
 *   different values.
 * - Created set_bit/clear_bit fuctions because they are not present
 *   on x86_64.
 * - Added locking support + extra file target.h to show how to use it.
 * - Added get_used_size function (REMOVED in 2.4)
 * - Added rtl_realloc and rtl_calloc function
 * - Implemented realloc clever support.
 * - Added some test code in the example directory.
 * - Bug fixed (discovered by the rockbox project: www.rockbox.org).
 *
 * (Oct 23 2006) Adam Scislowicz:
 *
 * - Support for ARMv5 implemented
 *
 */

#include <sys/types.h>

static size_t init_memory_pool(size_t, void *);
static size_t get_used_size(void *);
static size_t get_max_size(void *);
static void destroy_memory_pool(void *);
static size_t add_new_area(void *, size_t, void *);
static void *malloc_ex(size_t, void *);
static void free_ex(void *, void *);
static void *realloc_ex(void *, size_t, void *);
static void *calloc_ex(size_t, size_t, void *);

#ifndef USE_PRINTF
#define USE_PRINTF      (0)
#endif

#include <string.h>

#ifndef TLSF_USE_LOCKS
#define TLSF_USE_LOCKS  (0)
#endif

#ifndef TLSF_STATISTIC
#define TLSF_STATISTIC  (1)
#endif

#ifndef USE_MMAP
#define USE_MMAP        (1)
#endif

#ifndef USE_SBRK
#define USE_SBRK        (0)
#endif

#if TLSF_USE_LOCKS
#include "tlsf_target.h"
#else
#define TLSF_CREATE_LOCK(_unused_)   do{}while(0)
#define TLSF_DESTROY_LOCK(_unused_)  do{}while(0)
#define TLSF_ACQUIRE_LOCK(_unused_)  do{}while(0)
#define TLSF_RELEASE_LOCK(_unused_)  do{}while(0)
#endif

#if TLSF_STATISTIC
#define TLSF_ADD_SIZE(tlsf, b) do {                                                                     \
        tlsf->used_size += (b->size & BLOCK_SIZE) + BHDR_OVERHEAD;      \
        if (tlsf->used_size > tlsf->max_size)                                           \
        tlsf->max_size = tlsf->used_size;                                               \
} while(0)

#define TLSF_REMOVE_SIZE(tlsf, b) do {                                                          \
        tlsf->used_size -= (b->size & BLOCK_SIZE) + BHDR_OVERHEAD;      \
} while(0)

#define TLSF_ADD_TOTAL_SIZE(tlsf, size) do {                \
        tlsf->total_size += size;                                   \
} while(0)

#else
#define TLSF_ADD_SIZE(tlsf, b)       do{}while(0)
#define TLSF_REMOVE_SIZE(tlsf, b)    do{}while(0)
#define TLSF_ADD_TOTAL_SIZE(tlsf, s) do{}while(0)
#endif

#if USE_MMAP || USE_SBRK
#include <unistd.h>
#endif

#if USE_MMAP
#include <sys/mman.h>
#endif

#if !defined(__GNUC__)
#ifndef __inline__
#define __inline__
#endif
#endif

/* The  debug functions  only can  be used  when _DEBUG_TLSF_  is set. */
#ifndef _DEBUG_TLSF_
#define _DEBUG_TLSF_  (0)
#endif

/*************************************************************************/
/* Definition of the structures used by TLSF */


/* Some IMPORTANT TLSF parameters */
/* Unlike the preview TLSF versions, now they are statics */
#define BLOCK_ALIGN (sizeof(void *) * 4)

#define MAX_FLI         (30)
#define MAX_LOG2_SLI    (5)
#define MAX_SLI         (1 << MAX_LOG2_SLI)     /* MAX_SLI = 2^MAX_LOG2_SLI */

#define FLI_OFFSET      (6)     /* tlsf structure just will manage blocks bigger */
/* than 128 bytes */
#define SMALL_BLOCK     (128)
#define REAL_FLI        (MAX_FLI - FLI_OFFSET)
#define MIN_BLOCK_SIZE  (sizeof (free_ptr_t))
#define BHDR_OVERHEAD   (sizeof (bhdr_t) - MIN_BLOCK_SIZE)
#define TLSF_SIGNATURE  (0x2A59FA59)

#define PTR_MASK        (sizeof(void *) - 1)
#define BLOCK_SIZE      (0xFFFFFFFF - PTR_MASK)

#define GET_NEXT_BLOCK(_addr, _r) ((bhdr_t *) ((char *) (_addr) + (_r)))
#define MEM_ALIGN                 ((BLOCK_ALIGN) - 1)
#define ROUNDUP_SIZE(_r)          (((_r) + MEM_ALIGN) & ~MEM_ALIGN)
#define ROUNDDOWN_SIZE(_r)        ((_r) & ~MEM_ALIGN)
#define ROUNDUP(_x, _v)           ((((~(_x)) + 1) & ((_v)-1)) + (_x))

#define BLOCK_STATE     (0x1)
#define PREV_STATE      (0x2)

/* bit 0 of the block size */
#define FREE_BLOCK      (0x1)
#define USED_BLOCK      (0x0)

/* bit 1 of the block size */
#define PREV_FREE       (0x2)
#define PREV_USED       (0x0)


#define DEFAULT_AREA_SIZE (1024*10)

#ifdef USE_MMAP
#define PAGE_SIZE (getKernelBase()->kb_PageSize)
#endif

#if USE_PRINTF
#include <stdio.h>
# define PRINT_MSG(fmt, args...) printf(fmt, ## args)
# define ERROR_MSG(fmt, args...) printf(fmt, ## args)
#else
# if !defined(PRINT_MSG)
#  define PRINT_MSG(fmt, args...) nbug(fmt, ## args)
# endif
# if !defined(ERROR_MSG)
#  define ERROR_MSG(fmt, args...) nbug(fmt, ## args)
# endif
#endif

typedef unsigned int u32_t;     /* NOTE: Make sure that this type is 4 bytes long on your computer */
typedef unsigned char u8_t;     /* NOTE: Make sure that this type is 1 byte on your computer */

typedef struct free_ptr_struct {
    struct bhdr_struct *prev;
    struct bhdr_struct *next;
} free_ptr_t;

typedef struct bhdr_struct {
    /* This pointer is just valid if the first bit of size is set */
    struct bhdr_struct *prev_hdr;
    /* The size is stored in bytes */
    size_t size;                /* bit 0 indicates whether the block is used and */
    /* bit 1 allows to know whether the previous block is free */
    u32_t fill[2];
    union {
        struct free_ptr_struct free_ptr;
        u8_t buffer[1];         /*sizeof(struct free_ptr_struct)]; */
    } ptr;
} bhdr_t;

/* This structure is embedded at the beginning of each area, giving us
 * enough information to cope with a set of areas */

typedef struct area_info_struct {
    bhdr_t *end;
    struct area_info_struct *next;
} area_info_t;

typedef struct TLSF_struct {
    /* the TLSF's structure signature */
    u32_t tlsf_signature;

#if TLSF_USE_LOCKS
    TLSF_MLOCK_T lock;
#endif

#if TLSF_STATISTIC
    /* These can not be calculated outside tlsf because we
     * do not know the sizes when freeing/reallocing memory. */
    size_t used_size;
    size_t max_size;
    size_t total_size;
#endif

    /* A linked list holding all the existing areas */
    area_info_t *area_head;

    /* the first-level bitmap */
    /* This array should have a size of REAL_FLI bits */
    u32_t fl_bitmap;

    /* the second-level bitmap */
    u32_t sl_bitmap[REAL_FLI];

    bhdr_t *matrix[REAL_FLI][MAX_SLI];
} tlsf_t;


/******************************************************************/
/**************     Helping functions    **************************/
/******************************************************************/
static __inline__ void set_bit(int nr, u32_t * addr);
static __inline__ void clear_bit(int nr, u32_t * addr);
static __inline__ int ls_bit(int x);
static __inline__ int ms_bit(int x);
static __inline__ void MAPPING_SEARCH(size_t * _r, int *_fl, int *_sl);
static __inline__ void MAPPING_INSERT(size_t _r, int *_fl, int *_sl);
static __inline__ bhdr_t *FIND_SUITABLE_BLOCK(tlsf_t * _tlsf, int *_fl, int *_sl);
static __inline__ bhdr_t *process_area(void *area, size_t size);
#if USE_SBRK || USE_MMAP
static __inline__ void *get_new_area(size_t * size);
#endif

static const int table[] = {
        -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4,
        4, 4,
        4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5,
        5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6,
        6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6,
        6, 6, 6, 6, 6, 6, 6,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7,
        7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7,
        7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7,
        7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7,
        7, 7, 7, 7, 7, 7, 7
};

static __inline__ int ls_bit(int i)
{
    unsigned int a;
    unsigned int x = i & -i;

    a = x <= 0xffff ? (x <= 0xff ? 0 : 8) : (x <= 0xffffff ? 16 : 24);
    return table[x >> a] + a;
}

static __inline__ int ms_bit(int i)
{
    unsigned int a;
    unsigned int x = (unsigned int) i;

    a = x <= 0xffff ? (x <= 0xff ? 0 : 8) : (x <= 0xffffff ? 16 : 24);
    return table[x >> a] + a;
}

static __inline__ void set_bit(int nr, u32_t * addr)
{
    addr[nr >> 5] |= 1 << (nr & 0x1f);
}

static __inline__ void clear_bit(int nr, u32_t * addr)
{
    addr[nr >> 5] &= ~(1 << (nr & 0x1f));
}

static __inline__ void MAPPING_SEARCH(size_t * _r, int *_fl, int *_sl)
{
    int _t;

    if (*_r < SMALL_BLOCK) {
        *_fl = 0;
        *_sl = *_r / (SMALL_BLOCK / MAX_SLI);
    } else {
        _t = (1 << (ms_bit(*_r) - MAX_LOG2_SLI)) - 1;
        *_r = *_r + _t;
        *_fl = ms_bit(*_r);
        *_sl = (*_r >> (*_fl - MAX_LOG2_SLI)) - MAX_SLI;
        *_fl -= FLI_OFFSET;
        /*if ((*_fl -= FLI_OFFSET) < 0) // FL wil be always >0!
         *_fl = *_sl = 0;
         */
        *_r &= ~_t;
    }
}

static __inline__ void MAPPING_INSERT(size_t _r, int *_fl, int *_sl)
{
    if (_r < SMALL_BLOCK) {
        *_fl = 0;
        *_sl = _r / (SMALL_BLOCK / MAX_SLI);
    } else {
        *_fl = ms_bit(_r);
        *_sl = (_r >> (*_fl - MAX_LOG2_SLI)) - MAX_SLI;
        *_fl -= FLI_OFFSET;
    }
}

static __inline__ bhdr_t *FIND_SUITABLE_BLOCK(tlsf_t * _tlsf, int *_fl, int *_sl)
{
    u32_t _tmp = _tlsf->sl_bitmap[*_fl] & (~0 << *_sl);
    bhdr_t *_b = NULL;

    if (_tmp) {
        *_sl = ls_bit(_tmp);
        _b = _tlsf->matrix[*_fl][*_sl];
    } else {
        *_fl = ls_bit(_tlsf->fl_bitmap & (~0 << (*_fl + 1)));
        if (*_fl > 0) {         /* likely */
            *_sl = ls_bit(_tlsf->sl_bitmap[*_fl]);
            _b = _tlsf->matrix[*_fl][*_sl];
        }
    }
    return _b;
}

#define EXTRACT_BLOCK_HDR(_b, _tlsf, _fl, _sl) do {                                     \
        _tlsf -> matrix [_fl] [_sl] = _b -> ptr.free_ptr.next;          \
        if (_tlsf -> matrix[_fl][_sl])                                                          \
        _tlsf -> matrix[_fl][_sl] -> ptr.free_ptr.prev = NULL;  \
        else {                                                                                                          \
            clear_bit (_sl, &_tlsf -> sl_bitmap [_fl]);                             \
            if (!_tlsf -> sl_bitmap [_fl])                                                  \
            clear_bit (_fl, &_tlsf -> fl_bitmap);                           \
        }                                                                                                                       \
        _b -> ptr.free_ptr.prev =  NULL;                                \
        _b -> ptr.free_ptr.next =  NULL;                                \
}while(0)

#define EXTRACT_BLOCK(_b, _tlsf, _fl, _sl) do {                                                 \
        if (_b -> ptr.free_ptr.next)                                                                    \
        _b -> ptr.free_ptr.next -> ptr.free_ptr.prev = _b -> ptr.free_ptr.prev; \
        if (_b -> ptr.free_ptr.prev)                                                                    \
        _b -> ptr.free_ptr.prev -> ptr.free_ptr.next = _b -> ptr.free_ptr.next; \
        if (_tlsf -> matrix [_fl][_sl] == _b) {                                                 \
            _tlsf -> matrix [_fl][_sl] = _b -> ptr.free_ptr.next;           \
            if (!_tlsf -> matrix [_fl][_sl]) {                                                      \
                clear_bit (_sl, &_tlsf -> sl_bitmap[_fl]);                              \
                if (!_tlsf -> sl_bitmap [_fl])                                                  \
                clear_bit (_fl, &_tlsf -> fl_bitmap);                           \
            }                                                                                                                       \
        }                                                                                                                               \
        _b -> ptr.free_ptr.prev = NULL;                                 \
        _b -> ptr.free_ptr.next = NULL;                                 \
} while(0)

#define INSERT_BLOCK(_b, _tlsf, _fl, _sl) do {                                                  \
        _b -> ptr.free_ptr.prev = NULL; \
        _b -> ptr.free_ptr.next = _tlsf -> matrix [_fl][_sl]; \
        if (_tlsf -> matrix [_fl][_sl])                                                                 \
        _tlsf -> matrix [_fl][_sl] -> ptr.free_ptr.prev = _b;           \
        _tlsf -> matrix [_fl][_sl] = _b;                                                                \
        set_bit (_sl, &_tlsf -> sl_bitmap [_fl]);                                               \
        set_bit (_fl, &_tlsf -> fl_bitmap);                                                             \
} while(0)

#if USE_SBRK || USE_MMAP
static __inline__ void *get_new_area(size_t * size)
{
    void *area;

#if USE_SBRK
    area = (void *)sbrk(0);
    if (((void *)sbrk(*size)) != ((void *) -1))
        return area;
#endif

#ifndef MAP_ANONYMOUS
    /* https://dev.openwrt.org/ticket/322 */
# define MAP_ANONYMOUS MAP_ANON
#endif

#if USE_MMAP
    *size = ROUNDUP(*size, PAGE_SIZE);
    if ((area = getKernelBase()->kb_PlatformData->iface->mmap(0, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) != MAP_FAILED)
        return area;
#endif
    return ((void *) ~0);
}
#endif

static __inline__ bhdr_t *process_area(void *area, size_t size)
{
    bhdr_t *b, *lb, *ib;
    area_info_t *ai;

    ib = (bhdr_t *) area;
    ib->size =
            (sizeof(area_info_t) <
                    MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : ROUNDUP_SIZE(sizeof(area_info_t)) | USED_BLOCK | PREV_USED;
    b = (bhdr_t *) GET_NEXT_BLOCK(ib->ptr.buffer, ib->size & BLOCK_SIZE);
    b->size = ROUNDDOWN_SIZE(size - 3 * BHDR_OVERHEAD - (ib->size & BLOCK_SIZE)) | USED_BLOCK | PREV_USED;
    b->ptr.free_ptr.prev = b->ptr.free_ptr.next = 0;
    lb = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    lb->prev_hdr = b;
    lb->size = 0 | USED_BLOCK | PREV_FREE;
    ai = (area_info_t *) ib->ptr.buffer;
    ai->next = 0;
    ai->end = lb;
    return ib;
}

/******************************************************************/
/******************** Begin of the allocator code *****************/
/******************************************************************/

/******************************************************************/
static size_t init_memory_pool(size_t mem_pool_size, void *mem_pool)
{
    /******************************************************************/
    tlsf_t *tlsf;
    bhdr_t *b, *ib;

    if (!mem_pool || !mem_pool_size || mem_pool_size < sizeof(tlsf_t) + BHDR_OVERHEAD * 8) {
        ERROR_MSG("init_memory_pool (): memory_pool invalid\n");
        return -1;
    }

    if (((unsigned long) mem_pool & PTR_MASK)) {
        ERROR_MSG("init_memory_pool (): mem_pool must be aligned to a word\n");
        return -1;
    }
    tlsf = (tlsf_t *) mem_pool;
    /* Check if already initialised */
    if (tlsf->tlsf_signature == TLSF_SIGNATURE) {
        b = GET_NEXT_BLOCK((u8_t *)mem_pool, ROUNDUP_SIZE(sizeof(tlsf_t)));
        return b->size & BLOCK_SIZE;
    }

    /* Zeroing the memory pool */
    memset(mem_pool, 0, sizeof(tlsf_t));

    tlsf->tlsf_signature = TLSF_SIGNATURE;

    TLSF_CREATE_LOCK(&tlsf->lock);

    ib = process_area(GET_NEXT_BLOCK
            (mem_pool, ROUNDUP_SIZE(sizeof(tlsf_t))), ROUNDDOWN_SIZE(mem_pool_size - sizeof(tlsf_t)));
    b = GET_NEXT_BLOCK(ib->ptr.buffer, ib->size & BLOCK_SIZE);
    free_ex(b->ptr.buffer, tlsf);
    tlsf->area_head = (area_info_t *) ib->ptr.buffer;

#if TLSF_STATISTIC
    tlsf->used_size = mem_pool_size - (b->size & BLOCK_SIZE);
    tlsf->max_size = tlsf->used_size;
    tlsf->total_size = mem_pool_size;
#endif

    return (b->size & BLOCK_SIZE);
}

/******************************************************************/
static size_t add_new_area(void *area, size_t area_size, void *mem_pool)
{
    /******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;
    area_info_t *ptr, *ptr_prev, *ai;
    bhdr_t *ib0, *b0, *lb0, *ib1, *b1, *lb1, *next_b;

    memset(area, 0, area_size);
    ptr = tlsf->area_head;
    ptr_prev = 0;

    ib0 = process_area(area, area_size);
    b0 = GET_NEXT_BLOCK(ib0->ptr.buffer, ib0->size & BLOCK_SIZE);
    lb0 = GET_NEXT_BLOCK(b0->ptr.buffer, b0->size & BLOCK_SIZE);

    /* Before inserting the new area, we have to merge this area with the
     already existing ones */

    while (ptr) {
        ib1 = (bhdr_t *) ((char *) ptr - BHDR_OVERHEAD);
        b1 = GET_NEXT_BLOCK(ib1->ptr.buffer, ib1->size & BLOCK_SIZE);
        lb1 = ptr->end;

        /* Merging the new area with the next physically contigous one */
        if ((unsigned long) ib1 == (unsigned long) lb0 + BHDR_OVERHEAD) {
            if (tlsf->area_head == ptr) {
                tlsf->area_head = ptr->next;
                ptr = ptr->next;
            } else {
                ptr_prev->next = ptr->next;
                ptr = ptr->next;
            }
            b0->size =
                    ROUNDDOWN_SIZE((b0->size & BLOCK_SIZE) +
                            (ib1->size & BLOCK_SIZE) + 2 * BHDR_OVERHEAD) | USED_BLOCK | PREV_USED;

            b1->prev_hdr = b0;
            lb0 = lb1;

            continue;
        }

        /* Merging the new area with the previous physically contigous
         one */
        if ((unsigned long) lb1->ptr.buffer == (unsigned long) ib0) {
            if (tlsf->area_head == ptr) {
                tlsf->area_head = ptr->next;
                ptr = ptr->next;
            } else {
                ptr_prev->next = ptr->next;
                ptr = ptr->next;
            }

            lb1->size =
                    ROUNDDOWN_SIZE((b0->size & BLOCK_SIZE) +
                            (ib0->size & BLOCK_SIZE) + 2 * BHDR_OVERHEAD) | USED_BLOCK | (lb1->size & PREV_STATE);
            next_b = GET_NEXT_BLOCK(lb1->ptr.buffer, lb1->size & BLOCK_SIZE);
            next_b->prev_hdr = lb1;
            b0 = lb1;
            ib0 = ib1;

            continue;
        }
        ptr_prev = ptr;
        ptr = ptr->next;
    }
    /* Inserting the area in the list of linked areas */
    ai = (area_info_t *) ib0->ptr.buffer;
    ai->next = tlsf->area_head;
    ai->end = lb0;
    tlsf->area_head = ai;
    free_ex(b0->ptr.buffer, mem_pool);

    TLSF_ADD_TOTAL_SIZE(tlsf, area_size);

    return (b0->size & BLOCK_SIZE);
}

/******************************************************************/
size_t get_used_size(void *mem_pool)
{
    /******************************************************************/
#if TLSF_STATISTIC
    return ((tlsf_t *) mem_pool)->used_size;
#else
    return 0;
#endif
}

/******************************************************************/
size_t get_max_size(void *mem_pool)
{
    /******************************************************************/
#if TLSF_STATISTIC
    return ((tlsf_t *) mem_pool)->max_size;
#else
    return 0;
#endif
}

/******************************************************************/
size_t get_total_size(void *mem_pool)
{
    /******************************************************************/
#if TLSF_STATISTIC
    return ((tlsf_t *) mem_pool)->total_size;
#else
    return 0;
#endif
}

/******************************************************************/
void destroy_memory_pool(void *mem_pool)
{
    /******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;

    tlsf->tlsf_signature = 0;

    TLSF_DESTROY_LOCK(&tlsf->lock);

}

/******************************************************************/
void *malloc_ex(size_t size, void *mem_pool)
{
    /******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;
    bhdr_t *b, *b2, *next_b;
    int fl, sl;
    size_t tmp_size;

    size = (size < MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : ROUNDUP_SIZE(size);

    /* Rounding up the requested size and calculating fl and sl */
    MAPPING_SEARCH(&size, &fl, &sl);

    /* Searching a free block, recall that this function changes the values of fl and sl,
     so they are not longer valid when the function fails */
    b = FIND_SUITABLE_BLOCK(tlsf, &fl, &sl);
#if USE_MMAP || USE_SBRK
    if (!b) {
        size_t area_size;
        void *area;
        /* Growing the pool size when needed */
        area_size = size + BHDR_OVERHEAD * 8;   /* size plus enough room for the requered headers. */
        area_size = (area_size > DEFAULT_AREA_SIZE) ? area_size : DEFAULT_AREA_SIZE;
        area = get_new_area(&area_size);        /* Call sbrk or mmap */
        if (area == ((void *) ~0))
            return NULL;        /* Not enough system memory */
        add_new_area(area, area_size, mem_pool);
        /* Rounding up the requested size and calculating fl and sl */
        MAPPING_SEARCH(&size, &fl, &sl);
        /* Searching a free block */
        b = FIND_SUITABLE_BLOCK(tlsf, &fl, &sl);
    }
#endif
    if (!b)
        return NULL;            /* Not found */

    EXTRACT_BLOCK_HDR(b, tlsf, fl, sl);

    /*-- found: */
    next_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    /* Should the block be split? */
    tmp_size = (b->size & BLOCK_SIZE) - size;
    if (tmp_size >= sizeof(bhdr_t)) {
        tmp_size -= BHDR_OVERHEAD;
        b2 = GET_NEXT_BLOCK(b->ptr.buffer, size);
        b2->size = tmp_size | FREE_BLOCK | PREV_USED;
        next_b->prev_hdr = b2;
        MAPPING_INSERT(tmp_size, &fl, &sl);
        INSERT_BLOCK(b2, tlsf, fl, sl);

        b->size = size | (b->size & PREV_STATE);
    } else {
        next_b->size &= (~PREV_FREE);
        b->size &= (~FREE_BLOCK);       /* Now it's used */
    }

    TLSF_ADD_SIZE(tlsf, b);

    return (void *) b->ptr.buffer;
}

/******************************************************************/
void free_ex(void *ptr, void *mem_pool)
{
    /******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;
    bhdr_t *b, *tmp_b;
    int fl = 0, sl = 0;

    if (!ptr) {
        return;
    }
    b = (bhdr_t *) ((char *) ptr - BHDR_OVERHEAD);
    b->size |= FREE_BLOCK;

    TLSF_REMOVE_SIZE(tlsf, b);

    b->ptr.free_ptr.prev = NULL;
    b->ptr.free_ptr.next = NULL;
    tmp_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    if (tmp_b->size & FREE_BLOCK) {
        MAPPING_INSERT(tmp_b->size & BLOCK_SIZE, &fl, &sl);
        EXTRACT_BLOCK(tmp_b, tlsf, fl, sl);
        b->size += (tmp_b->size & BLOCK_SIZE) + BHDR_OVERHEAD;
    }
    if (b->size & PREV_FREE) {
        tmp_b = b->prev_hdr;
        MAPPING_INSERT(tmp_b->size & BLOCK_SIZE, &fl, &sl);
        EXTRACT_BLOCK(tmp_b, tlsf, fl, sl);
        tmp_b->size += (b->size & BLOCK_SIZE) + BHDR_OVERHEAD;
        b = tmp_b;
    }
    MAPPING_INSERT(b->size & BLOCK_SIZE, &fl, &sl);
    INSERT_BLOCK(b, tlsf, fl, sl);

    tmp_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    tmp_b->size |= PREV_FREE;
    tmp_b->prev_hdr = b;
}

/******************************************************************/
void *realloc_ex(void *ptr, size_t new_size, void *mem_pool)
{
    /******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;
    void *ptr_aux;
    unsigned int cpsize;
    bhdr_t *b, *tmp_b, *next_b;
    int fl, sl;
    size_t tmp_size;

    if (!ptr) {
        if (new_size)
            return (void *) malloc_ex(new_size, mem_pool);
        if (!new_size)
            return NULL;
    } else if (!new_size) {
        free_ex(ptr, mem_pool);
        return NULL;
    }
    b = (bhdr_t *) ((char *) ptr - BHDR_OVERHEAD);
    next_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    new_size = (new_size < MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : ROUNDUP_SIZE(new_size);
    tmp_size = (b->size & BLOCK_SIZE);
    if (new_size <= tmp_size) {
        TLSF_REMOVE_SIZE(tlsf, b);
        if (next_b->size & FREE_BLOCK) {
            MAPPING_INSERT(next_b->size & BLOCK_SIZE, &fl, &sl);
            EXTRACT_BLOCK(next_b, tlsf, fl, sl);
            tmp_size += (next_b->size & BLOCK_SIZE) + BHDR_OVERHEAD;
            next_b = GET_NEXT_BLOCK(next_b->ptr.buffer, next_b->size & BLOCK_SIZE);
            /* We allways reenter this free block because tmp_size will
             be greater then sizeof (bhdr_t) */
        }
        tmp_size -= new_size;
        if (tmp_size >= sizeof(bhdr_t)) {
            tmp_size -= BHDR_OVERHEAD;
            tmp_b = GET_NEXT_BLOCK(b->ptr.buffer, new_size);
            tmp_b->size = tmp_size | FREE_BLOCK | PREV_USED;
            next_b->prev_hdr = tmp_b;
            next_b->size |= PREV_FREE;
            MAPPING_INSERT(tmp_size, &fl, &sl);
            INSERT_BLOCK(tmp_b, tlsf, fl, sl);
            b->size = new_size | (b->size & PREV_STATE);
        }
        TLSF_ADD_SIZE(tlsf, b);
        return (void *) b->ptr.buffer;
    }
    if ((next_b->size & FREE_BLOCK)) {
        if (new_size <= (tmp_size + (next_b->size & BLOCK_SIZE))) {
            TLSF_REMOVE_SIZE(tlsf, b);
            MAPPING_INSERT(next_b->size & BLOCK_SIZE, &fl, &sl);
            EXTRACT_BLOCK(next_b, tlsf, fl, sl);
            b->size += (next_b->size & BLOCK_SIZE) + BHDR_OVERHEAD;
            next_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
            next_b->prev_hdr = b;
            next_b->size &= ~PREV_FREE;
            tmp_size = (b->size & BLOCK_SIZE) - new_size;
            if (tmp_size >= sizeof(bhdr_t)) {
                tmp_size -= BHDR_OVERHEAD;
                tmp_b = GET_NEXT_BLOCK(b->ptr.buffer, new_size);
                tmp_b->size = tmp_size | FREE_BLOCK | PREV_USED;
                next_b->prev_hdr = tmp_b;
                next_b->size |= PREV_FREE;
                MAPPING_INSERT(tmp_size, &fl, &sl);
                INSERT_BLOCK(tmp_b, tlsf, fl, sl);
                b->size = new_size | (b->size & PREV_STATE);
            }
            TLSF_ADD_SIZE(tlsf, b);
            return (void *) b->ptr.buffer;
        }
    }

    if (!(ptr_aux = malloc_ex(new_size, mem_pool))){
        return NULL;
    }
    cpsize = ((b->size & BLOCK_SIZE) > new_size) ? new_size : (b->size & BLOCK_SIZE);

    memcpy(ptr_aux, ptr, cpsize);

    free_ex(ptr, mem_pool);
    return ptr_aux;
}

/******************************************************************/
void *calloc_ex(size_t nelem, size_t elem_size, void *mem_pool)
{
    /******************************************************************/
    void *ptr;

    if (nelem <= 0 || elem_size <= 0)
        return NULL;

    if (!(ptr = malloc_ex(nelem * elem_size, mem_pool)))
        return NULL;
    memset(ptr, 0, nelem * elem_size);

    return ptr;
}

#if 1
/***************  DEBUG FUNCTIONS   **************/

/* The following functions have been designed to ease the debugging of */
/* the TLSF  structure.  For non-developing  purposes, it may  be they */
/* haven't too much worth.  To enable them, _DEBUG_TLSF_ must be set.  */

extern void dump_memory_region(unsigned char *mem_ptr, unsigned int size);
extern void print_block(bhdr_t * b);
extern void print_tlsf(tlsf_t * tlsf);
void print_all_blocks(tlsf_t * tlsf);

void dump_memory_region(unsigned char *mem_ptr, unsigned int size)
{

    unsigned long begin = (unsigned long) mem_ptr;
    unsigned long end = (unsigned long) mem_ptr + size;
    int column = 0;

    begin >>= 2;
    begin <<= 2;

    end >>= 2;
    end++;
    end <<= 2;

    PRINT_MSG("\nMemory region dumped: 0x%lx - 0x%lx\n\n", begin, end);

    column = 0;
    PRINT_MSG("0x%lx ", begin);

    while (begin < end) {
        if (((unsigned char *) begin)[0] == 0)
            PRINT_MSG("00");
        else
            PRINT_MSG("%02x", ((unsigned char *) begin)[0]);
        if (((unsigned char *) begin)[1] == 0)
            PRINT_MSG("00 ");
        else
            PRINT_MSG("%02x ", ((unsigned char *) begin)[1]);
        begin += 2;
        column++;
        if (column == 8) {
            PRINT_MSG("\n0x%lx ", begin);
            column = 0;
        }

    }
    PRINT_MSG("\n\n");
}

void print_block(bhdr_t * b)
{
    if (!b)
        return;
    PRINT_MSG(">> [%p] (", b);
    if ((b->size & BLOCK_SIZE))
        PRINT_MSG("%lu bytes, ", (unsigned long) (b->size & BLOCK_SIZE));
    else
        PRINT_MSG("sentinel, ");
    if ((b->size & BLOCK_STATE) == FREE_BLOCK)
        PRINT_MSG("free [%p, %p], ", b->ptr.free_ptr.prev, b->ptr.free_ptr.next);
    else
        PRINT_MSG("used, ");
    if ((b->size & PREV_STATE) == PREV_FREE)
        PRINT_MSG("prev. free [%p])\n", b->prev_hdr);
    else
        PRINT_MSG("prev used)\n");
}

void print_tlsf(tlsf_t * tlsf)
{
    bhdr_t *next;
    int i, j;

    PRINT_MSG("\nTLSF at %p\n", tlsf);

    PRINT_MSG("FL bitmap: 0x%x\n\n", (unsigned) tlsf->fl_bitmap);

    for (i = 0; i < REAL_FLI; i++) {
        if (tlsf->sl_bitmap[i])
            PRINT_MSG("SL bitmap 0x%x\n", (unsigned) tlsf->sl_bitmap[i]);
        for (j = 0; j < MAX_SLI; j++) {
            next = tlsf->matrix[i][j];
            if (next)
                PRINT_MSG("-> [%d][%d]\n", i, j);
            while (next) {
                print_block(next);
                next = next->ptr.free_ptr.next;
            }
        }
    }
}

void print_all_blocks(tlsf_t * tlsf)
{
    area_info_t *ai;
    bhdr_t *next;
    PRINT_MSG("\nTLSF at %p\nALL BLOCKS\n\n", tlsf);
    ai = tlsf->area_head;
    while (ai) {
        next = (bhdr_t *) ((char *) ai - BHDR_OVERHEAD);
        while (next) {
            print_block(next);
            if ((next->size & BLOCK_SIZE))
                next = GET_NEXT_BLOCK(next->ptr.buffer, next->size & BLOCK_SIZE);
            else
                next = NULL;
        }
        ai = ai->next;
    }
}

#endif

static void * tlsf_allocmem(struct MemHeaderExt *mhe, IPTR size, ULONG *flags)
{
    void *ptr = malloc_ex(size, mhe->mhe_UserData);

    mhe->mhe_MemHeader.mh_Free = get_total_size(mhe->mhe_UserData) - get_used_size(mhe->mhe_UserData);

    if (flags && (*flags & MEMF_CLEAR))
        bzero(ptr, size);

    return ptr;
}

static void tlsf_freemem(struct MemHeaderExt *mhe, APTR ptr, IPTR size)
{
    free_ex(ptr, mhe->mhe_UserData);

    mhe->mhe_MemHeader.mh_Free = get_total_size(mhe->mhe_UserData) - get_used_size(mhe->mhe_UserData);
}

static void * tlsf_realloc(struct MemHeaderExt *mhe, APTR old, IPTR size)
{
    void *ptr = realloc_ex(old, size, mhe->mhe_UserData);

    mhe->mhe_MemHeader.mh_Free = get_total_size(mhe->mhe_UserData) - get_used_size(mhe->mhe_UserData);

    return ptr;
}

static ULONG tlsf_avail(struct MemHeaderExt *mhe, ULONG flags)
{
    IPTR ret = 0;

    if (flags & MEMF_TOTAL)
        ret = get_total_size(mhe->mhe_UserData);
    else if (flags & MEMF_LARGEST)
    {
        tlsf_t *tlsf = (tlsf_t *)mhe->mhe_UserData;
        bhdr_t *b = NULL;

        /* Find the list of largest blocks */
        if (tlsf->fl_bitmap)
        {
            /* The task is to find largest free block in this TLSF pool. */
            int fl = ms_bit(tlsf->fl_bitmap);

            if (tlsf->sl_bitmap[fl])
            {
                int sl = ms_bit(tlsf->sl_bitmap[fl]);

                b = tlsf->matrix[fl][sl];
            }
        }

        /*
         At this point b is a pointer to valid free block header, or NULL.
         Iterate through the blocks in this list - one of them is the largest one.
         */
        while (b)
        {
            /* Is this block the largest one? Update the return value then */
            if ((b->size & BLOCK_SIZE) > ret)
                ret = b->size & BLOCK_SIZE;

            /* Go to next block in this list */
            b = b->ptr.free_ptr.next;
        }
    }
    else
        ret = (get_total_size(mhe->mhe_UserData) - get_used_size(mhe->mhe_UserData));

    return ret;
}

/* AllocAbs implementation for TLSF. A little brute force but it seems to work OK */
static void * tlsf_allocabs(struct MemHeaderExt *mhe, IPTR size, APTR addr)
{
    tlsf_t *tlsf = (tlsf_t *)mhe->mhe_UserData;
    u8_t *region_start;
    u8_t *region_end;
    area_info_t *area;

    /* Round up the size to BHDR_OVERHEAD */
    size = ROUNDUP_SIZE(size);

    region_start = addr;
    region_end = (u8_t *)addr + size;

    /*
     Go throught the memory areas assigned to this tlsf pool.
     Search the one which contains requested block
     */
    area = tlsf->area_head;

    while(area)
    {
        /* Beginning of area's memory starts with contents of first block header */
        bhdr_t *b0 = (bhdr_t *)((char *)area - BHDR_OVERHEAD);
        bhdr_t *b1 = GET_NEXT_BLOCK(b0, b0->size & BLOCK_SIZE);

        /* Requested region has to be within b1 and can not exceed the area sentinel */
        if (b1->ptr.buffer <= region_start && (u8_t *)area->end >= region_end)
        {
            break;
        }

        area = area->next;
    }

    /* IF the region was not within the areas of this tlsf, make new area containing it */
    if (!area)
    {
#if USE_MMAP
        APTR new_area_start = (APTR)(((IPTR)addr - sizeof(area_info_t) - sizeof(bhdr_t)) & ~(PAGE_SIZE-1));
        APTR new_area_end = (APTR)(((IPTR)addr + size + sizeof(bhdr_t) + PAGE_SIZE-1) & ~(PAGE_SIZE-1));

        void *ptr;

        /* mmap the region at fixed address. */
        if ((ptr = getKernelBase()->kb_PlatformData->iface->mmap(new_area_start, (IPTR)new_area_end - (IPTR)new_area_start, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) != MAP_FAILED)
        {
            /* mmap succeeded. add new memory area to TLSF pool */
            add_new_area(ptr, (IPTR)new_area_end - (IPTR)new_area_start, tlsf);
        }
        else
        {
            /* mmap failed. allocabs shall fail too */
            return NULL;
        }
#else
        return NULL;
#endif
    }

    /*
     At this point there is an area which contains the requested region.

     Next step is to iterate through all the free entries which fullfil the size requrement for the
     requested region. We will check which of the entries contains the region itself.
     */

    int fl = 0;
    int sl = 0;
    size_t sz = size;

    /* Start searching here. It doesn't make sense to go through regions which are smaller */
    MAPPING_SEARCH(&sz, &fl, &sl);

    int i,j;

    /* Start looking now :) */
    for (i=fl; i < MAX_FLI; i++)
    {
        for (j=sl; j < MAX_SLI; j++)
        {
            bhdr_t *b0 = tlsf->matrix[i][j];

            /* If block was there, check it */
            while (b0)
            {
                bhdr_t *b1 = GET_NEXT_BLOCK(b0->ptr.buffer, b0->size & BLOCK_SIZE);

                /* The block has to contain _whole_ requested region, max exceed it in size though */
                if (b0->ptr.buffer <= region_start && (u8_t *)b1 >= region_end)
                {
                    /* block header of requested region */
                    bhdr_t *breg = (bhdr_t *)((u8_t *)addr - BHDR_OVERHEAD);

                    /*
                     This is the block we're looking for. Unchain it from the bidirectional list of
                     free blocks now.

                     Previous entry's next will point to this block's next. If previous is NULL, matrix
                     will be set to block's next
                     */
                    if (b0->ptr.free_ptr.prev)
                        b0->ptr.free_ptr.prev->ptr.free_ptr.next = b0->ptr.free_ptr.next;
                    else
                        tlsf->matrix[i][j] = b0->ptr.free_ptr.next;

                    /*
                     Next entry's prev will point to this block's previous.
                     */
                    if (b0->ptr.free_ptr.next)
                        b0->ptr.free_ptr.next->ptr.free_ptr.prev = b0->ptr.free_ptr.prev;

                    /* Empty SL matrix for size j? Clear bit */
                    if (!tlsf->matrix[i][j])
                    {
                        clear_bit(j, &tlsf->sl_bitmap[i]);

                        /* Empty entire SL matrix for given FL index? clear that bit too */
                        if (tlsf->sl_bitmap[i])
                            clear_bit(i, &tlsf->fl_bitmap);
                    }

                    b0->ptr.free_ptr.prev = NULL;
                    b0->ptr.free_ptr.next = NULL;
                    b0->size = (b0->size & ~BLOCK_STATE) | USED_BLOCK;

                    /*
                     At this point the block is removed from free list and marked as used.
                     Now, split it if necessary...
                     */

                    /* begin of the block != begin of the block header of requested region? */
                    if (b0 != breg)
                    {
                        /*
                         Adjust region's block header. Mark in size that previous (aka b0) is free.
                         Reduce the size of b0 as well as size of breg too.
                         */
                        breg->prev_hdr = b0;
                        breg->size = (b0->size & BLOCK_SIZE) - ((IPTR)breg - (IPTR)b0);
                        breg->size |= PREV_FREE | USED_BLOCK;

                        /* Update the next block. Mark in size that previous (breg) is used */
                        b1->prev_hdr = breg;
                        b1->size = (b1->size & ~PREV_STATE) | PREV_USED;

                        /* b0's prev state is keept. b0 itself is marked as free block */
                        b0->size &= PREV_STATE;
                        b0->size |= ((IPTR)breg - (IPTR)b0->ptr.buffer) | FREE_BLOCK;

                        /* Insert b0 to free list */
                        MAPPING_INSERT(b0->size & BLOCK_SIZE, &fl, &sl);
                        INSERT_BLOCK(b0, tlsf, fl, sl);
                    }

                    /* Is it necessary to split the requested region at the end? */
                    if ((BHDR_OVERHEAD + (breg->size & BLOCK_SIZE)) > size)
                    {
                        int tmp_size = (breg->size & BLOCK_SIZE) - size - BHDR_OVERHEAD;

                        /* New region header directly at end of the requested region */
                        bhdr_t *b2 = GET_NEXT_BLOCK(breg->ptr.buffer, size);

                        /* Adjust fields */
                        b2->prev_hdr = breg;
                        b2->size = tmp_size | PREV_USED | FREE_BLOCK;

                        /* requested region's size is now smaller */
                        breg->size &= PREV_STATE;
                        breg->size |= size;

                        /* The next block header point to newly created one */
                        b1->prev_hdr = b2;
                        b1->size = (b1->size & ~PREV_STATE) | PREV_FREE;

                        /* Insert newly created block to free list */
                        MAPPING_INSERT(b2->size & BLOCK_SIZE, &fl, &sl);
                        INSERT_BLOCK(b2, tlsf, fl, sl);
                    }

                    TLSF_ADD_SIZE(tlsf, breg);

                    mhe->mhe_MemHeader.mh_Free = get_total_size(mhe->mhe_UserData) - get_used_size(mhe->mhe_UserData);

                    return breg->ptr.buffer;
                }

                b0 = b0->ptr.free_ptr.next;
            }
        }
    }

    return NULL;
}

/*
 * Create MemHeader structure for the specified RAM region.
 * The header will be placed in the beginning of the region itself.
 * The header will NOT be added to the memory list!
 */
void krnCreateMemHeader(CONST_STRPTR name, BYTE pri, APTR start, IPTR size, ULONG flags)
{
    /* The MemHeader itself does not have to be aligned */
    struct MemHeaderExt *mhe = start;

    name = "TLSF Memory";

    /* If the end is less than (1 << 31), MEMF_31BIT is implied */
    if (((IPTR)start+size) < (1UL << 31))
        flags |= MEMF_31BIT;
    else
        flags &= ~MEMF_31BIT;

    mhe->mhe_MemHeader.mh_Node.ln_Succ    = NULL;
    mhe->mhe_MemHeader.mh_Node.ln_Pred    = NULL;
    mhe->mhe_MemHeader.mh_Node.ln_Type    = NT_MEMORY;
    mhe->mhe_MemHeader.mh_Node.ln_Name    = (STRPTR)name;
    mhe->mhe_MemHeader.mh_Node.ln_Pri     = pri;
    mhe->mhe_MemHeader.mh_Attributes      = flags | MEMF_MANAGED;

    /* The chunks have no meaning here. It's managed memory... */
    mhe->mhe_MemHeader.mh_First           = NULL;

    /*
     * mh_Lower and mh_Upper are informational only. Since our MemHeader resides
     * inside the region it describes, the region includes MemHeader.
     */
    mhe->mhe_MemHeader.mh_Lower           = (APTR)0;
    mhe->mhe_MemHeader.mh_Upper           = (APTR)-1;

    mhe->mhe_UserData = start + ((sizeof(struct MemHeaderExt)+15)&~15);

    bzero(mhe->mhe_UserData, 1024);

    init_memory_pool(size - ((sizeof(struct MemHeaderExt)+15)&~15), mhe->mhe_UserData);

    mhe->mhe_MemHeader.mh_Free = get_total_size(mhe->mhe_UserData) - get_used_size(mhe->mhe_UserData);

    mhe->mhe_Alloc = tlsf_allocmem;
    mhe->mhe_AllocAbs = tlsf_allocabs;
    mhe->mhe_Free = tlsf_freemem;
    mhe->mhe_ReAlloc = tlsf_realloc;
    mhe->mhe_Avail = tlsf_avail;
}

/*
 * Create informational MemHeader for ROM region.
 * The header will be allocated from system's public memory lists.
 * It will be not possible to allocate memory from the created MemHeader.
 * The header will be added to the memory list.
 * This routine uses exec.library/Allocate() for memory allocation, so it is safe
 * to use before exec.library and kernel.resource memory management is initialized.
 */
struct MemHeader *krnCreateROMHeader(CONST_STRPTR name, APTR start, APTR end)
{
    struct MemHeader *mh = AllocMem(sizeof(struct MemHeader), MEMF_ANY);

    if (mh)
    {
        mh->mh_Node.ln_Type = NT_MEMORY;
        mh->mh_Node.ln_Name = (STRPTR)name;
        mh->mh_Node.ln_Pri = -128;
        mh->mh_Attributes = MEMF_KICK;
        mh->mh_First = NULL;
        mh->mh_Lower = start;
        mh->mh_Upper = end + 1;			/* end is the last valid address of the region */
        mh->mh_Free = 0;                        /* Never allocate from this chunk! */
        Enqueue(&SysBase->MemList, &mh->mh_Node);
    }

    return mh;
}
