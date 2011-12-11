/*
**	Data.h
**
**	Global symbol definitions
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _DATA_H
#define _DATA_H 1

	/* In VersionTag.a */

extern LONG			 TermVersion;
extern LONG			 TermRevision;
extern UBYTE			 TermName[];
extern UBYTE			 TermDate[];
extern UBYTE FAR		 VersTag[];

/**********************************************************************/


extern BOOL			 DebugFlag;

	/* Libraries. */

extern struct ExecBase		*SysBase;
extern struct DosLibrary	*DOSBase;
extern struct Library		*GTLayoutBase;

extern struct IntuitionBase	*IntuitionBase;
extern struct GfxBase		*GfxBase;
extern struct RxsLib		*RexxSysBase;
extern struct LocaleBase	*LocaleBase;
extern struct Library		*GadToolsBase,
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
				*DataTypesBase,
				*OwnDevUnitBase;
#ifdef __AROS__
struct UtilityBase              *UtilityBase;
#endif
extern struct Device		*TimerBase,
				*ConsoleDevice;
extern BOOL			 Kick30;

	/* Timer device. */

extern struct timerequest	*TimeRequest;
extern struct MsgPort		*TimePort;

	/* Console device. */

extern struct IOStdReq		*ConsoleRequest;
extern struct KeyMap		*KeyMap;
extern BPTR			 KeySegment;

	/* Screen & Windows. */

extern struct Screen		*Screen,
				*SharedScreen;
extern struct Window		*Window;
extern ULONG			 OpenWindowTag;
extern LONG			 WindowWidth,
				 WindowHeight,
				 WindowLeft,
				 WindowTop;
extern struct Window		*StatusWindow;
extern UWORD			 StatusOffset;
extern struct RastPort		 StatusRastPort,
				*StatusRPort;
extern UWORD			 StatusDisplayLeft,
	 			 StatusDisplayTop,
	 			 StatusDisplayWidth,
	 			 StatusDisplayHeight;
extern APTR			 OldWindowPtr;
extern UBYTE			 ScreenTitle[80];
extern UBYTE			 WindowTitle[80];
extern UBYTE			 DepthMask;
extern LONG			 RenderPens[4],
				 MappedPens[2][32],
				 SafeTextPen;
extern struct DrawInfo		*DrawInfo;
extern UWORD			*Pens;
extern LONG			 ScreenWidth,
				 ScreenHeight;
extern BOOL			 UseMasking;

extern struct BlockMarker	*WindowMarker;
extern BOOL			 Marking;

	/* Graphics rendering. */

extern struct ViewPort		*VPort;
extern struct RastPort		*RPort;
extern struct Region		*ClipRegion,
				*OldRegion;
extern struct TextFont		*GFX,
				*CurrentFont,
				*UserTextFont;
extern WORD			 UserFontHeight,
				 UserFontWidth,
				 UserFontBase;
extern UBYTE			 UserFontName[MAX_FILENAME_LENGTH];
extern struct TextFont		*TextFont;
extern WORD			 TextFontHeight,
				 TextFontWidth,
				 TextFontBase;
extern struct TTextAttr		 TextAttr;
extern UBYTE			 TextFontName[MAX_FILENAME_LENGTH];
extern struct TTextAttr		 UserFont;
extern struct TagItem		 TagDPI[2];
extern APTR			 VisualInfo;
extern UWORD			 FontRightExtend;

extern ULONG			(* ReadAPen)(struct RastPort *RPort);
extern ULONG			(* ReadBPen)(struct RastPort *RPort);
extern ULONG			(* ReadDrMd)(struct RastPort *RPort);
extern ULONG			(* SetMask)(struct RastPort *RPort,ULONG Mask);
extern ULONG			(* GetMask)(struct RastPort *RPort);

extern UWORD			 MonoPens[];
extern UWORD			 StandardPens[];
extern UWORD			 ANSIPens[];
extern UWORD			 EGAPens[];
extern UWORD			 NewEGAPens[];
extern UWORD			 AtomicColours[16];
extern UWORD			 EGAColours[16];
extern UWORD			 ANSIColours[16];

extern UWORD			 DefaultColours[32],
				 BlinkColours[32],
				 NormalColours[32],
				 StandardColours[32],
				 PaletteSize;

	/* Line/Column offset tables. */

extern LONG			*OffsetXTable,
				*OffsetYTable;

	/* DOS data. */

