/*
** $Id$
*/

#include "mystring.h"
#include "structs.h"
#include "misc.h"
#include "textinput_mcc.h"
#include "cmanager.h"
#include "cmanager_protos.h"
#include "cmanager_pragmas.h"
#include <rexx/rxslib.h>
#include <proto/rexxsyslib.h>

#define MAXRXOUTSTANDING (300)
#define RXERRORNOCMD (30)

extern struct Library *CManagerBase;

void openrexxport( struct Data *data )
{
	if( !data->rexxPort )
		data->rexxPort = CreateMsgPort();
	data->rexxPortBits = 1 << data->rexxPort->mp_SigBit;
}


ULONG disprexxport( struct Data *data )
{
	struct RexxMsg *RexxMsg;
	ULONG gotmsgs = 0;

	if( !data->rexxPort )
		return( FALSE );
	while( RexxMsg = (struct RexxMsg *)GetMsg( data->rexxPort ) )
	{
		gotmsgs = TRUE;
		if( RexxMsg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG )
		{
			if( *((char *)RexxMsg->rm_Result2) == 'b' )
				DoMethod( (Object *)RexxMsg->rm_Args[ 3 ], MUIM_Textinput_HandleMisspell, strchr( RexxMsg->rm_Args[ 1 ], ' ' ) + 1, RexxMsg->rm_Args[ 2 ], RexxMsg->rm_Result2 );
			FreeVec( RexxMsg->rm_Args[ 1 ] );
			DeleteArgstring( RexxMsg->rm_Args[ 0 ] );
			DeleteRexxMsg( RexxMsg );
			data->stillNeedReplies--;
		}
		else
		{
			// We somehow got sent a message? Wacky.
			ReplyMsg( (struct Message *)RexxMsg );
		}
	}
	return( gotmsgs );
}

void closerexxport( struct Data *data )
{
	if( data->rexxPort )
	{
		while( data->stillNeedReplies )
		{
			WaitPort( data->rexxPort );
			disprexxport( data );
		}
		DeleteMsgPort( data->rexxPort );
		data->rexxPort = NULL;
	}
	data->rexxPortBits = 0;
}

struct RexxMsg *sendrexxcmd( struct Data *data, char *host, char *s, STRPTR p1, STRPTR p2, STRPTR p3 )
{
	struct MsgPort *rexxport;
	struct RexxMsg *RexxMsg;

	if( !data->rexxPort || data->stillNeedReplies > MAXRXOUTSTANDING - 1 )
		return( NULL );
	if( (RexxMsg = CreateRexxMsg( data->rexxPort, NULL, NULL ) ) && ( RexxMsg->rm_Args[0] = CreateArgstring( s, strlen (s) ) ) )
	{
		RexxMsg->rm_Action = RXCOMM | RXFF_NOIO | RXFF_RESULT | RXFF_STRING; // Just RXCOMM?
		RexxMsg->rm_Args[1] = p1;
		RexxMsg->rm_Args[2] = p2;
		RexxMsg->rm_Args[3] = p3;
		RexxMsg->rm_Node.mn_Node.ln_Name = host;
		Forbid();
		if( rexxport = FindPort( host ) )
			PutMsg( rexxport, (struct Message *)RexxMsg );
		Permit();
		if( rexxport )
		{
			data->stillNeedReplies++;
			return( RexxMsg );
		}
		else
			DeleteArgstring( RexxMsg->rm_Args[0] );
	}
	if( RexxMsg )
		DeleteRexxMsg( RexxMsg );
	return( NULL );
}

void freemisspells( struct Data *data )
{
	struct misspell *next = data->misspells;
	while( next )
	{
		struct misspell *ms;
		ms = next;
		next = ms->next;
		FreePooled( data->pool, ms, sizeof( *ms ) + strlen( ms->misspell ) + 1 );
	}
	data->misspells = NULL;
}

void freeurls( struct Data *data )
{
	struct viewurl *next = data->urls;
	while( next )
	{
		struct viewurl *vu;
		vu = next;
		next = vu->next;
		FreePooled( data->pool, vu, sizeof( *vu ) + strlen( vu->url ) + 1 );
	}
	data->urls = NULL;
}

void findurls( struct Data *data )
{
	struct VATS_URL *url, *urlp;
	struct viewurl *vu, *vulist = NULL;
	if( !PARSEURLS )
		return;
	if( !data->buffer )
		return;
	freeurls( data );
	url = urlp = VAT_ScanForURLS( data->buffer );
	while( urlp )
	{
		vu = AllocPooled( data->pool, sizeof( *vu ) + strlen( urlp->url ) + 1 );
		if( vu )
		{
			strcpy( vu->url, urlp->url );
			vu->word.xstart = urlp->offset;
			vu->word.xsize = urlp->len;
			vu->next = vulist;
			vu->selected = FALSE;
			vulist = vu; 
		}
		urlp = urlp->next;
	}
	data->urls = vulist;
	if( url )
		VAT_FreeURLList( url );
}

struct CMUser *cm_getentry( void )
{
	APTR handle;
	struct CMUser *cmu = 0;

	handle = CM_StartManager( NULL, NULL );
	if( handle )
	{
		cmu = CM_GetEntry( handle, CMGE_USER );
		CM_FreeHandle( handle, TRUE );
	}
	return( cmu );
}

struct FileInfoBlock *FileInfo( UBYTE *name, struct FileInfoBlock *fib )
{
	BPTR handle;
	if( handle = Lock( name, ACCESS_READ ) )
	{
		if( Examine( handle, fib ) )
		{
			UnLock( handle );
			return( fib );
		}
		else
			UnLock( handle );
	}
	return( NULL );
}

