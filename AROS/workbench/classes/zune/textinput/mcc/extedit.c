/*
** $Id$
*/

#include "mystring.h"
#include "textinput_mcc.h"
#include "textinput_cfg.h"
#include "rev.h"

#define D_S(type,name) char a_##name[sizeof(type)+3]; \
                       type *name = (type *)((LONG)(a_##name+3) & ~3);

struct exteditinfonode {
	struct Message m;
	int cancel;
	char filename[ 30 ];
	char editcall[ 200 ];
	APTR obj;
	int sync;
	struct Process *handleproc;
};

static void __saveds handlefunc( void )
{
	struct exteditinfonode *ee;
	struct Process *me = ( struct Process *)FindTask( NULL );
	char buffer[ 256 ];
	BPTR l;
	struct DateStamp ds;
	struct timeval tv1, tv2;
	int changed = FALSE;
	int rc;
	struct NotifyRequest nr;
	D_S(struct FileInfoBlock,fib);

	while( !( ee = ( struct exteditinfonode*)GetMsg( &me->pr_MsgPort ) ) )
		WaitPort( &me->pr_MsgPort );

	l = Lock( ee->filename, SHARED_LOCK );
	Examine( l, fib );
	UnLock( l );
	ds = fib->fib_Date;

	strcpy( buffer, ee->editcall );
	strcat( buffer, " " );
	strcat( buffer, ee->filename );

	if( !ee->sync )
	{
		ULONG sigs;

		memset( &nr, 0, sizeof( nr ) );
		nr.nr_Name = ee->filename;
		nr.nr_stuff.nr_Signal.nr_SignalNum = SIGBREAKB_CTRL_F;
		nr.nr_stuff.nr_Signal.nr_Task = me;
		nr.nr_Flags = NRF_SEND_SIGNAL;
		StartNotify( &nr );
startagain:
		rc = SystemTags( buffer,
			SYS_Asynch, TRUE,
			NP_StackSize, 16384,
			SYS_Input, Open( "NIL:", MODE_NEWFILE ),
			SYS_Output, Open( "NIL:", MODE_NEWFILE ),
			TAG_DONE
		);
		
		for(;;)
		{
			changed = FALSE;
			sigs = Wait( SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F | SIGBREAKF_CTRL_E );
			if( sigs & SIGBREAKF_CTRL_C )
				break;
			if( sigs & SIGBREAKF_CTRL_E )
				goto startagain;
			Forbid();
			if( ee->obj && muiGlobalInfo( ee->obj ) )
			{
				DoMethod( _app( ee->obj	), MUIM_Application_PushMethod, ee->obj, 2, MUIM_Textinput_ExternalEditDone, 2 );
			}
			Permit();
		}
	
		EndNotify( &nr );
	}
	else
	{
		GetSysTime( &tv1 );
		rc = SystemTags( buffer, 
			NP_StackSize, 16384,
			TAG_DONE 
		);
		GetSysTime( &tv2 );

		l = Lock( ee->filename, SHARED_LOCK );
		Examine( l, fib );
		UnLock( l );
#define DNE( x ) (ds.ds_##x!=fib->fib_Date.ds_##x)
		if( DNE( Days ) || DNE( Minute ) || DNE( Tick ) )
			changed = TRUE;

		SubTime( &tv2, &tv1 );
		if( !tv2.tv_secs )
			changed = -1;
	}

	if( rc < 0 || rc >= 10 )
		changed = -1;

	Forbid();
	if( ee->obj && muiGlobalInfo( ee->obj ) )
	{
		DoMethod( _app( ee->obj	), MUIM_Application_PushMethod, ee->obj, 2, MUIM_Textinput_ExternalEditDone, changed );
	}
	Permit();

	ee->handleproc = NULL;
}

APTR ee_add( char *filename, APTR obj, int sync )
{
	struct exteditinfonode *ee;
	char *s;

	ee = AllocMem( sizeof( *ee ), MEMF_CLEAR );
	if( !ee )
		return( NULL );

	strcpy( ee->filename, filename );
	ee->obj = obj;

	s = NULL;
	DoMethod( obj, MUIM_GetConfigItem, MUICFG_Textinput_ExternalEditor, &s );
	if( s && *s )
	{
		stccpy( ee->editcall, s, sizeof( ee->editcall ) );
	}
	else
	{
		if( GetVar( "EDITOR", ee->editcall, sizeof( ee->editcall ), 0 ) < 1 )
		{
			strcpy( ee->editcall, "ED" );
		}
	}

	ee->sync = sync;

	ee->handleproc = CreateNewProcTags(
		NP_Entry, handlefunc,
		NP_Name, "Textinput ExternalEdit Manager",
		NP_Priority, 1,
		NP_Cli, TRUE,
		NP_StackSize, 16 * 1024,
		TAG_DONE
	);

	if( !ee->handleproc )
	{
		FreeMem( ee, sizeof( *ee ) );
		return( NULL );
	}

	PutMsg( &ee->handleproc->pr_MsgPort, ee );

	return( ee );
}

void ee_free( struct exteditinfonode *ee )
{
	APTR obj = ee->obj;
	ee->obj = NULL;
	Forbid();
	while( ee->handleproc )
	{
		Signal( ee->handleproc, SIGBREAKF_CTRL_C );
		Delay( 1 );
	}
	Permit();
	FreeMem( ee, sizeof( *ee ) );
	if( obj )
		DoMethod( _app( obj ), MUIM_Application_KillPushMethod, obj );
}

void ee_setobj( struct exteditinfonode *ee, APTR obj )
{
	ee->obj = obj;
}

void ee_restart( struct exteditinfonode *ee )
{
	Forbid();
	if( ee->handleproc )
		Signal( ee->handleproc, SIGBREAKF_CTRL_E );
	Permit();

}
