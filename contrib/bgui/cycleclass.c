/*
 * @(#) $Header$
 *
 * BGUI library
 * cycleclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.8  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.7  2002/09/17 17:11:16  stegerg
 * inversvid drawmode together with Text() works now, so no
 * workaround needed.
 *
 * Revision 42.6  2000/08/17 15:09:18  chodorowski
 * Fixed compiler warnings.
 *
 * Revision 42.5  2000/07/18 18:32:38  stegerg
 * The AllocBaseInfo/AllocBaseInfoDebug calls in OpenPopUpWindow method
 * missed a TAG_DONE. Manuel, if you read this: this is a real (also in AmigaOS)
 * bug, not just AROS!!!
 *
 * Revision 42.4  2000/07/11 17:04:41  stegerg
 * temp fix for small gfx bug which happens because of INVERSVID
 * drawmode not yet working in AROS.
 *
 * Revision 42.3  2000/05/29 15:42:49  stegerg
 * fixed some "comparison is always 1 due to limited range of data type"
 * errors
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
 * Revision 42.0  2000/05/09 22:08:45  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:08  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.3  1998/12/07 03:06:57  mlemos
 * Replaced OpenFont and CloseFont calls by the respective BGUI debug macros.
 *
 * Revision 41.10.2.2  1998/03/01 23:06:01  mlemos
 * Corrected RastPort tag where GadgetInfo was being passed.
 *
 * Revision 41.10.2.1  1998/03/01 15:37:32  mlemos
 * Added support to track BaseInfo memory leaks.
 *
 * Revision 41.10  1998/02/25 21:11:52  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:59  mlemos
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
   struct Window     *cd_PopWindow;    /* Popup window pointer.            */
   Object            *cd_ArrayLabel;   /* Label object for active label.   */
   Object            *cd_Image;        /* Image for the popcycle.          */
   UBYTE            **cd_Labels;       /* Array of object labels.          */
   ULONG              cd_Active;       /* Active label.                    */
   ULONG              cd_NumLabels;    /* Amount of labels.                */
   struct TextAttr   *cd_PopupFont;    /* Font for pop window.             */
   struct TextFont   *cd_Font;         /* General font.                    */
   UWORD              cd_Flags;        /* See below.                       */
   UWORD              cd_Previous;     /* Previously selected label.       */
   UWORD              cd_ImageWidth;   /* Width of area for clicking.      */
}  CD;

#define CDF_POPUP       (1<<0)         /* Popup mode.          */
#define CDF_NOTIFY      (1<<1)         /* Notify change.       */
#define CDF_THIN        (1<<2)         /* Thin frames.         */
#define CDF_POPACTIVE   (1<<3)         /* Pop on active entry. */

///

/// OM_NEW
/*
 * Create a new object.
 */
METHOD(CycleClassNew, struct opSet *, ops)
{
   CD             *cd;
   struct TagItem *tags;
   ULONG           rc;

   tags = DefTagList(BGUI_CYCLE_GADGET, ops->ops_AttrList);

   /*
    * First we let the superclass
    * create an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      /*
       * Get instance data.
       */
      cd = INST_DATA(cl, rc);

      /*
       * Set up imagery.
       */
      cd->cd_ImageWidth = 18;

      cd->cd_Image      = BGUI_NewObject(BGUI_VECTOR_IMAGE, VIT_BuiltIn,     BUILTIN_CYCLE,
							    TAG_DONE);

      /*
       * Setup a Label class object used to render the active label.
       */
      cd->cd_ArrayLabel = BGUI_NewObject(BGUI_LABEL_IMAGE,  LAB_Place,       PLACE_IN,
							    LAB_Highlight,   FALSE,
							    LAB_Underscore,  0,
							    IMAGE_ErasePen,  0,
							    TAG_DONE);

      AsmCoerceMethod(cl, (Object *)rc, OM_SET, tags, NULL);

      if (cd->cd_ArrayLabel && cd->cd_Image && cd->cd_Labels)
      {
	 ;
      }
      else
      {
	 AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
	 rc = 0;
      };
   };
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_SET
/*
 * Change the object attributes.
 */
