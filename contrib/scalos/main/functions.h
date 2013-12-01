// Functions.h
// $Date$
// $Revision$


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <exec/types.h>
#include <dos/datetime.h>
#include <intuition/intuition.h>
#include <datatypes/iconobject.h>
#include <scalos/menu.h>
#include <scalos/scalos.h>

#include "defs.h"
#include "scalos_structures.h"
#include "Wrappers.h"

#include <stdarg.h>
#include <stddef.h>

/* ------------------------------------------------- */

#ifndef min
#define	min(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b)	((a) > (b) ? (a) : (b))
#endif

/* ------------------------------------------------- */

// Debugging Macros

#include "debug.h"

/* ================================================= */
/* ================================================= */

/* ------------------------------------------------- */

// defined in amiga.lib

#ifndef __AROS__
extern ULONG HookEntry();
#endif

//----------------------------------------------------------------------------

/* ================================================= */
/* ================================================= */

// defined in About.c
///
void AboutProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
BOOL ReadScalosLogo(struct DatatypesImage **Logo);
///
/* ------------------------------------------------- */

// defined in AppMenu.c
///
void AppMenu_Init(void);
BOOL AppMenu_AddItem(struct AppMenuInfo *ami);
BOOL AppMenu_RemoveItem(struct AppMenuInfo *ami);
void AppMenu_DisposeAppMenuInfo(struct AppMenuInfo *ami);
void AppMenu_CreateMenu(void);
void AppMenu_ResetMenu(void);
struct ScaWindowStruct *FindActiveScalosWindow(void);
///
/* ------------------------------------------------- */

// defined in AutoUpdate.c
///
ULONG DeviceWindowCheckUpdate(struct internalScaWindowTask *iwt);
ULONG IconWindowCheckUpdate(struct internalScaWindowTask *iwt);
ULONG TextWindowCheckUpdate(struct internalScaWindowTask *iwt);
void RealUpdateIcon(struct internalScaWindowTask *iwt, struct UpdateIconData *arg);
///
/* ------------------------------------------------- */

// defined in Backdrop.c
///
void BackDropInitList(struct BackDropList *bdl);
LONG BackdropLoadList(struct BackDropList *bdl);
void BackdropFreeList(struct BackDropList *bdl);
void BackdropFilterList(struct BackDropList *bdl, BPTR dirLock);
BOOL BackdropAddLine(struct BackDropList *bdl, CONST_STRPTR NewLine, LONG PosX, LONG PosY);
BOOL BackdropRemoveLine(struct BackDropList *bdl, BPTR iconLock);
BOOL RewriteBackdrop(struct ScaIconNode *in);
ULONG AdjustBackdropRenamed(BPTR oLock, struct ScaIconNode *in);
struct ScaIconNode *AddBackdropIcon(BPTR iconDirLock, CONST_STRPTR iconName, WORD PosX, WORD PosY);
struct ScaIconNode *FindBackdropIcon(BPTR dirLock, CONST_STRPTR iconName);
struct ScaIconNode *FindBackdropIconExclusive(BPTR dirLock, CONST_STRPTR iconName);
void DoLeaveOutIcon(struct internalScaWindowTask *iwt, BPTR DirLock,
	CONST_STRPTR IconName, WORD x, WORD y);
BOOL IsPermanentBackDropIcon(struct internalScaWindowTask *iwt, struct BackDropList *bdl,
	BPTR fLock, CONST_STRPTR FileName);
void PutAwayIcon(struct internalScaWindowTask *iwt,
	BPTR iconDirLock, CONST_STRPTR IconName, BOOL RemovePermanent);
BOOL BackdropWait(BPTR dirLock);
///
/* ------------------------------------------------- */

// defined in BackFill.c
///
SAVEDS(ULONG) BackFillHookFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg);
void WindowBackFill(struct RastPort *rp,
	struct BackFillMsg *msg, struct BitMap *bitmap,
	LONG bmWidth, LONG bmHeight, WORD BGPen,
	LONG XOffset, LONG YOffset, APTR MaskPlane);
///
/* ------------------------------------------------- */

// defined in ButtonClassGadget.c
///
struct ScalosClass *initButtonGadgetClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in ChildProcess.c
///
// NumLongs : number of longwords in ArgArray (!!not number of entries!!)
BOOL RunProcess(struct ScaWindowTask *iwt, RUNPROCFUNC Routine, ULONG NumLongs,
	struct WBArg *ArgArray, struct MsgPort *ReplyPort);
BOOL ChildProcessRun(struct internalScaWindowTask *iwt, struct ScalosMessage *msg, ULONG FirstTag, ...);
///
/* ------------------------------------------------- */

// defined in Class.c
///
ULONG ClassDragQuery(struct DragEnter *drge, struct internalScaWindowTask *swt);
ULONG ClassDragEnter(struct DragEnter *drge, struct internalScaWindowTask *swt);
ULONG ClassDragLeave(struct DragEnter *drge, struct internalScaWindowTask *swt);
void ClassHideDragBobs(struct internalScaWindowTask *iwt, struct DragHandle *dh);
void ClassSelectIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon, BOOL Selected);
void ClassSelectIconObj(struct ScaWindowStruct *swi, Object *IconObj, BOOL Selected);
ULONG ClassCheckInfoData(const struct InfoData *info);
BOOL ClassIsDiskWritable(BPTR dLock);
ULONG ClassWinTimerMsg(struct internalScaWindowTask *, struct Message *, APTR);
void ClassTimerToolTipMsg(struct internalScaWindowTask *iwt);
Object *ClassGetWindowIconObject(struct internalScaWindowTask *iwt, Object **allocIconObj);
void ClassFormatDate(struct DateTime *dt, ULONG DateMaxLen, ULONG TimeMaxLen);		  // +jl+ 20010518
ULONG ClassCountSelectedIcons(struct internalScaWindowTask *iwt);
void ClassDragFinish_IconWin(struct ScalosArg **arglist);
void ClassDragBegin_DeviceWin(struct ScalosArg **ArgList, struct DragNode **dnList);
void ClassDragBegin_IconWin(struct ScalosArg **ArgList, struct DragNode **dn);
void ClassSetDefaultIconFlags(struct ScaIconNode *in, BOOL IsDefIcon);
///
/* ------------------------------------------------- */

// defined in cleanup.c
///
void IconWindow_UnCleanup(struct internalScaWindowTask *iwt, struct Region *UnCleanUpRegion);
extern void IconWindow_Cleanup(struct internalScaWindowTask *iwt);
///
/* ------------------------------------------------- */

