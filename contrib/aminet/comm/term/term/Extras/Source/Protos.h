/*
**	Protos.h
**
**	Global function prototype definitions
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _PROTOS_H
#define _PROTOS_H 1

/**********************************************************************/

	/* Debugging macro */

#define DB(x)	;
/*#define DB(x)	x */

/**********************************************************************/

	/* debug.lib */

VOID kprintf(STRPTR,...);

	/* amiga.lib */
#ifndef __AROS__
ULONG HookEntry(struct Hook *,...);
#endif

	/* For DICE and the glue variant we have to change back to the standard stubs */

#if defined(_DCC) || defined(USE_GLUE)
#define AsmCreatePool(a1,a2,a3,x)	LibCreatePool(a1,a2,a3)
#define AsmDeletePool(a1,x)		LibDeletePool(a1)
#define AsmFreePooled(a1,a2,a3,x)	LibFreePooled(a1,a2,a3)
#define AsmAllocPooled(a1,a2,x)		LibAllocPooled(a1,a2)
#else
APTR ASM AsmCreatePool(REG(d0) ULONG MemFlags,REG(d1) ULONG PuddleSize,REG(d2) ULONG ThreshSize,REG(a6) struct ExecBase *SysBase);
VOID ASM AsmDeletePool(REG(a0) APTR PoolHeader,REG(a6) struct ExecBase *SysBase);
APTR ASM AsmAllocPooled(REG(a0) APTR PoolHeader, REG(d0) ULONG Size,REG(a6) struct ExecBase *SysBase);
VOID ASM AsmFreePooled(REG(a0) APTR PoolHeader,REG(a1) APTR Memory,REG(d0) ULONG MemSize,REG(a6) struct ExecBase *SysBase);
#endif

/**********************************************************************/

	/* StackCall.a */

LONG STACKARGS StackCall(LONG *Success,LONG StackSize,LONG ArgCount,LONG (* Function)(ULONG Arg1,...),...);
LONG StackSize(struct Task *Task);

/**********************************************************************/

	/* Glue.asm */

VOID CustomStuffTextGlue(LONG *Data, UBYTE Char);
VOID CustomCountCharGlue(LONG *Count, UBYTE Char);

VOID StuffCharGlue(struct FormatContext *Context, UBYTE Char);

VOID CountCharGlue(ULONG *Count);

struct RendezvousData * RendezvousLoginGlue(struct MsgPort *ReadPort, struct MsgPort *WritePort, struct TagItem *TagList);
VOID RendezvousLogoffGlue(struct RendezvousData *Data);
struct Node * RendezvousNewNodeGlue(STRPTR Name);

LONG xem_sread_glue(APTR Buffer, LONG Size, ULONG Timeout);
ULONG xem_toptions_glue(LONG NumOpts, struct xem_option **Opts);
LONG xem_swrite_glue(STRPTR Buffer, LONG Size);
LONG xem_tgets_glue(STRPTR Prompt, STRPTR Buffer, ULONG Size);
VOID xem_tbeep_glue(ULONG Times, ULONG Delay);
LONG xem_macrodispatch_glue(struct XEmulatorMacroKey *XEM_MacroKey);

LONG xpr_fopen_glue(STRPTR FileName, STRPTR AccessMode);
LONG xpr_fclose_glue(struct Buffer *File);
LONG xpr_fread_glue(APTR Buffer, LONG Size, LONG Count, struct Buffer *File);
LONG xpr_fwrite_glue(APTR Buffer, LONG Size, LONG Count, struct Buffer *File);
LONG xpr_fseek_glue(struct Buffer *File, LONG Offset, LONG Origin);
LONG xpr_sread_glue(APTR Buffer, ULONG Size, ULONG Timeout);
LONG xpr_swrite_glue(APTR Buffer, LONG Size);
LONG xpr_update_glue(struct XPR_UPDATE *UpdateInfo);
LONG xpr_gets_glue(STRPTR Prompt, STRPTR Buffer);
LONG xpr_setserial_glue(LONG Status);
LONG xpr_ffirst_glue(STRPTR Buffer, STRPTR Pattern);
LONG xpr_fnext_glue(LONG OldState, STRPTR Buffer, STRPTR Pattern);
LONG xpr_finfo_glue(STRPTR FileName, LONG InfoType);
ULONG xpr_options_glue(LONG NumOpts, struct xpr_option **Opts);
LONG xpr_unlink_glue(STRPTR FileName);
LONG xpr_getptr_glue(LONG InfoType);
LONG xpr_stealopts_glue(STRPTR Prompt, STRPTR Buffer);

/**********************************************************************/

/* About.c */
BOOL ShowAbout(LONG Ticks);

/* Accountant.c */
VOID DeleteAccountant(VOID);
BOOL CreateAccountant(VOID);
ULONG QueryAccountantTime(struct timeval *Time);
ULONG QueryAccountantCost(VOID);
ULONG StopAccountant(VOID);
VOID StartAccountant(ULONG OnlineSeconds);

/* AmigaGuide.c */
VOID GuideCleanup(VOID);
VOID GuideContext(LONG NewContextID);
ULONG SAVE_DS ASM GuideSetupHook(REG(a0) struct Hook *UnusedHook, REG(a2) HelpMsg *UnusedHelpMessage, REG(a1) struct IBox *UnusedBounds);
VOID GuideSetup(VOID);
VOID GuideDisplay(LONG ContextID);

/* ARexx.c */
BOOL IsNumeric(STRPTR String);
STRPTR CreateResult(STRPTR ResultString, LONG *Results);
STRPTR CreateResultLen(STRPTR ResultString, LONG *Results, LONG Len);
BOOL CreateVarArgs(STRPTR Value, struct RexxPkt *Packet, STRPTR Stem, ...);
STRPTR CreateVar(STRPTR Value, struct RexxPkt *Packet, STRPTR Name);
STRPTR CreateMatchBuffer(STRPTR Pattern);
BOOL MatchBuffer(STRPTR Buffer, STRPTR Name);
VOID DeleteMatchBuffer(STRPTR Buffer);
LONG ToMode(STRPTR Name);
LONG ToList(STRPTR Name);
LONG ToConfig(STRPTR Name);
LONG ToRequester(STRPTR Name);
LONG ToWindow(STRPTR Name);
VOID RexxPktCleanup(struct RexxPkt *Packet, STRPTR Result);
VOID SAVE_DS RexxServer(VOID);
BOOL HandleRexxJob(JobNode *UnusedJob);

/* ARexxAttributes.c */
STRPTR RexxGetAttr(struct RexxPkt *Pkt);
STRPTR RexxSetAttr(struct RexxPkt *Pkt);

/* ARexxCommands.c */
STRPTR RexxWait(struct RexxPkt *Pkt);
STRPTR RexxSendFile(struct RexxPkt *Pkt);
STRPTR RexxSelect(struct RexxPkt *Pkt);
STRPTR RexxSaveAs(struct RexxPkt *Pkt);
STRPTR RexxRemove(struct RexxPkt *Pkt);
STRPTR RexxRequestFile(struct RexxPkt *Pkt);
STRPTR RexxReceiveFile(struct RexxPkt *Pkt);
STRPTR RexxPrint(struct RexxPkt *Pkt);
STRPTR RexxOpenRequester(struct RexxPkt *Pkt);
STRPTR RexxOpen(struct RexxPkt *Pkt);
STRPTR RexxDelay(struct RexxPkt *Pkt);
STRPTR RexxCapture(struct RexxPkt *Pkt);
STRPTR RexxAdd(struct RexxPkt *Pkt);
STRPTR RexxActivate(struct RexxPkt *UnusedPkt);
STRPTR RexxBaud(struct RexxPkt *Pkt);
STRPTR RexxBeepScreen(struct RexxPkt *UnusedPkt);
STRPTR RexxCallMenu(struct RexxPkt *Pkt);
STRPTR RexxClear(struct RexxPkt *Pkt);
STRPTR RexxClearScreen(struct RexxPkt *UnusedPkt);
STRPTR RexxClose(struct RexxPkt *Pkt);
STRPTR RexxCloseDevice(struct RexxPkt *UnusedPkt);
STRPTR RexxCloseRequester(struct RexxPkt *Pkt);
STRPTR RexxDeactivate(struct RexxPkt *UnusedPkt);
STRPTR RexxDial(struct RexxPkt *Pkt);
STRPTR RexxDuplex(struct RexxPkt *Pkt);
STRPTR RexxFault(struct RexxPkt *Pkt);
STRPTR RexxGetClip(struct RexxPkt *Pkt);
STRPTR RexxGoOnline(struct RexxPkt *Pkt);
STRPTR RexxHangup(struct RexxPkt *Pkt);
STRPTR RexxHelp(struct RexxPkt *Pkt);
STRPTR RexxOpenDevice(struct RexxPkt *Pkt);
STRPTR RexxParity(struct RexxPkt *Pkt);
STRPTR RexxPasteClip(struct RexxPkt *Pkt);
STRPTR RexxProcessIO(struct RexxPkt *Pkt);
STRPTR RexxProtocol(struct RexxPkt *Pkt);
STRPTR RexxPutClip(struct RexxPkt *Pkt);
STRPTR RexxQuit(struct RexxPkt *Pkt);
STRPTR RexxRedial(struct RexxPkt *Pkt);
STRPTR RexxRequestNotify(struct RexxPkt *Pkt);
STRPTR RexxRequestNumber(struct RexxPkt *Pkt);
STRPTR RexxRequestResponse(struct RexxPkt *Pkt);
STRPTR RexxRequestString(struct RexxPkt *Pkt);
STRPTR RexxReset(struct RexxPkt *Pkt);
STRPTR RexxResetScreen(struct RexxPkt *UnusedPkt);
STRPTR RexxResetStyles(struct RexxPkt *UnusedPkt);
STRPTR RexxResetText(struct RexxPkt *UnusedPkt);
STRPTR RexxResetTimer(struct RexxPkt *UnusedPkt);
STRPTR RexxSave(struct RexxPkt *Pkt);
STRPTR RexxSend(struct RexxPkt *Pkt);
STRPTR RexxSendBreak(struct RexxPkt *Pkt);
STRPTR RexxSpeak(struct RexxPkt *Pkt);
STRPTR RexxStopBits(struct RexxPkt *Pkt);
STRPTR RexxTextBuffer(struct RexxPkt *Pkt);
STRPTR RexxTimeout(struct RexxPkt *Pkt);
STRPTR RexxTrap(struct RexxPkt *Pkt);
STRPTR RexxWindow(struct RexxPkt *Pkt);
STRPTR RexxRX(struct RexxPkt *Pkt);
STRPTR RexxExecTool(struct RexxPkt *Pkt);
STRPTR RexxRead(struct RexxPkt *Pkt);

