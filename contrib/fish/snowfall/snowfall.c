#include <aros/oldprograms.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <exec/memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Avoid conflicts */
#define BitMapHeader MyBitMapHeader
#define ColorRegister MyColorRegister
#include "myiff.h"
#include <ctype.h>

#include <proto/graphics.h>
#include <proto/exec.h>

#define MAXSNOWMASS 200

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Screen *s,*bs;
struct Window *w;
struct	NewScreen ns={0,0,320,256,5,1,2,0,CUSTOMSCREEN,0,0,0,0};
struct NewWindow nw={0,0,320,256,-1,-1,CLOSEWINDOW,WINDOWCLOSE|BORDERLESS|
	SMART_REFRESH|NOCAREREFRESH|ACTIVATE,0,0,0,0,0,0,0,0,0,CUSTOMSCREEN};
UWORD BlackColors[32];

int LoadPicture( char *);
int  GetBMHD(FILE *fd, struct BitMapHeader *bmhd, long *formsize);
int GetCMap(FILE *fd,long *formsize,UWORD *colortable,UWORD *modes);
int LoadBitMap (FILE *fd, struct ViewPort *vp, struct BitMapHeader *header);
void loc_abort(char *errortext);

void OpenAll(filename,backup)
char *filename;
int backup;
{
	long sec,mic;
	
	if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0L))==0) loc_abort("No gfxbase");
	if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0L))==0)
		loc_abort("No intuition");
	CurrentTime(&sec,&mic);
	srand((int)mic);
	if ((LoadPicture(filename))==0) loc_abort("Can't load picture");
	if (backup) 
		BltBitMap(&s->BitMap,0L,0L,&bs->BitMap,0L,0L,(long)s->Width,(long)s->Height,0xC0L,0xFFL,0L);
}

void loc_abort(errortext)
char *errortext;
{
	if (errortext!=0) puts(errortext);
	if (w) CloseWindow(w);
	if (bs) CloseScreen(bs);
	if (s) CloseScreen(s);
	if (GfxBase) CloseLibrary((struct Library *)GfxBase);
	if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	exit(0);
}

int LoadPicture(filename)
char *filename;
{
	FILE  	*fd;
	struct	BitMapHeader bmhd;
	long  	formsize;
	UWORD 	*colortable[32],modes;
	
	if (!(fd = fopen (filename, "r")))
		return(0);
	if (GetBMHD(fd,&bmhd,&formsize)==-1)
		return(0);

	if (GetCMap(fd,&formsize,colortable,&modes)==-1)
		return(0);
/* buildupdascreen */
	if (modes==0xffff)
	{
		modes=0;
		if (bmhd.w>452) modes|=HIRES;
		if (bmhd.h>352) modes|=LACE;
	}
	ns.LeftEdge=bmhd.x;
	ns.TopEdge=bmhd.y;
	ns.Width=bmhd.w;
	ns.Height=bmhd.h;
	ns.Depth=bmhd.nplanes;
	ns.ViewModes=modes;
	ns.Type=CUSTOMSCREEN;
	if ((bs=OpenScreen(&ns))==0) loc_abort("Can't open backupscreen");
	if ((s=(struct Screen *)OpenScreen(&ns))==0) loc_abort("No memory for screen");
	LoadRGB4(&s->ViewPort,BlackColors,(long)1<<bmhd.nplanes);
	nw.Screen=s;
	nw.Width=bmhd.w;
	nw.Height=bmhd.h;
	if ((w=(struct Window *)OpenWindow(&nw))==0) loc_abort("No window here");
	if (LoadBitMap(fd,&s->ViewPort,&bmhd)==-1) return(0);
	LoadRGB4(&s->ViewPort,colortable,(long)1<<bmhd.nplanes);
	return(1);
}

