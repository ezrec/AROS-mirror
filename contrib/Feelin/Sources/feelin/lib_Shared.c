/*
**    $VER: lib_Shared.c 02.00 (2005/07/07)
**
**    Shared objects management
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

 * 02.00 ** 2005/07/07 *
 
   Since any object (subclass of FC_Object)  have  semaphore  capabilities,
   FC_Shared  is deprecated, because its only purpose was to offer lockable
   objects.
 
*/

#include "Private.h"

//#define DB_SHARED_FIND
//#define DB_SHARED_CREATE
//#define DB_SHARED_OPEN
//#define DB_SHARED_CLOSE

struct FeelinSharedNode
{
    struct FeelinSharedNode        *Next;
    struct FeelinSharedNode        *Prev;

    STRPTR                          Name;                   // allocated
    uint32                          Refs;
    FObject                         Object;                 // referenced object (shortcut to FC_Shared -> Object)
};

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///f_shared_findnode
STATIC struct FeelinSharedNode * f_shared_findnode(STRPTR Name,struct in_FeelinBase *FeelinBase)
{
   struct FeelinSharedNode *node;

   #ifdef DB_SHARED_FIND
   F_Log(0,"F_SharedFind() Name '%s' - Head 0x%08lx",Name,FeelinBase -> SharedList.Head);
   #endif

   for (node = (struct FeelinSharedNode *)(FeelinBase -> SharedList.Head) ; node ; node = node -> Next)
   {
      int32 cmp = F_StrCmp(Name,node -> Name,ALL);

      #ifdef DB_SHARED_FIND
      F_Log(0,"cmp '%s' ?= '%s' = %ld",Name,node -> Name,cmp);
      #endif

      if (cmp == 0) break;
      else if (cmp < 0) return NULL;
   }

   #ifdef DB_SHARED_FIND
   F_Log(0,"find_node 0x%08lx",node);
   #endif

   return node;
}
//+
///f_shared_createnode
STATIC struct FeelinSharedNode * f_shared_createnode(FObject Object,STRPTR Name,struct in_FeelinBase *FeelinBase)
{
///DB_SHARED_CREATE
#ifdef DB_SHARED_CREATE
   F_Log(0,"F_SharedCreate() - Object 0x%08lx - Name '%s'",Object,Name);
#endif
//+

   if (Name && Object)
   {
///DB_SHARED_CREATE
#ifdef DB_SHARED_CREATE
      F_Log(0,"find name '%s'",Name);
#endif
//+

      if (!F_SharedFind(Name))
      {
         struct FeelinSharedNode *node = F_New(sizeof (struct FeelinSharedNode));

///DB_SHARED_CREATE
#ifdef DB_SHARED_CREATE
         F_Log(0,"allocate node");
#endif
//+

         if (node)
         {
            node -> Object = Object;
            node -> Name = F_StrNew(NULL,"%s",Name);

///DB_SHARED_CREATE
#ifdef DB_SHARED_CREATE
            F_Log(0,"name (%s) >> (%s)",Name,node -> Name);
#endif
//+

            if (node -> Name)
            {
               struct FeelinSharedNode *prev;

               for (prev = (struct FeelinSharedNode *)(FeelinBase -> SharedList.Tail) ; prev ; prev = prev -> Prev)
               {
                  if (F_StrCmp(node -> Name,prev -> Name,ALL) > 0) break;
               }

               F_LinkInsert((FList *) &FeelinBase -> SharedList,(FNode *) node,(FNode *) prev);

///DB_SHARED_CREATE
#ifdef DB_SHARED_CREATE
                  F_Log(0,"node 0x%08lx - prev 0x%08lx (%s)",node,prev,prev -> Name);
#endif
//+

               return node;
            }
            F_Dispose(node);
         }
      }
   }
   return NULL;
}
//+

/****************************************************************************
*** Functions ***************************************************************
****************************************************************************/

