/*
 * FileX © 1993-1994 bei Klaas Hermanns
 */
		/* Eigene Includes */

#ifdef USE_PROTO_INCLUDES

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/keymap.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>
#include <proto/diskfont.h>
#include <proto/asl.h>
#include <proto/rexxsyslib.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/locale.h>
#include <proto/asl.h>
#include <proto/reqtools.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>

#include <dos/dos.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <graphics/gfxbase.h>
#include <libraries/locale.h>
#include <libraries/reqtools.h>
#include <libraries/asl.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <workbench/startup.h>

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#endif

#include "filex.h"
#include "filexstructs.h"
#include "filexstrings.h"
#include "filexarexx.h"
#include "filexarexx_protos.h"

	/* Prototypes */

#include "allprotos.h"

	/* LibraryBases */

struct Library *CxBase;
struct Library *KeymapBase;
struct IntuitionBase *IntuitionBase;
struct Library *GadToolsBase;
struct Library *DiskfontBase;
struct Library *AslBase;
struct RxsLib *RexxSysBase;
struct ReqToolsBase *ReqToolsBase;
struct GfxBase *GfxBase;
struct Library *IconBase;
#ifdef NEED_UTILITYBASE_UTILITYBASE
struct UtilityBase *UtilityBase;
#else
struct Library *UtilityBase;
#endif
/*struct Library *XpkBase;*/
struct DosLibrary *DOSBase;
struct Library *WorkbenchBase;
struct Library *IFFParseBase;
struct ExecBase *SysBase;
struct LocaleBase *LocaleBase;

char programname[256];
extern void __saveds DoWindow(void);	/* Prototype für Hauptschleifenfkt. */
static char errorbuffer[256];

struct MsgPort *WndPort;	/* Port für alle IDCMP-Nachrichten */
struct MsgPort *AppPort;	/* AppPort für alle Windows */

static WORD MoveStatusBalkenY, MoveStatusBalkenMaxOff, MoveStatusBalkenMinOff;

struct DisplayData *AktuDD;
struct DisplayInhalt *AktuDI;

static struct List OldFileList = {
	( struct Node * )&OldFileList.lh_Tail,
	0,
	( struct Node * )&OldFileList.lh_Head,
	0,0
};
struct List FileList = {
	( struct Node * )&FileList.lh_Tail,
	0,
	( struct Node * )&FileList.lh_Head,
	0,0
};
struct List DisplayInhaltList = {
	( struct Node * )&DisplayInhaltList.lh_Tail,
	0,
	( struct Node * )&DisplayInhaltList.lh_Head,
	0,0
};

/*void Gibs( void )
{
	struct FileData *FD;
	struct DisplayData *DD;
	struct DisplayInhalt *DI;

	FD = (struct FileData *)FileList.lh_Head;

	while( FD != ( struct FileData * )&FileList.lh_Tail )
	{
		Printf("FileData = %8lx >%s<\n", FD, FD->Name );

		DD = (struct DisplayData *)FD->DisplayList.lh_Head;

		while( DD != ( struct DisplayData * )&FD->DisplayList.lh_Tail )
		{
			Printf("        %8lx %8lx %8lx\n", DD, DD->FD, DD->DI );
			DD = ( struct DisplayData *)DD->Node.ln_Succ;
		}

		FD = ( struct FileData *)FD->Node.ln_Succ;
	}

	DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
		Printf("DisplayInhalt = %8lx\n", DI );

		DD = (struct DisplayData *)DI->DisplayList.lh_Head;

		while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
		{
			Printf("        %8lx %8lx %8lx\n", DD, DD->FD, DD->DI );
			DD = ( struct DisplayData *)DD->Node.ln_Succ;
		}

		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}
}
*/
#define SECRETNUMBER 4
int secretpos = -1;
char *secret[] = 
{
"You have activated the secred mode!",
/*"SWF3 C-Team mit Krisi und Anke!",*/
/*"SUPERTRAMP ist genial!!",*/
/*"M * A * S * H!",*/
/*"Save the Whales!",*/
/*"Rottach '91-'93!",*/
/*"Zweifel Chips, kalte Schnitzel, Schlappen Baguette.",*/
/*"Jack and Jill went up the hill!",*/
"Fish don't like milk!",
"We do not burn, we shine!",
"Dreadlock holiday!",
"Even the trees had been a bad move...",
};

long ElementZahl( struct List *l )
{
	struct Node *n;
	long a = 0;

	n = l->lh_Head;

	while( n != ( struct Node * )&l->lh_Tail )
	{
		a++;

		n = n->ln_Succ;
	}

	return( a );
}

/*
 * ChecksumCorrect()
 *
 * Prüft die Checksumme eines Keys.
 * -1 => Kein Key vorhanden.
 * 1 => Checksumme ok.
 * 0 => Checksumme falsch!
 */

static long ChecksumCorrect( void )
{
	ULONG realchecksumme = 0xdead;
	int k;

	if( regname[0] == 0 ) return( -1 );

	for(k = 0; k < strlen(regname); k++)
	{
		realchecksumme += regname[ k ] << 4;
		realchecksumme ^= regname[ k ] << 24;
	}

	realchecksumme ^= regnum;

	if( realchecksumme == checksumme )
		return( 1 );
	else
		return( 0 );
}

static BOOL LoadKeyFile( char *KeyFileName )
{
	BPTR fh;
	BOOL Success = FALSE;

	if(( fh = Open( KeyFileName, MODE_OLDFILE )))
	{
		static UBYTE Key[256];
		LONG KeyLaenge, namelaenge, k;
		UBYTE *ptr;

		KeyLaenge = Read( fh, Key, 256 );

		ptr = Key + 4;

		for( k = 0; *ptr != 0xa5; k++ )
		{
			regname[ k ] = *ptr ^ 0xa5;
			ptr += 3;
		}

		regname[ namelaenge = k ] = 0;

		regnum = 0;
		regnum += Key[ KeyLaenge - 8 ] << 24;
		regnum += Key[ KeyLaenge - 6 ] << 16;
		regnum += Key[ KeyLaenge - 3 ] << 8;
		regnum += Key[ KeyLaenge - 1 ];
		regnum ^= 0x11223344;

		checksumme = 0;
		checksumme += Key[ KeyLaenge - 7 ] << 16;
		checksumme += Key[ KeyLaenge - 5 ] << 24;
		checksumme += Key[ KeyLaenge - 4 ] << 8;
		checksumme += Key[ KeyLaenge - 2 ];

		Close( fh );

		Success = TRUE;
	}

	return( Success );
}

BOOL QuitView( long Typ, struct DisplayData *DD )
{
	long z;

		/* Falls wir im Grabmemorymodus sind, keine Sicherheitsabfrage */

	if( DD->FD->Typ == FD_GRAB )
		return( TRUE );

	z = ElementZahl( &DD->FD->DisplayList );

	if(( z != 1) || (( z == 1 ) && ( QuitRequester( Typ, DD ))))
		return( TRUE );
	else
		return( FALSE );
}

BOOL QuitRequester( long Typ, APTR Zeiger )
{
	struct FileData *FD, *OneFD = NULL;
	struct DisplayInhalt *DI;
	struct DisplayData *DD;
	long Changes = 0;
	long Anzahl = 0;

	switch( Typ )
	{
		case 0:
			FD = (struct FileData *)FileList.lh_Head;

			while( FD != ( struct FileData * )&FileList.lh_Tail )
			{
				if( FD->Typ == FD_FILE )
				{
					Changes += FD->FullChanges + FD->Changes + FD->RedoChanges;
		
					if( Changes ) Anzahl++;
		
					OneFD = FD;
				}
		
				FD = ( struct FileData *)FD->Node.ln_Succ;
			}
			break;

		case 1:
			DI = ( struct DisplayInhalt * )Zeiger;
			FD = (struct FileData *)FileList.lh_Head;

			while( FD != ( struct FileData * )&FileList.lh_Tail )
			{
				struct DisplayData *DD;
				BOOL Found = FALSE;

				DD = GetDDFromFD( FD->DisplayList.lh_Head );

				while( DD != GetDDFromFD( &FD->DisplayList.lh_Tail ))
				{
					if( DD->DI == DI )
					{
						Found = TRUE;
						break;
					}
					DD = GetDDFromFD( DD->FNode.ln_Succ );
				}

				if( Found && ( FD->Typ == FD_FILE ))
				{
					Changes += FD->FullChanges + FD->Changes + FD->RedoChanges;
		
					if( Changes ) Anzahl++;
		
					OneFD = FD;
				}
		
				FD = ( struct FileData *)FD->Node.ln_Succ;
			}
			break;

		case 2:
		case 3:
		case 4:
		case 5:
			DD = ( struct DisplayData * )Zeiger;

			Anzahl = 1;
			Changes = DD->FD->FullChanges + DD->FD->Changes + DD->FD->RedoChanges;
			OneFD = DD->FD;
			break;
	}

	if( Changes )
	{
		char ReqText[256];

		if( Anzahl == 1 )
			strcpy( ReqText, GetStr( MSG_INFO_GLOBAL_CHANGESTOFILE ));
		else
			strcpy( ReqText, GetStr( MSG_INFO_GLOBAL_CHANGESTOFILES ));
				
		switch( Typ )
		{
			case 0:
				strcat( ReqText, GetStr( MSG_INFO_GLOBAL_WANTTOQUIT ));
				break;
			case 1:
				strcat( ReqText, GetStr( MSG_INFO_GLOBAL_WANTTOCLOSEWINDOW ));
				break;
			case 2:
				strcat( ReqText, GetStr( MSG_INFO_GLOBAL_WANTTOCLOSEVIEW ));
				break;
			case 3:
				strcat( ReqText, GetStr( MSG_INFO_GLOBAL_WANTTOCLEAR ));
				break;
			case 4:
				strcat( ReqText, GetStr( MSG_INFO_GLOBAL_WANTTOGRAB ));
				break;
			case 5:
				strcat( ReqText, GetStr( MSG_INFO_GLOBAL_WANTTOOPEN ));
				break;
		}

		if( Anzahl == 1 )
			return(( BOOL )MyFullRequestNoLocale( ReqText, GetStr( MSG_GADGET_CHANGES ), Changes, (*FilePart( OneFD->Name )) ? (OneFD->Name) : ((UBYTE *)GetStr( MSG_INFO_GLOBAL_UNNAMED ))));
		else
			return(( BOOL )MyFullRequestNoLocale( ReqText, GetStr( MSG_GADGET_CHANGES ), Changes, Anzahl ));
	}
	else
		return( TRUE );
}

