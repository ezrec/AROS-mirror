/*
 * Source generated with ARexxBox 1.11 (May  5 1993)
 * which is Copyright (c) 1992,1993 Michael Balzer
 *
 * and heavily modified by Klaas Hermanns
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <rexx/storage.h>
#include <rexx/rxslib.h>
#include <rexx/errors.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/rexxsyslib.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/rexxsyslib_pragmas.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "filexarexx.h"

#include "allprotos.h"
#include "filex.h"
#include "filexstructs.h"
#include "filexstrings.h"

/* ARexx Hilfsfunktionen */

BOOL ValidDD( struct DisplayData *DDD )
{
	struct DisplayData *DD;
	struct DisplayInhalt *DI;

	DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
		DD = (struct DisplayData *)DI->DisplayList.lh_Head;

		while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
		{
			if( DD == DDD )
				return( TRUE );

			DD = ( struct DisplayData *)DD->Node.ln_Succ;
		}

		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}

	return( FALSE );
}


BOOL ValidDI( struct DisplayInhalt *DDI )
{
	struct DisplayInhalt *DI;

	DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

	while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
	{
		if( DI == DDI )
			return( TRUE );

		DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
	}

	return( FALSE );
}

BOOL ValidFD( struct FileData *FFD )
{
	struct FileData *FD;

	FD = (struct FileData *)FileList.lh_Head;

	while( FD != ( struct FileData * )&FileList.lh_Tail )
	{
		if( FD == FFD )
			return( TRUE );

		FD = ( struct FileData *)FD->Node.ln_Succ;
	}

	return( FALSE );
}

/*************************/

#ifdef AREXX

void rx_clear( struct RexxHost *host, struct rxd_clear **rxd, long action )
{
	struct rxd_clear *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		if(( rd->arg.force != FALSE ) || ( QuitView( 3, DD )))
			ClearDDFile( DD, TRUE );
		else
			rd->rc=RC_WARN;
	}
}

void rx_open( struct RexxHost *host, struct rxd_open **rxd, long action )
{
	struct rxd_open *rd = *rxd;
	BOOL Success = FALSE;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		if(( rd->arg.force != FALSE ) || ( QuitView( 5, DD )))
		{
			if( rd->arg.filename )
				Success = MyOpen( rd->arg.filename, DD );
			else
				Success = OpenFile( DD );
		}
	
		if( !Success )
			rd->rc=RC_WARN;
	}
}

void rx_print( struct RexxHost *host, struct rxd_print **rxd, long action )
{
	struct FileData *FD;

	if(( *rxd )->arg.id )
	{
		if( ValidFD( *( *rxd )->arg.id ))
		{
			FD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		FD = AktuDD->FD;

	if( !(*rxd)->rc )
		PrintFile( FD );
}

void rx_quit( struct RexxHost *host, struct rxd_quit **rxd, long action )
{
	struct rxd_quit *rd = *rxd;

	if(( rd->arg.force != FALSE ) || ( QuitRequester( 0, 0 )))
	{
		mainflags |= MF_ENDE;
	}
	else
		rd->rc = RC_WARN;
}

void rx_save( struct RexxHost *host, struct rxd_save **rxd, long action )
{
	struct rxd_save *rd = *rxd;
	struct FileData *FD;

	if(( *rxd )->arg.id )
	{
		if( ValidFD( *( *rxd )->arg.id ))
		{
			FD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		FD = AktuDD->FD;

	if( !(*rxd)->rc )
	{
		if( SaveFile( FD ))
			rd->rc = 0;
		else
			rd->rc = RC_WARN;
	}
}

void rx_saveas( struct RexxHost *host, struct rxd_saveas **rxd, long action )
{
	struct rxd_saveas *rd = *rxd;
	BOOL Success;
	struct FileData *FD;

	if(( *rxd )->arg.id )
	{
		if( ValidFD( *( *rxd )->arg.id ))
		{
			FD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		FD = AktuDD->FD;

	if( !(*rxd)->rc )
	{
		if( rd->arg.name )
		{
			Success = SaveNewName( rd->arg.name, FD );
		}
		else
			Success = SaveAsFile( FD );
	
		if( Success )
			rd->rc = 0;
		else
			rd->rc = RC_WARN;
	}
}

void rx_nop( struct RexxHost *host, struct rxd_nop **rxd, long action )
{
	/* Nichts :-) */
}

void rx_requestnotify( struct RexxHost *host, struct rxd_requestnotify **rxd, long action )
{
	struct rxd_requestnotify *rd = *rxd;

	if( rd->arg.prompt )
		MyRequestNoLocale( rd->arg.prompt, 0 );
	else
		DisplayLocaleText( MSG_INFO_AREXX_NOTIFY );
}

void rx_requestresponse( struct RexxHost *host, struct rxd_requestresponse **rxd, long action )
{
	struct rxd_requestresponse *rd = *rxd;

	if( !rd->arg.buttons )
		rd->arg.buttons = GetStr( MSG_GADGET_YESNO );

	rd->rc = MyFullRequestNoLocale( rd->arg.prompt, rd->arg.buttons, 0 );
}

void rx_requeststring( struct RexxHost *host, struct rxd_requeststring **rxd, long action )
{
	struct rxd_requeststring *rd = *rxd;

	switch( action )
	{
		case RXIF_INIT:
			rd->res.string = AllocVec(256,MEMF_ANY);
			*rd->res.string = 0;
			break;
			
		case RXIF_ACTION:
			if( rd->arg.mydefault )
				strcpy(rd->res.string,rd->arg.mydefault);

			if( 0 == MyGetString( rd->res.string, rd->arg.prompt, 256 ))
				rd->rc=RC_WARN;
			else
				rd->rc=0;

			break;
		
		case RXIF_FREE:
			FreeVec(rd->res.string);
			break;
	}
}

void rx_column( struct RexxHost *host, struct rxd_column **rxd, long action )
{
	struct rxd_column *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( rd->arg.delta )
		MoveCursorX( *rd->arg.delta, DD );
}

void rx_cursor( struct RexxHost *host, struct rxd_cursor **rxd, long action )
{
	struct rxd_cursor *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		if( rd->arg.up != FALSE )
			MoveCursorY( -1, DD );
		else
			if( rd->arg.down != FALSE )
				MoveCursorY( 1, DD );
	
		if( rd->arg.left != FALSE )
			MoveCursorX( -1, DD );
		else
			if( rd->arg.right != FALSE )
				MoveCursorX( 1, DD );
	}
}

void rx_gotobookmark( struct RexxHost *host, struct rxd_gotobookmark **rxd, long action )
{
	struct rxd_gotobookmark *rd = *rxd;

	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		if(( *rd->arg.number > 0 ) && ( *rd->arg.number <= 10 ))
		{
			JumpToLocation( *rd->arg.number, DD );
			rd->rc = 0;
		}
		else
			rd->rc = RC_WARN;
	}
}

void rx_gotocolumn( struct RexxHost *host, struct rxd_gotocolumn **rxd, long action )
{
	struct rxd_gotocolumn *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
		SetCursorX( *rd->arg.column, DD );
}

void rx_gotoline( struct RexxHost *host, struct rxd_gotoline **rxd, long action )
{
	struct rxd_gotoline *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
		SetCursorY( *rd->arg.line, DD );
}

void rx_line( struct RexxHost *host, struct rxd_line **rxd, long action )
{
	struct rxd_line *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
		MoveCursorY( *rd->arg.delta, DD );
}

void rx_position( struct RexxHost *host, struct rxd_position **rxd, long action )
{
	struct rxd_position *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		if( rd->arg.sof != FALSE ) SetCursorStart( DD );
		else if( rd->arg.eof != FALSE ) SetCursorEnd( DD );
	
		if( rd->arg.sol != FALSE ) SetCursorStartOfLine( DD );
		else if( rd->arg.eol != FALSE ) SetCursorEndOfLine( DD );
	}
}

void rx_setbookmark( struct RexxHost *host, struct rxd_setbookmark **rxd, long action )
{
	struct rxd_setbookmark *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		rd->rc=RC_WARN;
	
		if( rd->arg.number )
			if(( *rd->arg.number > 0 ) && ( *rd->arg.number <= 10 ))
			{
				MarkLocation( *rd->arg.number, DD );
				rd->rc = 0;
			}
	}
}

void rx_find( struct RexxHost *host, struct rxd_find **rxd, long action )
{
	struct rxd_find *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		searchmode &= ~SM_REPLACE;

		rd->rc = 0;
	
		if( rd->arg.text )
		{
			SetSearchString(rd->arg.text);
	
			if( rd->arg.hex != FALSE )
				searchmode&=~SM_STRING;
			else
				searchmode|=SM_STRING;
		}
	
		if( rd->arg.prompt != FALSE )
		{
			OpenSearchWindow( FALSE );
		}
		else
		{
			BOOL quiet = FALSE;
	
			if( rd->arg.quiet != FALSE )
				quiet = TRUE;
	
			if( rd->arg.backwards != FALSE )
			{
				if(!(SearchNext(BM_BACKWARD,quiet,FALSE, DD)))
					rd->rc=RC_WARN;
			}
			else
			{
				if(!SearchNext(0,quiet,FALSE, DD))
					rd->rc=RC_WARN;
			}
		}
	}
}

void rx_findchange( struct RexxHost *host, struct rxd_findchange **rxd, long action )
{
	struct rxd_findchange *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		searchmode|=SM_REPLACE;
	
		rd->rc = 0;
	
		if(rd->arg.find)SetSearchString(rd->arg.find);
		if(rd->arg.change)SetReplaceString(rd->arg.change);
	
		if(rd->arg.hex!=FALSE)
			searchmode&=~SM_STRING;
		else
			searchmode|=SM_STRING;
	
		if(rd->arg.prompt!=FALSE)
		{
			OpenSearchWindow( TRUE );
		}
		else
		{
			BOOL quiet=FALSE,all=FALSE;
	
			if(rd->arg.quiet!=FALSE)quiet=TRUE;
			if(rd->arg.all!=FALSE)all=TRUE;
	
			if(rd->arg.backwards!=FALSE)
			{
				if(!(SearchNext(BM_BACKWARD,quiet,all,DD)))
					rd->rc=RC_WARN;
			}
			else
			{
				if(!SearchNext(0,quiet,all,DD))
					rd->rc=RC_WARN;
			}
		}
	}
}

