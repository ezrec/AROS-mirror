/*
 * The Rexx interface code for Amiga Mosaic.
 * Copyright © 1994, Mike W. Meyer
 *
 * All items exported from this module will be prefixed with "Rexx". Currently,
 * those are:
 *
 * RexxCommands - the supported Command structure for the MUI Rexx port.
 * RexxLaunch - used to launch a rexx macro
 * RexxDispatch - Dispatches rexx messages coming in on our port.
 * RexxSignalBits - the bits we need to wait on for outstanding messages.
 *
 * static data items are all lower case, with "_" seperating words.
 */

#include "includes.h"
#include "HTML.h"
#include "mosaic.h"
#include "globals.h"
#include "htmlgad.h"
#include "protos.h"

#include <dos/dos.h>
#include <rexx/storage.h>
#include <proto/rexxsyslib.h>

#include "amosaic_cat.h"

#define RETURN_BAD_ARGS	15	/* Bogus arguments in command */
#define REXXNAME	"www"	/* Our macro extension */
#define MAXMACRO	13	/* Maximum number of user macros */
#define MAXUSERMACRO	9	/* last macro with a menu label */

static char *fetch_title(struct mark_up *, char *) ;
static int stem_hrefs(struct mark_up *, char *, char *, int, struct Message *) ;
static int stem_images(struct mark_up *, char *, char *, int, struct Message *) ;

extern ULONG HookEntry() ;

extern char *home_document ;
extern mo_window window ;	/* Change this later */

/*
 * The Hooks that MUI calls when it gets a Rexx command.
 */

/* Fetch something from the WWW */
#define FETCH_OPTS	"Item,Stem,URL/k,Index/k/f,Map/k/f,Form/k/f"
enum { fetch_item, fetch_stem, fetch_url,
	fetch_index, fetch_map, fetch_form, fetch_count} ;
static ULONG Fetch(struct Hook *h, void *o, ULONG *parms) ;
static struct Hook fetch_hook = { {0}, HookEntry, &Fetch, &window} ;

/* Get information from Mosaic */
#define GET_OPTS	"Item/a,Stem"
enum { get_item, get_stem, get_count} ;
static ULONG Get(struct Hook *h, void *o, ULONG *parms) ;
static struct Hook get_hook = { {0}, HookEntry, &Get, &window} ;

/* Jump to a document */
#define JUMP_OPTS	"Item,URL/k,Index/k/f,Map/k/f,Form/k/f"
enum { jump_item, jump_url, jump_index, jump_map, jump_form, jump_count} ;
static ULONG Jump(struct Hook *h, void *o, ULONG *parms) ;
static struct Hook jump_hook = { {0}, HookEntry, &Jump, &window} ;

/* Open a dialog window  (Open is in dos_protos, so...) */
#define OPEN_OPTS	"Item"
enum { open_item, open_count} ;
static ULONG www_open(struct Hook *h, void *o, ULONG *parms) ;
static struct Hook open_hook = { {0}, HookEntry, &www_open, NULL} ;

/* Twiddle misc. crap */
#define OPTION_OPTS	"Item/a,To/n"
enum { option_item, option_to, option_count } ;
static ULONG Option(struct Hook *h, void *o, ULONG *parms) ;
static struct Hook option_hook = { {0}, HookEntry, &Option, &window} ;

/* Manipulate the screen */
#define SCREEN_OPTS	"Item/a"
enum {screen_item, screen_count} ;
static ULONG Screen(struct Hook *h, void *o, ULONG *parms) ;
static struct Hook screen_hook = { {0}, HookEntry, &Screen, &window} ;

/* Manipulate the window */
#define WINDOW_OPTS	"Item/a"
enum {window_item, window_count} ;
static ULONG Window(struct Hook *h, void *o, ULONG *parms) ;
static struct Hook window_hook = { {0}, HookEntry, &Window, &window} ;

/*
 * The structure we pass to MUI so it can call all of the above things.
 */
