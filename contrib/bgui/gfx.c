/*
 * @(#) $Header$
 *
 * BGUI library
 * gfx.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.6  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.5  2003/01/18 19:09:56  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.4  2000/08/09 11:45:57  chodorowski
 * Removed a lot of #ifdefs that disabled the AROS_LIB* macros when not building on AROS. This is now handled in contrib/bgui/include/bgui_compilerspecific.h.
 *
 * Revision 42.3  2000/05/29 00:40:23  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
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
 * Revision 42.0  2000/05/09 22:09:05  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:20  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.2  1999/07/04 05:16:05  mlemos
 * Added a debuging version of the function BRectFill.
 *
 * Revision 41.10.2.1  1998/07/05 19:26:58  mlemos
 * Added debugging code to trap invalid RectFill calls.
 *
 * Revision 41.10  1998/02/25 21:12:07  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:24  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/*
 * Default drawinfo pens.
 */
makeproto UWORD DefDriPens[12] = { 0, 1, 1, 2, 1, 3, 1, 0, 2, 1, 2, 1 };

/*
 * Disabled pattern.
 */
STATIC UWORD DisPat[2] = { 0x2222, 0x8888 };

/*
 * Calculate the text width.
 */
//makeproto ASM ULONG TextWidth(REG(a1) struct RastPort *rp, REG(a0) UBYTE *text)
makeproto ASM REGFUNC2(ULONG, TextWidth,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(A0, UBYTE *, text))
{
   return TextWidthNum(rp, text, strlen(text));
}
REGFUNC_END

//makeproto ASM ULONG TextWidthNum(REG(a1) struct RastPort *rp, REG(a0) UBYTE *text, REG(d0) ULONG len)
makeproto ASM REGFUNC3(ULONG, TextWidthNum,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(A0, UBYTE *, text),
	REGPARAM(D0, ULONG, len))
{
   struct TextExtent te;
   ULONG             extent;

   /*
    * Call TextExtent to find out the text width.
    */
   TextExtent(rp, text, len, &te);

   /*
    * Figure out extent width.
    */
   extent = te.te_Extent.MaxX - te.te_Extent.MinX + 1;

   /*
    * Return which ever is bigger.. extent or te.te_Width.
    */
   return( extent /*( extent > te.te_Width ) ? extent : te.te_Width */);
}
REGFUNC_END

/*
 * Disable the given area.
 */
//makeproto ASM VOID BDisableBox(REG(a0) struct BaseInfo *bi, REG(a1) struct IBox *area)
makeproto ASM REGFUNC2(VOID, BDisableBox,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(A1, struct IBox *, area))
{
   BSetAfPt(bi, DisPat, 1);
   BSetDrMd(bi, JAM1);
   BSetDPenA(bi, SHADOWPEN);
   BBoxFillA(bi, area);
   BClearAfPt(bi);
}
REGFUNC_END

/*
 * Render a frame/separator title.
 */
makeproto VOID RenderTitle(Object *title, struct BaseInfo *bi, WORD l, WORD t, WORD w, BOOL highlight, BOOL center, UWORD place)
{
   struct IBox box;

   w -= 24;
   l += 12;
   
   if (w > 8)
   {
      DoMethod(title, TEXTM_DIMENSIONS, bi->bi_RPort, &box.Width, &box.Height);
	 
      /*
       * Figure out where to render.
       */
      if (box.Width > w) box.Width = w;

      switch (place)
      {
      case 1:
	 box.Left = l;
	 break;
      case 2:
	 box.Left = l + w - box.Width;
	 break;
      default:
	 box.Left = l + ((w - box.Width) >> 1);
	 break;
      };
      if (box.Left < l) box.Left = l;

      /*
       * Figure out y position.
       */
      if (center) box.Top = t - (box.Height >> 1);
      else        box.Top = t - bi->bi_RPort->TxBaseline;

      /*
       * Setup rastport.
       */
      BSetDPenA(bi, BACKGROUNDPEN);
      BSetDrMd(bi, JAM1);
      BClearAfPt(bi);

      /*
       * Clear text area.
       */
      BRectFill(bi, box.Left - 4, box.Top, box.Left + box.Width + 4, box.Top + box.Height);

      BSetDPenA(bi, highlight ? HIGHLIGHTTEXTPEN : TEXTPEN);
      DoMethod(title, TEXTM_RENDER, bi, &box);
   };
}

