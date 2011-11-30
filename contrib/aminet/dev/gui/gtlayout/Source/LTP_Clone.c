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

#include "Assert.h"

#ifdef DO_CLONING	/* Support code */

VOID
LTP_CloneScreen(struct LayoutHandle *Handle,LONG Width,LONG Height)
{
	struct ColorSpec	*ColourSpec;
	struct ColourRecord	*ColourRecord;
	struct CloneExtra	*Extra;
	LONG				 AllocationSize = 0;	/* For the sake of the compiler, make sure this is initialized */
	LONG				 i,j,NumColours;
	UWORD				*Pens;
	struct Screen		*Screen;
	ULONG				 DisplayID;
	UWORD				 EndPen;

	ColourSpec		= NULL;
	ColourRecord	= NULL;
	Extra			= Handle->CloneExtra;
	Pens			= Handle->CloneExtra->ScreenPens;
	DisplayID		= GetVPModeID(&Handle->Screen->ViewPort);
	EndPen			= (UWORD)~0;

	if(Handle->ExactClone)
		NumColours = Handle->Screen->ViewPort.ColorMap->Count;
	else
	{
		NumColours = Extra->TotalPens;

		DisplayID &= ~(HAM | EXTRA_HALFBRITE);
	}

	Height += Handle->Screen->BarHeight + 1;

	if(Width < Extra->MinWidth)
		Width = Extra->MinWidth;

	if(Height < Extra->MinHeight)
		Height = Extra->MinHeight;

	if(Handle->SimpleClone)
		Pens = &EndPen;
	else
	{
		if(V39)
		{
			AllocationSize = sizeof(struct ColourRecord) + sizeof(struct ColourTriplet) * NumColours;

			if(!(ColourRecord = (struct ColourRecord *)LTP_Alloc(Handle,AllocationSize)))
			{
				DB(kprintf("no colours\n"));

				return;
			}
			else
			{
				ColourRecord->NumColours = NumColours;

				if(Handle->ExactClone)
					GetRGB32(Handle->Screen->ViewPort.ColorMap,0,NumColours,(ULONG *)ColourRecord->Triplets);
				else
				{
					for(i = 0 ; i < NumColours ; i++)
						GetRGB32(Handle->Screen->ViewPort.ColorMap,Extra->Pens[i],1,(ULONG *)&ColourRecord->Triplets[i]);
				}
			}
		}
		else
		{
			AllocationSize = sizeof(struct ColorSpec) * (NumColours + 1);

			if(!(ColourSpec = (struct ColorSpec *)LTP_Alloc(Handle,AllocationSize)))
			{
				DB(kprintf("no colours either\n"));

				return;
			}
			else
			{
				LONG RGB,Which;

				for(i = 0 ; i < NumColours ; i++)
				{
					if(Handle->ExactClone)
						Which = i;
					else
						Which = Extra->Pens[i];

					RGB = GetRGB4(Handle->Screen->ViewPort.ColorMap,Which);

					ColourSpec[i].ColorIndex	= i;
					ColourSpec[i].Red			= (RGB >> 8) & 0xF;
					ColourSpec[i].Green			= (RGB >> 4) & 0xF;
					ColourSpec[i].Blue			= (RGB	   ) & 0xF;
				}

				ColourSpec[i].ColorIndex = -1;
			}
		}

		if(Handle->ExactClone)
		{
			for(i = 0 ; i < Handle->DrawInfo->dri_NumPens ; i++)
				Pens[i] = Extra->Pens[i];
		}
		else
		{
			for(i = 0 ; i < Handle->DrawInfo->dri_NumPens ; i++)
			{
				for(j = 0 ; j < NumColours ; j++)
				{
					if(Extra->Pens[j] == Handle->DrawInfo->dri_Pens[i])
					{
						Pens[i] = j;

						break;
					}
				}
			}
		}

		Pens[i] = (UWORD)~0;
	}

	DB(kprintf("Calling openscreen\n"));

	if((Screen = OpenScreenTags(NULL,
		SA_Width,		Width,
		SA_Height,		Height,
		SA_Overscan,	OSCAN_TEXT,
		SA_AutoScroll,	TRUE,
		SA_DisplayID,	DisplayID,
		SA_Behind,		TRUE,
		SA_Depth,		Handle->SimpleClone ? 2 : Extra->Depth,
		SA_Colors,		ColourSpec,
		SA_Colors32,	ColourRecord,
		SA_Pens,		Pens,
		SA_Font,		Handle->TextAttr,
		SA_Title,		Handle->CloneScreenTitle ? Handle->CloneScreenTitle : Handle->Screen->DefaultTitle,
		SA_SharePens,	TRUE,
		SA_Interleaved,	TRUE,

		Handle->SimpleClone ? TAG_IGNORE : SA_BackFill,		&Handle->BackfillHook,
		Handle->SimpleClone ? TAG_IGNORE : SA_BlockPen,		Pens[SHADOWPEN],
		Handle->SimpleClone ? TAG_IGNORE : SA_DetailPen,	Pens[BACKGROUNDPEN],
	TAG_DONE)))
	{
		struct DrawInfo *DrawInfo;

		DB(kprintf("getting drawinfo\n"));

		if((DrawInfo = GetScreenDrawInfo(Screen)))
		{
			APTR VisualInfo;

			DB(kprintf("getting visualinfo\n"));

			if((VisualInfo = GetVisualInfoA(Screen,NULL)))
			{
				UnlockPubScreen(NULL,Handle->PubScreen);

				FreeScreenDrawInfo(Handle->Screen,Handle->DrawInfo);

				FreeVisualInfo(Handle->VisualInfo);

				Pens = DrawInfo->dri_Pens;

				Handle->PubScreen		= NULL;
				Handle->Screen			= Screen;
				Handle->DrawInfo		= DrawInfo;
				Handle->VisualInfo		= VisualInfo;
				Handle->TextPen			= Pens[TEXTPEN];
				Handle->BackgroundPen	= Pens[BACKGROUNDPEN];
				Handle->ShinePen		= Pens[SHINEPEN];
				Handle->ShadowPen		= Pens[SHADOWPEN];
				Handle->AspectX			= DrawInfo->dri_Resolution.X;
				Handle->AspectY			= DrawInfo->dri_Resolution.Y;

				if(Handle->SimpleClone)
					Handle->MaxPen = 0;
				else
					Handle->MaxPen = Extra->TotalPens - 1;

				Extra->Screen = Screen;

				LTP_Free(Handle,ColourRecord,AllocationSize);
				LTP_Free(Handle,ColourSpec,AllocationSize);

				DB(kprintf("fertig\n"));

				return;
			}

			FreeScreenDrawInfo(Screen,DrawInfo);
		}

		CloseScreen(Screen);
	}

	LTP_Free(Handle,ColourRecord,AllocationSize);
	LTP_Free(Handle,ColourSpec,AllocationSize);

	Handle->Failed = TRUE;

	DB(kprintf("fehlschlag\n"));
}

