/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number: 1d51 0x9d510030 to 0x9d5100A0
                                           0x9d5100C0 to 0x9d5100FF

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

/* NList_mcc.c */

extern void release_pen(struct MUI_RenderInfo *mri, ULONG *pen);
extern void obtain_pen(struct MUI_RenderInfo *mri, ULONG *pen, struct MUI_PenSpec *ps);
extern ULONG mNL_New(struct IClass *cl,Object *obj,struct opSet *msg);
extern ULONG mNL_Dispose(struct IClass *cl,Object *obj,Msg msg);
extern ULONG mNL_Setup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg);
extern ULONG mNL_Cleanup(struct IClass *cl,Object *obj,struct MUIP_Cleanup *msg);

/* NList_mcc0.c */

/*
**	Dispatcher
*/

/* NList_mcc1.c */

extern ULONG mNL_AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg);
extern ULONG mNL_Notify(struct IClass *cl,Object *obj,struct MUIP_Notify *msg);
extern ULONG mNL_Set(struct IClass *cl,Object *obj,Msg msg);
extern ULONG mNL_Get(struct IClass *cl,Object *obj,struct opGet *msg);

/* NList_mcc2.c */

extern ULONG mNL_HandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg);
extern ULONG mNL_HandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg);
extern ULONG mNL_CreateDragImage(struct IClass *cl,Object *obj,struct MUIP_CreateDragImage *msg);
extern ULONG mNL_DeleteDragImage(struct IClass *cl,Object *obj,struct MUIP_DeleteDragImage *msg);
extern BOOL  NL_Prop_First_Adjust(Object *obj,struct NLData *data);
extern ULONG mNL_Trigger(struct IClass *cl,Object *obj,Msg msg);

/* NList_mcc3.c */

extern void NL_SetObjInfos(Object *obj,struct NLData *data,BOOL setall);
extern ULONG mNL_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg);
extern ULONG mNL_DropDraw(struct IClass *cl,Object *obj,struct MUIP_NList_DropDraw *msg);

/* NList_mcc4.c */

extern BOOL DontDoColumn(struct NLData *data,LONG ent,WORD column);
extern void ParseColumn(Object *obj,struct NLData *data,WORD column,ULONG mypen);
extern void WidthColumn(Object *obj,struct NLData *data,WORD column,WORD updinfo);
extern void AllParseColumns(Object *obj,struct NLData *data);
extern void FreeAffInfo(Object *obj,struct NLData *data);
extern BOOL NeedAffInfo(Object *obj,struct NLData *data,WORD niask);
extern void NL_GetDisplayArray(Object *obj,struct NLData *data,LONG ent);
extern void FindCharInColumn(Object *obj,struct NLData *data,LONG ent,WORD column,WORD xoffset,WORD *charxoffset,WORD *charnum);
extern void NL_DoWrapAll(Object *obj,struct NLData *data,BOOL force,BOOL update);
extern void AllWidthColumns(Object *obj,struct NLData *data);
extern void NL_SetColsAdd(Object *obj,struct NLData *data,LONG ent,WORD addimages);
extern void NL_SetColsRem(Object *obj,struct NLData *data,LONG ent);

/* NList_mcc5.c */

extern void NL_SetCols(Object *obj,struct NLData *data);
extern LONG NL_DoNotifies(Object *obj,struct NLData *data,LONG which);
extern void NL_UpdateScrollersValues(Object *obj,struct NLData *data);
extern ULONG NL_UpdateScrollers(Object *obj,struct NLData *data,BOOL force);
extern LONG NL_DrawQuietBG(Object *obj,struct NLData *data,LONG dowhat,LONG bg);
extern void NL_Select(Object *obj,struct NLData *data,LONG dowhat,LONG ent,BYTE sel);
extern void ScrollVert(Object *obj,struct NLData *data,WORD dy,LONG LPVisible);
extern void ScrollHoriz(Object *obj,struct NLData *data,WORD dx,LONG LPVisible);
extern LONG  NL_ColToColumn(Object *obj,struct NLData *data,LONG col);
extern LONG  NL_ColumnToCol(Object *obj,struct NLData *data,LONG column);
extern LONG  NL_SetCol(Object *obj,struct NLData *data,LONG column,LONG col);
extern LONG  NL_ColWidth(Object *obj,struct NLData *data,LONG col,LONG width);
extern BYTE *NL_Columns(Object *obj,struct NLData *data,BYTE *columns);
extern ULONG mNL_ColToColumn(struct IClass *cl,Object *obj,struct MUIP_NList_ColToColumn *msg);
extern ULONG mNL_ColumnToCol(struct IClass *cl,Object *obj,struct MUIP_NList_ColumnToCol *msg);
extern ULONG mNL_SetColumnCol(struct IClass *cl,Object *obj,struct MUIP_NList_SetColumnCol *msg);
extern ULONG mNL_List_ColWidth(struct IClass *cl,Object *obj,struct MUIP_NList_ColWidth *msg);
extern ULONG mNL_ContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg);
extern ULONG mNL_ContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg);
extern void NL_Stack_Alert(Object *obj,struct NLData *data,LONG why);

