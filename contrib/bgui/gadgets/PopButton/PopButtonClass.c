/*
 * @(#) $Header$
 *
 * PopButtonClass.c
 *
 * (C) Copyright 1999 BGUI Developement team.
 * (C) Copyright 1996 Ian J. Einman.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * NMC: Modified to allow PMB_PopPosition == ~0, so that menu opens
 *      below button, even if activated by mouse.
 *
 * NMC: Added option to disable individual menu items, using new flag
 * PMF_DISABLED in pm_Flags, and/or new methods PMBM_ENABLE_ITEM
 * and PMBM_DISABLE_ITEM. New method PMBM_ENABLE_STATUS returns
 * enable/disable status of an item.

 * $Log$
 * Revision 42.9  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.8  2003/01/18 19:10:20  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.7  2002/09/17 17:11:16  stegerg
 * inversvid drawmode together with Text() works now, so no
 * workaround needed.
 *
 * Revision 42.6  2000/08/11 08:09:39  chodorowski
 * Removed METHOD #define, already defined in bgui_compilerspecific.h.
 *
 * Revision 42.5  2000/08/09 10:17:25  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.4  2000/08/08 13:51:35  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.3  2000/07/11 17:07:01  stegerg
 * compiles now. again a temp fix for not yet working INVERSVID drawmode.
 *
 * Revision 42.2  2000/07/09 03:05:09  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:29:07  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:21:23  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:10  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:10  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1999/05/31 01:42:35  mlemos
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
#ifndef __AROS__
#include <proto/rexxsyslib.h>
#endif
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
#define CLAMP(v,i,a) if (v < i) v = i; else if (v > a) v = a;

/*
 * Simple type-cast.
 */
#define GADGET(x) ((struct Gadget *)x)

/*
** OS macros.
**/
#define OS30      (((struct Library * )SysBase)->lib_Version >= 39)
#define OS20      (((struct Library * )SysBase)->lib_Version <  39)


extern UBYTE _LibName[]   = "bgui_popbutton.gadget";
extern UBYTE _LibID[]     = "\0$VER: bgui_popbutton.gadget 41.10 (29.5.99) ©1996 Ian J. Einman ©1999 BGUI Developers Team";
extern UWORD _LibVersion  = 41;
extern UWORD _LibRevision = 10;

static ULONG instances=0;

/*
** PopupButtonClass object instance data.
**/
typedef struct {
   struct PopMenu        *pmd_MenuLabels;         /* Menu labels.                 */
   ULONG                  pmd_NumMenuLabels;   /* Number of menu labels.  */
   ULONG                  pmd_PopPos;    /* Menu under the mouse.   */
   struct Image          *pmd_Image;      /* Image for the button.   */
   Object                *pmd_VectorImage;   /* Vector image for the button. */
   Object                *pmd_CheckMark;     /* CheckMark image.     */
   UWORD                  pmd_CheckWidth;         /* Scaled checkmark width. */
   UWORD                  pmd_CheckHeight;  /* Scaled checkmark height.   */
   struct Window         *pmd_PopWindow;     /* Popup window pointer.   */
   UWORD                  pmd_PopWindowWidth;  /* Width of the pop window.   */
   UWORD                  pmd_PopWindowHeight; /* Height of the pop window.  */
   struct TextFont       *pmd_PopFont;      /* Pop window font.     */
   ULONG                  pmd_Selected;     /* Selected menu number.   */
   UWORD                  pmd_Flags;     /* See below.        */
}  PMD;

#define PMDF_NOEVENT    (1<<0)         /* ActivateGadget() activation. */

/*
** I need a new checkmark image which is "tighter"
** than the built-in BGUI one.
**/
STATIC const struct VectorItem myCheckMark[] = {
   {   14,     8,     VIF_SCALE        },
   {   1,       4,       VIF_MOVE | VIF_AREASTART  },
   {   4,       7,       VIF_DRAW         },
   {   6,       7,       VIF_DRAW         },
   {   12,     1,     VIF_DRAW         },
   {   13,     1,     VIF_DRAW         },
   {   11,     1,     VIF_DRAW         },
   {   6,       6,       VIF_DRAW         },
   {   5,       6,       VIF_DRAW         },
   {   3,       4,       VIF_DRAW         },
   {   1,       4,       VIF_LASTITEM     },
};

/*
** Default drawinfo pens. Just in
** case we don't get 'm from the
** system.
**/
STATIC UWORD DefDriPens[12] = { 0, 1, 1, 2, 1, 3, 1, 0, 2, 1, 2, 1 };

/*
** For filtering vectorclass attributes.
** Ofcourse this array needs to be updated
** when there are attributes added to the
** vectorclass.
**/
STATIC ULONG VectorAttrs[] = {   VIT_VectorArray,
	    VIT_BuiltIn,
	    VIT_Pen,
	    VIT_DriPen,
	    TAG_END };

/*
** See if we should create a vector image.
**/
//STATIC ASM Object *CreateVectorImage( REG(a1) struct TagItem *attrs )
STATIC ASM REGFUNC1(Object *, CreateVectorImage,
	REGPARAM(A1, struct TagItem *, attrs))
{
   Class       *class;
   struct TagItem    *clones;
   Object         *vector = NULL;

   /*
   ** Get a pointer to the vectorclass.
   **/
   if ( class = BGUI_GetClassPtr( BGUI_VECTOR_IMAGE )) {
      /*
      ** We clone the original taglist
      ** to filter out any unwanted
      ** attributes.
      **/
      if ( clones = CloneTagItems( attrs )) {
	 /*
	 ** Filter out any non-vectorclass
	 ** attributes.
	 **/
	 if ( FilterTagItems( clones, VectorAttrs, TAGFILTER_AND ))
	    /*
	    ** Create the vector image.
	    **/
	    vector = NewObjectA( class, NULL, clones );
	 /*
	 ** Free the clones.
	 **/
	 FreeTagItems( clones );
      }
   }
   return( vector );
}
REGFUNC_END

/*
** Notify about an attribute change.
**/
STATIC ULONG NotifyAttrChange( Object *obj, struct GadgetInfo *gi, ULONG flags, Tag tag1, ... )
{
   return( DoMethod( obj, OM_NOTIFY, &tag1, gi, flags ));
}

