/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=8
*/

#ifndef _GTLAYOUT_INTERNAL_H
#define _GTLAYOUT_INTERNAL_H 1

	// Debugging stuff

#ifdef DEBUG
#define DB(x)	x
#else
#define DB(x)	;
#endif	/* DEBUG */

	// Keep this one handy

#ifndef __AROS__
VOID kprintf(STRPTR,...);
#endif

/*****************************************************************************/


	// Keyboard qualifiers

#define QUALIFIER_SHIFT 	(IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)
#define QUALIFIER_ALT		(IEQUALIFIER_LALT | IEQUALIFIER_RALT)
#define QUALIFIER_CONTROL	(IEQUALIFIER_CONTROL)


/*****************************************************************************/


	// Undefine these to remove various chunks of support from this code

#ifdef _GTLAYOUT_GLOBAL_H
#define DO_PASSWORD_KIND
#define DO_GAUGE_KIND
#define DO_TAPEDECK_KIND
#define DO_HEXHOOK
#define DO_PICKSHORTCUTS
#define DO_CLONING
#define DO_LEVEL_KIND
#define DO_BOOPSI_KIND
#define DO_MENUS
#define DO_POPUP_KIND
#define DO_TAB_KIND
#else
#define DO_PASSWORD_KIND
#define DO_GAUGE_KIND
#define DO_CLONING
#define DO_BOOPSI_KIND
#define DO_POPUP_KIND
#endif	// _GTLAYOUT_GLOBAL_H

	// These types require that the BOOPSI_KIND support
	// code is included.

#if defined(DO_LEVEL_KIND) || defined(DO_POPUP_KIND) || defined(DO_TAB_KIND)
#define DO_BOOPSI_KIND
#endif	/* defined(DO_LEVEL_KIND) || defined(DO_POPUP_KIND) || defined(DO_TAB_KIND) */

/*****************************************************************************/

#ifndef __AROS__
APTR ASM AsmCreatePool(REG(d0) ULONG MemFlags,REG(d1) ULONG PuddleSize,REG(d2) ULONG ThreshSize,REG(a6) struct Library *SysBase);
VOID ASM AsmDeletePool(REG(a0) APTR PoolHeader,REG(a6) struct Library *SysBase);
APTR ASM AsmAllocPooled(REG(a0) APTR PoolHeader,REG(d0) ULONG Size,REG(a6) struct Library *SysBase);
VOID ASM AsmFreePooled(REG(a0) APTR PoolHeader,REG(a1) APTR Memory,REG(d0) ULONG MemSize,REG(a6) struct Library *SysBase);
#else
#define AsmCreatePool(MemFlags,PuddleSize,TreshSize,SysBase) CreatePool(MemFlags,PuddleSize,TreshSize)
#define AsmDeletePool(PoolHeader,SysBase) DeletePool(PoolHeader)
#define AsmAllocPooled(PoolHeader,Size,SysBase) AllocPooled(PoolHeader,Size)
#define AsmFreePooled(PoolHeader,Memory,MemSize,SysBase) FreePooled(PoolHeader,Memory,MemSize)
#endif

/*****************************************************************************/


	// Scan an Exec list

#define SCANLIST(l,n)	for(n = (APTR)((struct MinList *)l)->mlh_Head; \
			((struct MinNode *)n)->mln_Succ; \
			n = (APTR)((struct MinNode *)n)->mln_Succ)

	// Scan the objects of a group

#define SCANGROUP(g,n)	for(n = (APTR)g->Special . Group . ObjectList . mlh_Head; \
			((struct MinNode *)n)->mln_Succ; \
			n = (APTR)((struct MinNode *)n)->mln_Succ)

	// Scan the objects of a group, only allowing active pages

#define SCANPAGE(g,n,p) for(p = 0, n = (APTR)g->Special . Group . ObjectList . mlh_Head; \
			((struct MinNode *)n)->mln_Succ; \
			n = (APTR)((struct MinNode *)n)->mln_Succ) \
			if ((p++ == g->Special . Group . ActivePage) || !g->Special . Group . Paging)

	// Get an object pointer from a gadget pointer

