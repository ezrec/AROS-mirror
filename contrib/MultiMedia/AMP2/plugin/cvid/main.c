/*
 *
 * main.c
 *
 */

/* FIXME:
 *
 * This plugin is in serious need of a complete rewrite, or atleast optimization...
 */

#include <stdio.h>
#include <stdlib.h>

#include <plugin.h>

#define uchar unsigned char
#define ushort unsigned short
#define ulong unsigned long

/* FIXME: Optimize */
#define get16(dptr) ((dptr[0] << 8) | dptr[1]); dptr+=2
#define get24(dptr) ((dptr[0] << 16) | (dptr[1] << 8) | dptr[2]); dptr+=3

#define cvidMaxStrips 16

struct Color2x2 {
  uchar y0, y1, y2, y3;
  uchar u, v;
};

struct CVIDData {
  struct Color2x2 *cvidMaps0[cvidMaxStrips];
  struct Color2x2 *cvidMaps1[cvidMaxStrips];
  long cvidVMap0[cvidMaxStrips];
  long cvidVMap1[cvidMaxStrips]; };

#define blockInc(x,y,xmax) {  \
  x+=4;                       \
  if (x>=xmax) { x=0; y+=4; } \
}

struct CVIDData *cvidData = NULL;

void SelectCVIDFuncs(void)
{
  int i;

  if (cvidData == NULL)
  {
    cvidData = (struct CVIDData *)malloc(sizeof(struct CVIDData));
    for (i=0; i<cvidMaxStrips; i++)
    {
      cvidData->cvidMaps0[i] = (struct Color2x2 *)malloc(256*sizeof(struct Color2x2));
      cvidData->cvidMaps1[i] = (struct Color2x2 *)malloc(256*sizeof(struct Color2x2));
      memset((uchar *)cvidData->cvidMaps0[i], 0, 256*sizeof(struct Color2x2));
      memset((uchar *)cvidData->cvidMaps1[i], 0, 256*sizeof(struct Color2x2));
      cvidData->cvidVMap0[i] = 0;
      cvidData->cvidVMap1[i] = 0;
    }
  }
}

static unsigned char *yuv[3];

#define yuv2x2(Y0, Y1, Y2, Y3, U, V, cmap2x2) \
  cmap2x2->y0=Y0; \
  cmap2x2->y1=Y1; \
  cmap2x2->y2=Y2; \
  cmap2x2->y3=Y3; \
  cmap2x2->u=U; \
  cmap2x2->v=V; \

void color2x2blk1(ulong xpos, ulong ypos, ulong width, struct Color2x2 *cm)
{
  uchar *y, *u, *v; 
  int off;

  off = ypos * width + xpos;
  y=yuv[0]+off;

  off = (ypos / 2) * (width / 2) + (xpos / 2);
  u=yuv[1] + off;
  v=yuv[2] + off;

/* y0y0y1y1
 * y0y0y1y1
 * y2y2y3y3
 * y2y2y3y3
 */

  y[0]=cm->y0;
  y[1]=cm->y0;
  y[2]=cm->y1;
  y[3]=cm->y1;
  y+=width;
  y[0]=cm->y0;
  y[1]=cm->y0;
  y[2]=cm->y1;
  y[3]=cm->y1;
  y+=width;
  y[0]=cm->y2;
  y[1]=cm->y2;
  y[2]=cm->y3;
  y[3]=cm->y3;
  y+=width;
  y[0]=cm->y2;
  y[1]=cm->y2;
  y[2]=cm->y3;
  y[3]=cm->y3;

  width /= 2;

/* u-u-
 * ---- => uu
 * u-u-    uu
 * ----
 *
 * v-v-
 * ---- => vv
 * v-v-    vv
 * ----
 */

  u[0]=cm->u;
  u[1]=cm->u;
  u+=width;
  u[0]=cm->u;
  u[1]=cm->u;

  v[0]=cm->v;
  v[1]=cm->v;
  v+=width;
  v[0]=cm->v;
  v[1]=cm->v;
}

