/*
 * @(#) $Header$
 *
 * BGUI library
 * propclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.4  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.3  2000/08/17 15:09:18  chodorowski
 * Fixed compiler warnings.
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
 * Revision 42.0  2000/05/09 22:09:56  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:55  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.5  2000/01/30 20:41:12  mlemos
 * Fixed bug of not adding the arrows size to the minimum and nominal
 * dimensions of the gadget.
 *
 * Revision 41.10.2.4  1998/12/06 21:40:53  mlemos
 * Ensured that when the parent view and window are passed to the arrow
 * gadgets on their creation.
 *
 * Revision 41.10.2.3  1998/11/13 18:21:53  mlemos
 * Reverted the workaround patch that set the screen pointer in the BaseInfo
 * structure as the real problem was fixed AllocBaseInfo function.
 *
 * Revision 41.10.2.2  1998/11/03 10:45:28  mlemos
 * Added workaround to avoid enforcer hits cause by ROM propclass looking at
 * the GadgetInfo Screen pointer inside GM_RENDER method.
 *
 * Revision 41.10.2.1  1998/10/01 04:38:52  mlemos
 * Fixed bug of sending interim notifications after adjusting the knob.
 *
 * Revision 41.10  1998/02/25 21:12:53  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:29  mlemos
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
   ULONG           pd_Flags;        /* see below                 */
   Object         *pd_Prop;         /* prop gadget object        */
   Object         *pd_Knob;         /* prop gadget knob          */
   Object         *pd_Arrow1;       /* up/left arrow             */
   Object         *pd_Arrow2;       /* down/right arrow          */
   LONG            pd_Top;          /* prop to position          */
   LONG            pd_Total;        /* prop total range          */
   LONG            pd_Visible;      /* prop visible portion      */
   LONG            pd_Min;          /* minimum level             */
   LONG            pd_Max;          /* maximum level             */
   LONG            pd_Level;        /* current level             */
   LONG            pd_Reset1;       /* Initial top value         */
   LONG            pd_Reset2;       /* Initial top level         */
   UWORD           pd_ArrowSize;    /* size of arrows            */
   UWORD           pd_RptTicks;     /* repeat treshold           */
}  PD;

#define PDF_ARROWS         (1<<0)         /* prop has arrows         */
#define PDF_NEWLOOK        (1<<1)         /* prop must be "NewLook"  */
#define PDF_PROPHORIZ      (1<<2)         /* prop moves horizontally */
#define PDF_PROPACTIVE     (1<<3)         /* prop is active          */
#define PDF_LEFT_UP        (1<<4)         /* left/up gadget active   */
#define PDF_RIGHT_DOWN     (1<<5)         /* right/down active       */
#define PDF_RESET          (1<<6)         /* Reset to pd_Initial     */
#define PDF_SLIDER         (1<<7)         /* Slider mode.            */
#define PDF_READY          (1<<8)         /* Created and ready.      */

///

/*
 * Calculate arrow and prop positions.
 */
