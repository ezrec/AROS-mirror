// MJW - THIS INFORMATION IS PUBLIC - should be OK to use anywhere
// MDF - PORT FROM NCSA VERSION 2.1

/****************************************************************************
 * NCSA Mosaic for the X Window System					    *
 * Software Development Group						    *
 * National Center for Supercomputing Applications			    *
 * University of Illinois at Urbana-Champaign				    *
 * 605 E. Springfield, Champaign IL 61820				    *
 * mosaic@ncsa.uiuc.edu 						    *
 *									    *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois	    *
 *									    *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois	    *
 * (UI), and ownership remains with the UI.				    *
 *									    *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such	    *
 * copies.								    *
 *									    *
 * Licensee may make derivative works.	However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then	    *
 * Licensee will (1) notify NCSA regarding its distribution of the	    *
 * derivative work, and (2) clearly notify users that such derivative	    *
 * work is a modified version and not the original NCSA Mosaic		    *
 * distributed by the UI.						    *
 *									    *
 * Any Licensee wishing to make commercial use of the Software should	    *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or	    *
 * part of the source code into a product for sale or license by or on	    *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a	    *
 * commercial product sold or licensed by or on behalf of Licensee.	    *
 *									    *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.						    *
 *									    *
 * By using or copying this Software, Licensee agrees to abide by the	    *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.	    *
 * UI shall have the right to terminate this license immediately by	    *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any	    *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.			    *
 *									    *
 * Comments and questions are welcome and can be sent to		    *
 * mosaic-x@ncsa.uiuc.edu.						    *
 ****************************************************************************/

#ifndef HTML_H
#define HTML_H

#include "XtoI.h"

typedef int (*visitTestProc)();
typedef void (*pointerTrackProc)();

typedef struct ele_ref_rec {
	int id, pos;
} ElementRef;

typedef struct link_rec {
	char *href;
	char *role;
} LinkInfo;


/*
 * Public Structures
 */
typedef struct acall_rec {
//	XEvent *event;
	int element_id;
	char *text;
	char *href;
} WbAnchorCallbackData;


typedef struct fcall_rec {
//	XEvent *event;
	char *href;
	char *method;
	char *enctype;
	int attribute_count;
	char **attribute_names;
	char **attribute_values;
} WbFormCallbackData;


typedef struct formpart_rec {
	struct formpart_rec *Next;
	long Type;
	char *Name; /* Same name as in widget info */
	struct wid_rec *Widget;
	APTR UserData; /* Usually a pointer to the mui object */
} FormPartInfo;


typedef struct form_rec {
	Widget HTML_Data;
	char *action;
	char *method;
	char *enctype;
	int start, end;
	struct form_rec *next;
	struct wid_rec *FirstWidget;
	struct formpart_rec *FirstFormPart;
	char *href;
} FormInfo;

typedef struct image_rec {
	int ismap;
	FormInfo *fptr;
	int internal;
	int delayed;
	int fetched;
	int width, height;
	int num_colors;
	int *reds;
	int *greens;
	int *blues;
	unsigned char *image_data;
	Pixmap image;
	char *text;
	APTR dt;		/* mdf */
	char *fname;		/* mdf */
	ULONG *colors;		/* mdf */
	struct BitMap *mask_bitmap;	/* sb */
} ImageInfo;


typedef struct wid_rec {
	Widget w;
	int type;
	int id;
	int x, y;
	int width, height;
	char *name;
	char *value;
	char *password;
	Boolean checked;
	Boolean mapped;
	Object *MUI_Object;
	struct wid_rec *next;
	FormInfo *ParentForm;
	Object *MUI_SubObject;
	char *option;
	struct wid_rec *NextRadio,*PrevRadio;
	ULONG flags;
	char *returnv;
} WidgetInfo;

typedef struct struct_rec
{
	long	type;
	Object	*obj;
	Object	*parrent;
	struct struct_rec *next;
} StructureInfo;

typedef struct sel_rec {
	Widget HTML_Data;
	struct mark_up *mptr;
	int is_value;
	char *retval_buf;
	char *option_buf;
	char **returns;
	char **options;
	int option_cnt;
	char **value;
	int value_cnt;
} SelectInfo;


typedef ImageInfo *(*resolveImageProc)();


