/*********************************************************************
----------------------------------------------------------------------

	guigfx_colorhandle

----------------------------------------------------------------------
*********************************************************************/

#include <render/render.h>
#include <libraries/cybergraphics.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <graphics/view.h>

#include <proto/render.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>

#include <guigfx/guigfx.h>

#include "guigfx_global.h"


/*--------------------------------------------------------------------

		sharedhistogram = CreateSharedHistogram(APTR histo)

		installiert ein shared histogram mit
		Nutzungszähler und Semaphore.

--------------------------------------------------------------------*/

struct SharedHistogram *CreateSharedHistogram(APTR histo)
{
	struct SharedHistogram *sh;

	if ((sh = AllocRenderVec(MemHandler, sizeof(struct SharedHistogram))))
	{
		InitSemaphore(&sh->semaphore);
		sh->count = 1;
		sh->histogram = histo;
	}

	return sh;
}

/*--------------------------------------------------------------------

		LinkToSharedHistogram(sharedhistogram)

		erzeugt einen link zu einem shared histogram

--------------------------------------------------------------------*/

void LinkToSharedHistogram(struct SharedHistogram *histo)
{
	ObtainSemaphore(&histo->semaphore);

	histo->count++;

	ReleaseSemaphore(&histo->semaphore);
}

/*--------------------------------------------------------------------

		UnLinkSharedHistogram(sharedhistogram)

		löst eine Verbindung zu einem sharehistogram

--------------------------------------------------------------------*/

void UnLinkSharedHistogram(struct SharedHistogram *histo)
{
	ObtainSemaphore(&histo->semaphore);

	if (--(histo->count) == 0)
	{
		DeleteHistogram(histo->histogram);
		ReleaseSemaphore(&histo->semaphore);
		FreeRenderVec(histo);					
	}
	else
	{
		ReleaseSemaphore(&histo->semaphore);
	}
}

/*--------------------------------------------------------------------

		colorhandle = CreateColorHandle(psm, tags)

		erzeugt ein ColorHandle.	
		numpixels, picture bzw. histogram müssen
		noch eingetragen werden.

--------------------------------------------------------------------*/

COLORHANDLE *CreateColorHandle(PSM *psm, TAGLIST tags)
{
	COLORHANDLE *ch;

	if ((ch = AllocRenderVecClear(MemHandler, sizeof(COLORHANDLE))))
	{
		ch->psm = psm;		
		ch->weight = GetTagData(GGFX_Weight, 1, tags);

		return ch;
	}

	return NULL;
}

/*--------------------------------------------------------------------

		DeleteColorHandle(psm, tags)

		löscht ein ColorHandle
	
--------------------------------------------------------------------*/

void DeleteColorHandle(COLORHANDLE *ch)
{
	if(ch->histogram)
	{
		UnLinkSharedHistogram(ch->histogram);
	}
	
	FreeRenderVec(ch);
}

/*--------------------------------------------------------------------

		UnlinkColorHandle(ch)

		entfernt ein Colorhandle aus seiner PenShareMap
	
--------------------------------------------------------------------*/

void UnlinkColorHandle(COLORHANDLE *ch)
{
	if(ch->psm)
	{
		ObtainSemaphore(&ch->psm->semaphore);
		Remove(&ch->node);
		ch->psm->numcolorhandles--;
		ch->psm->modified = TRUE;
		ReleaseSemaphore(&ch->psm->semaphore);
	}
}

/*--------------------------------------------------------------------

		LinkColorHandle(ch)

		linkt ein Colorhandle an seine PenShareMap
	
--------------------------------------------------------------------*/

void LinkColorHandle(COLORHANDLE *ch)
{
	ObtainSemaphore(&ch->psm->semaphore);
	AddTail(&ch->psm->colorlist,&ch->node);
	ch->psm->numcolorhandles++;
	ch->psm->modified = TRUE;
	ReleaseSemaphore(&ch->psm->semaphore);
}


/*********************************************************************
----------------------------------------------------------------------

		RemColorHandle(ch)

		entfernt ein Colorhandle aus seiner Pensharemap
		und löscht es.

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM RemColorHandle(REG(a0) COLORHANDLE *ch)
{
	if (ch)
	{
		UnlinkColorHandle(ch);
		DeleteColorHandle(ch);	
	}
}


/*********************************************************************
----------------------------------------------------------------------

		colorhandle = AddPicture(psm, picture, tags);

		fügt ein picture einer pensharemap hinzu und
		erzeugt dafür ein colorhandle

		GGFX_Weight				Gewichtungsfaktor

----------------------------------------------------------------------
*********************************************************************/

COLORHANDLE SAVE_DS ASM *AddPictureA(REG(a0) PSM *psm, REG(a1) PIC *pic, REG(a2) TAGLIST tags)
{
	COLORHANDLE *ch;

	ObtainSemaphore(&psm->semaphore);

	if ((ch = CreateColorHandle(psm, tags)))
	{
		BOOL success;

		ObtainSemaphore(&pic->semaphore);

		if (pic->hstype == HSTYPE_UNDEFINED)
		{
			pic->hstype = psm->hstype;
			DB(kprintf("(!) Histogramm der PenShareMap: %ld\n", pic->hstype));
		}

		success = UpdatePicture(pic);
		
		if(success)
		{
			LinkToSharedHistogram(pic->histogram);
			ch->histogram = pic->histogram;
			ch->numpixels = pic->width*pic->height;
			LinkColorHandle(ch);
		}
		else
		{
			DeleteColorHandle(ch);
			ch = NULL;		
		}

		ReleaseSemaphore(&pic->semaphore);
	}

	ReleaseSemaphore(&psm->semaphore);
	
	return ch;
}