#define GETOBJECT(g,n)	((n = (ObjectNode *)g->UserData) && (n->Host == g) && (n->PointBack == n))


/*****************************************************************************/


#ifdef DO_PASSWORD_KIND
#define TEST_PASSWORD_KIND(n) (n->Type == PASSWORD_KIND)
#else
#define TEST_PASSWORD_KIND(n) (FALSE)
#endif	/* DO_PASSWORD_KIND */

	// Is an object derived from STRING_KIND?

#define LIKE_STRING_KIND(n) (n->Type == STRING_KIND || n->Type == FRACTION_KIND || TEST_PASSWORD_KIND(n))


/*****************************************************************************/


#define NUMELEMENTS(s) (sizeof(s) / sizeof((s)[0]))

#define NOT !
#define CANNOT !


/*****************************************************************************/


typedef char BOOLEAN;


/*****************************************************************************/


#define INCREMENTER_KIND	42
#define PICKER_KIND		43
#define GROUP_KIND		44

#define LAPR_Gadget		TAG_USER+100
#define LAPR_Object		TAG_USER+101

typedef struct GaugeExtra
{
	LONG			InfoLength;
	STRPTR			InfoText;
	WORD			LastPercentage;
	BOOLEAN			NoTicks;
	BOOLEAN			Discrete;
} GaugeExtra;

typedef struct TapeDeckExtra
{
	struct Image *		ButtonImage;
	LONG			ButtonWidth,
				ButtonHeight;
	BOOLEAN 		ButtonType;
	BOOLEAN			Toggle;
	BOOLEAN			Smaller;
	BOOLEAN			Tick;
} TapeDeckExtra;

typedef struct ButtonExtra
{
	struct Image *		ButtonImage;
	STRPTR			KeyStroke;
	STRPTR *		Lines;
	UBYTE			LineCount;

	BOOLEAN 		ReturnKey;
	BOOLEAN 		EscKey;
	BOOLEAN 		ExtraFat;
	BOOLEAN			DefaultCorrection;
	BOOLEAN			Smaller;
} ButtonExtra;

typedef struct BarExtra
{
	struct ObjectNode *	Parent;
	BOOLEAN 		FullSize;
} BarExtra;

typedef struct BoxExtra
{
	struct ObjectNode *	Parent;
	STRPTR *		Labels;
	STRPTR *		Lines;
	UWORD			Spacing;
	WORD			MaxSize;
	WORD			TextPen;
	WORD			BackPen;
	BYTE			AlignText;
	BOOLEAN 		DrawBox;
	BOOLEAN 		ReserveSpace;
} BoxExtra;

typedef struct FrameExtra
{
	struct Hook *		RefreshHook;
	UWORD			InnerWidth;
	UWORD			InnerHeight;
	struct Gadget		Dummy;
	WORD			PlusWidth;	// Is used by the resizing process
	WORD			PlusHeight;	// to change the inner width
	BOOLEAN 		DrawBox;
	BOOLEAN			GenerateEvents;
	BOOLEAN			ResizeX;
	BOOLEAN			ResizeY;
} FrameExtra;

typedef struct PickerExtra
{
	struct Gadget *		Parent;		// Important: must match IncrementerExtra!
	struct Image *		Image;		// Important: must match IncrementerExtra!
} PickerExtra;

typedef struct IncrementerExtra
{
	struct Gadget *		Parent;		// Important: must match PickerExtra!
	struct Image *		Image;		// Important: must match PickerExtra!
	LONG			Amount;
} IncrementerExtra;

typedef struct GroupExtra
{
	struct MinList		ObjectList;
	struct ObjectNode *	ParentGroup;
	LONG			MaxOffset;
	LONG			MaxSize;
	ULONG			ActivePage;
	UWORD			ExtraLeft;
	UWORD			ExtraTop;
	UWORD			FrameType;
	UBYTE			MiscFlags;
	BOOLEAN 		Horizontal;
	BOOLEAN 		Paging;
	BOOLEAN 		Spread;
	BOOLEAN 		SameSize;
	BOOLEAN 		LastAttributes;
	BOOLEAN 		Visible;
	BOOLEAN			Frame;
	BOOLEAN			IndentX;
	BOOLEAN			IndentY;
	BOOLEAN			NoIndent;
	BOOLEAN			AlignRight;
} GroupExtra;

