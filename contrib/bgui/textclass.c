/*
 * @(#) $Header$
 *
 * BGUI library
 * textclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.7  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.6  2003/01/18 19:10:02  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.5  2001/02/02 21:06:50  stegerg
 * used TextFit() with strDirection param = 0, but it must be -1 or 1
 * (autodocs)
 *
 * Revision 42.4  2001/01/28 04:53:21  bergers
 * Fixed some compiler complaints (some casts were missing.).
 *
 * Revision 42.3  2000/08/09 11:45:57  chodorowski
 * Removed a lot of #ifdefs that disabled the AROS_LIB* macros when not building on AROS. This is now handled in contrib/bgui/include/bgui_compilerspecific.h.
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
 * Revision 42.0  2000/05/09 22:10:31  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:21  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.7  1999/08/10 22:38:03  mlemos
 * Removed comments of escape sequences not implemented.
 *
 * Revision 41.10.2.6  1999/07/23 19:46:18  mlemos
 * Added support to set draw mode to JAM1, JAM2 or complement in the text
 * command sequences.
 *
 * Revision 41.10.2.5  1998/12/06 22:30:11  mlemos
 * Fixed bug of using the wrong RastPort to compute XPos of text string.
 *
 * Revision 41.10.2.4  1998/05/22 03:51:25  mlemos
 * Added new line characters to the assertion debug messages that warn about
 * NULL text strings being passed to TotalWidth and TotalHeight.
 *
 * Revision 41.10.2.3  1998/03/01 18:46:12  mlemos
 * Fixed short allocation for label text string.
 *
 * Revision 41.10.2.2  1998/03/01 15:39:39  mlemos
 * Added support to track BaseInfo memory leaks.
 *
 * Revision 41.10.2.1  1998/02/28 02:42:26  mlemos
 * Added debug assert code to trap attempts to pass NULL text pointers to TextHeight and TextWidth functions.
 *
 * Revision 41.10  1998/02/25 21:13:23  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:58  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.
#include "include/classdefs.h"

/*
 * Object instance data.
 */
typedef struct td_ {
   ULONG            td_Flags;              /* Flags.                           */
   UBYTE           *td_Text;               /* The text itself.                 */
   ULONG            td_TextID;             /* Text ID.                         */
   ULONG           *td_Args;               /* Arguments for format string.     */
}  TD;

#define TEXTF_COPY         (1<<0)          /* Copy text.                       */
#define TEXTF_COPIED       (1<<1)          /* This text is a copy.             */

///

/// Text_Get

STATIC UBYTE *Text_Get(TD *td)
{
   UBYTE  *text;

   if (td->td_Text && td->td_Args)
   {
      if (text = BGUI_AllocPoolMem(CompStrlenF(td->td_Text, td->td_Args)))
      {
	 DoSPrintF(text, td->td_Text, td->td_Args);
	 return text;
      };
      return NULL;
   };
   return td->td_Text;
}
///
/// Text_Clear

STATIC VOID Text_Clear(TD *td)
{
   if (td->td_Flags & TEXTF_COPIED)
   {
      BGUI_FreePoolMem(td->td_Text);
      td->td_Flags &= ~TEXTF_COPIED;
   };
   td->td_Text = NULL;
}
///
/// Text_Set

static void Text_Set(TD *td, char *text)
{
   Text_Clear(td);

   if (text && (td->td_Flags & TEXTF_COPY))
   {
      if (td->td_Text = BGUI_AllocPoolMem(strlen(text)+1))
      {
	 strcpy(td->td_Text, text);
	 td->td_Flags |= TEXTF_COPIED;
      };
   }
   else
   {
      td->td_Text = text;
   };
}
///
/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(TextClassNew, struct opSet *, ops)
{
   ULONG     rc;

   /*
    * First we let the superclass
    * create an object.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      /*
       * Set attributes.
       */
      AsmCoerceMethod(cl, (Object *)rc, RM_SETM, ops->ops_AttrList, RAF_INITIAL);
   }
   return rc;
}
METHOD_END
///
/// RM_GETATTRFLAGS
/*
 * Get the flags of an attribute.
 */