/* ASCIIPanel.c */
struct Window *CreateASCIIWindow(BOOL Send);
VOID DeleteASCIIWindow(struct Window *Window, BOOL WaitForIt);
VOID AddASCIIMessage(STRPTR Message, ...);
VOID UpdateASCIIWindow(LONG Bytes, LONG MaxBytes, LONG Lines);
BOOL HandleASCIIWindow(VOID);

/* ASCIITransfer.c */
VOID ASCIISendSetup(VOID);
BOOL InternalASCIIUpload(STRPTR SingleFile, BOOL WaitForIt);
BOOL InternalASCIIDownload(STRPTR Name, BOOL WaitForIt);

/* Beep.c */
VOID Beep(VOID);
VOID BellSignal(VOID);

/* Boxes.c */
VOID SZ_SetTopEdge(LONG Top);
VOID SZ_SetLeftEdge(LONG Left);
VOID SZ_SetAbsoluteTop(LONG Top);
VOID SZ_SetWidth(LONG Width);
VOID SZ_AddLeftOffset(LONG Offset);
LONG SZ_LeftOffsetN(LONG DataArray, ...);
VOID SZ_SizeCleanup(VOID);
BOOL SZ_SizeSetup(struct Screen *Screen, struct TextAttr *TextAttr);
ULONG SZ_GetLen(STRPTR String);
VOID SZ_FreeBoxes(struct TextBox *FirstBox);
LONG SZ_BoxWidth(LONG Chars);
LONG SZ_BoxHeight(LONG Lines);
struct TextBox *SZ_CreateTextBox(struct TextBox **FirstBox, ...);
VOID SZ_SetBoxTitles(struct TextBox *Box, STRPTR Array, ...);
VOID SZ_SetLine(struct RastPort *RPort, struct TextBox *Box, LONG Line, STRPTR String);
VOID SZ_PrintLine(struct RastPort *RPort, struct TextBox *Box, LONG Line, STRPTR String, ...);
VOID SZ_DrawBoxes(struct RastPort *RPort, struct TextBox *FirstBox);
VOID SZ_MoveBoxes(struct TextBox *FirstBox, LONG Left, LONG Top);
VOID SZ_SetBoxes(struct TextBox *FirstBox, LONG Left, LONG Top);
LONG SZ_GetBoxInfo(struct TextBox *Box, LONG Type);

/* Buffer.c */
VOID AddLine(STRPTR Line, LONG Size);
VOID DeleteBuffer(VOID);
BOOL CreateBuffer(VOID);
VOID FreeBuffer(VOID);
VOID DeleteSearchInfo(struct SearchInfo *Info);
struct SearchInfo *CreateSearchInfo(STRPTR Pattern, BOOL Forward, BOOL IgnoreCase, BOOL WholeWords);
LONG SearchTextBuffer(struct SearchInfo *Info);
BOOL HandleSearchMessage(struct SearchContext *Context, struct IntuiMessage **MessagePtr);
VOID DeleteSearchContext(struct SearchContext *Context);
struct SearchContext *CreateSearchContext(struct Window *ParentWindow, STRPTR Buffer, struct Hook *HistoryHook, struct List *HistoryHookList, BOOL *Forward, BOOL *IgnoreCase, BOOL *WholeWords);

/* Call.c */
VOID MakeCall(STRPTR Name, STRPTR Number);
VOID StopCall(BOOL Finish);

/* Capture.c */
VOID CaptureFilteredTo_Buffer(APTR Buffer, LONG Size);
VOID CaptureFilteredTo_Buffer_File(APTR Buffer, LONG Size);
VOID CaptureFilteredTo_Buffer_File_Printer(APTR Buffer, LONG Size);
VOID CaptureFilteredTo_File(APTR Buffer, LONG Size);
VOID CaptureFilteredTo_File_Printer(APTR Buffer, LONG Size);
VOID CaptureFilteredTo_Printer(APTR Buffer, LONG Size);
VOID CaptureFilteredTo_Buffer_Printer(APTR Buffer, LONG Size);
VOID CaptureRawTo_Buffer(APTR Buffer, LONG Size);
VOID CaptureRawTo_Buffer_File(APTR Buffer, LONG Size);
VOID CaptureRawTo_Buffer_File_Printer(APTR Buffer, LONG Size);
VOID CaptureRawTo_Buffer_Printer(APTR Buffer, LONG Size);
VOID CaptureRawTo_File(APTR Buffer, LONG Size);
VOID CaptureRawTo_File_Printer(APTR Buffer, LONG Size);
VOID CaptureRawTo_Printer(APTR Buffer, LONG Size);
VOID ClosePrinterCapture(BOOL Force);
BOOL OpenPrinterCapture(BOOL Controller);
VOID CloseFileCapture(VOID);
BOOL OpenFileCapture(BOOL Raw);
VOID CaptureToFile(APTR Buffer, LONG Size);
VOID CaptureToPrinter(APTR Buffer, LONG Size);

/* CapturePanel.c */
BOOL CapturePanel(struct Window *Parent, struct Configuration *LocalConfig);

/* CaptureParser.c */
VOID CaptureParserExit(VOID);
BOOL CaptureParserInit(VOID);
ParseContext *CreateParseContext(VOID);
VOID CaptureParser(ParseContext *Context, STRPTR Buffer, LONG Size, COPTR OutputRoutine);

/* Chat.c */
VOID HideChatGadget(VOID);
VOID DeleteChatGadget(VOID);
VOID UpdateChatGadget(VOID);
VOID ActivateChat(BOOL Reactivate);
BOOL CreateChatGadget(VOID);
VOID HandleChatGadget(UWORD GadgetCode);
VOID MarkChatGadgetAsActive(VOID);

/* Choose.c */
struct FileRequester *SaveDrawer(struct Window *Parent, STRPTR TitleText, STRPTR PositiveText, STRPTR DrawerName, LONG DrawerNameSize);
struct FileRequester *OpenDrawer(struct Window *Parent, STRPTR TitleText, STRPTR PositiveText, STRPTR DrawerName, LONG DrawerNameSize);
struct FileRequester *SaveFile(struct Window *Parent, STRPTR TitleText, STRPTR PositiveText, STRPTR Pattern, STRPTR FullName, LONG FullNameSize);
struct FileRequester *OpenSingleFile(struct Window *Parent, STRPTR TitleText, STRPTR PositiveText, STRPTR Pattern, STRPTR FullName, LONG FullNameSize);
struct FileRequester *OpenSeveralFiles(struct Window *Parent, STRPTR TitleText, STRPTR PositiveText, STRPTR Pattern, STRPTR FullName, LONG FullNameSize);
BOOL OpenAnyFont(struct Window *Parent, STRPTR FontName, WORD *FontSize);
BOOL OpenFixedFont(struct Window *Parent, STRPTR FontName, WORD *FontSize);

/* Clip.c */
VOID CloseClip(VOID);
LONG GetClip(STRPTR Buffer, LONG Len);
LONG OpenClip(LONG Unit);
BOOL GetClipContents(LONG Unit, APTR *Buffer, LONG *Size);
BOOL AddClip(STRPTR Buffer, LONG Size);
BOOL SaveClip(STRPTR Buffer, LONG Size);
LONG LoadClip(STRPTR Buffer, LONG Size);
BOOL WriteTranslatedToClip(struct IFFHandle *Handle, STRPTR Buffer, LONG Length);

/* ClipPanel.c */
BOOL ClipPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* Colour.c */
VOID Colour96xColourTable(ULONG *Source96, ColourTable *Table, LONG NumColours);
VOID Colour12xColourTable(UWORD *Source12, ColourTable *Table, LONG NumColours);
VOID Colour12x96(UWORD *Source12, ULONG *Dest96, LONG NumColours);
VOID Colour96x12(ULONG *Source96, UWORD *Dest12, LONG NumColours);
VOID CopyColourEntry(ColourTable *Source, ColourTable *Destination, LONG From, LONG To);
VOID ColourTablex96(ColourTable *Table, ULONG *Dest96);
VOID DeleteColourTable(ColourTable *Table);
ColourTable *CreateColourTable(LONG NumEntries, UWORD *Source12, ULONG *Source96);
VOID LoadColourTable(struct ViewPort *VPort, ColourTable *Table, UWORD *Colour12, LONG NumColours);
VOID GrabColours(struct ViewPort *VPort, ColourTable *Table);
VOID CopyColours(ColourTable *From, ColourTable *To);

