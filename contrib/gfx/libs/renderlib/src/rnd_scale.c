
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <render/renderhooks.h>
#include <utility/hooks.h>
#include <proto/utility.h>
#include <proto/exec.h>


struct TxLine
{
	WORD w;
	WORD x0;
	LONG stx, sty, dtx, dty;
};


#define NUMV		4

static void inittexture(WORD *coords, WORD *texcoords, LONG width, LONG height, struct TxLine *txline)
{
	LONG upperindex, index, lowery, uppery, j, k;
	LONG bordertableft[8], bordertabright[8];
	LONG *bpl, *bpr;
	LONG y, indexleft, indexright, ycountleft, ycountright;
	FLOAT stxl, styl, aktleftx, stxr, styr, aktrightx, f;
	FLOAT dtxl, dtyl, deltaxleft, dtxr, dtyr, deltaxright;

	LONG turnindex = 
		((coords[2]-coords[0]) * (coords[5]-coords[1]) -
		(coords[4]-coords[0]) * (coords[3]-coords[1])) > 0 ? 1 : -1;


	upperindex = 0;
	index = 0;
	lowery = uppery = (LONG) coords[(index<<1)+1];
	j = 1;

	do
	{
		index = j;
		y = (LONG) coords[(index<<1)+1];
		if (y < uppery)
		{
			upperindex = j;
			uppery = y;
		}
		if (y > lowery)
		{
			lowery = y;
		}
	} while (++j < NUMV);
		
		
	if (uppery >= lowery) return;
	
 	bpl = bordertableft;
 	k = 2;

	for(;;)
	{
		index = upperindex;

		for(;;)
		{		
			j = index - turnindex;
			if (j == NUMV)
			{
				j = 0;
			}
			else if (j < 0)
			{
				j = NUMV - 1;
			}
	
			if ((LONG) coords[(j<<1)+1] != uppery)
			{
				break;
			}

			index = j;
		}
		
		while ((LONG) coords[(index<<1)+1] < lowery)
		{
			*bpl++ = index << 1;
			index -= turnindex;
			if (index == NUMV)
			{
				index = 0;
			}
			else if (index < 0)
			{
				index = NUMV-1;
			}
		}
		
		*bpl = index << 1;
		
		if (--k == 0) break;
		
		bpl = bordertabright;
		turnindex = -turnindex;
	}

	
	txline += uppery;

	
	bpl = bordertableft;
	bpr = bordertabright;
	
	indexleft = *bpl;
	indexright = *bpr;

	ycountleft = 0;
	ycountright = 0;	

	y = uppery;


	do
	{
		if (ycountleft == 0)
		{
			stxl = texcoords[indexleft];
			styl = texcoords[indexleft+1];
			aktleftx = coords[indexleft];
			ycountleft = - (LONG) coords[indexleft+1];
			indexleft = *(++bpl);
			ycountleft += (LONG) coords[indexleft+1];
			f = 1 / (FLOAT) ycountleft;
			dtxl = (texcoords[indexleft]-stxl)*f;
			dtyl = (texcoords[indexleft+1]-styl)*f;
			deltaxleft = (coords[indexleft]-aktleftx)*f;
		}

		if (ycountright == 0)
		{
			stxr = texcoords[indexright];
			styr = texcoords[indexright+1];
			aktrightx = coords[indexright];
			ycountright = - (LONG) coords[indexright+1];
			indexright = *(++bpr);
			ycountright += (LONG) coords[indexright+1];
			f = 1 / (FLOAT) ycountright;
			dtxr = (texcoords[indexright]-stxr)*f;
			dtyr = (texcoords[indexright+1]-styr)*f;
			deltaxright = (coords[indexright]-aktrightx)*f;
		}


		if (y >= 0)
		{
			LONG x0, x1, w;

			if (y >= height) return;

			x0 = (LONG) aktleftx;
			x1 = (LONG) aktrightx;
			w = x1 - x0;
			
			if (w > 0)
			{
				LONG stx,sty;
				LONG dtx,dty;

				stx = stxl * 65536;
				sty = styl * 65536;
				f = 65536 / (FLOAT) w;
				dtx = (LONG) ((stxr-stxl)*f);
				dty = (LONG) ((styr-styl)*f);
				
				if (x0 < 0)
				{
					stx -= x0*dtx;
					sty -= x0*dty;
					w += x0;
					x0 = 0;
				}
				if (x1 >= width)
				{
					w -= x1-width+1;
				}

				if (w > 0)
				{
					txline->w = w;
					txline->x0 = x0;
					txline->stx = stx;
					txline->sty = sty;
					txline->dtx = dtx;
					txline->dty = dty;
				}
			}
		}

		aktleftx += deltaxleft;
		aktrightx += deltaxright;
		stxl += dtxl;
		styl += dtyl;
		stxr += dtxr;
		styr += dtyr;
		
		ycountleft--;
		ycountright--;
		txline++;

	} while (++y < lowery);
}


