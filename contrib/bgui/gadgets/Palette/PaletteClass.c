/*
 * @(#) $Header$
 *
 * PaletteClass.c
 *
 * (C) Copyright 1999 BGUI Developement team.
 * (C) Copyright 1996 Ian J. Einman.
 * (C) Copyright 1995-1996 Jaba Development.
 * (C) Copyright 1995-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.8  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.7  2003/01/18 19:10:18  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.6  2000/08/11 08:05:01  chodorowski
 * Fixed an unterminated comment, and some other stuff to build properly.
 *
 * Revision 42.5  2000/08/09 10:17:25  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.4  2000/08/08 13:51:35  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.3  2000/07/11 17:17:24  stegerg
 * struct Library *BGUIBase, not struct BGUIBase *BGUIBase.
 * fixed a "comparison always 0 due to limited ..." bug.
 *
 * Revision 42.2  2000/07/09 03:05:09  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:29:07  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:21:13  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:00  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:01  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.4  2000/05/04 04:49:17  mlemos
 * Added a missing decrement of the number of created instances when OM_NEW
 * fails.
 *
 * Revision 1.1.2.3  1999/07/23 21:32:05  mlemos
 * Fixed the coordinates of the drag token object.
 *
 * Revision 1.1.2.2  1999/07/23 20:53:20  mlemos
 * Made the rendering of the drag object be done using DrawImageState.
 *
 * Revision 1.1.2.1  1999/05/31 01:42:25  mlemos
 * Ian sources.
 *
 *
 *
 */

#include <exec/types.h>
#include <exec/libraries.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <libraries/bgui.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/bgui.h>
#include <proto/utility.h>
#include <proto/layers.h>
//#include <proto/rexxsyslib.h>
#include <proto/dos.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef BGUI_COMPILERSPECIFIC_H
#include <bgui/bgui_compilerspecific.h>
#endif

#ifdef __AROS__
extern struct Library * BGUIBase;
#endif


/*
 * Clamp a value in a range.
 */
#define CLAMP(v,i,a) if ((v) < (i)) (v) = (i); else if ((v) > (a)) (v) = (a);

/*
 * Simple type-cast.
 */
#define GADGET(x) ((struct Gadget *)x)

/*
** OS macros.
**/
#define OS30      (((struct Library * )SysBase)->lib_Version >= 39)
#define OS20      (((struct Library * )SysBase)->lib_Version <  39)

extern UBYTE _LibName[]   = "bgui_palette.gadget";
extern UBYTE _LibID[]     = "\0$VER: bgui_palette.gadget 41.10 (29.5.99) ©1996 Ian J. Einman ©1999 BGUI Developers Team";
extern UWORD _LibVersion  = 41;
extern UWORD _LibRevision = 10;

static ULONG instances=0;

/*
 * PaletteClass object instance data.
 */
typedef struct {
   UWORD       pd_ColorOffset;   /* Offset in palette/table.          */
   UWORD       pd_Depth;         /* Palette depth.                    */
   UWORD       pd_NumColors;     /* Number of colors.                 */
   UWORD      *pd_PenTable;      /* Table of pens.                    */
   UWORD       pd_CurrentColor;  /* Currently selected color.         */
   Object     *pd_Frame;         /* Frame for selected color.         */
   UWORD       pd_Columns;       /* Number of columns.                */
   struct IBox pd_ColorBox;      /* Bounds of real color box.         */
   UWORD       pd_RectWidth;     /* Width of color rectangle.         */
   UWORD       pd_RectHeight;    /* Height of color rectangle.        */
   UWORD       pd_InitialColor;  /* Color selected when going active. */
   BOOL        pd_ResetInitial;  /* Reset initial color?              */
   BOOL        pd_Dragging;      /* Dragging?                         */
}  PD;

/*
 * Make sure that "CurrentColor" is a valid
 * pen number in the colors which are displayed
 * in the palette object.
 */
//STATIC ASM UWORD ValidateColor( REG(a0) PD *pd, REG(d0) ULONG pen )
STATIC ASM REGFUNC2(UWORD, ValidateColor,
	REGPARAM(A0, PD *, pd),
	REGPARAM(D0, ULONG, pen))
{
   UWORD       *tab = pd->pd_PenTable, i;

   /*
    * Do we have a pen table?
    */
   if ( tab ) {
      /*
       * Check the pens in the table.
       */
      for ( i = 0; i < pd->pd_NumColors; i++ ) {
	 /*
	  * Does the pen exist in the
	  * pen table?
	  */
	 if ( pen == tab[ i + pd->pd_ColorOffset ] )
	    return(( UWORD )pen );
      }
      /*
       * The pen does not exist in the table.
       * Make it the first pen in the table.
       */
      i = tab[ pd->pd_ColorOffset ];
   } else {
      /*
       * Check if the pen number is
       * in range.
       */
      if ( pen >= pd->pd_ColorOffset && pen <= ( pd->pd_ColorOffset + pd->pd_NumColors - 1 ))
	 return(( UWORD )pen );
      /*
       * No. Default to the first pen.
       */
      i = pd->pd_ColorOffset;
   }

   return( i );
}
REGFUNC_END

