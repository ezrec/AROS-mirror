/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

/*****************************************************************************/

#include <exec/memory.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

struct FatBitMap
{
	struct BitMap	BitMap;
	LONG			Width;
	LONG			Height;
};

/*****************************************************************************/

LONG
LTP_GetDepth(struct BitMap *BitMap)
{
	if(V39)
		return((LONG)GetBitMapAttr(BitMap,BMA_DEPTH));
	else
		return(BitMap->Depth);
}

VOID
LTP_DeleteBitMap(struct BitMap *BitMap,BOOL Chip)
{
	if(V39 && !Chip)
		FreeBitMap(BitMap);
	else
	{
		if(BitMap)
		{
			struct FatBitMap *Fat;
			LONG i;

			Fat = (struct FatBitMap *)BitMap;

			if(Fat->Width && Fat->Height)
			{
				for(i = 0 ; i < BitMap->Depth ; i++)
					FreeRaster(BitMap->Planes[i],Fat->Width,Fat->Height);
			}
			else
			{
				for(i = 0 ; i < BitMap->Depth ; i++)
					FreeVec(BitMap->Planes[i]);
			}

			FreeVec(BitMap);
		}
	}
}

struct BitMap *
LTP_CreateBitMap(LONG Width,LONG Height,LONG Depth,struct BitMap *Friend,BOOL Chip)
{
	struct BitMap *BitMap;

	if(V39 && !Chip)
		BitMap = AllocBitMap(Width,Height,Depth,BMF_MINPLANES,Friend);
	else
	{
		struct FatBitMap *Fat;

		if(!(Fat = (struct FatBitMap *)AllocVec(sizeof(struct FatBitMap),MEMF_ANY | MEMF_PUBLIC | MEMF_CLEAR)))
			BitMap = NULL;
		else
		{
			LONG i;

			BitMap = (struct BitMap *)Fat;

			InitBitMap(BitMap,Depth,Width,Height);

			for(i = 0 ; i < Depth ; i++)
			{
				if(!(BitMap->Planes[i] = AllocRaster(Width,Height)))
				{
					LONG j;

					for(j = 0 ; j < i ; j++)
						FreeRaster(BitMap->Planes[j],Width,Height);

					FreeVec(Fat);

					return(NULL);
				}
			}

			Fat->Width	= Width;
			Fat->Height	= Height;

			if(Chip)
			{
				ULONG Type = MEMF_CHIP;

				for(i = 0 ; i < Depth ; i++)
					Type &= TypeOfMem(BitMap->Planes[i]);

				if(!(Type & MEMF_CHIP))
				{
					LONG PageSize;

					for(i = 0 ; i < Depth ; i++)
					{
						FreeRaster(BitMap->Planes[i],Width,Height);
						BitMap->Planes[i] = NULL;
					}

					PageSize = BitMap->BytesPerRow * BitMap->Rows;

					for(i = 0 ; i < Depth ; i++)
					{
						if(!(BitMap->Planes[i] = AllocVec(PageSize,MEMF_CHIP)))
						{
							LONG j;

							for(j = 0 ; j < i ; j++)
								FreeVec(BitMap->Planes[j]);

							FreeVec(Fat);

							return(NULL);
						}
					}

					Fat->Width = Fat->Height = 0;
				}
			}
		}
	}

	return(BitMap);
}