/* CommandPanel.c */
BOOL CommandPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* Config.c */
VOID StripGlobals(struct Configuration *LocalConfig);
VOID FinalFix(struct Configuration *LocalConfig, BOOL UnusedIsPhonebook, LONG Version, LONG Revision);
VOID FixOldConfig(struct Configuration *LocalConfig, UBYTE ConfigChunkType, BOOL IsPhonebook, LONG Version, LONG Revision);
VOID FixScreenPens(struct ScreenSettings *ScreenConfig);
VOID ResetConfig(struct Configuration *LocalConfig, STRPTR PathBuffer);
VOID DeleteConfigEntry(struct Configuration *LocalConfig, LONG Type);
VOID ResetConfigEntry(struct Configuration *LocalConfig, LONG Type);
APTR GetConfigEntry(struct Configuration *LocalConfig, LONG Type);
LONG CompareConfigEntries(APTR a, APTR b, LONG Type);
VOID PutConfigEntry(struct Configuration *LocalConfig, APTR Data, LONG Type);
VOID CopyConfigEntry(struct Configuration *LocalConfig, LONG Type, APTR Data);
APTR CreateNewConfigEntry(LONG Type);
BOOL CreateConfigEntry(struct Configuration *LocalConfig, LONG Type);
VOID DeleteConfiguration(struct Configuration *LocalConfig);
struct Configuration *CreateConfiguration(BOOL Fill);
VOID SaveConfig(struct Configuration *Src, struct Configuration *Dst);
VOID SwapConfig(struct Configuration *Src, struct Configuration *Dst);
BOOL SavePhonebook(STRPTR Name, PhonebookHandle *PhoneHandle);
PhonebookHandle *LoadPhonebook(STRPTR Name);
BOOL WriteConfig(STRPTR Name, struct Configuration *LocalConfig);
BOOL ReadConfig(STRPTR Name, struct Configuration *LocalConfig);
VOID ResetHotkeys(struct Hotkeys *Hotkeys);
VOID ResetSpeechConfig(struct SpeechConfig *SpeechConfig);
VOID ResetCursorKeys(struct CursorKeys *Keys);
VOID ResetSound(struct SoundConfig *SoundConfig);
VOID ResetMacroKeys(struct MacroKeys *Keys);
BOOL LoadMacros(STRPTR Name, struct MacroKeys *Keys);
BOOL WriteIFFData(STRPTR Name, APTR Data, LONG Size, ULONG Type);
BOOL ReadIFFData(STRPTR Name, APTR Data, LONG Size, ULONG Type);

/* Console.c */
VOID ConProcess(STRPTR String, LONG Size);
VOID ConPrintf(STRPTR String, ...);
VOID ConProcessUpdate(VOID);
VOID ConOutputUpdate(VOID);
VOID ConFontScaleUpdate(VOID);
VOID ConTransferUpdate(VOID);
VOID ConClear(VOID);
VOID ConResetFont(VOID);
VOID ConResetStyles(VOID);
VOID ConResetTerminal(VOID);
BOOL SetConsoleQuiet(BOOL NewSettings);

/* ControlSequences.c */
VOID SerialCommand(STRPTR String);
VOID ConsoleCommand(STRPTR String);

/* CopyPanel.c */
BOOL CopyPanel(struct Window *Parent, struct Configuration *LocalConfig, BOOL Selective);

/* Crypt.c */
VOID Encrypt(UBYTE *Source, LONG SourceLen, UBYTE *Destination, UBYTE *Key, LONG KeyLen);
VOID Decrypt(UBYTE *Source, LONG SourceLen, UBYTE *Destination, UBYTE *Key, LONG KeyLen);

/* CursorPanel.c */
BOOL CursorPanelConfig(struct Configuration *LocalConfig, struct CursorKeys *CursorKeys, STRPTR LastCursorKeys, struct Window *Parent, BOOL *ChangedPtr);

/* CustomRequest.c */
#ifdef __AROS__
VOID SAVE_DS ASM CustomStuffText(REG(d0) UBYTE Char, REG(a3) LONG *Data);
VOID ASM CustomCountChar(REG(d0) UBYTE Char, REG(a3) LONG *Count);
#else
VOID SAVE_DS ASM CustomStuffText(REG(a3) LONG *Data, REG(d0) UBYTE Char);
VOID ASM CustomCountChar(REG(a3) LONG *Count, REG(d0) UBYTE Char);
#endif
LONG ShowInfo(struct Window *Parent, STRPTR Title, STRPTR Continue, STRPTR FormatString, ...);

/* DatePanel.c */
BOOL DatePanel(struct Window *Parent, struct TimeDateNode *Node);

/* DayPanel.c */
BOOL DayPanel(struct Window *Parent, struct TimeDateNode *Node);

/* Dial.c */
BOOL DialPanel(PhonebookHandle *PhoneHandle);

/* DialList.c */
VOID ToggleDialEntry(PhonebookHandle *PhoneHandle, PhoneEntry *Entry);
VOID MarkDialEntry(PhonebookHandle *PhoneHandle, PhoneEntry *Entry);
VOID UnmarkDialEntry(PhonebookHandle *PhoneHandle, PhoneEntry *Entry);
VOID ReattachDialNode(PhonebookHandle *PhoneHandle, PhoneEntry *Entry);
VOID CleanseDialList(PhonebookHandle *PhoneHandle);
struct DialNode *RemoveAndDeleteRelatedDialNodes(PhonebookHandle *PhoneHandle, struct DialNode *ThisNode);
VOID DeleteDialNode(struct DialNode *SomeNode);
VOID RemoveDialNode(PhonebookHandle *PhoneHandle, struct DialNode *SomeNode);
BOOL AddDialNode(PhonebookHandle *PhoneHandle, struct DialNode *Node);
VOID DeleteDialList(PhonebookHandle *PhoneHandle);
BOOL AddDialEntry(PhonebookHandle *PhoneHandle, PhoneEntry *Entry, STRPTR Number);
BOOL AddAllDialEntries(PhonebookHandle *PhoneHandle);

/* EditRoutine.c */
ULONG SAVE_DS ASM CommonEditRoutine(REG(a0) struct Hook *UnusedHook, REG(a2) struct SGWork *Work, REG(a1) Msg msg);

/* Emulation.c */
VOID UpdatePens(VOID);
LONG GetFontWidth(VOID);
VOID RethinkRasterLimit(VOID);
#ifdef __AROS__
# ifdef ScrollRegion
# undef ScrollRegion
# endif
#endif
VOID ScrollRegion(LONG Direction);
BOOL DoCancel(VOID);
BOOL ParseCode(LONG c);
VOID NormalCursor(VOID);
VOID GhostCursor(VOID);
VOID RepositionCursor(VOID);
VOID ClearCursor(VOID);
VOID DrawCursor(VOID);
VOID BackupRender(VOID);
VOID ShiftChar(LONG Size);
VOID Ignore(VOID);
VOID ScrollDown(STRPTR Buffer);
VOID ScrollUp(STRPTR Buffer);
VOID CursorScrollDown(VOID);
VOID DownLine(VOID);
VOID CursorScrollUp(VOID);
VOID NextLine(VOID);
VOID SaveCursor(VOID);
VOID FontStuff(STRPTR Buffer);
VOID LoadCursor(VOID);
VOID ScaleFont(STRPTR Buffer);
VOID AlignmentTest(VOID);
VOID SetTab(VOID);
VOID RequestTerminal(STRPTR Buffer);
VOID RequestTerminalParams(STRPTR Buffer);
VOID SoftReset(VOID);
VOID Reset(VOID);
VOID PrinterController(STRPTR Buffer);
VOID RequestInformation(STRPTR Buffer);
VOID SetSomething(STRPTR Buffer);
VOID NumericAppMode(STRPTR Buffer);
VOID MoveCursor(STRPTR Buffer);
VOID MoveColumn(STRPTR Buffer);
VOID EraseLine(STRPTR Buffer);
VOID EraseScreen(STRPTR Buffer);
VOID EraseCharacters(STRPTR Buffer);
VOID InsertCharacters(STRPTR Buffer);
VOID InsertLine(STRPTR Buffer);
VOID ClearLine(STRPTR Buffer);
VOID SetTabs(STRPTR Buffer);
VOID SetAbsolutePosition(STRPTR Buffer);
VOID SetTopPosition(STRPTR Buffer);
VOID SetAttributes(STRPTR Buffer);
#ifdef __AROS__
# ifdef SetRegion
#  undef SetRegion
# endif
#endif
VOID SetRegion(STRPTR Buffer);
VOID ResetCursor(VOID);
VOID MoveCursorUp(STRPTR Buffer);
VOID MoveCursorDown(STRPTR Buffer);
VOID VT52_CursorUp(STRPTR UnusedBuffer);
VOID VT52_CursorDown(STRPTR UnusedBuffer);
VOID VT52_CursorRight(STRPTR UnusedBuffer);
VOID VT52_CursorLeft(STRPTR UnusedBuffer);
VOID VT52_CursorHome(STRPTR UnusedBuffer);
VOID VT52_RevLF(STRPTR UnusedBuffer);
VOID VT52_EraseEOS(STRPTR UnusedBuffer);
VOID VT52_EraseEOL(STRPTR UnusedBuffer);
VOID VT52_SetCursor(STRPTR Buffer);
VOID VT52_PrintOn(STRPTR UnusedBuffer);
VOID VT52_PrintOff(STRPTR UnusedBuffer);
VOID VT52_PrintScreen(STRPTR UnusedBuffer);
VOID VT52_PrintLine(STRPTR UnusedBuffer);
VOID VT52_EnterANSI(STRPTR UnusedBuffer);

/* EmulationPanel.c */
BOOL EmulationPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* EmulationProcess.c */
VOID DeleteEmulationProcess(VOID);
BOOL CreateEmulationProcess(VOID);

/* EmulationSpecial.c */
BOOL DoxON_xOFF(VOID);
BOOL DoBackspace(VOID);
BOOL DoBeep(VOID);
BOOL DoLF(VOID);
BOOL DoShiftIn(VOID);
BOOL DoShiftOut(VOID);
BOOL DoCR_LF(VOID);
BOOL DoFF(VOID);
BOOL DoLF_FF_VT(VOID);
BOOL DoCR(VOID);
BOOL DoTab(VOID);
BOOL DoEnq(VOID);
BOOL DoEsc(VOID);
BOOL DoCsi(VOID);
BOOL DoNewEsc(LONG UnusedChar);
BOOL DoNewCsi(LONG UnusedChar);

/* EndOfLineTranslate.c */
VOID Update_CR_LF_Translation(VOID);

