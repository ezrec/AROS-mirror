/* _WriteILBMFile.c ranxerox 2.0 890521 900323 */
#include <aros/oldprograms.h>
#include <libraries/dosextens.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#define SAVE 0L
#define NOSAVE 1L
#define NOOPEN 2L

extern struct GfxBase *GfxBase;

static BPTR _W_fh;
static long flag;
static void cWrite();

/* FIXME: This will only work on screens with BMF_STANDARD bitmaps! */
long WriteILBMFile(filename,scr)
char filename[];
struct Screen *scr;
{
unsigned long scrrowbytes,rr,pp;
UBYTE colors;
unsigned short vm;
short index,red,green,blue,color;
UBYTE *planepointer;
long tmp;
UBYTE depth = GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH);
UWORD flags = GetBitMapAttr(scr->RastPort.BitMap, BMA_FLAGS);
if (!(flags & BMF_STANDARD))
    return NOSAVE;

flag=SAVE;
vm=( (scr->ViewPort.Modes) & (HIRES|HAM|LACE|EXTRA_HALFBRITE) );
colors=(1<<depth);
if(vm&HAM)colors=16;
if(vm&EXTRA_HALFBRITE)colors=32;

if((_W_fh=Open(filename,MODE_NEWFILE))==BNULL)flag=NOOPEN;
cWrite((char *)"FORM",4L);
tmp=(long)(3L*colors+(scr->Width)/8L*(scr->Height)*(depth)+60L);
cWrite((char *)&tmp,4L);
cWrite((char *)"ILBMBMHD",8L);
tmp=20L;
cWrite((char *)&tmp,4L);
cWrite((char *)&scr->Width,2L);
cWrite((char *)&scr->Height,2L);
tmp=0L;
cWrite((char *)&tmp,4L);
tmp=(long)depth<<24;
cWrite((char *)&tmp,2L);
tmp=0L;
cWrite((char *)&tmp,4L);
tmp=(long)0xA0B<<16;
cWrite((char *)&tmp,2L);
cWrite((char *)&scr->Width,2L);
cWrite((char *)&scr->Height,2L);
cWrite((char *)"CAMG",4L);
tmp=4L;
cWrite((char *)&tmp,4L);
tmp=(long)vm;
cWrite((char *)&tmp,4L);
cWrite((char *)"CMAP",4L);
tmp=(long)colors*3;
cWrite((char *)&tmp,4L);
for(index=0;index<colors;index++)
{
 color=GetRGB4(scr->ViewPort.ColorMap,(long)index);
 red=color>>8;color-=red<<8;
 green=color>>4;color-=green<<4;
 blue=color;
 tmp=red*16;tmp<<=8;
 tmp+=green*16;tmp<<=8;
 tmp+=blue*16 ;tmp<<=8;
 cWrite((char *)&tmp,3L);
};
cWrite((char *)"BODY",4L);
tmp=(long)((scr->Width)/8L*(scr->Height)*(depth));
cWrite((char *)&tmp,4L);
scrrowbytes=scr->Width/8;
for(rr=0;rr<scr->Height;rr++)
{ for(pp=0;pp<depth;pp++)
  {
  planepointer=scr->RastPort.BitMap->Planes[pp] + (rr*scrrowbytes);
  cWrite((char *)planepointer,(long)scrrowbytes);
  };
};
if(_W_fh)Close(_W_fh);
if(flag==NOSAVE)DeleteFile(filename);
return(flag);
}

void cWrite(pntr,len)
char *pntr;
long len;
{
  if(flag==SAVE)
    {
     if( Write(_W_fh,pntr,len) == -1L)flag=NOSAVE;
    };
}