/*
 * defines and structures used for the formatted element list
 */

#define E_TEXT		1
#define E_BULLET	2
#define E_LINEFEED	3
#define E_IMAGE 	4
#define E_WIDGET	5
#define E_HRULE 	6

struct ele_rec {
	int type;
	ImageInfo *pic_data;
	WidgetInfo *widget_data;
	XFontStruct *font;
	int style;		/* MDF - For Amiga SetSoftStyle() hack */
	int alignment;
	Boolean internal;
	Boolean selected;
	int indent_level;
	int start_pos, end_pos;
	int x, y;
	int y_offset;
	int width;
	int line_number;
	int line_height;
	int ele_id;
	int underline_number;
	Boolean dashed_underline;
	Boolean strikeout;
	unsigned long fg;
	unsigned long bg;
	char *anchorName;
	char *anchorHRef;
	char *edata;
	int edata_len;
	struct ele_rec *next;
	struct ele_rec *prev;
};

struct ref_rec {
	char *anchorHRef;
	struct ref_rec *next;
};

struct delay_rec {
	char *src;
	struct delay_rec *next;
};


/*
 * defines and structures used for the HTML parser, and the
 * parsed object list.
 */

/* Mark types */
#define M_UNKNOWN	-1
#define M_NONE		0
#define M_TITLE 	1
#define M_HEADER_1	2
#define M_HEADER_2	3
#define M_HEADER_3	4
#define M_HEADER_4	5
#define M_HEADER_5	6
#define M_HEADER_6	7
#define M_ANCHOR	8
#define M_PARAGRAPH	9
#define M_ADDRESS	10
#define M_PLAIN_TEXT	11
#define M_UNUM_LIST	12
#define M_NUM_LIST	13
#define M_LIST_ITEM	14
#define M_DESC_LIST	15
#define M_DESC_TITLE	16
#define M_DESC_TEXT	17
#define M_PREFORMAT	18
#define M_PLAIN_FILE	19
#define M_LISTING_TEXT	20
#define M_INDEX 	21
#define M_MENU		22
#define M_DIRECTORY	23
#define M_IMAGE 	24
#define M_FIXED 	25
#define M_BOLD		26
#define M_ITALIC	27
#define M_EMPHASIZED	28
#define M_STRONG	29
#define M_CODE		30
#define M_SAMPLE	31
#define M_KEYBOARD	32
#define M_VARIABLE	33
#define M_CITATION	34
#define M_BLOCKQUOTE	35
#define M_STRIKEOUT	36
#define M_INPUT 	37
#define M_FORM		38
#define M_HRULE 	39
#define M_LINEBREAK	40
#define M_BASE		41
#define M_SELECT	42
#define M_OPTION	43
#define M_TEXTAREA	44

/* syntax of Mark types */
#define MT_TITLE	"title"
#define MT_HEADER_1	"h1"
#define MT_HEADER_2	"h2"
#define MT_HEADER_3	"h3"
#define MT_HEADER_4	"h4"
#define MT_HEADER_5	"h5"
#define MT_HEADER_6	"h6"
#define MT_ANCHOR	"a"
#define MT_PARAGRAPH	"p"
#define MT_ADDRESS	"address"
#define MT_PLAIN_TEXT	"xmp"
#define MT_UNUM_LIST	"ul"
#define MT_NUM_LIST	"ol"
#define MT_LIST_ITEM	"li"
#define MT_DESC_LIST	"dl"
#define MT_DESC_TITLE	"dt"
#define MT_DESC_TEXT	"dd"
#define MT_PREFORMAT	"pre"
#define MT_PLAIN_FILE	"plaintext"
#define MT_LISTING_TEXT "listing"
#define MT_INDEX	"isindex"
#define MT_MENU 	"menu"
#define MT_DIRECTORY	"dir"
#define MT_IMAGE	"img"
#define MT_FIXED	"tt"
#define MT_BOLD 	"b"
#define MT_ITALIC	"i"
#define MT_EMPHASIZED	"em"
#define MT_STRONG	"strong"
#define MT_CODE 	"code"
#define MT_SAMPLE	"samp"
#define MT_KEYBOARD	"kbd"
#define MT_VARIABLE	"var"
#define MT_CITATION	"cite"
#define MT_BLOCKQUOTE	"blockquote"
#define MT_STRIKEOUT	"strike"
#define MT_INPUT	"input"
#define MT_FORM 	"form"
#define MT_HRULE	"hr"
#define MT_LINEBREAK	"br"
#define MT_BASE 	"base"
#define MT_SELECT	"select"
#define MT_OPTION	"option"
#define MT_TEXTAREA	"textarea"