/* FastMacroPanel.c */
BOOL FastMacroPanelConfig(struct Configuration *LocalConfig, struct List *FastMacroList, STRPTR LastFastMacros, struct Window *Parent, BOOL *ChangedPtr);

/* FastMacros.c */
struct MacroNode *CreateFastMacroNode(STRPTR Macro);
BOOL SaveFastMacros(STRPTR Name, struct List *FastMacroList);
BOOL LoadFastMacros(STRPTR Name, struct List *FastMacroList);

/* FastMacroWindow.c */
VOID RefreshFastWindow(VOID);
VOID CloseFastWindow(VOID);
BOOL OpenFastWindow(VOID);
VOID HandleFastWindowGadget(ULONG MsgClass, UWORD MsgCode);

/* FileBuffer.c */
LONG BPrintf(struct Buffer *Buffer, STRPTR Format, ...);
BOOL BufferFlush(struct Buffer *Buffer);
BOOL BufferClose(struct Buffer *Buffer);
struct Buffer *BufferOpen(STRPTR Name, STRPTR AccessMode);
BOOL BufferSeek(struct Buffer *Buffer, LONG Offset, LONG Origin);
LONG BufferRead(struct Buffer *Buffer, STRPTR Destination, LONG Size);
LONG BufferWrite(struct Buffer *Buffer, STRPTR Source, LONG Size);

/* FixPath.c */
VOID AttachCLI(struct WBStartup *Startup);

/* Flow.c */
LONG FlowFilter(UBYTE *Data, LONG Size, UBYTE Mask);
VOID ResetDataFlowFilter(VOID);

/* HotkeyPanel.c */
BOOL HotkeyPanelConfig(struct Configuration *LocalConfig, struct Hotkeys *Hotkeys, STRPTR LastKeys, BOOL *ChangePtr);

/* Hotkeys.c */
VOID ShutdownCx(VOID);
BOOL SetupCx(VOID);
BOOL LoadHotkeys(STRPTR Name, struct Hotkeys *Keys);

/* Identify.c */
LONG GetFileType(STRPTR Name, BOOL AttachComment);
VOID Identify(STRPTR Name, BOOL AttachComment);
VOID AddIcon(STRPTR Name, LONG Index, BOOL Override);

/* ImportPanel.c */
BOOL ImportPanel(struct Window *Parent, PhonebookHandle *PhoneHandle, PhoneEntry *Entry, struct List *TimeDateList);

/* InfoWindow.c */
VOID CloseInfoWindow(VOID);
VOID OpenInfoWindow(VOID);

/* Init.c */
struct Library *SafeOpenLibrary(STRPTR Name, LONG Version);
VOID TTYResize(VOID);
VOID UpdateTerminalLimits(VOID);
VOID Current2DefaultPalette(struct Configuration *SomeConfig);
VOID Default2CurrentPalette(struct Configuration *SomeConfig);
VOID PaletteSetup(struct Configuration *SomeConfig);
VOID ScreenSizeStuff(VOID);
VOID PubScreenStuff(VOID);
VOID ConfigSetup(VOID);
BOOL DisplayReset(VOID);
BOOL DeleteDisplay(VOID);
STRPTR CreateDisplay(BOOL UsePresetSize, BOOL Activate);
VOID CloseAll(VOID);
STRPTR OpenAll(STRPTR ConfigPath);

/* Job.c */
VOID DeleteJobQueue(JobQueue *Queue);
JobQueue *CreateJobQueue(VOID);
VOID DeleteJob(JobNode *Node);
JobNode *CreateJob(STRPTR Name, WORD Type, JOBFUNCTION Function, ULONG Mask);
VOID RemoveJob(JobQueue *Queue, JobNode *Job);
VOID AddJob(JobQueue *Queue, JobNode *Job);
VOID SuspendJob(JobQueue *Queue, JobNode *Job);
VOID ActivateJob(JobQueue *Queue, JobNode *Job);
VOID UpdateJobMask(JobQueue *Queue, JobNode *Job, ULONG Mask);
VOID UpdateJob(JobQueue *Queue, JobNode *Job, ULONG Mask);
ULONG GetJobMask(JobNode *Job);
BOOL RunJob(JobNode *Job);
VOID ProcessJobQueue(JobQueue *Queue);

/* KeyConvert.c */
LONG ConvertTheKey(STRPTR Buffer, LONG *Len, UWORD Code, UWORD Qualifier, ULONG Prev);
LONG KeyConvert(struct IntuiMessage *Message, STRPTR Buffer, LONG *Len);

/* Launch.c */
VOID DeleteLaunchMsg(LaunchMsg *Startup);
LaunchMsg *CreateRexxCmdLaunchMsg(STRPTR RexxCmd, struct RexxPkt *RexxPkt, LAUNCHCLEANUP Cleanup);
LaunchMsg *CreateProgramLaunchMsg(STRPTR Program, LAUNCHCLEANUP Cleanup);
LONG LaunchSomething(STRPTR OriginalStreamName, BOOL Synchronous, LaunchMsg *Startup);
VOID LaunchSetup(VOID);
LONG GetLaunchCounter(VOID);

/* LibPanel.c */
BOOL SignaturePanel(struct Window *Parent, STRPTR String, LONG ReceiveMode);
BOOL LibPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* LibPanelPlus.c */
VOID ASCIITransferPanel(struct Window *Window, struct Configuration *LocalConfig);
BOOL ExternalCommandPanel(struct Window *Parent, STRPTR Command);

/* Lists.c */
VOID ClearGenericList(struct GenericList *List, BOOL Notify);
struct GenericList *CreateGenericList(VOID);
struct Node *FirstGenericListNode(struct GenericList *List);
struct Node *LastGenericListNode(struct GenericList *List);
struct Node *NextGenericListNode(struct GenericList *List);
struct Node *PrevGenericListNode(struct GenericList *List);
struct Node *DeleteGenericListNode(struct GenericList *List, struct Node *Node, BOOL Notify);
struct Node *CreateGenericListNode(LONG Size, STRPTR Name);
VOID AddGenericListNode(struct GenericList *List, struct Node *Node, LONG Mode, BOOL Notify);
LONG GenericListCount(struct GenericList *List);
struct Node *RemoveFirstGenericListNode(struct GenericList *List);
VOID LockGenericList(struct GenericList *List);
VOID SharedLockGenericList(struct GenericList *List);
VOID UnlockGenericList(struct GenericList *List);
VOID SelectGenericListNode(struct GenericList *List, struct Node *Node, LONG Offset);

/* Locale.c */
VOID LocaleOpen(STRPTR CatalogName, STRPTR BuiltIn, LONG Version);
VOID LocaleClose(VOID);
VOID LanguageCheck(VOID);
VOID SmallCurrency(STRPTR Buffer, LONG BufferSize);
VOID InsertGrouping(STRPTR Buffer, STRPTR GroupData, STRPTR GroupSeparator);
VOID CreateSum(LONG Quantity, BOOL UseCurrency, STRPTR Buffer, LONG BufferSize);
VOID LocalizeString(STRPTR *Strings, LONG From, LONG To);
VOID LocalizeStringTable(STRPTR *Strings, LONG *Table);
STRPTR LocaleString(ULONG ID);
STRPTR SAVE_DS ASM LocaleHookFunc(REG(a0) struct Hook *UnusedHook, REG(a2) APTR Unused, REG(a1) LONG ID);
BOOL FormatStamp(struct DateStamp *Stamp, STRPTR BothBuffer, LONG BothBufferSize, BOOL SubstituteDay);
VOID FormatTime(STRPTR Buffer, LONG BufferSize, LONG Hours, LONG Minutes, LONG Seconds);
STRPTR SAVE_DS STACKARGS StandardShowTime(struct Gadget *UnusedGadget, LONG Seconds);

/* MacroPanel.c */
BOOL MacroPanelConfig(struct Configuration *LocalConfig, struct MacroKeys *MacroKeys, STRPTR LastMacros, struct Window *Parent, BOOL *ChangePtr);

/* Main.c */
LONG Main(VOID);
VOID SendInputTextBuffer(STRPTR Buffer, LONG Len, BOOL Bell, BOOL ConvertLF);
BOOL HandleSerialCheckJob(JobNode *UnusedJob);
BOOL HandleQueueJob(JobNode *UnusedJob);
BOOL HandleOwnDevUnitJob(JobNode *UnusedJob);
BOOL HandleWindowJob(JobNode *UnusedJob);
BOOL HandleWindowAndXEMJob(JobNode *UnusedJob);
BOOL HandleWindow(VOID);
VOID HandleMenuCode(ULONG Code, ULONG Qualifier);
BOOL HandleReleaseSerial(JobNode *UnusedJob);
BOOL HandleStartupFile(JobNode *UnusedJob);
BOOL HandleIconifyJob(JobNode *UnusedJob);
BOOL HandleRebuildMenuJob(JobNode *UnusedJob);
BOOL HandleResetDisplayJob(JobNode *UnusedJob);
BOOL HandleOnlineCleanupJob(JobNode *UnusedJob);
BOOL HandleDisplayCostJob(JobNode *Job);

/* Matrix.c */
VOID CloseMatrixWindow(VOID);
BOOL DispatchMatrixWindow(ULONG *MsgClass, UWORD MsgCode, ULONG MsgQualifier, UBYTE Char);
struct Window *OpenMatrixWindow(struct Window *Parent);

/* Memory.c */
BOOL MemorySetup(VOID);
VOID MemoryCleanup(VOID);
VOID FreeVecPooled(APTR Memory);
APTR AllocVecPooled(ULONG Size, ULONG Flags);

