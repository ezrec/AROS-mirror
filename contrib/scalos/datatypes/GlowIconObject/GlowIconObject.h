// GlowIconObject.h
// $Date$
// $Revision$


#ifndef	GLOWICONOBJECT_H_INCLUDED
#define	GLOWICONOBJECT_H_INCLUDED

#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <scalos/scalosgfx.h>

#include <defs.h>

//-----------------------------------------------------------------------------

#define	MEMPOOL_MEMFLAGS	MEMF_PUBLIC
#define	MEMPOOL_PUDDLESIZE	16384
#define	MEMPOOL_THRESHSIZE	16384

//-----------------------------------------------------------------------------

#define	LIB_VERSION	40
#define	LIB_REVISION	14

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

//-----------------------------------------------------------------------------

#define	BYTESPERROW(width)      ((((width) + 15) & 0xfff0) >> 3)

// Width for TempRp.BitMap for ReadPixelLine8() and WritePixelLine8()
#define	TEMPRP_WIDTH(width)	(8 * ((((width) + 15) >> 4) << 1))

//-----------------------------------------------------------------------------

#ifndef __AROS__
#define	BNULL		((BPTR) NULL)
#endif

//-----------------------------------------------------------------------------

#define	ID_ICON			MAKE_ID('I','C','O','N')
#define	ID_FACE			MAKE_ID('F','A','C','E')
#define	ID_IMAG			MAKE_ID('I','M','A','G')
#define ID_ARGB			MAKE_ID('A','R','G','B')

//-----------------------------------------------------------------------------

struct GlowIconObjectDtLibBase
	{
	struct ClassLibrary nib_ClassLibrary;

	struct SegList *nib_SegList;

	UBYTE nib_Initialized;
	};

//----------------------------------------------------------------------------

LIBFUNC_PROTO(ObtainInfoEngine, libbase, ULONG);

ULONG InitDatatype(struct GlowIconObjectDtLibBase *dtLib);
ULONG OpenDatatype(struct GlowIconObjectDtLibBase *dtLib);
void CloseDatatype(struct GlowIconObjectDtLibBase *dtLib);

/* ------------------------------------------------- */

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

struct FaceChunk	/* 6 Bytes */
	{
	UBYTE fc_Width;		// Width - 1
	UBYTE fc_Height;	// Height - 1
	UBYTE fc_Flags;
	UBYTE fc_AspectRatio;
	UWORD fc_MaxPaletteBytes;
	};

// Values in fc_Flags
#define	FCB_Borderless		0
#define	FCF_Borderless		(1 << FCB_Borderless)

/* ------------------------------------------------- */

struct ImageChunk	/* 10 Bytes */
	{
	UBYTE ic_TransparentColor;		// index of transparent color
	UBYTE ic_PaletteSize;			// number of palette entries - 1
	UBYTE ic_Flags;				// see below
	UBYTE ic_ImageCompressionType;		// compression type - imagedata ?
	UBYTE ic_PaletteCompressionType;	// compression type - palette ?
	UBYTE ic_BitsPerPixel;			// number of significant bits/pixel
	UWORD ic_NumImageBytes;			// number of compressed image bytes - 1
	UWORD ic_NumPaletteBytes;		// number of compressed palette bytes - 1
	};

#define	ICB_IsTransparent	0
#define	ICF_IsTransparent	(1 << ICB_IsTransparent)
#define	ICB_HasPalette		1
#define	ICF_HasPalette		(1 << ICB_HasPalette)

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

/* ------------------------------------------------- */

struct NewImage
	{
	WORD nim_Width;
	WORD nim_Height;

	struct ImageChunk nim_ImageChunk;

	UWORD nim_PaletteSize;		// number of palette entries

	UBYTE *nim_ImageData;		// (allocated) chunky image data, 1 byte per pixel

	struct ColorRegister *nim_Palette;	// (allocated) palette data, NULL for 32bit GlowIcons

	struct ARGBHeader nim_ARGBheader;
	};

//-----------------------------------------------------------------------------

struct NewImagePenList
	{
	ULONG		nip_PaletteSize;
	UBYTE 		*nip_PenArray;		// Array of allocated pens for Normal image, 1 byte per pen
	};

