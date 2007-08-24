/*

$VER: 05.00 (2005/08/10)
 
   Portability update.
   
   Metaclass support.

$VER: 04.00 (2005/05/03)
 
   Preference items have been totaly changed. They are no longer defined as
   "FP_Button_Frame"  but more CSS like e.g. "$frame-button". Note well the
   dollar sign, which is used instead of "FP_" to recognize the string as a
   preference  item,  needed  to  recognize preference item from developers
   overrides.
 
$VER: 03.00 (2004/12/18)

   The class does no longer manage a global preference object.  Instead,  a
   reference  object  can  be defined for each object. The AppServer shared
   object holds a FC_Preference  object  used  by  each  application  as  a
   reference for their own FC_Preference object.

   The class does no longer update applications, which  was  a  very  dirty
   job.  Instead,  the FA_Preference_Update is set to TRUE when preferences
   have  been  modified.  Setting  a  notification   on   this   attribute,
   Applications  can  react on preferences modifications like they want to.
   The   attribute   is   NOT   set   by   the   FM_Preference_Read,    nor
   FM_Preference_AddXxx.  The attribute is set to TRUE only on notification
   of file modification. The attribute is also used by  FC_PreferenceEditor
   objects.

   The FA_Preference_Application attribute as been removed  because  it  is
   now useless.

   When writting preference file, items with the  same  contents  as  their
   reference are removed.

*/

#include "Project.h"

struct Library                     *IFFParseBase;
struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,Prefs_New);
F_METHOD_PROTO(void,Prefs_Dispose);
F_METHOD_PROTO(void,Prefs_Get);
F_METHOD_PROTO(void,Prefs_Set);

F_METHOD_PROTO(void,Prefs_Find);
F_METHOD_PROTO(void,Prefs_Add);
F_METHOD_PROTO(void,Prefs_AddLong);
F_METHOD_PROTO(void,Prefs_AddString);
F_METHOD_PROTO(void,Prefs_Remove);
F_METHOD_PROTO(void,Prefs_Clear);
F_METHOD_PROTO(void,Prefs_Read);
F_METHOD_PROTO(void,Prefs_Write);
F_METHOD_PROTO(void,Prefs_Resolve);
F_METHOD_PROTO(void,Prefs_ResolveInt);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
    CUD = F_LOD(Class,Obj);
    
    if (F_SUPERDO())
    {
        CUD -> db_Notify = DOSNotifyObject,"FA_DOSNotify_Name",DEBUGVARNAME,End;

        F_Do(Obj,F_IDM(FM_Meta_Debug));
     
        F_Do(CUD -> db_Notify,FM_Notify,"FA_DOSNotify_Update",FV_Notify_Always,
            Obj,F_IDM(FM_Meta_Debug),0);

        if ((CUD -> ListSemaphore = F_NewObj(FC_Object,/*FA_Semaphore_Name,"PreferenceList",*/TAG_DONE)) != NULL)
        {
            if ((IFFParseBase = OpenLibrary("iffparse.library",36)))
            {
                return Obj;
            }
        }
    }
    return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);
 
   F_DisposeObj(CUD -> db_Notify);

   if (IFFParseBase)
   {
      CloseLibrary(IFFParseBase); IFFParseBase = NULL;
   }

   if (CUD -> ListSemaphore)
   {
      F_DisposeObj(CUD -> ListSemaphore); CUD -> ListSemaphore = NULL;
   }

   F_SUPERDO();
}
//+
///Class_Debug
F_METHOD(void,Class_Debug)
{
   STRPTR buf = F_New(1024);

   CUD = F_LOD(Class,Obj);

   CUD -> db_Read    = FALSE;
   CUD -> db_Write   = FALSE;
   CUD -> db_Resolve = FALSE;

   if (buf)
   {
      BPTR file = Open(DEBUGVARNAME,MODE_OLDFILE);

      if (file)
      {
         if (FGets(file,buf,1024))
         {
            struct RDArgs *rda = (struct RDArgs *) AllocDosObject(DOS_RDARGS,NULL);

            if (rda)
            {
               rda -> RDA_Source.CS_Buffer = buf;
               rda -> RDA_Source.CS_Length = F_StrLen(buf);

               if (ReadArgs(TEMPLATE,&CUD -> db_Array,rda))
               {
                  FreeArgs(rda);
               }
               FreeDosObject(DOS_RDARGS,rda);
            }
         }
         Close(file);
      }
      F_Dispose(buf);
   }
}
//+

F_QUERY()
{
    switch (Which)
    {
///Meta
        case FV_Query_MetaClassTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD        (Class_Debug,   "Private1"),
                F_METHODS_ADD_STATIC (Class_New,      FM_New),
                F_METHODS_ADD_STATIC (Class_Dispose,  FM_Dispose),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Class),
                F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Name", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Reference", FV_TYPE_OBJECT),
                F_ATTRIBUTES_ADD("Update", FV_TYPE_BOOLEAN),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(Prefs_Find,         "Find"),
                F_METHODS_ADD(Prefs_Add,          "Add"),
                F_METHODS_ADD(Prefs_AddLong,      "AddLong"),
                F_METHODS_ADD(Prefs_AddString,    "AddString"),
                F_METHODS_ADD(Prefs_Remove,       "Remove"),
                F_METHODS_ADD(Prefs_Clear,        "Clear"),
                F_METHODS_ADD(Prefs_Resolve,      "Resolve"),
                F_METHODS_ADD(Prefs_ResolveInt,   "ResolveInt"),
                F_METHODS_ADD(Prefs_Read,         "Read"),
                F_METHODS_ADD(Prefs_Write,        "Write"),

                F_METHODS_ADD_STATIC(Prefs_New,           FM_New),
                F_METHODS_ADD_STATIC(Prefs_Dispose,       FM_Dispose),
                F_METHODS_ADD_STATIC(Prefs_Get,           FM_Get),
                F_METHODS_ADD_STATIC(Prefs_Set,           FM_Set),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Object),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}
