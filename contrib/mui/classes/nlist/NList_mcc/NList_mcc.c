/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number:

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

#ifdef __AROS__
#include <proto/intuition.h>
#endif

#include "private.h"

/* IO macros */
/*
 *#define IO_SIGBIT(req)  ((LONG)(((struct IORequest *)req)->io_Message.mn_ReplyPort->mp_SigBit))
 *#define IO_SIGMASK(req) ((LONG)(1L<<IO_SIGBIT(req)))
 */


DEFAULT_KEYS_ARRAY

#define INIT_PEN(attr,var_dest,test_init) \
  { \
    var_dest = -1; \
    if((tag = FindTagItem(attr, msg->ops_AttrList))) \
    { \
      test_init = tag->ti_Data; \
    } \
    else \
      test_init = FALSE; \
  }

#define INIT_BG(attr,var_dest,test_init) \
  { \
    var_dest = -1; \
    if((tag = FindTagItem(attr, msg->ops_AttrList))) \
    { \
      var_dest = tag->ti_Data; \
      test_init = TRUE; \
    } \
    else \
      test_init = FALSE; \
  }


#define LOAD_PEN(test,var_dest,cfg_attr,defaultval) \
  { \
    if (!test) \
    { \
      LONG ptrd; \
      if (DoMethod(obj, MUIM_GetConfigItem, cfg_attr, &ptrd)) \
        obtain_pen(data->mri, &var_dest, (struct MUI_PenSpec *) ptrd); \
      else \
        obtain_pen(data->mri, &var_dest, (struct MUI_PenSpec *) defaultval); \
    } \
    else \
      obtain_pen(data->mri, &var_dest, (struct MUI_PenSpec *) test); \
  }

#define LOAD_BG(test,var_dest,cfg_attr,defaultval) \
  { \
    if (!test) \
    { \
      LONG ptrd; \
      if (DoMethod(obj, MUIM_GetConfigItem, cfg_attr, &ptrd)) \
        var_dest = ptrd; \
      else \
        var_dest = (LONG) defaultval; \
    } \
  }


#ifdef __AROS__
AROS_HOOKPROTONH(NL_ConstructFunc_String, APTR, APTR, pool, char *, str)
#else
HOOKPROTONH(NL_ConstructFunc_String, APTR, APTR pool, char *str)
#endif
{
  HOOK_INIT
  
  char *new;
  LONG len = 0;

  while(str[len] != '\0' && str[len] != '\n' && str[len] != '\r' )
  {
    len++;
  }

  if((new = (char *)NL_Malloc2(pool, len+1, "DestructHook_String")))
  {
    memcpy(new, str, len*sizeof(char));
    new[len] = '\0'; // we have to terminate with a \0
  }

  return((APTR)new);
  
  HOOK_EXIT
}
MakeHook(NL_ConstructHook_String, NL_ConstructFunc_String);

#ifdef __AROS__
AROS_HOOKPROTONH(NL_DestructFunc_String, void, APTR, pool, char *, entry)
#else
HOOKPROTONH(NL_DestructFunc_String, void, APTR pool, char *entry)
#endif
{
  HOOK_INIT
  
  NL_Free2(pool,(void *) entry,"DestructHook_String");
  
  HOOK_EXIT
}
MakeHook(NL_DestructHook_String, NL_DestructFunc_String);

#ifdef __AROS__
AROS_HOOKPROTONHNO(NL_LayoutFuncNList, ULONG, struct MUI_LayoutMsg *, lm)
#else
HOOKPROTONHNO(NL_LayoutFuncNList, ULONG, struct MUI_LayoutMsg *lm)
#endif
{
  HOOK_INIT
  
  switch (lm->lm_Type)
  {
    case MUILM_MINMAX:
    {
      lm->lm_MinMax.MinWidth  = 1;
      lm->lm_MinMax.DefWidth  = 1;
      lm->lm_MinMax.MaxWidth  = 1;
      lm->lm_MinMax.MinHeight = 1;
      lm->lm_MinMax.DefHeight = 1;
      lm->lm_MinMax.MaxHeight = 1;

      return(0);
      //return (MUILM_UNKNOWN);
    }
    break;

    case MUILM_LAYOUT:
    {
      Object *cstate = (Object *)lm->lm_Children->mlh_Head;
      Object *child;
      LONG mw,mh;

      while((child = NextObject(&cstate)))
      {
        mw = (LONG) _defwidth(child);
        mh = (LONG) _defheight(child);

        if(!MUI_Layout(child,lm->lm_Layout.Width+MUI_MAXMAX,lm->lm_Layout.Height+MUI_MAXMAX,mw,mh,0))
        {
          /*return(FALSE);*/
        }
      }

      return(TRUE);
    }
    break;
  }

  return(MUILM_UNKNOWN);
  
  HOOK_EXIT
}
MakeStaticHook(NL_LayoutHookNList, NL_LayoutFuncNList);

#ifdef __AROS__
AROS_HOOKPROTONHNO(NL_LayoutFuncGroup, ULONG, struct MUI_LayoutMsg *, lm)
#else
HOOKPROTONHNO(NL_LayoutFuncGroup, ULONG, struct MUI_LayoutMsg *lm)
#endif
{
  HOOK_INIT
  
  switch (lm->lm_Type)
  {
    case MUILM_MINMAX:
    {
      lm->lm_MinMax.MinWidth  = MUI_MAXMAX+100;
      lm->lm_MinMax.DefWidth  = MUI_MAXMAX+100;
      lm->lm_MinMax.MaxWidth  = MUI_MAXMAX+100;
      lm->lm_MinMax.MinHeight = MUI_MAXMAX+100;
      lm->lm_MinMax.DefHeight = MUI_MAXMAX+100;
      lm->lm_MinMax.MaxHeight = MUI_MAXMAX+100;

      lm->lm_MinMax.MinWidth  = 2;
      lm->lm_MinMax.DefWidth  = 20;
      lm->lm_MinMax.MaxWidth  = MUI_MAXMAX+100;
      lm->lm_MinMax.MinHeight = 2;
      lm->lm_MinMax.DefHeight = 20+100;
      lm->lm_MinMax.MaxHeight = MUI_MAXMAX+100;

      return(0);
      //return (MUILM_UNKNOWN);
    }
    break;

    case MUILM_LAYOUT:
    {
      Object *cstate = (Object *)lm->lm_Children->mlh_Head;
      Object *child;
      LONG mw,mh;

      while((child = NextObject(&cstate)))
      {
        mw = (LONG) _defwidth(child);
        mh = (LONG) _defheight(child);

        if (!MUI_Layout(child,0,0,mw,mh,0))
        {
          /*return(FALSE);*/
        }
      }
      return(TRUE);
    }
    break;
  }

  return(MUILM_UNKNOWN);
  
  HOOK_EXIT
}
MakeStaticHook(NL_LayoutHookGroup, NL_LayoutFuncGroup);


void release_pen(struct MUI_RenderInfo *mri, ULONG *pen)
{
  if(*pen != -1)
  { MUI_ReleasePen(mri, *pen);
    *pen = -1;
  }
}


