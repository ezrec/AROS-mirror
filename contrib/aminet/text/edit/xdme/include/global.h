/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Defines/structs that mostly all sources depend on.

    $Log$
    Revision 1.1  2001/10/06 20:18:46  digulla
    Initial revision

 * Revision 1.4  1994/12/22  10:01:34  digulla
 * added Makros
 *
 * Revision 1.3  1994/09/09  12:28:04  digulla
 * Moved config struct into config.h
 * added support for title fg and bg pens
 *
 * Revision 1.2  1994/08/19  14:02:34  digulla
 * Renamed Wdisable to Saveicons and added WindowTitles
 *
 * Revision 1.1  1994/08/14  14:26:15  digulla
 * Initial revision
 *

******************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

/**************************************
		Includes
**************************************/


/**************************************
	    Globale Variable
**************************************/


/**************************************
	Defines und Strukturen
**************************************/
typedef LONG			Line;
typedef LONG			Column;

typedef struct Node		NODE;
typedef struct MinNode		MNODE;
typedef struct List		LIST;
typedef struct MinList		MLIST;
typedef struct MsgPort		PORT;
typedef struct Window		WIN;
typedef struct Screen		SCREEN;
typedef struct Message		MSG;
typedef struct TextAttr 	TA;
typedef struct TextFont 	FONT;
typedef struct RastPort 	RP;
typedef struct IntuiMessage	IMESS;
typedef struct IntuitionBase	IBASE;
typedef struct Menu		MENU;
typedef struct MenuItem 	ITEM;
typedef struct IntuiText	ITEXT;
typedef struct Process		PROC;
typedef UBYTE		      * LINE;

/* as long as DAS doesn't align after const strings don't define const */
#define CONST	    const


#if 0
New def. for line:

typedef struct {
     UWORD   flags;	/* flags	    */
     UBYTE   lvl;	/* folding level    */
     VString text;	/* text 	    */
} LINE;
#endif

/* Config */
#include "config.h"

struct GlobalFlags
{
    ULONG _debug : 1;		 /* Do debugging */
    ULONG _NoRequest : 1;	 /* PATCH_NULL: disable requestors		 */
    ULONG _Showtitle : 1;	 /* Used by untitle to disable some title calls  */
    ULONG _Msgchk : 1;		 /* Force message queue check for break 	 */
    ULONG _Windowcycling : 1;	 /* PATCH_NULL : SELECT last SELECT next == SELECT first */
    ULONG _ActivateToFront : 1;  /* PATCH_NULL : SELECTed windows are moved to front	 */
    ULONG _SourceBreaks : 1;	 /* PATCH_NULL : errors in source abort the read	 */
    ULONG _Savetabs : 1;	 /* SaveTabs mode?				 */

    ULONG _SimpleTabs : 1;	 /* PATCH_NULL [25 Jan 1993] : SaveTabs mode only until firstnb ? */
    ULONG _memoryfail : 1;	 /* out of memory flag				 */
    ULONG _Comlinemode : 1;	 /* Is commandline active ?			 */
    ULONG _Abortcommand : 1;	 /* Should loop-command (while/repeat) abort ?   */
    ULONG _global_search : 1;	 /* Do a global search ?			 */
    ULONG _Quitflag : 1;	 /* Should DME close actual window ?		 */
    ULONG _QuitAll : 1; 	 /* Should DME quit ?				 */
    ULONG _Overide : 1; 	 /* Quit even if file was modified		 */

    ULONG _Saveicons : 1;	 /* Save Icons with files			 */
    ULONG _MForceTitle : 1;	 /* Force refresh of title			 */
    ULONG _FollowCursor : 1;	 /* Make sure cursor is visible 		 */
    ULONG _WindowTitles : 1;	 /* PATCH_NULL 16-08-94 Decide to use SetWindowTitles or not */
    ULONG _SLine : 1;		 /* PATCH_NULL 14-06-93 enable movement after EoL */