extern struct Process		*ThisProcess;
extern struct Process		*StatusProcess;
extern struct Process		*RexxProcess;
extern struct MsgPort		*TermRexxPort;
extern struct WBStartup		*WBenchMsg;
extern BPTR			 WBenchLock;
extern struct TermPort		*TermPort;
extern BYTE			 CheckBit,
				 OwnDevBit;
extern BOOL			 InRexx;
extern struct timeval		 LimitTime;
extern UBYTE			 LimitMacro[MAX_FILENAME_LENGTH];

	/* Character conversion. */

extern struct InputEvent	*FakeInputEvent;

	/* Serial I/O. */

extern struct IOExtSer		*ReadRequest;
extern struct IOExtSer		*WriteRequest;
extern struct MsgPort		*ReadPort;
extern struct MsgPort		*WritePort;
extern UBYTE			 ReadChar;
extern UBYTE			*ReadBuffer,
				*HostReadBuffer;
extern ULONG			 DTERate,
				 SerialBufferSize;
extern BOOL			 ProcessIO;
extern BOOL			 FirstInvocation;

extern UBYTE			 NewDevice[MAX_FILENAME_LENGTH];
extern LONG			 NewUnit;
extern BOOL			 UseNewDevice,
				 UseNewUnit;

extern LONG			 BaudRates[];
extern WORD			 NumBaudRates;

extern struct RendezvousSemaphore	RendezvousSemaphore;

	/* Localization support. */

extern struct Catalog		*Catalog;
extern struct Locale		*Locale;
extern STRPTR			 DecimalPoint;
extern struct CatCompArrayType	*AppStrings;
extern WORD			 NumAppStrings;
extern UBYTE			 ConvNumber[30],
				 ConvNumber10[30];

	/* Hotkeys. */

extern struct Hotkeys		 Hotkeys;

	/* Static default fonts. */

extern struct TextAttr DefaultFont;
extern struct TextAttr GFXFont;

	/* Character lookup tables. */

extern BYTE IsPrintable[256];
extern BYTE IsGlyph[256];
extern BYTE CharType[256];

	/* ISO -> IBM font conversion. */

extern UBYTE IBMConversion[256];

	/* IBM -> ISO font conversion. */

extern UBYTE ISOConversion[256];

	/* Table of graphics characters. */

extern BYTE GfxTable[256];

	/* Special character treatment. */

extern struct SpecialKey	 SpecialKeys[14];
extern JUMP			*SpecialTable;

extern BYTE			 AbortMap[256];
extern JUMP			*AbortTable;

extern CONTRANSFER		 ConDump,
				 ConOutput,
				 ConProcessData;

	/* Some more handy buffers. */

extern UBYTE ARexxCommandBuffer[256],AmigaDOSCommandBuffer[256],DialNumberBuffer[256];

	/* Screen pull-down-menu. */

extern struct NewMenu		*TermMenu;
extern WORD			 NumMenuEntries;

extern struct SignalSemaphore	 MenuSemaphore;
extern struct Menu		*Menu;
extern struct Image		*AmigaGlyph,*CheckGlyph;

	/* Cross-hatch pattern. */

extern ULONG Crosshatch;

	/* Configuration. */

extern struct Configuration	*Config,
				*PrivateConfig,
				*BackupConfig;
extern struct SpeechConfig	 SpeechConfig;
extern BOOL			 English;
extern BOOL			 Online,WasOnline;
extern ULONG			 OnlineMinutes;
extern struct SignalSemaphore	 OnlineSemaphore;

extern UBYTE			 LastConfig[MAX_FILENAME_LENGTH];
extern UBYTE			 DefaultPubScreenName[MAXPUBSCREENNAME + 1],
				 SomePubScreenName[MAXPUBSCREENNAME + 1];

extern WORD			 StatusStackPointer;
extern WORD			 StatusStack[10];
extern struct SignalSemaphore	 StatusSemaphore;

	/* xON/xOFF processing. */

extern struct SignalSemaphore	 xONxOFF_Semaphore;
extern LONG			 xONxOFF_Lock;
extern BOOL			 xOFF_Enabled;

	/* Capture file. */

extern struct Buffer		*FileCapture;
extern BPTR			 PrinterCapture;
extern UBYTE			 CaptureName[MAX_FILENAME_LENGTH];
extern BOOL			 StandardPrinterCapture,
				 ControllerActive;
extern LONG			 BufferFlushCount;
extern COPTR			 CaptureData;
extern BOOL			 RawCapture;
extern ParseContext		*ParserStuff;

	/* Buffer. */

