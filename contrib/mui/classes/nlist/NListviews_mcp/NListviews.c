/***************************************************************************

 NListviews.mcp - New Listview MUI Custom Class Preferences
 Registered MUI class, Serial Number: 1d51

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

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

#include <stdarg.h>
#include <string.h>

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <dos/dosextens.h>
#include <proto/exec.h>
#include <libraries/commodities.h>
#include <libraries/locale.h>
#include <devices/inputevent.h>
#include <libraries/asl.h>
#include <libraries/mui.h>

#ifndef __AROS__
#include <mui/HotkeyString_mcc.h>
#else
#warning "No MUIC_HotkeyString class yet in AROS!"
#define MUIC_HotkeyString   	MUIC_String
#define MUIA_HotkeyString_Snoop TAG_IGNORE
#endif

#include <proto/locale.h>
#include <proto/console.h>
#include <proto/commodities.h>
#include <proto/intuition.h>
#include <clib/alib_protos.h>

#include <proto/muimaster.h>

#include "private.h"
#include "rev.h"
#include "mcc_common.h"

#include "MUI/NListview_mcc.h"

#ifdef __AROS__
extern struct Device *ConsoleDevice;
#endif

#define INTUIBASEMIN 39


#define SimpleButtonCycle(name,helpstring) \
  (void *)MUI_NewObject(MUIC_Text, \
    ButtonFrame, \
    MUIA_CycleChain, 1, \
    MUIA_Font, MUIV_Font_Button, \
    MUIA_Text_Contents, name, \
    MUIA_Text_PreParse, "\33c", \
    MUIA_InputMode    , MUIV_InputMode_RelVerify, \
    MUIA_Background   , MUII_ButtonBack, \
    MUIA_ShortHelp, helpstring, \
  TAG_DONE)


#define ToggleButtonCycle(name,selected,disable,helpstring) \
  (void *)MUI_NewObject(MUIC_Text, \
    ButtonFrame, \
    MUIA_CycleChain, 1, \
    MUIA_Font, MUIV_Font_Button, \
    MUIA_Text_Contents, name, \
    MUIA_Text_PreParse, "\33c", \
    MUIA_Text_SetMax, TRUE, \
    MUIA_InputMode    , MUIV_InputMode_Toggle, \
    MUIA_Selected     , selected,\
    MUIA_Background   , MUII_ButtonBack, \
    MUIA_Disabled     , disable,\
    MUIA_ShortHelp, helpstring, \
  TAG_DONE)
/*
    MUIA_ShowSelState , FALSE,\
*/
struct QualifierDef {
  char *qualstr;
  LONG qualval;
};

/*
static struct QualifierDef QualTab[] =
{
  { "SHIFT",     (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT) },
  { "LSHIFT",     IEQUALIFIER_LSHIFT },
  { "RSHIFT",     IEQUALIFIER_RSHIFT },
  { "CONTROL",    IEQUALIFIER_CONTROL },
  { "ALT",       (IEQUALIFIER_LALT|IEQUALIFIER_RALT) },
  { "LALT",       IEQUALIFIER_LALT },
  { "RALT",       IEQUALIFIER_RALT },
  { "COMMAND",   (IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND) },
  { "LCOMMAND",   IEQUALIFIER_LCOMMAND },
  { "RCOMMAND",   IEQUALIFIER_RCOMMAND },
  { "MIDBUTTON",  IEQUALIFIER_MIDBUTTON },
  { "RBUTTON",    IEQUALIFIER_RBUTTON },
  { "LEFTBUTTON", IEQUALIFIER_LEFTBUTTON },
  {  NULL,        0 }
};
*/

#define String2(contents,maxlen)\
  (void *)MUI_NewObject(MUIC_String,\
    StringFrame,\
    MUIA_CycleChain, 1,\
    MUIA_String_MaxLen  , maxlen,\
    MUIA_String_Contents, contents,\
    TAG_DONE)

#define LOAD_DATALONG(obj,attr,cfg_attr,defaultval) \
  { \
    LONG *ptrd; \
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, cfg_attr))) \
      set(obj, attr, *ptrd); \
    else \
      set(obj, attr, defaultval); \
  }

#define SAVE_DATALONG(obj,attr,cfg_attr) \
  { \
    LONG ptrd; \
    get(obj, attr, &ptrd); \
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &ptrd, 8, cfg_attr); \
  }

#define LOAD_DATASPEC(obj,attr,cfg_attr,defaultval) \
  { \
    LONG ptrd; \
    if((ptrd = DoMethod(msg->configdata, MUIM_Dataspace_Find, cfg_attr))) \
      set(obj, attr, ptrd); \
    else \
      set(obj, attr, defaultval); \
  }

#define SAVE_DATASPEC(obj,attr,cfg_attr) \
  { \
    LONG ptrd; \
    get(obj, attr, &ptrd); \
    if (ptrd) \
      DoMethod(msg->configdata, MUIM_Dataspace_Add, ptrd, sizeof(struct MUI_PenSpec), cfg_attr); \
  }

#define SAVE_DATASPEC2(obj,attr,cfg_attr) \
  { \
    LONG ptrd; \
    get(obj, attr, &ptrd); \
    if (ptrd) \
      DoMethod(msg->configdata, MUIM_Dataspace_Add, ptrd, sizeof(struct MUI_ImageSpec), cfg_attr); \
  }

#define LOAD_DATAFONT(obj,cfg_attr) \
  { \
    LONG ptrd; \
    if((ptrd = DoMethod(msg->configdata, MUIM_Dataspace_Find, cfg_attr))) \
      set(obj, MUIA_String_Contents, ptrd); \
    else \
      set(obj, MUIA_String_Contents, ""); \
  }

#define SAVE_DATAFONT(obj,cfg_attr) \
  { \
    LONG ptrd; \
    get(obj, MUIA_String_Contents, &ptrd); \
    if (ptrd) \
    { \
      LONG dlen = strlen((char *) ptrd) + 1; \
      if (dlen > 2) \
        DoMethod(msg->configdata, MUIM_Dataspace_Add, ptrd, dlen, cfg_attr); \
      else \
        DoMethod(msg->configdata, MUIM_Dataspace_Remove, cfg_attr); \
    } \
  }

/* *********************************************************************** */

static DEFAULT_KEYS_ARRAY

static char *functions_names[] =
{
  "Multiselect/Block Qualifier",
  "Drag Qualifier",
  "Balance Qualifier",
  "Copy to clipboard",
  "Default width column",
  "Default width all columns",
  "Default order column",
  "Default order all columns",
  "Select to top",
  "Select to bottom",
  "Select to page up",
  "Select to page down",
  "Select up",
  "Select down",
  "Toggle active",
  "Fast MouseWheel Qualifier",
  "Horiz.MouseWheel Qualifier",
  "Qualified Title Click Qualifier",
  "",NULL
};

static ULONG keytags[] =
{
  KEYTAG_QUALIFIER_MULTISELECT    ,
  KEYTAG_QUALIFIER_DRAG           ,
  KEYTAG_QUALIFIER_BALANCE        ,
  KEYTAG_COPY_TO_CLIPBOARD        ,
  KEYTAG_DEFAULT_WIDTH_COLUMN     ,
  KEYTAG_DEFAULT_WIDTH_ALL_COLUMNS,
  KEYTAG_DEFAULT_ORDER_COLUMN     ,
  KEYTAG_DEFAULT_ORDER_ALL_COLUMNS,
  KEYTAG_SELECT_TO_TOP            ,
  KEYTAG_SELECT_TO_BOTTOM         ,
  KEYTAG_SELECT_TO_PAGE_UP        ,
  KEYTAG_SELECT_TO_PAGE_DOWN      ,
  KEYTAG_SELECT_UP                ,
  KEYTAG_SELECT_DOWN              ,
  KEYTAG_TOGGLE_ACTIVE            ,
  KEYTAG_QUALIFIER_WHEEL_FAST     ,
  KEYTAG_QUALIFIER_WHEEL_HORIZ    ,
  KEYTAG_QUALIFIER_TITLECLICK2    ,
  (ULONG)NULL
};



static struct KeyBinding empty_key = { 0, (UWORD)-1, 0 };

/* *********************************************************************** */

static char *MainTextArray[] =
{
  "If you have problems, try to increase the stack value,",
  "in the icon infos if you launch the program from icon,",
  "stack of CLI if you start it from CLI/Shell,",
  "and if you launch it from some utility read its docs",
  "to see how to increase it.",
  "A stack of 12Kb, 16Kb or 20Kb is most of time a good idea.",
  " ",
  "If you still have problems, try to see if they happen with the Demo prog, then tell me.",
  "\033C",
  "Latest public release can be found on aminet (dev/mui).",
  "All releases are available on",
  "http://iutsoph.unice.fr/amiga/mui/MCC_NListxxx.lha",
  " and",
  "ftp://iutsoph.unice.fr/pub/amiga/mui/MCC_NListxxx.lha",
  "\033C",
  "\033r\0333(C) Gilles Masson, 1996-1998",
  "\033r\0333masson@iutsoph.unice.fr",
  "\033C",
  "\033cThis new list/listview custom class",
  "\033chandles its own configurable backgrounds,",
  "\033cpens, fonts, scrollbars, keys and qualifiers",
  "\033C",
  "\033cThe way used to handle cursor with multiselection",
  "\033cis not exactly as the listview one !",
  "\033cDrag&Drop is supported, both scrollbars",
  "\033ccan be configured to disappear automatically",
  "\033cwhen not needed because all is visible.",
  "\033cJust try it...",
  "\033C",
  "\033cYou can horizontaly scroll with cursor keys,",
  "\033cor going on the right and left of the list",
  "\033cwhile selecting with the mouse.",
  "\033cTry just clicking on the left/right borders !",
  "\033C",
  "\033cYou can change columns widths just dragging",
  "\033cthe vertical separator between columns titles",
  "\033c(try it using the balancing qualifier too).",
  "\033C",
  "\033cYou can change columns order just dragging",
  "\033cthe column titles.",
  "\033C",
  "\033cThere is builtin copy to clipboard stuff",
  "\033cand classic char selection capability.",
  "\033C",
  "\033cTry the Demo program to test all that...",
  " ",
  " ",
  "\033r\033bGive some feedback about it ! :-)",
  "\033C",
  "**************************************************************************************************",
  NULL
};


