#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <dos/dos.h>
#include <libraries/gadtools.h>
#include <devices/clipboard.h>
#include <devices/inputevent.h>

#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>

#ifdef __AROS__
#include <aros/asmcall.h>
#endif

#include <string.h>
#include <ctype.h>

#endif

#include "filexstructs.h"
#include "filexstrings.h"
#include "allprotos.h"
#include "windowinfo.h"

enum { CM_BIN = 0, CM_DEC, CM_HEX, CM_OCT, CM_RAW, CM_TEXT };

static struct WindowInfo *ClipConvWI;
static struct Window *ClipConvWnd;
static struct Gadget *ClipConvGList;
static struct Gadget *ClipConvGadgets[6];
static long ClipConvLeft = -1, ClipConvTop = -1, ClipConvWidth = -1;

static long ClipConvTo, ClipConvFrom;
static char *FromStringDisplay, *ToStringDisplay;

static char *CCModeNames[] =
{
	MSG_GADGET_CLIPCONV_CYCLE_BIN,
	MSG_GADGET_CLIPCONV_CYCLE_DEC,
	MSG_GADGET_CLIPCONV_CYCLE_HEX,
	MSG_GADGET_CLIPCONV_CYCLE_OCT,
	MSG_GADGET_CLIPCONV_CYCLE_RAW,
	MSG_GADGET_CLIPCONV_CYCLE_TEXT,
	0
};

static BOOL Localized = FALSE;

static void SetClipConvFromGadget( char *text )
{
	if( FromStringDisplay ) FreeVec( FromStringDisplay );

	if(( FromStringDisplay = AllocVec( strlen( text ) + 1, MEMF_ANY )))
	{
		strcpy( FromStringDisplay, text );

		GT_SetGadgetAttrs( ClipConvGadgets[ 3 ] , ClipConvWnd, NULL,
									GTTX_Text, FromStringDisplay,
									TAG_DONE);
	}
}

static void SetClipConvToGadget( char *text )
{
	if( ToStringDisplay ) FreeVec( ToStringDisplay );

	if(( ToStringDisplay = AllocVec( strlen( text ) + 1, MEMF_ANY )))
	{
		strcpy( ToStringDisplay, text );

		GT_SetGadgetAttrs( ClipConvGadgets[ 4 ] , ClipConvWnd, NULL,
									GTTX_Text, ToStringDisplay,
									TAG_DONE);
	}
}

static BOOL SetClipConvData( char *Daten, LONG Laenge, BOOL SetFrom, BOOL SetRaw, BOOL Convert )
{
	UBYTE *ptr, *ToDaten;

	if(( ToDaten = ptr = AllocVec( Laenge * 2 + 1, MEMF_ANY )))
	{
		int k;

		if( !SetRaw )
		{
			strncpy( ToDaten, Daten, Laenge );
			ToDaten[ Laenge ] = 0;
		}
		else
		{
			for( k = 0; k < Laenge; k++ )
			{
				*ptr++ = hexarray[( Daten[ k ] >> 4 ) & 0xf ];
				*ptr++ = hexarray[ Daten[ k ] & 0xf ];
			}
	
			*ptr = 0;
		}
		
		if( SetFrom )
			SetClipConvFromGadget( ToDaten );
		else
			SetClipConvToGadget( ToDaten );

		if( Convert )
			SaveClip( ToDaten, Laenge * ( SetRaw ? 2 : 1 ));

		FreeVec( ToDaten );
		return( FALSE );
	}
	else
	{
		if( SetFrom )
			SetClipConvFromGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
		else
			SetClipConvToGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
		return( TRUE );
	}
}

