/*
 *
 *	DISCLAIMER:
 *
 *	This program is provided as a service to the programmer
 *	community to demonstrate one or more features of the Amiga
 *	personal computer.  These code samples may be freely used
 *	for commercial or noncommercial purposes.
 * 
 * 	Commodore Electronics, Ltd ("Commodore") makes no
 *	warranties, either expressed or implied, with respect
 *	to the program described herein, its quality, performance,
 *	merchantability, or fitness for any particular purpose.
 *	This program is provided "as is" and the entire risk
 *	as to its quality and performance is with the user.
 *	Should the program prove defective following its
 *	purchase, the user (and not the creator of the program,
 *	Commodore, their distributors or their retailers)
 *	assumes the entire cost of all necessary damages.  In 
 *	no event will Commodore be liable for direct, indirect,
 *	incidental or consequential damages resulting from any
 *	defect in the program even if it has been advised of the 
 *	possibility of such damages.  Some laws do not allow
 *	the exclusion or limitation of implied warranties or
 *	liabilities for incidental or consequential damages,
 *	so the above limitation or exclusion may not apply.
 *
 */

/* layers.c */

/* **********************************************************************
 * THIS EXAMPLE SHOWS HOW TO USE THE layers.library.  Certain functions
 * are not available in the system software prior to the release of
 * version 1.1.  Therefore this example can only be compiled if your
 * C-disk supports version 1.1 or beyond.
 ********************************************************************* */

/* author:  Rob Peck, 12/1/85 */

/* This code may be freely utilized to develop programs for the Amiga. */

#include "aros/oldprograms.h"
#include "exec/types.h"
#include "graphics/gfx.h"
//#include "hardware/dmabits.h"
#include "hardware/custom.h"
#include "hardware/blit.h"
#include "graphics/gfxmacros.h"
#include "graphics/copper.h"
#include "graphics/view.h"
#include "graphics/gels.h"
#include "graphics/regions.h"
#include "graphics/clip.h"
#include "exec/exec.h"
#include "graphics/text.h"
#include "graphics/gfxbase.h"
/* ********************** added for layers support ************************ */
#include "graphics/layers.h"
#include "graphics/clip.h"
/* ********************** added for layers support ************************ */


#define DEPTH 2  
#define WIDTH 320 
#define HEIGHT 200 
#define NOT_ENOUGH_MEMORY -1000
#define FOREVER for(;;) 
        /* construct a simple display */ 

#define FLAGS LAYERSMART
struct View *oldview;	/* save pointer to old view so can go back to sys */ 
struct View v;
struct ViewPort vp;
struct ColorMap *cm;	/* pointer to colormap structure, dynamic alloc */
struct RasInfo ri;
struct BitMap b;
/* made 3 separate rastports for layers testing ********************** */
struct RastPort *rp[3];		/* rastport for each layer */
/* dynamically created RastPorts from the calls to CreateUpfrontLayer */

short i,j,k,n;
struct GfxBase *GfxBase;

SHORT  boxoffsets[] = { 802, 2010, 3218 };
USHORT colortable[] = { 0x000, 0xf00, 0x0f0, 0x00f };
			/* black, red, green, blue */
UBYTE *displaymem;
UBYTE *colorpalette;

struct LayersBase *LayersBase;
struct Layer_Info *li;
struct Layer *layer[3];
 
void FreeMemory();

