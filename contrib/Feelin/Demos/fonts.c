/*

   This example shows how font changes are dynamic handled

*/

#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelin.h>

struct FeelinBase                  *FeelinBase;

///new_title
FObject new_title(STRPTR Title)
{
   return   TextObject,
            FA_Frame,         "$text-frame",
            FA_Back,          "$text-back",
            FA_ColorScheme,   "$text-scheme",
            FA_ChainToCycle,  FALSE,
            FA_Text,          Title,
            FA_Text_PreParse, "<align=center><pens style=emboss>",
            FA_Text_VCenter,  TRUE,
            FA_Weight,        5,
            End;
}
//+

int main(void)
{
   FObject app,win,pop_1,pop_2,pop_3;

   if (F_FEELIN_OPEN)
   {
      app = AppObject,
//         FA_ColorScheme, "c:64DC82,c:55AF64,c:0A460A,c:FF0000,c:FFFFFF",

         Child, win = WindowObject,
//            FA_Back,         "P:Feelin:Images/StripGreen.png",
            FA_Window_Title, "Feelin : Fonts",
            FA_Window_Open,   TRUE,

            Child, VGroup,
               Child, TextObject,
                  FA_ChainToCycle,  FALSE,
                  FA_Text,          "<align=center><pens text=halfdark shadow=halfshadow>The layout adapts itself<br>to font changes.<br>Try <b><font face=topaz size=8>topaz/8</font></b> to see...",
                  FA_SetMax,        FV_SetHeight,
                  End,

               Child, VGroup, FA_Group_Rows, 3,
                  Child, new_title("Icons"),   Child, pop_1 = PopFontObject, End,
                  Child, new_title("Default"), Child, pop_2 = PopFontObject, End,
                  Child, new_title("Screen"),  Child, pop_3 = PopFontObject, End,
               End,

               Child, BarObject, End,

               Child, HGroup, FA_Group_SameSize,TRUE,
                  Child, SimpleButton("<b>Save</b>"),
                  Child, SimpleButton("<pens text=halfdark>Use</pens>"),
                  Child, SimpleButton("<pens text=highligth>Cancel</pens>"),
               End,
            End,
         End,
      End;

      if (app)
      {
         F_Do(pop_1,FM_Notify,"Contents",FV_Notify_Always,FV_Notify_Self,FM_Set,2,FA_Font,FV_Notify_Value);
         F_Do(pop_2,FM_Notify,"Contents",FV_Notify_Always,FV_Notify_Self,FM_Set,2,FA_Font,FV_Notify_Value);
         F_Do(pop_3,FM_Notify,"Contents",FV_Notify_Always,FV_Notify_Self,FM_Set,2,FA_Font,FV_Notify_Value);

         F_Set(pop_1,(ULONG) "Contents",(ULONG) "Helvetica/9");
         F_Set(pop_2,(ULONG) "Contents",(ULONG) "Helvetica/11");
         F_Set(pop_3,(ULONG) "Contents",(ULONG) "Helvetica/13");

         F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE,app,FM_Application_Shutdown,0);
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

