// iconobject.h
// $Date$
// $Revision$


#ifndef	ICONOBJECT_H_INCLUDED
#define	ICONOBJECT_H_INCLUDED

#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <scalos/scalosgfx.h>
#include <datatypes/iconobject.h>

#include <defs.h>
#include <stddef.h>

//-----------------------------------------------------------------------------

#define	NO_ICON_POSITION_SHORT	((UWORD) 0x8000)

#define	FRAMETYPE_NONE		((UWORD) -1)

#define	BYTESPERROW(width)      ((((width) + 15) & 0xfff0) >> 3)
#define	ALIGN_LONG(x)		(((x) + 3) & 0xfffc)

struct IconObjectDtLibBase;

//-----------------------------------------------------------------------------

LIBFUNC_PROTO(ObtainInfoEngine, libbase, ULONG);

ULONG InitDatatype(struct IconObjectDtLibBase *dtLib);
ULONG OpenDatatype(struct IconObjectDtLibBase *dtLib);
ULONG CloseDatatype(struct IconObjectDtLibBase *dtLib);

//----------------------------------------------------------------------------

#define	LIB_VERSION	40
#define	LIB_REVISION	27

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

//-----------------------------------------------------------------------------

//#define DEBUG_MEMORY

#ifdef DEBUG_MEMORY
#define MyAllocVecPooled(mempool, size)		\
	MyAllocVecPooled_Debug(mempool, size, __FILE__, __FUNC__, __LINE__)
#define MyFreeVecPooled(mempool, mem)		\
	MyFreeVecPooled_Debug(mempool, mem, __FILE__, __FUNC__, __LINE__)
#endif /* DEBUG_MEMORY */

//-----------------------------------------------------------------------------

#define	PUBMEMPOOL_MEMFLAGS	MEMF_PUBLIC
#define	PUBMEMPOOL_PUDDLESIZE	16384
#define	PUBMEMPOOL_THRESHSIZE	16384

//-----------------------------------------------------------------------------

#define	CHIPMEMPOOL_MEMFLAGS	(MEMF_PUBLIC | MEMF_CHIP)
#define	CHIPMEMPOOL_PUDDLESIZE	8192
#define	CHIPMEMPOOL_THRESHSIZE	8192

//-----------------------------------------------------------------------------

struct IconObjectDtLibBase
	{
	struct ClassLibrary nib_ClassLibrary;

	struct SegList *nib_SegList;

	UBYTE nib_Initialized;
	};

//-----------------------------------------------------------------------------

struct IconObjectTextLine
	{
	UWORD	iotl_numchars;
	UWORD	iotl_TextStart;
	WORD	iotl_TextLeft;
	WORD	iotl_TextWidth;
	WORD	iotl_TextHeight;
	struct TextExtent iotl_TextExtent;
	};

struct IconObjectARGB
	{
	struct ARGBHeader iargb_ARGBimage;	// optional ARGB image data,  (gg->Width x gg->Height)
	struct ARGBHeader iargb_ScaledARGBImage;

	struct ARGBHeader iargb_ClonedARGBImage;
	ULONG	iargb_CopyARGBImageData;	// Flag: create a copy of

	UBYTE	*iargb_AlphaChannel;		// optional alpha channel data, (gg->Width x gg->Height)
	};

struct IconObjectMask
	{
	UBYTE 	*iom_Mask;			// unselected icon mask - raw BitMap plane with size of icon w/o borders
	struct BitMap *iom_MaskBM;		// BitMap with unselected icon mask - size includes borders, ready to be blitted to/from.
	struct BitMap *iom_MaskInvBM;		// BitMap with unselected icon inverted mask - size includes borders, ready to be blitted to/from.

	ULONG	iom_Width;		       	// Dimensions of iom_Mask
	ULONG	iom_Height;
	};