void rx_findnext( struct RexxHost *host, struct rxd_findnext **rxd, long action )
{
	struct rxd_findnext *rd = *rxd;
	BOOL quiet=FALSE;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
	
		if(rd->arg.quiet!=FALSE)quiet=TRUE;
	
		if(rd->arg.backwards!=FALSE)
		{
			if(!(SearchNext(BM_BACKWARD,quiet,FALSE,DD)))
				rd->rc=RC_WARN;
		}
		else
		{
			if(!SearchNext(0,quiet,FALSE,DD))
				rd->rc=RC_WARN;
		}
	}
}

void rx_activate( struct RexxHost *host, struct rxd_activate **rxd, long action )
{
	mainflags |= MF_ICONIFY;
}

void rx_deactivate( struct RexxHost *host, struct rxd_deactivate **rxd, long action )
{
	RemoveIconify();
}

/****************************Attributes****************************/

#include "FileXARexxattribute.h"

BOOL IsNumeric(STRPTR String)
{
	while(*String && (*String == ' ' || *String == '\t'))
		String++;

	if( *String == '-' ) String++;

	while(*String)
	{
		if(*String < '0' || *String > '9')
			return(FALSE);
		else
			String++;
	}

	return(TRUE);
}

LONG FindAttrObject(UBYTE *Name)
{
	BOOL gefunden=FALSE;
	LONG obnum=0;

	while((!gefunden) &&attributeobjects[obnum].name)
	{
		if(0==stricmp(attributeobjects[obnum].name,Name))
			gefunden=TRUE;
		else
			obnum++;
	}

	if(gefunden)return(obnum);
	else return(-1);
}

LONG FindFieldAttr(UBYTE *Name,LONG obnum)
{
	BOOL gefunden=FALSE;
	LONG num=0;	

	while((attributeobjects[obnum].attr[num].name)&&(!gefunden))
	{
		if(stricmp(attributeobjects[obnum].attr[num].name,Name)==0)
			gefunden=TRUE;
		else
			num++;
	}

	if(gefunden)return(num);
	else return(-1);
}

void SetSingleAttr(struct rxd_setattr *rd,LONG obnum,LONG attrnum,UBYTE *String, APTR *id)
{
	LONG Number;
	BOOL Boolean;

	switch(attributeobjects[obnum].attr[attrnum].typ)
	{
		case 1:
			if(IsNumeric(String))
			{
				Number=atol(String);
			}
			else
			{
				rd->rc2=(LONG)GetStr( MSG_INFO_AREXX_VARHASTOBEANUMBER );
				rd->rc=-RC_ERROR;
				return;
			}
			break;
		case 2:
			while(*String && ((*String==' ')||(*String=='\t')))
				String++;

			if(0 == stricmp(String,"FALSE"))
			{
				Boolean=FALSE;
			}
			else if(0 == stricmp(String,"TRUE"))
			{
				Boolean=TRUE;
			}
			else
			{
				rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_VARHASTOBETRUEORFALSE );
				rd->rc=-RC_ERROR;
				return;
			}
			break;
	}

	switch(obnum)
	{
		case APPLICATION_ATTRNUM:
			switch(attrnum)
			{
				case PRIORITY_APPATTR:
					if((Number>=-128)&&(Number<=127))SetTaskPri(FindTask(NULL), taskpri = Number);
					break;
				case SEARCHSTRING_APPATTR:
					SetSearchString(String);
					break;
				case REPLACESTRING_APPATTR:
					SetReplaceString(String);
					break;
				case STRINGSEARCH_APPATTR:
					if(Boolean)
						searchmode|=SM_STRING;
					else
						searchmode&=~SM_STRING;
					break;
				case CASESENSITIVSEARCH_APPATTR:
					if(Boolean)
						searchmode|=SM_CASE;
					else
						searchmode|=SM_CASE;
					break;
				case WILDSEARCH_APPATTR:
					if(Boolean)
						searchmode|=SM_WILDCARDS;
					else
						searchmode|=SM_WILDCARDS;
					break;
				case FILLSTRING_APPATTR:
					SetFillString( String );
					break;

				case STRINGFILL_APPATTR:
					if(Boolean)
						SetFillMode( FM_STRING );
					else
						SetFillMode( FM_NUMBER );
					break;

				case MAXUNDOMEMSIZE_APPATTR:
					SetMaxUndoMemSize(Number);
					break;
				case MAXUNDOLEVEL_APPATTR:
					SetUndoLevel(Number);
					break;
				case ALTJUMP_APPATTR:
					altsprungweite=Number;
					break;
				case SCROLLBORDER_APPATTR:
					SetScrollRand(Number);
					break;
				case CLIPBOARDUNIT_APPATTR:
					SetClipboardUnit(Number);
					break;
				case COMMANDSHELLWINDOW_APPATTR:
					SetCommandShellWindow(String);
					break;
				case COMMANDWINDOW_APPATTR:
					SetCommandWindow(String);
					break;
				case OVERWRITE_APPATTR:
					if(Boolean==TRUE)
						mainflags|=MF_OVERWRITE;
					else
						mainflags&=~MF_OVERWRITE;
					break;

				case USEASL_APPATTR:
					SetRequester( Boolean );
					break;

				default:
					rd->rc2=(LONG)GetStr( MSG_INFO_AREXX_FIELDISREADONLY );
					rd->rc=-RC_WARN;
			}
			break;

		case VIEW_ATTRNUM:
			{
				struct DisplayData *DD;
				struct DisplayInhalt *DI;

				if( id )
				{
					DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;
				
					while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
					{
						DD = (struct DisplayData *)DI->DisplayList.lh_Head;
				
						while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
						{
							if( DD == *id )
								break;

							DD = ( struct DisplayData *)DD->Node.ln_Succ;
						}

						if( DD == *id )
							break;
				
						DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
					}

					if( *id != DD )
					{
						return;
					}
				}
				else
					DD = AktuDD;

				switch(attrnum)
				{
					case DISPLAYTYP_VIEWATTR:
						SetDisplayTyp(Number, DD);
						break;

					case DISPLAY_VIEWATTR:
						if(( Number > 0 ) && (Number <= 3 ))
						{
							if( DD->DisplayForm != Number )
							{
								DD->DisplayForm = Number;
								MakeDisplay( DD->DI );
							}
						}
						break;
	
					case DISPLAYSPACES_VIEWATTR:
						if(( Number >= 0 ) && (Number <= 3 ))
						{
							if( Number == 3 ) Number = 8;
	
							if(DD->DisplaySpaces != Number)
							{
								DD->DisplaySpaces = Number;
								MakeDisplay( DD->DI );
							}
						}					
						break;
	
					default:
						rd->rc2=(LONG)GetStr( MSG_INFO_AREXX_FIELDISREADONLY );
						rd->rc=-RC_WARN;
						break;
				}
			}
			break;

		case WINDOW_ATTRNUM:
		case FILE_ATTRNUM:
		case WINDOWS_ATTRNUM:
		case FILES_ATTRNUM:
			rd->rc2=(LONG)GetStr( MSG_INFO_AREXX_FIELDISREADONLY );
			rd->rc=-RC_WARN;
			break;
	}
}

