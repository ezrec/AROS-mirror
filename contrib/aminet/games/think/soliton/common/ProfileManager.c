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

#include <stdlib.h>
#include <string.h>

#include <clib/alib_protos.h>
#include <exec/memory.h>
#include <libraries/iffparse.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "Cardgame.h"
#include "Locales.h"
#include "ProfileManager.h"

#define USE_IMG_PROFILES_COLORS
#include "IMG_Profiles.c"

struct Profil
{
  char name[30];
  char cardset[255];
  char pattern[255];
};

struct ProfileManager_Data
{
  Object * ST_Cardset;
  Object * ST_Pattern;
  Object * ST_Name;
  Object * LV_Profiles;
  Object * menu;
  Object * cardgame;
  long     menusize;
};

static const char HID[]   = "Soliton Profiles";
static const char FNAME[] = "PROGDIR:Soliton.profiles";

static void buildMenu(struct ProfileManager_Data *data)
{
  struct Profil* p;
  Object* item;
  LONG i, sel;

  for(i = 0; i < data->menusize; i++)
  {
    item = MenuObj(data->menu, i + 1);
    if(item)
      DoMethod(data->menu, MUIM_Family_Remove, item);
  }

  data->menusize = xget(data->LV_Profiles, MUIA_List_Entries);
  sel = xget(data->LV_Profiles, MUIA_List_Active);

  for(i = 0; i < data->menusize; i++)
  {
    DoMethod(data->LV_Profiles, MUIM_List_GetEntry, i, &p);
    item =  MenuitemObject,
              MUIA_Menuitem_Title  , p->name,
              MUIA_UserData        , i + 1,
              MUIA_Menuitem_Exclude, (~(1 << (i+2))),
              MUIA_Menuitem_Checkit, TRUE,
              MUIA_Menuitem_Checked, (i == sel),
              End;
    DoMethod(data->menu, MUIM_Family_AddTail, item);
    DoMethod(item, MUIM_Notify, MUIA_Menuitem_Trigger, 
             MUIV_EveryTime, data->LV_Profiles, 3, MUIM_Set, MUIA_List_Active, i);
  }
}

static void submitCardgame(struct ProfileManager_Data *data)
{
  if(data->cardgame)
  {
    struct Profil * p = NULL;
    DoMethod(data->LV_Profiles, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &p);

    if(p)
      DoMethod(data->cardgame, MUIM_Cardgame_SetGraphic, p->cardset, p->pattern);
    else
      DoMethod(data->cardgame, MUIM_Cardgame_SetGraphic, "", ""); 
  }
}

/****************************************************************************************
  Open / Close / LoadProfile
****************************************************************************************/

static IPTR ProfileManager_Open(/*struct IClass* cl,*/ Object* obj/*, Msg msg*/)
{
  setatt(_app(obj), MUIA_Application_Sleep, TRUE);
  setatt(obj, MUIA_Window_Open, TRUE);
  return 0;
}

static IPTR ProfileManager_Close(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct ProfileManager_Data* data = (struct ProfileManager_Data *) INST_DATA(cl, obj);
  setatt(obj, MUIA_Window_Open, FALSE);
  buildMenu(data);
  setatt(_app(obj), MUIA_Application_Sleep, FALSE);
  submitCardgame(data);
  return 0;
}

/****************************************************************************************
  LoadProfile
****************************************************************************************/

BOOL getlin(BPTR f, char * buf, ULONG size)
{
  char *b;

  if(!FGets(f, (STRPTR) buf, size-1))
    return FALSE;
  for(b = buf; *b; ++b)
  {
    if(*b == '\n')
      *b = 0;
  }
  return TRUE;
}