/*
** Copy the menu array.
**/
//STATIC ASM BOOL CopyArray( REG(a0) PMD *pmd, REG(a1) struct PopMenu *pm )
STATIC ASM REGFUNC2(BOOL, CopyArray,
	REGPARAM(A0, PMD *, pmd),
	REGPARAM(A1, struct PopMenu *, pm))
{
   struct PopMenu *tmp = pm;
   ULONG     size;
   BOOL      rc = FALSE;

   /*
   ** Count the number of entries
   ** available in the menu.
   **/
   while ( tmp->pm_Label ) {
      pmd->pmd_NumMenuLabels++;
      tmp++;
   }

   /*
   ** Compute the size of the needed
   ** allocation. One extra slot is
   ** allocated to store the terminating
   ** entry.
   **/
   size = ( pmd->pmd_NumMenuLabels + 1 ) * sizeof( struct PopMenu );

   /*
   ** Allocate enough memory to hold
   ** the copy of the array.
   **/
   if ( pmd->pmd_MenuLabels = ( struct PopMenu * )AllocVec( size, MEMF_PUBLIC )) {
      /*
      ** Copy the array.
      **/
      CopyMem( pm, pmd->pmd_MenuLabels, size );
      /*
      ** Check if the position is correct.
      **/
      if ( pmd->pmd_PopPos != ~0 )     /* NMC:Added */
	 {
	 if ( pmd->pmd_PopPos >= pmd->pmd_NumMenuLabels )
	    pmd->pmd_PopPos = pmd->pmd_NumMenuLabels - 1;
	 }
      /*
      ** Success :)
      **/
      rc = TRUE;
   }
   return( rc );
}
REGFUNC_END

/*
** Scale the checkmark.
**/
//STATIC ASM VOID ScaleCheckMark( REG(a0) PMD *pmd, REG(a1) struct RastPort *rp )
STATIC ASM REGFUNC2(VOID, ScaleCheckMark,
	REGPARAM(A0, PMD *, pmd),
	REGPARAM(A1, struct RastPort *, rp))
{
   UBYTE        *refstr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,_:";

   /*
   ** Could probably use some work
   ** but it works for me.
   **/
   pmd->pmd_CheckHeight = rp->TxHeight;
   pmd->pmd_CheckWidth  = TextLength( rp, refstr, 55 ) / 55;
   pmd->pmd_CheckWidth  = pmd->pmd_CheckWidth + 6;

   SetAttrs( pmd->pmd_CheckMark, IA_Width, pmd->pmd_CheckWidth, IA_Height, pmd->pmd_CheckHeight, TAG_END );
}
REGFUNC_END

/*
** Re-compute the size of the popup window.
**/
//STATIC ASM VOID ComputePopWindowSize( REG(a0) PMD *pmd, REG(a1) struct TextFont *tf )
STATIC ASM REGFUNC2(VOID, ComputePopWindowSize,
	REGPARAM(A0, PMD *, pmd),
	REGPARAM(A1, struct TextFont *, tf))
{
   struct RastPort         rp;
   UWORD       width = 0, height = 0, tlen;
   struct PopMenu        *labels = pmd->pmd_MenuLabels;

   /*
   ** Initialize rastport and setup the font.
   **/
   InitRastPort( &rp );
   SetFont( &rp, tf );

   ScaleCheckMark( pmd, &rp );

   /*
   ** Compute the size of the
   ** window accoording to the
   ** menu labels.
   **/
   while ( labels->pm_Label ) {
      /*
      ** Compute the width of a normal
      ** menu label.
      **/
      if ( labels->pm_Label != PMB_BARLABEL ) {
	 if ( labels->pm_Flags & PMF_CHECKIT ) tlen = pmd->pmd_CheckWidth + 2;
	 else                 tlen = 0;
	 if (( tlen += TextLength( &rp, labels->pm_Label, strlen( labels->pm_Label ))) > width )
	    width = tlen;
	 height += tf->tf_YSize + 1;
      } else
	 height += 6;
      /*
      ** Next...
      **/
      labels++;
   }

   /*
   ** Set values.
   **/
   pmd->pmd_PopWindowWidth  = width  + 12;
   pmd->pmd_PopWindowHeight = height + 4;

}
REGFUNC_END

/*
** Render a bar-label.
**/
//STATIC ASM VOID RenderBarLabel( REG(a0) struct RastPort *rp, REG(d0) UWORD top, REG(d1) UWORD width )
STATIC ASM REGFUNC3(VOID, RenderBarLabel,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(D0, UWORD, top),
	REGPARAM(D1, UWORD, width))
{
   /*
   ** Render first line.
   **/
   SetDrPt( rp, 0x7777 );     /* %0111011101110111 */
   Move( rp, 6,    top );
   Draw( rp, width, top );

   /*
   ** Render second line.
   **/
   SetDrPt( rp, 0xDDDD );     /* %1101110111011101 */
   Move( rp, 6,    top + 1 );
   Draw( rp, width, top + 1 );

   SetDrPt( rp, ( UWORD )-1 );   /* %1111111111111111 */
}
REGFUNC_END

/*
** Get the y-position of the item
** number "entry".
**/
//STATIC ASM UWORD GetYPos( REG(a0) struct RastPort *rp, REG(a1) struct PopMenu *labels, REG(d0) ULONG entry )
STATIC ASM REGFUNC3(UWORD, GetYPos,
	REGPARAM(A0, struct RastPort *, rp),
	REGPARAM(A1, struct PopMenu *, labels),
	REGPARAM(D0, ULONG, entry))
{
   UWORD       num = 0, ypos = 2;

   /*
   ** Pass through the labels updating
   ** the Y-counter until "num" matches "entry".
   **/
   while ( labels->pm_Label && num != entry ) {
      if ( labels->pm_Label == PMB_BARLABEL ) ypos += 6;
      else              ypos += rp->TxHeight + 1;
      labels++;
      num++;
   }

   return( ypos );
}
REGFUNC_END

