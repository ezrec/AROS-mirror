/***************************************************************************

 Mailtext - An MUI Custom Class for eMessage display
 Copyright (C) 1996-2001 by Olaf Peters
 Copyright (C) 2002-2006 by Mailtext Open Source Team

 GCC & OS4 Portage by Alexandre Balaban

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Mailtext OpenSource project :  http://sourceforge.net/projects/mailtext/

 $Id$

 Example for Mailtext.mcc

 show_mailtext.c

***************************************************************************/


#define TESTTEXT "Testtext for Mailtext CustomClass\n\n*Bold* /Italics/ _Underline_ #Coloured# _/*Combination*/_\n\n\nQuoted Text:\n\n1>2>3>4>5> *Bold* /Italics/ _Underline_ #Coloured#\n\nURLs: http://sourceforge.net/projects/mailtext/\n\nTo clear things up a bit: (let + be *, /, _ or #)\n\n   +Text+\n   ^    ^\n   |    |\n   |    +- the terminating char\n   |\n   +- the introducing char\n\n-- \nolf@magicolf.de"

#define ID_ActionURL   1
#define ID_ActionEMail 2

#include <mui/demo.h>
#include <exec/memory.h>

#include <mui/Mailtext_mcc.h>
#include <mui/NListview_mcc.h>

#include <dos/rdargs.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

static APTR app;
static APTR window;
static APTR mt ;
static APTR lv ;

int main(int argc, char *argv[])
{
    ULONG signals;
    BOOL running = TRUE, testTextUsed = TRUE ;
    struct RDArgs *rdArgs ;
    struct FileInfoBlock *fib ;
    BPTR fh ;
    char *text ;
    LONG args[2] = {0L, 0L};
    STRPTR str ;

    init() ;

    if (rdArgs = ReadArgs("FILE", args, 0))
    {
        if (args[0])
        {
            if (fh = Open((STRPTR)args[0], MODE_OLDFILE))
            {
                if (fib = AllocDosObjectTags(DOS_FIB, TAG_DONE))
                {
                    if (ExamineFH(fh, fib))
                    {
                        if (text = AllocVec(fib->fib_Size+1, MEMF_PUBLIC | MEMF_CLEAR))
                        {
                            if (Read(fh, text, fib->fib_Size) == fib->fib_Size)
                            {
                                testTextUsed = FALSE ;
                                text[fib->fib_Size] = '\0' ;
                            }
                            else
                            {
                                (void)PrintFault(IoErr(), "Show_Mailtext") ;
                                FreeVec(text) ;
                            }
                        }
                    }
                    FreeDosObject(DOS_FIB, fib) ;
                }
                (void)Close(fh) ;
            }
            else
            {
                (void)PrintFault(IoErr(), "Show_Mailtext") ;
            }
        }
        FreeArgs(rdArgs) ;
    }
    else
    {
        (void)PrintFault(IoErr(), "Show_Mailtext") ;
    }

    if (testTextUsed)
    {
        text = TESTTEXT ;
    }

    app = ApplicationObject,
              MUIA_Application_Title,       "Show_MailtextClass",
              MUIA_Application_Version,     "v1.0",
              MUIA_Application_Copyright,   "©1996 by Olaf Peters",
              MUIA_Application_Author,      "Olaf Peters",
              MUIA_Application_Description, "Demonstrates the mailtext class.",
              MUIA_Application_Base,        "SHOWMAILTEXT",

              SubWindow, window = WindowObject,
                                      MUIA_Window_Title, "MailtextClass",
                                      MUIA_Window_ID,    MAKE_ID('M','A','I','L'),

                                      WindowContents, VGroup,

                                                          Child, lv = NListviewObject,
                                                                      MUIA_NListview_NList, mt = MailtextObject,
                                                                                               MUIA_Mailtext_Text,              text,
                                                                                               MUIA_Mailtext_ForbidContextMenu, FALSE,
                                                                                               MUIA_Font,                       MUIV_Font_Fixed,
                                                                                               MUIA_Frame,                      MUIV_Frame_InputList,
                                                                                               MUIA_NList_Input,                FALSE,
                                                                                               MUIA_NList_MultiSelect,          FALSE,
                                                                                           End,
                                                                     MUIA_CycleChain, TRUE,
                                                                 End,
                                                      End,
                                  End,
          End;

    if (!app)
    {
        if (!testTextUsed)
        {
            FreeVec(text) ;
        }

        fail(app,"Failed to create Application.") ;
    }

    DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app,
                               2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit) ;

    DoMethod(mt, MUIM_Notify, MUIA_Mailtext_ActionURL, MUIV_EveryTime, app,
                               2, MUIM_Application_ReturnID, ID_ActionURL) ;

    DoMethod(mt, MUIM_Notify, MUIA_Mailtext_ActionEMail, MUIV_EveryTime, app,
                               2, MUIM_Application_ReturnID, ID_ActionEMail) ;


    SetAttrs(window, MUIA_Window_Open, TRUE,
                     MUIA_Window_DefaultObject, lv,
             TAG_DONE) ;

    while (running)
    {
        switch (DoMethod(app,MUIM_Application_Input,&signals))
        {
             case MUIV_Application_ReturnID_Quit:
                 running = FALSE;
             break;

             case ID_ActionURL :
                 get(mt, MUIA_Mailtext_ActionURL, &str) ;
                 printf("URL: %s\n", str) ;
             break ;

             case ID_ActionEMail :
                 get(mt, MUIA_Mailtext_ActionEMail, &str) ;
                 printf("EMail: <%s>\n", str) ;

        }

        if (running && signals)
        {
            Wait(signals) ;
        }
    }

    set(window, MUIA_Window_Open, FALSE);

    MUI_DisposeObject(app);

    if (!testTextUsed)
    {
        FreeVec(text) ;
    }

    fail(NULL,NULL);
}


