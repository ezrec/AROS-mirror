/*
**  Test 4.c
**
**    GUI-Source. Created with BGUIBuilder V1.0
*/

#ifndef LIBRARIES_BGUI_H
#include <libraries/bgui.h>
#endif

#ifndef LIBRARIES_BGUI_MACROS_H
#include <libraries/bgui_macros.h>
#endif

#ifndef BGUI_PROTO_H
#include <proto/bgui.h>
#endif

#include <proto/intuition.h>
#include <proto/exec.h>
#include <gadgets/gradientslider.h>

#include <stdio.h>

struct Library *BGUIBase=NULL;
struct IntuitionBase * IntuitionBase;

enum
{
        WIN4_MASTER, WIN4_GRSLIDER1,

        WIN4_NUMGADS
};

Object *Test_4Objs[WIN4_NUMGADS];

/*
**      Ensure correct operation on ECS.
**/
#define GRADCOLORS 4

static WORD penns[ GRADCOLORS + 1 ] =
{
     1,2,3,~0
};

Object *InitTest_4( void )
{
        Object *win;
        Object **ar = Test_4Objs;

        win = WindowObject,
                WINDOW_SmartRefresh, TRUE,
                WINDOW_AutoAspect, TRUE,
                WINDOW_MasterGroup, ar[WIN4_MASTER] = VGroupObject,
                        FRM_Type,FRTYPE_NEXT,
                        FRM_Title,NULL,
                        StartMember, ar[WIN4_GRSLIDER1]= ExternalObject,
                          EXT_MinWidth,           10,
                          EXT_MinHeight,          10,
                          EXT_ClassID,            "gradientslider.gadget",
                          EXT_NoRebuild,          TRUE,
                          GRAD_PenArray,          penns,
                          PGA_Freedom,            LORIENT_VERT,
                          GA_ID,                  WIN4_GRSLIDER1,
                          EndObject,
                        EndMember,
                EndObject,
        EndObject;

        return( win );
}

int main(int argc,char **argv)
{
        Object *window;
        struct Library *GradientSliderBase;

        if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
        {
          printf("Could not open Intuition.library!\n");
          return -1;
        }

        if((BGUIBase=OpenLibrary(BGUINAME,BGUIVERSION)))
        {
printf("trying to open gradientslider.gadget\n");
    if((GradientSliderBase=OpenLibrary("Gadgets/gradientslider.gadget",39L)))
    {
printf("opened gradientslider.gadget\n");
                        if((window=InitTest_4())!=NULL
                        && WindowOpen(window)!=NULL)
                        {
                                IPTR signal;

                                if(GetAttr(WINDOW_SigMask,window,&signal)
                                && signal!=0)
                                {
                                        for(;;)
                                        {
                                                Wait(signal);
                                                switch(HandleEvent(window))
                                                {
                                                        case WMHI_CLOSEWINDOW:
                                                                break;
                                                        case WMHI_NOMORE:
                                                        default:
                                                                continue;
                                                }
                                                break;
                                        }
                                }
                                DisposeObject(window);
                        }
                        CloseLibrary(GradientSliderBase);
                }
                CloseLibrary(BGUIBase);
        }

        CloseLibrary((struct Library *)IntuitionBase);

        return 0;
}
