/*
 * @(#) $Header$
 *
 * BGUI library
 * baseclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.9  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.8  2003/01/18 19:09:55  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.7  2000/08/17 15:09:18  chodorowski
 * Fixed compiler warnings.
 *
 * Revision 42.6  2000/08/10 17:52:47  stegerg
 * temp fix for relayout refresh bug which only happens in AROS. temp. solved
 * by doing a RefreshGList in windowclass.c/WindowClassRelease method.
 *
 * Revision 42.5  2000/08/09 11:45:57  chodorowski
 * Removed a lot of #ifdefs that disabled the AROS_LIB* macros when not building on AROS. This is now handled in contrib/bgui/include/bgui_compilerspecific.h.
 *
 * Revision 42.4  2000/07/07 17:07:49  stegerg
 * fixed PACKW #define, which must be different on AROS, because
 * I changed all methods to use STACKULONG/STACKUWORD/??? types.
 *
 * Revision 42.3  2000/05/29 00:40:23  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.2  2000/05/15 19:27:00  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:46  stegerg
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
 * Revision 42.0  2000/05/09 22:08:19  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:53:49  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.15  1999/08/30 00:46:12  mlemos
 * Fixed rendering a object in BASE_RELAYOUT when the minimum dimensions did
 * not change.
 *
 * Revision 41.10.2.14  1999/08/30 00:20:01  mlemos
 * Assured that the object is not rendered in BASE_RELAYOUT if it is inhibited.
 *
 * Revision 41.10.2.13  1999/08/29 20:21:55  mlemos
 * Renamed the GRM_RELAYOUT to BASE_RELAYOUT.
 *
 * Revision 41.10.2.12  1999/08/01 04:15:28  mlemos
 * Avoided trying to copy invisible rendered portions of an object in a view.
 *
 * Revision 41.10.2.11  1999/07/26 22:17:33  mlemos
 * Re-enabled drag token conversion into a window while stopped.
 *
 * Revision 41.10.2.10  1999/07/03 15:17:31  mlemos
 * Replaced the calls to CallHookPkt to BGUI_CallHookPkt.
 *
 * Revision 41.10.2.9  1998/12/08 03:28:20  mlemos
 * Ensured that whenever the gadget border activation flags are changed the
 * the IMAGE_Border attribute of the frame and the label will reflect the
 * current gadget position.
 * Corrected the choice of background color for erasing the area of a gadget
 * in the border without any parents with frames.
 *
 * Revision 41.10.2.8  1998/12/07 14:46:15  mlemos
 * Ensured that any previously opened font is closed when setting BT_TextAttr.
 *
 * Revision 41.10.2.7  1998/12/07 03:36:59  mlemos
 * Fixed potential text font leak.
 *
 * Revision 41.10.2.6  1998/12/07 03:06:54  mlemos
 * Replaced OpenFont and CloseFont calls by the respective BGUI debug macros.
 *
 * Revision 41.10.2.5  1998/11/13 16:44:18  mlemos
 * Fixed missing Rastport assignment in GM_RENDER that was preventing view
 * class to work.
 *
 * Revision 41.10.2.4  1998/07/05 19:34:11  mlemos
 * Made calls to AllocBaseInfo in BaseClassDragActive and BaseClassDragUpdate
 * ensure that the rastport is properly obtained.
 *
 * Revision 41.10.2.3  1998/03/01 17:26:15  mlemos
 * Fixed BaseInfo memory leak.
 *
 * Revision 41.10.2.2  1998/03/01 15:36:46  mlemos
 * Removed tracing debugging statements.
 *
 * Revision 41.10.2.1  1998/03/01 15:33:31  mlemos
 * Added support to track BaseInfo memory leaks.
 *
 * Revision 41.10  1998/02/25 21:11:33  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:27  mlemos
 * Ian sources
 *
 *
 */

#define WW(x)

/* Get the prototype for kprintf() */

#ifdef __AROS__
#include <clib/arossupport_protos.h>
#else
#include <clib/debug_protos.h>
#endif

/// Class definitions.

#include "include/classdefs.h"

#define bd_Flags      bd_BC.bc_Flags         /* See below.                       */
#define bd_OuterBox   bd_BC.bc_OuterBox      /* Domain of object.                */
#define bd_InnerBox   bd_BC.bc_InnerBox      /* Internal area.                   */
#define bd_HitBox     bd_BC.bc_HitBox        /* Object hitbox.                   */
#define bd_Extentions bd_BC.bc_Extensions    /* Label extensions.                */
#define bd_Frame      bd_BC.bc_Frame         /* Frame for this object.           */
#define bd_Label      bd_BC.bc_Label         /* Label for this object.           */
#define bd_Parent     bd_BC.bc_Window        /* Parent window object.            */
#define bd_View       bd_BC.bc_View          /* Parent view object.              */
#define bd_RPort      bd_BC.bc_RPort         /* RastPort that is rendered to.    */
#define bd_TextAttr   bd_BC.bc_TextAttr      /* Font to use.                     */
#define bd_TextFont   bd_BC.bc_TextFont      /* Font to use (opened).            */
#define bd_Group      bd_BC.bc_Group
#define bd_Window     bd_BC.bc_Window
#define bd_Member     bd_BC.bc_GroupNode     /* Associated group member data.    */

/*
 * Object instance data.
 */
typedef struct bd_ {
   BC                 bd_BC;                 /* Shared data.                     */
   struct Hook       *bd_HelpHook;           /* Hook for help function.          */
   UBYTE             *bd_HelpFile;           /* File name for help.              */
   UBYTE             *bd_HelpNode;           /* Node name for AmigaGuide file.   */
   ULONG              bd_HelpLine;           /* Line number in help file.        */
   UBYTE             *bd_HelpText;           /* Text for requester help.         */
   LONG               bd_HelpTextID;         /* Text for requester help.         */
   UBYTE             *bd_ToolTip;            /* ToolTip text.                    */
   LONG               bd_ToolTipID;          /* ToolTip text ID.                 */
   struct RastPort   *bd_PreBuffer;          /* Render beforehand for later use. */
   BMO               *bd_BMO;                /* Drag bitmap information.         */
   UWORD              bd_Thresh;             /* Drag Threshold.                   */
   UWORD              bd_DragQual;           /* Drag qualifier.                  */
   struct Window     *bd_DragWindow;         /* Window of drag bitmap.           */
   UWORD              bd_MouseAct;           /* Mouse button activation.         */
   UWORD              bd_Qual;               /* Input qualifier.                 */
   UBYTE             *bd_Key, bd_RawKey;     /* Key equivalent.                  */
   WORD               bd_LeftOffset;         /* Left offset.                     */
   WORD               bd_RightOffset;        /* Right offset.                    */
   WORD               bd_TopOffset;          /* Top offset.                      */
   WORD               bd_BottomOffset;       /* Bottom offset.                   */
}  BD;

#define BDF_DRAGGABLE      (1 << 0)          /* The object can be dragged.       */
#define BDF_DROPPABLE      (1 << 1)          /* The object can be dropped on.    */
#define BDF_LABEL_CLICK    (1 << 2)          /* Count label-clicks as a hit.     */
#define BDF_REPORT_ID      (1 << 3)          /* Report ID if RMB/MMB             */
#define BDF_NORECESSED     (1 << 4)          /* Dont recess frame when selected. */
#define BDF_ISACTIVE       (1 << 5)          /* Object active?                   */
#define BDF_MOVING         (1 << 6)          /* We're moving...                  */
#define BDF_MOVE_DROPBOX   (1 << 10)         /* Standard moving dropbox          */
#define BDF_BUFFER         (1 << 11)         /* Buffer this object?              */
#define BDF_INHIBITED      (1 << 12)         /* Located on a invisible page.     */
#define BDF_GOT_LABEL_EXT  (1 << 13)         /* Got label extentions.            */
#define BDF_FIXED_FONT     (1 << 14)         /* Font set during create.          */
#define BDF_CACHE_NOTIFY   (1 << 15)         /* Notify upon window opening.      */

///

/// CalcDimensions

makeproto ULONG CalcDimensions(Class *cl, Object *obj, struct bmDimensions *bmd, UWORD mx, UWORD my)
{
   BC       *bc = BASE_DATA(obj);
   ULONG     rc;

   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)bmd))
   {
      bmd->bmd_Extent->be_Min.Width  += mx;
      bmd->bmd_Extent->be_Min.Height += my;

      bmd->bmd_Extent->be_Nom.Width  += mx;
      bmd->bmd_Extent->be_Nom.Height += my;
   };
   return rc;
}
///