/*********************************************************************
----------------------------------------------------------------------

		colorhandle = AddPalette(psm, palette, tags);

		fügt eine palette einer pensharemap hinzu und
		erzeugt dafür ein colorhandle

		GGFX_Weight				Gewichtungsfaktor
		GGFX_NumColors			Anzahl Farben in der Palette
		GGFX_PaletteFormat		Format der Palette

----------------------------------------------------------------------
*********************************************************************/

COLORHANDLE SAVE_DS ASM *AddPaletteA(
	REG(a0) PSM *psm,
	REG(a1) PALETTE palette,
	REG(a2) TAGLIST tags)
{
	COLORHANDLE *ch;

	if ((ch = CreateColorHandle(psm, tags)))
	{
		BOOL success = FALSE;

		if ((ch->numpixels = GetTagData(GGFX_NumColors, 0, tags)))
		{
			APTR histogram;
			int i;
			ULONG *p = (ULONG *) palette;
			
			if ((histogram = CreateHistogram(RND_HSType, psm->hstype,
				RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
			{
				if ((ch->histogram = CreateSharedHistogram(histogram)))
				{
					switch (GetTagData(GGFX_PaletteFormat, PALFMT_RGB8, tags))
					{
						case PALFMT_RGB8:
							success = TRUE;
							for(i=0; (i < ch->numpixels) && success; ++i)
							{
								success = (AddRGB(histogram, *p++, 1) == ADDH_SUCCESS);
							}
							break;
			
						case PALFMT_RGB32:
						{
							ULONG rgb;
							success = TRUE;
							for(i=0; (i < ch->numpixels) && success; ++i)
							{
								rgb = ((p[0] & 0xff000000) >> 8)
									+ ((p[1] & 0xff000000) >> 16)
									+ ((p[2] & 0xff000000) >> 24);
								success = (AddRGB(histogram, rgb, 1) == ADDH_SUCCESS);
								p += 3;
							}
							break;
						}
					}
				}
				else
				{
					DeleteHistogram(histogram);
				}
			}
		}

		if(success)
		{
			LinkColorHandle(ch);
		}
		else
		{
			DeleteColorHandle(ch);
			ch = NULL;		
		}

	}

	return ch;
}


/*********************************************************************
----------------------------------------------------------------------

		colorhandle = AddPixelArray(psm, array, width, height, tags);

		fügt ein pixelarray einer pensharemap hinzu und
		erzeugt dafür ein colorhandle

		GGFX_PixelFormat		Format des Arrays
		GGFX_Weight				Gewichtungsfaktor
		GGFX_Palette			Palette
		GGFX_NumColors			Anzahl Farben in der Palette
		GGFX_PaletteFormat		Format der Palette

----------------------------------------------------------------------
*********************************************************************/

COLORHANDLE SAVE_DS ASM *AddPixelArrayA(REG(a0) PSM *psm, REG(a1) APTR array,
	REG(d0) UWORD width , REG(d1) UWORD height, REG(a2) TAGLIST tags)
{
	COLORHANDLE *ch;

	if ((ch = CreateColorHandle(psm, tags)))
	{
		APTR histogram;
		BOOL success = FALSE;
		ch->numpixels = width*height;

		if ((histogram = CreateHistogram(RND_HSType, psm->hstype,
			RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
		{
			if ((ch->histogram = CreateSharedHistogram(histogram)))
			{
				switch (GetTagData(GGFX_PixelFormat, PIXFMT_CHUNKY_CLUT, tags))
				{
					case PIXFMT_CHUNKY_CLUT:
					{
						PALETTE palette;
						if ((palette = (PALETTE) GetTagData(GGFX_Palette, 0, tags)))
						{
							ULONG numcolors;
							if ((numcolors = GetTagData(GGFX_NumColors, 0, tags)))
							{
								PALETTE mypalette;
								if ((mypalette = CreatePalette(RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
								{
									ULONG palfmt = GetTagData(GGFX_PaletteFormat, PALFMT_RGB8, tags);
									ImportPalette(mypalette, palette, numcolors,
											RND_PaletteFormat, palfmt, TAG_DONE);
											
									if (AddChunkyImage(histogram, array, width,
										height, mypalette, TAG_DONE) == ADDH_SUCCESS)
									{
										success = TRUE;
									}
									DeletePalette(mypalette);
								}
							}
						}
						break;		
					}
		
					case PIXFMT_0RGB_32:
						if (AddRGBImage(histogram, array, width, height, TAG_DONE) == ADDH_SUCCESS)
						{
							success = TRUE;
						}
						break;		
				}
			}
			else
			{
				DeleteHistogram(histogram);
			}
		}

		if(success)
		{
			LinkColorHandle(ch);
		}
		else
		{
			DeleteColorHandle(ch);
			ch = NULL;		
		}

	}

	return ch;
}


