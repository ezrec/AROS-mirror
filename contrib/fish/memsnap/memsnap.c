#include <aros/oldprograms.h>

/*
*	MemSnap.c
*
*	Just like MeMeter (a program I've been using for ages now).
*	Why write it? The MeMeter I've been using crashes under 2.0,
*	and I thought I'd take the time to learn (a bit) about 2.0
*	by programming with it. Nothing groundbreaking here, though.
*
*	Martin W Scott, 3/92.
*/
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include "wintext.h"		/* font-independent positioning of text */
#include "icon.h"
#include "menu.h"

char version_str[] = "$VER: MemSnapII 1.0 (" __DATE__ ")";

#define CHARS_ACROSS	32	/* max chars across for window texts */
#define CHARS_DOWN	 4	/* how many rows of text in window */
#define REMOVECHARS	18	/* how many cols removed when window small */

#define LARGE_HEADER "       Current Snapshot     Used"
#define SMALL_HEADER "    Memory"

#define ABOUT_TXT "\
  MemSnapII v1.0\n\
by Martin W. Scott\n\
\n\
     Freeware"

WINTEXT wtexts[] =
{
    {&wtexts[1], "       Current Snapshot     Used", 0, 0, 3, 0, JAM2},
    {&wtexts[2], " Chip", 0, 1, 2, 0, JAM2},
    {&wtexts[3], " Fast", 0, 2, 2, 0, JAM2},
    {NULL, "Total", 0, 3, 2, 0, JAM2}
};

char cbuf[3][9], sbuf[3][9], ubuf[3][9];

WINTEXT ctexts[] =
{
    {&ctexts[1], &cbuf[0][0], 6, 1, 1, 0, JAM2},
    {&ctexts[2], &cbuf[1][0], 6, 2, 1, 0, JAM2},
    {NULL, &cbuf[2][0], 6, 3, 1, 0, JAM2}
};

WINTEXT stexts[] =
{
    {&stexts[1], &sbuf[0][0], 15, 1, 1, 0, JAM2},
    {&stexts[2], &sbuf[1][0], 15, 2, 1, 0, JAM2},
    {NULL, &sbuf[2][0], 15, 3, 1, 0, JAM2}
};

WINTEXT utexts[] =
{
    {&utexts[1], &ubuf[0][0], 24, 1, 1, 0, JAM2},
    {&utexts[2], &ubuf[1][0], 24, 2, 1, 0, JAM2},
    {NULL, &ubuf[2][0], 24, 3, 1, 0, JAM2}
};

/* back to normal stuff */

struct Gadget drag_gadget =
{
/* whole window */
    NULL,
    0, 0, 0, 0,
    GRELWIDTH | GRELHEIGHT | GADGHNONE,
    GADGIMMEDIATE,
    GTYP_WDRAGGING
/* rest is 0/NULL */
};

struct Gadget biggadget =	/* this will be snapshot gadget */
{
    &drag_gadget,
    0, 0, 0, 0,
    GRELWIDTH | GADGHCOMP,
    RELVERIFY,
    GTYP_BOOLGADGET
/* rest is 0/NULL */
};

#define LEFTEDGE 40		/* window coordinates */
#define TOPEDGE  20

struct NewWindow nw =
{
    LEFTEDGE, TOPEDGE, 0, 0, -1, -1,	/* dimension, pens */
    NEWSIZE | MENUPICK | GADGETUP,	/* IDCMP flags */
    WFLG_SMART_REFRESH,		/* window flags */
    &biggadget, NULL,		/* gadgets, checkmark */
    NULL,			/* title */
    NULL, NULL,			/* screen, bitmap */
    0, 0, 0, 0,			/* extrema of dimensions */
    WBENCHSCREEN		/* screen to open onto */
};

struct TagItem wtags[] =
{
    {WA_AutoAdjust, TRUE},
    {TAG_DONE}
};

struct Window *w;		/* screen pointer */
struct Menu *menu;
struct GfxBase *GfxBase;	/* graphics pointer */
struct IntuitionBase *IntuitionBase;	/* intuition pointer */
struct Library *IconBase, *DiskfontBase, *GadToolsBase;
WINTEXTINFO wtinfo;
struct WBStartup *WBenchMsg = NULL;

#define MEMSNAP_TIME	10L
#define MEMONLY_TIME	25L

/******************************************************************************/