struct MUI_Command RexxCommands[] = {
	{ "fetch",	FETCH_OPTS,	fetch_count,	&fetch_hook} ,
	{ "get",	GET_OPTS,	get_count,	&get_hook} ,
	{ "jump",	JUMP_OPTS,	jump_count,	&jump_hook} ,
	{ "open",	OPEN_OPTS,	open_count,	&open_hook} ,
	{ "option",	OPTION_OPTS,	option_count,	&option_hook} ,
	{ "screen",	SCREEN_OPTS,	screen_count,	&screen_hook} ,
	{ "window",	WINDOW_OPTS,	window_count,	&window_hook} ,
	{ NULL,		NULL,		NULL,	NULL } ,
	} ;

/*
 * The code that implements the hooks.
 */

/* Display the named url in the given window */
static enum index_types { index_index, index_map, index_form} ;

static char *
make_url(char *url, char *index, int type) {
	char *freeme = NULL, *out ;
	unsigned x, y ;

	/* Deal with the index */
	if (index) {
		strtok(url, "#") ;
		strtok(url, "?") ;
		switch (type) {
			case index_index:
				freeme = mo_escape_part(index) ;
				break ;

			case index_map:
				freeme = malloc(24) ;
				sscanf(index, "%u %u", &x, &y) ;
				sprintf(freeme, "%u,%u", x, y) ;
				break ;

			case index_form:
				return NULL ;
				/* Not implemented yet */
				break ;

			default: return NULL ;	/* Oops */
			}
		out = malloc(strlen(url) + strlen(freeme) + 2) ;
		sprintf(out, "%s?%s", url, freeme) ;
		free(freeme) ;
		freeme = url = out ;
		}
	
	mo_convert_newlines_to_spaces(url) ;
	url = mo_url_canonicalize(url, NULL) ;
	if (freeme) free(freeme) ;
	return  url ;
	}


/* Set a stem variable's 0 index to the given count */
static void
stem_count(struct Message *msg, char *stem, int count) {
	char var[1024], value[12] ;

	sprintf(var, "%s.0", stem) ;
	sprintf(value, "%d", count) ;
	SetRexxVar(msg, var, value, strlen(value)) ;
	}

