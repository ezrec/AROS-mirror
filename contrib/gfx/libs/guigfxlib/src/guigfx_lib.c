
#include "guigfx_global.h"

#ifdef __MORPHOS__
PIC *MakePictureA_GATE(void);
PIC *LoadPictureA_GATE(void);
PIC *ReadPictureA_GATE(void);
PIC *ClonePictureA_GATE(void);
void DeletePicture_GATE(void);
BOOL UpdatePicture_GATE(void);
COLORHANDLE *AddPictureA_GATE(void);
COLORHANDLE *AddPaletteA_GATE(void);
COLORHANDLE *AddPixelArrayA_GATE(void);
void RemColorHandle_GATE(void);
PSM *CreatePenShareMapA_GATE(void);
void DeletePenShareMap_GATE(void);
DRAWHANDLE *ObtainDrawHandleA_GATE(void);
void ReleaseDrawHandle_GATE(void);
BOOL DrawPictureA_GATE(void);
BOOL MapPaletteA_GATE(void);
LONG MapPenA_GATE(void);
struct BitMap *CreatePictureBitMapA_GATE(void);
ULONG DoPictureMethodA_GATE(void);
ULONG GetPictureAttrsA_GATE(void);
ULONG LockPictureA_GATE(void);
void UnLockPicture_GATE(void);
BOOL IsPictureA_GATE(void);
DIRECTDRAWHANDLE *CreateDirectDrawHandleA_GATE(void);
void DeleteDirectDrawHandle_GATE(void);
BOOL DirectDrawTrueColorA_GATE(void);
BOOL CreatePictureMaskA_GATE(void);
#endif

/*****************************************************************************/

/* Reserved library-function and dummy entrypoint */
LONG LibNull()
{
	return(NULL);
}

/*****************************************************************************/

//#define SysBase GuiGFXBase->ExecBase
struct ExecBase *SysBase = NULL;
struct GuiGFXBase *GuiGFXBase = NULL;

/*****************************************************************************/

ULONG LibVectors[] =
{
#ifdef __MORPHOS__
	FUNCARRAY_32BIT_NATIVE,
#endif
	(ULONG) &LibOpen,
	(ULONG) &LibClose,
#ifdef __MORPHOS__
	(ULONG) &LIBExpunge,
#else
	(ULONG) &LibExpunge,
#endif
	(ULONG) &LibNull,

#ifdef __MORPHOS__
	(ULONG) &MakePictureA_GATE,
	(ULONG) &LoadPictureA_GATE,
	(ULONG) &ReadPictureA_GATE,
	(ULONG) &ClonePictureA_GATE,
	(ULONG) &DeletePicture_GATE,
	(ULONG) &UpdatePicture_GATE,
	(ULONG) &AddPictureA_GATE,
	(ULONG) &AddPaletteA_GATE,
	(ULONG) &AddPixelArrayA_GATE,
	(ULONG) &RemColorHandle_GATE,
	(ULONG) &CreatePenShareMapA_GATE,
	(ULONG) &DeletePenShareMap_GATE,
	(ULONG) &ObtainDrawHandleA_GATE,
	(ULONG) &ReleaseDrawHandle_GATE,
	(ULONG) &DrawPictureA_GATE,
	(ULONG) &MapPaletteA_GATE,
	(ULONG) &MapPenA_GATE,
	(ULONG) &CreatePictureBitMapA_GATE,
	(ULONG) &DoPictureMethodA_GATE,
	(ULONG) &GetPictureAttrsA_GATE,
	(ULONG) &LockPictureA_GATE,
	(ULONG) &UnLockPicture_GATE,
	(ULONG) &IsPictureA_GATE,
	(ULONG) &CreateDirectDrawHandleA_GATE,
	(ULONG) &DeleteDirectDrawHandle_GATE,
	(ULONG) &DirectDrawTrueColorA_GATE,
	(ULONG) &CreatePictureMaskA_GATE,
#else
	(ULONG) &MakePictureA,
	(ULONG) &LoadPictureA,
	(ULONG) &ReadPictureA,
	(ULONG) &ClonePictureA,
	(ULONG) &DeletePicture,
	(ULONG) &UpdatePicture,
	(ULONG) &AddPictureA,
	(ULONG) &AddPaletteA,
	(ULONG) &AddPixelArrayA,
	(ULONG) &RemColorHandle,
	(ULONG) &CreatePenShareMapA,
	(ULONG) &DeletePenShareMap,
	(ULONG) &ObtainDrawHandleA,
	(ULONG) &ReleaseDrawHandle,
	(ULONG) &DrawPictureA,
	(ULONG) &MapPaletteA,
	(ULONG) &MapPenA,
	(ULONG) &CreatePictureBitMapA,
	(ULONG) &DoPictureMethodA,
	(ULONG) &GetPictureAttrsA,
	(ULONG) &LockPictureA,
	(ULONG) &UnLockPicture,
	(ULONG) &IsPictureA,
	(ULONG) &CreateDirectDrawHandleA,
	(ULONG) &DeleteDirectDrawHandle,
	(ULONG) &DirectDrawTrueColorA,
	(ULONG) &CreatePictureMaskA,
#endif

	0xFFFFFFFF
};