//makeproto VOID ASM SetDashedLine(REG(a0) struct BaseInfo *bi, REG(d0) UWORD offset)
makeproto ASM REGFUNC2(VOID, SetDashedLine,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, UWORD, offset))
{
   /*
    * Render a SHINE/SHADOW pen, dotted box or,
    * when the two pens are equal a SHADOW/BACKGROUND
    * pen dotted box.
    */
   BSetDPenA(bi, SHINEPEN);
   BSetDPenB(bi, SHADOWPEN);
   BSetDrMd(bi, JAM2);
   BSetDrPt(bi, 0xF0F0F0F0 >> offset);

   if (bi->bi_RPort->FgPen == bi->bi_RPort->BgPen)
      BSetDPenA(bi, BACKGROUNDPEN);
}
REGFUNC_END

/*
 * Quickly render a bevelled box.
 */
makeproto VOID RenderBevelBox(struct BaseInfo *bi, WORD l, WORD t, WORD r, WORD b, UWORD state, BOOL recessed, BOOL thin)
{
   struct RastPort *rp = bi->bi_RPort;

   /*
    * Selected or normal?
    */
   if ((state == IDS_SELECTED) || (state == IDS_INACTIVESELECTED))
   {
      recessed = !recessed;
   };
   
   /*
    * Shiny side.
    */
   BSetDPenA(bi, recessed ? SHADOWPEN : SHINEPEN);

   HLine(rp, l, t, r);
   VLine(rp, l, t, b - 1);
   if (!thin) VLine(rp, l + 1, t + 1, b - 1);

   /*
    * Shadow side.
    */
   BSetDPenA(bi, recessed ? SHINEPEN : SHADOWPEN);

   HLine(rp, l, b, r);
   VLine(rp, r, t + 1, b);
   if (!thin) VLine(rp, r - 1, t + 1, b - 1);
}

#ifdef DEBUG_BGUI
//makeproto ASM VOID SRectFillDebug(REG(a0) struct RastPort *rp, REG(d0) LONG l, REG(d1) LONG t, REG(d2) LONG r, REG(d3) LONG b,REG(a1) STRPTR file,REG(d4) ULONG line)
makeproto ASM REGFUNC7(VOID, SRectFillDebug,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(D0, LONG, l),
	REGPARAM(D1, LONG, t),
	REGPARAM(D2, LONG, r),
	REGPARAM(D3, LONG, b),
	REGPARAM(A1, STRPTR, file),
	REGPARAM(D4, ULONG, line))
#else
//ASM VOID SRectFill(REG(a0) struct RastPort *rp, REG(d0) LONG l, REG(d1) LONG t, REG(d2) LONG r, REG(d3) LONG b)
ASM REGFUNC5(VOID, SRectFill,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(D0, LONG, l),
	REGPARAM(D1, LONG, t),
	REGPARAM(D2, LONG, r),
	REGPARAM(D3, LONG, b))
#endif
{
   if ((r >= l) && (b >= t))
      RectFill(rp, l, t, r, b);
#ifdef DEBUG_BGUI
   else
	   D(bug("***Invalid RectFill (%lx,%ld,%ld,%ld,%ld) (%s,%lu)\n",rp,l,t,r,b,file ? file : (STRPTR)"Unknown file",line));
#endif
}
REGFUNC_END

/*
 * Do a safe rect-fill.
 */
#ifdef DEBUG_BGUI
  //makeproto ASM VOID BRectFillDebug(REG(a0) struct BaseInfo *bi, REG(d0) LONG l, REG(d1) LONG t, REG(d2) LONG r, REG(d3) LONG b,REG(a1) STRPTR file,REG(d4) ULONG line)
  makeproto ASM REGFUNC7(VOID, BRectFillDebug,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, LONG, l),
	REGPARAM(D1, LONG, t),
	REGPARAM(D2, LONG, r),
	REGPARAM(D3, LONG, b),
	REGPARAM(A1, STRPTR, file),
	REGPARAM(D4, ULONG, line))
  {
     SRectFillDebug(bi->bi_RPort, l, t, r, b,file,line);
  }
  REGFUNC_END
