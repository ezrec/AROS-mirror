/*
 * @(#) $Header$
 *
 * BGUI library
 * progressclass.c
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
 * Revision 42.1  2000/05/15 19:27:02  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:09:53  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:53  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.3  1999/07/23 19:44:34  mlemos
 * Added missing setting of text pen to draw the progress done value.
 * Ensured that text drawing is done in complement mode if the text pen is the
 * the same as the fill pen or background pen.
 *
 * Revision 41.10.2.2  1998/11/16 20:00:23  mlemos
 * Replaced a FreeVec call by BGUI_FreePoolMem.
 *
 * Revision 41.10.2.1  1998/06/21 21:14:32  mlemos
 * Prevented the Render method to draw an invalid size bar.
 *
 * Revision 41.10  1998/02/25 21:12:50  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:24  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/// Class definitions.
/*
 * The instance data used by the
 * objects of this class.
 */
typedef struct ProgressData {
   LONG             pd_Min;         /* minimum progression   */
   LONG             pd_Max;         /* maximum progression   */
   LONG             pd_Done;        /* current progression   */
   LONG             pd_Divisor;     /* Done divisor          */
   UWORD            pd_Flags;       /* see below             */

   UBYTE           *pd_Text;
   UBYTE           *pd_Buffer;
   ULONG            pd_BufSize;
   struct TextFont *pd_Font;
} PD;

#define PDF_VERTICAL         (1<<0)

///
/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(ProgressClassNew, struct opSet *, ops)
{
   PD             *pd;
   struct TagItem *tags;
   ULONG           rc;

   tags = DefTagList(BGUI_PROGRESS_GADGET, ops->ops_AttrList);

   /*
    * Let the superclass setup an object for us.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      pd = INST_DATA(cl, rc);

      /*
       * Setup gadget.
       */
      pd->pd_Max     = 100;
      pd->pd_Divisor = 1;

      AsmDoMethod((Object *)rc, OM_SET, ops->ops_AttrList, NULL);
   };
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_SET, OM_UPDATE
/*
 * Change the object attributes.
 */
