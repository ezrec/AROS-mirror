#include "ownincs/xem.h"

#define NUL         '\x00'
#define BS          '\x08'
#define LF          '\x0A'
#define VT          '\x0B'
#define FF          '\x0C'
#define CR          '\x0D'
#define CAN         '\x18'
#define SUB         '\x1A'
#define ESC         '\x1B'
#define CSI         '\x9B'

#define XSIZE			(con->font->tf_XSize)
#define YSIZE			(con->font->tf_YSize)
#define BOTLINE		(con->font->tf_Baseline)
#define ARGUMENTS		16


VOID mySetRast(struct ASCIIConsole *con);
VOID myScrollRaster(struct ASCIIConsole *con, WORD dx, WORD dy, WORD xMin, WORD yMin,	WORD xMax, WORD yMax);
VOID myRectFill(struct ASCIIConsole *con, WORD xMin, WORD yMin, WORD xMax, WORD yMax, UBYTE fgPen);
VOID myMove_Text(struct ASCIIConsole *con, UWORD xPos, UWORD yPos, STRPTR string, UWORD count);


struct ASCIIConsole {
	struct XEM_IO *io;
	struct XEmulatorMacroKey *macrokeys;
	struct RastPort *rp;
	struct TextFont *font;

	struct IBox border;

	BYTE displayInactive;
	BYTE isReady;

	BOOL inESC;
	BOOL inCSI;
	BOOL ASCII;		/* or TTY */

	UWORD foreground_pen;
	UWORD background_pen;
	UWORD lines;
	UWORD columns;
	WORD	col;
	WORD	row;
	WORD	ordcol;
	UWORD	ordc;
	UBYTE	ordText[255];	/* sollte auch in Zukunft reichen..(-: */
	UBYTE	linAttr[255];	/* sollte auch in Zukunft reichen..(-: */
};
