/*
**  Test 1.c
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

#ifdef __AROS__
#include <proto/alib.h>
#else
#include <clib/alib_protos.h>
#endif

#include <stdio.h>

struct Library *BGUIBase=NULL;
struct IntuitionBase * IntuitionBase;

enum
{
        WIN1_MASTER, WIN1_PROP1, WIN1_CYCLE1, WIN1_BUTTON1, WIN1_STRING1, 

        WIN1_NUMGADS
};

Object *Test_1Objs[WIN1_NUMGADS];

STRPTR cycle_labels[]=
{
        "Label 1",
        "Label 2",
        "Label 3",
        NULL
};

Object *InitTest_1( void )
{
        Object *win;
        Object **ar = Test_1Objs;

printf("%s: Trying to create Window Object\n",__FILE__);
        win = WindowObject,
                WINDOW_SmartRefresh, TRUE,
                WINDOW_AutoAspect, TRUE,
                WINDOW_MasterGroup, ar[WIN1_MASTER] = VGroupObject,
                        FRM_Type,FRTYPE_NEXT,
                        FRM_Title,NULL,
                        StartMember, ar[WIN1_PROP1] = PropObject,
                                LAB_Label, "Prop",
                                LAB_Underscore, '_',
                                PGA_Total, 25,
                                PGA_Visible, 5,
                                PGA_Top, 0,
                                PGA_Total, 25,
                                PGA_Arrows, TRUE,
                                PGA_NewLook, TRUE,
                                PGA_Freedom, FREEHORIZ,
                                GA_ID, WIN1_PROP1,
                        EndObject,  EndMember,
                        StartMember, ar[WIN1_CYCLE1] = CycleObject,
                                ButtonFrame,
                                LAB_Label, "Cycle",
                                LAB_Place, PLACE_LEFT,
                                GA_ID, WIN1_CYCLE1,
                                CYC_Labels, cycle_labels,
                                CYC_Active, 0,
                                CYC_Popup, TRUE,
                        EndObject, EndMember,
                        StartMember, ar[WIN1_BUTTON1] = ButtonObject,
                                ButtonFrame,
                                LAB_Label, "Button",
                                LAB_Underscore, '_',
                                GA_ID, WIN1_BUTTON1,
                        EndObject, EndMember,
                        StartMember, ar[WIN1_STRING1] = StringObject,
                                RidgeFrame,
                                LAB_Label, "String",
                                LAB_Place, PLACE_LEFT,
                                LAB_Underscore, '_',
                                STRINGA_TextVal, "NULL",
                                STRINGA_MaxChars, 65,
                                STRINGA_MinCharsVisible, 0,
                                GA_ID, WIN1_STRING1,
                        EndObject, EndMember,
                EndObject,
        EndObject;

printf("%s %s %d: win = %x\n",__FILE__, "InitTest_1()" ,__LINE__,win);

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
printf("%s: Trying to open BGUI Lib!\n",__FILE__);
        if(BGUIBase=OpenLibrary(BGUINAME,BGUIVERSION))
        {
printf("%s: Successfully opened bgui.library!\n",__FILE__);
                if((window=InitTest_1())!=NULL
                && WindowOpen(window)!=NULL)
                {
                        ULONG signal;
printf("%s %s %d: survived Init_Test!\n",__FILE__, "main()" ,__LINE__);
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