void color2x2blk4(ulong xpos, ulong ypos, ulong width, struct Color2x2 *cm0, struct Color2x2 *cm1, struct Color2x2 *cm2, struct Color2x2 *cm3)
{
  uchar *y, *u, *v; 
  int off;

  off = ypos * width + xpos;
  y=yuv[0]+off;

  off = (ypos / 2) * (width / 2) + (xpos / 2);
  u=yuv[1] + off;
  v=yuv[2] + off;

/* y0y1 | y0y1
 * y2y3 | y2y3    0 | 1
 * ----------- => ----- 
 * y0y1 | y0y1    2 | 3
 * y2y3 | y2y3
 */

  y[0]=cm0->y0;
  y[1]=cm0->y1;
  y[2]=cm1->y0;
  y[3]=cm1->y1;
  y+=width;
  y[0]=cm0->y2;
  y[1]=cm0->y3;
  y[2]=cm1->y2;
  y[3]=cm1->y3;
  y+=width;
  y[0]=cm2->y0;
  y[1]=cm2->y1;
  y[2]=cm3->y0;
  y[3]=cm3->y1;
  y+=width;
  y[0]=cm2->y2;
  y[1]=cm2->y3;
  y[2]=cm3->y2;
  y[3]=cm3->y3;

  width /= 2;

/* u- | u-
 * -- | --    u0 | u1
 * ------- => -------
 * u- | u-    u2 | u3
 * -- | --
 *
 * v- | v-
 * -- | --    v0 | v1
 * ------- => -------
 * v- | v-    v2 | v3
 * -- | --
 */

  u[0]=cm0->u;
  u[1]=cm1->u;
  u+=width;
  u[0]=cm2->u;
  u[1]=cm3->u;

  v[0]=cm0->v;
  v[1]=cm1->v;
  v+=width;
  v[0]=cm2->v;
  v[1]=cm3->v;
}

