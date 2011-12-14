#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <libraries/iffparse.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <proto/graphics.h>

#include <string.h>
#endif

#include "filexstructs.h"
#include "allprotos.h"
#include "filexstrings.h"

#define ID_FLEX	MAKE_ID('F','L','E','X')	/* FileX - Chunk */
#define ID_PREF	MAKE_ID('P','R','E','F')	/* PreferencesID */

UBYTE SettingsFullName[256];
UBYTE ARexxCommandSettingsFullName[256];

/*
 * BOOL LoadSettings( UBYTE *Name, BOOL quiet )
 * 
 * Läd Settings ein und setzt diese.
 * Falls quiet = TRUE, kein Requester bei Fehlern
 */

BOOL LoadSettings( UBYTE *Name, BOOL quiet )
{
	struct IFFHandle *Handle;
	BOOL Success = FALSE;

	if((Handle = AllocIFF()))
	{
	if((Handle -> iff_Stream = (IPTR)Open(Name,MODE_OLDFILE)))
	{
	InitIFFasDOS(Handle);

	if(!OpenIFF(Handle,IFFF_READ))
	{
	if(!StopChunk(Handle,ID_PREF,ID_FLEX))
	if(!ParseIFF(Handle,IFFPARSE_SCAN))
	{
		union { UBYTE *bptr; UWORD *wptr; ULONG *lptr; } ptr;

		UWORD version;
		LONG SettingsBufferLen;
		APTR SettingsBuffer;

			/* Versionsnummer des Files einlesen und überprüfen */
		
		if( ReadChunkBytes( Handle, &version, 2 ))
		{
		if( version == 3 )
		{
		if( ReadChunkBytes( Handle, &SettingsBufferLen, 4 ))
		{
				/* Speicher für den Buffer allokieren und die Daten einlesen */

			if(( ptr.bptr = SettingsBuffer = AllocMem( SettingsBufferLen, MEMF_ANY )))
			{
				ULONG new_screenflags;
				UWORD new_fhoehe;
				UWORD FontNameLen, commandshellwindowlen,	commandwindowlen;
				WORD new_displaytyp;

				if( ReadChunkBytes( Handle, SettingsBuffer, SettingsBufferLen ))
				{
					DefaultBPR						= *ptr.wptr++;
					DefaultZeilen					= *ptr.lptr++;
					DefaultLeft			= *ptr.wptr++;
					DefaultTop				= *ptr.wptr++;

					SetClipboardUnit( *ptr.wptr++ );
					SetRequester( *ptr.wptr++ );

					if( *ptr.wptr++ )
						mainflags |= MF_OVERWRITE;
					else
						mainflags &= ~MF_OVERWRITE;

					altsprungweite				= *ptr.wptr++;
					SetScrollRand( *ptr.wptr++ );
					SetMaxUndoMemSize( *ptr.wptr++ );
					SetUndoLevel( *ptr.wptr++ );
					SetTaskPri( FindTask( 0 ), taskpri = *ptr.wptr++);
					DefaultDisplayForm					= *ptr.wptr++;
					DefaultDisplaySpaces				= *ptr.wptr++;

					new_screenflags			= *ptr.wptr++;
					new_fhoehe					= *ptr.wptr++;

					commandwindowlen			= *ptr.wptr++;
					commandshellwindowlen	= *ptr.wptr++;
					FontNameLen					= *ptr.wptr++;
					new_displaytyp				= *ptr.wptr++;

					switch( new_screenflags )
					{
						case SF_PUBLIC:
							if( *ptr.wptr )
							{
								SetSollPublicScreenName( ptr.bptr + 2 );

								ptr.bptr += *ptr.wptr + 2;
							}
						 	break;

						case SF_OWN_PUBLIC:
							{
								ULONG k, Color;

								SetScreenModeData( TRUE,	*ptr.lptr,
																	*(ptr.lptr + 1),
																	*(ptr.lptr + 2),
																	*(ptr.wptr + 6),
																	*(ptr.wptr + 7),
																	*(ptr.wptr + 8));

								ptr.bptr += 18;

								for( k = 0; k < 8; k++ )
								{
									Color = *ptr.lptr++;

									ScreenColors[k].ColorIndex = k;
									ScreenColors[k].Red   = ( Color >> 8 ) & 0xf;
									ScreenColors[k].Green = ( Color >> 4 ) & 0xf;
									ScreenColors[k].Blue  = ( Color      ) & 0xf;
								}
							}

							break;
					}

					if(commandwindowlen)
					{
						SetCommandWindow( ptr.bptr );
						ptr.bptr += commandwindowlen;
					}

					if(commandshellwindowlen)
					{
						SetCommandShellWindow( ptr.bptr );
						ptr.bptr += commandshellwindowlen;
					}

					if( FontNameLen )
					{
						MySetFontNameSize( ptr.bptr, new_fhoehe, 0 );
						ptr.bptr += FontNameLen;
					}

					if( new_displaytyp == 3 )
					{
						memcpy( displaytab[ 3 ], ptr.bptr, 256 );
						*ptr.bptr += 256;
					}

					if( Scr ) ReopenDisplay( new_screenflags );
					else
					{
						ScreenFlags = new_screenflags;
					}
					Success = TRUE;
				}

				FreeMem( SettingsBuffer, SettingsBufferLen );
			}
		}
		}
		else
		{
			MyRequest( MSG_INFO_GLOBAL_UNKNOWNVERSIONCANTLOADSETTINGS, ( IPTR )Name );
			quiet = TRUE;
		}
		}
	}
	CloseIFF(Handle);
	}
	Close((BPTR)Handle -> iff_Stream);
	}
	FreeIFF(Handle);
	}

	if(( !quiet ) && ( !Success ))
		MyRequest( MSG_INFO_GLOBAL_CANTLOADSETTINGS, ( IPTR )Name );

	if(Success)
		strcpy( SettingsFullName, Name );

	return(Success);
}

