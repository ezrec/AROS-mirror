/*
 * @(#) $Header$
 *
 * BGUI library
 * request.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.5  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.4  2003/01/18 19:10:00  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.3  2000/08/09 11:45:57  chodorowski
 * Removed a lot of #ifdefs that disabled the AROS_LIB* macros when not building on AROS. This is now handled in contrib/bgui/include/bgui_compilerspecific.h.
 *
 * Revision 42.2  2000/05/15 19:27:02  stegerg
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
 * Revision 42.0  2000/05/09 22:10:03  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:59  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:12:59  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:37  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/*
 * IDCMPHook data.
 */
typedef struct {
   struct bguiRequest *ihd_Request;     /* Requester structure.         */
   Object             *ihd_ReturnObj;   /* Return key object.      */
   ULONG               ihd_MaxID;       /* Maximum gadget ID.      */
}  IHD;

/*
 * Parse a button label.
 */
//STATIC ASM UBYTE *GetButtonName( REG(a0) UBYTE *source, REG(a1) UBYTE *uc, REG(d0) ULONG uchar )
STATIC ASM REGFUNC3(UBYTE *, GetButtonName,
	REGPARAM(A0, UBYTE *, source),
	REGPARAM(A1, UBYTE *, uc),
	REGPARAM(D0, ULONG, uchar))
{
   uc[0] = uc[1] = 0;

   /*
    * Search for the end-of-string or
    * the seperator character.
    */
   while (*source && *source != '|')
   {
      /*
       * Look up the character
       * to underline.
       */
      if (*source == uchar)
      {
	 uc[0] = tolower(source[1]);
      };
      source++;
   };

   /*
    * When we reached a seperator we
    * truncate the gadget string and
    * return a pointer to the next
    * name.
    */
   if (*source == '|')
   {
      *source++ = 0;
      return source;
   };

   /*
    * The end, finito, ende, einde....
    */
   return NULL;
}
REGFUNC_END

/*
 * Create the button group.
 */
STATIC Object *CreateButtonGroup( UBYTE *gstring, ULONG xen, UBYTE uchar, ULONG *maxid, Object **retobj )
{
   Object         *master, *button;
   UBYTE       *ptr, underscore[ 2 ];
   UWORD        id = 2, style = FS_NORMAL;
   BOOL         sing = FALSE, got_def = FALSE;

   /*
    * Single gadget?
    */
   if (!strchr( gstring, '|'))
      sing = TRUE;

   /*
    * Create a horizontal master group.
    */
   if (master = BGUI_NewObject(BGUI_GROUP_GADGET, GROUP_Spacing, GRSPACE_WIDE, GROUP_EqualWidth, TRUE, TAG_END))
   {
      /*
       * Add a spacing object for single
       * gadget requesters.
       */
      if (sing)
      {
	 if (!AsmDoMethod(master, GRM_ADDSPACEMEMBER, DEFAULT_WEIGHT))
	    goto fail;
      }

      /*
       * Loop to get all the gadgets
       * names.
       */
      while (ptr = gstring)
      {
	 /*
	  * Get a pointer to the next
	  * gadget name.
	  */
	 gstring = GetButtonName( gstring, &underscore[ 0 ], uchar );

	 /*
	  * Default button?
	  */
	 if ((*ptr == '*') && (!got_def))
	 {
	    ptr++;
	    got_def = TRUE;
	    style = FSF_BOLD;
	 }

	 /*
	  * Create the button.
	  */
	 if (button = BGUI_NewObject(BGUI_BUTTON_GADGET, LAB_Label, ptr, LAB_Style, style, LAB_Underscore, uchar,
	    GA_ID, id++, BT_Key, underscore, xen ? FRM_Type : TAG_IGNORE, FRTYPE_XEN_BUTTON, TAG_DONE))
	 {
	    /*
	     * Pick this button if it's the first
	     * created or the default button.
	     */
	    if ((id == 3) || (style == FSF_BOLD))
	       *retobj = button;

	    /*
	     * Add it to the master group.
	     */
	    if ( ! AsmDoMethod( master, GRM_ADDMEMBER, button, LGO_FixMinHeight, TRUE, TAG_END ))
	       goto fail;

	 }
	 else
	 {
	    /*
	     * Button creation failed.
	     */
	    fail:
	    DisposeObject( master );
	    return NULL;
	 }
	 /*
	  * Reset style.
	  */
	 style = FS_NORMAL;
      }

      /*
       * Save the maximum gadget ID.
       */
      *maxid = id - 2;

      /*
       * Add a spacing object for single
       * gadget requesters.
       */
      if (sing)
      {
	 if (!AsmDoMethod(master, GRM_ADDSPACEMEMBER, DEFAULT_WEIGHT))
	    goto fail;
      }

      /*
       * The last (right most) button get's
       * 0 as it's ID to keep compatible with
       * intuition's EasyRequest().
       */
      DoSetMethod(button, NULL, GA_ID, 1, TAG_END);

      /*
       * Return the master group.
       */
      return master;
   }
   return NULL;
}

