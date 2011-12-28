// MDF - PORT FROM NCSA VERSION 2.1

/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

#if defined(_AMIGA) || defined(__AROS__)
#include "includes.h"
#include <exec/types.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <intuition/intuitionbase.h>
#include <clib/graphics_protos.h>
#include <graphics/gfxmacros.h>
#include "XtoI.h"
#include "globals.h"
#include "htmlgad.h"
#include "HTML.h"

void DisposeForms(FormInfo *Form);

#endif /* _AMIGA */

#include <stdio.h>
/* nothing in here can be used outside the widget */
#include "HTMLP.h"

/*
#ifdef MOTIF
#include <Xm/DrawingA.h>
#include <Xm/ScrollBar.h>
#else
#include "DrawingArea.h"
#include <X11/Xaw/Scrollbar.h>
#endif
#include <X11/cursorfont.h>
*/

#define	MARGIN_DEFAULT		20
#define	CLICK_TIME		500
#define	SELECT_THRESHOLD	3
#define	MAX_UNDERLINES		3
#define DEFAULT_INCREMENT       18

#ifndef ABS
#define ABS(x)  (((x) > 0) ? (x) : ((x) * -1))
#endif

#define	W_TEXTFIELD	0
#define	W_CHECKBOX	1
#define	W_RADIOBOX	2
#define	W_PUSHBUTTON	3
#define	W_PASSWORD	4
#define	W_OPTIONMENU	5


extern struct Region *installClipRegion(struct Window *, struct Layer *,
					struct Region *);
extern int FormatAll(HTMLGadClData *, int *);
extern int DocumentWidth(HTMLGadClData *, struct mark_up *);
extern void PlaceLine(HTMLGadClData *, int);
extern void TextRefresh(HTMLGadClData *, struct ele_rec *, int, int);
extern void ImageRefresh(HTMLGadClData *, struct ele_rec *);
extern void LinefeedRefresh(HTMLGadClData *, struct ele_rec *);
extern void BulletRefresh(HTMLGadClData *, struct ele_rec *);
extern void RefreshTextRange(HTMLGadClData *, struct ele_rec *, struct ele_rec *);
extern void FreeColors(void *, void *);
extern void FreeImages(HTMLGadClData *);
extern void HideWidgets();
extern void MapWidgets();
extern int SwapElements(struct ele_rec *, struct ele_rec *, int, int);
extern int ElementLessThan(struct ele_rec *, struct ele_rec *, int, int);
extern int IsDelayedHRef(HTMLGadClData *, char *);
extern int IsIsMapForm(HTMLGadClData *, char *);
extern int AnchoredHeight(HTMLGadClData *);
extern char *ParseMarkTag(char *, char *, char *);
extern char *ParseTextToString(struct ele_rec *, struct ele_rec *,
			       struct ele_rec *, int, int, int, int);
extern char *ParseTextToPrettyString(HTMLGadClData *, struct ele_rec *,
				     struct ele_rec *, struct ele_rec *,
				     int, int, int, int);
extern char *ParseTextToPSString();
extern struct mark_up *HTMLParse(struct mark_up *, char *);
extern struct ele_rec *LocateElement(HTMLGadClData *, int, int, int *);
extern struct ele_rec **MakeLineList(struct ele_rec *, int);
extern void FreeHRefs(struct ref_rec *);
extern struct ref_rec *AddHRef();
extern void FreeDelayedImages(struct delay_rec *);
extern struct delay_rec *AddDelayedImage();
extern ImageInfo *NoImageData();
extern void ImageSubmitForm();
extern void AddNewWidgets(HTMLGadClData *);
extern void HideWidgets(HTMLGadClData *);
static void CallLinkCallbacks(HTMLGadClData *);
void ViewRedisplay(HTMLGadClData *, int, int, int, int);

#if 0
//static Cursor in_anchor_cursor = NULL;
static void *in_anchor_cursor = NULL;
#endif

/*
 * Either the vertical or hortizontal scrollbar has been moved
 */
void
ScrollToPos(HTMLGadClData * HTML_Data, int bar, int value)
//ScrollToPos(Widget w, HTMLGadClData * HTML_Data, int value)
{
	/*
	 * If we've moved the vertical scrollbar
	 */
	if (bar == 0)
	{
		/*
		 * We've scrolled down. Copy up the untouched part of the
		 * window.  Then Clear and redraw the new area
		 * exposed.
		 */
		if (value > HTML_Data->scroll_y)
		{
			int dy;
	    
			dy = value - HTML_Data->scroll_y;
			if (dy > HTML_Data->view_height){
				HTML_Data->scroll_y = value;
				XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width,HTML_Data->view_height, False);
				ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width,HTML_Data->view_height);
			}
			else{
				XCopyArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),XtWindow(HTML_Data->view),HTML_Data->drawGC, 0, dy,HTML_Data->view_width,HTML_Data->view_height - dy,0, 0);
				HTML_Data->scroll_y = value;
				XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, (int)HTML_Data->view_height - dy,HTML_Data->view_width, dy, False);
				ViewRedisplay(HTML_Data,0, (int)HTML_Data->view_height - dy,HTML_Data->view_width, dy);
			}
		}
		/*
		 * We've scrolled up. Copy down the untouched part of the
		 * window.  Then Clear and redraw the new area
		 * exposed.
		 */
		else if (value < HTML_Data->scroll_y)
		{
			int dy;
	    
			dy = HTML_Data->scroll_y - value;
			if (dy > HTML_Data->view_height){
				HTML_Data->scroll_y = value;
				XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width,HTML_Data->view_height, False);
				ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width,HTML_Data->view_height);
			}
			else{
				XCopyArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),XtWindow(HTML_Data->view),HTML_Data->drawGC, 0, 0,HTML_Data->view_width,HTML_Data->view_height - dy,0, dy);
				HTML_Data->scroll_y = value;
				XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width, dy, False);
				ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width, dy);
			}
		}
	}
	/*
	 * Else we've moved the horizontal scrollbar
	 */
	else if (bar == 1)
	{
		/*
		 * We've scrolled right. Copy left the untouched part of the
		 * window.  Then Clear and redraw the new area
		 * exposed.
		 */
		if (value > HTML_Data->scroll_x){
			int dx;
	    
			dx = value - HTML_Data->scroll_x;
			if (dx > HTML_Data->view_width){
				HTML_Data->scroll_x = value;
				XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width,HTML_Data->view_height, False);
				ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width,HTML_Data->view_height);
			}
			else{
				XCopyArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),XtWindow(HTML_Data->view),HTML_Data->drawGC, dx, 0,HTML_Data->view_width - dx,HTML_Data->view_height,0, 0);
				HTML_Data->scroll_x = value;
				XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),(int)HTML_Data->view_width - dx, 0,dx, HTML_Data->view_height, False);
				ViewRedisplay(HTML_Data,(int)HTML_Data->view_width - dx, 0,dx, HTML_Data->view_height);
			}
		}
		/*
		 * We've scrolled left. Copy right the untouched part of the
		 * window.  Then Clear and redraw the new area
		 * exposed.
		 */
		else if (value < HTML_Data->scroll_x)
		{
			int dx;
	    
			dx = HTML_Data->scroll_x - value;
			if (dx > HTML_Data->view_width){
				HTML_Data->scroll_x = value;
				XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width,HTML_Data->view_height, False);
				ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width,HTML_Data->view_height);
			}
			else{
				XCopyArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),XtWindow(HTML_Data->view),HTML_Data->drawGC, 0, 0,HTML_Data->view_width - dx,HTML_Data->view_height,dx, 0);
				HTML_Data->scroll_x = value;
				XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,dx, HTML_Data->view_height, False);
				ViewRedisplay(HTML_Data,0, 0,dx, HTML_Data->view_height);
			}
		}
	}

}

/*
 * Reformat the window and ( scrollbars ).
 * May be called because of a changed document, or because of a changed
 * window size.
 */
void
ReformatWindow(HTMLGadClData * HTML_Data)
{
	int temp;
	int new_width;
//	Dimension swidth, sheight;
//	Dimension st;

	/*
	 * Find the current scrollbar sizes, and shadow thickness and format
	 * the document to the current window width
	 * (assume a vertical scrollbar)
	 */
	new_width = HTML_Data->view_width;
	temp = FormatAll(HTML_Data, &new_width);

	/*
	 * If we need the vertical scrollbar, place and manage it,
	 * and store the current viewing area width.
	 */

	/*
	 * Calculate the actual max width and height of the complete
	 * formatted document.
	 * The max width may exceed the preformatted width due to special
	 * factors in the formatting of the widget.
	 * Use the max of the 2 here, but leave max_pre_width unchanged
	 * for future formatting calls.
	 */
	/*
	 * new_width includes the margins, and HTML_Data->max_pre_width
	 * does not, fix that here.
	 */
	new_width = new_width - (2 * HTML_Data->margin_width);
	if (HTML_Data->max_pre_width > new_width)
	{
		new_width = HTML_Data->max_pre_width;
	}
	/*
	 * If the maximum width derives from a formatted, as opposed to
	 * unformatted piece of text, allow a 20% of margin width slop
	 * over into the margin to cover up a minor glick with terminaing
	 * punctuation after anchors at the end of the line.
	 */
	else
	{
		new_width = new_width - (20 * HTML_Data->margin_width / 100);
	}

	HTML_Data->doc_height = temp;
	HTML_Data->doc_width = new_width + (2 * HTML_Data->margin_width);
	if (HTML_Data->view_width > HTML_Data->doc_width)
	{
		HTML_Data->doc_width = HTML_Data->view_width;
	}

}

