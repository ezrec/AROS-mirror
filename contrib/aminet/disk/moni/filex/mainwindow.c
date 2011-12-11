/*
 * MainWindow.c © 1993-1994 by Klaas Hermanns
 *
 * Funktionsübersicht:
 *
 * LONG OpenDisplay( void );						Display öffnen
 * BOOL CloseDisplay( void );						Display schließen
 * void ReopenDisplay( long );					Display neu öffnen
 * void DrawViewBoxes( void );					Anzeigerahmen zeichnen
 * void DoFontrequester( void );					Fontrequester behandeln
 * void SetScreenOwnPublic( void );				einem eigenen oder
 * BOOL DoPubScreenListReq( void );				Publicscreen aus Liste
 */

#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <exec/execbase.h>
#include <intuition/intuition.h>
#include <libraries/asl.h>
#include <libraries/reqtools.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/reqtools.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/gadtools.h>
#include <proto/wb.h>
#include <clib/alib_protos.h>

#include <string.h>
#include <stdio.h>
#endif

#include "filexstrings.h"
#include "filexstructs.h"
#include "filex.h"
#include "allprotos.h"

		/* Prototypes der lokalen Funktionen */

static int MySetupScreen( void );
static LONG OpenFileXWindow( struct DisplayInhalt *DI );

		/* Variablen */

struct Screen *Scr;
struct BitMap TempBM;
struct RastPort TempRP;
WORD TempBreite, TempHoehe;

WORD ScreenFlags = SF_DEFAULT_PUBLIC;		/* Screentyp, auf dem FileX liegt */
char *SollPublicScreenName;
APTR VisualInfo;			/* Zeiger auf das VisualInfo des Screens */
struct ScreenModeData ScreenModeData;

struct ColorSpec ScreenColors[] = {
	{~0, 0x00, 0x00, 0x00},
	 {1, 0x00, 0x00, 0x00},
	 {2, 0x00, 0x00, 0x00},
	 {3, 0x00, 0x00, 0x00},
	 {5, 0x00, 0x00, 0x00},
	 {6, 0x00, 0x00, 0x00},
	 {7, 0x00, 0x00, 0x00},
	 {8, 0x00, 0x00, 0x00},
	{~0, 0x00, 0x00, 0x00} };


UWORD __chip Stopwatch[(1 + 16 + 1) * 2] =
{
	0x0000,0x0000,

	0x0400,0x07C0,
	0x0000,0x07C0,
	0x0100,0x0380,
	0x0000,0x07E0,
	0x07C0,0x1FF8,
	0x1FF0,0x3FEC,
	0x3FF8,0x7FDE,
	0x3FF8,0x7FBE,
	0x7FFC,0xFF7F,
	0x7EFC,0xFFFF,
	0x7FFC,0xFFFF,
	0x3FF8,0x7FFE,
	0x3FF8,0x7FFE,
	0x1FF0,0x3FFC,
	0x07C0,0x1FF8,
	0x0000,0x07E0,

	0x0000,0x0000
};

void FreeTempBitMap( void )
{
	if( TempBM.Planes[ 0 ])
		FreeRaster( TempBM.Planes[ 0 ], TempBreite, TempHoehe );
}

BOOL AllocTempBitMap( void )
{
	WORD Breite, Hoehe = 8;
	struct DisplayInhalt *DI;
	PLANEPTR NewPlane;

	if( !Scr )
		return( TRUE );

	Breite = Scr->Width;

	DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
		if( DI->fhoehe > Hoehe )
			Hoehe = DI->fhoehe;
			
		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}

	if( !( NewPlane = ( PLANEPTR )AllocRaster( Breite, Hoehe )))
		return( FALSE );
	else
	{
		FreeTempBitMap();
		
		TempBM.Planes[ 0 ] = NewPlane;
		TempBreite = Breite;
		TempHoehe = Hoehe;

		InitBitMap( &TempBM, 1, Breite, Hoehe );

		InitRastPort( &TempRP );
		TempRP.BitMap = &TempBM;

		return( TRUE );
	}
}

	/* SetWaitPointer(struct Window *Window):
	 *
	 *	Set the busy wait mouse pointer.
	 */

