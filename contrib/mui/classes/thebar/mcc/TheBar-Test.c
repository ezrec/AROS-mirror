/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#include <stdio.h>
#include <string.h>

#include <clib/alib_protos.h>

#include <exec/tasks.h>
#include <libraries/mui.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include "private.h"
#include "mcc_common.h"
#include "class.h"
#include "Debug.h"

#include "SDI_hook.h"

#if defined(__amigaos4__)
struct Library *GfxBase = NULL;
struct Library *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *DataTypesBase = NULL;
#elif defined(__MORPHOS__)
struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *DataTypesBase = NULL;
#else
struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *DataTypesBase = NULL;
#endif

#if defined(__amigaos4__)
struct GraphicsIFace *IGraphics = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct UtilityIFace *IUtility = NULL;
struct CyberGfxIFace *ICyberGfx = NULL;
struct DataTypesIFace *IDataTypes = NULL;
#endif

DISPATCHERPROTO(_Dispatcher);
struct MUI_CustomClass *ThisClass = NULL;
struct MUI_CustomClass *lib_spacerClass = NULL;
struct MUI_CustomClass *lib_dragBarClass = NULL;
ULONG lib_flags = 0;

// global data
static const char *appearances[] = { "Images and text", "Images", "Text", NULL};
static const char *labelPoss[] = { "Bottom", "Top", "Right", "Left", NULL};

static struct MUIS_TheBar_Button buttons[] =
{
  { 0, 0, "_Pred", "Pread mail.",    0, 0, NULL, NULL },
  { 1, 1, "_Next", "Next mail.",     0, 0, NULL, NULL },
  { 2, 2, "_Move", "Move somewhere.",0, 0, NULL, NULL },
  { MUIV_TheBar_BarSpacer, 3, NULL, NULL, 0, 0, NULL, NULL },
  { 3, 4, "_Forw", "Forward somewhere.", 0, 0, NULL, NULL },
  { 4, 5, "F_ind", "Find something.", 0, 0, NULL, NULL },
  { 5, 6, "_Save", "Save mail.", 0, 0, NULL, NULL },
  { MUIV_TheBar_End , 0, NULL, NULL, 0, 0, NULL, NULL },
};

// static hooks
HOOKPROTONHNO(SaveFunc, ULONG, ULONG *qual)
{
  D(DBF_STARTUP, "SaveHook triggered: %08lx\n", *qual);

  return 0;
}
MakeStaticHook(SaveHook, SaveFunc);

HOOKPROTONHNO(SleepFunc, ULONG, Object **sb)
{
  ULONG sleeping;

  DoMethod(*sb, MUIM_TheBar_GetAttr, 6, MUIA_TheBar_Attr_Sleep, &sleeping);

  D(DBF_STARTUP, "SleepHook triggered: %08lx %ld\n", (ULONG)*sb, sleeping);

  DoMethod(*sb, MUIM_TheBar_SetAttr, 6, MUIA_TheBar_Attr_Sleep, !sleeping);

  return 0;
}
MakeStaticHook(SleepHook, SleepFunc);

