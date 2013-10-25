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
#include "SetPatch.h"
#include "version.h"

/******************************************************************************/
/* include the minimal startup code to be able to start the class from a      */
/* shell without crashing the system                                          */
/******************************************************************************/
#include "shellstart.c"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#if !defined(VIRTUAL)
#define SUPERCLASS    MUIC_Group
#define CLASS         MUIC_TheBar
#else
#define SUPERCLASS    MUIC_Virtgroup
#define CLASS         MUIC_TheBarVirt
#endif

#define INSTDATA      InstData

#define USERLIBID     CLASS " " LIB_REV_STRING " [" SYSTEMSHORT "/" CPU "] (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION 19

#define USEDCLASSES used_mccs
static const char *used_mccs[] = { "TheBarVirt.mcc", "TheButton.mcc", NULL };

#define USEDCLASSESP used_mcps
static const char *used_mcps[] = { "TheBar.mcp", NULL };

#define MIN_STACKSIZE 8192

#define CLASSINIT
static BOOL ClassInit(UNUSED struct Library *base);

#define CLASSEXPUNGE
static BOOL ClassExpunge(UNUSED struct Library *base);

struct Library *DataTypesBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *PictureDTBase = NULL;

#if defined(__amigaos4__)
struct DataTypesIFace *IDataTypes = NULL;
struct CyberGfxIFace *ICyberGfx = NULL;
#endif

// some variables we carry for the whole lifetime of the lib
struct MUI_CustomClass *lib_thisClass    = NULL;
struct MUI_CustomClass *lib_spacerClass  = NULL;
struct MUI_CustomClass *lib_dragBarClass = NULL;
ULONG lib_flags = 0;

/******************************************************************************/

static BOOL ClassInit(UNUSED struct Library *base);
static BOOL ClassExpunge(UNUSED struct Library *base);

/******************************************************************************/

#include <mccinit.c>

/******************************************************************************/

static BOOL ClassInit(UNUSED struct Library *base)
{
    ENTER();

    if ((DataTypesBase = OpenLibrary("datatypes.library",37)) &&
        GETINTERFACE(IDataTypes, struct DataTypesIFace *, DataTypesBase) &&
        initSpacerClass() &&
        initDragBarClass() &&
        CreateSharedPool() == TRUE)
    {
        // we open the cybgraphics.library but without failing if
        // it doesn't exist
        CyberGfxBase = OpenLibrary("cybergraphics.library",41);
        #ifdef __amigaos4__
        if (!GETINTERFACE(ICyberGfx,struct CyberGfxIFace *,CyberGfxBase))
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

        // check the version of MUI
        if (MUIMasterBase->lib_Version>=20)
        {
            setFlag(lib_flags, BASEFLG_MUI20);

            if (MUIMasterBase->lib_Version>20 || MUIMasterBase->lib_Revision>=5341)
                setFlag(lib_flags, BASEFLG_MUI4);
        }

        GetSetPatchVersion();

        setFlag(lib_flags,BASEFLG_Init);
        lib_thisClass = ThisClass;

        RETURN(TRUE);
        return(TRUE);
    }

    ClassExpunge(base);

    RETURN(FALSE);
    return(FALSE);
}

/******************************************************************************/

static BOOL ClassExpunge(UNUSED struct Library *base)
{
    ENTER();

    DeleteSharedPool();

    // clear the data of our subclasses
    if (lib_spacerClass)
    {
        MUI_DeleteCustomClass(lib_spacerClass);
        lib_spacerClass = NULL;
    }

    if (lib_dragBarClass)
    {
        MUI_DeleteCustomClass(lib_dragBarClass);
        lib_dragBarClass = NULL;
    }

    if (PictureDTBase)
    {
        CloseLibrary(PictureDTBase);
        PictureDTBase = NULL;
    }

    if (CyberGfxBase)
    {
        DROPINTERFACE(ICyberGfx);
        CloseLibrary(CyberGfxBase);
        CyberGfxBase = NULL;
    }

    if (DataTypesBase)
    {
        DROPINTERFACE(IDataTypes);
        CloseLibrary(DataTypesBase);
        DataTypesBase = NULL;
    }

    clearFlag(lib_flags,BASEFLG_Init|BASEFLG_MUI20|BASEFLG_MUI4|BASEFLG_BROKENMOSPDT);

    LEAVE();

    RETURN(TRUE);
    return(TRUE);
}

/******************************************************************************/
