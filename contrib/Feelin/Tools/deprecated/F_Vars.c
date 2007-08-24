;/*
   F_Create.rexx EXE F_Vars
   QUIT
   _________________________________________________________________________

   Written by Olivier LAVIALE <gofromiel@gofromiel.com>

*/

#include <dos/dos.h>
#include <dos/var.h>
#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelin.h>

struct FeelinBase                  *FeelinBase;

///Text_
FObject Text_(STRPTR Text)
{
   return   TextObject,
            FA_ChainToCycle,  FALSE,
            FA_Text,          Text,
            FA_Text_PreParse, "<align=justify><pens style=shadow>",
            FA_Text_Shortcut, FALSE,
            FA_Text_VCenter,  TRUE,
            End;
}
//+
///CheckBox_
FObject CheckBox_(LONG Selected)
{
   return   ImageObject,
            FA_SetMin,           TRUE,
            FA_SetMax,           TRUE,
            FA_Selected,         Selected,
            FA_InputMode,        FV_InputMode_Toggle,
           "FA_Image_Spec",     "FP_CheckMark_Image",
            FA_Back,            "FP_CheckMark_Back",
            FA_Frame,           "FP_CheckMark_Frame",
            End;
}
//+

typedef struct FeelinItem
{
   STRPTR                           Name;
   FObject                          Object;
}                                                           FItem;

typedef struct FeelinVariable
{
   STRPTR                           Name;
   STRPTR                           Template;
   LONG                            *ReadArray;
   FItem                           *Items;
}                                                           FVar;

/***************************************************************************/
/*** Items Arrays **********************************************************/
/***************************************************************************/

static FItem items_application[] =
{
   "PUSH",     NULL,
   "WINDOW",   NULL,

    NULL
};

static LONG ra_application[] = { FALSE, FALSE, 0 };

enum  {

      ITEM_APPLICATION_PUSH,
      ITEM_APPLICATION_WINDOW

      };

#define TEMPLATE_APPLICATION                    "PUSH/S,WINDOW/S"

/*** Family ****************************************************************/

static FItem items_family[] =
{
   "ADD",   NULL,
   "REM",   NULL,

    NULL
};

static LONG ra_family[] = { FALSE, FALSE, 0 };

enum  {

      ITEM_FAMILY_ADD,
      ITEM_FAMILY_REM

      };

#define TEMPLATE_FAMILY                         "ADD/S,REM=REMOVE/S"

/*** Preference ************************************************************/

static FItem items_preference[] =
{
   "READ",     NULL,
   "WRITE",    NULL,
   "RESOLVE",  NULL,

    NULL
};

static LONG ra_preference[] = { FALSE, FALSE, FALSE, 0 };

enum  {

      ITEM_PREFERENCE_READ,
      ITEM_PREFERENCE_WRITE,
      ITEM_PREFERENCE_RESOLVE

      };

#define TEMPLATE_PREFERENCE                     "READ/S,WRITE/S,RESOLVE/S"

/***************************************************************************/
/*** Vars Arrays ***********************************************************/
/***************************************************************************/

static FVar Variables[] =
{
   "DBG_APPLICATION", TEMPLATE_APPLICATION,  ra_application,   items_application,
   "DBG_FAMILY",      TEMPLATE_FAMILY,       ra_family,        items_family,
   "DBG_PREFERENCE",  TEMPLATE_PREFERENCE,   ra_preference,    items_preference,

    NULL
};

enum  {

      VAR_APPLICATION,
      VAR_FAMILY,
      VAR_PREFERENCE

      };

/*** Class *****************************************************************/

#define FM_VARS_READ                            (FCCA_BASE + 0)
#define FM_VARS_SAVE                            (FCCA_BASE + 1)
#define FM_VARS_USE                             (FCCA_BASE + 2)
#define FM_VARS_TEST                            (FCCA_BASE + 3)
#define FM_VARS_CANCEL                          (FCCA_BASE + 4)

struct LocalObjectData
{
   FAreaData                       *AreaData;
};