static void __saveds OldFileListCallBack( struct List *List, struct Node *node )
{
	if( node )
		MyOpen( node->ln_Name, AktuDD );
}

static BOOL DoMenu( struct IntuiMessage *Msg )
{
	UWORD Number;
	BOOL Stop = FALSE;

	Number = Msg->Code;

	while(( !Stop ) && ( Number != MENUNULL ))
	{
		struct MenuItem *mi;
		
		if((mi = ItemAddress(FileXMenus, Number)))
		{
			switch(MENUNUM(Number))
			{
				case 0:					/* Project */
					switch(ITEMNUM(Number))
					{
						case	0:			/* New */
							ClearDDFile( AktuDD, FALSE );
							break;
						case	1:			/* Open */
							if( QuitView( 5, AktuDD ))
								OpenFile( AktuDD );
							break;
						case	2:			/* Open previous */
							if( !IsListEmpty( &OldFileList ))
							{
								if( QuitView( 5, AktuDD ))
									OpenListReq( &OldFileList, GetStr( MSG_WINDOWTITLE_PREVIOUSFILES ), &OldFileListCallBack );
							}
							else
								MyBeep();
							break;
						case	4:			/* Save */
							SaveFile( AktuDD->FD );
							break;
						case	5:			/* Save as */
							SaveAsFile( AktuDD->FD );
							break;
						case	7:			/* View */
							switch(SUBNUM(Number))
							{
								case	0:
									NewDisplay( FALSE );
									break;
								case	1:
									NewDisplay( TRUE );
									break;
								case	3:
									ActivateNextDisplay();
									break;
								case	4:
									ActivatePreviousDisplay();
									break;
								case	6:
									MakeDisplayMax( AktuDD );
									break;
								case	8:
									MakeDisplayBigger( AktuDD );
									break;
								case	9:
									MakeDisplaySmaller( AktuDD );
									break;
								case	11:
									if( SaveFile( AktuDD->FD ))
										Stop = CloseAktuView();
									break;
								case	12:
									Stop = CloseAktuView();
									break;
							}
							break;
						case	8:			/* Window */
							switch(SUBNUM(Number))
							{
								case	0:
									OpenNewWindow( FALSE );
									break;
								case	1:
									OpenNewWindow( TRUE );
									break;
								case	3:
									ActivateNextWindow();
									break;
								case	4:
									ActivatePreviousWindow();
									break;
								case	6:
									Stop = CloseAktuWindow();
									break;
							}
							break;
						case	10:
							if(QuitView( 4, AktuDD ))
								OpenGrabWindow();
							break;
						case	12:			/* Print */
							PrintFile( AktuDD->FD );
							break;
						case	14:		/* Iconify */
							mainflags |= MF_ICONIFY;
							break;
						case	16:		/* About */
							About();
							break;
						case	18:		/* Quit */
							if( QuitRequester( 0, 0 ))
								mainflags |= MF_ENDE;
							break;
					}
					break;
				case 1:					/* Edit */
					switch(ITEMNUM(Number))
					{
						case	0:			/* Mark Block */
							Mark( AktuDD );
							break;
						case	2:			/* Cut Block */
							Cut(AktuDD);
							break;
						case	3:			/* Copy Block */
							Copy(AktuDD);
							break;
						case	4:			/* Fill */
							if(AktuDD->Flags&DD_MARK)
								OpenFillWindow();
							else
								DisplayLocaleText( MSG_INFO_GLOBAL_NOBLOCKMARKED );
							break;
						case	6:			/* Paste */
							Paste(AktuDD);
							break;
						case	7:			/* Insert */
							if( !( AktuDD->FD->Typ == FD_GRAB ))
								InsertBlock(AktuDD);
							else
								DisplayLocaleText( MSG_INFO_GLOBAL_INSERTINGNOTPOSSIBLEINGRABMODE );
							break;
						case	8:
							if( !( AktuDD->FD->Typ == FD_GRAB ))
								AppendBlock(AktuDD);
							else
								DisplayLocaleText( MSG_INFO_GLOBAL_APPENDINGNOTPOSSIBLEINGRABMODE );
							break;
						case	9:		/* Print Block */
							PrintBlock();
							break;
						case	11:
							LoadBlockFR();
							break;
						case	12:
							SaveBlockFR();
							break;
						case	14:
							if( !Undo( AktuDD->FD ))
								DisplayLocaleText( MSG_INFO_GLOBAL_NOTHINGTOUNDO );
							break;
						case	15:
							if( !Redo( AktuDD->FD ))
								DisplayLocaleText( MSG_INFO_GLOBAL_NOTHINGTOREDO );
							break;
						case	17:
							if(( AktuDD->DisplayForm & DF_HEXASCII ) == DF_HEXASCII )
								WechselCursorBereich( AktuDD );
							break;
					}
					break;
				case 2:					/* Search and Replace */
					switch(ITEMNUM(Number))
					{
						case	0:
							OpenSearchWindow( FALSE );
							break;
						case	1:
							OpenSearchWindow( TRUE );
							break;
						case	2:
							SearchNext(0,FALSE,FALSE,AktuDD);
							break;
						case	3:
							SearchNext(BM_BACKWARD,FALSE,FALSE,AktuDD);
							break;
					}
					break;
				case 3:			/* Settings */
					switch(ITEMNUM(Number))
					{
						case	0:
							AdjustWindowSize( AktuDI );
							break;

						case	2:			/* Display */
							switch(SUBNUM(Number))
							{
								case	1:			/* only ASCII */
									SetDisplayTyp( 0, AktuDD );
									break;
								case	0:			/* nur Buchtsaben und Zahlen */
									SetDisplayTyp( 1, AktuDD );
									break;
								case	2:			/* all */
									SetDisplayTyp( 2, AktuDD );
									break;
								case	4:			/* Userdefined */
									SetDisplayTyp( 3, AktuDD );
									break;
								case	5:
									LoadUserDisplaytypFR();
									break;
							}
							break;
						case	4:			/* Select Font... */
							DoFontrequester( AktuDI );
							break;
						case	5:			/* Screen */
							Stop = TRUE;

							switch(SUBNUM(Number))
							{
								case	1:			/* Workbench */
									ReopenDisplay(SF_WORKBENCH);
									break;
								case	2:			/* Public */
									OpenPubScreenListReq();
									break;
								case	3:			/* Own Public */
									SetScreenOwnPublic();
									break;
								case	0:			/* Default Public */
									ReopenDisplay(SF_DEFAULT_PUBLIC);
									break;
							}
							break;
						case	6:			/* Palette... */
							Palette();
							break;
						case	8:			/* Miscellaneous... */
							OpenMiscSettings();
							break;
						case	10:			/* Load Settings... */
							LoadSettingsFR();
							Stop = TRUE;
							break;
						case	11:			/* Save Settings */
							SaveSettingsDefault();
							break;
						case	12:			/* Save Settings as...*/
							SaveSettingsFR();
							break;
					}
					break;
				case 4:					/* Move */
					switch(ITEMNUM(Number))
					{
						case 0:
							if( AktuDD->FD->Len ) JumpToByte();
							else MyBeep();
							break;
						case 2:
							MarkLocation( 0, AktuDD );
							break;
						case 3:
							JumpToLocation( 0, AktuDD );
							break;
					}
					break;
				case 5:					/* ARexx */
					switch(ITEMNUM(Number))
					{
						case 0:
/*							OpenCommandShell();*/
							break;
						case 2:
							ExecuteARexxCommand(0);
							break;
						case 3:
							SetARexxCommand(-1,0);
							Stop = TRUE;
							break;
						case 16:
							if( LoadARexxCommandsFR())
								Stop = TRUE;
							break;
						case 17:
							SaveARexxCommandsDefault();
							break;
						case 18:
							SaveARexxCommandsFR();
							break;
						default:
							if((ITEMNUM(Number)>=5)&&(ITEMNUM(Number)<=15))
							{
								ExecuteARexxCommandNumber(ITEMNUM(Number)-5);
							}
							break;
					}
					break;

				case 6:					/* Misc */
					switch(ITEMNUM(Number))
					{
						case 0:
							OpenCalc();
							break;
						case 1:
							OpenClipConv();
							break;
					}
					break;
			}
			Number = mi->NextSelect;
		}
		else
			break;
	}

	return( Stop );
}

