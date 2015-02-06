// TextIconClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <libraries/mcpgfx.h>
#include <utility/pack.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <cybergraphx/cybergraphics.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/cybergraphics.h>
#include "debug.h"
#include <proto/scalosgfx.h>
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "TextIconHighlightClass.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

#define	TIDTA_FIRSTTAG	(TIDTA_WidthArray - 1)
#define	IDTA_FIRSTTAG	(IDTA_Type - 1)


struct TextIColumn
	{
	STRPTR tic_String;			// pointer to the contents
	WORD tic_Width;				// Width in pixels
	WORD tic_Chars;				// Number of chars;
	};

struct TextIClassInst
	{
	ULONG	txicl_TextStyle;		// Text style for icon text

	struct TextIColumn txicl_positions[TXICOL_MAX];

	struct ReadIconListData txicl_rild;

	struct TypeNode	*txicl_TypeNode;	// DefIcons TypeNode or Workbench icon type (e.g. WBDISK)

	UWORD   txicl_PenNormal;
	UWORD   txicl_PenSelected;
	UWORD   txicl_PenDrawersNormal;
	UWORD   txicl_PenDrawersSelected;
	UWORD	txicl_PenShadow;
	UWORD	txicl_PenOutline;
	UWORD	txicl_LeftRightSpace;		// Space left and right of text columns

	ULONG	txicl_UserFlags;		// IDTA_UserFlags

	struct	TextFont *txicl_Font;		// font to use or NULL
	struct TTFontFamily *txicl_TTFont;	// ttEngine font to use or NULL

	WORD	*txicl_widtharray;		//pointer to an array of words

	BYTE	txicl_size[16];			//Size string
	BYTE	txicl_prot[10];			//Protection bits string
	BYTE	txicl_date[80];			//Date string
	BYTE	txicl_time[80];			//Time string
	BYTE	txicl_owner[80];		//Owner string
	BYTE	txicl_group[80];		//Group string
	BYTE	txicl_filetype[80];		//FileType string
	BYTE	txicl_version[80];		//Version string
	BYTE	txicl_miniicon[2];		//Mini-Icon string

	UBYTE	txicl_HitOnNameOnly;		// flag: only NAME column reacts to mouse hits
	UBYTE	txicl_TextMode;

	struct Rectangle txicl_IconRect;	// size limits for mini-icon

	struct Hook *ixicl_ColWidthChangeHook;	// Hook to be called when column width is changed

	struct internalScaWindowTask *txicl_WindowTask;
	Object	*txicl_IconObject;
	Object	*txicl_Highlight;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) INTERRUPT TextIconClassDispatcher(Class *cl, Object *o, Msg msg);
static ULONG TextIcon_New(Class *cl, Object *o, Msg msg);
static ULONG TextIcon_Dispose(Class *cl, Object *o, Msg msg);
static ULONG TextIcon_Set(Class *cl, Object *o, Msg msg);
static ULONG TextIcon_Get(Class *cl, Object *o, Msg msg);
static ULONG TextIcon_Layout(Class *cl, Object *o, Msg msg);
static ULONG TextIcon_Draw(Class *cl, Object *o, Msg msg);
static ULONG TextIcon_HitTest(Class *cl, Object *o, Msg msg);
static void SetAttributes(struct TextIClassInst *inst, struct opSet *msg);
static void DimRect(struct RastPort *rp,
	struct ARGB Numerator, struct ARGB Denominator,
	WORD xMin, WORD yMin, WORD xMax, WORD yMax);
static Object *CreateIconObject(struct TextIClassInst *inst, struct TagItem *tagList);
static BOOL GetTextColumnX(Class *cl, Object *o, struct Rectangle *TextColumnRect);
static void DrawMiniIcon(struct TextIClassInst *inst,
	struct iopDraw *iop, WORD x, WORD y);
static BOOL IsColumnEnabled(ULONG ColumnIndex);
static void TextIcon_DrawText(struct TextIClassInst *inst, struct RastPort *rp,
	ULONG x, ULONG y, ULONG PosIndex, UBYTE TextColor, UBYTE FgColorUnselected);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

Class *initTextIconClass(void)
{
	Class *TextIconClass;

	TextIconClass = MakeClass( NULL,
			(STRPTR) "gadgetclass",
			NULL,
			sizeof(struct TextIClassInst),
			0 );

	if (TextIconClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(TextIconClass->cl_Dispatcher,  TextIconClassDispatcher);
		}

	return TextIconClass;
}


static SAVEDS(ULONG) INTERRUPT TextIconClassDispatcher(Class *cl, Object *o, Msg msg)
{
	struct TextIClassInst *inst;
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = TextIcon_New(cl, o, msg);
		break;
	case OM_DISPOSE:
		Result = TextIcon_Dispose(cl, o, msg);
		break;
	case OM_SET:
		Result = TextIcon_Set(cl, o, msg);
		break;
	case OM_GET:
		Result = TextIcon_Get(cl, o, msg);
		break;
	case IDTM_Layout:
		Result = TextIcon_Layout(cl, o, msg);
		break;
	case IDTM_Draw:
		Result = TextIcon_Draw(cl, o, msg);
		break;
	case GM_HITTEST:
		Result = TextIcon_HitTest(cl, o, msg);
		break;

	default:
		inst = INST_DATA(cl, o);

		if (NULL == inst->txicl_IconObject)
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			inst->txicl_IconObject = CreateIconObject(inst, NULL);
			}
		d1(KPrintF("%s/%s/%ld: txicl_IconObject=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_IconObject));
		if (inst->txicl_IconObject)
			Result = DoMethodA(inst->txicl_IconObject, msg);
		else
			Result = DoSuperMethodA(cl, o, msg);
		d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		break;
		}

	return Result;
}


