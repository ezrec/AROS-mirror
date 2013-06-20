#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <libraries/reqtools.h>
#include <libraries/asl.h>
#include <libraries/locale.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>
#include <proto/iffparse.h>
#include <proto/asl.h>
#include <proto/reqtools.h>
#include <proto/locale.h>
#include <clib/alib_protos.h>

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#endif

#include "filexstructs.h"
#include "filexstrings.h"
#include "filexarexx.h"
#include "allprotos.h"

BOOL IsHexString(UBYTE *string)
{
	while(*string)
	{
		if(!isxdigit(*string))return(FALSE);
		string++;
	}
	return(TRUE);
}

/*
 * BOOL MyGetString( char *String, char *WindowTitle, ULONG Max )
 * 
 * Öffnet ein Window mit Stringgadget zur Eingabe eines Strings.
 */

BOOL MyGetString( char *String, char *WindowTitle, ULONG Max )
{
	enum {
		GD_GETSTRING_STRING,
		GD_GETSTRING_OK,
		GD_GETSTRING_CANCEL
	};

	static struct MyNewGadget GetStringNewGadgets[] =
	{
		{STRING_KIND, 0, 0, 0, MSG_GADGET_STRING_STRING, 0, 8, 0, 0, 30},
	
		{BUTTON_KIND, GP_LEFTBOTTOM, 0, 0, MSG_GADGET_OK, 0, 0, 0, 0, 0},
		{BUTTON_KIND, GP_RIGHTBOTTOM, 0, 0, MSG_GADGET_CANCEL, 0, 0, 0, 0, 0},
		{0}
	};

	static struct WindowData GetStringWD =
	{
		NULL,	NULL,	FALSE, NULL, NULL,
		0,0,
		&GetStringNewGadgets[ 0 ], 3
	};

	BOOL Result = FALSE;

	long err;

	GetStringNewGadgets[ GD_GETSTRING_STRING ].Max = Max;
	GetStringNewGadgets[ GD_GETSTRING_STRING ].CurrentValue = ( IPTR )String;

	if(( err = NewOpenAWindow( &GetStringWD, WindowTitle )))
		MyRequest( MSG_INFO_GLOBAL_CANTOPENWINDOW, err );
	else
	{
		int EndeFlag = FALSE;
		ULONG signals;
		struct IntuiMessage	*m, Msg;
		struct Gadget *gad;

		ActivateGadget( GetStringWD.Gadgets[ GD_GETSTRING_STRING ], GetStringWD.Wnd, 0);
	
		while( EndeFlag == FALSE)
		{
			signals=Wait(1 << GetStringWD.Wnd->UserPort->mp_SigBit);
	
			if(signals&(1 << GetStringWD.Wnd->UserPort->mp_SigBit))
				while((m = GT_GetIMsg( GetStringWD.Wnd->UserPort)))
				{
					CopyMem((char *)m, (char *)&Msg, (long)sizeof(struct IntuiMessage));
	
					GT_ReplyIMsg(m);
	
					KeySelect(GetStringWD.Gadgets, &Msg);
	
					gad = (struct Gadget *) Msg.IAddress;
	
					switch( Msg.Class )
					{
						case	IDCMP_REFRESHWINDOW:
							GT_BeginRefresh( GetStringWD.Wnd );
							GT_EndRefresh( GetStringWD.Wnd, TRUE );
							break;

						case	IDCMP_VANILLAKEY:
							if( Msg.Code == 13)
							{
								EndeFlag = TRUE;
								Result = TRUE;
							}
							break;
	
						case	IDCMP_CLOSEWINDOW:
							EndeFlag = TRUE;
							break;
	
						case IDCMP_MOUSEMOVE:
							((struct MyNewGadget *)gad->UserData)->CurrentValue = Msg.Code;
							break;
	
						case	IDCMP_GADGETUP:
							switch(gad->GadgetID)
							{
								case GD_GETSTRING_OK:
								case GD_GETSTRING_STRING:
									Result = TRUE;
									EndeFlag = TRUE;
									break;
								case GD_GETSTRING_CANCEL:
									EndeFlag = TRUE;
									break;
							}
							break;
					}
				}
		}

		if( Result )
			strcpy( String, GetString( GetStringWD.Gadgets[ GD_GETSTRING_STRING ]));

		NewCloseAWindow( &GetStringWD );
	}

	return( Result );
}

