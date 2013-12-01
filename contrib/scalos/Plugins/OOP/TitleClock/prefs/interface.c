/*
 *  interface.c - Functions for creating the UI and handling IDCMP
 *
 * $Date$
 * $Revision$
 *
 *  0.1
 *  20010723    DM  + Routines for calculating sizes of gadgets, resizing window and font sensitivity.
 *  20010717    DM  + Created, although it's probably a bit messy
 *

On startup, load from ENV:
Write to ENV: on use
Write to ENV: and ENVARC: on save
Load from ENVARC: on "Last saved"

 */

#include <exec/types.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/diskfont.h>
#include <proto/graphics.h>
#include <proto/locale.h>
#include <proto/dos.h>

#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <intuition/imageclass.h>
#include <libraries/gadtools.h>
#include <utility/hooks.h>
#include <utility/tagitem.h>

#include <defs.h>
#include "title_clock_prefs.h"
#include "screen.h"
#include "misc.h"
#include "tcp_locale.h"
#include "prefs_file.h"
#include "interface.h"
#include "requesters.h"
#include "saveicon.h"

#include <stdio.h>
#include <string.h>


struct FormatDateHookData
	{
	STRPTR fdhd_Buffer;		// buffer to write characters to
	size_t fdhd_Length;		// length of buffer
	};

/* Gadget ID numbers */
enum {
	GID_DATE_FORMAT = 1,
	GID_DATE_EXAMPLE,
	GID_TIME_FORMAT,
	GID_TIME_EXAMPLE,
	GID_SAVE,
	GID_USE,
	GID_CANCEL,
	GID_MAX_GADGETS
};


/* Menu ID numbers (used for the UserData field) */
enum {
	MID_PROJECT_MENU,
	MID_PROJECT_OPEN,
	MID_PROJECT_SAVEAS,
	MID_PROJECT_QUIT,
	MID_EDIT_MENU,
	MID_EDIT_RESET,
	MID_EDIT_LASTSAVED,
	MID_EDIT_RESTORE,
	MID_SETTINGS_MENU,
	MID_SETTINGS_CREATEICONS
};


struct Screen       *tcp_Screen = NULL;     /* The screen we will be opening on */
APTR                tcp_vi = NULL;          /* VisualInfo for gadtoosl and stuff */
struct DrawInfo     *tcp_Screen_dri = NULL;
struct Window       *tcp_Window = NULL;     /* There's only 1 window in this prefs program */
struct Gadget       *tcp_GadList = NULL;    /* The gadgets in the window */
struct Menu         *tcp_MenuStrip = NULL;  /* Menus to be attached to the window */

struct Gadget       *tcp_Gadgets[GID_MAX_GADGETS] = {0};    /* Simple access to each created gadget */

struct TextAttr     topaz = {"topaz.font", 8, 0, 0};    /* Default fallbasck font */
//struct TextAttr     screenfont;                         /* Font used for the screen (and also the one we want to use for our gadgets */
struct TextAttr     *tcp_UseFont;            /* Font used for gadgets */

struct TextFont     *tcp_WindowFont = NULL; /* The font used for the main window */


/* Open's font, but checks if diskfont.library has been opened first, if not just uses graphics.library OpenFont,
 * which should pretty much always be available, since it's one of the required libraries and it's in ROM.
 */
struct TextFont *myOpenDiskFont(struct TextAttr *ta)
{
	if(DiskfontBase)
	{
		return(OpenDiskFont(ta));
	}
	return(OpenFont(ta));
}


void SizeBorders(struct Screen *s, WORD *right, WORD *bottom)
{
	struct DrawInfo *dri;       /* Screen's drawinfo data */
	struct Image    *img;       /* Image for size gadget */
	LONG            sis;        /* size of image for either med or low res screens */

	/* Store the default sizes */
	if(right) *right = 18;
	if(bottom) *bottom = 10;

	if(s)
	{
		if(NULL != (dri = GetScreenDrawInfo(s)) )
		{
			if(s->Flags & SCREENHIRES) sis = SYSISIZE_MEDRES; else sis = SYSISIZE_LOWRES;
			img = (struct Image *) NewObject(NULL, "sysiclass",  SYSIA_DrawInfo, dri,
												SYSIA_Which, SIZEIMAGE,
												SYSIA_Size, sis,
												TAG_DONE);
			if(img)
			{
				if(right) *right = img->Width;
				if(bottom) *bottom = img->Height;
				DisposeObject((Object *) img);
			}
			FreeScreenDrawInfo(s, dri);
		} /* drawinfo */
	} /* screen */
}


