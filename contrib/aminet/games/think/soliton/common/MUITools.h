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
#include <mui.h>
#else
#include <libraries/mui.h>
#endif

/****************************************************************************************
  MUI
****************************************************************************************/

#include "SDI_compiler.h"
#define HOOKPROTO(name, ret, obj, param) static SAVEDS ASM(ret) name(REG(a0, struct Hook *hook), REG(a2, obj), REG(a1, param))
#define HOOKPROTONH(name, ret, obj, param) static SAVEDS ASM(ret) name(REG(a2, obj), REG(a1, param))
#define HOOKPROTONHNO(name, ret, param) static SAVEDS ASM(ret) name(REG(a1, param))
#define DISPATCHERPROTO(name) static ASM(ULONG) SAVEDS name(REG(a0, struct IClass * cl), REG(a2, Object * obj), REG(a1, Msg msg))

#define TAGBASE_KAI (TAG_USER | (0617 << 16))

/****************************************************************************************
  MUI-help functions
****************************************************************************************/

#define getatt(obj,attr,store) GetAttr(attr,obj,(ULONG *)store)
#define setatt(obj,attr,value) SetAttrs(obj,attr,value,TAG_DONE)

LONG xget(Object* obj, ULONG attribute);
ULONG DoSuperNew(struct IClass* cl, Object* obj, ULONG tag1, ...);
void ErrorReq(int messagenum);

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
