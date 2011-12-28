// MDF - PORT FROM NCSA VERSION 2.1
// In progress...

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

#include "includes.h"
#include "HTML.h"
#include "mosaic.h"
#include "globals.h"
#include "protos.h"
#include <dos/dos.h>
#include <unistd.h>

#include "amosaic_cat.h"

extern IPTR HookEntry();

extern Widget toplevel;
extern mo_window *current_win;

typedef enum
{
	mo_plaintext = 0, mo_formatted_text, mo_html, mo_latex, mo_postscript,
	mo_mif
} mo_format_token;

static char default_save_dir[255] ;

const char *format_opts[4];

/* ------------------------------------------------------------------------ */
/* ----------------------------- SAVE WINDOW ------------------------------ */
/* ------------------------------------------------------------------------ */

/* ------------------------- mo_post_save_window -------------------------- */

static unsigned long save_win_cb(struct Hook *h, void *o, int *msg)
{
	mo_window *win = (mo_window *)h->h_Data;
	Object *window = (Object *)o;
	FILE *fp;
	char *fname = "NIL:";

	get(win->format_optmenu, MUIA_Cycle_Active, &(win->save_format));
	
	switch (*msg)
		{
		case 0:
			set(window, MUIA_Window_Open, FALSE);
			get(win->save_fname, MUIA_String_Contents, &fname);
			fp = fopen(fname, "w");
			if (!fp)
	{
		/* error */
	}
			else
	{
		if (win->save_format == mo_plaintext)
			{
				char *text = HTMLGetText (win->scrolled_win, 0);
				if (text)
		{
			fputs (text, fp);
			free (text);
		}
			}
		else if (win->save_format == mo_formatted_text)
			{
				char *text = HTMLGetText (win->scrolled_win, 1);
				if (text)
		{
			fputs (text, fp);
			free (text);
		}
			}
		else if (win->save_format == mo_postscript)
			{
				char *text = HTMLGetText (win->scrolled_win, 2);
				if (text)
		{
			fputs (text, fp);
			free (text);
		}
			}
		else if (win->current_node && win->current_node->text)
			{
				/* HTML source */
				fputs (win->current_node->text, fp);
			}
		fclose (fp);
		break;
	}
		case 1:
			set(window, MUIA_Window_Open, FALSE);
			break;
		}
	return (unsigned long) 0;
}


mo_status mo_post_save_window (mo_window *win)
{
	if (!win->save_win)
		{
			struct Hook *save_win_cb_hook = malloc(sizeof(struct Hook));
			Object *b_save, *b_clear, *b_cancel;

			save_win_cb_hook->h_Entry = HookEntry;
			save_win_cb_hook->h_SubEntry = save_win_cb;
			save_win_cb_hook->h_Data = win;

			format_opts[0]=GetamosaicString(MSG_REQ_SAVE_FILEFORMAT_PLAIN);
			format_opts[1]=GetamosaicString(MSG_REQ_SAVE_FILEFORMAT_FORMATTED);
			format_opts[2]=GetamosaicString(MSG_REQ_SAVE_FILEFORMAT_HTML);
			format_opts[3]=NULL;

			win->save_win = WindowObject,
				MUIA_Window_Title, GetamosaicString(MSG_REQ_SAVE_TITLE),
				MUIA_Window_RefWindow, win->view,
				MUIA_Window_ID,MAKE_ID('S','V','A','S'),
				WindowContents, VGroup,
					Child,ColGroup(2),
						Child, Label2(GetamosaicString(MSG_REQ_SAVE_FILEFORMAT)),
						Child, win->format_optmenu = CycleObject,
									 MUIA_Cycle_Entries, format_opts, End,
						Child, Label2(GetamosaicString(MSG_REQ_SAVE_FILE)),
						Child,PopaslObject,
							MUIA_Popstring_String,win->save_fname=StringObject,StringFrame,MUIA_String_MaxLen,512,End,
							MUIA_Popstring_Button,PopButton(MUII_PopDrawer),
							ASLFR_TitleText,GetamosaicString(MSG_REQ_SAVE_FILEREQ_TITLE),
							ASLFR_PositiveText,GetamosaicString(MSG_REQ_SAVE_FILEREQ_POSTEXT),
							ASLFR_DoSaveMode,TRUE,
							End,
						End,
					Child, HGroup,
						MUIA_Group_SameWidth,TRUE,
						Child, b_save = SimpleButton(GetamosaicString(MSG_REQ_SAVE_SAVE)),
						Child, b_clear = SimpleButton(GetamosaicString(MSG_REQ_SAVE_CLEAR)),
						Child, b_cancel = SimpleButton(GetamosaicString(MSG_REQ_SAVE_CANCEL)),
					End,
				End,
			End;

			DoMethod(win->save_win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
				 win->save_win, 3, MUIM_Set, MUIA_Window_Open, FALSE);
			DoMethod(b_save, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->save_win, 3, MUIM_CallHook, save_win_cb_hook, 0);
			DoMethod(b_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->save_win, 3, MUIM_CallHook, save_win_cb_hook, 1);
			DoMethod(b_clear, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->save_fname, 3, MUIM_Set, MUIA_String_Contents, NULL);
			DoMethod(win->save_fname, MUIM_Notify, MUIA_String_Acknowledge,
				 MUIV_EveryTime, win->save_win, 3, MUIM_CallHook,
				 save_win_cb_hook, 0);

			DoMethod(App, OM_ADDMEMBER, win->save_win);
		}

	if (!*default_save_dir) {
		if (Rdata.download_dir) strcpy(default_save_dir, Rdata.download_dir);
#ifdef HAVE_GETCD
		else getcd(0, default_save_dir);
#else
		else getcwd(default_save_dir,sizeof(default_save_dir)-1);
#endif
	}
	AddPart(default_save_dir, FilePart(win->cached_url), 200);
	set(win->save_fname, MUIA_String_Contents, default_save_dir);
	set(win->save_win, MUIA_Window_Open, TRUE);
	*(PathPart(default_save_dir)) = '\0';
	return mo_succeed;
}