/* Calculate sizes of required gadget area, include them with window border sizes and
 * return true or false to say whether this window will fit on this screen.
 *
 * Current window layout is like this:
 *
 * |-----------------------------------|
 * |#| Titlebar Clock plugin prefs |%|F|
 * |-----------------------------------|
 * |   Date ========================== |
 * |Example ========================== |
 * |   Time ========================== |
 * |Example ========================== |
 * |                                   |
 * | Save          Use          Cancel |
 * |-----------------------------------|
 */

#define GAP         4   /* Gap between gadgets and window borders */
#define BUTTONEXTRA 12  /* Extra space at left and right of buttons */
#define TEXTGADGAP  8   /* Gap between text and (string|text) gadgets */
#define MINSIZE     70  /* Minimum size of string gadgets */

BOOL CalcMinSizes(struct Screen *scr, ULONG *width, ULONG *height, struct TextAttr *ta)
{
	BOOL    willfit = 0;            /* Flag to quickly show whether window will fit on screen or not */
	WORD    wbl, wbt, wbr, wbb;     /* Borders on windows */
	WORD    gh;                     /* Height of a standard 1 line gadget */
	WORD    miniw, minih;           /* Minimum inner width and height (gadget area) */
	WORD    strlabel;
	WORD    gadlabel;
	struct TextFont *tf;

	if(!ta) return(0);

	if(NULL != (tf = myOpenDiskFont(ta)) )
	{
		if(scr)
		{
			wbl = scr->WBorLeft;
			wbt = scr->WBorTop + scr->Font->ta_YSize + 1;
			wbb = scr->WBorBottom;
			SizeBorders(scr, &wbr, NULL);

			gh = scr->Font->ta_YSize + 6; if(gh < 14) gh = 14;

			minih = gh * 5 + GAP * 6;
			if(wbt + minih + wbb <= scr->Height)
			{
				/* Work out required width of string gadgets with an crappy example format string */
				strlabel = MaxLen(3,USTextLength(GetLocString(MSG_DATE_GAD), tf),
						USTextLength(GetLocString(MSG_EXAMPLE_GAD), tf),
						USTextLength(GetLocString(MSG_TIME_GAD), tf) );
				strlabel += TEXTGADGAP + USTextLength((STRPTR)"%%%%%%%%", tf);

				gadlabel = MaxLen(3,USTextLength(GetLocString(MSG_SAVE_GAD), tf),
						USTextLength(GetLocString(MSG_USE_GAD), tf),
						USTextLength(GetLocString(MSG_CANCEL_GAD), tf)) + BUTTONEXTRA;
				gadlabel = 3 * gadlabel + 4 * GAP;

				miniw = strlabel; if(gadlabel > miniw) miniw = gadlabel;
				if(wbl + miniw + wbr <= scr->Width)
				{
					willfit = 1;
					if(width) *width = (wbl + miniw + wbr);
					if(height) *height = (wbt + minih + wbb);
				} /* width fits */
			} /* Height fits */
		} /* screen valid */
		CloseFont(tf);
	} /* textfont opened */
	return(willfit);
}


void FreeMainGadgets(struct Gadget *gadlist)
{
	int     i;  /* Loop counter for clearing gadget pointer array */

	if(gadlist) FreeGadgets(gadlist);

	for(i = 0; i < GID_MAX_GADGETS; i++)
	{
		tcp_Gadgets[i] = NULL;
	}
}


#define RetFail(x,y)  if(!(x)) { FreeMainGadgets(y); CloseFont(tf); return(NULL); }

/*
void printng(struct NewGadget *ng)
{
	printf("ng.ng_LeftEdge=%d\n",ng->ng_LeftEdge);
	printf("ng.ng_TopEdge=%d\n",ng->ng_TopEdge);
	printf("ng.ng_Width=%d\n",ng->ng_Width);
	printf("ng.ng_Height=%d\n",ng->ng_Height);
	if(ng->ng_GadgetText) printf("ng.ng_GadgetText=%s\n",ng->ng_GadgetText);
	printf("ng.ng_TextAttr=0x%08lX\n",(ULONG)ng->ng_TextAttr);
	printf("ng.ng_GadgetID=%u\n",ng->ng_GadgetID);
	printf("ng.ng_Flags=%lu\n",ng->ng_Flags);
	printf("ng.ng_VisualInfo=0x%08lX\n",(ULONG)ng->ng_VisualInfo);
	printf("ng.ng_UserData=0x%08lX\n",(ULONG)ng->ng_UserData);
}
*/


