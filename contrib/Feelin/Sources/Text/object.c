#include "Private.h"

/*** Methods ***************************************************************/

///Text_New
F_METHOD(uint32,Text_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    /* Default values */

    LOD -> Flags    = FF_Text_Static | FF_Text_Shortcut;

    return F_SuperDo(Class,Obj,Method,

        FA_SetMin, FV_SetBoth,

    TAG_MORE,Msg);
}
//+
///Text_Dispose
F_METHOD(void,Text_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (!(FF_Text_Static & LOD -> Flags))
    {
        F_Dispose(LOD -> Text);
    }

    LOD -> Text = NULL;

    F_SUPERDO();
}
//+
///Text_Set

#define FF_UPDATE_CONTENTS                      (1 << 0)
#define FF_UPDATE_PREP                          (1 << 1)
#define FF_UPDATE_ALTPREP                       (1 << 2)
#define FF_UPDATE_STATE                         (1 << 3)
#define FF_UPDATE_FONT                          (1 << 4)
#define FF_UPDATE_SHORTCUT                      (1 << 5)

F_METHOD(void,Text_Set)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem         *Tags = Msg,item;

    bits32 update=0;
    BOOL selected = (0 != (FF_Area_Selected & _flags));

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Selected:
        {
            selected = item.ti_Data; 

            if (LOD -> Prep[0] != LOD -> Prep[1] && LOD -> Prep[1] != NULL) update |= FF_UPDATE_STATE;
        }
        break;

        case FA_Font:
        {
            update |= FF_UPDATE_FONT;
        }
        break;

        /****/

        case FA_Text:
        {
            update |= FF_UPDATE_CONTENTS;
            
            if (FF_Text_Static & LOD -> Flags)
            {
                LOD -> Text = (STRPTR)(item.ti_Data);
            }
            else
            {
                F_Dispose(LOD -> Text); LOD -> Text = NULL;
                
                LOD->Text = F_StrNew(NULL,"%s",item.ti_Data);
            }
        }
        break;

        case FA_Text_PreParse:
        {
            update |= FF_UPDATE_PREP;
            
            LOD -> Prep[0] = (STRPTR)(item.ti_Data);
        }
        break;
        
        case FA_Text_AltPreParse:
        {
            update |= FF_UPDATE_ALTPREP;

            LOD -> Prep[1] = (STRPTR)(item.ti_Data);
        }
        break;

        case FA_Text_Shortcut:
        {
            update |= FF_UPDATE_SHORTCUT;

            if (item.ti_Data) LOD -> Flags |= FF_Text_Shortcut;
            else              LOD -> Flags &= ~FF_Text_Shortcut;
        }
        break;

        case FA_Text_HCenter:      if (item.ti_Data) LOD -> Flags |= FF_Text_HCenter; else LOD -> Flags &= ~FF_Text_HCenter; break;
        case FA_Text_VCenter:      if (item.ti_Data) LOD -> Flags |= FF_Text_VCenter; else LOD -> Flags &= ~FF_Text_VCenter; break;
/*FIXME:

    Si pas encore de buffer créer et copier. Si buffer mettre à zéro et détruire

*/
        case FA_Text_Static:       if (item.ti_Data) LOD -> Flags |= FF_Text_Static;  else LOD -> Flags &= ~FF_Text_Static;  break;
    }

    F_SUPERDO();

    if (update && _render)
    {
        STRPTR prep;

        if (FF_UPDATE_PREP & update)
        {
            LOD -> Prep[0] = (STRPTR) F_Do(_app,FM_Application_Resolve,LOD -> Prep[0],NULL);
        }

        if (FF_UPDATE_ALTPREP & update)
        {
            LOD -> Prep[1] = (STRPTR) F_Do(_app,FM_Application_Resolve,LOD -> Prep[1],NULL);
        }
         
        prep = LOD -> Prep[selected ? 1 : 0];
        if (!prep) prep = LOD -> Prep[0];

        F_Do(LOD -> TextDisplay,FM_Set,
            
            FA_TextDisplay_Font,     _font,
            FA_TextDisplay_PreParse, prep,
            FA_TextDisplay_Contents, LOD -> Text,
            FA_TextDisplay_Shortcut, (0 != (FF_Text_Shortcut & LOD -> Flags)),
            
            TAG_DONE);

        F_Set(Obj,FA_ControlChar,F_Get(LOD -> TextDisplay,FA_TextDisplay_Shortcut));
        
//      F_Log(0,"Text (%s) Short (%ld)(%lc),",LOD -> Text,(0 != (FF_Text_Shortcut & LOD -> Flags)),F_Get(LOD -> TextDisplay,FA_TextDisplay_Shortcut));

        F_Draw(Obj,FF_Draw_Update);
    }
}
//+
///Text_Get
F_METHOD(void,Text_Get)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem         *Tags = Msg,item;

    F_SUPERDO();

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Text:             F_STORE(LOD -> Text);    break;
        case FA_Text_PreParse:    F_STORE(LOD -> Prep[0]); break;
        case FA_Text_AltPreParse: F_STORE(LOD -> Prep[1]); break;
        case FA_Text_HCenter:     F_STORE(FALSE != (FF_Text_HCenter & LOD -> Flags)); break;
        case FA_Text_VCenter:     F_STORE(FALSE != (FF_Text_VCenter & LOD -> Flags)); break;
        case FA_Text_Static:      F_STORE(FALSE != (FF_Text_Static  & LOD -> Flags)); break;
    }
}
//+