void obtain_pen(struct MUI_RenderInfo *mri, ULONG *pen, struct MUI_PenSpec *ps)
{
  release_pen(mri, pen);
  *pen = MUI_ObtainPen(mri, ps, 0);
}



static LONG Calc_Stack(Object *obj,struct NLData *data)
{
  LONG total;

#ifdef MORPHOS
    return 100000;
#endif


  if (data->NList_Process && data->NList_SPLower)
  {
/*
 *     if ((data->NList_Process->pr_Task.tc_Node.ln_Type == NT_PROCESS) && (data->NList_Process->pr_CLI != NULL))
 *     { data->NList_SPUpper = (char *) data->NList_Process->pr_ReturnAddr + sizeof(ULONG);
 *       total = *(LONG *) data->NList_Process->pr_ReturnAddr;
 *       data->NList_SPLower = data->NList_SPUpper - ((char *) total);
 *     }
 *     else
 */
    { data->NList_SPUpper = (char *) data->NList_Process->pr_Task.tc_SPUpper;
      data->NList_SPLower = (char *) data->NList_Process->pr_Task.tc_SPLower;
      total = (LONG) (data->NList_SPUpper - data->NList_SPLower);
    }
    data->NList_SPLowest = data->NList_SPUpper;
    data->NList_SPmin = data->NList_SPLower + (data->NList_StackCheck*1024);

    STACK_CHECK;
    /* return the good result only if the current stack is between min and max stack ! */
    if ((data->NList_SPLowest > data->NList_SPLower) && (data->NList_SPLowest < data->NList_SPUpper))
      return (total);
  }
  /* if not a process or if current stack is bad against min and max then make fake values */
  data->NList_SPUpper = (char *) 0x7FFFFFFFL;
  data->NList_SPLower = NULL;
  data->NList_SPmin = data->NList_SPLower + (data->NList_StackCheck*1024);
  data->NList_SPLowest = data->NList_SPmin + 10;
  total = (LONG) (data->NList_SPUpper - data->NList_SPLower);
  return (100000);
}

#ifdef __AROS__
#if defined(__PPC__)
IPTR DoSuperNew(Class *cl, Object *obj, Tag tag1, ...) __stackparm;
#endif
IPTR DoSuperNew(Class *cl, Object *obj, Tag tag1, ...)
{
  AROS_SLOWSTACKMETHODS_PRE(tag1)
  
  retval = DoSuperNewTagList(cl, obj, NULL,  (struct TagItem *) AROS_SLOWSTACKMETHODS_ARG(tag1));

  AROS_SLOWSTACKMETHODS_POST
}
#elif !defined(__MORPHOS__)
Object * STDARGS VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  va_list args;

  #if defined(__amigaos4__)
  va_startlinear(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, va_getlinearva(args, ULONG), NULL);
  #else
  va_start(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, args, NULL);
  #endif

  va_end(args);

  return rc;
}
#endif

ULONG mNL_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct NLData *data;
  //$$$$Sensei: msg->ops_AttrList is changed to taglist EVERYWHERE in OM_NEW!!!
  // It should speed up a bit.
    struct TagItem    *tag;
    struct TagItem    *taglist    = msg->ops_AttrList;
  LONG DragType = MUIV_NList_DragType_None;
  LONG DragSortable = FALSE; /*FALSE*/
  LONG Draggable = FALSE;    /*FALSE*/
  LONG Dropable = TRUE;      /*TRUE*/
  LONG dropable = TRUE;      /*TRUE*/
  STRPTR ShortHelp = NULL;   // RHP: Added for Special ShortHelp
  APTR img_tr,grp;
  char *img_name = "noimage";

  if((tag = FindTagItem(MUIA_ShortHelp, taglist)))
    ShortHelp = (STRPTR)tag->ti_Data;

  if((tag = FindTagItem(MUIA_FixHeightTxt, taglist)))
    tag->ti_Tag = TAG_IGNORE;

  if((tag = FindTagItem(MUIA_FixWidthTxt, taglist)))
    tag->ti_Tag = TAG_IGNORE;

  if((tag = FindTagItem(MUIA_Draggable, taglist)))
  {
    Draggable = tag->ti_Data;
    tag->ti_Tag = TAG_IGNORE;
  }

  if((tag = FindTagItem(MUIA_Dropable, taglist)))
  {
    Dropable = dropable = tag->ti_Data;
    tag->ti_Tag = TAG_IGNORE;
  }

  if((tag = FindTagItem(MUIA_NList_DragSortable, taglist)) ||
     (tag = FindTagItem(MUIA_List_DragSortable, taglist)))
  {
    DragSortable = tag->ti_Data;
    tag->ti_Tag = TAG_IGNORE;
  }

  if((tag = FindTagItem(MUIA_NList_DragType, taglist)) ||
     (tag = FindTagItem(MUIA_Listview_DragType, taglist)))
  {
    DragType = tag->ti_Data;
    tag->ti_Tag = TAG_IGNORE;
  }

  if((DragType != MUIV_NList_DragType_None) || DragSortable)
    Draggable = TRUE;
  else if (Draggable)
    DragType = MUIV_NList_DragType_Default;
  else
  {
    DragType = MUIV_NList_DragType_None;
    DragSortable = FALSE;
  }

  if(DragSortable)
    dropable = TRUE;

