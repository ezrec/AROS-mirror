/*
 * @(#) $Header$
 *
 * BGUI library
 * labelclass.c
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
 * Revision 42.7  2003/01/18 19:09:57  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.6  2000/08/17 15:09:18  chodorowski
 * Fixed compiler warnings.
 *
 * Revision 42.5  2000/07/03 21:21:00  bergers
 * Replaced stch_l & stcu_d and had to make a few changes in other places because of that.
 *
 * Revision 42.4  2000/07/02 06:08:33  bergers
 * Compiles library alright (except that I took stch_l & stcu_d out) and seems to create the right type of object. Test1 also compiles alright but crashes somewhere...
 *
 * Revision 42.3  2000/05/29 15:42:49  stegerg
 * fixed some "comparison is always 1 due to limited range of data type"
 * errors
 *
 * Revision 42.2  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.1  2000/05/15 19:27:01  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:09:16  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:29  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.7  1999/08/03 05:11:53  mlemos
 * Ensured that the creation of the label rotation bitmaps are allocated as
 * friends of the rastport's bitmap.
 *
 * Revision 41.10.2.6  1998/12/07 03:07:01  mlemos
 * Replaced OpenFont and CloseFont calls by the respective BGUI debug macros.
 *
 * Revision 41.10.2.5  1998/10/16 02:58:47  mlemos
 * Fixed a bug in the computation of label dimensions that was ignoring that
 * labels may have padding space to left or above them.
 *
 * Revision 41.10.2.4  1998/10/15 04:59:44  mlemos
 * Fixed a bug in the computation of label dimensions that was ignoring that
 * labels have padding space arround them depending on their positions.
 * Fixed a bug that was making label class maximum size to not update
 * depending on nominal size.
 *
 * Revision 41.10.2.3  1998/05/22 03:50:04  mlemos
 * Ensured that the extent fields are initialized even when the label text is
 * set to NULL.
 *
 * Revision 41.10.2.2  1998/03/02 23:48:46  mlemos
 * Switched vector allocation functions calls to BGUI allocation functions.
 *
 * Revision 41.10.2.1  1998/02/28 02:24:43  mlemos
 * Made Dimensions method return size (0,0) when the label text is set to NULL
 *
 * Revision 41.10  1998/02/25 21:12:21  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:42  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.
#include "include/classdefs.h"

/*
 * Object instance data.
 */
typedef struct ld_ {
   ULONG            ld_Flags;              /* Flags.                           */
   UBYTE           *ld_Text;               /* The label itself.                */
   ULONG            ld_TextID;             /* Text ID.                         */
   struct IBox      ld_Extent;             /* Label extent.                    */
   UWORD            ld_Width, ld_Height;   /* Nominal label size.              */
   struct TextAttr *ld_TextAttr;           /* Font to use.                     */
   struct TextFont *ld_Font;               /* The opened font.                 */
   UWORD            ld_Style, ld_SelStyle; /* Style in which to render.        */
   UBYTE            ld_Pen, ld_SelPen;     /* Pens to render normal/selected.  */
   UBYTE            ld_Place;              /* Where to render.                 */
   UBYTE            ld_UnderscoreChar;     /* Underscore indicator.            */
   UBYTE            ld_ErasePen;           /* Pen to erase with.               */
}  LD;

#define LABF_SELFOPEN      (1<<31)
#define LABF_EXT_VALID     (1<<30)
#define LABF_DRIPEN        (1<<29)
#define LABF_SELDRIPEN     (1<<28)
#define LABF_DEFPEN        (1<<27)
#define LABF_DEFSELPEN     (1<<26)
#define LABF_NOPLACEIN     (1<<25)

#define LD_ENTRY(tag, offset, flags) PACK_ENTRY(LAB_TAGSTART, tag, ld_, offset, flags)
#define LD_FLAG(tag, flag) PACK_LONGBIT(LAB_TAGSTART, tag, ld_, ld_Flags, PKCTRL_BIT, flag)