/*
 * Create a new palette object.
 */
METHOD(PaletteClassNew, struct opSet *,ops)
{
   PD             *pd;
   struct TagItem *tstate = ops->ops_AttrList, *tag;
   Object         *label;
   ULONG           rc, place, data;

   if(BGUIBase==NULL
   && (BGUIBase=OpenLibrary("bgui.library",41))==NULL)
      return(NULL);

   /*
    * Let the superclass make the object.
    */
   if (rc = DoSuperMethodA(cl, obj, (Msg)ops))
   {
      instances++;

      /*
       * Get a pointer to the object
       * it's instance data.
       */
      pd = INST_DATA(cl, rc);

      /*
       * Setup the default settings.
       */
      pd->pd_Depth     = 1;
      pd->pd_NumColors = 2;

      /*
       * Setup the instance data.
       */
      while (tag = NextTagItem(&tstate))
      {
	 data = tag->ti_Data;
	 switch (tag->ti_Tag)
	 {
	 case PALETTE_Depth:
	    CLAMP(data, 1, 8);
	    pd->pd_Depth     = data;
	    pd->pd_NumColors = 1 << data;
	    break;

	 case PALETTE_ColorOffset:
	    pd->pd_ColorOffset = data;
	    break;

	 case PALETTE_PenTable:
	    pd->pd_PenTable = (UWORD *)data;
	    break;

	 case PALETTE_CurrentColor:
	    pd->pd_CurrentColor = data;
	    break;
	 };
      };

      /*
       * Make sure the offset stays in range.
       */
       
      /* AROS FIX: (WORD) typecast, otherwise gcc error "comparison always 0, due to limited datatype..." */
      #ifdef __AROS__
      CLAMP((WORD)pd->pd_ColorOffset, 0, 256 - (WORD)pd->pd_NumColors);
      #else
      CLAMP( pd->pd_ColorOffset, 0, 256 - pd->pd_NumColors );
      #endif

      /*
       * The color must remain OK.
       */
      pd->pd_CurrentColor = ValidateColor(pd, pd->pd_CurrentColor);

      /*
       * See if the object has a label
       * attached to it.
       */
      DoMethod((Object *)rc, OM_GET, BT_LabelObject, &label);
      if (label)
      {
	 /*
	  * Yes. Query the place because it may
	  * not be PLACE_IN for obvious reasons.
	  */
	 DoMethod(label, OM_GET, LAB_Place, &place);
	 if (place == PLACE_IN)
	    SetAttrs(label, LAB_Place, PLACE_LEFT, TAG_END);
      }

      /*
       * Create a frame for the
       * currently selected pen.
       */
      if (pd->pd_Frame = BGUI_NewObject(BGUI_FRAME_IMAGE, FRM_Type, FRTYPE_BUTTON, FRM_Flags, FRF_RECESSED, TAG_END))
	 return rc;

      /*
       * No frame means no object.
       */
      CoerceMethod(cl, (Object *)rc, OM_DISPOSE);
      instances--;
   }
   if(instances==0
   && BGUIBase)
   {
      CloseLibrary(BGUIBase);
      BGUIBase=NULL;
   }
   return 0;
}
METHOD_END

/*
 * Dispose of the object.
 */
METHOD(PaletteClassDispose, Msg,msg)
{
   PD       *pd = INST_DATA(cl, obj);
   ULONG rc;

   /*
    * Dispose of the frame.
    */
   if (pd->pd_Frame) DoMethod(pd->pd_Frame, OM_DISPOSE);

   /*
    * The superclass handles
    * the rest.
    */
   rc=DoSuperMethodA(cl, obj, msg);
   if(--instances==0)
   {
      CloseLibrary(BGUIBase);
      BGUIBase=NULL;
   }
   return rc;
}
METHOD_END

/*
 * Get an attribute.
 */
METHOD(PaletteClassGet, struct opGet *,opg)
{
   PD         *pd = INST_DATA( cl, obj );
   ULONG       rc = 1;

   switch (opg->opg_AttrID)
   {
   case PALETTE_CurrentColor:
      /*
       * Pass on the currently selected color.
       */
      *(opg->opg_Storage) = pd->pd_CurrentColor;
      break;
   case BT_PostRenderHighestClass:
      /*
       * Let BGUI know we support this.
       */
      *(opg->opg_Storage) = (ULONG)cl;
      break;
   default:
      /*
       * Everything else goes
       * to the superclass.
       */
      rc = DoSuperMethodA(cl, obj, (Msg)opg);
      break;
   };
   return rc;
}
METHOD_END