/* ------------------------------------------------------------------------ */
		/* -------------------------- SAVEBINARY WINDOW --------------------------- */
/* ------------------------------------------------------------------------ */

char *temp_binary_fnam;

mo_status mo_post_savebinary_window (mo_window *win)
{
	extern char *url;
	struct FileRequester *fr;
	struct Window *w = NULL;
	int result;
	char *cmd = NULL, *urlname;
	
	if (!*default_save_dir) {
		if (Rdata.download_dir) strcpy(default_save_dir, Rdata.download_dir);
#ifdef HAVE_GETCD
		else getcd(0, default_save_dir);
#else
		else getcwd(default_save_dir,sizeof(default_save_dir)-1);
#endif
	}
	urlname = FilePart(url) ;
	fr = MUI_AllocAslRequestTags(ASL_FileRequest, ASLFR_TitleText, GetamosaicString(MSG_REQ_SAVEBINARY_TITLE),
						 TAG_DONE);
	GetAttr(MUIA_Window_Window, WI_Main, (IPTR *)(&w));
	result = MUI_AslRequestTags(fr, ASLFR_Window, w, ASLFR_PositiveText, GetamosaicString(MSG_REQ_SAVEBINARY_POSTEXT),
						ASLFR_DoSaveMode, TRUE,
						ASLFR_InitialDrawer, default_save_dir,
						ASLFR_InitialFile, urlname,
						TAG_DONE);
	if (result) {
		AddPart(default_save_dir, fr->rf_Dir, 226300);
		AddPart(default_save_dir, fr->rf_File, 200);
		cmd = (char *)malloc((strlen(temp_binary_fnam)+
				strlen(default_save_dir)+16));
		sprintf(cmd, "Copy %s %s", temp_binary_fnam, default_save_dir);
		system(cmd);
		strcpy(default_save_dir, fr->rf_Dir);	/* For next time */
		unlink(temp_binary_fnam);
	}
	free(temp_binary_fnam);
	free(cmd);
	
	if (fr) MUI_FreeAslRequest(fr);

	return mo_succeed;
}

void rename_binary_file (char *fnam)
{
	mo_window *win = current_win;
	temp_binary_fnam = strdup (fnam);
	mo_post_savebinary_window (win);
}

/* ----------------------- mo_post_open_window ------------------------ */

static unsigned long open_win_cb(struct Hook *h, void *o, int *msg)
{
	mo_window *win = (mo_window *)h->h_Data;
	Object *window = (Object *)o;
	char *url = "";

	set(window, MUIA_Window_Open, FALSE);

	switch (*msg)
		{
		case 0:
			get(win->open_text, MUIA_String_Contents, &url);
			mo_convert_newlines_to_spaces (url);
			if (!strstr (url, ":"))
				{
					char *freeme = url;
					url = mo_url_canonicalize_local (url);
					free (freeme);
				}
			mo_load_window_text (win, url, NULL);
			/* The following free breaks things under the following conditions:

				 Start Mosaic with home page something that can't be fetched.
				 Interrupt, hit 'open', enter a URL, fetch document.
				 Follow relative link. */
			/* free (url); */
			break;
		case 1:
			/* Do nothing. */
			break;
		}

	return (unsigned long)0;
}