    ULONG pad : 11;
};

#define GETF_DEBUG(ed)              globalflags._debug
#define SETF_DEBUG(ed,f)            (globalflags._debug = (f))
#define GETF_NOREQUEST(ed)          globalflags._NoRequest
#define SETF_NOREQUEST(ed,f)        (globalflags._NoRequest = (f))
#define GETF_SHOWTITLE(ed)          globalflags._Showtitle
#define SETF_SHOWTITLE(ed,f)        (globalflags._Showtitle = (f))
#define GETF_MSGCHECK(ed)           globalflags._Msgchk
#define SETF_MSGCHECK(ed,f)         (globalflags._Msgchk = (f))
#define GETF_WINDOWCYCLING(ed)      globalflags._Windowcycling
#define SETF_WINDOWCYCLING(ed,f)    (globalflags._Windowcycling = (f))
#define GETF_ACTIVATETOFRONT(ed)    globalflags._ActivateToFront
#define SETF_ACTIVATETOFRONT(ed,f)  (globalflags._ActivateToFront = (f))
#define GETF_SOURCEBREAKS(ed)       globalflags._SourceBreaks
#define SETF_SOURCEBREAKS(ed,f)     (globalflags._SourceBreaks = (f))
#define GETF_SAVETABS(ed)           globalflags._Savetabs
#define SETF_SAVETABS(ed,f)         (globalflags._Savetabs = (f))
#define GETF_SIMPLETABS(ed)         globalflags._SimpleTabs
#define SETF_SIMPLETABS(ed,f)       (globalflags._SimpleTabs = (f))
#define GETF_MEMORYFAIL(ed)         globalflags._memoryfail
#define SETF_MEMORYFAIL(ed,f)       (globalflags._memoryfail = (f))
#define GETF_COMLINEMODE(ed)        globalflags._Comlinemode
#define SETF_COMLINEMODE(ed,f)      (globalflags._Comlinemode = (f))
#define GETF_ABORTCOMMAND(ed)       globalflags._Abortcommand
#define SETF_ABORTCOMMAND(ed,f)     (globalflags._Abortcommand = (f))
#define GETF_GLOBALSEARCH(ed)       globalflags._global_search
#define SETF_GLOBALSEARCH(ed,f)     (globalflags._global_search = (f))
#define GETF_QUITFLAG(ed)           globalflags._Quitflag
#define SETF_QUITFLAG(ed,f)         (globalflags._Quitflag = (f))
#define GETF_QUITALL(ed)            globalflags._QuitAll
#define SETF_QUITALL(ed,f)          (globalflags._QuitAll = (f))
#define GETF_OVERIDE(ed)            globalflags._Overide
#define SETF_OVERIDE(ed,f)          (globalflags._Overide = (f))
#define GETF_SAVEICONS(ed)          globalflags._Saveicons
#define SETF_SAVEICONS(ed,f)        (globalflags._Saveicons = (f))
#define GETF_MFORCETITLE(ed)        globalflags._MForceTitle
#define SETF_MFORCETITLE(ed,f)      (globalflags._MForceTitle = (f))
#define GETF_FOLLOWCURSOR(ed)       globalflags._FollowCursor
#define SETF_FOLLOWCURSOR(ed,f)     (globalflags._FollowCursor = (f))
#define GETF_WINDOWTITLES(ed)       globalflags._WindowTitles
#define SETF_WINDOWTITLES(ed,f)     (globalflags._WindowTitles = (f))
#define GETF_SLINE(ed)              globalflags._SLine
#define SETF_SLINE(ed,f)            (globalflags._SLine = (f))