void SetWaitPointer(struct Window *Window)
{
	if( Kick30 )
	{
		SetWindowPointer(Window,
			WA_BusyPointer,		TRUE,
			WA_PointerDelay,	TRUE,
		TAG_DONE);
	}
	else
		SetPointer(Window,Stopwatch,16,16,-6,0);
}

	/* ClrWaitPointer(struct Window *Window):
	 *
	 *	Remove the busy wait mouse pointer.
	 */

void ClrWaitPointer(struct Window *Window)
{
	if(Kick30)
		SetWindowPointer(Window,TAG_DONE);
	else
		ClearPointer(Window);
}

static int BlockCount;

	/* BlockMainWindow()
	 *
	 * Blocks the mainwindows for any inputs. 
	 */

void BlockMainWindow( void )
{
	struct DisplayInhalt *DI;

	BlockCount++;

	if( BlockCount > 1 )
		return;

	DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
		if( DI->Wnd )
		{
			DI->InvisibleRequest.RWindow = DI->Wnd;
	
				/* Install the wait mouse pointer. */
	
			SetWaitPointer( DI->Wnd );
	
				/* Install the requester if possible. */
	
			Request( &DI->InvisibleRequest, DI->Wnd );
		}

		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}
}

void ReleaseMainWindow( void )
{
	struct DisplayInhalt *DI;

	BlockCount--;

	if( BlockCount )
		return;

	DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
			/* Restore the mouse pointer. */
	
		ClrWaitPointer( DI->Wnd );
	
			/* End the requester activity. */
	
		EndRequest( &DI->InvisibleRequest, DI->Wnd );
	
			/* Reset it to zeroes. */
	
		memset( &DI->InvisibleRequest, 0, sizeof( struct Requester ));

		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}
}

void SetScreenModeData(	BOOL Valid,
								ULONG DisplayID,
								ULONG DisplayWidth,
								ULONG DisplayHeight,
								UWORD DisplayDepth,
								UWORD OverscanType,
								BOOL AutoScroll	)
{
	ScreenModeData.smd_Valid = Valid;
	ScreenModeData.smd_DisplayID = DisplayID;
	ScreenModeData.smd_DisplayWidth = DisplayWidth;
	ScreenModeData.smd_DisplayHeight = DisplayHeight;
	ScreenModeData.smd_DisplayDepth = DisplayDepth;
	ScreenModeData.smd_OverscanType = OverscanType;
	ScreenModeData.smd_AutoScroll = AutoScroll;
}

/*
 * int MySetupScreen( void )
 * 
 * Öffnet/Reserviert Screen und holt VisualInfo.
 */

static int MySetupScreen( void )
{
		/* Sicherheitsabfrage: Falls Screen schon bereit, direkt zurück */

	if(Scr) return(0);

		/* Screen bereitstellen/öffnen und Scr-Zeiger setzen */

	switch( ScreenFlags )
	{
		case SF_WORKBENCH:
			Scr = LockPubScreen("Workbench");
			break;
		case SF_PUBLIC:
			Scr = LockPubScreen( SollPublicScreenName );
			break;
		case SF_OWN_PUBLIC:
			{
				static char screenname[80];
				static UWORD pens[]={65535};
				char TempName[16];

				if(filexid==0) strcpy(TempName,"FILEX");
				else sprintf(TempName,"FILEX.%ld",filexid);

				sprintf(screenname,GetStr(MSG_SCREENTITLE),TempName);

				Scr = OpenScreenTags(0L,
					SA_Title,	screenname,
					SA_Type,		PUBLICSCREEN,
					SA_Colors,	&ScreenColors[0],
					SA_Pens,		&pens[0],
					SA_SysFont,	1,
					SA_PubName,		TempName,
					SA_Width,		ScreenModeData.smd_DisplayWidth,
					SA_Height,		ScreenModeData.smd_DisplayHeight,
					SA_Depth,		ScreenModeData.smd_DisplayDepth,
					SA_DisplayID,	ScreenModeData.smd_DisplayID,
					SA_Overscan,	ScreenModeData.smd_OverscanType,
					SA_AutoScroll,	ScreenModeData.smd_AutoScroll,
					TAG_DONE);

				if(!Scr)
				{
					Scr=LockPubScreen(NULL);
					ScreenFlags = SF_DEFAULT_PUBLIC;
				}
				else
				{
					PubScreenStatus(Scr,0);
				}
			}
			break;
	}

	if(!Scr)
	{
		Scr=LockPubScreen(NULL);
		ScreenFlags = SF_DEFAULT_PUBLIC;
	}

	if(!Scr) return(1);

		/* VisualInfo für Gadgets, Menus und Bevelboxes ermitteln */

	if(!(VisualInfo=GetVisualInfo(Scr, TAG_DONE))) return(2);

	if( !AllocTempBitMap()) return( 3 );
	
	return(0);
}


