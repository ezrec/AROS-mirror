/*
 * This program, when called with arg "smith", tries to read smith.r, smith.g
 * and smith.b, and write a combined raw data file acceptable to RAY2, called
 * smith.tmp
 */
#include <stdio.h>
#include "mytypes.h"

#define RED 0
#define GRN 1
#define BLU 2
#define NumPrims 3 /* number of primary colors */

#define MakeID(a,b,c,d) ( ((a)<<24) | ((b)<<16) | ((c)<<8) | (d) )

#define ID_FORM MakeID('F','O','R','M')
#define ID_ILBM MakeID('I','L','B','M')
#define ID_BMHD MakeID('B','M','H','D')
#define ID_CMAP MakeID('C','M','A','P')
#define ID_CAMG MakeID('C','A','M','G')
#define ID_BODY MakeID('B','O','D','Y')

#define ROUNDUP(x) ( ((x)+1) & (~1L) )

struct CHUNK {
    unsigned long Id, Size;
} Chunk;

struct BMHD {
    short w,h,x,y;
    char npl,mask,compress,pad1;
    short transparentColor;
    char xAspect,yAspect;
    short pageWidth,pageHeight;
};

/*
 * compare width and height of two bit maps
 * return false if equal
 */
bool CmpBMHD(a, b)
    struct BMHD *a, *b;
{
    return( a->w != b->w ||
            a->h != b->h ||
          a->npl != b->npl
          );
}


struct BMHD bmhd[NumPrims];
static FILE *ftrip[NumPrims], *fout;
char *nametrip[NumPrims];
char *oname;
bool FileError;
short depth;

unsigned char colmap[NumPrims][32][3];


/*
 * read len bytes from selected file
 */
void FRead(buffer, len, filenum)
    char *buffer;
    int len, filenum;
{
    if( fread(buffer, len, 1, ftrip[filenum]) == 0 ) {
        fprintf(stderr, "read error in %s\n", nametrip[filenum] );
        exit(-1);
    }
}



#define ABORT(str) { fprintf(stderr,str); exit(-1); }

#define ReadChunk()  FRead(&Chunk, sizeof(Chunk), filenum)

void ReadIffHeaders(filenum)
    int filenum;
{
    long    ILBMid;
    bool cmapFlag = false,
         bmhdFlag = false;
    short i;


    ReadChunk ();
    if (Chunk.Id != ID_FORM)
        ABORT ("Not an IFF File");

    FRead (&ILBMid, 4, filenum);
    if (ILBMid != ID_ILBM)
        ABORT ("Not an ILBM File");

    while (1) {
        long camgdata;

        ReadChunk ();

        if (Chunk.Id == ID_BODY) {
            if (!cmapFlag) {
                printf("no cmap before body in %s\n", nametrip[filenum]);
                FileError = true;
            }
            if (!bmhdFlag) {
                printf("no bmhd before the body in %s\n", nametrip[filenum]);
                FileError = true;
            }
            return;
        }

        if( feof( ftrip[filenum] ) ) {
            printf("reached end of file without seeing body in %s\n", nametrip[filenum]);
            FileError = true;
            return;
        }

        switch (Chunk.Id) {
            case ID_CMAP:
                FRead (colmap[filenum], Chunk.Size, filenum);
                for (i = 0; i < 32; i++) {
                    colmap[filenum][i][filenum] >>= 4;
                }
                cmapFlag = true;
                break;
            case ID_BMHD:
                FRead(&bmhd[filenum], Chunk.Size, filenum);
                bmhdFlag = true;
                break;
            case ID_CAMG:
                FRead( &camgdata, sizeof(camgdata),filenum );
                break;
            default:            /* unknown identifier */
                fseek( ftrip[filenum], Chunk.Size, 1);
                break;
        }
    }
}


/*
 * leftmost pixel of byte is in most significant bit of byte
 */
static char GetIlbmVal( rastlist, h)
    char *rastlist[6];
    int h;
{
    int i;
    char value = 0;
    short mask, bytep;

    mask = 0x80 >> ( h & 7);
    bytep = h >> 3;

    for( i = depth-1; i >= 0; i-- ) {
        value <<= 1;
        value |= (*(rastlist[i]+bytep) & mask) ? 1: 0;
    }
    return( value );
}

/*
 * decompress a runlength row of bytes
 */