BOOL DoExecute(UBYTE *cmd, BOOL async, UBYTE *output, UBYTE *directory, ULONG stack, WORD prio)
{
	BPTR newCurrentDir;
	BOOL success;
	success = FALSE;
	if( newCurrentDir = Lock( directory, SHARED_LOCK ) )
	{
		BPTR inHandle, outHandle, oldCurrentDir;
		oldCurrentDir = CurrentDir(newCurrentDir);
		if (outHandle = Open( output, MODE_NEWFILE ) )
		{
			struct MsgPort *oldct, *newct;
			if( IsInteractive( outHandle ) )
			{
				newct = ( ( struct FileHandle * ) BADDR( outHandle ) )->fh_Type;
				oldct = SetConsoleTask( newct );
				inHandle = Open( "CONSOLE:", MODE_OLDFILE );
				SetConsoleTask( oldct );
			}
			else
			{
				newct = NULL;
				inHandle = Open( "NIL:", MODE_OLDFILE );
			}
			if( inHandle )
			{
				struct TagItem tagItems[] = {
				 SYS_Output,		(ULONG)NULL,
				 SYS_Input,		(ULONG)NULL,
				 NP_ConsoleTask, (ULONG)NULL,
				 SYS_Asynch,		(ULONG)FALSE,
				 NP_StackSize,	(ULONG)8196,
				 NP_Priority,	(ULONG)0,
				 SYS_UserShell,	(ULONG)TRUE,
				 TAG_DONE
				};
				tagItems[0].ti_Data = (ULONG)outHandle;
				tagItems[1].ti_Data = (ULONG)inHandle;
				tagItems[2].ti_Data = (ULONG)newct;
				tagItems[3].ti_Data = (ULONG)async;
				tagItems[4].ti_Data = (ULONG)stack;
				tagItems[5].ti_Data = (ULONG)prio;
				if( SystemTagList( cmd, tagItems ) != -1L )
					success = TRUE;
				if ( !( async && success ) )
					Close( inHandle );
			}
			if( !( async && success ) )
				Close( outHandle );
		}
		CurrentDir( oldCurrentDir );
		UnLock( newCurrentDir );
	}
	return( success );
}

BOOL ispell_run( void )
{
	struct MsgPort *port;
	UBYTE hashfile[255], language[255], personal[255], options[255], command[255], home[255];
	struct Process *me = (struct Process *)FindTask( NULL );
	APTR oldwp = me->pr_WindowPtr;
	__aligned struct FileInfoBlock fib = { 0 };
	Forbid();
	port = FindPort( "IRexxSpell" );
	Permit();
	if( port )
		return( TRUE );
	strcpy( language, "english" );
	strcpy( options, "-W 1" );
	if( GetVar( "WORDS.prefs", command, sizeof(command), GVF_GLOBAL_ONLY ) != -1 )
	{
		if( *command )
		{
			struct RDArgs *rdArgs, *args;
			if( rdArgs = AllocDosObject( DOS_RDARGS, NULL ) )
			{
				LONG argArray[] = { 0, 0, 0, 0, 0 };
				strcat(command, "\n");
				rdArgs->RDA_Source.CS_Buffer = command;
				rdArgs->RDA_Source.CS_Length = strlen(command);
				rdArgs->RDA_Source.CS_CurChr = 0;
				rdArgs->RDA_DAList			 = 0;
				rdArgs->RDA_Buffer			 = NULL;
				if (args = ReadArgs( "LANGUAGE/K,QUICKCHECK/K,BOUNDARYCHARS/K,BEEP/N,ARGS/F", argArray, rdArgs ) )
				{
					if (argArray[ 0 ])			 // LANGUAGE/K
						strcpy( language, (UBYTE *)argArray[ 0 ] );
					if (argArray[ 4 ])			 // ARGS/F
						strcpy( options, (UBYTE *)argArray[ 4 ]);
					FreeArgs(args);
				}
				FreeDosObject(DOS_RDARGS, rdArgs);
			}
		}
	}
	sprintf( personal, "/ispell/lib/.ispell_%s", language );
	sprintf( hashfile, "ispell:lib/%s.hash", language );
	me->pr_WindowPtr = (APTR)-1;
	if( FileInfo( hashfile, &fib ) )
	{
		ULONG avail = AvailMem( MEMF_LARGEST );
		if( fib.fib_Size < avail )
		{
			int try;
			if( GetVar("HOME", home, sizeof(home), GVF_GLOBAL_ONLY) == -1)
				SetVar("HOME", "SYS:", -1, GVF_GLOBAL_ONLY);
			me->pr_WindowPtr = (APTR)-1;
			if( *options)
				sprintf( command, "ISpell:bin/ispell -d%s -p%s -r %s", hashfile, personal, options );
			else
				sprintf( command, "ISpell:bin/ispell -d%s -p%s -r", hashfile, personal );
			if( DoExecute( command, TRUE, "NIL:", "ispell:", 32768, 1 ) )
			{
				for( try = 50; try-- && ( port == NULL ); Delay( 10 ) )
				{
					Forbid();
					port = FindPort( "IRexxSpell" );
					Permit();
				}
			}
			if ( port )
			{
				me->pr_WindowPtr = oldwp;
				return( TRUE );
			}
		}
	}
	me->pr_WindowPtr = oldwp;
	return( FALSE );

}
