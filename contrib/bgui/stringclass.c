/*
 * @(#) $Header$
 *
 * BGUI library
 * stringclass.c
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
 * Revision 42.0  2000/05/09 22:10:23  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:14  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.2  1999/08/05 01:40:28  mlemos
 * Added Jilles Tjoelker fix to assure that OM_GET always get a STRING_LongVal
 * value within the integer range limits.
 *
 * Revision 41.10.2.1  1998/07/05 19:27:59  mlemos
 * Made calls to RectFill be made to safe RectFill instead.
 *
 * Revision 41.10  1998/02/25 21:13:15  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:52  mlemos
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
   Object             *sd_StrGad;        /* strgclass object                 */
   UBYTE              *sd_TextContents;     /* contents of the string gadget   */
   LONG                sd_IntegerContents;     /* contents of the integer gadget  */
   LONG                sd_IntegerMin;       /* minimum integer contents         */
   LONG                sd_IntegerMax;       /* maximum integer contents         */
   struct TextFont    *sd_Font;         /* string gadget font     */
   ULONG               sd_Pens;          /* Copy of user-pens.     */
   ULONG               sd_Active;        /*   "   "   "    "                  */
   ULONG               sd_MinChars;      /* Min # of chars visible.      */
   UBYTE              *sd_Buffer;        /* BGUI allocated buffers    */
   UBYTE              *sd_UndoBuffer;       /*   "       "       "               */
   UBYTE              *sd_WorkBuffer;       /*   "       "       "               */
   UWORD               sd_Flags;         /* See below        */
   Object             *sd_T_View;
   WORD                sd_T_X, sd_T_Y;
   WORD                sd_T_W, sd_T_H;
   struct RastPort    *sd_T_Buffer;
   struct Window      *sd_T_Window;
} SD;

#define SDF_STRINGACTIVE    (1<<0)         /* The strgclass object is active  */

///


/*
 * Obtain the LongVal and TextVal
 * values from the strgclass object.
 */
//STATIC ASM VOID GetValues(REG(a0) SD *sd)
STATIC ASM REGFUNC1(VOID, GetValues,
	REGPARAM(A0, SD *, sd))
{
   Get_Attr(sd->sd_StrGad, STRINGA_TextVal, &sd->sd_TextContents);
   Get_Attr(sd->sd_StrGad, STRINGA_LongVal, &sd->sd_IntegerContents);
}
REGFUNC_END

/*
 * Object cleanup.
 */
