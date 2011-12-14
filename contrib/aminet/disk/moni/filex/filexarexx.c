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

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/rexxsyslib.h>
#include <proto/utility.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/rexxsyslib_pragmas.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dos/rdargs.h>

#include "filexarexx.h"

#include "filexstructs.h"
#include "allprotos.h"

struct rxs_stemnode
{
	struct rxs_stemnode *succ;
	char *name;
	char *value;
};


void (*ARexxResultHook)( struct RexxHost *, struct RexxMsg * ) = NULL;



void ReplyRexxCommand(
	struct RexxMsg	*rexxmessage,
	long			primary,
	long			secondary,
	char			*result )
{
	if( rexxmessage->rm_Action & RXFF_RESULT )
	{
		if( primary == 0 )
		{
			secondary = result
				? (long) CreateArgstring( result, strlen(result) )
				: (long) NULL;
		}
		else
		{
			char buf[16];
			
			if( primary > 0 )
			{
				sprintf( buf, "%ld", secondary );
				result = buf;
			}
			else
			{
				primary = -primary;
				result = (char *) secondary;
			}
			
			SetRexxVar(rexxmessage, "RC2", result, strlen(result) );
			
			secondary = 0;
		}
	}
	else if( primary < 0 )
		primary = -primary;
	
	rexxmessage->rm_Result1 = primary;
	rexxmessage->rm_Result2 = secondary;
	ReplyMsg( (struct Message *) rexxmessage );
}


void FreeRexxCommand( struct RexxMsg *rexxmessage )
{
	if( !rexxmessage->rm_Result1 && rexxmessage->rm_Result2 )
		DeleteArgstring( (char *) rexxmessage->rm_Result2 );

	if( rexxmessage->rm_Stdin &&
		rexxmessage->rm_Stdin != Input() )
		Close( rexxmessage->rm_Stdin );

	if( rexxmessage->rm_Stdout &&
		rexxmessage->rm_Stdout != rexxmessage->rm_Stdin &&
		rexxmessage->rm_Stdout != Output() )
		Close( rexxmessage->rm_Stdout );

	DeleteArgstring( (char *) ARG0(rexxmessage) );
	DeleteRexxMsg( rexxmessage );
}


struct RexxMsg *CreateRexxCommand( struct RexxHost *host, CONST_STRPTR buff, BPTR fh )
{
	struct RexxMsg *rexx_command_message;

	if( (rexx_command_message = CreateRexxMsg( host->port,
		rexx_extension, host->port->mp_Node.ln_Name)) == NULL )
	{
		return( NULL );
	}

	if( (rexx_command_message->rm_Args[0] =
		(IPTR)CreateArgstring(buff,strlen(buff))) == 0 )
	{
		DeleteRexxMsg(rexx_command_message);
		return( NULL );
	}

	rexx_command_message->rm_Action = RXCOMM | RXFF_RESULT;
	rexx_command_message->rm_Stdin  = fh;
	rexx_command_message->rm_Stdout = fh;
	
	return( rexx_command_message );
}


struct RexxMsg *CommandToRexx( struct RexxHost *host, struct RexxMsg *rexx_command_message )
{
	struct MsgPort *rexxport;
	
	Forbid();

	if( (rexxport = FindPort(RXSDIR)) == NULL )
	{
		Permit();
		return( NULL );
	}

	PutMsg( rexxport, &rexx_command_message->rm_Node );
	
	Permit();
	
	++host->replies;
	return( rexx_command_message );
}


struct RexxMsg *SendRexxCommand( struct RexxHost *host, CONST_STRPTR buff, BPTR fh )
{
	struct RexxMsg *rcm;
	
	if(( rcm = CreateRexxCommand(host, buff, fh) ))
		return CommandToRexx( host, rcm );
	else
		return NULL;
}


