/**********************************************************************/
/*                                                                    */
/*                              ARoach.c                              */
/*                                                                    */
/*     © Copyright Stefan Winterstein 1993. Freely Distributable.     */
/*                                                                    */
/**********************************************************************/


#include "ARoach.h"

/* #define DEBUG		*//* general debugging output */
/* #define ROACHNUMBERING	*//* each roach has a number */
/* #define USE_WB_WINDOW	*//* use WB backdrop window if available (Hack!) */
/* #define CHECK_COLLISION	*//* roaches turn when bumping into each other */
#define INTELLIGENT		/* roaches sense near window */

#ifdef DEBUG
#define D(x)	x;
#else
#define D(x)	;
#endif

int 	RandInt(int maxVal);
void 	AddRoach(void);
void	TurnRoach(Roach *);
void	MoveRoach(int rx);
int 	RoachOverRect(Roach *, int, int, int, int, unsigned int, unsigned int);
int 	RoachInRect(Roach *, int, int, int, int, unsigned int, unsigned int);
void	DrawRoaches(void);
void	MoveRoaches(void);
void 	RemoveRoaches(void);
int		MarkHiddenRoaches(void);
void	checkSquish(void);
void	ConvertRoachData(void);
struct Window *FindRootWindow(struct Screen *);
struct Bob *MakeBOBFromData(char *data, UWORD width, UWORD height);
void 	SetupAnimation(void);
void 	CleanExit(LONG ReturnCode);
void 	Die(const UBYTE * msg);
int 	main(int argc, char **argv);


/*
 * Global Data
 */
struct GelsInfo *GelsInfo;
struct Layer *myLayer;
struct Layer_Info *LayerInfo;
char   PubScreen[MAXNAMESIZE];
int    RasDepth;
struct RastPort *rp;
struct RDArgs RDArgs;
struct Screen *Screen;
struct ViewPort *vp;
struct Window *roachWindow;
struct Window *userWindow;


/*
 * Parameters for SAS/C 6
 */
int __oslibversion = 37;	/* for SAS/C auto-init */
char __stdiowin[] = "CON:10/10/600/100/ARoach Output";    /* SAS/C window from WB */
long __stack = 16000;	/* enough for 1000 Roaches */

/*
 * Things for option parsing
 */
#define ROACH_LIMIT		1000	/* max. # of roaches */
					/* depends on __stack */
#define ROACH_SPEED		20	/* default speed */
#define ROACH_NUM		10	/* default # of roaches */

#define TEMPLATE "ROACHES/K/N,SPEED/K/N,SQUISH/S,SCREEN=PUBSCREEN/K,HELP/S"
enum { OPT_ROACHES, OPT_SPEED, OPT_SQUISH, OPT_SCREEN, OPT_HELP,
	   OPT_COUNT};

ULONG opts[OPT_COUNT];

char ExtendedHelp[] =	/* Yes, ReadArgs() _can_ give extended help! */
    "\n"
	"ARoach - cockroaches hide under your windows\n"
    "\n"
	"© Stefan Winterstein 1993. Freely Distributable.\n"
	"The original Xroach is © 1991 by J.T. Anderson.\n"
	"Options:\n"
    "\tROACHES=<n>     : Number of roaches (default: 10)\n"
    "\tSPEED=<n>       : Speed for the insects (default: 20)\n"
    "\tSQUISH          : Enables roach squishing with mouse click\n"
    "\tSCREEN=<string> : Name of public screen to run on.\n"
    ;

/*
 * Tags for the roach window
 */
struct TagItem roachWindowTags[] = {
	{WA_Backdrop, TRUE},
	{WA_Borderless, TRUE},
// 	{WA_SimpleRefresh, TRUE},	/* -> faster, but garbage on screen */
 	{WA_SmartRefresh, TRUE},
	{WA_IDCMP, IDCMP_MOUSEBUTTONS},
	{WA_PubScreenName, (IPTR) PubScreen},
    {WA_PubScreenFallBack, TRUE},
	{TAG_DONE, NULL}
};

/*
 * Tags for the user window
 */
struct TagItem userWindowTags[] = {
	{WA_Width, 160},
	{WA_Height, 50},
	{WA_MaxWidth, -1},
	{WA_MaxHeight, -1},
	{WA_IDCMP, IDCMP_CLOSEWINDOW},
	{WA_Flags, WFLG_CLOSEGADGET + WFLG_DRAGBAR + WFLG_SIZEGADGET + WFLG_DEPTHGADGET + WFLG_RMBTRAP},
	{WA_Title, (IPTR) "ARoach"},
	{WA_PubScreenName, (IPTR) PubScreen},
    {WA_PubScreenFallBack, TRUE},
	{TAG_DONE, NULL}
};