struct Gadget *CreateMainGadgets(struct Screen *scr, APTR vi, ULONG ww, ULONG wh, struct TextAttr *gadfont)
{
	WORD    wbl, wbt, wbr, wbb;     /* Borders on windows */
	WORD    gh;                     /* Height of a standard 1 line gadget */
//	WORD    strlabel = 0;           /* Maximum length of the 4 string/text gadget labels */
//	WORD    gadlabel = 0;           /* Maximum length of ONE!!! of the bottom 3 buttons */
	struct TextFont     *tf;        /* We need to open the font for some size calculations, but the gadgets take the TextAttr */
	struct NewGadget    ng;         /* Used in the creation of the gadtools gadgets */
	STRPTR              examplemsg; /* Text for "Example" text gadget */
	STRPTR              timemsg;    /* Text for "Time" string gadget */
	struct Gadget       *gadlist = NULL;   /* Context for gadgets */
	struct Gadget       *gad;       /* Working pointer for creating gadgets */

	if(!gadfont)
		return(NULL);

	//printf("Gadfont OK\n");

	if(NULL != (tf = myOpenDiskFont(gadfont)) )
	{
		//printf("disk font opened ok\n");
		if(scr && vi)
		{
			//printf("screen and vi valid\n");
			gad = CreateContext(&gadlist);
			//printf("Created context, gad=0x%08lX\n", (ULONG)gad);
			RetFail(gad, gadlist);

			wbl = scr->WBorLeft;
			wbt = scr->WBorTop + scr->Font->ta_YSize + 1;
			wbb = scr->WBorBottom;
			SizeBorders(scr, &wbr, NULL);

			//printf("border size l/t/r/b=%d/%d/%d/%d\n", wbl, wbt, wbr, wbb);

			gh = gadfont->ta_YSize + 6;
			if(gh < 14)
				gh = 14;
			//printf("gadget height=%d\n", gh);


			examplemsg = GetLocString(MSG_EXAMPLE_GAD);
			timemsg = GetLocString(MSG_TIME_GAD);

			//printf("Example msg=%s\n", examplemsg);
			//printf("Time msg=%s\n", timemsg);

			/* Set up the nasic newgadget structure (most of which will be reused for each gadget */
			//ng.ng_LeftEdge =
			ng.ng_TopEdge = wbt + GAP;
			//ng.ng_Width =
			ng.ng_Height = gh;
			ng.ng_GadgetText = GetLocString(MSG_DATE_GAD);
			ng.ng_TextAttr = gadfont;
			ng.ng_GadgetID = GID_DATE_FORMAT;
			ng.ng_Flags = PLACETEXT_LEFT;
			ng.ng_VisualInfo = vi;
			ng.ng_LeftEdge = wbl + GAP + MaxLen(3, USTextLength(ng.ng_GadgetText, tf), USTextLength(examplemsg, tf), USTextLength(timemsg, tf)) + TEXTGADGAP;
			ng.ng_Width = (ww - wbr - GAP - ng.ng_LeftEdge);

			//printng(&ng);

			gad = CreateGadget(STRING_KIND, gad, &ng,
				GT_Underscore, (ULONG)'_',
				GTST_String, (ULONG)datestr,
				GTST_MaxChars, (ULONG)FORMAT_SIZE-1,
                                TAG_DONE);
			//printf("gad=%08lx\n", (ULONG)gad);
			RetFail(gad, gadlist);
			//printf("gadget OK\n");
			tcp_Gadgets[ng.ng_GadgetID] = gad;


			ng.ng_TopEdge += gh + GAP;
			ng.ng_GadgetText = examplemsg;
			ng.ng_GadgetID = GID_DATE_EXAMPLE;
			//printng(&ng);
			gad = CreateGadget(TEXT_KIND, gad, &ng,
				GTTX_Text, date_ex,
				GTTX_Border, 1,
                                TAG_DONE);
			RetFail(gad, gadlist);
			tcp_Gadgets[ng.ng_GadgetID] = gad;


			ng.ng_TopEdge += gh + GAP;
			ng.ng_GadgetText = timemsg;
			ng.ng_GadgetID = GID_TIME_FORMAT;
			//printng(&ng);
			gad = CreateGadget(STRING_KIND, gad, &ng,
				GT_Underscore, '_',
				GTST_String, timestr,
				GTST_MaxChars, EXAMPLE_SIZE-1,
                                TAG_DONE);
			RetFail(gad, gadlist);
			tcp_Gadgets[ng.ng_GadgetID] = gad;


			ng.ng_TopEdge += gh + GAP;
			ng.ng_GadgetText = examplemsg;
			ng.ng_GadgetID = GID_TIME_EXAMPLE;
			//printng(&ng);
			//printf("time example='%s'\n", time_ex);
			gad = CreateGadget(TEXT_KIND, gad, &ng,
				GTTX_Text, time_ex,
				GTTX_Border, 1,
                                TAG_DONE);
			RetFail(gad, gadlist);
			tcp_Gadgets[ng.ng_GadgetID] = gad;


			examplemsg = GetLocString(MSG_USE_GAD);
			timemsg = GetLocString(MSG_CANCEL_GAD);

			ng.ng_LeftEdge = wbl + GAP;
			ng.ng_TopEdge += gh + GAP;
			ng.ng_GadgetText = GetLocString(MSG_SAVE_GAD);
			ng.ng_Width = MaxLen(3, USTextLength(ng.ng_GadgetText, tf), USTextLength(examplemsg, tf), USTextLength(timemsg, tf)) + BUTTONEXTRA;
			ng.ng_GadgetID = GID_SAVE;
			ng.ng_Flags = PLACETEXT_IN;
			//printng(&ng);
			gad = CreateGadget(BUTTON_KIND, gad, &ng,
				GT_Underscore, '_',
                                TAG_DONE);
			RetFail(gad, gadlist);
			tcp_Gadgets[ng.ng_GadgetID] = gad;


			ng.ng_LeftEdge = (ww + wbl - wbr - ng.ng_Width) / 2;
			ng.ng_GadgetText = examplemsg;
			ng.ng_GadgetID = GID_USE;
			//printng(&ng);
			gad = CreateGadget(BUTTON_KIND, gad, &ng,
				GT_Underscore, '_',
                                TAG_DONE);
			RetFail(gad, gadlist);
			tcp_Gadgets[ng.ng_GadgetID] = gad;


			ng.ng_LeftEdge = ww - wbr - GAP - ng.ng_Width;
			ng.ng_GadgetText = timemsg;
			ng.ng_GadgetID = GID_CANCEL;
			//printng(&ng);
			gad = CreateGadget(BUTTON_KIND, gad, &ng,
				GT_Underscore, '_',
                                TAG_DONE);
			RetFail(gad, gadlist);
			tcp_Gadgets[ng.ng_GadgetID] = gad;

		}
		CloseFont(tf);
	}
	return(gadlist);
}


