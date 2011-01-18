/*
 * @(#) $Header$
 *
 * BGUIDemo.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.6  2004/06/20 12:24:32  verhaegs
 * Use REGFUNC macro's in BGUI source code, not AROS_UFH
 *
 * Revision 42.5  2004/06/16 23:59:30  chodorowski
 * Added missing AROS_USERFUNC_INIT/EXIT.
 *
 * Revision 42.4  2003/01/18 19:10:09  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.3  2000/08/10 11:50:54  chodorowski
 * Cleaned up and prettyfied the GUIs a bit.
 *
 * Revision 42.2  2000/07/04 05:02:22  bergers
 * Made examples compilable.
 *
 * Revision 42.1  2000/05/15 19:29:50  stegerg
 * replacements for REG macro.
 *
 * Revision 42.0  2000/05/09 22:19:21  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:12  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:33  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.5  1999/02/19 05:03:47  mlemos
 * Added support to build with Storm C.
 *
 * Revision 1.1.2.4  1998/12/08 03:54:25  mlemos
 * Associated tooltip texts to the buttons of the main window.
 *
 * Revision 1.1.2.3  1998/12/07 00:12:46  mlemos
 * Made the pages tabs and radio button be activated usning the keyboard.
 *
 * Revision 1.1.2.2  1998/03/01 20:32:48  mlemos
 * Ensured that the stack is never less than 8000 bytes.
 *
 * Revision 1.1.2.1  1998/02/28 17:44:57  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc BGUIDemo.c -proto -mi -ms -mRR -lbgui
quit
*/

#ifndef __STORM__
long __stack=8000;
#endif /* __STORM__ */

#include "DemoCode.h"

/*
** Online-Help texts.
**/
UBYTE    *MainHelp      = ISEQ_C "BGUI is a shared library which offers a set of\n"
		 "BOOPSI classes to allow for easy and flexible GUI creation.\n\n"
		 "The main window is also an AppWindow. Drop some icons\n"
		 "on it and see what happens.\n\n"
		 "All windows also detect the aspect ratio of the screen they are\n"
		 "located on and adjust frame thickness accoording to this.\n\n"
		 "All other windows in this demo also have online-help. To access\n"
		 "this help press the " ISEQ_B "HELP" ISEQ_N " key when the window is active.";

UBYTE    *GroupsHelp    = ISEQ_C "The BGUI layout engine is encapsulated in the groupclass.\n"
		 "The groupclass will layout all of it's members into a specific area.\n"
		 "You can pass layout specific attributes to all group members\n"
		 "which allows for flexible and powerful layout capabilities.";

UBYTE    *NotifHelp     = ISEQ_C "Notification can be used to let an object keep one or\n"
		 "more other objects informed about it's status. BGUI offers several\n"
		 "kinds of notification of which two (conditional and map-list) are\n"
		 "shown in this demonstration.";

UBYTE    *InfoHelp      = ISEQ_C "Not much more can be said about the BGUI infoclass than\n"
		 "is said in this window. Except maybe that this text is shown in an\n"
		 "infoclass object as are all body texts from a BGUI requester.";

UBYTE    *ImageHelp     = ISEQ_C "This window shows you the built-in images that BGUI has\n"
		 "to offer. Ofcourse these images are all scalable and it is possible\n"
		 "to create your own, scalable, imagery with the BGUI vectorclass.";

UBYTE    *BackfillHelp     = ISEQ_C "Here you see the built-in backfill patterns BGUI supports.\n"
		 "These backfill patterns can all be used in groups and frames.\n"
		 "The frameclass also offers you the possibility to add hooks for\n"
		 "custom backfills and frame rendering.\n\n"
		 "The bottom frame shows you a custom backfill hook which renders a\n"
		 "simple pattern known from the WBPattern prefs editor as background.";

UBYTE    *PagesHelp     = ISEQ_C "The pageclass allows you to setup a set of pages containing\n"
		 "BGUI gadgets or groups. This will give you the oppertunity to\n"
		 "have several set's of gadgets in a single window.\n\n"
		 "This window has a IDCMP-hook installed which allows you to\n"
		 "control the Tabs object with your TAB key.";


/*
** Window objects.
**/
Object      *WA_Main  = NULL, *WA_Groups = NULL, *WA_Notif = NULL;
Object      *WA_Info  = NULL, *WA_Image  = NULL, *WA_BFill = NULL;
Object      *WA_Pages = NULL;

/*
** Gadget objects from the main window.
**/
Object      *BT_Groups,   *BT_Notif,    *BT_Quit;
Object      *BT_Info,     *BT_Images,   *BT_BFill;
Object      *BT_Pages,    *BT_IconDone, *BT_IconQuit;
Object      *LV_IconList, *PG_Pager;

/*
** One, shared, message port for all
** demo windows.
**/
struct MsgPort *SharedPort;

/*
** Menus & gadget ID's.
**/
#define ID_ABOUT     1L
#define ID_QUIT                 2L

/*
** A small menu strip.
**/
struct NewMenu MainMenus[] = {
   Title( "Project" ),
      Item( "About...", "?", ID_ABOUT ),
      ItemBar,
      Item( "Quit",     "Q", ID_QUIT  ),
   End
};

/*
** Put up a simple requester.
**/
ULONG Req( struct Window *win, UBYTE *gadgets, UBYTE *body, ... )
{
   struct bguiRequest   req = { NULL };

   req.br_GadgetFormat  = gadgets;
   req.br_TextFormat = body;
   req.br_Flags      = BREQF_CENTERWINDOW|BREQF_XEN_BUTTONS|BREQF_AUTO_ASPECT;

   return( BGUI_RequestA( win, &req, ( ULONG * )( &body + 1 )));
}