kprintf("NGR_Class = %x\n", NGR_Class);

  obj = (Object *) DoSuperNew(cl,obj,
    MUIA_Group_LayoutHook, &NL_LayoutHookNList,
    MUIA_FillArea, FALSE,
    MUIA_Dropable, dropable,
    NoFrame,
      Child, grp = NewObject(NGR_Class->mcc_Class,NULL,
        MUIA_Group_LayoutHook, &NL_LayoutHookGroup,
        MUIA_FillArea, FALSE,
        NoFrame,
        Child, img_tr = MUI_NewObject(MUIC_Image,
          MUIA_FillArea,FALSE,
          MUIA_Image_Spec,img_name,
/*
 *         MUIA_Image_FontMatch, TRUE,
 *         MUIA_Font, Topaz_8,
 *         MUIA_Image_State, IDS_NORMAL,
 */
        TAG_DONE),
        /*Child, HVSpace,*/
      TAG_DONE),

    TAG_MORE, taglist
  );


  if (!obj)
    return(0);
  data = INST_DATA(cl,obj);
  data->this = obj;
  data->SETUP = FALSE;
  data->SHOW = FALSE;
  data->DRAW = 0;
  data->ncl = cl;
  data->ocl = OCLASS(obj);
  data->rp = NULL;
  data->NL_Group = grp;
  data->NList_Process = (struct Process *) FindTask(NULL);
  data->NList_StackCheck = 2;
  data->NList_SPLower = NULL;
  Calc_Stack(obj,data);
  data->NList_SPLower = (char *) 4L;
  data->NList_Title = NULL;
  data->NList_TitleSeparator = TRUE;
  data->NList_TitleMark = MUIV_NList_TitleMark_None;
  data->NList_TitleMark2 = MUIV_NList_TitleMark2_None;
  data->NList_LastInserted = -1;
  data->NList_Quiet = 0;
  data->NList_AffActive = MUIV_NList_Active_Off;
  data->NList_Active = MUIV_NList_Active_Off;
  data->NList_Smooth = 0;
  data->VertPropObject = NULL;
  data->NList_AffFirst = 0;
  data->NList_AffFirst_Incr = 0;
  data->NList_First = 0;
  data->NList_First_Incr = 0;
  data->NList_Visible = 50000;
  data->NList_Entries = 0;
  data->NList_Prop_First = 0;
  data->NList_Prop_First_Prec = 0;
  data->NList_Prop_First_Real = 0;
  data->NList_Prop_Add = 0;
  data->NList_Prop_Wait = 2;
  data->NList_Prop_Visible = 0;
  data->NList_Prop_Entries = 0;
  data->NList_Horiz_AffFirst = 0;
  data->NList_Horiz_First = 0;
  data->NList_Horiz_Visible = 0;
  data->NList_Horiz_Entries = 0;
  data->old_prop_visible = -1;
  data->old_prop_entries = -1;
  data->old_prop_delta = -1;
  data->old_horiz_visible = -1;
  data->old_horiz_entries = -1;
  data->old_horiz_delta = -1;
  data->NList_MultiSelect = MUIV_NList_MultiSelect_None;
  data->NList_Input = TRUE;
  data->NList_DefaultObjectOnClick = TRUE;
  data->NList_KeepActive = 0;
  data->NList_MakeActive = 0;
  data->NList_AutoVisible = FALSE;
  data->NList_Font = 0;
  data->NList_DragType = DragType;
  data->NList_Dropable = Dropable;
  data->NList_DragColOnly = -1;
  data->NList_DragSortable = DragSortable;
  data->NList_DropMark = -1;
  data->NList_SortType = MUIV_NList_SortType_None;
  data->NList_SortType2 = MUIV_NList_SortType_None;
  data->NList_ButtonClick = -1;
  data->NList_SerMouseFix = 0;
  data->NList_Keys = default_keys;
  data->NList_ShortHelp = ShortHelp; // RHP: Added for Special ShortHelp
  data->Wheel_Keys = NULL;
  data->pushtrigger = 0;
  data->marktype = 0;
  data->NList_ShowDropMarks = TRUE;
  data->NImage2 = NULL;
  data->NImage.NImgObj = img_tr;
  data->NImage.ImgName = img_name;
  data->NImage.next = NULL;
  data->multiselect = MUIV_NList_MultiSelect_None;
  data->multisel_qualifier = 0;
  data->drag_qualifier = 0;
  data->InUseFont = NULL;
  data->badrport = FALSE;
  data->moves = FALSE;
  data->multiclick = 0;
  data->multiclickalone = 0;
  data->click_line = -2;
  data->click_x = 0;
  data->click_y = 0;
  data->NList_SelectChange = FALSE;
  data->EntriesArray = NULL;
  data->LastEntry = 0;
  data->vpos = 1;
  data->voff = 1;
  data->vinc = 1;
  data->addvinc = 1;
  data->hpos = 1;
  data->adding_member = 0;
  data->format_chge = 1;
  data->do_draw_all = TRUE;
  data->do_draw_title = TRUE;
  data->do_draw_active = TRUE;
  data->do_draw = TRUE;
  data->do_parse = TRUE;
  data->do_setcols = TRUE;
  data->do_updatesb = TRUE;
  data->do_wwrap = TRUE;
  data->force_wwrap = FALSE;
  data->do_images = TRUE;
  data->nodraw = 1;
  data->drawsuper = FALSE;
  data->dropping = FALSE;
  data->markdraw = FALSE;
  data->markerase = FALSE;
  data->markdrawnum = -1;
  data->markerasenum = -1;
  data->NumIntuiTick = 0;
  data->sorted = FALSE;
  data->selectmode = TE_Select_Line;
  data->first_change = LONG_MAX;
  data->last_change = LONG_MIN;
  data->lastselected = MUIV_NList_Active_Off;
  data->lastactived = MUIV_NList_Active_Off;
  data->selectskiped = FALSE;
  data->NList_ListBackGround = -1;
  data->actbackground = -1;
  data->NList_CompareHook = NULL;
  data->NList_ConstructHook = NULL;
  data->NList_DestructHook = NULL;
  data->NList_DisplayHook = NULL;
  data->NList_MultiTestHook = NULL;
  data->NList_CopyEntryToClipHook = NULL;
  data->NList_CopyColumnToClipHook = NULL;
  data->NList_CompareHook2 = FALSE;
  data->NList_ConstructHook2 = FALSE;
  data->NList_DestructHook2 = FALSE;
  data->NList_DisplayHook2 = FALSE;
  data->NList_CopyEntryToClipHook2 = FALSE;
  data->NList_CopyColumnToClipHook2 = FALSE;
/*
  data->NList_Pool = NULL;
  data->NList_PoolPuddleSize = 2008;
  data->NList_PoolThreshSize = 1024;
*/
  data->NList_MinLineHeight = 5;
  data->MinImageHeight = 5;
  data->NList_AdjustHeight = 0;
  data->NList_AdjustWidth = 0;
  data->NList_SourceArray = 0;
  data->NList_DefClickColumn = 0;
  data->NList_AutoCopyToClip = TRUE;
  data->NList_UseImages = NULL;
  data->NList_TabSize = 8;
  data->NList_SkipChars = NULL;
  data->NList_WordSelectChars = NULL;
  data->NList_EntryValueDependent = FALSE;
  data->NList_DragLines = DEFAULT_DRAGLINES;
  data->NList_WheelStep = 1;
  data->NList_WheelFast = 5;
  data->NList_WheelMMB = FALSE;
  data->NList_PrivateData = NULL;
  data->NList_ContextMenu = data->ContextMenu = data->ContextMenuOn = MUIV_NList_ContextMenu_Default;
  data->ListCompatibility = FALSE;
  data->NList_Disabled = FALSE;
  data->MenuObj = NULL;
  data->LastImage = 0;
  data->DragRPort = NULL;
  data->mri = NULL;
  data->cols = NULL;
  data->Title_PixLen = -1;
  data->numcols = 0;
  data->numcols2 = 0;
  data->column[0] = -1;
  data->aff_infos = NULL;
  data->numaff_infos = 0;
  data->spacesize = 6;
  data->tabsize = data->spacesize * data->NList_TabSize;
