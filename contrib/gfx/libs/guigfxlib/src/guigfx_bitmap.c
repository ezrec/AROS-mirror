/*********************************************************************
----------------------------------------------------------------------

	guigfx_bitmap

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

	ReadBitMapArray	(bm, pic, displayID, tags)

	convert data from a bitmap structure to a prepared picture structure.
	this function inserts the array and the pixelformat to the picture.

	GGFX_SourceX
	GGFX_SourceY
	GGFX_SourceWidth
	GGFX_SourceHeight

--------------------------------------------------------------------*/

#if !defined(__MORPHOS__) && !defined(__AROS__)
BOOL ReadBitMapArray(struct BitMap *bm, PIC *pic, UWORD displayID, Tag tag1, ...)
{
	return ReadBitMapArrayA(bm, pic, displayID, (TAGLIST) &tag1);
}
#endif

BOOL ReadBitMapArrayA(struct BitMap *bm, PIC *pic, UWORD displayID, TAGLIST tags)
{
	BOOL success = FALSE;
	int width, height, pixelformat;
	APTR array;

	if ((array = ReadBitMap(bm, displayID, pic->palette, &pixelformat, &width, &height,
		GGFX_SourceWidth, pic->width,
		GGFX_DestWidth, pic->width,
		TAG_DONE)))
	{
		pic->owner = TRUE;	
		pic->pixelformat = pixelformat;
		pic->array = array;
		success = TRUE;
	}

	return success;
}




/*--------------------------------------------------------------------


	num = GetBitMapInfo(bitmap, displayID, tags)
	
		BMAPATTR_Width

		BMAPATTR_Height

		BMAPATTR_Depth

		BMAPATTR_CyberGFX
		
		BMAPATTR_BitMapFormat	(returns actual bitmap format)
	
			PIXFMT_BITMAP_HAM8
			PIXFMT_BITMAP_HAM6
			PIXFMT_BITMAP_CLUT
			PIXFMT_BITMAP_RGB


		BMAPATTR_PixelFormat	(returns the applicable ggfx pixel format)
		
			PIXFMT_CHUNKY_CLUT
			PIXFMT_0RGB_32
		

		BMAPATTR_Flags
			
			returns BMF_ flags


--------------------------------------------------------------------*/

#ifndef __MORPHOS__


#ifdef __AROS__
#define GetBitMapInfo(bm,displayid,args...) \
({ \
     IPTR __args[] = { args }; \
     GetBitMapInfoA((bm), (displayid), (TAGLIST)__args); \
})
#else
int GetBitMapInfo(struct BitMap *bm, ULONG displayID, Tag tag1, ...)
{
	return GetBitMapInfoA(bm, displayID, (TAGLIST) &tag1);
}
#endif


#endif


