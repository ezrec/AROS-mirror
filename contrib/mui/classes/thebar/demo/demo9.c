
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <mui/TheBar_mcc.h>
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

#define ROWS     10
#define COLS      7
#define HSPACE    1
#define VSPACE    1
#define PIC(x,y) (x+y*COLS)

struct MUIS_TheBar_Button buttons[] =
{
    {PIC(2,2),  0, "Rea_d"},
    {PIC(1,7),  1, "_Edit"},
    {PIC(0,6),  2, "De_lete"},
    {PIC(1,6),  3, "Ge_TAdd"},
    {PIC(1,9),  4, "_New"},
    {PIC(1,5),  5, "_Reply"},
    {PIC(2,3),  6, "For_ward"},
    {PIC(2,5),  7, "Fetc_h"},
    {PIC(2,7),  8, "_Send"},
    {PIC(3,5),  9, "S_earch"},
    {PIC(0,9), 10, "F_ilter"},
    {PIC(1,0), 11, "_Filters"},
    {PIC(0,1), 12, "_Abook"},
    {PIC(0,3), 13, "_Config"},
    {MUIV_TheBar_End},
};

/***********************************************************************/

char *appearances[] = {"Images and text","Images","Text",NULL};
char *labelPoss[] = {"Bottom","Top","Right","Left",NULL};

int
main(int argc,char **argv)
{
    int res;

    if (MUIMasterBase = OpenLibrary("muimaster.library",19))
    {
        Object *app, *win, *sb, *appearance, *labelPos, *borderless, *sunny, *raised, *scaled, *update;

        if (app = ApplicationObject,
                MUIA_Application_Title,         "TheBar Demo9",
                MUIA_Application_Version,       "$VER: TheBarDemo9 1.0 (2.7.2003)",
                MUIA_Application_Copyright,     "Copyright 2003 by Alfonso Ranieri",
                MUIA_Application_Author,        "Alfonso Ranieri <alforan@tin.it>",
                MUIA_Application_Description,  "TheBar example",
                MUIA_Application_Base,         "THEBAREXAMPLE",

                SubWindow, win = WindowObject,
                    MUIA_Window_ID,             MAKE_ID('M','A','I','N'),
                    MUIA_Window_Title,          "TheBar Demo9",
                    WindowContents, VGroup,
                        Child, sb = TheBarVirtObject,
                            MUIA_Group_Horiz,            TRUE,
                            MUIA_TheBar_EnableKeys,      TRUE,
                            MUIA_TheBar_Buttons,         buttons,
                            MUIA_TheBar_Strip,           "PROGDIR:Pics/SimpleMail",
                            MUIA_TheBar_StripRows,       ROWS,
                            MUIA_TheBar_StripCols,       COLS,
                            MUIA_TheBar_StripHorizSpace, HSPACE,
                            MUIA_TheBar_StripVertSpace,  VSPACE,
                        End,
                        Child, VGroup,
                            GroupFrameT("Settings"),
                            Child, HGroup,
                                Child, Label2("Appearance"),
                                Child, appearance = MUI_MakeObject(MUIO_Cycle,NULL,appearances),
                                Child, Label2("Label pos"),
                                Child, labelPos = MUI_MakeObject(MUIO_Cycle,NULL,labelPoss),
                            End,
                            Child, HGroup,
                                Child, HSpace(0),
                                Child, Label1("Borderless"),
                                Child, borderless = MUI_MakeObject(MUIO_Checkmark,NULL),
                                Child, HSpace(0),
                                Child, Label1("Sunny"),
                                Child, sunny = MUI_MakeObject(MUIO_Checkmark,NULL),
                                Child, HSpace(0),
                                Child, Label1("Raised"),
                                Child, raised = MUI_MakeObject(MUIO_Checkmark,NULL),
                                Child, HSpace(0),
                                Child, Label1("Scaled"),
                                Child, scaled = MUI_MakeObject(MUIO_Checkmark,NULL),
                                Child, HSpace(0),
                            End,
                        End,
                        Child, update = MUI_MakeObject(MUIO_Button,"_Update"),
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
                    ULONG appearanceV, labelPosV, borderlessV, sunnyV, raisedV, scaledV;

                    get(appearance,MUIA_Cycle_Active,&appearanceV);
                    get(labelPos,MUIA_Cycle_Active,&labelPosV);
                    get(borderless,MUIA_Selected,&borderlessV);
                    get(sunny,MUIA_Selected,&sunnyV);
                    get(raised,MUIA_Selected,&raisedV);
                    get(scaled,MUIA_Selected,&scaledV);

                    SetAttrs(sb,MUIA_TheBar_ViewMode,   appearanceV,
                                MUIA_TheBar_LabelPos,   labelPosV,
                                MUIA_TheBar_Borderless, borderlessV,
                                MUIA_TheBar_Sunny,      sunnyV,
                                MUIA_TheBar_Raised,     raisedV,
                                MUIA_TheBar_Scaled,     scaledV,
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