extern UBYTE			**BufferLines;
extern LONG			 Lines;
extern struct SignalSemaphore	 BufferSemaphore;
extern LONG			 MaxLines;
extern LONG			 BufferSpace;
extern BOOL			 BufferClosed;
extern BOOL			 BufferFrozen;

extern struct MinList		 TextBufferHistory;

	/* Sound support.c */

extern struct SoundConfig	 SoundConfig;
extern BOOL			 SoundChanged;

	/* Phonebook. */
/*
extern struct PhoneEntry	**Phonebook;
extern LONG			 PhoneSize;
extern LONG			 NumPhoneEntries;
extern struct MinList		 PhoneGroupList;
extern ULONG			 PhonebookID;
extern ULONG			 PhonebookDefaultGroup;
extern UBYTE			 PhonePassword[256];
extern BOOL			 PhonePasswordUsed,
				 PhonebookAutoDial,
				 PhonebookAutoExit,
				 RebuildMenu;
*/
extern PhonebookHandle		*GlobalPhoneHandle;
extern BOOL			 RebuildMenu;
extern UBYTE			 Password[100];
extern UBYTE			 UserName[100];

extern UBYTE			 CurrentBBSName[40],
				 CurrentBBSNumber[100],
				 CurrentBBSComment[100];

extern UBYTE			 LastPhone[MAX_FILENAME_LENGTH];
extern UBYTE			 LastKeys[MAX_FILENAME_LENGTH];
extern UBYTE			 LastMacros[MAX_FILENAME_LENGTH];
extern UBYTE			 LastCursorKeys[MAX_FILENAME_LENGTH];
extern UBYTE			 LastTranslation[MAX_FILENAME_LENGTH];
extern UBYTE			 LastSpeech[MAX_FILENAME_LENGTH];
extern UBYTE			 LastSound[MAX_FILENAME_LENGTH];

extern struct MacroKeys		*MacroKeys;
extern struct CursorKeys	*CursorKeys;

	/* Console stuff. */

extern UBYTE			*StripBuffer;
extern UWORD			 LastLine,LastColumn,LastPixel;
extern UWORD			 LastPrintableColumn;
extern UWORD			 LastPrintablePixel;
extern UWORD			 CharCellNominator,CharCellDenominator;
extern UBYTE			 CurrentFontScale,FontScalingRequired,CurrentCharWidth;
extern UBYTE			 Charset,Attributes,FgPen,BgPen;
extern WORD			 CursorX,CursorY,Top,Bottom;
extern BOOL			 UseRegion,RegionSet,InSequence,ConsoleQuiet,Blocking;
extern BYTE			*TabStops;
extern UWORD			 TabStopMax;
extern BYTE			 CharMode[2];

extern BOOL			 VT52_Mode;

extern BOOL			 ResetDisplay,
				 FixScreenSize;

extern struct SignalSemaphore	 TerminalSemaphore;	/* Protection from shape changers */

	/* Flow filter. */

extern STRPTR			 AttentionBuffers[SCAN_COUNT];
extern BOOL			 BaudPending,UseFlow;
extern UBYTE			 BaudBuffer[80],BaudCount;

extern struct FlowInfo		 FlowInfo;

extern STRPTR			 DataHold;
extern LONG			 DataSize;

	/* Transfer data. */

extern BOOL			 BinaryTransfer;
extern LONG			 TransferAbortState;
extern WORD			 TransferAbortCount;
extern LONG			 TransferBits;

extern CONTRANSFER		 ConTransfer;

extern UBYTE			*DownloadPath;
extern BOOL			 Uploading;
extern BOOL			 DidTransfer;

extern struct Window		*TransferWindow;
extern UBYTE			 TransferTitleBuffer[256];
extern BOOL			 TransferZoomed;
extern CONST_STRPTR		 TransferWindowTitle;

extern struct LayoutHandle	*TransferHandle;
extern struct List		 TransferInfoList;

extern struct FileTransferInfo	*FileTransferInfo;

extern struct XPR_IO		*XprIO;
extern struct MinList		 Update_MsgList;
extern WORD			 Update_MsgCount;
extern UBYTE			 ProtocolOptsBuffer[256];
extern BOOL			 NewOptions;
extern BOOL			 XPRCommandSelected;
extern UBYTE			 LastXprLibrary[MAX_FILENAME_LENGTH];
extern UBYTE			 TransferProtocolName[MAX_FILENAME_LENGTH];
extern BOOL			 UsesZModem,SendAbort,TransferAborted,TransferFailed,TransferError;