/* anchor tags */
#define AT_NAME 	"name"
#define AT_HREF 	"href"


struct mark_up {
	int type;
	int is_end;
	char *start;
	char *text;
	char *end;
	struct mark_up *next;
};

/* HTML Gaget internal data...
    must only be used by files that are called from the gadget itself */
/* Class data for the HTML Gadget */

/* I had to include this to be able to straighten out some serious bugs      *
 * I want to replace the 'Widget' references everywhere with a HTMLGadClData *
 * because that is what it is!						     */
typedef struct HTMLGadClDataS
{
  /* Amiga-specific part */

  struct Window *win;
  struct RastPort *rp;
  char *inlined_image_to_load;	/* temporary hack? */
  char *chosen_anchor;		/* temporary hack? */
  int active;			/* hack? */

  /* Resources */
  Dimension			margin_width;
  Dimension			margin_height;

  Widget			view;
  Widget			hbar;
  Widget			vbar;
  Boolean			hbar_top;
  Boolean			vbar_right;

  XtCallbackList	anchor_callback;
  XtCallbackList	link_callback;
  XtCallbackList	form_callback;

  char				*title;
  char				*raw_text;
  char				*header_text;
  char				*footer_text;
  /*
   * Without motif we have to define our own forground resource
   * instead of using the manager's
   */
#ifndef MOTIF
  Pixel 			foreground;
  Pixel 		background_pixel; /* MDF added; "core" class */
#endif
  Pixel 			anchor_fg;
  Pixel 			visitedAnchor_fg;
  Pixel 			activeAnchor_fg;
  Pixel 			activeAnchor_bg;
  int				num_anchor_underlines;
  int				num_visitedAnchor_underlines;
  Boolean			dashed_anchor_lines;
  Boolean			dashed_visitedAnchor_lines;
  Boolean			fancy_selections;
  Boolean			border_images;
  Boolean			delay_images;
  Boolean			is_index;
  int				percent_vert_space;

  XFontStruct		*font;
  XFontStruct		*italic_font;
  XFontStruct		*bold_font;
  XFontStruct		*fixed_font;
  XFontStruct		*header1_font;
  XFontStruct		*header2_font;
  XFontStruct		*header3_font;
  XFontStruct		*header4_font;
  XFontStruct		*header5_font;
  XFontStruct		*header6_font;
  XFontStruct		*address_font;
  XFontStruct		*plain_font;
  XFontStruct		*listing_font;

  /*
    int (*previously_visited_test)();
    ImageInfo * (*resolveImage)();
    */
  XtPointer			previously_visited_test;
  XtPointer			resolveImage;
  XtPointer			resolveDelayedImage;

  XtPointer		  pointer_motion_callback;

  /* PRIVATE */
  Dimension			max_pre_width;
  Dimension		view_left; /* MDF temp hack? */
  Dimension		view_top;  /* MDF temp hack? */
  Dimension			view_width;
  Dimension			view_height;
  int				doc_width;
  int				doc_height;
  int				scroll_x;
  int				scroll_y;
  Boolean			use_hbar;
  Boolean			use_vbar;
  struct ele_rec	*formatted_elements;
  int				line_count;
  struct ele_rec	**line_array;
  struct ele_rec	*select_start;
  struct ele_rec	*select_end;
  int				sel_start_pos;
  int				sel_end_pos;
  struct ele_rec	*new_start;
  struct ele_rec	*new_end;
  int				new_start_pos;
  int				new_end_pos;
  struct ele_rec	*active_anchor;
  //	GC			drawGC;
  void *			drawGC;
  int				press_x;
  int				press_y;
  Time				but_press_time;
  Time				selection_time;
  struct mark_up	*html_objects;
  struct mark_up	*html_header_objects;
  struct mark_up	*html_footer_objects;
  struct ref_rec	*my_visited_hrefs;
  struct delay_rec	*my_delayed_images;
	WidgetInfo		*widget_list;
  FormInfo			*form_list;

  struct ele_rec	  *cached_tracked_ele;

  int				new_scroll_value;
  int				new_scroll_dir;
	Object			*SB_Vert,*SB_Horiz;
	Object			*Virt;
	LONG			new_scroll_y;
	WidgetInfo		*new_widget_list;
	ULONG			reparsing;
	ULONG			object_flags;			/*	*/
	ULONG			have_forms;				/* BOOL */
	char			*post_text;				/* The text that should be sent to a cgi script. */
	char			*post_href;				/* The location to send the text to. */
	BOOL			keyctrl;
	BOOL			have_formsI;
  Pixel 			anchor_fgc;
  Pixel 			visitedAnchor_fgc;
  Pixel 			activeAnchor_fgc;
} HTMLGadClData; /* Was HTMLRec */

