/*********************************************************************
----------------------------------------------------------------------

	guigfx_picture

----------------------------------------------------------------------
*********************************************************************/

#include <render/render.h>
#include <render/renderhooks.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#ifndef __AROS__
#include <datatypes/pictureclassext.h>
#endif
#include <libraries/cybergraphics.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <graphics/view.h>
#include <exec/memory.h>
#include <proto/dos.h>

#include <proto/render.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>


#include <guigfx/guigfx.h>

#include "guigfx_global.h"
#include "guigfx_picturemethod.h"
#include "guigfx_picture.h"
#include "guigfx_bitmap.h"


/*--------------------------------------------------------------------

	success = PrepareDrawing(pic)

	bereitet ein Bild darauf vor, gezeichnet werden zu können

--------------------------------------------------------------------*/

BOOL PrepareDrawing(PIC *pic)
{
	BOOL success;

						#ifdef DEBUG
						if (!pic)
						{
							DB(kprintf("#!*** Error: no pic!!! nicht meine schuld!!!\n"));
						}
						else
						{
							DB(kprintf("#!picture width/height/pixelformat: %d,%d,%d\n", pic->width, pic->height, pic->pixelformat));
						}
						#endif


	ObtainSemaphore(&pic->semaphore);
	
	switch (pic->pixelformat)
	{
		case PIXFMT_RGB_24:
			success = PIC_Render(pic, PIXFMT_0RGB_32, NULL);
			break;

		default:
			success = TRUE;
			break;
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}







/*********************************************************************
----------------------------------------------------------------------

	DeletePicture(pic)

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM DeletePicture(REG(a0) PIC *pic)
{
	if (pic)
	{
		ObtainSemaphore(&pic->semaphore);

		if (pic->scaleengine)
		{
			FreeRenderVec(pic->scaleengine);
			pic->scaleengine = NULL;
		}
	
		if (pic->alphaarray)
		{
			FreeRenderVec(pic->alphaarray);
			pic->alphaarray = NULL;
		}
	
		if (pic->histogram)
		{
			UnLinkSharedHistogram(pic->histogram);
			pic->histogram = NULL;
		}
	
		if (pic->palette)
		{
			DeletePalette(pic->palette);
		}
	
		if (pic->owner)
		{
			FreeRenderVec(pic->array);
		}
	
		ReleaseSemaphore(&pic->semaphore);
	
		FreeRenderVec(pic);
	}
}


/*********************************************************************
----------------------------------------------------------------------

		pic = MakePictureA(array, width, height, tags);

		make a guigfx picture from raw data.
	
		array 	- may be NULL, in which case an image will be created
		width	- source array width (overrides bm_Width)
		height	- source array height (overrides bm_Height)
		tags	

			GGFX_SourceX		- clip source left (default: 0)
			GGFX_SourceY		- clip source top (default: 0)
			GGFX_SourceWidth	- clip source width (default: width)
			GGFX_SourceHeight	- clip source height (default: height)
			GGFX_DestWidth		- dest width (default: sourcewidth)
			GGFX_DestHeight		- dest height (default: sourceheight)
			
			GGFX_PixelFormat	default: PIXFMT_CHUNKY_CLUT or bitmap format
				PIXFMT_CHUNKY_CLUT
				PIXFMT_0RGB_32
				PIXFMT_BITMAP_CLUT
				PIXFMT_BITMAP_HAM6
				PIXFMT_BITMAP_HAM8
				PIXFMT_BITMAP_RGB	(?)
				PIXFMT_RGB_24
	
			GGFX_Palette
				source palette (only recognized in CLUT and HAM modes)
	
			GGFX_NumColors
				default: undefined
		
			GGFX_PaletteFormat
				PALFMT_RGB32
				PALFMT_RGB8				default
				PALFMT_RGB4
				PALFMT_PALETTE			private
			
			GGFX_AspectX				Default: 1
		
			GGFX_AspectY				Default: 1
		
			GGFX_Owner					Default: FALSE			(private)
			
				force picture to free its array with DeletePicture().
				this tag allows other functions to submit an array to
				a picture. an independent picture is always owner of its data.
	
			GGFX_Independent			Default: FALSE
	
			GGFX_BufferSize
			
			GGFX_AlphaPresent
	
			GGFX_HSType

----------------------------------------------------------------------
*********************************************************************/


/*---------------------------------------------------------------------

	rgb24to32
	
	hook zum konvertieren/skalieren von rgb_24 auf rgb_32

--------------------------------------------------------------------*/

struct rgb24to32data
{
	UWORD width, totalwidth;
	ULONG *destbuffer;
};

#ifdef __MORPHOS__
FUNC_HOOK(ULONG, rgb24to32, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG Argb24to32(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS rgb24to32(	register __a0 struct Hook *hook,
								register __a2 APTR buffer,
								register __a1 struct RND_LineMessage *msg )
#endif
{
/******
	if (msg->RND_LMsg_type == LMSGTYPE_LINE_FETCH)
	{
		struct insertalphadata *args = (struct insertalphadata *)(hook->h_Data);

		InsertAlphaChannel((UBYTE *)buffer, args->width, 1, args->destbuffer, NULL);
		args->destbuffer += args->totalwidth;
	}
*******/
	return TRUE;
}

/*-------------------------------------------------------------------*/


#ifndef __MORPHOS__
PIC *MakePicture(APTR array, UWORD width, UWORD height, Tag tag1, ...)
{
	return MakePictureA(array, width, height, (TAGLIST) &tag1);
}
#endif

PIC SAVE_DS ASM *MakePictureA(
	REG(a0) APTR data,
	REG(d0) UWORD totalsourcewidth,
	REG(d1) UWORD totalsourceheight,
	REG(a1) TAGLIST taglist)
{
	PIC *pic = NULL;
	BOOL success = FALSE;

	if ((pic = AllocRenderVecClear(MemHandler, sizeof(PIC))))
	{

		int sourcewidth, sourceheight;
		int destwidth, destheight;
		int sourcex, sourcey;
		int sourceformat, destformat = PIXFMT_CHUNKY_CLUT;
		BOOL independent = FALSE;
		BOOL owner = FALSE;
		BOOL bitmap = FALSE;
		BOOL scaling = FALSE;
		BOOL datacanreside = TRUE;
		int bytesrequired, maxbytes;
		BOOL paletterequired = FALSE;
		PALETTE palette = NULL;
		BOOL externalpalette = FALSE;			// external PALFMT_PALETTE to be incorporated?
		int allocbytes = 0;
		APTR array = NULL;
		BOOL externalarray = FALSE;			// external array to be incorporated?
		int hstype;
		

	
		InitSemaphore(&pic->semaphore);
		pic->scaleengine = NULL;
		pic->scalepixelformat = ~0;
		pic->scalesourcewidth = -1;
		pic->scalesourceheight = -1;
		pic->scaledestwidth = -1;
		pic->scaledestheight = -1;
		pic->histogram = NULL;



		success = TRUE;

		sourceformat = GetTagData(GGFX_PixelFormat, PIXFMT_CHUNKY_CLUT, taglist);

		sourcewidth = GetTagData(GGFX_SourceWidth, totalsourcewidth, taglist);
		sourceheight = GetTagData(GGFX_SourceHeight, totalsourceheight, taglist);

		sourcex = GetTagData(GGFX_SourceX, 0, taglist);
		sourcey = GetTagData(GGFX_SourceY, 0, taglist);

		destwidth = GetTagData(GGFX_DestWidth, sourcewidth, taglist);
		destheight = GetTagData(GGFX_DestHeight, sourceheight, taglist);

		hstype = GetTagData(GGFX_HSType, HSTYPE_UNDEFINED, taglist);


		if (sourcewidth != destwidth || sourceheight != destheight)
		{
			scaling = TRUE;				// scaling is involved
		}


		if (totalsourcewidth != destwidth || totalsourceheight != destheight || scaling
			|| sourcex != 0 || sourcey != 0 || !data)
		{
			datacanreside = FALSE;		// data has to be moved or created
		}


		switch (sourceformat)
		{
			case PIXFMT_BITMAP_RGB:
				destformat = PIXFMT_0RGB_32;
				bitmap = TRUE;
				datacanreside = FALSE;
				break;

			case PIXFMT_BITMAP_HAM6:
			case PIXFMT_BITMAP_HAM8:
				destformat = PIXFMT_0RGB_32;
				bitmap = TRUE;
				datacanreside = FALSE;
				paletterequired = TRUE;
				break;
	
			case PIXFMT_BITMAP_CLUT:
				destformat = PIXFMT_CHUNKY_CLUT;
				bitmap = TRUE;
				datacanreside = FALSE;
				paletterequired = TRUE;
				break;
	
			case PIXFMT_0RGB_32:
				destformat = PIXFMT_0RGB_32;
				pic->alphapresent = GetTagData(GGFX_AlphaPresent, FALSE, taglist);
				break;

			case PIXFMT_RGB_24:
				if (data)
				{
					destformat = PIXFMT_0RGB_32;
					pic->alphapresent = FALSE;
					datacanreside = FALSE;
				}
				else
				{
					success = FALSE;
				}
				break;
			
			case PIXFMT_CHUNKY_CLUT:
				destformat = PIXFMT_CHUNKY_CLUT;
				paletterequired = TRUE;
				break;
	
			default:
				success = FALSE;		// undefined source format
				break;
		}


		if (bitmap && !data)
		{			
			success = FALSE;			//	a bitmap without source data is invalid
		}


		if (datacanreside)
		{
			if (data)
			{
				//	the user may decide to create an independent picture

				independent = GetTagData(GGFX_Independent, FALSE, taglist);
			}
			else
			{
				independent = TRUE;
			}
		}
		else
		{
			// data cannot reside, so the user cannot demand it.
			
			if ((independent = GetTagData(GGFX_Independent, TRUE, taglist)) == FALSE)
			{
				success = FALSE;
			}
		}

	

		if (success)
		{
			bytesrequired = destwidth * destheight * PIXELSIZE(destformat);
			maxbytes = bytesrequired;

			if (independent)
			{
				owner = TRUE;	// an independent picture is always owner of its data
				allocbytes = bytesrequired;
			}
			else
			{
				//	a non-independent picture is by default not owner of its data,
				//	but guigfx functions can enforce a picture to 'take over' the data

				owner = GetTagData(GGFX_Owner, FALSE, taglist);
				
				//	the user can specify a buffer overhang for the static picture:
				
				maxbytes = GetTagData(GGFX_BufferSize, maxbytes, taglist);
				if (maxbytes < bytesrequired)
				{
					success = FALSE;
				}
			}
		}
		


		if (success)
		{
			if (paletterequired)
			{
				PALETTE userpalette;
				int paletteformat;

				success = FALSE;
				
				userpalette = (PALETTE) GetTagData(GGFX_Palette, NULL, taglist);
				paletteformat = GetTagData(GGFX_PaletteFormat, PALFMT_RGB8, taglist);
			
				if (userpalette)
				{
					if (paletteformat == PALFMT_PALETTE)
					{
						// incorporate palette to the picture
					
						palette = userpalette;
						externalpalette = TRUE;			// this is an external palette handed over
						success = TRUE;
					}
					else
					{
						// import palette
					
						if ((palette = CreatePalette(RND_RMHandler, (IPTR)MemHandler,
							RND_HSType, hstype == HSTYPE_UNDEFINED ? DEFAULT_PALETTE_HSTYPE : hstype, 
							TAG_DONE)))
						{
							int numcolors;

							if ((numcolors = GetTagData(GGFX_NumColors, 0, taglist)))
							{
								ImportPalette(palette, userpalette, numcolors, 
									RND_PaletteFormat, paletteformat, TAG_DONE);
				
								success = TRUE;
							}
						}
					}
				}
				else
				{
					// create default palette
					
					if ((palette = CreatePalette(RND_RMHandler, (IPTR)MemHandler,
						RND_HSType, hstype == HSTYPE_UNDEFINED ? DEFAULT_PALETTE_HSTYPE : hstype, 
						TAG_DONE)))
					{
						ImportPalette(palette, defaultpalette, 256, NULL);
		
						success = TRUE;
					}					
				}			
			}
		}
		


		if (success)
		{
			success = FALSE;
		
			if (bitmap)
			{
				ULONG displayID = 0;
				
				if (sourceformat == PIXFMT_BITMAP_HAM6 || sourceformat == PIXFMT_BITMAP_HAM8)
				{
						displayID = HAM;
				}
				
				if ((array = ReadBitMap(data, displayID, palette, &destformat, &destwidth, &destheight,
					GGFX_SourceX, sourcex, GGFX_SourceY, sourcey,
					GGFX_SourceWidth, sourcewidth, GGFX_SourceHeight, sourceheight,
					GGFX_DestWidth, destwidth, GGFX_DestHeight, destheight,
					TAG_DONE)))
				{
					success = TRUE;
				}
			}
			else if (allocbytes)
			{
				DB(kprintf("!makepicture alloc(allocbytes)\n"));
				if ((array = AllocRenderVec(MemHandler, allocbytes)))
				{
					if (data)
					{
						if (sourceformat == destformat)
						{
							if (scaling)
							{
								APTR scalingengine;
								
								if ((scalingengine = CreateScaleEngine(sourcewidth, sourceheight,
									destwidth, destheight, RND_RMHandler, (IPTR)MemHandler,
									RND_PixelFormat, destformat, TAG_DONE)))
								{
									Scale(scalingengine, data, array,
										RND_SourceWidth, totalsourcewidth, TAG_DONE);
									DeleteScaleEngine(scalingengine);
									success = TRUE;
								}
							}
							else
							{
								// ÄÄÄH?! WAS IST MIT OFFSETS???
							
								TurboCopyMem(data, array, allocbytes);
								success = TRUE;
							}
						}
						else
						{
							if (sourceformat == PIXFMT_RGB_24 && destformat == PIXFMT_0RGB_32)
							{
								if (scaling)
								{
									/***********
								
									ULONG *linebuffer; 
									
									success = FALSE;
									
									if (linebuffer = AllocRenderVec(MemHandler, data->destwidth*sizeof(ULONG)))
									{
										APTR scalingengine;
										struct Hook drawhook;
										
										if (scalingengine = CreateScaleEngine(sourcewidth, sourceheight,
											destwidth, destheight, RND_RMHandler, MemHandler,
											RND_PixelFormat, destformat, TAG_DONE))
										{
											Scale(scalingengine, data, array,
												RND_LineHook, &drawhook
												RND_SourceWidth, totalsourcewidth, TAG_DONE);
											DeleteScaleEngine(scalingengine);
											success = TRUE;
										}
										
										FreeRenderVec(linebuffer);
									}

									*****/

									// not supported yet. TODO.
								
									success = FALSE;

								}
								else
								{
									int x, y;
									UBYTE *sp = (UBYTE *) data;
									UBYTE *dp = (UBYTE *) array;
								
									for (x = 0; x < sourcewidth; ++x)
									{
										for (y = 0; y < sourceheight; ++y)
										{
											*dp++ = 0;
											*dp++ = *sp++;
											*dp++ = *sp++;
											*dp++ = *sp++;
										}
										sp += 3 * (totalsourcewidth - sourcewidth);
									}
									success = TRUE;
								}		
							}
							else
							{
								// not supported
								
								success = FALSE;
							}
						}
					}
					else
					{
						TurboFillMem(array, allocbytes, 0);
						success = TRUE;
					}
				}
			}
			else
			{
				array = data;
				externalarray = TRUE;
				success = TRUE;
			}

			pic->owner = owner;
		}


		if (success)
		{
			pic->maxbytes = maxbytes;

			pic->aspectx = GetTagData(GGFX_AspectX, 1, taglist);
			pic->aspecty = GetTagData(GGFX_AspectY, 1, taglist);

			pic->width = destwidth;
			pic->height = destheight;
			
			pic->hstype = hstype;

			pic->pixelformat = destformat;			
		

			pic->palette = palette;			// include palette to the picture 
			pic->array = array;				// include array to the picture
		}
		else
		{
			// external palettes are not deleted unless the picture could be created successfully.
	
			if (!externalpalette)
			{
				pic->palette = palette;
			}

			// external arrays are not deleted unless the picture could be created successfully.
			
			if (!externalarray)
			{
				pic->array = array;
			}
	
			DeletePicture(pic);
			pic = NULL;
		}

	}

	return pic;
}







/*********************************************************************
----------------------------------------------------------------------

		pic = ReadPicture(rastport, colormap, x, y, width, height, tags);
	
		liest ein Picture aus einem Rastport.
		
		GGFX_HSType
			HSTYPE_12BIT_TURBO		default
			
		GGFX_AspectX				Default: 1
	
		GGFX_AspectY				Default: 1

		GGFX_DestWidth
		
		GGFX_DestHeight


		GGFX_ModeID

----------------------------------------------------------------------
*********************************************************************/

PIC SAVE_DS ASM *ReadPictureA(
	REG(a0) struct RastPort *rp,
	REG(a1) struct ColorMap *cm,
	REG(d0) UWORD x,
	REG(d1) UWORD y,
	REG(d2) UWORD width,
	REG(d3) UWORD height,
	REG(a2) TAGLIST taglist)
{
	PIC *pic;
	BOOL success = FALSE;

	if ((pic = AllocRenderVecClear(MemHandler, sizeof(PIC))))
	{
		ULONG modeID;
		int destwidth, destheight, destformat;
		int sourcewidth, sourceheight;

		
		sourcewidth = width;
		sourceheight = height;
		destwidth = GetTagData(GGFX_DestWidth, width, taglist);
		destheight = GetTagData(GGFX_DestHeight, height, taglist);
		

		pic->histogram = NULL;
		pic->owner = TRUE;

		pic->hstype = GetTagData(GGFX_HSType, HSTYPE_UNDEFINED, taglist);
		modeID = GetTagData(GGFX_ModeID, INVALID_ID, taglist);



		if ((pic->palette = CreatePalette(RND_RMHandler, (IPTR)MemHandler, RND_HSType, 
			pic->hstype == HSTYPE_UNDEFINED ? DEFAULT_PALETTE_HSTYPE : pic->hstype, 
			TAG_DONE)))
		{
			int i;
			ULONG rgb[3];
			for(i=0; i<256; ++i)
			{
				GetRGB32(cm, i, 1, rgb);
				ImportPalette(pic->palette, rgb, 1,
								RND_PaletteFormat, PALFMT_RGB32,
								RND_NewPalette, FALSE,
								RND_FirstColor, i,
								TAG_DONE);
			}
		}

		

		if ((pic->array = ReadBitMap(rp->BitMap, modeID, pic->palette, 
				&destformat, &destwidth, &destheight,
				GGFX_SourceX, x, GGFX_SourceY, y,
				GGFX_SourceWidth, sourcewidth, GGFX_SourceHeight, sourceheight,
				GGFX_DestWidth, destwidth, GGFX_DestHeight, destheight,
				TAG_DONE)))
		{
			pic->width = destwidth;
			pic->height = destheight;
			pic->aspectx = GetTagData(GGFX_AspectX, 1, taglist);
			pic->aspecty = GetTagData(GGFX_AspectY, 1, taglist);
			pic->pixelformat = destformat;

			success = TRUE;
		}
	}



	if(success == FALSE)
	{
		DeletePicture(pic);
		pic = NULL;
	}
	else
	{
		InitSemaphore(&pic->semaphore);
	}

	return pic;
}


/*********************************************************************
----------------------------------------------------------------------

	BOOL UpdatePicture(picture);

	Aktualisiert ein Picture
	(Neuberechnung des Histogramms usw.)

----------------------------------------------------------------------
*********************************************************************/

BOOL SAVE_DS ASM UpdatePicture(REG(a0) PIC *pic)
{
	BOOL success = TRUE;

	ObtainSemaphore(&pic->semaphore);

	/*
	 *	shared histogram erzeugen
	 */

	if(!pic->histogram)
	{
		APTR histo;
		success = FALSE;


		if (pic->hstype == HSTYPE_UNDEFINED)
		{
			pic->hstype = DEFAULT_PICTURE_HSTYPE;
		}

		if ((histo = CreateHistogram(RND_RMHandler, (IPTR)MemHandler,
				RND_HSType, pic->hstype, TAG_DONE)))
		{
			if ((pic->histogram = CreateSharedHistogram(histo)))
			{
				success = TRUE;
			}
			else
			{
				DeleteHistogram(histo);
			}
		}
	}

	/*
	 *	Histogramm aufbauen
	 */

	if (success)
	{
		if ((success = PrepareDrawing(pic)))	/* eliminiert unbrauchbare Pixelformate */
		{
			int interleave;
			
			if (pic->height/DEFAULT_PICTURE_INTERLEAVE == 0)
			{
				interleave = 1;
			}
			else
			{
				interleave = DEFAULT_PICTURE_INTERLEAVE;
			}
			
			success = FALSE;
			switch (pic->pixelformat)
			{
				case PIXFMT_CHUNKY_CLUT:
					if(pic->palette)
					{
						success = (AddChunkyImage(pic->histogram->histogram, pic->array,
							pic->width, pic->height/interleave, pic->palette, 
							RND_SourceWidth, pic->width*interleave,
							TAG_DONE) == ADDH_SUCCESS);
					}
					break;
		
				case PIXFMT_0RGB_32:
					success = (AddRGBImage(pic->histogram->histogram, (APTR) pic->array,
						pic->width, pic->height/interleave, 
						RND_SourceWidth, pic->width*interleave,
						TAG_DONE) == ADDH_SUCCESS);
					break;
			}
		}
	}

	if (!success)
	{
		if (pic->histogram)
		{
			UnLinkSharedHistogram(pic->histogram);
			pic->histogram = NULL;
		}
	}

	ReleaseSemaphore(&pic->semaphore);
	
	return success;
}


/*********************************************************************
----------------------------------------------------------------------

	count = GetPictureAttrsA(picture, taglist)

	holt Picture-Attribute

----------------------------------------------------------------------
*********************************************************************/

ULONG SAVE_DS ASM GetPictureAttrsA(REG(a0) PIC *pic, REG(a1) TAGLIST tags)
{
	ULONG count = 0;

	if (pic)
	{
		ULONG *p;
		
		if ((p = (ULONG *) GetTagData(PICATTR_Width, NULL, tags)))
		{
			*p = pic->width;
			count++;
		}
	
		if ((p = (ULONG *) GetTagData(PICATTR_Height, NULL, tags)))
		{
			*p = pic->height;
			count++;
		}
	
		if ((p = (ULONG *) GetTagData(PICATTR_RawData, NULL, tags)))
		{
			*p = (ULONG) pic->array;
			count++;
		}
	
		if ((p = (ULONG *) GetTagData(PICATTR_PixelFormat, NULL, tags)))
		{
			*p = pic->pixelformat;
			count++;
		}
	
		if ((p = (ULONG *) GetTagData(PICATTR_AspectX, NULL, tags)))
		{
			*p = pic->aspectx;
			count++;
		}
	
		if ((p = (ULONG *) GetTagData(PICATTR_AspectY, NULL, tags)))
		{
			*p = pic->aspecty;
			count++;
		}

		if ((p = (ULONG *) GetTagData(PICATTR_AlphaPresent, NULL, tags)))
		{
			*p = (ULONG) pic->alphapresent;
			count++;
		}
		
		if ((p = (ULONG *) GetTagData(PICATTR_NumPaletteEntries, NULL, tags)))
		{
			if (pic->palette)
			{
				*p = 256;
			}
			else
			{
				*p = 0;
			}
			count++;
		}

		if ((p = (ULONG *) GetTagData(PICATTR_Palette, NULL, tags)))
		{
			if (pic->palette)
			{
				ExportPaletteA(pic->palette, p, NULL);
				count++;
			}
		}
		
	}
	
	return count;
}


/*********************************************************************
----------------------------------------------------------------------

	picture = ClonePicture(picture);

	klont ein Bild

----------------------------------------------------------------------
*********************************************************************/

PIC SAVE_DS ASM *ClonePictureA(REG(a0) PIC *pic, REG(a1) TAGLIST tags)
{
	PIC *newpic = NULL;

	if (pic)
	{
		APTR array = NULL;
		BOOL success = FALSE;
		PALETTE newpalette = NULL;
		
		int sx, sy, sw, sh, dw, dh;

		ObtainSemaphore(&pic->semaphore);
		
		if (PrepareDrawing(pic))		/* eliminiert unbrauchbare Pixelformate */
		{
			if (InsertAlphaArray(pic))	/* Alpha-Channel soll im array vorliegen */
			{
				sx = GetTagData(GGFX_SourceX, 0, tags);
				sy = GetTagData(GGFX_SourceY, 0, tags);
				sw = GetTagData(GGFX_SourceWidth, pic->width, tags);
				sh = GetTagData(GGFX_SourceHeight, pic->height, tags);
				dw = GetTagData(GGFX_DestWidth, pic->width, tags);
				dh = GetTagData(GGFX_DestHeight, pic->height, tags);

				if ((array = AllocRenderVec(MemHandler, 
					dw * dh * PIXELSIZE(pic->pixelformat))))
				{
					success = TRUE;
			
					if (pic->palette)
					{
						success = FALSE;
						if ((newpalette = CreatePalette(RND_RMHandler, (IPTR)MemHandler, RND_HSType, 
								pic->hstype == HSTYPE_UNDEFINED ? DEFAULT_PALETTE_HSTYPE : pic->hstype, 
								TAG_DONE)))
						{
							ULONG numcolors;
							
							if ((numcolors = GetPaletteAttrs(pic->palette, NULL)))
							{
								ImportPalette(newpalette, pic->palette, numcolors, 
									RND_PaletteFormat, PALFMT_PALETTE, TAG_DONE);
								success = TRUE;
							}
						}
					}
			
					if (success)
					{
						char *p;
						int pixelsize = PIXELSIZE(pic->pixelformat);
						p = pic->array + (pic->width * sy + sx) * pixelsize;
						
						if (sw == dw && sh == dh)
						{
							if (sw == pic->width)
							{
								TurboCopyMem(p, array, sw * sh * pixelsize);
							}
							else
							{
								char *temparray = array;
								int i;
								for (i = 0; i < sh; ++i)
								{
									TurboCopyMem(p, temparray, sw * pixelsize);
									p += pic->width * pixelsize;
									temparray += sw * pixelsize;
								}
							}
						}
						else
						{
							APTR scaleengine;
							success = FALSE;
							if ((scaleengine = CreateScaleEngine(sw, sh, dw, dh,
								RND_PixelFormat, pic->pixelformat,
								RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
							{
								success = (Scale(scaleengine, p, array,
									RND_SourceWidth, pic->width, TAG_DONE) == CONV_SUCCESS);
								DeleteScaleEngine(scaleengine);
							}
						}


						if (success)
						{
							success = FALSE;
	
							if ((newpic = MakePicture(array, dw, dh,
								GGFX_PixelFormat, pic->pixelformat,
								GGFX_Palette, (IPTR)newpalette,
								GGFX_PaletteFormat, PALFMT_PALETTE,
								GGFX_HSType, pic->hstype,
								GGFX_AspectX, pic->aspectx,
								GGFX_AspectY, pic->aspecty,
								GGFX_Owner, TRUE,
								GGFX_AlphaPresent, pic->alphapresent,
								TAG_DONE)))
							{
								if (!newpic->palette && newpalette)
								{
									/* 
									**	the palette was not required!
									*/
								
									DeletePalette(newpalette);
									newpalette = NULL;
								}

								newpic->directdraw = pic->directdraw;
								success = TRUE;
							}
						}
					}
			
				}
			}
		}

		ReleaseSemaphore(&pic->semaphore);
	
		if (!success)
		{
			if (newpalette)
			{
				DeletePalette(newpalette);
			}
			
			FreeRenderVec(array);
			
			newpic = NULL;
		}
	}

	return newpic;
}



/*********************************************************************
----------------------------------------------------------------------

	result = LockPicture(picture, lockmode, args);

	lockt ein Bild

	LOCKMODE_DRAWHANDLE
	LOCKMODE_FORCE

----------------------------------------------------------------------
*********************************************************************/

ULONG SAVE_DS ASM LockPictureA(REG(a0) PIC *pic, REG(d0) ULONG lockmode, REG(a1) ULONG *arg)
{
	return FALSE;

/*
	ULONG result = FALSE;

	if(pic)
	{	
		switch (lockmode & LOCKMODE_MASK)
		{
			case LOCKMODE_DRAWHANDLE:
				result = Lock_DrawHandle(pic, lockmode, (DRAWHANDLE *) arg[0], (TAGLIST) &arg[1]);
				break;

			default:
				result = FALSE;
				break;
		}
	}

	return result;
*/
}


/*********************************************************************
----------------------------------------------------------------------

	UnLockPicture(picture, lockmode);

	gibt einen Lockzustand frei (derzeit immer nur einen auf einmal!)

	LOCKMODE_DRAWHANDLE

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM UnLockPicture(REG(a0) PIC *pic, REG(d0) ULONG lockmode)
{
/*
	if(pic)
	{	
		ObtainSemaphore(&pic->semaphore);
		switch (lockmode & LOCKMODE_MASK)
		{
			case LOCKMODE_DRAWHANDLE:
				pic->directdraw = NULL;
				RemapPicture(pic, pic->pentabptr);
				pic->pentabptr = NULL;
				break;

			default:
				break;
		}
		ReleaseSemaphore(&pic->semaphore);
	}
*/
}

