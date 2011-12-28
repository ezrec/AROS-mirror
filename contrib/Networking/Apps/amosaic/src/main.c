#include "includes.h"
#include "HTML.h"
#include "mosaic.h"
#include "globals.h"
#include "htmlgad.h"
#include "gui.h"
#include <dos/dos.h>
#include <sys/stat.h>
#include "XtoI.h"
#include "HTMLP.h"
#include "protos.h"
#include "TextEditField.h"
#include <unistd.h>

#include "amosaic_cat.h"
struct Library *LocaleBase;

#include "AboutPic.h"

void __regargs _CXBRK(void);

extern IPTR HookEntry();

extern AppData Rdata;
extern __far int WWW_TraceFlag;

//char *statuspage="http://insti.physics.sunysb.edu/AMosaic/";

#ifdef __AROS__
#undef DEBUG
#endif

int DEBUG;
extern char *cached_url;
extern char *home_document;
extern char *startup_document;
extern long HTMLFormImages;

int lib_version;
char *url, *canon, *title, *tmp;
char *text, *texthead, *ref;
mo_window window;

struct MsgPort *FileNotifyPort;
ULONG FileNotifySigBit;

void amiga_init_hooks(void);
static void locate_mime_files(void);
void PasteStringClipBoard(char *string);

//#define TRACE_INPUT

/*------------------------------------------------------------------------
  Initialize everything, load an initial URL, and enter main loop.
------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	BOOL running=TRUE;
	ULONG signal;
	HTMLGadClData *inst = NULL;
	int transfer_in_progress=0;

	/* A LOT OF THE STUFF AT THE TOP HERE SHOULD BE MOVED EVENTUALLY! */

	lib_version = IntuitionBase->LibNode.lib_Version;
	//	lib_version = 38;		// for testing WB2.x compatibilty on 3.x machines

        if (lib_version < 39) {
	    printf("Requires WB 3.0 or later\n") ;
	    exit(0) ;
	    }

	LocaleBase=OpenLibrary("locale.library",38);
	OpenamosaicCatalog(NULL,NULL);

	amiga_init_hooks();
	amiga_load_prefs();		/* RData is extern'ed */
	locate_mime_files();
	mo_setup_global_history();

#ifndef __AROS__
	/* Use the new 6.50 WB argument support. */
	if (argc == 0) {
		argc = _WBArgc;
		argv = _WBArgv;
	}
#endif

	if ((argc > 1) && (!strcmp(argv[1], "-x"))) {
	        argc -= 1 ;
		argv += 1 ;
		DEBUG = 1;
	}

	WWW_TraceFlag = DEBUG;

	fire_er_up(argc, argv);

	if (startup_document)
		url = strdup(startup_document);
	else
	url = strdup(home_document);

	setup_win(&window);		/* this is just an internal structure */

#ifdef TRACE_INPUT
	printf("Loading fonts... ");
	fflush(stdout);
#endif

	open_fonts();			/* open our set of fonts */

#ifdef TRACE_INPUT
	printf("Ok.\n");
#endif

	init();					/* Initialize MUI itself */
	MUI_HTMLGadClass = HTMLGadClInit(); /* Initialize our new gadget class */
	HTMLGadClass=MUI_HTMLGadClass->mcc_Class;
	SpinnerClass = SpinnerClInit(); /* Initialize the spinner class */
	BoingTransferClass = BoingTransferClInit(); /* Initialize the spinner class */
	MUI_NewGroupClass=NewGroupClInit();
	NewGroupClass=MUI_NewGroupClass->mcc_Class;
	TextEditFieldClass=TextEditFieldClInit();
	if((FileNotifyPort=CreatePort(NULL,0)))
		FileNotifySigBit=1<<(FileNotifyPort->mp_SigBit);
	else
		printf("Couldn't create notify port\n");


	mui_init();			/* Initialize the interface */

	if (lib_version >= 39) {

	/* Find out what screen we are on.	If it is not the workbench screen,
		 release all the pens on the screen!	This is a hack until we decide
		 on another way to get pubscreens with shareable pens. */
		}

	DoMethod(BT_Forward, MUIM_Set, MUIA_Disabled, TRUE);
	DoMethod(BT_Back, MUIM_Set, MUIA_Disabled, TRUE);

	window.HTML_Gad = HTML_Gad;
	window.scrolled_win = INST_DATA(HTMLGadClass,HTML_Gad);
	window.view = WI_Main;

	get(HTML_Gad, HTMLA_inst, (IPTR *)(&inst));
