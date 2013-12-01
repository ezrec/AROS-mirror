// pngdt.c
// $Date$
// $Revision$

#ifdef __amigaos4__
#define __USE_INLINE__
#endif // __amigaos4__

#include <stdlib.h>
#include <stdio.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <utility/tagitem.h>
#include <datatypes/pictureclass.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define __USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/datatypes.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include <clib/alib_protos.h>

#if !defined(PDTA_AlphaChannel)
#define PDTA_AlphaChannel	(DTA_Dummy + 256)	/* Alphachannel input */
#endif //!defined(PDTA_AlphaChannel)


struct Library *DataTypesBase;

#ifdef __amigaos4__
//struct Library *NewlibBase;
//struct Interface *INewlib;
struct GraphicsIFace *IGraphics;
struct IntuitionIFace *IIntuition;
struct DataTypesIFace *IDataTypes;

struct Library *IntuitionBase;
struct Library *GfxBase;
#else // __amigaos4__
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
#endif // __amigaos4__


int main(int argc, char *argv[])
{
	Object *ImageObj = NULL;
	ULONG *ImgData = NULL;

	do	{
		struct BitMap *bm = NULL;
		struct BitMap *cbm = NULL;
		struct BitMapHeader *bmhd = NULL;
		ULONG Result;
		ULONG alpha = 0;

		DataTypesBase = OpenLibrary("datatypes.library", 39);
		if (NULL == DataTypesBase)
			break;
		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
		if (NULL == IntuitionBase)
			break;
		GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 39);
		if (NULL == GfxBase)
			break;

#ifdef __amigaos4__
		IIntuition = (struct IntuitionIFace *) GetInterface((struct Library *) IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			break;
		IGraphics = (struct GraphicsIFace *) GetInterface((struct Library *) GfxBase, "main", 1, NULL);
		if (NULL == IGraphics)
			break;
		IDataTypes = (struct DataTypesIFace *) GetInterface((struct Library *) DataTypesBase, "main", 1, NULL);
		if (NULL == IDataTypes)
			break;
#endif // __amigaos4__

		ImageObj = (Object *) NewDTObject(argv[1],
				DTA_SourceType, DTST_FILE,
				DTA_GroupID, GID_PICTURE,
				PDTA_DestMode, PMODE_V43,
				TAG_END);

		if (NULL == ImageObj)
		{
			fprintf(stderr, "Failed to create Image Object <%s>\n", argv[1]);
			break;
		}

		printf("ImageObj created OK.\n");

		SetAttrs(ImageObj,
				PDTA_Remap, FALSE,
				PDTA_UseFriendBitMap, FALSE,
				PDTA_AlphaChannel, TRUE,
				TAG_END);

		// DoDTMethodA()
		if (!DoDTMethod(ImageObj, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE))
			{
			printf("DTM_PROCLAYOUT failed\n");
			break;
			}

		Result = GetDTAttrs(ImageObj,
			PDTA_BitMap, (ULONG) &bm,
			PDTA_ClassBitMap, (ULONG) &cbm,
			PDTA_BitMapHeader, (ULONG) &bmhd,
			TAG_END);

		printf("Result=%ld  bm=%08lx  cbm=%08lx  bmhd=%08lx\n", Result, bm, cbm, bmhd);
		if (Result < 2)
			break;

		if (NULL == bm || NULL == bmhd)
			break;

		printf("Width=%ld  Height=%ld  Depth=%ld  Masking=%ld\n",
			bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth, bmhd->bmh_Masking);

		Result = GetDTAttrs(ImageObj,
			PDTA_AlphaChannel, &alpha,
			TAG_END);
		printf("Result=%ld  PDTA_AlphaChannel=%ld\n", Result, alpha);

		printf("BitMap Width=%ld  Height=%ld  Depth=%ld\n",
			GetBitMapAttr(bm, BMA_WIDTH), GetBitMapAttr(bm, BMA_HEIGHT),
			GetBitMapAttr(bm, BMA_DEPTH));

		printf("ClassBitMap Width=%ld  Height=%ld  Depth=%ld\n",
			GetBitMapAttr(cbm, BMA_WIDTH), GetBitMapAttr(cbm, BMA_HEIGHT),
			GetBitMapAttr(cbm, BMA_DEPTH));

		ImgData = calloc(bmhd->bmh_Width * bmhd->bmh_Height, sizeof(ULONG));
		printf("Allocated ImgData=%08lx\n", ImgData);
		if (NULL == ImgData)
			break;

		DoMethod(ImageObj,
			PDTM_READPIXELARRAY,
			ImgData,
			PBPAFMT_ARGB,
			bmhd->bmh_Width * sizeof(ULONG),
			0,
			0,
			bmhd->bmh_Width,
			bmhd->bmh_Height
			);

		{
		ULONG x;
		ULONG y = bmhd->bmh_Height / 2;
		ULONG *up = (ULONG *) ImgData;

		up += y * bmhd->bmh_Width;
		for (x = 0; x < bmhd->bmh_Width; x++)
			{
			if (*up)
				printf("x=%-4ld  ARGB=%08lx\n", x, *up);
			up++;
			}
		}

		} while (0);

	if (ImgData)
		free(ImgData);

#ifdef __amigaos4__
	if (IDataTypes)
		{
		DropInterface((struct Interface *)IDataTypes);
		IDataTypes = NULL;
		}
	if (IGraphics)
		{
		DropInterface((struct Interface *)IGraphics);
		IGraphics = NULL;
		}
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif // __amigaos4__

	if (GfxBase)
		CloseLibrary((struct Library *) GfxBase);
	if (IntuitionBase)
		CloseLibrary((struct Library *) IntuitionBase);
	if (DataTypesBase)
		CloseLibrary(DataTypesBase);
}

