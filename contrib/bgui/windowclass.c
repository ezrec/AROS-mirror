/*
 * @(#) $Header$
 *
 * BGUI library
 * windowclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.10  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.9  2003/01/18 19:10:03  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.8  2000/08/17 15:09:18  chodorowski
 * Fixed compiler warnings.
 *
 * Revision 42.7  2000/08/10 17:52:47  stegerg
 * temp fix for relayout refresh bug which only happens in AROS. temp. solved
 * by doing a RefreshGList in windowclass.c/WindowClassRelease method.
 *
 * Revision 42.6  2000/07/10 16:35:49  stegerg
 * bugfix: if a screen's size was smaller than it's OSCAN_TEXT overscan size
 * then windows sometimes could not be created because of badly
 * calculated coordinates.
 *
 * Revision 42.5  2000/07/07 17:11:50  stegerg
 * had to change BASE_FINDKEY and GRM_WHICHOBJECT method calls
 * for AROS (#ifdef), because of changed types in method structs
 * (UWORD --> STACKUWORD, ...)
 *
 * Revision 42.4  2000/06/01 01:41:38  bergers
 * Only 2 linker problems left: stch_l & stcu_d. Somebody might want to replace them (embraced by #ifdef __AROS__), please.
 *
 * Revision 42.3  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.2  2000/05/15 19:27:03  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:48  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:10:42  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:30  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.14  2000/01/30 18:56:13  mlemos
 * Fixed missing end tag when activating a gadget with the right or middle
 * mouse buttons.
 *
 * Revision 41.10.2.13  1999/08/30 04:59:21  mlemos
 * Added the WindowClassSetupGadget function to implement the
 * WINDOW_SETUPGADGET method.
 * Removed left out locale debugging statement.
 *
 * Revision 41.10.2.12  1999/07/28 21:04:17  mlemos
 * Removed the RefreshWindowFrame call in WM_SECURE method because the it may
 * cause deadlocks.
 *
 * Revision 41.10.2.11  1999/07/03 15:17:48  mlemos
 * Replaced the calls to CallHookPkt to BGUI_CallHookPkt.
 *
 * Revision 41.10.2.10  1998/12/07 03:37:22  mlemos
 * Fixed potential text font leak.
 *
 * Revision 41.10.2.9  1998/12/07 03:07:10  mlemos
 * Replaced OpenFont and CloseFont calls by the respective BGUI debug macros.
 *
 * Revision 41.10.2.8  1998/11/16 20:02:28  mlemos
 * Replaced FreeVec calls by BGUI_FreePoolMem calls.
 *
 * Revision 41.10.2.7  1998/09/20 22:26:31  mlemos
 * Made a debugging statement using tprintfe use D(bug()) macro instead.
 *
 * Revision 41.10.2.6  1998/06/21 22:07:51  mlemos
 * Ensured that the return code of OM_NEW is initialized just in case the
 * master group object is NULL.
 *
 * Revision 41.10.2.5  1998/05/22 02:56:41  mlemos
 * Ensured that any tooltips are closed with other relevant input events such
 * as mouse and keyboard input.
 *
 * Revision 41.10.2.4  1998/04/28 14:03:07  mlemos
 * Made WindowClass only consider IDCMP_INTUITICKS messages for tooltips.
 *
 * Revision 41.10.2.3  1998/04/28 02:29:34  mlemos
 * Made HandleIDCMP method ignore IDCMP_IDCMPUPDATE messages when handling
 * tooltips.
 *
 * Revision 41.10.2.2  1998/03/02 23:51:36  mlemos
 * Switched vector allocation functions calls to BGUI allocation functions.
 *
 * Revision 41.10.2.1  1998/03/01 15:40:21  mlemos
 * Added support to track BaseInfo memory leaks.
 *
 * Revision 41.10  1998/02/25 21:13:34  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:10:06  mlemos
 * Ian sources
 *
 *
 */

#define WW(x)

#include "include/classdefs.h"

/*
 * Tab-cycling.
 */
typedef struct TabCycle {
   struct TabCycle   *tc_Next;
   struct TabCycle   *tc_Prev;
   Object            *tc_Object;
}  TABCYCLE;

typedef struct {
   TABCYCLE          *cl_First;
   TABCYCLE          *cl_EndMark;
   TABCYCLE          *cl_Last;
}  CYCLELIST;

/*
 * Update notification.
 */
typedef struct UpdateN {
   struct UpdateN    *up_Next;
   struct UpdateN    *up_Prev;
   ULONG              up_ID;
   Object            *up_Target;
   struct TagItem    *up_MapList;
}  UPN;

typedef struct {
   UPN   *ul_First;
   UPN   *ul_EndMark;
   UPN   *ul_Last;
}  UPL;

/*
 * Object instance data.
 */
typedef struct {
   ULONG              wd_Flags;           /* see below                       */
   struct Window     *wd_WindowPtr;       /* points to opened window         */
   struct Screen     *wd_Screen;          /* points to screen to use         */
   struct DrawInfo   *wd_DrawInfo;        /* the DrawInfo for this screen.   */
   UWORD              wd_TextW, wd_TextH; /* Overscan dimensions.            */
   UBYTE             *wd_WindowTitle;     /* title of the window             */
   ULONG              wd_WindowTitleID;
   UBYTE             *wd_ScreenTitle;     /* active window screen title    */
   ULONG              wd_ScreenTitleID;
   UWORD              wd_Position;        /* positioning                   */
   UWORD              wd_WidthScale;      /* width percentage from remaining width     */
   UWORD              wd_HeightScale;     /* height percentage from remaining height   */
   struct MsgPort    *wd_UserPort;        /* shared/normal window user port        */
   struct Menu       *wd_Menus;           /* created menu strip            */
   APTR               wd_VisualInfo;      /* visual info for menu strip    */
   struct NewMenu    *wd_MenuStrip;    /* points to NewMenu array       */
   UWORD              wd_SizeW, wd_SizeH; /* size gadget dimensions        */
   UWORD              wd_CloseW;    /* close gadget width              */
   UWORD              wd_DepthW;    /* depth gadget width              */
   UWORD              wd_ZoomW;     /* zoom gadget width            */
   Object            *wd_Gadgets;      /* window master group             */
   UWORD              wd_MasterW, wd_MasterH;   /* master group sizes.             */
   Object            *wd_LBorder;      /* left-border group.              */
   UWORD              wd_LWidth, wd_LHeight;
   Object            *wd_TBorder;      /* top-border group.            */
   UWORD              wd_TWidth, wd_THeight;
   Object            *wd_RBorder;      /* right-border group.             */
   UWORD              wd_RWidth, wd_RHeight;
   Object            *wd_BBorder;      /* bottom-border group.                      */
   UWORD              wd_BWidth, wd_BHeight;
   UWORD              wd_Left;      /* window border sizes.                      */
   UWORD              wd_Right;
   UWORD              wd_Top;
   UWORD              wd_Bottom;
   WORD               wd_Zoom[4];         /* WINDOW_TitleZip size.           */
   UWORD              wd_MinW, wd_MinH;   /* Minimum sizes. */
   UWORD              wd_MaxW, wd_MaxH;   /* Maximum sizes. */
   struct Screen     *wd_PubScreen;       /* points to pub screen to use.    */
   APTR               wd_Lock;            /* IDCMP lock                */
   struct IBox        wd_FixedPos;        /* pos. and size after change      */
   struct IBox        wd_RelPos;          /* box for relative positioning    */
   UBYTE             *wd_PubScreenName;   /* points to pubscreen name        */
   ULONG              wd_WindowFlags;     /* WFLG_xxxx                 */
   ULONG              wd_IDCMPFlags;      /* IDCMP_xxxx                */
   ULONG              wd_SleepCnt;        /* sleep nest counter            */
   struct Hook       *wd_HelpHook;        /* help hook                     */
   UBYTE             *wd_HelpFile;        /* amigaguide file               */
   UBYTE             *wd_HelpNode;        /* amigaguide node               */
   ULONG              wd_HelpLine;        /* amigaguide line               */
   UBYTE             *wd_HelpText;        /* help text                     */
   ULONG              wd_HelpTextID;
   struct Hook       *wd_IDCMPHook;       /* IDCMP hook                    */
   struct Hook       *wd_VerifyHook;      /* IDCMP_VERIFY hook            */
   ULONG              wd_IDCMPHookBits;   /* bits to trigger IDCMP hook         */
   ULONG              wd_VerifyHookBits;  /* bits to trigger IDCMP_VERIFY hook        */
   struct TextAttr   *wd_Font;            /* font to use               */
   struct TextAttr   *wd_FallBackFont;    /* font to fall back to if the above fails   */
   struct TextAttr   *wd_MenuFont;        /* font to use for the menus.         */
   struct TextFont   *wd_UsedFont;        /* Currently used default font.       */
   struct TextFont   *wd_OrigFont;        /* Original window font.              */
   CYCLELIST          wd_CycleList;       /* tab-cycling list                   */
   UWORD              wd_NextSelect;      /* menu drag-selections                      */
   UPL                wd_UpdateList;      /* list of update notification objects      */
   ULONG              wd_ID;              /* unique window ID                   */
   struct MsgPort    *wd_AppPort;         /* appwindow message port             */
   struct AppWindow  *wd_AppWindow;       /* appwindow                          */
   UWORD              wd_OW, wd_OH;       /* to check for a size change.        */
   struct Hook       *wd_ToolTipHook;     /* Custom tooltip hook.               */
   ULONG              wd_ToolTicks;       /* Ticks passed without mouse move.   */
   WORD               wd_ToolX, wd_ToolY; /* mouse position for tool tip.       */
   ULONG              wd_ToolTickTime;    /* ticks to delay before showing tool tip.   */
   APTR               wd_ToolTip;         /* Current tool-tip.                  */
   Object            *wd_ToolTipObject;   /* Current tool-tip object.           */
   struct Region     *wd_Region;          /* Clip Region.                       */
   struct RastPort   *wd_BufferRP;        /* Buffer rastport.                   */
   struct Layer      *wd_BufferLayer;     /* Buffer layer.                      */
   UWORD              wd_BRW, wd_BRH;     /* Buffer rasport's size.             */
   struct bguiLocale *wd_Locale;          /* Localize automatically.            */
   UBYTE             *wd_Catalog;         /* Catalog name.                      */
   Object            *wd_DGMObject;       /* For sending GoActive messages.     */
}  WD;

#define WDF_SHARED_MSGPORT (1<<0)         /* window user port is shared         */
#define WDF_SCREEN_LOCKED  (1<<1)         /* we have locked a screen            */
#define WDF_LOCK_WIDTH     (1<<2)         /* window width is fixed              */
#define WDF_LOCK_HEIGHT    (1<<3)         /* window height is fixed             */
#define WDF_CHANGE_VALID   (1<<4)         /* window FixedPos is valid           */
#define WDF_RELBOX         (1<<5)         /* window is placed relative to IBox  */
#define WDF_DRAGSELECT     (1<<6)         /* we are in a menu drag-select       */
#define WDF_IS_APPWINDOW   (1<<7)         /* we are an AppWindow                */
#define WDF_NO_BUFFER      (1<<8)         /* don't use a buffer rastport        */
#define WDF_AUTOASPECT     (1<<9)         /* auto set frame thickness.          */
#define WDF_CLOSEONESC     (1<<10)        /* close window when ESC is pressed.  */
#define WDF_SHOWTITLE      (1<<11)        /* show screen title.                 */
#define WDF_REMOVED        (1<<12)        /* master gadget manually removed.    */
#define WDF_USERTICKS      (1<<13)        /* user set IDCMP_INTUITICKS.         */
#define WDF_LOCKOUT        (1<<14)        /* ignore all IDCMP.                  */
#define WDF_TITLEZIP       (1<<15)        /* Preferences zip style.             */
#define WDF_AUTOKEYLABEL   (1<<16)        /* Call BASE_KEYLABEL automatically?  */
#define WDF_FREELOCALE     (1<<17)        /* Call BASE_KEYLABEL automatically?  */
#define WDF_CONSTRAINTS    (1<<18)        /* We have new constraints pending.   */
#define WDF_PREBUFFER      (1<<19)        /* Should we render before we open?   */
/*
 * For PackBoolTags().
 */
STATIC struct TagItem boolTags[] = {
   WINDOW_LockWidth,    WDF_LOCK_WIDTH,
   WINDOW_LockHeight,   WDF_LOCK_HEIGHT,
   WINDOW_NoBufferRP,   WDF_NO_BUFFER,
   WINDOW_PreBufferRP,  WDF_PREBUFFER,
   WINDOW_AutoAspect,   WDF_AUTOASPECT,
   WINDOW_CloseOnEsc,   WDF_CLOSEONESC,
   WINDOW_ShowTitle,    WDF_SHOWTITLE,
   WINDOW_AppWindow,    WDF_IS_APPWINDOW,
   WINDOW_TitleZip,     WDF_TITLEZIP,
   WINDOW_AutoKeyLabel, WDF_AUTOKEYLABEL,
   TAG_END
};

STATIC struct TagItem boolTags1[] = {
   WINDOW_DragBar,      WFLG_DRAGBAR,
   WINDOW_SizeGadget,   WFLG_SIZEGADGET,
   WINDOW_CloseGadget,  WFLG_CLOSEGADGET,
   WINDOW_DepthGadget,  WFLG_DEPTHGADGET,
   WINDOW_SizeBottom,   WFLG_SIZEBBOTTOM,
   WINDOW_SizeRight,    WFLG_SIZEBRIGHT,
   WINDOW_Activate,     WFLG_ACTIVATE,
   WINDOW_RMBTrap,      WFLG_RMBTRAP,
   WINDOW_ReportMouse,  WFLG_REPORTMOUSE,
   WINDOW_Borderless,   WFLG_BORDERLESS,
   WINDOW_Backdrop,     WFLG_BACKDROP,
   TAG_END
};

static Class *WindowClass = NULL;

/*
 * Make a private copy of the NewMenu array.
 */
STATIC BOOL CopyNewMenuArray( WD *wd, struct NewMenu *array )
{
   struct NewMenu    *ptr = array;
   ULONG        num = 0;

   /*
    * Menu array OK?
    */
   if ( ! array )
      return( TRUE );

   /*
    * Count the number of NewMenu
    * structure in the array.
    */
   while (ptr->nm_Type != NM_END)
   {
      num++;
      ptr++;
   }

   /*
    * Include the NM_END.
    */
   num++;

   /*
    * Allocate a private copy.
    */
   if ( wd->wd_MenuStrip = ( struct NewMenu * )BGUI_AllocPoolMem( num * sizeof( struct NewMenu ))) {
      /*
       * Copy the array.
       */
      CopyMem(( void * )array, ( void * )wd->wd_MenuStrip, num * sizeof( struct NewMenu ));
      return( TRUE );
   }
   return( FALSE );
}

/*
 * Find out the minumum dimensions.
 */
STATIC ULONG GroupDimensions(Object *master, struct TextAttr *font, UWORD *minw, UWORD *minh)
{
   struct TextFont   *tf = NULL;
   struct RastPort    rp;
   ULONG              rc;
   struct GadgetInfo  gi;
   WD                *wd;
   BC                *bc = BASE_DATA(master);

   /*
    * Initialize the RastPort.
    */
   InitRastPort(&rp);

   /*
    * Setup the font.
    */
   if (font)
   {
      if (tf = BGUI_OpenFont(font)) FSetFont(&rp, tf);
      else return 0;

      DoSetMethodNG(master, BT_TextAttr, font, TAG_END);
   }

   if (bc->bc_Window)
   {
      wd = INST_DATA(WindowClass, bc->bc_Window);

      /*
       * Setup fake GadgetInfo (used for the DrawInfo).
       */
      bzero(&gi, sizeof(gi));
      gi.gi_RastPort       = &rp;
      gi.gi_Screen         = wd->wd_Screen;
      gi.gi_DrInfo         = wd->wd_DrawInfo;
      gi.gi_Pens.DetailPen = wd->wd_DrawInfo->dri_Pens[DETAILPEN];
      gi.gi_Pens.BlockPen  = wd->wd_DrawInfo->dri_Pens[BLOCKPEN];

      /*
       * Send the message.
       */
      rc = AsmDoMethod(master, GRM_DIMENSIONS, &gi, &rp, minw, minh, 0);
   };

   /*
    * Close font when opened.
    */
   if (tf) BGUI_CloseFont(tf);

   return rc;
}


/*
 * Back-filling hook. This hook makes sure that the window background is
 * always BACKGROUNDPEN. The hook will only fill the areas that have grown
 * because of a re-size and the right and bottom border of the window in it's
 * previous position if the size has grown.
 */