METHOD(TextClassGetAttrFlags, struct rmAttr *, ra)
{
   static struct TagItem chart[] =
   {
      TEXTA_Text,           CHART_ATTR(td_, td_Text               ) | RAF_CUSTOM | RAF_NOP | RAF_RESIZE,
      TEXTA_TextID,         CHART_ATTR(td_, td_TextID             ),
      TEXTA_Args,           CHART_ATTR(td_, td_Args               ) | RAF_RESIZE,

      TEXTA_CopyText,       CHART_FLAG(td_, td_Flags,  TEXTF_COPY ) | RAF_CUSTOM,

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
METHOD(TextClassSet, struct rmAttr *, ra)
{
   return BGUI_SetAttrChart(cl, obj, ra);
}
METHOD_END
///
/// RM_SETCUSTOM
/*
 * Set custom attributes.
 */
METHOD(TextClassSetCustom, struct rmAttr *, ra)
{
   TD               *td = INST_DATA(cl, obj);
   ULONG             attr = ra->ra_Attr->ti_Tag;
   ULONG             data = ra->ra_Attr->ti_Data;

   switch (attr)
   {
   case TEXTA_Text:
      Text_Set(td, (char *)data);
      break;

   case TEXTA_CopyText:
      if (data && !(td->td_Flags & TEXTF_COPIED))
      {
	 Text_Set(td, td->td_Text);
      };
      break;
   };
   return 1;
}
METHOD_END
///
/// RM_GET
/*
 * Get an attribute.
 */
METHOD(TextClassGet, struct rmAttr *, ra)
{
   return BGUI_GetAttrChart(cl, obj, ra);
}
METHOD_END
///
/// RM_GETCUSTOM
/*
 * Get custom attributes.
 */
METHOD(TextClassGetCustom, struct rmAttr *, ra)
{
   TD               *td = INST_DATA(cl, obj);
   ULONG             attr = ra->ra_Attr->ti_Tag;
   ULONG            *store = (ULONG *)ra->ra_Attr->ti_Data;

   switch (attr)
   {
   case TEXTA_Text:
      STORE td->td_Text;
      break;
   };
   return 1;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Dispose of the object.
 */
METHOD(TextClassDispose, Msg, msg)
{
   TD       *td = INST_DATA(cl, obj);

   /*
    * Clear the text.
    */
   Text_Clear(td);

   /*
    * The rest goes to the superclass.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// TEXTM_RENDER
/*
 * Render a text graphic.
 */
METHOD(TextClassRender, struct tmRender *, tmr)
{
   TD                *td = INST_DATA(cl, obj);
   UBYTE             *text;

   if (text = Text_Get(td))
   {
      RenderText(tmr->tmr_BInfo, text, tmr->tmr_Bounds);
      if (td->td_Args) BGUI_FreePoolMem(text);
   };
   return 1;
}
METHOD_END
///
/// TEXTM_DIMENSIONS
/*
 * Render a text graphic.
 */
METHOD(TextClassDimensions, struct tmDimensions *, tmd)
{
   TD                *td = INST_DATA(cl, obj);
   UBYTE             *text;

   if (text = Text_Get(td))
   {
      if (tmd->tmd_Extent.Width)  *(tmd->tmd_Extent.Width)  = TotalWidth (tmd->tmd_RPort, text);
      if (tmd->tmd_Extent.Height) *(tmd->tmd_Extent.Height) = TotalHeight(tmd->tmd_RPort, text);

      if (td->td_Args) BGUI_FreePoolMem(text);
   };
   return 1;
}
METHOD_END
///
/// BASE_LOCALIZE

METHOD(TextClassLocalize, struct bmLocalize *, bml)
{
   TD        *td = INST_DATA(cl, obj);
   ULONG      rc = 0;
   
   if (td->td_TextID)
   {
      Text_Set(td, BGUI_GetCatalogStr(bml->bml_Locale, td->td_TextID, td->td_Text));
      rc = 1;
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
   TEXTM_RENDER,          (APTR)TextClassRender,
   TEXTM_DIMENSIONS,      (FUNCPTR)TextClassDimensions,

   RM_GETATTRFLAGS,       (FUNCPTR)TextClassGetAttrFlags,
   RM_SET,                (FUNCPTR)TextClassSet,
   RM_SETCUSTOM,          (FUNCPTR)TextClassSetCustom,
   RM_GET,                (FUNCPTR)TextClassGet,
   RM_GETCUSTOM,          (FUNCPTR)TextClassGetCustom,
   OM_NEW,                (FUNCPTR)TextClassNew,
   OM_DISPOSE,            (FUNCPTR)TextClassDispose,
   BASE_LOCALIZE,         (FUNCPTR)TextClassLocalize,
   DF_END,                NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitTextClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_ROOT_OBJECT,
			 CLASS_ObjectSize,     sizeof(TD),
			 CLASS_DFTable,        ClassFunc,
			 TAG_DONE);
}
///






/*
 * Figure out the total text width and height
 * of an text with info-style command sequences.
 */
#ifdef __AROS__
makearosproto
AROS_LH4(VOID, BGUI_InfoTextSize,
    AROS_LHA(struct RastPort *, rp, A0),
    AROS_LHA(UBYTE *, text, A1),
    AROS_LHA(UWORD *, wi, A2),
    AROS_LHA(UWORD *, wh, A3),
    struct Library *, BGUIBase, 18, BGUI)
#else
makeproto SAVEDS ASM VOID BGUI_InfoTextSize(REG(a0) struct RastPort *rp, REG(a1) UBYTE *text, REG(a2) UWORD *wi, REG(a3) UWORD *wh)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   if (wi) *wi = TotalWidth( rp, text);
   if (wh) *wh = TotalHeight(rp, text);

   AROS_LIBFUNC_EXIT
}

/*
 * Render the text with info-style command sequences
 * inside the bounding box. Text is automatically
 * trucated when out of bounds.
 */
#ifdef __AROS__
makearosproto
AROS_LH4(VOID, BGUI_InfoText,
    AROS_LHA(struct RastPort *, rp, A0),
    AROS_LHA(UBYTE *, text, A1),
    AROS_LHA(struct IBox *, bounds, A2),
    AROS_LHA(struct DrawInfo *, dri, A3),
    struct Library *, BGUIBase, 19, BGUI)
#else
makeproto SAVEDS ASM VOID BGUI_InfoText( REG(a0) struct RastPort *rp, REG(a1) UBYTE *text, REG(a2) struct IBox *bounds, REG(a3) struct DrawInfo *dri )
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   RenderInfoText(rp, text, PENS(dri), bounds, (UWORD)~0);

   AROS_LIBFUNC_EXIT
}

/*
 * Alignment flags.
 */
#define TF_CENTER          (1<<0)
#define TF_RIGHT           (1<<1)
#define TF_SHADOW          (1<<2)
#define TF_UNDERLINE       (1<<3)
#define TF_HIGHUNDERLINE   (1<<4)
#define TF_KEEP            (1<<5)
#define TF_WRAP            (1<<6)

/*
 * Command sequences:
 *
 * \33b     Switch to bold
 * \33i     Switch to italic
 * \33u     Switch to underlined
 * \33n     Switch to normal
 * \33s     Switch to shadow
 * \33z     Switch to underlined, offset 2
 * \33Z     Switch to highlight underlined, offset 2
 * \33c     Center this and the following lines
 * \33r     Right-align this and the following lines
 * \33l     Left-align this and the following lines
 * \33k     Keep style flags.
 * \33w     Wrap text.
 * \33d<n>  Switch to DrawInfo pen <n>
 * \33p<n>  Switch to pen <n>
 * \33D<n>  Switch to background DrawInfo pen <n>
 * \33P<n>  Switch to background pen <n>
 *
 */

/// ParseCommSeq
/*
 * Parse a command sequence.
 */
STATIC UBYTE *ParseCommSeq(struct BaseInfo *bi, UBYTE *text, UWORD *old_style, UWORD *flags)
{
   UWORD    new_style = *old_style;

   /*
    * Let's see what we have here...
    */
   switch (*text++)
   {
   case '-':                           /* Turn off.   */
      switch (*text++)
      {
      case 'b':
	 new_style &= ~FSF_BOLD;
	 break;
      case 'i':
	 new_style &= ~FSF_ITALIC;
	 break;
      case 'u':
	 new_style &= ~FSF_UNDERLINED;
	 break;
      case 's':
	 *flags &= ~TF_SHADOW;
	 break;
      case 'k':
	 *flags &= ~TF_KEEP;
	 break;
      case 'w':
	 *flags &= ~TF_WRAP;
	 break;
      case 'z':
	 *flags &= ~TF_UNDERLINE;
	 break;
      case 'Z':
	 *flags &= ~TF_HIGHUNDERLINE;
	 break;
      default:
	 text -= 2;
	 break;
      };
      break;
	 
   case 'b':                           /* Bold.       */
      new_style |= FSF_BOLD;
      break;

   case 'i':                           /* Italic.     */
      new_style |= FSF_ITALIC;
      break;

   case 'u':                           /* Underscore. */
      new_style |= FSF_UNDERLINED;
      break;

   case 'n':                           /* Normal.     */
      new_style = FS_NORMAL;
      *flags &= ~(TF_SHADOW|TF_UNDERLINE|TF_HIGHUNDERLINE);
      break;

   case 'k':                           /* Keep styles. */
      *flags |= TF_KEEP;
      break;

   case 'w':                           /* Wrap text. */
      *flags |= TF_WRAP;
      break;

   case 's':                           /* Shadowed.   */
      *flags |= TF_SHADOW;
      break;

   case 'z':                           /* Underlined offset = 2. */
      *flags |= TF_UNDERLINE;
      break;

   case 'Z':                           /* Highlight underlined offset = 2. */
      *flags |= TF_HIGHUNDERLINE;
      break;
      
   case  'c':                          /* Center.     */
      *flags &= ~TF_RIGHT;
      *flags |= TF_CENTER;
      break;

   case  'r':                          /* Right.      */
      *flags &= ~TF_CENTER;
      *flags |= TF_RIGHT;
      break;

   case  'l':                          /* Left.       */
      *flags &= ~(TF_RIGHT|TF_CENTER);
      break;

   case 'd':                           /* DriPen.     */
      BSetDPenA(bi, strtol(text, (char **)&text, 0));
      break;
   case 'D':
      BSetDrMd(bi, JAM2);
      BSetDPenB(bi, strtol(text, (char **)&text, 0));
      break;

   case 'p':                           /* Pen.        */
      BSetPenA(bi, strtol(text, (char **)&text, 0));
      break;
   case 'P':
      BSetDrMd(bi, JAM2);
      BSetPenB(bi, strtol(text, (char **)&text, 0));
      break;
      
   case '1':
      BSetDrMd(bi, JAM1);
      break;
   case '2':
      BSetDrMd(bi, JAM2);
      break;
   case 'C':
      BSetDrMd(bi, COMPLEMENT);
      break;

   case '\33':                         /* Do nothing. */
      break;
      
   case '\0':
      /*
       * EOL.
       */
   default:
      text--;
      break;
   };

   /*
    * Style changed?
    */
   if (new_style != *old_style)
   {
      *old_style = new_style;
      BSetFontStyle(bi, new_style);
   }
   return text;
}
///
/// XPos
/*
 * Determine the x position
 * where to render the line.
 */
STATIC WORD XPos(struct BaseInfo *bi, UBYTE *text, UWORD *old_style, struct IBox *domain, UWORD *flags)
{
   struct RastPort *rp = bi->bi_RPort;
   struct RastPort  rp2 = *rp;
   struct BaseInfo  bi2 = *bi;
   ULONG            line_len = 0;
   UWORD            new_style = *old_style;
   WORD             xpos, i;

   bi2.bi_RPort = &rp2;

   /*
    * Scan text.
    */
   while (*text && (*text != '\n'))
   {
      /*
       * What have we got...
       */
      switch (*text)
      {
      case '\33':
	 /*
	  * Command sequence.
	  */
	 text = ParseCommSeq(&bi2, text + 1, &new_style, flags);
	 break;

      default:
	 i = 0;
	 /*
	  * Count non-action characters.
	  */
	 while (text[i] && (text[i] != '\33') && (text[i] != '\n')) i++;

	 if (i)
	 {
	    /*
	     * Determine their length.
	     */
	    line_len += TextWidthNum(&rp2, text, i);

	    /*
	     * Adjust pointer.
	     */
	    text += i;
	 }
	 break;
      }
   }

   /*
    * Calculate x-position.
    */
   if (*flags & TF_CENTER)     xpos = domain->Left + ((domain->Width - line_len) >> 1);
   else if (*flags & TF_RIGHT) xpos = domain->Left + domain->Width - 1 - line_len;
   else                        xpos = domain->Left;

   /*
    * Don't pass the left edge.
    */
   if (xpos < domain->Left) xpos = domain->Left;

   return xpos;
}
///
/// TotalHeight
/*
 * Determine the total height of the text.
 */
//makeproto UWORD ASM TotalHeight( REG(a0) struct RastPort *rp, REG(a1) UBYTE *text )
makeproto ASM REGFUNC2(UWORD, TotalHeight,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(A1, UBYTE *, text))
{
   UWORD    nl = 1;
   UBYTE    c;

   /*
    * Count the number of lines.
    */
   if(text==NULL)
   {
      D(bug("*** NULL text pointer in TotalHeight function (%s,%ld)\n",__FILE__,__LINE__));
      return(0);
   }
   while (c = *text++)
   {
      if (c == '\n') nl++;
   }
   return (UWORD)(nl * rp->TxHeight);
}
REGFUNC_END
///
/// TotalWidth
/*
 * Determine the total width
 * of the information text.
 */
//makeproto UWORD ASM TotalWidth(REG(a0) struct RastPort *rp, REG(a1) UBYTE *text)
makeproto ASM REGFUNC2(UWORD, TotalWidth,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(A1, UBYTE *, text))
{
   struct RastPort rport = *rp;
   ULONG           line_len = 0, len = 0;
   UWORD           new_style = FS_NORMAL, flags = 0, i;

   struct BaseInfo bi;

   bi.bi_Pens  = DefDriPens;
   bi.bi_RPort = &rport;

   /*
    * Start as FS_NORMAL.
    */
   BSetFontStyle(&bi, FS_NORMAL);

   /*
    * Scan text.
    */
   if(text==NULL)
   {
      D(bug("*** NULL text pointer in TotalWidth function (%s,%ld)\n",__FILE__,__LINE__));
      return(0);
   }
   while (*text)
   {
      /*
       * What have we got.
       */
      switch (*text)
      {
      case '\33':
	 /*
	  * Escape sequence.
	  */
	 text = ParseCommSeq(&bi, ++text, &new_style, &flags);
	 break;

      default:
	 i = 0;
	 /*
	  * Count the non-action characters.
	  */
	 while (text[i] && (text[i] != '\33') && (text[i] != '\n')) i++;

	 /*
	  * Determine their length.
	  */
	 line_len += TextWidthNum(&rport, text, i);

	 /*
	  * New line or end-of-line?
	  */
	 if (text[i] == '\n' || text[i] == '\0')
	 {
	    if (!(flags & TF_WRAP))
	    {
	       /*
		* Is it wider than the previous ones?
		*/
	       if (line_len > len)
		  len = line_len;
	    };

	    /*
	     * Go on when this was a new line.
	     */
	    if (text[i] == '\n')
	    {
	       text++;
	       BSetFontStyle(&bi, FS_NORMAL);
	    };

	    /*
	     * Set line_len to 0.
	     */
	    line_len = 0;
	 };
	 text += i;
	 break;
      }
   }
   return (UWORD)len;
}
REGFUNC_END
///
/// RenderInfoText
/*
 * Render the information text.
 */
makeproto void RenderInfoText(struct RastPort *rp, UBYTE *text, UWORD *pens, struct IBox *domain, UWORD backpen)
{
   struct BaseInfo *bi;

#ifdef DEBUG_BGUI
   if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_RastPort, rp, BI_Pens, pens, TAG_DONE))
#else
   if (bi = AllocBaseInfo(BI_RastPort, rp, BI_Pens, pens, TAG_DONE))
#endif
   {
      if (!pens) pens = DefDriPens;

      BSetDrMd(bi, JAM1);
      BClearAfPt(bi);
      /*
       * First we fill the background.
       */
      if (backpen != (UWORD)~1)
      {
	 if (backpen != (UWORD)~0)
	 {
	    BSetPenA(bi, backpen);
	    BBoxFillA(bi, domain);
	 };

	 /*
	  * Set initial pen.
	  */
	 BSetDPenA(bi, (backpen == bi->bi_Pens[FILLPEN]) ? FILLTEXTPEN : TEXTPEN);
      };
      RenderText(bi, text, domain);

      FreeBaseInfo(bi);
   };
}
///
/// RenderText
/*
 * Render the information text.
 */
