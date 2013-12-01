// TTlayout.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <graphics/gfxmacros.h>
#include <datatypes/pictureclass.h>
#include <utility/hooks.h>
#include <libraries/mcpgfx.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/diskfont.h>
#include <proto/ttengine.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"
#include "DefIcons.h"
#include "TTLayout.h"
#include <ScalosMcpGfx.h>

//----------------------------------------------------------------------------

//#define	DUMPLAYOUT

struct FormatStringBuffInfo
	{
	char *fsbi_Buffer;
	size_t fsbi_BuffLength;
	};

//----------------------------------------------------------------------------

static void TTPositionItem(struct ttDef *ttd, struct IBox *layoutBox, ULONG LayoutMode);
static void TTLayoutItem(struct RastPort *rp, struct ttDef *ttd, struct IBox *layoutBox, 
	ULONG LayoutMode, struct Hook *userHook);
static void TTSpreadHoriz(struct ttDef *ttd, WORD newWidth, struct IBox *parentBox);
static void TTSpreadVert(struct ttDef *ttd, WORD newHeight, struct IBox *parentBox);
static SAVEDS(ULONG) TTStringFormatHookFunc(struct Hook *theHook, struct Locale *myLocale,
	char NextChar);

#ifdef DUMPLAYOUT
static void DumpLayout(struct ttDef *ttd);
#endif

//----------------------------------------------------------------------------

struct ttDef *TT_CreateItem(ULONG FirstTag, ...)
{
	va_list args;
	struct ttDef *ttd = NULL;
	struct TagItem *TagList;

	va_start(args, FirstTag);

	TagList = ScalosVTagList(FirstTag, args);
	if (TagList)
		{
		ttd = TT_CreateItemA(TagList);
		FreeTagItems(TagList);
		}

	va_end(args);

	return ttd;
}


struct ttDef *TT_CreateItemA(struct TagItem *TagList)
{
	struct TagItem *ti;
	struct ttDef *ttd;
	CONST_STRPTR Text;

