#include "csource/VT340/VT340_models.h"
#include "ownincs/xem.h"

#ifndef abs
#define abs(x) ((x)<0?-(x):(x))
#endif

#define SYSTEM_TEXTSTYLES (FSF_BOLD | FSF_UNDERLINED)

#define ATTR_NORMAL		0
#define ATTR_UNDERLINE	(1<<4)
#define ATTR_FAT			(1<<5)
#define ATTR_HIGHLIGHT	(1<<6)
#define ATTR_INVERSE		(1<<7)
#define ATTRIBUTES_WORD ((con->apen<<8) | con->attr | con->bpen)
#define ATTRIBUTES_APEN(word) ((word>>8) & 0x0F)
#define ATTRIBUTES_BPEN(word) (word & 0x0F)
#define ATTRIBUTES_STYLE(word) (word & 0x60)

#define DCS_BUFLEN 2048
#define CURSOR_ON  50
#define CURSOR_OFF 40

#define DEFAULT_SCREENNAME "ram:screen.asc"
#define DEFAULT_PRINTERNAME "prt:"

#define SETS 4		/* VT340 has 4 charset-registers.. */
enum { ASCII=0, DEC_SUPPLEMENTAL, ISO_SUPPLEMENTAL, GRAPHICS, TECHNICAL, FONTS };

#define F_GXGL				(1<<0)	/* für fstat */

#define T_USE_REGION		(1<<0)	/* für tstat */
#define T_WRAP				(1<<1)

#define U_CURSOR_VISIBLE (1<<0)	/* für ustat */
#define U_INSERT			(1<<1)
#define U_NEWLINE			(1<<2)
#define U_KEYAPP			(1<<3)
#define U_CURAPP			(1<<4)
#define U_HOST_STATUSLINE_ACTIVE	(1<<5)
#define U_HOST_STATUSLINE		(1<<6)
#define U_BACKSPACE_BS			(1<<7)

#define P_24LINES			(1<<0)	/* für pstat */
#define P_ADJUST_LINES	(1<<1)
#define P_ISO_LATIN1		(1<<2)
#define P_CONVERT			(1<<3)
#define P_DESTRUCTIVEBS	(1<<4)
#define P_DESTRUCTIVEFF	(1<<5)
#define P_DESTRUCTIVEDEL	(1<<6)
#define P_SMOOTH_SCROLL	(1<<7)
#define P_ANSI_MODE		(1<<8)	/* else VT-52 */
#define P_VT340_MODE		(1<<9)	/* else VT-102 */
#define P_OWN_STATUSLINE (1<<10)
#define P_ANSI_COLORS   (1<<11)
#define P_QUICK_REND		(1<<12)
#define P_CURSOR_BLINKING (1<<13)
#define P_CURSOR_UNDERSCORE (1<<14)

#define G_AUTO_PRINT		(1<<0)	/* für gstat */
#define G_AUTO_PRINT_INACTIVE (1<<1)
#define G_PRINT_CONTROL	(1<<2)
#define G_FULLSCREEN		(1<<3)
#define G_FORMFEED		(1<<4)

#define SCREEN_BUF 32
#define ANSWER_BACK 32

#define NUL  '\x00'
#define ENQ  '\x05'
#define BEL  '\x07'
#define BS   '\x08'
#define HT   '\x09'
#define LF   '\x0A'
#define VT   '\x0B'
#define FF   '\x0C'
#define CR   '\x0D'
#define SO   '\x0E'
#define SI   '\x0F'
#define DLE  '\x10'
#define XON  '\x11'
#define XOF  '\x13'
#define CAN  '\x18'
#define SUB  '\x1A'
#define ESC  '\x1B'
#define FS   '\x1C'
#define GS   '\x1D'
#define RS   '\x1E'
#define US   '\x1F'
#define DEL  '\x7F'
#define IND  '\x84'
#define NEL  '\x85'
#define SSA  '\x86'
#define ESA  '\x87'
#define HTS  '\x88'
#define RI   '\x8D'
#define SS2  '\x8E'
#define SS3  '\x8F'
#define DCS  '\x90'
#define STS  '\x93'
#define SPA  '\x96'
#define EPA  '\x97'
#define CSI  '\x9B'
#define ST   '\x9C'
#define OSC  '\x9D'
#define PM   '\x9E'
#define APC  '\x9F'
#define END  '\xFF'

#define XSIZE			8		/* lots of `<< 3' in the source..!! */
#define FONT_YSIZE	8
#define MAX_YSIZE		11
#define BOTLINE		6
#define ARGUMENTS		16
#define MAXCOLUMNS	80
#define MAXROWS		144

#define BACKGROUND_PEN	0

#define MAXARGS	16
#define MAXINTER	10

#define ERASEABLE 'E'
#define DONT_ERASE 'N'

enum	{ IDENT_VT100, IDENT_VT101, IDENT_VT102, IDENT_VT220, IDENT_VT340 };
enum	{ SCALE_ATTR_TOP2X=3, SCALE_ATTR_BOT2X, SCALE_ATTR_NORMAL, SCALE_ATTR_2X };

struct SpecialKey {
	VOID	(*Routine)(struct VT340Console *);
	UWORD Key;	/* UWORD just because of padding..)-: */
};



struct VT340Console {
	struct XEM_IO *io;
	struct XEmulatorMacroKey *macrokeys;
	struct RastPort *rp;