static char *Pages[] =
{
  "Fonts",
  "Colors",
  "Misc",
  "ScrollBars",
  "Keys",
  NULL
};

static char *Pages3[] =
{
  "Misc1",
  "Misc2",
  "Misc3",
  "Misc4",
  "Misc5",
  "Misc6",
  NULL
};

static char *RS_MultiSelects[] =
{
  "Qualifier",
  "Always",
  NULL
};

static char *RS_StackCheck[] =
{
  "< 1Kb free",
  "< 2Kb free",
  "< 3Kb free",
  NULL
};

static char *RS_ColWidthDrag[] =
{
  "Top Bar",
  "Full Bar",
  "Visible",
  NULL
};

static char *RS_DragTypes[] =
{
  "Immediate",
  "Borders",
  "Qualifier",
  NULL
};

static char *RS_VSB[] =
{
  "Always",
  "Auto",
  "FullAuto",
  NULL
};

static char *RS_HSB[] =
{
  "Always",
  "Auto",
  "FullAuto",
  "None",
  NULL
};

static char *RS_Menu[] =
{
  "Always",
  "TopOnly",
  "Never",
  NULL
};


enum StringsID
{
    MSG_FONTS,
    MSG_COLORS,
    MSG_PENS,
    MSG_BACKGROUNDS,
    MSG_TITLE_PEN_WIN,
    MSG_TITLE_PEN_HELP,
    MSG_PBG_TITLE,
    MSG_TITLE_BG_WIN,
    MSG_TITLE_BG_HELP,
    MSG_LIST_PEN_WIN,
    MSG_LIST_PEN_HELP,
    MSG_PBG_LIST,
    MSG_LIST_BG_WIN,
    MSG_LIST_BG_HELP,
    MSG_SELECT_PEN_WIN,
    MSG_SELECT_PEN_HELP,
    MSG_PBG_SELECT,
    MSG_SELECT_BG_WIN,
    MSG_SELECT_BG_HELP,
    MSG_CURSOR_PEN_WIN,
    MSG_CURSOR_PEN_HELP,
    MSG_PBG_CURSOR,
    MSG_CURSOR_BG_WIN,
    MSG_CURSOR_BG_HELP,
    MSG_UNSEL_PEN_WIN,
    MSG_UNSEL_PEN_HELP,
    MSG_PBG_UNSEL,
    MSG_UNSEL_BG_WIN,
    MSG_UNSEL_BG_HELP,
    MSG_NORMAL_FONT,
    MSG_NORMAL_FONT_HELP,
    MSG_NORMAL_FONT_ASL,
    MSG_LITTLE_FONT,
    MSG_LITTLE_FONT_HELP,
    MSG_LITTLE_FONT_ASL,
    MSG_FIXED_FONT,
    MSG_FIXED_FONT_HELP,
    MSG_FIXED_FONT_ASL,
    MSG_MULTISELECT,
    MSG_MULTISELECT_HELP,
    MSG_MMB_MULTISEL,
    MSG_MMB_MULTISEL_HELP,
    MSG_LEADING,
    MSG_LEADING_HELP,
    MSG_DRAGTYPE,
    MSG_DRAGTYPE_HELP,
    MSG_DRAG_QUALIFIER_HELP,
    MSG_DEFAULT_CONTEXT_MENU,
    MSG_DEFAULT_CONTEXT_MENU_HELP,
    MSG_PARTIAL_COL_MARK,
    MSG_PARTIAL_COL_MARK_HELP,
    MSG_PARTIAL_CHARS_DRAWN,
    MSG_PARTIAL_CHARS_DRAWN_HELP,
    MSG_BALANCING_COLS,
    MSG_BALANCING_COLS_HELP,
    MSG_FORCE_SELECT_PEN,
    MSG_FORCE_SELECT_PEN_HELP,
    MSG_LIST_LIKE_MULTISEL,
    MSG_LIST_LIKE_MULTISEL_HELP,
    MSG_STACK_WARNING,
    MSG_STACK_WARNING_HELP,
    MSG_SB_HORIZONTAL,
    MSG_SB_HORIZONTAL_HELP,
    MSG_SB_VERTICAL,
    MSG_SB_VERTICAL_HELP,
    MSG_SMOOTH_SCROLL,
    MSG_SMOOTH_SCROLLING,
    MSG_SMOOTH_SCROLLING_HELP,
    MSG_SPECIAL_POINTER_COLORS,
    MSG_SPECIAL_POINTER_COLORS_HELP,
    MSG_SERMOUSE_FIX,
    MSG_SERMOUSE_FIX_HELP,
    MSG_WHEEL_STEP,
    MSG_WHEEL_STEP_HELP,
    MSG_WHEEL_FAST,
    MSG_WHEEL_FAST_HELP,
    MSG_MMB_FASTWHEEL,
    MSG_MMB_FASTWHEEL_HELP,
    MSG_DRAG_LINES,
    MSG_DRAG_LINES_HELP,
    MSG_SNOOP_KEY,
    MSG_INSERT_KEY,
    MSG_REMOVE_KEY,
    MSG_UPDATE_KEYS,
    MSG_DEFAULT_KEYS
};


/*
static STRPTR STRING(enum StringsID id,STRPTR defstr)
{
  if (LocaleBase)
    return(GetCatalogStr(catalog,id,defstr));
  return(defstr);
}
*/

#define STRING(a,b) b


#if !defined(__SASC) && !defined(__AROS__)
static char *stpcpy(char *to,char *from)
{
  register char *to2 = to;

  while (*from)
    *to2++ = *from++;
  *to2 = '\0';
  return (to2);
}
#endif

static LONG DeadKeyConvert(struct NListviews_MCP_Data *data,struct KeyBinding *key)
{
  char *text = data->rawtext;
  UWORD qual = key->kb_Qualifier & KBQUAL_MASK;
  UWORD same = key->kb_Qualifier & KBSYM_MASK;
  int posraw;
  data->rawtext[0] = '\0';

  if (qual)
  { if ((qual & KBQUALIFIER_CAPS) && (same & KBSYM_CAPS))
                                          text = stpcpy(text,"caps ");
    else if ((qual & KBQUALIFIER_SHIFT) && (same & KBSYM_SHIFT))
    {                                     text = stpcpy(text,"shift ");
      if (qual & IEQUALIFIER_CAPSLOCK)    text = stpcpy(text,"capslock ");
    }
    else
    { if (qual & IEQUALIFIER_LSHIFT)      text = stpcpy(text,"lshift ");
      if (qual & IEQUALIFIER_RSHIFT)      text = stpcpy(text,"rshift ");
      if (qual & IEQUALIFIER_CAPSLOCK)    text = stpcpy(text,"capslock ");
    }
    if (qual & IEQUALIFIER_CONTROL)       text = stpcpy(text,"control ");
    if ((qual & KBQUALIFIER_ALT) && (same & KBSYM_ALT))
                                          text = stpcpy(text,"alt ");
    else
    { if (qual & IEQUALIFIER_LALT)        text = stpcpy(text,"lalt ");
      if (qual & IEQUALIFIER_RALT)        text = stpcpy(text,"ralt ");
    }
    if (qual & IEQUALIFIER_LCOMMAND)      text = stpcpy(text,"lcommand ");
    if (qual & IEQUALIFIER_RCOMMAND)      text = stpcpy(text,"rcommand ");
    if (qual & IEQUALIFIER_NUMERICPAD)    text = stpcpy(text,"numpad ");
  }

  if (!(key->kb_KeyTag & 0x00004000) && (key->kb_Code != (UWORD)~0))
  {
    switch (key->kb_Code & 0x7F)
    {
      case 0x40 : strcpy(text,"space"); break;
      case 0x41 : strcpy(text,"backspace"); break;
      case 0x42 : strcpy(text,"tab"); break;
      case 0x43 : strcpy(text,"enter"); break;
      case 0x44 : strcpy(text,"return"); break;
      case 0x45 : strcpy(text,"esc"); break;
      case 0x46 : strcpy(text,"del"); break;
      case 0x4C : strcpy(text,"up"); break;
      case 0x4D : strcpy(text,"down"); break;
      case 0x4E : strcpy(text,"right"); break;
      case 0x4F : strcpy(text,"left"); break;
      case 0x50 : strcpy(text,"f1"); break;
      case 0x51 : strcpy(text,"f2"); break;
      case 0x52 : strcpy(text,"f3"); break;
      case 0x53 : strcpy(text,"f4"); break;
      case 0x54 : strcpy(text,"f5"); break;
      case 0x55 : strcpy(text,"f6"); break;
      case 0x56 : strcpy(text,"f7"); break;
      case 0x57 : strcpy(text,"f8"); break;
      case 0x58 : strcpy(text,"f9"); break;
      case 0x59 : strcpy(text,"f10"); break;
      case 0x5F : strcpy(text,"help"); break;
      default:
        data->ievent.ie_NextEvent = NULL;
        data->ievent.ie_Class = IECLASS_RAWKEY;
        data->ievent.ie_SubClass = 0;
        data->ievent.ie_Code = key->kb_Code;
        data->ievent.ie_Qualifier = 0;
        data->ievent.ie_position.ie_addr = (APTR) &data->ievent;
        posraw = RawKeyConvert(&data->ievent,text,MCPMAXRAWBUF-strlen(data->rawtext),0L);
        if (posraw >= 0)
          text[posraw] = '\0';
      break;
    }
  }
  return ((LONG)strlen(data->rawtext));
}


