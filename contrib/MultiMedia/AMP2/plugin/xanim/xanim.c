/*
 * xanim.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(__AROS__)
#include <string.h>
#endif

#include "types.h"
#include "dlfcn.h"

static unsigned char *chunky = NULL; /* VIDEO */

#define PRINTF if (1 == 0) printf

typedef void XA_ANIM_HDR;

typedef struct
{
  XA_ANIM_HDR *anim_hdr; /* used to add stuff to Free Chain */
  u32 compression;       /* input/output compression */
  u32 x,y;               /* input/output x,y */
  u32 depth;             /* input depth  and cfunc */
  void *extra;           /* extra for delta */
  u32 xapi_rev;          /* XAnim API rev */
  u32 (*decoder)();      /* decoder routine */
  u8 *description;       /* text string */
  u32 avi_ctab_flag;     /* AVI ctable to be read */
  u32 (*avi_read_ext)(); /* routine to read extended data */
} XA_CODEC_HDR;

#define CODEC_SUPPORTED    1
#define CODEC_UNKNOWN      0
#define CODEC_UNSUPPORTED -1

#define XAVID_WHAT_NO_MORE 0x0000
#define XAVID_AVI_QUERY    0x0001
#define XAVID_QT_QUERY     0x0002
#define XAVID_DEC_FUNC     0x0100

#define XAVID_API_REV      0x0003

typedef struct {
  u32 what;
  u32 id;
  s32 (*iq_func)();  /* init/query function */
  u32 (*dec_func)(); /* opt decode function */
} XAVID_FUNC_HDR;

typedef struct {
  u32 api_rev;
  u8 *desc;
  u8 *rev;
  u8 *copyright;
  u8 *mod_author;
  u8 *authors;
  u32 num_funcs;
  XAVID_FUNC_HDR    *funcs;
} XAVID_MOD_HDR;

typedef struct
{
  u16 red,green,blue,gray;
} ColorReg;

typedef struct XA_ACTION_STRUCT
{
 s32 type;          /* type of action */
 s32 cmap_rev;          /* rev of cmap */
 u8 *data;          /* data ptr */
 struct XA_ACTION_STRUCT *next;
 struct XA_CHDR_STRUCT *chdr;
 ColorReg *h_cmap;  /* For IFF HAM images */
 u32 *map;
 struct XA_ACTION_STRUCT *next_same_chdr; /*ptr to next action with same cmap*/
} XA_ACTION;

typedef struct XA_CHDR_STRUCT
{
 s32 rev;
 ColorReg *cmap;
 u32 csize,coff;
 u32 *map;
 u32 msize,moff;
 struct XA_CHDR_STRUCT *next;
 XA_ACTION *acts;
 struct XA_CHDR_STRUCT *new_chdr;
} XA_CHDR;

typedef struct
{
  u32 cmd;               /* decode or query */
  u32 skip_flag;         /* skip_flag */
  u32 imagex,imagey;     /* Image Buffer Size */
  u32 imaged;       /* Image depth */
  XA_CHDR *chdr;         /* Color Map Header */
  u32 map_flag;          /* remap image? */
  u32 *map;              /* map to use */
  u32 xs,ys;        /* pos of changed area */
  u32 xe,ye;        /* size of change area */
  u32 special;      /* Special Info */
  void *extra;      /* Decompression specific info */
} XA_DEC_INFO;

typedef struct
{
  u32 cmd;               /* decode or query */
  u32 skip_flag;         /* skip_flag */
  u32 imagex,imagey;     /* Image Buffer Size */
  u32 imaged;       /* Image depth */
  XA_CHDR *chdr;         /* Color Map Header */
  u32 map_flag;          /* remap image? */
  u32 *map;              /* map to use */
  u32 xs,ys;        /* pos of changed area */
  u32 xe,ye;        /* size of change area */
  u32 special;      /* Special Info */
  u32 bytes_pixel;  /* bytes per pixel */
  u32 image_type;        /* type of image */
  u32 tmp1;              /* future expansion */
  u32 tmp2;              /* future expansion */
  u32 tmp3;              /* future expansion */
  u32 tmp4;              /* future expansion */
  void *extra;      /* Decompression specific info */
} XA_DEC2_INFO;

typedef struct
{
  u8 *Ybuf;
  u8 *Ubuf;
  u8 *Vbuf;
  u8 *the_buf;
  u32 the_buf_size;
  u16 y_w,y_h;
  u16 uv_w,uv_h;
} YUVBufs;

