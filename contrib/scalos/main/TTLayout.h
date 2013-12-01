// TTLayout.h
// $Date$
// $Revision$

#ifndef	TTLAYOUT_H
#define	TTLAYOUT_H

//----------------------------------------------------------------------------

#include "scalos_structures.h"

#define	TT_VERT_SPACE	1
#define	TT_HORIZ_SPACE	3

enum TTLayoutMode 
	{ 
	TTL_Horizontal, 
	TTL_Vertical 
	};

enum TTType 
	{
	TTTYPE_Container, 
	TTTYPE_Text, 
	TTTYPE_Image, 
	TTTYPE_DTImage, 
	TTTYPE_WideTitleBar, 
	TTTYPE_Space, 
	TTTYPE_TextHook 
	};

enum TTVertAlign
	{
	TTVERTALIGN_Top,
	TTVERTALIGN_Bottom,
	TTVERTALIGN_Center,
//	TTVERTALIGN_Baseline,
	};

struct ttDef
	{
	struct ttDef *ttd_Next;

	struct ttDef *ttd_Sub;			// Subitem/Group pointer

	UWORD ttd_LayoutMode;			// enum TTLayoutMode

	ULONG ttd_MsgID;			// Message ID for locale text

	union	{
		STRPTR ttc_Text;
		struct DatatypesImage *ttc_Image;
		} ttd_Contents;

	struct TextExtent ttd_TextExtent;	// bounding box for ttc_Text

	STRPTR ttd_HiddenHookText;		// Textual argument of TT_HiddenHook

	enum TTType ttd_Type;			// type of entry

	ULONG ttd_Attributes;			// text atributes (bold, ...)
	ULONG ttd_TextPen;			// text pen
	ULONG ttd_Space;

	ULONG ttd_HorizAlign;			// horizontal contents alignment
	enum TTVertAlign ttd_VertAlign;		// vertical contents alignment

	ULONG ttd_FrameType;			// some of the MF_FRAME_... values, e.g. MF_FRAME_NONE

	UWORD ttd_SpaceHoriz;			// horizontal spacing between cells
	UWORD ttd_SpaceVert;			// vertical spacing between cells

	UWORD ttd_SpaceLeft;			// additional left space inside cell
	UWORD ttd_SpaceRight;			// additional right space inside cell
	UWORD ttd_SpaceTop;			// additional top space inside cell
	UWORD ttd_SpaceBottom;			// additional bottom space inside cell

	struct IBox ttd_Box;			// Bounding box for item

	struct TextAttr ttd_TextAttr;
	STRPTR ttd_TTFontSpec;

	struct TextFont *ttd_Font;		// Font for Text
	struct TTFontFamily ttd_TTFont;		// TTEngine font

	BOOL ttd_Hidden;
	};

//----------------------------------------------------------------------------

// Tags for TT_CreateTTItem()

#define	TT_FirstTag	(TAG_USER+17490)

#define	TT_Dummy	TAG_IGNORE

#define	TT_Title	TT_FirstTag+1		// CONST_STRPTR
#define	TT_TitleID	TT_FirstTag+2		// CONST_STRPTR
#define	TT_HorizGroup	TT_FirstTag+3		// struct ttDef *
#define	TT_VertGroup	TT_FirstTag+4		// struct ttDef *
#define	TT_TextStyle	TT_FirstTag+5		// ULONG - combination of FSF_UNDERLINED, FSF_BOLD, and FSF_ITALIC
#define	TT_HAlign	TT_FirstTag+6		// ULONG - GTJ_LEFT, GTJ_RIGHT, or GTJ_CENTER
#define	TT_Item		TT_FirstTag+7		// struct ttDef *
#define	TT_Members	TT_FirstTag+8		// struct ttDef *
#define	TT_LayoutMode	TT_FirstTag+9		// ULONG - 
#define	TT_Hidden	TT_FirstTag+10		// ULONG - BOOL
#define	TT_WideTitleBar	TT_FirstTag+11		// ULONG - BOOL
#define	TT_TextPen	TT_FirstTag+12		// ULONG - Pen number for text
#define	TT_Image	TT_FirstTag+13		// struct DataTypesImage *
#define	TT_Space	TT_FirstTag+14		// ULONG - number of pixels space
#define	TT_Font		TT_FirstTag+15		// struct TextAttr *
#define	TT_DrawFrame	TT_FirstTag+16		// ULONG - some of the MF_FRAME_... values, e.g. MF_FRAME_NONE
#define	TT_TitleHook	TT_FirstTag+17		// CONST_STRPTR - userHook will be called in TTLayout() with
						//   hook=userHook, object=ttDef, message=supplied text
						// user hook MUST return string (will be duplicated).
						// Buffer is guaranteed to be empty at entry.
						// If anything goes wrong, an empty buffer MUST be returned.
#define	TT_HiddenHook	TT_FirstTag+18		// CONST_STRPTR - userHook will be called in TTLayout() with
						//   hook=userHook, object=ttDef, message=supplied text
						// user hook MUST return ULONG result
#define	TT_SpaceLeft	TT_FirstTag+19		// ULONG - additional space on left side of cell in pixel
#define	TT_SpaceRight	TT_FirstTag+20		// ULONG - additional space on right side of cell in pixel
#define	TT_SpaceTop	TT_FirstTag+21		// ULONG - additional space on top side of cell in pixel
#define	TT_SpaceBottom	TT_FirstTag+22		// ULONG - additional space on bottom side of cell in pixel
#define	TT_DTImage	TT_FirstTag+23		// CONST_STRPTR file name of datatypes image
#define	TT_VAlign	TT_FirstTag+24		// ULONG enum TTVertAlign
#define	TT_TTFont	TT_FirstTag+25		// struct TTTextAttr * - spec for TT font to use

//----------------------------------------------------------------------------

#define	TT_CreateTooltip	TT_CreateItem

#define TTSpace(s)	TT_Item, (ULONG) TT_CreateItem(TT_Space, (s))
#define TTItem(t)	TT_Item, (ULONG) TT_CreateItem(TT_Title, (t)
#define TTTitleHook(t)	TT_Item, (ULONG) TT_CreateItem(TT_TitleHook, (t)
#define TTItemID(t)	TT_Item, (ULONG) TT_CreateItem(TT_TitleID, (t)
#define TTHoriz		TT_Item, (ULONG) TT_CreateItem(TT_LayoutMode, TTL_Horizontal
#define TTVert		TT_Item, (ULONG) TT_CreateItem(TT_LayoutMode, TTL_Vertical
#define TTMembers	TT_Members, (ULONG) TT_CreateItem(TT_Dummy, 0
#define TTobjName(t)	TT_Item, (ULONG) TT_CreateItem(TT_Title, (t), \
				TT_TextStyle, FSF_BOLD, TT_HAlign, GTJ_CENTER

#ifndef End
#define End		TAG_DONE)
#endif

//----------------------------------------------------------------------------

struct ttDef *TT_CreateItemA(struct TagItem *TagList);
struct ttDef *TT_CreateItem(ULONG FirstTag, ...);
void TTDisposeItem(struct ttDef *oldDef);
void TTRender(struct RastPort *rp, struct ttDef *oldDef, WORD xLeft, WORD yTop);
void TTLayout(struct RastPort *rp, struct ttDef *oldDef, struct IBox *layoutBox, 
	ULONG LayoutMode, struct Hook *userHook);

//----------------------------------------------------------------------------

#endif /* TTLAYOUT_H */
