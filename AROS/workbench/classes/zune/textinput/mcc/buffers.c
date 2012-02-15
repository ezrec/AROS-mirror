/*
** $Id$
*/

#include "mystring.h"
#include "structs.h"
#include "misc.h"

int dobuffersize( struct Data *data, int len )
{
	char *newbuffer;
	int offs_cursor, offs_rmarkstart = 0, offs_rmarkend = 0;

	if( len < data->maxlen )
		return( TRUE );
	if( !data->autoexpand )
		return( FALSE );

	newbuffer = AllocVecPooled( data->pool, len + 513 );
	if( !newbuffer )
		return( FALSE );

	memcpy( newbuffer, data->buffer, data->maxlen + 1 );

	offs_cursor = data->cursorp - data->buffer;

	if( data->markmode )
	{
		offs_rmarkstart = data->rmarkstart - data->buffer;
		offs_rmarkend = data->rmarkend - data->buffer;
	}

	data->maxlen = len + 512;
	newbuffer[ data->maxlen ] = 0;
	FreeVecPooled( data->pool, data->buffer );
	data->buffer = newbuffer;

	data->cursorp = data->buffer + offs_cursor;

	if( data->markmode )
	{
		data->rmarkstart = data->buffer + offs_rmarkstart;
		data->rmarkend = data->buffer + offs_rmarkend;
		data->markstart = STRMIN( data->rmarkstart, data->rmarkend );
		data->markend = STRMAX( data->rmarkstart, data->rmarkend );
	}

	return( TRUE );
}

#ifdef DEBUGUNDO
void dumpaction( struct actionnode *action )
{
#define PA(n,m) kprintf("%s: %ld\n",n,action->m);
#define PT(n,m) if( ( action->type & ACTION_MASK ) == ACTION_ ## m ) kprintf("(ACTION_%s)\n",n)
#define PF(n,m) if( action->type & ACTIONF_ ## m ) kprintf("(ACTIONF_%s)\n",n)
	kprintf("---action---\n");
	PA("type",type&ACTION_MASK);
	PT("INSERT",INSERT);
	PT("DELETE",DELETE);
	PT("REPLACE",REPLACE);
	PT("SURROUND",SURROUND);
	PT("UNSURROUND",UNSURROUND);
	PA("flags",type&ACTIONF_MASK);
	PF("UNCONDITIONAL",UNCONDITIONAL);
	PF("STRIPCR",STRIPCR);
	PF("REDO",REDO);
	PF("NOFLUSH",NOFLUSH);
	PA("pos",pos);
	PA("len",len);
	PA("datalen",datalen);
	if( action->datalen && ( action->type & ACTION_MASK ) != ACTION_SURROUND )
	{
		int i;
		kprintf("data: ");
		for( i = 0; i < action->datalen; i++ )
			kprintf( "%lc",action->data[ i ] );
		kprintf("\n");
	}
}
#else
#define dumpaction(x)
#endif

void freeaction( struct Data *data, struct actionnode *action )
{
	int len;
	len = sizeof( struct actionnode );
	if( ( action->type & ACTION_MASK ) != ACTION_SURROUND )
		len += action->datalen;
	if( action->type & ACTIONF_REDO )
	{
		data->redosize -= len;
		data->redocount--;
	}
	else
	{
		data->undosize -= len;
		data->undocount--;
	}
	FreeVecPooled( data->pool, action );
}

/*
This routine has been added in order to provide a consistent method of altering
the buffer. This will allow, for example, undo/redo facilities in the future.
Note:
ACTION_DELETE is just ACTION_REPLACE without a replacement string
ACTION_INSERT is just ACTION_REPLACE without replacing anything
Right now they exist primarily for clarity.
*/

