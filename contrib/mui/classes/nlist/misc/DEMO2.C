
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>

#ifdef __GNUC__
# include <proto/alib.h>
#endif
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>

#include <clib/muimaster_protos.h>

#include <MUI/NListview_mcc.h>

#include <proto/muimaster.h>



/* ************ IMG definitions ************** */

const ULONG list_colors[24] =
{
	0xadadadad,0xadadadad,0xadadadad,
	0x7b7b7b7b,0x7b7b7b7b,0x7b7b7b7b,
	0x3b3b3b3b,0x67676767,0xa2a2a2a2,
	0xadadadad,0xadadadad,0xadadadad,
	0xadadadad,0xadadadad,0xadadadad,
	0xadadadad,0xadadadad,0xadadadad,
	0xffffffff,0xffffffff,0xffffffff,
	0x00000000,0x00000000,0x00000000,
};

#define LIST_WIDTH        23
#define LIST_HEIGHT       14
#define LIST_DEPTH         3
#define LIST_COMPRESSION   0
#define LIST_MASKING       2

const UBYTE list_body[168] = {
0x00,0x00,0x44,0x00,0xff,0xff,0xb8,0x00,0xff,0xff,0xb8,0x00,0x00,0x00,0x00,
0x00,0x80,0x00,0x64,0x00,0xff,0xff,0xb8,0x00,0x3b,0xb8,0x46,0x00,0x80,0x00,
0x64,0x00,0xc4,0x47,0xfc,0x00,0x00,0x00,0x46,0x00,0x80,0x00,0x64,0x00,0xff,
0xff,0xfc,0x00,0x36,0xd8,0x46,0x00,0x80,0x00,0x64,0x00,0xc9,0x27,0xfc,0x00,
0x00,0x00,0x46,0x00,0x80,0x00,0x64,0x00,0xff,0xff,0xfc,0x00,0x3d,0xe8,0x6e,
0x00,0x80,0x00,0x5c,0x00,0xc2,0x17,0xcc,0x00,0x00,0x00,0x46,0x00,0x80,0x00,
0x78,0x00,0xff,0xff,0xf8,0x00,0x00,0x00,0x40,0x00,0x80,0x00,0x64,0x00,0xff,
0xff,0xf8,0x00,0xbf,0xff,0xee,0x00,0x7f,0xff,0xdc,0x00,0x3f,0xff,0xcc,0x00,
0x00,0x44,0x46,0x00,0xff,0xbb,0xb8,0x00,0xff,0xbb,0xb8,0x00,0x00,0x00,0x00,
0x00,0x80,0x66,0x64,0x00,0xff,0xbb,0xb8,0x00,0xbf,0xee,0xee,0x00,0x7f,0xdd,
0xdc,0x00,0x3f,0xcc,0xcc,0x00,0x3f,0xee,0xee,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00, };

#define IMG \
  BodychunkObject,\
    MUIA_FixWidth             , LIST_WIDTH ,\
    MUIA_FixHeight            , LIST_HEIGHT,\
    MUIA_Bitmap_Width         , LIST_WIDTH ,\
    MUIA_Bitmap_Height        , LIST_HEIGHT,\
    MUIA_Bodychunk_Depth      , LIST_DEPTH ,\
    MUIA_Bodychunk_Body       , (UBYTE *) list_body,\
    MUIA_Bodychunk_Compression, LIST_COMPRESSION,\
    MUIA_Bodychunk_Masking    , LIST_MASKING,\
    MUIA_Bitmap_SourceColors  , (ULONG *) list_colors,\
    MUIA_Bitmap_Transparent   , 0,\
  End

/* ************ IMG2 definitions ************** */

const ULONG list2_colors[24] =
{
	0xabababab,0xadadadad,0xc5c5c5c5,
	0x7b7b7b7b,0x7b7b7b7b,0x7b7b7b7b,
	0x3b3b3b3b,0x67676767,0xa2a2a2a2,
	0x00000000,0x00000000,0x00000000,
	0xffffffff,0xa9a9a9a9,0x97979797,
	0xffffffff,0xffffffff,0xffffffff,
	0x00000000,0x00000000,0x00000000,
	0xadadadad,0xadadadad,0xadadadad,
};