//STATIC ASM ULONG StringClassDispose( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
STATIC ASM REGFUNC3(ULONG, StringClassDispose,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
   SD       *sd = INST_DATA(cl, obj);

   /*
    * Dispose of the strgadget.
    */
   DisposeObject(sd->sd_StrGad);

   /*
    * Kill the buffers.
    */
   if (sd->sd_Buffer    ) BGUI_FreePoolMem(sd->sd_Buffer    );
   if (sd->sd_UndoBuffer) BGUI_FreePoolMem(sd->sd_UndoBuffer);
   if (sd->sd_WorkBuffer) BGUI_FreePoolMem(sd->sd_WorkBuffer);

   /*
    * The rest is for the superclass.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
REGFUNC_END

/*
 * Allocate buffers. (Only when maxchars > 128)
 */
//STATIC ASM BOOL StringClassBuffers( REG(a0) SD *sd, REG(a1) struct TagItem *tags, REG(d0) ULONG max )
STATIC ASM REGFUNC3(BOOL, StringClassBuffers,
	REGPARAM(A0, SD *, sd),
	REGPARAM(A1, struct TagItem *, tags),
	REGPARAM(D0, ULONG, max))
{
   /*
    * We only allocate buffer when:
    *
    * A) The maximum amount of characters exceeds
    *    128 bytes.
    * B) The user did not supply his/her own
    *    buffers.
    */
   if ( ! FindTagItem( STRINGA_Buffer, tags )) {
      if ( ! ( sd->sd_Buffer = ( UBYTE * )BGUI_AllocPoolMem( max )))
         return( FALSE );
   }
   if ( ! FindTagItem( STRINGA_UndoBuffer, tags )) {
      if ( ! ( sd->sd_UndoBuffer = ( UBYTE * )BGUI_AllocPoolMem( max )))
         return( FALSE );
   }
   if ( ! FindTagItem( STRINGA_WorkBuffer, tags )) {
      if ( ! ( sd->sd_WorkBuffer = ( UBYTE * )BGUI_AllocPoolMem( max )))
         return( FALSE );
   }
   return( TRUE );
}
REGFUNC_END

/*
 * Create a shiny new object.
 */
//STATIC ASM ULONG StringClassNew( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opSet *ops )
STATIC ASM REGFUNC3(ULONG, StringClassNew,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops))
{
   SD             *sd;
   struct TagItem *tag, *tags;
   ULONG           rc;

   tags = DefTagList(BGUI_STRING_GADGET, ops->ops_AttrList);

   /*
    * First we let the superclass
    * create an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      sd = INST_DATA(cl, rc);

      /*
       * Force activation flags.
       */
      GADGET(rc)->Activation |= GACT_RELVERIFY;

      /*
       * Setup defaults.
       */
      sd->sd_IntegerMin = GetTagData(STRINGA_IntegerMin, 0xEFFFFFFF, tags);
      sd->sd_IntegerMax = GetTagData(STRINGA_IntegerMax, 0x0FFFFFFF, tags);

      /*
       * We take care of the GA_Disabled attribute
       * so we don't let the strgclass see it.
       */
      if (tag = FindTagItem(GA_Disabled, tags))
         tag->ti_Tag = TAG_IGNORE;

      /*
       * Handle NULL-STRINGA_TextVal.
       */
      if (tag = FindTagItem(STRINGA_TextVal, tags))
      {
         if (!tag->ti_Data)
            tag->ti_Data = (ULONG)"";
      }
      else if (tag = FindTagItem(STRINGA_LongVal, tags))
      {
         /*
          * Clamp the integer value between
          * min and max.
          */
         tag->ti_Data = range((LONG)tag->ti_Data, sd->sd_IntegerMin, sd->sd_IntegerMax);
      };

      /*
       * Work-around for STRINGA_MaxChars.
       */
      if (tag = FindTagItem(STRINGA_MaxChars, tags))
      {
         tag->ti_Data++;
         /*
          * See if we need to allocate the
          * buffers.
          */
         if (tag->ti_Data > 128)
         {
            if (!StringClassBuffers(sd, tags, tag->ti_Data))
               goto stringFail;
         };
      };

      /*
       * Create a strgclass object.
       */
      if (sd->sd_StrGad = NewObject( NULL, StrGClass,
                            sd->sd_Buffer     ? STRINGA_Buffer     : TAG_IGNORE, sd->sd_Buffer,
                            sd->sd_UndoBuffer ? STRINGA_UndoBuffer : TAG_IGNORE, sd->sd_UndoBuffer,
                            sd->sd_WorkBuffer ? STRINGA_WorkBuffer : TAG_IGNORE, sd->sd_WorkBuffer,
                            TAG_MORE, tags))
      {
         /*
          * Copy of the pens.
          */
         if (tag = FindTagItem(STRINGA_Pens,       tags)) sd->sd_Pens   = tag->ti_Data;
         if (tag = FindTagItem(STRINGA_ActivePens, tags)) sd->sd_Active = tag->ti_Data;

         /*
          * Obtain LongVal and TextVal values.
          */
         GetValues(sd);

         /*
          * Get the minimum number of chars always visible.
          */
         sd->sd_MinChars = GetTagData(STRINGA_MinCharsVisible, 2, tags);
         if (sd->sd_MinChars < 2) sd->sd_MinChars = 2;
      }
      else
      {
         stringFail:
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = 0;
      };
   };
   FreeTagItems(tags);

   return rc;
}
REGFUNC_END


/*
 * Change the object attributes.
 */