static IPTR ProfileManager_LoadProfiles(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct ProfileManager_Data *data = (struct ProfileManager_Data *) INST_DATA(cl, obj);
  char id[300];
  LONG sel, ok = 0;
  struct Profil *p = 0;
  BPTR f;
  
  if((f = Open(FNAME, MODE_OLDFILE)))
  {
    if(getlin(f, id, sizeof(id)) && !strcmp(id, HID) && getlin(f, id, sizeof(id)))
    {
      /* ignore size for loading */
      sel = strtol(id+5, 0, 10);

      while(ok >= 0 && getlin(f, id, sizeof(id)))
      {
        if(!id[0])
          ; /* ignore empty lines */
        else if(id[1] == '=')
        {
          switch(id[0])
          {
          case 'N':
            if(!(p = (struct Profil *) AllocVec(sizeof(struct Profil), MEMF_PUBLIC|MEMF_CLEAR)))
              ok = -1;
            else
            {
              DoMethod(data->LV_Profiles, MUIM_List_InsertSingle, p, MUIV_List_Insert_Bottom);
              ++ok;
              strncpy(p->name, id+2, sizeof(p->name));
            }
            break;
          case 'C':
            if(p)
              strncpy(p->cardset, id+2, sizeof(p->cardset));
            break;
          case 'P':
            if(p)
              strncpy(p->pattern, id+2, sizeof(p->pattern));
            break;
          }
        }
        else
          ok = -1;
      }
      setatt(data->LV_Profiles, MUIA_List_Active, sel < ok ? sel : 1);
    }
    Close(f);
  }

  if(!ok)
  {
    DoMethod(data->LV_Profiles, MUIM_List_Clear);

    if((p = (struct Profil *) AllocVec(sizeof(struct Profil), MEMF_PUBLIC)))
    {
      strncpy(p->name, GetStr(MSG_CARDS_STANDARD_BIG), sizeof(p->name));
      strcpy(p->cardset, "Graphics/Cards_Standard Big.iff");
      strcpy(p->pattern, "Graphics/Pattern_Standard.iff");
      DoMethod(data->LV_Profiles, MUIM_List_InsertSingle, p, MUIV_List_Insert_Bottom);
    }

    if((p = (struct Profil *) AllocVec(sizeof(struct Profil), MEMF_PUBLIC)))
    {
      strncpy(p->name, GetStr(MSG_CARDS_STANDARD_MEDIUM), sizeof(p->name));
#ifdef __AROS__
      strcpy(p->cardset, "Graphics/Cards_Standard_Medium.iff");
      strcpy(p->pattern, "Graphics/Pattern_Standard.iff");
#else
      strcpy(p->cardset, "Graphics/Cards_Standard Medium.iff");
      strcpy(p->pattern, "Graphics/Pattern_Standard.iff");
#endif
      DoMethod(data->LV_Profiles, MUIM_List_InsertSingle, p, MUIV_List_Insert_Bottom);
    }

    if((p = (struct Profil *) AllocVec(sizeof(struct Profil), MEMF_PUBLIC)))
    {
      strncpy(p->name, GetStr(MSG_CARDS_STANDARD_SMALL), sizeof(p->name));
      strcpy(p->cardset, "Graphics/Cards_Standard Small.iff");
      strcpy(p->pattern, "Graphics/Pattern_Standard.iff");
      DoMethod(data->LV_Profiles, MUIM_List_InsertSingle, p, MUIV_List_Insert_Bottom);
    }

    setatt(data->LV_Profiles, MUIA_List_Active, 1);
  }

  buildMenu(data);
  return 0;
}

/******************************************************************************
  DisplayFunc / DestructFunc
******************************************************************************/

HOOKPROTONH(DisplayFunc, LONG, char** array, struct Profil* entry)
{
  if(entry)
    *array = entry->name;
  return 0;
}

HOOKPROTONHNO(DestructFunc, LONG, struct Profil* entry)
{
  if(entry)
    FreeVec(entry);
  return 0;
}

MakeStaticHook(DispHook, DisplayFunc);
MakeStaticHook(DestructHook, DestructFunc);

/****************************************************************************************
  Add / Rem / Redefine / Select
****************************************************************************************/

static IPTR ProfileManager_Add(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct ProfileManager_Data* data = (struct ProfileManager_Data *) INST_DATA(cl, obj);
  struct Profil *p;

  if((p = (struct Profil *) AllocVec(sizeof(struct Profil), MEMF_PUBLIC)))
  {
    strncpy(p->name, GetStr(MSG_SETTINGS_NOPROFILE), sizeof(p->name));
    p->cardset[0] = 0;
    p->pattern[0] = 0;
    DoMethod(data->LV_Profiles, MUIM_List_InsertSingle, p, MUIV_List_Insert_Bottom);
    setatt(data->LV_Profiles, MUIA_List_Active, MUIV_List_Active_Bottom);
    return TRUE;
  }

  return FALSE;
}

static IPTR ProfileManager_Rem(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct ProfileManager_Data* data = (struct ProfileManager_Data *) INST_DATA(cl, obj);
  DoMethod(data->LV_Profiles, MUIM_List_Remove, MUIV_List_Remove_Active);
  return 0;
}