void DecodeCVID(uchar *from, ulong width, ulong height, ulong encSize)
{
  long x, y, cvidMapNum;
  ulong len;
  ulong kk, strips, cNum, yTop;

  x=0;
  y=0;
  yTop=0;
  from++;
  len=get24(from);
  if (len!=encSize) {
    if (len & 0x01) len++;
    if (len!=encSize) return;
  }
  from+=4;
  strips=get16(from);
  cvidMapNum=strips;

  for (kk=0; kk<strips; kk++) {
    ulong cid, y1;
    long topSize, cSize;
    struct Color2x2 *cvidCMap0=cvidData->cvidMaps0[kk];
    struct Color2x2 *cvidCMap1=cvidData->cvidMaps1[kk];
    if (cvidData->cvidVMap0[kk]==0) {
      ulong idx;
      struct Color2x2 *src, *dst;
      idx=(kk==0)?(strips-1):(kk-1);
      src=cvidData->cvidMaps0[idx];
      dst=cvidData->cvidMaps0[kk];
      cvidData->cvidVMap0[kk]=1;
      memcpy(dst, src, sizeof(struct Color2x2)*256);
    }
    if (cvidData->cvidVMap1[kk]==0) {
      ulong idx;
      struct Color2x2 *src, *dst;
      idx=(kk==0)?(strips-1):(kk-1);
      src=cvidData->cvidMaps1[idx];
      dst=cvidData->cvidMaps1[kk];
      cvidData->cvidVMap1[kk]=1;
      memcpy(dst, src, sizeof(struct Color2x2)*256);
    }
    from+=2;
    topSize=get16(from);
    from+=4;
    y1=get16(from);
    from+=2;

    yTop+=y1;
    topSize-=12;
    x=0;
    while (topSize>0) {
      cid=get16(from);
      cSize=get16(from);
      topSize-=cSize;
      cSize-=4;
      switch (cid) {
        case 0x2000:
        case 0x2200:
          { ulong i;
            struct Color2x2 *cvidMap;
            //void (*yuv2x2) (ulong y0, ulong y1, ulong y2, ulong y3, ulong u, ulong v, struct Color2x2 *cmap2x2);
            if (cid==0x2200) {
              cvidMap=cvidCMap1;
              for (i=0; i<cvidMapNum; i++) cvidData->cvidVMap1[i]=0;
              cvidData->cvidVMap1[kk]=1;
              //yuv2x2=yuv2x2_14;
            } else {
              cvidMap=cvidCMap0;
              for (i=0; i<cvidMapNum; i++) cvidData->cvidVMap0[i]=0;
              cvidData->cvidVMap0[kk]=1;
              //yuv2x2=yuv2x2_42;
            }
            cNum=cSize/6;
            for (i=0; i<cNum; i++) {
              ulong Y0, Y1, Y2, Y3, U, V;
              Y0=from[0];
              Y1=from[1];
              Y2=from[2];
              Y3=from[3];
              U=from[4];
              V=from[5];
              yuv2x2(Y0,Y1,Y2,Y3,U,V,cvidMap);
              cvidMap++;
              from+=6;
            }
          }
          break;

        case 0x2100:
        case 0x2300:
          { ulong flag, mask;
            struct Color2x2 *cvidMap;
            //void (*yuv2x2) (ulong y0, ulong y1, ulong y2, ulong y3, ulong u, ulong v, struct Color2x2 *cmap2x2);
            if (cid==0x2300) {
              cvidMap=cvidCMap1;
              //yuv2x2=yuv2x2_14;
            } else {
              cvidMap=cvidCMap0;
              //yuv2x2=yuv2x2_42;
            }
            while (cSize>0) {
              flag=*(ulong *)from;
              from+=4;
              cSize-=4;
              mask=0x80000000;
              while (mask) {
                if (mask & flag) {
                  ulong Y0, Y1, Y2, Y3, U, V;
                  Y0=from[0];
                  Y1=from[1];
                  Y2=from[2];
                  Y3=from[3];
                  U=from[4];
                  V=from[5];
                  cSize-=6;
                  from+=6;
                  yuv2x2(Y0,Y1,Y2,Y3,U,V,cvidMap);
                }
                cvidMap++;
                mask>>=1;
              }
            }
          }
          break;

        case 0x3000:
          { ulong flag;
            while ((cSize>0) && (y<yTop)) {
              ulong mask;
              flag=*(ulong *)from;
              from+=4;
              cSize-=4;
              mask=0x80000000;
              while (mask) {
                if (y>=yTop) break;
                if (mask & flag) {
                  ulong d0, d1, d2, d3;
                  d0=from[0];
                  d1=from[1];
                  d2=from[2];
                  d3=from[3];
                  cSize-=4;
                  from+=4;
                  color2x2blk4(x,y,width,&cvidCMap0[d0],&cvidCMap0[d1],&cvidCMap0[d2],&cvidCMap0[d3]);
                } else {
                  ulong d;
                  d=*from++;
                  cSize--;
                  color2x2blk1(x,y,width,&cvidCMap1[d]);
                }
                blockInc(x,y,width);
                mask>>=1;
              }
              if (cSize<4) {
                from+=cSize;
                cSize=0;
              }
            }
            if (cSize) from+=cSize;
          }
          break;

        case 0x3200:
          { while ((cSize>0) && (y<yTop)) {
              ulong d;
              d=*from++;
              cSize--;
              color2x2blk1(x,y,width,&cvidCMap1[d]);
              blockInc(x,y,width);
            }
            if (cSize) from+=cSize;
          }
          break;

        case 0x3100:
          { ulong flag, flag0, flag1, flag2;
            flag1=flag2=0;
            flag0=1;
            while ((cSize>0) && (y<yTop)) {
              ulong mCode;
              long j;
              flag=*(ulong *)from;
              from+=4;
              cSize-=4;
              for (j=30; j>=0; j-=2) {
                if (y>=yTop) break;
                mCode=(flag>>j) & 0x03;
                switch (mCode) {
                  case 0x0:
                    { flag0=1;
                      if (flag1) {
                        ulong d=*from++;
                        flag1=0;
                        cSize--;
                        color2x2blk1(x,y,width,&cvidCMap1[d]);
                      }
                      blockInc(x,y,width);
                    }
                    break;

                  case 0x1:
                    { flag1=1;
                      if ((flag2) || (flag0)) {
                        flag0=flag2=0;
                      } else {
                        ulong d=*from++;
                        cSize--;
                        color2x2blk1(x,y,width,&cvidCMap1[d]);
                      }
                    }
                    break;

                  case 0x2:
                    { flag2=1;
                      if (flag1) {
                        ulong d0, d1, d2, d3;
                        flag1=0;
                        d0=from[0];
                        d1=from[1];
                        d2=from[2];
                        d3=from[3];
                        cSize-=4;
                        from+=4;
                        color2x2blk4(x,y,width,&cvidCMap0[d0],&cvidCMap0[d1],&cvidCMap0[d2],&cvidCMap0[d3]);
                        blockInc(x,y,width);
                      } else {
                        ulong d=*from++;
                        cSize--;
                        color2x2blk1(x,y,width,&cvidCMap1[d]);
                      }
                    }
                    break;

                  case 0x3:
                    { ulong d0, d1, d2, d3;
                      d0=from[0];
                      d1=from[1];
                      d2=from[2];
                      d3=from[3];
                      cSize-=4;
                      from+=4;
                      color2x2blk4(x,y,width,&cvidCMap0[d0],&cvidCMap0[d1],&cvidCMap0[d2],&cvidCMap0[d3]);
                    }
                    break;
                }
                blockInc(x,y,width);
              }
            }
            if (cSize) from+=cSize;
          }
          break;

        case 0x2400:
        case 0x2600:
          { ulong i, cNum;
            struct Color2x2 *cvidMap;
            if (cid==0x2600) {
              cvidMap=cvidCMap1;
              for (i=0; i<cvidMapNum; i++) cvidData->cvidVMap1[i]=0;
              cvidData->cvidVMap1[kk]=1;
            } else {
              cvidMap=cvidCMap0;
              for (i=0; i<cvidMapNum; i++) cvidData->cvidVMap0[i]=0;
              cvidData->cvidVMap0[kk]=1;
            }
            cNum=cSize/4;
            for (i=0; i<cNum; i++) {
#if 0 // Some dither helper, I don't care...
              cvidMap[i].clr00=from[0];
              cvidMap[i].clr10=from[1];
              cvidMap[i].clr20=from[2];
              cvidMap[i].clr30=from[3];
#endif
              from+=4;
            }
          }
          break;

        case 0x2500:
        case 0x2700:
          { ulong flag, mask, ci;
            struct Color2x2 *cvidMap;
            if (cid==0x2700)
              cvidMap=cvidCMap1;
            else
              cvidMap=cvidCMap0;
            ci=0;
            while (cSize>0) {
              flag=*(ulong *)from;
              from+=4;
              cSize-=4;
              mask=0x80000000;
              while (mask) {
                if (mask & flag) {
#if 0 // Some dither helper, I don't care...
                  cvidMap[ci].clr00=from[0];
                  cvidMap[ci].clr10=from[1];
                  cvidMap[ci].clr20=from[2];
                  cvidMap[ci].clr30=from[3];
#endif
                  from+=4;
                  cSize-=4;
                }
                ci++;
                mask>>=1;
              }
            }
          }
          break;

        default:
          return;
          break;
      }
    }
  }
}