//STATIC SAVEDS ASM VOID BackFill_func(REG(a0) struct Hook *hook, REG(a2) struct RastPort *rp, REG(a1) BFINFO *info)
STATIC SAVEDS ASM REGFUNC3(VOID, BackFill_func,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, struct RastPort *, rp),
	REGPARAM(A1, BFINFO *, info))
{
   struct Layer    *save_layer;
   struct Window   *w = NULL;
   struct BaseInfo *bi;

   /*
    * Do not disturb the previous window contents. When the area to fill is located in the previous GUI area
    * we do not back fill.  This makes sure the previous GUI remains visible until it is overwritten by the
    * resized GUI.
    */
   if (info->bf_Layer) w = info->bf_Layer->Window;

   if (!w || (info->bf_X > w->BorderLeft) || (info->bf_Y > w->BorderTop))
   {
      /*
       * NO LAYER!
       */
      save_layer = rp->Layer;
      rp->Layer  = NULL;

#ifdef DEBUG_BGUI
      if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_RastPort, rp, TAG_DONE))
#else
      if (bi = AllocBaseInfo(BI_RastPort, rp, TAG_DONE))
#endif
      {
	 /*
	  * Setup the rastport.
	  */
	 BSetDrMd(bi, JAM1);
	 BSetDPenA(bi, BACKGROUNDPEN);
	 
	 /*
	  * Fill the area.
	  */
	 BRectFillA(bi, &info->bf_Rect);

	 FreeBaseInfo(bi);
      };
      rp->Layer = save_layer;
   }
}
REGFUNC_END

STATIC struct Hook BackFill_hook = { NULL, NULL, (FUNCPTR)BackFill_func, NULL, NULL };

/*
 * Set attributes.
 */
