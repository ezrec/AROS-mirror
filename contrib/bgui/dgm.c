/*
 * @(#) $Header$
 *
 * BGUI library
 * dgm.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.6  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.5  2003/01/18 19:09:56  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.4  2000/08/09 11:45:57  chodorowski
 * Removed a lot of #ifdefs that disabled the AROS_LIB* macros when not building on AROS. This is now handled in contrib/bgui/include/bgui_compilerspecific.h.
 *
 * Revision 42.3  2000/05/29 00:40:23  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.2  2000/05/15 19:27:01  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:47  stegerg
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
 * Revision 42.0  2000/05/09 22:08:48  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:10  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:11:55  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:03  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.
#include "include/classdefs.h"

typedef struct {
   Object            *dd_Target;       /* Target object. */
   struct Window     *dd_Window;       /* Target window. */
   struct InputEvent  ie;
}  DD;
///
/// OM_SET
METHOD(DGMClassSet, struct opSet *, ops)
{
   DD                  *dd = INST_DATA(cl, obj);
   struct TagItem      *tag, *tstate = ops->ops_AttrList;
   struct IntuiMessage *imsg;
   ULONG               *ptr = NULL, *res;
   BOOL                 domethod = FALSE;

   /*
    * Scan tags.
    */
   while (tag = NextTagItem(&tstate))
   {
      switch (tag->ti_Tag)
      {
	 case DGM_Result:
	    /*
	     * Pickup result storage. We return the result
	     * in a pointer passed to us to avoid the system
	     * glitch (or is it a real bug?).
	     */
	    res = ( ULONG * )tag->ti_Data;
	    break;

	 case DGM_Object:
	    /*
	     * Pickup target object.
	     */
	    dd->dd_Target = (Object *)tag->ti_Data;
	    break;
	    
	 case DGM_IntuiMsg:
	    if (imsg = (struct IntuiMessage *)(tag->ti_Data))
	    {
	       dd->dd_Window           = imsg->IDCMPWindow;
	       
	       dd->ie.ie_NextEvent     = NULL;
	       dd->ie.ie_Class         = IECLASS_RAWMOUSE;
	       dd->ie.ie_Code          = imsg->Code;
	       dd->ie.ie_Qualifier     = imsg->Qualifier;
	       dd->ie.ie_X             = imsg->MouseX;
	       dd->ie.ie_Y             = imsg->MouseY;
	    }
	    break;
	 
	 case DGM_DoMethod:
	    domethod = TRUE;

	    /*
	     * Get a pointer to the message.
	     */
	    ptr = (ULONG *)tag->ti_Data;

	    /*
	     * What kind of method?
	     */
	    switch (*ptr)
	    {
	       case OM_NEW:
	       case OM_SET:
	       case OM_UPDATE:
	       case OM_NOTIFY:
		  /*
		   * These get the GadgetInfo as the
		   * third long-word.
		   */
		  ptr[2] = (ULONG)ops->ops_GInfo;
		  break;

	       default:
		  /*
		   * All the rest get it at the
		   * second long-word.
		   */
		  ptr[1] = (ULONG)ops->ops_GInfo;
		  break;
	    };
	    break;
      };
   };

   /*
    * Blast off method.
    */
   if (domethod)
   {
      if (dd->dd_Target)
      {
	 *res = (ULONG)AsmDoMethodA(dd->dd_Target, (Msg)ptr);
	 /*
	  * Return result for compatibility reasons.
	  */
      };
      return *res;
   }
   return 1;
}
METHOD_END
///
/// BGUI_DoGadgetMethod(obj, win, req)
/*
 * Emulate the OS 3.0 DoGadgetMethod call.
 */
makeproto ULONG myDoGadgetMethod(Object *obj, struct Window *win, struct Requester *req, ULONG methodid, ... )
{
   return BGUI_DoGadgetMethodA(obj, win, req, (Msg)&methodid);
}

/*
 * Emulate the intuition DoGadgetMethod() call.
 */
#ifdef __AROS__
//makearosproto
AROS_LH4(ULONG, BGUI_DoGadgetMethodA,
    AROS_LHA(Object *, obj, A0),
    AROS_LHA(struct Window *, win, A1),
    AROS_LHA(struct Requester *, req, A2),
    AROS_LHA(Msg, msg, A3),
    struct Library *, BGUIBase, 11, BGUI)
