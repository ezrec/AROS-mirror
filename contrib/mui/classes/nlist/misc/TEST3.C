
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

/*
#include <proto/muimaster.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <libraries/mui.h>
#include <mui/nlist_mcc.h>
#include <mui/nlistview_mcc.h>

struct Library *MUIMasterBase = NULL;
*/

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
  "\033C",
  "\033I[3:kmel/kmel_arrowdown.image]",
  "\033I[3:WD/11pt/ArrowLeft.mf0]",
  "\033C",
  "\033I[3:MUI:libs/kmel/kmel_arrowdown.image]",
  "\033I[3:MUI:libs/WD/11pt/ArrowLeft.mf0]",
  "\033C",
  "",
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
  NULL
};

static char *RS_radio[] =
{
  "Shift",
  "Always",
  "other",
  "Shift",
  "Always",
  "other",
  "Shift",
  "Always",
  "other",
  "Shift",
  "Always",
  "other",
  "Shift",
  "Always",
  "other",
  "Shift",
  "Always",
  "other",
  "Shift",
  "Always",
  "other",
  "Shift",
  "Always",
  "other",
  "Shift",
  "Always",
  "other",
  NULL
};


APTR obj1,obj2,obj3,obj4,obj1b,obj2b,obj3b,obj4b,obj5;


static void OnObj(void)
{
  register long *a1 __asm("a1");    long *val = a1;
  APTR va = (APTR) (val[0]);

  if (va == NULL)
    kprintf("obj NULL \n");
  else if (va == obj1)
    kprintf("obj1:  %lX\n",val[0]);
  else if (va == obj2)
    kprintf("obj2:  %lX\n",val[0]);
  else if (va == obj3)
    kprintf("obj3:  %lX\n",val[0]);
  else if (va == obj4)
    kprintf("obj4:  %lX\n",val[0]);
  else if (va == obj1b)
    kprintf("obj1b: %lX\n",val[0]);
  else if (va == obj2b)
    kprintf("obj2b: %lX\n",val[0]);
  else if (va == obj3b)
    kprintf("obj3b: %lX\n",val[0]);
  else if (va == obj4b)
    kprintf("obj4b: %lX\n",val[0]);
  else if (va == obj5)
    kprintf("obj5: %lX\n",val[0]);
  else
    kprintf("objet: %lX\n",val[0]);

}

static const struct Hook OnObjHook =    { { NULL,NULL },(VOID *)OnObj, NULL,NULL };

int main(void)
{
   void *window, *app;

   if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)))
  return;

  if (app = ApplicationObject,
    MUIA_Application_Title      , "OptyTest",
    MUIA_Application_Version    , "$VER: OptyTest 1.0 (27.02.97)",
    MUIA_Application_Copyright  , "=A91997, Opty inc.",
    MUIA_Application_Author     , "Opty",
    MUIA_Application_Description, "just a test",
    MUIA_Application_Base       , "OPTYTEST",

    SubWindow,window = WindowObject,
      MUIA_Window_Title, "Opty Test",
      MUIA_Window_ID   , 1,
      MUIA_Window_Width , MUIV_Window_Width_Screen(50),
      MUIA_Window_Height, MUIV_Window_Height_Screen(50),
      MUIA_Window_NeedsMouseObject, TRUE,
      WindowContents, VGroup,
        Child, ScrollgroupObject,
          MUIA_Scrollgroup_Contents, HGroupV,
            VirtualFrame,
            GroupFrame,
            Child, VGroup,
              Child, VSpace(0),
              Child, RadioObject,
                MUIA_Radio_Entries,RS_radio,
              End,
              Child, VSpace(0),
            End,
            Child, VGroup,
              Child, HGroup,
/*
                Child, obj1b = NListObject,
                  MUIA_CycleChain, 1,
                  MUIA_Window_NeedsMouseObject, TRUE,
                  MUIA_NList_SourceArray, MainTextArray,
                  MUIA_NList_DefaultObjectOnClick, TRUE,
                End,
*/
                Child, obj1 = NListviewObject,
                  MUIA_CycleChain, 1,
                  MUIA_NListview_NList, obj1b = NListObject,
                    MUIA_Window_NeedsMouseObject, TRUE,
                    MUIA_NList_SourceArray, MainTextArray,
                    MUIA_NList_DefaultObjectOnClick, TRUE,
                  End,
                End,
                Child, BalanceObject, MUIA_CycleChain, 1, End,
                Child, obj2 = ListviewObject,
                  MUIA_CycleChain, 1,
                  MUIA_Listview_List, obj2b = ListObject, InputListFrame,
                    MUIA_Window_NeedsMouseObject, TRUE,
                    MUIA_List_SourceArray, MainTextArray,
                  End,
                End,
              End,
              Child, BalanceObject, End,
              Child, HGroup,
                Child, obj3 = ListviewObject,
                  MUIA_CycleChain, 1,
                  MUIA_Listview_List, obj3b = ListObject, InputListFrame,
                    MUIA_Window_NeedsMouseObject, TRUE,
                    MUIA_List_SourceArray, MainTextArray,
                  End,
                End,
                Child, BalanceObject, MUIA_CycleChain, 1, End,
                Child, obj4 = ListviewObject,
                  MUIA_CycleChain, 1,
                  MUIA_Listview_List, obj4b = ListObject, InputListFrame,
                    MUIA_Window_NeedsMouseObject, TRUE,
                    MUIA_List_SourceArray, MainTextArray,
                  End,
                End,
              End,
              Child, obj5 = TextObject,
                  ButtonFrame,
                  MUIA_CycleChain, 1,
                  MUIA_Font, MUIV_Font_Button,
                  MUIA_Text_Contents, "obj5 obj5 obj5 obj5 obj5 obj5 obj5 obj5 obj5 obj5 obj5 obj5 obj5 obj5",
                  MUIA_Text_PreParse, "\33c",
                  MUIA_InputMode    , MUIV_InputMode_RelVerify,
                  MUIA_Background   , MUII_ButtonBack,
              End,
            End,
          End,
        End,
      End,
    End,
  End)
  {
    ULONG sigs = 0;

    DoMethod(window,MUIM_Notify,MUIA_Window_MouseObject, MUIV_EveryTime,
      window, 3, MUIM_CallHook, &OnObjHook,MUIV_TriggerValue);

    DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    set(window, MUIA_Window_Open, TRUE);

    while (DoMethod(app, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit)
    {
      if (sigs)
      {
        sigs = Wait(sigs | SIGBREAKF_CTRL_C);
        if (sigs & SIGBREAKF_CTRL_C) break;
      }
    }

    set(window, MUIA_Window_Open, FALSE);

    MUI_DisposeObject(app);
  }

  CloseLibrary(MUIMasterBase);
}


