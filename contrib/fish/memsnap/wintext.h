/*
*	wintext.h
*
*	Header for font-independent window-text system, which allows
*	writing of text based on character positions.
*
*	MWS 3/92.
*/

typedef struct wintext {
	struct wintext	*next;		/* next WINTEXT */
	char		*text;		/* actual text to be rendered */
	BYTE		lpos, tpos;	/* character coordinates of 1st char in string */
	UWORD		pen, bg, mode;	/* color and drawmode for text */
} WINTEXT;

typedef struct wintextinfo {
	struct TextAttr tattr;		/* screen's default font - we'll use this */
	struct TextFont *tf;		/* opened font */
	struct Window *window;		/* window this wintextinfo is for */
	UWORD	font_x, font_y;		/* dimensions of default font */
	UWORD	font_baseline;		/* baseline of font */
	BYTE	loffset, toffset,	/* origin (in pixels) for text rendering */
		roffset, boffset;	/* and right and bottom border widths */
} WINTEXTINFO;

BOOL InitWinTextInfo(WINTEXTINFO *);
void WinText(WINTEXTINFO *, char *text, UWORD lpos, UWORD tpos, UWORD pen, UWORD mode);
void RenderWinTexts(WINTEXTINFO *, WINTEXT *);