/*
 * BOOL GetWert( ULONG *Wert, char *WindowTitle, ULONG Min, ULONG Max )
 * 
 * Öffnet ein Window mit Slidergadget zum Einstellen eines Wertes.
 */

BOOL GetWert( ULONG *Wert, char *WindowTitle, ULONG Min, ULONG Max )
{
	enum {
		GD_GETWERT_NUMBER,
		GD_GETWERT_OK,
		GD_GETWERT_CANCEL
	};

	static struct MyNewGadget GetWertNewGadgets[] =
	{
		{SLIDER_KIND, 0, 0, 0, MSG_GADGET_NUMBER_NUMBER, 0, 8, 0, 8, 0},
	
		{BUTTON_KIND, GP_LEFTBOTTOM, 0, 0, MSG_GADGET_OK, 0, 0, 0, 0, 0},
		{BUTTON_KIND, GP_RIGHTBOTTOM, 0, 0, MSG_GADGET_CANCEL, 0, 0, 0, 0, 0},
		{0}
	};

	static struct WindowData GetWertWD =
	{
		NULL,	NULL,	FALSE, NULL, NULL,
		0,0,
		&GetWertNewGadgets[ 0 ], 14
	};

	BOOL Result = FALSE;

	long err;

	GetWertNewGadgets[ GD_GETWERT_NUMBER ].Max = Max;
	GetWertNewGadgets[ GD_GETWERT_NUMBER ].Min = Min;
	GetWertNewGadgets[ GD_GETWERT_NUMBER ].CurrentValue = *Wert;

	if(( err = NewOpenAWindow(	&GetWertWD, GetStr( WindowTitle ))))
		MyRequest( MSG_INFO_GLOBAL_CANTOPENWINDOW, err );
	else
	{
		int EndeFlag = FALSE;
		ULONG signals;
		struct IntuiMessage	*m, Msg;
		struct Gadget *gad;

		while( EndeFlag == FALSE)
		{
			signals=Wait(1 << GetWertWD.Wnd->UserPort->mp_SigBit);
	
			if(signals&(1 << GetWertWD.Wnd->UserPort->mp_SigBit))
				while((m = GT_GetIMsg( GetWertWD.Wnd->UserPort)))
				{
					CopyMem((char *)m, (char *)&Msg, (long)sizeof(struct IntuiMessage));
	
					GT_ReplyIMsg(m);
	
					KeySelect(GetWertWD.Gadgets, &Msg);
	
					gad = (struct Gadget *) Msg.IAddress;
	
					switch( Msg.Class )
					{
						case	IDCMP_REFRESHWINDOW:
							GT_BeginRefresh( GetWertWD.Wnd );
							GT_EndRefresh( GetWertWD.Wnd, TRUE );
							break;
	
						case	IDCMP_VANILLAKEY:
							if( Msg.Code == 13)
							{
								EndeFlag = TRUE;
								Result = TRUE;
							}
							else
							if(( Msg.Code >= '0' ) && ( Msg.Code <= '9' ))
							{
								if( Msg.Code == '0' )
									Msg.Code += 10;
								GT_SetGadgetAttrs( GetWertWD.Gadgets[ GD_GETWERT_NUMBER ] , GetWertWD.Wnd, NULL,
									GTSL_Level, Msg.Code - '0',
									TAG_DONE);
								GetWertNewGadgets[ GD_GETWERT_NUMBER ].CurrentValue = Msg.Code - '0';
							}
							break;
	
						case	IDCMP_CLOSEWINDOW:
							EndeFlag = TRUE;
							break;
	
						case IDCMP_MOUSEMOVE:
							((struct MyNewGadget *)gad->UserData)->CurrentValue = Msg.Code;
							break;
	
						case	IDCMP_GADGETUP:
							switch(gad->GadgetID)
							{
								case GD_GETWERT_OK:
									Result = TRUE;
									EndeFlag = TRUE;
									break;
								case GD_GETWERT_CANCEL:
									EndeFlag = TRUE;
									break;
							}
							break;
					}
				}
		}

		if( Result )
			*Wert = GetWertNewGadgets[ GD_GETWERT_NUMBER ].CurrentValue;

		NewCloseAWindow( &GetWertWD );
	}

	return( Result );
}