/*
** Render a popmenu entry.
**/
STATIC VOID RenderMenuEntry( struct RastPort *rp, PMD *pmd, UWORD *pens, ULONG num, BOOL sel, struct DrawInfo *dr )
{
   UWORD       ypos, pena, penb;
   struct PopMenu        *label = &pmd->pmd_MenuLabels[ num ];
   static UWORD      ghostpat[] = { 0x2222, 0x8888 };

   /*
   ** Force select to FALSE if item disabled (NMC:Added)
   **/
   if ( sel && (label->pm_Flags & PMF_DISABLED) )
      sel = FALSE;

   /*
   ** Compute entry position.
   **/
   ypos = GetYPos( rp, pmd->pmd_MenuLabels, num );

   /*
   ** Setup the correct pens.
   **/
   if ( OS20 ) {
      pena = pens[ DETAILPEN ];
      penb = pens[ BLOCKPEN  ];
   } else {
      pena = pens[ BARDETAILPEN ];
      penb = pens[ BARBLOCKPEN  ];
   }

   /*
   ** We only backfill when we render
   ** a normal menu label.
   **/
   if ( label->pm_Label != PMB_BARLABEL ) {
      /*
      ** Backfill the menu label.
      **/
      if ( OS30 ) SetAPen( rp, sel ? pena : penb );
      else      SetAPen( rp, penb );
      SetDrMd( rp, JAM1 );

      RectFill( rp, 4, ypos, pmd->pmd_PopWindowWidth - 5, ypos + rp->TxHeight );
   }

   /*
   ** Setup drawmode.
   **/
   if ( OS30 ) SetDrMd( rp, sel ? JAM2|INVERSVID : JAM2 );
   else      SetDrMd( rp, JAM2 );

   /*
   ** Set the pens.
   **/

   SetAPen( rp, pena );
   SetBPen( rp, penb );

   /*
   ** Render the menu label.
   **/
   if ( label->pm_Label != PMB_BARLABEL ) {
      /*
      ** Checkable?
      **/
      if ( label->pm_Flags & PMF_CHECKIT ) {
	 /*
	 ** Checked?
	 **/
	 if ( label->pm_Flags & PMF_CHECKED ) {
	    SetAttrs( pmd->pmd_CheckMark, VIT_Pen, sel ? penb : pena, TAG_END );
	    DrawImageState( rp, ( struct Image * )pmd->pmd_CheckMark, 6, ypos, IDS_NORMAL, dr );
	 }
	 Move( rp, 8 + pmd->pmd_CheckWidth, ypos + rp->TxBaseline + 1 );
      } else
	 Move( rp, 6, ypos + rp->TxBaseline + 1 );
      Text( rp, label->pm_Label, strlen( label->pm_Label ));

      /*
      ** Disabled? (NMC:Added)
      **/
      if ( label->pm_Flags & PMF_DISABLED ) {
	 SetAPen( rp, penb );
	 SetDrMd( rp, JAM1 );
	 SetAfPt( rp, ghostpat, 1 );
	 RectFill( rp, 4, ypos, pmd->pmd_PopWindowWidth - 5, ypos + rp->TxHeight );
	 SetAfPt( rp, NULL, 0 );
      }

      /*
      ** Invers the entry if were running
      ** on OS 2.04 and it's selected. (NMC:moved & edited)
      **/
      if ( OS20 && sel ) {
	 SetDrMd( rp, JAM2|COMPLEMENT );
	 RectFill( rp, 4, ypos, pmd->pmd_PopWindowWidth - 5, ypos + rp->TxHeight );
      }
   } else
      RenderBarLabel( rp, ypos + 2, pmd->pmd_PopWindowWidth - 6 );
}

/*
** Open the popup menu window.
**
** Please note that if there are more items that
** there will fit on the screen the list is clipped
** off at the bottom.
**/
//STATIC ASM ULONG OpenPopupWindow( REG(a0) PMD *pmd, REG(a1) Object *obj, REG(a2) struct gpInput *gpi, REG(d0) BOOL mouse )
STATIC ASM REGFUNC4(ULONG, OpenPopupWindow,
	REGPARAM(A0, PMD *, pmd),
	REGPARAM(A1, Object *, obj),
	REGPARAM(A2, struct gpInput *, gpi),
	REGPARAM(D0, BOOL, mouse))
{
   struct Screen     *screen = gpi->gpi_GInfo->gi_Screen;
   struct RastPort          rpt;
   UWORD        sw, sh, wleft, wtop, wwi, wwh, *pens;
   struct IBox    *ibox;
   struct TextAttr         *tattr = NULL;
   struct TextFont         *tf;
   struct RastPort         *rp;
   ULONG        rc = GMR_NOREUSE;

   /*
   ** Get a pointer to the pen array.
   **/
   pens = gpi->gpi_GInfo->gi_DrInfo ? gpi->gpi_GInfo->gi_DrInfo->dri_Pens : DefDriPens;

   /*
   ** Get screen dimensions.
   **/
   sw = screen->Width;
   sh = screen->Height;

   /*
   ** Get object bounds and the font
   ** we use..
   **/
   DoMethod( obj, OM_GET, BT_HitBox,   &ibox );
   DoMethod( obj, OM_GET, BT_TextAttr, &tattr );

   /*
   ** Open the font.
   **/
   if ( tattr ) {
      if ( tf = pmd->pmd_PopFont = OpenFont( tattr )) {
	 /*
	 ** Compute popwindow size.
	 **/
	 ComputePopWindowSize( pmd, tf );

	 /*
	 ** Setup dummy rastport.
	 **/
	 InitRastPort( &rpt );
	 SetFont( &rpt, tf );

	 /*
	 ** Copy dimensions.
	 **/
	 wwi = pmd->pmd_PopWindowWidth;
	 wwh = pmd->pmd_PopWindowHeight;

	 /*
	 ** Compute the popwindow
	 ** position.
	 **/
	 if ( mouse && ( pmd->pmd_PopPos != ~0 ) ) {  /* NMC:Added */
	    /*
	    ** Mouse activation means that the window
	    ** is opened with the first label centered
	    ** under the mouse.
	    **/
	    wleft = screen->MouseX - ( wwi >> 1 );
	    wtop  = screen->MouseY - ( 2 + GetYPos( &rpt, pmd->pmd_MenuLabels, pmd->pmd_PopPos ) + ( tf->tf_YSize >> 1 ));
	 } else {
	    /*
	    ** Key actiation will open the window
	    ** under the object.
	    **/
	    wleft  = ibox->Left + (( ibox->Width >> 1 ) - ( wwi >> 1 )) + gpi->gpi_GInfo->gi_Window->LeftEdge;
	    wtop   = ibox->Top + ibox->Height + gpi->gpi_GInfo->gi_Window->TopEdge;
	 }


	 /*
	 ** Open the pop window.
	 **/
	 pmd->pmd_PopWindow = OpenWindowTags( NULL, WA_Left,      wleft,
			   WA_Top,     wtop,
			   WA_Width,      wwi,
			   WA_Height,     wwh,
			   WA_Flags,      0L,
			   WA_IDCMP,      0L,
			   WA_Borderless, TRUE,
			   WA_AutoAdjust, TRUE,
			   WA_SimpleRefresh, TRUE,
			   WA_NoCareRefresh, TRUE,
			   WA_RMBTrap,    TRUE,
			   WA_CustomScreen,  screen,
			   TAG_END );

	 /*
	 ** Success?
	 **/
	 if ( pmd->pmd_PopWindow ) {
	    /*
	    ** Pick up the rastport.
	    **/
	    rp = pmd->pmd_PopWindow->RPort;

	    /*
	    ** Set the font.
	    **/
	    SetFont( rp, tf );

	    /*
	    ** Background pen.
	    **/
	    if ( OS20 ) sw = pens[ BLOCKPEN    ];
	    else      sw = pens[ BARBLOCKPEN ];

	    /*
	    ** Backfill the menu.
	    **/
	    SetAPen( rp, sw );
	    SetDrMd( rp, JAM1 );
	    RectFill( rp, 0, 0, wwi, wwh );

	    /*
	    ** Detail pen.
	    **/
	    if ( OS20 ) sw = pens[ DETAILPEN    ];
	    else      sw = pens[ BARDETAILPEN ];

	    /*
	    ** Render frame.
	    **/
	    SetAPen( rp, sw );

	    Move( rp, 0, 0 );
	    Draw( rp, wwi - 1, 0);
	    Draw( rp, wwi - 1, wwh - 1 );
	    Draw( rp, 0, wwh - 1 );
	    Draw( rp, 0, 0 );
	    Move( rp, 1, 0 );
	    Draw( rp, 1, wwh - 1 );
	    Move( rp, wwi - 2, 0 );
	    Draw( rp, wwi - 2, wwh - 1 );

	    /*
	    ** Render the menus...
	    **/
	    for ( sw = 0; sw < pmd->pmd_NumMenuLabels; sw++ )
	       RenderMenuEntry( rp, pmd, pens, sw, FALSE, gpi->gpi_GInfo->gi_DrInfo );

	    rc = GMR_MEACTIVE;
	 } else {
	    CloseFont( tf );
	    pmd->pmd_PopFont = NULL;
	 }
      }
   }
   return( rc );
}
REGFUNC_END

