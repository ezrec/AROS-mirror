/* KDPgfx v0.8 ... by wrecK/KnP YhdZ/KnP
  some graphic functions for KDP
  (hardcoded for 320x200x8bit)

*/

#ifndef KDPGFX_H
#define KDPGFX_H

//#include <exec/types.h>
#include <string.h>
//#include <stdlib.h>



#include "kdp.h"


extern UBYTE KDPcgcoltab[256*256];
//color adding table
extern UBYTE  KDPcoltab[512];

void  KDPpset(int x,int y,UBYTE col,UBYTE *vmem);
UBYTE KDPpoint(int x,int y,UBYTE *vmem);
void  KDPaddpset(int x,int y,UBYTE col,UBYTE *vmem);
void  KDPaddpset2(int x,int y,UBYTE col,UBYTE *vmem);
void  KDPaddsp(int x,int y,UBYTE *sp,UBYTE *vmem);
void  KDPsp(int x,int y,UBYTE *sp,UBYTE *vmem);
void  KDPsp256(int x,int y,int size,UBYTE *tex,UBYTE *vmem);
void  KDPpset2(int x,int y,UBYTE col,UBYTE *vmem);
void  KDPline(int x0,int y0,int x1,int y1,UBYTE col,UBYTE *vmem);
void  KDPline2(int x0,int y0,int x1,int y1,UBYTE col,UBYTE *vmem);
void  KDPcls(UBYTE *vmem);
void  KDPblur2(UBYTE *vmem);
void  KDPfire(UBYTE *vmem);
void  KDPblur3(UBYTE *vmem);
void  KDPblur(int speed,UBYTE *vmem);
void  KDPcircle(int xo,int yo,int a,int b,UBYTE col,UBYTE *vmem);
void  KDPcircle2(int xo,int yo,int a,int b,UBYTE col,UBYTE *vmem);
void  KDPpalRange(float r1,float g1,float b1,int r2,int g2,int b2, int begin, int end,UBYTE *pal);
int   KDPfindCol(UBYTE *pl,int red,int green,int blue);
int   KDPreadBMP(char *filename,UBYTE *pal,UBYTE *image);
int   KDPwriteBMP(char *filename,UBYTE *pal,UBYTE *image,int width,int height);
void  KDPbox(int x1,int y1,int x2,int y2,UBYTE col,UBYTE *vmem);
void  KDPbox2(int x1,int y1,int x2,int y2,UBYTE col,UBYTE *vmem);
void  KDPmakecoltab1(UBYTE *pal);

#endif /* KDPGFX_H */

