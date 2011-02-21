/*
**  test9.c
**
**    Bug No. 35 test code provided by Jeff Grimmet
*/

#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>
#include <proto/bgui.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include <stdio.h>

Object *Win, *ra,*HD_List,*HD_Add_Entry,*HD_Exit;
struct Window *win;

#define ID_HD_List      1
#define ID_HD_Add_Entry 2
#define ID_HD_Exit      3

struct Library *BGUIBase;
struct IntuitionBase * IntuitionBase;

void GimmeQuickDisplayWindow(void)
{
   BOOL UseSmartWin=FALSE;

   Win = WindowObject,
   WINDOW_Title,     "Skimmer ©1998 Digerati Dreams",
   WINDOW_SizeGadget,   TRUE,
   WINDOW_SmartRefresh, UseSmartWin, // BOOL stored in my prefs
//   WINDOW_SharedPort,   SharedPort,  // Pointer to a shared port
   WINDOW_AutoAspect,   TRUE,
   WINDOW_AutoKeyLabel, TRUE,
   WINDOW_CloseOnEsc,   TRUE,
   WINDOW_ScaleHeight,  25,
//   WINDOW_Bounds,(CheckIBox(&HDWinBox) ? &HDWinBox : NULL), // Use stored settings if available
   WINDOW_IDCMP,IDCMP_CHANGEWINDOW,
//   WINDOW_IDCMPHook,&SizeHook,
   WINDOW_IDCMPHookBits,IDCMP_CHANGEWINDOW,
//   WINDOW_Font,(FontOK ? &WinFont: NULL),                   // Use user font if selected
//   WINDOW_ToolTicks,Ticks,                                  // Also a stored user setting
   WINDOW_MasterGroup,
      VGroupObject, HOffset( 1 ), VOffset( 1 ), Spacing( 2 ),

         StartMember,
            HD_List = ListviewObject,
               LAB_Label,"_Select Drives ",
               LAB_Place,PLACE_ABOVE,
               GA_ID,ID_HD_List,
               LISTV_EntryArray,NULL,
               LISTV_Columns,          3,
//               LISTV_ColumnWeights,    hd_weights,
               LISTV_Title, "Scan\tDevice\tName",
               LISTV_SortEntryArray,FALSE,
            EndObject,
         EndMember,

         StartMember,HorizSeperator,EndMember,

         StartMember,
            HGroupObject, HOffset( 1 ), VOffset( 1 ), Spacing( 2 ),
               StartMember, HD_Add_Entry    = Button( "_Add entry ", ID_HD_Add_Entry ), EndMember,
               StartMember, HD_Exit  = Button( "_Exit", ID_HD_Exit ), EndMember,
            EndObject, Weight(1),
         EndMember,


      EndObject,
   EndObject;
}

int main(int argc, char **argv)
{
    IPTR sigmask;
    BOOL quit = FALSE;

    if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
    { 
       printf("Could not open Intuition.library!\n");
       return -1;  
    }


    if((BGUIBase=OpenLibrary(BGUINAME,0L))) {
        GimmeQuickDisplayWindow();
        if(Win)
        {
            win = WindowOpen(Win);
            GetAttr(WINDOW_SigMask, Win, &sigmask);

            while(!quit) {
                Wait(sigmask);

                if(sigmask & sigmask ) {
                    ULONG rc;

                    while((rc = HandleEvent(Win)) != WMHI_NOMORE) {
                        switch(rc) {
                            case ID_HD_Add_Entry:
                               AddEntry(win,HD_List,"New entry",LVAP_TAIL);
                               break;
                            case WMHI_CLOSEWINDOW:
                            case ID_HD_Exit:
                                quit = TRUE;
                                break;
                        }
                    }
                }
            }

            DisposeObject(Win);
        }

        CloseLibrary(BGUIBase);
    }

    CloseLibrary((struct Library *)IntuitionBase);

    return 0;
}