/*
** Main window button ID's.
**/
#define ID_MAIN_GROUPS     3L
#define ID_MAIN_NOTIF      4L
#define ID_MAIN_INFO    5L
#define ID_MAIN_IMAGE      6L
#define ID_MAIN_BFILL      7L
#define ID_MAIN_PAGES      8L
#define ID_MAIN_ICON_CONT  9L

/*
** Open main window.
**/
struct Window *OpenMainWindow( ULONG *appmask )
{
   struct Window     *window = NULL;

   WA_Main = WindowObject,
      WINDOW_Title,           "BGUI Demo",
      WINDOW_ScreenTitle,     "BGUI Demo - ©1996 Ian J. Einman, ©1993-1995 Jaba Development.",
      WINDOW_MenuStrip,       MainMenus,
      WINDOW_SmartRefresh,    TRUE,
      WINDOW_AppWindow,       TRUE,
      WINDOW_SizeGadget,      FALSE,
      WINDOW_HelpText,        MainHelp,
      WINDOW_AutoAspect,      TRUE,
      WINDOW_SharedPort,      SharedPort,
      WINDOW_AutoKeyLabel,    TRUE,
      WINDOW_ScaleWidth,      10,
      WINDOW_CloseOnEsc,      TRUE,
      WINDOW_MasterGroup,
	 VGroupObject, NormalOffset, NormalSpacing, GROUP_BackFill, SHINE_RASTER,
	    StartMember,
	       PG_Pager = PageObject,
		  /*
		  ** Main page.
		  **/
		  PageMember,
		     VGroupObject, WideSpacing, GROUP_BackFill, SHINE_RASTER,
			StartMember,
			   InfoObject,
			      INFO_TextFormat, ISEQ_C
				       ISEQ_HIGHLIGHT
				       "\033s\033bBGUI Demo\n"
				       ISEQ_TEXT
				       ISEQ_N
				       "©1996 Ian J. Einman\n"
				       "©1993-1995 Jaba Development\n\n"
				       "\033iPress the HELP key for more info.",
			      INFO_FixTextWidth,   TRUE,
			      INFO_MinLines,    5,
			      FRM_Type,      FRTYPE_NEXT,
			   EndObject,
			EndMember,
			StartMember,
			   HGroupObject, NormalSpacing,
			      StartMember,
				 VGroupObject, NormalSpacing,
				    StartMember,
				       BT_Groups = ButtonObject,
					  LAB_Label, "_Groups",
					  GA_ID, ID_MAIN_GROUPS, 
					  BT_ToolTip, "Shows different group layouts",
				       EndObject,
				    EndMember,
				    StartMember,
				       BT_Notif = ButtonObject,
					  LAB_Label, "_Notification",
					  GA_ID, ID_MAIN_NOTIF, 
					  BT_ToolTip, "Shows notification between gadgets",
				       EndObject,
				    EndMember,
				    VarSpace( DEFAULT_WEIGHT ),
				 EndObject,
			      EndMember,
			      StartMember,
				 VGroupObject, NormalSpacing,
				    StartMember,
				       BT_Images = ButtonObject,
					  LAB_Label, "_Images",
					  GA_ID, ID_MAIN_IMAGE, 
					  BT_ToolTip, "Shows different types of built-in images",
				       EndObject,
				    EndMember,
				    StartMember,
				       BT_BFill = ButtonObject,
					  LAB_Label, "_BackFill",
					  GA_ID, ID_MAIN_BFILL, 
					  BT_ToolTip, "Shows different types of background filling",
				       EndObject,
				    EndMember,
				    StartMember,
				       BT_Quit = ButtonObject,
					  LAB_Label, "_Quit",
					  GA_ID, ID_QUIT, 
					  BT_ToolTip, "Quits this program",
				       EndObject,
				    EndMember,
				 EndObject,
			      EndMember,
			      StartMember,
				 VGroupObject, NormalSpacing,
				    StartMember,
				       BT_Pages = ButtonObject,
					  LAB_Label, "_Pages",
					  GA_ID, ID_MAIN_PAGES, 
					  BT_ToolTip, "Shows paged of groups",
				       EndObject,
				    EndMember,
				    StartMember,
				       BT_Info = ButtonObject,
					  LAB_Label, "Info_Class",
					  GA_ID, ID_MAIN_INFO,
					  BT_ToolTip, "Shows the Information gadget",
				       EndObject,
				    EndMember,
				    VarSpace( DEFAULT_WEIGHT ),
				 EndObject,
			      EndMember,
			   EndObject, FixMinHeight, EqualWidth,
			EndMember,
		     EndObject,
		  /*
		  ** Icon-drop list page.
		  **/
		  PageMember,
		     VGroupObject, NormalSpacing, GROUP_BackFill, SHINE_RASTER,
			StartMember,
			   InfoObject,
			      INFO_TextFormat,  ISEQ_C
				       "The following icons were dropped\n"
				       "in the window:",
			      INFO_FixTextWidth,   TRUE,
			      INFO_MinLines,    2,
			      INFO_HorizOffset, 13,
			      FRM_Type,      FRTYPE_BUTTON,
			      FRM_Recessed,     TRUE,
			   EndObject, FixMinHeight,
			EndMember,
			StartMember,
			   LV_IconList = ListviewObject,
			      LISTV_ReadOnly,         TRUE,
			   EndObject,
			EndMember,
			StartMember,
			   HGroupObject,
			      StartMember, BT_IconDone = PrefButton( "_Continue", ID_MAIN_ICON_CONT ), EndMember,
			      VarSpace( DEFAULT_WEIGHT ),
			      StartMember, BT_IconQuit = PrefButton( "_Quit",     ID_QUIT ), EndMember,
			   EndObject, FixMinHeight,
			EndMember,
		     EndObject,
	       EndObject,
	    EndMember,
	 EndObject,
   EndObject;

