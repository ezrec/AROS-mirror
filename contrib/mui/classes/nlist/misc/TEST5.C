
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
#include <graphics/gfxmacros.h>

#ifdef __GNUC__
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <clib/muimaster_protos.h>
#endif

#ifdef __SASC
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/asl_protos.h>
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds
#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/muimaster_pragmas.h>
extern struct Library *SysBase,*IntuitionBase,*UtilityBase,*GfxBase,*DOSBase,*IconBase;
ULONG __stack = 16384;
int CXBRK(void) { return(0); }
int _CXBRK(void) { return(0); }
void chkabort(void) {}
#endif

struct Library *MUIMasterBase;

#include <MUI/NListview_mcc.h>
#include <MUI/NFloattext_mcc.h>

#include <proto/muimaster.h>

/* *********************************************** */

/* MUI STUFF */

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


static APTR APP_Main,
            WIN_Main,
            BT_NoMulti,
            BT_Multi,
            BT_AllMulti,
            BT_RemAct,
            BT_RemSel,
            LV_Text,
            LI_Text,
            LV_Text2,
            LI_Text2,
            SL_Height,
            SL_Delta,
            SL_Delta2,
            PP_PenTitle,
            PP_PenList,
            PP_PenCursor,
            GR_Text,
            GR_Text2;



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
  "",
  "The numbers in the left column are list entry numbers !",
  "So don't be surprised if sorting entries",
  "don't change that numbers...",
  "",
  "You can push column titles as real buttons...",
  "",
  "You can drag column separator bar in",
  "the title to adjust column width...",
  "(or in upper half of first entry if title is off)",
  "",
  "If, while dragging the bar, you press the",
  "menu button, the width of the column will",
  "come back to its default...",
  "",
  "",
  "\033cF2 to copy selected lines to printer",
  "\033cF3 to copy all lines to file RAM:tmp.txt",
  "",
  "\033cF4 to copy selected lines to clipboard 0.",
  "\033cF5 to copy active line to clipboard 0.",
  "\033cF6 to copy all lines to clipboard 0.",
  "",
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
  "Examples of horizontal line and thick line centered on entry",
  "\033C",
  "\033C\033t",
  "\033C\033t[I7]",
  "\033C\033t[M7]",
  "\033C\033t[-5]",
  "Examples of horizontal line and thick line centered on entry",
  "",
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
  APP_Main = NULL;
  if (!(MUIMasterBase = (struct Library *) OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN-1)))
    fail(NULL,"Failed to open "MUIMASTER_NAME".");
}


/* *********************************************** */

static char bufdelta[100];


