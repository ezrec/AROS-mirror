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
#include "version.h"
#include <mcc_common.h>

/******************************************************************************/
/* include the minimal startup code to be able to start the class from a      */
/* shell without crashing the system                                          */
/******************************************************************************/
#include "shellstart.c"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#define CLASS         "TheBar.mcp"
#define SUPERCLASSP   MUIC_Mccprefs

#define INSTDATAP     InstData

#define USERLIBID     CLASS " " LIB_REV_STRING " [" SYSTEMSHORT "/" CPU "] (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION 19

#define CLASSINIT
#define CLASSEXPUNGE

#define USEDCLASSES used_mccs
static const char *used_mccs[] = { "TheBar.mcc", "TheBarVirt.mcc", "TheButton.mcc", NULL };

#define MIN_STACKSIZE 8192

#include "locale.h"

/******************************************************************************/

struct Library *DataTypesBase = NULL;
struct Library *IFFParseBase = NULL;
#if !defined(__amigaos4__)
struct Library *CyberGfxBase = NULL;
#endif

#if !defined(__amigaos4__) && !defined(__MORPHOS__)
struct LocaleBase *LocaleBase = NULL;
#else
struct Library *LocaleBase = NULL;
#endif

#if defined(__amigaos4__)
struct DataTypesIFace *IDataTypes = NULL;
//struct CyberGfxIFace *ICyberGfx = NULL;
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
ULONG lib_flags = 0;

#if !defined(__MORPHOS__)
static BOOL nbitmapCanHandleRawData;
#endif

/******************************************************************************/

#define IDSSIZE(ids) (sizeof(ids)/sizeof(ids[0]))

static const void *regIDs[] =
{
    MSG_Reg_Colors,
    MSG_Reg_Appearance,
    MSG_Reg_Spacing,
    MSG_Reg_Options,
    NULL
};
STRPTR regs[IDSSIZE(regIDs)];

static const void *frameIDs[] =
{
    MSG_FrameStyle_Recessed,
    MSG_FrameStyle_Normal,
    NULL
};
STRPTR frames[IDSSIZE(frameIDs)];

static const void *precisionIDs[] =
{
    MSG_Precision_Gui,
    MSG_Precision_Icon,
    MSG_Precision_Image,
    MSG_Precision_Exact,
    NULL
};
STRPTR precisions[IDSSIZE(precisionIDs)];

static const void *dismodeIDs[] =
{
    MSG_DisMode_Shape,
    MSG_DisMode_Grid,
    MSG_DisMode_FullGrid,
    MSG_DisMode_Sunny,
    MSG_DisMode_Blend,
    MSG_DisMode_BlendGrey,
    NULL
};
STRPTR dismodes[IDSSIZE(dismodeIDs)];

static const void *spacersSizeIDs[] =
{
    MSG_SpacersSize_Quarter,
    MSG_SpacersSize_Half,
    MSG_SpacersSize_One,
    MSG_SpacersSize_None,
    MSG_SpacersSize_OnePoint,
    MSG_SpacersSize_TwoPoint,
    NULL
};
STRPTR spacersSizes[IDSSIZE(spacersSizeIDs)];

static const void *viewModeIDs[] =
{
    MSG_TextGfx,
    MSG_Gfx,
    MSG_Text,
    NULL
};
STRPTR viewModes[IDSSIZE(viewModeIDs)];

static const void *labelPosIDs[] =
{
    MSG_LabelPos_Bottom,
    MSG_LabelPos_Top,
    MSG_LabelPos_Right,
    MSG_LabelPos_Left,
    NULL
};
STRPTR labelPoss[IDSSIZE(labelPosIDs)];

/******************************************************************************/

static BOOL ClassInit(UNUSED struct Library *base);
static BOOL ClassExpunge(UNUSED struct Library *base);

/******************************************************************************/
#define USE_ICON8_COLORS
#define USE_ICON8_BODY

#include "icon.h"

#if defined(__MORPHOS__)
#include <mui/Rawimage_mcc.h>
#else
#include <mui/NBitmap_mcc.h>
#endif

static Object *get_prefs_image(void)
{
  Object *obj;

  #if !defined(__MORPHOS__)
  if(nbitmapCanHandleRawData == TRUE)
  {
    obj = NBitmapObject,
      MUIA_FixWidth,       ICON32_WIDTH,
      MUIA_FixHeight,      ICON32_HEIGHT,
      MUIA_NBitmap_Type,   MUIV_NBitmap_Type_ARGB32,
      MUIA_NBitmap_Normal, icon32,
      MUIA_NBitmap_Width,  ICON32_WIDTH,
      MUIA_NBitmap_Height, ICON32_HEIGHT,
    End;
  }
  else
  {
    obj = NULL;
  }
  #else
  /*
  no truecolor image data for MorphOS yet
  obj = RawimageObject,
    MUIA_Rawimage_Data, icon32,
  End;
  */
  obj = NULL;
  #endif

  // if the 32bit image data couldn't be loaded
  // we fall back to the 8bit icon
  if(obj == NULL)
  {
    obj = BodychunkObject,\
      MUIA_FixWidth,              ICON8_WIDTH,\
      MUIA_FixHeight,             ICON8_HEIGHT,\
      MUIA_Bitmap_Width,          ICON8_WIDTH ,\
      MUIA_Bitmap_Height,         ICON8_HEIGHT,\
      MUIA_Bodychunk_Depth,       ICON8_DEPTH,\
      MUIA_Bodychunk_Body,        (UBYTE *)icon8_body,\
      MUIA_Bodychunk_Compression, ICON8_COMPRESSION,\
      MUIA_Bodychunk_Masking,     ICON8_MASKING,\
      MUIA_Bitmap_SourceColors,   (ULONG *)icon8_colors,\
      MUIA_Bitmap_Transparent,    0,\
    End;
  }

  return obj;
}

