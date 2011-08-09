/*
** $Id$
**
** © 1999-2000 Marcin Orlowski <carlos@amiga.com.pl>
*/

#include <aros/debug.h>

#include "Popplaceholder_mcc.h"
#include "popph_private.h"

#include <mui/BetterString_mcc.h>
#include <libraries/asl.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#define list_format_single "P=\033l"
#define list_format_multi  "P=\033r BAR,"

// HOOK: WindowHook
AROS_UFH3(void, WindowFunc,
    AROS_UFHA(struct Hook *, hook     , A0),
    AROS_UFHA(Object *     , popobject, A2),
    AROS_UFHA(Object *     , window   , A1))
{
    AROS_USERFUNC_INIT

    struct Popplaceholder_Data *data = hook->h_Data;
    set( window, MUIA_Window_ActiveObject, data->lv );

    AROS_USERFUNC_EXIT
}

// HOOK: ObjStrHook
AROS_UFH3(void *, ObjStrFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, list, A2),
    AROS_UFHA(Object *, str, A1))
{
    AROS_USERFUNC_INIT

    struct Popplaceholder_Data *data = hook->h_Data;

    char *list_entry;
    char *current_string;
    char *buffer;
    char key[ POPPH_MAX_KEY_LEN ];

    if( !data->str )
	return 0;

    DoMethod(list, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &list_entry);

    if( list_entry )
    {
	char *zero;
	int keylen;
	int stringlen;
	int pos;


	zero = _strchr( list_entry, data->separator );

	if( zero )
	    keylen = _strlen( list_entry, data->separator );
	else
	    keylen = _strlen( list_entry, '\0' );


	D(bug( "ObjStrFunc: KeyLen: %ld\n", keylen ));

	keylen = (keylen > POPPH_MAX_KEY_LEN) ? (POPPH_MAX_KEY_LEN - 1) : keylen ;

	CopyMem( list_entry, key, keylen );
	key[ keylen ] = 0;

	if( data->replacemode )
	{
	    set( data->str, MUIA_Popph_Contents, key );
	}
	else
	{
	    get( data->str, MUIA_Popph_BufferPos, &pos );
	    get( data->str, MUIA_Popph_Contents, &current_string );
	    stringlen = _strlen( current_string, 0 );

	    D(bug( "ObjStrFunc: BufferPos: %ld\n", pos ));
	    D(bug( "ObjStrFunc: AllocVec( %ld+%ld+5 bytes )\n", keylen, stringlen ));

	    if( ( buffer = AllocVec( keylen + stringlen + 5, MEMF_CLEAR ) ) )
	    {
		CopyMem( current_string, buffer, pos );
		CopyMem( key, buffer + pos, keylen );
		CopyMem( current_string + pos, buffer + pos + keylen, stringlen - pos );

		set( data->str, MUIA_Popph_Contents, buffer );
		set( data->str, MUIA_Popph_BufferPos, pos + keylen );
		FreeVec( buffer );
	    }
	    else
	    {
		DisplayBeep(0);
	    }
	}
    }

    return 0;

    AROS_USERFUNC_EXIT
}

// HOOK: DisplayHook
AROS_UFH3(void *, DisplayFunc,
    AROS_UFHA(struct Hook *, hook , A0),
    AROS_UFHA(char **      , array, A2),
    AROS_UFHA(char *       , node , A1))
{
    AROS_USERFUNC_INIT

    static char key[ POPPH_MAX_KEY_LEN ];
    static char *info;

    struct Popplaceholder_Data *data = hook->h_Data;


    if( node )
    {
	char *zero = _strchr(node, data->separator);

	if( zero )
	{
	    CopyMem(node, key, zero-node);
	    key[zero-node] = 0;
	    info = zero+1;

	    // D(bug("'%s', '%s'\n", key, info));
	    *array++ = key;
	    *array++ = info;
	}
	else
	{
	    *array++ = node;
	    *array++ = NULL;
	}
    }
    else
    {
	// D(bug( "Hook: '%s'  '%s'\n", data->title1, data->title2 ));
	*array++ = data->title1;
	*array++ = data->title2;
    }

    return 0;

    AROS_USERFUNC_EXIT
}