#if 0
/*
 * Initialize is called when the widget is first initialized.
 * Check to see that all the starting resources are valid.
 */
static void
Initialize(HTMLGadClData * request, HTMLGadClData * new)
{
	/*
	 *	Make sure the underline numbers are within bounds.
	 */
	if (new->num_anchor_underlines < 0)
	{
		new->num_anchor_underlines = 0;
	} 
	if (new->num_anchor_underlines > MAX_UNDERLINES)
	{
		new->num_anchor_underlines = MAX_UNDERLINES;
	} 
	if (new->num_visitedAnchor_underlines < 0)
	{
		new->num_visitedAnchor_underlines = 0;
	} 
	if (new->num_visitedAnchor_underlines > MAX_UNDERLINES)
	{
		new->num_visitedAnchor_underlines = MAX_UNDERLINES;
	} 

	/*
	 * Parse the raw text with the HTML parser.  And set the formatted 
	 * element list to NULL.
	 */
	new->html_objects = HTMLParse(NULL, request->raw_text);
	CallLinkCallbacks(new);
	new->html_header_objects =
		HTMLParse(NULL, request->header_text);
	new->html_footer_objects =
		HTMLParse(NULL, request->footer_text);
	new->formatted_elements = NULL;
	new->my_visited_hrefs = NULL;
	new->my_delayed_images = NULL;
	new->widget_list = NULL;
	new->form_list = NULL;

	/*
	 * Blank document
	 */
	new->line_array = NULL;
	new->line_count = 0;

	/*
	 * Find the max width of a preformatted
	 * line in this document.
	 */
	new->max_pre_width = DocumentWidth(new, new->html_objects);

	/*
	 * Create the scrollbars.
	 * Find their dimensions and then decide which scrollbars you
	 * will need, and what the dimensions of the viewing area are.
	 * Start assuming a vertical scrollbar and a horizontal one.
	 * The remove vertical if short enough, and remove horizontal
	 * if narrow enough.
	 */
//	CreateScrollbars(new);
	new->scroll_x = 0;
	new->scroll_y = 0;
	ReformatWindow(new);

	/*
	 * Initialize private widget resources
	 */
	new->drawGC = NULL;
	new->select_start = NULL;
	new->select_end = NULL;
	new->sel_start_pos = 0;
	new->sel_end_pos = 0;
	new->new_start = NULL;
	new->new_end = NULL;
	new->new_start_pos = 0;
	new->new_end_pos = 0;
	new->active_anchor = NULL;
	new->press_x = 0;
	new->press_y = 0;

        new->cached_tracked_ele = NULL;

        /* Initialize cursor used when pointer is inside anchor. */
//        if (in_anchor_cursor == NULL)
//          in_anchor_cursor = XCreateFontCursor (XtDisplay (new), XC_hand2);

        return;
}
#endif


#ifdef DEBUG
void
DebugHook(x, y, width, height)
	int x, y, width, height;
{
fprintf(stderr, "Redrawing (%d,%d) %dx%d\n", x, y, width, height);
}
#endif


/*
 * This is called by redisplay.  It is passed a rectangle
 * in the viewing area, and it redisplays that portion of the
 * underlying document area.
 */
void
ViewRedisplay(
            HTMLGadClData * HTML_Data,
            int x,
            int y,
            int width,
            int height)
{
	int sx, sy;
//mjw	int doc_x;
	int doc_y;
	int i, start, end, guess;

	struct Region *new, *old = NULL;
	struct Rectangle rect;

	/*
	 * Use scrollbar values to map from view space to document space.
	 */
	sx = sy = 0;
	if (HTML_Data->use_vbar == True)
	{
		sy += HTML_Data->scroll_y;
	}
	if (HTML_Data->use_hbar == True)
	{
		sx += HTML_Data->scroll_x;
	}

//mjw	doc_x = x + sx;
	doc_y = y + sy;

	/*
	 * Find the lines that overlap the exposed area.
	 */
	start = 0;
	end = HTML_Data->line_count - 1;

	/*
	 * Heuristic to speed up redraws by guessing at the starting line.
	 */
	guess = doc_y / (HTML_Data->font->tf_YSize);
	if (guess > end)
	{
		guess = end;
	}
	while (guess > 0)
	{
		if ((HTML_Data->line_array[guess] != NULL)&&
			(HTML_Data->line_array[guess]->y < doc_y))
		{
			break;
		}
		guess--;
	}
	if (guess < start)
	{
		guess = start;
	}

	for (i=guess; i<HTML_Data->line_count; i++)
	{
		if (HTML_Data->line_array[i] == NULL)
		{
			continue;
		}

		if (HTML_Data->line_array[i]->y < doc_y)
		{
			start = i;
		}
		if (HTML_Data->line_array[i]->y > (doc_y + height))
		{
			end = i;
			break;
		}
	}

	if (HTML_Data->rp != NULL){
		/*
		 * If we have a GC draw the lines that overlap the exposed area.
		 */
		if(!HTML_Data->have_forms){
			rect.MinX = HTML_Data->view_left;
			rect.MinY = HTML_Data->view_top;
			rect.MaxX = HTML_Data->view_left + HTML_Data->view_width;
			rect.MaxY = HTML_Data->view_top  + HTML_Data->view_height - 1;

/* sb: VERY ugly! Will rewrite it, later */

			new = NewRegion();
			OrRectRegion(new, &rect);

			old = installClipRegion(HTML_Data->win, HTML_Data->win->WLayer, new);
		}
		for (i=start; i<=end; i++)
		{
			PlaceLine(HTML_Data, i);
		}

		if(!HTML_Data->have_forms){
			new = installClipRegion(HTML_Data->win, HTML_Data->win->WLayer, old);
			DisposeRegion(new);
			}
	  SetDrPt(HTML_Data->rp, 0xFFFF);
	}
}


void
ViewClearAndRefresh(HTMLGadClData * HTML_Data)
{
	/*
	 * Only refresh if we have a window already.
	 * (if we have a GC we have a window)
	 */
	if (HTML_Data->rp != NULL){
		SetAPen(HTML_Data->rp, 0);
		RectFill(HTML_Data->rp, HTML_Data->view_left, HTML_Data->view_top,HTML_Data->view_left + HTML_Data->view_width,HTML_Data->view_top + HTML_Data->view_height);
		SetAPen(HTML_Data->rp, 1);
		ViewRedisplay(HTML_Data, 0, 0,
			HTML_Data->view_width, HTML_Data->view_height);
	}
}

/*
 * Find the complete text for this the anchor that aptr is a part of
 * and set it into the selection.
 */
static void
FindSelectAnchor(HTMLGadClData * HTML_Data, struct ele_rec *aptr)
{
	struct ele_rec *eptr;

	eptr = aptr;
	while ((eptr->prev != NULL)&&
		(eptr->prev->anchorHRef != NULL)&&
		(strcmp(eptr->prev->anchorHRef, eptr->anchorHRef) == 0))
	{
		eptr = eptr->prev;
	}
	HTML_Data->select_start = eptr;
	HTML_Data->sel_start_pos = 0;

	eptr = aptr;
	while ((eptr->next != NULL)&&
		(eptr->next->anchorHRef != NULL)&&
		(strcmp(eptr->next->anchorHRef, eptr->anchorHRef) == 0))
	{
		eptr = eptr->next;
	}
	HTML_Data->select_end = eptr;
	HTML_Data->sel_end_pos = eptr->edata_len - 2;
}


/*
 * Set as active all elements in the widget that are part of the anchor
 * in the widget's start ptr.
 */
