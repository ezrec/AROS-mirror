/*
 * @(#) $Header$
 *
 * BGUI library
 * frameclass.c
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
 * Revision 42.0  2000/05/09 22:09:01  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:17  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.7  1999/07/04 05:24:40  mlemos
 * Fixed attempts to render areas witrh invalid dimensions.
 *
 * Revision 41.10.2.6  1999/07/03 15:17:37  mlemos
 * Replaced the calls to CallHookPkt to BGUI_CallHookPkt.
 *
 * Revision 41.10.2.5  1999/07/03 15:12:26  mlemos
 * Ensured that frame is only drawn if dimensions are bigger than 0.
 * Removed debug code to warn when dimensions are too small.
 *
 * Revision 41.10.2.4  1999/07/03 14:29:56  mlemos
 * Added assertion code to warn frame dimensions are too small.
 *
 * Revision 41.10.2.3  1999/07/03 14:13:08  mlemos
 * Fixed off by one mistake on the computation of the minimum frame size
 *
 * Revision 41.10.2.2  1998/12/07 03:06:59  mlemos
 * Replaced OpenFont and CloseFont calls by the respective BGUI debug macros.
 *
 * Revision 41.10.2.1  1998/02/28 02:22:54  mlemos
 * Made setting the frame title to NULL to reset text title object.
 *
 * Revision 41.10  1998/02/25 21:12:05  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:20  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.

#include "include/classdefs.h"

/*
 * Object instance data.
 */
typedef struct fd_ {
   ULONG                 fd_Flags;        /* Flags.                  */
   Object               *fd_Title;        /* Frame title.            */
   struct TextAttr      *fd_TitleFont;    /* Font for the title.     */
   struct TextFont      *fd_Font;         /* The opened font.        */
   struct Hook           fd_DefBackfill;  /* Default backfill hook.  */
   struct Hook          *fd_FrameHook;    /* Custom frame hook.      */
   struct Hook          *fd_BackFillHook; /* Custom backfill hook.   */
   struct bguiPattern   *fd_Pattern;      /* Normal fill pattern.    */
   struct bguiPattern   *fd_SelPattern;   /* Selected fill pattern.  */
   Object               *fd_ParentGroup;  /* Parent Group.           */
   UWORD                 fd_Type;         /* Frame type.             */
   UWORD                 fd_BackFill;     /* Backfilling type.       */
   UBYTE                 fd_Horizontal;   /* Horizontal thickness    */
   UBYTE                 fd_Vertical;     /* Vertical thickness      */
   
   UWORD    fd_BackPen, fd_BackPen2;      /* Normal backfill pens.   */
   UWORD    fd_SelPen,  fd_SelPen2;       /* Selected backfill pens. */
   BYTE     fd_OuterX1, fd_OuterX2, fd_OuterY1, fd_OuterY2;
   BYTE     fd_InnerX1, fd_InnerX2, fd_InnerY1, fd_InnerY2;
}  FD;

#define FRF_SELFOPEN    (1<<31)
#define FRF_INBORDER    (1<<30)  /* Image in window border? */
#define FRF_BACKDRI     (1<<29)
#define FRF_SELDRI      (1<<28)
#define FRF_BACKDRI2    (1<<27)
#define FRF_SELDRI2     (1<<26)
#define FRF_CUST_THICK  (1<<25)
#define FRF_FILL_OUTER  (1<<24)

#define FD_ENTRY(tag, offset, flags) PACK_ENTRY(FRM_TAGSTART, tag, fd_, offset, flags)
#define FD_FLAG(tag, flag) PACK_LONGBIT(FRM_TAGSTART, tag, fd_, fd_Flags, PKCTRL_BIT, flag)

static ULONG FramePackTable[] =
{
   PACK_STARTTABLE(FRM_TAGSTART),

   FD_ENTRY(FRM_Flags,                   fd_Flags,         PKCTRL_ULONG),
   FD_ENTRY(FRM_CustomHook,              fd_FrameHook,     PKCTRL_ULONG),
   FD_ENTRY(FRM_BackFillHook,            fd_BackFillHook,  PKCTRL_ULONG),
   FD_ENTRY(FRM_TextAttr,                fd_TitleFont,     PKCTRL_ULONG),
   FD_ENTRY(FRM_Type,                    fd_Type,          PKCTRL_UWORD),
   FD_ENTRY(FRM_BackFill,                fd_BackFill,      PKCTRL_UWORD),
   FD_ENTRY(FRM_FillPattern,             fd_Pattern,       PKCTRL_ULONG),
   FD_ENTRY(FRM_SelectedFillPattern,     fd_SelPattern,    PKCTRL_ULONG),

   FD_ENTRY(FRM_FrameWidth,              fd_Horizontal,    PKCTRL_UBYTE),
   FD_ENTRY(FRM_FrameHeight,             fd_Vertical,      PKCTRL_UBYTE),
   FD_ENTRY(FRM_OuterOffsetLeft,         fd_OuterX1,       PKCTRL_BYTE),
   FD_ENTRY(FRM_OuterOffsetRight,        fd_OuterX2,       PKCTRL_BYTE),
   FD_ENTRY(FRM_OuterOffsetTop,          fd_OuterY1,       PKCTRL_BYTE),
   FD_ENTRY(FRM_OuterOffsetBottom,       fd_OuterY2,       PKCTRL_BYTE),
   FD_ENTRY(FRM_InnerOffsetLeft,         fd_InnerX1,       PKCTRL_BYTE),
   FD_ENTRY(FRM_InnerOffsetRight,        fd_InnerX2,       PKCTRL_BYTE),
   FD_ENTRY(FRM_InnerOffsetTop,          fd_InnerY1,       PKCTRL_BYTE),
   FD_ENTRY(FRM_InnerOffsetBottom,       fd_InnerY2,       PKCTRL_BYTE),

   FD_FLAG(FRM_EdgesOnly,                FRF_EDGES_ONLY),
   FD_FLAG(FRM_Recessed,                 FRF_RECESSED),
   FD_FLAG(FRM_CenterTitle,              FRF_CENTER_TITLE),
   FD_FLAG(FRM_HighlightTitle,           FRF_HIGHLIGHT_TITLE),
   FD_FLAG(FRM_ThinFrame,                FRF_THIN_FRAME),
   FD_FLAG(FRM_TitleLeft,                FRF_TITLE_LEFT),
   FD_FLAG(FRM_TitleRight,               FRF_TITLE_RIGHT),

   PACK_ENDTABLE
};
///