#define LD_PENTRY(tag, offset, flags) PACK_ENTRY(BGUI_PB, tag, ld_, offset, flags)
#define LD_PFLAG(tag, flag) PACK_LONGBIT(BGUI_PB, tag, ld_, ld_Flags, PKCTRL_BIT, flag)

static ULONG LabelPackTable[] =
{
   PACK_STARTTABLE(LAB_TAGSTART),

   LD_ENTRY(LAB_Label,            ld_Text,              PKCTRL_ULONG),
   LD_ENTRY(LAB_LabelID,          ld_TextID,            PKCTRL_ULONG),
   LD_ENTRY(LAB_TextAttr,         ld_TextAttr,          PKCTRL_ULONG),
   LD_ENTRY(LAB_Style,            ld_Style,             PKCTRL_UWORD),
   LD_ENTRY(LAB_SelectedStyle,    ld_SelStyle,          PKCTRL_UWORD),
   LD_ENTRY(LAB_Place,            ld_Place,             PKCTRL_UBYTE),
   LD_ENTRY(LAB_Underscore,       ld_UnderscoreChar,    PKCTRL_UBYTE),
   LD_FLAG(LAB_Highlight,         LABF_HIGHLIGHT),
   LD_FLAG(LAB_HighUScore,        LABF_HIGH_USCORE),
   LD_FLAG(LAB_FlipX,             LABF_FLIP_X),
   LD_FLAG(LAB_FlipY,             LABF_FLIP_Y),
   LD_FLAG(LAB_FlipXY,            LABF_FLIP_XY),
   LD_FLAG(LAB_NoPlaceIn,         LABF_NOPLACEIN),

   PACK_NEWOFFSET(BGUI_PB),

   LD_PENTRY(IMAGE_ErasePen,      ld_ErasePen,          PKCTRL_UBYTE),

   PACK_ENDTABLE
};
///

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(LabelClassNew, struct opSet *, ops)
{
   LD       *ld;
   ULONG     rc;

   /*
    * First we let the superclass
    * create an object.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      /*
       * Get to the instance data.
       */
      ld = INST_DATA(cl, rc);

      /*
       * Clear instance data and
       * setup the defaults.
       */
      ld->ld_Flags          = LABF_DEFPEN|LABF_DEFSELPEN;
      ld->ld_Style          = (UWORD)~0;
      ld->ld_SelStyle       = (UWORD)~0;
      ld->ld_Place          = PLACE_IN;
      ld->ld_UnderscoreChar = '_';

      /*
       * Set attributes.
       */
      AsmCoerceMethod(cl, (Object *)rc, RM_SETM, ops->ops_AttrList, RAF_INITIAL);

#if __AROS__
#warning A comment within a comment makes gcc puke...
#if 0

      /*
      if (fail)
      {
	 /*
	  * Failure!
	  */
	 AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
	 rc = 0;
      }
      */
#endif
#endif
      // ld->ld_Flags |= LABF_FLIP_XY;

   }
   return rc;
}
METHOD_END
///
/// RM_GETATTRFLAGS
/*
 * Get the flags of an attribute.
 */
