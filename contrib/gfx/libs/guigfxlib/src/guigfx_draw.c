/*********************************************************************
----------------------------------------------------------------------

	guigfx_draw

----------------------------------------------------------------------
*********************************************************************/

#include <render/render.h>
#include <render/renderhooks.h>
#include <libraries/cybergraphics.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <graphics/view.h>
#include <intuition/classusr.h>

#include <proto/render.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>

#include <guigfx/guigfx.h>

#include "guigfx_global.h"
#include "guigfx_picturemethod.h"
#include "guigfx_bitmap.h"


#define HOOKDELAY 2

#ifdef __AROS__
#define GetBitMapInfo(bm,displayid,args...) \
({ \
     IPTR __args[] = { args }; \
     GetBitMapInfoA((bm), (displayid), (TAGLIST)__args); \
})
#endif

/*---------------------------------------------------------------------

	Zeichenstrukturen

--------------------------------------------------------------------*/

struct DrawData
{
	APTR array;
	UWORD totalwidth;
	ULONG pixelformat;

	APTR sourcepalette;
	APTR destpalette;
	UBYTE *pentab;

	WORD sourcex, sourcey;
	UWORD sourcewidth, sourceheight;
	
	UWORD destx, desty;
	UWORD destwidth, destheight;

	DRAWHANDLE *dh;
	UWORD dithermode;		/* hat Vorrang vor dem Dithermode im Drawhandle */
	UWORD ditheramount;

	struct Hook *callbackhook;
	PIC *pic;
	
	UWORD colormode;
	
	WORD *coords;
	
	BOOL clearbg;
	
	BOOL autodither;
	
//	ULONG bgcolor;

	BOOL	usescalepixelarray;
	
	struct SignalSemaphore *rastlock;

};

struct drawdata_truecolor
{
	struct RastPort *rp;
	UWORD x, y, width;
	struct Hook *hook;
	PIC *pic;
	ULONG *linebuffer;		// for clearing
	ULONG bgcolor;			// with that color
	struct SignalSemaphore *rastlock;
	WORD hookdelaycount;
};

struct drawdata_chunkytruecolor
{
	struct RastPort *rp;
	UWORD x, y, width;
	APTR palette;
	ULONG *rgbbuffer;
	struct Hook *hook;
	PIC *pic;
	UBYTE *linebuffer;		// for clearing
	UBYTE bgpen;			// with that color
	struct SignalSemaphore *rastlock;
	WORD hookdelaycount;
};

struct drawdata_chunkyclut
{
	struct RastPort *rp, *temprp;
	UWORD x, y, width;
	struct Hook *hook;
	PIC *pic;
	UBYTE *linebuffer;		// for clearing
	UBYTE bgpen;			// with that color
	struct SignalSemaphore *rastlock;
	WORD hookdelaycount;
};

struct drawdata_chunkybitmap
{
	struct BitMap *bm;
	UWORD x, y, width;
	struct Hook *hook;
	PIC *pic;
	UBYTE *linebuffer;		// for clearing
	UBYTE bgpen;			// with that color
	WORD hookdelaycount;
};



/*--------------------------------------------------------------------

		continue = calldrawhook(hook, pic, zeile)

		führt den User-Callback-Hook mit der Message
		GGFX_MSGTYPE_LINEDRAWN für Zeichenoperationen aus.

		Konventionen: die Funktion muß TRUE oder FALSE zurückliefern.

--------------------------------------------------------------------*/

ULONG __inline calldrawhook (struct Hook *hook, PIC *pic, ULONG zeile)
{
	if (hook)
	{
		return CallHook(hook, (Object *) pic,
			(ULONG) GGFX_MSGTYPE_LINEDRAWN, (ULONG) zeile);
	}

	return TRUE;
}


/*--------------------------------------------------------------------

		dithermode = GetDitherMode
		(drawhandle, array, palette, width, height, pixelformat, totalwidth, autodither)

--------------------------------------------------------------------*/

ULONG GetDitherMode(DRAWHANDLE *dh, APTR array, PALETTE palette,
	UWORD width, UWORD height, ULONG pixelformat, UWORD totalwidth, UWORD defaultmode,
	BOOL autodither)
{
	if (dh)
	{
		if (dh->rasthandle->colormode == COLORMODE_CLUT)
		{
			if (autodither && (defaultmode != DITHERMODE_NONE) && !dh->rasthandle->truecolor)
			{
				ULONG d = 0;
		
				switch (pixelformat)
				{
					case PIXFMT_CHUNKY_CLUT:
					
						#ifdef DEBUG
						if (!palette)
						{
							DB(kprintf("*** Error: No palette specified for ChunkyArrayiversity.\n"));
						}
						#endif
					
						d = ChunkyArrayDiversity(array, palette,
								width, height,
								RND_MapEngine, (IPTR)dh->mapengine,
								RND_Palette, (IPTR)dh->realpalette,
								RND_SourceWidth, totalwidth,
								RND_Interleave, DEFAULT_PICTURE_INTERLEAVE, TAG_DONE);
						break;
		
					case PIXFMT_0RGB_32:
						d = RGBArrayDiversity(array,
								width, height,
								RND_MapEngine, (IPTR)dh->mapengine,
								RND_Palette, (IPTR)dh->realpalette,
								RND_SourceWidth, totalwidth,
								RND_Interleave, DEFAULT_PICTURE_INTERLEAVE, TAG_DONE);
						break;
	
					default:
						DB(kprintf("***error: undefined pixelformat in getdithermode\n"));
						break;
				}
	
				DB(kprintf("diversity: %ld  ", d));
				DB(kprintf("threshold: %ld\n", dh->ditherthreshold));
		
				if (d > dh->ditherthreshold)
				{
					return defaultmode;
				}
				else
				{
					return DITHERMODE_NONE;
				}
			}
		}
	}

	return defaultmode;
}


/*---------------------------------------------------------------------

	drawchunkytruecolor

	callback zum Zeichnen von Chunky-Daten auf Truecolor

--------------------------------------------------------------------*/