char *doaction( struct Data *data, ULONG typewithflags, char *pos, int len, char *str, int newlen )
{
	char *rcp = NULL;
	int undoable = FALSE;
	int flags = typewithflags & ACTIONF_MASK;
	int type = typewithflags & ACTION_MASK;
	int notall = FALSE;
	struct actionnode *action = NULL;
	int actionlen = 0;
	int reallen = len, offs_pos = pos - data->buffer;
	if( reallen == -1 )
		reallen = strlen( pos );
	else if( reallen && !pos[ reallen - 1 ] )
	{
		if( --reallen < 0 )
			return( FALSE );
	}
	if( type == ACTION_DELETE && !reallen )
		return( FALSE );
	if( !( flags & ACTIONF_NOFLUSH ) )
	{
		struct actionnode *n;
		while((n = REMHEAD( &data->redolist )))
			freeaction( data, n );
	}
	if( !data->noinput )
	{
		int datalen = ( type == ACTION_DELETE || type == ACTION_REPLACE || type == ACTION_UNSURROUND ) ? reallen : 0;
		int actnewlen = newlen;
		if( str && actnewlen == -1 )
			actnewlen = strlen( str );
		actionlen = sizeof( struct actionnode ) + datalen;
		if( ( actionlen + data->undosize + data->redosize <= data->maxundobytes ) || ( data->undocount + data->redocount + 1 <= data->maxundolevels ) )
		{
			if((action = AllocVecPooled( data->pool, actionlen )))
			{
				action->type = typewithflags;
				action->pos = offs_pos;
				action->len = actnewlen;
				action->datalen = datalen;
				if( datalen )
					memcpy( action->data, pos, datalen );
				if( type == ACTION_SURROUND )
					action->datalen = reallen;
				dumpaction( action );
				undoable = TRUE;
			}
		}
	}
	if( type == ACTION_INSERT )
	{
		if( newlen == -1 )
			newlen = strlen( str );
		if( dobuffersize( data, strlen( data->buffer ) + newlen ) )
		{
			memmove( data->buffer + offs_pos + newlen, data->buffer + offs_pos, strlen( data->buffer + offs_pos ) + 1 );
			memcpy( data->buffer + offs_pos, str, newlen );
			rcp = data->buffer + offs_pos + newlen;
		}
	}
	else if( type == ACTION_DELETE )
	{
		if( len == -1 || !pos[ len - 1 ] )
			*pos = 0;
		else
			strcpy( pos, pos + reallen );
		rcp = pos;
	}
	else if( type == ACTION_REPLACE )
	{
		if( newlen == -1 )
			newlen = strlen( str );
		if( len == -1 )
		{
			// Replace whole contents, offs_pos is assumed to be 0 atm
			char *s;
			notall = !dobuffersize( data, offs_pos + 1 + newlen );
			if( notall )
			{
				if( flags & ACTIONF_UNCONDITIONAL )
					newlen = min( newlen, data->maxlen );
				else
					newlen = 0;
			}
			if( newlen )
			{
				stccpy( data->buffer + offs_pos, str, newlen + 1 );
				if( flags & ACTIONF_STRIPCR )
				{
					for( s = data->buffer + offs_pos; *s; )
					{
						if( *s == '\r' )
						{
							strcpy( s, s + 1 );
							notall = TRUE;
							newlen--;
						}
						else
							s++;
					}
				}
				rcp = data->buffer + offs_pos + newlen;
			}
		}
		else
		{
			if( dobuffersize( data, offs_pos + 1 + reallen + newlen ) )
			{
				memmove( data->buffer + offs_pos + newlen, data->buffer + offs_pos + reallen, strlen( data->buffer + offs_pos + reallen ) + 1 );
				memcpy( data->buffer + offs_pos, str, newlen );
				rcp = data->buffer + offs_pos + newlen;
			}
		}
	}
	else if( type == ACTION_SURROUND )
	{
		if( newlen == -1  )
			newlen = strlen( str );
		if( dobuffersize( data, offs_pos + 1 + newlen * 2 ) )
		{
			pos = data->buffer + offs_pos;
			memmove( pos + reallen + newlen * 2, pos + reallen, strlen( pos + reallen ) + 1 );
			memmove( pos + newlen, pos, reallen );
			memcpy( pos, str, newlen );
			memcpy( pos + reallen + newlen, str, newlen );
			rcp = pos + reallen + newlen * 2;
		}
	}
	else if( type == ACTION_UNSURROUND )
	{
		memcpy( pos, pos + reallen, newlen );
		memcpy( pos + newlen, pos + newlen + reallen * 2, strlen( pos + newlen + reallen * 2 ) + 1 );
		rcp = pos + reallen;
	}
	if( action )
	{
		if( !rcp )
			FreeVecPooled( data->pool, action );
		else
		{
			if( notall )
				action->len = data->maxlen;
			if( flags & ACTIONF_REDO )
			{
				ADDHEAD( &data->redolist, action );
				data->redosize += actionlen;
				data->redocount++;
			}
			else
			{
				ADDHEAD( &data->undolist, action );
				data->undosize += actionlen;
				data->undocount++;
			}
		}
	}
	return( rcp );
}

char *undoaction( struct Data *data, struct actionnode *action )
{
	int type = action->type & ACTION_MASK;
	int flags = action->type & ACTIONF_MASK;
	char *pos = data->buffer + action->pos, *datastr = action->data, *rcp;
	int poslen = action->len, datalen = action->datalen;
	switch( type )
	{
		case ACTION_INSERT:
			type = ACTION_DELETE;
			break;
		case ACTION_DELETE:
			type = ACTION_INSERT;
			break;
		case ACTION_REPLACE:
			break;
		case ACTION_SURROUND:
			type = ACTION_UNSURROUND;
			break;
		case ACTION_UNSURROUND:
			type = ACTION_SURROUND;
			break;
		default:
			return( 0 );
	}
	if((rcp = doaction( data, type | ( ( flags & ACTIONF_REDO ) ^ ACTIONF_REDO ) | ACTIONF_NOFLUSH, pos, poslen, datastr, datalen )))
	{
		REMOVE( action );
		freeaction( data, action );
		return( rcp );
	}
	return( FALSE );

}
