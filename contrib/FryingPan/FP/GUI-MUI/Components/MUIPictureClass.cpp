/*
 * FryingPan - Amiga CD/DVD Recording Software (User Interface and supporting Libraries only)
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "MUIPictureClass.h"
#include <Generic/LibrarySpool.h>
#include <libclass/intuition.h>
#include <libclass/muimaster.h>
#include <libraries/mui.h>
#include <libclass/utility.h>

#define __NOGLOBALIFACE__
#include <proto/graphics.h>

   static struct Library *GfxBase = 0;
#ifdef __AMIGAOS4__
   static struct GraphicsIFace *IGraphics = 0;
#endif

   static int            openCount = 0;

MUIPictureClass::MUIPictureClass(IClass *cls)
{
   openCount++;
   parent   = cls;
   image1   = "";
   image2   = "";
   width    = 64;
   height   = 48;
   dtimg1   = 0;
   dtimg2   = 0;
   dt       = DatatypesIFace::GetInstance(44);
   if (dt == 0)
   {
      request("Warning", "This program requires datatypes.library v44 or later\nGraphical buttons will not be displayed.", "Continue", 0);
   }

   if (GfxBase == 0)
   {
      GfxBase = Exec->OpenLibrary("graphics.library", 39);
   }
#ifdef __AMIGAOS4__
   if ((GfxBase != 0) && (IGraphics == 0))
   {
      IGraphics = (GraphicsIFace*)Exec->GetInterface(GfxBase, "main", 1, (TagItem*)NULL);
   }
#endif
}

MUIPictureClass::~MUIPictureClass()
{
   closeImages();
   dt->FreeInstance();
   openCount--;
   if (0 == openCount)
   {
#ifdef __AMIGAOS4__
      Exec->DropInterface((Interface*)IGraphics);
      IGraphics = 0;
#endif
      Exec->CloseLibrary(GfxBase);
      GfxBase = 0;
   }
}

IPTR MUIPictureClass::DoMtd(Object *obj, IPTR msg)
{
   uint16  *minmax;
   IPTR      k;

   switch (((IPTR *)msg)[0]) 
   {
      case OM_NEW:
         {
            if (!(obj = (Object *)DoSuperMtd(parent, obj, msg))) 
               return 0;

            k = (IPTR)Utility->GetTagData(MUIA_Picture_NormalImage, 0, (TagItem*)((IPTR *)msg)[1]);
            if (k != 0)
               image1 = (char*)k;
            k = (IPTR)Utility->GetTagData(MUIA_Picture_SelectedImage, 0, (TagItem*)((IPTR *)msg)[1]);
            if (k != 0)
               image2 = (char*)k;

            isDisabled = Utility->GetTagData(MUIA_Disabled, 0, (struct TagItem*)((IPTR *)msg)[1]) ? true : false;
            isSelected = Utility->GetTagData(MUIA_Selected, 0, (struct TagItem*)((IPTR *)msg)[1]) ? true : false;

            openImages();

            return (IPTR)obj;
         }
         break;

      case OM_DISPOSE:
      case MUIM_Hide:
         break;
      
      case MUIM_Show:
         {
            if (dt != 0)
            {
               if (dtimg1 != 0)
               {
                  dt->SetDTAttrsA(dtimg1, 0, 0, (TagItem*)ARRAY(
                     PDTA_Screen,         (IPTR)_screen(obj), 
                     PDTA_DestMode,       PMODE_V43, 
                     PDTA_UseFriendBitMap,true,
                     TAG_DONE,            0));

                  dt->DoDTMethodA(dtimg1, 0, 0, ARRAY(DTM_PROCLAYOUT, 0, 1));
               }
               if (dtimg2 != 0)
               {
                  dt->SetDTAttrsA(dtimg2, 0, 0, (TagItem*)ARRAY(
                     PDTA_Screen,         (IPTR)_screen(obj), 
                     PDTA_DestMode,       PMODE_V43, 
                     PDTA_UseFriendBitMap,true,
                     TAG_DONE,            0));
                  
                  dt->DoDTMethodA(dtimg2, 0, 0, ARRAY(DTM_PROCLAYOUT, 0, 1));
               }
            }
         }
         break;

      case MUIM_AskMinMax:
         {
            DoSuperMtd(parent, obj, msg);

            minmax = (uint16*)(((IPTR *)msg)[1]);
            minmax[0] = width;
            minmax[2] = width;
            minmax[4] = width;
            minmax[1] = height;
            minmax[3] = height;
            minmax[5] = height;
         }
         return 0;

      case OM_SET:
         {
            bool flg;
            bool refresh = false;
//            image1 = (char*)GetTagData(MUIA_Picture_NormalImage,   (int32)image1.Data(), (TagItem*)((IPTR *)msg)[1]);
//            image2 = (char*)GetTagData(MUIA_Picture_SelectedImage, (int32)image2.Data(), (TagItem*)((IPTR *)msg)[1]);
            flg = Utility->GetTagData(MUIA_Disabled, isDisabled, (struct TagItem*)((IPTR *)msg)[1]) ? true : false;
            if (flg != isDisabled)
            {
               isDisabled = flg;
               refresh = true;
            }

            flg = Utility->GetTagData(MUIA_Selected, isSelected, (struct TagItem*)((IPTR *)msg)[1]) ? true : false;
            if (isSelected != flg)
            {
               isSelected = flg;
               refresh = true;
            }

            if (refresh)
               MUIMaster->MUI_Redraw(obj, 0);
         }
         break;

      case MUIM_NoNotifySet:
      case MUIM_Set:
         {
            bool refresh = false;
            if (((IPTR *)msg)[1] == MUIA_Picture_NormalImage)
               image1 = ((char*)msg)[2];

            if (((IPTR *)msg)[1] == MUIA_Picture_SelectedImage)
               image2 = ((char*)msg)[2];

            if (((IPTR *)msg)[1] == MUIA_Disabled)
            {
               bool flg = ((IPTR *)msg)[2] ? true : false;
               if (flg != isDisabled)
               {
                  isDisabled = flg;
                  refresh = true;
               }
            }

            if (((IPTR *)msg)[1] == MUIA_Selected)
            {
               bool flg = ((IPTR *)msg)[2] ? true : false;
               if (flg != isSelected)
               {
                  refresh = true;
                  isSelected = flg;
               }
            }
            if (refresh)
               MUIMaster->MUI_Redraw(obj, 0);
         }
         break;

      case MUIM_Draw:
         {
            Object* o = 0;
            BitMap* bitmap = 0;
            void*   drawinfo = 0;

            if (0 == _rp(obj)) 
               break;

            if ((isSelected) && (dtimg2 != 0))
               o = dtimg2;
            else if (dtimg1 != 0)
               o = dtimg1;
            else
               break;


            drawinfo = dt->ObtainDTDrawInfoA(o, (TagItem*)ARRAY(
               PDTA_Screen,   (IPTR)_screen(obj),
               TAG_DONE,      0));

            if (drawinfo != 0)
            {
               dt->DrawDTObjectA(
                  _rp(obj),
                  o,
                  _mleft(obj),
                  _mtop(obj),
                  width,
                  height,
                  0,
                  0,
                  0);
               dt->ReleaseDTDrawInfo(o, drawinfo);
            }
            else
            {
               dt->GetDTAttrsA(o, (TagItem*)ARRAY(
                        PDTA_DestBitMap,     (IPTR)&bitmap,
                        TAG_DONE,            0));
               if ((0 != bitmap) && (0 != GfxBase))
               {
#ifndef __amigaos4
                  BltBitMapRastPort(bitmap, 0, 0, _rp(obj), _mleft(obj), _mtop(obj), width, height, 0xc0);
#else
                  IGraphics->BltBitMapRastPort(bitmap, 0, 0, _rp(obj), _mleft(obj), _mtop(obj), width, height, 0xc0);
#endif
               }
            }
         }
         return 0;

   }
   return DoSuperMtd(parent, obj, msg);
}

void MUIPictureClass::openImages()
{
   if (dt == 0)
      return;

   closeImages();


   dtimg1 = dt->NewDTObjectA((APTR)image1.Data(), (TagItem*)ARRAY(
            DTA_GroupID,      (IPTR)GID_PICTURE,
            PDTA_Remap,       true,
            OBP_Precision,    (IPTR)PRECISION_EXACT,
            TAG_DONE,         0));

   dtimg2 = dt->NewDTObjectA((APTR)image2.Data(), (TagItem*)ARRAY(
            DTA_GroupID,      (IPTR)GID_PICTURE,
            PDTA_Remap,       true,
            OBP_Precision,    (IPTR)PRECISION_EXACT,
            TAG_DONE,         0));

   width  = 0x7fff;
   height = 0x7fff;

   if (dtimg1 != 0)
   {
      BitMapHeader *bmhd = NULL;

      dt->GetDTAttrsA(dtimg1, (TagItem*)ARRAY(
         PDTA_BitMapHeader,   (IPTR)&bmhd,
         TAG_DONE,            0));

      width  = width  < bmhd->bmh_Width ? width : bmhd->bmh_Width;
      height = height < bmhd->bmh_Height ? height : bmhd->bmh_Height;
   }

   if (dtimg2 != 0)
   {
      BitMapHeader *bmhd = NULL;

      dt->GetDTAttrsA(dtimg1, (TagItem*)ARRAY(
         PDTA_BitMapHeader,   (IPTR)&bmhd,
         TAG_DONE,            0));

      width  = width  < bmhd->bmh_Width ? width : bmhd->bmh_Width;
      height = height < bmhd->bmh_Height ? height : bmhd->bmh_Height;
   }

   if ((width == 0x7fff) && (height == 0x7fff))
   {
      width = 64;
      height = 48;
   }
}

void MUIPictureClass::closeImages()
{
   if (dt != 0)
   {
      if (dtimg1 != 0)
      {
         dt->DisposeDTObject(dtimg1);
      }
      if (dtimg2 != 0)
      {
         dt->DisposeDTObject(dtimg2);
      }
   }
   width    = 64;
   height   = 48;
   dtimg1   = 0;
   dtimg2   = 0;
}
