/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#define INTUITION_NO_INLINE_STDARG

#define DEBUG 0
#include <aros/debug.h>

#include <proto/alib.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/icon.h>

#include <intuition/intuition.h>
#include <libraries/mui.h>
#include <libraries/gadtools.h>
#include <workbench/startup.h>

#include <zune/iconimage.h>

#include "display.h"
#include "locale.h"

extern IPTR CalcDisplay_Dispatcher();
struct MUI_CustomClass *CalcDisplay_CLASS;
Object *CalcDisplayObj;
struct Hook BuffToClipHook, ClipToBuffHook;

const UBYTE CalcCopyrStr[] = "Copyright © 2012, The AROS Development Team. All rights reserved.";

enum
{
    ID_ABOUT,
    ID_QUIT,

    ID_BASIC,
    ID_PROGRAMMER,
    ID_SCIENTIFIC,

    ID_DEC,
    ID_HEX,
    ID_OCT,
    ID_BIN,

    ID_COPY,
    ID_PASTE
};

#define ID_FTXT	MAKE_ID('F','T','X','T')
#define ID_CHRS	MAKE_ID('C','H','R','S')

AROS_UFH3
(
    void, BuffToClipFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(void *, dummy, A2),
    AROS_UFHA(void **, dummy2, A1)
)
{
    AROS_USERFUNC_INIT

    struct	IFFHandle	*IFFHandle;
    BOOL	written = FALSE;
    STRPTR        valueStr = 0;

    GET(CalcDisplayObj, MUIA_CalcDisplay_Input, &valueStr);

    if ((valueStr) && (strlen(valueStr) > 0))
    {
        if((IFFHandle = AllocIFF()))
        {
            if((IFFHandle->iff_Stream = (ULONG)OpenClipboard(0)))
            {
                InitIFFasClip(IFFHandle);

                if(!OpenIFF(IFFHandle, IFFF_WRITE))
                {
                    if(!PushChunk(IFFHandle, ID_FTXT, ID_FORM, IFFSIZE_UNKNOWN))
                    {
                        if(!PushChunk(IFFHandle, 0, ID_CHRS, IFFSIZE_UNKNOWN))
                        {
                            if(WriteChunkBytes(IFFHandle, valueStr, strlen(valueStr)) == strlen(valueStr))
                            {
                                if(!PopChunk(IFFHandle))
                                    written = TRUE;
                            }
                        }
                        if(written)
                            PopChunk(IFFHandle);
                    }
                    CloseIFF(IFFHandle);
                }
                CloseClipboard((struct ClipboardHandle *)IFFHandle->iff_Stream);
            }
            FreeIFF(IFFHandle);
        }
    }
    AROS_USERFUNC_EXIT
}

AROS_UFH3
(
    void, ClipToBuffFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(void *, dummy, A2),
    AROS_UFHA(void **, dummy2, A1)
)
{
    AROS_USERFUNC_INIT

    UBYTE chunk_buffer[MUIV_CalcDisplay_MaxInputLen];
    struct IFFHandle *IFFHandle;
    struct ContextNode  *cn;
    ULONG error, read = 0;

    if ((IFFHandle = AllocIFF()))
    {
        if ((IFFHandle->iff_Stream = (ULONG)OpenClipboard(0)))
        {
            InitIFFasClip(IFFHandle);

            if (!OpenIFF(IFFHandle, IFFF_READ))
            {
                if (!StopChunk(IFFHandle, ID_FTXT, ID_CHRS))
                {
                    if (!(error = ParseIFF(IFFHandle, IFFPARSE_SCAN)))
                    {
                        cn = CurrentChunk(IFFHandle);

                        if (cn && (cn->cn_Type == ID_FTXT) && (cn->cn_ID == ID_CHRS))
                        {
                            read = ReadChunkBytes(IFFHandle, chunk_buffer, MUIV_CalcDisplay_MaxInputLen);
                        }
                    }
                }
                CloseIFF(IFFHandle);
            }
            CloseClipboard((struct ClipboardHandle *)IFFHandle->iff_Stream);
        }
        FreeIFF(IFFHandle);
    }

    if (read > 0)
    {   
        char *clipend = (chunk_buffer + read);
        for (;read > 0; read--)
        {
            IPTR inputval = (IPTR)*(char *)(clipend - read);
            SET(CalcDisplayObj, MUIA_CalcDisplay_Input, inputval);
        }
    }
    AROS_USERFUNC_EXIT
}

