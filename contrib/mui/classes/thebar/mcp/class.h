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

 $Id: class.h 140 2007-12-26 09:55:26Z marust $

***************************************************************************/

#ifndef _CLASS_H
#define _CLASS_H

#ifdef __AROS__
#define MUIMASTER_YES_INLINE_STDARG
#endif

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#ifndef __AROS__
#include <clib/debug_protos.h>
#endif

#include <libraries/gadtools.h>

#include <string.h>

#include <mui/TheBar_mcc.h>

#include "TheBar_mcp.h"
#include "SDI_compiler.h"

/***********************************************************************/

#define COLORADJBASE 0xF76B1000

#define MUIA_Coloradj_RedComp    COLORADJBASE+0
#define MUIA_Coloradj_GreenComp  COLORADJBASE+1
#define MUIA_Coloradj_BlueComp   COLORADJBASE+2
#define MUIA_Coloradj_Colorfield COLORADJBASE+3

/***********************************************************************/

#define POPBACKTAGBASE TBPTAGBASE+100

#define POPBACKGROUNDSPECSIZE 280

/***********************************************************************/
/*
** Methods
*/

#define MUIM_Popbackground_Open          POPBACKTAGBASE+0
#define MUIM_Popbackground_Close         POPBACKTAGBASE+1
#define MUIM_Popbackground_SetSpec       POPBACKTAGBASE+2
#define MUIM_Popbackground_GetSpec       POPBACKTAGBASE+3
#define MUIM_Popbackground_SelectPattern POPBACKTAGBASE+4
#define MUIM_Popbackground_ShowBitmap    POPBACKTAGBASE+5
#define MUIM_Popbackground_GradientCol   POPBACKTAGBASE+6
#define MUIM_Popbackground_Swap          POPBACKTAGBASE+7
#define MUIM_Popbackground_SetStatus     POPBACKTAGBASE+8
#define MUIM_Popbackground_GetStatus     POPBACKTAGBASE+9

/***********************************************************************/
/*
** Methods structures
*/

struct MUIP_Popbackground_Close
{
    ULONG MethodID;
    ULONG success;
};

struct MUIP_Popbackground_SetSpec
{
    ULONG                       MethodID;
    STRPTR                      spec;
    struct MUIS_TheBar_Gradient *grad;
    ULONG                       flags;
};

enum
{
    MUIV_Popbackground_SetSpec_Image = 1<<0,
};

enum
{
    MUIV_Popbackground_SetSpec_Fail,
    MUIV_Popbackground_SetSpec_Spec,
    MUIV_Popbackground_SetSpec_Grad,
};

struct MUIP_Popbackground_GetSpec
{
    ULONG                       MethodID;
    STRPTR                      spec;
    struct MUIS_TheBar_Gradient *grad;
    ULONG                       flags;
};

enum
{
    MUIV_Popbackground_GetSpec_Image = 1<<0,
};


enum
{
    MUIV_Popbackground_GetSpec_Fail,
    MUIV_Popbackground_GetSpec_Spec,
    MUIV_Popbackground_GetSpec_Grad,
};

struct MUIP_Popbackground_SelectPattern
{
    ULONG MethodID;
    ULONG id;
};

struct MUIP_Popbackground_GradientCol
{
    ULONG MethodID;
    ULONG from;
};

struct MUIP_Popbackground_SetStatus
{
    ULONG                            MethodID;
    struct MUIS_Popbackground_Status *status;
};

struct MUIP_Popbackground_GetStatus
{
    ULONG                            MethodID;
    struct MUIS_Popbackground_Status *status;
};

/***********************************************************************/
/*
** Attributes
*/

#define MUIA_Popbackground_PopObj     POPBACKTAGBASE+0
#define MUIA_Popbackground_BackObj    POPBACKTAGBASE+1
#define MUIA_Popbackground_Gradient   POPBACKTAGBASE+2
#define MUIA_Popbackground_Grad       POPBACKTAGBASE+3
#define MUIA_Popbackground_From       POPBACKTAGBASE+4
#define MUIA_Popbackground_To         POPBACKTAGBASE+5
#define MUIA_Popbackground_File       POPBACKTAGBASE+6
#define MUIA_Popbackground_Horiz      POPBACKTAGBASE+7

/***********************************************************************/
/*
** Structures
*/

struct MUI_PopbackgroundSpec
{
    char buf[POPBACKGROUNDSPECSIZE];
};

struct MUIS_Popbackground_Status
{
    LONG                        pattern;
    LONG                        pen;
    LONG                        colormap;
    ULONG                       rgb;
    char                        bitmap[256];
    struct MUIS_TheBar_Gradient grad;
};

/***********************************************************************/
/*
** MUI macros
*/

