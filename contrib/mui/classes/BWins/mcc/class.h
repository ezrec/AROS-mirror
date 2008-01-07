#ifndef _CLASS_H
#define _CLASS_H

#define __USE_SYSBASE
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/locale.h>
#ifdef __AROS__
#include <proto/popupmenu.h>
#else
#include <proto/pm.h>
#endif

#include <clib/alib_protos.h>

#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/cghooks.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/icclass.h>

#include <libraries/gadtools.h>

#ifndef __AROS__
#include <clib/debug_protos.h>
#include <mui/muiundoc.h>
#include <dos.h>
#endif

#include <string.h>

#include <BWin_private_mcc.h>

#include "base.h"

#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

extern char libName[];
extern char author[];

#define CATNAME "BWin.catalog"

/***********************************************************************/
/*
** MCCs macros
*/

#ifdef __AROS__
#define ContentsObject BOOPSIOBJMACRO_START(lib_contents->mcc_Class)
#define SysGadObject   BOOPSIOBJMACRO_START(lib_boopsi->mcc_Class)
#else
#define ContentsObject NewObject(lib_contents->mcc_Class,NULL
#define SysGadObject   NewObject(lib_boopsi->mcc_Class,NULL
#endif

/***********************************************************************/
/*
** MUI macros
*/

#ifndef nfset
#define nfset(obj,attr,value) SetAttrs(obj,MUIA_Group_Forward,FALSE,attr,value,TAG_DONE)
#endif

/* This is a MUI hack! */
#define _backspec(obj) ((APTR)(*((ULONG *)(((char *)(obj))+80))))

/***********************************************************************/
/*
** PMenus macros
*/

/* Menus */
#define MTITLE(t,d)         {NM_TITLE,(STRPTR)(t),0,0,0,(APTR)(d)}
#define MITEM(t,d)          {NM_ITEM,(STRPTR)(t),0,0,0,(APTR)(d)}
#define MTITEM(t,d)         {NM_ITEM,(STRPTR)(t),0,CHECKIT|MENUTOGGLE,0,(APTR)(d)}
#define MEITEM(t,d,e)       {NM_ITEM,(STRPTR)(t),0,CHECKIT|MENUTOGGLE,(LONG)(e),(APTR)(d)}
#define MBAR(d)             {NM_ITEM,(STRPTR)NM_BARLABEL,0,0,0,(APTR)(d)}
#define MSUBITEM(t,d)       {NM_SUB,(STRPTR)(t),0,0,0,(APTR)(d)}
#define MESUBITEM(t,d,e)    {NM_SUB,(STRPTR)(t),0,CHECKIT|MENUTOGGLE,(LONG)(e),(APTR)(d)}
#define MSUBBAR             {NM_SUB,(STRPTR)NM_BARLABEL,0,0,0,NULL}
#define MEND                {NM_END,NULL,0,0,0,NULL}

#define pmget(pm,attr,value) PM_GetItemAttrs(((struct PopupMenu *)(pm)),(ULONG)(attr),(ULONG *)(value),TAG_DONE)
#define pmset(pm,attr,value) PM_SetItemAttrs(((struct PopupMenu *)(pm)),(ULONG)(attr),(ULONG)(value),TAG_DONE)
#define PMIDItem(t,id)       PMItem(t), PM_ID, id
#define PMDBar(id)           PMBar, PM_ID, id, PMEnd

/***********************************************************************/

#include "class_protos.h"

/***********************************************************************/

#endif /* _CLASS_H */