struct InstanceData
	{
	STRPTR	iobj_text;
	ULONG	iobj_stacksize;
	STRPTR	iobj_defaulttool;
	STRPTR	iobj_ToolWindow;
	STRPTR	*iobj_tooltypes;

	struct IBox iobj_winrect;

	STRPTR	iobj_name;
	UWORD	iobj_numchars;			// strlen(iobj_name)

	UWORD	iobj_frametype;
	UWORD	iobj_frametypesel;

	struct Rectangle iobj_SizeConstraints;
	UWORD	iobj_ScalingPercentage;

	struct TextExtent iobj_TextExtent;
	WORD	iobj_GlobalTextLeftOffset;

	UWORD	iobj_imgleft;			// Inner offsets
	UWORD	iobj_imgright;
	UWORD	iobj_imgtop;
	UWORD	iobj_imgbottom;

	ULONG	iobj_wincurx;			// VR Offset
	ULONG	iobj_wincury;

	struct IconObjectMask iobj_NormalMask;
	struct IconObjectMask iobj_SelectedMask;

	struct RastPort iobj_RenderRastPort;	// statically allocated RastPort for gg->GadgetRender
	struct RastPort iobj_SelectRastPort;	// statically allocated RastPort for gg->SelectRender

	struct RastPort iobj_BackfillRastPort;  // statically allocated RastPort for backfill
	struct RastPort *iobj_BackfillRp;       // allocated auxiliary RastPort for backfill

	struct IconObjectARGB iobj_NormalARGB;	// unselected ARGB image
	struct IconObjectARGB iobj_SelectedARGB; // selected ARGB image

	ULONG   iobj_UnscaledWidth;		// temporary storage for ITDM_ScaleIcon new width until used on next IDTM_Draw
	ULONG   iobj_UnscaledHeight;		// temporary storage for ITDM_ScaleIcon new height until used on next IDTM_Draw

	ULONG   iobj_ScaledWidth;		// temporary storage for ITDM_ScaleIcon new width until used on next IDTM_Draw
	ULONG   iobj_ScaledHeight;		// temporary storage for ITDM_ScaleIcon new height until used on next IDTM_Draw

	ULONG   iobj_NakedMaskWidth;		// Mask width without borders
	ULONG   iobj_NakedMaskHeight;		// Mask height without borders

	struct	Hook *iobj_FontHook;

	void 	*iobj_Fonthandle;		// opaque font handle
	struct	TextFont *iobj_Font;		// +jl+ 20010818 TextFont for the icon text or NULL
	UWORD	iobj_FontBaseLine;

	ULONG	iobj_ddflags;			// DrawerData Flags
	ULONG	iobj_UserFlags;			// +jl+ 20020928
	ULONG	iobj_OverlayType;		// Type of overlay to draw aboe this icon
						// not used by icon datatype - only for storage

	ULONG	iobj_NakedWidth;		// Width without borders
	ULONG	iobj_NakedHeight;		// Height without borders

	ULONG	iobj_UnscaledNakedWidth;	// Width without borders
	ULONG	iobj_UnscaledNakedHeight;	// Height without borders

	ULONG	iobj_TextStyle;			// +jl+ 20011231
	ULONG	iobj_Borderless;		// +jl+ 20030420

	UWORD	iobj_viewmodes;			//DrawerData Viewmodes

	UWORD	iobj_TextPen;			//standard Text pen
	UWORD	iobj_TextPenSel;		//selected Text pen
	UWORD	iobj_TextPenOutline;		//Outline Text pen
	UWORD	iobj_TextPenShadow;		//Shadow Text pen
	UWORD	iobj_textskip;			//Space between image and text
	UWORD	iobj_TextDrawMode;		// DrawMode for standard (no shadow, no outline) icon text
	UWORD	iobj_TextBackPen;		// Background pen for icon text, for JAM2 iobj_TextDrawMode
	UWORD   iobj_TextPenBgSel;		// Pen for rectangle around selected icon text

	ULONG	iobj_BackfillPenNorm;		// pen to use for unselected background fill or IDTA_BACKFILL_NONE
	ULONG	iobj_BackfillPenSel;		// pen to use for selected background fill or IDTA_BACKFILL_NONE

	UWORD	iobj_HalfShadowPen;
	UWORD	iobj_HalfShinePen;

	UWORD	iobj_TextRectBorderX;		// Additional horizontal border around selected icon text rectangle
	UWORD	iobj_TextRectBorderY;		// Additional vertical border around selected icon text rectangle
	UWORD	iobj_TextRectRadius;		// Radius for selected icon text rectangle corners

	struct SignalSemaphore iobj_LayoutSemaphore;

	UBYTE	iobj_type;			//IconType
	UBYTE	iobj_layoutflags;
	UBYTE	iobj_textmode;			//IDTV_TextMode_Normal, IDTV_TextMode_Outline, IDTV_TextMode_Shadow
	UBYTE	iobj_MultiLineText;		//Flag: Allow splitting of icon text into multiple lines
	UBYTE	iobj_SelectedTextRectangle;	//Flag: draw rectangle around selected icon text

	struct SignalSemaphore iobj_TextSemaphore;	//This semaphore protects iobj_text during IDTM_Layout and IDTM_Draw

	struct IconObjectTextLine iobj_TextLines[10];
	};