void main()
{
	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
	if (GfxBase == NULL) exit(1);

	LayersBase = (struct LayersBase *)OpenLibrary("layers.library",0); 
	if(LayersBase == NULL) exit(2);

	oldview = GfxBase->ActiView;	/* save current view, go back later */
        /* example steals screen from Intuition if started from WBench */
	
	li = NewLayerInfo();	/* get a LayerInfo structure */
	if(li == NULL) exit(100);

	/* not needed if gotten by NewLayerInfo 	InitLayers(li);   */

                        	/* initialize view */
        InitView(&v);
				/* link view into viewport */
        v.ViewPort = &vp;
				/* init view port */
        InitVPort(&vp);
				/* now specify critical characteristics */
	vp.DWidth = WIDTH;
	vp.DHeight = HEIGHT;
	vp.RasInfo = &ri;
				/* init bit map (for rasinfo and rastport) */
        InitBitMap(&b,DEPTH,WIDTH,HEIGHT);
				/* (init RasInfo) */
        ri.BitMap = &b;
        ri.RxOffset = 0;	/* align upper left corners of display
				 * with upper left corner of drawing area */
        ri.RyOffset = 0;
	ri.Next = NULL;
				/* (init color table) */
	cm = GetColorMap(4);	/* 4 entries, since only 2 planes deep */
	colorpalette = (UBYTE *)cm->ColorTable;
	for(i=0; i<4; i++)
		*colorpalette++ = colortable[i];
				/* copy my colors into this data structure */
	vp.ColorMap = cm;	/* link it with the viewport */

       		                 /* allocate space for bitmap */
        for(i=0; i<DEPTH; i++)
           {
           b.Planes[i] = (PLANEPTR)AllocRaster(WIDTH,HEIGHT);
           if(b.Planes[i] == NULL) exit(NOT_ENOUGH_MEMORY);
           }

	MakeVPort( &v, &vp );	/* construct copper instr (prelim) list */
	MrgCop( &v );		/* merge prelim lists together into a real 
				 * copper list in the view structure. */

	for(i=0; i<2; i++)
		{
		displaymem = (UBYTE *)b.Planes[i];
		for(j=0; j<RASSIZE(WIDTH,HEIGHT); j++)
			*displaymem++ = 0;	
		/* zeros to all bytes of the display area */					}

	LoadView(&v);

	/* now fill some boxes so that user can see something */

	layer[0] = CreateUpfrontLayer(li,&b,5,5,85,65,FLAGS,NULL);
		/* layerinfo, common bitmap, x,y,x2,y2,
		 * flags = 0 (simple refresh), null pointer to superbitmap */
	if(layer[0] == NULL) goto cleanup1;
	
	layer[1] = CreateUpfrontLayer(li,&b,20,20,100,80,FLAGS,NULL);
	if(layer[1] == NULL) goto cleanup2;

	layer[2] = CreateUpfrontLayer(li,&b,45,45,125,105,FLAGS,NULL);
	if(layer[2] == NULL) goto cleanup3;

	for(i=0; i<3; i++)	/* layers are created, now draw to them */
	{ 
	rp[i] = layer[i]->rp;
	SetAPen(rp[i],i+1);
	SetDrMd(rp[i],JAM1);
	RectFill(rp[i],0,0,79,59);
	}
	SetAPen(rp[0],0);
	Move(rp[0],5,30);
	Text(rp[0],"Layer 0",7);

	SetAPen(rp[1],0);
	Move(rp[1],5,30);
	Text(rp[1],"Layer 1",7);

	SetAPen(rp[2],0);
	Move(rp[2],5,30);
	Text(rp[2],"Layer 2",7);
	
	Delay(120);	/* 2 seconds before first change */
	BehindLayer((LONG)li,layer[2]);

	Delay(120);	/* another change 2 seconds later */

	UpfrontLayer((LONG)li,layer[0]);

	for(i=0; i<30; i++)
	{
		MoveLayer((LONG)li,layer[1],1,3);
		Delay(10);	/* wait .16 seconds (uses DOS function) */
		
	}

    cleanup3:
	LoadView(oldview);              /* put back the old view  */
	DeleteLayer((LONG)li,layer[2]);
    cleanup2:
	DeleteLayer((LONG)li,layer[1]);
    cleanup1:
	DeleteLayer((LONG)li,layer[0]);

	DisposeLayerInfo(li);
	FreeMemory();	
	CloseLibrary((struct Library *)GfxBase);

}	/* end of main() */


void FreeMemory()
{      		/* return user and system-allocated memory to sys manager */

  	for(i=0; i<DEPTH; i++)			/* free the drawing area */
           FreeRaster(b.Planes[i],WIDTH,HEIGHT);
	FreeColorMap(cm);			/* free the color map */
		/* free dynamically created structures */
	FreeVPortCopLists(&vp);		
 	FreeCprList(v.LOFCprList);   
}	