void
SetAnchor(HTMLGadClData * HTML_Data)
{
	struct ele_rec *eptr;
	struct ele_rec *start;
	struct ele_rec *end;
	unsigned long fg, bg;
	unsigned long old_fg, old_bg;
	struct Region *new, *old;
	struct Rectangle rect;

	eptr = HTML_Data->active_anchor;
	if ((eptr == NULL)||(eptr->anchorHRef == NULL))
	{
		return;
	}
	fg = HTML_Data->activeAnchor_fg;
	bg = HTML_Data->activeAnchor_bg;

	FindSelectAnchor(HTML_Data, eptr);

	start = HTML_Data->select_start;
	end = HTML_Data->select_end;

	/* install amiga clipping region */
	rect.MinX = HTML_Data->view_left;
	rect.MinY = HTML_Data->view_top;
	rect.MaxX = HTML_Data->view_left + HTML_Data->view_width;
	rect.MaxY = HTML_Data->view_top  + HTML_Data->view_height - 1;
	new = NewRegion();
	OrRectRegion(new, &rect);
	old = installClipRegion(HTML_Data->win, HTML_Data->win->WLayer, new);
	/* end clipping */

	eptr = start;
	while ((eptr != NULL)&&(eptr != end))
	{
		if (eptr->type == E_TEXT)
		{
			old_fg = eptr->fg;
			old_bg = eptr->bg;
			eptr->fg = fg;
			eptr->bg = bg;
			TextRefresh(HTML_Data, eptr,
				0, (eptr->edata_len - 2));
			eptr->fg = old_fg;
			eptr->bg = old_bg;
		}
		else if (eptr->type == E_IMAGE)
		{
			old_fg = eptr->fg;
			old_bg = eptr->bg;
			eptr->fg = fg;
			eptr->bg = bg;
			ImageRefresh(HTML_Data, eptr);
			eptr->fg = old_fg;
			eptr->bg = old_bg;
		}
	/*
	 * Linefeeds in anchor spanning multiple lines should NOT
	 * be highlighted!
		else if (eptr->type == E_LINEFEED)
		{
			old_fg = eptr->fg;
			old_bg = eptr->bg;
			eptr->fg = fg;
			eptr->bg = bg;
			LinefeedRefresh(HTML_Data, eptr);
			eptr->fg = old_fg;
			eptr->bg = old_bg;
		}
	*/
		eptr = eptr->next;
	}
	if (eptr != NULL)
	{
		if (eptr->type == E_TEXT)
		{
			old_fg = eptr->fg;
			old_bg = eptr->bg;
			eptr->fg = fg;
			eptr->bg = bg;
			TextRefresh(HTML_Data, eptr,
				0, (eptr->edata_len - 2));
			eptr->fg = old_fg;
			eptr->bg = old_bg;
		}
		else if (eptr->type == E_IMAGE)
		{
			old_fg = eptr->fg;
			old_bg = eptr->bg;
			eptr->fg = fg;
			eptr->bg = bg;
			ImageRefresh(HTML_Data, eptr);
			eptr->fg = old_fg;
			eptr->bg = old_bg;
		}
	/*
	 * Linefeeds in anchor spanning multiple lines should NOT
	 * be highlighted!
		else if (eptr->type == E_LINEFEED)
		{
			old_fg = eptr->fg;
			old_bg = eptr->bg;
			eptr->fg = fg;
			eptr->bg = bg;
			LinefeedRefresh(HTML_Data, eptr);
			eptr->fg = old_fg;
			eptr->bg = old_bg;
		}
	*/
	}
	new = installClipRegion(HTML_Data->win, HTML_Data->win->WLayer, old);
	DisposeRegion(new);
}

#if 0
/*
 * Draw selection for all elements in the widget
 * from start to end.
 */
static void
DrawSelection(HTMLGadClData * HTML_Data, struct ele_rec *start, struct ele_rec *end,
	      int start_pos, int end_pos)
{
	struct ele_rec *eptr;
	int epos;

	if ((start == NULL)||(end == NULL))
	{
		return;
	}

	/*
	 * Keep positions within bounds (allows us to be sloppy elsewhere)
	 */
	if (start_pos < 0)
	{
		start_pos = 0;
	}
	if (start_pos >= start->edata_len - 1)
	{
		start_pos = start->edata_len - 2;
	}
	if (end_pos < 0)
	{
		end_pos = 0;
	}
	if (end_pos >= end->edata_len - 1)
	{
		end_pos = end->edata_len - 2;
	}

	if (SwapElements(start, end, start_pos, end_pos))
	{
		eptr = start;
		start = end;
		end = eptr;
		epos = start_pos;
		start_pos = end_pos;
		end_pos = epos;
	}

	eptr = start;
	while ((eptr != NULL)&&(eptr != end))
	{
		int p1, p2;

		if (eptr == start)
		{
			p1 = start_pos;
		}
		else
		{
			p1 = 0;
		}
		p2 = eptr->edata_len - 2;

		if (eptr->type == E_TEXT)
		{
			eptr->selected = True;
			eptr->start_pos = p1;
			eptr->end_pos = p2;
			TextRefresh(HTML_Data, eptr, p1, p2);
		}
		else if (eptr->type == E_LINEFEED)
		{
			eptr->selected = True;
			LinefeedRefresh(HTML_Data, eptr);
		}
		eptr = eptr->next;
	}
	if (eptr != NULL)
	{
		int p1, p2;

		if (eptr == start)
		{
			p1 = start_pos;
		}
		else
		{
			p1 = 0;
		}

		if (eptr == end)
		{
			p2 = end_pos;
		}
		else
		{
			p2 = eptr->edata_len - 2;
		}

		if (eptr->type == E_TEXT)
		{
			eptr->selected = True;
			eptr->start_pos = p1;
			eptr->end_pos = p2;
			TextRefresh(HTML_Data, eptr, p1, p2);
		}
		else if (eptr->type == E_LINEFEED)
		{
			eptr->selected = True;
			LinefeedRefresh(HTML_Data, eptr);
		}
	}
}


/*
 * Set selection for all elements in the widget's
 * start to end list.
 */
static void
SetSelection(HTMLGadClData * HTML_Data)
{
	struct ele_rec *start;
	struct ele_rec *end;
	int start_pos, end_pos;

	start = HTML_Data->select_start;
	end = HTML_Data->select_end;
	start_pos = HTML_Data->sel_start_pos;
	end_pos = HTML_Data->sel_end_pos;
	DrawSelection(HTML_Data, start, end, start_pos, end_pos);
}
#endif

/*
 * Erase the selection from start to end
 */
static void
EraseSelection(HTMLGadClData * HTML_Data, struct ele_rec *start, struct ele_rec *end,
	       int start_pos, int end_pos)
{
	struct ele_rec *eptr;
	int epos;

	if ((start == NULL)||(end == NULL))
	{
		return;
	}

	/*
	 * Keep positoins within bounds (allows us to be sloppy elsewhere)
	 */
	if (start_pos < 0)
	{
		start_pos = 0;
	}
	if (start_pos >= start->edata_len - 1)
	{
		start_pos = start->edata_len - 2;
	}
	if (end_pos < 0)
	{
		end_pos = 0;
	}
	if (end_pos >= end->edata_len - 1)
	{
		end_pos = end->edata_len - 2;
	}

	if (SwapElements(start, end, start_pos, end_pos))
	{
		eptr = start;
		start = end;
		end = eptr;
		epos = start_pos;
		start_pos = end_pos;
		end_pos = epos;
	}

	eptr = start;
	while ((eptr != NULL)&&(eptr != end))
	{
		int p1, p2;

		if (eptr == start)
		{
			p1 = start_pos;
		}
		else
		{
			p1 = 0;
		}
		p2 = eptr->edata_len - 2;

		if (eptr->type == E_TEXT)
		{
			eptr->selected = False;
			TextRefresh(HTML_Data, eptr, p1, p2);
		}
		else if (eptr->type == E_LINEFEED)
		{
			eptr->selected = False;
			LinefeedRefresh(HTML_Data, eptr);
		}
		eptr = eptr->next;
	}
	if (eptr != NULL)
	{
		int p1, p2;

		if (eptr == start)
		{
			p1 = start_pos;
		}
		else
		{
			p1 = 0;
		}

		if (eptr == end)
		{
			p2 = end_pos;
		}
		else
		{
			p2 = eptr->edata_len - 2;
		}

		if (eptr->type == E_TEXT)
		{
			eptr->selected = False;
			TextRefresh(HTML_Data, eptr, p1, p2);
		}
		else if (eptr->type == E_LINEFEED)
		{
			eptr->selected = False;
			LinefeedRefresh(HTML_Data, eptr);
		}
	}
}


/*
 * Clear the current selection (if there is one)
 */
static void
ClearSelection(HTMLGadClData * HTML_Data)
{
	struct ele_rec *start;
	struct ele_rec *end;
	int start_pos, end_pos;

	start = HTML_Data->select_start;
	end = HTML_Data->select_end;
	start_pos = HTML_Data->sel_start_pos;
	end_pos = HTML_Data->sel_end_pos;
	EraseSelection(HTML_Data, start, end, start_pos, end_pos);

	if ((start == NULL)||(end == NULL))
	{
		HTML_Data->select_start = NULL;
		HTML_Data->select_end = NULL;
		HTML_Data->sel_start_pos = 0;
		HTML_Data->sel_end_pos = 0;
		HTML_Data->active_anchor = NULL;
		return;
	}

	HTML_Data->select_start = NULL;
	HTML_Data->select_end = NULL;
	HTML_Data->sel_start_pos = 0;
	HTML_Data->sel_end_pos = 0;
	HTML_Data->active_anchor = NULL;
}


/*
 * clear from active all elements in the widget that are part of the anchor.
 * (These have already been previously set into the start and end of the
 * selection.
 */
