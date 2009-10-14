#include "Private.h"

/***************************************************************************/
/*** Private ***************************************************************/
/***************************************************************************/

#if 0
///Window_Iterator
STATIC ASM void Window_Iterator(REG_A0 FWindow_EachFunc Func,REG_A1 APTR UserData,REG_A2 FObject Obj)
{
   if (Obj)
   {
      FObject sub;

      Func(Obj,UserData);

      for (sub = (FObject) F_Get(Obj,FA_Family_Head) ; sub ; sub = _next(sub))
      {
         Window_Iterator(Func,UserData,Obj);
      }
   }
}
//+
#endif

///Window_Cycle
FObject Window_Cycle(FClass *Class,FObject Obj,uint32 Direction)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   FCycleNode *node;

   if (LOD->ActiveObj)
   {
      /* first we search the current active object in the cycle list */

      for (node = (FCycleNode *) LOD->CycleChain.Head ; node ; node = node->Next)
      {
         if (node->Object == LOD->ActiveObj)
         {
            break;
         }
      }
   }
   else
   {
      node = NULL;
   }

   switch (Direction)
   {
      case FV_Window_ActiveObject_Next:
      {
         if (node)
         {
            for (node = node->Next ; node ; node = node->Next)
            {
               if (FF_Area_CanDraw & node->AreaPublic->Flags &&
                 !(FF_Area_Disabled & node->AreaPublic ->Flags ))
               {
                  break;
               }
            }

            if (!node)
            {
               for (node = (FCycleNode *) LOD->CycleChain.Head ; node ; node = node->Next)
               {
                  if (FF_Area_CanDraw & node->AreaPublic->Flags &&
                    !(FF_Area_Disabled & node->AreaPublic ->Flags ))
                  {
                     break;
                  }
               }
            }
         }
         else
         {
            node = (FCycleNode *)(LOD->CycleChain.Head);
         }
      }
      break;

      case FV_Window_ActiveObject_Prev:
      {
         if (node)
         {
            for (node = node->Prev ; node ; node = node->Prev)
            {
               if (FF_Area_CanDraw & node->AreaPublic->Flags &&
                 !(FF_Area_Disabled & node->AreaPublic ->Flags ))
               {
                  break;
               }
            }

            if (!node)
            {
               for (node = (FCycleNode *) LOD->CycleChain.Tail ; node ; node = node->Prev)
               {
                  if (FF_Area_CanDraw & node->AreaPublic->Flags &&
                    !(FF_Area_Disabled & node->AreaPublic ->Flags ))
                  {
                     break;
                  }
               }
            }
         }
         else
         {
            node = (FCycleNode *)(LOD->CycleChain.Tail);
         }
      }
      break;

      default:
      {
         return (FObject) Direction;
      }
   }

   if (node)
   {
      return node->Object;
   }
   return NULL;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Window_New