	ttd = ScalosAlloc(sizeof(struct ttDef));
	d1(kprintf("%s/%s/%ld: ALLOC ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd));
	if (NULL == ttd)
		return NULL;

	memset(ttd, 0, sizeof(struct ttDef));

	ttd->ttd_Sub = ttd->ttd_Next = NULL;
	ttd->ttd_FrameType = MF_FRAME_NONE;

	ttd->ttd_SpaceHoriz = TT_HORIZ_SPACE;
	ttd->ttd_SpaceVert = TT_VERT_SPACE;

	ttd->ttd_VertAlign = TTVERTALIGN_Center;

	ttd->ttd_SpaceLeft = 0;
	ttd->ttd_SpaceRight = 0;
	ttd->ttd_SpaceTop = 0;
	ttd->ttd_SpaceBottom = 0;

	if (FindTagItem(TT_TTFont, TagList))
		{
		ttd->ttd_TTFontSpec = NULL;
		memset(&ttd->ttd_TextAttr, 0, sizeof(ttd->ttd_TextAttr));
		}
	else
		{
		if (CurrentPrefs.pref_UseScreenTTFont)
			{
			// Prepare default TT font (= screen font)
			ttd->ttd_TTFontSpec = AllocCopyString(CurrentPrefs.pref_ScreenTTFontDescriptor);

			Scalos_OpenTTFont(ttd->ttd_TTFontSpec, &ttd->ttd_TTFont);
			}
		}

	ttd->ttd_TextPen = PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT];

	while ((ti = NextTagItem(&TagList)))
		{
		switch (ti->ti_Tag)
			{
		case TT_Title:
			Text = (CONST_STRPTR) ti->ti_Data;
			ttd->ttd_Contents.ttc_Text = ScalosAlloc(1 + strlen(Text));
			d1(kprintf("%s/%s/%ld: ALLOC ttc_Text=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Contents.ttc_Text));
			ttd->ttd_Type = TTTYPE_Text;
			if (ttd->ttd_Contents.ttc_Text)
				strcpy(ttd->ttd_Contents.ttc_Text, Text);
			break;

		case TT_TitleHook:
			Text = (CONST_STRPTR) ti->ti_Data;
			ttd->ttd_Contents.ttc_Text = ScalosAlloc(1 + strlen(Text));
			d1(kprintf("%s/%s/%ld: ALLOC ttc_Text=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Contents.ttc_Text));
			ttd->ttd_Type = TTTYPE_TextHook;
			if (ttd->ttd_Contents.ttc_Text)
				strcpy(ttd->ttd_Contents.ttc_Text, Text);
			break;

		case TT_TitleID:
			Text = GetLocString(ti->ti_Data);
			ttd->ttd_Contents.ttc_Text = ScalosAlloc(1 + strlen(Text));
			d1(kprintf("%s/%s/%ld: ALLOC ttc_Text=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Contents.ttc_Text));
			ttd->ttd_Type = TTTYPE_Text;
			if (ttd->ttd_Contents.ttc_Text)
				strcpy(ttd->ttd_Contents.ttc_Text, Text);
			break;

		case TT_DrawFrame:
			ttd->ttd_FrameType = ti->ti_Data;
			d1(kprintf("%s/%s/%ld: FrameType=%ld\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_FrameType));
			break;

		case TT_Space:
			ttd->ttd_Space = ti->ti_Data;
			if (ttd->ttd_Space > 128)
				ttd->ttd_Space = 128;
			ttd->ttd_Type = TTTYPE_Space;
			break;

		case TT_DTImage:
			ttd->ttd_Type = TTTYPE_DTImage;
			ttd->ttd_Contents.ttc_Image = CreateDatatypesImage((STRPTR) ti->ti_Data, 0);
			d1(kprintf("%s/%s/%ld: ALLOC ttc_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Contents.ttc_Image));
			break;

		case TT_Image:
			ttd->ttd_Contents.ttc_Image = (struct DatatypesImage *) ti->ti_Data;
			ttd->ttd_Type = TTTYPE_Image;
			break;

		case TT_WideTitleBar:
			ttd->ttd_Type = TTTYPE_WideTitleBar;
			break;

		case TT_Font:
			if (ttd->ttd_TextAttr.ta_Name)
				FreeCopyString(ttd->ttd_TextAttr.ta_Name);
			ttd->ttd_TextAttr = *((struct TextAttr *) ti->ti_Data);
			ttd->ttd_Font = OpenDiskFont((struct TextAttr *) ti->ti_Data);
			ttd->ttd_TextAttr.ta_Name = AllocCopyString(ttd->ttd_TextAttr.ta_Name);
			d1(kprintf("%s/%s/%ld: TextAttr=%08lx  TextFont=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data, ttd->ttd_Font));
			break;

		case TT_TTFont:
			if (ti->ti_Data)
				{
				if (ttd->ttd_TTFontSpec)
					FreeCopyString(ttd->ttd_TTFontSpec);
				ttd->ttd_TTFontSpec = AllocCopyString((CONST_STRPTR) ti->ti_Data);

				d1(kprintf("%s/%s/%ld: ttd_TTFontSpec=<%s>\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_TTFontSpec));

				Scalos_OpenTTFont(ttd->ttd_TTFontSpec, &ttd->ttd_TTFont);
				}
			d1(kprintf("%s/%s/%ld: TTFontDesc=%08lx  TTFont=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, ti->ti_Data, ttd->ttd_TTFont.ttff_Normal));
			break;

		case TT_Members:
			if (ti->ti_Data)
				{
				struct ttDef **ttSub = &ttd->ttd_Sub;

				while (*ttSub)
					ttSub = &(*ttSub)->ttd_Next;

				*ttSub = (struct ttDef *) ti->ti_Data;
				}

			break;

		case TT_LayoutMode:
			switch (ti->ti_Data)
				{
			case TTL_Horizontal:
			case TTL_Vertical:
				ttd->ttd_LayoutMode = ti->ti_Data;
				break;
				}
			break;

		case TT_TextStyle:
			ttd->ttd_Attributes = ti->ti_Data;
			break;

		case TT_TextPen:
			ttd->ttd_TextPen = ti->ti_Data;
			break;

		case TT_HAlign:
			ttd->ttd_HorizAlign = ti->ti_Data;
			break;

		case TT_VAlign:
			ttd->ttd_VertAlign = ti->ti_Data;
			break;

		case TT_Item:
			if (ti->ti_Data)
				{
				struct ttDef **ttNext = &ttd->ttd_Next;

				while (*ttNext)
					ttNext = &(*ttNext)->ttd_Next;

				*ttNext = (struct ttDef *) ti->ti_Data;
				}
			break;

		case TT_Hidden:
			ttd->ttd_Hidden = (BOOL) ti->ti_Data;
			break;

		case TT_HiddenHook:
			Text = (CONST_STRPTR) ti->ti_Data;
			ttd->ttd_HiddenHookText = ScalosAlloc(1 + strlen(Text));
			d1(kprintf("%s/%s/%ld: ALLOC ttd_HiddenHookText=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_HiddenHookText));
			if (ttd->ttd_HiddenHookText)
				strcpy(ttd->ttd_HiddenHookText, Text);
			d1(kprintf("%s/%s/%ld: ttd_HiddenHookText=<%s>\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_HiddenHookText));
			break;

		case TT_SpaceLeft:
			ttd->ttd_SpaceLeft = ti->ti_Data;
			break;

		case TT_SpaceRight:
			ttd->ttd_SpaceRight = ti->ti_Data;
			break;

		case TT_SpaceTop:
			ttd->ttd_SpaceTop = ti->ti_Data;
			break;

		case TT_SpaceBottom:
			ttd->ttd_SpaceBottom = ti->ti_Data;
			break;
			}
		}

	d1(kprintf("%s/%s/%ld: ttd=%08lx  Type=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, ttd, ttd->ttd_Type, ttd->ttd_Box.Left, ttd->ttd_Box.Top, \
		ttd->ttd_Box.Width, ttd->ttd_Box.Height));

	return ttd;
}


void TTDisposeItem(struct ttDef *ttd)
{
	while (ttd)
		{
		struct ttDef *nextDef = ttd->ttd_Next;

		d1(kprintf("%s/%s/%ld: ttd=%08lx  Next=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd, ttd->ttd_Next));

		if (ttd->ttd_Sub)
			{
			d1(kprintf("%s/%s/%ld: ttd_Sub=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Sub));

			TTDisposeItem(ttd->ttd_Sub);
			}

		d1(kprintf("%s/%s/%ld: ttd_Contents.ttc_Text=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
			ttd->ttd_Contents.ttc_Text, ttd->ttd_Contents.ttc_Text ? ttd->ttd_Contents.ttc_Text : (STRPTR) ""));

		switch (ttd->ttd_Type)
			{
		case TTTYPE_Text:
		case TTTYPE_TextHook:
			if (ttd->ttd_Contents.ttc_Text)
				{
				d1(kprintf("%s/%s/%ld: FREE ttc_Text=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Contents.ttc_Text));
				ScalosFree(ttd->ttd_Contents.ttc_Text);
				ttd->ttd_Contents.ttc_Text = NULL;
				}
			break;
		case TTTYPE_DTImage:
			d1(KPrintF("%s/%s/%ld: DisposeDataTypesImage ttc_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Contents.ttc_Image));
			DisposeDatatypesImage(&ttd->ttd_Contents.ttc_Image);
			break;
		default:
			break;
			}

		if (ttd->ttd_HiddenHookText)
			{
			d1(kprintf("%s/%s/%ld: FREE ttd->ttd_HiddenHookText=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_HiddenHookText));
			ScalosFree(ttd->ttd_HiddenHookText);
			ttd->ttd_HiddenHookText = NULL;
			}

		Scalos_CloseFont(&ttd->ttd_Font, &ttd->ttd_TTFont);

		if (ttd->ttd_TTFontSpec)
			{
			FreeCopyString(ttd->ttd_TTFontSpec);
			ttd->ttd_TTFontSpec = NULL;
			}
		if (ttd->ttd_TextAttr.ta_Name)
			{
			FreeCopyString(ttd->ttd_TextAttr.ta_Name);
			ttd->ttd_TextAttr.ta_Name = NULL;
			}

		d1(kprintf("%s/%s/%ld: FREE ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd));
		ScalosFree(ttd);

		ttd = nextDef;
		}
}


void TTLayout(struct RastPort *rp, struct ttDef *ttd, struct IBox *layoutBox, ULONG LayoutMode, struct Hook *userHook)
{
	layoutBox->Left = layoutBox->Top = layoutBox->Width = layoutBox->Height = 0;

	TTLayoutItem(rp, ttd, layoutBox, LayoutMode, userHook);

#ifdef DUMPLAYOUT
	kprintf("%s/%s/%ld: ----------------------START----------------------\n", __FILE__, __FUNC__, __LINE__);
	DumpLayout(ttd);
	kprintf("%s/%s/%ld: ---------------------- END-----------------------\n", __FILE__, __FUNC__, __LINE__);
#endif

	TTPositionItem(ttd, layoutBox, LayoutMode);
}


static void TTLayoutItem(struct RastPort *rp, struct ttDef *ttd, struct IBox *layoutBox, 
	ULONG LayoutMode, struct Hook *userHook)
{
	struct ttDef *xDef;
	WORD MaxWidth, MaxHeight;
	ULONG LastAttr = FS_NORMAL;

	MaxWidth = MaxHeight = 0;

	for (xDef=ttd; xDef; xDef=xDef->ttd_Next)
		{
		if (xDef->ttd_HiddenHookText)
			{
			d1(kprintf("%s/%s/%ld: ttd_HiddenHookText=<%s>\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_HiddenHookText));

			// Call hidden hook, store result in ttd_Hidden
			if (userHook)
				{
				xDef->ttd_Hidden = CallHookPkt(userHook, xDef, xDef->ttd_HiddenHookText);

				d1(kprintf("%s/%s/%ld: xDef->ttd_Hidden=%lu\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Hidden));
				}
			}

		if (!xDef->ttd_Hidden)
			{
			if (xDef->ttd_Sub)
				{
				d1(kprintf("%s/%s/%ld: ttd_Sub=%08lx\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Sub));

				xDef->ttd_Box.Width = 0;
				xDef->ttd_Box.Height = 0;

				TTLayoutItem(rp, xDef->ttd_Sub, &xDef->ttd_Box, xDef->ttd_LayoutMode, userHook);
				}
			else
				{
				struct TextFont *oldFont = NULL;

				d1(kprintf("%s/%s/%ld: ttd_Hidden=%ld\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Hidden));
				d1(kprintf("%s/%s/%ld: ttd_Contents.ttc_Text=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
					xDef->ttd_Contents.ttc_Text, xDef->ttd_Contents.ttc_Text ? xDef->ttd_Contents.ttc_Text : (STRPTR) ""));

				if (rp->Font != xDef->ttd_Font)
					{
					oldFont = rp->Font;
					Scalos_SetFont(rp, xDef->ttd_Font, &xDef->ttd_TTFont);
					LastAttr = FS_NORMAL;
					}

				switch (xDef->ttd_Type)
					{
				case TTTYPE_Space:
					d1(kprintf("%s/%s/%ld: Space=%ld\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Space));
					switch (LayoutMode)
						{
					case TTL_Horizontal:
						xDef->ttd_Box.Width = xDef->ttd_Space;
						xDef->ttd_Box.Height = 0;
						break;
					case TTL_Vertical:
						xDef->ttd_Box.Width = 0;
						xDef->ttd_Box.Height = xDef->ttd_Space;
						d1(kprintf("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Box.Height));
						break;
						}
					break;

				case TTTYPE_WideTitleBar:
					d1(kprintf("%s/%s/%ld: WideTitleBar\n", __FILE__, __FUNC__, __LINE__));
					xDef->ttd_Box.Width = 1;
					xDef->ttd_Box.Height = 2;
					d1(kprintf("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Box.Height));
					break;

				case TTTYPE_TextHook:
					d1(kprintf("%s/%s/%ld: UserFunction ttc_Text=%08lx\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Contents.ttc_Text));
					if (userHook)
						{
						STRPTR userText;

						userText = (STRPTR) CallHookPkt(userHook, xDef, xDef->ttd_Contents.ttc_Text);

						if (userText)
							{
							STRPTR newText = ScalosAlloc(1 + strlen(userText));
							d1(kprintf("%s/%s/%ld: ALLOC newText=%08lx\n", __FILE__, __FUNC__, __LINE__, newText));
							if (newText)
								{
								strcpy(newText, userText);
								d1(kprintf("%s/%s/%ld: FREE ttc_Text=%08lx\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Contents.ttc_Text));
								ScalosFree(xDef->ttd_Contents.ttc_Text);
								xDef->ttd_Contents.ttc_Text = newText;
								}
							}

						d1(kprintf("%s/%s/%ld: ttc_Text=<%s>\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Contents.ttc_Text));
						}
					// continue with TTTYPE_Text w/o break;

				case TTTYPE_Text:
					xDef->ttd_TextExtent.te_Width = 0;
					xDef->ttd_TextExtent.te_Height = 0;
					xDef->ttd_TextExtent.te_Extent.MinX = xDef->ttd_TextExtent.te_Extent.MinY 
						= xDef->ttd_TextExtent.te_Extent.MaxX = xDef->ttd_TextExtent.te_Extent.MaxY = 0;

					d1(kprintf("%s/%s/%ld: ttd_Contents.ttc_Text=%08lx\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Contents.ttc_Text));
					if (xDef->ttd_Contents.ttc_Text)
						{
						CONST_STRPTR pText = xDef->ttd_Contents.ttc_Text;
						ULONG fontHeight;

						if (LastAttr != xDef->ttd_Attributes)
							{
							Scalos_SetSoftStyle(rp, xDef->ttd_Attributes, 
								FSF_BOLD | FSF_UNDERLINED | FSF_ITALIC,
								&xDef->ttd_TTFont);
							LastAttr = xDef->ttd_Attributes;
							}

						fontHeight = Scalos_GetFontHeight(rp);

						d1(kprintf("%s/%s/%ld: ttd_Contents.ttc_Text=<%s>\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Contents.ttc_Text));

						while (*pText)
							{
							struct TextExtent ext2;
							CONST_STRPTR pText2;
							size_t Length;

							for (pText2 = pText, Length=0; *pText2 && '\n' != *pText2;
								Length++, pText2++)
								;

							if ('\n' == *pText2)
								pText2++;

							Scalos_TextExtent(rp, pText, Length, &ext2);

							ext2.te_Height = max(ext2.te_Height, fontHeight);

							d1(kprintf("%s/%s/%ld: ttd=%08lx pText=<%s> FontHeight=%ld  h=%ld  MinY=%ld  MaxY=%ld\n", \
									__FILE__, __FUNC__, __LINE__, xDef, pText, Scalos_GetFontHeight(rp), ext2.te_Height, \
									ext2.te_Extent.MinY, ext2.te_Extent.MaxY));

							xDef->ttd_TextExtent.te_Width = max(xDef->ttd_TextExtent.te_Width, ext2.te_Width);
							xDef->ttd_TextExtent.te_Height += ext2.te_Height;
							xDef->ttd_TextExtent.te_Extent.MinX = min(xDef->ttd_TextExtent.te_Extent.MinX, ext2.te_Extent.MinX);
							xDef->ttd_TextExtent.te_Extent.MinY = min(xDef->ttd_TextExtent.te_Extent.MinY, ext2.te_Extent.MinY);
							xDef->ttd_TextExtent.te_Extent.MaxX = max(xDef->ttd_TextExtent.te_Extent.MaxX, ext2.te_Extent.MaxX);
							xDef->ttd_TextExtent.te_Extent.MaxY = max(xDef->ttd_TextExtent.te_Extent.MaxY, ext2.te_Extent.MaxY);

							pText = pText2;
							}
						}
					else
						{
						xDef->ttd_TextExtent.te_Width = xDef->ttd_TextExtent.te_Height = 0;
						}

					xDef->ttd_Box.Width = xDef->ttd_TextExtent.te_Width;
					xDef->ttd_Box.Height = xDef->ttd_TextExtent.te_Height;
					d1(kprintf("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Box.Height));
					break;

				case TTTYPE_DTImage:
				case TTTYPE_Image:
					if (xDef->ttd_Contents.ttc_Image)
						{
						xDef->ttd_Box.Width = xDef->ttd_Contents.ttc_Image->dti_BitMapHeader->bmh_Width;
						xDef->ttd_Box.Height = xDef->ttd_Contents.ttc_Image->dti_BitMapHeader->bmh_Height;
						}
					else
						{
						xDef->ttd_Box.Width = 0;
						xDef->ttd_Box.Height = 0;
						}
					d1(kprintf("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Box.Height));
					break;

				default:
					break;
					}

				if (oldFont)
					{
					Scalos_SetFont(rp, oldFont, NULL);
					LastAttr = FS_NORMAL;
					}

				d1(kprintf("%s/%s/%ld: LayoutMode=%ld  Width=%ld  Height=%ld\n", \
					__FILE__, __FUNC__, __LINE__, LayoutMode, xDef->ttd_TextExtent.te_Width, xDef->ttd_TextExtent.te_Height));
				}

			xDef->ttd_Box.Width += xDef->ttd_SpaceLeft + xDef->ttd_SpaceRight;
			xDef->ttd_Box.Height += xDef->ttd_SpaceTop + xDef->ttd_SpaceBottom;

			d1(kprintf("%s/%s/%ld: ttd=%08lx  Type=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, xDef, xDef->ttd_Type, xDef->ttd_Box.Left, xDef->ttd_Box.Top, \
				xDef->ttd_Box.Width, xDef->ttd_Box.Height));
			}

		switch (LayoutMode)
			{
		case TTL_Horizontal:
			d1(kprintf("%s/%s/%ld: Width=%ld\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Box.Width));

			if (!xDef->ttd_Hidden && xDef->ttd_Box.Width)
				layoutBox->Width += xDef->ttd_Box.Width + ttd->ttd_SpaceHoriz;

			if (MaxHeight < xDef->ttd_Box.Height)
				MaxHeight = xDef->ttd_Box.Height;
			break;
		case TTL_Vertical:
			d1(kprintf("%s/%s/%ld: layoutBox->Height=%ld  ttd_Box.Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, layoutBox->Height, xDef->ttd_Box.Height));

			if (!xDef->ttd_Hidden && xDef->ttd_Box.Height)
				layoutBox->Height += xDef->ttd_Box.Height + ttd->ttd_SpaceVert;

			d1(kprintf("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, layoutBox->Height));

			if (MaxWidth < xDef->ttd_Box.Width)
				MaxWidth = xDef->ttd_Box.Width;
			break;
			}

		d1(kprintf("%s/%s/%ld: ttd=%08lx  Type=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, xDef, xDef->ttd_Type, xDef->ttd_Box.Left, xDef->ttd_Box.Top, \
			xDef->ttd_Box.Width, xDef->ttd_Box.Height));
		}

	// adjust parent box size
	switch (LayoutMode)
		{
	case TTL_Horizontal:
		if (layoutBox->Height < MaxHeight)
			layoutBox->Height = MaxHeight;
		d1(kprintf("%s/%s/%ld: Height=%ld\n", __FILE__, __FUNC__, __LINE__, layoutBox->Height));
		break;
	case TTL_Vertical:
		if (layoutBox->Width < MaxWidth)
			layoutBox->Width = MaxWidth;
		break;
		}

	// adjust all items in same group to equal height/width
	for (xDef=ttd; xDef; xDef=xDef->ttd_Next)
		{
		d1(kprintf("%s/%s/%ld: MaxWidth=%ld  MaxHeight=%ld\n", __FILE__, __FUNC__, __LINE__, MaxWidth, MaxHeight));

		switch (LayoutMode)
			{
		case TTL_Horizontal:
			if (xDef->ttd_Box.Height < MaxHeight)
				{
				TTSpreadVert(xDef->ttd_Sub, MaxHeight, &xDef->ttd_Box);
				}
			break;
		case TTL_Vertical:
			if (xDef->ttd_Box.Width < MaxWidth)
				{
				TTSpreadHoriz(xDef->ttd_Sub, MaxWidth, &xDef->ttd_Box);
				}
			break;
			}

		d1(kprintf("%s/%s/%ld: ttd=%08lx  Type=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, xDef, xDef->ttd_Type, xDef->ttd_Box.Left, xDef->ttd_Box.Top, \
			xDef->ttd_Box.Width, xDef->ttd_Box.Height));
		}

	d1(kprintf("%s/%s/%ld: LayoutBox Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, layoutBox->Left, layoutBox->Top, layoutBox->Width, layoutBox->Height));
}


static void TTPositionItem(struct ttDef *ttd, struct IBox *layoutBox, ULONG LayoutMode)
{
	WORD Left = layoutBox->Left, Top = layoutBox->Top;
	struct ttDef *xDef;

	for (xDef=ttd; xDef; xDef=xDef->ttd_Next)
		{
		xDef->ttd_Box.Left = Left;
		xDef->ttd_Box.Top = Top;

		d1(kprintf("%s/%s/%ld: ttd=%08lx  Type=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, xDef, xDef->ttd_Type, xDef->ttd_Box.Left, xDef->ttd_Box.Top, \
			xDef->ttd_Box.Width, xDef->ttd_Box.Height));

		if (!xDef->ttd_Hidden)
			{
			if (xDef->ttd_Sub)
				{
				d1(kprintf("%s/%s/%ld: ttd_Sub=%08lx\n", __FILE__, __FUNC__, __LINE__, xDef->ttd_Sub));

				TTPositionItem(xDef->ttd_Sub, &xDef->ttd_Box, xDef->ttd_LayoutMode);
				}

			switch (LayoutMode)
				{
			case TTL_Horizontal:
				if (xDef->ttd_Box.Width > 0)
					Left += xDef->ttd_Box.Width + xDef->ttd_SpaceHoriz;
				break;
			case TTL_Vertical:
				if (xDef->ttd_Box.Height > 0)
					Top += xDef->ttd_Box.Height + xDef->ttd_SpaceVert;
				break;
				}
			}
		}

}


void TTRender(struct RastPort *rp, struct ttDef *ttd, WORD xLeft, WORD yTop)
{
	LONG rpWidth = GetBitMapAttr(rp->BitMap, BMA_WIDTH);
	LONG rpHeight = GetBitMapAttr(rp->BitMap, BMA_HEIGHT);
	ULONG LastAttr = FS_NORMAL;

	while (ttd)
		{
		if (!ttd->ttd_Hidden)
			{
			WORD x, y;

			if (ttd->ttd_Sub)
				{
				d1(kprintf("%s/%s/%ld: ttd_Sub=%08lx  xLeft=%ld yTop=%ld\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Sub, xLeft, yTop));

				TTRender(rp, ttd->ttd_Sub, xLeft, yTop);
				}

			d1(kprintf("%s/%s/%ld: ttd=%08lx  Type=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, ttd, ttd->ttd_Type, ttd->ttd_Box.Left, ttd->ttd_Box.Top, \
				ttd->ttd_Box.Width, ttd->ttd_Box.Height));

			if (!((xLeft + ttd->ttd_Box.Left) >= rpWidth 
				|| (yTop + ttd->ttd_Box.Top) >= rpHeight
				|| (xLeft + ttd->ttd_Box.Left + ttd->ttd_Box.Width) <= 0
				|| (yTop + ttd->ttd_Box.Top + ttd->ttd_Box.Height) <= 0))
				{
				struct TextFont *oldFont = NULL;

				d1(kprintf("%s/%s/%ld: ttd_Hidden=%ld\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Hidden));
				d1(kprintf("%s/%s/%ld: ttd_Contents.ttc_Text=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
					ttd->ttd_Contents.ttc_Text, ttd->ttd_Contents.ttc_Text ? ttd->ttd_Contents.ttc_Text : (STRPTR) ""));

				d1(kprintf("%s/%s/%ld: Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Box.Left, ttd->ttd_Box.Top));
				d1(kprintf("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Box.Width, ttd->ttd_Box.Height));

				if (rp->Font != ttd->ttd_Font)
					{
					oldFont = rp->Font;
					Scalos_SetFont(rp, ttd->ttd_Font, &ttd->ttd_TTFont);
					LastAttr = FS_NORMAL;
					}

				switch (ttd->ttd_Type)
					{
				case TTTYPE_Space:
					d1(kprintf("%s/%s/%ld: Space\n", __FILE__, __FUNC__, __LINE__));
					break;

				case TTTYPE_WideTitleBar:
					d1(kprintf("%s/%s/%ld: WideTitleBar\n", __FILE__, __FUNC__, __LINE__));

					x = xLeft + ttd->ttd_Box.Left + ttd->ttd_SpaceLeft;
					y = ttd->ttd_Box.Top + ttd->ttd_SpaceTop;

					SetAPen(rp, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);
					Move(rp, x, y + yTop);
					Draw(rp, x + ttd->ttd_Box.Width - ttd->ttd_SpaceRight, y + yTop);

					SetAPen(rp, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);
					Move(rp, x, y + yTop + 1);
					Draw(rp, x + ttd->ttd_Box.Width - ttd->ttd_SpaceRight, y + yTop+ 1);
					break;

				case TTTYPE_TextHook:	// user-defined text has been (hopefully) inserted at layout time
				case TTTYPE_Text:
					d1(kprintf("%s/%s/%ld: ttd_Contents.ttc_Text=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
						ttd->ttd_Contents.ttc_Text, ttd->ttd_Contents.ttc_Text ? ttd->ttd_Contents.ttc_Text : (STRPTR) ""));

					if (ttd->ttd_Contents.ttc_Text)
						{
						WORD Top = ttd->ttd_Box.Top + ttd->ttd_SpaceTop;
						CONST_STRPTR pText = ttd->ttd_Contents.ttc_Text;

						d1(kprintf("%s/%s/%ld: VertAlign=%ld\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_VertAlign));

						switch (ttd->ttd_VertAlign)
							{
//						case TTVERTALIGN_Baseline:
						case TTVERTALIGN_Top:
							break;
						case TTVERTALIGN_Bottom:
							Top += (ttd->ttd_Box.Height - ttd->ttd_SpaceBottom - ttd->ttd_SpaceTop) 
								- ttd->ttd_TextExtent.te_Height;

							d1(kprintf("%s/%s/%ld: Top  Box.Height=%ld te_Height=%ld\n", \
								__FILE__, __FUNC__, __LINE__, ttd->ttd_Box.Height, ttd->ttd_TextExtent.te_Height));
							break;
						case TTVERTALIGN_Center:
							Top += ((ttd->ttd_Box.Height - ttd->ttd_SpaceBottom - ttd->ttd_SpaceTop) 
								- ttd->ttd_TextExtent.te_Height) / 2;

							d1(kprintf("%s/%s/%ld: Center  Box.Height=%ld te_Height=%ld\n", \
								__FILE__, __FUNC__, __LINE__, ttd->ttd_Box.Height, ttd->ttd_TextExtent.te_Height));
							break;
							}

						while (*pText)
							{
							WORD x, l;
							size_t TextLen;
							CONST_STRPTR pText2;

							for (pText2 = pText, TextLen=0; *pText2 && '\n' != *pText2;
								TextLen++, pText2++)
								;

							if ('\n' == *pText2)
								pText2++;

							if (LastAttr != ttd->ttd_Attributes)
								{
								Scalos_SetSoftStyle(rp, 
									ttd->ttd_Attributes, FSF_BOLD | FSF_UNDERLINED | FSF_ITALIC,
									&ttd->ttd_TTFont);
								LastAttr = ttd->ttd_Attributes;
								}
							SetAPen(rp, ttd->ttd_TextPen);

							x = ttd->ttd_Box.Left;

							switch (ttd->ttd_HorizAlign)
								{
							case GTJ_LEFT:
								break;

							case GTJ_CENTER:
								l = Scalos_TextLength(rp, pText, TextLen);
								x += (ttd->ttd_Box.Width - ttd->ttd_SpaceLeft - ttd->ttd_SpaceRight - l) / 2;
								break;

							case GTJ_RIGHT:
								l = Scalos_TextLength(rp, pText, TextLen);
								x += (ttd->ttd_Box.Width - ttd->ttd_SpaceRight) - l;
								break;
								}

							x += ttd->ttd_SpaceLeft;

							d1(kprintf("%s/%s/%ld: ttd=%08lx Text=<%s>  x=%ld  y=%ld\n", \
								__FILE__, __FUNC__, __LINE__, ttd, pText, xLeft + x, yTop + Top + Scalos_GetFontBaseline(rp)));

							Move(rp, xLeft + x, yTop + Top + Scalos_GetFontBaseline(rp));
							Scalos_Text(rp, pText, TextLen);

							Top += Scalos_GetFontHeight(rp) + ttd->ttd_SpaceVert;

							pText = pText2;
							}
						}
					break;

				case TTTYPE_Image:
				case TTTYPE_DTImage:
					d1(kprintf("%s/%s/%ld: Image\n", __FILE__, __FUNC__, __LINE__));

					if (ttd->ttd_Contents.ttc_Image)
						{
						WORD Top = ttd->ttd_Box.Top + ttd->ttd_SpaceTop;

						x = 0;
						switch (ttd->ttd_HorizAlign)
							{
						case GTJ_LEFT:
							break;

						case GTJ_CENTER:
							x = (ttd->ttd_Box.Width - ttd->ttd_SpaceLeft 
								- ttd->ttd_SpaceRight - ttd->ttd_Contents.ttc_Image->dti_BitMapHeader->bmh_Width) / 2;
							break;

						case GTJ_RIGHT:
							x = ttd->ttd_Box.Width - ttd->ttd_SpaceRight
								- ttd->ttd_Contents.ttc_Image->dti_BitMapHeader->bmh_Width;
							break;
							}

						switch (ttd->ttd_VertAlign)
							{
//						case TTVERTALIGN_Baseline:
						case TTVERTALIGN_Top:
							break;
						case TTVERTALIGN_Bottom:
							Top += (ttd->ttd_Box.Height - ttd->ttd_SpaceBottom - ttd->ttd_SpaceTop) 
								- ttd->ttd_Contents.ttc_Image->dti_BitMapHeader->bmh_Height;
							break;
						case TTVERTALIGN_Center:
							Top += ((ttd->ttd_Box.Height - ttd->ttd_SpaceBottom - ttd->ttd_SpaceTop) 
								- ttd->ttd_Contents.ttc_Image->dti_BitMapHeader->bmh_Height) / 2;
							break;
							}

						x += ttd->ttd_SpaceLeft;

						DtImageDraw(ttd->ttd_Contents.ttc_Image,
							rp,
							xLeft + x, yTop + Top,
							ttd->ttd_Contents.ttc_Image->dti_BitMapHeader->bmh_Width,
							ttd->ttd_Contents.ttc_Image->dti_BitMapHeader->bmh_Height
							);
						}
					break;

				default:
					break;
					}

				if (MF_FRAME_NONE != ttd->ttd_FrameType)
					{
					d1(kprintf("%s/%s/%ld: Frame MinX=%ld MinY=%ld MaxX=%ld MaxY=%ld\n", \
						__FILE__, __FUNC__, __LINE__, xLeft + ttd->ttd_Box.Left, ttd->ttd_Box.Top + yTop,\
						xLeft + ttd->ttd_Box.Left + ttd->ttd_Box.Width - 1,\
						ttd->ttd_Box.Top + yTop + ttd->ttd_Box.Height - 1));

					McpGfxDrawFrame(rp, xLeft + ttd->ttd_Box.Left,
						ttd->ttd_Box.Top + yTop,
						xLeft + ttd->ttd_Box.Left + ttd->ttd_Box.Width - 1,
						ttd->ttd_Box.Top + yTop + ttd->ttd_Box.Height - 1,
						IA_ShadowPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN],
						IA_HighlightPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN],
						IA_FrameType, ttd->ttd_FrameType,
						TAG_END);
					}
#if 0
				else
					{
					// TEST code - outline every cell
					McpGfxDrawFrame(rp, xLeft + ttd->ttd_Box.Left,
						ttd->ttd_Box.Top + yTop,
						xLeft + ttd->ttd_Box.Left + ttd->ttd_Box.Width - 1,
						ttd->ttd_Box.Top + yTop + ttd->ttd_Box.Height - 1,
						IA_ShadowPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN],
						IA_HighlightPen, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN],
						IA_FrameType, MF_FRAME_BORDER,
						TAG_END);
					}
#endif
				if (oldFont)
					{
					Scalos_SetFont(rp, oldFont, NULL);
					LastAttr = FS_NORMAL;
					}
				}
			}
#if 0
		else
			{
			d1(kprintf("%s/%s/%ld: Skipped ttd Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", \
				__FILE__, __FUNC__, __LINE__, xLeft + ttd->ttd_Box.Left, yTop + ttd->ttd_Box.Top, ttd->ttd_Box.Width, ttd->ttd_Box.Height));
			}
#endif
		ttd = ttd->ttd_Next;
		}
}


static void TTSpreadHoriz(struct ttDef *ttd, WORD newWidth, struct IBox *parentBox)
{
	struct ttDef *xDef;
	ULONG ttCount;
	WORD additionalWidth = newWidth - parentBox->Width;
	WORD WidthIncrement;

	for (ttCount=0, xDef=ttd; xDef; xDef=xDef->ttd_Next)
		{
		if (!xDef->ttd_Hidden && xDef->ttd_Box.Width > 0)
			ttCount++;
		}

	if (ttCount)
		{
		WidthIncrement = additionalWidth / ttCount;
		if (WidthIncrement < 1)
			WidthIncrement = 1;

		while (ttd && additionalWidth)
			{
			if (!ttd->ttd_Hidden && ttd->ttd_Box.Width > 0)
				{
				TTSpreadHoriz(ttd->ttd_Sub, ttd->ttd_Box.Width + WidthIncrement, &ttd->ttd_Box);

				additionalWidth -= WidthIncrement;
				}
			ttd = ttd->ttd_Next;
			}
		}

	parentBox->Width = newWidth;
}


static void TTSpreadVert(struct ttDef *ttd, WORD newHeight, struct IBox *parentBox)
{
	struct ttDef *xDef;
	ULONG ttCount;
	WORD additionalHeight = newHeight - parentBox->Height;
	WORD HeightIncrement;

	for (ttCount=0, xDef=ttd; xDef; xDef=xDef->ttd_Next)
		{
		if (!xDef->ttd_Hidden && xDef->ttd_Box.Height > 0)
			ttCount++;
		}

	d1(kprintf("%s/%s/%ld: newHeight=%ld  parentBox->Height=%ld\n", __FILE__, __FUNC__, __LINE__, newHeight, parentBox->Height));
	d1(kprintf("%s/%s/%ld: additionalHeight=%ld  ttCount=%ld\n", __FILE__, __FUNC__, __LINE__, additionalHeight, ttCount));

	if (ttCount)
		{
		HeightIncrement = additionalHeight / ttCount;
		if (HeightIncrement < 1)
			HeightIncrement = 1;

		while (ttd && additionalHeight)
			{
			if (!ttd->ttd_Hidden && ttd->ttd_Box.Height > 0)
				{
				TTSpreadVert(ttd->ttd_Sub, ttd->ttd_Box.Height + HeightIncrement, &ttd->ttd_Box);

				additionalHeight -= HeightIncrement;
				}
			ttd = ttd->ttd_Next;
			}
		}

	d1(kprintf("%s/%s/%ld: newHeight=%ld\n", __FILE__, __FUNC__, __LINE__, newHeight));

	parentBox->Height = newHeight;
}


static SAVEDS(ULONG) TTStringFormatHookFunc(
	struct Hook *theHook,
	struct Locale *myLocale,
	char NextChar)
{
	struct FormatStringBuffInfo *fsbi = (struct FormatStringBuffInfo *) theHook->h_Data;

	(void) myLocale;

	if (fsbi->fsbi_BuffLength > 1)
		{
		*fsbi->fsbi_Buffer++ = NextChar;
		fsbi->fsbi_BuffLength--;
		}
	else
		*fsbi->fsbi_Buffer = '\0';

	return 0L;
}


#if !defined(ScaFormatString)
void ScaFormatString(char *Buffer, const char *Format, ...)
{
	va_list Args;

	va_start(Args, Format);

	ScaFormatStringArgs(Buffer, INT_MAX, Format, Args);

	va_end(Args);
}
#endif /* ScaFormatString */


#if !defined(ScaFormatStringMaxLength)
void ScaFormatStringMaxLength(char *Buffer, size_t BuffLen, const char *Format, ...)
{
	va_list Args;

	va_start(Args, Format);

	ScaFormatStringArgs(Buffer, BuffLen, Format, Args);

	va_end(Args);
}
#endif /* ScaFormatStringMaxLength */


void ScaFormatStringArgs(char *Buffer, size_t BuffLength, const char *Format, APTR Args)
{
	if (LocaleBase && ((struct Library *) LocaleBase)->lib_Version >= 38 && ScalosLocale)
		{
		struct Hook myHook;
		struct FormatStringBuffInfo fsbi;

		fsbi.fsbi_Buffer = Buffer;
		fsbi.fsbi_BuffLength = BuffLength;

		SETHOOKFUNC(myHook, TTStringFormatHookFunc);
		myHook.h_Data = &fsbi;

		FormatString(ScalosLocale, (STRPTR) Format, Args, &myHook);
		}
	else
		{
		// here converting %lU und %lD format strings to lower case
		// because snprintf doesn't recognize them
		char *FormatCopy;

		FormatCopy = AllocCopyString(Format);
		d1(kprintf("%s/%s/%ld: ALLOC FormatCopy=%08lx\n", __FILE__, __FUNC__, __LINE__, FormatCopy));
		if (FormatCopy)
			{
			char *p;

			p = strstr(Format, "%lU");
			if (p)
				p[2] = 'u';

			p = strstr(Format, "%lD");
			if (p)
				p[2] = 'd';

			vsnprintf(Buffer, BuffLength, Format, Args);
			d1(kprintf("%s/%s/%ld: FREE FormatCopy=%08lx\n", __FILE__, __FUNC__, __LINE__, FormatCopy));
			FreeCopyString(FormatCopy);
			}
		}
}


#ifdef DUMPLAYOUT
static void DumpLayout(struct ttDef *ttd)
{
	while (ttd)
		{
		if (ttd->ttd_Sub)
			DumpLayout(ttd->ttd_Sub);

		kprintf("%s/%s/%ld: ttd=%08lx  Type=%ld  Left=%ld  Top=%ld  Width=%ld  Height=%ld\n",
			__FILE__, __FUNC__, __LINE__, ttd, ttd->ttd_Type,
			ttd->ttd_Box.Left, ttd->ttd_Box.Top,
			ttd->ttd_Box.Width, ttd->ttd_Box.Height
			);

		ttd = ttd->ttd_Next;
		}
}
#endif
