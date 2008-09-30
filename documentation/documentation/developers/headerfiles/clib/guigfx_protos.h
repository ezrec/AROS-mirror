#ifndef CLIB_GUIGFX_PROTOS_H
#define CLIB_GUIGFX_PROTOS_H

/*
    *** Automatically generated from 'guigfx.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <utility/tagitem.h>
#include <graphics/view.h>
#include <graphics/rastport.h>
AROS_LP4(APTR, MakePictureA,
         AROS_LPA(APTR, data, A0),
         AROS_LPA(UWORD, totalsourcewidth, D0),
         AROS_LPA(UWORD, totalsourceheight, D1),
         AROS_LPA(struct TagItem *, taglist, A1),
         LIBBASETYPEPTR, GuiGFXBase, 5, GuiGFX
);
AROS_LP2(APTR, LoadPictureA,
         AROS_LPA(STRPTR, filename, A0),
         AROS_LPA(struct TagItem *, taglist, A1),
         LIBBASETYPEPTR, GuiGFXBase, 6, GuiGFX
);
AROS_LP7(APTR, ReadPictureA,
         AROS_LPA(struct RastPort *, rp, A0),
         AROS_LPA(struct ColorMap *, cm, A1),
         AROS_LPA(UWORD, x, D0),
         AROS_LPA(UWORD, y, D1),
         AROS_LPA(UWORD, width, D2),
         AROS_LPA(UWORD, height, D3),
         AROS_LPA(struct TagItem *, taglist, A2),
         LIBBASETYPEPTR, GuiGFXBase, 7, GuiGFX
);
AROS_LP2(APTR, ClonePictureA,
         AROS_LPA(APTR, pic, A0),
         AROS_LPA(struct TagItem *, taglist, A1),
         LIBBASETYPEPTR, GuiGFXBase, 8, GuiGFX
);
AROS_LP1(void, DeletePicture,
         AROS_LPA(APTR, pic, A0),
         LIBBASETYPEPTR, GuiGFXBase, 9, GuiGFX
);
AROS_LP1(BOOL, UpdatePicture,
         AROS_LPA(APTR, pic, A0),
         LIBBASETYPEPTR, GuiGFXBase, 10, GuiGFX
);
AROS_LP3(APTR, AddPictureA,
         AROS_LPA(APTR, psm, A0),
         AROS_LPA(APTR, pic, A1),
         AROS_LPA(struct TagItem *, taglist, A2),
         LIBBASETYPEPTR, GuiGFXBase, 11, GuiGFX
);
AROS_LP3(APTR, AddPaletteA,
         AROS_LPA(APTR, psm, A0),
         AROS_LPA(APTR, palette, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, GuiGFXBase, 12, GuiGFX
);
AROS_LP5(APTR, AddPixelArrayA,
         AROS_LPA(APTR, psm, A0),
         AROS_LPA(APTR, array, A1),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, GuiGFXBase, 13, GuiGFX
);
AROS_LP1(void, RemColorHandle,
         AROS_LPA(APTR, ch, A0),
         LIBBASETYPEPTR, GuiGFXBase, 14, GuiGFX
);
AROS_LP1(APTR, CreatePenShareMapA,
         AROS_LPA(struct TagItem *, taglist, A0),
         LIBBASETYPEPTR, GuiGFXBase, 15, GuiGFX
);
AROS_LP1(void, DeletePenShareMap,
         AROS_LPA(APTR, psm, A0),
         LIBBASETYPEPTR, GuiGFXBase, 16, GuiGFX
);
AROS_LP4(APTR, ObtainDrawHandleA,
         AROS_LPA(APTR, psm, A0),
         AROS_LPA(struct RastPort *, rp, A1),
         AROS_LPA(struct ColorMap *, cm, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, GuiGFXBase, 17, GuiGFX
);
AROS_LP1(void, ReleaseDrawHandle,
         AROS_LPA(APTR, dh, A0),
         LIBBASETYPEPTR, GuiGFXBase, 18, GuiGFX
);
AROS_LP5(BOOL, DrawPictureA,
         AROS_LPA(APTR, dh, A0),
         AROS_LPA(APTR, pic, A1),
         AROS_LPA(UWORD, x, D0),
         AROS_LPA(UWORD, y, D1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, GuiGFXBase, 19, GuiGFX
);
AROS_LP4(BOOL, MapPaletteA,
         AROS_LPA(APTR, dh, A0),
         AROS_LPA(APTR, palette, A1),
         AROS_LPA(UBYTE *, pentab, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, GuiGFXBase, 20, GuiGFX
);
AROS_LP3(LONG, MapPenA,
         AROS_LPA(APTR, dh, A0),
         AROS_LPA(ULONG, rgb, D0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, GuiGFXBase, 21, GuiGFX
);
AROS_LP3(struct BitMap *, CreatePictureBitMapA,
         AROS_LPA(APTR, dh, A0),
         AROS_LPA(APTR, pic, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, GuiGFXBase, 22, GuiGFX
);
AROS_LP3(ULONG, DoPictureMethodA,
         AROS_LPA(APTR, pic, A0),
         AROS_LPA(ULONG, method, D0),
         AROS_LPA(ULONG *, arg, A1),
         LIBBASETYPEPTR, GuiGFXBase, 23, GuiGFX
);
AROS_LP2(ULONG, GetPictureAttrsA,
         AROS_LPA(APTR, pic, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, GuiGFXBase, 24, GuiGFX
);
AROS_LP3(ULONG, LockPictureA,
         AROS_LPA(APTR, pic, A0),
         AROS_LPA(ULONG, lockmode, D0),
         AROS_LPA(ULONG *, arg, A1),
         LIBBASETYPEPTR, GuiGFXBase, 25, GuiGFX
);
AROS_LP2(void, UnLockPicture,
         AROS_LPA(APTR, pic, A0),
         AROS_LPA(ULONG, lockmode, D0),
         LIBBASETYPEPTR, GuiGFXBase, 26, GuiGFX
);
AROS_LP2(BOOL, IsPictureA,
         AROS_LPA(char *, filename, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, GuiGFXBase, 27, GuiGFX
);
AROS_LP6(APTR, CreateDirectDrawHandleA,
         AROS_LPA(APTR, dh, A0),
         AROS_LPA(UWORD, sourcewidth, D0),
         AROS_LPA(UWORD, sourceheight, D1),
         AROS_LPA(UWORD, destwidth, D2),
         AROS_LPA(UWORD, destheight, D3),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, GuiGFXBase, 28, GuiGFX
);
AROS_LP1(void, DeleteDirectDrawHandle,
         AROS_LPA(APTR, ddh, A0),
         LIBBASETYPEPTR, GuiGFXBase, 29, GuiGFX
);
AROS_LP5(BOOL, DirectDrawTrueColorA,
         AROS_LPA(APTR, ddh, A0),
         AROS_LPA(ULONG *, array, A1),
         AROS_LPA(UWORD, x, D0),
         AROS_LPA(ULONG, y, D1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, GuiGFXBase, 30, GuiGFX
);
AROS_LP4(BOOL, CreatePictureMaskA,
         AROS_LPA(APTR, pic, A0),
         AROS_LPA(UBYTE *, array, A1),
         AROS_LPA(UWORD, arraywidth, D0),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, GuiGFXBase, 31, GuiGFX
);

#endif /* CLIB_GUIGFX_PROTOS_H */