#define GROUPF_WidthDone	(1<<0)
#define GROUPF_HeightDone	(1<<1)

typedef struct ListExtra
{
	struct List *		Labels;
	struct Gadget *		Link;
	LONG			LinkID;
	struct Hook *		CallBack;
	LONG			AutoPageID;
	struct TextAttr *	TextAttr;
	STRPTR *		ExtraLabels;
	UWORD			ExtraLabelWidth;
	UWORD			MaxPen;
	UWORD			MaxGrowX,MaxGrowY;
	UWORD			MinChars,MinLines;
	WORD			FixedGlyphWidth;
	WORD			FixedGlyphHeight;
	BOOLEAN 		ReadOnly;
	BOOLEAN 		CursorKey;
	BOOLEAN 		AllocatedList;
	BOOLEAN			LockSize;
	BOOLEAN			SizeLocked;
	BOOLEAN			ResizeX;
	BOOLEAN			ResizeY;
	BOOLEAN			FlushLabelLeft;
	BOOLEAN			IgnoreListContents;
	BOOLEAN			AdjustForString;
} ListExtra;

typedef struct RadioExtra
{
	STRPTR *		Choices;
	LONG			AutoPageID;
	UWORD			LabelWidth;
	UBYTE			TitlePlace;
	BOOLEAN			TabKey;
} RadioExtra;

typedef struct TextExtra
{
	STRPTR			Text;
	struct Gadget *		Picker;
	WORD			FrontPen;
	WORD			BackPen;
	UWORD			Len;
	UBYTE			Justification;
	BOOLEAN 		Border;
	BOOLEAN 		CopyText;
	BOOLEAN 		UsePicker;
	BOOLEAN			LockSize;
	BOOLEAN			SizeLocked;
} TextExtra;

typedef struct NumberExtra
{
	LONG			Number;
	STRPTR			Format;
	WORD			FrontPen;
	WORD			BackPen;
	WORD			MaxNumberLen;
	UBYTE			Justification;
	BOOLEAN 		Border;
} NumberExtra;

typedef struct CycleExtra
{
	STRPTR *		Choices;
	LONG			AutoPageID;
	BOOLEAN			TabKey;
} CycleExtra;

typedef struct PalExtra
{
	UBYTE *			ColourTable;
	UBYTE *			TranslateBack;
	struct Gadget *		Picker;
	UWORD			Depth;
	UWORD			NumColours;
	UWORD			IndicatorWidth;
	BOOLEAN 		SmallPalette;
	BOOLEAN			UsePicker;
} PalExtra;

typedef struct SliderExtra
{
	STRPTR			LevelFormat;
	DISPFUNC		DispFunc;
	LONG			MaxLevelLen;
	LONG			LevelWidth;
	STRPTR			OriginalLabel;
	UBYTE			LevelPlace;
	BOOLEAN 		FullLevelCheck;
} SliderExtra;

typedef struct StringExtra
{
	struct LayoutHandle *	LayoutHandle;
	struct Hook *		HistoryHook;
	ULONG			MaxHistoryLines;
	ULONG			NumHistoryLines;
	struct Node *		CurrentNode;

	STRPTR			String;
	LONG			MaxChars;
	struct Hook *		EditHook;
	struct Hook *		ValidateHook;
	struct Gadget *		Picker;
	STRPTR			Backup;
	STRPTR			RealString;
	STRPTR			Original;
	LONG			LinkID;

	struct Gadget *		LeftIncrementer;
	struct Gadget *		RightIncrementer;

	struct Hook *		IncrementerHook;

	UBYTE			Justification;
	BOOLEAN 		LastGadget;
	BOOLEAN 		UsePicker;
	BOOLEAN			Activate;
} StringExtra;

