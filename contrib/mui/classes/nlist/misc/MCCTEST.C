

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
#include <clib/muimaster_protos.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>

#include <proto/muimaster.h>

#include "NList_mcc.h"
#include "NListview_mcc.h"
#include "NListviews_mcp.h"

static VOID fail(APTR APP_Main,char *str);
static VOID init(VOID);
int main(int argc,char *argv[]);

/* MUI STUFF */

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#ifndef SAVEDS
#define SAVEDS
#endif

struct Library *MUIMasterBase = NULL;

/*LONG __stack = 30000;*/


APTR  APP_Main,
      WIN_Main,
      img;

/*static char *Pages[]   = { "MCC Object","MCP Object","ImageDisplay","Volum","NVolum","Settings","Settingsgroup",NULL };*/
static char *Pages[]   = { "MCC Object","MCP Object",NULL };

static char *MainTextArray[] =
{
  "\033cNList / NListview",
  " ",
  "\033cThis new list/listview custom class handle standards list backgrounds",
  "\033c(read only list one for the title)",
  "\033cthe 'cursel' list color is here used for unselected cursor !",
  "\033cI think this way of handling multiselection with mouse and keys is better...",
  "\033cyou can horizontaly scroll with cursor keys,",
  "\033cor going on the right and left of the list while selecting with the mouse.",
  "\033cTry just clicking on the left/right borders !",
  "\033cThe features that are lacking are multicolumn, images, sort,",
  "\033cdrag and drop, and some others :-(",
  "\033cI think about adding classic char selection,",
  "\033cand perhaps some way to permit making editor possibilities easily...",
  " ",
  " ",
  "\033rGive some feedback about it ! :-)",
  " ",
  "\033r(masson@alto.unice.fr)",
  NULL
};

/* MUI ERROR? */
static VOID fail(APTR APP_Main,char *str)
{
  if (APP_Main)
    MUI_DisposeObject(APP_Main);

  if (MUIMasterBase)
    CloseLibrary(MUIMasterBase);
  if (str)
  {
    puts(str);
    exit(20);
  }
  exit(0);
}

/* STANDARD INIT FUNCTION FOR MUI */
static VOID init(VOID)
{
  if (!(MUIMasterBase = (struct Library *) OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN-1)))
    fail(NULL,"Failed to open "MUIMASTER_NAME".");
}

/* MAIN PROGRAM */
int main(int argc,char *argv[])
{
  int     numarg;
  LONG val;

  init();

  APP_Main = ApplicationObject,
    MUIA_Application_Title      , "mcctest",
    MUIA_Application_Version    , "$VER: mcctest ["__DATE__"]",
    MUIA_Application_Copyright  , "Written by Gilles Masson, 1996",
    MUIA_Application_Author     , "Gilles Masson",
    MUIA_Application_Description, "mcctest",
    MUIA_Application_Base       , "mcctest",

    SubWindow, WIN_Main = WindowObject,
      MUIA_Window_Title, "mcctest 1996",
      MUIA_Window_Width, MUIV_Window_Width_Visible(80),
      MUIA_Window_Height, MUIV_Window_Height_Visible(80),
      WindowContents, VGroup,
        Child, HGroup,
          MUIA_Group_SameWidth, TRUE,
          Child, SimpleButton("Button1"),
          Child, img = ImageObject,
            MUIA_Image_Spec,"1:11",
          End,
          Child, SimpleButton("Button2"),
        End,
        Child, RegisterGroup(Pages),
          MUIA_Register_Frame, TRUE,
          Child, NListviewObject,
            MUIA_Background, MUII_ListBack,
            MUIA_Frame, MUIV_Frame_InputList,
            MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Default,
            MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Default,
            MUIA_NList_SourceArray, MainTextArray,
          End,
          Child, NListviewsMcpObject,
          End,
/*
          Child, VGroup,
            Child, HGroup,
              Child, MUI_NewObject(MUIC_Popimage,
                MUIA_Window_Title, "Adjust Entry Background",
                MUIA_Draggable, TRUE,
                MUIA_ShortHelp, "Adjust Entry Background.",
              End,
              Child, MUI_NewObject(MUIC_Popimage,
              End,
            End,
            Child, HGroup,
              Child, MUI_NewObject(MUIC_Imagedisplay,
                MUIA_Window_Title, "Show Background",
                MUIA_Draggable, TRUE,
                MUIA_ShortHelp, "Show  Background.",
              End,
              Child, MUI_NewObject(MUIC_Imageadjust,
                MUIA_Window_Title, "Adjust Background",
                MUIA_Draggable, TRUE,
                MUIA_ShortHelp, "Adjust Background.",
              End,
            End,
          End,
          Child, MUI_NewObject(MUIC_Volumelist,
            InputListFrame, MUIA_Dirlist_Directory, "ram:",
          End,
          Child, MUI_NewObject("Volumelist.mcc",
            InputListFrame, MUIA_Dirlist_Directory, "ram:",
          End,
          Child, MUI_NewObject(MUIC_Settings,
          End,
          Child, MUI_NewObject(MUIC_Settingsgroup,
          End,
*/
        End,
      End,
    End,
  End;

  if(!APP_Main) fail(APP_Main,"Failed to create Application.");

  DoMethod(WIN_Main,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    APP_Main,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
/*
  set(img,MUIA_Image_Spec,"1:12");

  get (img,MUIA_Width,&val);
  printf("Width=%ld \n",val);
  get (img,MUIA_Height,&val);
  printf("Height=%ld \n",val);

  printf("_defwidth=%d  _defheight=%d \n",_defwidth(img),_defheight(img));

  set(WIN_Main,MUIA_Window_Open,TRUE);

  get (img,MUIA_Width,&val);
  printf("Width=%ld \n",val);
  get (img,MUIA_Height,&val);
  printf("Height=%ld \n",val);

  printf("_defwidth=%d  _defheight=%d \n",_defwidth(img),_defheight(img));

  set(img,MUIA_Image_Spec,"1:16");

  get (img,MUIA_Width,&val);
  printf("Width=%ld \n",val);
  get (img,MUIA_Height,&val);
  printf("Height=%ld \n",val);

  printf("_defwidth=%d  _defheight=%d \n",_defwidth(img),_defheight(img));
*/

  set(WIN_Main,MUIA_Window_Open,TRUE);

  {
    ULONG sigs = 0;


    while (DoMethod(APP_Main,MUIM_Application_NewInput,&sigs) != MUIV_Application_ReturnID_Quit){

      if (sigs){
        sigs = Wait(sigs | SIGBREAKF_CTRL_C);
        if (sigs & SIGBREAKF_CTRL_C) break;
      }
    }
  }

  set(WIN_Main,MUIA_Window_Open,FALSE);
  fail(APP_Main,NULL);
}