/* NList_mcc6.c */

extern WORD DrawTitle(Object *obj,struct NLData *data,LONG minx,LONG maxx,WORD hfirst);
extern void DrawOldLine(Object *obj,struct NLData *data,LONG ent,LONG minx,LONG maxx,WORD hfirst);
extern WORD DrawLines(Object *obj,struct NLData *data,LONG e1,LONG e2,LONG minx,LONG maxx,WORD hfirst,WORD hmax,WORD small,BOOL do_extrems,WORD not_all);
extern LONG DrawText(Object *obj,struct NLData *data,LONG ent,LONG x,LONG y,LONG minx,LONG maxx,ULONG mypen,LONG dxpermit,BOOL forcepen);
extern LONG DrawDragText(Object *obj,struct NLData *data,BOOL draw);
extern void DisposeDragRPort(Object *obj,struct NLData *data);
extern struct RastPort *CreateDragRPort(Object *obj,struct NLData *data,LONG numlines,LONG first,LONG last);

/* NList_func.c */

extern void NL_SegChanged(struct NLData *data,LONG ent1,LONG ent2);
extern void NL_Changed(struct NLData *data,LONG ent);
extern void NL_UnSelectAll(Object *obj,struct NLData *data,LONG untouch_ent);
extern void UnSelectCharSel(Object *obj,struct NLData *data,BOOL redraw);
extern void SelectFirstPoint(Object *obj,struct NLData *data,WORD x,WORD y);
extern void SelectSecondPoint(Object *obj,struct NLData *data,WORD x,WORD y);
extern void NL_List_First(Object *obj,struct NLData *data,LONG lf,struct TagItem *tag);
extern void NL_List_Active(Object *obj,struct NLData *data,LONG la,struct TagItem *tag,LONG newactsel,LONG acceptsame);
extern void NL_List_Horiz_First(Object *obj,struct NLData *data,LONG hf,struct TagItem *tag);
extern LONG NList_Compare(Object *obj,struct NLData *data,APTR s1,APTR s2);
extern ULONG NL_List_SelectChar(Object *obj,struct NLData *data,LONG pos,LONG seltype,LONG *state);
extern ULONG NL_List_Select(Object *obj,struct NLData *data,LONG pos,LONG pos2,LONG seltype,LONG *state);
extern ULONG NL_List_TestPosOld(Object *obj,struct NLData *data,LONG x,LONG y,struct MUI_List_TestPos_Result *res);
extern ULONG NL_List_TestPos(Object *obj,struct NLData *data,LONG x,LONG y,struct MUI_NList_TestPos_Result *res);

