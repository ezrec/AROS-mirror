
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
LONG __stack = 16384;
int CXBRK(void) { return(0); }
int _CXBRK(void) { return(0); }
void chkabort(void) {}
#endif

struct Library *MUIMasterBase;

#include <MUI/NListview_mcc.h>
#include <MUI/NDirlist_mcc.h>

#include <proto/muimaster.h>


/* *********************************************** */

APTR APP_Main,
     WIN_2,
     LV_Text2,
     LI_Text2,
     ST_string;

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


/* MAIN PROGRAM */
int main(int argc,char *argv[])
{
  LONG win_opened;
  LONG result;
  static const struct Hook DisplayLI_TextHook  =    { { NULL,NULL },(VOID *)DisplayLI_Text, NULL,NULL };

  init();

  APP_Main = ApplicationObject,
    MUIA_Application_Title      , "NDirlist-Demo",
    MUIA_Application_Version    , "$VER: NDirlist-Demo 0.6 (" __DATE__ ")",
    MUIA_Application_Copyright  , "Written by Gilles Masson, 1997",
    MUIA_Application_Author     , "Gilles Masson",
    MUIA_Application_Description, "NDirlist-Demo",
    MUIA_Application_Base       , "NDirlist-Demo",

    SubWindow, WIN_2 = WindowObject,
      MUIA_Window_Title, "NDirlist-Demo 1997",
      MUIA_Window_ID   , MAKE_ID('W','I','N','1'),
      WindowContents, VGroup,
        MUIA_Group_VertSpacing, 1,
        Child, LV_Text2 = NListviewObject,
          MUIA_CycleChain, 1,
          MUIA_NListview_NList, LI_Text2 = NDirlistObject,
            MUIA_NDirlist_Directory, "JEUX:",
          End,
          MUIA_ShortHelp, "The 2nd list",
        End,
        Child, ST_string = StringObject,
          MUIA_CycleChain, 1,
          StringFrame,
        End,
      End,
    End,
  End;

  if(!APP_Main) fail(APP_Main,"Failed to create Application.");

  DoMethod(WIN_2,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    APP_Main, 5, MUIM_Application_PushMethod,
    APP_Main,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

  set(WIN_2, MUIA_Window_ActiveObject, ST_string);
  set(WIN_2, MUIA_Window_DefaultObject, LV_Text2);


  set(WIN_2,MUIA_Window_Open,TRUE);


  get(WIN_2,MUIA_Window_Open,&win_opened);
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


  set(WIN_2,MUIA_Window_Open,FALSE);

  fail(APP_Main,NULL);
}

