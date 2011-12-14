#ifdef USE_PROTO_INCLUDES

#include <intuition/sghooks.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>

#include <SDI/SDI_hook.h>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#endif

#include "filexstructs.h"
#include "filexstrings.h"
#include "allprotos.h"
#include "windowinfo.h"

enum { CM_BIN = 0, CM_DEC, CM_HEX, CM_OCT, CM_RAW, CM_TEXT };
enum { CO_GLEICH = 0, CO_ADD, CO_SUB, CO_DIV, CO_MUL };

static BOOL IsCharInMode( char Zeichen, UWORD Mode )
{
	switch( Mode )
	{
		case CM_BIN:
			return(( BOOL )(( Zeichen == '0' ) || ( Zeichen == '1' )));
			break;

		case CM_DEC:
			return(( BOOL )isdigit( Zeichen ));
			break;

		case CM_HEX:
			return(( BOOL )isxdigit( Zeichen ));
			break;

		case CM_OCT:
			return(( BOOL )(( Zeichen >= '0' ) && ( Zeichen <= '7' )));
			break;
	}

    	return FALSE;
}

static UBYTE *CalcTasten[] =
{
	"D", "E", "F", ">",
	"A", "B", "C", "<",
	"7", "8", "9", "/",
	"4", "5", "6", "*",
	"1", "2", "3", "-",
	"0", "±", "=", "+"
};

static CONST_STRPTR CalcModes[] =
{
	MSG_GADGET_CALC_CYCLE_BIN,
	MSG_GADGET_CALC_CYCLE_DEC,
	MSG_GADGET_CALC_CYCLE_HEX,
	MSG_GADGET_CALC_CYCLE_OCT,
	0
};

static BOOL Localized = FALSE;

static struct WindowInfo *CalcWI;
static struct Window *CalcWnd;
static struct Gadget *CalcGList;
static struct Gadget *CalcGadgets[27];
static long CalcLeft = -1, CalcTop = -1, CalcWidth = -1;

static UWORD CalcMode = CM_DEC, CalcOp = CO_GLEICH;
static long CalcDisplayWert, LastCalcWert;
static BOOL NewCalcWert = FALSE;

static long DoCalcOp( UWORD Op, long eins, long zwei )
{
	switch( Op )
	{
		case CO_ADD:
			return( eins + zwei );
			break;
		case CO_SUB:
			return( eins - zwei );
			break;
		case CO_MUL:
			return( eins * zwei );
			break;
		case CO_DIV:
			if( zwei )
				return( eins / zwei );
			else
			{
				DisplayLocaleText( MSG_INFO_CALC_DIVISONBYZERO );
				return( 0 );
			}
			break;
	}
	
	return 0;
}

HOOKPROTO(CalcHookFunc, ULONG, struct SGWork *sgw, Msg msg)
{

/*	struct InputEvent *ie;*/
	ULONG return_code = ~0;

	if( msg->MethodID == SGH_KEY )
	{
/*		ie = sgw->IEvent;


{
	void kprintf(UBYTE *fmt,...);

	kprintf("*msg = %ld\n", *msg );
	kprintf("Work:%s:\n", sgw->WorkBuffer );
	kprintf("Prev:%s:\n", sgw->PrevBuffer );
	kprintf("Modes:%lx:\n", sgw->Modes );
	kprintf("code:%lx:\n", sgw->Code);
	kprintf("Prev:%s:\n", sgw->PrevBuffer );
	kprintf("BufferPos:%lx:\n", sgw->BufferPos );
	kprintf("NumChars:%lx:\n", sgw->NumChars );
	kprintf("Actions:%lx:\n", sgw->Actions );
	kprintf("EditOp:%lx:\n", sgw->EditOp);

	kprintf("IE Class:%lx:\n", ie->ie_Class);
	kprintf("IE Qualifier:%lx:\n", ie->ie_Qualifier);
	kprintf("IE Code:%lx:\n", ie->ie_Code);
	kprintf("**************************\n");
}
*/

		switch( sgw->EditOp )
		{
			case EO_INSERTCHAR:
/*				if( ie->ie_Class == IECLASS_RAWKEY )*/
				{
					if(( !IsCharInMode( sgw->Code, CalcMode ))	||
						( sgw->WorkBuffer[0] == '0' ))
						sgw->Actions = SGA_BEEP;
				}
				break;

			case EO_NOOP:
				break;

			case EO_ENTER:
				break;

			case EO_BIGCHANGE:
				{
					int k;

					for( k = 0; k < strlen( sgw->WorkBuffer  ); k++ )
					{
						if( !IsCharInMode( sgw->WorkBuffer[ k ], CalcMode ))
						{
							sgw->Actions = SGA_BEEP;
						}
					}
				}
				break;

			default:
/*				MyBeep();*/
				break;
		}
	}
	else
		return_code = 0;

   return(return_code);
}