METHOD(CycleClassSetUpdate, struct opUpdate *, opu)
{
   CD                 *cd = INST_DATA(cl, obj);
   struct TagItem     *tstate = opu->opu_AttrList, *tag;
   ULONG               oact = cd->cd_Active, data;
   WORD                dis = GADGET(obj)->Flags & GFLG_DISABLED;
   UBYTE             **new_labels = NULL;
   int                 num_labels;
   BOOL                render = FALSE;
   struct GadgetInfo  *gi = opu->opu_GInfo;

   /*
    * First let the superclass do its thing.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * Force the correct activation method.
    */
   GADGET(obj)->Activation |= GACT_RELVERIFY;
   GADGET(obj)->Activation &= ~GACT_IMMEDIATE;

   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case BT_TextAttr:
	 cd->cd_PopupFont = (struct TextAttr *)data;
	 /*
	  * Let the label know.
	  */
	 DoSetMethodNG(cd->cd_ArrayLabel, LAB_TextAttr, data, TAG_DONE);
	 break;

      case FRM_ThinFrame:
	 if (data) cd->cd_Flags |= CDF_THIN;
	 else      cd->cd_Flags &= ~CDF_THIN;
	 break;

      case CYC_Active:
	 cd->cd_Active = data;
	 break;

      case CYC_Labels:
	 /*
	  * Count the number of labels in the array (offset by 1).
	  */
	 num_labels = data ? CountLabels((UBYTE **)data) : -1;

	 if (num_labels >= 0)
	 {
	    if (new_labels = BGUI_AllocPoolMem((num_labels + 2) * sizeof(UBYTE *)))
	    {
	       if (cd->cd_Labels) BGUI_FreePoolMem(cd->cd_Labels);
	       cd->cd_Labels    = new_labels;
	       cd->cd_NumLabels = num_labels;
	       CopyMem((UBYTE *)data, (UBYTE *)new_labels, (num_labels + 1) * sizeof(UBYTE *));
	    };
	 };
	 render = TRUE;
	 break;

      case CYC_Popup:
	 if (data) cd->cd_Flags |= CDF_POPUP;
	 else      cd->cd_Flags &= ~CDF_POPUP;
	 break;

      case CYC_PopActive:
	 if (data) cd->cd_Flags |= CDF_POPACTIVE;
	 else      cd->cd_Flags &= ~CDF_POPACTIVE;
	 break;
      };
   };

   if ((GADGET(obj)->Flags & GFLG_DISABLED) != dis)
      /*
       * Re-render when the gadget disabled state changed.
       */
      render = TRUE;

   /*
    * Here's that sanity check again.
    */
   if (cd->cd_Active > cd->cd_NumLabels)
       cd->cd_Active = cd->cd_NumLabels;

   if ((oact != cd->cd_Active) || new_labels)
   {
      /*
       * Setup new text.
       */
      DoSetMethodNG(cd->cd_ArrayLabel, LAB_Label, cd->cd_Labels[cd->cd_Active], LAB_Place, PLACE_IN, TAG_DONE);
      render = TRUE;
   };

   if (render) DoRenderMethod(obj, gi, GREDRAW_REDRAW);

   /*
    * Notify the target when necessary.
    */
   if (oact != cd->cd_Active)
   {
      DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, CYC_Active, cd->cd_Active, TAG_DONE);
   }
   return 1;
}
METHOD_END
///

/// RenderArrow
/*
 * Render the cycle arrow.
 */
STATIC VOID RenderArrow(struct BaseInfo *bi, struct IBox *ib, ULONG state, CD *cd)
{
   Object   *image = cd->cd_Image;
   ULONG     type;
   
   /*
    * Get coords.
    */
   int w = cd->cd_ImageWidth;
   int h = ib->Height;
   int l = ib->Left - w;
   int t = ib->Top;

   /*
    * Render the small seperator.
    */
   BSetDrMd(bi, JAM1);

   BSetDPenA(bi, (state != IDS_SELECTED) ? SHINEPEN : SHADOWPEN);
   VLine(bi->bi_RPort, l + w + 1, t + 2, t + h - 3);

   BSetDPenA(bi, (state != IDS_SELECTED) ? SHADOWPEN : SHINEPEN);
   VLine(bi->bi_RPort, l + w, t + 2, t + h - 3);

   if (state == IDS_SELECTED)
   {
      type = BUILTIN_CYCLE2;
   }
   else
   {
      type = (cd->cd_Flags & CDF_POPUP) ? BUILTIN_POPUP : BUILTIN_CYCLE;
   };

   /*
    * Setup and render the image.
    */
   DoSetMethodNG(image, VIT_BuiltIn, type, IA_Left, l, IA_Top, t, IA_Width, w, IA_Height, h, TAG_END);
   BDrawImageState(bi, image, 0, 0, state);
}
///
/// RenderPopupEntry
/*
 * Render an entry in the popup-list.
 */