/*
** Return the menu number under the mouse
**      or ~0 if mouse not over selectable item. (NMC:added)
**/
//STATIC ASM LONG Selected( REG(a0) PMD *pmd, REG(a1) struct RastPort *rp )
STATIC ASM REGFUNC2(LONG, Selected,
	REGPARAM(A0, PMD *, pmd),
	REGPARAM(A1, struct RastPort *, rp))
{
   WORD        mx = pmd->pmd_PopWindow->MouseX, my = pmd->pmd_PopWindow->MouseY, ypos = 2;
   struct PopMenu        *labels = pmd->pmd_MenuLabels;
   LONG        item = 0;

   /*
   ** Mouse still in the window?
   **/
   if ( mx >= 4 && mx < ( pmd->pmd_PopWindow->Width - 4 ) && my >= 2 && my < ( pmd->pmd_PopWindow->Height - 2 )) {
      /*
      ** Start at Y-offset 2 which is
      ** where the first item is located.
      **/
      ypos = 2;

      /*
      ** Go through the labels until the
      ** mouse position is located inside
      ** the offset.
      **/
      while ( labels->pm_Label ) {
	 if ( labels->pm_Label == PMB_BARLABEL ) {
	    /*
	    ** Is it on this barlabel?
	    **/
	    if ( my >= ypos && my < ( ypos + 6 ))
	       /*
	       ** Not selectable!
	       **/
	       return( ~0 );
	    ypos += 6;
	 } else {
	    /*
	    ** Is it on this item?
	    **/
	    if ( my >= ypos && my < ( ypos + rp->TxHeight + 1 )) {
	       /*
	       ** Is item disabled? (NMC:Added)
	       **/
	       if ( labels->pm_Flags & PMF_DISABLED )
		  /*
		  ** Yes. Not selectable!
		  **/
		  return( ~0 );
	       else
		  /*
		  ** No. Return it's number.
		  **/
		  return( item );
	       }
	    ypos += rp->TxHeight + 1;
	 }
	 labels++;
	 item++;
      }
   }
   return( ~0 );
}
REGFUNC_END

/*
** Determine if indicated item is selectable,
** returning TRUE or FALSE. (NMC:Added)
**/

//STATIC ASM BOOL ItemSelectable( REG(a0) PMD *pmd, REG(d0) ULONG num )
STATIC ASM REGFUNC2(BOOL, ItemSelectable,
	REGPARAM(A0, PMD *, pmd),
	REGPARAM(D0, ULONG, num))
{
   if ( ( pmd->pmd_MenuLabels[ num ].pm_Label == PMB_BARLABEL ) ||
      ( pmd->pmd_MenuLabels[ num ].pm_Flags & PMF_DISABLED ) )
      return( FALSE );
   return( TRUE );
}
REGFUNC_END

/*
** Find the previous selectable item.
** Return current selection if no previous
** found or no item selected to begin with. (NMC:Added)
**/
//STATIC ASM ULONG PrevItem( REG(a0) PMD *pmd )
STATIC ASM REGFUNC1(ULONG, PrevItem,
	REGPARAM(A0, PMD *, pmd))
{
   ULONG    prev = pmd->pmd_Selected - 1;

   /*
   ** Are we at the top already?
   ** Or, no item selected at all? (NMC:Added)
   **/
   if ( ! pmd->pmd_Selected  || ( pmd->pmd_Selected == ~0 ) )
      return( pmd->pmd_Selected );

   /*
   ** Look up the item. (NMC:Edited)
   **/
   while ( ! ItemSelectable( pmd, prev ) ) {
      if ( ! prev )
	 return( pmd->pmd_Selected );
      prev--;
   }

   return( prev );
}
REGFUNC_END

/*
** Find the next selectable item.
** Return current selection if no next found. (NMC:Added)
**/
//STATIC ASM ULONG NextItem( REG(a0) PMD *pmd )
STATIC ASM REGFUNC1(ULONG, NextItem,
	REGPARAM(A0, PMD *, pmd))
{
   ULONG    next = pmd->pmd_Selected + 1;

   /*
   ** Are we at the end already?
   ** Or, no item selected at all? (NMC:Added)
   **/
   if ( ( pmd->pmd_Selected == pmd->pmd_NumMenuLabels - 1 ) ||
      ( pmd->pmd_Selected == ~0 ) )
      return( pmd->pmd_Selected );

   /*
   ** Look up the item. (NMC:Edited)
   **/
   while ( ! ItemSelectable( pmd, next ) ) {
      if ( next == pmd->pmd_NumMenuLabels - 1 )
	 return( pmd->pmd_Selected );
      next++;
   }

   return( next );
}
REGFUNC_END

/*
** Mutually exclude other items.
**/
//STATIC ASM VOID MutEx( REG(a0) PMD *pmd )
STATIC ASM REGFUNC1(VOID, MutEx,
	REGPARAM(A0, PMD *, pmd))
{
   struct PopMenu        *labels = pmd->pmd_MenuLabels, *the_one = &pmd->pmd_MenuLabels[ pmd->pmd_Selected ];
   UWORD       i;

   /*
   ** Anything to exclude?
   **/
   if ( the_one->pm_MutualExclude ) {
      /*
      ** Maximum range which can be
      ** excluded is 32 items.
      **/
      for ( i = 0; i < 32; i++, labels++ ) {
	 /*
	 ** End of menu?
	 **/
	 if ( ! labels->pm_Label )
	    break;
	 /*
	 ** Skip the currently selected
	 ** item.
	 **/
	 if ( labels != the_one ) {
	    /*
	    ** Checkable item?
	    **/
	    if ( labels->pm_Flags & PMF_CHECKIT ) {
	       /*
	       ** Exclude bit set?
	       **/
	       if ( the_one->pm_MutualExclude & ( 1 << i ))
		  /*
		  ** Yes. Un-check it.
		  **/
		  labels->pm_Flags &= ~PMF_CHECKED;
	    }
	 }
      }
   }
}
REGFUNC_END

