/* Urltext.mcc example (SAS/C) */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <mui/Urltext_mcc.h>

#ifndef __AROS__
typedef ULONG IPTR;
#endif

/***********************************************************************/

long __stack = 8192;

/***********************************************************************/

#ifndef __AROS__
ULONG DoMethod(APTR,unsigned long MethodID,...);
#endif

/***********************************************************************/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG)(a)<<24|(ULONG)(b)<<16|(ULONG)(c)<<8|(ULONG)(d))
#endif

/***********************************************************************/

Object *
urlTextObject(struct Library *MUIMasterBase,STRPTR url,STRPTR text,ULONG font)
{
    return UrltextObject,
        MUIA_Font,          font,
        MUIA_Urltext_Text,  text,
        MUIA_Urltext_Url,   url,
    End;
}

/***********************************************************************/

int main(void)
{
    struct Library *MUIMasterBase;

    if ((MUIMasterBase = OpenLibrary("muimaster.library",19)))
    {
        APTR app, mwin, u0, u1, u2, u3, t0;

        if ((app = ApplicationObject,
            MUIA_Application_Title,         "Urltext",
            MUIA_Application_Version,       "$VER: Urltext 1.2 (16.12.2001)",
            MUIA_Application_Copyright,     "Copyright 2000-2001 Alfonso Ranieri",
            MUIA_Application_Author,        "Alfonso Ranieri",
            MUIA_Application_Description,   "Urltext example",
            MUIA_Application_Base,          "URLTEXT",
            SubWindow, mwin = WindowObject,
                MUIA_Window_Title,  "Urltext example",
                MUIA_Window_ID,     MAKE_ID('M','W','I','N'),
                WindowContents, VGroup,
                    Child, VGroup,
                        GroupFrame,
                        MUIA_Background, MUII_GroupBack,
                        Child, VSpace(0),
                        Child, HGroup,
                            Child, HSpace(0),
                            Child, ColGroup(3),
                                Child, u0 = urlTextObject(MUIMasterBase,"http://web.tiscalinet.it/amiga/","Alfie's home page",MUIV_Font_Big),
                                Child, HSpace(0),
                                Child, u1 = urlTextObject(MUIMasterBase,"http://web.tiscalinet.it/amiga/rxmui","RxMUI home page",MUIV_Font_Normal),
                                Child, u2 = urlTextObject(MUIMasterBase,"http://www.egroups.co/group/rxmui","RxMUI mail list",MUIV_Font_Normal),
                                Child, HSpace(0),
                                Child, u3 = urlTextObject(MUIMasterBase,"mailto:alforan@tin.it","Alfonso Ranieri",MUIV_Font_Normal),
                            End,
                            Child, HSpace(0),
                        End,
                        Child, VSpace(0),
                    End,
                    Child, t0 = TextObject,
                        MUIA_Frame,         MUIV_Frame_Text,
                        MUIA_Background,    MUII_TextBack,
                        MUIA_Text_PreParse, (IPTR)"\33c",
                    End,
                End,
            End,
        End))
        {
            ULONG sigs;

            DoMethod(mwin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

            DoMethod(u0,MUIM_Notify,MUIA_Urltext_Url,MUIV_EveryTime,t0,3,MUIM_Set,MUIA_Text_Contents,MUIV_TriggerValue);
            DoMethod(u1,MUIM_Notify,MUIA_Urltext_Url,MUIV_EveryTime,t0,3,MUIM_Set,MUIA_Text_Contents,MUIV_TriggerValue);
            DoMethod(u2,MUIM_Notify,MUIA_Urltext_Url,MUIV_EveryTime,t0,3,MUIM_Set,MUIA_Text_Contents,MUIV_TriggerValue);
            DoMethod(u3,MUIM_Notify,MUIA_Urltext_Url,MUIV_EveryTime,t0,3,MUIM_Set,MUIA_Text_Contents,MUIV_TriggerValue);

            set(mwin,MUIA_Window_Open,TRUE);

            for (sigs = 0; DoMethod(app,MUIM_Application_NewInput,&sigs)!=MUIV_Application_ReturnID_Quit; )
                if (sigs && ((sigs = Wait(sigs|SIGBREAKF_CTRL_C)) & SIGBREAKF_CTRL_C))
                     break;

            MUI_DisposeObject(app);
        }
        else Printf("Can't create Application\n");

        CloseLibrary(MUIMasterBase);
    }
    else Printf("Can't open muimaster.library version 19 or higher\n");

    return 0;
}

/***********************************************************************/