void FreeMainMenus(struct Menu *mnu)
{
	if(mnu) FreeMenus(mnu);
}


/* Structure for helping with automatically creating locale string menus */
/*struct LocaleNewMenu
{
	UBYTE   lnm_Type;
	UBYTE   lnm_Pad;
	ULONG   lnm_LabelMsgNum;
	ULONG   lnm_CommKeyMsgNum;
	UWORD   lnm_Flags;
	LONG    lnm_MutualExclude;
	APTR    lnm_UserData;
};
*/


struct Menu *CreateMainMenus(struct Screen *scr, APTR visinf)
{
	/* Mwoahahaha, evil structure of doom for creating menus, with strings
	 * replaced by their locale MESSAGE NUMBER!!! which then gets parsed and
	 * the strings replaced. NULL still represents a NULL pointer, so make
	 * sure none of the menu strings are string number 0!!!! Oh, aye,
	 * barlabels stay as they are, so -1 numbered strings are out too.
	 */
	struct NewMenu  nm[] = {    {NM_TITLE, (STRPTR)MSG_PROJECT_MENU, NULL, 0, 0, (APTR)MID_PROJECT_MENU},
					{NM_ITEM, (STRPTR)MSG_PROJECT_OPEN, (STRPTR)MSG_PROJECT_OPEN_SCT, 0, 0, (APTR)MID_PROJECT_OPEN},
					{NM_ITEM, (STRPTR)MSG_PROJECT_SAVEAS, (STRPTR)MSG_PROJECT_SAVEAS_SCT, 0, 0, (APTR)MID_PROJECT_SAVEAS},
					{NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL},
					{NM_ITEM, (STRPTR)MSG_PROJECT_QUIT, (STRPTR)MSG_PROJECT_QUIT_SCT, 0, 0, (APTR)MID_PROJECT_QUIT},
					{NM_TITLE, (STRPTR)MSG_EDIT_MENU, NULL, 0, 0, (APTR)MID_EDIT_MENU},
					{NM_ITEM, (STRPTR)MSG_EDIT_RESET, (STRPTR)MSG_EDIT_RESET_SCT, 0, 0, (APTR)MID_EDIT_RESET},
					{NM_ITEM, (STRPTR)MSG_EDIT_LASTSAVED, (STRPTR)MSG_EDIT_LASTSAVED_SCT, 0, 0, (APTR)MID_EDIT_LASTSAVED},
					{NM_ITEM, (STRPTR)MSG_EDIT_RESTORE, (STRPTR)MSG_EDIT_RESTORE_SCT, 0, 0, (APTR)MID_EDIT_RESTORE},
					{NM_TITLE, (STRPTR)MSG_SETTINGS_MENU, NULL, 0, 0, (APTR)MID_SETTINGS_MENU},
					{NM_ITEM, (STRPTR)MSG_SETTINGS_CREATEICONS, (STRPTR)MSG_SETTINGS_CREATEICONS_SCT, CHECKIT, 0, (APTR)MID_SETTINGS_CREATEICONS},
					{NM_END, NULL, NULL, 0, 0, NULL}};
//	int i;
	struct NewMenu  *ptr;   /* For parsing the above array */
	struct Menu     *mnu;   /* The menus we create */

