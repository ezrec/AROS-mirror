/*
 * @(#) $Header$
 *
 * BGUI library
 * buttonclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.3  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
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
 * Revision 42.0  2000/05/09 22:08:32  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:53:59  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.3  1999/07/31 01:54:49  mlemos
 * Ensured that the base object frame is restored before the object is
 * disposed if the frame object was overwritte by the button class.
 *
 * Revision 41.10.2.2  1998/12/06 23:01:41  mlemos
 * Fixed bug of select only buttons being deselected by keyboard activation.
 *
 * Revision 41.10.2.1  1998/11/17 16:05:36  mlemos
 * Fixed the return value of BASE_RENDER to always reflect image changes.
 *
 * Revision 41.10  1998/02/25 21:11:42  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:43  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.
#include "include/classdefs.h"

/*
 * Object instance data.
 */
typedef struct {
   ULONG              bd_Flags;        /* See below.                    */
   struct Image      *bd_Image;        /* Intuition image.              */
   struct Image      *bd_SelImage;     /* Selected intuition image.     */
   Object            *bd_Vector;       /* Vector image.                 */
   Object            *bd_SelVector;    /* Selected vector image.        */
   Object            *bd_StoreFrame;   /* Storage for frame object.     */
   UWORD              bd_ScaleWidth;   /* Design width of the vector.   */
   UWORD              bd_ScaleHeight;  /* Design height of the vector.  */
}  BD;

#define BDF_NO_DESELECT    (1<<0)      /* Don't deselect when selected. */
#define BDF_ENCLOSE        (1<<1)      /* Enclose image in frame.       */
#define BDF_SYSIMAGE       (1<<2)      /* This is a system image.       */
#define BDF_STORED_FRAME   (1<<3)      /* Stored base instance frame.   */
///
/// ButtonSetAttrs
/*
 * Change the object's attributes.
 */
METHOD(ButtonSetAttrs, struct opSet *, ops)
{
   BD              *bd = INST_DATA(cl, obj);
   BC              *bc = BASE_DATA(obj);
   struct TagItem  *tstate = ops->ops_AttrList, *tag;
   ULONG            data, attr;
   BOOL             vis = FALSE;

   /*
    * Evaluate tags.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (attr = tag->ti_Tag)
      {
      case VIT_VectorArray:
      case VIT_BuiltIn:
      case VIT_Pen:
      case VIT_DriPen:
         if (bd->bd_Vector)
         {
            DoSetMethodNG(bd->bd_Vector, attr, data, TAG_DONE);
            vis = TRUE;
         };
         break;

      case SYSIA_Which:
         if (bd->bd_Flags & BDF_SYSIMAGE)
         {
            DoSetMethodNG((Object *)bd->bd_Image, SYSIA_Which, data, TAG_DONE);
            break;
         };
         bd->bd_Image      = (struct Image *)BGUI_NewObject(BGUI_SYSTEM_IMAGE, SYSIA_Which, data, TAG_DONE);
         bd->bd_StoreFrame = bc->bc_Frame;
         bd->bd_Flags     |= BDF_STORED_FRAME;
         bc->bc_Frame      = NULL;
         bd->bd_SelImage   = NULL;
         bd->bd_Flags     |= BDF_SYSIMAGE;
         vis = TRUE;
         break;

      case BUTTON_Image:
      case BUTTON_SelectedImage:
         if (bd->bd_Flags & BDF_SYSIMAGE)
         {
            bc->bc_Frame      = bd->bd_StoreFrame;
            bd->bd_StoreFrame = NULL;
            bd->bd_Flags &= ~BDF_STORED_FRAME;
            if (bd->bd_Image) DisposeObject(bd->bd_Image);
            bd->bd_Flags &= ~BDF_SYSIMAGE;
         };
         if (attr == BUTTON_Image)
         {
            bd->bd_Image = (struct Image *)data;
            vis = TRUE;
         }
         else
         {
            bd->bd_SelImage = (struct Image *)data;
            vis = TRUE;
         };
         break;

      case BUTTON_Vector:
         if (bd->bd_Vector) DisposeObject(bd->bd_Vector);
         bd->bd_Vector = (Object *)data;
         vis = TRUE;
         break;

      case BUTTON_SelectedVector:
         if (bd->bd_SelVector) DisposeObject(bd->bd_SelVector);
         bd->bd_SelVector = (Object *)data;
         vis = TRUE;
         break;

      case BUTTON_ScaleMinWidth:
         bd->bd_ScaleWidth = data;
         break;

      case BUTTON_ScaleMinHeight:
         bd->bd_ScaleHeight = data;
         break;

      case BUTTON_SelectOnly:
         if (data) bd->bd_Flags |= BDF_NO_DESELECT;
         else      bd->bd_Flags &= ~BDF_NO_DESELECT;
         break;

      case BUTTON_EncloseImage:
         if (data) bd->bd_Flags |= BDF_ENCLOSE;
         else      bd->bd_Flags &= ~BDF_ENCLOSE;
         break;
      };
   };

   /*
    * Force the correct activation flags.
    */
   if (GADGET(obj)->Activation & GACT_TOGGLESELECT)
   {
      GADGET(obj)->Activation |= GACT_IMMEDIATE;
      GADGET(obj)->Activation &= ~GACT_RELVERIFY;
   }
   else
   {
      GADGET(obj)->Activation &= ~GACT_IMMEDIATE;
      GADGET(obj)->Activation |= GACT_RELVERIFY;
   }
   return vis;
}
METHOD_END
///
/// OM_NEW
/*
 * Create a new object.
 */
