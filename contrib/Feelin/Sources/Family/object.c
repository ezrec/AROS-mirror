#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///family_search
STATIC FFamilyNode * family_search(FClass *Class,FObject Obj,FObject Member,struct FeelinBase *FeelinBase)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   FFamilyNode *node;

   for (node = (FFamilyNode *)(LOD->list.Head) ; node ; node = node -> Next)
   {
      if (node -> Object == Member) break;
   }
   return node;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Family_New
F_METHOD(uint32,Family_New)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Child:
      {
         FFamilyNode *node;

         if (!F_Do((LOD->owner) ? LOD->owner : Obj,FM_AddMember,item.ti_Data,FV_AddMember_Tail))
         {
            struct TagItem *real;

            if (item.ti_Data)
            {
               if (LOD->owner)
               {
                  F_Log(FV_LOG_DEV,"FM_AddMember() failed - Child %s{%08lx} - Owner: %s{%08lx}",_classname(item.ti_Data),item.ti_Data,_classname(LOD->owner),LOD->owner);
               }
               else
               {
                  F_Log(FV_LOG_DEV,"FM_AddMember() failed - Child %s{%08lx}",_classname(item.ti_Data),item.ti_Data);
               }
            }
            else
            {
               if (LOD->owner)
               {
                  F_Log(FV_LOG_USER,"NULL object(s) - Owner: %s{%08lx}",_classname(LOD->owner),LOD->owner);
               }
               else
               {
                  F_Log(FV_LOG_USER,"NULL object(s)");
               }
            }

            /* one child in not valid (== NULL),  we  must  remove  objects
            already member of the family, then dispose all valid objects in
            the taglist and return NULL */

            while ((node = (FFamilyNode *)(LOD->list.Tail)) != NULL)
            {
               F_Do((LOD->owner) ? LOD->owner : Obj,FM_RemMember,node -> Object);

               if (node == (FFamilyNode *)(LOD->list.Tail))
               {
                  F_Log(FV_LOG_DEV,"!! LINK ERROR !!"); return NULL;
               }
            }

            Tags = Msg;

            while ((real = F_DynamicNTI(&Tags,&item,Class)) != NULL)
            {
               if ((item.ti_Tag == FA_Child) && (item.ti_Data))
               {
                  F_DisposeObj((FObject)(item.ti_Data)); real -> ti_Tag = TAG_IGNORE; real -> ti_Data = NULL;
               }
            }

            F_Set(LOD->owner,FA_Family,NULL);
            LOD->owner = NULL;

            return NULL;
         }
      }
      break;

      case FA_Family_Owner:
      {
         LOD->owner = (FObject)(item.ti_Data);

         F_Set(LOD->owner,FA_Family,(ULONG)(Obj));
      }
      break;

      case FA_Family_CreateNodeHook:
      {
         LOD->create_node_hook = (struct Hook *)(item.ti_Data);
      }
      break;

      case FA_Family_DeleteNodeHook:
      {
         LOD->delete_node_hook = (struct Hook *)(item.ti_Data);
      }
      break;
   }
   
   return F_SUPERDO();
}
//+
///Family_Dispose
F_METHOD(void,Family_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FFamilyNode *node;

//    F_Log(0,"BEGIN");

    while ((node = (FFamilyNode *) LOD->list.Tail) != NULL)
    {
        /* remember that FM_RemMember disposes the node */

//        FObject owner = LOD->owner ? LOD->owner : Obj;
        FObject object = node -> Object;

//        F_Log(0,"REMMEMBER owner %s{%08lx} object %s{%08lx} >> BEGIN",_classname(owner),owner,_classname(object),object);

        F_Do((LOD->owner) ? LOD->owner : Obj,FM_RemMember,object);
//        F_Log(0,"REMMEMBER owner %s{%08lx} object %s{%08lx} >> DONE",_classname(owner),owner,_classname(object),object);
        
//        F_Log(0,"DISPOSE %s{%08lx} >> BEGIN",_classname(object),object);
        F_DisposeObj(object);
//        F_Log(0,"DISPOSE %s{%08lx} >> DONE",_classname(object),object);
    }

    LOD->owner = NULL;

//    F_Log(0,"SUPERDO >> BEGIN");
    F_SUPERDO();
//    F_Log(0,"SUPERDO >> DONE");
    
//    F_Log(0,"DONE");
}
//+
///Family_Get
F_METHOD(void,Family_Get)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   struct TagItem         *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Family:      F_STORE(Obj);              break;
      case FA_Family_Head: F_STORE(LOD->list.Head); break;
      case FA_Family_Tail: F_STORE(LOD->list.Tail); break;
   }

   F_SUPERDO();
}
//+

