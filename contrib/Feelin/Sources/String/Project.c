/*

$VER: 06.00 (2005/08/10)
 
    Portability update.
    
    Added the specials values FV_String_Decimal and FV_String_Hexadecimal to
    the FA_String_Accept and FA_String_Reject attributes.
    
    FA_String_Max no longer includes the NULL character in count.
    
    FM_List_FindString support.
 
$VER: 05.00 (2005/05/04)
 
    FA_String_Changed is no longer set along  with  FA_String_Contents.  The
    attribute  is  now  *only*  set  when  the  user  valid  the string with
    FV_KEY_PRESS.
    
    Getting FA_String_Contents returns NULL when the string is empty instead
    of a pointer to the string.
    
    FA_String_Contents is not updated if new pointer  is  same  the  as  the
    string buffer. (solves some notification troubles).
    
    Renamed FA_String_MaxLen as FA_String_Max
    
    If FA_String_Max is not defined, string buffer expands (and reduce) dynamicaly.

    When blinking, cursor is of block color only when the blink is drawn,
    otherwise it is of active color.
    
$VER: 04.00 (2003/08/18)

    Added Preference support

    Rename FA_String_TextCursor as FA_String_TextBlock, because it was  very
    confusing.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,String_New);
F_METHOD_PROTO(void,String_Dispose);
F_METHOD_PROTO(void,String_Get);
F_METHOD_PROTO(void,String_Set);
F_METHOD_PROTO(void,String_Setup);
F_METHOD_PROTO(void,String_Cleanup);
F_METHOD_PROTO(void,String_AskMinMax);
F_METHOD_PROTO(void,String_GoActive);
F_METHOD_PROTO(void,String_GoInactive);
F_METHOD_PROTO(void,String_Draw);
F_METHOD_PROTO(void,String_HandleEvent);
F_METHOD_PROTO(void,Prefs_New);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(Format) =
            {
                F_VALUES_ADD("Left",     FV_String_Left),
                F_VALUES_ADD("Center",   FV_String_Center),
                F_VALUES_ADD("Right",    FV_String_Right),

                F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Accept",         FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("AdvanceOnCR",    FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("Blink",          FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("BlinkSpeed",     FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Changed",        FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Contents",       FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Cursor",         FV_TYPE_STRING),
                F_ATTRIBUTES_ADD_WITH_VALUES("Format",FV_TYPE_INTEGER, Format),
                F_ATTRIBUTES_ADD("Integer",        FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Max",            FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Reject",         FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Secret",         FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("TextActive",     FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("TextBlock",      FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("TextInactive",   FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("AttachedList",   FV_TYPE_OBJECT),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(String_New,         FM_New),
                F_METHODS_ADD_STATIC(String_Dispose,     FM_Dispose),
                F_METHODS_ADD_STATIC(String_Get,         FM_Get),
                F_METHODS_ADD_STATIC(String_Set,         FM_Set),
                F_METHODS_ADD_STATIC(String_Setup,       FM_Setup),
                F_METHODS_ADD_STATIC(String_Cleanup,     FM_Cleanup),
                F_METHODS_ADD_STATIC(String_AskMinMax,   FM_AskMinMax),
                F_METHODS_ADD_STATIC(String_GoActive,    FM_GoActive),
                F_METHODS_ADD_STATIC(String_GoInactive,  FM_GoInactive),
                F_METHODS_ADD_STATIC(String_Draw,        FM_Draw),
                F_METHODS_ADD_STATIC(String_HandleEvent, FM_HandleEvent),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Area),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Prefs
        case FV_Query_PrefsTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Prefs_New, FM_New),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(PreferenceGroup),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}

