
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/io.h>
#include <intuition/imageclass.h>
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
      LI_Text;


char *MainTextArray[] =
{
  "test  :-)",
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
    MUIA_Application_Title      , "NList-test1",
    MUIA_Application_Version    , "$VER: NList-test1 0.1 (" __DATE__ ")",
    MUIA_Application_Copyright  , "Written by Gilles Masson, 1996",
    MUIA_Application_Author     , "Gilles Masson",
    MUIA_Application_Description, "NList-test1",
    MUIA_Application_Base       , "NList-test1",

    SubWindow, WIN_Main = WindowObject,
      MUIA_Window_Title, "NList-test1 1996",
      WindowContents, GR_VGroup = VGroup,
        Child, HGroup,
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:WD/11pt/ArrowRight.mf0",
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:kmel/kmel_arrowdown2.image",
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:kmel/kmel_arrowdown.image",
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "1:17",
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:WD/11pt/ArrowLeft.mf0",
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:WD/11pt/ArrowDown.mf0",
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "5:NList.brush",
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
        End,
        Child, HGroup,
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:WD/11pt/ArrowRight.mf0",
            MUIA_Image_FreeHoriz, TRUE,
            MUIA_Image_FreeVert, TRUE,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:kmel/kmel_arrowdown2.image",
            MUIA_Image_FreeHoriz, TRUE,
            MUIA_Image_FreeVert, TRUE,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:kmel/kmel_arrowdown.image",
            MUIA_Image_FreeHoriz, TRUE,
            MUIA_Image_FreeVert, TRUE,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "1:17",
            MUIA_Image_FreeHoriz, TRUE,
            MUIA_Image_FreeVert, TRUE,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:WD/11pt/ArrowLeft.mf0",
            MUIA_Image_FreeHoriz, TRUE,
            MUIA_Image_FreeVert, TRUE,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "3:WD/11pt/ArrowDown.mf0",
            MUIA_Image_FreeHoriz, TRUE,
            MUIA_Image_FreeVert, TRUE,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_Image_Spec, "5:NList.brush",
            MUIA_Image_FreeHoriz, TRUE,
            MUIA_Image_FreeVert, TRUE,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
        End,
        Child, HGroup,
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_FillArea,FALSE,
            MUIA_Image_Spec, "3:WD/11pt/ArrowRight.mf0",
            MUIA_Image_FontMatch, TRUE,
            MUIA_Font, MUIV_Font_Fixed,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_FillArea,FALSE,
            MUIA_Image_Spec, "3:kmel/kmel_arrowdown2.image",
            MUIA_Image_FontMatch, TRUE,
            MUIA_Font, MUIV_Font_Fixed,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_FillArea,FALSE,
            MUIA_Image_Spec, "3:kmel/kmel_arrowdown.image",
            MUIA_Image_FontMatch, TRUE,
            MUIA_Font, MUIV_Font_Fixed,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_FillArea,FALSE,
            MUIA_Image_Spec, "1:17",
            MUIA_Image_FontMatch, TRUE,
            MUIA_Font, MUIV_Font_Fixed,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_FillArea,FALSE,
            MUIA_Image_Spec, "3:WD/11pt/ArrowLeft.mf0",
            MUIA_Image_FontMatch, TRUE,
            MUIA_Font, MUIV_Font_Fixed,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_FillArea,FALSE,
            MUIA_Image_Spec, "3:WD/11pt/ArrowDown.mf0",
            MUIA_Image_FontMatch, TRUE,
            MUIA_Font, MUIV_Font_Fixed,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
          Child, MUI_MakeObject(MUIO_VBar, 5),
          Child, HSpace(0),
          Child, ImageObject,
            MUIA_FillArea,FALSE,
            MUIA_Image_Spec, "5:NList.brush",
            MUIA_Image_FontMatch, TRUE,
            MUIA_Font, MUIV_Font_Fixed,
            MUIA_Image_State, IDS_NORMAL,
          End,
          Child, HSpace(0),
        End,
        Child, LI_Text= NListviewObject,
          MUIA_NList_SourceArray, MainTextArray,
        End,
      End,
    End,
  End;

  if(!APP_Main)
    fail(APP_Main,"Failed to create Application.");
/*
kprintf("test1 app=       0x%08lX \n",APP_Main);
kprintf("test1 group=     0x%08lX \n",GR_VGroup);
kprintf("test1 nlistview= 0x%08lX \n",LI_Text);
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