/// _OpenAsl
IPTR Popph__MUIM_OpenAsl(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct Popplaceholder_Data *data = INST_DATA(cl,obj);

    Object *app = (Object *)XGET( obj, MUIA_ApplicationObject );
    struct Window *win;
    APTR   req = data->asl_req;
    IPTR   res=0;

    char buf[ POPPH_MAX_STRING_LEN * 2 ];
    char *path;
    char *path_part;


    set( app, MUIA_Application_Sleep, TRUE );
    get( obj, MUIA_Window, &win );      

    path = (char *)XGET( obj, MUIA_Popph_Contents );
    path_part = PathPart( path );
    CopyMem( path, buf, path_part - path );
    buf[ path_part - path ] = 0;


    if(MUI_AslRequestTags( req,
		ASLFR_Window         , win,
		ASLFR_InitialFile    , FilePart( path ),
		ASLFR_InitialDrawer  , buf,
		TAG_DONE))
    {
	switch( data->asl_type )
	{
	    case ASL_FileRequest:
		{
		    struct FileRequester *freq = req;
		    {
			res = (IPTR)buf;
			stccpy(buf , freq->fr_Drawer, sizeof( buf ));
			if (freq->fr_File)
			{
			    AddPart(buf, freq->fr_File  , sizeof( buf ));
			}

			set( obj, MUIA_Popph_Contents, buf );
		    }
		}
		break;
	}
    }

    set( app, MUIA_Application_Sleep, FALSE );

    return res;
}