METHOD(ButtonClassNew, struct opSet *, ops)
{
   BD               *bd;
   struct TagItem   *tags;
   ULONG             rc;
   struct opSet      opn = *ops;
   
   tags = DefTagList(BGUI_BUTTON_GADGET, ops->ops_AttrList);

   /*
    * First we let the superclass
    * create an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      /*
       * Get the instance data.
       */
      bd = INST_DATA(cl, rc);

      opn.ops_AttrList = tags;
      ButtonSetAttrs(cl, (Object *)rc, &opn);

      /*
       * See if we get a vector image.
       */
      if (!bd->bd_Image && !bd->bd_Vector)
         bd->bd_Vector = CreateVector(tags);
   };
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_SET, OM_UPDATE
/*
 * Change the object's attributes.
 */
METHOD(ButtonClassSetUpdate, struct opUpdate *, opu)
{
   BD              *bd = INST_DATA(cl, obj);
   WORD             dis = GADGET(obj)->Flags & GFLG_DISABLED;
   WORD             sel = GADGET(obj)->Flags & GFLG_SELECTED;
   BOOL             vis;

   /*
    * First we let the superclass do it's thing.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * F*ck interim messages.
    */
   if (opu->MethodID == OM_UPDATE && (opu->opu_Flags & OPUF_INTERIM))
      return 1;

   vis = ButtonSetAttrs(cl, obj, (struct opSet *)opu);

   /*
    * Visual change necessary?
    */
   if (((GADGET(obj)->Flags & GFLG_DISABLED) != dis) ||
       ((GADGET(obj)->Flags & GFLG_SELECTED) != sel) || vis)
   {
      DoRenderMethod(obj, opu->opu_GInfo, GREDRAW_REDRAW);
   }

   /*
    * Notify target when a toggle gadget's selected state changed.
    */
   if (GADGET(obj)->Activation & GACT_TOGGLESELECT)
   {
      if ((GADGET(obj)->Flags & GFLG_SELECTED) != sel)
      {
         /*
          * Special MX magic...
          */
         if (!(sel && (bd->bd_Flags & BDF_NO_DESELECT)))
         {
            DoNotifyMethod(obj, opu->opu_GInfo, 0, GA_ID,       GADGET(obj)->GadgetID,
                                GA_Selected, !sel, GA_UserData, GADGET(obj)->UserData, TAG_END);
         };
      };
   };

   return 1;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the object.
 */
METHOD(ButtonClassRender, struct bmRender *, bmr)
{
   BD                *bd = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   struct BaseInfo   *bi = bmr->bmr_BInfo;
   struct Image      *image = NULL;
   Object            *vector = NULL;
   int                x, y;
   ULONG              rc;
   ULONG              state = GadgetState(bi, obj, FALSE);

   /*
    * Render the baseclass.
    */
   rc=AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Pick up the image to render.
    */
   if (GADGET(obj)->Flags & GFLG_SELECTED)
   {
      image  = bd->bd_SelImage;
      vector = bd->bd_SelVector;
   };
   if (!image)  image  = bd->bd_Image;
   if (!vector) vector = bd->bd_Vector;

   /*
    * Setup and render the gadget.
    */
   if (image)
   {
      if (bd->bd_Flags & BDF_SYSIMAGE)
      {
         /*
          * Setup the image dimensions.
          */
         image->LeftEdge = bc->bc_OuterBox.Left;
         image->TopEdge  = bc->bc_OuterBox.Top;
         image->Width    = bc->bc_OuterBox.Width;
         image->Height   = bc->bc_OuterBox.Height;

         x = y = 0;
      }
      else
      {
         /*
          * Setup the left and top edge.
          */
         image->LeftEdge = bc->bc_InnerBox.Left;
         image->TopEdge  = bc->bc_InnerBox.Top;

         /*
          * Center it in the hitbox.
          */
         x = (bc->bc_InnerBox.Width  - image->Width ) >> 1;
         y = (bc->bc_InnerBox.Height - image->Height) >> 1;
      };
      /*
       * Render it.
       */
      BDrawImageState(bi, (Object *)image, x, y, state);

      rc = 1;
   }
   else if (vector)
   {
      /*
       * Setup vector bounds.
       */
      DoSetMethodNG(vector,
         IA_Left,    bc->bc_InnerBox.Left  - 1,  IA_Top,     bc->bc_InnerBox.Top    - 1,
         IA_Width,   bc->bc_InnerBox.Width + 2,  IA_Height,  bc->bc_InnerBox.Height + 2,
         TAG_END);

      /*
       * Render it.
       */
      rc = AsmDoMethod(vector, BASE_RENDER, bi, state);
   };

   /*
    * Disabled?
    */
   if (GADGET(obj)->Flags & GFLG_DISABLED)
      BDisableBox(bi, &bc->bc_HitBox);

   return rc;
}
METHOD_END
///
/// GM_GOACTIVE
/*
 * We must go active.
 */
//STATIC ASM ULONG ButtonClassGoActive( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpInput *gpi )
STATIC ASM REGFUNC3(ULONG, ButtonClassGoActive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi))
{
   BD                   *bd = INST_DATA(cl, obj);
   ULONG                 rc;
   struct GadgetInfo    *gi = gpi->gpi_GInfo;

   /*
    * We cannot go active when
    * we are disabled.
    */
   if (GADGET(obj)->Flags & GFLG_DISABLED)
      return GMR_NOREUSE;

   /*
    * We must be triggered by a mouse click
    * or something simular.
    */
   if (gpi->gpi_IEvent)
   {
      /*
       * Let the superclass have a go...
       */
      AsmDoSuperMethodA(cl, obj, (Msg)gpi);

      /*
       * Toggle gadget?
       */
      if (GADGET(obj)->Activation & GACT_TOGGLESELECT)
      {
         /*
          * Special MX magic...
          */
         if ((bd->bd_Flags & BDF_NO_DESELECT) && (GADGET( obj )->Flags & GFLG_SELECTED))
            return GMR_NOREUSE;

         /*
          * Toggle selected bit.
          */
         GADGET(obj)->Flags ^= GFLG_SELECTED;

         /*
          * Redraw and notify.
          */
         DoRenderMethod(obj, gi, GREDRAW_REDRAW);
         DoNotifyMethod(obj, gi, 0, GA_ID,       GADGET(obj)->GadgetID,
                                    GA_Selected, GADGET(obj)->Flags & GFLG_SELECTED,
                                    GA_UserData, GADGET(obj)->UserData, TAG_DONE);

         rc = GMR_NOREUSE | GMR_VERIFY;
      }
      else
      {
         /*
          * Normal buttons require input.
          */
         GADGET(obj)->Flags |= GFLG_SELECTED;
         DoRenderMethod(obj, gpi->gpi_GInfo, GREDRAW_REDRAW);
         rc = GMR_MEACTIVE;
      }
   } else
      rc = GMR_NOREUSE;

   return rc;
}
REGFUNC_END