/* Menus.c */
VOID PrepareLocalizedMenu(struct NewMenu **NewMenuPtr, WORD *NumMenuEntryPtr);
VOID SetMenuStrips(struct Menu *Menu);
VOID ResetMenuStrips(struct Menu *Menu);
VOID ClearMenuStrips(VOID);
BOOL AttachMenu(struct Menu *ThisMenu);
VOID DisconnectDialMenu(VOID);
struct Menu *BuildMenu(VOID);
VOID SetClipMenu(BOOL Mode);
VOID SetRedialMenu(VOID);
VOID SetDialMenu(BOOL Mode);
VOID SetTransferMenu(BOOL Mode);
VOID SetRasterMenu(BOOL Mode);
BOOL GetItem(ULONG MenuID);
VOID SetItem(ULONG MenuID, BOOL Mode);
BOOL CreateMenuGlyphs(struct Screen *Screen, struct DrawInfo *DrawInfo, struct Image **AmigaPtr, struct Image **CheckPtr);

/* MiscPanel.c */
BOOL MiscPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* ModemPanel.c */
BOOL ModemPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* MsgQueue.c */
APTR GetMsgItem(struct MsgQueue *Queue);
VOID PutMsgItem(struct MsgQueue *Queue, struct MsgItem *Item);
VOID DeleteMsgItem(struct MsgItem *Item);
struct MsgItem *CreateMsgItem(LONG Size);
VOID UnlockMsgQueue(struct MsgQueue *Queue);
VOID DeleteMsgQueue(struct MsgQueue *Queue);
struct MsgQueue *CreateMsgQueue(ULONG SigMask, LONG MaxSize);
VOID SetQueueDiscard(struct MsgQueue *Queue, BOOL Mode);

/* NewMarker.c */
struct MarkerContext *CreateMarkerContext(MARKER_AskPosition AskPosition, MARKER_Scroll Scroll, MARKER_Highlight Highlight, MARKER_Highlight Unhighlight, MARKER_TransferStartStop TransferStartStop, MARKER_Transfer Transfer, MARKER_Put Put, MARKER_Put PutLine, MARKER_PickWord PickWord);
VOID ResetMarkerContext(struct MarkerContext *Context);
VOID UpMarker(struct MarkerContext *Context);
VOID DownMarker(struct MarkerContext *Context, LONG FirstVisibleLine, LONG NumVisibleLines, LONG NumLines, LONG FirstVisibleColumn, LONG NumVisibleColumns, LONG NumColumns);
VOID MoveMouseMarker(struct MarkerContext *Context);
BOOL CheckMarkerHighlighting(struct MarkerContext *Context, LONG Line, LONG *Left, LONG *Right);
VOID TransferMarker(struct MarkerContext *Context, ULONG Qualifier);
BOOL SetWordMarker(struct MarkerContext *Context, LONG FirstVisibleLine, LONG NumVisibleLines, LONG NumLines, LONG FirstVisibleColumn, LONG NumVisibleColumns, LONG NumColumns);
//#ifndef __AROS__
VOID SetMarker(struct MarkerContext *Context, LONG FirstVisibleLine, LONG NumVisibleLines, LONG NumLines, LONG FirstVisibleColumn, LONG NumVisibleColumns, LONG NumColumns, LONG Line, LONG WordLeft, LONG WordRight);
//#endif
VOID SelectAllMarker(struct MarkerContext *Context, LONG FirstVisibleLine, LONG NumVisibleLines, LONG NumLines, LONG FirstVisibleColumn, LONG NumVisibleColumns, LONG NumColumns, LONG FirstLeft, LONG FirstLine, LONG LastRight, LONG LastLine);
BOOL CheckMarker(struct MarkerContext *Context);

/* OldConfig.c */
PhonebookHandle *LoadOldPhonebook(STRPTR Name);
BOOL ReadOldConfig(STRPTR Name, struct Configuration *LocalConfig);

/* OwnDevUnit.c */
VOID UnlockDevice(VOID);
BOOL LockDevice(STRPTR Device, LONG Unit, STRPTR ErrorString, LONG ErrorStringSize);

/* Packet.c */
VOID ClearPacketHistory(VOID);
VOID AddPacketHistory(STRPTR Buffer);
VOID DeletePacketWindow(BOOL WindowOnly);
BOOL CreatePacketWindow(VOID);

/* PathPanel.c */
BOOL PathPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* PatternPanel.c */
BOOL PatternPanelConfig(struct List *PatternList, STRPTR LastPattern, BOOL *ChangedPtr);

/* Phone.c */
PhoneEntry **CreatePhonebookTable(PhoneEntry **OldTable, LONG OldTableSize, LONG NumEntriesWanted, LONG *NumEntriesGot);
VOID DeletePhoneEntry(PhoneEntry *Entry);
PhoneEntry *CreatePhoneEntry(PhonebookHandle *PhoneHandle);
VOID RemovePhoneEntry(PhonebookHandle *PhoneHandle, PhoneEntry *Entry);
BOOL AddPhoneEntry(PhonebookHandle *PhoneHandle, PhoneEntry *NewEntry);
BOOL MergePhonebooks(PhonebookHandle *SrcHandle, PhonebookHandle *DstHandle);
PhonebookHandle *CreatePhonebook(LONG NumEntries, BOOL AllocateEntries);
VOID DeletePhonebook(PhonebookHandle *PhoneHandle);
VOID CopyPhoneHeader(PhoneHeader *From, PhoneHeader *To);
LONG MovePhoneEntry(PhonebookHandle *PhoneHandle, LONG Index, LONG How);
PhoneEntry *GetActiveEntry(PhonebookHandle *PhoneHandle);
VOID SetActiveEntry(PhonebookHandle *PhoneHandle, PhoneEntry *Entry);
PhoneEntry *LockActiveEntry(PhonebookHandle *PhoneHandle);
VOID UnlockActiveEntry(PhonebookHandle *PhoneHandle);
PhoneEntry *ClonePhoneEntry(PhonebookHandle *PhoneHandle, PhoneEntry *Source);

/* PhoneGroup.c */
VOID DeletePhoneGroupNode(PhoneGroupNode *GroupNode);
VOID FreePhoneGroupList(struct List *List);
VOID InsertSorted(struct List *List, struct Node *Node);
PhoneGroupNode *CreatePhoneGroup(PhonebookHandle *PhoneHandle, STRPTR Name);
VOID RemoveGroupEntry(PhoneEntry *Entry);
VOID DeleteGroupEntry(PhoneEntry *Entry);
PhoneNode *AddGroupEntry(PhoneGroupNode *NewGroup, PhoneEntry *Entry);
VOID SortGroupList(struct List *List, LONG How, BOOL ReverseOrder);

/* PhoneList.c */
struct List *CreateRegularPhoneList(PhonebookHandle *PhoneHandle);
PhoneNode *CreatePhoneNode(PhoneEntry *Entry, BOOL InitName);
VOID UpdatePhoneNode(PhoneNode *Node);
VOID UpdatePhoneList(PhonebookHandle *PhoneHandle);
BOOL InitPhoneList(PhonebookHandle *PhoneHandle);
VOID DeletePhoneList(PhonebookHandle *PhoneHandle);
VOID SortPhoneList(PhonebookHandle *PhoneHandle, LONG How, BOOL ReverseOrder);

/* PhonePanel.c */
BOOL PhonePanel(ULONG InitialQualifier);

/* PhonePanelPlus.c */
BOOL SaveChanges(struct Window *Parent);
BOOL EditConfig(struct Configuration *Config, LONG Type, ULONG Qualifier, struct Window *Window);
BOOL ChangeState(LONG Type, LONG Default, struct PhoneNode *Node);
STRPTR *BuildLabels(PhonebookHandle *PhoneHandle);
VOID FindGroup(struct List *List, LONG *GroupIndex, LONG *NodeIndex, PhoneNode *Wanted);

/* PhonePanelUI.c */
LayoutHandle *CreateEditorHandle(PhoneListContext *Context, BOOL Activate);
LayoutHandle *CreateManagerHandle(PhoneListContext *Context, struct Window *Parent);
LayoutHandle *CreateSelectorHandle(struct Window *Parent, STRPTR Name, STRPTR Number, STRPTR Comment, BYTE *Mode);
LayoutHandle *CreateGroupHandle(struct Window *Parent, STRPTR *Labels, ULONG Group);
LayoutHandle *CreateSortHandle(struct Window *Parent, WORD *Criteria, BOOL *Reverse);
VOID UpdatePasswordWindow(LayoutHandle *Handle);
BOOL GetWindowPassword(LayoutHandle *Handle, STRPTR Password);
LayoutHandle *CreatePasswordHandle(struct Window *Parent, BOOL PasswordEnabled);

/* PickDisplayMode.c */
BOOL PickDisplayMode(struct Window *Parent, ULONG *CurrentMode, struct Configuration *Config);

/* PickFile.c */
BOOL ValidateFile(STRPTR FileName, LONG Type, STRPTR RealName);
BOOL PickFile(struct Window *Window, STRPTR Directory, STRPTR Pattern, STRPTR Prompt, STRPTR Name, LONG Type);

/* PickScreen.c */
BOOL PickScreen(struct Window *Window, STRPTR Name);

/* Print.c */
BOOL PrintText(BPTR File, struct Window *ReqWindow, LONG *Error, STRPTR String, ...);
BOOL PrintFileInformation(BPTR File, struct Window *ReqWindow, LONG *Error, STRPTR Name, ULONG Flags);
BOOL PrintEntry(BPTR File, struct Window *ReqWindow, BOOL Plain, LONG *Error, struct PhoneEntry *Entry, ULONG Flags);
BOOL PrintScreen(BPTR File, struct Window *ReqWindow, LONG *Error);
BOOL PrintClip(BPTR File, struct Window *ReqWindow, LONG *Error);
BOOL PrintBuffer(BPTR File, struct Window *ReqWindow, LONG *Error);
VOID PrintSomething(LONG Source);
VOID PrintRegion(LONG Top, LONG Bottom, BOOL FormFeed);
BOOL PrintScreenGfx(VOID);

/* PrintPanel.c */
VOID PrintPanel(struct Window *Window, struct List *PhoneList, LONG Count);

