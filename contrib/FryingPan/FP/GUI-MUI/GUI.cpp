/*
 * FryingPan - Amiga CD/DVD Recording Software (User Interface and supporting Libraries only)
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "GUI.h"
#include <Generic/LibrarySpool.h>
#include <libclass/dos.h>
#include <libclass/intuition.h>
#include <libraries/mui.h>
#include "MUIPageSelect.h"
#include "MUITracks.h"
#include "MUIMedia.h"
#include "MUIContents.h"
#include "MUIDrive.h"
#include "MUISettings.h"
#include "MUIRecord.h"
#include "MUIDriveSelect.h"
#include "MUIPopAction.h"
#include <Generic/Configuration.h>
#include "../IEngine.h"
#include <Generic/Debug.h>
#include <libclass/datatypes.h>
#include <LibC/LibC.h>

#include "Components/MUIWindowMenu.h"

using namespace GenNS;

/*
 * localization area
 */
enum Loc
{
   loc_File                   = lg_Global,
   loc_AboutMUI,
   loc_MUISettings,
   loc_Translate,
   loc_Quit,

   loc_DriveProgress          = lg_Global+ls_Group1,

   loc_FailedToCreateApp      = lg_Global+ls_Req,
};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_File,               "File",           "MNU_FILE"            },
   {  loc_AboutMUI,           "a&About MUI",    "MNU_ABOUT_MUI"       },
   {  loc_MUISettings,        "s&MUI Settings", "MNU_MUI_SETTINGS"    },
   {  loc_Translate,          "t&Translate",    "MNU_TRANSLATE"       },
   {  loc_Quit,               "q&Quit",         "MNU_QUIT"            },
   {  loc_DriveProgress,      "Drive Progress", "LBL_DRIVE_PROGRESS"  },

   {  loc_FailedToCreateApp,  
      "Failed to create application\nFryingPan will now close",   
      "REQ_APP_CREATE_FAILED"                                     },

   {  Localization::LocaleSet::Set_Last, 0, 0                     }
};

static const char* LocaleGroup = "MAIN";
static const int   LocaleVersion = 2;
/*
 * that's it :)
 */

GUI::GUI()
{
   _createDebug(true, "MUI.interface");

   _dx(Lvl_Info, "MUI.interface " __DATE__ "");
   pApp                 = 0;
   Config               = new Configuration("FryingPan");
   _dx(Lvl_Info, "Created configuration element (%08lx)", (uint)Config);

   Glb.CurrentEngine    = new SyncProperty<IEngine*>();
   Glb.WriteSelection   = Globals::Select_Tracks;
   bCompact             = false;

   _dx(Lvl_Info, "Initializing libraries/modules");
   MUIMaster            = MUIMasterIFace::GetInstance(0);
   Datatypes            = DatatypesIFace::GetInstance(0);
   iicon                = IconIFace::GetInstance(0);
   Glb.FryingPanEngine  = EngineIFace::GetInstance(0);
   _dx(Lvl_Info, "Opened engine (%08lx)", (uint)Glb.FryingPanEngine);

   appobj               = iicon->GetDiskObject("FryingPan");
   _dx(Lvl_Info, "Opened application icon (%08lx)", (uint)appobj);
   _dx(Lvl_Info, "Initializing hooks");

   /*
    * initialize all hooks
    */
   hShowHide.Initialize(this, &GUI::doShowHide);
   hPager.Initialize(this, &GUI::doChangePage);
   hEngineCmd.Initialize(this, &GUI::doEngineMessage);
   hEngineMsg.Initialize(this, &GUI::doEngineInternalMessage);
   hEngineChanged.Initialize(this, &GUI::doEngineChanged);
   hAction.Initialize(this, &GUI::doUserAction);

   /*
    * open engines
    */
   _dx(Lvl_Info, "Probing/setting engine");
   for (int i=0; i<4; i++)
      Glb.Engines[i]  = Glb.FryingPanEngine->getEngine(i);

   Glb.CurrentEngine->Assign(Glb.Engines[0]);
  
   /*
    * open configuration
    */ 
   ASSERT(NULL != Config);
   if (NULL != Config)
   {
      _dx(Lvl_Info, "Reading configuration file");
      Config->readFile("ENV:FryingPan/Interface-MUI.prefs");
   }

   /*
    * here all objects are required to initialize:
    * - Localization strings
    * no object should initialize here:
    * - GUI elements
    */
   _dx(Lvl_Info, "Building localization map");
   Glb.Loc.Add((Localization::LocaleSet*)&LocaleSets, LocaleGroup);

   _dx(Lvl_Info, "Creating GUI elements");
   Select      =  new MUIPageSelect(Config, Glb);
   DriveSelect =  new MUIDriveSelect(Config, Glb);
   Tracks      =  new MUITracks(Config, Glb);
   Media       =  new MUIMedia(Config, Glb);
   Contents    =  new MUIContents(Config, Glb);
   Drive       =  new MUIDrive(Config, Glb);
   Settings    =  new MUISettings(Config, Glb);
   Record      =  new MUIRecord(Config, Glb);
   Action      =  new MUIPopAction(Glb);
   Menu        =  new MUIWindowMenu();
 
   _dx(Lvl_Info, "Reading locale");
   Glb.Loc.ReadCatalog("fryingpan.catalog", LocaleVersion);

   /*
    * starting here, we can use localization :)
    */
   _dx(Lvl_Info, "Setting up timer");
   pTimer      =  new Timer();
   pMsgPort    =  new Port(hEngineMsg.GetHook());
   Cfg         =  new ConfigParser(Config, "Main", 0);

   _dx(Lvl_Info, "Init complete");
}
   