///Family_Add
F_METHODM(FFamilyNode *,Family_Add,FS_AddMember)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (Msg -> Object)
   {
      if (family_search(Class,Obj,Msg -> Object,FeelinBase))
      {
         if (LOD->owner)
         {
            F_Log(FV_LOG_DEV,"%s{%08lx} already member of %s{%08lx}",_classname(Msg -> Object),Msg -> Object,_classname(LOD->owner),LOD->owner);
         }
         else
         {
            F_Log(FV_LOG_DEV,"%s{%08lx} already member",_classname(Msg -> Object),Msg -> Object);
         }
      }
      else
      {
         /* The object  is  added  to  the  family  *only*  if  the  method
         FM_Connect returns TRUE. */

         if (F_Do(Msg -> Object,FM_Connect,((LOD->owner) ? LOD->owner : Obj),Obj))
         {
            FFamilyNode *node;

            if (LOD->create_node_hook)
            {
               node = (FFamilyNode *) CallHookPkt(LOD->create_node_hook,((LOD->owner) ? LOD->owner : Obj),Msg);
            }
            else
            {
               node = F_NewP(CUD -> NodePool,sizeof (FFamilyNode));
            }

            if (node)
            {
               node -> Next = NULL;
               node -> Prev = NULL;
               node -> Object = Msg -> Object;

               switch (Msg -> Position)
               {
                  case FV_AddMember_Tail:
                  {
                     F_LinkTail(&LOD->list,(FNode *) node);
///db_AddMember
                     if (CUD -> db_AddMember)
                     {
                        F_Log(0,"(TAIL) Node 0x%08lx - %s{%08lx}",node,_classname(node -> Object),node -> Object);
                     }
//+
                     return node;
                  }
                  break;

                  case FV_AddMember_Head:
                  {
                     F_LinkHead(&LOD->list,(FNode *) node);
///db_AddMember
                     if (CUD -> db_AddMember)
                     {
                        F_Log(0,"(HEAD) Node 0x%08lx - %s{%08lx}",node,_classname(node -> Object),node -> Object);
                     }
//+
                     return node;
                  }
                  break;

                  case FV_AddMember_Insert:
                  {
                     FFamilyNode *prev;

                     if (!Msg -> Previous)
                     {
                        F_LinkHead(&LOD->list,(FNode *) node);
///db_AddMember
                     if (CUD -> db_AddMember)
                     {
                        F_Log(0,"(HEAD) Node 0x%08lx - %s{%08lx}",node,_classname(node -> Object),node -> Object);
                     }
//+
                        return node;
                     }
                     if ((prev = family_search(Class,Obj,Msg -> Previous,FeelinBase)) != NULL)
                     {
                        F_LinkInsert(&LOD->list,(FNode *) node,(FNode *) prev);
///db_AddMember
                        if (CUD -> db_AddMember)
                        {
                           F_Log(0,"(INSR) Node 0x%08lx - %s{%08lx}",node,_classname(node -> Object),node -> Object);
                        }
//+
                        return node;
                     }
                     else
                     {
                        F_Log(FV_LOG_DEV,"%s{%08lx} (Previous) is not member of the family",_classname(Msg -> Previous),Msg -> Previous);
                     }
                  }
                  break;

                  default:
                  {
                     if (LOD->owner)
                     {
                        F_Log(FV_LOG_DEV,"%ld is not a valid position - Object %s{%08lx} - Owner %s{%08lx}",Msg -> Position,_classname(Msg -> Object),Msg -> Object,_classname(LOD->owner),LOD->owner);
                     }
                     else
                     {
                        F_Log(FV_LOG_DEV,"%ld is not a valid position - Object %s{%08lx}",Msg -> Position,_classname(Msg -> Object),Msg -> Object);
                     }
                  }
               }

/*** error *****************************************************************/

               F_Dispose(node);
            }

            F_Do(Msg -> Object,FM_Disconnect);
         }
         else
         {
            if (LOD->owner)
            {
               F_Log(FV_LOG_DEV,"Unable to connect %s{%08lx} to %s{%08lx}",_classname(Msg -> Object),Msg -> Object,_classname(LOD->owner),LOD->owner);
            }
            else
            {
               F_Log(FV_LOG_DEV,"Unable to connect %s{%08lx}",_classname(Msg -> Object),Msg -> Object);
            }
         }
      }
   }
   return NULL;
}
//+
///Family_Rem
F_METHODM(uint32,Family_Rem,FS_RemMember)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

///db_RemMember
    if (CUD -> db_RemMember)
    {
        if (LOD->owner)
        {
            F_Log(FV_LOG_DEV,"Member: %s{%08lx} - Owner: %s{%08lx}",_classname(Msg -> Member),Msg -> Member,_classname(LOD->owner),LOD->owner);
        }
        else
        {
            F_Log(FV_LOG_DEV,"Member: %s{%08lx}",_classname(Msg -> Member),Msg -> Member);
        }
    }
//+

    if (Msg -> Member)
    {
        FFamilyNode *node = family_search(Class,Obj,Msg -> Member,FeelinBase);

//        F_Log(0,"%s{%08lx}",_classname(Msg -> Member),Msg -> Member);

        if (node)
        {
//            F_Log(0,"Node 0x%08lx - %s{%08lx} - DISCONNECT",node,_classname(Msg -> Member),Msg -> Member);
            F_Do(node -> Object,FM_Disconnect);
//            F_Log(0,"Link remove");
            F_LinkRemove(&LOD->list,(FNode *) node);

//            F_Log(0,"dispose node - hook 0x%08lx - node 0x%08lx",LOD->delete_node_hook, node);
 
            if (LOD->delete_node_hook)
            {
                CallHookPkt(LOD->delete_node_hook,((LOD->owner) ? LOD->owner : Obj),&node);
            }
            else
            {
                F_Dispose(node);
            }

//            F_Log(0,"DONE");
            
            return TRUE;
        }
        else
        {
            if (LOD->owner)
            {
                F_Log(FV_LOG_DEV,"%s{%08lx} is not a member. Owner: %s{%08lx}",_classname(Msg -> Member),Msg -> Member,_classname(LOD->owner),LOD->owner);
            }
            else
            {
                F_Log(FV_LOG_DEV,"%s{%08lx} is not a member.",_classname(Msg -> Member),Msg -> Member);
            }
        }
    }
    return FALSE;
}
//+

