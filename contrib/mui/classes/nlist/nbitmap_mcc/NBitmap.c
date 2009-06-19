/***************************************************************************

 NBitmap.mcc - New Bitmap MUI Custom Class
 Copyright (C) 2006 by Daniel Allsopp
 Copyright (C) 2007-2008 by NList Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

// ansi includes
#include <string.h>

// system includes
#include <proto/exec.h>
#include <proto/datatypes.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <mui/NBitmap_mcc.h>

// system
#include <datatypes/pictureclass.h>

#if defined(__amigaos4__)
#include <graphics/blitattr.h>
#else
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#endif

#if defined(__MORPHOS__)
#include <exec/execbase.h>
#include <exec/system.h>
#endif

// libraries
#include <libraries/mui.h>

// local includes
#include "NBitmap.h"
#include "private.h"
#include "version.h"
#include "Debug.h"

#if !defined(__amigaos4__) && !defined(__MORPHOS__)
//#include "WritePixelArrayAlpha.c"
  #ifndef WritePixelArrayAlpha
    #if defined(__SASC)
      ULONG WritePixelArrayAlpha(APTR, UWORD, UWORD, UWORD, struct RastPort *, UWORD, UWORD, UWORD, UWORD, ULONG);
      #pragma libcall CyberGfxBase WritePixelArrayAlpha d8 76543921080A
    #else
      #define WritePixelArrayAlpha(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7, __p8, __p9) \
        LP10(216, ULONG , WritePixelArrayAlpha, \
          APTR , __p0, a0, \
          UWORD , __p1, d0, \
          UWORD , __p2, d1, \
          UWORD , __p3, d2, \
          struct RastPort *, __p4, a1, \
          UWORD , __p5, d3, \
          UWORD , __p6, d4, \
          UWORD , __p7, d5, \
          UWORD , __p8, d6, \
          ULONG , __p9, d7, \
          , CYBERGRAPHICS_BASE_NAME)
    #endif
  #endif
#endif

// functions
/// GetConfigItem()
//
ULONG GetConfigItem(Object *obj, ULONG configitem, ULONG defaultsetting)
{
  ULONG value;
  ULONG result = defaultsetting;

  ENTER();

  if(DoMethod(obj, MUIM_GetConfigItem, configitem, &value))
    result = *(ULONG *)value;

  RETURN(result);
  return result;
}
///
/// InitConfig()
//
VOID InitConfig(Object *obj, struct InstData *data)
{
  ENTER();

  if(obj != NULL && data != NULL)
  {
    data->prefs.show_label = 0;
    data->prefs.overlay_type = 0;
    data->prefs.overlay_r = 10;
    data->prefs.overlay_g = 36;
    data->prefs.overlay_b = 106;
    data->prefs.overlay_shadeover = 1500; // = 1.5 if divided by 1000
    data->prefs.overlay_shadepress = 2500; // = 2.5 if divided by 1000
    data->prefs.spacing_horiz = 2;
    data->prefs.spacing_vert = 2;
  }

  LEAVE();
}
///
/// FreeConfig()
//
VOID FreeConfig(struct InstData *data)
{
  ENTER();

  if(data != NULL)
  {
    // nothing yet
  }

  LEAVE();
}
///
/// NBitmap_LoadImage()
//
BOOL NBitmap_LoadImage(STRPTR filename, uint32 item, struct IClass *cl, Object *obj)
{
  BOOL result = FALSE;
  struct InstData *data;

  if((data = INST_DATA(cl, obj)) != NULL && filename != NULL)
  {
    data->dt_obj[item] = NewDTObject(filename,
                                     DTA_GroupID,           GID_PICTURE,
                                     OBP_Precision,         PRECISION_EXACT,
                                     PDTA_FreeSourceBitMap, TRUE,
                                     PDTA_DestMode,         PMODE_V43,
                                     PDTA_UseFriendBitMap,  TRUE,
                                     TAG_DONE);
    SHOWVALUE(DBF_DATATYPE, data->dt_obj[item]);
    if(data->dt_obj[item] != NULL)
      result = TRUE;
  }

  RETURN(result);
  return result;
}
///
/// NBitmap_UpdateImage()
//
VOID NBitmap_UpdateImage(uint32 item, STRPTR filename, struct IClass *cl, Object *obj)
  {
    struct InstData *data = NULL;
    struct Screen *scr = NULL;

    if((data = INST_DATA(cl, obj)) != NULL)
    {
      if(filename != NULL)
      {
        if(data->dt_obj[item] != NULL)
        {
          /* free old image data */
          if(data->fmt == PBPAFMT_LUT8)
            SetDTAttrs(data->dt_obj[item], NULL, NULL, PDTA_Screen, NULL, TAG_DONE);

          DisposeDTObject(data->dt_obj[item]);
          data->dt_obj[item] = NULL;

          if(data->arraypixels[item] != NULL)
          {
            FreeVec(data->arraypixels[item]);
            data->arraypixels[item] = NULL;
          }

          /* load new image */
          if((NBitmap_LoadImage(filename, item, cl, obj)) != FALSE)
          {
            /* setup new image */
            if((NBitmap_ExamineData(data->dt_obj[item], item, cl, obj)) != FALSE)
            {
              /* screen */
              scr = _screen(obj);
              data->scrdepth = GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH);

              if(data->fmt == PBPAFMT_LUT8)
              {
                /* layout image */
                SetDTAttrs(data->dt_obj[item], NULL, NULL, PDTA_Screen, scr, TAG_DONE);
                if(DoMethod(data->dt_obj[item], DTM_PROCLAYOUT, NULL, 1))
                {
                  GetDTAttrs(data->dt_obj[item], PDTA_CRegs, &data->dt_colours[item], TAG_DONE);
                  GetDTAttrs(data->dt_obj[item], PDTA_MaskPlane, &data->dt_mask[item], TAG_DONE);
                  GetDTAttrs(data->dt_obj[item], PDTA_DestBitMap, &data->dt_bitmap[item], TAG_DONE);

                  if(data->dt_bitmap[item] == NULL) GetDTAttrs(data->dt_obj[item], PDTA_BitMap, &data->dt_bitmap[item], TAG_DONE);
                }
              }
            }
          }

        }
      }
    }
  }