/* Fetch information from the Web */
static ULONG
Fetch(struct Hook *h, void *o, ULONG *parms) {
	int count ;
	char *item = (char *) parms[fetch_item] ;
	char *stem = (char *) parms[fetch_stem] ;
	char *url =  (char *) parms[fetch_url] ;
	char *html, *oldurl, *fetched_html, *title, *freeme = NULL, buffer[1024] ;
	mo_window *window = (mo_window *) h->h_Data ;
	Object *app = (Object *) o ;
	struct mark_up *objects ;
	struct Message *msg ;
	/* Free'd next time we get run; maybe use as a cache? */
	static struct mark_up *fetched_objects = NULL ;
	
	/* Inline is an oddball - we have to do it before the URL handling */
	if (!stricmp("inline", item)) {
		ImageInfo *img ;
		char buffer2[15] ;

		if (!url) return RETURN_BAD_ARGS ;
		LoadInlinedImage(url) ;
		if (!(img = mo_fetch_cached_image_data(url))) return RETURN_FAIL ;

		get(app, MUIA_Application_RexxMsg, &msg) ;
		sprintf(buffer, "%s.URL", stem) ;
		SetRexxVar(msg, buffer, url, strlen(url)) ;
		sprintf(buffer, "%s.ISMAP", stem) ;
		SetRexxVar(msg, buffer, img->ismap ? "1" : "0", 1) ;
		sprintf(buffer, "%s.CURRENT", stem) ;
		SetRexxVar(msg, buffer, "1", 1) ;
		sprintf(buffer, "%s.WIDTH", stem) ;
		sprintf(buffer2, "%d", img->width) ;
		SetRexxVar(msg, buffer, buffer2, strlen(buffer2)) ;
		sprintf(buffer, "%s.HEIGHT", stem) ;
		sprintf(buffer2, "%d", img->height) ;
		SetRexxVar(msg, buffer, buffer2, strlen(buffer2)) ;
		sprintf(buffer, "%s.FILENAME", stem) ;
		SetRexxVar(msg, buffer, img->fname, strlen(img->fname)) ;
		return RETURN_OK ;
		}

	/* Get the HTML & objects for it. */
	if (!url) {
		extern int binary_transfer ;
//mjw		HTMLRec *inst ;

		oldurl = window->current_node->url ;
		html = window->current_node->text ;
//mjw		get(HTML_gad, HTML_inst, &inst) ;
//mjw		objects = inst->html->html_objects ;
                objects = TO_HTML_GetHTMLObjects();
		}
	else {
		int old_binary = binary_transfer ;
		char *index =  (char *) parms[fetch_index] ;
		char *form =  (char *) parms[fetch_form] ;
		char *map =  (char *) parms[fetch_map] ;

		oldurl = url ;
		if (index) url = make_url(url, index, index_index) ;
		else if (form) url = make_url(url, form, index_form) ;
		else if (map) url = make_url(url, map, index_map) ;
		else url = make_url(url, NULL, index_index) ;

		binary_transfer = 0 ;
		html = mo_pull_er_over(url, &fetched_html) ;
		binary_transfer = old_binary ;
		objects = fetched_objects = HTMLParse(fetched_objects, html) ;
		}

	if (!stricmp("title", item))
		set(app, MUIA_Application_RexxString,
			url ? freeme = fetch_title(objects, url)
			    : window->current_node->title) ;
	else if (!stricmp("source", item))
		set(app, MUIA_Application_RexxString, html) ;

	/* Body and head are waiting for an upgrade to the web */
	else if (!stricmp("body", item)) return RETURN_FAIL ;
	else if (!stricmp("head", item)) return RETURN_FAIL ;

	/* The rest of the things need the RVI */
	else if (!stem) return RETURN_BAD_ARGS ;
	else if (!stricmp("links", item)) {
		get(app, MUIA_Application_RexxMsg, &msg) ;
		stem_count(msg, stem, stem_hrefs(objects, stem, oldurl, 0, msg)) ;
		}
	else if (!stricmp("images", item)) {
		get(app, MUIA_Application_RexxMsg, &msg) ;
		stem_count(msg, stem, stem_images(objects, stem, oldurl, 0, msg)) ;
		}
	else if (!stricmp("urllist", item)) {
		get(app, MUIA_Application_RexxMsg, &msg) ;
		count = stem_hrefs(objects, stem, oldurl, 0, msg) ;
		stem_count(msg, stem, stem_images(objects, stem, oldurl, count, msg)) ;
		}
	else if (!stricmp("document", item)) {
		get(app, MUIA_Application_RexxMsg, &msg) ;

		title = url ? freeme = fetch_title(objects, url) : window->current_node->title,
		sprintf(buffer, "%s.TITLE", stem) ;
		SetRexxVar(msg, buffer, title, strlen(title)) ;
		sprintf(buffer, "%s.SOURCE", stem) ;
		SetRexxVar(msg, buffer, html, strlen(html)) ;
		sprintf(buffer, "%s.TYPE", stem) ;
		SetRexxVar(msg, buffer, "text/html", 9) ;

		sprintf(buffer, "%s.URLLIST", stem) ;
		count = stem_hrefs(objects, buffer, oldurl, 0, msg) ;
		stem_count(msg, buffer, stem_images(objects, buffer, oldurl, count, msg)) ;
		}
	else return RETURN_BAD_ARGS ;

	if (freeme) free(freeme) ;
	if (fetched_html) free(fetched_html) ;
	return RETURN_OK ;
	}

