/****************************************************************
*								*
*	rubik.c - An Animated Rubik's Cube for the Amiga        *
*		by Bill Kinnersley				*
*		Physics Department				*
*		Montana State University			*
*		Bozeman, MT 59717				*
*								*
*	This program compiles under Aztec C version 3.20a	*
*	using either the 16-bit or 32-bit integer options.	*
*	It is placed in the public domain.			*
*								*
*	It was largely inspired by and adapted from two		*
*	other public domain programs:				*
*		amiga3d by Barry A. Whitebook			*
*		skewb by Raymond S. Brand 			*
*								*
*	Rubik's Cube is a trademark of the Ideal Toy Corp.	*
*								*
****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <exec/memory.h>
#include "rubik.h"

struct TmpRas tmpras;
struct BitMap bitmap[5];
struct RastPort r[5], *rp[5];
struct RasInfo ri[2], *rip[2], *oldrip;

/* Rasters 0 and 1 are used alternately for the screen display.
   Raster 2 holds slices in the background which do not currently
   have to move, and 3 holds foreground slices.  Raster 4 holds
   a black-and-white version of Raster 3 to be used for masking. */

UWORD frametoggle=0, framecount=0, axis=0, slice=0;
UWORD newaxis=YES, newcube=YES;

long palette[8] = {
	0x0456, 0x0000, 0x0fff, 0x0fd0, 0x0f50, 0x0d00, 0x0060, 0x000d};

extern struct Tile ff[3][3][3], fb[3][3][3];
extern struct Objectinfo SliceInfo[3][3];

BOOL allocinfo(struct Objectinfo *objectinfo);
void deallocinfo(struct Objectinfo *objectinfo);
void matrixinit(struct Matrix *um);
void doframe(struct Screen *screen);
void doobject(struct RastPort *rap, struct Objectinfo *objectinfo, int doit);
void rubik(struct Screen *screen, struct Window *window); 
int  dographics(void);
void cubeinit(void);

struct GfxBase * GfxBase;
struct IntuitionBase * IntuitionBase;
/*
 extern void *AllocMem(), *AllocRaster(), *GetMsg();
*/
int main(int argc, char **argv) {
	WORD i, j, error = FALSE;
	cubeinit();		/* Build the cube and all 9 slices */
	if ((GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0))==NULL) exit(-1);

	/* Allocate all rasters */
	if ((tmpras.RasPtr = (BYTE *) AllocRaster(TMPWIDTH, TMPHEIGHT))
		==NULL) error = YES;	
	else tmpras.Size = RASSIZE(TMPWIDTH, TMPHEIGHT);
	for (j=0; j<5; j++) {
		InitBitMap(&bitmap[j], N_BIT_PLANES, WIDTH, MAXHINOLACE);
		for (i=0; i< N_BIT_PLANES; i++) if (!error)
			if ((bitmap[j].Planes[i] = (PLANEPTR) AllocMem(
			RASSIZE(WIDTH, MAXHINOLACE), CLEAR))==NULL) {
				error = YES;
	  			for (--i; i>=0; --i) FreeRaster(bitmap[j].
					Planes[i],WIDTH, MAXHINOLACE);
			}
	}

	if (!error) dographics();	/* Main routine */

	/* Deallocate rasters and quit */
	FreeRaster(tmpras.RasPtr, TMPWIDTH, TMPHEIGHT);
	for (j=0; j<5; j++) for (i=0; i< N_BIT_PLANES; i++)
		FreeRaster(bitmap[j].Planes[i], WIDTH, MAXHINOLACE);

	return 0;
}

#define MAXPOINTS 64L

struct AreaInfo areainfo;
UWORD areafill[(MAXPOINTS/2)*5];

struct NewScreen ns = {LEFT, 0, WIDTH, MAXHINOLACE, N_BIT_PLANES,
	-1, -1, 0, CUSTOMSCREEN, NULL, " ", NULL, NULL};

struct NewWindow nw = {0, TOP, WIDTH, HEIGHT, -1, -1, CLOSEWINDOW,
	WINDOWCLOSE | SIMPLE_REFRESH | ACTIVATE, 
	NULL, NULL, " ", NULL, NULL, 0, 0, 0, 0, CUSTOMSCREEN};

int dographics()
{
	struct Screen *screen;
	struct Window *window;

	if ((IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0L))==NULL)
		return (-1); 
	if ((screen = (struct Screen *)OpenScreen(&ns))==NULL) return(-1);

	/* Center the screen by hand, since Intuition won't. */
	screen->ViewPort.DxOffset = LEFT;
	MakeScreen(screen);
	RethinkDisplay();

	nw.Screen = screen;
	if ((window = (struct Window *)OpenWindow(&nw))==NULL) return (-1);
	InitArea(&areainfo, areafill, MAXPOINTS);
	window->RPort->AreaInfo = &areainfo;
	window->RPort->TmpRas = &tmpras;

	rubik(window->WScreen, window);		/* Main routine */

	CloseWindow(window);
	CloseScreen(screen);

	return 0;
}