GUI::~GUI()
{
   _dx(Lvl_Info, "Disposing individual modules");
   delete Record;
   delete Settings;
   delete Drive;
   delete Contents;
   delete Media;
   delete Tracks;
   delete DriveSelect;
   delete Select;
   delete Action;

   _dx(Lvl_Info, "Disposing configuration element");
   delete Cfg;

   if (NULL != Config)
   {
      _dx(Lvl_Info, "Creating configuration directories (if needed)");
      BPTR lock;
      lock = DOS->CreateDir("ENVARC:FryingPan");
      if (lock)
         DOS->UnLock(lock);
      lock = DOS->CreateDir("ENV:FryingPan");
      if (lock)
         DOS->UnLock(lock);
      
      _dx(Lvl_Info, "Recording configuration back to disk");

      Config->writeFile("ENVARC:FryingPan/Interface-MUI.prefs");
      Config->writeFile("ENV:FryingPan/Interface-MUI.prefs");

      _dx(Lvl_Info, "Disposing main configuration element");
      delete Config;
   }

   _dx(Lvl_Info, "Halting and disposing timer");
   delete pTimer;
   delete pMsgPort;

   _dx(Lvl_Info, "Disposing disk object");
   if (0 != appobj)
      iicon->FreeDiskObject(appobj);
   iicon->FreeInstance();

   _dx(Lvl_Info, "Disposing engine");
   if (Glb.FryingPanEngine != NULL)
   {
      Glb.FryingPanEngine->FreeInstance();
   }
   delete Glb.CurrentEngine;

   _dx(Lvl_Info, "Closing libraries");
   Datatypes->FreeInstance();
   MUIMaster->FreeInstance();

   _dx(Lvl_Info, "All done.");
   _destroyDebug();
}