/* prototypes for general routines */

int main (int, char **);
BOOL OpenLibs(void);
void CloseAll(void); //, main(int, char **);
BOOL long2str(LONG, char *, UWORD);

#ifdef LATTICE	/* save the odd byte */
#include <stdlib.h>
void MemCleanup(void){}
#endif

BOOL 
OpenLibs()			/* open required libraries */
{
    if ((GfxBase = (void *) OpenLibrary("graphics.library", 0L)) &&
	(IntuitionBase = (void *) OpenLibrary("intuition.library", 37L)) &&
	(DiskfontBase = (void *) OpenLibrary("diskfont.library", 36L)) &&
	(GadToolsBase = (void *) OpenLibrary("gadtools.library", 37L)) &&
	(IconBase = (void *) OpenLibrary("icon.library", 37L)))
	return TRUE;
    CloseAll();
    return FALSE;
}


void 
CloseAll()			/* close opened libraries */
{
    if (menu)
    {
	ClearMenuStrip(w);
	FreeMemSnapMenu();
    }
    if (wtinfo.tf)
	CloseFont(wtinfo.tf);
    if (w)
	CloseWindow(w);
    if (IconBase)
	CloseLibrary(IconBase);
    if (DiskfontBase)
	CloseLibrary(DiskfontBase);
    if (GadToolsBase)
	CloseLibrary(GadToolsBase);
    if (GfxBase)
	CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase)
	CloseLibrary((struct Library *)IntuitionBase);
}


/* and this one is rather specific to this program... */

BOOL 
long2str(LONG n, char *s, UWORD len)	/* long to string, right-adjusted */
{				/* will NOT null-terminate */
    /* len is space in buffer (excl. '\0') */
    /* also, prints nothin if zero */
    short sign;			/* minus sign required? */
    char *t = &s[len - 1];	/* get last space in string */

    if (n < 0)			/* get sign of n */
    {
	n = -n;
	sign = -1;
	len--;			/* reduce space (we'll need it for '-') */
    }
    else
	sign = 0;

    while (n && len)		/* work to do and space to do it */
    {
	*t = '0' + (n % 10);	/* get rightmost digit */
	t--;			/* mave back up string */
	len--;
	n /= 10;
    }

    if (sign)
	*t-- = '-';		/* put sign in now */

    while (len--)		/* fill remainder with spaces */
	*t-- = ' ';

    if (n)
	return FALSE;		/* failure */
    return TRUE;
}


/******************************************************************************/

/* Memory data management/manipulation routines */

void obtainmem(ULONG *), submem(ULONG *, ULONG *, ULONG *), updatemem(ULONG *, WINTEXT *);

#define CHIP 0
#define FAST 1
#define TOTAL 2

#define clearmem(mem) 		mem[CHIP] = mem[FAST] = mem[TOTAL] = 0L

void 
obtainmem(ULONG * mem)		/* store current memory */
{
    mem[TOTAL] = mem[CHIP] = AvailMem(MEMF_CHIP);
    mem[TOTAL] += (mem[FAST] = AvailMem(MEMF_FAST));
}


void 
submem(ULONG * to, ULONG * from, ULONG * howmuch)	/* to = from - howmuch */
{
    to[CHIP] = from[CHIP] - howmuch[CHIP];
    to[FAST] = from[FAST] - howmuch[FAST];
    to[TOTAL] = from[TOTAL] - howmuch[TOTAL];
}


void 
updatemem(ULONG * mem, WINTEXT * memtext)	/* update specified display */
{
    long2str(mem[CHIP], memtext[CHIP].text, 8);
    long2str(mem[FAST], memtext[FAST].text, 8);
    long2str(mem[TOTAL], memtext[TOTAL].text, 8);

    RenderWinTexts(&wtinfo, memtext);
}

/* pop up a requester */
void
EasyEasyRequest(char *str)
{
    struct EasyStruct es;

    es.es_StructSize = sizeof(struct EasyStruct);

    es.es_Flags = 0L;
    es.es_Title = "MemSnap Message";
    es.es_TextFormat = str;
    es.es_GadgetFormat = "OK";
    EasyRequestArgs(NULL, &es, NULL, NULL);
}


#define Msg(s) EasyEasyRequest(s)

/******************************************************************************/