#define LIST2_WIDTH        23
#define LIST2_HEIGHT       13
#define LIST2_DEPTH         3
#define LIST2_COMPRESSION   0
#define LIST2_MASKING       2

const UBYTE list2_body[156] = {
0x04,0x00,0x04,0x00,0x78,0x00,0x08,0x00,0x30,0x00,0x08,0x00,0x79,0x00,0x02,
0x00,0x8e,0x00,0x1c,0x00,0x3e,0x00,0x1c,0x00,0x7e,0x10,0x04,0x00,0x83,0xe0,
0x08,0x00,0xfe,0x80,0x08,0x00,0x45,0x02,0x00,0x00,0x85,0x9c,0x00,0x00,0xff,
0x70,0x00,0x00,0x7e,0x1c,0x80,0x00,0xc1,0x3b,0x78,0x00,0xfe,0xf9,0x30,0x00,
0x00,0x2f,0xb0,0x00,0xff,0x60,0x4c,0x00,0x7e,0xbf,0x48,0x00,0x7c,0x27,0x68,
0x00,0x03,0xc1,0xa4,0x00,0x01,0x7f,0xb4,0x00,0x01,0x1a,0xd8,0x00,0x03,0xe3,
0xa4,0x00,0x01,0xbf,0xac,0x00,0x00,0x6c,0xaa,0x00,0x04,0x99,0xd4,0x00,0x03,
0x97,0xfc,0x00,0x01,0x76,0x5a,0x00,0x38,0x8d,0xec,0x00,0x1e,0x8b,0x7c,0x00,
0x02,0xeb,0x72,0x00,0x0d,0x04,0xfc,0x00,0x09,0x06,0xf8,0x00,0x01,0x84,0x06,
0x00,0x0e,0x02,0xf8,0x00,0x06,0x01,0xb0,0x00,0x00,0x03,0xfc,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00, };

#define IMG2 \
  BodychunkObject,\
    MUIA_FixWidth             , LIST2_WIDTH ,\
    MUIA_FixHeight            , LIST2_HEIGHT,\
    MUIA_Bitmap_Width         , LIST2_WIDTH ,\
    MUIA_Bitmap_Height        , LIST2_HEIGHT,\
    MUIA_Bodychunk_Depth      , LIST2_DEPTH ,\
    MUIA_Bodychunk_Body       , (UBYTE *) list2_body,\
    MUIA_Bodychunk_Compression, LIST2_COMPRESSION,\
    MUIA_Bodychunk_Masking    , LIST2_MASKING,\
    MUIA_Bitmap_SourceColors  , (ULONG *) list2_colors,\
    MUIA_Bitmap_Transparent   , 0,\
  End

/* *********************************************** */


/* MUI STUFF */

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


struct Library *MUIMasterBase;


APTR  APP_Main,
      WIN_Main,
      BM_img,
      BM_img2,
      BT_TitleOn,
      BT_TitleOff,
      BT_NoMulti,
      BT_Multi,
      BT_AllMulti,
      BT_InputOn,
      BT_InputOff,
      BT_Sort,
      BT_DragSortOn,
      BT_DragSortOff,
      BT_SelLine,
      BT_SelChar,
      BT_OpenWin2,
      BT_RemAct,
      BT_RemSel,
      BT_Clear,
      BT_Add,
      LV_Text,
      LI_Text;


#define ID_LIST2_ACTIVE 1

#define NL "\n"
#define NL2 " "


/* *********************************************** */