//STATIC ASM ULONG StringClassSetUpdate( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opUpdate *opu )
STATIC ASM REGFUNC3(ULONG, StringClassSetUpdate,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate *, opu))
{
   SD                *sd = INST_DATA(cl, obj);
   struct TagItem    *clones, *tag;
   WORD               dis = GADGET( obj )->Flags & GFLG_DISABLED, gv = 0;
   ULONG              inhibit, rc = 1;
   BC                *bc = BASE_DATA(obj);
   struct GadgetInfo *gi = opu->opu_GInfo;

   /*
    * First we let the superclass
    * change what it can.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * Clone tags.
    */
   if (clones = CloneTagItems(opu->opu_AttrList))
   {
      /*
       * Get min and max.
       */
      sd->sd_IntegerMin = GetTagData(STRINGA_IntegerMin, sd->sd_IntegerMin, clones);
      sd->sd_IntegerMax = GetTagData(STRINGA_IntegerMax, sd->sd_IntegerMax, clones);
      /*
       * Handle NULL-STRINGA_TextVal.
       */
      if (tag = FindTagItem(STRINGA_TextVal, clones))
      {
         if (!tag->ti_Data) tag->ti_Data = (ULONG)"";
         gv = TRUE;
      }
      else if (tag = FindTagItem(STRINGA_LongVal, clones))
      {
         /*
          * Clamp value between min and max.
          */
         tag->ti_Data = range((LONG)tag->ti_Data, sd->sd_IntegerMin, sd->sd_IntegerMax);
         gv = TRUE;
      }

      /*
       * We handle GA_Disabled.
       */
      if (tag = FindTagItem(GA_Disabled, clones))
         tag->ti_Tag = TAG_IGNORE;

      /*
      if (tag = FindTagItem(GA_Left,   clones)) tag->ti_Data += bc->bc_InnerBox.Left   - bc->bc_OuterBox.Left;
      if (tag = FindTagItem(GA_Top,    clones)) tag->ti_Data += bc->bc_InnerBox.Top    - bc->bc_OuterBox.Top;
      if (tag = FindTagItem(GA_Width,  clones)) tag->ti_Data += bc->bc_InnerBox.Width  - bc->bc_OuterBox.Width;
      if (tag = FindTagItem(GA_Height, clones)) tag->ti_Data += bc->bc_InnerBox.Height - bc->bc_OuterBox.Height;
      */

      /*
       * Rendering inhibited?
       */
      Get_SuperAttr(cl, obj, BT_Inhibit, &inhibit);

      /*
       * Set strgclass attributes.
       */
      AsmDoMethod(sd->sd_StrGad, opu->MethodID, clones, inhibit ? NULL : gi, opu->opu_Flags);

      /*
       * Force activation flags.
       */
      GADGET( obj )->Activation |= GACT_RELVERIFY;

      /*
       * Setting a new font? Save it..
       */
      if (opu->MethodID == OM_SET)
      {
         if (tag = FindTagItem(BT_TextAttr, clones))
         {
            Get_SuperAttr(cl, obj, BT_TextFont, &sd->sd_Font);
            DoSetMethodNG(sd->sd_StrGad, STRINGA_Font, sd->sd_Font, TAG_END);
         }
         /*
          * Copy of the pens.
          */
         if (tag = FindTagItem(STRINGA_Pens,       clones)) sd->sd_Pens   = tag->ti_Data;
         if (tag = FindTagItem(STRINGA_ActivePens, clones)) sd->sd_Active = tag->ti_Data;
      }

      /*
       * Get the LongVal and TextVal values.
       */
      GetValues(sd);

      /*
       * Change visually?
       */
      if (((GADGET(obj)->Flags & GFLG_DISABLED) != dis) || gv)
         DoRenderMethod(obj, gi, GREDRAW_UPDATE);

      /*
       * When GACT_LONGINT is set we notify
       * STRINGA_LongVal when it changed. Otherwise
       * we notify STRINGA_TextVal.
       */
      if (gv)
      {
         if (GADGET(sd->sd_StrGad)->Activation & GACT_LONGINT)
            DoNotifyMethod(obj, gi, opu->opu_Flags, GA_ID, GADGET(obj)->GadgetID, STRINGA_LongVal, sd->sd_IntegerContents, TAG_END);
         else
            DoNotifyMethod(obj, gi, opu->opu_Flags, GA_ID, GADGET(obj)->GadgetID, STRINGA_TextVal, sd->sd_TextContents, TAG_END);
      }

      /*
       * Free the clones.
       */
      FreeTagItems(clones);
   } else
      rc = 0;

   return rc;
}
REGFUNC_END

/// OM_GET
/*
 * Get an attribute.
 */
METHOD(StringClassGet, struct opGet *, opg)
{
   SD          *sd = INST_DATA(cl, obj);
   ULONG        rc = 1, *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   case STRINGA_StringInfo:
      /*
       * Give 'm the pointer to the
       * StringInfo structure.
       */
      STORE GADGET( sd->sd_StrGad )->SpecialInfo;
      break;

   default:
      /*
       * First we try the strgclass object.
       */
      rc = AsmDoMethodA(sd->sd_StrGad, (Msg)opg);

      /*
       * Then the superclass.
       */
      if (!rc) rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);

      /*
       * Clamp the integer value between min and max
       * Use: if the program does GetAttr() when the user has typed a
       * value but did not press return, or if the strgclass OM_GET failed
       * for some reason.
       */
      if (opg->opg_AttrID == STRINGA_LongVal) {
         *(opg->opg_Storage) = range(*(opg->opg_Storage), sd->sd_IntegerMin, sd->sd_IntegerMax);
      }
      break;
   };
   return rc;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the gadget.
 */