void DoFileXWindowMessage( void )
{
	struct IntuiMessage *m, Msg;
/*	BOOL Stop = FALSE;*/
	struct DisplayInhalt *DI;

	while((m = ( struct IntuiMessage * )GetMsg( WndPort )))
	{
		DI = ( struct DisplayInhalt * )m->IDCMPWindow->UserData;

/*	while(( !Stop ) && ( !( mainflags & MF_ENDE )) && DI->Wnd && (m = ( struct IntuiMessage * )GetMsg( DI->Wnd->UserPort )))*/
		CopyMem((char *)m, (char *)&Msg, (long)sizeof(struct IntuiMessage));

		ReplyMsg(( struct Message * )m );

/*		PrintIntuiMessage( &Msg );*/

		switch( Msg.Class )
		{
			case IDCMP_ACTIVEWINDOW:
				MyActivateWindow( DI );
				break;

			case	IDCMP_VANILLAKEY:
				switch( Msg.Code )
				{
					case 27:
						/*Stop = */CloseAktuWindow();
						break;
					default:
						if( AktuDD->FD->Len )
						{
							if( AktuDD->Flags & DD_HEX )
							{
								if(isxdigit(Msg.Code))
								{
									UBYTE oldwert=AktuDD->FD->Mem[AktuDD->CPos];
	
									if(!(AktuDD->Flags&DD_HEXEDITPOS))
									{
	
										AktuDD->FD->Mem[AktuDD->CPos]&=0xf;
										if(isdigit(Msg.Code))
											AktuDD->FD->Mem[AktuDD->CPos]+=(Msg.Code-'0') * 0x10;
										else
											AktuDD->FD->Mem[AktuDD->CPos]+=(tolower(Msg.Code)-'a'+10)*0x10;
	
										AddUndoCharHex(AktuDD->CPos,oldwert,AktuDD->FD->Mem[AktuDD->CPos],FALSE, AktuDD->FD );
	
										AllDisplayChar( AktuDD->FD, AktuDD->CPos );
										UpdateAllStatusZeilen( AktuDD->FD );
										AktuDD->Flags|=DD_HEXEDITPOS;
									}
									else
									{
										AktuDD->FD->Mem[AktuDD->CPos]&=0xf0;
										if(isdigit(Msg.Code))
											AktuDD->FD->Mem[AktuDD->CPos]+=(Msg.Code-'0');
										else
											AktuDD->FD->Mem[AktuDD->CPos]+=tolower(Msg.Code)-'a'+10;
	
										AddUndoCharHex(AktuDD->CPos,oldwert,AktuDD->FD->Mem[AktuDD->CPos],TRUE, AktuDD->FD );
	
										AllDisplayChar( AktuDD->FD, AktuDD->CPos );
										UpdateAllStatusZeilen( AktuDD->FD );
										MoveCursorX( 1, AktuDD );
										AktuDD->Flags&=~DD_HEXEDITPOS;
									}
								}
								else
									MyBeep();
							}
							else
							{
								AddUndoChar(AktuDD->CPos,AktuDD->FD->Mem[AktuDD->CPos],Msg.Code, AktuDD->FD );
								AktuDD->FD->Mem[AktuDD->CPos]=Msg.Code;
								AllDisplayChar( AktuDD->FD, AktuDD->CPos );
								UpdateAllStatusZeilen( AktuDD->FD );
								MoveCursorX( 1, AktuDD );
							}
						}
						else
							MyBeep();
						break;
				}
				break;

			case IDCMP_RAWKEY:
							/* Mögliche Qualifier:	0x08=CONTROL */
							/* 							0x01=LSHIFT	 */
							/* 							0x02=RSHIFT	 */
							/* 							0x10=LALT	 */
							/* 							0x20=RALT	 */
							/* 							0x40=LAMIGA  */
							/* 							0x80=RAMIGA  */
				if((Msg.Qualifier & 0x02) && (Msg.Qualifier & 0x08))	/* RShift Ctrl */
				switch(Msg.Code)
				{
					case CURSORDOWN:
						if( secretpos == -1 )
							secretpos = 0;
						else
						{
							secretpos++;
							if( secretpos >= SECRETNUMBER ) secretpos = 0;
						}
						SetStatusZeile( secret[ secretpos ], AktuDD );
						break;
					case CURSORUP:
						if( secretpos == -1 )
							secretpos = 0;
						else
						{
							secretpos--;
							if( secretpos < 0 ) secretpos = SECRETNUMBER - 1;
						}
						SetStatusZeile( secret[ secretpos ], AktuDD );
						break;
				}
				if(Msg.Qualifier & (0x10|0x20))	/* Alts */
				switch(Msg.Code)
				{
					case CURSORDOWN:
						MoveCursorY( altsprungweite, AktuDD );
						break;
					case CURSORUP:
						MoveCursorY( -altsprungweite, AktuDD );
						break;
					case CURSORRIGHT:
						SetCursorAltRight( AktuDD );
						break;
					case CURSORLEFT:
						SetCursorAltLeft( AktuDD );
						break;
				}
				else
				if(Msg.Qualifier & (0x01|0x02))	/* Shifts */
				switch(Msg.Code)
				{
					case CURSORDOWN:
						MoveCursorY( AktuDD->Zeilen - 1, AktuDD );
						break;
					case CURSORUP:
						MoveCursorY( 1 - AktuDD->Zeilen, AktuDD );
						break;
					case CURSORRIGHT:
						SetCursorEndOfLine( AktuDD );
						break;
					case CURSORLEFT:
						SetCursorStartOfLine( AktuDD );
						break;
				}
				else
				if(Msg.Qualifier & 0x08)				/* Control */
				switch(Msg.Code)
				{
					case CURSORUP:
						SetCursorStart( AktuDD );
						break;
					case CURSORDOWN:
						SetCursorEnd( AktuDD );
						break;
					case CURSORRIGHT:
						SetCursorEndOfLine( AktuDD );
						break;
					case CURSORLEFT:
						SetCursorStartOfLine( AktuDD );
						break;
				}
				else
				switch(Msg.Code)						/* Nichts */
				{
					case CURSORDOWN:
						MoveCursorY( 1, AktuDD );
						break;
					case CURSORUP:
						MoveCursorY( -1, AktuDD );
						break;
					case CURSORLEFT:
						MoveCursorX( -1, AktuDD );
						break;
					case CURSORRIGHT:
						MoveCursorX( 1, AktuDD );
						break;
				}
				break;

			case	IDCMP_NEWSIZE:
				MakeDisplay( DI );
				RefreshWindowFrame( DI->Wnd );
				break;

			case	IDCMP_CLOSEWINDOW:
				/*Stop = */CloseAktuWindow();
				break;

			case	IDCMP_GADGETUP:
				DisplayFromScroller( AktuDD );
				SetCursor( AktuDD->CPos, AktuDD );
				break;

			case	IDCMP_GADGETDOWN:
				if(( struct DisplayData * )(( struct Gadget * )Msg.IAddress)->UserData != AktuDD )
					ChangeAktuView(( struct DisplayData * )(( struct Gadget * )Msg.IAddress)->UserData);
				break;

			case	IDCMP_IDCMPUPDATE:
				DisplayFromScroller( AktuDD );
				break;

			case	IDCMP_MOUSEMOVE:
				if( mainflags & MF_MOVESTATUSBALKEN )
				{
					WORD Off;
					
					Off = Msg.MouseY - ( AktuDD->sbby - BOXADDY / 2 );

					if( Off > 0 )
						Off = Off / DI->fhoehe;
					else
						Off = Off / DI->fhoehe - 1;

					if(( Off >= MoveStatusBalkenMinOff ) && ( Off <= MoveStatusBalkenMaxOff ))
						if( Off != MoveStatusBalkenY )
						{
							InvertStatusBalkenRahmen( AktuDD, MoveStatusBalkenY );
							InvertStatusBalkenRahmen( AktuDD, MoveStatusBalkenY = Off );
						}
				}
				else if(mainflags&MF_MOVECURSOR)
				{
					SetzeCursorMaus(Msg.MouseX,Msg.MouseY, AktuDD,DI);
				}
				break;

			case	IDCMP_MOUSEBUTTONS:
				switch(Msg.Code)
				{
					case SELECTUP:
						AktuDI->Wnd->Flags &= ~WFLG_REPORTMOUSE;
						mainflags &= ~MF_LEFTMOUSEDOWN;
						mainflags &= ~MF_MOVECURSOR;

						if( mainflags & MF_MOVESTATUSBALKEN )
						{
							InvertStatusBalkenRahmen( AktuDD, MoveStatusBalkenY );
							mainflags &= ~MF_MOVESTATUSBALKEN;
							ModifyIDCMP( AktuDI->Wnd, AktuDI->Wnd->IDCMPFlags | ( IDCMP_RAWKEY | IDCMP_VANILLAKEY ));

							if( MoveStatusBalkenY != 0 )
							{
								struct DisplayData *DD;

								if( MoveStatusBalkenY > 0 )
								{
									((struct DisplayData *)AktuDD->Node.ln_Pred)->Zeilen += MoveStatusBalkenY;

									DD = AktuDD;
									while( MoveStatusBalkenY )
									{
										if( DD->Zeilen - 1 > MoveStatusBalkenY )
										{
											DD->Zeilen -= MoveStatusBalkenY;
											MoveStatusBalkenY = 0;
										}
										else
										{
											MoveStatusBalkenY -= DD->Zeilen -1;
											DD->Zeilen = 1;
										}

										DD = ( struct DisplayData * )DD->Node.ln_Succ;
									}
								}
								else
								{
									MoveStatusBalkenY = -MoveStatusBalkenY;

									AktuDD->Zeilen += MoveStatusBalkenY;

									DD = ( struct DisplayData * )AktuDD->Node.ln_Pred;
									while( MoveStatusBalkenY )
									{
										if( DD->Zeilen - 1 > MoveStatusBalkenY )
										{
											DD->Zeilen -= MoveStatusBalkenY;
											MoveStatusBalkenY = 0;
										}
										else
										{
											MoveStatusBalkenY -= DD->Zeilen -1;
											DD->Zeilen = 1;
										}

										DD = ( struct DisplayData * )DD->Node.ln_Pred;
									}
								}

								MakeDisplay( AktuDI );
							}
						}
						break;

					case SELECTDOWN:
						AktuDI->Wnd->Flags |= WFLG_REPORTMOUSE;
						if( StatusBalkenHit(Msg.MouseX,Msg.MouseY,AktuDI))
						{
							if( AktuDD != ( struct DisplayData *)AktuDI->DisplayList.lh_Head )
							{
								MoveStatusBalkenY = Msg.MouseY - ( AktuDD->sbby - BOXADDY / 2 );

								if( MoveStatusBalkenY > 0 )
									MoveStatusBalkenY = MoveStatusBalkenY / AktuDI->fhoehe;
								else
									MoveStatusBalkenY = MoveStatusBalkenY / AktuDI->fhoehe - 1;

								mainflags |= MF_MOVESTATUSBALKEN;

								ModifyIDCMP( AktuDI->Wnd, AktuDI->Wnd->IDCMPFlags & ~( IDCMP_RAWKEY | IDCMP_VANILLAKEY ));

								{
									struct DisplayData *DD;

									DD = ( struct DisplayData * )AktuDD->Node.ln_Pred;
									MoveStatusBalkenMinOff = 0;

									while(DD != ( struct DisplayData *)&AktuDI->DisplayList.lh_Head )
									{
										MoveStatusBalkenMinOff -= DD->Zeilen - 1;
										DD = ( struct DisplayData * )DD->Node.ln_Pred;
									}

									DD = AktuDD;
									MoveStatusBalkenMaxOff = 0;

									while(DD != ( struct DisplayData *)&AktuDI->DisplayList.lh_Tail )
									{
										MoveStatusBalkenMaxOff += DD->Zeilen - 1;
										DD = ( struct DisplayData * )DD->Node.ln_Succ;
									}
								}

								if( MoveStatusBalkenY < MoveStatusBalkenMinOff )
									 MoveStatusBalkenY = MoveStatusBalkenMinOff;

								if( MoveStatusBalkenY > MoveStatusBalkenMaxOff )
									 MoveStatusBalkenY = MoveStatusBalkenMaxOff;

								InvertStatusBalkenRahmen( AktuDD, MoveStatusBalkenY );
							}
						}
						else
						{
							static ULONG LastSeconds = 0, LastMicros = 0;
							BOOL Doppel = FALSE;
							long OldCPos;
							struct DisplayData *OldDD;
							
								/* Doppelklick testen */
							
							if( DoubleClick( LastSeconds, LastMicros, Msg.Seconds, Msg.Micros ))
								Doppel = TRUE;
							
							LastSeconds = Msg.Seconds;
							LastMicros = Msg.Micros;

							mainflags|=MF_LEFTMOUSEDOWN;
							
							OldCPos = AktuDD->CPos;
							OldDD = AktuDD;
							
								/* Prüfen, ob Cursor überhaupt irgendwo hingesetzt wurde */
							
							if( SetzeCursorMaus( Msg.MouseX, Msg.MouseY, NULL, AktuDI ))
							{
								mainflags |= MF_MOVECURSOR;
							
									/* Falls Doppelclick und Cursor an gleicher Stelle */
							
								if( Doppel && ( AktuDD == OldDD ) && ( AktuDD->CPos == OldCPos ))
								{
									Mark( AktuDD );
								}
							}
						}
						break;
				}
				break;

			case	IDCMP_INTUITICKS:
				if( mainflags & MF_MOVECURSOR )
					SetzeCursorMaus(Msg.MouseX,Msg.MouseY, AktuDD, AktuDI);
				break;

			case	IDCMP_MENUPICK:
				if( mainflags & MF_MOVESTATUSBALKEN )
				{
					InvertStatusBalkenRahmen( AktuDD, MoveStatusBalkenY );
					mainflags &= ~MF_MOVESTATUSBALKEN;
					ModifyIDCMP( AktuDI->Wnd, AktuDI->Wnd->IDCMPFlags | ( IDCMP_RAWKEY | IDCMP_VANILLAKEY ));
				}
				/*Stop = */DoMenu( &Msg );
				break;
		}
	}
}