//-----------------------------------------------------------------------------

struct InstanceData
	{
   	struct DiskObject	*aio_DiskObject;
   	struct DrawerData	*aio_myDrawerData;
   	struct Screen		*aio_Screen;

	UWORD			aio_ImageLeft;
	UWORD			aio_ImageTop;

	UBYTE			aio_Borderless;			//Flag: Don't draw border around icon!

	struct Image		aio_DoImage1;			//diskObj.do_Gadget.GadgetRender
	struct Image		aio_DoImage2;

	struct FaceChunk	aio_FaceChunk;

	STRPTR			aio_DefaultTool;
	STRPTR			aio_ToolWindow;
	STRPTR			*aio_ToolTypes;
	ULONG			aio_ToolTypesLength;		// Allocated length for aio_ToolTypes

	struct NewImage		*aio_Image1;			// Normal image
	struct NewImage		*aio_Image2;			// Selected image

	struct NewImagePenList	aio_PenList1;			// pen list for Normal image
	struct NewImagePenList	aio_PenList2;			// pen list for selected image

	ULONG			aio_BackfillPenNorm;		// pen to use for unselected background fill or IDTA_BACKFILL_NONE
	ULONG			aio_BackfillPenSel;		// pen to use for selected background fill or IDTA_BACKFILL_NONE
        };

//---------------------------------------------------------------

// private and temporary Data during dtWrite

struct WriteData
	{
	STRPTR		aiowd_DefaultTool;
	STRPTR		*aiowd_ToolTypes;
	ULONG		aiowd_StackSize;
	struct IBox 	*aiowd_WindowRect;
	ULONG		aiowd_CurrentX;
	ULONG		aiowd_CurrentY;
	ULONG		aiowd_Flags;
	ULONG		aiowd_ViewModes;
	ULONG		aiowd_Type;
	};

/* ------------------------------------------------- */

#define UGetByte()	(*source++)
#define UPutByte(c)	(*dest++ = (c))

/* ------------------------------------------------- */

#define	Sizeof(x)	(sizeof(x) / sizeof(x[0]))

//-----------------------------------------------------------------------------

// defined in  mempools.lib

extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);

/* ------------------------------------------------- */

// from debug.lib
#ifdef __AROS__
#include <clib/arossupport_protos.h>
#define KPrintF kprintf
#else
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);
#endif


#define	d1(x)		;
#define	d2(x)		{ Forbid(); x; Permit(); }

//#define	TIMESTAMPS
#define	TIMESTAMP_d1()		;
#define	TIMESTAMP_d2()		\
	{			\
	struct EClockVal ev;	\
	ULONG ticks;		\
	ticks = ReadEClock(&ev); \
	KPrintF("%s/%s/%ld: ticks=%lu  hi=%8lu  lo=%8lu\n", __FILE__, __FUNC__, __LINE__, ticks, ev.ev_hi, ev.ev_lo);	\
	}

/* ------------------------------------------------- */

#if !defined(__amigaos4__) && !defined(__AROS__)
VOID UpdateWorkbench(CONST_STRPTR name, BPTR parentlock, LONG action);

#if defined(__MORPHOS__)

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#define UpdateWorkbench(__p0, __p1, __p2) \
	LP3NR(30, UpdateWorkbench, \
		CONST_STRPTR , __p0, a0, \
		BPTR, __p1, a1, \
		LONG, __p2, d0, \
		, WB_BASE_NAME, 0, 0, 0, 0, 0, 0)

#else

#ifdef __SASC

#ifdef __CLIB_PRAGMA_LIBCALL
#pragma  libcall WorkbenchBase UpdateWorkbench        01e 09803
#endif /* __CLIB_PRAGMA_LIBCALL */

#else

#define WBInfo(lock, name, screen) \
	LP3NR(30, UpdateWorkbench, CONST_STRPTR, name, a0, BPTR, parentlock, a1, LONG, action, d0, \
	, WB_BASE_NAME)

#endif /* __SASC */

#endif /* __MORPHOS__ */

#endif /* !__amigaos4__ */

/* ------------------------------------------------- */

#endif // GLOWICONOBJECT_H_INCLUDED