#define PREFSIMAGEOBJECT get_prefs_image()

#include "mccinit.c"

/******************************************************************************/

static BOOL ClassInit(UNUSED struct Library *base)
{
  ENTER();

  if((DataTypesBase = OpenLibrary("datatypes.library",37)) &&
     GETINTERFACE(IDataTypes,struct DataTypesIFace *,DataTypesBase) &&
     (IFFParseBase = OpenLibrary("iffparse.library",37)) &&
     GETINTERFACE(IIFFParse,struct IFFParseIFace *,IFFParseBase) &&
     (LocaleBase = (APTR)OpenLibrary("locale.library",36)) &&
     GETINTERFACE(ILocale,struct LocaleIFace *,LocaleBase))
  {
    BOOL success = TRUE;

    // check for MUI 3.9+
    if(LIB_VERSION_IS_AT_LEAST(MUIMasterBase, 20, 0))
    {
      lib_flags |= BASEFLG_MUI20;

      // check for MUI 4.0+
      if(LIB_VERSION_IS_AT_LEAST(MUIMasterBase, 20, 5500))
        lib_flags |= BASEFLG_MUI4;
    }

    // on MUI 3.1 system's we do have to
    // initialize our subclasses as well
    #if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
    if(!(lib_flags & BASEFLG_MUI20))
    {
      if(!initColoradjust() ||
         !initPenadjust() ||
         !initBackgroundadjust() ||
         !initPoppen() ||
         !initPopbackground())
      {
        success = FALSE;
      }
    }
    #endif

    if(success == TRUE)
    {
      // open the TheBar_mcp catalog
      OpenCat();

      LocalizeArray(regs,regIDs);
      LocalizeArray(frames,frameIDs);
      LocalizeArray(precisions,precisionIDs);
      LocalizeArray(dismodes,dismodeIDs);
      LocalizeArray(spacersSizes,spacersSizeIDs);
      LocalizeArray(viewModes,viewModeIDs);
      LocalizeArray(labelPoss,labelPosIDs);

      #if defined(__amigaos3__)
      dismodes[4] = NULL;
      #endif

      // we open the cybgraphics.library but without failing if
      // it doesn't exist
      #if !defined(__amigaos4__)
      CyberGfxBase = OpenLibrary("cybergraphics.library", 41);
/*
      if(!GETINTERFACE(ICyberGfx, struct CyberGfxIFace *, CyberGfxBase))
      {
        CloseLibrary(CyberGfxBase);
        CyberGfxBase = NULL;
      }
*/
      #endif

      #if !defined(__MORPHOS__)
      {
        struct Library *nbitmapMcc;

        nbitmapCanHandleRawData = FALSE;

        // we need at least NBitmap.mcc V15.8 to be able to let it handle raw image data
        if((nbitmapMcc = OpenLibrary("mui/NBitmap.mcc", 0)) != NULL)
        {
          SHOWVALUE(DBF_ALWAYS, nbitmapMcc->lib_Version);
          SHOWVALUE(DBF_ALWAYS, nbitmapMcc->lib_Revision);

          if(nbitmapMcc->lib_Version > 15 || (nbitmapMcc->lib_Version == 15 && nbitmapMcc->lib_Revision >= 8))
            nbitmapCanHandleRawData = TRUE;

          CloseLibrary(nbitmapMcc);
        }

        SHOWVALUE(DBF_ALWAYS, nbitmapCanHandleRawData);
      }
      #endif

      lib_flags |= BASEFLG_Init;

      RETURN(TRUE);
      return TRUE;
    }
  }

  ClassExpunge(base);

  RETURN(FALSE);
  return FALSE;
}

/******************************************************************************/

static BOOL
ClassExpunge(UNUSED struct Library *base)
{
  ENTER();

  #if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
  if(!(lib_flags & BASEFLG_MUI20))
  {
    freePopbackground();
    freePoppen();
    freeBackgroundadjust();
    freePenadjust();
    freeColoradjust();
  }
  #endif

  #if !defined(__amigaos4__)
  if(CyberGfxBase != NULL)
  {
    DROPINTERFACE(ICyberGfx);
    CloseLibrary(CyberGfxBase);
    CyberGfxBase = NULL;
  }
  #endif

  // close the catalog
  CloseCat();

  if(LocaleBase != NULL)
  {
    DROPINTERFACE(ILocale);
    CloseLibrary((struct Library *)LocaleBase);
    LocaleBase = NULL;
  }

  if(IFFParseBase != NULL)
  {
    DROPINTERFACE(IIFFParse);
    CloseLibrary(IFFParseBase);
    IFFParseBase = NULL;
  }

  if(DataTypesBase != NULL)
  {
    DROPINTERFACE(IDataTypes);
    CloseLibrary(DataTypesBase);
    DataTypesBase = NULL;
  }

  lib_flags &= ~(BASEFLG_Init|BASEFLG_MUI20|BASEFLG_MUI4);

  RETURN(TRUE);
  return TRUE;
}

/******************************************************************************/