#ifdef __SASC
SAVEDS ASM void DeltaLI_TextFunc(REG(a1) LONG *param)
{
#endif
#ifdef __GNUC__
static void DeltaLI_TextFunc(void)
{ register LONG *a1 __asm("a1");    LONG *param = a1;
#endif
  if (param[0] && param[1])
  {
    sprintf(bufdelta,"BAR W=-1 DELTA=%ld,BAR W=-1 DELTA=%ld,BAR W=-1",param[2],param[2]);
    set((void *) param[0],param[1],bufdelta);
  }
}



/* *********************************************** */

static char buf[20];
static char buf2[30];


#ifdef __SASC
SAVEDS ASM void DisplayLI_TextFunc(REG(a2) char **array, REG(a1) char *str)
{
#endif
#ifdef __GNUC__
static void DisplayLI_TextFunc(void)
{ register char **a2 __asm("a2");   char **array = a2;
  register char *a1 __asm("a1");    char *str = a1;
#endif
  LONG *num = (LONG *) (array-1);

  if (str)
  { sprintf(buf,"%ld",*num);
    array[0]  = buf;
    sprintf(buf2,"%lx",(*num) + 5);
    array[1]  = buf2;
    array[2]  = (char *) str;
  }
  else
  {
    array[0]  = "Num";
    array[1]  = "Num+5";
    array[2]  = "This is the list title !\033n\033b   :-)";
  }
}


/* *********************************************** */

#define SimpleButtonCycle(name) \
  TextObject, \
    ButtonFrame, \
    MUIA_CycleChain, 1, \
    MUIA_Font, MUIV_Font_Button, \
    MUIA_Text_Contents, name, \
    MUIA_Text_PreParse, "\33c", \
    MUIA_InputMode    , MUIV_InputMode_RelVerify, \
    MUIA_Background   , MUII_ButtonBack, \
  End

#define SimpleButtonCycle2(name) \
  TextObject, \
    ButtonFrame, \
    MUIA_Font, MUIV_Font_Button, \
    MUIA_Text_Contents, name, \
    MUIA_Text_PreParse, "\33c", \
    MUIA_InputMode    , MUIV_InputMode_RelVerify, \
    MUIA_Background   , MUII_ButtonBack, \
  End

/* *********************************************** */


/* MAIN PROGRAM */
int main(int argc,char *argv[])
{
  LONG win_opened;
  LONG result;
  static const struct Hook DisplayLI_TextHook = { { NULL,NULL },(VOID *)DisplayLI_TextFunc, NULL,NULL };
  static const struct Hook DeltaLI_TextHook = { { NULL,NULL },(VOID *)DeltaLI_TextFunc, NULL,NULL };

  init();

  APP_Main = ApplicationObject,
    MUIA_Application_Title      , "NList-Demo5",
    MUIA_Application_Version    , "$VER: NList-Demo5",
    MUIA_Application_Copyright  , "Written by Gilles Masson, 1998",
    MUIA_Application_Author     , "Gilles Masson",
    MUIA_Application_Description, "NList-Demo5",
    MUIA_Application_Base       , "NList-Demo5",

    SubWindow, WIN_Main = WindowObject,
      MUIA_Window_Title, "NList-Demo5 1997",
      MUIA_Window_ID   , MAKE_ID('T','W','I','N'),
      WindowContents, VGroup,
        Child, HGroup,
          Child, BT_NoMulti = SimpleButtonCycle2("NoMulti"),
          Child, BT_Multi = SimpleButtonCycle2("Multi"),
          Child, BT_AllMulti = SimpleButtonCycle2("AllMulti"),
          Child, BT_RemAct = SimpleButtonCycle2("RemAct"),
          Child, BT_RemSel = SimpleButtonCycle2("RemSel"),
        End,
        Child, HGroup,
          MUIA_Weight, 2,
          Child, PP_PenTitle = PoppenObject,
            MUIA_Window_Title, "Title Pen",
          End,
          Child, PP_PenList = PoppenObject,
            MUIA_Window_Title, "List Pen",
          End,
          Child, PP_PenCursor = PoppenObject,
            MUIA_Window_Title, "Cursor Pen",
          End,
        End,
        Child, SL_Height = SliderObject,
          MUIA_Numeric_Min  , -20,
          MUIA_Numeric_Max  , 30,
          MUIA_Numeric_Value, 1,
        End,
        Child, SL_Delta = SliderObject,
          MUIA_Numeric_Min  , 2,
          MUIA_Numeric_Max  , 50,
          MUIA_Numeric_Value, 4,
        End,
        Child, SL_Delta2 = SliderObject,
          MUIA_Numeric_Min  , 0,
          MUIA_Numeric_Max  , 50,
          MUIA_Numeric_Value, 4,
        End,
        Child, GR_Text = HGroup,
          Child, LV_Text = NListviewObject,
            MUIA_CycleChain, 1,
            MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_None,
            MUIA_NListview_NList, LI_Text = NListObject,
              MUIA_ObjectID, MAKE_ID('O','0','0','7'),
              MUIA_NList_DefaultObjectOnClick, TRUE,
              MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_None,
              MUIA_NList_DisplayHook, &DisplayLI_TextHook,
              MUIA_NList_Format, "BAR W=-1,BAR W=-1,BAR W=-1",
              MUIA_NList_SourceArray, MainTextArray,
              MUIA_NList_AutoVisible, TRUE,
              MUIA_NList_TitleSeparator, FALSE,
              MUIA_NList_Title, TRUE,
              MUIA_NList_EntryValueDependent, TRUE,
              MUIA_NList_MinColSortable,0 ,
              MUIA_NList_TitleMark, (MUIV_NList_TitleMark_Box|2) ,
              MUIA_NList_TitlePen, "m7",
            End,
          End,
        End,
        Child, GR_Text2 = HGroup,
          Child, LV_Text2 = ListviewObject,
            MUIA_CycleChain, 1,
            MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_None,
            MUIA_Listview_List, LI_Text2 = ListObject,
              InputListFrame,
              MUIA_ObjectID, MAKE_ID('O','0','0','8'),
              MUIA_List_DisplayHook, &DisplayLI_TextHook,
              MUIA_List_Format, "BAR W=-1,BAR W=-1,BAR W=-1",
              MUIA_List_SourceArray, MainTextArray,
              MUIA_List_AutoVisible, TRUE,
              MUIA_List_Title, TRUE,
            End,
          End,
        End,
      End,
    End,

  End;

  if(!APP_Main) fail(APP_Main,"Failed to create Application.");

  DoMethod(BT_NoMulti, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_None);
  DoMethod(BT_Multi, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted);
  DoMethod(BT_AllMulti, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 3, MUIM_Set,MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Always);
  DoMethod(BT_RemAct, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 2, MUIM_NList_Remove,MUIV_NList_Remove_Active);
  DoMethod(BT_RemSel, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text, 2, MUIM_NList_Remove,MUIV_NList_Remove_Selected);
  DoMethod(SL_Height, MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime,
    LI_Text, 3, MUIM_Set,MUIA_NList_MinLineHeight,MUIV_TriggerValue);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime,
    LI_Text, 5, MUIM_CallHook,&DeltaLI_TextHook,LI_Text,MUIA_NList_Format,MUIV_TriggerValue);

  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 50,
    LI_Text, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 49,
    LI_Text, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 48,
    GR_Text, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 48,
    GR_Text, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 47,
    LV_Text, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 46,
    LV_Text, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 45,
    LV_Text, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 44,
    GR_Text, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 44,
    GR_Text, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 43,
    GR_Text, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta, MUIM_Notify, MUIA_Numeric_Value, 42,
    GR_Text, 3, MUIM_Set, MUIA_Disabled, FALSE);



  DoMethod(PP_PenTitle, MUIM_Notify, MUIA_Pendisplay_Spec,MUIV_EveryTime,
    LI_Text, 3, MUIM_Set,MUIA_NList_TitlePen,MUIV_TriggerValue);
  DoMethod(PP_PenList, MUIM_Notify, MUIA_Pendisplay_Spec,MUIV_EveryTime,
    LI_Text, 3, MUIM_Set,MUIA_NList_ListPen,MUIV_TriggerValue);
  DoMethod(PP_PenCursor, MUIM_Notify, MUIA_Pendisplay_Spec,MUIV_EveryTime,
    LI_Text, 3, MUIM_Set,MUIA_NList_CursorPen,MUIV_TriggerValue);



  DoMethod(BT_NoMulti, MUIM_Notify, MUIA_Pressed,FALSE,
    LV_Text2, 3, MUIM_Set,MUIA_Listview_MultiSelect,MUIV_Listview_MultiSelect_None);
  DoMethod(BT_Multi, MUIM_Notify, MUIA_Pressed,FALSE,
    LV_Text2, 3, MUIM_Set,MUIA_Listview_MultiSelect,MUIV_Listview_MultiSelect_Shifted);
  DoMethod(BT_AllMulti, MUIM_Notify, MUIA_Pressed,FALSE,
    LV_Text2, 3, MUIM_Set,MUIA_Listview_MultiSelect,MUIV_Listview_MultiSelect_Always);
  DoMethod(BT_RemAct, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text2, 2, MUIM_List_Remove,MUIV_List_Remove_Active);
  DoMethod(BT_RemSel, MUIM_Notify, MUIA_Pressed,FALSE,
    LI_Text2, 2, MUIM_List_Remove,MUIV_List_Remove_Selected);
  DoMethod(SL_Height, MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime,
    LI_Text2, 3, MUIM_Set,MUIA_List_MinLineHeight,MUIV_TriggerValue);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime,
    LI_Text2, 5, MUIM_CallHook,&DeltaLI_TextHook,LI_Text2,MUIA_List_Format,MUIV_TriggerValue);

  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 50,
    LI_Text2, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 49,
    LI_Text2, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 48,
    GR_Text2, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 48,
    GR_Text2, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 47,
    LV_Text2, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 46,
    LV_Text2, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 45,
    LV_Text2, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 44,
    GR_Text2, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 44,
    GR_Text2, 3, MUIM_Set, MUIA_Disabled, FALSE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 43,
    GR_Text2, 3, MUIM_Set, MUIA_Disabled, TRUE);
  DoMethod(SL_Delta2, MUIM_Notify, MUIA_Numeric_Value, 42,
    GR_Text2, 3, MUIM_Set, MUIA_Disabled, FALSE);


  DoMethod(WIN_Main,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    APP_Main, 5, MUIM_Application_PushMethod,
    APP_Main,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

  DoMethod(APP_Main,MUIM_Application_Load,MUIV_Application_Load_ENVARC);

  set(WIN_Main,MUIA_Window_Open,TRUE);


  DoMethod(LI_Text,MUIM_NList_Select,5,MUIV_NList_Select_On, NULL);
  DoMethod(LI_Text,MUIM_NList_Select,6,MUIV_NList_Select_On, NULL);
  DoMethod(LI_Text,MUIM_NList_Select,8,MUIV_NList_Select_On, NULL);
  DoMethod(LV_Text2,MUIM_List_Select,5,MUIV_NList_Select_On, NULL);
  DoMethod(LV_Text2,MUIM_List_Select,6,MUIV_NList_Select_On, NULL);
  DoMethod(LV_Text2,MUIM_List_Select,8,MUIV_NList_Select_On, NULL);



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

  DoMethod(APP_Main,MUIM_Application_Save,MUIV_Application_Save_ENVARC);

  set(WIN_Main,MUIA_Window_Open,FALSE);

  fail(APP_Main,NULL);
}