/* Raster.c */
VOID DeleteRaster(VOID);
BOOL CreateRaster(VOID);
VOID RasterEraseScreen(LONG Mode);
VOID RasterEraseLine(LONG Mode);
VOID RasterEraseCharacters(LONG Chars);
VOID RasterClearLine(LONG Lines, LONG Top);
VOID RasterInsertLine(LONG Lines, LONG Top);
VOID RasterScrollRegion(LONG Direction, LONG RasterTop, LONG RasterBottom, LONG RasterLines);
VOID RasterShiftChar(LONG Size);
VOID RasterPutString(STRPTR String, LONG Length);
VOID SaveRasterDummy(LONG UnusedFirst, LONG UnusedLast);
VOID SaveRasterReal(LONG First, LONG Last);

/* RatePanel.c */
BOOL RatePanel(struct Window *Parent, PhonebookHandle *PhoneHandle, PhoneEntry *Entry, struct List *TimeDateList);

/* Remember.c */
VOID FinishRecord(VOID);
BOOL CreateRecord(STRPTR BBSName);
VOID DeleteRecord(VOID);
VOID RememberResetOutput(VOID);
VOID RememberOutputText(STRPTR String, LONG Size);
VOID RememberResetInput(VOID);
VOID RememberInputText(STRPTR String, LONG Size);
VOID RememberSpill(VOID);

/* Rendezvous.c */
struct RendezvousData *SAVE_DS ASM RendezvousLogin(REG(a0) struct MsgPort *ReadPort, REG(a1) struct MsgPort *WritePort, REG(a2) struct TagItem *UnusedTagList);
VOID SAVE_DS ASM RendezvousLogoff(REG(a0) struct RendezvousData *Data);
struct Node *SAVE_DS ASM RendezvousNewNode(REG(a0) STRPTR Name);
VOID RendezvousSetup(VOID);
VOID AddRendezvousSemaphore(STRPTR Name);
VOID RemoveRendezvousSemaphore(VOID);

/* SaveWindow.c */
BOOL SaveWindow(STRPTR Name, struct Window *Window);

/* Scale.c */
VOID DeleteScale(VOID);
BOOL CreateScale(struct Window *Parent);
VOID PrintScaled(STRPTR Buffer, LONG Size, LONG Scale);

/* ScreenPanel.c */
BOOL ScreenPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* Scroll.c */
VOID ScrollLineRectFill(struct RastPort *RPort, LONG MinX, LONG MinY, LONG MaxX, LONG MaxY);
VOID ScrollLineRectFillNoTabChange(struct RastPort *RPort, LONG MinX, LONG MinY, LONG MaxX, LONG MaxY);
VOID ScrollLineRaster(struct RastPort *RPort, LONG DeltaX, LONG DeltaY, LONG MinX, LONG MinY, LONG MaxX, LONG MaxY, BOOL Smooth);
VOID ScrollLineRasterNoTabChange(struct RastPort *RPort, LONG DeltaX, LONG DeltaY, LONG MinX, LONG MinY, LONG MaxX, LONG MaxY, BOOL Smooth);
VOID ScrollLineEraseScreen(LONG Mode);
VOID ScrollLineEraseLine(LONG Mode);
VOID ScrollLineEraseCharacters(LONG Chars);
VOID ScrollLineShiftChar(LONG Size);
VOID ScrollLinePutString(LONG Length);

/* SendText.c */
BOOL MatchPrompt(STRPTR Data, LONG Size, STRPTR Prompt, LONG PromptLen);
BOOL SendLinePrompt(STRPTR Line, LONG Len);
BOOL SendLineSimple(STRPTR Line, LONG Len);
BOOL SendLineDial(STRPTR Line, LONG Len);
BOOL SendLineDelay(STRPTR Line, LONG Len);
BOOL SendLineEcho(STRPTR Line, LONG Len);
BOOL SendLineAnyEcho(STRPTR Line, LONG Len);
BOOL SendLineKeyDelay(STRPTR Line, LONG Len);
SENDLINE ChangeSendLine(SENDLINE NewSendLine);
VOID RestoreSendLine(SENDLINE ChangedSendLine, SENDLINE OriginalSendLine);
VOID SendSetup(VOID);

/* Serial.c */
VOID SendBreak(VOID);
VOID HangUp(VOID);
VOID CopyWriteFlags(VOID);
VOID SerWriteVerbatim(APTR Buffer, LONG Size, BOOL Echo);
VOID SerWriteUpdate(VOID);
VOID SerWrite(APTR Buffer, LONG Size);
VOID RestartSerial(VOID);
VOID ClearSerial(VOID);
VOID DeleteSerial(VOID);
STRPTR GetSerialError(LONG Error, BOOL *ResetPtr);
BOOL CreateSerial(STRPTR ErrorBuffer, LONG ErrorBufferSize);
LONG ReconfigureSerial(struct Window *Window, struct SerialSettings *SerialConfig);
VOID ReopenSerial(VOID);
BOOL HandleSerialJob(JobNode *UnusedJob);
VOID UpdateSerialJob(VOID);
BOOL HandleSerialCacheJob(JobNode *UnusedJob);
ULONG SerialStdWaitForData(ULONG OtherBits);
ULONG SerialCacheWaitForData(ULONG OtherBits);
ULONG SerialStdGetWaiting(VOID);
ULONG SerialCacheGetWaiting(VOID);
LONG SerialStdRead(UBYTE *Buffer, LONG Len);
LONG SerialCacheRead(UBYTE *Buffer, LONG Size);

/* SerialIO.c */
VOID ResetSerialRead(VOID);
BOOL CheckSerialRead(VOID);
BYTE WaitSerialRead(VOID);
BYTE FlushSerialRead(VOID);
BYTE DoSerialBreak(VOID);
VOID CompletelyFlushSerialRead(VOID);
VOID StopSerialRead(VOID);
VOID StartSerialRead(APTR Data, ULONG Length);
BYTE DoSerialRead(APTR Data, ULONG Length);
VOID ResetSerialWrite(VOID);
BYTE WaitSerialWrite(VOID);
VOID StopSerialWrite(VOID);
VOID StartSerialWrite(APTR Data, ULONG Length);
BYTE DoSerialWrite(APTR Data, ULONG Length);
BYTE DoSerialCmd(LONG Command);
ULONG GetSerialWaiting(VOID);
LONG GetSerialStatus(VOID);
VOID GetSerialInfo(ULONG *Waiting, UWORD *Status);
BYTE SetBothSerialAttributes(Tag FirstTag, ...);
BYTE SetSerialReadAttributes(Tag FirstTag, ...);
ULONG GetSerialWriteAttributes(Tag FirstTag, ...);
BOOL DropDTR(STRPTR Device, LONG Unit);
BYTE OpenSerialDevice(STRPTR Device, LONG Unit);

/* SerialPanel.c */
BOOL SerialPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* SortCompare.c */
SORTFUNC GetSortFunc(LONG How, BOOL ReverseOrder);

/* Sound.c */
VOID FreeSound(struct SoundInfo *SoundInfo);
struct SoundInfo *LoadSound(STRPTR Name, BOOL Warn);
VOID PlaySound(struct SoundInfo *SoundInfo);
VOID SoundExit(VOID);
VOID SoundInit(VOID);
VOID SoundPlay(LONG Sound);

/* SoundPanel.c */
BOOL SoundPanelConfig(struct SoundConfig *SoundConfig, STRPTR LastSound, BOOL *ChangedPtr);

/* Speech.c */
VOID DeleteSpeech(VOID);
BOOL CreateSpeech(VOID);
VOID Say(STRPTR String, ...);
VOID SpeechSetup(VOID);

/* SpeechPanel.c */
BOOL SpeechPanelConfig(struct SpeechConfig *SpeechConfig, STRPTR LastSpeech, BOOL *ChangePtr);

/* Start.c */
LONG SAVE_DS STACKARGS Start(VOID);

/* StatusDisplay.c */
VOID SAVE_DS StatusServer(VOID);
VOID ForceStatusUpdate(VOID);

/* TerminalPanel.c */
BOOL TerminalPanel(struct Window *Parent, struct Configuration *LocalConfig);

/* TerminalTest.c */
VOID StopTerminalTest(VOID);
VOID StartTerminalTest(ULONG Qualifier);
BOOL HandleTerminalTestJob(JobNode *UnusedJob);

/* TextBufferWindow.c */
BOOL LaunchBuffer(VOID);
VOID TerminateBuffer(VOID);
VOID MoveBuffer(struct SignalSemaphore *Access, struct TextBufferInfo **Data, LONG Mode);
VOID NotifyBuffer(struct SignalSemaphore *Access, struct TextBufferInfo **Data, ULONG Signals);
VOID MoveReview(LONG Mode);
VOID DeleteReview(VOID);
BOOL CreateReview(VOID);

/* TimeDate.c */
VOID SetActivePattern(struct List *Pattern);
struct List *GetActivePattern(VOID);
VOID UnlockActivePattern(VOID);
struct List *LockActivePattern(VOID);
VOID FreeTimeDateNode(struct TimeDateNode *Node);
VOID FreeTimeDateList(struct List *List);
BOOL CopyTimeDateList(struct List *From, struct List *To, BOOL SkipFirst);
VOID AdaptTimeDateNode(struct TimeDateNode *Node);
VOID SortTimeTable(struct TimeDateNode *Node);
struct List *BuildTimeList(struct TimeDateNode *Node);
BOOL ResizeTimeDateNode(struct TimeDateNode *Node, LONG Count, UBYTE Time);
BOOL DeleteTimeDateNode(struct TimeDateNode *Node, LONG Index);
struct TimeDateNode *CreateTimeDateNode(LONG Month, LONG Day, STRPTR Comment, LONG Count);
VOID SelectTime(struct PhoneEntry *SomeEntry, struct List *List, struct timeval *TimeVal);
struct List *FindTimeDate(struct List *Patterns, STRPTR Number);
VOID DeletePatternNode(struct PatternNode *Pattern);
struct PatternNode *CreatePatternNode(STRPTR Comment);
VOID DeletePatternList(struct List *List);
VOID FreePatternList(struct List *List);
VOID ConvertTimeDate(struct TimeDateOld *Old, struct TimeDate *New);
struct List *LoadTimeDateList(STRPTR Name, LONG *Error);
BOOL SaveTimeDateList(STRPTR Name, struct List *List, LONG *Error);