/*
 * static ULONG MyMainFullRequest( char *Text, char *ButtonText, APTR Args )
 * 
 * Öffnet eine Informationsrequester.
 */

static ULONG MyMainFullRequest( CONST_STRPTR Text, CONST_STRPTR ButtonText, APTR Args )
{
	if( UseAsl )
	{
		struct EasyStruct	Easy;
		ULONG Result;
		char *Gadgets;

		if(( Gadgets = AllocVec( strlen( ButtonText ) + 1, MEMF_ANY )))
		{
				/* Unterstriche aus dem Gadgettext entfernen */

			{
				char *gptr = Gadgets;
				CONST_STRPTR ptr = ButtonText;
	
				while( *ptr )
				{
					if( *ptr != '_' ) *gptr++ = *ptr++;
					else ptr++;
				}
	
				*gptr = 0;
			}

				/* Standard data. */
		
			Easy.es_StructSize = sizeof( struct EasyStruct );
			Easy.es_Flags = 0;
			Easy.es_Title = GetStr( MSG_WINDOWTITLE_INFO );
			Easy.es_TextFormat = ( STRPTR )Text;
			Easy.es_GadgetFormat = ( STRPTR )Gadgets;
		
			Result = EasyRequestArgs( AktuDI->Wnd, &Easy, NULL, Args);

			FreeVec( Gadgets );
		}
		else
			Result = 0;

		return( Result );
	}
	else
		return( rtEZRequestTags( Text, ButtonText, NULL, Args, RTEZ_ReqTitle, GetStr( MSG_WINDOWTITLE_INFO ), AktuDI->Wnd ? RT_Window : TAG_IGNORE, AktuDI->Wnd, RT_Underscore, '_', TAG_DONE ));
}

void DisplayLocaleText( CONST_STRPTR Text )
{
	MyMainFullRequest( GetStr( Text ), GetStr( MSG_GADGET_CONTINUE ), 0 );
}

ULONG MyRequest( CONST_STRPTR Text, IPTR Data )
{
	return( MyMainFullRequest( GetStr( Text ), GetStr( MSG_GADGET_CONTINUE ), &Data ));
}

ULONG MyRequestNoLocale( CONST_STRPTR Text, IPTR Data )
{
	return( MyMainFullRequest( Text, GetStr( MSG_GADGET_CONTINUE ), &Data ));
}

/* 
 * MyFullRequest( char *Text, char *ButtonText, ... )
 *
 * Öffnet einen Requester bestimmbarem Text und Gadgets und einer
 * beliebigen Anzahl von Argumenten
 */

ULONG __stdargs MyFullRequestNoLocale( CONST_STRPTR Text, CONST_STRPTR ButtonText, ... )
{
	ULONG Result;
	va_list VarArgs;

	va_start( VarArgs, ButtonText );
#if defined(__AROS__) && defined(__ARM_ARCH__)
        #warning "TODO: fix va_arg usage for ARM"
#else
	Result = MyMainFullRequest( Text, ButtonText, VarArgs );
#endif
	va_end( VarArgs );

	return( Result );
}

ULONG __stdargs MyFullRequest( CONST_STRPTR Text, CONST_STRPTR ButtonText, ... )
{
	ULONG Result;
	va_list VarArgs;

	va_start( VarArgs, ButtonText );
#if defined(__AROS__) && defined(__ARM_ARCH__)
        #warning "TODO: fix va_arg usage for ARM"
#else
	Result = MyMainFullRequest( GetStr( Text ), GetStr( ButtonText ), VarArgs );
#endif
	va_end( VarArgs );

	return( Result );
}

/*
 * char *GetStr(char *idstr)
 * 
 * Ermittelt, wenn möglich, einen übersetzten String aus dem ID-String.
 */

CONST_STRPTR GetStr(CONST_STRPTR idstr)
{
	CONST_STRPTR local;

	local = idstr + 2;
	if(LocaleBase) return(GetCatalogStr(Catalog, idstr[0] * 256L + idstr[1], local));
	else return(local);
}


enum {
	GD_JUMPTO_NUMBER,
	GD_JUMPTO_OK,
	GD_JUMPTO_CANCEL
};

static struct MyNewGadget JumptoNewGadgets[] =
{
	{HEX_KIND, 0, 0, 0, MSG_GADGET_JUMPTO_NUMBER, 0, 8, 0, 9, 0},