void CloseDownARexxHost( struct RexxHost *host )
{
	struct RexxMsg *rexxmsg;
	
	if( host->port )
	{
		/* Port abmelden */
		RemPort( host->port );
		
		/* Auf noch ausstehende Replies warten */
		while( host->replies > 0 )
		{
			WaitPort( host->port );
			
			while(( rexxmsg = (struct RexxMsg *) GetMsg(host->port) ))
			{
				if( rexxmsg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG )
				{
					if( !rexxmsg->rm_Args[15] )
					{
						/* Reply zu einem SendRexxCommand()-Call */
						if( ARexxResultHook )
							ARexxResultHook( host, rexxmsg );
					}
					
					FreeRexxCommand( rexxmsg );
					--host->replies;
				}
				else
					ReplyRexxCommand( rexxmsg, -20, (long) "Host closing down", NULL );
			}
		}
		
		/* MsgPort leeren */
		while(( rexxmsg = (struct RexxMsg *) GetMsg(host->port) ))
			ReplyRexxCommand( rexxmsg, -20, (long) "Host closing down", NULL );
		
		if( !(host->flags & ARB_HF_USRMSGPORT) )
			DeleteMsgPort( host->port );
	}
	
	if( host->rdargs ) FreeDosObject( DOS_RDARGS, host->rdargs );
	FreeVec( host );
}


struct RexxHost *SetupARexxHost( char *basename, struct MsgPort *usrport )
{
	struct RexxHost *host;
	int ext = 1;
	
	if( !basename )
		basename = RexxPortBaseName;
	else if( !*basename )
		basename = RexxPortBaseName;
	
	if( !(host = AllocVec(sizeof *host, MEMF_CLEAR)) )
		return NULL;
	
	strcpy( host->portname, basename );
	
	if( usrport )
	{
		host->port   = usrport;
		host->flags |= ARB_HF_USRMSGPORT;
	}
	else if( !(host->port = CreateMsgPort()))
	{
		FreeVec( host );
		return NULL;
	}
	else
	{
		host->port->mp_Node.ln_Pri = 0;
	}
	
	Forbid();
	
	while( FindPort(host->portname) )
		sprintf( host->portname, "%s.%ld", basename, (long)(ext++) );
	
	host->port->mp_Node.ln_Name = host->portname;
	AddPort( host->port );
	
	Permit();
	
	if( !(host->rdargs = AllocDosObject(DOS_RDARGS, NULL)) )
	{
		RemPort( host->port );
		if( !usrport ) DeleteMsgPort( host->port );
		FreeVec( host );
		return NULL;
	}
	
	host->rdargs->RDA_Flags = RDAF_NOPROMPT;
	
	return( host );
}


struct rxs_command *FindRXCommand( char *com )
{
	struct rxs_command *rxc;
	int n = strlen( com ), ug = 0, og = command_cnt - 1, pos, cmp = 0;
	
	if( n == 0 )
		return NULL;
	
	/* Init */
	
	pos = (og + ug) / 2;
	rxc = rxs_commandlist + pos;
	
	/* Suchen */
	
	while((og>=ug) && (cmp = strnicmp(com, rxc->command,n)))
	{
		if(cmp < 0)
			og = pos - 1;
		else
			ug = pos + 1;
		
		pos = (og + ug) / 2;
		rxc = rxs_commandlist + pos;
	}
	
	if(cmp)
		return( NULL );
	else
	{
		while((rxc!=rxs_commandlist) &&
				(cmp = strnicmp(com, (rxc-1)->command,n)==0))
			rxc--;
		
		return(rxc);
	}
}


static struct rxs_command *ParseRXCommand( char **arg )
{
	char com[256], *s, *t;
	
	s = *arg;
	t = com;
	
	while( *s && *s != ' ' && *s != '\n' )
		*t++ = *s++;
	
	*t = '\0';
	while( *s == ' ' ) ++s;
	*arg = s;
	
	return( FindRXCommand( com ) );
}


static char *CreateVAR( struct rxs_stemnode *stem )
{
	char *var;
	struct rxs_stemnode *s;
	long size = 0;
	
	if( !stem || stem == (struct rxs_stemnode *) -1L )
		return( (char *) stem );
	
	for( s = stem; s; s = s->succ )
		size += strlen( s->value ) + 1;
	
	if( !(var = AllocVec( size + 1, MEMF_ANY )) )
		return( (char *) -1 );
	
	*var = '\0';
	
	for( s = stem; s; s = s->succ )
	{
		strcat( var, s->value );
		if( s->succ )
			strcat( var, " " );
	}
	
	return( var );
}