// OM_NEW
IPTR Popph__OM_NEW(struct IClass *cl, Object *obj, Msg msg)
{
    struct Popplaceholder_Data *data;

    Object *group, *popobj, *popbutton, *string;
    Object *lv, *list;
    int    strmaxlen;

//    ULONG  tag_contents = MUIA_String_Contents;
//    ULONG  tag_bufpos   = MUIA_String_BufferPos;
//    ULONG  tag_maxlen   = MUIA_String_MaxLen;
    ULONG  tag_attached = MUIA_String_AttachedList;

    struct MUI_CustomClass *CL_String = NULL;
    char   *string_class = MUIC_String;
    char   string_type   = MUIV_Popph_StringType_String;
    char   use_our_string_class = FALSE;

    Object *popaslbutton = NULL;
    ULONG  use_asl = FALSE;
    APTR   asl_req = NULL;
    ULONG  asl_type;

    ULONG  avoid;


    D(bug(__NAME ": OM_NEW\n"));


    // let's discover some initial settings...

    if(((struct opSet *)msg)->ops_AttrList)
    {
	string    = (Object *)GetTagData( MUIA_Popph_StringObject, (IPTR)NULL, ((struct opSet *)msg)->ops_AttrList );
	avoid     = GetTagData( MUIA_Popph_Avoid , 0    , ((struct opSet *)msg)->ops_AttrList );
	use_asl   = GetTagData( MUIA_Popph_PopAsl, FALSE, ((struct opSet *)msg)->ops_AttrList );


	if( ! (strmaxlen = GetTagData( MUIA_String_MaxLen, 0,  ((struct opSet *)msg)->ops_AttrList )) )
	    strmaxlen = GetTagData( MUIA_Popph_StringMaxLen, 0,  ((struct opSet *)msg)->ops_AttrList );

	if( strmaxlen == 0 )
	    strmaxlen = POPPH_MAX_STRING_LEN + (POPPH_MAX_STRING_LEN * use_asl );

    }

    D(bug(__NAME ": CustomStringObj: %lx (maxlen: %ld)\n", string, strmaxlen ));

    // let's try which string objects we can use...

    //D(bug(__NAME ": _Avoid_: %lx\n", avoid, ));

#ifdef HAVE_TEXTINPUT_MCC
    if( (!string) && (!(avoid & MUIV_Popph_Avoid_Textinput )) )
    {
	if( (string = TextinputObject, End ) )
	{
	    string_class = MUIC_Textinput;
	    tag_contents = MUIA_Textinput_Contents;
	    tag_bufpos   = MUIA_Textinput_CursorPos;
	    tag_maxlen   = MUIA_Textinput_MaxLen;
	    tag_attached = MUIA_Textinput_AttachedList;
	    string_type  = MUIV_Popph_StringType_Textinput;

	    use_our_string_class = TRUE;
	}
    }
#endif

    if( ( (!string) && (!(avoid & MUIV_Popph_Avoid_Betterstring)) ) )
    {
	if( (string = BetterStringObject, End) )
	{
	    string_class = MUIC_BetterString;
	    string_type  = MUIV_Popph_StringType_Betterstring;

	    use_our_string_class = TRUE;
	}
    }

    if( !string )
    {
	string = StringObject, End;

	string_type  = MUIV_Popph_StringType_String;
	use_our_string_class = TRUE;
    }

    D(bug(__NAME ": String: %08lx ('%s'), OurClass: %s\n", string, string_class, use_our_string_class ? "Yes" : "No"));

    if( use_our_string_class && string )
    {
	if( string )
	{
	    MUI_DisposeObject( string );
	}
	else
	{
	    D(bug(__NAME ": No string object class available!?\n"));
	    return (IPTR)NULL;
	}
    }

    // let's create string subclass...
    if( use_our_string_class )
    {
	if( !(CL_String = MUI_CreateCustomClass(NULL, string_class, NULL, sizeof(struct PPHS_Data), PPHS_Dispatcher)) )
	{
	    D(bug(__NAME ": Can't create '%s' custom class\n", string_class ));
	    return (IPTR)NULL;
	}

	D(bug(__NAME ": CustomClass created: %lx (%s)\n", CL_String, string_class));
    }

    D(bug(__NAME ": UseASL: %s\n", use_asl ? "Yes" : "No" ));

    if( use_asl )
    {
	ULONG asl_drawers;

	asl_type = GetTagData( MUIA_Popph_AslType, ASL_FileRequest, ((struct opSet *)msg)->ops_AttrList );

	asl_drawers = GetTagData( ASLFR_DrawersOnly, FALSE, ((struct opSet *)msg)->ops_AttrList );
	popaslbutton = MUI_MakeObject( MUIO_PopButton, asl_drawers ? MUII_PopDrawer : MUII_PopFile );

	if( !popaslbutton )
	{
	    D(bug(__NAME ": Cant create popaslbutton!\n"));
	    goto cleanup;
	}

	if( !(asl_req = MUI_AllocAslRequestTags( asl_type, TAG_MORE, ((struct opSet *)msg)->ops_AttrList )))
	{
	    D(bug(__NAME ": Cant allocate asl request!\n"));
	    goto cleanup;
	}
	D(bug(__NAME ": AslReq: %lx\n", asl_req ));
    }

    // let's create Popph object...

    if( use_our_string_class )
    {
	if( !(string = NewObject( CL_String->mcc_Class, NULL, StringFrame,
			MUIA_Popph_StringMaxLen, strmaxlen,
			MUIA_CycleChain, TRUE,
			MUIA_Dropable  , TRUE,
			MUIA_String_AdvanceOnCR, TRUE,
			MUIA_Popph_StringType, string_type,
			TAG_MORE, ((struct opSet *)msg)->ops_AttrList )) )
	    // TAG_DONE )) )
	{
	    D(bug(__NAME ": Cant create string object!\n"));
	    goto cleanup;
	}
	D(bug(__NAME ": CustomStringObject created: %lx\n", string ));
    }

    obj = (Object *)DoSuperNewTags(cl, obj, NULL,
	MUIA_Group_Horiz, TRUE,
	Child, group = HGroup,
	    MUIA_Group_Spacing, 0,
	    Child, popobj = PopobjectObject,
		MUIA_Popstring_String, string,
		MUIA_Popstring_Button, popbutton = PopButton(MUII_PopUp),
		MUIA_Popobject_Object, lv = ListviewObject,
		    MUIA_CycleChain, TRUE,
		    MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
		    MUIA_Listview_List, list = ListObject,
			InputListFrame,
			MUIA_List_Format, list_format_multi,
			TAG_MORE, ((struct opSet *)msg)->ops_AttrList,
		    End,
		    TAG_MORE, ((struct opSet *)msg)->ops_AttrList,
		End, 
	    End, 
	End,
    TAG_DONE);

    if( !obj )
    {
	D(bug(__NAME ": Class object creation failed!\n"));
	goto cleanup;
    }

    if( use_asl )
	DoMethod( group, OM_ADDMEMBER, popaslbutton );

    /*** init data ***/
    data = INST_DATA(cl, obj);

    data->CL_String    = CL_String;

    data->lv           = lv;
    data->str          = string;

    D(bug("%lx\n", string ));

    data->popbutton    = popbutton;
    data->popaslbutton = popaslbutton;
    data->popobj       = popobj;

    data->separator    = '|';
    data->copyentries  = FALSE;
    data->replacemode  = FALSE;
    data->singlecolumn = FALSE;

    data->asl_active   = FALSE;
    data->asl_req      = asl_req;
    data->asl_type     = asl_type;

    data->title1[0]    = 0;
    data->title2[0]    = 0;

    // only this object is allowed to drop anything...

    set( string, MUIA_Popph_DropObject, list );

    set( string, tag_attached, lv );

    //    set(popbutton, MUIA_CycleChain, TRUE);

    data->DisplayHook.h_Data  = (APTR)data;
    data->DisplayHook.h_Entry = (VOID *)DisplayFunc;
    set(lv, MUIA_List_DisplayHook, &data->DisplayHook );

    data->ObjStrHook.h_Data  = (APTR)data;
    data->ObjStrHook.h_Entry = (VOID *)ObjStrFunc;
    set(popobj, MUIA_Popobject_ObjStrHook, &data->ObjStrHook );

    data->WindowHook.h_Data  = (APTR)data;
    data->WindowHook.h_Entry = (VOID *)WindowFunc;
    set(popobj, MUIA_Popobject_WindowHook, &data->WindowHook );

    DoMethod(lv, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
	    popobj, 2, MUIM_Popstring_Close, TRUE);

    if( use_asl )
    {
	DoMethod( popaslbutton, MUIM_Notify, MUIA_Pressed, FALSE,
		obj, 1, MUIM_Popph_OpenAsl );
    }

    /*** trick to set arguments ***/
    if(((struct opSet *)msg)->ops_AttrList) 
    {
	msg->MethodID = OM_SET;
	DoMethodA(obj, (Msg)msg);
	msg->MethodID = OM_NEW;
    }

    return (IPTR)obj;

cleanup:

    D(bug(__NAME ": OM_NEW: cleanup called\n"));

    if( asl_req )
	MUI_FreeAslRequest( asl_req );

    if( string )
	MUI_DisposeObject( string );
    if( CL_String )
	MUI_DeleteCustomClass( CL_String );

    if( popaslbutton )
	MUI_DisposeObject( popaslbutton );

    return (IPTR)NULL;
}

