#ifndef _GUIGFX_PICTURE_H
#define _GUIGFX_PICTURE_H	1

#include "guigfx_internal.h"

ULONG Lock_DrawHandle(PIC *pic, ULONG mode, DRAWHANDLE *dh, TAGLIST tags);

#ifdef __MORPHOS__
#define MakePicture(array, width, height, tags...) \
	({unsigned long _tags[] = { tags }; MakePictureA(array, width, height, (TAGLIST)_tags);})
#else
PIC *MakePicture(APTR array, UWORD width, UWORD height, Tag tag1, ...);
#endif

#endif
