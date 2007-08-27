#include "Private.h"

/*** Private ***************************************************************/

///Group_DoA
/*

   used: Group_Set(), Group_Get(), Group_Setup()

*/

void Group_DoA(FClass *Class,FObject Obj,uint32 Method,APTR Msg)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FAreaNode *node;

   for _each
   {
      F_DoA(node -> Object,Method,Msg);
   }
}
//+

/*** Methods ***************************************************************/

F_METHOD_PROTO(void,Group_Propagate);

///Group_New
F_METHOD(uint32,Group_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*real,item;

    STRPTR page_p_Font = "$page-font";
    STRPTR page_p_APrep = "$page-preparse-active";
    STRPTR page_p_IPrep = "$page-preparse-inactive";
    BOOL   pagemode=FALSE;
    BOOL   horizontal=FALSE;

    F_AREA_SAVE_PUBLIC;
    
    LOD -> Flags    = FF_Group_BufferRegion;
    LOD -> Rows     = 1;
    LOD -> Columns  = 1;
    LOD -> p_HSpace = "$group-spacing-horizontal";
    LOD -> p_VSpace = "$group-spacing-vertical";

    _flags |= FF_Area_Group;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Horizontal:
        {
            horizontal = item.ti_Data;
        }
        break;
   
        case FA_Group_HSpacing:
        {
            LOD -> p_HSpace = (STRPTR)(item.ti_Data);
        }
        break;
        
        case FA_Group_VSpacing:
        {
            LOD -> p_VSpace = (STRPTR)(item.ti_Data);
        }
        break;
        
        case FA_Group_RelSizing:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_Group_RelSizing;
            }
            else
            {
                LOD -> Flags &= ~FF_Group_RelSizing;
            }
        }
        break;
        
        case FA_Group_SameWidth:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_Group_SameWidth;
            }
            else
            {
                LOD -> Flags &= ~FF_Group_SameWidth;
            }
        }
        break;
        
        case FA_Group_SameHeight:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_Group_SameHeight;
            }
            else
            {
                LOD -> Flags &= ~FF_Group_SameHeight;
            }
        }
        break;
        
        case FA_Group_SameSize:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_Group_SameSize;
            }
            else
            {
                LOD -> Flags &= ~FF_Group_SameWidth;
            }
        }
        break;
        
        case FA_Group_Rows:
        {
           LOD -> Rows    = (item.ti_Data > 1) ? item.ti_Data : 1;
           LOD -> Columns = 0;
        }
        break;

        case FA_Group_Columns:
        {
           LOD -> Columns = (item.ti_Data > 1) ? item.ti_Data : 1;
           LOD -> Rows    = 0;
        }
        break;

        case FA_Group_LayoutHook:
        {
            LOD -> LayoutHook = (struct Hook *)(item.ti_Data);
        }
        break;
        
        case FA_Group_MinMaxHook:
        {
            LOD -> MinMaxHook = (struct Hook *)(item.ti_Data);
        }
        break;

        case FA_Group_PageMode:
        {
            pagemode = item.ti_Data;
        }
        break;
        
        case FA_Group_PageFont:
        {
            page_p_Font = (STRPTR)(item.ti_Data);
        }
        break;
        
        case FA_Group_PageAPreParse:
        {
            page_p_APrep = (STRPTR)(item.ti_Data);
        }
        break;
        
        case FA_Group_PageIPreParse:
        {
            page_p_IPrep = (STRPTR)(item.ti_Data);
        }
        break;
    }

    if (pagemode)
    {
        LOD -> PageData = F_New(sizeof (struct Group_PageData));
        
        if (LOD -> PageData)
        {
            LOD -> PageData -> p_Font  = page_p_Font;
            LOD -> PageData -> p_IPrep = page_p_IPrep;
            LOD -> PageData -> p_APrep = page_p_APrep;
        }
    }

    if (!LOD -> LayoutHook)
    {
        if (LOD -> PageData)
        {
            LOD -> LayoutHook = &CUD -> Hook_PLayout;
        }
        else
        {
            if (LOD -> Rows > 1 || LOD -> Columns > 1)
            {
                LOD -> LayoutHook = &CUD -> Hook_ALayout;
            }
            else
            {
                if (horizontal)
                {
                    LOD -> LayoutHook = &CUD -> Hook_HLayout;
                }
                else
                {
                    LOD -> LayoutHook = &CUD -> Hook_VLayout;
                }
            }
        }
    }

    if (FamilyObject,
        
        FA_Family_Owner,            Obj,
        FA_Family_CreateNodeHook,  &CUD -> Hook_CreateFamilyNode,
        FA_Family_DeleteNodeHook,  &CUD -> Hook_DeleteFamilyNode,
  
        TAG_MORE,Msg, End)
    {
        return F_SuperDo(Class,Obj,Method,

            FA_ChainToCycle, (LOD -> PageData) ? TRUE : FALSE,
            (LOD -> PageData) ? FA_Back : TAG_IGNORE, "$page-back",
            (LOD -> PageData) ? FA_NoFill : TAG_IGNORE, TRUE,
            FA_Frame_PreParse,   "$group-frame-preparse",
            FA_Frame_Position,   "$group-frame-position",
            FA_Frame_Font,       "$group-frame-font",

        TAG_MORE, Msg);
    }

    Tags = Msg;

    while ((real = F_DynamicNTI(&Tags,&item,Class)) != NULL)
    {
        if (item.ti_Tag == FA_Child)
        {
            F_DisposeObj((FObject)(item.ti_Data)); real -> ti_Tag = TAG_IGNORE; real -> ti_Data = NULL;
        }
    }

    return 0;
}
//+
///Group_Dispose
F_METHOD(void,Group_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_DisposeObj(LOD -> Family); LOD -> Family = NULL;

    if (LOD -> Region)
    {
        DisposeRegion(LOD -> Region); LOD -> Region = NULL;
    }

    if (LOD -> PageData)
    {
        F_Dispose(LOD -> PageData); LOD -> PageData = NULL;
    }

    F_SUPERDO();
}
//+
///Group_Get
F_METHOD(void,Group_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    F_OBJDO(LOD -> Family);

    if (GetTagData(FA_Group_Forward,FALSE,Tags))
    {
        Group_DoA(Class,Obj,Method,Msg);
    }

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Group_HSpacing:       F_STORE(LOD -> HSpace); break;
        case FA_Group_VSpacing:       F_STORE(LOD -> VSpace); break;
        case FA_Group_PageTitle:      F_STORE(LOD -> Name); break;
        case FA_Group_ActivePage:     F_STORE((LOD -> PageData) ? ((LOD -> PageData -> ActiveNode) ? LOD -> PageData -> ActiveNode -> Object : 0) : 0); break;
        case FA_Group_BufferRegion:   F_STORE(NULL != (FF_Group_BufferRegion & LOD -> Flags)); break;
    }
    F_SUPERDO();
}
//+
///Group_Set
F_METHOD(void,Group_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    BOOL forward = FALSE;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Family:
        {
            LOD -> Family = (FObject)(item.ti_Data);
        }
        break;

        case FA_Group_HSpacing:
        {
            LOD -> HSpace = item.ti_Data;
        }
        break;
        
        case FA_Group_VSpacing:
        {
            LOD -> HSpace = item.ti_Data;
        }
        break;
        
        case FA_Group_PageTitle:
        {
            LOD -> Name = (STRPTR)(item.ti_Data);
        }
        break;

        case FA_Group_Forward:
        {
           forward = item.ti_Data;
        }
        break;

        case FA_Group_ActivePage:
        {
            if (LOD -> PageData)
            {
                FAreaNode *node = LOD -> PageData -> ActiveNode;

                switch (item.ti_Data)
                {
                    case FV_Group_ActivePage_Prev:
                    {
                        if (node) node = node -> Prev;
                        if (!node) node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Tail);
                    }
                    break;

                    case FV_Group_ActivePage_Next:
                    {
                        if (node) node = node -> Next;
                        if (!node) node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head);
                    }
                    break;

                    case FV_Group_ActivePage_Last:
                    {
                        node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Tail);
                    }
                    break;

                    case FV_Group_ActivePage_First:
                    {
                        node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head);
                    }
                    break;

                    default:
                    {
                        uint32 i;

                        node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head);

                        for (i = 0 ; i < item.ti_Data ; i++)
                        {
                            if (!node -> Next) break; node = node -> Next;
                        }
                    }
                    break;
                }
             
                if (node && (node != LOD -> PageData -> ActiveNode))
                {
                    if (LOD -> PageData -> ActiveNode)
                    {
                        F_Do(LOD -> PageData -> ActiveNode -> Object,FM_Hide);
                    }

                    LOD -> PageData -> UpdateNode = LOD -> PageData -> ActiveNode;
                    LOD -> PageData -> ActiveNode = node;

                    if (LOD -> PageData -> ActiveNode)
                    {
                        F_Do(LOD -> PageData -> ActiveNode -> Object,FM_Show);
                    }

                    if (_render)
                    {
                        int16 x = _x + 10;
                        int16 y = _y + LOD -> PageData -> Font -> tf_YSize + 12;
                        int16 w = _w - 20;
                        int16 h = _h - LOD -> PageData -> Font -> tf_YSize - 17;

                        F_Layout(node -> Object,
                           (_sub_minw == _sub_maxw) ? (w - _sub_minw) / 2 + x : x,
                           (_sub_minh == _sub_maxh) ? (h - _sub_minh) / 2 + y : y,
                           (_sub_minw == _sub_maxw) ? _sub_minw : w,
                           (_sub_minh == _sub_maxh) ? _sub_maxh : h,
                        0);

                        F_Draw(Obj,FF_Draw_Update);
                    }
                }
            }
        }
        break;

        case FA_Group_BufferRegion:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_Group_BufferRegion;
            }
            else
            {
                LOD -> Flags &= ~FF_Group_BufferRegion;
            }

            if (!(FF_Group_BufferRegion & LOD -> Flags))
            {
                if (LOD -> Region)
                {
                    DisposeRegion(LOD -> Region); LOD -> Region = NULL;
                }
            }
        }
        break;
    }

    if (forward)
    {
        Group_DoA(Class,Obj,Method,Msg);
    }

    F_SUPERDO();
}
//+

