//
// pmmem.h
//
// $Date$
// $Revision$
//

#include "pmpriv.h"
#include "pmmem.h"

APTR PM_AllocVecPooled(size_t size)
{
	APTR ptr;

	if (MemPool)
		{
		ObtainSemaphore(&MemPoolSemaphore);
		ptr = AllocPooled(MemPool, size + sizeof(size_t));
		ReleaseSemaphore(&MemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = size;

			d1(kprintf(__FILE__ "/%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n",
				__FUNC__, __LINE__, MemPool, size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf(__FILE__ "/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FUNC__, __LINE__, MemPool, size));

	return NULL;
}

void PM_FreeVecPooled(APTR mem)
{
	d1(kprintf(__FUNC__ "/%ld:  MemPool=%08lx  mem=%08lx\n", __LINE__, MemPool, mem));
	if (MemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		d1(kprintf(__FILE__ "/%s/%ld:  MemPool=%08lx  size=%lu  mem=%08lx\n",
			__FUNC__, __LINE__, MemPool, size, &sptr[1]));

		ObtainSemaphore(&MemPoolSemaphore);
		FreePooled(MemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&MemPoolSemaphore);
		}
}

ULONG PM_String_Length(STRPTR s)
{
	ULONG r=(ULONG)s;
	
	while (*s++);
	
	return ((ULONG)s)-r;
}

STRPTR PM_String_Copy(STRPTR Source, STRPTR Dest, LONG Len)
{
	if (Len==-1) 
		{
		while (*Source) 
			*Dest++=*Source++;
		*Dest++=0;
		return Dest;
		}
	else 
		{
		LONG ctr=0;
		
		while (ctr<Len) 
			{
			Dest[ctr]=Source[ctr];
			ctr++;
			}
		return &Dest[ctr];
		}
}

ULONG PM_String_Compare(STRPTR str1, STRPTR str2)
{
	ULONG i, j=0;

	if (!str1 || !str2)
		return 0;
	for(i=0;;i++) 
		{
		j+=str1[i];
		j-=str2[i];
		if (!str1[i] || !str2[i] || j)
			return j;
		}
}

void PM_StrCat(STRPTR str1, STRPTR str2)
{
	while (*str1) 
		str1++;
	while (*str2) 
		*str1++=*str2++;
	*str1=0;
}