void rubik(screen, window)
struct Screen *screen;
struct Window *window;
{
	WORD i, j, error;
	struct IntuiMessage *message;

	/* Set colors for this screen */
	for (i=0; i < (1<<N_BIT_PLANES); i++)
		SetRGB4(&screen->ViewPort, (long)i, palette[i]>>8 & 0xfL,
			palette[i]>>4 & 0xfL, palette[i] & 0xfL);
	oldrip = screen->ViewPort.RasInfo;	/* Save ptr to old info */
	for (j=0; j<5; j++) {
		r[j] = *(window->RPort);   		/* Struct copy */
		r[j].TmpRas = &tmpras;
		r[j].BitMap = &bitmap[j];
		r[j].Layer = NULL;
		rp[j] = &r[j]; 
	}
	for (j=0; j<2; j++) {
		ri[j] = *(screen->ViewPort.RasInfo);	/* Struct copy */
		ri[j].BitMap = &bitmap[j];
		rip[j] = &ri[j];
	}
	// SetRast(rp[frametoggle],0L);	/* Clear raster we're displaying */
	for (j=0; j<4; j++) SetOPen(rp[j],1L);
	/* Pixels in rp[4] will be  either "all bits on" or "all bits off" */
	SetOPen(rp[4],0L); SetAPen(rp[4],7L);

	/* Allocate info structures */
	for (i=0; i<3; i++) for (j=0; j<3; j++) {
		error = allocinfo(&SliceInfo[i][j]);
		if (error) puts("Allocation error");
	}
	while (TRUE) {		/* Main Display Loop */
		if (newaxis) {	/* Have to build the back and front slices */
			SetRast(rp[2],0L); SetRast(rp[3],0L);
		/* The 9 slices are represented by SliceInfo[slice][axis]
	   	where slice=0 is in back and slice=2 is in the front */
		/* First do any background slices */
			if (slice>0)
				doobject(rp[2], &SliceInfo[0][axis], FALSE);
				/* FALSE means don't rotate */
			if (slice>1)
				doobject(rp[2], &SliceInfo[1][axis], FALSE);
		/* Do any foreground slices */
			if (slice<1)
				doobject(rp[3], &SliceInfo[1][axis], FALSE);
			if (slice<2) {
				doobject(rp[3], &SliceInfo[2][axis], FALSE);
		/* Copy foreground to rp[4] and flood with color 7
		   (all bits on) to produce a mask. */
				ClipBlit(rp[3], 0L, 0L, rp[4], 0L, 0L,
					WIDTH, 200L, 0xc0L);
				Flood(rp[4],0L,128L,100L);
			}
		}

		do {		/* Check for user termination */
		message = (struct IntuiMessage *) GetMsg(window->UserPort);
			if (message) {
				if (message->Class==CLOSEWINDOW) goto finis;
				ReplyMsg((struct Message *)message);
			}
		} while (message);

		doframe(screen);	/* Advance the position */

		/* Here goes the screen flip */
			/* Take a deep breath and hold it */
		Forbid(); WaitTOF(); WaitBlit(); Disable();
		screen->ViewPort.RasInfo = rip[frametoggle];
		ScrollVPort(&screen->ViewPort);
			/* OK, you may breathe again */
		Enable(); Permit();
	}
finis:	for (i=0; i<3; i++) for (j=0; j<3; j++)
		deallocinfo(&SliceInfo[i][j]);	/* Deallocate structures */
	screen->ViewPort.RasInfo = oldrip;	/* Restore rasinfo pointer */
}

void doframe(screen)
struct Screen *screen;
{
	WORD i, j, pcount, a1, a2, s1, temp;

