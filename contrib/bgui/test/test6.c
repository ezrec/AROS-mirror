/*
**  test6.c
**
**    Bug No. 23 test code provided by Jilles Tjoelker.
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

#include <stdio.h>

struct Library *BGUIBase=NULL;
struct IntuitionBase * IntuitionBase;

// In the E source, the font info objects had formatting.
// This is the "GUI" page.

#define ID_LFONT 1
#define ID_TFONT 1
#define ID_UITEM 1
#define ID_POPFILE 1
#define ID_EDIT 1
// For BGUI 41.8: prevents BCLR #7,8
#undef LISTV_Titles
#define LISTV_Titles TAG_IGNORE

// Object *w0,*w1,*w2,*i1,*i2,*i3;
ULONG weights[3]={15,200,300};
// The following are not in the E source:
STRPTR labels[]={"Nothing (-)","ARexx (A)",NULL};
ULONG umweights[]={50,7,100};

#define InfoMed InfoObj

Object *InitTest_6( void )
{
        Object *win;

        win = WindowObject,
                WINDOW_SmartRefresh, TRUE,
                WINDOW_AutoAspect, TRUE,
                WINDOW_MasterGroup, 
      VGroupObject,NormalSpacing,NormalOffset,
/*
        StartMember,HGroupObject,NarrowSpacing,
*/
          StartMember,InfoMed("Message list","bbb/11",0,1),FixMinHeight,EndMember,
          StartMember,ButtonObject,VIT_BuiltIn,BUILTIN_POPUP,GA_ID,ID_LFONT,EndObject,FixAspect(15,13),EndMember,
/*
        EndObject,FixMinHeight,EndMember,
        StartMember,HGroupObject,NarrowSpacing,
*/
          StartMember,InfoMed("Message text","aaa/9",0,1),FixMinHeight,EndMember,
/*
          StartMember,ButtonObject,VIT_BuiltIn,BUILTIN_POPUP,GA_ID,ID_TFONT,EndObject,FixAspect(15,13),EndMember,
        EndObject,FixMinHeight,EndMember,
*/
      EndObject,
                EndObject;

        return( win );
}

int main(argc,argv)
{
        Object *window;

        if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
        {
          printf("Could not open Intuition.library!\n");
          return -1;
        }

        if(BGUIBase=OpenLibrary(BGUINAME,BGUIVERSION))
        {
                if((window=InitTest_6())!=NULL
                && WindowOpen(window)!=NULL)
                {
                        ULONG signal;

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
                CloseLibrary(BGUIBase);
        }
        CloseLibrary((struct Library *)IntuitionBase);

}
