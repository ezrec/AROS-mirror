
/*
	Simple memory allocation routines
*/

#include <exec/memory.h>
#include <proto/exec.h>
#include <string.h>
#include <stdlib.h>

typedef	struct memheader_t
{
	unsigned int	magic;
	int				size;
	int				pad[2];			/* to make header 16 bytes alligned */
} MemHeader;

static APTR memoryPool = NULL;

static void MemoryExit(void);
void kprintf(char *fmt,...);

void MemoryInit(void)
{
	// initialize memory pool:
	// puddle size: 1MB
	// threshold : 256kB ( FIXME: find out better value )
	
	memoryPool = CreatePool(MEMF_ANY | MEMF_SEM_PROTECTED, 1024*1024, 64*1024);

	atexit(MemoryExit);
}

static void MemoryExit(void)
{
	if (memoryPool != NULL)
		DeletePool(memoryPool);
}


void mfree(void *mem)
{
	if (memoryPool != NULL)
	{
		if (mem != NULL)
		{
			MemHeader *mh = (MemHeader*)( (unsigned char*)mem - sizeof(MemHeader));
			unsigned int *endofmem = (unsigned int*)((unsigned char*)mem + mh->size);

			if (mh->magic != 0xDEADBEEF || *endofmem != 0xDEADBEEF)
			{
				kprintf("[Memory]: Invalid memory header! ptr = 0x%x size = %d head = 0x%x, tail = 0x%x\n", mem, mh->size, mh->magic, *endofmem);
				DumpTaskState(FindTask(NULL));
				return;
			}
			else
			{
				mh->magic = 0xABADCAFE;
				FreePooled(memoryPool, mh, mh->size + sizeof(MemHeader) + 4);
			}
		}
	}
}

void *mmalloc(int size)
{
	if (size == 0)
		return NULL;

	if (memoryPool != NULL)
	{
		MemHeader *mh;
		unsigned char *mem;

		mh = AllocPooledAligned(memoryPool, size + sizeof(MemHeader) + 4, 16, 0);
		mem = (unsigned char*)mh + sizeof(MemHeader);

		if (mh != NULL)
		{
            mh->size = size;
			mh->magic = 0xDEADBEEF;
			*(unsigned int*)(mem + size) = 0xDEADBEEF;

			return mem;
		}
	}

	return NULL;

}

void *mcalloc(int a, int b)
{
	void *mem = mmalloc(a * b);

	if (mem != NULL)
	{
		memset(mem, 0, a * b);
		return mem;
	}

	return NULL;
}


