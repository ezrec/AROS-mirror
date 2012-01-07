/*
    Copyright © 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 0

#include <aros/debug.h>

#include "intelG45_intern.h"
#include "intelG45_regs.h"
#include "aros_winsys.h"

#include "util/u_memory.h"
#include "i915/i915_winsys.h"
#include "i915/i915_debug.h"

#include "i915/i915_reg.h"


#ifdef GALLIUM_SIMULATION
    #undef LOCK_HW
    #undef START_RING
    #undef OUT_RING
    #undef ADVANCE_RING
    #undef WAIT_IDLE
    #undef DO_FLUSH
    #undef UNLOCK_HW
    #define LOCK_HW
    #define START_RING(x)
    #define OUT_RING(x)
    #define ADVANCE_RING()
    #define WAIT_IDLE()
    #define DO_FLUSH()
    #define UNLOCK_HW
    #warning GALLIUM_SIMULATION MODE!
#endif

#define IMPLEMENT() bug("[GMA winsys]------IMPLEMENT(%s)\n", __func__)


static APTR hw_status[1024];
static APTR *bb_map;
static ULONG temp_index;
static struct SignalSemaphore BatchBufferLock;

extern struct g45staticdata sd;
#define sd ((struct g45staticdata*)&(sd))
#define LOCK_BB          { ObtainSemaphore(&BatchBufferLock); }
#define UNLOCK_BB        { ReleaseSemaphore(&BatchBufferLock); }

#define BASEADDRESS(p) ((uint32_t)(p) - (intptr_t)sd->Card.Framebuffer)
struct i915_winsys_batchbuffer *batchbuffer_create(struct i915_winsys *iws);

ULONG reserve_status_index(APTR p)
{
    int i;
    for(i=0;i<1024;i++)
    {
        if( hw_status[i] == 0 )
        {
            hw_status[i] = p;
            D(bug("[GMA winsys] reserve_status_index(%p)=%d\n",p,i));
            return i;
        }
    }
    return 0;
}

VOID free_status_index(ULONG i)
{
    hw_status[i] = 0;
    D(bug("[GMA winsys] free_status_index(%d)\n",i));
}

BOOL get_status(ULONG i)
{
#ifdef GALLIUM_SIMULATION
 return 0;
#endif
    return readl( &sd->HardwareStatusPage[ i ]);
}

VOID set_status(ULONG i,ULONG v)
{
#ifndef GALLIUM_SIMULATION
    writel( v, &sd->HardwareStatusPage[ i ] );
#endif
}

APTR AllocGfxMem(ULONG size)
{
    APTR result;
#ifdef GALLIUM_SIMULATION
    return malloc(size);
#endif

    result = Allocate(&sd->CardMem, size );
    //D(bug("[GMA winsys] AllocGfxMem(%d) = %p\n",size,result));
    return result;
}

VOID FreeGfxMem(APTR ptr, ULONG size)
{
#ifdef GALLIUM_SIMULATION
    free(ptr);return;
#endif
     Deallocate(&sd->CardMem, ptr,  size );
}

VOID init_aros_winsys()
{

    // clean hw_status table,reserve first 100,( 0-15 is reserved,and bitmapclass uses at least 16-20)
    int i;
    for(i=0;i<1024;i++)
    {
        if(i<100) hw_status[i]=(APTR)1;
        else hw_status[i]=0;
    }

    // allocate batchbuffer
    bb_map = (APTR)((IPTR)AllocGfxMem( 16*4096 + 4096) & ~4095);
    temp_index = reserve_status_index( bb_map );

    InitSemaphore(&BatchBufferLock);
}

/*******************************************************************
* Batchbuffer functions.
******************************************************************/

#define BATCH_RESERVED 16

static void batchbuffer_reset(struct aros_batchbuffer *batch)
{

    D(bug("[GMA winsys] batchbuffer_reset\n"));
    memset(batch->base.map, 0, batch->actual_size);
    batch->base.ptr = batch->base.map;
    batch->base.size = batch->actual_size - BATCH_RESERVED;
    batch->base.relocs = 0;

}