void DoAppWindowMessage( void )
{
	struct AppMessage *appMsg;
	long k;

	while((appMsg=(struct AppMessage *)GetMsg(AppPort)))
	{
		if( appMsg->am_Type == 7 )		/* Icon ins Fenster gezogen */
		{
			struct WBArg *argptr;
			BOOL fertig=FALSE;

			argptr=appMsg->am_ArgList;

			for(k=0;(k<appMsg->am_NumArgs) && fertig==FALSE;k++,argptr++)
			{
				if(strlen(argptr->wa_Name))
				{
					struct DisplayData *DD;
					UBYTE buffer[256];

					NameFromLock(argptr->wa_Lock,buffer,256);

					AddPart(buffer,argptr->wa_Name,256);

					MyActivateWindow(( struct DisplayInhalt * )appMsg->am_UserData );
					ActivateWindow( AktuDI->Wnd );

					if( ( DD = GetroffenerView(appMsg->am_MouseX,appMsg->am_MouseY,AktuDI)))
						ChangeAktuView( DD );

					MyOpen(buffer, AktuDD);

					fertig=TRUE;
				}
			}
		}

		ReplyMsg((struct Message *)appMsg);
	}
}

void DoARexxMsg( void )
{
#ifdef AREXX
	ARexxDispatch( MyRexxHost );
#endif
}

static struct RDArgs *cliargs;
static struct MyArgs
{
	UBYTE *ma_PublicScreenName;
	UBYTE *ma_ArexxPortName;
	UBYTE *ma_ArexxStartupScript;
	UBYTE *ma_SettingsName;
	UBYTE *ma_CommandsName;
	UBYTE *ma_LanguageName;
	UBYTE *ma_KeyName;
	LONG ma_Sync;
	UBYTE *ma_FileName;
} myargs = { 0,0,0,0,0,0,0,0 };

struct DiskObject *dob;

char *ParseArgsAndAllocThings( void )
{
	char *errorstring;
	char *String;
	long k;

		/* Locale Initialisierungen */

	String = 0;

	if( cliargs )
	{
		if( myargs.ma_LanguageName )
		{
			String = myargs.ma_LanguageName;
		}
	}
	else if( dob )
	{
		String = FindToolType((UBYTE **) dob->do_ToolTypes, "LANGUAGE" );
	}

	if(LocaleBase)
	{
		if(!(Locale = OpenLocale( 0 )))
		{
			Fault(IoErr(),"FileX",errorbuffer, 256);
			return(errorbuffer);
		}

		Catalog = OpenCatalog(Locale, "FileX.catalog", OC_BuiltInLanguage, "english", OC_Version, 2,(String ? OC_Language : TAG_DONE ), String, TAG_DONE );
	}

	LocalizeAll();


		/* Allgemeine Initialisierung, unabhängig von Argumenten */

	strcpy( DefaultFontName, GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name );
	DefaultFontSize = GfxBase->DefaultFont->tf_YSize;

	Kick30 = ( SysBase->LibNode.lib_Version >= 39 );

	if( !( TextLineBuffer = AllocVec( TEXTLINEBUFFERLEN, MEMF_ANY)))
		return( GetStr( MSG_INFO_GLOBAL_CANTALLOCSOMEMEM ));

		/* Windowdefinitionsstrings allokieren */

	if(!(arexxcommandshellwindow = AllocVec(strlen(DEFAULT_COMMANDSHELLWINDOW) + 1,MEMF_ANY)))
		return(GetStr(MSG_INFO_GLOBAL_CANTALLOCSOMEMEM));

	strcpy(arexxcommandshellwindow,DEFAULT_COMMANDSHELLWINDOW);

	if(!(arexxcommandwindow = AllocVec(strlen(DEFAULT_COMMANDWINDOW) + 1,MEMF_ANY)))
		return(GetStr(MSG_INFO_GLOBAL_CANTALLOCSOMEMEM));

	strcpy(arexxcommandwindow,DEFAULT_COMMANDWINDOW);

		/* ARexxCommandos im Menu auf 'leer` setzen */

	for( k = 0; k < 10; k++ )
		SetEmptyARexxMenu( k );

		/* ReplyPort für Beantwortung eigener Msgs einrichten */

	if( !InitReplyPort())
		return( GetStr( MSG_INFO_GLOBAL_CANTCREATEMSGPORT ));

		/* Start Taskpriority bestimmen */

	oldtaskpri = taskpri = FindTask( NULL )->tc_Node.ln_Pri;

	InitSearchHistory();

	if((errorstring = AllocRequester()))
		return( errorstring );

		/* Einen Windowport für alle IDCMP-Nachtrichten einrichten */

	if((WndPort=CreateMsgPort()))
	{
		MyAddSignal( 1L << WndPort->mp_SigBit, &DoFileXWindowMessage);
	}
			/* AppMessagePort erstellen */

	if((AppPort=CreateMsgPort()))
	{
		MyAddSignal( 1L << AppPort->mp_SigBit, &DoAppWindowMessage);
	}


		/* Settings laden. */

	if( cliargs )
	{
		if(myargs.ma_SettingsName)
		{
			if(!LoadSettings(myargs.ma_SettingsName,TRUE))LoadSettingsDefault();
		}
		else
			LoadSettingsDefault();
	}
	else if( dob )	
	{
		if((String=FindToolType((UBYTE **)dob->do_ToolTypes,"SETTINGS")))
		{
			if(!LoadSettings(String,TRUE))LoadSettingsDefault();
		}
		else
			LoadSettingsDefault();
	}

		/* Verwaltungsstruckturen init */

	AktuDI = AllocDI();
	AktuDD = AllocDD( AktuDI, AllocFD( FD_FILE ));
	AktuDI->AktuDD = AktuDD;


		/* Argumentparsing und entsprechende Initialisierungen */

	if( cliargs )
	{
		if(myargs.ma_CommandsName)
		{
			if(!LoadARexxCommands(myargs.ma_CommandsName))
				LoadARexxCommandsDefault();
		}
		else
			LoadARexxCommandsDefault();
	
		if(myargs.ma_KeyName)
		{
			if( !LoadKeyFile( myargs.ma_KeyName ))
				if( !LoadKeyFile( "PROGDIR:filex.key" ))
					if( !LoadKeyFile( "l:filex.key" ))
						LoadKeyFile( "s:filex.key" );
		}
		else
			if( !LoadKeyFile( "PROGDIR:filex.key" ))
				if( !LoadKeyFile( "l:filex.key" ))
					LoadKeyFile( "s:filex.key" );
	
		if( myargs.ma_PublicScreenName )
		{
			SetScreenTypePublic( myargs.ma_PublicScreenName );
		}
	
		if(myargs.ma_FileName)
		{
			MyOpen(myargs.ma_FileName, AktuDD);
		}

		if(myargs.ma_ArexxStartupScript)
		{
			if((startuparexxscript = AllocVec(strlen(myargs.ma_ArexxStartupScript)+1,MEMF_ANY)))
				strcpy(startuparexxscript,myargs.ma_ArexxStartupScript);
		}
	}
	else if( dob )	
	{
		if((String=FindToolType((UBYTE **)dob->do_ToolTypes,"COMMANDS")))
		{
			if(!LoadARexxCommands(String))
				LoadARexxCommandsDefault();
		}
		else
			LoadARexxCommandsDefault();

		if((String=FindToolType((UBYTE **)dob->do_ToolTypes,"KEY")))
		{
			if( !LoadKeyFile( String ))
				if( !LoadKeyFile( "PROGDIR:filex.key" ))
					if( !LoadKeyFile( "l:filex.key" ))
						LoadKeyFile( "s:filex.key" );
		}
		else
			if( !LoadKeyFile( "PROGDIR:filex.key" ))
				if( !LoadKeyFile( "l:filex.key" ))
					LoadKeyFile( "s:filex.key" );

		if((String=FindToolType((UBYTE **)dob->do_ToolTypes,"PUBSCREENNAME")))
		{
			SetScreenTypePublic( myargs.ma_PublicScreenName );
		}

		if((String=FindToolType((UBYTE **)dob->do_ToolTypes,"FILE")))
		{
			MyOpen(String,AktuDD);
		}

		if((String=FindToolType((UBYTE **)dob->do_ToolTypes,"STARTUP")))
		{
			if((startuparexxscript = AllocVec(strlen(String)+1,MEMF_ANY)))
				strcpy(startuparexxscript,String);
		}
	}

#ifdef AREXX
		/* ARexx Setup */

	String = 0;

	if( cliargs )
		String = myargs.ma_ArexxPortName;
	else if( dob )
		String = FindToolType((UBYTE **) dob->do_ToolTypes,"PORTNAME" );

	if( !( MyRexxHost = SetupARexxHost( String, NULL )))
		return(GetStr(MSG_INFO_AREXX_CANTSETUPAREXXHOST));

	MyAddSignal( 1L << MyRexxHost->port->mp_SigBit, &DoARexxMsg);

		/* ID von FileX anhand des ARexxportnamens bestimmen */

	{
		long k;
		for(k=0;(k<strlen(MyRexxHost->portname))&&(MyRexxHost->portname[k]!='.');k++);

		if(MyRexxHost->portname[k]=='.')
		{
			stcd_l(&MyRexxHost->portname[k+1],&filexid);
		}
		else
			filexid=0;
	}

#else

	filexid = 0;

#endif	/* AREXX */

		/* Alles wieder freigeben */

	if( cliargs )
		FreeArgs( cliargs );
	else
		if( dob )
			FreeDiskObject(dob);

	return( NULL );
}