extern struct Process		*TransferProcess;

	/* The standard ZModem cancel sequence
	 * (ten CAN chars followed by ten
	 * BS chars).
	 */

extern UBYTE			 ZModemCancel[];

	/* Pay per minute. */

extern LONG			 PayPerUnit[2];
extern LONG			 SecPerUnit[2];
extern struct List		*PatternList,
				*ActivePattern;
extern UBYTE			 LastPattern[MAX_FILENAME_LENGTH];
extern BOOL			 PatternsChanged;
extern struct SignalSemaphore	 PatternSemaphore;

	/* Execute a command/script. */

extern ULONG			 BlockNestCount;
extern BOOL			 WeAreBlocking;

	/* Dial list. */

extern BOOL			 SendStartup;
extern LONG			 FirstDialMenu;
extern struct List		 RexxDialMsgList;
extern BOOL			 DialItemsAvailable;

	/* TermMain data. */

extern BOOL			 ReleaseSerial;
extern BOOL			 MainTerminated,
	 			 IconTerminated,
				 DoIconify,
				 DoDial,
				 KeepQuiet;

extern BOOL			 ConfigChanged,PhonebookChanged,TranslationChanged,
				 MacroChanged,CursorKeysChanged,FastMacrosChanged,
				 HotkeysChanged,SpeechChanged,BufferChanged;

	/* String gadget hooks. */

extern struct Window		*CommandWindow;
extern struct Gadget		*CommandGadget;

extern BOOL			 ClipInput,
				 ClipXerox,
				 ClipPrefix;

	/* Text sending. */

extern UBYTE			 SendPrompt[256];
extern LONG			 SendPromptLen;
extern SENDLINE			 SendLine;

	/* Packet window. */

extern struct Window		*PacketWindow;
extern struct Gadget		*PacketGadgetArray[2];
extern struct Menu		*PacketMenu;
extern struct Gadget		*PacketGadgetList;
extern struct List		 PacketHistoryList;

	/* Some more colour control flags. */

extern BOOL			 Initializing,LoadColours;

	/* The character raster. */

extern UBYTE			*Raster,*RasterAttr;
extern LONG			 RasterWidth,RasterHeight;
extern struct SignalSemaphore	 RasterSemaphore;
extern BOOL			 RasterEnabled;

	/* Generic lists. */

extern struct GenericList	*GenericListTable[GLIST_COUNT];

	/* File version test. */

extern ULONG VersionProps[2];

	/* Window stack. */

extern struct Window		*TopWindow;

extern struct Window		*WindowStack[5];
extern WORD			 WindowStackPtr;

	/* Global term ID. */

extern LONG			 TermID;
extern UBYTE			 TermIDString[MAXPUBSCREENNAME + 1];
extern UBYTE			 RexxPortName[256];

	/* Fast! macros. */

extern struct List		 FastMacroList;
extern LONG			 FastMacroCount;
extern UBYTE			 LastFastMacros[MAX_FILENAME_LENGTH];
extern struct Window		*FastWindow;
extern WORD			 FastWindowLeft,
				 FastWindowTop,
				 FastWindowHeight;

	/* Double-buffered file locking. */

extern struct List		 DoubleBufferList;
extern struct SignalSemaphore	 DoubleBufferSemaphore;

	/* XEM interface. */

extern struct XEM_IO		*XEM_IO;
extern ULONG			 XEM_Signal;
extern struct XEmulatorHostData XEM_HostData;
extern CONST UBYTE		*OptionTitle;
extern struct List		 XEM_MacroList;
extern struct XEmulatorMacroKey *XEM_MacroKeys;
extern UBYTE			 EmulationName[MAX_FILENAME_LENGTH];

	/* Screen scrolling support. */

extern UWORD			 ScrollLineFirst,ScrollLineLast,
				 ScrollLineCount;
extern struct ScrollLineInfo	*ScrollLines;

	/* term review buffer. */

extern struct Window		*ReviewWindow;

	/* Background information. */

extern ULONG			 BytesIn,
				 BytesOut;
extern struct DateStamp		 SessionStart;
extern struct Window		*InfoWindow;

	/* Gadget support routines */

extern LONG			 InterWidth,
				 InterHeight;

	/* Character translation. */

extern struct TranslationEntry	**SendTable,
				**ReceiveTable;

	/* Window alignment information. */