typedef struct
{
  u32 Uskip_mask;
  s32 *YUV_Y_tab;
  s32 *YUV_UB_tab;
  s32 *YUV_VR_tab;
  s32 *YUV_UG_tab;
  s32 *YUV_VG_tab;
} YUVTabs;

static YUVBufs jpg_YUVBufs;
static YUVTabs def_yuv_tabs;

void TheEnd1(char *err_mess);

static void XA_YUV411111_To_RGB()
{ 
  PRINTF("XA_YUV411111_To_RGB\n");
}

static void *XA_YUV411111_Func() {
  PRINTF("XA_YUV411111_Func\n");
  return XA_YUV411111_To_RGB;
}

static u8 *jpg_Ybuf = NULL;
static u8 *jpg_Ubuf = NULL;
static u8 *jpg_Vbuf = NULL;
static u32 jpg_MCUbuf_size;

#define DCTSIZE2 64

void JPG_Alloc_MCU_Bufs(XA_ANIM_HDR *anim_hdr, u32 width, u32 height, u32 full_flag)
{ u32 twidth  = (width + 15) / 16;
  u32 theight = (height + 15) / 16;  if (theight & 1) theight++;

  PRINTF("JPG_Alloc_MCU_Bufs: %08lx, %ld %ld %ld\n", (u32)anim_hdr, width, height, full_flag);

  if (full_flag == 1) twidth *= (theight << 2);
  else             twidth <<= 2; // four dct's deep

/*
  if (anim_hdr) XA_Add_Func_To_Free_Chain(anim_hdr,JPG_Free_MCU_Bufs);
*/

  if (jpg_Ybuf == 0)
  { jpg_MCUbuf_size = twidth;
    jpg_Ybuf = (u8 *)malloc(twidth * DCTSIZE2);
    jpg_Ubuf = (u8 *)malloc(twidth * DCTSIZE2);
    jpg_Vbuf = (u8 *)malloc(twidth * DCTSIZE2);
  }
  else if (twidth > jpg_MCUbuf_size)
  { jpg_MCUbuf_size = twidth;
    jpg_Ybuf = (u8 *)realloc( jpg_Ybuf,(twidth * DCTSIZE2) );
    jpg_Ubuf = (u8 *)realloc( jpg_Ubuf,(twidth * DCTSIZE2) );
    jpg_Vbuf = (u8 *)realloc( jpg_Vbuf,(twidth * DCTSIZE2) );
  }
  jpg_YUVBufs.Ybuf = jpg_Ybuf;
  jpg_YUVBufs.Ubuf = jpg_Ubuf;
  jpg_YUVBufs.Vbuf = jpg_Vbuf;
}

static s32 *YUV_Y_tab = NULL;
static s32 *YUV_UB_tab = NULL;
static s32 *YUV_VR_tab = NULL;
static s32 *YUV_UG_tab = NULL;
static s32 *YUV_VG_tab = NULL;
static s32 *YUV2_UB_tab = NULL;
static s32 *YUV2_VR_tab = NULL;
static s32 *YUV2_UG_tab = NULL;
static s32 *YUV2_VG_tab = NULL;