	for(ptr = nm; ptr->nm_Type != NM_END; ptr++)
	{
		//printf("Pointing to %08lx, type is %u, UD=%lu\n", (ULONG)ptr, ptr->nm_Type, (ULONG)ptr->nm_UserData);

		if(NULL != ptr->nm_Label && NM_BARLABEL != ptr->nm_Label)
		{
			ptr->nm_Label = GetLocString((ULONG)ptr->nm_Label);
		}

		if(NULL != ptr->nm_CommKey)
		{
			ptr->nm_CommKey = GetLocString((ULONG)ptr->nm_CommKey);
		}
	}

	mnu = CreateMenus(nm, TAG_DONE);
	if(mnu)
	{
		LayoutMenus(mnu, visinf, GTMN_NewLookMenus, 1, TAG_DONE);
	}
	return(mnu);
}


void CloseMainWindow(void)
{
	if(tcp_Window)
	{
		if(tcp_MenuStrip)
			ClearMenuStrip(tcp_Window);
		if(tcp_GadList)
			RemoveGList(tcp_Window, tcp_GadList, -1);
		CloseWindow(tcp_Window);
		tcp_Window = NULL;
	}

	if(tcp_MenuStrip)
	{
		FreeMenus(tcp_MenuStrip);
		tcp_MenuStrip = NULL;
	}

	if(tcp_GadList)
	{
		FreeGadgets(tcp_GadList);
		tcp_GadList = NULL;
	}

	if(tcp_Screen_dri)
		FreeScreenDrawInfo(tcp_Screen, tcp_Screen_dri);
	tcp_Screen_dri = NULL;

	if(tcp_vi)
		FreeVisualInfo(tcp_vi);
	tcp_vi = NULL;

	if(tcp_Screen)
		UnlockPubScreen(NULL, tcp_Screen);
	tcp_Screen = NULL;
}