// defined in CLIStart.c
///
BOOL CLIStart(BPTR dirLock, CONST_STRPTR PrgName, Object *iconObject, ULONG def_StackSize);
BPTR DupWBPathList(void);
///
/* ------------------------------------------------- */

// defined in ControlBar.c
///
BOOL ControlBarAdd(struct internalScaWindowTask *iwt);
void ControlBarRemove(struct internalScaWindowTask *iwt);
void ControlBarRebuild(struct internalScaWindowTask *iwt);
void ControlBarUpdateViewMode(struct internalScaWindowTask *iwt, UBYTE ViewByType);
UBYTE ControlBarViewModeFromCode(ULONG Code);
void ControlBarUpdateShowMode(struct internalScaWindowTask *iwt);
void ControlBarUpdateHistory(struct internalScaWindowTask *iwt);
void SetControlBarOnOff(struct internalScaWindowTask *iwt);
void ControlBarSwitchHistoryEntry(struct internalScaWindowTask *iwt);
struct ExtGadget *ControlBarFindGadget(struct internalScaWindowTask *iwt,
	ULONG GadgetID);
struct ControlBarGadgetEntry *ControlBarFindGadgetByID(struct internalScaWindowTask *iwt, UWORD GadgetID);
UWORD ControlBarQueryGadgetType(struct internalScaWindowTask *iwt, struct ExtGadget *gg);
void ControlBarActionButton(struct internalScaWindowTask *iwt, struct ExtGadget *gg);
///
/* ------------------------------------------------- */

// defined in crc32.c
///
ULONG update_crc(ULONG crc, const unsigned char *buf, size_t len);
///
/* ------------------------------------------------- */

// defined in CycleGadgetClass.c
///
struct ScalosClass *initCycleGadgetClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in DefIcons.c
///
LONG InitDefIcons(void);
void CleanupDefIcons(void);
void NewDefIconsPrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg);
BOOL ChangedDefIconsPrefs(void);
struct TypeNode *DefIconsIdentify(BPTR dirLock, CONST_STRPTR Name, ULONG IconType);
Object *ReturnDefIconObjTags(BPTR dirLock, CONST_STRPTR Name, ULONG FirstTag, ...);
Object *ReturnDefIconObj(BPTR dirLock, CONST_STRPTR Name, struct TagItem *TagList);
///
/* ------------------------------------------------- */

// defined in DeviceWindowClass.c
///
struct ScalosClass *initDeviceWindowClass(const struct PluginClass *plug);
void FreeBackdropIconList(struct ScaBackdropIcon **bdiList);
void RemoveScalosBackdropIcon(struct ScaIconNode *in);
void CreateSbiForIcon(struct ScaIconNode *in);
///
/* ------------------------------------------------- */

// defined in DevListClass.c
///
struct ScalosClass *initDevListClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in DoubleClick.c
///
BOOL IconDoubleClick(struct internalScaWindowTask *iwt, struct ScaIconNode *in, ULONG Flags);
BOOL ArexxToolStart(struct internalScaWindowTask *iwt, BPTR dirLock, CONST_STRPTR ProgName);
///
/* ------------------------------------------------- */

// defined in DragDrop.c
///
void DragDrop(struct Window *win, LONG MouseX, LONG MouseY, ULONG Qualifier,
	struct internalScaWindowTask *iwt);
ULONG ScalosDropAddIcon(BPTR DirLock, CONST_STRPTR IconName, WORD x, WORD y);
void ScalosDropRemoveIcon(BPTR DirLock, CONST_STRPTR IconName);
///
/* ------------------------------------------------- */