extern ULONG mNL_List_GetEntry(struct IClass *cl,Object *obj,struct  MUIP_NList_GetEntry *msg);
extern ULONG mNL_List_GetEntryInfo(struct IClass *cl,Object *obj,struct  MUIP_NList_GetEntryInfo *msg);
extern ULONG mNL_List_Jump(struct IClass *cl,Object *obj,struct  MUIP_NList_Jump *msg);
extern ULONG mNL_List_Select(struct IClass *cl,Object *obj,struct  MUIP_NList_Select *msg);
extern ULONG mNL_List_TestPos(struct IClass *cl,Object *obj,struct MUIP_NList_TestPos *msg);
extern ULONG mNL_List_TestPosOld(struct IClass *cl,Object *obj,struct MUIP_List_TestPos *msg);
extern ULONG mNL_List_Redraw(struct IClass *cl,Object *obj,struct MUIP_NList_Redraw *msg);
extern ULONG mNL_List_RedrawEntry(struct IClass *cl,Object *obj,struct MUIP_NList_RedrawEntry *msg);
extern ULONG mNL_List_NextSelected(struct IClass *cl,Object *obj,struct MUIP_NList_NextSelected *msg);
extern ULONG mNL_List_PrevSelected(struct IClass *cl,Object *obj,struct MUIP_NList_PrevSelected *msg);
extern ULONG mNL_List_GetSelectInfo(struct IClass *cl,Object *obj,struct MUIP_NList_GetSelectInfo *msg);
extern ULONG mNL_List_DoMethod(struct IClass *cl,Object *obj,struct MUIP_NList_DoMethod *msg);
extern ULONG mNL_List_GetPos(struct IClass *cl,Object *obj,struct MUIP_NList_GetPos *msg);

/* NList_func2.c */

extern LONG  NL_GetSelects(struct NLData *data,Object *obj,LONG ent);
extern BOOL  NL_InsertTmpLine(struct NLData *data,Object *obj,LONG pos);
extern void  NL_DeleteTmpLine(struct NLData *data,Object *obj,LONG pos);
extern ULONG NL_List_Sort(Object *obj,struct NLData *data);
extern ULONG NL_List_Insert(struct NLData *data,Object *obj,APTR *entries,LONG count,LONG pos,LONG wrapcol,LONG align,ULONG flags);
extern ULONG NL_List_Replace(struct NLData *data,Object *obj,APTR entry,LONG pos,LONG wrapcol,LONG align);
extern ULONG NL_List_Clear(struct NLData *data,Object *obj);
extern ULONG NL_List_Remove(struct NLData *data,Object *obj,LONG pos);
extern ULONG NL_List_Exchange(struct NLData *data,Object *obj,LONG pos1,LONG pos2);
extern ULONG NL_List_Move_Selected(struct NLData *data,Object *obj,LONG to);
extern ULONG NL_List_Move(struct NLData *data,Object *obj,LONG from,LONG to);

extern ULONG mNL_List_Sort(struct IClass *cl,Object *obj,struct  MUIP_NList_Sort *msg);
extern ULONG mNL_List_Sort2(struct IClass *cl,Object *obj,struct  MUIP_NList_Sort2 *msg);
extern ULONG mNL_List_Sort3(struct IClass *cl,Object *obj,struct  MUIP_NList_Sort3 *msg);
extern ULONG mNL_List_Insert(struct IClass *cl,Object *obj,struct  MUIP_NList_Insert *msg);
extern ULONG mNL_List_InsertSingle(struct IClass *cl,Object *obj,struct  MUIP_NList_InsertSingle *msg);
extern ULONG mNL_List_InsertWrap(struct IClass *cl,Object *obj,struct  MUIP_NList_InsertWrap *msg);
extern ULONG mNL_List_InsertSingleWrap(struct IClass *cl,Object *obj,struct  MUIP_NList_InsertSingleWrap *msg);
extern ULONG mNL_List_ReplaceSingle(struct IClass *cl,Object *obj,struct  MUIP_NList_ReplaceSingle *msg);
extern ULONG mNL_List_Exchange(struct IClass *cl,Object *obj,struct MUIP_NList_Exchange *msg);
extern ULONG mNL_List_Move(struct IClass *cl,Object *obj,struct MUIP_NList_Move *msg);
extern ULONG mNL_List_Clear(struct IClass *cl,Object *obj,struct  MUIP_NList_Clear *msg);
extern ULONG mNL_List_Remove(struct IClass *cl,Object *obj,struct MUIP_NList_Remove *msg);
extern ULONG mNL_DragQuery(struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg);
extern ULONG mNL_DragBegin(struct IClass *cl,Object *obj,struct MUIP_DragBegin *msg);
extern ULONG mNL_DragReport(struct IClass *cl,Object *obj,struct MUIP_DragReport *msg);
extern ULONG mNL_DragFinish(struct IClass *cl,Object *obj,struct MUIP_DragFinish *msg);
extern ULONG mNL_DragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg);
extern ULONG mNL_DropType(struct IClass *cl,Object *obj,struct MUIP_NList_DropType *msg);
extern ULONG mNL_DropEntryDrawErase(struct IClass *cl,Object *obj,struct MUIP_NList_DropEntryDrawErase *msg);