STATIC VOID CalcPropMuck(Class *cl, Object *obj, struct BaseInfo *bi)
{
   PD              *pd = INST_DATA(cl, obj);
   BC              *bc = BASE_DATA(obj);
   int              size = pd->pd_ArrowSize;
   ULONG            kw, kh;
   struct IBox      arrow1, arrow2;

   /*
    * Check for arrows and where they are placed.
    */
   if (pd->pd_Flags & PDF_ARROWS)
   {
      /*
       * Copy the hitbox to the destinations
       */
      arrow1 = arrow2 = bc->bc_HitBox;

      /*
       * FREEHORIZ will get the arrows
       * on the right side. FREEVERT will
       * get them below.
       */
      if (pd->pd_Flags & PDF_PROPHORIZ)
      {
	 arrow1.Left  += bc->bc_HitBox.Width;
	 arrow2.Left   = arrow1.Left + size;
	 arrow1.Width  = arrow2.Width = size;
      }
      else
      {
	 arrow1.Top   += bc->bc_HitBox.Height;
	 arrow2.Top    = arrow1.Top + size;
	 arrow1.Height = arrow2.Height = size;
      }
      SetGadgetBounds(pd->pd_Arrow1, &arrow1);
      SetGadgetBounds(pd->pd_Arrow2, &arrow2);
   }

   /*
    * Setup the slider.
    */

   if (pd->pd_Flags & PDF_SLIDER)
   {
      if (pd->pd_Flags & PDF_PROPHORIZ) pd->pd_Top = pd->pd_Level - pd->pd_Min;
      else                              pd->pd_Top = pd->pd_Max - pd->pd_Level;
   };

   /*
    * Set it up.
    */
   DoSetMethodNG(pd->pd_Prop, GA_Left,     bc->bc_InnerBox.Left,  GA_Top,    bc->bc_InnerBox.Top,
			      GA_Width,    bc->bc_InnerBox.Width, GA_Height, bc->bc_InnerBox.Height,
			      PGA_Top,     pd->pd_Top,            PGA_Total, pd->pd_Total,
			      PGA_Visible, pd->pd_Visible,        TAG_DONE);

   if (pd->pd_Knob)
   {
      kw = bc->bc_InnerBox.Width;
      kh = bc->bc_InnerBox.Height;

      if ((pd->pd_Total > 0) && (pd->pd_Total > pd->pd_Visible))
      {
	 if (pd->pd_Flags & PDF_PROPHORIZ)
	 {
	    kw = (kw * (ULONG)pd->pd_Visible) / pd->pd_Total;
	    if (kw < 4) kw = 4;
	 }
	 else
	 {
	    kh = (kh * (ULONG)pd->pd_Visible) / pd->pd_Total;
	    if (kh < 4) kh = 4;
	 }
      };
      DoSetMethodNG(pd->pd_Knob, IA_Width, kw, IA_Height, kh, TAG_DONE);
   };
}

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(PropClassNew, struct opSet *, ops)
{
   PD             *pd;
   BC             *bc;
   struct TagItem *tags;
   ULONG           rc;
   BOOL            horiz;

   tags = DefTagList(BGUI_PROP_GADGET, ops->ops_AttrList);

   /*
    * First we let the superclass get us an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      /*
       * Obtain instance data.
       */
      pd = INST_DATA(cl, rc);
      bc = BASE_DATA(rc);

      if (!bc->bc_Frame) goto failure;

      /*
       * Make sure we are GACT_RELVERIFY.
       */
      GADGET(rc)->Activation |= GACT_RELVERIFY;

      /*
       * Setup defaults.
       */
      pd->pd_Visible = 1;
      pd->pd_Total   = 1;
      pd->pd_Flags   = GetTagData(PGA_Slider, FALSE, tags) ? PDF_SLIDER|PDF_PROPHORIZ : PDF_ARROWS;
      pd->pd_Max     = 15;

      AsmCoerceMethod(cl, (Object *)rc, OM_SET, tags, NULL);

      horiz = pd->pd_Flags & PDF_PROPHORIZ;

      /*
       * Arrows?
       */
      if (pd->pd_Flags & PDF_ARROWS)
      {
	 pd->pd_Arrow1 = BGUI_NewObject(BGUI_BUTTON_GADGET,
					VIT_BuiltIn, horiz ? BUILTIN_ARROW_LEFT : BUILTIN_ARROW_UP,
					BT_ParentView,bc->bc_View,
					BT_ParentWindow,bc->bc_Window,
					TAG_DONE);

	 pd->pd_Arrow2 = BGUI_NewObject(BGUI_BUTTON_GADGET,
					VIT_BuiltIn, horiz ? BUILTIN_ARROW_RIGHT : BUILTIN_ARROW_DOWN,
					BT_ParentView,bc->bc_View,
					BT_ParentWindow,bc->bc_Window,
					TAG_DONE);

	 if (!(pd->pd_Arrow1 && pd->pd_Arrow2)) goto failure;
      };

      if (pd->pd_Prop = NewObject(NULL, PropGClass,
					GA_RelVerify,   TRUE,
					PGA_Borderless, TRUE,
					PGA_Freedom,    horiz ? FREEHORIZ : FREEVERT,
					TAG_MORE,       tags))
      {
	 /*
	 pd->pd_Knob = BGUI_NewObject(BGUI_FRAME_IMAGE,
			  FRM_Type,        FRTYPE_FUZZ_RIDGE,
			  //FRM_FrameWidth,  2,
			  //FRM_FrameHeight, 2,
			  FRM_ThinFrame,   TRUE,
			  FRM_BackFill,    SHINE_RASTER,
			  IMAGE_InBorder,  GADGET(rc)->Activation & BORDERMASK,
			  TAG_END);
	 */


	 /*
	  * No disabled.
	  */
	 DoSetMethodNG(pd->pd_Prop, GA_Disabled, FALSE, TAG_END);

	 if (pd->pd_Knob)
	 {
	    DoSetMethodNG(pd->pd_Prop, GA_Image, pd->pd_Knob, GA_SelectRender, pd->pd_Knob, TAG_DONE);
	 };

	 /*
	  * We target the IDCMP port (when allowed).
	  */
	 if (!GetTagData(PGA_DontTarget, FALSE, tags))
	    DoSuperSetMethodNG(cl, (Object *)rc, ICA_TARGET, ICTARGET_IDCMP, TAG_END);

	 pd->pd_Flags |= PDF_READY;

	 /*
	  * So far, so good....
	  */
	 FreeTagItems(tags);
	 return rc;
      }
      failure:
      /*
       * Shit! Something screwed up...
       */
      AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
   }
   FreeTagItems(tags);
   return NULL;
}
METHOD_END
///
/// OM_SET, OM_UPDATE
/*
 * Change some attributes.
 */