/**
* Create a new batchbuffer.
*/
struct i915_winsys_batchbuffer *batchbuffer_create(struct i915_winsys *iws)
{
    D(bug("[GMA winsys] batchbuffer_create\n"));
    struct aros_winsys *idws = aros_winsys(iws);
    struct aros_batchbuffer *batch = CALLOC_STRUCT(aros_batchbuffer);

    batch->actual_size = idws->max_batch_size;
    batch->base.map = MALLOC(batch->actual_size);

    batch->base.ptr = NULL;
    batch->base.size = 0;

    batch->base.relocs = 0;
    batch->base.iws = iws;

    batchbuffer_reset(batch);

    return &batch->base;
}

/**
* Validate buffers for usage in this batchbuffer.
* Does space-checking and asorted other book-keeping.
*
* @batch
* @buffers array to buffers to validate
* @num_of_buffers size of the passed array
*/
boolean batchbuffer_validate_buffers(struct i915_winsys_batchbuffer *batch,
                   struct i915_winsys_buffer **buffers,
                   int num_of_buffers)
{
     D(
        bug("[GMA winsys] batchbuffer_validate_buffers\n");
        int i;
        for(i=0;i<num_of_buffers;i++)
        {
            bug("    buffer %p\n",buffers[i]);
        }
     );
   //  IMPLEMENT();
     return TRUE;
}

/**
* Emit a relocation to a buffer.
* Target position in batchbuffer is the same as ptr.
*
* @batch
* @reloc buffer address to be inserted into target.
* @usage how is the hardware going to use the buffer.
* @offset add this to the reloc buffers address
* @target buffer where to write the address, null for batchbuffer.
* @fenced relocation needs a fence.
*/
int batchbuffer_reloc(struct i915_winsys_batchbuffer *batch,
                        struct i915_winsys_buffer *reloc,
                        enum i915_winsys_buffer_usage usage,
                        unsigned offset, boolean fenced)
{
    *(uint32_t *)(batch->ptr) = BASEADDRESS( aros_buffer(reloc)->map ) + offset ;
    D(bug("[GMA winsys] batchbuffer_reloc reloc %p offset %d fenced %s base=%p \n",reloc,offset,fenced ? "true" : "false",*(uint32_t *)(batch->ptr)));
    batch->ptr += 4;
    batch->relocs++;
    return 0;
}

/**
* Flush a bufferbatch.
*/

#define MI_BATCH_NON_SECURE     (1)

void batchbuffer_flush(struct i915_winsys_batchbuffer *batch,
                             struct pipe_fence_handle **fence)
{
    D(bug("[GMA winsys] batchbuffer_flush size=%d\n",batch->ptr - batch->map));

#if 0
    batch->ptr = batch->map;
    *(uint32_t *)(batch->ptr) = MI_NOOP;
    batch->ptr += 4;
#endif

    *(uint32_t *)(batch->ptr) = MI_BATCH_BUFFER_END;
    batch->ptr += 4;
//  i915_dump_batchbuffer( batch );

#ifndef GALLIUM_SIMULATION

    LOCK_BB
        // wait until previous batchbuffer is ready.
        while( get_status( temp_index ) ){
          //  bug("wait...\n");
        };

        // copy to gfxmem
        memcpy(bb_map, batch->map, batch->ptr - batch->map );

        // set status
        set_status( temp_index , 1 );

        LOCK_HW

            //run
            START_RING(6);

                // start batchbuffer
                OUT_RING( MI_BATCH_BUFFER_START | (2 << 6) );
                OUT_RING( BASEADDRESS( bb_map ) | MI_BATCH_NON_SECURE);

                // clean status
                OUT_RING((0x21 << 23) | 1);
                OUT_RING( temp_index << 2 );
                OUT_RING(0);
                OUT_RING(0);

            ADVANCE_RING();

        UNLOCK_HW

    UNLOCK_BB

    // wait...
   // while( get_status( temp_index ) ){};

#endif
    batchbuffer_reset( aros_batchbuffer(batch) );
}