void
UnsetAnchor(HTMLGadClData * HTML_Data)
{
	struct ele_rec *eptr;
	struct Region *new, *old;
	struct Rectangle rect;

	if(eptr = HTML_Data->select_start){

		/* Amiga clipping stuff */
		rect.MinX = HTML_Data->view_left;
		rect.MinY = HTML_Data->view_top;
		rect.MaxX = HTML_Data->view_left + HTML_Data->view_width;
		rect.MaxY = HTML_Data->view_top  + HTML_Data->view_height - 1;
		new = NewRegion();
		OrRectRegion(new, &rect);
		old = installClipRegion(HTML_Data->win, HTML_Data->win->WLayer, new);
		/* end clipping */

		/*
		 * Clear any activated images
		 */

		while ((eptr != NULL)&&(eptr != HTML_Data->select_end)){
			if (eptr->type == E_IMAGE){
				ImageRefresh(HTML_Data, eptr);}
			eptr = eptr->next;
			}
		if ((eptr != NULL)&&(eptr->type == E_IMAGE)){
			ImageRefresh(HTML_Data, eptr);}

		/*
		 * Clear the activated anchor
		 */
		ClearSelection(HTML_Data); /* Selection doesn't work for AMosaic */

		new = installClipRegion(HTML_Data->win, HTML_Data->win->WLayer, old);
		DisposeRegion(new);
	}
}

#if 0
/*
 * Erase the old selection, and draw the new one in such a way
 * that advantage is taken of overlap, and there is no obnoxious
 * flashing.
 */
static void
ChangeSelection(HTMLGadClData * HTML_Data, struct ele_rec *start, struct ele_rec *end,
		int start_pos, int end_pos)
{
	struct ele_rec *old_start;
	struct ele_rec *old_end;
	struct ele_rec *new_start;
	struct ele_rec *new_end;
	struct ele_rec *eptr;
	int epos;
	int new_start_pos, new_end_pos;
	int old_start_pos, old_end_pos;

	old_start = HTML_Data->new_start;
	old_end = HTML_Data->new_end;
	old_start_pos = HTML_Data->new_start_pos;
	old_end_pos = HTML_Data->new_end_pos;
	new_start = start;
	new_end = end;
	new_start_pos = start_pos;
	new_end_pos = end_pos;

	if ((new_start == NULL)||(new_end == NULL))
	{
		return;
	}

	if ((old_start == NULL)||(old_end == NULL))
	{
		DrawSelection(HTML_Data, new_start, new_end,
			new_start_pos, new_end_pos);
		return;
	}

	if (SwapElements(old_start, old_end, old_start_pos, old_end_pos))
	{
		eptr = old_start;
		old_start = old_end;
		old_end = eptr;
		epos = old_start_pos;
		old_start_pos = old_end_pos;
		old_end_pos = epos;
	}

	if (SwapElements(new_start, new_end, new_start_pos, new_end_pos))
	{
		eptr = new_start;
		new_start = new_end;
		new_end = eptr;
		epos = new_start_pos;
		new_start_pos = new_end_pos;
		new_end_pos = epos;
	}

	/*
	 * Deal with all possible intersections of the 2 selection sets.
	 *
	 ********************************************************
	 *			*				*
	 *      |--		*	     |--		*
	 * old--|		*	new--|			*
	 *      |--		*	     |--		*
	 *			*				*
	 *      |--		*	     |--		*
	 * new--|		*	old--|			*
	 *      |--		*	     |--		*
	 *			*				*
	 ********************************************************
	 *			*				*
	 *      |----		*	       |--		*
	 * old--|		*	  new--|		*
	 *      | |--		*	       |		*
	 *      |-+--		*	     |-+--		*
	 *        |		*	     | |--		*
	 *   new--|		*	old--|			*
	 *        |--		*	     |----		*
	 *			*				*
	 ********************************************************
	 *			*				*
	 *      |---------	*	     |---------		*
	 *      |		*	     |			*
	 *      |      |--	*	     |      |--		*
	 * new--| old--|	*	old--| new--|		*
	 *      |      |--	*	     |      |--		*
	 *      |		*	     |			*
	 *      |---------	*	     |---------		*
	 *			*				*
	 ********************************************************
	 *
	 */
	if ((ElementLessThan(old_end, new_start, old_end_pos, new_start_pos))||
	    (ElementLessThan(new_end, old_start, new_end_pos, old_start_pos)))
	{
		EraseSelection(HTML_Data, old_start, old_end,
			old_start_pos, old_end_pos);
		DrawSelection(HTML_Data, new_start, new_end,
			new_start_pos, new_end_pos);
	}
	else if ((ElementLessThan(old_start, new_start,
			old_start_pos, new_start_pos))&&
		 (ElementLessThan(old_end, new_end, old_end_pos, new_end_pos)))
	{
		if (new_start_pos != 0)
		{
			EraseSelection(HTML_Data, old_start, new_start,
				old_start_pos, new_start_pos - 1);
		}
		else
		{
			EraseSelection(HTML_Data, old_start, new_start->prev,
				old_start_pos, new_start->prev->edata_len - 2);
		}
		if (old_end_pos < (old_end->edata_len - 2))
		{
			DrawSelection(HTML_Data, old_end, new_end,
				old_end_pos + 1, new_end_pos);
		}
		else
		{
			DrawSelection(HTML_Data, old_end->next, new_end,
				0, new_end_pos);
		}
	}
	else if ((ElementLessThan(new_start, old_start,
			new_start_pos, old_start_pos))&&
		 (ElementLessThan(new_end, old_end, new_end_pos, old_end_pos)))
	{
		if (old_start_pos != 0)
		{
			DrawSelection(HTML_Data, new_start, old_start,
				new_start_pos, old_start_pos - 1);
		}
		else
		{
			DrawSelection(HTML_Data, new_start, old_start->prev,
				new_start_pos, old_start->prev->edata_len - 2);
		}
		if (new_end_pos < (new_end->edata_len - 2))
		{
			EraseSelection(HTML_Data, new_end, old_end,
				new_end_pos + 1, old_end_pos);
		}
		else
		{
			EraseSelection(HTML_Data, new_end->next, old_end,
				0, old_end_pos);
		}
	}
	else if ((ElementLessThan(new_start, old_start,
			new_start_pos, old_start_pos))||
		 (ElementLessThan(old_end, new_end, old_end_pos, new_end_pos)))
	{
		if ((new_start != old_start)||(new_start_pos != old_start_pos))
		{
			if (old_start_pos != 0)
			{
				DrawSelection(HTML_Data, new_start, old_start,
					new_start_pos, old_start_pos - 1);
			}
			else
			{
				DrawSelection(HTML_Data, new_start, old_start->prev,
					new_start_pos,
					old_start->prev->edata_len - 2);
			}
		}
		if ((old_end != new_end)||(old_end_pos != new_end_pos))
		{
			if (old_end_pos < (old_end->edata_len - 2))
			{
				DrawSelection(HTML_Data, old_end, new_end,
					old_end_pos + 1, new_end_pos);
			}
			else
			{
				DrawSelection(HTML_Data, old_end->next, new_end,
					0, new_end_pos);
			}
		}
	}
	else
	{
		if ((old_start != new_start)||(old_start_pos != new_start_pos))
		{
			if (new_start_pos != 0)
			{
				EraseSelection(HTML_Data, old_start, new_start,
					old_start_pos, new_start_pos - 1);
			}
			else
			{
				EraseSelection(HTML_Data, old_start, new_start->prev,
					old_start_pos,
					new_start->prev->edata_len - 2);
			}
		}
		if ((new_end != old_end)||(new_end_pos != old_end_pos))
		{
			if (new_end_pos < (new_end->edata_len - 2))
			{
				EraseSelection(HTML_Data, new_end, old_end,
					new_end_pos + 1, old_end_pos);
			}
			else
			{
				EraseSelection(HTML_Data, new_end->next, old_end,
					0, old_end_pos);
			}
		}
	}
}
#endif


/* MDF - I should port this!! */

/*
 * Go through the parsed marks and for all the <LINK> tags in the document
 * call the LinkCallback.
 */
static void
CallLinkCallbacks(HTMLGadClData * HTML_Data)
{
	struct mark_up *mptr;
	LinkInfo l_info;

	mptr = HTML_Data->html_objects;
	while (mptr != NULL)
	{
		if (mptr->type == M_BASE)
		{
			l_info.href = ParseMarkTag(mptr->start, MT_BASE,
				"HREF");
			l_info.role = ParseMarkTag(mptr->start, MT_BASE,
				"ROLE");
//			XtCallCallbackList ((Widget)HTML_Data, HTML_Data->link_callback,
//				(XtPointer)&l_info);
			if (l_info.href != NULL)
			{
				free(l_info.href);
			}
			if (l_info.role != NULL)
			{
				free(l_info.role);
			}
		}
		mptr = mptr->next;
	}
}

#if 0
/*
 * Search through the whole document, and recolor the internal elements with
 * the passed HREF.
 */
