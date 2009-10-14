#include "Private.h"

//#define DB_TILESIMPLE
//#define DB_TILECOMPLEX

///typedef

enum    {

        FV_PICTURE_MODE_NOREPEAT,
        FV_PICTURE_MODE_REPEAT,
        FV_PICTURE_MODE_SCALE

        };

typedef struct in_Image_Picture
{
    FObject                         Pic;

    uint32                          width; // *must* be uint32
    uint32                          height; // *must* be uint32

    uint32                          mode;
    uint32                          filter;
}
FImage;
//+

/****************************************************************************
*** picture *****************************************************************
****************************************************************************/

///id_picture_create
STATIC F_CODE_CREATE_XML(id_picture_create)
{
    STATIC FDOCValue xml_values_mode[] =
    {
        {"no-repeat",    FV_PICTURE_MODE_NOREPEAT},
        {"repeat",       FV_PICTURE_MODE_REPEAT},
        {"scale",        FV_PICTURE_MODE_SCALE},

        F_ARRAY_END
    };

    STATIC FDOCValue xml_values_filter[] =
    {
        {"nearest",      0},
        {"bilinear",     1},
        {"average",      2},

        F_ARRAY_END
    };

    STRPTR src=NULL;
    uint32 mode=FV_PICTURE_MODE_NOREPEAT;
    uint32 filter=0;

    for ( ; Attribute ; Attribute = Attribute -> Next)
    {
        switch (Attribute -> Name -> ID)
        {
            case FV_XML_ID_SRC:  src = Attribute -> Data; break;
            case FV_XML_ID_MODE: mode = F_Do(CUD -> XMLDocument,F_ID(CUD -> IDs,FM_Document_Resolve),Attribute -> Data,FV_TYPE_INTEGER,xml_values_mode,NULL); break;
            case FV_XML_ID_FILTER: filter = F_Do(CUD->XMLDocument, F_ID(CUD->IDs, FM_Document_Resolve), Attribute->Data, FV_TYPE_INTEGER, xml_values_filter, NULL); break;
        }
    }

    if (src)
    {
        FImage *image = F_NewP(CUD -> Pool,sizeof (FImage));

        if (image)
        {
            image -> Pic = PictureObject,

                "FA_Picture_Name",    src,

                End;

            if (image -> Pic)
            {
                uint32 w,h;

                F_Do(image -> Pic,FM_Get,

                    "FA_BitMap_Width",  &w,
                    "FA_BitMap_Height", &h,

                    TAG_DONE);

//                F_Log(0,"pic (0x%08lx)(%s) (%ld x %ld)",image->Pic,src,w,h);

                image -> width = w;
                image -> height = h;
                image -> mode = mode;
                image->filter = filter;

                return image;
            }

            F_Dispose(image);
        }
    }
    return NULL;
}
//+
///id_picture_delete
STATIC F_CODE_DELETE(id_picture_delete)
{
   F_DisposeObj(image -> Pic);
   F_Dispose(image);
}
//+
///id_picture_setup
STATIC F_CODE_SETUP(id_picture_setup)
{
    /*
    return (int32) F_Do(image -> Pic,F_IDO(FM_BitMap_Render),

        F_IDO(FA_BitMap_TargetScreen), F_Get(Render -> Display, F_IDO(FA_Display_Screen)),
        F_IDO(FA_BitMap_RenderedWidth), &image -> width,
        F_IDO(FA_BitMap_RenderedHeight), &image -> height,

        TAG_DONE);
    */

    return TRUE;
}
//+
///id_picutre_get
STATIC F_CODE_GET(id_picture_get)
{
    int32 rc=0;

    switch (Attribute)
    {
        case FA_ImageDisplay_Height:
        {
           rc = image -> height;
        }
        break;

        case FA_ImageDisplay_Width:
        {
           rc = image -> width;
        }
        break;

        case FA_ImageDisplay_Mask:
        {
//            F_Log(0,"FA_ImageDisplay_Mask");

            rc = TRUE;
        }
        break;
    }
    return rc;
}
//+
///id_picture_draw
STATIC F_CODE_DRAW(id_picture_draw)
{
    FBox b;

    uint32 px=0,py=0;

    STRPTR mode = "Tile";

    b.x = Msg -> Rect -> x1;
    b.y = Msg -> Rect -> y1;
    b.w = Msg -> Rect -> x2 - Msg -> Rect -> x1 + 1;
    b.h = Msg -> Rect -> y2 - Msg -> Rect -> y1 + 1;

    switch (image->mode)
    {
        case FV_PICTURE_MODE_NOREPEAT:
        {
//            int32 fill = FALSE;
__no_repeat:

            if (image->width < b.w)
            {
                b.x = (b.w - image->width) / 2 + b.x;
                b.w = image->width;

//                fill = TRUE;
            }

            if (image->height < b.h)
            {
                b.y = (b.h - image->height) / 2 + b.y;
                b.h = image->height;

//                fill = TRUE;
            }

            #if 0
            if (fill)
            {
                SetAPen(Msg->Render->RPort, 3);
                RectFill(Msg->Render->RPort, Msg->Rect->x1, Msg->Rect->y1, Msg->Rect->x2, Msg->Rect->y2);
            }
            #endif

            mode = "Copy";
        }
        break;

        case FV_PICTURE_MODE_SCALE:
        {
            if (Msg -> Origin)
            {
/*
                if ((image -> width != Msg -> Origin -> w) ||
                    (image -> height != Msg -> Origin -> h))
                {
                    F_Do(image -> Pic,F_IDO(FM_BitMap_Render),

                        F_IDO(FA_BitMap_TargetScreen), F_Get(Msg -> Render -> Display, F_IDO(FA_Display_Screen)),
                        F_IDO(FA_BitMap_TargetWidth), Msg -> Origin -> w,
                        F_IDO(FA_BitMap_TargetHeight), Msg -> Origin -> h,
                        F_IDO(FA_BitMap_RenderedWidth), &image -> width,
                        F_IDO(FA_BitMap_RenderedHeight), &image -> height,

                        TAG_DONE);
                }
*/
                mode = "Scale";
            }
            else
            {

                /* We *need* origins to scale the picture, if we don't have
                them we only perform a regular blit */

                goto __no_repeat;

            }
        }

        case FV_PICTURE_MODE_REPEAT:
        {
            if (Msg -> Origin)
            {
                px = (b.x - Msg -> Origin -> x) % image -> width;
                py = (b.y - Msg -> Origin -> y) % image -> height;
            }
        }
        break;
    }

//    F_Log(0,"(%3ld : %3ld) TO (%3ld : %3ld) (%3ld x %3ld)",px,py, b.x,b.y,b.w,b.h);

    F_Do(image -> Pic, F_IDO(FM_BitMap_Blit),

        Msg -> Render -> RPort,

        px, py, &b,

        "FA_BitMap_BlitMode", mode,
        "FA_BitMap_ScaleFilter", image->filter,

        TAG_DONE);

    return TRUE;
}
//+

struct in_CodeTable id_picture_table =
{
   (in_Code_Create *)   &id_picture_create,
   (in_Code_Delete *)   &id_picture_delete,
   (in_Code_Setup *)    &id_picture_setup,
    NULL,
   (in_Code_Get *)      &id_picture_get,
   (in_Code_Draw *)     &id_picture_draw
};
