/*
    Copyright © 2011, Thore Böckelmann. All rights reserved.
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "symbols.h"

#include "Rawimage_mcc.h"

#define CLASS       MUIC_Rawimage
#define SUPERCLASS  MUIC_Pixmap

#define INSTDATA        Data

#define USEDCLASSESP used_mcps
static const char *used_mcps[] = { NULL };

struct Data
{
    int dummy;
};

/******************************************************************************/
/* define the functions used by the startup code ahead of including mccinit.c */
/******************************************************************************/

#define COPYRIGHT "(c) 2011 Thore Böckelmann"
#include "copyright.h"

#define VERSION   20
#define REVISION 18
#define LIBDATE  "09.11.2011"
#define USERLIBID CLASS " " STR(VERSION)"."STR(REVISION) " (" LIBDATE ")" FULLVERS

#define libfunc(x)  Rawimage_Lib ## x
#define reqfunc     muilink_use_Rawimage
#define addname(x)  Rawimage_ ## x

#define USE_INTUITIONBASE

#include "mccinit.c"

/* ------------------------------------------------------------------------- */

#if !defined(__MORPHOS__) // MorphOS has it`s own DoSuperNew Method in amiga.lib
static Object * STDARGS VARARGS68K DoSuperNew(struct IClass *cl, Object * obj, ...)
{
    Object *rc;
    VA_LIST args;

    VA_START(args, obj);
    rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
    VA_END(args);

    return rc;
}
#endif

/* ------------------------------------------------------------------------- */

static BOOL setRawimage(struct IClass *cl, Object *obj, struct MUI_RawimageData *rdata)
{
    BOOL success = FALSE;
    LONG format;
    LONG compression;

    switch(rdata->ri_Format)
    {
        case RAWIMAGE_FORMAT_RAW_ARGB_ID:
            format = MUIV_Pixmap_Format_ARGB32;
            compression = MUIV_Pixmap_Compression_None;
            break;

        case RAWIMAGE_FORMAT_RAW_RGB_ID:
            format = MUIV_Pixmap_Format_RGB24;
            compression = MUIV_Pixmap_Compression_None;
            break;

        case RAWIMAGE_FORMAT_BZ2_ARGB_ID:
            format = MUIV_Pixmap_Format_ARGB32;
            compression = MUIV_Pixmap_Compression_BZip2;
            break;

        case RAWIMAGE_FORMAT_BZ2_RGB_ID:
            format = MUIV_Pixmap_Format_RGB24;
            compression = MUIV_Pixmap_Compression_BZip2;
            break;

        case RAWIMAGE_FORMAT_RLE_ARGB_ID:
            format = MUIV_Pixmap_Format_ARGB32;
            compression = MUIV_Pixmap_Compression_RLE;
            break;

        case RAWIMAGE_FORMAT_RLE_RGB_ID:
            format = MUIV_Pixmap_Format_RGB24;
            compression = MUIV_Pixmap_Compression_RLE;
            break;

        default:
            format = -1;
            compression = MUIV_Pixmap_Compression_None;
            break;
    }

    if(format != -1)
    {
        // Pixmap.mui will return failure in case the compression is not supported
        success = SetSuperAttrs(cl, obj, MUIA_FixWidth, rdata->ri_Width,
                                         MUIA_FixHeight, rdata->ri_Height,
                                         MUIA_Pixmap_Width, rdata->ri_Width,
                                         MUIA_Pixmap_Height, rdata->ri_Height,
                                         MUIA_Pixmap_Format, format,
                                         MUIA_Pixmap_Data, rdata->ri_Data,
                                         MUIA_Pixmap_Compression, compression,
                                         MUIA_Pixmap_CompressedSize, rdata->ri_Size,
                                         TAG_DONE);
    }

    return success;
}

static ULONG ASM Rawimage_New(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct opSet *msg))
{
    if((obj = (Object *)DoSuperNew(cl, obj,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct MUI_RawimageData *rdata;

        if((rdata = (struct MUI_RawimageData *)GetTagData(MUIA_Rawimage_Data, (ULONG)NULL, msg->ops_AttrList)) != NULL)
        {
            if(setRawimage(cl, obj, rdata) == FALSE)
            {
                CoerceMethod(cl, obj, OM_DISPOSE);
                obj = NULL;
            }
        }
    }

    return((ULONG)obj);
}


static ULONG ASM Rawimage_Set(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct opSet *msg))
{
    struct MUI_RawimageData *rdata;

    if((rdata = (struct MUI_RawimageData *)GetTagData(MUIA_Rawimage_Data, (ULONG)NULL, msg->ops_AttrList)) != NULL)
    {
        setRawimage(cl, obj, rdata);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}


DISPATCHER(_Dispatcher)
{
    switch (msg->MethodID)
    {
        case OM_NEW             : return(Rawimage_New        (cl,obj,(APTR)msg));
        case OM_SET             : return(Rawimage_Set        (cl,obj,(APTR)msg));
    }

    return(DoSuperMethodA(cl,obj,msg));
}