   /*
   ** Object created OK?
   **/
   if ( WA_Main ) {
      /*
      ** Open the window.
      **/
      if ( window = WindowOpen( WA_Main )) {
	 /*
	 ** Obtain appwindow signal mask.
	 **/
	 GetAttr( WINDOW_AppMask, WA_Main, appmask );
      }
   }

   return( window );
}

/*
** Macros for the group objects. GObj() creates
** a simple infoclass object with some text in
** it. TObj() creates a simple groupclass object
** with a button frame.
**/
#define GObj(t)\
   InfoObject,\
      INFO_TextFormat,  t,\
      INFO_FixTextWidth,   TRUE,\
      INFO_HorizOffset, 4,\
      INFO_VertOffset,  3,\
      ButtonFrame,\
      FRM_Flags,     FRF_RECESSED,\
   EndObject

#define NWObj(v,id)\
   StringObject,\
      STRINGA_LongVal,       v,\
      STRINGA_MaxChars,      3,\
      STRINGA_IntegerMin,    1,\
      STRINGA_IntegerMax,    999,\
      STRINGA_Justification, GACT_STRINGCENTER,\
      GA_ID,                 id,\
      FuzzRidgeFrame,\
   EndObject, Weight(v)

#define TObj\
   HGroupObject, HOffset(4), VOffset(3),\
      ButtonFrame,\
      FRM_BackFill,  FILL_RASTER,\
      FRM_Flags,     FRF_RECESSED,\
   EndObject

#define ID_GROUP_W0 500
#define ID_GROUP_W1 501
#define ID_GROUP_W2 502
#define ID_GROUP_W3 503

Object *w[4] = { NULL, NULL, NULL, NULL };

/*
** Open up the groups window.
**/
struct Window *OpenGroupsWindow( void )
{
   struct Window        *window = NULL;

   /*
   ** If the object has not been created
   ** already we build it.
   **/
   if ( ! WA_Groups ) {
      WA_Groups = WindowObject,
	 WINDOW_Title,        "BGUI Groups",
	 WINDOW_RMBTrap,      TRUE,
	 WINDOW_SmartRefresh, TRUE,
	 WINDOW_HelpText,     GroupsHelp,
	 WINDOW_AutoAspect,   TRUE,
	 WINDOW_SharedPort,   SharedPort,
	 WINDOW_ScaleWidth,   20,
	 WINDOW_ScaleHeight,  20,
	 WINDOW_MasterGroup,
	    VGroupObject, NormalOffset, NormalSpacing,
	       StartMember,
		  HGroupObject, WideSpacing,
		     StartMember,
			HGroupObject, NeXTFrame, FrameTitle( "Horizontal" ), NormalSpacing,
			   NormalHOffset, TOffset(GRSPACE_NARROW), BOffset(GRSPACE_NORMAL),
			   StartMember, TObj, EndMember,
			   StartMember, TObj, EndMember,
			   StartMember, TObj, EndMember,
			EndObject,
		     EndMember,
		     StartMember,
			VGroupObject, NeXTFrame, FrameTitle( "Vertical" ), NormalSpacing,
			   NormalHOffset, TOffset(GRSPACE_NARROW), BOffset(GRSPACE_NORMAL),
			   StartMember, TObj, EndMember,
			   StartMember, TObj, EndMember,
			   StartMember, TObj, EndMember,
			EndObject,
		     EndMember,
		     StartMember,
			VGroupObject, NeXTFrame, FrameTitle( "Grid" ), NormalSpacing,
			   NormalHOffset, TOffset(GRSPACE_NARROW), BOffset(GRSPACE_NORMAL),
			   StartMember,
			      HGroupObject, NormalSpacing,
				 StartMember, TObj, EndMember,
				 StartMember, TObj, EndMember,
				 StartMember, TObj, EndMember,
			      EndObject,
			   EndMember,
			   StartMember,
			      HGroupObject, NormalSpacing,
				 StartMember, TObj, EndMember,
				 StartMember, TObj, EndMember,
				 StartMember, TObj, EndMember,
			      EndObject,
			   EndMember,
			   StartMember,
			      HGroupObject, NormalSpacing,
				 StartMember, TObj, EndMember,
				 StartMember, TObj, EndMember,
				 StartMember, TObj, EndMember,
			      EndObject,
			   EndMember,
			EndObject,
		     EndMember,
		  EndObject,
	       EndMember,
	       StartMember,
		  VGroupObject, NormalSpacing, FrameTitle("Free, Fixed and Weight sizes."), NeXTFrame,
		     NormalHOffset, TOffset(GRSPACE_NARROW), BOffset(GRSPACE_NORMAL),
		     StartMember,
			HGroupObject, NormalSpacing,
			   StartMember, w[0] = NWObj( 25, ID_GROUP_W0), EndMember,
			   StartMember, w[1] = NWObj( 50, ID_GROUP_W1), EndMember,
			   StartMember, w[2] = NWObj( 75, ID_GROUP_W2), EndMember,
			   StartMember, w[3] = NWObj(100, ID_GROUP_W3), EndMember,
			EndObject,
		     EndMember,
		     StartMember,
			HGroupObject, NormalSpacing,
			   StartMember, GObj( ISEQ_C   "Free"  ), EndMember,
			   StartMember, GObj( ISEQ_C ISEQ_B "Fixed" ), FixMinWidth, EndMember,
			   StartMember, GObj( ISEQ_C   "Free"  ), EndMember,
			   StartMember, GObj( ISEQ_C ISEQ_B "Fixed" ), FixMinWidth, EndMember,
			EndObject,
		     EndMember,
		  EndObject, FixMinHeight,
	       EndMember,
	    EndObject,
      EndObject;
   }

