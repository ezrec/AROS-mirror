/*
 * include/aros/class-protos.h
 *
 */

/* listclass.c                                */

extern Class *InitListClass(void);

/* KillBGUI.c                                 */

/* No external prototypes defined. */

/* buttonclass.c                              */

extern Class *InitButtonClass(void);

/* labelclass.c                               */

extern Class *InitLabelClass(void);

/* aslreqclass.c                              */

extern Class *InitAslReqClass(void);

/* strformat.c                                */

//extern ASM ULONG CompStrlenF(REG(a0) UBYTE *, REG(a1) ULONG *);
extern ASM REGFUNCPROTO2(ULONG, CompStrlenF, REGPARAM(A0, UBYTE *,), REGPARAM(A1, ULONG *,));

//extern ASM VOID DoSPrintF(REG(a0) UBYTE *, REG(a1) UBYTE *, REG(a2) ULONG *);
extern ASM REGFUNCPROTO3(VOID, DoSPrintF, REGPARAM(A0, UBYTE *,), REGPARAM(A1, UBYTE *,), REGPARAM(A2, ULONG *,));
#ifdef __AROS__
#else
//extern void sprintf(char *, char *, ...);
#endif
extern void tprintf(char *, ...);

/* misc.c                                     */

extern struct TextAttr Topaz80;

//extern ASM LONG max(REG(d0) LONG, REG(d1) LONG);
extern ASM REGFUNCPROTO2(LONG, max, REGPARAM(D0, LONG,), REGPARAM(D1, LONG,));

//extern ASM LONG min(REG(d0) LONG, REG(d1) LONG);
extern ASM REGFUNCPROTO2(LONG, min, REGPARAM(D0, LONG,), REGPARAM(D1, LONG,));

//extern ASM LONG abs(REG(d0) LONG);

//extern ASM LONG range(REG(d0) LONG, REG(d1) LONG, REG(d2) LONG);
extern ASM REGFUNCPROTO3(LONG, range, REGPARAM(D0, LONG,), REGPARAM(D1, LONG,), REGPARAM(D2, LONG,));

//extern ASM ULONG CountLabels(REG(a0) UBYTE **);
extern ASM REGFUNCPROTO1(ULONG, CountLabels, REGPARAM(A0, UBYTE **,));

//extern ASM BOOL PointInBox(REG(a0) struct IBox *, REG(d0) WORD, REG(d1) WORD);
extern ASM REGFUNCPROTO3(BOOL, PointInBox, REGPARAM(A0, struct IBox *,), REGPARAM(D0, WORD,), REGPARAM(D1, WORD,));

extern VOID Scale(struct RastPort *, UWORD *, UWORD *, UWORD, UWORD, struct TextAttr *);
extern WORD MapKey(UWORD, UWORD, APTR *);

//extern ASM VOID ShowHelpReq( REG(a0) struct Window *, REG(a1) UBYTE *);
extern ASM REGFUNCPROTO2(VOID, ShowHelpReq, REGPARAM(A0, struct Window *,), REGPARAM(A1, UBYTE *,));

//extern ASM UBYTE *DoBuffer(REG(a0) UBYTE *, REG(a1) UBYTE **, REG(a2) ULONG *, REG(a3) ULONG *);
extern ASM REGFUNCPROTO4(UBYTE *, DoBuffer, REGPARAM(A0, UBYTE *,), REGPARAM(A1, UBYTE **,), REGPARAM(A2, ULONG *,), REGPARAM(A3, ULONG *,));

extern VOID DoMultiSet(Tag, ULONG, ULONG, Object *, ...);

//extern ASM VOID SetGadgetBounds(REG(a0) Object *, REG(a1) struct IBox *);
extern ASM REGFUNCPROTO2(VOID, SetGadgetBounds, REGPARAM(A0, Object *,), REGPARAM(A1, struct IBox *,));

//extern ASM VOID SetImageBounds(REG(a0) Object *, REG(a1) struct IBox *);
extern ASM REGFUNCPROTO2(VOID, SetImageBounds, REGPARAM(A0, Object *,), REGPARAM(A1, struct IBox *,));

//extern ASM VOID UnmapTags(REG(a0) struct TagItem *, REG(a1) struct TagItem *);
extern ASM REGFUNCPROTO2(VOID, UnmapTags, REGPARAM(A0, struct TagItem *,), REGPARAM(A1, struct TagItem *,));

//extern ASM Object *CreateVector(REG(a0) struct TagItem *);
extern ASM REGFUNCPROTO1(Object *, CreateVector, REGPARAM(A0, struct TagItem *,));

//extern ASM struct TagItem *BGUI_NextTagItem(REG(a0) struct TagItem **);
extern ASM REGFUNCPROTO1(struct TagItem *, BGUI_NextTagItem, REGPARAM(A0, struct TagItem **,));

/* baseclass.c                                */

