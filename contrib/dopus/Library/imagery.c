/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed

under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "dopuslib.h"
#include <aros/macros.h>

#define DB(x) ((struct DOpusBase *)x)

struct Image *get_image_data(struct DOpusRemember **,int,int,int,struct BitMap *,struct RastPort *);
void drawline(struct RastPort *rp,int,int,int,int);


/*****************************************************************************

    NAME */

	AROS_LH8(void, DoArrow,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, p, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(int, fg, D4),
	AROS_LHA(int, bg, D5),
	AROS_LHA(int, d,  D6),

/*  LOCATION */
	struct Library *, DOpusBase, 9, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/

{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	int x1,y1,x2,y2,x3,y3,x4,y4,xb;
/* kprintf("DoArrow(rp=%p, x=%d, y=%d, w=%d, h=%d, fg=%d, bg=%d, d=%d)\n"
	, p, x, y, w, h, fg, bg, d);
*/
	SetAPen(p,bg);
	xb=x+w;
	RectFill(p,x,y,xb-1,y+h-1);
	SetAPen(p,fg);
	switch (d) {
		case 0:
			y1=y+h-2;
			y3=y+1;
			goto sameupdown;

		case 1:
			y1=y+1;
			y3=y+h-2;
sameupdown:
			x1=x+2;
			x2=xb-3;
			x3=x+(w/2)-1;
			x4=xb-(w/2);
			y2=y1;
			y4=y3;
			break;

		case 2:
			x1=x+1;
			x3=xb-2;
			goto sameleftright;

		case 3:
			x1=xb-2;
			x3=x+1;
sameleftright:
			x2=x1;
			y1=y+1;
			y2=y+h-2;
			y3=y+(h/2);
			if (h%2==0) --y3;
			x4=x3;
			y4=y3;
			break;
	}
	drawline(p,x1,y1,x3,y3);
	drawline(p,x2,y2,x4,y4);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH7(void, DrawRadioButton,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp,  A0),
	AROS_LHA(int, x, 	D0),
	AROS_LHA(int, y, 	D1),
	AROS_LHA(int, w, 	D2),
	AROS_LHA(int, h, 	D3),
	AROS_LHA(int, hi, 	D4),
	AROS_LHA(int, lo, 	D5),

/*  LOCATION */
	struct Library *, DOpusBase, 83, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	SetAPen(rp,hi);
	drawline(rp,x,y-1,x+w-3,y-1);
	drawline(rp,x-1,y,x,y);
	drawline(rp,x-2,y+1,x-2,y+h-4);
	drawline(rp,x-1,y+1,x-1,y+h-4);
	drawline(rp,x-1,y+h-3,x,y+h-3);
	SetAPen(rp,lo);
	drawline(rp,x,y+h-2,x+w-3,y+h-2);
	drawline(rp,x+w-3,y+h-3,x+w-2,y+h-3);
	drawline(rp,x+w-1,y+h-4,x+w-1,y+1);
	drawline(rp,x+w-2,y+h-4,x+w-2,y+1);
	drawline(rp,x+w-3,y,x+w-2,y);
	
	AROS_LIBFUNC_EXIT
}

void drawline(rp,x1,y1,x2,y2)
struct RastPort *rp;
int x1,y1,x2,y2;
{
	Move(rp,x1,y1);
	Draw(rp,x2,y2);
}

/*****************************************************************************

    NAME */

	AROS_LH7(struct Image *, GetButtonImage,

/*  SYNOPSIS */
	AROS_LHA(int, w, 	D0),
	AROS_LHA(int, h, 	D1),
	AROS_LHA(int, fg, 	D2),
	AROS_LHA(int, bg, 	D3),
	AROS_LHA(int, fpen, 	D4),
	AROS_LHA(int, bpen, 	D5),
	AROS_LHA(struct DOpusRemember **, key, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 84, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	struct Image *image;
	short a,b,depth;
	struct BitMap tbm;
	struct RastPort trp;
	

	b=(fg>bg)?fg:bg;
	if (fpen>b) b=fpen;
	if (bpen>b) b=bpen;

	depth=2; w+=4;
	for (a=0;a<8;a++) if (b&(1<<a)) depth=a+1;
	

	if (!(image=get_image_data(key,w,h,depth,&tbm,&trp))) {
		return(NULL);
	}

#warning AROS: graphics.library functions dont seem to work on standard hand made (InitBitmap = no AllocBitmap) planar bitmaps

#if 0
	SetAPen(&trp,bpen);
	RectFill(&trp,0,0,w-1,h-1);
	DrawRadioButton(&trp,2,1,w-4,h,(fpen!=bpen)?bg:fg,(fpen!=bpen)?fg:bg);

	if (w>10 && h>4 && fpen!=bpen) {
		SetAPen(&trp,fpen);
		RectFill(&trp,4,2,w-7,h-3);
		SetAPen(&trp,bpen);
		WritePixel(&trp,4,2);
		WritePixel(&trp,4,h-3);
		WritePixel(&trp,w-7,2);
		WritePixel(&trp,w-7,h-3);
	}
#else
	if ((w != 19) || (h != 9))
	{
	    kprintf("AROS-DOpus (contrib/dopus/Library/imagery.c): I can handle only 19 x 9 images here\n");
	}
	else
	{
	    static ULONG radio_data1[2*9] =
	    {
		0x00000000,
		0x00018000,
		0x0000C000,
		0x0000C000,
		0x0000C000,
		0x0000C000,
		0x0000C000,
		0x00018000,
		0x1FFF0000,
		
		0x1FFF0000,
		0x30000000,
		0x60000000,
		0x60000000,
		0x60000000,
		0x60000000,
		0x60000000,
		0x30000000,
		0x00000000
	    };

	    static ULONG radio_data2[2*9] =
	    {
                0x1FFF0000,
		0x30000000,
		0x63F80000,
		0x67FC0000,
		0x67FC0000,
		0x67FC0000,
		0x63F80000,
		0x30000000,
		0x00000000,
		
		0x00000000,
		0x00018000,
		0x03F8C000,
		0x07FCC000,
		0x07FCC000,
		0x07FCC000,
		0x03F8C000,
		0x00018000,
		0x1FFF0000
		
	    };

	    ULONG *src = (fpen != bpen) ? radio_data2 : radio_data1;
	    ULONG *dest = (ULONG *)tbm.Planes[0];
	    
	    WORD y;
	
	    for(y = 0; y < 9 * 2; y++)
	    {
	    	ULONG gfx = *src++;
		
	        *dest = AROS_LONG2BE(gfx);
		dest += (tbm.BytesPerRow / 4);
	    }	    
	}
	
#endif

	image->LeftEdge=-2;
	image->TopEdge=-1;
	return(image);
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH4(struct Image *, GetCheckImage,

/*  SYNOPSIS */
	AROS_LHA(int, fg, D0),
	AROS_LHA(int, bg, D1),
	AROS_LHA(int, pen, D2),
	AROS_LHA(struct DOpusRemember **, key, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 87, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	struct Image *image;
	int a,b,depth;
	struct BitMap tbm;
	struct RastPort trp;
	
	b=(fg>bg)?fg:bg;
	depth=2;
	for (a=0;a<8;a++) if (b&(1<<a)) depth=a+1;

	if (!(image=get_image_data(key,13,7,depth,&tbm,&trp))) return(NULL);

	SetDrMd(&trp,JAM1);
	SetAPen(&trp,bg);
	
#warning AROS: graphics.library functions dont seem to work on standard hand made (InitBitmap = no AllocBitmap) planar bitmaps

#if 0
	RectFill(&trp,0,0,12,6);

	if (pen) {
		SetAPen(&trp,fg);
		BltTemplate((char *)DB(DOpusBase)->pdb_check,0,2,&trp,0,0,13,7);
	}
#else
	if (pen) {
	    UWORD *src = (UWORD *)DB(DOpusBase)->pdb_check;
	    UWORD *dest = (UWORD *)tbm.Planes[0];
	    
	    WORD y;
	
	    for(y = 0; y < 7; y++)
	    {
	    	UWORD gfx = *src++;
		
		/* no need to fix endianess here, because the read above plus the write
		   below cause two byte swappings on little endian machines => it ends
		   up as it was in src = big endian = correct */
	        *dest = gfx; /* AROS_WORD2BE(gfx); */
		dest += (tbm.BytesPerRow / 2);
	    }
	}
#endif
	image->LeftEdge=7;
	image->TopEdge=2;

	return(image);
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH8(void, Do3DFrame,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp,  A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(char *, title, A1),
	AROS_LHA(int, hi, D4),
	AROS_LHA(int, lo, D5),

/*  LOCATION */
	struct Library *, DOpusBase, 76, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	char of,ob;
	int a,l;

	of=rp->FgPen; ob=rp->BgPen;
	Do3DBox(rp,x+2,y+1,w-4,h-2,lo,hi);
	Do3DBox(rp,x+4,y+2,w-8,h-4,hi,lo);
	SetAPen(rp,of); SetBPen(rp,ob);
	if (title) {
		a=strlen(title);
		x+=((w-(l=(TextLength(rp,title,a))))/2);
		SetAPen(rp,ob);
		RectFill(rp,
			x-rp->Font->tf_XSize/2,y+2-rp->Font->tf_Baseline,
			x+l+rp->Font->tf_XSize/2,y+2-rp->Font->tf_Baseline+rp->Font->tf_YSize);
		SetAPen(rp,of);
		Move(rp,x,y+2); Text(rp,title,a);
	}
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

/* AROS: we have no __chip keyword */
extern UBYTE /* __chip */ glass_image1[];
extern UBYTE /* __chip */ glass_image2[];

	AROS_LH5(void, DoGlassImage,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp,  A0),
	AROS_LHA(struct Gadget *, gadget, A1),
	AROS_LHA(int, shine,	D0),
	AROS_LHA(int, shadow, 	D1),
	AROS_LHA(int, type, 	D2),

/*  LOCATION */
	struct Library *, DOpusBase, 93, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


	int x,y;
	char op,om;

	rp = &IntuitionBase->ActiveScreen->RastPort;
	
	om=rp->DrawMode; op=rp->FgPen;
	SetDrMd(rp,JAM1);

	if (type==0) {
		x=gadget->LeftEdge+((gadget->Width-16)/2);
		y=gadget->TopEdge+((gadget->Height-8)/2);

		x = y = 0;
		
		SetAPen(rp,shine);
		BltTemplate((char *)glass_image2,0,2,rp,x,y,16,8);
		SetAPen(rp,shadow);
		BltTemplate((char *)glass_image1,0,2,rp,x,y,16,8);
	}

	SetAPen(rp,op);
	SetDrMd(rp,om);
	
	AROS_LIBFUNC_EXIT
}

struct Image *get_image_data(key,width,height,depth,bm,rp)
struct DOpusRemember **key;
int width,height,depth;
struct BitMap *bm;
struct RastPort *rp;
{
	struct Image *image;
	USHORT *data;
	short a,words;
	
	words=(width+15)/16;

	if (!(image=LAllocRemember(key,sizeof(struct Image),MEMF_CLEAR)) ||
		!(data=LAllocRemember(key,words*2*height*depth,MEMF_CHIP|MEMF_CLEAR))) {

		return(NULL);
	}

	InitBitMap(bm,depth,width,height);
	for (a=0;a<depth;a++) bm->Planes[a]=(PLANEPTR)&data[words*height*a];
	InitRastPort(rp);
	
	rp->BitMap=bm;

	image->Width=width;
	image->Height=height;
	image->Depth=depth;
	image->ImageData=data;
	image->PlanePick=(1<<depth)-1;

	return(image);
}