#ifdef __AROS__
makearosproto
AROS_LH3(VOID, BGUI_PostRender,
    AROS_LHA(Class *, cl, A0),
    AROS_LHA(Object *, obj, A2),
    AROS_LHA(struct gpRender *, gpr, A1),
    struct Library *, BGUIBase, 23, BGUI)
#else
makeproto SAVEDS ASM VOID BGUI_PostRender(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpRender *gpr)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   AROS_LIBFUNC_EXIT
}

/// RM_REFRESH
/*
 * Refresh after a set.
 */
METHOD(BaseClassRefresh, struct rmRefresh *, rr)
{
   ULONG rc = rr->rr_Flags;

   if (rc & (RAF_RESIZE|RAF_REDRAW))
   {
      DoRenderMethod(obj, (struct GadgetInfo *)rr->rr_Context, GREDRAW_REDRAW);

      rc &= ~(RAF_RESIZE|RAF_REDRAW);
   };
   return rc;
}
METHOD_END
///
/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(BaseClassNew, struct opSet *, ops)
{
   BD                *bd;
   ULONG              rc;
   struct TagItem    *tags = ops->ops_AttrList;

   /*
    * We let the superclass setup an object
    * for us.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      /*
       * Get the instance data.
       */
      bd = INST_DATA(cl, rc);

      /*
       * Try to create a label
       * and a frame.
       */
      bd->bd_DragQual   = (UWORD)~0;
      bd->bd_RawKey     = (UBYTE)~0;
      bd->bd_Qual       = (UWORD)~0;
      bd->bd_Thresh     = 3;

      AsmCoerceMethod(cl, (Object *)rc, RM_SETM, ops->ops_AttrList, RAF_INITIAL);
   }
   return rc;
}
METHOD_END
///
/// RM_GET
/*
 * Get an attribute.
 */
METHOD(BaseClassGet, struct rmAttr *, ra)
{
   BD       *bd = (BD *)INST_DATA(cl, obj);
   ULONG     rc;

   rc = BGUI_GetAttrChart(cl, obj, ra);

   return rc;
}
METHOD_END
///
/// RM_GETCUSTOM
/*
 * Get custom attributes.
 */
METHOD(BaseClassGetCustom, struct rmAttr *, ra)
{
   BD               *bd = (BD *)INST_DATA(cl, obj);
   ULONG             attr  = ra->ra_Attr->ti_Tag;
   ULONG            *store = (ULONG *)ra->ra_Attr->ti_Data;

   switch (attr)
   {
   case BT_ToolTip:
      if (bd->bd_Flags & BDF_ISACTIVE)
      STORE NULL;
      break;
   };
   return 1;
}
METHOD_END
///
/// RM_GETATTRFLAGS
/*
 * Get the flags of an attribute.
 */
METHOD(BaseClassGetAttrFlags, struct rmAttr *, ra)
{
   static struct TagItem chart[] =
   {
      BT_LabelClick,       CHART_FLAG(bd_, bd_Flags, BDF_LABEL_CLICK),
      BT_NoRecessed,       CHART_FLAG(bd_, bd_Flags, BDF_NORECESSED),
      BT_DragObject,       CHART_FLAG(bd_, bd_Flags, BDF_DRAGGABLE),
      BT_DropObject,       CHART_FLAG(bd_, bd_Flags, BDF_DROPPABLE),
      BT_Buffer,           CHART_FLAG(bd_, bd_Flags, BDF_BUFFER),
      BT_IsActive,         CHART_FLAG(bd_, bd_Flags, BDF_ISACTIVE),
      BT_ReportID,         CHART_FLAG(bd_, bd_Flags, BDF_REPORT_ID),
      BT_Inhibit,          CHART_FLAG(bd_, bd_Flags, BDF_INHIBITED) | RAF_NOSET,

      BT_LabelObject,      CHART_ATTR(bd_, bd_Label)        | RAF_CUSTOM | RAF_NOSET,
      BT_FrameObject,      CHART_ATTR(bd_, bd_Frame)        | RAF_CUSTOM | RAF_NOSET,
      BT_TextAttr,         CHART_ATTR(bd_, bd_TextAttr)     | RAF_CUSTOM | RAF_NOSET,
      BT_TextFont,         CHART_ATTR(bd_, bd_TextFont)     | RAF_NOSET,
      BT_HelpHook,         CHART_ATTR(bd_, bd_HelpHook),
      BT_HelpFile,         CHART_ATTR(bd_, bd_HelpFile),
      BT_HelpNode,         CHART_ATTR(bd_, bd_HelpNode),
      BT_HelpLine,         CHART_ATTR(bd_, bd_HelpLine),
      BT_HelpText,         CHART_ATTR(bd_, bd_HelpText),
      BT_HelpTextID,       CHART_ATTR(bd_, bd_HelpTextID),
      BT_ToolTip,          CHART_ATTR(bd_, bd_ToolTip)      | RAF_CUSTOM,
      BT_ToolTipID,        CHART_ATTR(bd_, bd_ToolTipID),
      BT_DragThreshold,    CHART_ATTR(bd_, bd_Thresh),
      BT_DragQualifier,    CHART_ATTR(bd_, bd_DragQual),
      BT_MouseActivation,  CHART_ATTR(bd_, bd_MouseAct),
      BT_Key,              CHART_ATTR(bd_, bd_Key)          | RAF_CUSTOM,
      BT_RawKey,           CHART_ATTR(bd_, bd_RawKey)       | RAF_CUSTOM,
      BT_Qualifier,        CHART_ATTR(bd_, bd_Qual),
      BT_HitBox,           CHART_ATTR(bd_, bd_HitBox)       | RAF_ADDRESS,
      BT_OuterBox,         CHART_ATTR(bd_, bd_OuterBox)     | RAF_ADDRESS,
      BT_InnerBox,         CHART_ATTR(bd_, bd_InnerBox)     | RAF_ADDRESS,
      BT_LeftOffset,       CHART_ATTR(bd_, bd_LeftOffset)   | RAF_SIGNED,
      BT_RightOffset,      CHART_ATTR(bd_, bd_RightOffset)  | RAF_SIGNED,
      BT_TopOffset,        CHART_ATTR(bd_, bd_TopOffset)    | RAF_SIGNED,
      BT_BottomOffset,     CHART_ATTR(bd_, bd_BottomOffset) | RAF_SIGNED,

      BT_ParentWindow,     CHART_ATTR(bd_, bd_Window)       | RAF_CUSTOM,
      BT_ParentView,       CHART_ATTR(bd_, bd_View),
      BT_ParentGroup,      CHART_ATTR(bd_, bd_Group)        | RAF_CUSTOM,
      BT_GroupMember,      CHART_ATTR(bd_, bd_Member),

      GA_ID,               RAF_SUPER | RAF_NOUPDATE | RAF_NOP,

      TAG_DONE
   };
   
   ULONG rc = GetTagData(ra->ra_Attr->ti_Tag, 0, chart);

   return rc;
}
METHOD_END
///
/// RM_SET
/*
 * Set standard attributes.
 */