extern ULONG CalcDimensions(Class *, Object *, struct bmDimensions *, UWORD, UWORD);
#ifndef __AROS__
extern SAVEDS ASM VOID BGUI_PostRender(REG(a0) Class *, REG(a2) Object *, REG(a1) struct gpRender *);
#endif

extern Class *BaseClass;
extern Class *InitBaseClass(void);

/* ver.c                                      */

/* No external prototypes defined. */

/* request.c                                  */

#ifndef __AROS__
extern SAVEDS ASM ULONG BGUI_RequestA(REG(a0) struct Window *, REG(a1) struct bguiRequest *, REG(a2) ULONG *);
#endif

/* cycleclass.c                               */

extern Class *InitCycleClass(void);

/* task.c                                     */

extern TL                     TaskList;
extern struct SignalSemaphore TaskLock;
extern void InitTaskList(void);
extern struct TextFont *BGUI_OpenFontDebug(struct TextAttr *,STRPTR,ULONG);
extern void BGUI_CloseFontDebug(struct TextFont *,STRPTR,ULONG);
extern void FreeTaskList(void);
extern TM *FindTaskMember(void);
extern TM *InternalFindTaskMember(struct Task *);
extern UWORD AddTaskMember(void);
extern BOOL FreeTaskMember( void);
extern BOOL GetWindowBounds(ULONG, struct IBox *);
extern VOID SetWindowBounds(ULONG, struct IBox *);

#ifndef __AROS__
extern SAVEDS ASM APTR BGUI_AllocPoolMemDebug(REG(d0) ULONG, REG(a0) STRPTR, REG(d1) ULONG);
extern SAVEDS ASM APTR BGUI_AllocPoolMem(REG(d0) ULONG);
extern SAVEDS ASM VOID BGUI_FreePoolMemDebug(REG(a0) APTR, REG(a1) STRPTR, REG(d0) ULONG);
extern SAVEDS ASM VOID BGUI_FreePoolMem(REG(a0) APTR);
extern SAVEDS ASM REGFUNCPROTO1( VOID , BGUI_FreePoolMem, REGPARAM(a0,APTR,));
#endif

//extern ASM BOOL AddIDReport(REG(a0) struct Window *, REG(d0) ULONG, REG(a1) struct Task *);
extern ASM REGFUNCPROTO3(BOOL, AddIDReport, REGPARAM(A0, struct Window *,), REGPARAM(D0, ULONG,), REGPARAM(A1, struct Task *,));

//extern ASM ULONG GetIDReport(REG(a0) struct Window *);
extern ASM REGFUNCPROTO1(ULONG, GetIDReport, REGPARAM(A0, struct Window *,));

extern struct Window *GetFirstIDReportWindow(void);

//extern ASM VOID RemoveIDReport(REG(a0) struct Window *);
extern ASM REGFUNCPROTO1(VOID, RemoveIDReport, REGPARAM(A0, struct Window *,));

//extern ASM VOID AddWindow(REG(a0) Object *, REG(a1) struct Window *);
extern ASM REGFUNCPROTO2(VOID, AddWindow, REGPARAM(A0, Object *,), REGPARAM(A1, struct Window *,));

//extern ASM VOID RemWindow(REG(a0) Object *);
extern ASM REGFUNCPROTO1(VOID, RemWindow, REGPARAM(A0, Object *,));

//extern ASM Object *WhichWindow(REG(a0) struct Screen *);
extern ASM REGFUNCPROTO1(Object *, WhichWindow, REGPARAM(A0, struct Screen *,));

extern struct TagItem *DefTagList(ULONG, struct TagItem *);

//extern SAVEDS ASM ULONG BGUI_CountTagItems(REG(a0) struct TagItem *);
extern SAVEDS ASM REGFUNCPROTO1(ULONG, BGUI_CountTagItems, REGPARAM(A0, struct TagItem *,));

//extern SAVEDS ASM struct TagItem *BGUI_MergeTagItems(REG(a0) struct TagItem *, REG(a1) struct TagItem *);
extern SAVEDS ASM REGFUNCPROTO2(struct TagItem *, BGUI_MergeTagItems, REGPARAM(A0, struct TagItem *,), REGPARAM(A1, struct TagItem *,));

//extern SAVEDS ASM struct TagItem *BGUI_CleanTagItems(REG(a0) struct TagItem *, REG(d0) LONG);
extern SAVEDS ASM REGFUNCPROTO2(struct TagItem *, BGUI_CleanTagItems, REGPARAM(A0, struct TagItem *,), REGPARAM(D0, LONG,));

#ifndef __AROS__
extern SAVEDS ASM struct TagItem *BGUI_GetDefaultTags(REG(d0) ULONG);
extern SAVEDS ASM VOID BGUI_DefaultPrefs(VOID);
extern SAVEDS ASM VOID BGUI_LoadPrefs(REG(a0) UBYTE *);
#endif

/* arexxclass.c                               */

extern Class *InitArexxClass(void);

/* stringclass.c                              */

extern Class *InitStringClass(void);

/* externalclass.c                            */