int main(void)
{
  if((GfxBase = (APTR)OpenLibrary("graphics.library", 38)) &&
    GETINTERFACE(IGraphics, GfxBase))
  if((IntuitionBase = (APTR)OpenLibrary("intuition.library", 38)) &&
    GETINTERFACE(IIntuition, IntuitionBase))
  if((UtilityBase = OpenLibrary("utility.library", 38)) &&
    GETINTERFACE(IUtility, UtilityBase))
  if((DataTypesBase = OpenLibrary("datatypes.library", 37)) &&
    GETINTERFACE(IDataTypes, DataTypesBase))
  {
  	// Open cybergraphics.library (optional)
    if((CyberGfxBase = OpenLibrary("cybergraphics.library", 41)) &&
       GETINTERFACE(ICyberGfx, CyberGfxBase) == FALSE)
    {
      CloseLibrary(CyberGfxBase);
      CyberGfxBase = NULL;
    }

    #if defined(DEBUG)
    SetupDebug();
    #endif

    ENTER();

    if((MUIMasterBase = OpenLibrary("muimaster.library", MUIMASTER_VMIN)) &&
      GETINTERFACE(IMUIMaster, MUIMasterBase))
    {
      Object *app, *win, *sb, *appearance, *labelPos, *borderless, *sunny, *raised, *scaled, *update;

      // now we init our subclasses
      initSpacerClass();
      initDragBarClass();

      ThisClass = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct InstData), ENTRY(_Dispatcher));

      if(ThisClass && (app = ApplicationObject,
                         MUIA_Application_Title,         "TheBar Demo1",
                         MUIA_Application_Version,       "$VER: TheBarDemo1 1.0 (24.6.2003)",
                         MUIA_Application_Copyright,     "Copyright 2003 by Alfonso Ranieri",
                         MUIA_Application_Author,        "Alfonso Ranieri <alforan@tin.it>",
                         MUIA_Application_Description,  "TheBar example",
                         MUIA_Application_Base,         "THEBAREXAMPLE",

                         SubWindow, win = WindowObject,
                           MUIA_Window_ID,             MAKE_ID('M','A','I','N'),
                           MUIA_Window_Title,          "TheBar Demo1",
                           WindowContents, VGroup,
                             Child, sb = NewObject(ThisClass->mcc_Class, NULL,
                               MUIA_Group_Horiz,       TRUE,
                               MUIA_TheBar_EnableKeys, TRUE,
                               MUIA_TheBar_Buttons,    buttons,
                               MUIA_TheBar_PicsDrawer, "PROGDIR://demo/pics",
                               MUIA_TheBar_Strip,      "Read.toolbar",
                               MUIA_TheBar_SelStrip,   "Read_S.toolbar",
                               MUIA_TheBar_DisStrip,   "Read_G.toolbar",
                               MUIA_TheBar_StripCols,  11,
                             End,
                             Child, VGroup,
                               GroupFrameT("Settings"),
                               Child, HGroup,
                                 Child, Label2("Appearance"),
                                 Child, appearance = MUI_MakeObject(MUIO_Cycle,NULL,appearances),
                                 Child, Label2("Label pos"),
                                 Child, labelPos = MUI_MakeObject(MUIO_Cycle,NULL,labelPoss),
                               End,
                               Child, HGroup,
                                 Child, HSpace(0),
                                 Child, Label1("Borderless"),
                                 Child, borderless = MUI_MakeObject(MUIO_Checkmark,NULL),
                                 Child, HSpace(0),
                                 Child, Label1("Sunny"),
                                 Child, sunny = MUI_MakeObject(MUIO_Checkmark,NULL),
                                 Child, HSpace(0),
                                 Child, Label1("Raised"),
                                 Child, raised = MUI_MakeObject(MUIO_Checkmark,NULL),
                                 Child, HSpace(0),
                                 Child, Label1("Scaled"),
                                 Child, scaled = MUI_MakeObject(MUIO_Checkmark,NULL),
                                 Child, HSpace(0),
                               End,
                             End,
                             Child, update = MUI_MakeObject(MUIO_Button,"_Update"),
                           End,
                         End,
                       End))
      {
        ULONG sigs = 0, id;

        DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
        DoMethod(update,MUIM_Notify,MUIA_Pressed,FALSE,app,2,MUIM_Application_ReturnID,TAG_USER);

        set(win,MUIA_Window_Open,TRUE);

        // now we generate some notifies
        DoMethod(sb, MUIM_TheBar_Notify, 5, MUIA_Pressed, FALSE, sb, 3, MUIM_CallHook, &SleepHook, sb);
        DoMethod(sb, MUIM_TheBar_Notify, 6, MUIA_Pressed, FALSE, sb, 3, MUIM_CallHook, &SaveHook, MUIV_TheBar_Qualifier);

        while((LONG)(id = DoMethod(app,MUIM_Application_NewInput,&sigs)) != MUIV_Application_ReturnID_Quit)
        {
          if(id==TAG_USER)
          {
            ULONG appearanceV, labelPosV, borderlessV, sunnyV, raisedV, scaledV;

            get(appearance,MUIA_Cycle_Active,&appearanceV);
            get(labelPos,MUIA_Cycle_Active,&labelPosV);
            get(borderless,MUIA_Selected,&borderlessV);
            get(sunny,MUIA_Selected,&sunnyV);
            get(raised,MUIA_Selected,&raisedV);
            get(scaled,MUIA_Selected,&scaledV);

            SetAttrs(sb,MUIA_TheBar_ViewMode,   appearanceV,
                        MUIA_TheBar_LabelPos,   labelPosV,
                        MUIA_TheBar_Borderless, borderlessV,
                        MUIA_TheBar_Sunny,      sunnyV,
                        MUIA_TheBar_Raised,     raisedV,
                        MUIA_TheBar_Scaled,     scaledV,
                        TAG_DONE);
          }

          if(sigs)
          {
            sigs = Wait(sigs | SIGBREAKF_CTRL_C);
            if(sigs & SIGBREAKF_CTRL_C) break;
          }
        }

        D(DBF_STARTUP, "cleaning up application object...");
        MUI_DisposeObject(app);
        
        D(DBF_STARTUP, "cleaning up mcc...");
        if(ThisClass)
          MUI_DeleteCustomClass(ThisClass);
      }
      else
        printf("Failed to create application\n");

     if(lib_dragBarClass)
       MUI_DeleteCustomClass(lib_dragBarClass);

     if(lib_spacerClass)
       MUI_DeleteCustomClass(lib_spacerClass);

      DROPINTERFACE(IMUIMaster);
      CloseLibrary(MUIMasterBase);
      MUIMasterBase = NULL;
    }
  }

  D(DBF_STARTUP, "freeing library bases/interfaces...");

  if(CyberGfxBase)
  {
    DROPINTERFACE(ICyberGfx);
    CloseLibrary(CyberGfxBase);
    CyberGfxBase = NULL;
  }

  if(DataTypesBase)
  {
    DROPINTERFACE(IDataTypes);
    CloseLibrary(DataTypesBase);
    DataTypesBase = NULL;
  }

  if(UtilityBase)
  {
    DROPINTERFACE(IUtility);
    CloseLibrary(UtilityBase);
  }

  if(IntuitionBase)
  {
    DROPINTERFACE(IIntuition);
    CloseLibrary((struct Library *)IntuitionBase);
  }

  if(GfxBase)
  {
    DROPINTERFACE(IGraphics);
    CloseLibrary((struct Library *)GfxBase);
  }

  RETURN(0);
  return 0;
}
