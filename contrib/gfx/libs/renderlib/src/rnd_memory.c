
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>

LIBAPI void memfill32(ULONG *d, ULONG len, ULONG fill)
{
	ULONG len8;
	len >>= 2;
	len8 = (len >> 3) + 1;
	switch (len & 7)
	{
		do
		{
					*d++ = fill;
			case 7:	*d++ = fill;
			case 6:	*d++ = fill;
			case 5:	*d++ = fill;
			case 4:	*d++ = fill;
			case 3:	*d++ = fill;
			case 2:	*d++ = fill;
			case 1:	*d++ = fill;
			case 0:	len8--;

		} while (len8);
	}
}

LIBAPI void memfill8(UBYTE *d, ULONG len, UBYTE fill)
{
	ULONG len8;
	len8 = (len >> 3) + 1;
	switch (len & 7)
	{
		do
		{
					*d++ = fill;
			case 7:	*d++ = fill;
			case 6:	*d++ = fill;
			case 5:	*d++ = fill;
			case 4:	*d++ = fill;
			case 3:	*d++ = fill;
			case 2:	*d++ = fill;
			case 1:	*d++ = fill;
			case 0:	len8--;

		} while (len8);
	}
}

static void memcpy8reverse(UBYTE *s, UBYTE *d, ULONG len)
{
	ULONG len8;
	s += len;
	d += len;
	len8 = (len >> 3) + 1;
	switch (len & 7)
	{
		do
		{
					*--d = *--s;
			case 7:	*--d = *--s;
			case 6:	*--d = *--s;
			case 5:	*--d = *--s;
			case 4:	*--d = *--s;
			case 3:	*--d = *--s;
			case 2:	*--d = *--s;
			case 1:	*--d = *--s;
			case 0:	len8--;

		} while (len8);
	}
}


static APTR public_alloc(RNDMH *rmh, LONG size)
{
	return AllocMem(size, rmh->memflags);
}

static void public_free(RNDMH *rmh, APTR mem, LONG size)
{
	FreeMem(mem, size);
}

static APTR priv_alloc(RNDMH *rmh, LONG size)
{
	APTR mem;
	ObtainSemaphore(&rmh->lock);
	mem = Allocate(&rmh->mh, size);
	ReleaseSemaphore(&rmh->lock);
	return mem;
}

static void priv_free(RNDMH *rmh, APTR mem, LONG size)
{
	ObtainSemaphore(&rmh->lock);
	Deallocate(&rmh->mh, mem, size);
	ReleaseSemaphore(&rmh->lock);
}

static APTR pool_alloc(RNDMH *rmh, LONG size)
{
	APTR mem;
	ObtainSemaphore(&rmh->lock);
	mem = AllocPooled(rmh->object, size);
	ReleaseSemaphore(&rmh->lock);
	return mem;
}

static void pool_free(RNDMH *rmh, APTR mem, LONG size)
{
	ObtainSemaphore(&rmh->lock);
	FreePooled(rmh->object, mem, size);
	ReleaseSemaphore(&rmh->lock);
}

LIBAPI RNDMH *CreateRMHandlerA(struct TagItem *tags)
{
	RNDMH *rmh = AllocMem(sizeof(RNDMH), MEMF_ANY);
	if (!rmh) return NULL;
	
	rmh->type = GetTagData(RND_MemType, RMHTYPE_PUBLIC, tags);
	switch (rmh->type)
	{
		default:
			goto fail;
	
		case RMHTYPE_POOL:
			rmh->memflags = GetTagData(RND_MemFlags, MEMF_ANY, tags);						
			rmh->object = CreatePool(rmh->memflags, 16384, 2048);
			if (!rmh->object) goto fail;
			rmh->alloc = pool_alloc;
			rmh->free = pool_free;
			InitSemaphore(&rmh->lock);
			return rmh;
					
		case RMHTYPE_PUBLIC:
			rmh->memflags = GetTagData(RND_MemFlags, MEMF_ANY, tags);
			rmh->alloc = public_alloc;
			rmh->free = public_free;
			return rmh;

		case RMHTYPE_PRIVATE:
		{	
			struct MemChunk *mc = (struct MemChunk *) GetTagData(RND_MemBlock, NULL, tags);
			ULONG size = GetTagData(RND_MemSize, 0, tags);
			if (!mc || !size) goto fail;
			rmh->mh.mh_Node.ln_Type = NT_MEMORY;
			rmh->mh.mh_Node.ln_Name = NULL;
			rmh->mh.mh_First = mc;
			rmh->mh.mh_Lower = (APTR) mc;
			rmh->mh.mh_Upper = (APTR) ((ULONG) mc + size);
			rmh->mh.mh_Free = size;
			mc->mc_Next = NULL;
			mc->mc_Bytes = size;
			rmh->alloc = priv_alloc;
			rmh->free = priv_free;
			memfill8((UBYTE *) &rmh->lock, sizeof(struct SignalSemaphore), 0);
			InitSemaphore(&rmh->lock);
			return rmh;
		}
	}

fail:

	FreeMem(rmh, sizeof(RNDMH));
	return NULL;
}

LIBAPI void DeleteRMHandler(RNDMH *rmh)
{
	if (!rmh) return;
	if (rmh->type == RMHTYPE_POOL)
	{
		DeletePool(rmh->object);
	}
	FreeMem(rmh, sizeof(RNDMH));
}

LIBAPI APTR AllocRenderMem(RNDMH *rmh, ULONG size)
{
	if (rmh) return (*rmh->alloc)(rmh, size);
	return AllocMem(size, MEMF_ANY);
}

LIBAPI void FreeRenderMem(RNDMH *rmh, APTR mem, ULONG size)
{
	if (rmh)
	{
		(*rmh->free)(rmh, mem, size);
	}
	else
	{
		FreeMem(mem, size);
	}
}

LIBAPI APTR AllocRenderVec(RNDMH *rmh, ULONG size)
{
	ULONG *mem;
	size += 8;
	mem = AllocRenderMem(rmh, size);
	if (mem)
	{
		*mem++ = (ULONG) rmh;
		*mem++ = size;
	}
	return mem;
}

LIBAPI void FreeRenderVec(ULONG *mem)
{
	if (mem)
	{
		ULONG size = *--mem;
		RNDMH *rmh = (RNDMH *) *--mem;
		FreeRenderMem(rmh, mem, size);
	}
}

LIBAPI APTR AllocRenderVecClear(APTR rmh, ULONG size)
{
	ULONG *mem;
	size = (size + 3) & ~3;
	mem = AllocRenderVec(rmh, size);
	if (mem)
	{
		memfill32(mem, size, 0);
	}
	return mem;
}


LIBAPI void TurboFillMem(APTR dst, ULONG len, ULONG v)
{
	if (((((ULONG) dst) & 3) == 0) && ((len & 3) == 0))
	{	
		v &= 0xff;
		v = (v<<8) | v;
		v = (v<<16) | v;
		memfill32(dst, len, v);
	}
	else
	{
		memfill8((UBYTE *) dst, len, (UBYTE) v);
	}
}


LIBAPI void TurboCopyMem(APTR src, APTR dst, ULONG len)
{
	if (((ULONG) dst > (ULONG) src) && ((ULONG) dst < (ULONG) src + len))
	{
		memcpy8reverse((UBYTE *) src, (UBYTE *) dst, len);
		return;
	}

	if ((((ULONG) src) & 3) || (((ULONG) dst) & 3) || len & 3)
	{
		CopyMem(src, dst, len);
		return;
	}

	CopyMemQuick(src, dst, len);
}