/* TimePanel.c */
BOOL TimePanel(struct Window *Parent, UBYTE *StartTime);

/* Tools.c */
VOID ListViewStateFill(struct LVDrawMsg *Msg);
VOID EraseWindow(struct Window *Window, UWORD *Pens);
LONG GetListMaxPen(UWORD *Pens);
VOID FillBox(struct RastPort *RPort, LONG Left, LONG Top, LONG Width, LONG Height);
VOID FillWindowBox(struct Window *Window, LONG Left, LONG Top, LONG Width, LONG Height);
VOID PlaceText(struct RastPort *RPort, LONG Left, LONG Top, STRPTR String, LONG Len);
VOID SetPens(struct RastPort *RPort, ULONG APen, ULONG BPen, ULONG DrMd);
LONG Atol(STRPTR Buffer);
VOID StripSpaces(STRPTR String);
VOID ReplaceWindowInfo(struct WindowInfo *NewInfo);
VOID PutWindowInfo(LONG ID, LONG Left, LONG Top, LONG Width, LONG Height);
VOID GetWindowInfo(LONG ID, LONG *Left, LONG *Top, LONG *Width, LONG *Height, LONG DefWidth, LONG DefHeight);
LONG GetBitMapDepth(struct BitMap *BitMap);
VOID GetDPI(ULONG Mode, ULONG *X_DPI, ULONG *Y_DPI);
VOID AddProtection(STRPTR Name, ULONG Mask);
BOOL GetPubScreenName(struct Screen *Screen, STRPTR Name);
VOID InitSinglePort(struct MsgPort *Port);
BOOL GoodStream(BPTR Stream);
struct DiskObject *GetProgramIcon(VOID);
LONG GetPenIndex(LONG Pen);
LONG GetScreenWidth(struct Window *Window);
LONG GetScreenHeight(struct Window *Window);
LONG GetScreenLeft(struct Window *Window);
ULONG OldGetAPen(struct RastPort *RPort);
ULONG OldGetBPen(struct RastPort *RPort);
ULONG OldGetDrMd(struct RastPort *RPort);
ULONG OldSetWrMsk(struct RastPort *RPort, ULONG Mask);
ULONG OldGetWrMsk(struct RastPort *RPort);
ULONG NewGetAPen(struct RastPort *RPort);
ULONG NewGetBPen(struct RastPort *RPort);
ULONG NewGetDrMd(struct RastPort *RPort);
ULONG NewSetWrMsk(struct RastPort *RPort, ULONG Mask);
ULONG NewGetWrMsk(struct RastPort *RPort);
VOID SetWait(struct Window *Window);
VOID ClrWait(struct Window *Window);
VOID GetModeName(ULONG Mode, STRPTR Buffer, LONG BufferSize);
BOOL ModeOkay(ULONG ID);
STRPTR ExtractString(STRPTR String, STRPTR Destination, BOOL ReturnEnd);
struct Node *RemoveGetNext(struct Node *Node);
LONG GetListSize(struct List *List);
struct Node *GetListNode(LONG Offset, struct List *List);
struct Node *CreateNode(STRPTR Name);
struct Node *CreateNodeArgs(STRPTR Format, ...);
VOID FormatAppend(STRPTR String, STRPTR Format, ...);
VOID FreeList(struct List *List);
LONG GetNodeOffset(struct Node *ListNode, struct List *List);
VOID MoveList(struct List *From, struct List *To);
struct List *CreateList(VOID);
VOID DeleteList(struct List *List);
VOID MoveNode(struct List *List, struct Node *Node, LONG How);
VOID LogAction(STRPTR String, ...);
BOOL GetString(BOOL LoadGadget, BOOL Password, LONG MaxChars, STRPTR Prompt, STRPTR Buffer);
VOID WakeUp(struct Window *Window, LONG Sound);
VOID BlockWindows(VOID);
VOID ReleaseWindows(VOID);
LONG LineRead(BPTR File, STRPTR Buffer, LONG MaxChars);
LONG GetBaudRate(STRPTR Buffer);
LONG GetFileSize(STRPTR Name);
VOID PutDimensionTags(struct Window *Reference, LONG Left, LONG Top, LONG Width, LONG Height);
struct TagItem *GetDimensionTags(struct Window *Reference, struct TagItem *Tags);
LONG ShowRequest(struct Window *Window, STRPTR Text, STRPTR Gadgets, ...);
VOID CloseWindowSafely(struct Window *Window);
VOID DelayTime(LONG Secs, LONG Micros);
VOID WaitTime(VOID);
VOID StopTime(VOID);
VOID StartTime(LONG Secs, LONG Micros);
BOOL GetEnvDOS(STRPTR Name, STRPTR Buffer, LONG BufferSize);
BOOL SetEnvDOS(STRPTR Name, STRPTR Value);
VOID BumpWindow(struct Window *SomeWindow);
VOID PushWindow(struct Window *Window);
VOID PopWindow(VOID);
VOID SplitFileName(STRPTR FullName, STRPTR *FileName, STRPTR DrawerName);
VOID SAVE_DS ASM BackfillRoutine(REG(a0) struct Hook *UnusedHook, REG(a2) struct RastPort *RPort, REG(a1) LayerMsg *Bounds);
UBYTE *ShrinkName(UBYTE *Source, UBYTE *Destination, LONG MaxLength, BOOL FixSuffix);
VOID BuildFontName(STRPTR Destination, LONG DestinationSize, STRPTR Name, LONG Size);
VOID FixName(STRPTR Name);
VOID ShowError(struct Window *Window, LONG Primary, LONG Secondary, STRPTR String);
struct List *BuildModeList(LONG *Index, ULONG DisplayMode, MODEFILTER ModeFilter, APTR UserData);
BOOL IsAssign(STRPTR Name);
BOOL LockInAssign(BPTR TheLock, STRPTR TheAssignment);
VOID DeleteBitMap(struct BitMap *BitMap);
struct BitMap *CreateBitMap(ULONG Width, ULONG Height, ULONG Depth, ULONG Flags, struct BitMap *Friend);
BOOL LaunchRexxAsync(STRPTR Command);
BOOL LaunchCommand(STRPTR Command);
BOOL LaunchCommandAsync(STRPTR Command);
struct Process *LaunchProcess(STRPTR Name, VOID (*Entry )(VOID ), BPTR Stream);
BOOL String2Boolean(STRPTR String);
VOID SendMessageGetReply(struct MsgPort *Port, struct Message *Message);
VOID SetOnlineState(BOOL IsOnline);
VOID SwapMem(APTR FromPtr, APTR ToPtr, LONG Size);
BOOL CloseIFFClip(struct IFFHandle *Handle);
struct IFFHandle *OpenIFFClip(LONG Unit, LONG Mode);
BOOL CloseIFFStream(struct IFFHandle *Handle);
struct IFFHandle *OpenIFFStream(STRPTR Name, LONG Mode);
VOID InitHook(struct Hook *Hook, HOOKFUNC Func, APTR Data);
ULONG FitText(struct RastPort *RPort, UWORD Width, STRPTR String, LONG Len);
VOID DispatchRexxDialMsgList(BOOL Ok);
VOID CancelZModem(VOID);
VOID WaitForHandshake(VOID);
VOID ShakeHands(struct Task *Notify, ULONG NotifyMask);
struct ViewPortExtra *GetViewPortExtra(struct ViewPort *ViewPort);
BPTR OpenToAppend(STRPTR Name, BOOL *Created);
VOID PushStatus(WORD NewStatus);
VOID PopStatus(VOID);
WORD GetStatus(VOID);
VOID Clear_xOFF(VOID);
VOID Set_xOFF(VOID);
BOOL Get_xOFF(VOID);
VOID Lock_xOFF(VOID);
VOID Unlock_xOFF(VOID);
VOID MoveListViewNode(LayoutHandle *Handle, struct List *List, LONG ListID, struct Node *Node, LONG *Offset, LONG How);
struct Process *StartProcessWaitForHandshake(STRPTR Name, TASKENTRY Entry, ...);
BOOL LocalGetCurrentDirName(STRPTR Buffer, LONG BufferSize);
VOID SafeObtainSemaphoreShared(struct SignalSemaphore *Semaphore);
struct Task *LocalCreateTask(STRPTR Name, LONG Priority, TASKENTRY Entry, ULONG StackSize, LONG NumArgs, ...);
VOID ChangeWindowPtr(APTR *Old, APTR New);
VOID RestoreWindowPtr(APTR Old);
VOID DisableDOSRequesters(APTR *WindowPtr);
VOID EnableDOSRequesters(APTR WindowPtr);
BOOL SpeechSynthesizerAvailable(VOID);
VOID CurrentTimeToStamp(struct DateStamp *Stamp);
struct TextFont *SmartOpenDiskFont(struct TextAttr *TextAttr);
VOID ArmLimit(ULONG SecondsToGo);
VOID DisarmLimit(VOID);
BOOL CheckLimit(VOID);

/* Transfer.c */
VOID FreeFileTransferInfo(struct FileTransferInfo *Info);
struct FileTransferInfo *AllocFileTransferInfo(VOID);
BOOL AddFileTransferNode(struct FileTransferInfo *Info, STRPTR Name, ULONG Size);
VOID ReadyFileTransferInfo(struct FileTransferInfo *Info, BOOL SortIt);
struct FileTransferInfo *BuildFileTransferInfo(struct FileRequester *FileRequester);
VOID StartReceiveXPR_File(LONG Type, STRPTR Name, BOOL WaitForIt);
VOID StartReceiveXPR_AskForFile(LONG Type, BOOL WaitForIt);
BOOL StartSendXPR_File(LONG Type, STRPTR TheFile, BOOL WaitForIt);
BOOL StartSendXPR_AskForFile(LONG Type, BOOL WaitForIt);
BOOL StartSendXPR_FromList(LONG Type, BOOL WaitForIt);
VOID ResetProtocol(VOID);
VOID SaveProtocolOpts(VOID);
VOID TransferCleanup(VOID);
VOID RemoveUploadListItem(STRPTR Name);
VOID CloseXPR(VOID);
BOOL ChangeProtocol(STRPTR ProtocolName, LONG Type);
BOOL ProtocolSetup(BOOL IgnoreOptions);