int GetCMap(fd,formsize,colortable,modes)
FILE *fd;
long *formsize;
UWORD *colortable;
UWORD *modes;
{
	struct ChunkHeader ch;
	register UBYTE *ctable;
	register int i,n;
	ULONG temp=0;
	
	*modes=0xffff;
	while (*formsize > 0) 
	{
		if (!fread (&ch, sizeof (ch), 1, fd))
			return(-1);
		*formsize -= sizeof (ch);

		switch (ch.TYPE)
		{
		case CMAP:
		{
			ctable=(UBYTE *)AllocMem((long)ch.chunksize,0L);
			fread (ctable, (int) ch.chunksize, 1, fd);

			for (i = n = 0; n < ch.chunksize; i++, n+=3)
				colortable[i] = ((ctable[n]>>4 ) << 8) +
					  ((ctable[n+1]>>4 ) << 4) +
					  ( ctable[n+2]>>4 );
			FreeMem(ctable,(long)ch.chunksize);
			
			break;
		}

		case CAMG:
			fread (&temp, (int) ch.chunksize, 1, fd);
			*modes = (UWORD) (temp & 0xffffffff);
			break;

		case BODY:
			return(0);
			break;

		case CRNG:
		case GRAB:
		case SPRT:
		case DPPV:	/*  Anyone know what this one is for?  */
		case DPPS:	/*  Anyone know what this one is for?  */
			fseek (fd, ch.chunksize, 1);
			break;

		default:
			printf("unrecognized chunk 0x%lx\n",ch.TYPE);
			fseek (fd, ch.chunksize, 1);
		}

		*formsize -= ch.chunksize;
		if (ch.chunksize & 1)		/*  Odd length chunk  */
		{
			*formsize --;
			fseek (fd, 1L, 1);
		}
	}
	return(-1);
}
	
int  GetBMHD(fd,bmhd,formsize)
FILE *fd;
struct BitMapHeader *bmhd;
long *formsize;
{
	long subtype;
	struct ChunkHeader ch;
	if (!fread (&ch, sizeof (ch), 1, fd))
	{
		return(-1);
	}
	
	if (ch.TYPE != FORM)
	{
printf("GetBMHD: alive !!\n");
		return(-1);
	}
	*formsize=ch.chunksize;
	
	if (!fread (&subtype, sizeof (subtype), 1, fd))
		return (-1);
	*formsize -= sizeof (subtype);

	if (subtype != ILBM)
		return(-1);

	if (!fread (&ch, sizeof (ch), 1, fd))
		return(-1);
	*formsize -= sizeof (ch);

	fread (bmhd, (int) ch.chunksize, 1, fd);

	*formsize -= ch.chunksize;
	return(0);
}

/* This routine is a fairly direct copy from Leo Schwab's Yaiffr - tanx */

int LoadBitMap (fd, vp, header)
FILE *fd;
struct ViewPort *vp;
struct BitMapHeader *header;
{
	register struct BitMap	*bm;
	register int		i, n;
	long			plane_offset = 0;

	bm = vp -> RasInfo -> BitMap;

	if (header->Compression != cmpNone &&
	    header->Compression != cmpByteRun1)
		return(-1);

	for (i=0; i < bm->Rows; i++) {
		for (n=0; n < bm->Depth; n++) {
/*- - - - - - - - - - -*/
if (!header->Compression) {	/* No compression */
	if (!fread (bm -> Planes[n] + plane_offset,
		    bm -> BytesPerRow, 1, fd))
		return(-1);

} else {
	int		so_far;
	register UBYTE	*dest = bm -> Planes[n] + plane_offset;
	char		len;

	/*
	 * Note:  All file I/O after this point is assumed to be sucessful.
	 * This is clearly a poor assumption, but it saves on typing.
	 * And besides, putting the checking in is simple :-) :-).
	 */
	so_far = bm -> BytesPerRow;
	while (so_far > 0) {
		if ((len = getc (fd)) >= 0) {	/*  Literal byte copy  */
			so_far -= ++len;
			if (!fread (dest, len, 1, fd)) return(-1);
			dest += len;

		} else if ( len == -128)	/*  NOP  */
			;

		else if (len < 0) {		/*  Replication count  */
			UBYTE	byte;

			len = -len + 1;
			so_far -= len;
			byte = getc (fd);
			while (--len >= 0)
				*dest++ = byte;
		}
	}
	if (so_far)
		return(-1);
}
/*- - - - - - - - - - -*/
		}
		plane_offset += bm -> BytesPerRow;
	}
	return(0);
}