	newaxis = FALSE;
	if (++framecount >= MAXFRAMES) {	/* Start a new slice */
		framecount = 0;
		newaxis = TRUE;
		matrixinit(SliceInfo[slice][axis].matrix);
		a1 = (axis + 1)%3; a2 = (axis + 2)%3;
		s1 = (axis==1) ? slice : 2-slice;
		/* Rotate side face colors */
		for (i=0; i<3; i++) {
			temp = ff[axis][i][s1].color;
			ff[axis][i][s1].color = ff[a1][s1][2-i].color;
			ff[a1][s1][2-i].color = fb[axis][s1][2-i].color;
			fb[axis][s1][2-i].color = fb[a1][i][s1].color;
			fb[a1][i][s1].color = temp;
		}
		/* Rotate front face colors */
		if (s1==0) for (i=0; i<2; i++) {
			temp = fb[a2][i][0].color;
			fb[a2][i][0].color = fb[a2][0][2-i].color;
			fb[a2][0][2-i].color = fb[a2][2-i][2].color;
			fb[a2][2-i][2].color = fb[a2][2][i].color;
			fb[a2][2][i].color = temp;
		}
		else if (s1==2) for (i=0; i<2; i++) {
			temp = ff[a2][0][i].color;
			ff[a2][0][i].color = ff[a2][2-i][0].color;
			ff[a2][2-i][0].color = ff[a2][2][2-i].color;
			ff[a2][2][2-i].color = ff[a2][i][2].color;
			ff[a2][i][2].color = temp;
		}
		/* Repaint all slices */
		for (i=0; i<3; i++) for (j=0; j<3; j++) {
			struct Objectinfo *objectinfo;
			WORD *nextcolor;
			objectinfo = &SliceInfo[i][j];
			nextcolor = objectinfo->colorbuf;
			for (pcount=0; pcount < objectinfo->numtiles;
			pcount++) {
				struct Tile *np;
				np = (objectinfo->tiles)[pcount];
				*nextcolor++ = np->color;
			}
			*nextcolor = 0;
		}
		if (++axis>2) {
			axis = 0;
			if (++slice>2) slice = 0;
		}
		if ((axis==0) && (slice==0)) newcube = FALSE;
		return;
	}
	/* Copy the background slices to the raster not being displayed */
	ClipBlit(rp[2], 0L, 0L, rp[frametoggle^1], 0L,0L,WIDTH,200L,0xc0L);
	/* Add the rotating slice */
	doobject(rp[frametoggle^1], &SliceInfo[slice][axis], YES);
	if (slice<2) {
		ClipBlit(rp[4],44L,24L,rp[frametoggle^1],44L,24L,168L,150L,
			0x20L);		/* Subtract the masked area */
		ClipBlit(rp[3],44L,24L,rp[frametoggle^1],44L,24L,168L,150L,
			0xe0L);		/* Add the foreground slices */
	}
	frametoggle ^= 1;	/* Next time, we'll use the other screen */
}

void deallocinfo(objectinfo)
struct Objectinfo *objectinfo;
{
	if ((objectinfo->bufpoints) && (objectinfo->bufpointsize))
		FreeMem(objectinfo->bufpoints,
		(long)objectinfo->bufpointsize);
	if ((objectinfo->pptrbuf) && (objectinfo->pptrbufsize))
		FreeMem(objectinfo->pptrbuf,
		(long)objectinfo->pptrbufsize);
	if ((objectinfo->colorbuf) && (objectinfo->colorbufsize))
		FreeMem(objectinfo->colorbuf,
		(long)objectinfo->colorbufsize);
}

BOOL allocinfo(objectinfo)
struct Objectinfo *objectinfo;
{
	WORD *nextcolor;
	long tcount, vcount;
	struct Vector **nextp;

	/* Bufpoints contains space for a list of Vectors arranged as
	   an array: V[point][frame] */
	if (objectinfo->numpoints==0) {
		objectinfo->bufpointsize = 0;
		return(FALSE);
	}
	if ((objectinfo->bufpoints = (struct Vector *)AllocMem(
	    objectinfo->numpoints * (long)sizeof(struct Vector) * MAXFRAMES, 
	    CLEAR))==NULL) {
		objectinfo->bufpointsize = 0;
		return(TRUE);
	}
	objectinfo->bufpointsize = objectinfo->numpoints
		 * sizeof(struct Vector) * MAXFRAMES;

	/* Traverse the Tile list and initialize buffers for color 
	   and offset lists.  Pptrbuf contains pointers into the
	   list of points. */
	vcount = 4 * objectinfo->numtiles;
	if ((objectinfo->pptrbuf = (struct Vector **)
	AllocMem((long)sizeof(APTR) * (vcount + 1), CLEAR))==NULL) {
		objectinfo->pptrbufsize = 0;
		return(TRUE);
	}
	else objectinfo->pptrbufsize = sizeof(APTR) * (vcount + 1);
	if ((objectinfo->colorbuf = (WORD *) AllocMem((long)sizeof(WORD)*
	(objectinfo->numtiles + 1), CLEAR))==NULL) {
		objectinfo->colorbufsize = 0;
		return(TRUE);
	}
	else objectinfo->colorbufsize = sizeof(WORD) *
		(objectinfo->numtiles + 1);

	/* Initialize buffer pointers */
	nextcolor = objectinfo->colorbuf;
	nextp = objectinfo->pptrbuf;
	for (tcount=0; tcount < objectinfo->numtiles; tcount++) {
		struct Tile *tp;
		WORD vc;
		struct Vector **v;

		tp = (objectinfo->tiles)[tcount];
		*nextcolor++ = tp->color;
		v = tp->vertexstart;
		for (vc=0; vc<4; vc++) {
			long poff;

	/* Search objectinfo->points for a pointer which matches */
			for (poff=0; poff < objectinfo->numpoints; poff++)
				if (v[vc]==(objectinfo->points)[poff]) break;
			*nextp = (struct Vector *)(objectinfo->bufpoints
				+ poff*MAXFRAMES);
			nextp++;
		}
	}
	/* Terminate pointer buffer arrays with a null pointer */
	*nextcolor = 0;	
	*nextp = 0;
	return(FALSE);
}
