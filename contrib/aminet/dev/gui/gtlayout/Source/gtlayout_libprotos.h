/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=8
*/

#ifndef _GTLAYOUT_LIBPROTOS_H
#define _GTLAYOUT_LIBPROTOS_H 1

/* gtlayout_lib.c */
LONG ReturnError(VOID);

/* LTP_AddGadgets.c */
VOID LTP_AddAndRefreshGadgets(struct Window *Window, struct Gadget *Gadgets);
VOID LTP_StripGadgets(LayoutHandle *Handle, struct Gadget *Gadgets);
VOID LTP_AddGadgets(LayoutHandle *handle);
VOID LTP_AddGadgetsDontRefresh(LayoutHandle *handle);

/* LTP_AddHistory.c */
VOID LTP_AddHistory(struct SGWork *Work);

/* LTP_AdjustItemPosition.c */
LONG LTP_AdjustItemPosition(struct MenuItem *Item, LONG Left, LONG Top);

/* LTP_AdjustMenuPosition.c */
VOID LTP_AdjustMenuPosition(RootMenu *Root);

/* LTP_Atol.c */
ULONG LTP_Atol(STRPTR String);

/* LTP_BackFillRoutine.c */
VOID SAVE_DS ASM LTP_BackfillRoutine(REG(a0) struct Hook *Hook, REG(a2) struct RastPort *RPort, REG(a1) struct LayerMsg *Bounds);

/* LTP_BitMap.c */
LONG LTP_GetDepth(struct BitMap *BitMap);
VOID LTP_DeleteBitMap(struct BitMap *BitMap, BOOL Chip);
struct BitMap *LTP_CreateBitMap(LONG Width, LONG Height, LONG Depth, struct BitMap *Friend, BOOL Chip);

/* LTP_BlinkButton.c */
BOOL LTP_BlinkButton(LayoutHandle *handle, struct Gadget *gadget);

/* LTP_CheckGlyph.c */
VOID LTP_DrawCheckGlyph(struct RastPort *RPort, LONG Left, LONG Top, struct CheckGlyph *Glyph, BOOL Selected);
VOID LTP_DeleteCheckGlyph(struct CheckGlyph *Glyph);
struct CheckGlyph *LTP_CreateCheckGlyph(LONG Width, LONG Height, struct BitMap *Friend, LONG BackPen, LONG GlyphPen);

/* LTP_Clone.c */
VOID LTP_CloneScreen(struct LayoutHandle *Handle, LONG Width, LONG Height);
BOOL LTP_PrepareCloning(struct LayoutHandle *Handle);

/* LTP_ConvertNum.c */
BOOL LTP_ConvertNum(BOOL negAllowed, STRPTR buffer, LONG *value);

/* LTP_CorrectItemList.c */
BOOL LTP_CorrectItemList(RootMenu *Root, ItemNode *First, ItemNode *Last);

/* LTP_CreateExtraObject.c */
struct Gadget *LTP_CreateExtraObject(LayoutHandle *handle, ObjectNode *parentNode, struct Gadget *parentGadget, struct NewGadget *ng, LONG imageType, LONG incAmount);

/* LTP_CreateGadgets.c */
VOID LTP_CreateGadgets(LayoutHandle *Handle, struct IBox *Bounds, LONG Left, LONG Top, LONG PlusWidth, LONG PlusHeight);

/* LTP_CreateMenuTagList.c */
BOOL LTP_CreateMenuTagList(RootMenu *Root, LONG *ErrorPtr, struct TagItem *TagList);

/* LTP_CreateMenuTemplate.c */
BOOL LTP_CreateMenuTemplate(RootMenu *Root, LONG *ErrorPtr, struct NewMenu *MenuTemplate);

/* LTP_CreateObjectNode.c */
ObjectNode *LTP_CreateObjectNode(LayoutHandle *handle, LONG type, ULONG id, STRPTR label);

/* LTP_DefaultEditRoutine.c */
ULONG SAVE_DS ASM LTP_DefaultEditRoutine(REG(a0) struct Hook *Hook, REG(a2) struct SGWork *Work, REG(a1) Msg msg);

/* LTP_DefaultHistoryHook.c */
ULONG SAVE_DS ASM LTP_DefaultHistoryHook(REG(a0) struct Hook *Hook, REG(a2) struct Gadget *Gadget, REG(a1) STRPTR NewString);