   /*
   ** Object OK?
   **/
   if ( WA_Groups ) {
      /*
      ** Open the window.
      **/
      window = WindowOpen( WA_Groups );
   }

   return( window );
}

/*
** Cycle gadget labels.
**/
UBYTE *NotifLabels[] = { "Enabled-->", "Disabled-->", "Still Disabled-->", NULL };

/*
** Notification map-lists.
**/
ULONG pga2sl[] = { PGA_Top,       SLIDER_Level,  TAG_END };
ULONG sl2prg[] = { SLIDER_Level,  PROGRESS_Done, TAG_END };
ULONG prg2in[] = { PROGRESS_Done, INDIC_Level,   TAG_END };

/*
** Open the notification window.
**/
struct Window *OpenNotifWindow( void )
{
   struct Window        *window = NULL;
   Object            *c, *b, *p1, *p2, *s1, *s2, *p, *i1, *i2;

   /*
   ** Not created yet? Create it now!
   **/
   if ( ! WA_Notif ) {
      WA_Notif = WindowObject,
	 WINDOW_Title,     "BGUI notification",
	 WINDOW_RMBTrap,         TRUE,
	 WINDOW_SmartRefresh, TRUE,
	 WINDOW_HelpText,  NotifHelp,
	 WINDOW_AutoAspect,   TRUE,
	 WINDOW_SharedPort,   SharedPort,
	 WINDOW_MasterGroup,
	    VGroupObject, NormalOffset, NormalSpacing,
	       StartMember, TitleSeperator( "Conditional" ), EndMember,
	       StartMember,
		  HGroupObject, NormalSpacing,
		     StartMember, c = PrefCycle( NULL, NotifLabels, 0, 0 ), EndMember,
		     StartMember, b = PrefButton( "Target", 0 ), EndMember,
		  EndObject, FixMinHeight,
	       EndMember,
	       StartMember, TitleSeperator( "Map-List" ), EndMember,
	       StartMember,
		  HGroupObject, NormalSpacing,
		     StartMember,
			VGroupObject, NormalSpacing,
			   StartMember, i1 = IndicatorFormat( 0, 100, 0, IDJ_CENTER, "%ld%%" ), FixMinHeight, EndMember,
			   StartMember, p1 = HorizProgress( NULL, 0, 100, 0 ), EndMember,
			EndObject,
		     EndMember,
		     StartMember, s1 = VertSlider( NULL, 0, 100, 0, 0 ), FixWidth( 16 ), EndMember,
		     StartMember, p = VertScroller( NULL, 0, 101, 1, 0 ), FixWidth( 16 ), EndMember,
		     StartMember, s2 = VertSlider( NULL, 0, 100, 0, 0 ), FixWidth( 16 ), EndMember,
		     StartMember,
			VGroupObject, NormalSpacing,
			   StartMember, i2 = IndicatorFormat( 0, 100, 0, IDJ_CENTER, "%ld%%" ), FixMinHeight, EndMember,
			   StartMember, p2 = VertProgress( NULL, 0, 100, 0 ), EndMember,
			EndObject,
		     EndMember,
		  EndObject,
	       EndMember,
	    EndObject,
      EndObject;

      if ( WA_Notif ) {
	 /*
	 ** Connect the cycle object with the button.
	 **/
	 AddCondit( c, b, CYC_Active, 0, GA_Disabled, FALSE, GA_Disabled, TRUE );
	 /*
	 ** Connect sliders, prop, progression and indicators.
	 **/
	 AddMap( s1, p1, sl2prg );
	 AddMap( s2, p2, sl2prg );
	 AddMap( p,  s1, pga2sl );
	 AddMap( p,  s2, pga2sl );
	 AddMap( p1, i1, prg2in );
	 AddMap( p2, i2, prg2in );
      }
   }

   /*
   ** Object OK?
   **/
   if ( WA_Notif ) {
      /*
      ** Open window.
      **/
      window = WindowOpen( WA_Notif );
   }

   return( window );
}

/*
** Open infoclass window.
**/
struct Window *OpenInfoWindow( void )
{
   struct Window        *window = NULL;
   ULONG           args[2];

   /*
   ** Setup arguments for the
   ** infoclass object.
   **/
   args[0] = AvailMem( MEMF_CHIP );
   args[1] = AvailMem( MEMF_FAST );

   /*
   ** Not created already?
   **/
   if ( ! WA_Info ) {
      WA_Info = WindowObject,
	 WINDOW_Title,     "BGUI information class",
	 WINDOW_RMBTrap,         TRUE,
	 WINDOW_SmartRefresh, TRUE,
	 WINDOW_HelpText,  InfoHelp,
	 WINDOW_AutoAspect,   TRUE,
	 WINDOW_SharedPort,   SharedPort,
	 WINDOW_MasterGroup,
	    VGroupObject, HOffset( 4 ), VOffset( 4 ),
	       StartMember,
		  InfoFixed( NULL,
			ISEQ_C "BGUI offers the InfoClass.\n"
			"This class is a text display class which\n"
			"allows things like:\n\n"
			ISEQ_SHINE    "C"
			ISEQ_SHADOW   "o"
			ISEQ_FILL     "l"
			ISEQ_FILLTEXT  "o"
			ISEQ_HIGHLIGHT "r"
			ISEQ_TEXT     "s\n\n"
			ISEQ_L "Left Aligned...\n"
			ISEQ_R "Right Aligned...\n"
			ISEQ_C "Centered...\n\n"
			ISEQ_B "Bold...\n"   ISEQ_N
			ISEQ_I "Italic...\n" ISEQ_N
			ISEQ_U "Underlined...\n\n"
			ISEQ_B ISEQ_I "And combinations!\n\n"
			ISEQ_N "Free CHIP:" ISEQ_SHINE " %lD" ISEQ_TEXT", Free FAST:" ISEQ_SHINE " %lD\n",
			&args[ 0 ],
			17 ),
	       EndMember,
	    EndObject,
      EndObject;
   }