/* Get information from Mosaic */
static ULONG
Get(struct Hook *h, void *o, ULONG *parms) {
	int count ;
	struct Message *msg ;
	char *item = (char *) parms[get_item] ;
	char *stem = (char *) parms[get_stem] ;
	mo_window *window = (mo_window *) h->h_Data ;
	Object *app = (Object *) o ;
	Object *win = window->view ;
	char buffer[1024], *freeme = NULL ;

	if (!stricmp("url", item))
		set(app, MUIA_Application_RexxString, window->current_node->url) ;
	else if (!stricmp("home", item))
		set(app, MUIA_Application_RexxString, home_document) ;
	else if (!stricmp("text", item))
		set(app, MUIA_Application_RexxString,
			 freeme = HTMLGetText(window->scrolled_win, 0)) ;
	else if (!stricmp("formatted", item))
		set(app, MUIA_Application_RexxString,
			 freeme = HTMLGetText(window->scrolled_win, 1)) ;
	else if (!stricmp("postscript", item))
		set(app, MUIA_Application_RexxString,
			 freeme = HTMLGetText(window->scrolled_win, 2)) ;
	else if (!stricmp("window", item)) {
		unsigned long top, left, width, height ;
		char *tmp ;

		get(win, MUIA_Window_TopEdge, &top) ;
		get(win, MUIA_Window_LeftEdge, &left) ;
		get(win, MUIA_Window_Height, &height) ;
		get(win, MUIA_Window_Width, &width) ;
		get(win, MUIA_Window_Title, &tmp) ;
		sprintf(buffer, "%u/%u/%u/%u/%s", left, top, width, height, tmp) ;
		set(app, MUIA_Application_RexxString, buffer) ;
		}
	else if (!stricmp("screen", item)) {
		return RETURN_BAD_ARGS ;
/* Sigh - this was supposed to work in 2.0, but doesn't
		get(win, MUIA_Window_PublicScreen, &tmp) ;
		set(app, MUIA_Application_RexxString, tmp) ;
*/
		}
	/* No selection in Amiga Mosaic yet */
	else if (!stricmp("selection", item)) return RETURN_FAIL ;
	else if (!stricmp("anchor", item)) return RETURN_FAIL ;

	/* hotlist & history require a stem */
	else if (!stem) return RETURN_BAD_ARGS ;
	else if (!stricmp("history", item)) {
		mo_node *node ;

		get(app, MUIA_Application_RexxMsg, &msg) ;
		for (count = 1, node = window->history; node != NULL;
		    node = node->next, count += 1) {
			sprintf(buffer, "%s.%d.URL", stem, count) ;
			SetRexxVar(msg, buffer, node->url, strlen(node->url)) ;
			sprintf(buffer, "%s.%d.REF", stem, count) ;
			SetRexxVar(msg, buffer, node->title, strlen(node->title)) ;
			}
		stem_count(msg, stem, count - 1) ;
		}
	/* No hotlist, either */
	else if (!stricmp("hotlist", item)) return RETURN_FAIL ;
	else return RETURN_BAD_ARGS ;

	if (freeme) free(freeme) ;
	return RETURN_OK ;
	}

/* Jump to different things on the WWW. */
static ULONG
Jump(struct Hook *h, void *o, ULONG *parms) {
	int doc;
	char *item = (char *) parms[jump_item] ;
	char *url = (char *) parms[jump_url] ;
	char *index = (char *) parms[jump_index] ;
	char *map = (char *) parms[jump_map] ;
	char *form = (char *) parms[jump_form] ;

	/* Must specify exactly one of item or url */
	if (url) {
		if (item) return RETURN_BAD_ARGS ;
		if (index) url = make_url(url, index, index_index) ;
		else if (form) url = make_url(url, form, index_form) ;
		else if (map) url = make_url(url, map, index_map) ;
		else url = make_url(url, NULL, index_index) ;
		if (url) mo_load_window_text((mo_window *) h->h_Data, url, NULL) ;
		else return RETURN_BAD_ARGS ;
		}
	else if (item) {
		if (!stricmp("home", item)) doc = mo_home_document ;
		else if (!stricmp("back", item)) doc = mo_back ;
		else if (!stricmp("forward", item)) doc = mo_forward ;
		else if (!stricmp("reload", item)) doc = mo_reload_document ;
		else return RETURN_BAD_ARGS ;
		DoMethod((Object *) o, MUIM_Application_ReturnID, doc) ;
		}
	else return RETURN_BAD_ARGS ;

	return RETURN_OK ;
	}