STATIC VOID RenderPopupEntry(struct BaseInfo *bi, CD *cd, ULONG num, BOOL sel)
{
   UWORD            ypos, xpos, tlen, pix = cd->cd_Flags & CDF_THIN ? 2 : 4;
   UBYTE           *name = cd->cd_Labels[num];
   struct RastPort *rp = bi->bi_RPort;

   /*
    * Compute text length.
    */
   tlen = TextWidth(rp, name);

   /*
    * Compute positions.
    */
   xpos = (cd->cd_PopWindow->Width - tlen) >> 1;
   ypos = (num * rp->TxHeight)  + 2;

   /*
    * Setup pens for filling.
    */
   #ifdef ENHANCED
   BSetDPenA(bi, sel ? BARDETAILPEN : BARBLOCKPEN);
   #else
   if (OS30) BSetDPenA(bi, sel ? BARDETAILPEN : BARBLOCKPEN);
   else      BSetDPenA(bi, BLOCKPEN);
   #endif

   /*
    * RectFill entry.
    */
   BSetDrMd(bi, JAM1);
   BRectFill(bi, pix, ypos, cd->cd_PopWindow->Width - pix - 1, ypos + rp->TxHeight - 1);

   /*
    * Setup drawmode.
    */
   #ifdef ENHANCED
   BSetDrMd(bi, sel ? JAM2|INVERSVID : JAM2);
   #else
   if (OS30) BSetDrMd(bi, sel ? JAM2|INVERSVID : JAM2);
   else      BSetDrMd(bi, JAM2);
   #endif

   /*
    * Setup pens for text.
    */
   BSetDPenA(bi, BARDETAILPEN);
   BSetDPenB(bi, BARBLOCKPEN);

   /*
    * Move to the desired location and render.
    */
   Move(rp, xpos, ypos + rp->TxBaseline);
   Text(rp, name, strlen(name));

   #ifndef ENHANCED
   /*
    * < OS 3.0 will get the entry
    * complemented if it is selected.
    */
   if (!OS30 && sel)
   {
      BSetDrMd(bi, JAM2|COMPLEMENT);
      BRectFill(bi, pix, ypos, cd->cd_PopWindow->Width - pix - 1, ypos + rp->TxHeight - 1);
   }
   #endif
}
///
/// GM_RENDER
/*
 * Render the object.
 */