// Bits in iobj_layoutflags:

#define	IOBLAYOUTB_Size			0
#define	IOBLAYOUTB_NormalImage		1
#define	IOBLAYOUTB_SelectedImage	2
#define	IOBLAYOUTB_ScaleNormal		3
#define	IOBLAYOUTB_ScaleSelected	4
#define	IOBLAYOUTB_BackfillNormal	5
#define IOBLAYOUTB_BackfillSelected	6

#define	IOBLAYOUTF_Size			(1 << IOBLAYOUTB_Size)
#define	IOBLAYOUTF_NormalImage		(1 << IOBLAYOUTB_NormalImage)
#define	IOBLAYOUTF_SelectedImage	(1 << IOBLAYOUTB_SelectedImage)
#define IOBLAYOUTF_ScaleNormal		(1 << IOBLAYOUTB_ScaleNormal)
#define IOBLAYOUTF_ScaleSelected	(1 << IOBLAYOUTB_ScaleSelected)
#define	IOBLAYOUTF_BackfillNormal	(1 << IOBLAYOUTB_BackfillNormal)
#define IOBLAYOUTF_BackfillSelected	(1 << IOBLAYOUTB_BackfillSelected)

/* ------------------------------------------------- */

// Flag values for ScaleBitMap() and ScaleARGBArray()

#define SCALEFLAGB_BILINEAR    		0
#define SCALEFLAGB_AVERAGE     		1
#define SCALEFLAGB_DOUBLESIZE  	  	2
#define SCALEFLAGB_CORRECTASPECT      	3

#define SCALEFLAGF_BILINEAR 		(1L << SCALEFLAGB_BILINEAR)
#define SCALEFLAGF_AVERAGE  		(1L << SCALEFLAGB_AVERAGE)
#define SCALEFLAGF_DOUBLESIZE  		(1L << SCALEFLAGB_DOUBLESIZE)
#define SCALEFLAGF_CORRECTASPECT	(1L << SCALEFLAGB_CORRECTASPECT)

// check equality of 2 ARGB variables
#define	ARGB_EQ(a, b)	(*((ULONG *) &(a)) == *((ULONG *) &(b)))

/* ------------------------------------------------- */

#define	Sizeof(x)	(sizeof(x) / sizeof(x[0]))

/* ------------------------------------------------- */

// from debug.lib
#if defined(__AROS__)
#include <clib/arossupport_protos.h>
#define KPrintF kprintf
#else
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);
#endif

//#define	TIMESTAMPS
#define	TIMESTAMP_d1()		;
#define	TIMESTAMP_d2()		\
	{			\
	struct EClockVal ev;	\
	ULONG ticks;		\
	ticks = ReadEClock(&ev); \
	KPrintF("%s/%s/%ld: ticks=%lu  hi=%8lu  lo=%8lu\n", __FILE__, __FUNC__, __LINE__, ticks, ev.ev_hi, ev.ev_lo);	\
	}

#define	d1(x)		;
#define	d2(x)		{ Forbid(); x; Permit(); }


#define	ASSERT_d1(expression)		((void) 0)

#define ASSERT_d2(expression)  \
  ((void) ((expression) ? 0 : __assert (expression, __FILE__, __FUNC__, __LINE__)))

#define __assert(expression, file, func, lineno)  \
  ( KPrintF("%s/%s/%lu: !!! failed assertion `%s'\n", file, func, lineno, #expression), 0 )

/* ------------------------------------------------- */

// defined in argb.c

void ARGB_Draw(Class *cl, Object *o, struct iopDraw *opd, LONG x, LONG y);
BOOL GenMasksFromARGB(Class *cl, Object *o);
BOOL GenAlphaFromARGB(Class *cl, Object *o, struct IconObjectARGB *img, struct ARGBHeader *ImgHeader);

/* ------------------------------------------------- */

// defined in iconobj.c

extern void *PubMemPool;
extern void *ChipMemPool;

#ifdef DEBUG_MEMORY
APTR MyAllocVecPooled_Debug(void *MemPool, size_t Size, CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine);
void MyFreeVecPooled_Debug(void *MemPool, APTR *mem, CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine);
#else /* DEBUG_MEMORY */
APTR MyAllocVecPooled(void *MemPool, size_t Size);
void MyFreeVecPooled(void *MemPool, APTR *mem);
#endif /* DEBUG_MEMORY */

/* ------------------------------------------------- */

#endif // ICONOBJECT_H_INCLUDED
