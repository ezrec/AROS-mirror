
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <clib/alib_protos.h>
#include <mui/TheBar_mcc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

const char * pics[] =
{
    "Add",
    "Clone",
    "Edit",
    "Remove",
    "Search",
    "Prefs",
    "Login",
    NULL
};

const char * spics[] =
{
    "Add_S",
    "Clone_S",
    "Edit_S",
    "Remove_S",
    "Search_S",
    "Prefs_S",
    "Login_S",
    NULL
};

const char * dpics[] =
{
    "Add_G",
    "Clone_G",
    "Edit_G",
    "Remove_G",
    "Search_G",
    "Prefs_G",
    "Login_G",
    NULL
};

struct MUIS_TheBar_Button buttons[] =
{
    {0,     0, "_Get", "Get the disc.", 0, 0, NULL, NULL},
    {1,     1, "Sa_ve", "Save the disc.", 0, 0, NULL, NULL},
    {2,     2, "_Stop", "Stop the connection.", 0, 0, NULL, NULL},
    {MUIV_TheBar_BarSpacer,-1, NULL, NULL, 0, 0, NULL, NULL},
    {3,     3, "_Disc", "Disc page.", 0, 0, NULL, NULL},
    {MUIV_TheBar_ButtonSpacer,-1, NULL, NULL, 0, 0, NULL, NULL},
    {0,     4, "_Stop2", "Stop2 the connection.", 0, 0, NULL, NULL},
    {4,     5, "_Matches", "Matches page.", MUIV_TheBar_ButtonFlag_Immediate|MUIV_TheBar_ButtonFlag_Selected, 1<<6, NULL, NULL},
    {MUIV_TheBar_ImageSpacer,-1, NULL, NULL, 0, 0, NULL, NULL},
    {5,     6, "_Edit", "Edit page.", MUIV_TheBar_ButtonFlag_Immediate, 1<<5, NULL, NULL},
    {MUIV_TheBar_End, 0, NULL, NULL, 0, 0, NULL, NULL},
};

const char *rowss[]       = {"0","1","2","3","6",NULL};
const char *colss[]       = {"0","1","2","3","4","5","6",NULL};
const char *viewModes[]   = {"Images and text","Images","Text",NULL};
const char *buttonsPoss[] = {"Left","Center","Right",NULL};
const char *labelPoss[]   = {"Bottom","Top","Right","Left",NULL};

const char *usedClasses[] = {"TheBar.mcc",NULL};

