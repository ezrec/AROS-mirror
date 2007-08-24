/*

   This example shows how font changes are dynamic handled

*/

#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelin.h>

struct FeelinBase                  *FeelinBase;

int main(void)
{
   FObject app,win;

   if (F_FEELIN_OPEN)
   {
      app = AppObject,
         Child, DialogObject,
            FA_Left,             "10%",
            FA_Top,              "10%",
 
            FA_Window_Title,     "Boolean",
           "FA_Dialog_Buttons",  "Boolean",
         End,

         Child, DialogObject,
            FA_Left,             "40%",
            FA_Top,              "20%",

            FA_Window_Title,     "Confirm",
           "FA_Dialog_Buttons",  "Confirm",
         End,

         Child, DialogObject,
            FA_Left,             "10%",
            FA_Top,              "70%",

            FA_Window_Title,     "Always",
           "FA_Dialog_Buttons",  "Always",
         End,

         Child, DialogObject,
            FA_Left,             "80%",
            FA_Top,              "10%",

            FA_Window_Title,     "Preference",
           "FA_Dialog_Buttons",  "Preference",
         End,

         Child, DialogObject,
            FA_Left,             "80%",
            FA_Top,              "60%",

            FA_Window_Title,     "PreferenceTest",
           "FA_Dialog_Buttons",  "PreferenceTest",
         End,
         
         Child, win = DialogObject,
            FA_Window_Title,     "Ok",
           "FA_Dialog_Buttons",  "Ok",
         End,
      End;

      if (app)
      {
         F_Do(win,FM_Notify,"FA_Dialog_Response",FV_Dialog_Response_Ok,app,FM_Application_Shutdown,0);
         F_Do(app,FM_Application_Run);
         F_DisposeObj(app);
      }
      
      F_FEELIN_CLOSE;
   }
   else
   {
      Printf("Unable to open feelin.library v%ld\n",FV_FEELIN_VERSION);
   }
   return 0;
}