//	window.scrolled_win=inst;

	Rdata.track_pointer_motion=0;
	mo_access_document(&window, url);
	Rdata.track_pointer_motion=1;


	/* */
	/* THIS IS THE MAIN APPLICATION INPUT LOOP */
	/* */

	while (running) {
#ifdef HAVE_REXX
		extern ULONG RexxSignalBits ;
		void RexxDispatch(void) ;
#endif
		int a;

#ifdef TRACE_INPUT
	kprintf("Entering MUI input loop\n");
#endif
		a = (DoMethod(App,MUIM_Application_Input,&signal));

#ifdef TRACE_INPUT
	kprintf("Exiting MUI input loop\n");
#endif

#ifdef HAVE_REXX
		signal |= RexxSignalBits;
#endif
/* | FileNotifySigBit); */
		switch(a) {
		
			
		case mo_exit_program:
		case MUIV_Application_ReturnID_Quit:
		running = FALSE;
		break;

	case mo_about:{
		extern char * amosaic_about_version;
		Object *win,*ok_gad,*omni_gad;
		BOOL about_running=TRUE;
		ULONG signals;
		win=WindowObject,
			MUIA_Window_Title,GetamosaicString(MSG_REQ_ABOUT_TITLE),
			MUIA_Window_RefWindow,WI_Main,
			MUIA_Window_CloseGadget,FALSE,
			WindowContents,VGroup,
				MUIA_Background,MUII_RequesterBack,
				Child,VGroup,
					Child,HGroup,
						TextFrame,
						MUIA_Weight,0,
						MUIA_Group_Spacing,0,
						MUIA_Background,MUII_BACKGROUND,
						Child,HSpace(0),		
						Child,BodychunkObject,
							MUIA_Bodychunk_Body,amlogo_body,
							MUIA_Bodychunk_Compression,AMLOGO_COMPRESSION,
							MUIA_Bodychunk_Depth,AMLOGO_DEPTH,
							MUIA_Bodychunk_Masking,AMLOGO_MASKING,
							MUIA_Bitmap_Width,AMLOGO_WIDTH,
							MUIA_Bitmap_Height,AMLOGO_HEIGHT,
							MUIA_Bitmap_SourceColors,amlogo_colors,
//							MUIA_Bitmap_Transparent,0,
							MUIA_FixWidth,AMLOGO_WIDTH,
							MUIA_FixHeight,AMLOGO_HEIGHT,
							End,
						Child,HSpace(0),
						End,
					Child,HGroup,
						MUIA_Group_Spacing,0,
						TextFrame,
						MUIA_Background,MUII_TextBack,
						MUIA_Weight,0,
						Child,HSpace(0),
						Child,TextObject,MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,amosaic_about_version,MUIA_Text_SetMax,TRUE,End,
						Child,HSpace(0),
						End,
					Child,omni_gad=TextObject,MUIA_Text_PreParse,"\033c",
						MUIA_Text_Contents,"http://www.omnipresence.com/",
						ButtonFrame,MUIA_Background,MUII_ButtonBack,MUIA_InputMode,MUIV_InputMode_RelVerify,
						MUIA_FixWidthTxt,"  http://www.omnipresence.com/  ",
						End,
					End,
				Child,HGroup,
					MUIA_Weight,0,
					Child,HSpace(0),
					Child,ok_gad=SimpleButton(GetamosaicString(MSG_REQ_ABOUT_OK)),
					Child,HSpace(0),
					End,
				End,
			End;							 

		if(win)
		{
			DoMethod(App,OM_ADDMEMBER,win);
			DoMethod(ok_gad,MUIM_Notify,MUIA_Pressed,FALSE,App,2,MUIM_Application_ReturnID,1);
			DoMethod(omni_gad,MUIM_Notify,MUIA_Pressed,FALSE,App,2,MUIM_Application_ReturnID,2);
			set(win,MUIA_Window_ActiveObject,ok_gad);
			set(App,MUIA_Application_Sleep,TRUE);
			set(win,MUIA_Window_Open,TRUE);
			while(about_running)
			{
				switch(DoMethod(App,MUIM_Application_Input,&signals))
				{
					case MUIV_Application_ReturnID_Quit:
						running=FALSE;
					case 1:
						about_running=FALSE;
						break;
					case 2:
						set(TX_URL, MUIA_String_Contents,"http://www.omnipresence.com/");
						DoMethod(App,MUIM_Application_ReturnID,ID_STRINGURL);
						about_running=FALSE;
						break;
				}
				if(about_running&&signals) Wait(signals);
			}
			set(win,MUIA_Window_Open,FALSE);
			set(App,MUIA_Application_Sleep,FALSE);
			DoMethod(App,OM_REMMEMBER,win);
			MUI_DisposeObject(win);
			}					
		}		 
		break;

	case mo_search_index:{
		char *str = "", *esc_str;
		int len = strlen(cached_url);

		get(STR_Search, MUIA_String_Contents, &str);
		esc_str = mo_escape_part(str);
		url = malloc(len + strlen(esc_str) + 5);
		strcpy(url, cached_url);
		strtok(url, "#");
		strtok(url, "?");
		if (url[strlen(url)-1] != '?'){
				strcat (url, "?");
			}
		strcat(url, esc_str);
		free(esc_str);
				set(HTML_Gad, HTMLA_active, FALSE);
//		inst->html->active = FALSE;
		mo_access_document(&window, url);
				set(HTML_Gad, HTMLA_active, TRUE);
//		inst->html->active = TRUE;
		set(STR_Search, MUIA_String_Contents, NULL);
		break;
		}

		case ID_STRINGURL:{
			int i = transfer_in_progress;

			get(TX_URL, MUIA_String_Contents, (IPTR *)(&tmp));
			url = malloc(strlen(tmp)+1);
			strcpy(url, tmp);
			transfer_in_progress = 1;
			set(HTML_Gad, HTMLA_active, FALSE);
			Rdata.track_pointer_motion=0;
			if (!i) mo_access_document(&window, url);
			Rdata.track_pointer_motion=1;
			set(HTML_Gad, HTMLA_active, TRUE);
			break;
		}

		case ID_GOTOANCHOR:{
		int i = transfer_in_progress;

		GetAttr(HTMLA_new_href, HTML_Gad, (IPTR *)(&tmp));
		url = malloc(strlen(tmp)+1);
		strcpy(url, tmp);
		transfer_in_progress = 1;
		set(HTML_Gad, HTMLA_active, FALSE);
		Rdata.track_pointer_motion=0;
		if (!i) mo_access_document(&window, url);
		Rdata.track_pointer_motion=1;
				set(HTML_Gad, HTMLA_active, TRUE);
		break;
		}

		case ID_GETTEXT:{
		int i = transfer_in_progress;

		GetAttr(HTMLA_get_href, HTML_Gad, (IPTR *)(&tmp));
		url = malloc(strlen(tmp)+1);
		strcpy(url, tmp);
		transfer_in_progress = 1;
		set(HTML_Gad, HTMLA_active, FALSE);
		Rdata.track_pointer_motion=0;
		if (!i) mo_access_document(&window, url);
		Rdata.track_pointer_motion=1;
				set(HTML_Gad, HTMLA_active, TRUE);
		break;
		}

		case ID_POSTTEXT:{
		int i = transfer_in_progress;
		char *query;
		GetAttr(HTMLA_post_href, HTML_Gad, (IPTR *)(&tmp));
		url = malloc(strlen(tmp)+1);
		strcpy(url, tmp);
		free(tmp); /* Yes, it is legal to free it here */

		GetAttr(HTMLA_post_text, HTML_Gad, (IPTR *)(&tmp));
		query = malloc(strlen(tmp)+1);
		strcpy(query, tmp);
		free(tmp); /* Yes, it is legal to free it here */

		transfer_in_progress = 1;
		set(HTML_Gad, HTMLA_active, FALSE);
		Rdata.track_pointer_motion=0;
		if (!i) mo_post_access_document(&window, url,
			"application/x-www-form-urlencoded", query); /* All forms post have a _post in the name */
				set(HTML_Gad, HTMLA_active, TRUE);
		Rdata.track_pointer_motion=1;
		break;
		}

		case mo_macro: case mo_macro_1: case mo_macro_2: case mo_macro_3:
		case mo_macro_4: case mo_macro_5: case mo_macro_6: case mo_macro_7:
		case mo_macro_8: case mo_macro_9: case mo_macro_10:
		case mo_add_hot_list: case mo_jump_hot_list: case mo_print_document:
#ifdef HAVE_REXX
		RexxLaunch(a - mo_macro_1, &window);
#endif
		break;

	case mo_back:
		mo_back_node(&window);
		break;

	case mo_forward:
		mo_forward_node(&window);
		break;

	case mo_home_document:
		mo_access_document(&window, home_document);
		break;

	case mo_load_amigahome:
		Rdata.track_pointer_motion=0;
		mo_access_document(&window, "http://www.omnipresence.com/amiga/");
		Rdata.track_pointer_motion=1;
		break;

	case mo_load_amosaichome:
		Rdata.track_pointer_motion=0;
		mo_access_document(&window, "http://www.omnipresence.com/amosaic/");
		Rdata.track_pointer_motion=1;
		break;

	case mo_reload_document:
		Rdata.track_pointer_motion=0;
		mo_reload_window_text(&window, 0);
		Rdata.track_pointer_motion=1;
		break;

	case mo_search:
		mo_post_search_window(&window);
		break; 

	case mo_save_document:
		mo_post_save_window(&window);
		break;

	case mo_clear_image_cache:{
//		struct Gadget *g;
//		struct Window *w;
		TO_HTML_FlushImageCache();

//mjw	mo_flush_image_cache(&window);
//mjw	GetAttr(MUIA_Boopsi_Object, HTML_Gad, (IPTR *)(&g));
//mjw	GetAttr(MUIA_Window_Window, WI_Main, (IPTR *)(&w));
//mjw	RefreshGList(g, w, NULL, 1);
		}
		break;

	case mo_expand_images_current:
		/* if (window.delay_image_loads) */ LoadInlinedImages();
		break;

//	case ID_MUIRESIZE:
//		ResetAmigaGadgets();
//		break;

	case mo_clear_global_history:
		mo_wipe_global_history(&window);
		break;

	case mo_open_document:
		mo_post_open_window(&window);
		break;

	case mo_open_local_document:
		if (get_filename(&url)) {
			char *url2;
			url2 = malloc(strlen(url)+2);
			strcpy(url2, "/");
			strcat(url2, url);
			url = mo_url_canonicalize_local(url2);
			Rdata.track_pointer_motion=0;
			mo_load_window_text(&window, url, NULL);
			Rdata.track_pointer_motion=1;
			}
		break;

	case mo_document_source:
		mo_post_source_window(&window);
		break;

	case mo_history_list:
		mo_post_history_win(&window);
		break;

	case mo_delay_image_loads:
		window.delay_image_loads = DoMethod(App, MUIM_Application_GetMenuCheck, mo_delay_image_loads);
		if (!window.delay_image_loads) LoadInlinedImages();
		break;

	case mo_image_buttons:
	case mo_buttons_text:
	case mo_buttons_image:
	case mo_buttons_imagetext:
		if(DoMethod(App, MUIM_Application_GetMenuCheck, mo_buttons_text))
			Rdata.image_buttons = 0;
		else if(DoMethod(App, MUIM_Application_GetMenuCheck, mo_buttons_image))
			Rdata.image_buttons = 1;
		else if(DoMethod(App, MUIM_Application_GetMenuCheck, mo_buttons_imagetext))
			Rdata.image_buttons = 2;

		SetImageButtons(Rdata.image_buttons);
		break;

	case mo_isindex_to_form:
		Rdata.isindex_to_form = DoMethod(App, MUIM_Application_GetMenuCheck, mo_isindex_to_form);
		SetSearchIndex(Rdata.image_buttons);
		mo_set_win_current_node(&window,window.current_node);
		break;

	case mo_url_to_clipboard:
		get(TX_URL, MUIA_String_Contents, (IPTR *)(&tmp));
		PasteStringClipBoard(tmp);	
		break;

	case mo_binary_transfer:
		window.binary_transfer = DoMethod(App, MUIM_Application_GetMenuCheck, mo_binary_transfer);
		break;

	case mo_show_form_images:{
		HTMLGadClData *inst=INST_DATA(HTMLGadClass,HTML_Gad);
		inst->have_formsI=1;
		set(WI_Main,MUIA_Window_Open,FALSE);
		set(WI_Main,MUIA_Window_Open,TRUE);
		set(WI_ImagesReady,MUIA_Window_Open,FALSE);
		inst->have_formsI=0;}
		break;

//	case ID_CONFIGSCROLLBARS:
//		ResetAmigaGadgets();
//		break;

	case ID_IMAGELOAD:
		if (lib_version < 39)	/* need WB 3.x */
			break;
		{
			int i=transfer_in_progress;

			transfer_in_progress = 1;
			if (!i) {
				char *img = NULL;
				HTMLFormImages=0;
				get(HTML_Gad, HTMLA_image_load, &img);
				Rdata.track_pointer_motion=0;
				LoadInlinedImage(img);
				Rdata.track_pointer_motion=1;
				if(inst->have_forms && HTMLFormImages)
					set(WI_ImagesReady,MUIA_Window_Open,TRUE);
				}
		}
		break;
	}

	if (signal)
		transfer_in_progress = 0;
#ifdef	TRACE_INPUT
	if(signal & SIGBREAKF_CTRL_C)
		kprintf("Ctrl C pressed\n");
#endif

	if (running && signal) {
		ULONG sig_rec;
#ifdef TRACE_INPUT
		kprintf("Waiting for signals\n");
#endif
		sig_rec=Wait(signal | FileNotifySigBit);
#ifdef TRACE_INPUT
		kprintf("Exit waiting for signals\n");
#endif
#ifdef HAVE_REXX
		if ((sig_rec /* & (~FileNotifySigBit) */ ) && RexxSignalBits)
			RexxDispatch();

		else if (sig_rec & FileNotifySigBit){
#else
		if (sig_rec & FileNotifySigBit){
#endif
			struct NotifyMessage *msg;
#ifdef TRACE_INPUT
			kprintf("Getting filenotify sig\n");
#endif
			while((msg=(struct NotifyMessage *)GetMsg(FileNotifyPort))){
				Object *obj=(Object *)msg->nm_NReq->nr_UserData;
#ifdef TRACE_INPUT
				kprintf("Getting filenotify msg for object %08lx\n",obj);
#endif
				ReplyMsg((struct Message *)msg);
				DoMethod(obj,MUIM_TEF_FileChange);
				}
			}
		}
#ifdef TRACE_INPUT
		else
			kprintf("No signal to wait for\n");
#endif
	}
	mo_flush_image_cache(&window);


/*
	set(WI_Main,MUIA_Window_Open,FALSE);

	{
		extern void ForgetALLHTMLAmigaData	(void);
		ForgetALLHTMLAmigaData();
	}
*/


	DoMethod(App,OM_REMMEMBER,WI_Main); /* We HAVE to close and dispose the window before we free the classes */
	MUI_DisposeObject(WI_Main);


	if(FileNotifyPort)
		DeletePort(FileNotifyPort); /* All pending messages are deleted by EndNotify */
	TextEditFieldClFree(TextEditFieldClass);
	BoingTransferClFree(BoingTransferClass);
	SpinnerClFree(SpinnerClass);
	HTMLGadClFree(MUI_HTMLGadClass);
	NewGroupClFree(MUI_NewGroupClass);
	
	close_fonts();			/* dispose our set of fonts */


	if (Rdata.use_global_history)
		mo_write_global_history();

#ifdef HAVE_REXX
	RexxQuit() ;
#endif

	CloseamosaicCatalog();
	if(LocaleBase)
		CloseLibrary(LocaleBase);

	fail(App,NULL);

	return 0;
}



