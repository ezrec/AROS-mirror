/*
**	Data.c
**
**	Global data structures.
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

BOOL			 DebugFlag;

	/* Libraries. */

struct ExecBase		*SysBase;
struct DosLibrary	*DOSBase;
struct Library		*GTLayoutBase;

struct IntuitionBase	*IntuitionBase;
struct GfxBase		*GfxBase;
struct RxsLib		*RexxSysBase;
struct LocaleBase	*LocaleBase;
struct Library		*GadToolsBase,
			*DiskfontBase,
#ifndef __AROS__
			*UtilityBase,
#endif
			*AslBase,
			*IFFParseBase,
			*CxBase,
			*IconBase,
			*LayersBase,
			*TranslatorBase,
			*WorkbenchBase,
			*AmigaGuideBase,
			*XEmulatorBase,
			*XProtocolBase,
			*DataTypesBase;

#ifdef __AROS__
struct UtilityBase      *UtilityBase;
#endif
struct Device		*TimerBase,
			*ConsoleDevice;
BOOL			 Kick30;

	/* Device locking. */

struct Library		*OwnDevUnitBase;
BYTE			 OwnDevBit = -1;

	/* Timer device. */

struct timerequest	*TimeRequest;
struct MsgPort		*TimePort;

	/* Console device. */

struct IOStdReq		*ConsoleRequest;
struct KeyMap		*KeyMap;
BPTR			 KeySegment;

	/* Screen & Windows. */

struct Screen		*Screen,
			*SharedScreen;
struct Window		*Window;
ULONG			 OpenWindowTag;
LONG			 WindowWidth,
			 WindowHeight,
			 WindowLeft,
			 WindowTop;
struct Window		*StatusWindow;
UWORD			 StatusOffset;
struct RastPort		 StatusRastPort,
			*StatusRPort;
UWORD			 StatusDisplayLeft,
			 StatusDisplayTop,
			 StatusDisplayWidth,
			 StatusDisplayHeight;
APTR			 OldWindowPtr;
UBYTE			 ScreenTitle[80];
UBYTE			 WindowTitle[80];
UBYTE			 DepthMask;
LONG			 RenderPens[4],
			 MappedPens[2][32],
			 SafeTextPen;
struct DrawInfo		*DrawInfo;
UWORD			*Pens;
LONG			 ScreenWidth,
			 ScreenHeight;
BOOL			 UseMasking;

struct BlockMarker	*WindowMarker;
BOOL			 Marking;

	/* Graphics rendering. */

struct ViewPort		*VPort;
struct RastPort		*RPort;
struct Region		*ClipRegion,
			*OldRegion;
struct TextFont		*GFX,
			*CurrentFont,
			*UserTextFont;
WORD			 UserFontHeight,
			 UserFontWidth,
			 UserFontBase;
UBYTE			 UserFontName[MAX_FILENAME_LENGTH];
struct TextFont		*TextFont;
WORD			 TextFontHeight,
			 TextFontWidth,
			 TextFontBase;
struct TTextAttr	 TextAttr;
UBYTE			 TextFontName[MAX_FILENAME_LENGTH];
struct TTextAttr	 UserFont;
struct TagItem		 TagDPI[2];
APTR			 VisualInfo;
UWORD			 FontRightExtend;

ULONG			(* ReadAPen)(struct RastPort *RPort);
ULONG			(* ReadBPen)(struct RastPort *RPort);
ULONG			(* ReadDrMd)(struct RastPort *RPort);
ULONG			(* SetMask)(struct RastPort *RPort,ULONG Mask);
ULONG			(* GetMask)(struct RastPort *RPort);

UWORD MonoPens[] =
{
	0,1,1,1,1,1,0,0,1,	0,1,1,(UWORD)~0
};

UWORD StandardPens[] =
{
	0,1,1,2,1,3,1,0,2,	1,2,1,(UWORD)~0
};

UWORD ANSIPens[] =
{
	0,4,7,7,4,6,7,0,6,	0,7,0,(UWORD)~0
};

UWORD EGAPens[] =
{
	0,15,15,15,8,7,15,0,7,	0,15,0,(UWORD)~0
};

UWORD NewEGAPens[] =
{
	7,0,0,15,0,6,0,7,15,	0,15,0,(UWORD)~0
};

UWORD AtomicColours[16] =
{
	0x000,
	0xDDD,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000,
	0x000
};

UWORD EGAColours[16] =
{
	0x000,	/* Black. */
	0xA00,	/* Dark red. */
	0x0A0,	/* Dark green. */
	0xA60,	/* Dark yellow (orange). */
	0x00A,	/* Dark blue. */
	0xA0A,	/* Dark magenta.*/
	0x0AA,	/* Dark cyan. */
	0xBBB,	/* Light grey. */
	0x666,	/* Dark grey. */
	0xF00,	/* Red. */
	0x0F0,	/* Green. */
	0xFF0,	/* Yellow. */
	0x00F,	/* Blue. */
	0xF0F,	/* Magenta. */
	0x0FF,	/* Cyan. */
	0xFFF	/* White. */
};

UWORD ANSIColours[16] =
{
	0x000,	/* Black. */
	0xF00,	/* Red. */
	0x0F0,	/* Green. */
	0xFF0,	/* Yellow. */
	0x00F,	/* Blue. */
	0xF0F,	/* Magenta. */
	0x0FF,	/* Cyan. */
	0xFFF,	/* White. */

	0x000,	/* Black. */
	0xF00,	/* Red. */
	0x0F0,	/* Green. */
	0xFF0,	/* Yellow. */
	0x00F,	/* Blue. */
	0xF0F,	/* Magenta. */
	0x0FF,	/* Cyan. */
	0xFFF	/* White. */
};

UWORD			 DefaultColours[32],
			 BlinkColours[32],
			 NormalColours[32],
			 StandardColours[32],
			 PaletteSize = 16;

	/* Line/Column offset tables. */

LONG			*OffsetXTable,
			*OffsetYTable;

	/* DOS data. */

struct Process		*ThisProcess;
struct Process		*StatusProcess;
struct Process		*RexxProcess;
struct MsgPort		*TermRexxPort;
struct WBStartup	*WBenchMsg;
BPTR			 WBenchLock;
struct TermPort		*TermPort;
BYTE			 CheckBit	= -1;
BOOL			 InRexx;
struct timeval		 LimitTime;
UBYTE			 LimitMacro[MAX_FILENAME_LENGTH];

	/* Character conversion. */

struct InputEvent	*FakeInputEvent;

	/* Serial I/O. */

struct IOExtSer		*ReadRequest;
struct IOExtSer		*WriteRequest;
struct MsgPort		*ReadPort;
struct MsgPort		*WritePort;
UBYTE			 ReadChar;
UBYTE			*ReadBuffer,
			*HostReadBuffer;
ULONG			 DTERate,
			 SerialBufferSize;
BOOL			 ProcessIO = TRUE;
BOOL			 FirstInvocation;

UBYTE			 NewDevice[MAX_FILENAME_LENGTH];
LONG			 NewUnit;
BOOL			 UseNewDevice,
			 UseNewUnit;

LONG BaudRates[] =
{
	110,	300,	600,	1200,	2400,	4800,	7200,
	9600,	12000,	14400,	19200,	31250,	38400,	57600,
	64000,	76800,	115200,	230400,	345600,	460800,	576000,
	614400,	691200,	806400,	921600
};

WORD NumBaudRates = NUM_ELEMENTS(BaudRates);

struct RendezvousSemaphore	RendezvousSemaphore;

	/* Localization support. */

struct Catalog	*Catalog;
struct Locale	*Locale;
STRPTR		 DecimalPoint;
UBYTE		 ConvNumber[30],
		 ConvNumber10[30];

	/* Hotkeys. */

struct Hotkeys Hotkeys;

	/* Static default fonts. */

struct TextAttr DefaultFont =
{
	(UBYTE *)"topaz.font",
	8,
	FS_NORMAL,
	FPF_ROMFONT | FPF_DESIGNED
};

struct TextAttr GFXFont =
{
	(UBYTE *)"GFX.font",
	8,
	FS_NORMAL,
	FPF_DISKFONT
};

	/* Character lookup tables. */

