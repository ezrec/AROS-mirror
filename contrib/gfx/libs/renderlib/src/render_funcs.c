#include <render/render.h>
#include <aros/libcall.h>

#include "lib_init.h"
#include "render_funcs.h"

static int dummy;

AROS_LH1(void, DeleteRMHandler,
    AROS_LHA(APTR, rmh, A0),
    struct Library *, RenderBase, 8, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    DeleteRMHandler(rmh);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(APTR, CreateRMHandlerA,
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, RenderBase, 7, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return CreateRMHandlerA(tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(APTR, AllocRenderMem,
    AROS_LHA(APTR, rmh, A0),
    AROS_LHA(ULONG, size, D0),
    struct Library *, RenderBase, 9, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return AllocRenderMem(rmh, size);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, FreeRenderMem,
    AROS_LHA(APTR, rmh, A0),
    AROS_LHA(APTR, mem, A1),
    AROS_LHA(ULONG, size, D0),
    struct Library *, RenderBase, 10, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    FreeRenderMem(rmh, mem, size);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(APTR, AllocRenderVec,
    AROS_LHA(APTR, rmh, A0),
    AROS_LHA(ULONG, size, D0),
    struct Library *, RenderBase, 11, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return AllocRenderVec(rmh, size);
    
    AROS_LIBFUNC_EXIT
}


AROS_LH1(void, FreeRenderVec,
    AROS_LHA(APTR, mem, A0),
    struct Library *, RenderBase, 12, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    FreeRenderVec(mem);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(APTR, AllocRenderVecClear,
    AROS_LHA(APTR, rmh, A0),
    AROS_LHA(ULONG, size, D0),
    struct Library *, RenderBase, 51, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return AllocRenderVecClear(rmh, size);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, TurboFillMem,
    AROS_LHA(APTR, mem, A0),
    AROS_LHA(ULONG, len, D0),
    AROS_LHA(ULONG, val, D1),
    struct Library *, RenderBase, 5, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    TurboFillMem(mem, len, val);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, TurboCopyMem,
    AROS_LHA(APTR, src, A0),
    AROS_LHA(APTR, dst, A1),
    AROS_LHA(ULONG, len, D0),
    struct Library *, RenderBase, 6, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    TurboCopyMem(src, dst, len);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(APTR, CreateHistogramA,
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, RenderBase, 13, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return CreateHistogramA(tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, DeleteHistogram,
    AROS_LHA(APTR, histo, A0),
    struct Library *, RenderBase, 14, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    DeleteHistogram(histo);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(ULONG, AddRGB,
    AROS_LHA(APTR, histo, A0),
    AROS_LHA(ULONG, rgb, D0),
    AROS_LHA(ULONG, count, D1),
    struct Library *, RenderBase, 16, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return AddRGB(histo, rgb, count);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(ULONG, QueryHistogram,
    AROS_LHA(APTR, histo, A0),
    AROS_LHA(Tag, tag, D0),
    struct Library *, RenderBase, 15, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return QueryHistogram(histo, tag);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH5(ULONG, AddRGBImageA,
    AROS_LHA(APTR, histo, A0),
    AROS_LHA(ULONG *, rgb, A1),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(ULONG, height, D1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 17, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return AddRGBImageA(histo, rgb, width, height, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(APTR, CreatePaletteA,
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, RenderBase, 29, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return CreatePaletteA(tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, DeletePalette,
    AROS_LHA(APTR, palette, A0),
    struct Library *, RenderBase, 30, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    DeletePalette((RNDPAL *)palette);
    
    AROS_LIBFUNC_EXIT
}


AROS_LH4(void, ImportPaletteA,
    AROS_LHA(APTR, palette, A0),
    AROS_LHA(APTR, coltab, A1),
    AROS_LHA(UWORD, numcol, D0),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 31, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    ImportPaletteA((RNDPAL *)palette, coltab, numcol, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, ExportPaletteA,
    AROS_LHA(APTR, palette, A0),
    AROS_LHA(APTR, coltab, A1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 32, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    ExportPaletteA((RNDPAL *)palette, coltab, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, FlushPalette,
    AROS_LHA(APTR, palette, A0),
    struct Library *, RenderBase, 35, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    FlushPalette((RNDPAL *)palette);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH6(ULONG, AddChunkyImageA,
    AROS_LHA(APTR, histogram, A0),
    AROS_LHA(UBYTE *, chunky, A1),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(ULONG, height, D1),
    AROS_LHA(APTR, palette, A2),
    AROS_LHA(struct TagItem *, tags, A3),
    struct Library *, RenderBase, 18, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return AddChunkyImageA((RNDHISTO *)histogram, chunky, width, height, (RNDPAL *)palette, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH4(ULONG, ExtractPaletteA,
    AROS_LHA(APTR, histogram, A0),
    AROS_LHA(ULONG *, palette, A1),
    AROS_LHA(UWORD, numcol, D0),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 19, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return ExtractPaletteA((RNDHISTO *)histogram, (RNDPAL *)palette, numcol, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(ULONG *, CreateHistogramPointerArray,
    AROS_LHA(APTR, histogram, A0),
    struct Library *, RenderBase, 39, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return (ULONG *)CreateHistogramPointerArray((RNDHISTO *)histogram);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(ULONG, CountRGB,
    AROS_LHA(APTR, histogram, A0),
    AROS_LHA(ULONG, rgb, D0),
    struct Library *, RenderBase, 33, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return CountRGB((RNDHISTO *)histogram, rgb);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(LONG, BestPen,
    AROS_LHA(APTR, palette, A0),
    AROS_LHA(ULONG, rgb, D0),
    struct Library *, RenderBase, 34, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return BestPen((RNDPAL *)palette, rgb);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(APTR, CreateMapEngineA,
    AROS_LHA(APTR, palette, A0),
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, RenderBase, 41, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return CreateMapEngineA((RNDPAL *)palette, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, DeleteMapEngine,
    AROS_LHA(APTR, mapengine, A0),
    struct Library *, RenderBase, 42, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    DeleteMapEngine((RNDMAP *)mapengine);
    
    AROS_LIBFUNC_EXIT
}


AROS_LH6(ULONG, MapRGBArrayA,
    AROS_LHA(APTR, engine, A0),
    AROS_LHA(ULONG *, rgb, A1),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(ULONG, height, D1),
    AROS_LHA(UBYTE *, chunky, A2),
    AROS_LHA(struct TagItem *, tags, A3),
    struct Library *, RenderBase, 43, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return MapRGBArrayA((RNDMAP *)engine, rgb, width, height, chunky, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH7(ULONG, MapChunkyArrayA,
    AROS_LHA(APTR, engine, A0),
    AROS_LHA(UBYTE *, src, A1),
    AROS_LHA(APTR, palette, A2),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(UBYTE *, dest, A3),
    AROS_LHA(struct TagItem *, tags, A4),
    struct Library *, RenderBase, 46, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return MapChunkyArrayA((RNDMAP *)engine, src, (RNDPAL *)palette, width, height, dest, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(ULONG, CountHistogram,
    AROS_LHA(APTR, histo, A0),
    struct Library *, RenderBase, 40, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return CountHistogram((RNDHISTO *)histo);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(ULONG, AddHistogramA,
    AROS_LHA(APTR, dst, A0),
    AROS_LHA(APTR, src, A1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 37, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return AddHistogramA((RNDHISTO *)dst, (RNDHISTO *)src, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH6(ULONG, Chunky2RGBA,
    AROS_LHA(UBYTE *, src, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(ULONG, height, D1),
    AROS_LHA(ULONG *, dst, A1),
    AROS_LHA(APTR, pal, A2),
    AROS_LHA(struct TagItem *, tags, A3),
    struct Library *, RenderBase, 22, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return Chunky2RGBA(src, width, height, dst, (RNDPAL *)pal, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH4(LONG, RGBArrayDiversityA,
    AROS_LHA(ULONG *, rgb, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, RenderBase, 44, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return RGBArrayDiversityA(rgb, width, height, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH5(LONG, ChunkyArrayDiversityA,
    AROS_LHA(UBYTE *, chunky, A0),
    AROS_LHA(APTR, palette, A1),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 45, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return  ChunkyArrayDiversityA(chunky, (RNDPAL *)palette, width, height, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, InsertAlphaChannelA,
    AROS_LHA(UBYTE *, chunky, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(ULONG *, rgb, A1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 47, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    InsertAlphaChannelA(chunky, width, height, rgb, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, ExtractAlphaChannelA,
    AROS_LHA(ULONG *, rgb, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(UBYTE *, chunky, A1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 48, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    ExtractAlphaChannelA(rgb, width, height, chunky, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, ApplyAlphaChannelA,
    AROS_LHA(ULONG *, src, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(ULONG *, dst, A1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 49, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    ApplyAlphaChannelA(src, width, height, dst, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, MixRGBArrayA,
    AROS_LHA(ULONG *, src, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(ULONG *, dst, A1),
    AROS_LHA(UWORD, ratio, D2),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 50, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    MixRGBArrayA(src, width, height, dst, ratio, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, CreateAlphaArrayA,
    AROS_LHA(ULONG *,rgb, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, RenderBase, 52, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    CreateAlphaArrayA(rgb, width, height, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, MixAlphaChannelA,
    AROS_LHA(ULONG *, rgb1, A0),
    AROS_LHA(ULONG *, rgb2, A1),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(ULONG *, dest, A2),
    AROS_LHA(struct TagItem *, tags, A3),
    struct Library *, RenderBase, 53, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    MixAlphaChannelA(rgb1, rgb2, width, height, dest, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, TintRGBArrayA,
    AROS_LHA(ULONG *, src, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(ULONG, rgb, D2),
    AROS_LHA(UWORD, ratio, D3),
    AROS_LHA(ULONG *, dest, A1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 54, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    TintRGBArrayA(src, width, height, rgb, ratio, dest, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(ULONG, SortPaletteA,
    AROS_LHA(APTR, palette, A0),
    AROS_LHA(ULONG, mode, D0),
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, RenderBase, 36, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return SortPaletteA((RNDPAL *)palette, mode, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH6(ULONG, RenderA,
    AROS_LHA(ULONG *, src, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(UBYTE *, dst, A1),
    AROS_LHA(APTR, palette, A2),
    AROS_LHA(struct TagItem *, tags, A3),
    struct Library *, RenderBase, 20, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return RenderA(src, width, height, dst, (RNDPAL *)palette, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, Planar2ChunkyA,
    AROS_LHA(PLANEPTR *, planetab, A0),
    AROS_LHA(UWORD, bytewidth, D0),
    AROS_LHA(UWORD, rows, D1),
    AROS_LHA(UWORD, depth, D2),
    AROS_LHA(UWORD, bytesperrow, D3),
    AROS_LHA(UBYTE *, dst, A1),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 21, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    Planar2ChunkyA((UWORD **)planetab, bytewidth, rows, depth, bytesperrow, dst, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, Chunky2BitMapA,
    AROS_LHA(UBYTE *, src, A0),
    AROS_LHA(UWORD, sx, D0),
    AROS_LHA(UWORD, sy, D1),
    AROS_LHA(UWORD, width, D2),
    AROS_LHA(UWORD, height, D3),
    AROS_LHA(struct BitMap *, bm, A1),
    AROS_LHA(UWORD, dx, D4),
    AROS_LHA(UWORD, dy, D5),
    AROS_LHA(struct TagItem *, tags, A2),
    struct Library *, RenderBase, 23, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    Chunky2BitMapA(src, sx, sy, width, height, bm, dx, dy, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH2(ULONG, GetPaletteAttrs,
    AROS_LHA(APTR, pal, A0),
    AROS_LHA(ULONG, args, D0),
    struct Library *, RenderBase, 55, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return GetPaletteAttrs((RNDPAL *)pal, args);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, RemapArrayA,
    AROS_LHA(UBYTE *, src, A0),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(UBYTE *, dst, A1),
    AROS_LHA(UBYTE *, pentab, A2),
    AROS_LHA(struct TagItem *, tags, A3),
    struct Library *, RenderBase, 56, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    RemapArrayA(src, width, height, dst, pentab, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH7(ULONG, ConvertChunkyA,
    AROS_LHA(UBYTE *, src, A0),
    AROS_LHA(APTR, srcpal, A1),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(UBYTE *, dst, A2),
    AROS_LHA(APTR, dstpal, A3),
    AROS_LHA(struct TagItem *, tags, A4),
    struct Library *, RenderBase, 27, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return ConvertChunkyA(src, (RNDPAL *)srcpal, width, height, dst, (RNDPAL *)dstpal, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, CreatePenTableA,
    AROS_LHA(UBYTE *, src, A0),
    AROS_LHA(APTR, srcpal, A1),
    AROS_LHA(UWORD, width, D0),
    AROS_LHA(UWORD, height, D1),
    AROS_LHA(APTR, newpal, A2),
    AROS_LHA(UBYTE *, pentab, A3),
    AROS_LHA(struct TagItem *, tags, A4),
    struct Library *, RenderBase, 28, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    CreatePenTableA(src, (RNDPAL *)srcpal, width, height, (RNDPAL *)newpal, pentab, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH5(APTR, CreateScaleEngineA,
    AROS_LHA(UWORD, sw, D0),
    AROS_LHA(UWORD, sh, D1),
    AROS_LHA(UWORD, dw, D2),
    AROS_LHA(UWORD, dh, A3),
    AROS_LHA(struct TagItem *, tags, A1),
    struct Library *, RenderBase, 24, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return CreateScaleEngineA(sw, sh, dw, dh, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, DeleteScaleEngine,
    AROS_LHA(APTR, sce, A0),
    struct Library *, RenderBase, 25, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    DeleteScaleEngine((RNDSCALE *)sce);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH4(ULONG, ScaleA,
    AROS_LHA(APTR, sce, A0),
    AROS_LHA(APTR, src, A1),
    AROS_LHA(APTR, dst, A2),
    AROS_LHA(struct TagItem *, tags, A3),
    struct Library *, RenderBase, 26, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return ScaleA((RNDSCALE *)sce, src, dst, tags);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH3(UWORD, ScaleOrdinate,
    AROS_LHA(UWORD, start, D0),
    AROS_LHA(UWORD, dest, D1),
    AROS_LHA(UWORD, ordinate, D2),
    struct Library *, RenderBase, 38, Render)
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,RenderBase)
    
    return ScaleOrdinate(start, dest, ordinate);
    
    AROS_LIBFUNC_EXIT
}
