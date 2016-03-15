/*
    Copyright © 2016, The AROS Development Team. All rights reserved.
    $Id$
*/

/*********************************************************************************************/

#include <proto/exec.h>
#include <proto/datatypes.h>
#include <proto/alib.h>
#include <proto/bz2.h>

#include <datatypes/pictureclass.h>

#include <stdio.h>
#include <stdlib.h>

static const char version[] __attribute__((used)) = "$VER: DatatypeToCSource 1.0 (14.3.2016)\n";

static void write_source(UBYTE *mem, ULONG width, ULONG height, BOOL pack);

#define TEMPLATE "FILE/A PACK/S"

enum
{
    ARG_FILE,
    ARG_PACK,
    ARG_COUNT
};

BOOL compressed = TRUE;

int main(int argc, char **argv)
{
    TEXT *file = "sys:test.png";
    UBYTE *mem;
    Object *obj;
    struct BitMapHeader *bmhd = NULL;
    struct pdtBlitPixelArray bpa_msg;
    ULONG width;
    ULONG height;

    obj = NewDTObject(file,
        DTA_GroupID, GID_PICTURE,
        PDTA_DestMode, PMODE_V43,
        TAG_DONE);
    if (obj)
    {
        GetDTAttrs(obj,
            PDTA_BitMapHeader,&bmhd,
            TAG_END);
        if (bmhd)
        {
            width = bmhd->bmh_Width;
            height = bmhd->bmh_Height;
            printf("width %d height %d depth %d\n", width, height, bmhd->bmh_Depth);
            mem = AllocVec(width * height * 4, MEMF_CLEAR);
            if (mem)
            {
                bpa_msg.MethodID = PDTM_READPIXELARRAY;
                bpa_msg.pbpa_PixelData = mem;
                bpa_msg.pbpa_PixelFormat = PBPAFMT_ARGB;
                bpa_msg.pbpa_PixelArrayMod = width * 4;
                bpa_msg.pbpa_Left = 0;
                bpa_msg.pbpa_Top = 0;
                bpa_msg.pbpa_Width = width;
                bpa_msg.pbpa_Height = height;

                printf("result PDTM_READPIXELARRAY %lu\n", DoMethodA(obj, (Msg)&bpa_msg));

                write_source(mem, width, height, compressed);

                FreeVec(mem);
            }
            else
            {
                puts("Can't allocate memory");
            }
        }
        DisposeDTObject(obj);
    }
    else
    {
        printf("Can't create picture datatype object from file %s\n", file);
    }

    return 0;
}

static void write_source(UBYTE *mem, ULONG width, ULONG height, BOOL compress)
{
    ULONG srclen = width * height * 4;;
    ULONG i;

    if (compress)
    {
        unsigned int destlen = srclen * 1.1 + 600;
        unsigned char *dest = malloc(destlen);
        if (dest)
        {
            int compressresult = BZ2_bzBuffToBuffCompress(dest, &destlen,
                mem, srclen,
                9,  // 1 to 9
                4,  // 0 to 4
                30);
            printf("BZ2_bzBuffToBuffCompress result %d\n", compressresult);
            if (compressresult == BZ_OK)
            {
                puts("UBYTE img[] = {");

                printf("    0x%02x, 0x%02x, 0x%02x, 0x%02x,  // width\n",
                    (width & 0xff000000) >> 24, (width & 0xff0000) >> 16, (width & 0xff00) >> 8, (width & 0xff));
                printf("    0x%02x, 0x%02x, 0x%02x, 0x%02x,  // height\n",
                    (height & 0xff000000) >> 24, (height & 0xff0000) >> 16, (height & 0xff00) >> 8, (height & 0xff));
                puts("    'B', 'Z', '2', '\\0',");
                printf("    0x%02x, 0x%02x, 0x%02x, 0x%02x,  // // number of bytes\n",
                    (destlen & 0xff000000) >> 24, (destlen & 0xff0000) >> 16, (destlen & 0xff00) >> 8, (destlen & 0xff));

                for (i = 0; i < destlen; i++)
                {
                    if ((i % 12) == 0)
                    {
                        printf("\n    ");
                    }
                    printf("0x%02x, ", dest[i]);
                }
                puts("\n};");
            }
            free(dest);
        }
        else
        {
            puts("Can't allocate memory for compressing");
        }
    }
    else
    {
        puts("UBYTE img[] = {");

        printf("    0x%02x, 0x%02x, 0x%02x, 0x%02x,  // width\n",
            (width & 0xff000000) >> 24, (width & 0xff0000) >> 16, (width & 0xff00) >> 8, (width & 0xff));
        printf("    0x%02x, 0x%02x, 0x%02x, 0x%02x,  // height\n",
            (height & 0xff000000) >> 24, (height & 0xff0000) >> 16, (height & 0xff00) >> 8, (height & 0xff));
        puts("    0x00, 0x00, 0x00, 0x00,");
        puts("    0x00, 0x00, 0x00, 0x00,  // number of bytes");

        for (i = 0; i < srclen; i++)
        {
            if ((i % 12) == 0)
            {
                printf("\n    ");
            }
            printf("0x%02x, ", mem[i]);
        }
        puts("\n};");
    }
}