MakeStaticHook(CalcHook, CalcHookFunc);

#define MAXCALCSTRINGLEN (8 * 4 + 2)

static void SetCalcDisplayWert( ULONG Number )
{
	UBYTE String[ MAXCALCSTRINGLEN ];

	CalcDisplayWert = Number;

	switch( CalcMode )
	{
		case CM_DEC:
			stcl_d( String, CalcDisplayWert );
			break;

		case CM_HEX:
			stcl_h( String, CalcDisplayWert );
			break;

		case CM_OCT:
			stcl_o( String, CalcDisplayWert );
			break;

		case CM_BIN:
			My_stcl_b( String, CalcDisplayWert );
			break;
	}

	GT_SetGadgetAttrs( CalcGadgets[ 24 ] , CalcWnd, NULL,
								GTST_String, String,
								TAG_DONE);
}

static long GetCalcDisplayWert( void )
{
	UBYTE String[ MAXCALCSTRINGLEN ], *ptr;

	strcpy( ptr = String, GetString( CalcGadgets[ 24 ]));

	switch( CalcMode )
	{
		case CM_DEC:
			stcd_l( String, &CalcDisplayWert );
			break;

		case CM_HEX:
			stch_l( String, &CalcDisplayWert );
			break;

		case CM_OCT:
			stco_l( String, &CalcDisplayWert );
			break;

		case CM_BIN:
			My_stcb_l( String, &CalcDisplayWert );
			break;
	}

	SetCalcDisplayWert( CalcDisplayWert );

	return( CalcDisplayWert );
}

static void SubCalcChar( void )
{
	UBYTE String[ MAXCALCSTRINGLEN ];

	strcpy( String, GetString( CalcGadgets[ 24 ]));

	if( strlen( String ) == 1 )
	    	String[ 0 ] = '0';
	else
		String[ strlen( String ) - 1 ] = 0;

	GT_SetGadgetAttrs( CalcGadgets[ 24 ] , CalcWnd, NULL,
								GTST_String, String,
								TAG_DONE);
}

BOOL CalcOverflow( char *String, UWORD Mode )
{
	BOOL TooBig = FALSE;

	if( *String == '0' )
		String++;

	switch( Mode )
	{
		case CM_DEC:
			if( *String != '-' )
			{
				if( strlen( String ) >= 10 )
				{
					if( strlen( String ) == 10 )
					{
						if( strcmp( String, "2147483648" ) >= 0 )
							TooBig = TRUE;
					}
					else
						TooBig = TRUE;
				}
			}
			else
			{
				if( strlen( String+1 ) >= 10 )
				{
					if( strlen( String+1 ) == 10 )
					{
						if( strcmp( String+1, "2147483648" ) > 0 )
							TooBig = TRUE;
					}
					else
						TooBig = TRUE;
				}
			}
			break;

		case CM_HEX:
			if( strlen( String ) > 8 )
				TooBig = TRUE;
			break;

		case CM_OCT:
			if( strlen( String ) >= 11 )
			{
				if( strlen( String ) == 11 )
				{
					if( strcmp( String, "40000000000" ) >= 0 )
						TooBig = TRUE;
				}
				else
					TooBig = TRUE;
			}
			break;

		case CM_BIN:
			if( strlen( String ) > 32 )
				TooBig = TRUE;
			break;
	}

	return( TooBig );
}