///vars_write
void vars_write(ULONG Destination)
{
   STRPTR name_buf;

   if (name_buf = F_New(1024))
   {
      STRPTR var_buf;

      if (var_buf = F_New(1024))
      {
         FVar *var;

         for (var = Variables ; var -> Name ; var++)
         {
            FItem *item;
            STRPTR cur = var_buf;

            CopyMem("Feelin",name_buf,7);
            AddPart(name_buf,var -> Name,1024);

            for (item = var -> Items ; item -> Name ; item++)
            {
//               F_Log(0,"%s.%s %ld",var -> Name,item -> Name,F_Get(item -> Object,FA_Selected));

               if (F_Get(item -> Object,FA_Selected))
               {
                  ULONG len = F_StrLen(item -> Name);

                  CopyMem(item -> Name,cur,len);

                  cur += len; *cur++ = ' ';
               }
            }

            *cur++ = '\n'; *cur+= '\0';

//            F_Log(0,"SAVE '%s' = '%s' (%ld)",name_buf,var_buf,cur - var_buf);

            SetVar(name_buf,var_buf,cur - var_buf,Destination);
         }
         F_Dispose(var_buf);
      }
      F_Dispose(name_buf);
   }
}
//+

///vars_New
F_METHOD(FObject,vars_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FObject b_save,b_use,b_test,b_cancel;

   LOD -> AreaData = (FAreaData *) F_Get(Obj,FA_AreaData);

   if (F_SuperDo(Class,Obj,Method,

      FA_ChainToCycle,  FALSE,

      Child, Page,
         Child, VGroup, FA_Group_PageTitle, "About",
            Child, TextObject,
               FA_SetMax,           FV_SetBoth,
               FA_ChainToCycle,     FALSE,
               FA_Text_Shortcut,    FALSE,
               FA_Text,
               "<align=justify><pens style=shadow>" \
//             "                                         "

               "Most classes can report a lot of useful<br>" \
               "information. e.g. FC_Family can report<br>" \
               "every object added or removed. These<br>" \
               "informations can be quite useful for a<br>" \
               "<align=left>clever developper.<br>" \
               "<br>"\
               "<align=justify>System variables are used to toggle reports.<br>" \
               "This tool has been created to easily<br>"\
               "<align=left>manipulate them.<br>" \
               "<br>" \
               "<align=center><i>This tool is currently for debugging purpose</i>",
            End,
         End,

         Child, ColGroup(2), FA_Group_PageTitle, "Application", FA_SetMax, FV_SetWidth,
            Child, items_application[ITEM_APPLICATION_PUSH].Object = CheckBox_(FALSE),
            Child, Text_("Report every method removed from the<br>" \
                         "application's queue and invoked"),

            Child, items_application[ITEM_APPLICATION_WINDOW].Object = CheckBox_(FALSE),
            Child, Text_("Report every window event"),
         End,

         Child, ColGroup(2), FA_Group_PageTitle,"Family", FA_SetMax, FV_SetWidth,
            Child, items_family[ITEM_FAMILY_ADD].Object = CheckBox_(FALSE),
            Child, Text_("Report every object added to a family"),

            Child, items_family[ITEM_FAMILY_REM].Object = CheckBox_(FALSE),
            Child, Text_("Report every object removed from a family"),
         End,

         Child, ColGroup(2), FA_Group_PageTitle,"Preference", FA_SetMax,FV_SetWidth,
            Child, items_preference[ITEM_PREFERENCE_READ].Object = CheckBox_(FALSE),
            Child, Text_("Report items read from a file"),

            Child, items_preference[ITEM_PREFERENCE_WRITE].Object = CheckBox_(FALSE),
            Child, Text_("Report items written to a file"),

            Child, items_preference[ITEM_PREFERENCE_RESOLVE].Object = CheckBox_(FALSE),
            Child, Text_("Report items not found when resolving"),
         End,
      End,

      Child, Bar,

      Child, HGroup, FA_SetMax,FV_SetHeight,
         Child, b_save   = SimpleButton("Save"),
         Child, b_use    = SimpleButton("Use"),
         Child, b_test   = SimpleButton("Test"),
         Child, Bar,
         Child, b_cancel = SimpleButton("Cancel"),
      End,

      TAG_MORE, Msg))
   {
      F_Do(b_save,   FM_Notify,FA_Pressed,FALSE,FV_Notify_Application,FM_Application_PushMethod,3,Obj,FM_VARS_SAVE,  0);
      F_Do(b_use,    FM_Notify,FA_Pressed,FALSE,FV_Notify_Application,FM_Application_PushMethod,3,Obj,FM_VARS_USE,   0);
      F_Do(b_test,   FM_Notify,FA_Pressed,FALSE,FV_Notify_Application,FM_Application_PushMethod,3,Obj,FM_VARS_TEST,  0);
      F_Do(b_cancel, FM_Notify,FA_Pressed,FALSE,FV_Notify_Application,FM_Application_PushMethod,3,Obj,FM_VARS_CANCEL,0);

      F_Do(Obj,FM_VARS_READ);

      return Obj;
   }
   return NULL;
}
//+
///vars_Read
F_METHOD(void,vars_Read)
{
   STRPTR name_buf;

   if (name_buf = F_New(1024))
   {
      STRPTR var_buf;

      if (var_buf = F_New(1024))
      {
         FVar *var;

         for (var = Variables ; var -> Name ; var++)
         {
            LONG len;

            CopyMem("Feelin",name_buf,7);
            AddPart(name_buf,var -> Name,1024);

            len = GetVar(name_buf,var_buf,1024,0);

//            F_Log(0,"\n   FILE '%s' (%ld)\n   '%s'",name_buf,len,var_buf);

            if (len > 0)
            {
               struct RDArgs *rda;

               if (rda = (struct RDArgs *) AllocDosObject(DOS_RDARGS,NULL))
               {
                  var_buf[len] = '\n'; var_buf[len + 1] = '\0';

                  rda -> RDA_Source.CS_Buffer = var_buf;
                  rda -> RDA_Source.CS_Length = F_StrLen(var_buf);

//                  F_Log(0,"CS_Buffer '%s' - CS_Length %ld",rda -> RDA_Source.CS_Buffer,rda -> RDA_Source.CS_Length);

                  if (ReadArgs(var -> Template,var -> ReadArray,rda))
                  {
                     FItem *item;
                     ULONG i = 0;

//                     F_Log(0,"ITEMS: 0x%08lx",var -> Items);

                     for (item = var -> Items ; item -> Name ; item++, i++)
                     {
//                        F_Log(0,"Item '%s' - Value '%ld'",item -> Name,var -> ReadArray[i]);

                        F_Do(item -> Object,FM_Set,
                                            FA_NoNotify,  TRUE,
                                            FA_Selected,  var -> ReadArray[i],
                                            TAG_DONE);
                     }

                     FreeArgs(rda);
                  }
                  else
                  {
                     F_Log(0,"ReadArgs() failed. Template: '%s'",var -> Template);
                  }

                  FreeDosObject(DOS_RDARGS,rda);
               }
            }
         }

         F_Dispose(var_buf);
      }

      F_Dispose(name_buf);
   }
}
//+
///vars_Save
F_METHOD(void,vars_Save)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   vars_write(GVF_GLOBAL_ONLY | GVF_SAVE_VAR);

   if (_render)
   {
      F_Set(_win,FA_Window_CloseRequest,TRUE);
   }
}
//+
///vars_Use
F_METHOD(void,vars_Use)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   vars_write(GVF_GLOBAL_ONLY);

   if (_render)
   {
      F_Set(_win,FA_Window_CloseRequest,TRUE);
   }
}
//+
///vars_Test
F_METHOD(void,vars_Test)
{
   vars_write(GVF_GLOBAL_ONLY);
}
//+

