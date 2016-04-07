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

#ifndef _CLASS_H
#define _CLASS_H

/***********************************************************************/
/*
** Includes
*/

#define __NOLIBBASE__
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
#include <mui/TheBar_mcc.h>
#include <mui/TheBar_mcp.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDI_compiler.h>
#include <SDI_stdarg.h>
#include <SDI_hook.h>

#include "debug.h"
#define CATCOMP_NUMBERS
#include "locale.h"

/***********************************************************************/
/*
** Libraries shared stuff
*/

#if defined(__amigaos4__)
extern struct Library         *SysBase;
extern struct Library         *DOSBase;
extern struct Library         *GfxBase;
extern struct Library         *IntuitionBase;
#else
extern struct ExecBase        *SysBase;
extern struct DosLibrary      *DOSBase;
extern struct IntuitionBase   *IntuitionBase;
extern struct GfxBase         *GfxBase;
#endif
#if defined(__AROS__)
extern struct UtilityBase     *UtilityBase;
#else
extern struct Library         *UtilityBase;
#endif
extern struct Library         *MUIMasterBase;

extern struct Library         *DataTypesBase;
extern struct Library         *CyberGfxBase;
extern struct Library         *IFFParseBase;
#if !defined(__amigaos4__) && !defined(__MORPHOS__)
extern struct LocaleBase      *LocaleBase;
#else
extern struct Library         *LocaleBase;
#endif

extern struct MUI_CustomClass *ThisClass;
#if !defined(__MORPHOS__) && !defined(__amigaos4__)
extern struct MUI_CustomClass *lib_coloradjust;
extern struct MUI_CustomClass *lib_penadjust;
extern struct MUI_CustomClass *lib_backgroundadjust;
extern struct MUI_CustomClass *lib_poppen;
extern struct MUI_CustomClass *lib_popbackground;
#endif

extern ULONG                  lib_flags;

/* lib_flags */
enum
{
  BASEFLG_Init    = 1<<0,
  BASEFLG_MUI20   = 1<<1,
  BASEFLG_MUI4    = 1<<2,
  BASEFLG_MORPHOS = 1<<3,
};

/*
** Class string to localize at init
** Should be in class.c, but mccinit
** doesn't permit that.
*/
extern STRPTR regs[], frames[], precisions[], dismodes[],
              spacersSizes[], viewModes[], labelPoss[];

/***************************************************************************/
/*
** Coloradjust class
*/

/* Tag base */
#define COLORADJBASE 0xF76B1000UL

/* Attributes */
#define MUIA_Coloradj_RedComp    COLORADJBASE+0
#define MUIA_Coloradj_GreenComp  COLORADJBASE+1
#define MUIA_Coloradj_BlueComp   COLORADJBASE+2
#define MUIA_Coloradj_Colorfield COLORADJBASE+3

/***********************************************************************/
/*
** Popbackground class
*/

/* Tag base */
#define POPBACKTAGBASE TBPTAGBASE+100

#define POPBACKGROUNDSPECSIZE 280

/* Methods */
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

/* Methods Structs */

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

/* Attributes */
#define MUIA_Popbackground_PopObj     POPBACKTAGBASE+0
#define MUIA_Popbackground_BackObj    POPBACKTAGBASE+1
#define MUIA_Popbackground_Gradient   POPBACKTAGBASE+2
#define MUIA_Popbackground_Grad       POPBACKTAGBASE+3
#define MUIA_Popbackground_From       POPBACKTAGBASE+4
#define MUIA_Popbackground_To         POPBACKTAGBASE+5
#define MUIA_Popbackground_File       POPBACKTAGBASE+6
#define MUIA_Popbackground_Horiz      POPBACKTAGBASE+7

/* Structs */

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
** Macros
*/

#define coloradjustObject       NewObject(lib_coloradjust->mcc_Class,NULL
#define penadjustObject         NewObject(lib_penadjust->mcc_Class,NULL
#define backgroundadjustObject  NewObject(lib_backgroundadjust->mcc_Class,NULL
#define poppenObject            NewObject(lib_poppen->mcc_Class,NULL
#define popbackObject           NewObject(lib_popbackground->mcc_Class,NULL