BOOL
LTP_PrepareCloning(struct LayoutHandle *Handle)
{
	DB(kprintf("prepare cloning\n"));

	if(Handle->CloneExtra)
		return(TRUE);
	else
	{
		struct DimensionInfo DimensionInfo;

		DB(kprintf("getting dimensions\n"));

		if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,GetVPModeID(&Handle->Screen->ViewPort)))
		{
			LONG AllocationSize;

			DB(kprintf("getting cloneextra\n"));

			if((Handle->CloneExtra = (struct CloneExtra *)LTP_Alloc(Handle,AllocationSize = sizeof(struct CloneExtra) + (sizeof(LONG) + sizeof(UWORD)) * Handle->DrawInfo->dri_NumPens + sizeof(UWORD))))
			{
				LONG	 i,j,TotalPens = 0;
				LONG	*Pens;
				BOOL	 NotFound;

				Handle->CloneExtra->Pens			= (LONG *)(Handle->CloneExtra + 1);
				Handle->CloneExtra->ScreenPens		= (UWORD *)(&Handle->CloneExtra->Pens[Handle->DrawInfo->dri_NumPens]);
				Handle->CloneExtra->MinWidth		= DimensionInfo.MinRasterWidth;
				Handle->CloneExtra->MinHeight		= DimensionInfo.MinRasterHeight;
				Handle->CloneExtra->MaxWidth		= DimensionInfo.MaxRasterWidth;
				Handle->CloneExtra->MaxHeight		= DimensionInfo.MaxRasterHeight;

				Handle->CloneExtra->Bounds.Left		= 0;
				Handle->CloneExtra->Bounds.Top		= 0;
				Handle->CloneExtra->Bounds.Width	= DimensionInfo.MaxRasterWidth;
				Handle->CloneExtra->Bounds.Height	= DimensionInfo.MaxRasterHeight;

				Pens = Handle->CloneExtra->Pens;

				if(Handle->ExactClone)
				{
					for(i = 0 ; i < Handle->DrawInfo->dri_NumPens ; i++)
						Pens[i] = Handle->DrawInfo->dri_Pens[i];

					TotalPens = Handle->DrawInfo->dri_NumPens;

					Handle->CloneExtra->Depth = Handle->DrawInfo->dri_Depth;
				}
				else
				{
					for(i = 0 ; i < Handle->DrawInfo->dri_NumPens ; i++)
					{
						for(j = 0,NotFound = TRUE ; NotFound && j < TotalPens ; j++)
						{
							if(Pens[j] == Handle->DrawInfo->dri_Pens[i])
								NotFound = FALSE;
						}

						if(NotFound)
							Pens[TotalPens++] = Handle->DrawInfo->dri_Pens[i];
					}

					for(i = 0 ; i < DimensionInfo.MaxDepth ; i++)
					{
						if(TotalPens <= (1L << i))
						{
							Handle->CloneExtra->Depth = i;

							break;
						}
					}
				}

				DB(kprintf("totalpens=%ld depth=%ld\n",TotalPens,Handle->CloneExtra->Depth));

				if((Handle->CloneExtra->TotalPens = TotalPens) && Handle->CloneExtra->Depth)
				{
					DB(kprintf("setting up glyphs\n"));

					if(LTP_GlyphSetup(Handle,Handle->InitialTextAttr))
					{
						Handle->Failed = Handle->Rescaled = FALSE;

						LTP_ResetGroups(Handle->TopGroup);

						DB(kprintf("fertig\n\n"));

						return(TRUE);
					}
				}
				else
				{
					LTP_Free(Handle,Handle->CloneExtra,AllocationSize);

					Handle->CloneExtra = NULL;
				}
			}
		}
	}

	DB(kprintf("fehlschlag\n\n"));

	return(FALSE);
}

#endif	/* DO_CLONING */
