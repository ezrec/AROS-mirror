/*
 * @(#) $Header$
 *
 * BGUI library
 * separatorclass.c
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
 * Revision 42.2  2000/08/17 15:09:18  chodorowski
 * Fixed compiler warnings.
 *
 * Revision 42.1  2000/05/15 19:27:02  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:10:11  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:05  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.2  1999/07/28 23:18:15  mlemos
 * Fixed superclass call to dispose the objects.
 *
 * Revision 41.10.2.1  1998/11/23 14:59:51  mlemos
 * Removed needless OM_GET method code.
 *
 * Revision 41.10  1998/02/25 21:13:06  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:44  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.

#include "include/classdefs.h"

/*
 * Object Instance data.
 */
typedef struct {
   ULONG            sd_Flags;     /* See below.          */
   Object          *sd_Title;     /* Separator title.    */
   UWORD            sd_Place;     /* Position.           */
}  SD;

#define SDF_THIN        (1<<0)         /* Thin lines.          */
#define SDF_HIGHLIGHT   (1<<1)         /* Highlight title.     */
#define SDF_HORIZONTAL  (1<<2)         /* Horizontal separator.*/
#define SDF_CENTER      (1<<3)         /* Center title.        */
#define SDF_RECESSED    (1<<4)         /* Recess lines.        */

