#include "Private.h"

struct FS_REFLECT                               { FClass *Class; uint32 Value; };
struct FS_HIDE                                  { FClass *Class; uint32 Value; };

///code_reflect
F_HOOKM(void,code_reflect,FS_REFLECT)
{
    struct FeelinClass *Class = Msg -> Class;

    F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_Scrollbar_First),Msg -> Value,TAG_DONE);
}
//+
///code_hide
F_HOOKM(void,code_hide,FS_HIDE)
{
    if (Msg->Value)
    {
        F_Set(Obj, FA_Hidden, FF_Hidden_Check | TRUE);
    }
    else
    {
        F_Set(Obj, FA_Hidden, FF_Hidden_Check | FALSE);
    }
}
//+

/*** Methods ***************************************************************/

///SB_New
F_METHOD(uint32,SB_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    BOOL horiz=FALSE;
    uint32 prop_entries=100,prop_first=0,prop_visible=100;

    F_AREA_SAVE_PUBLIC;
  
    LOD -> p_Type = "$scrollbar-type";

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Horizontal:        horiz = item.ti_Data; break;
        case FA_Scrollbar_Type:    LOD -> p_Type = (STRPTR)(item.ti_Data); break;
        case FA_Scrollbar_Entries: prop_entries = item.ti_Data; break;
        case FA_Scrollbar_First:   prop_first = item.ti_Data; break;
        case FA_Scrollbar_Visible: prop_visible = item.ti_Data; break;
    }

    LOD -> Button1 = ImageObject,
        FA_Back,         (horiz) ? "$scrollbar-arrow-left-back"   : "$scrollbar-arrow-top-back",
        FA_Frame,        (horiz) ? "$scrollbar-arrow-left-frame"  : "$scrollbar-arrow-top-frame",
        FA_ColorScheme,  (horiz) ? "$scrollbar-arrow-left-scheme" : "$scrollbar-arrow-top-scheme",
       "FA_Image_Spec",  (horiz) ? "$scrollbar-arrow-left-image"  : "$scrollbar-arrow-top-image",
        FA_SetMax,       (horiz) ? FV_SetWidth : FV_SetHeight,

        FA_ChainToCycle, FALSE,
        FA_InputMode,    FV_InputMode_Release,
    End;

    LOD -> Button2 = ImageObject,
        FA_Back,         (horiz) ? "$scrollbar-arrow-right-back"   : "$scrollbar-arrow-bottom-back",
        FA_Frame,        (horiz) ? "$scrollbar-arrow-right-frame"  : "$scrollbar-arrow-bottom-frame",
        FA_ColorScheme,  (horiz) ? "$scrollbar-arrow-right-scheme" : "$scrollbar-arrow-bottom-scheme",
       "FA_Image_Spec",  (horiz) ? "$scrollbar-arrow-right-image"  : "$scrollbar-arrow-bottom-image",
        FA_SetMax,       (horiz) ? FV_SetWidth : FV_SetHeight,

        FA_ChainToCycle, FALSE,
        FA_InputMode,    FV_InputMode_Release,
    End;

    LOD -> Prop = PropObject,
        FA_Back,         (horiz) ? "$scrollbar-prop-back" : "$scrollbar-prop-back-vertical",
        FA_Frame,        "$scrollbar-prop-frame",
        FA_ColorScheme,  "$scrollbar-prop-scheme",

        FA_Horizontal,    horiz,
        FA_ChainToCycle,  FALSE,
        "Entries",        prop_entries,
        "First",          prop_first,
        "Visible",        prop_visible,
        
        "Knob", AreaObject,
            FA_Frame,         "$scrollbar-knob-frame",
            FA_Back,          (horiz) ? "$scrollbar-knob-back" : "$scrollbar-knob-back-vertical",
            FA_ColorScheme,   "$scrollbar-knob-scheme",
            FA_ChainToCycle,   FALSE,
        End,
    End;

    if (!LOD -> Button1 || !LOD -> Button2 || !LOD -> Prop)
    {
        F_DisposeObj(LOD -> Button1); LOD -> Button1 = NULL;
        F_DisposeObj(LOD -> Button2); LOD -> Button2 = NULL;
        F_DisposeObj(LOD -> Prop); LOD -> Prop = NULL;

        return NULL;
    }

    F_Do(LOD -> Prop,FM_Notify,"Useless",FV_Notify_Always,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_hide),Class,FV_Notify_Value);
    F_Do(LOD -> Prop,FM_Notify,"First",FV_Notify_Always,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_reflect),Class,FV_Notify_Value);
    F_Do(LOD -> Button1,FM_Notify,FA_Pressed,TRUE,LOD -> Prop,"FM_Prop_Decrease",1,1);
    F_Do(LOD -> Button2,FM_Notify,FA_Pressed,TRUE,LOD -> Prop,"FM_Prop_Increase",1,1);
    F_Do(LOD -> Button1,FM_Notify,FA_Timer,FV_Notify_Always,LOD -> Prop,"FM_Prop_Decrease",1,1);
    F_Do(LOD -> Button2,FM_Notify,FA_Timer,FV_Notify_Always,LOD -> Prop,"FM_Prop_Increase",1,1);

    return F_SuperDo(Class,Obj,Method,
       
        FA_Frame,        "$scrollbar-frame",
        FA_Back,         (horiz) ? "$scrollbar-back" : "$scrollbar-back-vertical",
        FA_ColorScheme,  "$scrollbar-scheme",

        FA_Group_VSpacing,   0,
        FA_Group_HSpacing,   0,
       
    TAG_MORE, Msg);
}
//+
///SB_Dispose
F_METHOD(void,SB_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_DisposeObj(LOD -> Prop);
    F_DisposeObj(LOD -> Button1);
    F_DisposeObj(LOD -> Button2);

    F_SUPERDO();
}
//+
///SB_Get