/*------------------------------------------------------------------------
	Open requester and get the name of a local file to load.
------------------------------------------------------------------------*/
int get_filename(char **name)
{
	struct FileRequester *fr;
	struct Window *w=NULL;
	int result;
	char *f;

	fr = MUI_AllocAslRequestTags(ASL_FileRequest, ASL_Hail, GetamosaicString(MSG_REQ_OPEN_LOCAL_TITLE),
						 TAG_DONE);
	GetAttr(MUIA_Window_Window, WI_Main, (IPTR *)(&w));
	result = MUI_AslRequestTags(fr, ASLFR_Window, w, TAG_DONE);
	if (result) {
		f = malloc(255);
#ifdef HAVE_GETCD
		getcd(0, f);
#else
		getcwd(f,254);
#endif
		AddPart(f, fr->rf_Dir, 200);
		AddPart(f, fr->rf_File, 200);
		*name = strdup(f);
		free(f);
	}
	MUI_FreeAslRequest(fr);
	return result;
}



void setup_win(struct mo_window *win)
{
	win->id = 0;
	win->history = NULL;
	win->current_node = NULL;
	win->reloading = 0;
	win->target_anchor = NULL;
	win->underlines = 1;
	win->visited_underlines = 1;
	win->cached_url = NULL;
	win->search_start = (void *)TO_HTMLMakeSpaceForElementRef();
//	win->search_start = (void *)malloc(sizeof (ElementRef));
	win->search_end = (void *)TO_HTMLMakeSpaceForElementRef();
//	win->search_end = (void *)malloc(sizeof (ElementRef));
	win->binary_transfer = 0;
	win->delay_image_loads = Rdata.delay_image_loads;
}


