
#include <string.h>

#include <proto/exec.h>
#include <exec/memory.h>

#include "mystic_data.h"
#include "mystic_global.h"



#ifdef USEPOOLS
	APTR mainpool;
	struct SignalSemaphore memsemaphore;

#ifdef __AROS__
#include <aros/symbolsets.h>
#include LC_LIBDEFS_FILE

AROS_SET_LIBFUNC(InitMem, LIBBASETYPE, LIBBASE)
{
    AROS_SET_LIBFUNC_INIT
    
    mainpool = CreatePool(MEMF_ANY, 4096, 4096);
    InitSemaphore(&memsemaphore);
    
    return mainpool != NULL;
    
    AROS_SET_LIBFUNC_EXIT
}

AROS_SET_LIBFUNC(CleanUpMem, LIBBASETYPE, LIBBASE)
{
    AROS_SET_LIBFUNC_INIT

    if (mainpool != NULL)
    {
	DeletePool(mainpool);
	mainpool = NULL;
    }
    
    return TRUE;
    
    AROS_SET_LIBFUNC_EXIT
}

ADD2INITLIB(InitMem, 0);
ADD2EXPUNGELIB(CleanUpMem, 0);
#endif

#endif


/*--------------------------------------------------------------------

	Malloc
	Free

--------------------------------------------------------------------*/

#ifdef TRACKMEMORY
	static LONG alloccount = 0;
	static LONG allocbytes = 0;
#endif

void *Malloc(unsigned long size)
{
	#ifndef USEPOOLS

		void *buf;
		if (buf = AllocVec(size, MEMF_PUBLIC))
		{
			#ifdef TRACKMEMORY
		
				alloccount++;
			
			#endif
		}
		return buf;

	#else

		ULONG *buf;
		
		ObtainSemaphore(&memsemaphore);

		if ((buf = AllocPooled(mainpool, size + sizeof(ULONG))))
		{
			*buf++ = size;

			#ifdef TRACKMEMORY
				alloccount++;
				allocbytes += size + sizeof(ULONG);
			#endif
		}

		ReleaseSemaphore(&memsemaphore);

		return (void *) buf;

	#endif
}

void *Malloclear(unsigned long size)
{
	#ifndef USEPOOLS

		void *buf;
		if (buf = AllocVec(size, MEMF_PUBLIC + MEMF_CLEAR))
		{
			#ifdef TRACKMEMORY
				alloccount++;
			#endif
		}
		return buf;

	#else

		ULONG *buf;

		ObtainSemaphore(&memsemaphore);

		if ((buf = AllocPooled(mainpool, size + sizeof(ULONG))))
		{
			*buf++ = size;

			memset(buf, 0, size);

			#ifdef TRACKMEMORY
				alloccount++;
				allocbytes += size + sizeof(ULONG);
			#endif
		}

		ReleaseSemaphore(&memsemaphore);

		return (void *) buf;

	#endif
}


void _Free(void *mem)
{
	#ifndef USEPOOLS

		if (mem)
		{
			#ifdef TRACKMEMORY
				alloccount--;
			#endif
			FreeVec(mem);
		}

	#else

		if (mem)
		{
			ULONG *buf = (ULONG *) mem;

			ObtainSemaphore(&memsemaphore);

			--buf;

			#ifdef TRACKMEMORY
				alloccount--;
				allocbytes -= *buf + sizeof(ULONG);
			#endif

			FreePooled(mainpool, buf, *buf + sizeof(ULONG));

			ReleaseSemaphore(&memsemaphore);
		}

	#endif
}


/*--------------------------------------------------------------------

	s2 = StrDup(s)

--------------------------------------------------------------------*/

char *_StrDup(char *s)
{
	char *s2 = NULL;

	if (s)
	{
		int l = strlen(s);
		if ((s2 = Malloc(l+1)))
		{
			strcpy(s2, s);
		}
	}
	
	return s2;
}



LONG _Strcmp(char *s1, char *s2)
{
	if (s1 && s2)
	{
		return strcmp(s1, s2);
	}

	if (s1 == NULL && s2 == NULL)
	{
		return 0;
	}
	
	return -1;
}
