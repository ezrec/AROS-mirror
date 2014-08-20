/*
	gentexture.h

	texture generation functions
	v1.0 translated from amigaE.
*/


#ifndef	_GENTEXTURE_H
#define	_GENTEXTURE_H

#include "chunky.h"

#ifdef __cplusplus
extern "C" {
#endif

void		txt_MakeTiledHoriz(Texture	*image,int border);

void		txt_MakeTiledVert(Texture	*txt);
void		txt_MakeTiled(Texture	*txt);
void		txt_AdjustBrightnessCOPYHI16(Texture	*src,Texture	*dst, int bri);
void		txt_AdjustBrightnessCOPYGRAY8(Texture	 *src,Texture	 *dst, int bri);
void		txt_AdjustBrightnessCOPYTRUE24(Texture	*src,Texture	*dst, int bri);

void		txt_CropZero( Texture	*txt , int tolerance );
void		txt_CropZero8( Texture	 *txt , int tolerance);

void		txt_AddBrightness(Texture	*txt,int);
void		txt_AddBrightnessCOPY(Texture	*src,Texture	*dst,int);
void		txt_AdjustContrast(Texture	*txt,int);
Texture		*txtDuplicate( Texture	*txt );

int			bnd(int,int,int);
int			rnd(int);
void		txt_EqualizeImage(Texture	*txt);
void		txt_FillImage(Texture	*txt, int r , int g , int b);

Texture*	txt_AllocSecondBuffer(Texture	*txt);

void		txt_Scramble( Texture	*txt );

void		txt_Blur8( Texture	   *txt , int size);
void		txt_Blur16( Texture	    *txt , int size);

void		txt_Min8( Texture *txt1 , Texture	*txt2 , Texture	*txt );
void		txt_Max8( Texture *txt1 , Texture	*txt2 , Texture	*txt );

void		txtInsertAlpha( Texture	*txt , Texture	*alphaTxt );
void		imgInsertAlpha( ChkImage *img, ChkImage *alpha, int mode );
void		txtFillImageAlpha( Texture *txt, int a );
void		chkFillAlpha(ChkImage *img, int a);

Texture		*txtCutRectangle(Texture *txt, int x, int y, int width, int height);

#define INSERTALPHA_ALPHA 0
#define INSERTALPHA_LUMINANCE 1

#ifdef __cplusplus
}
#endif

#endif