/* LTP_Delay.c */
VOID LTP_Delay(ULONG Seconds, ULONG Micros);

/* LTP_DeleteObjectNode.c */
VOID LTP_DeleteObjectNode(LayoutHandle *handle, ObjectNode *node);

/* LTP_DetermineSize.c */
LONG LTP_GetPickerWidth(LONG fontHeight, LONG aspectX, LONG aspectY);
LONG LTP_GetPickerSize(LayoutHandle *Handle);
VOID LTP_DetermineSize(LayoutHandle *Handle, ObjectNode *Node);

/* LTP_Draw.c */
VOID LTP_ResetRenderInfo(struct RastPort *RPort);
VOID LTP_GhostBox(struct RastPort *RPort, LONG Left, LONG Top, LONG Width, LONG Height, UWORD Pen);
VOID LTP_EraseBox(struct RastPort *rp, LONG left, LONG top, LONG width, LONG height);
VOID LTP_FillBox(struct RastPort *rp, LONG left, LONG top, LONG width, LONG height);
VOID LTP_DrawLine(struct RastPort *rp, LONG x0, LONG y0, LONG x1, LONG y1);
VOID LTP_PolyDraw(struct RastPort *rp, LONG totalPairs, LONG left, LONG top, ...);
VOID LTP_RenderBevel(struct RastPort *rp, UWORD *pens, LONG left, LONG top, LONG width, LONG height, BOOL recessed, WORD thickness);
VOID LTP_DrawBevel(LayoutHandle *handle, LONG left, LONG top, LONG width, LONG height);
VOID LTP_DrawBevelBox(LayoutHandle *handle, ObjectNode *node);
VOID LTP_PrintText(struct RastPort *rp, STRPTR text, LONG textLen, LONG x, LONG y);
VOID LTP_DrawGroove(LayoutHandle *handle, LONG left, LONG top, LONG width, LONG height, LONG from, LONG to);
VOID LTP_DrawGroupLabel(LayoutHandle *handle, ObjectNode *node);
VOID LTP_DrawGroupFrame(LayoutHandle *handle, ObjectNode *node);

/* LTP_DrawBackFore.c */
VOID LTP_DrawBackFore(struct RastPort *RPort, BOOL Back, LONG Left, LONG Top, LONG Width, LONG Height);

/* LTP_DrawBox.c */
VOID LTP_DrawMultiLineButton(struct RastPort *RPort, LONG Left, LONG Top, LONG Width, LONG Height, struct ImageInfo *ImageInfo, BOOL Bold);
VOID LTP_DrawBox(struct RastPort *rp, struct DrawInfo *drawInfo, LONG left, LONG top, LONG width, LONG height, BOOL selected, BOOL ghosted, ImageInfo *imageInfo);

/* LTP_DrawGauge.c */
VOID LTP_GaugeFill(struct RastPort *rp, LONG pen, ObjectNode *node, LONG left, LONG right, LONG height);
VOID LTP_DrawTick(LayoutHandle *Handle, ObjectNode *Node, LONG Left, LONG Top);
VOID LTP_DrawGauge(LayoutHandle *handle, ObjectNode *node, LONG percent, BOOL fullRefresh);

/* LTP_DrawGroup.c */
VOID LTP_DrawObjectLabel(LayoutHandle *Handle, ObjectNode *Node);
VOID LTP_DrawGroup(LayoutHandle *Handle, ObjectNode *Group);

/* LTP_DrawIncrementer.c */
VOID LTP_DrawIncrementer(struct RastPort *rp, struct DrawInfo *drawInfo, BOOL leftDirection, LONG left, LONG top, LONG width, LONG height);

/* LTP_DrawPalette.c */
VOID LTP_DrawPalette(struct LayoutHandle *Handle, struct ObjectNode *Node);

/* LTP_DrawPicker.c */
VOID LTP_DrawPicker(struct RastPort *RPort, BOOL UpDirection, LONG Left, LONG Top, LONG Width, LONG Height);
VOID LTP_DrawAdjustedPicker(struct RastPort *rp, BOOL upDirection, LONG left, LONG top, LONG width, LONG height, LONG aspectX, LONG aspectY);

/* LTP_DrawPrevNext.c */
VOID LTP_DrawPrevNext(struct RastPort *RPort, BOOL Prev, LONG Left, LONG Top, LONG Width, LONG Height);

