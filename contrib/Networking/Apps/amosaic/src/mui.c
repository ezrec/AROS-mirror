#include "includes.h"
#include <string.h>
#include "globals.h"
#include "HTML.h" /* For mosaic.h */
#include "mosaic.h"
#include "htmlgad.h"
#include "gui.h"
#include "XtoI.h"
#include "protos.h"
#include "spinner.h"
#include "Boingtransfer.h"

#include "image_back.h"
#include "image_forward.h"
#include "image_reload.h"
#include "image_home.h"
#include "image_open.h"
#include "image_amiga.h"
#include "image_amosaic.h"

#include "amosaic_cat.h"

extern AppData Rdata;

#define MUIA_Prop_Delta                 0x8042c5f4 /* is. LONG              */
#ifndef MUIA_Prop_DeltaFactor
#define MUIA_Prop_DeltaFactor           0x80427c5e /* isg LONG              */
#endif
#ifndef MUIA_FillArea
#define MUIA_FillArea 0x804294A3 /* V4 BOOL (private) */
#endif

#define mui_version (MUIMasterBase->lib_Version)

extern char *amosaic_version;
extern char *amosaic_window_title;
extern struct Hook ScrollHook;
extern struct Hook LayoutHook;

#define SimpleButtonHelp(name,helptext)\
	TextObject,\
		ButtonFrame,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp,helptext,\
		End

extern int anchor_visited_predicate();
extern void ImageResolve();

extern mo_window window;

extern char * amosaic_about_version;

Object *DBG_Str_Gad;
Object *BT_Debug;
Object *IButtonBack;
Object *IButtonForward;
Object *IButtonReload;
Object *IButtonHome;
Object *IButtonOpen;
Object *IButtonAmiga;
Object *IButtonAMosaic;
Object *TButtonBack;
Object *TButtonForward;
Object *TButtonReload;
Object *TButtonHome;
Object *TButtonOpen;
Object *TButtonAmiga;
Object *TButtonAMosaic;
Object *LB_Title;
Object *TopGroup;
Object *TTopGroup;

Object *SearchIndex;
Object *ButtonGroup;

APTR MN_RexxMenu=NULL;

char *RexxMacroNames[10], ShortCuts[10][2];

///general Hotlist stuff (made by rs)
#include <time.h>

APTR BT_AddHotlist,PopURL,URLGroupList,URLList;
APTR BT_HotlistGroupParent,TX_HotlistGroupTitle,BT_HotlistGroupNew,BT_HotlistGroupDelete,BT_HotlistGroupEdit,BT_HotlistGroupSort,BT_HotlistGroupUp,BT_HotlistGroupDown;
APTR BT_HotlistNew,BT_HotlistDelete,BT_HotlistEdit,BT_HotlistSort,BT_HotlistMove,BT_HotlistUp,BT_HotlistDown;

struct HotlistEntry {
  char Title[128];
  char Location[500];
  char Date[30];
};

struct HotlistGroupEntry {
  int Depth;
  char Title[30],FullTitle[200];
  APTR ChildList;
  APTR LinkList;
  struct HotlistGroupEntry *Parent;
};   

void LoadHotlist(void);
void SaveHotlist(void);

struct HotlistGroupEntry GroupRootList,*CurrentShownURLGroup;

HOOKPROTO(URLStrObjFunc, long, Object *pop, Object *str)
{
  char *s = NULL;
  struct HotlistEntry *x;
  int i;
  get(str,MUIA_String_Contents,&s);
  for (i=0;;i++)
  {
    DoMethod(URLList,MUIM_List_GetEntry,i,&x);
    if(!x)
    {
      set(URLList,MUIA_List_Active,MUIV_List_Active_Off);
      break;
    }
    else if (!stricmp(x->Location,s))
    {
      set(URLList,MUIA_List_Active,i);
      DoMethod(URLList,MUIM_List_Jump,i);
      break;
    }
  }
  return(TRUE);
}

MakeHook(URLStrObjHook, URLStrObjFunc);

HOOKPROTO(URLObjStrFunc, void, Object *pop, Object *str)
{
  struct HotlistEntry *x;
  DoMethod(pop,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,&x);
  set(str,MUIA_String_Contents,x->Location);
  set(str,MUIA_String_Acknowledge,TRUE);
}

MakeHook(URLObjStrHook, URLObjStrFunc);

void ShowHotlist(struct HotlistGroupEntry *hge)
{
  long i,Count = 0;
  set(TX_HotlistGroupTitle,MUIA_Text_Contents,hge->FullTitle);
  set(URLList,MUIA_List_Quiet,TRUE);
  set(URLGroupList,MUIA_List_Quiet,TRUE);
  DoMethod(URLList,MUIM_List_Clear);
  DoMethod(URLGroupList,MUIM_List_Clear);
  get(hge->ChildList,MUIA_List_Entries,&Count);
  for(i=0;i<Count;i++)
  {
    struct HotlistGroupEntry *he;
    DoMethod(hge->ChildList,MUIM_List_GetEntry,i,&he);
    DoMethod(URLGroupList,MUIM_List_InsertSingle,he,MUIV_List_Insert_Bottom);
  }
  get(hge->LinkList,MUIA_List_Entries,&Count);
  for(i=0;i<Count;i++)
  {
    struct HotlistEntry *he;
    DoMethod(hge->LinkList,MUIM_List_GetEntry,i,&he);
    DoMethod(URLList,MUIM_List_InsertSingle,he,MUIV_List_Insert_Bottom);
  }
  set(URLList,MUIA_List_Quiet,FALSE);
  set(URLGroupList,MUIA_List_Quiet,FALSE);
  set(BT_HotlistGroupParent,MUIA_Disabled,hge->Parent?FALSE:TRUE);
  CurrentShownURLGroup=hge;    
}

HOOKPROTO(SelectHotlistConstructFunc, APTR, APTR pool, struct HotlistGroupEntry *hge)
{
  struct HotlistGroupEntry *new;
  if((new=AllocPooled(pool,sizeof(struct HotlistGroupEntry))))
  {
    *new=*hge;
    return(new);
  }
  return(NULL);
}
MakeHook(SelectHotlistConstructHook , SelectHotlistConstructFunc);


HOOKPROTO(SelectHotlistDestructFunc, VOID, APTR pool, struct HotlistGroupEntry *hge)
{
  FreePooled(pool,hge,sizeof(struct HotlistGroupEntry));
}
MakeHook(SelectHotlistDestructHook , SelectHotlistDestructFunc);


HOOKPROTO(SelectHotlistCompareFunc, LONG, struct HotlistGroupEntry *hge2, struct HotlistGroupEntry *hge1)
{
  return(stricmp(hge1->FullTitle,hge2->FullTitle));
}

MakeHook(SelectHotlistCompareHook , SelectHotlistCompareFunc);


HOOKPROTO(SelectHotlistDisplayFunc, LONG, char **array, struct HotlistGroupEntry *hge)
{
  *array  = hge->FullTitle[0]?hge->FullTitle:"Root";
  return(0);
}

MakeHook(SelectHotlistDisplayHook , SelectHotlistDisplayFunc);

void ScanHotlistGroups(APTR List,struct HotlistGroupEntry *hge,struct HotlistGroupEntry *HiddenGroup)
{
  long anz = 0,i;
  if(hge!=HiddenGroup)
    DoMethod(List,MUIM_List_InsertSingle,hge,MUIV_List_Insert_Sorted);
  get(hge->ChildList,MUIA_List_Entries,&anz);
  for(i=0;i<anz;i++)
  {
    struct HotlistGroupEntry *entry;
    DoMethod(hge->ChildList,MUIM_List_GetEntry,i,&entry);
    ScanHotlistGroups(List,entry,HiddenGroup);
  }
}

struct HotlistGroupEntry *SelectHotlistGroup(char *Title,struct HotlistGroupEntry *HiddenGroup)
{
  APTR win,LV_Groups,BT_Ok,BT_Cancel;
  BOOL edit_running=TRUE;
  struct HotlistGroupEntry *ret=NULL;
  ULONG signals;
  win=WindowObject,
    MUIA_Window_Title,Title,
    MUIA_Window_ID,MAKE_ID('S','H','L','G'),
    WindowContents,VGroup,
      Child,LV_Groups=ListviewObject,
        MUIA_Listview_List,ListObject,
          InputListFrame,
          MUIA_List_ConstructHook,&SelectHotlistConstructHook,
          MUIA_List_DestructHook, &SelectHotlistDestructHook,
          MUIA_List_DisplayHook,  &SelectHotlistDisplayHook,
          MUIA_List_CompareHook,  &SelectHotlistCompareHook,
          End,
        End,
      Child,HGroup,
        MUIA_Group_SameWidth,TRUE,
        Child,BT_Ok=SimpleButton(GetamosaicString(MSG_HOTLIST_EDIT_OK)),
        Child,BT_Cancel=SimpleButton(GetamosaicString(MSG_HOTLIST_EDIT_CANCEL)),
        End,
      End,
    End;

  ScanHotlistGroups(LV_Groups,&GroupRootList,HiddenGroup);

  if(win)
  {
    DoMethod(App,OM_ADDMEMBER,win);
    DoMethod(BT_Ok,MUIM_Notify,MUIA_Pressed,FALSE,App,2,MUIM_Application_ReturnID,2);
    DoMethod(BT_Cancel,MUIM_Notify,MUIA_Pressed,FALSE,App,2,MUIM_Application_ReturnID,1);
    DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,App,2,MUIM_Application_ReturnID,1);

    DoMethod(win,MUIM_Window_SetCycleChain,LV_Groups,BT_Ok,BT_Cancel,NULL);

    DoMethod(LV_Groups,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,App,2,MUIM_Application_ReturnID,2);

    set(App,MUIA_Application_Sleep,TRUE);
    set(win,MUIA_Window_Open,TRUE);

