#include <aros/oldprograms.h>
#include <stdio.h>
#include "mytypes.h"
#include "readilbm.h"

void OutErr(char *);

#define DefRes 2
int MapImageV = DefRes*DefRepV,
    MapImageH= DefRes*DefRepH; /* virtual screen size */
static int PixV=DefRes,
           PixH=DefRes;  /* true ilbm size in pixels */
short MapRepV = DefRepV,
      MapRepH = DefRepH;
static short BytesPerLine;
static unsigned char *Raster= null;
static long MaxShade;
static bool AxisFlipped = DefXYFlip;
/*
 * Update the MapImageH and MapImageV variables
 */
void PrepImgPix()
{
    MapImageV = PixV * MapRepV;
    MapImageH = PixH * MapRepH;
    if( AxisFlipped ) {
        int temp;
        temp = MapImageV;
        MapImageV = MapImageH;
        MapImageH = temp;
    }
}
/*
 * free up any memory holding mapping image
 */
void CloseImgPix()
{
        if( Raster) free(Raster);
        Raster = null;
        PixV = 0xff; PixH = 0xff;
        PrepImgPix();
}

/*
 * cause x and y axises to be reversed
 */
void FlipImgPix( flip )
    bool flip;
{
    AxisFlipped = flip;
    PrepImgPix();
}

/*
 * 4 bits per pixel means 2 pixels per byte
 */
bool OpenImgPix(sizex, sizey, maxshade)
    int sizex, sizey;
    short maxshade;
{
    CloseImgPix();
    if( maxshade == 0 ) {
        OutErr("OpenImgPix: got max shade = 0\n");
        maxshade = 1;
     }
    MaxShade = maxshade;
    BytesPerLine = (sizex +1)/2;
    Raster = (unsigned char *) malloc( BytesPerLine * sizey);
    if( !Raster ) {
        printf("OpenImgPix: not enough memory\n");
        return(false); /* no memory err */
    }

    PixV = sizey;
    PixH = sizex;
    PrepImgPix();
    return(true);
}

#define CalcByte(cbx,cby) (Raster + ( BytesPerLine * cby ) + (cbx >> 1))


void SetImgPix(x, y, val)
    int x, y; /* location */
    int val;
{
    unsigned char *bite;
    unsigned char shade;

    if( x > PixH || x < 0 || y > PixV || y < 0 ) {
        printf("SetImgPix(%d,%d,%d) out of range\n",x,y,val);
        return;
    }

    if( !Raster) return;
    shade = ( (val<< 4)-val)/MaxShade;
    bite = CalcByte(x,y);
    if( x & 1) {
        *bite = (*bite & 0xf) | ( shade <<4 );
    }
    else {
        *bite = (*bite & 0xf0) | shade;
    }
}


short GetImgPix(x,y)
    int x, y;
{
    unsigned char *bite;

    if( AxisFlipped ) {
        int temp;
        temp = x; x = y; y = temp;
    }

    x %= PixH;
    y %= PixV;

    if( !Raster ) {
        return( (short)(((x ^ y)& 0x10) ? 0xff: 0));
    }
    bite = CalcByte(x,y);

    if( x & 1) {
        return((short)(*bite & 0xf0));
    }
    else {
        return( (short)((*bite & 0x0f) <<4));
    }
}