/*
** Compute minimum object dimensions.
**/
//STATIC ASM VOID SetDimensions( REG(a0) Object *obj, REG(a1) struct grmDimensions *dim, REG(d0) UWORD mx, REG(d1) UWORD my )
STATIC ASM REGFUNC4(VOID, SetDimensions,
	REGPARAM(A0, Object *, obj),
	REGPARAM(A1, struct grmDimensions *, dim),
	REGPARAM(D0, UWORD, mx),
	REGPARAM(D1, UWORD, my))
{
   Object            *label = NULL, *frame = NULL;
   ULONG           place, fh = 0, fv = 0;

   /*
   ** Including frame?
   **/
   if ( ! ( dim->grmd_Flags & GDIMF_NO_FRAME )) {
      /*
       * Obtain frame object.
       */
      DoMethod( obj, OM_GET, BT_FrameObject, &frame );

      /*
       *      Get width & height.
       * NMC 23/12/95: Added Jan's bugfix to this
       * to handle case of no frame object.
       */
      if ( frame ) {
	 GetAttr( FRM_FrameWidth,  frame, &fh );
	 GetAttr( FRM_FrameHeight, frame, &fv );
	 fh <<= 1;
	 fh  += 4;
	 fv <<= 1;
	 fv  += 2;
      }
   }

   /*
   ** Get object label.
   **/
   DoMethod( obj, OM_GET, BT_LabelObject, &label );
   if ( label ) {
      /*
      ** Get label position.
      **/
      DoMethod( label, OM_GET, LAB_Place, &place );

      switch ( place ) {
	 case  PLACE_LEFT:
	 case  PLACE_IN:
	 case  PLACE_RIGHT:
	    if ( my < *( dim->grmd_MinSize.Height )) my  = *( dim->grmd_MinSize.Height );
	    else               my += fv;
	    break;
	 case  PLACE_ABOVE:
	 case  PLACE_BELOW:
	    my += *( dim->grmd_MinSize.Height );
	    break;
      }

      switch ( place ) {
	 case  PLACE_ABOVE:
	 case  PLACE_IN:
	 case  PLACE_BELOW:
	    if ( mx < *( dim->grmd_MinSize.Width )) mx  = *( dim->grmd_MinSize.Width );
	    else              mx += fh;
	    break;
	 case  PLACE_LEFT:
	 case  PLACE_RIGHT:
	    mx += *( dim->grmd_MinSize.Width );
	    break;
      }
   } else {
      mx += fh;
      my += fv;
   }

   *( dim->grmd_MinSize.Width  ) = mx;
   *( dim->grmd_MinSize.Height ) = my;
}
REGFUNC_END

/*
** Create a shiny new object.
**/
//STATIC ASM ULONG PMBClassNew( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opSet *ops )
STATIC ASM REGFUNC3(ULONG, PMBClassNew,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops))
{
   PMD         *pmd;
   struct TagItem    *tstate = ops->ops_AttrList, *tag;
   struct PopMenu    *pm=NULL;
   ULONG        rc = 0L;

   if(BGUIBase==NULL
   && (BGUIBase=OpenLibrary("bgui.library",41))==NULL)
      return(NULL);

   /*
   ** First we let the superclass
   ** create an object.
   **/
   if ( rc = DoSuperMethodA( cl, obj, ( Msg )ops )) {

      instances++;

      /*
      ** Get to the instance data.
      **/
      pmd = ( PMD * )INST_DATA( cl, rc );

      /*
      ** Is this necessary?
      **/
      bzero(( char * )pmd, sizeof( PMD ));

      /*
      ** Get attributes.
      **/
      while ( tag = NextTagItem( &tstate )) {
	 switch ( tag->ti_Tag ) {

	    case  PMB_Image:
	       pmd->pmd_Image = ( struct Image * )tag->ti_Data;
	       break;

	    case  PMB_MenuEntries:
	       pm = ( struct PopMenu * )tag->ti_Data;
	       break;

	    case  PMB_PopPosition:
	       pmd->pmd_PopPos = tag->ti_Data;
	       break;
	 }
      }

      /*
      ** If we do not have a intuition image
      ** we go for a vectorclass image.
      **/
      if ( ! pmd->pmd_Image )
	 pmd->pmd_VectorImage = CreateVectorImage( ops->ops_AttrList );

      /*
      ** We _must_ have menu entries.
      **/
      if ( pm && CopyArray( pmd, pm )) {
	 /*
	 ** Force the correct activation flags.
	 **/
	 SetAttrs(( Object * )rc, GA_Immediate, FALSE, GA_RelVerify, TRUE, TAG_END );
	 if ( pmd->pmd_CheckMark = BGUI_NewObject( BGUI_VECTOR_IMAGE, VIT_VectorArray, myCheckMark, IA_Left, 0, IA_Top, 0, TAG_END ))
	    return( rc );
      }

      /*
      ** Otherwise we fail :(
      **/
      CoerceMethod( cl, ( Object * )rc, OM_DISPOSE );
   }
   if(instances==0
   && BGUIBase)
   {
      CloseLibrary(BGUIBase);
      BGUIBase=NULL;
   }
   return( NULL );
}
REGFUNC_END

/*
** Set object attributes.
**/
//STATIC ASM ULONG PMBClassSet( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opSet *ops )
STATIC ASM REGFUNC3(ULONG, PMBClassSet,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );
   struct TagItem    *tstate = ops->ops_AttrList, *tag;
   struct RastPort         *rp;
   ULONG        rc = 0L;
   WORD         dis = GADGET( obj )->Flags & GFLG_DISABLED;
   BOOL         update = FALSE;

   /*
   ** Be curtious, let the superclass go
   ** first.
   **/
   rc = DoSuperMethodA( cl, obj, ( Msg )ops );

   /*
   ** Get tags.
   **/
   while ( tag = NextTagItem( &tstate )) {
      switch ( tag->ti_Tag ) {

	 case  VIT_VectorArray:
	 case  VIT_BuiltIn:
	 case  VIT_Pen:
	 case  VIT_DriPen:
	    /*
	    ** Only set when a vector image
	    ** exists!
	    **/
	    if ( pmd->pmd_VectorImage ) {
	       SetAttrs( pmd->pmd_VectorImage, tag->ti_Tag, tag->ti_Data, TAG_END );
	       update = TRUE;
	    }
	    break;

	 case  PMB_Image:
	    /*
	    ** Only changeable when a previous image
	    ** exists!
	    **
	    ** Note that you are responsible of making
	    ** sure the image fit's inside the dimensions
	    ** of the one you're replacing!
	    **/
	    if ( pmd->pmd_Image ) {
	       pmd->pmd_Image = ( struct Image * )tag->ti_Data;
	       update = TRUE;
	    }
	    break;
      }
   }

   /*
   ** Visual update necessary?
   **/
   if ((( GADGET( obj )->Flags & GFLG_DISABLED ) != dis ) || update ) {
      /*
      ** Re-render the object.
      **/
      if ( ops->ops_GInfo ) {
	 if ( rp = ObtainGIRPort( ops->ops_GInfo )) {
	    DoMethod( obj, GM_RENDER, ops->ops_GInfo, rp, GREDRAW_REDRAW );
	    ReleaseGIRPort( rp );
	 }
      }
   }
   return( rc );
}
REGFUNC_END