METHOD(BaseClassSet, struct rmAttr *, ra)
{
   BD               *bd;
   ULONG             attr, data, rc;

   rc = BGUI_SetAttrChart(cl, obj, ra);

   if (!(rc & RAF_UNDERSTOOD))
   {
      bd = (BD *)INST_DATA(cl, obj);
      attr = ra->ra_Attr->ti_Tag;
      data = ra->ra_Attr->ti_Data;

      if (!bd->bd_Label && LAB_TAG(attr))
      {
	 bd->bd_Label = BGUI_NewObject(BGUI_LABEL_IMAGE, IMAGE_InBorder, GADGET(obj)->Activation & BORDERMASK, TAG_DONE);
	 rc = RAF_RESIZE;
      };
      if (bd->bd_Label) rc |= AsmDoMethodA(bd->bd_Label, (Msg)ra);

      if (rc & RAF_UNDERSTOOD) return rc;

      if (LGO_TAG(attr))
      {
	 rc = RAF_UNDERSTOOD|RAF_RESIZE;

	 if (bd->bd_Member) DoSetMethodNG(bd->bd_Member, attr, data, TAG_DONE);
      }
      else if (FRM_TAG(attr))
      {
	 rc = RAF_UNDERSTOOD|RAF_REDRAW;

	 if (attr == FRM_Type) rc |= RAF_RESIZE;

	 if (bd->bd_Frame)
	 {
	    DoSetMethodNG(bd->bd_Frame, attr, data, TAG_DONE);
	 }
	 else
	 {
	    if (attr != FRM_ThinFrame)
	    {
	       bd->bd_Frame = BGUI_NewObject(BGUI_FRAME_IMAGE, attr, data, IMAGE_InBorder,
					     GADGET(obj)->Activation & BORDERMASK, TAG_DONE);
	       rc |= RAF_RESIZE;
	    };
	 };
      };
   }
   else
   {
      switch(ra->ra_Attr->ti_Tag)
      {
	 case GA_TopBorder:
	 case GA_BottomBorder:
	 case GA_LeftBorder:
	 case GA_RightBorder:
	    bd = (BD *)INST_DATA(cl, obj);
	    if (bd->bd_Frame)
	    {
	       DoSetMethodNG(bd->bd_Frame, IMAGE_InBorder, GADGET(obj)->Activation & BORDERMASK, TAG_END);
	       rc |= RAF_REDRAW;
	    };
	    if (bd->bd_Label)
	    {
	       DoSetMethodNG(bd->bd_Label, IMAGE_InBorder, GADGET(obj)->Activation & BORDERMASK, TAG_END);
	       rc |= RAF_REDRAW;
	    };
	    break;
      }
   }
   return rc;
}
METHOD_END
///
/// RM_SETCUSTOM
/*
 * Set custom attributes.
 */
METHOD(BaseClassSetCustom, struct rmAttr *, ra)
{
   BD               *bd = (BD *)INST_DATA(cl, obj);
   struct TextAttr  *ta;
   struct TextFont  *tf;
   ULONG             attr = ra->ra_Attr->ti_Tag;
   ULONG             data = ra->ra_Attr->ti_Data;

   switch (attr)
   {
   case BT_LabelObject:
      if (bd->bd_Label) DisposeObject(bd->bd_Label);
      if (bd->bd_Label = (Object *)data)
      {
	 bd->bd_Flags &= ~BDF_GOT_LABEL_EXT;
	 DoSetMethodNG(bd->bd_Label, IMAGE_InBorder, GADGET(obj)->Activation & BORDERMASK, TAG_END);
      };
      break;

   case BT_FrameObject:
      if (bd->bd_Frame) DisposeObject(bd->bd_Frame);
      if (bd->bd_Frame = (Object *)data)
      {
	 DoSetMethodNG(bd->bd_Frame, IMAGE_InBorder, GADGET(obj)->Activation & BORDERMASK, TAG_END);
      };
      break;

   case BT_TextAttr:
      /*
       * Only change the font if it was not defined yet and if it was
       * not set at create time.
       */
      if (bd->bd_Flags & BDF_FIXED_FONT)
	 break;

      /*
       * We only change the font when it opens ok.
       */
      if (ta = (struct TextAttr *)data)
      {
	 if (tf = BGUI_OpenFont(ta))
	 {
	    bd->bd_TextAttr = ta;
	    if(bd->bd_TextFont)
		BGUI_CloseFont(bd->bd_TextFont);
	    bd->bd_TextFont = tf;
	    bd->bd_Flags   &= ~BDF_GOT_LABEL_EXT;

	    if (ra->ra_Flags & RAF_INITIAL) bd->bd_Flags |= BDF_FIXED_FONT;

	    /*
	     * Change the font settings on the images.
	     */
	    if (bd->bd_Label) DoSetMethodNG(bd->bd_Label, IMAGE_TextFont, tf, TAG_END);
	    if (bd->bd_Frame) DoSetMethodNG(bd->bd_Frame, IMAGE_TextFont, tf, TAG_END);
	 };
      };
      break;

   case BT_Key:
      bd->bd_RawKey = (UBYTE)~0;
      break;

   case BT_RawKey:
      bd->bd_Key = NULL;
      break;

   case BT_ParentGroup:
      if (bd->bd_Frame) DoSetMethodNG(bd->bd_Frame, BT_ParentGroup, data, TAG_DONE);
      break;

   case BT_ParentWindow:
      if (bd->bd_Flags & BDF_CACHE_NOTIFY)
      {
	 //DoNotifyMethod(obj, ops->ops_GInfo, 0, GA_ID, GADGET(obj)->GadgetID, TAG_END);
	 bd->bd_Flags &= ~BDF_CACHE_NOTIFY;
      };
      break;
   };
   return 1;
}
METHOD_END
///
/// OM_NOTIFY
METHOD(BaseClassNotify, struct opUpdate *, opu)
{
   BD       *bd = INST_DATA(cl, obj);

   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   if (!opu->opu_GInfo || !opu->opu_GInfo->gi_Window)
   {
      bd->bd_Flags |= BDF_CACHE_NOTIFY;
   };

   return 1;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * They do not need us anymore.
 */
METHOD(BaseClassDispose, Msg, msg)
{
   BD       *bd = (BD *)INST_DATA(cl, obj);

   /*
    * Delete frame and label.
    */
   if (bd->bd_Frame)
      DisposeObject(bd->bd_Frame);
   if (bd->bd_Label)
      DisposeObject(bd->bd_Label);
   if (bd->bd_PreBuffer)
      BGUI_FreeRPortBitMap(bd->bd_PreBuffer);
   if (bd->bd_TextFont)
      BGUI_CloseFont(bd->bd_TextFont);

   /*
    * Let the superclass dispose
    * of us.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// GM_HITTEST
/*
 * Are we hit?
 */
METHOD(BaseClassHitTest, struct gpHitTest *, gpht)
{
   BD          *bd   = INST_DATA(cl, obj);
   struct IBox *ibox = (bd->bd_Flags & BDF_LABEL_CLICK) ? &bd->bd_OuterBox : &bd->bd_HitBox;

   /*
    * Get absolute click position.
    */
   WORD x = GADGET(obj)->LeftEdge + gpht->gpht_Mouse.X;
   WORD y = GADGET(obj)->TopEdge  + gpht->gpht_Mouse.Y;

   return (ULONG)(PointInBox(ibox, x, y) ? GMR_GADGETHIT : 0);
}
METHOD_END
///
/// GM_HELPTEST
/*
 * Are we hit?
 */
METHOD(BaseClassHelpTest, struct gpHitTest *, gpht)
{
   BD          *bd   = INST_DATA(cl, obj);
   struct IBox *ibox = &bd->bd_OuterBox;

   /*
    * Get absolute click position.
    */
   WORD x = GADGET(obj)->LeftEdge + gpht->gpht_Mouse.X;
   WORD y = GADGET(obj)->TopEdge  + gpht->gpht_Mouse.Y;

   return (ULONG)(PointInBox(ibox, x, y) ? GMR_HELPHIT : GMR_NOHELPHIT);
}
METHOD_END
///
/// GM_RENDER
/*
 * Render the object.
 */
METHOD(BaseClassRenderX, struct gpRender *, gpr)
{
   BD                *bd = INST_DATA(cl, obj);
   struct RastPort   *rp = NULL;
   struct BaseInfo   *bi;
   int                x, y, from_x, from_y, to_x, to_y;
   struct IBox       *box, to;
   ULONG              rc = 0, gok = TRUE;

   /*
    * Are we inhibited? If so we do not render ourselves.
    */
   if (bd->bd_Window)
      Get_Attr(bd->bd_Window, WINDOW_GadgetOK, &gok);

   if ((bd->bd_Flags & BDF_INHIBITED) || (!gok))
      return 0;

   if (bd->bd_Flags & BDF_BUFFER)
   {
      /*
       * Use buffer?
       */
      if (bd->bd_Window)
      {
	 Get_Attr(bd->bd_Window, WINDOW_BufferRP, (ULONG *)&rp);
      }
   };

   if (!rp) rp = gpr->gpr_RPort;

   /*
    * Handle virtual groups.
    */
   if (bd->bd_View)
   {
      Get_Attr(bd->bd_View, VIEW_ObjectBuffer, (ULONG *)&rp);
   };

   if (bd->bd_RPort = rp)
   {
#ifdef DEBUG_BGUI
      if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gpr->gpr_GInfo, BI_RastPort, rp, TAG_DONE))
#else
      if (bi = AllocBaseInfo(BI_GadgetInfo, gpr->gpr_GInfo, BI_RastPort, rp, TAG_DONE))
#endif
      {
	 bi->bi_Window = bd->bd_Parent;

	 /*
	  * Layout the object.
	  */
	 if (rc = AsmDoMethod(obj, BASE_LAYOUT, bi, GADGETBOX(obj), 0))
	 {
	    /*
	     * Remove layer from buffer if not needed.
	     */
	    //if (bd->bd_Parent) AsmDoMethod(bd->bd_Parent, WM_CLIP, NULL, CLIPACTION_NONE);

	    rc = AsmDoMethod(obj, BASE_RENDER, bi, gpr->gpr_Redraw);
	 };
	 FreeBaseInfo(bi);
      };
   };

   if (rc)
   {
      if (bd->bd_View)
      {
	 if (rp != gpr->gpr_RPort)
	 {
	    Get_Attr(bd->bd_View, VIEW_AbsoluteX, &x);
	    Get_Attr(bd->bd_View, VIEW_AbsoluteY, &y);

	    from_x = bd->bd_OuterBox.Left;
	    from_y = bd->bd_OuterBox.Top;

	    to.Left   = to_x = x + from_x;
	    to.Top    = to_y = y + from_y;
	    to.Width  = bd->bd_OuterBox.Width;
	    to.Height = bd->bd_OuterBox.Height;

	    /*
	     * Convert from ibox to rectangle.
	     */
	    to.Width  += to.Left - 1;
	    to.Height += to.Top  - 1;

	    AsmDoMethod(bd->bd_View, VIEW_CLIP, &to, TRUE);

	    /*
	     * Convert from rectangle to ibox.
	     */
	    to.Width  -= to.Left - 1;
	    to.Height -= to.Top  - 1;

	    /*
	     * Get clipping offsets.
	     */
	    from_x += to.Left - to_x;
	    from_y += to.Top  - to_y;

	    /*
	     * Copy the object to the window.
	     */
	    if(to.Width>0
	    && to.Height>0)
	       ClipBlit(rp, from_x, from_y, gpr->gpr_RPort, to.Left, to.Top, to.Width, to.Height, 0xC0);

	    //if (BASE_DATA(bd->bd_View)->bc_View)
	    //{
	    //   DoSetMethodNG(BASE_DATA(bd->bd_View)->bc_View, VIEW_ObjectBuffer, NULL, TAG_DONE);
	    //};
	 };
      }
      else
      {
	 if (rp && (rp != gpr->gpr_RPort))
	 {
	    /*
	     * Remove layer from buffer.
	     */
	    if (bd->bd_Window)
	    {
	       AsmDoMethod(bd->bd_Window, WM_CLIP, NULL, CLIPACTION_NONE);
	    };

	    box = (gpr->gpr_Redraw == GREDRAW_REDRAW) ? &bd->bd_OuterBox : &bd->bd_InnerBox;

	    /*
	     * Blast the GUI to the window.
	     */
	    if(box->Width>0
	    && box->Height>0)
	       ClipBlit(rp, box->Left, box->Top, gpr->gpr_RPort, box->Left, box->Top, box->Width, box->Height, 0xC0);
	 };
      };
   };
   return rc;
}
METHOD_END
///
/// BASE_LAYOUT
/*
 * Render the baseclass imagery.
 */