F_METHOD(uint32,Window_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    LOD->FramePublic = (FFramePublic *) F_Get(Obj,FA_Frame_PublicData);

    /* set default values */

    LOD->GADFlags = FF_WINDOW_GAD_DRAG  | FF_WINDOW_GAD_CLOSE |
                      FF_WINDOW_GAD_DEPTH | FF_WINDOW_GAD_ICONIFY;

    LOD->WINFlags = FF_WINDOW_WIN_ACTIVE;

    LOD->p_Scheme    = "$window-scheme";
    LOD->p_Decorator = "$decorator-class";

    /* Parse initial attributes */

    while  (F_DynamicNTI(&Tags,&item,NULL))
    switch (item.ti_Tag)
    {
        case FA_Child:                if (!item.ti_Data) return 0; break;

        case FA_Window_GadNone:       if (item.ti_Data) LOD->GADFlags  = 0; break;
        case FA_Window_GadDragbar:    if (item.ti_Data) LOD->GADFlags |= FF_WINDOW_GAD_DRAG;       else LOD->GADFlags &= ~FF_WINDOW_GAD_DRAG;       break;
        case FA_Window_GadClose:      if (item.ti_Data) LOD->GADFlags |= FF_WINDOW_GAD_CLOSE;      else LOD->GADFlags &= ~FF_WINDOW_GAD_CLOSE;      break;
        case FA_Window_GadDepth:      if (item.ti_Data) LOD->GADFlags |= FF_WINDOW_GAD_DEPTH;      else LOD->GADFlags &= ~FF_WINDOW_GAD_DEPTH;      break;
        case FA_Window_GadIconify:    if (item.ti_Data) LOD->GADFlags |= FF_WINDOW_GAD_ICONIFY;    else LOD->GADFlags &= ~FF_WINDOW_GAD_ICONIFY;    break;

        case FA_Window_Active:        if (item.ti_Data) LOD->WINFlags |= FF_WINDOW_WIN_ACTIVE;     else LOD->WINFlags &= ~FF_WINDOW_WIN_ACTIVE;     break;
        case FA_Window_Backdrop:      if (item.ti_Data) LOD->WINFlags |= FF_WINDOW_WIN_BACKDROP;   else LOD->WINFlags &= ~FF_WINDOW_WIN_BACKDROP;   break;
        case FA_Window_Borderless:    if (item.ti_Data) LOD->WINFlags |= FF_WINDOW_WIN_BORDERLESS; else LOD->WINFlags &= ~FF_WINDOW_WIN_BORDERLESS; break;

        /*** Preferences ***/

        case FA_ColorScheme:          LOD->p_Scheme    = (STRPTR)(item.ti_Data); break;
        case FA_Window_Decorator:     LOD->p_Decorator = (STRPTR)(item.ti_Data); break;
    }

    if (FF_WINDOW_WIN_BORDERLESS & LOD->WINFlags)
    {
        LOD->GADFlags = 0;
        LOD->p_Decorator = NULL;
    }

    return F_SuperDo(Class,Obj,Method,

       FA_Back,    "$window-back",
       FA_Frame,   "$window-frame",

    TAG_MORE,Msg);
}
//+
///Window_Dispose
F_METHOD(void,Window_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(Obj,FM_Window_Close);

    F_DisposeObj(LOD->Root); LOD->Root = NULL;

    F_SUPERDO();
}
//+
///Window_Get
F_METHOD(void,Window_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;
   struct Window *win = LOD->Win;

   F_SUPERDO();

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Child:                F_STORE(LOD->Root);         break;
      case FA_Parent:
      case FA_Application:          F_STORE(LOD->Application);  break;
//         case FA_ContextHelp:       F_STORE(self.contexthelp
//         CASE FA_ContextMenu         ; ^save := self.contextmenu

      case FA_Left:                 F_STORE(win ? win->LeftEdge : LOD->Box.x); break;
      case FA_Top:                  F_STORE(win ? win->TopEdge  : LOD->Box.y); break;
      case FA_Width:                F_STORE(win ? win->Width    : LOD->Box.w); break;
      case FA_Height:               F_STORE(win ? win->Height   : LOD->Box.h); break;
      case FA_Right:                F_STORE(win ? win->LeftEdge + win->Width  - 1 : LOD->Box.x + LOD->Box.w - 1); break;
      case FA_Bottom:               F_STORE(win ? win->TopEdge  + win->Height - 1 : LOD->Box.y + LOD->Box.h - 1); break;

      case FA_Back:                 F_STORE(LOD->FramePublic->Back);  break;
      case FA_ColorScheme:          F_STORE(LOD->Scheme);             break;
      case FA_DisabledColorScheme:  F_STORE(LOD->DisabledScheme);     break;

      case FA_Window:               F_STORE(win); break;
      case FA_Window_Open:          F_STORE(0 != (FF_WINDOW_SYS_OPENREQUEST & LOD->SYSFlags)); break;
      case FA_Window_Title:         F_STORE(LOD->WinTitle);  break;
      case FA_Window_ScreenTitle:   F_STORE(LOD->ScrTitle);  break;
      case FA_Window_ActiveObject:  F_STORE(LOD->ActiveObj); break;

      case FA_Window_GadDragbar:    F_STORE(0 != (FF_WINDOW_GAD_DRAG       & LOD->GADFlags)); break;
      case FA_Window_GadClose:      F_STORE(0 != (FF_WINDOW_GAD_CLOSE      & LOD->GADFlags)); break;
      case FA_Window_GadDepth:      F_STORE(0 != (FF_WINDOW_GAD_DEPTH      & LOD->GADFlags)); break;
      case FA_Window_GadIconify:    F_STORE(0 != (FF_WINDOW_GAD_ICONIFY    & LOD->GADFlags)); break;

      case FA_Window_Active:        F_STORE(0 != (FF_WINDOW_WIN_ACTIVE     & LOD->WINFlags)); break;
      case FA_Window_Resizable:     F_STORE(0 != (FF_WINDOW_WIN_RESIZABLE  & LOD->WINFlags)); break;
      case FA_Window_Borderless:    F_STORE(0 != (FF_WINDOW_WIN_BORDERLESS & LOD->WINFlags)); break;
   }
}
//+
///Window_Set
F_METHOD(void,Window_Set)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,NULL))
   switch (item.ti_Tag)
   {
      case FA_Left:
      {
         if (item.ti_Data < 0xFFFF)
         {
            LOD->UserBox.x = item.ti_Data;
         }
         else
         {
            STRPTR str = (STRPTR)(item.ti_Data);
            int32 val;

            str += stcd_l(str,&val);

            LOD->UserBox.x = val;

            if (*str == '%') LOD->BOXFlags |= FF_WINDOW_BOX_XPERCENT;
            else             LOD->BOXFlags &= ~FF_WINDOW_BOX_XPERCENT;
         }
      }
      break;

      case FA_Top:
      {
         if (item.ti_Data < 0xFFFF)
         {
            LOD->UserBox.y = item.ti_Data;
         }
         else
         {
            STRPTR str = (STRPTR)(item.ti_Data);
            int32 val;

            str += stcd_l(str,&val);

            LOD->UserBox.y = val;

            if (*str == '%') LOD->BOXFlags |= FF_WINDOW_BOX_YPERCENT;
            else             LOD->BOXFlags &= ~FF_WINDOW_BOX_YPERCENT;
         }
      }
      break;

      case FA_Width:
      {
         if (item.ti_Data < 0xFFFF)
         {
            LOD->UserBox.w = item.ti_Data;
         }
         else
         {
            STRPTR str = (STRPTR)(item.ti_Data);
            int32 val;

            str += stcd_l(str,&val);

            LOD->UserBox.w = val;

            if (*str == '%') LOD->BOXFlags |= FF_WINDOW_BOX_WPERCENT;
            else             LOD->BOXFlags &= ~FF_WINDOW_BOX_WPERCENT;
         }
      }
      break;

      case FA_Height:
      {
         if (item.ti_Data < 0xFFFF)
         {
            LOD->UserBox.h = item.ti_Data;
         }
         else
         {
            STRPTR str = (STRPTR)(item.ti_Data);
            int32 val;

            str += stcd_l(str,&val);

            LOD->UserBox.h = val;

            if (*str == '%') LOD->BOXFlags |= FF_WINDOW_BOX_HPERCENT;
            else             LOD->BOXFlags &= ~FF_WINDOW_BOX_HPERCENT;
         }
      }
      break;

      case FA_Child:
      {
         F_Do(Obj,FM_AddMember,item.ti_Data,FV_AddMember_Tail);
      }
      break;

      case FA_ContextHelp:
      {
         LOD->ContextHelp = (STRPTR)(item.ti_Data);
      }
      break;

      case FA_Window_Open:
      {
         if (item.ti_Data) LOD->SYSFlags |= FF_WINDOW_SYS_OPENREQUEST;
         else              LOD->SYSFlags &= ~FF_WINDOW_SYS_OPENREQUEST;

         if (LOD->Render)
         {
            if (F_Get(LOD->Application,FA_Application_Sleep) == FALSE)
            {
               if (FF_WINDOW_SYS_OPENREQUEST & LOD->SYSFlags)
               {
                  F_Do(LOD->Application,FM_Application_PushMethod,Obj,FM_Window_Open,0);
               }
               else
               {
                  F_Do(LOD->Application,FM_Application_PushMethod,Obj,FM_Window_Close,0);
               }
            }
         }
      }
      break;

      case FA_Window_Title:
      {
         LOD->WinTitle = (STRPTR)(item.ti_Data);

         if (LOD->Win)
         {
            if (!LOD->Decorator && (FF_WINDOW_GAD_DRAG & LOD->GADFlags))
            {
               SetWindowTitles(LOD->Win,LOD->WinTitle,(STRPTR)(-1));
            }
            else
            {
               LOD->Win->Title = LOD->WinTitle;
            }
         }
      }
      break;

      case FA_Window_ScreenTitle:
      {
         if ((LOD->ScrTitle = (STRPTR)(item.ti_Data)) != NULL)
         {
            if (LOD->Win)
            {
               SetWindowTitles(LOD->Win,(STRPTR)(-1),LOD->ScrTitle);
            }
         }
      }
      break;

      case FA_Window_Active:
      {
         if (LOD->Win)
         {
            WindowToFront(LOD->Win);
            ActivateWindow(LOD->Win);
         }

         if (item.ti_Data)    LOD->WINFlags |= FF_WINDOW_WIN_ACTIVE;
         else                 LOD->WINFlags &= ~FF_WINDOW_WIN_ACTIVE;
      }
      break;

      case FA_Window_ActiveObject:
      {
         FObject ao = Window_Cycle(Class,Obj,item.ti_Data);

         if (ao != LOD->ActiveObj)
         {
            F_Set(LOD->ActiveObj,     FA_Active,FALSE);
            F_Set(LOD->ActiveObj = ao,FA_Active,TRUE);
         }
      }
      break;
   }

   F_SUPERDO();
}
//+
///Window_Import
F_METHODM(uint32,Window_Import,FS_Import)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   uint32 id = F_Get(Obj,FA_ID);

   //F_Log(0,"ID (%4.4s) Root %s{%08lx}",&LOD->ID,_classname(LOD->Root),LOD->Root);

   if (id)
   {
      FBox *data = (FBox *) F_Do(Msg->Dataspace,Msg->id_Find,id);

      if (data)
      {
         CopyMem((FBox *)(data),&LOD->Box,sizeof (FBox));
      }
      else
      {
         LOD->Box.w = 0;
         LOD->Box.h = 0;
      }
   }

   return F_OBJDO(LOD->Root);
}
//+
///Window_Export
F_METHODM(uint32,Window_Export,FS_Export)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 id = F_Get(Obj,FA_ID);

    if (id)
    {
        F_Do(Msg->Dataspace,Msg->id_Add,id,&LOD->Box,sizeof (FBox));
    }

    return F_OBJDO(LOD->Root);
}
//+
///Window_Connect
F_METHODM(uint32,Window_Connect,FS_Connect)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD->Application = Msg->Parent;

    return TRUE;
}
//+
///Window_Disconnect
F_METHOD(uint32,Window_Disconnect)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD->Application = NULL;

    return TRUE;
}
//+