void __saveds NewTaskStart( void )
{
	char *errorstring;

	if((errorstring = ParseArgsAndAllocThings()))
	{
		MyRequestNoLocale( errorstring , 0 );
	}
	else
	{
	#ifndef __AROS__
		if( 1 != ChecksumCorrect())
			Gedenken();
    	#endif
		DoWindow();
	}
}

	/* ProcessCleanup(register __d1 BPTR SegList):
	 *
	 *	Frees all resource the main process has allocated when
	 *	it exits.
	 */

#ifdef __AROS__
STATIC VOID ProcessCleanup(ULONG retcode, BPTR SegList)
#else
STATIC VOID __saveds __asm
ProcessCleanup(register __d1 BPTR SegList)
#endif
{
	CloseAll();

	Forbid();

	UnLoadSeg(SegList);

	CloseLibs();
}

	/* SegmentSplit(STRPTR Name,BYTE Pri,LONG StackSize,APTR Function):
	 *
	 *	Create a new process from the current one.
	 */

STATIC struct Process *
SegmentSplit(STRPTR Name,LONG StackSize,APTR Function)
{
	struct Process			*Child;
	struct CommandLineInterface	*CLI;

	CLI = (struct CommandLineInterface *)BADDR(((struct Process *)SysBase -> ThisTask) -> pr_CLI);

	Forbid();

	Child = CreateNewProcTags(
		NP_CommandName,	(LONG)"FileX",
		NP_Name,	Name,
		NP_StackSize,	StackSize,
		NP_Entry,	Function,
		NP_Cli,		TRUE,
		NP_ExitCode,	ProcessCleanup,
		NP_ExitData,	CLI->cli_Module,
	TAG_DONE);

	if(Child)
		CLI -> cli_Module = NULL;

	Permit();

	return(Child);
}

int main(void)
{
	UBYTE *errorstring;
	struct Process *myproc;

	myproc = (struct Process *)SysBase -> ThisTask;

	if( myproc->pr_CLI )
	{
			/* Clistart */

		if((errorstring = OpenLibs()))
		{
			if( ReqToolsBase || IntuitionBase )
				MyRequestNoLocale( errorstring , 0 );
			else
				Printf( "%s\n", errorstring );

			CloseLibs();
			return(RETURN_FAIL);
		}

			/* Programmnamen bestimmen */

		GetProgramName(programname,256);

		if(!(cliargs=ReadArgs("PUBSCREEN/K,PORTNAME/K,STARTUP/K,SETTINGS/K,COMMANDS/K,LANGUAGE/K,KEY/K,SYNC/S,FILE",(LONG *)&myargs,0)))
		{
			PrintFault( IoErr(), "FileX");
			CloseLibs();
			return(RETURN_FAIL);
		}

		if(!myargs.ma_Sync)
		{
				/* Create a new process from our code. */

			if(!SegmentSplit("FileX",16384,NewTaskStart))
			{
				Printf(GetStr(MSG_INFO_GLOBAL_FAILEDTOCREATEPROCESS));

				return(RETURN_FAIL);
			}
		}
		else
		{
			if((errorstring = ParseArgsAndAllocThings()))
				MyRequestNoLocale( errorstring , 0 );
			else
			{
				if( 1 != ChecksumCorrect())
					Gedenken();

				DoWindow();
			}

			CloseAll();
			CloseLibs();
		}
	}
	else
	{
		struct WBArg *wbarg;
		struct WBStartup		*WBenchMsg;
		BPTR WBStartLock;				/* Lock der auf das aktuelle Verzeichnis */
											/* beim Start von der Workbench zeigt */

		WaitPort(&myproc->pr_MsgPort);
		WBenchMsg = (struct WBStartup *)GetMsg(&myproc->pr_MsgPort);

		if((errorstring=OpenLibs()))
		{
			if( ReqToolsBase || IntuitionBase )
				MyRequestNoLocale( errorstring , 0 );
			else
				Printf( "%s\n", errorstring );

			CloseLibs();
			return(RETURN_FAIL);
		}

		WBStartLock = CurrentDir(WBenchMsg->sm_ArgList->wa_Lock);

		wbarg=WBenchMsg->sm_ArgList;

		NameFromLock(wbarg->wa_Lock,programname,256);
		AddPart(programname,wbarg->wa_Name,256);

		dob = GetDiskObject(programname);

		if((errorstring = ParseArgsAndAllocThings()))
			MyRequestNoLocale( errorstring , 0 );
		else
		{
			if( 1 != ChecksumCorrect())
				Gedenken();

			if(WBenchMsg->sm_NumArgs > 1)
			{
				CurrentDir((WBenchMsg->sm_ArgList + 1)->wa_Lock);

				MyOpen((WBenchMsg->sm_ArgList + 1)->wa_Name, AktuDD);
			}

			DoWindow();
		}

		CurrentDir(WBStartLock);

		CloseAll();
		CloseLibs();

		Forbid();

		ReplyMsg((struct Message *)WBenchMsg);
	}

	return(RETURN_OK);
}

void About(void)
{

#ifdef __M68020
#define CPUSTRING "68020"
#else
#define CPUSTRING "68000"
#endif

	if( 1 == MyFullRequest( MSG_INFO_GLOBAL_ABOUTTEXT1, MSG_GADGET_ABOUT1, CPUSTRING ,VSTRING, DATE ))
	if( 1 == MyFullRequest( MSG_INFO_GLOBAL_ABOUTTEXT2, MSG_GADGET_ABOUT2, 0 ))
	{
		if( regname[0] )
			MyFullRequest( MSG_INFO_GLOBAL_REGISTERED, MSG_GADGET_REGISTERED, regnum, regname );
		else
 			MyFullRequest( MSG_INFO_GLOBAL_NOTREGISTERED, MSG_GADGET_REGISTERED, 0 );
	}
}

void MyBeep(void)
{
	DisplayBeep(Scr);
}

/*
 * BOOL LoadUserDisplaytyp(char *fullname)
 *
 * Öffnet einen FileRequester und läd einen userdefined Displaytyp ein
 */

BOOL LoadUserDisplaytyp(char *fullname)
{
	BPTR fh;
	BOOL Success=FALSE;

	if((fh=Open(fullname,MODE_OLDFILE)))
	{
		Read(fh,displaytab[3],256);

		SetDisplayTyp(3, AktuDD );

		Success=TRUE;

		Close(fh);
	}
	else
		MyRequest( MSG_INFO_GLOBAL_CANTOPENFILE, ( ULONG )fullname );

	return(Success);
}

/*
 * BOOL LoadUserDisplaytypFR(void)
 *
 * Öffnet einen FileRequester und läd einen userdefined Displaytyp ein
 */

BOOL LoadUserDisplaytypFR(void)
{
	static UBYTE DisplaytypFullName[256];

	if( MyRequestFile( DisplaytypFullName, GetStr(MSG_WINDOWTITLE_SELECTDISPLAYFILE), 0, FALSE ))
		return( LoadUserDisplaytyp( DisplaytypFullName ));
	else
		return( FALSE );
}

/*
 * void SetScrollerGadget(void)
 *
 * Setz das Scrollergadget auf den richtigen Wert, d.h.
 * laenge+DD->Zeilen => Größe des Knobs
 * ScrollerPos => Position
 */