///
/// NBitmap_ExamineData()
//
BOOL NBitmap_ExamineData(Object *dt_obj, uint32 item, struct IClass *cl, Object *obj)
  {
    BOOL result = FALSE;
    ULONG arraysize;

    struct pdtBlitPixelArray pbpa;
    struct InstData *data = INST_DATA(cl, obj);

    if(dt_obj != NULL)
    {
      /* bitmap header */
      GetDTAttrs(dt_obj, PDTA_BitMapHeader, &data->dt_header[item], TAG_DONE);
      D(DBF_DATATYPE, "examine: BMHD dimensions %ldx%ldx%ld", data->dt_header[item]->bmh_Width, data->dt_header[item]->bmh_Height, data->dt_header[item]->bmh_Depth);
      data->depth = data->dt_header[item]->bmh_Depth;

      if(data->depth>0 && data->depth<=8)
      {
        /* colour lookup bitmap */
        data->fmt = PBPAFMT_LUT8;

        /* bitmap header */
        data->width =  data->dt_header[0]->bmh_Width;
        data->height =  data->dt_header[0]->bmh_Height;

        result = TRUE;
        D(DBF_DATATYPE, "examine: using LUT8 bitmaps");
      }
      else if(data->depth >=24)
      {
        #if defined(__MORPHOS__)
        /* XXX: Check out is this needed in OS 3 */
        IPTR use_alpha;

        GetDTAttrs(dt_obj, PDTA_AlphaChannel, (IPTR)&use_alpha, TAG_DONE);

        if (use_alpha)
          data->depth = 32;
        #endif

        /* true colour bitmap */
        if(data->depth == 24)
        {
          data->fmt = PBPAFMT_RGB;
          D(DBF_DATATYPE, "examine: using 24bit RGB data");
        }
        else if(data->depth == 32)
        {
          data->fmt = PBPAFMT_ARGB;
          D(DBF_DATATYPE, "examine: using 32bit ARGB data");
        }

        data->width =  data->dt_header[0]->bmh_Width;
        data->height =  data->dt_header[0]->bmh_Height;
        data->arraybpp = data->depth/8;
        data->arraybpr = data->arraybpp * data->width;

        #if defined(__MORPHOS__)
        if (SysBase->LibNode.lib_Version >= 51)
        {
          ULONG altivec_align = 0;

          NewGetSystemAttrs(&altivec_align, sizeof(altivec_align), SYSTEMINFOTYPE_PPC_ALTIVEC, TAG_DONE);

          if (altivec_align)
            data->arraybpr = (data->arraybpr + 15) & ~15;
        }
        #endif

        arraysize = (data->arraybpr) * data->height;

        /* get array of pixels */
        if((data->arraypixels[item] = AllocVec(arraysize, MEMF_ANY|MEMF_CLEAR)) != NULL)
        {
          ULONG error;

          memset(&pbpa, 0, sizeof(struct pdtBlitPixelArray));

          pbpa.MethodID = PDTM_READPIXELARRAY;
          pbpa.pbpa_PixelData = data->arraypixels[item];
          pbpa.pbpa_PixelFormat = data->fmt;
          pbpa.pbpa_PixelArrayMod = data->arraybpr;
          pbpa.pbpa_Left = 0;
          pbpa.pbpa_Top = 0;
          pbpa.pbpa_Width = data->width;
          pbpa.pbpa_Height = data->height;

          error = DoMethodA(dt_obj, (Msg)(VOID*)&pbpa);
          D(DBF_DATATYPE, "examine: READPIXELARRAY returned %ld", error);

          result = TRUE;
        }
      }
    }

    return(result);
  }