#ifdef __MORPHOS__
FUNC_HOOK(ULONG, drawchunkytruecolor, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG drawchunkytruecolor(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS drawchunkytruecolor(	register __a0 struct Hook *hook,
										register __a2 APTR buffer,
										register __a1 struct RND_LineMessage *msg )
#endif
{
	ULONG result = TRUE;

	struct drawdata_chunkytruecolor *data = hook->h_Data;

	switch (msg->RND_LMsg_type)
	{
		case LMSGTYPE_LINE_FETCH:
		{
			if (data->linebuffer)
			{
				TurboFillMem(data->linebuffer, data->width, data->bgpen);
			}
			break;
		}

		case LMSGTYPE_LINE_RENDERED:
		{
			Chunky2RGB((UBYTE *)buffer, data->width, 1, data->rgbbuffer, data->palette, NULL);
	
			if (data->rastlock) ObtainSemaphore(data->rastlock);

			WritePixelArray(data->rgbbuffer, 0, 0, data->width, data->rp,
				data->x, data->y, data->width, 1, RECTFMT_ARGB);

			if (data->rastlock) ReleaseSemaphore(data->rastlock);
	
			if (--data->hookdelaycount < 0)
			{
				result = calldrawhook(data->hook, data->pic, data->y);
				data->hookdelaycount = HOOKDELAY;
			}
			else
			{
				result = TRUE;
			}
	
			data->y++;

			break;
		}
	}

	return result;
}


/*---------------------------------------------------------------------

	drawtruecolor

	callback zum Zeichnen von Truecolor auf Truecolor

--------------------------------------------------------------------*/

#ifdef __MORPHOS__
FUNC_HOOK(ULONG, drawtruecolor, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG drawtruecolor(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS drawtruecolor(	register __a0 struct Hook *hook,
									register __a2 APTR buffer,
									register __a1 struct RND_LineMessage *msg )
#endif
{
	ULONG result = TRUE;
	struct drawdata_truecolor *data = hook->h_Data;

	switch (msg->RND_LMsg_type)
	{
		case LMSGTYPE_LINE_FETCH:
		{
			ULONG *p;
			
			if ((p = data->linebuffer))
			{
				int i;
				for (i = 0; i < data->width; ++i)
				{
					*p++ = data->bgcolor;
				}
			}
			break;
		}

		case LMSGTYPE_LINE_RENDERED:
		{
			if (data->rastlock) ObtainSemaphore(data->rastlock);

			WritePixelArray(buffer, 0, 0, data->width, data->rp, data->x, data->y,
				data->width, 1, RECTFMT_ARGB);

			if (data->rastlock) ReleaseSemaphore(data->rastlock);

			if (--data->hookdelaycount < 0)
			{
				result = calldrawhook(data->hook, data->pic, data->y);
				data->hookdelaycount = HOOKDELAY;
			}
			else
			{
				result = TRUE;
			}

			data->y++;
		
			break;
		}
	}

	return result;
}


/*---------------------------------------------------------------------

	drawchunkyclut

	callback zum Zeichnen von Chunky-Daten auf CLUT

--------------------------------------------------------------------*/

#ifdef __MORPHOS__
FUNC_HOOK(ULONG, drawchunkyclut, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG drawchunkyclut(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS drawchunkyclut(	register __a0 struct Hook *hook,
									register __a2 APTR buffer,
									register __a1 struct RND_LineMessage *msg )
#endif
{
	struct drawdata_chunkyclut *data = hook->h_Data;

	ULONG result = TRUE;

	switch (msg->RND_LMsg_type)
	{
		case LMSGTYPE_LINE_FETCH:
		{
			if (data->linebuffer)
			{
				TurboFillMem(data->linebuffer, data->width, data->bgpen);
			}
			break;
		}

		case LMSGTYPE_LINE_RENDERED:
		{
			if (data->rastlock) ObtainSemaphore(data->rastlock);

			WritePixelLine8(data->rp, data->x, data->y, data->width,
					buffer, data->temprp);

			if (data->rastlock) ReleaseSemaphore(data->rastlock);

			if (--data->hookdelaycount < 0)
			{
				result = calldrawhook(data->hook, data->pic, data->y);
				data->hookdelaycount = HOOKDELAY;
			}
			else
			{
				result = TRUE;
			}
	
			data->y++;

			break;
		}
	}

	return result;
}


/*---------------------------------------------------------------------

	drawchunkybitmap

	callback zum Zeichnen von Chunky-Daten
	in eine BMF_STANDARD BitMap

--------------------------------------------------------------------*/

#ifdef __MORPHOS__
FUNC_HOOK(ULONG, drawchunkybitmap, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG drawchunkybitmap(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS drawchunkybitmap	(	register __a0 struct Hook *hook,
										register __a2 APTR buffer,
										register __a1 struct RND_LineMessage *msg )
#endif
{
	ULONG result = TRUE;
	struct drawdata_chunkybitmap *data = hook->h_Data;

	switch (msg->RND_LMsg_type)
	{
		case LMSGTYPE_LINE_FETCH:
		{
			if (data->linebuffer)
			{
				TurboFillMem(data->linebuffer, data->width, data->bgpen);
			}
			break;
		}

		case LMSGTYPE_LINE_RENDERED:
		{
			Chunky2BitMapA(buffer, 0, 0, data->width, 1, data->bm, data->x, data->y, NULL);

			if (--data->hookdelaycount < 0)
			{
				result = calldrawhook(data->hook, data->pic, data->y);
				data->hookdelaycount = HOOKDELAY;
			}
			else
			{
				result = TRUE;
			}
	
			data->y++;

			break;
		}
	}

	return result;
}




/*---------------------------------------------------------------------

	CreateDrawData

	erzeugt eine Drawdata-Struktur

--------------------------------------------------------------------*/

struct DrawData *CreateDrawData(DRAWHANDLE *dh, PIC *pic)
{
	struct DrawData *data;
	
	if ((data = AllocRenderVecClear(MemHandler, sizeof(struct DrawData))))
	{
		data->ditheramount = DEFAULT_DITHERAMOUNT;

		if (pic)
		{
			data->array = pic->array;
			data->totalwidth = pic->width;
			data->pic = pic;
			data->sourcepalette = pic->palette;
			data->sourcewidth = pic->width;
			data->sourceheight = pic->height;
			data->destwidth = pic->width;
			data->destheight = pic->height;
			data->pixelformat = pic->pixelformat;
			data->colormode = COLORMODE_CLUT;
		}
		
		if (dh)
		{
			data->dh = dh;
			data->dithermode = dh->dithermode;
			data->ditheramount = dh->ditheramount;
			data->destpalette = dh->realpalette;
			data->pentab = dh->mainpentab;
			data->colormode = dh->rasthandle->colormode;
		}
	}
	
	return data;
}



/*---------------------------------------------------------------------

	DrawRastHandle

	allgemeine Zeichenfunktion zum Zeichnen in ein RastHandle

--------------------------------------------------------------------*/

BOOL DrawRastHandle(RASTHANDLE *rh, struct DrawData *data)
{
	UBYTE *p;
	APTR scaleengine = NULL;
	BOOL success = TRUE;
	BOOL directdraw;
	BOOL scaling = FALSE;
	PIC *pic = data->pic;


	if (!calldrawhook(data->callbackhook, pic, 0))
	{
		return FALSE;
	}


	if (data->dh && pic)
	{
		directdraw = !!(pic->directdraw == data->dh);
	}
	else
	{
		directdraw = FALSE;
	}


	p = ((UBYTE *) data->array) +
		PIXELSIZE(data->pixelformat) * (data->sourcex + data->sourcey * data->totalwidth);


	if (data->coords ||
		(data->sourcewidth != data->destwidth) || (data->sourceheight != data->destheight))
	{
		ULONG pixelformat = data->pixelformat;

		BOOL newengine = TRUE;


		if (!data->coords)
		{
			DB(kprintf("§§§no coordinates\n"));

			if ((data->sourcewidth == data->destwidth) && (data->sourceheight == data->destheight))
			{
				DB(kprintf("§§§no scaling required\n"));

				DeleteScaleEngine(pic->scaleengine);
				pic->scaleengine = NULL;
				newengine = FALSE;
			}

			if (newengine)
			{
				if (rh->truecolor && (pixelformat == PIXFMT_0RGB_32))
				{
					if (data->usescalepixelarray)
					{
						DB(kprintf("§§§no scaling required with cgfx v41\n"));

						DeleteScaleEngine(pic->scaleengine);
						pic->scaleengine = NULL;
						newengine = FALSE;
					}
				}
			}
			
			if (newengine)
			{
				if (pic->scaleengine)
				{
					if (pic->scalepixelformat == pixelformat &&
						pic->scalesourcewidth == data->sourcewidth &&
						pic->scalesourceheight == data->sourceheight &&
						pic->scaledestwidth == data->destwidth &&
						pic->scaledestheight == data->destheight)
					{
						DB(kprintf("§§§ using old scaling engine...\n"));
						newengine = FALSE;
					}
				}
			}
		}

		if (newengine)
		{
			DeleteScaleEngine(pic->scaleengine);

			DB(kprintf("§§§creating scaling engine...\n"));
			
			if ((pic->scaleengine = CreateScaleEngine(data->sourcewidth, data->sourceheight,
				data->destwidth, data->destheight, RND_PixelFormat, pixelformat,
				RND_DestCoordinates, (IPTR)data->coords,
				RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
			{
				pic->scalepixelformat = pixelformat;
				pic->scalesourcewidth = data->sourcewidth;
				pic->scalesourceheight = data->sourceheight;
				pic->scaledestwidth = data->destwidth;
				pic->scaledestheight = data->destheight;
			}
			else
			{
				success = FALSE;
			}
		}

		scaling = TRUE;
		scaleengine = pic->scaleengine;
	}


	if (success)
	{
		UBYTE *linebuffer;
		success = FALSE;

		if (rh->truecolor)
		{
			/*
			 *	auf truecolor zeichnen
			 */

			switch (data->pixelformat)
			{
				case PIXFMT_CHUNKY_CLUT:
				{
					if (scaleengine)
					{
						if ((linebuffer = AllocRenderVec(MemHandler, data->destwidth*5)))
						{
							struct Hook drawhook;
							struct drawdata_chunkytruecolor hookdata;
	
							hookdata.rp = rh->rp;
							hookdata.x = data->destx;
							hookdata.y = data->desty;
							hookdata.width = data->destwidth;
							hookdata.palette = data->sourcepalette;
							hookdata.rgbbuffer = (ULONG *) linebuffer;
							hookdata.hook = data->callbackhook;
							hookdata.pic = pic;
							hookdata.rastlock = data->rastlock;
							hookdata.hookdelaycount = 0;
							if (data->clearbg)
							{
								hookdata.bgpen = BestPen(pic->palette, data->dh->bgcolor);
								hookdata.linebuffer = linebuffer + data->destwidth * 4;
							}
							else
							{
								hookdata.linebuffer = NULL;
							}						
							drawhook.h_Data = &hookdata;
#ifdef __MORPHOS__
							drawhook.h_Entry = (HOOKFUNC) &drawchunkytruecolor;
#elif defined(__AROS__)
    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
							drawhook.h_SubEntry = (HOOKFUNC)drawchunkytruecolor;
#else
							drawhook.h_Entry = (HOOKFUNC) drawchunkytruecolor;
#endif

							DB(kprintf("drawing/scaling chunky to truecolor\n"));
			
							success = !!(Scale(scaleengine, (APTR) p, linebuffer + data->destwidth*4,
								RND_SourceWidth, data->totalwidth, RND_DestWidth, 0,
								RND_LineHook, (IPTR)&drawhook, TAG_DONE) == CONV_SUCCESS);

							FreeRenderVec(linebuffer);
						}
					}
					else
					{
						if ((linebuffer = AllocRenderVec(MemHandler, data->destwidth*4)))
						{
							struct Hook drawhook;
							struct drawdata_truecolor hookdata;
	
							hookdata.rp = rh->rp;
							hookdata.x = data->destx;
							hookdata.y = data->desty;
							hookdata.width = data->destwidth;
							hookdata.hook = data->callbackhook;
							hookdata.pic = pic;
							hookdata.rastlock = data->rastlock;
							hookdata.hookdelaycount = 0;
							if (data->clearbg)
							{
								hookdata.bgcolor = data->dh->bgcolor;
								hookdata.linebuffer = (ULONG *) linebuffer;
							}
							else
							{
								hookdata.linebuffer = NULL;
							}						
							drawhook.h_Data = &hookdata;
#ifdef __MORPHOS__
							drawhook.h_Entry = (HOOKFUNC) &drawtruecolor;
#elif defined(__AROS__)
    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
							drawhook.h_SubEntry = (HOOKFUNC) drawtruecolor;
#else
							drawhook.h_Entry = (HOOKFUNC) drawtruecolor;
#endif

							DB(kprintf("drawing chunky to truecolor\n"));

							success = !!(Chunky2RGB((APTR) p, data->destwidth, data->destheight,
								(ULONG *) linebuffer, data->sourcepalette, RND_LineHook, (IPTR)&drawhook,
								RND_SourceWidth, data->totalwidth,
								RND_DestWidth, 0, TAG_DONE) == CONV_SUCCESS);

							FreeRenderVec(linebuffer);
						}
					}
					break;
				}

				case PIXFMT_0RGB_32:
				{
					if (scaling)
					{
						DB(kprintf("drawing/scaling truecolor to truecolor\n"));

				//		if (cgfx41 && env_usescalepixelarray)
						if (!scaleengine && cgfx41)
						{
					//		success = calldrawhook(NULL, pic, 0);
					//		if (success)
					//		{
								ScalePixelArray((APTR)p, data->sourcewidth, data->sourceheight, 
									data->totalwidth*4, rh->rp, data->destx, data->desty, data->destwidth,
									data->destheight, RECTFMT_ARGB);
					//		}
							success = TRUE;
						}
						else
						{
							if ((linebuffer = AllocRenderVec(MemHandler, data->destwidth*4)))
							{
								struct Hook drawhook;
								struct drawdata_truecolor hookdata;
	
								hookdata.rp = rh->rp;
								hookdata.x = data->destx;
								hookdata.y = data->desty;
								hookdata.width = data->destwidth;
								hookdata.hook = data->callbackhook;
								hookdata.pic = pic;
								hookdata.rastlock = data->rastlock;
								hookdata.hookdelaycount = 0;
								if (data->clearbg)
								{
									hookdata.bgcolor = data->dh->bgcolor;
									hookdata.linebuffer = (ULONG *) linebuffer;
								}
								else
								{
									hookdata.linebuffer = NULL;
								}
								drawhook.h_Data = &hookdata;
#ifdef __MORPHOS__
								drawhook.h_Entry = (HOOKFUNC) &drawtruecolor;
#elif defined(__AROS__)
    	    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
								drawhook.h_SubEntry = (HOOKFUNC)drawtruecolor;
#else
								drawhook.h_Entry = (HOOKFUNC) drawtruecolor;
#endif

								success = !!(Scale(scaleengine, (APTR) p, linebuffer,
									RND_SourceWidth, data->totalwidth, RND_DestWidth, 0,
									RND_LineHook, (IPTR)&drawhook, TAG_DONE) == CONV_SUCCESS);
						
								FreeRenderVec(linebuffer);
							}
						}
					}
					else
					{
						DB(kprintf("drawing truecolor to truecolor\n"));

						WritePixelArray((APTR) data->array, data->sourcex, data->sourcey,
							data->totalwidth*4, rh->rp, data->destx, data->desty,
							data->destwidth, data->destheight, RECTFMT_ARGB);
						success = TRUE;
					}
					break;
				}

				#ifdef DEBUG				
				default:
				{
					DB(kprintf("*** Illegal condition: undefined pixelformat.\n"));
					break;
				}
				#endif
			}
		}
		else
		{
			/*
			 *	auf clut zeichnen
			 */

			switch (data->pixelformat)
			{
				case PIXFMT_CHUNKY_CLUT:
				{
					if (directdraw)
					{
						if (scaleengine)
						{
							/*
							 *	zuerst versuchen, den ganzen Block zu skalieren und zu zeichnen
							 */
							
							if (rh->drawmode == DRAWMODE_CYBERGFX)
							{ 
								UBYTE *buffer;
								if ((buffer = AllocRenderVec(MemHandler, data->destwidth * data->destheight)))
								{
									if (data->clearbg)
									{
										TurboFillMem(buffer, data->destwidth * data->destheight, data->dh->bgpen);
									}
								
									DB(kprintf("scaling/drawing chunky to chunky block-wise in directdraw mode\n"));

									if (Scale(scaleengine, (APTR) p, buffer,
										RND_SourceWidth, data->totalwidth, TAG_DONE) == CONV_SUCCESS)
									{
										WritePixelArray((APTR)buffer, 0, 0, data->destwidth, rh->rp,
											data->destx, data->desty, data->destwidth, data->destheight, RECTFMT_LUT8);

										success = TRUE;
									}
									FreeRenderVec(buffer);
								}
							}
							
							if (!success)
							{
								if ((linebuffer = AllocRenderVec(MemHandler, (data->destwidth + 15) & ~15)))
								{
									switch (rh->drawmode)
									{
										case DRAWMODE_BITMAP:
										{
											struct BitMap *bm;
		
											DB(kprintf("blitting chunky to chunky in directdraw mode\n"));
											
											if ((bm = AllocBitMap(data->destwidth, data->destheight, rh->rp->BitMap->Depth, BMF_STANDARD+BMF_INTERLEAVED, NULL/*rh->rp->BitMap*/)))
											{
												struct Hook drawhook;
												struct drawdata_chunkybitmap hookdata;
						
												hookdata.bm = bm;
												hookdata.x = 0;
												hookdata.y = 0;
												hookdata.width = data->destwidth;
												hookdata.hook = NULL;	/*data->callbackhook;*/
												hookdata.pic = NULL; 	/*data->pic;*/
												hookdata.hookdelaycount = 0;
												if (data->clearbg)
												{
													hookdata.bgpen = data->dh->bgpen;
													hookdata.linebuffer = (UBYTE *) linebuffer;
												}
												else
												{
													hookdata.linebuffer = NULL;
												}						
												drawhook.h_Data = &hookdata;
#ifdef __MORPHOS__
												drawhook.h_Entry = (HOOKFUNC) &drawchunkybitmap;
#elif defined(__AROS__)
    	    	    	    	    	    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
												drawhook.h_SubEntry = (HOOKFUNC) drawchunkybitmap;
#else
												drawhook.h_Entry = (HOOKFUNC) drawchunkybitmap;
#endif

												if (Scale(scaleengine, (APTR) p, linebuffer,
													RND_SourceWidth, data->totalwidth, RND_DestWidth, 0,
													RND_LineHook, (IPTR)&drawhook, TAG_DONE) == CONV_SUCCESS)
												{
													if (data->rastlock) ObtainSemaphore(data->rastlock);

													BltBitMapRastPort(bm, 0, 0, rh->rp, data->destx, data->desty,
														data->destwidth, data->destheight, 0xc0);
													
													if (data->rastlock) ReleaseSemaphore(data->rastlock);

													success = TRUE;
												}
										
												FreeBitMap(bm);
											}
											break;
										}
										default:
										{
											struct Hook drawhook;
											struct drawdata_chunkyclut hookdata;
						
											hookdata.rp = rh->rp;
											hookdata.temprp = rh->temprp;
											hookdata.x = data->destx;
											hookdata.y = data->desty;
											hookdata.width = data->destwidth;
											hookdata.hook = NULL;	/*data->callbackhook;*/
											hookdata.pic = NULL;	/*data->pic;*/
											hookdata.rastlock = data->rastlock;
											hookdata.hookdelaycount = 0;
											if (data->clearbg)
											{
												hookdata.bgpen = data->dh->bgpen;
												hookdata.linebuffer = (UBYTE *) linebuffer;
											}
											else
											{
												hookdata.linebuffer = NULL;
											}						
											drawhook.h_Data = &hookdata;
#ifdef __MORPHOS__
											drawhook.h_Entry = (HOOKFUNC) &drawchunkyclut;
#elif defined(__AROS__)
    	    	    	    	    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
											drawhook.h_SubEntry = (HOOKFUNC) drawchunkyclut;
#else
											drawhook.h_Entry = (HOOKFUNC) drawchunkyclut;
#endif

											DB(kprintf("drawing/scaling chunky to chunky in directdraw mode\n"));
							
											success = !!(Scale(scaleengine, (APTR) p, linebuffer,
												RND_SourceWidth, data->totalwidth, RND_DestWidth, 0,
												RND_LineHook, (IPTR)&drawhook, TAG_DONE) == CONV_SUCCESS);
												
											break;
										}
									}
									FreeRenderVec(linebuffer);
								}
							}
						}
						else
						{
							if (rh->drawmode == DRAWMODE_BITMAP)
							{
								struct BitMap *bm;
		
								DB(kprintf("blitting chunky to chunky in directdraw mode\n"));
								
								if ((bm = AllocBitMap(data->destwidth, data->destheight, rh->rp->BitMap->Depth, BMF_STANDARD+BMF_INTERLEAVED, NULL/*rh->rp->BitMap*/)))
								{
									Chunky2BitMap((UBYTE *) p, 0, 0, data->destwidth,
										data->destheight, bm, 0, 0, RND_SourceWidth, data->totalwidth);

									if (data->rastlock) ObtainSemaphore(data->rastlock);

									BltBitMapRastPort(bm, 0, 0, rh->rp, data->destx, data->desty,
										data->destwidth, data->destheight, 0xc0);

									if (data->rastlock) ReleaseSemaphore(data->rastlock);

									success = TRUE;
							
									FreeBitMap(bm);
								}
							}
							else
							{
								DB(kprintf("drawing chunky to chunky in directdraw mode\n"));
	
								if (rh->drawmode == DRAWMODE_CYBERGFX)
								{
									if (data->rastlock) ObtainSemaphore(data->rastlock);

									WritePixelArray((APTR) p, 0, 0, data->totalwidth, rh->rp,
										data->destx, data->desty, data->destwidth, data->destheight, RECTFMT_LUT8);

									if (data->rastlock) ReleaseSemaphore(data->rastlock);
								}
								else
								{
									if (gfx40)
									{
										if (data->rastlock) ObtainSemaphore(data->rastlock);

										WriteChunkyPixels(rh->rp, data->destx, data->desty,
											data->destx+data->destwidth-1, data->desty+data->destheight-1,
											(APTR) p, data->totalwidth);

										if (data->rastlock) ReleaseSemaphore(data->rastlock);
									}
									else
									{
										int i;

										if (data->rastlock) ObtainSemaphore(data->rastlock);

										for (i = data->desty; i < data->desty + data->destheight; ++i)
										{
											WritePixelLine8(rh->rp, data->destx, i,
												data->destwidth, (APTR) p, rh->temprp);
											p += data->totalwidth;
										}

										if (data->rastlock) ReleaseSemaphore(data->rastlock);
									}
								
								}
								success = TRUE;
							}
						}
					}
					else
					{
						if ((linebuffer = AllocRenderVec(MemHandler, (data->destwidth + 15) & ~15)))
						{
							struct Hook drawhook;
							struct drawdata_chunkyclut hookdata;
			
							hookdata.rp = rh->rp;
							hookdata.temprp = rh->temprp;
							hookdata.x = data->destx;
							hookdata.y = data->desty;
							hookdata.width = data->destwidth;
							hookdata.hook = data->callbackhook;
							hookdata.pic = pic;
							hookdata.rastlock = data->rastlock;
							hookdata.hookdelaycount = 0;

				/*			if (data->clearbg)
							{
								hookdata.bgpen = data->dh->mainpentab[BestPen(data->dh->mainpalette, data->bgcolor)];
								hookdata.linebuffer = (UBYTE *) linebuffer;
							}
							else
							{
				*/				hookdata.linebuffer = NULL;
				//			}						

							drawhook.h_Data = &hookdata;
#ifdef __MORPHOS__
							drawhook.h_Entry = (HOOKFUNC) &drawchunkyclut;
#elif defined(__AROS__)
    	    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
							drawhook.h_SubEntry = (HOOKFUNC) drawchunkyclut;
#else
							drawhook.h_Entry = (HOOKFUNC) drawchunkyclut;
#endif

							DB(kprintf("converting/drawing/scaling chunky to chunky\n"));
					
							success = !!(ConvertChunky((APTR) p, data->sourcepalette,
								data->destwidth, data->destheight, linebuffer, data->destpalette,
								RND_BGPen, BestPen(data->sourcepalette, data->dh->bgcolor),
								RND_PenTable, (IPTR)data->pentab,
								RND_SourceWidth, data->totalwidth, RND_DestWidth, 0,
								RND_ScaleEngine, (IPTR)scaleengine,
								RND_MapEngine, (IPTR)data->dh->mapengine,
								RND_DitherMode,
									GetDitherMode(data->dh, (APTR) p, data->sourcepalette,
										data->sourcewidth, data->sourceheight,
										data->pixelformat, data->totalwidth, data->dithermode, data->autodither),
								RND_LineHook, (IPTR)&drawhook, TAG_DONE) == CONV_SUCCESS);
									
							FreeRenderVec(linebuffer);
						}
					}
					break;
				}

				case PIXFMT_0RGB_32:
				{
					if ((linebuffer = AllocRenderVec(MemHandler, (data->destwidth + 15) & ~15)))
					{
						struct Hook drawhook;
						struct drawdata_chunkyclut hookdata;

						hookdata.rp = rh->rp;
						hookdata.temprp = rh->temprp;
						hookdata.x = data->destx;
						hookdata.y = data->desty;
						hookdata.width = data->destwidth;
						hookdata.hook = data->callbackhook;
						hookdata.pic = pic;
						hookdata.rastlock = data->rastlock;
						hookdata.hookdelaycount = 0;

			/*			if (data->clearbg)
						{
							hookdata.bgpen = data->dh->mainpentab[BestPen(data->dh->mainpalette, data->bgcolor)];
							hookdata.linebuffer = (UBYTE *) linebuffer;
						}
						else
						{
			*/				hookdata.linebuffer = NULL;
			//			}						
						drawhook.h_Data = &hookdata;
#ifdef __MORPHOS__
						drawhook.h_Entry = (HOOKFUNC) &drawchunkyclut;
#elif defined(__AROS__)
    	    	    	    	    	    	drawhook.h_Entry = HookEntry;
						drawhook.h_SubEntry = (HOOKFUNC) drawchunkyclut;
#else
						drawhook.h_Entry = (HOOKFUNC) drawchunkyclut;
#endif

						DB(kprintf("converting/drawing/scaling truecolor to chunky\n"));

						success = !!(Render((APTR) p, data->destwidth, data->destheight,
								linebuffer, data->destpalette,
								RND_BGColor, data->dh->bgcolor,
								RND_PenTable, (IPTR)data->pentab,
								RND_SourceWidth, data->totalwidth,
								RND_DestWidth, 0,
								RND_ScaleEngine, (IPTR)scaleengine,
								RND_MapEngine, (IPTR)data->dh->mapengine,
								RND_ColorMode, data->colormode,
								RND_DitherMode,
									GetDitherMode(data->dh, (APTR) p, NULL,
										data->sourcewidth, data->sourceheight,
										data->pixelformat, data->totalwidth,
										data->dithermode, data->autodither),
								RND_LineHook, (IPTR)&drawhook, 
								TAG_DONE) == REND_SUCCESS);
							
						FreeRenderVec(linebuffer);
					}
					break;
				}
				
				#ifdef DEBUG				
				default:
				{
					DB(kprintf("*** Illegal condition: undefined pixelformat.\n"));
					break;
				}
				#endif
				
			}
		}
	}

	if (pic->scaleengine && data->coords)
	{
		DeleteScaleEngine(pic->scaleengine);
		pic->scaleengine = NULL;
	}

	return success;
}



/*********************************************************************
----------------------------------------------------------------------

	success = MapPalette(drawhandle, palette, pentab, tags);

		mappt eine Palette auf ein Drawhandle und
		erzeugt dafür eine PenTable.

		GGFX_NumColors		Anzahl Farben in der Palette
		GGFX_PaletteFormat	Format der Palette
	
----------------------------------------------------------------------
*********************************************************************/

BOOL SAVE_DS ASM MapPaletteA(
	REG(a0) DRAWHANDLE *dh,
	REG(a1) APTR palette,
	REG(a2) UBYTE *pentab,
	REG(a3) TAGLIST tags)
{
	BOOL success;
	
	int i;
	ULONG rgb;
	ULONG *p = (ULONG *) palette;

	ULONG numcolors;
	success = FALSE;
	
	if ((numcolors = GetTagData(GGFX_NumColors, 0, tags)))
	{
		switch (GetTagData(GGFX_PaletteFormat, PALFMT_RGB8, tags))
		{
			case PALFMT_RGB8:
				for(i=0; i<numcolors; ++i)
				{
					*pentab++ = dh->mainpentab[BestPen(dh->realpalette, *p++)];
				}
				success = TRUE;
				break;
	
			case PALFMT_RGB32:
				for(i=0; i<numcolors; ++i)
				{
					rgb = (p[0] & 0xff000000 >> 8)
						+ (p[1] & 0xff000000 >> 16)
						+ (p[2] & 0xff000000 >> 24);
	
					*pentab++ = dh->mainpentab[BestPen(dh->realpalette, rgb)];
					p += 3;
				}
				success = TRUE;
				break;
		}
	}
	
	return success;
}


/*********************************************************************
----------------------------------------------------------------------

	pen = MapPen(drawhandle, RGB, tags);

		mappt einen RGB auf ein Drawhandle und
		liefert dafür den entsprechenden Pen.
	
----------------------------------------------------------------------
*********************************************************************/

LONG SAVE_DS ASM MapPenA(REG(a0) DRAWHANDLE *dh, REG(d0) ULONG rgb, REG(a1) TAGLIST tags)
{
	return dh->mainpentab[BestPen(dh->realpalette, rgb)];
}



/*********************************************************************
----------------------------------------------------------------------

	success = CreatePictureMask(picture, array, arraywidth, tags);

		GGFX_SourceWidth			
		GGFX_SourceHeight			
		GGFX_SourceX
		GGFX_SourceY

		GGFX_DestWidth				skalierte Breite
		GGFX_DestHeight			skalierte Höhe
	//	GGFX_DestX
	//	GGFX_DestY
		
		GGFX_Ratio					
	
----------------------------------------------------------------------
*********************************************************************/

BOOL SAVE_DS ASM CreatePictureMaskA(REG(a0) PIC *pic, REG(a1) UBYTE *array, REG(d0) UWORD arraywidth, REG(a2) TAGLIST tags)
{
	BOOL success = FALSE;

	ObtainSemaphore(&pic->semaphore);

	if (pic)
	{
		UBYTE *p;
		int sourcex, sourcey, sourcewidth, sourceheight, destwidth, destheight;
	//	int destx, desty;
		int threshold;
		int x, y;
		int bitmask;
		int realsourcewidth;
		
		sourcex = GetTagData(GGFX_SourceX, 0, tags);
		sourcey = GetTagData(GGFX_SourceY, 0, tags);
		sourcewidth = GetTagData(GGFX_SourceWidth, pic->width, tags);
		sourceheight = GetTagData(GGFX_SourceHeight, pic->height, tags);

	//	destx = GetTagData(GGFX_DestX, 0, tags);
	//	desty = GetTagData(GGFX_DestY, 0, tags);
		destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
		destheight = GetTagData(GGFX_DestHeight, pic->height, tags);

		threshold = GetTagData(GGFX_Ratio, 128, tags);

		if (ExtractAlphaArray(pic))
		{
			success = TRUE;
	
			if (pic->alphaarray)
			{
				UBYTE *temparray = NULL;
				
				p = pic->alphaarray + sourcex + sourcey * pic->width;
				realsourcewidth = pic->width;
				
				if (sourcewidth != destwidth || sourceheight != destheight)
				{
					success = FALSE;
					
					DB(kprintf("*** scaling mask bitmap\n"));
	
					if ((temparray = AllocRenderVec(MemHandler, destwidth*destheight)))
					{
						APTR scaleengine;
	
						if ((scaleengine = CreateScaleEngine(sourcewidth, sourceheight,
							destwidth, destheight, RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
						{
							if (Scale(scaleengine, p, temparray,
								RND_SourceWidth, pic->width, TAG_DONE) == CONV_SUCCESS)
							{
								success = TRUE;
								p = temparray;
								realsourcewidth = destwidth;
							}
						
							DeleteScaleEngine(scaleengine);
						}
					}
				}
				
				if (success)
				{			
					for (y = 0; y < destheight; ++y)
					{
						bitmask = 0x80;
					
						for (x = 0; x < destwidth; ++x)
						{					
							if (*p++ >= threshold)
							{
								array[x>>3] |= bitmask;
							}
							else
							{
								array[x>>3] &= ~bitmask; 
							}
		
							if (!(bitmask >>= 1))
							{
								bitmask = 0x80;
							}
						}
						p += realsourcewidth - destwidth;
						array += arraywidth;
					}
				}
				
				if (temparray)
				{
					FreeRenderVec(temparray);
				}
			}
			else
			{
				for (y = 0; y < destheight; ++y)
				{
					bitmask = 0x80;
	
					for (x = 0; x < destwidth; ++x)
					{
						array[x>>3] |= bitmask;
						if (!(bitmask >>= 1))
						{
							bitmask = 0x80;
						}
					}
					array += arraywidth;
				}
			}
		}
	}

	ReleaseSemaphore(&pic->semaphore);

	return success;
}






/*********************************************************************
----------------------------------------------------------------------

	TEST VERSION: pic=NULL zulässig

	bitmap = CreatePictureBitMap(drawhandle, pic, tags);

		GGFX_DestWidth				skalierte Breite
		GGFX_DestHeight				skalierte Höhe
		GGFX_SourceWidth			
		GGFX_SourceHeight			
		GGFX_SourceX
		GGFX_SourceY

		GGFX_DitherMode
		GGFX_DitherAmount

		erzeugt aus einem Array eine auf das DrawHandle angepaßte Bitmap.
	
----------------------------------------------------------------------
*********************************************************************/

struct BitMap SAVE_DS ASM *CreatePictureBitMapA(REG(a0) DRAWHANDLE *dh, REG(a1) PIC *pic, REG(a2) TAGLIST tags)
{
	BOOL success = FALSE;
	struct BitMap *bm = NULL;

	if (dh)
	{
//		ULONG pixelformat;
		LONG depth;
		UWORD destwidth, destheight;
		ULONG additional_flags = 0;

		if (pic)
		{
			ObtainSemaphore(&pic->semaphore);

			destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
			destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
		
			if (PrepareDrawing(pic))
			{
		
				if (dh->rasthandle->colormode != COLORMODE_CLUT)
				{
					if (!pic->directdraw)
					{
						/*
						 *	für HAM: versuchen, auf RGB umzurechnen. Wenn
						 *	das nicht klappt, ist's auch nicht weiter schlimm.
						 */
				
						PIC_Render(pic, PIXFMT_0RGB_32, NULL);
					}
				}
		
				success = TRUE;
			}
		}
		else
		{
			destwidth = GetTagData(GGFX_DestWidth, 0, tags);
			destheight = GetTagData(GGFX_DestHeight, 0, tags);
			additional_flags = BMF_CLEAR;
			success = (destwidth && destheight);
			
		}

		if (success)
		{
			GetBitMapInfo(dh->rasthandle->rp->BitMap, dh->modeID,
				BMAPATTR_Depth, (IPTR)&depth, TAG_DONE);

			switch (dh->rasthandle->drawmode)
			{
				case DRAWMODE_CYBERGFX:
				{
					bm = AllocBitMap(destwidth, destheight, depth, BMF_MINPLANES | additional_flags, dh->rasthandle->rp->BitMap);
					break;
				}
				
				case DRAWMODE_WRITECHUNKYPIXELS:
				case DRAWMODE_WRITEPIXELARRAY:
				{
					bm = AllocBitMap(destwidth, destheight, depth, additional_flags, dh->rasthandle->rp->BitMap);
					break;
				}
			
				case DRAWMODE_BITMAP:
				{
					bm = AllocBitMap(destwidth, destheight, depth, BMF_STANDARD | additional_flags, dh->rasthandle->rp->BitMap);
					break;
				}
			}
			
			success = !!bm;
		}

			
		if (success && pic)
		{
			RASTHANDLE *rh;
			struct RastPort rp;
			
			success = FALSE;
		
			InitRastPort(&rp);
			rp.BitMap = bm;
		
			if ((rh = CreateRastHandle(&rp, dh->modeID)))
			{
				struct DrawData *data;
			
				if ((data = CreateDrawData(dh, pic)))
				{
					data->sourcex = GetTagData(GGFX_SourceX, 0, tags);
					data->sourcey = GetTagData(GGFX_SourceY, 0, tags);
					data->sourcewidth = GetTagData(GGFX_SourceWidth, pic->width, tags);
					data->sourceheight = GetTagData(GGFX_SourceHeight, pic->height, tags);
					data->destwidth = destwidth;
					data->destheight = destheight;
					data->autodither = GetTagData(GGFX_AutoDither, dh->autodither, tags);
					data->dithermode = GetTagData(GGFX_DitherMode, dh->dithermode, tags);
					data->ditheramount = GetTagData(GGFX_DitherAmount, dh->ditheramount, tags);
					data->callbackhook = (struct Hook *) GetTagData(GGFX_CallBackHook, NULL, tags);
					data->usescalepixelarray = FALSE;
					data->rastlock = NULL;

					success = DrawRastHandle(rh, data);
		
					FreeRenderVec(data);
				}
		
				DeleteRastHandle(rh);
			}
		}

		if (!success)
		{
			if (bm)
			{
				FreeBitMap(bm);
				bm = NULL;
			}
		}

		if (pic)
		{
			ReleaseSemaphore(&pic->semaphore);
		}

	}

	return bm;

}









/*********************************************************************
----------------------------------------------------------------------

	success = DrawPictureA(drawhandle,pic,x,y,tags);

	Zeichnet ein Bild via DrawHandle.

	GGFX_DestWidth				skalierte Breite
	GGFX_DestHeight				skalierte Höhe
	GGFX_SourceWidth			Ausgangsbreite
	GGFX_SourceHeight			Ausgangshöhe
	GGFX_SourceX				Start-X
	GGFX_SourceY				Start-Y
	GGFX_DitherMode
	GGFX_DitherAmount
	GGFX_CallbackHook			für Zeichenoperationen
	GGFX_RastLock


	TEST:
	
	RND_DestCoordinates

----------------------------------------------------------------------
*********************************************************************/

BOOL SAVE_DS ASM DrawPictureA(
	REG(a0) DRAWHANDLE *dh,
	REG(a1) PIC *pic,
	REG(d0) UWORD x,
	REG(d1) UWORD y,
	REG(a2) TAGLIST tags)
{

	BOOL success = TRUE;

	DB(kprintf("x,y: %ld,%ld\n", x,y));

	ObtainSemaphore(&pic->semaphore);

	if (PrepareDrawing(pic))
	{
		struct DrawData *data;

		if (dh->rasthandle->colormode != COLORMODE_CLUT)
		{
			if (!pic->directdraw)
			{
				/*
				 *	für HAM: versuchen, auf RGB umzurechnen. Wenn
				 *	das nicht klappt, ist's auch nicht weiter schlimm.
				 */
		
				PIC_Render(pic, PIXFMT_0RGB_32, NULL);
			}
		}

		if ((data = CreateDrawData(dh, pic)))
		{
			data->sourcex = GetTagData(GGFX_SourceX, 0, tags);
			data->sourcey = GetTagData(GGFX_SourceY, 0, tags);
			data->sourcewidth = GetTagData(GGFX_SourceWidth, pic->width, tags);
			data->sourceheight = GetTagData(GGFX_SourceHeight, pic->height, tags);
			data->destx = x;
			data->desty = y;
			data->destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
			data->destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
			data->dithermode = GetTagData(GGFX_DitherMode, dh->dithermode, tags);
			data->ditheramount = GetTagData(GGFX_DitherAmount, dh->ditheramount, tags);
			data->callbackhook = (struct Hook *) GetTagData(GGFX_CallBackHook, NULL, tags);
			data->autodither = GetTagData(GGFX_AutoDither, dh->autodither, tags);
			data->rastlock = (struct SignalSemaphore *) GetTagData(GGFX_RastLock, NULL, tags);

			data->coords = (WORD *) GetTagData(RND_DestCoordinates, NULL, tags);

			data->clearbg = !!data->coords;
		//	data->bgcolor = GetTagData(GGFX_BGColor, 0x000000, tags);
			
			data->usescalepixelarray = env_usescalepixelarray;
			
			success = DrawRastHandle(dh->rasthandle, data);
	
			FreeRenderVec(data);
		}
	}

	ReleaseSemaphore(&pic->semaphore);		///	!!!

	return success;
}


//////////////////////////////////////////////////////////////////////




/*********************************************************************
----------------------------------------------------------------------

	DeleteDirectDrawHandleA(directdrawhandle);

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM DeleteDirectDrawHandle(REG(a0) DIRECTDRAWHANDLE *ddh)
{
	if (ddh)
	{
		FreeRenderVec(ddh->scalebuffer);
		FreeRenderVec(ddh->mapbuffer);
		if (ddh->scaleengine)
		{
			DeleteScaleEngine(ddh->scaleengine);
		}
		FreeRenderVec(ddh);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	directdrawhandle = CreateDirectDrawHandleA(drawhandle,
		sourcewidth, sourceheight, destwidth, destheight, tags);

	anfordern eines Direct-Draw-Handles für
	LowLevel-Rendern/Skalieren

	GGFX_PixelFormat	Default:	PIXFMT_0RGB_32

----------------------------------------------------------------------
*********************************************************************/

DIRECTDRAWHANDLE SAVE_DS ASM *CreateDirectDrawHandleA(
	REG(a0) DRAWHANDLE *dh,
	REG(d0) UWORD sourcewidth,
	REG(d1) UWORD sourceheight,
	REG(d2) UWORD destwidth,
	REG(d3) UWORD destheight,
	REG(a1) TAGLIST tags)
{
	DIRECTDRAWHANDLE *ddh = NULL;
	BOOL success = FALSE;
	int pixelformat = GetTagData(GGFX_PixelFormat, PIXFMT_0RGB_32, tags);
	

	if (dh)
	{

		switch (pixelformat)
		{

			case PIXFMT_0RGB_32:

				if ((ddh = AllocRenderVecClear(MemHandler, sizeof(DIRECTDRAWHANDLE))))
				{
					BOOL scaling_is_involved = (sourcewidth != destwidth || sourceheight != destheight);
					BOOL source_is_smaller = (sourcewidth*sourceheight < destwidth*destheight);
					BOOL dest_is_truecolor = dh->rasthandle->truecolor;
					BOOL mapping_is_involved = FALSE;
					int scalepixelformat = PIXFMT_CHUNKY_CLUT;
					int scalebuffersize = 0;
					int mapbuffersize = 0;
				
					ddh->pixelformat = pixelformat;
					ddh->sourcewidth = sourcewidth;
					ddh->sourceheight = sourceheight;
					ddh->destwidth = destwidth;
					ddh->destheight = destheight;
					ddh->dh = dh;
		
		
					if (dest_is_truecolor)
					{
						if (scaling_is_involved)
						{
							ddh->drawmode = DDMODE_SCALEDRAW;
							scalepixelformat = PIXFMT_0RGB_32;
							if (!(cgfx41 /*&& env_usescalepixelarray*/))
							{
								scalebuffersize = destwidth * destheight * 4;
							}
						}
						else
						{
							ddh->drawmode = DDMODE_DRAW;
						}
					}
					else
					{
						mapping_is_involved = TRUE;
						if (source_is_smaller)
						{
							ddh->drawmode = DDMODE_MAPSCALEDRAW;
							scalepixelformat = PIXFMT_CHUNKY_CLUT;
							scalebuffersize = destwidth * destheight;
							mapbuffersize = sourcewidth * sourceheight;
						}
						else
						{
							if (scaling_is_involved)
							{
								ddh->drawmode = DDMODE_SCALEMAPDRAW;
								scalepixelformat = PIXFMT_0RGB_32;
								scalebuffersize = destwidth * destheight * 4;
								// no mapbuffer required. mapping can be performed over itself.
							}
							else
							{
								ddh->drawmode = DDMODE_MAPDRAW;
								mapbuffersize = destwidth * destheight;
							}
						}
					}
		
					success = TRUE;
					
					if (scaling_is_involved)
					{
						success = !!(ddh->scaleengine = CreateScaleEngine(sourcewidth, sourceheight,
								destwidth, destheight, RND_PixelFormat, scalepixelformat,
								RND_RMHandler, (IPTR)MemHandler, TAG_DONE));
					}
		
					if (success && mapping_is_involved)
					{
						ObtainSemaphore(&dh->mapsemaphore);
						if (!dh->mapengine)
						{
							success = !!(dh->mapengine = CreateMapEngine(dh->realpalette,
									RND_RMHandler, (IPTR)MemHandler, TAG_DONE));
						}
						ReleaseSemaphore(&dh->mapsemaphore);
					}
		
					if (success && scalebuffersize)
					{
						success = !!(ddh->scalebuffer = AllocRenderVec(MemHandler, scalebuffersize));
					}
		
					if (success && mapbuffersize)
					{
						success = !!(ddh->mapbuffer = AllocRenderVec(MemHandler, mapbuffersize));
					}
				}
				break;

/***************************************

			case PIXFMT_CHUNKY_CLUT:

				if (ddh = AllocRenderVecClear(MemHandler, sizeof(DIRECTDRAWHANDLE)))
				{
					BOOL scaling_is_involved = (sourcewidth != destwidth || sourceheight != destheight);
					BOOL source_is_smaller = (sourcewidth*sourceheight < destwidth*destheight);
					BOOL dest_is_truecolor = dh->rasthandle->truecolor;
					BOOL mapping_is_involved = FALSE;
					int scalepixelformat = PIXFMT_CHUNKY_CLUT;
					int scalebuffersize = 0;
					int mapbuffersize = 0;
				
					ddh->pixelformat = pixelformat;
					ddh->sourcewidth = sourcewidth;
					ddh->sourceheight = sourceheight;
					ddh->destwidth = destwidth;
					ddh->destheight = destheight;
					ddh->dh = dh;
		
		
					if (dest_is_truecolor)
					{
						if (scaling_is_involved)
						{
							if (cgfx41)
							{
								// chunky->rgb, mit ScalePixelArray() zeichnen

							}
							else
							{
								if (source_is_smaller)
								{
									// chunky->rgb, rgb skalieren, zeichnen

								}
								else
								{
									// chunky skalieren, chunky->rgb, zeichnen
								}
							}
						}
						else
						{
							// chunky->rgb, zeichnen

						}
					}
					else
					{
						if (scaling_is_involved)
						{
							if (source_is_smaller)
							{
								// chunky mappen, skalieren, zeichnen
							}
							else
							{
								// chunky skalieren, mappen, zeichnen
							}
						}
						else
						{
							// chunky mappen, zeichnen
						}
					}
		
					success = TRUE;
					
					if (scaling_is_involved)
					{
						success = !!(ddh->scaleengine = CreateScaleEngine(sourcewidth, sourceheight,
								destwidth, destheight, RND_PixelFormat, scalepixelformat,
								RND_RMHandler, MemHandler, TAG_DONE));
					}
		
					if (success && mapping_is_involved)
					{
						ObtainSemaphore(&dh->mapsemaphore);
						if (!dh->mapengine)
						{
							success = !!(dh->mapengine = CreateMapEngine(dh->realpalette,
									RND_RMHandler, MemHandler, TAG_DONE));
						}
						ReleaseSemaphore(&dh->mapsemaphore);
					}
		
					if (success && scalebuffersize)
					{
						success = !!(ddh->scalebuffer = AllocRenderVec(MemHandler, scalebuffersize));
					}
		
					if (success && mapbuffersize)
					{
						success = !!(ddh->mapbuffer = AllocRenderVec(MemHandler, mapbuffersize));
					}
				}
				break;

**************************************************/


			default:
				break;

		}

		if (!success)
		{
			DeleteDirectDrawHandle(ddh);
			ddh = NULL;
		}

	}
	
	return ddh;
}


/*********************************************************************
----------------------------------------------------------------------

	success = DirectDrawTruecolorA(directdrawhandle, array, x,y, tags);

	Zeichnet ein TrueColor-Array auf ein DrawHandle
	mit minimalem Overhead, ohne Dithering.
	
	GGFX_SourceWidth

----------------------------------------------------------------------
*********************************************************************/

BOOL SAVE_DS ASM DirectDrawTrueColorA(
	REG(a0) DIRECTDRAWHANDLE *ddh,
	REG(a1) ULONG *array,
	REG(d0) UWORD x,
	REG(d1) UWORD y,
	REG(a2) TAGLIST tags)
{
	BOOL success = FALSE;

	if (ddh)
	{
		int totalsourcewidth;
		RASTHANDLE *rh = ddh->dh->rasthandle;
		UBYTE *drawbuffer = NULL;
	//	int drawwidth, drawheight, drawtotalwidth;
	
		totalsourcewidth = GetTagData(GGFX_SourceWidth, ddh->sourcewidth, tags);

		switch (ddh->drawmode)
		{
			case DDMODE_DRAW:

				//drawbuffer = (UBYTE *) array;
				
				WritePixelArray(array, 0, 0, totalsourcewidth * 4,
					rh->rp, x, y, ddh->destwidth, ddh->destheight, RECTFMT_ARGB);

				success = TRUE;
				break;

			case DDMODE_SCALEDRAW:

				if (cgfx41 /* && env_usescalepixelarray*/)
				{
					ScalePixelArray(array, ddh->sourcewidth, ddh->sourceheight, 
							totalsourcewidth*4, rh->rp, x, y, ddh->destwidth,
							ddh->destheight, RECTFMT_ARGB);
				}
				else
				{
					Scale(ddh->scaleengine, array, ddh->scalebuffer,
						RND_SourceWidth, totalsourcewidth, TAG_DONE);
			
					WritePixelArray(ddh->scalebuffer, 0, 0, ddh->destwidth * 4,
						rh->rp, x, y, ddh->destwidth, ddh->destheight, RECTFMT_ARGB);
				}
				success = TRUE;
				break;
			
			case DDMODE_MAPDRAW:
			
				if (MapRGBArray(ddh->dh->mapengine, array,
					ddh->sourcewidth, ddh->sourceheight, ddh->mapbuffer,
					RND_SourceWidth, totalsourcewidth,
					RND_PenTable, (IPTR)ddh->dh->mainpentab, 
					TAG_DONE) == CONV_SUCCESS)
				{
					drawbuffer = ddh->mapbuffer;
				}
				break;
		
			case DDMODE_SCALEMAPDRAW:
			
				Scale(ddh->scaleengine, array, ddh->scalebuffer,
					RND_SourceWidth, totalsourcewidth, TAG_DONE);

				if (MapRGBArray(ddh->dh->mapengine, ddh->scalebuffer,
					ddh->destwidth, ddh->destheight, ddh->scalebuffer,
					RND_PenTable, (IPTR)ddh->dh->mainpentab,
					TAG_DONE) == CONV_SUCCESS)
				{
					drawbuffer = ddh->scalebuffer;
				}
				break;

			case DDMODE_MAPSCALEDRAW:
			
				if (MapRGBArray(ddh->dh->mapengine, array,
					ddh->sourcewidth, ddh->sourceheight, ddh->mapbuffer,
					RND_SourceWidth, totalsourcewidth,
					RND_PenTable, (IPTR)ddh->dh->mainpentab, 
					TAG_DONE) == CONV_SUCCESS)
				{
					Scale(ddh->scaleengine, ddh->mapbuffer, ddh->scalebuffer, NULL);
					drawbuffer = ddh->scalebuffer;
				}
				break;
			
			default:
				break;
		}


		if (drawbuffer)
		{
			switch (rh->drawmode)
			{
				case DRAWMODE_CYBERGFX:
				{
					WritePixelArray(drawbuffer, 0, 0, ddh->destwidth,
						rh->rp, x, y, ddh->destwidth, ddh->destheight, RECTFMT_LUT8);
					break;
				}

				case DRAWMODE_WRITEPIXELARRAY:
				{
					if (gfx40)
					{
						WriteChunkyPixels(rh->rp, x, y, x+ddh->destwidth-1, y+ddh->destheight-1,
							(APTR) drawbuffer, ddh->destwidth);
					}
					else
					{
						if (ddh->destwidth & 15)
						{
							int i;
							char *p = drawbuffer;
							for (i = y; i < y + ddh->destheight; ++i)
							{
								WritePixelLine8(rh->rp, x, i, ddh->destwidth, (APTR) p, rh->temprp);
								p += ddh->destwidth;
							}
						}
						else
						{
							WritePixelArray8(rh->rp, x, y, x+ddh->destwidth-1, y+ddh->destheight-1,
								drawbuffer, rh->temprp);
						}
					}				
					break;
				}
				
				case DRAWMODE_BITMAP:
				{
					struct BitMap *bm;
					if ((bm = AllocBitMap(ddh->destwidth, ddh->destheight,
						rh->rp->BitMap->Depth, BMF_STANDARD+BMF_INTERLEAVED, NULL)))
					{
						Chunky2BitMap(drawbuffer, 0,0, ddh->destwidth, ddh->destheight,
							bm, 0,0, NULL);
		
						BltBitMapRastPort(bm, 0, 0, rh->rp, x, y,
							ddh->destwidth, ddh->destheight, 0xc0);
		
						FreeBitMap(bm);
					}
					break;
				}
			}
			success = TRUE;
		}

	}

	return success;
}