void SetScrollerGadget( struct DisplayData *DD )
{
	LONG Gesamtzeilen=( DD->FD->Len + DD->BPR - 1 ) / DD->BPR - DD->Zeilen;

	if(Gesamtzeilen<65000)
		SetGadgetAttrs(DD->PropGadget,DD->DI->Wnd,0,
					PGA_Total,(Gesamtzeilen < 0) ? 0 : Gesamtzeilen+DD->Zeilen,
					PGA_Top,(ULONG)DD->SPos,
					PGA_Visible,DD->Zeilen,
					TAG_DONE);
	else
	{
		SetGadgetAttrs(DD->PropGadget,DD->DI->Wnd,0,
					PGA_Total,65001,
					PGA_Top,(ULONG)DD->SPos*650/Gesamtzeilen*100,
					PGA_Visible,1,
					TAG_DONE);
	}
}

/*
 * long GetFileLaenge(BPTR fh)
 *
 * Ermittelt die Länge des Files mit der FileHandlestruktur fh
 * (Lange, aber schnelle Version)
 */

long GetFileLaenge(BPTR fh)
{
	struct FileInfoBlock *fib;
	LONG size=0;

	if((fib=(struct FileInfoBlock *)AllocDosObject(DOS_FIB,NULL)))
	{
		ExamineFH(fh,fib);

		size=fib->fib_Size;

		FreeDosObject(DOS_FIB,fib);
	}

	return(size);
}

/*
 * long GetFileLaengeName(char *name)
 *
 * Ermittelt die Länge des Files mit dem Namen name
 * (Lange, aber schnelle Version)
 */

long GetFileLaengeName(char *name)
{
	struct FileInfoBlock *fib;
	LONG size=0;

	if((fib=(struct FileInfoBlock *)AllocDosObject(DOS_FIB,NULL)))
	{
		BPTR FileLock;

		if((FileLock = Lock(name,ACCESS_READ)))
		{
			Examine(FileLock,fib);

			size=fib->fib_Size;

			UnLock(FileLock);
		}

		FreeDosObject(DOS_FIB,fib);
	}

	return(size);
}

void FreeNLNode( struct Node *node )
{
	if( node )
	{
		Remove( node );
		if( node ->ln_Name ) FreeMem(node->ln_Name,strlen(node->ln_Name)+1);
		FreeMem(node,sizeof(struct Node));
	}
}

void AddNLName( struct List *List, char *Name )
{
	struct Node *node;

	if(( node = AllocMem( sizeof( struct Node ), MEMF_CLEAR )))
	{
		if(( node->ln_Name = AllocMem( strlen( Name ) + 1, MEMF_ANY )))
			strcpy( node->ln_Name, Name );

		AddTail( List, node );
	}
}

struct FileData *AllocFD( WORD Typ )
{
	struct FileData *FD;

	if(( FD = AllocVec( sizeof( struct FileData ), MEMF_CLEAR )))
	{
		AddTail( &FileList, &FD->Node );

		FD->Typ = Typ;

		NewList( &FD->DisplayList );

		if(( FD->Name = AllocVec( 256, MEMF_CLEAR )))
		{
			if( AllocUndostructs( FD ))
			{
				int k;

				FD->LocationNumber = 1;
				
				for( k = 0; k < 10; k++ )
					FD->Locations[ k ] = -1;
			}
			else
			{
				FreeFD( FD );
				FD = 0;
			}
		}
		else
		{
			FreeFD( FD );
			FD = 0;
		}
	}

	return( FD );
}

void FreeFD( struct FileData *FD )
{
	if( !FD )
		return;

	Remove( &FD->Node );

	if( FD->Name )
	{
		FreeVec( FD->Name );
		FD->Name = 0;
	}

	if( FD->Typ == FD_FILE )
	if( FD->Mem && FD->RLen )
	{
		FreeMem( FD->Mem, FD->RLen );
		FD->Mem = 0;
		FD->RLen = FD->Len = 0;
	}

	FreeUndostructs( FD );
	FreeVec( FD );
}

void FreeDD( struct DisplayData *DD )
{
	if( !DD )
		return;

	if( DD->PropGadget )
	{
		if( DD->Wnd )
			RemoveGadget( DD->Wnd, DD->PropGadget );
		
		DisposeObject((APTR) DD->PropGadget );
		DD->PropGadget = 0;
	}

	if( ElementZahl( &DD->FD->DisplayList ) == 1 )
		FreeFD( DD->FD );
	else
		Remove( &DD->FNode );

	Remove( &DD->Node );

	FreeVec( DD );
}

struct DisplayData *AllocDD( struct DisplayInhalt *DI, struct FileData *FD )
{
	struct DisplayData *DD;

	if(( DD = AllocVec( sizeof( struct DisplayData ), MEMF_CLEAR )))
	{
			/* Defaultwerte setzen */

		DD->DisplayForm = DefaultDisplayForm;
		DD->DisplaySpaces = DefaultDisplaySpaces;
		DD->Zeilen = DefaultZeilen;
		DD->BPR = DefaultBPR;
		DD->DisplayTyp = DefaultDisplayTyp;

			/* andere Werte setzten */

		DD->Wnd = DI->Wnd;
		DD->FD = FD;
		DD->DI = DI;

		AddTail( &FD->DisplayList, &DD->FNode );
		AddTail( &DI->DisplayList, &DD->Node );
	}

	return( DD );
}

void FreeDI( struct DisplayInhalt *DI )
{
	if( !DI )
		return;

	while( !IsListEmpty( &DI->DisplayList ))
		FreeDD(( struct DisplayData * )DI->DisplayList.lh_Head );

	Remove( &DI->Node );

	FreeVec( DI );
}

struct DisplayInhalt *AllocDI( void )
{
	struct DisplayInhalt *DI;

	if(( DI = AllocVec( sizeof( struct DisplayInhalt ), MEMF_CLEAR )))
	{
			/* Defaultfont setzen */

		MySetFontNameSize( DefaultFontName, DefaultFontSize, DI );

		DI->WindowTop = DefaultTop;
		DI->WindowLeft = DefaultLeft;

		AddTail( &DisplayInhaltList, &DI->Node );
		NewList( &DI->DisplayList );
	}

	return( DI );
}

BOOL MyOpen( char *Name, struct DisplayData *DD )
{
	BPTR fh;
	BOOL Success=FALSE;
	struct FileData *FD;

	FD = DD->FD;

	if( !IsListEmpty( &OldFileList ))
	{
		if( !FindName( &OldFileList, Name ))
			AddNLName( &OldFileList, Name);
	}
	else
		AddNLName( &OldFileList, Name);

		/* File versuchen zu öffnen */

	fh = Open( Name , MODE_OLDFILE );

	if( ElementZahl( &FD->DisplayList ) == 1 )
		FreeFD( FD );
	else
		Remove( &DD->FNode );

	FD = AllocFD( FD_FILE );
	AddTail( &FD->DisplayList, &DD->FNode );
	DD->FD = FD;

	if( fh )
	{
		FD->RLen = FD->Len = GetFileLaenge( fh );

			/* Prüfen, ob das File einen Inhalt hat */

		if( FD->RLen )
		{
			if(( FD->Mem = AllocMem( FD->Len, MEMF_ANY )))
			{
				if( FD->Len == Read( fh, FD->Mem, FD->RLen ))
					Success = TRUE;
			}
			else
			{
				MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, FD->Len );
				FD->RLen = FD->Len = 0;
			}
		}
	}

		/* Falls File nicht geöffnet werden konnte oder Länge 0,
		 * einen Dummyeintrag erzeugen */

	if( !fh || !FD->RLen )
	{
			/* Das File ist leer. Trotzdem ein Byte allokieren, */
			/* damit es eingeladen werden kann. */

		FD->Len = 0;
		FD->RLen = 1;

		if(( FD->Mem = AllocMem( FD->RLen, MEMF_ANY )))
		{
			Success = TRUE;
		}
		else
		{
			MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, FD->Len );
			FD->RLen = FD->Len = 0;
		}
	}

	strcpy( FD->Name, Name );

		/* Falls das Window geöffnet ist, alles neudarstellen */

	if( DD->DI->Wnd )
	{
		CursorOff( DD );
		DD->SPos = 0;
		SetScrollerGadget( DD );
		RedrawDisplay( DD );
		DD->CPos = 0;
		CursorOn( DD );
		UpdateStatusZeile( DD );
	}

	if( fh )
		Close( fh );

	if( !Success )
		ClearDDFile( DD, TRUE );

	return( Success );
}

BOOL FileLoaded( struct FileData *FD )
{
	return(( BOOL )*FilePart( FD->Name ));
}

void PrintFile( struct FileData * FD )
{
	struct DisplayData *FirstDD;

	FirstDD = GetDDFromFD( FD->DisplayList.lh_Head );

	MyPrint( FD->Mem, FD->Len, FirstDD->DisplayTyp, FirstDD->DI->Wnd );
}

BOOL SaveFile( struct FileData *FD )
{
	if( *FilePart( FD->Name ))
		return( Save( FD ));
	else
		return( SaveAsFile( FD ));
}

BOOL OpenFile( struct DisplayData *DD )
{
	char Buffer[ 256 ];

	strcpy( Buffer, DD->FD->Name );

	if( MyRequestFile( Buffer, GetStr(MSG_WINDOWTITLE_OPENFILE), 0, TRUE ))
		return( MyOpen( Buffer, DD ));
	else
		return( FALSE );
}

void ClearDDFile( struct DisplayData *DD, BOOL Force )
{
	struct FileData *FD;

	if( Force || QuitView( 3, DD ))
	{
		FD = AllocFD( FD_FILE );
	
		strcpy( FD->Name, DD->FD->Name );
		*FilePart( FD->Name ) = 0;
	
		if( ElementZahl( &DD->FD->DisplayList ) == 1 )
			FreeFD( DD->FD );
		else
			Remove( &DD->FNode );
	
		DD->FD = FD;
		DD->CPos = 0;
	
		AddTail( &FD->DisplayList, &DD->FNode );
	
		if( DD->Wnd )
		{
			CursorOff( DD );
			UpdateStatusZeile( DD );
			SetScrollerGadget( DD );
			RedrawDisplay( DD );
		}
	}
}

BOOL SaveNewName( char *name, struct FileData *FD )
{
	strcpy( FD->Name, name );
	return( Save( FD ));
}

