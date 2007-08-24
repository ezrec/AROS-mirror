#include "Private.h"

/*** Methods ***************************************************************/

///Numeric_Reset
F_METHOD(int32,Numeric_Reset)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Set(Obj, F_IDA(FA_Numeric_Value), LOD->Default);

    return LOD->Value;
}
//+
///Numeric_Increase
F_METHODM(int32,Numeric_Increase,FS_Numeric_Increase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Set(Obj, F_IDA(FA_Numeric_Value), LOD->Value + Msg->Value);

    return LOD->Value;
}
//+
///Numeric_Decrease
F_METHODM(int32,Numeric_Decrease,FS_Numeric_Decrease)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Set(Obj, F_IDA(FA_Numeric_Value), LOD->Value - Msg->Value);

    return LOD->Value;
}
//+
///Numeric_Stringify
F_METHODM(STRPTR,Numeric_Stringify,FS_Numeric_Stringify)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD->StringArray)
    {
        F_StrFmt(LOD->String, "%s", LOD->StringArray[Msg->Value]);
        
        return LOD->String;
    }
    else if (LOD->Format)
    {
        F_StrFmt(LOD->String, LOD->Format, Msg->Value);

       return LOD->String;
    }
    return NULL;
}
//+

///Numeric_HandleEvent
F_METHODM(uint32,Numeric_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg->Event->Key)
    {
        uint32 step = MAX(1,(LOD->Max - LOD->Min) / LOD->Steps);
        uint32 val;

        if (F_Get(Obj, FA_Horizontal))
        {
            switch (Msg->Event->Key)
            {
                case FV_KEY_LEFT:       val = LOD->Value - 1;    break;
                case FV_KEY_RIGHT:      val = LOD->Value + 1;    break;
                case FV_KEY_STEPLEFT:   val = LOD->Value - step; break;
                case FV_KEY_STEPRIGHT:  val = LOD->Value + step; break;
                case FV_KEY_FIRST:      val = LOD->Min;          break;
                case FV_KEY_LAST:       val = LOD->Max;          break;
                default:                return 0;
            }
        }
        else
        {
            switch (Msg->Event->Key)
            {
                case FV_KEY_UP:         val = LOD->Value - 1;    break;
                case FV_KEY_DOWN:       val = LOD->Value + 1;    break;
                case FV_KEY_STEPUP:     val = LOD->Value - step; break;
                case FV_KEY_STEPDOWN:   val = LOD->Value + step; break;
                case FV_KEY_TOP:        val = LOD->Min;          break;
                case FV_KEY_BOTTOM:     val = LOD->Max;          break;
                default:                return 0;
            }
        }

        F_Set(Obj, F_IDA(FA_Numeric_Value), val);

        return FF_HandleEvent_Eat;
    }
    else if (Msg->Event->Class == FF_EVENT_BUTTON)
    {
        if (Msg->Event->Code == FV_EVENT_BUTTON_WHEEL)
        {
            if (_inside(Msg->Event->MouseX,_ix,_ix2) &&
                _inside(Msg->Event->MouseY,_iy,_iy2))
            {
                uint32 step = MAX(1,(LOD->Max - LOD->Min) / LOD->Steps);
 
                if (FF_EVENT_BUTTON_DOWN & Msg->Event->Flags)
                {
                    F_Do(Obj,F_IDM(FM_Numeric_Increase),step);
                }
                else
                {
                    F_Do(Obj,F_IDM(FM_Numeric_Decrease),step);
                }
            }
        }
    }
    return 0;
}
//+