/*
 * Render the color rectangles
 * inside the palette object it's
 * color box.
 */
//STATIC ASM VOID RenderColorRects( REG(a0) PD *pd, REG(a1) struct RastPort *rp, REG(a2) struct IBox *area, REG(a3) struct DrawInfo *dri )
STATIC ASM REGFUNC4(VOID, RenderColorRects,
	REGPARAM(A0, PD *, pd),
	REGPARAM(A1, struct RastPort *, rp),
	REGPARAM(A2, struct IBox *, area),
	REGPARAM(A3, struct DrawInfo *, dri))
{
   UWORD    colorwidth, colorheight, columns = 1, rows = 1, depth = pd->pd_Depth;
   UWORD    hadjust, vadjust, left, top, colsize, rowsize, c, r, color;
   UWORD    offset = pd->pd_ColorOffset;

   /*
    * Get the first color of the
    * displayed palette.
    */
   if ( pd->pd_PenTable ) color = pd->pd_PenTable[ offset ];
   else            color = offset;

   /*
    * Get the Width and height of the
    * area in which we layout the
    * color rectangles.
    */
   colorwidth  = area->Width;
   colorheight = area->Height;

   /*
    * Layout the color
    * rectangles.
    */
   while ( depth ) {
      if (( colorheight << 1 ) > colorwidth ) {
	 colorheight >>= 1;
	 rows      <<= 1;
      } else {
	 colorwidth  >>= 1;
	 columns     <<= 1;
      }
      depth--;
   }

   /*
    * Compute the pixel width and
    * height of the color rectangles.
    */
   colsize = area->Width  / columns;
   rowsize = area->Height / rows;

   /*
    * Since this is not precise
    * we compute the space left
    * over to adjust the left
    * and top offset at which we
    * begin.
    *
    * Also we re-compute the color
    * box dimensions.
    */
   hadjust = ( area->Width  - ( area->Width  = colsize * columns )) >> 1;
   vadjust = ( area->Height - ( area->Height = rowsize * rows    )) >> 1;

   /*
    * Adjust the colorbox position.
    */
   area->Left += hadjust;
   area->Top  += vadjust;

   /*
    * Get initial left and top offset.
    */
   left = area->Left + 1;
   top  = area->Top  + 1;

   /*
    * No patterns!
    */
   SetAfPt( rp, NULL, 0 );

   /*
    * Set these up. We need this information to
    * compute the color the mouse is over.
    */
   pd->pd_Columns   = columns;
   pd->pd_RectWidth  = colsize;
   pd->pd_RectHeight = rowsize;

   /*
    * Now render the rectangles.
    */
   for (r = 0; r < rows; r++)
   {
      for (c = 0; c < columns; c++)
      {
	 /*
	  * The currently selected color
	  * is done with a frameclass object.
	  */
	 if (color == pd->pd_CurrentColor)
	 {
	    /*
	     * Setup the object.
	     */
	    SetAttrs( pd->pd_Frame, IA_Left, left,
		     IA_Top,         top,
		     IA_Width,   colsize - 1,
		     IA_Height,  rowsize - 1,
		     FRM_BackPen,   color,
		     TAG_END );
	    /*
	     * Render it.
	     */
	    DrawImageState( rp, ( struct Image * )pd->pd_Frame, 0, 0, IDS_NORMAL, dri );
	 }
	 else
	 {
	    /*
	     * Other colors we do ourselves.
	     */
	    SetAPen( rp, color );
	    RectFill( rp, left, top, left + colsize - 2, top + rowsize - 2 );
	 }
	 left += colsize;
	 /*
	  * Get the next color from the
	  * displayed palette.
	  */
	 if ( pd->pd_PenTable ) color = pd->pd_PenTable[ ++offset ];
	 else            color++;
      }
      left = area->Left + 1;
      top += rowsize;
   }
}
REGFUNC_END

/*
 * Render the palette object.
 */
