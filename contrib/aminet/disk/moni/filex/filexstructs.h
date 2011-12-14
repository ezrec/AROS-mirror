#ifndef FILEXSTRUCTS_H
#define FILEXSTRUCTS_H
      /******************************/
      /*         Strukturen         */
      /******************************/

struct Prefs
{
	BOOL UseAsl, Overwrite;
};

struct DisplayData
{
	struct Node Node;			/* Verkettung innerhalb eines Fensters */
	struct Node FNode;		/* Verkettung innerhalb eines Files */
	struct FileData *FD;
	struct DisplayInhalt *DI;
	struct Window *Wnd;
	
	BYTE DisplayForm, DisplaySpaces;		/* DF_HEXASCII, 2 */
	short Flags;
	long Zeilen;
	WORD BPR;
	short ScrollRand;
	long CPos, MPos, SPos;
	short DisplayTyp;

	UWORD abx, hbx, mbx, bby, sbby, sbx, StatusZeilenBreite;
	struct Gadget *PropGadget;
};

#define DD_HEX	(1L << 0)
#define DD_MARK	(1L << 1)
#define DD_HEXEDITPOS (1L << 2)	/* Falls Hexeingabe:Obere/Untere 4-Bit */

struct DisplayInhalt
{
	struct Node Node;
	struct List DisplayList;
	struct DisplayData *AktuDD;
	struct Window *Wnd;
	ULONG WindowTop, WindowLeft;
	UWORD fhoehe, fbreite, fbase;
	struct TextFont *TextFont;		/* Textfontzeiger oder 0 für Standardfont */
	struct AppWindow *AppWnd;		/* Ggf. Zeiger auf Appwindow */
	struct Requester InvisibleRequest;
};



#define FD_FILE 0
#define FD_GRAB 1

struct FileData
{
	struct Node Node;
	struct List DisplayList;
	UBYTE *Name;
	short Typ;
	ULONG Len, RLen;
	UBYTE *Mem;

	long Changes,RedoChanges,FullChanges;
	struct Undostruct *Undos;
	WORD UndoTiefe;
	WORD UndoStart;
	long UndoMemSize;
	long Locations[ 10 ];
	ULONG LocationNumber;
};

struct WindowData
{
	struct Window *Wnd;
	struct WindowInfo *WI;
	BOOL SchonMalGeoeffnet;
	struct Gadget *GList;
	struct Gadget **Gadgets;
	WORD Left, Top;
	struct MyNewGadget *NG;	
	WORD GadgetZahl;
};

struct ScreenModeData
{
	BOOL smd_Valid;					/* Gültige Werte in der Strucktur? */
	ULONG smd_DisplayID;				/* Display mode ID		       */
	ULONG smd_DisplayWidth;			/* Width of display in pixels       */
	ULONG smd_DisplayHeight;		/* Height of display in pixels      */
	UWORD smd_DisplayDepth;			/* Number of bit-planes of display  */
	UWORD smd_OverscanType;			/* Type of overscan of display      */
	BOOL  smd_AutoScroll;			/* Display should auto-scroll?      */
};

struct Speicher
{
	UBYTE *mem;
	ULONG len;
};

struct Undostruct
{
	UWORD typ;
	UBYTE wert;
	UBYTE neuwert;
	LONG pos;

	struct Speicher u, r;
};

struct MyNewGadget
{
	BYTE Typ;
	BYTE Pos;
	UBYTE Key, Disabled;
	char *Title;
	WORD Min, Max;
	IPTR CurrentValue;
	UBYTE MinHexs, MinChars;
};

      /******************************/
		/*           MACROS           */
		/******************************/

#define GetString( g )      ((( struct StringInfo * )g->SpecialInfo )->Buffer  )
#define GetNumber( g )      ((( struct StringInfo * )g->SpecialInfo )->LongInt )

#define   Max(a,b)    ((a) > (b) ? (a) : (b))

		/******************************/
      /*          DEFINES           */
		/******************************/

#define SCROLLERNEU -1000		/* Wert, um Darstellung neu aufzubauen */
										/* oder den Cursor neu zu setzten */

	/* Werte für MiscSettings */

#define MIN_ALTSPRUNGWEITE 0	/* Minimale Altsprungweite */
#define MAX_ALTSPRUNGWEITE 30	/* Maximale Altsprungweite */
#define DEFAULT_ALTSPRUNGWEITE 7	/* Default */

#define MAX_UNDO_LEVEL 9999
#define MIN_UNDO_LEVEL 2
#define DEFAULT_UNDO_LEVEL	200