/*
 * LONG OpenDisplay()
 * 
 * Öffnet das Display, d.h. den Screen, alle Dateifenster
 */

LONG OpenDisplay( void )
{
	ULONG err;
	struct DisplayInhalt *DI;

	if(( err = MySetupScreen()))
	{
		MyRequest( MSG_INFO_GLOBAL_CANTSETUPSCREEN, err );
		CloseDisplay();
		return( 1L );
	}

		/* Alle Windows öffnen und neuzeichnen */

	DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
		if((err = OpenFileXWindow( DI )))
		{
			if( err < 10 )
				MyRequest( MSG_INFO_GLOBAL_CANTOPENWINDOW, err );
			else
				MyRequestNoLocale(( char * )err , 0 );

			CloseDisplay();
			return( 2L );
		}
		else
		{
			struct DisplayData *DD;

			DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

			while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
			{
				DD->Wnd = DI->Wnd;

				DD = ( struct DisplayData *)DD->Node.ln_Succ;
			}

			MakeDisplay( DI );
		}

		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}
	
	ScreenToFront( Scr );

	return(0L);
}

void CloseFileXWindow( struct DisplayInhalt *DI )
{
	if(DI->Wnd)
	{
		struct IntuiMessage	*IntuiMessage;
		struct Node		*Successor;
	
			/* Doppelt gemoppelt hält besser */

		if( DI->Wnd->MenuStrip )
			ClearMenuStrip( DI->Wnd );
	
			/* Falls wir ein AppWindow waren, dieses entfernen */

		if( DI->AppWnd )
		{
			RemoveAppWindow( DI->AppWnd );
			DI->AppWnd = 0;
		}

			/* Windowposition und -größe merken */

		DI->WindowLeft = DI->Wnd->LeftEdge;
		DI->WindowTop = DI->Wnd->TopEdge;

			/* CLoseWindowSafely */

		Forbid();
	
		IntuiMessage = ( struct IntuiMessage * )DI->Wnd->UserPort->mp_MsgList.lh_Head;
	
		while(( Successor = IntuiMessage->ExecMessage.mn_Node.ln_Succ ))
		{
			if( IntuiMessage->IDCMPWindow == DI->Wnd )
			{
				Remove(( struct Node * )IntuiMessage );
	
				ReplyMsg(( struct Message * )IntuiMessage );
			}
	
			IntuiMessage = ( struct IntuiMessage * ) Successor;
		}
	
		DI->Wnd->UserPort = NULL;
	
		ModifyIDCMP( DI->Wnd, 0 );
	
		Permit();

		CloseWindow(DI->Wnd);
		DI->Wnd = NULL;

			/* Alle Fensterzeiger der Displays löschen */
			/* Alle PropGadgets freigeben */

		{
			struct DisplayData *DD;

			DD = ( struct DisplayData * )DI->DisplayList.lh_Head;

			while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
			{
				DD->Wnd = 0;

				if( DD->PropGadget )
				{
					if( DD->Wnd )
						RemoveGadget( DD->Wnd, DD->PropGadget );
					
					DisposeObject((APTR) DD->PropGadget );
					DD->PropGadget = 0;
				}

				DD = ( struct DisplayData *)DD->Node.ln_Succ;
			}
		}
	}
}

