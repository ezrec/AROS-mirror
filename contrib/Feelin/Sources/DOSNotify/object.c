#include "Private.h"

///DN_New
F_METHOD(uint32,DN_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
       case FA_DOSNotify_Name:
       {
          LOD -> Notify.nr_Name = (STRPTR)(item.ti_Data);
       }
       break;
    }
    
    if (LOD -> Notify.nr_Name)
    {
       if (F_Do(CUD -> Thread,CUD -> id_Send,FV_Thread_AddNotify,&LOD -> Notify,Obj))
       {
          return F_SUPERDO();
       }
       else
       {
          F_Log(0,"Unable to create DOS notification '%s'",LOD -> Notify.nr_Name);
       }
    }

    return NULL;
}
//+
///DN_Dispose
F_METHOD(void,DN_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

/*GOFROMIEL: Tu avais oublié  '&LOD  ->  Notify'  ce  qui  est  quand  même
important :-) */
   
    F_Do(CUD -> Thread,CUD -> id_Send,FV_Thread_RemNotify,&LOD -> Notify);

    F_SUPERDO();
}
//+
