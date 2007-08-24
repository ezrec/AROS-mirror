#include "Private.h"

/*** Private ***************************************************************/

///code_list_display
STATIC F_HOOKM(FListDisplay *,code_list_display,FS_List_Display)
{
   struct FeelinPGroup *node = Msg->Entry;

   if (node)
   {
      Msg->Strings[0] = node->Name;
   }
   return NULL;
}
//+

/*** Methods ***************************************************************/

///PE_New
F_METHOD(APTR,PE_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD->h_Disp.h_Entry = (HOOKFUNC) F_FUNCTION_GATE(code_list_display);

    if (F_SuperDo(Class,Obj,Method,

//        FA_Window_GadClose,     FALSE,
        FA_Width,               "50%",
        FA_Height,              "50%",
        "FA_Dialog_Buttons",     FV_Dialog_Buttons_PreferenceTest,
        "FA_Dialog_Separator",   TRUE,
  
    TAG_MORE,Msg))
    {
        FObject root = HGroup,
           Child, VGroup, FA_SetMax,FV_SetWidth,
              Child, ListviewObject, "FA_Listview_List",
                 LOD->list = ListObject,
                    FA_SetMin,              FV_SetWidth,
                    FA_MinHeight,           50,
                   "FA_List_DisplayHook",   &LOD->h_Disp,
                 End,
              End,

              Child, VGroup, FA_SetMax,FV_SetHeight,
                 Child, HGroup, FA_Group_HSpacing, 2,
                    Child, PopFrameObject, DontChain, "Couple",TRUE, End,
                    Child, PopImageObject, DontChain, "Couple",TRUE, End,
                 End,
                 
                 Child, PopSchemeObject, DontChain, End,
              End,
           End,
           
            Child, LOD->Master = VGroup,
                Child, LOD->GroupTitle = TextObject,
                    FA_SetMax,        FV_SetHeight,
                    FA_ChainToCycle,  FALSE,
                    FA_Frame,        "$text-frame",
                    FA_Back,         "$text-back",
                    FA_ColorScheme,  "$text-scheme",
                    FA_Font,         "$text-font",
                    FA_Text,         "(about)",
                    FA_Text_PreParse,"<align=center>",
                End,
            End,
        End;
      
        if (root)
        {
            uint32 i=0;
            uint32 id_InsertSingle = F_DynamicFindID("FM_List_InsertSingle");
         
            struct FeelinPGroup *gnode;
         
            LOD->MList = F_SharedOpen(FC_ModulesList);
 
            if (LOD->MList)
            {
                FModuleCreated *cnode;
                
                for (cnode = (FModuleCreated *) F_Get(LOD->MList,(uint32) "FA_ModulesList_Prefs") ; cnode ; cnode = cnode->Next)
                {
                    gnode = F_New(sizeof (struct FeelinPGroup));

                    if (gnode)
                    {
                        if ((gnode->Object = F_NewObj(cnode->Class->Name,TAG_DONE)))
                        {
                            struct FeelinPGroup *gprev;

                            gnode->Name = (STRPTR) F_Get(gnode->Object,FA_Group_PageTitle);

                            for (gprev = (struct FeelinPGroup *) LOD->GroupList.Tail ; gprev ; gprev = gprev->Prev)
                            {
                               if (F_StrCmp(gnode->Name,gprev->Name,ALL) > 0) break;
                            }
                         
                            F_LinkInsert(&LOD->GroupList,(FNode *) gnode,(FNode *) gprev);
                        }
                        else
                        {
                            F_Log(FV_LOG_DEV,"Unable to create object from %s",cnode->Module->lib_Node.ln_Name);
                            F_Dispose(gnode);
                        }
                    }
                }
            }
            
            /* The 'About' group should always comes first */
            
            gnode = F_New(sizeof (struct FeelinPGroup));

            if (gnode)
            {
                gnode->Object = VGroup, FA_Group_PageTitle, F_CAT(ABOUT),
                    Child, TextObject,
                        FA_Frame, "<frame id='18' padding='10' />",
                        FA_Back, "<image type='gradient' start='#B8F0F8' middle='#000000' end='#002080' />",
                        FA_ChainToCycle, FALSE,
                        FA_Text_VCenter, TRUE,
                        FA_Text,

                        "<align=center><image type='picture' src='feelin:resources/system/logo.png'><br>"
                        "<font face='Helvetica' size='9' color='#1040A0'>Copyright © 2001-2005</font><br>"
                        "<font face='Helvetica' size='15' color='#FFFFFF'><b>Olivier LAVIALE</b></font><br>"
                        "<pens text='#185CB8'>www.gofromiel.com/feelin/</pens><br>"
                        "<br>"
                        "<font face='Helvetica' size='11' color='#FFFF00'>- Contributors -</font>"
                        "<br>"
                        "<font face='Helvetica' size='13' color='#FFFFFF'><b>Guillaume ROGUEZ</b></font><font color='#B8F0F8'> : MorphOS port</font><br>"
                        "<font face='Helvetica' size='13' color='#FFFFFF'><b>AROS Team</b></font><font color='#B8F0F8'> : AROS port</font><br>"
                        "<br>"
                        "<font face='Helvetica' size='9' color='#1040A0'><pens text='shadow'>(built 2005/08/20)</pens></font><br>",
                    End,
                End;

                if (gnode->Object)
                {
                   gnode->Name = (STRPTR) F_Get(gnode->Object,FA_Group_PageTitle);

                   F_LinkHead(&LOD->GroupList,(FNode *) gnode);
                }
                else
                {
                   F_Dispose(gnode);
                }
            }

            for (gnode = (struct FeelinPGroup *) LOD->GroupList.Head ; gnode ; gnode = gnode->Next)
            {
                gnode->Position = i++;

                F_Do(LOD->list,id_InsertSingle,gnode,FV_List_Insert_Bottom);
            }

            /*** setup notifications ***/
            
            F_Do(Obj,FM_Notify,"FA_Dialog_Response",FV_Dialog_Response_Save,  Obj,F_IDM(FM_PreferenceEditor_Save),0);
            F_Do(Obj,FM_Notify,"FA_Dialog_Response",FV_Dialog_Response_Use,   Obj,F_IDM(FM_PreferenceEditor_Use),0);
            F_Do(Obj,FM_Notify,"FA_Dialog_Response",FV_Dialog_Response_Apply, Obj,F_IDM(FM_PreferenceEditor_Test),0);
            F_Do(Obj,FM_Notify,"FA_Dialog_Response",FV_Dialog_Response_Cancel,Obj,F_IDM(FM_PreferenceEditor_Cancel),0);
                                                        
            F_Set(LOD->list, (uint32) "FA_List_Active", 0);
 
            F_Do(LOD->list,FM_Notify,"FA_List_Active",FV_Notify_Always,Obj,F_IDM(FM_PreferenceEditor_Change),1,FV_Notify_Value);

            F_Do(Obj,F_IDM(FM_PreferenceEditor_Change),0);

            if (F_Do((FObject) F_Get(Obj,FA_Child),FM_AddMember,root,FV_AddMember_Head))
            {
               return Obj;
            }
        }
    }
    return NULL;
}
//+
///PE_Dispose
F_METHOD(void,PE_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct FeelinPGroup *node;

    while ((node = (struct FeelinPGroup *) LOD->GroupList.Head))
    {
 //      F_Log(0,"NODE 0x%08lx - Object 0x%08lx (%s)",node,node->Object,node->Name);

        F_LinkRemove(&LOD->GroupList,(FNode *) node);
        F_DisposeObj(node->Object);
        F_Dispose(node);
    }

    F_Dispose(LOD->Title); LOD->Title = NULL;

    F_SharedClose(LOD->MList);

    F_SUPERDO();
}
//+
///PE_Connect
F_METHODM(ULONG,PE_Connect,FS_Connect)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   LOD->Application = Msg->Parent;
   LOD->Preferences = (FObject) F_Get(LOD->Application,FA_Application_Preference);