METHOD(CycleClassRenderX, struct gpRender *, gpr)
{
   CD              *cd = INST_DATA(cl, obj);

   DoSuperSetMethodNG(cl, obj, BT_LeftOffset, cd->cd_ImageWidth, TAG_DONE);

   return AsmDoSuperMethodA(cl, obj, (Msg)gpr);
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the object.
 */
METHOD(CycleClassRender, struct bmRender *, bmr)
{
   CD              *cd = INST_DATA(cl, obj);
   BC              *bc = BASE_DATA(obj);
   struct BaseInfo *bi = bmr->bmr_BInfo;
   int              state = GADGET(obj)->Flags & GFLG_SELECTED ? IDS_SELECTED : IDS_NORMAL;

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Setup array label.
    */
   *IMAGEBOX(cd->cd_ArrayLabel) = bc->bc_InnerBox;

   /*
    * Render the arrow.
    */
   RenderArrow(bi, &bc->bc_InnerBox, state, cd);

   /*
    * We always render the active gadget label.
    */
   BDrawImageState(bi, cd->cd_ArrayLabel, 0, 0, state);

   /*
    * Ghost the gadget when it is disabled.
    */
   if (GADGET(obj)->Flags & GFLG_DISABLED)
      BDisableBox(bi, &bc->bc_HitBox);

   return 1;
}
METHOD_END
///

/// OpenPopupWindow
/*
 * Open the popup-window.
 */
METHOD(OpenPopupWindow, struct gpInput *, gpi)
{
   CD                   *cd = INST_DATA(cl, obj);
   BC                   *bc = BASE_DATA(obj);
   UWORD                 max_items, num_items, wleft, wtop, wwi, wwh, bg, i;
   struct IBox          *ibox;
   struct RastPort      *rp;
   struct GadgetInfo    *gi = gpi->gpi_GInfo;
   struct BaseInfo      *bi;
   int                   ysize;
   ULONG                 rc = GMR_NOREUSE;

   /*
    * Get the gadget hitbox bounds.
    */
   Get_SuperAttr(cl, obj, BT_HitBox, &ibox);

   /*
    * Open the font.
    */
   if (cd->cd_PopupFont)
      cd->cd_Font = BGUI_OpenFont(cd->cd_PopupFont);

   /* BAD CODE! Does not check OpenFont result! */

   ysize = cd->cd_Font->tf_YSize;

   /*
    * How many items will fit?
    */
   max_items = (gi->gi_Screen->Height - 4) / ysize;

   /*
    * How many do we have?
    */
   num_items = (cd->cd_NumLabels + 1) > max_items ? max_items : (cd->cd_NumLabels + 1);

   /*
    * Compute window position and dimensions.
    */
   wleft = bc->bc_HitBox.Left + gi->gi_Window->LeftEdge;
   wwi   = bc->bc_HitBox.Width;
   wwh   = (num_items * ysize) + 4;

   if (cd->cd_Flags & CDF_POPACTIVE)
      wtop = gi->gi_Screen->MouseY - (2 + ((cd->cd_Active * ysize) + (ysize >> 1)));
   else
      wtop = bc->bc_HitBox.Top + bc->bc_HitBox.Height + gi->gi_Window->TopEdge;

   /*
    * Pop the window.
    */
   cd->cd_PopWindow = OpenWindowTags(NULL, WA_Left,           wleft,   WA_Top,            wtop,
					   WA_Width,          wwi,     WA_Height,         wwh,
					   WA_Flags,          0,       WA_IDCMP,          0,
					   WA_Borderless,     TRUE,    WA_AutoAdjust,     TRUE,
					   WA_SmartRefresh,   TRUE,    WA_NoCareRefresh,  TRUE,
					   WA_RMBTrap,        TRUE,    WA_CustomScreen,   gi->gi_Screen,
					   TAG_DONE);
   /*
    * success?
    */
   if (cd->cd_PopWindow)
   {
      /*
       * No previous selected item.
       */
      cd->cd_Previous = (UWORD)~0;

      /* AROS BUGFIX --> AMIGAOS BUGFIX: TAG_DONE missed in both AllocBaseInfoDebug and AllocBaseInfo call below */
#ifdef DEBUG_BGUI
      if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_Screen, gi->gi_Screen, BI_RastPort, rp = cd->cd_PopWindow->RPort, TAG_DONE))
#else
      if (bi = AllocBaseInfo(BI_Screen, gi->gi_Screen, BI_RastPort, rp = cd->cd_PopWindow->RPort, TAG_DONE))
#endif
      {
	 /*
	  * Setup the font.
	  */
	 if (cd->cd_Font) BSetFont(bi, cd->cd_Font);

	 /*
	  * Setup pop window rastport.
	  */
	 BSetDPenA(bi, BARBLOCKPEN);
	 BSetDrMd(bi, JAM1);

	 /*
	  * Fill...
	  */
	 BBoxFill(bi, 0, 0, wwi, wwh);

	 /*
	  * Setup pop window rastport.
	  */
	 BSetDPenA(bi, BARDETAILPEN);

	 /*
	  * Render border.
	  */
	 Move(rp, 0, 0 );
	 Draw(rp, wwi - 1, 0);
	 Draw(rp, wwi - 1, wwh - 1 );
	 Draw(rp, 0, wwh - 1 );
	 Draw(rp, 0, 0 );

	 if (!(cd->cd_Flags & CDF_THIN))
	 {
	    Move(rp, 1, 0 );
	    Draw(rp, 1, wwh - 1 );
	    Move(rp, wwi - 2, 0 );
	    Draw(rp, wwi - 2, wwh - 1 );
	 }

	 /*
	  * Render list-entries.
	  */
	 for (i = 0; i < num_items; i++)
	    RenderPopupEntry(bi, cd, i, FALSE);

	 FreeBaseInfo(bi);

	 rc = GMR_MEACTIVE;
      };
   }
   return rc;
}
METHOD_END
///

/// GM_GOACTIVE
/*
 * Go in active mode.
 */