/*
 * BOOL CloseDisplay( void )
 *
 * Schließt alle DateiFenster und gibt Screen frei
 */

BOOL CloseDisplay( void )
{
	struct DisplayInhalt *DI;

		/* Falls eigener Screen, prüfen, ob noch andere Windows */
		/* auf ihm offen sind. Falls ja, FALSE zurückliefern */
		/* Falls nein, Screen auf privat */

	if(Scr && (ScreenFlags == SF_OWN_PUBLIC))
	{
		struct List	*PubScreenList;
		struct PubScreenNode	*ScreenNode;

		PubScreenList = LockPubScreenList();

		for(ScreenNode = (struct PubScreenNode *)PubScreenList -> lh_Head ; ScreenNode -> psn_Node . ln_Succ ; ScreenNode = (struct PubScreenNode *)ScreenNode -> psn_Node . ln_Succ)
		{
			if(ScreenNode -> psn_Screen == Scr)
				break;
		}

		if(ScreenNode)
		{
			int EigeneFensterZahl = 0;

				/* Zahl der von FileX geöffneten Fenster berechnen */

			DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

			while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
			{
				if( DI->Wnd )
					EigeneFensterZahl++;

				DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
			}

			if(ScreenNode -> psn_VisitorCount != EigeneFensterZahl )
			{
				UnlockPubScreenList();

				DisplayLocaleText( MSG_INFO_GLOBAL_SCREENUSED );

				return(FALSE);
			}
			else
			{
				Forbid();

				UnlockPubScreenList();

				PubScreenStatus(Scr,PSNF_PRIVATE);

				Permit();
			}
		}
		else
			UnlockPubScreenList();
	}

		/* Alle Windows schliessen */

	DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
		CloseFileXWindow( DI );
		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}

	if(Scr)
	{
		MyFreeMenus();

			/* VisualInfo freigeben */

		if(VisualInfo)
		{
			FreeVisualInfo( VisualInfo);
			VisualInfo=NULL;
		}
	
			/* PublicScreen freigeben oder Screen schließen */

		switch( ScreenFlags )
		{
			case SF_WORKBENCH:
			case SF_DEFAULT_PUBLIC:
			case SF_PUBLIC:
				UnlockPubScreen(NULL,Scr);
				break;
			case SF_OWN_PUBLIC:
				CloseScreen(Scr);
				break;
		}

		Scr=NULL;
	}

	return(TRUE);
}

void SetSollPublicScreenName( char* Name )
{
	if( SollPublicScreenName )
		FreeVec( SollPublicScreenName );

	if(( SollPublicScreenName = AllocVec( strlen( Name ) + 1, MEMF_ANY )))
	{
		strcpy( SollPublicScreenName, Name );
	}
}

void SetScreenTypePublic( char *Name )
{
	SetSollPublicScreenName( Name );

		/* Falls Screen schon offen, neu öffnen */

	if( Scr ) ReopenDisplay( SF_PUBLIC );
	else ScreenFlags = SF_PUBLIC;
}

/*
 * void ReopenDisplay(long type)
 * 
 * Schließt das FileXFenster, setzt den Screentype und öffnet
 * das Display neu.
 */

void ReopenDisplay( long type )
{
	long premf = mainflags;

	if( mainflags & MF_CLIPCONV )
		CloseClipConv();

	if( mainflags & MF_CALC )
		CloseCalc();

	if( CloseDisplay())
	{
		ScreenFlags = type;

		if( OpenDisplay())
		{
			CloseDisplay();
			ScreenFlags = SF_WORKBENCH;
			OpenDisplay();
		}
	}

	if( premf & MF_CLIPCONV )
		OpenClipConv();

	if( premf & MF_CALC )
		OpenCalc();

	ActivateWindow( AktuDI->Wnd );
}

/*
 * static LONG OpenFileXWindow( void )
 * 
 * Öffnet das HauptFileXWindow.
 */