///f_shared_find
F_LIB_SHARED_FIND
{
   struct FeelinSharedNode *node;

   #ifdef DB_SHARED_FIND
   F_Log(0,"find: '%s'",Name);
   #endif

   if ((node = f_shared_findnode(Name,FeelinBase)))
   {
//      return node -> Shared;
      return node -> Object;
   }
   return NULL;
}
//+
///f_shared_create
F_LIB_SHARED_CREATE
{
   struct FeelinSharedNode *node;

   #ifdef DB_SHARED_CREATE
   F_Log(0,"F_SharedCreate() - Object 0x%08lx - Name '%s'",Object,Name);
   #endif
   
   ObtainSemaphore(&FeelinBase -> SharedList.Semaphore);

   node = f_shared_createnode(Object,Name,FeelinBase);
   
   ReleaseSemaphore(&FeelinBase -> SharedList.Semaphore);
   
   if (node)
   {
      return node -> Object;
   }
   return NULL;
}
//+
///f_shared_delete
F_LIB_SHARED_DELETE
{
   if (Shared)
   {
      struct FeelinSharedNode *node;

      ObtainSemaphore(&FeelinBase -> SharedList.Semaphore);

      for (node = (struct FeelinSharedNode *)(FeelinBase -> SharedList.Head) ; node ; node = node -> Next)
      {
         if (node -> Object == Shared)
         {
            if (node -> Refs)
            {
               F_Log(FV_LOG_DEV,"Shared %s{%08lx} refs %ld - forced remove !",node -> Name,node -> Object,node -> Refs);
            }

            F_LinkRemove((FList *) &FeelinBase -> SharedList,(FNode *) node);

            F_DisposeObj(node -> Object);
            
            F_Dispose(node -> Name);
            F_Dispose(node);

            break;
         }
      }
   
      if (!node)
      {
         F_Log(FV_LOG_DEV,"Shared %s{%08lx} not found",_classname(Shared),Shared);
      }

      ReleaseSemaphore(&FeelinBase -> SharedList.Semaphore);
   }
}
//+
///f_shared_open
F_LIB_SHARED_OPEN
{
   if (Name)
   {
      struct FeelinSharedNode *node;

      ObtainSemaphore(&FeelinBase -> SharedList.Semaphore);

      node = f_shared_findnode(Name,FeelinBase);
      
      if (!node)
      {
         FObject object = F_NewObj(Name,TAG_DONE);

         if (object)
         {
            #ifdef DB_SHARED_OPEN
            F_Log(0,"F_Shared_Open() - Object %s{%08lx}",_classname(object),object);
            #endif

            node = f_shared_createnode(object,Name,FeelinBase);

            #ifdef DB_SHARED_OPEN
            F_Log(0,"F_Shared_Open() - Node 0x%08lx",node);
            #endif

            if (!node)
            {
               F_DisposeObj(object);
            }
         }
         else
         {
            F_Log(FV_LOG_DEV,"F_Shared_Open() - Unable to create object from '%s'",Name);
         }
      }

      if (node)
      {
         node -> Refs++;
      }

      ReleaseSemaphore(&FeelinBase -> SharedList.Semaphore);

      if (node)
      {
         return node -> Object;
      }
   }
   return NULL;
}
//+
///f_shared_close
F_LIB_SHARED_CLOSE
{
   if (Shared)
   {
      struct FeelinSharedNode *node;

      ObtainSemaphore(&FeelinBase -> SharedList.Semaphore);

      for (node = (struct FeelinSharedNode *)(FeelinBase -> SharedList.Head) ; node ; node = node -> Next)
      {
         if (node -> Object == Shared)
         {
            node -> Refs--;
            
            if (node -> Refs == 0)
            {
               F_SharedDelete(Shared);
            }

            break;
         }
      }
   
      if (!node)
      {
         F_Log(FV_LOG_DEV,"F_SharedClose() %s{%08lx} not found",_classname(Shared),Shared);
      }

      ReleaseSemaphore(&FeelinBase -> SharedList.Semaphore);
   }
}
//+