///
/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(SepClassNew, struct opSet *, ops)
{
   ULONG           rc;

   /*
    * Let the superclass create an object.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      AsmCoerceMethod(cl, (Object *)rc, OM_SET, ops->ops_AttrList, NULL);
   }
   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Dispose of this.
 */
METHOD(SepClassDispose, Msg, msg)
{
   SD               *sd = INST_DATA(cl, obj);

   if (sd->sd_Title) DisposeObject(sd->sd_Title);

   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// OM_SET
/*
 * Set.
 */
METHOD(SepClassSet, struct opSet *, ops)
{
   SD              *sd = INST_DATA(cl, obj);
   struct TagItem  *tag, *tstate = ops->ops_AttrList;
   ULONG            rc, data;

   rc = AsmDoSuperMethodA(cl, obj, (Msg)ops);

   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case FRM_ThinFrame:
      case SEP_Thin:
	 /*
	  * Pickup thin lines.
	  */
	 if (data) sd->sd_Flags |= SDF_THIN;
	 else      sd->sd_Flags &= ~SDF_THIN;
	 break;

      case SEP_CenterTitle:
	 if (data) sd->sd_Flags |= SDF_CENTER;
	 else      sd->sd_Flags &= ~SDF_CENTER;
	 break;

      case SEP_Horiz:
	 if (data) sd->sd_Flags |= SDF_HORIZONTAL;
	 else      sd->sd_Flags &= ~SDF_HORIZONTAL;
	 break;

      case SEP_Highlight:
	 if (data) sd->sd_Flags |= SDF_HIGHLIGHT;
	 else      sd->sd_Flags &= ~SDF_HIGHLIGHT;
	 break;

      case SEP_Recessed:
	 if (data) sd->sd_Flags |= SDF_RECESSED;
	 else      sd->sd_Flags &= ~SDF_RECESSED;
	 break;

      case SEP_TitleLeft:
	 if (data) sd->sd_Place = 1;
	 else      sd->sd_Place = 0;
	 break;

      case SEP_TitleRight:
	 if (data) sd->sd_Place = 2;
	 else      sd->sd_Place = 0;
	 break;

      case SEP_Title:
	 if (!sd->sd_Title) sd->sd_Title = BGUI_NewObject(BGUI_TEXT_GRAPHIC, TEXTA_CopyText, TRUE, TAG_DONE);
	 if (sd->sd_Title) DoSetMethodNG(sd->sd_Title, TEXTA_Text, data, TAG_DONE);
	 break;

      case SEP_TitleID:
	 if (!sd->sd_Title) sd->sd_Title = BGUI_NewObject(BGUI_TEXT_GRAPHIC, TEXTA_CopyText, TRUE, TAG_DONE);
	 if (sd->sd_Title) DoSetMethodNG(sd->sd_Title, TEXTA_TextID, data, TAG_DONE);
	 break;
      };
   };
   return rc;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render.
 */
METHOD(SepClassRender, struct bmRender *, bmr)
{
   SD               *sd = INST_DATA(cl, obj);
   BC               *bc = BASE_DATA(obj);
   struct BaseInfo  *bi = bmr->bmr_BInfo;
   struct RastPort  *rp = bi->bi_RPort;
   int               l, t, w, h, s;
   BOOL              recessed;

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Get left and top.
    */
   l = bc->bc_OuterBox.Left;
   t = bc->bc_OuterBox.Top;
   w = bc->bc_OuterBox.Width;
   h = bc->bc_OuterBox.Height;

   recessed = sd->sd_Flags & SDF_RECESSED;

   /*
    * Only a single pen.
    */
   BSetDrMd(bi, JAM1);

   /*
    * Horizontal?
    */
   if (sd->sd_Flags & SDF_HORIZONTAL)
   {
      /*
       * Shiny side.
       */
      BSetDPenA(bi, recessed ? SHADOWPEN : SHINEPEN);

      /*
       * Center it in the given space?
       */
      if (sd->sd_Title)
      {
	 if (sd->sd_Flags & SDF_CENTER) t += h >> 1;
	 else                           t += h - (rp->TxHeight - rp->TxBaseline);
      };

      /*
       * Move to the correct location.
       */
      Move(rp, l, t);
      Draw(rp, l + w - 2, t);
      WritePixel(rp, l, t + 1);

      /*
       * Shadow side.
       */
      BSetDPenA(bi, recessed ? SHINEPEN : SHADOWPEN);

      /*
       * Move to the correct location.
       */
      Move(rp, l + 1, t + 1);
      Draw(rp, l + w - 1, t + 1);
      WritePixel(rp, l + w - 1, t);

      /*
       * Title?
       */
      if (sd->sd_Title)
      {
	 /*
	  * Set font if necessary.
	  */
	 if (bc->bc_TextFont) BSetFont(bi, bc->bc_TextFont);

	 /*
	  * Render the title.
	  */
	 RenderTitle(sd->sd_Title, bi, l, t, w,
		     sd->sd_Flags & SDF_HIGHLIGHT, sd->sd_Flags & SDF_CENTER, sd->sd_Place);
      }
   }
   else
   {
      /*
       * Shiny side.
       */
      BSetDPenA(bi, recessed ? SHADOWPEN : SHINEPEN);

      /*
       * Thin or thick.
       */
      s = sd->sd_Flags & SDF_THIN ? 1 : 2;

      /*
       * Pick position.
       */
      l += (w >> 1) - s;

      /*
       * Render shiny side.
       */
      Move(rp, l, t);
      Draw(rp, l, t + h - 1);

      /*
       * Thick?
       */
      if (!(sd->sd_Flags & SDF_THIN))
      {
	 Draw(rp, l + 1, t + h - 1);
	 Draw(rp, l + 1, t);
	 Draw(rp, l + 3, t);
      }
      else
      {
	 Move(rp, l, t);
	 Draw(rp, l + 1, t);
      }

      /*
       * Shadow side.
       */
      BSetDPenA(bi, recessed ? SHINEPEN : SHADOWPEN);

      /*
       * Render.
       */
      Move(rp, l + s, t + 1);
      Draw(rp, l + s, t + h - 1);

      /*
       * Thick?
       */
      if (!(sd->sd_Flags & SDF_THIN))
      {
	 s++;
	 Draw(rp, l + s, t + h - 1);
	 Draw(rp, l + s, t + 1);
	 Move(rp, l, t + h - 1);
	 Draw(rp, l + s, t + h - 1);
      }
      else
	 Draw(rp, l, t + h - 1);
   };
   return 1;
}
METHOD_END
///
/// GM_HITTEST
/*
 * Hit test.
 */
METHOD(SepClassHitTest, Msg, msg)
{
   return 0;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * Dimensions.
 */
METHOD(SepClassDimensions, struct bmDimensions *, bmd)
{
   SD               *sd = INST_DATA(cl, obj);
   struct BaseInfo  *bi = bmd->bmd_BInfo;
   BC               *bc = BASE_DATA(obj);
   int               mx, my;

   /*
    * Minimum width.
    */
   if (sd->sd_Flags & SDF_HORIZONTAL)
   {
      /*
       * Do we have a font?
       */
      if (bc->bc_TextFont && sd->sd_Title)
	 BSetFont(bi, bc->bc_TextFont);

      mx = 2;

      if (sd->sd_Title) my = bi->bi_RPort->TxHeight;
      else              my = 2;
   }
   else
   {
      mx = sd->sd_Flags & SDF_THIN ? 2 : 4;
      my = 2;
   };
   return CalcDimensions(cl, obj, bmd, mx, my);
}
METHOD_END
///
/// BASE_LOCALIZE

METHOD(SepClassLocalize, struct bmLocalize *, bml)
{
   SD       *sd = INST_DATA(cl, obj);
   ULONG     rc = 0;

   if (sd->sd_Title) rc = AsmDoMethodA(sd->sd_Title, (Msg)bml);

   return rc;
}
METHOD_END
///
/// Class initialization.
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,          (FUNCPTR)SepClassRender,
   GM_HITTEST,           (FUNCPTR)SepClassHitTest,
   BASE_DIMENSIONS,      (FUNCPTR)SepClassDimensions,
   OM_NEW,               (FUNCPTR)SepClassNew,
   OM_DISPOSE,           (FUNCPTR)SepClassDispose,
   OM_SET,               (FUNCPTR)SepClassSet,
   BASE_LOCALIZE,        (FUNCPTR)SepClassLocalize,
   DF_END,               NULL
};

/*
 * Initialize the separator class.
 */
makeproto Class *InitSepClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
			 CLASS_ObjectSize,     sizeof(SD),
			 CLASS_DFTable,        ClassFunc,
			 TAG_DONE);
}
///