BOOL LoadSettingsFR(void)
{
	if( MyRequestFile( SettingsFullName, GetStr( MSG_WINDOWTITLE_LOADSETTINGS ), "#?.prefs", FALSE ))
		return( LoadSettings( SettingsFullName,FALSE ));
	else
		return( FALSE );
}

void LoadSettingsDefault(void)
{
	if(!LoadSettings("FileX.prefs",TRUE))
		LoadSettings("ENVARC:FileX/FileX.prefs",TRUE);
}


BOOL SaveSettings( UBYTE *Name, BOOL quiet )
{
	struct IFFHandle *Handle;
	BOOL Success = FALSE;

	if((Handle = AllocIFF()))
	{
	if((Handle -> iff_Stream = (IPTR)Open(Name,MODE_NEWFILE)))
	{
	InitIFFasDOS(Handle);

	if(!OpenIFF(Handle,IFFF_WRITE))
	{
	if(!PushChunk(Handle,ID_PREF,ID_FORM,IFFSIZE_UNKNOWN))
	if(!PushChunk(Handle,0,ID_FLEX,IFFSIZE_UNKNOWN))
	{
		UWORD version = 3;
		
		if( WriteChunkBytes( Handle, &version, 2 ))
		{
			union { UBYTE *bptr; UWORD *wptr; ULONG *lptr; } ptr;

			APTR SettingsBuffer;
			LONG SettingsBufferLen;

			UWORD FontNameLen, commandshellwindowlen, commandwindowlen;


				/* Längen der variabellangen Daten bestimmen */

			commandwindowlen = strlen( arexxcommandwindow ) + 1;
			commandshellwindowlen = strlen( arexxcommandshellwindow ) + 1;
			FontNameLen = strlen( AktuDI->TextFont->tf_Message.mn_Node.ln_Name ) + 1;


				/* Gesamtlänge des Zwischenspeichers berechenen */

			SettingsBufferLen = 42 + FontNameLen + commandwindowlen + commandshellwindowlen;
			if( AktuDD->DisplayTyp == 3 ) SettingsBufferLen += 256;

			switch( ScreenFlags )
			{
				case SF_PUBLIC:
					SettingsBufferLen += 2 + strlen( SollPublicScreenName ) + 1;
				 	break;

				case SF_OWN_PUBLIC:
					SettingsBufferLen += 18 + 8 * 4;
				 	break;
			}

				/* Zwischenspeicher in einem Rutsch speichern */
			
			if( WriteChunkBytes( Handle, &SettingsBufferLen, 4 ))
			if(( ptr.bptr = SettingsBuffer = AllocMem( SettingsBufferLen, MEMF_ANY )))
			{
				DefaultBPR = *ptr.wptr++ = AktuDD->BPR;
				DefaultZeilen = *ptr.lptr++ = AktuDD->Zeilen;
				DefaultLeft = *ptr.wptr++ = AktuDI->Wnd->LeftEdge;
				DefaultTop = *ptr.wptr++ = AktuDI->Wnd->TopEdge;

				*ptr.wptr++ = GetClipboardUnit();
				*ptr.wptr++ = UseAsl;
				*ptr.wptr++ = (mainflags & MF_OVERWRITE) ? TRUE : FALSE;
				*ptr.wptr++ = altsprungweite;
				*ptr.wptr++ = realscrollrand;
				*ptr.wptr++ = maxundomemsize;
				*ptr.wptr++ = UndoLevel;
				*ptr.wptr++ = taskpri;
				DefaultDisplayForm = *ptr.wptr++ = AktuDD->DisplayForm;
				DefaultDisplaySpaces = *ptr.wptr++ = AktuDD->DisplaySpaces;

				*ptr.wptr++ = ScreenFlags;
				
				DefaultFontSize = *ptr.wptr++ = AktuDI->fhoehe;
				strcpy( DefaultFontName, AktuDI->TextFont->tf_Message.mn_Node.ln_Name );

				*ptr.wptr++ = commandwindowlen;
				*ptr.wptr++ = commandshellwindowlen;
				*ptr.wptr++ = FontNameLen;
				DefaultDisplayTyp = *ptr.wptr++ = AktuDD->DisplayTyp;

				switch( ScreenFlags )
				{
					case SF_PUBLIC:
						{
							WORD PSNLen = strlen( SollPublicScreenName ) + 1;

							*ptr.wptr++ = PSNLen;

							if( PSNLen )
							{
								memcpy( ptr.bptr, SollPublicScreenName, PSNLen );
								ptr.bptr += PSNLen;
							}
						}
					 	break;

					case SF_OWN_PUBLIC:
						*ptr.lptr++ = ScreenModeData.smd_DisplayID;
						*ptr.lptr++ = ScreenModeData.smd_DisplayWidth;
						*ptr.lptr++ = ScreenModeData.smd_DisplayHeight;
						*ptr.wptr++ = ScreenModeData.smd_DisplayDepth;
						*ptr.wptr++ = ScreenModeData.smd_OverscanType;
						*ptr.wptr++ = ScreenModeData.smd_AutoScroll;

						{
							ULONG color, k;

							for( k = 0; k < 8; k++ )
							{
								color = GetRGB4( Scr->ViewPort.ColorMap, k );

								*ptr.lptr++ = color;
							}
						}

					 	break;
				}

				if( commandwindowlen )
				{
					memcpy( ptr.bptr, arexxcommandwindow, commandwindowlen );
					ptr.bptr += commandwindowlen;
				}

				if( commandshellwindowlen )
				{
					memcpy( ptr.bptr, arexxcommandshellwindow, commandshellwindowlen );
					ptr.bptr += commandshellwindowlen;
				}

				if( FontNameLen )
				{
					memcpy( ptr.bptr, AktuDI->TextFont->tf_Message.mn_Node.ln_Name, FontNameLen );
					ptr.bptr += FontNameLen;
				}

				if( AktuDD->DisplayTyp == 3 )
				{
					memcpy( ptr.bptr , displaytab[ 3 ], 256 );
/*					ptr.bptr += 256;*/
				}

				if( WriteChunkBytes( Handle, SettingsBuffer, SettingsBufferLen ))
				{
					Success = TRUE;
				}

				FreeMem( SettingsBuffer, SettingsBufferLen );
			}
		}
	}
	CloseIFF(Handle);
	}
	Close((BPTR)Handle -> iff_Stream);
	}
	FreeIFF(Handle);
	}

	if((!quiet)&&(!Success))
		MyRequest( MSG_INFO_GLOBAL_CANTSAVESETTINGS, ( IPTR )Name );

	return(Success);
}