static void ReadDecomLine(linebytes, rp, filenum)
    int linebytes, filenum;
    char *rp;
{
    int runlen;
    char pixel;

    while (linebytes) {
        runlen = getc (ftrip[filenum]);
        if (runlen > 127)
            runlen -= 256;
        if (runlen >= 0) {
            runlen++;
            FRead (rp, runlen, filenum);
            rp += runlen;
            linebytes -= runlen;
        }
        else {
            runlen = -runlen + 1;
            linebytes -= runlen;
            pixel = getc (ftrip[filenum]);
            do
                *(rp++) = pixel;
            while (--runlen);
        }
    }
}
/*
 * read raster line from one of the color files
 */
void LoadRaster(raster, ByteWidth, filenum)
    char *raster[6];
    int ByteWidth, filenum;
{
    if(bmhd[filenum].compress = 0) {
        FRead (raster, ByteWidth * depth, filenum);
    }
    else {
        short i;
        for( i = 0; i < depth; i++) {
            ReadDecomLine( ByteWidth, raster[i], filenum);
        }
    }
}

/*
 * write intensity values for a given gun
 * note: I've made dirty assumption about byte ordering in a short word
 * if you run this on an ibm (ha ha ha) you'll have to change it
 */
#define ChipSize 4
void SaveRaster( raster, width, prim)
    char *raster[];
    int width, prim;
{
    short i, j;
    unsigned short val;

    for( i = 0; i < width; i += ChipSize ) {
        val = colmap[prim][GetIlbmVal(raster, i+3)][prim] << ChipSize;
        val |= colmap[prim][GetIlbmVal(raster, i+2)][prim];
        fputc( val&0xff, fout);
        val = colmap[prim][GetIlbmVal(raster, i+1)][prim] << ChipSize;
        val |= colmap[prim][GetIlbmVal(raster, i+0)][prim];
        fputc( val&0xff, fout);
    }
}

/*
 * translate body of file to format RAY2 likes
 */
void TranslateRay2Body()
{
    int BytesWidth;
    int row, height, width, plane, prim;
    char *raster[6];

    depth = bmhd[0].npl;
    width = bmhd[0].w;
    height = bmhd[0].h;
    BytesWidth = (width + 7)/8 ;

    for( plane = 0; plane < bmhd[0].npl; plane++ ) {
        raster[plane] = (char *)malloc(BytesWidth);
    }

    for( row = 0; !FileError && row < height; row++ ) {
        fputc((unsigned char)(row>>8),fout);      /* write line number */
        fputc((unsigned char)(row&0xff),fout);

        for( prim = RED; prim <= BLU; prim++ ) {
            LoadRaster( raster, BytesWidth, prim);
            SaveRaster( raster, width, prim);
        }
    }
}



main(argc, argv)
    int argc;
    char *argv[];
{
    char buff[4][80];
    char *extensions = "rgb";
    int i;

    if( argc == 1 || *argv[1] == '?' ) {
        fprintf(stderr,"usage:  merge filename\n");
        exit(-1);
    }
    FileError = false;

    for( i = 0; i < NumPrims; i++ ) {
        sprintf(buff[i], "%s.%c", argv[1], extensions[i]);
        nametrip[i] = buff[i];
        ftrip[i] = fopen(nametrip[i],"r");
        if(!ftrip[i]) {
            fprintf(stderr,"unable to read %s\n", nametrip[i]);
            FileError = true;
        }
    }

    sprintf(buff[3], "%s.tmp", argv[1]);
    oname = (argc>2)? argv[2]: buff[3];
    fout = fopen( oname,"w");
    if(!fout) {
        fprintf(stderr,"unable to write to %s\n", oname);
        FileError = true;
    }

    if( FileError ) goto ErrorExit;

    for( i = 0; i < NumPrims; i++ ) {
        printf("reading header %d\n", i);
        ReadIffHeaders(i);
    }

    for( i = 1; i < NumPrims; i++ ) {
        if( CmpBMHD(&bmhd[0], &bmhd[i])) {
            fprintf(stderr,"image sizes differ(r != %d)\n", i);
            FileError = true;
        }
    }

    if( FileError ) goto ErrorExit;
    printf("about to translate body\n");
    TranslateRay2Body();

    ErrorExit:
        for( i = 0; i < NumPrims; i++) {
            if(ftrip[i]) fclose( ftrip[i] );
        }
        if(fout) fclose(fout);
}