METHOD(StringClassRender, struct bmRender *, bmr)
{
   SD              *sd = INST_DATA(cl, obj);
   BC              *bc = BASE_DATA(obj);
   struct BaseInfo *bi = bmr->bmr_BInfo;
   ULONG            rc, mypens;

   /*
    * First the superclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Setup default pens.
    */
   mypens = PACKPENS(bi->bi_Pens[TEXTPEN], bi->bi_Pens[BACKGROUNDPEN]);

   /*
    * Setup string object size and pens.
    */
   *GADGETBOX(sd->sd_StrGad) = bc->bc_InnerBox;

   DoSetMethodNG(sd->sd_StrGad, STRINGA_Pens,       sd->sd_Pens   ? sd->sd_Pens   : mypens,
                                STRINGA_ActivePens, sd->sd_Active ? sd->sd_Active : mypens,
                                TAG_DONE);

   /*
    * Let the strgclass render itself.
    */
   BClearAfPt(bi);
   rc = AsmDoMethod(sd->sd_StrGad, GM_RENDER, bi, bi->bi_RPort, GREDRAW_REDRAW);

   /*
    * Disable the gadget when necessary.
    */
   if (GADGET(obj)->Flags & GFLG_DISABLED) BDisableBox(bi, &bc->bc_InnerBox);

   return rc;
}
METHOD_END
///
/// GM_HITTEST
/*
 * Did they click in the strgclass object?
 */
METHOD(StringClassHitTest, struct gpHitTest *, gph)
{
   SD          *sd = INST_DATA(cl, obj);
   ULONG        rc = 0;

   /*
    * Only when not disabled.
    */
   if (!(GADGET(obj)->Flags & GFLG_DISABLED))
   {
      /*
       * Hit inside the object?
       */
      if (AsmDoSuperMethodA(cl, obj, (Msg)gph) == GMR_GADGETHIT)
      {
         /*
          * Pass on to the strgclass object.
          */
         rc = AsmDoMethodA(sd->sd_StrGad, (Msg)gph);
      };
   }
   return rc;
}
METHOD_END
///
/// GM_GOACTIVE
/*
 * Go to the active state and/or handle the user input.
 */
METHOD(StringClassGoActive, struct gpInput *, gpi)
{
   SD                *sd = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   ULONG              rc = GMR_NOREUSE;
   struct GadgetInfo *gi;
   struct RastPort   *rp;
   struct Window     *win;
   int                x, y, w, h, wx1, wy1, wx2, wy2;

   /*
    * Only if not disabled.
    */
   if (!(GADGET(obj)->Flags & GFLG_DISABLED))
   {
      if (bc->bc_View)
      {
         /*
          * Get coordinates of object.
          */
         Get_Attr(bc->bc_View, VIEW_AbsoluteX, (ULONG *)&x);
         Get_Attr(bc->bc_View, VIEW_AbsoluteY, (ULONG *)&y);

         gi = gpi->gpi_GInfo;
         win = gi->gi_Window;

         x = bc->bc_OuterBox.Left + x - 2;
         y = bc->bc_OuterBox.Top  + y - 2;
         w = bc->bc_OuterBox.Width    + 4;
         h = bc->bc_OuterBox.Height   + 4;

         /*
          * Get bounds of master group.
          */
         wx1 = win->BorderLeft;
         wy1 = win->BorderTop;
         wx2 = win->Width  - win->BorderRight;
         wy2 = win->Height - win->BorderBottom;

         /*
          * Clip to fit in the master group.
          */
         if (x < wx1) x = wx1;
         if (y < wy1) y = wy1;

         if ((x + w) > wx2) x -= (x + w) - wx2;
         if ((y + h) > wy2) y -= (y + h) - wy2;

         if (x < wx1) { x = wx1; w = wx2 - wx1; };
         if (y < wy1) { y = wy1; h = wy2 - wy1; };

         if (rp = BGUI_ObtainGIRPort(gi))
         {
            if (!(sd->sd_T_Buffer = BGUI_CreateRPortBitMap(rp, w, h, FGetDepth(rp))))
            {
               ReleaseGIRPort(rp);
               return rc;
            };

            /*
             * Store deltas.
             */
            sd->sd_T_X = (x + 2) - bc->bc_OuterBox.Left;
            sd->sd_T_Y = (y + 2) - bc->bc_OuterBox.Top;
            sd->sd_T_W = (w - 4) - bc->bc_OuterBox.Width;
            sd->sd_T_H = (h - 4) - bc->bc_OuterBox.Height;

            sd->sd_T_Window = win;
            sd->sd_T_View   = bc->bc_View;
            bc->bc_View     = NULL;

            /*
             * Save area to buffer.
             */
            ClipBlit(rp, x, y, sd->sd_T_Buffer, 0, 0, w, h, 0xC0);

            AsmDoMethod(obj, BASE_MOVEBOUNDS, sd->sd_T_X, sd->sd_T_Y, sd->sd_T_W, sd->sd_T_H, TAG_DONE);
 
            /*
             * Clear the area and render the object.
             */
            SetAPen(rp, 0);
            SRectFill(rp, x, y, x + w - 1, y + h - 1);
            AsmDoMethod(obj, GM_RENDER, gi, rp, GREDRAW_REDRAW);

            ReleaseGIRPort(rp);
         };
      };

      /*
       * Let the superclass have a go...
       */
      AsmDoSuperMethodA(cl, obj, (Msg)gpi);

      /*
       * Pass message on.
       */
      if ((rc = AsmDoMethodA(sd->sd_StrGad, (Msg)gpi)) == GMR_MEACTIVE)
         /*
          * Mark us as active.
          */
         sd->sd_Flags |= SDF_STRINGACTIVE;
   }

   return rc;
}
METHOD_END
///
/// GM_HANDLEINPUT
/*
 * Handle the user input.
 */