//  data->NList_Pool_Given = FALSE;
  data->NList_TypeSelect = MUIV_NList_TypeSelect_Line;
  data->min_sel = 1;
  data->max_sel = 1;
  data->sel_pt[0].ent = -1;
  data->sel_pt[1].ent = -1;
  data->minx_change_entry = -1;
  data->maxx_change_entry = -1;
  data->drag_border = FALSE;
  data->ScrollBarsPos = -4;
  data->ScrollBars = 0;
  data->ScrollBarsOld = 0;
  data->ScrollBarsTime = 0;
  data->display_ptr = NULL;
  data->Notify = 0;
  data->DoNotify = 0;
  data->Notifying = 0;
  data->TitleClick = -1;
  data->TitleClick2 = -1;
  data->NList_ForcePen = MUIV_NList_ForcePen_Default;
  data->ForcePen = FALSE;
  data->UpdatingScrollbars = FALSE;
  data->UpdateScrollersRedrawn = FALSE;
  data->drawall_bits = 0;
  data->drawall_dobit = 0;
  data->refreshing = FALSE;
  data->VirtGroup = NULL;
  data->VirtGroup2 = NULL;
  data->VirtClass = NULL;
  data->NList_ColWidthDrag = DEFAULT_CWD;
  data->NList_PartialCol = TRUE;
  data->NList_PartialChar = FALSE;
  data->NList_List_Select = MUIV_NList_Select_List;
  data->NList_MinColSortable = 1;
  data->NList_Imports = MUIV_NList_Imports_Active | MUIV_NList_Imports_First | MUIV_NList_Imports_Cols;
  data->NList_Exports = MUIV_NList_Exports_Active | MUIV_NList_Exports_First | MUIV_NList_Exports_Cols;
  data->listobj = NULL;
  data->scrollersobj = NULL;
  data->affover = -1; // RHP: Added for Shorthelp
  data->affbutton = -1;
  data->affbuttonline = -1;
  data->affbuttoncol = -1;
  data->affbuttonstate = 0;
  data->storebutton = TRUE;
  data->PointerObj = NULL;
  data->PointerObj2 = NULL;
  data->MOUSE_MOVE = FALSE;
  data->pad1 = -1;
  data->pad2 = TRUE;
  data->nlie = NULL;

/*D(bug("%lx|NEW 1 \n",obj));*/

    //$$$Sensei: major rewrite memory handling. PuddleSize and ThreshSize takes memory, nothing else.
    /* User pool was specified passed? */
    if(    ( tag = FindTagItem( MUIA_NList_Pool, taglist ) ) ||
            ( tag = FindTagItem( MUIA_List_Pool, taglist ) ) )
    {
        data->Pool = (APTR) tag->ti_Data;
    }
    else
    {

        /* Create internal pool using specified parameters or default one. */
        data->Pool    = data->PoolInternal    = NL_Pool_Create(
            GetTagData( MUIA_NList_PoolPuddleSize, GetTagData( MUIA_List_PoolPuddleSize, MUIV_NList_PoolPuddleSize_Default, taglist ), taglist ),
            GetTagData( MUIA_NList_PoolThreshSize, GetTagData( MUIA_List_PoolThreshSize, MUIV_NList_PoolThreshSize_Default, taglist ), taglist ) );

    }

    /* Is pool available? */
    if(!data->Pool)
    {
      /* It's not available, so we've to dispose object and return error. */
      CoerceMethod( cl, obj, OM_DISPOSE );
      return(0);
    }

  if ((tag = FindTagItem(MUIA_NList_ConstructHook, taglist)) ||
      (tag = FindTagItem(MUIA_List_ConstructHook, taglist)))
  { if (tag->ti_Data == MUIV_NList_ConstructHook_String)
      data->NList_ConstructHook = (struct Hook *) &NL_ConstructHook_String;
    else
      data->NList_ConstructHook = (struct Hook *) tag->ti_Data;
  }
  if ((tag = FindTagItem(MUIA_NList_DestructHook, taglist)) ||
      (tag = FindTagItem(MUIA_List_DestructHook, taglist)))
  { if (tag->ti_Data == MUIV_NList_DestructHook_String)
      data->NList_DestructHook = (struct Hook *) &NL_DestructHook_String;
    else
      data->NList_DestructHook = (struct Hook *) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_ListCompatibility, taglist)))
    data->ListCompatibility = TRUE;
  else
    data->ListCompatibility = FALSE;

  if((tag = FindTagItem(MUIA_Disabled, taglist)))
    data->NList_Disabled = 1;
  else
    data->NList_Disabled = FALSE;

  if((tag = FindTagItem(MUIA_NList_CompareHook, taglist)) ||
     (tag = FindTagItem(MUIA_List_CompareHook, taglist)))
  {
    data->NList_CompareHook = (struct Hook *) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_DisplayHook, taglist)) ||
     (tag = FindTagItem(MUIA_List_DisplayHook, taglist)))
  {
    data->NList_DisplayHook = (struct Hook *) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_MultiTestHook, taglist)) ||
     (tag = FindTagItem(MUIA_List_MultiTestHook, taglist)))
  {
    data->NList_MultiTestHook = (struct Hook *) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_CopyEntryToClipHook, taglist)))
    data->NList_CopyEntryToClipHook = (struct Hook *) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_CopyColumnToClipHook, taglist)))
    data->NList_CopyColumnToClipHook = (struct Hook *) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_ConstructHook2, taglist)))
  {
    data->NList_ConstructHook = (struct Hook *) tag->ti_Data;
    data->NList_ConstructHook2 = TRUE;
  }

  if((tag = FindTagItem(MUIA_NList_DestructHook2, taglist)))
  {
    data->NList_DestructHook = (struct Hook *) tag->ti_Data;
    data->NList_DestructHook2 = TRUE;
  }

  if((tag = FindTagItem(MUIA_NList_CompareHook2, taglist)))
  {
    data->NList_CompareHook = (struct Hook *) tag->ti_Data;
    data->NList_CompareHook2 = TRUE;
  }

  if((tag = FindTagItem(MUIA_NList_DisplayHook2, taglist)))
  {
    data->NList_DisplayHook = (struct Hook *) tag->ti_Data;
    data->NList_DisplayHook2 = TRUE;
  }

  if((tag = FindTagItem(MUIA_NList_CopyEntryToClipHook2, taglist)))
  {
    data->NList_CopyEntryToClipHook = (struct Hook *) tag->ti_Data;
    data->NList_CopyEntryToClipHook2 = TRUE;
  }

  if((tag = FindTagItem(MUIA_NList_CopyColumnToClipHook2, taglist)))
  {
    data->NList_CopyColumnToClipHook = (struct Hook *) tag->ti_Data;
    data->NList_CopyColumnToClipHook2 = TRUE;
  }

  if((tag = FindTagItem(MUICFG_NList_ForcePen, taglist)))
    data->NList_ForcePen = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_Format, taglist)) ||
     (tag = FindTagItem(MUIA_List_Format, taglist)))
  {
    data->NList_Format = (char *) tag->ti_Data;
    if (!NL_Read_Format(obj,data,(char *) tag->ti_Data,(tag->ti_Tag == MUIA_List_Format)))
    {
      CoerceMethod(cl, obj, OM_DISPOSE);
      return(0);
    }
  }
  else
  {
    data->NList_Format = NULL;
    if (!NL_Read_Format(obj,data,"",FALSE))
    {
      CoerceMethod(cl, obj, OM_DISPOSE);
      return(0);
    }
  }

  if (!NeedAffInfo(obj,data,AFFINFOS_START_MAX))
  {
    CoerceMethod(cl, obj, OM_DISPOSE);
    return(0);
  }