/**
* Destroy a batchbuffer.
*/
void batchbuffer_destroy(struct i915_winsys_batchbuffer *ibatch)
{
    D(bug("[GMA winsys] batchbuffer_destroy %p\n",ibatch));
    struct aros_batchbuffer *batch = aros_batchbuffer(ibatch);
    FREE(ibatch->map);
    FREE(batch);
}

/*******************************************************************
* Buffer functions.
*****************************************************************/
D(
    static char *i915_type_to_name(enum i915_winsys_buffer_type type)
    {
       char *name;
       if (type == I915_NEW_TEXTURE) {
          name = "gallium3d_texture";
       } else if (type == I915_NEW_VERTEX) {
          name = "gallium3d_vertex";
       } else if (type == I915_NEW_SCANOUT) {
          name = "gallium3d_scanout";
       } else {
          name = "gallium3d_unknown";
       }
       return name;
    }
)

/**
* Create a buffer.
*/
struct i915_winsys_buffer *
      buffer_create(struct i915_winsys *iws,
                       unsigned size,
                       enum i915_winsys_buffer_type type)
{
    struct aros_buffer *buf = CALLOC_STRUCT(aros_buffer);
    if (!buf)
    return NULL;

    // allocate page aligned gfx memory
    buf->allocated_size = size + 4095;
    if( !(buf->allocated_map = AllocGfxMem(buf->allocated_size) ) ) return NULL;
    buf->map = (APTR)(((uint32_t)buf->allocated_map + 4095)& ~4095);
    buf->size = size;
    D(bug("[GMA winsys] buffer_create size %d type %s = %p map %p\n",size,i915_type_to_name(type),buf,buf->map));

    return (struct i915_winsys_buffer *)buf;

}


/**
* Create a tiled buffer.
*
* *stride, height are in bytes. The winsys tries to allocate the buffer with
* the tiling mode provide in *tiling. If tiling is no possible, *tiling will
* be set to I915_TILE_NONE. The calculated stride (incorporateing hw/kernel
* requirements) is always returned in *stride.
*/
struct i915_winsys_buffer *
      buffer_create_tiled(struct i915_winsys *iws,
                             unsigned *stride, unsigned height,
                             enum i915_winsys_buffer_tile *tiling,
                             enum i915_winsys_buffer_type type)
{
    D(bug("[GMA winsys] buffer_create_tiled stride=%d heigh=%d type:%s\n",*stride,height,i915_type_to_name(type)));
    // Tiling is not implemented.
    *tiling = I915_TILE_NONE;
    return buffer_create( iws, *stride * height, type );
}


/**
* Creates a buffer from a handle.
* Used to implement pipe_screen::resource_from_handle.
* Also provides the stride information needed for the
* texture via the stride argument.
*/
struct i915_winsys_buffer *
buffer_from_handle(struct i915_winsys *iws,
                            struct winsys_handle *whandle,
                            enum i915_winsys_buffer_tile *tiling,
                            unsigned *stride)
{
    D(bug("[GMA winsys] buffer_from_handle\n"));
    IMPLEMENT();
    return NULL;
}


/**
* Used to implement pipe_screen::resource_get_handle.
* The winsys might need the stride information.
*/
boolean buffer_get_handle(struct i915_winsys *iws,
                                struct i915_winsys_buffer *buffer,
                                struct winsys_handle *whandle,
                                unsigned stride)
{
    D(bug("[GMA winsys] buffer_get_handle\n"));
    IMPLEMENT();
    return FALSE;
}


/**
* Map a buffer.
*/
void *buffer_map(struct i915_winsys *iws,
                       struct i915_winsys_buffer *buffer,
                       boolean write)
{
    D(bug("[GMA winsys] buffer_map %p\n",buffer));

    // wait until batchbuffer is ready. optimization: check if buffer is used in current batchbuffer.?
    while( get_status( temp_index )){}

    return aros_buffer(buffer)->map;
}