// OM_DISPOSE
IPTR Popph__OM_DISPOSE(struct IClass *cl, Object *obj, struct opSet *msg)
{
    APTR cl_string;
    struct Popplaceholder_Data *data = INST_DATA(cl,obj);

    D(bug(__NAME ": OM_DISPOSE\n"));
    D(bug(__NAME ": data->asl_req: %lx\n", data->asl_req ));

    if( data->asl_req )
    {
	MUI_FreeAslRequest( data->asl_req );
	data->asl_req = NULL;
    }

    //    D(bug(__NAME ":  >> Dispose (SuperClass)\n"));

    cl_string = data->CL_String;

    DoSuperMethodA(cl, obj, (Msg)msg);

    D(bug(__NAME ": cl_string: %lx\n", cl_string ));
    if( cl_string )
    {
	MUI_DeleteCustomClass( cl_string );
    }
    return 0;
}

/// OM_SET
IPTR Popph__OM_SET(struct IClass *cl, Object *obj, Msg msg)
{
    struct Popplaceholder_Data *data = INST_DATA(cl,obj);
    const struct TagItem *tags;
    struct TagItem *tag;

    for( ( tags=((struct opSet *)msg)->ops_AttrList ) ; ( tag=NextTagItem(&tags) ) ; )
    {
	switch(tag->ti_Tag)
	{
	    //   case MUIA_String_...:
	    case MUIA_String_Accept:
	    case MUIA_String_Acknowledge:
	    case MUIA_String_AdvanceOnCR:
	    case MUIA_String_AttachedList:
	    case MUIA_String_BufferPos:
	    case MUIA_String_DisplayPos:
	    case MUIA_String_EditHook:
	    case MUIA_String_Format:
	    case MUIA_String_Integer:
	    case MUIA_String_LonelyEditHook:
	    case MUIA_String_MaxLen:
	    case MUIA_String_Reject:
	    case MUIA_String_Secret:

#if HAVE_TEXTINPUT_MCC
	    //   case MUIA_Textinput_...:
	    case MUIA_Textinput_Multiline:
	    case MUIA_Textinput_MaxLen:
	    case MUIA_Textinput_MaxLines:
	    case MUIA_Textinput_AutoExpand:
	    case MUIA_Textinput_LeftOffset:
	    case MUIA_Textinput_TopOffset:
	    case MUIA_Textinput_TSCO:
	    case MUIA_Textinput_Blinkrate:
	    case MUIA_Textinput_Cursorstyle:
	    case MUIA_Textinput_AdvanceOnCR:
	    case MUIA_Textinput_TmpExtension:
	    case MUIA_Textinput_Quiet:
	    case MUIA_Textinput_Acknowledge:
	    case MUIA_Textinput_Integer:
	    case MUIA_Textinput_MinVersion:
	    case MUIA_Textinput_DefKeys:
	    case MUIA_Textinput_DefaultPopup:
	    case MUIA_Textinput_WordWrap:
	    case MUIA_Textinput_IsNumeric:
	    case MUIA_Textinput_MinVal:
	    case MUIA_Textinput_MaxVal:
	    case MUIA_Textinput_AcceptChars:
	    case MUIA_Textinput_RejectChars:
	    case MUIA_Textinput_Changed:
	    case MUIA_Textinput_AttachedList:
	    case MUIA_Textinput_RemainActive:
	    case MUIA_Textinput_CursorPos:
	    case MUIA_Textinput_Secret:
	    case MUIA_Textinput_Lines:
	    case MUIA_Textinput_Editable:
	    case MUIA_Textinputscroll_UseWinBorder:
	    case MUIA_Textinput_IsOld:
	    case MUIA_Textinput_MarkStart:
	    case MUIA_Textinput_MarkEnd:
	    case MUIA_Textinputscroll_VertScrollerOnly:
	    case MUIA_Textinput_NoInput:
	    case MUIA_Textinput_SetMin:
	    case MUIA_Textinput_SetMax:
	    case MUIA_Textinput_SetVMax:
	    case MUIA_Textinput_Styles:
	    case MUIA_Textinput_PreParse:
	    case MUIA_Textinput_Format:
	    case MUIA_Textinput_SetVMin:
	    case MUIA_Textinput_HandleURLHook:
	    case MUIA_Textinput_Tabs:
	    case MUIA_Textinput_TabLen:
	    case MUIA_Textinput_Bookmark1:
	    case MUIA_Textinput_Bookmark2:
	    case MUIA_Textinput_Bookmark3:
	    case MUIA_Textinput_CursorSize:
	    case MUIA_Textinput_TopLine:
#endif

	    //   case MUIA_BetterString_...:
	    case MUIA_BetterString_SelectSize:
	    case MUIA_BetterString_StayActive:
	    case MUIA_BetterString_Columns:

	    case MUIA_Popph_ContextMenu:
		set( data->str, tag->ti_Tag, tag->ti_Data );
		break;

	    //   case MUIA_List_...:
	    case MUIA_List_Active:
	    case MUIA_List_AdjustHeight:
	    case MUIA_List_AdjustWidth:
	    case MUIA_List_AutoVisible:
	    case MUIA_List_CompareHook:
	    case MUIA_List_ConstructHook:
	    case MUIA_List_DestructHook:
	    case MUIA_List_DisplayHook:
	    case MUIA_List_DragSortable:
	    case MUIA_List_DropMark:
	    case MUIA_List_Entries:
	    case MUIA_List_First:
	    case MUIA_List_Format:
	    case MUIA_List_InsertPosition:
	    case MUIA_List_MinLineHeight:
	    case MUIA_List_MultiTestHook:
	    case MUIA_List_Pool:
	    case MUIA_List_PoolPuddleSize:
	    case MUIA_List_PoolThreshSize:
	    case MUIA_List_Quiet:
	    case MUIA_List_ShowDropMarks:
	    case MUIA_List_SourceArray:
	    case MUIA_List_Title:
	    case MUIA_List_Visible:

	    //   case MUIA_Listview_...:
	    case MUIA_Listview_ClickColumn:
	    case MUIA_Listview_DefClickColumn:
	    case MUIA_Listview_DoubleClick:
	    case MUIA_Listview_DragType:
	    case MUIA_Listview_Input:
	    case MUIA_Listview_List:
	    case MUIA_Listview_MultiSelect:
	    case MUIA_Listview_ScrollerPos:
	    case MUIA_Listview_SelectChange:
		set( data->lv, tag->ti_Tag, tag->ti_Data );
		break;

	    case MUIA_Popph_Array:
		DoMethod( data->lv, MUIM_List_Insert, tag->ti_Data, -1, MUIV_List_Insert_Bottom );
		break;

#if HAVE_TEXTINPUT_MCC
	    case MUIA_Textinput_Contents:
#endif
	    case MUIA_String_Contents:
	    case MUIA_Popph_Contents:
		set( data->str, MUIA_Popph_Contents, tag->ti_Data );
		break;

	    case MUIA_Popph_PopbuttonKey:
		set( data->popbutton, MUIA_ControlChar, tag->ti_Data );
		break;

	    case MUIA_Popph_StringKey:
		set( data->str, MUIA_ControlChar, tag->ti_Data );
		break;

	    case MUIA_Popph_Separator:
		data->separator = tag->ti_Data;
		break;

	    case MUIA_Popph_CopyEntries:
		data->copyentries = tag->ti_Data;

		if( tag->ti_Data )
		{
		    // TRUE

		    set( data->lv, MUIA_List_ConstructHook, MUIV_List_ConstructHook_String );
		    set( data->lv, MUIA_List_DestructHook , MUIV_List_DestructHook_String );
		}
		else
		{
		    set( data->lv, MUIA_List_ConstructHook, NULL );
		    set( data->lv, MUIA_List_DestructHook , NULL );
		}
		break;

	    case MUIA_Popph_ReplaceMode:
		data->replacemode = tag->ti_Data;
		set( data->str, MUIA_Popph_ReplaceMode, tag->ti_Data );
		break;

	    case MUIA_Popph_PopCycleChain:
		{
		    set( data->popbutton, MUIA_CycleChain, tag->ti_Data );
		    if( data->popaslbutton )
			set( data->popaslbutton, MUIA_CycleChain, tag->ti_Data );
		}
		break;

	    case MUIA_Popph_Title:
		{
		    set( data->lv, MUIA_List_Title, (tag->ti_Data > 0) ? TRUE : FALSE );

		    data->title1[0] = 0;
		    data->title2[0] = 0;

		    if( tag->ti_Data )
		    {
			int keylen;
			char *zero;

			zero = _strchr( (char *)tag->ti_Data, data->separator );

			D(bug( "TitleSrc: '%s'\n", (char *)tag->ti_Data ));


			// is separator found
			if( zero )
			    keylen = _strlen( (char *)tag->ti_Data, data->separator );
			else
			    keylen = _strlen( (char *)tag->ti_Data, '\0' );

			keylen = (keylen > POPPH_MAX_KEY_LEN) ? (POPPH_MAX_KEY_LEN - 1) : keylen ;

			CopyMem( (APTR)tag->ti_Data, data->title1, keylen );
			data->title1[ keylen ] = 0;
			D(bug( "Title1: '%s' (%ld)\n", data->title1, _strlen( data->title1, '\0' ) ));

			if( zero )
			{
			    zero++;

			    keylen = _strlen( zero, '\0' );
			    keylen = (keylen > POPPH_MAX_STRING_LEN) ? (POPPH_MAX_STRING_LEN - 1) : keylen ;

			    CopyMem( zero, data->title2, keylen );
			    data->title2[ keylen ] = 0;
			    D(bug( "Title2: '%s', (%ld)\n", data->title2, keylen ));
			}
		    }
		}
		break;

	    case MUIA_Popph_SingleColumn:
		{
		    data->singlecolumn = tag->ti_Data;
		    set( data->lv, MUIA_List_Format, tag->ti_Data ? list_format_single : list_format_multi );
		}
		break;

	}
    }
    return DoSuperMethodA(cl, obj, msg);
}