void rx_setattr( struct RexxHost *host, struct rxd_setattr **rxd, long action)
{
	struct rxd_setattr *rd = *rxd;
	STRPTR Result;
	UBYTE VarName[160];
	LONG obnum;	
	LONG attrnum=0;

		/* OBJECT suchen */

	if(-1==(obnum=FindAttrObject(rd->arg.object)))
	{
		rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_OBJECTNOTFOUND );
		rd->rc=-RC_WARN;
		return;
	}

		/* FIELD falls vorhanden suchen */

	if(rd->arg.field)
	{
		if(-1==(attrnum=FindFieldAttr(rd->arg.field,obnum)))
		{
			rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_FIELDNOTFOUND );
			rd->rc=-RC_WARN;
			return;
		}
	}

	rd->rc = 0;

	if(host->flags&ARB_HF_CMDSHELL)
	{
		if(rd->arg.fromstem)
		{
			rd->rc=-RC_WARN;
			rd->rc2=(LONG)GetStr( MSG_INFO_AREXX_FROMSTEMNOTPOSSIBLEINCOMMANDSHELL );
		}
		else
		{
			if(rd->arg.fromvar)
			{
				SetSingleAttr(rd,obnum,attrnum,rd->arg.fromvar, rd->arg.id);
			}
			else
			{
				rd->rc=-RC_WARN;
				if(rd->arg.field)rd->rc2=(LONG)GetStr( MSG_INFO_AREXX_FROMVARREQUIRED );
				else rd->rc2=(LONG)GetStr( MSG_INFO_AREXX_FROMVARANDFIELDREQUIRED );
			}
		}
	}
	else
	{
			/* Eine STEM Variable auslesen ? */

		if(rd->arg.fromstem)
		{
				/* FIELD vorhanden, dann nur einen Wert setzen */

			if(rd->arg.field)
			{
				sprintf(VarName,"%s.%s",rd->arg.fromstem,attributeobjects[obnum].attr[attrnum].name);

				if(!GetRexxVar((struct Message *)host->akturexxmsg, VarName, &Result))
					SetSingleAttr(rd,obnum,attrnum,Result, rd->arg.id);
				else
				{
					rd->rc2= (LONG) GetStr( MSG_INFO_AREXX_CANTGETVAR );
					rd->rc=-RC_ERROR;
				}
			}
			else
			{
				BOOL noerror=TRUE;

				/* STEMVAR auslesen und alle Felder setzen */

				while(noerror && attributeobjects[obnum].attr[attrnum].name)
				{
					sprintf(VarName,"%s.%s",rd->arg.fromstem,attributeobjects[obnum].attr[attrnum].name);

					if(!GetRexxVar((struct Message *)host->akturexxmsg, VarName, &Result))
					{
						if(Result && strlen(Result))
						{
							SetSingleAttr(rd,obnum,attrnum,Result, rd->arg.id);
							if(rd->rc<-RC_WARN)noerror=FALSE;
						}
					}
					else
					{
						noerror=FALSE;
						rd->rc2= (LONG) GetStr( MSG_INFO_AREXX_CANTGETVAR );
						rd->rc=-RC_ERROR;
					}

					attrnum++;
				}
			}
		}
		else
		if(rd->arg.fromvar)
		{
			if(rd->arg.field)
			{
				SetSingleAttr(rd,obnum,attrnum,rd->arg.fromvar, rd->arg.id);
			}
			else
			{
				rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_FROMSTEMORFIELDREQUIRED );
				rd->rc=-RC_WARN;
			}
		}
		else
		{
			rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_FROMVARORFROMSTEMREQUIRED );
			rd->rc=-RC_WARN;
		}
	}
}