static LONG NL_SaveKeys(struct NListviews_MCP_Data *data)
{
  LONG pos,ne = 0;
  struct KeyBinding *key;
  get(data->mcp_listkeys, MUIA_NList_Entries, &ne);
  ne++;
  data->nlkeys_size = ne*sizeof(struct KeyBinding);

  if((data->nlkeys = (struct KeyBinding *) AllocMem(data->nlkeys_size+4,0L)))
  {
    pos = 0;

    while (pos < ne)
    {
      DoMethod(data->mcp_listkeys,MUIM_NList_GetEntry,pos, &key);
      if (key)
      {
        data->nlkeys[pos] = *key;
        if (data->nlkeys[pos].kb_KeyTag & 0x00004000)
          data->nlkeys[pos].kb_Code = (UWORD)~0;
      }
      else
        break;
      pos++;
    }

    data->nlkeys[pos].kb_KeyTag = 0L;
    data->nlkeys[pos].kb_Code = (UWORD)~0;
    data->nlkeys[pos].kb_Qualifier = 0;
    pos++;
    ne = ((pos * sizeof(struct KeyBinding)) + 3) & 0xFFFFFFFC;

    return (ne);
  }

  return (0);
}


static void NL_LoadKeys(Object *list,struct KeyBinding *keys)
{
  int i = 0;
  set(list, MUIA_NList_Quiet, TRUE);
  DoMethod(list,MUIM_NList_Clear);
  while (keys[i].kb_KeyTag)
  { DoMethod(list,MUIM_NList_InsertSingle,&keys[i], MUIV_NList_Insert_Bottom);
    i++;
  }
  set(list, MUIA_NList_Quiet, FALSE);
}


static void NL_UpdateKeys(Object *list,struct KeyBinding *keys)
{
  int i;
  LONG pos,posmax = -1;
  struct KeyBinding *key;
  set(list, MUIA_NList_Quiet, TRUE);
  get(list,MUIA_NList_Entries,&posmax);
  for (i = 0; keys[i].kb_KeyTag; i++)
  {
    for (pos = 0; pos < posmax; pos++)
    {
      DoMethod(list,MUIM_NList_GetEntry, pos, &key);
      if (key && (key->kb_KeyTag == keys[i].kb_KeyTag))
        break;
/*
{
LONG k1 = (LONG) key->kb_KeyTag;
LONG k2 = (LONG) keys[i].kb_KeyTag;
kprintf("%lx|pos=%ld  key=%lx  kt=%lx (== %lx)\n",list,pos,key,k1,k2);
}
else
{
LONG k1 = (LONG) key->kb_KeyTag;
LONG k2 = (LONG) keys[i].kb_KeyTag;
kprintf("%lx|pos=%ld  key=%lx  kt=%lx (!= %lx)\n",list,pos,key,k1,k2);
}
*/
    }
    if (pos >= posmax)
      DoMethod(list,MUIM_NList_InsertSingle,&keys[i], MUIV_NList_Insert_Bottom);
  }
  set(list, MUIA_NList_Quiet, FALSE);
}

#ifdef __AROS__
AROS_HOOKPROTONH(StrObjFunc, LONG, Object *, pop, Object *, str)
#else
HOOKPROTONH(StrObjFunc, LONG, Object *pop, Object *str)
#endif
{
  HOOK_INIT
  
  LONG i;

  get(str,MUIA_UserData,&i);
  if (i >= 0)
    set(pop,MUIA_List_Active,i);
  else
    set(pop,MUIA_List_Active,MUIV_List_Active_Off);
  return(TRUE);
  
  HOOK_EXIT
}
MakeStaticHook(StrObjHook, StrObjFunc);

#ifdef __AROS__
AROS_HOOKPROTONH(ObjStrFunc, VOID, Object *, pop, Object *, str)
#else
HOOKPROTONH(ObjStrFunc, VOID, Object *pop, Object *str)
#endif
{
  HOOK_INIT
  
  LONG i = -1;
  get(pop,MUIA_List_Active,&i);
  if (i >= 0)
  {
    set(str,MUIA_UserData,i);
    set(str,MUIA_Text_Contents,functions_names[i]);
  }
  else
  { i = -1;
    set(str,MUIA_UserData,i);
    set(str,MUIA_Text_Contents,"");
  }
  
  HOOK_EXIT
}
MakeStaticHook(ObjStrHook, ObjStrFunc);


#ifdef __AROS__
AROS_HOOKPROTONH(WindowFunc, VOID, Object *, pop, Object *, win)
#else
HOOKPROTONH(WindowFunc, VOID, Object *pop, Object *win)
#endif
{
  HOOK_INIT
  
  set(win,MUIA_Window_DefaultObject,pop);
  
  HOOK_EXIT
}
MakeStaticHook(WindowHook, WindowFunc);


#ifdef __AROS__
AROS_HOOKPROTONH(TxtFctFunc, VOID, Object *, list, long *, val)
#else
HOOKPROTONH(TxtFctFunc, VOID, Object *list, long *val)
#endif
{
  HOOK_INIT
  
  Object *txtfct = (Object *) val[0];
  struct KeyBinding *key = NULL;
  LONG i = -1;
  get(txtfct,MUIA_UserData,&i);
  DoMethod(list,MUIM_NList_GetEntry,MUIV_NList_GetEntry_Active, &key);
  if (key && (i >= 0))
  { key->kb_KeyTag = keytags[i];
    DoMethod(list,MUIM_NList_Redraw,MUIV_NList_Redraw_Active);
    get(list,MUIA_NList_Active,&i);
    set(list,MUIA_NList_Active,i);
  }
  
  HOOK_EXIT
}
MakeStaticHook(TxtFctHook, TxtFctFunc);

#ifdef __AROS__
AROS_HOOKPROTONH(AckFunc, VOID, Object *, list, long *, val)
#else
HOOKPROTONH(AckFunc, VOID, Object *list, long *val)
#endif
{
  HOOK_INIT
  
  Object *stringkey = (Object *) val[0];
  struct KeyBinding *key = NULL;
  char *ackstr = NULL;
  get(stringkey,MUIA_String_Contents, &ackstr);
  DoMethod(list,MUIM_NList_GetEntry,MUIV_NList_GetEntry_Active, &key);
  if (ackstr && key)
  { LONG res;
    IX ix;
    ix.ix_Version = IX_VERSION;
    res = ParseIX(ackstr,&ix);
    key->kb_Qualifier = (ix.ix_Qualifier & KBQUAL_MASK) | ((ix.ix_QualSame << 12) & KBSYM_MASK);
    key->kb_Code = ix.ix_Code;
    DoMethod(list,MUIM_NList_Redraw,MUIV_NList_Redraw_Active);
  }
  
  HOOK_EXIT
}
MakeStaticHook(AckHook, AckFunc);

#ifdef __AROS__
AROS_HOOKPROTONH(ActiveFunc, VOID, Object *, list, long *, val)
#else
HOOKPROTONH(ActiveFunc, VOID, Object *list, long *val)
#endif
{
  HOOK_INIT
  
  struct NListviews_MCP_Data *data = (struct NListviews_MCP_Data *) (val[0]);
/*  Object *win = NULL;*/
  ULONG active = (ULONG) (val[1]);
  struct KeyBinding *key = NULL;
  if (active >= 0)
  { DoMethod(list,MUIM_NList_GetEntry,MUIV_NList_GetEntry_Active, &key);
    if (key)
    { LONG i;
      i = 0;
      while ((keytags[i] > 0) && (keytags[i] != key->kb_KeyTag))
        i++;
      if (keytags[i] == key->kb_KeyTag)
      { nnset(data->mcp_stringkey,MUIA_HotkeyString_Snoop, FALSE);
        nnset(data->mcp_stringkey,MUIA_Disabled, FALSE);
        nnset(data->mcp_snoopkey,MUIA_Disabled, FALSE);
        nnset(data->mcp_txtfct,MUIA_UserData,i);
        nnset(data->mcp_txtfct,MUIA_Text_Contents,functions_names[i]);

        DeadKeyConvert(data,key);
        nnset(data->mcp_stringkey,MUIA_String_Contents, data->rawtext);
/*
        get(list, MUIA_WindowObject, &win);
        if (win)
          set(win, MUIA_Window_ActiveObject, data->mcp_stringkey);
*/
      }
      else
        key = NULL;
    }
  }
  if (!key)
  { nnset(data->mcp_txtfct,MUIA_UserData,-1);
    nnset(data->mcp_txtfct,MUIA_Text_Contents,"");
    nnset(data->mcp_stringkey,MUIA_String_Contents, "");
    nnset(data->mcp_stringkey,MUIA_Disabled, TRUE);
    nnset(data->mcp_snoopkey,MUIA_Disabled, TRUE);
  }
  
  HOOK_EXIT
}
MakeStaticHook(ActiveHook, ActiveFunc);


#ifdef __AROS__
AROS_HOOKPROTONHNP(DefaultFunc, VOID, Object *, list)
#else
HOOKPROTONHNP(DefaultFunc, VOID, Object *list)
#endif
{
  HOOK_INIT
  
  if (list)
    NL_LoadKeys(list,default_keys);
  
  HOOK_EXIT
}
MakeStaticHook(DefaultHook, DefaultFunc);

#ifdef __AROS__
AROS_HOOKPROTONHNP(UpdateFunc, VOID, Object *, list)
#else
HOOKPROTONHNP(UpdateFunc, VOID, Object *list)
#endif
{
  HOOK_INIT
  
  if (list)
    NL_UpdateKeys(list,default_keys);
    
  HOOK_EXIT
}
MakeStaticHook(UpdateHook, UpdateFunc);

#ifdef __AROS__
AROS_HOOKPROTONHNP(InsertFunc, VOID, Object *, list)
#else
HOOKPROTONHNP(InsertFunc, VOID, Object *list)
#endif
{
  HOOK_INIT
  
  if (list)
  { struct KeyBinding *key;
    LONG pos;
    DoMethod(list,MUIM_NList_GetEntry,MUIV_NList_GetEntry_Active, &key);
    if (!key)
    { empty_key.kb_KeyTag = keytags[0];
      key = &empty_key;
    }
    set(list,MUIA_NList_Quiet,TRUE);
    DoMethod(list,MUIM_NList_InsertSingle,key, MUIV_NList_Insert_Active);
    get(list,MUIA_NList_InsertPosition,&pos);
    set(list,MUIA_NList_Active,pos);
    set(list,MUIA_NList_Quiet,FALSE);
  }
  
  HOOK_EXIT
}
MakeStaticHook(InsertHook, InsertFunc);

