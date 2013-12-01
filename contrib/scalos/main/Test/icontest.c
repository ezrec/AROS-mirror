// icontest.c
// 29 Feb 2004 11:01:39


#include <exec/types.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <intuition/classusr.h>
#include <clib/exec_protos.h>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include <proto/iconobject.h>
#include <clib/iconobject_protos.h>
#include <scalos/iconobject.h>
#include <scalos/scalos.h>


#define	TEST_LOOP	50

struct Library *IconobjectBase = NULL;


main(int argc, char *argv[])
{
	ULONG n;
	ULONG FastMem[TEST_LOOP];
	ULONG ChipMem[TEST_LOOP];
	LONG FastMemDiffMin = LONG_MAX, FastMemDiffMax = LONG_MIN, FastMemDiffAvg = 0;
	LONG ChipMemDiffMin = LONG_MAX, ChipMemDiffMax = LONG_MIN, ChipMemDiffAvg = 0;

	IconobjectBase = OpenLibrary(ICONOBJECTNAME, 39);

	if (NULL == IconobjectBase)
		{
		fprintf(stderr, "failed to open iconobject.library\n");
		return 10;
		}

	printf("Running NewIconObject/DisposeIconObject %ld times\n\n", TEST_LOOP);

	for (n = 0; n < TEST_LOOP; n++)
		{
		Object *IconObj;

		IconObj = NewIconObjectTags(argv[1], TAG_END);
		if (NULL == IconObj)
			break;

		DisposeIconObject(IconObj);

		FastMem[n] = AvailMem(MEMF_FAST);
		ChipMem[n] = AvailMem(MEMF_CHIP);
		}

	for (n = 1; n < TEST_LOOP; n++)
		{
		LONG ChipDelta = ChipMem[n] - ChipMem[n-1];
		LONG FastDelta = FastMem[n] - FastMem[n-1];

		FastMemDiffAvg += FastDelta;
		ChipMemDiffAvg += ChipDelta;

		if (FastDelta < FastMemDiffMin)
			FastMemDiffMin = FastDelta;
		if (ChipDelta < ChipMemDiffMin)
			ChipMemDiffMin = ChipDelta;
		if (FastDelta > FastMemDiffMax)
			FastMemDiffMax = FastDelta;
		if (ChipDelta > ChipMemDiffMax)
			ChipMemDiffMax = ChipDelta;

		printf("%2ld:  FastDelta=%-10ld  ChipDelta=%-10ld\n", 
			n, FastDelta, ChipDelta);
		}

	FastMemDiffAvg /= TEST_LOOP;
	ChipMemDiffAvg /= TEST_LOOP;

	printf("\n");
	printf("FastMem delta   Min=%-8ld   Avg=%-8ld  Max=%-8ld\n", FastMemDiffMin, FastMemDiffAvg, FastMemDiffMax);
	printf("ChipMem delta   Min=%-8ld   Avg=%-8ld  Max=%-8ld\n", ChipMemDiffMin, ChipMemDiffAvg, ChipMemDiffMax);

	CloseLibrary(IconobjectBase);

	printf("Press any key to exit...\n");
	getchar();
}