METHOD(PropClassSetUpdate, struct opUpdate *, opu)
{
   PD             *pd = INST_DATA(cl, obj);
   BC             *bc = BASE_DATA(obj);
   struct TagItem *tstate = opu->opu_AttrList, *tag;
   ULONG           data, type, redraw = 0, ho, vo;
   LONG            tmp, val, omin, omax, olev, oldtop, oldtot, oldvis;
   BOOL            fc = !(pd->pd_Flags & PDF_READY);
   WORD            dis = GADGET(obj)->Flags & GFLG_DISABLED;

   /*
    * First we let the superclass do it's thing.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * Make sure we stay GACT_RELVERIFY.
    */
   GADGET(obj)->Activation |= GACT_RELVERIFY;

   if (pd->pd_Flags & PDF_SLIDER)
   {
      omin = pd->pd_Min;
      omax = pd->pd_Max;
      olev = pd->pd_Level;
   }
   else
   {
      oldtop = pd->pd_Top;
      oldtot = pd->pd_Total;
      oldvis = pd->pd_Visible;
   };

   /*
    * Let's see if we need to change
    * some known attributes ourselves.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case PGA_Top:
	 pd->pd_Top = data;
	 break;

      case PGA_Total:
	 pd->pd_Total = data;
	 break;

      case PGA_Visible:
	 pd->pd_Visible = data;
	 break;

      case PGA_Freedom:
	 if (data == FREEHORIZ) pd->pd_Flags |= PDF_PROPHORIZ;
	 else                   pd->pd_Flags &= ~PDF_PROPHORIZ;
	 break;

      case PGA_Arrows:
	 if (data) pd->pd_Flags |= PDF_ARROWS;
	 else      pd->pd_Flags &= ~PDF_ARROWS;
	 break;

      case PGA_ArrowSize:
	 pd->pd_ArrowSize = data;
	 break;

      case SLIDER_Level:
	 pd->pd_Level  = data;
	 break;

      case SLIDER_Min:
	 pd->pd_Min = data;
	 break;

      case SLIDER_Max:
	 pd->pd_Max = data;
	 break;

      case PGA_XenFrame:
	 if (data) data = FRTYPE_XEN_BUTTON;
	 else      data = FRTYPE_BUTTON;
	 goto set_frametype;

      case PGA_NoFrame:
      case SLIDER_NoFrame:
	 if (data) data = FRTYPE_NONE;
	 else      data = FRTYPE_BUTTON;

      case FRM_Type:
      set_frametype:
	 DoMultiSet(FRM_Type, data, 3, bc->bc_Frame, pd->pd_Arrow1, pd->pd_Arrow2);
	 fc = TRUE;
	 break;

      case PGA_ThinFrame:
      case SLIDER_ThinFrame:
      case FRM_ThinFrame:
	 DoMultiSet(FRM_ThinFrame, data, 3, bc->bc_Frame, pd->pd_Arrow1, pd->pd_Arrow2);
	 fc = TRUE;
	 break;

      case GA_TopBorder:
      case GA_BottomBorder:
      case GA_LeftBorder:
      case GA_RightBorder:
	 DoMultiSet(tag->ti_Tag, data, 4, pd->pd_Arrow1, pd->pd_Arrow2, pd->pd_Knob, pd->pd_Prop);

	 if (data)
	 {
	    DoSetMethodNG(bc->bc_Frame,  FRM_Type, FRTYPE_BORDER, FRM_EdgesOnly, TRUE, TAG_DONE);

	    DoSetMethodNG(pd->pd_Arrow1, BT_FrameObject, NULL,
	       SYSIA_Which, (pd->pd_Flags & PDF_PROPHORIZ) ? LEFTIMAGE : UPIMAGE, TAG_DONE);

	    DoSetMethodNG(pd->pd_Arrow2, BT_FrameObject, NULL,
	       SYSIA_Which, (pd->pd_Flags & PDF_PROPHORIZ) ? RIGHTIMAGE : DOWNIMAGE, TAG_DONE);
	 };
	 fc = TRUE;
	 break;

      case BT_ParentWindow:
      case BT_ParentView:
	 DoMultiSet(tag->ti_Tag, data, 3, pd->pd_Arrow1, pd->pd_Arrow2, pd->pd_Knob);
	 break;
      };
   };

   if (pd->pd_Flags & PDF_SLIDER)
   {
      /*
       * Stupidity tests.
       */
      if (pd->pd_Min > pd->pd_Max)
      {
	 tmp        = pd->pd_Min;
	 pd->pd_Min = pd->pd_Max;
	 pd->pd_Max = tmp;
      };

      /*
       * Calculate the absolute difference between min and max.
       */
      pd->pd_Total   = pd->pd_Max - pd->pd_Min + 1;
      pd->pd_Visible = 1;

      val = pd->pd_Level;
   }
   else
   {
      pd->pd_Min = 0;
      pd->pd_Max = max(pd->pd_Total - pd->pd_Visible, 0);

      val = pd->pd_Top;
   };

   if      (val < pd->pd_Min) val = pd->pd_Min;
   else if (val > pd->pd_Max) val = pd->pd_Max;

   if (pd->pd_Flags & PDF_SLIDER)
   {
      pd->pd_Level = val;

      /*
       * Check if any of these values changed.
       */
      if ((pd->pd_Min != omin) || (pd->pd_Max != omax) || (pd->pd_Level != olev))
	 redraw = GREDRAW_UPDATE;
   }
   else
   {
      pd->pd_Top = val;

      /*
       * Check if any of these values changed.
       */
      if ((pd->pd_Top != oldtop) || (pd->pd_Total != oldtot) || (pd->pd_Visible != oldvis))
	 redraw = GREDRAW_UPDATE;
   };

   if (fc)
   {
      Get_Attr(bc->bc_Frame, FRM_Type, &data);
      switch (data)
      {
      case FRTYPE_NONE:
	 ho = vo = 0;
      case FRTYPE_BORDER:
	 if (GADGET(obj)->Activation & (GACT_LEFTBORDER|GACT_RIGHTBORDER)) { ho = 4; vo = 1; };
	 if (GADGET(obj)->Activation & (GACT_TOPBORDER|GACT_BOTTOMBORDER)) { ho = 2; vo = 2; };
	 DoSetMethodNG(bc->bc_Frame, FRM_Type, FRTYPE_NONE, TAG_DONE);
	 break;
      default:
	 Get_Attr(bc->bc_Frame, FRM_ThinFrame, &data);
	 vo = 1;
	 ho = data ? 1 : 2;
	 break;
      };
      DoSuperSetMethodNG(cl, obj, BT_LeftOffset,  ho,  BT_RightOffset,  ho,
				  BT_TopOffset,   vo,  BT_BottomOffset, vo,  TAG_DONE);

      redraw = GREDRAW_REDRAW;
   };


   if (pd->pd_Flags & PDF_READY)
   {
      /*
       * Disable state changed?
       */
      if ((GADGET(obj)->Flags & GFLG_DISABLED) != dis)
	 redraw = GREDRAW_REDRAW;

      /*
       * Re-render the gadget.
       */
      if (redraw) DoRenderMethod(obj, opu->opu_GInfo, redraw);

      /*
       * Notify our target if the
       * top value changed.
       */
      if (pd->pd_Flags & PDF_SLIDER)
      {
	 type = (pd->pd_Level != olev) ? SLIDER_Level : 0;
      }
      else
      {
	 type = (pd->pd_Top != oldtop) ? PGA_Top : 0;
      };

      if (type) DoNotifyMethod(obj, opu->opu_GInfo, opu->MethodID == OM_UPDATE ? opu->opu_Flags : 0,
		   GA_ID, GADGET(obj)->GadgetID, type, val, TAG_DONE);
   }
   return 1;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the gadget.
 */