static LONG OpenFileXWindow( struct DisplayInhalt *DI )
{
		/* Sicherheitsabfrage: Falls das Window schon auf ist, direkt zurück */

	if( DI->Wnd ) return( 0L );

		/* Falls erstes Öffnen, Fenster unter Titelbar setzen */

	if( DI->WindowTop == -1 ) DI->WindowTop = Scr->WBorTop + Scr->RastPort.TxHeight + 1;

	if(!(DI->Wnd = OpenWindowTags(NULL,
				WA_Left,		DI->WindowLeft,
				WA_Top,		DI->WindowTop,
				WA_Title,	"FileX "VSTRING" © 1993-1994 Klaas Hermanns",
				WA_InnerWidth,	GetFitWidth( DI ),
				WA_InnerHeight,GetFitHeight( DI ),
				WA_IDCMP,		0,
				WA_Flags,		WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_SMART_REFRESH | WFLG_ACTIVATE | WFLG_NEWLOOKMENUS,
				WA_PubScreen,	Scr,
				WA_MaxWidth,	-1,
				WA_MaxHeight,	-1,
				TAG_DONE )))
		return(4L);

	SetAPen( DI->Wnd->RPort, 1 );

		/* UserData auf zugehörigen DisplayInhalt setzen */

	DI->Wnd->UserData = ( APTR )DI;

		/* Globalen Windowport eintragen */

	DI->Wnd->UserPort = WndPort;

#if 0
	ModifyIDCMP( DI->Wnd,-1 & (~(IDCMP_DELTAMOVE|IDCMP_INTUITICKS|IDCMP_MENUVERIFY))/*SCROLLERIDCMP | ARROWIDCMP | IDCMP_MENUPICK | IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_VANILLAKEY | IDCMP_REFRESHWINDOW | IDCMP_IDCMPUPDATE | IDCMP_NEWSIZE | IDCMP_CHANGEWINDOW | IDCMP_SIZEVERIFY*/);
#endif

	ModifyIDCMP(	DI->Wnd,
						IDCMP_ACTIVEWINDOW |
						IDCMP_GADGETUP |
						IDCMP_GADGETDOWN |
						IDCMP_MOUSEMOVE |
						IDCMP_MENUPICK  |
						IDCMP_CLOSEWINDOW  |
						IDCMP_RAWKEY  |
						IDCMP_VANILLAKEY  |
						IDCMP_IDCMPUPDATE  |
						IDCMP_NEWSIZE  |
						IDCMP_MOUSEBUTTONS |
						IDCMP_INTUITICKS |
						IDCMP_SIZEVERIFY );
		/* Versuchen ein AppWindow draus zu machen */

	DI->AppWnd = AddAppWindowA( 1, (ULONG)DI, DI->Wnd, AppPort, NULL );

		/* Window in den Prozess eintragen */

/*	(( struct Process * )FindTask( NULL ))->pr_WindowPtr = DI->Wnd;*/

		/* Menus setzen */

	if( !FileXMenus )
		SetNewMenus();
	else
		SetMenuStrip(DI->Wnd, FileXMenus);

		/* Font setzen */

	if( DI->TextFont ) SetFont( DI->Wnd->RPort, DI->TextFont );

	return(0L);
}

void MySetFont( struct TextAttr *ta, struct DisplayInhalt *DI )
{
	struct TextFont *OldTextFont = DI->TextFont;

	if(( DI->TextFont = OpenDiskFont( ta )))
	{
		if( OldTextFont ) CloseFont( OldTextFont );

		DI->fhoehe = DI->TextFont->tf_YSize;
		DI->fbase = DI->TextFont->tf_Baseline;
		DI->fbreite = DI->TextFont->tf_XSize;

		AllocTempBitMap();

		if( DI->Wnd )
		{
			SetFont( DI->Wnd->RPort, DI->TextFont);

			MakeDisplay( DI );
		}
	}
	else
	{
		DI->TextFont = OldTextFont;
		MyFullRequest( MSG_INFO_GLOBAL_CANTOPENFONT, MSG_GADGET_CONTINUE, ta->ta_Name, ta->ta_YSize );
	}

/*	Printf("MySetFont - TextFont %8lx OldFont %8lx\n", TextFont, OldTextFont );*/
}