BOOL SaveSettingsFR(void)
{
	if( MyRequestFile( SettingsFullName, GetStr( MSG_WINDOWTITLE_SAVEFILEAS ), "#?.prefs", FALSE ))
		return( SaveSettings( SettingsFullName, FALSE ));
	else
		return( FALSE );
}

void SaveSettingsDefault(void)
{
	if( *SettingsFullName )
		SaveSettings(SettingsFullName,FALSE);
	else
	{
		if(!SaveSettings("FileX.prefs",TRUE))
		{
			BPTR lock;

			if((lock=Lock("ENVARC:FileX",ACCESS_READ)))
			{
				UnLock(lock);
			}
			else
			{
				if((lock=CreateDir("ENVARC:FileX")))
					UnLock(lock);
			}

			SaveSettings("ENVARC:FileX/FileX.prefs",TRUE);
		}
	}
}

/********************* ARexxCommandSettings *********************/

BOOL LoadARexxCommands(UBYTE *Name)
{
	BOOL Success = FALSE;
	BPTR fh;
	long laenge,k;
	UBYTE *mem,*ptr,*start;

	if((fh = Open(Name,MODE_OLDFILE)))
	{
		if((laenge=GetFileLaenge(fh)))
		{
			if((mem=ptr=AllocVec(laenge,MEMF_ANY)))
			{
				Read(fh,mem,laenge);

				for( k = 0; ( k < 10 ) && ( ptr < mem + laenge ); k++ )
				{
					start = ptr;
					while(( *ptr != 0xa ) && ( ptr < mem + laenge )) ptr++;

					if( ptr < mem + laenge )
					{
						*ptr++ = 0;

						if( strlen( start ))
							SetARexxMenu( k, start );
						else
							SetEmptyARexxMenu( k );
					}
				}

				for(;k<10;k++)
					SetEmptyARexxMenu( k );

				if( Scr ) SetNewMenus();

				Success = TRUE;
				
				FreeVec( mem );
			}
		}

		Close( fh );
	}

	if( Success )
		strcpy( ARexxCommandSettingsFullName, Name );

	return( Success );
}