	{BUTTON_KIND, GP_LEFTBOTTOM, 0, 0, MSG_GADGET_OK, 0, 0, 0, 0, 0},
	{BUTTON_KIND, GP_RIGHTBOTTOM, 0, 0, MSG_GADGET_CANCEL, 0, 0, 0, 0, 0},
	{0}
};

static struct WindowData JumptoWD =
{
	NULL,	NULL,	FALSE, NULL, NULL,
	0,0,
	&JumptoNewGadgets[ 0 ], 3
};

void JumpToByte(void)
{
	static long wert = 0;
	long err;
	UBYTE NumberString[9];

	JumptoNewGadgets[ GD_JUMPTO_NUMBER ].CurrentValue = ( IPTR ) NumberString;
	sprintf( NumberString, "%lx", wert );

	if(( err = NewOpenAWindow( &JumptoWD, GetStr(MSG_WINDOWTITLE_JUMPTOBYTE))))
		MyRequest( MSG_INFO_GLOBAL_CANTOPENWINDOW, err );
	else
	{
		int EndeFlag = FALSE;
		ULONG signals;
		struct IntuiMessage	*m, Msg;
		struct Gadget *gad;
	
		BOOL wie = FALSE;		/* Wie wurde Window verlassen ? */

		ActivateGadget( JumptoWD.Gadgets[ GD_JUMPTO_NUMBER ], JumptoWD.Wnd, 0);
	
		while(EndeFlag==FALSE)
		{
			signals=Wait(1 << JumptoWD.Wnd->UserPort->mp_SigBit);
	
			if(signals&(1 << JumptoWD.Wnd->UserPort->mp_SigBit))
				while((m = GT_GetIMsg( JumptoWD.Wnd->UserPort)))
				{
					CopyMem((char *)m, (char *)&Msg, (long)sizeof(struct IntuiMessage));
	
					GT_ReplyIMsg(m);
	
					KeySelect(JumptoWD.Gadgets, &Msg);
	
					gad = (struct Gadget *) Msg.IAddress;
	
					switch( Msg.Class )
					{
						case	IDCMP_REFRESHWINDOW:
							GT_BeginRefresh( JumptoWD.Wnd );
							GT_EndRefresh( JumptoWD.Wnd, TRUE );
							break;
	
						case	IDCMP_VANILLAKEY:
							if( Msg.Code == 13)
							{
								EndeFlag = TRUE;
								wie = TRUE;
							}
							break;
	
						case	IDCMP_CLOSEWINDOW:
							EndeFlag = TRUE;
							break;
	
						case IDCMP_MOUSEMOVE:
							((struct MyNewGadget *)gad->UserData)->CurrentValue = Msg.Code;
							break;
	
						case	IDCMP_GADGETUP:
							switch(gad->GadgetID)
							{
								case GD_JUMPTO_NUMBER:
								case GD_JUMPTO_OK:
									if( !IsHexString( GetString(JumptoWD.Gadgets[GD_JUMPTO_NUMBER])))
									{
										MyRequest( MSG_INFO_GLOBAL_ILLEGALCHARACTERS, ( IPTR )GetString(JumptoWD.Gadgets[GD_JUMPTO_NUMBER]));
										ActivateGadget(JumptoWD.Gadgets[GD_JUMPTO_NUMBER],JumptoWD.Wnd,0);
									}
									else
									{	
										wie = TRUE;
										EndeFlag = TRUE;
									}
									break;
								case GD_JUMPTO_CANCEL:
									EndeFlag = TRUE;
									break;
							}
							break;
					}
				}
		}

		if( wie )
			if( IsHexString( GetString(JumptoWD.Gadgets[GD_JUMPTO_NUMBER])))
				stch_l( GetString( JumptoWD.Gadgets[ GD_JUMPTO_NUMBER ]), &wert);

		NewCloseAWindow( &JumptoWD );

		if( wie )
		{
			if( AktuDD->FD->Typ == FD_GRAB )
			{
				if( wert < ( long )AktuDD->FD->Mem )
					wert = ( long )AktuDD->FD->Mem;
				if( wert >= ( long )AktuDD->FD->Mem + AktuDD->FD->Len )
					wert = ( long )AktuDD->FD->Mem + AktuDD->FD->Len - 1;
				
				SetCursor( wert - ( long )AktuDD->FD->Mem, AktuDD );
			}
			else
			{
				if( wert >= AktuDD->FD->Len ) wert = AktuDD->FD->Len - 1;
				SetCursor( wert, AktuDD);
			}
		}
	}
}

