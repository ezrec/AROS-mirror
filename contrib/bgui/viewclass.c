/*
 * @(#) $Header$
 *
 * BGUI library
 * viewclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.1  2000/05/15 19:27:03  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:10:39  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:27  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.4  1999/08/01 22:49:31  mlemos
 * Assured proper replacement of scroller bars after the object is created.
 *
 * Revision 41.10.2.3  1999/08/01 19:23:33  mlemos
 * Assured that external scroller gadgets are redrawn when the view position
 * is updated.
 *
 * Revision 41.10.2.2  1999/08/01 04:13:48  mlemos
 * Assured that the rendering of a view within a view is not clipped against
 * the window boundaries.
 * Assured that the view of the self created scroll bars is the same as of the
 * view object.
 * Made the scroll bars be rendered or updated in the GM_RENDER method.
 * Added support for the VIEW_CLIP method to not clip against the parent view
 * if requested.
 *
 * Revision 41.10.2.1  1998/12/06 22:36:10  mlemos
 * Set parent window of contained object on the view object creation.
 * Set parent window and parent view of of the scroller gadgets on the view
 * object creation.
 *
 * Revision 41.10  1998/02/25 21:13:31  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:10:04  mlemos
 * Ian sources
 *
 *
 */

/// Class definitiions.
#include "include/classdefs.h"

/*
 * Instance data.
 */
typedef struct {
   ULONG              vd_Flags;           /* View flags.               */
   struct IBox        vd_AreaBox;         /* Current area bounds.      */
   UWORD              vd_MinWidth;        /* Minimum width of area.    */
   UWORD              vd_MinHeight;       /* Minimum height of area.   */
   Object            *vd_Object;          /* Object to view.           */
   UWORD              vd_ObjectWidth;     /* Width of object.          */
   UWORD              vd_ObjectHeight;    /* Height of object.         */
   struct RastPort   *vd_ObjectBuffer;    /* RastPort buffer.          */
   UWORD              vd_ScaleWidth;      /* Extra width.              */
   UWORD              vd_ScaleHeight;     /* Extra height.             */
   UWORD              vd_ScaleMinWidth;   /* Scaled minimum width.     */
   UWORD              vd_ScaleMinHeight;  /* Scaled minimum height.    */
   Object            *vd_VScroller;       /* Vertical scroller.        */
   Object            *vd_HScroller;       /* Horizontal scroller.      */
   UWORD              vd_ScrollerW;       /* Width of v-scroller.      */
   UWORD              vd_ScrollerH;       /* Height of h-scroller.     */
   WORD               vd_X, vd_Y;         /* Current position.         */
   WORD               vd_BX, vd_BY;       /* Base position.            */
   WORD               vd_LX, vd_LY;       /* Last position.            */
   UWORD              vd_Qualifier;       /* Qualifier for drag move.  */
   Object            *vd_Parent;          /* Parent window.            */
   Object            *vd_ParentView;      /* Parent view.              */
   struct Rectangle   vd_ClipRect;        /* Clipping rectangle.       */
   UWORD              vd_CachedWidth;     /* Cached width of object.   */
   UWORD              vd_CachedHeight;    /* Cached height of object.  */
   BOOL               vd_OwnVScroller;    /* Own Vertical scroller.    */
   BOOL               vd_OwnHScroller;    /* Own Horizontal scroller.  */
}  VD;

#define VDF_NODISPOSEOBJECT   (1<<0)      /* Don't dispose object.     */
#define VDF_VPROPACTIVE       (1<<1)      /* VScroller active.         */
#define VDF_HPROPACTIVE       (1<<2)      /* HScroller active.         */
#define VDF_DIRECT            (1<<3)      /* Directly access object.   */
#define VDF_DRAGMOVE          (1<<4)      /* Move while dragging.      */
#define VDF_NO_DRAGMOVE       (1<<5)      /* Allow this feature.       */

/*
 * Prop map-lists.
 */
static struct TagItem VProp2View[] = {
   PGA_Top,       VIEW_Y,
   TAG_DONE
};

static struct TagItem HProp2View[] = {
   PGA_Top,       VIEW_X,
   TAG_DONE
};

