/*

$VER: DOSNotify 04.00 (2005/08/10)
 
   Portability update.
   
   Metaclass support.

$VER: DOSNotify 03.00 (2004/12/18)

   The class had been rewritten. The class do not calls hooks  and  methods
   itself,  instead the attribute FA_DOSNotify_Update is set to the name of
   the modified watched file. Thus, the  user  of  the  object  can  set  a
   notification of its choice on the attribute.

*/

#include "Project.h"

struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,DN_New);
F_METHOD_PROTO(void,DN_Dispose);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
    CUD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        CUD -> Thread = ThreadObject,

           "FA_Thread_Entry", F_FUNCTION_GATE(Thread_Main),
           "FA_Thread_Name", "dosnotify.thread",
           
           End;
           
        if (CUD -> Thread)
        {
            CUD -> id_Send = F_DynamicFindID("FM_Thread_Send");
            CUD -> id_Update = F_DynamicFindID("FA_DOSNotify_Update");

            return Obj;
        }
    }
    return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
    CUD = F_LOD(Class,Obj);
  
    F_DisposeObj(CUD -> Thread); CUD -> Thread = NULL;
    
    F_SUPERDO();
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
                F_METHODS_ADD_STATIC(Class_New,      FM_New),
                F_METHODS_ADD_STATIC(Class_Dispose,  FM_Dispose),

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
                F_ATTRIBUTES_ADD("Update", FV_TYPE_BOOLEAN),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(DN_New, FM_New),
                F_METHODS_ADD_STATIC(DN_Dispose, FM_Dispose),

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

