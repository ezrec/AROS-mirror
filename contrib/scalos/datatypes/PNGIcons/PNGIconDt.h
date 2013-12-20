// PNGIconDt.h
// $Date$
// $Revision$


#ifndef PNGICONDT_H
#define	PNGICONDT_H

#include <datatypes/iconobject.h>
#include <scalos/scalosgfx.h>
#include <intuition/classes.h>
#include <workbench/workbench.h>

#include <defs.h>

//-----------------------------------------------------------------------------

#define	MAX_FILENAME	1024

#define	MINSTACKSIZE	8192

#define	MAXICONWIDTH 	256
#define	MAXICONHEIGHT	256

//-----------------------------------------------------------------------------

#if !defined(__AROS__)
#define	BNULL			((BPTR) NULL)
#endif

#define	MAKE_ARGB(r,g,b)	( ((r) << 16) + ((g) << 8) + (b) )

// Width for TempRp.BitMap for ReadPixelLine8() and WritePixelLine8()
#define	TEMPRP_WIDTH(width)	(8 * ((((width) + 15) >> 4) << 1))

//-----------------------------------------------------------------------------

#define	MEMPOOL_MEMFLAGS	MEMF_ANY
#define	MEMPOOL_PUDDLESIZE	32768
#define	MEMPOOL_THRESHSIZE	8192

//-----------------------------------------------------------------------------

#define	LIB_VERSION	40
#define	LIB_REVISION	21

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

//----------------------------------------------------------------------------

struct PngIconObjectDtLibBase
	{
	struct ClassLibrary nib_ClassLibrary;

	struct SegList *nib_SegList;

	UBYTE nib_Initialized;
	};

//-----------------------------------------------------------------------------

LIBFUNC_PROTO(ObtainInfoEngine, libbase, ULONG);

ULONG InitDatatype(struct PngIconObjectDtLibBase *dtLib);
ULONG OpenDatatype(struct PngIconObjectDtLibBase *dtLib);
ULONG CloseDatatype(struct PngIconObjectDtLibBase *dtLib);

//-----------------------------------------------------------------------------

struct InstanceData
	{
	char		id_FileName[MAX_FILENAME];

	ULONG		id_Type;		// WB Icon Type

	BOOL		id_DoNotFreeIcon;
	BOOL		id_IconHunkLoaded;	// set after first Icon hunk is read, ensures any following icon hunk is ignored
	BOOL		id_TypeSet;		// set if PNGICONA_TYPE  encountered - no more type guessing if set

	LONG		id_DirEntryType;	// fib_DirEntryType

	LONG		id_CurrentX;
	LONG		id_CurrentY;

	ULONG		id_StackSize;

	struct DrawerData id_DrawerData;	// Args for drawer window

	// PNG Icon data - also valid for Color icons
	struct ARGBHeader id_NormalImage;
	struct ARGBHeader id_SelectedImage;

	struct List	id_ToolTypesList;
	ULONG		id_ToolTypesCount;

	STRPTR		id_ToolWindow;
	STRPTR		id_DefaultTool;
	STRPTR		*id_ToolTypes;
	ULONG		id_ToolTypesLength;	// allocated length of id_ToolTypes
	};

//-----------------------------------------------------------------------------

#define PNGICONDUMMY TAG_USER + 0x1000

#define PNGICONA_XPOS		(PNGICONDUMMY + 0x01)
#define PNGICONA_YPOS		(PNGICONDUMMY + 0x02)
#define PNGICONA_DRAWERXPOS	(PNGICONDUMMY + 0x03)
#define PNGICONA_DRAWERYPOS	(PNGICONDUMMY + 0x04)
#define PNGICONA_DRAWERWIDTH	(PNGICONDUMMY + 0x05)
#define PNGICONA_DRAWERHEIGHT	(PNGICONDUMMY + 0x06)
#define PNGICONA_DRAWERVIEWMODE (PNGICONDUMMY + 0x07)
#define PNGICONA_TOOLWINDOW	(PNGICONDUMMY + 0x08)
#define PNGICONA_STACKSIZE	(PNGICONDUMMY + 0x09)
#define PNGICONA_DEFTOOL	(PNGICONDUMMY + 0x0A) // Identifies a project icon, may be null
#define PNGICONA_TOOLTYPE	(PNGICONDUMMY + 0x0B) // As many instances as needed
#define PNGICONA_SORTMODE	(PNGICONDUMMY + 0x0c)
#define PNGICONA_OFFSETX	(PNGICONDUMMY + 0x0d)
#define PNGICONA_OFFSETY	(PNGICONDUMMY + 0x0e)
#define PNGICONA_TYPE		(PNGICONDUMMY + 0x0f) // icon type WBDRAWER etc.