static BOOL AddCalcChar( UBYTE Zeichen, UWORD Mode )
{
	UBYTE String[ MAXCALCSTRINGLEN ];
	BOOL TooBig;

	if( !IsCharInMode( Zeichen, Mode )) return( FALSE );

	if( NewCalcWert == TRUE )
	{
		NewCalcWert = FALSE;
		*String = 0;
	}
	else
	{
		strcpy( String, GetString( CalcGadgets[ 24 ]));

		if( *String == '0' )
		{
			strcpy( String, String + 1 );
		}
	}

	strncat( String, &Zeichen, 1 );

	TooBig = CalcOverflow( String, Mode );

	if( !TooBig )
		GT_SetGadgetAttrs( CalcGadgets[ 24 ] , CalcWnd, NULL,
									GTST_String, String,
									TAG_DONE);
	else
		DisplayLocaleText( MSG_INFO_CALC_OVERFLOW );

	return( TRUE );
}

static void SetCalcMode( UWORD Mode )
{
	long wert;

	wert = GetCalcDisplayWert();

	CalcMode = Mode;

	SetCalcDisplayWert( wert );
	
	GT_SetGadgetAttrs( CalcGadgets[ 25 ] , CalcWnd, NULL,
								GTCY_Active, Mode,
								TAG_DONE);
}

static void SetCalcOp( UWORD Op )
{
	if( NewCalcWert != TRUE )
	{
		if( CalcOp != CO_GLEICH )
			SetCalcDisplayWert( DoCalcOp( CalcOp, LastCalcWert, GetCalcDisplayWert()));

		LastCalcWert = GetCalcDisplayWert();

		NewCalcWert = TRUE;
	}

	CalcOp = Op;
}