/*
	Tell libwww what screen to open external viewers on
*/
char *gui_whichscreen(void)
{
	struct Screen *scr = NULL;
	struct PubScreenNode *p;
	struct List *l;
	char *name = "Workbench";

	get(WI_Main, MUIA_Window_Screen, &scr);
	l = LockPubScreenList();
	for (p = (struct PubScreenNode *)(l->lh_Head); p != NULL;
			 p = (struct PubScreenNode *)(p->psn_Node.ln_Succ)) {
		if (p->psn_Screen == scr) name = p->psn_Node.ln_Name;
	}
	UnlockPubScreenList();
	
	return(name);
}


unsigned long back_node_cb(void)
{
	mo_back_node(&window);
	return (unsigned long) 0;
}

unsigned long forward_node_cb(void)
{
	mo_forward_node(&window);
	return (unsigned long) 0;
}

/*
extern unsigned long update_debug_cb(void);
*/

extern unsigned long icon_pressed_cb(void);

void amiga_init_hooks(void)
{
/*	debug_hook.h_Entry = HookEntry;
	debug_hook.h_SubEntry = update_debug_cb;
	debug_hook.h_Data = NULL;
*/
	interrupt_hook.h_Entry = HookEntry;
	interrupt_hook.h_SubEntry = icon_pressed_cb;
	interrupt_hook.h_Data = NULL;

	goback_hook.h_Entry = HookEntry;
	goback_hook.h_SubEntry = back_node_cb;
	goback_hook.h_Data = NULL;

	goforward_hook.h_Entry = HookEntry;
	goforward_hook.h_SubEntry = forward_node_cb;
	goforward_hook.h_Data = NULL;
}