    while(edit_running)
    {
      switch(DoMethod(App,MUIM_Application_Input,&signals))
      {
        case MUIV_Application_ReturnID_Quit:
        case 1:
          ret=NULL;
          edit_running=FALSE;
          break;
        case 2:
          {
            DoMethod(LV_Groups,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,&ret);
            edit_running=FALSE;
          }
          break;
      }
      if(edit_running&&signals) Wait(signals);
    }
    set(win,MUIA_Window_Open,FALSE);
    set(App,MUIA_Application_Sleep,FALSE);
    DoMethod(App,OM_REMMEMBER,win);
    MUI_DisposeObject(win);
  }

  return(ret);
}
///
///HotlistLinkFunctions
HOOKPROTO(HotlistConstructFunc, APTR, APTR pool, struct HotlistEntry *he)
{
  struct HotlistEntry *new;
  if((new=AllocPooled(pool,sizeof(struct HotlistEntry))))
  {
    *new=*he;
    return(new);
  }
  return(NULL);
}

MakeHook(HotlistConstructHook , HotlistConstructFunc);


HOOKPROTO(HotlistDestructFunc, VOID, APTR pool, struct HotlistEntry *he)
{
  FreePooled(pool,he,sizeof(struct HotlistEntry));
}

MakeHook(HotlistDestructHook , HotlistDestructFunc);


HOOKPROTO(HotlistCompareFunc, LONG, struct HotlistEntry *he2, struct HotlistEntry *he1)
{
  return(stricmp(he1->Title,he2->Title));
}
MakeHook(HotlistCompareHook , HotlistCompareFunc);


HOOKPROTO(HotlistDisplayFunc, LONG, char **array, struct HotlistEntry *he)
{
  *array  = he->Title;
  return(0);
}
MakeHook(HotlistDisplayHook , HotlistDisplayFunc);


HOOKPROTO(HotlistButtonsFunc, void, Object *List, APTR *arg)
{
  long Count = 0,Selected = 0;
  get(List,MUIA_List_Entries,&Count);
  get(List,MUIA_List_Active,&Selected);
  if(Selected==-1)
  {
    set(BT_HotlistDelete,MUIA_Disabled,TRUE);
    set(BT_HotlistEdit,MUIA_Disabled,TRUE);
    set(BT_HotlistMove,MUIA_Disabled,TRUE);
    set(BT_HotlistUp,MUIA_Disabled,TRUE);
    set(BT_HotlistDown,MUIA_Disabled,TRUE);
  }
  else
  {
    set(BT_HotlistDelete,MUIA_Disabled,FALSE);
    set(BT_HotlistEdit,MUIA_Disabled,FALSE);
    set(BT_HotlistMove,MUIA_Disabled,FALSE);
    set(BT_HotlistUp,MUIA_Disabled,Selected<1?TRUE:FALSE);
    set(BT_HotlistDown,MUIA_Disabled,Selected==Count-1?TRUE:FALSE);
  }
}

MakeHook(HotlistButtonsHook, HotlistButtonsFunc);
 
BOOL EditHotlistEntry(struct HotlistEntry *Entry)
{
  APTR win,ST_Title,ST_Location,BT_Ok,BT_Cancel;
  BOOL edit_running=TRUE,ret=FALSE;
  ULONG signals;
  win=WindowObject,
    MUIA_Window_Title,GetamosaicString(MSG_HOTLIST_EDIT_WINDOW_TITLE),
    MUIA_Window_ID,MAKE_ID('H','L','S','T'),
    WindowContents,VGroup,
      Child,ColGroup(2),
        Child,Label2(GetamosaicString(MSG_HOTLIST_EDIT_TITLE)),
        Child,ST_Title=String(Entry->Title,128),
        Child,Label2(GetamosaicString(MSG_HOTLIST_EDIT_URL)),
        Child,ST_Location=String(Entry->Location,500),
        End,
      Child,HGroup,
        MUIA_Group_SameWidth,TRUE,
        Child,BT_Ok=SimpleButton(GetamosaicString(MSG_HOTLIST_EDIT_OK)),
        Child,BT_Cancel=SimpleButton(GetamosaicString(MSG_HOTLIST_EDIT_CANCEL)),
        End,
      End,
    End;               

  if(win)
  {
    DoMethod(App,OM_ADDMEMBER,win);
    DoMethod(BT_Ok,MUIM_Notify,MUIA_Pressed,FALSE,App,2,MUIM_Application_ReturnID,2);
    DoMethod(BT_Cancel,MUIM_Notify,MUIA_Pressed,FALSE,App,2,MUIM_Application_ReturnID,1);
    DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,App,2,MUIM_Application_ReturnID,1);

    DoMethod(win,MUIM_Window_SetCycleChain,ST_Title,ST_Location,BT_Ok,BT_Cancel,NULL);

    DoMethod(ST_Title,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,win,3,MUIM_Set,MUIA_Window_ActiveObject,ST_Location);
    DoMethod(ST_Location,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,win,3,MUIM_Set,MUIA_Window_ActiveObject,BT_Ok);

    set(win,MUIA_Window_ActiveObject,ST_Title);
    set(App,MUIA_Application_Sleep,TRUE);
    set(win,MUIA_Window_Open,TRUE);

    while(edit_running)
    {
      switch(DoMethod(App,MUIM_Application_Input,&signals))
      {
        case MUIV_Application_ReturnID_Quit:
        case 1:
          ret=FALSE;
          edit_running=FALSE;
          break;
        case 2:
          {
            char *Title = NULL,*Location = NULL;
            get(ST_Title,MUIA_String_Contents,&Title);
            get(ST_Location,MUIA_String_Contents,&Location);
            strcpy(Entry->Title,Title);
            strcpy(Entry->Location,Location);
            ret=TRUE;
            edit_running=FALSE;
          }
          break;
      }
      if(edit_running&&signals) Wait(signals);
    }
    set(win,MUIA_Window_Open,FALSE);
    set(App,MUIA_Application_Sleep,FALSE);
    DoMethod(App,OM_REMMEMBER,win);
    MUI_DisposeObject(win);
  }     

  return(ret);
}

HOOKPROTO(HotlistNewFunc, void, Object *List, APTR *arg)
{
  char *Title = NULL,*Location = NULL;
  struct HotlistEntry Entry;
  struct tm *p;
  time_t t;
  time(&t);
  p=localtime(&t);
  if(TX_Title)
    get(TX_Title,MUIA_Text_Contents,&Title);
  else
    get(WI_Main,MUIA_Window_Title,&Title);
  get(TX_URL,MUIA_String_Contents,&Location);
  strcpy(Entry.Title,Title);
  strcpy(Entry.Location,Location);
  strftime(&Entry.Date[0],30,"%a %b %d %H:%M:%S %Y",p);
  if(EditHotlistEntry(&Entry))
  {
    DoMethod(List,MUIM_List_InsertSingle,&Entry,MUIV_List_Insert_Bottom);
    DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_InsertSingle,&Entry,MUIV_List_Insert_Bottom);
    SaveHotlist();
  }
}
MakeHook(HotlistNewHook, HotlistNewFunc);


HOOKPROTO(HotlistDeleteFunc, void, Object *List, APTR *arg)
{
  long Active = 0;
  get(List,MUIA_List_Active,&Active);
  DoMethod(List,MUIM_List_Remove,Active);
  DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_Remove,Active);
  SaveHotlist();
}

MakeHook(HotlistDeleteHook, HotlistDeleteFunc);

HOOKPROTO(HotlistEditFunc, void, Object *List, APTR *arg)
{
  long Active = 0;
  struct HotlistEntry *Entry;
  get(List,MUIA_List_Active,&Active);
  DoMethod(List,MUIM_List_GetEntry,Active,&Entry);
  if(EditHotlistEntry(Entry))
  {
    DoMethod(List,MUIM_List_Redraw,MUIV_List_Redraw_Active);
    DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_Remove,Active);
    DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_InsertSingle,Entry,Active);
    SaveHotlist();
  }
}

MakeHook(HotlistEditHook, HotlistEditFunc);

HOOKPROTO(HotlistAddFunc, void, Object *List, APTR *arg)
{
  char *Title = NULL,*Location = NULL;
  struct HotlistEntry Entry;
  struct tm *p;
  time_t t;
  time(&t);
  p=localtime(&t);
  if(TX_Title)
    get(TX_Title,MUIA_Text_Contents,&Title);
  else
    get(WI_Main,MUIA_Window_Title,&Title);
  get(TX_URL,MUIA_String_Contents,&Location);
  strcpy(Entry.Title,Title);
  strcpy(Entry.Location,Location);
  strftime(&Entry.Date[0],30,"%a %b %d %H:%M:%S %Y",p);
  DoMethod(List,MUIM_List_InsertSingle,&Entry,MUIV_List_Insert_Bottom);
  DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_InsertSingle,&Entry,MUIV_List_Insert_Bottom);
  SaveHotlist();
}

MakeHook(HotlistAddHook, HotlistAddFunc);

HOOKPROTO(HotlistSortFunc, void, Object *List, APTR *arg)
{
  DoMethod(List,MUIM_List_Sort);
  DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_Sort);
  set(List,MUIA_List_Active,MUIV_List_Active_Off);
  SaveHotlist();
}

MakeHook(HotlistSortHook, HotlistSortFunc);

HOOKPROTO(HotlistMoveFunc, void, Object *List, APTR *arg)
{
  long Active = 0;
  struct HotlistEntry *he;
  struct HotlistGroupEntry *hge;

  get(URLList,MUIA_List_Active,&Active);
  if(Active!=-1)
  {
    DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_GetEntry,Active,&he);
    if((hge=SelectHotlistGroup("Where do you want to move this link?",CurrentShownURLGroup)))
    {
      DoMethod(hge->LinkList,MUIM_List_InsertSingle,he,MUIV_List_Insert_Bottom);
      DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_Remove,Active);
      ShowHotlist(CurrentShownURLGroup);
      SaveHotlist();
    }
  }
}