#define MIN_UNDO_MEM 10
#define MAX_UNDO_MEM 9999
#define DEFAULT_UNDO_MEM 250

#define MIN_SCROLLRAND 0
#define MAX_SCROLLRAND 99

	/*************************/

#define DEFAULT_ZEILEN 20

#if 0
#define CF_HEX		(1L << 0)		/* Cursor aktiv im Hexfeld */
#define CF_ASCII	(1L << 1)		/* Cursor aktiv in ASCII-Feld */
#define CF_HEXEDITPOS (1L << 2)	/* Falls Hexeingabe:Obere/Untere 4-Bit */
#define CF_MARK	(1L << 3)		/* Wir sind im Markierungsmodus ? */
#endif

#define MF_LEFTMOUSEDOWN	(1L << 0)		/* Linke Maustaste gedrückt */
#define MF_ICONIFY			(1L << 5)		/* Sollen wir uns iconifien? */
#define MF_OVERWRITE			(1L << 6)		/* Files direkt überschreiben? */
#define MF_ICONIFIED			(1L << 7)		/* Sind wir iconified? */
#define MF_ENDE				(1L << 8)		/* FileX soll beendet werden */
#define MF_CALC				(1L << 9)		/* Taschenrechner offen? */
#define MF_CLIPCONV			(1L <<10)		/* ClipConv offen? */
#define MF_LISTREQ			(1L <<11)		/* ListReq offen? */
#define MF_MOVESTATUSBALKEN (1L <<11)		/* Statusbalken wird gezogen */
#define MF_LOCKGUI			(1L <<12)		/* GUI gesperrt? */
#define MF_WDWINDOWOPEN		(1L <<13)		/* Ein WindowData-Fenster offen? */
#define MF_MOVECURSOR		(1L <<14)		/* Wird der Cursor gerade verschoben, d.h. Mausgedrückt innerhalb eines Views */

#define COPYMASK 1			/* Planemaske beim Kopieren im Scrollfenster */

		/* Werte für Screenflags */

#define SF_WORKBENCH			1L /* Workbenchscreen */
#define SF_OWN_PUBLIC		2L	/* Eigener Publicscreen */
#define SF_PUBLIC				3L	/* Publicscreen. Name in `publicscreenname' */
#define SF_DEFAULT_PUBLIC	4L	/* defaul Publicscreen */

#define UT_ASCII	(1L << 1)			/* Asciizeichen an pos */
#define UT_HEX_H	(1L << 2)			/* Highnibble von Hexzeichen an pos */
#define UT_HEX_L	(1L << 3)			/* Lownibble von Hexzeichen an pos */
#define UT_INSERT	(1L << 4)			/* Insert/Append */
#define UT_PASTE	(1L << 5)			/* Paste */
#define UT_CUT		(1L << 6)			/* Cut */

#define SM_CASE		(1L << 0)		/* Case-sensetiv */
#define SM_STRING		(1L << 2)		/* String */
#define SM_REPLACE	(1L << 4)		/* Search and Replace */
#define SM_WILDCARDS	(1L << 5)		/* Suchen mit Mustern */

/* Flags für BM-Routinen */
#define BM_CASE		(1L << 0)		/* Case-sensetiv */
			/* ACHTUNG! WICHTIG!! gleiche Definition in main.c */
#define BM_BACKWARD	(1L << 1)		/* Rückwärts */
#define BM_WILDCARDS	(1L << 2)		/* Mit Wildcards */

#define FM_NUMBER (1L << 0)
#define FM_STRING (1L << 1)

#define WILDCHAR '?'

#define GP_LEFTBOTTOM 1
#define GP_RIGHTBOTTOM 2
#define GP_MIDDLEBOTTOM 3
#define GP_NEWCOLUMN 4

#define HEX_KIND 20

#define DF_HEX 1
#define DF_ASCII 2
#define DF_HEXASCII 3

#define TEXTLINEBUFFERLEN 4096

#define BOXADDX 8		/* Breite einer Bevelbox ohne Inhalt*/
#define BOXADDY 4		/* Höhe einer Bevelbox ohne Inhalt*/

#define DEFAULT_COMMANDSHELLWINDOW "CON:////CommandShell/AUTO/CLOSE/SCREEN FILEX"
#define DEFAULT_COMMANDWINDOW "CON:////ARexx command window/AUTO/CLOSE/WAIT/SCREEN FILEX"

#define GetDDFromFD( x ) ((struct DisplayData *)(((UBYTE *)(x))-sizeof(struct Node)))

#endif /* FILEXSTRUCTS_H */
