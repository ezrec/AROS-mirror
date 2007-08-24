#include "Private.h"

//#define DB_OPENFONT
 
/*** Private ***************************************************************/

///app_create_name
STATIC STRPTR app_create_name(struct LocalObjectData *LOD,STRPTR Name)
{
    STRPTR buf = F_New(256);

    if (buf)
    {
        if (((uint32)(Name) == FV_Application_ENV) ||
            ((uint32)(Name) == FV_Application_ENVARC))
        {
            if (LOD -> Title)
            {
                AddPart(buf,(Name == FV_Application_ENV) ? "ENV:Feelin" : "ENVARC:Feelin",255);
                AddPart(buf,LOD -> Title,255);
                CopyMem(".obj",buf + F_StrLen(buf),4);
            }
            else
            {
                F_Dispose(buf);

                return NULL;
            }
        }
        else
        {
            AddPart(buf,Name,255);
        }
        return buf;
    }
    return NULL;
}
//+
///app_create_ta
STATIC struct TextAttr *app_create_ta(STRPTR Define,struct TextAttr *TA)
{
    if (Define)
    {
        STRPTR name = F_New(256);

        if (name)
        {
            STRPTR back = name;

            while ((*Define != NULL) && (*Define != '/'))
            
            *name++ = *Define++;

            CopyMem(".font",name,5);
            
            name = back;

            if (*Define++ == '/')
            {
                uint16 size = atol(Define);
                
                if (size)
                {
                    TA -> ta_Name  = name;
                    TA -> ta_YSize = size;
                    TA -> ta_Style = FS_NORMAL;
                    TA -> ta_Flags = NULL;

                    return TA;
                }
            }
            F_Dispose(name);
        }
    }
    return NULL;
}
//+
///app_save_iff
STATIC void app_save_iff(FClass *Class,FObject Obj,STRPTR Name)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    static uint32 *nulls[] = {0,0};
    STRPTR file_name;

    if ((uint32)(Name) == FV_Application_BOTH)
    {
        app_save_iff(Class,Obj,(STRPTR)(FV_Application_ENV));
        app_save_iff(Class,Obj,(STRPTR)(FV_Application_ENVARC));

        return;
    }

    if ((uint32)(Name) == FV_Application_ENV)
    {
        BPTR lock = Lock("ENV:Feelin",ACCESS_WRITE);

        if (!lock) lock = CreateDir("ENV:Feelin");
        if (lock) UnLock(lock);
    }
    else if ((uint32)(Name) == FV_Application_ENVARC)
    {
        BPTR lock = Lock("ENVARC:Feelin",ACCESS_WRITE);

        if (!lock) lock = CreateDir("ENVARC:Feelin");
        if (lock) UnLock(lock);
    }

    if ((file_name = app_create_name(LOD,Name)) != NULL)
    {
        struct IFFHandle *iff = AllocIFF();

        if (iff)
        {
            if ((iff -> iff_Stream = Open(file_name,MODE_NEWFILE)) != NULL)
            {
                InitIFFasDOS(iff);

                if (!OpenIFF(iff,IFFF_WRITE))
                {
                    if (!PushChunk(iff,MAKE_ID('P','R','E','F'),MAKE_ID('F','O','R','M'),IFFSIZE_UNKNOWN))
                    {
                        if (!PushChunk(iff,MAKE_ID('P','R','E','F'),MAKE_ID('P','R','H','D'),8))
                        {
                            WriteChunkBytes(iff,nulls,8);
                            PopChunk(iff);
                        }

//                  F_DebugOut("Application.SaveOBJ - Space 0x%lx\n",LOD -> OBJSpace);

                        F_Do(LOD -> Dataspace,F_IDO(FM_Dataspace_WriteIFF),iff,MAKE_ID('P','R','E','F'),MAKE_ID('F','O','B','J'));

                        PopChunk(iff);
                    }
                    CloseIFF(iff);
                }
                Close(iff -> iff_Stream);
            }
            FreeIFF(iff);
        }
        F_Dispose(file_name);
    }
};
//+

/*** Methods ***************************************************************/