/* TransferPanel.c */
VOID AddTransferInfo(BOOL Error, STRPTR Message, ...);
BOOL TransferPanel(STRPTR Title);
VOID DeleteTransferPanel(BOOL WaitForPrompt);

/* Translate.c */
VOID TranslateSetup(struct TranslationHandle *Handle, STRPTR SourceBuffer, LONG SourceLen, STRPTR DestinationBuffer, LONG DestinationLen, struct TranslationEntry **Table);
LONG TranslateBuffer(struct TranslationHandle *Handle);
UBYTE NameToCode(STRPTR Name);
STRPTR CodeToName(UBYTE Code);
VOID FreeTranslationTable(struct TranslationEntry **Table);
struct TranslationEntry **AllocTranslationTable(VOID);
VOID FreeTranslationEntry(struct TranslationEntry *Entry);
LONG TranslateString(STRPTR From, STRPTR To);
struct TranslationEntry *AllocTranslationEntry(STRPTR String);
BOOL FillTranslationTable(struct TranslationEntry **Table);
BOOL IsStandardTable(struct TranslationEntry **Table);
VOID TranslateBack(STRPTR From, LONG Len, STRPTR To, LONG ToLen);
BOOL SaveTranslationTables(STRPTR Name, struct TranslationEntry **SendTable, struct TranslationEntry **ReceiveTable);
BOOL LoadTranslationTables(STRPTR Name, struct TranslationEntry **SendTable, struct TranslationEntry **ReceiveTable);
BOOL LoadDefaultTranslationTables(STRPTR FileName);

/* TranslationPanel.c */
BOOL TranslationPanelConfig(struct Configuration *LocalConfig, struct TranslationEntry ***SendTablePtr, struct TranslationEntry ***ReceiveTablePtr, STRPTR LastTranslation, struct Window *Parent, BOOL *ChangePtr);

/* TrapPanel.c */
BOOL TrapPanelConfig(struct Configuration *LocalConfig, BOOL *ChangedPtr);

/* Traps.c */
struct TrapNode *ChangeTrapNode(struct GenericList *TrapList, struct TrapNode *Current, STRPTR Sequence, STRPTR Command);
struct TrapNode *CreateTrapNode(STRPTR SequenceBuffer, STRPTR CommandBuffer);
BOOL SaveTraps(STRPTR Name, struct List *TrapList);
BOOL LoadTraps(STRPTR Name, struct GenericList *TrapList);
VOID TrapFilter(STRPTR Data, LONG Size);

/* UploadPanel.c */
LONG UploadPanel(LONG SendMode);

/* UploadQueue.c */
VOID NotifyUploadQueue(struct GenericList *UnusedList, WORD Login);
VOID CloseQueueWindow(VOID);
VOID DeleteQueueProcess(VOID);
BOOL CreateQueueProcess(VOID);
VOID ToggleUploadQueueIcon(BOOL Mode);

/* Verify.c */
BOOL FindPath(struct Window *Parent, STRPTR Path, BOOL CanCreate, LONG *Error);
BOOL FindDrawer(struct Window *Parent, STRPTR Drawer, BOOL CanCreate, LONG *Error);
BOOL FindFile(struct Window *Parent, STRPTR File, LONG *Error);
BOOL FindProgram(struct Window *Parent, STRPTR Program, LONG *Error);
BOOL FindLibDev(struct Window *Parent, STRPTR File, LONG Type, LONG *Error);

/* VSPrintf.c */
#ifdef __AROS__
VOID ASM StuffChar(REG(d0) UBYTE Char, REG(a3) struct FormatContext *Context);
#else
VOID ASM StuffChar(REG(a3) struct FormatContext *Context, REG(d0) UBYTE Char);
#endif
VOID LimitedVSPrintf(LONG Size, STRPTR Buffer, STRPTR FormatString, va_list VarArgs);
VOID LimitedSPrintf(LONG Size, STRPTR Buffer, STRPTR FormatString, ...);
VOID VSPrintf(STRPTR Buffer, STRPTR FormatString, va_list VarArgs);
VOID SPrintf(STRPTR Buffer, STRPTR FormatString, ...);
#ifdef __AROS__
VOID ASM CountChar(REG(d0) UBYTE dummy,REG(a3) ULONG *Count);
#else
VOID ASM CountChar(REG(a3) ULONG *Count);
#endif
ULONG GetFormatLength(STRPTR FormatString, va_list VarArgs);
ULONG GetFormatLengthArgs(STRPTR FormatString, ...);
VOID LimitedStrcat(LONG Size, STRPTR To, STRPTR From);
VOID LimitedStrcpy(LONG Size, STRPTR To, STRPTR From);
VOID LimitedStrncpy(LONG Size, STRPTR To, STRPTR From, LONG FromSize);

/* WindowMarker.c */
VOID WindowMarkerStop(VOID);
VOID WindowMarkerSelectAll(VOID);
VOID WindowMarkWord(VOID);
VOID WindowMarkerStart(ULONG MsgQualifier);
VOID WindowMarkerInterrupt(VOID);
VOID WindowMarkerMoveMouse(VOID);
VOID WindowMarkerTransfer(ULONG MsgQualifier);

/* XEM.c */
LONG SAVE_DS xem_sflush(VOID);
LONG SAVE_DS xem_squery(VOID);
LONG SAVE_DS ASM xem_sread(REG(a0) APTR Buffer, REG(d0) LONG Size, REG(d1) ULONG Timeout);
ULONG SAVE_DS ASM xem_toptions(REG(d0) LONG NumOpts, REG(a0) struct xem_option **Opts);
LONG SAVE_DS ASM xem_swrite(REG(a0) STRPTR Buffer, REG(d0) LONG Size);
LONG SAVE_DS xem_sbreak(VOID);
VOID SAVE_DS xem_sstart(VOID);
LONG SAVE_DS xem_sstop(VOID);
LONG SAVE_DS ASM xem_tgets(REG(a0) STRPTR Prompt, REG(a1) STRPTR Buffer, REG(d0) ULONG Size);
VOID SAVE_DS ASM xem_tbeep(REG(d0) ULONG Times, REG(d1) ULONG UnusedDelay);
LONG SAVE_DS ASM xem_macrodispatch(REG(a0) struct XEmulatorMacroKey *XEM_MacroKey);
BOOL SetEmulatorOptions(LONG Mode);
VOID CloseEmulator(BOOL Exit);
BOOL OpenEmulator(STRPTR Name);
VOID SetupXEM_MacroKeys(struct MacroKeys *Keys);
VOID HandleXEM(VOID);
BOOL HandleXEMJob(JobNode *UnusedJob);

/* XPR.c */
LONG SAVE_DS ASM xpr_fopen(REG(a0) STRPTR FileName, REG(a1) STRPTR AccessMode);
LONG SAVE_DS ASM xpr_fclose(REG(a0) struct Buffer *File);
LONG SAVE_DS ASM xpr_fread(REG(a0) APTR Buffer, REG(d0) LONG Size, REG(d1) LONG Count, REG(a1) struct Buffer *File);
LONG SAVE_DS ASM xpr_fwrite(REG(a0) APTR Buffer, REG(d0) LONG Size, REG(d1) LONG Count, REG(a1) struct Buffer *File);
LONG SAVE_DS ASM xpr_fseek(REG(a0) struct Buffer *File, REG(d0) LONG Offset, REG(d1) LONG Origin);
LONG SAVE_DS ASM xpr_sread(REG(a0) APTR Buffer, REG(d0) ULONG Size, REG(d1) ULONG Timeout);
LONG SAVE_DS ASM xpr_swrite(REG(a0) APTR Buffer, REG(d0) LONG Size);
LONG SAVE_DS xpr_sflush(VOID);
LONG SAVE_DS ASM xpr_update(REG(a0) struct XPR_UPDATE *UpdateInfo);
LONG SAVE_DS xpr_chkabort(VOID);
LONG SAVE_DS ASM xpr_gets(REG(a0) STRPTR Prompt, REG(a1) STRPTR Buffer);
LONG SAVE_DS ASM xpr_setserial(REG(d0) LONG Status);
LONG SAVE_DS ASM xpr_ffirst(REG(a0) STRPTR Buffer, REG(a1) STRPTR UnusedPattern);
LONG SAVE_DS ASM xpr_fnext(REG(d0) LONG UnusedOldState, REG(a0) STRPTR Buffer, REG(a1) STRPTR UnusedPattern);
LONG SAVE_DS ASM xpr_finfo(REG(a0) STRPTR FileName, REG(d0) LONG InfoType);
ULONG SAVE_DS ASM xpr_options(REG(d0) LONG NumOpts, REG(a0) struct xpr_option **Opts);
LONG SAVE_DS ASM xpr_unlink(REG(a0) STRPTR FileName);
LONG SAVE_DS xpr_squery(VOID);
LONG SAVE_DS ASM xpr_getptr(REG(d0) LONG InfoType);
LONG SAVE_DS ASM xpr_stealopts(REG(a0) STRPTR UnusedPrompt, REG(a1) STRPTR Buffer);

/* zmodem.c */
LONG ZTransmit(struct WBArg *ArgList, LONG NumArgs, BOOL TextMode);
LONG ZReceive(VOID);
VOID InternalZModemTextUpload(VOID);
VOID InternalZModemBinaryUpload(VOID);
VOID ZSettings(VOID);

/**********************************************************************/

#endif /* _PROTOS_H */
