/*
    Copyright © 2016, The AROS Development Team. All rights reserved.
    $Id$
*/

/******************************************************************************

    NAME

        DatatypeToCSource

    SYNOPSIS

        FILES/A/M, PACK/S

    LOCATION

        Extras:Multimedia/Gfx/DatatypesToCSource

    FUNCTION

        Load picture files with the datatypes library and write C source
        to stdout. The output can be used by Rawimage MCC.

    INPUTS

        FILES  --  the files which should be converted
	PACK   --  compress the image with bz2 library. Defaults to FALSE

    RESULT

    NOTES

    EXAMPLE
        DatatypeToCSource file1.png file2.png
        DatatypeToCSource ram:#?.png PACK

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/datatypes.h>
#include <proto/alib.h>

#include <datatypes/pictureclass.h>

#include <stdio.h>
#include <stdlib.h>
#include <bzlib.h>

//#define DEBUG 1
#include <aros/debug.h>

static const char version[] __attribute__((used)) = "$VER: DatatypeToCSource 1.0 (15.3.2016)\n";

static void write_source(STRPTR filename, UBYTE *mem, ULONG width, ULONG height, BOOL compress);
static void dt2src(STRPTR filename, BOOL compress);

#define TEMPLATE "FILES/A/M,PACK/S"
#define COMPRESSRATE (9)
#define PATHLEN (1024)

enum
{
    ARG_FILES,
    ARG_PACK,
    ARG_COUNT
};


int main(int argc, char **argv)
{
    STRPTR *filenames = NULL;
    BOOL compress = FALSE;
    struct RDArgs *rda;
    IPTR args[ARG_COUNT] = {0};
    struct AnchorPath *anchorpath;
    LONG error;

    rda = ReadArgs(TEMPLATE, args, NULL);
    if (!rda)
    {
        PrintFault(IoErr(), argv[0]);
        return RETURN_ERROR;
    }

    if (args[ARG_FILES])
    {
        filenames = (STRPTR *)args[ARG_FILES];
    }
    if (args[ARG_PACK])
    {
        compress = TRUE;
    }

    if (anchorpath = AllocMem(sizeof(struct AnchorPath) + PATHLEN, MEMF_ANY))
    {
        while (*filenames)
        {
            memset(anchorpath, 0, sizeof *anchorpath);
            anchorpath->ap_Strlen = PATHLEN;
            anchorpath->ap_BreakBits = SIGBREAKF_CTRL_C;

            if ((error = MatchFirst(*filenames, anchorpath)) == 0)
            {
                do
                {
                    D(bug("name %s\n", anchorpath->ap_Buf));
                    if (anchorpath->ap_Info.fib_DirEntryType < 0) // ignore dirs
                    {
                        dt2src(anchorpath->ap_Buf, compress);
                    }
                } while ((error = MatchNext(anchorpath)) == 0);
            }
            MatchEnd(anchorpath);

            if (error != ERROR_NO_MORE_ENTRIES)
            {
                PrintFault(error, argv[0]);
            }
            filenames++;
        }
        FreeMem(anchorpath, sizeof(struct AnchorPath) + PATHLEN);
    }

    if (rda)
    {
        FreeArgs(rda);
    }
    return RETURN_OK;
}


static void dt2src(STRPTR filename, BOOL compress)
{
    UBYTE *mem;
    Object *obj;
    struct BitMapHeader *bmhd = NULL;
    struct pdtBlitPixelArray bpa_msg;
    ULONG width;
    ULONG height;

    obj = NewDTObject(filename,
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
            D(bug("width %d height %d depth %d\n", width, height, bmhd->bmh_Depth));
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
                DoMethodA(obj, (Msg)&bpa_msg);

                write_source(filename, mem, width, height, compress);

                FreeVec(mem);
            }
            else
            {
                fputs("Can't allocate memory", stderr);
            }
        }
        DisposeDTObject(obj);
    }
    else
    {
        fprintf(stderr, "Can't create picture datatype object from file %s\n", filename);
    }
}


static void write_source(STRPTR filename, UBYTE *mem, ULONG width, ULONG height, BOOL compress)
{
    ULONG srclen = width * height * 4;;
    ULONG i;

    printf("// Created from file %s\n\n", filename);
    if (compress)
    {
        unsigned int destlen = srclen * 1.1 + 600;
        unsigned char *dest = malloc(destlen);
        if (dest)
        {
            int compressresult = BZ2_bzBuffToBuffCompress(dest, &destlen,
                mem, srclen,
                COMPRESSRATE,
                0,
                30);
            if (compressresult == BZ_OK)
            {
                puts("const unsigned char img[] =\n{");

                printf("    0x%02x, 0x%02x, 0x%02x, 0x%02x,  // width\n",
                    (width & 0xff000000) >> 24, (width & 0xff0000) >> 16, (width & 0xff00) >> 8, (width & 0xff));
                printf("    0x%02x, 0x%02x, 0x%02x, 0x%02x,  // height\n",
                    (height & 0xff000000) >> 24, (height & 0xff0000) >> 16, (height & 0xff00) >> 8, (height & 0xff));
                puts("    'B', 'Z', '2', '\\0',");
                printf("    0x%02x, 0x%02x, 0x%02x, 0x%02x,  // number of bytes\n",
                    (destlen & 0xff000000) >> 24, (destlen & 0xff0000) >> 16, (destlen & 0xff00) >> 8, (destlen & 0xff));

                for (i = 0; i < destlen; i++)
                {
                    if ((i % 12) == 0)
                    {
                        printf("\n    ");
                    }
                    printf("0x%02x, ", dest[i]);
                }
                puts("\n};\n");
            }
            else
            {
                fprintf(stderr, "BZ2_bzBuffToBuffCompress returned error %d\n", compressresult);
            }
            free(dest);
        }
        else
        {
            fputs("Can't allocate memory for compressing", stderr);
        }
    }
    else
    {
        puts("const unsigned char img[] =\n{");

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
        puts("\n};\n");
    }
}
