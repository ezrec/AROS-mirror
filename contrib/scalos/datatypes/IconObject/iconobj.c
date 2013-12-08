// iconobj.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/resident.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/cghooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/iconobject.h>
#include <utility/pack.h>
#include <libraries/mcpgfx.h>
#include <scalos/scalosgfx.h>

#define	__USE_SYSBASE
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/cybergraphics.h>
#include <proto/scalosgfx.h>
#include <proto/timer.h>

#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>

#include <defs.h>
#include <Year.h>
#include <ScalosMcpGfx.h>

#include "iconobj.h"

#ifdef __AROS__
// FIXME: temporary fix until we have figured out
// how to deal with these deprecated defines.
#define IA_ShadowPen    (IA_Dummy + 0x09)
#define IA_HighlightPen (IA_Dummy + 0x0A)
#endif

//----------------------------------------------------------------------------

#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) < (b) ? (a) : (b))

#define	IDTA_TAGBASE   (IDTA_Type - 1)

#define	AREAMAX		30

#define	TT_RADIUS	5	// default value for iobj_TextRectRadius

#define	TT_BORDER_X	4	// default value for iobj_TextRectBorderX
#define	TT_BORDER_Y	2       // default value for iobj_TextRectBorderY

//----------------------------------------------------------------------------

// Memory allocation

#define	SCALOS_MEM_START_MAGIC	0x5343414d
#define	SCALOS_MEM_END_MAGIC	0xe34341cd

#define	SCALOS_MEM_TRAILER	20

struct AllocatedMemFromPoolDebug
	{
	size_t		amp_Size;			// Size of allocated memory

	ULONG		amp_Line;			// Line number of allocator
	CONST_STRPTR	amp_File;			// File name of allocator
	CONST_STRPTR	amp_Function;			// Name of allocating function

	ULONG		amp_Magic;

	UBYTE		amp_UserData[0];		// Start of user-visible memory
	};

struct AllocatedMemFromPool
	{
	size_t		amp_Size;			// Size of allocated memory
	UBYTE		amp_UserData[0];		// Start of user-visible memory
	};

//----------------------------------------------------------------------------

struct GfxBase *GfxBase;
T_UTILITYBASE UtilityBase;
struct ExecBase *SysBase;
struct Library *CyberGfxBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
struct ScalosGfxBase *ScalosGfxBase;
#ifdef TIMESTAMPS
T_TIMERBASE TimerBase;
#endif /* TIMESTAMPS */
#ifdef __amigaos4__
struct Library *NewlibBase;
struct Interface *INewlib;
struct GraphicsIFace *IGraphics;
struct UtilityIFace *IUtility;
struct ExecIFace *IExec;
struct CyberGfxIFace *ICyberGfx;
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
struct ScalosGfxIFace *IScalosGfx;
#endif

void *PubMemPool;
void *ChipMemPool;

static struct SignalSemaphore MemPoolSemaphore;

static Class *IconObjectClass;

static const ULONG packTable[] =
	{
	PACK_STARTTABLE(IDTA_TAGBASE),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_UserFlags, 		InstanceData, iobj_UserFlags, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_OverlayType, 		InstanceData, iobj_OverlayType, 			PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextStyle, 		InstanceData, iobj_TextStyle, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Font, 			InstanceData, iobj_Font, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Borderless, 		InstanceData, iobj_Borderless, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Type, 			InstanceData, iobj_type, 				PKCTRL_UBYTE | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_FontHook, 		InstanceData, iobj_FontHook, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Fonthandle, 		InstanceData, iobj_Fonthandle, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Stacksize, 		InstanceData, iobj_stacksize, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_WinCurrentX, 		InstanceData, iobj_wincurx, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_WinCurrentY, 		InstanceData, iobj_wincury, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_FrameType, 		InstanceData, iobj_frametype, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_FrameTypeSel, 		InstanceData, iobj_frametypesel, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_HalfShadowPen, 		InstanceData, iobj_HalfShadowPen, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_HalfShinePen, 		InstanceData, iobj_HalfShinePen, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Flags, 			InstanceData, iobj_ddflags, 				PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_ViewModes, 		InstanceData, iobj_viewmodes, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextPen, 			InstanceData, iobj_TextPen, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextPenSel, 		InstanceData, iobj_TextPenSel, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextPenBgSel, 		InstanceData, iobj_TextPenBgSel, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextPenOutline, 		InstanceData, iobj_TextPenOutline, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextPenShadow, 		InstanceData, iobj_TextPenShadow, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextBackPen, 		InstanceData, iobj_TextBackPen, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextDrawMode, 		InstanceData, iobj_TextDrawMode, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextSkip, 		InstanceData, iobj_textskip, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_MultiLineText, 		InstanceData, iobj_MultiLineText, 			PKCTRL_UBYTE | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_TextMode, 		InstanceData, iobj_textmode, 				PKCTRL_UBYTE | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Backfill, 		InstanceData, iobj_BackfillPenNorm, 			PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_BackfillSel, 		InstanceData, iobj_BackfillPenSel, 			PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_InnerLeft, 		InstanceData, iobj_imgleft, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_InnerTop, 		InstanceData, iobj_imgtop, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_InnerRight, 		InstanceData, iobj_imgright, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_InnerBottom, 		InstanceData, iobj_imgbottom, 				PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_SelTextRectBorderX, 	InstanceData, iobj_TextRectBorderX, 			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_SelTextRectBorderY, 	InstanceData, iobj_TextRectBorderY,			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_SelTextRectRadius, 	InstanceData, iobj_TextRectRadius,			PKCTRL_UWORD | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_SelectedTextRectangle,	InstanceData, iobj_SelectedTextRectangle, 		PKCTRL_UBYTE | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Width_Mask_Normal, 	InstanceData, iobj_NormalMask.iom_Width, 		PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Height_Mask_Normal, 	InstanceData, iobj_NormalMask.iom_Height, 		PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Width_Mask_Selected, 	InstanceData, iobj_SelectedMask.iom_Width, 		PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_Height_Mask_Selected, 	InstanceData, iobj_SelectedMask.iom_Height, 		PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_CopyARGBImageData, 	InstanceData, iobj_NormalARGB.iargb_CopyARGBImageData,	PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENTRY(IDTA_TAGBASE, IDTA_CopySelARGBImageData,	InstanceData, iobj_SelectedARGB.iargb_CopyARGBImageData, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
	PACK_ENDTABLE
	};

//----------------------------------------------------------------------------

// must be public for AROS
SAVEDS(ULONG) INTERRUPT IconObjectDispatcher(Class *cl, Object *o, Msg msg);

//-----------------------------------------------------------------------------

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops);
static ULONG DtDispose(Class *cl, Object *o, Msg msg);
static ULONG DtHitTest(Class *cl, Object *o, struct gpHitTest *gpht);
static ULONG DtRender(Class *cl, Object *o, struct gpRender *gpr);
static ULONG DtErase(Class *cl, Object *o, struct iopErase *iope);
static ULONG DtFindToolType(Class *cl, Object *o, struct iopFindToolType *ioft);
static ULONG DtGetToolTypeValue(Class *cl, Object *o, struct iopGetToolTypeValue *iotv);
static ULONG DtDraw(Class *cl, Object *o, struct iopDraw *iopd);
static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *iopl);
static ULONG DtFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf);
static ULONG DtSet(Class *cl, Object *o, struct opSet *ops);
static ULONG DtGet(Class *cl, Object *o, struct opGet *opg);
static ULONG DtScaleIcon(Class *cl, Object *o, struct iopScaleIcon *iops);

static ULONG DoOmSet(Class *cl, Object *o, struct opSet *ops);
static void LayoutIconText(struct InstanceData *inst, struct RastPort *rp, struct ExtGadget *gg);
static void DrawIconText(Class *cl, Object *o, struct iopDraw *iopd);
static void GetPosition(struct iopDraw *iopd, struct ExtGadget *gg, WORD *x, WORD *y);
static void DrawTextAt(struct InstanceData *inst, struct RastPort *rp, ULONG x, ULONG y);
static void DoReLayout(struct InstanceData *inst, struct ExtGadget *gg, struct RastPort *rp);
static BOOL IsSeparator(char ch);
static ULONG MySetSoftStyle(struct InstanceData *inst, struct RastPort *rp, ULONG style, ULONG enable);
static void MyText(struct InstanceData *inst, struct RastPort *rp, CONST_STRPTR string, WORD Length);
static void MyTextExtent(struct InstanceData *inst, struct RastPort *rp, 
	CONST_STRPTR string, WORD Length, struct TextExtent *tExt);
static void MySetFont(struct InstanceData *inst, struct RastPort *rp, struct TextFont *tf);
static void MySetTransparency(struct InstanceData *inst, struct RastPort *rp, ULONG Transparency);
static void MyDoneRastPort(struct InstanceData *inst, struct RastPort *rp);
static UWORD MyGetFontBaseline(struct InstanceData *inst, struct RastPort *rp);
static void CalculateTextDimensions(struct InstanceData *inst, struct ExtGadget *gg, struct RastPort *rp);
static BOOL ToolTypeNameCmp(CONST_STRPTR ToolTypeName, CONST_STRPTR ToolTypeContents);
static void SetTags(Class *cl, Object *o, struct opSet *ops);
static void SetAllocTag(ULONG TagValue, STRPTR *DataPtr, struct TagItem *TagList);
static void SetAllocTagProtected(ULONG TagValue, STRPTR *DataPtr,
	struct TagItem *TagList, struct SignalSemaphore *Sema);
static STRPTR AllocCopyString(CONST_STRPTR OrigString);
static void SetToolTypes(struct InstanceData *inst, const STRPTR *NewToolTypes);
static STRPTR *CloneToolTypes(const STRPTR *OrigToolTypes);
static void DrawFrame(Class *cl, Object *o, struct RastPort *rp,
	ULONG Width, ULONG Height, UWORD FrameType);
static void DrawMaskFrame(struct InstanceData *inst, struct BitMap *DestBitMap,
	ULONG Width, ULONG Height, UWORD FrameType);
static struct RastPort *InitRast(Class *cl, Object *o, struct iopLayout *iopl, struct RastPort *rp);
static struct BitMap *GenMask(Class *cl, Object *o,
	ULONG BmWidth, ULONG BmHeight, ULONG BmDepth, ULONG BmFlags,
	UBYTE *MaskPlane,
	UWORD FrameType, ULONG MaskWidth, ULONG MaskHeight);
static struct BitMap *GenInvMask(Class *cl, Object *o, struct BitMap *SrcBitMap);
static BOOL ScaleRenderBitMapsNormal(Class *cl, Object *o,
	ULONG NewWidth, ULONG NewHeight, struct Screen *screen);
static BOOL ScaleRenderBitMapsSelected(Class *cl, Object *o,
	ULONG NewWidth, ULONG NewHeight, struct Screen *screen);
static void LayoutIconSize(Class *cl, Object *o, struct RastPort *rp);
static struct BitMap *ScaleMaskBitMap(struct BitMap *SourceBM,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG *NewWidth, ULONG *NewHeight,
	ULONG ScaleFlags, struct BitMap *FriendBM);
static void CheckIfScalingRequired(Class *cl, Object *o);
static void ReplaceARGBImage(struct IconObjectARGB *img, struct ARGBHeader *argbh);
static void FreeARGBImage(struct IconObjectARGB *img);
static void FreeARGBImageLayout(struct IconObjectARGB *img);
static void FreeMask(struct IconObjectMask *Mask);
static void LayoutARGB(Class *cl, Object *o, struct IconObjectARGB *img);
static BOOL ScaleARGB(Class *cl, Object *o, struct IconObjectARGB *img,
	struct IconObjectMask *Mask, ULONG NewWidth, ULONG NewHeight, ULONG ScaleFlags);
static void EraseIconText(Class *cl, Object *o, struct RastPort *rp, WORD x, WORD y);
static void DrawIconTextRect(Class *cl, Object *o, struct RastPort *rp, WORD x, WORD y);

static void DumpMaskPlane(const struct IconObjectMask *Mask);
static void DumpMask(const struct IconObjectMask *Mask);
static void DumpMaskBM(const struct IconObjectMask *Mask);

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

char ALIGNED libName[] = "iconobject.datatype";
char ALIGNED libIdString[] = "$VER: iconobject.datatype "
        STR(LIB_VERSION) "." STR(LIB_REVISION)
	" (02 Nov 2008 23:57:06) "
	COMPILER_STRING
	" ©1999" CURRENTYEAR " The Scalos Team";

//-----------------------------------------------------------------------------

