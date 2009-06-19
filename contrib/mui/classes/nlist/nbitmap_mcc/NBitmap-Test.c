/***************************************************************************

 NBitmap.mcc - New Bitmap MUI Custom Class
 Copyright (C) 2007 by NList Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TextEditor class Support Site:  http://www.sf.net/projects/texteditor-mcc

 $Id$

***************************************************************************/

#include <libraries/iffparse.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/datatypes.h>
#include <mui/NBitmap_mcc.h>

#include "private.h"

#if defined(__amigaos4__)
struct Library *IntuitionBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *GfxBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *DataTypesBase = NULL;
struct Library *MUIMasterBase = NULL;
#else
struct IntuitionBase *IntuitionBase = NULL;
#if defined(__AROS__)
struct UtilityBase *UtilityBase = NULL;
#else
struct Library *UtilityBase = NULL;
#endif
struct GfxBase *GfxBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *DataTypesBase = NULL;
struct Library *MUIMasterBase = NULL;
#endif

#if defined(__amigaos4__)
struct IntuitionIFace *IIntuition = NULL;
struct UtilityIFace *IUtility = NULL;
struct GraphicsIFace *IGraphics = NULL;
struct CyberGfxIFace *ICyberGfx = NULL;
struct DataTypesIFace *IDataTypes = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
#endif

DISPATCHERPROTO(_Dispatcher);

int main(void)
{
  if((IntuitionBase = (APTR)OpenLibrary("intuition.library", 36)) &&
    GETINTERFACE(IIntuition, IntuitionBase))
  {
    #if defined(DEBUG)
    SetupDebug();
    #endif

    if((UtilityBase = (APTR)OpenLibrary("utility.library", 36)) &&
      GETINTERFACE(IUtility, UtilityBase))
    {
      if((GfxBase = (APTR)OpenLibrary("graphics.library", 36)) &&
        GETINTERFACE(IGraphics, GfxBase))
      {
        if((CyberGfxBase = OpenLibrary("cybergraphics.library", 40)) &&
          GETINTERFACE(ICyberGfx, CyberGfxBase))
        {
          if((DataTypesBase = OpenLibrary("datatypes.library", 36)) &&
            GETINTERFACE(IDataTypes, DataTypesBase))
          {
            if((MUIMasterBase = OpenLibrary("muimaster.library", 19)) &&
              GETINTERFACE(IMUIMaster, MUIMasterBase))
            {
              struct MUI_CustomClass *mcc;

              mcc = MUI_CreateCustomClass(NULL, "Area.mui", NULL, sizeof(struct InstData), ENTRY(_Dispatcher));

              if(mcc != NULL)
              {
                Object *app, *window;
                const char * const classes[] = { "NBitmap.mcc", NULL };

                app = MUI_NewObject("Application.mui",
                      MUIA_Application_Author,      "NBitmap.mcc Open Source Team",
                      MUIA_Application_Base,        "NBitmap-Test",
                      MUIA_Application_Copyright,   "(c) 2007 NBitmap.mcc Open Source Team",
                      MUIA_Application_Description, "NBitmap.mcc test program",
                      MUIA_Application_Title,       "NBitmap-Test",
                      MUIA_Application_Version,     "$VER: NBitmap-Test (" __DATE__ ")",
                      MUIA_Application_UsedClasses, classes,

                      MUIA_Application_Window,
                        window = WindowObject,
                        MUIA_Window_Title,    "NBitmap-Test",
                        MUIA_Window_ID,       MAKE_ID('M','A','I','N'),
                        MUIA_Window_RootObject, VGroup,
                          Child, HGroup,
                            Child, NewObject(mcc->mcc_Class, NULL,
                              MUIA_NBitmap_Type, MUIV_NBitmap_Type_File,
                              MUIA_NBitmap_Normal, "icon.png",
                              MUIA_NBitmap_Label, "Music",
                              MUIA_NBitmap_Button, TRUE,
                            End,
                            Child, RectangleObject, End,
                          End,
                          Child, RectangleObject, End,
                        End,
                      End,
                    End;

                if(app != NULL)
                {
                  ULONG sigs = 0;

                  DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

                  //set(window, MUIA_Window_ActiveObject, editorgad);
                  set(window, MUIA_Window_Open, TRUE);

                  while((LONG)DoMethod(app, MUIM_Application_NewInput, &sigs) != (LONG)MUIV_Application_ReturnID_Quit)
                  {
                    if(sigs)
                    {
                      sigs = Wait(sigs | SIGBREAKF_CTRL_C);

                      if(sigs & SIGBREAKF_CTRL_C)
                        break;
                    }
                  }

                  MUI_DisposeObject(app);
                }
                else
                  PutStr("Failed to create application\n");

                MUI_DeleteCustomClass(mcc);
              }

              DROPINTERFACE(IDataTypes);
              CloseLibrary(DataTypesBase);
              DataTypesBase = NULL;
            }

            DROPINTERFACE(ICyberGfx);
            CloseLibrary(CyberGfxBase);
            CyberGfxBase = NULL;
          }

          DROPINTERFACE(IGraphics);
          CloseLibrary((struct Library *)GfxBase);
          GfxBase = NULL;
        }


        DROPINTERFACE(IMUIMaster);
        CloseLibrary(MUIMasterBase);
        MUIMasterBase = NULL;
      }

      DROPINTERFACE(IUtility);
      CloseLibrary((struct Library *)UtilityBase);
      UtilityBase = NULL;
    }
  }

  if(IntuitionBase)
  {
    DROPINTERFACE(IIntuition);
    CloseLibrary((struct Library *)IntuitionBase);
  }

  return 0;
}
