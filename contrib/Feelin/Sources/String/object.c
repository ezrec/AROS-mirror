#include "Private.h"

/*** Methods ***************************************************************/

///String_New
F_METHOD(uint32,String_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    F_AREA_SAVE_PUBLIC;

    LOD -> Flags = FF_String_AdvanceOnCR;
    LOD -> Max   = 0;
    LOD -> Sel   = 1;

    LOD -> p_Cursor     = "$string-cursor";
    LOD -> p_Blink      = "$string-blink";
    LOD -> p_BlinkSpeed = "$string-blink-speed";
    LOD -> p_ASpec      = "$string-text-active";
    LOD -> p_BSpec      = "$string-text-block";
    LOD -> p_ISpec      = "$string-text-inactive";

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_String_Blink:         LOD -> p_Blink       = (STRPTR)(item.ti_Data); break;
        case FA_String_BlinkSpeed:    LOD -> p_BlinkSpeed  = (STRPTR)(item.ti_Data); break;
        case FA_String_Cursor:        LOD -> p_Cursor      = (STRPTR)(item.ti_Data); break;
        case FA_String_Max:           LOD -> Max           = (item.ti_Data) ? (item.ti_Data + 1) : 0; break;
        case FA_String_Secret:        if (item.ti_Data) LOD -> Flags |= FF_String_Secret; else LOD -> Flags &= ~FF_String_Secret; break;
        case FA_String_TextActive:    LOD -> p_ASpec       = (STRPTR)(item.ti_Data); break;
        case FA_String_TextBlock:     LOD -> p_BSpec       = (STRPTR)(item.ti_Data); break;
        case FA_String_TextInactive:  LOD -> p_ISpec       = (STRPTR)(item.ti_Data); break;
    }

    return F_SuperDo(Class,Obj,Method,

        FA_Back,         "$string-back",
        FA_Font,         "$string-font",
        FA_Frame,        "$string-frame",
        FA_ColorScheme,  "$string-scheme",
        FA_SetMax,        FV_SetHeight,

    TAG_MORE,Msg);
}
//+
///String_Dispose
F_METHOD(void,String_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Dispose(LOD -> String);  LOD -> String = NULL;
    F_Dispose(LOD -> Undo);    LOD -> Undo = NULL;

    F_SUPERDO();
}
//+
///String_Set
F_METHOD(void,String_Set)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item,*tag;

    while  ((tag = F_DynamicNTI(&Tags,&item,Class)) != NULL)
    switch (item.ti_Tag)
    {
        case FA_String_Accept:
        {
            switch (item.ti_Data)
            {
                case FV_String_Decimal:
                {
                    LOD -> Accept = "0123456789";
                }
                break;

                case FV_String_Hexadecimal:
                {
                    LOD -> Accept = "0123456789ABCDEFabcdef";
                }
                break;

                default:
                {
                    LOD -> Accept = (STRPTR)(item.ti_Data);
                }
                break;
            }
        }
        break;

        case FA_String_AdvanceOnCR:
        {
            if (item.ti_Data) LOD -> Flags |=  FF_String_AdvanceOnCR;
            else              LOD -> Flags &= ~FF_String_AdvanceOnCR;
        }
        break;

        case FA_String_BlinkSpeed:
        {
            LOD -> BlinkSpeed   = item.ti_Data;
            LOD -> BlinkElapsed = 0;
        }
        break;

        case FA_String_Contents:
        {
            if (item.ti_Data != (uint32)(LOD -> String))
            {
                LOD -> Pos = 0; LOD -> Len = 0;
                LOD -> Cur = 0; LOD -> Sel = 1;

                if (item.ti_Data)
                {
                    STRPTR str = (STRPTR)(item.ti_Data);

                    while (*str)
                    {
                        String_Insert(Class,Obj,*str++);

                        if (LOD -> Max)
                        {
                            if (LOD -> Len + 1 == LOD -> Max) break;
                        }
                    }

                    F_Dispose(LOD -> Undo);
                    LOD -> Undo = F_StrNew(NULL,"%s",LOD -> String);
                }
                else if (LOD -> String)
                {
                    if (LOD -> Max)
                    {
                        LOD -> String[0] = 0;
                    }
                    else
                    {
                        F_Dispose(LOD -> String); LOD -> String = NULL; LOD -> Allocated = 0;
                    }
                }

    //            F_Log(0,"len (%ld)(%s)(%ld)",LOD -> Len,LOD -> String,LOD -> String[LOD -> Len]);

                LOD -> Pos = 0;
                LOD -> Cur = 0;

                F_Draw(Obj,FF_Draw_Object);

                tag -> ti_Data = (uint32) LOD -> String;
            }
        }
        break;

        case FA_String_Format:
        {
            LOD -> Justify = item.ti_Data;
            F_Draw(Obj,FF_Draw_Object);
        }
        break;

        case FA_String_Integer:
        {
            STRPTR spec;

            spec = F_StrNew(NULL,"%ld",item.ti_Data);

            F_Set(Obj,F_IDA(FA_String_Contents),(uint32)(spec));

            F_Dispose(spec);
        }
        break;

        case FA_String_Reject:
        {
            switch (item.ti_Data)
            {
                case FV_String_Decimal:     LOD -> Reject = "0123456789"; break;
                case FV_String_Hexadecimal: LOD -> Reject = "0123456789ABCDEFabcdef"; break;
                default:                    LOD -> Reject = (STRPTR)(item.ti_Data);
            }
        }
        break;

        case FA_String_AttachedList:
        {
            LOD -> AttachedList = (FObject)(item.ti_Data);
        }
        break;
    }

    F_SUPERDO();
}
//+
///String_Get
F_METHOD(void,String_Get)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   struct TagItem         *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_String_AdvanceOnCR:   F_STORE(0 != (LOD -> Flags & FF_String_AdvanceOnCR)); break;
      case FA_String_AttachedList:  F_STORE(LOD -> AttachedList); break;
      case FA_String_Blink:         F_STORE(LOD -> Blink);  break;
      case FA_String_BlinkSpeed:    F_STORE(LOD -> BlinkSpeed);   break;
      case FA_String_Contents:      F_STORE((LOD -> String && *LOD -> String) ? LOD -> String : NULL); break;
      case FA_String_Cursor:        F_STORE(LOD -> Cursor); break;
      case FA_String_Format:        F_STORE(LOD -> Justify); break;
      case FA_String_Integer:       F_STORE(atol(LOD -> String));  break;
      case FA_String_Max:           F_STORE(LOD -> Max);    break;
      case FA_String_TextActive:    F_STORE(LOD -> APen);   break;
      case FA_String_TextBlock:     F_STORE(LOD -> BPen);   break;
      case FA_String_TextInactive:  F_STORE(LOD -> IPen);   break;
   }
   F_SUPERDO();
}
//+