#else
makeproto SAVEDS ASM ULONG BGUI_DoGadgetMethodA( REG(a0) Object *obj, REG(a1) struct Window *win, REG(a2) struct Requester *req, REG(a3) Msg msg )
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   Object      *dgm;
   ULONG        rc;

   /*
    * Valid window?
    */
   if (win)
   {
      /*
       * Create DGMClass object.
       */
      if (dgm = BGUI_NewObjectA(BGUI_DGM_OBJECT, NULL))
      {
	 /*
	  * Send of method to the target. We simple pass the DGMClass
	  * a pointer to the result storage, the target object and the
	  * method to send and the class takes care of the rest :)
	  */
	 SetGadgetAttrs((struct Gadget *)dgm, win, req, DGM_Result, &rc, DGM_Object, obj, DGM_DoMethod, msg, TAG_END);

	 /*
	  * Dispose of the object.
	  */
	 AsmDoMethod(dgm, OM_DISPOSE);

	 /*
	  * Return result.
	  */
	 return rc;
      }
   }

   /*
    * Fall back to the AsmDoMethod() call if
    * an important argument is missing.
    */
   return AsmDoMethodA(obj, msg);

   AROS_LIBFUNC_EXIT
}
///
/// GM_GOACTIVE, GM_HANDLEINPUT
METHOD(DGMClassGoActive, struct gpInput *, gpi2)
{
   DD             *dd = INST_DATA(cl, obj);
   struct gpInput  gpi = *gpi2;
   struct IBox    *bounds = NULL;
   ULONG           rc = 0;
   Object         *target = dd->dd_Target;

   if (target)
   {
      if (gpi.MethodID == GM_GOACTIVE)
	 gpi.gpi_IEvent = &dd->ie;

      Get_Attr(target, BT_HitBox, &bounds);
      if (bounds)
      {
	 gpi.gpi_Mouse.X = gpi.gpi_GInfo->gi_Window->MouseX - bounds->Left;
	 gpi.gpi_Mouse.Y = gpi.gpi_GInfo->gi_Window->MouseY - bounds->Top;
      };

      /*
       * Forward the message to the target.
       */
      SetGadgetAttrs((struct Gadget *)obj, dd->dd_Window, NULL,
	 DGM_Result, &rc, DGM_Object, target, DGM_DoMethod, &gpi, TAG_END);
   };
   return rc;
}
METHOD_END
///
/// GM_GOINACTIVE
METHOD(DGMClassGoInactive, Msg, msg)
{
   DD          *dd   = INST_DATA(cl, obj);
   ULONG        rc   = 0, id, mouseact = 0, report = FALSE;
   Object      *win  = NULL, *target;

   if (target = dd->dd_Target)
   {
      Get_Attr(target, BT_ReportID, &report);
      Get_Attr(target, BT_ParentWindow, (ULONG *)&win);
      Get_Attr(target, BT_MouseActivation, &mouseact);
      /*
       * Forward the message to the target.
       */
      SetGadgetAttrs((struct Gadget *)obj, dd->dd_Window, NULL,
	 DGM_Result, &rc, DGM_Object, target, DGM_DoMethod, msg, TAG_END);
      
      if (win && report)
      {
	 id = GADGET(target)->GadgetID;
	 if (dd->ie.ie_Code == IECODE_RBUTTON)
	 {
	    AsmDoMethod(win, WM_REPORT_ID, id | WMHI_RMB, 0);
	 };
	 if (dd->ie.ie_Code == IECODE_MBUTTON)
	 {
	    AsmDoMethod(win, WM_REPORT_ID, id | WMHI_MMB, 0);
	 };
      };
   };
   return rc;
}
METHOD_END
///
/// Class initialization.
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_SET,                 (FUNCPTR)DGMClassSet,
   GM_GOACTIVE,            (FUNCPTR)DGMClassGoActive,
   GM_HANDLEINPUT,         (FUNCPTR)DGMClassGoActive,
   GM_GOINACTIVE,          (FUNCPTR)DGMClassGoInactive,
   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitDGMClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassID, GadgetClass,
			 CLASS_ObjectSize,   sizeof(DD),
			 CLASS_DFTable,      ClassFunc,
			 TAG_DONE);
}
///

/*
 * I have created this class for two main reasons:
 *
 * 1) OS 2.04 compatibility. This way we _always_ get a system build
 *    GadgetInfo structure no matter what OS we run on since the
 *    SetGadgetAttrs() call has been available since V36 (OS 2.0)
 *    of the system.
 *
 * 2) I am having trouble with the OS 3.0 DoGadgetMethod() call which
 *    does not always returns the result of the called method. Don't
 *    ask why, it just doesn't.
 *
 * With the aid of this class I get compatibility from OS 2.04 and up
 * and I work around what seems to be an OS bug while doing it. The only
 * bad thing may be the (slight) overhead.
 */
