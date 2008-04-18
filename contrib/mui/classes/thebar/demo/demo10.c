
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <clib/alib_protos.h>
#include <mui/TheBar_mcc.h>
#include <string.h>
#include <stdio.h>
#include "SDI_compiler.h"

/***********************************************************************/

long __stack = 8192;
#if defined(__amigaos4__)
struct Library *IntuitionBase;
struct Library *MUIMasterBase;
struct IntuitionIFace *IIntuition;
struct MUIMasterIFace *IMUIMaster;
#else
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;
#endif

/***********************************************************************/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

/***********************************************************************/

#if defined(__amigaos4__)
#define CLOSELIB(lib, iface)              { if((iface) && (lib)) { DropInterface((APTR)(iface)); iface = NULL; CloseLibrary((struct Library *)lib); lib = NULL; } }
#define GETINTERFACE(iname, iface, base)  ((iface) = (APTR)GetInterface((struct Library *)(base), (iname), 1L, NULL))
#define DROPINTERFACE(iface)              { DropInterface((APTR)(iface)); iface = NULL; }
#else
#define CLOSELIB(lib, iface)              { if((lib)) { CloseLibrary((struct Library *)lib); lib = NULL; } }
#define GETINTERFACE(iname, iface, base)  TRUE
#define DROPINTERFACE(iface)              ((void)0)
#endif

/***********************************************************************/

struct MUIS_TheBar_Button buttons[] =
{
    {0, 0, "Ball1", "Ball1 help", MUIV_TheBar_ButtonFlag_Immediate|MUIV_TheBar_ButtonFlag_Selected, 6, NULL, NULL},
    {1, 1, "Ball2", "Ball2 help", MUIV_TheBar_ButtonFlag_Immediate, 								5, NULL, NULL},
    {2, 2, "Ball3", "Ball3 help", MUIV_TheBar_ButtonFlag_Immediate, 								3, NULL, NULL},
    {MUIV_TheBar_End,0,NULL,NULL,0,0,NULL,NULL}
};

/***********************************************************************/

const char *appareances[] = {"Images and text","Images","Text",NULL};
const char *labelPoss[] = {"Bottom","Top","Right","Left",NULL};

const char *usedClasses[] = {"TheBar.mcc",NULL};

int
main(UNUSED int argc,char **argv)
{
    int res;

    if((IntuitionBase = (APTR)OpenLibrary("intuition.library",39)) != NULL &&
       GETINTERFACE("main", IIntuition, IntuitionBase))
    {
        if((MUIMasterBase = OpenLibrary("muimaster.library",19)) != NULL &&
           GETINTERFACE("main", IMUIMaster, MUIMasterBase))
        {
            Object *app, *win, *tb, *pg;

            if ((app = ApplicationObject,
                    MUIA_Application_Title,         "TheBar Demo1",
                    MUIA_Application_Version,       "$VER: TheBarDemo1 1.0 (24.6.2003)",
                    MUIA_Application_Copyright,     "Copyright 2003 by Alfonso Ranieri",
                    MUIA_Application_Author,        "Alfonso Ranieri <alforan@tin.it>",
                    MUIA_Application_Description,   "TheBar example",
                    MUIA_Application_Base,          "THEBAREXAMPLE",
                    MUIA_Application_UsedClasses,   usedClasses,

                    SubWindow, win = WindowObject,
                        MUIA_Window_ID,             MAKE_ID('M','A','I','N'),
                        MUIA_Window_Title,          "TheBar Demo1",
                        WindowContents, VGroup,
                            Child, tb = TheBarObject,
                            	MUIA_Group_Horiz,			  TRUE,
				                MUIA_TheBar_Buttons,          buttons,
				                MUIA_TheBar_PicsDrawer,       "PROGDIR:Pics",
				                MUIA_TheBar_Strip,            "Balls.png",
				                MUIA_TheBar_StripCols,        3,
				                MUIA_TheBar_StripRows,        1,
				                MUIA_TheBar_StripHSpace,      0,
				                MUIA_TheBar_StripVSpace,      0,
				                MUIA_TheBar_EnableKeys,       TRUE,
				                MUIA_TheBar_IgnoreAppearance, TRUE,
                        		MUIA_TheBar_ViewMode,   MUIV_TheBar_ViewMode_Gfx,
                                MUIA_TheBar_Borderless, TRUE,
                                MUIA_TheBar_Sunny,      TRUE,
                                MUIA_TheBar_Raised,     FALSE,
                                MUIA_TheBar_NtRaiseActive, TRUE,
					            MUIA_TheBar_IgnoreAppearance, TRUE,
                            End,
                            Child, pg = VGroup,
                            	VirtualFrame,
			    			    MUIA_Background, MUII_PageBack,
                                MUIA_Group_PageMode,TRUE,

                                Child, VGroup,
                                	Child, VSpace(0),
	            					Child, HGroup,
                                		Child, HSpace(0),
                                		Child, TextObject,
                                        	MUIA_Text_Contents, "Page of ball 1",
										End,
                                		Child, HSpace(0),
                                    End,
                                	Child, VSpace(0),
                                End,

                                Child, VGroup,
                                	Child, VSpace(0),
	            					Child, HGroup,
                                		Child, HSpace(0),
                                		Child, TextObject,
                                        	MUIA_Text_Contents, "Page of ball 2",
										End,
                                		Child, HSpace(0),
                                    End,
                                	Child, VSpace(0),
                                End,

                                Child, VGroup,
                                	Child, VSpace(0),
	            					Child, HGroup,
                                		Child, HSpace(0),
                                		Child, TextObject,
                                        	MUIA_Text_Contents, "Page of ball 3",
										End,
                                		Child, HSpace(0),
                                    End,
                                	Child, VSpace(0),
                                End,
							End,
                    	End,
                    End,
                End))
            {
                ULONG sigs = 0, id;

                DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
                DoMethod(tb,MUIM_Notify,MUIA_TheBar_MouseOver,MUIV_EveryTime,tb,3,MUIM_Set,MUIA_TheBar_Active,MUIV_TriggerValue);
                DoMethod(tb,MUIM_Notify,MUIA_TheBar_Active,MUIV_EveryTime,pg,3,MUIM_Set,MUIA_Group_ActivePage,MUIV_TriggerValue);
                        
                set(win,MUIA_Window_Open,TRUE);

                while ((LONG)(id = DoMethod(app,MUIM_Application_NewInput,&sigs))!=MUIV_Application_ReturnID_Quit)
                {
                    if (sigs)
                    {
                        sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                        if (sigs & SIGBREAKF_CTRL_C) break;
                    }
                }

                MUI_DisposeObject(app);

                res = RETURN_OK;
            }
            else
            {
                printf("%s: can't create application\n",argv[0]);
                res = RETURN_FAIL;
            }

            CLOSELIB(MUIMasterBase, IMUIMaster);
        }
        else
        {
            printf("%s: Can't open muimaster.library ver 19 or higher\n",argv[0]);
            res = RETURN_ERROR;
        }

        CLOSELIB(IntuitionBase, IIntuition);
    }
    else
    {
        printf("%s: Can't open intuition.library ver 39 or higher\n",argv[0]);
        res = RETURN_ERROR;
    }

    return res;
}

/***********************************************************************/
