/*
 * clines.c : Color line pattern generator experimental interface
 *
 * this stuf adapted from mackie.c on fish disk 131.
 *  by Joel Swank 11-13-88
 *
 * Version 1.1 5/26/89
 *
 *
 */

#include "window.h"

/*
 *   The maximum number of lines on the screen at once.
 */
#define MAXLINES (100)

#define INTUITION_REV 1L

/*
 *   The global variables.
 */
struct GfxBase	*GfxBase;
struct RastPort *rastport ;
int screenheight, screenwidth;
int erasecolor, curcolor ;
struct Screen *scr;
struct Window	*Wind = NULL;
struct MsgPort	*CreatePort() ;
struct IntuitionBase	*IntuitionBase ;
struct IntuiMessage	*msg;
struct Window *wH = NULL;        /* help window */
struct RastPort *rpH = NULL;     /* help RastPort */
struct TextFont *font = NULL;

USHORT keycode, stop, rotcolor, bkgnd, freeze, erase;
ULONG class;

void done(int how);
void clr_grf();
void do_help();
void startlines();
void drawnew();
void colors();
void advancelines();
void about();
void draw_help();
void eraseold();

/*
 *   MainLine
 */

void main() {
   long i ;

   screenheight = YSIZE;
   screenwidth = XSIZE;
 
   if ((IntuitionBase = (struct IntuitionBase *)
       OpenLibrary("intuition.library", INTUITION_REV)) == NULL)
      done(21);

   GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", NULL);
   if (GfxBase == NULL) done(22);

   if ( (scr = OpenScreen(&newscreen)) == NULL)  
   	{
	AutoRequest(0L,&scrfailtxt,0L,&oktxt,0L,0L,300L,75L);
	done(26);
	}
   New_Window.Screen = scr;

   if ((Wind = (struct Window *) OpenWindow(&New_Window)) == NULL)
      done(25);
   rastport = Wind->RPort;

   LoadRGB4(&(scr->ViewPort),(UWORD *)&Palette,PaletteColorCount);

   clr_grf();
   do_help();

   startlines() ;
   for (i=0; i<MAXLINES; i++) {
      advancelines() ;
      drawnew() ;
   }
   colors() ;
   stop = FALSE;
   freeze = FALSE;
   erase = TRUE;
   rotcolor = TRUE;

   while (1) {

	while((msg = (struct IntuiMessage *) GetMsg(Wind->UserPort)) ||
	     (msg = (struct IntuiMessage *) GetMsg(wH->UserPort))) {
		keycode = msg->Code;
		class = msg->Class;
		ReplyMsg((struct Message *)msg);
		switch(class) {
			case VANILLAKEY:
				switch (keycode)
					{
					case ' ':		/* start/stop */
					  if (stop == TRUE) stop = FALSE;
					  else stop = TRUE;
					  break;
					case 'o':		/* one iteration */
					  stop = TRUE;
					  goto once;
					case 'e':		/* toggle erase */
					  if (erase == TRUE) erase = FALSE;
					  else erase = TRUE;
					  break;
					case 'r':		/* toggle pallete changes */
					  if (rotcolor == TRUE)  rotcolor = FALSE;
					  else rotcolor = TRUE;
					  break;
					case 'b':		/* Black/white background */
					  if (bkgnd)
					  	{
						bkgnd = FALSE;
      					SetRGB4(&(scr->ViewPort), 0L, 0L, 0L, 0L);
					  } else {
      					SetRGB4(&(scr->ViewPort), 0L, 15L, 15L, 15L);
						bkgnd = TRUE;
						}
					  break;
					case 'f':		/* freeze/thaw color changing */
					  if (freeze == TRUE) freeze = FALSE;
					  else freeze = TRUE;
					  break;
					case 'a':		/* do about requester  */
					  about();
					  break;
					case 'q':		/* Exit */
					case 'x':
					case '\033':
					  done(0);
					  break;
					}
				continue;
			case REFRESHWINDOW :
				draw_help();
				break;
			case CLOSEWINDOW:
				done(0);
			}
		}
	  if (stop == TRUE) 
	  	{
		Wait(( 1L << Wind->UserPort->mp_SigBit) |	/* wait on mesg */
		    ( 1L << wH->UserPort->mp_SigBit));
		continue;
		}

   once:
      if (erase) eraseold() ;
      advancelines() ;
      drawnew() ;
      if (erase) eraseold() ;
      advancelines() ;
      drawnew() ;
      if (erase) eraseold() ;
      advancelines() ;
      drawnew() ;
      if (erase) eraseold() ;
      advancelines() ;
      drawnew() ;
      if (erase) eraseold() ;
      advancelines() ;
      drawnew() ;
      if (erase) eraseold() ;
      advancelines() ;
      drawnew() ;
      colors() ;
   }
}
/*
 * End of MainLine 
 */


