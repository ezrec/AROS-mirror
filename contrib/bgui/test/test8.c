/*
**  test8.c
**
**    Bug No. 28 test code provided by Janne Jalkanen
*/

#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>
#include <proto/bgui.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include <stdio.h>

Object *Win, *ra;
struct Window *win;

#define GID_DW_HIDE 1
#define GID_DW_RIGHTPROP 2
#define GID_DW_BOTTOMPROP 3

struct Library *BGUIBase;
struct IntuitionBase * IntuitionBase;

void GimmeQuickDisplayWindow(void)
{
        Win = WindowObject,
            WINDOW_Title, "Test window",
            WINDOW_ScreenTitle, "BGUI test",
            WINDOW_Screen, NULL,
            WINDOW_SmartRefresh, FALSE,
            WINDOW_SizeRight, TRUE,
            WINDOW_NoBufferRP, TRUE,
            WINDOW_TBorderGroup,
                HGroupObject, HOffset(0), VOffset(0), Spacing(0),
                    StartMember,
                        ButtonObject,
                            VIT_BuiltIn,    BUILTIN_ARROW_UP,
                            BUTTON_EncloseImage, TRUE,
                            GA_ID,          GID_DW_HIDE,
                            BorderFrame,
                        EndObject,
                    EndMember,
                EndObject,
            WINDOW_RBorderGroup,
                VGroupObject,
                    StartMember,
                        PropObject,
                            PGA_Top,        0,
                            PGA_Total,      100,
                            PGA_Visible,    10,
                            PGA_Arrows,     TRUE,
                            PGA_NewLook,    TRUE,
                            // BorderFrame,
                            GA_ID,          GID_DW_RIGHTPROP,
                            GA_RelVerify,   TRUE,
                        EndObject,
                    EndMember,
                EndObject,
            WINDOW_BBorderGroup,
                HGroupObject,
                    StartMember,
                        PropObject,
                            PGA_Top,        0,
                            PGA_Total,      100,
                            PGA_Visible,    10,
                            PGA_Arrows,     TRUE,
                            PGA_Freedom,    FREEHORIZ,
                            PGA_NewLook,    TRUE,
                            GA_ID,          GID_DW_BOTTOMPROP,
                            GA_RelVerify,   TRUE,
                            BorderFrame,
                        EndObject,
                    EndMember,
                EndObject,
            WINDOW_MasterGroup,
                VGroupObject,
                    StartMember,
                        AreaObject,
                            AREA_MinWidth,100,
                            AREA_MinHeight,100,
                        EndObject,
                    EndMember,
                EndObject, /* Mastervgroup */
            EndObject;

}

int main(int argc, char **argv)
{
    ULONG sigmask;
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
                            case WMHI_CLOSEWINDOW:
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