///Window_AddMember
F_METHODM(APTR,Window_AddMember,FS_AddMember)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD->Root)
    {
        F_Log(FV_LOG_DEV,"Window can also have one child: Root %s{%08lx}",_classname(LOD->Root),LOD->Root);
    }
    else
    {
        if (Msg->Object)
        {
            _root_area_public = (FAreaPublic *) F_Get(Msg->Object,FA_Area_PublicData);

            if (_root_area_public)
            {
                if (F_Do(Msg->Object,FM_Connect,Obj,NULL))
                {
                    return (LOD->Root = Msg->Object);
                }
                else
                {
                    F_Log(FV_LOG_DEV,"%s{%08lx} refused connection",_classname(Msg->Object),Msg->Object);
                }
            }
            else
            {
                F_Log(FV_LOG_DEV,"Child must be a subclass of FC_Area: %s{%08lx}",_classname(Msg->Object),Msg->Object);
            }
        }
    }
    return NULL;
}
//+
///Window_RemMember
F_METHODM(void,Window_RemMember,FS_RemMember)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg->Member)
    {
        if (Msg->Member == LOD->Root)
        {
             F_Do(LOD->Root,FM_Disconnect);

             LOD->Root = NULL;
             _root_area_public = NULL;
        }
        else if (Msg->Member == LOD->Decorator)
        {
            F_Do(LOD->Decorator,FM_Disconnect);

            LOD->Decorator = NULL;
            _deco_area_public = NULL;
        }
        else
        {
            F_Log(0,"%s{%08lx} is not root nor decorator",_classname(Msg->Member),Msg->Member);
        }
    }
}
//+

