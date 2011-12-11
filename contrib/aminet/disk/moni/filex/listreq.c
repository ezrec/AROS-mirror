#ifdef USE_PROTO_INCLUDES

#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <clib/alib_protos.h>

#endif

#include "filexstructs.h"
#include "filexstrings.h"
#include "allprotos.h"
#include "windowinfo.h"

static struct WindowInfo *LRWI;
static struct Window *LRWnd;
static struct Gadget *LRGList;
static struct Gadget *LRGadgets[4];
static long LRLeft = -1, LRTop = -1, LRWidth = -1, LRHeight = -1;
static struct List *LRList;
static int LRCurrent, LREntries;
static void ( * LRFunc )( struct List *, struct Node * );

static long MakeLRGadgets( UWORD Breite, UWORD Height )
{
	struct NewGadget ng;
	struct Gadget *g;

	if( LRGList )
	{
		RemoveGList( LRWnd, LRGList, -1 );

		FreeGadgets( LRGList );
		LRGList = NULL;
	}

	if(! (g = CreateContext(&LRGList)))
		return(1L);

	ng.ng_VisualInfo = VisualInfo;
	ng.ng_TextAttr = &LRWI->TA;
	ng.ng_GadgetID = 0;
	ng.ng_LeftEdge = Scr->WBorLeft + BORDERX + BEVELX + BUTTONX;
	ng.ng_Width = Breite - 2 * BORDERX - 2 * BEVELX - 2 * INNERX;
	ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BORDERY + BEVELY + BUTTONY;


	ng.ng_GadgetText = 0;
	ng.ng_Height = Height - LRWI->TF->tf_YSize - 6 - 2 * BORDERY - 2 * BEVELY - 2 * BUTTONY - INNERGADGETY;

	LRGadgets[0] = g = CreateGadget( LISTVIEW_KIND, g, &ng, GTLV_Labels, LRList, GTLV_ShowSelected, NULL, GTLV_Selected, LRCurrent, TAG_DONE );
	ng.ng_GadgetID++;


	ng.ng_LeftEdge = Scr->WBorLeft + BORDERX;
	ng.ng_Width = ( Breite - INNERGADGETX - 2 * BORDERX ) / 2;
	ng.ng_Flags = PLACETEXT_IN;
	ng.ng_GadgetText = GetStr( MSG_GADGET_OK );
	ng.ng_Height = LRWI->TF->tf_YSize + 6;
	ng.ng_TopEdge = Height + LRWnd->BorderTop - ng.ng_Height - BORDERY;
	LRGadgets[1] = g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );
	ng.ng_GadgetID++;


	ng.ng_LeftEdge = Breite - ng.ng_Width;
	ng.ng_GadgetText =  GetStr( MSG_GADGET_CANCEL );
	LRGadgets[2] = g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

	if(!g) return(2L);

	if( LRWnd )
	{
		SetzeMuster( LRWnd );

		MyFilledDrawBevelBox(LRWnd->RPort,	LRWnd->BorderLeft + BORDERX,
															LRWnd->BorderTop + BORDERY,
															LRWnd->Width - LRWnd->BorderRight - LRWnd->BorderLeft - BORDERX * 2,
															LRWnd->Height - LRWnd->BorderBottom - LRWnd->BorderTop - 2 * BORDERY - LRWI->TF->tf_YSize - 6 - BUTTONY,
															TRUE);
	
		AddGList( LRWnd, LRGList, -1, -1, 0);

		RefreshGadgets(LRGList, LRWnd, 0);
		GT_RefreshWindow( LRWnd, NULL );
	}

    	return 0;
}

void CloseLR( void )
{
	MyRemoveSignal( 1L << LRWnd->UserPort->mp_SigBit );
	ReleaseMainWindow();

	mainflags &= ~MF_LISTREQ;

	if( LRWnd )
	{
		LRLeft = LRWnd->LeftEdge;
		LRTop = LRWnd->TopEdge;
		LRWidth = LRWnd->Width - LRWnd->BorderLeft - LRWnd->BorderRight;
		LRHeight = LRWnd->Height - LRWnd->BorderTop - LRWnd->BorderBottom;

		CloseWindow( LRWnd );
		LRWnd = NULL;
	}

	if( LRGList )
	{
		FreeGadgets( LRGList );
		LRGList = NULL;
	}

	if( LRWI )
	{
		NewCalcCleanup( LRWI );
		LRWI = NULL;
	}
}

