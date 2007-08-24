/*

   This program allows you to configure the look and feel of the  GUI  part
   offered  by  Feelin.  As  you  can  see,  it  very  easy  to include the
   preference editor in an application. If the application has  a  basename
   (defined  by  FA_Application_Base)  preferences  will be defined for the
   application, otherwise the preferences are defined globaly. Applications
   with  a  basename  inherit  items not defined from the global preference
   object (managed by the shared object "AppServer").

*/

#include <libraries/feelin.h>
#include <feelin/preference.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelin.h>

struct FeelinBase *FeelinBase;

///Main
int32 main(void)
{
   FObject app,edt;

   if (F_FEELIN_OPEN)
   {
      app = AppObject,
         FA_Application_Title,        "PreferenceEditor",
         FA_Application_Version,      "$VER: Preference Editor 1.00 (2004/09/01)",
         FA_Application_Copyright,    "©2001 - 2005, Olivier LAVIALE",
         FA_Application_Author,       "Olivier LAVIALE <www.gofromiel.com/feelin>",
         FA_Application_Description,  "Configure GUI",
//         FA_Application_Base,         "BASE_TEST",

         Child, edt = PreferenceEditorObject, FA_Window_Open,TRUE, End,
      End;

      if (app)
      {
         F_Do(edt,FM_Notify,FA_Window_CloseRequest,TRUE, app,FM_Application_Shutdown,0);
         F_Do(app,FM_Application_Run);
         F_DisposeObj(app);
      }

      F_FEELIN_CLOSE;
   }
   else
   {
      Printf("Unable to open feelin.library v%ld\n",FV_FEELIN_VERSION);

      return 21;
   }

   return 0;
}
//+