static int MakeCalcGadgets( UWORD Breite )
{
	struct NewGadget ng;
	struct Gadget *g;
	int k;
	UWORD ButtonBreite;

	if( CalcGList )
	{
		GetCalcDisplayWert();

		RemoveGList( CalcWnd, CalcGList, -1 );

		FreeGadgets( CalcGList );
		CalcGList = NULL;
	}

	if(! (g = CreateContext(&CalcGList)))
		return(1L);

	ButtonBreite =  (Breite - ( 3 * INNERGADGETX + 2 * BORDERX + 2 * BEVELX + 2 * INNERX )) / 4;

	ng.ng_VisualInfo = VisualInfo;
	ng.ng_TextAttr = &CalcWI->TA;
	ng.ng_Flags = PLACETEXT_LEFT;
	ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BEVELY + INNERY + BORDERY;
	ng.ng_GadgetID = 0;

	ng.ng_Width = ButtonBreite;
	ng.ng_Height = CalcWI->TF->tf_YSize + 6;
	ng.ng_Flags = PLACETEXT_IN;

	for( k = 0; k < 24; k++ )
	{
		ng.ng_TopEdge =  Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BORDERY + BEVELY + BUTTONY + ( k / 4 ) * ( CalcWI->TF->tf_YSize + 6 + INNERGADGETY) + 2 * ( CalcWI->TF->tf_YSize + 6 + INNERGADGETY );
		ng.ng_LeftEdge = Scr->WBorLeft + BORDERX + BEVELX + BUTTONX + ( k % 4 ) * ButtonBreite + ( k % 4 ) * INNERGADGETX;
		ng.ng_GadgetText = CalcTasten[k];

		CalcGadgets[k] = g = CreateGadgetA( BUTTON_KIND, g, &ng, NULL );

		if(!g) return(2L);

		ng.ng_GadgetID++;
	}

	{
	static struct TagItem StringTags[6] =
	{{GTST_String, 0}, {GTST_MaxChars, 0},{STRINGA_Justification, GACT_STRINGRIGHT}, {GTST_EditHook, (IPTR) &CalcHook}, {TAG_DONE}};
	ng.ng_GadgetText = 0;
	ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BORDERY + BEVELY + BUTTONY;
	ng.ng_LeftEdge = Scr->WBorLeft + BORDERX + BEVELX + BUTTONX;
	ng.ng_Width = Breite - 2 * BORDERX - 2 * BEVELX - 2 * INNERX;
	ng.ng_Height = CalcWI->TF->tf_YSize + 6;
	StringTags[0].ti_Data = 0;
	StringTags[1].ti_Data = 32;	/* Max */

	CalcGadgets[k++] = g = CreateGadgetA( STRING_KIND, g, &ng, StringTags );
	ng.ng_GadgetID++;
	}

	{
	static struct TagItem CycleTags[3] =
	{{GTCY_Labels, 0}, {GTCY_Active, 0}, {TAG_DONE}};

	ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BORDERY + BEVELY + BUTTONY + CalcWI->TF->tf_YSize + 6 + INNERGADGETY;
	ng.ng_LeftEdge = Scr->WBorLeft + BORDERX + BEVELX + BUTTONX;
	ng.ng_Width = Breite - 2 * BORDERX - 2 * BEVELX - 2 * INNERX;
	ng.ng_Height = CalcWI->TF->tf_YSize + 6;

	CycleTags[0].ti_Data = ( IPTR )CalcModes;	/* Anzahl */
	CycleTags[1].ti_Data = CalcMode;	/* Active */

	CalcGadgets[k] = g = CreateGadgetA( CYCLE_KIND, g, &ng, CycleTags);
	}

	if( CalcWnd )
	{
		SetzeMuster( CalcWnd );

		MyFilledDrawBevelBox(CalcWnd->RPort,	CalcWnd->BorderLeft + BORDERX,
															CalcWnd->BorderTop + BORDERY,
															CalcWnd->Width - CalcWnd->BorderRight - CalcWnd->BorderLeft - BORDERX * 2,
															CalcWnd->Height - CalcWnd->BorderBottom - CalcWnd->BorderTop - 2 * BORDERY,
															TRUE);
	
		AddGList( CalcWnd, CalcGList, -1, -1, 0);

		RefreshGadgets(CalcGList, CalcWnd, 0);
		GT_RefreshWindow( CalcWnd, NULL );

		SetCalcDisplayWert( CalcDisplayWert );
	}
	
	return 0; /* stegerg: CHECKME, there was no return at all */
}

static void CalcToClip( void )
{
	long wert =	GetCalcDisplayWert();
	SaveClip( ( STRPTR )&wert, 4 );
}

static void ClipToCalc( void )
{
	ULONG Laenge;
	UBYTE *String;

	String = GetClip( &Laenge );

	if(( Laenge > 4 ) || ( !Laenge ))
	{
		DisplayLocaleText( MSG_INFO_CALC_CLIPCONTENTSNOTANUMBER );
	}
	else
	{
		long wert;

		wert = String[0];
		if( Laenge >= 2 ) wert = ( wert << 8 ) + String[1];
		if( Laenge >= 3 ) wert = ( wert << 8 ) + String[2];
		if( Laenge >= 4 ) wert = ( wert << 8 ) + String[3];

		SetCalcDisplayWert( wert );
	}

	FreeMem( String, Laenge );
}							