#else
  //ASM VOID BRectFill(REG(a0) struct BaseInfo *bi, REG(d0) LONG l, REG(d1) LONG t, REG(d2) LONG r, REG(d3) LONG b)
  ASM REGFUNC5(VOID, BRectFill,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, LONG, l),
	REGPARAM(D1, LONG, t),
	REGPARAM(D2, LONG, r),
	REGPARAM(D3, LONG, b))
  {
     SRectFill(bi->bi_RPort, l, t, r, b);
  }
  REGFUNC_END
#endif

//makeproto ASM VOID BRectFillA(REG(a0) struct BaseInfo *bi, REG(a1) struct Rectangle *rect)
makeproto ASM REGFUNC2(VOID, BRectFillA,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(A1, struct Rectangle *, rect))
{
   BRectFill(bi, rect->MinX, rect->MinY, rect->MaxX, rect->MaxY);
}
REGFUNC_END

//makeproto ASM VOID BBoxFill(REG(a0) struct BaseInfo *bi, REG(d0) LONG l, REG(d1) LONG t, REG(d2) LONG w, REG(d3) LONG h)
makeproto ASM REGFUNC5(VOID, BBoxFill,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, LONG, l),
	REGPARAM(D1, LONG, t),
	REGPARAM(D2, LONG, w),
	REGPARAM(D3, LONG, h))
{
   SRectFill(bi->bi_RPort, l, t, l + w - 1, t + h - 1);
}
REGFUNC_END

//makeproto ASM VOID BBoxFillA(REG(a0) struct BaseInfo *bi, REG(a1) struct IBox *box)
makeproto ASM REGFUNC2(VOID, BBoxFillA,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(A1, struct IBox *, box))
{
   BBoxFill(bi, box->Left, box->Top, box->Width, box->Height);
}
REGFUNC_END

/*
 * Background filling.
 */

//makeproto ASM VOID RenderBackFill(REG(a0) struct RastPort *rp, REG(a1) struct IBox *ib, REG(a2) UWORD *pens, REG(d0) ULONG type)
makeproto ASM REGFUNC4(VOID, RenderBackFill,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(A1, struct IBox *, ib),
	REGPARAM(A2, UWORD *, pens),
	REGPARAM(D0, ULONG, type))
{
   int apen, bpen;

   if (!pens) pens = DefDriPens;

   /*
    * Which type?
    */
   switch (type)
   {
   case  SHINE_RASTER:
      apen = SHINEPEN;
      bpen = BACKGROUNDPEN;
      break;

   case  SHADOW_RASTER:
      apen = SHADOWPEN;
      bpen = BACKGROUNDPEN;
      break;

   case  SHINE_SHADOW_RASTER:
      apen = SHINEPEN;
      bpen = SHADOWPEN;
      break;

   case  FILL_RASTER:
      apen = FILLPEN;
      bpen = BACKGROUNDPEN;
      break;

   case  SHINE_FILL_RASTER:
      apen = SHINEPEN;
      bpen = FILLPEN;
      break;

   case  SHADOW_FILL_RASTER:
      apen = SHADOWPEN;
      bpen = FILLPEN;
      break;

   case  SHINE_BLOCK:
      apen = SHINEPEN;
      bpen = apen;
      break;

   case  SHADOW_BLOCK:
      apen = SHADOWPEN;
      bpen = apen;
      break;

   default:
      apen = BACKGROUNDPEN;
      bpen = apen;
      break;
   };
   RenderBackFillRaster(rp, ib, pens[apen], pens[bpen]);
}
REGFUNC_END

#ifdef DEBUG_BGUI
//makeproto ASM VOID RenderBackFillRasterDebug(REG(a0) struct RastPort *rp, REG(a1) struct IBox *ib, REG(d0) UWORD apen, REG(d1) UWORD bpen,REG(a2) STRPTR file, REG(d2) ULONG line)
makeproto ASM REGFUNC6(VOID, RenderBackFillRasterDebug,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(A1, struct IBox *, ib),
	REGPARAM(D0, UWORD, apen),
	REGPARAM(D1, UWORD, bpen),
	REGPARAM(A2, STRPTR, file),
	REGPARAM(D2, ULONG, line))