#ifdef __AROS__
AROS_HOOKPROTONH(DisplayFunc, VOID, Object *, obj, struct NList_DisplayMessage *, ndm)
#else
HOOKPROTONH(DisplayFunc, VOID, Object *obj, struct NList_DisplayMessage *ndm)
#endif
{
  HOOK_INIT
  
  struct KeyBinding *key = (struct KeyBinding *) ndm->entry;
  struct NListviews_MCP_Data *data;
  get(obj,MUIA_UserData,&data);

  if (key && data)
  {
    LONG i;

    ndm->preparses[0]  = "\033r";

    DeadKeyConvert(data,key);
    ndm->strings[0] = data->rawtext;

    ndm->strings[1] = "\033c=";

    i = 0;
    while ((keytags[i] > 0) && (keytags[i] != key->kb_KeyTag))
      i++;
    ndm->strings[2]  = functions_names[i];
  }
  else
  {
    ndm->strings[0]  = "\033cKey";
    ndm->strings[1]  = "";
    ndm->strings[2]  = "\033cAction";
  }
  
  HOOK_EXIT
}
MakeStaticHook(DisplayHook, DisplayFunc);

#ifdef __AROS__
AROS_HOOKPROTONHNO(ConstructFunc, APTR, struct NList_ConstructMessage *, ncm)
#else
HOOKPROTONHNO(ConstructFunc, APTR, struct NList_ConstructMessage *ncm)
#endif
{
  HOOK_INIT
  
  struct KeyBinding *key = (struct KeyBinding *) ncm->entry;

  struct KeyBinding *key2 = (struct KeyBinding *) AllocMem(sizeof(struct KeyBinding),0L);
  if (key2)
    *key2 = *key;

  return ((APTR) key2);
  
  HOOK_EXIT
}
MakeStaticHook(ConstructHook, ConstructFunc);

#ifdef __AROS__
AROS_HOOKPROTONHNO(DestructFunc, VOID, struct NList_DestructMessage *, ndm)
#else
HOOKPROTONHNO(DestructFunc, VOID, struct NList_DestructMessage *ndm)
#endif
{
  HOOK_INIT
  
  struct KeyBinding *key = (struct KeyBinding *) ndm->entry;

  FreeMem((void *) key,sizeof(struct KeyBinding));
  
  HOOK_EXIT
}
MakeStaticHook(DestructHook, DestructFunc);