BOOL LoadARexxCommandsFR(void)
{
	if( MyRequestFile( ARexxCommandSettingsFullName, GetStr( MSG_WINDOWTITLE_LOADCOMMANDS ), "#?.prefs", FALSE ))
		return( LoadARexxCommands( ARexxCommandSettingsFullName ));
	else
		return( FALSE );	
}

void LoadARexxCommandsDefault(void)
{
	if(!LoadARexxCommands("FileXARexxCommands.prefs"))
		LoadARexxCommands("ENVARC:FileX/FileXARexxCommands.prefs");
}

BOOL SaveARexxCommands(UBYTE *Name)
{
	BOOL Success = FALSE;
	BPTR fh;
	long k;
	BYTE lineend=0xa;

	if((fh=Open(Name,MODE_NEWFILE)))
	{
		for(k=0;k<10;k++)
		{
			if(arexxcommandused[k] == TRUE)
				Write(fh, arexxcommands[k], strlen(arexxcommands[k]));

			Write(fh,&lineend,1);
		}

		Success=TRUE;

		Close(fh);
	}

	return(Success);
}

BOOL SaveARexxCommandsFR(void)
{
	if( MyRequestFile( ARexxCommandSettingsFullName, GetStr( MSG_WINDOWTITLE_SAVECOMMANDS ), "#?.prefs", FALSE ))
		return( SaveARexxCommands( ARexxCommandSettingsFullName ));
	else
		return( FALSE );	
}

void SaveARexxCommandsDefault(void)
{
	if( *ARexxCommandSettingsFullName )
		SaveARexxCommands( ARexxCommandSettingsFullName );
	else
	{
		if(!SaveARexxCommands("FileXARexxCommands.prefs"))
		{
			BPTR lock;

			if((lock=Lock("ENVARC:FileX",ACCESS_READ)))
			{
				UnLock(lock);
			}
			else
			{
				if((lock=CreateDir("ENVARC:FileX")))
					UnLock(lock);
			}

			SaveARexxCommands("ENVARC:FileX/FileXARexxCommands.prefs");
		}
	}
}