/*
** Re-render the object.
**/
//STATIC ASM ULONG PMBClassRender( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpRender *gpr )
STATIC ASM REGFUNC3(ULONG, PMBClassRender,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpRender *, gpr))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );
   struct RastPort         *rp = gpr->gpr_RPort;
   struct IBox    *ibox;
   static UWORD       ghostpat[] = { 0x2222, 0x8888 };
   UWORD       *pens, xoff, yoff;
   ULONG        rc;

   /*
   ** First the superclass. If that returns
   ** NULL we don't render.
   **/
   if ( rc = DoSuperMethodA( cl, obj, ( Msg )gpr )) {
      /*
      ** Get hitbox bounds.
      **/
      DoMethod( obj, OM_GET, BT_HitBox, &ibox );

      /*
      ** Image to render?
      **/
      if ( pmd->pmd_Image ) {
	 /*
	 ** Set the left and top edge.
	 **/
	 xoff = ( ibox->Width  >> 1 ) - ( pmd->pmd_Image->Width   >> 1 ) + ibox->Left;
	 yoff = ( ibox->Height >> 1 ) - ( pmd->pmd_Image->Height >> 1 ) + ibox->Top;
	 /*
	 ** Render it.
	 **/
	 DrawImageState( rp, pmd->pmd_Image, xoff, yoff, IDS_NORMAL, gpr->gpr_GInfo->gi_DrInfo );
      } else if ( pmd->pmd_VectorImage ) {
	 /*
	 ** Set bounds.
	 **/
	 SetAttrs( pmd->pmd_VectorImage, IA_Left,   ibox->Left,
		     IA_Top,    ibox->Top,
		     IA_Width,  ibox->Width,
		     IA_Height, ibox->Height,
		     TAG_END );
	 /*
	 ** Render it.
	 **/
	 DrawImageState( rp, ( struct Image * )pmd->pmd_VectorImage, 0, 0, IDS_NORMAL, gpr->gpr_GInfo->gi_DrInfo );
      }
      /*
      ** Object disabled?
      **/
      if ( GADGET( obj )->Flags & GFLG_DISABLED ) {
	 /*
	 ** Get the hitbox dimensions.
	 **/
	 DoMethod( obj, OM_GET, BT_HitBox, &ibox );
	 /*
	 ** Pick up pen array.
	 **/
	 pens = gpr->gpr_GInfo->gi_DrInfo ? gpr->gpr_GInfo->gi_DrInfo->dri_Pens : DefDriPens;
	 /*
	 ** Ghost it.
	 **/
	 SetAfPt( gpr->gpr_RPort, ghostpat, 1 );
	 SetAPen( gpr->gpr_RPort, pens[ SHADOWPEN ] );
	 SetDrMd( gpr->gpr_RPort, JAM1 );
	 RectFill( gpr->gpr_RPort, ibox->Left, ibox->Top, ibox->Left + ibox->Width - 1, ibox->Top + ibox->Height - 1 );
      }
   }
   BGUI_PostRender(cl, obj, gpr);

   return( rc );
}
REGFUNC_END

/*
** Let's go active.
**/
//STATIC ASM ULONG PMBClassGoActive( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpInput *gpi )
STATIC ASM REGFUNC3(ULONG, PMBClassGoActive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );
   UWORD       *pens;
   ULONG        rc = GMR_NOREUSE;

   /*
   ** Disabled means that we refuse to go active.
   **/
   if ( GADGET( obj )->Flags & GFLG_DISABLED )
      return( rc );

   /*
   ** By default there's nothing selected.
   **/
   pmd->pmd_Selected = ~0;

   /*
   ** Let's open the popup window.
   **/
   if (( rc = OpenPopupWindow( pmd, obj, gpi, gpi->gpi_IEvent ? TRUE : FALSE )) == GMR_MEACTIVE ) {
      /*
      ** Activated by ActivateGadget()?
      **/
      if ( ! gpi->gpi_IEvent ) {
	 /*
	 ** Mark us as key-activated.
	 **/
	 pmd->pmd_Flags |= PMDF_NOEVENT;
	 /*
	 ** Pick up pen array.
	 **/
	 pens = gpi->gpi_GInfo->gi_DrInfo ? gpi->gpi_GInfo->gi_DrInfo->dri_Pens : DefDriPens;
	 /*
	 ** Pre-select the first menu.
	 **/
	 pmd->pmd_Selected = pmd->pmd_PopPos;

	 if ( pmd->pmd_Selected == ~0 )   /* NMC:Added */
	    pmd->pmd_Selected = 0;
	 /*
	 ** Try to get a selectable item.
	 ** (NMC:Edited)
	 **/

	 if ( ! ItemSelectable( pmd, pmd->pmd_Selected ) ) {
	    /*
	    ** Scan down for a selectable item
	    **/
	    pmd->pmd_Selected = NextItem( pmd );

	    if ( ! ItemSelectable( pmd, pmd->pmd_Selected ) ) {
	       /*
	       ** Nope? Try scanning up, then
	       **/
	       pmd->pmd_Selected = PrevItem( pmd );
	    }
	 }

	 if ( ItemSelectable( pmd, pmd->pmd_Selected ) )
	    /*
	    ** Select the item.
	    **/
	    RenderMenuEntry( pmd->pmd_PopWindow->RPort, pmd, pens, pmd->pmd_Selected, TRUE, gpi->gpi_GInfo->gi_DrInfo );
	 else
	    /*
	    ** Nothing selectable at all
	    **/
	    pmd->pmd_Selected = ~0;
      }
   }
   return( rc );
}
REGFUNC_END