BYTE IsPrintable[256] =
{
	0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,
	0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

BYTE IsGlyph[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

BYTE CharType[256] =
{
	0,0,0,0,0,0,0,0,0,0,6,0,0,7,0,0,
	0,4,0,5,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,
	0,0,0,0,0,3,1,1,1,1,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

	/* ISO -> IBM font conversion. */

UBYTE IBMConversion[256] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x00,0x00,
	0x00,0x11,0x00,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x00,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xAD,0x9B,0x9C,0x00,0x9D,0x00,0x15,0x00,0x00,0xA6,0x00,0x00,0x00,0xAE,0x00,
	0xF8,0xF1,0xFD,0x00,0x00,0xE6,0x14,0xF9,0x00,0x00,0xA7,0xAF,0xAC,0xAB,0x00,0xA8,
	0x00,0x00,0x00,0x00,0x8E,0x8F,0x92,0x80,0x00,0x90,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xA5,0x00,0x00,0x00,0x00,0x99,0x00,0x00,0x00,0x00,0x00,0x9A,0x00,0x00,0xE1,
	0x85,0xA0,0x83,0x00,0x84,0x86,0x91,0x87,0x8A,0x82,0x88,0x89,0x8D,0xA1,0x8C,0x00,
	0x00,0xA4,0x95,0xA2,0x93,0x00,0x94,0x00,0x00,0x97,0xA3,0x96,0x81,0x00,0x00,0x98
};

	/* IBM -> ISO font conversion. */

UBYTE ISOConversion[256] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x00,0x00,
	0x00,0x11,0x00,0x13,0xB6,0xA7,0x00,0x00,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x00,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	0xC7,0xFC,0xE9,0xE2,0xE4,0xE0,0xE5,0xE7,0xEA,0xEB,0xE8,0x00,0xEE,0xEC,0xC4,0xC5,
	0xC9,0xE6,0xC6,0xF4,0xF6,0xF2,0xFB,0xF9,0xFF,0xD6,0xDC,0xA2,0xA3,0xA5,0x00,0x00,
	0xE1,0xED,0xF3,0xFA,0xF1,0xD1,0xAA,0xBA,0xBF,0x00,0x00,0xBD,0xBC,0xA1,0xAE,0xBB,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xDF,0x00,0x00,0x00,0x00,0xB5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xB1,0x00,0x00,0x00,0x00,0x00,0x00,0xB0,0xB7,0x00,0x00,0x00,0xB2,0x00,0x00
};

	/* Table of graphics characters. */

BYTE GfxTable[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

	/* Special character treatment. */

struct SpecialKey SpecialKeys[14] =
{
	XOF,	(JUMP)DoxON_xOFF,	/* Halt data flow. */
	XON,	(JUMP)DoxON_xOFF,	/* Restart data flow. */
	BEL,	(JUMP)DoBeep,		/* Ring the bell. */
	BKS,	(JUMP)DoBackspace,	/* Erase a character. */
	ENT,	(JUMP)DoLF_FF_VT,	/* Enter key. */
	FFD,	(JUMP)DoFF,		/* Form feed. */
	VTB,	(JUMP)DoLF_FF_VT,	/* Vertical tab (huh?). */
	RET,	(JUMP)DoCR,		/* Return key. */
	TAB,	(JUMP)DoTab,		/* Move to next tab stop. */
	SI,	(JUMP)DoShiftIn,	/* Change to graphics mode */
	SO,	(JUMP)DoShiftOut,	/* Change out of graphics mode. */
	ENQ,	(JUMP)DoEnq,		/* Transmit answerback message. */
	ESC,	(JUMP)DoEsc,		/* Start new control sequence. */
	CSI,	(JUMP)DoCsi		/* Start new control sequence. */
};

JUMP *SpecialTable;

BYTE AbortMap[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,1,2,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

JUMP *AbortTable;

CONTRANSFER ConDump,ConOutput,ConProcessData;

	/* Some more handy buffers. */

UBYTE ARexxCommandBuffer[256],AmigaDOSCommandBuffer[256],DialNumberBuffer[256];

	/* Screen pull-down-menu. */

struct NewMenu		*TermMenu;
WORD			 NumMenuEntries;

struct SignalSemaphore	MenuSemaphore;
struct Menu		*Menu;
struct Image		*AmigaGlyph,*CheckGlyph;

	/* Cross-hatch pattern. */

ULONG Crosshatch = 0x5555AAAA;

	/* Configuration. */

struct Configuration	*Config,
			*PrivateConfig,
			*BackupConfig;
struct SpeechConfig	 SpeechConfig;
BOOL			 English = TRUE;
BOOL			 Online,WasOnline;
ULONG			 OnlineMinutes;
struct SignalSemaphore	 OnlineSemaphore;

UBYTE			 LastConfig[MAX_FILENAME_LENGTH];
UBYTE			 DefaultPubScreenName[MAXPUBSCREENNAME + 1],
			 SomePubScreenName[MAXPUBSCREENNAME + 1];

WORD			 StatusStackPointer;
WORD			 StatusStack[10];
struct SignalSemaphore	 StatusSemaphore;

	/* xON/xOFF processing. */

struct SignalSemaphore	 xONxOFF_Semaphore;
LONG			 xONxOFF_Lock;
BOOL			 xOFF_Enabled;

	/* Capture file. */

struct Buffer		*FileCapture;
BPTR			 PrinterCapture;
UBYTE			 CaptureName[MAX_FILENAME_LENGTH];
BOOL			 StandardPrinterCapture,
			 ControllerActive;
LONG			 BufferFlushCount;
COPTR			 CaptureData;
BOOL			 RawCapture;
ParseContext		*ParserStuff;

	/* Buffer. */

UBYTE			**BufferLines;
LONG			 Lines;
struct SignalSemaphore	 BufferSemaphore;
LONG			 MaxLines = 100;
LONG			 BufferSpace;
BOOL			 BufferClosed;
BOOL			 BufferFrozen;

struct MinList		 TextBufferHistory;

	/* Sound support.c */

struct SoundConfig	 SoundConfig;
BOOL			 SoundChanged;

	/* Phonebook. */

PhonebookHandle		*GlobalPhoneHandle;
BOOL			 RebuildMenu;
UBYTE			 Password[100];
UBYTE			 UserName[100];

UBYTE			 CurrentBBSName[40],
			 CurrentBBSNumber[100],
			 CurrentBBSComment[100];

UBYTE			 LastPhone[MAX_FILENAME_LENGTH];
UBYTE			 LastKeys[MAX_FILENAME_LENGTH];
UBYTE			 LastMacros[MAX_FILENAME_LENGTH];
UBYTE			 LastCursorKeys[MAX_FILENAME_LENGTH];
UBYTE			 LastTranslation[MAX_FILENAME_LENGTH];
UBYTE			 LastSpeech[MAX_FILENAME_LENGTH];
UBYTE			 LastSound[MAX_FILENAME_LENGTH];

struct MacroKeys	*MacroKeys;
struct CursorKeys	*CursorKeys;

	/* Console stuff. */

UBYTE			*StripBuffer;
UWORD			 LastLine,LastColumn,LastPixel;
UWORD			 LastPrintableColumn;
UWORD			 LastPrintablePixel;
UWORD			 CharCellNominator,CharCellDenominator;
UBYTE			 CurrentFontScale,FontScalingRequired,CurrentCharWidth;
UBYTE			 Charset,Attributes,FgPen,BgPen;
WORD			 CursorX,CursorY,Top,Bottom;
BOOL			 UseRegion,RegionSet,InSequence,ConsoleQuiet,Blocking;
BYTE			*TabStops;
UWORD			 TabStopMax;
BYTE			 CharMode[2];

BOOL			 VT52_Mode;

BOOL			 ResetDisplay,
			 FixScreenSize;

struct SignalSemaphore	 TerminalSemaphore;	/* Protection from shape changers */

	/* Flow filter. */

STRPTR			 AttentionBuffers[SCAN_COUNT];
BOOL			 BaudPending,UseFlow;
UBYTE			 BaudBuffer[80],BaudCount;

struct FlowInfo		 FlowInfo;

STRPTR			 DataHold;
LONG			 DataSize;

	/* Transfer data. */

BOOL			 BinaryTransfer = TRUE;
LONG			 TransferAbortState;
WORD			 TransferAbortCount;
LONG			 TransferBits;

CONTRANSFER		 ConTransfer;

UBYTE			*DownloadPath;
BOOL			 Uploading;
BOOL			 DidTransfer;

struct Window		*TransferWindow;
UBYTE			 TransferTitleBuffer[256];
BOOL			 TransferZoomed;
STRPTR			 TransferWindowTitle;

struct LayoutHandle	*TransferHandle;
struct List		 TransferInfoList;

struct FileTransferInfo	*FileTransferInfo;

struct XPR_IO		*XprIO;
struct MinList		 Update_MsgList;
WORD			 Update_MsgCount;
UBYTE			 ProtocolOptsBuffer[256];
BOOL			 NewOptions;
BOOL			 XPRCommandSelected;
UBYTE			 LastXprLibrary[MAX_FILENAME_LENGTH];
UBYTE			 TransferProtocolName[MAX_FILENAME_LENGTH];
BOOL			 UsesZModem,SendAbort,TransferAborted,TransferFailed,TransferError;

struct Process		*TransferProcess;

	/* The standard ZModem cancel sequence
	 * (ten CAN chars followed by ten
	 * BS chars).
	 */

UBYTE ZModemCancel[] =
{
	24,24,24,24,24,24,24,24,24,24,
	 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

	/* Pay per minute. */

LONG			 PayPerUnit[2];
LONG			 SecPerUnit[2];
struct List		*PatternList,
			*ActivePattern;
UBYTE			 LastPattern[MAX_FILENAME_LENGTH];
BOOL			 PatternsChanged;
struct SignalSemaphore	 PatternSemaphore;

	/* Execute a command/script. */

ULONG			 BlockNestCount;
BOOL			 WeAreBlocking;

	/* Dial list. */

BOOL			 SendStartup;
LONG			 FirstDialMenu = -1;
struct List		 RexxDialMsgList;

	/* This variable stores the current state of the
	 * dialing menu items.
	 */

BOOL			 DialItemsAvailable = TRUE;

	/* TermMain data. */

BOOL			 ReleaseSerial;
BOOL			 MainTerminated,
			 IconTerminated = TRUE,
			 DoIconify,
			 DoDial = DIAL_IGNORE,
			 KeepQuiet;

BOOL			 ConfigChanged,PhonebookChanged,TranslationChanged,
			 MacroChanged,CursorKeysChanged,FastMacrosChanged,
			 HotkeysChanged,SpeechChanged,BufferChanged;

	/* String gadget hooks. */

struct Window		*CommandWindow;
struct Gadget		*CommandGadget;

BOOL			 ClipInput,
			 ClipXerox,
			 ClipPrefix;

	/* Text sending. */

UBYTE			 SendPrompt[256];
LONG			 SendPromptLen;
SENDLINE		 SendLine;

	/* Packet window. */

struct Window		*PacketWindow;
struct Gadget		*PacketGadgetArray[2];
struct Menu		*PacketMenu;
struct Gadget		*PacketGadgetList;
struct List		 PacketHistoryList;

	/* Some more colour control flags. */

BOOL			 Initializing,LoadColours;

	/* The character raster. */

UBYTE			*Raster,*RasterAttr;
LONG			 RasterWidth,RasterHeight;
struct SignalSemaphore	 RasterSemaphore;
BOOL			 RasterEnabled = TRUE;

	/* Generic lists. */

struct GenericList	*GenericListTable[GLIST_COUNT];

	/* File version test. */

ULONG VersionProps[2] =
{
	ID_TERM,
	ID_VERS
};

	/* Window stack. */

struct Window		*TopWindow;

struct Window		*WindowStack[5];
WORD			 WindowStackPtr;

	/* Global term ID. */

LONG			 TermID;
UBYTE			 TermIDString[MAXPUBSCREENNAME + 1];
UBYTE			 RexxPortName[256];

	/* Fast! macros. */

struct List		 FastMacroList;
LONG			 FastMacroCount;
UBYTE			 LastFastMacros[MAX_FILENAME_LENGTH];
struct Window		*FastWindow;
WORD			 FastWindowLeft		= -1,
			 FastWindowTop		= -1,
			 FastWindowHeight	= -1;

	/* Double-buffered file locking. */

struct List		 DoubleBufferList;
struct SignalSemaphore	 DoubleBufferSemaphore;

	/* XEM interface. */

struct XEM_IO		*XEM_IO;
ULONG			 XEM_Signal;
struct XEmulatorHostData XEM_HostData;
UBYTE			*OptionTitle;
struct List		 XEM_MacroList;
struct XEmulatorMacroKey *XEM_MacroKeys;
UBYTE			 EmulationName[MAX_FILENAME_LENGTH];

	/* Screen scrolling support. */

UWORD			 ScrollLineFirst,ScrollLineLast,
			 ScrollLineCount;
struct ScrollLineInfo	*ScrollLines;

	/* term review buffer. */

struct Window		*ReviewWindow;

	/* Background information. */

ULONG			 BytesIn,
			 BytesOut;
struct DateStamp	 SessionStart;
struct Window		*InfoWindow;

	/* Gadget support routines */

LONG			 InterWidth,
			 InterHeight;

	/* Character translation. */

struct TranslationEntry	**SendTable,
			**ReceiveTable;

	/* Window alignment information. */

struct WindowInfo WindowInfoTable[] =
{
	WINDOW_PACKET,		WC_ALIGNBELOW|WC_EXPANDWIDTH,	0,0,0,0,
	WINDOW_STATUS,		WC_ALIGNRIGHT,			0,0,0,0,
	WINDOW_REVIEW,		WC_ALIGNTOP|WC_EXPANDWIDTH,	0,0,0,0,
	WINDOW_FAST,		WC_ALIGNTOP|WC_ALIGNSIDE,	0,0,0,0,
	WINDOW_MAIN,		NULL,				-1,-1,0,0,
	WINDOW_CHARTAB,		WC_ALIGNTOP,			0,0,0,0,
	WINDOW_QUEUE,		WC_ALIGNLEFT|WC_ALIGNTOP,	0,0,0,0,
	WINDOW_FILETRANSFER,	WC_ALIGNLEFT|WC_ALIGNTOP,	0,0,0,0,

	-1
};

struct SignalSemaphore WindowInfoSemaphore;

	/* New user interface. */

struct Hook LocaleHook;

	/* AmigaGuide hook. */

struct Hook GuideHook;

	/* Special queue management. */

struct MsgQueue	*SpecialQueue;

	/* Window and Screen backfill hook. */

struct Hook BackfillHook;

ULONG BackfillTag;

	/* Intercept data going through SerWrite. */

BYPASS	SerWriteBypass;

	/* The incredibly shrinking file name. */

UBYTE	ShrunkenName[MAX_FILENAME_LENGTH],
	OriginalName[MAX_FILENAME_LENGTH];

	/* Terminal emulation task. */

struct MsgQueue *TerminalQueue;

	/* AppIcon data. */

STATIC UWORD DropData[304] =
{
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x2000,0x1000,0x0000,0x0000,0xF000,0x1000,
	0x0000,0x0001,0x9800,0x1000,0x0000,0x0003,0x1C3E,0x1000,
	0x0000,0x0003,0xFC30,0x1000,0x0000,0x0000,0x01FF,0x1000,
	0x0000,0x0000,0x01FC,0x1000,0x0000,0x0060,0x0000,0x1000,
	0x0000,0x001F,0xFFFF,0xF000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0700,0x0000,
	0x07FF,0xFFFE,0x0700,0x0000,0x07FF,0xFFEE,0x0700,0x0000,
	0x0755,0x555E,0x0700,0x0000,0x07AA,0xAAAE,0x1FC0,0x0000,
	0x0755,0x555E,0x0F80,0x0000,0x07AA,0xAAAE,0x0700,0x0000,
	0x0755,0x555E,0x0200,0x0000,0x07AA,0xAAAE,0x0000,0x0000,
	0x0755,0x555E,0x0000,0x0000,0x07FF,0xFFFE,0x0000,0x0000,
	0x07FF,0xFFFE,0x0000,0x0000,0x07FF,0xFFFC,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x7FFF,0xFFFF,0xE000,0x0000,
	0x7FFF,0xFEFF,0xE000,0x0000,0x7FFF,0xFEF0,0x0000,0x0000,
	0x7FE5,0x54E7,0xFF80,0x0000,0x7FFF,0xFF8E,0x0FC0,0x0000,
	0x7FFF,0xFFBD,0xFCF0,0x0000,0x0FFF,0xFFFF,0xFEE0,0x0000,
	0x0FFF,0xFFFB,0xD700,0x0000,0x0000,0x0007,0xAF80,0x0000,
	0x0000,0x000F,0xFF80,0x0000,0x0000,0x0007,0xFF00,0x0000,

	0x0000,0x007F,0xFFFE,0x0000,0x0000,0x007F,0xFFFF,0x8000,
	0x0000,0x0060,0x20FF,0xE000,0x0000,0x0060,0xF0FF,0xE000,
	0x0000,0x0061,0x98FF,0xE000,0x0000,0x0063,0x1CBF,0xE000,
	0x0000,0x0063,0xFCB7,0xE000,0x0000,0x007F,0xFDFF,0xE000,
	0x0000,0x007F,0xFDFF,0xE000,0x0000,0x007F,0xFFFF,0xE000,
	0x0000,0x001F,0xFFFF,0xE000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x07FF,0xFFFC,0x0000,0x0000,
	0x0FFF,0xFFFC,0x0000,0x0000,0x0EAA,0xAABC,0x0000,0x0000,
	0x0F55,0x555C,0x0000,0x0000,0x0EAA,0xAABC,0x0000,0x0000,
	0x0F55,0x555C,0x0000,0x0000,0x0EAA,0xAABC,0x0000,0x0000,
	0x0F55,0x555C,0x0000,0x0000,0x0EAA,0xAABC,0x0000,0x0000,
	0x0FFF,0xFFFC,0x0000,0x0000,0x0FFF,0xFFFC,0x0000,0x0000,
	0x0FFF,0xFFFC,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x7FFF,0xFFFF,0xC000,0x0000,0xFFFF,0xFFFF,0xC000,0x0000,
	0xFFEA,0xABFF,0xC000,0x0000,0xFFE0,0x03FF,0xFF00,0x0000,
	0xFFFF,0xFFFF,0xFFC0,0x0000,0xFFFF,0xFFFF,0xF9E0,0x0000,
	0x0000,0x007F,0xFDE0,0x0000,0x1FFF,0xFFC7,0xAF00,0x0000,
	0x0000,0x0007,0x5F00,0x0000,0x0000,0x000F,0xFF80,0x0000,
	0x0000,0x001F,0xFF00,0x0000,0x0000,0x0000,0x0000,0x0000
};

STATIC struct Image DropImage =
{
	0,0,
	52,38,2,
	DropData,
	0x03,0x00,
	NULL
};

struct DiskObject DropIcon =
{
	WB_DISKMAGIC,
	WB_DISKVERSION,

	{NULL,
	175,18,
	52,39,
	0x0005,
	0x0003,
	0x0001,
	&DropImage,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL},

	WBTOOL,
	NULL,
	NULL,
	NO_ICON_POSITION,
	NO_ICON_POSITION,
	NULL,
	NULL,
	8192
};

	/* Remember some of the incoming data. */

BOOL	RememberOutput,
	RememberInput,

	Recording,
	RecordingLine;

	/* Trap settings. */

BOOL		 WatchTraps;
BOOL		 TrapsChanged;
UBYTE		 LastTraps[MAX_FILENAME_LENGTH];
ParseContext	*TrapStuff;

	/* CR/LF receiver translation data. */

TRANSLATEFUNC	Translate_CR_LF;

	/* End of line conversion labels. */

STRPTR EOL_Labels[] =
{
	"-",
	"«CR»",
	"«LF»",
	"«CR»«LF»",
	"«LF»«CR»",

	NULL
};


/**********************************************************************/


	/* Character access tables. */

STATIC UBYTE Table0[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Table1[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Table2[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Table3[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Table4[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Table5[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Table6[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Table7[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Table8[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

STATIC UBYTE Term0[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

	/* This follows the control code information. */

struct ControlCode ANSICode[] =
{
	/* Single character sequences */

	'A',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_CursorUp,		/* Cursor up */
	'B',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_CursorDown,		/* Cursor down */
	'C',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_CursorRight,		/* Cursor right */
	'D',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_CursorLeft,		/* Cursor left */
	'F',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* Enter graphics mode */
	'G',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* Exit graphics mode */
	'H',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_CursorHome,		/* Cursor to home */
	'I',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_RevLF,		/* Reverse line feed */
	'J',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_EraseEOS,		/* Erase to end of screen */
	'K',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_EraseEOL,		/* Erase to end of line */
	'W',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_PrintOn,		/* Enter printer controller mode */
	'X',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_PrintOff,		/* Exit printer controller mode */
	']',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_PrintScreen,		/* Print screen */
	'V',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_PrintLine,		/* Print cursor line */
	'^',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* Enter auto print mode */
	'_',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* Exit auto print mode */

/*	'D',	Table0,	NULL,	 0 ,	1,	(EPTR)CursorScrollDown,		// IND */
	'M',	Table0,	NULL,	 0 ,	1,	(EPTR)CursorScrollUp,		/* RI */
	'E',	Table0,	NULL,	 0 ,	1,	(EPTR)NextLine,			/* NEL */
	'7',	Table0,	NULL,	 0 ,	1,	(EPTR)SaveCursor,		/* DECSC */
	'8',	Table0,	NULL,	 0 ,	1,	(EPTR)LoadCursor,		/* DECRC */
	'=',	Table0,	NULL,	 0 ,	1,	(EPTR)NumericAppMode,		/* DECPAM */
	'>',	Table0,	NULL,	 0 ,	1,	(EPTR)NumericAppMode,		/* DECPNM */
	'N',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* SS2 */
	'O',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* SS3 */
/*	'H',	Table0,	NULL,	 0 ,	1,	(EPTR)SetTab,			// HTS */
	'P',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* DCS */
	'/',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* ST */
	'Z',	Table0,	NULL,	 0 ,	1,	(EPTR)RequestTerminal,		/* DECID */
	'c',	Table0,	NULL,	 0 ,	1,	(EPTR)Reset,			/* RIS */
	'<',	Table0,	NULL,	 0 ,	1,	(EPTR)VT52_EnterANSI,		/* VT52 "Enter ANSI mode" */
	'~',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* LS1R */
	'n',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* LS2 */
	0x7D,	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* LS2R */
	'o',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* LS3 */
	'|',	Table0,	NULL,	 0 ,	1,	(EPTR)Ignore,			/* LS3R */

	/* Double character sequences */

	'[',	Table0,	NULL,	's',	2,	(EPTR)SaveCursor,
	'[',	Table0,	NULL,	'u',	2,	(EPTR)LoadCursor,

	'(',	Table0,	NULL,	'A',	2,	(EPTR)FontStuff,		/* SCS */
	'(',	Table0,	NULL,	'B',	2,	(EPTR)FontStuff,		/* ASCII font */
	'(',	Table0,	NULL,	'0',	2,	(EPTR)FontStuff,		/* DEC special graphics font */
	'(',	Table0,	NULL,	'<',	2,	(EPTR)Ignore,			/* DEC supplemental font */
	'(',	Table0,	NULL,	'4',	2,	(EPTR)Ignore,			/* Dutch font */
	'(',	Table0,	NULL,	'5',	2,	(EPTR)Ignore,			/* Finnish font */
	'(',	Table0,	NULL,	'C',	2,	(EPTR)Ignore,			/* Finnish font */
	'(',	Table0,	NULL,	'R',	2,	(EPTR)Ignore,			/* French font */
	'(',	Table0,	NULL,	'Q',	2,	(EPTR)Ignore,			/* French Canadian font */
	'(',	Table0,	NULL,	'K',	2,	(EPTR)Ignore,			/* German font */
	'(',	Table0,	NULL,	'Y',	2,	(EPTR)Ignore,			/* Italian font */
	'(',	Table0,	NULL,	'6',	2,	(EPTR)Ignore,			/* Norwegian/Danish font */
	'(',	Table0,	NULL,	'E',	2,	(EPTR)Ignore,			/* Norwegian/Danish font */
	'(',	Table0,	NULL,	'Z',	2,	(EPTR)Ignore,			/* Spanish font */
	'(',	Table0,	NULL,	'7',	2,	(EPTR)Ignore,			/* Swedish font */
	'(',	Table0,	NULL,	'H',	2,	(EPTR)Ignore,			/* Swedish font */
	'(',	Table0,	NULL,	'=',	2,	(EPTR)Ignore,			/* Swiss font */

	')',	Table0,	NULL,	'A',	2,	(EPTR)FontStuff,		/* SCS */
	')',	Table0,	NULL,	'B',	2,	(EPTR)FontStuff,		/* ASCII font */
	')',	Table0,	NULL,	'0',	2,	(EPTR)FontStuff,		/* DEC special graphics font */
	')',	Table0,	NULL,	'<',	2,	(EPTR)Ignore,			/* DEC supplemental font */
	')',	Table0,	NULL,	'4',	2,	(EPTR)Ignore,			/* Dutch font */
	')',	Table0,	NULL,	'5',	2,	(EPTR)Ignore,			/* Finnish font */
	')',	Table0,	NULL,	'C',	2,	(EPTR)Ignore,			/* Finnish font */
	')',	Table0,	NULL,	'R',	2,	(EPTR)Ignore,			/* French font */
	')',	Table0,	NULL,	'Q',	2,	(EPTR)Ignore,			/* French Canadian font */
	')',	Table0,	NULL,	'K',	2,	(EPTR)Ignore,			/* German font */
	')',	Table0,	NULL,	'Y',	2,	(EPTR)Ignore,			/* Italian font */
	')',	Table0,	NULL,	'6',	2,	(EPTR)Ignore,			/* Norwegian/Danish font */
	')',	Table0,	NULL,	'E',	2,	(EPTR)Ignore,			/* Norwegian/Danish font */
	')',	Table0,	NULL,	'Z',	2,	(EPTR)Ignore,			/* Spanish font */
	')',	Table0,	NULL,	'7',	2,	(EPTR)Ignore,			/* Swedish font */
	')',	Table0,	NULL,	'H',	2,	(EPTR)Ignore,			/* Swedish font */
	')',	Table0,	NULL,	'=',	2,	(EPTR)Ignore,			/* Swiss font */

	'+',	Table0,	NULL,	'A',	2,	(EPTR)Ignore,			/* British font */
	'+',	Table0,	NULL,	'B',	2,	(EPTR)Ignore,			/* ASCII font */
	'+',	Table0,	NULL,	'0',	2,	(EPTR)Ignore,			/* DEC special graphics font */
	'+',	Table0,	NULL,	'<',	2,	(EPTR)Ignore,			/* DEC supplemental font */
	'+',	Table0,	NULL,	'4',	2,	(EPTR)Ignore,			/* Dutch font */
	'+',	Table0,	NULL,	'5',	2,	(EPTR)Ignore,			/* Finnish font */
	'+',	Table0,	NULL,	'C',	2,	(EPTR)Ignore,			/* Finnish font */
	'+',	Table0,	NULL,	'R',	2,	(EPTR)Ignore,			/* French font */
	'+',	Table0,	NULL,	'Q',	2,	(EPTR)Ignore,			/* French Canadian font */
	'+',	Table0,	NULL,	'K',	2,	(EPTR)Ignore,			/* German font */
	'+',	Table0,	NULL,	'Y',	2,	(EPTR)Ignore,			/* Italian font */
	'+',	Table0,	NULL,	'6',	2,	(EPTR)Ignore,			/* Norwegian/Danish font */
	'+',	Table0,	NULL,	'E',	2,	(EPTR)Ignore,			/* Norwegian/Danish font */
	'+',	Table0,	NULL,	'Z',	2,	(EPTR)Ignore,			/* Spanish font */
	'+',	Table0,	NULL,	'7',	2,	(EPTR)Ignore,			/* Swedish font */
	'+',	Table0,	NULL,	'H',	2,	(EPTR)Ignore,			/* Swedish font */
	'+',	Table0,	NULL,	'=',	2,	(EPTR)Ignore,			/* Swiss font */

	'*',	Table0,	NULL,	'A',	2,	(EPTR)Ignore,			/* British font */
	'*',	Table0,	NULL,	'B',	2,	(EPTR)Ignore,			/* ASCII font */
	'*',	Table0,	NULL,	'0',	2,	(EPTR)Ignore,			/* DEC special graphics font */
	'*',	Table0,	NULL,	'<',	2,	(EPTR)Ignore,			/* DEC supplemental font */
	'*',	Table0,	NULL,	'4',	2,	(EPTR)Ignore,			/* Dutch font */
	'*',	Table0,	NULL,	'5',	2,	(EPTR)Ignore,			/* Finnish font */
	'*',	Table0,	NULL,	'C',	2,	(EPTR)Ignore,			/* Finnish font */
	'*',	Table0,	NULL,	'R',	2,	(EPTR)Ignore,			/* French font */
	'*',	Table0,	NULL,	'Q',	2,	(EPTR)Ignore,			/* French Canadian font */
	'*',	Table0,	NULL,	'K',	2,	(EPTR)Ignore,			/* German font */
	'*',	Table0,	NULL,	'Y',	2,	(EPTR)Ignore,			/* Italian font */
	'*',	Table0,	NULL,	'6',	2,	(EPTR)Ignore,			/* Norwegian/Danish font */
	'*',	Table0,	NULL,	'E',	2,	(EPTR)Ignore,			/* Norwegian/Danish font */
	'*',	Table0,	NULL,	'Z',	2,	(EPTR)Ignore,			/* Spanish font */
	'*',	Table0,	NULL,	'7',	2,	(EPTR)Ignore,			/* Swedish font */
	'*',	Table0,	NULL,	'H',	2,	(EPTR)Ignore,			/* Swedish font */
	'*',	Table0,	NULL,	'=',	2,	(EPTR)Ignore,			/* Swiss font */

	'#',	Table0,	NULL,	'3',	2,	(EPTR)ScaleFont,		/* DECHDL (top half) */
	'#',	Table0,	NULL,	'4',	2,	(EPTR)ScaleFont,		/* DECHDL (bottom half) */
	'#',	Table0,	NULL,	'5',	2,	(EPTR)ScaleFont,		/* DECSWL */
	'#',	Table0,	NULL,	'6',	2,	(EPTR)ScaleFont,		/* DECDWL */
	'#',	Table0,	NULL,	'8',	2,	(EPTR)AlignmentTest,		/* DECALN */
	' ',	Table0,	NULL,	'F',	2,	(EPTR)Ignore,			/* S7C1T */
	' ',	Table0,	NULL,	'G',	2,	(EPTR)Ignore,			/* C8C1T */

	/* Three character sequence */

	'Y',	Table7,	NULL,	 0 ,	3,	(EPTR)VT52_SetCursor,		/* Direct cursor address */

	/* Multiple character sequences */

	'(',	Table6,	Term0,	'D',	0,	(EPTR)Ignore,			/* SCS */

	'[',	Table3,	NULL,	'i',	0,	(EPTR)PrinterController,	/* Print mode */

	'[',	Table3,	NULL,	'n',	0,	(EPTR)RequestInformation,	/* DSR */
	'[',	Table3,	NULL,	'c',	0,	(EPTR)RequestTerminal,		/* DA */
	'[',	Table3,	NULL,	'x',	0,	(EPTR)RequestTerminalParams,	/* DECREQTPARM */
	'[',	Table3,	NULL,	'h',	0,	(EPTR)SetSomething,		/* Terminal modes */
	'[',	Table3,	NULL,	'l',	0,	(EPTR)SetSomething,		/* Terminal modes */

	'[',	Table4,	NULL,	'h',	0,	(EPTR)Ignore,
	'[',	Table5,	NULL,	'p',	0,	(EPTR)Ignore,			/* DECSCL */

	'[',	Table1,	NULL,	'A',	0,	(EPTR)MoveCursor,		/* CUU */
	'[',	Table1,	NULL,	'B',	0,	(EPTR)MoveCursor,		/* CUD */
	'[',	Table1,	NULL,	'C',	0,	(EPTR)MoveCursor,		/* CUF */
	'[',	Table1,	NULL,	'D',	0,	(EPTR)MoveCursor,		/* CUB */
	'[',	Table1,	NULL,	'G',	0,	(EPTR)MoveColumn,
	'[',	Table3,	NULL,	'K',	0,	(EPTR)EraseLine,		/* EL/DECSEL */
	'[',	Table3,	NULL,	'J',	0,	(EPTR)EraseScreen,		/* ED/DECSED */
	'[',	Table1,	NULL,	'P',	0,	(EPTR)EraseCharacters,		/* DCH */
	'[',	Table1,	NULL,	'X',	0,	(EPTR)EraseCharacters,		/* ECH (sort of) */
	'[',	Table1,	NULL,	'@',	0,	(EPTR)InsertCharacters,		/* ICH */
	'[',	Table1,	NULL,	'L',	0,	(EPTR)InsertLine,		/* IL */
	'[',	Table1,	NULL,	'M',	0,	(EPTR)ClearLine,		/* DL */
	'[',	Table1,	NULL,	'g',	0,	(EPTR)SetTabs,			/* TBC */
	'[',	Table5,	NULL,	'q',	0,	(EPTR)Ignore,			/* DECSCA */

	'[',	Table2,	NULL,	'H',	0,	(EPTR)SetAbsolutePosition,	/* CUP */
	'[',	Table2,	NULL,	'f',	0,	(EPTR)SetTopPosition,		/* HVP */
	'[',	Table2,	NULL,	'm',	0,	(EPTR)SetAttributes,		/* SGR */
	'[',	Table2,	NULL,	'y',	0,	(EPTR)Ignore,			/* DECTST */
	'[',	Table2,	NULL,	'r',	0,	(EPTR)SetRegion,		/* DECSTBM */

	'[',	Table1,	NULL,	'S',	0,	(EPTR)ScrollUp,
	'[',	Table1,	NULL,	'T',	0,	(EPTR)ScrollDown,
	'[',	Table1,	NULL,	'E',	0,	(EPTR)MoveCursorDown,
	'[',	Table1,	NULL,	'F',	0,	(EPTR)MoveCursorUp,

	'[',	Table8,	NULL,	'z',	0,	(EPTR)Ignore,			/* DEC private stuff */
	'[',	Table8,	NULL,	'{',	0,	(EPTR)Ignore			/* DEC private stuff */
};

WORD NumCodes = NUM_ELEMENTS(ANSICode);


/**********************************************************************/


UWORD			PenTable[16];
UBYTE			TextAttributeTable[16];
WORD			ForegroundPen,
			BackgroundPen;


/**********************************************************************/


BOOL			ChatMode;
BOOL			UseInternalZModem;
ULONG			SliderType;
ULONG			CycleType;


/**********************************************************************/


struct ColourTable	*NormalColourTable,
			*BlinkColourTable,
			*ANSIColourTable,
			*EGAColourTable,
			*DefaultColourTable,
			*MonoColourTable;


/**********************************************************************/


UBYTE			 Language[80];


/**********************************************************************/


STRPTR CharCodes[256] =
{
	"NUL","SOH","STX","ETX","EOT","ENQ","ACK","BEL","BS" ,"HT" ,"LF" ,"VT" ,"FF" ,"CR" ,"SO" ,"SI" ,
	"DLE","DC1","DC2","DC3","DC4","NAK","SYN","ETB","CAN","EM" ,"SUB","ESC","FS" ,"GS" ,"RS" ,"US" ,
	"SP" , "!" , "\"", "#" , "$" , "%" , "&" , "'" , "(" , ")" , "*" , "+" , "," , "-" , "." , "/" ,
	 "0" , "1" , "2" , "3" , "4" , "5" , "6" , "7" , "8" , "9" , ":" , ";" , "<" , "=" , ">" , "?" ,
	 "@" , "A" , "B" , "C" , "D" , "E" , "F" , "G" , "H" , "I" , "J" , "K" , "L" , "M" , "N" , "O" ,
	 "P" , "Q" , "R" , "S" , "T" , "U" , "V" , "W" , "X" , "Y" , "Z" , "[" , "\\", "]" , "^" , "_" ,
	 "`" , "a" , "b" , "c" , "d" , "e" , "f" , "g" , "h" , "i" , "j" , "k" , "l" , "m" , "n" , "o" ,
	 "p" , "q" , "r" , "s" , "t" , "u" , "v" , "w" , "x" , "y" , "z" , "{" , "|" , "}" , "~" ,"DEL",
	"128","129","130","131","132","133","134","135","136","137","138","139","140","141","SS2","SS3",
	"DCS","145","146","147","148","149","150","151","152","153","154","CSI","ST" ,"OSC","PM" ,"APC",
	"NBS", "¡" , "¢" , "£" , "¤" , "¥" , "¦" , "§" , "¨" , "©" , "ª" , "«" , "¬" ,"SHY", "®" , "¯" ,
	 "°" , "±" , "²" , "³" , "´" , "µ" , "¶" , "·" , "¸" , "¹" , "º" , "»" , "¼" , "½" , "¾" , "¿" ,
	 "À" , "Á" , "Â" , "Ã" , "Ä" , "Å" , "Æ" , "Ç" , "È" , "É" , "Ê" , "Ë" , "Ì" , "Í" , "Î" , "Ï" ,
	 "Ð" , "Ñ" , "Ò" , "Ó" , "Ô" , "Õ" , "Ö" , "×" , "Ø" , "Ù" , "Ú" , "Û" , "Ü" , "Ý" , "Þ" , "ß" ,
	 "à" , "á" , "â" , "ã" , "ä" , "å" , "æ" , "ç" , "è" , "é" , "ê" , "ë" , "ì" , "í" , "î" , "ï" ,
	 "ð" , "ñ" , "ò" , "ó" , "ô" , "õ" , "ö" , "÷" , "ø" , "ù" , "ú" , "û" , "ü" , "ý" , "þ" , "ÿ"
};


/**********************************************************************/


struct Window		*MatrixWindow;


/**********************************************************************/


SAVERASTER		 SaveRaster;


/**********************************************************************/


struct SignalSemaphore	 BufferTaskSemaphore;
struct TextBufferInfo	*BufferInfoData;

struct SignalSemaphore	 ReviewTaskSemaphore;
struct TextBufferInfo	*ReviewInfoData;


/**********************************************************************/


STRPTR BooleanMappings[] =
{
	"OFF",
	"ON",
	NULL
};

STATIC STRPTR TransferMappings1[] =
{
	"XPR",
	"PROGRAM",
	NULL
};

STATIC STRPTR TransferMappings2[] =
{
	"XPR",
	"PROGRAM",
	"DEFAULT",
	NULL
};

STATIC STRPTR TransferMappings3[] =
{
	"XPR",
	"PROGRAM",
	"DEFAULT",
	"INTERNAL",
	NULL
};

STATIC STRPTR DestructiveBSMappings[] =
{
	"OFF",
	"OVERSTRIKE",
	"SHIFT",
	NULL
};

STRPTR ParityMappings[] =
{
	"NONE",
	"EVEN",
	"ODD",
	"MARK",
	"SPACE",
	NULL
};

STRPTR HandshakingMappings[] =
{
	"NONE",
	"RTSCTS",
	"RTSCTSDSR",
	NULL
};

STRPTR DuplexMappings[] =
{
	"FULL",
	"HALF",
	NULL
};

STATIC STRPTR ColourMappings[] =
{
	"FOUR",
	"EIGHT",
	"SIXTEEN",
	"TWO",
	NULL
};

STATIC STRPTR StatusMappings[] =
{
	"DISABLED",
	"STANDARD",
	"COMPRESSED",
	NULL
};

STATIC STRPTR BellMappings[] =
{
	"NONE",
	"VISIBLE",
	"AUDIBLE",
	"BOTH",
	"SYSTEM",
	NULL
};

STATIC STRPTR AlertMappings[] =
{
	"NONE",
	"BELL",
	"SCREEN",
	"BOTH",
	NULL
};

STATIC STRPTR EmulationMappings[] =
{
	"INTERNAL",
	"ATOMIC",
	"TTY",
	"EXTERNAL",
	"HEX",
	NULL
};

STATIC STRPTR FontMappings[] =
{
	"STANDARD",
	"IBM",
	"IBMRAW",
	NULL
};

STATIC STRPTR EOL_Mappings[] =
{
	"IGNORE",
	"CR",
	"LF",
	"CRLF",
	"LFCR",
	NULL
};

STATIC STRPTR IdentifyMappings[] =
{
	"IGNORE",
	"FILETYPE",
	"SOURCE",
	NULL
};

STATIC STRPTR ApplicationMappings[] =
{
	"STANDARD",
	"APPLICATION",
	NULL
};

STATIC STRPTR FontScaleMappings[] =
{
	"NORMAL",
	"HALF",
	NULL
};

STATIC STRPTR ScrollMappings[] =
{
	"JUMP",
	"SMOOTH",
	NULL
};

STATIC STRPTR SexMappings[] =
{
	"MALE",
	"FEMALE",
	NULL
};

STATIC STRPTR OnlineMappings[] =
{
	"TIME",
	"COST",
	"BOTH",
	NULL
};

STATIC STRPTR AutoDateMappings[] =
{
	"NAME",
	"INCLUDE",
	NULL
};

STATIC STRPTR PacingMappings[] =
{
	"DIRECT",
	"ECHO",
	"ANYECHO",
	"PROMPT",
	"DELAY",
	"KEYBOARD",
	NULL
};

STATIC STRPTR OpenBufferMappings[] =
{
	"TOP",
	"END",
	NULL
};

STATIC STRPTR BufferPositionMappings[] =
{
	"LEFT",
	"MID",
	"RIGHT",
	NULL
};

STATIC STRPTR OwnDevUnitRequestMappings[] =
{
	"RELEASE",
	"RELEASERETRY",
	"IGNORE",
	NULL
};

STATIC STRPTR TransferErrorNotifyMappings[] =
{
	"NEVER",
	"ALWAYS",
	"START",
	"END",
	NULL
};

STATIC STRPTR DialModeMappings[] =
{
	"PULSE",
	"TONE",
	"MODEM",
	"ISDN",
	NULL
};

STATIC STRPTR RequesterModeMappings[] =
{
	"CENTRE",
	"PREFS",
	"IGNORE",
	NULL
};

STATIC STRPTR BufferModeMappings[] =
{
	"DATAFLOW",
	"REVIEW",
	NULL
};

STATIC STRPTR IdentificationMappings[] =
{
	"VT200",
	"VT102",
	"VT101",
	"VT100",
	NULL
};

struct AttributeEntry AttributeTable[] =
{
	0,	"TERM",				INFO_STEM,	TRUE,	NULL,			ATTR_APPLICATION,

	1,	 "VERSION",			INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_VERSION,
	1,	 "SCREEN",			INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_SCREEN,

	1,	 "SESSION",			INFO_STEM,	TRUE,	NULL,			ATTR_APPLICATION_SESSION,
	2,	  "ONLINE",			INFO_BOOLEAN,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_ONLINE,
	2,	  "SESSIONSTART",		INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_SESSIONSTART,
	2,	  "BYTESSENT",			INFO_NUMERIC,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_BYTESSENT,
	2,	  "BYTESRECEIVED",		INFO_NUMERIC,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_BYTESRECEIVED,
	2,	  "CONNECTMESSAGE",		INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_CONNECTMESSAGE,
	2,	  "BBSNAME",			INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_BBSNAME,
	2,	  "BBSNUMBER",			INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_BBSNUMBER,
	2,	  "BBSCOMMENT",			INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_BBSCOMMENT,
	2,	  "USERNAME",			INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_USERNAME,
	2,	  "ONLINEMINUTES",		INFO_NUMERIC,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_ONLINEMINUTES,
	2,	  "ONLINECOST",			INFO_NUMERIC,	TRUE,	NULL,			ATTR_APPLICATION_SESSION_ONLINECOST,

	1,	 "AREXX",			INFO_TEXT,	TRUE,	NULL,			ATTR_APPLICATION_AREXX,
	1,	 "LASTERROR",			INFO_NUMERIC,	TRUE,	NULL,			ATTR_APPLICATION_LASTERROR,

	1,	 "TERMINAL",			INFO_STEM,	TRUE,	NULL,			ATTR_APPLICATION_TERMINAL,
	2,	  "ROWS",			INFO_NUMERIC,	TRUE,	NULL,			ATTR_APPLICATION_TERMINAL_ROWS,
	2,	  "COLUMNS",			INFO_NUMERIC,	TRUE,	NULL,			ATTR_APPLICATION_TERMINAL_COLUMNS,

	1,	 "BUFFER",			INFO_STEM,	TRUE,	NULL,			ATTR_APPLICATION_BUFFER,
	2,	  "SIZE",			INFO_NUMERIC,	TRUE,	NULL,			ATTR_APPLICATION_BUFFER_SIZE,

	0,	"SERIALPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_SERIAL,
	1,	 "BAUDRATE",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SERIAL_BAUDRATE,
	1,	 "BREAKLENGTH",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SERIAL_BREAKLENGTH,
	1,	 "BUFFERSIZE",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SERIAL_BUFFERSIZE,
	1,	 "DEVICENAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SERIAL_DEVICENAME,
	1,	 "UNIT",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SERIAL_UNIT,
	1,	 "BITSPERCHAR",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SERIAL_BITSPERCHAR,
	1,	 "PARITYMODE",			INFO_MAPPED,	FALSE,	ParityMappings,		ATTR_PREFS_SERIAL_PARITYMODE,
	1,	 "STOPBITS",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SERIAL_STOPBITS,
	1,	 "HANDSHAKINGMODE",		INFO_MAPPED,	FALSE,	HandshakingMappings,	ATTR_PREFS_SERIAL_HANDSHAKINGMODE,
	1,	 "DUPLEXMODE",			INFO_MAPPED,	FALSE,	DuplexMappings,		ATTR_PREFS_SERIAL_DUPLEXMODE,
	1,	 "INTERNALXONXOFF",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_XONXOFF,
	1,	 "XONXOFF",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_XONXOFF,
	1,	 "HIGHSPEED",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_HIGHSPEED,
	1,	 "SHARED",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_SHARED,
	1,	 "STRIPBIT8",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_STRIPBIT8,
	1,	 "CARRIERCHECK",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_CARRIERCHECK,
	1,	 "PASSXONXOFFTHROUGH",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_PASSXONXOFFTHROUGH,
	1,	 "QUANTUM",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SERIAL_QUANTUM,
	1,	 "USEOWNDEVUNIT",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_USE_OWNDEVUNIT,
	1,	 "OWNDEVUNITREQUESTS",		INFO_MAPPED,	FALSE,	OwnDevUnitRequestMappings,	ATTR_PREFS_SERIAL_OWNDEVUNIT_REQUEST,
	1,	 "DIRECTCONNECTION",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_DIRECT_CONNECTION,
	1,	 "RELEASEWHENONLINE",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_RELEASE_WHEN_ONLINE,
	1,	 "RELEASEWHENDIALING",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_RELEASE_WHEN_DIALING,
	1,	 "NOODUIFSHARED",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_NO_ODU_IF_SHARED,
	1,	 "LOCALECHO",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SERIAL_LOCAL_ECHO,

	0,	"MODEMPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_MODEM,
	1,	 "MODEMINITTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_MODEMINITTEXT,
	1,	 "MODEMEXITTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_MODEMEXITTEXT,
	1,	 "MODEMHANGUPTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_MODEMHANGUPTEXT,
	1,	 "DIALPREFIXTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_DIALPREFIXTEXT,
	1,	 "DIALSUFFIXTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_DIALSUFFIXTEXT,
	1,	 "NOCARRIERTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_NOCARRIERTEXT,
	1,	 "NODIALTONETEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_NODIALTONETEXT,
	1,	 "CONNECTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_CONNECTTEXT,
	1,	 "VOICETEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_VOICETEXT,
	1,	 "RINGTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_RINGTEXT,
	1,	 "BUSYTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_BUSYTEXT,
	1,	 "OKTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_OKTEXT,
	1,	 "ERRORTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_ERRORTEXT,
	1,	 "REDIALDELAY",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MODEM_REDIALDELAY,
	1,	 "DIALRETRIES",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MODEM_DIALRETRIES,
	1,	 "DIALTIMEOUT",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MODEM_DIALTIMEOUT,
	1,	 "CONNECTAUTOBAUD",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MODEM_CONNECTAUTOBAUD,
	1,	 "HANGUPDROPSDTR",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MODEM_HANGUPDROPSDTR,
	1,	 "REDIALAFTERHANGUP",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MODEM_REDIALAFTERHANGUP,
	1,	 "NOCARRIERISBUSY",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MODEM_NOCARRIERISBUSY,
	1,	 "CONNECTLIMIT",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MODEM_CONNECTLIMIT,
	1,	 "CONNECTLIMITMACRO",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_CONNECTLIMITMACRO,
	1,	 "TIMETOCONNECT",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MODEM_TIME_TO_CONNECT,
	1,	 "VERBOSEDIALING",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MODEM_VERBOSEDIALING,
	1,	 "DIALMODE",			INFO_MAPPED,	FALSE,	DialModeMappings,	ATTR_PREFS_MODEM_DIAL_MODE,
	1,	 "INTERDIALDELAY",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MODEM_INTER_DIAL_DELAY,
	1,	 "CHARSENDDELAY",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MODEM_CHAR_SEND_DELAY,
	1,	 "DONOTSENDCOMMANDS",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MODEM_DO_NOT_SEND_COMMANDS,
	1,	 "PBXPREFIXTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MODEM_PBX_PREFIX,
	1,	 "PBX",				INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MODEM_PBX_MODE,

	0,	"COMMANDPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_COMMANDS,
	1,	 "STARTUPMACROTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_COMMANDS_STARTUPMACROTEXT,
	1,	 "LOGINMACROTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_COMMANDS_LOGINMACROTEXT,
	1,	 "LOGOFFMACROTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_COMMANDS_LOGOFFMACROTEXT,
	1,	 "UPLOADMACROTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_COMMANDS_UPLOADMACROTEXT,
	1,	 "DOWNLOADMACROTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_COMMANDS_DOWNLOADMACROTEXT,

	0,	"SCREENPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_SCREEN,
	1,	 "COLOURMODE",			INFO_MAPPED,	FALSE,	ColourMappings,		ATTR_PREFS_SCREEN_COLOURMODE,
	1,	 "FONTNAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SCREEN_FONTNAME,
	1,	 "FONTSIZE",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SCREEN_FONTSIZE,
	1,	 "MAKESCREENPUBLIC",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_MAKESCREENPUBLIC,
	1,	 "SHANGHAIWINDOWS",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_SHANGHAIWINDOWS,
	1,	 "BLINKING",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_BLINKING,
	1,	 "FASTERLAYOUT",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_FASTERLAYOUT,
	1,	 "TITLEBAR",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_TITLEBAR,
	1,	 "STATUSLINEMODE",		INFO_MAPPED,	FALSE,	StatusMappings,		ATTR_PREFS_SCREEN_STATUSLINEMODE,
	1,	 "USEPUBSCREEN",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_USEWORKBENCH,
	1,	 "PUBSCREENNAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SCREEN_PUBSCREENNAME,
	1,	 "ONLINEDISPLAY",		INFO_MAPPED,	FALSE,	OnlineMappings,		ATTR_PREFS_SCREEN_ONLINEDISPLAY,
	1,	 "USEPENS",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_USEPENS,
	1,	 "WINDOWBORDER",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_WINDOW_BORDER,
	1,	 "SPLITSTATUS",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SCREEN_SPLIT_STATUS,

	0,	"TERMINALPREFS",		INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_TERMINAL,
	1,	 "BELLMODE",			INFO_MAPPED,	FALSE,	BellMappings,		ATTR_PREFS_TERMINAL_BELLMODE,
	1,	 "ALERTMODE",			INFO_MAPPED,	FALSE,	AlertMappings,		ATTR_PREFS_MISC_ALERTMODE,
	1,	 "EMULATIONMODE",		INFO_MAPPED,	FALSE,	EmulationMappings,	ATTR_PREFS_TERMINAL_EMULATIONMODE,
	1,	 "FONTMODE",			INFO_MAPPED,	FALSE,	FontMappings,		ATTR_PREFS_TERMINAL_FONTMODE,
	1,	 "SENDCRMODE",			INFO_MAPPED,	FALSE,	EOL_Mappings,		ATTR_PREFS_TERMINAL_SENDCRMODE,
	1,	 "SENDLFMODE",			INFO_MAPPED,	FALSE,	EOL_Mappings,		ATTR_PREFS_TERMINAL_SENDLFMODE,
	1,	 "RECEIVECRMODE",		INFO_MAPPED,	FALSE,	EOL_Mappings,		ATTR_PREFS_TERMINAL_RECEIVECRMODE,
	1,	 "RECEIVELFMODE",		INFO_MAPPED,	FALSE,	EOL_Mappings,		ATTR_PREFS_TERMINAL_RECEIVELFMODE,
	1,	 "NUMCOLUMNS",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_TERMINAL_NUMCOLUMNS,
	1,	 "NUMLINES",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_TERMINAL_NUMLINES,
	1,	 "KEYMAPNAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TERMINAL_KEYMAPNAME,
	1,	 "EMULATIONNAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TERMINAL_EMULATIONNAME,
	1,	 "FONTNAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TERMINAL_FONTNAME,
	1,	 "FONTSIZE",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_TERMINAL_FONTSIZE,
	1,	 "USETERMINALPROCESS",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TERMINAL_USETERMINALPROCESS,
	1,	 "AUTOSIZE",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TERMINAL_AUTOSIZE,

	0,	"PATHPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_PATHS,
	1,	 "ASCIIUPLOADPATH",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_ASCIIUPLOADPATH,
	1,	 "ASCIIDOWNLOADPATH",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_ASCIIDOWNLOADPATH,
	1,	 "TEXTUPLOADPATH",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_TEXTUPLOADPATH,
	1,	 "TEXTDOWNLOADPATH",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_TEXTDOWNLOADPATH,
	1,	 "BINARYUPLOADPATH",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_BINARYUPLOADPATH,
	1,	 "BINARYDOWNLOADPATH",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_BINARYDOWNLOADPATH,
	1,	 "CONFIGPATH",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_CONFIGPATH,
	1,	 "EDITORNAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_EDITORNAME,
	1,	 "HELPFILENAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PATHS_HELPFILENAME,

	0,	"MISCPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_MISC,
	1,	 "PRIORITY",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MISC_PRIORITY,
	1,	 "BACKUPCONFIG",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_BACKUPCONFIG,
	1,	 "OPENFASTMACROPANEL",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_OPENFASTMACROPANEL,
	1,	 "RELEASEDEVICE",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_RELEASEDEVICE,
	1,	 "OVERRIDEPATH",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_OVERRIDEPATH,
	1,	 "AUTOUPLOAD",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_AUTOUPLOAD,
	1,	 "SETARCHIVEDBIT",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_SETARCHIVEDBIT,
	1,	 "COMMENTMODE",			INFO_MAPPED,	FALSE,	IdentifyMappings,	ATTR_PREFS_MISC_COMMENTMODE,
	1,	 "TRANSFERICONS",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_TRANSFERICONS,
	1,	 "CREATEICONS",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_CREATEICONS,
	1,	 "SIMPLEIO",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_SIMPLEIO,
	1,	 "HIDEUPLOADICON",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_HIDE_UPLOAD_ICON,
	1,	 "TRANSFERPERFMETER",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_TRANSFER_PERFMETER,
	1,	 "IOBUFFERSIZE",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MISC_IOBUFFERSIZE,
	1,	 "PROTECTIVEMODE",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_OVERWRITE_WARNING,
	1,	 "ALERTMODE",			INFO_MAPPED,	FALSE,	AlertMappings,		ATTR_PREFS_MISC_ALERTMODE,
	1,	 "REQUESTERMODE",		INFO_MAPPED,	FALSE,	RequesterModeMappings,	ATTR_PREFS_MISC_REQUESTERMODE,
	1,	 "REQUESTERWIDTH",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MISC_REQUESTERWIDTH,
	1,	 "REQUESTERHEIGHT",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_MISC_REQUESTERHEIGHT,
	1,	 "WAITSTRING",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MISC_WAITSTRING,
	1,	 "WAITCOMMAND",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MISC_WAITSTRING,
	1,	 "CONSOLEWINDOW",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_MISC_CONSOLEWINDOW,
	1,	 "SUPPRESSOUTPUT",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_MISC_SUPPRESSOUTPUT,

	0,	"CLIPBOARDPREFS",		INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD,
	1,	 "UNIT",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD_UNIT,
	1,	 "LINEDELAY",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD_LINEDELAY,
	1,	 "CHARDELAY",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD_CHARDELAY,
	1,	 "INSERTPREFIXTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD_INSERTPREFIXTEXT,
	1,	 "INSERTSUFFIXTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD_INSERTSUFFIXTEXT,
	1,	 "LINEPROMPTTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD_LINEPROMPT,
	1,	 "TEXTPACING",			INFO_MAPPED,	FALSE,	PacingMappings,		ATTR_PREFS_CLIPBOARD_TEXTPACING,
	1,	 "SENDTIMEOUT",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD_SENDTIMEOUT,
	1,	 "CONVERTLF",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CLIPBOARD_CONVERTLF,

	0,	"CAPTUREPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_CAPTURE,
	1,	 "LOGACTIONS",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_LOGACTIONS,
	1,	 "LOGCALLS",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_LOGCALLS,
	1,	 "LOGFILENAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_LOGFILENAME,
	1,	 "MAXBUFFERSIZE",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_MAXBUFFERSIZE,
	1,	 "BUFFER",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_BUFFER,
	1,	 "CONNECTAUTOCAPTURE",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_CONNECTAUTOCAPTURE,
	1,	 "AUTOCAPTUREDATE",		INFO_MAPPED,	FALSE,	AutoDateMappings,	ATTR_PREFS_CAPTURE_AUTOCAPTUREDATE,
	1,	 "CAPTUREFILTER",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_CAPTUREFILTER,
	1,	 "CAPTUREPATH",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_CAPTUREPATH,
	1,	 "CALLLOGFILENAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_CALLLOGFILENAME,
	1,	 "BUFFERSAVEPATH",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_BUFFERSAVEPATH,
	1,	 "OPENBUFFERWINDOW",		INFO_MAPPED,	FALSE,	OpenBufferMappings,	ATTR_PREFS_CAPTURE_OPENBUFFERWINDOW,
	1,	 "OPENBUFFERSCREEN",		INFO_MAPPED,	FALSE,	OpenBufferMappings,	ATTR_PREFS_CAPTURE_OPENBUFFERSCREEN,
	1,	 "BUFFERSCREENPOSITION",	INFO_MAPPED,	FALSE,	BufferPositionMappings,	ATTR_PREFS_CAPTURE_BUFFERSCREENPOSITION,
	1,	 "BUFFERWIDTH",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_BUFFERWIDTH,
	1,	 "REMEMBERBUFFERWINDOW",	INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_REMEMBER_BUFFERWINDOW,
	1,	 "REMEMBERBUFFERSCREEN",	INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_REMEMBER_BUFFERSCREEN,
	1,	 "SEARCHHISTORY",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_SEARCH_HISTORY,
	1,	 "CONVERTCHARACTERS",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_CONVERTCHARACTERS,
	1,	 "BUFFERMODE",			INFO_MAPPED,	FALSE,	BufferModeMappings,	ATTR_PREFS_CAPTURE_BUFFERMODE,
	1,	 "BUFFERSAFETYMEMORY",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_CAPTURE_BUFFERSAFETYMEMORY,

	0,	"FILEPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_FILE,
	1,	 "TRANSFERPROTOCOLNAME",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FILE_TRANSFERPROTOCOLNAME,
	1,	 "TRANSLATIONFILENAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FILE_TRANSLATIONFILENAME,
	1,	 "MACROFILENAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FILE_MACROFILENAME,
	1,	 "CURSORFILENAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FILE_CURSORFILENAME,
	1,	 "FASTMACROFILENAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FILE_FASTMACROFILENAME,

	0,	"EMULATIONPREFS",		INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_EMULATION,
	1,	 "IDENTIFICATION",		INFO_MAPPED,	FALSE,	IdentificationMappings,	ATTR_PREFS_EMULATION_IDENTIFICATION,
	1,	 "CURSORMODE",			INFO_MAPPED,	FALSE,	ApplicationMappings,	ATTR_PREFS_EMULATION_CURSORMODE,
	1,	 "NUMERICMODE",			INFO_MAPPED,	FALSE,	ApplicationMappings,	ATTR_PREFS_EMULATION_NUMERICMODE,
	1,	 "CURSORWRAP",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_CURSORWRAP,
	1,	 "LINEWRAP",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_LINEWRAP,
	1,	 "INSERTMODE",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_INSERTMODE,
	1,	 "NEWLINEMODE",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_NEWLINEMODE,
	1,	 "FONTSCALEMODE",		INFO_MAPPED,	FALSE,	FontScaleMappings,	ATTR_PREFS_EMULATION_FONTSCALEMODE,
	1,	 "SCROLLMODE",			INFO_MAPPED,	FALSE,	ScrollMappings,		ATTR_PREFS_EMULATION_SCROLLMODE,
	1,	 "DESTRUCTIVEBACKSPACE",	INFO_MAPPED,	FALSE,	DestructiveBSMappings,	ATTR_PREFS_EMULATION_DESTRUCTIVEBACKSPACE,
	1,	 "SWAPBSDELETE",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_SWAPBSDELETE,
	1,	 "PRINTERENABLED",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_PRINTERENABLED,
	1,	 "ANSWERBACKTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_EMULATION_ANSWERBACKTEXT,
	1,	 "CLSRESETSCURSOR",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_CLS_RESETS_CURSOR,
	1,	 "CURSORLOCKED",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_CURSORLOCKED,
	1,	 "FONTLOCKED",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_FONTLOCKED,
	1,	 "NUMPADLOCKED",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_NUMPADLOCKED,
	1,	 "MAXPRESCROLL",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_EMULATION_MAXSCROLL,
	1,	 "MAXJUMP",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_EMULATION_MAXJUMP,
	1,	 "WRAPLOCKED",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_WRAPLOCKED,
	1,	 "STYLELOCKED",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_STYLELOCKED,
	1,	 "COLOURLOCKED",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_COLOURLOCKED,
	1,	 "USEPENS",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_EMULATION_USEPENS,

	0,	"TRANSFERPREFS",		INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_TRANSFER,
	1,	 "QUIETTRANSFER",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_QUIET_TRANSFER,
	1,	 "MANGLEFILENAMES",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_MANGLE_FILE_NAMES,
	1,	 "LINEDELAY",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_LINEDELAY,
	1,	 "CHARDELAY",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_CHARDELAY,
	1,	 "LINEPROMPTTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_LINEPROMPT,
	1,	 "TEXTPACING",			INFO_MAPPED,	FALSE,	PacingMappings,		ATTR_PREFS_TRANSFER_TEXTPACING,
	1,	 "SENDTIMEOUT",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_SENDTIMEOUT,
	1,	 "STRIPBIT8",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_STRIP_BIT_8,
	1,	 "IGNOREDATAPASTTERMINATOR",	INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_IGNORE_DATA_PAST_ARNOLD,
	1,	 "TERMINATORCHAR",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TERMINATOR_CHAR,
	1,	 "ERRORNOTIFYCOUNT",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_ERROR_NOTIFY_COUNT,
	1,	 "ERRORNOTIFYWHEN",		INFO_MAPPED,	FALSE,	TransferErrorNotifyMappings,	ATTR_PREFS_TRANSFER_ERROR_NOTIFY_WHEN,
	1,	 "OVERRIDEPATH",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_OVERRIDEPATH,
	1,	 "SETARCHIVEDBIT",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_SETARCHIVEDBIT,
	1,	 "COMMENTMODE",			INFO_MAPPED,	FALSE,	IdentifyMappings,	ATTR_PREFS_TRANSFER_COMMENTMODE,
	1,	 "TRANSFERICONS",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TRANSFERICONS,
	1,	 "HIDEUPLOADICON",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_HIDE_UPLOAD_ICON,
	1,	 "TRANSFERPERFMETER",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TRANSFER_PERFMETER,
	1,	 "DEFAULTLIBRARY",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_DEFAULT_LIBRARY,
	1,	 "DEFAULTPROTOCOL",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_DEFAULT_LIBRARY,
	1,	 "ASCIIUPLOADLIBRARY",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_ASCII_UPLOAD_LIBRARY,
	1,	 "ASCIIUPLOADPROTOCOL",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_ASCII_UPLOAD_LIBRARY,
	1,	 "ASCIIDOWNLOADLIBRARY",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_LIBRARY,
	1,	 "ASCIIDOWNLOADPROTOCOL",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_LIBRARY,
	1,	 "TEXTUPLOADLIBRARY",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TEXT_UPLOAD_LIBRARY,
	1,	 "TEXTUPLOADPROTOCOL",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TEXT_UPLOAD_LIBRARY,
	1,	 "TEXTDOWNLOADLIBRARY",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_LIBRARY,
	1,	 "TEXTDOWNLOADPROTOCOL",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_LIBRARY,
	1,	 "BINARYUPLOADLIBRARY",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_BINARY_UPLOAD_LIBRARY,
	1,	 "BINARYUPLOADPROTOCOL",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_BINARY_UPLOAD_LIBRARY,
	1,	 "BINARYDOWNLOADLIBRARY",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_LIBRARY,
	1,	 "BINARYDOWNLOADPROTOCOL",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_LIBRARY,
	1,	 "DEFAULTTYPE",			INFO_MAPPED,	FALSE,	TransferMappings1,	ATTR_PREFS_TRANSFER_DEFAULT_TYPE,
	1,	 "DEFAULTSENDSIGNATURE",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_DEFAULT_SEND_SIGNATURE,
	1,	 "DEFAULTRECEIVESIGNATURE",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_DEFAULT_RECEIVE_SIGNATURE,
	1,	 "ASCIIUPLOADTYPE",		INFO_MAPPED,	FALSE,	TransferMappings3,	ATTR_PREFS_TRANSFER_ASCII_UPLOAD_TYPE,
	1,	 "ASCIIUPLOADSIGNATURE",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_ASCII_UPLOAD_SIGNATURE,
	1,	 "ASCIIDOWNLOADTYPE",		INFO_MAPPED,	FALSE,	TransferMappings3,	ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_TYPE,
	1,	 "ASCIIDOWNLOADSIGNATURE",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_SIGNATURE,
	1,	 "TEXTUPLOADTYPE",		INFO_MAPPED,	FALSE,	TransferMappings2,	ATTR_PREFS_TRANSFER_TEXT_UPLOAD_TYPE,
	1,	 "TEXTUPLOADSIGNATURE",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TEXT_UPLOAD_SIGNATURE,
	1,	 "TEXTDOWNLOADTYPE",		INFO_MAPPED,	FALSE,	TransferMappings2,	ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_TYPE,
	1,	 "TEXTDOWNLOADSIGNATURE",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_SIGNATURE,
	1,	 "BINARYUPLOADTYPE",		INFO_MAPPED,	FALSE,	TransferMappings2,	ATTR_PREFS_TRANSFER_BINARY_UPLOAD_TYPE,
	1,	 "BINARYUPLOADSIGNATURE",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_BINARY_UPLOAD_SIGNATURE,
	1,	 "BINARYDOWNLOADTYPE",		INFO_MAPPED,	FALSE,	TransferMappings2,	ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_TYPE,
	1,	 "BINARYDOWNLOADSIGNATURE",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_SIGNATURE,
	1,	 "IDENTIFYCOMMAND",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_IDENTIFY_COMMAND,
	1,	 "EXPANDBLANKLINES",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_TRANSFER_EXPAND_BLANK_LINES,

	0,	"SPEECHPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_SPEECH,
	1,	 "RATE",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SPEECH_RATE,
	1,	 "PITCH",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SPEECH_PITCH,
	1,	 "FREQUENCY",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SPEECH_FREQUENCY,
	1,	 "SEXMODE",			INFO_MAPPED,	FALSE,	SexMappings,		ATTR_PREFS_SPEECH_SEXMODE,
	1,	 "VOLUME",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SPEECH_VOLUME,
	1,	 "SPEECH",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SPEECH_SPEECH,

	0,	"TRANSLATIONPREFS",		INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_TRANSLATIONS,
	1,	 NULL,				INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_TRANSLATIONS_X,
	2,	 "SEND",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSLATIONS_X_SEND,
	2,	 "RECEIVE",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_TRANSLATIONS_X_RECEIVE,

	0,	"FASTMACROPREFS",		INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_FASTMACROS,
	1,	 "COUNT",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_FASTMACROS_COUNT,
	1,	  NULL,				INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_FASTMACROS_X,
	2,	  "NAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FASTMACROS_X_NAME,
	2,	  "CODE",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FASTMACROS_X_CODE,

	0,	"HOTKEYPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_HOTKEYS,
	1,	 "TERMSCREENTOFRONTTEXT",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_HOTKEYS_TERMSCREENTOFRONTTEXT,
	1,	 "BUFFERSCREENTOFRONTTEXT",	INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_HOTKEYS_BUFFERSCREENTOFRONTTEXT,
	1,	 "SKIPDIALENTRYTEXT",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_HOTKEYS_SKIPDIALENTRYTEXT,
	1,	 "ABORTAREXX",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_HOTKEYS_ABORTAREXX,
	1,	 "COMMODITYPRIORITY",		INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_HOTKEYS_COMMODITYPRIORITY,
	1,	 "HOTKEYSENABLED",		INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_HOTKEYS_HOTKEYSENABLED,

	0,	"CURSORKEYPREFS",		INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS,
	1,	 "UPTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_UPTEXT,
	1,	 "RIGHTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_RIGHTTEXT,
	1,	 "DOWNTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_DOWNTEXT,
	1,	 "LEFTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_LEFTTEXT,

	1,	 "SHIFT",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_SHIFT,
	2,	  "UPTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_SHIFT_UPTEXT,
	2,	  "RIGHTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_SHIFT_RIGHTTEXT,
	2,	  "DOWNTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_SHIFT_DOWNTEXT,
	2,	  "LEFTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_SHIFT_LEFTTEXT,

	1,	 "ALT",				INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_ALT,
	2,	  "UPTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_ALT_UPTEXT,
	2,	  "RIGHTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_ALT_RIGHTTEXT,
	2,	  "DOWNTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_ALT_DOWNTEXT,
	2,	  "LEFTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_ALT_LEFTTEXT,

	1,	 "CONTROL",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_CONTROL,
	2,	  "UPTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_CONTROL_UPTEXT,
	2,	  "RIGHTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_CONTROL_RIGHTTEXT,
	2,	  "DOWNTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_CONTROL_DOWNTEXT,
	2,	  "LEFTTEXT",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CURSORKEYS_CONTROL_LEFTTEXT,

	0,	"FUNCTIONKEYPREFS",		INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_FUNCTIONKEYS,
	1,	  NULL,				INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FUNCTIONKEYS_X,
	1,	 "SHIFT",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_FUNCTIONKEYS_SHIFT,
	2,	   NULL,			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FUNCTIONKEYS_SHIFT_X,
	1,	 "ALT",				INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_FUNCTIONKEYS_ALT,
	2,	   NULL,			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FUNCTIONKEYS_ALT_X,
	1,	 "CONTROL",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_FUNCTIONKEYS_CONTROL,
	2,	   NULL,			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_FUNCTIONKEYS_CONTROL_X,

	0,	"PROTOCOLPREFS",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_PROTOCOL,

	0,	"CONSOLEPREFS",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_CONSOLE,

	0,	"SOUNDPREFS",			INFO_STEM,	FALSE,	NULL,			ATTR_PREFS_SOUND,
	1,	 "BELLNAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SOUND_BELLNAME,
	1,	 "CONNECTNAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SOUND_CONNECTNAME,
	1,	 "DISCONNECTNAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SOUND_DISCONNECTNAME,
	1,	 "GOODTRANSFERNAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SOUND_GOODTRANSFERNAME,
	1,	 "BADTRANSFERNAME",		INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SOUND_BADTRANSFERNAME,
	1,	 "RINGNAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SOUND_RINGNAME,
	1,	 "VOICENAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SOUND_VOICENAME,
	1,	 "ERRORNAME",			INFO_TEXT,	FALSE,	NULL,			ATTR_PREFS_SOUND_ERROR,
	1,	 "PRELOAD",			INFO_BOOLEAN,	FALSE,	NULL,			ATTR_PREFS_SOUND_PRELOAD,
	1,	 "VOLUME",			INFO_NUMERIC,	FALSE,	NULL,			ATTR_PREFS_SOUND_VOLUME,

	0,	"PHONEBOOK",			INFO_STEM,	TRUE,	NULL,			ATTR_PHONEBOOK,

	1,	 "COUNT",			INFO_NUMERIC,	TRUE,	NULL,			ATTR_PHONEBOOK_COUNT,
	1,	  NULL,				INFO_STEM,	TRUE,	NULL,			ATTR_PHONEBOOK_X,
	2,	  "NAME",			INFO_TEXT,	TRUE,	NULL,			ATTR_PHONEBOOK_X_NAME,
	2,	  "NUMBER",			INFO_TEXT,	TRUE,	NULL,			ATTR_PHONEBOOK_X_NUMBER,
	2,	  "COMMENTTEXT",		INFO_TEXT,	TRUE,	NULL,			ATTR_PHONEBOOK_X_COMMENTTEXT,
	2,	  "USERNAME",			INFO_TEXT,	TRUE,	NULL,			ATTR_PHONEBOOK_X_USERNAME,
	2,	  "PASSWORDTEXT",		INFO_TEXT,	TRUE,	NULL,			ATTR_PHONEBOOK_X_PASSWORDTEXT
};

WORD AttributeTableSize = NUM_ELEMENTS(AttributeTable);

	/* Information on arguments to exclude each other.*/

STATIC struct ExclusionInfo ExclusionInfoTable_00[] =
{
	1,2, 5,6, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_01[] =
{
	0,1, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_02[] =
{
	2,3, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_03[] =
{
	0,1, 0,2, 0,3, 0,4, 1,2, 1,3, 1,4, 2,3, 2,4, 3,4, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_04[] =
{
	0,1, 0,2, 1,2, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_05[] =
{
	0,2, 0,3, 0,4, 0,5, 2,3, 2,4, 2,5, 3,4, 3,5, 4,5, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_06[] =
{
	2,3, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_07[] =
{
	0,1, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_08[] =
{
	0,1, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_09[] =
{
	0,1, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_10[] =
{
	1,2, 2,3, 2,4, 2,5, 2,6, 2,7, 2,8, 2,9, 2,10, 2,11, 4,5, 6,7, 8,9, 8,10, 8,11, 9,10, 9,11, 10,11, -1,-1
};

STATIC struct ExclusionInfo ExclusionInfoTable_11[] =
{
	2,3, 2,4, 3,4, -1,-1
};

	/* This is where the information on what arguments exclude
	 * other arguments is linked to the actual commands.
	 */

struct ExclusionInfo *ExclusionTable[] =
{
	/* ACTIVATE */		NULL,
	/* ADD */		ExclusionInfoTable_00,
	/* BAUD */		NULL,
	/* BEEPSCREEN */	NULL,
	/* CALLMENU */		NULL,
	/* CAPTURE */		ExclusionInfoTable_11,
	/* CLEAR */		NULL,
	/* CLEARSCREEN */	NULL,
	/* CLOSE */		NULL,
	/* CLOSEDEVICE */	NULL,
	/* CLOSEREQUESTER */	NULL,
	/* DEACTIVATE */	NULL,
	/* DELAY */		NULL,
	/* DIAL */		NULL,
	/* DUPLEX */		ExclusionInfoTable_01,
	/* EXECTOOL */		NULL,
	/* FAULT */		NULL,
	/* GETATTR */		ExclusionInfoTable_02,
	/* GETCLIP */		NULL,
	/* GOONLINE */		NULL,
	/* HANGUP */		NULL,
	/* HELP */		NULL,
	/* OPEN */		NULL,
	/* OPENDEVICE */	NULL,
	/* OPENREQUESTER */	NULL,
	/* PARITY */		ExclusionInfoTable_03,
	/* PASTECLIP */		NULL,
	/* PRINT */		NULL,
	/* PROTOCOL */		ExclusionInfoTable_04,
	/* PROCESSIO */		ExclusionInfoTable_09,
	/* PUTCLIP */		NULL,
	/* QUIT */		NULL,
	/* READ */		NULL,
	/* RECEIVEFILE */	NULL,
	/* REDIAL */		NULL,
	/* REMOVE */		NULL,
	/* REQUESTFILE */	NULL,
	/* REQUESTNOTIFY */	NULL,
	/* REQUESTNUMBER */	NULL,
	/* REQUESTRESPONSE */	NULL,
	/* REQUESTSTRING */	NULL,
	/* RESETSCREEN */	NULL,
	/* RESETSTYLES */	NULL,
	/* RESETTEXT */		NULL,
	/* RESETTIMER */	NULL,
	/* RX */		NULL,
	/* SAVE */		NULL,
	/* SAVEAS */		NULL,
	/* SELECTITEM */	ExclusionInfoTable_05,
	/* SEND */		NULL,
	/* SENDBREAK */		NULL,
	/* SENDFILE */		NULL,
	/* SETATTR */		ExclusionInfoTable_06,
	/* SPEAK */		NULL,
	/* STOPBITS */		ExclusionInfoTable_07,
	/* TEXTBUFFER */	ExclusionInfoTable_08,
	/* TIMEOUT */		ExclusionInfoTable_09,
	/* TRAP */		ExclusionInfoTable_09,
	/* WAIT */		NULL,
	/* WINDOW */		ExclusionInfoTable_10,
};

	/* This is where the information on which arguments (switches and keywords)
	 * are actually required is stored.
	 */

UWORD InclusionTable[] =
{
	/* ACTIVATE */		NULL,
	/* ADD */		INCLUDE_05|INCLUDE_06,
	/* BAUD */		NULL,
	/* BEEPSCREEN */	NULL,
	/* CALLMENU */		NULL,
	/* CAPTURE */		NULL,
	/* CLEAR */		NULL,
	/* CLEARSCREEN */	NULL,
	/* CLOSE */		NULL,
	/* CLOSEDEVICE */	NULL,
	/* CLOSEREQUESTER */	NULL,
	/* DEACTIVATE */	NULL,
	/* DELAY */		INCLUDE_00|INCLUDE_01|INCLUDE_02,
	/* DIAL */		NULL,
	/* DUPLEX */		INCLUDE_00|INCLUDE_01,
	/* EXECTOOL */		NULL,
	/* FAULT */		NULL,
	/* GETATTR */		NULL,
	/* GETCLIP */		NULL,
	/* GOONLINE */		NULL,
	/* HANGUP */		NULL,
	/* HELP */		INCLUDE_00|INCLUDE_01,
	/* OPEN */		NULL,
	/* OPENDEVICE */	NULL,
	/* OPENREQUESTER */	NULL,
	/* PARITY */		INCLUDE_00|INCLUDE_01|INCLUDE_02|INCLUDE_03|INCLUDE_04,
	/* PASTECLIP */		NULL,
	/* PRINT */		NULL,
	/* PROTOCOL */		INCLUDE_00|INCLUDE_01|INCLUDE_02,
	/* PROCESSIO */		INCLUDE_00|INCLUDE_01,
	/* PUTCLIP */		NULL,
	/* QUIT */		NULL,
	/* READ */		INCLUDE_00|INCLUDE_01,
	/* RECEIVEFILE */	NULL,
	/* REDIAL */		NULL,
	/* REMOVE */		NULL,
	/* REQUESTFILE */	NULL,
	/* REQUESTNOTIFY */	NULL,
	/* REQUESTNUMBER */	NULL,
	/* REQUESTRESPONSE */	NULL,
	/* REQUESTSTRING */	NULL,
	/* RESETSCREEN */	NULL,
	/* RESETSTYLES */	NULL,
	/* RESETTEXT */		NULL,
	/* RESETTIMER */	NULL,
	/* RX */		NULL,
	/* SAVE */		NULL,
	/* SAVEAS */		INCLUDE_00|INCLUDE_02|INCLUDE_03|INCLUDE_04|INCLUDE_05,
	/* SELECTITEM */	NULL,
	/* SEND */		INCLUDE_03|INCLUDE_04,
	/* SENDBREAK */		NULL,
	/* SENDFILE */		NULL,
	/* SETATTR */		INCLUDE_02|INCLUDE_03,
	/* SPEAK */		NULL,
	/* STOPBITS */		INCLUDE_00|INCLUDE_01,
	/* TEXTBUFFER */	INCLUDE_00|INCLUDE_01,
	/* TIMEOUT */		INCLUDE_00|INCLUDE_01,
	/* TRAP */		INCLUDE_00|INCLUDE_01,
	/* WAIT */		NULL,
	/* WINDOW */		INCLUDE_01|INCLUDE_02|INCLUDE_03|INCLUDE_04|INCLUDE_05|INCLUDE_06|INCLUDE_07|INCLUDE_08|INCLUDE_09|INCLUDE_10|INCLUDE_11,
};

	/* This is where command names, arguments and the routines
	 * to execute the actual commands are linked together.
	 */

struct CommandInfo CommandTable[] =
{
	TRUE,	FALSE,	RexxActivate,		"ACTIVATE",		 NULL,
	TRUE,	FALSE,	RexxAdd,		"ADDITEM",		"TO/A,BEFORE/S,AFTER/S,RESPONSE/K,COMMAND/K,PHONE/K/F,NAME/K/F",
	FALSE,	FALSE,	RexxBaud,		"BAUD",			"RATE/A/N",
	FALSE,	FALSE,	RexxBeepScreen,		"BEEPSCREEN",		 NULL,
	FALSE,	FALSE,	RexxCallMenu,		"CALLMENU",		"TITLE/A/F",
	FALSE,	FALSE,	RexxCapture,		"CAPTURE",		"TO/A,NAME/K,OVERWRITE/S,APPEND/S,SKIP/S",
	FALSE,	FALSE,	RexxClear,		"CLEAR",		"FROM/A,FORCE/S",
	FALSE,	FALSE,	RexxClearScreen,	"CLEARSCREEN",		 NULL,
	FALSE,	FALSE,	RexxClose,		"CLOSE",		"FROM/A",
	FALSE,	FALSE,	RexxCloseDevice,	"CLOSEDEVICE",		 NULL,
	TRUE,	FALSE,	RexxCloseRequester,	"CLOSEREQUESTER",	 NULL,
	TRUE,	FALSE,	RexxDeactivate,		"DEACTIVATE",		 NULL,
	FALSE,	FALSE,	RexxDelay,		"DELAY",		"MIC=MICROSECONDS/K/N,SEC=SECONDS/N,MIN=MINUTES/K/N,QUIET/S",
	FALSE,	FALSE,	RexxDial,		"DIAL",			"WAIT=SYNC/S,NUM/F",
	FALSE,	FALSE,	RexxDuplex,		"DUPLEX",		"FULL/S,HALF=ECHO/S",
	TRUE,	TRUE,	RexxExecTool,		"EXECTOOL",		"CONSOLE/S,ASYNC/S,PORT/S,COMMAND/A/F",
	TRUE,	FALSE,	RexxFault,		"FAULT",		"CODE/A/N",
	TRUE,	FALSE,	RexxGetAttr,		"GETATTR",		"OBJECT/A,FIELD,STEM/K,VAR/K",
	FALSE,	FALSE,	RexxGetClip,		"GETCLIP",		"UNIT/K/N",
	FALSE,	FALSE,	RexxGoOnline,		"GOONLINE",		 NULL,
	FALSE,	FALSE,	RexxHangup,		"HANGUP",		 NULL,
	TRUE,	FALSE,	RexxHelp,		"HELP",			"COMMAND,PROMPT/S",
	FALSE,	FALSE,	RexxOpen,		"OPEN",			"NAME/K,TO/A",
	FALSE,	FALSE,	RexxOpenDevice,		"OPENDEVICE",		"NAME/K,UNIT/K/N",
	FALSE,	FALSE,	RexxOpenRequester,	"OPENREQUESTER",	"REQUESTER/A",
	FALSE,	FALSE,	RexxParity,		"PARITY",		"EVEN/S,ODD/S,NONE/S,MARK/S,SPACE/S",
	TRUE,	FALSE,	RexxPasteClip,		"PASTECLIP",		"UNIT/K/N",
	FALSE,	FALSE,	RexxPrint,		"PRINT",		"FROM/A,TO/K,SERIAL/S,MODEM/S,SCREEN/S,TERMINAL/S,USER/S,COMMENT/S,SIZE/S,DATE/S,ATTR/S",
	FALSE,	FALSE,	RexxProtocol,		"PROTOCOL",		"NONE/S,RTSCTS/S,RTSCTSDTR/S",
	FALSE,	FALSE,	RexxProcessIO,		"PROCESSIO",		"ON/S,OFF/S",
	FALSE,	FALSE,	RexxPutClip,		"PUTCLIP",		"UNIT/K/N,TEXT/A/F",
	TRUE,	FALSE,	RexxQuit,		"QUIT",			"FORCE/S",
	FALSE,	FALSE,	RexxRead,		"READ",			"NUM/K/N,CR/S,NOECHO/S,VERBATIM/S,TIMEOUT/K/N,TERMINATOR/K,PROMPT/K/F",
	FALSE,	FALSE,	RexxReceiveFile,	"RECEIVEFILE",		"MODE/K,NAME/K",
	FALSE,	FALSE,	RexxRedial,		"REDIAL",		 NULL,
	TRUE,	FALSE,	RexxRemove,		"REMITEM",		"FROM/A,NAME/K/F",
	FALSE,	FALSE,	RexxRequestFile,	"REQUESTFILE",		"TITLE/K,PATH/K,FILE/K,PATTERN/K,MULTI/S,STEM=NAME/K",
	FALSE,	FALSE,	RexxRequestNotify,	"REQUESTNOTIFY",	"TITLE/K,PROMPT/A/F",
	FALSE,	FALSE,	RexxRequestNumber,	"REQUESTNUMBER",	"DEFAULT/K/N,PROMPT/K/F",
	FALSE,	FALSE,	RexxRequestResponse,	"REQUESTRESPONSE",	"TITLE/K,OPTIONS/K,PROMPT/A/F",
	FALSE,	FALSE,	RexxRequestString,	"REQUESTSTRING",	"SECRET/S,DEFAULT/K,PROMPT/K/F",
	FALSE,	FALSE,	RexxResetScreen,	"RESETSCREEN",		 NULL,
	FALSE,	FALSE,	RexxReset,		"RESET",		"CLEAR/S,STYLES/S,TEXT/S,TIMER/S",
	FALSE,	FALSE,	RexxResetStyles,	"RESETSTYLES",		 NULL,
	FALSE,	FALSE,	RexxResetText,		"RESETTEXT",		 NULL,
	FALSE,	FALSE,	RexxResetTimer,		"RESETTIMER",		 NULL,
	TRUE,	TRUE,	RexxRX,			"RX",			"CONSOLE/S,ASYNC/S,COMMAND/A/F",
	FALSE,	FALSE,	RexxSave,		"SAVE",			"FROM/A",
	FALSE,	FALSE,	RexxSaveAs,		"SAVEAS",		"NAME/K,FROM/A",
	TRUE,	FALSE,	RexxSelect,		"SELECTITEM",		"NAME/K,FROM/A,NEXT/S,PREV=PREVIOUS/S,TOP/S,BOTTOM/S",
	FALSE,	FALSE,	RexxSend,		"SEND",			"NOECHO/S,LOCAL/S,LITERAL/S,BYTE/K/N,TEXT/F",
	FALSE,	FALSE,	RexxSendBreak,		"SENDBREAK",		 NULL,
	FALSE,	FALSE,	RexxSendFile,		"SENDFILE",		"MODE/K,NAMES/M",
	FALSE,	FALSE,	RexxSetAttr,		"SETATTR",		"OBJECT/A,FIELD,STEM/K,VAR",
	TRUE,	FALSE,	RexxSpeak,		"SPEAK",		"TEXT/A/F",
	FALSE,	FALSE,	RexxStopBits,		"STOPBITS",		"0/S,1/S",
	FALSE,	FALSE,	RexxTextBuffer,		"TEXTBUFFER",		"LOCK/S,UNLOCK/S",
	FALSE,	FALSE,	RexxTimeout,		"TIMEOUT",		"SEC=SECONDS/N,OFF/S",
	FALSE,	FALSE,	RexxTrap,		"TRAP",			"ON/S,OFF/S",
	FALSE,	FALSE,	RexxWait,		"WAIT",			"NOECHO/S,TIMEOUT/K/N,TEXT/F",
	FALSE,	FALSE,	RexxWindow,		"WINDOW",		"NAMES/A/M,OPEN/S,CLOSE/S,ACTIVATE/S,MIN/S,MAX/S,FRONT/S,BACK/S,TOP/S,BOTTOM/S,UP/S,DOWN/S",
};

WORD CommandTableSize = NUM_ELEMENTS(CommandTable);

struct MsgPort	*RexxPort;
LONG		 LastRexxError;
LONG		 RexxTimeoutVal;
BOOL		 UpdateRequired,TransferUpdateRequired;


/**********************************************************************/


	/* The signal semaphores to initialize at program startup time. */

struct SignalSemaphore *StartupSemaphoreTable[] =
{
	&MenuSemaphore,
	&OnlineSemaphore,
	&StatusSemaphore,
	&xONxOFF_Semaphore,
	&TerminalSemaphore,
	&PatternSemaphore,
	&DoubleBufferSemaphore,
	&WindowInfoSemaphore,
	&BufferTaskSemaphore,
	&ReviewTaskSemaphore,
	&RasterSemaphore,
	&BufferSemaphore,

	NULL
};

	/* The lists to initialize at program startup time. */

struct List *StartupListTable[] =
{
	(struct List *)&TextBufferHistory,
	(struct List *)&Update_MsgList,
	&TransferInfoList,
	&RexxDialMsgList,
	&PacketHistoryList,
	&FastMacroList,
	&DoubleBufferList,
	&XEM_MacroList,

	NULL
};


/**********************************************************************/


	/* Job queues and jobs. */

JobQueue	*MainJobQueue;
JobNode		*XEMJob,
		*SerialCheckJob,
		*SerialJob,
		*OwnDevUnitJob,
		*WindowJob,
		*SingleWindowJob,
		*WindowAndXEMJob,
		*RexxJob,
		*QueueJob,
		*ReleaseSerialJob,
		*StartupFileJob,
		*IconifyJob,
		*RebuildMenuJob,
		*ResetDisplayJob,
		*OnlineCleanupJob,
		*SerialCacheJob,
		*DisplayCostJob,
		*TerminalTestJob;

	/* Jobs to create at program startup time. */

struct JobInitStruct JobInitTable[] =
{
	"XEM",			&XEMJob,		HandleXEMJob,		JOBTYPE_Wait,
	"SerialCheck",		&SerialCheckJob,	HandleSerialCheckJob,	JOBTYPE_Wait,
	"Serial",		&SerialJob,		HandleSerialJob,	JOBTYPE_Wait,
	"OwnDevUnit",		&OwnDevUnitJob,		HandleOwnDevUnitJob,	JOBTYPE_Wait,
	"Window",		&SingleWindowJob,	HandleWindowJob,	JOBTYPE_Wait,
	"WindowAndXEM",		&WindowAndXEMJob,	HandleWindowAndXEMJob,	JOBTYPE_Wait,
	"Rexx",			&RexxJob,		HandleRexxJob,		JOBTYPE_Wait,
	"MsgQueue",		&QueueJob,		HandleQueueJob,		JOBTYPE_Wait,
	"ReleaseSerial",	&ReleaseSerialJob,	HandleReleaseSerial,	JOBTYPE_Once,
	"StartupFile",		&StartupFileJob,	HandleStartupFile,	JOBTYPE_Disposable,
	"Iconify",		&IconifyJob,		HandleIconifyJob,	JOBTYPE_Once,
	"RebuildMenu",		&RebuildMenuJob,	HandleRebuildMenuJob,	JOBTYPE_Once,
	"ResetDisplay",		&ResetDisplayJob,	HandleResetDisplayJob,	JOBTYPE_Once,
	"OnlineCleanup",	&OnlineCleanupJob,	HandleOnlineCleanupJob,	JOBTYPE_Once,
	"SerialCache",		&SerialCacheJob,	HandleSerialCacheJob,	JOBTYPE_Always,
	"DisplayCost",		&DisplayCostJob,	HandleDisplayCostJob,	JOBTYPE_Always,
	"TerminalTest",		&TerminalTestJob,	HandleTerminalTestJob,	JOBTYPE_Always,
	NULL
};


/**********************************************************************/


	/* Simplified serial read routines that know about cached data. */

ULONG	(*SerialWaitForData)(ULONG OtherBits);
ULONG	(*SerialGetWaiting)(VOID);
LONG	(*SerialRead)(UBYTE *Buffer,LONG Size);