///
/// OM_NEW

METHOD(ViewClassNew, struct opSet *, ops)
{
   VD             *vd;
   ULONG           rc;
   struct TagItem *tstate, *tag, *tags;

   tags = DefTagList(BGUI_VIEW_GADGET, ops->ops_AttrList);

   /*
    * Let the superclass create an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      vd = INST_DATA(cl, rc);

      AsmCoerceMethod(cl, (Object *)rc, OM_SET, tags, NULL);
      
      /*
       * Object required!
       */
      if (vd->vd_Object)
      {
         BC *bc = BASE_DATA(rc);

         DoSetMethodNG(vd->vd_Object, BT_ParentView, rc, BT_ParentGroup, rc, BT_ParentWindow, vd->vd_Parent, TAG_DONE);

         vd->vd_OwnHScroller=vd->vd_OwnVScroller=TRUE;

         /*
          * Filter out frame and label attributes.
          */
         tstate = tags;
         while (tag = NextTagItem(&tstate))
         {
            switch (tag->ti_Tag)
            {
            /*
             * We control the horizontal, we control the vertical.
             */
            case PGA_Freedom:
            /*
             * Don't disable prop!
             */
            case GA_Disabled:
            /*
             * No drag'n'drop on the prop.
             */
            case BT_DragObject:
            case BT_DropObject:
               tag->ti_Tag = TAG_IGNORE;
               break;

            case VIEW_HScroller:
               tag->ti_Tag = TAG_IGNORE;
               vd->vd_OwnHScroller=FALSE;
               break;

            case VIEW_VScroller:
               tag->ti_Tag = TAG_IGNORE;
               vd->vd_OwnVScroller=FALSE;
               break;

            default:
               if (FRM_TAG(tag->ti_Tag) || LAB_TAG(tag->ti_Tag))
                  tag->ti_Tag = TAG_IGNORE;
               break;
            };
         };

         if(vd->vd_OwnHScroller)
         {
            Object *scroller;

            vd->vd_OwnHScroller=FALSE;
            if((scroller = BGUI_NewObject(BGUI_PROP_GADGET,
               GA_ID, GADGET(rc)->GadgetID,
               PGA_DontTarget, TRUE, 
               PGA_Freedom, FREEHORIZ,
               PGA_Arrows, FALSE,
               BT_ParentView, bc->bc_View,
               BT_ParentWindow, vd->vd_Parent,
               TAG_MORE, tags)))
            {
               DoSetMethodNG((Object *)rc, VIEW_HScroller, scroller, TAG_DONE);
               vd->vd_OwnHScroller=TRUE;
               vd->vd_ScrollerH = 14;
            }
         }

         if(vd->vd_OwnVScroller)
         {
            Object *scroller;

            vd->vd_OwnVScroller=FALSE;
            if((scroller = BGUI_NewObject(BGUI_PROP_GADGET,
               GA_ID, GADGET(rc)->GadgetID,
               PGA_DontTarget, TRUE, 
               PGA_Freedom, FREEVERT,
               PGA_Arrows, FALSE,
               BT_ParentView, bc->bc_View,
               BT_ParentWindow, vd->vd_Parent,
               TAG_MORE, tags)))
            {
               DoSetMethodNG((Object *)rc, VIEW_VScroller, scroller, TAG_DONE);
               vd->vd_OwnVScroller=TRUE;
               vd->vd_ScrollerW = 14;
            }
         }

         return rc;
      }
      else
      {
         /*
          * Fail :(
          */
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = 0;
      };
   };
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_DISPOSE