/*
 * Built-in background hook. This hook will take
 * care of the backfilling rasters and the special
 * backfilling pens.
 */
//STATIC SAVEDS ASM BuiltInBack(REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct FrameDrawMsg *fdm)
STATIC SAVEDS ASM REGFUNC3(IPTR, BuiltInBack,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct FrameDrawMsg *, fdm))
{
   FD                   *fd = INST_DATA(OCLASS(obj), obj);
   UWORD                *pens = PENS(fdm->fdm_DrawInfo), apen, bpen;
   struct RastPort      *rp = fdm->fdm_RPort;
   struct IBox           ib;
   struct bguiPattern   *pat;

   int                   fill;

   int x1 = fdm->fdm_Bounds->MinX;
   int x2 = fdm->fdm_Bounds->MaxX;
   int y1 = fdm->fdm_Bounds->MinY;
   int y2 = fdm->fdm_Bounds->MaxY;

   BOOL  sel = ((fdm->fdm_State == IDS_SELECTED) || (fdm->fdm_State == IDS_INACTIVESELECTED));

   STATIC UBYTE backfill[][2] =
   {
      { BACKGROUNDPEN, BACKGROUNDPEN },   // STANDARD_FILL
      {      SHINEPEN, BACKGROUNDPEN },   // SHINE_RASTER
      {     SHADOWPEN, BACKGROUNDPEN },   // SHADOW_RASTER
      {      SHINEPEN,     SHADOWPEN },   // SHINE_SHADOW_RASTER
      {       FILLPEN, BACKGROUNDPEN },   // FILL_RASTER
      {      SHINEPEN,       FILLPEN },   // SHINE_FILL_RASTER
      {     SHADOWPEN,       FILLPEN },   // SHADOW_FILL_RASTER
      {      SHINEPEN,      SHINEPEN },   // SHINE_BLOCK
      {     SHADOWPEN,     SHADOWPEN },   // SHADOW_BLOCK
      {       FILLPEN,       FILLPEN },   // FILL_BLOCK
   };

   /*
    * Render the backfill?
    */
   if (fdm->fdm_MethodID == FRM_RENDER)
   {
      /*
       * Setup domain.
       */
      ib.Left     =  x1;
      ib.Top      =  y1;
      ib.Width    =  x2 - x1 + 1;
      ib.Height   =  y2 - y1 + 1;

      pat = fd->fd_Pattern;
      if (sel && !pat) pat = fd->fd_SelPattern;

      if (pat)
      {
	 BGUI_FillRectPattern(rp, pat, x1, y1, x2, y2);
	 return FRC_OK;
      };

      if (fd->fd_BackFill == STANDARD_FILL)
      {
	 /*
	  * A raster backfill. Set up the pens.
	  */

	 /*
	  * First check for a normal (dri)pen.
	  * If none is given we use the default color.
	  */
	 if ((apen = fd->fd_BackPen) != (UWORD)~0)
	 {
	    if (fd->fd_Flags & FRF_BACKDRI) apen = pens[apen];

	    if ((bpen = fd->fd_BackPen2) == (UWORD)~0)
	    {
	       bpen = apen;
	    }
	    else
	    {
	       if (fd->fd_Flags & FRF_SELDRI2) bpen = pens[bpen];
	    };
	 };

	 if (sel && (fd->fd_SelPen != (UWORD)~0))
	 {
	    /*
	     * First check for a selected (dri)pen.
	     * If none is given we use the default color.
	     */
	    if ((apen = fd->fd_SelPen) != (UWORD)~0)
	    {
	       if (fd->fd_Flags & FRF_SELDRI) apen = pens[apen];

	       if ((bpen = fd->fd_SelPen2) == (UWORD)~0)
	       {
		  bpen = apen;
	       }
	       else
	       {
		  if (fd->fd_Flags & FRF_SELDRI2) bpen = pens[bpen];
	       };
	    };
	 }

	 if (apen == (UWORD)~0)
	 {
	    switch (fd->fd_Type)
	    {
	    /*
	     * NeXT, DropBox and Ridge frames do not support a SELECTED state
	     * so we force the BACKGROUNDPEN for these types.
	     */
	    case FRTYPE_NEXT:
	    case FRTYPE_DROPBOX:
	    case FRTYPE_RIDGE:
	    case FRTYPE_FUZZ_RIDGE:
	    case FRTYPE_RADIOBUTTON:
	       apen = BACKGROUNDPEN;
	       break;
	       
	    default:
	       /*
		* Check the state of the frame
		*/
	       switch (fdm->fdm_State)
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
		  apen = ((fd->fd_Flags & FRF_INBORDER) || (fd->fd_Type == FRTYPE_BORDER)) ? FILLPEN : BACKGROUNDPEN;
		  break;
	       };
	    };
	    apen = bpen = pens[apen];
	 };
      }
      else
      {
	 fill = fd->fd_BackFill;
	 if (fill > FILL_BLOCK) fill = 0;

	 apen = pens[backfill[fill][0]];
	 bpen = pens[backfill[fill][1]];
      };
      
      /*
       * Fill the frame.
       */
      RenderBackFillRaster(rp, &ib, apen, bpen);

      return FRC_OK;
   };
   return FRC_UNKNOWN;
}
REGFUNC_END

/// FRAMEM_BACKFILL
/*
 * Frame background routine.
 */
