/*
 * @(#) $Header$
 *
 * BGUI library
 * rootclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.5  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.4  2003/01/18 19:10:00  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.3  2000/08/17 15:09:18  chodorowski
 * Fixed compiler warnings.
 *
 * Revision 42.2  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.1  2000/05/15 19:27:02  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:10:06  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:01  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.7  1999/07/31 01:56:37  mlemos
 * Added calls to the code that keeps track of the created objects.
 *
 * Revision 41.10.2.6  1999/07/03 15:17:45  mlemos
 * Replaced the calls to CallHookPkt to BGUI_CallHookPkt.
 *
 * Revision 41.10.2.5  1998/11/21 00:27:15  mlemos
 * Fixed erroneous code in OM_UPDATE.
 *
 * Revision 41.10.2.4  1998/11/19 01:23:33  mlemos
 * Fixed the return value type of BASE_ADDMAP value.
 *
 * Revision 41.10.2.3  1998/09/19 01:54:21  mlemos
 * Made BASE_ADDHOOK method return TRUE on success rather than the hook linet
 * entry pointer.
 *
 * Revision 41.10.2.2  1998/03/02 23:49:04  mlemos
 * Switched vector allocation functions calls to BGUI allocation functions.
 *
 * Revision 41.10.2.1  1998/03/01 15:39:19  mlemos
 * Added support to track BaseInfo memory leaks.
 *
 * Revision 41.10  1998/02/25 21:13:01  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:41  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

#define NOTIFY_MAP      1
#define NOTIFY_ATTR     2
#define NOTIFY_METHOD   3
#define NOTIFY_HOOK     4

typedef struct Notif {
   struct Notif      *n_Next;
   struct Notif      *n_Prev;
   UBYTE              n_Type;
   BYTE               n_Pri;
   ULONG              n_Flags;
   struct TagItem     n_Condition;
   Object            *n_Object;
}  NOTIF;

/*
 * Structures to track notification
 */
typedef struct {
   NOTIF              m_Node;
   struct TagItem    *m_MapList;
}  MAP;

typedef struct {
   NOTIF              a_Node;
   struct TagItem     a_Attr;
}  ATTR;

typedef struct {
   NOTIF              m_Node;
   ULONG              m_Method[1];
}  METHOD;

typedef struct {
   NOTIF              h_Node;
   struct Hook       *h_Hook;
}  HOOK;


/*
 * Object instance data.
 */
typedef struct {
   struct MinNode     rd_Node;               /* Node for adding to objects.       */
   WORD               rd_NodeOffset;         /* Offset of node from object.       */
   struct MinList     rd_NotifyList;         /* Notification list.                */
   UWORD              rd_LoopCount;          /* To prevent infinite notification. */
}  RD;

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(RootClassNew, struct opSet *, ops)
{
   RD             *rd;
   ULONG           rc;

   /*
    * We let the superclass setup an object
    * for us.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      /*
       * Get the instance data.
       */
      rd = INST_DATA(cl, rc);

      rd->rd_NodeOffset = (BYTE *)rc - (BYTE *)rd;

      /*
       * Initialize the notification lists.
       */
      NewList((struct List *)&rd->rd_NotifyList);
#ifdef DEBUG_BGUI
      TrackNewObject((Object *)rc,ops->ops_AttrList);
#endif
   }
   return rc;
}
METHOD_END
///
/// OM_NOTIFY
/*
 * We need to update our targets.
 */
