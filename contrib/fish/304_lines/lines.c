/*
 * lines.c : Line pattern generator experimental interface
 *
 * this stuf adapted from mackie.c on fish disk 131.
 *  by Joel Swank 11-13-88
 *
 *
 *
 *
 */

/*
 *   The maximum number of lines on the screen at once.
 */

#include <aros/oldprograms.h>
#include <exec/types.h>
#include <intuition/intuition.h>

#define MAXLINES (100)

#define XSIZE 670
#define YSIZE 440
#define MAXHELP 20

#define INTUITION_REV 1L

/*
 *   The global variables.
 */
struct GfxBase	*GfxBase;
struct RastPort *rastport ;
int screenheight, screenwidth ;
struct Screen *scr;
struct Window	*Wind = NULL;
struct MsgPort	*CreatePort() ;
struct IntuitionBase	*IntuitionBase ;
struct IntuiMessage	*msg;

void eraseold();

/*
 * definition of the main window
 */

static struct NewWindow	New_Window = {
	0, 2,			/* Take all except the */
	XSIZE, YSIZE,	/* top two lines       */
	-1, -1,			/* Default pens */
	CLOSEWINDOW+VANILLAKEY,	/* Inputs acceppeted */
	WINDOWCLOSE		/* Fairly standard window */
	| BACKDROP | SIMPLE_REFRESH | BORDERLESS ,
	NULL,			/* no gadgets */
	(struct Image *) NULL,
	(UBYTE *) "h for Help",	/* title */
	(struct Screen *) NULL,		/* filled at startup */
	(struct BitMap *) NULL,
	0, 0, 0, 0,		/* no change sizes, doesn't matter */
	CUSTOMSCREEN
	} ;




static struct NewScreen newscreen = {
   0, 0,
   XSIZE, YSIZE+2,
   1,
   0, 1,
   HIRES | LACE | SCREENQUIET,
   CUSTOMSCREEN,
   NULL,
   NULL,
   NULL,
   NULL } ;

char *HelpText[40] = {
"    Lines - line pattern generator",
"       by Joel Swank 11/13/88",
" ",
"   Control from keyboard as follows:",
" ",
" space   Start/Stop generation",
" o       Do one generation",
" f       Freeze Thaw color rotation",
" h       Help Screen",
" x,q     Exit Program",
" Escape  Exit Program",
" ",
"    Return to continue",
NULL };

void done();
void clr_grf();
void startlines();
void advancelines();
void drawnew();
void colors();
void do_help();

/*
 *   MainLine
 */
void main() {
   long i ;
   USHORT keycode, stop, freeze;

   screenheight = YSIZE;
   screenwidth = XSIZE;
 
   if ((IntuitionBase = (struct IntuitionBase *)
       OpenLibrary("intuition.library", INTUITION_REV)) == NULL)
      done(21);

   GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", NULL);
   if (GfxBase == NULL) done(22);

   if ( (scr = OpenScreen(&newscreen)) == NULL)  done(26);
   New_Window.Screen = scr;

   if ((Wind = (struct Window *) OpenWindow(&New_Window)) == NULL)
      done(25);
   rastport = Wind->RPort;

   SetRGB4(&(scr->ViewPort), 0L, 0L, 0L, 0L) ;
   clr_grf();

   startlines() ;
   for (i=0; i<MAXLINES; i++) {
      advancelines() ;
      drawnew() ;
   }
   colors() ;

   stop = FALSE;
   freeze = FALSE;
   while (1) {

	while(NULL != (msg = (struct IntuiMessage *) GetMsg(Wind->UserPort))) {
		switch(msg->Class) {
			case VANILLAKEY:
				keycode = msg->Code;
				ReplyMsg((struct Message *)msg);
				switch (keycode)
					{
					case ' ':		/* start/stop */
					  if (stop == TRUE) stop = FALSE;
					  else stop = TRUE;
					  break;
					case 'h':		/* Help */
					  /* wait for this message */
					  if (!stop) Wait( 1L << Wind->UserPort->mp_SigBit);
					  do_help();
					     /* wait on next mesg */
					  Wait( 1L << Wind->UserPort->mp_SigBit);
					  clr_grf();
					  break;
					case 'o':		/* one iteration */
					  stop = TRUE;
					  goto once;
					case 'f':		/* freeze/thaw color changing */
					  if (freeze == TRUE) freeze = FALSE;
					  else freeze = TRUE;
					  break;
					case 'q':		/* Exit */
					case 'x':
					case '\033':
					  done(0);
					  break;
					}
				continue;
			case CLOSEWINDOW:
				ReplyMsg((struct Message *)msg);
				done(0);
			}
			ReplyMsg((struct Message *)msg);
		}
	  if (stop == TRUE) 
	  	{
		Wait( 1L << Wind->UserPort->mp_SigBit);	/* wait on mesg */
		continue;
		}

   once:
      eraseold() ;
      advancelines() ;
      drawnew() ;
      eraseold() ;
      advancelines() ;
      drawnew() ;
      eraseold() ;
      advancelines() ;
      drawnew() ;
      eraseold() ;
      advancelines() ;
      drawnew() ;
      eraseold() ;
      advancelines() ;
      drawnew() ;
      eraseold() ;
      advancelines() ;
      drawnew() ;
      if (!freeze) colors() ;
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
    if (Wind) CloseWindow(Wind) ;
    if (scr) CloseScreen(scr) ;
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase) ;
    if (GfxBase) CloseLibrary((struct Library *)GfxBase) ;
 
    //OpenWorkBench() ;		/* As requested */
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
short dr, dg, db ;
short or, og, ob ;
short nr, ng, nb ;
/*
 *   Initialize things for the first lines.
 */
void startlines() {
   ptr = 0 ;
   if (dx1 == 0) {
      ox1 = randm(screenwidth) ;
      ox2 = randm(screenwidth) ;
      oy1 = randm(screenheight) ;
      oy2 = randm(screenheight) ;
      dx1 = 3 ;
      dx2 = 4 ;
      dy1 = 1 ;
      dy2 = 6 ;
      nr = 53 ;
      ng = 33 ;
      nb = 35 ;
      dr = -3 ;
      dg = 5 ;
      db = 7 ;
   }
   SetRGB4(&(scr->ViewPort), 0L, 0L, 0L, 0L) ;
   SetRGB4(&(scr->ViewPort), 1L, (long)(nr >> 3),
                                 (long)(ng >> 3), (long)(nb >> 3)) ;
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
   or = nr ;
   og = ng ;
   ob = nb ;
   adv(&or, &dr, &nr, 128) ;
   adv(&og, &dg, &ng, 128) ;
   adv(&ob, &db, &nb, 128) ;
   SetRGB4(&(scr->ViewPort), 1L, (long)(nr >> 3),
                                    (long)(ng >> 3), (long)(nb >> 3)) ;
}

/*
 * do_help - display help text
 */

void do_help()
{
	int i;
	clr_grf();

	for (i=0; i<MAXHELP; i++)	/* dump the whole help text array */
		{
		if (!HelpText[i]) break;
		Move(rastport,50L,(long) (i+1)*9+20);
		Text(rastport,HelpText[i], (long) strlen(HelpText[i]));
		}

}


void clr_grf()
{
   SetAPen(rastport, 0L) ;
   Forbid() ;
   RectFill(rastport, 0L, 0L, (long)screenwidth-1, (long)screenheight-1) ;
   Permit() ;
   SetAPen(rastport, 1L) ;
}
