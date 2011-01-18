
/*
 * $Id$
 *
 */

#include "defs.h"

Prototype LIST	    DBase;
Prototype ED	  * Ep;
Prototype struct Config default_config;
Prototype struct GlobalFlags globalflags;
Prototype long	    Nsu;
Prototype UBYTE     CtlC;
Prototype UBYTE     Current[MAXLINELEN];
Prototype UBYTE     Deline[MAXLINELEN];
Prototype UBYTE     Space[32];
Prototype Column    Clen;
Prototype UBYTE   * Partial;
Prototype UBYTE   * esc_partial;
Prototype UBYTE   * String;
Prototype UWORD     ColumnPos[MAXLINELEN];
Prototype UWORD     RowPos[MAXROWS];
Prototype UWORD     Xsize;
Prototype UWORD     Ysize;
Prototype UWORD     XTbase;
Prototype UWORD     YTbase;
Prototype UWORD     Lines;
Prototype UWORD     Columns;
Prototype UWORD     Xbase;
Prototype UWORD     Ybase;
Prototype UWORD     Xpixs;
Prototype UWORD     Ypixs;
Prototype UWORD     LineDistance;
Prototype UBYTE   * av[];
Prototype WORD	    PageJump;
Prototype UBYTE     RexxPortName[8];
Prototype UBYTE     tmp_buffer[MAXLINELEN];
Prototype UWORD     NumClicks;
Prototype ULONG     LoopCont;
Prototype ULONG     LoopBreak;
Prototype ULONG     MacroRecord;

Prototype struct IntuitionBase * IntuitionBase;
Prototype struct GfxBase       * GfxBase;
Prototype struct Library       * IconBase;
Prototype struct Library       * AslBase;
Prototype struct DosLibrary    * DOSBase;
Prototype struct WBStartup     * Wbs;

ED  * Ep;			/* Current Editor			*/
struct Config default_config;	/* Config-struct with default config	*/
LIST  DBase;			/* Doubly linked list of Files		*/

UBYTE	Deline[MAXLINELEN];	/* last deleted line			*/
UBYTE	Current[MAXLINELEN];	/* Current Line buffer and length	*/
UBYTE	Space[32] = { 32,32,32,32, 32,32,32,32, 32,32,32,32, 32,32,32,32,
		      32,32,32,32, 32,32,32,32, 32,32,32,32, 32,32,32,32 };

Column	Clen;			/* Length of actual line */
UBYTE  * Partial;		/* Partial command for communication w/ CF_PAR	*/ /* PATCH_NULL change in function */
UBYTE  * esc_partial;		/* Partial command line when executing ESCIMM	*/ /* PATCH_NULL added */
UBYTE  * String;		/* String Capture variable			*/

UWORD  ColumnPos[MAXLINELEN];	/* Offset to a specific Position on screem */
UWORD  RowPos[MAXROWS];

UWORD  Xsize,  Ysize;		/* font character sizes 		   */
UWORD  Lines,  Columns; 	/* character rows/cols available	   */
UWORD  Xbase,  Ybase;		/* offset pixel base for display	   */
UWORD  XTbase, YTbase;		/* used for text display		   */
UWORD  Xpixs,  Ypixs;		/* actual Right/Bottom pixels. Directly
				   usable in RectFill() or ScrollRaster () */
UWORD  LineDistance = 1;	/* empty space that goes between two lines
				   of text on the screen */

UBYTE * av[8];			/* Argument-array */

struct WBStartup     * Wbs;	/* Startup-Message from WB */

WORD   PageJump;		/* Amount in % for page/prop. gadget */

UBYTE RexxPortName[8];		/* Name of ARexx-Port */

UBYTE tmp_buffer[256];		/* This is a global buffer to prevent unusual
				   stack-usage. DON'T CALL SUBROUTINES WHILE
				   HOLDING IMPORTANT DATA IN IT ! */

UWORD  NumClicks;		/* number of button-presses within double-
				   clicktime */
ULONG LoopCont;
ULONG LoopBreak;
ULONG MacroRecord;

long	Nsu;			/* Used to disable screen updates. If != 0,
				   we must not do any updates */

/* Flags */

struct GlobalFlags globalflags;

UBYTE	CtlC;		/* Keycode for 'c'                              */

