
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

long __stack = 32000;
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

const char *pics[] =
{
    "CDGet",
    "CDSave",
    "CDStop",
    "CDDisc",
    "CDMatches",
    "CDEdit",
    NULL
};

struct MUIS_TheBar_Button buttons[] =
{
    {0, 0,  "_Get", "Get the disc.", 0, 0, NULL, NULL},
    {2, 1,  "_Stop", "Stop the connection.", 0, 0, NULL, NULL},
    {MUIV_TheBar_ButtonSpacer, -1, NULL, NULL, 0, 0, NULL, NULL},
    {1, 2,  "Sa_ve", "Save the disc.", 0, 0, NULL, NULL},
    {MUIV_TheBar_BarSpacer, -1, NULL, NULL, 0, 0, NULL, NULL},
    {3,  3, "_Disc", "Disc page.", 0, (1<<4)|(1<<5), NULL, NULL},
    {4,  4, "_Matches", "Matches page.", MUIV_TheBar_ButtonFlag_Selected, (1<<3)|(1<<5), NULL, NULL},
    {5,  5, "_Edit", "Edit page.", 0, (1<<3)|(1<<4), NULL, NULL},
    {MUIV_TheBar_End, -1, NULL, NULL, 0, 0, NULL, NULL},
};

const char *rowss[]       = {"0","1","2","3","6",NULL};
const char *colss[]       = {"0","1","2","3","4","5","6",NULL};
const char *viewModes[]   = {"Images and text","Images","Text",NULL};
const char *barPoss[]     = {"Left","Center","Right",NULL};
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
            Object *app, *bwin, *cwin, *mg, *sb, *viewMode, *barPos, *labelPos, *borderless,
                   *sunny, *raised, *scaled, *enableKeys, *barSpacer, *freeHoriz, *freeVert,
                   *update, *rows, *cols, *rb, *rs, *ri, *ra;

            if ((app = ApplicationObject,
                    MUIA_Application_Title,        "TheBar Demo2",
                    MUIA_Application_Version,      "$VER: TheBarDemo2 1.0 (24.6.2003)",
                    MUIA_Application_Copyright,    "Copyright 2003 by Alfonso Ranieri",
                    MUIA_Application_Author,       "Alfonso Ranieri <alforan@tin.it>",
                    MUIA_Application_Description,  "TheBar example",
                    MUIA_Application_Base,         "THEBAREXAMPLE",
                    MUIA_Application_UsedClasses,  usedClasses,

                    SubWindow, bwin = WindowObject,
                        MUIA_Window_ID,    MAKE_ID('B','W','I','N'),
                        MUIA_Window_Title, "TheBar Demo2",
                        WindowContents, mg = VGroup,
                            Child, sb = TheBarObject,
                                MUIA_TheBar_Frame, 	          TRUE,
                                MUIA_Group_Horiz,             TRUE,
                                MUIA_TheBar_IgnoreAppearance, TRUE,
                                MUIA_TheBar_ViewMode,         MUIV_TheBar_ViewMode_Gfx,
                                MUIA_TheBar_Buttons,          buttons,
                                MUIA_TheBar_PicsDrawer,       "PROGDIR:PICS",
                                MUIA_TheBar_Pics,             pics,
                                MUIA_TheBar_DragBar,          TRUE,
                            End,
                        End,
                    End,

                    SubWindow, cwin = WindowObject,
                        MUIA_Window_ID,    MAKE_ID('C','W','I','N'),
                        MUIA_Window_Title, "TheBar Demo2 Control",

                        WindowContents, VGroup,

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
                                    Child, barPos = MUI_MakeObject(MUIO_Cycle,NULL,barPoss),
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
                                                Child, enableKeys = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel1("Enable keys"),
                                                Child, barSpacer = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel1("Bar spacer"),
                                                Child, freeHoriz = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Free horiz"),
                                                Child, freeVert = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Free vert"),
                                                Child, RectangleObject, MUIA_FixHeightTxt,"\n",End,
                                                Child, RectangleObject, MUIA_FixHeightTxt,"\n",End,
                                                Child, rb = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Remove bar spacers"),
                                                Child, rs = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Remove button spacers"),
                                                Child, ri = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Remove image spacers"),
                                                Child, ra = MUI_MakeObject(MUIO_Checkmark,NULL),
                                                Child, LLabel("Remove all"),
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

                DoMethod(bwin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
                DoMethod(cwin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
                DoMethod(update,MUIM_Notify,MUIA_Pressed,FALSE,app,2,MUIM_Application_ReturnID,TAG_USER);

                set(cwin,MUIA_Window_Open,TRUE);
                set(bwin,MUIA_Window_Open,TRUE);

                while ((id = DoMethod(app,MUIM_Application_NewInput,&sigs))!=MUIV_Application_ReturnID_Quit)
                {
                    if (id==(LONG)TAG_USER)
                    {
                        IPTR rowsV = 0,
                             colsV = 0,
                             viewModeV = 0,
                             barPosV = 0,
                             labelPosV = 0,
                             borderlessV = 0,
                             sunnyV = 0,
                             raisedV = 0,
                             scaledV = 0,
                             enableKeysV = 0,
                             barSpacerV = 0,
                             freeHorizV = 0,
                             freeVertV = 0,
                             rbV = 0,
                             rsV = 0,
                             riV = 0,
                             raV = 0,
                             rem = 0;

                        get(rows,MUIA_Radio_Active,&rowsV);
                        if (rowsV==4) rowsV = 6;
                        get(cols,MUIA_Radio_Active,&colsV);

                        get(viewMode,MUIA_Cycle_Active,&viewModeV);
                        get(barPos,MUIA_Cycle_Active,&barPosV);
                        get(labelPos,MUIA_Cycle_Active,&labelPosV);

                        get(borderless,MUIA_Selected,&borderlessV);
                        get(sunny,MUIA_Selected,&sunnyV);
                        get(raised,MUIA_Selected,&raisedV);
                        get(scaled,MUIA_Selected,&scaledV);
                        get(enableKeys,MUIA_Selected,&enableKeysV);
                        get(barSpacer,MUIA_Selected,&barSpacerV);
                        get(freeHoriz,MUIA_Selected,&freeHorizV);
                        get(freeVert,MUIA_Selected,&freeVertV);

                        get(rb,MUIA_Selected,&rbV);
                        get(rs,MUIA_Selected,&rsV);
                        get(ri,MUIA_Selected,&riV);
                        get(ra,MUIA_Selected,&raV);
                        rem = 0;
                        if (rbV) rem |= MUIV_TheBar_RemoveSpacers_Bar;
                        if (rsV) rem |= MUIV_TheBar_RemoveSpacers_Button;
                        if (riV) rem |= MUIV_TheBar_RemoveSpacers_Image;
                        if (raV) rem |= MUIV_TheBar_RemoveSpacers_All;

                        SetAttrs(sb,MUIA_TheBar_Columns,      colsV,
                                    MUIA_TheBar_Rows,         rowsV,
                                    MUIA_TheBar_ViewMode,     viewModeV,
                                    MUIA_TheBar_Borderless,   borderlessV,
                                    MUIA_TheBar_Sunny,        sunnyV,
                                    MUIA_TheBar_Raised,       raisedV,
                                    MUIA_TheBar_Scaled,       scaledV,
                                    MUIA_TheBar_EnableKeys,   enableKeysV,
                                    MUIA_TheBar_BarSpacer,    barSpacerV,
                                    MUIA_TheBar_FreeHoriz,    freeHorizV,
                                    MUIA_TheBar_FreeVert,     freeVertV,
                                    MUIA_TheBar_BarPos,       barPosV,
                                    MUIA_TheBar_LabelPos,     labelPosV,
                                    MUIA_TheBar_RemoveSpacers,rem,
                                    TAG_DONE);
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
