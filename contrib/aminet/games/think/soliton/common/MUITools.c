/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string.h>                 /* strchr */

#include <clib/alib_protos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "Locales.h"
#include "MUITools.h"

long GetControlChar(char* label)
{
  if (label) 
  {
    char* c = strchr(label, '_');
    if(c)
      return ToLower(*(c+1));
  }
  return 0;
}

/****************************************************************************************
  MUI-Hilfstools
****************************************************************************************/

LONG xget(Object* obj, ULONG attribute)
{ 
  LONG x = 0;
  getatt(obj, attribute, &x);
  return x;
}

/****************************************************************************************
  MUI-Creation"makros"
****************************************************************************************/

Object* MakeButton(int num, int help)
{
  Object* obj = MUI_MakeObject(MUIO_Button, GetStr(num));
  if (obj) 
  {
    setatt(obj, MUIA_CycleChain, 1);
    if (help) setatt(obj, MUIA_ShortHelp, GetStr(help));
  }
  return obj;
}

Object *MakeString(int maxlen, int num, char* contents, int help)
{
  char* titel;
  Object *obj;

  titel = num ? GetStr(num) : NULL;

  if((obj = MUI_MakeObject(MUIO_String, titel, maxlen)))
  {
    setatt(obj, MUIA_CycleChain, 1);
    setatt(obj, MUIA_String_Contents, contents);
    if(help)
      setatt(obj, MUIA_ShortHelp, GetStr(help));
  }
  return obj;
}

Object *MakeCheck(int num, BOOL pressed, int help)
{
  Object *obj = MUI_MakeObject(MUIO_Checkmark, GetStr(num));
  if (obj) 
  {
    setatt(obj, MUIA_CycleChain, 1           );
    setatt(obj, MUIA_Selected  , pressed     );
    setatt(obj, MUIA_ShortHelp , GetStr(help));
  }
  return(obj);
}

Object* MakeSlider(int min, int max, int val, int text, int help)
{
  Object *obj = MUI_MakeObject(MUIO_Slider, GetStr(text), min, max);
  if (obj) 
  {
    setatt(obj, MUIA_CycleChain  , 1);
    setatt(obj, MUIA_Numeric_Value, val);
    setatt(obj, MUIA_ShortHelp   , GetStr(help));
  }
  return(obj);
}

Object *MakeImage(const UBYTE* data, LONG w, LONG h, LONG d, 
                 LONG compr, LONG mask, const ULONG* colors)
{
  return BodychunkObject,
    MUIA_FixWidth             , w,
    MUIA_FixHeight            , h,
    MUIA_Bitmap_Width         , w,
    MUIA_Bitmap_Height        , h,
    MUIA_Bodychunk_Depth      , d,
    MUIA_Bodychunk_Body       , data,
    MUIA_Bodychunk_Compression, compr,
    MUIA_Bodychunk_Masking    , mask,
    MUIA_Bitmap_SourceColors  , colors,
    MUIA_Bitmap_Transparent   , 0,
    MUIA_ShowSelState  , FALSE,
    End;
}

Object *MakeLabel1    (int num) {return MUI_MakeObject(MUIO_Label, GetStr(num), MUIO_Label_SingleFrame);}
Object *MakeLabel2    (int num) {return MUI_MakeObject(MUIO_Label, GetStr(num), MUIO_Label_DoubleFrame);}

Object* MenuObj(Object* strip, int data)
{
  return (Object*)DoMethod(strip, MUIM_FindUData, data);
}

void ErrorReq(int messagenum)
{
  struct EasyStruct rs;
  
  rs.es_StructSize = sizeof(struct EasyStruct);
  rs.es_Flags = 0;
  rs.es_Title = GetStr(MSG_ERROR_REQUEST);
  rs.es_TextFormat = GetStr(messagenum);
  rs.es_GadgetFormat = GetStr(MSG_ERROR_OK);

  EasyRequest(NULL, &rs, NULL);
}

