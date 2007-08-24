#include "Private.h"

/*** Methods ***************************************************************/

///String_AskMinMax
F_METHOD(uint32,String_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _minw += _font -> tf_XSize * 4;
    _minh += _font -> tf_YSize;

    return F_SUPERDO();
}
//+
///String_GoActive
F_METHOD(void,String_GoActive)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_Get(Obj,FA_Selected)) return; // Already active

    F_Do(Obj,FM_ModifyHandler,(LOD -> BlinkSpeed) ? FF_EVENT_TICK | FF_EVENT_KEY : FF_EVENT_KEY,0);
    F_Set(Obj,FA_Selected,TRUE);
}
//+
///String_GoInactive
F_METHOD(void,String_GoInactive)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(Obj,FM_ModifyHandler,0,(LOD -> BlinkSpeed) ? FF_EVENT_TICK | FF_EVENT_KEY : FF_EVENT_KEY);
    F_Set(Obj,FA_Selected,FALSE);
}
//+