/// OM_GET
IPTR Popph__OM_GET(struct IClass *cl, Object *obj, Msg msg)
{
    struct Popplaceholder_Data *data = INST_DATA(cl,obj);
    IPTR   *store = ((struct opGet *)msg)->opg_Storage;

    //    D(bug(__NAME ": GET\n"));

    switch(((struct opGet *)msg)->opg_AttrID)
    {
	//   case MUIA_String_...:
	case MUIA_String_Accept:
	case MUIA_String_Acknowledge:
	case MUIA_String_AdvanceOnCR:
	case MUIA_String_AttachedList:
	case MUIA_String_BufferPos:
	case MUIA_String_DisplayPos:
	case MUIA_String_EditHook:
	case MUIA_String_Format:
	case MUIA_String_Integer:
	case MUIA_String_LonelyEditHook:
	case MUIA_String_MaxLen:
	case MUIA_String_Reject:
	case MUIA_String_Secret:

#ifdef HAVE_TEXTINPUT_MCC
	//   case MUIA_Textinput_...:
	case MUIA_Textinput_Multiline:
	case MUIA_Textinput_MaxLen:
	case MUIA_Textinput_MaxLines:
	case MUIA_Textinput_AutoExpand:
	case MUIA_Textinput_LeftOffset:
	case MUIA_Textinput_TopOffset:
	case MUIA_Textinput_TSCO:
	case MUIA_Textinput_Blinkrate:
	case MUIA_Textinput_Cursorstyle:
	case MUIA_Textinput_AdvanceOnCR:
	case MUIA_Textinput_TmpExtension:
	case MUIA_Textinput_Quiet:
	case MUIA_Textinput_Acknowledge:
	case MUIA_Textinput_Integer:
	case MUIA_Textinput_MinVersion:
	case MUIA_Textinput_DefKeys:
	case MUIA_Textinput_DefaultPopup:
	case MUIA_Textinput_WordWrap:
	case MUIA_Textinput_IsNumeric:
	case MUIA_Textinput_MinVal:
	case MUIA_Textinput_MaxVal:
	case MUIA_Textinput_AcceptChars:
	case MUIA_Textinput_RejectChars:
	case MUIA_Textinput_Changed:
	case MUIA_Textinput_AttachedList:
	case MUIA_Textinput_RemainActive:
	case MUIA_Textinput_CursorPos:
	case MUIA_Textinput_Secret:
	case MUIA_Textinput_Lines:
	case MUIA_Textinput_Editable:
	case MUIA_Textinputscroll_UseWinBorder:
	case MUIA_Textinput_IsOld:
	case MUIA_Textinput_MarkStart:
	case MUIA_Textinput_MarkEnd:
	case MUIA_Textinputscroll_VertScrollerOnly:
	case MUIA_Textinput_NoInput:
	case MUIA_Textinput_SetMin:
	case MUIA_Textinput_SetMax:
	case MUIA_Textinput_SetVMax:
	case MUIA_Textinput_Styles:
	case MUIA_Textinput_PreParse:
	case MUIA_Textinput_Format:
	case MUIA_Textinput_SetVMin:
	case MUIA_Textinput_HandleURLHook:
	case MUIA_Textinput_Tabs:
	case MUIA_Textinput_TabLen:
	case MUIA_Textinput_Bookmark1:
	case MUIA_Textinput_Bookmark2:
	case MUIA_Textinput_Bookmark3:
	case MUIA_Textinput_CursorSize:
	case MUIA_Textinput_TopLine:
#endif

	//   case MUIA_BetterString_...:
	case MUIA_BetterString_SelectSize:
	case MUIA_BetterString_StayActive:
	case MUIA_BetterString_Columns:

	case MUIA_Popph_ContextMenu:
	case MUIA_Popph_StringMaxLen:
	case MUIA_Popph_StringType:
	    *store = XGET( data->str, ((struct opGet *)msg)->opg_AttrID );
	    break;

	//   case MUIA_List_...:
	case MUIA_List_Active:
	case MUIA_List_AdjustHeight:
	case MUIA_List_AdjustWidth:
	case MUIA_List_AutoVisible:
	case MUIA_List_CompareHook:
	case MUIA_List_ConstructHook:
	case MUIA_List_DestructHook:
	case MUIA_List_DisplayHook:
	case MUIA_List_DragSortable:
	case MUIA_List_DropMark:
	case MUIA_List_Entries:
	case MUIA_List_First:
	case MUIA_List_Format:
	case MUIA_List_InsertPosition:
	case MUIA_List_MinLineHeight:
	case MUIA_List_MultiTestHook:
	case MUIA_List_Pool:
	case MUIA_List_PoolPuddleSize:
	case MUIA_List_PoolThreshSize:
	case MUIA_List_Quiet:
	case MUIA_List_ShowDropMarks:
	case MUIA_List_SourceArray:
	case MUIA_List_Title:
	case MUIA_List_Visible:

	//   case MUIA_Listview_...:
	case MUIA_Listview_ClickColumn:
	case MUIA_Listview_DefClickColumn:
	case MUIA_Listview_DoubleClick:
	case MUIA_Listview_DragType:
	case MUIA_Listview_Input:
	case MUIA_Listview_List:
	case MUIA_Listview_MultiSelect:
	case MUIA_Listview_ScrollerPos:
	case MUIA_Listview_SelectChange:
	    *store = XGET( data->lv, ((struct opGet *)msg)->opg_AttrID );
	    break;

#ifdef HAVE_TEXTINPUT_MCC
	case MUIA_Textinput_Contents:
#endif
	case MUIA_String_Contents:
	case MUIA_Popph_Contents:
	    //            D(bug(__NAME "MUIA_Popph_Contents\n"));
	    *store = XGET( data->str, MUIA_Popph_Contents );
	    return TRUE;

	case MUIA_Popph_Separator:
	    //            D(bug(__NAME "MUIA_Popph_Separator\n"));
	    *store = data->separator;
	    return TRUE;

	case MUIA_Popph_CopyEntries:
	    //            D(bug(__NAME "MUIA_Popph_CopyEntries\n"));
	    *store = data->copyentries;
	    return TRUE;

	case MUIA_Popph_ReplaceMode:
	    *store = data->replacemode;
	    return TRUE;

	case MUIA_Popasl_Active:
	case MUIA_Popph_AslActive:
	    *store = data->asl_active;
	    return TRUE;

	case MUIA_Popasl_Type:
	case MUIA_Popph_AslType:
	    *store = data->asl_type;
	    return TRUE;

	case MUIA_Popph_StringObject:
	    *store = (IPTR)data->str;
	    return TRUE;

	case MUIA_Popph_ListObject:
	    *store = (IPTR)data->lv;
	    return TRUE;

	case MUIA_Popph_PopCycleChain:
	    *store = XGET( data->popbutton, MUIA_CycleChain);
	    return TRUE;

	case MUIA_Popph_SingleColumn:
	    *store = data->singlecolumn;
	    return TRUE;

    }
    return DoSuperMethodA(cl, obj, msg);
}