//void __regargs _CXBRK(void)
//{
//}


/* MWM
	 Apparently we have lots of places to look for MIME files.	Blech.
*/
static void locate_mime_files(void)
{
	extern char *global_extension_map;
	extern	char *global_type_map;
	/* the places to be searched, in decreasing order of desirability */
	static char *extension_map_places[6]={
		"env:mosaic/mime.types",		"env:mosaic/.mime.types",
		"env:mime.types",		"env:.mime.types",
		"s:mime.types",		"s:.mime.types"
		};

	static char *type_map_places[6]={
		"env:mosaic/mailcap",		"env:mosaic/.mailcap",
		"env:mailcap",		"env:.mailcap",
		"s:mailcap",		"s:.mailcap"
		};


	/* only mess with it if it is set to the default or unset */
	if ((global_extension_map == NULL) ||
			(! stricmp("s:.mime.types",global_extension_map))){
		FILE *test; int i;
		for (i=0; i<6; i++)
			if ((FILE *)NULL !=(test=fopen(extension_map_places[i],"r"))){
	fclose(test);
	global_extension_map = extension_map_places[i];
				break;
			}
	}
	/* only mess with it if it is set to the default or unset */
	if ((global_type_map == NULL) ||
			(! stricmp("s:.mailcap",global_type_map))){
		FILE *test; int i;
		for (i=0; i<6; i++)
			if ((FILE *)NULL !=(test=fopen(type_map_places[i],"r"))){
	fclose(test);
	global_type_map = type_map_places[i];
				break;
			}
	}
}