METHOD(CycleClassGoActive, struct gpInput *, gpi)
{
   CD          *cd = INST_DATA(cl, obj);
   BC          *bc = BASE_DATA(obj);
   ULONG        rc = GMR_NOREUSE;

   /*
    * We don't go active when we are disabled.
    */
   if ( GADGET( obj )->Flags & GFLG_DISABLED )
      return( GMR_NOREUSE );

   /*
    * We only go active when triggered by a mouse click.
    */
   if (gpi->gpi_IEvent)
   {
      /*
       * Let the superclass have a go...
       */
      AsmDoSuperMethodA(cl, obj, (Msg)gpi);

      /*
       * Popup mode?
       */
      if (cd->cd_Flags & CDF_POPUP)
      {
	 if (gpi->gpi_GInfo->gi_Window->MouseX > (bc->bc_HitBox.Left + cd->cd_ImageWidth))
	 {
	    /*
	     * Open the popup window.
	     * Do normal selection if the popup window will not open.
	     */
	    if (OpenPopupWindow(cl, obj, gpi) == 0) return 0;
	 };
      };
      GADGET(obj)->Flags |= GFLG_SELECTED;
      DoRenderMethod(obj, gpi->gpi_GInfo, GREDRAW_REDRAW);
      rc = GMR_MEACTIVE;
   }
   return rc;
}
METHOD_END
///

/*
 * Which entry is selected?
 */
//STATIC ASM UWORD Selected(REG(a0) CD *cd, REG(a1) struct RastPort *rp)
STATIC ASM REGFUNC2(UWORD, Selected,
	REGPARAM(A0, CD *, cd),
	REGPARAM(A1, struct RastPort *, rp))
{
   WORD        mx = cd->cd_PopWindow->MouseX, my = cd->cd_PopWindow->MouseY;
   LONG        item = ~0;

   /*
    *  Mouse in the window?
    */
   if ( mx >= 4 && mx < ( cd->cd_PopWindow->Width - 4 ) && my >= 2 && my < ( cd->cd_PopWindow->Height - 2 )) {
      /*
       * Find the entry the mouse is over.
       */
      item = ( my - 2 ) / rp->TxHeight;

      if ( item > cd->cd_NumLabels ) item = ~0;
   }
   return(( UWORD )item );
}
REGFUNC_END
/// GM_HANDLEINPUT
/*
 * Handle the gadget input.
 */
METHOD(CycleClassHandleInput, struct gpInput *, gpi)
{
   CD                   *cd = INST_DATA(cl, obj);
   struct gpHitTest      gph;
   WORD                  sel = 0, item;
   struct GadgetInfo    *gi = gpi->gpi_GInfo;
   struct BaseInfo      *bi;
   ULONG                 rc = GMR_MEACTIVE;

   /*
    * Window up?
    */
   if (cd->cd_PopWindow)
   {
      /*
       * Is the parent window still activated?
       * If not we return immediatly.
       *
       * Security. Intuition should do this already.
       */
      if (!(gi->gi_Window->Flags & WFLG_WINDOWACTIVE))
	 return GMR_NOREUSE;

#ifdef DEBUG_BGUI
      if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gi, BI_RastPort, cd->cd_PopWindow->RPort, TAG_DONE))
#else
      if (bi = AllocBaseInfo(BI_GadgetInfo, gi, BI_RastPort, cd->cd_PopWindow->RPort, TAG_DONE))