/*D(bug("%lx|NEW 5 \n",obj));*/

  if ((tag = FindTagItem(MUIA_NList_Input, taglist)) ||
      (tag = FindTagItem(MUIA_Listview_Input, taglist)))
    data->NList_Input = tag->ti_Data;

  if(!FindTagItem(MUIA_Frame, taglist))
  {
    if (data->NList_Input)
    {
      nnset(obj,MUIA_Frame, MUIV_Frame_InputList);
    }
    else
    {
      nnset(obj,MUIA_Frame, MUIV_Frame_ReadList);
    }
  }

  if((tag = FindTagItem(MUIA_ContextMenu, taglist)))
  {
    data->NList_ContextMenu = data->ContextMenu = data->ContextMenuOn = tag->ti_Data;
  }
  else
    notdoset(obj,MUIA_ContextMenu,data->ContextMenu);

  if((tag = FindTagItem(MUIA_Font, taglist)))
    data->NList_Font = tag->ti_Data;
  else if (!data->ListCompatibility)
    data->NList_Font = MUIV_NList_Font;
  else
    data->NList_Font = MUIV_Font_List;

  if((tag = FindTagItem(MUIA_NList_AutoCopyToClip, taglist)))
    data->NList_AutoCopyToClip = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_TabSize, taglist)))
    data->NList_TabSize = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_SkipChars, taglist)))
    data->NList_SkipChars = (char *) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_WordSelectChars, taglist)))
    data->NList_WordSelectChars = (char *) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_EntryValueDependent, taglist)))
    data->NList_EntryValueDependent = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_PrivateData, taglist)))
    data->NList_PrivateData = (APTR) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_Title, taglist)) ||
     (tag = FindTagItem(MUIA_List_Title, taglist)))
  {
    data->NList_Title = (char *) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_TitleSeparator, taglist)))
    data->NList_TitleSeparator = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_TitleClick, taglist)))
    WANT_NOTIFY(NTF_TitleClick);

  if((tag = FindTagItem(MUIA_NList_TitleClick2, taglist)))
    WANT_NOTIFY(NTF_TitleClick2);

  if((tag = FindTagItem(MUIA_NList_MultiSelect, taglist)) ||
     (tag = FindTagItem(MUIA_Listview_MultiSelect, taglist)))
  {
    data->multiselect = data->NList_MultiSelect = (LONG) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_DefaultObjectOnClick, taglist)))
    data->NList_DefaultObjectOnClick = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_MinLineHeight, taglist)))
    data->NList_MinLineHeight = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_List_MinLineHeight, taglist)))
    data->NList_MinLineHeight = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_TypeSelect, taglist)))
    data->NList_TypeSelect = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_AutoVisible, taglist)) ||
     (tag = FindTagItem(MUIA_List_AutoVisible, taglist)))
  {
    data->NList_AutoVisible = (LONG) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_DefClickColumn, taglist)) ||
     (tag = FindTagItem(MUIA_Listview_DefClickColumn, taglist)))
  {
    data->NList_DefClickColumn = (LONG) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_ShowDropMarks, taglist)) ||
     (tag = FindTagItem(MUIA_List_ShowDropMarks, taglist)))
  {
    data->NList_ShowDropMarks = (LONG) tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NList_DragColOnly, taglist)))
    data->NList_DragColOnly = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_SortType, taglist)))
    data->NList_SortType = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_SortType2, taglist)))
    data->NList_SortType2 = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_MinColSortable, taglist)))
    data->NList_MinColSortable = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_Imports, taglist)))
    data->NList_Imports = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_Exports, taglist)))
    data->NList_Exports = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_TitleMark, taglist)))
    data->NList_TitleMark = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_TitleMark2, taglist)))
    data->NList_TitleMark2 = (LONG) tag->ti_Data;

  if((tag = FindTagItem(MUIA_NList_Columns, taglist)))
  {
    NL_Columns(obj,data,(BYTE *) tag->ti_Data);
  }

  if (data->NList_DragSortable)
    data->NList_ShowDropMarks = TRUE;

/*D(bug("%lx|NEW 6 \n",obj));*/

  INIT_PEN(MUIA_NList_TitlePen,   data->NList_TitlePen,   data->Pen_Title_init);
  INIT_PEN(MUIA_NList_ListPen,    data->NList_ListPen,    data->Pen_List_init);
  INIT_PEN(MUIA_NList_SelectPen,  data->NList_SelectPen,  data->Pen_Select_init);
  INIT_PEN(MUIA_NList_CursorPen,  data->NList_CursorPen,  data->Pen_Cursor_init);
  INIT_PEN(MUIA_NList_UnselCurPen,data->NList_UnselCurPen,data->Pen_UnselCur_init);

  INIT_BG(MUIA_NList_TitleBackground,   data->NList_TitleBackGround,   data->BG_Title_init);
  INIT_BG(MUIA_Background,              data->NList_ListBackGround,    data->BG_List_init);
  if (!data->BG_List_init)
    INIT_BG(MUIA_NList_ListBackground,  data->NList_ListBackGround,    data->BG_List_init);
  INIT_BG(MUIA_NList_SelectBackground,  data->NList_SelectBackground,  data->BG_Select_init);
  INIT_BG(MUIA_NList_CursorBackground,  data->NList_CursorBackground,  data->BG_Cursor_init);
  INIT_BG(MUIA_NList_UnselCurBackground,data->NList_UnselCurBackground,data->BG_UnselCur_init);

/*
 *   for (tags=((struct opSet *)msg)->ops_AttrList;tag=NextTagItem(&tags);)
 *   { switch (tag->ti_Tag)
 *     { case MYATTR_PEN:
 *         if (tag->ti_Data)
 *           data->penspec = *((struct MUI_PenSpec *)tag->ti_Data);
 *         break;
 *     }
 *   }
 */