mo_status mo_post_open_window (mo_window *win)
{
	if (!win->open_win)
		{
			struct Hook *open_win_cb_hook = malloc(sizeof(struct Hook));
			Object *b_open, *b_clear, *b_cancel;
			
			open_win_cb_hook->h_Entry = HookEntry;
			open_win_cb_hook->h_SubEntry = open_win_cb;
			open_win_cb_hook->h_Data = win;

			/* Create it for the first time. */
			win->open_win = WindowObject,
				MUIA_Window_Title, GetamosaicString(MSG_REQ_URL_TITLE),
				MUIA_Window_RefWindow, win->view,
				MUIA_Window_ID,MAKE_ID('O','U','R','L'),
				WindowContents, VGroup,
					Child, HGroup,
						Child, Label2(GetamosaicString(MSG_REQ_URL_URL)),
						Child, win->open_text = StringObject, StringFrame,
									 MUIA_String_MaxLen, 512, End,
					End,
					Child, HGroup,
						Child, b_open = SimpleButton(GetamosaicString(MSG_REQ_URL_OPEN)),
						Child, b_clear = SimpleButton(GetamosaicString(MSG_REQ_URL_CLEAR)),
						Child, b_cancel = SimpleButton(GetamosaicString(MSG_REQ_URL_CANCEL)),
					End,
				End,
			End;

			DoMethod(win->open_win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
				 win->open_win, 3, MUIM_Set, MUIA_Window_Open, FALSE);
			DoMethod(b_open, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->open_win, 3, MUIM_CallHook, open_win_cb_hook, 0);
			DoMethod(b_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->open_win, 3, MUIM_CallHook, open_win_cb_hook, 1);
			DoMethod(b_clear, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->open_text, 3, MUIM_Set, MUIA_String_Contents, NULL);
			DoMethod(win->open_text, MUIM_Notify, MUIA_String_Acknowledge,
				 MUIV_EveryTime, win->open_win, 3, MUIM_CallHook,
				 open_win_cb_hook, 0);
			DoMethod(win->open_win, MUIM_Window_SetCycleChain,win->open_text,b_open,b_clear,b_cancel,NULL);
	
			DoMethod(App, OM_ADDMEMBER, win->open_win);
		}

	{
		char *url = "";
		get(TX_URL,MUIA_String_Contents,&url);
		set(win->open_text,MUIA_String_Contents,url);
	}		 

	set(win->open_win, MUIA_Window_ActiveObject, win->open_text);
	set(win->open_win, MUIA_Window_Open, TRUE);

	return mo_succeed;
}


/* ----------------------- mo_post_source_window ------------------------ */

mo_status mo_post_source_window (mo_window *win)
{
	if (!win->source_win)
		{
			/* Create it for the first time. */
			win->source_win = WindowObject,
				MUIA_Window_Title, GetamosaicString(MSG_WIN_SOURCE_TITLE),
				MUIA_Window_ID, MAKE_ID('S','R','C','E'),
				MUIA_Window_RefWindow, win->view,
//rs				MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Centered,
//rs				MUIA_Window_TopEdge, MUIV_Window_TopEdge_Centered,
//rs				MUIA_Window_Width, 400,
//rs				MUIA_Window_Height, 150,
				WindowContents, VGroup,
					Child, win->source_text = ListviewObject, MUIA_Listview_Input,FALSE,MUIA_Listview_List,
								 FloattextObject, ReadListFrame, End,
					End,
				End,
			End;

			DoMethod(win->source_win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
				 win->source_win, 3, MUIM_Set, MUIA_Window_Open, FALSE);

			DoMethod(App, OM_ADDMEMBER, win->source_win);
		}

	set(win->source_win, MUIA_Window_Open, TRUE);

	if (win->current_node)
		{
			set(win->source_text, MUIA_Floattext_Text, win->current_node->text);
		}
	return mo_succeed;
}



/* ----------------------- mo_post_search_window ------------------------ */