METHOD(FrameClassBackfill, struct fmBackfill *, fmb)
{
   FD                   *fd = INST_DATA(cl, obj);
   struct FrameDrawMsg   fdraw;
   
   /*
    * Setup framedraw message.
    */
   fdraw.fdm_MethodID   = FRM_RENDER;
   fdraw.fdm_RPort      = fmb->fmb_BInfo->bi_RPort;
   fdraw.fdm_DrawInfo   = fmb->fmb_BInfo->bi_DrInfo;
   fdraw.fdm_Bounds     = fmb->fmb_Bounds;
   fdraw.fdm_State      = fmb->fmb_State;

   /*
    * Call the custom back-fill hook routine.
    * Note that this can also be the default backfill hook
    * which is defined above.
    */
   BGUI_CallHookPkt(fd->fd_BackFillHook, (VOID *)obj, (VOID *)&fdraw);

   return 1;
}
METHOD_END

#define Line(x1,y1,x2,y2) { Move(rp, x1, y1); Draw(rp, x2, y2); }
#define Point(x1,y1) WritePixel(rp, x1, y1)

/// RenderRadioFrame
/*
 * Render the radio button frame
 */
STATIC VOID RenderRadioFrame(struct bmRender *bmr, WORD l, WORD t, WORD r, WORD b, FD *fd)
{
   struct BaseInfo *bi = bmr->bmr_BInfo;
   struct RastPort *rp = bi->bi_RPort;
   WORD             array[16], *a;
   BOOL             normal = !(fd->fd_Flags & FRF_RECESSED);
   BOOL             thin = fd->fd_Flags & FRF_THIN_FRAME;

   /*
    * Check if we need to draw a normal or a selected frame
    */
   switch (bmr->bmr_Flags)
   {
   case IDS_SELECTED:
   case IDS_INACTIVESELECTED:
      normal = !normal;
      break;
   };

   /*
    * Draw the shine side of the frame
    */
   a = array;

   *a++ = l + 2;  *a++ = t;
   *a++ = l;      *a++ = t + 2;
   *a++ = l;      *a++ = b - 2;
   *a++ = l + 2;  *a++ = b;
   *a++ = l + 2;  *a++ = b - 1;

   /*
    * Thin lines?
    */
   if (!thin)
   {
   *a++ = l + 1;  *a++ = b - 2;
   *a++ = l + 1;  *a++ = t + 2;
   *a++ = l + 2;  *a++ = t + 1;
   };

   BSetDPenA(bi, normal ? SHINEPEN : SHADOWPEN);
   Move(rp, r - 3, t);
   PolyDraw(rp, (a - array) >> 1, array);

   /*
    * Draw the shadow side of the frame
    */
   a = array;

   *a++ = r - 2;  *a++ = b;
   *a++ = r;      *a++ = b - 2;
   *a++ = r;      *a++ = t + 2;
   *a++ = r - 2;  *a++ = t;
   *a++ = r - 2;  *a++ = t + 1;

   /*
    * Thin lines?
    */
   if (!thin)
   {
   *a++ = r - 1;  *a++ = t + 2;
   *a++ = r - 1;  *a++ = b - 2;
   *a++ = r - 2;  *a++ = b - 1;
   };

   BSetDPenA(bi, normal ? SHADOWPEN : SHINEPEN);
   Move(rp, l + 3, b);
   PolyDraw(rp, (a - array) >> 1, array);
}
///
/// RenderTabFrame
/*
 * Render the tab frame.
 */
STATIC VOID RenderTabFrame(struct bmRender *bmr, int l0, int t0, int r0, int b0, FD *fd)
{
   struct BaseInfo *bi = bmr->bmr_BInfo;
   struct RastPort *rp = bi->bi_RPort;

   int l1 = l0 + 1, l2 = l0 + 2, l3 = l0 + 3, l4 = l0 + 4;
   int r1 = r0 - 1, r2 = r0 - 2, r3 = r0 - 3, r4 = r0 - 4;
   int t1 = t0 + 1, t2 = t0 + 2, t3 = t0 + 3, t4 = t0 + 4;
   int b1 = b0 - 1, b2 = b0 - 2, b3 = b0 - 3, b4 = b0 - 4;

   BOOL sel  = ((bmr->bmr_Flags == IDS_SELECTED) || (bmr->bmr_Flags == IDS_INACTIVESELECTED));
   BOOL thin = fd->fd_Flags & FRF_THIN_FRAME;

   switch (fd->fd_Type)
   {
   case FRTYPE_TAB_TOP:
      /*
       * Shine side.
       */
      BSetDPenA(bi, SHINEPEN);
      if (sel)
      {
	 Point(l0, b0);
	 Line(l1, t2, l1, b1); if (!thin) Line(l2, t2, l2, b1);
	 Line(l2, t1, l3, t1);
	 Line(l3, t0, r3, t0);
      }
      else
      {
	 Line(l0, b0, r0, b0);
	 Line(l2, t3, l2, b1); if (!thin) Line(l3, t3, l3, b1);
	 Line(l3, t2, l4, t2);
	 Line(l4, t1, r4, t1);
      };

      /*
       * Shadow side.
       */
      BSetDPenA(bi, SHADOWPEN);
      if (sel)
      {
	 Line(r2, t1, r3, t1);
	 Line(r1, t2, r1, b1); if (!thin) Line(r2, t2, r2, b1);
	 Point(r0, b0);
      }
      else
      {
	 Line(r3, t2, r4, t2);
	 Line(r2, t3, r2, b1); if (!thin) Line(r3, t3, r3, b1);
      };
      break;

   case FRTYPE_TAB_BOTTOM:
      /*
       * Shine side.
       */
      BSetDPenA(bi, SHINEPEN);
      if (sel)
      {
	 Point(l0, t0);
	 Line(l1, t1, l1, b2); if (!thin) Line(l2, t1, l2, b2);
	 Line(l2, b1, l3, b1);
      }
      else
      {
	 Line(l2, t1, l2, b3); if (!thin) Line(l3, t1, l3, b3);
	 Line(l3, b2, l4, b2);
      };

      /*
       * Shadow side.
       */
      BSetDPenA(bi, SHADOWPEN);
      if (sel)
      {
	 Point(r0, t0);
	 Line(r3, b1, r2, b1);
	 Line(r1, t1, r1, b2); if (!thin) Line(r2, t1, r2, b2);
	 Line(l3, b0, r3, b0);
      }
      else
      {
	 Line(l0, t0, r0, t0);
	 Line(r4, b2, r3, b2);
	 Line(r2, t1, r2, b3); if (!thin) Line(r3, t1, r3, b3);
	 Line(l4, b1, r4, b1);
      };
      break;
   };
}
///
/// RenderXenFrame
/*
 * Render the XEN button frame.
 */