void XA_Gen_YUV_Tabs(XA_ANIM_HDR *anim_hdr)
{
  s32 i;
  float t_ub,  t_vr,  t_ug,  t_vg;
  float t2_ub, t2_vr, t2_ug, t2_vg;

  PRINTF("XA_Gen_YUV_Tabs: %08lx\n", (u32)anim_hdr);

/*
  XA_Add_Func_To_Free_Chain(anim_hdr,XA_Free_YUV_Tabs);
*/

  if (YUV_Y_tab==0)
  {
    YUV_Y_tab =  (s32 *)malloc( 256 * sizeof(s32) );
    YUV_UB_tab = (s32 *)malloc( 256 * sizeof(s32) );
    YUV_VR_tab = (s32 *)malloc( 256 * sizeof(s32) );
    YUV_UG_tab = (s32 *)malloc( 256 * sizeof(s32) );
    YUV_VG_tab = (s32 *)malloc( 256 * sizeof(s32) );
    if (   (YUV_Y_tab==0)  || (YUV_UB_tab==0) || (YUV_VR_tab==0)
        || (YUV_UG_tab==0) || (YUV_VG_tab==0) ) 
               TheEnd1("YUV_GEN: yuv tab malloc err");

    YUV2_UB_tab = (s32 *)malloc( 256 * sizeof(s32) );
    YUV2_VR_tab = (s32 *)malloc( 256 * sizeof(s32) );
    YUV2_UG_tab = (s32 *)malloc( 256 * sizeof(s32) );
    YUV2_VG_tab = (s32 *)malloc( 256 * sizeof(s32) );
    if (   (YUV2_UB_tab==0) || (YUV2_VR_tab==0)
        || (YUV2_UG_tab==0) || (YUV2_VG_tab==0) ) 
               TheEnd1("YUV2_GEN: yuv tab malloc err");
  }

  t_ub = (1.77200/2.0) * (float)(1<<6) + 0.5;
  t_vr = (1.40200/2.0) * (float)(1<<6) + 0.5;
  t_ug = (0.34414/2.0) * (float)(1<<6) + 0.5;
  t_vg = (0.71414/2.0) * (float)(1<<6) + 0.5;
  t2_ub = (1.4 * 1.77200/2.0) * (float)(1<<6) + 0.5;
  t2_vr = (1.4 * 1.40200/2.0) * (float)(1<<6) + 0.5;
  t2_ug = (1.4 * 0.34414/2.0) * (float)(1<<6) + 0.5;
  t2_vg = (1.4 * 0.71414/2.0) * (float)(1<<6) + 0.5;

  for(i=0;i<256;i++)
  {
    float x = (float)(2 * i - 255);
    
    YUV_UB_tab[i] = (s32)( ( t_ub * x) + (1<<5));
    YUV_VR_tab[i] = (s32)( ( t_vr * x) + (1<<5));
    YUV_UG_tab[i] = (s32)( (-t_ug * x)         );
    YUV_VG_tab[i] = (s32)( (-t_vg * x) + (1<<5));
    YUV_Y_tab[i]  = (s32)( (i << 6) | (i >> 2) );

    YUV2_UB_tab[i] = (s32)( ( t2_ub * x) + (1<<5));
    YUV2_VR_tab[i] = (s32)( ( t2_vr * x) + (1<<5));
    YUV2_UG_tab[i] = (s32)( (-t2_ug * x)         );
    YUV2_VG_tab[i] = (s32)( (-t2_vg * x) + (1<<5));
  }
  def_yuv_tabs.Uskip_mask = 0;
  def_yuv_tabs.YUV_Y_tab  = YUV_Y_tab;
  def_yuv_tabs.YUV_UB_tab = YUV_UB_tab;
  def_yuv_tabs.YUV_VR_tab = YUV_VR_tab;
  def_yuv_tabs.YUV_UG_tab = YUV_UG_tab;
  def_yuv_tabs.YUV_VG_tab = YUV_VG_tab;
}

#define MAXJSAMPLE 255
#define CENTERJSAMPLE 128

static u8 *jpg_samp_limit = NULL;
static u8 *xa_byte_limit = NULL;

void JPG_Setup_Samp_Limit_Table(XA_ANIM_HDR *anim_hdr)
{
  u8 *table;
  s32 i;

  PRINTF("JPG_Setup_Samp_Limit_Table: %08lx\n", (u32)anim_hdr);

  if (jpg_samp_limit==0)
  {
     jpg_samp_limit = (u8 *)malloc((5 * (MAXJSAMPLE+1) + CENTERJSAMPLE));
     if (jpg_samp_limit==0) 
     { printf("JJ: samp limit malloc err\n");
       return;
     } /*POD note improve this */
  }
  else return; /* already done */

/*
  if (anim_hdr) XA_Add_Func_To_Free_Chain(anim_hdr,JPG_Free_Samp_Limit_Table);
*/

  xa_byte_limit = jpg_samp_limit + (MAXJSAMPLE + 1);

  table = jpg_samp_limit;
  table += (MAXJSAMPLE+1);   /* allow negative subscripts of simple table */

  /* First segment of "simple" table: limit[x] = 0 for x < 0 */
  memset(table - (MAXJSAMPLE+1), 0, (MAXJSAMPLE+1));

  /* Main part of "simple" table: limit[x] = x */
  for (i = 0; i <= MAXJSAMPLE; i++) table[i] = (u8) i;

  table += CENTERJSAMPLE;       /* Point to where post-IDCT table starts */
  /* End of simple table, rest of first half of post-IDCT table */

  for (i = CENTERJSAMPLE; i < 2*(MAXJSAMPLE+1); i++) table[i] = MAXJSAMPLE;

  /* Second half of post-IDCT table */
  memset(table + (2 * (MAXJSAMPLE+1)), 0, (2 * (MAXJSAMPLE+1) - CENTERJSAMPLE));
  memcpy(table + (4 * (MAXJSAMPLE+1) - CENTERJSAMPLE),
          (u8 *)(jpg_samp_limit+(MAXJSAMPLE+1)), CENTERJSAMPLE);
}