///
/// NBitmap_FreeImage()
//
VOID NBitmap_FreeImage(uint32 item, struct IClass *cl, Object *obj)
{
  struct InstData *data = NULL;

  if(((data = INST_DATA(cl, obj)) != NULL))
  {
    if(data->dt_obj[item] != NULL)
    {
      DisposeDTObject(data->dt_obj[item]);
      data->dt_obj[item] = NULL;

      if(data->arraypixels[item] != NULL)
      {
        FreeVec(data->arraypixels[item]);
        data->arraypixels[item] = NULL;
      }
    }
  }
}

///
/// NBitmap_SetupShades()
// create an ARGB shade
BOOL NBitmap_SetupShades(struct InstData *data)
{
  uint32 pixel, altivec_align;

  ENTER();

  altivec_align = 0;

  #if defined(__MORPHOS__)
  if (SysBase->LibNode.lib_Version >= 51)
  {
    NewGetSystemAttrs(&altivec_align, sizeof(altivec_align), SYSTEMINFOTYPE_PPC_ALTIVEC, TAG_DONE);
  }
  #endif

  data->shadeWidth = data->width + data->border_horiz - 2;
  data->shadeHeight = data->height + data->border_vert - 2;
  data->shadeBytesPerRow = data->shadeWidth * 4;

  if (altivec_align)
    data->shadeBytesPerRow = (data->shadeBytesPerRow + 15) & ~15;

  // the shades pixel color
  pixel = ((ULONG)data->prefs.overlay_r << 16) | ((ULONG)data->prefs.overlay_g << 8) | (ULONG)data->prefs.overlay_b;

  #if defined(__MORPHOS__)
  if((data->pressedShadePixels = AllocVecAligned(data->shadeBytesPerRow * data->shadeHeight, MEMF_ANY, altivec_align ? 16 : 8, 0)) != NULL)
  #else
  if((data->pressedShadePixels = AllocVec(data->shadeBytesPerRow * data->shadeHeight, MEMF_ANY)) != NULL)
  #endif
  {
    uint32 w, h;
    uint32 alpha;
    uint32 *p = data->pressedShadePixels;

    // calculate the alpha channel value
    alpha = (255L - (((255L * 1000L) / (uint32)data->prefs.overlay_shadepress) & 0xff)) << 24;

    // fill the array with the pixel and alpha channel value
    // the border will be the 100% opaque pixel color
    for(h = 0; h < data->shadeHeight; h++)
    {
      for(w = 0; w < data->shadeWidth; w++)
      {
        if(h == 0 || h == data->shadeHeight-1 || w == 0 || w == data->shadeWidth-1)
          *p++ = 0xff000000 | pixel;
        else
          *p++ = alpha | pixel;
      }

      p += (data->shadeBytesPerRow - data->shadeWidth * 4) / 4;
    }
  }

  #if defined(__MORPHOS__)
  if((data->overShadePixels = AllocVecAligned(data->shadeBytesPerRow * data->shadeHeight, MEMF_ANY, altivec_align ? 16 : 8, 0)) != NULL)
  #else
  if((data->overShadePixels = AllocVec(data->shadeBytesPerRow * data->shadeHeight, MEMF_ANY)) != NULL)
  #endif
  {
    uint32 w, h;
    uint32 alpha;
    uint32 *p = data->overShadePixels;

    // calculate the alpha channel value
    alpha = (255L - (((255L * 1000L) / (uint32)data->prefs.overlay_shadeover) & 0xff)) << 24;

    // fill the array with the pixel and alpha channel value
    // the border will be the 100% opaque pixel color
    for(h = 0; h < data->shadeHeight; h++)
    {
      for(w = 0; w < data->shadeWidth; w++)
      {
        if(h == 0 || h == data->shadeHeight-1 || w == 0 || w == data->shadeWidth-1)
          *p++ = 0xff000000 | pixel;
        else
          *p++ = alpha | pixel;
      }

      p += (data->shadeBytesPerRow - data->shadeWidth * 4) / 4;
    }
  }

  RETURN((data->pressedShadePixels != NULL && data->overShadePixels != NULL));
  return (data->pressedShadePixels != NULL && data->overShadePixels != NULL);
}