LIBFUNC(ObtainInfoEngine, libbase, ULONG)
{
	(void) libbase;

	return (ULONG) IconObjectClass;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

// return 0 if error occurred
ULONG InitDatatype(struct IconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	dtLib->nib_Initialized = FALSE;

	return 1;
}

// return 0 if error occurred
ULONG OpenDatatype(struct IconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (!dtLib->nib_Initialized)
		{
		struct MsgPort *FBlitPort;
		ULONG ChipMemPoolFlags;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		dtLib->nib_Initialized = TRUE;

		InitSemaphore(&MemPoolSemaphore);

		CyberGfxBase = OpenLibrary(CYBERGFXNAME, 0);
		// CyberGfxBase may be NULL

		FBlitPort = FindPort("FBlit");
		d1(kprintf("%s/%s/%ld:  FBlitPort=%08lx\n", __FILE__, __FUNC__, __LINE__, FBlitPort));


		// don't use chip mem if CyberGfx library found or FBlit port found.
		if (FBlitPort || CyberGfxBase)
			{
			ChipMemPoolFlags = PUBMEMPOOL_MEMFLAGS;
			d1(kprintf("%s/%s/%ld: FBlitPort=%08lx CyberGfxBase=%08lx ChipMemPoolFlags = MEMF_PUBLIC\n", \
				__FILE__, __FUNC__, __LINE__, FBlitPort, CyberGfxBase));
			}
		else
			{
			ChipMemPoolFlags = CHIPMEMPOOL_MEMFLAGS;
			d1(kprintf("%s/%s/%ld: FBlitPort=%08lx CyberGfxBase=%08lx ChipMemPoolFlags = (MEMF_PUBLIC | MEMF_CHIP)\n", \
				__FILE__, __FUNC__, __LINE__, FBlitPort, CyberGfxBase));
			}

		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
		d1(kprintf("%s/%s/%ld: IntuitionBase=%08lx\n", __FILE__, __FUNC__, __LINE__, IntuitionBase));
		if (NULL == IntuitionBase)
			return 0;

		DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 39);
		d1(kprintf("%s/%s/%ld: DOSBase=%08lx\n", __FILE__, __FUNC__, __LINE__, DOSBase));
		if (NULL == DOSBase)
			return 0;

		UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
		d1(kprintf("%s/%s/%ld: UtilityBase=%08lx\n", __FILE__, __FUNC__, __LINE__, UtilityBase));
		if (NULL == UtilityBase)
			return 0;

		GfxBase = (struct GfxBase *) OpenLibrary(GRAPHICSNAME, 39);
		d1(kprintf("%s/%s/%ld: GfxBase=%08lx\n", __FILE__, __FUNC__, __LINE__, GfxBase));
		if (NULL == GfxBase)
			return 0;

		ScalosGfxBase = (struct ScalosGfxBase *) OpenLibrary(SCALOSGFXNAME, 41);
		d1(KPrintF("%s/%s/%ld: ScalosGfxBase=%08lx\n", __FILE__, __FUNC__, __LINE__, ScalosGfxBase));
		if (NULL == ScalosGfxBase)
			return 0;

#ifdef TIMESTAMPS
		{
		struct timerequest *iorequest;

		iorequest = (struct timerequest *) CreateIORequest(CreateMsgPort(), sizeof(struct timerequest));
		OpenDevice("timer.device", UNIT_VBLANK, &iorequest->tr_node, 0);
		TimerBase = (T_TIMERBASE) iorequest->tr_node.io_Device;
		}
#endif /* TIMESTAMPS */

#ifdef __amigaos4__
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			return 0;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			return 0;
		IDOS = (struct DOSIFace *) GetInterface((struct Library *) DOSBase, "main", 1, NULL);
		if (IDOS == NULL)
			return 0;
		IUtility = (struct UtilityIFace *) GetInterface((struct Library *) UtilityBase, "main", 1, NULL);
		if (NULL == IUtility)
			return 0;
		if (CyberGfxBase != NULL)       // CyberGfxBase may be NULL
			{
			ICyberGfx = (struct CyberGfxIFace *) GetInterface((struct Library *) CyberGfxBase, "main", 1, NULL);
			if (ICyberGfx == NULL)
				return 0;
			}
		IIntuition = (struct IntuitionIFace *) GetInterface((struct Library *) IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			return 0;
		IGraphics = (struct GraphicsIFace *) GetInterface((struct Library *) GfxBase, "main", 1, NULL);
		if (NULL == IGraphics)
			return 0;
		IScalosGfx = (struct ScalosGfxIFace *) GetInterface((struct Library *) ScalosGfxBase, "main", 1, NULL);
		if (NULL == IScalosGfx)
			return 0;
#endif /* __amigaos4__ */

		PubMemPool = CreatePool(PUBMEMPOOL_MEMFLAGS, PUBMEMPOOL_PUDDLESIZE, PUBMEMPOOL_THRESHSIZE);
		d1(kprintf("%s/%s/%ld: PubMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, PubMemPool));
		if (NULL == PubMemPool)
			return 0;
		
		ChipMemPool = CreatePool(ChipMemPoolFlags, CHIPMEMPOOL_PUDDLESIZE, CHIPMEMPOOL_THRESHSIZE);
		d1(kprintf("%s/%s/%ld: ChipMemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, ChipMemPool));
		if (NULL == ChipMemPool)
			return 0;
		
		IconObjectClass = dtLib->nib_ClassLibrary.cl_Class = MakeClass(libName, 
			"datatypesclass", NULL, sizeof(struct InstanceData), 0);
		d1(kprintf("%s/%s/%ld:  IconObjectClass=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObjectClass));
		if (NULL == IconObjectClass)
			return 0;

		SETHOOKFUNC(IconObjectClass->cl_Dispatcher, IconObjectDispatcher);
		IconObjectClass->cl_Dispatcher.h_Data = dtLib;

		// Make class available for the public
		AddClass(IconObjectClass);
		}

	d1(kprintf("%s/%s/%ld:  Open Success!\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}

void CloseDatatype(struct IconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt < 1)
		{
		if (IconObjectClass)
			{
			RemoveClass(IconObjectClass);
			FreeClass(IconObjectClass);
			IconObjectClass = dtLib->nib_ClassLibrary.cl_Class = NULL;
			}

		if (NULL != ChipMemPool)
			{
			DeletePool(ChipMemPool);
			ChipMemPool = NULL;
			}
		if (NULL != PubMemPool)
			{
			DeletePool(PubMemPool);
			PubMemPool = NULL;
			}

#ifdef __amigaos4__
		if (ICyberGfx)
			{
			DropInterface((struct Interface *)ICyberGfx);
			ICyberGfx = NULL;
			}
		if (IScalosGfx)
			{
			DropInterface((struct Interface *)IScalosGfx);
			IScalosGfx = NULL;
			}
		if (IGraphics)
			{
			DropInterface((struct Interface *)IGraphics);
			IGraphics = NULL;
			}
		if (IIntuition)
			{
			DropInterface((struct Interface *)IIntuition);
			IIntuition = NULL;
			}
		if (IUtility)
			{
			DropInterface((struct Interface *)IUtility);
			IUtility = NULL;
			}
		if (IDOS)
			{
			DropInterface((struct Interface *)IDOS);
			IDOS = NULL;
			}
		if (INewlib)
			{
			DropInterface(INewlib);
			INewlib = NULL;
			}
		if (NewlibBase)
			{
			CloseLibrary(NewlibBase);
			NewlibBase = NULL;
			}
#endif

		if (NULL != CyberGfxBase)
			{
			CloseLibrary(CyberGfxBase);
			CyberGfxBase = NULL;
			}
		if (NULL != ScalosGfxBase)
			{
			CloseLibrary((struct Library *) ScalosGfxBase);
			ScalosGfxBase = NULL;
			}
		if (NULL != GfxBase)
			{
			CloseLibrary((struct Library *) GfxBase);
			GfxBase = NULL;
			}
		if (NULL != IntuitionBase)
			{
			CloseLibrary((struct Library *) IntuitionBase);
			IntuitionBase = NULL;
			}
		if (NULL != UtilityBase)
			{
			CloseLibrary((struct Library *) UtilityBase);
			UtilityBase = NULL;
			}
		if (NULL != DOSBase)
			{
			CloseLibrary((struct Library *) DOSBase);
			DOSBase = NULL;
			}
		}

	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

}

//-----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT IconObjectDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld:  cl=%08lx  o=%08lx  msg=%08lx  MethodID=%08ld\n", __FILE__, __FUNC__, __LINE__,
		 cl, o, msg, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		o = (Object *) DoSuperMethodA(cl, o, msg);
		d1(KPrintF("%s/%s/%ld:  OM_NEW  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		if (o)
			{
			d1(KPrintF("%s/%s/%ld:  OM_NEW  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			if (!DtNew(cl, o, (struct opSet *) msg))
				{
				DoMethod(o, OM_DISPOSE);
				o = NULL;
				}
			}
		Result = (ULONG) o;
		break;

	case OM_DISPOSE:
		d1(kprintf("%s/%s/%ld:  OM_DISPOSE cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtDispose(cl, o, msg);
		break;

	case OM_GET:
		d1(kprintf("%s/%s/%ld:  OM_GET cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtGet(cl, o, (struct opGet *) msg);
		break;

	case OM_SET:
		d1(kprintf("%s/%s/%ld:  OM_SET cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtSet(cl, o, (struct opSet *) msg);
		break;

	case GM_HITTEST:
		d1(kprintf("%s/%s/%ld:  GM_HITTEST cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtHitTest(cl, o, (struct gpHitTest *) msg);
		break;

	case GM_RENDER:
		d1(kprintf("%s/%s/%ld:  GM_RENDER cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtRender(cl, o, (struct gpRender *) msg);
		break;

	case GM_GOACTIVE:
		d1(kprintf("%s/%s/%ld:  GM_GOACTIVE cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtRender(cl, o, (struct gpRender *) msg);
		break;

	case GM_GOINACTIVE:
		d1(kprintf("%s/%s/%ld:  GM_GOINACTIVEclcl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtRender(cl, o, (struct gpRender *) msg);
		break;

	case IDTM_Draw:
		d1(kprintf("%s/%s/%ld:  IDTM_Draw cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtDraw(cl, o, (struct iopDraw *) msg);
		break;

	case IDTM_Layout:
		Result = DtLayout(cl, o, (struct iopLayout *) msg);
		d1(KPrintF("%s/%s/%ld:  IDTM_Layout cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		break;

	case IDTM_FreeLayout:
		d1(kprintf("%s/%s/%ld:  IDTM_FreeLayout cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtFreeLayout(cl, o, (struct iopFreeLayout *) msg);
		break;

	case IDTM_FindToolType:
		d1(kprintf("%s/%s/%ld:  IDTM_FindToolType cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtFindToolType(cl, o, (struct iopFindToolType *) msg);
		break;

	case IDTM_GetToolTypeValue:
		d1(kprintf("%s/%s/%ld:  IDTM_GetToolTypeValue cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));
		Result = DtGetToolTypeValue(cl, o, (struct iopGetToolTypeValue *) msg);
		break;

	case IDTM_Erase:
		d1(kprintf("%s/%s/%ld:  IDTM_Erase cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));

		Result = DtErase(cl, o, (struct iopErase *) msg);
		break;

	case IDTM_ScaleIcon:
		d1(KPrintF("%s/%s/%ld:  IDTM_ScaleIcon cl=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg));

		Result = DtScaleIcon(cl, o, (struct iopScaleIcon *) msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	d1(kprintf("%s/%s/%ld:  result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

//-----------------------------------------------------------------------------

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	BOOL Success = TRUE;

	memset(inst, 0, sizeof(struct InstanceData));

	TIMESTAMP_d1();

	InitSemaphore(&inst->iobj_TextSemaphore);
	InitSemaphore(&inst->iobj_LayoutSemaphore);

	inst->iobj_TextPen = 1;
	inst->iobj_TextPenSel = 1;
	inst->iobj_TextPenOutline = 2;
	inst->iobj_TextPenBgSel = 3;
	inst->iobj_TextPenShadow = 1;
	inst->iobj_TextDrawMode = JAM1;
	inst->iobj_TextBackPen = 0;
	inst->iobj_TextRectBorderX = TT_BORDER_X;
	inst->iobj_TextRectBorderY = TT_BORDER_Y;
	inst->iobj_TextRectRadius = TT_RADIUS;

	inst->iobj_SelectedTextRectangle = 1;
	inst->iobj_textskip = 1;
	inst->iobj_frametypesel = (UWORD) -1;
	inst->iobj_TextStyle = FS_NORMAL;
	inst->iobj_BackfillPenNorm = inst->iobj_BackfillPenSel = IDTA_BACKFILL_NONE;

	inst->iobj_ScalingPercentage = 100;

	inst->iobj_SizeConstraints.MinX = inst->iobj_SizeConstraints.MinY = 0;
	inst->iobj_SizeConstraints.MaxX = inst->iobj_SizeConstraints.MaxY = SHRT_MAX;

	SetTags(cl, o, ops);

	d1(kprintf("%s/%s/%ld:  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
	TIMESTAMP_d1();

	return Success;
}

//-----------------------------------------------------------------------------

static ULONG DtDispose(Class *cl, Object *o, Msg msg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	ObtainSemaphore(&inst->iobj_LayoutSemaphore);

	MyFreeVecPooled(PubMemPool, (APTR *) &inst->iobj_text);
	MyFreeVecPooled(PubMemPool, (APTR *) &inst->iobj_defaulttool);
	MyFreeVecPooled(PubMemPool, (APTR *) &inst->iobj_ToolWindow);
	MyFreeVecPooled(PubMemPool, (APTR *) &inst->iobj_name);
	MyFreeVecPooled(PubMemPool, (APTR *) &inst->iobj_tooltypes);

	FreeARGBImage(&inst->iobj_NormalARGB);
	FreeARGBImage(&inst->iobj_SelectedARGB);

	if (gg->SelectRender)
		{
		struct RastPort *rp = (struct RastPort *) gg->SelectRender;

		d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, rp->BitMap));
		FreeBitMap(rp->BitMap);
		gg->SelectRender = NULL;
		}
	if (gg->GadgetRender)
		{
		struct RastPort *rp = (struct RastPort *) gg->GadgetRender;

		d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, rp->BitMap));
		FreeBitMap(rp->BitMap);
		gg->GadgetRender = NULL;
		}
	if (inst->iobj_BackfillRp)
		{
		if (inst->iobj_BackfillRp->BitMap)
			{
			d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_BackfillRp->BitMap));
			FreeBitMap(inst->iobj_BackfillRp->BitMap);
			inst->iobj_BackfillRp->BitMap = NULL;
			}

		inst->iobj_BackfillRp = NULL;
		}

	FreeMask(&inst->iobj_NormalMask);
	FreeMask(&inst->iobj_SelectedMask);

	ReleaseSemaphore(&inst->iobj_LayoutSemaphore);

	return DoSuperMethodA(cl, o, msg);
}

//-----------------------------------------------------------------------------

// For GM_HITTEST, return GMR_GADGETHIT if you were indeed hit,
// otherwise return zero.

static ULONG DtHitTest(Class *cl, Object *o, struct gpHitTest *gpht)
{
	ULONG Result = 0;

	d1(kprintf("%s/%s/%ld:  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	Result = DoSuperMethodA(cl, o, (Msg) gpht);
	if (GMR_GADGETHIT == Result)
		{
		struct InstanceData *inst = INST_DATA(cl, o);
		struct ExtGadget *gg = (struct ExtGadget *) o;
		struct BitMap *MaskBM;
		LONG x, y;
		ULONG BmWidth, BmHeight;

		if (gg->Flags & GFLG_SELECTED)
			MaskBM = inst->iobj_SelectedMask.iom_MaskBM;
		else
			MaskBM = inst->iobj_NormalMask.iom_MaskBM;

		d1(kprintf("%s/%s/%ld:  MaskBM=%08lx\n", __FILE__, __FUNC__, __LINE__, MaskBM));

		if (NULL == MaskBM)
			{
			Result = GMR_GADGETHIT;
			}
		else
			{
			d1(kprintf("%s/%s/%ld:  iobj_imgleft=%ld  iobj_imgtop=%ld\n", __FILE__, __FUNC__, __LINE__, inst->iobj_imgleft, inst->iobj_imgtop));

			x = gpht->gpht_Mouse.X - gg->LeftEdge - inst->iobj_imgleft;
			y = gpht->gpht_Mouse.Y - gg->TopEdge - inst->iobj_imgtop;

			BmWidth = GetBitMapAttr(MaskBM, BMA_WIDTH);
			BmHeight = GetBitMapAttr(MaskBM, BMA_HEIGHT);

			d1(kprintf("%s/%s/%ld:  x=%ld  y=%ld  BmWidth=%lu  BmHeight=%lu\n", __FILE__, __FUNC__, __LINE__, x, y, BmWidth, BmHeight));

			if ( x >= 0 && y >= 0 && x < BmWidth && y < BmHeight)
				{
				struct RastPort rp;

				InitRastPort(&rp);

				rp.BitMap = MaskBM;

				d1(kprintf("%s/%s/%ld:  ReadPixel()=%ld\n", __FILE__, __FUNC__, __LINE__, ReadPixel(&rp, x, y)));

				if (ReadPixel(&rp, x, y))
					Result = GMR_GADGETHIT;
				else
					Result = 0;
				}
			}
		}

	d1(kprintf("%s/%s/%ld:  o=%08lx  Result=%ld\n", __FILE__, __FUNC__, __LINE__, o, Result));

	return Result;
}

//-----------------------------------------------------------------------------

static ULONG DtRender(Class *cl, Object *o, struct gpRender *gpr)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct RastPort *rp;

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	DoSuperMethodA(cl, o, (Msg) gpr);

	if (GM_RENDER == gpr->MethodID)
		rp = gpr->gpr_RPort;
	else
		rp = ObtainGIRPort(gpr->gpr_GInfo);

	if (rp)
		{
		if (GM_RENDER == gpr->MethodID && !gpr->gpr_Redraw)
			gg->Flags ^= GFLG_SELECTED;

		if (((gg->Flags & GFLG_SELECTED) && !(inst->iobj_layoutflags & IOBLAYOUTF_SelectedImage))
			|| (!(gg->Flags & GFLG_SELECTED) && !(inst->iobj_layoutflags & IOBLAYOUTF_NormalImage)))
			{
			DoMethod(o, IDTM_Layout,
				gpr->gpr_GInfo->gi_Screen, 
				gpr->gpr_GInfo->gi_Window,
				rp,
				gpr->gpr_GInfo->gi_DrInfo,
				(gg->Flags & GFLG_SELECTED) ? IOLAYOUTF_SelectedImage : IOLAYOUTF_NormalImage);
			}

		DoMethod(o, IDTM_Draw, 
			gpr->gpr_GInfo->gi_Screen, 
			gpr->gpr_GInfo->gi_Window,
			rp,
			gpr->gpr_GInfo->gi_DrInfo,
			-gpr->gpr_GInfo->gi_Domain.Left,
			-gpr->gpr_GInfo->gi_Domain.Top,
			0);

		if (GM_RENDER != gpr->MethodID)
			ReleaseGIRPort(rp);
		}

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtErase(Class *cl, Object *o, struct iopErase *iope)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	// Erase icon image
	EraseRect(iope->iope_RastPort,
		iope->iope_XOffset + gg->LeftEdge,
		iope->iope_YOffset + gg->TopEdge,
		iope->iope_XOffset + gg->LeftEdge + gg->Width - 1,
		iope->iope_YOffset + gg->TopEdge + gg->Height - 1);

	// Erase icon text
	EraseIconText(cl, o,
		iope->iope_RastPort,
		iope->iope_YOffset + gg->TopEdge,
		iope->iope_XOffset + gg->LeftEdge);

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtFindToolType(Class *cl, Object *o, struct iopFindToolType *ioft)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG Result = 0;

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx  Start=%08lx  Name=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, o, inst, *ioft->ioftt_ToolTypeValue, ioft->ioftt_ToolTypeName));

	if (inst->iobj_tooltypes)
		{
		STRPTR *TTSearch = inst->iobj_tooltypes;

		if (*ioft->ioftt_ToolTypeValue)
			{
			// Start search from given position
			TTSearch = ioft->ioftt_ToolTypeValue;

			// find starting point inf tooltypes array
			while (*TTSearch)
				{
				if (*TTSearch == *ioft->ioftt_ToolTypeValue)
					break;
				TTSearch++;
				}

			d1(kprintf("%s/%s/%ld:  Start=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, TTSearch, *TTSearch));
			}

		while (*TTSearch)
			{
			d1(kprintf("%s/%s/%ld:  Search=<%s>\n", __FILE__, __FUNC__, __LINE__, *TTSearch));

			if (ToolTypeNameCmp(ioft->ioftt_ToolTypeName, *TTSearch))
				{
				// Found!
				Result = 1;
				d1(kprintf("%s/%s/%ld:  Found=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, TTSearch, *TTSearch));
				*ioft->ioftt_ToolTypeValue = *TTSearch;
				break;
				}
			TTSearch++;
			}
		}

	d1(kprintf("%s/%s/%ld:  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

//-----------------------------------------------------------------------------

static ULONG DtGetToolTypeValue(Class *cl, Object *o, struct iopGetToolTypeValue *iotv)
{
//	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG Result = 0;
	STRPTR tt = iotv->iotv_ToolTypeValue;

	d1(kprintf("%s/%s/%ld:  ToolType=<%s>\n", __FILE__, __FUNC__, __LINE__, tt, tt ? tt : (UBYTE *) ""));

	if (tt)
		{
		while (*tt && '=' !=  *tt)
			tt++;

		if ('=' ==  *tt)
			{
			++tt;	// Skip "="

			d1(kprintf("%s/%s/%ld:  Value=<%s>\n", __FILE__, __FUNC__, __LINE__, tt));

			if ('$' == *tt || (strlen(tt) > 2 && 0 == Stricmp(tt, "0x")))
				{
				// Hex string recognized
				LONG HexVal = 0;

				Result = 1;

				while (*tt)
					{
					UBYTE ch = ToUpper(*tt++);

					if (!isxdigit(ch))
						{
						Result = 0;
						break;
						}

					if (ch <= '9')
						HexVal = (HexVal << 4) + (ch - '0');
					else
						HexVal = (HexVal << 4) + (ch - 55);
					}

				if (Result)
					*iotv->iotv_Value = HexVal;
				}
			else
				{
				// Decimal string expected
				if (-1 != StrToLong(tt, iotv->iotv_Value))
					Result = 1;	// Success!
				}
			}
		}

	d1(kprintf("%s/%s/%ld:  Result=%ld  Value=%ld\n", __FILE__, __FUNC__, __LINE__, Result, *iotv->iotv_Value));

	return Result;
}

//-----------------------------------------------------------------------------

static ULONG DtDraw(Class *cl, Object *o, struct iopDraw *iopd)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct RastPort *rp;
	PLANEPTR MaskPlanePtr;
	WORD Left, Top;

	d1(KPrintF("%s/%s/%ld:  START o=%08lx <%s>  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->iobj_text, inst));
	d1(KPrintF("%s/%s/%ld:  iobj_selmaskbm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskBM));

	ObtainSemaphoreShared(&inst->iobj_LayoutSemaphore);

	if (inst->iobj_layoutflags & IOBLAYOUTF_ScaleNormal)
		{
		d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
		ScaleRenderBitMapsNormal(cl, o,
			inst->iobj_ScaledWidth, inst->iobj_ScaledHeight,
			iopd->iopd_Screen);
		}
	if (inst->iobj_layoutflags & IOBLAYOUTF_ScaleSelected)
		{
		d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
		ScaleRenderBitMapsSelected(cl, o,
			inst->iobj_ScaledWidth, inst->iobj_ScaledHeight,
			iopd->iopd_Screen);
		}

	if (gg->Flags & GFLG_SELECTED)
		{
		d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  iobj_selmaskbm=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst, inst->iobj_SelectedMask.iom_MaskBM));
		rp = (struct RastPort *) gg->SelectRender;
		MaskPlanePtr = inst->iobj_SelectedMask.iom_MaskBM ? inst->iobj_SelectedMask.iom_MaskBM->Planes[0] : NULL;

		d1(DumpMask(&inst->iobj_SelectedMask));

		if (rp && (inst->iobj_layoutflags & IOBLAYOUTF_BackfillSelected))
			{
			inst->iobj_layoutflags &= ~IOBLAYOUTF_BackfillSelected;
			d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  iobj_BackfillPenSel=%ld\n", __FILE__, __FUNC__, __LINE__, o, inst, inst->iobj_BackfillPenSel));

			if (IDTA_BACKFILL_NONE != inst->iobj_BackfillPenSel)
				{
				// Fill transparent parts of icon with backfill color via inverted mask
				d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  rp=%08lx  iobj_BackfillRp=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst, rp, inst->iobj_BackfillRp));

				ASSERT_d1(gg->Width <  GetBitMapAttr(rp->BitMap, BMA_WIDTH));
				ASSERT_d1(gg->Height <  GetBitMapAttr(rp->BitMap, BMA_HEIGHT));

				SetRast(inst->iobj_BackfillRp, inst->iobj_BackfillPenSel);
				d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
				BltMaskBitMapRastPort(inst->iobj_BackfillRp->BitMap,
					0, 0,
					rp, 0, 0,
					gg->Width, gg->Height,
					ABC | ABNC | ANBC,
					inst->iobj_SelectedMask.iom_MaskInvBM->Planes[0]);

				d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  iobj_BackfillRp=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst, inst->iobj_BackfillRp));
				}
			}
		}
	else
		{
		rp = (struct RastPort *) gg->GadgetRender;
		MaskPlanePtr = inst->iobj_NormalMask.iom_MaskBM ? inst->iobj_NormalMask.iom_MaskBM->Planes[0] : NULL;

		d1(DumpMask(&inst->iobj_NormalMask));

		if (rp && (inst->iobj_layoutflags & IOBLAYOUTF_BackfillNormal))
			{
			inst->iobj_layoutflags &= ~IOBLAYOUTF_BackfillNormal;
			d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  iobj_BackfillPenNorm=%ld\n", __FILE__, __FUNC__, __LINE__, o, inst, inst->iobj_BackfillPenNorm));

			if (IDTA_BACKFILL_NONE != inst->iobj_BackfillPenNorm)
				{
				// Fill transparent parts of icon with backfill color via inverted mask
				ASSERT_d1(gg->Width <  GetBitMapAttr(rp->BitMap, BMA_WIDTH));
				ASSERT_d1(gg->Height <  GetBitMapAttr(rp->BitMap, BMA_HEIGHT));
				
                                SetRast(inst->iobj_BackfillRp, inst->iobj_BackfillPenNorm);
				BltMaskBitMapRastPort(inst->iobj_BackfillRp->BitMap,
					0, 0,
					rp, 0, 0,
					gg->Width, gg->Height,
					ABC | ABNC | ANBC,
					inst->iobj_NormalMask.iom_MaskInvBM->Planes[0]);
				}
			}
		}

	GetPosition(iopd, gg, &Left, &Top);

	if (rp && !(iopd->iopd_DrawFlags & IODRAWF_NoImage))
		{
		d1(KPrintF("%s/%s/%ld:  o=%08lx  rp=%08lx  BitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, o, rp, rp->BitMap));
		d1(KPrintF("%s/%s/%ld:  Bytes=%ld  Rows=%ld  Flags=%02lx  Depth=%ld\n", \
			__FILE__, __FUNC__, __LINE__, rp->BitMap->BytesPerRow, rp->BitMap->Rows, rp->BitMap->Flags, rp->BitMap->Depth));

		if (inst->iobj_NormalARGB.iargb_ARGBimage.argb_ImageData && CyberGfxBase
			&& GetCyberMapAttr(iopd->iopd_Screen->RastPort.BitMap, CYBRMATTR_ISCYBERGFX))
			{
			LONG x, y;

			d1(KPrintF("%s/%s/%ld:  draw via ARGB_Draw o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

			x = iopd->iopd_XOffset;
			y = iopd->iopd_YOffset;

			if (IODRAWF_AbsolutePos & iopd->iopd_DrawFlags)
				{
				if (!(IODRAWF_NoText & iopd->iopd_DrawFlags))
					{
					x += gg->LeftEdge - gg->BoundsLeftEdge;
					y += gg->TopEdge - gg->BoundsTopEdge;
					}
				}
			else
				{
				x += gg->LeftEdge;
				y += gg->TopEdge;
				}

			ARGB_Draw(cl, o, iopd, x, y);
			}
		else
			{
			ULONG BackfillPen = (gg->Flags & GFLG_SELECTED) ? inst->iobj_BackfillPenSel : inst->iobj_BackfillPenNorm;

			if (!(iopd->iopd_DrawFlags & IODRAWF_NoEraseBg))
				{
				// Erase icon background
				d1(KPrintF("%s/%s/%ld:  erase background  o=%08lx  inst=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, o, inst));

				EraseRect(iopd->iopd_RastPort,
					Left, Top,
					Left + gg->Width - 1,
					Top + gg->Height - 1);
				}

			d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  MaskPlanePtr=%08lx  BackfillPen=%ld\n", __FILE__, __FUNC__, __LINE__, o, inst, MaskPlanePtr, BackfillPen));

			if (MaskPlanePtr && (IDTA_BACKFILL_NONE == BackfillPen))
				{
				ASSERT_d1(gg->Width <  GetBitMapAttr(iopd->iopd_RastPort->BitMap, BMA_WIDTH));
				ASSERT_d1(gg->Height <  GetBitMapAttr(iopd->iopd_RastPort->BitMap, BMA_HEIGHT));

				d1(KPrintF("%s/%s/%ld:  NormMaskWidth=%lu  NormMaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_Width, inst->iobj_NormalMask.iom_Height));
				d1(KPrintF("%s/%s/%ld:  SelMaskWidth=%lu  SelMaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_Width, inst->iobj_SelectedMask.iom_Height));
				d1(KPrintF("%s/%s/%ld:  iom_MaskBM: Width=%lu  Height=%lu  Depth=%lu  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
					GetBitMapAttr(inst->iobj_NormalMask.iom_MaskBM, BMA_WIDTH), \
					GetBitMapAttr(inst->iobj_NormalMask.iom_MaskBM, BMA_HEIGHT), \
					GetBitMapAttr(inst->iobj_NormalMask.iom_MaskBM, BMA_DEPTH), \
					GetBitMapAttr(inst->iobj_NormalMask.iom_MaskBM, BMA_FLAGS)));
				d1(KPrintF("%s/%s/%ld:  gg->Width=%lu  gg->Height=%lu\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));
				d1(KPrintF("%s/%s/%ld:  use BltMaskBitMapRastPort   o=%08lx  inst=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, o, inst));

				d1(DumpMaskPlane(&inst->iobj_NormalMask));
				d1(DumpMaskPlane(&inst->iobj_SelectedMask));

				BltMaskBitMapRastPort(rp->BitMap,
					0, 0,
					iopd->iopd_RastPort,
					Left, Top,
                                        gg->Width, gg->Height,
					ABC | ABNC | ANBC,
                                        MaskPlanePtr);
				}
			else
				{
				ASSERT_d1(gg->Width <  GetBitMapAttr(iopd->iopd_RastPort->BitMap, BMA_WIDTH));
				ASSERT_d1(gg->Height <  GetBitMapAttr(iopd->iopd_RastPort->BitMap, BMA_HEIGHT));

				d1(KPrintF("%s/%s/%ld:  use ClipBlit   o=%08lx  inst=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, o, inst));
				ClipBlit(rp, 0, 0,
					iopd->iopd_RastPort,
					Left, Top, gg->Width, gg->Height,
					ABC | ABNC);
				}
			}
		}

	if (rp && !(iopd->iopd_DrawFlags & IODRAWF_NoText))
		{
		d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
		if (inst->iobj_text)
			DrawIconText(cl, o, iopd);
		}

	ReleaseSemaphore(&inst->iobj_LayoutSemaphore);

	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *iopl)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld:  START o=%08lx <%s>  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->iobj_text, inst));
	TIMESTAMP_d1();

	ObtainSemaphore(&inst->iobj_LayoutSemaphore);

	d1(KPrintF("%s/%s/%ld:  iargb_ARGBimage  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		inst->iobj_NormalARGB.iargb_ARGBimage.argb_Width, \
		inst->iobj_NormalARGB.iargb_ARGBimage.argb_Height));
	d1(KPrintF("%s/%s/%ld:  iobj_layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_layoutflags));

	if (inst->iobj_UnscaledNakedWidth != inst->iobj_NakedWidth
		|| inst->iobj_UnscaledNakedHeight != inst->iobj_NakedHeight
		|| (inst->iobj_layoutflags & (IOBLAYOUTF_ScaleNormal | IOBLAYOUTF_ScaleSelected)))
		{
		// We need to scale the icon!
		d1(KPrintF("%s/%s/%ld:  UnscaledWidth=%lu  UnscaledHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_UnscaledWidth, inst->iobj_UnscaledHeight));
		d1(KPrintF("%s/%s/%ld:  ScaledWidth=%lu  ScaledHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_ScaledWidth, inst->iobj_ScaledHeight));

		DoMethod(o, IDTM_ScaleIcon,
			inst->iobj_NakedWidth, inst->iobj_NakedHeight,
			iopl->iopl_Flags);
		}

	TIMESTAMP_d1();

	d1(KPrintF("%s/%s/%ld:  iargb_ARGBimage  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		inst->iobj_NormalARGB.iargb_ARGBimage.argb_Width, \
		inst->iobj_NormalARGB.iargb_ARGBimage.argb_Height));

	LayoutIconSize(cl, o, iopl->iopl_RastPort);

	d1(KPrintF("%s/%s/%ld:  iargb_ARGBimage  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		inst->iobj_NormalARGB.iargb_ARGBimage.argb_Width, \
		inst->iobj_NormalARGB.iargb_ARGBimage.argb_Height));

	d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
	TIMESTAMP_d1();

	// If ARGB image data present, allocate auxiliary buffer for rendering
	if (inst->iobj_NormalARGB.iargb_ARGBimage.argb_ImageData)
		{
		d1(KPrintF("%s/%s/%ld:  iopl=%08lx  iopl_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, iopl, iopl->iopl_Flags));

		LayoutARGB(cl, o, &inst->iobj_NormalARGB);

		d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

		// If selected ARGB image data present, allocate auxiliary buffer for rendering
		if (inst->iobj_SelectedARGB.iargb_ARGBimage.argb_ImageData)
			{
			d1(KPrintF("%s/%s/%ld:  iopl=%08lx  iopl_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, iopl, iopl->iopl_Flags));

			LayoutARGB(cl, o, &inst->iobj_SelectedARGB);

			d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
			}
		}

	d1(KPrintF("%s/%s/%ld:  iopl=%08lx  iopl_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, iopl, iopl->iopl_Flags));
	TIMESTAMP_d1();

	if (!(inst->iobj_layoutflags & (IOBLAYOUTF_NormalImage | IOBLAYOUTF_SelectedImage))
		&& (iopl->iopl_Flags & (IOLAYOUTF_NormalImage | IOLAYOUTF_SelectedImage))
		&& (IDTA_BACKFILL_NONE != inst->iobj_BackfillPenNorm))
		{
		inst->iobj_BackfillRp = InitRast(cl, o, iopl, &inst->iobj_BackfillRastPort);
		inst->iobj_layoutflags |= IOBLAYOUTF_BackfillNormal | IOBLAYOUTF_BackfillSelected;
		}

	TIMESTAMP_d1();

	if (!(inst->iobj_layoutflags & IOBLAYOUTF_NormalImage)
		&& (iopl->iopl_Flags & IOLAYOUTF_NormalImage)
		&& NULL == gg->GadgetRender)
		{
		gg->GadgetRender = InitRast(cl, o, iopl, &inst->iobj_RenderRastPort);

		d1(KPrintF("%s/%s/%ld:  IOBLAYOUTF_NormalImage GadgetRender=%08lx\n", __FILE__, __FUNC__, __LINE__, gg->GadgetRender));
		TIMESTAMP_d1();

		if (gg->GadgetRender)
			{
			struct RastPort *rp = (struct RastPort *) gg->GadgetRender;
			ULONG BackfillPen = (gg->Flags & GFLG_SELECTED) ? inst->iobj_BackfillPenSel : inst->iobj_BackfillPenNorm;

			if (!(inst->iobj_layoutflags & IOBLAYOUTF_ScaleNormal))
				{
				d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
				DrawFrame(cl, o, rp,
					inst->iobj_UnscaledWidth,
					inst->iobj_UnscaledHeight,
					inst->iobj_frametype);
				}

			inst->iobj_layoutflags |= IOBLAYOUTF_NormalImage;
			TIMESTAMP_d1();

			d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  BackfillPen=%ld\n", __FILE__, __FUNC__, __LINE__, o, inst, BackfillPen));
			if (IDTA_BACKFILL_NONE != BackfillPen)
				{
				SetAPen(rp, BackfillPen);
				RectFill(rp,
					inst->iobj_imgleft, inst->iobj_imgtop,
					gg->Width - inst->iobj_imgleft - inst->iobj_imgright - 1,
					gg->Height - inst->iobj_imgtop - inst->iobj_imgbottom - 1);
				}

			TIMESTAMP_d1();

			// mask bitmap already generated?
			if (NULL == inst->iobj_NormalMask.iom_MaskBM)
				{
				d1(KPrintF("%s/%s/%ld:  NormMaskWidth=%lu  NormMaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, \
					inst->iobj_NormalMask.iom_Width, inst->iobj_NormalMask.iom_Height));
				d1(KPrintF("%s/%s/%ld:  iobj_NakedMaskWidth=%ld  iobj_NakedMaskHeight=%ld\n", \
					__FILE__, __FUNC__, __LINE__, inst->iobj_NakedMaskWidth, inst->iobj_NakedMaskHeight));

				inst->iobj_NormalMask.iom_MaskBM = GenMask(cl, o,
					GetBitMapAttr(rp->BitMap, BMA_WIDTH),
					GetBitMapAttr(rp->BitMap, BMA_HEIGHT),
					GetBitMapAttr(rp->BitMap, BMA_DEPTH),
					GetBitMapAttr(rp->BitMap, BMA_FLAGS),
                                        inst->iobj_NormalMask.iom_Mask,
					(inst->iobj_layoutflags & IOBLAYOUTF_ScaleNormal) ? FRAMETYPE_NONE : inst->iobj_frametype,
					inst->iobj_NormalMask.iom_Width,
                                        inst->iobj_NormalMask.iom_Height);

				d1(KPrintF("%s/%s/%ld:  iom_Width=%ld  iom_Height=%ld\n", \
					__FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_Width, inst->iobj_NormalMask.iom_Height));

				d1(DumpMaskBM(&inst->iobj_NormalMask));
				}
			TIMESTAMP_d1();
			if (IDTA_BACKFILL_NONE != BackfillPen)
				{
				inst->iobj_NormalMask.iom_MaskInvBM = GenInvMask(cl, o, inst->iobj_NormalMask.iom_MaskBM);
				}
			}
		}

	d1(KPrintF("%s/%s/%ld:  iopl=%08lx  iopl_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, iopl, iopl->iopl_Flags));
	TIMESTAMP_d1();

	if (!(inst->iobj_layoutflags & IOBLAYOUTF_SelectedImage)
		&& (iopl->iopl_Flags & IOLAYOUTF_SelectedImage)
		&& NULL == gg->SelectRender)
		{
		gg->SelectRender = InitRast(cl, o, iopl, &inst->iobj_SelectRastPort);

		d1(KPrintF("%s/%s/%ld:  IOBLAYOUTF_SelectedImage SelectRender=%08lx\n", __FILE__, __FUNC__, __LINE__, gg->SelectRender));

		if (gg->SelectRender)
			{
			struct RastPort *rp = (struct RastPort *) gg->SelectRender;
			ULONG BackfillPen = (gg->Flags & GFLG_SELECTED) ? inst->iobj_BackfillPenSel : inst->iobj_BackfillPenNorm;

			if (!(inst->iobj_layoutflags & IOBLAYOUTF_ScaleSelected))
				{
				d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
				DrawFrame(cl, o, rp,
					inst->iobj_UnscaledWidth,
					inst->iobj_UnscaledHeight,
					inst->iobj_frametypesel);
				}

			inst->iobj_layoutflags |= IOBLAYOUTF_SelectedImage;

			if (IDTA_BACKFILL_NONE != BackfillPen)
				{
				SetAPen(rp, BackfillPen);
				RectFill(rp,
					inst->iobj_imgleft, inst->iobj_imgtop,
					gg->Width - inst->iobj_imgleft - inst->iobj_imgright - 1,
					gg->Height - inst->iobj_imgtop - inst->iobj_imgbottom - 1);
				}

			// mask bitmap already generated?
			if (NULL == inst->iobj_SelectedMask.iom_MaskBM)
				{
				d1(KPrintF("%s/%s/%ld:  SelMaskWidth=%lu  SelMaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_Width, inst->iobj_SelectedMask.iom_Height));

				inst->iobj_SelectedMask.iom_MaskBM = GenMask(cl, o,
					GetBitMapAttr(rp->BitMap, BMA_WIDTH),
					GetBitMapAttr(rp->BitMap, BMA_HEIGHT),
					GetBitMapAttr(rp->BitMap, BMA_DEPTH),
					GetBitMapAttr(rp->BitMap, BMA_FLAGS),
					inst->iobj_SelectedMask.iom_Mask,
					(inst->iobj_layoutflags & IOBLAYOUTF_ScaleSelected) ? FRAMETYPE_NONE : inst->iobj_frametypesel,
					inst->iobj_SelectedMask.iom_Width,
					inst->iobj_SelectedMask.iom_Height);

				d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  iobj_selmask=%08lx  iobj_selmaskbm=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, o, inst, inst->iobj_SelectedMask.iom_Mask, inst->iobj_SelectedMask.iom_MaskBM));
				}
			if (IDTA_BACKFILL_NONE != BackfillPen)
				{
				inst->iobj_SelectedMask.iom_MaskInvBM = GenInvMask(cl, o, inst->iobj_SelectedMask.iom_MaskBM);
				}
			}
		d1(KPrintF("%s/%s/%ld:  iobj_selmaskbm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskBM));
		}

	ReleaseSemaphore(&inst->iobj_LayoutSemaphore);

	d1(KPrintF("%s/%s/%ld:  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_layoutflags));
	d1(KPrintF("%s/%s/%ld:  END.\n", __FILE__, __FUNC__, __LINE__));
	TIMESTAMP_d1();

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  <%s>  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->iobj_text, inst));

	ObtainSemaphore(&inst->iobj_LayoutSemaphore);

	FreeARGBImageLayout(&inst->iobj_NormalARGB);
	FreeARGBImageLayout(&inst->iobj_SelectedARGB);

	if (inst->iobj_BackfillRp)
		{
		if (inst->iobj_BackfillRp->BitMap)
			{
			d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_BackfillRp->BitMap));
			FreeBitMap(inst->iobj_BackfillRp->BitMap);
			inst->iobj_BackfillRp->BitMap = NULL;
			}

		inst->iobj_BackfillRp = NULL;
		}
	if (gg->SelectRender)
		{
		struct RastPort *rp = (struct RastPort *) gg->SelectRender;

		if (rp->BitMap)
			{
			d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, rp->BitMap));
			FreeBitMap(rp->BitMap);
			rp->BitMap = NULL;
			}

		gg->SelectRender = NULL;
		}
	if (gg->GadgetRender)
		{
		struct RastPort *rp = (struct RastPort *) gg->GadgetRender;

		if (rp->BitMap)
			{
			d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, rp->BitMap));
			FreeBitMap(rp->BitMap);
			rp->BitMap = NULL;
			}
		gg->GadgetRender = NULL;
		}
	if (inst->iobj_NormalMask.iom_MaskBM)
		{
		d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_MaskBM));
		FreeBitMap(inst->iobj_NormalMask.iom_MaskBM);
		inst->iobj_NormalMask.iom_MaskBM = NULL;
		}
	if (inst->iobj_SelectedMask.iom_MaskBM)
		{
		d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskBM));
		FreeBitMap(inst->iobj_SelectedMask.iom_MaskBM);
		inst->iobj_SelectedMask.iom_MaskBM = NULL;
		}
	if (inst->iobj_NormalMask.iom_MaskInvBM)
		{
		d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_MaskInvBM));
		FreeBitMap(inst->iobj_NormalMask.iom_MaskInvBM);
		inst->iobj_NormalMask.iom_MaskInvBM = NULL;
		}
	if (inst->iobj_SelectedMask.iom_MaskInvBM)
		{
		d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskInvBM));
		FreeBitMap(inst->iobj_SelectedMask.iom_MaskInvBM);
		inst->iobj_SelectedMask.iom_MaskInvBM = NULL;
		}

	// Remove borders
	gg->Width = inst->iobj_NakedWidth;
	gg->Height = inst->iobj_NakedHeight;

	inst->iobj_UnscaledWidth = inst->iobj_UnscaledNakedWidth;
	inst->iobj_UnscaledHeight = inst->iobj_UnscaledNakedHeight;

	d1(KPrintF("%s/%s/%ld:  iobj_NakedMaskWidth=%ld  iobj_NakedMaskHeight=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_NakedMaskWidth, inst->iobj_NakedMaskHeight));
	d1(KPrintF("%s/%s/%ld:  iobj_imgleft=%ld  iobj_imgright=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_imgleft, inst->iobj_imgright));
	d1(KPrintF("%s/%s/%ld:  iobj_imgtop=%ld  iobj_imgbottom=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_imgtop, inst->iobj_imgbottom));

	inst->iobj_layoutflags &= (IOBLAYOUTF_ScaleNormal | IOBLAYOUTF_ScaleSelected);

	d1(KPrintF("%s/%s/%ld:  END layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_layoutflags));

	ReleaseSemaphore(&inst->iobj_LayoutSemaphore);

	return DoSuperMethodA(cl, o, (Msg) opf);
}

//----------------------------------------------------------------------------------------

static ULONG DtSet(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG Result;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, INST_DATA(cl, o)));

	// Make sure we have no pending layout when settings are changed!
	ObtainSemaphore(&inst->iobj_LayoutSemaphore);

	Result = DoSuperMethodA(cl, o, (Msg) ops);

	SetTags(cl, o, ops);

	ReleaseSemaphore(&inst->iobj_LayoutSemaphore);

	d1(KPrintF("%s/%s/%ld:  END  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, INST_DATA(cl, o)));

	return Result;
}

//----------------------------------------------------------------------------------------

static ULONG DtGet(Class *cl, Object *o, struct opGet *opg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG result = 1;
	struct TagItem AttrList[2];

	AttrList[0].ti_Tag = opg->opg_AttrID;
	AttrList[0].ti_Data = (ULONG) opg->opg_Storage;
	AttrList[1].ti_Tag = TAG_END;
	UnpackStructureTags(inst, packTable, AttrList);

	switch (opg->opg_AttrID)
		{
	case IDTA_Text:
		*opg->opg_Storage = (ULONG) inst->iobj_text;
		break;

	case IDTA_LayoutFlags:
		*opg->opg_Storage = inst->iobj_layoutflags;
		break;

	case IDTA_DefaultTool:
		*opg->opg_Storage = (ULONG) inst->iobj_defaulttool;
		break;

	case IDTA_ToolWindow:
		*opg->opg_Storage = (ULONG) inst->iobj_ToolWindow;
		break;

	case IDTA_Mask_Normal:
		*opg->opg_Storage = (ULONG) ((inst->iobj_NormalMask.iom_MaskBM && (IDTA_BACKFILL_NONE == inst->iobj_BackfillPenNorm))
			? inst->iobj_NormalMask.iom_MaskBM->Planes[0] : NULL);
		break;

	case IDTA_Mask_Selected:
		*opg->opg_Storage = (ULONG) ((inst->iobj_SelectedMask.iom_MaskBM && (IDTA_BACKFILL_NONE == inst->iobj_BackfillPenSel))
			? inst->iobj_SelectedMask.iom_MaskBM->Planes[0] : NULL);
		break;

	case IDTA_MaskBM_Normal:
		*opg->opg_Storage = (ULONG) ((IDTA_BACKFILL_NONE == inst->iobj_BackfillPenNorm) ? inst->iobj_NormalMask.iom_MaskBM : NULL);
		break;

	case IDTA_MaskBM_Selected:
		*opg->opg_Storage = (ULONG) ((IDTA_BACKFILL_NONE == inst->iobj_BackfillPenSel) ? inst->iobj_SelectedMask.iom_MaskBM : NULL);
		break;

	case IDTA_AlphaChannel:
		*opg->opg_Storage = (ULONG) inst->iobj_NormalARGB.iargb_AlphaChannel;
		break;

	case IDTA_SelAlphaChannel:
		*opg->opg_Storage = (ULONG) inst->iobj_SelectedARGB.iargb_AlphaChannel;
		break;

	case IDTA_ARGBImageData:
		*opg->opg_Storage = (ULONG) &inst->iobj_NormalARGB.iargb_ARGBimage;
		d1(KPrintF("%s/%s/%ld: argbh=%08lx  argb_ImageData=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, &inst->iobj_NormalARGB.iargb_ARGBimage,
			inst->iobj_NormalARGB.iargb_ARGBimage.argb_ImageData));
		break;

	case IDTA_SelARGBImageData:
		*opg->opg_Storage = (ULONG) &inst->iobj_SelectedARGB.iargb_ARGBimage;
		d1(KPrintF("%s/%s/%ld: argbh=%08lx  argb_ImageData=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, &inst->iobj_SelectedARGB.iargb_ARGBimage,
			inst->iobj_SelectedARGB.iargb_ARGBimage.argb_ImageData));
		break;

	case IDTA_ToolTypes:
		*opg->opg_Storage = (ULONG) inst->iobj_tooltypes;
		break;

	case DTA_Name:
		*opg->opg_Storage = (ULONG) inst->iobj_name;
		break;

	case IDTA_UnscaledWidth:
		*opg->opg_Storage = (ULONG) inst->iobj_UnscaledWidth;
		break;

	case IDTA_UnscaledHeight:
		*opg->opg_Storage = (ULONG) inst->iobj_UnscaledHeight;
		break;

	case IDTA_WindowRect:
		switch (inst->iobj_type)
			{
		case WBDISK:
		case WBDRAWER:
		case WBGARBAGE:
			d1(kprintf("%s/%s/%ld: IDTA_WindowRect  <%s> Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
				inst->iobj_text, inst->iobj_winrect.Left, inst->iobj_winrect.Top, \
				inst->iobj_winrect.Width, inst->iobj_winrect.Height));

			if (0 != inst->iobj_winrect.Width && 0 != inst->iobj_winrect.Height)
				*opg->opg_Storage = (ULONG) &inst->iobj_winrect;
			else
				*opg->opg_Storage = (ULONG) NULL;
			break;
		default:
			result = 0;
			break;
			}
		break;

	default:
		result = DoSuperMethodA(cl, o, (Msg) opg);
		break;
		}

	return result;
}

//----------------------------------------------------------------------------------------

static ULONG DtScaleIcon(Class *cl, Object *o, struct iopScaleIcon *iops)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG NewWidth, NewHeight;
	ULONG Success;
	struct Hook *RenderHook = NULL;

	d1(KPrintF("%s/%s/%ld:  START o=%08lx  <%s> inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->iobj_name, inst));

	GetAttr(IDTA_RenderHook, o, (APTR) &RenderHook);
	if (RenderHook)
		return FALSE;		// No scaling with RenderHook

	NewWidth = iops->iops_NewWidth;
	NewHeight = iops->iops_NewHeight;

	d1(KPrintF("%s/%s/%ld:  NewWidth=%lu  NewHeight=%lu\n", __FILE__, __FUNC__, __LINE__, NewWidth, NewHeight));

	ObtainSemaphore(&inst->iobj_LayoutSemaphore);

	if (inst->iobj_NormalARGB.iargb_ARGBimage.argb_ImageData)
		{
		// Scale ARGB icon
		ULONG ScaleFlags = SCALEFLAGF_DOUBLESIZE | SCALEFLAGF_BILINEAR
				| SCALEFLAGF_AVERAGE;

		Success = ScaleARGB(cl, o, &inst->iobj_NormalARGB, &inst->iobj_NormalMask,
			NewWidth, NewHeight, ScaleFlags);

		if (Success && inst->iobj_SelectedARGB.iargb_ARGBimage.argb_ImageData)
			{
			ScaleARGB(cl, o, &inst->iobj_SelectedARGB,
				&inst->iobj_SelectedMask, NewWidth, NewHeight, ScaleFlags);
			}

		// Mask and Alpha are automatically recreated during IDTM_Layout

		if (Success)
			{
			// Mask will be generated in scaled size
			inst->iobj_NakedMaskWidth = NewWidth;
			inst->iobj_NakedMaskHeight = NewHeight;
			d1(KPrintF("%s/%s/%ld:  NormMaskWidth=%lu  NormMaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_Width, inst->iobj_NormalMask.iom_Height));
			}
		}
	else
		{
		// Scaling is postponed until IDTM_Draw
		d1(KPrintF("%s/%s/%ld:  Scaling is postponed until IDTM_Draw\n", __FILE__, __FUNC__, __LINE__));

		Success = TRUE;

		if (iops->iops_Flags & IOLAYOUTF_NormalImage)
			inst->iobj_layoutflags |= IOBLAYOUTF_ScaleNormal;
		if (iops->iops_Flags & IOLAYOUTF_SelectedImage)
			inst->iobj_layoutflags |= IOBLAYOUTF_ScaleSelected;
		}

	if (Success)
		{
		inst->iobj_NakedWidth = NewWidth;
		inst->iobj_NakedHeight = NewHeight;

		inst->iobj_layoutflags &= ~IOBLAYOUTF_Size;
		}

	ReleaseSemaphore(&inst->iobj_LayoutSemaphore);

	d1(KPrintF("%s/%s/%ld:  END  Success=%ld  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, Success, inst->iobj_layoutflags));

	return Success;
}

//----------------------------------------------------------------------------------------

static ULONG DoOmSet(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	PackStructureTags(inst, packTable, ops->ops_AttrList);

	return 0;
}

//----------------------------------------------------------------------------------------

static void LayoutIconText(struct InstanceData *inst, struct RastPort *rp, struct ExtGadget *gg)
{
	TIMESTAMP_d1();
	MySetFont(inst, rp, inst->iobj_Font);

	inst->iobj_FontBaseLine = MyGetFontBaseline(inst, rp);

	TIMESTAMP_d1();
	MySetSoftStyle(inst, rp,
		inst->iobj_TextStyle, FSF_ITALIC | FSF_UNDERLINED | FSF_BOLD | FSF_EXTENDED);

	TIMESTAMP_d1();
	DoReLayout(inst, gg, rp);

	TIMESTAMP_d1();
	MyDoneRastPort(inst, rp);

	TIMESTAMP_d1();
}

//----------------------------------------------------------------------------------------

static void DrawIconText(Class *cl, Object *o, struct iopDraw *iopd)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	WORD x, y;
	WORD TextX, TextY;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ObtainSemaphoreShared(&inst->iobj_TextSemaphore);

	GetPosition(iopd, gg, &x, &y);

	MySetFont(inst, iopd->iopd_RastPort, inst->iobj_Font);

	MySetSoftStyle(inst, iopd->iopd_RastPort, 
		inst->iobj_TextStyle, FSF_ITALIC | FSF_UNDERLINED | FSF_BOLD | FSF_EXTENDED);

	TextX = x - inst->iobj_GlobalTextLeftOffset + 1;
	TextY = y + gg->Height + inst->iobj_textskip + inst->iobj_FontBaseLine;

	if (inst->iobj_SelectedTextRectangle)
		{
		TextY += inst->iobj_TextRectBorderY;
		//TextX += inst->iobj_TextRectBorderX;
		}

	if (gg->Flags & GFLG_SELECTED)
		{
		DrawIconTextRect(cl, o, iopd->iopd_RastPort, x, y);
		}
	else
		{
		EraseIconText(cl, o, iopd->iopd_RastPort, x, y);
		}

#if 0
	{
	// TEST: Draw frame around image part
	WORD x0, y0;
	WORD x1, y1;

	y0 = y;
	y1 = y0 + gg->Height - 1;
	x0 = x;
	x1 = x0 + gg->Width - 1;

	Move(iopd->iopd_RastPort, x0, y0);
	Draw(iopd->iopd_RastPort, x0, y1);
	Draw(iopd->iopd_RastPort, x1, y1);
	Draw(iopd->iopd_RastPort, x1, y0);
	Draw(iopd->iopd_RastPort, x0, y0);
	}
#endif

#if 0
	{
	// TEST: Draw frame around entire icon
	WORD x0, y0;
	WORD x1, y1;

	y0 = y;
	y1 = y0 + gg->BoundsHeight;
	x0 = x + (gg->BoundsLeftEdge - gg->LeftEdge);
	x1 = x0 + gg->BoundsWidth;

	SetAPen(iopd->iopd_RastPort, 1);

	Move(iopd->iopd_RastPort, x0, y0);
	Draw(iopd->iopd_RastPort, x0, y1);
	Draw(iopd->iopd_RastPort, x1, y1);
	Draw(iopd->iopd_RastPort, x1, y0);
	Draw(iopd->iopd_RastPort, x0, y0);
	}
#endif

#if 0
	{
	// TEST: Draw frame around text part
	WORD x0, y0;
	WORD x1, y1;

	SetAPen(iopd->iopd_RastPort, 1);

	y0 = TextY - inst->iobj_FontBaseLine;
	y1 = y0 + inst->iobj_TextExtent.te_Height;
	x0 = TextX;
	if (inst->iobj_SelectedTextRectangle)
		x0 += inst->iobj_TextRectBorderX;
	x1 = x0 + inst->iobj_TextExtent.te_Width;

	d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, x1 - x0, y1 - y0));

	Move(iopd->iopd_RastPort, x0, y0);
	Draw(iopd->iopd_RastPort, x0, y1);
	Draw(iopd->iopd_RastPort, x1, y1);
	Draw(iopd->iopd_RastPort, x1, y0);
	Draw(iopd->iopd_RastPort, x0, y0);
	}
#endif
	if (IDTV_TextMode_Outline == inst->iobj_textmode)
		{
		// draw outline text
		ULONG OrigDrMd;

		OrigDrMd = GetDrMd(iopd->iopd_RastPort);

		MySetTransparency(inst, iopd->iopd_RastPort, 255 / 2);	// 50% transparent
		SetAPen(iopd->iopd_RastPort, inst->iobj_TextPenOutline);

		DrawTextAt(inst, iopd->iopd_RastPort, TextX, TextY + 1);
		DrawTextAt(inst, iopd->iopd_RastPort, TextX + 1, TextY);
		DrawTextAt(inst, iopd->iopd_RastPort, TextX + 1, TextY + 2);
		DrawTextAt(inst, iopd->iopd_RastPort, TextX + 2, TextY + 1);
		DrawTextAt(inst, iopd->iopd_RastPort, TextX, TextY);

		SetAPen(iopd->iopd_RastPort, 
			(gg->Flags & GFLG_SELECTED) ? inst->iobj_TextPenSel : inst->iobj_TextPen);

		MySetTransparency(inst, iopd->iopd_RastPort, 0);	// opaque
		SetDrMd(iopd->iopd_RastPort, JAM1);

		DrawTextAt(inst, iopd->iopd_RastPort, TextX + 1, TextY + 1);

		SetDrMd(iopd->iopd_RastPort, OrigDrMd);
		}
	else if (IDTV_TextMode_Shadow == inst->iobj_textmode)
		{
		// draw shadow
		ULONG OrigDrMd;

		OrigDrMd = GetDrMd(iopd->iopd_RastPort);

		SetAPen(iopd->iopd_RastPort, inst->iobj_TextPenShadow);
		MySetTransparency(inst, iopd->iopd_RastPort, 255 / 2);	// 50% transparent

		DrawTextAt(inst, iopd->iopd_RastPort, TextX + 1, TextY + 1);

		SetAPen(iopd->iopd_RastPort, 
			(gg->Flags & GFLG_SELECTED) ? inst->iobj_TextPenSel : inst->iobj_TextPen);

		SetDrMd(iopd->iopd_RastPort, JAM1);
		MySetTransparency(inst, iopd->iopd_RastPort, 0);	// opaque

		DrawTextAt(inst, iopd->iopd_RastPort, TextX, TextY);

		SetDrMd(iopd->iopd_RastPort, OrigDrMd);
		}
	else
		{
		// standard icon text

		SetABPenDrMd(iopd->iopd_RastPort,
			(gg->Flags & GFLG_SELECTED) ? inst->iobj_TextPenSel : inst->iobj_TextPen,
			inst->iobj_TextBackPen,
			inst->iobj_TextDrawMode);
		DrawTextAt(inst, iopd->iopd_RastPort, TextX, TextY);
		}

	MyDoneRastPort(inst, iopd->iopd_RastPort);

	ReleaseSemaphore(&inst->iobj_TextSemaphore);
}

//----------------------------------------------------------------------------------------

static void GetPosition(struct iopDraw *iopd, struct ExtGadget *gg, WORD *x, WORD *y)
{
	*x = iopd->iopd_XOffset;
	*y = iopd->iopd_YOffset;

	if (iopd->iopd_DrawFlags & IODRAWF_AbsolutePos)
		{
		if (!(iopd->iopd_DrawFlags & IODRAWF_NoText))
			{
			*x += gg->LeftEdge - gg->BoundsLeftEdge;
			*y += gg->TopEdge - gg->BoundsTopEdge;
			}
		}
	else
		{
		*x += gg->LeftEdge;
		*y += gg->TopEdge;

		if (iopd->iopd_DrawFlags & IODRAWF_NoText)
			{
			*x -= gg->LeftEdge - gg->BoundsLeftEdge;
			*y -= gg->TopEdge - gg->BoundsTopEdge;
			}
		}
}

//----------------------------------------------------------------------------------------

static void DrawTextAt(struct InstanceData *inst, struct RastPort *rp, ULONG x, ULONG y)
{
	struct IconObjectTextLine *iotl;
	ULONG n;

	for (n = 0; n < Sizeof(inst->iobj_TextLines); n++)
		{
		iotl = &inst->iobj_TextLines[n];

		if (0 == iotl->iotl_numchars)
			break;

		Move(rp, x + iotl->iotl_TextLeft, y);
		MyText(inst, rp, inst->iobj_text + iotl->iotl_TextStart, iotl->iotl_numchars);
		y += iotl->iotl_TextExtent.te_Height;
		}
}

//----------------------------------------------------------------------------------------

static void DoReLayout(struct InstanceData *inst, struct ExtGadget *gg, struct RastPort *rp)
{
	ULONG n;

	d1(kprintf("%s/%s/%ld: iobj_text=<%s>  %08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_text, inst->iobj_text));

	ObtainSemaphoreShared(&inst->iobj_TextSemaphore);

	memset(inst->iobj_TextLines, 0, sizeof(inst->iobj_TextLines));
	memset(&inst->iobj_TextExtent, 0, sizeof(inst->iobj_TextExtent));

	inst->iobj_numchars = strlen(inst->iobj_text);

	d1(kprintf("%s/%s/%ld: iobj_text=<%s>  iobj_numchars=%lu\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_text, inst->iobj_numchars));

	for (n = 0; n < Sizeof(inst->iobj_TextLines); n++)
		{
		struct IconObjectTextLine *iotl;

		iotl = &inst->iobj_TextLines[n];

		iotl->iotl_TextStart = 0;
		iotl->iotl_numchars = 0;
		}

	inst->iobj_TextLines[0].iotl_numchars = inst->iobj_numchars;
	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	CalculateTextDimensions(inst, gg, rp);

	d1(kprintf("%s/%s/%ld: iobj_text=<%s>  %08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_text, inst->iobj_text));
	d1(kprintf("%s/%s/%ld: Extent.w=%ld  Extent.h=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_TextExtent.te_Width, inst->iobj_TextExtent.te_Height));

	if (inst->iobj_MultiLineText)
		{
		ULONG MaxUsedLineIndex = 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (2 * gg->BoundsWidth > 3 * gg->Width)
			{
			struct IconObjectTextLine *iotl;
			BOOL Split = FALSE;
			LONG n1, n2;

			iotl = &inst->iobj_TextLines[MaxUsedLineIndex];
			if (iotl->iotl_numchars > 2 * 3)
				{
				// Start at center of text, and check characters left and right of
				// center for suitable division point
				// Move check points farther and farther from center, until front or back
				// borders (3 characters left) are reached.
				for (n1 = n2 = iotl->iotl_numchars / 2; !Split
					&& (n1 > iotl->iotl_TextStart + 3 || n2 < iotl->iotl_numchars - 3); )
					{
					if ( IsSeparator(inst->iobj_text[n1]) )
						{
						inst->iobj_TextLines[MaxUsedLineIndex + 1].iotl_numchars = iotl->iotl_numchars - n1 - 1;
						iotl->iotl_numchars = n1 + 1;
						inst->iobj_TextLines[MaxUsedLineIndex + 1].iotl_TextStart = n1 + 1;
						Split = TRUE;
						}
					else if ( IsSeparator(inst->iobj_text[n2]) )
						{
						inst->iobj_TextLines[MaxUsedLineIndex + 1].iotl_numchars = iotl->iotl_numchars - n2 - 1;
						iotl->iotl_numchars = n2 + 1;
						inst->iobj_TextLines[MaxUsedLineIndex + 1].iotl_TextStart = n2 + 1;
						Split = TRUE;
						}

					d1(if (Split) kprintf("%s/%s/%ld: SplitChar=%02lx  n=%ld\n", \
						__FILE__, __FUNC__, __LINE__, inst->iobj_text[n], n));

					if (n1 > iotl->iotl_TextStart + 3)
						n1--;	// move check point 1 left towards start of text
					if (n2 < iotl->iotl_numchars - 3)
						n2++;	// move check point 2 right towards end of text
					}

				d1(kprintf("%s/%s/%ld: iobj_text=<%s>  %08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_text, inst->iobj_text));
				d1(kprintf("%s/%s/%ld: Split=%ld  MaxLine=%ld  numchars=%ld  TextStart=%ld\n", \
					__FILE__, __FUNC__, __LINE__, Split, \
					MaxUsedLineIndex, inst->iobj_TextLines[MaxUsedLineIndex + 1].iotl_numchars,\
					inst->iobj_TextLines[MaxUsedLineIndex + 1].iotl_TextStart));

				if (!Split && gg->BoundsWidth > 2 * gg->Width)
					{
					// No suitable division point found,
					// and text more than twice as wide as icon.
					// force division at arithmetic center of text (half of length).
					n = iotl->iotl_numchars / 2;

					inst->iobj_TextLines[MaxUsedLineIndex + 1].iotl_numchars = iotl->iotl_numchars - n - 1;
					iotl->iotl_numchars = n + 1;
					inst->iobj_TextLines[MaxUsedLineIndex + 1].iotl_TextStart = n + 1;
					Split = TRUE;
					}

				if (Split)
					CalculateTextDimensions(inst, gg, rp);
				}

			d1(kprintf("%s/%s/%ld: Extent.w=%ld  Extent.h=%ld\n", \
				__FILE__, __FUNC__, __LINE__, inst->iobj_TextExtent.te_Width, inst->iobj_TextExtent.te_Height));
			}
		}

	inst->iobj_GlobalTextLeftOffset = inst->iobj_TextExtent.te_Width - gg->Width;

	d1(kprintf("%s/%s/%ld: Width=%ld  BoundsWidth=%ld  Left=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gg->Width, gg->BoundsWidth, inst->iobj_GlobalTextLeftOffset));

	inst->iobj_GlobalTextLeftOffset /= 2;

	if (inst->iobj_SelectedTextRectangle)
		inst->iobj_GlobalTextLeftOffset += inst->iobj_TextRectBorderX;

	if (inst->iobj_GlobalTextLeftOffset > 0)
		{
		// Text is wider than image
		gg->BoundsLeftEdge -= inst->iobj_GlobalTextLeftOffset;
		}
	else
		{
		// Image is wider than text
		struct IconObjectTextLine *iotl;
		ULONG n;

		for (n = 0; n < Sizeof(inst->iobj_TextLines); n++)
			{
			iotl = &inst->iobj_TextLines[n];

			iotl->iotl_TextLeft += inst->iobj_GlobalTextLeftOffset;
			}
		}

	d1(kprintf("%s/%s/%ld: iobj_text=<%s>  %08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_text, inst->iobj_text));

	ReleaseSemaphore(&inst->iobj_TextSemaphore);
}


static BOOL IsSeparator(char ch)
{
	return (BOOL) ('-' == ch || '_' == ch || '=' == ch || isspace(ch));
}

static ULONG MySetSoftStyle(struct InstanceData *inst, struct RastPort *rp, ULONG style, ULONG enable)
{
	if (inst->iobj_FontHook)
		{
		struct ioFontMsg_SetFontStyle iofsfs;

		iofsfs.MsgID = iofMsg_SETFONTSTYLE;
		iofsfs.iofsfs_RastPort = rp;
		iofsfs.iofsfs_FontHandle = inst->iobj_Fonthandle;
		iofsfs.iofsfs_Style = style;
		iofsfs.iofsfs_Enable = enable;

		return (ULONG) CallHookPkt(inst->iobj_FontHook, NULL, &iofsfs);
		}
	else
		{
		return SetSoftStyle(rp, style, enable);
		}
}

static void MyText(struct InstanceData *inst, struct RastPort *rp, CONST_STRPTR string, WORD Length)
{
	if (inst->iobj_FontHook)
		{
		struct ioFontMsg_Text ioft;

		ioft.MsgID = iofMsg_TEXT;
		ioft.ioft_RastPort = rp;
		ioft.ioft_FontHandle = inst->iobj_Fonthandle;
		ioft.ioft_String = string;
		ioft.ioft_Length = Length;

		CallHookPkt(inst->iobj_FontHook, NULL, &ioft);
		}
	else
		{
		Text(rp, string, Length);
		}
}

static void MyTextExtent(struct InstanceData *inst, struct RastPort *rp, CONST_STRPTR string, WORD Length, struct TextExtent *tExt)
{
	if (inst->iobj_FontHook)
		{
		struct ioFontMsg_TextExtent iofte;

		iofte.MsgID = iofMsg_TEXTEXTENT;
		iofte.iofte_RastPort = rp;
		iofte.iofte_FontHandle = inst->iobj_Fonthandle;
		iofte.iofte_String = string;
		iofte.iofte_Length = Length;
		iofte.iofte_TextExtent = tExt;

		CallHookPkt(inst->iobj_FontHook, NULL, &iofte);
		}
	else
		{
		TextExtent(rp, string, Length, tExt);
		}
}

static void MySetFont(struct InstanceData *inst, struct RastPort *rp, struct TextFont *tf)
{
	if (inst->iobj_FontHook)
		{
		struct ioFontMsg_SetFont iofsf;

		iofsf.MsgID = iofMsg_SETFONT;
		iofsf.iofsf_RastPort = rp;
		iofsf.iofsf_FontHandle = inst->iobj_Fonthandle;
		iofsf.iofsf_TextFont = tf;

		CallHookPkt(inst->iobj_FontHook, NULL, &iofsf);
		}
	else
		{
		if (tf)
			SetFont(rp, tf);
		}
}

static void MySetTransparency(struct InstanceData *inst, struct RastPort *rp, ULONG Transparency)
{
	if (inst->iobj_FontHook)
		{
		struct ioFontMsg_SetTransparency iofst;

		iofst.MsgID = iofMsg_SETTRANSPARENCY;
		iofst.iofst_RastPort = rp;
		iofst.iofst_FontHandle = inst->iobj_Fonthandle;
		iofst.iofst_Transparency = Transparency;

		CallHookPkt(inst->iobj_FontHook, NULL, &iofst);
		}
	else
		{
		// not supported for standard fonts
		}
}

static void MyDoneRastPort(struct InstanceData *inst, struct RastPort *rp)
{
	if (inst->iobj_FontHook)
		{
		struct ioFontMsg_DoneRastPort iofdr;

		iofdr.MsgID = iofMsg_DONERASTPORT;
		iofdr.iofdr_RastPort = rp;
		iofdr.iofdr_FontHandle = inst->iobj_Fonthandle;

		CallHookPkt(inst->iobj_FontHook, NULL, &iofdr);
		}
}

static UWORD MyGetFontBaseline(struct InstanceData *inst, struct RastPort *rp)
{
	if (inst->iobj_FontHook)
		{
		struct ioFontMsg_GetFontBaseline iofgfb;

		iofgfb.MsgID = iofMsg_GETFONTBASELINE;
		iofgfb.iofgfb_RastPort = rp;
		iofgfb.iofgfb_FontHandle = inst->iobj_Fonthandle;

		return (UWORD) CallHookPkt(inst->iobj_FontHook, NULL, &iofgfb);
		}
	else
		{
		return (UWORD) rp->Font->tf_Baseline;
		}
}

//-----------------------------------------------------------------------------

static void CalculateTextDimensions(struct InstanceData *inst, struct ExtGadget *gg, struct RastPort *rp)
{
	ULONG n;

	gg->BoundsWidth = gg->Width;
	gg->BoundsHeight = gg->Height;

	if (inst->iobj_SelectedTextRectangle)
		gg->BoundsHeight += 2 * TT_BORDER_Y;

	inst->iobj_TextExtent.te_Width = inst->iobj_TextExtent.te_Height = 0;

	for (n = 0; n < Sizeof(inst->iobj_TextLines); n++)
		{
		struct IconObjectTextLine *iotl;

		iotl = &inst->iobj_TextLines[n];

		if (iotl->iotl_numchars)
			{
			d1(KPrintF("%s/%s/%ld: text=<%s>  TextStart=%ld  numchars=%ld\n", \
				__FILE__, __FUNC__, __LINE__, inst->iobj_text + iotl->iotl_TextStart, iotl->iotl_TextStart, iotl->iotl_numchars));

			MyTextExtent(inst, rp, inst->iobj_text + iotl->iotl_TextStart, 
				iotl->iotl_numchars, &iotl->iotl_TextExtent);

			iotl->iotl_TextWidth = 1 + iotl->iotl_TextExtent.te_Extent.MaxX - iotl->iotl_TextExtent.te_Extent.MinX;
			if (iotl->iotl_TextExtent.te_Width > iotl->iotl_TextWidth)
				iotl->iotl_TextWidth = iotl->iotl_TextExtent.te_Width;

			iotl->iotl_TextHeight = 1 + iotl->iotl_TextExtent.te_Extent.MaxY - iotl->iotl_TextExtent.te_Extent.MinY;
			if (iotl->iotl_TextExtent.te_Height > iotl->iotl_TextHeight)
				iotl->iotl_TextHeight = iotl->iotl_TextExtent.te_Height;

			iotl->iotl_TextHeight += 1;

			switch (inst->iobj_textmode)
				{
			case IDTV_TextMode_Outline:
				iotl->iotl_TextWidth += 2;
				iotl->iotl_TextHeight += 2;
				break;
			case IDTV_TextMode_Shadow:
				iotl->iotl_TextWidth += 1;
				iotl->iotl_TextHeight += 1;
				break;
			default:
				break;
				}

			inst->iobj_TextExtent.te_Width = max(inst->iobj_TextExtent.te_Width, 2 + iotl->iotl_TextExtent.te_Width);
			inst->iobj_TextExtent.te_Height += iotl->iotl_TextExtent.te_Height;

			d1(KPrintF("%s/%s/%ld: iotl_TextExtent: te_Width=%ld  te_Height=%ld  MinX=%ld  MaxX=%ld\n", \
				__FILE__, __FUNC__, __LINE__, iotl->iotl_TextExtent.te_Width, \
				iotl->iotl_TextExtent.te_Height,
				iotl->iotl_TextExtent.te_Extent.MinX,
                                iotl->iotl_TextExtent.te_Extent.MaxX));

			if (inst->iobj_SelectedTextRectangle)
				gg->BoundsWidth = max(gg->BoundsWidth, (2 * inst->iobj_TextRectBorderX + inst->iobj_TextExtent.te_Width + 1));
			else
				gg->BoundsWidth = max(gg->BoundsWidth, inst->iobj_TextExtent.te_Width);

			gg->BoundsHeight += iotl->iotl_TextHeight + inst->iobj_textskip;
			}
		}
	for (n = 0; n < Sizeof(inst->iobj_TextLines); n++)
		{
		struct IconObjectTextLine *iotl;

		iotl = &inst->iobj_TextLines[n];

		if (iotl->iotl_numchars)
			{
			// Center text inside <BoundsWidth>
			iotl->iotl_TextLeft = (1 + gg->BoundsWidth - iotl->iotl_TextWidth) / 2;

			d1(kprintf("%s/%s/%ld: w=%ld  h=%ld  Extent.w=%ld  Extent.h=%ld  Left=%ld\n", \
				__FILE__, __FUNC__, __LINE__, iotl->iotl_TextWidth, iotl->iotl_TextHeight, iotl->iotl_TextExtent.te_Width, \
				iotl->iotl_TextExtent.te_Height, iotl->iotl_TextLeft));
			}
		}
}

//-----------------------------------------------------------------------------

static BOOL ToolTypeNameCmp(CONST_STRPTR ToolTypeName, CONST_STRPTR ToolTypeContents)
{
#if 0
	while (*ToolTypeName && *ToolTypeContents
		&& '=' != *ToolTypeContents
		&& ToLower(*ToolTypeName) == ToLower(*ToolTypeContents))
		{
		ToolTypeName++;
		ToolTypeContents++;
		}
#else
	while (*ToolTypeName && *ToolTypeContents)
		{
		UBYTE ch1;	// Work-around for GCC compiler bug

		if ('=' == *ToolTypeContents)
			break;
		ch1 = ToLower(*ToolTypeName);
		if (ch1 != ToLower(*ToolTypeContents))
			break;

		ToolTypeName++;
		ToolTypeContents++;
		}
#endif

	return (BOOL) ('\0' == *ToolTypeName && ('\0' == *ToolTypeContents || '=' == *ToolTypeContents));
}

//-----------------------------------------------------------------------------

static void SetTags(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct TagItem *dTag;
	BOOL SizeSet = FALSE;
	BOOL NeedFreeLayout = FALSE;

	DoOmSet(cl, o, ops);

	SetAllocTagProtected(IDTA_Text, &inst->iobj_text, ops->ops_AttrList, &inst->iobj_TextSemaphore);
	SetAllocTag(IDTA_DefaultTool, &inst->iobj_defaulttool, ops->ops_AttrList);
	SetAllocTag(IDTA_ToolWindow, &inst->iobj_ToolWindow, ops->ops_AttrList);

	dTag = FindTagItem(IDTA_ARGBImageData, ops->ops_AttrList);
	if (dTag)
		{
		d1(KPrintF("%s/%s/%ld: o=%08lx  IDTA_ARGBImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, o, dTag->ti_Data));
		NeedFreeLayout = TRUE;
		ReplaceARGBImage(&inst->iobj_NormalARGB, (struct ARGBHeader *) dTag->ti_Data);
		}

	dTag = FindTagItem(IDTA_SelARGBImageData, ops->ops_AttrList);
	if (dTag)
		{
		d1(KPrintF("%s/%s/%ld: o=%08lx  IDTA_SelARGBImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, o, dTag->ti_Data));
		NeedFreeLayout = TRUE;
		ReplaceARGBImage(&inst->iobj_SelectedARGB, (struct ARGBHeader *) dTag->ti_Data);
		}

	dTag = FindTagItem(GA_Width, ops->ops_AttrList);
	if (dTag)
		{
		d1(KPrintF("%s/%s/%ld: o=%08lx  GA_Width=%ld\n", __FILE__, __FUNC__, __LINE__, o, dTag->ti_Data));
		SizeSet = TRUE;
		NeedFreeLayout = TRUE;
		gg->Width = inst->iobj_NakedMaskWidth
			= inst->iobj_NakedWidth
			= inst->iobj_ScaledWidth
                        = dTag->ti_Data;

		if (!(inst->iobj_layoutflags & (IOBLAYOUTF_ScaleNormal | IOBLAYOUTF_ScaleSelected)))
			{
			inst->iobj_UnscaledWidth = inst->iobj_UnscaledNakedWidth = dTag->ti_Data;
			}

		d1(KPrintF("%s/%s/%ld: o=%08lx  Width=%lu\n", __FILE__, __FUNC__, __LINE__, o, gg->Width));
		}
	dTag = FindTagItem(GA_Height, ops->ops_AttrList);
	if (dTag)
		{
		SizeSet = TRUE;
		NeedFreeLayout = TRUE;
		gg->Height = inst->iobj_NakedMaskHeight
			= inst->iobj_NakedHeight
			= inst->iobj_ScaledHeight
                        = dTag->ti_Data;

		if (!(inst->iobj_layoutflags & (IOBLAYOUTF_ScaleNormal | IOBLAYOUTF_ScaleSelected)))
			{
			inst->iobj_UnscaledHeight = inst->iobj_UnscaledNakedHeight = dTag->ti_Data;
			}

		d1(KPrintF("%s/%s/%ld: o=%08lx  Height=%lu\n", __FILE__, __FUNC__, __LINE__, o, gg->Height));
		}

	dTag = FindTagItem(IDTA_UnscaledWidth, ops->ops_AttrList);
	if (dTag)
		{
		inst->iobj_UnscaledWidth = inst->iobj_UnscaledNakedWidth = dTag->ti_Data;
		NeedFreeLayout = TRUE;
		}

	dTag = FindTagItem(IDTA_UnscaledHeight, ops->ops_AttrList);
	if (dTag)
		{
		inst->iobj_UnscaledHeight = inst->iobj_UnscaledNakedHeight = dTag->ti_Data;
		NeedFreeLayout = TRUE;
		}

	dTag = FindTagItem(IDTA_ToolTypes, ops->ops_AttrList);
	if (dTag)
		SetToolTypes(inst, (const STRPTR *) dTag->ti_Data);

	SetAllocTag(DTA_Name, &inst->iobj_name, ops->ops_AttrList);

	dTag = FindTagItem(IDTA_ScalePercentage, ops->ops_AttrList);
	if (dTag)
		{
		inst->iobj_ScalingPercentage = dTag->ti_Data;

		if (inst->iobj_ScalingPercentage < IDTA_ScalePercentage_MIN)
			inst->iobj_ScalingPercentage = IDTA_ScalePercentage_MIN;
		else if (inst->iobj_ScalingPercentage > IDTA_ScalePercentage_MAX)
			inst->iobj_ScalingPercentage = IDTA_ScalePercentage_MAX;

		d1(KPrintF("%s/%s/%ld: IDTA_ScalePercentage: %ld\n", \
			__FILE__, __FUNC__, __LINE__, inst->iobj_ScalingPercentage));
		}

	dTag = FindTagItem(IDTA_SizeConstraints, ops->ops_AttrList);
	if (dTag)
		{
		struct Rectangle *NewConstraints = (struct Rectangle *) dTag->ti_Data;

		inst->iobj_SizeConstraints = *NewConstraints;
		d1(KPrintF("%s/%s/%ld: iobj_SizeConstraint: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", \
			__FILE__, __FUNC__, __LINE__, inst->iobj_SizeConstraints.MinX, \
			inst->iobj_SizeConstraints.MinY, inst->iobj_SizeConstraints.MaxX, \
			inst->iobj_SizeConstraints.MaxY));
		}

	dTag = FindTagItem(IDTA_Mask_Normal, ops->ops_AttrList);
	if (dTag)
		{
		// dispose old normal mask
		MyFreeVecPooled(ChipMemPool, (APTR *) &inst->iobj_NormalMask.iom_Mask);
		if (inst->iobj_NormalMask.iom_MaskBM)
			{
			d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_MaskBM));
			FreeBitMap(inst->iobj_NormalMask.iom_MaskBM);
			inst->iobj_NormalMask.iom_MaskBM = NULL;
			}

		d1(KPrintF("%s/%s/%ld: IDTA_Mask_Normal=%08lx  iom_Width=%ld  iom_Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, dTag->ti_Data, inst->iobj_NormalMask.iom_Width,\
                        inst->iobj_NormalMask.iom_Height));

		if (dTag->ti_Data)
			{
			// create new normal mask
			size_t Size;

			Size = inst->iobj_NormalMask.iom_Height * BYTESPERROW(inst->iobj_NormalMask.iom_Width);
			Size = ALIGN_LONG(Size);

			inst->iobj_NormalMask.iom_Mask = MyAllocVecPooled(ChipMemPool, Size);

			if (inst->iobj_NormalMask.iom_Mask)
				CopyMemQuick((UBYTE *) dTag->ti_Data, inst->iobj_NormalMask.iom_Mask, Size);

			d1(DumpMask(&inst->iobj_NormalMask));
			}
		}

	dTag = FindTagItem(IDTA_Mask_Selected, ops->ops_AttrList);
	if (dTag)
		{
		// dispose old selected mask
		MyFreeVecPooled(ChipMemPool, (APTR *) &inst->iobj_SelectedMask.iom_Mask);
		if (inst->iobj_SelectedMask.iom_MaskBM)
			{
			d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskBM));
			FreeBitMap(inst->iobj_SelectedMask.iom_MaskBM);
			inst->iobj_SelectedMask.iom_MaskBM = NULL;
			}

		d1(KPrintF("%s/%s/%ld: IDTA_Mask_Selected=%08lx  iom_Width=%ld  iom_Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, dTag->ti_Data, inst->iobj_NormalMask.iom_Width, \
                        inst->iobj_NormalMask.iom_Height));
		d1(KPrintF("%s/%s/%ld:  iobj_selmaskbm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskBM));

		if (dTag->ti_Data)
			{
			// create new selected mask
			size_t Size;

			Size = inst->iobj_SelectedMask.iom_Height * BYTESPERROW(inst->iobj_SelectedMask.iom_Width);
			Size = ALIGN_LONG(Size);

			inst->iobj_SelectedMask.iom_Mask = MyAllocVecPooled(ChipMemPool, Size);

			if (inst->iobj_SelectedMask.iom_Mask)
				CopyMemQuick((UBYTE *) dTag->ti_Data, inst->iobj_SelectedMask.iom_Mask, Size);
			}
		}

	dTag = FindTagItem(IDTA_WindowRect, ops->ops_AttrList);
	if (dTag && dTag->ti_Data)
		{
		struct IBox *WinRect = (struct IBox *) dTag->ti_Data;

		d1(kprintf("%s/%s/%ld: IDTA_WindowRect=%08lx  <%s> Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
			WinRect, inst->iobj_text, WinRect->Left, WinRect->Top, WinRect->Width, WinRect->Height));

		inst->iobj_winrect = *WinRect;
		}

	if (SizeSet)
		CheckIfScalingRequired(cl, o);

	if (NeedFreeLayout)
		{
		d1(KPrintF("%s/%s/%ld: FreeLayout enforced\n", __FILE__, __FUNC__, __LINE__));
		DoMethod(o, IDTM_FreeLayout, IOFREELAYOUTF_ScreenAvailable);
		}
}

//-----------------------------------------------------------------------------

static void SetAllocTag(ULONG TagValue, STRPTR *DataPtr, struct TagItem *TagList)
{
	struct TagItem *dTag;

	dTag = FindTagItem(TagValue, TagList);
	if (dTag)
		{
		STRPTR OldData = *DataPtr;

		*DataPtr = AllocCopyString((CONST_STRPTR) dTag->ti_Data);
		MyFreeVecPooled(PubMemPool, (APTR) &OldData);
		}
}

//-----------------------------------------------------------------------------

static void SetAllocTagProtected(ULONG TagValue, STRPTR *DataPtr,
	struct TagItem *TagList, struct SignalSemaphore *Sema)
{
	struct TagItem *dTag;

	dTag = FindTagItem(TagValue, TagList);
	if (dTag)
		{
		STRPTR OldData = *DataPtr;

		ObtainSemaphore(Sema);

		*DataPtr = AllocCopyString((CONST_STRPTR) dTag->ti_Data);
		MyFreeVecPooled(PubMemPool, (APTR) &OldData);

		ReleaseSemaphore(Sema);
		}
}

//-----------------------------------------------------------------------------

static STRPTR AllocCopyString(CONST_STRPTR OrigString)
{
	STRPTR NewString;

	if (OrigString)
		{
		NewString = MyAllocVecPooled(PubMemPool, 1 + strlen(OrigString));
		if (NewString)
			{
			strcpy(NewString, OrigString);
			}
		}
	else
		{
		NewString = NULL;
		}

	return NewString;
}

//-----------------------------------------------------------------------------

static void SetToolTypes(struct InstanceData *inst, const STRPTR *NewToolTypes)
{
	// same ToolType array as before?
	if (NewToolTypes != (const STRPTR *) inst->iobj_tooltypes)
		{
		STRPTR *OldToolTypes = inst->iobj_tooltypes;

		inst->iobj_tooltypes = CloneToolTypes(NewToolTypes);

		if (OldToolTypes)
			MyFreeVecPooled(PubMemPool, (APTR) &OldToolTypes);
		}
}

//-----------------------------------------------------------------------------

static STRPTR *CloneToolTypes(const STRPTR *OrigToolTypes)
{
	const STRPTR *TTPtr;
	STRPTR *TTClone;
	size_t AllocLen = sizeof(STRPTR);
	size_t StringCount = 1;

	d1(kprintf("%s/%s/%ld:  OrigToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, OrigToolTypes));

	for (TTPtr = OrigToolTypes; *TTPtr; TTPtr++)
		{
		d1(kprintf("%s/%s/%ld:  *TTPtr=<%s>\n", __FILE__, __FUNC__, __LINE__, *TTPtr));
		StringCount++;
		AllocLen += sizeof(STRPTR) + 1 + strlen(*TTPtr);
		}

	d1(kprintf("%s/%s/%ld:  StringCount=%lu  AllocLen=%lu\n", __FILE__, __FUNC__, __LINE__, StringCount, AllocLen));

	TTClone = MyAllocVecPooled(PubMemPool, AllocLen);
	if (TTClone)
		{
		STRPTR *TTDestPtr;
		STRPTR StringSpace;

		StringSpace = ((STRPTR) TTClone) + StringCount * sizeof(STRPTR);
		TTPtr = OrigToolTypes;
		TTDestPtr = TTClone;

		while (TTPtr && *TTPtr)
			{
			*TTDestPtr = StringSpace;
			strcpy(*TTDestPtr, *TTPtr);

			StringSpace += 1 + strlen(*TTPtr);
			TTDestPtr++;
			TTPtr++;
			}
		*TTDestPtr = NULL;
		}

	d1(kprintf("%s/%s/%ld:  TTClone=%08lx\n", __FILE__, __FUNC__, __LINE__, TTClone));

	return TTClone;
}

//-----------------------------------------------------------------------------

static void DrawFrame(Class *cl, Object *o, struct RastPort *rp,
	ULONG Width, ULONG Height, UWORD FrameType)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	if (!inst->iobj_Borderless
		&& 0 != inst->iobj_imgleft
		&& 0 != inst->iobj_imgtop
		&& FRAMETYPE_NONE != FrameType)
		{
		struct RastPort rpCopy;
		struct InstanceData *inst = INST_DATA(cl, o);

		rpCopy = *rp;

		McpGfxDrawFrame(&rpCopy, 0, 0,
			Width - 1, Height - 1,
			IA_HalfShinePen, inst->iobj_HalfShinePen,
			IA_HalfShadowPen, inst->iobj_HalfShadowPen,
			IA_Recessed, (FrameType & 0x8000) ? TRUE : FALSE,
			IA_FrameType, FrameType & 0x7fff,
			TAG_END);
		}
}

//-----------------------------------------------------------------------------

static void DrawMaskFrame(struct InstanceData *inst, struct BitMap *DestBitMap,
	ULONG Width, ULONG Height, UWORD FrameType)
{
	if (!inst->iobj_Borderless
		&& FRAMETYPE_NONE != FrameType
		&& 0 != inst->iobj_imgleft
		&& 0 != inst->iobj_imgtop)
		{
		struct RastPort TempRp;

		InitRastPort(&TempRp);
		TempRp.BitMap = DestBitMap;

		McpGfxDrawFrame(&TempRp,
			0, 0, Width - 1, Height - 1,
			IA_ShinePen, 1,
			IA_ShadowPen, 1,
			IA_HalfShinePen, 1,
			IA_HalfShadowPen, 1,
			IA_Recessed, (FrameType & 0x8000) ? TRUE : FALSE,
			IA_FrameType, FrameType & 0x7fff,
				TAG_END);
		}
}

//-----------------------------------------------------------------------------

static struct RastPort *InitRast(Class *cl, Object *o, struct iopLayout *iopl, struct RastPort *rp)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG BmDepth;

	memset(rp, 0, sizeof(struct RastPort));
	InitRastPort(rp);

	BmDepth = GetBitMapAttr(iopl->iopl_Screen->RastPort.BitMap, BMA_DEPTH);

	rp->BitMap = AllocBitMap(1 + inst->iobj_UnscaledWidth, 1 + inst->iobj_UnscaledHeight,
		BmDepth,
		BMF_MINPLANES | BMF_INTERLEAVED,
		iopl->iopl_Screen->RastPort.BitMap);

	return rp;
}

//-----------------------------------------------------------------------------

static struct BitMap *GenMask(Class *cl, Object *o,
	ULONG BmWidth, ULONG BmHeight, ULONG BmDepth, ULONG BmFlags,
	UBYTE *MaskPlane,
        UWORD FrameType, ULONG MaskWidth, ULONG MaskHeight)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct BitMap *DestBitMap;
	LONG ImgMaskWidth, ImgMaskHeight;

	ImgMaskWidth = inst->iobj_NakedMaskWidth
		+ inst->iobj_imgleft + inst->iobj_imgright;
	ImgMaskHeight = inst->iobj_NakedMaskHeight
		+ inst->iobj_imgtop + inst->iobj_imgbottom;

	// after ARGB scaling, SrcBitMap might be smaller than actual image
	if (BmWidth < ImgMaskWidth)
		BmWidth = (ImgMaskWidth + 15) & 0xfff0;
	if (BmHeight < ImgMaskHeight)
		BmHeight = ImgMaskHeight;

#if !defined(__AROS__)
	// XXX: is this really AROS specific?
	// maybe it's better to remove BMF_INTERLEAVED at all
	if (BmFlags & BMF_INTERLEAVED)
		{
		BmWidth *= BmDepth;
		}
#endif

	d1(KPrintF("%s/%s/%ld:  o=%08lx  <%s>  BmWidth=%lu  BmHeight=%lu\n", __FILE__, __FUNC__, __LINE__, o, inst->iobj_name, BmWidth, BmHeight));
	d1(KPrintF("%s/%s/%ld:  MaskPlane=%08lx\n", __FILE__, __FUNC__, __LINE__, MaskPlane));
	d1(KPrintF("%s/%s/%ld:  MaskWidth=%lu  MaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, MaskWidth, MaskHeight));
	d1(KPrintF("%s/%s/%ld:  ImgMaskWidth=%lu  ImgMaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, ImgMaskWidth, ImgMaskHeight));
	d1(KPrintF("%s/%s/%ld:  iobj_imgleft=%ld  iobj_imgright=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_imgleft, inst->iobj_imgright));
	d1(KPrintF("%s/%s/%ld:  iobj_imgtop=%ld  iobj_imgbottom=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_imgtop, inst->iobj_imgbottom));
	d1(KPrintF("%s/%s/%ld:  iobj_NakedMaskWidth=%ld  iobj_NakedMaskHeight=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst->iobj_NakedMaskWidth, inst->iobj_NakedMaskHeight));

	DestBitMap = AllocBitMap(BmWidth, BmHeight, 1, BMF_CLEAR, NULL);
	d1(KPrintF("%s/%s/%ld:  DestBitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, DestBitMap));
	if (DestBitMap)
		{
		ASSERT_d1(BmWidth >= MaskWidth);
		ASSERT_d1(BmHeight >= MaskHeight);

		if (MaskPlane && (BmWidth >= MaskWidth) && (BmHeight >= MaskHeight))
			{
			struct BitMap TempBM;

			d1(KPrintF("%s/%s/%ld:  MaskWidth=%lu  MaskHeight=%lu  BytesPerRow=%ld\n", __FILE__, __FUNC__, __LINE__, MaskWidth, MaskHeight, BYTESPERROW(MaskWidth)));
			d1(KPrintF("%s/%s/%ld:  ImgMaskWidth=%lu  ImgMaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, ImgMaskWidth, ImgMaskHeight));
			d1(KPrintF("%s/%s/%ld:  BmWidth=%lu  BmHeight=%lu\n", __FILE__, __FUNC__, __LINE__, BmWidth, BmHeight));

			memset(&TempBM, 0, sizeof(TempBM));
			TempBM.BytesPerRow = BYTESPERROW(MaskWidth);
			TempBM.Rows = MaskHeight;
			TempBM.Flags = 0;
			TempBM.Depth = 1;
			TempBM.Planes[0] = MaskPlane;

#if 0
			{
			struct RastPort rp;

			InitRastPort(&rp);
			rp.BitMap = &TempBM;
			SetAPen(&rp, 1);
			RectFill(&rp,
				0, 0,
				MaskWidth - 1,
				MaskHeight - 1);
			}
#endif

			d1(KPrintF("%s/%s/%ld:  BytesPerRow=%lu\n", __FILE__, __FUNC__, __LINE__, TempBM.BytesPerRow));

			ASSERT_d1(MaskWidth <= GetBitMapAttr(DestBitMap, BMA_WIDTH));
			ASSERT_d1(MaskHeight <= GetBitMapAttr(DestBitMap, BMA_HEIGHT));

			BltBitMap(&TempBM, 0, 0,
				DestBitMap, 
				inst->iobj_imgleft, inst->iobj_imgtop,
				MaskWidth,
				MaskHeight,
				ABC | ABNC, ~0, NULL);
			}
		else
			{
			struct ExtGadget *gg = (struct ExtGadget *) o;
			struct RastPort rp;
			WORD Right, Bottom;

			InitRastPort(&rp);
			rp.BitMap = DestBitMap;

			Right  = min(gg->Width - inst->iobj_imgright - 1, BmWidth - 1);
			Bottom = min(gg->Height - inst->iobj_imgbottom - 1, BmHeight - 1);

			ASSERT_d1(Right <  GetBitMapAttr(rp.BitMap, BMA_WIDTH));
			ASSERT_d1(Bottom <  GetBitMapAttr(rp.BitMap, BMA_HEIGHT));

			SetAPen(&rp, 1);
			RectFill(&rp,
				inst->iobj_imgleft, inst->iobj_imgtop,
				Right,
				Bottom);
			}
		WaitBlit();

		ASSERT_d1(ImgMaskWidth <= GetBitMapAttr(DestBitMap, BMA_WIDTH));
		ASSERT_d1(ImgMaskHeight <= GetBitMapAttr(DestBitMap, BMA_HEIGHT));

		DrawMaskFrame(inst, DestBitMap,
			ImgMaskWidth,
			ImgMaskHeight,
			FrameType);
		}

	d1(KPrintF("%s/%s/%ld:  END  DestBitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, DestBitMap));

	return DestBitMap;
}

//-----------------------------------------------------------------------------

static struct BitMap *GenInvMask(Class *cl, Object *o, struct BitMap *SrcBitMap)
{
//	  struct InstanceData *inst = INST_DATA(cl, o);
	struct BitMap *DestBitMap;
	ULONG BmWidth, BmHeight;
	ULONG BmFlags;
	ULONG BmDepth;

	BmFlags = GetBitMapAttr(SrcBitMap, BMA_FLAGS);
	BmWidth = GetBitMapAttr(SrcBitMap, BMA_WIDTH);
	BmHeight = GetBitMapAttr(SrcBitMap, BMA_HEIGHT);
	BmDepth = GetBitMapAttr(SrcBitMap, BMA_DEPTH);

	DestBitMap = AllocBitMap(BmWidth, BmHeight, BmDepth, BmFlags | BMF_CLEAR, NULL);
	d1(KPrintF("%s/%s/%ld:  DestBitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, DestBitMap));
	if (DestBitMap)
		{
		// Blit inverted copy of SrcBitMap to DestBitMap
		ASSERT_d1(BmWidth <=  GetBitMapAttr(DestBitMap, BMA_WIDTH));
		ASSERT_d1(BmHeight <=  GetBitMapAttr(DestBitMap, BMA_HEIGHT));

		BltBitMap(SrcBitMap,
			0, 0,
			DestBitMap,
			0, 0,
			BmWidth, BmHeight,
			ANBC | ANBNC,
			~0,
                        NULL);
		}

	d1(KPrintF("%s/%s/%ld:  END  DestBitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, DestBitMap));

	return DestBitMap;
}

//-----------------------------------------------------------------------------

static BOOL ScaleRenderBitMapsNormal(Class *cl, Object *o,
	ULONG NewWidth, ULONG NewHeight, struct Screen *screen)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct RastPort *rpNormal = (struct RastPort *) gg->GadgetRender;
	struct BitMap *NormBM = NULL;
	struct BitMap *NormMaskBM = NULL;
	ULONG *ColorTable;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld:  START cl=%08lx  o=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, cl, o, inst->iobj_name));
	d1(KPrintF("%s/%s/%ld:  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_layoutflags));

	do	{
		ULONG ScaleFlags = SCALEFLAGF_DOUBLESIZE | SCALEFLAGF_BILINEAR
			| SCALEFLAGF_AVERAGE;

		d1(KPrintF("%s/%s/%ld:  iobj_UnscaledWidth=%lu  iobj_UnscaledHeight=%ld\n", __FILE__, __FUNC__, __LINE__, inst->iobj_UnscaledWidth, inst->iobj_UnscaledHeight));
		d1(KPrintF("%s/%s/%ld:  iobj_UnscaledNakedWidth=%lu  iobj_UnscaledNakedHeight=%ld\n", __FILE__, __FUNC__, __LINE__, inst->iobj_UnscaledNakedWidth, inst->iobj_UnscaledNakedHeight));
		d1(KPrintF("%s/%s/%ld:  NewWidth=%lu  NewHeight=%ld\n", __FILE__, __FUNC__, __LINE__, NewWidth, NewHeight));
		d1(KPrintF("%s/%s/%ld:  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_layoutflags));

		ColorTable = MyAllocVecPooled(PubMemPool,
			screen->ViewPort.ColorMap->Count * sizeof(ULONG) * 3);
		if (NULL == ColorTable)
			break;

		GetRGB32(screen->ViewPort.ColorMap,
			0, screen->ViewPort.ColorMap->Count, ColorTable);

		if (rpNormal && (inst->iobj_layoutflags & IOBLAYOUTF_ScaleNormal))
			{
			struct ScaleBitMapArg sbma;

			sbma.sbma_SourceBM = rpNormal->BitMap;
			sbma.sbma_SourceWidth = inst->iobj_UnscaledWidth;
			sbma.sbma_SourceHeight = inst->iobj_UnscaledHeight;
			sbma.sbma_DestWidth = &NewWidth;
			sbma.sbma_DestHeight = &NewHeight;
			sbma.sbma_NumberOfColors = screen->ViewPort.ColorMap->Count;
			sbma.sbma_ColorTable = ColorTable;
			sbma.sbma_Flags = ScaleFlags;
			sbma.sbma_ScreenBM = screen->RastPort.BitMap;

			NormBM = ScalosGfxScaleBitMap(&sbma, NULL);
			d1(KPrintF("%s/%s/%ld:  NormBM=%08lx\n", __FILE__, __FUNC__, __LINE__, NormBM));
			if (NULL == NormBM)
				break;

			inst->iobj_layoutflags &= ~IOBLAYOUTF_ScaleNormal;

			d1(KPrintF("%s/%s/%ld:  iom_MaskBM=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_MaskBM));
			if (inst->iobj_NormalMask.iom_MaskBM)
				{
				d1(DumpMaskBM(&inst->iobj_NormalMask));

				NormMaskBM = ScaleMaskBitMap(inst->iobj_NormalMask.iom_MaskBM,
					inst->iobj_NormalMask.iom_Width,
					inst->iobj_NormalMask.iom_Height,
					&NewWidth, &NewHeight,
					ScaleFlags,
					screen->RastPort.BitMap);
				d1(KPrintF("%s/%s/%ld:  NormMaskBM=%08lx  NewWidth=%ld  NewHeight=%ld\n", \
					__FILE__, __FUNC__, __LINE__, NormMaskBM, NewWidth, NewHeight));
				if (NULL == NormMaskBM)
					break;

				DrawMaskFrame(inst, NormMaskBM,
					NewWidth, NewHeight,
					inst->iobj_frametype);
				}

			// Replace GadgetRender BitMap by scaled one
			d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, rpNormal->BitMap));
			FreeBitMap(rpNormal->BitMap);
			rpNormal->BitMap = NormBM;
			NormBM = NULL;

			// optionally replace normal mask BitMap by scaled one
			if (NormMaskBM)
				{
				d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_MaskBM));
				if (inst->iobj_NormalMask.iom_MaskBM)
					FreeBitMap(inst->iobj_NormalMask.iom_MaskBM);
				inst->iobj_NormalMask.iom_MaskBM = NormMaskBM;

				NormMaskBM = NULL;

				d1(DumpMaskBM(&inst->iobj_NormalMask));
				}
			
			DrawFrame(cl, o, rpNormal,
				NewWidth,
				NewHeight,
				inst->iobj_frametype);
                        }

		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld:  NormMaskWidth=%lu  NormMaskHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NormalMask.iom_Width, inst->iobj_NormalMask.iom_Height));
	d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, NormBM));
	if (NormBM)
		FreeBitMap(NormBM);
	d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, NormMaskBM));
	if (NormMaskBM)
		FreeBitMap(NormMaskBM);
	if (ColorTable)
		MyFreeVecPooled(PubMemPool, (APTR) &ColorTable);

	d1(KPrintF("%s/%s/%ld:  END cl=%08lx  o=%08lx  Success=%ld  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, Success, inst->iobj_layoutflags));

	return Success;
}

//-----------------------------------------------------------------------------

static BOOL ScaleRenderBitMapsSelected(Class *cl, Object *o,
	ULONG NewWidth, ULONG NewHeight, struct Screen *screen)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct RastPort *rpSelect = (struct RastPort *) gg->SelectRender;
	struct BitMap *SelBM = NULL;
	struct BitMap *SelMaskBM = NULL;
	ULONG *ColorTable;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld:  START cl=%08lx  o=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o));
	d1(KPrintF("%s/%s/%ld:  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_layoutflags));

	do	{
		ULONG ScaleFlags = SCALEFLAGF_DOUBLESIZE | SCALEFLAGF_BILINEAR
			| SCALEFLAGF_AVERAGE;

		d1(KPrintF("%s/%s/%ld:  NewWidth=%lu  NewHeight=%ld\n", __FILE__, __FUNC__, __LINE__, NewWidth, NewHeight));
		d1(KPrintF("%s/%s/%ld:  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_layoutflags));
		d1(KPrintF("%s/%s/%ld:  SelBM: Width=%lu  Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, GetBitMapAttr(rpSelect->BitMap, BMA_WIDTH), \
			GetBitMapAttr(rpSelect->BitMap, BMA_HEIGHT)));

		ColorTable = MyAllocVecPooled(PubMemPool,
			screen->ViewPort.ColorMap->Count * sizeof(ULONG) * 3);
		if (NULL == ColorTable)
			break;

		GetRGB32(screen->ViewPort.ColorMap,
			0, screen->ViewPort.ColorMap->Count, ColorTable);

		if (rpSelect && (inst->iobj_layoutflags & IOBLAYOUTF_ScaleSelected))
			{
			struct ScaleBitMapArg sbma;

			sbma.sbma_SourceBM = rpSelect->BitMap;
			sbma.sbma_SourceWidth = inst->iobj_UnscaledWidth;
			sbma.sbma_SourceHeight = inst->iobj_UnscaledHeight;
			sbma.sbma_DestWidth = &NewWidth;
			sbma.sbma_DestHeight = &NewHeight;
			sbma.sbma_NumberOfColors = screen->ViewPort.ColorMap->Count;
			sbma.sbma_ColorTable = ColorTable;
			sbma.sbma_Flags = ScaleFlags;
			sbma.sbma_ScreenBM = screen->RastPort.BitMap;

			SelBM = ScalosGfxScaleBitMap(&sbma, NULL);
			d1(KPrintF("%s/%s/%ld:  SelBM=%08lx\n", __FILE__, __FUNC__, __LINE__, SelBM));
			if (NULL == SelBM)
				break;

			inst->iobj_layoutflags &= ~IOBLAYOUTF_ScaleSelected;

			d1(KPrintF("%s/%s/%ld:  iobj_selmaskbm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskBM));
			if (inst->iobj_SelectedMask.iom_MaskBM)
				{
				SelMaskBM = ScaleMaskBitMap(inst->iobj_SelectedMask.iom_MaskBM,
					inst->iobj_SelectedMask.iom_Width,
					inst->iobj_SelectedMask.iom_Height,
					&NewWidth, &NewHeight,
					ScaleFlags,
					screen->RastPort.BitMap);
				d1(KPrintF("%s/%s/%ld:  SelMaskBM=%08lx\n", __FILE__, __FUNC__, __LINE__, SelMaskBM));
				if (NULL == SelMaskBM)
					break;

				DrawMaskFrame(inst, SelMaskBM,
					NewWidth, NewHeight,
					inst->iobj_frametypesel);
				}

			// Replace SelectRender BitMap by scaled one
			d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, rpSelect->BitMap));
			FreeBitMap(rpSelect->BitMap);
			rpSelect->BitMap = SelBM;
			SelBM = NULL;

			// optionally replace selected mask BitMap by scaled one
			if (SelMaskBM)
				{
				d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskBM));
				if (inst->iobj_SelectedMask.iom_MaskBM)
					FreeBitMap(inst->iobj_SelectedMask.iom_MaskBM);

				inst->iobj_SelectedMask.iom_MaskBM = SelMaskBM;

                                SelMaskBM = NULL;
				}

			DrawFrame(cl, o, rpSelect,
				NewWidth,
				NewHeight,
				inst->iobj_frametypesel);
			}
		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, SelBM));
	d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, SelMaskBM));
	d1(KPrintF("%s/%s/%ld:  iobj_selmaskbm=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_SelectedMask.iom_MaskBM));

	if (SelBM)
		FreeBitMap(SelBM);
	if (SelMaskBM)
		FreeBitMap(SelMaskBM);
	if (ColorTable)
		MyFreeVecPooled(PubMemPool, (APTR) &ColorTable);

	d1(KPrintF("%s/%s/%ld:  END cl=%08lx  o=%08lx  Success=%ld  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, Success, inst->iobj_layoutflags));

	return Success;
}

//-----------------------------------------------------------------------------

static void LayoutIconSize(Class *cl, Object *o, struct RastPort *rp)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld:  START cl=%08lx  o=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o));
	d1(KPrintF("%s/%s/%ld:  layoutflags=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->iobj_layoutflags));
	TIMESTAMP_d1();

	if (!(inst->iobj_layoutflags & IOBLAYOUTF_Size))
		{
		struct ExtGadget *gg = (struct ExtGadget *) o;

		inst->iobj_layoutflags |= IOBLAYOUTF_Size;

		d1(KPrintF("%s/%s/%ld:  NakedWidth=%lu  NakedHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_NakedWidth, inst->iobj_NakedHeight));
		d1(KPrintF("%s/%s/%ld:  UnscalednakedWidth=%lu  UnscalednakedHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_UnscaledNakedWidth, inst->iobj_UnscaledNakedHeight));
		TIMESTAMP_d1();

		// adjust icon dimensions (add border)
		gg->Width  = inst->iobj_NakedWidth
			+ inst->iobj_imgleft + inst->iobj_imgright;
		gg->Height = inst->iobj_NakedHeight
			+ inst->iobj_imgtop + inst->iobj_imgbottom;

		d1(KPrintF("%s/%s/%ld:  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

		inst->iobj_UnscaledWidth = inst->iobj_UnscaledNakedWidth
			+ inst->iobj_imgleft + inst->iobj_imgright;
		inst->iobj_UnscaledHeight = inst->iobj_UnscaledNakedHeight
			+ inst->iobj_imgtop + inst->iobj_imgbottom;

		d1(KPrintF("%s/%s/%ld:  UnscaledWidth=%lu  UnscaledHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_UnscaledWidth, inst->iobj_UnscaledHeight));

		inst->iobj_ScaledWidth = inst->iobj_NakedWidth
			+ inst->iobj_imgleft + inst->iobj_imgright;
		inst->iobj_ScaledHeight = inst->iobj_NakedHeight
			+ inst->iobj_imgtop + inst->iobj_imgbottom;

		d1(KPrintF("%s/%s/%ld:  ScaledWidth=%lu  ScaledHeight=%lu\n", __FILE__, __FUNC__, __LINE__, inst->iobj_ScaledWidth, inst->iobj_ScaledHeight));
		TIMESTAMP_d1();

		// ExtGadget has valid bounds
		gg->MoreFlags |= GMORE_BOUNDS;

		gg->BoundsLeftEdge = gg->LeftEdge;
		gg->BoundsTopEdge = gg->TopEdge;
		gg->BoundsWidth = gg->Width;
		gg->BoundsHeight = gg->Height;

		TIMESTAMP_d1();

		if (inst->iobj_text)
			LayoutIconText(inst, rp, gg);
		}
	TIMESTAMP_d1();
}

//-----------------------------------------------------------------------------

static struct BitMap *ScaleMaskBitMap(struct BitMap *SourceBM,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG *NewWidth, ULONG *NewHeight,
	ULONG ScaleFlags, struct BitMap *FriendBM)
{
	struct BitMap *SourceBMFriend;
	struct BitMap *NewMaskBM = NULL;
	struct BitMap *ScaledBM = NULL;

	d1(KPrintF("%s/%s/%ld:  START SourceWidth=%lu  SourceHeight=%lu\n", __FILE__, __FUNC__, __LINE__, SourceWidth, SourceHeight));
	d1(KPrintF("%s/%s/%ld:  DestWidth=%lu  DestHeight=%lu\n", __FILE__, __FUNC__, __LINE__, *NewWidth, *NewHeight));
	d1(KPrintF("%s/%s/%ld:  SourceBM: Width=%lu  Height=%ld  BytesPerRow=%ld\n", \
			__FILE__, __FUNC__, __LINE__, GetBitMapAttr(SourceBM, BMA_WIDTH), \
			GetBitMapAttr(SourceBM, BMA_HEIGHT), SourceBM->BytesPerRow));

	do	{
		ULONG BmWidth;
		struct ScaleBitMapArg sbma;
		static ULONG ColorTable[] =
			{
			0, 0, 0,        // pen 0 = black
			~0, ~0, ~0,     // pen 1 = white
			};

		SourceBMFriend = AllocBitMap(GetBitMapAttr(SourceBM, BMA_WIDTH),
			GetBitMapAttr(SourceBM, BMA_HEIGHT),
			1, BMF_CLEAR, FriendBM);
		if (NULL == SourceBMFriend)
			break;

		d1(KPrintF("%s/%s/%ld:  SourceBMFriend Planes=%08lx %08lx %08lx %08lx\n",
			__FILE__, __FUNC__, __LINE__, SourceBMFriend->Planes[0], SourceBMFriend->Planes[1], SourceBMFriend->Planes[2], SourceBMFriend->Planes[3]));

		ASSERT_d1(SourceWidth <=  GetBitMapAttr(SourceBMFriend, BMA_WIDTH));
		ASSERT_d1(SourceHeight <=  GetBitMapAttr(SourceBMFriend, BMA_HEIGHT));

		BltBitMap(SourceBM, 0, 0,
			SourceBMFriend, 0, 0,
			SourceWidth, SourceHeight,
			ABC | ABNC,
			~0,
			NULL);
		WaitBlit();

#if 0
		{
		struct RastPort rp;

		InitRastPort(&rp);
		rp.BitMap = SourceBMFriend;
		SetAPen(&rp, 1);
		RectFill(&rp,
			0, 0,
			GetBitMapAttr(SourceBMFriend, BMA_WIDTH) - 1,
			GetBitMapAttr(SourceBMFriend, BMA_HEIGHT) - 1);
		}
#endif

		sbma.sbma_SourceBM = SourceBMFriend;
		sbma.sbma_SourceWidth = SourceWidth;
		sbma.sbma_SourceHeight = SourceHeight;
		sbma.sbma_DestWidth = NewWidth;
		sbma.sbma_DestHeight = NewHeight;
		sbma.sbma_NumberOfColors = 2;
		sbma.sbma_ColorTable = ColorTable;
		sbma.sbma_Flags = ScaleFlags;
		sbma.sbma_ScreenBM = FriendBM;

		ScaledBM = ScalosGfxScaleBitMap(&sbma, NULL);
		if (NULL == ScaledBM)
			break;

		BmWidth = *NewWidth;
		if (GetBitMapAttr(FriendBM, BMA_FLAGS) & BMF_INTERLEAVED)
			BmWidth *= GetBitMapAttr(FriendBM, BMA_DEPTH);

		NewMaskBM = AllocBitMap(BmWidth, *NewHeight,
			1, BMF_CLEAR, NULL);
		if (NULL == NewMaskBM)
			break;

		ASSERT_d1(*NewWidth <=  GetBitMapAttr(NewMaskBM, BMA_WIDTH));
		ASSERT_d1(*NewHeight <=  GetBitMapAttr(NewMaskBM, BMA_HEIGHT));

		BltBitMap(ScaledBM, 0, 0,
			NewMaskBM, 0, 0,
			*NewWidth, *NewHeight,
			ABC | ABNC,
			~0,
			NULL);

		d1(KPrintF("%s/%s/%ld:  NewMaskBM: Width=%lu  Height=%ld  BytesPerRow=%ld\n", \
			__FILE__, __FUNC__, __LINE__, GetBitMapAttr(NewMaskBM, BMA_WIDTH), \
			GetBitMapAttr(NewMaskBM, BMA_HEIGHT), NewMaskBM->BytesPerRow));

		WaitBlit();
		} while (0);

	d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, SourceBMFriend));
	d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, ScaledBM));

	if (SourceBMFriend)
		FreeBitMap(SourceBMFriend);
	if (ScaledBM)
		FreeBitMap(ScaledBM);

	d1(KPrintF("%s/%s/%ld:  END  NewMaskBM=%08lx\n", __FILE__, __FUNC__, __LINE__, NewMaskBM));

	return NewMaskBM;
}

//-----------------------------------------------------------------------------

static void CheckIfScalingRequired(Class *cl, Object *o)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld:  START o=%08lx  <%s>  iobj_NakedWidth=%ld  iobj_NakedHeight=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, inst->iobj_name, inst->iobj_NakedWidth, inst->iobj_NakedHeight));

	if (inst->iobj_ScalingPercentage != 100
		|| inst->iobj_NakedWidth < inst->iobj_SizeConstraints.MinX
		|| inst->iobj_NakedWidth > inst->iobj_SizeConstraints.MaxX
		|| inst->iobj_NakedHeight < inst->iobj_SizeConstraints.MinY
		|| inst->iobj_NakedHeight> inst->iobj_SizeConstraints.MaxY)
		{
		// We need to scale the icon!
		d1(KPrintF("%s/%s/%ld:  Need to scale icon <%s>\n", __FILE__, __FUNC__, __LINE__, inst->iobj_name));

		inst->iobj_NakedWidth = (gg->Width * inst->iobj_ScalingPercentage) / 100;
		inst->iobj_NakedHeight = (gg->Height * inst->iobj_ScalingPercentage) / 100;

		if (gg->Width < inst->iobj_SizeConstraints.MinX)
			inst->iobj_NakedWidth = inst->iobj_SizeConstraints.MinX;
		else if (gg->Width > inst->iobj_SizeConstraints.MaxX)
			inst->iobj_NakedWidth = inst->iobj_SizeConstraints.MaxX;

		if (gg->Height < inst->iobj_SizeConstraints.MinY)
			inst->iobj_NakedHeight = inst->iobj_SizeConstraints.MinY;
		else if (gg->Height > inst->iobj_SizeConstraints.MaxY)
			inst->iobj_NakedHeight = inst->iobj_SizeConstraints.MaxY;

		ScalosGfxCalculateScaleAspect(inst->iobj_UnscaledWidth, inst->iobj_UnscaledHeight,
			&inst->iobj_NakedWidth, &inst->iobj_NakedHeight);

		if (inst->iobj_UnscaledNakedWidth != inst->iobj_NakedWidth
			|| inst->iobj_UnscaledNakedHeight != inst->iobj_NakedHeight)
			{
			gg->Width = inst->iobj_ScaledWidth = inst->iobj_NakedWidth;
			gg->Height = inst->iobj_ScaledHeight = inst->iobj_NakedHeight;

			inst->iobj_layoutflags &= ~IOBLAYOUTF_Size;
			}

		inst->iobj_layoutflags |= IOBLAYOUTF_ScaleNormal | IOBLAYOUTF_ScaleSelected;
		}

	d1(KPrintF("%s/%s/%ld:  END \n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------------

#if defined(DEBUG_MEMORY)

APTR MyAllocVecPooled_Debug(void *MemPool, size_t Size, CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	struct AllocatedMemFromPoolDebug *ptr;

	if (MemPool)
		{
		size_t AllocSize;

		AllocSize = Size + sizeof(struct AllocatedMemFromPoolDebug) + SCALOS_MEM_TRAILER * sizeof(ULONG);

		ObtainSemaphore(&MemPoolSemaphore);
		ptr = AllocPooled(MemPool, AllocSize);
		ReleaseSemaphore(&MemPoolSemaphore);
		if (ptr)
			{
			ULONG n;

			ptr->amp_Size = AllocSize;

			ptr->amp_Line = CallingLine;
			ptr->amp_File = CallingFile;
			ptr->amp_Function = CallingFunc;

			ptr->amp_Magic = SCALOS_MEM_START_MAGIC;

			for (n = 0; n < SCALOS_MEM_TRAILER; n++)
				*((ULONG *) &ptr->amp_UserData[Size + n * sizeof(ULONG)]) = SCALOS_MEM_END_MAGIC;

			return (APTR) &ptr->amp_UserData;
			}
		}

	return NULL;
}


void MyFreeVecPooled_Debug(void *MemPool, APTR *mem, CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine)
{
	if (MemPool && *mem)
		{
		ULONG n;
		size_t OrigSize;
		struct AllocatedMemFromPoolDebug *ptr;

		ptr = (struct AllocatedMemFromPoolDebug *) (((UBYTE *) *mem) - offsetof(struct AllocatedMemFromPoolDebug, amp_UserData));

		if (ptr->amp_Magic != SCALOS_MEM_START_MAGIC)
			{
			kprintf("ScalosFreeVecPooled: %08lx START_MAGIC not found, called from %s/%s/%ld\n",
				*mem, CallingFile, CallingFunc, CallingLine);
			return;
			}

		OrigSize = ptr->amp_Size - sizeof(struct AllocatedMemFromPoolDebug) - SCALOS_MEM_TRAILER * sizeof(ULONG);

		// Check if block trailer is OK
		for (n = 0; n < SCALOS_MEM_TRAILER; n++)
			{
			if (*((ULONG *) &ptr->amp_UserData[OrigSize + n * sizeof(ULONG)]) != SCALOS_MEM_END_MAGIC)
				{
				kprintf("ScalosFreeVecPooled: %08lx trailer damaged, called from %s/%s/%ld\n",
					*mem, CallingFile, CallingFunc, CallingLine);
				kprintf("               original Length=%lu, allocated from %s/%s/%ld\n",
					OrigSize, ptr->amp_File, ptr->amp_Function, ptr->amp_Line);
				}
			}

		ObtainSemaphore(&MemPoolSemaphore);
		FreePooled(MemPool, ptr, ptr->amp_Size);
		ReleaseSemaphore(&MemPoolSemaphore);

		*mem = NULL;
		}
}

#else /* DEBUG_MEMORY */

APTR MyAllocVecPooled(void *MemPool, size_t Size)
{
	APTR ptr;

	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, MemPool, Size));

	if (MemPool)
		{
		ObtainSemaphore(&MemPoolSemaphore);
		ptr = AllocPooled(MemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&MemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, MemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, MemPool, Size));

	return NULL;
}


void MyFreeVecPooled(void *MemPool, APTR *mem)
{
	d1(KPrintF("%s/%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, MemPool, *mem));
	if (MemPool && *mem)
		{
		size_t size;
		size_t *sptr = (size_t *) *mem;
		APTR MemPtr;

		MemPtr = &sptr[-1];
		size = sptr[-1];

		ObtainSemaphore(&MemPoolSemaphore);
		FreePooled(MemPool, MemPtr, size + sizeof(size_t));
		ReleaseSemaphore(&MemPoolSemaphore);
		}

	*mem = NULL;
}


#endif /* DEBUG_MEMORY */

//-----------------------------------------------------------------------------

struct TagItem *ScalosVTagList(ULONG FirstTag, va_list args)
{
	struct TagItem *ClonedTagList;
	ULONG AllocatedSize = 10;

	d1(KPrintF("%s/%s/%ld: FirstTag=%08lx  args=%08lx\n", __FILE__, __FUNC__, __LINE__, FirstTag, args));

	do	{
		ULONG NumberOfTags = 1;
		BOOL Finished = FALSE;
		struct TagItem *ti;

		ClonedTagList = ti = AllocateTagItems(AllocatedSize);
		if (NULL == ClonedTagList)
			break;

		ClonedTagList->ti_Tag = FirstTag;
		while (!Finished)
			{
			switch (ti->ti_Tag)
				{
			case TAG_MORE:
				ti->ti_Data = va_arg(args, ULONG);
				Finished = TRUE;
				break;
			case TAG_END:
				Finished = TRUE;
				break;
			default:
				ti->ti_Data = va_arg(args, ULONG);
				break;
				}

			d1(KPrintF("%s/%s/%ld  ti=%08lx  Tag=%08lx  Data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti, ti->ti_Tag, ti->ti_Data));

			if (!Finished)
				{
				if (++NumberOfTags >= AllocatedSize)
					{
					// we need to extend our allocated taglist
					struct TagItem *ExtendedTagList;
					ULONG ExtendedSize = 2 * AllocatedSize;

					ExtendedTagList	= AllocateTagItems(ExtendedSize);
					if (NULL == ExtendedTagList)
						{
						FreeTagItems(ClonedTagList);
						ClonedTagList = NULL;
						break;
						}

					d1(KPrintF("%s/%s/%ld: ExtendedTagList=%08lx  ClonedTagList=%08lx\n", __FILE__, __FUNC__, __LINE__, ExtendedTagList, ClonedTagList));

					// copy contents from old to extended taglist
					memcpy(ExtendedTagList, ClonedTagList, sizeof(struct TagItem) * AllocatedSize);

					// free old taglist
					FreeTagItems(ClonedTagList);
					ClonedTagList = ExtendedTagList;

					ti = ClonedTagList + NumberOfTags - 1;
					AllocatedSize = ExtendedSize;
					}
				else
					{
					ti++;
					}

				ti->ti_Tag = va_arg(args, ULONG);
				}
			}

		d1(KPrintF("%s/%s/%ld: NumberOfTags=%lu  AllocatedSize=%lu\n", __FILE__, __FUNC__, __LINE__, NumberOfTags, AllocatedSize));
		} while (0);

	d1(KPrintF("%s/%s/%ld: ClonedTagList=%08lx\n", __FILE__, __FUNC__, __LINE__, ClonedTagList));

	d1({ ULONG n; \
		for (n = 0; ClonedTagList; n++) \
			{ \
			KPrintF("%s/%s/%ld: TagList[%ld]: Tag=%08lx  Data=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, n, ClonedTagList[n].ti_Tag, ClonedTagList[n].ti_Data); \
			if (TAG_END == ClonedTagList[n].ti_Tag || TAG_MORE == ClonedTagList[n].ti_Tag)
				break;
			} \
		})

	return ClonedTagList;
}

//-----------------------------------------------------------------------------

static void ReplaceARGBImage(struct IconObjectARGB *img, struct ARGBHeader *argbh)
{
	if (argbh)
		img->iargb_ARGBimage = *argbh;
	else
		img->iargb_ARGBimage.argb_ImageData = NULL;

	d1(KPrintF("%s/%s/%ld: argb_Width=%ld  argb_Height=%ld\n", __FILE__, __FUNC__, __LINE__, img->iargb_ARGBimage.argb_Width, img->iargb_ARGBimage.argb_Height));

	if (img->iargb_CopyARGBImageData)
		{
		ScalosGfxFreeARGB(&img->iargb_ClonedARGBImage.argb_ImageData);

		if (img->iargb_ARGBimage.argb_ImageData)
			{
			ULONG Length = img->iargb_ARGBimage.argb_Width * img->iargb_ARGBimage.argb_Height * sizeof(struct ARGB);

			img->iargb_ClonedARGBImage = img->iargb_ARGBimage;

			img->iargb_ClonedARGBImage.argb_ImageData = ScalosGfxCreateARGB(img->iargb_ARGBimage.argb_Width, img->iargb_ARGBimage.argb_Height, NULL);
			if (img->iargb_ClonedARGBImage.argb_ImageData)
				{
				memcpy(img->iargb_ClonedARGBImage.argb_ImageData,
					img->iargb_ARGBimage.argb_ImageData, Length);

				img->iargb_ARGBimage = img->iargb_ClonedARGBImage;
				d1(KPrintF("%s/%s/%ld: argbh=%08lx  argb_ImageData=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, img->iargb_ARGBimage, img->iargb_ARGBimage.argb_ImageData));
				}
			}
		}
}

//-----------------------------------------------------------------------------

static void FreeARGBImage(struct IconObjectARGB *img)
{
	if (img)
		{
		ScalosGfxFreeARGB(&img->iargb_ScaledARGBImage.argb_ImageData);
		ScalosGfxFreeARGB(&img->iargb_ClonedARGBImage.argb_ImageData);
		MyFreeVecPooled(PubMemPool, (APTR *) &img->iargb_AlphaChannel);
		d1(KPrintF("%s/%s/%ld: w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, img->iargb_ARGBimage.argb_Width, img->iargb_ARGBimage.argb_Height));
		}
}

//-----------------------------------------------------------------------------

static void FreeARGBImageLayout(struct IconObjectARGB *img)
{
	MyFreeVecPooled(PubMemPool, (APTR *) &img->iargb_AlphaChannel);
}

//-----------------------------------------------------------------------------

static void FreeMask(struct IconObjectMask *Mask)
{
	MyFreeVecPooled(ChipMemPool, (APTR *) &Mask->iom_Mask);

	if (Mask->iom_MaskBM)
		{
		d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, Mask->iom_MaskBM));
		FreeBitMap(Mask->iom_MaskBM);
		Mask->iom_MaskBM = NULL;
		}
	if (Mask->iom_MaskInvBM)
		{
		d1(KPrintF("%s/%s/%ld:  bm=%08lx\n", __FILE__, __FUNC__, __LINE__, Mask->iom_MaskInvBM));
		FreeBitMap(Mask->iom_MaskInvBM);
		Mask->iom_MaskInvBM = NULL;
		}
}

//-----------------------------------------------------------------------------

static void LayoutARGB(Class *cl, Object *o, struct IconObjectARGB *img)
{
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ARGBHeader *ImgHeader;

	d1(KPrintF("%s/%s/%ld:  START  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
	d1(KPrintF("%s/%s/%ld:  iargb_ARGBimage  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
		img->iargb_ARGBimage.argb_Width, \
		img->iargb_ARGBimage.argb_Height));

	if (img->iargb_ScaledARGBImage.argb_ImageData)
		ImgHeader = &img->iargb_ScaledARGBImage;
	else
		ImgHeader = &img->iargb_ARGBimage;

	d1(KPrintF("%s/%s/%ld:  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	if ((inst->iobj_imgleft + ImgHeader->argb_Width) > gg->BoundsWidth)
		{
		d1(KPrintF("%s/%s/%ld:  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		}

	// Generate iobj_normmaskbm+iobj_selmaskbm from ARGB image
	GenMasksFromARGB(cl, o);

	d1(KPrintF("%s/%s/%ld:  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	// Extract alpha channel data from ARGB image
	GenAlphaFromARGB(cl, o, img, ImgHeader);

	d1(KPrintF("%s/%s/%ld:  END o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
}

//-----------------------------------------------------------------------------

static BOOL ScaleARGB(Class *cl, Object *o, struct IconObjectARGB *img,
	struct IconObjectMask *Mask, ULONG NewWidth, ULONG NewHeight, ULONG ScaleFlags)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	BOOL Success = FALSE;

	ScalosGfxFreeARGB(&img->iargb_ScaledARGBImage.argb_ImageData);

	img->iargb_ScaledARGBImage.argb_Width = NewWidth;
	img->iargb_ScaledARGBImage.argb_Height = NewHeight;

	img->iargb_ScaledARGBImage.argb_ImageData = ScalosGfxScaleARGBArrayTags(&img->iargb_ARGBimage,
		&img->iargb_ScaledARGBImage.argb_Width,
		&img->iargb_ScaledARGBImage.argb_Height,
		SCALOSGFX_ScaleARGBArrayFlags, ScaleFlags,
		TAG_END);

	d1(KPrintF("%s/%s/%ld:  NewWidth=%lu  NewHeight=%lu\n", __FILE__, __FUNC__, __LINE__, img->iargb_ScaledARGBImage.argb_Width, img->iargb_ScaledARGBImage.argb_Height));
	d1(KPrintF("%s/%s/%ld:  NewARGB=%08lx\n", __FILE__, __FUNC__, __LINE__, img->iargb_ScaledARGBImage.argb_ImageData));

	// Mask and Alpha are automatically recreated during IDTM_Layout

	if (img->iargb_ScaledARGBImage.argb_ImageData)
		{
		Success = TRUE;

		Mask->iom_Width = inst->iobj_SelectedMask.iom_Width = img->iargb_ScaledARGBImage.argb_Width;
		Mask->iom_Height = inst->iobj_SelectedMask.iom_Height = img->iargb_ScaledARGBImage.argb_Height;
		d1(KPrintF("%s/%s/%ld:  Mask->Width=%lu  Mask->Height=%lu\n", __FILE__, __FUNC__, __LINE__, Mask->iom_Width, Mask->iom_Height));
		}

	return Success;
}

//-----------------------------------------------------------------------------

static void EraseIconText(Class *cl, Object *o, struct RastPort *rp, WORD x, WORD y)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	WORD TextLeft = x - inst->iobj_GlobalTextLeftOffset + 1;
	WORD TextRight = TextLeft + inst->iobj_TextExtent.te_Width - 1;
	WORD TextTop = y + gg->Height + inst->iobj_textskip;
	WORD TextBottom = TextTop + inst->iobj_TextExtent.te_Height - 1;

	if (inst->iobj_SelectedTextRectangle)
		{
//		  TextLeft -= inst->iobj_TextRectBorderX;
		TextRight += 2 * inst->iobj_TextRectBorderX;
		TextBottom += 2 * inst->iobj_TextRectBorderY;
		}

	EraseRect(rp,
		TextLeft, TextTop,
		TextRight, TextBottom);
}

//-----------------------------------------------------------------------------

static void DrawIconTextRect(Class *cl, Object *o, struct RastPort *rp, WORD x, WORD y)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	if (inst->iobj_SelectedTextRectangle)
		{
		PLANEPTR myPlanePtr;
		LONG bmWidth = GetBitMapAttr(rp->BitMap, BMA_WIDTH);
		LONG bmHeight = GetBitMapAttr(rp->BitMap, BMA_HEIGHT);

		do	{
			struct ExtGadget *gg = (struct ExtGadget *) o;
			WORD AreaBuffer[AREAMAX];
			struct TmpRas myTmpRas;
			struct AreaInfo AInfo;
			struct RastPort myRp = *rp;
			WORD TextLeft = x - inst->iobj_GlobalTextLeftOffset + 1;
			WORD TextRight = TextLeft + inst->iobj_TextExtent.te_Width + (2 * inst->iobj_TextRectBorderX) - 1;
			WORD TextTop = y + gg->Height + inst->iobj_textskip;
			WORD TextBottom = TextTop + inst->iobj_TextExtent.te_Height + (2 * inst->iobj_TextRectBorderY) - 1;
			UWORD Radius = inst->iobj_TextRectRadius;

			myPlanePtr = AllocRaster(bmWidth, bmHeight);
			if (myPlanePtr == NULL)
				break;

			InitTmpRas(&myTmpRas, myPlanePtr, RASSIZE(bmWidth, bmHeight));
			myRp.TmpRas = &myTmpRas;

			InitArea(&AInfo, AreaBuffer, (AREAMAX * sizeof(WORD)) / 5);
			myRp.AreaInfo = &AInfo;

			SetABPenDrMd(&myRp,
				inst->iobj_TextPenBgSel,
				0,
				JAM1);

			//  1-----2		TextTop
			// /       \            .
			// 8       3            .
			// |       |            .
			// 7       4            .
			//  \     /             .
			//   6---5              TextBottom

			AreaMove(&myRp, TextLeft + Radius, TextTop);			// 1

			AreaDraw(&myRp, TextRight - Radius,	TextTop);       	// -> 2
			AreaDraw(&myRp, TextRight, 		TextTop + Radius);      // -> 3
			AreaDraw(&myRp, TextRight, 	       	TextBottom - Radius);   // -> 4
			AreaDraw(&myRp, TextRight - Radius,     TextBottom);    	// -> 5

			AreaDraw(&myRp, TextLeft + Radius,      TextBottom);	     	// -> 6
			AreaDraw(&myRp, TextLeft, 	        TextBottom - Radius);   // -> 7
			AreaDraw(&myRp, TextLeft, 	        TextTop + Radius);      // -> 8

			AreaEnd(&myRp);

			InitArea(&AInfo, AreaBuffer, (AREAMAX * sizeof(WORD)) / 5);

			AreaCircle(&myRp, TextLeft  + Radius,   TextTop + Radius, Radius);						     // top left
			AreaCircle(&myRp, TextRight - Radius,   TextTop + Radius, Radius);	// top right
			AreaCircle(&myRp, TextLeft  + Radius, 	TextBottom - Radius, Radius);					     // bottom left
			AreaCircle(&myRp, TextRight - Radius,   TextBottom - Radius, Radius);   // bottom right

			AreaEnd(&myRp);

			d1(KPrintF("%s/%s/%ld:  iobj_TextPenBgSel=%ld\n", __FILE__, __FUNC__, __LINE__, inst->iobj_TextPenBgSel));
			} while (0);

		if (myPlanePtr)
			FreeRaster(myPlanePtr, bmWidth, bmHeight);
		}
	else
		{
		EraseIconText(cl, o, rp, x, y);
		}
}

//-----------------------------------------------------------------------------
static void DumpMaskPlane(const struct IconObjectMask *Mask)
{
	LONG x, y;
	const UBYTE *mp;

	if (!Mask || !Mask->iom_MaskBM || !Mask->iom_MaskBM->Planes[0])
		return;

	mp = Mask->iom_MaskBM->Planes[0];

	for (y = 0; y < Mask->iom_Height; y++)
		{
		KPrintF("%s/%s/%ld:  ", __FILE__, __FUNC__, __LINE__);

		for (x = 0; x < Mask->iom_Width; x++)
			{
			UBYTE px = mp[x / 8] & (0x80 >> (x % 8));

			KPrintF("%ld", (px ? 1 : 0));
			}
		KPrintF("\n");

		mp += BYTESPERROW(Mask->iom_Width);
		}
}
//-----------------------------------------------------------------------------
static void DumpMask(const struct IconObjectMask *Mask)
{
	LONG x, y;
	const UBYTE *mp;

	if (!Mask || !Mask->iom_Mask)
		return;

	mp = Mask->iom_Mask;

	for (y = 0; y < Mask->iom_Height; y++)
		{
		KPrintF("%s/%s/%ld:  ", __FILE__, __FUNC__, __LINE__);

		for (x = 0; x < Mask->iom_Width; x++)
			{
			UBYTE px = mp[x / 8] & (0x80 >> (x % 8));

			KPrintF("%ld", (px ? 1 : 0));
			}
		KPrintF("\n");

		mp += BYTESPERROW(Mask->iom_Width);
		}
}

//-----------------------------------------------------------------------------

static void DumpMaskBM(const struct IconObjectMask *Mask)
{
	LONG x, y;
	const UBYTE *mp;
	struct RastPort rp;

	if (!Mask || !Mask->iom_Mask)
		return;

	mp = Mask->iom_Mask;

	InitRastPort(&rp);
	rp.BitMap = Mask->iom_MaskBM;

	for (y = 0; y < Mask->iom_Height; y++)
		{
		KPrintF("%s/%s/%ld:  ", __FILE__, __FUNC__, __LINE__);

		for (x = 0; x < Mask->iom_Width; x++)
			{
			UBYTE px = ReadPixel(&rp, x, y);

			KPrintF("%ld", (px ? 1 : 0));
			}
		KPrintF("\n");

		mp += BYTESPERROW(Mask->iom_Width);
		}
}
//-----------------------------------------------------------------------------

#if !defined(__SASC)
// Replacement for SAS/C library functions

void exit(int x)
{
   (void) x;
   while (1)
      ;
}

#endif /* !defined(__SASC) */
//-----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2INITLIB(InitDatatype, 0);
ADD2EXPUNGELIB(CloseDatatype, 0);
ADD2OPENLIB(OpenDatatype, 0);

#endif
//-----------------------------------------------------------------------------