static unsigned long search_win_cb(struct Hook *h, void *o, int *msg)
{
	mo_window *win = (mo_window *)h->h_Data;

	switch (*msg)
		{
		case 0: /* Search */
			{
				char *str = NULL;

	get(win->search_win_text, MUIA_String_Contents, &str);

//	TO_HTML_SetSearch();

//#if FALSE /*	above fn HAS TO DO */

				if (str && *str)
					{
						int backward = 0, caseless = 0, rc;

			get(win->search_backwards_toggle, MUIA_Selected, &backward);
			get(win->search_caseless_toggle, MUIA_Selected, &caseless);

						if (!backward)
							{
								/* Either win->search_start->id is 0, in which case the search
									 should start from the beginning, or it's non-0, in which case
									 at least one search step has already been taken.
									 If the latter, it should be incremented so as to start
									 the search after the last hit.	Right? */
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
			MUI_RequestA(App, WI_Main, 0,
						 GetamosaicString(MSG_REQ_FINDRESULT_TITLE), GetamosaicString(MSG_REQ_FINDRESULT_OK),
						 GetamosaicString(MSG_REQ_FINDRESULT_NO_MORE_MATCHES),
						 NULL) ;
								else
			MUI_RequestA(App, WI_Main, 0,
						 GetamosaicString(MSG_REQ_FINDRESULT_TITLE), GetamosaicString(MSG_REQ_FINDRESULT_OK),
						 GetamosaicString(MSG_REQ_FINDRESULT_NO_MATCHES),
						 NULL) ;
							}
						else
							{
								/* Now search_start and search_end are starting and ending
									 points of the match. */
		HTMLGotoId(win->scrolled_win,
			((ElementRef *)win->search_start)->id);			 
//		ResetAmigaGadgets();

								/* Set the selection. */
#if 0 /* Needs to wait for HTMLSetSelection to work */
								HTMLSetSelection (win->scrolled_win, (ElementRef *)win->search_start,
																	(ElementRef *)win->search_end);
#endif
							} /* found a target */
					} /* if str && *str */
//#endif
			} /* case */
			break;

		case 1: /* Clear */
			/* Clear out the search text */
			set(win->search_win_text, MUIA_String_Contents, NULL);

			/* Subsequent searches start at the beginning. */
			TO_HTML_reset_search(win);
			/*
			((ElementRef *)win->search_start)->id = 0;
			*/
			/* Reposition document at top of screen. */
			HTMLGotoId(win->scrolled_win, 0);
/*			HTMLGotoId(win, 0); */
			break;
		}

	return (unsigned long)0;
}

mo_status mo_post_search_window (mo_window *win)
{
	if (!win->search_win)
		{
			struct Hook *search_win_cb_hook = malloc(sizeof(struct Hook));
			Object *b_find, *b_clear, *b_cancel;
			
			search_win_cb_hook->h_Entry = HookEntry;
			search_win_cb_hook->h_SubEntry = search_win_cb;
			search_win_cb_hook->h_Data = win;

			/* Create it for the first time. */
			win->search_win = WindowObject,
				MUIA_Window_Title, GetamosaicString(MSG_REQ_FIND_TITLE),
				MUIA_Window_ID,MAKE_ID('F','I','N','D'),
				MUIA_Window_RefWindow, win->view,
				WindowContents, VGroup,
					Child, HGroup,
						Child, Label2(GetamosaicString(MSG_REQ_FIND_STRING)),
						Child, win->search_win_text = StringObject, StringFrame,
									 MUIA_String_MaxLen, 512, End,
					End,
					Child, HGroup,
						Child, Label1(GetamosaicString(MSG_REQ_FIND_CASELESS)),
						Child, win->search_caseless_toggle = CheckMark(TRUE),
						Child, Label1(GetamosaicString(MSG_REQ_FIND_BACKWARDS)),
						Child, win->search_backwards_toggle = CheckMark(FALSE),
					End,
					Child, HGroup,
						Child, b_find = SimpleButton(GetamosaicString(MSG_REQ_FIND_FIND)),
						Child, b_clear = SimpleButton(GetamosaicString(MSG_REQ_FIND_CLEAR)),
						Child, b_cancel = SimpleButton(GetamosaicString(MSG_REQ_FIND_CANCEL)),
					End,
				End,
			End;

			DoMethod(win->search_win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
				 win->search_win, 3, MUIM_Set, MUIA_Window_Open, FALSE);
			DoMethod(b_find, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->search_win, 3, MUIM_CallHook, search_win_cb_hook, 0);
			DoMethod(b_clear, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->search_win, 3, MUIM_CallHook, search_win_cb_hook, 1);
			DoMethod(b_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
				 win->search_win, 3, MUIM_Set, MUIA_Window_Open, FALSE);
			DoMethod(win->search_win_text, MUIM_Notify, MUIA_String_Acknowledge,
				 MUIV_EveryTime, win->search_win, 3, MUIM_CallHook,
				 search_win_cb_hook, 0);
			DoMethod(win->search_win_text, MUIM_Notify, MUIA_String_Acknowledge,
				 MUIV_EveryTime, win->search_win, 3, MUIM_Set,
				 MUIA_Window_ActiveObject, b_find );
			DoMethod(win->search_win,MUIM_Window_SetCycleChain,win->search_win_text,win->search_caseless_toggle,
				win->search_backwards_toggle,b_find,b_clear,b_cancel,NULL);
			DoMethod(App, OM_ADDMEMBER, win->search_win);
		}

	set(win->search_win, MUIA_Window_Open, TRUE);
	set(win->search_win, MUIA_Window_ActiveObject,win->search_win_text);
	return mo_succeed;
}