void MySetFontNameSize( char *Name, UWORD YSize, struct DisplayInhalt *DI )
{
	struct TextAttr ta;

	ta.ta_Name = Name;
	ta.ta_YSize = YSize;
	ta.ta_Style = 0;
	ta.ta_Flags = 0;

	strcpy( DefaultFontName, Name );
	DefaultFontSize = YSize;

	if( DI )
		MySetFont( &ta, DI );
}

void DoFontrequester( struct DisplayInhalt *DI )
{
	if( UseAsl )
	{
		struct FontRequester *Asl_FontReq;

		if(( Asl_FontReq = (struct FontRequester *)
				AllocAslRequestTags(	ASL_FontRequest,
											ASLFO_TitleText, GetStr( MSG_WINDOWTITLE_SELECTFONT ),
											ASLFO_Flags, FOF_FIXEDWIDTHONLY,
											TAG_DONE )))
		{
			if( AslRequestTags( Asl_FontReq,	ASLFO_Window, DI->Wnd,
														ASLFO_InitialName, DI->TextFont->tf_Message.mn_Node.ln_Name,
														ASLFO_InitialSize, DI->TextFont->tf_YSize,
														TAG_DONE ))
			{
				MySetFont( &Asl_FontReq->fo_Attr, DI );
			}

			FreeAslRequest( Asl_FontReq );
		}
		else
			DisplayLocaleText( MSG_INFO_GLOBAL_CANTALLOCFONTREQ );
	}
	else
	{
		struct rtFontRequester *FontReq;

		if(( FontReq = rtAllocRequestA( RT_FONTREQ, NULL )))
		{
			rtChangeReqAttr( FontReq,	RTFO_FontName, DI->TextFont->tf_Message.mn_Node.ln_Name,
												RTFO_FontHeight, DI->TextFont->tf_YSize,
												RTFO_FontStyle, 0,
												RTFO_FontFlags, 0,
												TAG_DONE );

			if( rtFontRequest( FontReq, GetStr( MSG_WINDOWTITLE_SELECTFONT ), RTFO_Flags, FREQF_FIXEDWIDTH | FREQF_SCALE, AktuDI->Wnd ? RT_Window : TAG_IGNORE, AktuDI->Wnd,TAG_DONE ))
			{
				MySetFont( &FontReq->Attr, DI );
			}

			rtFreeRequest( FontReq );
		}
		else
			DisplayLocaleText( MSG_INFO_GLOBAL_CANTALLOCFONTREQ );
	}
}