/*D(bug("%lx|NEW 7 \n",obj));*/
  if((tag = FindTagItem(MUIA_NList_AdjustHeight, taglist)))
  {
    if (tag->ti_Data)
      data->NList_AdjustHeight = -1;
  }

  if((tag = FindTagItem(MUIA_NList_AdjustWidth, taglist)))
  {
    if (tag->ti_Data)
      data->NList_AdjustWidth = -1;
  }

  if((tag = FindTagItem(MUIA_NList_SourceInsert, taglist)) && tag->ti_Data)
  {
    struct MUIP_NList_InsertWrap *ins = (struct MUIP_NList_InsertWrap *) tag->ti_Data;

    NL_List_Insert(data,obj,ins->entries,ins->count,ins->pos,ins->wrapcol,ins->align & ALIGN_MASK,0);

    if(data->NList_Entries > 0)
      data->NList_SourceArray = 1;

    if((tag = FindTagItem(MUIA_NList_First, taglist)))
      NL_List_First(obj,data,(long) tag->ti_Data,tag);

    if((tag = FindTagItem(MUIA_NList_Active, taglist)))
      NL_List_Active(obj,data,(long) tag->ti_Data,tag,MUIV_NList_Select_None,FALSE);
  }
  else if((tag = FindTagItem(MUIA_NList_SourceString, taglist)) && tag->ti_Data)
  {
    NL_List_Insert(data,obj,(APTR *) tag->ti_Data,-2,MUIV_NList_Insert_Bottom,0,0,0);

    if(data->NList_Entries > 0)
      data->NList_SourceArray = 1;

    if((tag = FindTagItem(MUIA_NList_First, taglist)))
      NL_List_First(obj,data,(long) tag->ti_Data,tag);

    if((tag = FindTagItem(MUIA_NList_Active, taglist)))
      NL_List_Active(obj,data,(long) tag->ti_Data,tag,MUIV_NList_Select_None,FALSE);
  }
  else if((tag = FindTagItem(MUIA_NList_SourceArray, taglist)) ||
          (tag = FindTagItem(MUIA_List_SourceArray, taglist)))
  {
    NL_List_Insert(data,obj,(APTR *) tag->ti_Data,-1,MUIV_NList_Insert_Bottom,0,0,0);

    if(data->NList_Entries > 0)
      data->NList_SourceArray = 1;

    if((tag = FindTagItem(MUIA_NList_First, taglist)))
      NL_List_First(obj,data,(long) tag->ti_Data,tag);

    if((tag = FindTagItem(MUIA_NList_Active, taglist)))
      NL_List_Active(obj,data,(long) tag->ti_Data,tag,MUIV_NList_Select_None,FALSE);
  }

  data->ihnode.ihn_Object  = obj;
  data->ihnode.ihn_Millis  = 30;
  data->ihnode.ihn_Method  = MUIM_NList_Trigger;
  data->ihnode.ihn_Flags   = MUIIHNF_TIMER;

  if((tag = FindTagItem(MUIA_NList_StackCheck, taglist)))
  {
    if (!tag->ti_Data)
      data->NList_SPLower = NULL;
  }

  if (data->NList_SPLower && Calc_Stack(obj,data) < 8000)
  {
    NL_Stack_Alert(obj,data,1);
    CoerceMethod(cl, obj, OM_DISPOSE);
    return(0);
  }

  set(obj,MUIA_FillArea,(LONG) FALSE);

/*D(bug("%lx|NEW 9 \n",obj));*/

  return((ULONG)obj);
}


ULONG mNL_Dispose(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NLData *data;
  data = INST_DATA(cl,obj);

/*D(bug("%lx|mNL_Dispose() 1 \n",obj));*/

  data->NList_Quiet = 1;
  data->SETUP = 3;

  NL_List_Clear(data,obj);

  data->format_chge = data->do_draw_all = data->do_draw_title = data->do_draw_active = data->do_draw = FALSE;
  data->do_parse = data->do_images = data->do_setcols = data->do_updatesb = data->do_wwrap = data->force_wwrap = FALSE;
  data->Notify = data->DoNotify = data->Notifying = 0;

  if (data->MenuObj)
  { MUI_DisposeObject(data->MenuObj);
    data->MenuObj = NULL;
  }

  DeleteNImages2(obj,data);

  DeleteNImages(obj,data);

  if (data->NList_UseImages)
    NL_Free(data,data->NList_UseImages,"Dispose_useimages");
  data->NList_UseImages = NULL;
  data->LastImage = 0;

  if (data->nlie)
  { NL_Free(data,(void *)data->nlie,"Dispose_Export struct");
    data->nlie = NULL;
  }

  FreeAffInfo(obj,data);

  NL_Free_Format(obj,data);

  //$$$Sensei
  NL_Pool_Delete( data->PoolInternal );

  return(DoSuperMethodA(cl,obj,msg));

}