   /*
   ** Object OK?
   **/
   if ( WA_Info ) {
      /*
      ** Open window.
      **/
      window = WindowOpen( WA_Info );
   }

   return( window );
}

/*
** Open images window.
**/
struct Window *OpenImageWindow( void )
{
   struct Window        *window = NULL;

   /*
   ** Not yet created?
   **/
   if ( ! WA_Image ) {
      WA_Image = WindowObject,
	 WINDOW_Title,        "BGUI images",
	 WINDOW_RMBTrap,      TRUE,
	 WINDOW_SmartRefresh, TRUE,
	 WINDOW_HelpText,     ImageHelp,
	 WINDOW_AutoAspect,   TRUE,
	 WINDOW_SharedPort,   SharedPort,
	 WINDOW_ScaleHeight,  10,
	 WINDOW_MasterGroup,
	    VGroupObject, NormalOffset, WideSpacing,
	       StartMember,
		  HGroupObject, TOffset(GRSPACE_NARROW), BOffset(GRSPACE_NORMAL), NormalSpacing, NeXTFrame, FrameTitle("Fixed size"),
		     VarSpace(DEFAULT_WEIGHT),
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_GETPATH,     ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_GETFILE,     ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_POPUP,       ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_CYCLE,       ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_CYCLE2,      ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_CHECKMARK,   ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_ARROW_UP,    ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_ARROW_DOWN,  ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_ARROW_LEFT,  ButtonFrame, EndObject, FixMinSize, EndMember,
		     StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_ARROW_RIGHT, ButtonFrame, EndObject, FixMinSize, EndMember,
		     VarSpace(DEFAULT_WEIGHT),
		  EndObject, FixMinHeight,
	       EndMember,
	       StartMember,
		  VGroupObject, NormalHOffset, TOffset(GRSPACE_NARROW), BOffset(GRSPACE_NORMAL), NormalSpacing, NeXTFrame, FrameTitle("Free size"),
		     StartMember,
			HGroupObject, NormalSpacing,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_GETPATH,     ButtonFrame, EndObject, EndMember,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_GETFILE,     ButtonFrame, EndObject, EndMember,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_POPUP,       ButtonFrame, EndObject, EndMember,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_CYCLE,       ButtonFrame, EndObject, EndMember,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_CYCLE2,      ButtonFrame, EndObject, EndMember,
			EndObject,
		     EndMember,
		     StartMember,
			HGroupObject, NormalSpacing,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_CHECKMARK,   ButtonFrame, EndObject, EndMember,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_ARROW_UP,    ButtonFrame, EndObject, EndMember,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_ARROW_DOWN,  ButtonFrame, EndObject, EndMember,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_ARROW_LEFT,  ButtonFrame, EndObject, EndMember,
			   StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_ARROW_RIGHT, ButtonFrame, EndObject, EndMember,
			EndObject,
		     EndMember,
		  EndObject,
	       EndMember,
	    EndObject,
      EndObject;
   }

   /*
   ** Object OK?
   **/
   if ( WA_Image ) {
      /*
      ** Open the window.
      **/
      window = WindowOpen( WA_Image );
   }

   return( window );
}

/*
** The BackFill hook to show custom backfills.
** Renders a pattern from the WBPattern preferences
** editor as back-fill.
**/
#ifdef __STORM__
ULONG SAVEDS ASM
#else
SAVEDS ASM ULONG
#endif

//BackFillHook( REG(a0) struct Hook *hook, REG(a2) Object *imo, REG(a1) struct FrameDrawMsg *fdm )
REGFUNC3(, BackFillHook,
	 REGPARAM(A0, struct Hook *, hook),
	 REGPARAM(A2, Object *, imo),
	 REGPARAM(A1, struct FrameDrawMsg *, fdm)
)
{
   UWORD pat[] =
   {
      0x0000, 0x0000, 0x0002, 0x0002, 0x000A, 0x000A, 0x002A, 0x002A,
      0x00AA, 0x002A, 0x03EA, 0x000A, 0x0FFA, 0x0002, 0x3FFE, 0x0000,
      0x0000, 0x7FFC, 0x4000, 0x5FF0, 0x5000, 0x57C0, 0x5400, 0x5500,
      0x5400, 0x5400, 0x5000, 0x5000, 0x4000, 0x4000, 0x0000, 0x0000,
   };

   fdm->fdm_RPort->Mask = 0x03;
   SetAfPt( fdm->fdm_RPort, pat, -4 );
   SetAPen( fdm->fdm_RPort, ( 1 << fdm->fdm_DrawInfo->dri_Depth ) - 1 );
   RectFill( fdm->fdm_RPort, fdm->fdm_Bounds->MinX, fdm->fdm_Bounds->MinY,
	      fdm->fdm_Bounds->MaxX, fdm->fdm_Bounds->MaxY );
   SetAfPt( fdm->fdm_RPort, NULL, 0 );

   return( FRC_OK );
}
REGFUNC_END

/*
** The hook structure.
**/
struct Hook BackFill = {
   NULL, NULL, (HOOKFUNC)BackFillHook, NULL, NULL
};

/*
** Open back-fill window.
**/
struct Window *OpenFillWindow( void )
{
   struct Window        *window = NULL;
   struct Screen        *screen = LockPubScreen(NULL);

   static struct bguiPattern bp;
   