/*
 * done - just clean up that which is open, and then leave.
 */
void done(how)
int how;
    {
    if (wH) CloseWindow(wH) ;
    if (Wind) CloseWindow(Wind) ;
    if (scr) CloseScreen(scr) ;
    if (font) CloseFont(font) ;
    if (GfxBase) CloseLibrary((struct Library *)GfxBase) ;
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase) ;
 
    exit(how) ;
    }

/*
 *   This routine returns a random value from 0 to n-1.
 */
int randm(i)
int i ;
{
   static long seed ;
   long rval ;

   if (seed == 0)
      seed = 323214521 + scr->MouseX +
              scr->MouseY ;
   seed = seed * 123213 + 121 ;
   rval = (seed >> 5) & 65535 ;
   return ((i * rval) >> 16) ;
}
/*
 *   This routine sets x and y values to a random number.
 */
static long x, y ;
void randomxy() {
   x = randm(screenwidth) ;
   y = randm(screenheight) ;
}
/*
 *   Main routines are always fun.
 */
short x1store[MAXLINES], y1store[MAXLINES] ;
short x2store[MAXLINES], y2store[MAXLINES] ;
short ptr ;
short dx1, dy1, dx2, dy2 ;
short ox1, oy1, ox2, oy2 ;
short nx1, ny1, nx2, ny2 ;
short dr[8], dg[8], db[8] ;
short or, og, ob ;
short nr[8], ng[8], nb[8] ;
/*
 *   Initialize things for the first lines.
 */
void startlines() {
   int i;

   ptr = 0 ;
   ox1 = randm(screenwidth) ;
   ox2 = randm(screenwidth) ;
   oy1 = randm(screenheight) ;
   oy2 = randm(screenheight) ;
   dx1 = 3 ;
   dx2 = 4 ;
   dy1 = 1 ;
   dy2 = 6 ;
   for (i=1;i < 8; i++)
      {
      nr[i] = (Palette[i] & 0x0F00) >> 5 ;
      ng[i] = (Palette[i] & 0x00F0) >> 1 ;
      nb[i] = (Palette[i] & 0x000F) << 3 ;
      dr[i] = -3 ;
      dg[i] = 5 ;
      db[i] = 7 ;
	  }
}
/*
 *   Advance the number by the delta, and check the boundaries.
 */
void adv(o, d, n, w)
short *o, *d, *n ;
long w ;
{
   *n = *o + *d ;
   if (*n < 0) {
      *n = 0 ;
      *d = randm(6) + 1 ;
   } else if (*n >= w) {
      *n = w - 1 ;
      *d = - randm(6) - 1 ;
   }
}
/*
 *   Advance the two points which make up the lines.
 */
void advancelines() {
   adv(&ox1, &dx1, &nx1, screenwidth) ;
   adv(&ox2, &dx2, &nx2, screenwidth) ;
   adv(&oy1, &dy1, &ny1, screenheight) ;
   adv(&oy2, &dy2, &ny2, screenheight) ;
}
/*
 *   Draw a new set of lines.
 */
