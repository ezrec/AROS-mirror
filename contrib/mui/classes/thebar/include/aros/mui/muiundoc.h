#ifndef MUI_UNDOC_H
#define MUI_UNDOC_H

#include <libraries/mui.h>
#define MUIM_Backfill MUIM_CustomBackfill

struct  MUIP_Backfill
{
    STACKED ULONG MethodID;
    STACKED LONG left;
    STACKED LONG top;
    STACKED LONG right;
    STACKED LONG bottom;
    STACKED LONG xoffset;
    STACKED LONG yoffset;
    STACKED LONG lum;
};

#define MUIA_FrameDynamic                   0x804223c9 /* V20 isg BOOL              */
#define MUIA_FrameVisible                   0x80426498 /* V20 isg BOOL              */

#endif
