/*
** $Id$
*/

#ifndef MBX
#define MAXKEYS 64
struct keyspec {
	IX *ix;
	ULONG action;
};
#endif

#ifndef MBX
#define USEWRITEMASK
#endif

#define NOMARK ((char *)-1)

#ifdef MBX
#define KERN(ch) getcharkern(data->font, ch)
#else
#define KERN(ch) ( data->fontfixed ? 0 : ((WORD *)data->font->tf_CharKern)[ ( *ch < data->font->tf_LoChar || *ch > data->font->tf_HiChar ? data->font->tf_HiChar + 1 : *ch ) - data->font->tf_LoChar ] )
#endif

#define NEGKERN(ch) ( KERN( ch ) < 0 )

#define PARSEURLS (data->clickableurls&&!(data->prohibitparse&MUIV_Textinput_ParseF_URL))

#define ACTION_INSERT     1
#define ACTION_DELETE     2
#define ACTION_REPLACE    3
#define ACTION_SURROUND   4
#define ACTION_UNSURROUND 5
#define ACTION_MASK  0x0000ffff
/* These flags for now work only under certain conditions */
#define ACTIONF_UNCONDITIONAL (1<<16)
#define ACTIONF_STRIPCR (1<<17)
#define ACTIONF_REDO (1<<18)
#define ACTIONF_NOFLUSH (1<<19)
#define ACTIONF_MASK 0xffff0000

struct actionnode
{
	struct MinNode node;
	ULONG type;
	ULONG pos;
	ULONG len;
	ULONG datalen;
	UBYTE data[ 0 ];
};

struct style {
	ULONG disable;
	ULONG usefortoken;
	ULONG styles;
	ULONG bgpen;
	ULONG fgpen;
	ULONG lastbgpen;
	ULONG lastfgpen;
	UBYTE html_isintag;
	UBYTE html_isinquote;
	UBYTE html_isincomment;
	UBYTE html_isinentity;
};

struct segment {
	UWORD xstart, xsize;
};

struct misspell {
	struct misspell *next;
	struct segment word;
	char misspell[ 0 ];
};

struct viewurl {
	struct viewurl *next;
	struct segment word;
	UBYTE selected;
	char url[ 0 ];
};

struct Data 
{
	#ifndef MBX
	//Rexx stuff
	struct MsgPort *rexxPort;
	int stillNeedReplies;
	ULONG rexxPortBits;
	struct MUI_InputHandlerNode rexxihn;
	#endif

	int undosize, undocount;
	struct MinList undolist;
	int redosize, redocount;
	struct MinList redolist;

	int maxundobytes;
	int maxundolevels;

	ULONG suggestparse;
	ULONG prohibitparse;

	// static stuff
	char *buffer;
	char *preparse;
	char *markstart, *markend;
	char *rmarkstart, *rmarkend;
	char *oldmarkstart;
	char *oldmarkend;
	char *cursorp;
	char *oldcursorp;
	char *calcbuf;
	char *startmisspell;
	char *endmisspell;
	int bookmarks[3];
	int calcbuflen;
	int preparsemaxlen;
	int maxlen;
	int maxlines;
	int setmin;
	int setmax;
	int setvmin;
	int setvmax;
	int findurls;
	int clickableurls;
	int spellcheck;
	int do_colors;
	int quickrender;
	int patternback;
	int format;
	int smooth;
	int tabs;
	int tablen;
	struct Hook *handleurlhook;
	ULONG fonttype;
	struct TextFont *font;

	#ifndef MBX
	struct fontcache *farray;
	#endif

	// display variables
	int topline; // top line
	int xoffset; // x pixel offset
	int yoffset; // y pixel offset
	int ycenter; // pixel offset for vertical centering
	int visiblelines;
	int lineheight;

	int linenum, maxxsize, maxlinewidth;

	int cursorx, cursory;
	int oldcursorx, oldcursory;

	int downx, downy;
	ULONG lastsecs, lastmics;
	int dblclickcount;

	int wordwrap;

	BFLAG multiline;
	BFLAG isactive;
	BFLAG autoexpand;
	BFLAG fontset;

	BFLAG cursorstate; // TRUE = invisible
	BFLAG blinkihnactive;
	BFLAG advanceoncr;
	BFLAG externaledit;	// in external edit mode

	BFLAG quiet;
	BFLAG needsinform;
	BFLAG isshown;
	BFLAG dowordwrap;

	BFLAG changed;
	BFLAG remainactive;
	BFLAG isnumeric;
	BFLAG issecret;

	BFLAG noedit;
	BFLAG negativekerning; // cursor is on a char with negative kerning
	UBYTE maxpen;
	UBYTE fontfixed;

	UBYTE controlchar;
	BFLAG noinput;
	BFLAG nocopy;
	BFLAG emailstyles;

	BFLAG goinactive; // Go inactive on next input event (i.e. keyup)

	BFLAG resetmarkoncursor;

	BFLAG padflags[ 2 ];

	struct TextFont *closefont;	// font to close

	ULONG pens[ 14 ];

	ULONG colpens[ 16 ];

	APTR scrollobj;
	struct viewurl *redrawurl;
	struct misspell *redrawmisspell;
	int redrawmode; // redraw what
	int markmode;
	int markmethod;
	int blinkrate;
	int cursorstyle;
	int cursorsize;
	int tickcount;
	int styles;

	int pxo, cellsize, oldpxo, oldcellsize; // cursor rendering info

	int minnumval, maxnumval;
	char *acceptchars, *rejectchars;

	char tmpfilename[ 32 ];
	char tmpextension[ 8 ];

	#ifndef MBX
	APTR eeh;
	#endif

	APTR popupmenu;
	APTR oldmenustrip;

	APTR attachedlist, attachedpop;

	int numkeys;
	int popupstate;

	int old_xoffset, old_topline, old_maxx, old_linenum;
	int oldpri;

 	APTR pool;
	struct viewurl *urls;
	struct misspell *misspells;

	struct MUI_EventHandlerNode ehn;
	struct MUI_InputHandlerNode ihn;

	char *undobuffer;
	int undobuffersize;

#ifndef MBX
	struct keyspec keyspecs[MAXKEYS];
#endif
	BFLAG noExtraSpacing;
};

enum {
	PENMODE_NORMAL,
	PENMODE_MARK,
	PENMODE_CURSOR
};

enum {
	pen_f_normal,
	pen_b_normal,
	pen_f_active,
	pen_b_active,
	pen_f_mark,
	pen_b_mark,
	pen_f_cursor,
	pen_b_cursor,
	pen_f_style,
	pen_b_style,
	pen_u_url,
	pen_s_url,
	pen_u_misspell,
	pen_f_pattern
};

enum {
	MARKMETHOD_CHAR,
	MARKMETHOD_WORD,
	MARKMETHOD_LINE,
	MARKMETHOD_ALL
};

enum {
	RDO_NONE,
	RDO_CURRENT,	// redraw current line (cursor)
	RDO_CURRENT_PREV,
	RDO_CURRENT_NEXT,
	RDO_CONTENTS,
	RDO_CURSOR,
	RDO_FROMCURSOR,
	RDO_FROMCURSOR_PREV,
	RDO_MOVECURSOR,
	RDO_MOVEMARK,
	RDO_SCROLL,
	RDO_URL,
	RDO_MISSPELL
};

#define YSPACING (data->noExtraSpacing?0:1)
#define XSPACING (data->noExtraSpacing?0:2)