MakeHook(HotlistMoveHook, HotlistMoveFunc);

HOOKPROTO(HotlistUpFunc, void, Object *List, APTR *arg)
{
  long Count = 0,Selected = 0;
  get(List,MUIA_List_Entries,&Count);
  get(List,MUIA_List_Active,&Selected);
  if(Selected>0)
  {
    DoMethod(List,MUIM_List_Exchange,Selected,Selected-1);
    DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_Exchange,Selected,Selected-1);
    set(List,MUIA_List_Active,Selected-1);
    SaveHotlist();
  }
}

MakeHook(HotlistUpHook, HotlistUpFunc);


HOOKPROTO(HotlistDownFunc, void, Object *List, APTR *arg)
{
  long Count = 0,Selected = 0;
  get(List,MUIA_List_Entries,&Count);
  get(List,MUIA_List_Active,&Selected);
  if(Selected>-1&&Selected<Count-1)
  {
    DoMethod(List,MUIM_List_Exchange,Selected,Selected+1);
    DoMethod(CurrentShownURLGroup->LinkList,MUIM_List_Exchange,Selected,Selected+1);
    set(List,MUIA_List_Active,Selected+1);
    SaveHotlist();
  }  
}     
MakeHook(HotlistDownHook, HotlistDownFunc);
         
///
///HotlistGroupFunctions

HOOKPROTO(HotlistGroupDestructFunc, VOID, APTR pool, struct HotlistGroupEntry *hge)
{
  if(hge->ChildList)
  {
    DoMethod(hge->ChildList,MUIM_List_Clear,TRUE);
    MUI_DisposeObject(hge->ChildList);
  }
  if(hge->LinkList)
  {
    DoMethod(hge->LinkList,MUIM_List_Clear,TRUE);
    MUI_DisposeObject(hge->LinkList);
  }   
  FreePooled(pool,hge,sizeof(struct HotlistGroupEntry));
}
MakeHook(HotlistGroupDestructHook , HotlistGroupDestructFunc);
 
HOOKPROTO(HotlistGroupCompareFunc, LONG, struct HotlistGroupEntry *hge2, struct HotlistGroupEntry *hge1)
{
  return(stricmp(hge1->Title,hge2->Title));
}
MakeHook(HotlistGroupCompareHook , HotlistGroupCompareFunc);
 
 
HOOKPROTO(HotlistGroupConstructFunc, APTR, APTR pool, struct HotlistGroupEntry *hge)
{
  struct HotlistGroupEntry *new;
  if((new=AllocPooled(pool,sizeof(struct HotlistGroupEntry))))
  {
    *new=*hge;
    if((new->ChildList=ListObject,
      MUIA_List_ConstructHook,hook,
      MUIA_List_DestructHook, &HotlistGroupDestructHook,
      MUIA_List_CompareHook,  &HotlistGroupCompareHook,
      End))
    if((new->LinkList=ListObject,
      MUIA_List_ConstructHook,&HotlistConstructHook,
      MUIA_List_DestructHook, &HotlistDestructHook,
      MUIA_List_CompareHook,  &HotlistCompareHook,
      End))
    return(new);
    FreePooled(pool,hge,sizeof(struct HotlistGroupEntry));
  }
  return(NULL);
}

MakeHook(HotlistGroupConstructHook , HotlistGroupConstructFunc);


HOOKPROTO(HotlistGroupDisplayFunc, LONG, char **array, struct HotlistGroupEntry *hge)
{
  *array  = hge->Title;
  return(0);
}
MakeHook(HotlistGroupDisplayHook , HotlistGroupDisplayFunc);
 
 
HOOKPROTO(HotlistGroupButtonsFunc, void, Object *List, APTR *arg)
{
  long Count = 0,Selected = 0;
  get(List,MUIA_List_Active,&Selected);
  if(Selected==-1)
  {
    set(BT_HotlistGroupDelete,MUIA_Disabled,TRUE);
    set(BT_HotlistGroupEdit,MUIA_Disabled,TRUE);
    set(BT_HotlistGroupUp,MUIA_Disabled,TRUE);
    set(BT_HotlistGroupDown,MUIA_Disabled,TRUE); 
  }
  else
  {
    get(List,MUIA_List_Entries,&Count);
    set(BT_HotlistGroupDelete,MUIA_Disabled,FALSE);
    set(BT_HotlistGroupEdit,MUIA_Disabled,FALSE);
    set(BT_HotlistGroupUp,MUIA_Disabled,Selected<1?TRUE:FALSE);
    set(BT_HotlistGroupDown,MUIA_Disabled,Selected==Count-1?TRUE:FALSE);   
  }
}

MakeHook(HotlistGroupButtonsHook, HotlistGroupButtonsFunc);

HOOKPROTO(HotlistGroupSelectFunc, void, Object *List, APTR *arg)
{
  long Selected = 0;
  get(List,MUIA_List_Active,&Selected);
  if(Selected!=-1)
  {
    struct HotlistGroupEntry *hge;
    DoMethod(CurrentShownURLGroup->ChildList,MUIM_List_GetEntry,Selected,&hge);
    ShowHotlist(hge);
  }
}

MakeHook(HotlistGroupSelectHook, HotlistGroupSelectFunc);
 
HOOKPROTO(HotlistGroupParentFunc, void, Object *List, APTR *arg)
{
  if(CurrentShownURLGroup->Parent)
    ShowHotlist(CurrentShownURLGroup->Parent);
}

MakeHook(HotlistGroupParentHook, HotlistGroupParentFunc);

BOOL EditHotlistGroupEntry(struct HotlistGroupEntry *Entry)
{
  APTR win,ST_Title,BT_Ok,BT_Cancel;
  BOOL edit_running=TRUE,ret=FALSE;
  ULONG signals;
  win=WindowObject,
    MUIA_Window_Title,GetamosaicString(MSG_HOTLIST_EDITGROUP_TITLE),
    MUIA_Window_ID,MAKE_ID('H','L','S','G'),
    WindowContents,VGroup,
      Child,ColGroup(2),
        Child,Label2(GetamosaicString(MSG_HOTLIST_EDIT_TITLE)),
        Child,ST_Title=String(Entry->Title,128),
        End,
      Child,HGroup,
        MUIA_Group_SameWidth,TRUE,
        Child,BT_Ok=SimpleButton(GetamosaicString(MSG_HOTLIST_EDIT_OK)),
        Child,BT_Cancel=SimpleButton(GetamosaicString(MSG_HOTLIST_EDIT_CANCEL)),
        End,
      End,
    End;               

  if(win)
  {
    DoMethod(App,OM_ADDMEMBER,win);
    DoMethod(BT_Ok,MUIM_Notify,MUIA_Pressed,FALSE,App,2,MUIM_Application_ReturnID,2);
    DoMethod(BT_Cancel,MUIM_Notify,MUIA_Pressed,FALSE,App,2,MUIM_Application_ReturnID,1);
    DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,App,2,MUIM_Application_ReturnID,1);

    DoMethod(win,MUIM_Window_SetCycleChain,ST_Title,BT_Ok,BT_Cancel,NULL);

    DoMethod(ST_Title,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,win,3,MUIM_Set,MUIA_Window_ActiveObject,BT_Ok);

    set(win,MUIA_Window_ActiveObject,ST_Title);
    set(App,MUIA_Application_Sleep,TRUE);
    set(win,MUIA_Window_Open,TRUE);

    while(edit_running)
    {
      switch(DoMethod(App,MUIM_Application_Input,&signals))
      {
        case MUIV_Application_ReturnID_Quit:
        case 1:
          ret=FALSE;
          edit_running=FALSE;
          break;
        case 2:
          {
            char *Title = NULL;
            get(ST_Title,MUIA_String_Contents,&Title);
            strcpy(Entry->Title,Title);
            *PathPart(Entry->FullTitle)=0;
            AddPart(Entry->FullTitle,Entry->Title,200);
            ret=TRUE;
            edit_running=FALSE;
          }
          break;
      }
      if(edit_running&&signals) Wait(signals);
    }
    set(win,MUIA_Window_Open,FALSE);
    set(App,MUIA_Application_Sleep,FALSE);
    DoMethod(App,OM_REMMEMBER,win);
    MUI_DisposeObject(win);
  }     

  return(ret);
}

HOOKPROTO(HotlistGroupNewFunc, void, Object *List, APTR *arg)
{
  struct HotlistGroupEntry Entry;
  strcpy(Entry.Title,GetamosaicString(MSG_HOTLIST_GROUP_DEFAULTNAME));
  Entry.Parent=CurrentShownURLGroup;
  strcpy(Entry.FullTitle,Entry.Parent->Title);
  AddPart(Entry.FullTitle,Entry.Title,200);
  if(EditHotlistGroupEntry(&Entry))
  {
    DoMethod(List,MUIM_List_InsertSingle,&Entry,MUIV_List_Insert_Bottom);
    DoMethod(CurrentShownURLGroup->ChildList,MUIM_List_InsertSingle,&Entry,MUIV_List_Insert_Bottom);
    SaveHotlist();
  }
}
MakeHook(HotlistGroupNewHook, HotlistGroupNewFunc);


HOOKPROTO(HotlistGroupDeleteFunc, void, Object *List, APTR *arg)
{
  long Active = 0;
  get(List,MUIA_List_Active,&Active);
  DoMethod(List,MUIM_List_Remove,Active);
  DoMethod(CurrentShownURLGroup->ChildList,MUIM_List_Remove,Active);
  SaveHotlist();
}     
MakeHook(HotlistGroupDeleteHook, HotlistGroupDeleteFunc);

