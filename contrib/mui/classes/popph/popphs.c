/*
** $Id$
**
** © 1999-2000 Marcin Orlowski <carlos@amiga.com.pl>
*/

#include <stdlib.h>	/* abs() */
#include <aros/debug.h>

#include "Popplaceholder_mcc.h"
#include "popph_private.h"

#include <mui/BetterString_mcc.h>

#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

// _DoCopy
int _DoCopy( Object *obj, struct PPHS_Data *data  )
{
    char *buf = (char *)XGET( obj, data->tag_contents );
    int  len = strlen( buf );

    return _WriteClip( buf, len, data->clip_device );
}

// _DoCut
int _DoCut( Object *obj, struct PPHS_Data *data )
{
    int  result = _DoCopy( obj, data );

    if( result )
	set( obj, data->tag_contents, "" );

    return result;
}

// DoCopy
IPTR PPHS__MUIM_DoCopy( struct IClass *cl, Object *obj, struct opSet *msg )
{
    struct PPHS_Data *data = INST_DATA(cl,obj);

    switch( data->stringtype )
    {
	case MUIV_Popph_StringType_Betterstring:
	    {
		char *buf = (char *)(XGET( obj, data->tag_contents ) + XGET( obj, data->tag_bufpos ));
		int  len = XGET( obj, MUIA_BetterString_SelectSize );

		// we do cut marked area only if there's any...
		if( len != 0 )
		{
		    // we need to do some calculations for reverse mark
		    if( len < 0 )
		    {
			buf -= abs(len);
		    }
		    _WriteClip( buf, abs(len), data->clip_device );
		}
		else
		{
		    _DoCopy( obj, data );
		}
	    }
	    break;
#ifdef HAVE_TEXTINPUT_MCC
	case MUIV_Popph_StringType_Textinput:
	    {
		DoMethod( obj, MUIM_Textinput_DoCopy );
	    }
	    break;
#endif
	    //       case MUIV_Popph_StringType_String:
	default:
	    {
		char *buf = (char *)XGET( obj, data->tag_contents );
		int  len = strlen( buf );

		_WriteClip( buf, len, data->clip_device );
	    }
	    break;
    }
    return 0;
}

// DoCut
IPTR PPHS__MUIM_DoCut( struct IClass *cl, Object *obj, struct opSet *msg )
{
    struct PPHS_Data *data = INST_DATA(cl,obj);

    DoMethod( obj, MUIM_Popph_DoCopy );

    switch( data->stringtype )
    {
	case MUIV_Popph_StringType_Textinput:
	    {
	    }
	    break;

	case MUIV_Popph_StringType_Betterstring:
	    {
		if( XGET( obj, MUIA_BetterString_SelectSize ) == 0 )
		    set( obj, data->tag_contents, "" );
		else
		    DoMethod( obj, MUIM_BetterString_ClearSelected );
	    }
	    break;

	    //       case MUIV_Popph_StringType_String:
	default:
	    {
		set( obj, data->tag_contents, "" );
	    }
	    break;
    }
    return 0;
}

// OM_NEW
IPTR PPHS__OM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct PPHS_Data *data;

    ULONG tag_contents;
    ULONG tag_bufpos;
    ULONG tag_maxlen;
    ULONG strmaxlen;

    strmaxlen = GetTagData( MUIA_Popph_StringMaxLen, 0, ((struct opSet *)msg)->ops_AttrList );

    switch( GetTagData( MUIA_Popph_StringType, MUIV_Popph_StringType_String,  ((struct opSet *)msg)->ops_AttrList ))
    {
	// MUIC_BetterString
	// MUIC_String
	case MUIV_Popph_StringType_String:
	case MUIV_Popph_StringType_Betterstring:
	    tag_contents = MUIA_String_Contents;
	    tag_bufpos   = MUIA_String_BufferPos;
	    tag_maxlen   = MUIA_String_MaxLen;
	    break;

#ifdef HAVE_TEXTINPUT_MCC
	// MUIC_Textinput
	case MUIV_Popph_StringType_Textinput:
	    tag_contents = MUIA_Textinput_Contents;
	    tag_bufpos   = MUIA_Textinput_CursorPos;
	    tag_maxlen   = MUIA_Textinput_MaxLen;
	    break;
#endif
    }

    obj = (Object *)DoSuperNewTags(cl,obj,NULL,
#ifdef HAVE_TEXTINPUT_MCC
	    MUIA_Textinput_DefaultPopup, FALSE,
#endif
	    tag_maxlen, strmaxlen,
	    TAG_MORE, msg->ops_AttrList);

    if( obj )
    {
	// init data
	data = INST_DATA(cl, obj);

	// some defaults
	data->dropobj     = NULL;
	data->separator   = '|';
	data->replacemode = FALSE;

	data->stringtype  = GetTagData( MUIA_Popph_StringType, MUIV_Popph_StringType_String, ((struct opSet *)msg)->ops_AttrList );

	data->contextmenu   = NULL;
	data->cmenu_enabled = TRUE;

	data->tag_contents = tag_contents;
	data->tag_bufpos   = tag_bufpos;
	data->tag_maxlen   = tag_maxlen;

	data->clip_device  = 0;        // default clipboard device

	// trick to set arguments
	msg->MethodID = OM_SET;
	DoMethodA(obj, (Msg)msg);
	msg->MethodID = OM_NEW;
    }
    return (IPTR)obj;
}

