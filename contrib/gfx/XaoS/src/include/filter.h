#ifndef IMAGE_H
#define IMAGE_H
#include "fconfig.h"
typedef unsigned char pixel_t;
typedef unsigned char rgb_t[4];	/*4 is better than 3 - makes multiplying easier */
struct palette
  {
    int start;
    int end;
    int maxentries;
    int version;
    int type;
    unsigned int *pixels;
    rgb_t *rgb;
    int flags;
    int (*alloccolor) (struct palette * pal, int init, int r, int g, int b);
    void (*setpalette) (struct palette * pal, int start, int end, rgb_t * rgb);
    void (*allocfinished) (struct palette * pal);
    void (*cyclecolors) (struct palette * pal, int direction);
    int size;			/*number of allocated color entries */
    void *data;			/*userdata */
  };
struct image
  {
    float pixelwidth, pixelheight;
    pixel_t **oldlines;
    pixel_t **currlines;
    void (*flip) (struct image * img);
    int width, height, nimages;
    int bytesperpixel;
    int currimage;
    int flags;
    int scanline;
    int version;
    struct palette *palette;
    void *data;			/*userdata */
  };
#define interpol1(i1,i2,n,mask) ((((i1)&(mask))*(n)+((i2)&(mask))*(256-(n)))&((mask)<<8))
#define interpol(i1,i2,n,mr,mg,mb) ((interpol1(i1,i2,n,mr)+interpol1(i1,i2,n,mg)+interpol1(i1,i2,n,mb))>>8)
/*#define interpol(i1,i2,n,mr,mg,mb) ((interpol1(i1,i2,n,mr|mb)+interpol1(i1,i2,n,mg))>>8) */
#define intertruec(i1,i2,n) interpol(i1,i2,n,255,(255UL<<8),(255UL<<16))
#define intertruecmi(i1,i2,n) (interpol((i1)>>8,(i2)>>8,n,(255UL<<16),(255UL<<8),255UL) << 8)
#define interrealc(i1,i2,n) interpol(i1,i2,n,31,992,31744)
#define interhic(i1,i2,n) interpol(i1,i2,n,31,2016,(31744*2))
#define intergray(i1,i2,n) (((i1)*n+(i2)*(256-(n)))>>8)
#define interpoltype(type,i1,i2,n) (type==HICOLOR?interhic(i1,i2,n):(type==REALCOLOR?interrealc(i1,i2,n):(type==GRAYSCALE?intergray(i1,i2,n):(type==TRUECOLORMI?intertruecmi(i1,i2,n):intertruec(i1,i2,n)))))
/*palette flags */
#define UNKNOWNENTRIES 1
#define DONOTCHANGE 2
#define FINISHLATER 4
#define UNFINISHED 8
/*image flags */
#define FREELINES 1
#define FREEDATA 2
/*palette types */
#define C256 1
#define REALCOLOR 2
#define HICOLOR 4
#define TRUECOLOR24 8
#define TRUECOLOR 16
#define FIXEDCOLOR 32
#define GRAYSCALE 64
			   /*actualy required by many windowed systems like plan9 or
			      BeOS. In this case set_range function has oposite meaning-
			      read current palette. It is called by mkpalette to get
			      range. Then default nearest color searching set_color is
			      sebmited */
#define TRUECOLORMI 128    /*Missordered BeOSish truecolor*/
#define LBITMAP 256
#define MBITMAP 512
#define LIBITMAP 1024
#define MIBITMAP 2048
/*special mage types used internaly by XaoS */
#define LARGEITER 4096
#define SMALLITER (4096*2)
#define TRUECOLORS (TRUECOLOR|TRUECOLORMI)
#define ALLMASK (C256|HICOLOR|TRUECOLOR|REALCOLOR|TRUECOLOR24|LARGEITER|GRAYSCALE|TRUECOLORMI)
#define BITMAPS (LBITMAP|MBITMAP|LIBITMAP|MIBITMAP)
#define MASK1BPP (SMALLITER|C256|GRAYSCALE)
#define MASK2BPP (LARGEITER|HICOLOR|REALCOLOR)
#define MASK3BPP (TRUECOLOR24)
#define MASK4BPP (TRUECOLORS)
/*flags for requirements */
#define IMAGEDATA 1
#define TOUCHIMAGE 2
#define NEWIMAGE 4
/*flags for initdata */
#define DATALOST 1
/*flags for doit */
#define INTERRUPTIBLE 1
#define PALETTEONLY 2
/*return flags */
#define INEXACT 1
#define CHANGED 2
#define ANIMATION 4
#define UNCOMPLETTE (1<<29)
/*flags for filters */
#define ALLOCEDIMAGE 1		/*used by inherimage mechanizm */
#define SHAREDDATA 2

#define PALGORITHMS 3
#ifdef _plan9_
#undef pixel32_t
#undef pixel8_t
#undef pixel16_t
#define pixel32_t unsigned int
#define pixel16_t unsigned short
#define pixel8_t unsigned char
#undef ppixel8_t
#undef ppixel16_t
#undef ppixel24_t
#undef ppixel32_t
#define ppixel8_t pixel8_t *
#define ppixel16_t pixel16_t *
#define ppixel24_t unsigned char *
#define ppixel32_t pixel32_t *
#else
#include <pixel_t.h>		/*avoid problems with plan9-it ignores #if
				   So code must be separated into another file */
#endif
#define imgetpixel(image,x,y) ((image)->bytesperpixel==1?(image)->currlines[y][x]:((image)->bytesperpixel==4?((pixel32_t*)(image)->currlines[y])[x]:(image)->bytesperpixel==3?(((pixel16_t *)(image)->currlines[y])[x]+((image)->currlines[y][3*(x)+2]<<16)):(((pixel16_t*)(image)->currlines[y])[x])))
struct requirements
  {
    int nimages;
    int supportedmask;
    int flags;
  };