static struct rxs_stemnode *new_stemnode( struct rxs_stemnode **first, struct rxs_stemnode **old )
{
	struct rxs_stemnode *new;
	
	if( !(new = AllocVec(sizeof(struct rxs_stemnode), MEMF_CLEAR)) )
	{
		return( NULL );
	}
	else
	{
		if( *old )
		{
			(*old)->succ = new;
			(*old) = new;
		}
		else
		{
			*first = *old = new;
		}
	}
	
	return( new );
}


static void free_stemlist( struct rxs_stemnode *first )
{
	struct rxs_stemnode *next;
	
	if( (long) first == -1 )
		return;
	
	for( ; first; first = next )
	{
		next = first->succ;
		if( first->name  ) FreeVec( first->name );
		if( first->value ) FreeVec( first->value );
		FreeVec( first );
	}
}


static struct rxs_stemnode *CreateSTEM( struct rxs_command *rxc, IPTR *resarray, char *stembase )
{
	struct rxs_stemnode *first = NULL, *old = NULL, *new;
	char resb[512], *rs, *rb;
	char longbuff[16];
	
	rb = resb;
	if( stembase )
	{
		while( *stembase )
			*rb++ = ToUpper( *stembase++ );
	}
	*rb = '\0';
	
	rb = resb + strlen(resb);
	rs = rxc->results;
	
	while( *rs )
	{
		char *t = rb;
		BOOL optn = FALSE, optm = FALSE;
		
		while( *rs && *rs != ',' )
		{
			if( *rs == '/' )
			{
				++rs;
				if( *rs == 'N' ) optn = TRUE;
				else if( *rs == 'M' ) optm = TRUE;
			}
			else
				*t++ = *rs;
			
			++rs;
		}
		
		if( *rs == ',' ) ++rs;
		*t = '\0';
		
		/*
		 * Resultat(e) erzeugen
		 */
		
		if( !*resarray )
		{
			++resarray;
			continue;
		}
		
		if( optm )
		{
			IPTR *r, index = 0;
			IPTR **subarray = (IPTR **) *resarray++;
			struct rxs_stemnode *countnd;
			
			/* Anzahl der Elemente */
			
			if( !(new = new_stemnode(&first, &old)) )
			{
				free_stemlist( first );
				return( (struct rxs_stemnode *) -1L );
			}
			countnd = new;
			
			/* Die Elemente selbst */
			
			while(( r = *subarray++ ))
			{
				if( !(new = new_stemnode(&first, &old)) )
				{
					free_stemlist( first );
					return( (struct rxs_stemnode *) -1L );
				}
				
				sprintf( t, ".%ld", index++ );
				new->name = StrDup( resb );
				
				if( optn )
				{
					sprintf( longbuff, "%ld", *r );
					new->value = StrDup( longbuff );
				}
				else
				{
					new->value = StrDup( (char *) r );
				}
			}
			
			/* Die Count-Node */
			
			strcpy( t, ".COUNT" );
			countnd->name = StrDup( resb );
			
			sprintf( longbuff, "%ld", index );
			countnd->value = StrDup( longbuff );
		}
		else
		{
			/* Neue Node anlegen */
			if( !(new = new_stemnode(&first, &old)) )
			{
				free_stemlist( first );
				return( (struct rxs_stemnode *) -1L );
			}
			
			new->name = StrDup( resb );
			
			if( optn )
			{
				sprintf( longbuff, "%ld", *((long *) *resarray) );
				new->value = StrDup( longbuff );
				++resarray;
			}
			else
			{
				new->value = StrDup( (char *) (*resarray++) );
			}
		}
	}
	
	return( first );
}