char MainTextString[] =
{
  "NList.mcc \033o[0] NListview.mcc \033o[1] NListviews.mcp" NL
  "\033C" NL
  "\033o[2] : \033o[2,50]" NL
  "\033C" NL
  "\033o[3]" NL
  "\033C" NL
  "\033o[4]" NL
  "\033C" NL
  ":\033I[3:WD/11pt/ArrowRight.mf0]:\033I[3:WD/11pt/ArrowDown.mf0]:" NL
  "\033C" NL
  ":\033I[1:17]:\033I[3:kmel/kmel_arrowdown2.image]:" NL
  "\033C" NL
  ":\033I[3:kmel/kmel_arrowdown.image]:" NL
  "\033C" NL
  ":\033I[3:WD/11pt/ArrowLeft.mf0]:" NL
  "\033C" NL
  ":\033I[5:NList.brush]:" NL
  "\033C" NL
  ":\033I[5:list1.brush]:" NL
  "\033C" NL
  ":\033I[3:WD/11pt/ArrowUp.mf0]:" NL
  "\033C" NL
  "This new list/listview custom class has its own backgrounds" NL2
  "and pens setables from mui prefs with NListviews.mcp !" NL2
  "It doesn't use the same way to handle multiselection" NL2
  "with mouse and keys than standard List/Listview." NL
  "" NL
  "\033lThis new\033b list/listview\0333 custom class\033i has its own\033P[2] backgrounds" NL2
  "and pens setables from mui prefs\0333 with\033u NListviews.mcp !" NL2
  "It doesn't use the same way\033P[2] to handle multiselection" NL2
  "with mouse and keys than standard List/Listview." NL
  "" NL
  "\033cThis new\033b list/listview\0333 custom class\033i has its own\033P[2] backgrounds" NL2
  "and pens setables from mui prefs\0333 with\033u NListviews.mcp !" NL2
  "It doesn't use the same way\033P[2] to handle multiselection" NL2
  "with mouse and keys than standard List/Listview." NL
  "" NL
  "\033rThis new\033b list/listview\0333 custom class\033i has its own\033P[2] backgrounds" NL2
  "and pens setables from mui prefs\0333 with\033u NListviews.mcp !" NL2
  "It doesn't use the same way\033P[2] to handle multiselection" NL2
  "with mouse and keys than standard List/Listview." NL
  "" NL
  "\033jThis new\033b list/listview\0333 custom class\033i has its own\033P[2] backgrounds" NL2
  "and pens setables from mui prefs\0333 with\033u NListviews.mcp !" NL2
  "It doesn't use the same way\033P[2] to handle multiselection" NL2
  "with mouse and keys than standard List/Listview." NL
  "\033C" NL
  "You can horizontally scroll with cursor keys," NL2
  "or going on the right and left of the list" NL2
  "while selecting with the mouse." NL2
  "(When there is no immediate draggin stuff active...)" NL2
  "Try just clicking on the left/right borders !" NL
  "\033C" NL
  "\033uGive some feedback about it ! :-)" NL
  "\033c\033uGive some feedback about it ! :-)" NL
  "\033r\033uGive some feedback about it ! :-)" NL
  "\033j\033uGive some feedback about it ! :-)" NL
  "\033r\033b(masson@iut-soph.unice.fr)" NL
  "\033r\033b(masson@iut-soph.unice.fr)(masson@iut-soph.unice.fr)" NL
  "\033r\033b(masson@iut-soph.unice.fr)(masson@iut-soph.unice.fr)(masson@iut-soph.unice.fr)"
};


/* *********************************************** */