// Viewmodes for PNGICONA_DRAWERVIEWMODE
// Actually Ambient supports only 2 values: 2 (only icons, by icon) and 3 (all files, by icon)
// For now we extend and interpret the bits as follows:
// bit0: 0=show only icons 1=show all files
// bit1: 0=view by text	   1=view by icon
// bit2 and 3, just introduced to maintain WB compatibility, mean the sorting criterium:
// 00=by name 01=by date 02=by size 03=by type

#define PNGDM_SHOWALLFILES  	0x00000001 // 0=shows only icons
#define PNGDM_VIEWBYICON    	0x00000002 // 0=view by text
#define PNGDM_SORTMODEMASK  	0x0000000C
#define PNGDM_SORTBYNAME    	0x00000000
#define PNGDM_SORTBYDATE    	0x00000004
#define PNGDM_SORTBYSIZE    	0x00000008
#define PNGDM_SORTBYTYPE    	0x0000000C

#define PNGHEADER1     		0x89504E47
#define PNGHEADER2     		0x0D0A1A0A
#define PNGIHDRHEADER  		MAKE_ID('I','H','D','R')
#define PNGIDATHEADER  		MAKE_ID('I','D','A','T')
#define PNGICONHEADER  		MAKE_ID('i','c','O','n')
#define PNGTERMHEADER  		MAKE_ID('I','E','N','D')
#define PNGIENDCRC     		0xAE426082  // CRC of IEND hunk is obviously always the same

// PNG format: PNGHEADER1 and 2,(chunks),(end chunk)
// PNG chunk format: LONG DataLength, LONG ChunkID, (data), LONG DataCRC

struct PngChunk
	{
	ULONG DataLength;
	ULONG ID;
	UBYTE *Data;
	};

typedef struct
	{
	ULONG Width;
	ULONG Height; 
	UBYTE Depth;
	UBYTE ColorType;
	UBYTE Compression;
	UBYTE Filter;
	UBYTE Interlace;
	} IHDR;
#define IHDR_SIZE 13 // WARNING: sizeof(such a badly aligned struct) is unpracticable !!!!


struct PiIconSpecial
{
	struct DiskObject *Myself;	// Security backpointer
	ULONG Flags;			// See defines below
	ULONG Width, Height;		// Icon size

	// PNG Icon data - also valid for Color icons

	ULONG *ImgData;			// Points to a 32-bits ARGB array
	UBYTE *IconHunk;		// Points to the icOn hunk into PNGFile. Null if "vergin" icon
	APTR *PNGFile;			// Cache of original png icon file (mainly for writing)
	ULONG PNGFileSize;		// Size of PNGFile buffer
	UBYTE *FakeImgData;		// Buffer containing a special ID (see below) plus original icon filename
	STRPTR *TTArray;		// At icon creation is == icon->do_ToolTypes. See notes in the source.
	STRPTR DefaultTool;	
	struct Image Template;		// Planar workaround for Workbench to show something when dragging
};

//-----------------------------------------------------------------------------

#define	ISF_PNGICON		0x00000001	// 0=Normal icon, no PNG
#define	ISF_PNGHASALPHA		0x00000002	// PNG icon has alpha
#define	ISF_ALTIMAGE		0x00000004	// Icon has alternate image (only color icons for now)

struct TTNode
{
	struct Node Node;
	ULONG  Length;		// Incl. zero
	char	ToolType[1];
};

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

#define	Sizeof(array)	(sizeof(array) / sizeof((array)[0]))

//-----------------------------------------------------------------------------

#define	d1(x)	;
#define	d2(x)	x;

//#define	TIMESTAMPS
#define	TIMESTAMP_d1()		;
#define	TIMESTAMP_d2()		\
	{			\
	struct EClockVal ev;	\
	ULONG ticks;		\
	ticks = ReadEClock(&ev); \
	KPrintF("%s/%s/%ld: ticks=%lu  hi=%8lu  lo=%8lu\n", __FILE__, __FUNC__, __LINE__, ticks, ev.ev_hi, ev.ev_lo);	\
	}

//-----------------------------------------------------------------------------

#endif /* PNGICONDT_H */