static void scale_text_clut(RNDSCALE *sce, struct ScaleData *data, UBYTE *src, UBYTE *dst, LONG dy, WORD tsw)
{
	struct TxLine *txline = ((struct TxLine *) data->data) + dy;
	LONG stx = txline->stx;
	LONG sty = txline->sty;
	LONG dtx = txline->dtx;
	LONG dty = txline->dty;
	WORD w = txline->w;

	dst += txline->x0;
	
	while (w--)
	{
		*dst++ = src[(stx >> 16) + (sty >> 16) * tsw];
		stx += dtx;
		sty += dty;
	}		
}

static void scale_text_rgb(RNDSCALE *sce, struct ScaleData *data, ULONG *src, ULONG *dst, LONG dy, WORD tsw)
{
	struct TxLine *txline = ((struct TxLine *) data->data) + dy;
	LONG stx = txline->stx;
	LONG sty = txline->sty;
	LONG dtx = txline->dtx;
	LONG dty = txline->dty;
	WORD w = txline->w;

	dst += txline->x0;
	
	while (w--)
	{
		*dst++ = src[(stx >> 16) + (sty >> 16) * tsw];
		stx += dtx;
		sty += dty;
	}		
}





static BOOL scale_init(RNDSCALE *sce, struct ScaleData *data)
{
	if (sce->usecoords)
	{
		data->data = AllocRenderVecClear(sce->rmh, sizeof(struct TxLine) * sce->dh);
		if (data->data)
		{
			WORD tc[8];
			tc[0] = 0; tc[1] = 0;
			tc[2] = sce->sw-1; tc[3] = 0;
			tc[4] = sce->sw-1; tc[5] = sce->sh-1;
			tc[6] = 0; tc[7] = sce->sh-1;
			inittexture(sce->coords, tc, sce->dw, sce->dh, data->data);
			return TRUE;
		}
		return FALSE;
	}

	data->cy = 0;
	data->sourcey = 0;

	data->deltax = sce->sw - 1;
	data->deltax /= (sce->dw - 1);

	data->deltay = sce->sh - 1;
	data->deltay /= (sce->dh - 1);

	return TRUE;
}

static void scale_exit(RNDSCALE *sce, struct ScaleData *data)
{
	if (sce->usecoords) FreeRenderVec(data->data);
}




static void scale_line_clut(RNDSCALE *sce, struct ScaleData *data, UBYTE *src, UBYTE *dst, LONG dy, WORD tsw)
{
	LONG w8 = (sce->dw >> 3) + 1;
	LONG cx = 0;
	LONG dx = data->deltax * 65536;
	src += data->cy * tsw;
	switch (sce->dw & 7)
	{
		do
		{
					*dst++ = src[cx>>16]; cx += dx;
			case 7:	*dst++ = src[cx>>16]; cx += dx;
			case 6:	*dst++ = src[cx>>16]; cx += dx;
			case 5:	*dst++ = src[cx>>16]; cx += dx;
			case 4:	*dst++ = src[cx>>16]; cx += dx;
			case 3:	*dst++ = src[cx>>16]; cx += dx;
			case 2:	*dst++ = src[cx>>16]; cx += dx;
			case 1:	*dst++ = src[cx>>16]; cx += dx;
			case 0:	w8--;

		} while (w8);
	}	

	data->cy = data->sourcey += data->deltay;
}

static void scale_line_rgb(RNDSCALE *sce, struct ScaleData *data, ULONG *src, ULONG *dst, LONG dy, WORD tsw)
{
	LONG w8 = (sce->dw >> 3) + 1;
	LONG cx = 0;
	LONG dx = data->deltax * 65536;
	src += data->cy * tsw;
	switch (sce->dw & 7)
	{
		do
		{
					*dst++ = src[cx>>16]; cx += dx;
			case 7:	*dst++ = src[cx>>16]; cx += dx;
			case 6:	*dst++ = src[cx>>16]; cx += dx;
			case 5:	*dst++ = src[cx>>16]; cx += dx;
			case 4:	*dst++ = src[cx>>16]; cx += dx;
			case 3:	*dst++ = src[cx>>16]; cx += dx;
			case 2:	*dst++ = src[cx>>16]; cx += dx;
			case 1:	*dst++ = src[cx>>16]; cx += dx;
			case 0:	w8--;

		} while (w8);
	}	

	data->cy = data->sourcey += data->deltay;
}