/*
 * Requester IDCMP hook.
 */
//STATIC SAVEDS ASM VOID ReqHookFunc(REG(a0) struct Hook *hook, REG(a2) Object *window, REG(a1) struct IntuiMessage *msg)
STATIC SAVEDS ASM REGFUNC3(VOID, ReqHookFunc,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, window),
	REGPARAM(A1, struct IntuiMessage *, msg))
{
   IHD      *ihd = (IHD *)hook->h_Data;
   int       id = 0;

   /*
    * Which key was pressed?
    */
   switch (msg->Code)
   {
   /*
    * Left-Amiga+V?
    */
   case 0x34:
      if (msg->Qualifier & IEQUALIFIER_LCOMMAND)
	 id = (ihd->ihd_MaxID == 1) ? 1 : 2;
      break;

   /*
    * Left-Amiga+B?
    */
   case 0x35:
      if (msg->Qualifier & IEQUALIFIER_LCOMMAND)
	 id = 1;
      break;

   /*
    * Return/Enter?
    */
   case 0x43:
   case 0x44:
      if (ihd->ihd_Request->br_Flags & BREQF_FAST_KEYS)
	 id = (ihd->ihd_MaxID == 1) ? 1 : GADGET(ihd->ihd_ReturnObj)->GadgetID;
      break;

   /*
    * Esc?
    */
   case 0x45:
      if (ihd->ihd_Request->br_Flags & BREQF_FAST_KEYS)
	 id = 1;
      break;
   }
   if (id) AsmDoMethod(window, WM_REPORT_ID, id, 0);
}
REGFUNC_END
static struct Hook ReqHook = { NULL, NULL, (HOOKFUNC)ReqHookFunc, NULL, NULL };

/*
 * Put up a BGUI requester.
 */
#ifdef __AROS__
makearosproto
AROS_LH3(ULONG, BGUI_RequestA,
    AROS_LHA(struct Window *, win, A0),
    AROS_LHA(struct bguiRequest *, es, A1),
    AROS_LHA(ULONG *, args, A2),
    struct Library *, BGUIBase, 7, BGUI)