///Group_InitChange
F_METHOD(void,Group_InitChange)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD -> Flags |= FF_GROUP_CHANGING;
}
//+
///Group_ExitChange

/* Will recalculate display after dynamic adding/removing */

F_METHOD(void,Group_ExitChange)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Flags & FF_GROUP_CHANGING)
    {
        LOD -> Flags &= ~FF_GROUP_CHANGING;

        if (_render)
        {
            F_Do(_win,FM_Window_RequestRethink,Obj);
        }
    }
}
//+

///Preferences
STATIC FPreferenceScript Script[] =
{
   { "$group-frame",               FV_TYPE_STRING,  "Spec",     DEF_GROUP_FRAME,0 },
   { "$group-frame-position",      FV_TYPE_INTEGER, "Active",   (STRPTR) DEF_GROUP_POSITION,0 },
   { "$group-frame-preparse",      FV_TYPE_STRING,  "Contents", DEF_GROUP_PREPARSE,0 },
   { "$group-frame-font",          FV_TYPE_STRING,  "Contents", DEF_GROUP_FONT,0 },
   { "$group-back",                FV_TYPE_STRING,  "Spec",     DEF_GROUP_BACK,0 },
   { "$group-spacing-horizontal",  FV_TYPE_INTEGER, "Value",    (STRPTR) DEF_GROUP_VSPACING,0 },
   { "$group-spacing-vertical",    FV_TYPE_INTEGER, "Value",    (STRPTR) DEF_GROUP_HSPACING,0 },
   { "$page-font",                 FV_TYPE_STRING,  "Contents", DEF_PAGE_FONT,0 },
   { "$page-preparse-inactive",    FV_TYPE_STRING,  "Contents", DEF_PAGE_PREPARSE,0 },
   { "$page-preparse-active",      FV_TYPE_STRING,  "Contents", DEF_PAGE_ALTPREPARSE,0 },
   { "$page-back",                 FV_TYPE_STRING,  "Spec",     DEF_PAGE_BACK,0 },
   { "$preference-frame",          FV_TYPE_STRING,  "Spec",     DEF_PREFERENCE_FRAME,0 },

   F_ARRAY_END
};