void DoSnow(mass,refresh,wind)
long mass,refresh,wind;
{
	struct IntuiMessage *msg;
	register long cx,cy,i,end,oldx,*x,*y,timer;
	
	timer=refresh;
	x=AllocMem(800L,0L);
	y=AllocMem(800L,0L);
	if (mass>MAXSNOWMASS) mass=MAXSNOWMASS;
	for (i=0;i<mass;i++) y[i]=-(rand()%w->Height);
	while ((msg=GetMsg(w->UserPort))==0)
	{
		if (refresh!=-1)
		{
			if (--refresh==0)
			{
				for (i=0;i<mass;i++)
					*(bs->BitMap.Planes[0]+(s->Width/8)*y[i]+x[i]/8) |= (128>>(x[i]%8));
				BltBitMap(&bs->BitMap,0L,0L,&s->BitMap,0L,0L,(long)s->Width,(long)s->Height,0xC0L,0xFFL,0x00L);
				for (i=0;i<mass;i++)
					*(bs->BitMap.Planes[0]+(s->Width/8)*y[i]+x[i]/8) &= ~(128>>(x[i]%8));
				refresh=timer;
			}
		}
		for (i=0;i<mass;i++)
		{
			if (y[i]<-1) y[i]++;
			else
			{
				cx=x[i];
				cy=y[i];
				if (cy==-1) {cy=0;cx=rand()%w->Width;}
				else
				{
					oldx=cx;
					cx+=(rand()%31-wind-15)/8;
					if (cx>s->Width-1) cx-=s->Width;
					if (cx<0) cx+=s->Width;
					if (ReadPixel(w->RPort,cx,cy+1)==0)
					{
						*(s->BitMap.Planes[0]+(s->Width/8)*cy+oldx/8)&=~(128>>(oldx%8));
						cy++;
						*(s->BitMap.Planes[0]+(s->Width/8)*cy+cx/8) |= (128>>(cx%8));
					}
					else
					{
						end=1;
						if ((ReadPixel(w->RPort,cx+1,cy+1)==0)&&
							(ReadPixel(w->RPort,cx-1,cy+1)==0))
						{
							if ((cx>0)&&(rand()%2==1)) cx--;
							else if (cx<w->Width-1) cx++;
								else cx--;
							end=0;
						}
						if ((ReadPixel(w->RPort,cx+1,cy+1)==0)&&(end==1)/* &&(rand()%6!=1) */)
							{cx++; end=0;}
						if ((ReadPixel(w->RPort,cx-1,cy+1)==0)&&(end==1)/* &&(rand()%6!=1) */)
							{cx--; end=0;}
						if (end==0)
						{
							*(s->BitMap.Planes[0]+(s->Width/8)*cy+oldx/8)&=~(128>>(oldx%8));
							cy++;
						}
						*(s->BitMap.Planes[0]+(s->Width/8)*cy+cx/8) |= (128>>(cx%8));
						if (end==1) cy=-1;
					}
				}
				x[i]=cx;
				y[i]=cy;
			}
		}
	}
	FreeMem(x,800L);
	FreeMem(y,800L);
}

int DisplayUsage()
{
	puts("Snowfall - made December 1989 by Lars R. Clausen");
	puts("[43mUsage:[0m SnowFall [-ppicture] [-n#snowflakes] [-trefreshtime] [-wwindforce]");
	exit(0);
}

int main(argc,argv)
int argc;
char **argv;
{
	char file[100];
	int number=40,refresh=10000,i,wind=0;
	
	if ((argc==2)&&(strcmp(argv[1],"?")==0)) DisplayUsage();
	
	strcpy(file,"SnowPic");
	for (i=1;i<argc;i++)
	{
		if (argv[i][0]=='-')
			switch (argv[i][1])
			{
				case 'p': strcpy(file,&argv[i][2]); break;
				case 'n': number=atoi(&argv[i][2]); break;
				case 't': refresh=atoi(&argv[i][2]); break;
				case 'w': wind=atoi(&argv[i][2]); break;
				default: DisplayUsage();
			}
		else DisplayUsage();
	}
	if (refresh>-1) OpenAll(file,1);
	else OpenAll(file,0);
	DoSnow((long)number,(long)refresh,(long)wind);
	loc_abort(0);
}