STATIC VOID RenderXenFrame(struct bmRender *bmr, WORD l, WORD t, WORD r, WORD b, FD *fd)
{
   struct BaseInfo *bi = bmr->bmr_BInfo;
   struct RastPort *rp = bi->bi_RPort;

   /*
    * Setup pen for the XEN lines
    * around the normal button frame.
    */
   BSetDPenA(bi, SHADOWPEN);
   BSetDrMd(bi, JAM1);

   /*
    * Move & Render...
    */
   t++; l++, r--; b--;

   HLine(rp, l, t - 1, r);
   VLine(rp, r - 1, t, b);
   HLine(rp, l, b - 1, r);
   VLine(rp, l - 1, t, b);

   /*
    * Render bevel.
    */
   RenderBevelBox(bi, l, t, r, b, bmr->bmr_Flags, fd->fd_Flags & FRF_RECESSED, fd->fd_Flags & FRF_THIN_FRAME);
}
///

/*
 * Pass on the frame thickness.
 */
//STATIC ASM VOID FrameThickness(REG(a0) Class *cl, REG(a2) Object *obj)
STATIC ASM REGFUNC2(VOID, FrameThickness,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj))
{
   FD                  *fd = INST_DATA(cl, obj);
   struct ThicknessMsg  tm;
   UBYTE                h, v;
   int                  type;
   BOOL                 thin;

   STATIC UBYTE thick[][4] =
   {
      { 1, 1, 1, 1 },   // FRTYPE_CUSTOM
      { 2, 1, 1, 1 },   // FRTYPE_BUTTON
      { 4, 2, 2, 2 },   // FRTYPE_RIDGE
      { 6, 3, 3, 3 },   // FRTYPE_DROPBOX
      { 4, 2, 2, 2 },   // FRTYPE_NEXT
      { 1, 1, 1, 1 },   // FRTYPE_RADIOBUTTON
      { 3, 2, 2, 2 },   // FRTYPE_XEN_BUTTON
      { 2, 1, 1, 1 },   // FRTYPE_TAB_ABOVE
      { 2, 1, 1, 1 },   // FRTYPE_TAB_BELOW
      { 1, 1, 1, 1 },   // FRTYPE_BORDER
      { 0, 0, 0, 0 },   // FRTYPE_NONE
      { 3, 2, 3, 3 },   // FRTYPE_FUZZ_BUTTON
      { 3, 2, 3, 3 },   // FRTYPE_FUZZ_RIDGE
      { 6, 2, 5, 2 },   // FRTYPE_TAB_TOP
      { 6, 2, 5, 2 },   // FRTYPE_TAB_BOTTOM
      { 2, 6, 2, 5 },   // FRTYPE_TAB_LEFT
      { 2, 6, 2, 5 },   // FRTYPE_TAB_RIGHT
   };

   if (!(fd->fd_Flags & FRF_CUST_THICK))
   {
      thin = fd->fd_Flags & FRF_THIN_FRAME;

      /*
       * Do we have a hook?
       */
      if (fd->fd_FrameHook)
      {
	 /*
	  * Yes. Call it to get the thickness
	  * of the custom frame.
	  */
	 tm.tm_MethodID             = FRM_THICKNESS;
	 tm.tm_Thickness.Horizontal = &h;
	 tm.tm_Thickness.Vertical   = &v;
	 tm.tm_Thin                 = thin;

	 /*
	  * If this method is not understood
	  * by the hook we use default values.
	  */
	 if (BGUI_CallHookPkt(fd->fd_FrameHook, (VOID *)obj, (VOID *)&tm) != FRC_OK)
	 {
	    h = v = 1;
	 };
      }
      else
      {
	 /*
	  * Setup frame thickness of the internal frames.
	  */
	 type = fd->fd_Type;
	 if (type > FRTYPE_FUZZ_RIDGE) type = 0;

	 h = thick[type][thin ? 2 : 0];
	 v = thick[type][thin ? 3 : 1];
      };
      fd->fd_Horizontal = h;
      fd->fd_Vertical   = v;
   };
}
REGFUNC_END

/*
 * Setup frame attributes.
 */