/* LTP_DrawTapeButton.c */
VOID LTP_DrawTapeButton(struct RastPort *RPort, ImageInfo *imageInfo, LONG Left, LONG Top, LONG Width, LONG Height, LONG AspectX, LONG AspectY, LONG Background);

/* LTP_FillMenu.c */
VOID LTP_FillSub(ULONG MenuID, ULONG ItemID, struct MenuItem *Item);
VOID LTP_FillItem(ULONG MenuID, struct MenuItem *Item);
VOID LTP_FillMenu(struct Menu *Menu);

/* LTP_Find.c */
LONG LTP_Find_Clicked_Item(LayoutHandle *handle, ObjectNode *radio, LONG x, LONG y);
ObjectNode *LTP_FindNode_Position(ObjectNode *group, LONG x, LONG y);
ObjectNode *LTP_FindNode(LayoutHandle *handle, LONG id);
struct Gadget *LTP_FindGadget(LayoutHandle *handle, LONG id);
struct Gadget *LTP_FindGadgetLinear(LayoutHandle *handle, LONG id);

/* LTP_FixExtraLabel.c */
VOID LTP_FixExtraLabel(RootMenu *Root, LONG *ErrorPtr);

/* LTP_GetCommandWidth.c */
LONG LTP_GetCommandWidth(RootMenu *Root, ItemNode *Item);

/* LTP_GetDisplayClip.c */
VOID LTP_GetDisplayClip(struct Screen *screen, WORD *left, WORD *top, WORD *width, WORD *height);

/* LTP_GlyphSetup.c */
VOID LTP_GetDefaultFont(struct TTextAttr *TextAttr);
struct TextFont *LTP_OpenFont(struct TextAttr *TextAttr);
BOOL LTP_GlyphSetup(struct LayoutHandle *Handle, struct TextAttr *TextAttr);

/* LTP_HandleHistory.c */
VOID LTP_HandleHistory(struct SGWork *Work);

/* LTP_HexEditRoutine.c */
ULONG SAVE_DS ASM LTP_HexEditRoutine(REG(a0) struct Hook *hook, REG(a2) struct SGWork *sgw, REG(a1) Msg msg);

/* LTP_ImageClass.c */
ULONG SAVE_DS ASM LTP_ImageDispatch(REG(a0) struct IClass *class, REG(a2) Object *object, REG(a1) Msg msg);

/* LTP_InitIText.c */
VOID LTP_InitIText(RootMenu *Root, struct IntuiText *IText);

/* LTP_LayoutGadgets.c */
VOID LTP_LayoutGadgets(LayoutHandle *Handle, ObjectNode *Group, LONG Left, LONG Top, LONG PlusWidth, LONG PlusHeight);

/* LTP_LayoutGroup.c */
VOID LTP_LayoutGroup(LayoutHandle *Handle, ObjectNode *Group);

/* LTP_LayoutMenu.c */
BOOL LTP_LayoutMenu(RootMenu *Root, LONG ExtraFront, LONG ExtraSpace);

/* LTP_LevelClass.c */
LONG LTP_QuerySliderSize(struct DrawInfo *dri, WORD Reference, WORD freedom, WORD useTicks);
VOID LTP_LevelGadgetDrawLabel(LayoutHandle *Handle, ObjectNode *Node, BOOL FullRefresh);
LONG LTP_GetCurrentLevel(ObjectNode *Node);
ULONG SAVE_DS ASM LTP_LevelClassDispatcher(REG(a0) Class *class, REG(a2) Object *object, REG(a1) Msg msg);

/* LTP_MakeItem.c */
ItemNode *LTP_MakeItem(RootMenu *Root, struct NewMenu *Template);

/* LTP_MakeMenu.c */
MenuNode *LTP_MakeMenu(RootMenu *Root, MenuNode *Menu, struct NewMenu *Template);

/* LTP_Memory.c */
APTR LTP_Alloc(LayoutHandle *handle, ULONG amount);
VOID LTP_Free(LayoutHandle *handle, APTR mem, ULONG memsize);

/* LTP_MoveToWindow.c */
VOID LTP_MoveToWindow(LayoutHandle *handle);

/* LTP_NewMenu.c */
RootMenu *LTP_NewMenu(struct Screen *Screen, struct TextAttr *TextAttr, struct Image *AmigaGlyph, struct Image *CheckGlyph, LONG *ErrorPtr);