struct LibInitStruct
{
	ULONG	LibSize;
	void	*FuncTable;
	void	*DataTable;
	void	(*InitFunc)(void);
};

struct LibInitStruct LibInitStruct = {
	sizeof(struct GuiGFXBase),
	LibVectors,
	NULL,
	(void (*)(void)) &LibInit
};

static const char LibVersion[] = "$VER: " LIBNAME " " LIBVER " " LIBDATE LIBTYPE;

static const struct Resident RomTag = {
	RTC_MATCHWORD,
	&RomTag,
	&RomTag + 1,
#ifdef __MORPHOS__
	RTF_PPC | RTF_AUTOINIT,
#else
	RTF_AUTOINIT,
#endif
	VERSION,
	NT_LIBRARY,
	0,
	LIBNAME,
	&LibVersion[6],
	&LibInitStruct
};

#ifdef __MORPHOS__
ULONG __amigappc__ = 1;
#endif

/*****************************************************************************/

struct Library * LIBENT LibInit(
		/* DO NOT change the order of these arguments! */
		REG(d0) struct Library *LibBase,
		REG(a0) BPTR Segment,
		REG(a6) struct ExecBase *ExecBase)
{
	SysBase = ExecBase;
	GuiGFXBase = (struct GuiGFXBase *) LibBase;

#ifndef __MORPHOS__
	if(ExecBase->LibNode.lib_Version < 39 ||
		!(ExecBase->AttnFlags & AFF_68020))
	{
		return(NULL);
	}
#endif

	GuiGFXBase->LibNode.lib_Revision = REVISION;
	GuiGFXBase->LibSegment = Segment;
	GuiGFXBase->ExecBase = ExecBase;

	InitSemaphore(&GuiGFXBase->LockSemaphore);

	return(LibBase);
}

/*****************************************************************************/

struct Library * LIBENT LibOpen(
#ifdef __MORPHOS__
	void)
{
	struct Library *LibBase = (struct Library *)REG_A6;
#else
	REG(a6) struct Library *LibBase)
{
#endif
	DB(kprintf("guigfx opencount when entering LibOpen: %ld\n", LibBase->lib_OpenCnt));

	GuiGFXBase->LibNode.lib_Flags &= ~LIBF_DELEXP;
	GuiGFXBase->LibNode.lib_OpenCnt++;

	ObtainSemaphore(&GuiGFXBase->LockSemaphore);

	if(GuiGFXBase->LibNode.lib_OpenCnt == 1)
	{
		if(!GGFX_Init())
		{
			GGFX_Exit();

			GuiGFXBase->LibNode.lib_OpenCnt--;

			LibBase = NULL;
		}
	}

	ReleaseSemaphore(&GuiGFXBase->LockSemaphore);

	return(LibBase);
}

/*****************************************************************************/

#ifdef __MORPHOS__
BPTR LIBExpunge(void)
{
	struct Library *LibBase = (struct Library *)REG_A6;

	return LibExpunge(LibBase);
}
#endif

BPTR LIBENT LibExpunge(
	REG(a6) struct Library *LibBase)
{
	DB(kprintf("guigfx opencount when entering LibExpunge: %ld\n", LibBase->lib_OpenCnt));

	if(GuiGFXBase->LibNode.lib_OpenCnt == 0 && 
		GuiGFXBase->LibSegment != NULL)
	{
		BPTR TempSegment = GuiGFXBase->LibSegment;

		Remove((struct Node *)GuiGFXBase);

		FreeMem((APTR)((ULONG)(GuiGFXBase) - (ULONG)(GuiGFXBase->LibNode.lib_NegSize)),
			GuiGFXBase->LibNode.lib_NegSize + GuiGFXBase->LibNode.lib_PosSize);

		GuiGFXBase = NULL;

		return(TempSegment);
	}
	else
	{
		GuiGFXBase->LibNode.lib_Flags |= LIBF_DELEXP;

		return(NULL);
	}
}