#define superget(cl,obj,tag,storage)    DoSuperMethod(cl,obj,OM_GET,tag,(IPTR)(storage))
#if !defined(superset)
#define superset(cl,obj,tag,val)        SetSuperAttrs(cl,obj,tag,(IPTR)(val),TAG_DONE)
#endif
#define addconfigitem(cfg,value,size,item) DoMethod(cfg,MUIM_Dataspace_Add,(IPTR)(value),size,item)

#undef get
#define get(obj,attr,store)            GetAttr((ULONG)(attr),(APTR)obj,(IPTR *)((IPTR)(store)))
#undef set
#define set(obj,attr,value)            SetAttrs((Object *)(obj),(ULONG)(attr),(IPTR)(value),TAG_DONE)
#undef nnset
#define nnset(obj,attr,value)          SetAttrs((Object *)(obj),MUIA_NoNotify,TRUE,(ULONG)(attr),(IPTR)(value),TAG_DONE)

// xget()
// Gets an attribute value from a MUI object
IPTR xget(Object *obj, const IPTR attr);
#if defined(__GNUC__)
  // please note that we do not evaluate the return value of GetAttr()
  // as some attributes (e.g. MUIA_Selected) always return FALSE, even
  // when they are supported by the object. But setting b=0 right before
  // the GetAttr() should catch the case when attr doesn't exist at all
  #define xget(OBJ, ATTR) ({IPTR b=0; GetAttr(ATTR, OBJ, &b); b;})
#endif

#define olabel(id)    Label(tr(id))
#define olabel1(id)   Label1(tr(id))
#define ollabel1(id)  LLabel1(tr(id))
#define olabel2(id)   Label2(tr(id))
#define oflabel(id)   FreeLabel(tr(id))
#define oclabel(id)   CLabel(tr(id))
#define owspace(w)    RectangleObject, MUIA_Weight, (w), End
#define ofhspace(str) RectangleObject, MUIA_FixHeightTxt, (str), End

#ifdef __MORPHOS__
#undef NewObject
#undef MUI_NewObject
#undef DoSuperNew
APTR NewObject(struct IClass *classPtr,CONST_STRPTR classID,ULONG tag1,...);
Object *MUI_NewObject(CONST_STRPTR classname,Tag tag1,...);
#endif

/***********************************************************************/
/*
** MUI undoc stuff
*/

#ifndef MUIA_Text_Copy
#define MUIA_Text_Copy               0x80427727UL
#endif

#ifndef MUIA_Window_MenuGadget
#define MUIA_Window_MenuGadget       0x8042324EUL
#endif

#ifndef MUIA_Window_SnapshotGadget
#define MUIA_Window_SnapshotGadget   0x80423C55UL
#endif

#ifndef MUIA_Window_ConfigGadget
#define MUIA_Window_ConfigGadget     0x8042E262UL
#endif

#ifndef MUIA_Window_IconifyGadget
#define MUIA_Window_IconifyGadget    0x8042BC26UL
#endif


#ifndef MUIA_Imagedisplay_Spec
#define MUIA_Imagedisplay_Spec 0x8042a547UL
#endif

#ifndef MUIA_Imageadjust_Type
#define MUIA_Imageadjust_Type 0x80422f2bUL
#endif

#ifndef MUIA_Framedisplay_Spec
#define MUIA_Framedisplay_Spec 0x80421794UL
#endif

#ifndef MUIM_Mccprefs_RegisterGadget
#define MUIM_Mccprefs_RegisterGadget 0x80424828UL
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

#ifndef MUIC_Crawling
#define MUIC_Crawling "Crawling.mcc"
#endif

#ifndef CrawlingObject
#define CrawlingObject MUI_NewObject(MUIC_Crawling
#endif

#ifndef MUIM_CreateDragImage
#define MUIM_CreateDragImage 0x8042eb6fUL /* V18 */ /* Custom Class */
struct  MUIP_CreateDragImage { STACKED ULONG MethodID; STACKED LONG touchx; STACKED LONG touchy; STACKED ULONG flags; }; /* Custom Class */

struct MUI_DragImage
{
    struct BitMap *bm;
    WORD width;
    WORD height;
    WORD touchx;
    WORD touchy;
    ULONG flags;
};

#endif

#ifndef MUIM_DeleteDragImage
#define MUIM_DeleteDragImage 0x80423037UL
struct MUIP_DeleteDragImage {STACKED ULONG MethodID; STACKED struct MUI_DragImage *di;};
#endif

/***********************************************************************/

#include "class_protos.h"

/***********************************************************************/

#endif /* _CLASS_H */
