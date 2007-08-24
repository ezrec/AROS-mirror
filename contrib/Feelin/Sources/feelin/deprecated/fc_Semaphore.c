/*
**    $VER: fc_Semaphore.c 1.04 (2005/04/08)
**
**    Semaphore class.
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

 * 01.04 ** 2005/04/08 *
 
   The  method  FM_Semaphore_Lock  could  be  call  upon  an  uninitialized
   semaphore  e.g.  when  a  subclass  invoke the method before passing the
   FM_New method. This was usualy leading to a task  hang.  This  is  fixed
   now,  if  the  semaphore  has not yet been initialized if is initialized
   just before locking.
   
   There was also a problem with flags handling in FM_Semaphore_Lock. I was
   checking  FF_Semaphore_Shared first instead of FF_Semaphore_Attempt, and
   because FF_Semaphore_Exclusive  is  0  (no  flag  is  defined),  locking
   semaphore with FF_Semaphore_Shared was leading to an AttemptSemaphore().
   Thus, unlocking it after was leading to an illegal semaphore state.

*/

#include "Private.h"

struct LocalObjectData
{
    struct SignalSemaphore          Semaphore;
    UWORD                           Flags;
};

#define FF_Semaphore_Public                     (1 << 0)
#define FF_Semaphore_Initialized                (1 << 1)

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Semaphore_New
F_METHOD(ULONG,Semaphore_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   InitSemaphore(&LOD -> Semaphore);
   
   LOD -> Flags |= FF_Semaphore_Initialized;

#ifdef DEBUG_CODE

   if (FF_DEBUG_PUBLICSEMAPHORES & FeelinBase -> DebugFlags)
   {
      F_Set(Obj,FA_Semaphore_Public,TRUE);
   }

#endif
 
   return F_SUPERDO();
}
//+
///Semaphore_Dispose
F_METHOD(void,Semaphore_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (FF_Semaphore_Public & LOD -> Flags)
   {
      LOD -> Flags &= ~FF_Semaphore_Public;

      RemSemaphore(&LOD -> Semaphore);
   }

   F_SUPERDO();
}
//+
///Semaphore_Get
F_METHOD(void,Semaphore_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Semaphore_Name:    F_STORE(LOD -> Semaphore.ss_Link.ln_Name); break;
      case FA_Semaphore_Public:  F_STORE(0 != (FF_Semaphore_Public & LOD -> Flags)); break;
   }

   F_SUPERDO();
}
//+
///Semaphore_Set
F_METHOD(void,Semaphore_Set)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Semaphore_Name:
      {
         LOD -> Semaphore.ss_Link.ln_Name = (STRPTR)(item.ti_Data);
      }
      break;

      case FA_Semaphore_Public:
      {
#ifdef DEBUG_CODE

         if (FF_DEBUG_PUBLICSEMAPHORES & FeelinBase -> DebugFlags)
         {
            item.ti_Data = TRUE;
            
            if (!LOD -> Semaphore.ss_Link.ln_Name)
            {
               LOD -> Semaphore.ss_Link.ln_Name = "FC_Semaphore.Unamned";
            }
         }

#endif
 
         if (item.ti_Data)
         {
            if (!(FF_Semaphore_Public & LOD -> Flags))
            {
               LOD -> Flags |= FF_Semaphore_Public;

               Forbid();
               Enqueue(&SysBase -> SemaphoreList,&LOD -> Semaphore);
               Permit();
            }
         }
         else
         {
            if (FF_Semaphore_Public & LOD -> Flags)
            {
               Forbid();
               RemSemaphore(&LOD -> Semaphore);
               Permit();

               LOD -> Flags &= ~FF_Semaphore_Public;
            }
         }
      }
   }
   F_SUPERDO();
}
//+
///Semaphore_Lock
F_METHODM(ULONG,Semaphore_Lock,FS_Semaphore_Lock)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (!(FF_Semaphore_Initialized & LOD -> Flags))
   {
      F_Log(0,"Semaphore was not initialized yet");
 
      InitSemaphore(&LOD -> Semaphore);
   }
/*
   F_Log(0,"Owner (%s)(0x%08lx)(%ld)(%ld)",
      LOD -> Semaphore.ss_Owner -> tc_Node.ln_Name,
      LOD -> Semaphore.ss_Owner,
      LOD -> Semaphore.ss_NestCount,
      LOD -> Semaphore.ss_QueueCount);
      
   if (!LOD -> Semaphore.ss_Owner &&
        LOD -> Semaphore.ss_QueueCount != -1)
   {
      F_Log(0,"There is no Owner, QueueCount shoud be -1 (%ld)",LOD -> Semaphore.ss_QueueCount);
      
      LOD -> Semaphore.ss_QueueCount = -1;
   }
*/

   if (FF_Semaphore_Attempt & Msg -> Flags)
   {
      if (FF_Semaphore_Shared & Msg -> Flags)
      {
//         F_Log(0,"AttemptSemaphoreShared");
         return AttemptSemaphoreShared(&LOD -> Semaphore);
      }
      else
      {
//         F_Log(0,"AttemptSemaphore");
         return AttemptSemaphore(&LOD -> Semaphore);
      }
   }
   else if (FF_Semaphore_Shared & Msg -> Flags)
   {
//      F_Log(0,"ObtainSemaphoreShared");
      ObtainSemaphoreShared(&LOD -> Semaphore);
   }
   else
   {
//      F_Log(0,"ObtainSemaphore");
      ObtainSemaphore(&LOD -> Semaphore);
   }

//   F_Log(0,"DONE");
   return 0;
}
//+
///Semaphore_Unlock
F_METHOD(void,Semaphore_Unlock)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

//   F_Log(0,"Owner (%s)(0x%08lx)",LOD -> Semaphore.ss_Owner -> tc_Node.ln_Name,LOD -> Semaphore.ss_Owner);

   ReleaseSemaphore(&LOD -> Semaphore);
}
//+

/****************************************************************************
*** Constructor *************************************************************
****************************************************************************/

STATIC F_ATTRIBUTES_ARRAY =
{
   F_ATTRIBUTES_ADD_STATIC("Name",     FV_STRING,  FA_Semaphore_Name),
   F_ATTRIBUTES_ADD_STATIC("Public",   FV_BOOLEAN, FA_Semaphore_Public),
   
   F_ARRAY_END
};

STATIC F_METHODS_ARRAY =
{
   F_METHODS_ADD_STATIC (Semaphore_New,                  FM_New),
   F_METHODS_ADD_STATIC (Semaphore_Dispose,              FM_Dispose),
   F_METHODS_ADD_STATIC (Semaphore_Get,                  FM_Get),
   F_METHODS_ADD_STATIC (Semaphore_Set,                  FM_Set),
   F_METHODS_ADD_BOTH   (Semaphore_Lock,     "Lock",     FM_Semaphore_Lock),
   F_METHODS_ADD_BOTH   (Semaphore_Unlock,   "Unlock",   FM_Semaphore_Unlock),
   
   F_ARRAY_END
};

STATIC F_TAGS_ARRAY =
{
   F_TAGS_ADD(Super,      FC_Object),
   F_TAGS_ADD(LODSize,    sizeof (struct LocalObjectData)),
   F_TAGS_ADD(Methods,    F_METHODS),
   F_TAGS_ADD(Attributes, F_ATTRIBUTES),
   
   F_ARRAY_END
};

///fc_semaphore_create
FClass * fc_semaphore_create(struct in_FeelinBase *FeelinBase)
{
   return F_CreateClassA(FC_Semaphore,F_TAGS);
}
//+