BOOL Save( struct FileData *FD )
{
	BPTR fh;
	BOOL Success=FALSE;

		/* Prüfen, ob File existiert. Falls ja, fragen, ob es überschrieben */
		/* werden soll */

	if(!(mainflags&MF_OVERWRITE))
		if(GetFileLaengeName(FD->Name))
			if(0 == MyFullRequest( MSG_INFO_GLOBAL_FILEALREADYEXISTS, MSG_GADGET_YESNO, ( ULONG )FD->Name ))
				return(Success);

	if((fh=Open(FD->Name,MODE_NEWFILE)))
	{
		if(FD->Len!=Write(fh,FD->Mem,FD->Len))
		{
						/* Falls nicht vollständig geschrieben, löschen */

			Close( fh );
			MyRequest( MSG_INFO_GLOBAL_CANTSAVEFILE, ( ULONG )FD->Name );
			DeleteFile( FD->Name );
		}
		else
		{
			FD->FullChanges=FD->RedoChanges=FD->Changes=0;
			Close(fh);

			UpdateAllStatusZeilen( FD );
		
			Success=TRUE;
		}
	}
	else
		MyRequest( MSG_INFO_GLOBAL_CANTSAVEFILE, ( ULONG )FD->Name );

	return(Success);
}

void UpdateAllStatusZeilen( struct FileData *FD )
{
	struct DisplayData *DD;

		/* Alle Statuszeilen des File neuzeichnen */

	DD = GetDDFromFD( FD->DisplayList.lh_Head );

	while( DD != GetDDFromFD( &FD->DisplayList.lh_Tail ))
	{
		UpdateStatusZeile( DD );

		DD = GetDDFromFD( DD->FNode.ln_Succ );
	}
}

BOOL SaveAsFile( struct FileData *FD )
{
	if( MyRequestFile( FD->Name, GetStr(MSG_WINDOWTITLE_SAVEFILEAS ), 0, TRUE ))
	{
		return( Save( FD ));
	}
	else
		return( FALSE );
}

void DisplayFromScroller( struct DisplayData *DD )
{
	ULONG newasp,oldasp=DD->SPos,oldpos=DD->CPos;

	GetAttr(PGA_Top,(APTR)DD->PropGadget,&newasp);

	if((DD->FD->Len+DD->BPR-1)/DD->BPR-DD->Zeilen>=65000)
		newasp=newasp/650*(((DD->FD->Len+DD->BPR-1)/DD->BPR)-DD->Zeilen)/100;
	
	DD->CPos += ( newasp - oldasp ) * DD->BPR;

	UpdateStatusZeile( DD );
	RedrawDisplay( DD );

	if(DD->Flags&DD_MARK)ChangeMark(oldpos,DD->CPos,oldasp,DD->SPos, DD);
}

void Mark( struct DisplayData *DD )
{
	if(DD->FD->Len!=0)
	{
		if(DD->Flags&DD_MARK)
		{
			DD->Flags&=~DD_MARK;
			MarkOff( DD );
			CursorOn( DD );
		}
		else
		{
			DD->Flags|=DD_MARK;
			DD->MPos = DD->CPos;

			MarkOn( DD );
		}
	}
}

void Palette(void)
{
	if( ReqToolsBase )
	{
		if(rtPaletteRequest(GetStr(MSG_WINDOWTITLE_PALETTE),NULL,AktuDI->Wnd ? RT_Window : TAG_IGNORE, AktuDI->Wnd,TAG_DONE))
		{
			ULONG k,color;
	
			for(k=0;k<8;k++)
			{
				color=GetRGB4(Scr->ViewPort.ColorMap,k);
				ScreenColors[k].ColorIndex=k;
				ScreenColors[k].Red  =(color >> 8)&0xf;
				ScreenColors[k].Green=(color >> 4)&0xf;
				ScreenColors[k].Blue =(color     )&0xf;
			}
		}
	}
	else
		DisplayLocaleText( MSG_INFO_GLOBAL_REQTOOLSLIBRARYREQUIRED );
}

/*void PrintIntuiMessage( struct IntuiMessage *m )
{
	char *classstring;

	if( m->Class == IDCMP_INTUITICKS ) return;

	switch( m->Class )
	{
		case IDCMP_SIZEVERIFY:
			classstring = "SIZEVERIFY";
			break;
		case IDCMP_NEWSIZE:
			classstring = "NEWSIZE";
			break;
		case IDCMP_REFRESHWINDOW:
			classstring = "REFRESHWINDOW";
			break;
		case IDCMP_MOUSEBUTTONS:
			classstring = "MOUSEBUTTONS";
			break;
		case IDCMP_MOUSEMOVE:
			classstring = "MOUSEMOVE";
			break;
		case IDCMP_GADGETDOWN:
			classstring = "GADGETDOWN";
			break;
		case IDCMP_GADGETUP:
			classstring = "GADGETUP";
			break;
		case IDCMP_REQSET:
			classstring = "REQSET";
			break;
		case IDCMP_MENUPICK:
			classstring = "MENUPICK";
			break;
		case IDCMP_CLOSEWINDOW:
			classstring = "CLOSEWINDOW";
			break;
		case IDCMP_RAWKEY:
			classstring = "RAWKEY";
			break;
		case IDCMP_REQVERIFY:
			classstring = "REQVERIFY";
			break;
		case IDCMP_REQCLEAR:
			classstring = "REQCLEAR";
			break;
		case IDCMP_MENUVERIFY:
			classstring = "MENUVERIFY";
			break;
		case IDCMP_NEWPREFS:
			classstring = "NEWPREFS";
			break;
		case IDCMP_DISKINSERTED:
			classstring = "DISKINSERTED";
			break;
		case IDCMP_DISKREMOVED:
			classstring = "DISKREMOVED";
			break;
		case IDCMP_WBENCHMESSAGE:
			classstring = "WBENCHMESSAGE";
			break;
		case IDCMP_ACTIVEWINDOW:
			classstring = "ACTIVEWINDOW";
			break;
		case IDCMP_INACTIVEWINDOW:
			classstring = "INACTIVEWINDOW";
			break;
		case IDCMP_DELTAMOVE:
			classstring = "DELTAMOVE";
			break;
		case IDCMP_VANILLAKEY:
			classstring = "VANILLAKEY";
			break;
		case IDCMP_INTUITICKS:
			classstring = "INTUITICKS";
			break;
		case IDCMP_IDCMPUPDATE:
			classstring = "IDCMPUPDATE";
			break;
		case IDCMP_MENUHELP:
			classstring = "MENUHELP";
			break;
		case IDCMP_CHANGEWINDOW:
			classstring = "CHANGEWINDOW";
			break;
		case IDCMP_GADGETHELP:
			classstring = "GADGETHELP";
			break;
		defaul:
			classstring = "UNKNOWN";
	}

	kprintf( "Class: %16s, Code: %4lx, Qual: %4lx, IAdr: %8lx\n",classstring, m->Code, m->Qualifier, m->IAddress);
}
*/

static ULONG WaitSignals;
#define MAXSIGNALS 20
static void ( * SignalFunktionTab[ MAXSIGNALS ])( void );
static ULONG Signals[ MAXSIGNALS ];

void MyAddSignal( ULONG Signal, void ( * Funktion )())
{
	long k;

/*	kprintf("MyAddSignal(%8lx,%8lx)\n", Signal, Funktion );*/

	WaitSignals |= Signal;

	for( k = 0; k < MAXSIGNALS; k++ )
	{
		if( SignalFunktionTab[ k ] == 0)
		{
			SignalFunktionTab[ k ] = Funktion;
			Signals[ k ] = Signal;
			break;
		}
	}

/*	if( k == MAXSIGNALS )*/
/*		kprintf("Warnung!: Kein Platz mehr in Signalliste\n");*/
}

void MyRemoveSignal( ULONG Signal )
{
	long k;
	BOOL Found = FALSE;

/*	kprintf("MyRemoveSignal(%8lx)\n", Signal);*/

	WaitSignals &= ~Signal;

	for( k = 0; k < MAXSIGNALS; k++ )
	{
		if( Signals[ k ] == Signal )
		{
			SignalFunktionTab[ k ] = 0;
			Signals[ k ] = 0;
			if( !Found )
				Found = TRUE;
/*			else*/
/*				kprintf("Warnung!: Signal doppelt belegt!\n");*/
		}
	}
}

#define MAXWDWINDOWS 10
static struct WindowData *OpenWD[ MAXWDWINDOWS ];
static int OpenWDCount;

void MyAddWindow( struct WindowData *wd )
{
	int k;

	mainflags |= MF_WDWINDOWOPEN;

	for( k = 0; k < MAXWDWINDOWS; k++ )
	{
		if( !OpenWD[ k ])
		{
			OpenWD[ k ] = wd;
			break;
		}
	}

	OpenWDCount++;
}

void MyRemoveWindow( struct WindowData *wd )
{
	int k;

	OpenWDCount--;

	if( !OpenWDCount )
		mainflags &= ~MF_WDWINDOWOPEN;
	
	for( k = 0; k < MAXWDWINDOWS; k++ )
	{
		if( OpenWD[ k ] == wd)
		{
			OpenWD[ k ] = NULL;
			break;
		}
	}
}

void CloseAllOpenWindows( void )
{
	int k;

	for( k = 0; k < MAXWDWINDOWS; k++ )
	{
		if( OpenWD[ k ])
		{
			NewCloseAWindow( OpenWD[ k ]);
			OpenWD[ k ] = NULL;
		}
	}

	if( mainflags & MF_LISTREQ )
		CloseLR();
}