int
main (int argc, char **argv)		/* provide a memory 'meter' */
{
    struct IntuiMessage *msg;		/* our window messages */
    ULONG cmem[3], smem[3], umem[3];	/* storage of memory information */
    ULONG class;			/* message class */
    UWORD code;				/* and code */
    WORD smallwidth, largewidth;	/* possible window sizes */
    BOOL small;				/* are we small? */


    if (argc == 0)
	WBenchMsg = (struct WBStartup *) argv;

    if (!OpenLibs())		/* failure => under 1.3 */
	return;

    GetOurIcon(WBenchMsg);
    if (InitWinTextInfo(&wtinfo))
    {
	/* size window to fit screen font */
	nw.LeftEdge = TTInt("LEFTEDGE", LEFTEDGE);
	nw.TopEdge = TTInt("TOPEDGE", TOPEDGE);
	small = TTBool("SMALL", FALSE);
	FreeOurIcon();		/* finished with it */

	nw.Height = CHARS_DOWN * wtinfo.font_y + wtinfo.toffset + wtinfo.boffset;
	largewidth = CHARS_ACROSS * wtinfo.font_x + wtinfo.loffset + wtinfo.roffset;
	smallwidth = largewidth - REMOVECHARS * wtinfo.font_x;
	nw.Width = small ? smallwidth : largewidth;
	wtexts[0].text = small ? SMALL_HEADER : LARGE_HEADER;

	/* and set up big gadget */
	biggadget.LeftEdge = wtinfo.loffset;
	biggadget.TopEdge = wtinfo.toffset;
	biggadget.Width = -wtinfo.roffset-wtinfo.loffset;
	biggadget.Height = wtinfo.font_y;

	if (w = OpenWindowTagList(&nw, wtags))
	{
	    if (w->Width != nw.Width)
	    {
		Msg("Window too small for text\nChoose a smaller font");
		CloseAll();
		return;
	    }

	    if (menu = AllocMemSnapMenu(w))
		SetMenuStrip(w, menu);
	    else
	    {
		Msg("Couldn't create menus");
		CloseAll();
		return;
	    }

	    wtinfo.window = w;
	    SetFont(w->RPort, wtinfo.tf);
	    RenderWinTexts(&wtinfo, wtexts);	/* draw initial texts */

	    clearmem(smem);	/* initialize memory display */

	    for (;;)		/* main event loop */
	    {
		while (msg = (struct IntuiMessage *) GetMsg(w->UserPort))
		{
		    class = msg->Class;
		    code = msg->Code;
		    ReplyMsg((struct Message *) msg);

		    if (class == GADGETUP)
		    {
			if (!small)
			{
				/* new snapshot */
				obtainmem(smem);
				updatemem(smem, stexts);
			}
			else goto makelarge;	/* naughty but so what? */
		    }
		    else if (class == NEWSIZE)
		    {
		        RenderWinTexts(&wtinfo, wtexts);	/* redraw texts */
			if (!small)
			    updatemem(smem, stexts);
		    }
		    else if (class == MENUPICK)
		    {
			switch (ITEMNUM(code))
			{
			case SMALL:
			    if (!small)
			    {
				wtexts[0].text = SMALL_HEADER;
				SizeWindow(w, smallwidth-largewidth, 0);
				small = TRUE;
			    }
			    break;

			case LARGE:
makelarge:		    if (small)	/* to do: move if nec. */
			    {
				WORD movex;
				wtexts[0].text = LARGE_HEADER;
				movex = w->WScreen->Width - w->LeftEdge - largewidth;
				if (movex < 0)
				    MoveWindow(w, movex, 0);
				SizeWindow(w, largewidth-smallwidth, 0);
				small = FALSE;
			    }
			    break;

			case ABOUT:
			    Msg(ABOUT_TXT);
			    break;

			case QUIT:
			    CloseAll();
			    return;
			}
		    }

		}		/* while */

		/* do memory window stuff */
		obtainmem(cmem);
		updatemem(cmem, ctexts);

		if (small)
		{
		    Delay(MEMONLY_TIME);
		}
		else
		{
		    submem(umem, smem, cmem);
		    updatemem(umem, utexts);

		    Delay(MEMSNAP_TIME);
		}

	    } /* for */
	}
	else
	    Msg("Can't open window");
    }
    else
	Msg("Can't open font");

    FreeOurIcon();

    CloseAll();
    return 0;
}