//STATIC ASM VOID SetFrameAttrs(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct TagItem *attr)
STATIC ASM REGFUNC3(VOID, SetFrameAttrs,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct TagItem *, attr))
{
   FD                *fd = INST_DATA(cl, obj);
   struct TextFont   *tf;
   struct TagItem    *tstate = attr, *tag;
   ULONG     temp = NULL;
   Object   *ob   = NULL;
   ULONG     data;

   if (tag = FindTagItem(FRM_Template, attr))
   {
      ob = (Object *)tag->ti_Data;
      if (ob)
      {
	 Get_Attr(ob, FRM_Template, &temp);
	 if (temp)
	 {
	    CopyMem((FD *)temp, fd, sizeof(FD));
	    fd->fd_Flags &= ~FRF_SELFOPEN;
	 };
      };
   };

   BGUI_PackStructureTags((APTR)fd, FramePackTable, attr);

   /*
    * Set attributes.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;       
   
      switch (tag->ti_Tag)
      {
      case BT_ParentGroup:
	 fd->fd_ParentGroup = (Object *)data;
	 break;

      case FRM_FrameWidth:
      case FRM_FrameHeight:
	 fd->fd_Flags |= FRF_CUST_THICK;
	 break;

      case IMAGE_TextFont:
	 if (fd->fd_Font && (fd->fd_Flags & FRF_SELFOPEN))
	    BGUI_CloseFont(fd->fd_Font);
	 fd->fd_Font   = (struct TextFont *)data;
	 fd->fd_Flags &= ~FRF_SELFOPEN;
	 break;

      case FRM_TextAttr:
	 if (data)
	 {
	    if (tf = BGUI_OpenFont((struct TextAttr *)tag->ti_Data))
	    {
	       if (fd->fd_Font && (fd->fd_Flags & FRF_SELFOPEN))
		  BGUI_CloseFont(fd->fd_Font);
	       fd->fd_Font  = tf;
	       fd->fd_TitleFont = (struct TextAttr *)tag->ti_Data;
	       fd->fd_Flags   |= FRF_SELFOPEN;
	    }
	 }
	 break;

      case FRM_BackFill:
	 fd->fd_BackFill   = data;
	 fd->fd_BackFillHook = NULL;
	 fd->fd_Flags      &= ~FRF_EDGES_ONLY;
	 break;
	    
      case FRM_BackRasterPen:
	 fd->fd_BackPen2   = data;
	 fd->fd_Flags      &= ~FRF_BACKDRI2;
	 if (fd->fd_BackPen != (UWORD)~0) break;
      case FRM_BackPen:
	 fd->fd_BackPen    = data;
	 fd->fd_Flags      &= ~FRF_BACKDRI;
	 break;

      case FRM_BackRasterDriPen:
	 fd->fd_BackPen2   = data;
	 fd->fd_Flags      |= FRF_BACKDRI2;
	 if (fd->fd_BackPen != (UWORD)~0) break;
      case FRM_BackDriPen:
	 fd->fd_BackPen    = data;
	 fd->fd_Flags      |= FRF_BACKDRI;
	 break;

      case FRM_SelectedBackRasterPen:
	 fd->fd_SelPen2    = data;
	 fd->fd_Flags      &= ~FRF_SELDRI2;
	 if (fd->fd_SelPen != (UWORD)~0) break;
      case FRM_SelectedBackPen:
	 fd->fd_SelPen     = data;
	 fd->fd_Flags      &= ~FRF_SELDRI;
	 break;

      case FRM_SelectedBackRasterDriPen:
	 fd->fd_SelPen2    = data;
	 fd->fd_Flags      |= FRF_SELDRI2;
	 if (fd->fd_SelPen != (UWORD)~0) break;
      case FRM_SelectedBackDriPen:
	 fd->fd_SelPen     = data;
	 fd->fd_Flags      |= FRF_SELDRI;
	 break;

      case FRM_Title:
	 if (!fd->fd_Title && data)
	    fd->fd_Title = BGUI_NewObject(BGUI_TEXT_GRAPHIC, TEXTA_CopyText, TRUE, TAG_DONE);
	 if (fd->fd_Title)
	 {
	    if(data)
	       DoSetMethodNG(fd->fd_Title, TEXTA_Text, data, TAG_DONE);
	    else
	    {
	       DisposeObject(fd->fd_Title);
	       fd->fd_Title=NULL;
	    }
	 }
	 break;

      case FRM_TitleID:
	 if (!fd->fd_Title) fd->fd_Title = BGUI_NewObject(BGUI_TEXT_GRAPHIC, TEXTA_CopyText, TRUE, TAG_DONE);
	 if (fd->fd_Title) DoSetMethodNG(fd->fd_Title, TEXTA_TextID, data, TAG_DONE);
	 break;

	 /*
	  * BGUI private!
	  */
      case IMAGE_InBorder:
	 if (data) fd->fd_Flags |= FRF_INBORDER;
	 else      fd->fd_Flags &= ~FRF_INBORDER;
	 break;

      case FRM_Type:
      case FRM_ThinFrame:
      case FRM_CustomHook:
	 FrameThickness(cl, obj);
	 break;
      };
   };
   
   /*
    * Sanity checks.
    */
   if (fd->fd_FrameHook)  fd->fd_Type = FRTYPE_CUSTOM;
   else if (!fd->fd_Type) fd->fd_Type = FRTYPE_BUTTON;
      
   if (fd->fd_BackFillHook == NULL)
      fd->fd_BackFillHook = &fd->fd_DefBackfill;

   switch (fd->fd_Type)
   {
   case FRTYPE_TAB_TOP:
   case FRTYPE_TAB_BOTTOM:
   case FRTYPE_TAB_LEFT:
   case FRTYPE_TAB_RIGHT:
      fd->fd_Flags |= FRF_FILL_OUTER;
      break;
   default:
      fd->fd_Flags &= ~FRF_FILL_OUTER;
      break;
   }
}
REGFUNC_END

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(FrameClassNew, struct opSet *, ops)
{
   FD             *fd;
   ULONG           rc;

   /*
    * First we let the superclass
    * create a new object.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      /*
       * Get us the instance data.
       */
      fd = INST_DATA(cl, rc);

      /*
       * Setup defaults.
       */
      fd->fd_Type     = FRTYPE_BUTTON;
      fd->fd_BackPen  = (UWORD)~0;
      fd->fd_SelPen   = (UWORD)~0;
      fd->fd_BackPen2 = (UWORD)~0;
      fd->fd_SelPen2  = (UWORD)~0;
      
      fd->fd_DefBackfill.h_Entry = (HOOKFUNC)BuiltInBack;
      
      /*
       * Setup the object.
       */
      SetFrameAttrs(cl, (Object *)rc, ops->ops_AttrList);
      FrameThickness(cl, (Object *)rc);
   };

   return rc;
}
METHOD_END
///
/// OM_SET
/*
 * Change one or more attrubutes.
 */
METHOD(FrameClassSet, struct opSet *, ops)
{
   FD              *fd = INST_DATA(cl, obj);
   ULONG            rc;

   /*
    * First we let the superclass
    * change the attributes it
    * knows.
    */
   rc = AsmDoSuperMethodA(cl, obj, (Msg)ops);

   /*
    * Setup attributes.
    */
   SetFrameAttrs(cl, obj, ops->ops_AttrList);

   return rc;
}
METHOD_END
///
/// OM_GET
/*
 * Give an attribute value.
 */