extern Class *InitExtClass( void);

/* classes.c                                  */

extern UBYTE *RootClass;
extern UBYTE *ImageClass;
extern UBYTE *GadgetClass;
extern UBYTE *PropGClass;
extern UBYTE *StrGClass;
extern Class *BGUI_MakeClass(ULONG, ...);

//extern SAVEDS ASM ULONG __GCD( REG(a0) Class *, REG(a2) Object *, REG(a1) Msg);
extern SAVEDS ASM REGFUNCPROTO3(ULONG, __GCD, REGPARAM(A0, Class *,), REGPARAM(A2, Object *,), REGPARAM(A1, Msg,));

#ifndef __AROS__
extern ASM Class *BGUI_MakeClassA(REG(a0) struct TagItem *);
extern SAVEDS ASM BOOL BGUI_FreeClass(REG(a0) Class *);
#endif

//extern ULONG ASM BGUI_GetAttrChart(REG(a0) Class *, REG(a2) Object *, REG(a1) struct rmAttr *);
extern ASM REGFUNCPROTO3(ULONG, BGUI_GetAttrChart, REGPARAM(A0, Class *,), REGPARAM(A2, Object *,), REGPARAM(A1, struct rmAttr *,));

//extern ULONG ASM BGUI_SetAttrChart(REG(a0) Class *, REG(a2) Object *, REG(a1) struct rmAttr *);
extern ASM REGFUNCPROTO3(ULONG, BGUI_SetAttrChart, REGPARAM(A0, Class *,), REGPARAM(A2, Object *,), REGPARAM(A1, struct rmAttr *,));

extern ULONG BGUI_PackStructureTag(UBYTE *, ULONG *, ULONG, ULONG);
extern ULONG BGUI_UnpackStructureTag(UBYTE *, ULONG *, ULONG, ULONG *);

#ifndef __AROS__
extern SAVEDS ULONG ASM BGUI_PackStructureTags(REG(a0) APTR, REG(a1) ULONG *, REG(a2) struct TagItem *);
extern SAVEDS ULONG ASM BGUI_UnpackStructureTags(REG(a0) APTR, REG(a1) ULONG *, REG(a2) struct TagItem *);
#endif

//extern ASM ULONG Get_Attr(REG(a0) Object *, REG(d0) ULONG, REG(a1) void *);
extern ASM REGFUNCPROTO3(ULONG, Get_Attr, REGPARAM(A0, Object *,), REGPARAM(D0, ULONG,), REGPARAM(A1, void *,));

//extern ASM ULONG Get_SuperAttr(REG(a2) Class *, REG(a0) Object *, REG(d0) ULONG, REG(a1) void *);
extern ASM REGFUNCPROTO4(ULONG, Get_SuperAttr, REGPARAM(A2, Class *,), REGPARAM(A0, Object *,), REGPARAM(D0, ULONG,), REGPARAM(A1, void *,));

extern ULONG NewSuperObject(Class *, Object *, struct TagItem *);
extern ULONG DoSetMethodNG(Object *, Tag, ...);
extern ULONG DoSuperSetMethodNG(Class *, Object *, Tag, ...);
extern ULONG DoSetMethod(Object *, struct GadgetInfo *, Tag, ...);
extern ULONG DoSuperSetMethod(Class *, Object *, struct GadgetInfo *, Tag, ...);
extern ULONG DoUpdateMethod(Object *, struct GadgetInfo *, ULONG, Tag, ...);
extern ULONG DoNotifyMethod(Object *, struct GadgetInfo *, ULONG, Tag, ...);

//extern ASM ULONG DoRenderMethod(REG(a0) Object *, REG(a1) struct GadgetInfo *, REG(d0) ULONG);
extern ASM REGFUNCPROTO3(ULONG, DoRenderMethod, REGPARAM(A0, Object *,), REGPARAM(A1, struct GadgetInfo *,), REGPARAM(D0, ULONG,));

//extern ASM ULONG ForwardMsg(REG(a0) Object *, REG(a1) Object *, REG(a2) Msg);
extern ASM REGFUNCPROTO3(ULONG, ForwardMsg, REGPARAM(A0, Object *,), REGPARAM(A1, Object *,), REGPARAM(A2, Msg,));

extern struct BaseInfo *AllocBaseInfoDebug(STRPTR, ULONG,ULONG, ...);
extern struct BaseInfo *AllocBaseInfo(ULONG, ...);

//extern SAVEDS ASM struct BaseInfo *BGUI_AllocBaseInfoDebugA(REG(a0) struct TagItem *,REG(a1) STRPTR, REG(d0) ULONG);
extern SAVEDS ASM REGFUNCPROTO3(struct BaseInfo *, BGUI_AllocBaseInfoDebugA, REGPARAM(A0, struct TagItem *,), REGPARAM(A1, STRPTR,), REGPARAM(D0, ULONG,));