METHOD(WindowClassSetUpdate, struct opSet *, ops)
{
   WD              *wd = INST_DATA(cl, obj);
   struct TagItem  *tstate = ops->ops_AttrList, *tag;
   struct Window   *w = wd->wd_WindowPtr;
   ULONG            data;

   /*
    * Set attributes.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;       
   
      switch (tag->ti_Tag)
      {
      case WINDOW_Screen:
	 if (!w) wd->wd_Screen = (struct Screen *)data;
	 break;

      case WINDOW_PubScreen:
	 if (!w) wd->wd_PubScreen = (struct Screen *)data;
	 break;

      case WINDOW_PubScreenName:
	 if (!w) wd->wd_PubScreenName = (UBYTE *)data;
	 break;
	 
      case WINDOW_Locale:
	 if (wd->wd_Locale && (wd->wd_Flags & WDF_FREELOCALE))
	 {
	    CloseLocale(wd->wd_Locale->bl_Locale);
	    CloseCatalog(wd->wd_Locale->bl_Catalog);
	    BGUI_FreePoolMem(wd->wd_Locale);
	 };
	 wd->wd_Catalog = NULL;
	 wd->wd_Locale  = (struct bguiLocale *)data;
	 wd->wd_Flags  &= ~WDF_FREELOCALE;
	 break;
      
      case WINDOW_Catalog:
	 if (LocaleBase == NULL) break;
	 
	 if (wd->wd_Locale && wd->wd_Catalog)
	 {
	    CloseLocale(wd->wd_Locale->bl_Locale);
	    CloseCatalog(wd->wd_Locale->bl_Catalog);
	    if (data == NULL)
	    {
	       BGUI_FreePoolMem(wd->wd_Locale);
	       wd->wd_Locale = NULL;
	       wd->wd_Catalog = NULL;
	       break;
	    };
	 }
	 else
	 {
	    if((wd->wd_Locale = BGUI_AllocPoolMem(sizeof(struct bguiLocale))))
	       memset(wd->wd_Locale,0,sizeof(struct bguiLocale));
	    wd->wd_Flags |= WDF_FREELOCALE;
	 };
	 if (wd->wd_Locale)
	 {
	    wd->wd_Catalog = (UBYTE *)data;
	    wd->wd_Locale->bl_Locale  = OpenLocale(NULL);
	    wd->wd_Locale->bl_Catalog = OpenCatalogA(NULL, wd->wd_Catalog, NULL);
	 };
	 break;

      case WINDOW_Title:
	 wd->wd_WindowTitle = (UBYTE *)data;
	 if (w && !(wd->wd_Flags & WFLG_BORDERLESS))
	 {
	    SetWindowTitles(w, wd->wd_WindowTitle, (UBYTE *)~0);
	    /*
	     * Notify the target when necessary.
	     */
	    DoNotifyMethod(obj, NULL, 0, tag->ti_Tag, data, TAG_END);
	 };
	 break;

      case WINDOW_ScreenTitle:
	 wd->wd_ScreenTitle = (UBYTE *)data;
	 if (w)
	 {
	    SetWindowTitles(w, (UBYTE *)~0, wd->wd_ScreenTitle);
	    /*
	     * Notify the target when necessary.
	     */
	    DoNotifyMethod(obj, NULL, 0, tag->ti_Tag, data, TAG_END);
	 };
	 break;

      case WINDOW_ToolTicks:
	 wd->wd_ToolTickTime = data;
	 wd->wd_ToolTicks    = 0;

	 if (wd->wd_ToolTickTime)  wd->wd_IDCMPFlags |= IDCMP_INTUITICKS;
	 else
	 {
	    if (!(wd->wd_Flags & WDF_USERTICKS))
	       wd->wd_IDCMPFlags &= ~IDCMP_INTUITICKS;
	 };
	 if (w) ModifyIDCMP(w, wd->wd_IDCMPFlags);
	 break;

      case WINDOW_Bounds:
	 if (data)
	 {
	    if (w)
	    {
	       ChangeWindowBox(w, IBOX(data)->Left,  IBOX(data)->Top,
				  IBOX(data)->Width, IBOX(data)->Height);
	    }
	    else
	    {
	       wd->wd_FixedPos = *IBOX(data);
	       wd->wd_Flags   |= WDF_CHANGE_VALID;
	    };
	 }
	 else
	    wd->wd_Flags &= ~WDF_CHANGE_VALID;
	 break;

      case WINDOW_CloseOnEsc:
	 if (data) wd->wd_Flags |= WDF_CLOSEONESC;
	 else      wd->wd_Flags &= ~WDF_CLOSEONESC;
	 break;

      case WINDOW_NoVerify:
	 if (w)
	 {
	    if (data) ModifyIDCMP(w, wd->wd_IDCMPFlags & ~IDCMP_SIZEVERIFY);
	    else      ModifyIDCMP(w, wd->wd_IDCMPFlags);
	 }
	 break;

      case  WINDOW_MenuFont:
	 wd->wd_MenuFont = (struct TextAttr *)data;
	 if (w && wd->wd_Menus)
	 {
	    ClearMenuStrip(w);
	    if ((!wd->wd_MenuFont) || (!LayoutMenus(wd->wd_Menus, wd->wd_VisualInfo,
	       GTMN_NewLookMenus, TRUE, GTMN_TextAttr, wd->wd_MenuFont, TAG_DONE)))
	    {
	       LayoutMenus(wd->wd_Menus, wd->wd_VisualInfo, GTMN_NewLookMenus, TRUE, TAG_END);
	    };
	    SetMenuStrip(w, wd->wd_Menus);
	 }
	 break;

      case WINDOW_SharedPort:
	 if (!w)
	 {
	    if (wd->wd_UserPort = (struct MsgPort *)data)
	       wd->wd_Flags |= WDF_SHARED_MSGPORT;
	    else
	       wd->wd_Flags &= ~WDF_SHARED_MSGPORT;
	 }
	 break;



      case WINDOW_TitleID:
	 wd->wd_WindowTitleID = data;
	 break;

      case WINDOW_ScreenTitleID:
	 wd->wd_ScreenTitleID = data;
	 break;

      case WINDOW_HelpFile:
	 wd->wd_HelpFile = (UBYTE *)data;
	 break;

      case WINDOW_HelpNode:
	 wd->wd_HelpNode = (UBYTE *)data;
	 break;

      case WINDOW_HelpLine:
	 wd->wd_HelpLine = data;
	 break;

      case WINDOW_HelpText:
	 wd->wd_HelpText = (UBYTE *)data;
	 break;

      case WINDOW_HelpTextID:
	 wd->wd_HelpTextID = data;
	 break;

      case WINDOW_IDCMPHook:
	 wd->wd_IDCMPHook = (struct Hook *)data;
	 break;

      case WINDOW_VerifyHook:
	 wd->wd_VerifyHook = (struct Hook *)data;
	 break;

      case WINDOW_IDCMPHookBits:
	 wd->wd_IDCMPHookBits = data;
	 break;

      case WINDOW_VerifyHookBits:
	 wd->wd_VerifyHookBits = data;
	 break;

      case WINDOW_Position:
	 wd->wd_Position = data;
	 break;

      case WINDOW_ScaleWidth:
	 wd->wd_WidthScale = data;
	 break;

      case WINDOW_ScaleHeight:
	 wd->wd_HeightScale = data;
	 break;

      case WINDOW_Font:
	 wd->wd_Font = (struct TextAttr *)data;
	 break;

      case WINDOW_FallBackFont:
	 wd->wd_FallBackFont = (struct TextAttr *)data;
	 break;

      case WINDOW_UniqueID:
	 wd->wd_ID = data;
	 break;
      };
   };
   return 1;
}
METHOD_END

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(WindowClassNew, struct opSet *, ops)
{
   WD             *wd;
   struct TagItem *tstate, *tag, *tags;
   ULONG           rc=0, data, idcmp;
   BOOL            fail = FALSE;

   Object *master, *lborder, *tborder, *rborder, *bborder;

   tags = DefTagList(BGUI_WINDOW_OBJECT, ops->ops_AttrList);

   master  = (Object *)GetTagData(WINDOW_MasterGroup,  NULL, tags);
   lborder = (Object *)GetTagData(WINDOW_LBorderGroup, NULL, tags);
   tborder = (Object *)GetTagData(WINDOW_TBorderGroup, NULL, tags);
   rborder = (Object *)GetTagData(WINDOW_RBorderGroup, NULL, tags);
   bborder = (Object *)GetTagData(WINDOW_BBorderGroup, NULL, tags);

   /*
    * For now we do not work without a master group.
    */

   /*
    * First we let the superclass setup an object.
    */
   if (master && (rc = NewSuperObject(cl, obj, tags)))
   {
      /*
       * Get the instance data.
       */
      wd = INST_DATA(cl, rc);

      wd->wd_Gadgets = master;
      wd->wd_LBorder = lborder;
      wd->wd_TBorder = tborder;
      wd->wd_RBorder = rborder;
      wd->wd_BBorder = bborder;

      wd->wd_DGMObject = BGUI_NewObjectA(BGUI_DGM_OBJECT, NULL);
      wd->wd_Region    = NewRegion();

      /*
       * Initialize cycle/update lists.
       */
      NewList((struct List *)&wd->wd_CycleList);
      NewList((struct List *)&wd->wd_UpdateList);

      /*
       * Setup defaults.
       */
      wd->wd_Position     = POS_CENTERSCREEN;
      wd->wd_FallBackFont = &Topaz80;
      wd->wd_ToolX        = wd->wd_ToolY   = -1;
      wd->wd_Zoom[0]      = wd->wd_Zoom[1] = -1;

      /*
       * Pack user boolean tags with predefined defaults.
       */
      wd->wd_WindowFlags = PackBoolTags(WFLG_DRAGBAR | WFLG_SIZEGADGET | WFLG_CLOSEGADGET | WFLG_DEPTHGADGET |
					WFLG_ACTIVATE | WFLG_NOCAREREFRESH | WFLG_SIZEBBOTTOM | WFLG_REPORTMOUSE |
					WFLG_NEWLOOKMENUS, tags, boolTags1);

      /*
       * Simple refresh?
       */
      if (!GetTagData(WINDOW_SmartRefresh, FALSE, tags)) wd->wd_WindowFlags |= WFLG_SIMPLE_REFRESH;

      idcmp = IDCMP_RAWKEY|IDCMP_GADGETUP|IDCMP_CHANGEWINDOW|IDCMP_INACTIVEWINDOW|IDCMP_ACTIVEWINDOW|
	      IDCMP_IDCMPUPDATE|IDCMP_SIZEVERIFY|IDCMP_NEWSIZE|IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS|IDCMP_MENUHELP;

      /*
       * Obtain all necessary window data.
       */
      tstate = tags;
      while (tag = NextTagItem(&tstate))
      {
	 data = tag->ti_Data;
	 
	 switch (tag->ti_Tag)
	 {
	 case WINDOW_MenuStrip:
	    if (!wd->wd_MenuStrip)
	    {
	       if (!CopyNewMenuArray(wd, (struct NewMenu *)data)) fail = TRUE;
	    }
	    break;

	 case WINDOW_IDCMP:
	    wd->wd_IDCMPFlags = data;
	    if (data & IDCMP_INTUITICKS) wd->wd_Flags |= WDF_USERTICKS;
	    break;

	 case WINDOW_PosRelBox:
	    wd->wd_RelPos = *IBOX(data);
	    wd->wd_Flags |= WDF_RELBOX;
	    break;
	 }
      }
      AsmCoerceMethod(cl, (Object *)rc, OM_SET, tags, NULL);

      if (!fail)
      {
	 /*
	  * Disable all system gadgets and bordergroups when this
	  * is a backdrop or borderless window.
	  */
	 if (wd->wd_WindowFlags & (WFLG_BORDERLESS|WFLG_BACKDROP))
	 {
	    wd->wd_WindowFlags &= ~(WFLG_SIZEGADGET|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_DRAGBAR);
	    wd->wd_WindowFlags |= WFLG_BORDERLESS;
	    wd->wd_WindowTitle  = NULL;
	 };

	 /*
	  * Pickup the other boolean tags.
	  */
	 wd->wd_Flags = PackBoolTags(wd->wd_Flags, tags, boolTags);

	 /*
	  * Pack some IDCMP flags.
	  */
	 if (wd->wd_WindowFlags & WFLG_CLOSEGADGET) idcmp |= IDCMP_CLOSEWINDOW;
	 if (wd->wd_MenuStrip)                      idcmp |= IDCMP_MENUPICK;

	 wd->wd_IDCMPFlags |= idcmp;

	 if (wd->wd_Locale) AsmDoMethod((Object *)rc, BASE_LOCALIZE, wd->wd_Locale);
	 if (wd->wd_Flags & WDF_AUTOKEYLABEL) AsmDoMethod((Object *)rc, BASE_KEYLABEL);

	 DoSetMethodNG(wd->wd_Gadgets, GROUP_IsMaster, TRUE, BT_Buffer, TRUE, TAG_DONE);

	 DoMultiSet(BT_ParentWindow, rc, 5, master, lborder, tborder, rborder, bborder);

	 if (lborder) DoSetMethodNG(lborder, GA_LeftBorder,   TRUE, TAG_DONE);
	 if (rborder) DoSetMethodNG(rborder, GA_RightBorder,  TRUE, TAG_DONE);
	 if (tborder) DoSetMethodNG(tborder, GA_TopBorder,    TRUE, TAG_DONE);
	 if (bborder) DoSetMethodNG(bborder, GA_BottomBorder, TRUE, TAG_DONE);
      }
      else
      {
	 AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
	 rc = 0;
      };
   };
   if (!rc)
   {
      /*
       * If the window object cannot be created we
       * dispose of the master group and border groups.
       */
      if (master)  AsmDoMethod(master,  OM_DISPOSE);
      if (lborder) AsmDoMethod(lborder, OM_DISPOSE);
      if (tborder) AsmDoMethod(tborder, OM_DISPOSE);
      if (rborder) AsmDoMethod(rborder, OM_DISPOSE);
      if (bborder) AsmDoMethod(bborder, OM_DISPOSE);
   };
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Get rid of an object.
 */
METHOD(WindowClassDispose, Msg, msg)
{
   WD          *wd = INST_DATA(cl, obj);
   TABCYCLE    *tc;
   UPN         *up;

   /*
    * When the window is open we close it.
    */
   if (wd->wd_WindowPtr)
      AsmDoMethod(obj, WM_CLOSE);

   if (wd->wd_Locale && wd->wd_Catalog)
   {
      CloseLocale(wd->wd_Locale->bl_Locale);
      CloseCatalog(wd->wd_Locale->bl_Catalog);

      if (wd->wd_Flags & WDF_FREELOCALE)
	 BGUI_FreePoolMem(wd->wd_Locale);
   };
   
   if (wd->wd_DGMObject)
   {
      DisposeObject(wd->wd_DGMObject);
   };

   if (wd->wd_Region)
   {
      DisposeRegion(wd->wd_Region);
   };

   /*
    * Remove and delete the tab-cycle list.
    */
   while (tc = (TABCYCLE *)RemHead((struct List *)&wd->wd_CycleList))
      BGUI_FreePoolMem(tc);

   /*
    * Remove and delete all update targets.
    */
   while (up = (UPN *)RemHead((struct List *)&wd->wd_UpdateList))
      BGUI_FreePoolMem(up);

   /*
    * Free the NewMenu array.
    */
   if (wd->wd_MenuStrip)
      BGUI_FreePoolMem(wd->wd_MenuStrip);

   /*
    * Dispose of the master group and
    * border groups.
    */
   if (wd->wd_Gadgets) AsmDoMethod(wd->wd_Gadgets, OM_DISPOSE);
   if (wd->wd_LBorder) AsmDoMethod(wd->wd_LBorder, OM_DISPOSE);
   if (wd->wd_RBorder) AsmDoMethod(wd->wd_RBorder, OM_DISPOSE);
   if (wd->wd_BBorder) AsmDoMethod(wd->wd_BBorder, OM_DISPOSE);
   if (wd->wd_TBorder) AsmDoMethod(wd->wd_TBorder, OM_DISPOSE);

   if(wd->wd_UsedFont)
   {
      BGUI_CloseFont(wd->wd_UsedFont);
      wd->wd_UsedFont=NULL;
   }
   /*
    * The superclass takes care of the rest.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/*
 * Setup the menu strip.
 */
//STATIC ASM BOOL DoMenuStrip( REG(a0) WD *wd, REG(a1) struct Screen *scr )
STATIC ASM REGFUNC2(BOOL, DoMenuStrip,
	REGPARAM(A0, WD *, wd),
	REGPARAM(A1, struct Screen *, scr))
{
   /*
    * Get the visual info. (We use
    * gadtools for the menus.)
    */
   if ( wd->wd_VisualInfo = GetVisualInfo( scr, TAG_END )) {
      /*
       * Create the menu-strip.
       */
      if ( wd->wd_Menus = CreateMenus( wd->wd_MenuStrip, TAG_END )) {
	 /*
	  * Layout the menu strip.
	  */
	 if ( LayoutMenus( wd->wd_Menus, wd->wd_VisualInfo, GTMN_NewLookMenus, TRUE, wd->wd_MenuFont ? GTMN_TextAttr : TAG_IGNORE, wd->wd_MenuFont, TAG_END ))
	    return( TRUE );
	 /*
	  * Everything below this point means failure.
	  */
	 FreeMenus( wd->wd_Menus );
	 wd->wd_Menus = NULL;
      }
      FreeVisualInfo( wd->wd_VisualInfo );
      wd->wd_VisualInfo = NULL;
   }
   return( FALSE );
}
REGFUNC_END

/*
 * Kill the AppWindow stuff.
 */
//STATIC ASM VOID KillAppWindow(REG(a0) WD *wd)
STATIC ASM REGFUNC1(VOID, KillAppWindow,
	REGPARAM(A0, WD *, wd))
{
   struct Message       *msg;

   /*
    * Remove the AppWindow.
    */
   if (wd->wd_AppWindow)
   {
#ifdef __AROS__
#warning Commented RemoveAppWindow
#else
      RemoveAppWindow(wd->wd_AppWindow);
#endif
      wd->wd_AppWindow = NULL;
   };

   /*
    * Do we have a message port?
    */
   if (wd->wd_AppPort)
   {
      /*
       * Kill all remaining messages and the port.
       */
      while (msg = GetMsg(wd->wd_AppPort))
	 ReplyMsg(msg);

      DeleteMsgPort(wd->wd_AppPort);
      wd->wd_AppPort = NULL;
   };
}
REGFUNC_END

/*
 * Make it an AppWindow.
 */
//STATIC ASM BOOL MakeAppWindow(REG(a0) WD *wd)
STATIC ASM REGFUNC1(BOOL, MakeAppWindow,
	REGPARAM(A0, WD *, wd))
{
   /*
    * Create a message port.
    */
   if (wd->wd_AppPort = CreateMsgPort())
   {
      /*
       * Create the app window.
       */
#ifdef __AROS__
#warning Commented AddAppWindowA
#else
      if (wd->wd_AppWindow = AddAppWindowA(0, 0, wd->wd_WindowPtr, wd->wd_AppPort, NULL))
	 return TRUE;
#endif

      KillAppWindow(wd);
   };
   return FALSE;
}
REGFUNC_END

/*
 * Compute width and height of the
 * system gadgets.
 */
//STATIC ASM VOID SystemSize(REG(a1) WD *wd)
STATIC ASM REGFUNC1(VOID, SystemSize,
	REGPARAM(A1, WD *,wd))
{
   struct TagItem   tags[4];
   Object          *image;
   BOOL             hires = wd->wd_Screen->Flags & SCREENHIRES;

   /*
    * Set DrawInfo and resolution.
    */
   tags[0].ti_Tag  = SYSIA_Which;
   tags[1].ti_Tag  = SYSIA_DrawInfo;
   tags[1].ti_Data = (ULONG)wd->wd_DrawInfo;
   tags[2].ti_Tag  = SYSIA_Size;
   tags[2].ti_Data = hires ? SYSISIZE_MEDRES : SYSISIZE_LOWRES;
   tags[3].ti_Tag  = TAG_DONE;

   /*
    * Sizing image.
    */
   tags[0].ti_Data = SIZEIMAGE;
   if (image = NewObjectA(NULL, SYSICLASS, tags))
   {
      wd->wd_SizeW  = IM_BOX(image)->Width;
      wd->wd_SizeH  = IM_BOX(image)->Height;
      DisposeObject(image);
   }
   else
   {
      wd->wd_SizeW  = hires ? 18 : 13;
      wd->wd_SizeH  = hires ? 10 : 11;
   }

   /*
    * Close image.
    */
   tags[0].ti_Data = CLOSEIMAGE;
   if (image = NewObjectA(NULL, SYSICLASS, tags))
   {
      wd->wd_CloseW = IM_BOX(image)->Width;
      DisposeObject(image);
   }
   else
      wd->wd_CloseW = hires ? 20 : 15;

   /*
    * Depth image.
    */
   tags[0].ti_Data = DEPTHIMAGE;
   if (image = NewObjectA(NULL, SYSICLASS, tags))
   {
      wd->wd_DepthW = IM_BOX(image)->Width;
      DisposeObject(image);
   }
   else
      wd->wd_DepthW = hires ? 24 : 18;

   /*
    * Zoom image.
    */
   tags[0].ti_Data = ZOOMIMAGE;
   if (image = NewObjectA(NULL, SYSICLASS, tags))
   {
      wd->wd_ZoomW  = IM_BOX(image)->Width;
      DisposeObject(image);
   }
   else
      wd->wd_ZoomW  = hires ? 24 : 18;
}
REGFUNC_END

/*
 * Query border sizes.
 */
//STATIC ASM void WBorderSize(REG(a0) WD *wd, REG(a1) struct Screen *scr, REG(a2) struct TextAttr *at)
STATIC ASM REGFUNC3(void, WBorderSize,
	REGPARAM(A0, WD *, wd),
	REGPARAM(A1, struct Screen *, scr),
	REGPARAM(A2, struct TextAttr *, at))
{
   UWORD    wl, wt, wr, wb;

   /*
    * Borders available?
    */
   if (!(wd->wd_WindowFlags & WFLG_BORDERLESS))
   {
      /*
       * Default border sizes.
       */
      wt = scr->WBorTop + scr->Font->ta_YSize + 1;
      wl = scr->WBorLeft;
      wr = scr->WBorRight;
      wb = scr->WBorBottom;

      /*
       * Get system gadget dimensions.
       */
      SystemSize(wd);

      /*
       * Adjust for sizing gadget.
       */
      if (wd->wd_WindowFlags & WFLG_SIZEGADGET)
      {
	 /*
	  * Bottom border size gadget?
	  */
	 if (wd->wd_WindowFlags & WFLG_SIZEBBOTTOM) wb = wd->wd_SizeH;
	 else                                       wr = wd->wd_SizeW;

	 /*
	  * Right border size gadget?
	  */
	 if (wd->wd_WindowFlags & WFLG_SIZEBRIGHT)  wr = wd->wd_SizeW;
      }

      /*
       * Adjust for border groups.
       */
      if (wd->wd_LBorder)
      {
	 GroupDimensions(wd->wd_LBorder, at, &wd->wd_LWidth, &wd->wd_LHeight);
	 if (wd->wd_LWidth > wl) wl = wd->wd_LWidth;
      }

      if (wd->wd_TBorder)
      {
	 GroupDimensions(wd->wd_TBorder, at, &wd->wd_TWidth, &wd->wd_THeight);
	 if (wd->wd_THeight > wt) wt = wd->wd_THeight;
      }

      if (wd->wd_RBorder)
      {
	 GroupDimensions(wd->wd_RBorder, at, &wd->wd_RWidth, &wd->wd_RHeight);
	 if (wd->wd_RWidth > wr) wr = wd->wd_RWidth;
      }

      if (wd->wd_BBorder)
      {
	 GroupDimensions(wd->wd_BBorder, at, &wd->wd_BWidth, &wd->wd_BHeight);
	 if (wd->wd_BHeight > wb) wb = wd->wd_BHeight;
      }

      /*
       * Set them.
       */
      wd->wd_Left   = wl;
      wd->wd_Top    = wt;
      wd->wd_Right  = wr;
      wd->wd_Bottom = wb;
   } else
      /*
       * No borders.
       */
      wd->wd_Left = wd->wd_Top = wd->wd_Right = wd->wd_Bottom = 0;
}
REGFUNC_END

BOOL WinSize(WD *wd, UWORD *win_w, UWORD *win_h)
{
   struct TextAttr *font;
   BOOL thin;

   struct Screen *scr = wd->wd_Screen;

   UWORD gmw, gmh;
   int   bw, bh, gx, gy, t1, t2;

   int sw = scr->Width;
   int sh = scr->Height;

   int vw = wd->wd_TextW;
   int vh = wd->wd_TextH;

   if (wd->wd_UsedFont)
   {
      BGUI_CloseFont(wd->wd_UsedFont);
      wd->wd_UsedFont = NULL;
   };

   /*
    * The screen can be smaller than the TEXT
    * overscan sizes. Check if this is the case.
    */
   if (vw > sw) vw = sw;
   if (vh > sh) vh = sh;

   /*
    * Setup aspect dependant framing and the
    * window object.
    */
   if (wd->wd_Flags & WDF_AUTOASPECT)
   {
      /*
       * Guess aspect ratio.
       */
      thin = ((wd->wd_DrawInfo->dri_Resolution.X / wd->wd_DrawInfo->dri_Resolution.Y) <= 1);

      DoMultiSet(FRM_ThinFrame, thin, 5, wd->wd_Gadgets, wd->wd_LBorder, wd->wd_TBorder, wd->wd_RBorder, wd->wd_BBorder);
   };

   /*
    * Pick up the font to use. This will either
    * be the font specified with WINDOW_Font or
    * the screen font.
    */
   font = wd->wd_Font ? wd->wd_Font : scr->Font;

   /*
    * When the window has grown too big for the font, the
    * recomputation using the fallback font will start here.
    */
   reCalc:

   /*
    * Ask the master group to compute its
    * absolute minimum dimension.
    */
   GroupDimensions(wd->wd_Gadgets, font, (UWORD *)&gmw, (UWORD *)&gmh);

   /*
    * Save these.
    */
   wd->wd_MasterW = gmw;
   wd->wd_MasterH = gmh;

   /*
    * Borderless windows, and backdrop for that matter,
    * have no borders and system gadgets so this part
    * of the computation can be skipped.
    */
   if (!(wd->wd_WindowFlags & WFLG_BORDERLESS))
   {
      /*
       * Compute border sizes.
       */
      WBorderSize(wd, scr, font);

      /*
       * Compute totals of border gadgets.
       */
      bw = wd->wd_Left + wd->wd_Right;
      bh = wd->wd_Top  + wd->wd_Bottom;

      /*
       * Add the minimum window border sizes.
       */
      gmw += bw;
      gmh += bh;

      /*
       * Check to see if any of the border groups
       * have a larger minimum width or height than
       * the current window size.
       */
      if (wd->wd_LBorder)
      {
	 t1 = wd->wd_LHeight + wd->wd_Top + wd->wd_Bottom;
	 if (t1 > gmh) gmh = t1;
      };

      if (wd->wd_RBorder)
      {
	 t1 = wd->wd_RHeight + wd->wd_Top + wd->wd_Bottom;
	 if (t1 > gmh) gmh = t1;
      };

      if (wd->wd_TBorder)
      {
	 t1 = wd->wd_TWidth;
	 if ((wd->wd_Flags & WDF_TITLEZIP) ||
	    (wd->wd_WindowFlags & WFLG_SIZEGADGET)) t1 += wd->wd_ZoomW;
	 if (wd->wd_WindowFlags & WFLG_DEPTHGADGET) t1 += wd->wd_DepthW;
	 if (wd->wd_WindowFlags & WFLG_CLOSEGADGET) t1 += wd->wd_CloseW;
	 if (t1 > gmw) gmw = t1;
      };

      if (wd->wd_BBorder)
      {
	 t1 = wd->wd_BWidth;
	 if (wd->wd_WindowFlags & WFLG_SIZEGADGET) t1 += wd->wd_SizeW;
	 if (t1 > gmw) gmw = t1;
      }

      /*
       * Setup master gadget dimensions.
       */
      gx = wd->wd_Left;
      gy = wd->wd_Top;

      /*
       * And the border groups.
       */
      if (wd->wd_LBorder)
      {
	 DoSetMethodNG(wd->wd_LBorder, GA_Left,         0,
				       GA_Top,          wd->wd_Top,
				       GA_Width,        wd->wd_Left,
				       GA_RelHeight,  -(wd->wd_Top),
				       TAG_END);
      };

      if (wd->wd_TBorder)
      {
	 t1  = wd->wd_WindowFlags & WFLG_DEPTHGADGET ? wd->wd_DepthW - 1: 0;
	 t1 += (wd->wd_WindowFlags & WFLG_SIZEGADGET) ||
	       (wd->wd_Flags & WDF_TITLEZIP) ? wd->wd_ZoomW  - 1: 0;

	 DoSetMethodNG(wd->wd_TBorder, GA_RelRight,   -(wd->wd_TWidth + t1 - 1),
				       GA_Top,          0,
				       GA_Width,        wd->wd_TWidth,
				       GA_Height,       wd->wd_Top,
				       TAG_END);
      };

      if (wd->wd_RBorder)
      {
	 t1 = wd->wd_WindowFlags & WFLG_SIZEGADGET ? wd->wd_SizeH - wd->wd_Bottom : 0;

	 DoSetMethodNG(wd->wd_RBorder, GA_RelRight,   -(wd->wd_Right - 1),
				       GA_Top,          wd->wd_Top,
				       GA_Width,        wd->wd_Right,
				       GA_RelHeight,  -(wd->wd_Top + wd->wd_Bottom + t1),
				       TAG_END);
      };

      if (wd->wd_BBorder)
      {
	 t1 = /*wd->wd_LBorder ?*/ wd->wd_Left /*: 0*/;
	 t2 = wd->wd_WindowFlags & WFLG_SIZEGADGET ? wd->wd_SizeW : 0;

	 DoSetMethodNG(wd->wd_BBorder, GA_Left,         t1,
				       GA_RelBottom,  -(wd->wd_Bottom - 1),
				       GA_RelWidth,   -(t1 + t2),
				       GA_Height,       wd->wd_Bottom,
				       TAG_END);
      };
   }
   else
   {
      /*
       * No offsets necessary for
       * borderless windows.
       */
      gx = gy = bw = bh = 0;
   };

   /*
    * Set values to the master group.
    */
   DoSetMethodNG(wd->wd_Gadgets, GA_Left, gx, GA_Top, gy, GA_RelWidth, -bw, GA_RelHeight, -bh, TAG_DONE);

   /*
    * Backdrop window?
    */
   if (wd->wd_WindowFlags & WFLG_BACKDROP)
   {
      /*
       * We force the window to be display
       * sized or display sized minus the
       * screen title bar.
       */
      if (wd->wd_Flags & WDF_SHOWTITLE)
      {
	 sh = scr->Height - scr->BarHeight - 1;
      }
      else
      {
	 /*
	  * Hide title?
	  */
	 ShowTitle(scr, FALSE);
      };

      gmw = sw;
      gmh = sh;
   };

   /*
    * Final check. Does the window fit and does
    * the width and height of the window read
    * positive values? If not retry with the
    * fallback font.
    */
   if ((gmw > sw) || (gmh > sh))
   {
      /*
       * Did it fail with the
       * fallback font?
       */
      if (font == wd->wd_FallBackFont)
	 return FALSE;

      /*
       * Retry with the fallback font.
       */
      font = wd->wd_FallBackFont;
      goto reCalc;
   };

   if (wd->wd_UsedFont = BGUI_OpenFont(font))
   {
      *win_w = gmw;
      *win_h = gmh;

      return TRUE;
   };
   return FALSE;
}

/// WM_OPEN
/*
 * Open up the window.
 */
METHOD(WindowClassOpen, Msg, msg)
{
   WD                *wd = INST_DATA(cl, obj);
   WORD               wleft = 0, wtop = 0, width = 0, height = 0, mw, mh;
   WORD               vw, vh;
   struct Screen     *pubscreen;
   struct Rectangle   rect;
   struct Gadget     *glist = GADGET(wd->wd_Gadgets);
   ULONG              modeID;
   BOOL               keepw = wd->wd_Flags & WDF_LOCK_WIDTH;
   BOOL               keeph = wd->wd_Flags & WDF_LOCK_HEIGHT;
   BOOL               pub = TRUE;
   int                borders = 0;
   struct Window     *w;
   struct Message    *imsg;

   /*
    * If the window is already opened we
    * return a pointer to it.
    */
   if (w = wd->wd_WindowPtr)
      return (ULONG)w;

   /*
    * Link border gadgets.
    */
   if (wd->wd_RBorder)
   {
      GADGET(wd->wd_RBorder)->NextGadget = glist;
      glist = GADGET(wd->wd_RBorder);
      borders++;
   };

   if (wd->wd_BBorder)
   {
      GADGET(wd->wd_BBorder)->NextGadget = glist;
      glist = GADGET(wd->wd_BBorder);
      borders++;
   };

   if (wd->wd_LBorder)
   {
      GADGET(wd->wd_LBorder)->NextGadget = glist;
      glist = GADGET(wd->wd_LBorder);
      borders++;
   };

   if (wd->wd_TBorder)
   {
      GADGET(wd->wd_TBorder)->NextGadget = glist;
      glist = GADGET(wd->wd_TBorder);
      borders++;
   };

   /*
    * Obtain a pointer to the
    * screen to use.
    */
   if (wd->wd_Screen)
   {
      pubscreen = wd->wd_Screen;
      pub = FALSE;
   }
   else
   {
      if (wd->wd_PubScreen) pubscreen = wd->wd_PubScreen;
      else
      {
	 /*
	  * Lock the screen.
	  */
	 if (!(pubscreen = LockPubScreen(wd->wd_PubScreenName)))
	    /*
	     * Fallback to the default screen
	     * when locking failed. Should be
	     * made optional?
	     */
	    pubscreen = LockPubScreen(NULL);
	 /*
	  * Tell'm we locked the screen.
	  */
	 if (pubscreen)
	    wd->wd_Flags |= WDF_SCREEN_LOCKED;
      };
      wd->wd_Screen = pubscreen;
   };

   /*
    * Do we have a screen?
    */
   if (!pubscreen) return 0;

   /*
    * Set up BDrawInfo drawinfo and pens.
    */
   wd->wd_DrawInfo = GetScreenDrawInfo(pubscreen);

   /*
   for (i = 0; i < NUMDRIPENS; i++)
   {
      wd->wd_BDI.bdi_Pens[i] = (wd->wd_BDI.bdi_DrInfo && (i < wd->wd_BDI.bdi_DrInfo->dri_NumPens)) ?
	 wd->wd_BDI.bdi_DrInfo->dri_Pens[i] : DefDriPens[i];
   };
    */

   vw = pubscreen->Width;
   vh = pubscreen->Height;

   /*
    * Obtain the mode ID from the screen to use.
    */

   if ((modeID = GetVPModeID(&pubscreen->ViewPort)) != INVALID_ID)
   {
      /*
       * Find out TEXT overscan
       * dimensions.
       */
      if (QueryOverscan(modeID, &rect, OSCAN_TEXT))
      {
	 /*
	  * Compute TEXT overscan width and height
	  * for this display mode.
	  */
	 vw = rect.MaxX - rect.MinX + 1;
	 vh = rect.MaxY - rect.MinY + 1;
	 
      };
   };

   /*
    * Store the text overscan dimensions.
    */
   wd->wd_TextW = vw;
   wd->wd_TextH = vh;

   /* AROS FIX <-> AMIGA FIX? */

   if (vw > pubscreen->Width)  vw = pubscreen->Width;
   if (vh > pubscreen->Height) vh = pubscreen->Height;
   
   if (!WinSize(wd, (UWORD *)&width, (UWORD *)&height))
      goto failure;

   /*
    * Backdrop window?
    */
   if (wd->wd_WindowFlags & WFLG_BACKDROP)
   {
      /*
       * We force the window to be display
       * sized or display sized minus the
       * screen title bar.
       */
      wleft = 0;

      if (wd->wd_Flags & WDF_SHOWTITLE)
	 wtop = pubscreen->BarHeight + 1;
      else
	 wtop = 0;

      goto forceThis;
   }

   /*
    * The computed width and height are the
    * absolute minimum size that the window
    * can have. We save them here for the
    * window alternate (zipped) position.
    */
   mw = width;
   mh = height;

   /*
    * Scale the window size?
    */
   if (wd->wd_WidthScale)  width  += ((vw - width)  * wd->wd_WidthScale)  / 100;
   if (wd->wd_HeightScale) height += ((vh - height) * wd->wd_HeightScale) / 100;

   /*
    * When the window was opened before and the size is known or there
    * is previous size information available we use this information.
    */
   if (wd->wd_Flags & WDF_CHANGE_VALID || GetWindowBounds(wd->wd_ID, &wd->wd_FixedPos))
   {
      /*
       * If the width or height of the window
       * is fixed then we reuse the previous
       * values. If not we use the new values.
       */
      width  = keepw ? width  : wd->wd_FixedPos.Width;
      height = keeph ? height : wd->wd_FixedPos.Height;

      /*
       * If the window is placed relative to a
       * given rectangle we place it again relative
       * to that rectangle. Otherwise we copy the
       * previous position.
       */
      if (wd->wd_Flags & WDF_RELBOX)
      {
	 wleft = ((wd->wd_RelPos.Width  - width)  >> 1) + wd->wd_RelPos.Left;
	 wtop  = ((wd->wd_RelPos.Height - height) >> 1) + wd->wd_RelPos.Top;
      }
      else
      {
	 wleft = wd->wd_FixedPos.Left;
	 wtop  = wd->wd_FixedPos.Top;
      }

      /*
       * In case the font changed we need to check if the window has become
       * smaller than allowed and adjust if necessary.
       */
      if (width  < mw) width  = mw;
      if (height < mh) height = mh;

      /*
       * Does the window still fit in this
       * situation?
       */
      if (width  > pubscreen->Width  && mw <= pubscreen->Width)  width  = pubscreen->Width;
      if (height > pubscreen->Height && mh <= pubscreen->Height) height = pubscreen->Height;

      /*
       * Adjust the window position when necessary.
       */
      if ((wleft + width)  > pubscreen->Width)  wleft = pubscreen->Width  - width;
      if ((wtop  + height) > pubscreen->Height) wtop  = pubscreen->Height - height;
   }
   else
   {
      /*
       * Window positioned relative to a given rectangle?
       */
      if (!(wd->wd_Flags & WDF_RELBOX))
      {
	 /*
	  * What position is requested?
	  */
	 switch (wd->wd_Position)
	 {
	 case POS_CENTERSCREEN:
	    wleft = ((vw - width)  >> 1) - pubscreen->ViewPort.DxOffset;
	    wtop  = ((vh - height) >> 1) - pubscreen->ViewPort.DyOffset;
	    break;

	 case POS_CENTERMOUSE:
	    wleft = pubscreen->MouseX - (width  >> 1) - pubscreen->ViewPort.DxOffset;
	    wtop  = pubscreen->MouseY - (height >> 1) - pubscreen->ViewPort.DyOffset;
	    break;
	 };
      }
      else
      {
	 /*
	  * Setup window according to the
	  * rectangle.
	  */
	 wleft = ((wd->wd_RelPos.Width  - width)  >> 1) + wd->wd_RelPos.Left;
	 wtop  = ((wd->wd_RelPos.Height - height) >> 1) + wd->wd_RelPos.Top;
      };
   }

   forceThis:

   /*
    * Do we have menus defined?
    */
   if (wd->wd_MenuStrip)
   {
      /*
       * Set the menus up.
       */
      if (!DoMenuStrip(wd, pubscreen))
	 goto failure;
   };

   /*
    * Setup zip positions.
    */
   if (wd->wd_Flags & WDF_TITLEZIP)
   {
      /*
       * We always have a zoom gadget.
       */
      wd->wd_Zoom[2] = wd->wd_ZoomW - 1;

      /*
       * Add the pixel-width of the title + 60.
       */
      if (wd->wd_WindowTitle)                    wd->wd_Zoom[2] += TextWidth(&pubscreen->RastPort, wd->wd_WindowTitle) + 60;

      /*
       * And any other system gadgets.
       */
      if (wd->wd_WindowFlags & WFLG_DEPTHGADGET) wd->wd_Zoom[2] += wd->wd_DepthW - 1;
      if (wd->wd_WindowFlags & WFLG_CLOSEGADGET) wd->wd_Zoom[2] += wd->wd_CloseW - 1;

      /*
       * And a top-group minimum size.
       */
      if (wd->wd_TBorder)                        wd->wd_Zoom[2] += wd->wd_TWidth;

      /*
       * Title bar-height.
       */
      wd->wd_Zoom[3] = wd->wd_Top;
   }

   if (wd->wd_Flags & WDF_PREBUFFER) AsmDoMethod(wd->wd_Gadgets, BASE_RENDERBUFFER, pubscreen, width, height);

   /*
    * Open the window (finally :))
    */
   w = wd->wd_WindowPtr = OpenWindowTags(NULL,
	 WA_Left,           wleft,                           WA_Top,            wtop,
	 WA_Width,          width,                           WA_Height,         height,
	 WA_Flags,          wd->wd_WindowFlags,              WA_Gadgets,        glist,
	 WA_BackFill,       &BackFill_hook,                  WA_RptQueue,       1,
	 WA_MinWidth,       mw,                              WA_MinHeight,      mh,
	 WA_MaxWidth,       (wd->wd_Flags & WDF_LOCK_WIDTH)  ? mw : ~0,
	 WA_MaxHeight,      (wd->wd_Flags & WDF_LOCK_HEIGHT) ? mh : ~0,
	 wd->wd_WindowTitle ? WA_Title       : TAG_IGNORE,   wd->wd_WindowTitle,
	 wd->wd_ScreenTitle ? WA_ScreenTitle : TAG_IGNORE,   wd->wd_ScreenTitle,
	 pub ? WA_PubScreen : WA_CustomScreen,               pubscreen,
	 wd->wd_Flags & WDF_TITLEZIP ? WA_Zoom : TAG_IGNORE, wd->wd_Zoom,
	 WA_IDCMP,          wd->wd_IDCMPFlags,
	 WA_MenuHelp,       TRUE,                            TAG_END);

   if (!w) goto failure;

   /*
    * Set the font.
    */
   wd->wd_OrigFont = w->RPort->Font;
   FSetFont(w->RPort, wd->wd_UsedFont);

   /*
    * Move the top-border group to the
    * beginning of the list.
    */
   if (borders)
   {
      RemoveGList(w, glist, borders);
      AddGList(w, glist, 0, borders, NULL);
   };

   /*
    * Save window bounds.
    */
   SetWindowBounds(wd->wd_ID, (struct IBox *)&w->LeftEdge);

   /*
    * Setup menu-strip if one exists.
    */
   if (wd->wd_Menus)
      SetMenuStrip(w, wd->wd_Menus );

   /*
    * When we have a shared MsgPort we must move any initial messages to it,
    * then set it up.
    */
   if (wd->wd_Flags & WDF_SHARED_MSGPORT)
   {
      /*
       * Set it to a flag that will not occur, to stop input without closing the port.
       */
      ModifyIDCMP(w, IDCMP_REQCLEAR);

      while (imsg = GetMsg(w->UserPort))
      {
	 /*
	  * Dump message onto the other port.
	  */
	 PutMsg(wd->wd_UserPort, imsg);
      };
      /*
       * Free intuition's port.
       */
      ModifyIDCMP(w, 0);

      /*
       * Poke the shared port.
       */
      w->UserPort = wd->wd_UserPort;

      /*
       * Setup the flags.
       */
      ModifyIDCMP(w, wd->wd_IDCMPFlags);
   }
   else
   {
      /*
       * Pick up the intuition created message port.
       */
      wd->wd_UserPort = w->UserPort;
   };

   /*
    * If requested setup this window
    * as a workbench App-Window.
    */
   if (wd->wd_Flags & WDF_IS_APPWINDOW)
      /*
       * Quietly fails.
       */
      MakeAppWindow(wd);

   /*
    * Add window to the internal list.
    * Also fails quietly...
    */
   AddWindow(obj, w);

   return (ULONG)w;

   failure:

   AsmCoerceMethod(cl, obj, WM_CLOSE);

   return 0;
}
METHOD_END
///

/*
 * Clear all messages from
 * the port which belong to
 * the window.
 */
//STATIC ASM VOID ClearMsgPort( REG(a0) WD *wd )
STATIC ASM REGFUNC1(VOID, ClearMsgPort,
	REGPARAM(A0, WD *, wd))
{
   struct IntuiMessage     *imsg;
   struct Node       *succ;

   /*
    * Don't race intuition.
    */
   Forbid();

   imsg = ( struct IntuiMessage * )wd->wd_UserPort->mp_MsgList.lh_Head;

   while ( succ = imsg->ExecMessage.mn_Node.ln_Succ ) {
      if ( imsg->IDCMPWindow == wd->wd_WindowPtr ) {
	 Remove(( struct Node * )imsg );
	 ReplyMsg(( struct Message * )imsg );
      }
      imsg = ( struct IntuiMessage * )succ;
   }
   Permit();
}
REGFUNC_END
/// WM_CLOSE
/*
 * Close the window.
 */
METHOD(WindowClassClose, Msg, msg)
{
   WD            *wd = INST_DATA(cl, obj);
   ULONG          rc = 0;
   struct Window *w;

   if (w = wd->wd_WindowPtr)
   {
      /*
       * When a shared MsgPort is used
       * we clear the UserPort ptr in the window
       * structure.
       */
      Forbid();
      ClearMsgPort(wd);
      if (wd->wd_Flags & WDF_SHARED_MSGPORT)
      {
	 w->UserPort = NULL;
      };
      ModifyIDCMP(w, 0);
      Permit();

      /*
       * Close tool-tip window.
       */
      AsmDoMethod(obj, WM_CLOSETOOLTIP);

      /*
       * Wakeup the window.
       */
      while (wd->wd_SleepCnt)
	 AsmDoMethod(obj, WM_WAKEUP);

      /*
       * AppWindow?
       */
      if (wd->wd_Flags & WDF_IS_APPWINDOW)
	 KillAppWindow(wd);

      /*
       * Remove the window menus.
       */
      if (wd->wd_Menus) ClearMenuStrip(w);

      /*
       * Setup original font.
       */
      FSetFont(w->RPort, wd->wd_OrigFont);

      /*
       * Close the window.
       */
      CloseWindow(w);
      wd->wd_WindowPtr = NULL;

      /*
       * Remove it and all outstanding ID's from the internal lists.
       */
      RemWindow(obj);
      RemoveIDReport(w);

      /*
       * Clear continuation flags.
       */
      wd->wd_NextSelect = MENUNULL;
      wd->wd_Flags    &= ~WDF_DRAGSELECT;

      /*
       * Free the menus.
       */
      if (wd->wd_Menus)
      {
	 FreeMenus(wd->wd_Menus);
	 wd->wd_Menus = NULL;
      }

      if (wd->wd_BufferRP)
      {
	 wd->wd_BufferRP->Layer = wd->wd_BufferLayer;
	 BGUI_FreeRPortBitMap(wd->wd_BufferRP);
	 wd->wd_BufferRP = NULL;
      };

      rc = 1;
   };

   if (wd->wd_UsedFont)
   {
      BGUI_CloseFont(wd->wd_UsedFont);
      wd->wd_UsedFont = NULL;
   };

   if (wd->wd_VisualInfo)
   {
      FreeVisualInfo(wd->wd_VisualInfo);
      wd->wd_VisualInfo = NULL;
   };

   if (wd->wd_DrawInfo)
   {
      FreeScreenDrawInfo(wd->wd_Screen, wd->wd_DrawInfo);
      wd->wd_DrawInfo = NULL;
   };

   if (wd->wd_Flags & WDF_SCREEN_LOCKED)
   {
      UnlockPubScreen(NULL, wd->wd_Screen);
      wd->wd_Flags &= ~WDF_SCREEN_LOCKED;
   };

   if (wd->wd_PubScreenName)
   {
      wd->wd_Screen = NULL;
   };

   return rc;
}
METHOD_END
///
/// WM_SLEEP
/*
 * Put the window to sleep.
 */
METHOD(WindowClassSleep, Msg, msg)
{
   WD         *wd = INST_DATA(cl, obj);
   ULONG       rc = 0;

   /*
    * Only go to sleep if the window is open.
    */
   if (wd->wd_WindowPtr)
   {
      /*
       * If the sleep nest counter is 0 we setup a requester
       * and a busy pointer.
       */
      if (wd->wd_SleepCnt++ == 0)
	 wd->wd_Lock = BGUI_LockWindow(wd->wd_WindowPtr);
      /*
       * Return 1 for sucess.
       */
      rc = 1;
   };
   return rc;
}
METHOD_END
///
/// WM_WAKEUP
/*
 * Wake the window back up.
 */
METHOD(WindowClassWakeUp, Msg, msg)
{
   WD         *wd = INST_DATA(cl, obj);
   ULONG       rc = 0;

   /*
    * Only decrease the sleep nest counter when the
    * window is open _and_ the sleep nest counter is not 0.
    */
   if (wd->wd_Lock)
   {
      if (--wd->wd_SleepCnt == 0)
      {
	 /*
	  * When the sleep nest counter becomes zero we remove the
	  * requester and reset the pointer.
	  */
	 BGUI_UnlockWindow(wd->wd_Lock);
	 wd->wd_Lock = NULL;
      };
      rc = 1;
   };
   return rc;
}
METHOD_END
///

/*
 * Keep track of window bound changes.
 */
//STATIC ASM ULONG WindowClassChange( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
STATIC ASM REGFUNC3(ULONG, WindowClassChange,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
   WD            *wd = INST_DATA(cl, obj);
   ULONG          rc = 0;
   struct Window *w;

   if (w = wd->wd_WindowPtr)
   {
      wd->wd_FixedPos = *IBOX(&w->LeftEdge);
      wd->wd_Flags   |= WDF_CHANGE_VALID;
      rc = 1;
      /*
       * Save window bounds.
       */
      SetWindowBounds(wd->wd_ID, (struct IBox *)&w->LeftEdge);
   }
   return rc;
}
REGFUNC_END

/*
 * Get an attribute.
 */
//STATIC ASM ULONG WindowClassGet(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opGet *opg)
STATIC ASM REGFUNC3(ULONG, WindowClassGet,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opGet *, opg))
{
   WD       *wd = (WD *)INST_DATA(cl, obj);
   ULONG     rc = 1, *store = opg->opg_Storage;

   struct Window *win;

   switch (opg->opg_AttrID)
   {
      case WINDOW_GadgetOK:
	 STORE !(wd->wd_Flags & WDF_REMOVED);
	 break;

      case WINDOW_ToolTicks:
	 STORE wd->wd_ToolTickTime;
	 break;

      case WINDOW_MenuStrip:
	 STORE wd->wd_Menus;
	 break;

      case WINDOW_UserPort:
	 if (wd->wd_WindowPtr) STORE wd->wd_UserPort;
	 else                  STORE NULL;
	 break;

      case WINDOW_SigMask:
	 STORE (wd->wd_WindowPtr ? (1 << wd->wd_UserPort->mp_SigBit) : 0);
	 break;

      case WINDOW_AppMask:
	 STORE (wd->wd_AppPort ? (1 << wd->wd_AppPort->mp_SigBit) : 0);
	 break;

      case WINDOW_Window:
	 STORE wd->wd_WindowPtr;
	 break;

      case WINDOW_Font:
	 STORE wd->wd_Font;
	 break;

      case WINDOW_Bounds:
	 if ( wd->wd_WindowPtr )
	    *(( struct IBox * )store ) = *(( struct IBox * )&wd->wd_WindowPtr->LeftEdge );
	 else
	    rc = 0;
	 break;
	 
      case WINDOW_Title:
	 STORE wd->wd_WindowTitle;
	 break;
	 
      case WINDOW_ScreenTitle:
	 STORE wd->wd_ScreenTitle;
	 break;
	 
      case WINDOW_TitleID:
	 STORE wd->wd_WindowTitleID;
	 break;
	 
      case WINDOW_ScreenTitleID:
	 STORE wd->wd_ScreenTitleID;
	 break;
	 
      case WINDOW_HelpTextID:
	 STORE wd->wd_HelpTextID;
	 break;
	 
      case WINDOW_AutoAspect:
	 STORE test(wd->wd_Flags & WDF_AUTOASPECT);
	 break;
	 
      case WINDOW_BufferRP:
	 if ((win = wd->wd_WindowPtr) && !(wd->wd_Flags & WDF_NO_BUFFER) && (FGetDepth(win->RPort) <= 8))
	 {
	    if ((wd->wd_BRW != win->Width) || (wd->wd_BRH != win->Height))
	    {
	       wd->wd_BRW = win->Width;
	       wd->wd_BRH = win->Height;
	       if (wd->wd_BufferRP)
	       {
		  wd->wd_BufferRP->Layer = wd->wd_BufferLayer;
		  BGUI_FreeRPortBitMap(wd->wd_BufferRP);
		  wd->wd_BufferRP = NULL;
	       };
	    };
	    if (!wd->wd_BufferRP)
	    {
	       wd->wd_BufferRP = BGUI_CreateRPortBitMap(win->RPort, wd->wd_BRW, wd->wd_BRH, 0);
	       wd->wd_BufferLayer = wd->wd_BufferRP->Layer;
	       wd->wd_BufferRP->Layer = NULL;
	    };
	    STORE wd->wd_BufferRP;
	 }
	 else
	 {
	    STORE NULL;
	 };
	 break;

      default:
	 rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
	 break;
   }
   return rc;
}
REGFUNC_END

/*
 * Put out a help-request.
 */
METHOD(WindowClassHelp, Msg, msg)
{
   WD                   *wd = INST_DATA( cl, obj );
   struct bmShowHelp     bsh;
#ifdef __AROS__
#warning Commented the following line
#else
   struct NewAmigaGuide  nag = { NULL };
#endif
   ULONG                 rc = BMHELP_FAILURE;
   struct Window        *w;

   /*
    * Only when the window is open.
    */
   if (w = wd->wd_WindowPtr)
   {
      /*
       * Setup a help request for the
       * master object.
       */
      bsh.MethodID      = BASE_SHOWHELP;
      bsh.bsh_Window    = w;
      bsh.bsh_Requester = NULL;
      bsh.bsh_Mouse.X   = w->MouseX;
      bsh.bsh_Mouse.Y   = w->MouseY;
      /*
       * Go to sleep and send the
       * request.
       */
      AsmDoMethod(obj, WM_SLEEP);

      /*
       * Any help shown by the master object?
       */
      if (AsmDoMethodA(wd->wd_Gadgets, (Msg)&bsh) == BMHELP_NOT_ME)
      {
	 /*
	  * No. Show the window help.
	  */
	 if (wd->wd_HelpFile || wd->wd_HelpText)
	 {
	    /*
	     * Only if the mouse is located
	     * in the window.
	     */
	    if (bsh.bsh_Mouse.X > 0 && bsh.bsh_Mouse.Y > 0 && bsh.bsh_Mouse.X < w->Width && bsh.bsh_Mouse.Y < w->Height)
	    {
	       if (wd->wd_HelpText)
	       {
		  ShowHelpReq(w, wd->wd_HelpText);
		  rc = BMHELP_OK;
	       }
	       else
	       {
#ifdef __AROS__
#warning Commented the following lines
#else
		  nag.nag_Name   = (STRPTR)wd->wd_HelpFile;
		  nag.nag_Node   = (STRPTR)wd->wd_HelpNode;
		  nag.nag_Line   = wd->wd_HelpLine;
		  nag.nag_Screen = w->WScreen;

		  if (DisplayAGuideInfo(&nag, NULL))
		     rc = BMHELP_OK;
#endif
	       };
	    };
	 };
      };
      /*
       * Wakeup the window.
       */
      AsmDoMethod(obj, WM_WAKEUP);
   };
   return rc;
}
METHOD_END

/*
 * Fill up an InputEvent structure
 * with RAWKEY information.
 */
//STATIC ASM VOID FillIE(REG(a0) struct InputEvent *ie, REG(a1) struct IntuiMessage *imsg)
STATIC ASM REGFUNC2(VOID, FillIE,
	REGPARAM(A0, struct InputEvent *, ie),
	REGPARAM(A1, struct IntuiMessage *, imsg))
{
   ie->ie_Class              = IECLASS_RAWKEY;
   ie->ie_Code               = imsg->Code;
   ie->ie_Qualifier          = imsg->Qualifier;
   ie->ie_EventAddress       = imsg->IAddress ? *((APTR *)(imsg->IAddress)) : NULL;
   ie->ie_TimeStamp.tv_secs  = imsg->Seconds;
   ie->ie_TimeStamp.tv_micro = imsg->Micros;
}
REGFUNC_END

/*
 * Get the first message from the
 * window port which belongs to the window.
 */
//STATIC ASM struct IntuiMessage *GetIMsg( REG(a0) WD *wd )
STATIC ASM REGFUNC1(struct IntuiMessage *, GetIMsg,
	REGPARAM(A0, WD *, wd))
{
   struct IntuiMessage     *imsg;
   struct MsgPort       *mp = wd->wd_UserPort;

   /*
    * No need to race intuition.
    */
   Forbid();

   for ( imsg = ( struct IntuiMessage * )mp->mp_MsgList.lh_Head; imsg->ExecMessage.mn_Node.ln_Succ; imsg = ( struct IntuiMessage * )imsg->ExecMessage.mn_Node.ln_Succ ) {
      if ( imsg->IDCMPWindow == wd->wd_WindowPtr ) {
	 Remove(( struct Node * )imsg );
	 Permit();
	 return( imsg );
      }
   }

   Permit();
   return( NULL );
}
REGFUNC_END

/*
 * Find out if an object must receive a key message.
 */
STATIC ULONG KeyGadget(Class *cl, Object *obj, struct IntuiMessage *imsg)
{

   WD          *wd = INST_DATA(cl, obj);
   struct InputEvent ie;
   struct wmKeyInput wk;
   struct IntuiMessage  *im = NULL;
   APTR         iadd;
   ULONG        irc, id, rc = WMHI_IGNORE;

   ULONG class = imsg->Class;
   UWORD code  = imsg->Code;
   UWORD qual  = imsg->Qualifier;
   UBYTE key   = (UBYTE)code;

   struct Window *w = wd->wd_WindowPtr;
   
   Object *ob;

   /*
    * No menus during keyboard
    * activation.
    */
   if (!(wd->wd_WindowFlags & WFLG_RMBTRAP))
   {
      Forbid();
      w->Flags |= WFLG_RMBTRAP;
      Permit();
   }
   
   /*
    * Initialize input event.
    */
   if (code & IECODE_UP_PREFIX)
      goto end;

   FillIE(&ie, imsg);
   if (MapRawKey(&ie, &key, 1, NULL) == 1)
      code = key;
   else
   {
      code = code | 0xFF00;
   };

   #ifdef __AROS__
   if (!(ob = (Object *)AsmDoMethod(obj, BASE_FINDKEY, qual, code)))
      goto end;
   #else
   if (!(ob = (Object *)AsmDoMethod(obj, BASE_FINDKEY, (qual << 16) | code)))
      goto end;
   #endif
   
   /*
    * Initialize wmKeyInput structure.
    */
   wk.MethodID    =   WM_KEYACTIVE;
   wk.wmki_IEvent =   &ie;
   wk.wmki_ID     =   &id;
   wk.wmki_Key    =   &key;

   /*
    * Send a WM_KEYACTIVE message to the target.
    */
   irc = BGUI_DoGadgetMethodA(ob, w, NULL, (Msg)&wk);

   /*
    * Reply the originating message.
    */
   ReplyMsg((struct Message *)imsg);
   imsg = NULL;

   /*
    * Are we done?
    */
   if (irc & WMKF_VERIFY)
   {
      /*
       * Yes. Return the id.
       */
      rc = id;
      goto end;
   }
   else if (irc & WMKF_CANCEL)
   {
      /*
       * We are cancelled :^(
       * Return WMHI_IGNORE.
       */
      goto end;
   }
   else if (irc & WMKF_ACTIVATE)
   {
      /*
       * We are told to activate the
       * gadget through intuition channels.
       */
      ClearMsgPort(wd);
      ActivateGadget(GADGET(ob), w, NULL);
      goto end;
   }
   else
   {
      /*
       * Boom. We are active.
       */
      wk.MethodID = WM_KEYINPUT;

      for(;;)
      {
	 WaitPort(wd->wd_UserPort);
	 while (im = GetIMsg(wd))
	 {
	    class = im->Class;
	    code  = im->Code;
	    qual  = im->Qualifier;
	    iadd  = im->IAddress;
	    
	    switch (class)
	    {
	    /*
	     * Check out the RAWKEY event.
	     */
	    case IDCMP_RAWKEY:
	       if (qual & IEQUALIFIER_REPEAT) break;
	       if ((code & IECODE_UP_PREFIX) == 0)
	       {
		  myDoGadgetMethod(ob, w, NULL, WM_KEYINACTIVE, NULL);
		  goto end;
	       };
	       FillIE(&ie, im);
	       irc = BGUI_DoGadgetMethodA(ob, wd->wd_WindowPtr, NULL, (Msg)&wk);
	       if (irc & WMKF_VERIFY)
	       {
		  myDoGadgetMethod(ob, w, NULL, WM_KEYINACTIVE, NULL);
		  rc = id;
		  goto end;
	       }
	       else if (irc & WMKF_CANCEL)
	       {
		  myDoGadgetMethod(ob, w, NULL, WM_KEYINACTIVE, NULL);
		  goto end;
	       };
	       break;

	    case IDCMP_CLOSEWINDOW:
	       /*
		* Pass on the close window request.
		*/
	       myDoGadgetMethod(ob, w, NULL, WM_KEYINACTIVE, NULL);
	       rc = WMHI_CLOSEWINDOW;
	       goto end;

	    case IDCMP_GADGETUP:
	       /*
		* Pass on the gadget request.
		*/
	       myDoGadgetMethod(ob, w, NULL, WM_KEYINACTIVE, NULL);
	       rc = GADGET(iadd)->GadgetID;
	       goto end;

	    case IDCMP_INACTIVEWINDOW:
	       /*
		* Window inactive? Abort keyboard actions.
		*/
	       myDoGadgetMethod(ob, w, NULL, WM_KEYINACTIVE, NULL);
	       ClearMsgPort(wd);
	       goto end;

	    case IDCMP_SIZEVERIFY:
	       /*
		* Exit... NOW!
		*/
	       myDoGadgetMethod(ob, w, NULL, WM_KEYINACTIVE, NULL);
	       wd->wd_OW     = w->Width;
	       wd->wd_OH     = w->Height;
	       wd->wd_Flags |= WDF_REMOVED;
	       RemoveGadget(w, (struct Gadget *)wd->wd_Gadgets);
	       ClearMsgPort(wd);
	       goto end;

	    };
	    /*
	     * Reply the message.
	     */
	    ReplyMsg((struct Message *)im);
	 };
      };
   };
   end:

   /*
    * Reply the un-replyed messages.
    */
   if (imsg) ReplyMsg((struct Message *)imsg);
   if (im)   ReplyMsg((struct Message *)im);

   /*
    * Menu actions possible if they were before the keyboard stuff
    * and if we don't stay active.
    */
   if (!(wd->wd_WindowFlags & WFLG_RMBTRAP))
   {
      Forbid();
      w->Flags &= ~WFLG_RMBTRAP;
      Permit();
   };
   return rc;
}

/*
 * Find tab-cycle entry.
 */
//STATIC ASM TABCYCLE *GetCycleNode( REG(a0) WD *wd, REG(a1) Object *obj )
STATIC ASM REGFUNC2(TABCYCLE *, GetCycleNode,
	REGPARAM(A0, WD *, wd),
	REGPARAM(A1, Object *, obj))
{
   TABCYCLE    *tc;

   for ( tc = wd->wd_CycleList.cl_First; tc->tc_Next; tc = tc->tc_Next ) {
      if ( tc->tc_Object == obj )
	 return( tc );
   }
   return( NULL );
}
REGFUNC_END

/*
 * Perform update notification.
 */
//STATIC ASM VOID UpdateNotification( REG(a0) WD *wd, REG(a1) struct TagItem *attr, REG(d0) ULONG id )
STATIC ASM REGFUNC3(VOID, UpdateNotification,
	REGPARAM(A0, WD *, wd),
	REGPARAM(A1, struct TagItem *, attr),
	REGPARAM(D0, ULONG, id))
{
   struct TagItem       *clones;
   UPN            *up;

   /*
    * Clone attributes.
    */
   if ( clones = CloneTagItems( attr )) {
      /*
       * Find target.
       */
      for ( up = wd->wd_UpdateList.ul_First; up->up_Next; up = up->up_Next )
      {
	 /*
	  * Is this the one?
	  */
	 if ( up->up_ID == id )
	 {
	    /*
	     * Map clones when necessary.
	     */
	    if ( up->up_MapList )
	       MapTags( clones, up->up_MapList, TRUE );
	    /*
	     * Set attributes to the target.
	     */
	    myDoGadgetMethod(up->up_Target, wd->wd_WindowPtr, NULL, OM_UPDATE, clones, NULL, 0);
	    /*
	     * Unmap clones when necessary.
	     */
	    if ( up->up_MapList )
	       UnmapTags( clones, up->up_MapList );
	 }
      }
      /*
       * Free clones.
       */
      FreeTagItems( clones );
   }
}
REGFUNC_END



/*
 * Default tool tip hook.
 */
//STATIC ASM ULONG ToolTip_func(REG(a0) struct Hook *h, REG(a2) Object *obj, REG(a1) struct ttCommand *ttc)
STATIC ASM REGFUNC3(ULONG, ToolTip_func,
	REGPARAM(A0, struct Hook *, h),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct ttCommand *, ttc))
{
   WD                *wd;
   struct Window     *tw = NULL;
   struct TextFont   *of;
   struct RastPort   *rp;
   struct IBox        ibx;
   UWORD              x, y, ww, wh;
   UBYTE             *tip, *tc;
   ULONG              args[2];
   struct BaseInfo   *bi;

   switch (ttc->ttc_Command)
   {
   case TT_SHOW:
      wd = (WD *)ttc->ttc_UserData;

      /*
       * Does the tip-object have a tool-tip?
       */
      tc = NULL;
      Get_Attr(ttc->ttc_Object, BT_ToolTip, (ULONG *)&tc);
      if (tc)
      {
	 /*
	  * Copy and setup the tip.
	  */
	 if (tip = (UBYTE *)BGUI_AllocPoolMem(strlen(tc) + 10))
	 {
	    args[0] = BARDETAILPEN;
	    args[1] = (ULONG)tc;
	    DoSPrintF(tip, "\033d%ld%s", args);
	 }
	 else
	 {
	    tip = tc;
	    tc = NULL;
	 };

	 rp = wd->wd_WindowPtr->RPort;

	 /*
	  * Set the font.
	  */
	 of = rp->Font;
	 FSetFont(rp, wd->wd_UsedFont);

	 BGUI_InfoTextSize(rp, tip, (UWORD *)&ibx.Width, (UWORD *)&ibx.Height);
	 FSetFont(rp, of);

	 /*
	  * Get mouse position.
	  */
	 x = wd->wd_WindowPtr->MouseX;
	 y = wd->wd_WindowPtr->MouseY;

	 /*
	  * Tool-tip width and height.
	  */
	 ww = ibx.Width  + 8;
	 wh = ibx.Height + 4;

	 /*
	  * Open tool-tip window with it's
	  * bottom right corner under the mouse.
	  */
	 if (tw = OpenWindowTags(NULL, WA_Left,         wd->wd_WindowPtr->LeftEdge + x - ww,
				       WA_Top,          wd->wd_WindowPtr->TopEdge  + y - wh,
				       WA_Width,        ww,
				       WA_Height,       wh,
				       WA_IDCMP,        0,
				       WA_Flags,        WFLG_BORDERLESS|WFLG_RMBTRAP,
				       WA_CustomScreen, wd->wd_Screen,
				       TAG_DONE))
	 {
	    rp = tw->RPort;

#ifdef DEBUG_BGUI
	    if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_Screen, wd->wd_Screen, BI_RastPort, rp, TAG_DONE))
#else
	    if (bi = AllocBaseInfo(BI_Screen, wd->wd_Screen, BI_RastPort, rp, TAG_DONE))
#endif
	    {
	       /*
		* Set the font.
		*/
	       of = rp->Font;
	       BSetFont(bi, wd->wd_UsedFont);

	       /*
		* Tip background color.
		*/
	       BSetDrMd(bi, JAM1);
	       BSetDPenA(bi, BARBLOCKPEN);
	       BRectFill(bi, 1, 1, ww - 2, wh - 2);

	       /*
		* Tip border and text.
		*/
	       BSetDPenA(bi, BARDETAILPEN);
	       Move(rp, 0,      0);
	       Draw(rp, ww - 1, 0);
	       Draw(rp, ww - 1, wh - 1);
	       Draw(rp, 0,      wh - 1);
	       Draw(rp, 0,      0);

	       /*
		* Render the tip.
		*/
	       ibx.Left = 4;
	       ibx.Top  = 2;

	       RenderText(bi, tip, &ibx);
	       BSetFont(bi, of);
	    };
	    FreeBaseInfo(bi);
	 }
	 /*
	  * Free copy.
	  */
	 if (tc) BGUI_FreePoolMem(tip);
      }
      break;

   case TT_HIDE:
      tw = (struct Window *)ttc->ttc_UserData;
      if (tw) CloseWindow(tw);
      break;
   };

   return (ULONG)tw;
}
REGFUNC_END
static struct Hook ToolTipHook = { NULL, NULL, (HOOKFUNC)ToolTip_func, NULL, NULL };