enum { ID_Num, ID_Cyc, ID_Str, ID_Frm, ID_Img, ID_Fnt   };

STATIC STRPTR str_pos[7];

#undef  F_CAT
#define F_CAT(n)                (FCC_CatalogTable[CAT_P_ ## n].String)

F_METHOD(uint32,Prefs_New)
{
    F_XML_DEFS_INIT(15);

    str_pos[0] = F_CAT(POS_UPLEFT);
    str_pos[1] = F_CAT(POS_UPRIGHT);
    str_pos[2] = F_CAT(POS_UPCENTER);
    str_pos[3] = F_CAT(POS_DOWNLEFT);
    str_pos[4] = F_CAT(POS_DOWNRIGHT);
    str_pos[5] = F_CAT(POS_DOWNCENTER);
    str_pos[6] = NULL;

    F_XML_DEFS_ADD("msg:group",F_CAT(GROUP));
    F_XML_DEFS_ADD("msg:position",F_CAT(POSITION));
    F_XML_DEFS_ADD("dat:positions",&str_pos);
    F_XML_DEFS_ADD("msg:preparse",F_CAT(PREPARSE));
    F_XML_DEFS_ADD("msg:font",F_CAT(FONT));
    F_XML_DEFS_ADD("msg:page",F_CAT(PAGE));
    F_XML_DEFS_ADD("msg:altpreparse",F_CAT(ALTPREPARSE));
    F_XML_DEFS_ADD("msg:frames",F_CAT(FRAMES));
    F_XML_DEFS_ADD("msg:normal",F_CAT(NORMAL));
    F_XML_DEFS_ADD("msg:virtual",F_CAT(VIRTUAL));
    F_XML_DEFS_ADD("msg:settings",F_CAT(SETTINGS));
    F_XML_DEFS_ADD("msg:backs",F_CAT(BACKS));
    F_XML_DEFS_ADD("msg:spacing",F_CAT(SPACING));
    F_XML_DEFS_ADD("msg:horiz",F_CAT(HORIZ));
    F_XML_DEFS_ADD("msg:verti",F_CAT(VERTI));
    F_XML_DEFS_DONE;

    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Group",

        "Script", Script,
        "XMLSource", "feelin/preference/group.xml",
        "XMLDefinitions", F_XML_DEFS,

    TAG_MORE,Msg);
}
//+