//extern SAVEDS ASM struct BaseInfo *BGUI_AllocBaseInfoA(REG(a0) struct TagItem *);
extern SAVEDS ASM REGFUNCPROTO1(struct BaseInfo *, BGUI_AllocBaseInfoA, REGPARAM(A0, struct TagItem *,));

extern void FreeBaseInfoDebug(struct BaseInfo *, STRPTR, ULONG);
extern void FreeBaseInfo(struct BaseInfo *);

/* indicatorclass.c                           */

extern Class *InitIndicatorClass(void);

/* checkboxclass.c                            */

extern Class *InitCheckBoxClass(void);

/* radiobuttonclass.c                         */

extern Class *InitRadioButtonClass(void);

/* bgui_locale.c                              */

extern UWORD  NumCatCompStrings;
extern struct CatCompArrayType CatCompArray[];

/* commodityclass.c                           */

extern Class *InitCxClass(void);

/* fontreqclass.c                             */

extern Class *InitFontReqClass(void);

/* frameclass.c                               */

extern Class *InitFrameClass(void);

/* screenreqclass.c                           */

extern Class *InitScreenReqClass(void);

/* sliderclass.c                              */

extern Class *InitSliderClass(void);

/* spacingclass.c                             */

extern Class *InitSpacingClass(void);

/* systemiclass.c                             */

extern Class *InitSystemClass(void);

/* pageclass.c                                */

extern Class *InitPageClass(void);

/* MakeProto.c                                */

/* No external prototypes defined. */

/* windowclass.c                              */

extern Class *InitWindowClass(void);

/* dgm.c                                      */

extern ULONG myDoGadgetMethod(Object *, struct Window *, struct Requester *, ULONG, ...);

#ifndef __AROS__
extern SAVEDS ASM ULONG BGUI_DoGadgetMethodA( REG(a0) Object *, REG(a1) struct Window *, REG(a2) struct Requester *, REG(a3) Msg);
#endif

extern Class *InitDGMClass(void);

/* groupclass.c                               */

extern Class *InitGroupNodeClass(void);

//extern ASM ULONG RelayoutGroup(REG(a0) Object *);
extern ASM REGFUNCPROTO1(ULONG, RelayoutGroup, REGPARAM(A0, Object *,));

extern Class *InitGroupClass(void);

/* rootclass.c                                */

extern Object *ListHeadObject(struct List *);
extern Object *ListTailObject(struct List *);
extern Class *InitRootClass(void);
extern Class *InitGadgetClass(void);
extern Class *InitImageClass(void);

/* libfunc.c                                  */

extern BOOL FreeClasses(void);
extern ULONG TrackNewObject(Object *,struct TagItem *);
extern BOOL TrackDisposedObject(Object *);
extern void MarkFreedClass(Class *);

#ifndef __AROS__
extern SAVEDS ASM Class *BGUI_GetClassPtr(REG(d0) ULONG);
#endif

extern Object *BGUI_NewObject(ULONG, Tag, ...);

#ifndef __AROS__
extern SAVEDS ASM Object *BGUI_NewObjectA(REG(d0) ULONG, REG(a0) struct TagItem *);
extern SAVEDS ASM struct BitMap *BGUI_AllocBitMap(REG(d0) ULONG, REG(d1) ULONG, REG(d2) ULONG, REG(d3) ULONG, REG(a0) struct BitMap *);
extern SAVEDS ASM VOID BGUI_FreeBitMap(REG(a0) struct BitMap *);
extern SAVEDS ASM struct RastPort *BGUI_CreateRPortBitMap(REG(a0) struct RastPort *, REG(d0) ULONG, REG(d1) ULONG, REG(d2) ULONG);
extern SAVEDS ASM VOID BGUI_FreeRPortBitMap(REG(a0) struct RastPort *);
extern SAVEDS ASM BOOL BGUI_Help(REG(a0) struct Window *, REG(a1) UBYTE *, REG(a2) UBYTE *, REG(d0) ULONG);
extern SAVEDS ASM APTR BGUI_LockWindow( REG(a0) struct Window *);
extern SAVEDS ASM VOID BGUI_UnlockWindow( REG(a0) APTR);
extern SAVEDS ASM STRPTR BGUI_GetLocaleStr(REG(a0) struct bguiLocale *, REG(d0) ULONG);
extern SAVEDS ASM STRPTR BGUI_GetCatalogStr(REG(a0) struct bguiLocale *, REG(d0) ULONG, REG(a1) STRPTR);
#endif

//extern SAVEDS ASM ULONG BGUI_CallHookPkt(REG(a0) struct Hook *,REG(a2) APTR,REG(a1) APTR);
extern SAVEDS ASM REGFUNCPROTO3(ULONG, BGUI_CallHookPkt, REGPARAM(A0, struct Hook *,), REGPARAM(A2, APTR,), REGPARAM(A1, APTR,));

/* filereqclass.c                             */

extern Class *InitFileReqClass(void);

/* separatorclass.c                           */

extern Class *InitSepClass(void);

/* areaclass.c                                */

