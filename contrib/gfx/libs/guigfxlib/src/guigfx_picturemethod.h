#ifndef _GUIGFX_DOPICTUREMETHOD_H
#define _GUIGFX_DOPICTUREMETHOD_H	1

#include "guigfx_internal.h"

ULONG PIC_Crop(struct Picture *pic, ULONG x, ULONG y, ULONG width, ULONG height, TAGLIST tags);
ULONG PIC_Render(struct Picture *pic, ULONG pixelformat, TAGLIST tags);
ULONG PIC_Scale(struct Picture *pic, ULONG width, ULONG height, TAGLIST tags);
ULONG PIC_Mix(struct Picture *pic1, struct Picture *pic2, TAGLIST tags);
ULONG PIC_SetAlpha(struct Picture *pic, UBYTE *array, ULONG width, ULONG height, TAGLIST tags);
ULONG PIC_MixAlpha(struct Picture *pic1, struct Picture *pic2, TAGLIST tags);
ULONG PIC_RenderPaletteA(struct Picture *pic, APTR palette, TAGLIST tags);

#ifdef __MORPHOS__
#define PIC_RenderPalette(pic, palette, tags...) \
	({unsigned long _tags[] = { tags }; PIC_RenderPaletteA(pic, palette, (TAGLIST)_tags);})
#else
ULONG PIC_RenderPalette(PIC *pic, APTR palette, Tag tag1, ...);
#endif

ULONG PIC_MapDrawHandle(struct Picture *pic, struct DrawHandle *dh, TAGLIST tags);
ULONG PIC_CreateAlphaMask(PIC *pic, ULONG rgb, TAGLIST tags);
ULONG PIC_Tint(PIC *pic, ULONG rgb, TAGLIST tags);
ULONG PIC_Texture(PIC *pic, PIC *texpic, WORD *coordinates, TAGLIST tags);
ULONG PIC_Set(PIC *pic, ULONG rgb, TAGLIST tags);
ULONG PIC_TintAlpha(PIC *pic, ULONG rgb, TAGLIST tags);
ULONG PIC_Insert(PIC *pic, PIC *sourcepic, TAGLIST tags);
ULONG PIC_FlipX(PIC *pic, TAGLIST tags);
ULONG PIC_FlipY(PIC *pic, TAGLIST tags);
ULONG PIC_CheckDither(PIC *pic, DRAWHANDLE *dh, TAGLIST tags);
ULONG PIC_Negative(PIC *pic, TAGLIST tags);
ULONG PIC_AutoCrop(PIC *pic, TAGLIST tags);

BOOL InsertAlphaArray(PIC *pic);
BOOL ExtractAlphaArray(PIC *pic);

#endif