static void
RecolorInternalHRefs(HTMLGadClData * HTML_Data, char *href)
{
	struct ele_rec *start;
	unsigned long fg;

	fg = HTML_Data->visitedAnchor_fg;
	start = HTML_Data->formatted_elements;
	while (start != NULL)
	{
		/*
		 * This one is internal
		 * This one has an href
		 * This is the href we want
		 */
		if ((start->internal == True)&&
		    (start->anchorHRef != NULL)&&
		    (strcmp(start->anchorHRef, href) == 0))
		{
			start->fg = fg;
			start->underline_number =
				HTML_Data->num_visitedAnchor_underlines;
			start->dashed_underline =
				HTML_Data->dashed_visitedAnchor_lines;
		}
		start = start->next;
	}
}
#endif

/*
 *************************************************************************
 ******************************* PUBLIC FUNCTIONS ************************
 *************************************************************************
 */


/*
 * Convenience function to return the text of the HTML document as a plain
 * ascii text string.
 * This function allocates memory for the returned string, that it is up
 * to the user to free.
 * Extra option flags "pretty" text to be returned.
 */
char *
HTMLGetText(HTMLGadClData *w, int pretty)
{
	HTMLGadClData * HTML_Data = w;
	char *text;
	char *tptr = NULL, *buf;
	struct ele_rec *start;
	struct ele_rec *end;

	text = NULL;
	start = HTML_Data->formatted_elements;
	end = start;
	while (end != NULL)
	{
		end = end->next;
	}

	if (pretty == 2)
	{
/*
		tptr = ParseTextToPSString(HTML_Data, start, start, end,
				0, 0,
				HTML_Data->font->max_bounds.width,
				HTML_Data->margin_width);
*/
	}
	else if (pretty)
	{
		int width;

		SetFont(HTML_Data->rp, HTML_Data->font);
		width = TextLength(HTML_Data->rp, "n", 1); /* average character? */
		tptr = ParseTextToPrettyString(HTML_Data, start, start, end,
				0, 0,
				width,
				HTML_Data->margin_width);
	}
	else
	{
		int width;

		SetFont(HTML_Data->rp, HTML_Data->font);
		width = TextLength(HTML_Data->rp, "n", 1); /* average character? */
		tptr = ParseTextToString(start, start, end,
				0, 0,
				width,
				HTML_Data->margin_width);
	}
	if (tptr != NULL)
	{
		if (text == NULL)
		{
			text = tptr;
		}
		else
		{
			buf = (char *)malloc(strlen(text) +
				strlen(tptr) + 1);
			strcpy(buf, text);
			strcat(buf, tptr);
			free(text);
			free(tptr);
			text = buf;
		}
	}
	return(text);
}


/*
 * Convenience function to return the element id of the element
 * nearest to the x,y coordinates passed in.
 * If there is no element there, return the first element in the
 * line we are on.  If there we are on no line, either return the
 * beginning, or the end of the document.
 */
int
HTMLPositionToId(HTMLGadClData *w, int x, int y)
{
	HTMLGadClData * HTML_Data = w;
	int i;
	int epos;
	struct ele_rec *eptr;

	eptr = LocateElement(HTML_Data, x, y, &epos);
	if (eptr == NULL)
	{
//mjw		x = x + HTML_Data->scroll_x;
//sb		y = y + HTML_Data->scroll_y; /* This look stupid! */
		eptr = HTML_Data->line_array[0];
		for (i=0; i<HTML_Data->line_count; i++)
		{
			if (HTML_Data->line_array[i] == NULL)
			{
				continue;
			}
			else if (HTML_Data->line_array[i]->y <= y)
			{
				eptr = HTML_Data->line_array[i];
				continue;
			}
			else
			{
				break;
			}
		}
	}

	/*
	 * 0 means the very top of the document.  We put you there for
	 * unfound elements.
	 * We also special case for when the scrollbar is at the
	 * absolute top.
	 */
	if ((eptr == NULL)||(HTML_Data->scroll_y == 0))
	{
		return(0);
	}
	else
	{
		return(eptr->ele_id);
	}
}


/*
 * Convenience function to return the position of the element
 * based on the element id passed in.
 * Function returns 1 on success and fills in x,y pixel values.
 * If there is no such element, x=0, y=0 and -1 is returned.
 */
int
HTMLIdToPosition(Widget w, int element_id, int *x, int *y)
{
	HTMLGadClData * HTML_Data = (HTMLGadClData *)w;
	struct ele_rec *start;
	struct ele_rec *eptr;

	eptr = NULL;
	start = HTML_Data->formatted_elements;
	while (start != NULL)
	{
		if (start->ele_id == element_id)
		{
			eptr = start;
			break;
		}
		start = start->next;
	}

	if (eptr == NULL)
	{
		*x = 0;
		*y = 0;
		return(-1);
	}
	else
	{
		*x = eptr->x;
		*y = eptr->y;
		return(1);
	}
}


/*
 * Convenience function to position the element
 * based on the element id passed at the top of the viewing area.
 * A passed in id of 0 means goto the top.
 */
void
HTMLGotoId(HTMLGadClData *w, int element_id)
{
	HTMLGadClData * HTML_Data = w;
	struct ele_rec *start;
	struct ele_rec *eptr;
	int newy;
#ifdef MOTIF
	int val, size, inc, pageinc;
#endif

	/*
	 * If we have no scrollbar, just return.
	 */
	if (HTML_Data->use_vbar == False)
	{
		return;
	}

	/*
	 * Find the element corrsponding to the id passed in.
	 */
	eptr = NULL;
	start = HTML_Data->formatted_elements;
	while (start != NULL)
	{
		if (start->ele_id == element_id)
		{
			eptr = start;
			break;
		}
		start = start->next;
	}

	/*
	 * No such element, do nothing.
	 */
	if ((element_id != 0)&&(eptr == NULL))
	{
		return;
	}

	if (element_id == 0)
	{
		newy = 0;
	}
	else
	{
		newy = eptr->y - 2;
	}
	if (newy < 0)
	{
		newy = 0;
	}
	if (newy > (HTML_Data->doc_height - (int)HTML_Data->view_height))
	{
		newy = HTML_Data->doc_height - (int)HTML_Data->view_height;
	}
	if (newy < 0)
	{
		newy = 0;
	}
	set(HTML_Gad,HTMLA_scroll_y,newy);
	set(HTML_Gad,HTMLA_scroll_x,0);
//	ScrollToPos(HTML_Data, 0, newy);
//	ScrollToPos(HTML_Data, 1, 0);
//	ScrollToPos(HTML_Data->vbar, HTML_Data, newy);
//	ScrollToPos(HTML_Data->hbar, HTML_Data, 0);
//	setScrollBar(HTML_Data->vbar, newy, 
//		     HTML_Data->doc_height,
//		     HTML_Data->view_height);
// This one makes no sense? #endif
}


/*
 * Convenience function to return the position of the anchor
 * based on the anchor NAME passed.
 * Function returns 1 on success and fills in x,y pixel values.
 * If there is no such element, x=0, y=0 and -1 is returned.
 */
int
HTMLAnchorToPosition(Widget w, char *name, int *x, int *y)
{
	HTMLGadClData * HTML_Data = (HTMLGadClData *)w;
	struct ele_rec *start;
	struct ele_rec *eptr;

	eptr = NULL;
	start = HTML_Data->formatted_elements;
	while (start != NULL)
	{
		if ((start->anchorName)&&
		    (strcmp(start->anchorName, name) == 0))
		{
			eptr = start;
			break;
		}
		start = start->next;
	}

	if (eptr == NULL)
	{
		*x = 0;
		*y = 0;
		return(-1);
	}
	else
	{
		*x = eptr->x;
		*y = eptr->y;
		return(1);
	}
}


/*
 * Convenience function to return the element id of the anchor
 * based on the anchor NAME passed.
 * Function returns id on success.
 * If there is no such element, 0 is returned.
 */
int
HTMLAnchorToId(Object *HTML_Gad, char *name)
{
	HTMLGadClData * HTML_Data = INST_DATA(HTMLGadClass,HTML_Gad);
	struct ele_rec *start;
	struct ele_rec *eptr;

	/*
	 * Find the passed anchor name
	 */
	eptr = NULL;
	start = HTML_Data->formatted_elements;
	while (start != NULL)
	{
		if ((start->anchorName)&&
		    (strcmp(start->anchorName, name) == 0))
		{
			eptr = start;
			break;
		}
		start = start->next;
	}

	if (eptr == NULL)
	{
		return(0);
	}
	else
	{
		return(eptr->ele_id);
	}
}


/*
 * Convenience function to return the HREFs of all active anchors in the
 * document.
 * Function returns an array of strings and fills num_hrefs passed.
 * If there are no HREFs NULL returned.
 */