extern Class *InitAreaClass(void);

/* Tap.c                                      */

/* No external prototypes defined. */

/* vectorclass.c                              */

extern Class *InitVectorClass(void);

/* mxclass.c                                  */

extern Class *InitMxClass(void);

/* viewclass.c                                */

extern Class *InitViewClass(void);

/* propclass.c                                */

extern Class *InitPropClass(void);

/* infoclass.c                                */

extern Class *InitInfoClass( void);

/* textclass.c                                */

extern Class *InitTextClass(void);
#ifndef __AROS__
extern SAVEDS ASM VOID BGUI_InfoTextSize(REG(a0) struct RastPort *, REG(a1) UBYTE *, REG(a2) UWORD *, REG(a3) UWORD *);
extern SAVEDS ASM VOID BGUI_InfoText( REG(a0) struct RastPort *, REG(a1) UBYTE *, REG(a2) struct IBox *, REG(a3) struct DrawInfo *);
#endif

//extern UWORD ASM TotalHeight( REG(a0) struct RastPort *, REG(a1) UBYTE *);
extern ASM REGFUNCPROTO2(UWORD, TotalHeight, REGPARAM(A0, struct RastPort *,), REGPARAM(A1, UBYTE *,));

//extern UWORD ASM TotalWidth(REG(a0) struct RastPort *, REG(a1) UBYTE *);
extern ASM REGFUNCPROTO2(UWORD, TotalWidth, REGPARAM(A0, struct RastPort *,), REGPARAM(A1, UBYTE *,));

extern void RenderInfoText(struct RastPort *, UBYTE *, UWORD *, struct IBox *, UWORD);
extern void RenderText(struct BaseInfo *, UBYTE *, struct IBox *);

/* lib.c                                      */

extern BOOL OS30;
extern VOID InitLocale(void);
#ifdef __AROS__
#else
extern SAVEDS ASM struct Library *LibInit(REG(a0) BPTR, REG(a6) struct ExecBase *);
extern SAVEDS ASM struct Library *LibOpen(REG(a6) struct Library *, REG(d0) ULONG);
extern SAVEDS ASM BPTR LibClose(REG(a6) struct Library *);
extern SAVEDS ASM BPTR LibExpunge(REG(a6) struct Library *);
extern SAVEDS LONG LibVoid(void);
#endif

/* gfx.c                                      */

extern UWORD DefDriPens[12];

//extern ASM ULONG TextWidth(REG(a1) struct RastPort *, REG(a0) UBYTE *);
extern ASM REGFUNCPROTO2(ULONG, TextWidth, REGPARAM(A1, struct RastPort *,), REGPARAM(A0, UBYTE *,));

//extern ASM ULONG TextWidthNum(REG(a1) struct RastPort *, REG(a0) UBYTE *, REG(d0) ULONG);
extern ASM REGFUNCPROTO3(ULONG, TextWidthNum, REGPARAM(A1, struct RastPort *,), REGPARAM(A0, UBYTE *,), REGPARAM(D0, ULONG,));

//extern ASM VOID BDisableBox(REG(a0) struct BaseInfo *, REG(a1) struct IBox *);
extern ASM REGFUNCPROTO2(VOID, BDisableBox, REGPARAM(A0, struct BaseInfo *,), REGPARAM(A1, struct IBox *,));

extern VOID RenderTitle(Object *, struct BaseInfo *, WORD, WORD, WORD, BOOL, BOOL, UWORD);

//extern VOID ASM SetDashedLine(REG(a0) struct BaseInfo *, REG(d0) UWORD);
extern ASM REGFUNCPROTO2(VOID, SetDashedLine, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, UWORD,));

extern VOID RenderBevelBox(struct BaseInfo *, WORD, WORD, WORD, WORD, UWORD, BOOL, BOOL);

//extern ASM VOID SRectFillDebug(REG(a0) struct RastPort *, REG(d0) LONG, REG(d1) LONG, REG(d2) LONG, REG(d3) LONG,REG(a1) STRPTR,REG(d4) ULONG);
extern ASM REGFUNCPROTO7(VOID, SRectFillDebug, REGPARAM(A0, struct RastPort *,), REGPARAM(D0, LONG,), REGPARAM(D1, LONG,), REGPARAM(D2, LONG,), REGPARAM(D3, LONG,), REGPARAM(A1, STRPTR,), REGPARAM(D4, ULONG, ));
extern ASM REGFUNCPROTO5(VOID, SRectFill, REGPARAM(A0, struct RastPort *, ), REGPARAM(D0, LONG,), REGPARAM(D1, LONG,), REGPARAM(D2, LONG,), REGPARAM(D3, LONG,));
        
//extern ASM VOID BRectFillDebug(REG(a0) struct BaseInfo *, REG(d0) LONG, REG(d1) LONG, REG(d2) LONG, REG(d3) LONG,REG(a1) STRPTR,REG(d4) ULONG);
extern ASM REGFUNCPROTO7(VOID, BRectFillDebug, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, LONG,), REGPARAM(D1, LONG,), REGPARAM(D2, LONG,), REGPARAM(D3, LONG,), REGPARAM(A1, STRPTR,), REGPARAM(D4, ULONG,));