BOOL OpenMainWindow(void)
{
	BOOL    retval  = 0;    /* Result. =0 for something failed, !=0 for all (screen, window, gadgets, menus) opened/created successfully */
	ULONG   wx, wy;         /* X,Y position of window */
	ULONG   ww, wh;         /* Current window width and height */
	ULONG   minww, minwh;   /* Minimum sizes for window */
	BOOL    sizeok;         /* size of window fits on screen? */

	if(!tcp_Screen) tcp_Screen = FindPubScreen("Workbench");
	if(tcp_Screen)
	{
		if(NULL != (tcp_Screen_dri = GetScreenDrawInfo(tcp_Screen)) )
		{
			if(NULL != (tcp_vi = GetVisualInfo(tcp_Screen, (ULONG)NULL)) )
			{
				tcp_UseFont = tcp_Screen->Font ? tcp_Screen->Font : &topaz;
				sizeok = CalcMinSizes(tcp_Screen, &minww, &minwh, tcp_UseFont);
				if(!sizeok && tcp_UseFont != &topaz)
				{
					//printf("screen font doesn't fit\n");

					/* Repeat with topaz8 if too big from screen */
					tcp_UseFont = &topaz;

					/* Quit if that is too big */
					sizeok = CalcMinSizes(tcp_Screen, &minww, &minwh, tcp_UseFont);
				}

				if(sizeok)
				{
					//printf("size ok, min width, height=%lu %lu\n", minww, minwh);

					/* Work out sizes and create gadgets */
					ww = (tcp_Screen->Width + minww) / 2;   // halfway between inimum and screen size
					wh = minwh;

					//printf("window sizes=%lu %lu\n", ww, wh);

					// Centred
					wx = (tcp_Screen->Width - ww) / 2;
					wy = (tcp_Screen->Height - wh) / 2;

					//printf("window positions=%lu %lu\n", wx, wy);

					ShowExample(datestr, GID_DATE_EXAMPLE, date_ex, sizeof(date_ex));
					ShowExample(timestr, GID_TIME_EXAMPLE, time_ex, sizeof(time_ex));
					tcp_GadList = CreateMainGadgets(tcp_Screen, tcp_vi, ww, wh, tcp_UseFont);

					//printf("gadgets created\n");

					/* Create main menus */
					tcp_MenuStrip = CreateMainMenus(tcp_Screen, tcp_vi);
					//tcp_MenuStrip = NULL;

					if(tcp_GadList && tcp_MenuStrip)
					{
						/* Open window and attach all the stuff */
						if(NULL != (tcp_Window=OpenWindowTags(NULL,
								WA_Flags, WFLG_CLOSEGADGET | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_SIZEGADGET | WFLG_NEWLOOKMENUS,
								WA_IDCMP, IDCMP_CLOSEWINDOW | BUTTONIDCMP | STRINGIDCMP | TEXTIDCMP | IDCMP_SIZEVERIFY | IDCMP_NEWSIZE | IDCMP_VANILLAKEY | IDCMP_MENUPICK | IDCMP_REFRESHWINDOW | IDCMP_CHANGEWINDOW | IDCMP_GADGETUP,
								WA_Activate, TRUE,
								WA_Left, wx,
								WA_Top, wy,
								WA_Width, ww,
								WA_Height, wh,
								WA_MinWidth, minww,
								WA_MinHeight, minwh,
								WA_MaxWidth, tcp_Screen->Width,
								WA_MaxHeight, 0,
								WA_Title, GetLocString(MSG_WINDOW),
								WA_Gadgets, tcp_GadList,
								WA_PubScreen, tcp_Screen,
								WA_SimpleRefresh, -1,
								//WA_MenuHelp, 1,
								TAG_DONE)) )
						{
							GT_RefreshWindow(tcp_Window, NULL);
							SetMenuStrip(tcp_Window, tcp_MenuStrip);
							retval = 1;
						}
					}
				}
			}
		}
		//if(tcp_GadList && tcp_MenuStrip && tcp_Window) retval = 1;
	}
	return(retval);
}


