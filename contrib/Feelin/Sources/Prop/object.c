#include "Private.h"

/*** Methods ***************************************************************/

///Prop_New
F_METHOD(uint32,Prop_New)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    BOOL horiz = FALSE;
  
    F_AREA_SAVE_PUBLIC;

    LOD -> Entries = 1;
    LOD -> Visible = 1;
    LOD -> First   = 0;
    LOD -> Step    = 3;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
       case FA_Horizontal:  horiz = item.ti_Data; break;
       case FA_Prop_Knob:   LOD -> Knob = (APTR)(item.ti_Data); break;
    }

    if (!LOD -> Knob)
    {
       LOD -> Knob = AreaObject,
          FA_Back,          (horiz) ? "$prop-knob-back" : "$prop-knob-back-vertical",
          FA_Frame,         "$prop-knob-frame",
          FA_ColorScheme,   "$prop-knob-scheme",
          FA_ChainToCycle,   FALSE,
          End;
    }

    _knob_area_public = (FAreaPublic *) F_Get(LOD -> Knob,FA_Area_PublicData);

    if (_knob_area_public)
    {
        F_Do(LOD -> Knob,FM_Connect,Obj);

        return F_SuperDo(Class,Obj,Method,

            FA_Back,         (horiz) ? "$prop-back" : "$prop-back-vertical",
            FA_Frame,        "$prop-frame",
            FA_ColorScheme,  "$prop-scheme",
            FA_NoFill,        TRUE,

            TAG_MORE,Msg);
    }
    return NULL;
}
//+
///Prop_Dispose
F_METHOD(void,Prop_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(LOD -> Knob,FM_Disconnect);
    F_DisposeObj(LOD -> Knob); LOD -> Knob = NULL; _knob_area_public = NULL;

    F_SUPERDO();
}
//+
///Prop_Get
F_METHOD(void,Prop_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    BOOL up = FALSE;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Prop_Entries: F_STORE(LOD -> Entries);  break;
        case FA_Prop_Visible: F_STORE(LOD -> Visible);  break;
        case FA_Prop_First:   F_STORE(LOD -> First);    break;
        case FA_Prop_Useless: F_STORE((LOD -> Entries == 0 || LOD -> Visible == LOD -> Entries) ? TRUE : FALSE); break;

        default: up = TRUE;
    }

    if (up) F_SUPERDO();
}
//+
///Prop_Set

#define FF_UPDATE_ENTRIES                       (1 << 0)
#define FF_UPDATE_VISIBLE                       (1 << 1)
#define FF_UPDATE_FIRST                         (1 << 2)

F_METHOD(void,Prop_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;
    
    bits32 update=0;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Prop_Entries:
        {
            update |= FF_UPDATE_ENTRIES;
            
            LOD -> Entries = item.ti_Data;
        }
        break;
        
        case FA_Prop_Visible:
        {
            update |= FF_UPDATE_VISIBLE;
           
            LOD -> Visible = MAX(1,(int32)(item.ti_Data));
        }
        break;
        
        case FA_Prop_First:
        {
            update |= FF_UPDATE_FIRST;
           
            LOD -> First = MAX(0,(int32)(item.ti_Data));
        }
        break;
    }
          
    if (update)
    {
  //      F_Log(0,"entries (%ld) visible (%ld) first (%ld)",LOD -> Entries,LOD -> Visible,LOD -> First);
   
        if ((FF_UPDATE_ENTRIES | FF_UPDATE_VISIBLE) & update)
        {
            if ((int32)(LOD -> Entries) < (int32)(LOD -> Visible))
            {
                LOD -> Visible = LOD -> Entries;
            }
        }
        
        if ((int32)(LOD -> First)  > (int32)(LOD -> Entries - LOD -> Visible))
        {
            LOD -> First = LOD -> Entries - LOD -> Visible;
        }
           
        if ((FF_UPDATE_ENTRIES | FF_UPDATE_VISIBLE) & update)
        {
            if (LOD -> Entries == 0 || LOD -> Visible == LOD -> Entries)
            {
                F_Set(Obj,F_IDA(FA_Prop_Useless),TRUE);
            }
            else
            {
                F_Set(Obj,F_IDA(FA_Prop_Useless),FALSE);
            }
        }

        F_Draw(Obj,(FF_UPDATE_VISIBLE & update) ? FF_Draw_Object : FF_Draw_Update);
    }

    F_SUPERDO();
}
//+

///Preferences
STATIC FPreferenceScript Script[] =
{
    { "$prop-frame",                FV_TYPE_STRING, "Spec", "<frame id='18' />",0               },
    { "$prop-back",                 FV_TYPE_STRING, "Spec", "halfshadow",0                },
    { "$prop-back-vertical",        FV_TYPE_STRING, "Spec", "halfshadow",0                },
    { "$prop-scheme",               FV_TYPE_STRING, "Spec", NULL,0                        },
    { "$prop-knob-frame",           FV_TYPE_STRING, "Spec", "<frame id='23' />\n<frame id='24' />",0   },
    { "$prop-knob-back",            FV_TYPE_STRING, "Spec", "fill",0                      },
    { "$prop-knob-back-vertical",   FV_TYPE_STRING, "Spec", "fill",0                      },
    { "$prop-knob-scheme",          FV_TYPE_STRING, "Spec", NULL,0                        },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    F_XML_DEFS_INIT(6);

    F_XML_DEFS_ADD("msg:container","Container");
    F_XML_DEFS_ADD("msg:frame","Frame");
    F_XML_DEFS_ADD("msg:back","Back");
    F_XML_DEFS_ADD("msg:scheme","Scheme");
    F_XML_DEFS_ADD("msg:knob","Knob");
    F_XML_DEFS_ADD("msg:examples","Examples");
    F_XML_DEFS_DONE;

    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Prop",

        "Script", Script,
        "XMLSource", "feelin/preference/prop.xml",
        "XMLDefinitions", F_XML_DEFS,

    TAG_MORE,Msg);
}
//+
