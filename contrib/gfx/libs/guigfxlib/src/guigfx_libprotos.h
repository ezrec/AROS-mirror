
#ifndef _GUIGFX_LIBPROTOS_H
#define _GUIGFX_LIBPROTOS_H 1

/* guigfx_lib.c */

struct Library * LIBENT LibInit(REG(d0) struct Library *LibBase, REG(a0) BPTR Segment, REG(a6) struct ExecBase *ExecBase);

struct Library * LIBENT LibOpen(
#ifdef __MORPHOS__
	void);
#else
	REG(a6) struct Library *LibBase);
#endif

#ifdef __MORPHOS__
BPTR LIBExpunge(void);
#endif
BPTR LIBENT LibExpunge(REG(a6) struct Library *LibBase);

BPTR LIBENT LibClose(
#ifdef __MORPHOS__
	void);
#else
	REG(a6) struct Library *LibBase);
#endif

LONG LibNull(void);


/* guigfx_InitExit.c */

#ifndef __AROS__
BOOL LIBENT GGFX_Init(void);
void LIBENT GGFX_Exit(void);
#endif


/* guigfx_picturemethod */

ULONG SAVE_DS ASM DoPictureMethodA(REG(a0) APTR, REG(d0) ULONG, REG(a1) ULONG *);


/* guigfx_picture */

PIC SAVE_DS ASM *ReadPictureA(REG(a0) struct RastPort *, REG(a1) struct ColorMap *, REG(d0) UWORD, REG(d1) UWORD, REG(d2) UWORD, REG(d3) UWORD, REG(a2) TAGLIST);
PIC SAVE_DS ASM *MakePictureA(REG(a0) APTR, REG(d0) UWORD, REG(d1) UWORD, REG(a1) TAGLIST);
void SAVE_DS ASM DeletePicture(REG(a0) PIC *);
BOOL SAVE_DS ASM UpdatePicture(REG(a0) PIC *);
ULONG SAVE_DS ASM GetPictureAttrsA(REG(a0) PIC *, REG(a1) TAGLIST);
PIC SAVE_DS ASM *ClonePictureA(REG(a0) PIC *, REG(a1) TAGLIST);
ULONG SAVE_DS ASM LockPictureA(REG(a0) PIC *, REG(d0) ULONG, REG(a1) ULONG *);
void SAVE_DS ASM UnLockPicture(REG(a0) PIC *, REG(d0) ULONG);


/* guigfx_picture */

BOOL ReadBitMapArrayA(struct BitMap *, PIC *, UWORD, TAGLIST);

#ifdef __MORPHOS__
#define ReadBitMapArray(bm, pic, displayID, tags...) \
	({unsigned long _tags[] = { tags }; ReadBitMapArrayA(bm, pic, displayID, (TAGLIST)_tags);})
#else
BOOL ReadBitMapArray(struct BitMap *, PIC *, UWORD, Tag, ...);
#endif


/* guigfx_loading */

PIC SAVE_DS ASM *LoadPictureA(REG(a0) STRPTR, REG(a1) TAGLIST);
BOOL SAVE_DS ASM IsPictureA(REG(a0) char *file_name, REG(a1) TAGLIST tags);


BOOL PrepareDrawing(PIC *pic);
//void RemapPicture(PIC *pic, UBYTE *pentab);


/* guigfx_rasthandle */

void DeleteRastHandle(RASTHANDLE *);
RASTHANDLE *CreateRastHandle(struct RastPort *rp, ULONG modeID);


/* guigfx_colorhandle */

void SAVE_DS ASM RemColorHandle(REG(a0) COLORHANDLE *);
COLORHANDLE SAVE_DS ASM *AddPictureA(REG(a0) PSM *, REG(a1) PIC *, REG(a2) TAGLIST);
COLORHANDLE SAVE_DS ASM *AddPaletteA(REG(a0) PSM *, REG(a1) PALETTE, REG(a2) TAGLIST);
COLORHANDLE SAVE_DS ASM *AddPixelArrayA(REG(a0) PSM *, REG(a1) APTR, REG(d0) UWORD, REG(d1) UWORD, REG(a2) TAGLIST);

struct SharedHistogram *CreateSharedHistogram(APTR histo);
void LinkToSharedHistogram(struct SharedHistogram *histo);
void UnLinkSharedHistogram(struct SharedHistogram *histo);


/* guigfx_pensharemap */

PSM SAVE_DS ASM *CreatePenShareMapA(REG(a0) TAGLIST);
void SAVE_DS ASM DeletePenShareMap(REG(a0) PSM *);


/* guigfx_drawhandle */

DRAWHANDLE SAVE_DS ASM *ObtainDrawHandleA(REG(a0) PSM *, REG(a1) struct RastPort *, REG(a2) struct ColorMap *, REG(a3) TAGLIST);
void SAVE_DS ASM ReleaseDrawHandle(REG(a0) DRAWHANDLE *);


/* guigfx_draw */

ULONG GetDitherMode(DRAWHANDLE *dh, APTR array, PALETTE palette, UWORD width, UWORD height, ULONG pixelformat, UWORD totalwidth, UWORD defaultmode, BOOL autodither);

BOOL SAVE_DS ASM DrawPictureA(REG(a0) DRAWHANDLE *, REG(a1) PIC *, REG(d0) UWORD, REG(d1) UWORD, REG(a2) TAGLIST);
BOOL SAVE_DS ASM MapPaletteA(REG(a0) DRAWHANDLE *, REG(a1) PALETTE, REG(a2) UBYTE *, REG(a3) TAGLIST);
LONG SAVE_DS ASM MapPenA(REG(a0) DRAWHANDLE *, REG(d0) ULONG, REG(a1) TAGLIST);
struct BitMap SAVE_DS ASM *CreatePictureBitMapA(REG(a0) DRAWHANDLE *, REG(a1) PIC *, REG(a2) TAGLIST);

DIRECTDRAWHANDLE SAVE_DS ASM *CreateDirectDrawHandleA(REG(a0) DRAWHANDLE *, REG(d0) UWORD, REG(d1) UWORD, REG(d2) UWORD, REG(d3) UWORD, REG(a1) TAGLIST);
void SAVE_DS ASM DeleteDirectDrawHandle(REG(a0) DIRECTDRAWHANDLE *);
BOOL SAVE_DS ASM DirectDrawTrueColorA(REG(a0) DIRECTDRAWHANDLE *, REG(a1) ULONG *, REG(d0) UWORD, REG(d1) UWORD, REG(a2) TAGLIST);


BOOL SAVE_DS ASM CreatePictureMaskA(REG(a0) PIC *pic, REG(a1) UBYTE *array, REG(d0) UWORD arraywidth, REG(a2) TAGLIST tags);


#endif