int
main(UNUSED int argc,char **argv)
{
    int res;

    if ((IntuitionBase = (APTR)OpenLibrary("intuition.library",39)) != NULL &&
        GETINTERFACE("main", IIntuition, IntuitionBase))
    {
        if ((MUIMasterBase = OpenLibrary("muimaster.library",19)) != NULL &&
            GETINTERFACE("main", IMUIMaster, MUIMasterBase))
        {
            Object *app, *win, *mg, *sb, *viewMode, *buttonsPos, *labelPos, *borderless, *sunny,
                   *raised, *scaled, *keys, *bspacer, *fixed, *free, *update, *rows, *cols, *s4, *h0,
                   *rb, *rs, *ri;

            if ((app = ApplicationObject,
                    MUIA_Application_Title,        "TheBar Demo3",
                    MUIA_Application_Version,      "$VER: TheBarDemo3 1.0 (24.6.2003)",
                    MUIA_Application_Copyright,    "Copyright 2003 by Alfonso Ranieri",
                    MUIA_Application_Author,       "Alfonso Ranieri <alforan@tin.it>",
                    MUIA_Application_Description,  "TheBar example",
                    MUIA_Application_Base,         "THEBAREXAMPLE",
                    MUIA_Application_UsedClasses,  usedClasses,

                    SubWindow, win = WindowObject,
                        MUIA_Window_ID,    MAKE_ID('M','A','I','N'),
                        MUIA_Window_Title, "TheBar Demo3",

                        WindowContents, mg = VGroup,

                            Child, sb = TheBarObject,
                                MUIA_Group_Horiz,             TRUE,
                                MUIA_TheBar_IgnoreAppearance, TRUE,
                                MUIA_TheBar_ViewMode,         MUIV_TheBar_ViewMode_Gfx,
                                MUIA_TheBar_Buttons,          buttons,
                                MUIA_TheBar_PicsDrawer,       "PROGDIR:Pics",
                                MUIA_TheBar_Pics,             pics,
                                MUIA_TheBar_SelPics,          spics,
                                MUIA_TheBar_DisPics,          dpics,
                            End,

                            Child, VGroup,
                                GroupFrameT("Appearance"),
                                Child, ColGroup(2),
                                    Child, Label2("Rows"),
                                    Child, HGroup,
                                        Child, rows = MUI_MakeObject(MUIO_Radio,NULL,rowss),
                                        Child, HSpace(0),
                                    End,
                                    Child, Label2("Columns"),
                                    Child, HGroup,
                                        Child, cols = MUI_MakeObject(MUIO_Radio,NULL,colss),
                                        Child, HSpace(0),
                                    End,
                                    Child, Label2("ViewMode"),
                                    Child, viewMode = MUI_MakeObject(MUIO_Cycle,NULL,viewModes),
                                    Child, Label2("Buttons position"),
                                    Child, buttonsPos = MUI_MakeObject(MUIO_Cycle,NULL,buttonsPoss),
                                    Child, Label2("Label position"),
                                    Child, labelPos = MUI_MakeObject(MUIO_Cycle,NULL,labelPoss),
                                End,
                                Child, ScrollgroupObject,
                                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                                    MUIA_Scrollgroup_Contents, VirtgroupObject,
                                        MUIA_Frame, MUIV_Frame_Virtual,
                                        Child, HGroup,
                                            Child, ColGroup(2),
                                                Child, borderless = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel1("Borderless"),
                                                Child, sunny = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel1("Sunny"),
                                                Child, raised = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel1("Raised"),
                                                Child, scaled = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel1("Scaled"),
                                                Child, keys = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel1("Enable keys"),
                                                Child, bspacer = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel1("Bar spacer"),
                                                Child, fixed = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Fixed"),
                                                Child, free = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Disable 0"),
                                                Child, h0 = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Hide 0"),
                                                Child, s4 = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Sleep 4"),
                                                Child, RectangleObject, MUIA_FixHeightTxt,"a",End,
                                                Child, RectangleObject, MUIA_FixHeightTxt,"a",End,
                                                Child, rb = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Remove Bars"),
                                                Child, rs = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Remove Spaces"),
                                                Child, ri = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Remove Images"),
                                            End,
                                            Child, HSpace(0),
                                        End,
                                        Child, VSpace(0),
                                    End,
                                End,
                                Child, update = MUI_MakeObject(MUIO_Button,"_Update"),
                            End,
                        End,
                    End,
                End) != NULL)
            {
                ULONG sigs = 0;
                LONG id;

                set(rows,MUIA_Group_Horiz,TRUE);
                set(cols,MUIA_Group_Horiz,TRUE);
                set(viewMode,MUIA_Cycle_Active,1);
                set(s4,MUIA_Selected,TRUE);
                DoMethod(sb,MUIM_TheBar_SetAttr,4,MUIA_TheBar_Attr_Sleep,TRUE);

                DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
                DoMethod(update,MUIM_Notify,MUIA_Pressed,FALSE,app,2,MUIM_Application_ReturnID,TAG_USER);

                set(win,MUIA_Window_Open,TRUE);

                while ((id = DoMethod(app,MUIM_Application_NewInput,&sigs))!=MUIV_Application_ReturnID_Quit)
                {
                    if (id==(LONG)TAG_USER)
                    {
                        IPTR c = 0,
                             r = 0,
                             vm = 0,
                             bp = 0,
                             lp = 0,
                             br = 0,
                             su = 0,
                             ra = 0,
                             sc = 0,
                             k = 0,
                             bs = 0,
                             fi = 0,
                             fr = 0,
                             hl0 = 0,
                             sl4 = 0,
                             rbl = 0,
                             rsl = 0,
                             ril = 0,
                             rem = 0;

                        get(rows,MUIA_Radio_Active,&r);
                        if (r==4) r = 6;
                        get(cols,MUIA_Radio_Active,&c);

                        get(viewMode,MUIA_Cycle_Active,&vm);
                        get(buttonsPos,MUIA_Cycle_Active,&bp);
                        get(labelPos,MUIA_Cycle_Active,&lp);

                        get(borderless,MUIA_Selected,&br);
                        get(sunny,MUIA_Selected,&su);
                        get(raised,MUIA_Selected,&ra);
                        get(scaled,MUIA_Selected,&sc);
                        get(keys,MUIA_Selected,&k);
                        get(bspacer,MUIA_Selected,&bs);
                        get(fixed,MUIA_Selected,&fi);
                        get(free,MUIA_Selected,&fr);

                        get(h0,MUIA_Selected,&hl0);
                        get(s4,MUIA_Selected,&sl4);

                        get(rb,MUIA_Selected,&rbl);
                        get(rs,MUIA_Selected,&rsl);
                        get(ri,MUIA_Selected,&ril);
                        rem = 0;
                        if (rbl) rem |= MUIV_TheBar_RemoveSpacers_Bar;
                        if (rsl) rem |= MUIV_TheBar_RemoveSpacers_Button;
                        if (ril) rem |= MUIV_TheBar_RemoveSpacers_Image;

                        DoMethod(mg,MUIM_Group_InitChange);
                        DoMethod(sb,MUIM_Group_InitChange);

                        SetAttrs(sb,MUIA_TheBar_Columns,      c,
                                    MUIA_TheBar_Rows,         r,
                                    MUIA_TheBar_ViewMode,     vm,
                                    MUIA_TheBar_Borderless,   br,
                                    MUIA_TheBar_Sunny,        su,
                                    MUIA_TheBar_Raised,       ra,
                                    MUIA_TheBar_Scaled,       sc,
                                    MUIA_TheBar_EnableKeys,   k,
                                    MUIA_TheBar_BarSpacer,    bs,
                                    MUIA_TheBar_RemoveSpacers,rem,
                                    MUIA_TheBar_LabelPos,     lp,
                                    TAG_DONE);

                        DoMethod(sb,MUIM_TheBar_SetAttr,0,MUIA_TheBar_Attr_Hide,hl0);
                        DoMethod(sb,MUIM_TheBar_SetAttr,4,MUIA_TheBar_Attr_Sleep,sl4);

                        DoMethod(sb,MUIM_TheBar_SetAttr,6,MUIA_TheBar_Attr_Disabled,fr);

                        DoMethod(sb,MUIM_Group_ExitChange);
                        DoMethod(mg,MUIM_Group_ExitChange);
                    }

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
