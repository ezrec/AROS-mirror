/*********************************************************************
----------------------------------------------------------------------

	MysticView
	View Class Definitions

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_VIEW_H
#define	MYSTIC_VIEW_H 1

#include <exec/semaphores.h>
#include <guigfx/guigfx.h>
#include <graphics/gfx.h>
#include <libraries/mysticview.h>

struct pip
{
	BOOL active;
	struct Region *region;
	int pdx, pdy, pdw, pdh, psx, psy, psw, psh;
	struct BitMap *quickbm;
};



struct picview
{
	struct SignalSemaphore semaphore;

	struct Screen *screen;
	struct ColorMap *colormap;
	struct RastPort *rastport;

	struct	picdrawer *picdrawer;		// draw task


	int		lockcount;					// number of external locks


	struct	SignalSemaphore *rastlock;

	//	view settings

	ULONG	backcolor, textcolor, markcolor;
	long	backpen;
	long	complementpen;
	long	markpen;

	ULONG	precision;
	int		viewaspectx, viewaspecty;
	int 	viewwidth, viewheight;
	int		viewx, viewy;
	BOOL	truecolor;
	BOOL	ham;
	BOOL	drawarrows;
	BOOL	drawpip;
	
	int dx, dy, dw, dh, cx, cy;


	//	draw data

	ULONG	izoom;
	float	zoom;

	ULONG	ixpos, iypos;
	float	xpos, ypos;

	int		displaymode;
	int		previewmode;

	APTR	psm;
	APTR	drawhandle;
	
	ULONG	picwidth;
	ULONG	picheight;
	ULONG	picaspectx;
	ULONG	picaspecty;
	
	float	viewaspect;		//	view aspect
	float	picaspect;		//	picture aspect
	
	
	//	pictures

	PICTURE *showpic;					// external
	PICTURE *quickpic;					// internal


	//	misc
	
	char *text;
	int textlen;
	char readysignal;					// signal to parent task
	
	int dither;
	ULONG dithermode;
	int autodither;
	BOOL autoditherchecked;
	
	BOOL staticpalette;
	ULONG hstype;
	APTR visualinfo;
	BOOL markcenter;


	//	viewrelative

	int	startx, starty;
	float startxpos, startypos;



	//	rotating

	int angle;
	WORD coordinates[8];


	//	rastport fuck

	UBYTE *rasbuffer;
	struct TmpRas tmpras;
	struct AreaInfo areainfo;
	char areabuffer[3*5*4];
	
	struct AreaInfo *oldareainfo;
	struct TmpRas *oldtmpras;

	//

	struct pip pip;
};

#endif