/* Open various objects */
static ULONG
www_open(struct Hook *h, void *o, ULONG *parms) {
	int doc ;
	char *item = (char *)parms[open_item] ;
	Object *app = (Object *) o ;

	if (!item) doc = mo_open_document ;
	else if (!stricmp("local", item)) doc = mo_open_local_document ;
	else if (!stricmp("source", item)) doc = mo_document_source ;
	else if (!stricmp("history", item)) doc = mo_history_list ;
	else if (!stricmp("about", item)) doc = mo_about ;
	else if (!stricmp("saveas", item)) doc = mo_save_document ;
	else return RETURN_BAD_ARGS ;

	DoMethod(app, MUIM_Application_ReturnID, doc) ;
	return RETURN_OK ;
	}

/* Twiddle Misc. crap */
static ULONG
Option(struct Hook *h, void *o, ULONG *parms) {
	char *item = (char *)parms[option_item] ;
	ULONG *to = (ULONG *)parms[option_to] ;
	Object *app = (Object *) o ;
	mo_window *window = (mo_window *)h->h_Data ;
	char buffer[15] ;

	if (!stricmp("clear", item)) return RETURN_FAIL ;
	else if (!stricmp("flush", item))
		DoMethod(app, MUIM_Application_ReturnID, mo_clear_image_cache) ;
	else if (!stricmp("load", item)) LoadInlinedImages() ;
	else if (!stricmp("delay", item)) {
		if (!to) {
			sprintf(buffer, "%d", window->delay_image_loads) ;
			set(app, MUIA_Application_RexxString, buffer) ;
			}
		else {
			window->delay_image_loads = *to ;
			DoMethod(app, MUIM_Application_SetMenuCheck,
					mo_delay_image_loads, *to) ;
			}
		}
	else if (!stricmp("binary", item)) {
		if (!to) {
			sprintf(buffer, "%d", window->binary_transfer) ;
			set(app, MUIA_Application_RexxString, buffer) ;
			}
		else {
			window->binary_transfer = *to ;
			DoMethod(app, MUIM_Application_SetMenuCheck,
					mo_binary_transfer, *to) ;
			}
		}
	else if (!stricmp("offset", item)) {
		if (!window->current_node) return RETURN_FAIL ;
		if (to) {
			window->current_node->docid = *to ;
			ResetAmigaGadgets() ;
			}
		else {
			sprintf(buffer, "%d", window->current_node->docid) ;
			set(app, MUIA_Application_RexxString, buffer) ;
			}
		}
	else return RETURN_BAD_ARGS ;

	return RETURN_OK ;
	}

/* Manipulate the Mosaic Screen */
static ULONG
Screen(struct Hook *h, void *o, ULONG *parms) {
	unsigned left, top ;
	char *item = (char *) parms[screen_item] ;
	Object *win = ((mo_window *)h->h_Data)->view ;
	struct Screen *scr ;

	if (!stricmp("front", item))
		DoMethod(win, MUIM_Window_ScreenToFront) ;
	else if (!stricmp("back", item))
		DoMethod(win, MUIM_Window_ScreenToBack) ;
	else {
		get(win, MUIA_Window_Screen, &scr) ;
		sscanf((char *) item, "%u/%u", &left, &top) ;
		if (IntuitionBase->LibNode.lib_Version >= 39)
			ScreenPosition(scr, SPOS_ABSOLUTE, left, top, 0, 0) ;
		else return RETURN_FAIL ;
		}

	return RETURN_OK ;
	}

/* Manipulate the Mosaic window */
static ULONG
Window(struct Hook *h, void *o, ULONG *parms) {
	unsigned left, top, width, height ;
	char *item = (char *) parms[window_item] ;
	Object *win = ((mo_window *)h->h_Data)->view ;
	struct Window *w ;

	get(win, MUIA_Window_Window, &w) ;
	if (!stricmp("front", item)) DoMethod(win, MUIM_Window_ToFront) ;
	else if (!stricmp("back", item)) DoMethod(win, MUIM_Window_ToBack) ;
	else if (!stricmp("zip", item)) ZipWindow(w) ;
	else {
		sscanf((char *) item, "%u/%u/%u/%u",
				&left, &top, &width, &height) ;
		ChangeWindowBox(w, left, top, width, height) ;
		}

	return RETURN_OK ;
	}

/*
 * Rexx macro support.
 */
ULONG RexxSignalBits ;

static struct MsgPort *port = NULL ;
static int replies_needed = 0 ;