/// WM_CLOSETOOLTIP

METHOD(WindowClassCloseTT, Msg, msg)
{
   WD               *wd = INST_DATA(cl, obj);
   struct ttCommand  ttc;

   if (wd->wd_ToolTip)
   {
      ttc.ttc_Command  = TT_HIDE;
      ttc.ttc_Object   = wd->wd_ToolTipObject;
      ttc.ttc_UserData = (APTR)wd->wd_ToolTip;

      BGUI_CallHookPkt(wd->wd_ToolTipHook ? wd->wd_ToolTipHook : &ToolTipHook, (void *)obj, (void *)&ttc);

      wd->wd_ToolX   = wd->wd_ToolY = -1;
      wd->wd_ToolTip = NULL;
   };
   return 1;
}
METHOD_END
///
/// WM_HANDLEIDCMP
/*
 * Handle a window's IDCMP messages.
 */
METHOD(WindowClassIDCMP, Msg, msg)
{
   struct IntuiMessage   *imsg;
   struct TagItem        *attr, *tag;
   struct MenuItem       *mi;
   TABCYCLE              *tc;
   WD                    *wd = INST_DATA(cl, obj);
   ULONG                  rc = WMHI_IGNORE, inhibit, id;
   UWORD                  code;
   Object                *obja;
   struct grmWhichObject  grwo;
   struct ttCommand       ttc;
   WORD                   x, y;
   ULONG                  mouseact;
   BOOL                   forward;
   Object                *tabbed;
   struct Window         *w = wd->wd_WindowPtr;
  

   /*
    * No window!
    */
   if (!w) return WMHI_NOMORE;

   /*
    * Locked out?
    */
   if (!(wd->wd_Flags & WDF_LOCKOUT))
   {
      /*
       * Are we in a drag-selection loop?
       * If so continue it.
       */
      if (wd->wd_Flags & WDF_DRAGSELECT)
      {
	 code = wd->wd_NextSelect;
	 imsg = NULL;
	 goto dragSelect;
      }

      /*
       * Reports on the stack?
       */
      if ((id = GetIDReport(w)) != ~0)
      {
	 /*
	  * Return the ID.
	  */
	 return id;
      };
   };

   /*
    * Is there a message.
    */
   while (imsg = GetIMsg(wd))
   {
      /*
       * Any movement?
       */
      {
	 BOOL close_tooltip;

	 if(!(close_tooltip=(w->MouseX != wd->wd_ToolX || w->MouseY != wd->wd_ToolY)))
	 {
	    switch(imsg->Class)
	    {
	       
	       case IDCMP_IDCMPUPDATE:
	       case IDCMP_DISKINSERTED:
	       case IDCMP_DISKREMOVED:
	       case IDCMP_WBENCHMESSAGE:
	       case IDCMP_INTUITICKS:
		  break;
	       default:
		  close_tooltip=TRUE;
		  break;
	    }
	 }
	 if(close_tooltip)
	 {
	    /*
	     * Do we have a tool-tip to close?
	     */
	    if (wd->wd_ToolTip)
	       AsmDoMethod(obj, WM_CLOSETOOLTIP);
	    else
	       wd->wd_ToolTicks = 0;
	 }
      }

      /*
       * Are we locked out?
       */
      if (wd->wd_Flags & WDF_LOCKOUT)
      {
	 /*
	  * Yes. Reply this message and continue.
	  */
	 ReplyMsg((struct Message *)imsg);
	 continue;
      };

      /*
       * Call the hook for the xxxxVERIFY messages
       * if one exists.
       */
      if (wd->wd_VerifyHook)
      {
	 if ((wd->wd_VerifyHookBits & imsg->Class) == imsg->Class)
	 {
	    BGUI_CallHookPkt(wd->wd_VerifyHook, (void *)obj, (void *)imsg);
	    ReplyMsg((struct Message *)imsg);
	    return rc;
	 }
      }

      /*
       * Call the IDCMPHook when it's available.
       */
       
      if (wd->wd_IDCMPHook && (wd->wd_IDCMPHookBits & imsg->Class))
      {
	 BGUI_CallHookPkt(wd->wd_IDCMPHook, (VOID *)obj, (VOID *)imsg);
      };

      code = imsg->Code;

      switch (imsg->Class)
      {
      case IDCMP_SIZEVERIFY:
	 /*
	  * Save window size and remove
	  * the master.
	  */
	 wd->wd_OW = w->Width;
	 wd->wd_OH = w->Height;
	 if (!(wd->wd_Flags & WDF_REMOVED))
	 {
	    wd->wd_Flags |= WDF_REMOVED;
	    RemoveGadget(w, (struct Gadget *)wd->wd_Gadgets);
	 }
	 break;

      case IDCMP_NEWSIZE:
WW(kprintf("WindowClassIDCMP: received IDCMP_NEWSIZE\n"));
	 /*
	  * If we have manually removed the
	  * master object we adjust the size
	  * ourselves.
	  */
	 if (wd->wd_Flags & WDF_REMOVED)
	 {
	    /*
	     * Setup new size for the master.
	     */
	    DoSetMethodNG(wd->wd_Gadgets, GA_Left,        w->BorderLeft,
					  GA_Top,         w->BorderTop,
					  GA_RelWidth,  -(w->BorderLeft + w->BorderRight ),
					  GA_RelHeight, -(w->BorderTop  + w->BorderBottom),
					  TAG_END);
	    /*
	     * Put master back online.
	     */
	    AddGadget(w, (struct Gadget *)wd->wd_Gadgets, -1);

	    /*
	     * Clear removed flag.
	     */
	    wd->wd_Flags &= ~WDF_REMOVED;

	    /*
	     * Only refresh the window when the
	     * size actually changed.
	     *
	     */


	    if ((w->Width  != wd->wd_OW) || (w->Height != wd->wd_OH))
	       RefreshWindowFrame(w);
	 }
	 break;

      case IDCMP_MOUSEBUTTONS:
	 if ((code | IECODE_UP_PREFIX) != SELECTUP)
	 {
	    /*
	     * Preset GRM_WHICHOBJECT method.
	     */
	    grwo.MethodID      = GRM_WHICHOBJECT;
	    grwo.grwo_Coords.X = imsg->MouseX;
	    grwo.grwo_Coords.Y = imsg->MouseY;

	    if (obja = (Object *)AsmDoMethodA(wd->wd_Gadgets, (Msg)&grwo))
	    {
	       Get_Attr(obja, BT_MouseActivation, &mouseact);

	       if ((mouseact & MOUSEACT_RMB_ACTIVE) && (code == IECODE_RBUTTON)
		|| (mouseact & MOUSEACT_MMB_ACTIVE) && (code == IECODE_MBUTTON))
	       {
		  SetAttrs(obja, BT_ReportID,
		  ((mouseact & MOUSEACT_RMB_REPORT) && (code == IECODE_RBUTTON)) ||
		  ((mouseact & MOUSEACT_MMB_REPORT) && (code == IECODE_MBUTTON)), TAG_DONE);

		  if (wd->wd_DGMObject)
		  {
		     DoSetMethodNG(wd->wd_DGMObject, DGM_Object, obja, DGM_IntuiMsg, imsg,TAG_END);
		     ActivateGadget((struct Gadget *)wd->wd_DGMObject, w, NULL);
		  };
	       };
	    };
	 };
	 break;

      case IDCMP_MOUSEMOVE:
	 if (wd->wd_WindowFlags & WFLG_RMBTRAP)
	 {
	    Forbid();
	    wd->wd_WindowPtr->Flags |= WFLG_RMBTRAP;
	    Permit();
	 }
	 else
	 {
	    mouseact = 0;
	    /*
	     * Preset GRM_WHICHOBJECT method.
	     */
	    grwo.MethodID        = GRM_WHICHOBJECT;
	    grwo.grwo_Coords.X   = imsg->MouseX;
	    grwo.grwo_Coords.Y   = imsg->MouseY;

	    if (obja = (Object *)AsmDoMethodA(wd->wd_Gadgets, (Msg)&grwo))
	    {
	       Get_Attr(obja, BT_MouseActivation, &mouseact);
	    };

	    Forbid();
	    if (mouseact & MOUSEACT_RMB_ACTIVE)
	    {
	       wd->wd_WindowPtr->Flags |= WFLG_RMBTRAP;
	    }
	    else
	    {
	       wd->wd_WindowPtr->Flags &= ~WFLG_RMBTRAP;
	    };
	    Permit();
	 };
	 break;

      case  IDCMP_ACTIVEWINDOW:
	 rc = WMHI_ACTIVE;
	 break;

      case  IDCMP_INACTIVEWINDOW:
	 rc = WMHI_INACTIVE;
	 break;

      case  IDCMP_CLOSEWINDOW:
	 rc = WMHI_CLOSEWINDOW;
	 break;

      case  IDCMP_CHANGEWINDOW:
WW(kprintf("WindowClassIDCMP: received IDCMP_CHANGEWINDOW\n"));
	 if (wd->wd_Flags & WDF_CONSTRAINTS)
	 {
WW(kprintf("WindowClassIDCMP: WDF_CONSTRAINTS is set\n"));
WW(kprintf("WindowClassIDCMP: calling WindowLimits(%d,%d,%d,%d)\n",wd->wd_MinW,wd->wd_MinH,wd->wd_MaxW,wd->wd_MaxH));
	    WindowLimits(w, wd->wd_MinW, wd->wd_MinH, wd->wd_MaxW, wd->wd_MaxH);
	    wd->wd_Flags &= ~WDF_CONSTRAINTS;

WW(kprintf("WindowClassIDCMP: calling WM_RELEASE\n"));
	    AsmDoMethod(obj, WM_RELEASE);
	 };
WW(kprintf("WindowClassIDCMP: calling WindowClassChange\n"));
	 WindowClassChange(cl, obj, (Msg)NULL);
	 break;

      case IDCMP_MENUPICK:
	 /*
	  * Menu selected?
	  */
	 if (code != MENUNULL)
	 {
	    dragSelect:
	    /*
	     * Get the selected item.
	     */
	    mi = ItemAddress(wd->wd_Menus, code);

	    /*
	     * Setup the return code ID.
	     */
	    rc = (ULONG)GTMENUITEM_USERDATA(mi);

	    /*
	     * Menu a CHECKIT item?
	     */
	    if (mi->Flags & CHECKIT)
	       /*
		* Fix the NewMenu array to represent
		* any changes in the CHECKED flag.
		*/
	       AsmDoMethod(obj, WM_CHECKITEM, rc, mi->Flags & CHECKED);
	    /*
	     * Set up the next item and say were
	     * drag-selecting.
	     */
	    if (mi->NextSelect != MENUNULL)
	    {
	       wd->wd_NextSelect = mi->NextSelect;
	       wd->wd_Flags     |= WDF_DRAGSELECT;
	       break;
	    };
	 };
	 /*
	  * Drag-selecting is done.
	  */
	 wd->wd_Flags &= ~WDF_DRAGSELECT;
	 break;

      case IDCMP_MENUHELP:
	 /*
	  * Get the selected item.
	  */
	 mi = ItemAddress(wd->wd_Menus, code);

	 /*
	  * Setup the return code ID.
	  */
	 if (mi) rc = WMHI_MENUHELP | (ULONG)GTMENUITEM_USERDATA(mi);
	 break;

      case IDCMP_RAWKEY:
	 if (code == 0x45)
	 {
	    /*
	     * Close on ESC?
	     */
	    if (wd->wd_Flags & WDF_CLOSEONESC)
	    {
	       rc = WMHI_CLOSEWINDOW;
	       break;
	    };
	 };
	 if (code == 0x5F)
	 {
	    /*
	     * HELP!!!!
	     */
	    WindowClassHelp(cl, obj, (Msg)NULL);
	    break;
	 };
	 /*
	  * See if this key triggers a gadget activation.
	  */
	 rc = KeyGadget(cl, obj, imsg);
	 /*
	  * The KeyGadget() routine has already replied the message for us.
	  */
	 imsg = NULL;
	 break;

      case IDCMP_GADGETUP:
	 rc = GADGET(imsg->IAddress)->GadgetID;
	 break;

      case IDCMP_IDCMPUPDATE:
	 /*
	  * IDCMP_IDCMPUPDATE handles the messages from
	  * slider, scroller objects and tab-cycling.
	  */
	 attr = (struct TagItem *)imsg->IAddress;

	 /*
	  * GA_ID makes sure it's comes from a gadget.
	  */
	 if (tag = FindTagItem(GA_ID, attr))
	 {
	    /*
	     * Pick up ID.
	     */
	    if (!FindTagItem(STRINGA_TextVal, attr) && !FindTagItem(STRINGA_LongVal, attr))
	       id = tag->ti_Data;
	    else
	       id = WMHI_IGNORE;

	    if (tabbed = (Object *)GetTagData(STRINGA_Tabbed, NULL, attr))
	    {
	       forward = TRUE;
	       /*
		* Do not notify!
		*/
	       rc = WMHI_IGNORE;
	    }
	    else if (tabbed = (Object *)GetTagData(STRINGA_ShiftTabbed, NULL, attr))
	    {
	       forward = FALSE;
	       /*
		* Do not notify!
		*/
	       rc = WMHI_IGNORE;
	    }
	    else if (tabbed = (Object *)GetTagData(WINDOW_ActNext, NULL, attr))
	    {
	       forward = TRUE;
	       /*
		* Notify.
		*/
	       rc = id;
	    }
	    else if (tabbed = (Object *)GetTagData(WINDOW_ActPrev, NULL, attr))
	    {
	       forward = FALSE;
	       /*
		* Notify.
		*/
	       rc = id;
	    }

	    /*
	     * String gadget tabbed?
	     */
	    if (tabbed)
	    {
	       /*
		* Check if it's in our list.
		*/
	       if (tc = GetCycleNode(wd, tabbed))
	       {
		  nextTabbed:
		  if (forward)
		  {
		     /*
		      * Activate next or first object.
		      */
		     if (tc != wd->wd_CycleList.cl_Last) tc = tc->tc_Next;
		     else                                tc = wd->wd_CycleList.cl_First;
		 }
		 else
		 {
		     /*
		      * Activate previous or last object.
		      */
		     if (tc != wd->wd_CycleList.cl_First) tc = tc->tc_Prev;
		     else                                 tc = wd->wd_CycleList.cl_Last;
		  };
		  Get_Attr(tc->tc_Object, BT_Inhibit, &inhibit);
		  if ((GADGET(tc->tc_Object)->Flags & GFLG_DISABLED) || inhibit)
		     goto nextTabbed;
		  ActivateGadget(GADGET(tc->tc_Object), w, NULL);
	       };
	    }
	    else
	    {
	       /*
		* Update notification.
		*/
	       UpdateNotification(wd, attr, id);
	       /*
		* Setup return code.
		*/
	       rc = id;
	    }
	 }
	 break;

      case  IDCMP_INTUITICKS:
	 if ( wd->wd_ToolTip || ( ! wd->wd_ToolTickTime ))
	    break;

	 /*
	  * Pick up mouse coordinates.
	  */
	 x = w->MouseX;
	 y = w->MouseY;

	 /*
	  * Preset GRM_WHICHOBJECT method.
	  */
	 grwo.MethodID      = GRM_WHICHOBJECT;
	 grwo.grwo_Coords.X = x;
	 grwo.grwo_Coords.Y = y;

	 /*
	  * Did the mouse position change?
	  */
	 if (x == wd->wd_ToolX && y == wd->wd_ToolY)
	 {
	    /*
	     * Tick delay reached?
	     */
	    if (wd->wd_ToolTicks++ == wd->wd_ToolTickTime)
	    {
	       /*
		* Find object under the mouse.
		*/
	       if (wd->wd_ToolTipObject = (Object *)AsmDoMethodA(wd->wd_Gadgets, (Msg)&grwo))
	       {
		  /*
		   * Show the tool tip.
		   */
		  ttc.ttc_Command  = TT_SHOW;
		  ttc.ttc_Object   = wd->wd_ToolTipObject;
		  ttc.ttc_UserData = (APTR)wd;

		  wd->wd_ToolTip   = (APTR)BGUI_CallHookPkt(wd->wd_ToolTipHook ? wd->wd_ToolTipHook : &ToolTipHook,
						 (void *)obj, (void *)&ttc);
	       };

	       /*
		* Clear tip data.
		*/
	       wd->wd_ToolTicks  = 0;
	    }
	 } else {
	    wd->wd_ToolX = x;
	    wd->wd_ToolY = y;
	 }
	 break;
      }

      /*
       * Reply the original message.
       */
      if (imsg) ReplyMsg((struct Message *)imsg);
      if (rc != WMHI_IGNORE) return rc;
   };
   return WMHI_NOMORE;
}
METHOD_END
///
/// WM_GADGETKEY
/*
 * Add a gadget key.
 */