///
/// GM_HANDLEINPUT
/*
 * Handle button gadget input.
 */
//STATIC ASM ULONG ButtonClassHandleInput( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpInput *gpi )
STATIC ASM REGFUNC3(ULONG, ButtonClassHandleInput,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *,gpi))
{
   WORD                  sel = 0;
   struct gpHitTest      gph;
   struct GadgetInfo    *gi = gpi->gpi_GInfo;
   ULONG                 rc = GMR_MEACTIVE;

   /*
    * Mouse over the object?
    */
   gph.MethodID      = GM_HITTEST;
   gph.gpht_GInfo    = gi;
   gph.gpht_Mouse.X  = gpi->gpi_Mouse.X;
   gph.gpht_Mouse.Y  = gpi->gpi_Mouse.Y;

   if (AsmDoMethodA(obj, (Msg)&gph) == GMR_GADGETHIT)
      sel = GFLG_SELECTED;
   
   /*
    * Check mouse input.
    */
   if (gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
   {
      switch (gpi->gpi_IEvent->ie_Code)
      {
         case MIDDLEUP:
         case MENUUP:
         case SELECTUP:
            /*
             * When we are selected we verify.
             */
            if (sel)
            {
               /*
                * Send final notification.
                */
               DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, GA_UserData, GADGET(obj)->UserData, TAG_END);
               rc = GMR_NOREUSE|GMR_VERIFY;
            }
            else
            {
               rc = GMR_NOREUSE;
            };
            sel = 0;
            break;

         case MIDDLEDOWN:
         case MENUDOWN:
         case SELECTDOWN:
            /*
             * Another button aborts the selection.
             */
            DoSetMethodNG(obj, BT_ReportID, FALSE, TAG_DONE);
            rc = GMR_NOREUSE;
            sel = 0;
            break;
      }
   } else if ( gpi->gpi_IEvent->ie_Class == IECLASS_TIMER ) {
      /*
       * Every timer tick will result in an interim
       * notification while the object is selected.
       */
      if (sel)
         DoNotifyMethod(obj, gi, OPUF_INTERIM, GA_ID, GADGET(obj)->GadgetID, GA_UserData, GADGET(obj)->UserData, TAG_END);
   }

   /*
    * Visual change?
    */
   if ((GADGET(obj)->Flags & GFLG_SELECTED) != sel)
   {
      /*
       * Flip selected bit and redraw.
       */
      GADGET(obj)->Flags ^= GFLG_SELECTED;
      DoRenderMethod(obj, gi, GREDRAW_REDRAW);
   }

   return rc;
}
REGFUNC_END
///
/// OM_GET
/*
 * They want to know something.
 */
