/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#include "class.h"
#include "private.h"
#include "debug.h"
#include <stdio.h>

/******************************************************************************/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

/******************************************************************************/

#if defined(__amigaos4__)
struct Library *GfxBase = NULL;
struct Library *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *DataTypesBase = NULL;
struct Library *PictureDTBase = NULL;

struct GraphicsIFace *IGraphics = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct CyberGfxIFace *ICyberGfx = NULL;
struct DataTypesIFace *IDataTypes = NULL;
#elif defined(__MORPHOS__)
struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *DataTypesBase = NULL;
struct Library *PictureDTBase = NULL;
#else
struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
#if defined(__AROS__)
struct UtilityBase *UtilityBase = NULL;
#else
struct Library *UtilityBase = NULL;
#endif
struct Library *CyberGfxBase = NULL;
struct Library *DataTypesBase = NULL;
struct Library *PictureDTBase = NULL;
#endif

DISPATCHERPROTO(_Dispatcher);
struct MUI_CustomClass *lib_thisClass = NULL;
struct MUI_CustomClass *lib_spacerClass = NULL;
struct MUI_CustomClass *lib_dragBarClass = NULL;
ULONG lib_flags = 0;

/******************************************************************************/

void closeAll(void)
{
    D(DBF_STARTUP,"cleaning up thisClass...");
    if (lib_thisClass)    MUI_DeleteCustomClass(lib_thisClass);
    D(DBF_STARTUP,"cleaning up dragBarClass...");
    if (lib_dragBarClass) MUI_DeleteCustomClass(lib_dragBarClass);
    D(DBF_STARTUP,"cleaning up spacerClass...");
    if (lib_spacerClass)  MUI_DeleteCustomClass(lib_spacerClass);

    D(DBF_STARTUP,"cleaning up MUIMasterBase...");
    if (MUIMasterBase)
    {
        DROPINTERFACE(IMUIMaster);
        CloseLibrary(MUIMasterBase);
    }

    D(DBF_STARTUP,"cleaning up VyberGfxBase...");
    if (CyberGfxBase)
    {
        DROPINTERFACE(ICyberGfx);
        CloseLibrary(CyberGfxBase);
    }

    D(DBF_STARTUP,"cleaning up DataTypesBase...");
    if (DataTypesBase)
    {
        DROPINTERFACE(IDataTypes);
        CloseLibrary(DataTypesBase);
    }

    D(DBF_STARTUP,"cleaning up UtilityBase...");
    if (UtilityBase)
    {
        DROPINTERFACE(IUtility);
        CloseLibrary((struct Library *)UtilityBase);
    }

    D(DBF_STARTUP,"cleaning up IntuitionBase...");
    if (IntuitionBase)
    {
        DROPINTERFACE(IIntuition);
        CloseLibrary((struct Library *)IntuitionBase);
    }

    D(DBF_STARTUP,"cleaning up GfxBase...");
    if (GfxBase)
    {
        DROPINTERFACE(IGraphics);
        CloseLibrary((struct Library *)GfxBase);
    }
}

ULONG initAll(void)
{
    if ((GfxBase = (APTR)OpenLibrary("graphics.library", 38)) &&
        GETINTERFACE(IGraphics, GfxBase) &&
        (IntuitionBase = (APTR)OpenLibrary("intuition.library", 38)) &&
        GETINTERFACE(IIntuition, IntuitionBase) &&
		#ifndef __amigaos4__
        (UtilityBase = (APTR)OpenLibrary("utility.library", 38)) &&
        GETINTERFACE(IUtility, UtilityBase) &&
		#endif        
		(DataTypesBase = OpenLibrary("datatypes.library", 37)) &&
        GETINTERFACE(IDataTypes, DataTypesBase) &&
        (MUIMasterBase = OpenLibrary("muimaster.library", 19/*MUIMASTER_VMIN*/)) &&
        GETINTERFACE(IMUIMaster, MUIMasterBase) &&
        initSpacerClass() &&
        initDragBarClass() &&
        (lib_thisClass = MUI_CreateCustomClass(NULL, (STRPTR)MUIC_Group, NULL, sizeof(struct InstData), ENTRY(_Dispatcher))))
    {
        CyberGfxBase = OpenLibrary("cybergraphics.library",41);
        #ifdef __amigaos4__
        if (!GETINTERFACE(ICyberGfx,CyberGfxBase))
        {
            CloseLibrary(CyberGfxBase);
            CyberGfxBase = NULL;
        }
        #endif

        PictureDTBase = OpenLibrary("picture.datatype",0);
        #if !defined(__amigaos4__) && !defined(__AROS__)
        if (PictureDTBase)
        {
            if (FindResident("MorphOS"))
            {
                if ((PictureDTBase->lib_Version<50) ||
                    (PictureDTBase->lib_Version==50 && PictureDTBase->lib_Revision<17))
                {
                    setFlag(lib_flags,BASEFLG_BROKENMOSPDT);
                }
            }
        }
        #endif

        if (MUIMasterBase->lib_Version>=20)
        {
            setFlag(lib_flags, BASEFLG_MUI20);

            if (MUIMasterBase->lib_Version>20 || MUIMasterBase->lib_Revision>=5341)
                setFlag(lib_flags, BASEFLG_MUI4);
        }

        #if defined(DEBUG)
        SetupDebug();
        #endif

        return TRUE;
  }

  return FALSE;
}
/******************************************************************************/