UBYTE *GetSingleAttr(struct rxd_getattr *rd,LONG obnum,LONG attrnum, APTR *id)
{
	static UBYTE Result[256];
	UBYTE *String;
	LONG Number;
	SHORT Boolean=FALSE;

	String = Result;
	*Result = 0;

	switch(obnum)
	{
		case APPLICATION_ATTRNUM:
			switch(attrnum)
			{
				case SCREEN_APPATTR:
					String = GetOwnScreenName();
					break;
				case VERSION_APPATTR:
					String = VSTRING;
					break;
				case REGNAME_APPATTR:
					if( regnum ) String = regname;
					else String = "";
					break;
				case REGNUMBER_APPATTR:
					Number = regnum;
					break;

				case AREXX_APPATTR:
					String = MyRexxHost->portname;
					break;
				case PRIORITY_APPATTR:
					Number = taskpri;
					break;
				case SEARCHSTRING_APPATTR:
					String = GetAktuSearchString();
					break;
				case REPLACESTRING_APPATTR:
					String = GetAktuReplaceString();
					break;
				case STRINGSEARCH_APPATTR:
					Boolean = (searchmode&SM_STRING) ? TRUE : FALSE;
					break;
				case CASESENSITIVSEARCH_APPATTR:
					Boolean = (searchmode&SM_CASE) ? TRUE : FALSE;
					break;
				case WILDSEARCH_APPATTR:
					Boolean = (searchmode&SM_WILDCARDS) ? TRUE : FALSE;
					break;
				case FILLSTRING_APPATTR:
					String = GetFillString();
					break;
				case STRINGFILL_APPATTR:
					Boolean = (GetFillMode() & FM_STRING) ? TRUE : FALSE;
					break;
				case MAXUNDOMEMSIZE_APPATTR:
					Number = maxundomemsize;
					break;
				case MAXUNDOLEVEL_APPATTR:
					Number = UndoLevel;
					break;
				case ALTJUMP_APPATTR:
					Number = altsprungweite;
					break;
				case SCROLLBORDER_APPATTR:
					Number = realscrollrand;
					break;
				case CLIPBOARDUNIT_APPATTR:
					Number = GetClipboardUnit();
					break;
				case COMMANDSHELLWINDOW_APPATTR:
					String = arexxcommandshellwindow;
					break;
				case COMMANDWINDOW_APPATTR:
					String = arexxcommandwindow;
					break;
				case OVERWRITE_APPATTR:
					if(mainflags&MF_OVERWRITE)Boolean=TRUE;
					break;
				case USEASL_APPATTR:
					Boolean = UseAsl;
					break;
			}
			break;

		case WINDOW_ATTRNUM:
			{
				struct DisplayInhalt *DI;

				if( id )
				{
					DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

					while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
					{
						if( DI == *id )
							break;

						DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
					}

					if( DI != *id )
					{
						rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
						return( 0 );
					}
				}
				else
					DI = AktuDI;

				switch(attrnum)
				{
					case LEFT_WNDATTR:
						Number=DI->Wnd->LeftEdge;
						break;
					case TOP_WNDATTR:
						Number=DI->Wnd->TopEdge;
						break;
					case WIDTH_WNDATTR:
						Number=DI->Wnd->Width;
						break;
					case HEIGHT_WNDATTR:
						Number=DI->Wnd->Height;
						break;
					case TITLE_WNDATTR:
						String=DI->Wnd->Title;
						break;
					case MIN_WIDTH_WNDATTR:
						Number=DI->Wnd->MinWidth;
						break;
					case MIN_HEIGHT_WNDATTR:
						Number=DI->Wnd->MinHeight;
						break;
					case MAX_WIDTH_WNDATTR:
						Number=DI->Wnd->MaxWidth;
						break;
					case MAX_HEIGHT_WNDATTR:
						Number=DI->Wnd->MaxHeight;
						break;
					case VIEWCOUNT_WNDATTR:
						{
							struct DisplayData *DD;
							int Anzahl = 0;

							DD = (struct DisplayData *)DI->DisplayList.lh_Head;

							while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
							{
								Anzahl++;
								DD = ( struct DisplayData *)DD->Node.ln_Succ;
							}
							
							Number = Anzahl;
						}
						break;
					case VIEWS_WNDATTR:
						{
							struct DisplayData *DD;

							DD = (struct DisplayData *)DI->DisplayList.lh_Head;

							while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
							{
								char Buffer[13];
								
								sprintf(Buffer,"%ld ", DD );
								strcat( Result, Buffer );
								DD = ( struct DisplayData *)DD->Node.ln_Succ;
							}
						}
						break;
				}
				break;
			}

		case FILE_ATTRNUM:
			{
				struct FileData *FD;

				if( id )
				{
					FD = (struct FileData *)FileList.lh_Head;
				
					while( FD != ( struct FileData * )&FileList.lh_Tail )
					{
						if( FD == *id )
							break;
						FD = ( struct FileData *)FD->Node.ln_Succ;
					}
					
					if( FD != *id )
					{
						rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
						return( 0 );
					}
				}
				else
					FD = AktuDD->FD;

				switch( attrnum )
				{
					case FULLNAME_FILEATTR:
						String = FD->Name;
						break;
					case PATH_FILEATTR:
						strcpy( Result, FD->Name );
						*PathPart( Result ) = 0;
						String = Result;
						break;
					case NAME_FILEATTR:
						String = FilePart( FD->Name);
						break;
					case CHANGES_FILEATTR:
						Number = FD->FullChanges+FD->Changes+FD->RedoChanges;
						break;
					case FILELEN_FILEATTR:
						Number = FD->Len;
						break;
					case MEMPOS_FILEATTR:
						Number = ( ULONG )FD->Mem;
						break;
					case VIEWCOUNT_FILEATTR:
						{
							int Anzahl = 0;
							struct DisplayData *DD;

							DD = GetDDFromFD( FD->DisplayList.lh_Head );

							while( DD != GetDDFromFD( &FD->DisplayList.lh_Tail ))
							{
								Anzahl++;
								DD = GetDDFromFD( DD->FNode.ln_Succ );
							}
							
							Number = Anzahl;
						}
						break;
					case VIEWS_FILEATTR:
						{
							struct DisplayData *DD;

							DD = GetDDFromFD( FD->DisplayList.lh_Head );

							while( DD != GetDDFromFD( &FD->DisplayList.lh_Tail ))
							{
								char Buffer[13];
								
								sprintf(Buffer,"%ld ", DD );
								strcat( Result, Buffer );
								DD = GetDDFromFD( DD->FNode.ln_Succ );
							}
						}
						break;
				}
			}
			break;

		case VIEW_ATTRNUM:
			{
				struct DisplayData *DD;
				struct DisplayInhalt *DI;

				if( id )
				{
					DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;
				
					while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
					{
						DD = (struct DisplayData *)DI->DisplayList.lh_Head;
				
						while( DD != ( struct DisplayData * )&DI->DisplayList.lh_Tail )
						{
							if( DD == *id )
								break;

							DD = ( struct DisplayData *)DD->Node.ln_Succ;
						}

						if( DD == *id )
							break;
				
						DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
					}

					if( DD != *id )
					{
						rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
						return( 0 );
					}
				}
				else DD = AktuDD;

				switch( attrnum )
				{
					case LINES_VIEWATTR:
						Number = DD->Zeilen;
						break;

					case BYTESPERLINE_VIEWATTR:
						Number = DD->BPR;
						break;

					case CURSORPOS_VIEWATTR:
						Number = DD->CPos;
						break;
					case MARKPOS_VIEWATTR:
						Number = DD->MPos;
						break;
					case DISPLAYTYP_VIEWATTR:
						Number = DD->DisplayTyp;
						break;
					case EDITASCII_VIEWATTR:
						if(!(DD->Flags&DD_HEX))Boolean=TRUE;
						break;
					case MARK_VIEWATTR:
						if(DD->Flags&DD_MARK)Boolean=TRUE;
						break;
					case DISPLAY_VIEWATTR:
						Number = DD->DisplayForm;
						break;
					case DISPLAYSPACES_VIEWATTR:
						if( DD->DisplaySpaces == 8 ) Number = 3;
						else Number = DD->DisplaySpaces;
						break;
					case FILE_VIEWATTR:
						Number = (LONG)DD->FD;
						break;
					case WINDOW_VIEWATTR:
						Number = (LONG)DD->DI;
						break;
				}
			}
			break;
	}

	switch(attributeobjects[obnum].attr[attrnum].typ)
	{
		case 1:
			String=Result;
			sprintf(String,"%ld",Number);
			break;
		case 2:
			String=Result;
			strcpy(String,(Boolean==TRUE) ? "TRUE" : "FALSE");
			break;
	}

	return(String);		
}

LONG MySetRexxVar(struct RexxMsg *msg,char *name,char *value,ULONG len,BPTR fh)
{
	if( msg )
		return( SetRexxVar((struct Message *) msg, name, value, len ));
	else
	{
		if( name )
			FPrintf(fh, "%s = %s\n",name,value);
		else
			FPrintf(fh, "%s\n",value);

		return( 0 );
	}
}

