#include <exec/memory.h>
#include <exec/semaphores.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <intuition/imageclass.h>
#include <cybergraphx/cybergraphics.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/intuition_lib.h>

#else
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#endif

#include "global.h"
#include "myimage.h"
#include "myimage_protos.h"
#include "p2c.h"
#include "remap.h"
#include "pack.h"
#include "config.h"
#include "asmmisc.h"

struct RastPort temprp;
struct BitMap tempbm;
struct SignalSemaphore temprpsem;

void InitMyImage(void)
{
	InitRastPort(&temprp);
	InitSemaphore(&temprpsem);
}

void CleanupMyImage(void)
{
}

BOOL MakeMyImage(struct ReqNode *reqnode,struct MyImage *im,struct MyImageSpec *spec,LONG bgpen,BOOL DoMask)
{
	UBYTE *data,*col,*chunky,*planar,*unpack;
	LONG reg,modulo;
	WORD width,height,numcols,depth,bmflags;
	WORD i,realdepth;
	BOOL rc = TRUE, tc = FALSE, notrans = FALSE;

	ObtainSemaphore(&temprpsem);

	im->flags = 0;

	if (spec->flags & MYIMSPF_NOTRANSP)
	{
		DoMask = FALSE;
		im->flags |= MYIMF_NOTRANSP;
	}
	if (spec->flags & MYIMSPF_PINGPONG)
	{
		im->flags |= MYIMF_PINGPONG;
	}

	data = (UBYTE *)spec->data;
	col = spec->col;
	width = spec->width;
	height = spec->height;
	numcols = spec->numcols;
	depth = 1;
	while(numcols > (1 << depth)) depth++;

	im->cm = reqnode->cm;

	realdepth = GetBitMapAttr( reqnode->scr->RastPort.BitMap, BMA_DEPTH );
	if (realdepth > 8)
	{
		realdepth = 8;

		if( CyberGfxBase )
		{
			if ( ( reqnode->cfg.bitmapflags & (BITMAPF_MINPLANES|BITMAPF_FRIEND) ) == (BITMAPF_MINPLANES|BITMAPF_FRIEND) )
				tc = TRUE;
		}
	}

	modulo = (width + 15) & ~15;

	if (spec->flags & MYIMSPF_CHUNKY)
	{
		if (!(im->chunkydata = AllocVec(modulo * height + 8,MEMF_PUBLIC | MEMF_REVERSE)))
		{
			rc = FALSE;
		} else {
			chunky = im->chunkydata;

			for(i = 0;i < height;i++)
			{
				if (spec->flags & MYIMSPF_PACKED)
				{
					UnpackBytes(data,
									chunky,
									width,
									(APTR *)&data);
				}
				else
				{
					CopyMem(data,chunky,width);
					data += width;
				}

				chunky += modulo;

			} /* for(i = 0;i < height;i++) */

			data = im->chunkydata;

		} /* if (!(im->chunkydata = AllocVec(modulo * height + 8,MEMF_PUBLIC | MEMF_REVERSE | MEMF_CLEAR))) else ... */

	} /* if (spec->flags & MYIMSPF_CHUNKY) */
	else
	{
		if (!(im->chunkydata = AllocVec(modulo * (height + 2) + 8,MEMF_PUBLIC | MEMF_REVERSE | MEMF_CLEAR)))
		{
			rc = FALSE;
		} else {

			planar = data;
			chunky = im->chunkydata;
			unpack = chunky + modulo * height;

			for(i = 0;i < height;i++)
			{
				if (spec->flags & MYIMSPF_PACKED)
				{
					UnpackBytes(planar,unpack,(modulo / 8) * depth,(APTR *)&planar);
					Planar2Chunky(unpack,chunky,modulo,depth);
				} else {
					Planar2Chunky(planar,chunky,modulo,depth);
					planar += (modulo / 8) * depth;
				}
				chunky += modulo;
			}

			data = im->chunkydata;

		} /* if (!(im->chunkydata = AllocVec(modulo * (height + 2) + 8,MEMF_PUBLIC | MEMF_REVERSE | MEMF_CLEAR))) else ... */

	} /* if (spec->flags & MYIMSPF_CHUNKY) else ... */

	if (rc)
	{
		im->width = width;
		im->height = height;
		im->depth = depth;
		im->numcols = numcols;

		im->framewidth = spec->framewidth;
		im->frameheight = spec->frameheight;
		im->frames = spec->frames;
		im->animspeed = spec->animspeed;

		if (!(im->coltable = AllocVec(im->numcols * (tc ? sizeof(ULONG) : sizeof(WORD)),MEMF_PUBLIC|MEMF_CLEAR)))
		{
			rc = FALSE;
		} else {

			if (im->flags & MYIMF_NOTRANSP) notrans = TRUE;
			if (DoMask==FALSE);if(bgpen==-1) notrans = TRUE;

			if( ! tc )
			{


				if (!(notrans))
				{
					im->coltable[0] = bgpen;
				}

				for(i = notrans ? 0 : 1;i < numcols;i++)
				{
					reg = ObtainBestPen(reqnode->cm,
										  RGB32(col[i * 3]),
										  RGB32(col[i * 3 + 1]),
										  RGB32(col[i * 3 + 2]),
										  OBP_Precision,PRECISION_EXACT,
										  OBP_FailIfBad,FALSE,
										  TAG_DONE);

					im->coltable[i] = reg;
				}

				im->flags |= MYIMF_PENSALLOCED;

			} /* if( ! tc ) */
			else
			{
				ULONG	triplet[3],
						*dst = (ULONG *) im->coltable;
				UBYTE	*src = col;

				if( ! ( notrans ) )
				{
					if (reqnode->rtg)
					{
						*dst++ = bgpen;
					} else {
						GetRGB32( reqnode->cm, bgpen, 1L, triplet );
			 			*dst++ = ( triplet[0] & 0xff0000 ) | ( triplet[1] & 0xff00 ) | ( triplet[2] & 0xff );
					}
		 			i = 1;src += 3;
				}
				else i = 0;

				for( ; i < numcols; i++ )
				{
					*dst++ = ( src[0] << 16 ) | ( src[1] << 8) | ( src[2] );
					src += 3;
				}
			}

			bmflags = BMF_CLEAR;

			i = reqnode->cfg.bitmapflags;

			if (DoMask && (!reqnode->rtg) &&
				 (!(i & BITMAPF_INTERLEAVED)))
			{
				/* avoid interleaved standard Amiga
				   bitmaps because mask would eat too
				   much CHIP RAM. User can still have
				   interleaved bitmaps/masks if in cfg
				   he sets "bitmapflags = FI" */

				if (GetBitMapAttr(reqnode->scr->RastPort.BitMap,BMA_FLAGS) & BMF_INTERLEAVED)
				{
					i &= ~BITMAPF_FRIEND;
				}
			}

			if (i & BITMAPF_DISPLAYABLE) bmflags |= BMF_DISPLAYABLE;
			if (i & BITMAPF_MINPLANES) bmflags |= BMF_MINPLANES;

			if (!(im->bm = AllocBitMap(width,
												height + 1,
												realdepth,
												bmflags,
												((i & BITMAPF_FRIEND) ? reqnode->scr->RastPort.BitMap : 0)
												)))
			{
				rc = FALSE;
			}
		}

	} /* if (rc) */


	if (rc)
	{

	#if 1

		if( DoMask )
		{
			LONG	bwidth = GetBitMapAttr(im->bm,BMA_WIDTH);
			LONG	bpr = bwidth / 8;
			BOOL	interleaved  = ( GetBitMapAttr( im->bm, BMA_FLAGS ) & BMF_INTERLEAVED ) != 0L;
			WORD	maskplanes = (interleaved ? realdepth : 1);

			if ((im->mask = AllocVec(maskplanes * (bpr * (height + 1)),
											 reqnode->rtg ? MEMF_PUBLIC : MEMF_CHIP )))
			{

			#ifndef NO_ASM

				MakeMask(im->mask,data,bwidth,modulo,height,maskplanes);

			#else

				UBYTE	*dst = (UWORD *) ((UBYTE*)im->mask + (bpr * maskplanes));

				chunky = data;

				for( i = 0; i < height; i++ )
				{
					UWORD	val = 128, word = 0, x;

					for( x = 0; x < width; x++ )
					{
						if( *chunky++ )
							word += val;

						if( ! ( val /= 2 ) )
						{
							*dst++ = word;
							word = 0; val = 128;
						}
					}

					if( val ) *dst++ = word;
					chunky += modulo - width;

					/* if bitmap is not aligned to 16 but to
					   32, 48, 64, ... dst must be adjusted */

					dst += (bwidth - modulo) / 8;

					if( interleaved )
					{
						UWORD	*src = (UWORD *) ((UBYTE *)dst-bpr);

						x = ((realdepth-1) * bpr)/2;

						do *dst++ = *src++;
						while( --x );
					}

				}

			#endif

			} /* if ((im->mask = AllocVec((bwidth * (height + 1)) / 8, reqnode->rtg ? MEMF_PUBLIC : MEMF_CHIP ))) */

		} /* if (DoMask) */

	#else
		while( DoMask )
		{
			struct BitMap	*tbm = NULL;

			if( ! tc )
			{
				temprp.BitMap = im->bm;
			}
			else
			{
				if( ! ( temprp.BitMap = tbm = AllocBitMap( (width+15)&~15, height+1, realdepth, 0L, NULL ) ) )
					break;
			}

			chunky = data;
			for (i = 0;i < height;i++)
			{
				WritePixelLine8(&temprp,0,i + 1,(width + 7) & ~7,chunky,&temprp);
				chunky += modulo;
			}

			if ((im->mask = AllocVec((GetBitMapAttr(im->bm,BMA_WIDTH) * (height + 1)) / 8,
										 reqnode->rtg ? MEMF_CLEAR | MEMF_PUBLIC :
															 MEMF_CHIP | MEMF_CLEAR )))
			{
				InitBitMap(&tempbm,realdepth,GetBitMapAttr(im->bm,BMA_WIDTH),height + 1);

				for(i = 0;i < realdepth;i++)
				{
					tempbm.Planes[i] = im->mask;
				}

				if( tbm )
					BltBitMap(tbm,0,1,&tempbm,0,1,width,height,0xEE,255,0);
				else
					BltBitMap(im->bm,0,1,&tempbm,0,1,width,height,0xEE,255,0);
			}
			FreeBitMap( tbm );

			break;

		} /* while (DoMask) */

	#endif

		temprp.BitMap = im->bm;

		if( tc )
		{ /*LUT*/
			WriteLUTPixelArray(data, 0, 0, modulo, &temprp, im->coltable,
			0,1, (width + 7) & ~7, height,  CTABFMT_XRGB8 );
		}
		else
		{

			RemapBytes(data,im->coltable,modulo * height + 1);

			chunky = data;
			for (i = 1;i <= height;i++)
			{
				WritePixelLine8(&temprp,0,i,(width + 7) & ~7,chunky,&temprp);
				chunky += modulo;
			}
		}

		if (im->chunkydata)
		{
			WaitBlit();
			FreeVec(im->chunkydata);
			im->chunkydata = 0;
		}

		im->flags |= MYIMF_IMAGEOK;

	} /* if (rc) */

	if (!rc) FreeMyImage(im);

	ReleaseSemaphore(&temprpsem);

	return rc;
}