#define xaMIN(x,y)   ( ((x)>(y))?(y):(x) )

#define XA_YUV_VARS \
 u32 mx = xaMIN(i_x,imagex); u32 my = xaMIN(i_y,imagey);    \
 u8 *rnglimit = xa_byte_limit; u8 *ybuf,*ubuf,*vbuf;   \
 s32 *YTab,*UBTab,*VRTab,*UGTab,*VGTab

#define XA_YUV_INIT \
  YTab  = yuv_tabs->YUV_Y_tab;     \
  UBTab = yuv_tabs->YUV_UB_tab; VRTab = yuv_tabs->YUV_VR_tab;    \
  UGTab = yuv_tabs->YUV_UG_tab; VGTab = yuv_tabs->YUV_VG_tab;    \
  ybuf = yuv_bufs->Ybuf; ubuf = yuv_bufs->Ubuf; vbuf = yuv_bufs->Vbuf

#define DEC_YUV_CALC(u,v,V2R,U2B,UV2G) { \
  V2R = VRTab[v];   \
  U2B = UBTab[u];   \
  UV2G = VGTab[v] + UGTab[u]; \
}

#define DEC_YUV_RGB(y,V2R,U2B,V2G,ip)   \
{ register s32 _r,_g,_b,YY;        \
  YY = YTab[y];                    \
  _r = (s32)rnglimit[(YY + V2R) >> 6];  \
  _g = (s32)rnglimit[(YY + UV2G) >> 6]; \
  _b = (s32)rnglimit[(YY + U2B) >> 6];  \
  ip++; *ip++ = (u8)_r; *ip++ = (u8)_g; *ip++ = (u8)_b; \
}

#define XA_YUV16_Y_VARS(IP_CAST)   \
  IP_CAST *iptr = (IP_CAST *)image;     u8 *yptr = ybuf;    \
  u8 *up = ubuf; u8 *vp = vbuf

#define XA_YUV16_X_VARS  u32 iU,iV; s32 V2R,U2B,UV2G

/* Use yuv_bufs->uv_w if defined, else use ix>>2 */
#define UVW16_KLUDGE(uvw,ix) ( (((uvw)?(uvw):((ix)>>2) + 3) >> 2) << 2)

void XA_YUV1611_To_RGB(u8 *image, u32 imagex, u32 imagey, u32 i_x, u32 i_y, YUVBufs *yuv_bufs, YUVTabs *yuv_tabs, u32 map_flag, u32 *map, XA_CHDR *chdr)
{ XA_YUV_VARS;
  u32 x,y;
//  u32 i_rowinc = 3 * (imagex - 4);
  u32 i_rowinc = 4 * (imagex - 4);
  u32 y_rowinc = i_x - 3;
//  u32 i_rowinc12 = 3 * (imagex << 2);
  u32 i_rowinc12 = 4 * (imagex << 2);
  u32 uv_inc = UVW16_KLUDGE(yuv_bufs->uv_w,i_x);
  register u32 USkip_Mask = yuv_tabs->Uskip_mask;
  XA_YUV_INIT;

  PRINTF("XA_YUV1611_To_RGB: %ld %ld %ld %ld : %ld\n", imagex, imagey, i_x, i_y, map_flag);

  for(y=0; y<my; y += 4)
  { XA_YUV16_Y_VARS(u8);
    for(x=0; x<mx; x += 4)
    { u32 iU,iV;
     u8 *ip = iptr; u8 *yp = yptr;
     s32 V2R,U2B,UV2G;

//     iptr += 12;    yptr += 4;
     iptr += 16;    yptr += 4;
     iU = (*up++);       iV = (*vp++);  
     if (iU & USkip_Mask) continue;  /* skip */
     DEC_YUV_CALC(iU,iV,V2R,U2B,UV2G);

     /* Row 0 */
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp  ,V2R,U2B,UV2G,ip);
     /* Row 1 */ ip += i_rowinc;  yp += y_rowinc;
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp  ,V2R,U2B,UV2G,ip);
     /* Row 2 */ ip += i_rowinc;  yp += y_rowinc;
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp  ,V2R,U2B,UV2G,ip);
     /* Row 3 */ ip += i_rowinc;  yp += y_rowinc;
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp++,V2R,U2B,UV2G,ip);
     DEC_YUV_RGB(*yp  ,V2R,U2B,UV2G,ip);
    } /* end x */
    image += i_rowinc12;
    ybuf += (i_x << 2);
    ubuf += uv_inc;
    vbuf += uv_inc;
  } /* end y */

} /* end function */