void rx_getattr( struct RexxHost *host, struct rxd_getattr **rxd, long action )
{
	struct rxd_getattr *rd = *rxd;
	struct RexxMsg *msg;
	BPTR fh;

	LONG obnum;	
	LONG attrnum=0;
	UBYTE VarName[160];
	UBYTE *Value;
	LONG Result;


		/* OBJECT suchen */

	if(-1==(obnum=FindAttrObject(rd->arg.object)))
	{
		rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_OBJECTNOTFOUND );
		rd->rc=-RC_WARN;
		return;
	}

		/* FIELD, falls vorhanden, suchen */

	if(rd->arg.field)
	{
		if(-1==(attrnum=FindFieldAttr(rd->arg.field,obnum)))
		{
			rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_FIELDNOTFOUND );
			rd->rc=-RC_WARN;
			return;
		}
	}

	rd->rc = 0;

	if(host->flags&ARB_HF_CMDSHELL)
	{
		msg = 0;
		fh = host->outfh;
	}
	else
	{
		msg = host->akturexxmsg;
		fh = 0;
	}

	switch( obnum )
	{
		case WINDOWS_ATTRNUM:
		case FILES_ATTRNUM:
			{
				struct Node *node, *endnode, *startnode;
				int Anzahl = 0;
				char Buffer[ 13 ];
		
				switch( obnum )
				{
					case WINDOWS_ATTRNUM:
						endnode = ( struct Node * )&DisplayInhaltList.lh_Tail;
						startnode = DisplayInhaltList.lh_Head;
						break;
					case FILES_ATTRNUM:
						endnode = ( struct Node * )&FileList.lh_Tail;
						startnode = FileList.lh_Head;
						break;
				}

				if( rd->arg.stem )
				{
					node = startnode;
		
					while( node != endnode )
					{
						sprintf( VarName, "%s.%ld", rd->arg.stem, Anzahl );
						sprintf( Buffer, "%ld", node );
		
						if(Result = MySetRexxVar(msg,VarName,Buffer,strlen(Buffer),fh))
						{
							rd->rc = -RC_ERROR;
							rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_CANTSETVAR );
						}
		
						Anzahl++;
						node = node->ln_Succ;
					}
		
					sprintf( VarName, "%s.COUNT", rd->arg.stem, Anzahl );
					sprintf( Buffer, "%ld", Anzahl );
		
					if(Result = MySetRexxVar(msg,VarName,Buffer,strlen(Buffer),fh))
					{
						rd->rc = -RC_ERROR;
						rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_CANTSETVAR );
					}
				}
				else
				{
					LONG Gesamtlaenge=0;
		
					node = startnode;
		
					while( node != endnode )
					{
						sprintf( Buffer, "%ld",node );
						Gesamtlaenge += 1 + strlen( Buffer );
		
						Anzahl++;
						node = node->ln_Succ;
					}
		
					sprintf( Buffer, "%ld", Anzahl );
					Gesamtlaenge += 1 + strlen( Buffer );
		
					if(Gesamtlaenge)
					if(Value = AllocVec(Gesamtlaenge+1,MEMF_ANY|MEMF_CLEAR))
					{
						node = startnode;
		
						while( node != endnode )
						{
							sprintf( Buffer, "%ld ",node );
							strcat( Value, Buffer );
		
							node = node->ln_Succ;
						}
		
						sprintf( Buffer, "%ld", Anzahl );
						strcat( Value, Buffer );
		
						if(Value)
						{
							if(rd->arg.var || (host->flags&ARB_HF_CMDSHELL))
							{
								if(Result = MySetRexxVar(msg,rd->arg.var,Value,strlen(Value),fh))
								{
									rd->rc = -RC_ERROR;
									rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_CANTSETVAR );
								}
							}
							else						
							{
								host->userdata=1;
								ReplyRexxCommand(msg,0,0,Value);
							}
		
							FreeVec( Value );
						}
					}
				}
			}
			break;

		default:
			if(rd->arg.stem)
			{
				if(rd->arg.field)
				{
					sprintf(VarName,"%s.%s",rd->arg.stem,rd->arg.field);
		
					Value=GetSingleAttr(rd,obnum,attrnum,rd->arg.id);
		
					if(( !Value ) || (Result = MySetRexxVar(msg,VarName,Value,strlen(Value),fh)))
					{
						rd->rc = -RC_ERROR;

						if( Value )
							rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_CANTSETVAR );
					}
				}
				else
				{
					BOOL noerror=TRUE;
		
					while(noerror && attributeobjects[obnum].attr[attrnum].name)
					{
						sprintf(VarName,"%s.%s",rd->arg.stem,attributeobjects[obnum].attr[attrnum].name);
		
						Value=GetSingleAttr(rd,obnum,attrnum,rd->arg.id);
		
						if(( !Value ) || (Result = MySetRexxVar(msg,VarName,Value,strlen(Value),fh)))
						{
							noerror=FALSE;
							rd->rc = -RC_ERROR;
							if( Value )
								rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_CANTSETVAR );
						}
		
						attrnum++;
					}
				}
			}
			else
			{
				if(rd->arg.field)
				{
					Value=GetSingleAttr(rd,obnum,attrnum,rd->arg.id);
				}
				else
				{
					LONG Gesamtlaenge=0, len;
		
					while(attributeobjects[obnum].attr[attrnum].name)
					{
						char *str;

						str = GetSingleAttr(rd,obnum,attrnum,rd->arg.id);
						len = str ? strlen(str) : 0;

						if( len )
							Gesamtlaenge += 1 + len;

						attrnum++;
					}
		
					attrnum=0;
					Value = 0;
		
					if(Gesamtlaenge)
					if(Value = AllocVec(Gesamtlaenge+1,MEMF_ANY|MEMF_CLEAR))
					{
						while(attributeobjects[obnum].attr[attrnum].name)
						{
							char *str;

							str = GetSingleAttr(rd,obnum,attrnum,rd->arg.id);
							
							if( str )
							{
								strcat(Value,str);
								strcat(Value," ");
							}
							attrnum++;
						}
		
						Value[Gesamtlaenge-1]=0;
					}
				}
		
				if(Value)
				{
					if(rd->arg.var || (host->flags&ARB_HF_CMDSHELL))
					{
						if(Result = MySetRexxVar(msg,rd->arg.var,Value,strlen(Value),fh))
						{
							rd->rc = -RC_ERROR;
							rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_CANTSETVAR );
						}
					}
					else						
					{
						host->userdata=1;
						ReplyRexxCommand(msg,0,0,Value);
					}
				}
				else
				{
					rd->rc = -RC_ERROR;

					if( Value )
						rd->rc2 = (LONG) GetStr( MSG_INFO_AREXX_CANTSETVAR );
				}
			}
			break;
	}
}

void rx_fault( struct RexxHost *host, struct rxd_fault **rxd, long action )
{
	struct rxd_fault *rd = *rxd;

	switch( action )
	{
		case RXIF_ACTION:
			if( rd->res.description = AllocVec(256,MEMF_ANY) )
			{
				if( !Fault( *rd->arg.number, "",
					rd->res.description, 256 ) )
				{
					rd->rc = -10;
					rd->rc2 = (long) GetStr( MSG_INFO_AREXX_FAULTFAILED );
				}
			}
			else
			{
				rd->rc = 10;
				rd->rc2 = ERROR_NO_FREE_STORE;
			}
			break;
		
		case RXIF_FREE:
			if(rd->res.description) FreeVec(rd->res.description);
			break;
	}
}

void rx_help( struct RexxHost *host, struct rxd_help **rxd, long action )
{
	struct rxd_help *rd = *rxd;
	struct rxs_command *rxc;
	int cnt = 1;

	switch( action )
	{
		case RXIF_ACTION:
			if( rd->arg.command )
			{
				rxc = FindRXCommand( rd->arg.command );
				if( !rxc )
				{
					rd->rc = -10;
					rd->rc2 = (long) GetStr( MSG_INFO_AREXX_UNKNOWNCOMMAND );
				}
				else
				{
					if( rd->res.commanddesc = AllocVec( strlen(rxc->command)
						+ ((rxc->args) ? strlen(rxc->args):0)
						+ ((rxc->results) ? strlen(rxc->results):0) + 20, MEMF_ANY|MEMF_CLEAR))
					{
						sprintf( rd->res.commanddesc,
							"%s%lc%s%s%s%s%s",
							rxc->command, 
							(rxc->results || rxc->args) ? ' ' : '\0',
							rxc->results ? "VAR/K,STEM/K" : "",
							(rxc->results && rxc->args) ? "," : "",
							rxc->args ? rxc->args : "",
							rxc->results ? " => " : "",
							rxc->results ? rxc->results : "" );
					}
					else
					{
						rd->rc = 10;
						rd->rc2 = ERROR_NO_FREE_STORE;
					}
				}
			}
			else
			{
				char **s;
				
				rxc = rxs_commandlist;
				while( rxc->command )
				{
					cnt++;
					rxc++;
				}
				
				if(!(s = AllocVec(4 * cnt, MEMF_ANY|MEMF_CLEAR)))
				{
					rd->rc = 10;
					rd->rc2 = ERROR_NO_FREE_STORE;
				}
				else
				{
					rd->res.commandlist = s;
					
					rxc = rxs_commandlist;
					while(rxc->command)
					{
						*s = rxc->command;
						s++;
						rxc++;
					}
				}
			}
			
			break;
		
		case RXIF_FREE:
			if(rd->res.commanddesc)	FreeVec(rd->res.commanddesc);
			if(rd->res.commandlist)	FreeVec(rd->res.commandlist);
			break;
	}
}

void rx_rx( struct RexxHost *host, struct rxd_rx **rxd, long action )
{
	struct {
		struct rxd_rx rd;
		long rc;
	} *rd = (void *) *rxd;
	
	BPTR fh = NULL;
	struct RexxMsg *sentrm;

	switch( action )
	{
		case RXIF_INIT:
			*rxd = AllocVec(sizeof *rd, MEMF_ANY);
			break;
			
		case RXIF_ACTION:
			/* Kommando leer? */
			if( !rd->rd.arg.command )
				return;
			
			/* Mit Ein-/Ausgabe-Fenster? */
			if( rd->rd.arg.console )
			{
				fh = Open(arexxcommandwindow, MODE_NEWFILE );
				if( !fh )
				{
					rd->rd.rc = -20;
					rd->rd.rc2 = (long) GetStr( MSG_INFO_AREXX_RXCANTOPENWINDOW );
					return;
				}
			}
			
			/* Kommando abschicken */
			if( !(sentrm = SendRexxCommand(host, rd->rd.arg.command, fh)) )
			{
				rd->rd.rc = -20;
				rd->rd.rc2 = (long) GetStr( MSG_INFO_AREXX_RXCANTSENDAREXXCOMMAND );
				return;
			}
			
			/* auf den Reply warten */
			if( !rd->rd.arg.async )
			{
				struct RexxMsg *rm;
				BOOL waiting = TRUE;
				
				do
				{
					WaitPort( host->port );
					
					while( rm = (struct RexxMsg *) GetMsg(host->port) )
					{
						/* Reply? */
						if( rm->rm_Node.mn_Node.ln_Type == NT_REPLYMSG )
						{
							/* 'unsere' Msg? */
							if( rm == sentrm )
							{
								rd->rc = rm->rm_Result1;
								rd->rd.res.rc = &rd->rc;
								
								if( !rm->rm_Result1 && rm->rm_Result2 )
								{
									/* Res2 ist String */
									rd->rd.res.result =
										strdup( (char *) rm->rm_Result2 );
								}
								
								waiting = FALSE;
							}
							
							FreeRexxCommand( rm );
							--host->replies;
						}
						
						/* sonst Kommando -> Fehler */
						else if( ARG0(rm) )
						{
							ReplyRexxCommand( rm, -20, (long)
								"CommandShell Port", NULL );
						}
					}
				}
				while( waiting );
			}
			
			break;
		
		case RXIF_FREE:
			if( rd->rd.res.result ) FreeVec( rd->rd.res.result );
			free( rd );
			break;
	}
}