ULONG mNL_Setup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg)
{
  register struct NLData *data;
  LONG ent;
  data = INST_DATA(cl,obj);
  STACK_CHECK;
  STACK_ALERT;

/*D(bug("%lx|mNL_Setup() 1 \n",obj));*/

  data->DRAW = 0;
  data->SETUP = FALSE;

  if (data->NList_Disabled)
    data->NList_Disabled = 1;

  if (data->NList_AdjustWidth)
    data->NList_AdjustWidth = -1;

  GetImages(obj,data);

  data->SETUP = TRUE;

/*  data->MinImageHeight = 5;*/
  data->display_ptr = NULL;
  if (data->NList_Font && !data->InUseFont)
  {
    char *fontname = NULL;
    ULONG fonttmp = data->NList_Font;
    if (data->NList_Font == MUIV_NList_Font)
    { fonttmp = MUIV_Font_List;
      DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_Font, &fontname);
    }
    else if (data->NList_Font == MUIV_NList_Font_Little)
    { fonttmp = MUIV_Font_Tiny;
      DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_Font_Little, &fontname);
    }
    else if (data->NList_Font == MUIV_NList_Font_Fixed)
    { fonttmp = MUIV_Font_Fixed;
      DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_Font_Fixed, &fontname);
    }
    if (fontname && *fontname)
    {
      struct TextAttr myta;
      LONG fsize = 8;
      int pp=0;
      char fname[32];
      while ((fontname[pp] != '/') && (fontname[pp] != '\0'))
      { if (pp < 42)
          fname[pp] = fontname[pp];
        pp++;
      }
      if (fontname[pp] != '\0')
        fsize = atol(&fontname[pp+1]);
      fname[pp++]='.'; fname[pp++]='f'; fname[pp++]='o'; fname[pp++]='n'; fname[pp++]='t'; fname[pp++]='\0';
      myta.ta_Name = fname;
      myta.ta_YSize = fsize;
      myta.ta_Style = 0;
      myta.ta_Flags = 0;
      data->InUseFont = OpenDiskFont(&myta);
    }
    if (data->InUseFont)
    { notdoset(obj,MUIA_Font,data->InUseFont);   /*_font(obj) = data->InUseFont;*/ }
    else
    { notdoset(obj,MUIA_Font,fonttmp); }
    NL_SetColsRem(obj,data,-2);
  }

  if (!(DoSuperMethodA(cl,obj,(Msg) msg)))
  {
    if (data->InUseFont)
    {
      notdoset(obj,MUIA_Font,0L);
      CloseFont(data->InUseFont);
      data->InUseFont = NULL;
    }
    return(FALSE);
  }

  data->rp = NULL;

  /*data->mri = msg->RenderInfo;*/
  data->mri = muiRenderInfo(obj);
  data->nodraw = 0;

  if (data->NList_MinLineHeight <= 0)
    data->addvinc = -data->NList_MinLineHeight;
  else
  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_VertInc, &ptrd))
      data->addvinc = *ptrd;
    else
      data->addvinc = DEFAULT_VERT_INC;
  }

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_StackCheck, &ptrd))
      data->NList_StackCheck = *ptrd + 1;
    else
      data->NList_StackCheck = 2;
    Calc_Stack(obj,data);
  }

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_ColWidthDrag, &ptrd))
      data->NList_ColWidthDrag = *ptrd;
    else
      data->NList_ColWidthDrag = DEFAULT_CWD;
  }

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_PartialCol, &ptrd))
      data->NList_PartialCol = *ptrd;
    else
      data->NList_PartialCol = TRUE;
  }

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_PartialChar, &ptrd))
      data->NList_PartialChar = *ptrd;
    else
      data->NList_PartialChar = 0;
  }

  { LONG *ptrd;
    data->NList_List_Select = MUIV_NList_Select_List;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_List_Select, &ptrd))
    { if (!*ptrd)
        data->NList_List_Select = MUIV_NList_Select_None;
    }
  }

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_SerMouseFix, &ptrd))
      data->NList_SerMouseFix = *ptrd;
    else
      data->NList_SerMouseFix = 0;
  }

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_DragLines, &ptrd))
      data->NList_DragLines = *ptrd + 1;
    else
      data->NList_DragLines = DEFAULT_DRAGLINES;
  }

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_WheelStep, &ptrd))
      data->NList_WheelStep = *ptrd;
    else
      data->NList_WheelStep = 1;
  }

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_WheelFast, &ptrd))
      data->NList_WheelFast = *ptrd;
    else
      data->NList_WheelFast = 5;
  }

  { LONG *ptrd;
    data->NList_WheelMMB = FALSE;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_WheelMMB, &ptrd))
    { if (*ptrd)
        data->NList_WheelMMB = TRUE;
    }
  }

  data->listobj = NULL;
  { Object *tagobj,*o = obj;
    if (!get(o,MUIA_Parent,&o))
      o = NULL;
    while (o)
    { if (get(o,MUIA_Listview_List,&tagobj))
      { if ((tagobj == obj) && !get(o,MUIA_NListview_NList,&tagobj))
        { LONG tagval;
          data->listobj = o;
          WANT_NOTIFY(NTF_LV_Select);
          WANT_NOTIFY(NTF_LV_Doubleclick);
          WANT_NOTIFY(NTF_L_Active);
          WANT_NOTIFY(NTF_Entries);
          if (get(data->listobj,MUIA_Listview_DragType,&tagval))
            data->NList_DragType = tagval;

          if (data->pad2)
          { struct IClass *lcl,*lcl1,*lcl2,*lcl3,*lcl4;
            UBYTE *ldata = ((UBYTE *) o) + 178;
            lcl = lcl1 = lcl2 = lcl3 = lcl4 = OCLASS(o);
            while (lcl->cl_Super)     /* when loop is finished : */
            { lcl4 = lcl3;            /*  Listview  */
              lcl3 = lcl2;            /*  Group     */
              lcl2 = lcl1;            /*  Area      */
              lcl1 = lcl;             /*  Notify    */
              lcl = lcl->cl_Super;    /*  rootclass */
            }
            if (lcl4->cl_InstSize == 68)  /* data size of Listview.mui class in 3.8 */
            {
              data->multiselect = data->NList_MultiSelect = (LONG) ldata[43];
              data->NList_Input = (LONG) ldata[65];
              ldata[40] = ldata[41] = ldata[42] = ldata[43] = 0;
              ldata[64] = ldata[65] = 0;
            }
          }
        }
        break;
      }
      if (!get(o,MUIA_Parent,&o))
        o = NULL;
    }
  }

  if (data->NList_DragType == MUIV_NList_DragType_Default)
  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_DragType, &ptrd))
      data->drag_type = *ptrd;
    else
      data->drag_type = MUIV_NList_DragType_Immediate;
  }
  else
    data->drag_type = data->NList_DragType;

  if ((data->NList_DragSortable) && (data->drag_type == MUIV_NList_DragType_None))
  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_DragType, &ptrd))
      data->drag_type = *ptrd;
    else
      data->drag_type = MUIV_NList_DragType_Immediate;
  }