METHOD(BaseClassLayout, struct bmLayout *, bml)
{
   BD              *bd = INST_DATA(cl, obj);
   int              dw, dh;
   UWORD            gflags = GADGET(obj)->Flags;
   struct BaseInfo *bi = bml->bml_BInfo;

   /*
    * Get gadget bounds.
    */
   bd->bd_OuterBox = *(bml->bml_Bounds);

   /*
    * Don't bother with this if we are completely absolute.
    */
   if (gflags & (GFLG_RELRIGHT|GFLG_RELBOTTOM|GFLG_RELWIDTH|GFLG_RELHEIGHT))
   {
      /*
       * Get domain width and height.
       */
      dw = bi->bi_Domain.Width;
      dh = bi->bi_Domain.Height;

      /*
       * Adjust for any relativity.
       */
      if (gflags & GFLG_RELRIGHT)  bd->bd_OuterBox.Left   += dw - 1;
      if (gflags & GFLG_RELBOTTOM) bd->bd_OuterBox.Top    += dh - 1;
      if (gflags & GFLG_RELWIDTH)  bd->bd_OuterBox.Width  += dw;
      if (gflags & GFLG_RELHEIGHT) bd->bd_OuterBox.Height += dh;
   };

   bd->bd_HitBox = bd->bd_OuterBox;

   /*
    * Do we have a label?
    */
   if (bd->bd_Label && bi->bi_RPort)
   {
      /*
       * If the extentions are already known we
       * we do not bother getting them again.
       */
      if (!(bd->bd_Flags & BDF_GOT_LABEL_EXT))
      {
	 /*
	  * Setup label extentions.
	  */
	 DoExtentMethod(bd->bd_Label, bi->bi_RPort, &bd->bd_Extentions, NULL, NULL, EXTF_MAXIMUM);

	 /*
	  * We got the extentions.
	  */
	 bd->bd_Flags |= BDF_GOT_LABEL_EXT;
      };

      /*
       * Adjust hitbox accordingly.
       */
      bd->bd_HitBox.Left   += -bd->bd_Extentions.Left;
      bd->bd_HitBox.Top    += -bd->bd_Extentions.Top;
      bd->bd_HitBox.Width  -= -bd->bd_Extentions.Left + bd->bd_Extentions.Width;
      bd->bd_HitBox.Height -= -bd->bd_Extentions.Top  + bd->bd_Extentions.Height;

      /*
       * Setup label bounds.
       */
      SetImageBounds(bd->bd_Label, &bd->bd_HitBox);
   };

   bd->bd_InnerBox = bd->bd_HitBox;

   bd->bd_InnerBox.Left   += bd->bd_LeftOffset;
   bd->bd_InnerBox.Top    += bd->bd_TopOffset;
   bd->bd_InnerBox.Width  -= bd->bd_LeftOffset + bd->bd_RightOffset;
   bd->bd_InnerBox.Height -= bd->bd_TopOffset  + bd->bd_BottomOffset;

   /*
    * Setup frame bounds.
    */
   if (bd->bd_Frame)
   {
      AsmDoMethod(bd->bd_Frame, FRAMEM_SETUPBOUNDS, &bd->bd_HitBox, &bd->bd_InnerBox);
   };
   return 1;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the baseclass imagery.
 */
METHOD(BaseClassRender, struct bmRender *, bmr)
{
   BD                *bd = INST_DATA(cl, obj), *bd2;
   struct BaseInfo   *bi = bmr->bmr_BInfo;
   ULONG              state;
   Object            *parent, *frame;
   struct Rectangle   rect;

   /*
    * Complete redraw?
    */
   if (bmr->bmr_Flags == GREDRAW_REDRAW)
   {
      /*
       * What state do we render?
       */
      state = GadgetState(bi, obj, bd->bd_Flags & BDF_NORECESSED);

      if (bd->bd_Frame)
      {
	 BDrawImageState(bi, bd->bd_Frame, 0, 0, state);
      }
      else
      {
	 bd2 = bd;

	 while (parent = bd2->bd_Group)
	 {
	    bd2 = INST_DATA(cl, parent);
	    if (frame = bd2->bd_Frame)
	    {
	       /*
		* Use the frame for our backfill.
		*/
	       rect.MinX = bd->bd_InnerBox.Left;
	       rect.MinY = bd->bd_InnerBox.Top;
	       rect.MaxX = bd->bd_InnerBox.Width  + rect.MinX - 1;
	       rect.MaxY = bd->bd_InnerBox.Height + rect.MinY - 1;

	       AsmDoMethod(frame, FRAMEM_BACKFILL, bi, &rect, state);
	       break;
	    };
	 };

	 if (!parent)
	 {
	    UWORD apen;

	    /*
	     * If we do not have a frame, we need to clear the area ourselves.
	     */
	    switch (state)
	    {
	       case IDS_SELECTED:
		  apen = FILLPEN;
		  break;
	       case IDS_INACTIVESELECTED:
	       case IDS_INACTIVENORMAL:
		  apen = BACKGROUNDPEN;
		  break;
	       default:
		  /*
		   * And BACKGROUNDPEN for normal frames.
		   */
		  apen = (GADGET(obj)->Activation & BORDERMASK) ? FILLPEN : BACKGROUNDPEN;
		  break;
	    };
	    BSetDPenA(bi, apen);
	    BSetDrMd(bi, JAM1);
	    BClearAfPt(bi);
	    BBoxFillA(bi, &bd->bd_InnerBox);
	 };
      };

      /*
       * Render the label.
       */
      if (bd->bd_Label) BDrawImageState(bi, bd->bd_Label, 0, 0, state);

      /*
      if (bd->bd_Frame)
      {
	 rect.MinX = bd->bd_InnerBox.Left + 2;
	 rect.MinY = bd->bd_InnerBox.Top  + 2;
	 rect.MaxX = bd->bd_InnerBox.Width  + rect.MinX - 5;
	 rect.MaxY = bd->bd_InnerBox.Height + rect.MinY - 5;

	 BSetDrPt(bi, 0xAAAA);
	 BSetDrMd(bi, JAM1);
	 BSetDPenA(bi, SHADOWPEN);
	 Move(bi->bi_RPort, rect.MaxX, rect.MinY);
	 Draw(bi->bi_RPort, rect.MinX, rect.MinY);
	 Draw(bi->bi_RPort, rect.MinX, rect.MaxY);
	 Draw(bi->bi_RPort, rect.MaxX, rect.MaxY);
	 Draw(bi->bi_RPort, rect.MaxX, rect.MinY);
      };
      */
   };
   return 1;
}
METHOD_END
///
/// BASE_HELP
/*
 * Show help.
 */
METHOD(BaseClassHelp, struct bmShowHelp *, bsh)
{
   BD                   *bd = INST_DATA(cl, obj);
#ifdef __AROS__
#warning Commented the following line
#else
   struct NewAmigaGuide  nag = { NULL };
#endif
   ULONG                 rc = BMHELP_NOT_ME;
   struct gpHitTest      gph;

   /*
    * We only show help when
    * necessary or possible.
    */
   if (bd->bd_HelpFile || bd->bd_HelpText || bd->bd_HelpHook)
   {
#ifdef __AROS__
#warning A comment within a comment makes gcc puke...
#if 0
      /*
      WORD                  l, t, w, h, dw, dh;
      /*
       * Setup the gadget bounds.
       */
      l = GADGET(obj)->LeftEdge;
      t = GADGET(obj)->TopEdge;
      w = GADGET(obj)->Width;
      h = GADGET(obj)->Height;

      /*
       * When one of the GFLGREL... flags
       * is set adjust the appropiate value.
       */
      if (!(GADGET(obj)->GadgetType & GTYP_REQGADGET))
      {
	 dw = bsh->bsh_Window->Width;
	 dh = bsh->bsh_Window->Height;
      } else {
	 dw = bsh->bsh_Requester->Width;
	 dh = bsh->bsh_Requester->Height;
      }

      /*
       * Adjust coords.
       */
      if (GADGET(obj)->Flags & GFLG_RELRIGHT )  l += dw - 1;
      if (GADGET(obj)->Flags & GFLG_RELBOTTOM)  t += dh - 1;
      if (GADGET(obj)->Flags & GFLG_RELWIDTH )  w += dw;
      if (GADGET(obj)->Flags & GFLG_RELHEIGHT)  h += dh;

      /*
       * Mouse pointer in the gadget?
       */
      if (bsh->bsh_Mouse.X >= l && bsh->bsh_Mouse.X < (l + w) &&
	  bsh->bsh_Mouse.Y >= t && bsh->bsh_Mouse.Y < (t + h))
      */
#endif
#endif

      gph.MethodID   = GM_HELPTEST;
      gph.gpht_Mouse.X = bsh->bsh_Mouse.X - GADGET(obj)->LeftEdge;
      gph.gpht_Mouse.Y = bsh->bsh_Mouse.Y - GADGET(obj)->TopEdge;

      if (BGUI_DoGadgetMethodA(obj, bsh->bsh_Window, bsh->bsh_Requester, (Msg)&gph) == GMR_HELPHIT)
      {
	 if (bd->bd_HelpHook)
	 {  /*
	     * Help Hook ?
	     */
	    rc = BGUI_CallHookPkt(bd->bd_HelpHook, obj, NULL);
	 }
	 else if (bd->bd_HelpText)
	 {
	    /*
	     * Help-requester?
	     */
	    ShowHelpReq(bsh->bsh_Window, bd->bd_HelpText);
	    rc = BMHELP_OK;
	 }
	 else if (bd->bd_HelpFile)
	 {
	    /*
	     * Initialize the NewAmigaGuide structure.
	     */
#ifdef __AROS__
#warning Commented the following lines...
#else
	    nag.nag_Name   = (STRPTR)bd->bd_HelpFile;
	    nag.nag_Node   = (STRPTR)bd->bd_HelpNode;
	    nag.nag_Line   = bd->bd_HelpLine;
	    nag.nag_Screen = bsh->bsh_Window->WScreen;

	    /*
	     * Try to open it.
	     */
	    if (!DisplayAGuideInfo(&nag, TAG_END)) rc = BMHELP_FAILURE;
	    else                                   rc = BMHELP_OK;
#endif
	 };
      };
   };
   return rc;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * They want to know our minimum dimensions.
 */                      
METHOD(BaseClassDimensions, struct bmDimensions *, bmd)
{
   BD           *bd = INST_DATA(cl, obj);

   bmd->bmd_Extent->be_Min.Width  = bd->bd_LeftOffset + bd->bd_RightOffset;
   bmd->bmd_Extent->be_Min.Height = bd->bd_TopOffset  + bd->bd_BottomOffset;

   bmd->bmd_Extent->be_Nom        = bmd->bmd_Extent->be_Min;

   bmd->bmd_Extent->be_Max.Width  = bmd->bmd_Extent->be_Max.Height = 0xFFFF;

   /*
    * Get frame offset.
    */
   if (bd->bd_Frame) AsmDoMethodA(bd->bd_Frame, (Msg)bmd);

   return 1;
}
METHOD_END
///
/// GRM_DIMENSIONS
/*
 * They want to know our minimum dimensions.
 */
METHOD(BaseClassDimensionsX, struct grmDimensions *, dim)
{
   BD                *bd = INST_DATA(cl, obj);
   struct BaseInfo   *bi;
   struct bguiExtent  be;
   ULONG              rc = 0;

#ifdef DEBUG_BGUI
   if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, dim->grmd_GInfo, BI_RastPort, dim->grmd_RPort, TAG_DONE))
#else
   if (bi = AllocBaseInfo(BI_GadgetInfo, dim->grmd_GInfo, BI_RastPort, dim->grmd_RPort, TAG_DONE))
#endif
   {
      if (rc = AsmDoMethod(obj, BASE_DIMENSIONS, bi, &be, 0))
      {
	 /*
	  * Compute minimum width/height according to the label and its position.
	  */
	 if (bd->bd_Label) AsmDoMethod(bd->bd_Label, BASE_DIMENSIONS, bi, &be, 0);

	 /*
	  * Copy to grmDimensions.
	  */
	 *(dim->grmd_MinSize.Width ) = be.be_Min.Width;
	 *(dim->grmd_MinSize.Height) = be.be_Min.Height;

	 if (dim->grmd_Flags & GDIMF_MAXIMUM)
	 {
	    *(dim->grmd_MaxSize.Width ) = be.be_Max.Width;
	    *(dim->grmd_MaxSize.Height) = be.be_Max.Height;
	 };

	 if (dim->grmd_Flags & GDIMF_NOMINAL)
	 {
	    *(dim->grmd_NomSize.Width ) = be.be_Nom.Width;
	    *(dim->grmd_NomSize.Height) = be.be_Nom.Height;
	 };
      };
      FreeBaseInfo(bi);
   };
   return rc;
}
METHOD_END
///
/// BASE_RELAYOUT

