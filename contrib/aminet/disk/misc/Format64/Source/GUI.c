/*
 *  Source generated with GadToolsBox V1.3
 *  which is (c) Copyright 1991,92 Jaba Development
 */

#define GT_Underscore	(GT_TagBase+64)

#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/alib.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <string.h>

#include "GUI.h"

struct Screen	     *Scr = NULL;
APTR		      VisualInfo = NULL;
struct Window	     *PrepWnd = NULL;
struct Window	     *StatusWnd = NULL;
struct Gadget	     *PrepGList = NULL;
struct Gadget	     *StatusGList = NULL;
struct Gadget	     *PrepGadgets[8];
struct Gadget	     *StatusGadgets[2];
UWORD		      PrepLeft = 0;
UWORD		      PrepTop = 23;
UWORD		      PrepWidth = 321;
UWORD		      PrepHeight = 77;
UWORD		      StatusLeft = 0;
UWORD		      StatusTop = 23;
UWORD		      StatusWidth = 318;
UWORD		      StatusHeight = 69;
UBYTE		     *PrepWdt = (UBYTE *)"NewFormat";
UBYTE		     *StatusWdt = (UBYTE *)"Status";
extern struct GfxBase *GfxBase;
struct TextAttr      *Font, Attr;
UWORD		      FontX, FontY;
UWORD		      OffX, OffY;

Rect box;

struct IntuiText  StatusIText[] = {
    1, 0, JAM1,3, 2, NULL, (UBYTE *)"Formatting Drive...", &StatusIText[1],
    1, 0, JAM1,3, 12, NULL, (UBYTE *)"Status:", NULL };

static UWORD ComputeX( UWORD value )
{
    return(( UWORD )(( FontX * value ) / 8 ));
}

static UWORD ComputeY( UWORD value )
{
    return(( UWORD )(( FontY * value ) / 8 ));
}

static void ComputeFont( UWORD width, UWORD height )
{
    Font = &Attr;
    Font->ta_Name = GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name;
    Font->ta_YSize = FontY = GfxBase->DefaultFont->tf_YSize;
    FontX = GfxBase->DefaultFont->tf_XSize;

    OffY = Scr->Font->ta_YSize + Scr->WBorTop + 1;
    OffX = Scr->WBorLeft;

    if ( width && height ) {
	if (( ComputeX( width ) + OffX + Scr->WBorRight ) > Scr->Width )
	    goto UseTopaz;
	if (( ComputeY( height ) + OffY + Scr->WBorBottom ) > Scr->Height )
	    goto UseTopaz;
    }
    return;

UseTopaz:
    Font->ta_Name = (STRPTR)"topaz.font";
    FontX = FontY = Font->ta_YSize = 8;
}

int SetupScreen( void )
{
    if ( ! ( Scr = LockPubScreen( NULL )))
	return( 1L );

    ComputeFont( 0L, 0L );

    if ( ! ( VisualInfo = GetVisualInfo( Scr, TAG_DONE )))
	return( 2L );

    return( 0L );
}

void CloseDownScreen( void )
{
    if ( VisualInfo ) {
	FreeVisualInfo( VisualInfo );
	VisualInfo = NULL;
    }

    if ( Scr        ) {
	UnlockPubScreen( NULL, Scr );
	Scr = NULL;
    }
}

