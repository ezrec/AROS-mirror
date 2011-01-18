
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
    {0, "_Get", "Get the disc.", 0, NULL},
    {1, "Sa_ve", "Save the disc.", 0, NULL},
    {2, "_Stop", "Stop the connection.", 0, NULL},
    {MUIV_SpeedBar_Spacer},
    {3, "_Disc", "Disc page.", 0, NULL},
    {4, "_Matches", "Matches page.", 0, NULL},
    {5, "_Edit", "Edit page.", 0, NULL},
    {MUIV_SpeedBar_End},
};

/***********************************************************************/

int
main(int argc,char **argv)
{
    int res;

    if (MUIMasterBase = OpenLibrary("muimaster.library",19))
    {
        Object                          *app, *win, *sb;
        Object                          *cfg, *update;
        struct MUIS_SpeedBarCfg_Config  c = {MUIV_SpeedBar_ViewMode_TextGfx,MUIV_SpeedBarCfg_Borderless|MUIV_SpeedBarCfg_Raising|MUIV_SpeedBarCfg_Sunny};

        if (app = ApplicationObject,
                MUIA_Application_Title,         "BWin Demo1",
                MUIA_Application_Version,       "$VER: BWin 14.0 (11.4.2002)",
                MUIA_Application_Copyright,     "Copyright 2002 by Alfonso Ranieri",
                MUIA_Application_Author,        "Alfonso Ranieri <alforan@tin.it>",
                MUIA_Application_Description,   "BWin.mcc test",
                MUIA_Application_Base,          "BWINTEST",

                SubWindow, win = BWinObject,
                    MUIA_Window_ID, MAKE_ID('M','A','I','N'),
                    //MUIA_BWin_DragBarTop, TRUE,

                    WindowContents, VGroup,
                        Child, HGroup,
                            GroupFrame,
                            Child, sb = SpeedBarObject,
                                MUIA_Group_Horiz,               TRUE,
                                MUIA_SpeedBar_Borderless,       TRUE,
                                MUIA_SpeedBar_RaisingFrame,     TRUE,
                                MUIA_SpeedBar_Sunny,            TRUE,
                                MUIA_SpeedBar_Buttons,          buttons,
                                MUIA_SpeedBar_StripUnderscore,  TRUE,
                                MUIA_SpeedBar_EnableUnderscore, TRUE,
                                MUIA_SpeedBar_BarSpacer,        TRUE,
                                MUIA_SpeedBar_PicsDrawer,       "PROGDIR:Pics",
                                MUIA_SpeedBar_Strip,            "Main.toolbar",
                                MUIA_SpeedBar_StripButtons,     14,
                            End,
                            Child, HSpace(0),
                        End,
                        Child, VSpace(0),
                        Child, VGroup,
                            GroupFrameT("Appareance"),
                            Child, cfg = SpeedBarCfgObject,
                                MUIA_SpeedBarCfg_Config, &c,
                            End,
                            Child, VSpace(0),
                            Child, update = MUI_MakeObject(MUIO_Button,"_Update"),
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
                    struct MUIS_SpeedBarCfg_Config *c;

                    get(cfg,MUIA_SpeedBarCfg_Config,&c);

                    SetAttrs(sb,MUIA_SpeedBar_ViewMode,c->ViewMode,
                                MUIA_SpeedBar_Borderless,c->Flags & MUIV_SpeedBarCfg_Borderless,
                                MUIA_SpeedBar_RaisingFrame,c->Flags & MUIV_SpeedBarCfg_Raising,
                                MUIA_SpeedBar_SmallImages,c->Flags & MUIV_SpeedBarCfg_SmallButtons,
                                MUIA_SpeedBar_Sunny,c->Flags & MUIV_SpeedBarCfg_Sunny,
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

        CloseLibrary(MUIMasterBase);
    }
    else
    {
        printf("%s: Can't open muimaster.library ver 19 or higher\n",argv[0]);
        res = RETURN_ERROR;
    }

    return res;
}

/***********************************************************************/