METHOD(ButtonClassGet, struct opGet *, opg)
{
   ULONG       rc = 1, *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   /*
    * We make GA_Selected OM_GET'able.
    */
   case GA_Selected:
      STORE (GADGET(obj)->Flags & GFLG_SELECTED);
      break;

   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   }
   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Dispose of ourselves.
 */
METHOD(ButtonClassDispose, Msg, msg)
{
   BD    *bd = INST_DATA(cl, obj);
   BC    *bc = BASE_DATA(obj);

   if ((bd->bd_Flags & BDF_SYSIMAGE) && bd->bd_Image)
      DisposeObject(bd->bd_Image);

   if(bd->bd_Flags & BDF_STORED_FRAME)
      bc->bc_Frame = bd->bd_StoreFrame;
      
   if (bd->bd_Vector)
      DisposeObject(bd->bd_Vector);

   if (bd->bd_SelVector)
      DisposeObject(bd->bd_SelVector);

   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// WM_KEYACTIVE
/*
 * We are activated by a key.
 */
METHOD(ButtonClassKeyActive, struct wmKeyInput *, wmki)
{
   ULONG              rc = WMKF_MEACTIVE;
   struct GadgetInfo *gi = wmki->wmki_GInfo;
   BD                *bd = INST_DATA(cl, obj);
   /*
    * If we are toggle-select we do not
    * need to go active.
    */
   if (GADGET(obj)->Activation & GACT_TOGGLESELECT)
   {
      /*
       * Toggle-select buttons do not
       * respond to repeated keys.
       */
      if (!(wmki->wmki_IEvent->ie_Qualifier & IEQUALIFIER_REPEAT)
      && (!(bd->bd_Flags & BDF_NO_DESELECT))
      || !(GADGET(obj)->Flags & GFLG_SELECTED))
      {
         /*
          * Flip selected bit.
          */
         GADGET(obj)->Flags ^= GFLG_SELECTED;

         /*
          * Re-render.
          */
         DoRenderMethod(obj, gi, GREDRAW_REDRAW );

         /*
          * Notify our state change.
          */
         DoNotifyMethod(obj, gi, 0, GA_ID, GADGET( obj )->GadgetID, GA_Selected, GADGET( obj )->Flags & GFLG_SELECTED ? TRUE : FALSE, GA_UserData, GADGET( obj )->UserData, TAG_END );

         /*
          * Setup our ID.
          */
         *(wmki->wmki_ID) = GADGET(obj)->GadgetID;

         rc = WMKF_VERIFY;
      }
      else
         rc = WMKF_CANCEL;
   }
   else
   {
      /*
       * Flip to selected.
       */
      GADGET(obj)->Flags |= GFLG_SELECTED;

      /*
       * Re-render.
       */
      DoRenderMethod(obj, gi, GREDRAW_REDRAW);
   }
   return rc;
}
METHOD_END
///
/// WM_KEYINPUT
/*
 * Handle key input messages.
 */
METHOD(ButtonClassKeyInput, struct wmKeyInput *, wmki)
{
   ULONG              rc   = WMKF_MEACTIVE;
   UWORD              qual = wmki->wmki_IEvent->ie_Qualifier;
   UWORD              code = wmki->wmki_IEvent->ie_Code, key;
   struct GadgetInfo *gi   = wmki->wmki_GInfo;

   /*
    * Repeat mode?
    */
   if (!(qual & IEQUALIFIER_REPEAT))
   {
      /*
       * A key released?
       */
      if (code & IECODE_UP_PREFIX)
      {
         /*
          * Check which key is released.
          */
         key = MapKey(code & ~IECODE_UP_PREFIX, qual, &wmki->wmki_IEvent->ie_EventAddress);

         /*
          * Is it the one that activated us?
          */
         if (*wmki->wmki_Key == key || *wmki->wmki_Key == tolower(key))
         {
            /*
             * Notify our target.
             */
            DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, GA_UserData, GADGET(obj)->UserData, TAG_END );

            /*
             * Setup our ID.
             */
            *(wmki->wmki_ID) = GADGET(obj)->GadgetID;
            rc = WMKF_VERIFY;
         }
      }
      else if ((qual & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) || wmki->wmki_IEvent->ie_Code == 0x45)
      {
         /*
          * We have been cancelled by either a SHIFT key
          * or the ESC key.
          */
         rc = WMKF_CANCEL;
      }
   }
   else
      /*
       * Repeated key messages will trigger
       * an interim notification.
       */
      DoNotifyMethod( obj, gi, OPUF_INTERIM, GA_ID, GADGET(obj)->GadgetID, GA_Selected, GADGET(obj)->Flags & GFLG_SELECTED ? TRUE : FALSE, GA_UserData, GADGET( obj )->UserData, TAG_END );

   return rc;
}
METHOD_END
///
/// WM_KEYINACTIVE
/*
 * We are forced to de-activate.
 */