/*
 * Things from Xroach
 */
unsigned int display_width, display_height;
int xoffset, yoffset;

Roach *roaches;
int maxRoaches;
int curRoaches;
float roachSpeed;
BOOL squishRoach = FALSE;

extern RoachMap roachPix[];
extern RoachMap squish;
extern unsigned char ReverseBits [];

/**********************************************************************/
/*                                                                    */
/*                          XRoach functions                          */
/*                                                                    */
/**********************************************************************/

/*
   Generate random integer between 0 and maxVal-1.
*/

#define RandInt(x)	RangeRand(x)	/* from amiga.lib */

/*
 * __inline int RandInt(int maxVal)
 * {
 *  	return ((rand()>>16) % maxVal);
 * }
 */



/*
   Give birth to a roach.
	i.e. set up "roaches[curRoaches]"
*/
void
AddRoach()
{
    Roach *r;
	struct Bob *bob;

    if (curRoaches >= maxRoaches)
		return;

	r = &roaches[curRoaches++];
    /* chose a heading */
	r->index = RandInt(ROACH_HEADINGS);
	/* find appropriate RoachMap */
	r->rm = &roachPix[r->index];

	/* Coordinates are for screen, not window */
	r->x = RandInt(display_width - r->rm->width) + xoffset;
	r->y = RandInt(display_height - r->rm->height) + yoffset;
	r->intX = r->x;
	r->intY = r->y;
	r->hidden = 0;
	r->steps = RandInt(200);
	r->turnLeft = RandInt(100) >= 50;

	/*
	 * Generate a bob for this roach.
	 * We find the image in our current RoachMap
	 */
	bob = MakeBOBFromData(r->rm->roachBits, r->rm->width, r->rm->height);

	bob->BobVSprite->X = r->x;
	bob->BobVSprite->Y = r->y;

	InitMasks(bob->BobVSprite);
	AddBob(bob, rp);

	r->bob = bob;

}

/*
   Check for roach overlapping specified rectangle.
*/
__inline int
RoachOverRect(roach, rx, ry, x, y, width, height)
Roach *roach;
int rx;
int ry;
int x;
int y;
unsigned int width;
unsigned int height;
{
    if (rx >= (x + width)) return 0;
    if ((rx + roach->rm->width) <= x) return 0;
    if (ry >= (y + height)) return 0;
    if ((ry + roach->rm->height) <= y) return 0;

    return 1;
}

/*
   Check for roach completely in specified rectangle.
*/
int
RoachInRect(roach, rx, ry, x, y, width, height)
Roach *roach;
int rx;
int ry;
int x;
int y;
unsigned int width;
unsigned int height;
{
    if (rx < x) return 0;
    if ((rx + roach->rm->width) > (x + width)) return 0;
    if (ry < y) return 0;
    if ((ry + roach->rm->height) > (y + height)) return 0;

    return 1;
}

/*
   Move a roach.
*/
void
MoveRoach(rx)
int rx;
{
    Roach *roach;
    float newX;
    float newY;
#ifdef CHECK_COLLISION
    Roach *r2;
    int ii;
#endif

    roach = &roaches[rx];
    newX = roach->x + (roachSpeed * roach->rm->cosine);
    newY = roach->y - (roachSpeed * roach->rm->sine);

    if (RoachInRect(roach, (int)newX, (int)newY,
				     xoffset, yoffset, display_width, display_height)) {

		roach->x = newX;
		roach->y = newY;
		roach->bob->BobVSprite->X = roach->x;
		roach->bob->BobVSprite->Y = roach->y;

		if (roach->steps-- <= 0) {
		    TurnRoach(roach);
		    roach->steps = RandInt(200);
		}

#ifdef CHECK_COLLISION
		/*
		 * collision checking
		 */
		for (ii=rx+1; ii<curRoaches; ii++) {
		    r2 = &roaches[ii];

			if (r2->hidden)	/* so roach will not turn before window */
				continue;	/* if there are already others hiding */

		    if (RoachOverRect(roach, (int)newX, (int)newY,
				r2->intX, r2->intY, r2->rm->width, r2->rm->height)) {

				TurnRoach(roach);
		    }
		}
#endif
    }
    else {
		TurnRoach(roach);
	    roach->steps = RandInt(100);
    }

}