static void DoRXCommand( struct RexxHost *host, struct RexxMsg *rexxmsg )
{
	struct rxs_command *rxc = NULL;
	char *argb, *arg;
	
	IPTR *array = NULL;
	IPTR *argarray;
	IPTR *resarray;
	
	char *cargstr = NULL;
	long rc=20, rc2=0;
	char *result = NULL;
	
	if( !(argb = AllocVec(strlen((char *) ARG0(rexxmsg)) + 2, MEMF_ANY)) )
	{
		rc2 = ERROR_NO_FREE_STORE;
		goto drc_cleanup;
	}
	
	/* welches Kommando? */
	
	strcpy( argb, (char *) ARG0(rexxmsg) );
	strcat( argb, "\n" );
	arg = argb;

	if( !( rxc = ParseRXCommand( &arg ) ) )
	{
		if(( arg = ExpandRXCommand( host, (char *) ARG0(rexxmsg) ) ))
		{
			FreeVec( argb );
			if( !(argb = AllocVec( strlen(arg) + 2, MEMF_ANY )) )
			{
				rc2 = ERROR_NO_FREE_STORE;
				goto drc_cleanup;
			}
			
			strcpy( argb, arg );
			strcat( argb, "\n" );
			FreeVec( arg );
			arg = argb;
			
			rxc = ParseRXCommand( &arg );
		}
	}
	
	if( !rxc )
	{
		/* Msg an ARexx schicken, vielleicht existiert ein Skript */
		struct RexxMsg *rm;
		
		if(( rm = CreateRexxCommand(host, (char *) ARG0(rexxmsg), BNULL) ))
		{
			/* Original-Msg merken */
			rm->rm_Args[15] = (IPTR) rexxmsg;
			
			if( CommandToRexx(host, rm) )
			{
				/* Reply wird später vom Dispatcher gemacht */
				if( argb ) FreeVec( argb );
				return;
			}
			else
				rc2 = ERROR_NOT_IMPLEMENTED;
		}
		else
			rc2 = ERROR_NO_FREE_STORE;
		
		goto drc_cleanup;
	}
	
	if( !(rxc->flags & ARB_CF_ENABLED) )
	{
		rc = -10;
		rc2 = (long) "Command disabled";
		goto drc_cleanup;
	}
	
	/* Speicher für Argumente etc. holen */
	
	if(!(rxc->flags & ARB_CF_OWNALLOC))
		array=AllocVec(rxc->structsize,MEMF_CLEAR);

	if(rxc->flags & ARB_CF_CALLINIT)
		(rxc->function)( host, (void **) &array, RXIF_INIT );

	cargstr = AllocVec( rxc->args ? 15+strlen(rxc->args) : 15, MEMF_ANY );

	if( !array || !cargstr )
	{
		rc2 = ERROR_NO_FREE_STORE;
		goto drc_cleanup;
	}
	
	argarray = array + 2;
	resarray = array + rxc->resindex;
	
	/* Argumente parsen */
	
	if( rxc->results )
		strcpy( cargstr, "VAR/K,STEM/K" );
	else
		*cargstr = '\0';
	
	if( rxc->args )
	{
		if( *cargstr )
			strcat( cargstr, "," );
		strcat( cargstr, rxc->args );
	}
	
	if( *cargstr )
	{
		host->rdargs->RDA_Source.CS_Buffer = arg;
		host->rdargs->RDA_Source.CS_Length = strlen(arg);
		host->rdargs->RDA_Source.CS_CurChr = 0;
		host->rdargs->RDA_DAList = (IPTR)NULL;
		host->rdargs->RDA_Buffer = NULL;
		
		if( !ReadArgs(cargstr, argarray, host->rdargs) )
		{
			rc = 10;
			rc2 = IoErr();
			goto drc_cleanup;
		}
	}
	
	/* Funktion aufrufen */

	host->akturexxmsg=rexxmsg;
	host->userdata=0;

	(rxc->function)( host, (void **) &array, RXIF_ACTION );

	rc = array[0];
	rc2 = array[1];

	/* Resultat(e) auswerten */

	if(host->userdata==0)
	if( rxc->results && rc==0 &&
		(rexxmsg->rm_Action & RXFF_RESULT) )
	{
		struct rxs_stemnode *stem, *s;
		
		stem = CreateSTEM( rxc, resarray, (char *)argarray[1] );
		result = CreateVAR( stem );
		
		if( result )
		{
			if( argarray[0] )
			{
				/* VAR */
				if( (long) result == -1 )
				{
					rc = 20;
					rc2 = ERROR_NO_FREE_STORE;
				}
				else
				{
					char *rb;
					
					for( rb = (char *) argarray[0]; *rb; ++rb )
						*rb = ToUpper( *rb );

					if( SetRexxVar( rexxmsg, (char *)argarray[0], result, strlen(result) ) )
					{
						rc = -10;
						rc2 = (long) "Unable to set Rexx variable";
					}
					
					FreeVec( result );
				}
				
				result = NULL;
			}
			
			if( !rc && argarray[1] )
			{
				/* STEM */
				if( (long) stem == -1 )
				{
					rc = 20;
					rc2 = ERROR_NO_FREE_STORE;
				}
				else
				{
					for( s = stem; s; s = s->succ )
						rc |= SetRexxVar( rexxmsg, s->name, s->value, strlen(s->value) );
					
					if( rc )
					{
						rc = -10;
						rc2 = (long) "Unable to set Rexx variable";
					}
					
					if( result && (long) result != -1 )
						FreeVec( result );
				}
				
				result = NULL;
			}
			
			/* Normales Resultat: Möglich? */
			
			if( (long) result == -1 )
			{
				/* Nein */
				rc = 20;
				rc2 = ERROR_NO_FREE_STORE;
				result = NULL;
			}
		}
		
		free_stemlist( stem );
	}
	
drc_cleanup:

	/* Nur RESULT, wenn weder VAR noch STEM */
	
	if(0==host->userdata)ReplyRexxCommand( rexxmsg, rc, rc2, result );
	
	/* benutzten Speicher freigeben */
	
	if( result ) FreeVec( result );
	FreeArgs( host->rdargs );
	if( cargstr ) FreeVec( cargstr );
	if( array )
	{
		if(rxc->flags & ARB_CF_CALLFREE)
			(rxc->function)( host, (void **) &array, RXIF_FREE );

		if(!(rxc->flags & ARB_CF_OWNALLOC))
			FreeVec(array);
	}

	if( argb ) FreeVec( argb );
}