/* NList_func3.c */

extern ULONG MyCallHookPkt(Object *obj,BOOL hdata,struct Hook *hook,APTR object,APTR message);
extern ULONG STDARGS VARARGS68K MyCallHookPktA(Object *obj, struct Hook *hook, ...);
extern LONG DeadKeyConvert(struct NLData *data,struct IntuiMessage *msg,UBYTE *buf,LONG bufsize,struct KeyMap *kmap);
extern char *ltoa(ULONG val, char *buffer, int len);

//$$$Sensei: new memory handling functions.
//sba: Removed some unused functions
extern APTR  NL_Pool_Create(ULONG, ULONG);
extern VOID  NL_Pool_Delete(APTR);
extern APTR  NL_Pool_Internal_Alloc(struct NLData *data, ULONG size);
extern VOID  NL_Pool_Internal_Free(struct NLData *data, APTR memory, ULONG size);

extern APTR	NL2_Malloc2( APTR pool, ULONG size, STRPTR string );
extern VOID	NL2_Free2( APTR pool, APTR memory, STRPTR string );
extern APTR	NL2_Malloc( struct NLData *data, ULONG size, STRPTR string );
extern VOID	NL2_Free( struct NLData *data, APTR memory, STRPTR string );

extern void NL_Free_Format(Object *obj,struct NLData *data);
extern BOOL NL_Read_Format(Object *obj,struct NLData *data,char *strformat,BOOL oldlist);
extern LONG NL_CopyTo(Object *obj,struct NLData *data,LONG pos,char *filename,ULONG clipnum,APTR *entries,struct Hook *hook);

extern ULONG mNL_CopyToClip(struct IClass *cl,Object *obj,struct MUIP_NList_CopyToClip *msg);
extern ULONG mNL_CopyTo(struct IClass *cl,Object *obj,struct MUIP_NList_CopyTo *msg);

/* NList_func4.c */

extern void  NL_SpecPointerColors(LONG blacknum);
extern void  NL_SetPointer(Object *obj,struct NLData *data,LONG type);
extern void  NL_ClearPointer(Object *obj,struct NLData *data);
extern LONG  NL_OnWindow(Object *obj,struct NLData *data,LONG x,LONG y);
extern struct NImgList *GetNImage(Object *obj,struct NLData *data,char *ImgName);
extern void DeleteNImages(Object *obj,struct NLData *data);
extern struct NImgList *GetNImage2(Object *obj,struct NLData *data,APTR imgobj);
extern void DeleteNImages2(Object *obj,struct NLData *data);
extern void GetNImage_Sizes(Object *obj,struct NLData *data);
extern void GetNImage_End(Object *obj,struct NLData *data);
extern void GetImages(Object *obj,struct NLData *data);

extern ULONG NL_CreateImage(Object *obj,struct NLData *data,Object *imgobj,ULONG flags);
extern ULONG NL_DeleteImage(Object *obj,struct NLData *data,APTR listimg);
extern ULONG NL_CreateImages(Object *obj,struct NLData *data);
extern ULONG NL_DeleteImages(Object *obj,struct NLData *data);
extern ULONG NL_UseImage(Object *obj,struct NLData *data,Object *imgobj,LONG imgnum,ULONG flags);
extern ULONG mNL_CreateImage(struct IClass *cl,Object *obj,struct MUIP_NList_CreateImage *msg);
extern ULONG mNL_DeleteImage(struct IClass *cl,Object *obj,struct MUIP_NList_DeleteImage *msg);
extern ULONG mNL_UseImage(struct IClass *cl,Object *obj,struct MUIP_NList_UseImage *msg);

/* Move.c */

extern void  NL_Move(void *dest,void *src,LONG len,long newpos);
extern void  NL_MoveD(void *dest,void *src,LONG len,long newpos);
