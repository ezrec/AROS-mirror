/* THIS FILE CONTAINS VERSIONS OF THINGS THAT USED TO TALK DIRECTLY TO THE 
   HTML GADGET'S INTERNALS, BUT THAT NOW USE METHODS TO DO THE SAME THING */
#include "includes.h"
#include "htmlgad.h"
#include "globals.h"
#include "HTML.h" /* but try to avoid using it */
#include "mosaic.h"
#include "protos.h"

/*
 * Convenience function to set the raw text into the widget.
 * Forces a reparse and a reformat.
 * If any pointer is passed in as NULL that text is unchanged,
 * if a pointer points to an empty string, that text is set to NULL;
 * Also pass an element ID to set the view area to that section of the new
 * text.  Finally pass an anchor NAME to set position of the new text
 * to that anchor.
 */

/* THIS IS TOO UGLY... WANT TO IMPLEMEMT IT AS A METHOD WHERE 
   WE SET THE THREE VISIBLE STRINGS, THEN CALL THE METHOD OT FUTZ WITH THE INTERNALS
*/
#if FALSE
HERE
void
TO_HTMLSetText(Object *w, char *text, char *header_text, char *footer_text, int element_id, char *target_anchor, WidgetInfo *ptr) /* last param was */ 
{
  WidgetInfo *wptr = (WidgetInfo *)ptr;
  struct ele_rec *start;
  struct ele_rec *eptr;
  int newy;
  
  if ((text == NULL)&&(header_text == NULL)&&(footer_text == NULL))
    {
      return;
    }
  
}

#endif

void *TO_HTMLMakeSpaceForElementRef(void){
  return malloc(sizeof (ElementRef));
}

extern mo_window window;

ULONG TO_HTMLGetScreenDepth(void)
{
	struct Screen *scr = NULL;
	get(window.view,MUIA_Window_Screen,&scr);
	if(!scr)
		return 3L;
	else
		return scr->RastPort.BitMap->Depth;
}

/*********************************************************************
  NAME: HTML_reset_search() (sb)

  PURPOSE: Reset the window search start.
*********************************************************************/
void TO_HTML_reset_search(mo_window *win)
{
/* something like this... */
 if (win->search_start)
  ((ElementRef *)win->search_start)->id = 0;
}

void TO_HTML_FlushImageCache(void)
{
	mo_flush_image_cache(&window);

	ReformatWindow((HTMLGadClData *)INST_DATA(HTMLGadClass,HTML_Gad));
	ResetAmigaGadgets();
	MUI_Redraw(HTML_Gad,MADF_DRAWOBJECT);
}

void TO_HTML_LoadInlinedImage(void)
{
	char *img = NULL;
	get(HTML_Gad, HTMLA_image_load, &img);
	LoadInlinedImage(img);
}

char **TO_HTMLGetImageSrcs (mo_window *win,int *num)
{
	HTMLGadClData *inst = NULL;
	get(HTML_Gad, HTMLA_inst, (ULONG *)(&inst));
	return HTMLGetImageSrcs(inst, num);
}

struct mark_up *TO_HTML_GetHTMLObjects(void)
{
	HTMLGadClData *inst=INST_DATA(HTMLGadClass,HTML_Gad);
	return inst->html_objects;
}

void TO_HTML_SetSearch(void)
{
#if FALSE /*	above fn HAS TO DO */
        if (str && *str)
          {
            int backward, caseless, rc;

	    get(win->search_backwards_toggle, MUIA_Selected, &backward);
	    get(win->search_caseless_toggle, MUIA_Selected, &caseless);

            if (!backward)
              {
                /* Either win->search_start->id is 0, in which case the search
                   should start from the beginning, or it's non-0, in which case
                   at least one search step has already been taken.
                   If the latter, it should be incremented so as to start
                   the search after the last hit.  Right? */
                if (((ElementRef *)win->search_start)->id)
                  {
                    ((ElementRef *)win->search_start)->id =
                      ((ElementRef *)win->search_end)->id;
                    ((ElementRef *)win->search_start)->pos =
                      ((ElementRef *)win->search_end)->pos;
                  }
                
              }

            rc = HTMLSearchText 
              (win->scrolled_win, str, 
               (ElementRef *)win->search_start, 
               (ElementRef *)win->search_end, 
               backward, caseless);

            if (rc == -1)
              {
                /* No match was found. */
                if (((ElementRef *)win->search_start)->id)
			MUI_Request(App, WI_Main, 0,
				    "Amiga Mosaic: Find Result", "*_OK",
				     "Sorry, no more matches in this document.",
				     TAG_END) ;
                else
			MUI_RequestA(App, WI_Main, 0,
				     "Amiga Mosaic: Find Result", "*_OK",
				     "Sorry, no matches in this document.",
				     TAG_END) ;
              }
            else
              {
                /* Now search_start and search_end are starting and ending
                   points of the match. */
		HTMLGotoId(win->scrolled_win,
			((ElementRef *)win->search_start)->id);	      
//		ResetAmigaGadgets();

                /* Set the selection. */
#if	0	/* Needs to wait for HTMLSetSelection to work */
                HTMLSetSelection (win->scrolled_win, (ElementRef *)win->search_start,
                                  (ElementRef *)win->search_end);
#endif
              } /* found a target */
          } /* if str && *str */
#endif
}

void ForgetALLHTMLAmigaData(void)
{
}