F_METHOD(void,SB_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Scrollbar_Entries: F_STORE(F_Get(LOD -> Prop,(uint32) "FA_Prop_Entries")); break;
        case FA_Scrollbar_First:   F_STORE(F_Get(LOD -> Prop,(uint32) "FA_Prop_First")); break;
        case FA_Scrollbar_Visible: F_STORE(F_Get(LOD -> Prop,(uint32) "FA_Prop_Visible")); break;
    }

    F_SUPERDO();
}
//+
///SB_Set

#define FF_FORWARD_ENTRIES                      (1 << 0)
#define FF_FORWARD_FIRST                        (1 << 1)
#define FF_FORWARD_VISIBLE                      (1 << 2)

F_METHOD(void,SB_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    bits32 flags=0;
    uint32 value_entries=0;
    uint32 value_first=0;
    uint32 value_visible=0;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Scrollbar_Entries: flags |= FF_FORWARD_ENTRIES; value_entries = item.ti_Data; break;
        case FA_Scrollbar_First:   flags |= FF_FORWARD_FIRST;   value_first   = item.ti_Data; break;
        case FA_Scrollbar_Visible: flags |= FF_FORWARD_VISIBLE; value_visible = item.ti_Data; break;
    }

    if (flags)
    {
        F_Do(LOD -> Prop,FM_Set,
            
            FA_NoNotify,TRUE,
            (FF_FORWARD_ENTRIES & flags) ? (uint32) "FA_Prop_Entries" : TAG_IGNORE, value_entries,
            (FF_FORWARD_FIRST   & flags) ? (uint32) "FA_Prop_First"   : TAG_IGNORE, value_first,
            (FF_FORWARD_VISIBLE & flags) ? (uint32) "FA_Prop_Visible" : TAG_IGNORE, value_visible,
            
            TAG_DONE);
    }

    F_SUPERDO();
}
//+

///SB_Setup
F_METHODM(uint32,SB_Setup,FS_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 type = F_Do(Msg -> Render -> Application,FM_Application_ResolveInt,LOD -> p_Type,FV_Scrollbar_Type_Bottom);

    switch (type)
    {
        case FV_Scrollbar_Type_Top:
        {
            F_Do(Obj,FM_AddMember,LOD -> Button1,FV_AddMember_Tail);
            F_Do(Obj,FM_AddMember,LOD -> Button2,FV_AddMember_Tail);
            F_Do(Obj,FM_AddMember,LOD -> Prop,FV_AddMember_Tail);
        }
        break;

        case FV_Scrollbar_Type_Sym:
        {
            F_Do(Obj,FM_AddMember,LOD -> Button1,FV_AddMember_Tail);
            F_Do(Obj,FM_AddMember,LOD -> Prop,FV_AddMember_Tail);
            F_Do(Obj,FM_AddMember,LOD -> Button2,FV_AddMember_Tail);
        }
        break;

        default:
        {
            F_Do(Obj,FM_AddMember,LOD -> Prop,FV_AddMember_Tail);
            F_Do(Obj,FM_AddMember,LOD -> Button1,FV_AddMember_Tail);
            F_Do(Obj,FM_AddMember,LOD -> Button2,FV_AddMember_Tail);
        }
        break;
    }

    return F_SUPERDO();
}
//+
///SB_Cleanup
F_METHOD(void,SB_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_SUPERDO();

    F_Do(Obj,FM_RemMember,LOD -> Prop);
    F_Do(Obj,FM_RemMember,LOD -> Button1);
    F_Do(Obj,FM_RemMember,LOD -> Button2);
}
//+