typedef struct IntegerExtra
{
	struct LayoutHandle *	LayoutHandle;
	struct Hook *		HistoryHook;
	ULONG			MaxHistoryLines;
	ULONG			NumHistoryLines;
	struct Node *		CurrentNode;

	struct Gadget *		LeftIncrementer;
	struct Gadget *		RightIncrementer;

	struct Hook *		IncrementerHook;

	LONG			Number;
	LONG			MaxChars;
	struct Hook *		EditHook;
	struct Hook *		ValidateHook;

	UBYTE			Justification;
	BOOLEAN 		LastGadget;
	BOOLEAN 		UseIncrementers;
	BOOLEAN			CustomHook;
	BOOLEAN			Activate;
} IntegerExtra;

typedef struct ScrollerExtra
{
	struct ObjectNode *	Parent;
	WORD			Visible;
	WORD			ArrowSize;
	BOOLEAN 		RelVerify;
	BOOLEAN			Immediate;
	BOOLEAN			Arrows;
	BOOLEAN			Vertical;
	BOOLEAN			Thin;
	BOOLEAN			FullSize;
} ScrollerExtra;

typedef struct LevelExtra
{
	WORD			Freedom;
	WORD			Ticks;
	WORD			NumTicks;
	LONG			Plus;
	STRPTR			LevelFormat;
	DISPFUNC		DispFunc;
	LONG			MaxLevelWidth;
	LONG			LevelPlace;
	BOOLEAN			FullLevelCheck;
} LevelExtra;

typedef struct BOOPSIExtra
{
	struct Library *	ClassBase;
	Class *			ClassInstance;
	STRPTR			ClassName;
	STRPTR			ClassLibraryName;
	struct TagItem *	ClassTags;
	ULONG			TagCurrent,
				TagTextAttr,
				TagDrawInfo,
				TagLink,
				TagScreen;
	LONG			Link;
	struct ObjectNode *	Parent;
	struct Hook *		ActivateHook;
	WORD			ExactWidth,
				ExactHeight;
	WORD			RelFontHeight;
	BOOLEAN			FullWidth;
	BOOLEAN			FullHeight;
} BOOPSIExtra;

typedef struct PopupExtra
{
	STRPTR *		Choices;
	LONG			AutoPageID;
	BOOLEAN			TabKey;
	BOOLEAN			CentreActive;
} PopupExtra;

typedef struct TabExtra
{
	STRPTR *		Choices;
	LONG			AutoPageID;
	struct ObjectNode *	Parent;
	BOOLEAN			TabKey;
	BOOLEAN			FullWidth;
	BOOLEAN			FrameGroup;
} TabExtra;

typedef struct ImageExtra
{
	struct Image *		Image;
	struct BitMap *		BitMap;
	PLANEPTR		BitMapMask;
	UWORD			BitMapLeft;
	UWORD			BitMapTop;
	UWORD			BitMapWidth;
	UWORD			BitMapHeight;
} ImageExtra;

typedef struct ObjectNode
{
	struct MinNode		Node;
	struct Gadget *		Host;
	struct ObjectNode *	PointBack;
	STRPTR			Label;
	LONG			ID;
	LONG			Min;
	LONG			Max;
	LONG			Current;
	APTR			Storage;

	UWORD			Left;
	UWORD			Top;
	UWORD			Width;
	UWORD			Height;
	UWORD			LabelWidth;
	UWORD			Lines;
	UWORD			Chars;
	UWORD			LayoutSpace;
	WORD			ExtraSpace;
	WORD			LabelChars;

	UBYTE			Type;
	BYTE			LabelPlace;
	UBYTE			Key;
	UBYTE			StorageType;
	BOOLEAN			DefaultSize;
	BOOLEAN 		Disabled;
	BOOLEAN 		NoKey;
	BOOLEAN 		HighLabel;
	BOOLEAN 		GroupIndent;
	BOOLEAN 		PageSelector;

	union
	{
		GaugeExtra		Gauge;
		TapeDeckExtra		TapeDeck;
		ButtonExtra		Button;
		BarExtra		Bar;
		BoxExtra		Box;
		FrameExtra		Frame;
		PickerExtra		Picker;
		IncrementerExtra	Incrementer;
		GroupExtra		Group;
		ListExtra		List;
		RadioExtra		Radio;
		TextExtra		Text;
		NumberExtra		Number;
		CycleExtra		Cycle;
		PalExtra		Palette;
		SliderExtra		Slider;
		StringExtra		String;
		IntegerExtra		Integer;
		ScrollerExtra		Scroller;
		LevelExtra		Level;
		BOOPSIExtra		BOOPSI;
		PopupExtra		Popup;
		TabExtra		Tab;
		ImageExtra		Image;
	} Special;
} ObjectNode;