//extern ASM VOID BRectFillA(REG(a0) struct BaseInfo *, REG(a1) struct Rectangle *);
extern ASM REGFUNCPROTO2(VOID, BRectFillA, REGPARAM(a0,struct BaseInfo *,), REGPARAM(a1, struct Rectangle *,));
extern ASM REGFUNCPROTO5(VOID, BRectFill, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, LONG,), REGPARAM(D1, LONG,), REGPARAM(D2, LONG,), REGPARAM(D3, LONG,));

//extern ASM VOID BBoxFill(REG(a0) struct BaseInfo *, REG(d0) LONG, REG(d1) LONG, REG(d2) LONG, REG(d3) LONG);
extern ASM REGFUNCPROTO5(VOID, BBoxFill, REGPARAM(A0, struct BaseInfo *,), REGPARAM(d0,LONG,), REGPARAM(d1,LONG,), REGPARAM(d2,LONG,), REGPARAM(d3,LONG,));

//extern ASM VOID BBoxFillA(REG(a0) struct BaseInfo *, REG(a1) struct IBox *);
extern ASM REGFUNCPROTO2(VOID, BBoxFillA, REGPARAM(a0, struct BaseInfo *,), REGPARAM(a1,struct IBox *,));

//extern ASM VOID RenderBackFill(REG(a0) struct RastPort *, REG(a1) struct IBox *, REG(a2) UWORD *, REG(d0) ULONG);
extern ASM REGFUNCPROTO4(VOID, RenderBackFill, REGPARAM(A0, struct RastPort *,), REGPARAM(A1, struct IBox *,), REGPARAM(A2, UWORD *,), REGPARAM(D0, ULONG,));

//extern ASM VOID RenderBackFillRasterDebug(REG(a0) struct RastPort *, REG(a1) struct IBox *, REG(d0) UWORD, REG(d1) UWORD,REG(a2) STRPTR, REG(d2) ULONG);
extern ASM REGFUNCPROTO6(VOID, RenderBackFillRasterDebug, REGPARAM(A0, struct RastPort *,), REGPARAM(A1, struct IBox *,), REGPARAM(D0, UWORD,), REGPARAM(D1, UWORD,), REGPARAM(A2, STRPTR,), REGPARAM(D2, ULONG,));
extern ASM REGFUNCPROTO4(VOID, RenderBackFillRaster, REGPARAM(A0, struct RastPort *,), REGPARAM(A1, struct IBox *,), REGPARAM(D0, UWORD,), REGPARAM(D1, UWORD,));
        
//extern ASM VOID DottedBox(REG(a0) struct BaseInfo *, REG(a1) struct IBox *);
extern ASM REGFUNCPROTO2(VOID, DottedBox, REGPARAM(A0, struct BaseInfo *,), REGPARAM(A1, struct IBox *,));

//extern ASM ULONG GadgetState(REG(a0) struct BaseInfo *, REG(a1) Object *, REG(d0) BOOL);
extern ASM REGFUNCPROTO3(ULONG, GadgetState, REGPARAM(A0, struct BaseInfo *,), REGPARAM(A1, Object *,), REGPARAM(D0, BOOL,));

#ifndef __AROS__
extern SAVEDS ASM VOID BGUI_FillRectPattern(REG(a1) struct RastPort *, REG(a0) struct bguiPattern *, REG(d0) ULONG, REG(d1) ULONG, REG(d2) ULONG, REG(d3) ULONG);
#endif

//extern VOID ASM HLine(REG(a1) struct RastPort *, REG(d0) UWORD, REG(d1) UWORD, REG(d2) UWORD);
extern ASM REGFUNCPROTO4(VOID, HLine, REGPARAM(A1, struct RastPort *,), REGPARAM(D0, UWORD,), REGPARAM(D1, UWORD,), REGPARAM(D2, UWORD,));

//extern VOID ASM VLine(REG(a1) struct RastPort *, REG(d0) UWORD, REG(d1) UWORD, REG(d2) UWORD);
extern ASM REGFUNCPROTO4(VOID, VLine, REGPARAM(A1, struct RastPort *,), REGPARAM(D0, UWORD,), REGPARAM(D1, UWORD,), REGPARAM(D2, UWORD,));

//extern ASM ULONG FGetAPen(REG(a1) struct RastPort *);
extern ASM REGFUNCPROTO1(ULONG, FGetAPen, REGPARAM(A1, struct RastPort *,));

//extern ASM ULONG FGetBPen(REG(a1) struct RastPort *);
extern ASM REGFUNCPROTO1(ULONG, FGetBPen, REGPARAM(A1, struct RastPort *,));
        
