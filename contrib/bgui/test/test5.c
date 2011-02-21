/*
**  test5.c
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

#define ID_W0 500

Object *w[1] = { NULL };

#define NWObj(v,id)\
   StringObject,\
      STRINGA_LongVal,       v,\
      STRINGA_MaxChars,      3,\
      STRINGA_IntegerMin,    1,\
      STRINGA_IntegerMax,    999,\
      STRINGA_Justification, GACT_STRINGCENTER,\
      GA_ID,                 id,\
      FuzzRidgeFrame,\
   EndObject, Weight(v)
   
Object *InitTest_5( void )
{
        Object *win;

        win = WindowObject,
                WINDOW_SmartRefresh, TRUE,
                WINDOW_AutoAspect, TRUE,
                WINDOW_MasterGroup, 
                        VGroupObject, NormalSpacing,
                                StartMember, w[0]=NWObj( 25, ID_W0), EndMember,
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
                if((window=InitTest_5())!=NULL
                && WindowOpen(window)!=NULL)
                {
                        IPTR signal;

                        if(GetAttr(WINDOW_SigMask,window,&signal)
                        && signal!=0)
                        {
                                for(;;)
                                {
                                        IPTR rc,id;

                                        Wait(signal);
                                        switch((rc=HandleEvent(window)))
                                        {
                                                case ID_W0:
                                                        id = rc - ID_W0;
                                                        GetAttr(STRINGA_LongVal, w[id], &rc);
                                                        SetAttrs(w[id], LGO_Weight, rc, TAG_DONE);
                                                        continue;
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