/*****************************************************************************/

BPTR LIBENT LibClose(
#ifdef __MORPHOS__
	void)
{
	struct Library *LibBase = (struct Library *)REG_A6;
#else
	REG(a6) struct Library *LibBase)
{
#endif
	DB(kprintf("guigfx opencount when entering LibClose: %ld\n", LibBase->lib_OpenCnt));

	ObtainSemaphore(&GuiGFXBase->LockSemaphore);

	if (GuiGFXBase->LibNode.lib_OpenCnt > 0)
	{
		if (!(--GuiGFXBase->LibNode.lib_OpenCnt))
			GGFX_Exit();
	}

	ReleaseSemaphore(&GuiGFXBase->LockSemaphore);

	if(GuiGFXBase->LibNode.lib_OpenCnt == 0 && 
		(GuiGFXBase->LibNode.lib_Flags & LIBF_DELEXP))
	{
		DB(kprintf("!>>closing down guigfx.library.\n"));
		return(LibExpunge(LibBase));
	}
	else
	{
		return(NULL);
	}
}

/*****************************************************************************/

#ifdef __MORPHOS__
PIC *MakePictureA_GATE(void)
{
	APTR array = (APTR)REG_A0;
	UWORD width = (UWORD)REG_D0;
	UWORD height = (UWORD)REG_D1;
	TAGLIST tags = (TAGLIST)REG_A1;

	return MakePictureA(array, width, height, tags);
}

PIC *LoadPictureA_GATE(void)
{
	STRPTR filename = (STRPTR)REG_A0;
	TAGLIST tags = (TAGLIST)REG_A1;

	return LoadPictureA(filename, tags);
}

PIC *ReadPictureA_GATE(void)
{
	struct RastPort *rp = (struct RastPort *)REG_A0;
	struct ColorMap *cm = (struct ColorMap *)REG_A1;
	UWORD x = (UWORD)REG_D0;
	UWORD y = (UWORD)REG_D1;
	UWORD width = (UWORD)REG_D2;
	UWORD height = (UWORD)REG_D3;
	TAGLIST tags = (TAGLIST)REG_A2;

	return ReadPictureA(rp, cm, x, y, width, height, tags);
}

PIC *ClonePictureA_GATE(void)
{
	PIC *pic = (PIC *)REG_A0;
	TAGLIST tags = (TAGLIST)REG_A1;

	return ClonePictureA(pic, tags);
}

void DeletePicture_GATE(void)
{
	PIC *pic = (PIC *)REG_A0;

	DeletePicture(pic);
}

BOOL UpdatePicture_GATE(void)
{
	PIC *pic = (PIC *)REG_A0;

	return UpdatePicture(pic);
}

COLORHANDLE *AddPictureA_GATE(void)
{
	PSM *psm = (PSM *)REG_A0;
	PIC *pic = (PIC *)REG_A1;
	TAGLIST tags = (TAGLIST)REG_A2;

	return AddPictureA(psm, pic, tags);
}

COLORHANDLE *AddPaletteA_GATE(void)
{
	PSM *psm = (PSM *)REG_A0;
	PALETTE palette = (PALETTE)REG_A1;
	TAGLIST tags = (TAGLIST)REG_A2;

	return AddPaletteA(psm, palette, tags);
}

COLORHANDLE *AddPixelArrayA_GATE(void)
{
	PSM *psm = (PSM *)REG_A0;
	APTR array = (APTR)REG_A1;
	UWORD width = (UWORD)REG_D0;
	UWORD height = (UWORD)REG_D1;
	TAGLIST tags = (TAGLIST)REG_A2;

	return AddPixelArrayA(psm, array, width, height, tags);
}

void RemColorHandle_GATE(void)
{
	COLORHANDLE *colorhandle = (COLORHANDLE *)REG_A0;

	RemColorHandle(colorhandle);
}

PSM *CreatePenShareMapA_GATE(void)
{
	TAGLIST tags = (TAGLIST)REG_A0;

	return CreatePenShareMapA(tags);
}

void DeletePenShareMap_GATE(void)
{
	PSM *psm = (PSM *)REG_A0;

	DeletePenShareMap(psm);
}

DRAWHANDLE *ObtainDrawHandleA_GATE(void)
{
	PSM *psm = (PSM *)REG_A0;
	struct RastPort *rp = (struct RastPort *)REG_A1;
	struct ColorMap *cm = (struct ColorMap *)REG_A2;
	TAGLIST tags = (TAGLIST)REG_A3;

	return ObtainDrawHandleA(psm, rp, cm, tags);
}