#else
//ASM VOID RenderBackFillRaster(REG(a0) struct RastPort *rp, REG(a1) struct IBox *ib, REG(d0) UWORD apen, REG(d1) UWORD bpen)
ASM REGFUNC4(VOID, RenderBackFillRaster,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(A1, struct IBox *, ib),
	REGPARAM(D0, UWORD, apen),
	REGPARAM(D1, UWORD, bpen))
#endif
{
   static UWORD pat[] = { 0x5555, 0xAAAA };
   /*
    * Setup RastPort.
    */

   FSetABPenDrMd(rp, apen, bpen, JAM2);

   if (apen == bpen)
   {
      FSetDrMd(rp, JAM1);
      FClearAfPt(rp);
   }
   else
   {
      SetAfPt(rp, pat, 1);
   };

   /*
    * Render...
    */
#ifdef DEBUG_BGUI
   SRectFillDebug(rp, ib->Left, ib->Top, ib->Left + ib->Width - 1, ib->Top + ib->Height - 1,file,line);
#else
   SRectFill(rp, ib->Left, ib->Top, ib->Left + ib->Width - 1, ib->Top + ib->Height - 1);
#endif

   /*
    * Clear area pattern.
    */
   FClearAfPt(rp);
}
REGFUNC_END

/*
 * Draw a dotted box.
 */
//makeproto ASM VOID DottedBox(REG(a0) struct BaseInfo *bi, REG(a1) struct IBox *ibx)
makeproto ASM REGFUNC2(VOID, DottedBox,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(A1, struct IBox *, ibx))
{
   int x1 = ibx->Left;
   int x2 = ibx->Left + ibx->Width - 1;
   int y1 = ibx->Top;
   int y2 = ibx->Top + ibx->Height - 1;
   
   ULONG secs, micros, hundredths;

   struct RastPort *rp = bi->bi_RPort;
   
   /*
    * We clear any thick framing which may be
    * there or not.
    */
   BSetDPenA(bi, BACKGROUNDPEN);
   Move(rp, x1 + 1, y1 + 1);
   Draw(rp, x1 + 1, y2 - 1);
   Draw(rp, x2 - 1, y2 - 1);
   Draw(rp, x2 - 1, y1 + 1);
   Draw(rp, x1 + 2, y1 + 1);

   CurrentTime(&secs, &micros);
   hundredths = ((secs & 0xFFFFFF) * 100) + (micros / 10000);
   SetDashedLine(bi, (hundredths / 5) % 8);

   /*
    * Draw the box.
    */
   Move(rp, x1, y1);
   Draw(rp, x2, y1);
   Draw(rp, x2, y2);
   Draw(rp, x1, y2);
   Draw(rp, x1, y1 + 1);
}
REGFUNC_END

/*
 * Find out rendering state.
 */
//makeproto ASM ULONG GadgetState(REG(a0) struct BaseInfo *bi, REG(a1) Object *obj, REG(d0) BOOL norec)
makeproto ASM REGFUNC3(ULONG, GadgetState,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(A1, Object *, obj),
	REGPARAM(D0, BOOL, norec))
{
   BOOL active = !(GADGET(obj)->Activation & BORDERMASK) || (bi->bi_IWindow->Flags & WFLG_WINDOWACTIVE);
   BOOL normal = !(GADGET(obj)->Flags & GFLG_SELECTED) || norec;
   
   return active ? (ULONG)(normal ? IDS_NORMAL         : IDS_SELECTED)
		 : (ULONG)(normal ? IDS_INACTIVENORMAL : IDS_INACTIVESELECTED);
}
REGFUNC_END

#ifdef __AROS__
makearosproto
AROS_LH6(VOID, BGUI_FillRectPattern,
    AROS_LHA(struct RastPort *, r, A1),
    AROS_LHA(struct bguiPattern *, bp, A0),
    AROS_LHA(ULONG, x1, D0),
    AROS_LHA(ULONG, y1, D1),
    AROS_LHA(ULONG, x2, D2),
    AROS_LHA(ULONG, y2, D3),
    struct Library *, BGUIBase, 22, BGUI)
