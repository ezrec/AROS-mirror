/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

***************************************************************************/

#include "class.h"
#include "private.h"
#include "rev.h"
#include <mcc_common.h>

/******************************************************************************/

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#define CLASS         "TheBar.mcp"
#define SUPERCLASSP   MUIC_Mccprefs

#define INSTDATAP     InstData

#define USERLIBID     CLASS " " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION 19

#define CLASSINIT
#define CLASSEXPUNGE
#define MIN_STACKSIZE 8192

/******************************************************************************/

struct Library *DataTypesBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *IFFParseBase = NULL;

#if !defined(__amigaos4__) && !defined(__MORPHOS__)
struct LocaleBase *LocaleBase = NULL;
#else
struct Library *LocaleBase = NULL;
#endif

#if defined(__amigaos4__)
struct DataTypesIFace *IDataTypes = NULL;
struct CyberGfxIFace *ICyberGfx = NULL;
struct IFFParseIFace *IIFFParse = NULL;
struct LocaleIFace *ILocale = NULL;
#endif

// some variables we carry for the whole lifetime of the lib
#if !defined(__MORPHOS__) && !defined(__amigaos4__)
struct MUI_CustomClass *lib_coloradjust = NULL;
struct MUI_CustomClass *lib_penadjust = NULL;
struct MUI_CustomClass *lib_backgroundadjust = NULL;
struct MUI_CustomClass *lib_poppen = NULL;
struct MUI_CustomClass *lib_popbackground = NULL;
#endif
struct Catalog *lib_cat = NULL;
ULONG lib_flags = 0;

/******************************************************************************/

static BOOL ClassInit(UNUSED struct Library *base);
static BOOL ClassExpunge(UNUSED struct Library *base);

/******************************************************************************/

#define USE_PREFSIMAGE_COLORS
#define USE_PREFSIMAGE_BODY
#define PREFSIMAGEOBJECT \
  BodychunkObject,\
    MUIA_FixWidth,              IMAGE_WIDTH,\
    MUIA_FixHeight,             IMAGE_HEIGHT,\
    MUIA_Bitmap_Width,          IMAGE_WIDTH ,\
    MUIA_Bitmap_Height,         IMAGE_HEIGHT,\
    MUIA_Bodychunk_Depth,       IMAGE_DEPTH,\
    MUIA_Bodychunk_Body,        (UBYTE *)image_body,\
    MUIA_Bodychunk_Compression, IMAGE_COMPRESSION,\
    MUIA_Bodychunk_Masking,     IMAGE_MASKING,\
    MUIA_Bitmap_SourceColors,   (ULONG *)image_palette,\
    MUIA_Bitmap_Transparent,    0,\
  End

/******************************************************************************/

#include "icon.bh"
#include <mccinit.c>

/******************************************************************************/

static BOOL
ClassInit(UNUSED struct Library *base)
{
    ENTER();

    if ((DataTypesBase = OpenLibrary("datatypes.library",37)) &&
        GETINTERFACE(IDataTypes,struct DataTypesIFace *,DataTypesBase) &&
        (IFFParseBase = OpenLibrary("iffparse.library",37)) &&
        GETINTERFACE(IIFFParse,struct IFFParseIFace *,IFFParseBase) &&
        (LocaleBase = (APTR)OpenLibrary("locale.library",36)) &&
        GETINTERFACE(ILocale,struct LocaleIFace *,LocaleBase))
    {
    	ULONG success = TRUE;

        // check for MUI 3.9+
        if (MUIMasterBase->lib_Version>=20)
        {
        	lib_flags |= BASEFLG_MUI20;

            // check for MUI 4.0+
            if (MUIMasterBase->lib_Version>20 || MUIMasterBase->lib_Revision>=5341)
            	lib_flags |= BASEFLG_MUI4;
        }

        // on MUI 3.1 system's we do have to
        // initialize our subclasses as well
        #if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
        if (!(lib_flags & BASEFLG_MUI20))
        {
        	if (!initColoradjust() ||
            	!initPenadjust() ||
              !initBackgroundadjust() ||
              !initPoppen() ||
              !initPopbackground())
            {
            	success = FALSE;
            }
        }
        #endif

        if (success)
        {
            initStrings();

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

            lib_flags |= BASEFLG_Init;

            RETURN(TRUE);
            return(TRUE);
        }
    }
  
    ClassExpunge(base);

    RETURN(FALSE);
    return(FALSE);
}

/******************************************************************************/

static BOOL
ClassExpunge(UNUSED struct Library *base)
{
    ENTER();

    #if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
    if (!(lib_flags & BASEFLG_MUI20))
    {
        freePopbackground();
        freePoppen();
        freeBackgroundadjust();
        freePenadjust();
        freeColoradjust();
    }
    #endif

    if (CyberGfxBase)
    {
        DROPINTERFACE(ICyberGfx);
        CloseLibrary(CyberGfxBase);
        CyberGfxBase = NULL;
    }

    if (LocaleBase)
    {
        if (lib_cat) CloseCatalog(lib_cat);
        DROPINTERFACE(ILocale);
        CloseLibrary((struct Library *)LocaleBase);
        LocaleBase = NULL;
    }

    if (IFFParseBase)
    {
        DROPINTERFACE(IIFFParse);
        CloseLibrary(IFFParseBase);
        IFFParseBase = NULL;
    }

    if (DataTypesBase)
    {
        DROPINTERFACE(IDataTypes);
        CloseLibrary(DataTypesBase);
        DataTypesBase = NULL;
    }

    lib_flags &= ~(BASEFLG_Init|BASEFLG_MUI20|BASEFLG_MUI4);

    RETURN(TRUE);
    return(TRUE);
}

/******************************************************************************/

#ifdef __AROS__
#include <aros/symbolsets.h>
ADD2INITLIB(ClassInit, 0);
ADD2EXPUNGELIB(ClassExpunge, 0);
#endif

/******************************************************************************/