void ARexxDispatch( struct RexxHost *host )
{
	struct RexxMsg *rexxmsg;

	while(( rexxmsg = (struct RexxMsg *) GetMsg(host->port) ))
	{
		if( (rexxmsg->rm_Action & RXCODEMASK) != RXCOMM )
		{
			/* Keine Rexx-Message */
			ReplyMsg( (struct Message *) rexxmsg );
		}
		else if( rexxmsg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG )
		{
			struct RexxMsg *org = (struct RexxMsg *) rexxmsg->rm_Args[15];
			
			if( org )
			{
				/* Reply zu durchgereichter Msg */
				if( rexxmsg->rm_Result1 != 0 )
				{
					/* Befehl unbekannt */
					ReplyRexxCommand( org, 20, ERROR_NOT_IMPLEMENTED, NULL );
				}
				else
				{
					ReplyRexxCommand( org, 0, 0, (char *) rexxmsg->rm_Result2 );
				}
			}
			else
			{
				/* Reply zu einem SendRexxCommand()-Call */
				if( ARexxResultHook )
					ARexxResultHook( host, rexxmsg );
			}
			
			FreeRexxCommand( rexxmsg );
			--host->replies;
		}
		else if( ARG0(rexxmsg) )
		{
			DoRXCommand( host, rexxmsg );
		}
		else
		{
			ReplyMsg( (struct Message *) rexxmsg );
		}
	}
}