///
/// CleanupShades()
// delete the ARGB shades
void NBitmap_CleanupShades(struct InstData *data)
{
  ENTER();

  if(data->pressedShadePixels != NULL)
  {
    FreeVec(data->pressedShadePixels);
    data->pressedShadePixels = NULL;
  }
  if(data->overShadePixels != NULL)
  {
    FreeVec(data->overShadePixels);
    data->overShadePixels = NULL;
  }

  LEAVE();
}

///
/// NBitmap_NewImage()
//
BOOL NBitmap_NewImage(struct IClass *cl, Object *obj)
{
  BOOL result = FALSE;
  struct InstData *data;

  ENTER();

  if(((data = INST_DATA(cl, obj))!=NULL) && (data->dt_obj[0]))
  {
    ULONG i;

    for(i=0;i<3;i++)
    {
      if(data->dt_obj[i] != NULL)
        result = NBitmap_ExamineData(data->dt_obj[i], i, cl, obj);
    }
  }

  RETURN(result);
  return result;
}

BOOL NBitmap_OldNewImage(struct IClass *cl, Object *obj)
{
  BOOL result = FALSE;
  struct InstData *data;

  /* need at least the normal image */
  if((data = INST_DATA(cl, obj)) !=NULL && data->dt_obj[0] != NULL)
  {
    ULONG i;

    for(i = 0; i < 3; i++)
    {
      if(data->dt_obj[i] != NULL)
      {
        struct FrameInfo fri;

        memset(&fri, 0, sizeof(struct FrameInfo));
        DoMethod(data->dt_obj[0], DTM_FRAMEBOX, NULL, &fri, &fri, sizeof(struct FrameInfo), 0);
        data->depth = fri.fri_Dimensions.Depth;
        D(DBF_DATATYPE, "new: framebox dimensions %ldx%ldx%ld", fri.fri_Dimensions.Width, fri.fri_Dimensions.Height, fri.fri_Dimensions.Depth);

        if(data->maxwidth == 0 || (data->maxwidth <= data->dt_header[i]->bmh_Width))
        {
          if(data->maxheight == 0 || (data->maxheight <= data->dt_header[i]->bmh_Height))
          {
            if(data->depth > 0 && data->depth <= 8)
            {
              /* colour lookup bitmap */
              data->fmt = PBPAFMT_LUT8;

              /* bitmap header */
              GetDTAttrs(data->dt_obj[i], PDTA_BitMapHeader, &data->dt_header[i], TAG_DONE);
              data->width =  data->dt_header[0]->bmh_Width;
              data->height =  data->dt_header[0]->bmh_Height;
              D(DBF_DATATYPE, "new: using LUT8 bitmaps");

              result = TRUE;
            }
            else if(data->depth > 8)
            {
              ULONG arraysize;

              /* correct read buffer */
              if(data->depth == 24)
              {
                data->fmt = PBPAFMT_RGB;
                D(DBF_DATATYPE, "new: using 24bit RGB data");
              }
              else
              {
                data->fmt = PBPAFMT_ARGB;
                D(DBF_DATATYPE, "new: using 32bit ARGB data");
              }

              /* bitmap header */
              GetDTAttrs(data->dt_obj[i], PDTA_BitMapHeader, &data->dt_header[i], TAG_DONE);
              data->width =  data->dt_header[0]->bmh_Width;
              data->height =  data->dt_header[0]->bmh_Height;
              data->arraybpp = data->depth / 8;
              data->arraybpr = data->arraybpp * data->width;
              arraysize = (data->arraybpr) * data->height;

              /* get array of pixels */
              if((data->arraypixels[i] = AllocVec(arraysize, MEMF_ANY|MEMF_CLEAR)) != NULL)
              {
                ULONG error;

                error = DoMethod(data->dt_obj[i], PDTM_READPIXELARRAY, data->arraypixels[i], data->fmt, data->arraybpr, 0, 0, data->width, data->height);
                D(DBF_DATATYPE, "new: READPIXELARRAY returned %ld", error);

                // finally create the shades
                result = NBitmap_SetupShades(data);
              }
            }
          }
        }
      }
    }
  }

  RETURN(result);
  return result;
}
///
/// NBitmap_DisposeImage()
//
VOID NBitmap_DisposeImage(struct IClass *cl, Object *obj)
{
  struct InstData *data;

  ENTER();

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    ULONG i;

    /* free datatype object */
    if(data->type == MUIV_NBitmap_Type_File)
    {
      for(i =0 ; i < 3; i++)
      {
        SHOWVALUE(DBF_DATATYPE, data->dt_obj[i]);
        if(data->dt_obj[i] != NULL)
        {
          DisposeDTObject(data->dt_obj[i]);
          data->dt_obj[i] = NULL;
        }
      }
    }

    if(data->label != NULL)
    {
      FreeVec(data->label);
      data->label = NULL;
    }

    /* free pixel memory */
    for(i = 0; i < 3; i++)
    {
      if(data->arraypixels[i] != NULL)
      {
        FreeVec(data->arraypixels[i]);
        data->arraypixels[i] = NULL;
      }
    }

    NBitmap_CleanupShades(data);
  }

  LEAVE();
}
///
/// NBitmap_SetupImage()
//
BOOL NBitmap_SetupImage(struct IClass *cl, Object *obj)
{
  struct InstData *data;
  BOOL result = FALSE;

  ENTER();

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    struct Screen *scr = NULL;

    /* stored config */
    InitConfig(obj, data);

    /* screen */
    scr = _screen(obj);
    data->scrdepth = GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH);

    /* input */
    if(data->button)
    {
      data->ehnode.ehn_Priority = 0;
      data->ehnode.ehn_Flags = MUI_EHF_GUIMODE;
      data->ehnode.ehn_Object = obj;
      data->ehnode.ehn_Class = cl;
      data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE;

      DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehnode);
    }

    /* 8-bit data */
    if(data->fmt == PBPAFMT_LUT8 && data->dt_obj[0] != NULL)
    {
      ULONG i;

      /* layout image */
      for(i = 0; i < 3; i++)
      {
        // set the new screen for this object
        SetDTAttrs(data->dt_obj[i], NULL, NULL, PDTA_Screen, scr, TAG_DONE);
        if(DoMethod(data->dt_obj[i], DTM_PROCLAYOUT, NULL, 1))
        {
          GetDTAttrs(data->dt_obj[i], PDTA_CRegs, &data->dt_colours[i],
                                      PDTA_MaskPlane, &data->dt_mask[i],
                                      PDTA_DestBitMap, &data->dt_bitmap[i],
                                      TAG_DONE);
          if(data->dt_bitmap[i] == NULL)
            GetDTAttrs(data->dt_obj[i], PDTA_BitMap, &data->dt_bitmap[i], TAG_DONE);
          SHOWVALUE(DBF_DATATYPE, data->dt_bitmap[i]);
        }
      }

      result = TRUE;
    }
    else if(data->depth > 8)
      result = NBitmap_SetupShades(data);
  }

  RETURN(result);
  return result;
}
///
/// NBitmap_CleanupImage()
//
VOID NBitmap_CleanupImage(struct IClass *cl, Object *obj)
{
  struct InstData *data;

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    // input
    if(data->button)
      DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->ehnode);

    if(data->fmt == PBPAFMT_LUT8 && data->dt_obj[0] != NULL)
    {
      ULONG i;

      /* layout image */
      for(i = 0; i < 3; i++)
      {
        // reset the screen pointer
        SetDTAttrs(data->dt_obj[i], NULL, NULL, PDTA_Screen, NULL, TAG_DONE);
      }
    }

    NBitmap_CleanupShades(data);

    // stored config
    FreeConfig(data);
  }
}
///
/// NBitmap_DrawSimpleFrame()
//
VOID NBitmap_DrawSimpleFrame(Object *obj, uint32 x, uint32 y, uint32 w, uint32 h)
{
  ENTER();

  SetAPen(_rp(obj), _pens(obj)[MPEN_SHADOW]);
  Move(_rp(obj), x, y+(h+1));
  Draw(_rp(obj), x, y);
  Draw(_rp(obj), x+(w+1), y);

  SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
  Draw(_rp(obj), x+(w+1), y+(h+1));
  Draw(_rp(obj), x, y+(h+1));

  LEAVE();
}
///
/// NBitmap_DrawImage()
//
BOOL NBitmap_DrawImage(struct IClass *cl, Object *obj)
{
  struct InstData *data;
  struct MUI_AreaData *areadata = NULL;

  ENTER();

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    LONG item;
    ULONG x, y, twidth, theight;

    areadata = muiAreaData(obj);

    /* coordinates */
    item = 0;
    x = areadata->mad_Box.Left;
    y = areadata->mad_Box.Top;
    twidth = (data->width + data->border_horiz) - 2;      /* subtract standard 1 pixel border */
    theight = (data->height + data->border_vert) - 2;

   /* clear */
   if(data->button) DoMethod(obj, MUIM_DrawBackground, areadata->mad_Box.Left, areadata->mad_Box.Top, areadata->mad_Box.Width, areadata->mad_Box.Height, 0, 0);

   /* label */
   if(data->label != NULL && data->button != FALSE)
   {
     uint32 labelx;

     SetFont(_rp(obj), _font(obj));
     SetAPen(_rp(obj), 1);

     labelx = (twidth/2) - (data->labelte.te_Width/2);

     Move(_rp(obj), x + labelx, _bottom(obj) - 3);
     Text(_rp(obj), data->label, strlen(data->label));
   }

    /* draw image */
    if(data->fmt == PBPAFMT_LUT8)
    {
      #if defined(__amigaos4__)
      uint32 error;
      #endif

      /* select bitmap */
      if(data->button && data->pressed && data->overlay && data->dt_bitmap[2])
        item = 2;

      SHOWVALUE(DBF_DRAW, item);
      SHOWVALUE(DBF_DRAW, data->dt_bitmap[item]);
      SHOWVALUE(DBF_DRAW, data->dt_mask[item]);

      #if defined(__amigaos4__)
      error = BltBitMapTags(BLITA_Source, data->dt_bitmap[item],
                            BLITA_Dest, _rp(obj),
                            BLITA_SrcX, 0,
                            BLITA_SrcY, 0,
                            BLITA_DestX, x + (data->border_horiz / 2),
                            BLITA_DestY, y + ((data->border_vert / 2) - (data->label_vert/2)),
                            BLITA_Width, data->width,
                            BLITA_Height, data->height,
                            BLITA_SrcType, BLITT_BITMAP,
                            BLITA_DestType, BLITT_RASTPORT,
                            BLITA_MaskPlane, data->dt_mask[item],
                            TAG_DONE);
      SHOWVALUE(DBF_DRAW, error);

      #else

      if(data->dt_mask[item] != NULL)
      {
        BltMaskBitMapRastPort(data->dt_bitmap[item], 0, 0, _rp(obj),
          _left(obj) + (data->border_horiz / 2),
          _top(obj) + (data->border_vert / 2),
          data->width,
          data->height,
          0xc0,
          (APTR)data->dt_mask[item]);
      }
      else
      {
        BltBitMapRastPort(data->dt_bitmap[item], 0, 0, _rp(obj),
          _left(obj) + (data->border_horiz / 2),
          _top(obj) + (data->border_vert / 2),
          data->width,
          data->height,
          0xc0);
      }
      #endif
    }
    else
    {
      /* select bitmap */
      if(data->button && data->pressed && data->overlay && data->arraypixels[2] != NULL)
        item = 2;

      SHOWVALUE(DBF_DRAW, item);
      SHOWVALUE(DBF_DRAW, data->arraypixels[item]);

      if(data->arraypixels[item] != NULL)
      {
        #if defined(__amigaos4__)
        int32 srctype;
        uint32 error;

        if(data->depth == 24)
         srctype = BLITT_RGB24;
        else
         srctype = BLITT_ARGB32;

        error = BltBitMapTags(BLITA_Source, data->arraypixels[item],
                              BLITA_Dest, _rp(obj),
                              BLITA_SrcX, 0,
                              BLITA_SrcY, 0,
                              BLITA_DestX, x + (data->border_horiz / 2),
                              BLITA_DestY, y + ((data->border_vert / 2) - (data->label_vert/2)),
                              BLITA_Width, data->width,
                              BLITA_Height, data->height,
                              BLITA_SrcType, srctype,
                              BLITA_DestType, BLITT_RASTPORT,
                              BLITA_SrcBytesPerRow, data->arraybpr,
                              BLITA_UseSrcAlpha, TRUE,
                              TAG_DONE);

        SHOWVALUE(DBF_DRAW, error);

        #else

        if(data->depth == 24)
          WritePixelArray(data->arraypixels[item], 0, 0, data->arraybpr, _rp(obj), _left(obj) + (data->border_horiz / 2), _top(obj) + (data->border_vert / 2), data->width, data->height, RECTFMT_RGB);
        else
          WritePixelArrayAlpha(data->arraypixels[item], 0, 0, data->arraybpr, _rp(obj), _left(obj) + (data->border_horiz / 2), _top(obj) + (data->border_vert / 2), data->width, data->height, 0xffffffff);

        #endif
      }
    }

    /* overlay */
    if(data->button && data->overlay)
    {
      if(data->prefs.overlay_type == 1 || data->scrdepth <= 8)
      {
        /* standard overlay */
        if(data->pressed)
          NBitmap_DrawSimpleFrame(obj, _left(obj), _top(obj), data->width, data->height);
        else
          NBitmap_DrawSimpleFrame(obj, _left(obj), _top(obj), data->width, data->height);
      }
      else
      {
        #if defined(__amigaos4__)
        uint32 error;

        if(data->pressed)
          error = BltBitMapTags(BLITA_Source, data->pressedShadePixels,
                                BLITA_Dest, _rp(obj),
                                BLITA_SrcX, 0,
                                BLITA_SrcY, 0,
                                BLITA_DestX, x+1,
                                BLITA_DestY, y+1,
                                BLITA_Width, data->shadeWidth,
                                BLITA_Height, data->shadeHeight,
                                BLITA_SrcType, BLITT_ARGB32,
                                BLITA_DestType, BLITT_RASTPORT,
                                BLITA_SrcBytesPerRow, data->shadeBytesPerRow,
                                BLITA_UseSrcAlpha, TRUE,
                                TAG_DONE);
        else
          error = BltBitMapTags(BLITA_Source, data->overShadePixels,
                                BLITA_Dest, _rp(obj),
                                BLITA_SrcX, 0,
                                BLITA_SrcY, 0,
                                BLITA_DestX, x+1,
                                BLITA_DestY, y+1,
                                BLITA_Width, data->shadeWidth,
                                BLITA_Height, data->shadeHeight,
                                BLITA_SrcType, BLITT_ARGB32,
                                BLITA_DestType, BLITT_RASTPORT,
                                BLITA_SrcBytesPerRow, data->shadeBytesPerRow,
                                BLITA_UseSrcAlpha, TRUE,
                                TAG_DONE);

        SHOWVALUE(DBF_DRAW, error);

        #else

        if(data->pressed)
          WritePixelArrayAlpha(data->pressedShadePixels, 0, 0, data->shadeBytesPerRow, _rp(obj), x+1, y+1, data->shadeWidth, data->shadeHeight, 0xffffffff);
        else
          WritePixelArrayAlpha(data->overShadePixels, 0, 0, data->shadeBytesPerRow, _rp(obj), x+1, y+1, data->shadeWidth, data->shadeHeight, 0xffffffff);

        #endif
      }
    }
  }

  RETURN(FALSE);
  return FALSE;
}
///