HOOKPROTO(HotlistGroupEditFunc, void, Object *List, APTR *arg)
{
  long Active = 0;
  struct HotlistGroupEntry *Entry;
  get(List,MUIA_List_Active,&Active);
  DoMethod(List,MUIM_List_GetEntry,Active,&Entry);
  if(EditHotlistGroupEntry(Entry))
  {
    DoMethod(List,MUIM_List_Redraw,MUIV_List_Redraw_Active);
    DoMethod(CurrentShownURLGroup->ChildList,MUIM_List_Remove,Active);
    DoMethod(CurrentShownURLGroup->ChildList,MUIM_List_InsertSingle,Entry,Active);
    SaveHotlist();
  }
}     
MakeHook(HotlistGroupEditHook, HotlistGroupEditFunc);

HOOKPROTO(HotlistGroupSortFunc, void, Object *List, APTR *arg)
{
  DoMethod(List,MUIM_List_Sort);
  DoMethod(CurrentShownURLGroup->ChildList,MUIM_List_Sort);
  set(List,MUIA_List_Active,MUIV_List_Active_Off);
  SaveHotlist();
}     
MakeHook(HotlistGroupSortHook, HotlistGroupSortFunc);

void CopyHotlistGroup(struct HotlistGroupEntry *source,struct HotlistGroupEntry *dest)
{
  long anz = 0,i;
  get(source->LinkList,MUIA_List_Entries,&anz);
  for(i=0;i<anz;i++)
  {
    struct HotlistEntry *entry;
    DoMethod(source->LinkList,MUIM_List_GetEntry,i,&entry);
    DoMethod(dest->LinkList,MUIM_List_InsertSingle,entry,MUIV_List_Insert_Bottom);
  }
  get(source->ChildList,MUIA_List_Entries,&anz);
  for(i=0;i<anz;i++)
  {
    struct HotlistGroupEntry *entry,*newdest;
    DoMethod(source->ChildList,MUIM_List_GetEntry,i,&entry);
    DoMethod(dest->ChildList,MUIM_List_InsertSingle,entry,MUIV_List_Insert_Bottom);
    DoMethod(dest->ChildList,MUIM_List_GetEntry,i,&newdest);
    CopyHotlistGroup(entry,newdest);
  }
}

HOOKPROTO(HotlistGroupUpFunc, void, Object *List, APTR *arg)
{
  long Count = 0,Selected = 0;
  get(List,MUIA_List_Entries,&Count);
  get(List,MUIA_List_Active,&Selected);
  if(Selected>0)
  {
    DoMethod(List,MUIM_List_Exchange,Selected,Selected-1);
    DoMethod(CurrentShownURLGroup->ChildList,MUIM_List_Exchange,Selected,Selected-1);
    set(List,MUIA_List_Active,Selected-1);
    SaveHotlist();
  }
}     
MakeHook(HotlistGroupUpHook, HotlistGroupUpFunc);

HOOKPROTO(HotlistGroupDownFunc, void, Object *List, APTR *arg)
{
  long Count = 0,Selected = 0;
  get(List,MUIA_List_Entries,&Count);
  get(List,MUIA_List_Active,&Selected);
  if(Selected>-1&&Selected<Count-1)
  {
    DoMethod(List,MUIM_List_Exchange,Selected,Selected+1);
    DoMethod(CurrentShownURLGroup->ChildList,MUIM_List_Exchange,Selected,Selected+1);
    set(List,MUIA_List_Active,Selected+1);
    SaveHotlist();
  }  
}     
MakeHook(HotlistGroupDownHook, HotlistGroupDownFunc);
 
///
///Hotlist Load/Save
void LoadHotlist(void)
{
  char buffer[530];
  FILE *file;
  struct HotlistGroupEntry *CurrentList;
  set(App,MUIA_Application_Sleep,TRUE);
  CurrentShownURLGroup=&GroupRootList;
  GroupRootList.Parent=NULL;
  strcpy(GroupRootList.Title,"");
  if((GroupRootList.ChildList=ListObject,
    MUIA_List_ConstructHook,&HotlistGroupConstructHook,
    MUIA_List_DestructHook, &HotlistGroupDestructHook,
    MUIA_List_CompareHook,  &HotlistGroupCompareHook,
    End))
  {  
    if((GroupRootList.LinkList=ListObject,
      MUIA_List_ConstructHook,&HotlistConstructHook,
      MUIA_List_DestructHook, &HotlistDestructHook,
      MUIA_List_CompareHook,  &HotlistCompareHook,
      End))
    {           
      CurrentList=&GroupRootList;
      if((file=fopen("env:mosaic/.mosaic-hotlist-default","r")))
      {
        if(fgets(buffer,530,file))
        {
          if(fgets(buffer,530,file))
          {                
            char FullPath[200];
            int depth=0;
            strcpy(FullPath,"");
            while(fgets(buffer,530,file))
            {
              buffer[strlen(buffer)-1]=0;
              if(!strnicmp(buffer,"@GROUP",6))
              {
                struct HotlistGroupEntry *NewCurrent;
                struct HotlistGroupEntry hge;
                hge.Depth=depth;
                hge.Parent=CurrentList;
                strcpy(hge.Title,&buffer[7]);
                AddPart(FullPath,hge.Title,200);
                strcpy(hge.FullTitle,FullPath);
                DoMethod(CurrentList->ChildList,MUIM_List_InsertSingle,&hge,MUIV_List_Insert_Bottom);
                set(CurrentList->ChildList,MUIA_List_Active,MUIV_List_Active_Bottom);
                DoMethod(CurrentList->ChildList,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,&NewCurrent);
                CurrentList=NewCurrent;
                depth++;
              }
              else
              {
                if(!strnicmp(buffer,"@ENDGROUP",9))
                {
                  CurrentList=CurrentList->Parent;
                  depth--;
                  *PathPart(FullPath)=0;
                }
                else
                {
                  char *d;
                  struct HotlistEntry new;
                  d=strstr(buffer," ");
                  *d=0;
                  strcpy(new.Location,buffer);
                  strcpy(new.Date,d+1);
                  if(fgets(buffer,128,file))
                  {
                    buffer[strlen(buffer)-1]=0;
                    strcpy(new.Title,buffer);
                    DoMethod(CurrentList->LinkList,MUIM_List_InsertSingle,&new,MUIV_List_Insert_Bottom);
                  }      
                }
              }
            }
          }
        }
        fclose(file);
        ShowHotlist(CurrentShownURLGroup);
      }
    }
  }
  set(App,MUIA_Application_Sleep,FALSE);
}          

void SaveGroup(FILE *file,struct HotlistGroupEntry *hge)
{
  long anz = 0,i;
  get(hge->LinkList,MUIA_List_Entries,&anz);
  for(i=0;i<anz;i++)
  {
    struct HotlistEntry *entry;
    DoMethod(hge->LinkList,MUIM_List_GetEntry,i,&entry);
    fprintf(file,"%s %s\n%s\n",entry->Location,entry->Date,entry->Title);
  }
  get(hge->ChildList,MUIA_List_Entries,&anz);
  for(i=0;i<anz;i++)
  {
    struct HotlistGroupEntry *entry;
    DoMethod(hge->ChildList,MUIM_List_GetEntry,i,&entry);
    fprintf(file,"@GROUP %s\n",entry->Title);
    SaveGroup(file,entry);
    fprintf(file,"@ENDGROUP\n");
  }  
}

void SaveHotlist(void)
{
  char *Files[]={"env:mosaic/.mosaic-hotlist-default","envarc:mosaic/.mosaic-hotlist-default"};
  FILE *file;
  int i;
  set(App,MUIA_Application_Sleep,TRUE);
  for(i=0;i<2;i++)
  {
    if((file=fopen(Files[i],"w")))
    {
      fprintf(file,"ncsa-xmosaic-hotlist-format-1\nDefault\n");
      SaveGroup(file,&GroupRootList);
      fclose(file);
    }       
  }
  set(App,MUIA_Application_Sleep,FALSE);
}
/// ^^^ All this by rs

/*
unsigned long update_debug_cb(void){
  SetAttrs(DBG_Str_Gad,MUIA_String_Contents,DBGstr,TAG_DONE);
  printf("Contents %s \n",DBGstr);
  return (unsigned long) 0;
}
*/
  
/*------------------------------------------------------------------------
  Set up the application and notifications.
------------------------------------------------------------------------*/
void mui_init(void)
{
  int (*visit_func) ();
  void (*image_func) ();
#ifdef HAVE_REXX
  extern struct Mui_Command RexxCommands[] ;
#endif
  ULONG win_open = 0;

  Object *SB_Vert,*SB_Horiz;

  Object *BT_ShowEm;

  visit_func = anchor_visited_predicate;
  image_func = ImageResolve;

//  DBGstr=(char *)calloc(50,sizeof(char));
  
//  strncpy(DBGstr,"???",19);

	App = ApplicationObject,
		MUIA_Application_Title      , "AmigaMosaic",
		MUIA_Application_Version    , amosaic_version,
		MUIA_Application_Copyright  , "©1994,1995",
		MUIA_Application_Author     , "The AMosaic DevTeam",
		MUIA_Application_Description, "World Wide Web Browser.",
		MUIA_Application_Base       , "AMOSAIC",
#ifdef HAVE_REXX
		MUIA_Application_Commands	, RexxCommands,
#endif

		SubWindow, WI_Main = WindowObject,
			MUIA_Window_Title, amosaic_window_title,
			MUIA_Window_ID, MAKE_ID('M','A','I','N'),
//			MUIA_Window_Menu, MN_Main,
			MUIA_Window_Menustrip,MenustripObject,
				Child,MenuObject,MUIA_Menu_Title,GetamosaicString(MSG_MENU_PROJECT),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_OPEN_URL),GetamosaicString(MSG_MENU_OPEN_URL_SC),0,(APTR)mo_open_document),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_OPEN_LOCAL),GetamosaicString(MSG_MENU_OPEN_LOCAL_SC),0,(APTR)mo_open_local_document),
					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_RELOAD_CURRENT),GetamosaicString(MSG_MENU_RELOAD_CURRENT_SC),0,(APTR)mo_reload_document),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_REFRESH_CURRENT),0,NM_ITEMDISABLED,(APTR)mo_refresh_document),