METHOD(ButtonClassKeyInActive, struct wmKeyInActive *, wmkia)
{
   /*
    * Change visuals to un-selected when
    * necessary.
    */
   if (!(GADGET(obj)->Activation & GACT_TOGGLESELECT))
   {
      if (GADGET(obj)->Flags & GFLG_SELECTED)
      {
         GADGET(obj)->Flags &= ~GFLG_SELECTED;
         DoRenderMethod( obj, wmkia->wmkia_GInfo, GREDRAW_REDRAW );
      }
   }

   return 0;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * They want our minimum dimensions.
 */
METHOD(ButtonClassDimensions, struct bmDimensions *, bmd)
{
   BD                   *bd = INST_DATA(cl, obj);
   struct BaseInfo      *bi = bmd->bmd_BInfo;
   Object               *temp;
   int                   rx, ry;
   UWORD                 mx = 1, my = 1;
   ULONG                 tmp;

   if (bd->bd_Image || bd->bd_SelImage)
   {
      /*
       * Get minimum sizes of the available images.
       */
      if (bd->bd_Image)
      {
         if (mx < bd->bd_Image->Width)  mx = bd->bd_Image->Width;
         if (my < bd->bd_Image->Height) my = bd->bd_Image->Height;
      };

      /*
       * Did we get a selected image?
       */
      if (bd->bd_SelImage)
      {
         if (mx < bd->bd_SelImage->Width)  mx = bd->bd_SelImage->Width;
         if (my < bd->bd_SelImage->Height) my = bd->bd_SelImage->Height;
      };

      if (!(bd->bd_Flags & BDF_ENCLOSE))
      {
         mx += 4;
         my += 2;
      };

      if (bd->bd_Flags & BDF_SYSIMAGE)
      {
         mx = my = 8;

         Get_Attr((Object *)bd->bd_Image, SYSIA_Which, &tmp);

         temp = NewObject(NULL, "sysiclass", SYSIA_Which, tmp, SYSIA_DrawInfo, bi->bi_DrInfo,
                   SYSIA_Size, (bi->bi_IScreen->Flags & SCREENHIRES) ? SYSISIZE_MEDRES : SYSISIZE_LOWRES, TAG_DONE);

         if (temp)
         {
            if (mx < IMAGE(temp)->Width)  mx = IMAGE(temp)->Width;
            if (my < IMAGE(temp)->Height) my = IMAGE(temp)->Height;

            DisposeObject(temp);
         };
      };
   }
   else if (bd->bd_Vector || bd->bd_SelVector)
   {
      /*
       * Scaled button?
       */
      if (bd->bd_ScaleWidth && bd->bd_ScaleHeight)
      {
         if (bi->bi_DrInfo)
         {
            rx = bi->bi_DrInfo->dri_Resolution.X;
            ry = bi->bi_DrInfo->dri_Resolution.Y;
         }
         else
         {
            rx = ry = 22;
         };
         
         my = bi->bi_RPort->TxHeight;
         mx = (my * ry * bd->bd_ScaleWidth) / (rx * bd->bd_ScaleHeight);
      }
      else
      {
         /*
          * Get minimum vector sizes.
          */
         if (bd->bd_Vector)
         {
            Get_Attr(bd->bd_Vector, VIT_MinWidth, &tmp);
            if (mx < tmp) mx = tmp;
            Get_Attr(bd->bd_Vector, VIT_MinHeight, &tmp);
            if (my < tmp) my = tmp;
         };

         if (bd->bd_SelVector)
         {
            Get_Attr(bd->bd_SelVector, VIT_MinWidth, &tmp);
            if (mx < tmp) mx = tmp;
            Get_Attr(bd->bd_SelVector, VIT_MinHeight, &tmp);
            if (my < tmp) my = tmp;
         };
         mx -= 2;
         my -= 2;
      };
   }
   else
   {
      /*
       * Normal button. When BUTTON_EncloseImage is
       * specified we make the label enclosed in the frame.
       */
      mx = (bd->bd_Flags & BDF_ENCLOSE) ? 0 : 4;
      my = (bd->bd_Flags & BDF_ENCLOSE) ? 0 : 2;
   };

   /*
    * Compute dimensions and add our values.
    */
   return CalcDimensions(cl, obj, bmd, mx, my);
}
METHOD_END
///
/// BASE_FINDKEY
METHOD(ButtonClassFindKey, struct bmFindKey *, bmfk)
{
   if (bmfk->bmfk_Key.Qual & IEQUALIFIER_REPEAT)
      return 0;
   else
      return AsmDoSuperMethodA(cl, obj, (Msg)bmfk);
}
METHOD_END
///
/// Class initialization.
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,            (FUNCPTR)ButtonClassRender,
   GM_GOACTIVE,            (FUNCPTR)ButtonClassGoActive,
   GM_HANDLEINPUT,         (FUNCPTR)ButtonClassHandleInput,

   OM_SET,                 (FUNCPTR)ButtonClassSetUpdate,
   OM_UPDATE,              (FUNCPTR)ButtonClassSetUpdate,
   OM_GET,                 (FUNCPTR)ButtonClassGet,
   OM_NEW,                 (FUNCPTR)ButtonClassNew,
   OM_DISPOSE,             (FUNCPTR)ButtonClassDispose,
   
   WM_KEYACTIVE,           (FUNCPTR)ButtonClassKeyActive,
   WM_KEYINPUT,            (FUNCPTR)ButtonClassKeyInput,
   WM_KEYINACTIVE,         (FUNCPTR)ButtonClassKeyInActive,

   BASE_DIMENSIONS,        (FUNCPTR)ButtonClassDimensions,
   BASE_FINDKEY,           (FUNCPTR)ButtonClassFindKey,
   DF_END,                 NULL,
};

/*
 * Simple class initialization.
 */
makeproto Class *InitButtonClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(BD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