/**
* Unmap a buffer.
*/
void buffer_unmap(struct i915_winsys *iws,
                        struct i915_winsys_buffer *buffer)
{
    D(bug("[GMA winsys] buffer_unmap %p\n",buffer));
   // IMPLEMENT();
}


/**
* Write to a buffer.
*
* Arguments follows pipe_buffer_write.
*/
int buffer_write(struct i915_winsys *iws,
                       struct i915_winsys_buffer *dst,
                       size_t offset,
                       size_t size,
                       const void *data)
{
    D(bug("[GMA winsys] buffer_write offset=%d size=%d\n",offset,size));
    IMPLEMENT();
    return 0;
}


void buffer_destroy(struct i915_winsys *iws,
                          struct i915_winsys_buffer *buffer)
{
    D(bug("[GMA winsys] buffer_destroy %p\n", buffer));

    // wait until batchbuffer is ready.
    while( get_status( temp_index )){}
    DO_FLUSH();

    FreeGfxMem(aros_buffer(buffer)->allocated_map,aros_buffer(buffer)->allocated_size);
    FREE(buffer);
}


/**
* Check if a buffer is busy.
*/
boolean buffer_is_busy(struct i915_winsys *iws,
                             struct i915_winsys_buffer *buffer)
{
    D(bug("[GMA winsys] buffer_is_busy %p =%d\n",buffer,get_status( temp_index )));
    // optimization: check if buffer is used in current batchbuffer.?
    if( get_status( temp_index )) return TRUE;
    return FALSE;
}




/****************************************************************
* Fence functions.
*************************************************************/

/**
* Reference fence and set ptr to fence.
*/
void fence_reference(struct i915_winsys *iws,
                           struct pipe_fence_handle **ptr,
                           struct pipe_fence_handle *fence)
{
    D(bug("[GMA winsys] fence_reference\n"));
    IMPLEMENT();
}


/**
* Check if a fence has finished.
*/
int fence_signalled(struct i915_winsys *iws,
                          struct pipe_fence_handle *fence)
{
    D(bug("[GMA winsys] fence_signalled\n"));
    IMPLEMENT();
    return 0;
}


/**
* Wait on a fence to finish.
*/
int fence_finish(struct i915_winsys *iws,
                       struct pipe_fence_handle *fence)
{
    D(bug("[GMA winsys] fence_finish\n"));
    IMPLEMENT();
    return 0;
}


void winsys_destroy(struct i915_winsys *ws)
{
    D(bug("[GMA winsys] winsys_destroy\n"));
    struct aros_winsys *aws = aros_winsys(ws);
    FREE(aws);
}

struct aros_winsys *
winsys_create()
{
    D(bug("[GMA winsys] winsys_create\n"));
    struct aros_winsys *aws;

    aws = CALLOC_STRUCT(aros_winsys);
    if (!aws) return NULL;

    aws->base.batchbuffer_create = batchbuffer_create;
    aws->base.validate_buffers = batchbuffer_validate_buffers;
    aws->base.batchbuffer_reloc = batchbuffer_reloc;
    aws->base.batchbuffer_flush = batchbuffer_flush;
    aws->base.batchbuffer_destroy = batchbuffer_destroy;

    aws->base.buffer_create = buffer_create;
    aws->base.buffer_create_tiled = buffer_create_tiled;
    aws->base.buffer_from_handle = buffer_from_handle;
    aws->base.buffer_get_handle = buffer_get_handle;
    aws->base.buffer_map = buffer_map;
    aws->base.buffer_unmap = buffer_unmap;
    aws->base.buffer_write = buffer_write;
    aws->base.buffer_destroy = buffer_destroy;
    aws->base.buffer_is_busy = buffer_is_busy;

    aws->base.fence_reference = fence_reference;
    aws->base.fence_signalled = fence_signalled;
    aws->base.fence_finish = fence_finish;

    aws->base.destroy = winsys_destroy;

    aws->base.pci_id = sd->ProductID;
#ifdef GALLIUM_SIMULATION
    aws->base.pci_id = 0x27AE;
#endif
    aws->max_batch_size = 16 * 4096;

    return aws;
}