void *XA_YUV1611_Func(u32 image_type)
{
  PRINTF("XA_YUV1611_Func, image_type: %08lx (%ld)\n", image_type, image_type);
  return XA_YUV1611_To_RGB;
}

s32 XA_Time_Read()
{
  PRINTF("XA_Time_Read\n");
  return 0;
}

void XA_Add_Func_To_Free_Chain(XA_ANIM_HDR *anim_hdr, void (*function)())
{
  PRINTF("XA_Add_Func_To_Free_Chain\n");
}

#include <stdarg.h>

void XA_Print(char *fmt, ...)
{
/*
  va_list vallist;
  va_start(vallist, fmt);
  vfprintf(stderr,fmt,vallist);
  va_end(vallist);
*/
}

void TheEnd1(char *err_mess)
{
  PRINTF("%s\n", err_mess);

  PRINTF("TheEnd\n");
}

/**/

void *_IO_stderr_ = NULL; /* FIXME */

void *__memcpy(void *dst, void *src, size_t size) { /*printf("__memcpy from %08lx to %08lx with %ld bytes\n", (u32)src, (u32)dst, (u32)size);*/ return memcpy(dst, src, size); }
void *__malloc(size_t size) { void *ptr = malloc(size); /*printf("__malloc: %ld @ %08lx\n", (u32)size, (u32)ptr); memset(ptr, 0, size); */ return ptr; }
void *__realloc(void *old, size_t size) { void *ptr = realloc(old, size); /* printf("__realloc: %ld @ %08lx\n", (u32)size, (u32)ptr); */ return ptr; }
void *__memset(void *ptr, int val, size_t size) { /*printf("__memset: %08lx @ %08lx length %ld\n", (u32)val, (u32)ptr, (u32)size);*/ return memset(ptr, val, size); }
void __free(void *ptr) { /*printf("__free\n");*/ free(ptr); }

static int __fprintf(FILE *fp, const char *fmt, ...) {
/*
  va_list vallist;
printf("__fprintf\n");
  va_start(vallist, fmt);
  vfprintf(stderr,fmt,vallist);
  va_end(vallist);
*/
  return 0;
}

/* IV4 only */
int __rand(void) { /*printf("rand\n");*/ return rand(); }
void __srand(unsigned __seed) { /*printf("srand\n");*/ srand(__seed); }
#include <setjmp.h>
#if 0
void __longjmp (jmp_buf buf, int val) { /*printf("longjump: %d\n", val);*/ siglongjmp(buf, val); }
int __sigsetjmp(jmp_buf env, int savemask) { /*printf("__sigsetjmp: %d\n", savemask);*/ return sigsetjmp(env, savemask); }
#else
static void __longjmp (jmp_buf buf, int val) { /*printf("longjump: %d\n", val);*/ }
static int __sigsetjmp(jmp_buf env, int savemask) { /*printf("__sigsetjmp: %d\n", savemask);*/ return 0; }
#endif

/* PPC only */
void __bzero(void *ptr, size_t size) { /*printf("__bzero: %08lx length %ld\n", (u32)ptr, (u32)size);*/ memset(ptr, 0, size); }
double __sqrt(double val) { /*printf("sqrt\n");*/ return sqrt(val); }

/* PPC, IV5 */
static FILE *__stderr;