METHOD(PropClassRender, struct bmRender *, bmr)
{
   PD                   *pd = INST_DATA(cl, obj);
   BC                   *bc = BASE_DATA(obj);
   struct BaseInfo      *bi = bmr->bmr_BInfo;
   struct RastPort      *rp = bi->bi_RPort;

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Setup the object.
    */
   CalcPropMuck(cl, obj, bi);

   /*
    * Complete re-render?
    */
   if (bmr->bmr_Flags == GREDRAW_REDRAW)
   {
      /*
       * Render the arrow images.
       */
      if (pd->pd_Flags & PDF_ARROWS)
      {
	 AsmDoMethod(pd->pd_Arrow1, GM_RENDER, bi, rp, GREDRAW_REDRAW);
	 AsmDoMethod(pd->pd_Arrow2, GM_RENDER, bi, rp, GREDRAW_REDRAW);
      };

      /*
       * Then the slider body.
       */
      AsmDoMethod(pd->pd_Prop, GM_RENDER, bi, rp, GREDRAW_REDRAW);
   }
   else
   {
      /*
       * Any arrow active?
       */
      if (pd->pd_Flags & PDF_LEFT_UP)
	 AsmDoMethod(pd->pd_Arrow1, GM_RENDER, bi, rp, GREDRAW_REDRAW);
      else if (pd->pd_Flags & PDF_RIGHT_DOWN)
	 AsmDoMethod(pd->pd_Arrow2, GM_RENDER, bi, rp, GREDRAW_REDRAW);

      /*
       * Only re-render the slider when it isn't active.
       */
      if (!(pd->pd_Flags & PDF_PROPACTIVE))
	 AsmDoMethod(pd->pd_Prop, GM_RENDER, bi, rp, GREDRAW_UPDATE);
   };

   /*
    * Ghost the gadget hitbox when it is disabled.
    */
   if (GADGET(obj)->Flags & GFLG_DISABLED)
   {
      BDisableBox(bi, &bc->bc_HitBox);
      if (pd->pd_Arrow1) BDisableBox(bi, GADGETBOX(pd->pd_Arrow1));
      if (pd->pd_Arrow2) BDisableBox(bi, GADGETBOX(pd->pd_Arrow2));
   }
   return 1;
}
METHOD_END
///
/// OM_GET
/*
 * They want to know something.
 */