METHOD(PaletteClassRender, struct gpRender *,gpr)
{
   PD                *pd = INST_DATA(cl, obj);
   struct RastPort    rp = *gpr->gpr_RPort;
   struct DrawInfo   *dri = gpr->gpr_GInfo->gi_DrInfo;
   struct IBox       *bounds;
   Object            *frame;
   ULONG              fw = 0, fh = 0;
   static UWORD       dispat[2] = { 0x2222, 0x8888 };
   ULONG              rc;

   /*
    * First we let the superclass
    * render. If it returns 0 we
    * do not render!
    */
   if (rc = DoSuperMethodA(cl, obj, (Msg)gpr))
   {
      /*
       * Get the hitbox bounds of the object
       * and copy it's contents. We need to
       * copy the data because we must adjust
       * it's contents.
       */
      DoMethod( obj, OM_GET, BT_HitBox, &bounds );
      pd->pd_ColorBox = *bounds;

      /*
       * Do we have a frame?
       */
      DoMethod( obj, OM_GET, BT_FrameObject, &frame );
      if ( frame ) {
	 /*
	  * Find out the frame thickness.
	  */
	 DoMethod( frame, OM_GET, FRM_FrameWidth,  &fw );
	 DoMethod( frame, OM_GET, FRM_FrameHeight, &fh );
	 fw++;
	 fh++;

	 /*
	  * Adjust bounds accoordingly.
	  */
	 pd->pd_ColorBox.Left   += fw;
	 pd->pd_ColorBox.Top    += fh;
	 pd->pd_ColorBox.Width     -= fw << 1;
	 pd->pd_ColorBox.Height    -= fh << 1;
      }

      /*
       * Render the color rectangles.
       */
      RenderColorRects( pd, &rp, &pd->pd_ColorBox, dri );

      /*
       * Disabled?
       */
      if ( GADGET(obj)->Flags & GFLG_DISABLED ) {
	 SetAPen( &rp, dri ? dri->dri_Pens[ SHADOWPEN ] : 2 );
	 SetDrMd( &rp, JAM1 );
	 SetAfPt( &rp, dispat, 1 );
	 RectFill( &rp, bounds->Left,
		   bounds->Top,
		   bounds->Left + bounds->Width  - 1,
		   bounds->Top  + bounds->Height - 1 );
      }

   }
   BGUI_PostRender(cl, obj, gpr);

   return rc;
}
METHOD_END

/*
 * Get the pen number of
 * ordinal color number "num".
 */
//STATIC ASM ULONG GetPenNumber( REG(a0) PD *pd, REG(d0) ULONG num )
STATIC ASM REGFUNC2(ULONG, GetPenNumber,
	REGPARAM(A0, PD *, pd),
	REGPARAM(D0, ULONG, num))
{
   /*
    * Return the pen number
    * from the pen table or...
    */
   if ( pd->pd_PenTable ) return( pd->pd_PenTable[ pd->pd_ColorOffset + num ] );

   /*
    * From the screen palette.
    */
   return( num + pd->pd_ColorOffset );
}
REGFUNC_END

/*
 * Determine the ordinal number
 * of the pen in the object.
 */
//STATIC ASM ULONG GetOrdinalNumber( REG(a0) PD *pd, REG(d0) ULONG pen )
STATIC ASM REGFUNC2(ULONG, GetOrdinalNumber,
	REGPARAM(A0, PD *, pd),
	REGPARAM(D0, ULONG, pen))
{
   UWORD       *tab = pd->pd_PenTable, i;

   /*
    * Do we have a pen table?
    */
   if ( tab ) {
      /*
       * Look up the pen in the table.
       */
      for ( i = 0; i < pd->pd_NumColors; i++ ) {
	 /*
	  * Is this the one?
	  */
	 if ( tab[ i + pd->pd_ColorOffset ] == pen )
	    return( i );
      }
   }

   /*
    * Return the ordinal palette pen.
    */
   return( pen - pd->pd_ColorOffset );
}
REGFUNC_END

/*
 * Determine which color rectangle
 * the coordinates are in.
 */
//STATIC ASM UWORD GetColor( REG(a0) PD *pd, REG(d0) ULONG x, REG(d1) ULONG y )
STATIC ASM REGFUNC3(UWORD, GetColor,
	REGPARAM(A0, PD *, pd),
	REGPARAM(D0, ULONG, x),
	REGPARAM(D1, ULONG, y))
{
   UWORD    col, row;

   /*
    * Compute the row and column
    * we clicked on.
    */
   row = y / pd->pd_RectHeight;
   col = x / pd->pd_RectWidth;

   /*
    * With this information we can simple
    * compute and return the color under
    * these coordinates.
    */
   return(( UWORD )GetPenNumber( pd, ( row * pd->pd_Columns ) + col ));
}
REGFUNC_END

/*
 * Get the top-left position of
 * a color in the color box.
 */