int main(int argc, char **argv)
{
    Object *CalcAppObj, *CalcWindObj, *CalcAboutWinObj, *CalcWContentsObj;
    Object *ButAObj, *ButTmp1Obj, *ButTmp2Obj, *ButCAObj, *ButCEObj;
    Object *ButBObj, *ButTmp5Obj, *ButTmp6Obj, *ButTmp7Obj, *ButTmp8Obj;
    Object *ButCObj, *But7Obj, *But8Obj, *But9Obj, *ButDIVObj;
    Object *ButDObj, *But4Obj, *But5Obj, *But6Obj, *ButMULTObj;
    Object *ButEObj, *But1Obj, *But2Obj, *But3Obj, *ButMINUSObj;
    Object *ButFObj, *But0Obj, *ButPeriodObj, *ButPLUSObj, *ButEQUALObj;
    Object *CalcMenuObj, *MenuEntry;

    struct NewMenu CalcMenu[] =
    {
        { NM_TITLE, _(MSG_MENU_PROJECT), NULL, 0, 0, NULL },
        { NM_ITEM, "Mode", NULL, 0, 0, NULL },
        { NM_SUB, "Basic", NULL, CHECKIT|CHECKED|MENUTOGGLE, ~(1 << 0), (APTR)ID_BASIC },
        { NM_SUB, "Programmer", NULL, CHECKIT|MENUTOGGLE, ~(1 << 1), (APTR)ID_PROGRAMMER },
        { NM_SUB, "Scientific", NULL, CHECKIT|MENUTOGGLE, ~(1 << 2), (APTR)ID_SCIENTIFIC },

        { NM_ITEM, _(MSG_MENU_BASE), NULL, 0, 0, NULL },
        { NM_SUB, _(MSG_MENU_HEXADECIMAL), NULL, CHECKIT|MENUTOGGLE, ~(1 << 0), (APTR)ID_HEX },
        { NM_SUB, _(MSG_MENU_DECIMAL), NULL, CHECKIT|CHECKED|MENUTOGGLE, ~(1 << 1), (APTR)ID_DEC },
        { NM_SUB, _(MSG_MENU_OCTAL), NULL, CHECKIT|MENUTOGGLE, ~(1 << 2), (APTR)ID_OCT },
        { NM_SUB, _(MSG_MENU_BINARY), NULL, CHECKIT|MENUTOGGLE, ~(1 << 3), (APTR)ID_BIN },

        { NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL },
        { NM_ITEM, _(MSG_MENU_ABOUT), "?", 0, 0, (APTR)ID_ABOUT },
        { NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL },
        { NM_ITEM, _(MSG_MENU_QUIT), "Q", 0, 0, (APTR)ID_QUIT },

        { NM_TITLE, _(MSG_MENU_EDIT), NULL, 0, 0, NULL },
        { NM_ITEM, _(MSG_MENU_COPY), "C", 0, 0, (APTR)ID_COPY },
        { NM_ITEM, _(MSG_MENU_PASTE), "V", 0, 0, (APTR)ID_PASTE },

        { NM_END }
    };

    static struct DiskObject *disko; 
    static struct WBStartup *argmsg;
    static struct WBArg *wb_arg;
    static STRPTR cxname;

    if (argc)
    {
        cxname = argv[0];
    } else {
        argmsg = (struct WBStartup *)argv;
        wb_arg = argmsg->sm_ArgList;
        cxname = wb_arg->wa_Name;
    }
    disko = GetDiskObject(cxname);

    CalcDisplay_CLASS = MUI_CreateCustomClass(NULL, MUIC_Area, NULL, sizeof(struct CalcDisplay_DATA), CalcDisplay_Dispatcher);

    CalcMenuObj = MUI_MakeObject(MUIO_MenustripNM, CalcMenu, (IPTR) NULL);

    CalcAppObj = ApplicationObject,
        MUIA_Application_Title,       _(MSG_APP_TITLE),
        MUIA_Application_Version,     (IPTR) "$VER: Calculator 1.4 (08.05.2012)",
        MUIA_Application_Copyright,   (IPTR)CalcCopyrStr,
        MUIA_Application_Author,      (IPTR) "Nick 'Kalamatee' Andrews",
        MUIA_Application_Description, _(MSG_APP_DESCRIPTION),
        MUIA_Application_Base,        (IPTR) "CALCULATOR",
        MUIA_Application_DiskObject, (IPTR)disko,

        SubWindow, (IPTR) (CalcWindObj = WindowObject,
            MUIA_Window_Title, _(MSG_WIN_TITLE),
            MUIA_Window_ID, MAKE_ID('C','A','L','C'),
            MUIA_Window_Menustrip, (IPTR) CalcMenuObj,
            MUIA_Window_SizeGadget, TRUE,
            WindowContents, (IPTR) (CalcWContentsObj = VGroup,

                Child, (IPTR)(CalcDisplayObj = NewObject(CalcDisplay_CLASS->mcc_Class, NULL,
                TAG_DONE)),
                Child, (IPTR)VGroup,
                    Child, (IPTR) ColGroup(5),
                        Child, ButAObj = SimpleButton("_A"),
                        Child, ButTmp1Obj = SimpleButton(" "),
                        Child, ButTmp2Obj = SimpleButton(" "),
                        Child, ButCAObj = SimpleButton("CA"),
                        Child, ButCEObj = SimpleButton("CE"),
                        Child, ButBObj = SimpleButton("_B"),
                        Child, ButTmp5Obj = SimpleButton(" "),
                        Child, ButTmp6Obj = SimpleButton(" "),
                        Child, ButTmp7Obj = SimpleButton(" "),
                        Child, ButTmp8Obj = SimpleButton(" "),
                        Child, ButCObj = SimpleButton("_C"),
                        Child, But7Obj = SimpleButton("_7"),
                        Child, But8Obj = SimpleButton("_8"),
                        Child, But9Obj = SimpleButton("_9"),
                        Child, ButDIVObj = SimpleButton("_/"),
                        Child, ButDObj = SimpleButton("_D"),
                        Child, But4Obj = SimpleButton("_4"),
                        Child, But5Obj = SimpleButton("_5"),
                        Child, But6Obj = SimpleButton("_6"),
                        Child, ButMULTObj = SimpleButton("_*"),
                        Child, ButEObj = SimpleButton("_E"),
                        Child, But1Obj = SimpleButton("_1"),
                        Child, But2Obj = SimpleButton("_2"),
                        Child, But3Obj = SimpleButton("_3"),
                        Child, ButMINUSObj = SimpleButton("_-"),
                        Child, ButFObj = SimpleButton("_F"),
                        Child, But0Obj = SimpleButton("_0"),
                        Child, ButPeriodObj = SimpleButton("_."),
                        Child, ButPLUSObj = SimpleButton("_+"),
                        Child, ButEQUALObj = SimpleButton("_="),
                    End,
                End,
            End),
        End),
        SubWindow, (IPTR) (CalcAboutWinObj = WindowObject,
            MUIA_Window_Title, _(MSG_MENU_ABOUT),
            MUIA_Window_SizeGadget, TRUE,
            WindowContents, (IPTR) (VGroup,
                Child, HGroup,
                    Child, IconImageObject,
                        MUIA_InputMode,             MUIV_InputMode_Toggle,
                        MUIA_IconImage_File,        (IPTR)"PROGDIR:Calculator",
                    End,
                    Child, VGroup,
                        Child, TextObject,
                            MUIA_Text_PreParse, "\33c\33b", 
                            MUIA_Text_Contents, (IPTR)_(MSG_APP_TITLE),
                        End,
                        Child, TextObject,
                            MUIA_Text_PreParse, "\33c", 
                            MUIA_Text_Contents, (IPTR)CalcCopyrStr,
                        End,
                    End,
                End,
            End),
        End),
    End;

    if (!CalcAppObj)
    {
        D(bug("[CALC] Failed to create Calculator GUI\n"));
        return 5;
    }

    SET(ButAObj, MUIA_Disabled, TRUE);
    SET(ButBObj, MUIA_Disabled, TRUE);
    SET(ButCObj, MUIA_Disabled, TRUE);
    SET(ButDObj, MUIA_Disabled, TRUE);
    SET(ButEObj, MUIA_Disabled, TRUE);
    SET(ButFObj, MUIA_Disabled, TRUE);
    SET(ButTmp1Obj, MUIA_Disabled, TRUE);
    SET(ButTmp2Obj, MUIA_Disabled, TRUE);
    SET(ButCAObj, MUIA_Disabled, TRUE);
    SET(ButCEObj, MUIA_Disabled, TRUE);
    SET(ButTmp5Obj, MUIA_Disabled, TRUE);
    SET(ButTmp6Obj, MUIA_Disabled, TRUE);
    SET(ButTmp7Obj, MUIA_Disabled, TRUE);
    SET(ButTmp8Obj, MUIA_Disabled, TRUE);

    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_QUIT)) != NULL)
    {
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            CalcAppObj, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    }
    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_ABOUT)) != NULL)
    {
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            CalcAboutWinObj, 3, MUIM_Set, MUIA_Window_Open, TRUE);
    }

    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_COPY)) != NULL)
    {
        BuffToClipHook.h_Entry = (HOOKFUNC) BuffToClipFunc;
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            CalcWindObj, 3, MUIM_CallHook, (IPTR)&BuffToClipHook, MenuEntry);
    }
    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_PASTE)) != NULL)
    {
        ClipToBuffHook.h_Entry = (HOOKFUNC) ClipToBuffFunc;
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            CalcWindObj, 3, MUIM_CallHook, (IPTR)&ClipToBuffHook, MenuEntry);
    }

    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_BASIC)) != NULL)
    {
        Object *TargetMenuEntry;
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
            ButPeriodObj, 3, MUIM_Set, MUIA_Disabled, FALSE);
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_DEC)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Menuitem_Checked, TRUE);
        }
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_HEX)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, TRUE);
        }
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_OCT)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, TRUE);
        }
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_BIN)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, TRUE);
        }
    }
    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_PROGRAMMER)) != NULL)
    {
        Object *TargetMenuEntry;
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
            ButPeriodObj, 3, MUIM_Set, MUIA_Disabled, TRUE);
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_HEX)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, FALSE);
        }
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_OCT)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, FALSE);
        }
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_BIN)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, FALSE);
        }
    }
    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_SCIENTIFIC)) != NULL)
    {
        Object *TargetMenuEntry;
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
            ButPeriodObj, 3, MUIM_Set, MUIA_Disabled, FALSE);
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_DEC)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Menuitem_Checked, TRUE);
        }
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_HEX)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, TRUE);
        }
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_OCT)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, TRUE);
        }
        if ((TargetMenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_BIN)) != NULL)
        {
            DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, TRUE,
                TargetMenuEntry, 3, MUIM_Set, MUIA_Disabled, TRUE);
        }
    }

    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_HEX)) != NULL)
    {
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            CalcDisplayObj, 3, MUIM_Set, MUIA_CalcDisplay_Base, 16);

        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
            ButAObj, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
            ButBObj, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
            ButCObj, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
            ButDObj, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
            ButEObj, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime,
            ButFObj, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
    }
    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_DEC)) != NULL)
    {
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            CalcDisplayObj, 3, MUIM_Set, MUIA_CalcDisplay_Base, 10);
    }
    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_OCT)) != NULL)
    {
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            CalcDisplayObj, 3, MUIM_Set, MUIA_CalcDisplay_Base, 8);
    }
    if ((MenuEntry = (Object *)DoMethod(CalcMenuObj, MUIM_FindUData, ID_BIN)) != NULL)
    {
        DoMethod(MenuEntry, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            CalcDisplayObj, 3, MUIM_Set, MUIA_CalcDisplay_Base, 2);
    }

    DoMethod(CalcWindObj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, CalcAppObj, 2,
        MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(ButCAObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)-1); // CA
    DoMethod(ButCEObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)-2); // CE
    DoMethod(ButCEObj, MUIM_Notify, MUIA_Pressed, TRUE, ButCEObj, 3,
        MUIM_Set, MUIA_Disabled, TRUE);

    DoMethod(CalcDisplayObj, MUIM_Notify, MUIA_CalcDisplay_Calculated, MUIV_EveryTime, ButCAObj, 3,
        MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);
    DoMethod(CalcDisplayObj, MUIM_Notify, MUIA_CalcDisplay_Calculated, MUIV_EveryTime, ButCEObj, 3,
        MUIM_Set, MUIA_Disabled, MUIV_TriggerValue);

    DoMethod(ButAObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'A');
    DoMethod(ButBObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'B');
    DoMethod(ButCObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'C');
    DoMethod(ButDObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'D');
    DoMethod(ButEObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'E');
    DoMethod(ButFObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'F');

    DoMethod(But1Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'1');
    DoMethod(But2Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'2');
    DoMethod(But3Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'3');
    DoMethod(But4Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'4');
    DoMethod(But5Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'5');
    DoMethod(But6Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'6');
    DoMethod(But7Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'7');
    DoMethod(But8Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'8');
    DoMethod(But9Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'9');
    DoMethod(But0Obj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'0');

    DoMethod(ButPeriodObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'.');

    DoMethod(ButDIVObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'/');
    DoMethod(ButMULTObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'*');
    DoMethod(ButMINUSObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'-');
    DoMethod(ButPLUSObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'+');
    DoMethod(ButEQUALObj, MUIM_Notify, MUIA_Pressed, TRUE, CalcDisplayObj, 3,
        MUIM_Set, MUIA_CalcDisplay_Input, (IPTR)'=');

    DoMethod(CalcAboutWinObj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        CalcAboutWinObj, 3, MUIM_Set, MUIA_Window_Open, TRUE);

    SET(CalcWindObj, MUIA_Window_Open, TRUE);
    {
        ULONG sigs = 0;

        while (DoMethod(CalcAppObj, MUIM_Application_NewInput,
                &sigs) != MUIV_Application_ReturnID_Quit)
        {
            if (sigs)
            {
                sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                if (sigs & SIGBREAKF_CTRL_C)
                    break;
            }
        }
    }

    SET(CalcWindObj, MUIA_Window_Open, FALSE);

    MUI_DisposeObject(CalcAppObj);

    MUI_DeleteCustomClass(CalcDisplay_CLASS);

    if (disko)
        FreeDiskObject(disko);
//main_error:
    return 0;
}
