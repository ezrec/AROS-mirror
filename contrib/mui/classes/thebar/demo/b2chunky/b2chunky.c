
#define __USE_SYSBASE
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <proto/locale.h>

#include <clib/alib_protos.h>
#include <clib/debug_protos.h>

#include <graphics/rpattr.h>
#include <graphics/gfxmacros.h>
#include <datatypes/pictureclass.h>

#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "b2chunky_rev.h"

/***********************************************************************/

char __ver[] = VERSTAG;
long __stack = 8192;

#define ABS(x) __builtin_abs(x)

/* brc1.c */
ULONG BRCPack ( char *pSource , char *pDest , LONG rowSize );

#define TEMPLATE "FROM/A,TO/K,NAME/K,TC=TRUECOLOR/S,NTC=NTCOMPRESS/S,TR=TRCOLOR/K,TRI=TRCOLORINDEX/K/N,AM=ALPHAMASK/S,RGBC=RGBCOLS/S"

struct args
{
    STRPTR from;
    STRPTR to;
    STRPTR name;
    ULONG  tc;
    ULONG  ntc;
    STRPTR tr;
    ULONG  *tri;
    ULONG  alpha;
    ULONG  RGB;
};

/***********************************************************************/

int
main(int argc,char **argv)
{
    struct args            args = {0};
    register struct RDArgs *ra;
    register BPTR          err;
    register ULONG         closeErr, errc = 0;
    register int           res = RETURN_FAIL;

    if (err = Open("CONSOLE:",MODE_NEWFILE)) closeErr = TRUE;
    else
    {
        err = Output();
        closeErr = FALSE;
    }

    if (ra = ReadArgs(TEMPLATE,(LONG *)&args,NULL))
    {
        register struct GfxBase *GfxBase;

        if (GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",39))
        {
            register struct Library *DataTypesBase;

            if (DataTypesBase = OpenLibrary("datatypes.library",39))
            {
                register Object *dto;

                FPrintf(err,"Loading file...\n");

                if (dto = NewDTObject((STRPTR)args.from,DTA_GroupID,GID_PICTURE,PDTA_Remap,FALSE,PDTA_DestMode,PMODE_V43,TAG_DONE))
                {
                    struct BitMapHeader *bmh;
                    ULONG               *colors, numColors;

                    if (GetDTAttrs(dto,PDTA_CRegs,&colors,PDTA_NumColors,&numColors,PDTA_BitMapHeader,&bmh,TAG_DONE)==3)
                    {
                        register UBYTE *chunky;
                        register ULONG size = bmh->bmh_Width*bmh->bmh_Height, r;

                        if (args.tc) size *= 4;

                        if (chunky = AllocVec(args.ntc ? size : size+size,MEMF_ANY))
                        {
                            if (args.tc) r = DoMethod(dto,PDTM_READPIXELARRAY,chunky,PBPAFMT_ARGB,4*bmh->bmh_Width,0,0,bmh->bmh_Width,bmh->bmh_Height);
                            else r = DoMethod(dto,PDTM_READPIXELARRAY,chunky,PBPAFMT_LUT8,bmh->bmh_Width,0,0,bmh->bmh_Width,bmh->bmh_Height);

                            if (FindResident("MorphOS"))
                            {
								struct Library *PictureDTBase = OpenLibrary("picture.datatype", 0);

                                if (PictureDTBase &&
          							(PictureDTBase->lib_Version<50) ||
              						(PictureDTBase->lib_Version==50 && PictureDTBase->lib_Revision<17))
								{
                                    r = TRUE;
                                }
                            }

                            if (!r && !args.tc)
                            {
                                register UBYTE *tchunky;
                                register ULONG tw = ((bmh->bmh_Width+15)>>4)<<4;

                                if (tw==bmh->bmh_Width) tchunky = chunky;
                                else tchunky = AllocVec(tw*bmh->bmh_Height,MEMF_ANY);

                                if (tchunky)
                                {
                                    if (DoDTMethod(dto,NULL,NULL,DTM_PROCLAYOUT,NULL,TRUE))
                                    {
                                        struct BitMap *bm;

                                        if (GetDTAttrs(dto,PDTA_BitMap,&bm,TAG_DONE))
                                        {
                                            struct RastPort trp;

                                            InitRastPort(&trp);

                                            if (trp.BitMap = AllocBitMap(8*(((bmh->bmh_Width+15)>>4)<<1),1,8,0,bm))
                                            {
                                                struct RastPort rp;
                                                register UBYTE  *dest;
                                                register int    y;

                                                InitRastPort(&rp);
                                                rp.BitMap = bm;

                                                dest = tchunky;

                                                for (y = 0; y<bmh->bmh_Height; y++)
                                                {
                                                    ReadPixelLine8(&rp,0,y,tw,dest,&trp);
                                                    dest += tw;
                                                }

                                                WaitBlit();
                                                FreeBitMap(trp.BitMap);

                                                if (tchunky!=chunky)
                                                {
                                                    register UBYTE *src;
                                                    register int   x;

                                                    src  = tchunky;
                                                    dest = chunky;

                                                    for (y = 0; y<bmh->bmh_Height; y++)
                                                    {
                                                        for (x = 0; x<bmh->bmh_Width; x++)
                                                            *dest++ = *src++;

                                                        src += tw-bmh->bmh_Width;
                                                    }
                                                }

                                                r = TRUE;
                                            }
                                        }
                                    }

                                    if (tchunky!=chunky) FreeVec(tchunky);
                                }
                            }

                            if (r)
                            {
                                register BPTR  out;
                                register ULONG closeOut;

                                if (args.to)
                                {
                                    out = Open((STRPTR)args.to,MODE_NEWFILE);
                                    closeOut = TRUE;
                                }
                                else
                                {
                                    out = Output();
                                    closeOut = FALSE;
                                }

                                if (out)
                                {
                                    register char  name[256], uname[256], *n, *c;
                                    register UBYTE *source = args.ntc ? chunky : chunky+size;
                                    register ULONG csize, i, ftr, broken = FALSE;
                                    ULONG          tc = 0;

                                    ftr = FALSE;

                                    if (args.tr)
                                    {
                                        if (args.tc)
                                        {
                                            if (args.tr[0]=='0' && args.tr[1]=='x') args.tr += 2;
                                            else if (args.tr[0]=='$') args.tr += 1;

                                            if (stch_l(args.tr,(LONG *)&tc)==strlen(args.tr)) ftr = TRUE;
                                            else FPrintf(err,">> TRCOLOR option ignored: invalid format\n");
                                        }
                                        else FPrintf(err,">> TRCOLOR option ignored: it may only be used in TRUECOLOR format\n");
                                    }

                                    if (!ftr)
                                    {
                                        if (args.tri)
                                        {
                                            tc = *args.tri;

                                            if (tc>=numColors)
                                            {
                                                if (args.tri) FPrintf(err,">> TRCOLORINDEX option ignored: invalid color index\n");
                                            }
                                            else ftr = TRUE;
                                        }

                                        if (!ftr)
                                        {
                                            if (bmh->bmh_Masking==mskHasTransparentColor) tc = bmh->bmh_Transparent;
                                            else
                                            {
                                                tc = 0;
                                                FPrintf(err,">> TRCOLORINDEX forced to 0\n");
                                            }
                                        }

                                        if (args.tc) tc = ((colors[tc] & 0xFF000000)>>8) | ((colors[tc+1] & 0xFF000000)>>16) | ((colors[tc+2] & 0xFF000000)>>24);
                                    }

                                    if (args.alpha)
                                    {
                                        if (args.tc)
                                        {
                                            register ULONG *src;

                                            FPrintf(err,"Appling alpha channel...\n");

                                            src   = (ULONG *)chunky;
                                            csize = size/4;

                                            while (csize--)
                                            {
                                                ULONG c, a;
                                                LONG  r, g, b;

                                                c  = *src;

                                                r  = (tc & 0xff0000)>>16;
                                                r -= (c & 0xff0000)>>16;
                                                g  = (tc & 0x00ff00)>>8;
                                                g -= (c & 0x00ff00)>>8;
                                                b  = (tc & 0x0000ff);
                                                b -= (c & 0x0000ff);
                                                a  = 0.299f*ABS(r) + 0.587f*ABS(g) + 0.114f*ABS(b);

                                                *src++ = c | (a<<24);
                                            }
                                        }
                                        else FPrintf(err,">> alpha channel may only be applied to true color format\n",PRG);
                                    }

                                    if (!args.ntc)
                                    {
                                        FPrintf(err,"Compressing...\n");

                                        csize = BRCPack(chunky,source,size);
                                    }
                                    else csize = size;

                                    n = (STRPTR)(args.name ? args.name : args.from);

                                    stccpy(name,FilePart(n),sizeof(name));
                                    if (c = strchr(name,'.')) *c = 0;
                                    strcpy(uname,name);
                                    for (c = uname; *c; c++) *c = toupper(*c);

                                    if (args.tc)
                                    {
                                        FPrintf(out,"#define %s_WIDTH      %ld\n",     uname,bmh->bmh_Width);
                                        FPrintf(out,"#define %s_HEIGHT     %ld\n",     uname,bmh->bmh_Height);
                                        FPrintf(out,"#define %s_TOTALWIDTH %ld\n",     uname,4*bmh->bmh_Width);
                                        FPrintf(out,"#define %s_CSIZE      %ld\n",     uname,args.ntc ? 0 : csize);
                                        FPrintf(out,"#define %s_NUMCOLORS  0\n",       uname);
                                        FPrintf(out,"#define %s_TRCOLOR    0x%08lx\n", uname,tc);
                                        FPrintf(out,"#define %s_FLAGS      %s\n",      uname,args.alpha ? "BRFLG_ARGB|BRFLG_AlphaMask" : "BRFLG_ARGB");
                                        FPrintf(out,"#define %s_Colors     NULL\n\n",  name);

                                        FPrintf(out,"UBYTE %s_Data[%ld] =\n\x7B",name,csize);
                                    }
                                    else
                                    {
                                        FPrintf(out,"#define %s_WIDTH      %ld\n",  uname,bmh->bmh_Width);
                                        FPrintf(out,"#define %s_HEIGHT     %ld\n",  uname,bmh->bmh_Height);
                                        FPrintf(out,"#define %s_TOTALWIDTH %ld\n",  uname,bmh->bmh_Width);
                                        FPrintf(out,"#define %s_CSIZE      %ld\n",  uname,args.ntc ? 0 : csize);
                                        FPrintf(out,"#define %s_NUMCOLORS  %ld\n",  uname,numColors);
                                        FPrintf(out,"#define %s_TRCOLOR    %ld\n",  uname,tc);
                                        FPrintf(out,"#define %s_FLAGS      %s\n\n", uname,args.RGB ? "0" : "BRFLG_ColorRGB8");

                                        FPrintf(out,"UBYTE %s_Data[%ld] =\n\x7B",name,csize);
                                    }

                                    FPrintf(err,"Writing chunky data...\n");

                                    i = 0;

                                    while (csize--)
                                    {
                                        if (SetSignal(0,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
                                        {
                                           broken = TRUE;
                                           break;
                                        }

                                        if (i%15==0) FPrintf(out,"\n    ");
                                        FPrintf(out,"0x%02lx",(ULONG)*source++);

                                        if (csize) FPrintf(out,", ");

                                        i++;
                                    }

                                    FPrintf(out,"\n\x7D;\n\n");

                                    if (!broken && !args.tc)
                                    {
                                        FPrintf(err,args.RGB ? "Writing RGB colors table...\n" : "Writing RGB8 colors table...\n");

                                        size = (args.RGB) ? 3*numColors : numColors;
                                        i = 0;

                                        FPrintf(out,"ULONG %s_Colors[%ld] = \n\x7B",name,size);

                                        while (size--)
                                        {
                                            if (SetSignal(0,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
                                            {
                                                broken = TRUE;
                                                break;
                                            }

                                            if (args.RGB)
                                            {
                                                if (i%3==0) FPrintf(out,"\n    ");

                                                FPrintf(out,"0x%08lx",*colors++);
                                            }
                                            else
                                            {
                                                register ULONG x;

                                                x = ((*colors++ & 0xFF000000)>>8) |
                                                    ((*colors++ & 0xFF000000)>>16) |
                                                    ((*colors++             )>>24);

                                                FPrintf(out,"\n    0x%08lx",x);
                                            }

                                            if (size) FPrintf(out,", ");

                                            i++;
                                        }

                                        FPrintf(out,"\n\x7D;\n");
                                    }

                                    if (broken) FPrintf(err,"%s: broken\n",PRG);
                                    if (closeOut) Close(out);

                                    res = RETURN_OK;
                                }
                                else
                                    if (closeOut) FPrintf(err,"%s: can't open file '%s'\n",PRG,args.to);
                                    else FPrintf(err,"%s: not enogh memory available\n",PRG);
                            }
                            else FPrintf(err,"%s: can't obtain chunky pixels from '%s'\n",PRG,args.from);

                            FreeVec(chunky);
                        }
                        else FPrintf(err,"%s: not enogh memory available\n",PRG);
                    }
                    else FPrintf(err,"%s: can't find colors and dimension of '%s'\n",PRG,args.from);

                    DisposeDTObject(dto);
                }
                else FPrintf(err,"%s: can't load file '%s'\n",PRG,args.from);

                CloseLibrary(DataTypesBase);
            }
            else FPrintf(err,"%s: can't open datatypes.library 39+\n",PRG);

            CloseLibrary((struct Library *)GfxBase);
        }
        else FPrintf(err,"%s: can't open graphics.library 39+\n",PRG);

        FreeArgs(ra);
    }
    else errc = IoErr();

    if (errc)
    {
        char fault[256];

        if (Fault(errc,PRG,fault,sizeof(fault))>0) FPrintf(err,"%s\n",fault);
        res = RETURN_ERROR;
    }

    if (closeErr) Close(err);

    return res;
}

/***********************************************************************/