typedef struct _ED
{
    NODE	    node;	/* for list */
    WIN 	  * win;	/* actual window */
    struct Gadget * propgad;	/* Pointer to our PropGadget */
    FONT	  * font;	/* actual font! */
    Line	    topline;
    Column	    topcolumn;	/* top left edge of window in the text */
    Line	    line;
    Column	    column;	/* actual cursor position */
    long	    lines,
		    maxlines;	/* number of text-lines we use and how much we
				   have at most */
    LINE	  * list;	/* list of text-lines */
    UBYTE	    name[64];
    UBYTE	    wtitle[130];
    ULONG	    _modified : 1;   /* is text modified ? */
    ULONG	    _iconmode : 1;   /* window in icon mode */
    ULONG	    _viewmode : 1;   /* PATCH_NULL: disable changes to text */
    BPTR	    dirlock;	/* directory lock */

    APTR	    textvars;	    /* PATCH_NULL: list of localized vars  */
#define MAX_TEXTFLAGS 32
    char	    textflags[(MAX_TEXTFLAGS+7)/8];
    APTR	    package;	    /* PATCH_NULL: ptr to current Package  */
/* --- I have decided not to use Packages but multiple Menues and Keytables */
    APTR	    menustrip;	    /* PATCH_NULL: ptr to current MenuStrip */
    APTR	    keytable;	    /* PATCH_NULL: ptr to current KeyTable  */

    /*
     *	CONFIG INFORMATION
     */
    struct Config config;

    /* diffs to the above flags */

    ULONG	    df_font : 1;    /* newfont is valid */
    ULONG	    df_topline : 1;
    ULONG	    df_topcolumn : 1;
    ULONG	    df_line : 1;
    ULONG	    df_column : 1;
    ULONG	    df_lines : 1;
    ULONG	    df_title : 1;   /* title has changed */

    ULONG	    df_pad : 25;    /* unused flags */
    FONT	  * newfont;	    /* new font */
    Line	    newtopline;     /* where the text should be */
    Column	    newtopcolumn;
    Line	    newline;	    /* new line/column */
    Column	    newcolumn;
    long	    newlines;	    /* new number of lines */

} ED;


/* config flags */
#define GETF_INSERTMODE(ed)         (ed)->config._insertmode
#define SETF_INSERTMODE(ed,f)       ((ed)->config._insertmode = (f))
#define GETF_IGNORECASE(ed)         (ed)->config._ignorecase
#define SETF_IGNORECASE(ed,f)       ((ed)->config._ignorecase = (f))
#define GETF_WORDWRAP(ed)           (ed)->config._wordwrap
#define SETF_WORDWRAP(ed,f)         ((ed)->config._wordwrap = (f))
#define GETF_AUTOSPLIT(ed)          (ed)->config._autosplit
#define SETF_AUTOSPLIT(ed,f)        ((ed)->config._autosplit = (f))
#define GETF_AUTOINDENT(ed)         (ed)->config._autoindent
#define SETF_AUTOINDENT(ed,f)       ((ed)->config._autoindent = (f))
#define GETF_AUTOUNBLOCK(ed)        (ed)->config._autounblock
#define SETF_AUTOUNBLOCK(ed,f)      ((ed)->config._autounblock = (f))
#define GETF_DOBACK(ed)             (ed)->config._doback
#define SETF_DOBACK(ed,f)           ((ed)->config._doback = (f))
#define GETF_NICEPAGING(ed)         (ed)->config._nicepaging
#define SETF_NICEPAGING(ed,f)       ((ed)->config._nicepaging = (f))
#define GETF_ICONACTIVE(ed)         (ed)->config._iconactive
#define SETF_ICONACTIVE(ed,f)       ((ed)->config._iconactive = (f))
#define GETF_BLOCKENDSFLOAT(ed)     (ed)->config._blockendsfloat
#define SETF_BLOCKENDSFLOAT(ed,f)   ((ed)->config._blockendsfloat = (f))

/* ed flags */
#define GETF_MODIFIED(ed)           (ed)->_modified
#define SETF_MODIFIED(ed,f)         ((ed)->_modified = (f))
#define GETF_ICONMODE(ed)           (ed)->_iconmode
#define SETF_ICONMODE(ed,f)         ((ed)->_iconmode = (f))
#define GETF_VIEWMODE(ed)           (ed)->_viewmode
#define SETF_VIEWMODE(ed,f)         ((ed)->_viewmode = (f))