//STATIC ASM VOID GetTopLeft( REG(a0) PD *pd, REG(d0) UWORD color, REG(a1) UWORD *x, REG(a2) UWORD *y )
STATIC ASM REGFUNC4(VOID, GetTopLeft,
	REGPARAM(A0, PD *, pd),
	REGPARAM(D0, UWORD, color),
	REGPARAM(A1, UWORD *, x),
	REGPARAM(A2, UWORD *, y))
{
   UWORD       row, col;

   /*
    * First compute the row and column
    * of the color.
    */
   row = color / pd->pd_Columns;
   col = color - ( row * pd->pd_Columns );

   /*
    * Now we can simply get the
    * x/y position of the color
    * box.
    */
   *x = pd->pd_ColorBox.Left + ( col * pd->pd_RectWidth  );
   *y = pd->pd_ColorBox.Top  + ( row * pd->pd_RectHeight );
}
REGFUNC_END

/*
 * Notify about an attribute change.
 */
STATIC ULONG NotifyAttrChange( Object *obj, struct GadgetInfo *gi, ULONG flags, Tag tag1, ... )
{
   return( DoMethod( obj, OM_NOTIFY, &tag1, gi, flags ));
}

/*
 * Change the currently selected color.
 */
//STATIC ASM VOID ChangeSelectedColor( REG(a0) PD *pd, REG(a1) struct GadgetInfo *gi, REG(d0) ULONG newcolor )
STATIC ASM REGFUNC3(VOID, ChangeSelectedColor,
	REGPARAM(A0, PD *, pd),
	REGPARAM(A1, struct GadgetInfo *, gi),
	REGPARAM(D0, ULONG, newcolor))
{
   struct RastPort         *rp;
   UWORD        l, t;

   /*
    * Allocate a rastport.
    */
   if ( gi && ( rp = ObtainGIRPort( gi ))) {
      /*
       * First pickup the coordinates
       * of the currently selected
       * color.
       */
      GetTopLeft( pd, GetOrdinalNumber( pd, pd->pd_CurrentColor ), &l, &t );

      /*
       * Render this color rectangle
       * as not-selected.
       */
      SetAPen( rp, pd->pd_CurrentColor );
      RectFill( rp, l + 1,
	       t + 1,
	       l + pd->pd_RectWidth  - 1,
	       t + pd->pd_RectHeight - 1 );

      /*
       * Now pickup the coordinates
       * of the new selected color.
       */
      GetTopLeft( pd, GetOrdinalNumber( pd, newcolor ), &l, &t );

      /*
       * Setup and render the frame to
       * reflect the new data.
       */
      SetAttrs( pd->pd_Frame, IA_Left,     l + 1,
	       IA_Top,      t + 1,
	       IA_Width,    pd->pd_RectWidth  - 1,
	       IA_Height,   pd->pd_RectHeight - 1,
	       FRM_BackPen, newcolor,
	       TAG_END );

      DrawImageState( rp, ( struct Image * )pd->pd_Frame, 0, 0, IDS_NORMAL, gi->gi_DrInfo );

      /*
       * Free up the rastport.
       */
      ReleaseGIRPort( rp );
   }
   /*
    * Setup the new color.
    */
   pd->pd_CurrentColor = newcolor;
}
REGFUNC_END

/*
 * Set attributes.
 */
METHOD(PaletteClassSet, struct opUpdate *,opu)
{
   PD                *pd = INST_DATA(cl, obj);
   struct TagItem    *tag;
   struct RastPort   *rp;
   ULONG              rc, new;
   WORD               dis = GADGET(obj)->Flags & GFLG_DISABLED;

   /*
    * First the superclass.
    */
   rc = DoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * Frame thickness change? When the window in which
    * we are located has WINDOW_AutoAspect set to TRUE
    * the windowclass distributes the FRM_ThinFrame
    * attribute to the objects in the window. Here we
    * simply intercept it to set the selected color
    * frame thickness.
    */
   if ( tag = FindTagItem( FRM_ThinFrame, opu->opu_AttrList ))
      /*
       * Set it to the frame.
       */
      SetAttrs( pd->pd_Frame, FRM_ThinFrame, tag->ti_Data, TAG_END );

   /*
    * Color change?
    */
   if ( tag = FindTagItem( PALETTE_CurrentColor, opu->opu_AttrList )) {
      /*
       * Make sure it's valid.
       */
      new = ValidateColor( pd, tag->ti_Data );
      /*
       * Did it really change?
       */
      if ( new != pd->pd_CurrentColor ) {
	 /*
	  * Yes. Show it and notify
	  * the change.
	  */
	 ChangeSelectedColor( pd, opu->opu_GInfo, new );
	 NotifyAttrChange( obj, opu->opu_GInfo, opu->MethodID == OM_UPDATE ? opu->opu_Flags : 0L, GA_ID, GADGET(obj)->GadgetID, PALETTE_CurrentColor, pd->pd_CurrentColor, TAG_END );
      }
   }

   /*
    * Disabled state changed?
    */
   if ((GADGET(obj)->Flags & GFLG_DISABLED ) != dis ) {
      if ( opu->opu_GInfo && ( rp = ObtainGIRPort( opu->opu_GInfo ))) {
	 DoMethod( obj, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_REDRAW );
	 ReleaseGIRPort( rp );
      }
   }

   return rc;
}
METHOD_END

