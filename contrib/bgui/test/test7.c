/*
**  test7.c
**
**    Bug No. 27 test code provided by Janne Jalkanen
*/

#include <exec/types.h>
#ifndef LIBRARIES_BGUI_H
#include <libraries/bgui.h>
#endif


#ifndef LIBRARIES_BGUI_MACROS_H
#include <libraries/bgui_macros.h>
#endif

#ifdef __AROS__
#include <proto/bgui.h>
#include <proto/intuition.h>
#else
#ifndef PRAGMAS_BGUI_PRAGMAS_H
#include <pragmas/bgui_pragmas.h>
#endif

#ifndef CLIB_INTUITION_PROTOS_H
#include <clib/intuition_protos.h>
#endif
#endif

#ifndef LIBRARIES_GADTOOLS_H
#include <libraries/gadtools.h>
#endif


#include <proto/exec.h>
#include <proto/bgui.h>

#include <stdio.h>

#define Local static

struct PrefsWindow {
    Object          *Win;
    struct Window   *win;
    Object          *VMDir, *PageSize, *ExtStackSize,
                    *MainFont, *ListFont, *DispName, *ColorPreview,
                    *MaxUndo, *FlushLibs, *ExtNiceVal, *ExtNiceValI,
                    *ExtPriority, *ExtPriorityI, *PreviewMode, *Confirm;

    Object          *Save, *Use, *Cancel;
};

Local ULONG Cyc2Page[] = { MX_Active, PAGE_Active, TAG_END };
Local UBYTE *Prefspages[] = { "S_ystem","_GUI", "M_isc", NULL };

struct Library *BGUIBase = NULL;
struct IntuitionBase * IntuitionBase;

#define GetStr(x) "Jotain"

struct PrefsWindow prefsw = {0};

Object *GimmePrefsWindow( VOID )
{
    Object *c, *p;

    if(!prefsw.Win) {
        prefsw.Win = WindowObject,
            WINDOW_Title,       "Main Preferences",
            WINDOW_ScreenTitle, "Foo",
            WINDOW_ScaleWidth,  25,
            WINDOW_LockHeight,  TRUE,
            WINDOW_AutoKeyLabel,TRUE,
            WINDOW_MasterGroup,
                VGroupObject, NormalHOffset, NormalVOffset, NarrowSpacing,
                    StartMember,
                        c = Tabs( NULL, Prefspages, 0, 0 ),
                    EndMember,
                    StartMember,
                        p = PageObject,
                                PageMember,
                                    VGroupObject, NarrowSpacing,
                                        VarSpace( 50 ),
                                    EndObject,
                                PageMember,
                                    VGroupObject, NarrowSpacing, NormalHOffset, NormalVOffset,
                                        VarSpace(50),
                                    EndObject,
                                PageMember,
                                    VGroupObject, NarrowSpacing, NormalVOffset, NormalHOffset,
                                        VarSpace(50),
                                    EndObject,

                            EndObject, /* PAGE OBJECT*/
                    EndMember,
                EndObject,
            EndObject;

        /* If window opened OK, then set cycle gadgets.. */

        if(prefsw.Win) {
            AddMap(c,p,Cyc2Page);
        } else {
            printf("Darnicles. Can't create prefs window\n");
        }
    }

    if( prefsw.Win ) {
        if(( prefsw.win = WindowOpen( prefsw.Win ) )) {
            /* nothing here */
        } else {
            printf("Darnicles. Can't open prefs window\n");
            DisposeObject( prefsw.Win );
            prefsw.Win = NULL;
        }
    }
    return prefsw.Win;
}

Object *Win;

int main(void)
{
    ULONG sigmask;
    BOOL quit = FALSE;
    struct Window *win;

    if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
    {
      printf("Could not open Intuition.library!\n");
      return -1;
    }

    if((BGUIBase=OpenLibrary(BGUINAME,0L))) {
        Win = GimmePrefsWindow();
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

        CloseLibrary(BGUIBase);
    }

    CloseLibrary((struct Library *)IntuitionBase);

    return 0;
}