static void DoCalcWndMsg( void )
{
	BOOL CloseMe = FALSE;
	struct IntuiMessage	*m, Msg;
	struct Gadget *gad;

	while(( m = GT_GetIMsg( CalcWnd->UserPort )))
	{
		CopyMem((char *)m, (char *)&Msg, (long)sizeof(struct IntuiMessage));

		GT_ReplyIMsg(m);

		gad = (struct Gadget *) Msg.IAddress;

		switch( Msg.Class )
		{
			case	IDCMP_NEWSIZE:
				MakeCalcGadgets( CalcWnd->Width - CalcWnd->BorderLeft - CalcWnd->BorderRight );
				RefreshWindowFrame( CalcWnd );
				break;

			case	IDCMP_REFRESHWINDOW:
				GT_BeginRefresh( CalcWnd );
				GT_EndRefresh( CalcWnd, TRUE );
				break;

			case	IDCMP_RAWKEY:
				switch( Msg.Code )
				{
					case CURSORUP:
					case CURSORLEFT:
						SetCalcMode(( CalcMode + 1 ) % 4);
						break;

					case CURSORDOWN:
					case CURSORRIGHT:
						SetCalcMode(( CalcMode + 3 ) % 4);
						break;
				}
				break;

			case	IDCMP_VANILLAKEY:
				if((( Msg.Code >= '0' ) && ( Msg.Code <= '9' )) ||
					(( ToUpper( Msg.Code ) >= 'A' ) && ( ToUpper( Msg.Code ) <= 'F' )))
				{
					AddCalcChar( Msg.Code, CalcMode );
					GetCalcDisplayWert();
				}
				else
				{
					switch( Msg.Code )
					{
						case 8:
							SubCalcChar();
							break;
						case 127:
							SetCalcDisplayWert( 0 );
							break;
						case 27:
							CloseMe = TRUE;
							break;
						case '\t':
							ActivateGadget( CalcGadgets[ 24 ], CalcWnd, 0 );
							break;
						case '<':
							CalcToClip();
							break;
						case '>':
							ClipToCalc();
							break;
						case ' ':
							ActivateWindow( AktuDI->Wnd );
							break;
						case '.':
							SetCalcDisplayWert( - GetCalcDisplayWert());
							if( NewCalcWert == TRUE )
								LastCalcWert = GetCalcDisplayWert();
							break;
						case '+':
							SetCalcOp( CO_ADD );
							break;
						case '-':
							SetCalcOp( CO_SUB );
							break;
						case '/':
							SetCalcOp( CO_DIV );
							break;
						case '*':
							SetCalcOp( CO_MUL );
							break;
						case 13:
						case '=':
							SetCalcOp( CO_GLEICH );
							break;
					}

				}
				break;

			case	IDCMP_CLOSEWINDOW:
				CloseMe = TRUE;
				break;

			case	IDCMP_GADGETUP:
				switch( gad->GadgetID )
				{	
					case 25:
						SetCalcMode( Msg.Code );
						break;
					case 21:		/* ± */
						SetCalcDisplayWert( - GetCalcDisplayWert());
						if( NewCalcWert == TRUE )
							LastCalcWert = GetCalcDisplayWert();
						break;
					case 23:		/* + */
						SetCalcOp( CO_ADD );
						break;
					case 19:		/* - */
						SetCalcOp( CO_SUB );
						break;
					case 11:		/* / */
						SetCalcOp( CO_DIV );
						break;
					case 15:		/* * */
						SetCalcOp( CO_MUL );
						break;
					case 22:		/* = */
						SetCalcOp( CO_GLEICH );
						break;
					case 7:		/* < */
						CalcToClip();
						break;
					case 3:		/* > */
						ClipToCalc();
						break;
					case 24:
						if( CalcOverflow( GetString( CalcGadgets[ 24 ]), CalcMode ))
						{
							DisplayLocaleText( MSG_INFO_CALC_OVERFLOW );
							ActivateGadget( CalcGadgets[ 24 ], CalcWnd, 0 );
						}
						break;
					default:
						AddCalcChar( *CalcTasten[ gad->GadgetID ], CalcMode );
						GetCalcDisplayWert();
						break;
				}
		}
	}

	if( CloseMe )
		CloseCalc();
}