#endif
      {
	 /*
	  * Where is the mouse?
	  */
	 item = Selected(cd, bi->bi_RPort);

	 /*
	  * Changed?
	  */
	 if (item != cd->cd_Previous)
	 {
	    /*
	     * Previous selected item?
	     */
	    if (cd->cd_Previous != (UWORD)~0) RenderPopupEntry(bi, cd, cd->cd_Previous, FALSE);

	    /*
	     * Render current entry.
	     */
	    if (item != ~0) RenderPopupEntry(bi, cd, item, TRUE);

	    /*
	     * Setup item.
	     */
	    cd->cd_Previous = item;
	 }
	 FreeBaseInfo(bi);

	 /*
	  * What's the event?
	  */
	 if (gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
	 {
	    switch (gpi->gpi_IEvent->ie_Code)
	    {
	    case SELECTUP:
	       /*
		* Setup the new label and return VERIFY.
		*/
	       if (cd->cd_Previous != (UWORD)~0)
	       {
		  if (cd->cd_Active != cd->cd_Previous)
		  {
		     /*
		      * We need notification.
		      */
		     cd->cd_Flags |= CDF_NOTIFY;

		     /*
		      * New active entry.
		      */
		     cd->cd_Active = cd->cd_Previous;

		     /*
		      * Setup the new label.
		      */
		     DoSetMethodNG(cd->cd_ArrayLabel, LAB_Label, cd->cd_Labels[cd->cd_Active], LAB_Place, PLACE_IN, TAG_END);

		     /*
		      * Re-render to show the change.
		      */
		     DoRenderMethod(obj, gi, GREDRAW_REDRAW);

		     /*
		      * We want to notify about the attribute change.
		      */
		     rc = GMR_NOREUSE | GMR_VERIFY;
		  } else
		     rc = GMR_NOREUSE;
	       } else
		  rc = GMR_NOREUSE;
	       break;

	    case MENUDOWN:
	       rc = GMR_NOREUSE;
	       break;
	    }
	 }
      }
   }
   else
   {
      /*
       * Mouse over the object?
       */
      gph.MethodID      = GM_HITTEST;
      gph.gpht_GInfo    = gpi->gpi_GInfo;
      gph.gpht_Mouse.X  = gpi->gpi_Mouse.X;
      gph.gpht_Mouse.Y  = gpi->gpi_Mouse.Y;

      if ( AsmDoMethodA( obj, ( Msg )&gph ) == GMR_GADGETHIT )
	 sel = GFLG_SELECTED;

      if ( gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE ) {
	 switch ( gpi->gpi_IEvent->ie_Code ) {

	    case  SELECTUP:
	       /*
		* When we are selected we setup a new label
		* and return VERIFY.
		*/
	       if ( sel ) {
		  /*
		   * Clicked with the shift key down cycles backward.
		   */
		  if ( gpi->gpi_IEvent->ie_Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT )) {
		     if ( cd->cd_Active )       cd->cd_Active--;
		     else              cd->cd_Active = cd->cd_NumLabels;
		  } else {
		     if ( cd->cd_Active < cd->cd_NumLabels ) cd->cd_Active++;
		     else              cd->cd_Active = 0;
		  }

		  /*
		   * Setup the new label.
		   */
		  DoSetMethodNG( cd->cd_ArrayLabel, LAB_Label, cd->cd_Labels[ cd->cd_Active ], LAB_Place, PLACE_IN, TAG_END  );

		  /*
		   * We want to notify about the attribute change.
		   */
		  rc = GMR_NOREUSE | GMR_VERIFY;
		  cd->cd_Flags |= CDF_NOTIFY;
	       } else
		  /*
		   * We are not selected anymore.
		   */
		  rc = GMR_NOREUSE;

	       sel = 0;
	       break;

	    case  MENUDOWN:
	       sel = 0;
	       rc = GMR_REUSE;
	       break;
	 }
      }

      /*
       * Do we need a visual change?
       */
      if ((GADGET(obj)->Flags & GFLG_SELECTED) != sel)
      {
	 /*
	  * Toggle the selected bit.
	  */
	 GADGET(obj)->Flags ^= GFLG_SELECTED;

	 /*
	  * Re-render the gadget.
	  */
	 DoRenderMethod(obj, gi, GREDRAW_REDRAW);
      };
   };
   return rc;
}
METHOD_END
///
/// GM_GOINACTIVE

METHOD(CycleClassGoInactive, struct gpGoInactive *, gpgi)
{
   CD          *cd = INST_DATA( cl, obj );

   /*
    * Popup window open?
    */
   if ( cd->cd_PopWindow ) {
      /*
       * Close the window.
       */
      CloseWindow( cd->cd_PopWindow );
      cd->cd_PopWindow = NULL;
   }

   /*
    * Close the font.
    */
   if ( cd->cd_Font ) {
      BGUI_CloseFont( cd->cd_Font );
      cd->cd_Font = NULL;
   }

   /*
    * Notify?
    */
   if ( cd->cd_Flags & CDF_NOTIFY ) {
      DoNotifyMethod( obj, gpgi->gpgi_GInfo, 0L, GA_ID, GADGET( obj )->GadgetID, CYC_Active, cd->cd_Active, TAG_END  );
      cd->cd_Flags &= ~CDF_NOTIFY;
   }

   return( AsmDoSuperMethodA( cl, obj, ( Msg )gpgi ));
}
METHOD_END
/// OM_GET
/// OM_GET
/*
 * Get an attribute.
 */