void MarkLocation( ULONG Number, struct DisplayData *DD )
{
	if( !Number )
	{
		if( !GetWert( &DD->FD->LocationNumber, MSG_WINDOWTITLE_LOCATIONNUMBER, 1, 10 ))
			return;
	}
	else
		DD->FD->LocationNumber = Number;

	DD->FD->Locations[ DD->FD->LocationNumber - 1 ] = DD->CPos;
}

void JumpToLocation( LONG Number, struct DisplayData *DD )
{
	if( !Number )
	{
		if( !GetWert( &DD->FD->LocationNumber, MSG_WINDOWTITLE_LOCATIONNUMBER, 1, 10 ))
			return;
	}
	else
		DD->FD->LocationNumber = Number;

	SetCursor( DD->FD->Locations[ DD->FD->LocationNumber - 1 ], DD);
}


/*
 * BOOL MyRequestFile(char *FullName, char *Title, char *Pattern, BOOL Buffered )
 * 
 * Öffnet einen Filerequester(Asl/Reqtools).
 *
 * ACHTUNG!: fullname wird ggf. verändert
 */

BOOL MyRequestFile( char *FullName, CONST_STRPTR Title, CONST_STRPTR Pattern, BOOL Buffered)
{
	BOOL Success = FALSE;
	char FileName[256], PathName[256];

	strcpy( FileName, FilePart( FullName ));
	strcpy( PathName, FullName);
	*PathPart( PathName ) = 0;

	if( UseAsl )
	{
		static char BufferedPattern[ 256 ];

		if( Buffered & ( !Pattern ))
			Pattern = BufferedPattern;

		if( AslRequestTags( Asl_FileFileReq, ASLFR_Window, AktuDI->Wnd, ASLFR_InitialFile, FileName, ASLFR_InitialDrawer, PathName, ASLFR_Window, AktuDI->Wnd, ASLFR_TitleText, Title, ASLFR_InitialPattern, Pattern ? Pattern : (CONST_STRPTR)"", TAG_DONE ))
		{
			strcpy( FullName, Asl_FileFileReq->rf_Dir );
			AddPart( FullName, Asl_FileFileReq->rf_File, 256 );

			Success = TRUE;
		}

		if( Buffered )
			strcpy( BufferedPattern, Asl_FileFileReq->fr_Pattern );
	}
	else
	{	
		struct rtFileRequester *FileReq;

		if( Buffered )
		{
			FileReq = FileFileReq;

			if( 0 != strcmp( PathName, FileReq->Dir ))
				rtChangeReqAttr( FileReq, RTFI_Dir, PathName, TAG_DONE );
		}
		else
			FileReq = rtAllocRequestA( RT_FILEREQ, NULL );

		if( FileReq )
		{
				/* Pathnamen und ggf. Pattern setzen */

			rtChangeReqAttr( FileReq, RTFI_Dir, PathName, Pattern ? RTFI_MatchPat : TAG_IGNORE, Pattern, TAG_DONE );

			if( rtFileRequest( FileReq, FileName, Title, RTFI_Flags, FREQF_PATGAD, AktuDI->Wnd ? RT_Window : TAG_IGNORE, AktuDI->Wnd, TAG_END ))
			{
				strcpy( FullName, FileReq->Dir );
				AddPart( FullName, FileName, 256 );

				Success = TRUE;
			}
	
			if( !Buffered )
				rtFreeRequest( FileReq );
		}
	}

	return( Success );
}

void ExecuteARexxCommand(CONST_STRPTR Name)
{
	BPTR fh;

	if(Name)
	{
		if((fh=Open(arexxcommandwindow,MODE_NEWFILE)))
			SendRexxCommand(MyRexxHost,Name,fh);
	}
	else
	{
		UBYTE fullname[256] = "Rexx:";

		if( MyRequestFile( fullname, GetStr( MSG_WINDOWTITLE_SELECTCOMMAND ), "#?.filex", FALSE ))
		{
			if((fh=Open(arexxcommandwindow,MODE_NEWFILE)))
				SendRexxCommand(MyRexxHost,fullname,fh);
		}
	}
}