static const char *appearances[] = { "Images and text", "Images", "Text", NULL};
static const char *labelPoss[] = { "Bottom", "Top", "Right", "Left", NULL};

static struct MUIS_TheBar_Button buttons[] =
{
    { 0, 0, "_Pred", "Pread mail.",    0, 0, NULL, NULL },
    { 1, 1, "_Next", "Next mail.",     0, 0, NULL, NULL },
    { 2, 2, "_Move", "Move somewhere.",0, 0, NULL, NULL },
    { MUIV_TheBar_BarSpacer, 3, NULL, NULL, 0, 0, NULL, NULL },
    { 3, 4, "_Forw", "Forward somewhere.", 0, 0, NULL, NULL },
    { 4, 5, "F_ind", "Find something.", 0, 0, NULL, NULL },
    { 5, 6, "_Save", "Save mail.", 0, 0, NULL, NULL },
    { MUIV_TheBar_End , 0, NULL, NULL, 0, 0, NULL, NULL },
};

// static hooks
HOOKPROTONHNO(SaveFunc, ULONG, UNUSED ULONG *qual)
{
    D(DBF_STARTUP, "SaveHook triggered: %08lx\n", *qual);

    return 0;
}
MakeStaticHook(SaveHook, SaveFunc);

HOOKPROTONHNO(SleepFunc, ULONG, Object **sb)
{
    IPTR sleeping;

    DoMethod(*sb, MUIM_TheBar_GetAttr, 6, MUIA_TheBar_Attr_Sleep, (IPTR)&sleeping);

    D(DBF_STARTUP, "SleepHook triggered: %08lx %ld\n", (ULONG)*sb, sleeping);

    DoMethod(*sb, MUIM_TheBar_SetAttr, 6, MUIA_TheBar_Attr_Sleep, !sleeping);

  return 0;
}
MakeStaticHook(SleepHook, SleepFunc);

int main(void)
{
    if (initAll())
    {
        Object *app, *win, *sb, *appearance, *labelPos, *borderless, *sunny, *raised, *scaled, *update;

        ENTER();
        app = ApplicationObject,
            MUIA_Application_Title,        "TheBar Demo1",
            MUIA_Application_Version,      "$VER: TheBarDemo1 1.0 (24.6.2003)",
            MUIA_Application_Copyright,    "Copyright 2003 by Alfonso Ranieri",
            MUIA_Application_Author,       "Alfonso Ranieri <alforan@tin.it>",
            MUIA_Application_Description,  "TheBar example",
            MUIA_Application_Base,         "THEBAREXAMPLE",

            SubWindow, win = WindowObject,
                MUIA_Window_ID,    MAKE_ID('M','A','I','N'),
                MUIA_Window_Title, "TheBar Demo1",

                WindowContents, VGroup,
                    Child, sb = NewObject(lib_thisClass->mcc_Class, NULL,
                        MUIA_Group_Horiz,       TRUE,
                        MUIA_TheBar_EnableKeys, TRUE,
                        MUIA_TheBar_Buttons,    buttons,
                        MUIA_TheBar_PicsDrawer, "AmiKit:t/thebar/demo/pics",
                        MUIA_TheBar_Strip,      "Read.toolbar",
                        MUIA_TheBar_SelStrip,   "Read_S.toolbar",
                        MUIA_TheBar_DisStrip,   "Read_G.toolbar",
                        MUIA_TheBar_StripCols,  11,
                    End,
                    Child, VGroup,
                        GroupFrameT("Settings"),
                        Child, HGroup,
                            Child, Label2("Appearance"),
                            Child, appearance = MUI_MakeObject(MUIO_Cycle,NULL,(ULONG)appearances),
                            Child, Label2("Label pos"),
                            Child, labelPos = MUI_MakeObject(MUIO_Cycle,NULL,(ULONG)labelPoss),
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
                    Child, update = MUI_MakeObject(MUIO_Button,(ULONG)"_Update"),
                End,
            End,
        End;
        if (app)
        {
            ULONG sigs = 0, id;

            DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
            DoMethod(update,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)app,2,MUIM_Application_ReturnID,TAG_USER);

            DoMethod(sb, MUIM_TheBar_Notify, 5, MUIA_Pressed, FALSE, (ULONG)sb, 3, MUIM_CallHook, (ULONG)&SleepHook, (ULONG)sb);
            DoMethod(sb, MUIM_TheBar_Notify, 6, MUIA_Pressed, FALSE, (ULONG)sb, 3, MUIM_CallHook, (ULONG)&SaveHook, MUIV_TheBar_Qualifier);

            set(win,MUIA_Window_Open,TRUE);

            while((LONG)(id = DoMethod(app,MUIM_Application_NewInput,(ULONG)&sigs)) != (LONG)MUIV_Application_ReturnID_Quit)
            {
                if (id==TAG_USER)
                {
                    ULONG appearanceV, labelPosV, borderlessV, sunnyV, raisedV, scaledV;

                    appearanceV = xget(appearance, MUIA_Cycle_Active);
                    labelPosV = xget(labelPos, MUIA_Cycle_Active);
                    borderlessV = xget(borderless, MUIA_Selected);
                    sunnyV = xget(sunny, MUIA_Selected);
                    raisedV = xget(raised, MUIA_Selected);
                    scaledV = xget(scaled, MUIA_Selected);

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

            D(DBF_STARTUP, "cleaning up application object...");
            MUI_DisposeObject(app);
        }
        else printf("Failed to create application\n");
    }
    else printf("Failed to create something\n");

    RETURN(0);
    return 0;
}