///App_Load
F_METHODM(int32,App_Load,FS_Application_Load)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Dataspace)
    {
        FFamilyNode *node;
        STRPTR name;

        F_Do(LOD -> Dataspace,F_IDO(FM_Dataspace_Clear));

        if ((name = app_create_name(LOD,Msg -> Name)) != NULL)
        {
            struct IFFHandle *iff;

            if ((iff = AllocIFF()) != NULL)
            {
                if ((iff -> iff_Stream = Open(name,MODE_OLDFILE)) != NULL)
                {
                    InitIFFasDOS(iff);

                    if (!OpenIFF(iff,IFFF_READ))
                    {
                        if (!StopChunk(iff,MAKE_ID('P','R','E','F'),MAKE_ID('F','O','B','J')))
                        {
                            if (!ParseIFF(iff,IFFPARSE_SCAN))
                            {
                                F_Do(LOD -> Dataspace,F_IDO(FM_Dataspace_ReadIFF),iff);
                            }
                        }
                        CloseIFF(iff);
                    }
                    Close(iff -> iff_Stream);
                }
                FreeIFF(iff);
            }

            F_Dispose(name);

            for (node = (FFamilyNode *) F_Get(Obj,FA_Family_Head) ; node ; node = node -> Next)
            {
                F_Do(node -> Object,FM_Import,LOD -> Dataspace,F_IDO(FM_Dataspace_Find));
            }
            return TRUE;
        }
    }
    return FALSE;
}
//+
///App_Save
F_METHODM(void,App_Save,FS_Application_Save)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Dataspace)
    {
        FFamilyNode *node;

        for (node = (FFamilyNode *) F_Get(Obj,FA_Family_Head) ; node ; node = node -> Next)
        {
            F_Do(node -> Object,FM_Export,LOD -> Dataspace,F_IDO(FM_Dataspace_Add));
        }

        app_save_iff(Class,Obj,Msg -> Name);
    }
}
//+
///App_Resolve
F_METHODM(uint32,App_Resolve,FS_Application_Resolve)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> ResolveMap)
    {
        if (((uint32)(Msg -> Name) > 0xFFFF) && (*Msg -> Name == '$'))
        {
            FAppResolveMap *map;
    
            for (map = LOD -> ResolveMap ; map -> Name ; map++)
            {
                if (F_StrCmp(Msg -> Name,map -> Name,ALL) == 0)
                {
                    return (uint32)(map -> Data);
                }
            }
        }
    }

    return F_DoA(LOD -> Preferences,F_IDO(FM_Preference_Resolve),Msg);
}
//+
///App_ResolveInt
F_METHODM(uint32,App_ResolveInt,FS_Application_Resolve)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> ResolveMap)
    {
        if (((uint32)(Msg -> Name) > 0xFFFF) && (*Msg -> Name == '$'))
        {
            FAppResolveMap *map;

            for (map = LOD -> ResolveMap ; map -> Name ; map++)
            {
                if (F_StrCmp(Msg -> Name,map -> Name,ALL) == 0)
                {
                    return (uint32)(map -> Data);
                }
            }
        }
    }

    return F_DoA(LOD -> Preferences,F_IDO(FM_Preference_ResolveInt),Msg);
}
//+
///App_OpenFont
F_METHODM(struct TextFont *,App_OpenFont,FS_Application_OpenFont)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    STRPTR Define = Msg -> Spec;
    struct TextFont *font = NULL;

/*** check inheritance *****************************************************/

    if ((uint32)(Define) == FV_Font_Inherit)
    {
        Define = NULL;

        if ((font = (struct TextFont *) F_Get((FObject) F_Get(Msg -> Object,FA_Parent),FA_Font)) != NULL)
        {
            struct TextAttr ta;

            ta.ta_Name  = font -> tf_Message.mn_Node.ln_Name;
            ta.ta_YSize = font -> tf_YSize;
            ta.ta_Style = font -> tf_Style;
            ta.ta_Flags = font -> tf_Flags;

            font = OpenFont(&ta);
        }
    }

/*** preference item *******************************************************/

    if (!font)
    {
        if ((Define = (STRPTR) F_Do(Obj,FM_Application_Resolve,Define,NULL)) != NULL)
        {
            struct TextAttr ta;

            if (app_create_ta((STRPTR)(Define),&ta))
            {
                font = OpenDiskFont(&ta); F_Dispose(ta.ta_Name);
            }
        }
    }

/*** preference item : FP_Font_Normal **************************************/

    if (!font)
    {
        if ((Define = (STRPTR) F_Do(Obj,FM_Application_Resolve,"$font-normal",NULL)) != NULL)
        {
            struct TextAttr ta;

            if (app_create_ta((STRPTR)(Define),&ta))
            {
                font = OpenDiskFont(&ta); F_Dispose(ta.ta_Name);
            }
        }
    }

/*** last fall back : screen font ******************************************/
 
    if (!font)
    {
        struct Screen *scr;

        if ((scr = (struct Screen *) F_Get(LOD -> Display,(uint32) "FA_Display_Screen")) != NULL)
        {
            font = OpenFont(scr -> Font);
        }
    }

    #ifdef DB_OPENFONT
    F_Log(0,"FONT (%s)(0x%08lx) - YSize %ld",font -> tf_Message.mn_Node.ln_Name,font,font -> tf_YSize);
    #endif

    return font;
}
//+

/*** Preference ************************************************************/

STATIC FPreferenceScript Script[] =
{
    { "$application-scheme",           FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$application-scheme-disabled",  FV_TYPE_STRING,  "Spec",     DEF_DISABLEDSCHEME,0 },
    { "$pophelp-scheme",               FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$font-tiny",                    FV_TYPE_STRING,  "Contents", NULL,0 },
    { "$font-normal",                  FV_TYPE_STRING,  "Contents", NULL,0 },
    { "$font-big",                     FV_TYPE_STRING,  "Contents", NULL,0 },
    { "$font-fixed",                   FV_TYPE_STRING,  "Contents", NULL,0 },
    { "$pophelp-font",                 FV_TYPE_STRING,  "Contents", NULL,0 },

    F_ARRAY_END
};
 
///Prefs_New
F_METHOD(uint32,Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Application",

        "Script",      Script,
        "XMLSource",   "feelin/preference/application.xml",
        
    TAG_MORE,Msg);
}
//+
