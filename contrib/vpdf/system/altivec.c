
#include <proto/exec.h>
#include <proto/utility.h>
#include <exec/resident.h>
#include <exec/system.h>

int hasAltivec = 0;

#include "altivec.h"

void setupAVExceptions(void)
{
	ULONG a[ 4 ] __attribute((aligned( 16 )));
	vector unsigned short b;
	b = vec_mfvscr();
	vec_st(b, 0, (unsigned short *) a);
	a[ 3 ] |= 1 << 16;
	b = vec_ld(0, (unsigned short *)a);
	vec_mtvscr(b);
}


void AltivecInit( void )
{
	struct Resident *MyResident;

	MyResident = FindResident("MorphOS");

	/* Altivec/VMX detection */

	if (MyResident && (MyResident->rt_Version > 1 || ((MyResident->rt_Flags & RTF_EXTENDED) && MyResident->rt_Version == 1 && MyResident->rt_Revision >= 5)))
	{
		struct TagItem MyTag = {TAG_DONE, 0};
		NewGetSystemAttrsA(&hasAltivec, sizeof(hasAltivec), SYSTEMINFOTYPE_PPC_ALTIVEC, &MyTag);
	}

	if (hasAltivec)
	{
		setupAVExceptions();
	}
}

///	Altivec cache prewarming stuff

/*
 * blockcount: number of blocks (from 0 to 255, 0 = 256 blocks)
 * blocksize: number of 16-byte chunks (from 0 to 31, 0 = 32 * 16-byte chunks)
 * blockstride: 16-bit value from -32768 to 32767 (0 = 32768), denotes the address
 *   increment from the beginning of one block to the beginning of the next.
 */

static void cpu_vec_dst(CONST_APTR ptr, ULONG opts, ULONG channel)
{
	switch (channel)
	{
		case 0:
			vec_dst((VECTOR_UBYTE *)ptr, opts, 0);
			break;

		case 1:
			vec_dst((VECTOR_UBYTE *)ptr, opts, 1);
			break;

		case 2:
			vec_dst((VECTOR_UBYTE *)ptr, opts, 2);
			break;

		case 3:
			vec_dst((VECTOR_UBYTE *)ptr, opts, 3);
			break;
	}
}

static void cpu_vec_dstt(CONST_APTR ptr, ULONG opts, ULONG channel)
{
	switch (channel)
	{
		case 0:
			vec_dstt((VECTOR_UBYTE *)ptr, opts, 0);
			break;

		case 1:
			vec_dstt((VECTOR_UBYTE *)ptr, opts, 1);
			break;

		case 2:
			vec_dstt((VECTOR_UBYTE *)ptr, opts, 2);
			break;

		case 3:
			vec_dstt((VECTOR_UBYTE *)ptr, opts, 3);
			break;
	}
}


static void cpu_vec_dstst(CONST_APTR ptr, ULONG opts, ULONG channel)
{
	switch (channel)
	{
		case 0:
			vec_dstst((VECTOR_UBYTE *)ptr, opts, 0);
			break;

		case 1:
			vec_dstst((VECTOR_UBYTE *)ptr, opts, 1);
			break;

		case 2:
			vec_dstst((VECTOR_UBYTE *)ptr, opts, 2);
			break;

		case 3:
			vec_dstst((VECTOR_UBYTE *)ptr, opts, 3);
			break;
	}
}


static void cpu_vec_dststt(CONST_APTR ptr, ULONG opts, ULONG channel)
{
	switch (channel)
	{
		case 0:
			vec_dststt((VECTOR_UBYTE *)ptr, opts, 0);
			break;

		case 1:
			vec_dststt((VECTOR_UBYTE *)ptr, opts, 1);
			break;

		case 2:
			vec_dststt((VECTOR_UBYTE *)ptr, opts, 2);
			break;

		case 3:
			vec_dststt((VECTOR_UBYTE *)ptr, opts, 3);
			break;
	}
}


static void cpu_vec_dss(ULONG channel)
{
	switch (channel)
	{
		case 0:
			vec_dss(0);
			break;

		case 1:
			vec_dss(1);
			break;

		case 2:
			vec_dss(2);
			break;

		case 3:
			vec_dss(3);
			break;
	}
}

void cpu_cache_stream_start(CONST_APTR ptr, ULONG blockcount, ULONG blocksize, LONG stride, ULONG flags)
{
	if (hasAltivec)
	{
		ULONG opts;
		ULONG channel = 0;

		opts = (blocksize << 24) | (blockcount << 16) | stride;

		if (flags & CCSSF_CHANNEL1)
		{
			channel = 1;
		}
		else if (flags & CCSSF_CHANNEL2)
		{
			channel = 2;
		}
		else if (flags & CCSSF_CHANNEL3)
		{
			channel = 3;
		}

		if (flags & CCSSF_WRITES)
		{
			if (flags & CCSSF_TRANSCIENT)
			{
				cpu_vec_dststt(ptr, opts, channel);
			}
			else
			{
				cpu_vec_dstst(ptr, opts, channel);
			}
		}
		else
		{
			if (flags & CCSSF_TRANSCIENT)
			{
				cpu_vec_dstt(ptr, opts, channel);
			}
			else
			{
				cpu_vec_dst(ptr, opts, channel);
			}
		}
	}
	else
	{
		//kprintf("[Altivec]: Not available\n");
	}
	/* XXX: fallback to dtst etc.. ? */
}


void cpu_cache_stream_stop(ULONG channel)
{
	if (hasAltivec)
	{
		cpu_vec_dss(channel);
	}
}

/*
 * Function using simple heuristics to determine caching.
 */

void cpu_cache_image_start(void *ptr, int rowbytes, int width, int lines)
{
	/* blocksize 0...31, 0 == 32 */
	int blocksize;
	int blocks = 256;

	if (!hasAltivec)
		return;

	if (lines == 1)
	{
		blocksize = width / (blocks * 16); /* size of block in 16byte units */
		if (blocksize == 0)
			blocksize = 1;
		if (blocksize > 31)
			blocksize = 0;
	}
	else
	{
		blocksize = (rowbytes * lines) / (blocks * 16);
		if (blocksize == 0)
			blocksize = 1;
		if (blocksize > 31)
			blocksize = 0;
	}

	if (blocks > 255)
		blocks = 0;

	cpu_cache_stream_start((void*)((unsigned int)ptr & ~0xf), blocks, blocksize, blocksize * 16, CCSSF_CHANNEL1);
}

void cpu_cache_image_stop(void)
{
	cpu_cache_stream_stop(1);
}

////