METHOD(BaseClassRelayout, struct bmRelayout *, bmr)
{
   BD                *bd = INST_DATA(cl, obj);
   UWORD              minw, minh, maxw = 0xFFFF, maxh = 0xFFFF;
   ULONG              rc = 0;
   struct GadgetInfo *gi = bmr->bmr_GInfo;
   struct RastPort   *rp = bmr->bmr_RPort;
   Object            *wo = bd->bd_Window;

WW(kprintf("** BaseClassRelayout\n"));
   if (!wo) return 0;
WW(kprintf("** BaseClassRelayout. wo != 0. calling GRM_DIMENSIONS\n"));

   AsmDoMethod(obj, GRM_DIMENSIONS, gi, rp, &minw, &minh, GDIMF_MAXIMUM, &maxw, &maxh);
WW(kprintf("** BaseClassRelayout. GRM_DIMENSIONS returned minw = %d  minh = %d\n",minw,minh));

   if ((minw <= bd->bd_OuterBox.Width) && (minh <= bd->bd_OuterBox.Height))
   {
WW(kprintf("** BaseClassRelayout: minw and minh are <= outerbox.width/height\n"));
      if(!(bd->bd_Flags & BDF_INHIBITED))
      {
kprintf("** BaseClassRelayout: not inhibited. trying GM_RENDER\n");

	 if((rp=BGUI_ObtainGIRPort(gi)))
	 {
WW(kprintf("** BaseClassRelayout: doing GM_RENDER\n"));
	    rc=AsmDoMethod(obj,GM_RENDER,gi,rp,GREDRAW_REDRAW);
	    ReleaseGIRPort(rp);
	 }
	 else
	    rc=0;
      }
      else
	 rc=1;
   }
   else
   {
WW(kprintf("** BaseClassRelayout: minw and minh are > outerbox.width/height\n"));
      if (bd->bd_Group)
      {
WW(kprintf("** BaseClassRelayout: has bd->bd_Group -> sending BASE_RELAYOUT to group"));
	 rc = AsmDoMethodA(bd->bd_Group, (Msg)bmr);
      }
      else
      {
WW(kprintf("** BaseClassRelayout: does not have bd->bd_Group sending WM_RELAYOUT\n"));
	 rc = AsmDoMethod(wo, WM_RELAYOUT);
      };
   };
   return rc;
}
METHOD_END
///
		  