METHOD(WindowClassGadgetKey, struct wmGadgetKey *, wmg)
{
// WD       *wd = INST_DATA(cl, obj);
   Object   *ob;
   
   /*
    * Only add when the object is valid.
    */
   if (ob = wmg->wmgk_Object)
   {
      DoSetMethodNG(ob, BT_Key, wmg->wmgk_Key, TAG_DONE);
   };
   return 1;
}
METHOD_END
///
/*
 * Find a menu by it's ID.
 */
//STATIC ASM struct Menu *FindMenu( REG(a0) struct Menu *ptr, REG(d0) ULONG id )
STATIC ASM REGFUNC2(struct Menu *, FindMenu,
	REGPARAM(A0, struct Menu *, ptr),
	REGPARAM(D0, ULONG, id))
{
   struct Menu    *tmp;

   if ( tmp = ptr ) {
      while ( tmp ) {
	 if ( id == ( ULONG )GTMENU_USERDATA( tmp )) return( tmp );
	 tmp = tmp->NextMenu;
      }
   }

   return( NULL );
}
REGFUNC_END

/*
 * Find a (sub)item by it's ID.
 */
//STATIC ASM struct MenuItem *FindItem( REG(a0) struct Menu *ptr, REG(d0) ULONG id )
STATIC ASM REGFUNC2(struct MenuItem *, FindItem,
	REGPARAM(A0, struct Menu *, ptr),
	REGPARAM(D0, ULONG, id))
{
   struct Menu    *tmp;
   struct MenuItem         *item, *sub;

   if ( tmp = ptr ) {
      while ( tmp ) {
	 if ( item = tmp->FirstItem ) {
	    while ( item ) {
	       if ( id == ( ULONG )GTMENUITEM_USERDATA( item )) return( item );
	       if ( sub = item->SubItem ) {
		  while ( sub ) {
		     if ( id == ( ULONG )GTMENUITEM_USERDATA( sub )) return( sub );
		     sub = sub->NextItem;
		  }
	       }
	       item = item->NextItem;
	    }
	 }
	 tmp = tmp->NextMenu;
      }
   }

   return( NULL );
}
REGFUNC_END