void ReleaseDrawHandle_GATE(void)
{
	DRAWHANDLE *drawhandle = (DRAWHANDLE *)REG_A0;

	ReleaseDrawHandle(drawhandle);
}

BOOL DrawPictureA_GATE(void)
{
	DRAWHANDLE *drawhandle = (DRAWHANDLE *)REG_A0;
	PIC *pic = (PIC *)REG_A1;
	UWORD x = (UWORD)REG_D0;
	UWORD y = (UWORD)REG_D1;
	TAGLIST tags = (TAGLIST)REG_A2;

	return DrawPictureA(drawhandle, pic, x, y, tags);
}

BOOL MapPaletteA_GATE(void)
{
	DRAWHANDLE *drawhandle = (DRAWHANDLE *)REG_A0;
	PALETTE palette = (PALETTE)REG_A1;
	UBYTE *pentab = (UBYTE *)REG_A2;
	TAGLIST tags = (TAGLIST)REG_A3;

	return MapPaletteA(drawhandle, palette, pentab, tags);
}

LONG MapPenA_GATE(void)
{
	DRAWHANDLE *drawhandle = (DRAWHANDLE *)REG_A0;
	ULONG rgb = (ULONG)REG_D0;
	TAGLIST tags = (TAGLIST)REG_A1;

	return MapPenA(drawhandle, rgb, tags);
}

struct BitMap *CreatePictureBitMapA_GATE(void)
{
	DRAWHANDLE *drawhandle = (DRAWHANDLE *)REG_A0;
	PIC *pic = (PIC *)REG_A1;
	TAGLIST tags = (TAGLIST)REG_A2;

	return CreatePictureBitMapA(drawhandle, pic, tags);
}

ULONG DoPictureMethodA_GATE(void)
{
	APTR pic = (APTR)REG_A0;
	ULONG method = (ULONG)REG_D0;
	ULONG *arguments = (ULONG *)REG_A1;

	return DoPictureMethodA(pic, method, arguments);
}

ULONG GetPictureAttrsA_GATE(void)
{
	PIC *pic = (PIC *)REG_A0;
	TAGLIST tags = (TAGLIST)REG_A1;

	return GetPictureAttrsA(pic, tags);
}

ULONG LockPictureA_GATE(void)
{
	PIC *pic = (PIC *)REG_A0;
	ULONG mode = (ULONG)REG_D0;
	ULONG *arguments = (ULONG *)REG_A1;

	return LockPictureA(pic, mode, arguments);
}

void UnLockPicture_GATE(void)
{
	PIC *pic = (PIC *)REG_A0;
	ULONG mode = (ULONG)REG_D0;

	UnLockPicture(pic, mode);
}

BOOL IsPictureA_GATE(void)
{
	char *filename = (char *)REG_A0;
	TAGLIST tags = (TAGLIST)REG_A1;

	return IsPictureA(filename, tags);
}

DIRECTDRAWHANDLE *CreateDirectDrawHandleA_GATE(void)
{
	DRAWHANDLE *drawhandle = (DRAWHANDLE *)REG_A0;
	UWORD sw = (UWORD)REG_D0;
	UWORD sh = (UWORD)REG_D1;
	UWORD dw = (UWORD)REG_D2;
	UWORD dh = (UWORD)REG_D3;
	TAGLIST tags = (TAGLIST)REG_A1;

	return CreateDirectDrawHandleA(drawhandle, sw, sh, dw, dh, tags);
}

void DeleteDirectDrawHandle_GATE(void)
{
	DIRECTDRAWHANDLE *ddh = (DIRECTDRAWHANDLE *)REG_A0;

	DeleteDirectDrawHandle(ddh);
}

BOOL DirectDrawTrueColorA_GATE(void)
{
	DIRECTDRAWHANDLE *ddh = (DIRECTDRAWHANDLE *)REG_A0;
	ULONG *array = (ULONG *)REG_A1;
	UWORD x = (UWORD)REG_D0;
	UWORD y = (UWORD)REG_D1;
	TAGLIST tags = (TAGLIST)REG_A2;

	return DirectDrawTrueColorA(ddh, array, x, y, tags);
}

BOOL CreatePictureMaskA_GATE(void)
{
	PIC *pic = (PIC *)REG_A0;
	UBYTE *array = (UBYTE *)REG_A1;
	UWORD arraywidth = (UWORD)REG_D0;
	TAGLIST tags = (TAGLIST)REG_A2;

	return CreatePictureMaskA(pic, array, arraywidth, tags);
}

#endif