// defined in DragDropBobs.c
///
void InitDrag(struct IntuiMessage *iMsg, struct internalScaWindowTask *iwt);
void RestoreDragIcons(struct internalScaWindowTask *iwt);
BOOL SuspendDrag(struct DragHandle *dh, struct internalScaWindowTask *iwt);
void ResumeDrag(struct DragHandle *dh, struct internalScaWindowTask *iwt, BOOL wasLocked);
void ReLockDrag(struct DragHandle *dh, struct internalScaWindowTask *iwt, BOOL wasLocked);
LIBFUNC_P2_PROTO(void, sca_LockDrag,
	A0, struct DragHandle *, dh,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(ULONG, sca_UnlockDrag,
	A0, struct DragHandle *, dh,
	A6, struct ScalosBase *, ScalosBase);
void DrawDrag(ULONG DrawFlags, struct internalScaWindowTask *iwt);
LIBFUNC_P8_PROTO(BOOL, sca_AddBob,
	A0, struct DragHandle *, dh, 
	A1, struct BitMap *, bm, 
	A2, APTR, Mask, 
	D0, ULONG, Width, 
	D1, ULONG, Height, 
	D2, LONG, XOffset, 
	D3, LONG, YOffset,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P5_PROTO(void, sca_DrawDrag,
	A0, struct DragHandle *, dh,
	D0, LONG, XOffset, 
	D1, LONG, YOffset,
	D2, ULONG, Flags,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(void, sca_EndDrag,
	A0, struct DragHandle *, dh,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(struct DragHandle *, sca_InitDrag,
	A0, struct Screen *, Scr,
	A6, struct ScalosBase *, ScalosBase);
void EndDrag(struct internalScaWindowTask *iwt);
void EndDragUnlock(struct internalScaWindowTask *iwt);
void DragRefreshIcons(struct internalScaWindowTask *iwt);
void BlitARGBMask(ULONG SrcWidth, ULONG SrcHeight,
		const struct ARGB *Src, LONG SrcLeft, LONG SrcTop,
		ULONG DestWidth, struct ARGB *Dest, LONG DestLeft, LONG DestTop,
		const struct BitMap *MaskBM, ULONG Trans);
void BlitARGB(ULONG SrcWidth, ULONG SrcHeight,
		const struct ARGB *Src, LONG SrcLeft, LONG SrcTop,
		ULONG DestWidth, struct ARGB *Dest, LONG DestLeft, LONG DestTop,
		ULONG Trans);
///
/* ------------------------------------------------- */

// defined in DrawIcon.c
///
void EraseIconObject(struct internalScaWindowTask *iwt, Object *IconObj);
void DrawIconObject(struct internalScaWindowTask *iwt, Object *IconObj, ULONG Flags);
void BlitARGBAlpha(struct RastPort *rp, const struct ARGBHeader *SrcH,
	ULONG DestLeft, ULONG DestTop,
	ULONG SrcLeft, ULONG SrcTop,
	ULONG Width, ULONG Height,
	struct ARGB *BufferBg);
void  ScalosBlurPixelArray(struct ARGB *Dest, const struct ARGB *Src,
	UWORD SizeX, UWORD SizeY, const UBYTE *Mask);
ULONG ScalosReadPixelArray(APTR DestRect, UWORD DestMod, struct RastPort *rp,
		UWORD SrcX, UWORD SrcY, UWORD SizeX, UWORD SizeY);
ULONG ScalosWritePixelArray(APTR SrcRect, UWORD SrcMod, struct RastPort *rp,
		UWORD DestX, UWORD DestY, UWORD SizeX, UWORD SizeY);
void RemoveIcons(struct internalScaWindowTask *iwt, struct ScaIconNode **);
BOOL IsIconObjVisible(const struct internalScaWindowTask *iwt, const Object *IconObj);
void DrawIconObjectTransparent(struct internalScaWindowTask *iwt, Object *IconObj,
	ULONG ScreenDepth, struct RastPort *rp, struct RastPort *rpMask,
	ULONG transparency, BOOL RealTransparency);
void DrawIconObjectK(struct internalScaWindowTask *iwt, Object *IconObj,
	ULONG ScreenDepth, struct RastPort *rp, struct RastPort *rpMask,
	const struct ARGB *K);
///
/* ------------------------------------------------- */

// defined in DropMarks.c
///
void CleanupDropMarkInfoList(struct List *dmList);
void CleanupDropMarkInfo(struct DropMarkInfo *dmi);
void DisplayWindowDropMark(struct internalScaWindowTask *iwt);
void EraseWindowDropMark(struct internalScaWindowTask *iwt);
void RedrawWindowDropMark(struct internalScaWindowTask *iwt, struct Region *clipRegion);
void DisplayIconDropMark(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
void EraseIconDropMark(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
void DisplayGadgetClickMark(struct internalScaWindowTask *iwt, struct ExtGadget *gg);
void EraseGadgetClickMark(struct internalScaWindowTask *iwt, struct ExtGadget *gg);
///
/* ------------------------------------------------- */

// defined in dtimage.c
///
BOOL InitDataTypesImage(void);
void CleanupDataTypesImage(void);
void DisposeDatatypesImage(struct DatatypesImage **dti);
struct DatatypesImage *CreateDatatypesImage(CONST_STRPTR ImageName, ULONG Flags);
void FillBackground(struct RastPort *rp, struct DatatypesImage *dtImage,
	WORD MinX, WORD MinY, WORD MaxX, WORD MaxY,
	ULONG XStart, ULONG YStart);
void DtImageDraw(struct DatatypesImage *dti, struct RastPort *rp,
	LONG Left, LONG Top, LONG Width, LONG Height);
BOOL TempName(STRPTR Buffer, size_t MaxLen);
///
/* ------------------------------------------------- */

// defined in DtImageClass.c
///
Class *initDtImageClass(void);
///
/* ------------------------------------------------- */

// defined in FileCommands.c
///
LONG CreateLinkCommand(Class *cl, Object *o, BPTR SrcDirLock, BPTR DestDirLock,
	CONST_STRPTR SrcFileName, CONST_STRPTR DestFileName);
LONG MoveCommand(Class *cl, Object *o, BPTR SrcDirLock, BPTR DestDirLock, CONST_STRPTR FileName);
LONG CopyCommand(Class *cl, Object *o, BPTR SrcDirLock, BPTR DestDirLock,
	CONST_STRPTR SrcFileName, CONST_STRPTR DestFileName);
LONG DeleteCommand(Class *cl, Object *o, BPTR DirLock, CONST_STRPTR FileName);
LONG CountCommand(Class *cl, Object *o, BPTR parentLock, CONST_STRPTR Name, BOOL Recursive);
BOOL ExistsObject(BPTR DirLock, CONST_STRPTR Name);
///
/* ------------------------------------------------- */

// defined in FileTransClass.c
///
struct ScalosClass *initFileTransClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in FileTypes.c
///
BOOL FileTypeInit(void);
void FileTypeCleanup(void);
void FileTypeFlush(BOOL Final);
struct FileTypeDef *FindFileType(struct internalScaWindowTask *iwt, const struct ScaIconNode *in);
struct FileTypeDef *FindFileTypeForTypeNode(struct internalScaWindowTask *iwt, const struct TypeNode *tNode);
void ReleaseFileType(struct FileTypeDef *ftd);
void NewFileTypesPrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg);
BOOL ChangedFileTypesPrefs(struct MainTask *mt);
///
/* ------------------------------------------------- */

// defined in FontUtil.c
///
BOOL FontUtilInit(void);
WORD Scalos_TextLength(struct RastPort *rp, CONST_STRPTR string, WORD Length);
ULONG Scalos_TextFit(struct RastPort *rp, CONST_STRPTR string, WORD Length, 
	struct TextExtent *textExtent, struct TextExtent *constrainingExtent, 
	WORD strDirection, UWORD constrainingBitWidth, UWORD constrainingBitHeight);
void Scalos_TextExtent(struct RastPort *rp, CONST_STRPTR string, 
	WORD Length, struct TextExtent *tExt);
void Scalos_Text(struct RastPort *rp, CONST_STRPTR string, WORD Length);
UWORD Scalos_GetFontHeight(struct RastPort *rp);
UWORD Scalos_GetFontBaseline(struct RastPort *rp);
ULONG Scalos_SetSoftStyle(struct RastPort *rp, ULONG style, ULONG enable, struct TTFontFamily *ttff);
void Scalos_SetFontTransparency(struct RastPort *rp, ULONG Transparency);
void Scalos_SetFont(struct RastPort *rp, struct TextFont *tf, struct TTFontFamily *ttFont);
ULONG Scalos_SetSoftStyle(struct RastPort *rp, ULONG style, ULONG enable, struct TTFontFamily *ttff);
APTR Scalos_OpenTTFont(CONST_STRPTR FontDesc, struct TTFontFamily *ttff);
void Scalos_CloseFont(struct TextFont **tf, struct TTFontFamily *ttFont);
void Scalos_InitRastPort(struct RastPort *rp);
void Scalos_DoneRastPort(struct RastPort *rp);
///
/* ------------------------------------------------- */

// defined in FrameImageClass.c
///
struct ScalosClass *initFrameImageClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in Functions.c
///
LIBFUNC_P2_PROTO(BOOL, sca_OpenIconWindow,
	A0, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase);
BPTR DiskInfoLock(const struct ScaIconNode *in);
STRPTR GetWsNameFromLock(BPTR WsLock);
Object *FunctionsFindIconObjectForPath(CONST_STRPTR Path, BOOL *WindowListLocked,
	struct ScaWindowStruct **wsIconListLocked, Object **allocIconObj);
void FunctionsGetSettingsFromIconObject(struct IconWindowProperties *iwp, Object *IconObj);
///
/* ------------------------------------------------- */

// defined in GadgetBarClass.c
///
struct ScalosClass *initGadgetBarClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in GadgetBarImageClass.c
///
struct ScalosClass *initGadgetBarImageClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in GadgetBarTextClass.c
///
struct ScalosClass *initGadgetBarTextClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in GaugeGadgetClass.c
///
struct ScalosClass *initGaugeGadgetClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in HistoryGadgetClass.c
///
struct ScalosClass *initHistoryGadgetClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in idcmp.c
///
ULONG IDCMPRefreshWindow(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
ULONG IDCMPDiskInserted(struct internalScaWindowTask *iwt, struct IntuiMessage *iMsg);
void AbortFunctions(struct internalScaWindowTask *iwt);
void RemoveDeviceIcon(struct internalScaWindowTask *iwt, struct ScaDeviceIcon *di,
	struct ScaIconNode **IconList);
///
/* ------------------------------------------------- */

// defined in IconifyClass.c
///
Class * initIconifyClass(void);
///
/* ------------------------------------------------- */

// defined in IconImageClass.c
///
Class *initIconImageClass(void);
BOOL freeIconImageClass(Class *cl);
///
/* ------------------------------------------------- */

// defined in IconWindowClass.c
///
struct ScalosClass *initIconWindowClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in IconWindow_MouseMove.c
///
void IDCMPDragMouseMove(struct internalScaWindowTask *iwt, struct IntuiMessage *msg);
void IconWin_EndDrag(struct internalScaWindowTask *iwt);
void IDCMPMMBMouseMove(struct internalScaWindowTask *iwt);
void IDCMPDragIntuiTicks(struct internalScaWindowTask *iwt, struct IntuiMessage *msg);
///
/* ------------------------------------------------- */

// defined in InputHandler.c
///
BOOL InitInputHandler(void);
void CleanupInputHandler(void);
BOOL PointInGadget(WORD x, WORD y, const struct Window *win, const struct Gadget *gad);
///
/* ------------------------------------------------- */

// defined in Lasso.c
///
void Lasso(WORD StartX, WORD StartY, ULONG Mode, struct internalScaWindowTask *iwt);
void BeginLasso(struct internalScaWindowTask *iwt, UWORD Qualifier);
void EndLasso(struct internalScaWindowTask *iwt);
///
/* ------------------------------------------------- */

// defined in LocaleStrings.c
///
CONST_STRPTR GetLocString(LONG StringID);
///
/* ------------------------------------------------- */

// defined in MainWait.c
///
void MainWait(struct MainTask *mainTask);
void RememberPrefsChanges(struct internalScaWindowTask *iwt, ULONG PrefsChangeFlags);
void StartMainTaskTimer(struct MainTask *mt);
struct NotifyNode *AddToMainNotifyList(struct NotifyTab *nft, ULONG Flags);
void RemFromMainNotifyList(struct NotifyNode *non);
///
/* ------------------------------------------------- */

// defined in Memory.c
///
BOOL MemoryInit(void);
void MemoryCleanup(void);
#ifndef DEBUG_MEMORY
APTR ScalosAlloc(ULONG Size);
APTR ScalosRealloc(APTR OldMem, ULONG NewSize);
void ScalosFree(APTR mem);
#endif /* DEBUG_MEMORY */
STRPTR AllocCopyString(CONST_STRPTR clp);
void FreeCopyString(STRPTR lp);
void *ScalosAllocNode(size_t size);
void ScalosFreeNode(void *node);
void *ScalosAllocMessage(size_t size);
void ScalosFreeMessage(void *node);
struct AnchorPath *ScalosAllocAnchorPath(ULONG Flags, size_t MaxPathLen);
void ScalosFreeAnchorPath(struct AnchorPath *ap);
struct InfoData *ScalosAllocInfoData(void);
void ScalosFreeInfoData(struct InfoData **pId);
STRPTR AllocPathBuffer(void);
void FreePathBuffer(STRPTR Buffer);
///
/* ------------------------------------------------- */

// defined in Menu.c
///
ULONG GetWindowMenuFlags(struct internalScaWindowTask *iwt);
ULONG ReadMenuPrefs(void);
void FreeMenuPrefs(void);
void ParseMenu(struct Menu *theMenu);
BOOL CompareCommand(CONST_STRPTR MenuComName, CONST_STRPTR CommandName);
void SetMenuOnOff(struct internalScaWindowTask *iwt);
void NewMenuPrefs(struct internalScaWindowTask *, struct NotifyMessage *);
BOOL ChangedMenuPrefs(struct MainTask *mt);
void CreateScalosMenu(void);
ULONG UpdateIconCount(struct internalScaWindowTask *iwt);
void ClearMainMenu(void);
///
/* ------------------------------------------------- */

// defined in MenuCommand.c
///
void WBInfoStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoEvent);
void GetIconsBoundingBox(struct internalScaWindowTask *iwt, struct Rectangle *BBox);
void DeleteProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
void UpdateAllProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
void ClearSelectionProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
BOOL ViewWindowBy(struct internalScaWindowTask *iwt, UBYTE NewViewByType);
void SetIconWindowRect(struct ScaIconNode *in);
///
/* ------------------------------------------------- */

// defined in Messages.c
///
ULONG CloseWindowMsg(struct internalScaWindowTask *iwt, struct Message *Msg, APTR p);
ULONG AsyncReplyMsg(struct internalScaWindowTask *iwt, 	struct Message *Msg, APTR p);
LONG SendAppMessage(struct AppObject *appo, ULONG AmClass, WORD x, WORD y);
BPTR LockScaModsDir(void);		/* *** DM0008 */
///
/* ------------------------------------------------- */

// defined in MouseIcon.c
///
struct ScaIconNode *CheckMouseIcon(struct ScaIconNode **IconList,
	struct internalScaWindowTask *iwt,
	LONG x, LONG y);
struct ScaIconNode *CheckMouseIconOuterBounds(struct ScaIconNode **IconList,
	LONG x, LONG y);
///
/* ------------------------------------------------- */

// defined in OpenDrawerByName.c
///
struct ScaWindowStruct *OpenDrawerByName(CONST_STRPTR Path, struct TagItem *TagList);
///
/* ------------------------------------------------- */

// defined in Patches.c
///
void SetAllPatches(struct MainTask *mt);
ULONG RemoveAllPatches(struct MainTask *mt);
BOOL PatchInit(void);
void PatchCleanup(void);
///
/* ------------------------------------------------- */

// defined in PopOpenWindows.c
///
void PopOpenWindow(struct internalScaWindowTask *iwt, struct DragHandle *dh);
void PopChildWindowDispose(struct internalScaWindowTask *iwt, struct ScaPopChildWindow *spcw);
void ClosePopupWindows(BOOL CloseAll);
///
/* ------------------------------------------------- */

// defined in PopupMenus.c
///
LONG TestPopup(struct internalScaWindowTask *iwt, WORD MouseX, WORD MouseY, UWORD Qualifier);
void ShowPopupMenu(struct internalScaWindowTask *iwt, struct msg_ShowPopupMenu *mpm);
///
/* ------------------------------------------------- */

// defined in Patterns.c
///
void RandomizePatterns(void);
void PatternsOff(struct MainTask *mt, struct MsgPort *ReplyPort);
void PatternsOn(struct MainTask *mt);
struct PatternNode *GetPatternNode(WORD PatternNo, const struct PatternNode *pNodeOld);
ULONG NewWindowPatternMsg(struct internalScaWindowTask *iwt, struct Message *Msg, APTR p);
void FreeBackFill(struct PatternInfo *);
BOOL SetBackFill(struct internalScaWindowTask *iwt, struct PatternNode *ptNode,
	struct PatternInfo *ptInfo, ULONG Flags, struct Screen *Scr);
void SetScreenBackfillHook(struct Hook *bfHook);
///
/* ------------------------------------------------- */

// defined in Prefs.c
///
ULONG ReadPalettePrefs(void);
void FreePalettePrefs(void);
void ReadFontPrefs(void);
void FreeFontPrefs(void);
LONG WriteWBConfig(void);
LONG ReadWBConfig(void);
void InitScalosPrefs(void);
BOOL ReadScalosPrefs(void);
void FreeScalosPrefs(void);
LONG ReadPatternPrefs(void);
void FreePatternPrefs(void);
ULONG RandomNumber(ULONG MaxRand);
void NewPatternPrefs(struct internalScaWindowTask *, struct NotifyMessage *);
BOOL ChangedPatternPrefs(struct MainTask *mt);
void NewMainPrefs(struct internalScaWindowTask *, struct NotifyMessage *);
BOOL ChangedMainPrefs(struct MainTask *mt);
void NewPalettePrefs(struct internalScaWindowTask *, struct NotifyMessage *);
BOOL ChangedPalettePrefs(struct MainTask *mt);
void NewFontPrefs(struct internalScaWindowTask *, struct NotifyMessage *);
BOOL ChangedFontPrefs(struct MainTask *mt);
ULONG GetPrefsCRCFromFH(BPTR fh);
ULONG GetPrefsCRCFromName(CONST_STRPTR FileName);
void OpenIconWindowFont(void);
void OpenTextWindowFont(void);
///
/* ------------------------------------------------- */

// defined in Rename.c
///
BOOL CollectRenameAdjustHistoryPaths(struct List *HList, BPTR fLock);
void AdjustRenameAdjustHistoryPaths(struct List *HList, BPTR fLock);
void CleanupRenameAdjustHistoryPaths(struct List *HList, BOOL WinListLocked);
///
/* ------------------------------------------------- */

// defined in Request.c
///
LONG UseRequest(struct Window *parentWin, ULONG BodyTextNum,
	ULONG GadgetsTextNum, APTR ArgList);
LONG UseRequestArgs(struct Window *parentWin,
	ULONG BodyTextNum, ULONG GadgetsTextNum, ULONG NumArgs, ...);
void UseAsyncRequestArgs(struct internalScaWindowTask *,
	LONG BodyTextNum, LONG GadgetTextNum, ULONG NumArgs, ...);
///
/* ------------------------------------------------- */

// defined in RootClass.c
///
Class * initRootClass(void);
void ProcessRootTimers(void);
///
/* ------------------------------------------------- */

// defined in ScaLibrary.c (is platform specific)
///
struct ScalosBase *sca_MakeLibrary(void);
///
/* ------------------------------------------------- */

// defined in Scalos.c
///
void SelectIcons(WORD StartX, WORD StartY, WORD StopX, WORD StopY, struct internalScaWindowTask *iwt);
void FreeIconList(struct internalScaWindowTask *iwt, struct ScaIconNode **IconList);
void FreeIconNode(struct internalScaWindowTask *iwt, struct ScaIconNode **IconList, struct ScaIconNode *in);
BOOL TestRegionNotEmpty(const struct Region *region);
void RefreshIconList(struct internalScaWindowTask *iwt, struct ScaIconNode *in, struct Region *DrawingRegion);
void RefreshIcons(struct internalScaWindowTask *iwt, struct Region *DrawingRegion);
ULONG ChipMemAttr(void);
LONG ScaSameLock(BPTR Lock1, BPTR Lock2);
void DisplayScreenTitleError(struct internalScaWindowTask *iwt, ULONG MsgId);
BOOL ExistsAssign(CONST_STRPTR AssignName);
LONG ScalosTagListInit(struct ScalosTagList *tagList);
void ScalosTagListCleanup(struct ScalosTagList *tagList);
LONG ScalosTagListNewEntry(struct ScalosTagList *tagList, ULONG tag, ULONG data);
void ScalosTagListEnd(struct ScalosTagList *tagList);
struct TagItem *ScalosVTagList(ULONG FirstTag, va_list args);
ULONG TranslateViewModesFromIcon(ULONG IconViewMode);
ULONG TranslateViewModesToIcon(ULONG ScalosIconViewMode);
ULONG TranslateScalosViewMode(ULONG ScalosViewMode);
BOOL IsIwtViewByIcon(struct internalScaWindowTask *iwt);
BOOL IsViewByIcon(ULONG ScalosViewMode);
BOOL IsShowAll(const struct ScaWindowStruct *ws);
BOOL IsShowAllType(UWORD ShowType);
void ScalosEndNotify(struct NotifyRequest *nr);
void SubtractDateStamp(struct DateStamp *from, const struct DateStamp *to);
Object *CloneIconObject(Object *OrigIconObj);

#if !defined(__SASC) && !defined(__MORPHOS__)
// SAS/C compatibility functions
extern char *stpblk(const char *);
extern size_t stccpy(char *, const char *, size_t);
extern void strins(char *to, const char *from);
#endif
///
/* ------------------------------------------------- */

// defined in Scalos_Cx.c
///
BOOL InitCx(void);
void CleanupCx(void);
struct Screen *SearchMouseScreen(LONG x, LONG y);
void HighlightIconUnderMouse(void);
void InvalidateLastIwtUnderPointer(struct internalScaWindowTask *iwt);
///
/* ------------------------------------------------- */

// defined in ScalosInit.c
///
SAVEDS(ULONG) LockScalosPens(void);
SAVEDS(void) UnlockScalosPens(void);
BOOL ChangedLocalePrefs(struct MainTask *mt);
///
/* ------------------------------------------------- */

// defined in ScalosLocale.c
///
void NewLocalePrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg);
BOOL ChangedLocalePrefs(struct MainTask *mt);
void InitLocale(void);
void CleanupLocale(void);
///
/* ------------------------------------------------- */

// defined in ScanDir.c
///
enum ScanDirResult ReadIconList(struct internalScaWindowTask *iwt);
struct ScaIconNode *IconWindowReadIcon(struct internalScaWindowTask *iwt, 
	CONST_STRPTR Name, struct ScaReadIconArg *ria);
void SetIconSupportsFlags(struct ScaIconNode *in, BOOL isDiskWritable);
BOOL IsSoftLink(CONST_STRPTR Name);
BOOL IsNoIconPosition(const struct ExtGadget *gg);
void SetIconName(Object *IconObj, struct ScaIconNode *in);
enum ScanDirResult GetFileList(struct ReadIconListControl *rilc,
	enum ScanDirResult (*CheckFunc)(struct ReadIconListControl *rilc),
	BOOL UseExAll, BOOL FetchIconType, BOOL CheckOverlap);
BOOL RilcInit(struct ReadIconListControl *rilc, struct internalScaWindowTask *iwt);
void RilcCleanup(struct ReadIconListControl *rilc);
enum ScanDirResult LinkIconScanList(struct ReadIconListControl *rilc);
LONG AddFileToFilesList(struct ReadIconListControl *rilc, BPTR dirLock, CONST_STRPTR Name);
BOOL IsFileHidden(CONST_STRPTR Filename, ULONG Protection);
BOOL ScanDirIsBackDropIcon(struct internalScaWindowTask *iwt, struct BackDropList *bdl,
	BPTR fLock, CONST_STRPTR FileName);
void ScanDirFillRildFromIse(struct ReadIconListData *rild, const struct IconScanEntry *ise);
void ScanDirUpdateStatusBarText(struct internalScaWindowTask *iwt, ULONG TotalIcons);
BOOL ScanDirIsError(enum ScanDirResult sdResult);
///
/* ------------------------------------------------- */

// defined in ScanDirText.c
///
struct ScaIconNode *TextWindowReadIcon(struct internalScaWindowTask *iwt,
	CONST_STRPTR Name, struct ScaReadIconArg *ria);
enum ScanDirResult ReadTextWindowIconList(struct internalScaWindowTask *iwt);
///
/* ------------------------------------------------- */

// defined in Semaphores.c
///
BOOL AttemptSemaphoreNoNest(SCALOSSEMAPHORE *sema);
void ScaObtainSemaphoreListA(struct TagItem *TagList);
void ScaObtainSemaphoreList(ULONG FirstTag, ...);
BOOL ScaAttemptSemaphoreListA(struct TagItem *OriginalTagList);
BOOL ScaAttemptSemaphoreList(ULONG FirstTag, ...);

#ifdef	DEBUG_SEMAPHORES

SCALOSSEMAPHORE	*DebugScalosCreateSemaphore(CONST_STRPTR, CONST_STRPTR, ULONG);
void DebugScalosDeleteSemaphore(SCALOSSEMAPHORE *sema, CONST_STRPTR, CONST_STRPTR, ULONG);
void DebugScalosInitSemaphore(SCALOSSEMAPHORE *xsema, CONST_STRPTR, CONST_STRPTR, ULONG);
void DebugScalosObtainSemaphore(SCALOSSEMAPHORE *xsema, CONST_STRPTR, CONST_STRPTR, ULONG);
void DebugScalosObtainSemaphoreShared(SCALOSSEMAPHORE *xsema, CONST_STRPTR, CONST_STRPTR, ULONG);
void DebugScalosReleaseSemaphore(SCALOSSEMAPHORE *xsema, CONST_STRPTR, CONST_STRPTR, ULONG);
ULONG DebugScalosAttemptSemaphore(SCALOSSEMAPHORE *xsema, CONST_STRPTR, CONST_STRPTR, ULONG);
ULONG DebugScalosAttemptSemaphoreShared(SCALOSSEMAPHORE *xsema, CONST_STRPTR, CONST_STRPTR, ULONG);

void DebugScalosLockIconListShared(struct internalScaWindowTask *iwt,
	CONST_STRPTR CallingFile, CONST_STRPTR CallingFunc, ULONG CallingLine);
void DebugScalosLockIconListExclusive(struct internalScaWindowTask *iwt,
	CONST_STRPTR CallingFile, CONST_STRPTR CallingFunc, ULONG CallingLine);

#else /* DEBUG_SEMAPHORES */

SCALOSSEMAPHORE	*ScalosCreateSemaphore(void);
void ScalosDeleteSemaphore(SCALOSSEMAPHORE *sema);
void ScalosInitSemaphore(SCALOSSEMAPHORE *xsema);
void ScalosObtainSemaphore(SCALOSSEMAPHORE *xsema);
void ScalosObtainSemaphoreShared(SCALOSSEMAPHORE *xsema);
void ScalosReleaseSemaphore(SCALOSSEMAPHORE *xsema);
ULONG ScalosAttemptSemaphore(SCALOSSEMAPHORE *xsema);
ULONG ScalosAttemptSemaphoreShared(SCALOSSEMAPHORE *xsema);

#endif /* DEBUG_SEMAPHORES */

///
/* ------------------------------------------------- */

// defined in SeparatorClassGadget.c
///
struct ScalosClass *initSeparatorGadgetClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in Shortcuts.c
///
BOOL ShortcutAddLines(struct BackDropList *bdlVolume);
void ShortcutReadPrefs(void);
void ShortcutFreePrefs(void);
BOOL ShortcutAddEntry(STRPTR path, LONG PosX, LONG PosY);
BOOL ShortcutRemoveEntry(BPTR iconLock);
///
/* ------------------------------------------------- */

// defined in Splash.c
///
ULONG InitSplash(struct MsgPort *ReplyPort);
void SplashAddUser(void);
void SplashRemoveUser(void);
void SplashDisplayProgress(const char *fmt, ULONG NumArgs, ...);
///
/* ------------------------------------------------- */

// defined in StatusBar.c
///
BOOL StatusBarAdd(struct internalScaWindowTask *iwt);
void StatusBarRemove(struct internalScaWindowTask *iwt);
UWORD StatusBarQueryGadgetID(struct internalScaWindowTask *iwt,
	const struct ExtGadget *gg, WORD x, WORD y);
///
/* ------------------------------------------------- */

// defined in StringGadgetClass.c
///
struct ScalosClass *initStringGadgetClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in Subroutines.c
///
void StripTrailingColon(STRPTR Line);
void StripTrailingLF(STRPTR Line);
void StripIconExtension(STRPTR Line);
BOOL CheckOverlap(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
BOOL ScaRectInRegion(const struct Region *TestRegion, const struct Rectangle *TestRect);
ULONG CheckPosition(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
STRPTR SafeStrCat(STRPTR dest, CONST_STRPTR src, size_t DestLen);
BOOL CheckAllowCloseWB(void);
void ClearMsgPort(struct MsgPort *msgPort);

ULONG WaitReply(struct MsgPort *port, struct internalScaWindowTask *iwt, ULONG MsgType);
ULONG HandleWindowTaskIconPortMessages(struct internalScaWindowTask *iwt, 
	struct Message *msg, ULONG *MessageType);
///
/* ------------------------------------------------- */

// defined in TextIconClass.c
///
Class * initTextIconClass(void);
///
/* ------------------------------------------------- */

// defined in TextIconHighlightClass.c
///
Class *initTextIconHighlightClass(void);
///
/* ------------------------------------------------- */

// defined in TextWindowClass.c
///
struct ScalosClass *initTextWindowClass(const struct PluginClass *plug);
void ReposTextIcons2(struct internalScaWindowTask *iwt);
HOOKFUNC GetTextIconSortFunction(struct internalScaWindowTask *iwt);
///
/* ------------------------------------------------- */

// defined in TitleClass.c
///
struct ScalosClass *initTitleClass(const struct PluginClass *plug);
void TitleClass_Convert64KMG(ULONG64 Number, STRPTR Buffer, size_t MaxLen);
void TitleClass_Convert64KMGRounded(ULONG64 Number, STRPTR Buffer, size_t MaxLen, ULONG Round);
///
/* ------------------------------------------------- */

// defined in ThumbnailCache.c
///
BOOL ThumbnailCacheInit(void);
void CleanupThumbnailCache(void);
BOOL ThumbnailCacheOpen(APTR *pThumbnailCacheHandle);
void ThumbnailCacheClose(APTR *pThumbnailCacheHandle);
void ThumbnailCacheAddARGB(BPTR fLock, APTR ThumbnailCacheHandle,
	const struct ARGBHeader *argbh, ULONG LifetimeDays);
void ThumbnailCacheAddRemapped(BPTR fLock, APTR ThumbnailCacheHandle,
	const struct ScalosBitMapAndColor *sac, ULONG LifetimeDays);
BOOL ThumbnailCacheFindARGB(BPTR fLock, APTR ThumbnailCacheHandle,
	struct ARGBHeader *argbh, ULONG LifetimeDays);
struct ScalosBitMapAndColor *ThumbnailCacheFindRemapped(BPTR fLock,
	APTR ThumbnailCacheHandle, ULONG LifetimeDays);
BOOL ThumbnailCacheRemoveEntry(BPTR DirLock, CONST_STRPTR FileName);
BOOL ThumbnailCacheCleanup(APTR ThumbnailCacheHandle);
BOOL ThumbnailCacheBegin(APTR ThumbnailCacheHandle);
BOOL ThumbnailCacheFinish(APTR ThumbnailCacheHandle);
///
/* ------------------------------------------------- */

// defined in Thumbnails.c
///
BOOL AddThumbnailIcon(struct internalScaWindowTask *iwt, Object *IconObj,
	BPTR DirLock, CONST_STRPTR Name, ULONG Flags, APTR UndoStep);
BOOL GenerateThumbnails(struct internalScaWindowTask *iwt);
void RearrangeThumbnailList(struct internalScaWindowTask *iwt);
void FlushThumbnailEntries(struct internalScaWindowTask *iwt);
void RemoveThumbnailEntry(struct internalScaWindowTask *iwt, Object *IconObj);
BOOL SetIconThumbnailARGB(struct internalScaWindowTask *iwt,
	struct SM_SetThumbnailImage_ARGB *smtia);
BOOL SetIconThumbnailRemapped(struct internalScaWindowTask *iwt,
	struct SM_SetThumbnailImage_Remapped *smtir);
void ThumbnailsStartCleanup(BOOL Force);
///
/* ------------------------------------------------- */

// defined in ToolTip.c
///
ULONG IconWinShowIconToolTip(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
ULONG IconWinShowGadgetToolTip(struct internalScaWindowTask *iwt, ULONG GadgetID, struct Hook *GadgetTextHook);
void ResetToolTips(struct internalScaWindowTask *iwt);
void GetProtectionString(ULONG Protection, STRPTR String, size_t MaxLen);
///
/* ------------------------------------------------- */

// defined in ToolTypes.c
///
LONG SetToolType(Object *iconObj, CONST_STRPTR ToolTypeName, CONST_STRPTR ToolTypeValue, BOOL SaveIcon);
LONG RemoveToolType(Object *iconObj, CONST_STRPTR ToolTypeName, BOOL SaveIcon);
STRPTR *CloneToolTypeArray(CONST_STRPTR *ToolTypeArray, ULONG AdditionalEntries);
///
/* ------------------------------------------------- */

// defined in TTLayout.c
///
void ScaFormatString(char *Buffer, const char *Format, ...);
void ScaFormatStringMaxLength(char *Buffer, size_t BuffLen, const char *Format, ...);
void ScaFormatStringArgs(char *Buffer, size_t BuffLength, const char *Format, APTR Args);
///
/* ------------------------------------------------- */

// defined in Undo.c
///
void UndoCleanup(void);
BOOL UndoAddEvent(struct internalScaWindowTask *iwt, enum ScalosUndoType type, ULONG FirstTag, ...);
BOOL UndoAddEventTagList(struct internalScaWindowTask *iwt, enum ScalosUndoType type, struct TagItem *TagList);
APTR UndoBeginStep(void);
void UndoEndStep(struct internalScaWindowTask *iwt, APTR event);
BOOL Undo(struct internalScaWindowTask *iwt);
BOOL Redo(struct internalScaWindowTask *iwt);
CONST_STRPTR UndoGetDescription(void);
CONST_STRPTR RedoGetDescription(void);
void UndoWindowSignalOpening(struct internalScaWindowTask *iwt);
void UndoWindowSignalClosing(struct internalScaWindowTask *iwt);
///
/* ------------------------------------------------- */

// defined in Wbl.c
///
SAVEDS(ULONG) INTERRUPT WblTask(void);
void RemWBProgram(struct WBStartup *wbStart);
///
/* ------------------------------------------------- */

// defined in WBStartup.c
///
SAVEDS(void) INTERRUPT WBStartup(void);
void WBStartupFinished(void);
///
/* ------------------------------------------------- */

// defined in Window.c
///
BOOL QueryObjectUnderPointer(struct internalScaWindowTask **iWinUnderPtr,
	struct ScaIconNode **IconUnderPtr,
	struct ScaIconNode **OuterBoundsIconUnderPtr,
	struct Window **foreignWindow);
void AdjustIconActive(struct internalScaWindowTask *iwt);
BOOL isCopyQualifier(ULONG Qualifier);
BOOL isMakeLinkQualifier(ULONG Qualifier);
BOOL isMoveQualifier(ULONG Qualifier);
BOOL isAlternateLssoQualifier(ULONG Qualifier);
void RunMenuCommand(struct internalScaWindowTask *iwt, 
	struct ScalosMenuTree *mtr, struct ScaIconNode *in, ULONG Flags);
void RunMenuCommandExt(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtDest,
	struct ScalosMenuTree *mtr, struct ScaIconNode *in, ULONG Flags);
LONG ScalosPutIcon(struct ScaIconNode *in, BPTR destDirLock, BOOL NeedUpdateIcon);
Object *LoadIconObject(BPTR DirLock, CONST_STRPTR IconName, struct TagItem *TagList);
LONG SaveIconObject(Object *IconObj, BPTR DirLock,
	CONST_STRPTR IconName, BOOL NeedUpdateIcon, 
	struct TagItem *TagList);
void  LockedCloseWindow(struct Window *win);
struct Window *LockedOpenWindowTagList(struct NewWindow *nw, struct TagItem *TagList);
struct Window *LockedOpenWindowTags(struct NewWindow *nw, ULONG FirstTag, ...);
VOID LockedSetWindowTitles(struct Window *win,
	CONST_STRPTR WindowTitle, CONST_STRPTR ScreenTitle);
void SafeSetMenuStrip(struct Window *win);
void SafeClearMenuStrip(struct Window *win);
CONST_STRPTR GetIconName(const struct ScaIconNode *in);
ULONG DisposeScalosWindow(struct internalScaWindowTask *iwt, struct ScaWindowStruct **WindowList);
void AdjustRenamedWindowName(BPTR objLock);
void LockWindow(struct internalScaWindowTask *iwt);
void UnLockWindow(struct internalScaWindowTask *iwt);
struct ScalosMenuTree *CloneMenuTree(const struct ScalosMenuTree *mtr);
void DisposeMenuTree(struct ScalosMenuTree *mtr);
void AppendToMenuTree(struct ScalosMenuTree **mtreList, struct ScalosMenuTree *mTree);
void SetIconWindowReadOnly(struct internalScaWindowTask *iwt, BOOL IsReadOnly);
void RedrawResizedWindow(struct internalScaWindowTask *iwt,
	WORD OldInnerWidth, WORD OldInnerHeight);
void ScaLockLayers(struct Layer_Info *li);
void ScaUnlockLayers(struct Layer_Info *li);
void ScaLockScreenLayers(void);
void ScaUnlockScreenLayers(void);
ULONG ScaLockIBase(ULONG LockNumber);
void ScaUnlockIBase(ULONG Lock);
void UpdateIconOverlays(struct internalScaWindowTask *iwt);
void AddIconOverlay(Object *IconObj, ULONG NewOverlay);
void RemoveIconOverlay(Object *IconObj, ULONG OldOverlay);
BOOL ScalosAttemptLockIconListShared(struct internalScaWindowTask *iwt);
BOOL ScalosAttemptLockIconListExclusive(struct internalScaWindowTask *iwt);

#ifndef	DEBUG_SEMAPHORES
void ScalosLockIconListShared(struct internalScaWindowTask *iwt);
void ScalosLockIconListExclusive(struct internalScaWindowTask *iwt);
#endif /* DEBUG_SEMAPHORES */

void ScalosUnLockIconList(struct internalScaWindowTask *iwt);
struct WindowHistoryEntry *WindowAddHistoryEntry(struct internalScaWindowTask *iwt, BPTR wsLock);
void WindowHistoryEntryDispose(struct internalScaWindowTask *iwt, struct WindowHistoryEntry *whe);
struct WindowHistoryEntry *WindowFindHistoryEntry(struct internalScaWindowTask *iwt, BPTR wsLock);
void WindowNewPath(struct internalScaWindowTask *iwt, CONST_STRPTR path);
void WindowCastShadow(struct Window *win, LONG ShadowWidth, LONG ShadowHeight);
void WindowSendAppIconMsg(struct internalScaWindowTask *iwt, ULONG AppIconClass, struct ScaIconNode *in);
void WindowFadeIn(struct Window *win);
void WindowFadeOut(struct Window *win);
///
/* ------------------------------------------------- */

// defined in WindowClass.c
///
struct ScalosClass *initWindowClass(const struct PluginClass *plug);
///
/* ------------------------------------------------- */

// defined in windowtask.c
///
extern void WindowTask(void);
///
/* ------------------------------------------------- */

// Handling of varargs
///
#if defined(__MORPHOS__) || defined(__amigaos4__)

#include <stdarg.h>

#define ScaFormatString(Buffer, Format, ...) \
	({ ULONG _args[] = { __VA_ARGS__ }; \
		ScaFormatStringArgs((Buffer), INT_MAX, (Format), _args); })

#define ScaFormatStringMaxLength(Buffer, BuffLen, Format, ...) \
	({ ULONG _args[] = { __VA_ARGS__ }; \
		ScaFormatStringArgs((Buffer), BuffLen, (Format), _args); })

#endif /* __MORPHOS__ */
///

#if defined(__SASC)
int snprintf(char *, size_t, const char *, /*args*/ ...);
int vsnprintf(char *, size_t, const char *, va_list ap);
#endif /* __SASC */

#endif /* FUNCTIONS_H */