#define Object_Parsing	 ((ULONG)0x01) /* Starting from scratch with a window */
#define Object_Reparsing ((ULONG)0x02) /* Modifying an old window */
#define Object_Changing  ((ULONG)0x04) /* Has done an InitChange or closed the window */
/*
 * Public functions
 */
#ifdef _NO_PROTO
extern char *HTMLGetText ();
extern char *HTMLGetTextAndSelection ();
extern char **HTMLGetHRefs ();
extern char **HTMLGetImageSrcs ();
extern void *HTMLGetWidgetInfo ();
extern void HTMLFreeWidgetInfo ();
extern void HTMLFreeImageInfo ();
extern LinkInfo *HTMLGetLinks ();
extern int HTMLPositionToId ();
extern int HTMLIdToPosition ();
extern int HTMLAnchorToPosition ();
extern int HTMLAnchorToId ();
extern void HTMLGotoId ();
extern void HTMLRetestAnchors ();
extern void HTMLClearSelection ();
extern void HTMLSetSelection ();
extern void HTMLSetText ();
extern int HTMLSearchText ();
#else
extern char *HTMLGetText (HTMLGadClData *w, int pretty);
extern char *HTMLGetTextAndSelection (Widget w, char **startp, char **endp,
					char **insertp);
extern char **HTMLGetHRefs (Widget w, int *num_hrefs);
extern char **HTMLGetImageSrcs (HTMLGadClData *w, int *num_srcs);
extern void *HTMLGetWidgetInfo (HTMLGadClData *w);
extern void HTMLFreeWidgetInfo (void *ptr);
extern void HTMLFreeImageInfo (Widget w);
extern LinkInfo *HTMLGetLinks (Widget w, int *num_links);
extern int HTMLPositionToId(HTMLGadClData *w, int x, int y);
extern int HTMLIdToPosition(Widget w, int element_id, int *x, int *y);
extern int HTMLAnchorToPosition(Widget w, char *name, int *x, int *y);
extern int HTMLAnchorToId(Widget w, char *name);
extern void HTMLGotoId(HTMLGadClData *w, int element_id);
extern void HTMLRetestAnchors(Widget w, visitTestProc testFunc);
extern void HTMLClearSelection (Widget w);
extern void HTMLSetSelection (Widget w, ElementRef *start, ElementRef *end);
extern void HTMLSetText (HTMLGadClData *w, char *text, char *header_text,
			char *footer_text, int element_id,
			char *target_anchor, void *ptr);
extern int HTMLSearchText (HTMLGadClData *HTML_Data, char *pattern,
	ElementRef *m_start, ElementRef *m_end, int backward, int caseless);
#endif /* _NO_PROTO */


/*
 * New resource names
 */