METHOD(PropClassGet, struct opGet *, opg)
{
   PD           *pd = INST_DATA(cl, obj);
   ULONG         rc = 1;
   ULONG        *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   case PGA_Freedom:
      STORE (pd->pd_Flags & PDF_PROPHORIZ) ? FREEHORIZ : FREEVERT;
      break;

   case PGA_Top:
      STORE pd->pd_Top;
      break;

   case SLIDER_Level:
      STORE pd->pd_Level;
      break;

   case SLIDER_Min:
      STORE pd->pd_Min;
      break;

   case SLIDER_Max:
      STORE pd->pd_Max;
      break;

   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   }
   return rc;
}
METHOD_END
///
/// GM_HITTEST
/*
 * Where we hit and if so, what was hit.
 */
METHOD(PropClassHitTest, struct gpHitTest *, gph)
{
   PD               *pd = INST_DATA(cl, obj);
   BC               *bc = BASE_DATA(obj);
   ULONG             rc = 0;

   /*
    * Calculate absolute click position.
    */
   WORD l = GADGETBOX(obj)->Left + gph->gpht_Mouse.X;
   WORD t = GADGETBOX(obj)->Top  + gph->gpht_Mouse.Y;

   if (PointInBox(&bc->bc_HitBox, l, t))
   {
      /*
       * Did they click inside the
       * proportional gadget?
       */
      rc = ForwardMsg(obj, pd->pd_Prop, (Msg)gph);

      if (rc == GMR_GADGETHIT) pd->pd_Flags |= PDF_PROPACTIVE;
   };

   if (!rc && pd->pd_Arrow1)
   {
      /*
       * Clicked in the left/up arrow?
       */
      rc = ForwardMsg(obj, pd->pd_Arrow1, (Msg)gph);

      if (rc == GMR_GADGETHIT) pd->pd_Flags |= PDF_LEFT_UP;
   };

   if (!rc && pd->pd_Arrow2)
   {
      /*
       * Clicked in the down/right arrow?
       */
      rc = ForwardMsg(obj, pd->pd_Arrow2, (Msg)gph);

      if (rc == GMR_GADGETHIT) pd->pd_Flags |= PDF_RIGHT_DOWN;
   };

   if (rc == GMR_GADGETHIT)
   {
      pd->pd_Reset1 = pd->pd_Top;
      pd->pd_Reset2 = pd->pd_Level;
   };
   return rc;
}
METHOD_END
///