static ULONG mNL_MCP_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  struct NListviews_MCP_Data *data;
  APTR group1, group2, group31, group32, group33, group34, group35, group36, group4, group5;

  if(!(obj = (Object *)DoSuperMethodA(cl, obj,(Msg) msg)))
    return(0);

  data = INST_DATA(cl,obj);

  group1 = group2 = group31 = group32 = group33 = group34 = group35 = group36 = group4 = group5 = NULL;

  data->mcp_group = data->mcp_list1 = data->mcp_list2 = NULL;
  data->mcp_PenTitle = data->mcp_PenList = NULL;
  data->mcp_PenSelect = data->mcp_PenCursor = data->mcp_PenUnselCur = NULL;
  data->mcp_BG_Title = data->mcp_BG_List = NULL;
  data->mcp_BG_Select = data->mcp_BG_Cursor = data->mcp_BG_UnselCur = NULL;
  data->mcp_R_Multi = data->mcp_B_MultiMMB = data->mcp_R_Drag = NULL;
  data->mcp_ST_DragQualifier = data->mcp_SL_VertInc = NULL;
  data->mcp_R_HSB = data->mcp_R_VSB = data->mcp_B_Smooth = NULL;
  data->mcp_Font = data->mcp_Font_Little = data->mcp_Font_Fixed = NULL;
  data->mcp_ForcePen = data->mcp_StackCheck = data->mcp_ColWidthDrag = NULL;
  data->mcp_PartialCol = data->mcp_List_Select = data->mcp_NList_Menu = NULL;
  data->mcp_PartialChar = data->mcp_PointerColor = data->mcp_SerMouseFix = data->mcp_DragLines = NULL;
  data->mcp_WheelStep = data->mcp_WheelFast = data->mcp_WheelMMB = NULL;
  data->mcp_listkeys = data->mcp_stringkey = data->mcp_snoopkey = NULL;
  data->mcp_insertkey = data->mcp_removekey = data->mcp_defaultkeys = data->mcp_updatekeys = NULL;
  data->mcp_txtfct = data->mcp_popstrfct = data->mcp_poplistfct = NULL;

  data->nlkeys = NULL;

  data->mcp_stringkey =   MUI_NewObject( MUIC_HotkeyString,
                            StringFrame, MUIA_CycleChain, 1,
                            MUIA_HotkeyString_Snoop, FALSE,
                            MUIA_Disabled,TRUE,
                          TAG_DONE );

  if (!data->mcp_stringkey)
  { data->mcp_stringkey = MUI_NewObject(MUIC_String,
                            StringFrame, MUIA_CycleChain, 1,
                          TAG_DONE);
  }

  group1 = MUI_NewObject(MUIC_Group,
          Child, data->mcp_list1 = MUI_NewObject( MUIC_NListview,
            MUIA_CycleChain, 1,
            MUIA_NList_Title,"\033cNList / NListview",
            MUIA_NList_TitleSeparator, TRUE,
            MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Default,
            MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Default,
            MUIA_NList_DefaultObjectOnClick, TRUE,
            MUIA_NList_SourceArray, MainTextArray,
          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Columns,(2), GroupFrameT(STRING(MSG_FONTS,"Fonts")),
            GroupSpacing(2),
            MUIA_Weight,(ULONG)30,
            Child, Label(STRING(MSG_NORMAL_FONT,"Normal Font:")),
            Child, data->mcp_Font = MUI_NewObject(MUIC_Popasl,
              MUIA_Popstring_String, String2(0,80),
              MUIA_Popstring_Button, PopButton(MUII_PopUp),
              MUIA_Popasl_Type , ASL_FontRequest,
              MUIA_ShortHelp, STRING(MSG_NORMAL_FONT_HELP,"Choose the normal NList font."),
              ASLFO_TitleText  , STRING(MSG_NORMAL_FONT_ASL,"Please select the normal font..."),
            TAG_DONE),

            Child, Label(STRING(MSG_LITTLE_FONT,"Little Font:")),
            Child, data->mcp_Font_Little = MUI_NewObject(MUIC_Popasl,
              MUIA_Popstring_String, String2(0,80),
              MUIA_Popstring_Button, PopButton(MUII_PopUp),
              MUIA_Popasl_Type , ASL_FontRequest,
              MUIA_ShortHelp, STRING(MSG_LITTLE_FONT_HELP,"Choose the little NList font."),
              ASLFO_TitleText  , STRING(MSG_LITTLE_FONT_ASL,"Please select the little font..."),
            TAG_DONE),

            Child, Label(STRING(MSG_FIXED_FONT,"Fixed Font:")),
            Child, data->mcp_Font_Fixed = MUI_NewObject(MUIC_Popasl,
              MUIA_Popstring_String, String2(0,80),
              MUIA_Popstring_Button, PopButton(MUII_PopUp),
              MUIA_Popasl_Type , ASL_FontRequest,
              MUIA_ShortHelp, STRING(MSG_FIXED_FONT_HELP,"Choose the fixed NList font."),
              ASLFO_TitleText  , STRING(MSG_FIXED_FONT_ASL,"Please select the fixed font..."),
              ASLFO_FixedWidthOnly, TRUE,
            TAG_DONE),
          TAG_DONE),
      TAG_DONE);

  group2 = MUI_NewObject(MUIC_Group,
          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Columns,(3), GroupFrameT(STRING(MSG_COLORS,"Colors")),
            GroupSpacing(2),
            Child, MUI_MakeObject(MUIO_BarTitle, STRING(MSG_PENS,"Pens")),
            Child, VCenter(Label("")),
            Child, MUI_MakeObject(MUIO_BarTitle, STRING(MSG_BACKGROUNDS,"Backs")),

            Child, data->mcp_PenTitle = MUI_NewObject(MUIC_Poppen,
              MUIA_CycleChain, 1,
              MUIA_Window_Title, STRING(MSG_TITLE_PEN_WIN,"Title Pen"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_TITLE_PEN_HELP,"Adjust Color of Title Pen."),
            TAG_DONE),
            Child, VCenter(Label(STRING(MSG_PBG_TITLE,"\033c Title "))),
            Child, data->mcp_BG_Title = MUI_NewObject(MUIC_Popimage,
              MUIA_CycleChain, 1,
              MUIA_Imageadjust_Type, MUIV_Imageadjust_Type_Background,
              MUIA_Window_Title, STRING(MSG_TITLE_BG_WIN,"Title Background"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_TITLE_BG_HELP,"Adjust Title Background."),
            TAG_DONE),

            Child, data->mcp_PenList = MUI_NewObject(MUIC_Poppen,
              MUIA_CycleChain, 1,
              MUIA_Window_Title, STRING(MSG_LIST_PEN_WIN,"List Pen"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_LIST_PEN_HELP,"Adjust Color of List Pen."),
            TAG_DONE),
            Child, VCenter(Label(STRING(MSG_PBG_LIST,"\033c List "))),
            Child, data->mcp_BG_List = MUI_NewObject(MUIC_Popimage,
              MUIA_CycleChain, 1,
              MUIA_Imageadjust_Type, MUIV_Imageadjust_Type_Background,
              MUIA_Window_Title, STRING(MSG_LIST_BG_WIN,"List Background"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_LIST_BG_HELP,"Adjust List Background."),
            TAG_DONE),

            Child, data->mcp_PenSelect = MUI_NewObject(MUIC_Poppen,
              MUIA_CycleChain, 1,
              MUIA_Window_Title, STRING(MSG_SELECT_PEN_WIN,"Selected Entry"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_SELECT_PEN_HELP,"Adjust Color of Selected Entry Pen."),
            TAG_DONE),
            Child, VCenter(Label(STRING(MSG_PBG_SELECT,"\033c Select "))),
            Child, data->mcp_BG_Select = MUI_NewObject(MUIC_Popimage,
              MUIA_CycleChain, 1,
              MUIA_Imageadjust_Type, MUIV_Imageadjust_Type_Background,
              MUIA_Window_Title, STRING(MSG_SELECT_BG_WIN,"Selected Entry"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_SELECT_BG_HELP,"Adjust Selected Entry Background."),
            TAG_DONE),

            Child, data->mcp_PenCursor = MUI_NewObject(MUIC_Poppen,
              MUIA_CycleChain, 1,
              MUIA_Window_Title, STRING(MSG_CURSOR_PEN_WIN,"Selected Cursor"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_CURSOR_PEN_HELP,"Adjust Color of Selected Cursor Pen."),
            TAG_DONE),
            Child, VCenter(Label(STRING(MSG_PBG_CURSOR,"\033c Cursor "))),
            Child, data->mcp_BG_Cursor = MUI_NewObject(MUIC_Popimage,
              MUIA_CycleChain, 1,
              MUIA_Imageadjust_Type, MUIV_Imageadjust_Type_Background,
              MUIA_Window_Title, STRING(MSG_CURSOR_BG_WIN,"Selected Cursor"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_CURSOR_BG_HELP,"Adjust Selected Cursor Background."),
            TAG_DONE),

            Child, data->mcp_PenUnselCur = MUI_NewObject(MUIC_Poppen,
              MUIA_CycleChain, 1,
              MUIA_Window_Title, STRING(MSG_UNSEL_PEN_WIN,"Unselected Cursor"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_UNSEL_PEN_HELP,"Adjust Color of Unselected Cursor Pen."),
            TAG_DONE),
            Child, VCenter(Label(STRING(MSG_PBG_UNSEL,"\033c Unsel "))),
            Child, data->mcp_BG_UnselCur = MUI_NewObject(MUIC_Popimage,
              MUIA_CycleChain, 1,
              MUIA_Imageadjust_Type, MUIV_Imageadjust_Type_Background,
              MUIA_Window_Title, STRING(MSG_UNSEL_BG_WIN,"Unselected Cursor"),
              MUIA_Draggable, TRUE,
              MUIA_ShortHelp, STRING(MSG_UNSEL_BG_HELP,"Adjust Unselected Cursor Background."),
            TAG_DONE),
          TAG_DONE),
      TAG_DONE);

  group31 = MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
          Child, MUI_NewObject(MUIC_Group,

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrameT(STRING(MSG_MULTISELECT,"MultiSelect")),
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, data->mcp_R_Multi = MUI_NewObject(MUIC_Radio,
                  MUIA_Radio_Entries,RS_MultiSelects,
                  MUIA_ShortHelp, STRING(MSG_MULTISELECT_HELP,"Choose your default multiselection mode."),
                TAG_DONE),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                  Child, data->mcp_B_MultiMMB = MUI_NewObject(MUIC_Image,
                    ImageButtonFrame,
                    MUIA_InputMode        , MUIV_InputMode_Toggle,
                    MUIA_Image_Spec       , MUII_CheckMark,
                    MUIA_Image_FreeVert   , TRUE,
                    MUIA_Background       , MUII_ButtonBack,
                    MUIA_ShowSelState     , FALSE,
                  TAG_DONE),
                  Child, Label(STRING(MSG_MMB_MULTISEL,"MMB multiselect")),
                  MUIA_ShortHelp, STRING(MSG_MMB_MULTISEL_HELP,"Set it if you want to use the Middle\nMouse Button as a multiselect qualifier."),
                TAG_DONE),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrameT(STRING(MSG_DRAGTYPE,"DragType")),
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
              Child, VSpace(0),
              Child, data->mcp_R_Drag = MUI_NewObject(MUIC_Radio,
                MUIA_Radio_Entries,RS_DragTypes,
                MUIA_ShortHelp, STRING(MSG_DRAGTYPE_HELP,"Set the drag mode. Immediate for the three,\nBorders for left/right list edges and qualifier,\nQualifier for qualifier only.\nImmediate is used only when no-multiselect mode."),
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
            Child, HSpace(0),
          TAG_DONE),

        TAG_DONE);

  group32 = MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
          Child, MUI_NewObject(MUIC_Group,

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                MUIA_HorizWeight, 1000,
                Child, VSpace(0),
                Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                  Child, Label(STRING(MSG_LEADING,"Leading")),
                  Child, data->mcp_SL_VertInc = MUI_NewObject(MUIC_Slider,
                    MUIA_CycleChain, 1,
                    MUIA_Numeric_Min  , 0,
                    MUIA_Numeric_Max  , 9,
                    MUIA_Numeric_Value, 1,
                  TAG_DONE),
                  MUIA_ShortHelp, STRING(MSG_LEADING_HELP,"Adjust the value which will be\nadded to the font height to get\nthe default entry height."),
                TAG_DONE),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrameT(STRING(MSG_DEFAULT_CONTEXT_MENU,"Default ContextMenu")),
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
              Child, VSpace(0),
              Child, data->mcp_NList_Menu = MUI_NewObject(MUIC_Radio,
                MUIA_Radio_Entries,RS_Menu,
                MUIA_ShortHelp, STRING(MSG_DEFAULT_CONTEXT_MENU_HELP,"NList permit to have its default\ncontext menu being disabled or enabled\nonly on title(top of list when no title).\n"),
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
            Child, HSpace(0),
          TAG_DONE),

        TAG_DONE);

  group33 = MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,

          Child, MUI_NewObject(MUIC_Group,

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                  Child, data->mcp_PartialCol = MUI_NewObject(MUIC_Image,
                    ImageButtonFrame,
                    MUIA_InputMode        , MUIV_InputMode_Toggle,
                    MUIA_Image_Spec       , MUII_CheckMark,
                    MUIA_Image_FreeVert   , TRUE,
                    MUIA_Background       , MUII_ButtonBack,
                    MUIA_ShowSelState     , FALSE,
                    MUIA_Selected         , TRUE,
                  TAG_DONE),
                  Child, Label(STRING(MSG_PARTIAL_COL_MARK,"Partial col. mark")),
                  MUIA_ShortHelp, STRING(MSG_PARTIAL_COL_MARK_HELP,"When set, a mark will be drawn\nwhen the contents of an entry\ncolumn couldn't be drawn fully."),
                TAG_DONE),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                  Child, data->mcp_PartialChar = MUI_NewObject(MUIC_Image,
                    ImageButtonFrame,
                    MUIA_InputMode        , MUIV_InputMode_Toggle,
                    MUIA_Image_Spec       , MUII_CheckMark,
                    MUIA_Image_FreeVert   , TRUE,
                    MUIA_Background       , MUII_ButtonBack,
                    MUIA_ShowSelState     , FALSE,
                    MUIA_Selected         , FALSE,
                  TAG_DONE),
                  Child, Label(STRING(MSG_PARTIAL_CHARS_DRAWN,"Partial chars drawn")),
                  MUIA_ShortHelp, STRING(MSG_PARTIAL_CHARS_DRAWN_HELP,"When set, chars on the right and\nleft edge of the list which are\nnot fully visible will be drawn."),
                TAG_DONE),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrameT(STRING(MSG_BALANCING_COLS,"Balancing Columns")),
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, data->mcp_ColWidthDrag = MUI_NewObject(MUIC_Radio,
                  MUIA_Radio_Entries,RS_ColWidthDrag,
                TAG_DONE),
                MUIA_ShortHelp, STRING(MSG_BALANCING_COLS_HELP,"NList permit to change the column width\nwith mouse dragging the column separator.\nThe moved bar can be visible on first\nline or title only, be visible on the\nfull list height, or the width changes\ncan be immediatly visibles.\n"),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

          TAG_DONE),

        TAG_DONE);

  group34 = MUI_NewObject(MUIC_Group,

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
              Child, VSpace(0),
              Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                Child, data->mcp_ForcePen = MUI_NewObject(MUIC_Image,
                  ImageButtonFrame,
                  MUIA_InputMode        , MUIV_InputMode_Toggle,
                  MUIA_Image_Spec       , MUII_CheckMark,
                  MUIA_Image_FreeVert   , TRUE,
                  MUIA_Background       , MUII_ButtonBack,
                  MUIA_ShowSelState     , FALSE,
                TAG_DONE),
                Child, Label(STRING(MSG_FORCE_SELECT_PEN,"Force select. pen")),
                MUIA_ShortHelp, STRING(MSG_FORCE_SELECT_PEN_HELP,"Set it if you want the Select,\nCursor and Unselected Cursor pen\ncolors to be forced."),
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
            Child, HSpace(0),
          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
              MUIA_HorizWeight, 1000,
              Child, VSpace(0),
              Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                Child, Label(STRING(MSG_DRAG_LINES,"Max visible drag")),
                Child, data->mcp_DragLines = MUI_NewObject(MUIC_Slider,
                  MUIA_CycleChain, 1,
                  MUIA_Numeric_Min  , 0,
                  MUIA_Numeric_Max  , 20,
                  MUIA_Numeric_Value, DEFAULT_DRAGLINES,
                TAG_DONE),
                MUIA_ShortHelp, STRING(MSG_DRAG_LINES_HELP,"Give the max number of lines\nwhich will be shown when\ndragged. When more you'll get\nthe message : Dragging xx Items..."),
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
            Child, HSpace(0),
          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
              MUIA_HorizWeight, 1000,
              Child, VSpace(0),
              Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                Child, Label(STRING(MSG_SPECIAL_POINTER_COLORS,"Pointer black col.num.")),
                Child, data->mcp_PointerColor = MUI_NewObject(MUIC_Slider,
                  MUIA_CycleChain, 1,
                  MUIA_Numeric_Min  , 1,
                  MUIA_Numeric_Max  , 3,
                  MUIA_Numeric_Value, 2,
                TAG_DONE),
                MUIA_ShortHelp, STRING(MSG_SPECIAL_POINTER_COLORS_HELP,"Set it to the color number which is\nthe black/dark in your system pointer\n(it is usefull\nwith some gfx board)."),
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
            Child, HSpace(0),
          TAG_DONE),

        TAG_DONE);

  group35 = MUI_NewObject(MUIC_Group,

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
              Child, VSpace(0),
              Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                Child, data->mcp_WheelMMB = MUI_NewObject(MUIC_Image,
                  ImageButtonFrame,
                  MUIA_InputMode        , MUIV_InputMode_Toggle,
                  MUIA_Image_Spec       , MUII_CheckMark,
                  MUIA_Image_FreeVert   , TRUE,
                  MUIA_Background       , MUII_ButtonBack,
                  MUIA_ShowSelState     , FALSE,
                TAG_DONE),
                Child, Label(STRING(MSG_MMB_FASTWHEEL,"MMB Fast Wheel")),
                MUIA_ShortHelp, STRING(MSG_MMB_FASTWHEEL_HELP,"Set it if you want to use the Middle\nMouse Button as a fast mouse wheel qualifier.\nSee Mouse Wheel qualifiers in\nkeubindings too."),
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
            Child, HSpace(0),
          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
              MUIA_HorizWeight, 1000,
              Child, VSpace(0),
              Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                Child, Label(STRING(MSG_WHEEL_STEP,"Mouse Wheel Step")),
                Child, data->mcp_WheelStep = MUI_NewObject(MUIC_Slider,
                  MUIA_CycleChain, 1,
                  MUIA_Numeric_Min  , 1,
                  MUIA_Numeric_Max  , 5,
                  MUIA_Numeric_Value, 1,
                TAG_DONE),
                MUIA_ShortHelp, STRING(MSG_WHEEL_STEP_HELP,"Set it to the number of entries scrolled\nfor each mouse wheel move."),
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
            Child, HSpace(0),
          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
              MUIA_HorizWeight, 1000,
              Child, VSpace(0),
              Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                Child, Label(STRING(MSG_WHEEL_FAST,"Mouse Wheel Fast Step")),
                Child, data->mcp_WheelFast = MUI_NewObject(MUIC_Slider,
                  MUIA_CycleChain, 1,
                  MUIA_Numeric_Min  , 2,
                  MUIA_Numeric_Max  , 10,
                  MUIA_Numeric_Value, 5,
                TAG_DONE),
                MUIA_ShortHelp, STRING(MSG_WHEEL_FAST_HELP,"Set it to the number of entries scrolled\nfor each mouse wheel move\nwhen in fast mode."),
              TAG_DONE),
              Child, VSpace(0),
            TAG_DONE),
            Child, HSpace(0),
          TAG_DONE),

        TAG_DONE);

  group36 = MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,

          Child, MUI_NewObject(MUIC_Group,

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                  Child, data->mcp_List_Select = MUI_NewObject(MUIC_Image,
                    ImageButtonFrame,
                    MUIA_InputMode        , MUIV_InputMode_Toggle,
                    MUIA_Image_Spec       , MUII_CheckMark,
                    MUIA_Image_FreeVert   , TRUE,
                    MUIA_Background       , MUII_ButtonBack,
                    MUIA_ShowSelState     , FALSE,
                    MUIA_Selected         , TRUE,
                  TAG_DONE),
                  Child, Label(STRING(MSG_LIST_LIKE_MULTISEL,"List like multiselect.")),
                  MUIA_ShortHelp, STRING(MSG_LIST_LIKE_MULTISEL_HELP,"Set it if you want the Multiselect\nwith key act near like List one."),
                TAG_DONE),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrame, MUIA_Background, MUII_GroupBack,
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                  Child, data->mcp_SerMouseFix = MUI_NewObject(MUIC_Image,
                    ImageButtonFrame,
                    MUIA_InputMode        , MUIV_InputMode_Toggle,
                    MUIA_Image_Spec       , MUII_CheckMark,
                    MUIA_Image_FreeVert   , TRUE,
                    MUIA_Background       , MUII_ButtonBack,
                    MUIA_ShowSelState     , FALSE,
                  TAG_DONE),
                  Child, Label(STRING(MSG_SERMOUSE_FIX,"Serial Mouse Fix")),
                  MUIA_ShortHelp, STRING(MSG_SERMOUSE_FIX_HELP,"Set it to fix Drag&Drop problem\nif you are using some special pointer\ndriver (most of time on serial port)."),
                TAG_DONE),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrameT(STRING(MSG_STACK_WARNING,"Stack Warning")),
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, data->mcp_StackCheck = MUI_NewObject(MUIC_Radio,
                  MUIA_Radio_Entries,RS_StackCheck,
                TAG_DONE),
                MUIA_ShortHelp, STRING(MSG_STACK_WARNING_HELP,"NList objects do checks to\nknow what part of stack is free\nand open a requester when it is\ntoo small.\nIt's not perfect because the stack\nis checked only at some points of the\nobject code and MUI code use more stack\nthan NList will ever see beeing used.\n\nAnyway it should prevent\nor explain most of stack crashes...\n\nIt seems that [< 2 Kb] is a good value."),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

          TAG_DONE),

        TAG_DONE);

  group4 = MUI_NewObject(MUIC_Group,

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrameT(STRING(MSG_SB_HORIZONTAL,"Horizontal Scrollbar")),
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, data->mcp_R_HSB = MUI_NewObject(MUIC_Radio,
                  MUIA_Radio_Entries,RS_HSB,
                TAG_DONE),
                MUIA_ShortHelp, STRING(MSG_SB_HORIZONTAL_HELP,"Set the default mode\nof the horizontal scroller."),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE, GroupFrameT(STRING(MSG_SB_VERTICAL,"Vertical Scrollbar")),
              Child, HSpace(0),
              Child, MUI_NewObject(MUIC_Group,
                Child, VSpace(0),
                Child, data->mcp_R_VSB = MUI_NewObject(MUIC_Radio,
                  MUIA_Radio_Entries,RS_VSB,
                TAG_DONE),
                MUIA_ShortHelp, STRING(MSG_SB_VERTICAL_HELP,"Set the default mode\nof the vertical scroller."),
                Child, VSpace(0),
              TAG_DONE),
              Child, HSpace(0),
            TAG_DONE),
            Child, VSpace(0),

          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group, GroupFrameT(STRING(MSG_SMOOTH_SCROLL,"Smooth Scrolling")),
            Child, VSpace(0),
            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
              Child, HSpace(0),
              Child, data->mcp_B_Smooth = MUI_NewObject(MUIC_Image,
                ImageButtonFrame,
                MUIA_InputMode        , MUIV_InputMode_Toggle,
                MUIA_Image_Spec       , MUII_CheckMark,
                MUIA_Image_FreeVert   , TRUE,
                MUIA_Background       , MUII_ButtonBack,
                MUIA_ShowSelState     , FALSE,
              TAG_DONE),
              Child, Label(STRING(MSG_SMOOTH_SCROLLING,"Smooth Scrolling.\n(set speed with\nthe smooth\nslider in Listviews)")),
              MUIA_ShortHelp, STRING(MSG_SMOOTH_SCROLLING_HELP,"Set it if you want to use Smooth\nscrolling for NList objects.\n\n*Use the smooth slider in Listviews*\n*prefs to set how fast/slow it will be.*\n\nSmooth for standard lists can be on or\noff, but a 0 smooth slider value make\nno smooth at all !"),
              Child, HSpace(0),
            TAG_DONE),
            Child, VSpace(0),
          TAG_DONE),

        TAG_DONE);

  group5 = MUI_NewObject(MUIC_Group,
          Child, MUI_NewObject( MUIC_NListview,
            MUIA_CycleChain, 1,
            MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always,
            MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_None,
            MUIA_NListview_NList, data->mcp_listkeys = MUI_NewObject( MUIC_NList,
              MUIA_NList_Title,TRUE,
              MUIA_NList_DragSortable, TRUE,
              MUIA_NList_Format, "W=100 NOBAR,NOBAR,W=100 NOBAR",
              MUIA_NList_DisplayHook2, &DisplayHook,
              MUIA_NList_ConstructHook2, &ConstructHook,
              MUIA_NList_DestructHook2, &DestructHook,
              MUIA_NList_MinColSortable, 10,
            TAG_DONE),
          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
              Child, data->mcp_stringkey,
              Child, data->mcp_snoopkey = ToggleButtonCycle("Snoop",FALSE,TRUE,STRING(MSG_SNOOP_KEY,"Start the grab of the hotkey.")),
            TAG_DONE),

            Child, MUI_NewObject(MUIC_Balance, TAG_DONE),

            Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
              Child, MUI_NewObject(MUIC_Text, MUIA_Text_Contents, "=", MUIA_Text_SetMax, TRUE, TAG_DONE),
              Child, data->mcp_popstrfct = MUI_NewObject(MUIC_Popobject,
                MUIA_InputMode, MUIV_InputMode_None,
                MUIA_Popstring_String, data->mcp_txtfct = MUI_NewObject(MUIC_Text,
                  TextFrame,
                  MUIA_Background, MUII_TextBack,
                  MUIA_Text_Contents, "                     ",
                  MUIA_UserData, -1,
                TAG_DONE),
                MUIA_Popstring_Button, PopButton(MUII_PopUp),
                MUIA_Popobject_StrObjHook, &StrObjHook,
                MUIA_Popobject_ObjStrHook, &ObjStrHook,
                MUIA_Popobject_WindowHook, &WindowHook,
                MUIA_Popobject_Object, data->mcp_poplistfct = MUI_NewObject(MUIC_Listview,
                  MUIA_Listview_List, MUI_NewObject(MUIC_List,
                    InputListFrame,
                    MUIA_List_SourceArray, functions_names,
                  TAG_DONE),
                TAG_DONE),
              TAG_DONE),
            TAG_DONE),

          TAG_DONE),

          Child, MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
            Child, data->mcp_insertkey = SimpleButtonCycle("Insert",STRING(MSG_INSERT_KEY,"Insert a new hotkey.")),
            Child, data->mcp_removekey = SimpleButtonCycle("Remove",STRING(MSG_REMOVE_KEY,"Remove a hotkey.")),
            Child, data->mcp_updatekeys = SimpleButtonCycle("Update keys",STRING(MSG_UPDATE_KEYS,"Add the default hotkeys for\nthe functions which are not\ncurrently in the list.")),
            Child, data->mcp_defaultkeys = SimpleButtonCycle("Default all keys",STRING(MSG_DEFAULT_KEYS,"Reset all hotkeys to default ones.")),
          TAG_DONE),

        TAG_DONE);

  data->mcp_group = MUI_NewObject(MUIC_Group,
    Child, MUI_NewObject(MUIC_Register,MUIA_Register_Titles,(Pages),
      MUIA_Register_Frame, TRUE,
      Child, group1,
      Child, group2,
      Child, MUI_NewObject(MUIC_Register,MUIA_Register_Titles,(Pages3),
        MUIA_Register_Frame, TRUE,
        Child, group31,
        Child, group32,
        Child, group33,
        Child, group34,
        Child, group35,
        Child, group36,
      TAG_DONE),
      Child, group4,
      Child, group5,
    TAG_DONE),
  TAG_DONE);

  if (data->mcp_PointerColor && (LIBVER(IntuitionBase) < INTUIBASEMIN))
    set(data->mcp_PointerColor,MUIA_Disabled,TRUE);

  if(!data->mcp_group)
  { CoerceMethod(cl, obj, OM_DISPOSE);
    return(0);
  }

  DoMethod(obj, OM_ADDMEMBER, data->mcp_group);

  set(data->mcp_stringkey,MUIA_String_AttachedList,data->mcp_listkeys);

  set(data->mcp_listkeys,MUIA_UserData,data);

  DoMethod(data->mcp_listkeys,MUIM_Notify,MUIA_NList_Active, MUIV_EveryTime,
           data->mcp_listkeys, 3, MUIM_CallHook, &ActiveHook,data,MUIV_TriggerValue);

  DoMethod(data->mcp_listkeys,MUIM_Notify,MUIA_NList_DoubleClick, MUIV_EveryTime,
           MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_ActiveObject, data->mcp_stringkey);

  DoMethod(data->mcp_poplistfct,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,
           data->mcp_popstrfct,2,MUIM_Popstring_Close,TRUE);

  DoMethod(data->mcp_txtfct, MUIM_Notify, MUIA_Text_Contents, MUIV_EveryTime,
           data->mcp_listkeys, 3, MUIM_CallHook, &TxtFctHook,data->mcp_txtfct);

  DoMethod(data->mcp_snoopkey, MUIM_Notify, MUIA_Selected, TRUE,
           MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_ActiveObject, data->mcp_stringkey);

  DoMethod(data->mcp_snoopkey, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
           data->mcp_stringkey, 3, MUIM_Set, MUIA_HotkeyString_Snoop, MUIV_TriggerValue);

  DoMethod(data->mcp_stringkey, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
           data->mcp_snoopkey, 3, MUIM_Set, MUIA_Selected, FALSE);

  DoMethod(data->mcp_stringkey, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
           data->mcp_listkeys, 3, MUIM_CallHook, &AckHook,data->mcp_stringkey);

  DoMethod(data->mcp_insertkey,MUIM_Notify, MUIA_Pressed, FALSE,
           data->mcp_listkeys, 3, MUIM_CallHook, &InsertHook);

  DoMethod(data->mcp_removekey,MUIM_Notify, MUIA_Pressed, FALSE,
           data->mcp_listkeys, 2,MUIM_NList_Remove,MUIV_NList_Remove_Active);

  DoMethod(data->mcp_defaultkeys,MUIM_Notify, MUIA_Pressed, FALSE,
           data->mcp_listkeys, 3, MUIM_CallHook, &DefaultHook);

  DoMethod(data->mcp_updatekeys,MUIM_Notify, MUIA_Pressed, FALSE,
           data->mcp_listkeys, 3, MUIM_CallHook, &UpdateHook);

  return ((ULONG)obj);
}


