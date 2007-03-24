
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <mui/SpeedBar_mcc.h>
#include <mui/SpeedBarCfg_mcc.h>
#include <mui/BWin_mcc.h>

#include <string.h>
#include <stdio.h>

/***********************************************************************/

long __stack = 8192;
struct Library *MUIMasterBase;

/***********************************************************************/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

/***********************************************************************/

struct MUIS_SpeedBar_Button buttons[] =
{
    {0, "Rea_d", "Read selected.", 0, NULL},
    {1, "_Modify", "Modify selected.", 0, NULL},
    {2, "M_ove", "Move selected.", 0, NULL},
    {MUIV_SpeedBar_Spacer},
    {3, "De_lete", "Delete selected.", 0, NULL},
    {4, "_Get", "Get news.", 0, NULL},
    {5, "Ne_w", "New entry.", 0, NULL},
    {MUIV_SpeedBar_End},
};

/***********************************************************************/

Object *
ocheckmark(STRPTR  key,ULONG selected)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Checkmark,key))
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_Selected,selected,TAG_DONE);

    return obj;
}

/****************************************************************************/

Object *
ocycle(STRPTR key,STRPTR *labels,ULONG active)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Cycle,key,labels))
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_Cycle_Active,active,TAG_DONE);

    return obj;
}

/****************************************************************************/

Object *
obutton(STRPTR label)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Button,label))
        SetAttrs(obj,MUIA_CycleChain,TRUE,TAG_DONE);

    return obj;
}

/***********************************************************************/

STRPTR viewModeStrings[] =
{
    "Icons and text",
    "Icons only",
    "Text only",
    NULL
};

STRPTR lPosStrings[] =
{
    "Bottom",
    "Top",
    "Right",
    "Left",
    NULL
};

#define DEF_ViewMode   MUIV_SpeedBar_ViewMode_TextGfx
#define DEF_Borderless FALSE
#define DEF_Raising    FALSE
#define DEF_Sunny      FALSE
#define DEF_Scaled     FALSE
#define DEF_Underscore TRUE
#define DEF_BarSpacer  TRUE
#define DEF_LPos       MUIV_SpeedBar_LabelPosition_Bottom
#define DEF_UseBWin    TRUE

/***********************************************************************/