static IPTR ProfileManager_Redefine(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct ProfileManager_Data* data = (struct ProfileManager_Data *) INST_DATA(cl, obj);

  struct Profil* p = NULL;
  DoMethod(data->LV_Profiles, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &p);

  if(p)
  {
    strncpy(p->name   , (char*)xget(data->ST_Name   , MUIA_String_Contents), sizeof(p->name));
    strncpy(p->cardset, (char*)xget(data->ST_Cardset, MUIA_String_Contents), sizeof(p->cardset));
    strncpy(p->pattern, (char*)xget(data->ST_Pattern, MUIA_String_Contents), sizeof(p->pattern));

    DoMethod(data->LV_Profiles, MUIM_List_Redraw, MUIV_List_Redraw_Active, &p);
  }

  return 0;
}

static IPTR ProfileManager_Select(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct ProfileManager_Data* data = (struct ProfileManager_Data *) INST_DATA(cl, obj);
  struct Profil* p = NULL;

  DoMethod(data->LV_Profiles, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &p);

  if(p)
  {
    setatt(data->ST_Name   , MUIA_String_Contents, p->name   );
    setatt(data->ST_Cardset, MUIA_String_Contents, p->cardset);
    setatt(data->ST_Pattern, MUIA_String_Contents, p->pattern);
  }
  else
  {
    setatt(data->ST_Name   , MUIA_String_Contents, "");
    setatt(data->ST_Cardset, MUIA_String_Contents, "");
    setatt(data->ST_Pattern, MUIA_String_Contents, "");
  }

  if(!xget(obj, MUIA_Window_Open))
    submitCardgame(data);

  return 0;
}


/****************************************************************************************
  Set / Get
****************************************************************************************/

static IPTR ProfileManager_Set(struct IClass* cl, Object* obj, struct opSet* msg)
{
  struct ProfileManager_Data* data = (struct ProfileManager_Data *) INST_DATA(cl, obj);
  struct TagItem *tag;

  if((tag = FindTagItem(MUIA_ProfileManager_ProfileMenu, msg->ops_AttrList)))
  {
    data->menu = (Object*)tag->ti_Data;
    return TRUE;
  }

  if((tag = FindTagItem(MUIA_ProfileManager_Cardgame, msg->ops_AttrList)))
  {
    data->cardgame = (Object*)tag->ti_Data;
    return TRUE;
  }

  return DoSuperMethodA(cl, obj, (Msg)msg);
}

/****************************************************************************************
  New / Dispose
****************************************************************************************/

static IPTR ProfileManager_New(struct IClass* cl, Object* obj, struct opSet* msg)
{
  Object *BT_Close, *BT_Add, *BT_Rem;
  struct ProfileManager_Data tmp;

  obj = (Object*)DoSuperNew(cl, obj,
    MUIA_Window_Title      , GetStr(MSG_PROFILES_TITLE),
    MUIA_Window_ID         , MAKE_ID('P','R','M','G'),
    MUIA_Window_CloseGadget, FALSE,
    MUIA_HelpNode          , "SETTINGS",
    WindowContents, VGroup,

      Child, HGroup,

        Child, MakeImage(IMG_Profiles_body, IMG_PROFILES_WIDTH, IMG_PROFILES_HEIGHT, 
                         IMG_PROFILES_DEPTH, IMG_PROFILES_COMPRESSION, IMG_PROFILES_MASKING, 
                         IMG_Profiles_colors),
        Child, TextObject,
          MUIA_Text_Contents, GetStr(MSG_PROFILES_TOPIC),
          MUIA_Font         , MUIV_Font_Big,
          MUIA_Text_SetMax  , FALSE,
          End,
        End,


      Child, HGroup,
        
        Child, tmp.LV_Profiles = ListviewObject,
          MUIA_Listview_Input , TRUE,
          MUIA_ShortHelp      , GetStr(MSG_SETTINGS_PROFILES_HELP),
          MUIA_Weight         , 80,
          MUIA_Listview_List  , ListObject, 
            InputListFrame,
            End,
          End,

        Child, VGroup,

          Child, HVSpace,

          Child, ColGroup(2),
            Child, MakeLabel2(MSG_SETTINGS_PROFILE),
            Child, tmp.ST_Name = MakeString(25, MSG_SETTINGS_PROFILE, "", MSG_SETTINGS_PROFILE_HELP),
            Child, MakeLabel2(MSG_SETTINGS_CARDSET),
            Child, PopaslObject,
              MUIA_Popstring_String, tmp.ST_Cardset = MakeString(250, MSG_SETTINGS_CARDSET, "", MSG_SETTINGS_CARDSET_HELP),
              MUIA_Popstring_Button, PopButton(MUII_PopFile),
              End,
            Child, MakeLabel2(MSG_SETTINGS_PATTERN),
            Child, PopaslObject,
              MUIA_Popstring_String, tmp.ST_Pattern = MakeString(250, MSG_SETTINGS_PATTERN, "", MSG_SETTINGS_PATTERN_HELP),
              MUIA_Popstring_Button, PopButton(MUII_PopFile),
              End,
            End,

          Child, HGroup,
            Child, BT_Add = MakeButton(MSG_PROFILES_ADD, MSG_PROFILES_ADD_HELP),
            Child, BT_Rem = MakeButton(MSG_PROFILES_REM, MSG_PROFILES_REM_HELP),
            End,

          Child, HVSpace,

          End,

        End,


      Child, HGroup,
        Child, HVSpace,
        Child, BT_Close   = MakeButton(MSG_PROFILES_CLOSE, 0),
        Child, HVSpace,
        End,

      End,

    TAG_MORE, msg->ops_AttrList);

  if (obj)
  {
    struct ProfileManager_Data* data = (struct ProfileManager_Data *) INST_DATA(cl, obj);
    *data = tmp;

    DoMethod(BT_Close , MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_ProfileManager_Close);
    DoMethod(BT_Add   , MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_ProfileManager_Add);
    DoMethod(BT_Rem   , MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_ProfileManager_Rem);
    setatt(data->LV_Profiles, MUIA_List_DisplayHook, &DispHook);
    setatt(data->LV_Profiles, MUIA_List_DestructHook, &DestructHook);

    DoMethod(data->LV_Profiles, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, 
             obj, 1, MUIM_ProfileManager_Select);

    DoMethod(tmp.ST_Name, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, 
            obj, 1, MUIM_ProfileManager_Redefine);
    DoMethod(tmp.ST_Cardset, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, 
            obj, 1, MUIM_ProfileManager_Redefine);
    DoMethod(tmp.ST_Pattern, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, 
            obj, 1, MUIM_ProfileManager_Redefine);

    data->menusize = 0;
    data->cardgame = NULL;

    return (IPTR)obj;
  }
  return 0;
}

