#include <devices/printer.h>
#include "ownincs/xem.h"
#include "csource/BBS/BBS_models.h"


#define SYSTEM_TEXTSTYLES (FSF_BOLD | FSF_UNDERLINED | FSF_ITALIC)
#define CURSOR_ON  50
#define CURSOR_OFF 40

#define ATTR_NORMAL		0
#define ATTR_HIGHLIGHT	(1<<4)
#define ATTR_FAT			(1<<5)
#define ATTR_ITALIC		(1<<6)
#define ATTR_UNDERLINE	(1<<7)
#define ATTR_INVERSE		(1<<8)


#define NEWLINE			(1<<0)		/* für stat */
#define X_SCROLLCURSOR	(1<<1)
#define Y_SCROLLCURSOR	(1<<2)
#define BLINK_CURSOR		(1<<3)
#define CURSOR_UNDERSCORE (1<<4)

#define NUL         '\x00'
#define STX         '\x02'
#define BEL         '\x07'
#define BS          '\x08'
#define HT          '\x09'
#define LF          '\x0A'
#define VT          '\x0B'
#define FF          '\x0C'
#define CR          '\x0D'
#define SO          '\x0E'
#define SI          '\x0F'
#define XON         '\x11'
#define XOFF        '\x13'
#define CAN         '\x18'
#define SUB         '\x1A'
#define ESC         '\x1B'
#define CSI         '\x9B'

#define XSIZE			(con->font->tf_XSize)
#define YSIZE			(con->font->tf_YSize)
#define BOTLINE		(con->font->tf_Baseline)
#define ARGUMENTS		16
#define MAXCOLUMNS	200	/* das entspricht einer Auflösung */
#define MAXROWS		200	/* von 1600 x 1600 Pixel bei einem 8'er Font..*/ 

#define MAXARGS 23
#define MAXINTER 10

#define BACKGROUND_PEN 0


struct BBSConsole {
	struct XEM_IO *io;
	struct XEmulatorMacroKey *macrokeys;
	struct RastPort *rp;
	struct TextFont *font;
	struct IBox border;

	struct timerequest *doTime;
	struct MsgPort *timerPort;
	BOOL requestPending;

	BOOL displayInactive;

	BPTR	prt;

	BOOL	inESC;
	BOOL	inCSI;
	BOOL	isReady;		/* TRUE, if XEmulatorOpenConsole() was successful.. */

	BOOL	cursorVisible;
	BOOL	cursorBlocked;

	ULONG argc;

	UWORD lines;
	UWORD columns;
	WORD	top;
	WORD	bot;
	WORD	col,	s_col;
	WORD	row,	s_row;
	UWORD	attr,	s_attr;
	UBYTE	apen,	s_apen;
	UBYTE	bpen,	s_bpen;

	UWORD	foregroundPen;		/* default, don't change! */
	BOOL	hiLite;
	BOOL	mono;

	UWORD	stat, s_stat, r_stat;

	UWORD	ordcol;
	UWORD	ordc;
	UWORD icnt;			/* offset für inter.. */


/* BYTE-Array.. */

	UBYTE	ordtext[MAXCOLUMNS];
	UBYTE	tabs[MAXCOLUMNS];
	UBYTE args[MAXARGS+1];/* in inESC wird der KommandoChar mit abgespeichert */
	UBYTE	inter[MAXINTER+1];		/* intermediates like '#' '?'.. */
	UBYTE shift;
	UBYTE s_shift;
	UBYTE blinkCounter;
	BYTE	libSignal;
};
