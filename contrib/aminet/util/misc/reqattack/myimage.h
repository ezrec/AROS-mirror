#ifndef MYIMAGE_H
#define MYIMAGE_H

#ifdef RGB32
#undef RGB32
#endif

#define RGB32(x) ((((ULONG)(x)) << 24) + (((ULONG)(x)) << 16) + (((ULONG)(x)) << 8) + (((ULONG)(x))))

struct MyImageSpec
{
	ULONG *data;
	UBYTE *col;
	WORD width;
	WORD height;
	WORD numcols;
	WORD framewidth;
	WORD frameheight;
	WORD frames;
	WORD animspeed;
	WORD flags;
};

#define MYIMSPF_EXTERNAL 1
#define MYIMSPF_NOTRANSP 2
#define MYIMSPF_PINGPONG 4
#define MYIMSPF_PACKED   8
#define MYIMSPF_CHUNKY	 16

struct MyImage
{
	struct BitMap *bm;
	struct BitMap *helpbm;
	struct ColorMap *cm;
	APTR chunkydata;
	APTR mask;
	WORD *coltable;
	WORD width,height,framewidth,frameheight,
		  frames,animspeed,depth,numcols;
	WORD flags;
};

#define MYIMF_PENSALLOCED 1
#define MYIMF_IMAGEOK	  2
#define MYIMF_NOTRANSP	  4
#define MYIMF_PINGPONG	  8
#define MYIMF_CHUNKY		  16

#define MYIMAGEOK(x) ((x)->flags & MYIMF_IMAGEOK)

#endif