/* Dispose and Save */
static IPTR ProfileManager_Dispose(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct ProfileManager_Data* data = (struct ProfileManager_Data *) INST_DATA(cl, obj);
  BPTR f;
  LONG size, i;
  struct Profil * p;

  if((f = Open(FNAME, MODE_NEWFILE)))
  {
    size = xget(data->LV_Profiles, MUIA_List_Entries);
    FPrintf(f, "%s\n%04ld %04ld\n", HID, size, xget(data->LV_Profiles, MUIA_List_Active));

    for(i = 0; i < size; i++)
    {
      DoMethod(data->LV_Profiles, MUIM_List_GetEntry, i, &p);
      FPrintf(f, "N=%s\nC=%s\n", p->name, p->cardset);
      if(p->pattern[0])
        FPrintf(f, "P=%s\n", p->pattern);
      FPrintf(f, "\n");
    }

    Close(f);
  }
  return 0;
}

/****************************************************************************************
  Dispatcher / Init / Exit
****************************************************************************************/

DISPATCHER(ProfileManager_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW     : return ProfileManager_New     (cl, obj, (struct opSet *)msg);
    case OM_DISPOSE : return ProfileManager_Dispose (cl, obj/*, msg*/);
    case OM_SET     : return ProfileManager_Set     (cl, obj, (struct opSet *)msg);
    case MUIM_ProfileManager_LoadProfiles : return ProfileManager_LoadProfiles  (cl, obj/*, msg*/);
    case MUIM_ProfileManager_Open         : return ProfileManager_Open    (/*cl,*/ obj/*, msg*/);
    case MUIM_ProfileManager_Close        : return ProfileManager_Close   (cl, obj/*, msg*/);
    case MUIM_ProfileManager_Redefine     : return ProfileManager_Redefine(cl, obj/*, msg*/);
    case MUIM_ProfileManager_Add          : return ProfileManager_Add     (cl, obj/*, msg*/);
    case MUIM_ProfileManager_Rem          : return ProfileManager_Rem     (cl, obj/*, msg*/);
    case MUIM_ProfileManager_Select       : return ProfileManager_Select  (cl, obj/*, msg*/);
  }
  return DoSuperMethodA(cl, obj, msg);
}

struct MUI_CustomClass *CL_ProfileManager = NULL;

BOOL ProfileManager_Init(void)
{
  if(!(CL_ProfileManager = MUI_CreateCustomClass(NULL, MUIC_Window, NULL,
  sizeof(struct ProfileManager_Data), ProfileManager_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_PROFILEMANAGERCLASS); 
    return FALSE;
  }
  return TRUE;
}

void ProfileManager_Exit(void)
{
  if(CL_ProfileManager)
    MUI_DeleteCustomClass(CL_ProfileManager);
}