/// BASE_LEFTEXT
/*
 * Left extention request.
 */
METHOD(BaseClassLeftExt, struct bmLeftExt *, le)
{
   BD            *bd = INST_DATA(cl, obj);
   UWORD          dummy;
   struct IBox    ibox;
   int            ext;

   /*
    * Do we have a label?
    */
   if (bd->bd_Label)
   {
      /*
       * Get the left-extention of the label if we do not have it yet.
       */
      if (!(bd->bd_Flags & BDF_GOT_LABEL_EXT))
      {
	 DoExtentMethod(bd->bd_Label, le->bmle_RPort, &ibox, &dummy, &dummy, EXTF_MAXIMUM);
	 ext = -ibox.Left;
      }
      else
      {
	 ext = -bd->bd_Extentions.Left;
      };
   }
   else ext = 0;

   *(le->bmle_Extention) = ext;

   return 1;
}
METHOD_END
///
/// WM_KEYACTIVE
/*
 * No key input.
 */
METHOD(BaseClassKeyActive, struct wmKeyInput *, wmki)
{
   return WMKF_CANCEL;
}
METHOD_END
///

/*
 * Pack two words in a long.
 */
//#define PACKW(w1,w2) (((w1<<16)&0xFFFF0000)|(w2&0xFFFF))

#ifdef __AROS__
#define PACKW(w1,w2) w1,w2
#else
#define PACKW(w1,w2) ( (((UWORD)w1) << 16) | ((UWORD)w2) )
#endif

#define bd_ActRec bd_BMO->bmo_ActRec
#define bd_ActWin bd_BMO->bmo_ActWin
#define bd_ActPtr bd_BMO->bmo_ActPtr

/// GM_GOACTIVE
/*
 * Go active.
 */
METHOD(BaseClassGoActive, struct gpInput *, gpi)
{
   BD           *bd = INST_DATA(cl, obj);

   /*
    * Are we draggable?
    */
   if (bd->bd_Flags & BDF_DRAGGABLE && gpi->gpi_IEvent->ie_Qualifier & bd->bd_DragQual)
   {
      /*
       * Close parent tool-tip.
       */
      if (bd->bd_Parent)
      {
	 Forbid();
	 AsmDoMethod(bd->bd_Parent, WM_CLOSETOOLTIP);
	 Permit();
      };

      /*
       * Let the object create a bitmap.
       */
      if (bd->bd_BMO = CreateBMO(obj, gpi->gpi_GInfo))
      {
	 return GMR_MEACTIVE;
      }
   }
   return GMR_NOREUSE;
}
METHOD_END
///
/// BASE_DRAGGING
/*
 * Handle input.
 */
METHOD(BaseClassDragging, struct gpInput *, gpi)
{
   BD                *bd = INST_DATA(cl, obj);
   BMO               *bmo = bd->bd_BMO;
   WORD               dx, dy, nx, ny;
   ULONG              rc = bd->bd_Flags & BDF_MOVING ? BDR_DRAGGING : BDR_NONE;
   struct IBox       *db;
   struct GadgetInfo *gi = gpi->gpi_GInfo;

   /*
    * We only process input if we have drag-buffers.
    */
   if (bmo)
   {
      /*
       * When we have an active receiver we send
       * it an update message each timer tick.
       */
      if ((bd->bd_Flags & BDF_MOVING) && (gpi->gpi_IEvent->ie_Class == IECLASS_TIMER))
      {
	 if (!--bmo->bmo_BMDelay)
	 {
	    /*
	     * Open window.
	     */
	    LayerBMO(bmo);
	 };

	 if (bd->bd_ActRec)
	 {
	    /*
	     * Send an update to the target.
	     */

	    /*
	     * Get mouse position.
	     */
	    nx = gi->gi_Screen->MouseX;
	    ny = gi->gi_Screen->MouseY;

	    /*
	     * Get the hitbox bounds of the
	     * receiver.
	     */
	    Get_Attr(bd->bd_ActRec, BT_HitBox, &db);

	    /*
	     * Get mouse coords relative to the
	     * receiver hitbox.
	     */
	    dx = nx - (bd->bd_ActPtr->LeftEdge + db->Left);
	    dy = ny - (bd->bd_ActPtr->TopEdge  + db->Top );

	    /*
	     * Send off a drag update message
	     * to the receiver.
	     */
	    if (!bmo->bmo_BMWindow && bmo->bmo_BMDelay) EraseBMO(bmo);

	    if (myDoGadgetMethod(bd->bd_ActRec, bd->bd_ActPtr, NULL, BASE_DRAGUPDATE, NULL, obj, PACKW(dx, dy)) != BUR_CONTINUE)
	    {
	       /*
		* Deactivate target.
		*/
	       myDoGadgetMethod(bd->bd_ActRec, bd->bd_ActPtr, NULL, BASE_DRAGINACTIVE, NULL, obj);
	       bd->bd_ActRec = NULL;
	    };

	    if (!bmo->bmo_BMWindow && bmo->bmo_BMDelay) MoveBMO(bmo, nx, ny);
	 };
      }
      else if (gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
      {
	 switch (gpi->gpi_IEvent->ie_Code)
	 {
	 case SELECTUP:
	 case MENUDOWN:
	    /*
	     * Were we moved?
	     */
	    if (bd->bd_Flags & BDF_MOVING)
	    {
	       /*
		* Hide object.
		*/
	       EraseBMO(bmo);

	       /*
		* "Drop" us on the target.
		*/
	       if (bd->bd_ActRec && gpi->gpi_IEvent->ie_Code == SELECTUP)
		  myDoGadgetMethod(bd->bd_ActRec, gi->gi_Window, gi->gi_Requester, BASE_DROPPED, NULL, obj, bd->bd_ActPtr, NULL);

	       /*
		* Deactivate the target.
		*/
	       if (bd->bd_ActRec)
		  myDoGadgetMethod(bd->bd_ActRec, bd->bd_ActPtr, NULL, BASE_DRAGINACTIVE, NULL, obj);

	       /*
		* Selecting the menu button or when there is no active drop object means we return
		* BDR_CANCEL. Otherwise we return BDR_DROP.
		*/
	       if ((gpi->gpi_IEvent->ie_Code == MENUDOWN) || ! bd->bd_ActRec)
		  rc = BDR_CANCEL;
	       else
		  rc = BDR_DROP;
	    } else
	       rc = BDR_NONE;

	    /*
	     * Unlock the window.
	     */
	    if (bd->bd_ActWin) AsmDoMethod(bd->bd_ActWin, WM_UNLOCK);
	    break;

	 default:
	    /*
	     * Pick up mouse coordinates.
	     */
	    nx = gi->gi_Screen->MouseX;
	    ny = gi->gi_Screen->MouseY;

	    bmo->bmo_BMDelay = 5;

	    /*
	     * Are we moving yet?
	     */
	    if (bd->bd_Flags & BDF_MOVING)
	    {
	       /*
		* Move the object to the new position.
		*/
	       MoveBMO(bmo, nx, ny);

	       /*
		* Were still dragging along...
		*/
	       rc = BDR_DRAGGING;
	    }
	    else
	    {
	       dx = nx - bmo->bmo_CX;
	       dy = ny - bmo->bmo_CY;
	       if (((dx * dx) + (dy * dy)) >= (bd->bd_Thresh * bd->bd_Thresh))
	       {
		  /*
		   * They have passed the Threshold.
		   */
		  bd->bd_Flags |= BDF_MOVING;

		  /*
		   * Tell'm to prepare for moving.
		   */
		  rc = BDR_DRAGPREPARE;
	       };
	    };
	    break;
	 }
      }
   }
   return rc;
}
METHOD_END
///
/// GM_GOINACTIVE

METHOD(BaseClassGoInactive, struct gpGoInactive *, gpgi)
{
   BD       *bd = INST_DATA(cl, obj);

   /*
    * Any buffering available?
    */
   if (bd->bd_BMO)
   {
      /*
       * Cleanup the buffers.
       */
      DeleteBMO(bd->bd_BMO);
      bd->bd_BMO = NULL;

      /*
       * Clear all drag related flags.
       */
      bd->bd_Flags &= ~BDF_MOVING;
   };

   return AsmDoSuperMethodA(cl, obj, (Msg)gpgi);
}
METHOD_END
///
/// BASE_DRAGQUERY
/*
 * Query request if this object is willing or able
 * to receive the drag-object data.
 */
METHOD(BaseClassDragQuery, struct bmDragPoint *, bmdp)
{
   BD          *bd = INST_DATA(cl, obj);
   ULONG        rc = BQR_REJECT;

   /*
    * The baseclass will allow _all_ drag objects
    * when this object is DROPPABLE and the mouse is
    * located inside the hitbox bounds.
    */

   if (bd->bd_Flags & BDF_DROPPABLE)
   {
      if (bmdp->bmdp_Mouse.X >= 0 &&
	  bmdp->bmdp_Mouse.Y >= 0 &&
	  bmdp->bmdp_Mouse.X < bd->bd_HitBox.Width &&
	  bmdp->bmdp_Mouse.Y < bd->bd_HitBox.Height)
	 rc = BQR_ACCEPT;
   }
   return rc;
}
METHOD_END
///
/// BASE_DRAGACTIVE
/*
 * Show us as being the active receiver of the
 * drag object.
 */
METHOD(BaseClassDragActive, struct bmDragMsg *, bmdm)
{
   BD                   *bd = INST_DATA(cl, obj);
   struct BaseInfo      *bi;

   bd->bd_Flags |= BDF_MOVE_DROPBOX;
   
   /*
    * Let's show'm were active.
    */
#ifdef DEBUG_BGUI
   if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, bmdm->bmdm_GInfo, BI_RastPort, NULL, TAG_DONE))