makeproto void RenderText(struct BaseInfo *bi, UBYTE *text, struct IBox *domain)
{
   struct RastPort     *rp = bi->bi_RPort;
   struct TextExtent    extent;
   WORD                 xpos, ypos = domain->Top + rp->TxBaseline;
   UWORD                style = FS_NORMAL, dubstyle = FS_NORMAL, flags = 0, i, nl, numc;
   ULONG                old_a, old_m;

   /*
    * Start as FS_NORMAL.
    */
   BSetFontStyle(bi, FS_NORMAL);

   /*
    * Get the height of the text.
    */
   nl = TotalHeight(rp, text);

   /*
    * Is it smaller than
    * the domain?
    */
   if (nl < domain->Height)
      /*
       * Yes. Adjust the top size
       * so that it get's centered.
       */
      ypos += (domain->Height - nl) >> 1;

   /*
    * Find out starting position.
    */
   xpos = XPos(bi, text, &dubstyle, domain, &flags);

   BSetDrMd(bi, JAM1);
   BSetFontStyle(bi, FS_NORMAL);
   flags &= ~(TF_SHADOW|TF_UNDERLINE|TF_HIGHUNDERLINE);

   /*
    * Scan text.
    */
   while (*text)
   {
      /*
       * What have we got...
       */
      switch (*text)
      {
      case '\33':
	 /*
	  * Command sequence.
	  */
	 text = ParseCommSeq(bi, ++text, &style, &flags);
	 break;

      case '\n':
	 /*
	  * Newline.
	  */
	 text++;

	 /*
	  * New position.
	  */
	 xpos = XPos(bi, text, &dubstyle, domain, &flags);

	 /*
	  * Are we passing the bottom of the
	  * rendering area?
	  */
	 if ((ypos += rp->TxHeight ) > domain->Top + domain->Height - 1)
	    return;

	 Move(rp, xpos, ypos);
	 if (!(flags & TF_KEEP))
	 {
	    BSetFontStyle(bi, FS_NORMAL);
	    flags &= ~(TF_SHADOW|TF_UNDERLINE|TF_HIGHUNDERLINE);
	 };
	 break;

      default:
	 /*
	  * Count non-action characters.
	  */
	 i = 0;
	 while (text[i] && (text[i] != '\33') && (text[i] != '\n')) i++;

	 if (i)
	 {
	    /*
	     * Render them.
	     */
	    if (numc = TextFit(rp, text, i, &extent, NULL, 1, max((LONG)(domain->Width - (xpos - domain->Left)), 0), rp->TxHeight))
	    {
	       if (flags & TF_SHADOW)
	       {
		  old_a = FGetAPen(rp);
		  old_m = FGetDrMd(rp);
		  BSetDPenA(bi, SHADOWPEN);
		  Move(rp, xpos + 1, ypos + 1);
		  Text(rp, text, numc);
		  BSetPenA(bi, old_a);
		  BSetDrMd(bi, JAM1);
		  Move(rp, xpos, ypos);
		  Text(rp, text, numc);
		  BSetDrMd(bi, old_m);
	       }
	       else
	       {
		  if (flags & (TF_UNDERLINE|TF_HIGHUNDERLINE))
		  {
		     if (flags & TF_HIGHUNDERLINE)
		     {
			old_a = FGetAPen(rp);
			BSetDPenA(bi, SHINEPEN);
		     };
		     Move(rp, xpos, ypos + 2);
		     Draw(rp, xpos + TextLength(rp, text, numc) - 2, ypos + 2);
		     if (flags & TF_HIGHUNDERLINE)
		     {
			BSetPenA(bi, old_a);
		     };
		  };
		  Move(rp, xpos, ypos);
		  Text(rp, text, numc);
	       };

	       /*
		* Skip remainder of the line
		* if possible.
		*/
	       if (numc < i)
	       {
		  while (text[i] && text[i] != '\n')
		     i++;
	       };
	       /*
		* Adjust x position.
		*/
	       xpos += TextWidthNum(rp, text, numc);
	    };
	    /*
	     * Adjust pointer.
	     */
	    text += i;
	 }
	 break;
      }
   }
}
///