METHOD(StringClassHandleInput, struct gpInput *, gpi)
{
   SD                *sd = INST_DATA(cl, obj);
   ULONG              rc;
   LONG               val;
   struct GadgetInfo *gi = gpi->gpi_GInfo;

   /*
    * Pass on the message.
    */
   rc = AsmDoMethodA(sd->sd_StrGad, (Msg)gpi);

   /*
    * RETURN or TAB or SHIFT TAB?
    */
   if (rc & GMR_VERIFY)
   {
      GetValues(sd);

      /*
       * Check if the entered value of an integer object is in range and,
       * if not, adjust it and don't go inactive.
       */
      if (GADGET(sd->sd_StrGad)->Activation & GACT_LONGINT)
      {
         /*
          * Get the value.
          */
         Get_Attr(sd->sd_StrGad, STRINGA_LongVal, &val);
         /*
          * Out of range?
          */
         if (val < sd->sd_IntegerMin)
         {
            val = sd->sd_IntegerMin;
            rc = GMR_MEACTIVE;
         }
         else if (val > sd->sd_IntegerMax)
         {
            val = sd->sd_IntegerMax;
            rc = GMR_MEACTIVE;
         }
         if (rc == GMR_MEACTIVE)
         {
            sd->sd_IntegerContents = val;
            DisplayBeep(gi->gi_Window->WScreen);
            DoSetMethod(sd->sd_StrGad, gi, STRINGA_LongVal, val, TAG_END);
         };
            
         DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, STRINGA_LongVal, sd->sd_IntegerContents, TAG_END);
      } else
         DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, STRINGA_TextVal, sd->sd_TextContents, TAG_END);

      /*
       * Shift-tabbed?
       */
      if (rc & GMR_NEXTACTIVE)
      {
         /*
          * Tell the window we're in
          * to activate the previous
          * GA_TabCycle object.
          */
         DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, STRINGA_Tabbed, obj, TAG_END);
         /*
          * We handle this ourselves.
          */
         rc &= ~GMR_NEXTACTIVE;
      }
      else if (rc & GMR_PREVACTIVE)
      {
         /*
          * Tell the window we're in
          * to activate the next
          * GA_TabCycle object.
          */
         DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, STRINGA_ShiftTabbed, obj, TAG_END);
         /*
          * We handle this ourselves.
          */
         rc &= ~GMR_PREVACTIVE;
      };
   };
   return rc;
}
METHOD_END
///
/// GM_GOINACTIVE
/*
 * Go inactive.
 */