char **
HTMLGetHRefs(Object *HTML_Gad, int *num_hrefs)
{
	HTMLGadClData * HTML_Data = INST_DATA(HTMLGadClass,HTML_Gad);
	int cnt;
	struct ele_rec *start;
	struct ele_rec *list;
	struct ele_rec *eptr;
	char **harray;

	list = NULL;
	cnt = 0;
	/*
	 * Construct a linked list of all the diffeent hrefs, counting
	 * then as we go.
	 */
	start = HTML_Data->formatted_elements;
	while (start != NULL)
	{
		/*
		 * This one has an HREF
		 */
		if (start->anchorHRef != NULL)
		{
			/*
			 * Check to see if we already have
			 * this HREF in our list.
			 */
			eptr = list;
			while (eptr != NULL)
			{
				if (strcmp(eptr->anchorHRef,
					start->anchorHRef) == 0)
				{
					break;
				}
				eptr = eptr->next;
			}
			/*
			 * This HREF is not, in our list.  Add it.
			 * That is, if it's not an internal reference.
			 */
			if ((eptr == NULL)&&(start->internal == False))
			{
				eptr = (struct ele_rec *)
					malloc(sizeof(struct ele_rec));
				eptr->anchorHRef = start->anchorHRef;
				eptr->next = list;
				list = eptr;
				cnt++;
			}
		}
		start = start->next;
	}

	if (cnt == 0)
	{
		*num_hrefs = 0;
		return(NULL);
	}
	else
	{
		*num_hrefs = cnt;
		harray = (char **)malloc(sizeof(char *) * cnt);
		eptr = list;
		cnt--;
		while (eptr != NULL)
		{
			harray[cnt] = (char *)
				malloc(strlen(eptr->anchorHRef) + 1);
			strcpy(harray[cnt], eptr->anchorHRef);
			start = eptr;
			eptr = eptr->next;
			free((char *)start);
			cnt--;
		}
		return(harray);
	}
}


/*
 * Convenience function to return the SRCs of all images in the
 * document.
 * Function returns an array of strings and fills num_srcs passed.
 * If there are no SRCs NULL returned.
 */
char **
HTMLGetImageSrcs(HTMLGadClData *w, int *num_srcs)
{
	HTMLGadClData * HTML_Data = w;
	struct mark_up *mptr;
	int cnt;
	char *tptr;
	char **harray;

	cnt = 0;
	mptr = HTML_Data->html_objects;
	while (mptr != NULL)
	{
		if (mptr->type == M_IMAGE)
		{
			tptr = ParseMarkTag(mptr->start, MT_IMAGE, "SRC");
			if ((tptr != NULL)&&(*tptr != '\0'))
			{
				cnt++;
				free(tptr);
			}
		}
		else if (mptr->type == M_INPUT)
		{
			tptr = ParseMarkTag(mptr->start, MT_INPUT, "TYPE");
			if (tptr && !stricmp(tptr,"image"))
			{
				free(tptr);
				tptr = ParseMarkTag(mptr->start, MT_IMAGE, "SRC");
				if ((tptr != NULL)&&(*tptr != '\0'))
					{
					cnt++;
					free(tptr);
					}
			}
			else
				free(tptr);

		}

		mptr = mptr->next;
	}

	if (cnt == 0)
	{
		*num_srcs = 0;
		return(NULL);
	}
	else
	{
		*num_srcs = cnt;
		harray = (char **)malloc(sizeof(char *) * cnt);
		mptr = HTML_Data->html_objects;
		cnt = 0;
		while (mptr != NULL)
		{
			if (mptr->type == M_IMAGE)
			{
				tptr = ParseMarkTag(mptr->start,MT_IMAGE,"SRC");
				if ((tptr != NULL)&&(*tptr != '\0'))
				{
					harray[cnt] = tptr;
					cnt++;
				}
			}
			else if (mptr->type == M_INPUT){
				tptr = ParseMarkTag(mptr->start, MT_INPUT, "TYPE");
				if (tptr && !stricmp(tptr,"image")){
					free(tptr);
					tptr = ParseMarkTag(mptr->start, MT_IMAGE, "SRC");
					if ((tptr != NULL)&&(*tptr != '\0')){
						harray[cnt] = tptr;
						cnt++;
						}
				}
				else
					free(tptr);
				}

			mptr = mptr->next;
		}
		return(harray);
	}
}


/*
 * Convenience function to return the link information
 * for all the <LINK> tags in the document.
 * Function returns an array of LinkInfo structures and fills
 * num_links passed.
 * If there are no LINKs NULL returned.
 */
LinkInfo *
HTMLGetLinks(Widget w, int *num_links)
{
	HTMLGadClData * HTML_Data = (HTMLGadClData *)w;
	struct mark_up *mptr;
	int cnt;
	char *tptr;
	LinkInfo *larray;

	cnt = 0;
	mptr = HTML_Data->html_objects;
	while (mptr != NULL)
	{
		if (mptr->type == M_BASE)
		{
			cnt++;
		}
		mptr = mptr->next;
	}

	if (cnt == 0)
	{
		*num_links = 0;
		return(NULL);
	}
	else
	{
		*num_links = cnt;
		larray = (LinkInfo *)malloc(sizeof(LinkInfo) * cnt);
		mptr = HTML_Data->html_objects;
		cnt = 0;
		while (mptr != NULL)
		{
			if (mptr->type == M_BASE)
			{
				tptr = ParseMarkTag(mptr->start,
					MT_BASE, "HREF");
				larray[cnt].href = tptr;
				tptr = ParseMarkTag(mptr->start,
					MT_BASE, "ROLE");
				larray[cnt].role = tptr;
				cnt++;
			}
			mptr = mptr->next;
		}
		return(larray);
	}
}



void *
HTMLGetWidgetInfo(HTMLGadClData *w)
{
	HTMLGadClData * HTML_Data = w;

	return((void *)HTML_Data->widget_list);
}


void
HTMLFreeImageInfo(Widget w)
{
	HTMLGadClData * HTML_Data = (HTMLGadClData *)w;

//	FreeColors(XtDisplay(w), DefaultColormapOfScreen(XtScreen(w)));
	FreeImages(HTML_Data);
}


void
HTMLFreeWidgetInfo(void *ptr)
{
	WidgetInfo *wptr = (WidgetInfo *)ptr;
	WidgetInfo *tptr;

	while (wptr != NULL)
	{
		tptr = wptr;
		wptr = wptr->next;
		if (tptr->w != NULL)
		{
//			XtDestroyWidget(tptr->w);
		}
		if (tptr->name != NULL)
		{
			free(tptr->name);
		}
		if ((tptr->value != NULL)&&(tptr->type != W_OPTIONMENU))
		{
			free(tptr->value);
		}
		free((char *)tptr);
	}
}


/*
 * Convenience function to redraw all active anchors in the
 * document.
 * Can also pass a new predicate function to check visited
 * anchors.  If NULL passed for function, uses default predicate
 * function.
 */
void
HTMLRetestAnchors(Widget w, visitTestProc testFunc)
{
	HTMLGadClData * HTML_Data = (HTMLGadClData *)w;
	struct ele_rec *start;

	if (testFunc == NULL)
	{
		testFunc = (visitTestProc)HTML_Data->previously_visited_test;
	}

	/*
	 * Search all elements
	 */
	start = HTML_Data->formatted_elements;
	while (start != NULL)
	{
		if ((start->internal == True)||
		    (start->anchorHRef == NULL))
		{
			start = start->next;
			continue;
		}

		if (testFunc != NULL)
		{
			if ((*testFunc)(HTML_Data, start->anchorHRef))
			{
			    start->fg = HTML_Data->visitedAnchor_fg;
			    start->underline_number =
				HTML_Data->num_visitedAnchor_underlines;
			    start->dashed_underline =
				HTML_Data->dashed_visitedAnchor_lines;
			}
			else
			{
			    start->fg = HTML_Data->anchor_fg;
			    start->underline_number =
				HTML_Data->num_anchor_underlines;
			    start->dashed_underline =
				HTML_Data->dashed_anchor_lines;
			}
		}
		else
		{
			start->fg = HTML_Data->anchor_fg;
			start->underline_number =
				HTML_Data->num_anchor_underlines;
			start->dashed_underline =
				HTML_Data->dashed_anchor_lines;
		}

		/*
		 * Since the element may have changed, redraw it
		 */
		switch(start->type)
		{
			case E_TEXT:
				TextRefresh(HTML_Data, start,
				     0, (start->edata_len - 2));
				break;
			case E_IMAGE:
				ImageRefresh(HTML_Data, start);
				break;
			case E_BULLET:
				BulletRefresh(HTML_Data, start);
				break;
			case E_LINEFEED:
				LinefeedRefresh(HTML_Data, start);
				break;
		}

		start = start->next;
	}
}


void
HTMLClearSelection(Widget w)
{
//	LoseSelection (w, NULL);
}


/*
 * Convenience function to return the text of the HTML document as a single
 * white space separated string, with pointers to the various start and
 * end points of selections.
 * This function allocates memory for the returned string, that it is up
 * to the user to free.
 */