	struct IBox border;

	struct Window *lockedWindow;


/*** Page stuff ***/
	struct List page[6];
	struct PageLine *topDisplayLine;
	struct PageLine *pageBuffer;
	UBYTE	pagesInUse;
	UBYTE	linesPerPage;
	UBYTE	currentPage;
	BYTE	cursorVisible;
	struct timerequest *doTime;
	struct MsgPort *timerPort;
	BOOL requestPending;
/******************/


	BPTR	screenHandle;
	BPTR	printerHandle;

	UBYTE	inESC;
	UBYTE	inCSI;
	UBYTE	inTEK;
	UBYTE	inDCS;
	UBYTE	inPrivate;
	UBYTE isReady;		/* TRUE, if XEmulatorOpenConsole() was successful.. */
	BOOL	displayInactive;

/***** scalefont *****/
	struct RastPort		*ScaleRPort;
	struct BitMap			*ScaleSrcBitMap;
	struct BitMap			*ScaleDstBitMap;
	struct BitScaleArgs	*ScaleArgs;

	UBYTE	ScaleTextStyle;
	WORD	ScaleCache;
	WORD	PlaneWidth;
	WORD	PlaneHeight;
/*********************/
	BOOL	hiLite;
	BOOL	mono;
	BOOL	bytesWritten;
	BOOL	holdScreen;
	UWORD foregroundPen;
	UWORD writeMask;

	UWORD	gset[SETS];
	UWORD	s_gset[SETS];
	UBYTE *fonts[FONTS];
	struct TextFont *dcFont;		/* display_controls font */
	struct TextFont *topazFont;	/* status-line font */
	struct Remember *fontRemember;

/*
**	UBYTE *textRaster;
**	UBYTE *eraseRaster;
**	UWORD *penRaster;
**	UWORD rasterWidth;
**	UWORD rasterHeight;
*/

	UWORD lset;
	UWORD slset;
	UWORD s_lset;
	UWORD rset;
	UWORD s_rset;

	ULONG argc;

	UBYTE	*DCS_buffer;

	WORD	columns;
	WORD	firstLine, lastLine, statusLine;
	WORD	top, bot;
	WORD	col, s_col, m_col;
	WORD	row, s_row, m_row;

	UBYTE	attr, s_attr;	/* -> ATTR_HIGHLIGHT, ATTR_FAT, etc.. */
	UBYTE	apen, s_apen;
	UBYTE	bpen, s_bpen;

	UWORD	upss;		/* User Preferred Supplemental set */

	UWORD	gstat;

	UWORD pstat;	/* private settings */
	UWORD r_pstat;

	UWORD	tstat;	/* settings that will be saved */
	UWORD	s_tstat;
	UWORD	r_tstat;

	UWORD	fstat;	/* textfont settings */

	UWORD	ustat;	/* user settings */
	UWORD	r_ustat;

	BOOL	CSIcmd;
	BOOL	ESCcmd;
	UWORD	firstArg;

	UWORD col0;
	UWORD col1;

	UWORD	ySize;	/* Höhe der zu bearbeitenden Linie..!! */
	UWORD	ordCol;
	UWORD	ordC;
	UWORD icnt;			/* offset für inter.. */

	UBYTE *buf;
	UBYTE *conv;

/* VT-52 area */
	BOOL VT52_dca;		/* this f*cking Direct Cursor Address */


/* TEKTRONIX TEK-4010/4014/4100 area.. */

	struct	Screen	*TEKscreen;
	struct	Window	*TEKwindow;
	struct	RastPort	*TEKrp;
	struct	AreaInfo TEKai;
	struct	TmpRas	TEKtr;
	VOID *TEKfillras;

	BOOL	TEKescmode;
	BOOL	TEKboxmode;
	BOOL	TEKcolormode;
	BOOL	TEKscale;

	enum { GIN=1, ALPHA, LINE, INCREMENT, POINT, MOVE };
	UWORD TEKmainmode;
	UWORD	TEKlace;		/* interlace TEK screen  0=no  1=yes */
	UWORD TEK4100;
	UWORD TEKapen;
	UWORD	TEKcolorindex;
	UWORD	TEKred;
	UWORD	TEKgreen;
	UWORD buffer[250];

	WORD	TEKx;			/*  X-Pos */
	WORD	TEKy;			/*  Y-Pos */
	WORD	TEKhiY;		/* High Y */ 
	WORD	TEKloY;		/* Low  Y */
	WORD	TEKhiX;		/* High X */

	UBYTE	TEKlastTag;
	UBYTE TEKlastc;

/* UBYTE-Array.. */
	UBYTE specialMap[256];
	UBYTE answerBack[ANSWER_BACK];
	UBYTE screenName[SCREEN_BUF];
	UBYTE printerName[SCREEN_BUF];
	UBYTE	ordText[MAXCOLUMNS];
	UBYTE	tabs[MAXCOLUMNS];

	UBYTE args[MAXARGS+1];/* in inESC wird der KommandoChar mit abgespeichert */
	UBYTE	inter[MAXINTER+1];		/* intermediates like '#' '?'.. */

	UBYTE	lastPRIVc;	/* only used by Privacy messages */
	UBYTE	DECSCA;
	UBYTE	identify;
	UBYTE	r_identify;
};