typedef struct LayoutHandle
{
		// WARNING: Beginning of this structure must match the definition in gtlayout.h!

	struct Screen *		Screen;
	struct DrawInfo *	DrawInfo;
	struct Window *		Window;
	APTR			VisualInfo;
	struct Image *		AmigaGlyph;
	struct Image *		CheckGlyph;
	APTR			UserData;
	struct Menu *		Menu;

		// End of public part

	struct Screen *		PubScreen;
	struct MsgPort *	MsgPort;
	struct Gadget **	GadgetArray;
	struct Gadget *		Previous;
	struct Gadget *		List;
	struct Image *		BevelImage;
	struct Image *		GrooveImage;
	APTR			WindowUserData;
	ULONG			IDCMP;
	ULONG			WA_ScreenTag;
	BOOL			UnlockPubScreen;
	BOOL			UseGroove;

		// BOOPSI support

	Object *		BOOPSIPrevious;
	Object *		BOOPSIList;

		// Incrementers

	ObjectNode *		ActiveIncrementer;
	WORD			IncrementerCountdown;
	WORD			IncrementerAccelerate;
	LONG			IncrementerIncrement;

		// Frames

	ObjectNode *		ActiveFrame;

		// Cached from DrawInfo structure

	UWORD			TextPen;
	UWORD			BackgroundPen;
	UWORD			ShadowPen;
	UWORD			ShinePen;

	UWORD			AspectX;
	UWORD			AspectY;

		// Double-click support

	ULONG			ClickSeconds;
	ULONG			ClickMicros;
	ObjectNode *		ClickObject;

	struct Hook *		LocaleHook;
	struct TextAttr *	TextAttr;
	struct TextAttr *	InitialTextAttr;
	struct TTextAttr	CopyTextAttr;
	struct RastPort 	RPort;

	APTR			Pool;

	UWORD			GlyphWidth;
	UWORD			GlyphHeight;

	UWORD			InterWidth;
	UWORD			InterHeight;

	LONG			Count;

	LONG			Index;

	ObjectNode *		TopGroup;
	ObjectNode *		CurrentGroup;
	ObjectNode *		EscKey;
	ObjectNode *		ReturnKey;
	ObjectNode *		CursorKey;
	ObjectNode *		GrowView;
	ObjectNode *		ResizeObject;
	ObjectNode *		TabKey;
	ObjectNode *		ActiveString;
#ifdef DO_PASSWORD_KIND
	struct Hook		PasswordEditHook;
#endif
#ifdef DO_HEXHOOK
	struct Hook		HexEditHook;
#endif	/* DO_HEXHOOK */
	struct Hook		DefaultEditHook;
	struct Hook *		StandardEditHook;

	struct Hook		BackfillHook;
	struct RastPort		BackfillRastPort;

#ifdef DO_LEVEL_KIND
	ObjectNode *		CurrentLevel;
#endif	/* DO_LEVEL_KIND */

	struct TTextAttr	FixedFont;

	struct LayoutHandle *	PointBack;

	LONG			GroupID;

#ifdef DO_PICKSHORTCUTS
	UBYTE			Keys[256];
#endif	/* DO_PICKSHORTCUTS */

	struct Window *		Parent;		// Parent of this window
	struct Hook *		HelpHook;		// Hook to call when help key is pressed
	LONG			MaxPen;

	WORD			SizeWidth;
	WORD			SizeHeight;

	BOOLEAN			SizeVerified;
	BOOLEAN 		AutoRefresh;
	BOOLEAN 		CloseFont;
	BOOLEAN 		Failed;
	BOOLEAN 		Rescaled;
	BOOLEAN 		AutoActivate;
	BOOLEAN 		MoveToWindow;
	BOOLEAN			NeedDelay;
	BOOLEAN			RawKeyFilter;
	BOOLEAN			FlushLeft;
	BOOLEAN			FlushTop;
	BOOLEAN			BlockParent;
	BOOLEAN			ExitFlush;
	BOOLEAN			NoKeys;

#ifdef DO_CLONING
	struct CloneExtra *	CloneExtra;		// Screen cloning information
	STRPTR			CloneScreenTitle;	// Title of the cloned screen, if any
	BOOLEAN 		CloningPermitted;	// Genetic engineering?
	BOOLEAN 		ExactClone;		// Make an exact copy, i.e. preserve all colours
	BOOLEAN			SimpleClone;		// Just a simple clone, please
	BYTE			Pad0;
#endif

	BOOLEAN			CloseTopGroup;		// Close the top group at LT_Build() time
} LayoutHandle;