///Prefs
STATIC FPreferenceScript Script[] =
{
    { "$scrollbar-prop-frame",          FV_TYPE_STRING,  "Spec",     "<frame id='0' />",0 },
    { "$scrollbar-prop-back",           FV_TYPE_STRING,  "Spec",     "halfshadow",0 },
    { "$scrollbar-prop-back-vertical",  FV_TYPE_STRING,  "Spec",     "halfshadow",0 },
    { "$scrollbar-prop-scheme",         FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$scrollbar-knob-frame",          FV_TYPE_STRING,  "Spec",     "<frame id='42' />\n<frame id='40' />",0 },
    { "$scrollbar-knob-back",           FV_TYPE_STRING,  "Spec",     "fill;halfshadow",0 },
    { "$scrollbar-knob-back-vertical",  FV_TYPE_STRING,  "Spec",     "fill;halfshadow",0 },
    { "$scrollbar-knob-scheme",         FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$scrollbar-frame",               FV_TYPE_STRING,  "Spec",     "<frame id='1' />",0 },
    { "$scrollbar-back",                FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$scrollbar-back-vertical",       FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$scrollbar-scheme",              FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$scrollbar-type",                FV_TYPE_INTEGER, "Active",   (STRPTR) FV_Scrollbar_Type_Sym,0 },
    { "$scrollbar-arrow-left-frame",    FV_TYPE_STRING,  "Spec",     "<frame id='42' padding-width='2' />\n<frame id='40' padding-width='2' />",0 },
    { "$scrollbar-arrow-left-image",    FV_TYPE_STRING,  "Spec",     "<image type='brush' src='defaults/arrow_l.fb0' />",0 },
    { "$scrollbar-arrow-left-back",     FV_TYPE_STRING,  "Spec",     "fill;halfshadow",0 },
    { "$scrollbar-arrow-left-scheme",   FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$scrollbar-arrow-right-frame",   FV_TYPE_STRING,  "Spec",     "<frame id='42' padding-width='2' />\n<frame id='40' padding-width='2' />",0 },
    { "$scrollbar-arrow-right-image",   FV_TYPE_STRING,  "Spec",     "<image type='brush' src='defaults/arrow_r.fb0' />",0 },
    { "$scrollbar-arrow-right-back",    FV_TYPE_STRING,  "Spec",     "fill;halfshadow",0 },
    { "$scrollbar-arrow-right-scheme",  FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$scrollbar-arrow-top-frame",     FV_TYPE_STRING,  "Spec",     "<frame id='42' padding-height='2' />\n<frame id='40' padding-height='2' />",0 },
    { "$scrollbar-arrow-top-image",     FV_TYPE_STRING,  "Spec",     "<image type='brush' src='defaults/arrow_t.fb0' />",0 },
    { "$scrollbar-arrow-top-back",      FV_TYPE_STRING,  "Spec",     "fill;halfshadow",0 },
    { "$scrollbar-arrow-top-scheme",    FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$scrollbar-arrow-bottom-frame",  FV_TYPE_STRING,  "Spec",     "<frame id='42' padding-height='2' />\n<frame id='40' padding-height='2' />",0 },
    { "$scrollbar-arrow-bottom-image",  FV_TYPE_STRING,  "Spec",     "<image type='brush' src='defaults/arrow_b.fb0' />",0 },
    { "$scrollbar-arrow-bottom-back",   FV_TYPE_STRING,  "Spec",     "fill;halfshadow",0 },
    { "$scrollbar-arrow-bottom-scheme", FV_TYPE_STRING,  "Spec",     NULL,0 },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    struct p_LocalObjectData *LOD = F_LOD(Class,Obj);

    F_XML_DEFS_INIT(8);
    F_XML_DEFS_ADD("msg:container","Container");
    F_XML_DEFS_ADD("msg:knob","Knob");
    F_XML_DEFS_ADD("msg:scrollbar","Scrollbar");
    F_XML_DEFS_ADD("msg:frame","Frame");
    F_XML_DEFS_ADD("msg:back","Back");
    F_XML_DEFS_ADD("msg:scheme","Scheme");
    F_XML_DEFS_ADD("msg:image","Image");
    F_XML_DEFS_ADD("dat:radio-entries",&LOD -> radio_entries);
    F_XML_DEFS_DONE;

    LOD -> radio_entries[0] = ScrollbarObject, FA_Horizontal,TRUE, "Entries",20, "Visible",5, "Type","Bottom", End;
    LOD -> radio_entries[1] = ScrollbarObject, FA_Horizontal,TRUE, "Entries",20, "Visible",5, "Type","Sym", End;
    LOD -> radio_entries[2] = ScrollbarObject, FA_Horizontal,TRUE, "Entries",20, "Visible",5, "Type","Top", End;

    if (!LOD -> radio_entries[0] || !LOD -> radio_entries[1] || !LOD -> radio_entries[2])
    {
        F_DisposeObj(LOD -> radio_entries[0]);
        F_DisposeObj(LOD -> radio_entries[1]);
        F_DisposeObj(LOD -> radio_entries[2]); return NULL;
    }

    return F_SuperDo(Class,Obj,Method,

       FA_Group_PageTitle, "Scrollbar",

       "Script",         Script,
       "XMLSource",      "feelin/preference/scrollbar.xml",
       "XMLDefinitions", F_XML_DEFS,

    TAG_MORE,Msg);
}
//+