void drawnew() {
   x1store[ptr] = ox1 = nx1 ;
   x2store[ptr] = ox2 = nx2 ;
   y1store[ptr] = oy1 = ny1 ;
   y2store[ptr] = oy2 = ny2 ;
   Move(rastport, (long)ox1, (long)oy1) ;
   Draw(rastport, (long)ox2, (long)oy2) ;
   Draw(rastport, (long)(screenwidth-ox1-1), (long)(screenheight-oy1-1)) ;
   Draw(rastport, (long)(screenwidth-ox2-1), (long)(screenheight-oy2-1)) ;
   Draw(rastport, (long)ox1, (long)oy1) ;
   ptr++ ;
   if (ptr == MAXLINES)
      ptr = 0 ;
}
/*
 *   Erase the old line.
 */
void eraseold() {
   short oldpen ;

   oldpen = rastport->FgPen ;
   SetAPen(rastport, 0L) ;
   Move(rastport, (long)x1store[ptr], (long)y1store[ptr]) ;
   Draw(rastport, (long)x2store[ptr], (long)y2store[ptr]) ;
   Draw(rastport, (long)(screenwidth-x1store[ptr]-1),
                  (long)(screenheight-y1store[ptr]-1)) ;
   Draw(rastport, (long)(screenwidth-x2store[ptr]-1), 
                  (long)(screenheight-y2store[ptr]-1)) ;
   Draw(rastport, (long)x1store[ptr], (long)y1store[ptr]) ;
   SetAPen(rastport, (long)oldpen) ;
}
/*
 *   This routine mucks with the colors.
 */
void colors() {

   if (!freeze)
      {
      if (++curcolor > 7) curcolor = 1 ;
      SetAPen(rastport, (long)curcolor) ;
	  }

   if (rotcolor && curcolor != 0)
      {
      or = nr[curcolor] ;
      og = ng[curcolor] ;
      ob = nb[curcolor] ;
      adv(&or, &dr[curcolor], &nr[curcolor], 128) ;
      adv(&og, &dg[curcolor], &ng[curcolor], 128) ;
      adv(&ob, &db[curcolor], &nb[curcolor], 128) ;
      SetRGB4(&(scr->ViewPort), (long)curcolor, (long)(nr[curcolor] >> 3),
                (long)(ng[curcolor] >> 3), (long)(nb[curcolor] >> 3)) ;
	  }

}




/*
 * do_help - display help text
 */

void do_help()
{

    NewWindowStructureHelp.Screen = scr;

	wH = OpenWindow(&NewWindowStructureHelp);	/* open the window */
	if ( wH == NULL )
		{
		AutoRequest(Wind,&winfailtxt,0L,&oktxt,0L,0L,300L,75L);
		done(20);
		}

	rpH = wH->RPort;	/* get a rastport pointer for the window */

	font = OpenFont(&TOPAZ80); /* make sure correct size font */
	if (font) SetFont(rpH,font);
	SetAPen(rpH,1L);
	draw_help();
}

void draw_help()
{
	int i;
	for (i=0; i<MAXHELP; i++)	/* dump the whole help text array */
		{
		if (!HelpText[i]) break;
		Move(rpH,5L,(long) (i+1)*9+20);
		Text(rpH,HelpText[i], (long) strlen(HelpText[i]));
		}
}



/*
 *  Handle 'about' request
 */


void about()
{
	AutoRequest(wH,&aboutmsg,0L,&oktxt,0L,0L,300L,75L);
}


void clr_grf(void)
{
   SetAPen(rastport, 0L) ;
   Forbid() ;
   RectFill(rastport, 0L, 0L, (long)screenwidth-1, (long)screenheight-1) ;
   Permit() ;
   SetAPen(rastport, 1L) ;
}

#ifdef AZTEC_C
_wb_parse()
{
}

_abort()
{
	done(13);
}
#endif
