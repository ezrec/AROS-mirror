#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#ifndef MYIMAGE_H
#include "myimage.h"
#endif

/* IMAGEID = 'RAIM' */
#define REQATTACK_IMAGEID 0x5241494D

#define RAIMF_NOTRANSPARENCY	1
#define RAIMF_PINGPONGANIM		2
#define RAIMF_PACKED				4
#define RAIMF_CHUNKY				8

struct RAImage
{
	ULONG	id;
	ULONG	filelen;
	WORD	version;
	WORD	width;
	WORD	height;
	WORD	framewidth;
	WORD	frameheight;
	WORD	numcols;
	WORD	frames;
	WORD	animspeed;
	ULONG flags;
	WORD	reserved[8];
	UBYTE *pal;
	UBYTE *data;
};

struct FileRAImage 
{
	UBYTE	id[4];
	UBYTE	filelen[4];
	UBYTE	version[2];
	UBYTE	width[2];
	UBYTE	height[2];
	UBYTE	framewidth[2];
	UBYTE	frameheight[2];
	UBYTE	numcols[2];
	UBYTE	frames[2];
	UBYTE	animspeed[2];
	UBYTE 	flags[4];
	UBYTE	reserved[16];
	UBYTE 	pal[4];
	UBYTE 	data[4];
};

struct RAImageNode
{
	struct Node			 node;
	LONG					 allocsize;
	struct MyImageSpec spec;
	WORD					 usecount;
	char					 filename[32];
	UBYTE					 raimage[1];  /*orig=0*/
};

#define ILM_GETIMAGE  1
#define ILM_FREEIMAGE 2
#define ILM_LOGOFROMMEM 3
#define ILM_FREELOGOMEM 4

struct ImageLoaderMsg
{
	struct Message		 msg;
	struct MyImageSpec *spec;
	char					 *filename;
	WORD					 code;
};

extern ULONG imageloadermask;

void InitImageLoader(void);
void CleanupImageLoader(void);
struct MyImageSpec *SendImageLoaderMsg(WORD code,char *filename,struct MyImageSpec *spec);
struct RAImageNode *LoadRAImage(char *name);
struct MyImageSpec *GetImageSpec(char *name,char *def,char *internal);
void HandleImageLoader(void);
struct MyImageSpec *SpecificImageLoader(ULONG *filememory);

#endif