//extern ASM ULONG FGetDrMd(REG(a1) struct RastPort *);
extern ASM REGFUNCPROTO1(ULONG, FGetDrMd, REGPARAM(A1, struct RastPort *,));

//extern ASM ULONG FGetDepth(REG(a1) struct RastPort *);
extern ASM REGFUNCPROTO1(ULONG, FGetDepth, REGPARAM(A1, struct RastPort *,));

//extern ASM VOID FSetAPen(REG(a1) struct RastPort *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, FSetAPen, REGPARAM(A1, struct RastPort *, ), REGPARAM(D0, ULONG,));

//extern ASM VOID FSetBPen(REG(a1) struct RastPort *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, FSetBPen, REGPARAM(A1, struct RastPort *,), REGPARAM(D0, ULONG,));

//extern ASM VOID FSetDrMd(REG(a1) struct RastPort *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, FSetDrMd, REGPARAM(A1, struct RastPort *,), REGPARAM(D0, ULONG,));

//extern ASM VOID FSetABPenDrMd(REG(a1) struct RastPort *, REG(d0) ULONG, REG(d1) ULONG, REG(d2) ULONG);
extern ASM REGFUNCPROTO4(VOID, FSetABPenDrMd, REGPARAM(A1, struct RastPort *,), REGPARAM(D0, ULONG,), REGPARAM(D1, ULONG,), REGPARAM(D2, ULONG,));

//extern ASM VOID FSetFont(REG(a1) struct RastPort *, REG(a0) struct TextFont *);
extern ASM REGFUNCPROTO2(VOID, FSetFont, REGPARAM(A1, struct RastPort *,), REGPARAM(A0, struct TextFont *,));

//extern ASM VOID FSetFontStyle(REG(a1) struct RastPort *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, FSetFontStyle, REGPARAM(A1, struct RastPort *,), REGPARAM(D0, ULONG, ));

//extern ASM VOID FClearAfPt(REG(a1) struct RastPort *);
extern ASM REGFUNCPROTO1(VOID, FClearAfPt, REGPARAM(A1, struct RastPort *,));

//extern ASM VOID BSetDPenA(REG(a0) struct BaseInfo *, REG(d0) LONG);
extern ASM REGFUNCPROTO2(VOID, BSetDPenA, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, LONG,));

//extern ASM VOID BSetPenA(REG(a0) struct BaseInfo *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, BSetPenA, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, ULONG,));

//extern ASM VOID BSetDPenB(REG(a0) struct BaseInfo *, REG(d0) LONG);
extern ASM REGFUNCPROTO2(VOID, BSetDPenB, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, LONG,));

//extern ASM VOID BSetPenB(REG(a0) struct BaseInfo *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, BSetPenB, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, ULONG,));

//extern ASM VOID BSetDrMd(REG(a0) struct BaseInfo *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, BSetDrMd, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, ULONG,));

//extern ASM VOID BSetFont(REG(a0) struct BaseInfo *, REG(a1) struct TextFont *);
extern ASM REGFUNCPROTO2(VOID, BSetFont, REGPARAM(A0, struct BaseInfo *,), REGPARAM(A1, struct TextFont *,));

//extern ASM VOID BSetFontStyle(REG(a0) struct BaseInfo *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, BSetFontStyle, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, ULONG,));

//extern ASM VOID BSetAfPt(REG(a0) struct BaseInfo *, REG(a1) UWORD *, REG(d0) ULONG);
extern ASM REGFUNCPROTO3(VOID, BSetAfPt, REGPARAM(A0, struct BaseInfo *,), REGPARAM(A1, UWORD *,), REGPARAM(D0, ULONG,));

//extern ASM VOID BClearAfPt(REG(a0) struct BaseInfo *);
extern ASM REGFUNCPROTO1(VOID, BClearAfPt, REGPARAM(A0, struct BaseInfo *,));

//extern ASM VOID BSetDrPt(REG(a0) struct BaseInfo *, REG(d0) ULONG);
extern ASM REGFUNCPROTO2(VOID, BSetDrPt, REGPARAM(A0, struct BaseInfo *,), REGPARAM(D0, ULONG,));

//extern ASM VOID BDrawImageState(REG(a0) struct BaseInfo *, REG(a1) Object *, REG(d0) ULONG, REG(d1) ULONG, REG(d2) ULONG);
extern ASM REGFUNCPROTO5(VOID, BDrawImageState, REGPARAM(A0, struct BaseInfo *,), REGPARAM(A1, Object *,), REGPARAM(D0, ULONG,), REGPARAM(D1, ULONG,), REGPARAM(D2, ULONG,));

/* progressclass.c                            */

extern Class *InitProgressClass(void);

/* blitter.c                                  */

//extern VOID ASM EraseBMO(REG(a0) BMO *);
extern ASM REGFUNCPROTO1(VOID, EraseBMO, REGPARAM(A0, BMO *,));

//extern VOID ASM LayerBMO(REG(a0) BMO *);
extern ASM REGFUNCPROTO1(VOID, LayerBMO, REGPARAM(A0, BMO *,));