// OM_DISPOSE
IPTR PPHS__OM_DISPOSE(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct PPHS_Data *data = INST_DATA(cl,obj);

    D(bug(__NAME ": OM_DISPOSE\n"));

    if( data->contextmenu )
	MUI_DisposeObject( data->contextmenu );

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

// OM_SET
IPTR PPHS__OM_SET(struct IClass *cl, Object *obj, Msg msg)
{
    struct PPHS_Data *data = INST_DATA(cl,obj);
    const struct TagItem *tags;
    struct TagItem *tag;

    for( (tags=((struct opSet *)msg)->ops_AttrList); (tag=NextTagItem(&tags)); )
    {
	//       D(bug(__NAME ": OM_SET Tag: %lx\n", tag->ti_Tag));

	switch(tag->ti_Tag)
	{
	    case MUIA_Popph_Contents:
		set( obj, data->tag_contents, tag->ti_Data );
		break;

	    case MUIA_Popph_BufferPos:
		set( obj, data->tag_bufpos, tag->ti_Data );
		break;

	    case MUIA_Popph_StringKey:
		set( obj, MUIA_ControlChar, tag->ti_Data );
		break;

	    case MUIA_Popph_Separator:
		data->separator = tag->ti_Data;
		break;

	    case MUIA_Popph_DropObject:
		data->dropobj = (Object *)tag->ti_Data;
		break;

	    case MUIA_Popph_ReplaceMode:
		data->replacemode = tag->ti_Data;
		break;

	    case MUIA_Popph_ContextMenu:
		data->cmenu_enabled = tag->ti_Data;
		set(obj, MUIA_ContextMenu, data->cmenu_enabled);
		break;

	}
    }
    return DoSuperMethodA(cl, obj, msg);
}

// OM_GET
IPTR PPHS__OM_GET(struct IClass *cl, Object *obj, Msg msg)
{
    struct PPHS_Data *data = INST_DATA(cl,obj);
    IPTR *store = ((struct opGet *)msg)->opg_Storage;

    //    D(bug( __NAME ": OM_GET\n"));

    switch(((struct opGet *)msg)->opg_AttrID)
    {
	case MUIA_Popph_Contents:
	    *store = XGET( obj, data->tag_contents );
	    return TRUE;

	case MUIA_Popph_BufferPos:
	    *store = XGET( obj, data->tag_bufpos );
	    return TRUE;

	case MUIA_Popph_Separator:
	    *store = data->separator;
	    return TRUE;

	case MUIA_Popph_StringMaxLen:
	    *store = XGET( obj, data->tag_maxlen );
	    return TRUE;

	case MUIA_Popph_StringType:
	    *store = data->stringtype;
	    return TRUE;

	case MUIA_Popph_ContextMenu:
	    *store = data->cmenu_enabled;
	    return TRUE;

    }
    return DoSuperMethodA(cl, obj, msg);
}

// mDragQuery
IPTR PPHS__MUIM_DragQuery(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
    struct PPHS_Data *data = INST_DATA(cl,obj);

    /* refuse to drop on ourselves */
    if( msg->obj != data->dropobj )
	return(MUIV_DragQuery_Refuse);

    return MUIV_DragQuery_Accept;
}

// mDragDrop
IPTR PPHS__MUIM_DragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
    struct PPHS_Data *data = INST_DATA(cl,obj);

    char *list_entry;
    char *current_string;
    char *buffer;
    char key[ POPPH_MAX_KEY_LEN ];


    DoMethod(msg->obj, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &list_entry);

    if( list_entry )
    {
	int stringlen;
	int keylen;
	int pos;
	char *zero;


	zero = _strchr( list_entry, data->separator );

	if( zero )
	    keylen = _strlen( list_entry, data->separator );
	else
	    keylen = _strlen( list_entry, '\0' );


	CopyMem( list_entry, key, keylen );
	key[ keylen ] = 0;

	if( data->replacemode )
	{
	    set( obj, MUIA_Popph_Contents, key );
	}
	else
	{
	    get( obj, MUIA_Popph_BufferPos, &pos );
	    get( obj, MUIA_Popph_Contents, &current_string );
	    stringlen = _strlen( current_string, 0 );

	    if( ( buffer = AllocVec( keylen + stringlen + 5, MEMF_CLEAR ) ) )
	    {
		CopyMem( current_string, buffer, pos );
		CopyMem( key, buffer + pos, keylen );
		CopyMem( current_string + pos, buffer + pos + keylen, stringlen - pos );

		set( obj, MUIA_Popph_Contents, buffer );
		set( obj, MUIA_Popph_BufferPos, pos + keylen );

		FreeVec( buffer );
	    }
	}
    }
    return 0;
}

// Dispatcher
BOOPSI_DISPATCHER(IPTR, PPHS_Dispatcher, cl, obj, msg)
{
    switch( msg->MethodID )
    {
	case OM_NEW            : return PPHS__OM_NEW         (cl, obj, (APTR)msg);
	case OM_DISPOSE        : return PPHS__OM_DISPOSE     (cl, obj, (APTR)msg);
	case OM_SET            : return PPHS__OM_SET         (cl, obj, (APTR)msg);
	case OM_GET            : return PPHS__OM_GET         (cl, obj, (APTR)msg);
	case MUIM_Popph_DoCut  : return PPHS__MUIM_DoCut     (cl, obj, (APTR)msg);
	case MUIM_Popph_DoCopy : return PPHS__MUIM_DoCut     (cl, obj, (APTR)msg);
	case MUIM_DragQuery    : return PPHS__MUIM_DragQuery (cl, obj, (APTR)msg);
	case MUIM_DragDrop     : return PPHS__MUIM_DragDrop  (cl, obj, (APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
BOOPSI_DISPATCHER_END