METHOD(ProgressClassSetUpdate, struct opUpdate *, opu)
{
   PD              *pd = INST_DATA(cl, obj);
   struct TagItem  *attr = opu->opu_AttrList, *tstate = attr, *tag;
   ULONG            tmp, data, odone = pd->pd_Done;
   BOOL             vc = FALSE;

   /*
    * First let the superclass do it's thing.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * Set attributes we know.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch ( tag->ti_Tag )
      {
      case PROGRESS_Min:
         pd->pd_Min = data;
         vc = TRUE;
         break;

      case PROGRESS_Max:
         pd->pd_Max = data;
         vc = TRUE;
         break;

      case PROGRESS_Done:
         pd->pd_Done = data / pd->pd_Divisor;
         vc = TRUE;
         break;

      case PROGRESS_Divisor:
         pd->pd_Divisor = data ? data : 1;
         break;

      case PROGRESS_FormatString:
         pd->pd_Text = (UBYTE *)data;
         vc = TRUE;
         break;

      case PROGRESS_Vertical:
         if (data) pd->pd_Flags |=  PDF_VERTICAL;
         else      pd->pd_Flags &= ~PDF_VERTICAL;
         vc = TRUE;
         break;

      case BT_TextAttr:
         Get_SuperAttr(cl, obj, BT_TextFont, &pd->pd_Font);
         break;
      }
   }

   /*
    * Sanity check.
    */
   if (pd->pd_Max < pd->pd_Min)
   {
      tmp = pd->pd_Min; pd->pd_Min = pd->pd_Max; pd->pd_Max = tmp;
   }
   else if (pd->pd_Max == pd->pd_Min)
      pd->pd_Max++;

   if (pd->pd_Done > pd->pd_Max)      pd->pd_Done = pd->pd_Max;
   else if (pd->pd_Done < pd->pd_Min) pd->pd_Done = pd->pd_Min;

   /*
    * Do we need to change visually?
    */
   if (vc)
      DoRenderMethod(obj, opu->opu_GInfo, GREDRAW_UPDATE);

   /*
    * Notify the target.
    */
   if (odone != pd->pd_Done)
      DoNotifyMethod(obj, opu->opu_GInfo, 0, GA_ID, GADGET(obj)->GadgetID, PROGRESS_Done, pd->pd_Done, TAG_END);

   return 1;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Hmm, they want us out of here.
 */
METHOD(ProgressClassDispose, Msg, msg)
{
   PD            *pd = INST_DATA(cl, obj);

   /*
    * Deallocate the string buffer.
    */
   if (pd->pd_Buffer) BGUI_FreePoolMem(pd->pd_Buffer);

   /*
    * And let the superclass take
    * care of the rest.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// BASE_RENDER
/*
 * Scale the area of the indicator
 * that needs to be filled.
 */
#define ScaleFill(s,m,d) UDivMod32( UMult32( d, s ) + UDivMod32( m, 2 ), m )

/*
 * Render the gadget imagery
 */
METHOD(ProgressClassRender, struct bmRender *, bmr)
{
   PD                *pd = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   struct BaseInfo   *bi = bmr->bmr_BInfo;
   struct RastPort   *rp = bi->bi_RPort;
   struct IBox        box1, box2;
   LONG               done, min, max;
   LONG               filled, space;
   UBYTE             *str;

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   min  = pd->pd_Min;
   max  = pd->pd_Max;
   done = pd->pd_Done;

   box1 = bc->bc_InnerBox;
   box2 = bc->bc_InnerBox;

   space = (pd->pd_Flags & PDF_VERTICAL) ? box1.Height : box1.Width;

   /*
    * Scale fill.
    */
   filled = ScaleFill(space, ABS(max - min), ABS(done));

   /*
    * Keep within the bounds.
    */
   if      (done >= max)    filled = space;
   else if (done <= min)    filled = 0;
   if      (filled > space) filled = space;

   /*
    * Vertical indication?
    */
   if (!(pd->pd_Flags & PDF_VERTICAL))
   {
      box1.Width  = filled;
      box2.Left  += filled;
      box2.Width -= filled;
   }
   else
   {
      box1.Top    += box1.Height - filled;
      box1.Height  = filled;
      box2.Height -= filled;
   };

   /*
    * Convert to Rectangle structures.
    * Render filled area and clear the rest.
    */
   if(box1.Width>0
   && box1.Height>0)
   {
      box1.Width  += box1.Left - 1;
      box1.Height += box1.Top  - 1;
      AsmDoMethod(bc->bc_Frame, FRAMEM_BACKFILL, bi, &box1, IDS_SELECTED);
   }
   if(box2.Width>0
   && box2.Height>0)
   {
      box2.Width  += box2.Left - 1;
      box2.Height += box2.Top  - 1;
      AsmDoMethod(bc->bc_Frame, FRAMEM_BACKFILL, bi, &box2, IDS_NORMAL);
   }

   /*
    * Render textual indicator when present.
    */
   if (pd->pd_Text)
   {
      /*
       * Setup the font.
       */
      if (pd->pd_Font) BSetFont(bi, pd->pd_Font);

      /*
       * Do we have a text?
       */
      if ((str = DoBuffer(pd->pd_Text, &pd->pd_Buffer, &pd->pd_BufSize, (ULONG *)&pd->pd_Done)) && strlen(str))
      {
         ULONG apen;
         STRPTR formatted=NULL;

         BGUI_InfoTextSize(rp, str, (UWORD *)&box1.Width, (UWORD *)&box1.Height);

         box1.Left = bc->bc_InnerBox.Left + ((bc->bc_InnerBox.Width  - box1.Width)  >> 1);
         box1.Top  = bc->bc_InnerBox.Top  + ((bc->bc_InnerBox.Height - box1.Height) >> 1);

         FSetAPen(rp,apen=bi->bi_DrInfo->dri_Pens[TEXTPEN]);
         if((apen!=bi->bi_DrInfo->dri_Pens[FILLPEN]
         && apen!=bi->bi_DrInfo->dri_Pens[BACKGROUNDPEN])
         || (formatted=BGUI_AllocPoolMem(strlen(ISEQ_COMPLEMENT)+strlen(str)+1)))
         {
            if(formatted)
            {
               strcpy(formatted,ISEQ_COMPLEMENT);
               strcpy(formatted+strlen(ISEQ_COMPLEMENT),str);
               str=formatted;
            }
            RenderText(bi, str, &box1);
            if(formatted)
               BGUI_FreePoolMem(formatted);
         }
      };
   };
   return 1;
}
METHOD_END
///
/// OM_GET
/*
 * They want to know an attribute value.
 */
METHOD(ProgressClassGet, struct opGet *, opg)
{
   PD          *pd = INST_DATA(cl, obj);
   ULONG        rc = 1, *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   case PROGRESS_Done:
      STORE pd->pd_Done;
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
 * We do not respond to
 * clicking in the gadget.
 */
METHOD(ProgressClassHitTest, struct gpHitTest *, gph)
{
   return 0;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * Our parent group needs to know
 * something about our dimensions.
 */
METHOD(ProgressClassDimensions, struct bmDimensions *, bmd)
{
   PD               *pd = INST_DATA(cl, obj);
   struct RastPort  *rp = bmd->bmd_BInfo->bi_RPort;
   UWORD             mw = 2, mh = 2, l;
   UBYTE            *str;

   /*
    * Any text?
    */
   if (pd->pd_Text)
   {
      /*
       * Obtain string.
       */
      if (str = DoBuffer(pd->pd_Text, &pd->pd_Buffer, &pd->pd_BufSize, (ULONG *)&pd->pd_Min))
      {
         /*
          * Compute size of minimum level.
          */
         l = TotalWidth(rp, str);

         /*
          * Obtain string.
          */
         if (str = DoBuffer(pd->pd_Text, &pd->pd_Buffer, &pd->pd_BufSize, (ULONG *)&pd->pd_Max))
         {
            /*
             * Compute size of maximum level.
             */
            mw = TotalWidth (rp, str);
            mh = TotalHeight(rp, str);

            /*
             * Take the largest.
             */
            mw = (mw > l) ? mw : l;

            /*
             * Add the width of 2/3 of a single space.
             */
            mw += (TextWidthNum(rp, "  ", 1) / 3);
         };
      };
   };
   /*
    * Setup minimum dimensions.
    */
   return CalcDimensions(cl, obj, bmd, mw, mh);
}
METHOD_END
///
/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,       (FUNCPTR)ProgressClassRender,
   BASE_DIMENSIONS,   (FUNCPTR)ProgressClassDimensions,

   OM_NEW,            (FUNCPTR)ProgressClassNew,
   OM_GET,            (FUNCPTR)ProgressClassGet,
   OM_SET,            (FUNCPTR)ProgressClassSetUpdate,
   OM_UPDATE,         (FUNCPTR)ProgressClassSetUpdate,
   OM_DISPOSE,        (FUNCPTR)ProgressClassDispose,
   GM_HITTEST,        (FUNCPTR)ProgressClassHitTest,
   DF_END,            NULL
};

/*
 * Initialize the progress class.
 */
makeproto Class *InitProgressClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(PD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
