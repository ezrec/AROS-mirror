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

#include <stdlib.h>

/*****************************************************************************/

#include "Assert.h"

/****************************************************************************/

static int compare(struct Gadget **a,struct Gadget **b)
{
	return(((int)a[0]->GadgetID) - ((int)b[0]->GadgetID));
}

/****************************************************************************/

VOID
LTP_CreateGadgets(LayoutHandle *Handle,struct IBox *Bounds,LONG Left,LONG Top,LONG PlusWidth,LONG PlusHeight)
{
	if(!Handle->Failed)
	{
		LONG	OldInterWidth	= Handle->InterWidth,
				OldInterHeight	= Handle->InterHeight;

		LONG	Attempt = 3,ScaleAttempt = 6;

		BOOL	Fits;
		BOOL	Cloned = FALSE;

			// If the window is already open, do with it

		#ifdef DO_CLONING
		{
			if(Handle->Window)
				Handle->CloningPermitted = FALSE;
		}
		#endif

		do
		{
			LTP_LayoutGroup(Handle,Handle->TopGroup);

			LTP_PlaceGroups(Handle,Handle->TopGroup,Left,Top);

			Fits = TRUE;

			DB(kprintf("TopGroup->Width %ld + PlusWidth %ld = %ld > Bounds->Width %ld\n",Handle->TopGroup->Width,PlusWidth,Handle->TopGroup->Width + PlusWidth,Bounds->Width));

			if(!Handle->Window)
			{
				if(Handle->TopGroup->Width + PlusWidth > Bounds->Width)
				{
					DB(kprintf("%ld > Screen->Width %ld\n",Handle->TopGroup->Width + PlusWidth,Handle->Screen->Width));

					if(Handle->TopGroup->Width + PlusWidth > Handle->Screen->Width)
					{
						Fits = FALSE;

						DB(kprintf("TopGroup->Height %ld + PlusHeight %ld = %ld > Bounds->Height %ld\n",Handle->TopGroup->Height,PlusHeight,Handle->TopGroup->Height + PlusHeight,Bounds->Height));

						if(Handle->TopGroup->Height + PlusHeight > Bounds->Height)
						{
							DB(kprintf("rescale TRUE,TRUE\n"));

							LTP_Rescale(Handle,TRUE,TRUE);
						}
						else
						{
							DB(kprintf("rescale TRUE,FALSE\n"));

							LTP_Rescale(Handle,TRUE,FALSE);
						}

						ScaleAttempt--;

						Left			= Left			- OldInterWidth		+ Handle->InterWidth;
						Top				= Top			- OldInterHeight	+ Handle->InterHeight;
						PlusWidth		= PlusWidth		- OldInterWidth		+ Handle->InterWidth;
						PlusHeight		= PlusHeight	- OldInterHeight	+ Handle->InterHeight;
						OldInterWidth	= Handle->InterWidth;
						OldInterHeight	= Handle->InterHeight;
					}
				}
				else
				{
					DB(kprintf("TopGroup->Height %ld + PlusHeight %ld = %ld > Bounds->Height %ld\n",Handle->TopGroup->Height,PlusHeight,Handle->TopGroup->Height + PlusHeight,Bounds->Height));

					if(Handle->TopGroup->Height + PlusHeight > Bounds->Height)
					{
						DB(kprintf("TopGroup->Height %ld + PlusHeight %ld = %ld > Screen->Height %ld\n",Handle->TopGroup->Height,PlusHeight,Handle->TopGroup->Height + PlusHeight,Handle->Screen->Height));

						if(Handle->TopGroup->Height + PlusHeight > Handle->Screen->Height)
						{
							DB(kprintf("rescale FALSE,TRUE\n"));
							Fits = FALSE;

							LTP_Rescale(Handle,FALSE,TRUE);

							Left			= Left			- OldInterWidth		+ Handle->InterWidth;
							Top				= Top			- OldInterHeight	+ Handle->InterHeight;
							PlusWidth		= PlusWidth		- OldInterWidth		+ Handle->InterWidth;
							PlusHeight		= PlusHeight	- OldInterHeight	+ Handle->InterHeight;
							OldInterWidth	= Handle->InterWidth;
							OldInterHeight	= Handle->InterHeight;

							ScaleAttempt--;
						}
					}
				}
			}

			DB(kprintf("ok\n"));

			#ifdef DO_CLONING
			{
				DB(kprintf("1) failed %ld cloned %ld cloningpermitted %ld\n",Handle->Failed,Cloned,Handle->CloningPermitted));

				if(Handle->Failed && !Cloned && Handle->CloningPermitted)
				{
					if(LTP_PrepareCloning(Handle))
					{
						Bounds = &Handle->CloneExtra->Bounds;

						Cloned = TRUE;
					}
				}
			}
			#endif

			if(Attempt > 0 && Fits && !Handle->Rescaled && !Cloned && !Handle->Failed && (Handle->GrowView || (Handle->ResizeObject && Handle->Window)))
			{
				LONG			Width	= Handle->TopGroup->Width	+ PlusWidth;
				LONG			Height	= Handle->TopGroup->Height	+ PlusHeight;
				ObjectNode *	View;

				Attempt--;

				if(Handle->GrowView)
					View = Handle->GrowView;
				else
					View = Handle->ResizeObject;

 				if(Bounds->Width != Width || Bounds->Height != Height)
				{
					LONG PlusChars			= 0;
					LONG PlusLines			= 0;
					LONG AvailableWidth		= Bounds->Width		- Width;
					LONG AvailableHeight	= Bounds->Height	- Height;
					LONG GlyphWidth;
					LONG GlyphHeight;

					/* Now that Handle->ResizeObject can be either a LISTVIEW_KIND or
					 * a FRAME_KIND we have to watch closely which data we access.
					 */
					if((View->Type == LISTVIEW_KIND) && (View->Special.List.TextAttr != NULL))
					{
						GlyphWidth	= View->Special.List.FixedGlyphWidth;
						GlyphHeight	= View->Special.List.FixedGlyphHeight;
					}
					else
					{
						GlyphWidth	= Handle->GlyphWidth;
						GlyphHeight	= Handle->GlyphHeight;
					}

					if(Handle->GrowView != NULL)
					{
						if(View->Special.List.MaxGrowX > 0)
						{
							PlusChars = AvailableWidth / GlyphWidth;

							if(PlusChars > 0 && View->Chars + PlusChars > View->Special.List.MaxGrowX)
								PlusChars = View->Special.List.MaxGrowX - View->Chars;

							if(PlusChars < 0)
								PlusChars = 0;
						}
					}
					else
					{
						BOOLEAN CanResize;

						if(View->Type == LISTVIEW_KIND)
							CanResize = View->Special.List.ResizeX;
						else
							CanResize = View->Special.Frame.ResizeX;

						if(CanResize)
						{
							if(AvailableWidth < 0)
								PlusChars = (AvailableWidth - (GlyphWidth + 1)) / GlyphWidth;
							else
								PlusChars = AvailableWidth / GlyphWidth;
						}
					}

					if(Handle->GrowView != NULL)
					{
						if(View->Special.List.MaxGrowY > 0)
						{
							PlusLines = AvailableHeight / GlyphHeight;

							if(PlusLines > 0 && View->Lines + PlusLines > View->Special.List.MaxGrowY)
								PlusLines = View->Special.List.MaxGrowY - View->Lines;

							if(PlusLines < 0)
								PlusLines = 0;
						}
					}
					else
					{
						BOOLEAN CanResize;

						if(View->Type == LISTVIEW_KIND)
							CanResize = View->Special.List.ResizeY;
						else
							CanResize = View->Special.Frame.ResizeY;

						if(CanResize)
						{
							if(AvailableHeight < 0)
								PlusLines = (AvailableHeight - (GlyphHeight + 1)) / GlyphHeight;
							else
								PlusLines = AvailableHeight / GlyphHeight;
						}
					}

					if(PlusChars != 0 || PlusLines != 0)
					{
						View->Chars += PlusChars;
						View->Lines += PlusLines;

						if(View->Type == FRAME_KIND)
						{
							View->Special.Frame.PlusWidth += PlusChars;
							if(View->Special.Frame.PlusWidth < 0)
								View->Special.Frame.PlusWidth = 0;

							View->Special.Frame.PlusHeight += PlusLines;
							if(View->Special.Frame.PlusHeight < 0)
								View->Special.Frame.PlusHeight = 0;
						}

						LTP_ResetGroups(Handle->TopGroup);

						Fits = FALSE;
					}
				}

				Handle->GrowView = NULL;
			}

			DB(kprintf("fits %ld handle->failed %ld attempt %ld\n",Fits,Handle->Failed,Attempt));
		}
		while(!Fits && !Handle->Failed && ScaleAttempt > 0);

		#ifdef DO_CLONING
		{
			if(!Handle->Failed && !Cloned && Handle->CloningPermitted && !Fits && ScaleAttempt <= 0)
			{
				if(LTP_PrepareCloning(Handle))
				{
					LTP_LayoutGroup(Handle,Handle->TopGroup);
					LTP_PlaceGroups(Handle,Handle->TopGroup,Left,Top);

					Cloned = TRUE;
				}
			}

			DB(kprintf("2) failed %ld cloned %ld\n",Handle->Failed,Cloned));

			if(!Handle->Failed && Cloned)
				LTP_CloneScreen(Handle,Handle->TopGroup->Width + PlusWidth,Handle->TopGroup->Height + PlusHeight);
		}
		#endif

		if(!Handle->Failed)
		{
			if(Handle->GadgetArray = LTP_Alloc(Handle,sizeof(struct Gadget *) * Handle->Count))
			{
				#ifdef DO_PICKSHORTCUTS
				{
					if(!Handle->NoKeys)
					{
						LTP_SearchKeys(Handle,Handle->TopGroup);
						LTP_SelectKeys(Handle,Handle->TopGroup);
					}
				}
				#endif

				Handle->Index = 0;

				LTP_LayoutGadgets(Handle,Handle->TopGroup,Left,Top,PlusWidth,PlusHeight);

				if(Handle->Failed)
					LTP_DisposeGadgets(Handle);
				else
					qsort(Handle->GadgetArray,Handle->Index,sizeof(struct Gadget *),(int (*)(const void *, const void *))compare);
			}
		}
	}
}