   /*
   ** Not yet created?
   **/
   if ( ! WA_BFill ) {
      WA_BFill = WindowObject,
	 WINDOW_Title,           "BGUI back fill patterns",
	 WINDOW_RMBTrap,         TRUE,
	 WINDOW_SmartRefresh,    TRUE,
	 WINDOW_HelpText,        BackfillHelp,
	 WINDOW_ScaleWidth,      40,
	 WINDOW_ScaleHeight,     40,
	 WINDOW_AutoAspect,      TRUE,
	 WINDOW_SharedPort,      SharedPort,
	 WINDOW_PubScreen,       screen,
	 WINDOW_MasterGroup,
	    HGroupObject, NormalOffset, WideSpacing,
	       StartMember,
		  VGroupObject, NormalOffset, NeXTFrame, FrameTitle("Raster Fill"), NormalSpacing,
		     StartMember,
			HGroupObject, NormalSpacing,
			   StartMember, InfoObject, ButtonFrame, ShineRaster,  EndObject, EndMember,
			   StartMember, InfoObject, ButtonFrame, ShadowRaster, EndObject, EndMember,
			   StartMember, InfoObject, ButtonFrame, ShineShadowRaster, EndObject, EndMember,
			   StartMember, InfoObject, ButtonFrame, FillRaster,   EndObject, EndMember,
			EndObject,
		     EndMember,
		     StartMember,
			HGroupObject, NormalSpacing,
			   StartMember, InfoObject, ButtonFrame, ShineFillRaster,   EndObject, EndMember,
			   StartMember, InfoObject, ButtonFrame, ShadowFillRaster, EndObject, EndMember,
			   StartMember, InfoObject, ButtonFrame, ShineBlock,  EndObject, EndMember,
			   StartMember, InfoObject, ButtonFrame, ShadowBlock, EndObject, EndMember,
			EndObject,
		     EndMember,
		  EndObject,
	       EndMember,
	       StartMember,
		  VGroupObject, NormalOffset, NeXTFrame, FrameTitle("Custom Hook"),
		     StartMember,
			InfoObject, ButtonFrame, FRM_BackFillHook, &BackFill, EndObject,
		     EndMember,
		  EndObject,
	       EndMember,
	       StartMember,
		  VGroupObject, NormalOffset, NeXTFrame, FrameTitle("Bitmap Pattern"),
		     StartMember,
			InfoObject, ButtonFrame, FRM_FillPattern, &bp, EndObject,
		     EndMember,
		  EndObject,
	       EndMember,
	    EndObject,
      EndObject;
   }
   UnlockPubScreen(NULL, screen);

   /*
   ** Object OK?
   **/
   if (WA_BFill)
   {
      bp.bp_Flags    = 0;
      bp.bp_Left     = 0;
      bp.bp_Top      = 0;
      bp.bp_Width    = 120;
      bp.bp_Height   = 80;
      bp.bp_BitMap   = screen->RastPort.BitMap;

      /*
      ** Open window.
      **/
      window = WindowOpen(WA_BFill);
   }

   return( window );
}

/*
** Cycle and Mx labels.
**/
UBYTE *PageLab[] = { "_Buttons", "_Strings", "_CheckBoxes", "_Radio-Buttons", NULL };
UBYTE *MxLab[]  = { "MX #_1",   "MX #_2",   "MX #_3",      "MX #_4",         NULL };

/*
** Cycle to Page map-list.
**/
ULONG Cyc2Page[] = { MX_Active, PAGE_Active, TAG_END };

/*
** Create a MX object with a title on top.
**/
#define MxGadget(label,labels)\
   MxObject,\
      GROUP_Style,      GRSTYLE_VERTICAL,\
      LAB_Label,     label,\
      LAB_Place,     PLACE_ABOVE,\
      LAB_Underscore,         '_',\
      LAB_Highlight,    TRUE,\
      MX_Labels,     labels,\
      MX_LabelPlace,    PLACE_LEFT,\
   EndObject, FixMinSize

/*
** Tabs-key control of the tabs gadget.
**/
#ifdef __STORM__
VOID SAVEDS ASM
#else
SAVEDS ASM VOID
#endif
#ifdef __AROS__
AROS_UFH3(,TabHookFunc,
	AROS_LHA(struct Hook *, hook, A0),
	AROS_LHA(Object *, obj, A2),
	AROS_LHA(struct IntuiMessage *, msg, A1))
#else
TabHookFunc( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct IntuiMessage *msg )
#endif
{
    AROS_USERFUNC_INIT

   struct Window        *window;
   Object            *mx_obj = ( Object * )hook->h_Data;
   ULONG           pos;

   /*
   ** Obtain window pointer and
   ** current tab position.
   **/
   GetAttr( WINDOW_Window, obj,  ( ULONG * )&window );
   GetAttr( MX_Active,  mx_obj, &pos );

   /*
   ** What key is pressed?
   **/
   if ( msg->Code == 0x42 ) {
      if ( msg->Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT )) pos--;
      else                          pos++;
      SetGadgetAttrs(( struct Gadget * )mx_obj, window, NULL, MX_Active, pos, TAG_END );
   }

    AROS_USERFUNC_EXIT
}

struct Hook TabHook = { NULL, NULL, ( HOOKFUNC )TabHookFunc, NULL, NULL };

/*
** Open pages window.
**/
struct Window *OpenPagesWindow( void )
{
   Object            *c, *p, *m, *s1, *s2, *s3;
   struct Window        *window = NULL;