static BOOL SetClipConvDataBin( char *Daten, LONG Laenge, BOOL SetFrom, BOOL Convert )
{
	UBYTE *ptr, *ToDaten;

	if(( ToDaten = ptr = AllocVec( Laenge * 8 + 1, MEMF_ANY )))
	{
		int k, j;
		int Aktu;

		for( k = 0; k < Laenge; k++ )
		{
			Aktu = Daten[ k ];

			for( j = 0; j < 8; j++ )
			{
				ptr[ 7 - j ] = ( Aktu & 1 ) + '0';

				Aktu = Aktu >> 1;
			}

			ptr += 8;
		}

		*ptr = 0;
		
		if( SetFrom )
			SetClipConvFromGadget( ToDaten );
		else
			SetClipConvToGadget( ToDaten );

		if( Convert )
			SaveClip( ToDaten, Laenge * 8 );

		FreeVec( ToDaten );
		return( FALSE );
	}
	else
	{
		if( SetFrom )
			SetClipConvFromGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
		else
			SetClipConvToGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
		return( TRUE );
	}
}

static BOOL IsDecString( UBYTE *String, LONG Laenge )
{
	UBYTE *ptr = String;
	BOOL Valid = TRUE;

	while( ptr < String + Laenge )
		if( !isdigit( *ptr ))
		{
			Valid = FALSE;
			break;
		}
		else
			ptr++;

	return( Valid );
}

static BOOL IsOctString( UBYTE *String, LONG Laenge )
{
	UBYTE *ptr = String;
	BOOL Valid = TRUE;

	while( ptr < String + Laenge )
		if( !(( *ptr >= '0' ) && ( *ptr <= '7' )))
		{
			Valid = FALSE;
			break;
		}
		else
			ptr++;

	return( Valid );
}