char *
HTMLGetTextAndSelection(Widget w, char **startp, char **endp, char **insertp)
{
	HTMLGadClData * HTML_Data = (HTMLGadClData *)w;
	int length;
	char *text;
	char *tptr;
	struct ele_rec *eptr;
	struct ele_rec *sel_start;
	struct ele_rec *sel_end;
	struct ele_rec *insert_start;
	int start_pos, end_pos, insert_pos;

	if (SwapElements(HTML_Data->select_start, HTML_Data->select_end,
		HTML_Data->sel_start_pos, HTML_Data->sel_end_pos))
	{
		sel_end = HTML_Data->select_start;
		end_pos = HTML_Data->sel_start_pos;
		sel_start = HTML_Data->select_end;
		start_pos = HTML_Data->sel_end_pos;
	}
	else
	{
		sel_start = HTML_Data->select_start;
		start_pos = HTML_Data->sel_start_pos;
		sel_end = HTML_Data->select_end;
		end_pos = HTML_Data->sel_end_pos;
	}

	insert_start = HTML_Data->new_start;
	insert_pos = HTML_Data->new_start_pos;
	*startp = NULL;
	*endp = NULL;
	*insertp = NULL;

	length = 0;

	eptr = HTML_Data->formatted_elements;
	while (eptr != NULL)
	{
		/*
		 * Skip the special internal text
		 */
		if (eptr->internal == True)
		{
			eptr = eptr->next;
			continue;
		}

		if (eptr->type == E_TEXT)
		{
			length = length + eptr->edata_len - 1;
		}
		else if (eptr->type == E_LINEFEED)
		{
			length = length + 1;
		}
		eptr = eptr->next;
	}

	text = (char *)malloc(length + 1);
	if (text == NULL)
	{
		fprintf(stderr, "No space for return string\n");
		return(NULL);
	}
	strcpy(text, "");

	tptr = text;

	eptr = HTML_Data->formatted_elements;
	while (eptr != NULL)
	{
		/*
		 * Skip the special internal text
		 */
		if (eptr->internal == True)
		{
			eptr = eptr->next;
			continue;
		}

		if (eptr->type == E_TEXT)
		{
			if (eptr == sel_start)
			{
				*startp = (char *)(tptr + start_pos);
			}

			if (eptr == sel_end)
			{
				*endp = (char *)(tptr + end_pos);
			}

			if (eptr == insert_start)
			{
				*insertp = (char *)(tptr + insert_pos);
			}

			strcat(text, (char *)eptr->edata);
			tptr = tptr + eptr->edata_len - 1;
		}
		else if (eptr->type == E_LINEFEED)
		{
			if (eptr == sel_start)
			{
				*startp = tptr;
			}

			if (eptr == sel_end)
			{
				*endp = tptr;
			}

			if (eptr == insert_start)
			{
				*insertp = tptr;
			}

			strcat(text, " ");
			tptr = tptr + 1;
		}
		eptr = eptr->next;
	}
	return(text);
}


/*
 * Convenience function to set the raw text into the widget.
 * Forces a reparse and a reformat.
 * If any pointer is passed in as NULL that text is unchanged,
 * if a pointer points to an empty string, that text is set to NULL;
 * Also pass an element ID to set the view area to that section of the new
 * text.  Finally pass an anchor NAME to set position of the new text
 * to that anchor.
 */
void
HTMLSetText(HTMLGadClData *HTML_Data, char *text, char *header_text, char *footer_text, int element_id, char *target_anchor, void *ptr)
{
//	HTMLGadClData *HTML_Data = INST_DATA(HTMLGadClass,HTML_Gad);
	struct ele_rec *start;
	struct ele_rec *eptr;
	int newy;

	if ((text == NULL)&&(header_text == NULL)&&(footer_text == NULL))
	{
		return;
	}

	/*
	 * Free up the old visited href list.
	 */
	FreeHRefs(HTML_Data->my_visited_hrefs);
	HTML_Data->my_visited_hrefs = NULL;

	/*
	 * Free up the old visited delayed images list.
	 */
	FreeDelayedImages(HTML_Data->my_delayed_images);
	HTML_Data->my_delayed_images = NULL;

	/*
	 * Hide any old widgets
	 */

	if(!(HTML_Data->object_flags&Object_Reparsing)){
		HideWidgets(HTML_Data);
		DisposeForms(HTML_Data->form_list);
		}
/*	HTML_Data->widget_list = wptr; */
	HTML_Data->form_list = NULL;

	if (text != NULL)
	{
		if (*text == '\0')
		{
			text = NULL;
		}
		HTML_Data->raw_text = text;

		/*
		 * Free any old colors and pixmaps
		 */
//		FreeColors(XtDisplay(HTML_Data),DefaultColormapOfScreen(XtScreen(HTML_Data)));
//		FreeImages(HTML_Data);

		/*
		 * Parse the raw text with the HTML parser
		 */
		HTML_Data->html_objects = HTMLParse(HTML_Data->html_objects,
			HTML_Data->raw_text);
		CallLinkCallbacks(HTML_Data);
	}
	if (header_text != NULL)
	{
		if (*header_text == '\0')
		{
			header_text = NULL;
		}
		HTML_Data->header_text = header_text;

		/*
		 * Parse the header text with the HTML parser
		 */
		HTML_Data->html_header_objects =
			HTMLParse(HTML_Data->html_header_objects,
			HTML_Data->header_text);
	}
	if (footer_text != NULL)
	{
		if (*footer_text == '\0')
		{
			footer_text = NULL;
		}
		HTML_Data->footer_text = footer_text;

		/*
		 * Parse the footer text with the HTML parser
		 */
		HTML_Data->html_footer_objects =
			HTMLParse(HTML_Data->html_footer_objects,
			HTML_Data->footer_text);
	}

	/*
	 * Reformat the new text
	 */
	if(HTML_Data->win){
		HTML_Data->max_pre_width = DocumentWidth(HTML_Data, HTML_Data->html_objects);
		ReformatWindow(HTML_Data);
		}

	/*
	 * If a target anchor is passed, override the element id
	 * with the id of that anchor.
	 */
	if (target_anchor != NULL)
	{
		int id;

		id = HTMLAnchorToId(HTML_Gad, target_anchor);
		if (id != 0)
		{
			element_id = id;
		}
	}

	/*
	 * Position text at id specified, or at top if no position
	 * specified.
	 * Find the element corrsponding to the id passed in.
	 */
	eptr = NULL;
	if (element_id != 0)
	{
		start = HTML_Data->formatted_elements;
		while (start != NULL)
		{
			if (start->ele_id == element_id)
			{
				eptr = start;
				break;
			}
			start = start->next;
		}
	}
	if (eptr == NULL)
	{
		newy = 0;
	}
	else
	{
		newy = eptr->y - 2;
	}
	if (newy < 0)
	{
		newy = 0;
	}
	if (newy > (HTML_Data->doc_height - (int)HTML_Data->view_height))
	{
		newy = HTML_Data->doc_height - (int)HTML_Data->view_height;
	}
	if (newy < 0)
	{
		newy = 0;
	}
	HTML_Data->scroll_x = 0;
	HTML_Data->scroll_y = 0;
	HTML_Data->new_scroll_y = newy;

#ifdef DEBUG
fprintf (stderr, "calling in HTMLSetText\n");
#endif

//	ConfigScrollBars(HTML_Data);
//	ScrollWidgets(HTML_Data);

	/*
	 * Display the new text
	 */
	AddNewWidgets(HTML_Data);

/*	MUI_Redraw(HTML_Gad,MADF_DRAWOBJECT); */

	/*
	 * Clear any previous selection
	 */
	HTML_Data->select_start = NULL;
	HTML_Data->select_end = NULL;
	HTML_Data->sel_start_pos = 0;
	HTML_Data->sel_end_pos = 0;
	HTML_Data->new_start = NULL;
	HTML_Data->new_end = NULL;
	HTML_Data->new_start_pos = 0;
	HTML_Data->new_end_pos = 0;
	HTML_Data->active_anchor = NULL;

    HTML_Data->cached_tracked_ele = NULL;
}


/*
 * To use faster TOLOWER as set up in HTMLparse.c
 */
#ifdef NOT_ASCII
#define TOLOWER(x)      (tolower(x))
#else
extern char map_table[];
#define TOLOWER(x)      (map_table[(int)(x)])
#endif /* NOT_ASCII */


/*
 * Convenience function to search the text of the HTML document as a single
 * white space separated string. Linefeeds are converted into spaces.
 *
 * Takes a pattern, pointers to the start and end blocks to store the
 * start and end of the match into.  Start is also used as the location to
 * start the search from for incremental searching.  If start is an invalid
 * position (id = 0).  Default start is the beginning of the document for
 * forward searching, and the end of the document for backwards searching.
 * The backward and caseless parameters I hope are self-explanatory.
 *
 * returns 1 on success
 *      (and the start and end positions of the match).
 * returns -1 otherwise (and start and end are unchanged).
 */
