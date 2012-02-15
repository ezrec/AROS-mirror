/*
** $Id$
*/

#define MUICFG_Textinput_ExternalEditor MCC_TI_ID(0)	/* STRPTR */
#define MUICFG_Textinput_Cursorstyle MCC_TI_ID(9) /* ULONG */
#define MUICFG_Textinput_Blinkrate MCC_TI_ID(10) /* ULONG */
#define MUICFG_Textinput_Font MCC_TI_ID(11) /* STRPTR */
#define MUICFG_Textinput_ButtonImage MCC_TI_ID(12) /* ImageSpec */
#define MUICFG_Textinput_EditSync MCC_TI_ID(14) /* BOOL */
#define MUICFG_Textinput_WordWrapOn MCC_TI_ID(15) /* BOOL */
#define MUICFG_Textinput_WordWrapAt MCC_TI_ID(16) /* ULONG */
#define MUICFG_Textinput_SavedVerRev MCC_TI_ID(17) /* UWORD/UWORD */
#define MUICFG_Textinput_SingleFallback MCC_TI_ID(18) /* BOOL */
#define MUICFG_Textinput_PopupSingle MCC_TI_ID(19) /* BOOL */
#define MUICFG_Textinput_PopupMulti MCC_TI_ID(20) /* BOOL */
#define MUICFG_Textinput_CursorSize MCC_TI_ID(21) /* ULONG */
#define MUICFG_Textinput_MarkQuals MCC_TI_ID(22) /* ULONG */
#define MUICFG_Textinput_FindURLInput MCC_TI_ID(23) /* BOOL */
#define MUICFG_Textinput_FindURLNoInput MCC_TI_ID(24) /* BOOL */
#define MUICFG_Textinput_UndoBytesSingle MCC_TI_ID(25) /* ULONG */
#define MUICFG_Textinput_UndoLevelsSingle MCC_TI_ID(26) /* ULONG */
#define MUICFG_Textinput_UndoBytesMulti MCC_TI_ID(27) /* ULONG */
#define MUICFG_Textinput_UndoLevelsMulti MCC_TI_ID(28) /* ULONG */
#define MUICFG_Textinput_ClickableURLInput MCC_TI_ID(29) /* BOOL */
#define MUICFG_Textinput_ClickableURLNoInput MCC_TI_ID(30) /* BOOL */
#define MUICFG_Textinput_FixedFont MCC_TI_ID(31) /* STRPTR */
#define MUICFG_Textinput_HiliteQuotes MCC_TI_ID(32) /* BOOL */

#define MUICFG_Textinput_KeyCount MCC_TI_ID(330)
#define MUICFG_Textinput_KeyBase MCC_TI_ID(331)

#define MUICFG_Textinput_Pens_Inactive_Foreground MCC_TI_ID(100) /* PenSpec */
#define MUICFG_Textinput_Pens_Inactive_Background MCC_TI_ID(101) /* PenSpec */
#define MUICFG_Textinput_Pens_Active_Foreground MCC_TI_ID(102) /* PenSpec */
#define MUICFG_Textinput_Pens_Active_Background MCC_TI_ID(103) /* PenSpec */
#define MUICFG_Textinput_Pens_Marked_Foreground MCC_TI_ID(104) /* PenSpec */
#define MUICFG_Textinput_Pens_Marked_Background MCC_TI_ID(105) /* PenSpec */
#define MUICFG_Textinput_Pens_Cursor_Foreground MCC_TI_ID(106) /* PenSpec */
#define MUICFG_Textinput_Pens_Cursor_Background MCC_TI_ID(107) /* PenSpec */
#define MUICFG_Textinput_Pens_Style_Foreground MCC_TI_ID(108) /* PenSpec */
#define MUICFG_Textinput_Pens_Style_Background MCC_TI_ID(109) /* PenSpec */
#define MUICFG_Textinput_Pens_URL_Underline MCC_TI_ID(110) /* PenSpec */
#define MUICFG_Textinput_Pens_URL_SelectedUnderline MCC_TI_ID(111) /* PenSpec */
#define MUICFG_Textinput_Pens_Misspell_Underline MCC_TI_ID(112) /* PenSpec */

struct textinput_keydef {
	ULONG action;
   char key[ 80 ];
};

