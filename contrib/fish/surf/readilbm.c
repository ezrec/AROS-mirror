#include <stdio.h>
#include <ctype.h>
#include "readilbm.h"
#include "mytypes.h"

void OutErr(char *);

#define RED 0
#define GRN 1
#define BLU 2

 /* color to grey conversion methods */

#define AVERAGE 0
#define LUMIN    1
#define DIST     2
#define REDONLY  3
#define GREENONLY 4
#define BLUEONLY 5

#define MakeID(a,b,c,d) ( ((a)<<24) | ((b)<<16) | ((c)<<8) | (d) )

#define ID_FORM MakeID('F','O','R','M')
#define ID_ILBM MakeID('I','L','B','M')
#define ID_BMHD MakeID('B','M','H','D')
#define ID_CMAP MakeID('C','M','A','P')
#define ID_CAMG MakeID('C','A','M','G')
#define ID_BODY MakeID('B','O','D','Y')

#define ROUNDUP(x) ( ((x)+1) & (~1L) )



static struct CHUNK {
    unsigned long Id, Size;
} Chunk;

static unsigned char   cmap[32][3];

struct BMHD {
    short w,h,x,y;
    char npl,mask,compress,pad1;
    short transparentColor;
    char xAspect,yAspect;
    short pageWidth,pageHeight;
};


static short    Color[32],              /* output colors        */
                Method = AVERAGE;       /* color conversion     */
static short     numcolors;

static
FILE            *fin;                   /* input file           */

static void FRead();
static void ReadBMHD();
static void ReadChunk();
static void RastOut();
static void HamOut();
static char GetIlbmVal();
static void InitColorMappings();
static short Convert();
static void ReadDecomLine();
static void ProcessRows();

#define ABORT(str) { OutErr(str); goto ErrorExit; }


void SetGreyModel( model )
    int model;
{
    Method = model;
}

 /*
  * main routine for reading in an iff file
  */

void ReadIlbm(filename)
    char *filename;
{
    struct BMHD bmhd;
    long    ILBMid;
    unsigned char cmapFlag = FALSE,
                  bmhdFlag = FALSE;


    if ((fin = fopen (filename, "r")) == NULL) {
        OutErr ("ERROR: cannot open input file");
        return;
    }
    /* read in iff file */

    ReadChunk ();
    if (Chunk.Id != ID_FORM)
        ABORT ("Not an IFF File");

    FRead (&ILBMid, 4);
    if (ILBMid != ID_ILBM)
        ABORT ("Not an ILBM File");

    while (1) {
        long camgdata;

        ReadChunk ();

        if (Chunk.Id == ID_BODY)
            break;

        if( feof( fin ) ) {
            ABORT("reached end of file without seeing body\n");
        }

        switch (Chunk.Id) {
            case ID_CMAP:
                FRead (cmap, Chunk.Size);
                numcolors = Chunk.Size/3;
                cmapFlag = TRUE;
                break;
            case ID_BMHD:
                ReadBMHD (&bmhd);
                bmhdFlag = TRUE;
                break;
            case ID_CAMG:
                FRead( &camgdata, sizeof(camgdata) );
                break;
            default:            /* unknown identifier */
                fseek( fin, Chunk.Size, 1);
                break;
        }
    }
    if (!cmapFlag) {
        ABORT("IFF file does not contain a CMAP chunk before the BODY\n");
    }

    if (!bmhdFlag) {
        ABORT("IFF file does not contain a BMHD chunk before the BODY\n");
    }

    InitColorMappings();
    if( OpenImgPix( bmhd.w, bmhd.h, Convert(0xf, 0xf, 0xf)) ) {
        ProcessRows(&bmhd);
    }

    ErrorExit:
        fclose( fin);
}

static void ProcessRows(bmhd)
    struct BMHD *bmhd;
{
    char *rastlist[6];
    char *Raster;
    int depth, i, v, pixwidth;
    int BytePerLine;

    depth = bmhd->npl;
    pixwidth = bmhd->w;
    BytePerLine =(pixwidth+7) / 8;


    Raster = (char *) malloc (BytePerLine * depth);

    if(!Raster ) {
        OutErr("ProcessRows:could not allocate Raster");
        return;
    }

    for( i = 0; i < depth; i++ ) {
        rastlist[i] = Raster + BytePerLine*i;
    }

    for( v = 0; v < bmhd->h; v++) {
        switch (bmhd->compress) {
            case 0:
                FRead (Raster, BytePerLine * depth);
                break;
            case 1:
                for( i = 0; i < depth; i++) {
                    ReadDecomLine( BytePerLine, rastlist[i]);
                }
                break;
            default:
                ABORT ("Unknown Compression type in BODY");

        }
        if(depth == 6 ) {
            HamOut( rastlist, pixwidth, v);
        }
        else {
            RastOut( rastlist, pixwidth, v, depth);
        }
    }

    ErrorExit:
        if( Raster ) free(Raster);
}


