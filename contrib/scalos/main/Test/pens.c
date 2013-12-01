// pens.c
// 17 Nov 2002 15:12:13


#include <exec/types.h>
#include <intuition/screens.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <stdlib.h>
#include <stdio.h>


static CONST_STRPTR driPenNames[] =
	{
	"DETAILPEN",
	"BLOCKPEN",
	"TEXTPEN",
	"SHINEPEN",
	"SHADOWPEN",
	"FILLPEN",
	"FILLTEXTPEN",
	"BACKGROUNDPEN",
	"HIGHLIGHTTEXTPEN",
	"BARDETAILPEN",
	"BARBLOCKPEN",
	"BARTRIMPEN",
	};

#define	Sizeof(array)	(sizeof(array)/sizeof(array[0]))

int main(int argc, char *argv[])
{
	struct Screen *pubScreen = NULL;
	struct DrawInfo *drawInfo = NULL;
	CONST_STRPTR PubScreenName = NULL;

	if (2 == argc)
		PubScreenName = argv[1];

	do	{
		short n;

		pubScreen = LockPubScreen(PubScreenName);
		if (NULL == pubScreen)
			{
			fprintf(stderr, "Failed to lock public screen.\n");
			break;
			}

		drawInfo = GetScreenDrawInfo(pubScreen);
		if (NULL == drawInfo)
			{
			fprintf(stderr, "GetScreenDrawInfo() failed.\n");
			break;
			}

		printf("DrawInfo  Version=%ld\nnumber of Pens = %ld\n", 
			drawInfo->dri_Version, drawInfo->dri_NumPens);

		for (n=0; n<drawInfo->dri_NumPens; n++)
			{
			ULONG rgb[4];

			if (n < Sizeof(driPenNames))
				printf("Pen %2ld <%20s>  = %3ld", n, driPenNames[n], drawInfo->dri_Pens[n]);
			else
				printf("Pen %2ld = %3ld", n, drawInfo->dri_Pens[n]);

			GetRGB32(pubScreen->ViewPort.ColorMap, drawInfo->dri_Pens[n], 1, rgb);

			printf("   RGB = %08lx/%08lx/%08lx\n", rgb[0], rgb[1], rgb[2]);
			}
		} while (0);

	if (drawInfo)
		FreeScreenDrawInfo(pubScreen, drawInfo);
	if (pubScreen)
		UnlockPubScreen(NULL, pubScreen);
}