/*
 * Let's go active :)
 */
METHOD(PaletteClassGoActive, struct gpInput *,gpi)
{
   PD          *pd = INST_DATA(cl, obj);
   WORD         l, t;
   UWORD        newcol;
   BOOL         change = FALSE;
   ULONG        rc = GMR_NOREUSE;

   /*
    * We do not go active when we are
    * disabled or when we where activated
    * by the ActivateGadget() call.
    */
   if ((GADGET(obj)->Flags & GFLG_DISABLED ) || ( ! gpi->gpi_IEvent ))
      return( rc );

   /*
    * Save color selected when going
    * active. This way we can reset
    * the initial color when the
    * gadget activity is aborted by
    * the user or intuition.
    */
   pd->pd_InitialColor = pd->pd_CurrentColor;

   /*
    * Get the coordinates relative
    * to the top-left of the colorbox.
    */
   l = gpi->gpi_Mouse.X - ( pd->pd_ColorBox.Left - GADGET(obj)->LeftEdge );
   t = gpi->gpi_Mouse.Y - ( pd->pd_ColorBox.Top  - GADGET(obj)->TopEdge  );

   /*
    * Are we really hit?
    */
   if ( l >= 0 && t >= 0 && l < pd->pd_ColorBox.Width && t < pd->pd_ColorBox.Height ) {
      /*
       * Did the color change?
       */
      if (( newcol = GetColor( pd, l, t )) != pd->pd_CurrentColor ) {
	 /*
	  * Yes. Setup the new color and send
	  * a notification.
	  */
	 ChangeSelectedColor( pd, gpi->gpi_GInfo, newcol );
	 change = TRUE;
      }

      /*
       * Now we call the superclass to setup any
       * drag and drop support. When the superclass
       * returns GMR_NOREUSE drag and drop is not
       * supported or memory does not allow for it.
       */
      if ( DoSuperMethodA( cl, obj, ( Msg )gpi ) == GMR_NOREUSE ) {
	 /*
	  * We only notify when we do not support
	  * drag and drop and the color changed.
	  */
	 if ( change )
	    NotifyAttrChange( obj, gpi->gpi_GInfo, 0L, GA_ID, GADGET(obj)->GadgetID, PALETTE_CurrentColor, pd->pd_CurrentColor, TAG_END );
	 /*
	  * When this is set to FALSE we do
	  * normal notification.
	  */
	 pd->pd_Dragging = FALSE;
      } else
	 /*
	  * Do not do any notification except when
	  * the mouse button is release before the
	  * object was actually dragged.
	  */
	 pd->pd_Dragging = TRUE;

      /*
       * Go active.
       */
      rc = GMR_MEACTIVE;
   }
   return rc;
}
METHOD_END

/*
 * Handle the user input.
 */
METHOD(PaletteClassHandleInput, struct gpInput *,gpi)
{
   PD               *pd = INST_DATA(cl, obj);
   struct gpInput    gpd = *gpi;
   WORD              l, t;
   UWORD             newcol;
   ULONG             rc = GMR_MEACTIVE, rcd;

   /*
    * Let's see what the baseclass tell's us about
    * drag and drop support.
    */
   gpd.MethodID = BASE_DRAGGING;

   switch (( rcd = DoMethodA(obj, (Msg)&gpd)))
   {
      case BDR_CANCEL:
	 /*
	  * Reset initially selected color.
	  */
	  pd->pd_ResetInitial = TRUE;

      case BDR_DROP:
      case BDR_NONE:
	 /*
	  * BDR_NONE means that we must process the input.
	  */

	 /*
	  * Get the coordinates relative
	  * to the top-left of the colorbox.
	  */
	 l = gpi->gpi_Mouse.X - ( pd->pd_ColorBox.Left - GADGET(obj)->LeftEdge );
	 t = gpi->gpi_Mouse.Y - ( pd->pd_ColorBox.Top  - GADGET(obj)->TopEdge  );

	 /*
	  * Mouse pointer located over the object?
	  */
	 if ( l >= 0 && t >= 0 && l < pd->pd_ColorBox.Width && t < pd->pd_ColorBox.Height ) {
	    /*
	     * Mouse over a new color?
	     */
	    if (( newcol = GetColor( pd, l, t )) != pd->pd_CurrentColor ) {
	       /*
		* We do not change the color when we
		* are in drag and drop mode.
		*/
	       if (!pd->pd_Dragging)
	       {
		  /*
		   * Change the selected color.
		   */
		  ChangeSelectedColor( pd, gpi->gpi_GInfo, newcol );
		  /*
		   * Send notification
		   * about the change.
		   */
		  NotifyAttrChange( obj, gpi->gpi_GInfo, 0L, GA_ID, GADGET(obj)->GadgetID, PALETTE_CurrentColor, pd->pd_CurrentColor, TAG_END );
	       }
	    }
	 }

	 /*
	  * Check mouse input.
	  */
	 if ( gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE ) {
	    switch ( gpi->gpi_IEvent->ie_Code ) {

	       case SELECTUP:
		  /*
		   * If we are in drag mode we still send
		   * one final notification here.
		   */
		  if (pd->pd_Dragging && (rcd == BDR_NONE))
		     NotifyAttrChange( obj, gpi->gpi_GInfo, 0L, GA_ID, GADGET(obj)->GadgetID, PALETTE_CurrentColor, pd->pd_CurrentColor, TAG_END );

		  /*
		   * Left-mouse button up means we
		   * return GMR_VERIFY.
		   */
		  rc = GMR_NOREUSE | GMR_VERIFY;
		  break;

	       case MENUDOWN:
		  /*
		   * The menu button aborts the
		   * selection.
		   */
		  pd->pd_ResetInitial = TRUE;
		  rc = GMR_NOREUSE;
		  break;
	    }
	 }
	 break;

   }
   return rc;
}
METHOD_END