/* LTP_PasswordEditRoutine.c */
ULONG SAVE_DS ASM LTP_PasswordEditRoutine(REG(a0) struct Hook *Hook, REG(a2) struct SGWork *Work, REG(a1) Msg msg);

/* LTP_PlaceGroups.c */
VOID LTP_PlaceGroups(LayoutHandle *handle, ObjectNode *group, LONG left, LONG top);

/* LTP_PopupClass.c */
ULONG SAVE_DS ASM LTP_PopupClassDispatcher(REG(a0) struct IClass *class, REG(a2) Object *object, REG(a1) Msg msg);

/* LTP_PrintBoxLine.c */
BOOL LTP_PrintLinePadded(LayoutHandle *Handle, LONG Left, LONG Top, LONG Space, STRPTR Line, LONG Len, LONG textPen, LONG backPen);
VOID LTP_PrintLine(LayoutHandle *handle, LONG alignType, LONG left, LONG top, LONG space, STRPTR line, LONG len, LONG textPen, LONG backPen);
VOID LTP_PrintBoxLine(LayoutHandle *handle, ObjectNode *node, LONG index);

/* LTP_PrintLabel.c */
VOID LTP_PrintLabel(LayoutHandle *handle, ObjectNode *node, LONG left, LONG top);

/* LTP_RenderArrow.c */
VOID LTP_RenderArrow(struct RastPort *RPort, BOOL LeftDirection, LONG Left, LONG Top, LONG Width, LONG Height);

/* LTP_RenderCircle.c */
VOID LTP_RenderCircle(struct RastPort *RPort, LONG Left, LONG Top, LONG Radius, LONG AspectX, LONG AspectY);

/* LTP_Rescale.c */
VOID LTP_ResetListViewTextAttrs(ObjectNode *group);
VOID LTP_Rescale(LayoutHandle *handle, BOOL trimWidth, BOOL trimHeight);

/* LTP_ResetGroups.c */
VOID LTP_ResetGroups(ObjectNode *group);

/* LTP_RPortAttrs.c */
VOID LTP_SetPens(struct RastPort *rp, LONG apen, LONG bpen, LONG mode);
VOID LTP_SetAPen(struct RastPort *rp, LONG apen);
VOID LTP_SetFont(LayoutHandle *handle, struct TextFont *font);

/* LTP_SearchKeys.c */
VOID LTP_SearchKeys(LayoutHandle *handle, ObjectNode *group);

/* LTP_SelectKeys.c */
VOID LTP_SelectKeys(LayoutHandle *handle, ObjectNode *group);

/* LTP_ShrinkMenu.c */
VOID LTP_ShrinkMenu(RootMenu *Root, ItemNode *First, ItemNode *Last, LONG Mask);

/* LTP_SizeDimensions.c */
ULONG LTP_GetSizeWidth(struct LayoutHandle *handle);
ULONG LTP_GetSizeHeight(struct LayoutHandle *handle);

/* LTP_Spread.c */
VOID LTP_Spread(LayoutHandle *Handle, ObjectNode *Group, LONG Width, LONG Height);

/* LTP_SPrintf.c */
VOID SPrintf(STRPTR buffer, STRPTR formatString, ...);

/* LTP_Storage.c */
VOID LTP_GetStorage(ObjectNode *Node);
VOID LTP_PutStorage(ObjectNode *Node);
VOID LTP_CopyFraction(STRPTR To, STRPTR From);

/* LTP_TabClass.c */
BOOL LTP_ObtainTabSize(struct IBox *Box, ...);
ULONG SAVE_DS ASM LTP_TabClassDispatcher(REG(a0) struct IClass *class, REG(a2) Object *object, REG(a1) Msg msg);

/* LT_Activate.c */
VOID LIBENT LT_Activate(REG(a0) LayoutHandle *handle, REG(d0) LONG id);

/* LT_Build.c */
struct Window *LIBENT LT_BuildA(REG(a0) LayoutHandle *handle, REG(a1) struct TagItem *TagParams);
VOID LTP_SelectInitialActiveGadget(LayoutHandle *Handle);
struct Window *LT_Layout(LayoutHandle *handle, STRPTR title, struct IBox *bounds, LONG extraWidth, LONG extraHeight, ULONG IDCMP, LONG align, ...);
struct Window *LIBENT LT_LayoutA(REG(a0) LayoutHandle *handle, REG(a1) STRPTR title, REG(a2) struct IBox *bounds, REG(d0) LONG extraWidth, REG(d1) LONG extraHeight, REG(d2) ULONG IDCMP, REG(d3) LONG align, REG(a3) struct TagItem *TagParams);
struct Window *LT_Build(LayoutHandle *Handle, ...);
APTR LIBENT LT_GetWindowUserData(REG(a0) struct Window *Window, REG(a1) APTR DefaultValue);