bool GUI::start()
{
   IPTR signals = 0;

   _dx(Lvl_Info, "Composing program menu");
   {  // Compose menu
      Menu->addMenu(Glb.Loc[loc_File]);
      Menu->addItem(Glb.Loc[loc_AboutMUI],      Action_AboutMUI,        Glb.Loc.Shortcut(loc_AboutMUI));
      Menu->addItem(Glb.Loc[loc_MUISettings],   Action_MUISettings,     Glb.Loc.Shortcut(loc_MUISettings));
      Menu->addSeparator();
#ifdef DEBUG
      Menu->addItem(Glb.Loc[loc_Translate],     Action_Translate,       Glb.Loc.Shortcut(loc_Translate));
      Menu->addSeparator();
#endif
      Menu->addItem(Glb.Loc[loc_Quit],          Action_Quit,            Glb.Loc.Shortcut(loc_Quit));
      Menu->setHook(hAction);
   }

   _dx(Lvl_Info, "Building main application");
   if (pApp == 0)
   {  // Create application object
      pApp = ApplicationObject,
            MUIA_Application_SingleTask,  true,
            MUIA_Application_Base,        (IPTR)"FryingPan",
            MUIA_Application_Title,       (IPTR)"The Frying Pan",
            MUIA_Application_DiskObject,  appobj,
            SubWindow,                    (IPTR)(pWin = WindowObject,
               MUIA_Window_Title,            (IPTR)"FryingPan 1.4",
               MUIA_Window_ID,               MAKE_ID('M', 'A', 'I', 'N'),
               MUIA_Window_Menustrip,        Menu->getObject(),
               MUIA_Window_AppWindow,        true,
               WindowContents,               (IPTR)VGroup,
                  Child,                        (IPTR)(elements = VGroup,
                      Child,                        (IPTR)Select->getObject(),
                      Child,                        (IPTR)DriveSelect->getObject(),
                      Child,                        (IPTR)(pages = PageGroup,
                          Child,                        (IPTR)Tracks->getObject(),
                          Child,                        (IPTR)Media->getObject(),
                          Child,                        (IPTR)Contents->getObject(),
                          Child,                        (IPTR)Drive->getObject(),
                          Child,                        (IPTR)Settings->getObject(),
                          Child,                        (IPTR)Record->getObject(),
                       End),
                  End),

                  Child,                      (IPTR)(switchview = HGroup,
                     GroupFrameT(Glb.Loc[loc_DriveProgress].Data()),
                     Child,                       (IPTR)(gauge = HGroup,
                        Child,                       (IPTR)muiGauge("...", ID_MainProgress),
                        MUIA_InputMode,               MUIV_InputMode_RelVerify,
                        MUIA_ShowSelState,            false,
                        Child,                       (IPTR)Action->getObject(),
                     End),
                  End),
               End,
            End),
         End;
   }
   _dx(Lvl_Info, "Application built at %08lx", (uint)pApp);

   /*
    * still not ctreated
    */
   if (pApp == 0)
   {
      request(Glb.Loc[Globals::loc_Error], Glb.Loc[loc_FailedToCreateApp], Glb.Loc[Globals::loc_OK], 0);
      return false;
   }
   else
   {
      DoMtd((Object *)pWin,  ARRAY(MUIM_Notify, MUIA_Window_CloseRequest, MUIV_EveryTime, (IPTR)pApp, 2, MUIM_Application_ReturnID, (IPTR)MUIV_Application_ReturnID_Quit));
      DoMtd((Object *)gauge, ARRAY(MUIM_Notify, MUIA_Pressed,             false,          (IPTR)pApp, 2, MUIM_CallHook,             (IPTR)hShowHide.GetHook()));
      (*Glb.CurrentEngine) << hEngineChanged.GetHook();
      Select->setHook(hPager.GetHook());
   }

   _dx(Lvl_Info, "Spinning up elements");
   Select->start();
   Tracks->start();
   Media->start();
   Contents->start();
   Drive->start();
   Settings->start();
   Record->start();

   _dx(Lvl_Info, "*pop*");
   Intuition->SetAttrsA(pWin, TAGARRAY(
            MUIA_Window_Open, true,
            TAG_DONE,         0));

   _dx(Lvl_Info, "Registering to all handles");
   for (int i=0; i<4; i++)
      Glb.Engines[i]->registerHandler(hEngineCmd.GetHook());
   
   _dx(Lvl_Info, "Setting up timer (500ms)");
   pTimer->AddRequest(500);

   _dx(Lvl_Info, "Initial update");
   update();

   _dx(Lvl_Info, "Entering main loop");
   while (DoMtd((Object *)pApp, ARRAY(MUIM_Application_NewInput, (IPTR)&signals)) != (IPTR)MUIV_Application_ReturnID_Quit)
   {
      if (signals)
         signals = Exec->Wait(signals | pTimer->GetSignals() | pMsgPort->GetSignals());

      pMsgPort->HandleSignals();

      if (signals & pTimer->GetSignals())
      {
         periodicUpdate();
         pTimer->AddRequest(500);
      }
   }

   _dx(Lvl_Info, "Unregistering from all handles");
   for (int i=0; i<4; i++)
      Glb.Engines[i]->unregisterHandler(hEngineCmd.GetHook());

   _dx(Lvl_Info, "Unregistration complete");
   return true;
}

void GUI::stop()
{
   if (NULL == pApp)
      return;

   _dx(Lvl_Info, "Hiding main window");
   Intuition->SetAttrsA(pWin, TAGARRAY(
            MUIA_Window_Open, false,
            TAG_DONE,         0));

   _dx(Lvl_Info, "Deregistering property notifier");
   (*Glb.CurrentEngine) >> hEngineChanged.GetHook();

   _dx(Lvl_Info, "Disposing MUI application");
   MUIMaster->MUI_DisposeObject((Object *)pApp);
   pApp = 0;

   _dx(Lvl_Info, "Halting individual modules");
   Select->stop();
   Tracks->stop();
   Media->stop();
   Contents->stop();
   Drive->stop();
   Settings->stop();
   Record->stop();
}

void GUI::dispose()
{
   _dx(Lvl_Info, "Shutting down");
   delete this;
}