void DoShellCommand( struct RexxHost *host, char *comline, BPTR fhout )
{
	struct rxs_command *rxc = NULL;
	char *argb, *arg;
	
	IPTR *array = NULL;
	IPTR *argarray;
	IPTR *resarray;
	
	char *cargstr = NULL;
	long rc=10, rc2=0;
	
	if( !(argb = AllocVec( strlen(comline) + 2, MEMF_ANY )) )
	{
		rc2 = ERROR_NO_FREE_STORE;
		goto dsc_cleanup;
	}
	
	/* welches Kommando? */
	
	strcpy( argb, comline );
	strcat( argb, "\n" );
	arg = argb;
	
	if( !( rxc = ParseRXCommand( &arg ) ) )
	{
		if(( arg = ExpandRXCommand( host, comline ) ))
		{
			FreeVec( argb );
			if( !(argb = AllocVec( strlen(arg) + 2, MEMF_ANY )) )
			{
				rc2 = ERROR_NO_FREE_STORE;
				goto dsc_cleanup;
			}
			
			strcpy( argb, arg );
			strcat( argb, "\n" );
			FreeVec( arg );
			arg = argb;
			
			rxc = ParseRXCommand( &arg );
		}
	}
	
	if( !rxc )
	{
		/* Msg an ARexx schicken, vielleicht existiert ein Skript */
		struct RexxMsg *sentrm, *rm;
		
		if(( sentrm = SendRexxCommand(host, comline, BNULL) ))
		{
			/* auf den Reply warten */
			BOOL waiting = TRUE;
			
			while( waiting )
			{
				WaitPort( host->port );
				
				while(( rm = (struct RexxMsg *) GetMsg(host->port) ))
				{
					/* Reply? */
					if( rm->rm_Node.mn_Node.ln_Type == NT_REPLYMSG )
					{
						/* zu diesem Kommando? */
						if( rm == sentrm )
						{
							if( rm->rm_Result1 )
							{
								rc  = 20;
								rc2 = ERROR_NOT_IMPLEMENTED;
							}
							else
							{
								rc = rc2 = 0;
								if( rm->rm_Result2 )
									FPrintf( fhout, "%s\n", rm->rm_Result2 );
							}
							
							waiting = FALSE;
						}
						
						FreeRexxCommand( rm );
						--host->replies;
					}
					
					/* sonst Kommando -> Fehler */
					else
					{
						ReplyRexxCommand( rm, -20, (long)
							"CommandShell Port", NULL );
					}
				}
			} /* while waiting */
		}
		else
			rc2 = ERROR_NO_FREE_STORE;
		
		goto dsc_cleanup;
	}
	
	if( !(rxc->flags & ARB_CF_ENABLED) )
	{
		rc = -10;
		rc2 = (long) "Command disabled";
		goto dsc_cleanup;
	}
	
	/* Speicher für Argumente etc. holen */
	
	if(!(rxc->flags & ARB_CF_OWNALLOC))
		array=AllocVec(rxc->structsize,MEMF_CLEAR);

	if(rxc->flags & ARB_CF_CALLINIT)
		(rxc->function)( host, (void **) &array, RXIF_INIT );

	cargstr = AllocVec( rxc->args ? 512+strlen(rxc->args) : 512, MEMF_ANY );
	
	if( !array || !cargstr )
	{
		rc2 = ERROR_NO_FREE_STORE;
		goto dsc_cleanup;
	}
	
	argarray = array + 2;
	resarray = array + rxc->resindex;
	
	/* Argumente parsen */
	
	*cargstr = '\0';
	
	if( rxc->results )
		strcpy( cargstr, "VAR/K,STEM/K" );
	
	if( rxc->args )
	{
		if( *cargstr )
			strcat( cargstr, "," );
		strcat( cargstr, rxc->args );
	}
	
	if( *cargstr )
	{
		host->rdargs->RDA_Source.CS_Buffer = arg;
		host->rdargs->RDA_Source.CS_Length = strlen(arg);
		host->rdargs->RDA_Source.CS_CurChr = 0;
		host->rdargs->RDA_DAList = (IPTR)NULL;
		host->rdargs->RDA_Flags  = RDAF_NOPROMPT;
		
		if( !ReadArgs(cargstr, argarray, host->rdargs) )
		{
			rc = 10;
			rc2 = IoErr();
			goto dsc_cleanup;
		}
	}
	
	/* Funktion aufrufen */
	
	host->outfh=fhout;

	(rxc->function)( host, (void **) &array, RXIF_ACTION );
	
	/* Resultat(e) ausgeben */
	
	if( rxc->results && array[0]==0 && fhout )
	{
		struct rxs_stemnode *stem, *s;
		char *result;
		
		stem = CreateSTEM( rxc, resarray, (char *)argarray[1] );
		result = CreateVAR( stem );
		
		if( result )
		{
			if( argarray[0] )
			{
				/* VAR */
				char *rb;
				
				if( (long) result == -1 )
				{
					free_stemlist( stem );
					rc2 = ERROR_NO_FREE_STORE;
					goto dsc_cleanup;
				}
				
				for( rb = (char *) argarray[0]; *rb; ++rb )
					*rb = ToUpper( *rb );
				
				FPrintf( fhout, "%s = %s\n", argarray[0], result );
				FreeVec( result );
				result = NULL;
			}
			
			if( argarray[1] )
			{
				/* STEM */
				if( (long) stem == -1 )
				{
					rc2 = ERROR_NO_FREE_STORE;
					goto dsc_cleanup;
				}
				
				for( s = stem; s; s = s->succ )
					FPrintf( fhout, "%s = %s\n", s->name, s->value );
				
				if( result && (long) result != -1 )
					FreeVec( result );
				result = NULL;
			}
			
			free_stemlist( stem );
			
			/* Normales Resultat: Möglich? */
			
			if( result )
			{
				if( (long) result != -1L )
				{
					/* Ja */
					FPrintf( fhout, "%s\n", result );
					FreeVec( result );
				}
				else
				{
					rc2 = ERROR_NO_FREE_STORE;
					goto dsc_cleanup;
				}
			}
		}
	}

	/* Rückgabewert */

	rc  = array[0];
	rc2 = array[1];

dsc_cleanup:

	arg = NULL;
	
	if( rc2 )
	{
		if( !cargstr ) cargstr = AllocVec( 512, MEMF_ANY );
		
		if( !cargstr )
		{
			arg = "ERROR: Absolutely out of memory";
		}
		else if( rc > 0 )
		{
			if( Fault( rc2, "ERROR", cargstr, 512 ) )
				arg = cargstr;
			else
				arg = "ERROR: Unknown Problem";
		}
		else if( rc < 0 )
		{
			strcpy( cargstr, "ERROR: " );
			strncat( cargstr, (char *) rc2, 500 );
			arg = cargstr;
		}
	}
	
	if( arg ) FPrintf( fhout, "%s\n", arg );
	
	/* benutzten Speicher freigeben */
	
	if( cargstr ) FreeVec( cargstr );
	if( array )
	{
		if(rxc->flags & ARB_CF_CALLFREE)
			(rxc->function)( host, (void **) &array, RXIF_FREE );

		if(!(rxc->flags & ARB_CF_OWNALLOC))
			FreeVec(array);
	}
	if( argb ) FreeVec( argb );
	FreeArgs( host->rdargs );
}