/*
** Handle the user input.
**/
//STATIC ASM ULONG PMBClassHandleInput( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpInput *gpi )
STATIC ASM REGFUNC3(ULONG, PMBClassHandleInput,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );
   ULONG        item, rc = GMR_MEACTIVE;
   UWORD       *pens;

   /*
   ** The keyboard control of the menu does not
   ** work correctly when you have an sunmouse/key
   ** like tool like Yak running which automatically
   ** activates the window under the mouse.
   **
   ** Intuition will automatically deactivate the
   ** object when the parent window becomes deactivated.
   **
   ** The below code is just to make sure.
   **/
   if ( ! ( gpi->gpi_GInfo->gi_Window->Flags & WFLG_WINDOWACTIVE ))
      return( GMR_NOREUSE );

   /*
   ** Pick up pen array.
   **/
   pens = gpi->gpi_GInfo->gi_DrInfo ? gpi->gpi_GInfo->gi_DrInfo->dri_Pens : DefDriPens;

   /*
   ** Mouse activated?
   **/
   if ( ! ( pmd->pmd_Flags & PMDF_NOEVENT )) {
      /*
      ** Where's the mouse.
      **/
      item = Selected( pmd, pmd->pmd_PopWindow->RPort );
      /*
      ** Did it change?
      **/
      if ( item != pmd->pmd_Selected ) {
	 /*
	 ** Unselect current entry and
	 ** select the new one.
	 **/
	 if ( pmd->pmd_Selected != ~0 ) RenderMenuEntry( pmd->pmd_PopWindow->RPort, pmd, pens, pmd->pmd_Selected, FALSE, gpi->gpi_GInfo->gi_DrInfo );
	 if ( item != ~0 )        RenderMenuEntry( pmd->pmd_PopWindow->RPort, pmd, pens, item, TRUE, gpi->gpi_GInfo->gi_DrInfo );
	 /*
	 ** Set it up.
	 **/
	 pmd->pmd_Selected = item;
      }
      /*
      ** Let's see the mouse stuff...
      **/
      if ( gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE ) {
	 switch ( gpi->gpi_IEvent->ie_Code ) {

	    case  SELECTUP:
	       /*
	       ** Report a change to the application.
	       **/
	       if ( pmd->pmd_Selected != ~0 ) {
		  /*
		  ** Notify the selection.
		  **/
		  NotifyAttrChange( obj, gpi->gpi_GInfo, 0L, GA_ID, GADGET( obj )->GadgetID, PMB_MenuNumber, pmd->pmd_Selected, TAG_END );
		  /*
		  ** PMF_CHECKIT item?
		  **/
		  if ( pmd->pmd_MenuLabels[ pmd->pmd_Selected ].pm_Flags & PMF_CHECKIT ) {
		     /*
		     ** Toggle PMF_CHECKED bit.
		     **/
		     pmd->pmd_MenuLabels[ pmd->pmd_Selected ].pm_Flags ^= PMF_CHECKED;
		     /*
		     ** Do mutual exclusion.
		     **/
		     MutEx( pmd );
		  }
		  /*
		  ** Return GMR_VERIFY. Tell intuition to
		  ** discard the input event.
		  **/
		  rc = GMR_NOREUSE | GMR_VERIFY;
	       } else
		  rc = GMR_NOREUSE;
	       break;

	    case  MENUDOWN:
	       /*
	       ** Abort the mission and let
	       ** intuition snap down the
	       ** menus.
	       **/
	       rc = GMR_REUSE;
	       break;
	 }
      }
   } else {
      if ( gpi->gpi_IEvent->ie_Class == IECLASS_RAWKEY ) {
	 switch ( gpi->gpi_IEvent->ie_Code ) {

	    case  0x4C:
	       /*
	       ** CURSOR UP
	       **/
	       if (( item =  PrevItem( pmd )) != pmd->pmd_Selected ) {
		  RenderMenuEntry( pmd->pmd_PopWindow->RPort, pmd, pens, pmd->pmd_Selected, FALSE, gpi->gpi_GInfo->gi_DrInfo );
		  pmd->pmd_Selected = item;
		  RenderMenuEntry( pmd->pmd_PopWindow->RPort, pmd, pens, pmd->pmd_Selected, TRUE, gpi->gpi_GInfo->gi_DrInfo );
	       }
	       break;

	    case  0x4D:
	       /*
	       ** CURSOR DOWN
	       **/
	       if (( item = NextItem( pmd )) != pmd->pmd_Selected ) {
		  RenderMenuEntry( pmd->pmd_PopWindow->RPort, pmd, pens, pmd->pmd_Selected, FALSE, gpi->gpi_GInfo->gi_DrInfo );
		  pmd->pmd_Selected = item;
		  RenderMenuEntry( pmd->pmd_PopWindow->RPort, pmd, pens, pmd->pmd_Selected, TRUE, gpi->gpi_GInfo->gi_DrInfo );
	       }
	       break;

	    case  0x44:
	       /*
	       ** RETURN
	       **/
	    case  0x43:
	       /*
	       ** ENTER
	       **/
	       /*
	       ** Ignore if nothing selected (NMC:Added)
	       **/
	       if ( pmd->pmd_Selected != ~0 ) {

		  NotifyAttrChange( obj, gpi->gpi_GInfo, 0L, GA_ID, GADGET( obj )->GadgetID, PMB_MenuNumber, pmd->pmd_Selected, TAG_END );
		  /*
		  ** PMF_CHECKIT item?
		  **/
		  if ( pmd->pmd_MenuLabels[ pmd->pmd_Selected ].pm_Flags & PMF_CHECKIT ) {
		     /*
		     ** Toggle PMF_CHECKED bit.
		     **/
		     pmd->pmd_MenuLabels[ pmd->pmd_Selected ].pm_Flags ^= PMF_CHECKED;
		     /*
		     ** Do mutual exclusion.
		     **/
		     MutEx( pmd );
		  }
		  /*
		  ** Return GMR_VERIFY. Tell intuition to
		  ** discard the input event.
		  **/
		  rc = GMR_NOREUSE | GMR_VERIFY;
	       }
	       break;

	    case  0x45:
	       /*
	       ** ESC
	       **/
	       rc = GMR_NOREUSE;
	       break;
	 }
      }
   }
   return( rc );
}
REGFUNC_END

/*
** Go inactive...
**/
//STATIC ASM ULONG PMBClassGoInActive( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct gpGoInactive *gpgi )
STATIC ASM REGFUNC3(ULONG, PMBClassGoInActive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpGoInactive *, gpgi))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );

   /*
   ** Close up shop.
   **/
   if ( pmd->pmd_PopWindow ) {
      CloseWindow( pmd->pmd_PopWindow );
      pmd->pmd_PopWindow = NULL;
   }

   if ( pmd->pmd_PopFont ) {
      CloseFont( pmd->pmd_PopFont );
      pmd->pmd_PopFont   = NULL;
   }

   /*
   ** Clear ActivateGadget() flag.
   **/
   pmd->pmd_Flags &= ~PMDF_NOEVENT;

   return( DoSuperMethodA( cl, obj, ( Msg )gpgi ));
}
REGFUNC_END

/*
** Get an attribute.
**/
//STATIC ASM ULONG PMBClassGet( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opGet *opg )
STATIC ASM REGFUNC3(ULONG, PMBClassGet,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opGet *, opg))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );
   ULONG        rc = 1;

   switch (opg->opg_AttrID)
   {
   case PMB_MenuNumber:
      *(opg->opg_Storage) = pmd->pmd_Selected;
      break;
   case BT_PostRenderHighestClass:
      *(opg->opg_Storage) = (ULONG)cl;
      break;
   default:
      rc = DoSuperMethodA(cl, obj, (Msg)opg);
      break;
   };
   return rc;
}
REGFUNC_END