extern struct WindowInfo WindowInfoTable[];
extern struct SignalSemaphore WindowInfoSemaphore;

	/* New user interface. */

extern struct Hook LocaleHook;

	/* AmigaGuide hook. */

extern struct Hook GuideHook;

	/* Special queue management. */

extern struct MsgQueue	*SpecialQueue;

	/* Window and Screen backfill hook. */

extern struct Hook BackfillHook;

extern ULONG BackfillTag;

	/* Intercept data going through SerWrite. */

extern BYPASS		SerWriteBypass;

	/* The incredibly shrinking file name. */

extern UBYTE		ShrunkenName[MAX_FILENAME_LENGTH],
			OriginalName[MAX_FILENAME_LENGTH];

	/* Terminal emulation task. */

extern struct MsgQueue *TerminalQueue;

	/* AppIcon data. */

extern struct DiskObject DropIcon;

	/* Remember some of the incoming data. */

extern BOOL	RememberOutput,
		RememberInput,

		Recording,
		RecordingLine;

	/* Trap settings. */

extern BOOL		WatchTraps;
extern BOOL		TrapsChanged;
extern UBYTE		LastTraps[MAX_FILENAME_LENGTH];
extern ParseContext	*TrapStuff;

	/* CR/LF receiver translation data. */

extern TRANSLATEFUNC	Translate_CR_LF;

	/* End of line conversion labels. */

extern STRPTR EOL_Labels[];


/**********************************************************************/


	/* This follows the control code information. */

extern struct ControlCode	 ANSICode[];
extern WORD			 NumCodes;


/**********************************************************************/


extern UWORD			PenTable[16];
extern CONST_STRPTR		ParityMappings[];
extern CONST_STRPTR		BooleanMappings[];
extern CONST_STRPTR		DuplexMappings[];
extern CONST_STRPTR		HandshakingMappings[];
extern UBYTE			TextAttributeTable[16];
extern WORD			ForegroundPen,
				BackgroundPen;


/**********************************************************************/


extern BOOL			ChatMode;
extern BOOL			UseInternalZModem;
extern ULONG			SliderType;
extern ULONG			CycleType;


/**********************************************************************/


extern struct ColourTable	*NormalColourTable,
				*BlinkColourTable,
				*ANSIColourTable,
				*EGAColourTable,
				*DefaultColourTable,
				*MonoColourTable;


/**********************************************************************/


extern UBYTE Language[80];


/**********************************************************************/


extern STRPTR CharCodes[256];


/**********************************************************************/


extern struct Window *MatrixWindow;


/**********************************************************************/


extern SAVERASTER SaveRaster;


/**********************************************************************/


extern struct SignalSemaphore	 BufferTaskSemaphore;
extern struct TextBufferInfo	*BufferInfoData;

extern struct SignalSemaphore	 ReviewTaskSemaphore;
extern struct TextBufferInfo	*ReviewInfoData;


/**********************************************************************/


extern struct AttributeEntry	 AttributeTable[];
extern WORD			 AttributeTableSize;

	/* This is where the information on what arguments exclude
	 * other arguments is linked to the actual commands.
	 */

extern struct ExclusionInfo *ExclusionTable[];

	/* This is where the information on which arguments (switches and keywords)
	 * are actually required is stored.
	 */

extern UWORD InclusionTable[];

	/* This is where command names, arguments and the routines
	 * to execute the actual commands are linked together.
	 */

extern struct CommandInfo	 CommandTable[];
extern WORD			 CommandTableSize;

extern struct MsgPort		*RexxPort;
extern LONG			 LastRexxError;
extern LONG			 RexxTimeoutVal;
extern BOOL			 UpdateRequired,TransferUpdateRequired;


/**********************************************************************/


	/* The signal semaphores and lists to initialize at program startup time. */

extern struct SignalSemaphore	*StartupSemaphoreTable[];
extern struct List		*StartupListTable[];


/**********************************************************************/


	/* Job queues and jobs. */

extern JobQueue	*MainJobQueue;
extern JobNode	*XEMJob,
		*SerialCheckJob,
		*WorkbenchWindowJob,
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

extern struct JobInitStruct JobInitTable[];

/**********************************************************************/

	/* Simplified serial read routines that know about cached data. */

extern ULONG	(*SerialWaitForData)(ULONG OtherBits);
extern ULONG	(*SerialGetWaiting)(VOID);
extern LONG	(*SerialRead)(UBYTE *Buffer,LONG Size);

/**********************************************************************/

#endif
