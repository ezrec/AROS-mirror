#ifndef CLIB_RENDER_PROTOS_H
#define CLIB_RENDER_PROTOS_H

/*
    *** Automatically generated from 'render.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <render/render.h>
#include <graphics/gfx.h>
AROS_LP3(void, TurboFillMem,
         AROS_LPA(APTR, mem, A0),
         AROS_LPA(ULONG, len, D0),
         AROS_LPA(ULONG, val, D1),
         LIBBASETYPEPTR, RenderBase, 5, Render
);
AROS_LP3(void, TurboCopyMem,
         AROS_LPA(APTR, src, A0),
         AROS_LPA(APTR, dst, A1),
         AROS_LPA(ULONG, len, D0),
         LIBBASETYPEPTR, RenderBase, 6, Render
);
AROS_LP1(APTR, CreateRMHandlerA,
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, RenderBase, 7, Render
);
AROS_LP1(void, DeleteRMHandler,
         AROS_LPA(APTR, rmh, A0),
         LIBBASETYPEPTR, RenderBase, 8, Render
);
AROS_LP2(APTR, AllocRenderMem,
         AROS_LPA(APTR, rmh, A0),
         AROS_LPA(ULONG, size, D0),
         LIBBASETYPEPTR, RenderBase, 9, Render
);
AROS_LP3(void, FreeRenderMem,
         AROS_LPA(APTR, rmh, A0),
         AROS_LPA(APTR, mem, A1),
         AROS_LPA(ULONG, size, D0),
         LIBBASETYPEPTR, RenderBase, 10, Render
);
AROS_LP2(APTR, AllocRenderVec,
         AROS_LPA(APTR, rmh, A0),
         AROS_LPA(ULONG, size, D0),
         LIBBASETYPEPTR, RenderBase, 11, Render
);
AROS_LP1(void, FreeRenderVec,
         AROS_LPA(APTR, mem, A0),
         LIBBASETYPEPTR, RenderBase, 12, Render
);
AROS_LP1(APTR, CreateHistogramA,
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, RenderBase, 13, Render
);
AROS_LP1(void, DeleteHistogram,
         AROS_LPA(APTR, histo, A0),
         LIBBASETYPEPTR, RenderBase, 14, Render
);
AROS_LP2(ULONG, QueryHistogram,
         AROS_LPA(APTR, histo, A0),
         AROS_LPA(Tag, tag, D0),
         LIBBASETYPEPTR, RenderBase, 15, Render
);
AROS_LP3(ULONG, AddRGB,
         AROS_LPA(APTR, histo, A0),
         AROS_LPA(ULONG, rgb, D0),
         AROS_LPA(ULONG, count, D1),
         LIBBASETYPEPTR, RenderBase, 16, Render
);
AROS_LP5(ULONG, AddRGBImageA,
         AROS_LPA(APTR, histo, A0),
         AROS_LPA(ULONG *, rgb, A1),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(ULONG, height, D1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 17, Render
);
AROS_LP6(ULONG, AddChunkyImageA,
         AROS_LPA(APTR, histogram, A0),
         AROS_LPA(UBYTE *, chunky, A1),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(ULONG, height, D1),
         AROS_LPA(APTR, palette, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, RenderBase, 18, Render
);
AROS_LP4(ULONG, ExtractPaletteA,
         AROS_LPA(APTR, histogram, A0),
         AROS_LPA(ULONG *, palette, A1),
         AROS_LPA(UWORD, numcol, D0),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 19, Render
);
AROS_LP6(ULONG, RenderA,
         AROS_LPA(ULONG *, src, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(UBYTE *, dst, A1),
         AROS_LPA(APTR, palette, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, RenderBase, 20, Render
);
AROS_LP7(void, Planar2ChunkyA,
         AROS_LPA(PLANEPTR *, planetab, A0),
         AROS_LPA(UWORD, bytewidth, D0),
         AROS_LPA(UWORD, rows, D1),
         AROS_LPA(UWORD, depth, D2),
         AROS_LPA(UWORD, bytesperrow, D3),
         AROS_LPA(UBYTE *, dst, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 21, Render
);
AROS_LP6(ULONG, Chunky2RGBA,
         AROS_LPA(UBYTE *, src, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(ULONG, height, D1),
         AROS_LPA(ULONG *, dst, A1),
         AROS_LPA(APTR, pal, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, RenderBase, 22, Render
);
AROS_LP9(void, Chunky2BitMapA,
         AROS_LPA(UBYTE *, src, A0),
         AROS_LPA(UWORD, sx, D0),
         AROS_LPA(UWORD, sy, D1),
         AROS_LPA(UWORD, width, D2),
         AROS_LPA(UWORD, height, D3),
         AROS_LPA(struct BitMap *, bm, A1),
         AROS_LPA(UWORD, dx, D4),
         AROS_LPA(UWORD, dy, D5),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 23, Render
);
AROS_LP5(APTR, CreateScaleEngineA,
         AROS_LPA(UWORD, sw, D0),
         AROS_LPA(UWORD, sh, D1),
         AROS_LPA(UWORD, dw, D2),
         AROS_LPA(UWORD, dh, A3),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, RenderBase, 24, Render
);
AROS_LP1(void, DeleteScaleEngine,
         AROS_LPA(APTR, sce, A0),
         LIBBASETYPEPTR, RenderBase, 25, Render
);
AROS_LP4(ULONG, ScaleA,
         AROS_LPA(APTR, sce, A0),
         AROS_LPA(APTR, src, A1),
         AROS_LPA(APTR, dst, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, RenderBase, 26, Render
);
AROS_LP7(ULONG, ConvertChunkyA,
         AROS_LPA(UBYTE *, src, A0),
         AROS_LPA(APTR, srcpal, A1),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(UBYTE *, dst, A2),
         AROS_LPA(APTR, dstpal, A3),
         AROS_LPA(struct TagItem *, tags, A4),
         LIBBASETYPEPTR, RenderBase, 27, Render
);
AROS_LP7(void, CreatePenTableA,
         AROS_LPA(UBYTE *, src, A0),
         AROS_LPA(APTR, srcpal, A1),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(APTR, newpal, A2),
         AROS_LPA(UBYTE *, pentab, A3),
         AROS_LPA(struct TagItem *, tags, A4),
         LIBBASETYPEPTR, RenderBase, 28, Render
);
AROS_LP1(APTR, CreatePaletteA,
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, RenderBase, 29, Render
);
AROS_LP1(void, DeletePalette,
         AROS_LPA(APTR, palette, A0),
         LIBBASETYPEPTR, RenderBase, 30, Render
);
AROS_LP4(void, ImportPaletteA,
         AROS_LPA(APTR, palette, A0),
         AROS_LPA(APTR, coltab, A1),
         AROS_LPA(UWORD, numcol, D0),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 31, Render
);
AROS_LP3(void, ExportPaletteA,
         AROS_LPA(APTR, palette, A0),
         AROS_LPA(APTR, coltab, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 32, Render
);
AROS_LP2(ULONG, CountRGB,
         AROS_LPA(APTR, histogram, A0),
         AROS_LPA(ULONG, rgb, D0),
         LIBBASETYPEPTR, RenderBase, 33, Render
);
AROS_LP2(LONG, BestPen,
         AROS_LPA(APTR, palette, A0),
         AROS_LPA(ULONG, rgb, D0),
         LIBBASETYPEPTR, RenderBase, 34, Render
);
AROS_LP1(void, FlushPalette,
         AROS_LPA(APTR, palette, A0),
         LIBBASETYPEPTR, RenderBase, 35, Render
);
AROS_LP3(ULONG, SortPaletteA,
         AROS_LPA(APTR, palette, A0),
         AROS_LPA(ULONG, mode, D0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, RenderBase, 36, Render
);
AROS_LP3(ULONG, AddHistogramA,
         AROS_LPA(APTR, dst, A0),
         AROS_LPA(APTR, src, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 37, Render
);
AROS_LP3(UWORD, ScaleOrdinate,
         AROS_LPA(UWORD, start, D0),
         AROS_LPA(UWORD, dest, D1),
         AROS_LPA(UWORD, ordinate, D2),
         LIBBASETYPEPTR, RenderBase, 38, Render
);
AROS_LP1(ULONG *, CreateHistogramPointerArray,
         AROS_LPA(APTR, histogram, A0),
         LIBBASETYPEPTR, RenderBase, 39, Render
);
AROS_LP1(ULONG, CountHistogram,
         AROS_LPA(APTR, histo, A0),
         LIBBASETYPEPTR, RenderBase, 40, Render
);
AROS_LP2(APTR, CreateMapEngineA,
         AROS_LPA(APTR, palette, A0),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, RenderBase, 41, Render
);
AROS_LP1(void, DeleteMapEngine,
         AROS_LPA(APTR, mapengine, A0),
         LIBBASETYPEPTR, RenderBase, 42, Render
);
AROS_LP6(ULONG, MapRGBArrayA,
         AROS_LPA(APTR, engine, A0),
         AROS_LPA(ULONG *, rgb, A1),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(ULONG, height, D1),
         AROS_LPA(UBYTE *, chunky, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, RenderBase, 43, Render
);
AROS_LP4(LONG, RGBArrayDiversityA,
         AROS_LPA(ULONG *, rgb, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, RenderBase, 44, Render
);
AROS_LP5(LONG, ChunkyArrayDiversityA,
         AROS_LPA(UBYTE *, chunky, A0),
         AROS_LPA(APTR, palette, A1),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 45, Render
);
AROS_LP7(ULONG, MapChunkyArrayA,
         AROS_LPA(APTR, engine, A0),
         AROS_LPA(UBYTE *, src, A1),
         AROS_LPA(APTR, palette, A2),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(UBYTE *, dest, A3),
         AROS_LPA(struct TagItem *, tags, A4),
         LIBBASETYPEPTR, RenderBase, 46, Render
);
AROS_LP5(void, InsertAlphaChannelA,
         AROS_LPA(UBYTE *, chunky, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(ULONG *, rgb, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 47, Render
);
AROS_LP5(void, ExtractAlphaChannelA,
         AROS_LPA(ULONG *, rgb, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(UBYTE *, chunky, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 48, Render
);
AROS_LP5(void, ApplyAlphaChannelA,
         AROS_LPA(ULONG *, src, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(ULONG *, dst, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 49, Render
);
AROS_LP6(void, MixRGBArrayA,
         AROS_LPA(ULONG *, src, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(ULONG *, dst, A1),
         AROS_LPA(UWORD, ratio, D2),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 50, Render
);
AROS_LP2(APTR, AllocRenderVecClear,
         AROS_LPA(APTR, rmh, A0),
         AROS_LPA(ULONG, size, D0),
         LIBBASETYPEPTR, RenderBase, 51, Render
);
AROS_LP4(void, CreateAlphaArrayA,
         AROS_LPA(ULONG *, rgb, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(struct TagItem *, tags, A1),
         LIBBASETYPEPTR, RenderBase, 52, Render
);
AROS_LP6(void, MixAlphaChannelA,
         AROS_LPA(ULONG *, rgb1, A0),
         AROS_LPA(ULONG *, rgb2, A1),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(ULONG *, dest, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, RenderBase, 53, Render
);
AROS_LP7(void, TintRGBArrayA,
         AROS_LPA(ULONG *, src, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(ULONG, rgb, D2),
         AROS_LPA(UWORD, ratio, D3),
         AROS_LPA(ULONG *, dest, A1),
         AROS_LPA(struct TagItem *, tags, A2),
         LIBBASETYPEPTR, RenderBase, 54, Render
);
AROS_LP2(ULONG, GetPaletteAttrs,
         AROS_LPA(APTR, pal, A0),
         AROS_LPA(ULONG, args, D0),
         LIBBASETYPEPTR, RenderBase, 55, Render
);
AROS_LP6(void, RemapArrayA,
         AROS_LPA(UBYTE *, src, A0),
         AROS_LPA(UWORD, width, D0),
         AROS_LPA(UWORD, height, D1),
         AROS_LPA(UBYTE *, dst, A1),
         AROS_LPA(UBYTE *, pentab, A2),
         AROS_LPA(struct TagItem *, tags, A3),
         LIBBASETYPEPTR, RenderBase, 56, Render
);

#endif /* CLIB_RENDER_PROTOS_H */