struct filter
  {
    struct filter *next, *previous;
    struct queue *queue;
    struct filteraction *action;
    struct image *image, *childimage;
    struct requirements req;
    struct fractal_context *fractalc;
    void *data;
    char *name;
    int flags;
    int imageversion;		/*For detection whether image changed or not */
    void (*wait_function) (struct filter * f);
    /*stuff for wait_function */
    int pos, max, incalculation, readyforinterrupt, interrupt;
    char *pass;
  };
struct initdata
  {
    void (*wait_function) (struct filter * f);
    struct image *image;
    struct fractal_context *fractalc;
    int flags;
  };
struct filteraction
  {
    char *name;
    char *shortname;
    int flags;
    struct filter *(*getinstance) (struct filteraction * a);
    void (*destroyinstance) (struct filter * f);
    int (*doit) (struct filter * f, int flags, int time);
    int (*requirement) (struct filter * f, struct requirements * r);
    int (*initialize) (struct filter * f, struct initdata * i);
    void (*convertup) (struct filter * f, int *x, int *y);
    void (*convertdown) (struct filter * f, int *x, int *y);
    void (*removefilter) (struct filter * f);
  };
struct queue
  {
    struct filter *first, *last;
    int isinitialized;
    struct filter *palettechg;
    struct image *saveimage;
  };


#define datalost(f,i) (((i)->flags&DATALOST)||((f)->imageversion&&(f)->imageversion!=(i)->image->version))
/*filter actions */

extern unsigned int col_diff[3][512];
struct filter *createfilter (struct filteraction *fa);
struct queue *create_queue (struct filter *f);
void insertfilter (struct filter *f1, struct filter *f2);
void removefilter (struct filter *f);
void addfilter (struct filter *f1, struct filter *f2);
int initqueue (struct queue *q);

/*Filter utility functions */
int reqimage (struct filter *f, struct requirements *req, int supportedmask, int flags);
int inherimage (struct filter *f, struct initdata *data, int flags, int width, int height, struct palette *palette, float pixelwidth, float pixelheight);
void destroyinheredimage (struct filter *f);
void updateinheredimage (struct filter *f);

void inhermisc (struct filter *f, struct initdata *i);

/*image actions */

void flipgeneric (struct image *img);
struct image *create_image_lines (int width, int height,
			  int nimages, pixel_t ** lines1, pixel_t ** lines2,
				  struct palette *palette, void (*flip) (struct image * img), int flags, float pixelwidth, float pixelheight);
struct image *create_image_cont (int width, int height, int scanlinesize,
				 int nimages, pixel_t * buf1, pixel_t * buf2,
				 struct palette *palette, void (*flip) (struct image * img), int flags, float pixelwidth, float pixelheight);
struct image *create_image_mem (int width, int height, int nimages, struct palette *palette, float pixelwidth, float pixelheight);
struct image *create_subimage (struct image *simg, int width, int height, int nimages, struct palette *palette, float pixelwidth, float pixelheight);

void destroy_image (struct image *img);
void clear_image (struct image *img);

/*palette */

int 
bytesperpixel (int type) CONSTF;
     void bestfit_init (void);
     struct palette *createpalette (int start, int end, int type, int flags, int maxentries,
    int (*alloccolor) (struct palette * pal, int init, int r, int g, int b),
   void (*setcolor) (struct palette * pal, int start, int end, rgb_t * rgb),
			       void (*allocfinished) (struct palette * pal),
		 void (*cyclecolors) (struct palette * pal, int direction));
     void destroypalette (struct palette *palette);
     int mkdefaultpalette (struct palette *palette);
     int mkstereogrampalette (struct palette *palette);
     int mkstarfieldpalette (struct palette *palette);
     int mkblurpalette (struct palette *palette);
     int mkgraypalette (struct palette *palette);
     int mkrgb (struct palette *palette);
     int mkpalette (struct palette *palette, int seed, int algorithm);
     int shiftpalette (struct palette *palette, int n);
     struct palette *clonepalette (struct palette *palette);
     void restorepalette (struct palette *dest, struct palette *src);
     void convertupgeneric (struct filter *f, int *x, int *y);
     void convertdowngeneric (struct filter *f, int *x, int *y);
     int fixedalloccolor (struct palette *palette, int init, int r, int g, int b) CONSTF;

#define setfractalpalette(f,p) if((f)->fractalc->palette==(f)->image->palette) (f)->fractalc->palette=(p)

#define TRUECOLORCASE(x) case 4:x;break
#define C256CASE(x) case 1:x;break

#ifdef STRUECOLOR24
#define TRUECOLOR24CASE(x) case 3:x;break;
#else
#define TRUECOLOR24CASE(x)
#endif

#ifdef SLARGEITER
#define SUPPORT16
#else
#ifdef SHICOLOR
#define SUPPORT16
#else
#ifdef SREALCOLOR
#define SUPPORT16
#endif
#endif
#endif
#ifdef SUPPORT16
#define TRUECOLOR16CASE(x) case 2:x;break;
#else
#define TRUECOLOR16CASE(x)
#endif

#define drivercall(i,x1,x2,x3,x4) switch((i).bytesperpixel) { \
  case 1:x1;break; \
  TRUECOLOR16CASE(x2);break; \
  TRUECOLOR24CASE(x3); \
  case 4:x4;break; \
}
#ifdef SMBITMAPS
#define SBITMAPS
#else
#ifdef SLBITMAPS
#define SBITMAPS
#endif
#endif

#ifdef SBITMAPS
#define SCONVERTORS
#else
#ifdef SFIXEDCOLOR
#define SCONVERTORS
#endif
#endif

#include "fractal.h"
#endif