int
main(int argc,char **argv)
{
    int res;

    if (MUIMasterBase = OpenLibrary("muimaster.library",19))
    {
        Object *app, *win, *root, *sb,
               *viewMode, *borderless, *sunny,
               *raising, *scaled, *lPos,
               *barSpacer, *underscore, *bwin, *update;
        char   *s;
        ULONG  useBWin = TRUE;

        if (app = ApplicationObject,
                MUIA_Application_Title,       "BWinDemo3",
                MUIA_Application_Version,     "$VER: BWinDemo3 18.4 (13.6.2002)",
                MUIA_Application_Copyright,   "Copyright 2002 by Alfonso Ranieri",
                MUIA_Application_Author,      "Alfonso Ranieri <alforan@tin.it>",
                MUIA_Application_Description, "BWin.mcc test",
                MUIA_Application_Base,        "BWINTEST",

                SubWindow, win = MUI_NewObject(DEF_UseBWin ? MUIC_BWin : MUIC_Window,
                    MUIA_Window_ID,    MAKE_ID('M','A','I','N'),
                    MUIA_Window_Title, "BWinDemo3",

                    WindowContents, root = VGroup,

                        Child, HGroup,
                            GroupFrame,
                            Child, sb = SpeedBarObject,
                                MUIA_Group_Horiz,               TRUE,
                                MUIA_SpeedBar_ViewMode,         DEF_ViewMode,
                                MUIA_SpeedBar_Borderless,       DEF_Borderless,
                                MUIA_SpeedBar_RaisingFrame,     DEF_Raising,
                                MUIA_SpeedBar_Sunny,            DEF_Sunny,
                                MUIA_SpeedBar_SmallImages,      DEF_Scaled,
                                MUIA_SpeedBar_BarSpacer,        DEF_BarSpacer,
                                MUIA_SpeedBar_LabelPosition,    DEF_LPos,
                                MUIA_SpeedBar_EnableUnderscore, DEF_Underscore,
                                MUIA_SpeedBar_StripUnderscore,  TRUE,
                                MUIA_SpeedBar_PicsDrawer,       "PROGDIR:Pics",
                                MUIA_SpeedBar_Buttons,          buttons,
                                MUIA_SpeedBar_Strip,            "Main.toolbar",
                                MUIA_SpeedBar_StripButtons,     14,
                                MUIA_SpeedBar_SpacerIndex,      -1,
                            End,
                            Child, HSpace(0),
                        End,

                        Child, VSpace(0),

                        Child, VGroup,
                            GroupFrameT("Appareance"),

                            Child, HGroup,
                                Child, Label2(s = "_Toolbar buttons appear as"),
                                Child, viewMode = ocycle(s,viewModeStrings,DEF_ViewMode),
                            End,

                            Child, HGroup,
                                Child, Label2(s = "_BorderLess"),
                                Child, borderless = ocheckmark(s,DEF_Borderless),

                                Child, RectangleObject, MUIA_Weight, 1, End,

                                Child, Label2(s = "_Highlight"),
                                Child, sunny = ocheckmark(s,FALSE),

                                Child, RectangleObject, MUIA_Weight, 1, End,

                                Child, Label2(s = "_Raised"),
                                Child, raising = ocheckmark(s,DEF_Raising),

                                Child, RectangleObject, MUIA_Weight, 1, End,

                                Child, Label2(s = "_Scaled"),
                                Child, scaled = ocheckmark(s,DEF_Scaled),

                                Child, RectangleObject, MUIA_Weight, 1, End,
                            End,

                            Child, HGroup,
                                Child, Label2(s = "T_ext is placed at"),
                                Child, lPos = ocycle(s,lPosStrings,DEF_LPos),

                                Child, RectangleObject, MUIA_Weight, 1, End,

                                Child, Label2(s = "B_ar spacer"),
                                Child, barSpacer = ocheckmark(s,DEF_BarSpacer),

                                Child, RectangleObject, MUIA_Weight, 1, End,

                                Child, Label2(s = "U_nderscore"),
                                Child, underscore = ocheckmark(s,DEF_Underscore),
                            End,
                        End,

                        Child, VSpace(0),

                        Child, HGroup,
                            Child, Label2(s = "Use BW_in"),
                            Child, bwin = ocheckmark(s,TRUE),
                            Child, RectangleObject, MUIA_Weight, 1, End,
                            Child, update = obutton("_Update"),
                        End,

                        Child, VSpace(0),
                    End,
                End,
            End)
        {
            ULONG sigs = 0, id;

            DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
            DoMethod(update,MUIM_Notify,MUIA_Pressed,FALSE,app,2,MUIM_Application_ReturnID,TAG_USER);

            set(win,MUIA_Window_Open,TRUE);

            while ((id = DoMethod(app,MUIM_Application_NewInput,&sigs))!=MUIV_Application_ReturnID_Quit)
            {
                if (id==TAG_USER)
                {
                    ULONG viewModeVal, borderlessVal, sunnyVal, raisingVal,
                          scaledVal, lPosVal, barSpacerVal, underscoreVal, v;

                    get(bwin,MUIA_Selected,&v);
                    if (v!=useBWin)
                    {
                        Object *w;

                        set(win,MUIA_Window_Open,FALSE);
                        set(win,WindowContents,NULL);

                        if (w = MUI_NewObject(v ? MUIC_BWin : MUIC_Window,
                                MUIA_Window_ID,    MAKE_ID('M','A','I','N'),
                                MUIA_Window_Title, "BWinDemo3",
                                WindowContents,    root,
                            End)
                        {
                            DoMethod(app,OM_REMMEMBER,win);
                            MUI_DisposeObject(win);

                            win = w;

                            DoMethod(app,OM_ADDMEMBER,win);
                            DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

                            useBWin = v;
                        }
                        else set(win,WindowContents,root);

                        v = TRUE;
                    }
                    else v = FALSE;

                    get(viewMode,MUIA_Cycle_Active,&viewModeVal);
                    get(borderless,MUIA_Selected,&borderlessVal);
                    get(lPos,MUIA_Cycle_Active,&lPosVal);
                    get(raising,MUIA_Selected,&raisingVal);
                    get(scaled,MUIA_Selected,&scaledVal);
                    get(sunny,MUIA_Selected,&sunnyVal);
                    get(barSpacer,MUIA_Selected,&barSpacerVal);
                    get(underscore,MUIA_Selected,&underscoreVal);

                    SetAttrs(sb,MUIA_SpeedBar_ViewMode,         viewModeVal,
                                MUIA_SpeedBar_Borderless,       borderlessVal,
                                MUIA_SpeedBar_RaisingFrame,     raisingVal,
                                MUIA_SpeedBar_SmallImages,      scaledVal,
                                MUIA_SpeedBar_Sunny,            sunnyVal,
                                MUIA_SpeedBar_LabelPosition,    lPosVal,
                                MUIA_SpeedBar_BarSpacer,        barSpacerVal,
                                MUIA_SpeedBar_EnableUnderscore, underscoreVal,
                                TAG_DONE);

                    if (v) set(win,MUIA_Window_Open,TRUE);
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
            printf("%s: Can't create application\n",argv[0]);
            res = RETURN_FAIL;
        }

        CloseLibrary(MUIMasterBase);
    }
    else
    {
        printf("%s: Can't open muimaster.library ver. 19 or higher\n",argv[0]);
        res = RETURN_ERROR;
    }

    return res;
}

/***********************************************************************/