/*
   Turn a roach.
*/
void
TurnRoach(roach)
Roach *roach;
{

    if (roach->turnLeft) {
		roach->index += (RandInt(70) / 10) + 1;
		if (roach->index >= ROACH_HEADINGS)
		    roach->index -= ROACH_HEADINGS;
    }
    else {
		roach->index -= (RandInt(70) / 10) + 1;
		if (roach->index < 0)
		    roach->index += ROACH_HEADINGS;
    }

	roach->rm = &roachPix[roach->index];

	roach->bob->BobVSprite->ImageData = (UWORD *) roach->rm->roachBits;
	InitMasks(roach->bob->BobVSprite);
}

#ifdef INTELLIGENT
void
TurnToHeading(Roach *roach, int index)
{
	roach->steps = 100;

    roach->index = index;
	roach->rm = &roachPix[roach->index];

	roach->bob->BobVSprite->ImageData = (UWORD *) roach->rm->roachBits;
	InitMasks(roach->bob->BobVSprite);

}
#endif

/*
   Mark hidden roaches.
*/
int
MarkHiddenRoaches()
{
    int rx;
    Roach *r;
    int nVisible;
	int ltvis, lbvis, rtvis, rbvis, visible;


    nVisible = 0;
    for (rx=0; rx<curRoaches; rx++) {
		r = &roaches[rx];

		/*
		 * Since we don't really care for exact info but for speed,
		 * we do not call LockLayerInfo here.
		 */
		ltvis = (WhichLayer(LayerInfo, r->intX +2, r->intY+13) == myLayer);
		lbvis = (WhichLayer(LayerInfo, r->intX +2, r->intY + r->rm->height-13) == myLayer);
        rtvis = (WhichLayer(LayerInfo, r->intX + r->rm->width -2, r->intY+13) == myLayer);
		rbvis = (WhichLayer(LayerInfo, r->intX + r->rm->width -2, r->intY + r->rm->height-13) == myLayer);

		visible = ltvis + lbvis + rtvis + rbvis;

		if (!r->hidden) {
		    if (visible == 0) {
					r->hidden = 1;

					D(printf("%d becomes hidden\n", rx))
		    }
		    else {  /* At least some part is hidden */
				nVisible++;

#ifdef INTELLIGENT
					if (visible <= 2) {
						if (!(rbvis || rtvis))	/* right side is hidden */
							TurnToHeading(r, 1);
						else if (!(lbvis || ltvis))	/* left side is hidden */
							TurnToHeading(r, 13);
						else if (!(lbvis || rbvis))	/* bottom is hidden */
							TurnToHeading(r, 17);
						else if (!(ltvis || rtvis))	/* top is hidden */
							TurnToHeading(r, 7);
					}
					else {
						if (!rtvis)
							TurnToHeading(r, 3);
						else if (!rbvis)
							TurnToHeading(r, 21);
						else if (!lbvis)
							TurnToHeading(r, 15);
						else if (!ltvis)
							TurnToHeading(r, 9);
					}
#endif
		    }
		}
		else {	/* Hidden roach becomes visible */
			if (visible) {
				r->hidden = 0;
				r->turnLeft = RandInt(100) >= 50;

				nVisible++;

				TurnRoach(r);

				/*
				 * Might panic now!!!
				 */
			}
		}
    }

  return nVisible;
}


/*
 *	checkSquish - Check to see if we have to squish any roaches.
 */
