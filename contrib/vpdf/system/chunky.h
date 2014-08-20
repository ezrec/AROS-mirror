/*
	chunky.h

	Chunky image structure definition.
	v1.0 translated from AmigaE.
*/

#ifndef	_CHUNKY_H
#define	_CHUNKY_H

typedef	union
{
	unsigned char	*dataB;
	unsigned int	*dataL;
	unsigned short	*dataW;
} dataPointer;

typedef	struct	chkimage
{
	int	width,height;
    union
	{
		unsigned char	*b;
		unsigned int	*l;
		unsigned short	*w;
		void			*p;
	}data ;
	int	pixfmt;					/*	image format (CHK_PIXFMTXXX) */
} ChkImage;

typedef	struct	texture
{
	struct	texture	*next;		/* for list support */
	struct	texture	*prev;

	ChkImage	*image;
	int		private;			/* is texture used as private in engine */

	char	*name;				/* texture filename */
	char	*alphaname;			/* texture's alphaimage filename */
	int		openc;				/* opening count */
	ChkImage	*second;		/* this is allocated with txt_AllocSecondBuffer() */

	float	x0,y0,scale;
	float	x,y;
	int		filtered;
	int		additive;
	int		substractive;
	float	transparency;

	void	*rendererTexture;	/* renderer specific data */
	void	*env;				/* enviroment. used by some functions. */

	char	*type;				/* */

} Texture;

ChkImage*		chkCreate( int width, int height, int format, int zero );
void			chkFree(ChkImage *image);

ChkImage*		chk_AllocImageStruct(int,int);


ChkImage*		chk_Duplicate(struct chkimage *);
ChkImage		*chkResize(ChkImage *img, int width, int height, int filter);
ChkImage		*chkResizeExt(ChkImage *img, int width, int height, int destfmt, int flags);
void			chkResizeInplace(ChkImage *img, int width, int height, int filter);
void			chk_DimRadial(struct	chkimage	*,float);
void			chk_Clear(struct	chkimage	*i,int k);
void			chk_Copy(struct	chkimage	*simage,struct chkimage *dimage);
ChkImage*		chk_CreateImageFromMem(void	*memory,int width,int height);
void			chk_DimRadialXY(struct	chkimage *image,int x,int y,int r,float factor);

int				chkGetBytesPerPixel( ChkImage	*img );
int				fmtGetBytesPerPixel( int format );


void	chk_CopyArea(struct	chkimage	*i1,int sx,int sy,struct	chkimage	*i2);

void	txt_Negative(struct	texture	*txt);
void	txt_Spherical(struct texture *txt);

void chk_ReadARGBPixel(struct chkimage *img, int x, int y, int *a, int *r, int *g, int *b);
void chk_WriteARGBPixel(struct chkimage *img, int x, int y, int a, int r, int g, int b);
#define	chkReadARGBPixel chk_ReadARGBPixel
#define	chkWriteARGBPixel chk_WriteARGBPixel

void chkFillImage(ChkImage *img, int r, int g, int b, int a);
int chkHasAlphaInformation(ChkImage *img);

void chkConvert(unsigned char *src, int sfmt, unsigned char *dst, int dfmt, int width, int height);


// chkuny image pixel formats
// CHK_PIXFMT_RGB565	-	used for normal textures
// CHK_PIXFMT_ARGB4444	-	used for textures with alpha channel
// CHK_PIXFMT_ARGB8888	-	used for high quality textures


#define	CHK_PIXFMT_RGB888		0
#define	CHK_PIXFMT_RGBA8888		1
#define	CHK_PIXFMT_RGB565		2
#define	CHK_PIXFMT_LUT			3
#define	CHK_PIXFMT_ARGB4444		4
#define	CHK_PIXFMT_RGBA5551		5
#define	CHK_PIXFMT_ARGB1555		6
#define	CHK_PIXFMT_ARGB8888		7
#define	CHK_PIXFMT_LUMINANCE	8
#define	CHK_PIXFMT_ALPHA		9
#define	CHK_PIXFMT_YUV422		10
#define	CHK_PIXFMT_YUV422C		11

#define CHK_CHANNEL_LUMINANCE 1
#define CHK_CHANNEL_CHROMINANCE 2
#define CHK_CHANNEL_SATURATION 4
#define CHK_CHANNEL_Y 8
#define CHK_CHANNEL_U 16
#define CHK_CHANNEL_V 32
#define CHK_CHANNEL_R 64
#define CHK_CHANNEL_G 128
#define CHK_CHANNEL_B 256

#endif
