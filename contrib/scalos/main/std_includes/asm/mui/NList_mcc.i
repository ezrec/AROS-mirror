;/*
;  NList.mcc (c) Copyright 1996-1997 by Gilles Masson
;  Registered MUI class, Serial Num: 1d51     $9d510030 to $9d5100A0 / $9d5100C0 to $9d5100FF
;  *** use only YOUR OWN Serial Number for your public custom class ***
;  NList_mcc.h
;*/


;/* MUI Prop and Scroller classes stuff which is still not in libraries/mui.h  (in MUI3.8) */
;/* it gives to the prop object it's increment value */
	IFND	MUIA_Prop_DeltaFactor
MUIA_Prop_DeltaFactor:	equ	$80427C5E
	ENDIF


	IFND	MUI_NList_MCC_I
MUI_NList_MCC_I	set	1

	IFND	LIBRARIES_MUI_I
	include <libraries/mui.i>
	ENDIF



;;MUIC_NList:	equ	"NList.mcc"
;;NListObject:	equ	MUI_NewObject(MUIC_NList


;/* Attributes */

MUIA_NList_TypeSelect:			equ	$9d510030 ;/* GM  is.  LONG              */
MUIA_NList_Prop_DeltaFactor:		equ	$9d510031 ;/* GM  ..gn LONG              */
MUIA_NList_Horiz_DeltaFactor:		equ	$9d510032 ;/* GM  ..gn LONG              */

MUIA_NList_Horiz_First:			equ	$9d510033 ;/* GM  .sgn LONG              */
MUIA_NList_Horiz_Visible:		equ	$9d510034 ;/* GM  ..gn LONG              */
MUIA_NList_Horiz_Entries:		equ	$9d510035 ;/* GM  ..gn LONG              */

MUIA_NList_Prop_First:			equ	$9d510036 ;/* GM  .sgn LONG              */
MUIA_NList_Prop_Visible:		equ	$9d510037 ;/* GM  ..gn LONG              */
MUIA_NList_Prop_Entries:		equ	$9d510038 ;/* GM  ..gn LONG              */

MUIA_NList_TitlePen:			equ	$9d510039 ;/* GM  isg  LONG              */
MUIA_NList_ListPen:			equ	$9d51003a ;/* GM  isg  LONG              */
MUIA_NList_SelectPen:			equ	$9d51003b ;/* GM  isg  LONG              */
MUIA_NList_CursorPen:			equ	$9d51003c ;/* GM  isg  LONG              */
MUIA_NList_UnselCurPen:			equ	$9d51003d ;/* GM  isg  LONG              */

MUIA_NList_ListBackground:		equ	$9d51003e ;/* GM  isg  LONG              */
MUIA_NList_TitleBackground:		equ	$9d51003f ;/* GM  isg  LONG              */
MUIA_NList_SelectBackground:		equ	$9d510040 ;/* GM  isg  LONG              */
MUIA_NList_CursorBackground:		equ	$9d510041 ;/* GM  isg  LONG              */
MUIA_NList_UnselCurBackground:		equ	$9d510042 ;/* GM  isg  LONG              */

MUIA_NList_MultiClick:			equ	$9d510043 ;/* GM  ..gn LONG              */

MUIA_NList_DefaultObjectOnClick:		equ	$9d510044 ;/* GM  is.  BOOL              */

MUIA_NList_ClickColumn:			equ	$9d510045 ;/* GM  ..g  LONG              */
MUIA_NList_DefClickColumn:		equ	$9d510046 ;/* GM  isg  LONG              */
MUIA_NList_DoubleClick:			equ	$9d510047 ;/* GM  ..gn LONG              */
MUIA_NList_DragType:			equ	$9d510048 ;/* GM  isg  LONG              */
MUIA_NList_Input:			equ	$9d510049 ;/* GM  isg  BOOL              */
MUIA_NList_MultiSelect:			equ	$9d51004a ;/* GM  is.  LONG              */
MUIA_NList_SelectChange:		equ	$9d51004b ;/* GM  ...n BOOL              */

MUIA_NList_Active:			equ	$9d51004c ;/* GM  isgn LONG              */
MUIA_NList_AdjustHeight:		equ	$9d51004d ;/* GM  i..  BOOL              */
MUIA_NList_AdjustWidth:			equ	$9d51004e ;/* GM  i..  BOOL              */
MUIA_NList_AutoVisible:			equ	$9d51004f ;/* GM  isg  BOOL              */
MUIA_NList_CompareHook:			equ	$9d510050 ;/* GM  is.  struct Hook *     */
MUIA_NList_ConstructHook:		equ	$9d510051 ;/* GM  is.  struct Hook *     */
MUIA_NList_DestructHook:		equ	$9d510052 ;/* GM  is.  struct Hook *     */
MUIA_NList_DisplayHook:			equ	$9d510053 ;/* GM  is.  struct Hook *     */
MUIA_NList_DragSortable:		equ	$9d510054 ;/* GM  isg  BOOL              */
MUIA_NList_DropMark:			equ	$9d510055 ;/* GM  ..g  LONG              */
MUIA_NList_Entries:			equ	$9d510056 ;/* GM  ..gn LONG              */
MUIA_NList_First:			equ	$9d510057 ;/* GM  isgn LONG              */
MUIA_NList_Format:			equ	$9d510058 ;/* GM  isg  STRPTR            */
MUIA_NList_InsertPosition:		equ	$9d510059 ;/* GM  ..gn LONG              */
MUIA_NList_MinLineHeight:		equ	$9d51005a ;/* GM  is.  LONG              */
MUIA_NList_MultiTestHook:		equ	$9d51005b ;/* GM  is.  struct Hook *     */
MUIA_NList_Pool:			equ	$9d51005c ;/* GM  i..  APTR              */
MUIA_NList_PoolPuddleSize:		equ	$9d51005d ;/* GM  i..  ULONG             */
MUIA_NList_PoolThreshSize:		equ	$9d51005e ;/* GM  i..  ULONG             */
MUIA_NList_Quiet:			equ	$9d51005f ;/* GM  .s.  BOOL              */
MUIA_NList_ShowDropMarks:		equ	$9d510060 ;/* GM  isg  BOOL              */
MUIA_NList_SourceArray:			equ	$9d510061 ;/* GM  i..  APTR *            */
MUIA_NList_Title:			equ	$9d510062 ;/* GM  isg  char *            */
MUIA_NList_Visible:			equ	$9d510063 ;/* GM  ..g  LONG              */
MUIA_NList_CopyEntryToClipHook:		equ	$9d510064 ;/* GM  is.  struct Hook *     */
MUIA_NList_KeepActive:			equ	$9d510065 ;/* GM  .s.  Obj *             */
MUIA_NList_MakeActive:			equ	$9d510066 ;/* GM  .s.  Obj *             */
MUIA_NList_SourceString:		equ	$9d510067 ;/* GM  i..  char *            */
MUIA_NList_CopyColumnToClipHook:		equ	$9d510068 ;/* GM  is.  struct Hook *     */
MUIA_NList_ListCompatibility:		equ	$9d510069 ;/* GM  ...  OBSOLETE          */
MUIA_NList_AutoCopyToClip:		equ	$9d51006A ;/* GM  is.  BOOL              */
MUIA_NList_TabSize:			equ	$9d51006B ;/* GM  isg  ULONG             */
MUIA_NList_SkipChars:			equ	$9d51006C ;/* GM  isg  char *            */
MUIA_NList_DisplayRecall:		equ	$9d51006D ;/* GM  .g.  BOOL              */
MUIA_NList_PrivateData:			equ	$9d51006E ;/* GM  isg  APTR              */
MUIA_NList_EntryValueDependent:		equ	$9d51006F ;/* GM  isg  BOOL              */

MUIA_NList_StackCheck:			equ	$9d510097 ;/* GM  i..  BOOL              */
MUIA_NList_WordSelectChars:		equ	$9d510098 ;/* GM  isg  char *            */
MUIA_NList_EntryClick:			equ	$9d510099 ;/* GM  ..gn LONG              */
MUIA_NList_DragColOnly:			equ	$9d51009A ;/* GM  isg  LONG              */
MUIA_NList_TitleClick:			equ	$9d51009B ;/* GM  isgn LONG              */
MUIA_NList_DropType:			equ	$9d51009C ;/* GM  ..g  LONG              */
MUIA_NList_ForcePen:			equ	$9d51009D ;/* GM  isg  LONG              */
MUIA_NList_SourceInsert:		equ	$9d51009E ;/* GM  i..  struct MUIP_NList_InsertWrap *   */
MUIA_NList_TitleSeparator:		equ	$9d51009F ;/* GM  isg  BOOL              */

MUIA_NList_SortType2:			equ	$9d5100ED ;/* GM  isgn LONG              */
MUIA_NList_TitleClick2:			equ	$9d5100EE ;/* GM  isgn LONG              */
MUIA_NList_TitleMark2:			equ	$9d5100EF ;/* GM  isg  LONG              */
MUIA_NList_MultiClickAlone:		equ	$9d5100F0 ;/* GM  ..gn LONG              */
MUIA_NList_TitleMark:			equ	$9d5100F1 ;/* GM  isg  LONG              */
MUIA_NList_DragSortInsert:		equ	$9d5100F2 ;/* GM  ..gn LONG              */
MUIA_NList_MinColSortable:		equ	$9d5100F3 ;/* GM  isg  LONG              */
MUIA_NList_Imports:			equ	$9d5100F4 ;/* GM  isg  LONG              */
MUIA_NList_Exports:			equ	$9d5100F5 ;/* GM  isg  LONG              */
MUIA_NList_Columns:			equ	$9d5100F6 ;/* GM  isgn BYTE *            */
MUIA_NList_LineHeight:			equ	$9d5100F7 ;/* GM  ..gn LONG              */
MUIA_NList_ButtonClick:			equ	$9d5100F8 ;/* GM  ..gn LONG              */
MUIA_NList_CopyEntryToClipHook2:		equ	$9d5100F9 ;/* GM  is.  struct Hook *     */
MUIA_NList_CopyColumnToClipHook2:		equ	$9d5100FA ;/* GM  is.  struct Hook *     */
MUIA_NList_CompareHook2:		equ	$9d5100FB ;/* GM  is.  struct Hook *     */
MUIA_NList_ConstructHook2:		equ	$9d5100FC ;/* GM  is.  struct Hook *     */
MUIA_NList_DestructHook2:		equ	$9d5100FD ;/* GM  is.  struct Hook *     */
MUIA_NList_DisplayHook2:		equ	$9d5100FE ;/* GM  is.  struct Hook *     */
MUIA_NList_SortType:			equ	$9d5100FF ;/* GM  isgn LONG              */


MUIA_NLIMG_EntryCurrent:		equ	MUIA_NList_First   ;/* LONG (special for nlist custom image object) */
MUIA_NLIMG_EntryHeight:			equ	MUIA_NList_Visible ;/* LONG (special for nlist custom image object) */

MUIA_NList_VertDeltaFactor:		equ	MUIA_NList_Prop_DeltaFactor   ;/* OBSOLETE NAME */
MUIA_NList_HorizDeltaFactor:		equ	MUIA_NList_Horiz_DeltaFactor  ;/* OBSOLETE NAME */


;/* Attributes special datas */

MUIV_NList_TypeSelect_Line:		equ	0
MUIV_NList_TypeSelect_Char:		equ	1

MUIV_NList_Font:			equ	-20
MUIV_NList_Font_Little:			equ	-21
MUIV_NList_Font_Fixed:			equ	-22

MUIV_NList_ConstructHook_String:	equ	-1
MUIV_NList_DestructHook_String:		equ	-1

MUIV_NList_Active_Off:			equ	-1
MUIV_NList_Active_Top:			equ	-2
MUIV_NList_Active_Bottom:		equ	-3
MUIV_NList_Active_Up:			equ	-4
MUIV_NList_Active_Down:			equ	-5
MUIV_NList_Active_PageUp:		equ	-6
MUIV_NList_Active_PageDown:		equ	-7

MUIV_NList_First_Top:			equ	-2
MUIV_NList_First_Bottom:		equ	-3
MUIV_NList_First_Up:			equ	-4
MUIV_NList_First_Down:			equ	-5
MUIV_NList_First_PageUp:		equ	-6
MUIV_NList_First_PageDown:		equ	-7
MUIV_NList_First_Up2:			equ	-8
MUIV_NList_First_Down2:			equ	-9
MUIV_NList_First_Up4:			equ	-10
MUIV_NList_First_Down4:			equ	-11

MUIV_NList_Horiz_First_Start:		equ	-2
MUIV_NList_Horiz_First_End:		equ	-3
MUIV_NList_Horiz_First_Left:		equ	-4
MUIV_NList_Horiz_First_Right:		equ	-5
MUIV_NList_Horiz_First_PageLeft:	equ	-6
MUIV_NList_Horiz_First_PageRight:	equ	-7
MUIV_NList_Horiz_First_Left2:		equ	-8
MUIV_NList_Horiz_First_Right2:		equ	-9
MUIV_NList_Horiz_First_Left4:		equ	-10
MUIV_NList_Horiz_First_Right4:		equ	-11

MUIV_NList_MultiSelect_None:		equ	0
MUIV_NList_MultiSelect_Default:		equ	1
MUIV_NList_MultiSelect_Shifted:		equ	2
MUIV_NList_MultiSelect_Always:		equ	3

MUIV_NList_Insert_Top:			equ	0
MUIV_NList_Insert_Active:		equ	-1
MUIV_NList_Insert_Sorted:		equ	-2
MUIV_NList_Insert_Bottom:		equ	-3

MUIV_NList_Remove_First:		equ	0
MUIV_NList_Remove_Active:		equ	-1
MUIV_NList_Remove_Last:			equ	-2
MUIV_NList_Remove_Selected:		equ	-3

MUIV_NList_Select_Off:			equ	0
MUIV_NList_Select_On:			equ	1
MUIV_NList_Select_Toggle:		equ	2
MUIV_NList_Select_Ask:			equ	3

MUIV_NList_GetEntry_Active:		equ	-1
MUIV_NList_GetEntryInfo_Line:		equ	-2

MUIV_NList_Select_Active:		equ	-1
MUIV_NList_Select_All:			equ	-2

MUIV_NList_Redraw_Active:		equ	-1
MUIV_NList_Redraw_All:			equ	-2
MUIV_NList_Redraw_Title:		equ	-3

MUIV_NList_Move_Top:			equ	0
MUIV_NList_Move_Active:			equ	-1
MUIV_NList_Move_Bottom:			equ	-2
MUIV_NList_Move_Next:			equ	-3 ;/* only valid for second parameter (and not with Move_Selected) */
MUIV_NList_Move_Previous:		equ	-4 ;/* only valid for second parameter (and not with Move_Selected) */
MUIV_NList_Move_Selected:		equ	-5 ;/* only valid for first parameter */

MUIV_NList_Exchange_Top:		equ	0
MUIV_NList_Exchange_Active:		equ	-1
MUIV_NList_Exchange_Bottom:		equ	-2
MUIV_NList_Exchange_Next:		equ	-3 ;/* only valid for second parameter */
MUIV_NList_Exchange_Previous:		equ	-4 ;/* only valid for second parameter */

MUIV_NList_Jump_Top:			equ	0
MUIV_NList_Jump_Active:			equ	-1
MUIV_NList_Jump_Bottom:			equ	-2
MUIV_NList_Jump_Up:			equ	-4
MUIV_NList_Jump_Down:			equ	-3

MUIV_NList_NextSelected_Start:		equ	-1
MUIV_NList_NextSelected_End:		equ	-1

MUIV_NList_PrevSelected_Start:		equ	-1
MUIV_NList_PrevSelected_End:		equ	-1

MUIV_NList_DragType_None:		equ	0
MUIV_NList_DragType_Default:		equ	1
MUIV_NList_DragType_Immediate:		equ	2
MUIV_NList_DragType_Borders:		equ	3
MUIV_NList_DragType_Qualifier:		equ	4

MUIV_NList_CopyToClip_Active:		equ	-1
MUIV_NList_CopyToClip_Selected:		equ	-2
MUIV_NList_CopyToClip_All:		equ	-3
MUIV_NList_CopyToClip_Entries:		equ	-4
MUIV_NList_CopyToClip_Entry:		equ	-5
MUIV_NList_CopyToClip_Strings:		equ	-6
MUIV_NList_CopyToClip_String:		equ	-7

MUIV_NList_CopyTo_Active:		equ	-1
MUIV_NList_CopyTo_Selected:		equ	-2
MUIV_NList_CopyTo_All:			equ	-3
MUIV_NList_CopyTo_Entries:		equ	-4
MUIV_NList_CopyTo_Entry:		equ	-5

MUIV_NLCT_Success:			equ	0
MUIV_NLCT_OpenErr:			equ	1
MUIV_NLCT_WriteErr:			equ	2
MUIV_NLCT_Failed:			equ	3

MUIV_NList_ForcePen_On:			equ	1
MUIV_NList_ForcePen_Off:		equ	0
MUIV_NList_ForcePen_Default:		equ	-1

MUIV_NList_DropType_Mask:		equ	$00FF
MUIV_NList_DropType_None:		equ	0
MUIV_NList_DropType_Above:		equ	1
MUIV_NList_DropType_Below:		equ	2
MUIV_NList_DropType_Onto:		equ	3

MUIV_NList_DoMethod_Active:		equ	-1
MUIV_NList_DoMethod_Selected:		equ	-2
MUIV_NList_DoMethod_All:		equ	-3

MUIV_NList_DoMethod_Entry:		equ	-1
MUIV_NList_DoMethod_Self:		equ	-2
MUIV_NList_DoMethod_App:		equ	-3

MUIV_NList_EntryValue:			equ	(MUIV_TriggerValue+$100)
MUIV_NList_EntryPosValue:		equ	(MUIV_TriggerValue+$102)
MUIV_NList_SelfValue:			equ	(MUIV_TriggerValue+$104)
MUIV_NList_AppValue:			equ	(MUIV_TriggerValue+$106)

MUIV_NList_ColWidth_All:		equ	-1
MUIV_NList_ColWidth_Default:		equ	-1
MUIV_NList_ColWidth_Get:		equ	-2

MUIV_NList_ContextMenu_Default:		equ	$9d510031
MUIV_NList_ContextMenu_TopOnly:		equ	$9d510033
MUIV_NList_ContextMenu_BarOnly:		equ	$9d510035
MUIV_NList_ContextMenu_Bar_Top:		equ	$9d510037
MUIV_NList_ContextMenu_Always:		equ	$9d510039
MUIV_NList_ContextMenu_Never:		equ	$9d51003b

MUIV_NList_Menu_DefWidth_This:		equ	$9d51003d
MUIV_NList_Menu_DefWidth_All:		equ	$9d51003f
MUIV_NList_Menu_DefOrder_This:		equ	$9d510041
MUIV_NList_Menu_DefOrder_All:		equ	$9d510043
MUIV_NList_Menu_Default_This:		equ	MUIV_NList_Menu_DefWidth_This
MUIV_NList_Menu_Default_All:		equ	MUIV_NList_Menu_DefWidth_All

MUIV_NList_SortType_None:		equ	$F0000000
MUIV_NList_SortTypeAdd_None:		equ	$00000000
MUIV_NList_SortTypeAdd_2Values:		equ	$80000000
MUIV_NList_SortTypeAdd_4Values:		equ	$40000000
MUIV_NList_SortTypeAdd_Mask:		equ	$C0000000
MUIV_NList_SortTypeValue_Mask:		equ	$3FFFFFFF

MUIV_NList_Sort3_SortType_Both:		equ	$00000000
MUIV_NList_Sort3_SortType_1:		equ	$00000001
MUIV_NList_Sort3_SortType_2:		equ	$00000002

MUIV_NList_Quiet_None:			equ	0
MUIV_NList_Quiet_Full:			equ	-1
MUIV_NList_Quiet_Visual:		equ	-2

MUIV_NList_Imports_Active:		equ	(1 << 0)
MUIV_NList_Imports_Selected:		equ	(1 << 1)
MUIV_NList_Imports_First:		equ	(1 << 2)
MUIV_NList_Imports_ColWidth:		equ	(1 << 3)
MUIV_NList_Imports_ColOrder:		equ	(1 << 4)
MUIV_NList_Imports_TitleMark:		equ	(1 << 7)
MUIV_NList_Imports_Cols:		equ	$000000F8
MUIV_NList_Imports_All:			equ	$0000FFFF

MUIV_NList_Exports_Active:		equ	(1 << 0)
MUIV_NList_Exports_Selected:		equ	(1 << 1)
MUIV_NList_Exports_First:		equ	(1 << 2)
MUIV_NList_Exports_ColWidth:		equ	(1 << 3)
MUIV_NList_Exports_ColOrder:		equ	(1 << 4)
MUIV_NList_Exports_TitleMark:		equ	(1 << 7)
MUIV_NList_Exports_Cols:		equ	$000000F8
MUIV_NList_Exports_All:			equ	$0000FFFF

MUIV_NList_TitleMark_ColMask:		equ	$000000FF
MUIV_NList_TitleMark_TypeMask:		equ	$F0000000
MUIV_NList_TitleMark_None:		equ	$F0000000
MUIV_NList_TitleMark_Down:		equ	$00000000
MUIV_NList_TitleMark_Up:		equ	$80000000
MUIV_NList_TitleMark_Box:		equ	$40000000
MUIV_NList_TitleMark_Circle:		equ	$C0000000

MUIV_NList_TitleMark2_ColMask:		equ	$000000FF
MUIV_NList_TitleMark2_TypeMask:		equ	$F0000000
MUIV_NList_TitleMark2_None:		equ	$F0000000
MUIV_NList_TitleMark2_Down:		equ	$00000000
MUIV_NList_TitleMark2_Up:		equ	$80000000
MUIV_NList_TitleMark2_Box:		equ	$40000000
MUIV_NList_TitleMark2_Circle:		equ	$C0000000

MUIV_NList_SetColumnCol_Default:	equ	-1

MUIV_NList_GetPos_Start:		equ	-1
MUIV_NList_GetPos_End:			equ	-1

MUIV_NList_SelectChange_Flag_Multi:	equ	(1 << 0)


;/* Structs */

;;;struct BitMapImage
;;;{
;;;  ULONG    control;   ;/* should be == to MUIM_NList_CreateImage for a valid BitMapImage struct */
;;;  WORD     width;     ;/* if control == MUIA_Image_Spec then obtainpens is a pointer to an Object */
;;;  WORD     height;
;;;  WORD    *obtainpens;
;;;  PLANEPTR mask;
;;;  struct BitMap imgbmp;
;;;  LONG     flags;
;;;};
;;;
;;;
;;;struct MUI_NList_TestPos_Result
;;;{
;;;  LONG  entry;   ;/* number of entry, -1 if mouse not over valid entry */
;;;  WORD  column;  ;/* numer of column, -1 if no valid column */
;;;  UWORD flags;   ;/* not in the list, see below */
;;;  WORD  xoffset; ;/* x offset in column */
;;;  WORD  yoffset; ;/* y offset of mouse click from center of line */
;;;  WORD  preparse;     ;/* 2 if in column preparse string, 1 if in entry preparse string, else 0 */
;;;  WORD  char_number;  ;/* the number of the clicked char in column, -1 if no valid */
;;;  WORD  char_xoffset; ;/* x offset of mouse clicked from left of char if positive */
;;;};                    ;/* and left of next char if negative. If there is no char there */
                      ;/* negative if from left of first char else from right of last one */

MUI_NLPR_ABOVE:		equ	(1<<0)
MUI_NLPR_BELOW:		equ	(1<<1)
MUI_NLPR_LEFT:		equ	(1<<2)
MUI_NLPR_RIGHT:		equ	(1<<3)
MUI_NLPR_BAR:		equ	(1<<4)  ;/* if between two columns you'll get the left
                           		;column number of both, and that flag */
MUI_NLPR_TITLE:		equ	(1<<5)  ;/* if clicked on title, only column, xoffset and yoffset (and MUI_NLPR_BAR)
                            		;are valid (you'll get MUI_NLPR_ABOVE too) */
MUI_NLPR_ONTOP:		equ	(1<<6)  ;/* it is on title/half of first visible entry */


;;;struct MUI_NList_GetEntryInfo
;;;{
;;;  LONG pos;             ;/* num of entry you want info about */
;;;  LONG line;            ;/* real line number */
;;;  LONG entry_pos;       ;/* entry num of returned entry ptr */
;;;  APTR entry;           ;/* entry pointer */
;;;  LONG wrapcol;         ;/* NOWRAP, WRAPCOLx, or WRAPPED|WRAPCOLx */
;;;  LONG charpos;         ;/* start char number in string (unused if NOWRAP) */
;;;  LONG charlen;         ;/* string lenght (unused if NOWRAP) */
;;;};

NOWRAP:			equ	$00
WRAPCOL0:		equ	$01
WRAPCOL1:		equ	$02
WRAPCOL2:		equ	$04
WRAPCOL3:		equ	$08
WRAPCOL4:		equ	$10
WRAPCOL5:		equ	$20
WRAPCOL6:		equ	$40
WRAPPED:		equ	$80


;;;struct MUI_NList_GetSelectInfo
;;;{
;;;  LONG start;        ;/* num of first selected *REAL* entry/line (first of wrapped from which start is issued) */
;;;  LONG end;          ;/* num of last selected *REAL* entry/line (first of wrapped from which start is issued) */
;;;  LONG num;          ;/* not used */
;;;  LONG start_column; ;/* column of start of selection in 'start' entry */
;;;  LONG end_column;   ;/* column of end of selection in 'end' entry */
;;;  LONG start_pos;    ;/* char pos of start of selection in 'start_column' entry */
;;;  LONG end_pos;      ;/* char pos of end of selection in 'end_column' entry */
;;;  LONG vstart;       ;/* num of first visually selected entry */
;;;  LONG vend;         ;/* num of last visually selected entry */
;;;  LONG vnum;         ;/* number of visually selected entries */
;;;};
;;;;/* NOTE that vstart==start, vend==end in all cases if no wrapping is used */

;/* Methods */

MUIM_NList_Clear:		equ	$9d510070 ;/* GM */
MUIM_NList_CreateImage:		equ	$9d510071 ;/* GM */
MUIM_NList_DeleteImage:		equ	$9d510072 ;/* GM */
MUIM_NList_Exchange:		equ	$9d510073 ;/* GM */
MUIM_NList_GetEntry:		equ	$9d510074 ;/* GM */
MUIM_NList_Insert:		equ	$9d510075 ;/* GM */
MUIM_NList_InsertSingle:	equ	$9d510076 ;/* GM */
MUIM_NList_Jump:		equ	$9d510077 ;/* GM */
MUIM_NList_Move:		equ	$9d510078 ;/* GM */
MUIM_NList_NextSelected:	equ	$9d510079 ;/* GM */
MUIM_NList_Redraw:		equ	$9d51007a ;/* GM */
MUIM_NList_Remove:		equ	$9d51007b ;/* GM */
MUIM_NList_Select:		equ	$9d51007c ;/* GM */
MUIM_NList_Sort:		equ	$9d51007d ;/* GM */
MUIM_NList_TestPos:		equ	$9d51007e ;/* GM */
MUIM_NList_CopyToClip:		equ	$9d51007f ;/* GM */
MUIM_NList_UseImage:		equ	$9d510080 ;/* GM */
MUIM_NList_ReplaceSingle:	equ	$9d510081 ;/* GM */
MUIM_NList_InsertWrap:		equ	$9d510082 ;/* GM */
MUIM_NList_InsertSingleWrap:	equ	$9d510083 ;/* GM */
MUIM_NList_GetEntryInfo:	equ	$9d510084 ;/* GM */
MUIM_NList_QueryBeginning:	equ	$9d510085 ;/* Obsolete */
MUIM_NList_GetSelectInfo:	equ	$9d510086 ;/* GM */
MUIM_NList_CopyTo:		equ	$9d510087 ;/* GM */
MUIM_NList_DropType:		equ	$9d510088 ;/* GM */
MUIM_NList_DropDraw:		equ	$9d510089 ;/* GM */
MUIM_NList_RedrawEntry:		equ	$9d51008a ;/* GM */
MUIM_NList_DoMethod:		equ	$9d51008b ;/* GM */
MUIM_NList_ColWidth:		equ	$9d51008c ;/* GM */
MUIM_NList_ContextMenuBuild:	equ	$9d51008d ;/* GM */
MUIM_NList_DropEntryDrawErase:	equ	$9d51008e ;/* GM */
MUIM_NList_ColToColumn:		equ	$9d51008f ;/* GM */
MUIM_NList_ColumnToCol:		equ	$9d510091 ;/* GM */
MUIM_NList_Sort2:		equ	$9d510092 ;/* GM */
MUIM_NList_PrevSelected:	equ	$9d510093 ;/* GM */
MUIM_NList_SetColumnCol:	equ	$9d510094 ;/* GM */
MUIM_NList_Sort3:		equ	$9d510095 ;/* GM */
MUIM_NList_GetPos:		equ	$9d510096 ;/* GM */
MUIM_NList_SelectChange:	equ	$9d5100A0 ;/* GM */
;;;struct  MUIP_NList_Clear              { ULONG MethodID; };
;;;struct  MUIP_NList_CreateImage        { ULONG MethodID; Object *obj; ULONG flags; };
;;;struct  MUIP_NList_DeleteImage        { ULONG MethodID; APTR listimg; };
;;;struct  MUIP_NList_Exchange           { ULONG MethodID; LONG pos1; LONG pos2; };
;;;struct  MUIP_NList_GetEntry           { ULONG MethodID; LONG pos; APTR *entry; };
;;;struct  MUIP_NList_Insert             { ULONG MethodID; APTR *entries; LONG count; LONG pos; };
;;;struct  MUIP_NList_InsertSingle       { ULONG MethodID; APTR entry; LONG pos; };
;;;struct  MUIP_NList_Jump               { ULONG MethodID; LONG pos; };
;;;struct  MUIP_NList_Move               { ULONG MethodID; LONG from; LONG to; };
;;;struct  MUIP_NList_NextSelected       { ULONG MethodID; LONG *pos; };
;;;struct  MUIP_NList_Redraw             { ULONG MethodID; LONG pos; };
;;;struct  MUIP_NList_Remove             { ULONG MethodID; LONG pos; };
;;;struct  MUIP_NList_Select             { ULONG MethodID; LONG pos; LONG seltype; LONG *state; };
;;;struct  MUIP_NList_Sort               { ULONG MethodID; };
;;;struct  MUIP_NList_TestPos            { ULONG MethodID; LONG x; LONG y; struct MUI_NList_TestPos_Result *res; };
;;;struct  MUIP_NList_CopyToClip         { ULONG MethodID; LONG pos; ULONG clipnum; APTR *entries; struct Hook *hook; };
;;;struct  MUIP_NList_UseImage           { ULONG MethodID; Object *obj; LONG imgnum; ULONG flags; };
;;;struct  MUIP_NList_ReplaceSingle      { ULONG MethodID; APTR entry; LONG pos; LONG wrapcol; LONG align; };
;;;struct  MUIP_NList_InsertWrap         { ULONG MethodID; APTR *entries; LONG count; LONG pos; LONG wrapcol; LONG align; };
;;;struct  MUIP_NList_InsertSingleWrap   { ULONG MethodID; APTR entry; LONG pos; LONG wrapcol; LONG align; };
;;;struct  MUIP_NList_GetEntryInfo       { ULONG MethodID; struct MUI_NList_GetEntryInfo *res; };
;;;struct  MUIP_NList_QueryBeginning     { ULONG MethodID; };
;;;struct  MUIP_NList_GetSelectInfo      { ULONG MethodID; struct MUI_NList_GetSelectInfo *res; };
;;;struct  MUIP_NList_CopyTo             { ULONG MethodID; LONG pos; char *filename; APTR *result; APTR *entries; };
;;;struct  MUIP_NList_DropType           { ULONG MethodID; LONG *pos; LONG *type; LONG minx, maxx, miny, maxy; LONG mousex, mousey; };
;;;struct  MUIP_NList_DropDraw           { ULONG MethodID; LONG pos; LONG type; LONG minx, maxx, miny, maxy; };
;;;struct  MUIP_NList_RedrawEntry        { ULONG MethodID; APTR entry; };
;;;struct  MUIP_NList_DoMethod           { ULONG MethodID; LONG pos; APTR DestObj; ULONG FollowParams; ;/* ... */  };
;;;struct  MUIP_NList_ColWidth           { ULONG MethodID; LONG col; LONG width; };
;;;struct  MUIP_NList_ContextMenuBuild   { ULONG MethodID; LONG mx; LONG my; LONG pos; LONG column; LONG flags; LONG ontop; };
;;;struct  MUIP_NList_DropEntryDrawErase { ULONG MethodID; LONG type; LONG drawpos; LONG erasepos; };
;;;struct  MUIP_NList_ColToColumn        { ULONG MethodID; LONG col; };
;;;struct  MUIP_NList_ColumnToCol        { ULONG MethodID; LONG column; };
;;;struct  MUIP_NList_Sort2              { ULONG MethodID; LONG sort_type; LONG sort_type_add; };
;;;struct  MUIP_NList_PrevSelected       { ULONG MethodID; LONG *pos; };
;;;struct  MUIP_NList_SetColumnCol       { ULONG MethodID; LONG column; LONG col; };
;;;struct  MUIP_NList_Sort3              { ULONG MethodID; LONG sort_type; LONG sort_type_add; LONG which; };
;;;struct  MUIP_NList_GetPos             { ULONG MethodID; APTR entry; LONG *pos; };
;;;struct  MUIP_NList_SelectChange       { ULONG MethodID; LONG pos; LONG state; ULONG flags; };


DISPLAY_ARRAY_MAX:	equ	64

ALIGN_LEFT:		equ	$0000
ALIGN_CENTER:		equ	$0100
ALIGN_RIGHT:		equ	$0200
ALIGN_JUSTIFY:		equ	$0400


;/*
; *  Be carrefull ! the 'sort_type2' member don't exist in releases before 19.96
; *  where MUIM_NList_Sort3, MUIA_NList_SortType2, MUIA_NList_TitleClick2 and
; *  MUIA_NList_TitleMark2 have appeared !
; *  You can safely use get(obj,MUIA_NList_SortType2,&st2) instead if you are not
; *  sure of the NList.mcc release which is used.
; */
;;;struct NList_CompareMessage
;;;{
;;;  APTR entry1;
;;;  APTR entry2;
;;;  LONG sort_type;
;;;  LONG sort_type2;
;;;};
;;;
;;;struct NList_ConstructMessage
;;;{
;;;  APTR entry;
;;;  APTR pool;
;;;};
;;;
;;;struct NList_DestructMessage
;;;{
;;;  APTR entry;
;;;  APTR pool;
;;;};
;;;
;;;struct NList_DisplayMessage
;;;{
;;;  APTR entry;
;;;  LONG entry_pos;
;;;  char *strings[DISPLAY_ARRAY_MAX];
;;;  char *preparses[DISPLAY_ARRAY_MAX];
;;;};
;;;
;;;struct NList_CopyEntryToClipMessage
;;;{
;;;  APTR entry;
;;;  LONG entry_pos;
;;;  char *str_result;
;;;  LONG column1;
;;;  LONG column1_pos;
;;;  LONG column2;
;;;  LONG column2_pos;
;;;  LONG column1_pos_type;
;;;  LONG column2_pos_type;
;;;};
;;;
;;;struct NList_CopyColumnToClipMessage
;;;{
;;;  char *string;
;;;  LONG entry_pos;
;;;  char *str_result;
;;;  LONG str_pos1;
;;;  LONG str_pos2;
;;;};


	ENDIF ; MUI_NList_MCC_I