void checkSquish()
{
    int x, y;
    int rx, i;
    Roach *r;
	struct Bob *tmpbob;
	BOOL checksquish = FALSE;
	struct IntuiMessage *imsg;

	/* Check for CLICK*/
	while ((imsg = (struct IntuiMessage *) GetMsg(roachWindow->UserPort))) {
		if (imsg->Class == IDCMP_MOUSEBUTTONS && imsg->Code == SELECTDOWN) {
			checksquish = TRUE;
			x = imsg->MouseX;
			y = imsg->MouseY;
		}
		else
			D(printf("Received IDCMP %d\n", imsg->Class))

		ReplyMsg((struct Message *) imsg);
	}

	if (checksquish) {
	    for (rx=0; rx<curRoaches; rx++) {
			r = &roaches[rx];

			if (x > r->intX &&
			    x < (r->intX + r->rm->width) &&
			    y > r->intY &&
			    y < (r->intY + r->rm->height)) {

				/*
				 * Roach r has been squished!
				 */
  				RemIBob(r->bob, rp, vp );	/* Remove Bob from GEL-system */

			/*
			 * This causes the next SortGList() to
			 * access illegal memory. WHY?
			 *
 			 *	freeBob(r->bob, RasDepth);  / Free its Bob struct /
			 *
			 * Then change FreeAllRoaches(): for(.. <curRoaches...)
			 */
				tmpbob = r->bob;

				BltTemplate((PLANEPTR) squish.roachBits, 0, (squish.height  / 8),
								rp, r->intX, r->intY, r->rm->width, r->rm->height);

			    for (i = rx; i < curRoaches - 1; i++) {
			        roaches[i] = roaches[i + 1];
	            }
				roaches[curRoaches-1].bob = tmpbob;

			    rx--;
			    curRoaches--;
			}
	    }
	}
    return;
}

/**********************************************************************/
/*                                                                    */
/*                     Amiga Animation functions                      */
/*                                                                    */
/**********************************************************************/

void
DrawRoaches()
{
#ifdef ROACHNUMBERING
Roach *r;
char number[4];
int i;
#endif

 	SortGList(rp);
 	WaitTOF();
	DrawGList(rp, vp);

#ifdef ROACHNUMBERING
	for (i=0; i < maxRoaches; i++) {
		r = &roaches[i];

		sprintf(number, "%2d", i);
		Move(rp, r->intX + 10, r->intY +10);
		Text(rp, number, 2);
	}
#endif

}

void
MoveRoaches()
{
int rx;
Roach *roach;

	for (rx=0; rx<curRoaches; rx++) {
		roach = &roaches[rx];
	    if (!roach->hidden) {
			MoveRoach(rx);
		    roach->intX = roach->x;
		    roach->intY = roach->y;
		}
	}

}

struct Bob *
MakeBOBFromData(char *data, UWORD width, UWORD height)
{
   NEWBOB newbob;
   struct Bob *bob;

	newbob.nb_Image = (UWORD *) data;
	newbob.nb_WordWidth = (width + 15) / 16;
	newbob.nb_LineHeight = height;
	newbob.nb_ImageDepth = ROACH_DEPTH;
	newbob.nb_PlanePick = 1;
	newbob.nb_PlaneOnOff = 0;
	newbob.nb_BFlags = SAVEBACK+OVERLAY;
	newbob.nb_DBuf = 0;
	newbob.nb_RasDepth = RasDepth;
	newbob.nb_X = 0;
	newbob.nb_Y = 0;
	newbob.nb_HitMask = 0;
	newbob.nb_MeMask = 0;

   bob = makeBob(&newbob);	/* from animtools */

   return(bob);
}


/*
 * X-Bitmap data stores bits reversed:  the leftmost bit in the bitmap is
 * the least significant bit in the character.
 * So we have to mirror them.
 */
void
ConvertRoachData()
{
int i, rx;
unsigned char *a;
RoachMap *rm;

	for (rx=0; rx<ROACH_HEADINGS; rx++) {

		rm = &roachPix[rx];
		a = rm->roachBits;

		for (i=0; i < (rm->height * (rm->width / 8)); i++) {
			a[i] = ReverseBits[a[i]];
		}
	}

	a = squish.roachBits;
	for (i=0; i < (squish.height * (squish.width  / 8)); i++) {
		a[i] = ReverseBits[a[i]];
	}

}


void SetupAnimation()
{
int ax;
RoachMap *rm;
int rx;
float angle;

	GelsInfo = setupGelSys(rp, 0);
	if (GelsInfo == NULL)
		Die("InitGELSystem() failed.\n");

	ConvertRoachData();

    /*
       Initialize sine & cosine in RoachMaps array.
    */
    for (ax=0; ax<360; ax+=ROACH_ANGLE) {
		rx = ax / ROACH_ANGLE;
		angle = rx * 0.261799387799;
		rm = &roachPix[rx];
		rm->sine = sin(angle);
		rm->cosine = cos(angle);
    }

	/*
	 * Setup roach array
	 */
    roaches = (Roach *) AllocMem(sizeof(Roach) * maxRoaches, MEMF_CLEAR);
	if (roaches == NULL)
		Die("Out of memory.\n");

    while (curRoaches < maxRoaches)
		AddRoach();		/* initializes "roaches[curRoaches]" */

    if (squishRoach) {
	}

}