static void UpdateClipConv( BOOL Convert )
{
	BOOL Error = FALSE;

	ULONG Laenge;
	UBYTE *String;

	if(( String = GetClip( &Laenge ) ))
	{
		UBYTE *FromDaten, *FromTempAlloc = 0;
		WORD FromLaenge;
		LONG FromWert;
		BOOL UseFromWert = FALSE;
		BOOL Valid = TRUE;

		switch( ClipConvFrom )
		{
			case CM_RAW:
			case CM_TEXT:
				{
					FromLaenge = Laenge;

					if( !Convert )
						if( FromLaenge > 256 )
							FromLaenge = 256;

					FromDaten = String;

					if( SetClipConvData( FromDaten, FromLaenge, TRUE, ClipConvFrom == CM_RAW, FALSE ))
						Error = TRUE;
				}

				break;

			case CM_BIN:
				{
					UBYTE *ptr = String;
					ULONG StellenZahl = 0;

					while( ptr < String + Laenge )
						if(( *ptr != ' ' ) && ( *ptr != '0' ) && ( *ptr != '1' ))
						{
							Valid = FALSE;
							break;
						}
						else
						{
							if( *ptr != ' ' )
								StellenZahl++;
	
							ptr++;
						}
	
					if( Valid && StellenZahl )
					{
						FromLaenge = ( StellenZahl + 7 ) / 8;
	
						if(( FromTempAlloc = AllocVec( FromLaenge, MEMF_ANY )))
						{
							int k, Bits;
							UBYTE Aktu;

							ptr = String;
	
							Bits = 8 - ( long )( StellenZahl % 8 );

							for( k = 0; k < FromLaenge; k++ )
							{
								Aktu = 0;

								while( Bits != 8 )
								{
									if( *ptr != ' ' )
									{
										Aktu = ( Aktu << 1L ) + *ptr - '0';
										Bits++;
									}

									ptr++;
								}

								FromTempAlloc[ k ] = Aktu;

								Bits = 0;
							}

							FromDaten = FromTempAlloc;

							{
								UBYTE *ptr;

								if(( ptr = AllocVec( Laenge + 1, MEMF_ANY )))
								{
									strncpy( ptr, String, Laenge );
									ptr[ Laenge ] = 0;

									SetClipConvFromGadget( ptr );
								}
								else
								{
									SetClipConvFromGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
									SetClipConvToGadget( "" );
									Error = TRUE;
								}
							}
						}
						else
						{
							Error = TRUE;
							SetClipConvFromGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
							SetClipConvToGadget( "" );
						}
					}
					else
					{
						Error = TRUE;
						SetClipConvFromGadget( GetStr( MSG_INFO_CLIPCONV_NOTVALID ));
						SetClipConvToGadget( "" );
					}
				}
				break;

			case CM_DEC:
			case CM_OCT:
				{
					if( ClipConvFrom == CM_DEC )
						Valid = IsDecString( String, Laenge );
					else
						Valid = IsOctString( String, Laenge );

					if( Valid )
					{
						UBYTE Buffer[ 13 ];
						UBYTE *ptr;

						if(( ptr = AllocVec( Laenge + 1, MEMF_ANY )))
						{
							strncpy( ptr, String, Laenge );
							ptr[ Laenge ] = 0;

							if( ClipConvFrom == CM_DEC )
							{
								stcd_l( ptr, &FromWert );
								stcl_d( Buffer, FromWert );
							}
							else
							{
								stco_l( ptr, &FromWert );
								stcl_o( Buffer, FromWert );
							}

							SetClipConvFromGadget( Buffer );
							UseFromWert = TRUE;

							FreeVec( ptr );
						}
						else
						{
							SetClipConvFromGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
							SetClipConvToGadget( "" );
							Error = TRUE;
						}
					}
					else
					{
						Error = TRUE;
						SetClipConvFromGadget( GetStr( MSG_INFO_CLIPCONV_NOTVALID ));
						SetClipConvToGadget( "" );
					}
				}
				break;

			case CM_HEX:
				{
					UBYTE *ptr = String;
					ULONG StellenZahl = 0;

					while( ptr < String + Laenge )
						if(( *ptr != ' ' ) && ( !isxdigit( *ptr )))
						{
							Valid = FALSE;
							break;
						}
						else
						{
							ptr++;

							if( *ptr != ' ' )
								StellenZahl++;
						}
	
					if( Valid && StellenZahl )
					{
						FromLaenge = ( StellenZahl + 1 ) / 2;
	
						if(( FromTempAlloc = AllocVec( FromLaenge, MEMF_ANY )))
						{
							int k, Bits;
							UBYTE Aktu;

							ptr = String;

							Bits = 2 - ( long )( StellenZahl % 2 );
							if( Bits == 2 ) Bits = 0;

							for( k = 0; k < FromLaenge; k++ )
							{
								Aktu = 0;

								while( Bits != 2 )
								{
									if( *ptr != ' ' )
									{
										if( isdigit( *ptr ))
											Aktu = ( Aktu << 4L ) + *ptr - '0';
										else
											Aktu = ( Aktu << 4L ) + ( ToUpper( *ptr ) - 'A' ) + 0xa;

										Bits++;
									}

									ptr++;
								}

								FromTempAlloc[ k ] = Aktu;

								Bits = 0;
							}

							FromDaten = FromTempAlloc;

							{
								UBYTE *ptr;

								if(( ptr = AllocVec( Laenge + 1, MEMF_ANY )))
								{
									strncpy( ptr, String, Laenge );
									ptr[ Laenge ] = 0;

									SetClipConvFromGadget( ptr );
								}
								else
								{
									SetClipConvFromGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
									SetClipConvToGadget( "" );
									Error = TRUE;
								}
							}
						}
						else
						{
							Error = TRUE;
							SetClipConvFromGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
							SetClipConvToGadget( "" );
						}
					}
					else
					{
						Error = TRUE;
						SetClipConvFromGadget( GetStr( MSG_INFO_CLIPCONV_NOTVALID ));
						SetClipConvToGadget( "" );
					}
				}

				break;
		}

	/* FromDaten | FromWert enthält jetzt die Daten */
	/* FromLaenge die Laenge der FromDaten */

		if( Valid && ( !Error ))
		{
			switch( ClipConvTo )
			{
				case CM_OCT:
				case CM_DEC:
					if(( !UseFromWert ) && ( FromLaenge <= 4 ))
					{
						FromWert = FromDaten[0];
						if( FromLaenge >= 2 ) FromWert = ( FromWert << 8 ) + FromDaten[1];
						if( FromLaenge >= 3 ) FromWert = ( FromWert << 8 ) + FromDaten[2];
						if( FromLaenge >= 4 ) FromWert = ( FromWert << 8 ) + FromDaten[3];
		
						UseFromWert = TRUE;
					}

					if( UseFromWert )
					{
						UBYTE Buffer[ 13 ];

						switch( ClipConvTo )
						{
							case CM_OCT:
								stcl_o( Buffer, FromWert );
								break;
							case CM_DEC:
								stcl_d( Buffer, FromWert );
								break;
						}

						SetClipConvToGadget( Buffer );

						if( Convert )
							SaveClip( Buffer, strlen( Buffer ));
					}
					else
						SetClipConvToGadget( GetStr( MSG_INFO_CLIPCONV_NOTCONVERTIBLE ));
					
					break;
				case CM_BIN:
				case CM_HEX:
				case CM_RAW:
				case CM_TEXT:
					if( UseFromWert )
					{
						if( FromWert & 0xffff0000 )
							if( FromWert & 0xff000000 )
								FromLaenge = 4;
							else
								FromLaenge = 3;
						else
							if( FromWert & 0xff00 )
								FromLaenge = 2;
							else
								FromLaenge = 1;

						if(( FromTempAlloc = AllocVec( FromLaenge, MEMF_ANY )))
						{
							int k;

							FromDaten = FromTempAlloc;

							for( k = 0; k < FromLaenge; k++ )
							{
								FromTempAlloc[ FromLaenge - k - 1 ] = FromWert & 0xff;
								FromWert = FromWert >> 8L;
							}

/*							UseFromWert = FALSE;*/
						}
						else
							SetClipConvToGadget( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));
					}

					if( ClipConvTo == CM_BIN )
						SetClipConvDataBin( FromDaten, FromLaenge, FALSE, Convert );
					else
					{
						if( ClipConvTo == CM_HEX )
							SetClipConvData( FromDaten, FromLaenge, FALSE, ClipConvTo != CM_TEXT, Convert );
						else
						{
							SetClipConvData( FromDaten, FromLaenge, FALSE, ClipConvTo != CM_TEXT, FALSE );

							if( Convert )
								SaveClip( FromDaten, FromLaenge );
						}
					}
					break;
			}
		}

		if( FromTempAlloc ) FreeVec( FromTempAlloc );
	}
	else
	{
		SetClipConvFromGadget( GetStr( MSG_INFO_CLIPCONV_EMPTY ));
		SetClipConvToGadget( "" );
	}

	if( String ) FreeMem( String, Laenge );
}