//   F_Log(0,"app %s{%08lx} - prefs %s{%08lx}",_classname(LOD->Application),LOD->Application,_classname(LOD->Preferences),LOD->Preferences);

   return F_SUPERDO();
}
//+
///PE_Disconnect
F_METHOD(ULONG,PE_Disconnect)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   LOD->Application = NULL;
   LOD->Preferences = NULL;

   return F_SUPERDO();
}
//+

///PE_Setup

/* if application's base is NULL global preferences are  edited.  Otherwise
if application has a title it will be displayed in the editor's window bar.
If the application's title is NULL the base is used instead. */

F_METHOD(ULONG,PE_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD->Application)
   {
      STRPTR base,title,old = LOD->Title;

      F_Do(LOD->Application,FM_Get,
         FA_Application_Title,      &title,
         FA_Application_Base,       &base,
         TAG_DONE);

//      F_Log(0,"App 0x%08lx ('%s','%s') - Prefs 0x%08lx",LOD->Application,title,base,LOD->Preferences);

      if (!base)
      {
         LOD->Title = F_StrNew(NULL,"%s",F_CAT(TITLE_DEFAULT));
      }
      else
      {
         LOD->Title = F_StrNew(NULL,F_CAT(TITLE),(title) ? title : base);
      }

      F_Do(Obj,FM_Set,
         FA_Window_Title,        LOD->Title,
         FA_Window_ScreenTitle,  F_CAT(TITLE_SCREEN),
         TAG_DONE);

      F_Dispose(old);

      F_Do(Obj,F_IDM(FM_PreferenceEditor_Load));
   }

   return F_SUPERDO();
}
//+
///PE_Change
F_METHODM(void,PE_Change,FS_PreferenceEditor_Change)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct FeelinPGroup *node;

    node = (struct FeelinPGroup *) F_Do(LOD->list,F_IDO(FM_List_GetEntry),Msg->Which);

    if (node &&
        node->Object != LOD->active_page)
    {
        //F_Log(0,"MASTER %s{%08lx} REMMEMBER %s{%08lx}",_classname(LOD->Master), LOD->Master, (LOD->active_page) ? _classname(LOD->active_page) : "", LOD->active_page);
        
        F_Do(LOD->Master, FM_Group_InitChange);
        F_Do(LOD->Master, FM_RemMember, LOD->active_page);
        
        LOD->active_page = node->Object;
        
        //F_Log(0,"MASTER %s{%08lx} ADDMEMBER %s{%08lx}",_classname(LOD->Master), LOD->Master, (LOD->active_page) ? _classname(LOD->active_page) : "", LOD->active_page);
        
        F_Do(LOD->Master, FM_AddMember, LOD->active_page, FV_AddMember_Tail);
        F_Do(LOD->Master, FM_Group_ExitChange);
        
        if (Msg->Which == 0)
        {
            F_Set(LOD->GroupTitle, FA_Hidden, FF_Hidden_Check | TRUE);
        }
        else
        {
            F_Do(LOD->GroupTitle, FM_Set,
 
                FA_Hidden, FF_Hidden_Check | FALSE,
                FA_Text, F_Get(node->Object, FA_Group_PageTitle),
                
                TAG_DONE);
        }
    }
}
//+