reloc_t reloc[] = {
  { "jpg_YUVBufs", &jpg_YUVBufs },
  { "XA_YUV411111_Func", &XA_YUV411111_Func },
  { "XA_YUV1611_Func", &XA_YUV1611_Func },
  { "def_yuv_tabs", &def_yuv_tabs },
  { "JPG_Alloc_MCU_Bufs", &JPG_Alloc_MCU_Bufs },
  { "XA_Gen_YUV_Tabs", &XA_Gen_YUV_Tabs },
  { "JPG_Setup_Samp_Limit_Table", &JPG_Setup_Samp_Limit_Table },
  { "XA_Time_Read", &XA_Time_Read },
  { "XA_Add_Func_To_Free_Chain", &XA_Add_Func_To_Free_Chain },
  { "XA_Print", &XA_Print },
  { "TheEnd1", &TheEnd1 },

  { "_IO_stderr_", &_IO_stderr_ },

  { "memcpy", &__memcpy },
  { "malloc", &__malloc },
  { "realloc", &__realloc },
  { "memset", &__memset },
  { "free", &__free },

  { "fprintf", &__fprintf },

/* IV4 only */
  { "rand", &__rand },
  { "srand", &__srand },
  { "longjmp", &__longjmp },
  { "__sigsetjmp", &__sigsetjmp },

/* PPC only */

  { "__bzero", &__bzero },
  { "sqrt", &__sqrt },

/* PPC, IV5 */

  { "stderr", &__stderr },

  { NULL, NULL},
};

/* MAIN */

static XAVID_MOD_HDR *mod_hdr;
static XA_CODEC_HDR *codec_hdr;
static XA_DEC_INFO *dec_info;
static XA_DEC2_INFO *dec2_info;

static u32 avi_read_ext()
{
  /* routine to read extended data */
  printf("avi_read_ext\n");
  return 0;
}

static void *handle = NULL;

int xanim_init(short w, short h, int format, char *path)
{
  XAVID_MOD_HDR *(*what_the)();
  XAVID_FUNC_HDR *func;
  s32 i, res;
  char dll[256];

  PRINTF("%dx%d with %08x\n", w, h, format);

//  #define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))
  #define MKTAG(a,b,c,d) (d | (c << 8) | (b << 16) | (a << 24))

  switch(format) {
    case MKTAG('i', 'v', '3', '1'):
    case MKTAG('I', 'V', '3', '1'):
    case MKTAG('i', 'v', '3', '2'):
    case MKTAG('I', 'V', '3', '2'):
      printf("IV32\n");
//      handle = dlopen("vid_iv32_2.1_linuxELFx86.xa", RTLD_NOW);
sprintf(dll, "%svid_iv32_2.1_linuxELFppc.xa", path);
      handle = dlopen(dll, RTLD_NOW);
      break;

    case MKTAG('i', 'v', '4', '1'):
    case MKTAG('I', 'V', '4', '1'):
      printf("IV41\n");
//      handle = dlopen("vid_iv41_1.1_linuxELFx86.xa", RTLD_NOW);
sprintf(dll, "%svid_iv41_1.1_linuxELFppc.xa", path);
      handle = dlopen(dll, RTLD_NOW);
      break;

    case MKTAG('i', 'v', '5', '0'):
    case MKTAG('I', 'V', '5', '0'):
      printf("IV50\n");
//      handle = dlopen("vid_iv50_1.0_linuxELFx86.xa", RTLD_NOW);
sprintf(dll, "%svid_iv50_1.0_linuxELFppc.xa", path);
      handle = dlopen(dll, RTLD_NOW);
      break;

    default:
      printf("unsupported codec\n");
      return 1;
  }

  if (handle == NULL) {
    printf("failed to open library: %s\n", dlerror());
    return 1;
  }

  what_the = dlsym(handle, "What_The");
  mod_hdr = what_the();

  printf("%ld\n", mod_hdr->api_rev);
  printf("%s\n", mod_hdr->desc);
  printf("%s\n", mod_hdr->rev);
  printf("%s\n", mod_hdr->copyright);
  printf("%s\n", mod_hdr->mod_author);
  printf("%s\n", mod_hdr->authors);
  printf("%ld\n", mod_hdr->num_funcs);

  #define swap32(val) ({ unsigned long x = val; x = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24); x; })

  codec_hdr->x = w;
  codec_hdr->y = h;
  codec_hdr->depth = 24; /* FIXME */
  codec_hdr->compression = format; //swap32(format);
  codec_hdr->avi_read_ext = avi_read_ext; /* FIXME */

  /* allocate some resources */
#if 1
  JPG_Alloc_MCU_Bufs(codec_hdr->anim_hdr, codec_hdr->x, codec_hdr->y, 1);
  JPG_Setup_Samp_Limit_Table(codec_hdr->anim_hdr);
  XA_Gen_YUV_Tabs(codec_hdr->anim_hdr);
#endif

  func = mod_hdr->funcs;
  for(i=0; i<mod_hdr->num_funcs; i++) {
    PRINTF("QUERY (what: %08lx, id: %08lx): ", func[i].what, func[i].id);
    if (func[i].what & XAVID_AVI_QUERY) PRINTF("AVI ");
    if (func[i].what & XAVID_QT_QUERY) PRINTF("QT ");
    if (func[i].what & XAVID_DEC_FUNC) PRINTF("DEC ");
    PRINTF("\n");

    PRINTF("iq: %08lx, dec: %08lx\n", (u32)func[i].iq_func, (u32)func[i].dec_func);

    res = func[i].iq_func(codec_hdr);
    PRINTF("query: %ld\n", res);

    if (res == CODEC_SUPPORTED) {
      PRINTF("%ld %08lx %s\n", codec_hdr->xapi_rev, (u32)codec_hdr->decoder, codec_hdr->description);
    } else {
      PRINTF("codec not supported\n");
      return 1; /* FIXME */
    }
  }

  PRINTF("init done...\n");
  return 0;
}