/* Other ED variables */
#define GetLine(ed)                 ((ed)->line)
#define GetTopline(ed)              ((ed)->topline)
#define GetColumn(ed)               ((ed)->column)
#define GetTopcolumn(ed)            ((ed)->topcolumn)
#define GetLines(ed)                ((ed)->lines)

#define beginconfig	    config
#define CONFIG_SIZE	    (sizeof(struct Config))
#define XDME_CONFIG	    "S:XDME.prefs"


/* Define Colors */
#define TEXT_FPEN(ed)   (ed)->config.fgpen
#define TEXT_BPEN(ed)   (ed)->config.bgpen
#define BLOCK_FPEN(ed)  (ed)->config.hgpen
#define BLOCK_BPEN(ed)  (ed)->config.bbpen
#define TITLE_FPEN(ed)  (ed)->config.tfpen
#define TITLE_BPEN(ed)  (ed)->config.tbpen
#define TEXT_MASK(ed)   (TEXT_FPEN(ed) | TEXT_BPEN(ed))
#define BLOCK_MASK(ed)  (TEXT_MASK(ed) | BLOCK_FPEN(ed) | BLOCK_BPEN(ed))
#define ALL_MASK	0xFF


/* Block-Types
    note:   BSxxx is invalid if BSline < 0,
	    BExxx is invalid if BEline < 0
    If BlockType == BT_NONE, one must not check any other values.
*/
#define BT_NONE     0	    /* No current block */
#define BT_LINE     1	    /* Block is full-line */
#define BT_NORMAL   2	    /* Block starts at BScolumn/BSline and ends at
			       BEcolumn/BEline */
#define BT_VERTICAL 3	    /* Block is a rectangle from BScolumn/BSline to
			       BEcolumn/BEline */
#define BT_HORIZONTAL 4     /* Block is a rectangle from BScolumn/BSline to
			       BEcolumn/BEline */

/* Result of lineflags(line): */
#define LINE_ABOVE	1	/* line < Ep->Topline */
#define LINE_VISIBLE	2	/* line >= Ep->Topline && line < Ep->Topline + Rows */
#define LINE_BELOW	4	/* line >= Ep->Topline + Rows */

/* Where in the block am I actually ?

   BP_INSIDE+BP_START	- Inside 1. line of block
   BP_START		- In the same line in which block starts but not inside
			  block.
   BP_START+END+INSIDE	- I am in the only line the block has.
   BP_START+BP_END	- dito but not INSIDE the block
*/
#define BP_OUTSIDE	    0	    /* No part of block on this line	*/
#define BP_BESIDE	    1	    /* There is some block on this line */
#define BP_INSIDE	    2	    /* Inside block.  We never have	*/
				    /* BP_INSIDE without BP_BESIDE	*/
#define BP_START	    4	    /* line == BSline */
#define BP_END		    8	    /* line == BEline */


typedef struct
{
    ED	 * ep;		    /* the editor the block is in */
    UWORD  type;	    /* BT_xxx */
    UWORD  flags;
    Line   start_line;	    /* start- and endline */
    Line   end_line;
    Column start_column;    /* start- and endcolumn */
    Column end_column;
    Column col[3][2];	    /* First  line  start, First  line	end */
			    /* Middle lines start, Middle lines end */
			    /* Last   line  start, Last   line	end */
} Block;


struct TextMarker
{
    ED	 * ep;		    /* The editor the marker is for */
    Line   line;	    /* line and column */
    Column column;
};


/* Macros */

#define redraw_textline(ed,line)  redraw_textlineseg((line),(ed)->topcolumn,(ed)->topcolumn+Columns)


/**************************************
	       Prototypes
**************************************/


#endif /* GLOBAL_H */

/******************************************************************************
*****  ENDE global.h
******************************************************************************/