   /*
   ** Not yet created?
   **/
   if ( ! WA_Pages ) {
      /*
      ** Create tabs-object.
      **/
      c = Tabs( NULL, PageLab, 0, 0 );

      /*
      ** Put it in the hook data.
      **/
      TabHook.h_Data = ( APTR )c;

      WA_Pages = WindowObject,
	 WINDOW_Title,           "BGUI pages",
	 WINDOW_RMBTrap,         TRUE,
	 WINDOW_SmartRefresh,    TRUE,
	 WINDOW_HelpText,        PagesHelp,
	 WINDOW_AutoAspect,      TRUE,
	 WINDOW_IDCMPHookBits,   IDCMP_RAWKEY,
	 WINDOW_IDCMPHook,       &TabHook,
	 WINDOW_SharedPort,      SharedPort,
	 WINDOW_AutoKeyLabel,    TRUE,
	 WINDOW_MasterGroup,
	    VGroupObject, NormalOffset, Spacing( 0 ),
	       StartMember, c, FixMinHeight, EndMember,
	       StartMember,
		  p = PageObject, FRM_Type, FRTYPE_TAB_ABOVE, Spacing(0),
		     /*
		     ** Button page.
		     **/
		     PageMember,
			VGroupObject, NormalOffset, NormalSpacing,
			   VarSpace(DEFAULT_WEIGHT),
			   StartMember, PrefButton( "Button #_1", 0 ), FixMinHeight, EndMember,
			   StartMember, PrefButton( "Button #_2", 0 ), FixMinHeight, EndMember,
			   StartMember, PrefButton( "Button #_3", 0 ), FixMinHeight, EndMember,
			   VarSpace(DEFAULT_WEIGHT),
			EndObject,
		     /*
		     ** String page.
		     **/
		     PageMember,
			VGroupObject, NormalOffset, NormalSpacing,
			   VarSpace(DEFAULT_WEIGHT),
			   StartMember, s1 = PrefString( "String #_1", "", 256, 0 ), FixMinHeight, EndMember,
			   StartMember, s2 = PrefString( "String #_2", "", 256, 0 ), FixMinHeight, EndMember,
			   StartMember, s3 = PrefString( "String #_3", "", 256, 0 ), FixMinHeight, EndMember,
			   VarSpace(DEFAULT_WEIGHT),
			EndObject,
		     /*
		     ** CheckBox page.
		     **/
		     PageMember,
			VGroupObject, NormalOffset, NormalSpacing,
			   VarSpace(DEFAULT_WEIGHT),
			   StartMember,
			      HGroupObject,
				 VarSpace(DEFAULT_WEIGHT),
				 StartMember,
				    VGroupObject, NormalSpacing,
				       StartMember, PrefCheckBox( "CheckBox #_1", FALSE, 0 ), EndMember,
				       StartMember, PrefCheckBox( "CheckBox #_2", FALSE, 0 ), EndMember,
				       StartMember, PrefCheckBox( "CheckBox #_3", FALSE, 0 ), EndMember,
				    EndObject, FixMinWidth,
				 EndMember,
				 VarSpace(DEFAULT_WEIGHT),
			      EndObject,
			   EndMember,
			   VarSpace(DEFAULT_WEIGHT),
			EndObject,
		     /*
		     ** Mx page.
		     **/
		     PageMember,
			VGroupObject, NormalOffset,
			   VarSpace(DEFAULT_WEIGHT),
			   StartMember,
			      HGroupObject,
				 VarSpace( DEFAULT_WEIGHT ),
				 StartMember, m = MxGadget( "_Mx Object", MxLab ), EndMember,
				 VarSpace( DEFAULT_WEIGHT ),
			      EndObject, FixMinHeight,
			   EndMember,
			   VarSpace(DEFAULT_WEIGHT),
			EndObject,
		  EndObject,
	       EndMember,
	    EndObject,
      EndObject;

      /*
      ** Object OK?
      **/
      if ( WA_Pages ) {
	 /*
	 ** Connect the cycle to the page.
	 **/
	 AddMap( c, p, Cyc2Page );
	 /*
	 ** Set tab-cycling order.
	 **/
	 DoMethod( WA_Pages, WM_TABCYCLE_ORDER, s1, s2, s3, NULL );
      }
   }

   /*
   ** Object OK?
   **/
   if ( WA_Pages ) {
      /*
      ** Open the window.
      **/
      window = WindowOpen( WA_Pages );
   }

   return( window );
}

