
/*
** $Id: popphs.c,v 1.5 2000/04/04 17:58:19 carlos Exp $
**
** © 1999-2000 Marcin Orlowski <carlos@amiga.com.pl>
*/

#include "popph.h"

#define __NAME "Popphs"

/*
#define MSG_MENU_TITLE   "Menu"
#define MSG_MENU_CUT     "Cut"
#define MSG_MENU_COPY    "Copy"
#define MSG_MENU_PASTE   "Paste"
#define MSG_MENU_CLEAR   "Clear"
*/

/// _DoCopy
int _DoCopy( Object *obj, struct PPHS_Data *data  )
{
char *buf = (char *)xget( obj, data->tag_contents );
int  len = strlen( buf );

    return( _WriteClip( buf, len, data->clip_device ) );
}
//|
/// _DoCut
int _DoCut( Object *obj, struct PPHS_Data *data )
{

int  result = _DoCopy( obj, data );

    if( result )
       set( obj, data->tag_contents, "" );

    return( result );
}
//|

/// DoCopy
ULONG PPHS_DoCopy( struct IClass *cl, Object *obj, struct opSet *msg )
{
struct PPHS_Data *data = INST_DATA(cl,obj);


    switch( data->stringtype )
       {
       case MUIV_Popph_StringType_Betterstring:
            {
            char *buf = (char *)(xget( obj, data->tag_contents ) + xget( obj, data->tag_bufpos ));
            int  len = xget( obj, MUIA_BetterString_SelectSize );

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


       case MUIV_Popph_StringType_Textinput:
            {
            DoMethod( obj, MUIM_Textinput_DoCopy );
            }
            break;


//       case MUIV_Popph_StringType_String:
       default:
            {
            char *buf = (char *)xget( obj, data->tag_contents );
            int  len = strlen( buf );

            _WriteClip( buf, len, data->clip_device );
            }
            break;
       }

    return( 0 );
}
//|
/// DoCut
ULONG PPHS_DoCut( struct IClass *cl, Object *obj, struct opSet *msg )
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
            if( xget( obj, MUIA_BetterString_SelectSize ) == 0 )
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

    return( 0 );
}
//|

/*
/// ContextMenu
static struct NewMenu pphs_menu[] =
{
    { NM_TITLE, (STRPTR)MSG_MENU_TITLE    ,0,0,0, (APTR)0  },

//        { NM_ITEM, (STRPTR)NM_BARLABEL                  ,0,0,0,(APTR)0  },

    { NM_ITEM,   (STRPTR)MSG_MENU_CUT     ,0,0,0, (APTR)MUIM_Popph_DoCut   },
    { NM_ITEM,   (STRPTR)MSG_MENU_COPY    ,0,0,0, (APTR)MUIM_Popph_DoCopy  },
//    { NM_ITEM,   (STRPTR)MSG_MENU_PASTE   ,0,0,0, (APTR)MUIM_Popph_DoPaste },
    { NM_ITEM,   (STRPTR)MSG_MENU_CLEAR   ,0,0,0, (APTR)MUIM_Popph_DoClear },

    { NM_END,NULL,0,0,0,(APTR)0 }
};
//|
*/

/// OM_NEW

ULONG ASM PPHS_New(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) struct opSet *msg)
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

           // MUIC_Textinput
           case MUIV_Popph_StringType_Textinput:
               tag_contents = MUIA_Textinput_Contents;
               tag_bufpos   = MUIA_Textinput_CursorPos;
               tag_maxlen   = MUIA_Textinput_MaxLen;
               break;
           }


    obj = (Object *)DoSuperNew(cl,obj,
                MUIA_Textinput_DefaultPopup, FALSE,
                tag_maxlen, strmaxlen,
//                MUIA_ContextMenu, TRUE,
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

    return((ULONG)obj);
}
//|
/// OM_DISPOSE
ULONG PPHS_Dispose(struct IClass *cl, Object *obj, struct opSet *msg)
{
struct PPHS_Data *data = INST_DATA(cl,obj);


    D(bug(__NAME ": OM_DISPOSE\n"));

    if( data->contextmenu )
       MUI_DisposeObject( data->contextmenu );

    return( DoSuperMethodA(cl, obj, msg) );
}
//|
/// OM_SET

ULONG ASM PPHS_Set(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
struct PPHS_Data *data = INST_DATA(cl,obj);
struct TagItem *tags, *tag;


    for(tags=((struct opSet *)msg)->ops_AttrList; tag=NextTagItem(&tags); )
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

    return(DoSuperMethodA(cl, obj, msg));
}