static ULONG TextIcon_New(Class *cl, Object *o, Msg msg)
{
	struct TextIClassInst *inst;
	struct opSet *ops = (struct opSet *) msg;

	o = (Object *) DoSuperMethodA(cl, o, msg);
	if (NULL == o)
		return 0;

	inst = INST_DATA(cl, o);

	memset(inst, 0, sizeof(struct TextIClassInst));

	inst->txicl_Highlight = NewObject(TextIconHighlightClass, NULL,
		TIHA_BaseColor, &CurrentPrefs.pref_SelectMarkerBaseColor,
		TIHA_Transparency, (ULONG) CurrentPrefs.pref_SelectMarkerTransparency,
		TAG_END);
	d1(KPrintF("%s/%s/%ld: txicl_Highlight=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_Highlight));
	if (NULL == inst->txicl_Highlight)
		{
		DoMethod(o, OM_DISPOSE);
		return 0;
		}

	inst->txicl_LeftRightSpace = 3;

	inst->txicl_positions[TXICOL_Name].tic_String = (STRPTR)inst->txicl_rild.rild_Name;
	inst->txicl_positions[TXICOL_Size].tic_String = (STRPTR)inst->txicl_size;
	inst->txicl_positions[TXICOL_Prot].tic_String = (STRPTR)inst->txicl_prot;
	inst->txicl_positions[TXICOL_Date].tic_String = (STRPTR)inst->txicl_date;
	inst->txicl_positions[TXICOL_Time].tic_String = (STRPTR)inst->txicl_time;
	inst->txicl_positions[TXICOL_Comment].tic_String = (STRPTR)inst->txicl_rild.rild_Comment;
	inst->txicl_positions[TXICOL_Owner].tic_String = (STRPTR)inst->txicl_owner;
	inst->txicl_positions[TXICOL_Group].tic_String = (STRPTR)inst->txicl_group;
	inst->txicl_positions[TXICOL_FileType].tic_String = (STRPTR)inst->txicl_filetype;
	inst->txicl_positions[TXICOL_Version].tic_String = (STRPTR)inst->txicl_version;
	inst->txicl_positions[TXICOL_MiniIcon].tic_String = (STRPTR)inst->txicl_miniicon;

	inst->txicl_rild.rild_SoloIcon = FALSE;
	inst->txicl_HitOnNameOnly = TRUE;

	inst->txicl_IconObject = NULL;
	inst->ixicl_ColWidthChangeHook = NULL;
	inst->txicl_WindowTask = NULL;

	inst->txicl_rild.rild_IconType = WBDISK;
	inst->txicl_TypeNode = NULL;
	inst->txicl_TextStyle = FS_NORMAL;
	inst->txicl_TextMode = IDTV_TextMode_Normal;

	inst->txicl_PenNormal = PalettePrefs.pal_PensList[PENIDX_FILETEXT];
	inst->txicl_PenSelected = PalettePrefs.pal_PensList[PENIDX_FILETEXTSEL];
	inst->txicl_PenDrawersNormal = PalettePrefs.pal_PensList[PENIDX_DRAWERTEXT];
	inst->txicl_PenDrawersSelected = PalettePrefs.pal_PensList[PENIDX_DRAWERTEXTSEL];
	inst->txicl_PenShadow = PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN];
	inst->txicl_PenOutline = PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN];

	SetAttributes(inst, ops);

	d1(KPrintF("%s/%s/%ld: name=<%s>  comment=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, inst->txicl_rild.rild_Name, inst->txicl_rild.rild_Comment));

	return (ULONG) o;
}


static ULONG TextIcon_Dispose(Class *cl, Object *o, Msg msg)
{
	struct TextIClassInst *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld: name=<%s>  comment=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, inst->txicl_rild.rild_Name, inst->txicl_rild.rild_Comment));

	if (inst->txicl_IconObject)
		{
		DisposeIconObject(inst->txicl_IconObject);
		inst->txicl_IconObject = NULL;
		}
	if (inst->txicl_Highlight)
		{
		DisposeObject(inst->txicl_Highlight);
		inst->txicl_Highlight = NULL;
		}
		
	return DoSuperMethodA(cl, o, msg);
}


static ULONG TextIcon_Set(Class *cl, Object *o, Msg msg)
{
	struct TextIClassInst *inst = INST_DATA(cl, o);

	DoSuperMethodA(cl, o, msg);

	if (inst->txicl_IconObject)
		DoMethodA(inst->txicl_IconObject, msg);

	SetAttributes(inst, (struct opSet *) msg);

	return 0;
}