/*
  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_DragQualifier, &ptrd))
      data->drag_qualifier = *ptrd;
    else
      data->drag_qualifier = 0;
  }
*/

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_Smooth, &ptrd))
      data->NList_Smooth = *ptrd;
    else
      data->NList_Smooth = 0;
    if (data->VertPropObject)
    { if (data->NList_Smooth)
        set(data->VertPropObject,MUIA_Prop_DoSmooth, TRUE);
      else
        set(data->VertPropObject,MUIA_Prop_DoSmooth, FALSE);
    }
  }

  {
    LONG *ptrd;

    if (data->NList_Keys && (data->NList_Keys != default_keys))
    {
      NL_Free(data,data->NList_Keys,"NList_Keys");
      data->NList_Keys = default_keys;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_Keys, &ptrd))
    {
      struct KeyBinding *keys = (struct KeyBinding *) ptrd;
      LONG nk = 0;

      while (keys[nk].kb_KeyTag)
        nk++;

      if((data->NList_Keys = NL_Malloc(data,sizeof(struct KeyBinding) * (nk + 1),"NList_Keys")))
      {
        while (nk >= 0)
        {
          data->NList_Keys[nk] = keys[nk];
          nk--;
        }
      }
      else
        data->NList_Keys = default_keys;
    }
    else
      data->NList_Keys = default_keys;
  }

  LOAD_PEN(data->Pen_Title_init,   data->NList_TitlePen,   MUICFG_NList_Pen_Title,   DEFAULT_PEN_TITLE);
  LOAD_PEN(data->Pen_List_init,    data->NList_ListPen,    MUICFG_NList_Pen_List,    DEFAULT_PEN_LIST);
  LOAD_PEN(data->Pen_Select_init,  data->NList_SelectPen,  MUICFG_NList_Pen_Select,  DEFAULT_PEN_SELECT);
  LOAD_PEN(data->Pen_Cursor_init,  data->NList_CursorPen,  MUICFG_NList_Pen_Cursor,  DEFAULT_PEN_CURSOR);
  LOAD_PEN(data->Pen_UnselCur_init,data->NList_UnselCurPen,MUICFG_NList_Pen_UnselCur,DEFAULT_PEN_UNSELCUR);

  LOAD_BG(data->BG_Title_init,   data->NList_TitleBackGround,   MUICFG_NList_BG_Title,   DEFAULT_BG_TITLE);
  LOAD_BG(data->BG_List_init,    data->NList_ListBackGround,    MUICFG_NList_BG_List,    DEFAULT_BG_LIST);
  LOAD_BG(data->BG_Select_init,  data->NList_SelectBackground,  MUICFG_NList_BG_Select,  DEFAULT_BG_SELECT);
  LOAD_BG(data->BG_Cursor_init,  data->NList_CursorBackground,  MUICFG_NList_BG_Cursor,  DEFAULT_BG_CURSOR);
  LOAD_BG(data->BG_UnselCur_init,data->NList_UnselCurBackground,MUICFG_NList_BG_UnselCur,DEFAULT_BG_UNSELCUR);

  { LONG *ptrd;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_PointerColor, &ptrd) && *ptrd)
      NL_SpecPointerColors(*ptrd);
  }

  if (data->NList_ForcePen == MUIV_NList_ForcePen_Default)
  { LONG *ptrd, fpen = MUIV_NList_ForcePen_Off;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_ForcePen, &ptrd))
      fpen = *ptrd;
    data->ForcePen = (LONG) fpen;
  }
  else
    data->ForcePen = data->NList_ForcePen;
  for (ent = 0;ent < data->NList_Entries;ent++)
    data->EntriesArray[ent]->PixLen = -1;

  data->actbackground = -1;

  if (data->NList_SourceArray)
    data->NList_SourceArray = 2;

  data->multiselect = data->NList_MultiSelect;
  data->multisel_qualifier = 0;
  { LONG *multisel;
    LONG mult = MUIV_NList_MultiSelect_Shifted;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_MultiSelect, (LONG) (&multisel)))
      mult = *multisel;
    if (data->NList_MultiSelect == MUIV_NList_MultiSelect_Default)
      data->multiselect = mult & 0x0007;
    if ((mult & MUIV_NList_MultiSelect_MMB_On) == MUIV_NList_MultiSelect_MMB_On)
      data->multisel_qualifier = IEQUALIFIER_MIDBUTTON;
  }

  if (data->NList_ContextMenu == MUIV_NList_ContextMenu_Default)
  { LONG *ptrd;
    data->ContextMenu = MUIV_NList_ContextMenu_Always;
    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NList_Menu, &ptrd))
    { switch (*ptrd)
      { case MUIV_NList_ContextMenu_TopOnly :
        case MUIV_NList_ContextMenu_Always :
        case MUIV_NList_ContextMenu_Never :
          data->ContextMenu = *ptrd;
          break;
      }
    }
  }
  if (data->ContextMenu != data->ContextMenuOn)
  { if (/*(((data->ContextMenu & 0x9d510030) == 0x9d510030) && (data->numcols <= 1)) ||*/ /* sba: Contextmenu problem: Disabled */
        (data->ContextMenu == MUIV_NList_ContextMenu_Never))
      notdoset(obj,MUIA_ContextMenu,NULL);
    else
      notdoset(obj,MUIA_ContextMenu,data->ContextMenu);
  }

  data->ScrollBarsPos = -2;
  data->ScrollBars = 0;
  data->ScrollBarsTime = -1;

  if (LIBVER(GfxBase) >= 39)
    NL_CreateImages(obj,data);
  else
    data->ScrollBarsPos = -3;

  data->drawsuper = FALSE;
  data->format_chge = 1;
  data->do_draw_all = data->do_draw_title = data->do_draw = TRUE;
  data->do_parse = data->do_setcols = data->do_updatesb = data->do_wwrap = TRUE;
  /*data->force_wwrap = TRUE;*/
  data->badrport = FALSE;
  data->refreshing = FALSE;

  data->first_change = LONG_MAX;
  data->last_change = LONG_MIN;

  data->adjustbar = -1;
  data->adjustbar_old = -1;

  Make_Active_Visible;

  {
    Object *o = obj;
    LONG virtleft,virttop;
    data->VirtGroup = NULL;
    data->VirtGroup2 = NULL;
    if (!get(o,MUIA_Parent,&o))
      o = NULL;
    while (o)
    { if (get(o,MUIA_Virtgroup_Left,&virtleft) && get(o,MUIA_Virtgroup_Top,&virttop))
      { data->VirtGroup = o;
        if (!data->VirtClass)
        { o = MUI_NewObject(MUIC_Virtgroup,Child, MUI_NewObject(MUIC_Rectangle, TAG_DONE), TAG_DONE);
          if (o)
          { data->VirtClass = OCLASS(o);
            MUI_DisposeObject(o);
          }
        }
        break;
      }
      if (!get(o,MUIA_Parent,&o))
        o = NULL;
    }
    data->VirtGroup2 = data->VirtGroup;
  }

/*  MUI_RequestIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INTUITICKS|IDCMP_ACTIVEWINDOW|IDCMP_INACTIVEWINDOW);*/
/*  MUI_RequestIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INTUITICKS);*/

  data->MOUSE_MOVE = FALSE;
/*  data->ehnode.ehn_Events = (IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INTUITICKS|IDCMP_MOUSEMOVE);*/
  data->ehnode.ehn_Events = (IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INTUITICKS|IDCMP_ACTIVEWINDOW|IDCMP_INACTIVEWINDOW);
  data->ehnode.ehn_Priority = 0;
  data->ehnode.ehn_Flags = 0;
  data->ehnode.ehn_Object = obj;
  data->ehnode.ehn_Class  = cl;

  DoMethod(_win(obj),MUIM_Window_AddEventHandler, &data->ehnode);

  DoMethod(_app(obj),MUIM_Application_AddInputHandler,&data->ihnode);

/*  GetNImage_Sizes(obj,data);*/

  data->pad2 = FALSE;

/*D(bug("%lx|mNL_Setup() 2 \n",obj));*/

  return(TRUE);
}



ULONG mNL_Cleanup(struct IClass *cl,Object *obj,struct MUIP_Cleanup *msg)
{
  register struct NLData *data = INST_DATA(cl,obj);
  ULONG retval;

/*D(bug("%lx|mNL_Cleanup() 1 \n",obj));*/

  data = INST_DATA(cl,obj);
  STACK_CHECK; STACK_ALERT;

  data->nodraw = 1;
  data->DRAW = 0;

  DoMethod(_app(obj),MUIM_Application_RemInputHandler,&data->ihnode);

  DoMethod(_win(obj),MUIM_Window_RemEventHandler, &data->ehnode);

/*  MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INTUITICKS|IDCMP_ACTIVEWINDOW|IDCMP_INACTIVEWINDOW);*/
/*  MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INTUITICKS);*/

  data->NList_Quiet++;
  data->VirtGroup = NULL;

  NL_DeleteImages(obj,data);

  if (data->NList_Keys && (data->NList_Keys != default_keys))
  { NL_Free(data,data->NList_Keys,"NList_Keys");
    data->NList_Keys = default_keys;
  }

  release_pen(data->mri, &data->NList_TitlePen);
  release_pen(data->mri, &data->NList_ListPen);
  release_pen(data->mri, &data->NList_SelectPen);
  release_pen(data->mri, &data->NList_CursorPen);
  release_pen(data->mri, &data->NList_UnselCurPen);

  retval = DoSuperMethodA(cl,obj,(Msg) msg);

  if (data->InUseFont)
  {
    notdoset(obj,MUIA_Font,0L);
    CloseFont(data->InUseFont);
    data->InUseFont = NULL;
  }

  data->voff = 1;

  data->rp = NULL;
  data->badrport = FALSE;
  data->mri = FALSE;
  data->UpdateScrollersRedrawn = FALSE;

  data->NList_Quiet--;
  data->SETUP = FALSE;

/*D(bug("%lx|mNL_Cleanup() 2 \n",obj));*/

  return (retval);
}