//extern VOID ASM DrawBMO(REG(a0) BMO *);
extern ASM REGFUNCPROTO1(VOID, DrawBMO, REGPARAM(A0, BMO *,));

//extern ASM BMO *CreateBMO(REG(a0) Object *, REG(a1) struct GadgetInfo *);
extern ASM REGFUNCPROTO2(BMO *, CreateBMO, REGPARAM(A0, Object *,), REGPARAM(A1, struct GadgetInfo *,));

//extern ASM VOID DeleteBMO(REG(a0) BMO *);
extern ASM REGFUNCPROTO1(VOID, DeleteBMO, REGPARAM(A0, BMO *,));

//extern ASM VOID MoveBMO(REG(a0) BMO *, REG(d0) WORD, REG(d1) WORD);
extern ASM REGFUNCPROTO3(VOID, MoveBMO, REGPARAM(A0, BMO *,), REGPARAM(D0, WORD,), REGPARAM(d1, WORD,));

/* miscasm.asm                                */

struct NewAmigaGuide;
//extern /*__stdargs*/ BOOL DisplayAGuideInfo(struct NewAmigaGuide *, Tag, ...);
extern /*__stdargs*/ BOOL DisplayAGuideInfo( struct NewAmigaGuide *, Tag, ...);

//extern ASM ULONG ScaleWeight(REG(d2) ULONG e, REG(d3) ULONG f, REG(d4) ULONG a);
extern ASM REGFUNCPROTO3(ULONG, ScaleWeight, REGPARAM(D2, ULONG,), REGPARAM(D3, ULONG,), REGPARAM(D4, ULONG,));

//extern /*__stdargs*/ ASM ULONG StrLenfA(REG(a0) UBYTE *, REG(a1) ULONG *);
extern ASM REGFUNCPROTO2(ULONG, StrLenfA, REGPARAM(A0, UBYTE *,), REGPARAM(A1, ULONG *,));

//extern /*__stdargs*/ ASM VOID SPrintfA(REG(a3) UBYTE *, REG(a0) UBYTE *, REG(a1) ULONG *);
extern ASM REGFUNCPROTO3(VOID, SPrintfA, REGPARAM(A3, UBYTE *,), REGPARAM(A0, UBYTE *,), REGPARAM(A1, ULONG *,));

//extern ASM VOID LHook_Count(REG(a0) struct Hook *hook, REG(a1) ULONG chr, REG(a2) struct Locale *loc);
extern ASM REGFUNCPROTO3(VOID, LHook_Count, REGPARAM(A0, struct Hook *,), REGPARAM(A2, struct Locale *,), REGPARAM(A1, ULONG,));

//extern ASM VOID LHook_Format(REG(a0) struct Hook *hook, REG(a1) ULONG chr, REG(a2) struct Locale *loc);
extern ASM REGFUNCPROTO3(VOID, LHook_Format, REGPARAM(A0, struct Hook *,), REGPARAM(A2, struct Locale *,), REGPARAM(A1, ULONG,));

//extern ASM struct RastPort *BGUI_ObtainGIRPort( REG(a0) struct GadgetInfo * );
extern ASM REGFUNCPROTO1(struct RastPort *, BGUI_ObtainGIRPort, REGPARAM(A0, struct GadgetInfo *,));

extern ULONG AsmDoMethod( Object *, ULONG MethodID, ... );

extern ULONG AsmDoSuperMethod( Class *, Object *, ULONG MethodID, ... );
extern ULONG AsmCoerceMethod( Class *, Object *, ULONG MethodID, ... );

//extern ASM ULONG AsmDoMethodA( REG(a2) Object *, REG(a1) Msg );
extern ASM REGFUNCPROTO2(ULONG, AsmDoMethodA, REGPARAM(A2, Object *,), REGPARAM(A1, Msg,));

//extern ASM ULONG AsmDoSuperMethodA( REG(a0) Class *, REG(a2) Object *, REG(a1) Msg );
extern ASM REGFUNCPROTO3(ULONG, AsmDoSuperMethodA, REGPARAM(A0, Class *,), REGPARAM(A2, Object *,), REGPARAM(A1, Msg,));

//extern ASM ULONG AsmCoerceMethodA( REG(a0) Class *, REG(a2) Object *, REG(a1) Msg );
extern ASM REGFUNCPROTO3(ULONG, AsmCoerceMethodA, REGPARAM(A0, Class *,), REGPARAM(A2, Object *,), REGPARAM(A1, Msg,));

/* libtag.asm                                 */

/* No external prototypes defined. */

/* stkext.asm                                 */

extern ASM APTR EnsureStack(VOID);

//extern ASM VOID RevertStack( REG(a0) APTR);
extern ASM REGFUNCPROTO1(VOID, RevertStack, REGPARAM(A0, APTR,));

extern VOID InitInputStack(VOID);
extern VOID FreeInputStack(VOID);

#define makeproto
#define makearosproto