#ifdef __AROS__
#define coloradjustObject       BOOPSIOBJMACRO_START(lib_coloradjust->mcc_Class)
#define penadjustObject         BOOPSIOBJMACRO_START(lib_penadjust->mcc_Class)
#define backgroundadjustObject  BOOPSIOBJMACRO_START(lib_backgroundadjust->mcc_Class)
#define poppenObject            BOOPSIOBJMACRO_START(lib_poppen->mcc_Class)
#define popbackObject           BOOPSIOBJMACRO_START(lib_popbackground->mcc_Class)
#else
#define coloradjustObject       NewObject(lib_coloradjust->mcc_Class,NULL
#define penadjustObject         NewObject(lib_penadjust->mcc_Class,NULL
#define backgroundadjustObject  NewObject(lib_backgroundadjust->mcc_Class,NULL
#define poppenObject            NewObject(lib_poppen->mcc_Class,NULL
#define popbackObject           NewObject(lib_popbackground->mcc_Class,NULL
#endif

/***********************************************************************/
/*
** MUI undoc stuff
*/

#ifndef MUIA_Window_MenuGadget
#define MUIA_Window_MenuGadget       0x8042324E
#endif

#ifndef MUIA_Window_SnapshotGadget
#define MUIA_Window_SnapshotGadget   0x80423C55
#endif

#ifndef MUIA_Window_ConfigGadget
#define MUIA_Window_ConfigGadget     0x8042E262
#endif

#ifndef MUIA_Window_IconifyGadget
#define MUIA_Window_IconifyGadget    0x8042BC26
#endif

/***********************************************************************/

#ifndef MUIM_Mccprefs_RegisterGadget
#define MUIM_Mccprefs_RegisterGadget 0x80424828
#endif

#ifndef MUII_PopFont
#define MUII_PopFont 42
#endif

#ifndef MUIV_Imageadjust_Type_All
#define MUIV_Imageadjust_Type_All 0
#endif
#ifndef MUIV_Imageadjust_Type_Image
#define MUIV_Imageadjust_Type_Image 1
#endif
#ifndef MUIV_Imageadjust_Type_Background
#define MUIV_Imageadjust_Type_Background 2
#endif
#ifndef MUIV_Imageadjust_Type_Pen
#define MUIV_Imageadjust_Type_Pen 3
#endif

/***********************************************************************/

#define superget(cl,obj,tag,storage)    DoSuperMethod(cl,obj,OM_GET,tag,(IPTR)(storage))
#define superset(cl,obj,tag,val)        SetSuperAttrs(cl,obj,tag,(IPTR)(val),TAG_DONE)
#define addconfigitem(cfg,value,size,item) DoMethod(cfg,MUIM_Dataspace_Add,(IPTR)(value),size,item)
#define copymem(to,from,len)            memcpy((to), (from), (len))

#define MUIVER20 20

/***********************************************************************/

// xget()
// Gets an attribute value from a MUI object
#ifdef __AROS__
#define xget XGET
#else
ULONG xget(Object *obj, const ULONG attr);
#if defined(__GNUC__)
  // please note that we do not evaluate the return value of GetAttr()
  // as some attributes (e.g. MUIA_Selected) always return FALSE, even
  // when they are supported by the object. But setting b=0 right before
  // the GetAttr() should catch the case when attr doesn't exist at all
  #define xget(OBJ, ATTR) ({ULONG b=0; GetAttr(ATTR, OBJ, &b); b;})
#endif
#endif

/****************************************************************************/

/* utils.c */
#ifdef __MORPHOS__
#elif defined(__AROS__)
Object * DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...);
#else
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...);
ULONG stch_l(char *chr_ptr, ULONG *u_ptr);
#endif

/* utils.c */
#define olabel(id)    Label(tr(id))
#define olabel1(id)   Label1(tr(id))
#define ollabel1(id)  LLabel1(tr(id))
#define olabel2(id)   Label2(tr(id))
#define oflabel(id)   FreeLabel(tr(id))
#define oclabel(id)   CLabel(tr(id))
#define owspace(w)    RectangleObject, MUIA_Weight, (w), End
#define ofhspace(str) RectangleObject, MUIA_FixHeightTxt, (str), End

Object *obutton(const void *text, const void *help);
Object *ocycle(const char **array, const void *key, const void *help);
Object *ocheck(const void *key, const void *help);
Object *oslider(const void * key, const void *help, LONG min, LONG max);
Object *opop(ULONG type, const void *key);
Object *opoppen(const void *key, const void *title, const void *help);
Object *opopfri(const void *key, const void *title, const void *help);
Object *opopback(ULONG gradient, const void *key, const void *title, const void *help);
Object *opopframe(const void *key, const void *title, const void *help);
void drawGradient(Object *obj, struct MUIS_TheBar_Gradient *grad);

#if !defined(__MORPHOS__) && !defined(__amigaos4__)
/* coloradjust.c */
void freeColoradjust ( void );
ULONG initColoradjust ( void );

/* penadjust.c */
void freePenadjust ( void );
ULONG initPenadjust ( void );

/* backgroundadjust.c */
void freeBackgroundadjust ( void );
ULONG initBackgroundadjust ( void );

/* poppen.c */
void freePoppen ( void );
ULONG initPoppen ( void );

/* popbackground.c */
void freePopbackground ( void );
ULONG initPopbackground ( void );
#endif

/***********************************************************************/

#endif /* _CLASS_H */