static struct StandardPacket *CreateStdPkt( void )
{
	struct StandardPacket *sp;

	if(( sp = AllocMem( sizeof( struct StandardPacket ), MEMF_PUBLIC )))
	{
		if(( sp->sp_Msg.mn_ReplyPort = CreatePort( NULL, 0 )))
		{
			sp->sp_Msg.mn_Node.ln_Name = ( char * )&sp->sp_Pkt;
			sp->sp_Pkt.dp_Link = &sp->sp_Msg;
			return( sp );
		}
		FreeMem( sp, sizeof( struct StandardPacket ));
	}

	return NULL;
}

static void DeleteStdPkt( struct StandardPacket *sp )
{
	DeletePort( sp->sp_Msg.mn_ReplyPort );
	FreeMem( sp, sizeof( struct StandardPacket ));
}

static void SendStdPkt( struct StandardPacket *sp, struct MsgPort *mp )
{
	sp->sp_Pkt.dp_Port = sp->sp_Msg.mn_ReplyPort;
	
	PutMsg( mp, &sp->sp_Msg );
}

static BOOL PacketUnterwegs;
static BPTR CSfh;
static char *CSbuffer;
static struct StandardPacket *CSsp;

void CloseCommandShell( void )
{
	if( MyRexxHost->flags & ARB_HF_CMDSHELL )
	{
		MyRexxHost->flags &= ~ARB_HF_CMDSHELL;
		MyRemoveSignal(  1 << CSsp->sp_Msg.mn_ReplyPort->mp_SigBit );

		Close( CSfh );
		CSfh = BNULL;

		if( PacketUnterwegs )
		{
/*			Wait(  1 << CSsp->sp_Msg.mn_ReplyPort->mp_SigBit );*/
			GetMsg( CSsp->sp_Msg.mn_ReplyPort );
			PacketUnterwegs = FALSE;
		}
	
		DeleteStdPkt( CSsp );
		CSsp = NULL;
	
		FreeMem( CSbuffer, 1024 );
		CSbuffer = NULL;
	}
}