//					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_URL_TO_CLIPBOARD),GetamosaicString(MSG_MENU_URL_TO_CLIPBOARD_SC),0,(APTR)mo_url_to_clipboard),
					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_FIND_IN_CURRENT),GetamosaicString(MSG_MENU_FIND_IN_CURRENT_SC),0,(APTR)mo_search),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_VIEW_SOURCE),GetamosaicString(MSG_MENU_VIEW_SOURCE_SC),0,(APTR)mo_document_source),
					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_SAVE_AS),GetamosaicString(MSG_MENU_SAVE_AS_SC),0,(APTR)mo_save_document),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_PRINT),GetamosaicString(MSG_MENU_PRINT_SC),0,(APTR)mo_print_document),
					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_ABOUT),GetamosaicString(MSG_MENU_ABOUT_SC),0,(APTR)mo_about),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_QUIT),GetamosaicString(MSG_MENU_QUIT_SC),0,(APTR)mo_exit_program),
					End,
				Child,MenuObject,MUIA_Menu_Title,GetamosaicString(MSG_MENU_EDIT),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_URL_TO_CLIPBOARD),0,0,(APTR)mo_url_to_clipboard),
//					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_COPY),GetamosaicString(MSG_MENU_COPY_SC),NM_ITEMDISABLED,0),
					End,
				Child,MenuObject,MUIA_Menu_Title,GetamosaicString(MSG_MENU_OPTIONS),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_LOAD_TO_DISK),GetamosaicString(MSG_MENU_LOAD_TO_DISK_SC),CHECKIT|MENUTOGGLE,(APTR)mo_binary_transfer),
					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_DELAY_IMAGE_LOADING),GetamosaicString(MSG_MENU_DELAY_IMAGE_LOADING_SC),CHECKIT|MENUTOGGLE,(APTR)mo_delay_image_loads),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_LOAD_IMAGES),GetamosaicString(MSG_MENU_LOAD_IMAGES_SC),0,(APTR)mo_expand_images_current),
					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_ISINDEX_TO_FORM),0,CHECKIT | MENUTOGGLE,(APTR)mo_isindex_to_form),

					Child,MenuitemObject,MUIA_Menuitem_Title,GetamosaicString(MSG_MENU_BUTTONS),
						Child,MenuitemObject,MUIA_Menuitem_Title,GetamosaicString(MSG_MENU_TEXT_ONLY),MUIA_Menuitem_Checkit,TRUE,MUIA_Menuitem_Exclude,~1,MUIA_UserData,(APTR)mo_buttons_text,End,
						Child,MenuitemObject,MUIA_Menuitem_Title,GetamosaicString(MSG_MENU_IMAGES_ONLY),MUIA_Menuitem_Checkit,TRUE,MUIA_Menuitem_Exclude,~2,MUIA_UserData,(APTR)mo_buttons_image,End,
						Child,MenuitemObject,MUIA_Menuitem_Title,GetamosaicString(MSG_MENU_IMAGES_AND_TEXT),MUIA_Menuitem_Checkit,TRUE,MUIA_Menuitem_Exclude,~4,MUIA_UserData,(APTR)mo_buttons_imagetext,End,
						End,

					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_FLUSH_IMAGE_CACHE),GetamosaicString(MSG_MENU_FLUSH_IMAGE_CACHE_SC),0,(APTR)mo_clear_image_cache),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_CLEAR_GLOBAL_HISTORY),GetamosaicString(MSG_MENU_CLEAR_GLOBAL_HISTORY_SC),0,(APTR)mo_clear_global_history),
					End,	
				Child,MenuObject,MUIA_Menu_Title,GetamosaicString(MSG_MENU_NAVIGATE),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_BACK),GetamosaicString(MSG_MENU_BACK_SC),0,(APTR)mo_back),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_FORWARD),GetamosaicString(MSG_MENU_FORWARD_SC),0,(APTR)mo_forward),
					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_HOME_DOCUMENT),GetamosaicString(MSG_MENU_HOME_DOCUMENT_SC),0,(APTR)mo_home_document),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_WINDOW_HISTORY),GetamosaicString(MSG_MENU_WINDOW_HISTORY_SC),0,(APTR)mo_history_list),
					Child,MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_HOTLIST),GetamosaicString(MSG_MENU_HOTLIST_SC),0,(APTR)mo_jump_hot_list),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_ADD_HOTLIST),GetamosaicString(MSG_MENU_ADD_HOTLIST_SC),0,(APTR)mo_add_hot_list),
					End,	
				Child,MN_RexxMenu=MenuObject,MUIA_Menu_Title,GetamosaicString(MSG_MENU_REXX),
					Child,MUI_MakeObject(MUIO_Menuitem,GetamosaicString(MSG_MENU_MACRO),GetamosaicString(MSG_MENU_MACRO_SC),0,(APTR)mo_macro),
					End,
				End,

			WindowContents, TTopGroup=VGroup,
				Child,TopGroup=HGroup, /* Title text */
					Child, LB_Title = Label2(GetamosaicString(MSG_GADGET_TITLE)),
					Child, TX_Title = TextObject, TextFrame, MUIA_Text_SetMin,FALSE,MUIA_ShortHelp,"This is the title of this document",MUIA_Background,MUII_TextBack,End,
					End,
				Child,HGroup,	/* Url + Buttons + Boing */
					Child,VGroup, /* Url + Buttons */
						Child,HGroup, /* Url */
							Child,TextObject,MUIA_Text_PreParse,"\033r",MUIA_Text_Contents,GetamosaicString(MSG_GADGET_URL),MUIA_Weight,0,MUIA_FixWidthTxt,GetamosaicString(MSG_GADGET_TITLE),End,
              Child,HGroup,
                MUIA_Group_Spacing,1,
                MUIA_Group_SameHeight,TRUE,
                Child,PopURL=PopobjectObject,
                  MUIA_Popstring_String,TX_URL=StringObject, StringFrame, MUIA_String_MaxLen,512,End, 
                  MUIA_Popstring_Button,PopButton(MUII_PopUp),
                  MUIA_Popobject_StrObjHook,&URLStrObjHook,
                  MUIA_Popobject_ObjStrHook,&URLObjStrHook,
                  MUIA_Popobject_Object,ColGroup(2),
                    MUIA_Group_Spacing,0,
											Child,BT_HotlistGroupParent=SimpleButton(GetamosaicString(MSG_HOTLIST_PARENT)),
                    Child,TX_HotlistGroupTitle=TextObject,TextFrame,MUIA_Background,MUII_TextBack,End,
                    Child,URLGroupList=ListviewObject,
                      MUIA_FixHeightTxt,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n",
                      MUIA_Listview_List,ListObject,
                        InputListFrame,
                        MUIA_List_ConstructHook,&HotlistGroupConstructHook,
                        MUIA_List_DestructHook, &HotlistGroupDestructHook,
                        MUIA_List_DisplayHook,  &HotlistGroupDisplayHook,
                        MUIA_List_CompareHook,  &HotlistGroupCompareHook,
                        End,
                      End,
                    Child,URLList=ListviewObject,
                      MUIA_Listview_List,ListObject,
                        InputListFrame,
                        MUIA_List_ConstructHook,&HotlistConstructHook,
                        MUIA_List_DestructHook, &HotlistDestructHook,
                        MUIA_List_DisplayHook,  &HotlistDisplayHook,
                        MUIA_List_CompareHook,  &HotlistCompareHook,
                        End,
                      End,               
                    Child,ColGroup(3),
                      MUIA_Weight,0,
                      MUIA_Group_Spacing,0,
                      MUIA_Group_SameWidth,TRUE,
                      Child,BT_HotlistGroupNew=SimpleButton(GetamosaicString(MSG_HOTLIST_NEW)),
                      Child,BT_HotlistGroupDelete=SimpleButton(GetamosaicString(MSG_HOTLIST_DELETE)),
                      Child,BT_HotlistGroupEdit=SimpleButton(GetamosaicString(MSG_HOTLIST_EDIT)),
                      Child,BT_HotlistGroupSort=SimpleButton(GetamosaicString(MSG_HOTLIST_SORT)),
                      Child,BT_HotlistGroupUp=SimpleButton("\033I[6:38]"),
                      Child,BT_HotlistGroupDown=SimpleButton("\033I[6:39]"),
                      End,
                    Child,ColGroup(3),
                      MUIA_Group_Spacing,0,
                      MUIA_Group_SameWidth,TRUE,
                      Child,BT_HotlistNew=SimpleButton(GetamosaicString(MSG_HOTLIST_NEW)),
                      Child,BT_HotlistDelete=SimpleButton(GetamosaicString(MSG_HOTLIST_DELETE)),
                      Child,BT_HotlistEdit=SimpleButton(GetamosaicString(MSG_HOTLIST_EDIT)),
                      Child,BT_HotlistSort=SimpleButton(GetamosaicString(MSG_HOTLIST_SORT)),
                      Child,BT_HotlistMove=SimpleButton(GetamosaicString(MSG_HOTLIST_MOVE)),
                      Child,HGroup,
                        MUIA_Group_Spacing,0,
                        Child,BT_HotlistUp=SimpleButton("\033I[6:38]"),
                        Child,BT_HotlistDown=SimpleButton("\033I[6:39]"),
                        End,
                      End,        
                    End,
                  End,
								Child,BT_AddHotlist=VGroup,
									MUIA_Weight,0,MUIA_InnerBottom,0,MUIA_InnerTop,0,
									MUIA_Group_Spacing,0,
									ButtonFrame,MUIA_Background,MUII_ButtonBack,MUIA_InputMode,MUIV_InputMode_RelVerify,
									Child,RectangleObject, MUIA_Weight, 1,End,
									Child,TextObject,MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,GetamosaicString(MSG_HOTLIST_ADD),MUIA_Weight,100,End,
									Child,RectangleObject, MUIA_Weight, 1,End,
									End,
								End,
							End,
						Child, HGroup, /* Buttons */
							Child,HGroupV,
								VirtualFrame,
								MUIA_Group_Spacing,0,
								Child,ButtonGroup=HGroup,
									MUIA_Group_SameWidth,TRUE,
									MUIA_Weight,0,
									Child,BT_Back=VGroup,
										MUIA_ShortHelp,"This button takes\nyou back one page",
										ButtonFrame,