METHOD(RootClassNotify, struct opUpdate *, opu)
{
   RD               *rd = INST_DATA(cl, obj);
   NOTIF            *n = (NOTIF *)rd->rd_NotifyList.mlh_Head;
   struct TagItem   *clones;
   Object           *target;
   struct TagItem   *tag;
   ULONG            *msg;
   ULONG             rc;
   BOOL              cond;
   int               type;

   /*
    * Make sure that the ICA_xxx
    * attributes will function.
    */
   rc = AsmDoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * Broadcast the message to all
    * targets mapping the attributes
    * when necessary.
    */
   while (n->n_Next)
   {
      type   = n->n_Type;
      target = n->n_Object;

      /*
       * Object counter not 0?  Do not update.
       */
      if (target)
      {
	 /*
	  * Peng...
	  */
	 if (AsmDoMethod(target, RM_SETLOOP))
	    /*
	     * Break the loop...we are done.
	     */
	    return rc;
      };

      /*
       * Check condition, if we have one.
       */
      if (n->n_Flags & (RAF_FALSE|RAF_TRUE))
      {
	 cond = FALSE;
	 /*
	  * Conditional tag present in
	  * the notification tags?
	  */
	 if (tag = FindTagItem(n->n_Condition.ti_Tag, opu->opu_AttrList))
	 {
	    cond = (n->n_Condition.ti_Data == tag->ti_Data);
	    if (n->n_Flags & RAF_FALSE) cond = !cond;
	 };
	 if (!cond) type = 0;
      };

      /*
       * Skip interim messages?
       */
      if (n->n_Flags & RAF_NO_INTERIM)
      {
	 if (opu->opu_Flags & OPUF_INTERIM) type = 0;
      };

      switch (type)
      {
      case NOTIFY_MAP:
	 /*
	  * When the object has a maplist
	  * we map the clones.
	  */
	 if (((MAP *)n)->m_MapList)
	 {
	    /*
	     * Clone the original attribute list.
	     */
	    if (clones = CloneTagItems(opu->opu_AttrList))
	    {
	       MapTags(clones, ((MAP *)n)->m_MapList, MAP_KEEP_NOT_FOUND);

	       AsmDoMethod(target, OM_UPDATE, clones, (n->n_Flags & RAF_NO_GINFO) ? NULL : opu->opu_GInfo, opu->opu_Flags);

	       /*
		* Free the clones.
		*/
	       FreeTagItems(clones);
	    };
	 }
	 else
	 {
	    AsmDoMethodA(n->n_Object, (Msg)opu);
	 };
	 break;

      case NOTIFY_ATTR:
	 /*
	  * Set tag.
	  */
	 DoSetMethod(target, (n->n_Flags & RAF_NO_GINFO) ? NULL : opu->opu_GInfo,
	    ((ATTR *)n)->a_Attr.ti_Tag, ((ATTR *)n)->a_Attr.ti_Data, TAG_DONE);
	 break;

      case NOTIFY_METHOD:
	 msg = ((METHOD *)n)->m_Method;

	 /*
	  * Set GadgetInfo?
	  */
	 if (!(n->n_Flags & RAF_NO_GINFO))
	 {
	    switch (msg[0])
	    {
	    case OM_NEW:
	    case OM_SET:
	    case OM_UPDATE:
	    case OM_NOTIFY:
	       /*
		* These methods get the GadgetInfo in the third long-word.
		*/
	       msg[2] = (ULONG)opu->opu_GInfo;
	       break;

	    default:
	       /*
		* All others in the second long-word.
		*/
	       msg[1] = (ULONG)opu->opu_GInfo;
	       break;
	    };
	 };
	 AsmDoMethodA(target, (Msg)msg);
	 break;

      case NOTIFY_HOOK:
	 if (!BGUI_CallHookPkt(((HOOK *)n)->h_Hook, (void *)obj, (void *)opu))
	    return rc;
	 break;
      };

      if (target) AsmDoMethod(target, RM_CLEARLOOP);

      n = n->n_Next;
   };
   return rc;
}
METHOD_END
///
/// OM_DISPOSE

STATIC VOID FreeNotif(NOTIF *n)
{
   switch (n->n_Type)
   {
   case NOTIFY_MAP:
      FreeTagItems(((MAP *)n)->m_MapList);
      break;
   };
   BGUI_FreePoolMem(n);
}

/*
 * They do not need us anymore.
 */