int OpenPrepWindow( char *volumeName )
{
    struct NewGadget	 ng;
    struct Gadget	*g;
    UWORD		wleft = PrepLeft, wtop = PrepTop, ww, wh;

    ComputeFont( PrepWidth, PrepHeight );

    ww = ComputeX( PrepWidth );
    wh = ComputeY( PrepHeight );

    if (( wleft + ww + OffX + Scr->WBorRight ) > Scr->Width ) wleft = Scr->Width - ww;
    if (( wtop + wh + OffY + Scr->WBorBottom ) > Scr->Height ) wtop = Scr->Height - wh;

    if ( ! ( g = CreateContext( &PrepGList )))
	return( 1L );

    ng.ng_LeftEdge	  =    OffX + ComputeX( 5 );
    ng.ng_TopEdge	  =    OffY + ComputeY( 33 );
    ng.ng_Width 	  =    ComputeX( 26 );
    ng.ng_Height	  =    ComputeY( 11 );
    ng.ng_GadgetText	  =    (UBYTE *)"_Fast File System";
    ng.ng_TextAttr	  =    Font;
    ng.ng_GadgetID	  =    GD_FFSGadget;
    ng.ng_Flags 	  =    PLACETEXT_RIGHT;
    ng.ng_VisualInfo	  =    VisualInfo;

    g = CreateGadget( CHECKBOX_KIND, g, &ng, GTCB_Checked, FALSE, GT_Underscore, '_', TAG_DONE );

    PrepGadgets[ 0 ] = g;

    ng.ng_LeftEdge	  =    OffX + ComputeX( 68 );
    ng.ng_TopEdge	  =    OffY + ComputeY( 16 );
    ng.ng_Width 	  =    ComputeX( 249 );
    ng.ng_Height	  =    ComputeY( 13 );
    ng.ng_GadgetText	  =    (UBYTE *)"_Name:";
    ng.ng_GadgetID	  =    GD_NameGadget;
    ng.ng_Flags 	  =    PLACETEXT_LEFT;

    g = CreateGadget( STRING_KIND, g, &ng, GTST_String, "Empty", GTST_MaxChars, 30, GT_Underscore, '_', TAG_DONE );

    PrepGadgets[ 1 ] = g;

    ng.ng_LeftEdge	  =    OffX + ComputeX( 188 );
    ng.ng_TopEdge	  =    OffY + ComputeY( 46 );
    ng.ng_GadgetText	  =    (UBYTE *)"C_reate Icons";
    ng.ng_GadgetID	  =    GD_IconGadget;
    ng.ng_Flags 	  =    PLACETEXT_RIGHT;

    g = CreateGadget( CHECKBOX_KIND, g, &ng, GTCB_Checked, TRUE, GT_Underscore, '_', TAG_DONE );

    PrepGadgets[ 2 ] = g;

    ng.ng_TopEdge	  =    OffY + ComputeY( 33 );
    ng.ng_GadgetText	  =    (UBYTE *)"_Quick Format";
    ng.ng_GadgetID	  =    GD_QuickFmtGadget;

    g = CreateGadget( CHECKBOX_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

    PrepGadgets[ 3 ] = g;

    ng.ng_LeftEdge	  =    OffX + ComputeX( 5 );
    ng.ng_TopEdge	  =    OffY + ComputeY( 46 );
    ng.ng_GadgetText	  =    (UBYTE *)"_Verify Format";
    ng.ng_GadgetID	  =    GD_VerifyGadget;

    g = CreateGadget( CHECKBOX_KIND, g, &ng, GTCB_Checked, TRUE, GT_Underscore, '_', TAG_DONE );

    PrepGadgets[ 4 ] = g;

    ng.ng_TopEdge	  =    OffY + ComputeY( 62 );
    ng.ng_Width 	  =    ComputeX( 56 );
    ng.ng_Height	  =    ComputeY( 12 );
    ng.ng_GadgetText	  =    (UBYTE *)"_OK";
    ng.ng_GadgetID	  =    GD_OKGadget;
    ng.ng_Flags 	  =    PLACETEXT_IN;

    g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

    PrepGadgets[ 5 ] = g;

    ng.ng_LeftEdge	  =    OffX + ComputeX( 260 );
    ng.ng_GadgetText	  =    (UBYTE *)"_Cancel";
    ng.ng_GadgetID	  =    GD_CancelGadget;

    g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

    PrepGadgets[ 6 ] = g;

    ng.ng_LeftEdge	  =    OffX + ComputeX( 69 );
    ng.ng_TopEdge	  =    OffY + ComputeY( 2 );
    ng.ng_Width 	  =    ComputeX( 248 );
    ng.ng_GadgetText	  =    (UBYTE *)"Device:";
    ng.ng_GadgetID	  =    GD_DeviceGad;
    ng.ng_Flags 	  =    PLACETEXT_LEFT;

    g = CreateGadget( TEXT_KIND, g, &ng, GTTX_Text, volumeName, GTTX_Border, TRUE, TAG_DONE );

    PrepGadgets[ 7 ] = g;

    if ( ! g )
	return( 2L );

    if ( ! ( PrepWnd = OpenWindowTags( NULL,
		    WA_Left,	      wleft,
		    WA_Top,	      wtop,
		    WA_Width,	      ww + OffX + Scr->WBorRight,
		    WA_Height,	      wh + OffY + Scr->WBorBottom,
		    WA_IDCMP,	      IDCMP_VANILLAKEY|IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|BUTTONIDCMP|CHECKBOXIDCMP,
		    WA_Flags,	      WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH|WFLG_RMBTRAP,
		    WA_Gadgets,       PrepGList,
		    WA_Title,	      PrepWdt,
		    WA_ScreenTitle,   "NewFormat V1.00 ©1992 by Dave Schreiber",
		    WA_PubScreen,     Scr,
		    TAG_DONE )))
	return( 4L );

    GT_RefreshWindow( PrepWnd, NULL );

    return( 0L );
}

void ClosePrepWindow( void )
{
    if ( PrepWnd        ) {
	CloseWindow( PrepWnd );
	PrepWnd = NULL;
    }

    if ( PrepGList      ) {
	FreeGadgets( PrepGList );
	PrepGList = NULL;
    }
}

void StatusRender( void )
{
    ULONG leftEdge[2],topEdge[2];
    ComputeFont( StatusWidth, StatusHeight );

    leftEdge[0]=StatusIText[0].LeftEdge;
    leftEdge[1]=StatusIText[1].LeftEdge;
    topEdge[0]=StatusIText[0].TopEdge;
    topEdge[1]=StatusIText[1].TopEdge;

    StatusIText[0].LeftEdge=ComputeX(StatusIText[0].LeftEdge);
    StatusIText[0].TopEdge=ComputeY(StatusIText[0].TopEdge);
    StatusIText[1].LeftEdge=ComputeX(StatusIText[1].LeftEdge);
    StatusIText[1].TopEdge=ComputeY(StatusIText[1].TopEdge);

    PrintIText(StatusWnd->RPort,StatusIText,OffX,OffY);

    StatusIText[0].LeftEdge=leftEdge[0];
    StatusIText[0].TopEdge=topEdge[0];
    StatusIText[1].LeftEdge=leftEdge[1];
    StatusIText[1].TopEdge=topEdge[1];

    DrawBevelBox( StatusWnd->RPort, OffX + ComputeX( 3 ),
	OffY + ComputeY( 38 ),
	ComputeX( 312 ),
	ComputeY( 13 ),
	GT_VisualInfo, VisualInfo, GTBB_Recessed, TRUE, TAG_DONE );

    box.left=OffX+ComputeX(3);
    box.top=OffY+ComputeY(38);
    box.width=ComputeX(312);
    box.height=ComputeY(13);
    box.center=box.left+box.width/2;

}

int OpenStatusWindow(char *bufPointer)
{
    struct NewGadget	 ng;
    struct Gadget	*g;
    UWORD		wleft = StatusLeft, wtop = StatusTop, ww, wh;

    ComputeFont( StatusWidth, StatusHeight );

    ww = ComputeX( StatusWidth );
    wh = ComputeY( StatusHeight );

    if (( wleft + ww + OffX + Scr->WBorRight ) > Scr->Width ) wleft = Scr->Width - ww;
    if (( wtop + wh + OffY + Scr->WBorBottom ) > Scr->Height ) wtop = Scr->Height - wh;

    if ( ! ( g = CreateContext( &StatusGList )))
	return( 1L );

    ng.ng_LeftEdge	  =    OffX + ComputeX( 3 );
    ng.ng_TopEdge	  =    OffY + ComputeY( 22 );
    ng.ng_Width 	  =    ComputeX( 312 );
    ng.ng_Height	  =    ComputeY( 13 );
    ng.ng_GadgetText	  =    NULL;
    ng.ng_TextAttr	  =    Font;
    ng.ng_GadgetID	  =    GD_StatusGadget;
    ng.ng_Flags 	  =    0;
    ng.ng_VisualInfo	  =    VisualInfo;

    g = CreateGadget( TEXT_KIND, g, &ng, GTTX_Border, TRUE, TAG_DONE );

    StatusGadgets[ 0 ] = g;

    ng.ng_LeftEdge	  =    OffX + ComputeX( 125 );
    ng.ng_TopEdge	  =    OffY + ComputeY( 54 );
    ng.ng_Width 	  =    ComputeX( 69 );
    ng.ng_GadgetText	  =    (UBYTE *)"_Stop";
    ng.ng_GadgetID	  =    GD_StopGadget;
    ng.ng_Flags 	  =    PLACETEXT_IN;

    g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

    StatusGadgets[ 1 ] = g;

    if ( ! g )
	return( 2L );

    if ( ! ( StatusWnd = OpenWindowTags( NULL,
		    WA_Left,	      wleft,
		    WA_Top,	      wtop,
		    WA_Width,	      ww + OffX + Scr->WBorRight,
		    WA_Height,	      wh + OffY + Scr->WBorBottom,
		    WA_IDCMP,	      IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_VANILLAKEY|BUTTONIDCMP,
		    WA_Flags,	      WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_SMART_REFRESH,
		    WA_Gadgets,       StatusGList,
		    WA_Title,	      StatusWdt,
		    WA_ScreenTitle,   "NewFormat V1.00 ©1992 by Dave Schreiber",
		    WA_PubScreen,     Scr,
		    TAG_DONE )))
	return( 4L );

    GT_RefreshWindow( StatusWnd, NULL );

    StatusRender();

    return( 0L );
}

void CloseStatusWindow( void )
{
    if ( StatusWnd        ) {
	CloseWindow( StatusWnd );
	StatusWnd = NULL;
    }

    if ( StatusGList      ) {
	FreeGadgets( StatusGList );
	StatusGList = NULL;
    }
}