/*
 * Go inactive.
 */
METHOD(PaletteClassGoInactive, struct gpGoInactive *,ggi)
{
   PD        *pd = INST_DATA(cl, obj);
   ULONG      rc;

   /*
    * First the superclass.
    */
   rc = DoSuperMethodA(cl, obj, (Msg)ggi);

   /*
    * Reset initial color?
    */
   if (( pd->pd_ResetInitial || ggi->gpgi_Abort == 1 ) && pd->pd_InitialColor != pd->pd_CurrentColor ) {
      /*
       * Reset color.
       */
      ChangeSelectedColor( pd, ggi->gpgi_GInfo, pd->pd_InitialColor );
      /*
       * Notification of the reset.
       */
      NotifyAttrChange( obj, ggi->gpgi_GInfo, 0L, GA_ID, GADGET(obj)->GadgetID, PALETTE_CurrentColor, pd->pd_CurrentColor, TAG_END );
      /*
       * Clear reset flag.
       */
      pd->pd_ResetInitial = FALSE;
   }

   /*
    * Clear drag flag.
    */
   pd->pd_Dragging = FALSE;

   return rc;
}
METHOD_END

/*
 * Tell'm our minimum dimensions.
 */
METHOD(PaletteClassDimensions, struct grmDimensions *,dim)
{
   PD          *pd = INST_DATA(cl, obj);
   UWORD        mx, my;
   ULONG        rc;

   /*
    * First the superclass.
    */
   rc = DoSuperMethodA(cl, obj, (Msg)dim);

   /*
    * Now we pass our requirements. Note that
    * it is a bit difficult for this type of
    * gadgetclass to determine a suitable
    * minimum size. It would probably be
    * best to add tags for optional
    * specification. I'll leave it at this
    * for now.
    */
   switch (pd->pd_NumColors)
   {
      case 2:
      case 4:
      case 8:
	 mx = 16;
	 my = 16;
	 break;

      case 16:
      case 32:
	 mx = 32;
	 my = 32;
	 break;

      default:
	 mx = 64;
	 my = 64;
	 break;
   }

   /*
    * Store these values.
    */
   *( dim->grmd_MinSize.Width  ) += mx;
   *( dim->grmd_MinSize.Height ) += my;

   return rc;
}
METHOD_END

/*
 * Key activation.
 */
METHOD(PaletteClassKeyActive, struct wmKeyInput *,wmki)
{
   PD          *pd = INST_DATA( cl, obj );
   UWORD        qual = wmki->wmki_IEvent->ie_Qualifier, new;

   /*
    * Get the ordinal number of
    * the currently selected pen.
    */
   new = GetOrdinalNumber( pd, pd->pd_CurrentColor );

   /*
    * Shifted is backwards, normal forward.
    */
   if ( qual & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT )) {
      if ( new          ) new--;
      else                new = pd->pd_NumColors - 1;
   } else {
      if ( new < ( pd->pd_NumColors - 1 )) new++;
      else                new = 0;
   }

   /*
    * Change the color.
    */
   ChangeSelectedColor( pd, wmki->wmki_GInfo, GetPenNumber( pd, new ));

   /*
    * Notify the change.
    */
   NotifyAttrChange( obj, wmki->wmki_GInfo, 0L, GA_ID, GADGET(obj)->GadgetID, PALETTE_CurrentColor, pd->pd_CurrentColor, TAG_END );

   /*
    * Setup ID to report to
    * the event handler.
    */
   *( wmki->wmki_ID ) = GADGET(obj)->GadgetID;

   return( WMKF_VERIFY );
}
METHOD_END