#define PHANTOM_GROUP_ID	-10000	// Phantom groups, i.e those without proper
					// IDs start with this ID

/*****************************************************************************/


	// Window locking

typedef struct LockNode
{
	struct MinNode		MinNode;
	struct Window *		Window;
	LONG			Count;
	ULONG			OldIDCMPFlags;
	struct Requester	Requester;
	LONG			MinWidth;
	LONG			MinHeight;
	LONG			MaxWidth;
	LONG			MaxHeight;
} LockNode;


/*****************************************************************************/


	// Image class extension

typedef struct ImageInfo
{
	STRPTR *		Lines;		// Text lines
	STRPTR			KeyStroke;	// Points to char that acts as keystroke identifier

	STRPTR			Label;		// Single label if any

	struct TextFont *	Font;		// Open font if any
	struct Image *		FrameImage;	// Button frame imagery, if any

	UWORD			ImageType;	// Type

	UWORD			GlyphWidth;	// Size
	UWORD			GlyphHeight;

	UWORD			LineCount;	// Number of lines below

	BOOLEAN			Emboss;	// Emboss the bevel box
	BOOLEAN			UseFrame;	// Always use the frame image?
} ImageInfo;

	// Image class extension types

#define IIA_ImageType	(TAG_USER+739)
#define IIA_GlyphWidth	(TAG_USER+740)
#define IIA_GlyphHeight	(TAG_USER+741)
#define IIA_Lines	(TAG_USER+742)
#define IIA_LineCount	(TAG_USER+743)
#define IIA_KeyStroke	(TAG_USER+744)
#define IIA_Emboss	(TAG_USER+745)
#define IIA_Label	(TAG_USER+746)
#define IIA_Font	(TAG_USER+747)

	// Supported image class extension types

enum
{
	IMAGECLASS_PICKER,
	IMAGECLASS_LEFTINCREMENTER,
	IMAGECLASS_RIGHTINCREMENTER,
	IMAGECLASS_BACKWARD,
	IMAGECLASS_FORWARD,
	IMAGECLASS_PREVIOUS,
	IMAGECLASS_NEXT,
	IMAGECLASS_RECORD,
	IMAGECLASS_PLAY,
	IMAGECLASS_STOP,
	IMAGECLASS_PAUSE,
	IMAGECLASS_REWIND,
	IMAGECLASS_EJECT,
	IMAGECLASS_MULTILINEBUTTON
};


/*****************************************************************************/


struct SliderClassData
{
	BOOL			IsVertical;
	WORD			UseTicks;

	UWORD			Width;
	UWORD			Height;

	UWORD			SeparatorSpace;
	UWORD			TickSpace;

	UWORD			TickTop;
	UWORD			RailTop;
	UWORD			ThumbTop;

	UWORD			ThumbWidth;
	UWORD			ThumbHeight;