static void ReadDecomLine(linebytes, rp)
    int linebytes;
    char *rp;
{
    int runlen;
    char pixel;

    while (linebytes) {
        runlen = getc (fin);
        if (runlen > 127)
            runlen -= 256;
        if (runlen >= 0) {
            runlen++;
            FRead (rp, runlen);
            rp += runlen;
            linebytes -= runlen;
        }
        else {
            runlen = -runlen + 1;
            linebytes -= runlen;
            pixel = getc (fin);
            do
                *(rp++) = pixel;
            while (--runlen);
        }
    }
}

 /*
  * Convert - convert (r,g,b) to hex greyscale.
  */

static short Convert(r,g,b)
    unsigned char   r,g,b;
{
    short   i,
            rd, gd, bd,
            min,
            dist,
            best = 0;

 /* convert color according to 'Method' */
    switch (Method) {
        case AVERAGE:           /* average r,g,b to obtain grey level */
            return ((short)((r + g + b) / 3));
        case LUMIN:             /* use NTSC luminescence as grey level */
            return ((short)((r * 30 + g * 59 + b * 11) / 100));
        case DIST:          /* use grey with minimum distance in color */
            min = 15*15 * 3;
            for( i = 0; i < numcolors; i++ ) {
                rd = r -i;
                gd = g - i;
                bd = b - i;
                dist = rd * rd + gd * gd + bd * bd;
                if( dist < min ) {
                    min = dist; best = i;
                }
            }
            return( best );
        case REDONLY:
            return((short)r);
        case GREENONLY:
            return((short)g);
        case BLUEONLY:
            return((short)b);
        default:
            exit(-1); /* error, big one */
    }
}                               /* Convert */

static void InitColorMappings()
{
    int     i;

 /* put colors in 4-bit range and Convert */
    for (i = 0; i < 32; i++) {
        cmap[i][RED] >>= 4;
        cmap[i][GRN] >>= 4;
        cmap[i][BLU] >>= 4;
        Color[i] = Convert (cmap[i][RED], cmap[i][GRN], cmap[i][BLU]);
    }
}


/*
 * leftmost pixel of byte is in most significant bit of byte
 */
static char GetIlbmVal( rastlist, h, bpp )
    char *rastlist[6];
    int h, bpp;
{
    int i;
    char value = 0;
    short mask, bytep;

    mask = 0x80 >> ( h & 7);
    bytep = h >> 3;

    for( i = bpp-1; i >= 0; i-- ) {
        value <<= 1;
        value |= (*(rastlist[i]+bytep) & mask) ? 1: 0;
    }
    return( value );
}

 /*
  * HamOut - output ham image in hex.
  */
static void HamOut(rastlist, pixwidth, v)
    char *rastlist[6];
    int pixwidth, v;
{
    unsigned char   lastred = 0,
                    lastgreen = 0,
                    lastblue = 0;
    int     h;
    char pixval;
    for( h = 0; h <pixwidth; h++ ) {
        short shade;

        shade = GetIlbmVal(rastlist, h, 6);
        pixval = shade & 0x0F;
        switch (shade & 0x30) {
        case 0x00:
            lastred = cmap[(int)pixval][RED];
            lastgreen = cmap[(int)pixval][GRN];
            lastblue = cmap[(int)pixval][BLU];
            shade = Color[(int)pixval];
            break;
        case 0x10:
            lastblue = pixval;
            shade = Convert(lastred,lastgreen,lastblue);
            break;
        case 0x20:
            lastred = pixval;
            shade = Convert(lastred,lastgreen,lastblue);
            break;
        case 0x30:
            lastgreen = pixval;
            shade = Convert(lastred,lastgreen,lastblue);
        }
        SetImgPix(h, v, shade);
    }
}

 /*
  * RastOut - handle normal bit mapped images
  */
static void RastOut(rastlist, pixwidth, v, depth)
    char *rastlist[6];
    int pixwidth, v, depth;
{
    int h;
    for( h = 0; h < pixwidth; h++ ) {
        short shade;
        shade = Color[(int)GetIlbmVal( rastlist, h, depth)];
        SetImgPix( h, v, shade);
    }
}

 /*
  * ReadChunk - read in an IFF Chunk.
  */

static void ReadChunk()
{
    FRead (&Chunk, sizeof (Chunk));

}                               /* ReadChunk */

 /*
  * ReadBMHD - read the BMHD structure.
  */

static void ReadBMHD(bmhd)
struct BMHD *bmhd;
{
    FRead (bmhd, Chunk.Size);
}                               /* ReadBMHD */


 /*
  * FRead - read 'len' bytes to 'pointer' while checking for an error.
  */

static void FRead(pointer,len)
char    *pointer;
int     len;
{
    if (fread (pointer, len, 1, fin) == 0)  {
        char outbuff[90]; sprintf(outbuff,"Fread Error in reading input file
        at %d ", ftell(fin)); OutErr(outbuff);
    }
}                               /* FRead */