/*
 * Find a NewMenu by it's ID.
 */
//STATIC ASM struct NewMenu *FindNewMenu( REG(a0) struct NewMenu *nm, REG(d0) ULONG id )
STATIC ASM REGFUNC2(struct NewMenu *, FindNewMenu,
	REGPARAM(A0, struct NewMenu *, nm),
	REGPARAM(D0, ULONG, id))
{
   while ( nm->nm_Type != NM_END ) {
      if ( id == ( ULONG )nm->nm_UserData ) return( nm );
      nm++;
   }

   return( NULL );
}
REGFUNC_END

/*
 * Disable a menu.
 */
//STATIC ASM ULONG WindowClassDisableMenu( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct wmMenuAction *wmma )
STATIC ASM REGFUNC3(ULONG, WindowClassDisableMenu,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct wmMenuAction *, wmma))
{
   WD       *wd = ( WD * )INST_DATA( cl, obj );
   struct Menu    *menu;
   struct MenuItem         *item;
   struct NewMenu    *newmenu;
   ULONG        rc = FALSE;

   /*
    * Is the window open and
    * are there menus?
    */
   if ( wd->wd_Menus && wd->wd_WindowPtr ) {
      /*
       * Remove the menus.
       */
      ClearMenuStrip( wd->wd_WindowPtr );
      if ( menu = FindMenu( wd->wd_Menus, wmma->wmma_MenuID )) {
	 /*
	  * Change the menu status.
	  */
	 if ( wmma->wmma_Set ) menu->Flags &= ~MENUENABLED;
	 else           menu->Flags |= MENUENABLED;
      } else if ( item = FindItem( wd->wd_Menus, wmma->wmma_MenuID )) {
	 /*
	  * Otherwise the (sub)item status.
	  */
	 if ( wmma->wmma_Set ) item->Flags &= ~ITEMENABLED;
	 else           item->Flags |= ITEMENABLED;
      }
      /*
       * Put the menus back online.
       */
      ResetMenuStrip( wd->wd_WindowPtr, wd->wd_Menus );
   }

   /*
    * Also change the corresponding NewMenu accoordingly.
    */
   if ( wd->wd_MenuStrip ) {
      if ( newmenu = FindNewMenu( wd->wd_MenuStrip, wmma->wmma_MenuID )) {
	 rc = TRUE;
	 if ( newmenu->nm_Type == NM_TITLE ) {
	    if ( wmma->wmma_Set ) newmenu->nm_Flags |= NM_MENUDISABLED;
	    else           newmenu->nm_Flags &= ~NM_MENUDISABLED;
	 } else if ( newmenu->nm_Type != NM_END ) {
	    if ( wmma->wmma_Set ) newmenu->nm_Flags |= NM_ITEMDISABLED;
	    else           newmenu->nm_Flags &= ~NM_ITEMDISABLED;
	 }
      }
   }

   return( rc );
}
REGFUNC_END