void SetScreenOwnPublic( void )
{
	if( UseAsl && ( SysBase->LibNode.lib_Version >= 38 ))
	{
		struct ScreenModeRequester *Asl_ScreenModeReq;

		if(( Asl_ScreenModeReq = (struct ScreenModeRequester *)
				AllocAslRequestTags(	ASL_ScreenModeRequest,
											ASLFO_TitleText, GetStr( MSG_WINDOWTITLE_SCREENMODE ),
											TAG_DONE )))
		{
			if( AslRequestTags(	Asl_ScreenModeReq,
										ASLSM_Window, AktuDI->Wnd,
										ASLSM_DoWidth, TRUE,
										ASLSM_DoHeight, TRUE,
										ASLSM_DoDepth, TRUE,
										ASLSM_DoOverscanType, TRUE,
										ASLSM_DoAutoScroll, TRUE,
										ScreenModeData.smd_Valid ? TAG_IGNORE : TAG_DONE, 0,
										ASLSM_InitialDisplayID, ScreenModeData.smd_DisplayID,
										ASLSM_InitialAutoScroll, ScreenModeData.smd_AutoScroll,
										ASLSM_InitialDisplayWidth, ScreenModeData.smd_DisplayWidth,
										ASLSM_InitialDisplayHeight, ScreenModeData.smd_DisplayHeight,
										ASLSM_InitialDisplayDepth, ScreenModeData.smd_DisplayDepth,
										ASLSM_InitialOverscanType, ScreenModeData.smd_OverscanType,
										TAG_DONE ))
			{
				SetScreenModeData( TRUE, Asl_ScreenModeReq->sm_DisplayID, Asl_ScreenModeReq->sm_DisplayWidth, Asl_ScreenModeReq->sm_DisplayHeight, Asl_ScreenModeReq->sm_DisplayDepth, Asl_ScreenModeReq->sm_OverscanType, Asl_ScreenModeReq->sm_AutoScroll );
				ReopenDisplay( SF_OWN_PUBLIC );
			}

			FreeAslRequest( Asl_ScreenModeReq );
		}
		else
			DisplayLocaleText( MSG_INFO_GLOBAL_CANTALLOCSCREENMODEREQ );
	}
	else
	if( ReqToolsBase )
	{
		struct rtScreenModeRequester *screenmodereq;
	
			/* Den Screenmoderequester allokieren */
	
		if(( screenmodereq = rtAllocRequestA( RT_SCREENMODEREQ, NULL )))
		{
			if( ScreenModeData.smd_Valid )
				rtChangeReqAttr(screenmodereq,	RTSC_DisplayID, ScreenModeData.smd_DisplayID,
															RTSC_AutoScroll, ScreenModeData.smd_AutoScroll,
															RTSC_DisplayWidth, ScreenModeData.smd_DisplayWidth,
															RTSC_DisplayHeight, ScreenModeData.smd_DisplayHeight,
															RTSC_DisplayDepth, ScreenModeData.smd_DisplayDepth,
															RTSC_OverscanType, ScreenModeData.smd_OverscanType,
															TAG_END );
	
	
			if( rtScreenModeRequest( screenmodereq, GetStr( MSG_WINDOWTITLE_SCREENMODE ), RTSC_Flags, SCREQF_OVERSCANGAD | SCREQF_AUTOSCROLLGAD | SCREQF_SIZEGADS | SCREQF_DEPTHGAD | SCREQF_GUIMODES, TAG_DONE ))
			{
				SetScreenModeData( TRUE, screenmodereq->DisplayID, screenmodereq->DisplayWidth, screenmodereq->DisplayHeight, screenmodereq->DisplayDepth, screenmodereq->OverscanType, screenmodereq->AutoScroll );
				ReopenDisplay( SF_OWN_PUBLIC );
			}
	
			rtFreeRequest(screenmodereq);
		}	
		else
			DisplayLocaleText( MSG_INFO_GLOBAL_CANTALLOCSCREENMODEREQ );
	}
	else
		DisplayLocaleText( MSG_INFO_GLOBAL_NOSCREENMODEREQ );
}

/*
 * static void FreeList(struct List *list)
 *
 * Gibt die gesamte Screenliste frei.
 */

static void FreeList(struct List *list)
{
	struct Node *node;
	while(!(IsListEmpty(list)))
	{
		node=list->lh_Head;
		RemHead(list);
		FreeMem(node->ln_Name,strlen(node->ln_Name)+1);
		FreeMem(node,sizeof(struct Node));
	}
	FreeMem(list,sizeof(struct List));
}

/*
 * static BOOL AddNode(char *name,struct List *list)
 *
 * Fügt einen Namen in die Screenliste ein
 */

static BOOL AddNode(char *name,struct List *list)
{
	struct Node *node;
	if((node = AllocMem(sizeof(struct Node),MEMF_CLEAR)))
	{
	if((node->ln_Name = AllocMem(strlen(name)+1,0)))
	{
		strcpy(node->ln_Name,name);
		AddTail(list,node);
		return(TRUE);
	}
	else 
	{
		if(node)FreeMem(node,sizeof(struct Node));
		return(FALSE);
	}
	}
	return(FALSE);
}

static void __saveds PubScreenListCallBack( struct List *List, struct Node *node )
{
	if( node )
		SetScreenTypePublic( node->ln_Name );

	FreeList( List );
}