///Preferences
STATIC FPreferenceScript Script[] =
{
    { "$string-font",          FV_TYPE_STRING,  "Contents", NULL,0 },
    { "$string-frame",         FV_TYPE_STRING,  "Spec",     "<frame id='47' padding-width='3' padding-height='1' />\n<frame id='47' padding-width='3' padding-height='1' />",0 },
    { "$string-back",          FV_TYPE_STRING,  "Spec",     "dark;shine",0 },
    { "$string-scheme",        FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$string-cursor",        FV_TYPE_STRING,  "Spec",     DEF_STRING_CURSOR,0 },
    { "$string-blink",         FV_TYPE_STRING,  "Spec",     DEF_STRING_BLINK,0 },
    { "$string-blink-speed",   FV_TYPE_INTEGER, "Value",    DEF_STRING_BLINK_SPEED,0 },
    { "$string-text-active",   FV_TYPE_STRING,  "Spec",     DEF_STRING_TEXT_ACTIVE,0 },
    { "$string-text-inactive", FV_TYPE_STRING,  "Spec",     DEF_STRING_TEXT_INACTIVE,0 },
    { "$string-text-block",    FV_TYPE_STRING,  "Spec",     DEF_STRING_TEXT_BLOCK,0 },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    F_XML_DEFS_INIT(13);
    F_XML_DEFS_ADD("msg:gadget","Gadget");
    F_XML_DEFS_ADD("msg:font","Font");
    F_XML_DEFS_ADD("msg:frame","Frame");
    F_XML_DEFS_ADD("msg:back","Back");
    F_XML_DEFS_ADD("msg:scheme","Scheme");
    F_XML_DEFS_ADD("msg:frame","Frame");
    F_XML_DEFS_ADD("msg:speed","Speed");
    F_XML_DEFS_ADD("msg:cursor","Cursor");
    F_XML_DEFS_ADD("msg:blink","Blink");
    F_XML_DEFS_ADD("msg:text","Text");
    F_XML_DEFS_ADD("msg:active","Active");
    F_XML_DEFS_ADD("msg:inactive","Inactive");
    F_XML_DEFS_ADD("msg:block","Block");
    F_XML_DEFS_DONE;

    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "String",

        "Script", Script,
        "XMLSource", "feelin/preference/string.xml",
        "XMLDefinitions", F_XML_DEFS,

    TAG_MORE,Msg);
}
//+