/* Clean up the port if we're through with it */
static void
delete_port(void) {

	if (replies_needed) return ;
	DeletePort(port) ;
	port = 0 ;
	RexxSignalBits = 0 ;
	}

/* actually run a rexx command */
static ULONG
run_command(char *command) {
	struct MsgPort *rexx ;
	struct RexxMsg *out ;
	char *muiname ;

	/* Make sure we have a port for this */
	if (!(port || (port = CreatePort(NULL, 0L)))) return RETURN_FAIL ;

	/* Set up the message to run */
	get(App, MUIA_Application_Base, &muiname) ;
	if (!(out = CreateRexxMsg(port, REXXNAME, muiname))) {
		delete_port() ;
		return RETURN_FAIL ;
		}
	out->rm_Action = RXCOMM ;
	out->rm_Args[0] = CreateArgstring(command, strlen(command)) ;

	/* Go do it to it */
	Forbid() ;
	if ((rexx = FindPort("REXX"))) PutMsg(rexx, (struct Message *) out) ;
	Permit() ;

	if (rexx) {
		replies_needed += 1 ;
		RexxSignalBits = (1l << port->mp_SigBit) ;
		return RETURN_OK ;
		}
	else {
		DeleteRexxMsg(out) ;
		delete_port() ;
		return RETURN_FAIL ;
		}
	}

/*
 * Menu strip settings - the labels are in the NewMenu structure. Since those
 * aren't dynamic in any sane sense of the word, and C constants are wimpy, we
 * wind up plugging them directly into the structure, knowing the offset.  Yuch.
 */

#define MENU_OFFSET 37

/* Pointers to the strings we're going to run as macros */
static char *rexx_macro[MAXMACRO] ;

void
RexxMenu(int item, char *string) {
	char *label, *command, *p ;

	extern char *RexxMacroNames[10];

	item -= mo_macro_1 ;
	if (item < 0 || MAXMACRO <= item) return ;

	label = strdup(string) ;	/* My own copy to work with */

	if (item > MAXUSERMACRO) {
		rexx_macro[item] = label ;
		return ;
	}

	if (!(command = strchr(label, '|'))) {
		RexxMacroNames[item]=strdup("~");
		return ;
	}

	*command++ = '\0' ;
	command = stpblk(command) ; 

	RexxMacroNames[item]=strdup(label);

	rexx_macro[item] = command ;
	}

/*
 * RexxLaunch - used to launch macros from inside of MUI. For now, we don't
 * deal cleanly with having multiple macros running. If all this works
 * (I.e. - MUI doesn't hose us yet again), I'll fix that.
 */

/* The callback hook for macros launched from the Rexx->Macro menu. */
static ULONG
macro_callback(struct Hook *h, void *o, int *msg) {
	mo_window *win = (mo_window *)h->h_Data ;
	Object *window = (Object *)o ;
	struct Window *w = NULL ;
	char *string, buffer[255] ;
	struct FileRequester *fr ;

	switch (*msg) {
	case 0:	/* Run*/
		set(window, MUIA_Window_Open, FALSE) ;
		get(win->macro_string, MUIA_String_Contents, &string) ;
		run_command(string) ;
		break ;

	case 1:	/* Cancel */
		set(window, MUIA_Window_Open, FALSE) ;
		break ;

		}
	return 0 ;
	}