/*
 * Remove all remaining roaches from the GEL-system
 */
void RemoveRoaches()
{
int i;

	/*
	 * All roaches >= curRoaches have already been removed by checkSquish
	 */
    for (i=0; i<curRoaches; i++) {
		RemBob(roaches[i].bob);
	}

}


void FreeAllRoaches(void)
{
int i;

	for (i=0; i < maxRoaches; i++) {
		if (roaches[i].bob) {
			freeBob(roaches[i].bob, RasDepth);
		}
	}

	FreeMem(roaches, sizeof(Roach) * maxRoaches);
}


/**********************************************************************/
/*                                                                    */
/*                       Own support functions                        */
/*                                                                    */
/**********************************************************************/


/*
 * FindRootWindow
 *
 * Opens and returns backdrop window on screen or NULL.
 * Screen must be locked.
 */
struct Window *
FindRootWindow(struct Screen *screen)
{
struct Window *win = NULL;

#ifdef USE_WB_WINDOW
	win = screen->FirstWindow;

	while (win) {
		if (win->Flags & WFLG_BACKDROP)
			break;
		win = win->NextWindow;
	}
#endif /* USE_WB_WINDOW */

	/*
	 * Open a backdrop if none available
	 */
	if (win == NULL) {
		roachWindow = OpenWindowTagList(NULL, roachWindowTags);	/* global  */
		win = roachWindow;
	}

	if (win == NULL)
		Die("Could not open window.\n");

	return(win);
}


/*
 * Generate a command line from Tooltypes and supply it to rda.
 */
void
Tooltypes2RDArgs(struct RDArgs *rda, int argc, char **argv)
{
char *cmdline, tmpbuffer[MAXCMDLINE];
char **ttarray, *tt;
int linelen;

	if ((ttarray = ArgArrayInit(argc, argv))) {

		*tmpbuffer = '\0';
		tt = *ttarray;

		for ( ; ttarray && tt; ttarray++, tt = *ttarray) {
			D(printf("%s\n", tt))

	 		if ( isalnum(*tt) ) {
				/*
				 * Idea: If there is a ' ' in ttarray, quote the argument
				 */
				strncat(tmpbuffer, tt, MAXCMDLINE);
				strncat(tmpbuffer, " ", MAXCMDLINE);
			}
            else {	/* Skip commented tooltypes */
				D(printf("Skipping `%s'\n", ttarray))
            }
		}

		strncat(tmpbuffer, "\n", MAXCMDLINE);	/* to make ReadArgs happy */
		linelen = strlen(tmpbuffer);

		if (linelen > 0) {
			cmdline = AllocMem(linelen, MEMF_CLEAR);
			if (cmdline == NULL) {
				Die("No memory for cmdline.\n");
			}
			strncpy(cmdline, tmpbuffer, linelen);

			rda->RDA_Source.CS_Buffer = cmdline;
			rda->RDA_Source.CS_Length = linelen;

			D(printf("Built cmdline:\n%s\n", cmdline))
		}

		ArgArrayDone();
	}

	return;
}