ULONG mNL_MCP_ConfigToGadgets(struct IClass *cl,Object *obj,struct MUIP_Settingsgroup_ConfigToGadgets *msg)
{
  struct NListviews_MCP_Data *data = INST_DATA(cl, obj);

  /*D(bug("GadgetsToConfig\n"));*/

  LOAD_DATASPEC(data->mcp_PenTitle,   MUIA_Pendisplay_Spec,  MUICFG_NList_Pen_Title,   DEFAULT_PEN_TITLE);
  LOAD_DATASPEC(data->mcp_PenList,    MUIA_Pendisplay_Spec,  MUICFG_NList_Pen_List,    DEFAULT_PEN_LIST);
  LOAD_DATASPEC(data->mcp_PenSelect,  MUIA_Pendisplay_Spec,  MUICFG_NList_Pen_Select,  DEFAULT_PEN_SELECT);
  LOAD_DATASPEC(data->mcp_PenCursor,  MUIA_Pendisplay_Spec,  MUICFG_NList_Pen_Cursor,  DEFAULT_PEN_CURSOR);
  LOAD_DATASPEC(data->mcp_PenUnselCur,MUIA_Pendisplay_Spec,  MUICFG_NList_Pen_UnselCur,DEFAULT_PEN_UNSELCUR);

  LOAD_DATASPEC(data->mcp_BG_Title,   MUIA_Imagedisplay_Spec,MUICFG_NList_BG_Title,    DEFAULT_BG_TITLE);
  LOAD_DATASPEC(data->mcp_BG_List,    MUIA_Imagedisplay_Spec,MUICFG_NList_BG_List,     DEFAULT_BG_LIST);
  LOAD_DATASPEC(data->mcp_BG_Select,  MUIA_Imagedisplay_Spec,MUICFG_NList_BG_Select,   DEFAULT_BG_SELECT);
  LOAD_DATASPEC(data->mcp_BG_Cursor,  MUIA_Imagedisplay_Spec,MUICFG_NList_BG_Cursor,   DEFAULT_BG_CURSOR);
  LOAD_DATASPEC(data->mcp_BG_UnselCur,MUIA_Imagedisplay_Spec,MUICFG_NList_BG_UnselCur, DEFAULT_BG_UNSELCUR);

  LOAD_DATALONG(data->mcp_SL_VertInc, MUIA_Numeric_Value,    MUICFG_NList_VertInc,     DEFAULT_VERT_INC);

  LOAD_DATALONG(data->mcp_B_Smooth,   MUIA_Selected,         MUICFG_NList_Smooth,      FALSE);

  LOAD_DATAFONT(data->mcp_Font,       MUICFG_NList_Font);
  LOAD_DATAFONT(data->mcp_Font_Little,MUICFG_NList_Font_Little);
  LOAD_DATAFONT(data->mcp_Font_Fixed, MUICFG_NList_Font_Fixed);

  {
    LONG *ptrd;
    LONG num = 0;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_DragType)))
    {
      if      (*ptrd == MUIV_NList_DragType_Qualifier)
        num = 2;
      else if (*ptrd == MUIV_NList_DragType_Borders)
        num = 1;
      else
        num = 0;
    }
    set(data->mcp_R_Drag,MUIA_Radio_Active, num);

  }

  {
    LONG *ptrd;
    LONG num = 1;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_StackCheck)))
    {
      if      (*ptrd == 0)
        num = 0;
      else if (*ptrd == 2)
        num = 2;
      else
        num = 1;
    }
    set(data->mcp_StackCheck,MUIA_Radio_Active, num);
  }

  {
    LONG *ptrd;
    LONG num = DEFAULT_CWD;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_ColWidthDrag)))
    {
      if      (*ptrd <= 0)
        num = 0;
      else if (*ptrd == 2)
        num = 2;
      else
        num = 1;
    }
    set(data->mcp_ColWidthDrag,MUIA_Radio_Active, num);
  }

  {
    LONG *ptrd;
    LONG num = 0;

    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_MultiSelect)))
      num = *ptrd;

    if ((num & MUIV_NList_MultiSelect_MMB_On) == MUIV_NList_MultiSelect_MMB_On)
    {
      set(data->mcp_B_MultiMMB, MUIA_Selected, TRUE);
    }
    else
    {
      set(data->mcp_B_MultiMMB, MUIA_Selected, FALSE);
    }
    num &= 0x0007;
    if (num == MUIV_NList_MultiSelect_Always)
      num = 1;
    else
      num = 0;
    set(data->mcp_R_Multi,MUIA_Radio_Active, num);
  }

  {
    LONG *ptrd;
    LONG num = DEFAULT_HSB;

    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListview_HSB)))
      num = *ptrd;

    if ((num < 1) || (num > 4))
      num = DEFAULT_HSB;
    num--;

    set(data->mcp_R_HSB,MUIA_Radio_Active, num);
  }

  {
    LONG *ptrd;
    LONG num = 1;

    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListview_VSB)))
      num = *ptrd;

    if ((num < 1) || (num > 3))
      num = 1;
    num--;
    set(data->mcp_R_VSB,MUIA_Radio_Active, num);
  }

  {
    LONG *ptrd;
    LONG num = FALSE;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_ForcePen)))
    {
      if (*ptrd)
        num = TRUE;
      else
        num = FALSE;
    }
    set(data->mcp_ForcePen, MUIA_Selected, num);
  }

  LOAD_DATALONG(data->mcp_DragLines,    MUIA_Numeric_Value,    MUICFG_NList_DragLines, DEFAULT_DRAGLINES);
  LOAD_DATALONG(data->mcp_PointerColor, MUIA_Numeric_Value,    MUICFG_NList_PointerColor, 2);
  LOAD_DATALONG(data->mcp_WheelStep,    MUIA_Numeric_Value,    MUICFG_NList_WheelStep, 1);
  LOAD_DATALONG(data->mcp_WheelFast,    MUIA_Numeric_Value,    MUICFG_NList_WheelFast, 5);
  LOAD_DATALONG(data->mcp_WheelMMB,     MUIA_Selected,         MUICFG_NList_WheelMMB,  FALSE);

  {
    LONG *ptrd;
    LONG num = FALSE;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_SerMouseFix)))
    {
      if (*ptrd)
        num = TRUE;
      else
        num = FALSE;
    }
    set(data->mcp_SerMouseFix, MUIA_Selected, num);
  }

  {
    LONG *ptrd;
    LONG num = TRUE;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_List_Select)))
    {
      if (*ptrd)
        num = TRUE;
      else
        num = FALSE;
    }
    set(data->mcp_List_Select, MUIA_Selected, num);
  }

  {
    LONG *ptrd;
    LONG num = TRUE;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_PartialCol)))
    {
      if (*ptrd)
        num = TRUE;
      else
        num = FALSE;
    }
    set(data->mcp_PartialCol, MUIA_Selected, num);
  }

  {
    LONG *ptrd;
    LONG num = FALSE;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_PartialChar)))
    {
      if (*ptrd)
        num = TRUE;
      else
        num = FALSE;
    }
    set(data->mcp_PartialChar, MUIA_Selected, num);
  }

  {
    LONG *ptrd;
    LONG num = 0;
    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_Menu)))
    {
      if      (*ptrd == MUIV_NList_ContextMenu_TopOnly)
        num = 1;
      else if (*ptrd == MUIV_NList_ContextMenu_Never)
        num = 2;
    }
    set(data->mcp_NList_Menu, MUIA_Radio_Active, num);
  }

  {
    LONG *ptrd;
    struct KeyBinding *keys = default_keys;

    if((ptrd = (LONG *) DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NList_Keys)))
      keys = (struct KeyBinding *) ptrd;

    NL_LoadKeys(data->mcp_listkeys,keys);
  }

  return(0);
}