void rx_font( struct RexxHost *host, struct rxd_font **rxd, long action )
{
	struct rxd_font *rd = *rxd;

	if(rd->arg.name&&rd->arg.size)
	{
		if(( *rxd )->arg.id )
		{
			if( ValidDI( *( *rxd )->arg.id ))
			{
				MySetFontNameSize(rd->arg.name,*rd->arg.size,*( *rxd )->arg.id);
			}
			else
			{
				(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
				(*rxd)->rc = -RC_WARN;
			}
		}
		else
			MySetFontNameSize(rd->arg.name,*rd->arg.size,AktuDI);
	}
}

void rx_multiundofunction( BOOL Un, struct rxd_redo **rxd )
{
	struct FileData *FD;

	if(( *rxd )->arg.id )
	{
		if( ValidFD( *( *rxd )->arg.id ))
		{
			FD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		FD = AktuDD->FD;

	if( !(*rxd)->rc )
	{
		struct rxd_redo *rd = *rxd;
		long k, i = 1;
	
		if( rd->arg.number )
			i = *rd->arg.number;
	
		for( k = 0; k < i; k++ )
		{
			if( Un )
			{
				if( !Undo( FD ))
					break;
			}
			else
			{
				if( !Redo( FD ))
					break;
			}
		}
	}
}

void rx_redo( struct RexxHost *host, struct rxd_redo **rxd, long action )
{
	rx_multiundofunction( FALSE, rxd );
}

void rx_undo( struct RexxHost *host, struct rxd_undo **rxd, long action )
{
	rx_multiundofunction( TRUE, rxd );
}

void rx_activatewindow( struct RexxHost *host, struct rxd_activatewindow **rxd, long action )
{
	if(( *rxd )->arg.id )
	{
		if( ValidDI( *( *rxd )->arg.id ))
		{
			MyActivateWindow( *( *rxd )->arg.id );
			ActivateWindow( AktuDI->Wnd );
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		ActivateWindow( AktuDI->Wnd );
}

void rx_activateview( struct RexxHost *host, struct rxd_activateview **rxd, long action )
{
	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			struct DisplayData *DD;

			DD = (struct DisplayData *)*( *rxd )->arg.id;

			if( DD->DI != AktuDI )
			{
				MyActivateWindow( DD->DI );
				ActivateWindow( DD->DI->Wnd );
			}

			ChangeAktuView( DD );
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
}

void rx_adjustwindowsize( struct RexxHost *host, struct rxd_activatewindow **rxd, long action )
{
	if(( *rxd )->arg.id )
	{
		if( ValidDI( *( *rxd )->arg.id ))
		{
			AdjustWindowSize( *( *rxd )->arg.id );
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		AdjustWindowSize( AktuDI );
}

void rx_changewindow( struct RexxHost *host, struct rxd_changewindow **rxd, long action )
{
	struct Window *Wnd;

	if(( *rxd )->arg.id )
	{
		if( ValidDI( *( *rxd )->arg.id ))
		{
			Wnd = ((struct DisplayInhalt *)*( *rxd )->arg.id)->Wnd;
		}
		else
		{
			Wnd = NULL;
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		Wnd = AktuDI->Wnd;

	if( Wnd ) 
	{
		WORD Left, Top, Width, Height;

		if(( *rxd )->arg.leftedge )
			Left = *( *rxd )->arg.leftedge;
		else
			Left = Wnd->LeftEdge;

		if(( *rxd)->arg.topedge )
			Top = *( *rxd )->arg.topedge;
		else
			Top = Wnd->TopEdge;

		if(( *rxd )->arg.width )
			Width = *( *rxd )->arg.width;
		else
			Width = Wnd->Width;

		if(( *rxd )->arg.height )
			Height = *( *rxd )->arg.height;
		else
			Height = Wnd->Height;

		ChangeWindowBox(Wnd,Left,Top,Width,Height);
	}
}

void rx_movewindow( struct RexxHost *host, struct rxd_movewindow **rxd, long action )
{
	struct rxd_movewindow *rd = *rxd;
	struct Window *Wnd;

	if(( *rxd )->arg.id )
	{
		if( ValidDI( *( *rxd )->arg.id ))
		{
			Wnd = ((struct DisplayInhalt *)*( *rxd )->arg.id)->Wnd;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		Wnd = AktuDI->Wnd;

	if( !(*rxd)->rc )
	{
		long leftedge = -1, topedge = -1;

		if( rd->arg.leftedge )
			leftedge = *rd->arg.leftedge;

		if( rd->arg.topedge )
			topedge = *rd->arg.topedge;

		if(leftedge==-1)leftedge=Wnd->LeftEdge;
		if(topedge==-1)topedge=Wnd->TopEdge;

		ChangeWindowBox(Wnd,leftedge,topedge,Wnd->Width,Wnd->Height);
	}
}

void rx_sizewindow( struct RexxHost *host, struct rxd_sizewindow **rxd, long action )
{
	struct rxd_sizewindow *rd = *rxd;
	long height, width;
	struct Window *Wnd;

	if(( *rxd )->arg.id )
	{
		if( ValidDI( *( *rxd )->arg.id ))
		{
			Wnd = ((struct DisplayInhalt *)*( *rxd )->arg.id)->Wnd;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		Wnd = AktuDI->Wnd;


	if( !(*rxd)->rc )
	{
		height=*rd->arg.height;
		width=*rd->arg.width;

		if(height==-1)height=Wnd->Height;
		if(width==-1)width=Wnd->Width;

		ChangeWindowBox(Wnd,Wnd->LeftEdge,Wnd->TopEdge,width,height);
	}
}

void rx_multiwindowfunction( WORD Typ, struct rxd_windowtoback **rxd )
{
	struct Window *Wnd;

	if(( *rxd )->arg.id )
	{
		if( ValidDI( *( *rxd )->arg.id ))
		{
			Wnd = ((struct DisplayInhalt *)*( *rxd )->arg.id)->Wnd;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		Wnd = AktuDI->Wnd;

	if( !(*rxd)->rc )
	{
		switch( Typ )
		{
			case 0:
				ZipWindow(Wnd);
				break;
			case 1:
				WindowToFront(Wnd);
				break;
			case 2:
				WindowToBack(Wnd);
				break;
		}
	}
}
void rx_zipwindow( struct RexxHost *host, struct rxd_zipwindow **rxd, long action )
{
	rx_multiwindowfunction( 0, rxd );
}

void rx_windowtofront( struct RexxHost *host, struct rxd_windowtofront **rxd, long action )
{
	rx_multiwindowfunction( 1, rxd );
}

void rx_windowtoback( struct RexxHost *host, struct rxd_windowtoback **rxd, long action )
{
	rx_multiwindowfunction( 2, rxd );
}

void rx_requestfile( struct RexxHost *host, struct rxd_requestfile **rxd, long action )
{
	struct rxd_requestfile *rd = *rxd;

	switch( action )
	{
		case RXIF_INIT:
			rd->res.filename = AllocVec(256,MEMF_CLEAR);
			break;
			
		case RXIF_ACTION:
			if( rd->arg.path ) strcpy( rd->res.filename, rd->arg.path );
			if( rd->arg.file ) AddPart( rd->res.filename, rd->arg.file, 256 );

			if( MyRequestFile( rd->res.filename, rd->arg.title ? rd->arg.title : 0, rd->arg.pattern ? rd->arg.pattern : 0, FALSE ))
				rd->rc = 0;
			else
				rd->rc=RC_WARN;	
			break;
		
		case RXIF_FREE:
			FreeVec(rd->res.filename);
			break;
	}
}

void rx_grabmemory( struct RexxHost *host, struct rxd_grabmemory **rxd, long action )
{
	struct rxd_grabmemory *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		if((rd->arg.start)&&(rd->arg.end))
		{
			if(( rd->arg.force != FALSE ) || ( QuitView( 4, DD )))
				GrabMemory( *rd->arg.start, *rd->arg.end, DD );
			else
				rd->rc = RC_WARN;
		}
	}
}

void rx_printblock( struct RexxHost *host, struct rxd_printblock **rxd, long action )
{
	PrintBlock();
}

void rx_multiddfunction( WORD Typ, struct rxd_cut **rxd )
{
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;


	if( !(*rxd)->rc )
		switch( Typ )
		{
			case 0:
				if( !( DD->FD->Typ == FD_GRAB ))
					AppendBlock( DD );
				break;

			case 1:
				Copy( DD );
				break;

			case 2:
				Cut( DD );
				break;

			case 3:
				FillDD( DD );
				break;

			case 4:
				if( !( DD->FD->Typ == FD_GRAB ))
					InsertBlock( DD );
				break;

			case 5:
				Paste( DD );
				break;

			case 6:
				Mark( DD );
				break;

			case 7:
				MakeDisplayMax( DD );
				break;

			case 8:
				MakeDisplayBigger( DD );
				break;

			case 9:
				MakeDisplaySmaller( DD );
				break;
		}
}

void rx_setblockmark( struct RexxHost *host, struct rxd_setblockmark **rxd, long action )
{
	rx_multiddfunction( 6, rxd );
}

void rx_cut( struct RexxHost *host, struct rxd_cut **rxd, long action )
{
	rx_multiddfunction( 2, rxd );
}

void rx_copy( struct RexxHost *host, struct rxd_copy **rxd, long action )
{
	rx_multiddfunction( 1, rxd );
}

void rx_fill( struct RexxHost *host, struct rxd_fill **rxd, long action )
{
	rx_multiddfunction( 3, rxd );
}

void rx_paste( struct RexxHost *host, struct rxd_paste **rxd, long action )
{
	rx_multiddfunction( 5, rxd );
}

void rx_insert( struct RexxHost *host, struct rxd_insert **rxd, long action )
{
	rx_multiddfunction( 4, rxd );
}

void rx_append( struct RexxHost *host, struct rxd_append **rxd, long action )
{
	rx_multiddfunction( 0, rxd );
}

void rx_expandview( struct RexxHost *host, struct rxd_expandview **rxd, long action )
{
	rx_multiddfunction( 7, rxd );
}

void rx_growview( struct RexxHost *host, struct rxd_growview **rxd, long action )
{
	rx_multiddfunction( 8, rxd );
}

void rx_shrinkview( struct RexxHost *host, struct rxd_shrinkview **rxd, long action )
{
	rx_multiddfunction( 9, rxd );
}


void rx_loadblock( struct RexxHost *host, struct rxd_loadblock **rxd, long action )
{
	struct rxd_loadblock *rd = *rxd;

	if( rd->arg.name )
	{
		if( !LoadBlock( rd->arg.name ))
			rd->rc = RC_WARN;
	}
	else
		if( !LoadBlockFR())
			rd->rc = RC_WARN;
}

void rx_saveblock( struct RexxHost *host, struct rxd_saveblock **rxd, long action )
{
	struct rxd_saveblock *rd = *rxd;

	if( rd->arg.name )
	{
		if( !SaveBlock( rd->arg.name ))
			rd->rc = RC_WARN;
	}
	else
		if( !SaveBlockFR())
			rd->rc = RC_WARN;
}

void rx_loadsettings( struct RexxHost *host, struct rxd_loadsettings **rxd, long action )
{
	struct rxd_loadsettings *rd = *rxd;

	if( rd->arg.name )
	{
		if(!LoadSettings(rd->arg.name,FALSE))rd->rc=RC_WARN;
	}
	else
	{	
		if(!LoadSettingsFR())rd->rc=RC_WARN;
	}
}

void rx_savesettings( struct RexxHost *host, struct rxd_savesettings **rxd, long action )
{
	SaveSettingsDefault();
}

void rx_savesettingsas( struct RexxHost *host, struct rxd_savesettingsas **rxd, long action )
{
	struct rxd_savesettingsas *rd = *rxd;

	if(rd->arg.name)
	{
		if(!SaveSettings(rd->arg.name,FALSE))rd->rc=RC_WARN;
	}
	else
	{
		if(!SaveSettingsFR())rd->rc=RC_WARN;
	}
}

void rx_loaddisplaytyp( struct RexxHost *host, struct rxd_loaddisplaytyp **rxd, long action )
{
	struct rxd_loaddisplaytyp *rd = *rxd;

	if(rd->arg.name)
	{
		if(!LoadUserDisplaytyp(rd->arg.name))rd->rc=RC_WARN;
	}
	else
		if(!LoadUserDisplaytypFR())rd->rc=RC_WARN;
}

void rx_assigncommand( struct RexxHost *host, struct rxd_assigncommand **rxd, long action )
{
	struct rxd_assigncommand *rd = *rxd;

	if(rd->arg.number)
	{
		SetARexxCommand(*rd->arg.number,rd->arg.name);
	}
	else
	{
		SetARexxCommand(-1,rd->arg.name);
	}
}

void rx_executecommand( struct RexxHost *host, struct rxd_executecommand **rxd, long action )
{
	struct rxd_executecommand *rd = *rxd;

	if(rd->arg.number)
	{
		ExecuteARexxCommandNumber(*rd->arg.number);
	}
	else
	if(rd->arg.name)
	{
		ExecuteARexxCommand(rd->arg.name);
	}
}

void rx_loadcommands( struct RexxHost *host, struct rxd_loadcommands **rxd, long action )
{
	struct rxd_loadcommands *rd = *rxd;

	if(rd->arg.name)
	{
		if(!LoadARexxCommands(rd->arg.name))rd->rc=RC_WARN;
	}
	else
		if(!LoadARexxCommandsFR())rd->rc=RC_WARN;
}

void rx_savecommands( struct RexxHost *host, struct rxd_savecommands **rxd, long action )
{
	SaveARexxCommandsDefault();
}

void rx_savecommandsas( struct RexxHost *host, struct rxd_savecommandsas **rxd, long action )
{
	struct rxd_savecommandsas *rd = *rxd;

	if(rd->arg.name)
	{
		if(!SaveARexxCommands(rd->arg.name))rd->rc=RC_WARN;
	}
	else
		if(!SaveARexxCommandsFR())rd->rc=RC_WARN;
}

void rx_setscreen( struct RexxHost *host, struct rxd_setscreen **rxd, long action )
{
	struct rxd_setscreen *rd = *rxd;

	if( rd->arg.own )
	{
		SetScreenOwnPublic();
	}
	else if(rd->arg.name)
	{
		SetScreenTypePublic( rd->arg.name );
	}
}

void rx_getblock( struct RexxHost *host, struct rxd_getblock **rxd, long action )
{
	struct rxd_getblock *rd = *rxd;

	switch( action )
	{
		case RXIF_ACTION:
			{
				UBYTE *blockmem;
				ULONG blocklaenge;

				if( blockmem = GetClip( &blocklaenge ))
				{
					if(blocklaenge<32767)
					{
						if(rd->res.block = AllocVec(blocklaenge*2+1,MEMF_ANY))
						{
							UBYTE *mem=rd->res.block,*bmem=blockmem;
							long k;
							
							for(k=0;k<blocklaenge;k++)
							{
								*mem++=hexarray[*bmem >>  4];
								*mem++=hexarray[*bmem++ &  0xf];
							}
							*mem=0;
						}
					}
					else
					{
						rd->rc=-RC_WARN;
						rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_BLOCKTOOLONG );
					}

					FreeMem( blockmem, blocklaenge );
				}
				else
				{
					rd->rc=-RC_WARN;
					rd->rc2=(LONG) GetStr( MSG_INFO_GLOBAL_BUFFEREMPTY );
				}
			}
			break;
		
		case RXIF_FREE:
			if(rd->res.block)FreeVec(rd->res.block);
			break;
	}
}

void rx_setblock( struct RexxHost *host, struct rxd_setblock **rxd, long action )
{
	struct rxd_setblock *rd = *rxd;
	UBYTE *ptr;
	long anz=0;

	ptr=rd->arg.block;

	while(*ptr){if(isxdigit(*ptr))anz++;ptr++;}
	
	if(ptr = AllocVec(anz/2,MEMF_ANY))
	{
		if(StringToMem(rd->arg.block, ptr, anz/2))
		{
			SaveClip(ptr,anz/2);
		}
		else
		{
			rd->rc=-RC_WARN;
			rd->rc2= (LONG) GetStr( MSG_INFO_AREXX_BLOCKCONTAINSILLEGALCHARACTERS );
		}
		FreeVec( ptr );
	}
}

void rx_gotobyte( struct RexxHost *host, struct rxd_gotobyte **rxd, long action )
{
	struct rxd_gotobyte *rd = *rxd;
	struct DisplayData *DD;

	if(( *rxd )->arg.id )
	{
		if( ValidDD( *( *rxd )->arg.id ))
		{
			DD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		DD = AktuDD;

	if( !(*rxd)->rc )
	{
		if( rd->arg.pos )
			SetCursor( *rd->arg.pos, DD );
	}
}

void rx_getbytes( struct RexxHost *host, struct rxd_getbytes **rxd, long action )
{
	struct rxd_getbytes *rd = *rxd;

	switch( action )
	{
		case RXIF_ACTION:
			{
				struct FileData *FD;

				if(( *rxd )->arg.id )
				{
					if( ValidFD( *( *rxd )->arg.id ))
					{
						FD = *( *rxd )->arg.id;
					}
					else
					{
						(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
						(*rxd)->rc = -RC_WARN;
					}
				}
				else
					FD = AktuDD->FD;
			
				if( !(*rxd)->rc )
				{
					if(*rd->arg.number<32767)
					{
						if(rd->res.hexstring = AllocVec(*rd->arg.number*2+1,MEMF_ANY))
						{
							UBYTE *ptr=rd->res.hexstring,*bmem=FD->Mem+*rd->arg.pos;
							long k;
							
							for(k=0;k<*rd->arg.number;k++)
							{
								*ptr++=hexarray[*bmem >>  4];
								*ptr++=hexarray[*bmem++ &  0xf];
							}
							*ptr=0;
						}
					}
					else
					{
						rd->rc=-RC_WARN;
						rd->rc2=(LONG) GetStr( MSG_INFO_AREXX_NUMBERTOOHIGH );
					}
				}
			}
			break;
		
		case RXIF_FREE:
			if(rd->res.hexstring) FreeVec(rd->res.hexstring);
			break;
	}
}

void rx_setbytes( struct RexxHost *host, struct rxd_setbytes **rxd, long action )
{
	struct rxd_setbytes *rd = *rxd;
	UBYTE *ptr;
	long anz=0;
	struct FileData *FD;

	if(( *rxd )->arg.id )
	{
		if( ValidFD( *( *rxd )->arg.id ))
		{
			FD = *( *rxd )->arg.id;
		}
		else
		{
			(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
			(*rxd)->rc = -RC_WARN;
		}
	}
	else
		FD = AktuDD->FD;

	if( !(*rxd)->rc )
	{
		ptr=rd->arg.hexstring;

		while(*ptr){if(isxdigit(*ptr))anz++;ptr++;}
	
		if(ptr = AllocMem(anz/2,MEMF_ANY))
		{
			if(StringToMem(rd->arg.hexstring, ptr, anz/2))
			{
				if(*rd->arg.pos+anz/2 <= FD->Len)
				{
					struct Speicher s, ps;
	
					ps.mem = ptr;
					ps.len = s.len = anz/2;
	
					if(s.mem = AllocMem(s.len,MEMF_ANY))
					{
						AddUndoPaste(*rd->arg.pos,s,ps,FD);
				
							/* Sichern */
				
						memcpy(s.mem, FD->Mem+*rd->arg.pos, s.len);
				
							/* String drüber */
				
						memcpy(FD->Mem+*rd->arg.pos, ptr, s.len);
					
						AllDisplay( FD, SCROLLERNEU );
					}
					else
					{
						rd->rc=RC_WARN;
						rd->rc2 = ERROR_NO_FREE_STORE;
					}
				}
				else
				{
					rd->rc=-RC_WARN;
					rd->rc2= (LONG) GetStr( MSG_INFO_AREXX_HEXSTRINGTOOLONG );
				}
			}
			else
			{
				rd->rc=-RC_WARN;
				rd->rc2= (LONG) GetStr( MSG_INFO_AREXX_HEXSTRINGCONTAINSILLEGALCHARACTERS );
			}
	
			if(rd->rc)FreeMem(ptr,anz/2);
		}
	}
}

void rx_new( struct RexxHost *host, struct rxd_new **rxd, long action )
{
	if(( *rxd )->arg.view != FALSE )
	{
		NewDisplay( FALSE );
	}
	else if(( *rxd )->arg.window != FALSE )
	{
		OpenNewWindow( FALSE );
	}
}

void rx_split( struct RexxHost *host, struct rxd_split **rxd, long action )
{
	if(( *rxd )->arg.view != FALSE )
	{
		NewDisplay( TRUE );
	}
	else if(( *rxd )->arg.window != FALSE )
	{
		OpenNewWindow( TRUE );
	}
}

void rx_close( struct RexxHost *host, struct rxd_close **rxd, long action )
{
	BOOL Force = FALSE;

	if(( *rxd )->arg.force != FALSE )
		Force = TRUE;

	if(( *rxd )->arg.view != FALSE )
	{
		if(( *rxd )->arg.id )
		{
			if( ValidDD( *( *rxd )->arg.id ))
				CloseView( *(*rxd)->arg.id, Force );
			else
			{
				(*rxd)->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
				(*rxd)->rc = -RC_WARN;
			}
		}
		else
			CloseView( AktuDD, Force );
	}
	else if(( *rxd )->arg.window != FALSE )
	{
		if(( *rxd )->arg.id )
		{
			if( ValidDI( *( *rxd )->arg.id ))
				CloseDIWindow( *(*rxd)->arg.id, Force );
			else
			{
				( *rxd )->rc2 = (LONG) GetStr( MSG_INFO_AREXX_INVALID_ID );
				( *rxd )->rc = -RC_WARN;
			}
		}
		else
			CloseDIWindow( AktuDI, Force );
	}
}

char *geburtstagstext = "Today is my birthday! If you send me a letter with "\
"the correct date, you'll get a free registration!";	/* Höhö :-) */

void rx_next( struct RexxHost *host, struct rxd_next **rxd, long action )
{
	if(( *rxd )->arg.view != FALSE )
	{
		ActivateNextDisplay();
	}
	else if(( *rxd )->arg.window != FALSE )
	{
		ActivateNextWindow();
	}
}

void rx_previous( struct RexxHost *host, struct rxd_previous **rxd, long action )
{
	if(( *rxd )->arg.view != FALSE )
	{
		ActivatePreviousDisplay();
	}
	else if(( *rxd )->arg.window != FALSE )
	{
		ActivatePreviousWindow();
	}
}

void rx_lockgui( struct RexxHost *host, struct rxd_lockgui **rxd, long action )
{
	if( mainflags & MF_LOCKGUI )
		return;

	if(( mainflags & MF_WDWINDOWOPEN ) || ( mainflags & MF_LISTREQ ))
	{
		( *rxd )->rc2 = (LONG) GetStr( MSG_INFO_AREXX_CANT_LOCK_GUI );
		( *rxd )->rc = -RC_WARN;
		return;
	}

	mainflags |= MF_LOCKGUI;

	BlockMainWindow();
}

void rx_unlockgui( struct RexxHost *host, struct rxd_lockgui **rxd, long action )
{
	if( mainflags & MF_LOCKGUI )
	{
		mainflags &= ~MF_LOCKGUI;

		ReleaseMainWindow();
	}
}

#endif /* AREXX */

#ifndef RX_ALIAS_C
char *ExpandRXCommand( struct RexxHost *host, char *command )
{
	/* Insert your ALIAS-HANDLER here */
	return( NULL );
}
#endif