int
HTMLSearchText (HTMLGadClData *HTML_Data, char *pattern, ElementRef *m_start, ElementRef *m_end,
		int backward, int caseless)
{
	int found, equal;
	char *match;
	char *tptr;
	char *mptr;
	char cval;
	struct ele_rec *eptr;
	int s_pos;
	struct ele_rec *s_eptr;
	ElementRef s_ref, e_ref;
	ElementRef *start, *end;

	/*
	 * If bad parameters are passed, just fail the search
	 */
	if ((pattern == NULL)||(*pattern == '\0')||
		(m_start == NULL)||(m_end == NULL))
	{
		return(-1);
	}

	/*
	 * If we are caseless, make a lower case copy of the pattern to
	 * match to use in compares.
	 *
	 * remember to free this before returning
	 */
	if (caseless)
	{
		match = (char *)malloc(strlen(pattern) + 1);
		tptr = pattern;
		mptr = match;
		while (*tptr != '\0')
		{
			*mptr = (char)TOLOWER((int)*tptr);
			mptr++;
			tptr++;
		}
		*mptr = '\0';
	}
	else
	{
		match = pattern;
	}

	/*
	 * Slimy coding.  I later decided I didn't want to change start and
	 * end if the search failed.  Rather than changing all the code,
	 * I just copy it into locals here, and copy it out again if a match
	 * is found.
	 */
	start = &s_ref;
	end = &e_ref;
	start->id = m_start->id;
	start->pos = m_start->pos;
	end->id = m_end->id;
	end->pos = m_end->pos;

	/*
	 * Find the user specified start position.
	 */
	if (start->id > 0)
	{
		found = 0;
		eptr = HTML_Data->formatted_elements;

		while (eptr != NULL)
		{
			if (eptr->ele_id == start->id)
			{
				s_eptr = eptr;
				found = 1;
				break;
			}
			eptr = eptr->next;
		}
		/*
		 * Bad start position, fail them out.
		 */
		if (!found)
		{
			if (caseless)
			{
				free(match);
			}
			return(-1);
		}
		/*
		 * Sanify the start position
		 */
		s_pos = start->pos;
		if (s_pos >= s_eptr->edata_len - 1)
		{
			s_pos = s_eptr->edata_len - 2;
		}
		if (s_pos < 0)
		{
			s_pos = 0;
		}
	}
	else
	{
		/*
		 * Default search starts at end for backward, and
		 * beginning for forwards.
		 */
		if (backward)
		{
			s_eptr = HTML_Data->formatted_elements;
			while (s_eptr->next != NULL)
			{
				s_eptr = s_eptr->next;
			}
			s_pos = s_eptr->edata_len - 2;
		}
		else
		{
			s_eptr = HTML_Data->formatted_elements;
			s_pos = 0;
		}
	}

	if (backward)
	{
		char *mend;

		/*
		 * Save the end of match here for easy end to start searching
		 */
		mend = match;
		while (*mend != '\0')
		{
			mend++;
		}
		if (mend > match)
		{
			mend--;
		}
		found = 0;
		equal = 0;
		mptr = mend;

		if (s_eptr != NULL)
		{
			eptr = s_eptr;
		}
		else
		{
			eptr = HTML_Data->formatted_elements;
			while (eptr->next != NULL)
			{
				eptr = eptr->next;
			}
		}

		while (eptr != NULL)
		{
			/*
			 * Skip the special internal text
			 */
			if (eptr->internal == True)
			{
				eptr = eptr->prev;
				continue;
			}

			if (eptr->type == E_TEXT)
			{
			    tptr = (char *)(eptr->edata + eptr->edata_len - 2);
			    if (eptr == s_eptr)
			    {
				tptr = (char *)(eptr->edata + s_pos);
			    }
			    while (tptr >= eptr->edata)
			    {
				if (equal)
				{
					if (caseless)
					{
						cval =(char)TOLOWER((int)*tptr);
					}
					else
					{
						cval = *tptr;
					}
					while ((mptr >= match)&&
						(tptr >= eptr->edata)&&
						(cval == *mptr))
					{
						tptr--;
						mptr--;
					    if (tptr >= eptr->edata)
					    {
						if (caseless)
						{
						cval =(char)TOLOWER((int)*tptr);
						}
						else
						{
							cval = *tptr;
						}
					    }
					}
					if (mptr < match)
					{
						found = 1;
						start->id = eptr->ele_id;
						start->pos = (int)
						    (tptr - eptr->edata + 1);
						break;
					}
					else if (tptr < eptr->edata)
					{
						break;
					}
					else
					{
						equal = 0;
					}
				}
				else
				{
					mptr = mend;
					if (caseless)
					{
						cval =(char)TOLOWER((int)*tptr);
					}
					else
					{
						cval = *tptr;
					}
					while ((tptr >= eptr->edata)&&
						(cval != *mptr))
					{
						tptr--;
					    if (tptr >= eptr->edata)
					    {
						if (caseless)
						{
						cval =(char)TOLOWER((int)*tptr);
						}
						else
						{
							cval = *tptr;
						}
					    }
					}
					if ((tptr >= eptr->edata)&&
						(cval == *mptr))
					{
						equal = 1;
						end->id = eptr->ele_id;
						end->pos = (int)
						    (tptr - eptr->edata + 1);
					}
				}
			    }
			}
			/*
			 * Linefeeds match to single space characters.
			 */
			else if (eptr->type == E_LINEFEED)
			{
				if (equal)
				{
					if (*mptr == ' ')
					{
						mptr--;
						if (mptr < match)
						{
							found = 1;
							start->id =eptr->ele_id;
							start->pos = 0;
						}
					}
					else
					{
						equal = 0;
					}
				}
				else
				{
					mptr = mend;
					if (*mptr == ' ')
					{
						equal = 1;
						end->id = eptr->ele_id;
						end->pos = 0;
						mptr--;
						if (mptr < match)
						{
							found = 1;
							start->id =eptr->ele_id;
							start->pos = 0;
						}
					}
				}
			}
			if (found)
			{
				break;
			}
			eptr = eptr->prev;
		}
	}
	else /* forward */
	{
		found = 0;
		equal = 0;
		mptr = match;

		if (s_eptr != NULL)
		{
			eptr = s_eptr;
		}
		else
		{
			eptr = HTML_Data->formatted_elements;
		}

		while (eptr != NULL)
		{
			/*
			 * Skip the special internal text
			 */
			if (eptr->internal == True)
			{
				eptr = eptr->next;
				continue;
			}

			if (eptr->type == E_TEXT)
			{
			    tptr = eptr->edata;
			    if (eptr == s_eptr)
			    {
				tptr = (char *)(tptr + s_pos);
			    }
			    while (*tptr != '\0')
			    {
				if (equal)
				{
					if (caseless)
					{
						cval =(char)TOLOWER((int)*tptr);
					}
					else
					{
						cval = *tptr;
					}
					while ((*mptr != '\0')&&
						(cval == *mptr))
					{
						tptr++;
						mptr++;
						if (caseless)
						{
						cval =(char)TOLOWER((int)*tptr);
						}
						else
						{
							cval = *tptr;
						}
					}
					if (*mptr == '\0')
					{
						found = 1;
						end->id = eptr->ele_id;
						end->pos = (int)
							(tptr - eptr->edata);
						break;
					}
					else if (*tptr == '\0')
					{
						break;
					}
					else
					{
						equal = 0;
					}
				}
				else
				{
					mptr = match;
					if (caseless)
					{
						cval =(char)TOLOWER((int)*tptr);
					}
					else
					{
						cval = *tptr;
					}
					while ((*tptr != '\0')&&
						(cval != *mptr))
					{
						tptr++;
						if (caseless)
						{
						cval =(char)TOLOWER((int)*tptr);
						}
						else
						{
							cval = *tptr;
						}
					}
					if (cval == *mptr)
					{
						equal = 1;
						start->id = eptr->ele_id;
						start->pos = (int)
							(tptr - eptr->edata);
					}
				}
			    }
			}
			else if (eptr->type == E_LINEFEED)
			{
				if (equal)
				{
					if (*mptr == ' ')
					{
						mptr++;
						if (*mptr == '\0')
						{
							found = 1;
							end->id = eptr->ele_id;
							end->pos = 0;
						}
					}
					else
					{
						equal = 0;
					}
				}
				else
				{
					mptr = match;
					if (*mptr == ' ')
					{
						equal = 1;
						start->id = eptr->ele_id;
						start->pos = 0;
						mptr++;
						if (*mptr == '\0')
						{
							found = 1;
							end->id = eptr->ele_id;
							end->pos = 0;
						}
					}
				}
			}
			if (found)
			{
				break;
			}
			eptr = eptr->next;
		}
	}

	if (found)
	{
		m_start->id = start->id;
		m_start->pos = start->pos;
		m_end->id = end->id;
		m_end->pos = end->pos;
	}

	if (caseless)
	{
		free(match);
	}

	if (found)
	{
		return(1);
	}
	else
	{
		return(-1);
	}
}