/*
 * (Un)check an item.
 */
//STATIC ASM ULONG WindowClassCheckItem( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct wmMenuAction *wmma )
STATIC ASM REGFUNC3(ULONG, WindowClassCheckItem,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct wmMenuAction *, wmma))
{
   WD       *wd = ( WD * )INST_DATA( cl, obj );
   struct MenuItem         *item;
   struct NewMenu    *newmenu;
   ULONG        rc = FALSE;

   /*
    * Is the window open and are there menus?
    */
   if ( wd->wd_Menus && wd->wd_WindowPtr ) {
      /*
       * Remove the menus.
       */
      ClearMenuStrip( wd->wd_WindowPtr );
      /*
       * Find the item and change it's
       * CHECKED status.
       */
      if ( item = FindItem( wd->wd_Menus, wmma->wmma_MenuID )) {
	 if ( wmma->wmma_Set ) item->Flags |= CHECKED;
	 else           item->Flags &= ~CHECKED;
      }
      /*
       * Put the menus back online.
       */
      ResetMenuStrip( wd->wd_WindowPtr, wd->wd_Menus );
   }

   /*
    * Change the corresponding NewMenu accoordingly.
    */
   if ( wd->wd_MenuStrip ) {
      if ( newmenu = FindNewMenu( wd->wd_MenuStrip, wmma->wmma_MenuID )) {
	 rc = TRUE;
	 if ( wmma->wmma_Set ) newmenu->nm_Flags |= CHECKED;
	 else           newmenu->nm_Flags &= ~CHECKED;
      }
   }

   return( rc );
}
REGFUNC_END

/*
 * Ask for the disabled status of a menu or (sub)item.
 */
//STATIC ASM ULONG WindowClassMenuDisabled( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct wmMenuQuery *wmmq )
STATIC ASM REGFUNC3(ULONG, WindowClassMenuDisabled,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct wmMenuQuery *, wmmq))
{
   struct NewMenu       *newmenu;
   WD          *wd = ( WD * )INST_DATA( cl, obj );

   if ( wd->wd_MenuStrip ) {
      if ( newmenu = FindNewMenu( wd->wd_MenuStrip, wmmq->wmmq_MenuID )) {
	 if ( newmenu->nm_Type == NM_TITLE )
	    return (ULONG)(newmenu->nm_Flags & NM_MENUDISABLED ? 1 : 0);
	 else if ( newmenu->nm_Type != NM_END )
	    return (ULONG)(newmenu->nm_Flags & NM_ITEMDISABLED ? 1 : 0);
      }
   }

   return ~0;
}
REGFUNC_END

/*
 * Ask for the CHECKED status of a (sub)item.
 */
//STATIC ASM ULONG WindowClassItemChecked( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct wmMenuQuery *wmmq )
STATIC ASM REGFUNC3(ULONG, WindowClassItemChecked,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct wmMenuQuery *, wmmq))
{
   struct NewMenu       *newmenu;
   WD          *wd = ( WD * )INST_DATA( cl, obj );

   if ( wd->wd_MenuStrip ) {
      if ( newmenu = FindNewMenu( wd->wd_MenuStrip, wmmq->wmmq_MenuID ))
	 return (ULONG)(newmenu->nm_Flags & CHECKED ? 1 : 0);
   }

   return ~0;
}
REGFUNC_END

/*
 * Add an objects to the tab-cycle list.
 */
METHOD(WindowClassCycleOrder, struct wmTabCycleOrder *, tco)
{
   WD             *wd = INST_DATA( cl, obj );
   TABCYCLE       *tc;
   Object        **ob = &tco->wtco_Object1;
   ULONG           rc = 1;

   /*
    * Cycle through list of objects.
    */
   while (*ob)
   {
      /*
       * Allocate tab-cycle node.
       */
      if (tc = (TABCYCLE *)BGUI_AllocPoolMem(sizeof(TABCYCLE)))
      {
	 /*
	  * Initialize structure.
	  */
	 tc->tc_Object = *ob;
	 /*
	  * Setup the object for tab-cycling.
	  */
	 DoSetMethodNG( *ob, GA_TabCycle, TRUE, ICA_TARGET, ICTARGET_IDCMP, TAG_END );
	 /*
	  * Add it to the list.
	  */
	 AddTail(( struct List * )&wd->wd_CycleList, ( struct Node * )tc );
      } else
	 rc = 0;
      ob++;
   };
   return rc;
}
METHOD_END
/// WM_GETAPPMSG
/*
 * Obtain an AppMessage.
 */
METHOD(WindowClassGetAppMsg, Msg, msg)
{
   WD       *wd = INST_DATA(cl, obj);
   ULONG     rc = 0;

   /*
    * Pop a message from the App port.
    */
   if (wd->wd_AppPort)
      rc = (ULONG)GetMsg(wd->wd_AppPort);

   return rc;
}
METHOD_END
///
/// WM_ADDUPDATE
/*
 * Add a target to the update notification list.
 */
METHOD(WindowClassAddUpdate, struct wmAddUpdate *, wmau)
{
   WD          *wd = INST_DATA(cl, obj);
   UPN         *up;
   ULONG        rc = 0;

   /*
    * Valid target?
    */
   if (wmau->wmau_Target)
   {
      /*
       * Allocate node.
       */
      if (up = (UPN *)BGUI_AllocPoolMem(sizeof(UPN)))
      {
	 /*
	  * Initialize node.
	  */
	 up->up_ID      = wmau->wmau_SourceID;
	 up->up_Target  = wmau->wmau_Target;
	 up->up_MapList = wmau->wmau_MapList;
	 /*
	  * Add it to the list.
	  */
	 AddTail((struct List *)&wd->wd_UpdateList, (struct Node *)up);
	 rc = 1;
      };
   };
   return rc;
}
METHOD_END
///
/// WM_REPORT_ID
/*
 *  Report an ID.
 *
 *  Changes made by T.Herold: Passes task to AddIDReport
 */
METHOD(WindowClassReportID, struct wmReportID *, wmri)
{
   WD            *wd = INST_DATA(cl, obj);
   ULONG          rc = 0;
   struct Task   *task;
   struct Window *w;
   /*
    *  Get task to signal.
    */
   if (wmri->wmri_Flags & WMRIF_TASK)
      task = wmri->wmri_SigTask;
   else
      task = wd->wd_UserPort->mp_SigTask;

   /*
    *  Add ID node.
   */
   if (w = wd->wd_WindowPtr)
   {
      rc = AddIDReport(w, wmri->wmri_ID, task);
      /*
       *  Double-click?
       */
      if (wmri->wmri_Flags & WMRIF_DOUBLE_CLICK)
      {
	 rc = AddIDReport(w, wmri->wmri_ID, task);
      };
      /*
       *  Set signal.
       */
      if (task) Signal(task, (1 << wd->wd_UserPort->mp_SigBit));
   }
   return rc;
}
METHOD_END
///
/*
 * Get a pointer to the window that signalled us.
 */
METHOD(WindowClassGetSigWin, Msg, msg )
{
   WD       *wd = ( WD * )INST_DATA( cl, obj );
   struct Window     *win;

   /*
    * Shared message port?
    */
   if ( wd->wd_Flags & WDF_SHARED_MSGPORT ) {
      /*
       * Any messages at the port?
       */
      if ( wd->wd_UserPort->mp_MsgList.lh_Head->ln_Succ )
	 /*
	  * Yes. Get the first.
	  */
	 win = (( struct IntuiMessage * )wd->wd_UserPort->mp_MsgList.lh_Head )->IDCMPWindow;
      else
	 /*
	  * Try for the first reported ID.
	  */
	 win = GetFirstIDReportWindow();
   } else
      /*
       * This window.
       */
      win = wd->wd_WindowPtr;

   return(( ULONG )win );
}
METHOD_END