void AllocMyImageHelpBM(struct MyImage *im,struct BitMap *blitbm)
{
	/* needed to blit animated logos correctly over
	   a background pattern */

	im->helpbm = AllocBitMap(im->framewidth,
									 im->frameheight * 2,
									 GetBitMapAttr(blitbm,BMA_DEPTH),
									 BMF_MINPLANES,
									 blitbm);
}

void SetupMyImageHelpBM(struct MyImage *im,struct RastPort *rp,WORD x,WORD y)
{
	/* needed to blit animated logos correctly over
	   a background pattern */

	if (im->helpbm)
	{
		ObtainSemaphore(&temprpsem);

		temprp.BitMap = im->helpbm;
		ClipBlit(rp,x,y,&temprp,0,0,im->framewidth,im->frameheight,192);

		ReleaseSemaphore(&temprpsem);
	}
}

void FreeMyImageMask(struct MyImage *im)
{
	if (im->mask)
	{
		FreeVec(im->mask);
		im->mask = 0;
	}
}

void FreeMyImage(struct MyImage *im)
{
	WORD i;

	if (GfxBase) WaitBlit();

	if (im->bm)
	{
		FreeBitMap(im->bm);
		im->bm = 0;
	}

	if (im->helpbm)
	{
		FreeBitMap(im->helpbm);
		im->helpbm = 0;
	}

	FreeMyImageMask(im);

	if (im->chunkydata)
	{
		FreeVec(im->chunkydata);
		im->chunkydata = 0;
	}

	if (im->flags & MYIMF_PENSALLOCED)
	{
		for(i = (im->flags & MYIMF_NOTRANSP) ? 0 : 1;i < im->numcols;i++)
		{
			if (im->coltable[i] != -1)
			{
				ReleasePen(im->cm,im->coltable[i]);
			}
		}
		im->flags &= ~MYIMF_PENSALLOCED;
	}

	if (im->coltable)
	{
		FreeVec(im->coltable);
		im->coltable = 0;
	}

	im->flags &= ~MYIMF_IMAGEOK;
}

void DrawMyImage(struct RastPort *rp,struct MyImage *im,WORD x,WORD y,WORD frame)
{
	WORD a;

	a = frame * im->framewidth;

	if (im->mask)
	{
		if (im->helpbm)
		{
			BltBitMap(im->helpbm,0,0,im->helpbm,0,im->frameheight,im->framewidth,im->frameheight,192,255,0);

			ObtainSemaphore(&temprpsem);
			temprp.BitMap = im->helpbm;
			BltMaskBitMapRastPort(im->bm,a,1,&temprp,0,im->frameheight,im->framewidth,im->frameheight,ANBC|ABNC|ABC,im->mask);
			ReleaseSemaphore(&temprpsem);

			BltBitMapRastPort(im->helpbm,0,im->frameheight,rp,x,y,im->framewidth,im->frameheight,192);

		} else {
			BltMaskBitMapRastPort(im->bm,a,1,rp,x,y,im->framewidth,im->frameheight,ANBC|ABNC|ABC,im->mask);
		}
	} else {
		BltBitMapRastPort(im->bm,a,1,rp,x,y,im->framewidth,im->frameheight,192);
	}
}