int GetBitMapInfoA(struct BitMap *bm, ULONG displayID, TAGLIST tags)
{
	int count = 0;

	if (bm)
	{

		BOOL cyberbitmap = FALSE;
		LONG depth;
		LONG *p;

		
		depth = GetBitMapAttr(bm, BMA_DEPTH);
	
		if (CyberGfxBase)
		{
			if (GetCyberMapAttr(bm, CYBRMATTR_ISCYBERGFX))
			{
				cyberbitmap = TRUE;
				depth = GetCyberMapAttr(bm, CYBRMATTR_DEPTH);
			}
		}

		if ((p = (LONG *) GetTagData(BMAPATTR_Width, NULL, tags)))
		{
			*p = GetBitMapAttr(bm, BMA_WIDTH);
			count++;
		}

		if ((p = (LONG *) GetTagData(BMAPATTR_Height, NULL, tags)))
		{
			*p = GetBitMapAttr(bm, BMA_HEIGHT);
			count++;
		}

		if ((p = (LONG *) GetTagData(BMAPATTR_Depth, NULL, tags)))
		{
			*p = depth;
			count++;
		}

		if ((p = (LONG *) GetTagData(BMAPATTR_CyberGFX, NULL, tags)))
		{
			*p = (LONG) cyberbitmap;
			count++;
		}

		if ((p = (LONG *) GetTagData(BMAPATTR_BitMapFormat, NULL, tags)))
		{
			if (cyberbitmap)
			{
				if (depth > 8)
				{
					*p = PIXFMT_BITMAP_RGB;
				}
				else
				{
					*p = PIXFMT_BITMAP_CLUT;
				}
			}
			else
			{
				if ((displayID != INVALID_ID) && (displayID & HAM))
				{
					switch (depth)
					{
						case 8:
							*p = PIXFMT_BITMAP_HAM8;
							break;
						case 6:
							*p = PIXFMT_BITMAP_HAM6;
							break;
						default:
							*p = PIXFMT_BITMAP_CLUT;
					}
				}
				else
				{
					*p = PIXFMT_BITMAP_CLUT;
				}
			}
			count++;
		}

		if ((p = (LONG *) GetTagData(BMAPATTR_PixelFormat, NULL, tags)))
		{
			LONG bf;

			GetBitMapInfo(bm, displayID, BMAPATTR_BitMapFormat, (IPTR)&bf, TAG_DONE);
		
			switch (bf)
			{
				case PIXFMT_BITMAP_HAM6:	
				case PIXFMT_BITMAP_HAM8:
				case PIXFMT_BITMAP_RGB:
					*p = PIXFMT_0RGB_32;
					break;
				
				default:
				case PIXFMT_BITMAP_CLUT:
					*p = PIXFMT_CHUNKY_CLUT;
					break;
			}
			count++;
		}

		if ((p = (LONG *) GetTagData(BMAPATTR_Flags, NULL, tags)))
		{
			*p = GetBitMapAttr(bm, BMA_FLAGS);
			count++;
		}
	
	}

	return count;
}






/*---------------------------------------------------------------------

	readrgb

	callback for reading RGB data from a rastport

--------------------------------------------------------------------*/

struct readrgbdata
{
	int sourcewidth;
	struct RastPort *rp;
	int sourcex;
	int sourcey;
	int lastline;
};