WORD HandleMainMessages(void)
{
	WORD                quit = 0;
	struct IntuiMessage *imsg;
	ULONG               ev;
	UWORD               code;
	APTR                iadd;

	ULONG               oldww, oldwh, ww, wh;
	STRPTR              dateptr, timeptr;

	struct MenuItem     *item;
	ULONG               mid;

	STRPTR              fname;  /* For Project->Open and Project->SaveAs */
	static BOOL         saveicons = FALSE;

	oldww = tcp_Window->Width;
	oldwh = tcp_Window->Height;

	while(NULL != (imsg = GT_GetIMsg(tcp_Window->UserPort)) && !quit)
	{
		ev = imsg->Class;
		code = imsg->Code;
		iadd = imsg->IAddress;
		GT_ReplyIMsg(imsg);

		switch(ev)
		{
		case IDCMP_REFRESHWINDOW:
			GT_BeginRefresh(tcp_Window);
			GT_EndRefresh(tcp_Window, -1);
			break;

		case IDCMP_CLOSEWINDOW:
			quit = 1;
			break;

		case IDCMP_GADGETUP:
			//printf("gadget up=%u\n", ((struct Gadget *)iadd)->GadgetID);
			switch(((struct Gadget *)iadd)->GadgetID)
			{
			case GID_DATE_FORMAT:
				//printf("date format gadget up\n");
				GT_GetGadgetAttrs(tcp_Gadgets[GID_DATE_FORMAT], tcp_Window, NULL, GTST_String, &dateptr, TAG_DONE);
				ShowExample(dateptr, GID_DATE_EXAMPLE, date_ex, sizeof(date_ex));
				break;

			case GID_TIME_FORMAT:
				//printf("time format gadget up\n");
				GT_GetGadgetAttrs(tcp_Gadgets[GID_TIME_FORMAT], tcp_Window, NULL, GTST_String, &timeptr, TAG_DONE);
				//printf("contents of string gadget='%s'\n", timeptr);
				ShowExample(timeptr, GID_TIME_EXAMPLE, time_ex, sizeof(time_ex));
				//printf("exaaaample='%s'\n", time_ex);
				break;

			case GID_SAVE:
				//printf("save hit\n");
				GT_GetGadgetAttrs(tcp_Gadgets[GID_DATE_FORMAT], tcp_Window, NULL, GTST_String, &dateptr, TAG_DONE);
				GT_GetGadgetAttrs(tcp_Gadgets[GID_TIME_FORMAT], tcp_Window, NULL, GTST_String, &timeptr, TAG_DONE);
				SavePrefs(dateptr, timeptr);
				if(saveicons)
				{
					SaveIcon(SAVENAME);
					SaveIcon(USENAME);
				}
				ForcePluginRead();
				quit = 1;
				break;

			case GID_USE:
				//printf("use\n");
				GT_GetGadgetAttrs(tcp_Gadgets[GID_DATE_FORMAT], tcp_Window, NULL, GTST_String, &dateptr, TAG_DONE);
				GT_GetGadgetAttrs(tcp_Gadgets[GID_TIME_FORMAT], tcp_Window, NULL, GTST_String, &timeptr, TAG_DONE);
				UsePrefs(dateptr, timeptr);
				if(saveicons)
				{
					SaveIcon(USENAME);
				}
				ForcePluginRead();
			case GID_CANCEL:
				//printf("cancel/use exit\n");
				quit = 1;
				break;
			}
			break;

		case IDCMP_MENUPICK:
			while(code != MENUNULL)
			{
				item = ItemAddress(tcp_MenuStrip, code);

				/* */
				//printf("menu pick, item address=%08lx, menunum=%u\n", (ULONG)item, code);
				mid = (ULONG)GTMENUITEM_USERDATA(item);
				//printf("Menu item hit: %lu\n", mid);
				switch(mid)
				{
				case MID_PROJECT_OPEN:
					fname = OpenFileReq(tcp_Window);
					if(fname)
					{
						ReadPrefsFile(fname, datestr, FORMAT_SIZE, timestr, FORMAT_SIZE);
						GT_SetGadgetAttrs(tcp_Gadgets[GID_DATE_FORMAT], tcp_Window, NULL, GTST_String, datestr, TAG_DONE);
						GT_SetGadgetAttrs(tcp_Gadgets[GID_TIME_FORMAT], tcp_Window, NULL, GTST_String, timestr, TAG_DONE);
						ShowExample(datestr, GID_DATE_EXAMPLE, date_ex, sizeof(date_ex));
						ShowExample(timestr, GID_TIME_EXAMPLE, time_ex, sizeof(time_ex));
						FreeVec(fname);
					}
					break;

				case MID_PROJECT_SAVEAS:
					GT_GetGadgetAttrs(tcp_Gadgets[GID_DATE_FORMAT], tcp_Window, NULL, GTST_String, &dateptr, TAG_DONE);
					GT_GetGadgetAttrs(tcp_Gadgets[GID_TIME_FORMAT], tcp_Window, NULL, GTST_String, &timeptr, TAG_DONE);
					fname = SaveFileReq(tcp_Window);
					if(fname)
					{
						WritePrefsFile(fname, dateptr, timeptr);
						if(saveicons) SaveIcon(fname);
						FreeVec(fname);
					}
					break;

				case MID_PROJECT_QUIT:
					quit = 1;
					break;

				case MID_EDIT_RESET:
					GT_SetGadgetAttrs(tcp_Gadgets[GID_DATE_FORMAT], tcp_Window, NULL, GTST_String, "", TAG_DONE);
					GT_SetGadgetAttrs(tcp_Gadgets[GID_TIME_FORMAT], tcp_Window, NULL, GTST_String, "", TAG_DONE);
					ShowExample("", GID_DATE_EXAMPLE, date_ex, sizeof(date_ex));
					ShowExample("", GID_TIME_EXAMPLE, time_ex, sizeof(time_ex));
					break;

				case MID_EDIT_LASTSAVED:
					ReadPrefsFile(SAVENAME, datestr, FORMAT_SIZE, timestr, FORMAT_SIZE);
					ShowExample(datestr, GID_DATE_EXAMPLE, date_ex, sizeof(date_ex));
					ShowExample(timestr, GID_TIME_EXAMPLE, time_ex, sizeof(time_ex));
					break;

				case MID_EDIT_RESTORE:
					GT_SetGadgetAttrs(tcp_Gadgets[GID_DATE_FORMAT], tcp_Window, NULL, GTST_String, date_re, TAG_DONE);
					GT_SetGadgetAttrs(tcp_Gadgets[GID_TIME_FORMAT], tcp_Window, NULL, GTST_String, time_re, TAG_DONE);
					ShowExample(date_re, GID_DATE_EXAMPLE, date_ex, sizeof(date_ex));
					ShowExample(time_re, GID_TIME_EXAMPLE, time_ex, sizeof(time_ex));
					break;

				case MID_SETTINGS_CREATEICONS:
					if(CHECKED == item->Flags & CHECKED)
					{
						saveicons = TRUE;
					}
					else
					{
						saveicons = FALSE;
					}
					break;

				default:
					break;
				}

				code = item->NextSelect;
			}
			break;

		case IDCMP_SIZEVERIFY:
			oldww = tcp_Window->Width;
			oldwh = tcp_Window->Height;
			RemoveGList(tcp_Window, tcp_GadList, -1);
			break;

		case IDCMP_CHANGEWINDOW:
			RemoveGList(tcp_Window, tcp_GadList, -1);
		case IDCMP_NEWSIZE:
			ww = tcp_Window->Width;
			wh = tcp_Window->Height;

			if(oldww != ww || oldwh != wh)
			{
				// Save contents of gadgets
				GT_GetGadgetAttrs(tcp_Gadgets[GID_DATE_FORMAT], tcp_Window, NULL, GTST_String, &dateptr, TAG_DONE);
				strcpy(datestr, dateptr);

				GT_GetGadgetAttrs(tcp_Gadgets[GID_TIME_FORMAT], tcp_Window, NULL, GTST_String, &timeptr, TAG_DONE);
				strcpy(timestr, timeptr);

				// Delete gadget list
				FreeMainGadgets(tcp_GadList);

				// Re-make gadgets
				//ShowExample(datestr, NULL, date_ex, sizeof(date_ex));
				//ShowExample(timestr, NULL, time_ex, sizeof(time_ex));
				tcp_GadList = CreateMainGadgets(tcp_Screen, tcp_vi, ww, wh, tcp_UseFont);

				oldww = ww; oldwh = wh;

				RefreshWindowFrame(tcp_Window);
				//SetRast(tcp_Window->RPort, tcp_Screen_dri->dri_Pens[BACKGROUNDPEN]);
				SetAPen(tcp_Window->RPort, tcp_Screen_dri->dri_Pens[BACKGROUNDPEN]);
				RectFill(tcp_Window->RPort, tcp_Window->BorderLeft, tcp_Window->BorderTop, tcp_Window->Width-tcp_Window->BorderRight-1,tcp_Window->Height-tcp_Window->BorderBottom-1);
			}

			if(tcp_GadList)
			{
				// Attach gadgets
				AddGList(tcp_Window, tcp_GadList, -1, -1, NULL);

				// Refresh gadgets
				RefreshGList(tcp_GadList, tcp_Window, NULL, -1);

				// Refresh window frame
				GT_RefreshWindow(tcp_Window, NULL);
			}
			else
			{
				quit = 1;
			}
			break;
		}
	}

	return(quit);
}