METHOD(CycleClassGet, struct opGet *, opg)
{
   CD        *cd = INST_DATA(cl, obj);
   ULONG      rc = 1, *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   case CYC_Active:
      STORE cd->cd_Active;
      break;

   case CYC_Popup:
      STORE cd->cd_Flags & CDF_POPUP;
      break;

   case CYC_PopActive:
      STORE cd->cd_Flags & CDF_POPACTIVE;
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
 * Dispose of the object.
 */
METHOD(CycleClassDispose, Msg, msg)
{
   CD       *cd = INST_DATA(cl, obj);

   if (cd->cd_ArrayLabel)
      DisposeObject(cd->cd_ArrayLabel);
   if (cd->cd_Image)
      DisposeObject(cd->cd_Image);

   if (cd->cd_Labels)
      BGUI_FreePoolMem(cd->cd_Labels);

   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * The object size is requested.
 */
METHOD(CycleClassDimensions, struct bmDimensions *, bmd)
{
   CD                *cd = INST_DATA(cl, obj);
   struct BaseInfo   *bi = bmd->bmd_BInfo;
   UBYTE            **labels = cd->cd_Labels, *label;
   int                mw = 0, mh = 0;
   struct TextExtent  te;

   /*
    * Find out the size of the largest label.
    */
   while (label = *labels++)
   {
      /*
       * Call TextExtent to find out the text width.
       */
      TextExtent(bi->bi_RPort, label, strlen(label), &te);
      if (te.te_Width  > mw) mw = te.te_Width;
      if (te.te_Height > mh) mh = te.te_Height;
   };

   /*
    * Include cycle/popup image width.
    */
   return CalcDimensions(cl, obj, bmd, mw + 30, mh + 2);
}
METHOD_END
///
/// WM_KEYACTIVE
/*
 * Key activation.
 */
METHOD(CycleClassKeyActive, struct wmKeyInput *, wmki)
{
   CD       *cd = INST_DATA(cl, obj);
   UWORD     qual = wmki->wmki_IEvent->ie_Qualifier;

   /*
    * Shifted is backwards, normal is forward.
    */
   if (qual & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
      cd->cd_Active--;
   else
      cd->cd_Active++;
   
   if (cd->cd_Active < 0) cd->cd_Active = cd->cd_NumLabels;
   if (cd->cd_Active > cd->cd_NumLabels) cd->cd_Active = 0;

   /*
    * Setup and re-render.
    */
   DoSetMethodNG(cd->cd_ArrayLabel, LAB_Label, cd->cd_Labels[cd->cd_Active], LAB_Place, PLACE_IN, TAG_END);
   DoNotifyMethod(obj, wmki->wmki_GInfo, 0, GA_ID, GADGET(obj)->GadgetID, CYC_Active, cd->cd_Active, TAG_END);
   DoRenderMethod(obj, wmki->wmki_GInfo, GREDRAW_REDRAW);

   /*
    * Setup object ID.
    */
   *(wmki->wmki_ID) = GADGET(obj)->GadgetID;

   return WMKF_VERIFY;
}
METHOD_END
///

/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,      (FUNCPTR)CycleClassRender,
   GM_RENDER,        (FUNCPTR)CycleClassRenderX,
   BASE_DIMENSIONS,  (FUNCPTR)CycleClassDimensions,

   OM_NEW,           (FUNCPTR)CycleClassNew,
   OM_SET,           (FUNCPTR)CycleClassSetUpdate,
   OM_UPDATE,        (FUNCPTR)CycleClassSetUpdate,
   OM_GET,           (FUNCPTR)CycleClassGet,
   OM_DISPOSE,       (FUNCPTR)CycleClassDispose,
   GM_GOACTIVE,      (FUNCPTR)CycleClassGoActive,
   GM_HANDLEINPUT,   (FUNCPTR)CycleClassHandleInput,
   GM_GOINACTIVE,    (FUNCPTR)CycleClassGoInactive,
   WM_KEYACTIVE,     (FUNCPTR)CycleClassKeyActive,
   DF_END,           NULL,
};

/*
 * Simple class initialization.
 */
makeproto Class *InitCycleClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
			 CLASS_ObjectSize,     sizeof(CD),
			 CLASS_DFTable,        ClassFunc,
			 TAG_DONE);
}
///