//|
/// OM_GET
static ULONG ASM PPHS_Get(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
struct PPHS_Data *data = INST_DATA(cl,obj);
ULONG  *store = ((struct opGet *)msg)->opg_Storage;

//    D(bug( __NAME ": OM_GET\n"));

    switch(((struct opGet *)msg)->opg_AttrID)
       {
       case MUIA_Popph_Contents:
            *store = xget( obj, data->tag_contents );
            return(TRUE);

       case MUIA_Popph_BufferPos:
            *store = xget( obj, data->tag_bufpos );
            return(TRUE);

       case MUIA_Popph_Separator:
            *store = data->separator;
            return(TRUE);

       case MUIA_Popph_StringMaxLen:
            *store = xget( obj, data->tag_maxlen );
            return(TRUE);

       case MUIA_Popph_StringType:
            *store = data->stringtype;
            return(TRUE);

       case MUIA_Popph_ContextMenu:
            *store = data->cmenu_enabled;
            return(TRUE);

       }

    return(DoSuperMethodA(cl, obj, msg));
}
//|

/*
/// ContextMenuBuild

ULONG PPHS_CMBuild(struct IClass *cl,Object *obj, struct MUIP_ContextMenuChoice *msg)
{
struct PPHS_Data *data = INST_DATA(cl,obj);


    D(bug(__NAME ": CMBuild: Old: %lx\n", data->contextmenu));

    if( data->contextmenu )
       MUI_DisposeObject( data->contextmenu );


    if( (data->contextmenu = MUI_MakeObject( MUIO_MenustripNM, pphs_menu, 0 )) )
           {
//           set( obj, MUIA_ContextMenu, data->contextmenu );

           D(bug( __NAME ": ContextMenu: %lx\n", data->contextmenu ));
           }

    return( (ULONG)data->contextmenu );
}

//|
/// ContextMenuChoice

ULONG PPHS_CMChoice(struct IClass *cl,Object *obj, struct MUIP_ContextMenuChoice *msg)
{
//struct PPHS_Data *data = INST_DATA(cl,obj);

LONG i = muiUserData( msg->item );

    D(bug( __NAME ": CMChoice: item: %lx\n", i ));

    if( i )
       {
       DoMethod( obj, i );
       }

    return( 0 );

}
//|
*/

/// mDragQuery
ULONG PPHS_DragQuery(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
struct PPHS_Data *data = INST_DATA(cl,obj);


    /* refuse to drop on ourselves */

    if( msg->obj != data->dropobj )
       return(MUIV_DragQuery_Refuse);

    return(MUIV_DragQuery_Accept);

}
//|
/// mDragDrop
ULONG PPHS_DragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
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

         if( buffer = AllocVec( keylen + stringlen + 5, MEMF_CLEAR ) )
            {
            CopyMem( current_string, buffer, pos );
            CopyMem( key, buffer + pos, keylen );
            CopyMem( current_string + pos, buffer + pos + keylen, stringlen - pos );

            set( obj, MUIA_Popph_Contents, buffer );
            set( obj, MUIA_Popph_BufferPos, pos + keylen );
//           set( xget( obj, MUIA_Window ), MUIA_Window_ActiveObject, obj );

//           DoMethod(_app(obj), MUIM_Application_PushMethod,
//           _win(obj), 3, MUIM_Set, MUIA_Window_ActiveObject, obj);

            FreeVec( buffer );
            }
         }
       }

       return( 0 );

}
//|

/// Dispatcher
ULONG ASM SAVEDS PPHS_Dispatcher(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
//struct PPHS_Data *data = INST_DATA(cl,obj);

//    D(bug(__NAME ": Disp: %lx\n", msg->MethodID ));

    switch( msg->MethodID )
       {
       case OM_NEW    : return(PPHS_New     (cl, obj, (APTR)msg));
       case OM_DISPOSE: return(PPHS_Dispose (cl, obj, (APTR)msg));
       case OM_SET    : return(PPHS_Set     (cl, obj, (APTR)msg));
       case OM_GET    : return(PPHS_Get     (cl, obj, (APTR)msg));
/*
       case MUIM_ContextMenuBuild  : return(PPHS_CMBuild   (cl, obj, (APTR)msg));
       case MUIM_ContextMenuChoice : return(PPHS_CMChoice  (cl, obj, (APTR)msg));
*/

       case MUIM_Popph_DoCut   :  return(PPHS_DoCut (cl, obj, (APTR)msg));
       case MUIM_Popph_DoCopy  :  return(PPHS_DoCut (cl, obj, (APTR)msg));


//#define MUIM_Textinput_DoPaste MCC_TI_ID(14)            /* V1 */

//       case MUIM_Popph_DoPaste:
/*
       case MUIM_Popph_DoClear:
            {
            set( obj, data->tag_contents, "" );
            }
            return(0);
*/
       case MUIM_DragQuery: return(PPHS_DragQuery (cl, obj, (APTR)msg));
       case MUIM_DragDrop : return(PPHS_DragDrop  (cl, obj, (APTR)msg));
       }

    return((ULONG)DoSuperMethodA(cl,obj,msg));

}
//|

#undef __NAME