#if 0
///Window_ForEach
F_METHODM(void,Window_ForEach,FS_Window_ForEach)
{
   Window_Iterator(Msg->Func,Msg->UserData,LOD->Root);
}
//+
#endif

/****************************************************************************
*** Preferences *************************************************************
****************************************************************************/

STATIC FPreferenceScript Script[] =
{
    { "$window-back",           FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$window-frame",          FV_TYPE_STRING,  "Spec",     "<frame id='50' padding-left='3' padding-top='3' padding-right='5' padding-bottom='5'/>",0 },
    { "$filedialog-back",       FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$filedialog-frame",      FV_TYPE_STRING,  "Spec",     "<frame id='50' padding-left='3' padding-top='3' padding-right='5' padding-bottom='5'/>",0 },
    { "$fontdialog-back",       FV_TYPE_STRING,  "Spec",     NULL,0 },
    { "$fontdialog-frame",      FV_TYPE_STRING,  "Spec",     "<frame id='50' padding-left='3' padding-top='3' padding-right='5' padding-bottom='5'/>",0 },
    { "$window-render-complex", FV_TYPE_INTEGER, "Selected", (STRPTR) FALSE,0 },
    { "$window-refresh-simple", FV_TYPE_INTEGER, "Selected", (STRPTR) FALSE,0 },

    F_ARRAY_END
};

extern struct FeelinCatalogEntry Catalog[];

F_METHOD(uint32,Prefs_New)
{
    F_XML_DEFS_INIT(5);

    F_XML_DEFS_ADD("msg:backgrounds",F_CAT(P_BACKGROUNDS));
    F_XML_DEFS_ADD("msg:windows",F_CAT(P_WINDOWS));
    F_XML_DEFS_ADD("msg:requesters",F_CAT(P_REQUESTERS));
    F_XML_DEFS_ADD("msg:rendering",F_CAT(P_RENDERING));
    F_XML_DEFS_ADD("msg:render",F_CAT(P_COMPLEX_RENDER));
    F_XML_DEFS_DONE;

    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, F_CAT(P_TITLE),

        "Script", Script,
        "XMLSource", "feelin/preference/window.xml",
        "XMLDefinitions", F_XML_DEFS,

    TAG_MORE, Msg);
}