	UWORD			ThumbLeft;
	UWORD			ThumbLast;
	ULONG			ThumbPosition;
	ULONG			ThumbMax;
	ULONG			NumTicks;

	struct DrawInfo *	DrawInfo;

	struct BitMap		ThumbBitMap;

	LONG			ActiveOffset;
	BOOL			DirectHit;
};

#define	SLA_Dummy 	(TAG_USER+0x700000)

#define SLA_DrawInfo	(SLA_Dummy+1)
#define SLA_Current	(SLA_Dummy+2)
#define SLA_Max		(SLA_Dummy+3)
#define SLA_Position	(SLA_Dummy+4)
#define SLA_NumTicks	(SLA_Dummy+5)
#define SLA_UseTicks	(SLA_Dummy+6)
#define SLA_Reference	(SLA_Dummy+7)


/*****************************************************************************/


enum
{
	STORAGE_BYTE,STORAGE_UBYTE,
	STORAGE_WORD,STORAGE_UWORD,
	STORAGE_LONG,STORAGE_ULONG,
	STORAGE_STRPTR
};


/*****************************************************************************/


	// A single colour in 96 bit precision

struct ColourTriplet
{
	ULONG Red;		// 32 bits each
	ULONG Green;
	ULONG Blue;
};

	// A colour table ready to submit to LoadRGB32

struct ColourRecord
{
	UWORD			NumColours;	// Number of colours in this chunk
	UWORD			FirstColour;	// First colour entry to set

	struct ColourTriplet	Triplets[0];	// Table entries

	WORD			Arnold; 	// Array terminator, needs to be set to zero
};

	// Screen reproduction

struct CloneExtra
{
	struct Screen *	Screen;
	LONG *		Pens;
	UWORD *		ScreenPens;
	UWORD		MinWidth,
			MinHeight,
			MaxWidth,
			MaxHeight,
			TotalPens;
	UBYTE		Depth;
	struct IBox	Bounds;
};


/*****************************************************************************/


#define ITEMF_HasSub	(1 << 0)		// Next item is a subitem
#define ITEMF_FirstSub	(1 << 1)		// This is the first subitem
#define ITEMF_IsSub	(1 << 2)		// This is a subitem
#define ITEMF_LastItem	(1 << 3)		// This is the last item
#define ITEMF_IsBar	(1 << 4)		// This is a separator bar
#define ITEMF_Command	(1 << 5)		// This item has a long command key sequence

typedef struct ItemNode
{
	struct MinNode	 Node;
	struct MenuItem	 Item;
	APTR		 UserData;
	ULONG		 ID;			// Item ID

	STRPTR		 ExtraLabel;		// For long command sequence labels
	UWORD		 Flags;			// Flag bits as defined above
	WORD		 Left,			// Actual left edge
			 Top;			// Actual top edge
	WORD		 Width;			// Actual width

	UWORD		 MenuCode;		// Intuition menu code

	ULONG		 Qualifier;		// Message qualifier
	UWORD		 Code;			// Message code
	UBYTE		 Char;			// ASCII code
} ItemNode, *pItemNode;

typedef struct MenuNode
{
	struct MinNode	 Node;
	struct Menu	 Menu;
	APTR		 UserData;
	ULONG		 ID;			// Item ID

	WORD		 Width;			// Actual width

	UWORD		 MenuCode;		// Intuition menu code
} MenuNode, *pMenuNode;

typedef struct RootMenu
{
	APTR			Pool;		// Memory allocation
	struct TextFont *	Font;		// Preferred item font
	struct TextAttr *	TextAttr;	// What goes into the single items
	struct TTextAttr	BoldAttr;	// Same as above, just in boldface
	struct DrawInfo *	DrawInfo;	// Screen drawing info
	struct Screen *		Screen;		// The screen we are building the menu for
	struct RastPort		RPort;		// Dummy RastPort for font measuring

	WORD			ItemHeight;	// Default item height
	UWORD			TextPen;	// Text rendering pen

	ULONG			CheckWidth,	// Size of the checkmark glyph
				CheckHeight,
				AmigaWidth,	// Size of the Amiga glyph
				AmigaHeight;

	struct MinList		MenuList;	// List of menus
	struct MinList		ItemList;	// List of menu items

	LayoutHandle *		Handle;		// Layout handle if any
	struct Hook *		LocaleHook;	// Localization, if no handle is provided

	struct MinNode		Node;		// The initial menu all the data is tied to
	struct Menu		Menu;
	APTR			UserData;
	ULONG			ID;
	WORD			Width;
	UWORD			MenuCode;
} RootMenu, *pRootMenu;