/*
** Dump the object.
**/
//STATIC ASM ULONG PMBClassDispose( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
STATIC ASM REGFUNC3(ULONG, PMBClassDispose,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );
   ULONG rc;

   /*
   ** If we have a vector image
   ** we dispose of it here.
   **/
   if ( pmd->pmd_VectorImage ) DisposeObject( pmd->pmd_VectorImage );
   if ( pmd->pmd_CheckMark   ) DisposeObject( pmd->pmd_CheckMark  );

   /*
   ** Free the allocated PopMenu
   ** structure array.
   **/
   if ( pmd->pmd_MenuLabels  ) FreeVec( pmd->pmd_MenuLabels );

   /*
   ** The rest is for the superclass.
   **/
   rc=DoSuperMethodA(cl,obj,msg);
   if(--instances==0)
   {
      CloseLibrary(BGUIBase);
      BGUIBase=NULL;
   }
   return rc;
}
REGFUNC_END

/*
 * They want our minimum dimensions.
 */
//STATIC ASM ULONG PMBClassDimensions( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct grmDimensions *dim )
STATIC ASM REGFUNC3(ULONG, PMBClassDimensions,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct grmDimensions *, dim))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );
   UWORD        mx, my;
   ULONG        frame, fw, fh;
   struct grmDimensions  dim1 = *dim;
   struct VectorItem *vit;

   if ( pmd->pmd_Image ) {
      /*
      ** Get minimum size of
      ** the image.
      **/
      mx = pmd->pmd_Image->Width;
      my = pmd->pmd_Image->Height;

      /*
      ** We enclose the image in the
      ** frame which means that we do
      ** the frame computation ourselves.
      **/
      dim1.grmd_Flags |= GDIMF_NO_FRAME;

      /*
      ** Let the superclass have a go at it.
      **/
      DoSuperMethodA( cl, obj, ( Msg )&dim1 );

      /*
      ** Add frame thickness.
      **/
      DoMethod( obj, OM_GET, BT_FrameObject, &frame );
      if ( frame ) {
	 DoMethod(( Object * )frame, OM_GET, FRM_FrameWidth,  &fw );
	 DoMethod(( Object * )frame, OM_GET, FRM_FrameHeight, &fh );
	 mx += fw << 1;
	 my += fh << 1;
      }

      /*
       * Set it.
       */
      SetDimensions( obj, &dim1, mx, my );
      return( 1L );
   } else if ( pmd->pmd_VectorImage ) {
      /*
      ** Get minimum vector sizes.
      **/
      DoMethod( pmd->pmd_VectorImage, OM_GET, VIT_VectorArray, ( ULONG * )&vit );

      /*
      ** Scan for the sizes.
      **/
      while( 1 ) {
	 /*
	 ** The scaling size is considered
	 ** to be the minimum object size.
	 **/
	 if ( vit->vi_Flags & VIF_SCALE ) {
	    mx = vit->vi_x;
	    my = vit->vi_y;
	    break;
	 }

	 /*
	 ** Last one?
	 **/
	 if ( vit->vi_Flags & VIF_LASTITEM )
	    goto doSuper;

	 /*
	 ** Next please...
	 **/
	 vit++;
      }

      /*
      ** First the superclass.
      **/
      dim1.grmd_Flags |= GDIMF_NO_FRAME;
      DoSuperMethodA( cl, obj, ( Msg )&dim1 );

      /*
      ** Now add our minimum sizes.
      **/
      SetDimensions( obj, &dim1, mx, my );
      return( 1L );
   }

   doSuper:
   /*
   ** Normal button.
   **/
   return( DoSuperMethodA( cl, obj, ( Msg )dim ));
}
REGFUNC_END

/*
** Do a command on the menu.
**/
//STATIC ASM ULONG PMBClassCommand( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct pmbmCommand *pc )
STATIC ASM REGFUNC3(ULONG, PMBClassCommand,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct pmbmCommand *, pc))
{
   PMD         *pmd = ( PMD * )INST_DATA( cl, obj );
   struct PopMenu    *label = &pmd->pmd_MenuLabels[ pc->pmbm_MenuNumber ];
   ULONG        rc = TRUE;

   /*
   ** What do they want?
   **/
   switch ( pc->MethodID ) {

      case  PMBM_CHECK_STATUS:
	 /*
	 ** TRUE means checked and FALSE means
	 ** not checked.
	 **/
	 rc = label->pm_Flags & PMF_CHECKED ? TRUE : FALSE;
	 break;

      case  PMBM_CHECK_MENU:
	 /*
	 ** Check the menu and do
	 ** mutual-exclusion.
	 **/
	 label->pm_Flags |= PMF_CHECKED;
	 MutEx( pmd );
	 break;

      case  PMBM_UNCHECK_MENU:
	 /*
	 ** Uncheck the menu.
	 **/
	 label->pm_Flags &= ~PMF_CHECKED;
	 break;

      case  PMBM_ENABLE_ITEM: /* NMC:Added */
	 /*
	 ** Enable the menuitem.
	 **/
	 label->pm_Flags &= ~PMF_DISABLED;
	 break;

      case  PMBM_DISABLE_ITEM:   /* NMC:Added */
	 /*
	 ** Disable the menuitem.
	 **/
	 label->pm_Flags |= PMF_DISABLED;
	 break;

      case  PMBM_ENABLE_STATUS:  /* NMC:Added */
	 /*
	 ** TRUE means enabled and FALSE means
	 ** disabled.
	 **/
	 rc = label->pm_Flags & PMF_DISABLED ? FALSE : TRUE;
	 break;
   }

   return( rc );
}
REGFUNC_END

METHOD(PMBClassKeyActive, Msg, msg)
{
   return WMKF_ACTIVATE;
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
   OM_NEW,               (FUNCPTR)PMBClassNew,
   OM_DISPOSE,           (FUNCPTR)PMBClassDispose,
   OM_GET,               (FUNCPTR)PMBClassGet,
   OM_SET,               (FUNCPTR)PMBClassSet,
   OM_UPDATE,            (FUNCPTR)PMBClassSet,
   GM_RENDER,            (FUNCPTR)PMBClassRender,
   GM_GOACTIVE,          (FUNCPTR)PMBClassGoActive,
   GM_HANDLEINPUT,       (FUNCPTR)PMBClassHandleInput,
   GM_GOINACTIVE,        (FUNCPTR)PMBClassGoInActive,
   GRM_DIMENSIONS,       (FUNCPTR)PMBClassDimensions,
   WM_KEYACTIVE,         (FUNCPTR)PMBClassKeyActive,
   PMBM_CHECK_STATUS,    (FUNCPTR)PMBClassCommand,
   PMBM_CHECK_MENU,      (FUNCPTR)PMBClassCommand,
   PMBM_UNCHECK_MENU,    (FUNCPTR)PMBClassCommand,
   PMBM_ENABLE_ITEM,     (FUNCPTR)PMBClassCommand,
   PMBM_DISABLE_ITEM,    (FUNCPTR)PMBClassCommand,
   PMBM_ENABLE_STATUS,   (FUNCPTR)PMBClassCommand,
   DF_END
};

/*
 * Initialize the class.
 */
SAVEDS ASM Class *BGUI_ClassInit(void)
{
   ClassBase = BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
			      CLASS_DFTable,        ClassFunc,
			      CLASS_ObjectSize,     sizeof(PMD),
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