METHOD(ViewClassDispose, Msg, msg)
{
   VD             *vd = INST_DATA(cl, obj);

   if (vd->vd_OwnVScroller) DisposeObject(vd->vd_VScroller);
   if (vd->vd_OwnHScroller) DisposeObject(vd->vd_HScroller);

   if (vd->vd_ObjectBuffer) BGUI_FreeRPortBitMap(vd->vd_ObjectBuffer);
   
   if (!(vd->vd_Flags & VDF_NODISPOSEOBJECT))
      if (vd->vd_Object) DisposeObject(vd->vd_Object);
      
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// OM_SET
METHOD(ViewClassSet, struct opSet *, ops)
{
   VD             *vd = INST_DATA(cl, obj);
   struct TagItem *tstate = ops->ops_AttrList, *tag;
   ULONG           rc = 1, update = FALSE, data;

   /*
    * First we let the superclass
    * do it's thing.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)ops);

   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case VIEW_MinWidth:
         vd->vd_MinWidth = data;
         break;

      case VIEW_MinHeight:
         vd->vd_MinHeight = data;
         break;

      case VIEW_ScaleMinWidth:
         vd->vd_ScaleMinWidth = data;
         break;

      case VIEW_ScaleMinHeight:
         vd->vd_ScaleMinHeight = data;
         break;

      case VIEW_ScaleWidth:
         vd->vd_ScaleWidth = data;
         break;

      case VIEW_ScaleHeight:
         vd->vd_ScaleHeight = data;
         break;

      case VIEW_Object:
         vd->vd_Object = (Object *)data;
         break;

      case VIEW_X:
         if (vd->vd_X != data) update = TRUE;
         vd->vd_X = data;
         break;

      case VIEW_Y:
         if (vd->vd_Y != data) update = TRUE;
         vd->vd_Y = data;
         break;

      case VIEW_ObjectBuffer:
         if (vd->vd_ObjectBuffer) BGUI_FreeRPortBitMap(vd->vd_ObjectBuffer);
         vd->vd_ObjectBuffer = (struct RastPort *)data;
         break;

      case VIEW_NoDisposeObject:
         if (data)
            vd->vd_Flags |= VDF_NODISPOSEOBJECT;
         else
            vd->vd_Flags &= ~VDF_NODISPOSEOBJECT;
         break;

      case VIEW_VScroller:
         if (vd->vd_OwnVScroller)
         {
            DisposeObject(vd->vd_VScroller);
            vd->vd_OwnVScroller=FALSE;
         }
         if (vd->vd_VScroller = (Object *)data)
         {
            AsmDoMethod(vd->vd_VScroller, BASE_ADDMAP, obj, VProp2View);
            vd->vd_ScrollerW = 0;
            update=TRUE;
         };
         break;

      case VIEW_HScroller:
         if (vd->vd_OwnHScroller)
         {
            DisposeObject(vd->vd_HScroller);
            vd->vd_OwnHScroller=FALSE;
         }
         if (vd->vd_HScroller = (Object *)data)
         {
            AsmDoMethod(vd->vd_HScroller, BASE_ADDMAP, obj, HProp2View);
            vd->vd_ScrollerH = 0;
            update=TRUE;
         };
         break;

      case BT_ParentView:
         vd->vd_ParentView = (Object *)data;
         break;

      case BT_ParentWindow:
         vd->vd_Parent = (Object *)data;
      case BT_TextAttr:
      case FRM_ThinFrame:
         DoMultiSet(tag->ti_Tag, data, 3, vd->vd_Object, vd->vd_HScroller, vd->vd_VScroller);
         break;
      };
   };
   if (vd->vd_ScaleWidth  < 100) vd->vd_ScaleWidth  = 100;
   if (vd->vd_ScaleHeight < 100) vd->vd_ScaleHeight = 100;

   if (update
   &&  ops->ops_GInfo)
      DoRenderMethod(obj, ops->ops_GInfo, GREDRAW_UPDATE);
   
   return rc;
}
METHOD_END
///
/// OM_GET
METHOD(ViewClassGet, struct opGet *, opg)
{
   VD       *vd = INST_DATA(cl, obj);
   BC       *bc = BASE_DATA(obj);
   ULONG     rc = 1;
   ULONG    *store = opg->opg_Storage;
   
   switch (opg->opg_AttrID)
   {
   case VIEW_MinWidth:
      STORE vd->vd_MinWidth;
      break;

   case VIEW_MinHeight:
      STORE vd->vd_MinHeight;
      break;

   case VIEW_ScaleMinWidth:
      STORE vd->vd_ScaleMinWidth;
      break;

   case VIEW_ScaleMinHeight:
      STORE vd->vd_ScaleMinHeight;
      break;

   case VIEW_ScaleWidth:
      STORE vd->vd_ScaleWidth;
      break;

   case VIEW_ScaleHeight:
      STORE vd->vd_ScaleHeight;
      break;

   case VIEW_X:
      STORE vd->vd_X;
      break;

   case VIEW_Y:
      STORE vd->vd_Y;
      break;

   case VIEW_AbsoluteX:
      if (vd->vd_ParentView)
         Get_Attr(vd->vd_ParentView, VIEW_AbsoluteX, store);
      else
         *store = 0;
      *store += ((vd->vd_Flags & VDF_DIRECT) ? &bc->bc_OuterBox : &bc->bc_InnerBox)->Left - vd->vd_X;
      break;

   case VIEW_AbsoluteY:
      if (vd->vd_ParentView)
         Get_Attr(vd->vd_ParentView, VIEW_AbsoluteY, store);
      else
         *store = 0;
      *store += ((vd->vd_Flags & VDF_DIRECT) ? &bc->bc_OuterBox : &bc->bc_InnerBox)->Top - vd->vd_Y;
      break;

   case VIEW_ObjectBuffer:
      STORE vd->vd_ObjectBuffer;
      break;

   case BT_PostRenderHighestClass:
      STORE cl;
      break;

   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   };
   return rc;
}
METHOD_END
///
/// ViewClassForward
/*
 * Forward a message to the object.
 */
METHOD(ViewClassForward, Msg, msg)
{
   VD              *vd = INST_DATA(cl, obj);
   ULONG            rc = GMR_NOREUSE;

   /*
    * Do we have an active member?
    */
   if (vd->vd_Object)
   {
      /*
       * Forward the message to the object.
       */
      rc = AsmDoMethodA(vd->vd_Object, msg);

      /*
       * Take over the object's GadgetID.
       */
      GADGET(obj)->GadgetID = GADGET(vd->vd_Object)->GadgetID;
   }
   return rc;
}
METHOD_END
///
/// GM_RENDER

METHOD(ViewClassRender, struct gpRender *, gpr)
{
   VD                *vd = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   struct GadgetInfo *gi = gpr->gpr_GInfo;
   struct RastPort   *rp;
   Object            *o = vd->vd_Object, *vscroll, *hscroll;
   ULONG              rc = 0;
   UWORD              ow, oh, vw, vh, sw, sh;
   struct Rectangle   cr;
   struct IBox       *box;

   DoSetMethodNG(bc->bc_Frame, FRM_OuterOffsetRight,  vd->vd_ScrollerW,
                               FRM_OuterOffsetBottom, vd->vd_ScrollerH,
                               TAG_DONE);

   /*
    * See if rendering is allowed.
    */
   if (!AsmDoSuperMethodA(cl, obj, (Msg)gpr))
      return 0;

   if (o && (rp = bc->bc_RPort))
   {
      if ((gpr->gpr_Redraw == GREDRAW_REDRAW) || !vd->vd_ObjectBuffer)
      {
         AsmDoMethod(o, GRM_DIMENSIONS, gi, rp, &vd->vd_CachedWidth, &vd->vd_CachedHeight, 0);
      }

      ow = ScaleWeight(vd->vd_CachedWidth, 100, vd->vd_ScaleWidth);
      oh = ScaleWeight(vd->vd_CachedHeight, 100, vd->vd_ScaleHeight);

      vscroll = vd->vd_VScroller;
      hscroll = vd->vd_HScroller;

      sw = vd->vd_ScrollerW;
      sh = vd->vd_ScrollerH;

      if ((ow < bc->bc_OuterBox.Width) && (oh < bc->bc_OuterBox.Height))
      {
         vd->vd_Flags |= VDF_DIRECT;

         if (sw) vscroll = 0;
         if (sh) hscroll = 0;

         box = &bc->bc_OuterBox;
      }
      else
      {
         vd->vd_Flags &= ~VDF_DIRECT;

         box = &bc->bc_InnerBox;
      };

      vw = box->Width;
      vh = box->Height;

      if (ow < vw) ow = vw;
      if (oh < vh) oh = vh;

      vd->vd_ObjectWidth  = ow;
      vd->vd_ObjectHeight = oh;

      vd->vd_X = range(vd->vd_X, 0, ow - vw);
      vd->vd_Y = range(vd->vd_Y, 0, oh - vh);

      if (hscroll) DoSetMethodNG(hscroll, PGA_Top, vd->vd_X, PGA_Total, ow, PGA_Visible, vw, TAG_DONE);
      if (vscroll) DoSetMethodNG(vscroll, PGA_Top, vd->vd_Y, PGA_Total, oh, PGA_Visible, vh, TAG_DONE);

      DoSetMethodNG(o, GA_Left, 0, GA_Top, 0, GA_Width, ow, GA_Height, oh, TAG_DONE);
      if ((gpr->gpr_Redraw == GREDRAW_REDRAW) || !vd->vd_ObjectBuffer)
      {
         if (vd->vd_ObjectBuffer) BGUI_FreeRPortBitMap(vd->vd_ObjectBuffer);

         if (!(vd->vd_ObjectBuffer = BGUI_CreateRPortBitMap(rp, ow, oh, FGetDepth(rp))))
            return 0;

         AsmDoMethod(o, GM_RENDER, gi, vd->vd_ObjectBuffer, GREDRAW_REDRAW);
      }

      if (hscroll
      &&  (!(vd->vd_Flags & VDF_DIRECT) 
      ||   !sw))
      {
         if (sw)
         {
            DoSetMethodNG(hscroll, GA_Left,     bc->bc_OuterBox.Left,
                                   GA_Top,      bc->bc_OuterBox.Top + bc->bc_OuterBox.Height - sh,
                                   GA_Width,    bc->bc_OuterBox.Width - sw,
                                   GA_Height,   sh,
                                   TAG_DONE);
         }
         AsmDoMethod(hscroll, GM_RENDER, gi, rp, gpr->gpr_Redraw);
      }
      if (vscroll
      &&  (!(vd->vd_Flags & VDF_DIRECT)
      ||   !sh))
      {
         if (sh)
         {
            DoSetMethodNG(vscroll, GA_Left,     bc->bc_OuterBox.Left + bc->bc_OuterBox.Width - sw,
                                   GA_Top,      bc->bc_OuterBox.Top,
                                   GA_Width,    sw,
                                   GA_Height,   bc->bc_OuterBox.Height - sh,
                                   TAG_DONE);
         }
         AsmDoMethod(vscroll, GM_RENDER, gi, rp, gpr->gpr_Redraw);
      }

      box = (vd->vd_Flags & VDF_DIRECT) ? &bc->bc_OuterBox : &bc->bc_InnerBox;

      vd->vd_X = range(vd->vd_X, 0, vd->vd_ObjectWidth  - box->Width);
      vd->vd_Y = range(vd->vd_Y, 0, vd->vd_ObjectHeight - box->Height);

      cr.MinX = box->Left;
      cr.MinY = box->Top;
      cr.MaxX = box->Left + box->Width  - 1;
      cr.MaxY = box->Top  + box->Height - 1;

      AsmDoMethod(obj, VIEW_CLIP, &cr, FALSE);
      ClipBlit(vd->vd_ObjectBuffer, vd->vd_X, vd->vd_Y,
               rp, cr.MinX, cr.MinY, cr.MaxX - cr.MinX + 1, cr.MaxY - cr.MinY + 1, 0xC0);

      rc = 1;
   };
   BGUI_PostRender(cl, obj, gpr);

   return rc;
}
METHOD_END
///

/*
 * Forward certain types of messages with modifications.
 */
ULONG ForwardViewMsg(Class *cl, Object *s, Object *d, Msg msg)
{
   VD            *vd = INST_DATA(cl, s);
   WORD          *mouse = NULL;
   ULONG          rc, storage;

   /*
    * Get address of mouse coordinates in message.
    */
   switch (msg->MethodID)
   {
   case GM_HITTEST:
   case GM_HELPTEST:
      mouse = (WORD *)&((struct gpHitTest *)msg)->gpht_Mouse;
      break;
   case GM_GOACTIVE:
   case GM_HANDLEINPUT:
      mouse = (WORD *)&((struct gpInput *)msg)->gpi_Mouse;
      break;
   };
   if (!mouse) return 0;

   /*
    * Store the coordinates.
    */
   storage = *(ULONG *)mouse;

   /*
    * Adjust the coordinates to be relative to the destination object.
    */
   mouse[0] += vd->vd_X;
   mouse[1] += vd->vd_Y;

   /*
    * Send the message to the destination object.
    */
   rc = AsmDoMethodA(d, msg);

   /*
    * Put the coordinates back to what they were originally.
    */
   *(ULONG *)mouse = storage;

   return rc;
}

/// GM_HITTEST
/*
 * Forward a message to the object.
 */
METHOD(ViewClassHitTest, struct gpHitTest *, gph)
{
   VD              *vd = INST_DATA(cl, obj);
   BC              *bc = BASE_DATA(obj);
   ULONG            rc = 0;

   /*
    * Get absolute click position.
    */
   WORD l = GADGET(obj)->LeftEdge + gph->gpht_Mouse.X;
   WORD t = GADGET(obj)->TopEdge  + gph->gpht_Mouse.Y;

   if (PointInBox(&bc->bc_InnerBox, l, t))
   {
      /*
       * Hit inside the view area?
       */
      rc = ForwardViewMsg(cl, obj, vd->vd_Object, (Msg)gph);
      if (rc != GMR_GADGETHIT)
      {
         if (!(vd->vd_Flags & (VDF_NO_DRAGMOVE|VDF_DIRECT)))
         {
            vd->vd_Flags |= VDF_DRAGMOVE;

            vd->vd_BX = vd->vd_X;
            vd->vd_BY = vd->vd_Y;
            vd->vd_LX = gph->gpht_Mouse.X;
            vd->vd_LY = gph->gpht_Mouse.Y;

            rc = GMR_GADGETHIT;
         };
      };

      /*
       * Take over the object's GadgetID.
       */
      GADGET(obj)->GadgetID = GADGET(vd->vd_Object)->GadgetID;
   };

   if (!rc && vd->vd_VScroller)
   {
      /*
       * Route the message.
       */
      rc = ForwardMsg(obj, vd->vd_VScroller, (Msg)gph);

      /*
       * Mark the scroller active.
       */
      if (rc == GMR_GADGETHIT) vd->vd_Flags |= VDF_VPROPACTIVE;
   };

   if (!rc && vd->vd_HScroller)
   {
      /*
       * Route the message.
       */
      rc = ForwardMsg(obj, vd->vd_HScroller, (Msg)gph);
      
      /*
       * Mark the scroller active.
       */
      if (rc == GMR_GADGETHIT) vd->vd_Flags |= VDF_HPROPACTIVE;
   };
   return rc;
}
METHOD_END
///
/// GM_HANDLEINPUT
/*
 * Handle input.
 */
METHOD(ViewClassHandleInput, struct gpInput *, gpi)
{
   VD                *vd = INST_DATA(cl, obj);
   struct InputEvent *ie;
   ULONG              rc;

   /*
    * Drag-moving?
    */
   if (vd->vd_Flags & VDF_DRAGMOVE)
   {
      ie = gpi->gpi_IEvent;

      // qualifier stuff here  return GMR_NOREUSE;

      if (ie->ie_Class == IECLASS_RAWMOUSE)
      {
         if (ie->ie_Code == SELECTUP)
         {
            return GMR_NOREUSE;
         };
         if (ie->ie_Code == MENUDOWN)
         {
            DoSetMethod(obj, gpi->gpi_GInfo, VIEW_X, vd->vd_BX, VIEW_Y, vd->vd_BY, TAG_DONE);

            return GMR_NOREUSE;
         };
      };
      //if (ie->ie_Class == IECLASS_POINTERPOS)
      {
         DoSetMethod(obj, gpi->gpi_GInfo, VIEW_X, vd->vd_BX - gpi->gpi_Mouse.X + vd->vd_LX,
                                          VIEW_Y, vd->vd_BY - gpi->gpi_Mouse.Y + vd->vd_LY, TAG_DONE);
      };

      return GMR_MEACTIVE;
   };

   /*
    * VScroller active?
    */
   if (vd->vd_Flags & VDF_VPROPACTIVE)
      return ForwardMsg(obj, vd->vd_VScroller, (Msg)gpi) & ~GMR_VERIFY;

   /*
    * HScroller active?
    */
   if (vd->vd_Flags & VDF_HPROPACTIVE)
      return ForwardMsg(obj, vd->vd_HScroller, (Msg)gpi) & ~GMR_VERIFY;

   if (vd->vd_Object)
   {
      /*
       * Route the message.
       */
      rc = ForwardViewMsg(cl, obj, vd->vd_Object, (Msg)gpi);

      /*
       * Take over the object's GadgetID.
       */
      *(gpi->gpi_Termination) =
      GADGET(obj)->GadgetID = GADGET(vd->vd_Object)->GadgetID;

      return rc;
   };
   return 0;
}
METHOD_END
///
/// GM_GOINACTIVE
/*
 * Go inactive.
 */
METHOD(ViewClassGoInactive, struct gpGoInactive *, gpgi)
{
   VD       *vd = INST_DATA(cl, obj);
   ULONG     rc = 0;

   if (vd->vd_Flags & VDF_DRAGMOVE)
   {
      vd->vd_Flags &= ~VDF_DRAGMOVE;
      return AsmDoSuperMethodA(cl, obj, (Msg)gpgi);
   };

   if (vd->vd_VScroller && (vd->vd_Flags & VDF_VPROPACTIVE))
   {
      /*
       * Mark the scroller as not active.
       */
      vd->vd_Flags &= ~VDF_VPROPACTIVE;
      /*
       * If the scroller was active pass this message on for compatibility reasons.
       */
      return AsmDoMethodA(vd->vd_VScroller, (Msg)gpgi);
   };

   if (vd->vd_HScroller && (vd->vd_Flags & VDF_HPROPACTIVE))
   {
      /*
       * Mark the scroller as not active.
       */
      vd->vd_Flags &= ~VDF_HPROPACTIVE;
      /*
       * If the scroller was active pass this message on for compatibility reasons.
       */
      return AsmDoMethodA(vd->vd_HScroller, (Msg)gpgi);
   };

   if (vd->vd_Object)
   {
      /*
       * Route the message.
       */
      rc = AsmDoMethodA(vd->vd_Object, (Msg)gpgi);

      /*
       * Take over the object's GadgetID.
       */
      GADGET(obj)->GadgetID = GADGET(vd->vd_Object)->GadgetID;
   };
   return rc;
}
METHOD_END
///
/// GRM_WHICHOBJECT
/*
 * Get the object under the mouse.
 */
METHOD(ViewClassWhichObject, struct grmWhichObject *, grwo)
{
   VD                    *vd = INST_DATA(cl, obj);
   BC                    *bc = BASE_DATA(obj);
   Object                *ob = vd->vd_Object;
   struct grmWhichObject  wo = *grwo;

   int x = grwo->grwo_Coords.X;
   int y = grwo->grwo_Coords.Y;
   int r = bc->bc_OuterBox.Left + bc->bc_OuterBox.Width  - vd->vd_ScrollerW;
   int b = bc->bc_OuterBox.Top  + bc->bc_OuterBox.Height - vd->vd_ScrollerH;

   if ((x >= r) && (y >= b));

   if (x >= r)
   {
      return (ULONG)vd->vd_VScroller;
   };
   if (y >= b)
   {
      return (ULONG)vd->vd_HScroller;
   };

   wo.MethodID = GRM_WHICHOBJECT;
   wo.grwo_Coords.X = x + vd->vd_X;
   wo.grwo_Coords.Y = y + vd->vd_Y;

   /*
    * Is object a group?
    */
   if (IsMulti(ob))
   {
      /*
       * Route message.
       */
      ob = (Object *)AsmDoMethodA(ob, (Msg)&wo);
   };
   return (ULONG)ob;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * Simple dimensions request.
 */
METHOD(ViewClassDimensions, struct bmDimensions *, bmd)
{
   VD              *vd = INST_DATA(cl, obj);
   struct BaseInfo *bi = bmd->bmd_BInfo;
   UWORD            ow, oh;
   ULONG            mw, mh;

   if (vd->vd_ScaleMinWidth || vd->vd_ScaleMinHeight)
   {
      AsmDoMethod(vd->vd_Object, GRM_DIMENSIONS, bi, bi->bi_RPort, &ow, &oh, 0);
   };

   if (vd->vd_ScaleMinWidth)
   {
      mw = ScaleWeight(ow, 100, vd->vd_ScaleMinWidth);
   }
   else
   {
      mw = vd->vd_MinWidth;
   };

   if (vd->vd_ScaleMinHeight)
   {
      mh = ScaleWeight(oh, 100, vd->vd_ScaleMinHeight);
   }
   else
   {
      mh = vd->vd_MinHeight;
   };

   if (mw < 8) mw = 8;
   if (mh < 8) mh = 8;

   /*
    * We simply add the specified minimum width and height
    * which are passed to us at create time, plus the scrollers.
    */
   return CalcDimensions(cl, obj, bmd, mw + vd->vd_ScrollerW, mh + vd->vd_ScrollerH);
}
METHOD_END
///
/// VIEW_CLIP

METHOD(ViewClassClip, struct vmClip *, vmc)
{
   VD          *vd = INST_DATA(cl, obj);
   BC          *bc = BASE_DATA(obj);
   struct IBox *box = (vd->vd_Flags & VDF_DIRECT) ? &bc->bc_OuterBox : &bc->bc_InnerBox;

   int     x1 = box->Left;
   int     y1 = box->Top;
   int     x2 = box->Width  + x1 - 1;
   int     y2 = box->Height + y1 - 1;

   if (vmc->vmc_Rectangle->MinX < x1) vmc->vmc_Rectangle->MinX = x1;
   if (vmc->vmc_Rectangle->MaxX > x2) vmc->vmc_Rectangle->MaxX = x2;
   if (vmc->vmc_Rectangle->MinY < y1) vmc->vmc_Rectangle->MinY = y1;
   if (vmc->vmc_Rectangle->MaxY > y2) vmc->vmc_Rectangle->MaxY = y2;

   if (vmc->vmc_WindowClip
   &&  vd->vd_ParentView)
      AsmDoMethodA(vd->vd_ParentView, (Msg)vmc);

   return 1;
}
METHOD_END
///
/// BASE_IS_MULTI
METHOD(ViewClassIsMulti, Msg, msg)
{
   return TRUE;
}
METHOD_END
///
/// Class initialization.
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   VIEW_CLIP,           (FUNCPTR)ViewClassClip,
   OM_NEW,              (FUNCPTR)ViewClassNew,
   OM_GET,              (FUNCPTR)ViewClassGet,
   OM_SET,              (FUNCPTR)ViewClassSet,
   OM_UPDATE,           (FUNCPTR)ViewClassSet,
   OM_DISPOSE,          (FUNCPTR)ViewClassDispose,
   GM_RENDER,           (FUNCPTR)ViewClassRender,
   BASE_DIMENSIONS,     (FUNCPTR)ViewClassDimensions,
   GRM_WHICHOBJECT,     (FUNCPTR)ViewClassWhichObject,
   GM_HITTEST,          (FUNCPTR)ViewClassHitTest,
   GM_GOACTIVE,         (FUNCPTR)ViewClassHandleInput,
   GM_HANDLEINPUT,      (FUNCPTR)ViewClassHandleInput,
   GM_GOINACTIVE,       (FUNCPTR)ViewClassGoInactive,
   BASE_MOVEBOUNDS,     (FUNCPTR)ViewClassForward,
   BASE_LOCALIZE,       (FUNCPTR)ViewClassForward,
   BASE_KEYLABEL,       (FUNCPTR)ViewClassForward,
   BASE_FINDKEY,        (FUNCPTR)ViewClassForward,
   BASE_SHOWHELP,       (FUNCPTR)ViewClassForward,
   BASE_INHIBIT,        (FUNCPTR)ViewClassForward,
   BASE_IS_MULTI,       (FUNCPTR)ViewClassIsMulti,
   DF_END,              NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitViewClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(VD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