#define WbNmarginWidth		"marginWidth"
#define WbNmarginHeight 	"marginHeight"
#define WbNtext 		"text"
#define WbNheaderText		"headerText"
#define WbNfooterText		"footerText"
#define WbNtitleText		"titleText"
#define WbNanchorUnderlines	"anchorUnderlines"
#define WbNvisitedAnchorUnderlines	"visitedAnchorUnderlines"
#define WbNdashedAnchorUnderlines	"dashedAnchorUnderlines"
#define WbNdashedVisitedAnchorUnderlines	"dashedVisitedAnchorUnderlines"
#define WbNanchorColor		"anchorColor"
#define WbNvisitedAnchorColor	"visitedAnchorColor"
#define WbNactiveAnchorFG	"activeAnchorFG"
#define WbNactiveAnchorBG	"activeAnchorBG"
#define WbNfancySelections	"fancySelections"
#define WbNimageBorders 	"imageBorders"
#define WbNdelayImageLoads	"delayImageLoads"
#define WbNisIndex		"isIndex"
#define WbNitalicFont		"italicFont"
#define WbNboldFont		"boldFont"
#define WbNfixedFont		"fixedFont"
#define WbNheader1Font		"header1Font"
#define WbNheader2Font		"header2Font"
#define WbNheader3Font		"header3Font"
#define WbNheader4Font		"header4Font"
#define WbNheader5Font		"header5Font"
#define WbNheader6Font		"header6Font"
#define WbNaddressFont		"addressFont"
#define WbNplainFont		"plainFont"
#define WbNlistingFont		"listingFont"
#define WbNanchorCallback	"anchorCallback"
#define WbNlinkCallback 	"linkCallback"
#define WbNsubmitFormCallback	"submitFormCallback"
#define WbNpreviouslyVisitedTestFunction "previouslyVisitedTestFunction"
#define WbNresolveImageFunction "resolveImageFunction"
#define WbNresolveDelayedImage "resolveDelayedImage"
#define WbNpercentVerticalSpace "percentVerticalSpace"
#define WbNpointerMotionCallback "pointerMotionCallback"
#define WbNverticalScrollOnRight "verticalScrollOnRight"
#define WbNhorizontalScrollOnTop "horizontalScrollOnTop"
#define WbNview 		 "view"
#define WbNverticalScrollBar	 "verticalScrollBar"
#define WbNhorizontalScrollBar	 "horizontalScrollBar"

/*
 * New resource classes
 */
#define WbCMarginWidth		"MarginWidth"
#define WbCMarginHeight 	"MarginHeight"
#define WbCText 		"Text"
#define WbCHeaderText		"HeaderText"
#define WbCFooterText		"FooterText"
#define WbCTitleText		"TitleText"
#define WbCAnchorUnderlines	"AnchorUnderlines"
#define WbCVisitedAnchorUnderlines	"VisitedAnchorUnderlines"
#define WbCDashedAnchorUnderlines	"DashedAnchorUnderlines"
#define WbCDashedVisitedAnchorUnderlines	"DashedVisitedAnchorUnderlines"
#define WbCAnchorColor		"AnchorColor"
#define WbCVisitedAnchorColor	"VisitedAnchorColor"
#define WbCActiveAnchorFG	"ActiveAnchorFG"
#define WbCActiveAnchorBG	"ActiveAnchorBG"
#define WbCFancySelections	"FancySelections"
#define WbCImageBorders 	"ImageBorders"
#define WbCDelayImageLoads	"DelayImageLoads"
#define WbCIsIndex		"IsIndex"
#define WbCItalicFont		"ItalicFont"
#define WbCBoldFont		"BoldFont"
#define WbCFixedFont		"FixedFont"
#define WbCHeader1Font		"Header1Font"
#define WbCHeader2Font		"Header2Font"
#define WbCHeader3Font		"Header3Font"
#define WbCHeader4Font		"Header4Font"
#define WbCHeader5Font		"Header5Font"
#define WbCHeader6Font		"Header6Font"
#define WbCAddressFont		"AddressFont"
#define WbCPlainFont		"PlainFont"
#define WbCListingFont		"ListingFont"
#define WbCPreviouslyVisitedTestFunction "PreviouslyVisitedTestFunction"
#define WbCResolveImageFunction "ResolveImageFunction"
#define WbCResolveDelayedImage "ResolveDelayedImage"
#define WbCPercentVerticalSpace "PercentVerticalSpace"
#define WbCPointerMotionCallback "PointerMotionCallback"
#define WbCVerticalScrollOnRight "VerticalScrollOnRight"
#define WbCHorizontalScrollOnTop "HorizontalScrollOnTop"
#define WbCView 		 "View"
#define WbCVerticalScrollBar	 "VerticalScrollBar"
#define WbCHorizontalScrollBar	 "HorizontalScrollBar"

//typedef Class  *HTMLWidgetClass;
typedef Object *HTMLWidget;

#endif /* HTML_H */