#else
makeproto SAVEDS ASM ULONG BGUI_RequestA(REG(a0) struct Window *win, REG(a1) struct bguiRequest *es, REG(a2) ULONG *args)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   Object         *window;
   struct Screen  *screen;
   IHD             ihd;
   struct Window  *wptr;
   UBYTE          *gstr, *wdt;
   ULONG           id, maxid, signal, rc = ~0;
   APTR            rlock = NULL;
   BOOL            cw = FALSE, running = TRUE;

   /*
    * Center on the window?
    */
   if (( es->br_Flags & BREQF_CENTERWINDOW ) && win )
      cw = TRUE;

   /*
    * Get the screen.
    */
   screen = win ? win->WScreen : es->br_Screen;

   /*
    * We need these strings.
    */
   if (es->br_TextFormat && es->br_GadgetFormat)
   {
      /*
       * Allocate private copy of the gadget string.
       */
      if (gstr = (UBYTE *)BGUI_AllocPoolMem(strlen(es->br_GadgetFormat) + 1))
      {
	 /*
	  * Copy the gadget string.
	  */
	 strcpy(gstr, es->br_GadgetFormat);

	 /*
	  * Figure out requester title.
	  */
	 if (es->br_Title) wdt = es->br_Title;
	 else
	 {
	    if (win && win->Title) wdt = win->Title;
	    else
	    {
	       InitLocale();
	       wdt = LOCSTR(MSG_BGUI_REQUESTA_TITLE);
	    }
	 }

	 /*
	  * Create a window object.
	  */
	 window = WindowObject,
	    WINDOW_Position,        es->br_ReqPos,
	    cw ? WINDOW_PosRelBox : TAG_IGNORE, IBOX( &win->LeftEdge ),
	    WINDOW_Title,           wdt,
	    WINDOW_CloseGadget,     FALSE,
	    WINDOW_SizeGadget,      FALSE,
	    WINDOW_RMBTrap,         TRUE,
	    WINDOW_SmartRefresh,    TRUE,
	    WINDOW_Font,            es->br_TextAttr,
	    WINDOW_IDCMP,           IDCMP_RAWKEY,
	    WINDOW_IDCMPHookBits,   IDCMP_RAWKEY,
	    WINDOW_IDCMPHook,       &ReqHook,
	    WINDOW_NoBufferRP,      TRUE,
	    WINDOW_AutoAspect,      es->br_Flags & BREQF_AUTO_ASPECT ? TRUE : FALSE,
	    WINDOW_AutoKeyLabel,    TRUE,

	    screen ? WINDOW_Screen : TAG_IGNORE, screen,

	    WINDOW_MasterGroup,
	       VGroupObject, NormalHOffset, NormalVOffset, NormalSpacing,
		  es->br_Flags & BREQF_NO_PATTERN ? TAG_IGNORE : GROUP_BackFill, SHINE_RASTER,
		  StartMember,
		     InfoObject,
			FRM_Type,            FRTYPE_BUTTON,
			FRM_Recessed,        TRUE,
			INFO_TextFormat,     es->br_TextFormat,
			INFO_FixTextWidth,   TRUE,
			INFO_FixTextHeight,  TRUE,
			INFO_Args,           args,
			INFO_HorizOffset,    10,
			INFO_VertOffset,     6,
		     EndObject,
		  EndMember,
		  StartMember,
		     CreateButtonGroup(gstr, es->br_Flags & BREQF_XEN_BUTTONS, es->br_Underscore, &maxid, &ihd.ihd_ReturnObj),
		     LGO_FixMinHeight, TRUE, 
		  EndMember,
	       EndObject,
	    EndObject;

	 if (window)
	 {
	    /*
	     * Setup hook data.
	     */
	    ihd.ihd_Request = es;
	    ihd.ihd_MaxID  = maxid;
	    ReqHook.h_Data = ( APTR )&ihd;

	    /*
	     * Open the window and wait
	     * for a gadget click.
	     */
	    if (wptr = (struct Window *)AsmDoMethod(window, WM_OPEN))
	    {
	       /*
		* Lock parent window?
		*/
	       if ((es->br_Flags & BREQF_LOCKWINDOW) && win)
		  rlock = BGUI_LockWindow(win);

	       /*
		* Pick up signal bit.
		*/
	       Get_Attr(window, WINDOW_SigMask, &signal);

	       do
	       {
		  Wait(signal);
		  while((id = AsmDoMethod( window, WM_HANDLEIDCMP )) != WMHI_NOMORE)
		  {
		     /*
		      * Gadget clicked?
		      */
		     if (id >= 1 && id <= maxid)
		     {
			rc = id - 1;
			running = FALSE;
		     }
		  }
	       } while (running);
	       AsmDoMethod(window, WM_CLOSE);
	       BGUI_UnlockWindow(rlock);
	    }
	    DisposeObject(window);
	 }
	 BGUI_FreePoolMem(gstr);
      }
   }
   return rc;

   AROS_LIBFUNC_EXIT
}