//										MUIA_HorizDisappear,7,
										MUIA_ControlChar,*GetamosaicString(MSG_BUTTON_BACK_SC),
										MUIA_Background,MUII_ButtonBack,
										MUIA_InputMode,MUIV_InputMode_RelVerify,
										MUIA_Group_Spacing,0,
										Child,IButtonBack=HGroup,
											MUIA_Group_Spacing,0,
											Child,HSpace(0),
											Child,BodychunkObject,
												MUIA_Bodychunk_Body,Back_body,
												MUIA_Bodychunk_Compression,BACK_COMPRESSION,
												MUIA_Bodychunk_Depth,BACK_DEPTH,
												MUIA_Bodychunk_Masking,BACK_MASKING,
												MUIA_Bitmap_Width,BACK_WIDTH,
												MUIA_Bitmap_Height,BACK_HEIGHT,
												MUIA_Bitmap_SourceColors,Back_colors,
												MUIA_Bitmap_Transparent,0,
												MUIA_FixWidth,BACK_WIDTH,
												MUIA_FixHeight,BACK_HEIGHT,
												End,
											Child,HSpace(0),
											End,
										Child,TButtonBack=TextObject,MUIA_Text_HiChar,*GetamosaicString(MSG_BUTTON_BACK_SC),MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,GetamosaicString(MSG_BUTTON_BACK),MUIA_Font,MUIV_Font_Tiny,End,
										End,
									Child,BT_Forward=VGroup,
										MUIA_ShortHelp,"This button takes\nyou forward one page",
										ButtonFrame,
//										MUIA_HorizDisappear,6,
										MUIA_ControlChar,*GetamosaicString(MSG_BUTTON_FORWARD_SC),
										MUIA_Background,MUII_ButtonBack,
										MUIA_InputMode,MUIV_InputMode_RelVerify,
										MUIA_Group_Spacing,0,		 
										Child,IButtonForward=HGroup,
											MUIA_Group_Spacing,0, 
											Child,HSpace(0),			
											Child,BodychunkObject,
												MUIA_Bodychunk_Body,Forward_body,
												MUIA_Bodychunk_Compression,FORWARD_COMPRESSION,
												MUIA_Bodychunk_Depth,FORWARD_DEPTH,
												MUIA_Bodychunk_Masking,FORWARD_MASKING,
												MUIA_Bitmap_Width,FORWARD_WIDTH,
												MUIA_Bitmap_Height,FORWARD_HEIGHT,
												MUIA_Bitmap_SourceColors,Forward_colors,
												MUIA_Bitmap_Transparent,0,
												MUIA_FixWidth,FORWARD_WIDTH,
												MUIA_FixHeight,FORWARD_HEIGHT,
												End,			
											Child,HSpace(0),
											End,
										Child,TButtonForward=TextObject,MUIA_Text_HiChar,*GetamosaicString(MSG_BUTTON_FORWARD_SC),MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,GetamosaicString(MSG_BUTTON_FORWARD),MUIA_Font,MUIV_Font_Tiny,End,
										End,
									Child,BT_Reload=VGroup,
										MUIA_ShortHelp,"This button reloads\nthe current page",
										ButtonFrame,
//										MUIA_HorizDisappear,5,
										MUIA_ControlChar,*GetamosaicString(MSG_BUTTON_RELOAD_SC),
										MUIA_Background,MUII_ButtonBack,			 
										MUIA_InputMode,MUIV_InputMode_RelVerify,
										MUIA_Group_Spacing,0,		 
										Child,IButtonReload=HGroup,
											MUIA_Group_Spacing,0, 
											Child,HSpace(0),			 
											Child,BodychunkObject,
												MUIA_Bodychunk_Body,Reload_body,
												MUIA_Bodychunk_Compression,RELOAD_COMPRESSION,
												MUIA_Bodychunk_Depth,RELOAD_DEPTH,
												MUIA_Bodychunk_Masking,RELOAD_MASKING,
												MUIA_Bitmap_Width,RELOAD_WIDTH,
												MUIA_Bitmap_Height,RELOAD_HEIGHT,
												MUIA_Bitmap_SourceColors,Reload_colors,
												MUIA_Bitmap_Transparent,0,
												MUIA_FixWidth,RELOAD_WIDTH,
												MUIA_FixHeight,RELOAD_HEIGHT,
												End,				
											Child,HSpace(0),
											End,
										Child,TButtonReload=TextObject,MUIA_Text_HiChar,*GetamosaicString(MSG_BUTTON_RELOAD_SC),MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,GetamosaicString(MSG_BUTTON_RELOAD),MUIA_Font,MUIV_Font_Tiny,End,
										End,
									Child,BT_Home=VGroup,
										MUIA_ShortHelp,"This button takes you to your\ndefault home page that you have\nin your configuration file.",
										ButtonFrame,
//										MUIA_HorizDisappear,4,
										MUIA_ControlChar,*GetamosaicString(MSG_BUTTON_HOME_SC),
										MUIA_Background,MUII_ButtonBack,			 
										MUIA_InputMode,MUIV_InputMode_RelVerify,
										MUIA_Group_Spacing,0,		 
										Child,IButtonHome=HGroup,
											MUIA_Group_Spacing,0, 
											Child,HSpace(0),			 
											Child,BodychunkObject,
												MUIA_Bodychunk_Body,Home_body,
												MUIA_Bodychunk_Compression,HOME_COMPRESSION,
												MUIA_Bodychunk_Depth,HOME_DEPTH,
												MUIA_Bodychunk_Masking,HOME_MASKING,
												MUIA_Bitmap_Width,HOME_WIDTH,
												MUIA_Bitmap_Height,HOME_HEIGHT,
												MUIA_Bitmap_SourceColors,Home_colors,
												MUIA_Bitmap_Transparent,0,
												MUIA_FixWidth,HOME_WIDTH,
												MUIA_FixHeight,HOME_HEIGHT,
												End,				
											Child,HSpace(0),
											End,
										Child,TButtonHome=TextObject,MUIA_Text_HiChar,*GetamosaicString(MSG_BUTTON_HOME_SC),MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,GetamosaicString(MSG_BUTTON_HOME),MUIA_Font,MUIV_Font_Tiny,End,
										End,
									Child,BT_Open=VGroup,
										MUIA_ShortHelp,"This button opens a new location",
										ButtonFrame,
//										MUIA_HorizDisappear,3,
										MUIA_Background,MUII_ButtonBack,			 
										MUIA_InputMode,MUIV_InputMode_RelVerify,
										MUIA_Group_Spacing,0,		 
										Child,IButtonOpen=HGroup,
											MUIA_Group_Spacing,0, 
											Child,HSpace(0),			 
											Child,BodychunkObject,
												MUIA_Bodychunk_Body,Open_body,
												MUIA_Bodychunk_Compression,OPEN_COMPRESSION,
												MUIA_Bodychunk_Depth,OPEN_DEPTH,
												MUIA_Bodychunk_Masking,OPEN_MASKING,
												MUIA_Bitmap_Width,OPEN_WIDTH,
												MUIA_Bitmap_Height,OPEN_HEIGHT,
												MUIA_Bitmap_SourceColors,Open_colors,
												MUIA_Bitmap_Transparent,0,
												MUIA_FixWidth,OPEN_WIDTH,
												MUIA_FixHeight,OPEN_HEIGHT,
												End,				
											Child,HSpace(0),
											End,
										Child,TButtonOpen=TextObject,MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,GetamosaicString(MSG_BUTTON_OPEN),MUIA_Font,MUIV_Font_Tiny,End,
										End,		
									Child,BT_Amiga=VGroup,
										MUIA_ShortHelp,"This button takes you\nto the Amiga home page",
										ButtonFrame,
//										MUIA_HorizDisappear,2,
										MUIA_Background,MUII_ButtonBack,			 
										MUIA_InputMode,MUIV_InputMode_RelVerify,
										MUIA_Group_Spacing,0,		 
										Child,IButtonAmiga=HGroup,
											MUIA_Group_Spacing,0, 
											Child,HSpace(0),			 
											Child,BodychunkObject,
												MUIA_Bodychunk_Body,Amiga_body,
												MUIA_Bodychunk_Compression,AMIGA_COMPRESSION,
												MUIA_Bodychunk_Depth,AMIGA_DEPTH,
												MUIA_Bodychunk_Masking,AMIGA_MASKING,
												MUIA_Bitmap_Width,AMIGA_WIDTH,
												MUIA_Bitmap_Height,AMIGA_HEIGHT,
												MUIA_Bitmap_SourceColors,Amiga_colors,
												MUIA_Bitmap_Transparent,0,
												MUIA_FixWidth,AMIGA_WIDTH,
												MUIA_FixHeight,AMIGA_HEIGHT,
												End,				
											Child,HSpace(0),
											End,
										Child,TButtonAmiga=TextObject,MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,"Amiga H.",MUIA_Font,MUIV_Font_Tiny,End,
										End,		

									Child,BT_AMosaic=VGroup,
										MUIA_ShortHelp,"This button takes you\nto the AMosaic home page",
										ButtonFrame,