/*
 * Remove an object from the tab-cycle list.
 */
METHOD(WindowClassRemove, struct wmRemoveObject *, wmro )
{
   WD          *wd = ( WD * )INST_DATA( cl, obj );
   TABCYCLE       *cyc;

   if ( wmro->wmro_Object ) {
      if ( wmro->wmro_Flags & WMROF_CYCLE_LIST ) {
	 if ( cyc = GetCycleNode( wd, wmro->wmro_Object )) {
	    Remove(( struct Node * )cyc );
	    BGUI_FreePoolMem( cyc );
	 }
      }
      return( TRUE );
   }
   return( FALSE );
}
METHOD_END

/// WM_SECURE
/*
 * Secure the master gadget by removing it.
 */
METHOD(WindowClassSecure, Msg, msg)
{
   WD            *wd = INST_DATA(cl, obj);
   ULONG          rc = 0;
   struct Window *w;

   /*
    * Window Open?
    */
   if (w = wd->wd_WindowPtr)
   {
      /*
       * Yes. Master gadget attached?
       */
      if (!(wd->wd_Flags & WDF_REMOVED))
      {
	 /*
	  * No. Remove it now.
	  */
	 wd->wd_Flags |= WDF_REMOVED;
	 RemoveGadget(w, (struct Gadget *)wd->wd_Gadgets);
      }
      rc = 1;
   };
   return rc;
}
METHOD_END
///
/// WM_RELEASE
/*
 * Release the master gadget by adding it.
 */
METHOD(WindowClassRelease, Msg, msg)
{
   WD            *wd = INST_DATA(cl, obj);
   ULONG          rc = 0;
   struct Window *w;

WW(kprintf("WindowClassRelease:\n"));

   /*
    * Window Open?
    */
   if (w = wd->wd_WindowPtr)
   {
WW(kprintf("WindowClassRelease: window is open\n"));
      /*
       * Yes. Master gadget removed?
       */
      if (wd->wd_Flags & WDF_REMOVED)
      {
WW(kprintf("WindowClassRelease: WDF_REMOVED set = master gadget was removed. Calling AddGadget\n"));
	 /*
	  * Put the master back on-line.
	  */
	 AddGadget(w, (struct Gadget *)wd->wd_Gadgets, -1);
WW(kprintf("WindowClassRelease: clearing WDF_REMOVED flag\n"));

	 wd->wd_Flags &= ~WDF_REMOVED;

      };
WW(kprintf("WindowClassRelease: calling RefreshGList: gad = %x\n", (struct Gadget *)wd->wd_Gadgets));

#warning temp AROS fix to fix refresh problem after an automatic window-resize done by BGUI
#ifdef __AROS__
RefreshGList((struct Gadget *)wd->wd_Gadgets, w, 0, 1);
#endif

      rc = 1;
   };
   return rc;
}
METHOD_END
///
/// WM_RELAYOUT
/*
 * Relayout the GUI.
 */
METHOD(WindowClassRelayout, Msg, msg)
{
   WD             *wd = INST_DATA(cl, obj);
   WORD            newl, newt, neww, newh;
   UWORD           min_x, min_y, max_x, max_y;
   BOOL            lock_w = wd->wd_Flags & WDF_LOCK_WIDTH;
   BOOL            lock_h = wd->wd_Flags & WDF_LOCK_HEIGHT;
   struct Window  *w;
   struct Screen  *s = wd->wd_Screen;

WW(kprintf("** WindowClassRelayout\n"));
   /*
    * Window open?
    */
   if (w = wd->wd_WindowPtr)
   {
WW(kprintf("** WindowClassRelayout: window is open. calling WM_SECURE\n"));
      AsmDoMethod(obj, WM_SECURE);
WW(kprintf("** WindowClassRelayout: calling WinSize\n"));

      if (!WinSize(wd, &min_x, &min_y))
      {
	 AsmDoMethod(obj, WM_RELEASE);
	 return 0;
      };
WW(kprintf("** WindowClassRelayout: WinSize call ok min_x = %d, min_y = %d\n", min_x, min_y));

      /*
       * Pick up old minimum width and height.
       */
      max_x = w->MaxWidth;
      max_y = w->MaxHeight;

      if (!(wd->wd_WindowFlags & WFLG_BACKDROP))
      {
	 /*
	  * Default window position/size.
	  */
	 newl = w->LeftEdge;
	 newt = w->TopEdge;
	 neww = w->Width;
	 newh = w->Height;

	 if (lock_w || (min_x > neww)) neww = min_x;
	 if (lock_h || (min_y > newh)) newh = min_y;

	 /*
	  * Find out how much we have to
	  * move the window.
	  */
	 if ((newl + neww) > s->Width)  newl = s->Width  - neww;
	 if ((newt + newh) > s->Height) newt = s->Height - newh;

	 /*
	  * Compute the new maximum values.
	  */
	 if (neww > max_x) max_x = neww;
	 if (newh > max_y) max_y = newh;

	 /*
	  * Re-size the window accoording to the master
	  * minimum size change.
	  */

	 WindowLimits(w, 1, 1, -1, -1);

WW(kprintf("** WindowClassRelayout: Calling ChangeWindowBox(%d,%d,%d,%d)\n",newl,newt,neww,newh));

	 ChangeWindowBox(w, newl, newt, neww, newh);

	 wd->wd_MinW = lock_w ? neww : min_x;
	 wd->wd_MinH = lock_h ? newh : min_y;
	 wd->wd_MaxW = lock_w ? neww : max_x;
	 wd->wd_MaxH = lock_h ? newh : max_y;
WW(kprintf("** WindowClassRelayout: setting wd->wd_MinW/H to %d,%d  wd->wd_MaxW/H to %d,%d\n",
wd->wd_MinW,
wd->wd_MinH,
wd->wd_MaxW,
wd->wd_MaxH));

	 wd->wd_Flags |= WDF_CONSTRAINTS;
      }
      else
      {
	 AsmDoMethod(obj, WM_RELEASE);
      };
   };
   return 1;
}
METHOD_END
///
/// WM_WHICHOBJECT
/*
 * Find the object under the mouse.
 */
METHOD(WindowClassWhichObject, Msg, msg)
{
   WD            *wd = INST_DATA(cl, obj);
   ULONG          rc = 0;
   struct Window *w;
   WORD           mx, my;

   /*
    * Window Open?
    */
   if (w = wd->wd_WindowPtr)
   {
      /*
       * Pickup mouse coords.
       */
      mx = w->MouseX;
      my = w->MouseY;

      /*
       * On the window?
       */
      if ((mx >= 0) && (my >= 0) && (mx < w->Width) && (my < w->Height))
	 /*
	  * Let's see what the master returns to us.
	  */
	  
	 #ifdef __AROS__
	 rc = AsmDoMethod(wd->wd_Gadgets, GRM_WHICHOBJECT, mx, my);
	 #else
	 rc = AsmDoMethod(wd->wd_Gadgets, GRM_WHICHOBJECT, (mx << 16) | my);
	 #endif
   }
   return rc;
}
METHOD_END
///
/// WM_LOCK, WM_UNLOCK

METHOD(WindowClassLock, Msg, msg)
{
   WD       *wd = INST_DATA(cl, obj);

   /*
    * Lock or unlock?
    */
   if (msg->MethodID == WM_LOCK) wd->wd_Flags |=  WDF_LOCKOUT;
   else                          wd->wd_Flags &= ~WDF_LOCKOUT;

   return 1;
}
METHOD_END
///
/// BASE_FINDKEY
/*
 * Which object has the proper key?
 */
METHOD(WindowClassFindKey, struct bmFindKey *, bmfk)
{
   WD          *wd = INST_DATA(cl, obj);
   Object      *ob, *gr;

   if (gr = wd->wd_Gadgets)
   {
      if (ob = (Object *)AsmDoMethodA(gr, (Msg)bmfk))
	 return (ULONG)ob;
   };
   if (gr = wd->wd_LBorder)
   {
      if (ob = (Object *)AsmDoMethodA(gr, (Msg)bmfk))
	 return (ULONG)ob;
   };
   if (gr = wd->wd_RBorder)
   {
      if (ob = (Object *)AsmDoMethodA(gr, (Msg)bmfk))
	 return (ULONG)ob;
   };
   if (gr = wd->wd_TBorder)
   {
      if (ob = (Object *)AsmDoMethodA(gr, (Msg)bmfk))
	 return (ULONG)ob;
   };
   if (gr = wd->wd_BBorder)
   {
      if (ob = (Object *)AsmDoMethodA(gr, (Msg)bmfk))
	 return (ULONG)ob;
   };
   return NULL;
}
METHOD_END
///
/// BASE_KEYLABEL
/*
 * Attach label keys to object.
 */
METHOD(WindowClassKeyLabel, struct bmKeyLabel *, bmkl)
{
   WD          *wd = INST_DATA(cl, obj);
   Object      *gr;

   if (gr = wd->wd_Gadgets) AsmDoMethodA(gr, (Msg)bmkl);
   if (gr = wd->wd_LBorder) AsmDoMethodA(gr, (Msg)bmkl);
   if (gr = wd->wd_RBorder) AsmDoMethodA(gr, (Msg)bmkl);
   if (gr = wd->wd_TBorder) AsmDoMethodA(gr, (Msg)bmkl);
   if (gr = wd->wd_BBorder) AsmDoMethodA(gr, (Msg)bmkl);

   return NULL;
}
METHOD_END
///
/// BASE_LOCALIZE
/*
 * Localize window.
 */
METHOD(WindowClassLocalize, struct bmLocalize *, bml)
{
   WD          *wd = INST_DATA(cl, obj);
   Object      *gr;

   if (gr = wd->wd_Gadgets) AsmDoMethodA(gr, (Msg)bml);
   if (gr = wd->wd_LBorder) AsmDoMethodA(gr, (Msg)bml);
   if (gr = wd->wd_RBorder) AsmDoMethodA(gr, (Msg)bml);
   if (gr = wd->wd_TBorder) AsmDoMethodA(gr, (Msg)bml);
   if (gr = wd->wd_BBorder) AsmDoMethodA(gr, (Msg)bml);

   if (wd->wd_WindowTitleID) wd->wd_WindowTitle =
      BGUI_GetCatalogStr(bml->bml_Locale, wd->wd_WindowTitleID, wd->wd_WindowTitle);

   if (wd->wd_ScreenTitleID) wd->wd_ScreenTitle =
      BGUI_GetCatalogStr(bml->bml_Locale, wd->wd_ScreenTitleID, wd->wd_ScreenTitle);
   
   if (wd->wd_HelpTextID) wd->wd_HelpText =
      BGUI_GetCatalogStr(bml->bml_Locale, wd->wd_HelpTextID, wd->wd_HelpText);
   
   return NULL;
}
METHOD_END
///
/// WM_CLIP
/*
 * Turn on or off buffer layering.
 */
METHOD(WindowClassClip, struct wmClip *, wmc)
{
   WD                *wd = INST_DATA(cl, obj);
   struct RastPort   *br = wd->wd_BufferRP;
   struct Rectangle  *cr = wmc->wmc_Rectangle;
   struct Region     *r  = wd->wd_Region;
   struct Layer      *wl;
   ULONG              rc = 0;

   if (wd->wd_WindowPtr && (wl = wd->wd_WindowPtr->RPort->Layer))
   {
      InstallClipRegion(wl, NULL);

      switch (wmc->wmc_Action)
      {
      case CLIPACTION_NONE:
	 if (br && br->Layer)
	 {
	    wd->wd_BufferLayer = br->Layer;
	    br->Layer = NULL;
	 };
	 return 1;
      case CLIPACTION_CLEAR:
	 ClearRegion(r);
	 rc = 1;
	 break;
      case CLIPACTION_OR:
	 rc = OrRectRegion(r, cr);
	 break;
      case CLIPACTION_AND:
	 AndRectRegion(r, cr);
	 rc = 1;
	 break;
      };

      if (rc)
      {
	 InstallClipRegion(wl, r);

	 if (br)
	 {
	    if (!br->Layer) br->Layer = wd->wd_BufferLayer;
	    InstallClipRegion(br->Layer, r);
	 };
      };
   };
   return rc;
}
METHOD_END

METHOD(WindowClassSetupGadget, struct wmSetupGadget *, wmsg)
{
   WD *wd = INST_DATA(cl, obj);
   struct TagItem  *tstate = wmsg->wmsg_Tags, *tag;

   while (tag = NextTagItem(&tstate))
   {
      switch(tag->ti_Tag)
      {
	 case BT_Inhibit:
	    AsmDoMethod(wmsg->wmsg_Object, BASE_INHIBIT, tag->ti_Tag);
	    break;
      }
   }

   if(wd->wd_UsedFont
   || ((wd->wd_Flags & WDF_AUTOASPECT)
   && wd->wd_DrawInfo))
   {
      struct TextAttr current_font;

      if(wd->wd_UsedFont)
      {
	 struct RastPort rp;

	 InitRastPort(&rp);
	 SetFont(&rp,wd->wd_UsedFont);
	 AskFont(&rp,&current_font);
      }

      SetAttrs(wmsg->wmsg_Object,
	 wd->wd_UsedFont ? BT_TextAttr : TAG_IGNORE,&current_font,
	 ((wd->wd_Flags & WDF_AUTOASPECT) && wd->wd_DrawInfo) ? FRM_ThinFrame : TAG_IGNORE, ((wd->wd_DrawInfo->dri_Resolution.X / wd->wd_DrawInfo->dri_Resolution.Y) <= 1),
	 TAG_END);
   }
   return(1);
}
METHOD_END

///
/// Class initialization.

/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,                 (FUNCPTR)WindowClassNew,
   OM_SET,                 (FUNCPTR)WindowClassSetUpdate,
   OM_UPDATE,              (FUNCPTR)WindowClassSetUpdate,
   OM_GET,                 (FUNCPTR)WindowClassGet,
   OM_DISPOSE,             (FUNCPTR)WindowClassDispose,
   WM_OPEN,                (FUNCPTR)WindowClassOpen,
   WM_CLOSE,               (FUNCPTR)WindowClassClose,
   WM_SLEEP,               (FUNCPTR)WindowClassSleep,
   WM_WAKEUP,              (FUNCPTR)WindowClassWakeUp,
   WM_HANDLEIDCMP,         (FUNCPTR)WindowClassIDCMP,
   WM_GADGETKEY,           (FUNCPTR)WindowClassGadgetKey,
   WM_DISABLEMENU,         (FUNCPTR)WindowClassDisableMenu,
   WM_CHECKITEM,           (FUNCPTR)WindowClassCheckItem,
   WM_MENUDISABLED,        (FUNCPTR)WindowClassMenuDisabled,
   WM_ITEMCHECKED,         (FUNCPTR)WindowClassItemChecked,
   WM_TABCYCLE_ORDER,      (FUNCPTR)WindowClassCycleOrder,
   WM_GETAPPMSG,           (FUNCPTR)WindowClassGetAppMsg,
   WM_ADDUPDATE,           (FUNCPTR)WindowClassAddUpdate,
   WM_REPORT_ID,           (FUNCPTR)WindowClassReportID,
   WM_GET_SIGNAL_WINDOW,   (FUNCPTR)WindowClassGetSigWin,
   WM_REMOVE_OBJECT,       (FUNCPTR)WindowClassRemove,

   WM_SECURE,              (FUNCPTR)WindowClassSecure,
   WM_RELEASE,             (FUNCPTR)WindowClassRelease,
   WM_RELAYOUT,            (FUNCPTR)WindowClassRelayout,
   WM_WHICHOBJECT,         (FUNCPTR)WindowClassWhichObject,
   WM_LOCK,                (FUNCPTR)WindowClassLock,
   WM_UNLOCK,              (FUNCPTR)WindowClassLock,
   WM_CLOSETOOLTIP,        (FUNCPTR)WindowClassCloseTT,
   WM_CLIP,                (FUNCPTR)WindowClassClip,
   WM_SETUPGADGET,         (FUNCPTR)WindowClassSetupGadget,

   BASE_FINDKEY,           (FUNCPTR)WindowClassFindKey,
   BASE_KEYLABEL,          (FUNCPTR)WindowClassKeyLabel,
   BASE_LOCALIZE,          (FUNCPTR)WindowClassLocalize,

   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitWindowClass(void)
{
   return WindowClass = BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_ROOT_OBJECT,
				       CLASS_ObjectSize,     sizeof(WD),
				       CLASS_DFTable,        ClassFunc,
				       TAG_DONE);
}
///