#else
   if (bi = AllocBaseInfo(BI_GadgetInfo, bmdm->bmdm_GInfo, BI_RastPort, NULL, TAG_DONE))
#endif
   {
      /*
       * Draw the box.
       */
      DottedBox(bi, &bd->bd_HitBox);

      FreeBaseInfo(bi);
   };
   return 1;
}
METHOD_END
///
/// BASE_DRAGUPDATE
/*
 * We return BUR_CONTINUE while the coordinates are
 * still in the hitbox bounds.
 */
METHOD(BaseClassDragUpdate, struct bmDragPoint *, bmdp)
{
   BD                *bd = INST_DATA(cl, obj);
   struct BaseInfo   *bi;
   
   int x = bmdp->bmdp_Mouse.X;
   int y = bmdp->bmdp_Mouse.Y;

   if (bd->bd_Flags & BDF_MOVE_DROPBOX)
   {
#ifdef DEBUG_BGUI
      if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, bmdp->bmdp_GInfo, BI_RastPort, NULL, TAG_DONE))
#else
      if (bi = AllocBaseInfo(BI_GadgetInfo, bmdp->bmdp_GInfo, BI_RastPort, NULL, TAG_DONE))
#endif
      {
	 DottedBox(bi, &bd->bd_HitBox);
	 FreeBaseInfo(bi);
      };
   };

   if ((x >= 0) && (y >= 0) && (x < bd->bd_HitBox.Width) && (y < bd->bd_HitBox.Height))
      return BUR_CONTINUE;

   return BUR_ABORT;
}
METHOD_END
///
/// BASE_DRAGDROPPED
/*
 * We beep the display when we are dropped and
 * the message get's here.
 */
METHOD(BaseClassDragDropped, struct bmDropped *, bmd)
{
   /*
    * Only beep our screen.
    */
   if (bmd->bmd_GInfo)
      DisplayBeep(bmd->bmd_GInfo->gi_Screen);

   return 1;
}
METHOD_END
///
/// BASE_DRAG_INACTIVE
/*
 * Deactivate us as being the active receiver of the
 * drag object.
 */
METHOD(BaseClassDragInactive, struct bmDragMsg *, bmdm)
{
   BD                *bd = INST_DATA(cl, obj);
   struct GadgetInfo *gi = bmdm->bmdm_GInfo;
   struct RastPort   *rp;

   bd->bd_Flags &= ~BDF_MOVE_DROPBOX;

   /*
    * We will force a redraw of the object
    * to eliminate any changes made when the
    * object was activated.
    */
   if (rp = BGUI_ObtainGIRPort(bmdm->bmdm_GInfo))
   {
      /*
       * Refresh the object.
       */
      AsmDoMethod(obj, GM_RENDER, gi, rp, GREDRAW_REDRAW);
      ReleaseGIRPort(rp);
      return 1;
   };
   return 0;
}
METHOD_END
///
/// BASE_GETDRAGOBJECT
/*
 * Create a bitmap with the image that has to be dragged
 * around. The baseclass defaults to a bitmap containing
 * the hitbox imagery of the object.
 */
METHOD(BaseClassGetObject, struct bmGetDragObject *, bmgo)
{
   BD                *bd = INST_DATA(cl, obj);
   struct GadgetInfo *gi = bmgo->bmgo_GInfo, tgi = *gi;
   struct RastPort    rp = gi->gi_Screen->RastPort;
   ULONG              rc;
   
   int x = bd->bd_HitBox.Left;
   int y = bd->bd_HitBox.Top;
   int w = bd->bd_HitBox.Width;
   int h = bd->bd_HitBox.Height;

   /*
    * Allocate a bitmap large enough to hold
    * the hitbox of the object.
    */
   if (rc = (ULONG)BGUI_AllocBitMap(w, h, FGetDepth(&rp), BMF_CLEAR, rp.BitMap))
   {
      /*
       * Setup rastport for rendering into this new BitMap.
       */
      rp.Layer  = NULL;
      rp.BitMap = (struct BitMap *)rc;
      
      /*
       * Setup the GadgetInfo for rendering gadget into this new BitMap.
       */
      tgi.gi_RastPort      = &rp;
      tgi.gi_Layer         = NULL;

      /*
       * Copy the HitBox bounds into the message structure.
       */
      *bmgo->bmgo_Bounds = bd->bd_HitBox;
      
      SetAttrs(obj, GA_Left, 0, GA_Top, 0, TAG_DONE);
      AsmDoMethod(obj, GM_RENDER, &tgi, &rp, GREDRAW_REDRAW);     
      SetAttrs(obj, GA_Left, x, GA_Top, y, TAG_DONE);
   };
   return rc;
}
METHOD_END
///
/// BASE_FREEDRAGOBJECT
/*
 * Free the bitmap allocate with BASE_GETDRAGOBJECT.
 */
METHOD(BaseClassFreeObject, struct bmFreeDragObject *, bmfo)
{
   /*
    * Simply free the bitmap created
    * in the BASE_GETDRAGOBJECT method.
    */
   BGUI_FreeBitMap(bmfo->bmfo_ObjBitMap);
   return 1;
}
METHOD_END
///
/// BASE_MOVEBOUNDS