//										MUIA_HorizDisappear,1,
										MUIA_Background,MUII_ButtonBack,
										MUIA_InputMode,MUIV_InputMode_RelVerify,
										MUIA_Group_Spacing,0,		 
										Child,IButtonAMosaic=HGroup,
											MUIA_Group_Spacing,0, 
											Child,HSpace(0),			 
											Child,BodychunkObject,
												MUIA_Bodychunk_Body,AMosaic_body,
												MUIA_Bodychunk_Compression,AMOSAIC_COMPRESSION,
												MUIA_Bodychunk_Depth,AMOSAIC_DEPTH,
												MUIA_Bodychunk_Masking,AMOSAIC_MASKING,
												MUIA_Bitmap_Width,AMOSAIC_WIDTH,
												MUIA_Bitmap_Height,AMOSAIC_HEIGHT,
												MUIA_Bitmap_SourceColors,AMosaic_colors,
												MUIA_Bitmap_Transparent,0,
												MUIA_FixWidth,AMOSAIC_WIDTH,
												MUIA_FixHeight,AMOSAIC_HEIGHT,
												End,				
											Child,HSpace(0),
											End,
										Child,TButtonAMosaic=TextObject,MUIA_Text_PreParse,"\033c",MUIA_Text_Contents,"AM Home",MUIA_Font,MUIV_Font_Tiny,End,
										End,		
									End,											 
								Child,HSpace(2000), /* A screen wider that 2000 is very unlikely */
								End,
							End,
						End,
					Child,VGroup,
						Child, BT_Interrupt = NewObject(BoingTransferClass, NULL,
							MUIA_ShortHelp,"This button interrupts\nthe current transfer",
							MUIA_Background,MUII_ButtonBack,
							MUIA_Frame, MUIV_Frame_ImageButton,
							MUIA_InputMode, MUIV_InputMode_RelVerify, TAG_DONE),
						Child,RectangleObject, MUIA_Weight, 1, End,
						End,
					End,
				Child, ColGroup(2),
					MUIA_Group_Spacing,1,
//					Child,HVSpace,
					Child, HTML_Gad = NewObject(HTMLGadClass, NULL,
						TextFrame,
						MUIA_FillArea,FALSE, /* I am handling the background */
						HTMLA_window, NULL,
						HTMLA_font, tf_norm,
						HTMLA_italic_font, tf_ital,
						HTMLA_bold_font, tf_bold,
						HTMLA_fixed_font, tf_fixed,
						HTMLA_header1_font, tf_h1,
						HTMLA_header2_font, tf_h2,
						HTMLA_header3_font, tf_h3,
						HTMLA_header4_font, tf_h4,
						HTMLA_header5_font, tf_h5,
						HTMLA_header6_font, tf_h6,
						HTMLA_plain_font, tf_plain,
						HTMLA_address_font, tf_address,
						HTMLA_listing_font, tf_listing, 
						HTMLA_visit_func, visit_func,
						HTMLA_image_func, image_func,
						TAG_DONE),
					Child, SB_Vert = ScrollbarObject, End,
					Child, SB_Horiz = ScrollbarObject, MUIA_Group_Horiz, TRUE, End,
					Child,HVSpace,
					End,

				Child, PG_Bottom = PageGroup,
					MUIA_Font,MUIV_Font_Tiny,
					Child, TX_Dest = TextObject, NoFrame, End,
					Child, HGroup,
						Child, GG_Progress = GaugeObject, MUIA_Gauge_Horiz, TRUE,
										MUIA_Gauge_Divide, 0, GaugeFrame, End,
						Child, TX_Progress = TextObject, NoFrame, MUIA_FixWidthTxt,"00000000", 
										MUIA_Text_Contents, "00000000", End,
						End,
					Child, SearchIndex=HGroup,
						MUIA_ShowMe,FALSE,
						Child, BT_Search = SimpleButtonHelp("Search","This button sends the search\nrequest to the remote server"),
						Child, STR_Search = StringObject, StringFrame, MUIA_String_MaxLen,
											512, MUIA_Weight, 800, MUIA_ShortHelp,"Here you can enter what you want\nto send to the remote server",End,
						End,
					End,
				End,
			End,
		SubWindow, WI_Wait = WindowObject,
			MUIA_Window_DragBar,FALSE,
			MUIA_Window_CloseGadget,FALSE,
			MUIA_Window_LeftEdge,MUIV_Window_LeftEdge_Centered,
			MUIA_Window_TopEdge,MUIV_Window_TopEdge_Centered,
			MUIA_Window_DepthGadget,FALSE,
			WindowContents, TextObject,
				NoFrame,
				MUIA_Text_Contents,GetamosaicString(MSG_REQ_CONSTRUCTING),
				MUIA_Text_SetMax,TRUE,
				End,
			End,
		SubWindow, WI_ImagesReady = WindowObject,
			MUIA_Window_ID, MAKE_ID('I','R','D','Y'),
			MUIA_Window_Title, "AMosaic info.",
			MUIA_Window_CloseGadget,FALSE,
			WindowContents, VGroup,
				Child,TextObject,
					NoFrame,
					MUIA_Text_Contents,GetamosaicString(MSG_REQ_FORM_READY),
					MUIA_Text_SetMax,TRUE,
					End,
				Child,HGroup,
					Child,HSpace(0),
					Child,BT_ShowEm=SimpleButtonHelp(GetamosaicString(MSG_REQ_SHOW_THEM),"Press here to recalculate\nthe current form"),
					Child,HSpace(0),
					End,
				End,
			End,
		End;

	if (!App) fail(App,"Failed to create Application.");

	{
		int i,cnt=0;
		for(i=0;i<10;i++)
		{
			if(RexxMacroNames[i])
			{
				APTR Obj,UserData;
				char *Label;
				if(!cnt)
				{
					if((Obj=MUI_MakeObject(MUIO_Menuitem,NM_BARLABEL,0,0,NULL)))
						DoMethod(MN_RexxMenu,MUIM_Family_AddTail,Obj);
				}
				ShortCuts[i][1] = '\0' ;
				switch(RexxMacroNames[i][0])
				{
					case '~':
						Label=NM_BARLABEL;
						UserData=(APTR)NULL;
						ShortCuts[i][0] = '\0' ;
						break;
					default:
						Label=RexxMacroNames[i];
						UserData=(APTR)(IPTR)(i+mo_macro_1);
						ShortCuts[i][0]='0'+(i+1)%10;
						break;
				}
				if((Obj=MUI_MakeObject(MUIO_Menuitem,Label, UserData ? ShortCuts[i] : NULL,0,UserData)))
				{
					DoMethod(MN_RexxMenu,MUIM_Family_AddTail,Obj);
					cnt++;
				}
			}
		}
	}

	LoadHotlist(); // New 12/6 /* This eats 30 k! Memory leak! */

	if(Rdata.title_to_window){
//		set(LB_Title,MUIA_ShowMe,False);
//		set(TX_Title,MUIA_ShowMe,False);

		DoMethod(TTopGroup,OM_REMMEMBER,TopGroup);
		MUI_DisposeObject(TopGroup);
		TX_Title=0;
		}

	DoMethod(WI_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, App, 2,
		MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	DoMethod(BT_Back, MUIM_Notify, MUIA_Pressed, FALSE, App, 2,
		MUIM_Application_ReturnID,mo_back);
	DoMethod(BT_Forward, MUIM_Notify, MUIA_Pressed, FALSE, App, 2,
		MUIM_Application_ReturnID,mo_forward);

	DoMethod(BT_Home, MUIM_Notify, MUIA_Pressed, FALSE,
		App, 2, MUIM_Application_ReturnID, mo_home_document);

	DoMethod(BT_Open, MUIM_Notify, MUIA_Pressed, FALSE,
		App, 2, MUIM_Application_ReturnID, mo_open_document);

	DoMethod(BT_Reload, MUIM_Notify, MUIA_Pressed, FALSE,
		App, 2, MUIM_Application_ReturnID, mo_reload_document);

	DoMethod(BT_Amiga, MUIM_Notify, MUIA_Pressed, FALSE,
		App, 2, MUIM_Application_ReturnID, mo_load_amigahome);

	DoMethod(BT_AMosaic, MUIM_Notify, MUIA_Pressed, FALSE,
		App, 2, MUIM_Application_ReturnID, mo_load_amosaichome);


	DoMethod(URLGroupList,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,URLGroupList,3,MUIM_CallHook,&HotlistGroupSelectHook,MUIV_TriggerValue); // New 6/12 /rs
	DoMethod(URLList,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,PopURL,2,MUIM_Popstring_Close,TRUE);

	DoMethod(URLGroupList,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,URLGroupList,3,MUIM_CallHook,&HotlistGroupButtonsHook,MUIV_TriggerValue);
	DoMethod(URLList,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,URLList,3,MUIM_CallHook,&HotlistButtonsHook,MUIV_TriggerValue);

	DoMethod(BT_HotlistGroupParent,MUIM_Notify,MUIA_Pressed,FALSE,URLGroupList,2,MUIM_CallHook,&HotlistGroupParentHook);
	DoMethod(BT_HotlistGroupNew,MUIM_Notify,MUIA_Pressed,FALSE,URLGroupList,2,MUIM_CallHook,&HotlistGroupNewHook);
	DoMethod(BT_HotlistGroupDelete,MUIM_Notify,MUIA_Pressed,FALSE,URLGroupList,2,MUIM_CallHook,&HotlistGroupDeleteHook);
	DoMethod(BT_HotlistGroupEdit,MUIM_Notify,MUIA_Pressed,FALSE,URLGroupList,2,MUIM_CallHook,&HotlistGroupEditHook);
	DoMethod(BT_HotlistGroupSort,MUIM_Notify,MUIA_Pressed,FALSE,URLGroupList,2,MUIM_CallHook,&HotlistGroupSortHook);
	DoMethod(BT_HotlistGroupUp,MUIM_Notify,MUIA_Pressed,FALSE,URLGroupList,2,MUIM_CallHook,&HotlistGroupUpHook);
	DoMethod(BT_HotlistGroupDown,MUIM_Notify,MUIA_Pressed,FALSE,URLGroupList,2,MUIM_CallHook,&HotlistGroupDownHook);

	DoMethod(BT_HotlistNew,MUIM_Notify,MUIA_Pressed,FALSE,URLList,2,MUIM_CallHook,&HotlistNewHook);
	DoMethod(BT_HotlistDelete,MUIM_Notify,MUIA_Pressed,FALSE,URLList,2,MUIM_CallHook,&HotlistDeleteHook);
	DoMethod(BT_HotlistEdit,MUIM_Notify,MUIA_Pressed,FALSE,URLList,2,MUIM_CallHook,&HotlistEditHook);
	DoMethod(BT_HotlistSort,MUIM_Notify,MUIA_Pressed,FALSE,URLList,2,MUIM_CallHook,&HotlistSortHook);
	DoMethod(BT_HotlistUp,MUIM_Notify,MUIA_Pressed,FALSE,URLList,2,MUIM_CallHook,&HotlistUpHook);
	DoMethod(BT_HotlistDown,MUIM_Notify,MUIA_Pressed,FALSE,URLList,2,MUIM_CallHook,&HotlistDownHook);
	DoMethod(BT_HotlistMove,MUIM_Notify,MUIA_Pressed,FALSE,URLList,2,MUIM_CallHook,&HotlistMoveHook);
	DoMethod(BT_AddHotlist,MUIM_Notify,MUIA_Pressed,FALSE,URLList,2,MUIM_CallHook,&HotlistAddHook);

	set(BT_HotlistDelete,MUIA_Disabled,TRUE);
	set(BT_HotlistEdit,MUIA_Disabled,TRUE);
	set(BT_HotlistUp,MUIA_Disabled,TRUE);
	set(BT_HotlistDown,MUIA_Disabled,TRUE);
	set(BT_HotlistMove,MUIA_Disabled,TRUE);

	set(BT_HotlistGroupDelete,MUIA_Disabled,TRUE);
	set(BT_HotlistGroupEdit,MUIA_Disabled,TRUE);
	set(BT_HotlistGroupUp,MUIA_Disabled,TRUE);
	set(BT_HotlistGroupDown,MUIA_Disabled,TRUE);		// ^^^ New 12/6 /rs


	DoMethod(BT_Interrupt, MUIM_Notify, MUIA_Pressed, FALSE,
		BT_Interrupt, 2, MUIM_CallHook, &interrupt_hook);



	set(HTML_Gad,HTMLA_SB_Vert,SB_Vert);
	set(HTML_Gad,HTMLA_SB_Horiz,SB_Horiz);

	DoMethod(SB_Vert, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
		HTML_Gad, 3, MUIM_Set, HTMLA_scroll_y, MUIV_TriggerValue);
	DoMethod(SB_Horiz, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
		HTML_Gad, 3, MUIM_Set, HTMLA_scroll_x, MUIV_TriggerValue);

	DoMethod(TX_URL,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
		App,2,MUIM_Application_ReturnID,ID_STRINGURL);


	DoMethod(G_Norm, MUIM_Notify, HTMLA_new_href, MUIV_EveryTime, App, 2,
		MUIM_Application_ReturnID, ID_GOTOANCHOR);
	DoMethod(G_Norm, MUIM_Notify, HTMLA_get_href, MUIV_EveryTime, App, 2,
		MUIM_Application_ReturnID, ID_GETTEXT);
	DoMethod(G_Norm, MUIM_Notify, HTMLA_image_load, MUIV_EveryTime, App, 2,
		MUIM_Application_ReturnID, ID_IMAGELOAD);
	DoMethod(G_Norm, MUIM_Notify, HTMLA_post_text, MUIV_EveryTime, App, 2,
		MUIM_Application_ReturnID, ID_POSTTEXT);

	DoMethod(BT_Search, MUIM_Notify, MUIA_Pressed, FALSE, App, 2,
		MUIM_Application_ReturnID, mo_search_index);
	DoMethod(STR_Search, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
		App, 2, MUIM_Application_ReturnID, mo_search_index);

	DoMethod(BT_ShowEm, MUIM_Notify, MUIA_Pressed, FALSE, App, 2,
		MUIM_Application_ReturnID, mo_show_form_images);

	/* Cycle chain, etc */

	DoMethod(WI_Main, MUIM_Window_SetCycleChain,
			TX_URL,BT_Back,BT_Forward,BT_Reload,BT_Home,BT_Open,BT_Amiga,BT_AMosaic,NULL);

	/* Inlined image foo */

	set(SB_Vert, MUIA_Prop_DeltaFactor, 15); /* arrows jump 15 pixels */
	set(SB_Horiz, MUIA_Prop_DeltaFactor, 15); /* arrows jump 15 pixels */

	/* Here we go! */

	DoMethod(App, MUIM_Application_SetMenuCheck, mo_delay_image_loads, Rdata.delay_image_loads);
	DoMethod(App, MUIM_Application_SetMenuCheck, mo_isindex_to_form, Rdata.isindex_to_form);

	DoMethod(App, MUIM_Application_SetMenuCheck, mo_buttons_text,Rdata.image_buttons == 0);
	DoMethod(App, MUIM_Application_SetMenuCheck, mo_buttons_image,Rdata.image_buttons == 1);
	DoMethod(App, MUIM_Application_SetMenuCheck, mo_buttons_imagetext,Rdata.image_buttons == 2);

	SetImageButtons(Rdata.image_buttons);
	SetSearchIndex(Rdata.image_buttons);

	set(WI_Main, MUIA_Window_Open, TRUE);
	get(WI_Main,MUIA_Window_Open,&win_open);
	if(!win_open){
		MUI_RequestA(App,0,0,0,"Ok","Failed to open window.",NULL);
		fail(App,"Unable to open window\n");
		}
}