// Dispatcher
BOOPSI_DISPATCHER(IPTR, _Dispatcher, cl, obj, msg)
{
    switch (msg->MethodID)
    {
	case OM_NEW    : return Popph__OM_NEW     (cl, obj, (APTR)msg);
	case OM_DISPOSE: return Popph__OM_DISPOSE (cl, obj, (APTR)msg);
	case OM_SET    : return Popph__OM_SET     (cl, obj, (APTR)msg);
	case OM_GET    : return Popph__OM_GET     (cl, obj, (APTR)msg);

	case MUIM_Popph_OpenAsl : return Popph__MUIM_OpenAsl (cl, obj, (APTR)msg);

  	//  case MUIM_List_...:
	case MUIM_List_Clear:
	case MUIM_List_CreateImage:
	case MUIM_List_DeleteImage:
	case MUIM_List_Exchange:
	case MUIM_List_GetEntry:
	case MUIM_List_Insert:
	case MUIM_List_InsertSingle:
	case MUIM_List_Jump:
	case MUIM_List_Move:
	case MUIM_List_NextSelected:
	case MUIM_List_Redraw:
	case MUIM_List_Remove:
	case MUIM_List_Select:
	case MUIM_List_Sort:
	case MUIM_List_TestPos:
	    {
		struct Popplaceholder_Data *data = INST_DATA(cl,obj);
		return DoMethodA( data->lv, msg );
	    }
    }

    return DoSuperMethodA(cl, obj, msg);
}
BOOPSI_DISPATCHER_END