static void UpdateClipConvSignal( void )
{
	UpdateClipConv( FALSE );
}

#ifdef __AROS__
AROS_UFH3S(ULONG, ClipHookFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(APTR, Obj, A2),
    AROS_UFHA(APTR, Msg, A1))
{
    AROS_USERFUNC_INIT
#else
static ULONG __saveds __asm ClipHookFunc(	register __a0 struct Hook *hook,
					register __a2 APTR Obj,
					register __a1 APTR Msg	)
{
#endif
	Signal( hook->h_Data, SIGBREAKF_CTRL_E );
   return(0);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

static struct Hook ClipHook =
{
	{0, 0}, (ULONG (*)()) ClipHookFunc, 0, 0
};

static void SetClipConvFrom( UWORD From )
{
	GT_SetGadgetAttrs( ClipConvGadgets[ 0 ] , ClipConvWnd, NULL,
								GTCY_Active, From,
								TAG_DONE);
	ClipConvFrom = From;

	UpdateClipConv( FALSE );
}

static void SetClipConvTo( UWORD To )
{
	GT_SetGadgetAttrs( ClipConvGadgets[ 1 ] , ClipConvWnd, NULL,
								GTCY_Active, To,
								TAG_DONE);
	ClipConvTo = To;

	UpdateClipConv( FALSE );
}


static int MakeClipConvGadgets( UWORD Breite )
{
	struct NewGadget ng;
	struct Gadget *g;
	static struct TagItem CycleTags[3] =
	{{GTCY_Labels, 0}, {GTCY_Active, 0}, {TAG_DONE}};
	UWORD CycleWidth = 0;

	Breite -= 2 * BORDERX + 2 * BEVELX + 2 * INNERX;


	if( ClipConvGList )
	{
		RemoveGList( ClipConvWnd, ClipConvGList, -1 );

		FreeGadgets( ClipConvGList );
		ClipConvGList = NULL;
	}

	{
		UWORD DummyBreite;
		char **Inhalt;

		Inhalt = CCModeNames;

		while( *Inhalt )
		{
			DummyBreite = MyTextLength( &ClipConvWI->RP, *Inhalt++ ) + 30;

			if( CycleWidth < DummyBreite )
				CycleWidth = DummyBreite;
		}
	}

	if(! (g = CreateContext(&ClipConvGList)))
		return(1L);

	ng.ng_VisualInfo = VisualInfo;
	ng.ng_TextAttr = &ClipConvWI->TA;
	ng.ng_Flags = PLACETEXT_LEFT;
	ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BEVELY + INNERY + BORDERY;
	ng.ng_GadgetID = 0;
	ng.ng_LeftEdge = Scr->WBorLeft + BORDERX + BEVELX + BUTTONX;
	ng.ng_Width = CycleWidth;
	ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BORDERY + BEVELY + BUTTONY;


	ng.ng_GadgetText = 0;
	ng.ng_Height = ClipConvWI->TF->tf_YSize + 6;

	CycleTags[0].ti_Data = ( ULONG )CCModeNames;
	CycleTags[1].ti_Data = ClipConvFrom;

	ClipConvGadgets[0] = g = CreateGadgetA( CYCLE_KIND, g, &ng, CycleTags);
	ng.ng_GadgetID++;


	CycleTags[1].ti_Data = ClipConvTo;
	ng.ng_TopEdge += ng.ng_Height + INNERGADGETY;
	ClipConvGadgets[1] = g = CreateGadgetA( CYCLE_KIND, g, &ng, CycleTags);
	ng.ng_GadgetID++;


	ng.ng_Width = Breite;
	ng.ng_Flags = PLACETEXT_IN;
	ng.ng_GadgetText = GetStr( MSG_GADGET_CLIPCONV_CONVERT );
	ng.ng_TopEdge += ng.ng_Height + INNERGADGETY;
	ClipConvGadgets[2] = g = CreateGadgetA( BUTTON_KIND, g, &ng, NULL );
	ng.ng_GadgetID++;


	ng.ng_Width = Breite - CycleWidth - INNERGADGETY;
	ng.ng_LeftEdge += CycleWidth + INNERGADGETY;
	ng.ng_GadgetText = 0;
	ng.ng_TopEdge = Scr->RastPort.TxHeight + Scr->WBorTop + 1 + BEVELY + INNERY + BORDERY;
	ClipConvGadgets[3] = g = CreateGadget( TEXT_KIND, g, &ng, GTTX_CopyText, TRUE, GTTX_Border, TRUE, TAG_DONE );
	ng.ng_GadgetID++;

	ng.ng_TopEdge += ng.ng_Height + INNERGADGETY;
	ClipConvGadgets[4] = g = CreateGadget( TEXT_KIND, g, &ng, GTTX_CopyText, TRUE, GTTX_Border, TRUE, TAG_DONE );

	if(!g) return(2L);

	if( ClipConvWnd )
	{
		SetzeMuster( ClipConvWnd );

		MyFilledDrawBevelBox(ClipConvWnd->RPort,	ClipConvWnd->BorderLeft + BORDERX,
															ClipConvWnd->BorderTop + BORDERY,
															ClipConvWnd->Width - ClipConvWnd->BorderRight - ClipConvWnd->BorderLeft - BORDERX * 2,
															ClipConvWnd->Height - ClipConvWnd->BorderBottom - ClipConvWnd->BorderTop - 2 * BORDERY,
															TRUE);
	
		AddGList( ClipConvWnd, ClipConvGList, -1, -1, 0);

		RefreshGadgets(ClipConvGList, ClipConvWnd, 0);
		GT_RefreshWindow( ClipConvWnd, NULL );

		UpdateClipConv( FALSE );
	}
	
	return 0; /* stegerg: CHECKME, return was missing completely */
}

static void DoClipConvWndMsg( void )
{
	BOOL CloseMe = FALSE;
	struct IntuiMessage	*m, Msg;
	struct Gadget *gad;

	while(( m = GT_GetIMsg( ClipConvWnd->UserPort )))
	{
		CopyMem((char *)m, (char *)&Msg, (long)sizeof(struct IntuiMessage));

		GT_ReplyIMsg(m);

		gad = (struct Gadget *) Msg.IAddress;

		switch( Msg.Class )
		{
			case	IDCMP_NEWSIZE:
				{
					MakeClipConvGadgets( ClipConvWnd->Width - ClipConvWnd->BorderLeft - ClipConvWnd->BorderRight );
	
					RefreshWindowFrame( ClipConvWnd );
				}
				break;

			case	IDCMP_RAWKEY:
				switch( Msg.Code )
				{
					case CURSORUP:
						SetClipConvFrom(( ClipConvFrom + 1 ) % 6);
						break;
					case CURSORLEFT:
						SetClipConvTo(( ClipConvTo + 1 ) % 6);
						break;

					case CURSORDOWN:
						SetClipConvFrom(( ClipConvFrom + 5 ) % 6);
						break;
					case CURSORRIGHT:
						SetClipConvTo(( ClipConvTo + 5 ) % 6);
						break;
						break;
				}
				break;

			case	IDCMP_VANILLAKEY:
				if( Msg.Code == 27 )
					CloseMe = TRUE;
				else
				if( Msg.Code == 13 )
					UpdateClipConv( TRUE );
				else
				if( Msg.Code == ' ' )
					ActivateWindow( AktuDI->Wnd );
					break;
					
				break;

			case	IDCMP_CLOSEWINDOW:
				CloseMe = TRUE;
				break;

			case	IDCMP_GADGETUP:
				switch( gad->GadgetID )
				{	
					case 0:
						SetClipConvFrom( Msg.Code );
						break;

					case 1:
						SetClipConvTo( Msg.Code );
						break;

					case 2:
						UpdateClipConv( TRUE );
						break;
				}
		}
	}

	if( CloseMe )
		CloseClipConv();
}

static struct IOClipReq *ClipboardIO;
static struct MsgPort *ClipboardMsgPort;

BOOL OpenClipConv( void )
{
	if( ClipConvWnd )
	{
		ActivateWindow( ClipConvWnd );
		return( TRUE );
	}

	if( ClipConvTop == -1 ) ClipConvTop = AktuDI->Wnd->TopEdge;
	if( ClipConvLeft == -1 ) ClipConvLeft = AktuDI->Wnd->LeftEdge + AktuDI->Wnd->Width;

		/* Lokalisieren des Cyclegadgets */

	if( !Localized )
	{
		char **s;

		Localized = TRUE;
		s = CCModeNames;

		while( *s )
		{
			*s = GetStr( *s );
			s++;
		}
	}

	{
		BOOL Success = FALSE;

		if(( ClipboardMsgPort = CreatePort(0L,0L) ))
		{
			if(( ClipboardIO = (struct IOClipReq *)	CreateExtIO(ClipboardMsgPort,sizeof(struct IOClipReq))))
			{
				if((!OpenDevice("clipboard.device", GetClipboardUnit(), ( struct IORequest * )ClipboardIO, 0)))
				{
					Success = TRUE;
				}
			}
		}

		if( !Success )
		{
			if( ClipboardIO ) DeleteExtIO(( struct IORequest * )ClipboardIO );
			if( ClipboardMsgPort ) DeletePort( ClipboardMsgPort );
			return( FALSE );
		}
		else
		{
			ClipHook.h_Data = FindTask( NULL );

			MyAddSignal( SIGBREAKF_CTRL_E, &UpdateClipConvSignal );

			ClipboardIO->io_Data = ( char * ) &ClipHook;
			ClipboardIO->io_Length = 1;
			ClipboardIO->io_Command = CBD_CHANGEHOOK;

			DoIO(( struct IORequest * ) ClipboardIO );
		}
	}


	if(( ClipConvWI = NewCalcSetup( Scr )))
	{
		UWORD ClipConvHeight;
		UWORD MinWidth = 0;
		UWORD DummyBreite;
		char **Inhalt;

		Inhalt = CCModeNames;

		while( *Inhalt )
		{
			DummyBreite = MyTextLength( &ClipConvWI->RP, *Inhalt++ ) + 30;

			if( MinWidth < DummyBreite )
				MinWidth = DummyBreite;
		}

		MinWidth += 8 * ClipConvWI->MaxHexWidth + 8 + INNERGADGETY;

		DummyBreite = MyTextLength( &ClipConvWI->RP, GetStr( MSG_GADGET_CLIPCONV_CONVERT )) + 16;

		if( MinWidth < DummyBreite )
			MinWidth = DummyBreite;

		MinWidth += 2 * BORDERX + 2 * BEVELX + 2 * INNERX;

		if(( ClipConvWidth == -1 ) || ( ClipConvWidth < MinWidth ))
			ClipConvWidth = MinWidth;

		ClipConvHeight = (ClipConvWI->TF->tf_YSize + 6) * 3 + INNERGADGETY * 2 + 2 * BORDERY + 2 * BEVELY + 2 * INNERY;

#ifndef WFLG_SIZEBOTTOM
#define WFLG_SIZEBOTTOM WFLG_SIZEBBOTTOM
#endif

		if ( ! ( ClipConvWnd = OpenWindowTags( NULL,
			WA_Left,	ClipConvLeft,
			WA_Top,		ClipConvTop,
			WA_InnerWidth,	ClipConvWidth,
			WA_InnerHeight,	ClipConvHeight,
			WA_MaxWidth,	-1,
			WA_MinWidth,	MinWidth,
			WA_IDCMP,	BUTTONIDCMP | INTEGERIDCMP | TEXTIDCMP | IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_VANILLAKEY | IDCMP_REFRESHWINDOW | IDCMP_NEWSIZE,
			WA_Flags,	WFLG_SIZEGADGET | WFLG_SIZEBOTTOM | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_SMART_REFRESH | WFLG_ACTIVATE | WFLG_RMBTRAP,
			WA_Title,	GetStr( MSG_WINDOWTITLE_CLIPCONV ),
			WA_ScreenTitle,	"FileX '94",
			WA_PubScreen,	Scr,
			TAG_DONE )))
			return(3L);

		MakeClipConvGadgets( ClipConvWidth );

		mainflags |= MF_CLIPCONV;

		MyAddSignal( 1L << ClipConvWnd->UserPort->mp_SigBit, &DoClipConvWndMsg );
	}
	
	return 0; /* stegerg: CHECKME, return was missing completely */
}

void CloseClipConv( void )
{
	MyRemoveSignal( 1L << ClipConvWnd->UserPort->mp_SigBit );
	MyRemoveSignal( SIGBREAKF_CTRL_E );

	mainflags &= ~MF_CLIPCONV;

	if( ClipboardIO )
	{
		ClipboardIO->io_Data = ( char * ) &ClipHook;
		ClipboardIO->io_Length = 0;
		ClipboardIO->io_Command = CBD_CHANGEHOOK;

		DoIO(( struct IORequest * ) ClipboardIO );

		CloseDevice(( struct IORequest * ) ClipboardIO );
		DeleteExtIO(( struct IORequest * ) ClipboardIO );
	}
	if( ClipboardMsgPort ) DeletePort( ClipboardMsgPort );

	if( ClipConvWnd )
	{
		ClipConvLeft = ClipConvWnd->LeftEdge;
		ClipConvTop = ClipConvWnd->TopEdge;
		ClipConvWidth = ClipConvWnd->Width - ClipConvWnd->BorderLeft - ClipConvWnd->BorderRight;

		CloseWindow( ClipConvWnd );
		ClipConvWnd = NULL;
	}

	if( FromStringDisplay )
	{
		FreeVec( FromStringDisplay );
		FromStringDisplay = NULL;
	}

	if( ToStringDisplay )
	{
		FreeVec( ToStringDisplay );
		ToStringDisplay = NULL;
	}

	if( ClipConvGList )
	{
		FreeGadgets( ClipConvGList );
		ClipConvGList = NULL;
	}

	if( ClipConvWI )
	{
		NewCalcCleanup( ClipConvWI );
		ClipConvWI = NULL;
	}
}