void DoCommandShellMsg( void )
{
	LONG rc1/*, rc2*/;

	GetMsg( CSsp->sp_Msg.mn_ReplyPort );

	PacketUnterwegs = FALSE;

/*	rc2 = CSsp->sp_Pkt.dp_Res2;*/
	rc1 = CSsp->sp_Pkt.dp_Res1;

		/* EOF ? */

	if( rc1 == 0 )
	{
		CloseCommandShell();
	}
	else
	{
		struct FileHandle *FH;
		char *s = CSbuffer;

		while( *s==' ' || *s=='\t' ) s++;
			
		if( *s != '\n' )
			DoShellCommand( MyRexxHost, s, CSfh );

/*		CSbuffer[ rc1-1 ] = 0;*/

		Write( CSfh, "->", 2 );
		
		FH = BADDR( CSfh );
		
		CSsp->sp_Pkt.dp_Type = ACTION_READ;
		CSsp->sp_Pkt.dp_Arg1 = FH->fh_Arg1;
		CSsp->sp_Pkt.dp_Arg3 = 1024;	/* BUFFERSIZE */
					
		CSsp->sp_Pkt.dp_Arg2 = (long)CSbuffer;	/* Datenzeiger */
					
		SendStdPkt( CSsp, FH->fh_Type );
		PacketUnterwegs = TRUE;
	}
}

BOOL OpenCommandShell( void )
{
	struct FileHandle *FH;

	if( MyRexxHost->flags & ARB_HF_CMDSHELL )
		return( TRUE );

	if(( CSfh = Open(arexxcommandshellwindow, MODE_NEWFILE )))
	{
		FH = BADDR( CSfh );
	
		if( FH->fh_Type )
		{
			if(( CSbuffer = AllocMem( 1024, MEMF_ANY )))
			{
				if(( CSsp = CreateStdPkt() ))
				{
					MyRexxHost->flags |= ARB_HF_CMDSHELL;
	
					Write( CSfh, "->", 2 );
			
					CSsp->sp_Pkt.dp_Type = ACTION_READ;
					CSsp->sp_Pkt.dp_Arg1 = FH->fh_Arg1;
					CSsp->sp_Pkt.dp_Arg3 = 1024;	/* BUFFERSIZE */
						
					CSsp->sp_Pkt.dp_Arg2 = (long)CSbuffer;	/* Datenzeiger */
						
					SendStdPkt( CSsp, FH->fh_Type );
					PacketUnterwegs = TRUE;
			
					MyAddSignal( 1 << CSsp->sp_Msg.mn_ReplyPort->mp_SigBit, &DoCommandShellMsg );
					return( TRUE );
				}
		
				FreeVec( CSbuffer );
				CSbuffer = NULL;
			}
		}

		Close( CSfh );
	}

	return( FALSE );

#if 0
	char comline[512], *s;
	struct RexxMsg *rm;

	if( !fhin )
		return;
	
	host->flags |= ARB_HF_CMDSHELL;
	do
	{
		/* Prompt ausgeben */
		
		if( fhout && prompt )
			FPuts( fhout, prompt );
		
		/* Befehl einlesen und abarbeiten */
		
		if( s = FGets(fhin, comline, 512) )
		{
			while( *s==' ' || *s=='\t' ) s++;
			
			if( *s != '\n' )
				DoShellCommand( host, s, fhout );
		}
		else
		{
			host->flags &= ~ARB_HF_CMDSHELL;
		}
		
		/* Port des Hosts leeren (asynchrone Replies) */
		
		while(( rm = (struct RexxMsg *) GetMsg(host->port)) )
		{
			/* Reply? */
			if( rm->rm_Node.mn_Node.ln_Type == NT_REPLYMSG )
			{
				if( !rm->rm_Args[15] )
				{
					/* Reply zu einem SendRexxCommand()-Call */
					if( ARexxResultHook )
						ARexxResultHook( host, rm );
				}
				
				FreeRexxCommand( rm );
				--host->replies;
			}
			
			/* sonst Kommando -> Fehler */
			else
			{
				ReplyRexxCommand( rm, -20, (long)
					"CommandShell Port", NULL );
			}
		}
	}
	while( host->flags & ARB_HF_CMDSHELL );
#endif
}