//mjw sleep() { }


void WriteLong(struct IOStdReq *ior,LONG ldata)
{
	ior->io_Data = &ldata;
	ior->io_Length = 4;
	ior->io_Command = CMD_WRITE;
	DoIO((struct IORequest *)ior);
}

void PasteStringClipBoard(char *string)
{
	struct MsgPort *ReplyPort;
	struct IOStdReq *ior;
	long unit=0,slen,length;
	BOOL odd=FALSE;

	if((ReplyPort=CreateMsgPort())){
		if((ior=(struct IOStdReq *)CreateExtIO(ReplyPort,sizeof(struct IOClipReq)))){
			if(!OpenDevice("clipboard.device",unit,(struct IORequest *)ior,0)){

				slen=strlen(string);
				odd= slen & 1;
				length=(slen+13) & ~1;

				ior->io_Offset=0;
				ior->io_Error=0;
				((struct IOClipReq *)ior)->io_ClipID=0;

				WriteLong(ior,MAKE_ID('F','O','R','M'));
				WriteLong(ior,length);
				WriteLong(ior,MAKE_ID('F','T','X','T'));
				WriteLong(ior,MAKE_ID('C','H','R','S'));
				WriteLong(ior,slen);

				ior->io_Data = string;
				ior->io_Length = slen;
				ior->io_Command = CMD_WRITE;
				DoIO((struct IORequest *)ior);
				if(odd){
					ior->io_Data = "";
					ior->io_Length = 1L;
					ior->io_Command = CMD_WRITE;
					DoIO((struct IORequest *)ior);
					}

				ior->io_Command = CMD_UPDATE;
				DoIO((struct IORequest *)ior);

				CloseDevice((struct IORequest *)ior);
				}
			DeleteExtIO((struct IORequest *)ior);
			}
		DeleteMsgPort(ReplyPort);
		}
}