BOOL OpenCalc( void )
{
	if( CalcWnd )
	{
		ActivateWindow( CalcWnd );
		return( TRUE );
	}

		/* Lokalisieren des Cyclegadgets */

	if( !Localized )
	{
		CONST_STRPTR *s;

		Localized = TRUE;
		s = CalcModes;

		while( *s )
		{
			*s = GetStr( *s );
			s++;
		}
	}

	if( CalcTop == -1 ) CalcTop = AktuDI->Wnd->TopEdge;
	if( CalcLeft == -1 ) CalcLeft = AktuDI->Wnd->LeftEdge + AktuDI->Wnd->Width;

	if(( CalcWI = NewCalcSetup( Scr )))
	{
		short k;
		ULONG MaxCalcWidth = 0, MinWidth;
		UWORD CalcHeight;

		for( k = 0; k < 24; k++ )
		{
			WORD Width;

			if((Width = TextLength(&CalcWI->RP,CalcTasten[k],1)) > MaxCalcWidth)
				MaxCalcWidth = Width;
		}

		MinWidth = MaxCalcWidth * 4 * 3 + 3 * INNERGADGETX + 2 * BORDERX + 2 * BEVELX + 2 * INNERX;

		if(( CalcWidth == -1) || ( MinWidth > CalcWidth )) CalcWidth = MinWidth;

		CalcHeight = (CalcWI->TF->tf_YSize + 6) * 8 + INNERGADGETY * 7 + 2 * BORDERY + 2 * BEVELY + 2 * INNERY;

#ifndef WFLG_SIZEBOTTOM
#define WFLG_SIZEBOTTOM WFLG_SIZEBBOTTOM
#endif

		if ( ! ( CalcWnd = OpenWindowTags( NULL,
			WA_Left,	CalcLeft,
			WA_Top,		CalcTop,
			WA_InnerWidth,	CalcWidth,
			WA_InnerHeight,	CalcHeight,
			WA_MaxWidth,	-1,
			WA_MinWidth,	MinWidth,
			WA_IDCMP,	BUTTONIDCMP | INTEGERIDCMP | TEXTIDCMP | IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_VANILLAKEY | IDCMP_REFRESHWINDOW | IDCMP_NEWSIZE,
			WA_Flags,	WFLG_SIZEGADGET | WFLG_SIZEBOTTOM | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_SMART_REFRESH | WFLG_ACTIVATE | WFLG_RMBTRAP,
			WA_Title,	GetStr( MSG_WINDOWTITLE_CALCULATOR ),
			WA_ScreenTitle,	"FileX '94",
			WA_PubScreen,	Scr,
			TAG_DONE )))
			return(3L);

		MakeCalcGadgets( CalcWidth );

		mainflags |= MF_CALC;

		MyAddSignal( 1L << CalcWnd->UserPort->mp_SigBit, &DoCalcWndMsg );
	}
	
	return 0; /* stegerg: CHECKME, return was missing completely */
}

void CloseCalc( void )
{
	MyRemoveSignal( 1L << CalcWnd->UserPort->mp_SigBit );

	mainflags &= ~MF_CALC;

	if( CalcWnd )
	{
		CalcLeft = CalcWnd->LeftEdge;
		CalcTop = CalcWnd->TopEdge;
		CalcWidth = CalcWnd->Width - CalcWnd->BorderLeft - CalcWnd->BorderRight;

		CloseWindow( CalcWnd );
		CalcWnd = NULL;
	}

	if( CalcGList )
	{
		FreeGadgets( CalcGList );
		CalcGList = NULL;
	}

	if( CalcWI )
	{
		NewCalcCleanup( CalcWI );
		CalcWI = NULL;
	}
}
