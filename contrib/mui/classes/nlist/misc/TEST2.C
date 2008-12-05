
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/io.h>
#include <libraries/dos.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
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


struct Library *MUIMasterBase;


APTR  APP_Main,
      WIN_Main,
      GR_VGroup,
      LI_Text,
      LI_Text2;


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
  "Examples of horizontal line centered on entry visible on left and rigth only",
  "\033c\033EExample of left-right horizontal line",
  "Examples of horizontal line centered on entry visible on left and rigth only",
  "",
  "Examples of bitmap images : \033o[0]  \033o[1]",
  "which can be used without subclassing the NList class.",
  "",
  "Examples of custom object images : \033o[2;9d51ffff;1]  \033o[2;9d51ffff;1,24]  \033o[2;9d51ffff;0,48]  \033o[2;9d51ffff;1,96]",
  "",
  "Examples of direct ImageSpec : \033I[3:kmel/kmel_arrowdown.image]  \033I[3:WD/11pt/ArrowLeft.mf0]",
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


int main(int argc,char *argv[])
{
  init();

  APP_Main = ApplicationObject,
    MUIA_Application_Title      , "NList-test2",
    MUIA_Application_Version    , "$VER: NList-test2 0.1 (" __DATE__ ")",
    MUIA_Application_Copyright  , "Written by Gilles Masson, 1996",
    MUIA_Application_Author     , "Gilles Masson",
    MUIA_Application_Description, "NList-test2",
    MUIA_Application_Base       , "NList-test2",

    SubWindow, WIN_Main = WindowObject,
      MUIA_Window_Title, "NList-test2 1996",
      WindowContents, GR_VGroup = HGroup,
        Child, LI_Text= NListviewObject,
          MUIA_CycleChain, 1,
          MUIA_NListview_NList, NListObject,
            /*InputListFrame,*/
            MUIA_NList_SourceArray, MainTextArray,
            MUIA_NList_DefaultObjectOnClick, TRUE,
          End,
        End,
        Child, BalanceObject,
          MUIA_CycleChain, 1,
        End,
        Child, LI_Text2= NListviewObject,
          MUIA_CycleChain, 1,
          MUIA_NListview_NList, NListObject,
            /*InputListFrame,*/
            MUIA_NList_SourceArray, MainTextArray,
            MUIA_NList_DefaultObjectOnClick, TRUE,
          End,
        End,
      End,
    End,
  End;

  if(!APP_Main)
    fail(APP_Main,"Failed to create Application.");
/*
kprintf("test2 app=   0x%08lX \n",APP_Main);
kprintf("test2 group= 0x%08lX \n",GR_VGroup);
kprintf("test2 nlist= 0x%08lX \n",LI_Text);
kprintf("test2 nlist2= 0x%08lX \n",LI_Text2);
*/
  DoMethod(WIN_Main,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    APP_Main, 5, MUIM_Application_PushMethod,
    APP_Main,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

  set(WIN_Main,MUIA_Window_Open,TRUE);

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
  set(WIN_Main,MUIA_Window_Open,FALSE);

  fail(APP_Main,NULL);

  return (0);
}

