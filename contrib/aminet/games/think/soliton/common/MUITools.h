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

#ifndef INCLUDE_MUITOOLS_H
#define INCLUDE_MUITOOLS_H

#ifdef USE_ZUNE
#include <libraries/mui.h>
#else
#include <libraries/mui.h>
#endif

/****************************************************************************************
  MUI
****************************************************************************************/

#include <SDI/SDI_compiler.h>
#include <SDI/SDI_hook.h>

#define TAGBASE_KAI (TAG_USER | (0617 << 16))

/****************************************************************************************
  MUI-help functions
****************************************************************************************/

#define getatt(obj,attr,store) GetAttr(attr,obj,store)
#define setatt(obj,attr,value) SetAttrs(obj,attr,value,TAG_DONE)

IPTR xget(Object* obj, ULONG attribute);
void ErrorReq(int messagenum);

#if !defined(__MORPHOS__)
#ifdef __AROS__
static inline Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, Tag tag1, ...)
{
    AROS_SLOWSTACKTAGS_PRE_AS(tag1, Object *)
    retval = (Object *)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1), NULL);
    AROS_SLOWSTACKTAGS_POST
}
#else
static inline Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  VA_LIST args;

  ENTER();

  VA_START(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
  VA_END(args);

  RETURN(rc);
  return rc;
}
#endif
#endif // !__MORPHOS__


/****************************************************************************************
  MUI-creation"makros"
****************************************************************************************/

long GetControlChar(char* label);

#define HBarObject   MUI_MakeObject(MUIO_HBar, 4)
#define VBarObject   MUI_MakeObject(MUIO_VBar, 8)

Object* MakeButton     (int num, int help);
Object* MakeString     (int maxlen, int num, char* contents, int help);
Object* MakeCheck      (int num, BOOL pressed, int help);
Object* MakeSlider     (int min, int max, int val, int text, int help);
Object* MakeImage      (const UBYTE* data, LONG w, LONG h, LONG d, 
                        LONG compr, LONG mask, const ULONG* colors);
Object* MakeLabel1     (int num);
Object* MakeLabel2     (int num);

Object* MenuObj(Object* strip, int data);

#endif