IPTR GUI::doShowHide(IPTR, IPTR)
{
   _dx(Lvl_Info, "Switching UI");
   _dx(Lvl_Info, "Hiding main window");
   Intuition->SetAttrsA(pWin, TAGARRAY(
            MUIA_Window_Open,     false,
            TAG_DONE,             0));

   _dx(Lvl_Info, "Rearranging elements (new state: %ld)", !bCompact);
   Intuition->SetAttrsA(elements, TAGARRAY(
      MUIA_ShowMe,   (unsigned)bCompact,
      TAG_DONE,      0
   ));

   bCompact = !bCompact;

   _dx(Lvl_Info, "Applying new ID");
   Intuition->SetAttrsA(pWin, TAGARRAY(
            MUIA_Window_ID,       bCompact ?
               MAKE_ID('M', 'I', 'N', 'I') : MAKE_ID('M', 'A', 'I', 'N'),
            TAG_DONE,             0));

   _dx(Lvl_Info, "Showing main window");
   Intuition->SetAttrsA(pWin, TAGARRAY(
            MUIA_Window_Open,     true,
            TAG_DONE,             0));

   _dx(Lvl_Info, "Done.");
   return 0;
}

IPTR GUI::doChangePage(IPTR, long page)
{
   _dx(Lvl_Info, "Flipping page to %ld", page);
   Intuition->SetAttrsA(pages, (TagItem*)ARRAY(
      MUIA_Group_ActivePage,  (IPTR)page-1,
      TAG_DONE,               0
   ));

   return 0;
}

IPTR GUI::doEngineMessage(EngineMessage msg, IEngine* src)
{
   pMsgPort->DoSync(msg, src);
   return 0;
}

IPTR GUI::doEngineInternalMessage(EngineMessage msg, IEngine* src)
{
   if (0 == pApp)
      return 0;

   IEngine *pEng = Glb.CurrentEngine->ObtainRead();
   _dx(Lvl_Info, "Received new engine message %ld", msg);

   if (pEng == src)
   {
      switch ((EngineMessage)msg)
      {
         case Eng_Update:
            {
               _dx(Lvl_Info, "Engine Update");
               update();
            }
            break;
      
         case Eng_JobStarted:
            {
               _dx(Lvl_Info, "Engine Job started");
               IEngine *pEng = Glb.CurrentEngine->ObtainRead();
               muiSetText(ID_MainProgress, pEng->getActionName());
               Glb.CurrentEngine->Release();
            }
            break;
         
         case Eng_JobFinished:
            {
               _dx(Lvl_Info, "Engine Job complete");
               IEngine *pEng = Glb.CurrentEngine->ObtainRead();
               muiSetText(ID_MainProgress, pEng->getActionName());
               Glb.CurrentEngine->Release();
            }
            break;
      
         case Eng_DisableISO:
            {
               _dx(Lvl_Info, "ISO constructor busy");
               Tracks->disableISO();
            }
            break;
         
         case Eng_EnableISO:
            {
               _dx(Lvl_Info, "ISO constructor free");
               Tracks->enableISO();
            }
            break;

         case Eng_UpdateLayout:
            {
               _dx(Lvl_Info, "Layout updated");
               layoutUpdate();
            }
            break;
      };
   }

   _dx(Lvl_Info, "Message handling complete", msg);
   Glb.CurrentEngine->Release();
   return 0;
}

void GUI::update()
{
   _dx(Lvl_Info, "Updating all elements");
   periodicUpdate();
   Drive->update();
   Media->update();
   Contents->update();
   Tracks->update();
   Settings->update();
   Record->update();
   Action->update();
   _dx(Lvl_Info, "Update complete");
}
   
void GUI::periodicUpdate()
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();
   if (NULL != eng)
   {
      muiSetValue(ID_MainProgress, eng->getActionProgress());
      muiSetText(ID_MainProgress, eng->getActionName());
   }
   Glb.CurrentEngine->Release();
}

void GUI::layoutUpdate()
{
   Tracks->layoutUpdate();
}

IPTR GUI::doEngineChanged(IEngine *old, IEngine *current)
{
   _dx(Lvl_Info, "Switching engine from %08lx to %08lx. Updating", (IPTR)old, (IPTR)current);
   update();
   return 0;
}

void GUI::setDebug(DbgHandler *h)
{
   Glb.dbg = h;
}

DbgHandler *GUI::getDebug()
{
   return Glb.dbg;
}

IPTR GUI::doUserAction(ActionID act, IPTR)
{
   switch (act)
   {
      case Action_AboutMUI:
         DoMtd((Object *)pApp, ARRAY(MUIM_Application_AboutMUI, (uint)pWin));
         break;

      case Action_MUISettings:
         DoMtd((Object *)pApp, ARRAY(MUIM_Application_OpenConfigWindow, 0));
         break;

      case Action_Translate:
//FIXME: give user some neat request here!!!!
         Glb.Loc.ExportCD("RAM:fryingpan.cd", LocaleVersion);
         Glb.Loc.ExportCT("RAM:fryingpan.ct", LocaleVersion);
         break;

      case Action_Quit:
         DoMtd((Object *)pApp, ARRAY(MUIM_Application_ReturnID, (IPTR)MUIV_Application_ReturnID_Quit));
         break;
   }

   return 0;
}