ULONG
RexxLaunch(int which, mo_window *win) {

	/* Now, figure out what to do */
	if (which < -1 || MAXMACRO <= which) return RETURN_ERROR ;

	if (which >= 0 && rexx_macro[which])
		return run_command(rexx_macro[which]) ;
	else if (which != -1) return RETURN_ERROR ;

	/* Going to get a macro from the user */
	if (!win->macro_win) {
		struct Hook *hook = malloc(sizeof(struct Hook)) ;
		Object *b_run, *b_clear, *b_cancel ;

		hook->h_Entry = HookEntry ;
		hook->h_SubEntry = macro_callback ;
		hook->h_Data = win ;

		win->macro_win = WindowObject ,
			MUIA_Window_Title, GetamosaicString(MSG_REQ_MACRO_TITLE),
			MUIA_Window_RefWindow, win->view,
			MUIA_Window_ID,MAKE_ID('R','E','X','X'),
			WindowContents, VGroup,
				Child, HGroup,
					Child, Label2(GetamosaicString(MSG_REQ_MACRO_MACRO)),
					Child,PopaslObject,
						MUIA_Popstring_String,win->macro_string=StringObject,StringFrame,MUIA_String_MaxLen,512,MUIA_String_Contents,"rexx:",End,
						MUIA_Popstring_Button,PopButton(MUII_PopDrawer),
						ASLFR_TitleText,GetamosaicString(MSG_REQ_MACRO_FILEREQ_TITLE),
						ASLFR_DoPatterns,TRUE,
						ASLFR_InitialPattern,"#?."REXXNAME,
						End,
					End,
				Child, HGroup,
					MUIA_Group_SameWidth,TRUE,
					Child, b_run = SimpleButton(GetamosaicString(MSG_REQ_MACRO_RUN)),
					Child, b_clear = SimpleButton(GetamosaicString(MSG_REQ_MACRO_CLEAR)),
					Child, b_cancel = SimpleButton(GetamosaicString(MSG_REQ_MACRO_CANCEL)),
					End,
				End,
			End;
		DoMethod(win->macro_win, MUIM_Notify,
			MUIA_Window_CloseRequest, TRUE,
			win->macro_win, 3, MUIM_Set, MUIA_Window_Open, FALSE);
		DoMethod(b_run, MUIM_Notify, MUIA_Pressed, FALSE,
			win->macro_win, 3, MUIM_CallHook, hook, 0);
		DoMethod(b_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
			win->macro_win, 3, MUIM_CallHook, hook, 1);
		DoMethod(b_clear, MUIM_Notify, MUIA_Pressed, FALSE,
			win->macro_string, 3, MUIM_Set, MUIA_String_Contents, NULL);
		DoMethod(win->macro_string, MUIM_Notify, MUIA_String_Acknowledge,
			MUIV_EveryTime, win->macro_win, 3, MUIM_CallHook,
			hook, 0);

		DoMethod(App, OM_ADDMEMBER, win->macro_win);
		}

	set(win->macro_win, MUIA_Window_Open, TRUE) ;
	return RETURN_OK ;
	}

/*
 * RexxDispatch - just eat reply messages to our port. If it's
 * anything but a reply message, we're royally hosed. So we set the Rexx
 * result code (hoping that it really is a rexx result) to a failure,
 * and reply to it.
 */
void
RexxDispatch(void) {
	struct RexxMsg *msg ;

	while ((msg = (struct RexxMsg *) GetMsg(port)))
		if (msg->rm_Node.mn_Node.ln_Type != NT_REPLYMSG) {
			msg->rm_Result1 = RETURN_FAIL ;
			ReplyMsg((struct Message *) msg) ;
			}
		else {
			DeleteArgstring(msg->rm_Args[0]) ;
			DeleteRexxMsg(msg) ;
			replies_needed -= 1 ;
			}

	delete_port() ;
	}

/*
 * Clean up any outstanding Rexx messages.
 */
void
RexxQuit(void) {

	while (replies_needed) {
		WaitPort(port) ;
		RexxDispatch() ;
		}
	}

/*
 * HTML parsing - we have to do this ourselves, as the current parsing is
 * wrapped up in the HTML Widget. If we ever grow multiple windows, this might
 * all get replaced by a widget with no attached window. Until then, it's
 * not clear how reentrant the gadget parsing is, so we don't risk it.
 */
static char *
fetch_title(struct mark_up *objects, char *url) {
	char *title = NULL, *tmp ;
	extern char *mo_grok_alternate_title(char *, char *) ;

	/* Skip down to the title (if we find one) */
	while (objects && objects->type != M_TITLE && !objects->is_end)
		objects = objects->next ;

	/* Pull all the text from inside the title */
	while (objects) {
		if (objects->type == M_NONE)
			if (!title) title = strdup(objects->text) ;
			else {
				tmp = malloc(strlen(title) +
					     strlen(objects->text) +
					     1) ;
				sprintf(tmp, "%s%s", title, objects->text) ;
				free(title) ;
				title = tmp ;
				}
		else if (objects->type == M_TITLE && objects->is_end)
			return title ;
		objects = objects->next ;
		}

	if (title) free(title) ;	/* No </title> */

	title = mo_grok_alternate_title(url, NULL) ;
	}