#else
makeproto SAVEDS ASM VOID BGUI_FillRectPattern(REG(a1) struct RastPort *r, REG(a0) struct bguiPattern *bp,
   REG(d0) ULONG x1, REG(d1) ULONG y1, REG(d2) ULONG x2, REG(d3) ULONG y2)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   int i, j;

   int x0 = bp->bp_Left;
   int y0 = bp->bp_Top;
   int w  = bp->bp_Width;
   int h  = bp->bp_Height;
   
   int col_min, col_max;
   int row_min, row_max;

   int from_x, to_x1, to_x2, to_w;
   int from_y, to_y1, to_y2, to_h;

   struct BitMap *bm = bp->bp_BitMap;
   
   if (bm && w && h)
   {
      col_min = x1 / w;
      row_min = y1 / h;
      col_max = x2++ / w + 1;
      row_max = y2++ / h + 1;
   
      for (i = col_min; i <= col_max; i++)
      {
	 to_x1 = i * w;
	 to_x2 = to_x1 + w;
      
	 if (to_x1 < x1)
	 {
	    from_x = x0 + x1 - to_x1;
	    to_x1  = x1;
	 }
	 else
	 {
	    from_x = x0;
	 };
	 if (to_x2 > x2)
	 {
	    to_x2 = x2;
	 };
	 if ((to_w = to_x2 - to_x1) > 0)
	 {
	    for (j = row_min; j <= row_max; j++)
	    {
	       to_y1 = j * h;
	       to_y2 = to_y1 + h;      
   
	       if (to_y1 < y1)
	       {
		  from_y = y0 + y1 - to_y1;
		  to_y1  = y1;
	       }
	       else
	       {
		  from_y = y0;
	       }
	       if (to_y2 > y2)
	       {
		  to_y2 = y2;
	       };
	       if ((to_h = to_y2 - to_y1) > 0)
	       {
		  BltBitMapRastPort(bm, from_x, from_y, r, to_x1, to_y1, to_w, to_h, 0xC0);
	       };
	    };
	 };
      };
   };

   AROS_LIBFUNC_EXIT
}

//makeproto VOID ASM HLine(REG(a1) struct RastPort *rp, REG(d0) UWORD l, REG(d1) UWORD t, REG(d2) UWORD r)
makeproto ASM REGFUNC4(VOID, HLine,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(D0, UWORD, l),
	REGPARAM(D1, UWORD, t),
	REGPARAM(D2, UWORD, r))
{
   Move(rp, l, t);
   Draw(rp, r, t);
}
REGFUNC_END

//makeproto VOID ASM VLine(REG(a1) struct RastPort *rp, REG(d0) UWORD l, REG(d1) UWORD t, REG(d2) UWORD b)
makeproto ASM REGFUNC4(VOID, VLine,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(D0, UWORD, l),
	REGPARAM(D1, UWORD, t),
	REGPARAM(D2, UWORD, b))
{
   Move(rp, l, t);
   Draw(rp, l, b);
}
REGFUNC_END

//makeproto ASM ULONG FGetAPen(REG(a1) struct RastPort *rp)
makeproto ASM REGFUNC1(ULONG, FGetAPen,
	REGPARAM(A1, struct RastPort *,rp))
{
   #ifdef ENHANCED
   return GetAPen(rp);
   #else
   if (OS30) return GetAPen(rp);
   return rp->FgPen;
   #endif
}
REGFUNC_END

//makeproto ASM ULONG FGetBPen(REG(a1) struct RastPort *rp)
makeproto ASM REGFUNC1(ULONG, FGetBPen,
	REGPARAM(A1, struct RastPort *,rp))
{
   #ifdef ENHANCED
   return GetBPen(rp);
   #else
   if (OS30) return GetBPen(rp);
   return rp->BgPen;
   #endif
}
REGFUNC_END

//makeproto ASM ULONG FGetDrMd(REG(a1) struct RastPort *rp)
makeproto ASM REGFUNC1(ULONG, FGetDrMd,
	REGPARAM(A1, struct RastPort *,rp))
{
   #ifdef ENHANCED
   return GetDrMd(rp);
   #else
   if (OS30) return GetDrMd(rp);
   return rp->DrawMode;
   #endif
}
REGFUNC_END

