#include "Private.h"

/****************************************************************************
*** Hooks *******************************************************************
****************************************************************************/

///code_minmax
F_HOOK(void,code_minmax)
{
    struct LocalObjectData *LOD = F_LOD(((FClass *)(Hook->h_Data)),Obj);

    if (LOD->strings)
    {
     
        uint32 minw=0,minh=0;
        STRPTR *str;

        //F_Set(_render,FA_Render_Forbid,TRUE);

        for (str = LOD->strings ; *str ; str++)
        {
            F_Set(LOD->text,FA_Text,(uint32)(*str));

            _text_minw = 0; _text_maxw = FV_MAXMAX;
            _text_minh = 0; _text_maxh = FV_MAXMAX;

            F_Do(LOD->text,FM_AskMinMax);

            if (_text_minw > minw) minw = _text_minw;
            if (_text_minh > minh) minh = _text_minh;
        }

        /* reset text min&max */

        F_Set(LOD->text,FA_Text,(uint32)(LOD->strings[LOD->active]));
        
        //F_Set(_render,FA_Render_Forbid,FALSE);

        minw += LOD->image_area_public->MinMax.MinW + F_Get(Obj,FA_Group_HSpacing);
        if (LOD->image_area_public->MinMax.MinH > minh) minh = LOD->image_area_public->MinMax.MinH;

        _minw += minw;
        _minh += minh;
    }
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Cycle_New
F_METHOD(uint32,Cycle_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    F_AREA_SAVE_PUBLIC;
  
    LOD->p_PreParse = "$cycle-preparse";
    LOD->p_Position = "$cycle-position";
    LOD->p_Layout   = "$cycle-layout";
    LOD->p_level    = "$cycle-level";

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Cycle_PreParse: LOD->p_PreParse = (STRPTR)(item.ti_Data); break;
        case FA_Cycle_Position: LOD->p_Position = (STRPTR)(item.ti_Data); break;
        case FA_Cycle_Layout:   LOD->p_Layout   = (STRPTR)(item.ti_Data); break;
        case FA_Cycle_Level:    LOD->p_level = (STRPTR)(item.ti_Data); break;
    }

    LOD->image = ImageObject,
       
        FA_ChainToCycle,   FALSE,
        FA_SetMax,         FV_SetWidth,
       "FA_Image_Spec",   "$cycle-image",
    
    End;

    LOD->text  = TextObject,
                        
        FA_ChainToCycle,   FALSE,
        FA_Text_VCenter,   TRUE,
    
    End;

    LOD->image_area_public = (FAreaPublic *) F_Get(LOD->image,FA_Area_PublicData);
    _text_area_public = (FAreaPublic *) F_Get(LOD->text,FA_Area_PublicData);

    return F_SuperDo(Class,Obj,Method,

        FA_Horizontal,          TRUE,
        FA_ChainToCycle,        TRUE,
        FA_SetMax,              FV_SetHeight,

        FA_Back,                "$cycle-button-back",
        FA_Font,                "$cycle-button-font",
        FA_Frame,               "$cycle-button-frame",

        FA_Group_MinMaxHook,    &Hook_MinMax,

    TAG_MORE, Msg);
}
//+
///Cycle_Dispose
F_METHOD(void,Cycle_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   if (FF_CYCLE_CREATED & LOD->flags)
   {
      F_Dispose(LOD->strings); LOD->strings = NULL;
   }

   F_DisposeObj(LOD->text);
   F_DisposeObj(LOD->image);
   
   F_SUPERDO();
}
//+
///Cycle_Get
F_METHOD(void,Cycle_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem         *Tags = Msg,
                           item;

   BOOL up = FALSE;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Cycle_Active:   F_STORE(LOD->active); break;

      default: up = TRUE;
   }

   if (up) F_SUPERDO();
}
//+
///Cycle_Set
F_METHOD(void,Cycle_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem         *Tags = Msg,*tag,item;
    
    uint32 type = FV_Cycle_EntriesType_String;
    uint8 separator = '|';
    APTR entries = NULL;
    BOOL update = FALSE;

    while  ((tag = F_DynamicNTI(&Tags,&item,Class)) != NULL)
    switch (item.ti_Tag)
    {
        case FA_Cycle_Active:
        {
            int32 active;

            switch (item.ti_Data)
            {
                case FV_Cycle_Active_Next:  active = (LOD->active + 1 >= LOD->numstrings) ? 0 : LOD->active + 1; break;
                case FV_Cycle_Active_Prev:  active = ((int32)(LOD->active - 1) < 0) ? LOD->numstrings - 1 : LOD->active - 1; break;
                case FV_Cycle_Active_First: active = 0;                   break;
                case FV_Cycle_Active_Last:  active = LOD->numstrings - 1;      break;
                default:                    active = MIN(MAX(0,(int32)(item.ti_Data)),LOD->numstrings - 1); break;
            }

            if (LOD->active != active)
            {
                LOD->active = active;

                F_Set(LOD->text,FA_Text,(uint32)(LOD->strings[LOD->active]));
            }
         
            tag->ti_Data = active;
        }
        break;

        case FA_Cycle_Entries:
        {
            entries = (APTR)(item.ti_Data); update = TRUE;
        }
        break;
     
        case FA_Cycle_EntriesType:
        {
            type = item.ti_Data;
        }
        break;
    
        case FA_Cycle_Level:
        {
            LOD->level = item.ti_Data;
        }
        break;
    
        case FA_Cycle_Separator:
        {
            separator = item.ti_Data;
        }
        break;
    }

    if (update)
    {
        if (FF_CYCLE_CREATED & LOD->flags)
        {
            F_Dispose(LOD->strings);
            
            LOD->flags &= ~FF_CYCLE_CREATED;
        }

        LOD->strings = NULL;
        LOD->numstrings = 0;

        if (type == FV_Cycle_EntriesType_Array)
        {
            STRPTR *en = (STRPTR *)(entries);

            LOD->strings = en;

            while (*en++)
            {
                LOD->numstrings++;
            }
        }
        else
        {
            STRPTR str = (STRPTR) entries;
            uint32 len = F_StrLen(str);

            if (len)
            {
                uint32 n=1;

                while (*str != '\0')
                {
                    if (*str++ == separator) n++;
                }

                if ((LOD->strings = F_New((sizeof(STRPTR) * (n+1)) + len + 1)) != NULL)
                {
                    STRPTR *en = LOD->strings;

                    LOD->numstrings = n;
                    LOD->flags |= FF_CYCLE_CREATED;

                    str = (STRPTR) ((uint32)(LOD->strings) + (sizeof(STRPTR) * (n+1)));

                    CopyMem((STRPTR) (entries), str, len + 1);

                    /* fill entries array and change ENTRIES_SEPARATOR into '\0' */
                    *(en++) = str;
                    
                    while (*str != '\0')
                    {
                        if (*str == separator)
                        {
                            *(str++) = '\0';
                            *(en++) = str;
                        }
                        else
                        {
                            str++;
                        }
                    }

                    /* terminate array */
                    
                    *en = NULL;
                }
            }
        }
     
        F_SuperDo(Class,Obj,Method,
 
            F_IDA(FA_Cycle_Active), LOD->active,
            
            TAG_MORE, Msg);
    }
    else
    {
        F_SUPERDO();
    }
}
//+