static ULONG TextIcon_Get(Class *cl, Object *o, Msg msg)
{
	struct TextIClassInst *inst = INST_DATA(cl, o);
	struct opGet *opg = (struct opGet *) msg;
	ULONG Success = TRUE;

	DoSuperMethodA(cl, o, msg);

	if (NULL == opg->opg_Storage)
		return 0;

	*(opg->opg_Storage) = 0;

	switch (opg->opg_AttrID)
		{
	case TIDTA_TextStyle:
		*(opg->opg_Storage) = inst->txicl_TextStyle;
		break;

	case IDTA_UserFlags:
		*(opg->opg_Storage) = inst->txicl_UserFlags;
		break;

	case DTA_Name:
	case IDTA_Text:
		d1(KPrintF("%s/%s/%ld: DTA_Name=<%s> %08lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_rild.rild_Name, inst->txicl_rild.rild_Name));
		*(opg->opg_Storage) = (ULONG) inst->txicl_rild.rild_Name;
		break;

	case TIDTA_FileType:
		*(opg->opg_Storage) = inst->txicl_rild.rild_Type;
		break;

	case TIDTA_Size:
		*(opg->opg_Storage) = ULONG64_LOW(inst->txicl_rild.rild_Size64);
		break;

	case TIDTA_Protection:
		*(opg->opg_Storage) = inst->txicl_rild.rild_Protection;
		break;

	case TIDTA_Days:
		*(opg->opg_Storage) = inst->txicl_rild.rild_DateStamp.ds_Days;
		break;

	case TIDTA_Mins:
		*(opg->opg_Storage) = inst->txicl_rild.rild_DateStamp.ds_Minute;
		break;

	case TIDTA_Ticks:
		*(opg->opg_Storage) = inst->txicl_rild.rild_DateStamp.ds_Tick;
		break;

	case TIDTA_Comment:
		*(opg->opg_Storage) = (ULONG) inst->txicl_rild.rild_Comment;
		break;

	case TIDTA_Owner_UID:
		*(opg->opg_Storage) = (ULONG) inst->txicl_rild.rild_OwnerUID;
		break;

	case TIDTA_Owner_GID:
		*(opg->opg_Storage) = (ULONG) inst->txicl_rild.rild_OwnerGID;
		break;

	case TIDTA_Font:
		*(opg->opg_Storage) = (ULONG) inst->txicl_Font;
		break;

	case TIDTA_TTFont:
		*(opg->opg_Storage) = (ULONG) inst->txicl_TTFont;
		break;

	case TIDTA_IconObject:
		*(opg->opg_Storage) = (ULONG) inst->txicl_IconObject;
		break;

	case TIDTA_SoloIcon:
		*(opg->opg_Storage) = (ULONG) inst->txicl_rild.rild_SoloIcon;
		break;

	case TIDTA_ColumnWidthChangeHook:
		*(opg->opg_Storage) = (ULONG) inst->ixicl_ColWidthChangeHook;
		break;

	case TIDTA_WidthArray:
		*(opg->opg_Storage) = (ULONG) inst->txicl_widtharray;
		break;

	case TIDTA_SelectNameOnly:
		*(opg->opg_Storage) = (ULONG) inst->txicl_HitOnNameOnly;
		break;

	case TIDTA_TextPenFileNormal:
		*(opg->opg_Storage) = (ULONG) inst->txicl_PenNormal;
		break;

	case TIDTA_TextPenFileSelected:
		*(opg->opg_Storage) = (ULONG) inst->txicl_PenSelected;
		break;

	case TIDTA_TextPenDrawerNormal:
		*(opg->opg_Storage) = (ULONG) inst->txicl_PenDrawersNormal;
		break;

	case TIDTA_TextPenDrawerSelected:
		*(opg->opg_Storage) = (ULONG) inst->txicl_PenDrawersSelected;
		break;

	case IDTA_TextPenShadow:
		*(opg->opg_Storage) = (ULONG) inst->txicl_PenShadow;
		break;

	case IDTA_TextPenOutline:
		*(opg->opg_Storage) = (ULONG) inst->txicl_PenOutline;
		break;

	case IDTA_TextMode:
		*(opg->opg_Storage) = (ULONG) inst->txicl_TextMode;
		break;

	case IDTA_Type:
	case TIDTA_IconType:
		d1(KPrintF("%s/%s/%ld: <%s>  IconType=%ld  txicl_rild.rild_Type=%ld\n", \
			__FILE__, __FUNC__, __LINE__, inst->txicl_rild.rild_Name, inst->txicl_rild.rild_IconType, inst->txicl_rild.rild_Type));
		*(opg->opg_Storage) = inst->txicl_rild.rild_IconType;
		break;

	case TIDTA_TypeNode:
		if (!IS_TYPENODE(inst->txicl_TypeNode))
			{
			inst->txicl_TypeNode = DefIconsIdentify(inst->txicl_WindowTask->iwt_WindowTask.mt_WindowStruct->ws_Lock,
				(STRPTR)inst->txicl_rild.rild_Name, ICONTYPE_NONE);
			}

		*(opg->opg_Storage) = inst->txicl_TypeNode ? (ULONG) inst->txicl_TypeNode : inst->txicl_rild.rild_IconType;
		break;

	case IDTA_MaskBM_Selected:
	case IDTA_MaskBM_Normal:
	case IDTA_AlphaChannel:
		*(opg->opg_Storage) = (ULONG) NULL;
		break;

	default:
		Success = FALSE;

		if (NULL == inst->txicl_IconObject)
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			inst->txicl_IconObject = CreateIconObject(inst, NULL);
			}

		if (inst->txicl_IconObject)
			DoMethodA(inst->txicl_IconObject, msg);

		DoSuperMethodA(cl, o, msg);
		break;
		}

	return Success;
}


static ULONG TextIcon_Layout(Class *cl, Object *o, Msg msg)
{
	struct iopLayout *iop = (struct iopLayout *) msg;
	struct TextIClassInst *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	short n;
	WORD *pWidth = inst->txicl_widtharray;
	BOOL colWidthChanged = FALSE;
	ULONG LastAttr = FS_NORMAL;

	DoSuperMethodA(cl, o, msg);

	gg->GadgetRender = (APTR) 1;
	gg->SelectRender = (APTR) 1;
	gg->Width = 0;

	d1(KPrintF("%s/%s/%ld: txicl_TTFont=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_TTFont));

	Scalos_SetFont(iop->iopl_RastPort, inst->txicl_Font, inst->txicl_TTFont);

	gg->Height = gg->BoundsHeight = inst->txicl_WindowTask->iwt_TextWindowLineHeight;

	for (n=0; n<TXICOL_MAX; n++)
		{
		WORD Length = 0;
		ULONG Attr = FS_NORMAL;

		if (TXICOL_Name == n)
			{
			Attr = inst->txicl_TextStyle;

			d1(KPrintF("%s/%s/%ld: txicl_rild.rild_Type=%ld  Attr=%04lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_rild.rild_Type, Attr));
			}

		if (LastAttr != Attr)
			{
			Scalos_SetSoftStyle(iop->iopl_RastPort, 
				Attr, FSF_BOLD | FSF_ITALIC | FSF_UNDERLINED | FSF_EXTENDED, 
				inst->txicl_TTFont);
			LastAttr = Attr;
			}

		inst->txicl_positions[n].tic_Chars = strlen(inst->txicl_positions[n].tic_String);

		inst->txicl_positions[n].tic_Width = 0;
		if (inst->txicl_positions[n].tic_Chars > 0)
			{
			struct TextExtent textExtent;

			Scalos_TextExtent(iop->iopl_RastPort, 
				inst->txicl_positions[n].tic_String, inst->txicl_positions[n].tic_Chars,
				&textExtent);

			Length = inst->txicl_positions[n].tic_Width = textExtent.te_Width + 2 * inst->txicl_LeftRightSpace;
			Length += 8;

			if (gg->Height < textExtent.te_Height + 1)
				gg->Height = gg->BoundsHeight = textExtent.te_Height + 1;
			}
		gg->Width += Length + 4;

		d1(KPrintF("%s/%s/%ld: String=<%s> Chars=%ld  Width=%ld\n", __FILE__, __FUNC__, __LINE__, \
			inst->txicl_positions[n].tic_String, inst->txicl_positions[n].tic_Chars, \
			inst->txicl_positions[n].tic_Width));

		d1(KPrintF("%s/%s/%ld: pWidth=%08lx\n", __FILE__, __FUNC__, __LINE__, pWidth));

		if (pWidth)
			{
			if (Length > *pWidth)
				{
				*pWidth = Length;
				if (!colWidthChanged && inst->ixicl_ColWidthChangeHook)
					CallHookPkt(inst->ixicl_ColWidthChangeHook, inst->txicl_WindowTask, o);

				colWidthChanged = TRUE;
				}

			pWidth++;
			}
		}

	d1(KPrintF("%s/%s/%ld: gg_Width=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width));

	gg->BoundsWidth = gg->Width;

	return 1;
}


static ULONG TextIcon_Draw(Class *cl, Object *o, Msg msg)
{
#if (0)
	static const struct ARGB NumFile = { (UBYTE) ~0, 70, 70, 70 };
	static const struct ARGB DenomFile =  { (UBYTE) ~0, 80, 80, 80 };
	static const struct ARGB NumDrawer = { (UBYTE) ~0, 65, 70, 65 };
	static const struct ARGB DenomDrawer =  { (UBYTE) ~0, 80, 80, 80 };
#endif
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct iopDraw *iop = (struct iopDraw *) msg;
	struct TextIClassInst *inst = INST_DATA(cl, o);
	struct Rectangle TextColumnRect;
	const WORD *pWidth;
	LONG x, y, xRight;
	UBYTE FgColorUnselected, FgColor;
	UBYTE BgColor;
	WORD FontBaseline;
#if (0)
	struct ARGB Num, Denom;
#endif
	ULONG BmRight;
	struct Rectangle HighlightRect;

	x = iop->iopd_XOffset;
	y = iop->iopd_YOffset;

	xRight = x;

	if (!(iop->iopd_DrawFlags & IODRAWF_AbsolutePos))
		{
		x += gg->BoundsLeftEdge;
		y += gg->BoundsTopEdge;
		}

	if (iop->iopd_DrawFlags & IODRAWF_NoText)
		{
		pWidth = inst->txicl_widtharray;

		if (pWidth)
			xRight += pWidth[TXICOL_Name];
		else
			xRight += inst->txicl_positions[TXICOL_Name].tic_Width;
		}
	else
		xRight += gg->Width;

	xRight--;
	HighlightRect.MaxY = y + gg->Height - 1;

	BgColor = PalettePrefs.pal_PensList[(inst->txicl_rild.rild_Type < 0) ? PENIDX_FILEBG : PENIDX_DRAWERBG];

	SetAPen(iop->iopd_RastPort, BgColor);
	SetDrMd(iop->iopd_RastPort, JAM1);

	d1(KPrintF("%s/%s/%ld: txicl_TTFont=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_TTFont));

	HighlightRect.MinY = y;

	if (!(iop->iopd_DrawFlags & IODRAWF_NoText)
		&& inst->txicl_HitOnNameOnly
		&& GetTextColumnX(cl, o, &TextColumnRect))
		{
		HighlightRect.MinX = TextColumnRect.MinX - inst->txicl_WindowTask->iwt_WindowTask.wt_XOffset;
		HighlightRect.MaxX = TextColumnRect.MaxX - inst->txicl_WindowTask->iwt_WindowTask.wt_XOffset;

		if (HighlightRect.MinX < 0)
			HighlightRect.MinX = 0;
		if (HighlightRect.MaxX < 0)
			HighlightRect.MaxX = 0;

		if (!(iop->iopd_DrawFlags & IODRAWF_AbsolutePos))
			{
			HighlightRect.MinX += inst->txicl_WindowTask->iwt_InnerLeft;
			HighlightRect.MaxX += inst->txicl_WindowTask->iwt_InnerLeft;
			}
		}
	else
		{
		HighlightRect.MinX = x;
		HighlightRect.MaxX = xRight;
		}

	d1(KPrintF("%s/%s/%ld: HighlightRect.MinX=%ld  HighlightRect.MaxX=%ld\n", __FILE__, __FUNC__, __LINE__, HighlightRect.MinX, HighlightRect.MaxX));

#if (0)
	Num = (inst->txicl_rild.rild_Type < 0) ? NumFile : NumDrawer;
	Denom = (inst->txicl_rild.rild_Type < 0) ? DenomFile : DenomDrawer;
#endif

	BmRight = GetBitMapAttr(iop->iopd_RastPort->BitMap, BMA_WIDTH);

	EraseRect(iop->iopd_RastPort,
		x,
		HighlightRect.MinY,
		BmRight,
                HighlightRect.MaxY);

	if (gg->Flags & GFLG_SELECTED)
		{
		SetAttrs(inst->txicl_Highlight,
			IA_Left, HighlightRect.MinX,
			IA_Top, HighlightRect.MinY,
			IA_Width, 1 + HighlightRect.MaxX - HighlightRect.MinX + 2,
			IA_Height, 1 + HighlightRect.MaxY - HighlightRect.MinY,
			IA_FGPen, BgColor,
			TIHA_Transparency, (ULONG) CurrentPrefs.pref_SelectMarkerTransparency,
			TAG_END);
		DrawImage(iop->iopd_RastPort, (struct Image *) inst->txicl_Highlight, 0, 0);
		};

	if (inst->txicl_UserFlags & ICONOBJ_USERFLAGF_DrawHighlite)
		{
		// Highlight select area
		ULONG ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

		if (CyberGfxBase && (ScreenDepth > 8))
			{
			UBYTE Transparency = (gg->Flags & GFLG_SELECTED) ? 255 : (CurrentPrefs.pref_SelectMarkerTransparency / 2);

			SetAttrs(inst->txicl_Highlight,
				IA_Left, HighlightRect.MinX,
				IA_Top, HighlightRect.MinY,
				IA_Width, 1 + HighlightRect.MaxX - HighlightRect.MinX + 2,
				IA_Height, 1 + HighlightRect.MaxY - HighlightRect.MinY,
				IA_FGPen, BgColor,
				TIHA_Transparency, (ULONG) Transparency,
				TAG_END);
			DrawImage(iop->iopd_RastPort, (struct Image *) inst->txicl_Highlight, 0, 0);
			}
		else
			{
			static const struct ARGB NumHilight = { (UBYTE) ~0, 100, 100, 100 };
			static const struct ARGB DenomHilight =  { (UBYTE) ~0, 80, 80, 80 };

			DimRect(iop->iopd_RastPort,
				NumHilight, DenomHilight,
				HighlightRect.MinX, HighlightRect.MinY,
				HighlightRect.MaxX, HighlightRect.MaxY);
			}
		}

	if (inst->txicl_rild.rild_Type < 0)
		{
		// file
		FgColor = (gg->Flags & GFLG_SELECTED) ? inst->txicl_PenSelected : inst->txicl_PenNormal;
		FgColorUnselected = inst->txicl_PenNormal;
		}
	else
		{
		// drawer
		FgColor = (gg->Flags & GFLG_SELECTED) ? inst->txicl_PenDrawersSelected : inst->txicl_PenDrawersNormal;
		FgColorUnselected = inst->txicl_PenDrawersNormal;
		}

	Scalos_SetFont(iop->iopd_RastPort, inst->txicl_Font, inst->txicl_TTFont);

	FontBaseline = Scalos_GetFontBaseline(iop->iopd_RastPort) + 1;
	y += FontBaseline;
	x += 1;

	pWidth = inst->txicl_widtharray;

	if (!(iop->iopd_DrawFlags & IODRAWF_NoText))
		{
		const BYTE *pColumn;
		ULONG LastAttr = FS_NORMAL;

		for (pColumn = CurrentPrefs.pref_ColumnsArray; *pColumn >= 0; pColumn++)
			{
			short Ndx = (short)*pColumn;
			WORD x0 = x + inst->txicl_LeftRightSpace;

			d1(KPrintF("%s/%s/%ld: x=%ld  w=%ld\n", __FILE__, __FUNC__, __LINE__, x, pWidth[Ndx]));

			if (pWidth)
				{
				ULONG Attr = FS_NORMAL;

				x += pWidth[Ndx];

				if (TXICOL_Name == Ndx)
					{
					Attr = inst->txicl_TextStyle;

					SetAPen(iop->iopd_RastPort, FgColor);
					d1(KPrintF("%s/%s/%ld: txicl_rild.rild_Type=%ld  Attr=%04lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_rild.rild_Type, Attr));
					}

				if (LastAttr != Attr)
					{
					Scalos_SetSoftStyle(iop->iopd_RastPort, 
						Attr, FSF_BOLD | FSF_ITALIC | FSF_UNDERLINED | FSF_EXTENDED, 
						inst->txicl_TTFont);
					LastAttr = Attr;
					}

				if (TXICOL_Size == Ndx)
					{
					// right alignment for SIZE column
					WORD w = pWidth[Ndx] - inst->txicl_positions[Ndx].tic_Width - 8;

					if (w >= 0)
						x0 += w;
					}
				}
			else
				{
				x += inst->txicl_positions[Ndx].tic_Width;
				}

			if (TXICOL_MiniIcon == Ndx && NULL == inst->txicl_IconObject)
				{
				inst->txicl_IconObject = CreateIconObject(inst, NULL);
				}
			if (TXICOL_MiniIcon == Ndx && inst->txicl_IconObject)
				{
				DrawMiniIcon(inst, iop, x0, y - FontBaseline);
				}
			else
				{
				TextIcon_DrawText(inst, iop->iopd_RastPort, x0, y, Ndx,
					FgColor, FgColorUnselected);
				}
			}
		}
	else
		{
		TextIcon_DrawText(inst, iop->iopd_RastPort, x, y, TXICOL_Name,
			FgColor, FgColorUnselected);
		}

	return 1;
}


static ULONG TextIcon_HitTest(Class *cl, Object *o, Msg msg)
{
	struct gpHitTest *gpht = (struct gpHitTest *) msg;
	struct TextIClassInst *inst = INST_DATA(cl, o);
	ULONG Result;

	Result = DoSuperMethodA(cl, o, msg);

	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	if (GMR_GADGETHIT == Result && inst->txicl_HitOnNameOnly)
		{
		struct Rectangle TextColumnRect;

		if (GetTextColumnX(cl, o, &TextColumnRect))
			{
			d1(KPrintF("%s/%s/%ld: MinX=%ld  MaxX=%ld\n", \
				__FILE__, __FUNC__, __LINE__, TextColumnRect.MinX, TextColumnRect.MaxX));

			if (gpht->gpht_Mouse.X >= TextColumnRect.MinX
				&& gpht->gpht_Mouse.X < TextColumnRect.MaxX)
				{
				Result = GMR_GADGETHIT;
				}
			else
				{
				Result = 0;
				}
			}

		d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		}
	else
		{
		Result = GMR_GADGETHIT;
		}

	return Result;
}


static void SetAttributes(struct TextIClassInst *inst, struct opSet *ops)
{
	struct DateTime dtm;
	const struct ReadIconListData *rild;
	static const ULONG packTable[] =
		{
		PACK_STARTTABLE(TIDTA_FIRSTTAG),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_WidthArray, 	       TextIClassInst, txicl_widtharray, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_TextStyle, 	       TextIClassInst, txicl_TextStyle, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_Font, 		       TextIClassInst, txicl_Font, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_TTFont, 	       TextIClassInst, txicl_TTFont, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_IconType, 	       TextIClassInst, txicl_rild.rild_IconType, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_TypeNode, 	       TextIClassInst, txicl_TypeNode, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_IconObject, 	       TextIClassInst, txicl_IconObject, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_ColumnWidthChangeHook,TextIClassInst, ixicl_ColWidthChangeHook, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_SoloIcon, 	       TextIClassInst, txicl_rild.rild_SoloIcon, PKCTRL_UBYTE | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_SelectNameOnly,       TextIClassInst, txicl_HitOnNameOnly, PKCTRL_UBYTE | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_WindowTask, 	       TextIClassInst, txicl_WindowTask, PKCTRL_ULONG | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_TextPenFileNormal,    TextIClassInst, txicl_PenNormal, PKCTRL_UWORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_TextPenFileSelected,  TextIClassInst, txicl_PenSelected, PKCTRL_UWORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_TextPenDrawerNormal,  TextIClassInst, txicl_PenDrawersNormal, PKCTRL_UWORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(TIDTA_FIRSTTAG, TIDTA_TextPenDrawerSelected,TextIClassInst, txicl_PenDrawersSelected, PKCTRL_UWORD | PKCTRL_PACKUNPACK),

		PACK_NEWOFFSET(IDTA_FIRSTTAG),
		PACK_ENTRY(IDTA_FIRSTTAG, IDTA_TextPenShadow, 	      TextIClassInst, txicl_PenShadow, PKCTRL_UWORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(IDTA_FIRSTTAG, IDTA_TextPenOutline, 	      TextIClassInst, txicl_PenOutline, PKCTRL_UWORD | PKCTRL_PACKUNPACK),
		PACK_ENTRY(IDTA_FIRSTTAG, IDTA_TextMode, 	      TextIClassInst, txicl_TextMode, PKCTRL_UBYTE | PKCTRL_PACKUNPACK),
		PACK_ENTRY(IDTA_FIRSTTAG, IDTA_UserFlags, 	      TextIClassInst, txicl_UserFlags, PKCTRL_ULONG | PKCTRL_PACKUNPACK),

		PACK_ENDTABLE
		};

	PackStructureTags(inst, packTable, ops->ops_AttrList);

	rild = (struct ReadIconListData *) GetTagData(TIDTA_ReadIconListData, (ULONG)NULL, ops->ops_AttrList);

	if (rild)
		{
		inst->txicl_rild = *rild;

		snprintf((char *)inst->txicl_version, sizeof(inst->txicl_version), "TBD");
		snprintf((char *)inst->txicl_miniicon, sizeof(inst->txicl_miniicon), "M");

		if (inst->txicl_rild.rild_SoloIcon)
			{
			stccpy((char *)inst->txicl_size,
				GetLocString(MSGID_TEXTICON_SOLOICON), 
				sizeof(inst->txicl_size));
			}
		else
			{
			if (inst->txicl_rild.rild_Type < 0)
				{
				// size only for files
				Convert64(ScalosLocale, inst->txicl_rild.rild_Size64, (char *)inst->txicl_size, sizeof(inst->txicl_size));
				}
			else
				{
				switch ((ULONG) inst->txicl_rild.rild_IconType)
					{
				case WBGARBAGE:
					stccpy((char *)inst->txicl_size,
						GetLocString(MSGID_TEXTICON_TRASHCAN), 
						sizeof(inst->txicl_size));
					break;
				default:
					stccpy((char *)inst->txicl_size,
						GetLocString(MSGID_TEXTICON_DRAWER), 
						sizeof(inst->txicl_size));
					break;
					}
				}
			}

		GetProtectionString(inst->txicl_rild.rild_Protection, (STRPTR)inst->txicl_prot, sizeof(inst->txicl_prot));

		dtm.dat_Stamp = inst->txicl_rild.rild_DateStamp;
		dtm.dat_Format = FORMAT_DOS;
		dtm.dat_Flags = DTF_SUBST;
		dtm.dat_StrDay = NULL;
		dtm.dat_StrDate = (STRPTR)inst->txicl_date;
		dtm.dat_StrTime = (STRPTR)inst->txicl_time;
		ClassFormatDate(&dtm, sizeof(inst->txicl_date), sizeof(inst->txicl_time));

		snprintf((char *)inst->txicl_owner, sizeof(inst->txicl_owner), "%lu", (unsigned long) inst->txicl_rild.rild_OwnerUID);
		snprintf((char *)inst->txicl_group, sizeof(inst->txicl_group), "%lu", (unsigned long) inst->txicl_rild.rild_OwnerGID);
		snprintf((char *)inst->txicl_filetype, sizeof(inst->txicl_filetype), "TBD");
		snprintf((char *)inst->txicl_version, sizeof(inst->txicl_version), "TBD");
		snprintf((char *)inst->txicl_miniicon, sizeof(inst->txicl_miniicon), "M");
		}

	d1(KPrintF("%s/%s/%ld: <%s>  IconType=%ld  txicl_rild.rild_Type=%ld\n", \
			__FILE__, __FUNC__, __LINE__, inst->txicl_rild.rild_Name, inst->txicl_rild.rild_IconType, inst->txicl_rild.rild_Type));

	if (IsColumnEnabled(TXICOL_FileType))
		{
		if (!IS_TYPENODE(inst->txicl_TypeNode))
			{
			inst->txicl_TypeNode = DefIconsIdentify(inst->txicl_WindowTask->iwt_WindowTask.mt_WindowStruct->ws_Lock,
				(STRPTR)inst->txicl_rild.rild_Name, ICONTYPE_NONE);
			}

		switch ((ULONG) inst->txicl_TypeNode)
			{
		case WBDISK:
			stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_WBDISK), sizeof(inst->txicl_filetype));
			break;
		case WBDRAWER:
		case WB_TEXTICON_DRAWER:
			stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_WBDRAWER), sizeof(inst->txicl_filetype));
			break;
		case WBTOOL:
		case WB_TEXTICON_TOOL:
			stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_WBTOOL), sizeof(inst->txicl_filetype));
			break;
		case WBPROJECT:
			stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_WBPROJECT), sizeof(inst->txicl_filetype));
			break;
		case WBGARBAGE:
			stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_WBGARBAGE), sizeof(inst->txicl_filetype));
			break;
		case WBDEVICE:
			stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_WBDEVICE), sizeof(inst->txicl_filetype));
			break;
		case WBKICK:
			stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_WBKICK), sizeof(inst->txicl_filetype));
			break;
		case WBAPPICON:
			stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_WBAPPICON), sizeof(inst->txicl_filetype));
			break;
		default:
			d1(KPrintF("%s/%s/%ld: txicl_TypeNode=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_TypeNode));
			if (IS_TYPENODE(inst->txicl_TypeNode))
				stccpy((char *)inst->txicl_filetype, inst->txicl_TypeNode->tn_Name, sizeof(inst->txicl_filetype));
			else
				stccpy((char *)inst->txicl_filetype, GetLocString(MSGID_ICONTYPE_UNKNOWN), sizeof(inst->txicl_filetype));
			break;
			}
		}

	switch ((ULONG) inst->txicl_TypeNode)
		{
	case WBDISK:
	case WBPROJECT:
	case WBGARBAGE:
	case WBDEVICE:
	case WBKICK:
	case WBAPPICON:
		inst->txicl_rild.rild_IconType = (ULONG) inst->txicl_TypeNode;
		break;
	case WBDRAWER:
		inst->txicl_rild.rild_IconType = WB_TEXTICON_DRAWER;      // Dir
		break;
	case WBTOOL:
		inst->txicl_rild.rild_IconType = WB_TEXTICON_TOOL;        // File
		break;
	default:
		if (inst->txicl_rild.rild_Type < 0)
			inst->txicl_rild.rild_IconType = WB_TEXTICON_TOOL;	// File
		else
			inst->txicl_rild.rild_IconType = WB_TEXTICON_DRAWER;    // Dir
		}

	d1(KPrintF("%s/%s/%ld: txicl_TTFont=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->txicl_TTFont));
}


static void DimRect(struct RastPort *rp,
	struct ARGB Numerator, struct ARGB Denominator,
	WORD xMin, WORD yMin, WORD xMax, WORD yMax)
{
	ULONG Depth;

	Depth = GetBitMapAttr(rp->BitMap, BMA_DEPTH);

	if (NULL != CyberGfxBase && Depth > 8)
		{
		ScalosGfxARGBRectMult(rp, &Numerator, &Denominator,
			xMin, yMin, xMax, yMax);
		}
}


static Object *CreateIconObject(struct TextIClassInst *inst, struct TagItem *tagList)
{
	BPTR OldDir;
	struct RastPort rp;
	struct TextExtent textExtent;

	// Calculate dimensions for icon
	InitRastPort(&rp);
	Scalos_SetFont(&rp, inst->txicl_Font, inst->txicl_TTFont);
	Scalos_TextExtent(&rp, "M", 1, &textExtent);
	inst->txicl_IconRect.MinX = inst->txicl_IconRect.MinY
		= inst->txicl_IconRect.MaxX = inst->txicl_IconRect.MaxY
                = max(textExtent.te_Width, textExtent.te_Height);
	
        OldDir = CurrentDir(inst->txicl_WindowTask->iwt_WindowTask.mt_WindowStruct->ws_Lock);

	inst->txicl_IconObject = NewIconObjectTags((STRPTR)inst->txicl_rild.rild_Name,
		IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
		IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
		IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
		IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
		IDTA_Text, (ULONG) inst->txicl_rild.rild_Name,
		IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
		IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
		IDTA_FrameTypeSel, MF_FRAME_NONE,
		IDTA_FrameType, MF_FRAME_NONE,
		IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes,
		IDTA_SizeConstraints, (ULONG) &inst->txicl_IconRect,
		IDTA_ScalePercentage, 100,
		TAG_MORE, (ULONG) tagList,
		TAG_END);
	d1(KPrintF("%s/%s/%ld: txicl_rild.rild_Name=<%s>  txicl_IconObject=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, inst->txicl_rild.rild_Name, inst->txicl_IconObject));

	if (NULL == inst->txicl_IconObject)
		{
		// try to get default icon
		inst->txicl_IconObject = SCA_GetDefIconObjectTags(inst->txicl_WindowTask->iwt_WindowTask.mt_WindowStruct->ws_Lock,
				(STRPTR)inst->txicl_rild.rild_Name,
				IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
				IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
				IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
				IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
				IDTA_Text, (ULONG) inst->txicl_rild.rild_Name,
				IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
				IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
				IDTA_FrameTypeSel, MF_FRAME_NONE,
				IDTA_FrameType, MF_FRAME_NONE,
				IDTA_SupportedIconTypes, CurrentPrefs.pref_SupportedIconTypes,
				IDTA_SizeConstraints, (ULONG) &inst->txicl_IconRect,
				IDTA_ScalePercentage, 100,
				TAG_MORE, (ULONG) tagList,
				TAG_END);

		if (inst->txicl_IconObject)
			{
			// default icons inherit ShowAllFiles and ViewMode settings from parent window.
			ULONG IconViewMode = TranslateViewModesToIcon(inst->txicl_WindowTask->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes);
			struct ScaWindowStruct *ws = inst->txicl_WindowTask->iwt_WindowTask.mt_WindowStruct;
			ULONG ddFlags;

			ddFlags = ws->ws_ViewAll;
			if (SortOrder_Ascending == ws->ws_SortOrder)
				ddFlags |= DDFLAGS_SORTASC;
			else if (SortOrder_Descending == ws->ws_SortOrder)
				ddFlags |= DDFLAGS_SORTDESC;

			SetAttrs(inst->txicl_IconObject,
				IDTA_Flags, ddFlags,
				IDTA_ViewModes, IconViewMode,
				TAG_END);
			}
		}

	CurrentDir(OldDir);

	return inst->txicl_IconObject;
}


static BOOL GetTextColumnX(Class *cl, Object *o, struct Rectangle *TextColumnRect)
{
	struct TextIClassInst *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	const BYTE *pColumn;
	const WORD *pWidth;
	ULONG x;

	pWidth = inst->txicl_widtharray;

	TextColumnRect->MinX = TextColumnRect->MaxX = 0;
	TextColumnRect->MinY = TextColumnRect->MaxY = 0;

	x = gg->BoundsLeftEdge;

	for (pColumn = CurrentPrefs.pref_ColumnsArray; *pColumn >= 0; pColumn++)
		{
		short Ndx = (short)*pColumn;

		d1(KPrintF("%s/%s/%ld: x=%ld  w=%ld\n", __FILE__, __FUNC__, __LINE__, x, pWidth[Ndx]));

		if (pWidth)
			{
			if (TXICOL_Name == Ndx)
				{
				d1(KPrintF("%s/%s/%ld: x=%ld  w=%ld\n", __FILE__, __FUNC__, __LINE__, x, pWidth[Ndx]));

				TextColumnRect->MinX = TextColumnRect->MaxX = x;
				TextColumnRect->MaxX += pWidth[Ndx] - 1;

				return TRUE;
				}

			x += pWidth[Ndx];
			}
		}

	return FALSE;
}


static void DrawMiniIcon(struct TextIClassInst *inst,
	struct iopDraw *iop, WORD x, WORD y)
{
	const struct ExtGadget *gg = (const struct ExtGadget *) inst->txicl_IconObject;
	WORD xCenter, yCenter;

	d1(KPrintF("%s/%s/%ld: String=<%s>  iopd_DrawFlags=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, inst->txicl_positions[TXICOL_Name].tic_String, \
                iop->iopd_DrawFlags));

	// Make sure icon is layouted before attempting to draw it
	if (gg->Flags & GFLG_SELECTED)
		{
		if (NULL == gg->SelectRender)
			{
			DoMethod(inst->txicl_IconObject, IDTM_Layout,
				iop->iopd_Screen,
				iop->iopd_Window,
				iop->iopd_RastPort,
				iop->iopd_DrawInfo,
				IOLAYOUTF_SelectedImage);
			}
		}
	else
		{
		if (NULL == gg->GadgetRender)
			{
			DoMethod(inst->txicl_IconObject, IDTM_Layout,
				iop->iopd_Screen,
				iop->iopd_Window,
				iop->iopd_RastPort,
				iop->iopd_DrawInfo,
				IOLAYOUTF_NormalImage);
			}
		}

	xCenter = (inst->txicl_IconRect.MaxX - gg->Width) / 2;
	yCenter = (inst->txicl_IconRect.MaxY - gg->Height) / 2;

	DoMethod(inst->txicl_IconObject,
		IDTM_Draw,
		iop->iopd_Screen,
		iop->iopd_Window,
		iop->iopd_RastPort,
		iop->iopd_DrawInfo,
		x + xCenter, y + yCenter,
		IODRAWF_NoText | IODRAWF_AbsolutePos | IODRAWF_NoEraseBg);
}


static BOOL IsColumnEnabled(ULONG ColumnIndex)
{
	const BYTE *pColumn;

	for (pColumn = CurrentPrefs.pref_ColumnsArray; *pColumn >= 0; pColumn++)
		{
		short Ndx = (short)*pColumn;

		if (ColumnIndex == Ndx)
			return TRUE;
		}

	return FALSE;
}


static void TextIcon_DrawText(struct TextIClassInst *inst, struct RastPort *rp,
	ULONG x, ULONG y, ULONG PosIndex, UBYTE FgColor, UBYTE FgColorUnselected)
{
	if (inst->txicl_HitOnNameOnly && TXICOL_Name != PosIndex)
		FgColor = FgColorUnselected;

	if (IDTV_TextMode_Outline == inst->txicl_TextMode)
		{
		Scalos_SetFontTransparency(rp, 255 / 2);  // 50% transparent
		SetAPen(rp, inst->txicl_PenOutline);

		Move(rp, x, y + 1);
		Scalos_Text(rp, inst->txicl_positions[PosIndex].tic_String,
			inst->txicl_positions[PosIndex].tic_Chars);

		Move(rp, x + 1, y);
		Scalos_Text(rp, inst->txicl_positions[PosIndex].tic_String,
			inst->txicl_positions[PosIndex].tic_Chars);

		Move(rp, x + 1, y + 2);
		Scalos_Text(rp, inst->txicl_positions[PosIndex].tic_String,
			inst->txicl_positions[PosIndex].tic_Chars);

		Move(rp, x + 2, y + 1);
		Scalos_Text(rp, inst->txicl_positions[PosIndex].tic_String,
			inst->txicl_positions[PosIndex].tic_Chars);

		Move(rp, x, y);
		Scalos_Text(rp, inst->txicl_positions[PosIndex].tic_String,
			inst->txicl_positions[PosIndex].tic_Chars);

		SetAPen(rp, FgColor);

		Scalos_SetFontTransparency(rp, 0);	  // opaque
		SetDrMd(rp, JAM1);

		Move(rp, x + 1, y + 1);
		}
	else if (IDTV_TextMode_Shadow == inst->txicl_TextMode)
		{
		SetAPen(rp, inst->txicl_PenShadow);
		Scalos_SetFontTransparency(rp, 255 / 2);  // 50% transparent

		Move(rp, x + 1, y + 1);
		Scalos_Text(rp, inst->txicl_positions[PosIndex].tic_String,
			inst->txicl_positions[PosIndex].tic_Chars);

		SetAPen(rp, FgColor);
		SetDrMd(rp, JAM1);
		Scalos_SetFontTransparency(rp, 0);	  // opaque

		Move(rp, x, y);
		}
	else
		{
		// IDTV_TextMode_Normal
		Move(rp, x, y);
		SetAPen(rp, FgColor);

		d1(KPrintF("%s/%s/%ld: String=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->txicl_positions[PosIndex].tic_String));
		}

	d1(KPrintF("%s/%s/%ld: String=<%s>\n", __FILE__, __FUNC__, __LINE__, inst->txicl_positions[PosIndex].tic_String));

	Scalos_Text(rp, inst->txicl_positions[PosIndex].tic_String,
		inst->txicl_positions[PosIndex].tic_Chars);
}