void __saveds DoWindow(void)
{
	ULONG signals;

		/* Stimmt die Checksumme nicht? */

	if( 0 == ChecksumCorrect())
	{
		DisplayLocaleText( MSG_INFO_GLOBAL_PATCHED );
	}
	else
	{
			/* Display öffnen */
	
		if(OpenDisplay()) return;
	
			/* Ggf. ein Startarexxscript ausführen */
	
#ifdef AREXX
		if(startuparexxscript)
		{
			BPTR fh;
	
			if(fh=Open(arexxcommandwindow,MODE_NEWFILE))
				SendRexxCommand(MyRexxHost,startuparexxscript,fh);
			FreeVec(startuparexxscript);
		}
#endif

		while( !( mainflags & MF_ENDE ))
		{
			long k;

/*			kprintf("Wait(%8lx)", WaitSignals );*/

			signals = Wait( WaitSignals );

/*			kprintf(" = %8lx\n", signals );*/

			for( k = 0; k < MAXSIGNALS; k++ )
			{
				if( SignalFunktionTab[ k ] )
					if( signals & Signals[ k ])
						SignalFunktionTab[ k ]();
			}
	
			if( mainflags & MF_ICONIFY )
			{
				mainflags &= ~MF_ICONIFY;

				if( MakeIconify() == FALSE )
					DisplayLocaleText( MSG_INFO_GLOBAL_ICONIFYFAILED );
			}
		}

		if( mainflags & MF_CALC )
			CloseCalc();
		if( mainflags & MF_CLIPCONV )
			CloseClipConv();

#ifdef __AROS__
#warning "stegerg: disabled for quick test!!"
#else
		CloseCommandShell();
#endif
		CloseAllOpenWindows();
	}
}

UBYTE *AllocRequester()
{ 
		/* Den Haupt-Filerequester allokieren */

	if( UseAsl )
	{
		if(!(Asl_FileFileReq=(struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
										ASL_FuncFlags,FILF_PATGAD|FILF_NEWIDCMP,
										TAG_DONE)))
			return(GetStr(MSG_INFO_GLOBAL_CANTALLOCFILEREQ));
	}
	else
	{
		if( !( FileFileReq = rtAllocRequestA( RT_FILEREQ, NULL )))
			return( GetStr( MSG_INFO_GLOBAL_CANTALLOCFILEREQ ));
	}

	return(0);
}

void FreeRequester(void)
{
	if( UseAsl )
	{
		if( Asl_FileFileReq ) FreeAslRequest( Asl_FileFileReq );
	}
	else
	{
		if( FileFileReq ) rtFreeRequest( FileFileReq );
	}
}

void LocalizeAll( void )
{
	LocalizeMenus();
}

static UBYTE *MyOpenLibrary(char *name, ULONG version, APTR *baseptr)
{
	if(!(*baseptr = OpenLibrary(name, version)))
	{
		sprintf(errorbuffer,GetStr(MSG_INFO_GLOBAL_CANTOPENLIBRARY), name, version);
		return(errorbuffer);
	}
	else
		return(0);
}

UBYTE *OpenLibs(void)
{
	char *errortext;

	LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library", 38);
	if((errortext = MyOpenLibrary("dos.library", 37, (APTR *)&DOSBase))) return(errortext);
	if((errortext = MyOpenLibrary("intuition.library", 37, (APTR *)&IntuitionBase))) return(errortext);
	if((errortext = MyOpenLibrary("graphics.library", 37, (APTR *)&GfxBase))) return(errortext);
	if((errortext = MyOpenLibrary("icon.library", 37, (APTR *)&IconBase))) return(errortext);
	if((errortext = MyOpenLibrary("commodities.library", 37, (APTR *)&CxBase))) return(errortext);
	if((errortext = MyOpenLibrary("asl.library", 37, (APTR *)&AslBase))) return(errortext);
	if((errortext = MyOpenLibrary("rexxsyslib.library", 36, (APTR *)&RexxSysBase))) return(errortext);
	if((errortext = MyOpenLibrary("utility.library", 37, (APTR *)&UtilityBase))) return(errortext);
	if((errortext = MyOpenLibrary("gadtools.library", 37, (APTR *)&GadToolsBase))) return(errortext);
	if((errortext = MyOpenLibrary("diskfont.library", 35, (APTR *)&DiskfontBase))) return(errortext);
	if((errortext = MyOpenLibrary("workbench.library", 37, (APTR *)&WorkbenchBase))) return(errortext);
	if((errortext = MyOpenLibrary("iffparse.library", 37, (APTR *)&IFFParseBase))) return(errortext);
	if((errortext = MyOpenLibrary("keymap.library", 37, (APTR *)&KeymapBase))) return(errortext);

	ReqToolsBase = ( struct ReqToolsBase * )OpenLibrary( REQTOOLSNAME, 38);

	if( ReqToolsBase ) UseAsl = FALSE;

/*	XpkBase=OpenLibrary(XPKNAME,0);*/

	return(0);
}

void CloseAll(void)
{
	long k;

		/* Display schließen */

	while(FALSE==CloseDisplay());

	if( SollPublicScreenName )
		FreeVec( SollPublicScreenName );

	if( TextLineBuffer ) FreeVec( TextLineBuffer );
	FreeTempBitMap();

	FreeRequester();

	while( !IsListEmpty( &OldFileList ))
		FreeNLNode( OldFileList.lh_Head );

	while( !IsListEmpty( &DisplayInhaltList ))
		FreeDI(( struct DisplayInhalt * )DisplayInhaltList.lh_Head );

	while( !IsListEmpty( &FileList ))
		FreeFD(( struct FileData * )FileList.lh_Head );

		/* Alle ARexxcommands freigeben */

	for(k=0;k<10;k++)
		if(arexxcommands[k])FreeVec(arexxcommands[k]);

		/* Windowstrings freigeben */

	if(arexxcommandshellwindow)FreeVec(arexxcommandshellwindow);
	if(arexxcommandwindow)FreeVec(arexxcommandwindow);

#ifdef AREXX
	if(MyRexxHost)CloseDownARexxHost(MyRexxHost);
#endif
	FreeSearchHistory();

		/* Windowport freigeben */

	MyRemoveSignal( 1L << WndPort->mp_SigBit );
	DeleteMsgPort( WndPort );

		/* Appport freigeben */

	MyRemoveSignal( 1L << AppPort->mp_SigBit );
	DeleteMsgPort( AppPort );

		/* ReplyPort freigeben */

	FreeReplyPort();

	SetTaskPri( FindTask( NULL ), oldtaskpri );

	if( LocaleBase )
	{
		CloseCatalog(Catalog);
		CloseLocale(Locale);
	}
}

void CloseLibs(void)
{
/*	if(XpkBase) CloseLibrary(XpkBase);*/

	CloseLibrary((struct Library *)ReqToolsBase);

	CloseLibrary(CxBase);
	CloseLibrary(KeymapBase);
	CloseLibrary((struct Library *)UtilityBase);
	CloseLibrary((struct Library *)IntuitionBase);
	CloseLibrary((struct Library *)GfxBase);
	CloseLibrary(GadToolsBase);
	CloseLibrary(DiskfontBase);
	CloseLibrary(AslBase);
	CloseLibrary(RexxSysBase);
	CloseLibrary((struct Library *)IconBase);
	CloseLibrary(WorkbenchBase);
	CloseLibrary(IFFParseBase);
	CloseLibrary((struct Library *)DOSBase);

	CloseLibrary((struct Library *)LocaleBase);
}

#define GHOEHE 16
#define GBREITE 62

void Gedenken( void )
{
	struct Window *Wnd;
	struct TextAttr ta;
	struct TextFont *tf;
	APTR VisualInfo;
	struct Screen *Scr;
	int k;

	Scr = LockPubScreen( NULL );

	ta.ta_Name = GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name;
	ta.ta_YSize = GfxBase->DefaultFont->tf_YSize;
	ta.ta_Style = 0;
	ta.ta_Flags = 0;

	if( !( tf = OpenDiskFont( &ta )))
	{
		ta.ta_Name = "topaz.font";
		ta.ta_YSize = 8;
		tf = OpenDiskFont( &ta );
	}

	Wnd = OpenWindowTags( NULL,
				WA_Left,	(Scr->Width - GBREITE * tf->tf_XSize - 8)/2,
				WA_Top,	(Scr->Height - GHOEHE * tf->tf_YSize - 4)/2,
/*				WA_Title,	"FileX 2.0 Nerv Requester",*/
				WA_InnerWidth,	GBREITE * tf->tf_XSize + 8,
				WA_InnerHeight,GHOEHE * tf->tf_YSize + 4,
				WA_Flags,		WFLG_RMBTRAP | WFLG_BORDERLESS | WFLG_SMART_REFRESH | WFLG_ACTIVATE,
				WA_PubScreen,	Scr,
				TAG_DONE );

	if( Wnd && tf )
	{
		SetRast( Wnd->RPort, 3 );

		if(( VisualInfo = GetVisualInfo( Wnd->WScreen, TAG_DONE )))
		{
			DrawBevelBox( Wnd->RPort, Wnd->BorderLeft, Wnd->BorderTop, Wnd->Width - Wnd->BorderRight - Wnd->BorderLeft, Wnd->Height - Wnd->BorderTop - Wnd->BorderBottom, GT_VisualInfo, VisualInfo, TAG_DONE);
			FreeVisualInfo( VisualInfo );
		}

		if( tf )
			SetFont( Wnd->RPort, tf );

		SetAPen( Wnd->RPort, 1 );
		SetBPen( Wnd->RPort, 3 );

		for( k = 0; k < 14; k++ )
		{
			Move( Wnd->RPort, Wnd->BorderLeft + 4, Wnd->BorderTop + tf->tf_Baseline + tf->tf_YSize * (k+1) + 2);
			Text( Wnd->RPort, GetStr(MSG_INFO_GLOBAL_NERVREQUESTER) + k * GBREITE, GBREITE );
		}

		Delay( 200 );

			/* Entweder 16 Sekunden oder 2 min warten :-) */

		for( k = 0; k < 40; k++)
		{
			ActivateWindow( Wnd );
			WindowToFront( Wnd );
			ScreenToFront( Wnd->WScreen );
			Delay( 20 );
		}
	}
	else
	{
		Delay( 400 );
	}

	if( Scr ) UnlockPubScreen( NULL, Scr );
	if( Wnd ) CloseWindow( Wnd );
	if( tf ) CloseFont( tf );
}