METHOD(RootClassDispose, Msg, msg)
{
   RD          *rd = INST_DATA(cl, obj);
   NOTIF       *n;

   /*
    * Remove all targets and deallocate all resources.
    */
   while (n = (NOTIF *)RemHead((struct List *)&rd->rd_NotifyList))
      FreeNotif(n);

   /*
    * Remove us from any list we may be in.
    */
   AsmDoMethod(obj, RM_REMOVE);

   /*
    * Let the superclass dispose of us.
    */
#ifdef DEBUG_BGUI
   TrackDisposedObject(obj);
#endif
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// OM_UPDATE
/*
 * Emulate OM_UPDATE with new methods.
 */
METHOD(RootClassUpdateX, struct opUpdate *, opu)
{
   ULONG rc = AsmCoerceMethod(cl,obj, RM_SETM, opu->opu_AttrList, (opu->opu_Flags & OPUF_INTERIM) ? RAF_INTERIM|RAF_UPDATE : RAF_UPDATE);

   if (opu->opu_GInfo
   && !(opu->opu_Flags & OPUF_INTERIM))
      AsmDoMethod(obj, RM_REFRESH, opu->opu_GInfo, rc);

   return rc;
}
METHOD_END
///
/// OM_SET
/*
 * Emulate OM_SET with new methods.
 */
METHOD(RootClassSetX, struct opSet *, ops)
{
   ULONG rc = AsmDoMethod(obj, RM_SETM, ops->ops_AttrList, 0);

   if (ops->ops_GInfo) AsmDoMethod(obj, RM_REFRESH, ops->ops_GInfo, rc);

   return rc;
}
METHOD_END
///
/// OM_GET
/*
 * Emulate OM_GET with new methods.
 */
METHOD(RootClassGetX, struct opGet *, opg)
{
   return AsmDoMethod(obj, RM_GET, &opg->opg_AttrID, 0);
}
METHOD_END
///
/// RM_SET
/*
 * Emulate RM_SET for any superclass.
 */
METHOD(RootClassSet, struct rmAttr *, ra)
{
   struct TagItem tags[2];
   ULONG          rc;

   tags[0].ti_Tag  = ra->ra_Attr->ti_Tag;
   tags[0].ti_Data = ra->ra_Attr->ti_Data;
   tags[1].ti_Tag  = TAG_DONE;

   rc = AsmDoSuperMethod(cl, obj, (ra->ra_Flags & RAF_UPDATE) ? OM_UPDATE : OM_SET, tags, NULL,
				  (ra->ra_Flags & RAF_INTERIM) ? OPUF_INTERIM : 0);

   return (ULONG)0;//((rc > 0) ? (RAF_UNDERSTOOD|RAF_REDRAW) : 0);
}
METHOD_END
///
/// RM_GET
/*
 * Emulate RM_GET for any superclass.
 */
METHOD(RootClassGet, struct rmAttr *, ra)
{
   AsmDoSuperMethod(cl, obj, OM_GET, ra->ra_Attr->ti_Tag, ra->ra_Attr->ti_Data);

   return 0;
}
METHOD_END
///
/// RM_SETM

METHOD(RootClassSetM, struct rmAttr *, ra1)
{
   struct rmAttr   ra;
   struct TagItem *tstate = ra1->ra_Attr;
   ULONG           rc = 0;

   ra.MethodID   = RM_SET;
   ra.ra_Flags   = ra1->ra_Flags;

   while (ra.ra_Attr = BGUI_NextTagItem(&tstate))
      rc |= AsmDoMethodA(obj, (Msg)&ra);

   return rc;
}
METHOD_END
///
/// RM_GETM

METHOD(RootClassGetM, struct rmAttr *, ra1)
{
   struct rmAttr   ra;
   struct TagItem *tstate = ra1->ra_Attr;
   ULONG           rc = 0;

   ra.MethodID   = RM_GET;
   ra.ra_Flags   = ra1->ra_Flags;

   while (ra.ra_Attr = BGUI_NextTagItem(&tstate))
      rc |= AsmDoMethodA(obj, (Msg)&ra);

   return rc;
}
METHOD_END
///

/// AddNotify

void AddNotify(RD *rd, NOTIF *n, Msg msg, int type)
{
   /*
    * Set notify object.
    */
   n->n_Type      = type;
   n->n_Pri       = ((struct rmAddAttr *)msg)->raa_Priority;
   n->n_Flags     = ((struct rmAddAttr *)msg)->raa_Flags;
   n->n_Condition = ((struct rmAddAttr *)msg)->raa_Condition;
   n->n_Object    = ((struct rmAddAttr *)msg)->raa_Object;

   /*
    * Add the target to the list.
    */
   Enqueue((struct List *)&rd->rd_NotifyList, (struct Node *)n);
}
///
/// RM_ADDMAP
/*
 * Add a map object to the list.
 */
METHOD(RootClassAddMap, struct rmAddMap *, ram)
{
   RD             *rd = INST_DATA(cl, obj);
   MAP            *am = NULL;

   /*
    * Check if the object is valid.
    */
   if (ram->ram_Object && (ram->ram_Object != obj))
   {
      /*
       * Allocate and initialize a MAP structure.
       */
      if (am = (MAP *)BGUI_AllocPoolMem(sizeof(MAP)))
      {
	 /*
	  * Check if we need to make a map list.
	  */
	 if (ram->ram_MapList)
	 {
	    /*
	     * Clone the maplist.
	     */
	    if (!(am->m_MapList = CloneTagItems(ram->ram_MapList)))
	    {
	       BGUI_FreePoolMem(am);
	       return NULL;
	    };
	 };

	 /*
	  * Add notification.
	  */
	 AddNotify(rd, (NOTIF *)am, (Msg)ram, NOTIFY_MAP);
      };
   }
   return (ULONG)am;
}
METHOD_END
///
/// RM_ADDATTR
/*
 * Add an attribute conditional object to the list.
 */
METHOD(RootClassAddAttr, struct rmAddAttr *, raa)
{
   RD          *rd = INST_DATA(cl, obj);
   ATTR        *aa = NULL;

   /*
    * Check if the object is valid.
    */
   if (raa->raa_Object && (raa->raa_Object != obj))
   {
      /*
       * Allocate and initialize an ATTR structure.
       */
      if (aa = (ATTR *)BGUI_AllocPoolMem(sizeof(ATTR)))
      {
	 /*
	  * Copy the attribute.
	  */
	 aa->a_Attr = raa->raa_Attr;

	 /*
	  * Add notification.
	  */
	 AddNotify(rd, (NOTIF *)aa, (Msg)raa, NOTIFY_ATTR);
      };
   }
   return (ULONG)aa;
}
METHOD_END
///
/// RM_ADDMETHOD
/*
 * Add a method object to the list.
 */
METHOD(RootClassAddMethod, struct rmAddMethod *, ram)
{
   RD         *rd = INST_DATA(cl, obj);
   METHOD     *am = NULL;

   /*
    * Check if the object is valid.
    */
   if (ram->ram_Size && ram->ram_Object && (ram->ram_Object != obj))
   {
      /*
       * Allocate and initialize a METHOD structure.
       */
      if (am = (METHOD *)BGUI_AllocPoolMem(sizeof(METHOD) + ram->ram_Size - sizeof(ULONG)))
      {
	 /*
	  * Copy the method.
	  */
	 CopyMem((void *)&ram->ram_MethodID, (void *)am->m_Method, ram->ram_Size);

	 /*
	  * Add notification.
	  */
	 AddNotify(rd, (NOTIF *)am, (Msg)ram, NOTIFY_METHOD);
      };
   };
   return (ULONG)am;
}
METHOD_END
///
/// RM_ADDHOOK
/*
 * Add a hook to the list.
 */
METHOD(RootClassAddHook, struct rmAddHook *, rah)
{
   RD          *rd = INST_DATA(cl, obj);
   HOOK        *ah = NULL;

   /*
    * Check if the hook is valid.
    */
   if (rah->rah_Hook)
   {
      /*
       * Allocate and initialize a HOOK structure.
       */
      if (ah = (HOOK *)BGUI_AllocPoolMem(sizeof(HOOK)))
      {
	 /*
	  * Copy the hook.
	  */
	 ah->h_Hook = rah->rah_Hook;

	 /*
	  * Add notification.
	  */
	 AddNotify(rd, (NOTIF *)ah, (Msg)rah, NOTIFY_HOOK);

	 ah->h_Node.n_Object = NULL;
      };
   };
   return (ULONG)ah;
}
METHOD_END
///

/*
 * Remove an object from a list.
 */
METHOD(NotifyClassRemove, struct bmRemove *, brt)
{
   RD           *rd = INST_DATA(cl, obj);
   NOTIF        *n = (NOTIF *)rd->rd_NotifyList.mlh_Head;
   ULONG         type;

#ifdef __AROS__
#warning A comment within a comment makes gcc puke...
#if 0
   /*
   switch (brt->MethodID)
   {
   case BASE_REMMAP:
      type = NOTIFY_TARGET;
      break;
   case BASE_REMCONDITIONAL:
      type = NOTIFY_CONDITIONAL;
      break;
   case BASE_REMMETHOD:
      type = NOTIFY_METHOD;
      break;
   case BASE_REMHOOK:
      type = NOTIFY_HOOK;
      break;
   };

   /*
    * Find an object in the target list.
    */
   while (n->n_Next)
   {
      if ((n->n_Type == type) && ((n->n_Object == brt->bar_Object) || (!n->n_Object)))
      {
	 /*
	  * Remove it from the list.
	  */
	 Remove((struct Node *)n);

	 /*
	  * Deallocate the structure.
	  */
	 FreeNotif(n);
	 
	 return 1;
      };
      n = n->n_Next;
   };
   */
#endif
#endif
   return 0;
}
METHOD_END

/// RM_SETLOOP
/*
 * Set infinite loop counter.
 */
METHOD(RootClassSetLoop, Msg, msg)
{
   RD          *rd = INST_DATA(cl, obj);

   if (rd->rd_LoopCount)
      return TRUE;

   rd->rd_LoopCount++;

   return FALSE;
}
METHOD_END
///
/// RM_CLEARLOOP
/*
 * Clear infinite loop counter.
 */
METHOD(RootClassClearLoop, Msg, msg)
{
   RD          *rd = INST_DATA(cl, obj);

   rd->rd_LoopCount--;

   return FALSE;
}
METHOD_END
///

/// RM_REMOVE

METHOD(RootClassRemove, Msg, msg)
{
   RD             *rd = INST_DATA(cl, obj);
   struct MinNode *mln = &rd->rd_Node;

   if (mln->mln_Succ && mln->mln_Pred)
   {
      Remove((struct Node *)mln);
      mln->mln_Succ = mln->mln_Pred = NULL;
      return 1;
   };
   return 0;
}
METHOD_END
///
/// RM_ADDHEAD

METHOD(RootClassAddHead, struct rmAdd *, ra)
{
   RD           *rd = INST_DATA(cl, obj);

   AddHead(ra->ra_List, (struct Node *)&rd->rd_Node);

   return 1;
}
METHOD_END
///
/// RM_ADDTAIL

METHOD(RootClassAddTail, struct rmAdd *, ra)
{
   RD           *rd = INST_DATA(cl, obj);

   AddTail(ra->ra_List, (struct Node *)&rd->rd_Node);

   return 1;
}
METHOD_END
///
/// RM_INSERT

METHOD(RootClassInsert, struct rmInsert *, ri)
{
   RD           *rd = INST_DATA(cl, obj), *rd2;

   if (ri->ri_Previous)
   {
      rd2 = INST_DATA(cl, ri->ri_Previous);
      Insert(ri->ri_List, (struct Node *)&rd->rd_Node, (struct Node *)&rd2->rd_Node);
   }
   else
   {
      AddHead(ri->ri_List, (struct Node *)&rd->rd_Node);
   };
   return 1;
}
METHOD_END
///
/// RM_PREV

METHOD(RootClassPrev, Msg, msg)
{
   RD             *rd = INST_DATA(cl, obj);
   struct MinNode *mn = rd->rd_Node.mln_Pred;

   if (mn && mn->mln_Pred)
      return (ULONG)((UBYTE *)mn + rd->rd_NodeOffset);

   return NULL;
}
METHOD_END
///
/// RM_NEXT

METHOD(RootClassNext, Msg, msg)
{
   RD             *rd = INST_DATA(cl, obj);
   struct MinNode *mn = rd->rd_Node.mln_Succ;

   if (mn && mn->mln_Succ)
      return (ULONG)((UBYTE *)mn + rd->rd_NodeOffset);

   return NULL;
}
METHOD_END
///

makeproto Object *ListHeadObject(struct List *lh)
{
   RD *rd     = (RD *)(lh->lh_Head);

   if (rd->rd_Node.mln_Succ) return (Object *)(((UBYTE *)rd) + rd->rd_NodeOffset);
   else                      return NULL;
}
makeproto Object *ListTailObject(struct List *lh)
{
   RD *rd     = (RD *)(lh->lh_TailPred);

   if (rd->rd_Node.mln_Pred) return (Object *)(((UBYTE *)rd) + rd->rd_NodeOffset);
   else                      return NULL;
}

/// BASE_ADDMAP

METHOD(BaseClassAddMap, struct bmAddMap *, bam)
{
   return((ULONG)(AsmDoMethod(obj, RM_ADDMAP, 0, 0, 0, 0, bam->bam_Object, bam->bam_MapList)!=NULL));
}
METHOD_END
///
/// BASE_ADDMETHOD

METHOD(BaseClassAddMethod, struct bmAddMethod *, bam)
{
   struct rmAddMethod *ram;

   int    size = bam->bam_Size * sizeof(ULONG);
   ULONG  nh = 0;

   if (ram = BGUI_AllocPoolMem(sizeof(struct bmAddMethod) + size))
   {
      ram->MethodID   = RM_ADDMETHOD;
      ram->ram_Object = bam->bam_Object;
      ram->ram_Flags  = bam->bam_Flags;
      ram->ram_Size   = size;

      CopyMem((void *)&bam->bam_MethodID, (void *)&ram->ram_MethodID, size);

      nh = AsmDoMethodA(obj, (Msg)ram);

      BGUI_FreePoolMem(ram);
   };
   return nh;
}
METHOD_END
///
/// BASE_ADDCONDITIONAL

METHOD(BaseClassAddConditional, struct bmAddConditional *, bac)
{
   ULONG nh;

   if (nh = AsmDoMethod(obj, RM_ADDATTR, 0, RAF_FALSE, bac->bac_Condition.ti_Tag, bac->bac_Condition.ti_Data,
			     bac->bac_Object,          bac->bac_FALSE.ti_Tag,     bac->bac_FALSE.ti_Data))
   {
      if (AsmDoMethod(obj, RM_ADDATTR, 0, RAF_TRUE,  bac->bac_Condition.ti_Tag, bac->bac_Condition.ti_Data,
			   bac->bac_Object,          bac->bac_TRUE.ti_Tag,      bac->bac_TRUE.ti_Data))
	 return TRUE;

      AsmDoMethod(obj, RM_REMNOTIFY, 0, nh);
   }
   return FALSE;
}
METHOD_END
///
/// BASE_ADDHOOK

METHOD(BaseClassAddHook, struct bmAddHook *, bah)
{
   return((ULONG)(AsmDoMethod(obj, RM_ADDHOOK, 0, 0, 0, 0, bah->bah_Hook)!=NULL));
}
METHOD_END
///

/// IM_DRAW

METHOD(ImageClassDraw, struct impDraw *, imp)
{
   int              x = imp->imp_Offset.X;
   int              y = imp->imp_Offset.Y;
   ULONG            rc;
   struct BaseInfo *bi;

#ifdef DEBUG_BGUI
   if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_RastPort, imp->imp_RPort, BI_DrawInfo, imp->imp_DrInfo, TAG_DONE))