//STATIC ASM VOID NotifyChange(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpInput *gpi, REG(d0) ULONG flags)
STATIC ASM REGFUNC4(VOID, NotifyChange,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi),
	REGPARAM(D0, ULONG, flags))
{
   PD          *pd = INST_DATA(cl, obj);
   ULONG        type;
   LONG         val, oldval = (pd->pd_Flags & PDF_SLIDER) ? pd->pd_Level : pd->pd_Top;

   /*
    * Obtain top-value.
    */
   Get_Attr(pd->pd_Prop, PGA_Top, &val);

   if (pd->pd_Flags & PDF_SLIDER)
   {
      /*
       * Make it a level.
       */
      if (pd->pd_Flags & PDF_PROPHORIZ) val = pd->pd_Min + val;
      else                              val = pd->pd_Max - val;

      pd->pd_Level = val;
      type = SLIDER_Level;
   }
   else
   {
      pd->pd_Top = val;
      type = PGA_Top;
   };

   /*
    * Notify.
    */
   if ((flags == OPUF_INTERIM) && (val == oldval)) return;

   DoNotifyMethod(obj, gpi->gpi_GInfo, flags, GA_ID, GADGET(obj)->GadgetID, type, val, TAG_DONE);
}
REGFUNC_END

/*
 * Adjust knob position in
 * whatever direction necessary.
 */
//STATIC ASM VOID AdjustKnob(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpInput *gpi)
STATIC ASM REGFUNC3(VOID, AdjustKnob,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi))
{
   PD        *pd = INST_DATA(cl, obj);
   LONG       top, total = max(pd->pd_Total - pd->pd_Visible, 0);

   Get_Attr(pd->pd_Prop, PGA_Top, &top);

   if (pd->pd_Flags & PDF_LEFT_UP)
   {
      top--;
   }
   else if (pd->pd_Flags & PDF_RIGHT_DOWN)
   {
      top++;
   };

   if ((top >= 0) && (top <= total))
   {
      DoSetMethod(pd->pd_Prop, gpi->gpi_GInfo, PGA_Top, top, TAG_DONE);
      NotifyChange(cl, obj, gpi, 0L);
   };
}
REGFUNC_END

/// GM_GOACTIVE

METHOD(PropClassGoActive, struct gpInput *, gpi)
{
   PD             *pd = INST_DATA(cl, obj);
   Object         *arrow;
   ULONG           rc = GMR_NOREUSE;

   /*
    * We do not go active when we are disabled or was activated
    * by Activate gadget.
    */
   if ((GADGET(obj)->Flags & GFLG_DISABLED) || (!gpi->gpi_IEvent))
   {
      pd->pd_Flags &= ~(PDF_PROPACTIVE|PDF_LEFT_UP|PDF_RIGHT_DOWN);
      return rc;
   }

   /*
    * Let the superclass have a go...
    */
   AsmDoSuperMethodA(cl, obj, (Msg)gpi);

   /*
    * Is the proportional gadget active?
    */
   if (pd->pd_Flags & PDF_PROPACTIVE)
   {
      /*
       * If so adjust the click
       * coordinates and route the
       * message to the proportional gadget.
       */
      rc = ForwardMsg(obj, pd->pd_Prop, (Msg)gpi);

      if (rc & GMR_VERIFY) NotifyChange(cl, obj, gpi, 0);
   }
   else if (pd->pd_Flags & (PDF_LEFT_UP|PDF_RIGHT_DOWN))
   {
      /*
       * Clear timer threshold.
       */
      pd->pd_RptTicks = 0;

      /*
       * Adjust knob and notify when the knob position changed.
       */
      AdjustKnob(cl, obj, gpi);

      /*
       * Route the message to the button gadget.
       */
      arrow = (pd->pd_Flags & PDF_LEFT_UP) ? pd->pd_Arrow1 : pd->pd_Arrow2;
      rc = ForwardMsg(obj, arrow, (Msg)gpi);
   }
   return rc;
}
METHOD_END
///
/// GM_HANDLEINPUT
/*
 * Handle user input.
 */
