// AmigaIconObj35.h
// $Date$
// $Revision$


#ifndef	AMIGAICONOBJ35_H_INCLUDED
#define	AMIGAICONOBJ35_H_INCLUDED

#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>

#include <defs.h>

//-----------------------------------------------------------------------------

#define	MEMPOOL_MEMFLAGS	MEMF_PUBLIC
#define	MEMPOOL_PUDDLESIZE	16384
#define	MEMPOOL_THRESHSIZE	16384

//-----------------------------------------------------------------------------

#define	LIB_VERSION	40
#define	LIB_REVISION	18

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

//-----------------------------------------------------------------------------

struct AmigaIconObj35DtLibBase
	{
	struct ClassLibrary nib_ClassLibrary;

	struct SegList *nib_SegList;

	UBYTE nib_Initialized;
	};

//-----------------------------------------------------------------------------

ULONG InitDatatype(struct AmigaIconObj35DtLibBase *dtLib);
ULONG OpenDatatype(struct AmigaIconObj35DtLibBase *dtLib);
ULONG CloseDatatype(struct AmigaIconObj35DtLibBase *dtLib);

LIBFUNC_PROTO(ObtainInfoEngine, libbase, ULONG);

//-----------------------------------------------------------------------------

struct MappedImage
	{
	UBYTE			*aiomi_ImageData;		// Image data, 1 byte per pixel

	struct ColorRegister 	*aiomi_Palette;
	ULONG			aiomi_PaletteSize;		// number of entries in aiomi_Palette

	UBYTE			*aiomi_PenList;			// (allocated) array of used pens

	ULONG			aiomi_TransparentColor;

	UBYTE			*aiomi_Mask;

	UBYTE			*aiomi_AllocatedImageData;
	struct ColorRegister	*aiomi_AllocatedPalette;
   	};


struct InstanceData
	{
   	struct DiskObject	*aio_DiskObject;
   	struct DrawerData	*aio_myDrawerData;
   	struct Screen		*aio_Screen;

	struct MappedImage	aio_Image1;
	struct MappedImage	aio_Image2;

	UWORD			aio_ImageLeft;
	UWORD			aio_ImageTop;
	UWORD			aio_MappedWidth;
	UWORD			aio_MappedHeight;

	ULONG			aio_Borderless;

	UBYTE			aio_DoNotFreeDiskObject;	//Flag: free icon on quit ?
	UBYTE			aio_AlwaysUseDrawIconState;	//Flag: always use icon.library DrawIconStateA()
	UBYTE			aio_fIsPaletteMapped;		//Flag: is palette mapped icon
	UBYTE			aio_fLayoutOk;

	struct Hook		*aio_RenderHook;		//new (struct Hook *)
	};

//---------------------------------------------------------------

struct tmp
	{
	ULONG	tmp_width;
	ULONG	tmp_height;
	ULONG	tmp_ispalmapped;
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

#define	Sizeof(x)	(sizeof(x) / sizeof(x[0]))

// Width for TempRp.BitMap for ReadPixelLine8() and WritePixelLine8()
#define	TEMPRP_WIDTH(width)	(8 * ((((width) + 15) >> 4) << 1))

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

/* ------------------------------------------------- */

#endif // AMIGAICONOBJ35_H_INCLUDED