#ifdef __MORPHOS__
FUNC_HOOK(ULONG, readrgb, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG readrgbfunc(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg)
#else
ULONG ASM SAVE_DS readrgbfunc(	register __a0 struct Hook *hook,
								register __a2 APTR buffer,
								register __a1 struct RND_LineMessage *msg )
#endif
{
	struct readrgbdata *data = hook->h_Data;

	if (msg->RND_LMsg_type == LMSGTYPE_LINE_FETCH)
	{
		if (data->lastline != msg->RND_LMsg_row)
		{
			data->lastline = msg->RND_LMsg_row;
			ReadPixelArray(buffer, 0,0, data->sourcewidth * 4, data->rp,
				data->sourcex, data->sourcey + msg->RND_LMsg_row, data->sourcewidth, 1, RECTFMT_ARGB);
		}
	}

	return TRUE;
}



/*---------------------------------------------------------------------

	readclut

	callback for reading chunky data from a rastport

--------------------------------------------------------------------*/

struct readclutdata
{
	int sourcewidth;
	struct RastPort *rp, *temprp;
	int sourcex;
	int sourcey;
	int lastline;

};


#ifdef __MORPHOS__
FUNC_HOOK(ULONG, readclut, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG readclutfunc(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS readclutfunc(	register __a0 struct Hook *hook,
								register __a2 APTR buffer,
								register __a1 struct RND_LineMessage *msg )
#endif
{
	struct readclutdata *data = hook->h_Data;

	if (msg->RND_LMsg_type == LMSGTYPE_LINE_FETCH)
	{
		if (data->lastline != msg->RND_LMsg_row)
		{
			data->lastline = msg->RND_LMsg_row;
			ReadPixelLine8(data->rp, data->sourcex, data->sourcey + msg->RND_LMsg_row, data->sourcewidth, buffer, data->temprp);
		}
	}

	return TRUE;
}



/*---------------------------------------------------------------------

	readham

	callback for reading ham data from a rastport

--------------------------------------------------------------------*/

struct readhamdata
{
	int sourcewidth;
	struct RastPort *rp, *temprp;
	int sourcex;
	int sourcey;
	int lastline;
	UBYTE *linebuffer;		// intermediate buffer for the HAM chunky bytes
	PALETTE palette;
	int colormode;
};


#ifdef __MORPHOS__
FUNC_HOOK(ULONG, readham, struct Hook *, hook, APTR, buffer, struct RND_LineMessage *, msg)
#elif defined(__AROS__)
ULONG ASM SAVE_DS readhamfunc(struct Hook *hook, APTR buffer, struct RND_LineMessage *msg )
#else
ULONG ASM SAVE_DS readhamfunc(	register __a0 struct Hook *hook,
								register __a2 APTR buffer,
								register __a1 struct RND_LineMessage *msg )
#endif
{
	struct readhamdata *data = hook->h_Data;

	if (msg->RND_LMsg_type == LMSGTYPE_LINE_FETCH)
	{
		if (data->lastline != msg->RND_LMsg_row)
		{
			data->lastline = msg->RND_LMsg_row;
			ReadPixelLine8(data->rp, 0, data->sourcey + msg->RND_LMsg_row, data->sourcewidth+data->sourcex, data->linebuffer, data->temprp);
			Chunky2RGB(data->linebuffer, data->sourcewidth, 1, buffer, data->palette,
				RND_ColorMode, data->colormode,
				RND_LeftEdge, data->sourcex,
				TAG_DONE);
		}
	}

	return TRUE;
}



/*--------------------------------------------------------------------

	array = ReadBitMap(bm, displayID, palette, *pixelformat, *width, *height, tags)
	
		read a bitmap to a pixel array.

		GGFX_SourceX
		GGFX_SourceY
		GGFX_SourceWidth
		GGFX_SourceHeight
		GGFX_DestWidth
		GGFX_DestHeight

--------------------------------------------------------------------*/

#ifndef __MORPHOS__
APTR ReadBitMap(struct BitMap *bm, UWORD displayID, PALETTE palette,
	int *pixelformat, int *width, int *height,
	Tag tag1, ...)
{
	return ReadBitMapA(bm, displayID, palette, 
		pixelformat, width, height,
		(TAGLIST) &tag1);
}
#endif

APTR ReadBitMapA(struct BitMap *bm, UWORD displayID, PALETTE palette,
	int *ptr_pixelformat, int *ptr_width, int *ptr_height, TAGLIST tags)
{
	APTR array = NULL;

	if (bm)
	{
		BOOL success;
		
		int sourcex, sourcey, sourcewidth, sourceheight, destwidth, destheight;	
		int width, height, flags, depth, iscyber, bmapformat, pixelformat;
		int bufsize;
		APTR scaleengine = NULL;
		struct RastPort rp, temprp;
	
		sourcex = GetTagData(GGFX_SourceX, 0, tags);
		sourcey = GetTagData(GGFX_SourceY, 0, tags);

		GetBitMapInfo(bm, displayID,
			BMAPATTR_Width, (IPTR)&width,
			BMAPATTR_Height, (IPTR)&height,
			BMAPATTR_Depth, (IPTR)&depth,
			BMAPATTR_Flags, (IPTR)&flags,
			BMAPATTR_CyberGFX, (IPTR)&iscyber,
			BMAPATTR_BitMapFormat, (IPTR)&bmapformat,
			BMAPATTR_PixelFormat, (IPTR)&pixelformat,
			TAG_DONE);

		sourcewidth = GetTagData(GGFX_SourceWidth, width, tags);
		sourceheight = GetTagData(GGFX_SourceHeight, height, tags);
		destwidth = GetTagData(GGFX_DestWidth, width, tags);
		destheight = GetTagData(GGFX_DestHeight, height, tags);


		success = TRUE;


		//	insert bitmap into rastport, create temp rastport

		InitRastPort(&rp);
		rp.BitMap = bm;
		TurboCopyMem(&rp, &temprp, sizeof(struct RastPort));
		temprp.Layer = NULL;
		if (!(temprp.BitMap = AllocBitMap(width, 1, depth, flags, bm)))
		{
			success = FALSE;
		}


		//	create scale engine

		if (success)
		{
			if (sourcewidth != destwidth || sourceheight != destheight)
			{
				if (!(scaleengine = CreateScaleEngine(
					sourcewidth, sourceheight,
					destwidth, destheight,
					RND_RMHandler, (IPTR)MemHandler,
					RND_PixelFormat, pixelformat,
					TAG_DONE)))
				{
					success = FALSE;
				}
			}
		}


		//	get buffer

		if (success)
		{
			if (pixelformat == PIXFMT_0RGB_32)
			{
				bufsize = destwidth * destheight * PIXELSIZE(pixelformat);
			}
			else
			{
				bufsize = ((destwidth+15) & ~15) * destheight;
			}

			DB(kprintf("!readbitmap alloc\n"));

			if (!(array = AllocRenderVec(MemHandler, bufsize)))
			{
				success = FALSE;
			}
		}



		//	convert data
		
		if (success)
		{
			success = FALSE;

			if (scaleengine)
			{

				//	with scaling

				switch (bmapformat)
				{
					case PIXFMT_BITMAP_RGB:
					{
						ULONG *linebuffer;

						DB(kprintf("~reading/scaling RGB bitmap\n"));
						
						if ((linebuffer = AllocRenderVec(MemHandler, sourcewidth * 4)))
						{
							struct Hook readhook;
							struct readrgbdata hookdata;
		
							hookdata.rp = &rp;
							hookdata.sourcex = sourcex;
							hookdata.sourcey = sourcey;
							hookdata.sourcewidth = sourcewidth;
							hookdata.lastline = -1;
	
							readhook.h_Data = &hookdata;
#ifdef __MORPHOS__
							readhook.h_Entry = (HOOKFUNC) &readrgb;
#elif defined(__AROS__)
    	    	    	    	    	    	    	readhook.h_Entry = HookEntry;
							readhook.h_SubEntry = (HOOKFUNC)readrgbfunc;
#else
							readhook.h_Entry = (HOOKFUNC) readrgbfunc;
#endif

							Scale(scaleengine, linebuffer, array,
								RND_SourceWidth, 0,
								RND_LineHook, (IPTR)&readhook,
								TAG_DONE);

							FreeRenderVec(linebuffer);
							success = TRUE;
						}
						break;
					}
					case PIXFMT_BITMAP_CLUT:
					{
						UBYTE *linebuffer;

						DB(kprintf("~reading/scaling CLUT bitmap\n"));

						if ((linebuffer = AllocRenderVec(MemHandler, ((sourcewidth+15) & ~15))))
						{
							struct Hook readhook;
							struct readclutdata hookdata;
		
							hookdata.rp = &rp;
							hookdata.temprp = &temprp;
							hookdata.sourcex = sourcex;
							hookdata.sourcey = sourcey;
							hookdata.sourcewidth = sourcewidth;
							hookdata.lastline = -1;
	
							readhook.h_Data = &hookdata;
#ifdef __MORPHOS__
							readhook.h_Entry = (HOOKFUNC) &readclut;
#elif defined(__AROS__)
    	    	    	    	    	    	    	readhook.h_Entry = HookEntry;
							readhook.h_SubEntry = (HOOKFUNC)readclutfunc;
#else
							readhook.h_Entry = (HOOKFUNC) readclutfunc;
#endif

							Scale(scaleengine, linebuffer, array,
								RND_SourceWidth, 0,
								RND_LineHook, (IPTR)&readhook,
								TAG_DONE);

							FreeRenderVec(linebuffer);
							success = TRUE;
						}
						break;					
					}

					case PIXFMT_BITMAP_HAM6:
					case PIXFMT_BITMAP_HAM8:
					{
						UBYTE *linebuffer;
						ULONG *rgbbuffer;

						DB(kprintf("~reading/scaling HAM bitmap\n"));

						if ((linebuffer = AllocRenderVec(MemHandler, ((sourcewidth + sourcex + 15) & ~15))))
						{
							if ((rgbbuffer = AllocRenderVec(MemHandler, sourcewidth * 4)))
							{
								struct Hook readhook;
								struct readhamdata hookdata;
			
								hookdata.rp = &rp;
								hookdata.temprp = &temprp;
								hookdata.sourcex = sourcex;
								hookdata.sourcey = sourcey;
								hookdata.sourcewidth = sourcewidth;
								hookdata.linebuffer = linebuffer;
								hookdata.palette = palette;
								hookdata.colormode = (depth == 8) ? COLORMODE_HAM8 : COLORMODE_HAM6;
								hookdata.lastline = -1;
		
								readhook.h_Data = &hookdata;
#ifdef __MORPHOS__
								readhook.h_Entry = (HOOKFUNC) &readham;
#elif defined(__AROS__)
    	    	    	    	    	    	    	    	readhook.h_Entry = HookEntry;
								readhook.h_SubEntry = (HOOKFUNC) readhamfunc;
#else
								readhook.h_Entry = (HOOKFUNC) readhamfunc;
#endif
	
								Scale(scaleengine, rgbbuffer, array,
									RND_SourceWidth, 0,
									RND_LineHook, (IPTR)&readhook,
									TAG_DONE);
	
								FreeRenderVec(rgbbuffer);
								success = TRUE;
							}
							FreeRenderVec(linebuffer);
						}
						break;					
					}
				}
			
			}
			else
			{
				//	no scaling
				
				switch (bmapformat)
				{
					case PIXFMT_BITMAP_RGB:
					{
						DB(kprintf("~reading RGB bitmap\n"));
					
						ReadPixelArray(array, 0,0, destwidth * 4, &rp,
							sourcex, sourcey, destwidth, destheight, RECTFMT_ARGB);
						success = TRUE;
						break;
					}

					case PIXFMT_BITMAP_CLUT:
					{
						int i;
						char *p = array;

						DB(kprintf("~reading CLUT bitmap\n"));

						for(i = 0; i < destheight; ++i)
						{
							ReadPixelLine8(&rp, sourcex, sourcey + i, destwidth, p, &temprp);
							p += destwidth;
						}
						success = TRUE;
						break;
					}
					
					case PIXFMT_BITMAP_HAM6:
					case PIXFMT_BITMAP_HAM8:
					{
						UBYTE *linebuffer;

						DB(kprintf("~reading HAM bitmap\n"));

						if ((linebuffer = AllocRenderVec(MemHandler, ((destwidth+15) & ~15) )))
						{
							int i;
							ULONG *p = array;
							for(i = 0; i < destheight; ++i)
							{
								ReadPixelLine8(&rp, sourcex, sourcey + i, destwidth, linebuffer, &temprp);

								Chunky2RGB(linebuffer, destwidth, 1, p, palette,
									RND_ColorMode, ((depth == 8) ? COLORMODE_HAM8 : COLORMODE_HAM6),
									TAG_DONE);

								p += destwidth;
							}
							FreeRenderVec(linebuffer);
							success = TRUE;
						}
						break;
					}

				}
			}
		
		}
		
		
			

		//	close down

		if (success)
		{
			*ptr_pixelformat = pixelformat;
			*ptr_width = destwidth;
			*ptr_height = destheight;
		}
		else
		{
			if (array)
			{
				FreeRenderVec(array);
				array = NULL;
			}
		}
		
		if (scaleengine)
		{
			DeleteScaleEngine(scaleengine);
		}

		if (temprp.BitMap)
		{
			FreeBitMap(temprp.BitMap);
		}

	}

	return array;
}