//makeproto ASM ULONG FGetDepth(REG(a1) struct RastPort *rp)
makeproto ASM REGFUNC1(ULONG, FGetDepth,
	REGPARAM(A1, struct RastPort *, rp))
{
   #ifdef ENHANCED
   return GetBitMapAttr(rp->BitMap, BMA_DEPTH);
   #else
   if (OS30) return GetBitMapAttr(rp->BitMap, BMA_DEPTH);
   return rp->BitMap->Depth;
   #endif
}
REGFUNC_END

//makeproto ASM VOID FSetAPen(REG(a1) struct RastPort *rp, REG(d0) ULONG pen)
makeproto ASM REGFUNC2(VOID, FSetAPen,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(D0, ULONG, pen))
{
   #ifdef ENHANCED
   SetRPAttrs(rp, RPTAG_APen, pen, TAG_END);
   #else
   if (OS30) SetRPAttrs(rp, RPTAG_APen, pen, TAG_END);
   else if (rp->FgPen != pen) SetAPen(rp, pen);
   #endif
}
REGFUNC_END

//makeproto ASM VOID FSetBPen(REG(a1) struct RastPort *rp, REG(d0) ULONG pen)
makeproto ASM REGFUNC2(VOID, FSetBPen,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(D0, ULONG, pen))
{
   #ifdef ENHANCED
   SetRPAttrs(rp, RPTAG_BPen, pen, TAG_END);
   #else
   if (OS30) SetRPAttrs(rp, RPTAG_BPen, pen, TAG_END);
   else if (rp->BgPen != pen) SetBPen(rp, pen);
   #endif
}
REGFUNC_END

//makeproto ASM VOID FSetDrMd(REG(a1) struct RastPort *rp, REG(d0) ULONG drmd)
makeproto ASM REGFUNC2(VOID, FSetDrMd,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(D0, ULONG, drmd))
{
   #ifdef ENHANCED
   SetRPAttrs(rp, RPTAG_DrMd, drmd, TAG_END);
   #else
   if (OS30) SetRPAttrs(rp, RPTAG_DrMd, drmd, TAG_END);
   else if (rp->DrawMode != drmd) SetDrMd(rp, drmd);
   #endif
}
REGFUNC_END

//makeproto ASM VOID FSetABPenDrMd(REG(a1) struct RastPort *rp, REG(d0) ULONG apen, REG(d1) ULONG bpen, REG(d2) ULONG mode)
makeproto ASM REGFUNC4(VOID, FSetABPenDrMd,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(D0, ULONG, apen),
	REGPARAM(D1, ULONG, bpen),
	REGPARAM(D2, ULONG, mode))
{
   #ifdef ENHANCED
   SetABPenDrMd(rp, apen, bpen, mode);
   #else
   if (OS30) SetABPenDrMd(rp, apen, bpen, mode);
   else
   {
      SetAPen(rp, apen);
      SetBPen(rp, bpen);
      SetDrMd(rp, mode);
   };
   #endif
}
REGFUNC_END

//makeproto ASM VOID FSetFont(REG(a1) struct RastPort *rp, REG(a0) struct TextFont *tf)
makeproto ASM REGFUNC2(VOID, FSetFont,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(A0, struct TextFont *, tf))
{
   #ifdef ENHANCED
   SetRPAttrs(rp, RPTAG_Font, tf, TAG_END);
   #else
   if (OS30) SetRPAttrs(rp, RPTAG_Font, tf, TAG_END);
   else
   {
      if (rp->Font != tf) SetFont(rp, tf);
   }
   #endif
}
REGFUNC_END

//makeproto ASM VOID FSetFontStyle(REG(a1) struct RastPort *rp, REG(d0) ULONG style)
makeproto ASM REGFUNC2(VOID, FSetFontStyle,
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(D0, ULONG, style))
{
   SetSoftStyle(rp, style, AskSoftStyle(rp));
}
REGFUNC_END

//makeproto ASM VOID FClearAfPt(REG(a1) struct RastPort *rp)
makeproto ASM REGFUNC1(VOID, FClearAfPt,
	REGPARAM(A1, struct RastPort *, rp))
{
   SetAfPt(rp, NULL, 0);
}
REGFUNC_END




