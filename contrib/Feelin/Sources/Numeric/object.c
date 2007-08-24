#include "Private.h"

/*** Methods ***************************************************************/

///Numeric_New
F_METHOD(uint32,Numeric_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;
 
    LOD->Min    = 0;
    LOD->Max    = 100;
    LOD->Steps  = 10;
    LOD->Format = "%ld";
    LOD->String = F_New(64);

    if (LOD->String)
    {
        return F_SUPERDO();
    }
    else
    {
        F_Log(FV_LOG_USER,"Unable to allocate Stringify Buffer");
    }
    return NULL;
}
//+
///Numeric_Dispose
F_METHOD(void,Numeric_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Dispose(LOD->String); LOD->String = NULL;
    F_SUPERDO();
}
//+
///Numeric_Set
F_METHOD(void,Numeric_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;

    BOOL check=FALSE;
 
    while  ((tag = F_DynamicNTI(&Tags,&item,Class)) != NULL)
    switch (item.ti_Tag)
    {
        case FA_Numeric_Default: LOD->Default = (int32)(item.ti_Data); break;
        case FA_Numeric_Min: LOD->Min = (int32)(item.ti_Data); check=TRUE; break;
        case FA_Numeric_Max: LOD->Max = (int32)(item.ti_Data); check=TRUE; break;
        case FA_Numeric_Steps: LOD->Steps = (int32)(item.ti_Data); break;
        case FA_Numeric_Format: LOD->Format = (STRPTR)item.ti_Data; break;
        case FA_Numeric_ComputeHook: LOD->ComputeHook = (APTR)(item.ti_Data); break;
        case FA_Numeric_StringArray: LOD->StringArray = (STRPTR *) item.ti_Data; break;
 
        case FA_Numeric_Value:
        {
            int32 val = (int32)(item.ti_Data);

            if (LOD->ComputeHook)
            {
                val = CallHookPkt(LOD->ComputeHook,Obj,&val);
            }

            if (val < LOD->Min) val = LOD->Min;
            if (val > LOD->Max) val = LOD->Max;

            tag->ti_Data = val;

            if (LOD->Value != val)
            {
                LOD->Value = val;

                if (LOD->Format || LOD->StringArray)
                {
                   F_Do(Obj, F_IDM(FM_Numeric_Stringify), (uint32)(val));
                }
                
                F_Draw(Obj, FF_Draw_Update);
            }
        }
        break;
    }

    F_SUPERDO();
    
    if (check)
    {
        if (LOD->Value < LOD->Min)
        {
            F_Set(Obj,F_IDA(FA_Numeric_Value),LOD->Min);
        }
        if (LOD->Value > LOD->Max)
        {
            F_Set(Obj,F_IDA(FA_Numeric_Value),LOD->Max);
        }
    }
}
//+
///Numeric_Get
F_METHOD(void,Numeric_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Numeric_Default:   F_STORE(LOD->Default); break;
        case FA_Numeric_Value:     F_STORE(LOD->Value); break;
        case FA_Numeric_Min:       F_STORE(LOD->Min); break;
        case FA_Numeric_Max:       F_STORE(LOD->Max); break;
        case FA_Numeric_Step:      F_STORE(MAX(1,(LOD->Max - LOD->Min) / LOD->Steps)); break;
        case FA_Numeric_Buffer:      F_STORE(LOD->String); break;
        case FA_Numeric_StringArray: F_STORE(LOD->StringArray); break;
    }

    F_SUPERDO();
}
//+
///Numeric_Import
F_METHODM(void,Numeric_Import,FS_Import)
{
    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        int32 * data = (int32 *) F_Do(Msg->Dataspace,Msg->id_Find,id);
  
        if (data)
        {
            F_Set(Obj,F_IDA(FA_Numeric_Value),*data);
        }
    }
}
//+
///Numeric_Export
F_METHODM(void,Numeric_Export,FS_Export)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        F_Do(Msg->Dataspace,Msg->id_Add,id,&LOD->Value,sizeof (int32));
    }
}
//+
