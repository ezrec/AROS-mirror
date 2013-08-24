/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#define CATCOMP_ARRAY
#include "class.h"

/***********************************************************************/

struct Catalog *
openCatalogVR(CONST_STRPTR name,ULONG minVer,ULONG minRev)
{
    if ((lib_cat = OpenCatalog(NULL,(STRPTR)name,OC_BuiltInLanguage,(IPTR)"english",TAG_DONE)))
    {
        ULONG ver = lib_cat->cat_Version;

        if ((ver<minVer) ? TRUE : ((ver==minVer) ? (lib_cat->cat_Revision<minRev) : FALSE))
        {
            CloseCatalog(lib_cat);
            lib_cat = NULL;
        }
    }

    return lib_cat;
}

/***********************************************************************/

STRPTR
tr(ULONG id)
{
    int low, high;

    for (low = 0, high = (sizeof(CatCompArray)/sizeof(CatCompArray[0]))-1; low<=high; )
    {
        int                     mid = (low+high) / 2, cond;
        struct CatCompArrayType *cca = (struct CatCompArrayType *)CatCompArray+mid;

        cond = id - cca->cca_ID;

        if(cond == 0)
            return lib_cat ? (STRPTR)GetCatalogStr(lib_cat, id, cca->cca_Str) : cca->cca_Str;
        else if(cond<0)
            high = mid-1;
        else
            low = mid+1;
    }

    return (STRPTR)"";
}

/****************************************************************************/

#define IDSSIZE(ids) (sizeof(ids)/sizeof(ids[0]))

static const ULONG regIDs[] =
{
    Msg_Reg_Colors,
    Msg_Reg_Appearance,
    Msg_Reg_Spacing,
    Msg_Reg_Options,
    0
};
STRPTR regs[IDSSIZE(regIDs)];

static const ULONG frameIDs[] =
{
    Msg_FrameStyle_Recessed,
    Msg_FrameStyle_Normal,
    0
};
STRPTR frames[IDSSIZE(frameIDs)];

static const ULONG precisionIDs[] =
{
    Msg_Precision_Gui,
    Msg_Precision_Icon,
    Msg_Precision_Image,
    Msg_Precision_Exact,
    0
};
STRPTR precisions[IDSSIZE(precisionIDs)];

static const ULONG dismodeIDs[] =
{
    Msg_DisMode_Shape,
    Msg_DisMode_Grid,
    Msg_DisMode_FullGrid,
    Msg_DisMode_Sunny,
    Msg_DisMode_Blend,
    Msg_DisMode_BlendGrey,
    0
};
STRPTR dismodes[IDSSIZE(dismodeIDs)];

static const ULONG spacersSizeIDs[] =
{
    Msg_SpacersSize_Quarter,
    Msg_SpacersSize_Half,
    Msg_SpacersSize_One,
    Msg_SpacersSize_None,
    Msg_SpacersSize_OnePoint,
    Msg_SpacersSize_TwoPoint,
    0
};
STRPTR spacersSizes[IDSSIZE(spacersSizeIDs)];

static const ULONG viewModeIDs[] =
{
    Msg_TextGfx,
    Msg_Gfx,
    Msg_Text,
    0
};
STRPTR viewModes[IDSSIZE(viewModeIDs)];

static const ULONG labelPosIDs[] =
{
    Msg_LabelPos_Bottom,
    Msg_LabelPos_Top,
    Msg_LabelPos_Right,
    Msg_LabelPos_Left,
    0
};
STRPTR labelPoss[IDSSIZE(labelPosIDs)];

void
localizeArray(STRPTR *strings, const ULONG *ids)
{
    while (*ids)
        *strings++ = tr(*ids++);
    // make sure the translated string array is NULL terminated
    *strings = NULL;
}

void
initStrings(void)
{
    if (openCatalogVR((CONST_STRPTR)"TheBar_mcp.catalog",0,0))
    {
        struct CatCompArrayType *cca;
        int                     cnt;

        for (cnt = (sizeof(CatCompArray)/sizeof(CatCompArray[0]))-1, cca = (struct CatCompArrayType *)CatCompArray+cnt;
             cnt>=0;
             cnt--, cca--)
        {
            STRPTR s;

            if ((s = (STRPTR)GetCatalogStr(lib_cat, cca->cca_ID, cca->cca_Str)))
                cca->cca_Str = s;
        }
    }

    localizeArray(regs,regIDs);
    localizeArray(frames,frameIDs);
    localizeArray(precisions,precisionIDs);
    localizeArray(dismodes,dismodeIDs);
    localizeArray(spacersSizes,spacersSizeIDs);
    localizeArray(viewModes,viewModeIDs);
    localizeArray(labelPoss,labelPosIDs);

    #if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
    dismodes[4] = NULL;
    #endif
}

/***********************************************************************/

ULONG
getKeyChar(STRPTR string)
{
    ULONG res = 0;

    if (string)
    {
        for (; *string && *string!='_'; string++);
        if (*string++)
            res = ToLower(*string);
    }

    return res;
}

/***********************************************************************/

