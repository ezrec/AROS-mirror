/*
 * window.h : Intuition structures needed for the screen, windows,
 *             and requesters. Parts created with PowerWindows 2.0.
 *             by Joel Swank 5/26/89
 */

#include <aros/oldprograms.h>
#include <intuition/intuition.h>

#define XSIZE 670
#define YSIZE 440
#define MAXHELP 20

/*
 * definition of the main window
 */

static struct NewWindow	New_Window = {
	0, 2,			/* Take all except the */
	XSIZE, YSIZE,	/* top two lines       */
	-1, -1,			/* Default pens */
	VANILLAKEY,	/* Inputs acceppeted */
	SMART_REFRESH /* window flags */
	| ACTIVATE | BORDERLESS ,
	NULL,			/* no gadgets */
	(struct Image *) NULL,
	(UBYTE *) NULL,
	(struct Screen *) NULL,		/* filled at startup */
	(struct BitMap *) NULL,
	0, 0, 0, 0,		/* no change sizes, doesn't matter */
	CUSTOMSCREEN
	} ;



/*
 * definition of my custom screen
 */


static struct NewScreen newscreen = {
   0, 0,
   XSIZE, YSIZE+2,
   3,
   0, 1,
   HIRES | LACE | SCREENQUIET,
   CUSTOMSCREEN,
   NULL,
   NULL,
   NULL,
   NULL } ;

static USHORT Palette[] = {
	0x0000, /* color #0 */
	0x003F, /* color #1 */
	0x000F, /* color #2 */
	0x0DEF, /* color #3 */
	0x00F0, /* color #4 */
	0x0F00, /* color #5 */
	0x0CF2, /* color #6 */
	0x0FFF	/* color #7 */
#define PaletteColorCount 8
};


/*
 * definition of the help text
 */

char *HelpText[40] = {
" Clines - Color line pattern generator",
" ",
"   Control from keyboard as follows:",
" ",
" space   Start/Stop generation",
" o       Do one generation",
" f       start/stop alternating color",
" e       start/stop erasing",
" r       start/stop changing palette",
" b       Toggle white/black background",
" a       About this program",
" x,q     Exit Program",
" Escape  Exit Program",
NULL };


struct TextAttr TOPAZ80 = {
    (STRPTR)"topaz.font",
    TOPAZ_EIGHTY,0,0
};


/*
 * definition of the help window
 */

struct NewWindow NewWindowStructureHelp = {
    0,0,    /* window XY origin relative to TopLeft of screen */
    344,161,    /* window width and height */
    0,1,    /* detail and block pens */
    REFRESHWINDOW | CLOSEWINDOW | VANILLAKEY,	/* Inputs acceppeted */
    WINDOWDRAG | WINDOWDEPTH | WINDOWCLOSE
	| SIMPLE_REFRESH ,    /* other window flags */
    NULL,    /* first gadget in gadget list */
    NULL,    /* custom CHECKMARK imagery */
    (UBYTE *)"      Clines Help Window      ",    /* window title */
    NULL,    /* custom screen pointer */
    NULL,    /* custom bitmap */
    0,0,    /* minimum width and height */
    640,200,    /* maximum width and height */
    CUSTOMSCREEN    /* destination screen type */
};

/*
 *    Data describing the requester text
 *
 */

struct IntuiText oktxt = {
	3,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	5,3,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *)"OK",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct IntuiText opfailtxt = {
	0,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	14,16,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *)"Open Failed",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct IntuiText scrfailtxt = {
	0,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	14,16,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *)"Screen failed to open",	/* pointer to text */
	NULL	/* next IntuiText structure */
};


struct IntuiText winfailtxt = {
	0,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	14,16,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *)"Window failed to open",	/* pointer to text */
	NULL	/* next IntuiText structure */
};


struct IntuiText cantxt = {
	3,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	5,3,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *)"CANCEL",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct IntuiText retrytxt = {
	3,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	5,3,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *)"RETRY",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct IntuiText aboutmsg4 = {
	0,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	14,40,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *) "         May 26, 1989",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct IntuiText aboutmsg3 = {
	3,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	14,30,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *)  "        by Joel Swank",	/* pointer to text */
	&aboutmsg4 /* next IntuiText structure */
};

struct IntuiText aboutmsg2 = {
	0,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	14,20,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *) "         Version 1.1",
	&aboutmsg3	/* next IntuiText structure */
};

struct IntuiText aboutmsg = {
	0,1,JAM2,	/* front and back text pens, drawmode and fill byte */
	14,10,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	(UBYTE *)"Color Line Pattern Generater",	/* pointer to text */
	&aboutmsg2	/* next IntuiText structure */
};