/*
** Main entry.
**/
VOID StartDemo( void )
{
   struct Window         *main = NULL, *groups = NULL, *notif = NULL, *info = NULL, *image = NULL, *bfill = NULL, *pages = NULL, *sigwin = ( struct Window * )~0;
   struct AppMessage      *apm;
   struct WBArg          *ap;
   ULONG       sigmask = 0L, sigrec, rc, i, appsig = 0L, id;
   BOOL        running = TRUE;
   UBYTE       name[ 256 ];

   /*
   ** Create the shared message port.
   **/
   if ( SharedPort = CreateMsgPort()) {
      /*
      ** Open the main window.
      **/
      if ( main = OpenMainWindow( &appsig )) {
	 /*
	 ** OR signal masks.
	 **/
	 sigmask |= ( appsig | ( 1L << SharedPort->mp_SigBit ));
	 /*
	 ** Loop...
	 **/
	 do {
	    /*
	    ** Wait for the signals to come.
	    **/
	    sigrec = Wait( sigmask );

	    /*
	    ** AppWindow signal?
	    **/
	    if ( sigrec & appsig ) {
	       /*
	       ** Obtain AppWindow messages.
	       **/
	       while ( apm = GetAppMsg( WA_Main )) {
		  /*
		  ** Get all dropped icons.
		  **/
		  for ( ap = apm->am_ArgList, i = 0; i < apm->am_NumArgs; i++, ap++ ) {
		     /*
		     ** Build fully qualified name.
		     **/
		     NameFromLock( ap->wa_Lock, name, 256 );
		     AddPart( name, ap->wa_Name, 256 );
		     /*
		     ** Add it to the listview.
		     **/
		     AddEntry( main, LV_IconList, (APTR)name, LVAP_SORTED );
		  }
		  /*
		  ** Important! We must reply the message!
		  **/
		  ReplyMsg(( struct Message * )apm );
	       }
	       /*
	       ** Switch to the Icon page.
	       **/
	       SetGadgetAttrs(( struct Gadget * )PG_Pager, main, NULL, PAGE_Active, 1, TAG_END );
	    }

	    /*
	    ** Find out the which window signalled us.
	    **/
	    if ( sigrec & ( 1 << SharedPort->mp_SigBit )) {
	       while ( sigwin = GetSignalWindow( WA_Main )) {

		  /*
		  ** Main window signal?
		  **/
		  if ( sigwin == main ) {
		     /*
		     ** Call the main-window event handler.
		     **/
		     while (( rc = HandleEvent( WA_Main )) != WMHI_NOMORE ) {
			switch ( rc ) {

			   case  WMHI_CLOSEWINDOW:
			   case  ID_QUIT:
			      running = FALSE;
			      break;

			   case  ID_ABOUT:
			      Req( main, "OK", ISEQ_C ISEQ_B "\33d8BGUIDemo" ISEQ_N "\33d2\n(C) Copyright 1993-1995 Jaba Development" );
			      break;

			   case  ID_MAIN_GROUPS:
			      /*
			      ** Open groups window.
			      **/
			      if ( ! groups )
				 groups = OpenGroupsWindow();
			      break;

			   case  ID_MAIN_NOTIF:
			      /*
			      ** Open notification window.
			      **/
			      if ( ! notif )
				 notif = OpenNotifWindow();
			      break;

			   case  ID_MAIN_INFO:
			      /*
			      ** Open infoclass window.
			      **/
			      if ( ! info )
				 info = OpenInfoWindow();
			      break;

			   case  ID_MAIN_IMAGE:
			      /*
			      ** Open images window.
			      **/
			      if ( ! image )
				 image = OpenImageWindow();
			      break;

			   case  ID_MAIN_BFILL:
			      /*
			      ** Open backfill window.
			      **/
			      if ( ! bfill )
				 bfill = OpenFillWindow();
			      break;

			   case  ID_MAIN_PAGES:
			      /*
			      ** Open pages window.
			      **/
			      if ( ! pages )
				 pages = OpenPagesWindow();
			      break;

			   case  ID_MAIN_ICON_CONT:
			      /*
			      ** Switch back to the main page.
			      **/
			      SetGadgetAttrs(( struct Gadget * )PG_Pager, main, NULL, PAGE_Active, 0, TAG_END );
			      /*
			      ** Clear all entries from the listview.
			      **/
			      ClearList( main, LV_IconList );
			      break;
			}
		     }
		  }

		  /*
		  ** The code below will close the
		  ** specific window.
		  **/
		  if ( sigwin == groups ) {
		     while (( rc = HandleEvent( WA_Groups )) != WMHI_NOMORE ) {
			switch ( rc ) {
			   case ID_GROUP_W0:
			   case ID_GROUP_W1:
			   case ID_GROUP_W2:
			   case ID_GROUP_W3:
			      id = rc - ID_GROUP_W0;
			      GetAttr(STRINGA_LongVal, w[id], &rc);
			      SetAttrs(w[id], LGO_Weight, rc, TAG_DONE);
			      break;

			   case  WMHI_CLOSEWINDOW:
			      WindowClose( WA_Groups );
			      groups = NULL;
			      break;
			}
		     }
		  }

		  if ( sigwin == notif ) {
		     while (( rc = HandleEvent( WA_Notif )) != WMHI_NOMORE ) {
			switch ( rc ) {
			   case  WMHI_CLOSEWINDOW:
			      WindowClose( WA_Notif );
			      notif = NULL;
			      break;
			}
		     }
		  }

		  if ( sigwin == info ) {
		     while (( rc = HandleEvent( WA_Info )) != WMHI_NOMORE ) {
			switch ( rc ) {
			   case  WMHI_CLOSEWINDOW:
			      WindowClose( WA_Info );
			      info = NULL;
			      break;
			}
		     }
		  }

		  if ( sigwin == image ) {
		     while (( rc = HandleEvent( WA_Image )) != WMHI_NOMORE ) {
			switch ( rc ) {
			   case  WMHI_CLOSEWINDOW:
			      WindowClose( WA_Image );
			      image = NULL;
			      break;
			}
		     }
		  }

		  if ( sigwin == bfill ) {
		     while (( rc = HandleEvent( WA_BFill )) != WMHI_NOMORE ) {
			switch ( rc ) {
			   case  WMHI_CLOSEWINDOW:
			      WindowClose( WA_BFill );
			      bfill = NULL;
			      break;
			}
		     }
		  }

		  if ( sigwin == pages ) {
		     while (( rc = HandleEvent( WA_Pages )) != WMHI_NOMORE ) {
			switch ( rc ) {
			   case  WMHI_CLOSEWINDOW:
			      WindowClose( WA_Pages );
			      pages = NULL;
			      break;
			}
		     }
		  }
	       }
	    }
	 } while ( running );
      }
      /*
      ** Dispose of all window objects.
      **/
      if ( WA_Pages )         DisposeObject( WA_Pages );
      if ( WA_BFill )         DisposeObject( WA_BFill );
      if ( WA_Image )         DisposeObject( WA_Image );
      if ( WA_Info )    DisposeObject( WA_Info );
      if ( WA_Notif )         DisposeObject( WA_Notif );
      if ( WA_Groups )  DisposeObject( WA_Groups );
      if ( WA_Main )    DisposeObject( WA_Main );
      /*
      ** Delete the shared message port.
      **/
      DeleteMsgPort( SharedPort );
   } else
      Tell( "Unable to create a message port.\n" );
}