///Main
void main(void)
{
   if (FeelinBase = (struct FeelinBase *) OpenLibrary("feelin.library",FV_VERSION))
   {
      struct FeelinClass *cc;

      static FClassMethod Methods[] =
      {
         (FMethod) vars_New,  NULL, FM_New,
         (FMethod) vars_Read, NULL, FM_VARS_READ,
         (FMethod) vars_Save, NULL, FM_VARS_SAVE,
         (FMethod) vars_Use,  NULL, FM_VARS_USE,
         (FMethod) vars_Test, NULL, FM_VARS_TEST,
          NULL
      };

      if (cc = F_CreateClass(NULL,
                             FA_Class_LODSize,        sizeof (struct LocalObjectData),
                             FA_Class_Super,          FC_Group,
                             FA_Class_Methods,        Methods,
                             FA_Class_Pool,           FALSE,
                             TAG_DONE))
      {
         FObject app,win;

         app = AppObject,
            FA_Application_Title,       "F_Variables",
            FA_Application_Version,     "$VER: F_Variables 1.00 (2004/08/22)",
            FA_Application_Copyright,   "©2001-2004, Olivier LAVIALE",
            FA_Application_Author,      "Olivier LAVIALE (goformiel@numericable.fr)",
            FA_Application_Description, "Edit classes' variables",
            FA_Application_Base,        "FVARS",

            Child, win = WindowObject,
               FA_Window_Title, "Feelin : Variables",
               FA_Window_Open,   TRUE,

               Child, F_NewObj(cc -> Name,TAG_DONE),
            End,
         End;

         if (app)
         {
            F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE,app,FM_Application_Shutdown,0);

            F_Do(app,FM_Application_Run);

            F_DisposeObj(app);
         }

         F_DeleteClass(cc);
      }

      CloseLibrary(FeelinBase);
   }
   else Printf("Unable to open feelin.library v%ld\n",FV_VERSION);
}
//+