/* LT_CreateHandle.c */
LayoutHandle *LIBENT LT_CreateHandle(REG(a0) struct Screen *Screen, REG(a1) struct TextAttr *Font);
LayoutHandle *LT_CreateHandleTags(struct Screen *Screen, ...);
LayoutHandle *LIBENT LT_CreateHandleTagList(REG(a0) struct Screen *Screen, REG(a1) struct TagItem *TagList);

/* LT_DeleteHandle.c */
VOID LTP_DisposeGadgets(struct LayoutHandle *Handle);
VOID LIBENT LT_DeleteHandle(REG(a0) LayoutHandle *Handle);

/* LT_DisposeMenu.c */
VOID LIBENT LT_DisposeMenu(REG(a0) struct Menu *Menu);

/* LT_FindMenuCommand.c */
struct MenuItem *LIBENT LT_FindMenuCommand(REG(a0) struct Menu *Menu, REG(d0) UWORD MsgCode, REG(d1) UWORD MsgQualifier, REG(a1) struct Gadget *MsgGadget);

/* LT_GetAttributes.c */
LONG LT_GetAttributes(LayoutHandle *Handle, LONG ID, ...);
LONG LIBENT LT_GetAttributesA(REG(a0) LayoutHandle *Handle, REG(d0) LONG ID, REG(a1) struct TagItem *TagList);

/* LT_GetCode.c */
LONG LIBENT LT_GetCode(REG(d0) ULONG MsgQualifier, REG(d1) ULONG MsgClass, REG(d2) UWORD MsgCode, REG(a0) struct Gadget *MsgGadget);

/* LT_GetMenuItem.c */
struct MenuItem *LIBENT LT_GetMenuItem(REG(a0) struct Menu *Menu, REG(d0) ULONG ID);

/* LT_HandleInput.c */
VOID LIBENT LT_HandleInput(REG(a0) LayoutHandle *Handle, REG(d0) ULONG MsgQualifier, REG(a1) ULONG *MsgClass, REG(a2) UWORD *MsgCode, REG(a3) struct Gadget **MsgGadget);

/* LT_IMsg.c */
struct IntuiMessage *LIBENT LT_GetIMsg(REG(a0) struct LayoutHandle *Handle);
VOID LIBENT LT_ReplyIMsg(REG(a0) struct IntuiMessage *Msg);

/* LT_InitExit.c */
BOOL LIBENT LT_Init(VOID);
VOID LIBENT LT_Exit(VOID);

/* LT_LabelWidth.c */
LONG LIBENT LT_LabelWidth(REG(a0) LayoutHandle *handle, REG(a1) STRPTR label);
LONG LIBENT LT_LabelChars(REG(a0) LayoutHandle *handle, REG(a1) STRPTR label);

/* LT_LayoutMenus.c */
struct Menu *LT_LayoutMenus(LayoutHandle *handle, struct NewMenu *menuTemplate, ...);
struct Menu *LIBENT LT_LayoutMenusA(REG(a0) LayoutHandle *handle, REG(a1) struct NewMenu *menuTemplate, REG(a2) struct TagItem *TagParams);

/* LT_LevelWidth.c */
VOID LTP_LevelWidth(LayoutHandle *handle, STRPTR levelFormat, DISPFUNC dispFunc, LONG min, LONG max, LONG *maxWidth, LONG *maxLen, BOOL fullCheck);
VOID LIBENT LT_LevelWidth(REG(a0) LayoutHandle *handle, REG(a1) STRPTR levelFormat, REG(a2) DISPFUNC dispFunc, REG(d0) LONG min, REG(d1) LONG max, REG(a3) LONG *maxWidth, REG(a5) LONG *maxLen, REG(d2) BOOL fullCheck);
VOID LIBENT LT_NewLevelWidth(REG(a0) LayoutHandle *handle, REG(a1) STRPTR levelFormat, REG(a2) DISPFUNC dispFunc, REG(d0) LONG min, REG(d1) LONG max, REG(a3) LONG *maxWidth, REG(d3) LONG *maxLen, REG(d2) BOOL fullCheck);