/*------------------------------------------------------------------------
  MUI macros.
------------------------------------------------------------------------*/

/*************************/
/* Init & Fail Functions */
/*************************/

VOID fail(APTR app,char *str)
{
	if (app)
		MUI_DisposeObject(app);

#if defined(USE_ZUNE) && !defined(__AROS__)
#else
	if (MUIMasterBase){
		CloseLibrary(MUIMasterBase);
		MUIMasterBase = NULL;
		}
#endif
	if (DataTypesBase){
		CloseLibrary (DataTypesBase);
		DataTypesBase = NULL;
		}

	if (str){
		puts(str);
		exit(20);
		}
	
	exit(0);
}

//int CXBRK(void) { return(0); }
//int _CXBRK(void) { return(0); }
//void chkabort(void) {}

extern int lib_version;

#if defined(USE_ZUNE) && !defined(__AROS__)

/* On AmigaOS we build a fake library base, because it's not compiled as sharedlibrary yet */
#include "muimaster_intern.h"
#include "prefs.h"

int openmuimaster(void)
{
    static struct MUIMasterBase_intern MUIMasterBase_instance;
    MUIMasterBase = (struct Library*)&MUIMasterBase_instance;

    MUIMasterBase_instance.sysbase = *((struct ExecBase **)4);
    MUIMasterBase_instance.dosbase = (void*)OpenLibrary("dos.library",37);
    MUIMasterBase_instance.utilitybase = (void*)OpenLibrary("utility.library",37);
    MUIMasterBase_instance.aslbase = OpenLibrary("asl.library",37);
    MUIMasterBase_instance.gfxbase = (void*)OpenLibrary("graphics.library",37);
    MUIMasterBase_instance.layersbase = OpenLibrary("layers.library",37);
    MUIMasterBase_instance.intuibase = (void*)OpenLibrary("intuition.library",37);
    MUIMasterBase_instance.cxbase = OpenLibrary("commodities.library",37);
    MUIMasterBase_instance.keymapbase = OpenLibrary("keymap.library",37);
    MUIMasterBase_instance.gadtoolsbase = OpenLibrary("gadtools.library",37);
    __zune_prefs_init(&__zprefs);

    return 1;
}

void closemuimaster(void)
{
}

#undef SysBase
#undef IntuitionBase
#undef GfxBase
#undef LayersBase
#undef UtilityBase

#endif


VOID init(VOID)
{
#if defined(USE_ZUNE) && !defined(__AROS__)

  openmuimaster();
#else
	if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME,8)))
		fail(NULL,"Failed to open "MUIMASTER_NAME".");
#endif

	if (lib_version >= 39) {
		if (!(DataTypesBase = OpenLibrary("datatypes.library", 0L)))
			fail(NULL,"Failed to open Datatypes.library");
	}
}

void SetImageButtons(long state)
{
	long s;
	long win_open = 0;
	s=state;

	get(WI_Main,MUIA_Window_Open,&win_open);

	if(win_open){
		if(mui_version>10)
			DoMethod(ButtonGroup,MUIM_Group_InitChange);
		else
			set(WI_Main,MUIA_Window_Open,FALSE);
		}

	set(IButtonBack,MUIA_ShowMe,s);
	set(IButtonForward,MUIA_ShowMe,s);
	set(IButtonReload,MUIA_ShowMe,s);
	set(IButtonHome,MUIA_ShowMe,s);
	set(IButtonOpen,MUIA_ShowMe,s);
	set(IButtonAmiga,MUIA_ShowMe,s);
	set(IButtonAMosaic,MUIA_ShowMe,s);

	s= (!state || (state==2));
	set(TButtonBack,MUIA_ShowMe,s);
	set(TButtonForward,MUIA_ShowMe,s);
	set(TButtonReload,MUIA_ShowMe,s);
	set(TButtonHome,MUIA_ShowMe,s);
	set(TButtonOpen,MUIA_ShowMe,s);
	set(TButtonAmiga,MUIA_ShowMe,s);
	set(TButtonAMosaic,MUIA_ShowMe,s);

	if(win_open){
		if(mui_version>10)
			DoMethod(ButtonGroup,MUIM_Group_ExitChange);
		else
			set(WI_Main,MUIA_Window_Open,TRUE);
		}
}

void SetSearchIndex(long state)
{
	set(SearchIndex,MUIA_ShowMe,state);
}

#ifndef __AROS__
__near
#endif
LONG __stack = 32768;  /* Make it big for egs users */