/*****************************************************************************/


typedef struct PopInfo
{
	struct CheckGlyph *	CheckGlyph;

	LONG			MaxLen;
	LONG			MaxWidth;

	struct Window *		Window;
	struct TextFont	*	Font;

	LONG			TopMost;

	LONG			LastDrawn;
	LONG			LastLabelDrawn;

	struct Image *		FrameImage;

	STRPTR *		Labels;
	LONG			NumLabels;
	LONG			Active;
	LONG			InitialActive;
	UWORD			ActiveLen;

	UWORD			MarkLeft;
	UWORD			MarkWidth;

	UWORD			LabelLeft;
	UWORD			LabelTop;

	UWORD			LineTop;

	UWORD			ArrowTop;
	UWORD			ArrowWidth;
	UWORD			ArrowHeight;

	UWORD			PickerWidth;

	UWORD			PopLeft;
	UWORD			PopWidth;

	UWORD			BoxLeft,BoxTop,
				BoxWidth,BoxHeight;

	UWORD			BoxLines;

	UWORD			SingleWidth,
				SingleHeight;

	UWORD			AspectX;
	UWORD			AspectY;

	UWORD			MenuText,MenuBack;
	UWORD			MenuTextSelect,MenuBackSelect;

	UBYTE			Flags;
	BOOLEAN			Blocked;
	BOOLEAN			CentreActive;
} PopInfo;

#define PIA_Labels		(TAG_USER+0x80000)
#define PIA_Active		(TAG_USER+0x80001)
#define PIA_Font		(TAG_USER+0x80002)
#define PIA_Highlight		(TAG_USER+0x80003)
#define PIA_CentreActive	(TAG_USER+0x80004)
#define PIA_AspectX		(TAG_USER+0x80005)
#define PIA_AspectY		(TAG_USER+0x80006)


/*****************************************************************************/


typedef struct TabEntry
{
	struct BitMap *	BitMap;
	UWORD		Left;
} TabEntry;

typedef struct TabInfo
{
	struct BitMap *	BitMap;
	struct RastPort	RPort;
	PLANEPTR	Mask;

	TabEntry *	Tabs;
	WORD		Count;
	WORD		Current;
	WORD		Initial;

	UWORD		Thick;
	UWORD		TabWidth;
	UWORD		TabHeight;

	WORD		Offset;
} TabInfo;

#define TIA_Labels	(TAG_USER+0x90000)
#define TIA_Font	(TAG_USER+0x90001)
#define TIA_Screen	(TAG_USER+0x90002)
#define TIA_Index	(TAG_USER+0x90003)
#define TIA_DrawInfo	(TAG_USER+0x90004)
#define TIA_SizeType	(TAG_USER+0x90005)


/*****************************************************************************/


struct CheckGlyph
{
	UWORD		Width,
			Height;
	struct BitMap *	Plain;
	struct BitMap *	Selected;
};


/*****************************************************************************/

/* These may not be defined in the default header files. */

#ifndef GDOMAIN_MINIMUM
#define	GDOMAIN_MINIMUM (0)	/* Minimum size */
#endif	/* GDOMAIN_MINIMUM */

#ifndef GDOMAIN_NOMINAL
#define	GDOMAIN_NOMINAL	(1)	/* Nominal size */
#endif	/* GDOMAIN_NOMINAL */

#ifndef GDOMAIN_MAXIMUM
#define	GDOMAIN_MAXIMUM	(2)	/* Maximum size */
#endif	/* GDOMAIN_MAXIMUM */


/*****************************************************************************/


#endif