static void DoLRWndMsg( void )
{
	BOOL CloseMe = FALSE;
	struct IntuiMessage	*m, Msg;
	struct Gadget *gad;

	while(( m = GT_GetIMsg( LRWnd->UserPort )))
	{
		CopyMem((char *)m, (char *)&Msg, (long)sizeof(struct IntuiMessage));

		GT_ReplyIMsg(m);

		gad = (struct Gadget *) Msg.IAddress;

		switch( Msg.Class )
		{
			case	IDCMP_NEWSIZE:
				{
					MakeLRGadgets( LRWnd->Width - LRWnd->BorderLeft - LRWnd->BorderRight, LRWnd->Height - LRWnd->BorderTop - LRWnd->BorderBottom );
	
					RefreshWindowFrame( LRWnd );
				}
				break;

			case	IDCMP_RAWKEY:
				switch( Msg.Code )
				{
					case CURSORUP:
						if( LRCurrent > 0 )
						{
							LRCurrent--;

							GT_SetGadgetAttrs( LRGadgets[ 0 ], LRWnd, NULL,
								GTLV_Selected, LRCurrent,
								GTLV_Top, LRCurrent,
								GTLV_MakeVisible, LRCurrent,
								TAG_DONE);
						}
						break;
					case CURSORDOWN:
						if( LRCurrent < LREntries - 1 )
						{
							LRCurrent++;

							GT_SetGadgetAttrs( LRGadgets[ 0 ], LRWnd, NULL,
								GTLV_Selected, LRCurrent,
								GTLV_Top, LRCurrent,
								GTLV_MakeVisible, LRCurrent,
								TAG_DONE);
						}
						break;
				}
				break;

			case	IDCMP_VANILLAKEY:
				switch( Msg.Code )
				{
					case 27:
						CloseMe = TRUE;
						LRCurrent = -1;
						break;
					case 13:
						CloseMe = TRUE;
						break;
					default:
						if( ToUpper( Msg.Code ) == ToUpper( GetUnderlinedKey( GetStr( MSG_GADGET_OK ))))
							CloseMe = TRUE;
						else
						if( ToUpper( Msg.Code ) == ToUpper( GetUnderlinedKey( GetStr( MSG_GADGET_CANCEL ))))
						{
							CloseMe = TRUE;
							LRCurrent = -1;
						}
						break;
				}
				break;

			case	IDCMP_CLOSEWINDOW:
				CloseMe = TRUE;
				break;

			case	IDCMP_GADGETUP:
				switch( gad->GadgetID )
				{	
					case 0:
						if( LRCurrent == Msg.Code )
						{
							CloseMe = TRUE;
						}
						else
							LRCurrent = Msg.Code;
						break;

					case 1:
						CloseMe = TRUE;
						break;

					case 2:
						LRCurrent = -1;
						CloseMe = TRUE;
						break;
				}
		}
	}

	if( CloseMe )
	{
		CloseLR();

		if( LRCurrent != -1 )
		{
			struct Node *node;
			int k;

			node = LRList->lh_Head;

			for( k = 0; k < LRCurrent; k++ )
				node = node->ln_Succ;

			Printf("LRCu=%ld %s\n", LRCurrent, node ->ln_Name);

			LRFunc( LRList, node );
		}
		else
			LRFunc( LRList, 0 );
	}
}

BOOL OpenListReq( struct List *List, char *Title, void ( * Func )())
{
	if( LRWnd )
	{
		ActivateWindow( LRWnd );
		return( TRUE );
	}

	if(LRLeft == -1) LRLeft = AktuDI->Wnd->LeftEdge + WNDXOPENOFF;
	if(LRTop == -1) LRTop = AktuDI->Wnd->TopEdge + WNDYOPENOFF;

	LRCurrent = 0;

	LRFunc = Func;

	{
		struct Node *n;

		LREntries = 0;

		n = List->lh_Head;

		while( n->ln_Succ != List->lh_Tail )
		{
			n = n->ln_Succ;
			LREntries++;
		}
	}

	if(( LRWI = NewCalcSetup( Scr )))
	{
		UWORD MinWidth, MinHeight;

		MinWidth =	MyTextLength( &LRWI->RP, GetStr( MSG_GADGET_OK )) + 16 +
						MyTextLength( &LRWI->RP, GetStr( MSG_GADGET_CANCEL )) + 16 +
						INNERGADGETX;

		MinWidth += 2 * BORDERX + 2 * BEVELX + 2 * INNERX;

		if( LRWidth == -1 )
			LRWidth = 30 * LRWI->AverageCharWidth;

		if( LRWidth < MinWidth )
			LRWidth = MinWidth;

		MinHeight = (LRWI->TF->tf_YSize + 6) + 2 * BORDERY + 2 * BEVELY + 2 * INNERY;

		MinHeight += LRWI->TF->tf_YSize * 4 + LISTVIEWBORDERY + INNERGADGETY;
		if( !Kick30 ) MinHeight += LISTVIEWBORDERY20;

		if(( LRHeight == -1 ) || ( LRHeight < MinHeight ))
			LRHeight = MinHeight;

#ifndef WFLG_SIZEBOTTOM
#define WFLG_SIZEBOTTOM WFLG_SIZEBBOTTOM
#endif

		if ( ! ( LRWnd = OpenWindowTags( NULL,
			WA_Left,	LRLeft,
			WA_Top,		LRTop,
			WA_InnerWidth,	LRWidth,
			WA_InnerHeight,	LRHeight,
			WA_MinWidth,	MinWidth,
			WA_MinHeight,	MinHeight,
			WA_MaxWidth,	-1,
			WA_MaxHeight,	-1,
			WA_IDCMP,	BUTTONIDCMP | INTEGERIDCMP | TEXTIDCMP | IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_VANILLAKEY | IDCMP_REFRESHWINDOW | IDCMP_NEWSIZE,
			WA_Flags,	WFLG_SIZEGADGET | WFLG_SIZEBOTTOM | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_SMART_REFRESH | WFLG_ACTIVATE | WFLG_RMBTRAP,
			WA_Title,	Title,
			WA_ScreenTitle,	"FileX '94",
			WA_PubScreen,	Scr,
			TAG_DONE )))
			return(3L);

		BlockMainWindow();

		LRList = List;

		MakeLRGadgets( LRWidth, LRHeight );

		mainflags |= MF_LISTREQ;

		MyAddSignal( 1L << LRWnd->UserPort->mp_SigBit, &DoLRWndMsg );
	}
	
	return TRUE; /* stegerg: CHECKME, return was missing completely */
}