/************************************************************************** 
**
**	createscaleengine
*/

LIBAPI RNDSCALE *CreateScaleEngineA(UWORD sw, UWORD sh, UWORD dw, UWORD dh, struct TagItem *tags)
{
	RNDMH *rmh = (RNDMH *) GetTagData(RND_RMHandler, NULL, tags);
	UWORD format = GetTagData(RND_PixelFormat, PIXFMT_CHUNKY_CLUT, tags);
	APTR scalefunc;
	UWORD bpp;
	RNDSCALE *sce;

	switch (format)
	{
		default: 
			return NULL;

		case PIXFMT_CHUNKY_CLUT:
			scalefunc = (APTR) scale_line_clut;
			bpp = 1;
			break;

		case PIXFMT_0RGB_32:
			scalefunc = (APTR) scale_line_rgb;
			bpp = 4;
			break;
	}
	
	sce = AllocRenderVec(rmh, sizeof(RNDSCALE));
	if (sce)
	{
		UWORD *coords = (UWORD *) GetTagData(RND_DestCoordinates, NULL, tags);
		if (coords)
		{
			sce->usecoords = TRUE;
			TurboCopyMem(coords, sce->coords, sizeof(WORD) * 8);
			if (format == PIXFMT_CHUNKY_CLUT)
			{
				scalefunc = (APTR) scale_text_clut;
			}
			else
			{
				scalefunc = (APTR) scale_text_rgb;
			}
		}
		else
		{
			sce->usecoords = FALSE;
		}
		sce->rmh = rmh;
		sce->sw = sw;
		sce->sh = sh;
		sce->dw = dw;
		sce->dh = dh;
		sce->format = format;
		sce->bpp = bpp;
		sce->scalefunc = (void (*)(RNDSCALE *, struct ScaleData *, APTR, APTR, LONG, WORD)) scalefunc;
		sce->initfunc = scale_init;
		sce->exitfunc = scale_exit;
	}
	return sce;
}

/************************************************************************** 
**
**	deletescaleengine
*/

LIBAPI void DeleteScaleEngine(RNDSCALE *engine)
{
	FreeRenderVec((ULONG *) engine);	
}

/************************************************************************** 
**
**	scale
*/

LIBAPI ULONG ScaleA(RNDSCALE *sce, UBYTE *src, UBYTE *dst, struct TagItem *tags)
{
	ULONG result = CONV_NO_DATA;
	if (sce && src && dst)
	{
		struct ScaleData scdata;
		result = CONV_NOT_ENOUGH_MEMORY;
		if ((*sce->initfunc)(sce, &scdata))
		{
			struct Hook *linehook = (struct Hook *) GetTagData(RND_LineHook, NULL, tags);
			LONG tsw = GetTagData(RND_SourceWidth, sce->sw, tags);
			LONG tdw = GetTagData(RND_DestWidth, sce->dw, tags) * sce->bpp;
			LONG y;

			if (linehook)
			{
				struct RND_LineMessage fetchmsg, rendermsg;
				fetchmsg.RND_LMsg_type = LMSGTYPE_LINE_FETCH;
				rendermsg.RND_LMsg_type = LMSGTYPE_LINE_RENDERED;
	
				result = CONV_CALLBACK_ABORTED;
				for (y = 0; y < sce->dh; ++y)
				{
					fetchmsg.RND_LMsg_row = scdata.sourcey;
					if (!CallHookPkt(linehook, src + scdata.cy * tsw, &fetchmsg)) goto abort;
				
					(*sce->scalefunc)(sce, &scdata, src, dst, y, tsw);
	
					rendermsg.RND_LMsg_row = y;
					if (!CallHookPkt(linehook, dst, &rendermsg)) goto abort;
	
					dst += tdw;
				}
			}
			else
			{
				for (y = 0; y < sce->dh; ++y)
				{
					(*sce->scalefunc)(sce, &scdata, src, dst, y, tsw);
					dst += tdw;
				}
			}
			
			result = CONV_SUCCESS;

abort:		(*sce->exitfunc)(sce, &scdata);
		}
	}
	return result;
}

/************************************************************************** 
**
**	scaleordinate
*/

LIBAPI UWORD ScaleOrdinate(UWORD source, UWORD dest, UWORD ordinate)
{
	FLOAT o = (FLOAT) (source - 1) / (dest - 1);
	o *= ordinate;
	return (UWORD) o;
}