int xanim_decode(void *data, int len)
{
  int a;

#define XA_DEC_SPEC_RGB       0x0001
#define XA_DEC_SPEC_CF4       0x0002
#define XA_DEC_SPEC_DITH 0x0004

#define XA_IMTYPE_RGB         0x0001
#define XA_IMTYPE_GRAY        0x0002
#define XA_IMTYPE_CLR8        0x0003
#define XA_IMTYPE_CLR16       0x0004
#define XA_IMTYPE_CLR32       0x0005
#define XA_IMTYPE_332         0x0006
#define XA_IMTYPE_332DITH     0x0007
#define XA_IMTYPE_CF4         0x0008
#define XA_IMTYPE_CF4DITH     0x0009

  dec2_info->imagex = codec_hdr->x;
  dec2_info->imagey = codec_hdr->y;
  dec2_info->imaged = codec_hdr->depth;

  dec2_info->special = XA_DEC_SPEC_RGB;
//  dec2_info->extra = codec_hdr->extra;

  dec2_info->tmp1 = dec2_info->tmp2 = dec2_info->tmp3 = dec2_info->tmp4 = 0;

//  dec2_info->bytes_pixel = 3;

  dec2_info->image_type = XA_IMTYPE_RGB;


//  dec2_info->cmd = 0;               /* decode or query */
//  dec2_info->skip_flag = 0;         /* skip_flag */


#if 0
  else if (dlta_hdr->xapi_rev == 0x0002)
  { dec2_info->imagex    = xa_imagex;
    dec2_info->imagey    = xa_imagey;
    dec2_info->imaged    = xa_imaged;
    dec2_info->chdr = act->chdr;
    dec2_info->map_flag  = map_flag;
    dec2_info->map  = remap_map;
    dec2_info->special   = dlta_hdr->special;
    dec2_info->extra     = dlta_hdr->extra;
    dec2_info->tmp1 = dec2_info->tmp2 = dec2_info->tmp3 = dec2_info->tmp4 = 0;
    dec2_info->bytes_pixel    = x11_bytes_pixel;
    dec2_info->image_type = XA_Get_Image_Type( dlta_hdr->special,
                                   act->chdr,map_flag);
/* POD or in dither and cf4 into special */
    if (xa_dither_flag)  dec2_info->special |= XA_DEC_SPEC_DITH;
/* XA_DEC_SPEC_CF4 */

    if (xa_skip_video == 0) { xa_skip_cnt = 0; dec2_info->skip_flag = 0; }
    else { if (xa_skip_cnt < XA_SKIP_MAX) dec2_info->skip_flag= xa_skip_video;
        else { xa_skip_cnt = 0; dec2_info->skip_flag = 0; } }

    the_pic = xa_pic; xbuff = xa_imagex; ybuff = xa_imagey;
    if ((xa_vid_fd >= 0) && (!(dlta_hdr->flags & DLTA_DATA)) )
    { int ret = XA_Read_Delta(xa_vidcodec_buf, xa_vid_fd,
                         dlta_hdr->fpos, dlta_hdr->fsize);
      /* if read fails */
      if (ret == xaFALSE) dlta_flag = ACT_DLTA_NOP;
      else dlta_flag = dlta_hdr->delta(the_pic,xa_vidcodec_buf,
                         dlta_hdr->fsize,dec2_info);
    }
    else
    { dlta_flag = dlta_hdr->delta(the_pic,dlta_hdr->data,
                         dlta_hdr->fsize,dec2_info);
    }
    if ( (dlta_flag == ACT_DLTA_DROP) || (dec2_info->skip_flag > 0) )
     { xa_skip_cnt++; xa_frames_skipd++; return; }
#endif

  PRINTF("decode: %d\n", len);

  a = codec_hdr->decoder(chunky, data, len, dec2_info);
  PRINTF("res: %d\n", a);

  return a;
}

/* */

#include <plugin.h>

static int video_w, video_h, video_gray;
static int xanim_error = 0;

static double current_time = 0.0;
static double current_frame_time = 0.0;
static double current_video_time = 0.0;

int decode_xanim(unsigned char *buf, unsigned long length, double sync_time)
{
  if (xanim_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    int result;

    result = xanim_decode(buf, length);

    if (sync_time > 0.0) {
      current_time = sync_time;
    }

    if (result == 0) {
      amp->video_refresh(&chunky, current_time, current_video_time);
    }

    amp->total_frames++;

    current_time += current_frame_time;
    current_video_time += current_frame_time;
  }

  return PLUGIN_OK;
}

int init_xanim(unsigned long type, unsigned long fourcc)
{
  int width, height;

  width = (amp->width + 3) & ~3;
  height = (amp->height + 3) & ~3;

  video_w = width;
  video_h = height;
  video_gray = amp->gray;

  __stderr = stderr; /* huh ? */

  codec_hdr = malloc(sizeof(*codec_hdr));
  memset(codec_hdr, 0, sizeof(*codec_hdr));

  dec_info = malloc(sizeof(*dec_info));
  memset(dec_info, 0, sizeof(*dec_info));

  dec2_info = malloc(sizeof(*dec2_info));
  memset(dec2_info, 0, sizeof(*dec2_info));

  chunky = malloc(video_w * video_h * 4);
  memset(chunky, 0, video_w * video_h * 4);

  xanim_error = xanim_init(video_w, video_h, fourcc, amp->plugin_path);

  if (xanim_error != 0) {
    return PLUGIN_FAIL;
  }

  amp->total_frames = 0;
  amp->skiped_frames = 0;

  xanim_error = 0;

  current_time = 0.0;
  current_frame_time = 1.0 / amp->framerate;
  current_video_time = 0.0;

  if (amp->video_init(width, height, width, CMODE_ARGB32) != PLUGIN_OK) {
    xanim_error = 1;
    return PLUGIN_FAIL;
  }

  return PLUGIN_OK;
}

int exit_xanim(void)
{
  if (handle != NULL) {
    dlclose(handle);
  }

  return PLUGIN_OK;
}

VERSION("XANIM v1.00 (020831)");

#define FOURCC_iv31 FOURCC( 'i', 'v', '3', '1')
#define FOURCC_IV31 FOURCC( 'I', 'V', '3', '1')
#define FOURCC_iv32 FOURCC( 'i', 'v', '3', '2')
#define FOURCC_IV32 FOURCC( 'I', 'V', '3', '2')
#define FOURCC_iv41 FOURCC( 'i', 'v', '4', '1')
#define FOURCC_IV41 FOURCC( 'I', 'V', '4', '1')
#define FOURCC_iv50 FOURCC( 'i', 'v', '5', '0')
#define FOURCC_IV50 FOURCC( 'I', 'V', '5', '0')

IDENT_INIT
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_iv31, init_xanim, decode_xanim, exit_xanim)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_IV31, init_xanim, decode_xanim, exit_xanim)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_iv32, init_xanim, decode_xanim, exit_xanim)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_IV32, init_xanim, decode_xanim, exit_xanim)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_iv41, init_xanim, decode_xanim, exit_xanim)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_IV41, init_xanim, decode_xanim, exit_xanim)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_iv50, init_xanim, decode_xanim, exit_xanim)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_IV50, init_xanim, decode_xanim, exit_xanim)
IDENT_EXIT