void ExecuteARexxCommandNumber(LONG Number)
{
	if((Number>=0)&&(Number<=9))
	{
		if(arexxcommandused[Number] == TRUE)
		{
			ExecuteARexxCommand(arexxcommands[Number]);
		}
	}
}

BOOL StringToMem(char *string,UBYTE *mem,long laenge)
{
	BOOL Success=TRUE;
	UBYTE *ptr=mem;
	UBYTE wert;

	while((ptr<mem+laenge) && Success)
	{
		while(*string && ((*string==' ') || (*string=='\t')))string++;

		if(*string && isxdigit(*string) && isxdigit(*(string+1)))
		{
			if(isdigit(*string))
				wert=(*string-'0') * 0x10;
			else
				wert=(tolower(*string)-'a'+10)*0x10;

			string++;

			if(*string)
			{
				if(isdigit(*string))
					wert+=*string-'0';
				else
					wert+=tolower(*string)-'a'+10;

				string++;
				*ptr++=wert;
			}
			else
				Success=FALSE;
		}
		else
			Success=FALSE;
	}

	return(Success);
}



/*********************************************************************
 *		Clipboardsupportroutinen													*
 *																							*
 *																							*
 *********************************************************************/

#include <libraries/iffparse.h>
#include <clib/iffparse_protos.h>

#define ID_FTXT	MAKE_ID('F','T','X','T')	/* Formatted text. */
#define ID_CHRS	MAKE_ID('C','H','R','S')	/* Character data. */

static LONG clipboardunit;

	/* SaveClip(STRPTR Buffer,LONG Size):
	 *
	 *	Send a given text buffer to the clipboard.
	 */

BOOL SaveClip(STRPTR Buffer,LONG Size)
{
	BYTE Success = FALSE;

	if(Size > 0)
	{
		struct IFFHandle *Handle;

		if((Handle = AllocIFF()))
		{
			if((Handle -> iff_Stream = (IPTR)OpenClipboard(clipboardunit)))
			{
				InitIFFasClip(Handle);

				if(!OpenIFF(Handle,IFFF_WRITE))
				{
					if(!PushChunk(Handle,ID_FTXT,ID_FORM,IFFSIZE_UNKNOWN))
					{
						if(!PushChunk(Handle,0,ID_CHRS,IFFSIZE_UNKNOWN))
						{
							if(WriteChunkBytes(Handle,Buffer,Size) == Size)
							{
								if(!PopChunk(Handle))
									Success = TRUE;
							}
						}
					}

					if(Success)
					{
						if(PopChunk(Handle))
							Success = FALSE;
					}

					CloseIFF(Handle);
				}

				CloseClipboard((struct ClipboardHandle *)Handle -> iff_Stream);
			}

			FreeIFF(Handle);
		}
	}

	return(Success);
}

UBYTE *GetClip( ULONG *laenge )
{
	struct IFFHandle	*Handle;
	UBYTE *mem = 0;

	*laenge = 0;

	if((Handle = AllocIFF()))
	{
		if((Handle -> iff_Stream = (IPTR)OpenClipboard(clipboardunit)))
		{
			InitIFFasClip(Handle);

			if(!OpenIFF(Handle,IFFF_READ))
			{
				if(!StopChunk(Handle,ID_FTXT,ID_CHRS))
				{
					if(!ParseIFF(Handle,IFFPARSE_SCAN))
					{
						struct ContextNode *ContextNode = CurrentChunk(Handle);

						if(ContextNode -> cn_Type == ID_FTXT)
						{
							*laenge = ContextNode -> cn_Size;

							if(*laenge)
							if((mem = AllocMem(*laenge,MEMF_ANY)))
							{
								ReadChunkRecords(Handle,mem,*laenge,1);
							}
						}
					}
				}

				CloseIFF(Handle);
			}

			CloseClipboard((struct ClipboardHandle *)Handle -> iff_Stream);
		}

		FreeIFF(Handle);
	}

	return( mem );
}

BOOL SetClipboardUnit(LONG new)
{
	if((new>=0)&&(new<=256)){clipboardunit=new;return(TRUE);}
	return(FALSE);
}

LONG GetClipboardUnit( void )
{
	return( clipboardunit );
}