static int video_w, video_h;
static int cvid_error = 0;

static double current_time = 0.0;
static double current_frame_time = 0.0;
static double current_video_time = 0.0;

int decode_cvid(unsigned char *buf, unsigned long length, double sync_time)
{
  if (cvid_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    DecodeCVID(buf, video_w, video_h, length);

    if (sync_time > 0.0) {
      current_time = sync_time;
    }

    amp->video_refresh(yuv, current_time, current_video_time);
    amp->total_frames++;

    current_time += current_frame_time;
    current_video_time += current_frame_time;
  }

  return PLUGIN_OK;
}

int init_cvid(unsigned long type, unsigned long fourcc)
{
  int width, height;

  width = amp->width;
  height = amp->height;

  yuv[0] = malloc(width * height);
  yuv[1] = malloc(width * height / 4);
  yuv[2] = malloc(width * height / 4);

  SelectCVIDFuncs();

  video_w = width;
  video_h = height;

  amp->total_frames = 0;
  amp->skiped_frames = 0;

  cvid_error = 0;

  current_time = 0.0;
  current_frame_time = 1.0 / amp->framerate;
  current_video_time = 0.0;

  amp->video_refresh_yuv2rgb(8388608, -2097152, -4194304, 8388608, 4194304, -606076928, 0x00000000);

  if (amp->video_init(width, height, width, CMODE_YUV420) != PLUGIN_OK) {
    cvid_error = 1;
    return PLUGIN_FAIL;
  }

  return PLUGIN_OK;
}

int exit_cvid(void)
{
  /* FIXME: Memory leak */

  return PLUGIN_OK;
}

VERSION("CVID v1.01 (020520)");

IDENT_INIT
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_CVID, init_cvid, decode_cvid, exit_cvid)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_cvid, init_cvid, decode_cvid, exit_cvid)
IDENT_EXIT

