
#ifndef _RND_MEMORY_H
#define _RND_MEMORY_H

#include <utility/tagitem.h>
#include <exec/memory.h>

typedef struct RenderMemHandler
{
	APTR (*alloc)(struct RenderMemHandler *object, LONG size);
	void (*free)(struct RenderMemHandler *object, APTR mem, LONG size);
	struct MemHeader mh;
	struct SignalSemaphore lock;
	APTR object;
	ULONG type;
	ULONG memflags;

} RNDMH;

LIBAPI void memfill8(UBYTE *d, ULONG len, UBYTE fill);
LIBAPI void memfill32(ULONG *d, ULONG len, ULONG fill);
LIBAPI void TurboFillMem(APTR dst, ULONG len, ULONG val);
LIBAPI void TurboCopyMem(APTR src, APTR dst, ULONG len);
LIBAPI RNDMH *CreateRMHandlerA(struct TagItem *tags);
LIBAPI void DeleteRMHandler(RNDMH *RNDMH);
LIBAPI APTR AllocRenderMem(RNDMH *RNDMH, ULONG size);
LIBAPI void FreeRenderMem(RNDMH *RNDMH, APTR mem, ULONG size);
LIBAPI APTR AllocRenderVec(RNDMH *RNDMH, ULONG size);
LIBAPI void FreeRenderVec(ULONG *mem);
LIBAPI APTR AllocRenderVecClear(APTR mem, ULONG size);

#endif