METHOD(PropClassHandleInput, struct gpInput *, gpi)
{
   PD            *pd = INST_DATA(cl, obj);
   Object        *arrow;
   WORD           l, t, sel = 0;
   ULONG          rc = GMR_MEACTIVE;

   /*
    * Calculate absolute click position.
    */
   l = GADGETBOX(obj)->Left + gpi->gpi_Mouse.X;
   t = GADGETBOX(obj)->Top  + gpi->gpi_Mouse.Y;

   /*
    * Is the proportional gadget active?
    */
   if (pd->pd_Flags & PDF_PROPACTIVE)
   {
      /*
       * Right mouse button pressed?
       */
      if (gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE && gpi->gpi_IEvent->ie_Code == MENUDOWN)
      {
	 /*
	  * Yep. Say we need to reset to
	  * the initial value.
	  */
	 pd->pd_Flags |= PDF_RESET;
	 return GMR_NOREUSE;
      }
      /*
       * If so adjust the click
       * coordinates and rout the
       * message to the proportional gadget.
       */
      rc = ForwardMsg(obj, pd->pd_Prop, (Msg)gpi);

      NotifyChange(cl, obj, gpi, (rc == GMR_MEACTIVE) ? OPUF_INTERIM : 0);
   }
   else
   {
      arrow = (pd->pd_Flags & PDF_LEFT_UP) ? pd->pd_Arrow1 : pd->pd_Arrow2;

      /*
       * Check if the mouse is
       * still over the active arrow.
       */
      if (PointInBox(GADGETBOX(arrow), l, t))
	 sel = GFLG_SELECTED;

      /*
       * Evaluate input.
       */
      if (gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
      {
	 switch (gpi->gpi_IEvent->ie_Code)
	 {
	 case SELECTUP:
	    /*
	     * They released the left
	     * mouse button.
	     */
	    rc = GMR_NOREUSE | GMR_VERIFY;

	    /*
	     * Unselect the arrow.
	     */
	    sel = 0;
	    break;

	 case MENUDOWN:
	    /*
	     * Terminate when they press the menubutton.
	     */
	    sel = 0;
	    pd->pd_Flags |= PDF_RESET;
	    rc = GMR_NOREUSE;
	    break;
	 }
      }
      else if (gpi->gpi_IEvent->ie_Class == IECLASS_TIMER)
      {
	 /*
	  * On every timer event we
	  * increase or decrease the knob
	  * position.
	  */
	 if (sel)
	 {
	    /*
	     * We delay with adjusting the knob position for a little while just
	     * like the key-repeat threshold.  Probably should use the system prefs
	     * for determing the delay but this is _so_ much easier.
	     */
	    if (pd->pd_RptTicks > 3)
	    {
	       /*
		* Adjust knob and notify when the knob position changed.
		*/
	       AdjustKnob(cl, obj, gpi);
	    }
	    else
	    {
	       /*
		* Increase repeat treshold.
		*/
	       pd->pd_RptTicks++;
	    };
	 };
      };

      /*
       * When the selected state
       * changed show it visually.
       */
      if ((GADGET(arrow)->Flags & GFLG_SELECTED) != sel)
      {
	 /*
	  * Flip selected bit and re-render.
	  */
	 GADGET(arrow)->Flags ^= GFLG_SELECTED;
	 DoRenderMethod(arrow, gpi->gpi_GInfo, GREDRAW_REDRAW);
      };
   };
   return rc;
}
METHOD_END
///
/// GM_GOINACTIVE
/*
 * Abort activity.
 */
METHOD(PropClassGoInActive, struct gpGoInactive *, ggi)
{
   PD          *pd = INST_DATA(cl, obj);
   ULONG        rc;

   /*
    * Is the proportional gadget
    * active?
    */
   if (pd->pd_Flags & PDF_PROPACTIVE)
   {
      /*
       * If so route the message to
       * the proportional gadget and
       * mark it as not active.
       */
      AsmDoMethodA(pd->pd_Prop, (Msg)ggi);
   };

   /*
    * Un-activate our arrows and let the super
    * class see what it can do with this message.
    */
   pd->pd_Flags &= ~(PDF_LEFT_UP|PDF_RIGHT_DOWN|PDF_PROPACTIVE);
   rc = AsmDoSuperMethodA(cl, obj, (Msg)ggi);

   /*
    * Reset to the initial value when necessary.
    */
   if (pd->pd_Flags & PDF_RESET)
   {
      DoSetMethod(obj, ggi->gpgi_GInfo, PGA_Top, pd->pd_Reset1, SLIDER_Level, pd->pd_Reset2, TAG_DONE);
      pd->pd_Flags &= ~PDF_RESET;
   }
   return rc;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * Find out our minumum size.
 */
METHOD(PropClassDimensions, struct bmDimensions *, bmd)
{
   PD       *pd = INST_DATA(cl, obj);
   BC       *bc = BASE_DATA(obj);
   BOOL      vborder = GADGET(obj)->Activation & (GACT_LEFTBORDER|GACT_RIGHTBORDER);
   BOOL      hborder = GADGET(obj)->Activation & (GACT_TOPBORDER|GACT_BOTTOMBORDER);
   BOOL      horiz = pd->pd_Flags & PDF_PROPHORIZ;
   int       arrowsize = pd->pd_ArrowSize;
   ULONG     rc;

   if (pd->pd_Flags & PDF_ARROWS)
   {
      if (!arrowsize)
      {
	 if      (vborder) arrowsize = 11;
	 else if (hborder) arrowsize = 16;
	 else              arrowsize = horiz ? 10 : 9;

	 pd->pd_ArrowSize = arrowsize;
      };
      DoSetMethodNG(bc->bc_Frame, horiz ? FRM_OuterOffsetRight : FRM_OuterOffsetBottom, arrowsize << 1, TAG_DONE);
   };

   /*
    * Add these to the superclass results.
    */
   rc = CalcDimensions(cl, obj, bmd, 8+(horiz ? arrowsize*2 : 0), 8+(horiz ? 0 : arrowsize*2 ));

   if (vborder) bmd->bmd_Extent->be_Min.Width  = bmd->bmd_Extent->be_Nom.Width  = 18;
   if (hborder) bmd->bmd_Extent->be_Min.Height = bmd->bmd_Extent->be_Nom.Height = 10;

   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Dispose of ourselves.
 */
METHOD(PropClassDispose, Msg, msg)
{
   PD       *pd = INST_DATA(cl, obj);

   /*
    * Dispose of the allocated objects.
    */
   if (pd->pd_Arrow1) DisposeObject(pd->pd_Arrow1);
   if (pd->pd_Arrow2) DisposeObject(pd->pd_Arrow2);
   if (pd->pd_Prop )  DisposeObject(pd->pd_Prop);
   if (pd->pd_Knob )  DisposeObject(pd->pd_Knob);

   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// WM_KEYACTIVE
/*
 * We are activated by the keyboard.
 */
METHOD(PropClassKeyActive, struct wmKeyInput *, wmki)
{
   PD       *pd = ( PD * )INST_DATA( cl, obj );
   UWORD        qual = wmki->wmki_IEvent->ie_Qualifier, code = wmki->wmki_IEvent->ie_Code;
   BOOL         sl = pd->pd_Flags & PDF_SLIDER;
   LONG         otop = sl ? pd->pd_Level : pd->pd_Top, ntop = otop;
   LONG         inc = pd->pd_Visible - 1;

   if (inc < 1) inc = 1;

   /*
    * Shifted key?
    */
   if (qual & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
   {
      if (ntop > pd->pd_Min) ntop -= inc;
      if (ntop < pd->pd_Min) ntop  = pd->pd_Min;
   }
   else
   {
      if (ntop < pd->pd_Max) ntop += inc;
      if (ntop > pd->pd_Max) ntop  = pd->pd_Max;
   }

   /*
    * Did the top position change?
    */
   if ((ntop != otop) && !(code & IECODE_UP_PREFIX))
   {
      /*
       * Let the slider know about this change.
       */
      DoSetMethod(obj, wmki->wmki_GInfo, sl ? SLIDER_Level : PGA_Top, ntop, TAG_END);
      /*
       * Skip the ID because the notification
       * will handle the change.
       */
   };
   /*
    * Tell then to ignore this message.
    */
   *(wmki->wmki_ID) = WMHI_IGNORE;

   return WMKF_VERIFY;
}
METHOD_END
///

/// Class initialization,
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,         (FUNCPTR)PropClassRender,
   BASE_DIMENSIONS,     (FUNCPTR)PropClassDimensions,

   OM_NEW,              (FUNCPTR)PropClassNew,
   OM_SET,              (FUNCPTR)PropClassSetUpdate,
   OM_UPDATE,           (FUNCPTR)PropClassSetUpdate,
   OM_GET,              (FUNCPTR)PropClassGet,
   OM_DISPOSE,          (FUNCPTR)PropClassDispose,
   GM_HITTEST,          (FUNCPTR)PropClassHitTest,
   GM_HANDLEINPUT,      (FUNCPTR)PropClassHandleInput,
   GM_GOACTIVE,         (FUNCPTR)PropClassGoActive,
   GM_GOINACTIVE,       (FUNCPTR)PropClassGoInActive,
   WM_KEYACTIVE,        (FUNCPTR)PropClassKeyActive,
   DF_END,              NULL
};

/*
 * Class initialization.
 */
makeproto Class *InitPropClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
			 CLASS_ObjectSize,     sizeof(PD),
			 CLASS_DFTable,        ClassFunc,
			 TAG_DONE);
}
///
