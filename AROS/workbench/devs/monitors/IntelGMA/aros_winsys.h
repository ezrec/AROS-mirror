/*
    Copyright © 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef INTEL_AROS_WINSYS_H
#define INTEL_AROS_WINSYS_H

#include "i915/i915_batchbuffer.h"
#include "hidd/gallium.h"

struct aros_winsys
{
    struct i915_winsys base;
    struct pipe_screen *pscreen;
    size_t max_batch_size;
};

static INLINE struct aros_winsys *
aros_winsys(struct i915_winsys *iws)
{
   return (struct aros_winsys *)iws;
}

struct aros_batchbuffer
{
   struct i915_winsys_batchbuffer base;
   size_t actual_size;
   APTR allocated_map;
   ULONG allocated_size;
};

static INLINE struct aros_batchbuffer *
aros_batchbuffer(struct i915_winsys_batchbuffer *batch)
{
   return (struct aros_batchbuffer *)batch;
}

struct aros_buffer {
   unsigned magic;
   APTR map;
   ULONG size;
   void *ptr;
   ULONG stride;
  // unsigned map_count;
  // boolean flinked;
  // unsigned flink;
   APTR allocated_map;
   ULONG allocated_size;
};

static INLINE struct aros_buffer *
aros_buffer(struct i915_winsys_buffer *buffer)
{
   return (struct aros_buffer *)buffer;
}

struct aros_winsys *winsys_create();

#endif
