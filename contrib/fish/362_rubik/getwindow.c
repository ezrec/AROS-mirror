/************************************************************
*
* getwindow.c ......used in rubik.c
* 
*
* M.J.R.  19/12/89
*
*************************************************************/

#include "header.c"

struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct IntuiMessage *MyIntuiMessage = NULL;

extern struct Screen *scr1, *scr2;
extern struct Gadget divegadget;
extern struct TextAttr ta;

struct NewScreen ns =
	{
	0,  			/* left edge - should be zero */
	0,  			/* top edge */
	320,			/* width - low resolution */
	200,			/* height - non-interlace */
	3,  			/* depth - 8 colours will be available */
	0,7,			/* detail pen and block pen specifications */
	0,  			/* display mode */
	CUSTOMSCREEN,	/* the screen type */
	&ta,			/* font */
	NULL,			/* no default title */
	NULL,			/* no special screen gadgets */
	NULL,			/* no special custom bitmap */
	};

extern struct Window *wnd1, *wnd2;

struct NewWindow nw =
	{
	0,0,320,199,			/* position and size	*/
	0,7,					/* pens					*/
	CLOSEWINDOW |
	GADGETUP |
	MOUSEBUTTONS,			/* IDCMPFlags			*/
	WINDOWCLOSE |
	BORDERLESS |
	ACTIVATE |
	RMBTRAP,				/* flags				*/
	&divegadget,			/* gadgets				*/
	NULL,					/* no special checkmark */
	NULL,					/* no title!			*/
	NULL,					/* screen pointer		*/
	NULL,					/* no bitmap			*/
	0,0,					/* minimum sizes		*/
	320,199,				/* maximum sizes		*/
	CUSTOMSCREEN			/* type of screen		*/
	};

WORD areabuffer[25];		/*  room for up to a decagon	*/

struct TmpRas tmpras;
struct AreaInfo ai;

void MyCleanup()
	{  
	if (wnd2)
		CloseWindow(wnd2);
	if (wnd1)
		CloseWindow(wnd1);
	if (scr2)
		CloseScreen(scr2);
	if (scr1)
		CloseScreen(scr1);
	if (GfxBase != NULL)
		CloseLibrary((struct Library *) GfxBase);
	if (IntuitionBase != NULL)
		CloseLibrary((struct Library *) IntuitionBase);
	}

void cleanup()
	{
	FreeRaster(tmpras.RasPtr,320,199);
	MyCleanup();
	}

void getwindow()
	{
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
	if (IntuitionBase == NULL)
		{
		MyCleanup();
		exit(FALSE);
		}

	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
	if (GfxBase == NULL)
		{
		MyCleanup();
		exit(FALSE);
		}	

	scr1 = (struct Screen *)OpenScreen(&ns);
	if (scr1 == NULL)
		{
		MyCleanup();
		exit(FALSE);
		}

	scr2 = (struct Screen *)OpenScreen(&ns);
	if (scr1 == NULL)
		{
		MyCleanup();
		exit(FALSE);
		}

	SetRGB4(&(scr1->ViewPort),1,14,12,2);		/*	y	*/
	SetRGB4(&(scr1->ViewPort),2,13,13,13);		/*	w	*/
	SetRGB4(&(scr1->ViewPort),3,0,10,0);		/*	g	*/
	SetRGB4(&(scr1->ViewPort),4,2,4,11);		/*	bu	*/
	SetRGB4(&(scr1->ViewPort),5,15,5,0);		/*	o	*/
	SetRGB4(&(scr1->ViewPort),6,11,3,1);		/*	bn	*/
	SetRGB4(&(scr1->ViewPort),7,10,10,10);		/*	gy	*/
	SetRGB4(&(scr1->ViewPort),0,0,0,0);			/*	bk	*/

	SetRGB4(&(scr2->ViewPort),1,14,12,2);		/*	y	*/
	SetRGB4(&(scr2->ViewPort),2,13,13,13);		/*	w	*/
	SetRGB4(&(scr2->ViewPort),3,0,10,0);		/*	g	*/
	SetRGB4(&(scr2->ViewPort),4,2,4,11);		/*	bu	*/
	SetRGB4(&(scr2->ViewPort),5,15,5,0);		/*	o	*/
	SetRGB4(&(scr2->ViewPort),6,11,3,1);		/*	bn	*/
	SetRGB4(&(scr2->ViewPort),7,10,10,10);		/*	gy	*/
	SetRGB4(&(scr2->ViewPort),0,0,0,0);			/*	bk	*/

	nw.Screen = scr1; /* fix the pointer into the struct  */

	wnd1 = (struct Window *)OpenWindow(&nw);
	if (wnd1 == NULL)
		{
		MyCleanup();
		exit(FALSE);
		}

	nw.Screen = scr2; /* fix the pointer into the struct  */

	wnd2 = (struct Window *)OpenWindow(&nw);
	if (wnd2 == NULL)
		{
		MyCleanup();
		exit(FALSE);
		}

	InitArea(&ai,&(areabuffer[0]),10);
        
        InitTmpRas(&tmpras,AllocRaster(320,199),RASSIZE(320,199));
	
	wnd1->RPort->AreaInfo = &ai;

	wnd1->RPort->TmpRas = &tmpras; 
	wnd2->RPort->AreaInfo = &ai;
	wnd2->RPort->TmpRas = wnd1->RPort->TmpRas;
	
	SetAPen(wnd2->RPort,7);
	SetDrMd(wnd2->RPort,JAM1);
	Move(wnd2->RPort,197,31);
	Draw(wnd2->RPort,197,23);
	Draw(wnd2->RPort,237,23);
	Move(wnd2->RPort,276,31);
	Draw(wnd2->RPort,276,23);
	Draw(wnd2->RPort,308,23);
	Move(wnd2->RPort,199,90);
	Draw(wnd2->RPort,199,82);
	Draw(wnd2->RPort,248,82);
	Draw(wnd2->RPort,248,90);
	Move(wnd2->RPort,260,90);
	Draw(wnd2->RPort,260,82);
	Draw(wnd2->RPort,309,82);
	Draw(wnd2->RPort,309,90);
	Move(wnd2->RPort,270,115);
	Draw(wnd2->RPort,301,115);
	Move(wnd2->RPort,265,147);
	Draw(wnd2->RPort,265,139);
	Draw(wnd2->RPort,305,139);
	
	RectFill(wnd2->RPort,206,101,244,112);
	Move(wnd2->RPort,253,137);
	Draw(wnd2->RPort,253,149);
	Draw(wnd2->RPort,197,149);
	Draw(wnd2->RPort,197,137);
	Draw(wnd2->RPort,253,137);
	Draw(wnd2->RPort,253,125);
	Draw(wnd2->RPort,197,125);
	Draw(wnd2->RPort,197,137);
	Draw(wnd2->RPort,197,113);
	Draw(wnd2->RPort,253,113);
	Draw(wnd2->RPort,253,125);
	
	Move(wnd2->RPort,210,110);
	SetAPen(wnd2->RPort,0);
	Text(wnd2->RPort,"MODE",4);
	
	ClipBlit
		(
		wnd2->RPort,190,20,
		wnd1->RPort,190,20,
		128,177,
		0xc0
		);
	}