char *MainTextArray[] =
{
  "\033c\033nThis new list/listview custom class has its own backgrounds",
  "\033cand pens setables from mui prefs with NListviews.mcp !",
  " ",
  "\033cIt doesn't use the same way to handle multiselection",
  "\033cwith mouse and keys than standard List/Listview.",
  " ",
  "\033cYou can horizontally scroll with cursor keys,",
  "\033cor going on the right and left of the list",
  "\033cwhile selecting with the mouse.",
  "\033c(When there is no draggin stuff active...)",
  "\033cTry just clicking on the left/right borders !",
  "",
  "\033r\033uGive some feedback about it ! :-)",
  "\033r\033b(masson@iut-soph.unice.fr)",
  "",
  "",
  "The numbers in the left column are list entry numbers !",
  "So don't be surprised if sorting entries",
  "don't change that numbers...",
  "",
  "",
  "\033cF4 to copy selected lines to clipboard 0.",
  "\033cF5 to copy active line to clipboard 0.",
  "\033cF6 to copy all lines to clipboard 0.",
  " ",
  "\033cThe classic RightAmiga+C to copy selected",
  "\033clines to clipboard 0 is made builtin.",
  " ",
  " ",
  "\033cYou can sort the entries by dragging.",
  "\033cYou start a drag using one of the qualifier",
  "\033ckeys which are set in prefs,",
  "\033cor going on the left and right of entries.",
  "",
  "",
  "Example of horizontal line in top of entry",
  "\033TExample of horizontal line in top of entry",
  "Example of horizontal line in top of entry",
  "",
  "Example of horizontal line centered on entry",
  "\033C",
  "Example of horizontal line centered on entry",
  "",
  "Example of horizontal line in bottom of entry",
  "\033BExample of horizontal line in bottom of entry",
  "Example of horizontal line in bottom of entry",
  "",
  "Examples of bitmap images : \033o[0]  \033o[1]",
  "which can be used without subclassing the NList class.",
  "",
  "",
  "",
  "ww\tTabulation test",
  "w\tTabulation test",
  "\033cww\tTabulation test",
  "\033cw\tTabulation test",
  "\033rww\tTabulation test",
  "\033rw\tTabulation test",
  "",
  "long line for FULLAUTO horizontal scroller test, long line for FullAuto horizontal scroller test, long line for FULLAUTO horizontal scroller test.",
  "",
  "0 just a little \033bline to test\033n stuffs",
  "1 just a little \033bline to test\033n stuffs",
  "2 just a little \033bline to test\033n stuffs",
  "3 just a little \033bline to test\033n stuffs",
  "\033c4 just a little \033uline to test\033n (center)",
  "\033c5 just a little \033uline to test\033n (center)",
  "\033c6 just a little \033uline to test\033n (center)",
  "\033r7 just a little \033uline to test\033n (right)",
  "\033r8 just a little \033uline to test\033n (right)",
  "\033r9 just a little \033uline to test\033n (right)",
  "10 just a little line to test stuffs and bugs",
  "11 just a little line to test stuffs and bugs",
  "12 just a little line to test stuffs and bugs",
  "13 just a little line to test stuffs and bugs",
  "14 just a little line to test stuffs and bugs",
  "15 just a little \033iline to test stuffs\033n and bugs",
  "16 just a little line to test stuffs and bugs",
  "17 just a little line to test stuffs and bugs",
  "18 just a little line to test stuffs and bugs",
  "19 just a little line to test stuffs and bugs",
  "\033c20 just a little line to test stuffs and bugs (center)",
  "\033c21 just a little line to test stuffs and bugs (center)",
  "\033c22 just a little line to test stuffs and bugs (center)",
  "\033c23 just a little line to test stuffs and bugs (center)",
  "24 just a little line to test stuffs and bugs, just a little line",
  "25 just a little line to test stuffs and bugs, just a little line",
  "\033r26 just a little line to test stuffs and bugs (right)",
  "\033r27 just a little line to test stuffs and bugs (right)",
  "\033r28 just a little line to test stuffs and bugs (right)",
  "\033r29 just a little line to test stuffs and bugs (right)",
  "30 just a little line to test stuffs and bugs, just a little line to test",
  "31 just a little line to test stuffs and bugs, just a little line to test",
  "32 \0332just a little line to test stuffs and bugs, just a little line to test",
  "33 \0333just a little line to test stuffs and bugs, just a little line to test",
  "34 \0334just a little line to test stuffs and bugs, just a little line to test",
  "35 \0335just a little line to test stuffs and bugs, just a little line to test",
  "36 \0336just a little line to test stuffs and bugs, just a little line to test",
  "37 \0337just a little line to test stuffs and bugs, just a little line to test",
  "38 \0338just a little line to test stuffs and bugs, just a little line to test",
  "39 \0339just a little line to test stuffs and bugs, just a little line to test",
  "40 \033P[]just a little line to test stuffs and bugs, just a little line to test stuffs ",
  "41 \033P[1]just a little line to test\033P[] stuffs and bugs, just a little line to test stuffs ",
  "42 \033P[2]just a little line to test\033P[] stuffs and bugs, just a little line to test stuffs ",
  "43 \033P[-1]just a little line to test\033P[] stuffs and bugs, just a little line to test stuffs ",
  "44 \033P[-3]just a little line to test\033P[] stuffs and bugs, just a little line to test stuffs ",
  "45 just a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "46 just a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "47 just a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "48 just a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "49 just a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "50 \033ijust a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "51 \033ujust a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "52 \033bjust a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "53 \033i\033ujust a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "54 \033i\033bjust a little line to test stuffs and bugs, just a little line to test stuffs and bugs",
  "55 \033b\033ujust a little line to test stuffs and bugs, just a little line to test, just a little line to test, just a little line to test",
  "56 \033i\033b\033ujust a little line to test stuffs and bugs, just a little line to test, just a little line to test, just a little line to test",
  "57 just a little line to test stuffs and bugs, just a little line to test, just a little line to test, just a little line to test",
  "58 just a little line to test stuffs and bugs, just a little line to test, just a little line to test, just a little line to test",
  "59 just a little line to test stuffs and bugs, just a little line to test, just a little line to test, just a little line to test",
  " ",
  " ",
  "\033c\033uI find it \033bnice\033n  :-)",
  NULL
};