void Initialize(int argc, char **argv)
{
struct Window *rootWin;		/* the window we will draw in */

	/*
	 * All libraries are opened by SAS/C autoinitialization (hopefully)
	 */

	RDArgs.RDA_ExtHelp = ExtendedHelp;

	if (argc == 0) {	/* we were started from Workbench */
		Tooltypes2RDArgs(&RDArgs, argc, argv);
	}

	if (ReadArgs(TEMPLATE, opts, &RDArgs) == NULL) {
        PrintFault(IoErr(), NULL);
		printf("Your options are not valid.\n");
		CleanExit(RETURN_WARN);
	}

	if (opts[OPT_HELP]) {
		printf("%s", ExtendedHelp);
		FreeArgs(&RDArgs);	/* don't forget! */
		CleanExit(RETURN_OK);
	}

	if (opts[OPT_ROACHES]) {
		maxRoaches = * (ULONG *) opts[OPT_ROACHES];
	}
	if (maxRoaches > ROACH_LIMIT) {
		printf("Won't use more than %d roaches.\n", ROACH_LIMIT);
		maxRoaches = ROACH_LIMIT;
	}
	if (maxRoaches < 1) {
		maxRoaches = ROACH_NUM;
	}
	if (opts[OPT_SPEED]) {
		roachSpeed = * (ULONG *) opts[OPT_SPEED];
	}
	if (roachSpeed < 1)
		roachSpeed = ROACH_SPEED;

	if (opts[OPT_SQUISH]) {
		squishRoach = TRUE;
	}

	if (opts[OPT_SCREEN]) {
		strncpy(PubScreen, (char *) opts[OPT_SCREEN], MAXNAMESIZE);
	}
	else
		strcpy(PubScreen, "Workbench");

	FreeArgs(&RDArgs);

	Screen  = LockPubScreen(PubScreen);
	if (Screen == NULL) {
		printf("Cannot lock screen `%s', using default screen.\n", PubScreen);
		Screen = LockPubScreen(NULL);
		if (Screen == NULL)
			Die("No screen available.\n");
	}

	ScreenToFront(Screen);

	userWindow = OpenWindowTagList(NULL, userWindowTags);	/* global  */
	if (userWindow == NULL)
		Die("Could not open user window.\n");

	rootWin = FindRootWindow(Screen);
	if (!rootWin)
		Die("Cannot find root window.\n");

	xoffset = 0;
	yoffset = Screen->BarHeight;

	display_width = rootWin->Width;
	display_height = rootWin->Height;

	rp = rootWin->RPort;
	SetAPen(rp, 1);
	SetDrMd(rp, JAM1);

	vp = &(rootWin->WScreen->ViewPort);
	RasDepth = rp->BitMap->Depth;

	LayerInfo = &(Screen->LayerInfo);
	myLayer = rp->Layer;

 	RangeSeed = -VBeamPos();	/*  Seed generator for amiga.lib */
}


void CleanExit(LONG ReturnCode)
{
	if (curRoaches > 0)
		RemoveRoaches();

	if (GelsInfo)
		cleanupGelSys(GelsInfo, rp);

	if (roaches)
		FreeAllRoaches();

	if (RDArgs.RDA_Source.CS_Buffer)
		FreeMem(RDArgs.RDA_Source.CS_Buffer, RDArgs.RDA_Source.CS_Length);

	if (roachWindow)
		CloseWindow(roachWindow);

	if (userWindow)
		CloseWindow(userWindow);

	if (Screen)
		UnlockPubScreen(NULL, Screen);

	exit(ReturnCode);
}


/* --------------------------------------------------------------
   Die on fatal error. Prints out info text, then exits gracefully.
   -------------------------------------------------------------- */
void Die(const UBYTE * msg)
{
	printf(msg);
	CleanExit(RETURN_FAIL);
}


BOOL KeepRunning(void)
{
struct IntuiMessage *imsg;
BOOL running = TRUE;

	if (curRoaches <= 0) {
		running = FALSE;
	}

	/* Check for CTRL-C */
	if (SetSignal(0L, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) {
		running = FALSE;
	}

	/* Check for CLOSEWINDOW */
	while ((imsg = (struct IntuiMessage *) GetMsg(userWindow->UserPort))) {
		if (imsg->Class == IDCMP_CLOSEWINDOW) {
			running = FALSE;
		}
		ReplyMsg((struct Message *) imsg);
	}

	return(running);
}

struct Library * LayersBase;
struct GfxBase * GfxBase;
struct Library * IconBase;
struct IntuitionBase * IntuitionBase;

BOOL openlibs(void)
{
  LayersBase = OpenLibrary("layers.library",0);
  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
  IconBase = OpenLibrary("icon.library",0);
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
  
  if (!LayersBase || !GfxBase || !IconBase || !IntuitionBase)
    return FALSE;
  
  return TRUE;    
}

void closelibs(void)
{
  if (LayersBase) CloseLibrary(LayersBase);
  if (GfxBase) CloseLibrary((struct Library *)GfxBase);
  if (IconBase) CloseLibrary(IconBase);
  if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
}


void __chkabort(void) {}

int
main(int argc, char **argv)
{
int nVis;

   if (TRUE == openlibs())
   {
	Initialize(argc, argv);

	SetupAnimation();

	nVis = maxRoaches;

	while (KeepRunning()) {

		MoveRoaches();

		if (nVis > 0)
			DrawRoaches();
		else
			Delay(5);

    	nVis = MarkHiddenRoaches();

		if (squishRoach)
			checkSquish();
	}
    }
    closelibs();
    
	CleanExit(RETURN_OK);

}