static M68KFUNC_P3(void, FormatDateHookFunc,
	A0, struct Hook *, theHook,
	A2, struct Locale *, locale,
	A1, char, ch)
{
	struct FormatDateHookData *fd = (struct FormatDateHookData *) theHook->h_Data;

	(void) locale;

	if (fd->fdhd_Length >= 1)
		{
		*(fd->fdhd_Buffer)++ = ch;
		fd->fdhd_Length--;
		}
}
M68KFUNC_END


void ShowExample(STRPTR format, UWORD gid, STRPTR buffer, size_t BuffLen)
{
	struct FormatDateHookData fd;
	struct Hook         fmthook;
	struct Locale       *loc;
	struct DateStamp    ds;
	STATIC_PATCHFUNC(FormatDateHookFunc)

	d1(KPrintF(__FILE__ "/%s/%ld: START\n", __FUNC__, __LINE__));

	fmthook.h_Entry = (HOOKFUNC) PATCH_NEWFUNC(FormatDateHookFunc);
	fmthook.h_Data = &fd;

	fd.fdhd_Buffer = buffer;
	fd.fdhd_Length = BuffLen;

	d1(KPrintF(__FILE__ "/%s/%ld: format buffer=<%s>\n", __FUNC__, __LINE__, format));

	loc = OpenLocale(NULL);     // Guaranteed to suck seeds
	DateStamp(&ds);
	if(*format)
        {
		FormatDate(loc, format, &ds, &fmthook);
	}
	else
	{
		FormatDate(loc, (GID_DATE_EXAMPLE == gid ? loc->loc_DateFormat : loc->loc_TimeFormat), &ds, &fmthook);
	}

	d1(KPrintF(__FILE__ "/%s/%ld: example string=<%s>\n", __FUNC__, __LINE__, buffer));

	CloseLocale(loc);

	if(tcp_Gadgets[gid])
		{
		GT_SetGadgetAttrs(tcp_Gadgets[gid], tcp_Window, NULL,
			GTTX_Text, buffer, TAG_DONE);
		}
}

