#include "Private.h"

/*** Methods ***************************************************************/

///Balance_New
F_METHOD(uint32,Balance_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    return F_SUPERDO();
}
//+
///Balance_Set
F_METHOD(void,Balance_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg, item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Balance_QuickDraw:
        {
            if (item.ti_Data) LOD -> Flags |=  FF_Balance_QuickDraw;
            else LOD -> Flags &= ~FF_Balance_QuickDraw;
        }
        break;
    }

    F_SUPERDO();
}
//+

///Balance_Import
F_METHODM(uint32,Balance_Import,FS_Import)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        uint16 *data = (uint16 *) F_Do(Msg -> Dataspace,Msg -> id_Find,id);

        if (data)
        {
            FAreaNode *area;

            for (area = (FAreaNode *) F_Get(_parent,FA_Family_Head) ; area ; area = area -> Next)
            {
                if (area -> Object != Obj)
                {
                    if (*data)  _area_weight = *data++;
                    else                        data++;
                }
            }
        }
        return TRUE;
    }
    return F_SUPERDO();
}
//+
///Balance_Export
F_METHODM(uint32,Balance_Export,FS_Export)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        FAreaNode *node;
        uint32 n=0;

        for (node = (FAreaNode *) F_Get(_parent,FA_Family_Head) ; node ; node = node -> Next)
        {
            n++;
        }

        if (n > 1)
        {
            uint32 size = (n - 1) * sizeof (uint16);
            uint16 *back = F_New(size);

            if (back)
            {
                uint16 *data = back;

                for (node = (FAreaNode *) F_Get(_parent,FA_Family_Head) ; node ; node = node -> Next)
                {
                    if (node -> Object != Obj)
                    {
                        *data++ = node -> AreaPublic -> Weight;
                    }
                }

                F_Do(Msg -> Dataspace,Msg -> id_Add,id,back,size);

                F_Dispose(back);
            }
        }
        return TRUE;
    }
    return F_SUPERDO();
}
//+