/*
 * The baseclass is asking us to create a bitmap containing
 * the image which needs to be dragged arround.
 */
METHOD(PaletteClassGetObject, struct bmGetDragObject *,bmgo)
{
   PD                  *pd = INST_DATA(cl, obj);
   struct BitMap       *bm;
   struct GadgetInfo   *gi = bmgo->bmgo_GInfo;
   ULONG                rc = 0, depth;
   UWORD                x, y;

   /*
    * First we figure out the depth of the
    * bitmap were located on.
    */
   if (((struct Library *)GfxBase )->lib_Version >= 39 ) depth = GetBitMapAttr( gi->gi_Screen->RastPort.BitMap, BMA_DEPTH );
   else                    depth = gi->gi_Screen->RastPort.BitMap->Depth;

   /*
    * Allocate the BitMap.
    */
   if ( bm = BGUI_AllocBitMap( pd->pd_RectWidth, pd->pd_RectHeight, depth, BMF_CLEAR, gi->gi_Screen->RastPort.BitMap )) {

      struct RastPort rp;

      InitRastPort(&rp);
      rp.BitMap=bm;

      SetAttrs(pd->pd_Frame,
	 IA_Left,0,
	 IA_Top,0,
	 IA_Width,pd->pd_RectWidth,
	 IA_Height,pd->pd_RectHeight,
	 FRM_BackPen,pd->pd_CurrentColor,
	 TAG_END );

      DrawImageState(&rp,(struct Image *)pd->pd_Frame,0,0,IDS_NORMAL,gi->gi_DrInfo);

      /*
       * Get color rectangle coordinates.
       */
      GetTopLeft( pd, pd->pd_CurrentColor, &x, &y);

      /*
       * Set the bounds. Note that the left and
       * top edge must be relative to the window.
       */
      bmgo->bmgo_Bounds->Left   = x;
      bmgo->bmgo_Bounds->Top    = y;
      bmgo->bmgo_Bounds->Width  = pd->pd_RectWidth;
      bmgo->bmgo_Bounds->Height = pd->pd_RectHeight;

      rc = ( ULONG )bm;
   }
   return( rc );
}
METHOD_END

/*
 * Deallocate the bitmap we created above.
 */
METHOD(PaletteClassFreeObject, struct bmFreeDragObject *,bmfo)
{
   /*
    * Simply free the bitmap.
    */
   BGUI_FreeBitMap(bmfo->bmfo_ObjBitMap);
   return 1;
}
METHOD_END

/*--------------------------------LIBARY CODE FOLLOWS-----------------------------------*/

/*
 * The following code is used for the shared library.
 */
static Class *ClassBase = NULL;

/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,               (FUNCPTR)PaletteClassNew,
   OM_DISPOSE,           (FUNCPTR)PaletteClassDispose,
   OM_GET,               (FUNCPTR)PaletteClassGet,
   OM_SET,               (FUNCPTR)PaletteClassSet,
   OM_UPDATE,            (FUNCPTR)PaletteClassSet,
   GM_RENDER,            (FUNCPTR)PaletteClassRender,
   GM_GOACTIVE,          (FUNCPTR)PaletteClassGoActive,
   GM_HANDLEINPUT,       (FUNCPTR)PaletteClassHandleInput,
   GM_GOINACTIVE,        (FUNCPTR)PaletteClassGoInactive,
   GRM_DIMENSIONS,       (FUNCPTR)PaletteClassDimensions,
   WM_KEYACTIVE,         (FUNCPTR)PaletteClassKeyActive,
   BASE_GETDRAGOBJECT,   (FUNCPTR)PaletteClassGetObject,
   BASE_FREEDRAGOBJECT,  (FUNCPTR)PaletteClassFreeObject,
   DF_END
};

/*
 * Initialize the class.
 */
SAVEDS ASM Class *BGUI_ClassInit(void)
{
   ClassBase = BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
			      CLASS_DFTable,        ClassFunc,
			      CLASS_ObjectSize,     sizeof(PD),
			      TAG_DONE);
   return ClassBase;
}

/*
 * Called each time the library is closed. It simply closes
 * the required libraries and frees the class.
 */
SAVEDS ASM BOOL BGUI_ClassFree(void)
{
   return BGUI_FreeClass(ClassBase);
}