METHOD(LabelClassGetAttrFlags, struct rmAttr *, ra)
{
   static struct TagItem chart[] =
   {
      LAB_Label,            CHART_ATTR(ld_, ld_Text           ) | RAF_RESIZE,
      LAB_LabelID,          CHART_ATTR(ld_, ld_TextID         ),
      LAB_TextAttr,         CHART_ATTR(ld_, ld_TextAttr       ) | RAF_CUSTOM | RAF_RESIZE,
      LAB_Style,            CHART_ATTR(ld_, ld_Style          ) | RAF_REDRAW,
      LAB_SelectedStyle,    CHART_ATTR(ld_, ld_SelStyle       ) | RAF_REDRAW,
      LAB_Place,            CHART_ATTR(ld_, ld_Place          ) | RAF_REDRAW,
      LAB_Underscore,       CHART_ATTR(ld_, ld_UnderscoreChar ) | RAF_CUSTOM,
      LAB_Template,         CHART_ATTR(ld_, ld_Flags          ) | RAF_CUSTOM | RAF_ADDRESS,
      IMAGE_ErasePen,       CHART_ATTR(ld_, ld_ErasePen       ),
      IMAGE_TextFont,       CHART_ATTR(ld_, ld_Font           ) | RAF_CUSTOM | RAF_NOP,
      LAB_Pen,              CHART_ATTR(ld_, ld_Pen            ) | RAF_CUSTOM | RAF_NOP,
      LAB_DriPen,           CHART_ATTR(ld_, ld_Pen            ) | RAF_CUSTOM | RAF_NOP,
      LAB_SelectedPen,      CHART_ATTR(ld_, ld_SelPen         ) | RAF_CUSTOM | RAF_NOP,
      LAB_SelectedDriPen,   CHART_ATTR(ld_, ld_SelPen         ) | RAF_CUSTOM | RAF_NOP,

      LAB_Highlight,        CHART_FLAG(ld_, ld_Flags, LABF_HIGHLIGHT  ) | RAF_REDRAW,
      LAB_HighUScore,       CHART_FLAG(ld_, ld_Flags, LABF_HIGH_USCORE) | RAF_REDRAW,
      LAB_FlipX,            CHART_FLAG(ld_, ld_Flags, LABF_FLIP_X     ) | RAF_REDRAW,
      LAB_FlipY,            CHART_FLAG(ld_, ld_Flags, LABF_FLIP_Y     ) | RAF_REDRAW,
      LAB_FlipXY,           CHART_FLAG(ld_, ld_Flags, LABF_FLIP_XY    ) | RAF_RESIZE,
      LAB_NoPlaceIn,        CHART_FLAG(ld_, ld_Flags, LABF_NOPLACEIN  ) | RAF_CUSTOM,

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
METHOD(LabelClassSet, struct rmAttr *, ra)
{
   return BGUI_SetAttrChart(cl, obj, ra);
}
METHOD_END
///
/// RM_SETCUSTOM
/*
 * Set custom attributes.
 */
METHOD(LabelClassSetCustom, struct rmAttr *, ra)
{
   LD               *ld = (LD *)INST_DATA(cl, obj);
   struct TextFont  *tf;
   ULONG             attr = ra->ra_Attr->ti_Tag;
   ULONG             data = ra->ra_Attr->ti_Data;

   switch (attr)
   {
   case LAB_Template:
      if (data)
      {
	 attr = NULL;
	 Get_Attr((Object *)data, LAB_Template, &attr);
	 if (attr)
	 {
	    CopyMem((LD *)attr, ld, sizeof(LD));
	    ld->ld_Flags &= ~LABF_SELFOPEN;
	 };
      };
      break;

   case IMAGE_TextFont:
      if (ld->ld_Font && (ld->ld_Flags & LABF_SELFOPEN))
	 BGUI_CloseFont(ld->ld_Font);
      ld->ld_Font   = (struct TextFont *)data;
      ld->ld_Flags &= ~LABF_SELFOPEN;
      break;

   case LAB_Place:
   case LAB_NoPlaceIn:
      if ((ld->ld_Flags & LABF_NOPLACEIN) && (ld->ld_Place == PLACE_IN))
	 ld->ld_Place = PLACE_LEFT;
      break;

   case LAB_Flags:
      ld->ld_Flags = (ld->ld_Flags & 0xFFFF0000) | data;
      break;

   case LAB_TextAttr:
      if (data && (tf = BGUI_OpenFont((struct TextAttr *)data)))
      {
	 if (ld->ld_Font && (ld->ld_Flags & LABF_SELFOPEN))
	    BGUI_CloseFont(ld->ld_Font);
	 ld->ld_Font     = tf;
	 ld->ld_TextAttr = (struct TextAttr *)data;
	 ld->ld_Flags   |= LABF_SELFOPEN;
      };
      break;

   case LAB_Pen:
   case LAB_DriPen:
      if ((UWORD)data == (UWORD)~0)
      {
	 ld->ld_Flags |= LABF_DEFPEN;
      }
      else
      {
	 ld->ld_Flags &= ~LABF_DEFPEN;
	 ld->ld_Pen    = data;
      };
      if (attr == LAB_Pen) ld->ld_Flags &= ~LABF_DRIPEN;
      else                 ld->ld_Flags |=  LABF_DRIPEN;
      break;

   case LAB_SelectedPen:
   case LAB_SelectedDriPen:
      if ((UWORD)data == (UWORD)~0)
      {
	 ld->ld_Flags |= LABF_DEFSELPEN;
      }
      else
      {
	 ld->ld_Flags &= ~LABF_DEFSELPEN;
	 ld->ld_SelPen = data;
      };
      if (attr == LAB_SelectedPen) ld->ld_Flags &= ~LABF_SELDRIPEN;
      else                         ld->ld_Flags |=  LABF_SELDRIPEN;
      break;
   };
   return 1;
}
METHOD_END
///

/// OM_GET
/*
 * Give an attribute.
 */
METHOD(LabelClassGet, struct opGet *, opg)
{
   LD       *ld = INST_DATA(cl, obj);
   ULONG     rc = 1, tag = opg->opg_AttrID, *store = opg->opg_Storage;
   UBYTE    *u;
   
   /*
    * First we try if the attribute is known to us. If not we pass
    * it on to the superclass.
    */
   switch (tag)
   {
      case LAB_Template:
	 STORE ld;
	 break;
      case LAB_Pen:
	 STORE (!(ld->ld_Flags & LABF_DEFPEN) && !(ld->ld_Flags & LABF_DRIPEN)) ? ld->ld_Pen : (ULONG)~0;
	 break;
      case LAB_DriPen:
	 STORE (!(ld->ld_Flags & LABF_DEFPEN) &&  (ld->ld_Flags & LABF_DRIPEN)) ? ld->ld_Pen : (ULONG)~0;
	 break;
      case LAB_SelectedPen:
	 STORE (!(ld->ld_Flags & LABF_DEFSELPEN) && !(ld->ld_Flags & LABF_SELDRIPEN)) ? ld->ld_SelPen : (ULONG)~0;
	 break;
      case LAB_SelectedDriPen:
	 STORE (!(ld->ld_Flags & LABF_DEFSELPEN) &&  (ld->ld_Flags & LABF_SELDRIPEN)) ? ld->ld_SelPen : (ULONG)~0;
	 break;
      case LAB_KeyChar:
	 if (ld->ld_Text && ld->ld_UnderscoreChar)
	 {
	    if (u = strchr(ld->ld_Text, ld->ld_UnderscoreChar))
	       STORE (u + 1);
	 };
	 break;
      case LAB_Flags:
	 STORE ld->ld_Flags;
	 break;
      default:
	 rc = BGUI_UnpackStructureTag((UBYTE *)ld, LabelPackTable, tag, store);
	 if (!rc) rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
	 break;
   };
   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Dispose of the object.
 */
METHOD(LabelClassDispose, Msg, msg)
{
   LD       *ld = INST_DATA(cl, obj);

   /*
    * Close the font.
    */
   if (ld->ld_Font && (ld->ld_Flags & LABF_SELFOPEN))
      BGUI_CloseFont(ld->ld_Font);

   /*
    * The rest goes to the superclass.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// IM_DRAW, IM_ERASE, IM_EXTENT
/*
 * Render, erase or find out it's extensions.
 */
METHOD(LabelClassDrawErase, struct impDraw *, dr)
{
   LD                *ld = INST_DATA(cl, obj);
   struct RastPort    rrp = *(dr->imp_RPort), trp;
   struct impExtent  *ie = (struct impExtent *)dr;
   struct IBox       *bounds, box;
   UBYTE             *chunky1, *chunky2;

   UBYTE       *text = NULL, *s, *d, c;
   UWORD       *pens = NULL, pen, style;
   WORD         xpos, ypos, tw, th;
   ULONG        rc = 0;
   int          i1, j1, j2, rw, rh;

   BOOL         sel = (dr->imp_State == IDS_SELECTED) || (dr->imp_State == IDS_INACTIVESELECTED);

   /*
    * Obtain image box bounds.
    */
   WORD l = IMAGE(obj)->LeftEdge;
   WORD t = IMAGE(obj)->TopEdge;
   WORD w = IMAGE(obj)->Width;
   WORD h = IMAGE(obj)->Height;

   /*
    * Get pens.
    */
   if (dr->MethodID == IM_DRAW) pens = PENS(dr->imp_DrInfo);

   /*
    * Add offsets when necessary.
    */
   if (dr->MethodID != IM_EXTENT)
   {
      l += dr->imp_Offset.X;
      t += dr->imp_Offset.Y;
   }
   else
   {
      bounds = ie->impe_Extent;
      bounds->Left = bounds->Top = bounds->Width = bounds->Height = 0;
   }

   /*
    * Only do the label when it actually exists.
    */
   if (s = ld->ld_Text)
   {
      text = BGUI_AllocPoolMem(strlen(s) + 30);
   };
   if (d = text)
   {
      rc = 1;

      *d++ = '\33';                          /* 2 */
      switch (ld->ld_Place)
      {
      case PLACE_LEFT:
	 *d++ = 'r';
	 break;
      case PLACE_RIGHT:
	 *d++ = 'l';
	 break;
      default:
	 *d++ = 'c';
	 break;
      };

      /*
       * Set font style.
       */
      style = sel ? ld->ld_SelStyle : ld->ld_Style;
      if (sel && (style == (UWORD)~0)) style = ld->ld_Style;

      if (style != (UWORD)~0)
      {
	 if (style & FSF_ITALIC)      /* 2 */
	 {
	    *d++ = '\33';
	    *d++ = 'i';
	 };
	 if (style & FSF_BOLD)        /* 2 */
	 {
	    *d++ = '\33';
	    *d++ = 'b';
	 };
	 if (style & FSF_UNDERLINED)  /* 2 */
	 {
	    *d++ = '\33';
	    *d++ = 'u';
	 };
      };

      c = 0;
      if (dr->MethodID == IM_DRAW)           /* 5 */
      {
	 if (ld->ld_Flags & LABF_HIGHLIGHT)
	 {
	    c   = 'd';
	    pen = HIGHLIGHTTEXTPEN;
	 }
	 else
	 {
	    /*
	     * What state are we in?
	     */
	    if (sel)
	    {
	       /*
		* First see if a selected (dri)pen is
		* given. If not use the default colors.
		*/
	       if (ld->ld_Flags & LABF_DEFSELPEN)
	       {
		  c   = 'd';
		  pen = (dr->imp_State == IDS_SELECTED) ? FILLTEXTPEN : TEXTPEN;
	       }
	       else
	       {
		  c = (ld->ld_Flags & LABF_SELDRIPEN) ? 'd' : 'p';
		  pen = ld->ld_SelPen;
	       };
	    }
	    else
	    {
	       /*
		* First see if a normal (dri)pen is
		* given. If not use the default colors.
		*/
	       if (ld->ld_Flags & LABF_DEFPEN)
	       {
		  c   = 'd';
		  pen = TEXTPEN;
	       }
	       else
	       {
		  c   = (ld->ld_Flags & LABF_DRIPEN) ? 'd' : 'p';
		  pen = ld->ld_Pen;
	       };
	    };
	 };
      }
      else if (dr->MethodID == IM_ERASE)
      {
	 c   = 'p';
	 pen = ld->ld_ErasePen;
      };
      if (c)                                       /* 6 */
      {
	 *d++ = '\33';
	 *d++ = c;
#ifdef __AROS__
	d += sprintf(d, "%u", pen);
#else         
	 d += stcu_d(d, pen);
#endif
      };

      *d++ = '\33';                                /* 2 */
      *d++ = '\33';

      while (c = *s++)
      {
	 if (c == ld->ld_UnderscoreChar)           /* 5 */
	 {
	    if (*s)
	    {
	       if ((*s < 32) || (*s == 127))
	       {
		  s++;
	       }
	       else
	       {
		  c = (ld->ld_Flags & LABF_HIGH_USCORE) ? 'Z' : 'z';
		  *d++ = '\33';
		  *d++ = c;
		  *d++ = *s++;
		  if (*s)
		  {
		     *d++ = '\33';
		     *d++ = '-';
		     *d++ = c;
		  };
	       };
	       strcpy(d, s);
	    };
	    break;
	 };
	 *d++ = c;
      };

      /*
       * If a font is available, set it in the rastport.
       */
      if (ld->ld_Font) FSetFont(&rrp, ld->ld_Font);

      /*
       * Setup drawmode.
       */
      FSetDrMd(&rrp, JAM1);

      /*
       * Get label pixel size.
       */
      th = TotalHeight(&rrp, text);
      tw = TotalWidth (&rrp, text);

      if (ld->ld_Flags & LABF_FLIP_XY)
      {
	 i1 = th; th = tw; tw = i1;
      };

      ld->ld_Width  = tw;
      ld->ld_Height = th;

      /*
       * Preset x & y position.
       */
      xpos = l;
      ypos = t;

      /*
       * No need to place the text when we
       * want the maximum extentiions.
       */
      if (dr->MethodID == IM_EXTENT && (ie->impe_Flags & EXTF_MAXIMUM))
	 goto maximumExtent;

      reCalculate:

      /*
       * Determine Y position.
       */
      switch (ld->ld_Place)
      {
	 case PLACE_LEFT:
	 case PLACE_IN:
	 case PLACE_RIGHT:
	    ypos += (h - th) >> 1;
	    break;

	 case PLACE_ABOVE:
	    ypos -= (th + 4);
	    break;

	 case PLACE_BELOW:
	    ypos += (h + 3);
	    break;
      }

      /*
       * Determine X position.
       */
      switch (ld->ld_Place)
      {
	 case PLACE_ABOVE:
	 case PLACE_IN:
	 case PLACE_BELOW:
	    xpos += (w - tw) >> 1;
	    break;

	 case PLACE_LEFT:
	    xpos -= (tw + 8);
	    break;

	 case PLACE_RIGHT:
	    xpos += (w + 7);
	    break;
      }

      /*
       * IM_EXTENT method?
       */
      if (dr->MethodID == IM_EXTENT)
      {
	 maximumExtent:
	 /*
	  * Pick up extent storage, preset to 0.
	  */
	 bounds = ie->impe_Extent;
	 bounds->Left = bounds->Top = bounds->Width = bounds->Height = 0;

	 /*
	  * No negative offsets.
	  */
	 if ((l <= 0) || (t <= 0))
	 {
	    if (l <= 0) l = tw + 8;
	    if (t <= 0) t = th + 4;
	    goto reCalculate;
	 }

	 /*
	  * Maximum?
	  */
	 if (ie->impe_Flags & EXTF_MAXIMUM)
	 {
	    switch (ld->ld_Place)
	    {
	    case PLACE_LEFT:
	       bounds->Left   = -(tw + 8);
	       break;
	    case PLACE_RIGHT:
	       bounds->Width  =  (tw + 7);
	       break;
	    case PLACE_ABOVE:
	       bounds->Top    = -(th + 4);
	       break;
	    case PLACE_BELOW:
	       bounds->Height =  (th + 3);
	       break;
	    }
	 }
	 else
	 {
	    if (xpos < l)                    bounds->Left   = -(l - xpos);
	    if ((xpos + tw) > (l + w - 1))   bounds->Width  = (xpos + tw) - (l + w - 1);
	    if (ypos < t)                    bounds->Top    = -(t - ypos);
	    if ((ypos + th) > (t + h - 1))   bounds->Height = (ypos + th) - (t + h - 1);
	 }

	 /*
	  * Store label width and height.
	  */
	 if (ie->impe_LabelSize.Width)  *(ie->impe_LabelSize.Width)  = tw;
	 if (ie->impe_LabelSize.Height) *(ie->impe_LabelSize.Height) = th;
      }
      else
      {
	 if (ld->ld_Flags & (LABF_FLIP_XY|LABF_FLIP_X|LABF_FLIP_Y))
	 {
	    rrp.BitMap = BGUI_AllocBitMap(tw, th, FGetDepth(&rrp), BMF_STANDARD, dr->imp_RPort->BitMap);
	    
	    if (rrp.BitMap)
	    {
	       rrp.Layer = NULL;
	       ClipBlit(dr->imp_RPort, xpos, ypos, &rrp, 0, 0, tw, th, 0xC0);
	    };
	    box.Left = box.Top = 0;
	 }
	 else
	 {
	    box.Left = xpos;
	    box.Top  = ypos;
	 };

	 /*
	  * Move to the correct location.
	  */
	 box.Width   = tw;
	 box.Height  = th;

	 if (rrp.BitMap) RenderInfoText(&rrp, text, pens, &box, (UWORD)~0);
	 
	 if (ld->ld_Flags & (LABF_FLIP_XY|LABF_FLIP_X|LABF_FLIP_Y))
	 {
	    if (rrp.BitMap)
	    {
	       rw = ((tw + 15) >> 4) << 4;
	       
	       if (chunky1 = BGUI_AllocPoolMem(rw * th))
	       {
		  trp = rrp;
		  if (trp.BitMap = BGUI_AllocBitMap(rw, 1, FGetDepth(&rrp), BMF_STANDARD, dr->imp_RPort->BitMap))
		  {
		     ReadPixelArray8(&rrp, 0, 0, tw - 1, th - 1, chunky1, &trp);
		     BGUI_FreeBitMap(rrp.BitMap);
		     rrp.BitMap = NULL;
		     
		     if (ld->ld_Flags & LABF_FLIP_X)
		     {
			s = chunky1;
			
			for (i1 = th; i1; i1--)
			{                          
			   for (j1 = 0, j2 = tw - 1; j1 < j2; j1++, j2--)
			   {
			      c = s[j1];
			      s[j1] = s[j2];
			      s[j2] = c;
			   };
			   s += rw;
			};
		     };

		     if (ld->ld_Flags & LABF_FLIP_Y)
		     {
			s = chunky1;
			d = chunky1 + rw * (th - 1);
			
			while (s < d)
			{                          
			   for (j1 = tw - 1; j1; j1--)
			   {
			      c = s[j1];
			      s[j1] = d[j1];
			      d[j1] = c;
			   };
			   s += rw;
			   d -= rw;
			};
		     };

		     if (ld->ld_Flags & LABF_FLIP_XY)
		     {
			chunky2 = chunky1;
			rh = ((th + 15) >> 4) << 4;

			if (chunky1 = BGUI_AllocPoolMem(tw * rh))
			{
			   for (i1 = 0; i1 < th; i1++)
			   {
			      for (j1 = 0; j1 < tw; j1++)
			      {
				 chunky1[(j1 * rh) + i1] = chunky2[(i1 * rw) + j1];
			      };
			   };
			   BGUI_FreePoolMem(chunky2);
			}
			else chunky1 = chunky2;
			i1 = tw; tw = th; th = i1;

			BGUI_FreeBitMap(trp.BitMap);
			trp.BitMap = BGUI_AllocBitMap(rw, 1, FGetDepth(&rrp), BMF_STANDARD, dr->imp_RPort->BitMap);
		     };
		     if (trp.BitMap)
		     {                    
			WritePixelArray8(dr->imp_RPort, xpos, ypos,
			   xpos + tw - 1, ypos + th - 1, chunky1, &trp);
			BGUI_FreeBitMap(trp.BitMap);
		     };
		  };                   
		  BGUI_FreePoolMem(chunky1);
	       };          
	       BGUI_FreeBitMap(rrp.BitMap);
	    };
	 };
      };
      BGUI_FreePoolMem(text);
   }
   return rc;
}
METHOD_END
///
/// BASE_LOCALIZE

METHOD(LabelClassLocalize, struct bmLocalize *, bml)
{
   LD    *ld = INST_DATA(cl, obj);
   ULONG  rc = 0;
   
   if (ld->ld_TextID)
   {
      ld->ld_Text = BGUI_GetCatalogStr(bml->bml_Locale, ld->ld_TextID, ld->ld_Text);
      rc = 1;
   }
   return rc;
}
METHOD_END
///
/// BASE_DIMENSIONS

METHOD(LabelClassDimensions, struct bmDimensions *, bmd)
{
   LD                *ld = INST_DATA(cl, obj);
   struct BaseInfo   *bi = bmd->bmd_BInfo;
   struct bguiExtent *be = bmd->bmd_Extent;
   UWORD                lh;
   UWORD                lw;

   {
      struct IBox extentions;

      if(DoExtentMethod(obj, bi->bi_RPort, &extentions, &lw, &lh, EXTF_MAXIMUM))
      {
	 lw=max(lw,max(-extentions.Left,extentions.Width));
	 lh=max(lh,max(-extentions.Top,extentions.Height));
      }
      else
	 lw=lh=0;
   }
   switch (ld->ld_Place)
   {
   case PLACE_ABOVE:
   case PLACE_BELOW:
      if (be->be_Min.Width < lw) be->be_Min.Width = lw;
      if (be->be_Nom.Width < lw) be->be_Nom.Width = lw;
      break;
   case PLACE_IN:
      lw += 4;
   default:
      be->be_Min.Width += lw;
      be->be_Nom.Width += lw;
      break;
   };

   switch (ld->ld_Place)
   {
   case PLACE_LEFT:
   case PLACE_RIGHT:
      if (be->be_Min.Height < lh) be->be_Min.Height = lh;
      if (be->be_Nom.Height < lh) be->be_Nom.Height = lh;
      break;
   case PLACE_IN:
      lh += 2;
   default:
      be->be_Min.Height += lh;
      be->be_Nom.Height += lh;
      break;
   };
   be->be_Max.Width=max(be->be_Max.Width,be->be_Nom.Width);
   be->be_Max.Height=max(be->be_Max.Height,be->be_Nom.Height);
   return 1;
}
METHOD_END
///

/// Class initialization.

/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   RM_SET,                (FUNCPTR)LabelClassSet,
   RM_SETCUSTOM,          (FUNCPTR)LabelClassSetCustom,
   RM_GETATTRFLAGS,       (FUNCPTR)LabelClassGetAttrFlags,
   IM_DRAW,               (FUNCPTR)LabelClassDrawErase,

   OM_GET,                (FUNCPTR)LabelClassGet,
   OM_NEW,                (FUNCPTR)LabelClassNew,
   OM_DISPOSE,            (FUNCPTR)LabelClassDispose,
   IM_EXTENT,             (FUNCPTR)LabelClassDrawErase,
   IM_ERASE,              (FUNCPTR)LabelClassDrawErase,
   BASE_LOCALIZE,         (FUNCPTR)LabelClassLocalize,
   BASE_DIMENSIONS,       (FUNCPTR)LabelClassDimensions,
   DF_END,                NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitLabelClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_IMAGE_OBJECT,
			 CLASS_ObjectSize,     sizeof(LD),
			 CLASS_DFTable,        ClassFunc,
			 TAG_DONE);
}
///