METHOD(FrameClassGet, struct opGet *, opg)
{
   FD       *fd = INST_DATA(cl, obj);
   ULONG     rc = 1, tag = opg->opg_AttrID, *store = opg->opg_Storage;

   /*
    * First we see if the attribute
    * they want is known to us. If not
    * we pass it onto the superclass.
    */
   switch (tag)
   {
      case FRM_Template:
	 STORE fd;
	 break;
      
      case FRM_FrameWidth:
	 STORE fd->fd_Horizontal;
	 break;
      case FRM_FrameHeight:
	 STORE fd->fd_Vertical;
	 break;
	 
      case FRM_BackPen:
	 STORE fd->fd_Flags & FRF_BACKDRI    ? ~0 : fd->fd_BackPen;
	 break;
      case FRM_BackDriPen:
	 STORE fd->fd_Flags & FRF_BACKDRI    ? fd->fd_BackPen : ~0;
	 break;
      case FRM_SelectedBackPen:
	 STORE fd->fd_Flags & FRF_SELDRI     ? ~0 : fd->fd_SelPen;
	 break;
      case FRM_SelectedBackDriPen:
	 STORE fd->fd_Flags & FRF_SELDRI     ? fd->fd_SelPen : ~0;
	 break;
      case FRM_BackRasterPen:
	 STORE fd->fd_Flags & FRF_BACKDRI2   ? ~0 : fd->fd_BackPen2;
	 break;
      case FRM_BackRasterDriPen:
	 STORE fd->fd_Flags & FRF_BACKDRI2   ? fd->fd_BackPen2 : ~0;
	 break;
      case FRM_SelectedBackRasterPen:
	 STORE fd->fd_Flags & FRF_SELDRI2    ? ~0 : fd->fd_SelPen2;
	 break;
      case FRM_SelectedBackRasterDriPen:
	 STORE fd->fd_Flags & FRF_SELDRI2    ? fd->fd_SelPen2 : ~0;
	 break;
	 
      default:
	 rc = BGUI_UnpackStructureTag((UBYTE *)fd, FramePackTable, tag, store);
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
METHOD(FrameClassDispose, Msg, msg)
{
   FD       *fd = INST_DATA(cl, obj);

   /*
    * Close the font.
    */
   if (fd->fd_Font && (fd->fd_Flags & FRF_SELFOPEN))
      BGUI_CloseFont(fd->fd_Font);

   /*
    * Dispose of the text graphic.
    */
   if (fd->fd_Title) DisposeObject(fd->fd_Title);

   /*
    * The rest goes to the superclass.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// FRAMEM_SETUPBOUNDS

METHOD(FrameClassSetupBounds, struct fmSetupBounds *, fmsb)
{
   FD            *fd = INST_DATA(cl, obj);
   struct IBox   *hb = fmsb->fmsb_HitBox;
   struct IBox   *ib = fmsb->fmsb_InnerBox;
   int            ox1 = fd->fd_OuterX1;
   int            ox2 = fd->fd_OuterX2;
   int            oy1 = fd->fd_OuterY1;
   int            oy2 = fd->fd_OuterY2;

   hb->Left   += ox1;
   hb->Top    += oy1;
   hb->Width  -= ox1 + ox2;
   hb->Height -= oy1 + oy2;

   ox1 += fd->fd_InnerX1 + fd->fd_Horizontal;
   ox2 += fd->fd_InnerX2 + fd->fd_Horizontal;
   oy1 += fd->fd_InnerY1 + fd->fd_Vertical;
   oy2 += fd->fd_InnerY2 + fd->fd_Vertical;

   if (fd->fd_Title)
   {
      if (fd->fd_Font)           oy1 += fd->fd_Font->tf_YSize;
      else if (fd->fd_TitleFont) oy1 += fd->fd_TitleFont->ta_YSize;
   };

   ib->Left   += ox1;
   ib->Top    += oy1;
   ib->Width  -= ox1 + ox2;
   ib->Height -= oy1 + oy2;

   SetImageBounds(obj, fmsb->fmsb_HitBox);

   return 1;
}
METHOD_END
///
/// BASE_DIMENSIONS

METHOD(FrameClassDimensions, struct bmDimensions *, bmd)
{
   FD                *fd = INST_DATA(cl, obj);
   struct bguiExtent *be = bmd->bmd_Extent;
   int                ox = (fd->fd_Horizontal << 1) + fd->fd_InnerX1 + fd->fd_InnerX2;
   int                oy = (fd->fd_Vertical   << 1) + fd->fd_InnerY1 + fd->fd_InnerY2;

   if (fd->fd_Title)
   {
      if (fd->fd_Font)           oy += fd->fd_Font->tf_YSize;
      else if (fd->fd_TitleFont) oy += fd->fd_TitleFont->ta_YSize;
   };

   be->be_Min.Width  += ox;
   be->be_Min.Height += oy;

   be->be_Nom.Width  += ox;
   be->be_Nom.Height += oy;
   
   return 1;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the frame.
 */
METHOD(FrameClassRender, struct bmRender *, bmr)
{
   FD                  *fd = INST_DATA(cl, obj);
   struct BaseInfo     *bi = bmr->bmr_BInfo;
   struct RastPort     *rp = bi->bi_RPort;
   struct Rectangle     rect;
   struct FrameDrawMsg  fdraw;
   WORD                 l, t, r, b, place;
   ULONG                rc = 1;
   int                  tv, th, dv, dh, shadow, shine, tmp, tmp2;
   int                  state = bmr->bmr_Flags;
   int                  i_fuzz, i_blank, i_swap, i_mix;
   Object              *fr;

   static UWORD pat[] = { 0x5555, 0xAAAA };

   static BYTE taboff[4][4][2] =
   {
      {  { 3, 2 }, { 3, 2 }, { 2, 1 }, { 0, 0 }  },
      {  { 3, 2 }, { 3, 2 }, { 0, 0 }, { 2, 1 }  },
      {  { 2, 1 }, { 0, 0 }, { 3, 2 }, { 3, 2 }  },
      {  { 0, 0 }, { 2, 1 }, { 3, 2 }, { 3, 2 }  },
   };

   BSetDrMd(bi, JAM2);

   /*
    * Calculate the frame position and dimensions
    */
   l = IMAGE(obj)->LeftEdge;
   t = IMAGE(obj)->TopEdge;
   r = IMAGE(obj)->Width  + l - 1;
   b = IMAGE(obj)->Height + t - 1;

   if (fd->fd_Title)
   {
      /*
       * Setup the font if one is present.
       */
      if (fd->fd_Font) BSetFont(bi, fd->fd_Font);

      /*
       * Adjust top position if there is a title present.
       */
      if (fd->fd_Flags & FRF_CENTER_TITLE)
	 t += (rp->TxBaseline - (rp->TxHeight >> 1));
      else
	 t += rp->TxBaseline;
   };

   rect.MinX = l;
   rect.MinY = t;
   rect.MaxX = r;
   rect.MaxY = b;
   /*
    * First we do the background.
    *
    * When this is a FRF_EDGES_ONLY frame we do nothing.
    * No hooks are called, and nothing is filled.
    */
   if (!(fd->fd_Flags & FRF_EDGES_ONLY))
   {
      if (fd->fd_Flags & FRF_FILL_OUTER)
      {
	 if (fr = fd->fd_ParentGroup ? BASE_DATA(fd->fd_ParentGroup)->bc_Frame : NULL)
	 {
	    AsmDoMethod(fr, FRAMEM_BACKFILL, bi, &rect, IDS_NORMAL);
	 }
	 else
	 {
	    BSetDPenA(bi, BACKGROUNDPEN);
	    BRectFillA(bi, &rect);
	 };
	 tmp  = fd->fd_Type - FRTYPE_TAB_TOP;
	 tmp2 = ((state == IDS_SELECTED) || (state == IDS_SELECTED)) ? 1 : 0;

	 rect.MinX += taboff[tmp][0][tmp2];
	 rect.MaxX -= taboff[tmp][1][tmp2];
	 rect.MinY += taboff[tmp][2][tmp2];
	 rect.MaxY -= taboff[tmp][3][tmp2];

	 state = IDS_NORMAL;
      };
      AsmDoMethod(obj, FRAMEM_BACKFILL, bi, &rect, state);
   };

   /*
    * Get frame thickness.
    */
   th = fd->fd_Horizontal;
   tv = fd->fd_Vertical;
   
   if ((th << 1) > (r - l + 1)) th = (r - l + 1) >> 1;
   if ((tv << 1) > (b - t + 1)) tv = (b - t + 1) >> 1;
   
   /*
    * Draw the frame (whatever type it is)
    */
   state = bmr->bmr_Flags;
   
   /*
    * Selected or normal?
    */
   switch (state)
   {
   case IDS_SELECTED:
   case IDS_INACTIVESELECTED:
      shadow = SHINEPEN;
      shine  = SHADOWPEN;
      break;

   default:
      shadow = SHADOWPEN;
      shine  = SHINEPEN;
      break;
   };

   /*
    * Swap pens if we must
    * recess the frame.
    */
   if (fd->fd_Flags & FRF_RECESSED)
   {
      tmp    = shadow;
      shadow = shine;
      shine  = tmp;
   }

   if(th>0
   && tv>0)
   {
      switch (fd->fd_Type)
      {
	 case  FRTYPE_CUSTOM:
	    /*
	     * Fill in the data structures for the custom frame hook
	     */
	    fdraw.fdm_MethodID   = FRM_RENDER;
	    fdraw.fdm_RPort      = rp;
	    fdraw.fdm_DrawInfo   = bi->bi_DrInfo;
	    fdraw.fdm_Bounds     = &rect;
	    fdraw.fdm_State      = state;
	    fdraw.fdm_Horizontal = th;
	    fdraw.fdm_Vertical   = tv;
	    /*
	     * Call the hook routine.
	     */
	    rc = BGUI_CallHookPkt(fd->fd_FrameHook, (VOID *)obj, (VOID *)&fdraw);
	    break;

	 case FRTYPE_RIDGE:
	 case FRTYPE_NEXT:
	 case FRTYPE_DROPBOX:
	 case FRTYPE_FUZZ_BUTTON:
	 case FRTYPE_FUZZ_RIDGE:
	 case FRTYPE_BUTTON:
	    i_fuzz   = -1;
	    i_blank  = -1;
	    i_swap   = -1;
	    i_mix    = -1;
	    
	    dv = (tv + th - 1) / th;   if (dv < 1) dv = 1;
	    dh = (th + tv - 1) / tv;   if (dh < 1) dh = 1;
	    
	    if (fd->fd_Type == FRTYPE_FUZZ_BUTTON)
	    {
	       /*
		* 1/4 Normal, 3/4 Raster
		*/
	       i_fuzz = (tv / dv) / 4;
	    };
	    if (fd->fd_Type == FRTYPE_FUZZ_RIDGE)
	    {
	       /*
		* 1/3 Normal, 1/3 Blank, 1/3 Recessed
		*/
	       tmp      = shine;
	       tmp2     = shadow;
	       i_mix    = (tv / dv) / 3 - 1;
	       i_swap   = (2 * tv / dv) / 3 - 1;
	    };
	    if (fd->fd_Type == FRTYPE_DROPBOX)
	    {
	       /*
		* 1/3 Normal, 1/3 Blank, 1/3 Recessed
		*/
	       tmp      = shine;
	       tmp2     = shadow;
	       i_blank  = (tv / dv) / 3 - 1;
	       i_swap   = (2 * tv / dv) / 3 - 1;
	    };
	    if (fd->fd_Type == FRTYPE_NEXT)
	    {
	       /*
		* 1/2 Recessed, 1/2 Normal
		*/
	       tmp2     = shine;
	       tmp      = shadow;
	       shine    = tmp;
	       shadow   = tmp2;
	       i_swap   = (tv / dv) / 2 - 1;
	    };
	    if (fd->fd_Type == FRTYPE_RIDGE)
	    {
	       /*
		* 1/2 Normal, 1/2 Recessed
		*/
	       tmp      = shine;
	       tmp2     = shadow;
	       i_swap   = (tv / dv) / 2 - 1;
	    };
	    
	    /*
	     * Render the bevelbox.
	     */
	    while ((tv > 0) && (th > 0))
	    {
	       if (shine >= 0)  BSetDPenA(bi, shine);
	       if(dh>0
	       && t<=b)
		  BRectFill(bi, l, t, l + dh - 1, b);   l += dh;
	       if (shadow >= 0) BSetDPenA(bi, shadow);
	       if(dv>0
	       && l<=r)
		  BRectFill(bi, l, b - dv + 1, r, b);   b -= dv;
	       if(dh>0
	       && t<=b)
		  BRectFill(bi, r - dh + 1, t, r, b);   r -= dh;
	       if (shine >= 0)  BSetDPenA(bi, shine);
	       if(dv>0
	       && l<=r)
		  BRectFill(bi, l, t, r, t + dv - 1);   t += dv;
	       tv -= dv;
	       th -= dh;
	       
	       if (i_fuzz-- == 0)
	       {
		  BSetAfPt(bi, pat, 1);
	       };
	       if (i_blank-- == 0)
	       {
		  shine    = BACKGROUNDPEN;
		  shadow   = BACKGROUNDPEN;
	       };
	       if (i_swap-- == 0)
	       {
		  BClearAfPt(bi);
		  shine    = tmp2;
		  shadow   = tmp;
	       };
	       if (i_mix-- == 0)
	       {
		  BSetAfPt(bi, pat, 1);
		  BSetDPenA(bi, shine);
		  BSetDPenB(bi, shadow);
		  shine = shadow = -1;
	       };
	    };
	    if (tv > 0)
	    {
	       BSetDPenA(bi, shine);
	       BRectFill(bi, l, t, r, t + tv - 1);
	       BSetDPenA(bi, shadow);
	       BRectFill(bi, l, b - tv + 1, r, b);
	    };
	    if (th > 0)
	    {
	       BSetDPenA(bi, shine);
	       BRectFill(bi, l, t, l + th - 1, b);
	       BSetDPenA(bi, shadow);
	       BRectFill(bi, r - th + 1, t, r, b);
	    };
	    break;

	 case FRTYPE_RADIOBUTTON:
	    RenderRadioFrame(bmr, l, t, r, b, fd);
	    break;

	 case FRTYPE_XEN_BUTTON:
	    RenderXenFrame(bmr, l, t, r, b, fd);
	    break;

	 case FRTYPE_TAB_TOP:
	 case FRTYPE_TAB_BOTTOM:
	 case FRTYPE_TAB_LEFT:
	 case FRTYPE_TAB_RIGHT:
	    RenderTabFrame(bmr, l, t, r, b, fd);
	    break;

	 case FRTYPE_TAB_ABOVE:
	    /*
	     * Render the frame.
	     */
	    BSetDPenA(bi, shine);
	    BRectFill(bi, l, t, l + th - 1, b - tv);
	    BSetDPenA(bi, shadow);
	    BRectFill(bi, l + 1, b - tv + 1, r, b);
	    BRectFill(bi, r - th + 1, t, r, b);
	    break;

	 case FRTYPE_TAB_BELOW:
	    /*
	     * Render the frame.
	     */
	    BSetDPenA(bi, shine);
	    BRectFill(bi, l, t, l + th - 1, b - tv + 1);
	    BRectFill(bi, l, t, r - th + 1, t + tv - 1);
	    BSetDPenA(bi, shadow);
	    BRectFill(bi, r - th + 1, t, r, b);
	    break;

	 case FRTYPE_BORDER:
	    /*
	     * Render the borderbox.
	     */
	    RenderBevelBox(bi, l, t, r, b, state, fd->fd_Flags & FRF_RECESSED, TRUE);
	    break;

	 case FRTYPE_NONE:
	    break;
      }
   }

   /*
    * Render frame the title.
    */
   if (fd->fd_Title)
   {
      place = 0;

      /*
       * Left or right?
       */
      if (fd->fd_Flags      & FRF_TITLE_LEFT ) place = 1;
      else if (fd->fd_Flags & FRF_TITLE_RIGHT) place = 2;

      /*
       * Render the title.
       */
      RenderTitle(fd->fd_Title, bi, l, t, r - l + 1,
		  fd->fd_Flags & FRF_HIGHLIGHT_TITLE, fd->fd_Flags & FRF_CENTER_TITLE, place);

   }

   return rc;
}
METHOD_END
///
/// BASE_LOCALIZE

METHOD(FrameClassLocalize, struct bmLocalize *, bml)
{
   FD    *fd = INST_DATA(cl, obj);
   ULONG  rc = 0;

   if (fd->fd_Title) rc = AsmDoMethodA(fd->fd_Title, (Msg)bml);
   
   return rc;
}
METHOD_END
///
/// Class initialization.

/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,         (FUNCPTR)FrameClassRender,
   BASE_DIMENSIONS,     (FUNCPTR)FrameClassDimensions,
   FRAMEM_BACKFILL,     (FUNCPTR)FrameClassBackfill,
   FRAMEM_SETUPBOUNDS,  (FUNCPTR)FrameClassSetupBounds,
   OM_SET,              (FUNCPTR)FrameClassSet,
   OM_GET,              (FUNCPTR)FrameClassGet,
   OM_NEW,              (FUNCPTR)FrameClassNew,
   OM_DISPOSE,          (FUNCPTR)FrameClassDispose,
   BASE_LOCALIZE,       (FUNCPTR)FrameClassLocalize,
   DF_END,              NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitFrameClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_IMAGE_OBJECT,
			 CLASS_ObjectSize,     sizeof(FD),
			 CLASS_DFTable,        ClassFunc,
			 TAG_DONE);
}
///