BOOL OpenPubScreenListReq( void )
{
	struct List *ScrList;
	char buffer[256];
	struct List *PubList;
	struct Node *node;
	int Anzahl = 0;

	if(!(ScrList = AllocMem(sizeof(struct List),MEMF_CLEAR)))return(FALSE);
	
	ScrList->lh_Head=(struct Node *)&ScrList->lh_Tail;
	ScrList->lh_Tail=0;
	ScrList->lh_TailPred=(struct Node *)&ScrList->lh_Head;

	PubList=LockPubScreenList();
	for(node=PubList->lh_Head;node->ln_Succ!=0;node=node->ln_Succ)
	{
		if(!(((struct PubScreenNode *)node)->psn_Flags&PSNF_PRIVATE))
		{
			strcpy(buffer,node->ln_Name);
			AddNode(buffer,ScrList);
			Anzahl++;
		}
	}
	UnlockPubScreenList();

	if(!Anzahl)
	{
		DisplayLocaleText( MSG_INFO_GLOBAL_NONOTPRIVATEPUBLICSCREENSFOUND );
		return( FALSE );
	}
	else
		return( OpenListReq( ScrList, GetStr(MSG_WINDOWTITLE_PUBLICSCREENLIST), &PubScreenListCallBack ));
}

/*******************************************************************/

void MyActivateWindow( struct DisplayInhalt *DI )
{
	AktuDI = DI;
	AktuDD = DI->AktuDD;
	SetDisplayCheckMarks();
}

void ActivateNextWindow( void )
{
	struct DisplayInhalt *DI;

	DI = ( struct DisplayInhalt *)AktuDI->Node.ln_Succ;

	if( DI == (struct DisplayInhalt *)&DisplayInhaltList.lh_Tail )
		DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	if( DI != AktuDI )
	{
		if( AktuDI->Wnd->Flags & WFLG_WINDOWACTIVE )
		{
			ActivateWindow( DI->Wnd );
			WindowToFront( DI->Wnd );
		}
		MyActivateWindow( DI );
	}
}

void ActivatePreviousWindow( void )
{
	struct DisplayInhalt *DI;

	DI = ( struct DisplayInhalt *)AktuDI->Node.ln_Pred;

	if( DI == (struct DisplayInhalt *)&DisplayInhaltList.lh_Head )
		DI = (struct DisplayInhalt *)DisplayInhaltList.lh_TailPred;

	if( DI != AktuDI )
	{
		if( AktuDI->Wnd->Flags & WFLG_WINDOWACTIVE )
		{
			ActivateWindow( DI->Wnd );
			WindowToFront( DI->Wnd );
		}
		MyActivateWindow( DI );
	}
}

BOOL OpenNewWindow( BOOL Split )
{
	BOOL Success = FALSE;
	struct DisplayInhalt *DI;
	struct FileData *FD;
	struct DisplayData *OldDD;

	DI = AllocDI();

	if( !Split )
	{
		FD = AllocFD( FD_FILE );
		strcpy( FD->Name, AktuDD->FD->Name );
		*(FilePart( FD->Name )) = 0;
	}
	else
		FD = AktuDD->FD;

	OldDD = AktuDD;

	if(( DI->AktuDD = AllocDD( DI, FD )))
	{
		if( Split )
		{
			AktuDD->CPos = OldDD->CPos;
			AktuDD->SPos = OldDD->SPos;
			AktuDD->DisplayForm = OldDD->DisplayForm;
			AktuDD->DisplaySpaces = OldDD->DisplaySpaces;
		}

		MyActivateWindow( DI );

		if( 0 == OpenFileXWindow( DI ))
		{
			AktuDD->Wnd = DI->Wnd;

			MakeDisplay( DI );

			Success = TRUE;
		}
	}

	if( !Success )
	{
		FreeDI( DI );
		CloseFileXWindow( DI );
	}

	return( Success );
}

char *GetOwnScreenName( void )
{
	struct List *PubList;
	struct Node *node;
	char *name = NULL;

	PubList=LockPubScreenList();
	for(node=PubList->lh_Head;node->ln_Succ!=0;node=node->ln_Succ)
	{
		if((((struct PubScreenNode *)node)->psn_Screen == Scr ))
		{
			name =  node->ln_Name;
			break;
		}
	}
	UnlockPubScreenList();

	return( name );
}
