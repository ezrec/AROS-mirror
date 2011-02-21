/*
**  test9.c
**
**    Bug No. 58 test code provided by Anton Rolls
*/

#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>
#include <proto/bgui.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <stdio.h>


Object *WO_Window, *PG_TestProp;
struct Window *win;

#define ID_TEST_PROP 1

struct Library *BGUIBase;
struct IntuitionBase * IntuitionBase;

void GimmeQuickDisplayWindow(void)
{
  //  Create window object.
  WO_Window = WindowObject,
    //WINDOW_Title,           "TestProp",
    //WINDOW_AutoAspect,      TRUE,
    //WINDOW_AutoKeyLabel,    TRUE,
    WINDOW_MasterGroup,
      HGroupObject, //NormalOffset, NormalSpacing,

        StartMember, // top,total,visible
          PG_TestProp = HorizScroller("Hello", 50,100,1, ID_TEST_PROP), //FixMinHeight,
        EndMember,

      EndObject,
    EndObject;
}

int main(void)
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
        if(WO_Window)
        {
            win = WindowOpen(WO_Window);
            GetAttr(WINDOW_SigMask, WO_Window, &sigmask);

            while(!quit) {
                Wait(sigmask);

                if(sigmask & sigmask ) {
                    ULONG rc;

                    while((rc = HandleEvent(WO_Window)) != WMHI_NOMORE) {
                        switch(rc) {
                            case WMHI_CLOSEWINDOW:
                                quit = TRUE;
                                break;
                        }
                    }
                }
            }

            DisposeObject(WO_Window);
        }

        CloseLibrary(BGUIBase);
    }

    CloseLibrary((struct Library *)IntuitionBase);

    return 0;
}
