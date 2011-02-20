/*
**  test2.c
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

#include <stdio.h>

struct Library *BGUIBase=NULL;
struct IntuitionBase * IntuitionBase;


UBYTE *NotifLabels[] = { "Enabled-->", "Disabled-->", "Still Disabled-->", NULL };

Object *InitTest_2( void )
{
        Object *win;

        win = WindowObject,
                WINDOW_SmartRefresh, TRUE,
                WINDOW_AutoAspect, TRUE,
                WINDOW_MasterGroup, 
                        VGroupObject, NormalSpacing,
                                StartMember, IndicatorFormat( 0, 100, 0, IDJ_CENTER, "%ld%%" ), FixMinHeight, EndMember,
                                StartMember, VertProgress( NULL, 0, 100, 0 ), EndMember,
                        EndObject,
                        VGroupObject, NormalOffset, NormalSpacing,
                                StartMember, TitleSeperator( "Conditional" ), EndMember,
                                StartMember,
                                        HGroupObject, NormalSpacing,
                                                StartMember, PrefCycle( NULL, NotifLabels, 0, 0 ), EndMember,
                                                StartMember, PrefButton( "Target", 0 ), EndMember,
                                        EndObject, FixMinHeight,
                                EndMember,
                                StartMember, TitleSeperator( "Map-List" ), EndMember,
                                StartMember,
                                        HGroupObject, NormalSpacing,
                                                StartMember,
                                                        VGroupObject, NormalSpacing,
                                                                StartMember, IndicatorFormat( 0, 100, 0, IDJ_CENTER, "%ld%%" ), FixMinHeight, EndMember,
                                                                StartMember, HorizProgress( NULL, 0, 100, 0 ), EndMember,
                                                        EndObject,
                                                EndMember,
                                                StartMember, VertSlider( NULL, 0, 100, 0, 0 ), FixWidth( 16 ), EndMember,
                                                StartMember, VertScroller( NULL, 0, 101, 1, 0 ), FixWidth( 16 ), EndMember,
                                                StartMember, VertSlider( NULL, 0, 100, 0, 0 ), FixWidth( 16 ), EndMember,
                                                StartMember,
                                                        VGroupObject, NormalSpacing,
                                                                StartMember, IndicatorFormat( 0, 100, 0, IDJ_CENTER, "%ld%%" ), FixMinHeight, EndMember,
                                                                StartMember, VertProgress( NULL, 0, 100, 0 ), EndMember,
                                                        EndObject,
                                                EndMember,
                                        EndObject,
                                EndMember,
                        EndObject,
                EndObject;

        return( win );
}

int main(int argc,char **argv)
{
        Object *window;

        if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
        {
          printf("Could not open Intuition.library!\n");
          return -1;
        }

        if((BGUIBase=OpenLibrary(BGUINAME,BGUIVERSION)))
        {
                if((window=InitTest_2())!=NULL
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

        return 0;
}