/****************************************************************************
*** Preferences *************************************************************
****************************************************************************/

STATIC FPreferenceScript Script[] =
{
    { "$cycle-font",          FV_TYPE_STRING,  "Contents", DEF_CYCLE_FONT,0 },
    { "$cycle-preparse",      FV_TYPE_STRING,  "Contents", DEF_CYCLE_PREP,0 },
    { "$cycle-image",         FV_TYPE_STRING,  "Spec",     DEF_CYCLE_IMAGE,0 },
    { "$cycle-layout",        FV_TYPE_INTEGER, "Active",   DEF_CYCLE_LAYOUT,0 },
    { "$cycle-position",      FV_TYPE_INTEGER, "Active",   (STRPTR) DEF_CYCLE_POSITION,0 },
    { "$cycle-level",         FV_TYPE_INTEGER, "Value",    (STRPTR) DEF_CYCLE_LEVEL,0 },
    { "$cycle-button-frame",  FV_TYPE_STRING,  "Spec",     DEF_CYCLE_BUTTON_FRAME,0 },
    { "$cycle-button-back",   FV_TYPE_STRING,  "Spec",     DEF_CYCLE_BUTTON_BACK,0 },
    { "$cycle-menu-frame",    FV_TYPE_STRING,  "Spec",     DEF_CYCLE_MENU_FRAME,0 },
    { "$cycle-menu-back",     FV_TYPE_STRING,  "Spec",     DEF_CYCLE_MENU_BACK,0 },
    { "$cycle-item-frame",    FV_TYPE_STRING,  "Spec",     DEF_CYCLE_ITEM_FRAME,0 },
    { "$cycle-item-back",     FV_TYPE_STRING,  "Spec",     DEF_CYCLE_ITEM_BACK,0 },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    STATIC STRPTR str_pos[] = { "On Active","Bellow",NULL };
    STATIC STRPTR str_lay[] = { "Right", "Left", NULL };

    F_XML_DEFS_INIT(13);
    F_XML_DEFS_ADD("msg:button","Button");
    F_XML_DEFS_ADD("msg:font","Font");
    F_XML_DEFS_ADD("msg:preparse","PreParse");
    F_XML_DEFS_ADD("msg:frame","Frame");
    F_XML_DEFS_ADD("msg:back","Back");
    F_XML_DEFS_ADD("msg:image","Image");
    F_XML_DEFS_ADD("msg:control","PopMenu Control");
    F_XML_DEFS_ADD("msg:position","Position");
    F_XML_DEFS_ADD("msg:level","Level");
    F_XML_DEFS_ADD("msg:speed","Speed");
    F_XML_DEFS_ADD("dat:positions",&str_pos);
    F_XML_DEFS_ADD("dat:layout",&str_lay);
    F_XML_DEFS_ADD("val:level",(APTR) DEF_CYCLE_LEVEL);
    F_XML_DEFS_DONE;

    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Cycle",

        "Script", Script,
        "XMLSource", "feelin/preference/cycle.xml",
        "XMLDefinitions", F_XML_DEFS,

    TAG_MORE,Msg);
}
