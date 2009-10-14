#include "Private.h"

///list_str_cmp
int32 list_str_cmp(STRPTR Str1,STRPTR Str2,uint32 Length,uint32 *LengthPtr)
{
    uint8 c1,c2;
    uint32 initial_length = Length;

    if (LengthPtr)
    {
        *LengthPtr = 0;
    }

    if (!Length)
    {
        return 0;
    }
    else if (!Str1)
    {
        return -1;
    }
    else if (!Str2)
    {
        return 1;
    }

    do
    {
        c1 = ToLower(*Str1++);
        c2 = ToLower(*Str2++);
    }
    while (c1 == c2 && c1 && --Length);

    if (LengthPtr)
    {
        *LengthPtr = initial_length - Length;
    }

    return (int32)(c1) - (int32)(c2);
}
//+

///List_FindString
F_METHODM(uint32,List_FindString,FS_List_FindString)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg -> String)
    {
        FLine *best = NULL;
        FLine *line;
        uint32 best_cmp_len=0;
        int32 best_cmp_val=-255;

        for (line = (FLine *) LOD -> LineList.Head ; line ; line = line -> Next)
        {
            int32 cmp;
            uint32 cmp_len;

            line_display(Class,Obj, line);

            cmp = list_str_cmp(Msg -> String,line -> Strings[0],ALL,&cmp_len);

            if (cmp_len > best_cmp_len)
            {
                best = line; best_cmp_len = cmp_len;
            }
            else if (!best_cmp_len)
            {
                if (cmp < 0 && cmp > best_cmp_val)
                {
                    best = line; best_cmp_val = cmp;
                }
            }
        }

        if (best)
        {
//            F_Log(0,"best (0x%08lx)(%ld)",best,best -> Position);

            return best -> Position;
        }
    }
    return FV_List_FindString_NotFound;
}
//+