ULONG mNL_MCP_GadgetsToConfig(struct IClass *cl,Object *obj,struct MUIP_Settingsgroup_GadgetsToConfig *msg)
{
  struct NListviews_MCP_Data *data = INST_DATA(cl, obj);

  /*D(bug("GadgetsToConfig\n"));*/

  SAVE_DATASPEC(data->mcp_PenTitle,   MUIA_Pendisplay_Spec,   MUICFG_NList_Pen_Title);
  SAVE_DATASPEC(data->mcp_PenList,    MUIA_Pendisplay_Spec,   MUICFG_NList_Pen_List);
  SAVE_DATASPEC(data->mcp_PenSelect,  MUIA_Pendisplay_Spec,   MUICFG_NList_Pen_Select);
  SAVE_DATASPEC(data->mcp_PenCursor,  MUIA_Pendisplay_Spec,   MUICFG_NList_Pen_Cursor);
  SAVE_DATASPEC(data->mcp_PenUnselCur,MUIA_Pendisplay_Spec,   MUICFG_NList_Pen_UnselCur);

  SAVE_DATASPEC2(data->mcp_BG_Title,   MUIA_Imagedisplay_Spec,MUICFG_NList_BG_Title);
  SAVE_DATASPEC2(data->mcp_BG_List,    MUIA_Imagedisplay_Spec,MUICFG_NList_BG_List);
  SAVE_DATASPEC2(data->mcp_BG_Select,  MUIA_Imagedisplay_Spec,MUICFG_NList_BG_Select);
  SAVE_DATASPEC2(data->mcp_BG_Cursor,  MUIA_Imagedisplay_Spec,MUICFG_NList_BG_Cursor);
  SAVE_DATASPEC2(data->mcp_BG_UnselCur,MUIA_Imagedisplay_Spec,MUICFG_NList_BG_UnselCur);

  SAVE_DATALONG(data->mcp_SL_VertInc, MUIA_Numeric_Value,     MUICFG_NList_VertInc);

  SAVE_DATALONG(data->mcp_B_Smooth,   MUIA_Selected,          MUICFG_NList_Smooth);

  SAVE_DATAFONT(data->mcp_Font,       MUICFG_NList_Font);
  SAVE_DATAFONT(data->mcp_Font_Little,MUICFG_NList_Font_Little);
  SAVE_DATAFONT(data->mcp_Font_Fixed, MUICFG_NList_Font_Fixed);

  {
    LONG ptrd,num;
    get(data->mcp_R_Drag, MUIA_Radio_Active, &ptrd);
    if      (ptrd == 2)
      num = MUIV_NList_DragType_Qualifier;
    else if (ptrd == 1)
      num = MUIV_NList_DragType_Borders;
    else
      num = MUIV_NList_DragType_Immediate;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_DragType);
  }

  {
    LONG ptrd,num;
    get(data->mcp_StackCheck, MUIA_Radio_Active, &ptrd);
    num = ptrd;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_StackCheck);
  }

  {
    LONG ptrd,num;
    get(data->mcp_ColWidthDrag, MUIA_Radio_Active, &ptrd);
    num = ptrd;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_ColWidthDrag);
  }

  {
    LONG ptrd,num;
    get(data->mcp_R_Multi, MUIA_Radio_Active, &ptrd);
    if (ptrd == 1)
      num = MUIV_NList_MultiSelect_Always;
    else
      num = MUIV_NList_MultiSelect_Shifted;
    get(data->mcp_B_MultiMMB, MUIA_Selected, &ptrd);
    if (ptrd)
      num |= MUIV_NList_MultiSelect_MMB_On;
    else
      num |= MUIV_NList_MultiSelect_MMB_Off;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_MultiSelect);
  }

  {
    LONG ptrd,num;
    get(data->mcp_R_HSB, MUIA_Radio_Active, &ptrd);
    num = ptrd+1;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NListview_HSB);
  }
  {
    LONG ptrd,num;
    get(data->mcp_R_VSB, MUIA_Radio_Active, &ptrd);
    num = ptrd+1;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NListview_VSB);
  }

  {
    LONG ptrd,num;
    get(data->mcp_ForcePen, MUIA_Selected, &ptrd);
    if (ptrd)
      num = TRUE;
    else
      num = FALSE;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_ForcePen);
  }

  SAVE_DATALONG(data->mcp_DragLines,    MUIA_Numeric_Value,     MUICFG_NList_DragLines);

  SAVE_DATALONG(data->mcp_PointerColor, MUIA_Numeric_Value,     MUICFG_NList_PointerColor);

  SAVE_DATALONG(data->mcp_WheelStep,    MUIA_Numeric_Value,     MUICFG_NList_WheelStep);

  SAVE_DATALONG(data->mcp_WheelFast,    MUIA_Numeric_Value,     MUICFG_NList_WheelFast);

  SAVE_DATALONG(data->mcp_WheelMMB,     MUIA_Selected,          MUICFG_NList_WheelMMB);

  {
    LONG ptrd,num;
    get(data->mcp_SerMouseFix, MUIA_Selected, &ptrd);
    if (ptrd)
      num = TRUE;
    else
      num = FALSE;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_SerMouseFix);
  }

  {
    LONG ptrd,num;
    get(data->mcp_List_Select, MUIA_Selected, &ptrd);
    if (ptrd)
      num = TRUE;
    else
      num = FALSE;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_List_Select);
  }

  {
    LONG ptrd,num;
    get(data->mcp_PartialCol, MUIA_Selected, &ptrd);
    if (ptrd)
      num = TRUE;
    else
      num = FALSE;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_PartialCol);
  }

  {
    LONG ptrd,num;
    get(data->mcp_PartialChar, MUIA_Selected, &ptrd);
    if (ptrd)
      num = TRUE;
    else
      num = FALSE;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_PartialChar);
  }

  {
    LONG ptrd,num = MUIV_NList_ContextMenu_Always;
    get(data->mcp_NList_Menu, MUIA_Radio_Active, &ptrd);
    if      (ptrd == 1)
      num = MUIV_NList_ContextMenu_TopOnly;
    else if (ptrd == 2)
      num = MUIV_NList_ContextMenu_Never;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &num, 8, MUICFG_NList_Menu);
  }

  {
    LONG sk = NL_SaveKeys(data);
    if (sk > 0)
    { DoMethod(msg->configdata, MUIM_Dataspace_Add, data->nlkeys, sk, MUICFG_NList_Keys);
      FreeMem((void *) data->nlkeys,data->nlkeys_size+4);
      data->nlkeys = NULL;
    }
  }

  return(0);
}


static ULONG mNL_MCP_Get(struct IClass *cl,Object *obj,Msg msg)
{
  ULONG *store = ((struct opGet *)msg)->opg_Storage;

  switch (((struct opGet *)msg)->opg_AttrID)
  {
    case MUIA_Version:
      *store = LIB_VERSION;
      return(TRUE);
      break;
    case MUIA_Revision:
      *store = LIB_REVISION;
      return(TRUE);
      break;
  }
  return (DoSuperMethodA(cl,obj,msg));
}

DISPATCHERPROTO(_DispatcherP)
{
  DISPATCHER_INIT
  
  switch (msg->MethodID)
  {
    case OM_NEW:                             return(mNL_MCP_New(cl,obj,(APTR)msg));
    case MUIM_Settingsgroup_ConfigToGadgets: return(mNL_MCP_ConfigToGadgets(cl,obj,(APTR)msg));
    case MUIM_Settingsgroup_GadgetsToConfig: return(mNL_MCP_GadgetsToConfig(cl,obj,(APTR)msg));
    case OM_GET:                             return(mNL_MCP_Get(cl,obj,(APTR)msg));
  }

  return(DoSuperMethodA(cl,obj,msg));
  
  DISPATCHER_EXIT
}