/* LT_LockWindow.c */
VOID LTP_DeleteWindowLock(LockNode *Node);
VOID LIBENT LT_LockWindow(REG(a0) struct Window *window);
VOID LIBENT LT_UnlockWindow(REG(a0) struct Window *window);
VOID LIBENT LT_DeleteWindowLock(REG(a0) struct Window *window);

/* LT_MenuControlTagList.c */
VOID LIBENT LT_MenuControlTagList(REG(a0) struct Window *Window, REG(a1) struct Menu *IntuitionMenu, REG(a2) struct TagItem *Tags);
VOID LT_MenuControlTags(struct Window *Window, struct Menu *Menu, ...);

/* LT_New.c */
VOID LTP_ReplaceLabelShortcut(LayoutHandle *Handle, ObjectNode *Node);
VOID LT_AddL(LayoutHandle *handle, LONG type, ULONG labelID, LONG id, ...);
VOID LT_New(LayoutHandle *handle, ...);
VOID LT_Add(LayoutHandle *Handle, LONG Type, STRPTR Label, LONG ID, ...);
VOID LIBENT LT_AddA(REG(a0) LayoutHandle *Handle, REG(d0) LONG Type, REG(d1) STRPTR Label, REG(d2) LONG ID, REG(a1) struct TagItem *TagList);
VOID LIBENT LT_NewA(REG(a0) LayoutHandle *handle, REG(a1) struct TagItem *tagList);
VOID LIBENT LT_EndGroup(REG(a0) LayoutHandle *handle);

/* LT_NewMenuTagList.c */
struct Menu *LIBENT LT_NewMenuTagList(REG(a0) struct TagItem *TagList);
struct Menu *LT_NewMenuTags(Tag FirstTag, ...);

/* LT_NewMenuTemplate.c */
struct Menu *LIBENT LT_NewMenuTemplate(REG(a0) struct Screen *Screen, REG(a1) struct TextAttr *TextAttr, REG(a2) struct Image *AmigaGlyph, REG(a3) struct Image *CheckGlyph, REG(d0) LONG *ErrorPtr, REG(d1) struct NewMenu *MenuTemplate);

/* LT_PressButton.c */
BOOL LIBENT LT_PressButton(REG(a0) LayoutHandle *handle, REG(d0) LONG id);

/* LT_Rebuild.c */
VOID LTP_Erase(LayoutHandle *Handle);
BOOL LIBENT LT_RebuildTagList(REG(a0) LayoutHandle *handle, REG(d0) BOOL clear, REG(a1) struct TagItem *TagParams);
BOOL LT_RebuildTags(LayoutHandle *Handle, BOOL Clear, ...);
BOOL LIBENT LT_Rebuild(REG(a0) LayoutHandle *handle, REG(a1) struct IBox *bounds, REG(a2) LONG extraWidth, REG(d0) LONG extraHeight, REG(d1) BOOL clear);

/* LT_Refresh.c */
VOID LIBENT LT_BeginRefresh(REG(a0) LayoutHandle *handle);
VOID LIBENT LT_EndRefresh(REG(a0) LayoutHandle *handle, REG(d0) BOOL complete);
VOID LIBENT LT_Refresh(REG(a0) LayoutHandle *handle);
VOID LIBENT LT_CatchUpRefresh(REG(a0) LayoutHandle *handle);

/* LT_SetAttributes.c */
VOID LT_SetAttributes(LayoutHandle *handle, LONG id, ...);
VOID LTP_AddAllAndRefreshThisGadget(LayoutHandle *Handle, struct Gadget *Gadget);
VOID LTP_FixState(LayoutHandle *Handle, BOOL State, struct Gadget *Gadget, UWORD Bit);
BOOL LTP_NotifyPager(LayoutHandle *Handle, LONG ID, LONG Page);
VOID LIBENT LT_SetAttributesA(REG(a0) LayoutHandle *handle, REG(d0) LONG id, REG(a1) struct TagItem *TagList);

/* LT_ShowWindow.c */
VOID LIBENT LT_ShowWindow(REG(a0) LayoutHandle *handle, REG(a1) BOOL activate);

/* LT_UpdateStrings.c */
VOID LIBENT LT_UpdateStrings(REG(a0) LayoutHandle *Handle);

#endif	/* _GTLAYOUT_LIBPROTOS_H */