/* *********************************************** */


static VOID fail(APTR APP_Main,char *str)
{
  if (APP_Main)
    MUI_DisposeObject(APP_Main);
  if (MUIMasterBase)
    CloseLibrary(MUIMasterBase);
  if (str)
  { puts(str);
    exit(20);
  }
  exit(0);
}


static VOID init(VOID)
{
  if (!(MUIMasterBase = (struct Library *) OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN-1)))
    fail(NULL,"Failed to open "MUIMASTER_NAME".");
}


/* *********************************************** */


/* MAIN PROGRAM */
int main(int argc,char *argv[])
{
  LONG win_opened;

  init();

  APP_Main = ApplicationObject,
    MUIA_Application_Title      , "NList-Demo2",
    MUIA_Application_Version    , "$VER: NList-Demo2 0.5 (" __DATE__ ")",
    MUIA_Application_Copyright  , "Written by Gilles Masson, 1996",
    MUIA_Application_Author     , "Gilles Masson",
    MUIA_Application_Description, "NList-Demo2",
    MUIA_Application_Base       , "NList-Demo2",

    SubWindow, WIN_Main = WindowObject,
      MUIA_Window_Title, "NList-Demo2 1996",
      MUIA_Window_ID   , MAKE_ID('T','W','I','N'),

      WindowContents, VGroup,
        Child, HGroup,
          Child, BT_InputOn = SimpleButton("InputOn"),
          Child, BT_InputOff = SimpleButton("InputOff"),
          Child, BT_SelLine = SimpleButton("SelLine"),
          Child, BT_SelChar = SimpleButton("SelChar"),
          Child, BT_Clear = SimpleButton("Clear"),
          Child, BT_Add = SimpleButton("Add"),
        End,
        Child, HGroup,
          Child, BT_NoMulti = SimpleButton("NoMulti"),
          Child, BT_Multi = SimpleButton("Multi"),
          Child, BT_AllMulti = SimpleButton("AllMulti"),
          Child, BM_img = IMG,
          Child, BT_RemAct = SimpleButton("RemAct"),
          Child, BT_RemSel = SimpleButton("RemSel"),
        End,
        Child, HGroup,
          Child, BT_DragSortOn = SimpleButton("DragSortOn"),
          Child, BT_DragSortOff = SimpleButton("DragSortOff"),
          Child, BM_img2 = IMG2,
          Child, BT_TitleOn = SimpleButton("TitleOn"),
          Child, BT_TitleOff = SimpleButton("TitleOff"),
          Child, BT_Sort = SimpleButton("Sort"),
          Child, BT_OpenWin2 = SimpleButton("Add2"),
        End,
/*
        Child, ListviewObject,
          MUIA_Listview_List, ListObject,
            InputListFrame,
            MUIA_List_AutoVisible, TRUE,
            MUIA_List_SourceArray, MainTextArray,
          End,
        End,
*/
        Child, LI_Text= NListviewObject,
          MUIA_NList_DragSortable,TRUE,
          MUIA_NList_DefaultObjectOnClick, TRUE,
          MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Default,
          MUIA_NList_AutoVisible, TRUE,
          MUIA_NList_TitleSeparator, TRUE,
          MUIA_ShortHelp, "The nice multicolumn\ndraggable list\nwith char selection\npossibility :)",
        End,
          /*MUIA_NList_SourceString, MainTextString,*/
          /*MUIA_NList_SourceArray, MainTextArray,*/
      End,
    End,
  End;

  if(!APP_Main) fail(APP_Main,"Failed to create Application.");

  DoMethod(BT_TitleOn, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_Title,"\033cThis is that list title !   :-)");
  DoMethod(BT_TitleOff, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_Title,NULL);

  DoMethod(BT_NoMulti, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_None);
  DoMethod(BT_Multi, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted);
  DoMethod(BT_AllMulti, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Always);

  DoMethod(BT_InputOn, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_Input,TRUE);
  DoMethod(BT_InputOff, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_Input,FALSE);

  DoMethod(BT_DragSortOn, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_DragSortable,TRUE);
  DoMethod(BT_DragSortOff, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_DragSortable,FALSE);

  DoMethod(BT_RemAct, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 2, MUIM_NList_Remove,MUIV_NList_Remove_Active);
  DoMethod(BT_RemSel, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 2, MUIM_NList_Remove,MUIV_NList_Remove_Selected);
  DoMethod(BT_Clear, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 1, MUIM_NList_Clear);
  DoMethod(BT_Add, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 6, MUIM_NList_InsertWrap,MainTextString,-2,MUIV_NList_Insert_Bottom,WRAPCOL1,ALIGN_JUSTIFY);

  DoMethod(BT_OpenWin2, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 6, MUIM_NList_InsertWrap,MainTextString,-2,MUIV_NList_Insert_Bottom,WRAPCOL0,ALIGN_JUSTIFY);

  DoMethod(BT_Sort, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 1, MUIM_NList_Sort);

  DoMethod(BT_SelLine, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line);
  DoMethod(BT_SelChar, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char);

  DoMethod(WIN_Main,MUIM_Notify, MUIA_Window_InputEvent, "f4",
    LI_Text, 4,MUIM_NList_CopyToClip,MUIV_NList_CopyToClip_Selected,0L,NULL);
  DoMethod(WIN_Main,MUIM_Notify, MUIA_Window_InputEvent, "f5",
    LI_Text, 4,MUIM_NList_CopyToClip,MUIV_NList_CopyToClip_Active,0L,NULL);
  DoMethod(WIN_Main,MUIM_Notify, MUIA_Window_InputEvent, "f6",
    LI_Text, 4,MUIM_NList_CopyToClip,MUIV_NList_CopyToClip_All,0L,NULL);

  DoMethod(WIN_Main,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    APP_Main, 5, MUIM_Application_PushMethod,
    APP_Main,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

  set(WIN_Main, MUIA_Window_DefaultObject, LI_Text);

  DoMethod(LI_Text,MUIM_NList_UseImage,BM_img,0,0);
  DoMethod(LI_Text,MUIM_NList_UseImage,BM_img2,1,0);
  DoMethod(LI_Text,MUIM_NList_UseImage,ImageObject,MUIA_FillArea,FALSE,MUIA_Image_Spec,"1:17",End,2,~0L);
/*
  DoMethod(LI_Text,MUIM_NList_InsertWrap,MainTextString, -2, MUIV_NList_Insert_Bottom,WRAPCOL0,ALIGN_LEFT);
*/
  set(WIN_Main,MUIA_Window_Open,TRUE);


  get(WIN_Main,MUIA_Window_Open,&win_opened);
  if (win_opened)
  { ULONG id,sigs = 0;
    char *line;

    while ((id = DoMethod(APP_Main,MUIM_Application_NewInput,&sigs)) != MUIV_Application_ReturnID_Quit)
    {
      if (sigs)
      { sigs = Wait(sigs | SIGBREAKF_CTRL_C);
        if (sigs & SIGBREAKF_CTRL_C) break;
      }
    }
  }
  else
    printf("failed to open main window !\n");


  set(WIN_Main,MUIA_Window_Open,FALSE);

  DoMethod(LI_Text,MUIM_NList_UseImage,NULL,-1);


  fail(APP_Main,NULL);
}