static void MoveBox(struct IBox *box, struct bmMoveBounds *bmmb)
{
   box->Left   += bmmb->bmmb_ChangeX;
   box->Top    += bmmb->bmmb_ChangeY;
   box->Width  += bmmb->bmmb_ChangeW;
   box->Height += bmmb->bmmb_ChangeH;
}

METHOD(BaseClassMoveBounds, struct bmMoveBounds *, bmmb)
{
   BD       *bd = INST_DATA(cl, obj);

   MoveBox(GADGETBOX(obj),   bmmb);
   MoveBox(&bd->bd_OuterBox, bmmb);
   MoveBox(&bd->bd_InnerBox, bmmb);
   MoveBox(&bd->bd_HitBox,   bmmb);

   return 1;
}
METHOD_END
///
/// BASE_FINDKEY
/*
 * Does object has the proper key?
 */
METHOD(BaseClassFindKey, struct bmFindKey *, bmfk)
{
#define QUAL_SHIFT (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)
#define QUAL_ALT (IEQUALIFIER_LALT|IEQUALIFIER_RALT)
#define QUAL_MASK 0x71FF

   BD    *bd = INST_DATA(cl, obj);
   
   UWORD  Key  = bmfk->bmfk_Key.Key;
   UWORD  Qual = bmfk->bmfk_Key.Qual & QUAL_MASK;
   UBYTE  key  = bd->bd_RawKey;
   UWORD  qual = bd->bd_Qual & QUAL_MASK;
   
   if ((GADGET(obj)->Flags & GFLG_DISABLED) || (bd->bd_Flags & BDF_INHIBITED))
      return NULL;

   if (qual == QUAL_MASK) Qual = qual;
   
   if (((qual & QUAL_SHIFT) == QUAL_SHIFT) && (Qual & QUAL_SHIFT)) Qual |= QUAL_SHIFT;
   if (((qual & QUAL_ALT)   == QUAL_ALT)   && (Qual & QUAL_ALT))   Qual |= QUAL_ALT;

   if (Key & 0xFF00)                /* RAWKEY press, must match exactly */
   {
      if (key != (UBYTE)~0)
      {
	 if ((key == (Key & 0x007F)) && (qual == Qual))
	 {
	    return (ULONG)obj;
	 };
      };
   }
   else                             /* VANILLAKEY press */
   {
      if (bd->bd_Key)               /* We have a vanilla key equivalent */
      {  
	 if ((ToLower(*bd->bd_Key) == ToLower(Key)) && (qual == Qual))
	 {
	    return (ULONG)obj;
	 };
      };
   };
   return NULL;
}
METHOD_END
///
/// BASE_KEYLABEL
/*
 * Attach key from label to object.
 */
METHOD(BaseClassKeyLabel, struct bmKeyLabel *, bmkl)
{
   BD         *bd = INST_DATA(cl, obj);
   ULONG       rc = 0;
   
   if (bd->bd_Label)
   {
      bd->bd_Key     = NULL;
      bd->bd_RawKey  = (UBYTE)~0;
      bd->bd_Qual    = 0;

      Get_Attr(bd->bd_Label, LAB_KeyChar, &rc);
      if (rc)
      {
	 bd->bd_Key  = (UBYTE *)rc;
	 bd->bd_Qual = (UWORD)~0;
      };
   };
   return rc;
}
METHOD_END
///
/// BASE_LOCALIZE
/*
 * Attach key from label to object.
 */
METHOD(BaseClassLocalize, struct bmLocalize *, bml)
{
   BD       *bd = INST_DATA(cl, obj);

   if (bd->bd_Label) AsmDoMethodA(bd->bd_Label, (Msg)bml);
   if (bd->bd_Frame) AsmDoMethodA(bd->bd_Frame, (Msg)bml);
   
   if (bd->bd_HelpTextID) bd->bd_HelpText =
      BGUI_GetCatalogStr(bml->bml_Locale, bd->bd_HelpTextID, bd->bd_HelpText);

   if (bd->bd_ToolTipID) bd->bd_ToolTip =
      BGUI_GetCatalogStr(bml->bml_Locale, bd->bd_ToolTipID, bd->bd_ToolTip);
   
   return 1;
}
METHOD_END
///
/// BASE_INHIBIT
/*
 * Inhibit/uninhibit object.
 */
METHOD(BaseClassInhibit, struct bmInhibit *, bmi)
{
   BD       *bd = INST_DATA(cl, obj);

   if (bmi->bmi_Inhibit) bd->bd_Flags |= BDF_INHIBITED;
   else                  bd->bd_Flags &= ~BDF_INHIBITED;

   return 1;
}
METHOD_END
///
/// BASE_IS_MULTI
METHOD(BaseClassIsMulti, Msg, msg)
{
   return FALSE;
}
METHOD_END
///
/// Class initialization.
/*
 * Class function array.
 */
STATIC DPFUNC ClassFunc[] =
{
   RM_GET,                 (FUNCPTR)BaseClassGet,
   RM_GETCUSTOM,           (FUNCPTR)BaseClassGetCustom,
   RM_SET,                 (FUNCPTR)BaseClassSet,
   RM_SETCUSTOM,           (FUNCPTR)BaseClassSetCustom,
   RM_GETATTRFLAGS,        (FUNCPTR)BaseClassGetAttrFlags,
   RM_REFRESH,             (FUNCPTR)BaseClassRefresh,
   OM_NOTIFY,              (FUNCPTR)BaseClassNotify,
   OM_DISPOSE,             (FUNCPTR)BaseClassDispose,
   OM_NEW,                 (FUNCPTR)BaseClassNew,

   BASE_LAYOUT,            (FUNCPTR)BaseClassLayout,
   BASE_RENDER,            (FUNCPTR)BaseClassRender,
   BASE_DIMENSIONS,        (FUNCPTR)BaseClassDimensions,
   
   GM_RENDER,              (FUNCPTR)BaseClassRenderX,
   GRM_DIMENSIONS,         (FUNCPTR)BaseClassDimensionsX,
   BASE_RELAYOUT,          (FUNCPTR)BaseClassRelayout,

   GM_HITTEST,             (FUNCPTR)BaseClassHitTest,
   GM_HELPTEST,            (FUNCPTR)BaseClassHelpTest,
   BASE_IS_MULTI,          (FUNCPTR)BaseClassIsMulti,
   BASE_SHOWHELP,          (FUNCPTR)BaseClassHelp,
   BASE_LEFTEXT,           (FUNCPTR)BaseClassLeftExt,
   WM_KEYACTIVE,           (FUNCPTR)BaseClassKeyActive,
   GM_GOACTIVE,            (FUNCPTR)BaseClassGoActive,
   GM_GOINACTIVE,          (FUNCPTR)BaseClassGoInactive,
   BASE_MOVEBOUNDS,        (FUNCPTR)BaseClassMoveBounds,
   BASE_DRAGGING,          (FUNCPTR)BaseClassDragging,
   BASE_DRAGQUERY,         (FUNCPTR)BaseClassDragQuery,
   BASE_DRAGACTIVE,        (FUNCPTR)BaseClassDragActive,
   BASE_DRAGUPDATE,        (FUNCPTR)BaseClassDragUpdate,
   BASE_DROPPED,           (FUNCPTR)BaseClassDragDropped,
   BASE_DRAGINACTIVE,      (FUNCPTR)BaseClassDragInactive,
   BASE_GETDRAGOBJECT,     (FUNCPTR)BaseClassGetObject,
   BASE_FREEDRAGOBJECT,    (FUNCPTR)BaseClassFreeObject,
   BASE_FINDKEY,           (FUNCPTR)BaseClassFindKey,
   BASE_KEYLABEL,          (FUNCPTR)BaseClassKeyLabel,
   BASE_LOCALIZE,          (FUNCPTR)BaseClassLocalize,
   BASE_INHIBIT,           (FUNCPTR)BaseClassInhibit,
   
   DF_END
};

makeproto Class *BaseClass = NULL;

/*
 * Simple class initialization.
 */
makeproto Class *InitBaseClass(void)
{
   return BaseClass = BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_GADGET_OBJECT,
				     CLASS_ObjectSize,     sizeof(BD),
				     CLASS_DFTable,        ClassFunc,
				     TAG_DONE);
}
///

