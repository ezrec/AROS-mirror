/*********************************************************************
----------------------------------------------------------------------

	guigfx_picturemethod

----------------------------------------------------------------------
*********************************************************************/

#include <render/render.h>
#include <render/renderhooks.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <graphics/view.h>
#include <exec/memory.h>

#include <proto/render.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/cybergraphics.h>
#include <libraries/cybergraphics.h>

#include <guigfx/guigfx.h>

#include "guigfx_global.h"
#include "guigfx_picturemethod.h"
#include "guigfx_convolve.h"


/*********************************************************************
----------------------------------------------------------------------

	BOOL InsertAlphaArray(pic)

	fügt einen ggf. vorhandenen, ausgelagerten
	Alpha-Channel in das Bild ein. Es wird vorausgesetzt,
	daß der ausgelagerte Alphachannel dieselbe Größe hat
	wie das Bild.

----------------------------------------------------------------------
*********************************************************************/

BOOL InsertAlphaArray(PIC *pic)
{
	BOOL success = TRUE;

	ObtainSemaphore(&pic->semaphore);
	
	if (pic->alphapresent && pic->alphaarray)
	{
		/*
		 *	hier ist höchste Vorsicht geboten. PIC_Render
		 *	und InsertAlphaArray rufen sich gegenseitig auf!
		 */
		
		if (pic->pixelformat != PIXFMT_0RGB_32)
		{
			PIC_Render(pic, PIXFMT_0RGB_32, NULL);
		}
		
		success = (pic->pixelformat == PIXFMT_0RGB_32);
		
		if (success)
		{
			if (pic->alphaarray)
			{
				InsertAlphaChannel(pic->alphaarray, pic->width, pic->height, (APTR)pic->array, NULL);
				FreeRenderVec(pic->alphaarray);
				pic->alphaarray = NULL;
			}
		}
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*********************************************************************
----------------------------------------------------------------------

	BOOL ExtractAlphaArray(pic)

	lagert einen ggf. im Bild vorhandenen Alphachannel in
	ein externes Array aus.

----------------------------------------------------------------------
*********************************************************************/

BOOL ExtractAlphaArray(PIC *pic)
{
	BOOL success = TRUE;

	ObtainSemaphore(&pic->semaphore);
	
	if (pic->alphapresent && !pic->alphaarray)
	{
		if (pic->pixelformat == PIXFMT_0RGB_32)
		{
			success = FALSE;
			if ((pic->alphaarray = AllocRenderVec(MemHandler, pic->width * pic->height)))
			{
				ExtractAlphaChannel((APTR) pic->array,
					pic->width, pic->height, pic->alphaarray, NULL);
				success = TRUE;
			}
		}
		#ifdef DEBUG
		else
		{
			DB(kprintf("*** Illegal condition: where is the alpha-channel?\n"));
		}
		#endif
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}



/*********************************************************************
----------------------------------------------------------------------

	DoPictureMethod(picture,method,argument,...)

	wendet eine Picture-Methode auf ein Bild an.

----------------------------------------------------------------------
*********************************************************************/

ULONG SAVE_DS ASM DoPictureMethodA(
	REG(a0) APTR pic, 
	REG(d0) ULONG method, 
	REG(a1) ULONG *arg)
{
	ULONG result = FALSE;

	if(pic)
	{	
		switch (method)
		{
			case PICMTHD_CROP:
				result = PIC_Crop((PIC *) pic, arg[0], arg[1], arg[2], arg[3], (TAGLIST) &arg[4]);
				break;
	
			case PICMTHD_RENDER:
				result = PIC_Render((PIC *) pic, arg[0], (TAGLIST) &arg[1]);
				break;
	
			case PICMTHD_SCALE:
				result = PIC_Scale((PIC *) pic, arg[0], arg[1], (TAGLIST) &arg[2]);
				break;
	
			case PICMTHD_MIX:
				result = PIC_Mix((PIC *) pic, (PIC *) arg[0], (TAGLIST) &arg[1]);
				break;
	
			case PICMTHD_SETALPHA:
				result = PIC_SetAlpha((PIC *) pic, (UBYTE *) arg[0], arg[1], arg[2], (TAGLIST) &arg[3]);
				break;
	
			case PICMTHD_MIXALPHA:
				result = PIC_MixAlpha((PIC *) pic, (PIC *) arg[0], (TAGLIST) &arg[1]);
				break;

			case PICMTHD_MAPDRAWHANDLE:
				result = PIC_MapDrawHandle((PIC *) pic, (struct DrawHandle *) arg[0], (TAGLIST) &arg[1]);
				break;

			case PICMTHD_CREATEALPHAMASK:
				result = PIC_CreateAlphaMask((PIC *) pic, (ULONG) arg[0], (TAGLIST) &arg[1]);
				break;

			case PICMTHD_TINT:
				result = PIC_Tint((PIC *) pic, (ULONG) arg[0], (TAGLIST) &arg[1]);
				break;

			case PICMTHD_TINTALPHA:
				result = PIC_TintAlpha((PIC *) pic, (ULONG) arg[0], (TAGLIST) &arg[1]);
				break;

			case PICMTHD_TEXTURE:
				result = PIC_Texture((PIC *) pic, (PIC *) arg[0], (WORD *) arg[1], (TAGLIST) &arg[2]);
				break;

			case PICMTHD_SET:
				result = PIC_Set((PIC *) pic, (ULONG) arg[0], (TAGLIST) &arg[1]);
				break;

			case PICMTHD_INSERT:
				result = PIC_Insert((PIC *) pic, (PIC *) arg[0], (TAGLIST) &arg[1]);
				break;

			case PICMTHD_FLIPX:
				result = PIC_FlipX((PIC *) pic, (TAGLIST) &arg[0]);
				break;

			case PICMTHD_FLIPY:
				result = PIC_FlipY((PIC *) pic, (TAGLIST) &arg[0]);
				break;

			case PICMTHD_CHECKAUTODITHER:
				result = PIC_CheckDither((PIC *) pic, (DRAWHANDLE *) arg[0], (TAGLIST) &arg[1]);
				break;

			case PICMTHD_NEGATIVE:
				result = PIC_Negative((PIC *) pic, (TAGLIST) &arg[1]);
				break;

			case PICMTHD_AUTOCROP:
				result = PIC_AutoCrop((PIC *) pic, (TAGLIST) &arg[1]);
				break;

			case PICMTHD_CONVOLVE:
				result = PIC_Convolve((PIC *) pic, (KERNEL *) arg[0], (TAGLIST) &arg[1]);
				break;
	
			default:
				result = FALSE;
		}
	}

	return result;
}



/*--------------------------------------------------------------------

	CreateAlphaMask (pic, rgb, tags)

	GGFX_SourceX		Default 0
	GGFX_SourceY		Default 0
	GGFX_SourceWidth	Bildbreite
	GGFX_SourceHeight	Bildhöhe

--------------------------------------------------------------------*/

ULONG PIC_CreateAlphaMask(PIC *pic, ULONG rgb, TAGLIST tags)
{
	BOOL success = FALSE;

	ObtainSemaphore(&pic->semaphore);

	if (InsertAlphaArray(pic))
	{
		UWORD sourcewidth, sourceheight, sourcex, sourcey;

		sourcewidth = GetTagData(GGFX_SourceWidth, pic->width, tags);
		sourceheight = GetTagData(GGFX_SourceHeight, pic->height, tags);
		sourcex = GetTagData(GGFX_SourceX, 0, tags);
		sourcey = GetTagData(GGFX_SourceY, 0, tags);

		/*
		 *	clipping
	
		sourcewidth = MIN(pic->width - sourcex, sourcewidth);
		sourceheight = MIN(pic->height - sourcey, sourceheight);

		 */

		success = TRUE;

		if (sourcewidth && sourceheight)
		{
			success = FALSE;

			if (PIC_Render(pic, PIXFMT_0RGB_32, NULL))
			{
				ULONG *p = ((ULONG *) pic->array) + sourcex + sourcey * pic->width;
	
				CreateAlphaArray(p, sourcewidth, sourceheight,
					RND_SourceWidth, pic->width, RND_AlphaWidth, pic->width,
					RND_MaskRGB, rgb, TAG_DONE);
	
				pic->alphapresent = TRUE;
	
				success = TRUE;	
			}
		}
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*---------------------------------------------------------------------

	insertalpha

	fügt eine Zeile Alpha-Channel in einen RGB-Buffer

--------------------------------------------------------------------*/

struct insertalphadata
{
	UWORD width, totalwidth;
	ULONG *destbuffer;
};

#ifdef __MORPHOS__
FUNC_HOOK(ULONG, insertalpha, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG insertalpha(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS insertalpha(	register __a0 struct Hook *hook,
								register __a2 APTR buffer,
								register __a1 struct RND_LineMessage *msg )
#endif
{
	if (msg->RND_LMsg_type == LMSGTYPE_LINE_RENDERED)
	{
		struct insertalphadata *args = (struct insertalphadata *)(hook->h_Data);

		InsertAlphaChannel((UBYTE *)buffer, args->width, 1, args->destbuffer, NULL);
		args->destbuffer += args->totalwidth;
	}

	return TRUE;
}

/*--------------------------------------------------------------------

	SetAlpha (pic,array,width,height,tags)

	ist array=NULL, so wird der Alphachannel entfernt.

	GGFX_DestWidth
	GGFX_DestHeight

--------------------------------------------------------------------*/

ULONG PIC_SetAlpha(PIC *pic, UBYTE *array, ULONG sourcewidth, ULONG sourceheight, TAGLIST tags)
{
	BOOL success = FALSE;
	UWORD destwidth, destheight, destx, desty;

	ObtainSemaphore(&pic->semaphore);

	if (array)
	{
		destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
		destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
		destx = GetTagData(GGFX_DestX, 0, tags);
		desty = GetTagData(GGFX_DestY, 0, tags);

		/*
		 *	clipping
	
		destwidth = MIN(pic->width - destx, destwidth);
		destheight = MIN(pic->height - desty, destheight);

		 */

		success = TRUE;

		if (destwidth && destheight)
		{
			success = FALSE;

			FreeRenderVec(pic->alphaarray);
			pic->alphaarray = NULL;
		
			if (PIC_Render(pic, PIXFMT_0RGB_32, NULL))
			{
				ULONG *dest = ((ULONG *) pic->array) + destx + desty * pic->width;
		
				if ((destwidth != sourcewidth) || (destheight != sourceheight))
				{
					APTR scaleengine;
		
					if ((scaleengine = CreateScaleEngine(sourcewidth, sourceheight,
						destwidth, destheight, RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
					{
						UBYTE *linebuffer;
						if ((linebuffer = AllocRenderVec(MemHandler, destwidth)))
						{
							struct Hook drawhook;
							struct insertalphadata args;
		
							args.width = destwidth;
							args.totalwidth = pic->width;
							args.destbuffer = dest;
							drawhook.h_Data = &args;
#ifdef __MORPHOS__
							drawhook.h_Entry = (HOOKFUNC) &insertalpha;
#elif defined(__AROS__)
    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
							drawhook.h_SubEntry = (HOOKFUNC) insertalpha;
#else
							drawhook.h_Entry = (HOOKFUNC) insertalpha;
#endif
		
							Scale(scaleengine, array, linebuffer, 
								RND_LineHook, (IPTR)&drawhook,
								RND_DestWidth, 0, TAG_DONE);
		
							FreeRenderVec(linebuffer);
		
							FreeRenderVec(pic->alphaarray);
							pic->alphaarray = NULL;
						
							pic->alphapresent = TRUE;
							success = TRUE;
						}
		
						DeleteScaleEngine(scaleengine);
					}
				}
				else
				{
					InsertAlphaChannel(array, sourcewidth, sourceheight, dest,
						RND_DestWidth, pic->width, TAG_DONE);
					pic->alphapresent = TRUE;
					success = TRUE;
				}
			}
		}
	}
	else
	{
		FreeRenderVec(pic->alphaarray);
		pic->alphaarray = NULL;
		pic->alphapresent = FALSE;
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*--------------------------------------------------------------------

	Crop (pic,x,y,width,height,tags)

--------------------------------------------------------------------*/

ULONG PIC_Crop(PIC *pic, ULONG x, ULONG y, ULONG width, ULONG height,
	TAGLIST tags)
{
	BOOL success = FALSE;

	ObtainSemaphore(&pic->semaphore);

	if (PrepareDrawing(pic))
	{
		if (InsertAlphaArray(pic))
		{
			switch (pic->pixelformat)
			{
				case PIXFMT_0RGB_32:
				{
					int i;
					ULONG *t, *p = ((ULONG *) pic->array) + y*pic->width + x;
					
					if (pic->owner)
					{
						if((t = AllocRenderVec(MemHandler,width*height*4)))
						{			
							for(i=0; i<height; ++i)
							{
								TurboCopyMem(p, t+width*i, width*4);
								p += pic->width;
							}
							FreeRenderVec(pic->array);
							pic->array = (UBYTE *) t;
							success = TRUE;
						}
					}
					else
					{
						t = (ULONG *) pic->array;
						for(i=0; i<height; ++i)
						{
							TurboCopyMem(p, t+width*i, width*4);
							p += pic->width;
						}
						success = TRUE;
					}
					break;
				}
						
				case PIXFMT_CHUNKY_CLUT:
				{
					int i;
					UBYTE *t, *p = ((UBYTE *) pic->array) + y*pic->width + x;
					
					if (pic->owner)
					{
						if((t = AllocRenderVec(MemHandler,width*height)))
						{			
							for(i=0; i<height; ++i)
							{
								TurboCopyMem(p, t+width*i, width);
								p += pic->width;
							}
							FreeRenderVec(pic->array);
							pic->array = t;
							success = TRUE;
						}
					}
					else
					{
						t = pic->array;
						for(i=0; i<height; ++i)
						{
							TurboCopyMem(p, t+width*i, width);
							p += pic->width;
						}
						success = TRUE;
					}
					break;
				}
			}
		}
	}

	if (success)
	{
		pic->width = width;
		pic->height = height;

		/*
		if(pic->histogram)
		{
			UnLinkSharedHistogram(pic->histogram);
			pic->histogram = NULL;
		}	
		*/

	}

	ReleaseSemaphore(&pic->semaphore);


	return success;
}


/*--------------------------------------------------------------------

	Render (pic,pixelformat,tags)

	- bei pic->pixelformat = RGB wird auf eine ggf. vorhandene Palette
	  gerendert. Wenn keine Palette vorhanden ist, wird eine Palette
	  mit 256 Farben erzeugt.

--------------------------------------------------------------------*/

ULONG PIC_Render(PIC *pic, ULONG pixelformat, TAGLIST tags)
{
	BOOL success = TRUE;

	ObtainSemaphore(&pic->semaphore);

	if ((pic->pixelformat != pixelformat))
	{
		success = FALSE;

		if (pic->directdraw == NULL)
		{
			switch (pic->pixelformat)
			{
				case PIXFMT_0RGB_32:
	
					switch (pixelformat)
					{
						case PIXFMT_CHUNKY_CLUT:
						
							/*
							 *	rgb32 -> chunky
							 */
						
							success = TRUE;
			
							if(!pic->palette)
							{
								success = FALSE;
								if (UpdatePicture(pic))
								{
									if ((pic->palette = CreatePalette(RND_RMHandler, (IPTR)MemHandler,
										RND_HSType, pic->hstype, TAG_DONE)))
									{
										if (ExtractPalette(pic->histogram->histogram, 
											pic->palette, 256, NULL) == EXTP_SUCCESS)
										{
											success = TRUE;
										}
									}
								}
							}
	
							if (success)
							{
								success = ExtractAlphaArray(pic);
							}
			
							if (success)
							{
								success = FALSE;
								if(pic->owner)
								{
									UBYTE *t;
									
									if ((t = AllocRenderVec(MemHandler, pic->width*pic->height)))
									{
										if (Render((APTR)pic->array, pic->width, pic->height, 
											t, pic->palette, NULL) == REND_SUCCESS)
										{
											success = TRUE;
											pic->directdraw = NULL;		//!!
											FreeRenderVec(pic->array);
											pic->array = t;
										}
										else
										{
											FreeRenderVec(t);
										}
									}
								}
								else
								{
									if (Render((APTR)pic->array, pic->width, pic->height, pic->array,
										pic->palette, NULL) == REND_SUCCESS)
									{
										pic->directdraw = NULL;		//!!
										success = TRUE;
									}
								}
							}
							
							if (!success)
							{
								if (pic->palette)
								{
									DeletePalette(pic->palette);
									pic->palette = NULL;
								}				
							}
							break;
	
						case PIXFMT_RGB_24:
						
							/*
							 *	rgb32->rgb24
							 */
					
							success = ExtractAlphaArray(pic);
							
							if (success)
							{
								ULONG i;
								UBYTE *p24 = pic->array;
								UBYTE *p32 = pic->array;
								
								for(i = 0; i < pic->width * pic->height; ++i)
								{
									*p24++ = p32[1];
									*p24++ = p32[2];
									*p24++ = p32[3];
									p32 += 4;
								}	
							}
	
							break;
					}
					break;
	
				case PIXFMT_CHUNKY_CLUT:
	
					switch (pixelformat)
					{
						case PIXFMT_0RGB_32:
	
							/*
							 *	chunky -> rgb32
							 */
	
							if (pic->owner)
							{
								ULONG *t;
								if((t = AllocRenderVec(MemHandler, pic->width*pic->height*4)))
								{
									Chunky2RGB(pic->array,pic->width,pic->height,t,pic->palette,NULL);
									
									FreeRenderVec(pic->array);
									DeletePalette(pic->palette);
									pic->palette = NULL;
									pic->array = (UBYTE *) t;
									pic->directdraw = NULL;		//!!
									success = TRUE;
								}
							}
							else
							{
								if(pic->width*pic->height*4 <= pic->maxbytes)
								{
									ULONG *t;
									if((t = AllocRenderVec(MemHandler, pic->width*pic->height*4)))
									{
										Chunky2RGB(pic->array,pic->width,pic->height,t,pic->palette,NULL);
										TurboCopyMem(t, pic->array, pic->width*pic->height*4);
										FreeRenderVec(t);
										DeletePalette(pic->palette);
										pic->palette = NULL;
										pic->directdraw = NULL;		//!!
										success = TRUE;
									}
								}
							}
							
							if (success)
							{
								pic->pixelformat = pixelformat; 	/* wichtig! */
								success = InsertAlphaArray(pic);
							}
							
							break;
	
						case PIXFMT_RGB_24:
						
							/*
							 *	chunky->rgb24
							 */
	
							if (PIC_Render(pic, PIXFMT_0RGB_32, tags))
							{
								if (PIC_Render(pic, PIXFMT_RGB_24, tags))
								{
									success = TRUE;
								}
							}
							break;
					}
					break;
	
				case PIXFMT_RGB_24:
					switch (pixelformat)
					{
						case PIXFMT_0RGB_32:
						{
							/*
							 *	RGB24 -> RGB32
							 */
	
							ULONG i;
							UBYTE *p24, *p32;
							
							if (pic->width * pic->height * 4 <= pic->maxbytes)
							{
								p24 = pic->array + pic->width * pic->height * 3;
								p32 = pic->array + pic->width * pic->height * 4;
								for(i = 0; i < pic->width * pic->height; ++i)
								{
									p32 -= 4;
									p32[3] = *--p24;
									p32[2] = *--p24;
									p32[1] = *--p24;
									p32[0] = 0;
								}
								success = TRUE;
							}
							else
							{
								success = FALSE;
								if (pic->owner)
								{
									if ((p32 = AllocRenderVecClear(MemHandler, pic->width * pic->height * 4)))
									{
										UBYTE *newarray = p32;
										p24 = pic->array;
										for(i = 0; i < pic->width * pic->height; ++i)
										{
											p32[1] = *p24++;
											p32[2] = *p24++;
											p32[3] = *p24++;
											p32 += 4;
										}
										FreeRenderVec(pic->array);
										pic->array = newarray;
										success = TRUE;
									}
								}
							}
							
							if (success)
							{
								pic->pixelformat = pixelformat; 	/* wichtig! */
								success = InsertAlphaArray(pic);
							}
							break;
						}
	
						case PIXFMT_CHUNKY_CLUT:
						
							/*
							 *	rgb24->chunky
							 */
							
							if (PIC_Render(pic, PIXFMT_0RGB_32, tags))
							{
								if (PIC_Render(pic, PIXFMT_CHUNKY_CLUT, tags))
								{
									success = TRUE;
								}
							}
							break;
					}
					break;
			}
		}

		if (success)
		{
			pic->pixelformat = pixelformat;
			
			/*
			if(pic->histogram)
			{
				UnLinkSharedHistogram(pic->histogram);
				pic->histogram = NULL;
			}
			*/
		}
	}


	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*--------------------------------------------------------------------

	Scale (width,height,tags)

--------------------------------------------------------------------*/

ULONG PIC_Scale(PIC *pic, ULONG width, ULONG height,
	TAGLIST tags)
{
	BOOL success = TRUE;

	ObtainSemaphore(&pic->semaphore);

	if ((pic->width != width) || (pic->height != height))
	{
		if ((success = PrepareDrawing(pic)))
		{
			if ((success = InsertAlphaArray(pic)))
			{
				APTR scaleengine;
				success = FALSE;
			
				if ((scaleengine = CreateScaleEngine(pic->width, pic->height, width, height,
					RND_PixelFormat, pic->pixelformat,
					RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
				{
					if (pic->owner)
					{
						APTR t;
						if ((t = AllocRenderVec(MemHandler, 
							width*height*PIXELSIZE(pic->pixelformat))))
						{
							Scale(scaleengine, pic->array, t, NULL);
							FreeRenderVec(pic->array);
							pic->array = t;
							success = TRUE;
						}
					}
					else
					{
						if (width*height*PIXELSIZE(pic->pixelformat) <= pic->maxbytes)
						{
							if(width <= pic->width)
							{
								Scale(scaleengine, pic->array, pic->array, NULL);
								success = TRUE;
							}
							else
							{
								APTR t;
								if ((t = AllocRenderVec(MemHandler,
									width*height*PIXELSIZE(pic->pixelformat))))
								{
									Scale(scaleengine, pic->array, t, NULL);
									TurboCopyMem(t, pic->array, width*height*PIXELSIZE(pic->pixelformat));
									FreeRenderVec(t);
									success = TRUE;
								}					
							}
						}
					}
					DeleteScaleEngine(scaleengine);
				}
			}
		}
	}

	if (success)
	{
		pic->width = width;
		pic->height = height;

		/*
		if(pic->histogram)
		{
			UnLinkSharedHistogram(pic->histogram);
			pic->histogram = NULL;
		}
		*/	
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}



/*---------------------------------------------------------------------

	mixrgb

	mixt eine Zeile RGB auf einen RGB-Buffer

--------------------------------------------------------------------*/

struct mixrgbdata
{
	UWORD width, ratio;
	ULONG *destbuffer;
	UWORD totalwidth;
};

#ifdef __MORPHOS__
FUNC_HOOK(ULONG, mixrgb, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG mixrgb(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS mixrgb(	register __a0 struct Hook *hook,
							register __a2 APTR buffer,
							register __a1 struct RND_LineMessage *msg )
#endif
{
	if (msg->RND_LMsg_type == LMSGTYPE_LINE_RENDERED)
	{
		struct mixrgbdata *args = (struct mixrgbdata *)(hook->h_Data);

		MixRGBArray((ULONG *)buffer, args->width, 1, args->destbuffer, args->ratio, NULL);

		args->destbuffer += args->totalwidth;
	}

	return TRUE;
}


/*---------------------------------------------------------------------

	mixchunkyrgb

	mixt eine Zeile Chunky auf einen RGB-Buffer

--------------------------------------------------------------------*/

struct mixchunkyrgbdata
{
	UWORD width, ratio;
	ULONG *destbuffer;
	ULONG *rgblinebuffer;
	APTR palette;
	UWORD totalwidth;
};
		
#ifdef __MORPHOS__
FUNC_HOOK(ULONG, mixchunkyrgb, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG mixchunkyrgb(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS mixchunkyrgb(	register __a0 struct Hook *hook,
								register __a2 APTR buffer,
								register __a1 struct RND_LineMessage *msg )
#endif
{
	if (msg->RND_LMsg_type == LMSGTYPE_LINE_RENDERED)
	{
		struct mixchunkyrgbdata *args = (struct mixchunkyrgbdata *) (hook->h_Data);

		Chunky2RGB(buffer, args->width, 1, args->rgblinebuffer, args->palette, NULL);
		MixRGBArray(args->rgblinebuffer, args->width, 1, args->destbuffer, args->ratio, NULL);

		args->destbuffer += args->totalwidth;
	}

	return TRUE;
}


/*--------------------------------------------------------------------

	Mix (pic1,pic2,tags)

	- es wird pic1 + pic2 -> pic1 gemischt, also das Fremdbild
	  auf das Bild gemischt, auf das die Methode angewandt wird.

	- das Fremdbild wird temporär umgerechnet und skaliert,
	  falls erforderlich.

	Tags:
		GGFX_Ratio (0-255)	Default: 128

--------------------------------------------------------------------*/

ULONG PIC_Mix(PIC *pic, PIC *mixpic, TAGLIST tags)
{
	BOOL success = TRUE;

	UWORD destwidth, destheight, destx, desty;
	UWORD sourcewidth, sourceheight, sourcex, sourcey;

	ObtainSemaphore(&pic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);
	
	sourcewidth = GetTagData(GGFX_SourceWidth, mixpic->width, tags);
	sourceheight = GetTagData(GGFX_SourceHeight, mixpic->height, tags);
	sourcex = GetTagData(GGFX_SourceX, 0, tags);
	sourcey = GetTagData(GGFX_SourceY, 0, tags);
	

	/*
	 *	clipping

	sourcewidth = MIN(mixpic->width - sourcex, sourcewidth);
	sourceheight = MIN(mixpic->height - sourcey, sourceheight);
	destwidth = MIN(pic->width - destx, destwidth);
	destheight = MIN(pic->height - desty, destheight);

	 */


	if (sourcewidth && sourceheight && destwidth && destheight)
	{
		success = FALSE;
	
		if (PIC_Render(pic, PIXFMT_0RGB_32, NULL))
		{
			APTR scaleengine = NULL;
			success = TRUE;
		
			ObtainSemaphoreShared(&mixpic->semaphore);
		
			if ((sourcewidth != destwidth) || (sourceheight != destheight))
			{
				success = FALSE;
				if ((scaleengine = CreateScaleEngine(sourcewidth, sourceheight,
					destwidth, destheight, RND_PixelFormat, mixpic->pixelformat,
					RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
				{
					success = TRUE;
				}		
			}			
		
			if (success)
			{
				ULONG *dest = ((ULONG *) pic->array) + destx + desty * pic->width;
				success = FALSE;
		
				switch (mixpic->pixelformat)
				{
					case PIXFMT_0RGB_32:
					{
						ULONG *source = ((ULONG *) mixpic->array) + sourcex + sourcey * mixpic->width;
						if (scaleengine)
						{
							APTR linebuffer;
				
							if ((linebuffer = AllocRenderVec(MemHandler, destwidth*4)))
							{
								struct Hook drawhook;
								struct mixrgbdata args;
		
								args.width = destwidth;
								args.totalwidth = pic->width;
								args.ratio = GetTagData(GGFX_Ratio, 128, tags);
								args.destbuffer = dest;
								drawhook.h_Data = &args;
#ifdef __MORPHOS__
								drawhook.h_Entry = (HOOKFUNC) &mixrgb;
#elif defined(__AROS__)
    	    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
								drawhook.h_SubEntry = (HOOKFUNC) mixrgb;
#else
								drawhook.h_Entry = (HOOKFUNC) mixrgb;
#endif
		
								Scale(scaleengine, source, 
									linebuffer, RND_SourceWidth, mixpic->width,
									RND_LineHook, (IPTR)&drawhook, RND_DestWidth, 0, TAG_DONE);
		
								FreeRenderVec(linebuffer);
		
								success = TRUE;
							}
						}
						else
						{
							MixRGBArray(source, sourcewidth, sourceheight, dest,
								GetTagData(GGFX_Ratio, 128, tags), 
								RND_SourceWidth, mixpic->width,
								RND_DestWidth, pic->width, NULL);
						}
						break;
		
					}				
					case PIXFMT_CHUNKY_CLUT:
					{
						UBYTE *source = ((UBYTE *) mixpic->array) + sourcex + sourcey * mixpic->width;
		
						if (scaleengine)
						{
							UBYTE *buffer;
				
							if ((buffer = AllocRenderVec(MemHandler, destwidth*5)))
							{
								struct Hook drawhook;
								struct mixchunkyrgbdata args;
		
								args.width = destwidth;
								args.totalwidth = pic->width;
								args.ratio = GetTagData(GGFX_Ratio, 128, tags);
								args.destbuffer = dest;
								args.palette = mixpic->palette;
								args.rgblinebuffer = (ULONG *)buffer;
								drawhook.h_Data = &args;
#ifdef __MORPHOS__
								drawhook.h_Entry = (HOOKFUNC) &mixchunkyrgb;
#elif defined(__AROS__)
    	    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
								drawhook.h_SubEntry = (HOOKFUNC)mixchunkyrgb;
#else
								drawhook.h_Entry = (HOOKFUNC) mixchunkyrgb;
#endif
		
								Scale(scaleengine, source, buffer+destwidth*4,
									RND_LineHook, (IPTR)&drawhook, RND_DestWidth, 0,
									RND_SourceWidth, mixpic->width, TAG_DONE);
		
								FreeRenderVec(buffer);
		
								success = TRUE;
							}
						}
						else
						{
							APTR buffer;
							if ((buffer = AllocRenderVec(MemHandler, destwidth*4)))
							{
								struct Hook drawhook;
								struct mixrgbdata args;
		
								args.width = sourcewidth;
								args.totalwidth = pic->width;
								args.ratio = GetTagData(GGFX_Ratio, 128, tags);
								args.destbuffer = dest;
								drawhook.h_Data = &args;
#ifdef __MORPHOS__
								drawhook.h_Entry = (HOOKFUNC) &mixrgb;
#elif defined(__AROS__)
    	    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
								drawhook.h_SubEntry = (HOOKFUNC)mixrgb;
#else
								drawhook.h_Entry = (HOOKFUNC) mixrgb;
#endif
		
								Chunky2RGB(source, sourcewidth, sourceheight, buffer,
									mixpic->palette, RND_DestWidth, 0,
									RND_SourceWidth, mixpic->width,
									RND_LineHook, (IPTR)&drawhook, TAG_DONE);
								FreeRenderVec(buffer);
								
								success = TRUE;
							}
						}
						break;
					}
				}
			}
		
			ReleaseSemaphore(&mixpic->semaphore);
		
			if (scaleengine)
			{
				DeleteScaleEngine(scaleengine);
			}
		}
	}
	
	ReleaseSemaphore(&pic->semaphore);

	return success;
}




/*--------------------------------------------------------------------

	TintAlpha (pic1, RGB, tags)

	Tags:
		GGFX_DestWidth
		GGFX_DestHeight
		GGFX_DestX
		GGFX_DestY

--------------------------------------------------------------------*/

ULONG PIC_TintAlpha(PIC *pic, ULONG rgb, TAGLIST tags)
{
	BOOL success = FALSE;

	UWORD destwidth, destheight, destx, desty;

	ObtainSemaphore(&pic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);

	if (destwidth && destheight)
	{
		if (PrepareDrawing(pic))
		{
			if (InsertAlphaArray(pic))
			{
				ULONG *dest = ((ULONG *) pic->array) + destx + desty * pic->width;
				ULONG *mixline;
				
			#ifdef GUIGFX_LITTLE_ENDIAN
				rgb = AROS_LONG2BE(rgb);
			#endif
				
				if ((mixline = AllocRenderVec(MemHandler, destwidth*4)))
				{
					int i;
					for (i = 0; i < destwidth; ++i)
					{
						mixline[i] = rgb;
					}
			
					ApplyAlphaChannel(mixline, destwidth, destheight, dest,
						RND_SourceWidth, 0,
						RND_DestWidth, pic->width,
						RND_AlphaChannel, (IPTR)dest,
						RND_AlphaWidth, pic->width,
						TAG_DONE);
				

					FreeRenderVec(mixline);

					success = TRUE;
				}
			}
		}
	}

	
	ReleaseSemaphore(&pic->semaphore);

	return success;
}



/*--------------------------------------------------------------------

	Tint (pic1,RGB,tags)

	Tags:
		GGFX_Ratio (0-255)	Default: 128
		GGFX_DestWidth
		GGFX_DestHeight
		GGFX_DestX
		GGFX_DestY

--------------------------------------------------------------------*/

ULONG PIC_Tint(PIC *pic, ULONG rgb, TAGLIST tags)
{
	BOOL success = TRUE;

	UWORD destwidth, destheight, destx, desty;

	ObtainSemaphore(&pic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);

	if (destwidth && destheight)
	{
		success = FALSE;
		
		if (PIC_Render(pic, PIXFMT_0RGB_32, NULL))
		{
			ULONG *dest = ((ULONG *) pic->array) + destx + desty * pic->width;
		
			TintRGBArray(dest, destwidth, destheight, rgb,
				GetTagData(GGFX_Ratio, 128, tags), dest,
				RND_SourceWidth, pic->width,
				RND_DestWidth, pic->width, TAG_DONE);
	
			success = TRUE;
		}
	}

	
	ReleaseSemaphore(&pic->semaphore);

	return success;
}



/*---------------------------------------------------------------------

	mixrgbalpha

	mixt eine Zeile RGB auf einen RGB-Buffer mit Alpha-Channel

--------------------------------------------------------------------*/

struct mixalphadata
{
	UWORD width, ratio;
	ULONG *destbuffer;
	UWORD totalwidth_dest;
	BOOL alphachannel_source;
	BOOL alphachannel_dest;
};

#ifdef __MORPHOS__
FUNC_HOOK(ULONG, mixrgbalpha, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG mixrgbalpha(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS mixrgbalpha(	register __a0 struct Hook *hook,
								register __a2 APTR buffer,
								register __a1 struct RND_LineMessage *msg )
#endif
{
	if (msg->RND_LMsg_type == LMSGTYPE_LINE_RENDERED)
	{
		struct mixalphadata *args = (struct mixalphadata *)(hook->h_Data);

		MixAlphaChannel((ULONG *) buffer, args->destbuffer, args->width, 1,	args->destbuffer,
				RND_AlphaChannel, args->alphachannel_source ? (ULONG) buffer : NULL,
				RND_AlphaChannel2, args->alphachannel_dest ? (ULONG) args->destbuffer : NULL,
				TAG_DONE);

		args->destbuffer += args->totalwidth_dest;
	}

	return TRUE;
}


/*--------------------------------------------------------------------

	MixAlpha (pic1,pic2,tags)

	- es wird pic1 + pic2 -> pic1 gemischt, also das Fremdbild
	  auf das Bild gemischt, auf das die Methode angewandt wird.

	- es werden alle etwaigen Alpha-Channels berücksichtigt. Ist
	  gar kein Alpha-Channel vorhanden, wird 50:50 gemischt.

	GGFX_DestWidth	Zielbreite im aktuellen Bild
	GGFX_DestHeight	Zielhöhe im aktuellen Bild
	GGFX_DestX		Ziel-X im aktuellen Bild
	GGFX_DestY		Ziel-Y im aktuellen Bild

	GGFX_SourceWidth	Breite im zweiten Bild
	GGFX_SourceHeight	Höhe im zweiten Bild
	GGFX_SourceX		Ursprung-X im zweiten Bild
	GGFX_SourceY		Ursprung-Y im zweiten Bild

--------------------------------------------------------------------*/

ULONG PIC_MixAlpha(PIC *pic, PIC *mixpic, TAGLIST tags)
{
	BOOL success = FALSE;

	ObtainSemaphore(&pic->semaphore);

	if (InsertAlphaArray(pic))
	{
		ObtainSemaphore(&mixpic->semaphore);

		if (InsertAlphaArray(mixpic))
		{
			if (PIC_Render(pic, PIXFMT_0RGB_32, NULL))
			{
				if (PIC_Render(mixpic, PIXFMT_0RGB_32, NULL))
				{
					UWORD sourcewidth, sourceheight, sourcex, sourcey;
					UWORD destwidth, destheight, destx, desty;
					APTR scaleengine = NULL;
		
					destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
					destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
					destx = GetTagData(GGFX_DestX, 0, tags);
					desty = GetTagData(GGFX_DestY, 0, tags);

					sourcewidth = GetTagData(GGFX_SourceWidth, mixpic->width, tags);
					sourceheight = GetTagData(GGFX_SourceHeight, mixpic->height, tags);
					sourcex = GetTagData(GGFX_SourceX, 0, tags);
					sourcey = GetTagData(GGFX_SourceY, 0, tags);


					/*
					 *	clipping
			
					sourcewidth = MIN(mixpic->width - sourcex, sourcewidth);
					sourceheight = MIN(mixpic->height - sourcey, sourceheight);
					destwidth = MIN(pic->width - destx, destwidth);
					destheight = MIN(pic->height - desty, destheight);

					 */

					success = TRUE;
			
					if (sourcewidth && sourceheight && destwidth && destheight)
					{
						if ((destwidth != sourcewidth) || (destheight != sourceheight))
						{
							success = FALSE;
							if ((scaleengine = CreateScaleEngine(sourcewidth, sourceheight,
								destwidth, destheight,
								RND_PixelFormat, PIXFMT_0RGB_32,
								RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
							{
								success = TRUE;
							}		
						}			
				
						if (success)
						{
							ULONG *source = ((ULONG *) mixpic->array) + sourcex + sourcey * mixpic->width;
							ULONG *dest = ((ULONG *) pic->array) + destx + desty * pic->width;
							success = FALSE;
				
							if (scaleengine)
							{
								APTR linebuffer;
							
								if ((linebuffer = AllocRenderVec(MemHandler, destwidth*4)))
								{
									struct Hook drawhook;
									struct mixalphadata args;
				
									args.width = destwidth;
									args.totalwidth_dest = pic->width;
									args.destbuffer = dest;
									args.alphachannel_source = mixpic->alphapresent;
									args.alphachannel_dest = pic->alphapresent;
									drawhook.h_Data = &args;
#ifdef __MORPHOS__
									drawhook.h_Entry = (HOOKFUNC) &mixrgbalpha;
#elif defined(__AROS__)
    	    	    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
									drawhook.h_SubEntry = (HOOKFUNC) mixrgbalpha;
#else
									drawhook.h_Entry = (HOOKFUNC) mixrgbalpha;
#endif
				
									Scale(scaleengine, (APTR) source, linebuffer,
										RND_LineHook, (IPTR)&drawhook, RND_SourceWidth, mixpic->width,
										RND_DestWidth, 0, TAG_DONE);
				
									FreeRenderVec(linebuffer);
				
									success = TRUE;
								}
							}
							else
							{
								MixAlphaChannel(source, dest, destwidth, destheight, dest,
									RND_SourceWidth, mixpic->width,
									RND_SourceWidth2, pic->width,
									RND_DestWidth, pic->width,
									RND_AlphaChannel, mixpic->alphapresent ? (IPTR) source : NULL,
									RND_AlphaChannel2, pic->alphapresent ? (IPTR) dest : NULL,
									RND_AlphaWidth, mixpic->width,
									RND_AlphaWidth2, pic->width,
									TAG_DONE);
							}
						}
				
						if (scaleengine)
						{
							DeleteScaleEngine(scaleengine);
						}
					}
				}
			}
		}
		ReleaseSemaphore(&mixpic->semaphore);
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*--------------------------------------------------------------------

	MapDrawHandle (pic, drawhandle, tags)

	mappt ein Bild auf ein Drawhandle, das heißt, Format
	und Farben werden angepaßt. Damit kann ein Bild sehr schnell
	in den entsprechenden Drawhandle gezeichnet werden.

--------------------------------------------------------------------*/

ULONG PIC_MapDrawHandle(PIC *pic, struct DrawHandle *dh,
	TAGLIST tags)
{
	BOOL success = FALSE;

	ObtainSemaphore(&pic->semaphore);

	if (dh)
	{
		if (dh->rasthandle->truecolor)
		{
			success = PIC_Render(pic, PIXFMT_0RGB_32, NULL);
		}
		else
		{
			#ifndef NDEBUG
			if (!(dh->realpalette && dh->mainpentab))
			{
				DB(kprintf("*** MapDrawHandle Panic!!!\n"));
			}
			else
			{
			#endif
		
			success = PIC_RenderPalette(pic, dh->realpalette, 
					GGFX_PaletteFormat, PALFMT_PALETTE,
					GGFX_PenTable, dh->mainpentab, TAG_DONE);

			#ifndef NDEBUG
			}
			#endif
		}
	}


	if (success)
	{
		pic->directdraw = dh;
	}


	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*--------------------------------------------------------------------

	RenderPalette (pic, palette, tags)

	rendert ein Picture auf PIXFMT_CHUNKY_CLUT mit angegebener
	Palette und Pentab und setzt die Palette. pentab darf NULL sein,
	palette nicht.

	GGFX_PenTable
	GGFX_PaletteFormat
	GGFX_NumColors

--------------------------------------------------------------------*/

#ifndef __MORPHOS__
ULONG PIC_RenderPalette(PIC *pic, APTR palette, Tag tag1, ...)
{
	return PIC_RenderPaletteA(pic, palette, (TAGLIST) &tag1);	
}
#endif

ULONG PIC_RenderPaletteA(PIC *pic, APTR palette, TAGLIST tags)
{
	UBYTE *pentab;
	BOOL success = FALSE;
	ULONG palfmt;
	
	APTR newpalette;


	ObtainSemaphore(&pic->semaphore);


	pentab = (UBYTE *) GetTagData(GGFX_PenTable, NULL, tags);
	palfmt = GetTagData(GGFX_PaletteFormat, PALFMT_RGB8, tags);


	if ((newpalette = CreatePalette(RND_HSType, 
		pic->hstype == HSTYPE_UNDEFINED ? DEFAULT_PALETTE_HSTYPE : pic->hstype,
		RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
	{
		ULONG numcolors;
		int i;

		if (palfmt == PALFMT_PALETTE)
		{
			if ((numcolors = GetPaletteAttrs(palette, NULL)))
			{
				ULONG rgb;
				for (i = 0; i < numcolors; ++i)
				{
					ExportPalette(palette, &rgb, RND_FirstColor, i,
						RND_NumColors, 1, TAG_DONE);
					ImportPalette(newpalette, &rgb, 1, RND_FirstColor, i,
						RND_NewPalette, FALSE, TAG_DONE);
				}
				success = TRUE;
			}
		}
		else
		{
			if ((numcolors = GetTagData(GGFX_NumColors, 0, tags)))
			{
				ULONG *palptr;
				for (i = 0; i < numcolors; ++i)
				{
					switch (palfmt)
					{
						case PALFMT_RGB32:
							palptr = ((ULONG *)palette) + 3 * i;
							break;
	
						case PALFMT_RGB4:
							palptr = (ULONG *) (((UWORD *)palette) + i);
							break;

						default:
						case PALFMT_RGB8:
							palptr = ((ULONG *)palette) + i;
							break;
					}								
					ImportPalette(newpalette, palptr, 1, RND_PaletteFormat, palfmt,
						RND_FirstColor, i, RND_NewPalette, FALSE, TAG_DONE);
				}
				success = TRUE;
			}
		}
	}


	if (success)
	{
		success = FALSE;

		if (PrepareDrawing(pic))
		{
			if (!pic->directdraw)
			{
				if (pic->pixelformat == PIXFMT_0RGB_32)
				{
					if (ExtractAlphaArray(pic))
					{
						if (Render((APTR)pic->array, pic->width, pic->height,
							pic->array, newpalette, RND_PenTable, (IPTR)pentab, TAG_DONE) == REND_SUCCESS)
						{
							success = TRUE;
						}
					}
				}
				else
				{
					if (ConvertChunky(pic->array, pic->palette, pic->width, pic->height,
						pic->array, newpalette, RND_PenTable, (IPTR)pentab, TAG_DONE) == CONV_SUCCESS)
					{
						success = TRUE;
					}
				}
			}
		}
	}

	if (success)
	{
		pic->pixelformat = PIXFMT_CHUNKY_CLUT;

		if (pic->palette)
		{
			DeletePalette(pic->palette);
		}

/*
		if (pentab)
		{
			int i;
		
			for (i = 0; i < 256; ++i)
			{
				pic->pentab[i] = pentab[i];
			}
			pic->pentabptr = pentab;
		}
*/

		pic->palette = newpalette;
	}
	else
	{
		if (newpalette)
		{
			DeletePalette(newpalette);
		}
	}



	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*--------------------------------------------------------------------

	Texture (pic1,pic2,*coordinates,tags)

	Tags:
		GGFX_DestWidth			(Ziel)
		GGFX_DestHeight
		GGFX_DestX
		GGFX_DestY

		GGFX_SourceWidth		(Textur)
		GGFX_SourceHeight
		GGFX_SourceX
		GGFX_SourceY

--------------------------------------------------------------------*/

ULONG PIC_Texture(PIC *pic, PIC *texpic, WORD *coords, TAGLIST tags)
{
	BOOL success = TRUE;

	UWORD destwidth, destheight, destx, desty;
	UWORD sourcewidth, sourceheight, sourcex, sourcey;

	ObtainSemaphore(&pic->semaphore);
	ObtainSemaphore(&texpic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);

	sourcewidth = GetTagData(GGFX_SourceWidth, texpic->width, tags);
	sourceheight = GetTagData(GGFX_SourceHeight, texpic->height, tags);
	sourcex = GetTagData(GGFX_SourceX, 0, tags);
	sourcey = GetTagData(GGFX_SourceY, 0, tags);

	if (PrepareDrawing(pic))
	{
		if (PrepareDrawing(texpic))
		{
			if (pic->pixelformat != texpic->pixelformat)
			{
				success = FALSE;
		
				if (texpic->pixelformat == PIXFMT_CHUNKY_CLUT)
				{
					success = PIC_Render(texpic, PIXFMT_0RGB_32, NULL);
				}	
		
				if (!success)
				{
					if (pic->pixelformat == PIXFMT_CHUNKY_CLUT)
					{
						success = PIC_Render(pic, PIXFMT_0RGB_32, NULL);
					}	
				}
		
				if (!success)
				{
					if (texpic->pixelformat == PIXFMT_0RGB_32)
					{
						success = PIC_RenderPalette(texpic, pic->palette,
							GGFX_PaletteFormat, PALFMT_PALETTE, TAG_DONE);
					}	
				}
		
				if (!success)
				{
					if (pic->pixelformat == PIXFMT_0RGB_32)
					{
						success = PIC_RenderPalette(pic, texpic->palette,
							GGFX_PaletteFormat, PALFMT_PALETTE, TAG_DONE);
					}
				}
			}
		}
	}

	if (success)
	{
		APTR texengine;
		UBYTE *source = (UBYTE *)texpic->array + (sourcex + sourcey * texpic->width) * PIXELSIZE(texpic->pixelformat);
		UBYTE *dest = (UBYTE *)pic->array + (destx + desty * pic->width) * PIXELSIZE(pic->pixelformat);

		success = FALSE;

		if ((texengine = CreateScaleEngine(sourcewidth, sourceheight, 
			destwidth, destheight, RND_RMHandler, (IPTR)MemHandler,
			RND_DestCoordinates, (IPTR)coords, RND_PixelFormat, pic->pixelformat, TAG_DONE)))
		{
			Scale(texengine, (APTR) source, (APTR) dest, 
				RND_SourceWidth, texpic->width,
				RND_DestWidth, pic->width, TAG_DONE);
			DeleteScaleEngine(texengine);
			success = TRUE;
		}
	}

	
	ReleaseSemaphore(&texpic->semaphore);
	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*--------------------------------------------------------------------

	Set (pic1,RGB,tags)

	Tags:
		GGFX_DestWidth			(Ziel)
		GGFX_DestHeight
		GGFX_DestX
		GGFX_DestY

--------------------------------------------------------------------*/

ULONG PIC_Set(PIC *pic, ULONG rgb, TAGLIST tags)
{
	BOOL success = FALSE;

	UWORD destwidth, destheight, destx, desty;

	ObtainSemaphore(&pic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);

	if (PrepareDrawing(pic))
	{
		switch (pic->pixelformat)
		{
			case PIXFMT_CHUNKY_CLUT:
			{
				UBYTE *p = (UBYTE *)pic->array + (destx + desty * pic->width);

				UBYTE pen;
				
				if (pic->directdraw)
				{
					pen = pic->directdraw->mainpentab[BestPen(pic->directdraw->mainpalette, rgb)];
				}
				else
				{
					pen = BestPen(pic->palette, rgb);
				}

				if (destwidth != pic->width)
				{
					int i;
					for (i = 0; i < destheight; ++i)
					{
						TurboFillMem(p, destwidth, pen);
						p += pic->width;
					}
				}
				else
				{
					TurboFillMem(p, destwidth*destheight, pen);
				}
				break;
			}
			
			case PIXFMT_0RGB_32:
			{
				ULONG *p = (ULONG *)pic->array + (destx + desty * pic->width);
				int x,y;
				
			#ifdef GUIGFX_LITTLE_ENDIAN
			    	rgb = AROS_LONG2BE(rgb);
			#endif
				for (y = 0; y < destheight; ++y)
				{
					for (x = 0; x < destwidth; ++x)
					{
						*p++ = rgb;
					}
					p += pic->width - destwidth;
				}
				break;
			}
		}
		success = TRUE;			
	}
	
	ReleaseSemaphore(&pic->semaphore);

	return success;
}



/*--------------------------------------------------------------------

	Insert (destpic, sourcepic, tags)

	Tags:
		GGFX_SourceWidth		(Quelle)
		GGFX_SourceHeight
		GGFX_SourceX
		GGFX_SourceY
		GGFX_DestWidth			(Ziel)
		GGFX_DestHeight
		GGFX_DestX
		GGFX_DestY

--------------------------------------------------------------------*/

ULONG PIC_Insert(PIC *pic, PIC *sourcepic, TAGLIST tags)
{
	BOOL success = FALSE;

	UWORD destwidth, destheight, destx, desty;
	UWORD sourcewidth, sourceheight, sourcex, sourcey;

	ObtainSemaphore(&pic->semaphore);
	ObtainSemaphore(&sourcepic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);

	sourcewidth = GetTagData(GGFX_SourceWidth, sourcepic->width, tags);
	sourceheight = GetTagData(GGFX_SourceHeight, sourcepic->height, tags);
	sourcex = GetTagData(GGFX_SourceX, 0, tags);
	sourcey = GetTagData(GGFX_SourceY, 0, tags);


	if (pic->directdraw && sourcepic->directdraw)
	{
		if (pic->directdraw == sourcepic->directdraw)
		{
			success = TRUE;
		}
	}
	

	if (!success)
	{
		if (PrepareDrawing(pic))
		{
			if (PrepareDrawing(sourcepic))
			{
				PIC_Render(sourcepic, PIXFMT_0RGB_32, NULL);
				PIC_Render(pic, PIXFMT_0RGB_32, NULL);
				
				success = (sourcepic->pixelformat == PIXFMT_0RGB_32 && pic->pixelformat == PIXFMT_0RGB_32);
			}
		}
	}


	if (success)
	{	
		UBYTE *source = (UBYTE *)sourcepic->array + (sourcex + sourcey * sourcepic->width) * PIXELSIZE(sourcepic->pixelformat);
		UBYTE *dest = (UBYTE *)pic->array + (destx + desty * pic->width) * PIXELSIZE(pic->pixelformat);
		success = FALSE;

		if (destwidth != sourcewidth || destheight != sourceheight)
		{
			APTR scaleengine;

			if ((scaleengine = CreateScaleEngine(sourcewidth, sourceheight,
						destwidth, destheight, RND_RMHandler, (IPTR)MemHandler, 
						RND_PixelFormat, pic->pixelformat, TAG_DONE)))
			{
				success = (Scale(scaleengine, source, dest,
								RND_SourceWidth, sourcepic->width,
								RND_DestWidth, pic->width, TAG_DONE) == CONV_SUCCESS);
			
				DeleteScaleEngine(scaleengine);
			}
		}
		else
		{
			int i;
			int pixelsize = PIXELSIZE(pic->pixelformat);

			for (i = 0; i < destheight; ++i)
			{
				TurboCopyMem(source, dest, pixelsize * destwidth);
				source += pixelsize * sourcepic->width;
				dest += pixelsize * pic->width;
			}

			success = TRUE;
		}

	}

	ReleaseSemaphore(&sourcepic->semaphore);
	ReleaseSemaphore(&pic->semaphore);

	return success;
}


/*--------------------------------------------------------------------

	FlipX (pic, tags)
	FlipY (pic, tags)
	
		GGFX_DestWidth
		GGFX_DestHeight
		GGFX_DestX
		GGFX_DestY

--------------------------------------------------------------------*/

ULONG PIC_FlipX(PIC *pic, TAGLIST tags)
{
	BOOL success = TRUE;

	UWORD destwidth, destheight, destx, desty;

	ObtainSemaphore(&pic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);

	if (PrepareDrawing(pic))
	{
		if (InsertAlphaArray(pic))
		{
			UBYTE *array = (UBYTE *)pic->array + (destx + desty * pic->width) * PIXELSIZE(pic->pixelformat);
			int x, y;
	
			if (pic->pixelformat == PIXFMT_CHUNKY_CLUT)
			{
				UBYTE *s, *d, t;
	
				for (y = 0; y < destheight; ++y)
				{
					s = array;
					d = array + destwidth;
					for (x = 0; x < destwidth/2; ++x)
					{
						t = *s;
						*s++ = *(--d);
						*d = t;
					}
					array += pic->width;
				}		
			}
			else
			{
				ULONG *s, *d, t;
	
				for (y = 0; y < destheight; ++y)
				{
					s = (ULONG *) array;
					d = ((ULONG *) array) + destwidth;
					for (x = 0; x < destwidth/2; ++x)
					{
						t = *s;
						*s++ = *(--d);
						*d = t;
					}
					array += pic->width * 4;
				}		
			}
		}
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}


ULONG PIC_FlipY(PIC *pic, TAGLIST tags)
{
	BOOL success = TRUE;

	UWORD destwidth, destheight, destx, desty;

	ObtainSemaphore(&pic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);

	if (PrepareDrawing(pic))
	{
		if (InsertAlphaArray(pic))
		{
			UBYTE *array = (UBYTE *)pic->array + (destx + desty * pic->width) * PIXELSIZE(pic->pixelformat);
			int x, y;
	
			if (pic->pixelformat == PIXFMT_CHUNKY_CLUT)
			{
				UBYTE *s, *d, t;
	
				for (y = 0; y < destheight/2; ++y)
				{
					s = array + y * pic->width;
					d = array + (destheight - y - 1) * pic->width;
					
					for (x = 0; x < destwidth; ++x)
					{
						t = *s;
						*s++ = *d;
						*d++ = t;
					}
				}		
			}
			else
			{
				ULONG *s, *d, t;

				for (y = 0; y < destheight/2; ++y)
				{
					s = ((ULONG *) array) + y * pic->width;
					d = ((ULONG *) array) + (destheight - y - 1) * pic->width;
					
					for (x = 0; x < destwidth; ++x)
					{
						t = *s;
						*s++ = *d;
						*d++ = t;
					}
				}		
			}
		}
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}

/*--------------------------------------------------------------------

	ULONG CheckDither (pic, drawhandle, tags)

--------------------------------------------------------------------*/

ULONG PIC_CheckDither(PIC *pic, DRAWHANDLE *dh, TAGLIST tags)
{
	BOOL dithers = FALSE;

	ObtainSemaphore(&pic->semaphore);

	if (!dh->rasthandle->truecolor)
	{
		if (PrepareDrawing(pic))
		{
			if (GetDitherMode(dh, pic->array, pic->palette,
				pic->width, pic->height, pic->pixelformat, pic->width, DITHERMODE_FS,
				TRUE /*dh->autodither*/) == DITHERMODE_FS)
			{
				dithers = TRUE;
			}
		}
	}

	ReleaseSemaphore(&pic->semaphore);
	
	return (ULONG) dithers;
}


/*--------------------------------------------------------------------

	Negative (pic, tags)
	
		GGFX_DestWidth
		GGFX_DestHeight
		GGFX_DestX
		GGFX_DestY

--------------------------------------------------------------------*/

ULONG PIC_Negative(PIC *pic, TAGLIST tags)
{
	BOOL success = TRUE;

	UWORD destwidth, destheight, destx, desty;

	ObtainSemaphore(&pic->semaphore);

	destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
	destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
	destx = GetTagData(GGFX_DestX, 0, tags);
	desty = GetTagData(GGFX_DestY, 0, tags);

	if (InsertAlphaArray(pic))
	{
		if (PIC_Render(pic, PIXFMT_0RGB_32, NULL))
		{
			ULONG *p = ((ULONG *) pic->array) + destx + desty * pic->width;
			ULONG *pp, rgb; 
			int x,y;

			for (y = 0; y < destheight; ++y)
			{
				pp = p;

				for (x = 0; x < destwidth; ++x)
				{
					rgb = *pp;
				#ifdef GUIGFX_LITTLE_ENDIAN
					*pp++ = (rgb & 0x000000ff) | (~rgb & 0xffffff00);
				#else
					*pp++ = (rgb & 0xff000000) | (~rgb & 0x00ffffff);
				#endif
				}
				
				p += pic->width;
			}
	
			success = TRUE;	
		}
	}


	ReleaseSemaphore(&pic->semaphore);

	return success;
}






void AutoCutChunky(char *array, int w, int h, int totalwidth,
	int *neww, int *newh, int *xoffs, int *yoffs)
{
	char chunky, *p;
	int i;
//	int x = 0, y = 0;
	
	int left, top, right, bottom;
	int numfound;
	int found = 0;
	
	left = 0;
	top = 0;
	right = w - 1;
	bottom = h - 1;

	do
	{
		numfound = 0;


		//	top

		if (!(found & 1) && h > 2)
		do
		{		
			p = array + left + top * totalwidth;
			chunky = *p;
			i = 0;
			while (i < w)
			{
				if (*p != chunky) break;
				p++;
				i++;			
			}
			if (i == w)
			{
				top++;
				numfound++;
				found |= 1;
				if (--h < 3) break;
			}
		} while (i == w);

		//	right

		if (!(found & 2) && w > 2)
		do
		{	
			p = array + right + top * totalwidth;
			chunky = *p;
			i = 0;
			while (i < h)
			{
				if (*p != chunky) break;
				p += totalwidth;
				i++;			
			}
			if (i == h)
			{
				right--;
				numfound++;
				found |= 2;
				if (--w < 3) break;
			}
		} while (i == h);

		//	bottom

		if (!(found & 4) && h > 2)
		do
		{	
			p = array + left + bottom * totalwidth;
			chunky = *p;
			i = 0;
			while (i < w)
			{
				if (*p != chunky) break;
				p++;
				i++;			
			}
			if (i == w)
			{
				bottom--;
				numfound++;
				found |= 4;
				if (--h < 3) break;
			}
		} while (i == w);

		//	left

		if (!(found & 8) && w > 2)
		do
		{	
			p = array + left + top * totalwidth;
			chunky = *p;
			i = 0;
			while (i < h)
			{
				if (*p != chunky) break;
				p += totalwidth;
				i++;			
			}
			if (i == h)
			{
				left++;
				numfound++;
				found |= 8;
				if (--w < 3) break;
			}
		} while (i == h);

	} while (numfound > 0 && w > 2 && h > 2);

	*neww = right - left + 1;
	*newh = bottom - top + 1;
	*xoffs = left;
	*yoffs = top;
}	


void AutoCutRGB(unsigned long *array, int w, int h, int totalwidth,
	int *neww, int *newh, int *xoffs, int *yoffs)
{
	unsigned long rgb, *p;
	int i;
//	int x = 0, y = 0;
	
	int left, top, right, bottom;
	int numfound;
	int found = 0;
	
	left = 0;
	top = 0;
	right = w - 1;
	bottom = h - 1;

	do
	{
		numfound = 0;


		//	top

		if (!(found & 1) && h > 2)
		do
		{		
			p = array + left + top * totalwidth;
			rgb = *p;
			i = 0;
			while (i < w)
			{
				if (*p != rgb) break;
				p++;
				i++;			
			}
			if (i == w)
			{
				top++;
				numfound++;
				found |= 1;
				if (--h < 3) break;
			}
		} while (i == w);


		//	right

		if (!(found & 2) && w > 2)
		do
		{	
			p = array + right + top * totalwidth;
			rgb = *p;
			i = 0;
			while (i < h)
			{
				if (*p != rgb) break;
				p += totalwidth;
				i++;			
			}
			if (i == h)
			{
				right--;
				numfound++;
				found |= 2;
				if (--w < 3) break;
			}
		} while (i == h);


		//	bottom

		if (!(found & 4) && h > 2)
		do
		{	
			p = array + left + bottom * totalwidth;
			rgb = *p;
			i = 0;
			while (i < w)
			{
				if (*p != rgb) break;
				p++;
				i++;			
			}
			if (i == w)
			{
				bottom--;
				numfound++;
				found |= 4;
				if (--h < 3) break;
			}
		} while (i == w);


		//	left

		if (!(found & 8) && w > 2)
		do
		{	
			p = array + left + top * totalwidth;
			rgb = *p;
			i = 0;
			while (i < h)
			{
				if (*p != rgb) break;
				p += totalwidth;
				i++;			
			}
			if (i == h)
			{
				left++;
				numfound++;
				found |= 8;
				if (--w < 3) break;
			}
		} while (i == h);

	} while (numfound > 0 && w > 2 && h > 2);


	*neww = right - left + 1;
	*newh = bottom - top + 1;
	*xoffs = left;
	*yoffs = top;
}	

/*--------------------------------------------------------------------

	AutoCrop (pic, tags)
	
		GGFX_SourceWidth
		GGFX_SourceHeight
		GGFX_SourceX
		GGFX_SourceY

--------------------------------------------------------------------*/

ULONG PIC_AutoCrop(PIC *pic, TAGLIST tags)
{
	BOOL success = FALSE;

	UWORD sourcewidth, sourceheight, sourcex, sourcey;

	ObtainSemaphore(&pic->semaphore);

	sourcewidth = GetTagData(GGFX_SourceWidth, pic->width, tags);
	sourceheight = GetTagData(GGFX_SourceHeight, pic->height, tags);
	sourcex = GetTagData(GGFX_SourceX, 0, tags);
	sourcey = GetTagData(GGFX_SourceY, 0, tags);

	if (PrepareDrawing(pic))
	{
		if (ExtractAlphaArray(pic))
		{
			int newx, newy, neww, newh;

			switch (pic->pixelformat)
			{
				case PIXFMT_CHUNKY_CLUT:

					AutoCutChunky((char *) pic->array + sourcex + sourcey * pic->width,
						sourcewidth, sourceheight, pic->width,
						&neww, &newh, &newx, &newy);

					break;

				case PIXFMT_0RGB_32:

					AutoCutRGB((ULONG *) pic->array + sourcex + sourcey * pic->width,
						sourcewidth, sourceheight, pic->width,
						&neww, &newh, &newx, &newy);
				
					break;
			}

			PIC_Crop(pic, newx, newy, neww, newh, NULL);
	
			success = TRUE;
		}
	}


	ReleaseSemaphore(&pic->semaphore);

	return success;
}