METHOD(StringClassGoInActive, struct gpGoInactive *, gpi)
{
   SD       *sd = INST_DATA(cl, obj);
   BC       *bc = BASE_DATA(obj);

   int       x, y, w, h;

   /*
    * Pass on to the string object
    * if it was active.
    */
   if (sd->sd_Flags & SDF_STRINGACTIVE)
   {
      AsmDoMethodA(sd->sd_StrGad, (Msg)gpi);
      /*
       * Were not active nomore.
       */
      sd->sd_Flags &= ~SDF_STRINGACTIVE;
   };

   if (sd->sd_T_View)
   {
      bc->bc_View = sd->sd_T_View;
      sd->sd_T_View = NULL;

      x = bc->bc_OuterBox.Left   - 2;
      y = bc->bc_OuterBox.Top    - 2;
      w = bc->bc_OuterBox.Width  + 4;
      h = bc->bc_OuterBox.Height + 4;

      AsmDoMethod(obj, BASE_MOVEBOUNDS, -sd->sd_T_X, -sd->sd_T_Y, -sd->sd_T_W, -sd->sd_T_H, TAG_DONE);

      ClipBlit(sd->sd_T_Buffer, 0, 0, sd->sd_T_Window->RPort, x, y, w, h, 0xC0);
      BGUI_DoGadgetMethod(obj, sd->sd_T_Window, NULL, GM_RENDER, NULL, sd->sd_T_Window->RPort, GREDRAW_REDRAW);
      BGUI_FreeRPortBitMap(sd->sd_T_Buffer);
   };

   return AsmDoSuperMethodA(cl, obj, (Msg)gpi);
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * The object size is requested.
 */
METHOD(StringClassDimensions, struct bmDimensions *, bmd)
{
   SD              *sd = INST_DATA(cl, obj);
   struct BaseInfo *bi = bmd->bmd_BInfo;
   int              tlen;

   if (sd->sd_Font) BSetFont(bi, sd->sd_Font);

   /*
    * We must make sure that at least sd_MinChars
    * characters will fit.
    */
   tlen = TextWidth(bi->bi_RPort, "M");

   /*
    * Set it up.
    */
   return CalcDimensions(cl, obj, bmd, tlen * sd->sd_MinChars + (tlen >> 1), bi->bi_RPort->TxHeight);
}
METHOD_END
///
/// WM_KEYACTIVE
/*
 * Keyboard activation.
 */
METHOD(StringClassKeyActive, struct wmKeyInput *, wmki)
{
   /*
    * Let intuition do it.
    */
   return WMKF_ACTIVATE;
}
METHOD_END
///
/// SM_FORMAT_STRING
/*
 * Format string to the string object.
 */
METHOD(StringClassFString, struct smFormatString *, smfs)
{
   SD       *sd = INST_DATA(cl, obj);
   UBYTE    *sb;
   ULONG     rc = 0;

   /*
    * Only on string objects.
    */
   if (!(GADGET(obj)->Activation & GACT_LONGINT))
   {
      /*
       * Allocate string buffer.
       */
      if (sb = (UBYTE *)BGUI_AllocPoolMem(CompStrlenF(smfs->smfs_FStr, &smfs->smfs_Arg1)))
      {
         /*
          * Format and set it.
          */
         DoSPrintF(sb, smfs->smfs_FStr, &smfs->smfs_Arg1);
         DoSetMethod(sd->sd_StrGad, smfs->smfs_GInfo, STRINGA_TextVal, sb, TAG_END);
         /*
          * Free the string.
          */
         BGUI_FreePoolMem(sb);
         rc = 1;
      }
   }
   return rc;
}
METHOD_END
///

/// Class initialization.
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,         (FUNCPTR)StringClassRender,
   BASE_DIMENSIONS,     (FUNCPTR)StringClassDimensions,

   OM_NEW,              (FUNCPTR)StringClassNew,
   OM_SET,              (FUNCPTR)StringClassSetUpdate,
   OM_UPDATE,           (FUNCPTR)StringClassSetUpdate,
   OM_GET,              (FUNCPTR)StringClassGet,
   OM_DISPOSE,          (FUNCPTR)StringClassDispose,
   GM_HITTEST,          (FUNCPTR)StringClassHitTest,
   GM_GOACTIVE,         (FUNCPTR)StringClassGoActive,
   GM_HANDLEINPUT,      (FUNCPTR)StringClassHandleInput,
   GM_GOINACTIVE,       (FUNCPTR)StringClassGoInActive,
   WM_KEYACTIVE,        (FUNCPTR)StringClassKeyActive,
   SM_FORMAT_STRING,    (FUNCPTR)StringClassFString,
   DF_END,              NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitStringClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(SD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