//makeproto ASM VOID BSetDPenA(REG(a0) struct BaseInfo *bi, REG(d0) LONG pen)
makeproto ASM REGFUNC2(VOID, BSetDPenA,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, LONG, pen))
{
   FSetAPen(bi->bi_RPort, bi->bi_Pens[pen]);
}
REGFUNC_END

//makeproto ASM VOID BSetPenA(REG(a0) struct BaseInfo *bi, REG(d0) ULONG pen)
makeproto ASM REGFUNC2(VOID, BSetPenA,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, ULONG, pen))
{
   FSetAPen(bi->bi_RPort, pen);
}
REGFUNC_END

//makeproto ASM VOID BSetDPenB(REG(a0) struct BaseInfo *bi, REG(d0) LONG pen)
makeproto ASM REGFUNC2(VOID, BSetDPenB,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, LONG, pen))
{
   FSetBPen(bi->bi_RPort, bi->bi_Pens[pen]);
}
REGFUNC_END

//makeproto ASM VOID BSetPenB(REG(a0) struct BaseInfo *bi, REG(d0) ULONG pen)
makeproto ASM REGFUNC2(VOID, BSetPenB,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, ULONG, pen))
{
   FSetBPen(bi->bi_RPort, pen);
}
REGFUNC_END

//makeproto ASM VOID BSetDrMd(REG(a0) struct BaseInfo *bi, REG(d0) ULONG drmd)
makeproto ASM REGFUNC2(VOID, BSetDrMd,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, ULONG, drmd))
{
   FSetDrMd(bi->bi_RPort, drmd);
}
REGFUNC_END

//makeproto ASM VOID BSetFont(REG(a0) struct BaseInfo *bi, REG(a1) struct TextFont *tf)
makeproto ASM REGFUNC2(VOID, BSetFont,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(A1, struct TextFont *, tf))
{
   FSetFont(bi->bi_RPort, tf);
}
REGFUNC_END

//makeproto ASM VOID BSetFontStyle(REG(a0) struct BaseInfo *bi, REG(d0) ULONG style)
makeproto ASM REGFUNC2(VOID, BSetFontStyle,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, ULONG, style))
{
   SetSoftStyle(bi->bi_RPort, style, AskSoftStyle(bi->bi_RPort));
}
REGFUNC_END

//makeproto ASM VOID BSetAfPt(REG(a0) struct BaseInfo *bi, REG(a1) UWORD *pat, REG(d0) ULONG size)
makeproto ASM REGFUNC3(VOID, BSetAfPt,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(A1, UWORD *, pat),
	REGPARAM(D0, ULONG, size))
{
   SetAfPt(bi->bi_RPort, pat, size);
}
REGFUNC_END

//makeproto ASM VOID BClearAfPt(REG(a0) struct BaseInfo *bi)
makeproto ASM REGFUNC1(VOID, BClearAfPt,
	REGPARAM(A0, struct BaseInfo *, bi))
{
   SetAfPt(bi->bi_RPort, NULL, 0);
}
REGFUNC_END

//makeproto ASM VOID BSetDrPt(REG(a0) struct BaseInfo *bi, REG(d0) ULONG pat)
makeproto ASM REGFUNC2(VOID, BSetDrPt,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(D0, ULONG, pat))
{
   SetDrPt(bi->bi_RPort, pat & 0xFFFF);
}
REGFUNC_END

//makeproto ASM VOID BDrawImageState(REG(a0) struct BaseInfo *bi, REG(a1) Object *image,
//   REG(d0) ULONG x, REG(d1) ULONG y, REG(d2) ULONG state)
makeproto ASM REGFUNC5(VOID, BDrawImageState,
	REGPARAM(A0, struct BaseInfo *, bi),
	REGPARAM(A1, Object *, image),
	REGPARAM(D0, ULONG, x),
	REGPARAM(D1, ULONG, y),
	REGPARAM(D2, ULONG, state))
{
   //tprintf("%08lx %08lx %ld %ld %04lx %08lx\n", /4*
   DrawImageState(bi->bi_RPort, IMAGE(image), x, y, state, bi->bi_DrInfo);
}
REGFUNC_END