#else
   if (bi = AllocBaseInfo(BI_RastPort, imp->imp_RPort, BI_DrawInfo, imp->imp_DrInfo, TAG_DONE))
#endif
   {
      if (x) IMAGE(obj)->LeftEdge += x;
      if (y) IMAGE(obj)->TopEdge  += y;

      rc = AsmDoMethod(obj, BASE_RENDER, bi, imp->imp_State);

      if (x) IMAGE(obj)->LeftEdge -= x;
      if (y) IMAGE(obj)->TopEdge  -= y;

      FreeBaseInfo(bi);
   };
   return rc;
}
METHOD_END
///

/// Class initialization.
/*
 * Class function array.
 */
STATIC DPFUNC ClassFunc[] =
{
   RM_SET,                 (FUNCPTR)RootClassSet,
   RM_GET,                 (FUNCPTR)RootClassGet,
   RM_SETM,                (FUNCPTR)RootClassSetM,
   RM_GETM,                (FUNCPTR)RootClassGetM,
   OM_SET,                 (FUNCPTR)RootClassSetX,
   OM_GET,                 (FUNCPTR)RootClassGetX,
   OM_UPDATE,              (FUNCPTR)RootClassUpdateX,
   OM_NEW,                 (FUNCPTR)RootClassNew,
   OM_DISPOSE,             (FUNCPTR)RootClassDispose,
   OM_NOTIFY,              (FUNCPTR)RootClassNotify,
   RM_SETLOOP,             (FUNCPTR)RootClassSetLoop,
   RM_CLEARLOOP,           (FUNCPTR)RootClassClearLoop,

   RM_REMOVE,              (FUNCPTR)RootClassRemove,
   RM_ADDHEAD,             (FUNCPTR)RootClassAddHead,
   RM_ADDTAIL,             (FUNCPTR)RootClassAddTail,
   RM_INSERT,              (FUNCPTR)RootClassInsert,
   RM_NEXT,                (FUNCPTR)RootClassNext,
   RM_PREV,                (FUNCPTR)RootClassPrev,

   RM_ADDMAP,              (FUNCPTR)RootClassAddMap,
   RM_ADDATTR,             (FUNCPTR)RootClassAddAttr,
   RM_ADDMETHOD,           (FUNCPTR)RootClassAddMethod,
   RM_ADDHOOK,             (FUNCPTR)RootClassAddHook,

   BASE_ADDMAP,            (FUNCPTR)BaseClassAddMap,
   BASE_ADDCONDITIONAL,    (FUNCPTR)BaseClassAddConditional,
   BASE_ADDMETHOD,         (FUNCPTR)BaseClassAddMethod,
   BASE_ADDHOOK,           (FUNCPTR)BaseClassAddHook,
   BASE_REMMAP,            (FUNCPTR)NotifyClassRemove,
   BASE_REMCONDITIONAL,    (FUNCPTR)NotifyClassRemove,
   BASE_REMMETHOD,         (FUNCPTR)NotifyClassRemove,
   BASE_REMHOOK,           (FUNCPTR)NotifyClassRemove,

   IM_DRAW,                (FUNCPTR)ImageClassDraw,

   DF_END
};

Class *InitRootSubClass(char *superclass)
{
   return BGUI_MakeClass(CLASS_SuperClassID, superclass,
			 CLASS_ObjectSize,   sizeof(RD),
			 CLASS_DFTable,      ClassFunc,
			 TAG_DONE);
}

/*
 * Simple class initialization.
 */
makeproto Class *InitRootClass(void)
{
   return InitRootSubClass("rootclass");
}

makeproto Class *InitGadgetClass(void)
{
   return InitRootSubClass("gadgetclass");
}

makeproto Class *InitImageClass(void)
{
   return InitRootSubClass("imageclass");
}
///