static int
stem_hrefs(struct mark_up *objects,
	char *stem,
	char *oldurl,
	int count,
	struct Message *msg) {

	char *ref = NULL, *url = NULL, *tmp, buffer[1024] ;

	while (objects) {
		if (objects->type == M_ANCHOR) {
			if (!objects->is_end) {
				if (url) free(url) ;
				if ((url = ParseMarkTag(objects->start, MT_ANCHOR, AT_HREF))) {
					if (*url) {
						tmp = mo_url_canonicalize(url,
									oldurl) ;
						free(url) ;
						url = tmp ;
						}
					else {
						free(url) ;
						url = NULL ;
						}
					}
				}
			else if (url) {	/* Got a pair! */
				sprintf(buffer, "%s.%d.URL", stem, count + 1) ;
				SetRexxVar(msg, buffer, url, strlen(url)) ;
				sprintf(buffer, "%s.%d.REF", stem, count + 1) ;
				if (ref) {
					SetRexxVar(msg, buffer, ref, strlen(ref)) ;
					free(ref) ;
					ref = NULL ;
					}
				else SetRexxVar(msg, buffer, "", 0) ;
				count = count + 1 ;
				free(url) ;
				url = NULL ;
				}
			else if (ref) {
				free(ref) ;
				ref = NULL ;
				}
			}
		else if (url && objects->type == M_NONE) {
			if (!ref) ref = strdup(objects->text) ;
			else {
				tmp = malloc(strlen(ref) +
					     strlen(objects->text) +
					     1) ;
				sprintf(tmp, "%s%s", ref, objects->text) ;
				free(ref) ;
				ref = tmp ;
				}
			}
		objects = objects->next ;
		}

	if (url) free(url) ;
	if (ref) free(ref) ;
	return count ;
	}

static int
stem_images(struct mark_up *objects,
	char *stem,
	char *oldurl,
	int count,
	struct Message *msg) {

	ImageInfo *img ;
	int ismap ;
	char *url, *url2, buffer[1024], buffer2[12] ;

	while (objects) {
		if (objects->type == M_IMAGE) {
			url = ParseMarkTag(objects->start, MT_IMAGE, "ISMAP") ;
			ismap = (int) url ;
			if (url) free(url) ;
			if ((url = ParseMarkTag(objects->start, MT_IMAGE, "SRC"))) {
				if (*url) {
					url2 = mo_url_canonicalize(url, oldurl) ;
					sprintf(buffer, "%s.%d.URL", stem, count + 1) ;
					SetRexxVar(msg, buffer, url2, strlen(url2)) ;
					sprintf(buffer, "%s.%d.ISMAP", stem, count + 1) ;
					SetRexxVar(msg, buffer, ismap ? "1" : "0", 1) ;

					sprintf(buffer, "%s.%d.CURRENT", stem, count + 1) ;
					if (!(img = mo_fetch_cached_image_data(url2)))
						SetRexxVar(msg, buffer, "0", 1) ;
					else {
						SetRexxVar(msg, buffer, "1", 1) ;
						sprintf(buffer, "%s.%d.WIDTH", stem, count + 1) ;
						sprintf(buffer2, "%d", img->width) ;
						SetRexxVar(msg, buffer, buffer2, strlen(buffer2)) ;
						sprintf(buffer, "%s.%d.HEIGHT", stem, count + 1) ;
						sprintf(buffer2, "%d", img->height) ;
						SetRexxVar(msg, buffer, buffer2, strlen(buffer2)) ;
						sprintf(buffer, "%s.%d.FILENAME", stem, count + 1) ;
						SetRexxVar(msg, buffer, img->fname, strlen(img->fname)) ;
						}
					count = count + 1 ;
					free(url2) ;
					}
				free(url) ;
				}
			}
		objects = objects->next ;
		}

	return count ;
	}
