/*
 * @(#) $Header$
 *
 * BGUI library
 * task.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.10  1998/02/25 21:13:21  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:56  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/*
 * Global data.
 */
makeproto TL                     TaskList;   /* List of openers.         */
makeproto struct SignalSemaphore TaskLock;   /* For exclusive task-list access. */

static APTR MemPool = NULL;

static void LoadPrefs(UBYTE *name);
static void DefPrefs(void);

/*
 * Initialize task list.
 */
makeproto void InitTaskList(void)
{
   /*
    * Initialize task-list.
    */
   NewList((struct List *)&TaskList);

   /*
    * And window list.
    */
   NewList((struct List *)&TaskList.tl_WindowList);

   /*
    * Initialize task-list access semaphore.
    */
   InitSemaphore(&TaskLock);

   /*
    * Create memory pool.
    */
   MemPool = BGUI_CreatePool(MEMF_PUBLIC | MEMF_CLEAR, 4096, 4096);
}

/*
 * Free task list.
 */
makeproto void FreeTaskList(void)
{
   /*
    * Delete memory pool.
    */
   BGUI_DeletePool(MemPool);
}

/*
 * Find a task member.
 *
 * Changes made by T.Herold:
 *
 * InternalFindTaskMember searches for a certain task in
 * the tasklist. FindTaskMember does this with FindTask(NULL)
 * as a parameter (just the way it always did). This keeps
 * changes to other functions to a minimum.
 *
 * Only AddIDReport will use InternalFind...
 */

makeproto TM *FindTaskMember(void)
{
   return InternalFindTaskMember(FindTask(NULL));
}

makeproto TM *InternalFindTaskMember(struct Task *task)
{
   TM    *tm;

   /*
    *  Look through the list for the member.
    */
   for (tm = TaskList.tl_First; tm->tm_Next; tm = tm->tm_Next)
   {
      if (tm->tm_TaskAddr == task) return tm;
   }
   return NULL;
}


/*
 * Add a task member to the list.
 */
makeproto UWORD AddTaskMember(void)
{
   TM       *tm;
   UWORD     rc = TASK_FAILED;
   UBYTE     buffer[64];

   /*
    * Lock the list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Does this task already have the
    * library open?
    */
   if (tm = FindTaskMember())
   {
      /*
       * Yes. Increase open counter,
       * unlock the semaphore and
       * exit.
       */
      tm->tm_Counter++;
      ReleaseSemaphore(&TaskLock);
      return TASK_REOPENED;
   };

   /*
    * Allocate a task member structure.
    */
   if (tm = AllocVecMem(MemPool, sizeof(TM)))
   {
      /*
       * Setup task address and counter.
       */
      tm->tm_TaskAddr = FindTask(NULL);
      tm->tm_Counter  = 1;

      /*
       * Initialize lists.
       */
      NewList((struct List *)&tm->tm_Windows);
      NewList((struct List *)&tm->tm_Prefs);
      NewList((struct List *)&tm->tm_RID);

      /*
       * Append it to the list.
       */
      AddTail((struct List *)&TaskList, (struct Node *)tm);

      DefPrefs();
      sprintf(buffer, "ENV:BGUI/%s.prefs", "Default");
      LoadPrefs(buffer);
      sprintf(buffer, "ENV:BGUI/%s.prefs", FilePart(((struct Node *)(tm->tm_TaskAddr))->ln_Name));
      LoadPrefs(buffer);

      rc = TASK_ADDED;
   }

   /*
    * Unlock the list.
    */
   ReleaseSemaphore(&TaskLock);

   return rc;
}


static void FreeTL_PI(PI *pi)
{
   FreeTagItems(pi->pi_Defaults);
   FreeVecMem(MemPool, pi);
}
static void FreeTL_WI(WI *wi)
{
   FreeVecMem(MemPool, wi);
}
static void FreeTL_RID(RID *rid)
{
   FreeVecMem(MemPool, rid);
}

/*
 * Free task member.
 */
makeproto BOOL FreeTaskMember( void )
{
   TM       *tm;
   PI       *pi;
   WI       *wi;
   RID      *rid;

   /*
    * Lock the list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Find the task member.
    */
   if (tm = FindTaskMember())
   {
      /*
       * Last opener?
       */
      if (--tm->tm_Counter)
      {
         /*
          * No. release the semaphore and exit.
          */
         ReleaseSemaphore(&TaskLock);
         return FALSE;
      }

      /*
       * Remove it from the list.
       */
      Remove((struct Node *)tm);

      /*
       * Remove and delete window/id stuff.
       */
      while (pi  = (PI *)RemHead((struct List *)&tm->tm_Prefs))   FreeTL_PI(pi);
      while (wi  = (WI *)RemHead((struct List *)&tm->tm_Windows)) FreeTL_WI(wi);
      while (rid = (RID *)RemHead((struct List *)&tm->tm_RID))    FreeTL_RID(rid);

      FreeVecMem(MemPool, tm);
   };

   /*
    * Unlock the list.
    */
   ReleaseSemaphore(&TaskLock);

   return TRUE;
}

/*
 * Look up a window.
 */
STATIC WI *FindWindowInfo(ULONG id, WL *wl)
{
   WI       *wi;

   /*
    * Scan the window list.
    */
   for (wi = wl->wl_First; wi->wi_Next; wi = wi->wi_Next)
   {
      if (id == wi->wi_WinID)
         return wi;
   }
   return NULL;
}

/*
 * Obtain the window bounds.
 */
makeproto BOOL GetWindowBounds(ULONG windowID, struct IBox *dest)
{
   TM       *tm;
   WI       *wi;
   BOOL         rc = FALSE;

   /*
    * No 0 ID!
    */
   if (!windowID)
      return FALSE;

   /*
    * Gain exclusive access.
    */
   ObtainSemaphore( &TaskLock );

   /*
    * Look up the task member.
    */
   if (tm = FindTaskMember())
   {
      /*
       * Window already in the list?
       */
      if (wi = FindWindowInfo(windowID, &tm->tm_Windows))
      {
         /*
          * Copy the data to dest.
          */
         *dest = wi->wi_Bounds;
         rc = TRUE;
      }
      else
      {
         /*
          * Allocate and append a new one.
          */
         if (wi = (WI *)AllocVecMem(MemPool, sizeof(WI)))
         {
            wi->wi_WinID = windowID;
            AddTail((struct List *)&tm->tm_Windows, (struct Node *)wi);
         }
      }
   }

   /*
    * Release lock.
    */
   ReleaseSemaphore(&TaskLock);

   return rc;
}

/*
 * Set the window bounds.
 */
makeproto VOID SetWindowBounds(ULONG windowID, struct IBox *set)
{
   TM       *tm;
   WI       *wi;

   /*
    * No 0 ID!
    */
   if (!windowID)
      return;

   /*
    * Gain exclusive access.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Look up the task member.
    */
   if (tm = FindTaskMember())
   {
      /*
       * Window in the list?
       */
      if (wi = FindWindowInfo(windowID, &tm->tm_Windows))
      {
         /*
          * Copy the data.
          */
         wi->wi_Bounds = *set;
      }
   }

   /*
    * Release lock.
    */
   ReleaseSemaphore( &TaskLock );
}

/*
 * Asm stubs for the pool routines.
 */
extern ASM APTR AsmCreatePool  ( REG(d0) ULONG, REG(d1) ULONG, REG(d2) ULONG, REG(a6) struct ExecBase * );
extern ASM APTR AsmAllocPooled ( REG(a0) APTR,  REG(d0) ULONG,                REG(a6) struct ExecBase * );
extern ASM APTR AsmFreePooled  ( REG(a0) APTR,  REG(a1) APTR,  REG(d0) ULONG, REG(a6) struct ExecBase * );
extern ASM APTR AsmDeletePool  ( REG(a0) APTR,                                REG(a6) struct ExecBase * );

/*
 * Create a memory pool.
 */
makeproto SAVEDS ASM APTR BGUI_CreatePool(REG(d0) ULONG memFlags, REG(d1) ULONG puddleSize, REG(d2) ULONG threshSize)
{
   #ifdef ENHANCED
   /*
    * V39 Exec function.
    */
   return CreatePool(memFlags, puddleSize, threshSize);

   #else
   /*
    * pools.lib function
    */
   return AsmCreatePool(memFlags, puddleSize, threshSize, SysBase);

   #endif
}

/*
 * Delete a memory pool.
 */
makeproto SAVEDS ASM VOID BGUI_DeletePool(REG(a0) APTR poolHeader)
{
   #ifdef ENHANCED
   /*
    * V39 Exec function.
    */
   DeletePool(poolHeader);

   #else
   /*
    * pools.lib function
    */
   AsmDeletePool(poolHeader, SysBase);

   #endif
}

/*
 * Allocate pooled memory.
 */
makeproto SAVEDS ASM APTR BGUI_AllocPooled(REG(a0) APTR poolHeader, REG(d0) ULONG memSize)
{
   #ifdef ENHANCED
   /*
    * V39 Exec function.
    */
   return AllocPooled(poolHeader, memSize);

   #else
   /*
    * pools.lib function
    */
   return AsmAllocPooled(poolHeader, memSize, SysBase);

   #endif
}

/*
 * Free pooled memory.
 */
makeproto SAVEDS ASM VOID BGUI_FreePooled(REG(a0) APTR poolHeader, REG(a1) APTR memory, REG(d1) ULONG memSize)
{
   #ifdef ENHANCED
   /*
    * V39 Exec function.
    */
   FreePooled(poolHeader, memory, memSize);

   #else
   /*
    * pools.lib function
    */
   AsmFreePooled(poolHeader, memory, memSize, SysBase);

   #endif
}

/*
 * Allocate memory.
 */
makeproto ASM APTR AllocVecMem(REG(a0) APTR mempool, REG(d0) ULONG size)
{
   ULONG    *mem;

   /*
    * Memory pool valid?
    */
   if (mempool)
   {
      size += sizeof(ULONG);
      /*
       * Allocate memory from the pool.
       */
      if (mem = (ULONG *)BGUI_AllocPooled(mempool, size))
      {
         /*
          * Store size.
          */
         *mem++ = size;
      };
   } else
      /*
       * Normal system allocation.
       */
      mem = (ULONG *)AllocVec(size, MEMF_PUBLIC | MEMF_CLEAR);

   return (APTR)mem;
}

/*
 * Free memory.
 */
makeproto ASM VOID FreeVecMem(REG(a0) APTR pool, REG(a1) APTR memptr)
{
   ULONG    *mem = (ULONG *)memptr;

   /*
    * Memory pool valid?
    */
   if (pool)
   {
      /*
       * Retrieve original allocation and size.
       */
      mem--;
      BGUI_FreePooled(pool, (APTR)mem, *mem);
   } else
      /*
       * Normal system de-allocation.
       */
      FreeVec(mem);
}

/*
 * Allocate memory from the pool.
 */
makeproto SAVEDS ASM APTR BGUI_AllocPoolMem(REG(d0) ULONG size)
{
   APTR        memPtr;

   /*
    * Lock the list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Allocate memory.
    */
   memPtr = AllocVecMem(MemPool, size);

   /*
    * Unlock the list.
    */
   ReleaseSemaphore(&TaskLock);

   return memPtr;
}

/*
 * Free memory from the pool.
 */
makeproto SAVEDS ASM VOID BGUI_FreePoolMem(REG(a0) APTR memPtr)
{
   if (memPtr)
   {
      /*
       * Lock the list.
       */
      ObtainSemaphore(&TaskLock);

      /*
       * Free the memory.
       */
      FreeVecMem(MemPool, memPtr);

      /*
       * Unlock the list.
       */
      ReleaseSemaphore(&TaskLock);
   };
}

/*
 *  Add an ID to the list.
 *
 *  Changes made by T.Herold
 *
 *  - Added reg(a1) Task param
 *  - Uses InternalFindTaskMember to find destination task.
 */

makeproto ASM BOOL AddIDReport(REG(a0) struct Window *window, REG(d0) ULONG id, REG(a1) struct Task *task)
{
   RID         *rid;
   TM          *tm;
   BOOL         rc = FALSE;

   /*
    *  Lock the task list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    *  Look up the task.
    */
   if (tm = InternalFindTaskMember(task))
   {
      /*
       *  Allocate an ID.
       */
      if (rid = (RID *)AllocVecMem(MemPool, sizeof(RID)))
      {
         /*
          *  Setup and add.
          */
         rid->rid_ID     = id;
         rid->rid_Window = window;
         AddTail((struct List *)&tm->tm_RID, (struct Node *)rid);
         rc = TRUE;
      };
   };

   /*
    *  Release the task list.
    */
   ReleaseSemaphore(&TaskLock);

   return rc;
}

/*
 * Get next ID for the window.
 */
makeproto ASM ULONG GetIDReport(REG(a0) struct Window *window)
{
   RID         *rid;
   TM          *tm;
   ULONG        rc = ~0;

   /*
    * Lock the task list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Find the task.
    */
   if (tm = FindTaskMember())
   {
      /*
       * See if an ID for the window
       * exists.
       */
      for (rid = tm->tm_RID.ril_First; rid->rid_Next; rid = rid->rid_Next)
      {
         /*
          * Is this the one?
          */
         if (rid->rid_Window == window)
         {
            /*
             * Yes. Get the ID and remove it.
             */
            rc = rid->rid_ID;
            Remove((struct Node *)rid);
            FreeTL_RID(rid);
            break;
         }
      }
   }

   /*
    * Release the task list.
    */
   ReleaseSemaphore(&TaskLock);

   return rc;
}

/*
 * Get the window of the first
 * ID on the stack.
 */
makeproto struct Window *GetFirstIDReportWindow(void)
{
   TM             *tm;
   struct Window  *win = NULL;

   /*
    * Lock the task list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Get the task.
    */
   if (tm = FindTaskMember())
   {
      /*
       * Are there any ID's?
       */
      if (tm->tm_RID.ril_First->rid_Next)
         win = tm->tm_RID.ril_First->rid_Window;
   }

   /*
    * Release the task list.
    */
   ReleaseSemaphore(&TaskLock);

   return win;
}

/*
 * Remove all ID's of this window.
 */
makeproto ASM VOID RemoveIDReport(REG(a0) struct Window *window)
{
   RID      *rid, *succ;
   TM       *tm;

   /*
    * Lock task list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Find the task.
    */
   if (tm = FindTaskMember())
   {
      rid = tm->tm_RID.ril_First;
      while (succ = rid->rid_Next)
      {
         if (rid->rid_Window == window)
         {
            Remove((struct Node *)rid);
            FreeTL_RID(rid);
         };
         rid = succ;
      }
   }

   /*
    * Release the task list.
    */
   ReleaseSemaphore(&TaskLock);
}

/*
 * Add an open window to the list.
 */
makeproto ASM VOID AddWindow(REG(a0) Object *wo, REG(a1) struct Window *win)
{
   WNODE       *wn;

   /*
    * Lock the list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Allocate node.
    */
   if (wn = (WNODE *)AllocVecMem(MemPool, sizeof(WNODE)))
   {
      /*
       * Set it up.
       */
      wn->wn_WindowObject = wo;
      wn->wn_Window       = win;

      /*
       * Add it to the list.
       */
      AddTail((struct List *)&TaskList.tl_WindowList, (struct Node *)wn);
   }

   /*
    * Release the list.
    */
   ReleaseSemaphore(&TaskLock);
}

/*
 * Remove a window from the list.
 */
makeproto ASM VOID RemWindow(REG(a0) Object *wo)
{
   WNODE       *wn;

   /*
    * Lock the list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Locate the node.
    */
   for (wn = TaskList.tl_WindowList.wnl_First; wn->wn_Next; wn = wn->wn_Next)
   {
      /*
       * Is this the one?
       */
      if (wn->wn_WindowObject == wo)
      {
         /*
          * Yes. Remove and deallocate it.
          */
         Remove((struct Node *)wn);
         FreeVecMem(MemPool, wn);

         /*
          * We're done here...
          */
         break;
      };
   };

   /*
    * Release the list.
    */
   ReleaseSemaphore(&TaskLock);
}

/*
 * Find the window located under the mouse.
 */
makeproto ASM Object *WhichWindow(REG(a0) struct Screen *screen)
{
   Object         *win = NULL;
   struct Layer   *layer;
   WNODE          *wn;

   /*
    * Lock the list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Find out the layer under the mouse pointer.
    */
   if (layer = WhichLayer(&screen->LayerInfo, screen->MouseX, screen->MouseY))
   {
      for (wn = TaskList.tl_WindowList.wnl_First; wn->wn_Next; wn = wn->wn_Next)
      {
         /*
          * Is this the one?
          */
         if (wn->wn_Window == layer->Window)
         {
            /*
             * Setup return code and exit loop.
             */
            win = wn->wn_WindowObject;
            break;
         };
      };
   };

   /*
    * Release the lock.
    */
   ReleaseSemaphore(&TaskLock);

   return win;
}

static PI *FindPrefInfo(TM *tm, ULONG id)
{
   PRL   *pl = &tm->tm_Prefs;
   PI    *pi;

   for (pi = pl->pl_First; pi->pi_Next; pi = pi->pi_Next)
   {
      if (id == pi->pi_PrefID)
      {
         return pi;
      };
   };
   return NULL;
}

#define TAG_PREFID (TAG_USER - 1)

static void NewPrefInfo(TM *tm, ULONG id, struct TagItem *tags)
{
   PRL        *pl = &tm->tm_Prefs;
   PI         *pi = FindPrefInfo(tm, id);

   if (!pi)
   {
      if (pi = AllocVecMem(MemPool, sizeof(PI)))
      {
         pi->pi_PrefID   = id;
         pi->pi_Defaults = NULL;
         AddTail((struct List *)pl, (struct Node *)pi);
      };
   };
   if (pi)
   {
      tags = BGUI_MergeTagItems(pi->pi_Defaults, tags);
      pi->pi_Defaults = BGUI_CleanTagItems(tags, 1);
   };
}

static void DefPrefs(void)
{
   TM              *tm;
   struct TagItem  *t1, *t2;
   ULONG            id, type;

   static struct TagItem deftags[] =
   {
      TAG_PREFID,            BGUI_AREA_GADGET,
      ICA_TARGET,            ICTARGET_IDCMP,
      FRM_EdgesOnly,         TRUE,

      TAG_PREFID,            BGUI_BUTTON_GADGET,
      FRM_Type,              FRTYPE_BUTTON,

      TAG_PREFID,            BGUI_CHECKBOX_GADGET,
      LAB_NoPlaceIn,         TRUE,
      BT_NoRecessed,         TRUE,
      BUTTON_ScaleMinWidth,  14,
      BUTTON_ScaleMinHeight, 8,

      TAG_PREFID,            BGUI_CYCLE_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_Type,              FRTYPE_BUTTON,

      TAG_PREFID,            BGUI_GROUP_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_DefaultType,       FRTYPE_NEXT,
      GROUP_DefHSpaceNarrow, 2,
      GROUP_DefHSpaceNormal, 4,
      GROUP_DefHSpaceWide,   8,
      GROUP_DefVSpaceNarrow, 2,
      GROUP_DefVSpaceNormal, 4,
      GROUP_DefVSpaceWide,   8,

      TAG_PREFID,            BGUI_INDICATOR_GADGET,
      BT_Buffer,             TRUE,
      FRM_DefaultType,       FRTYPE_BUTTON,
      FRM_Recessed,          TRUE,

      TAG_PREFID,            BGUI_INFO_GADGET,
      LAB_NoPlaceIn,         TRUE,
      BT_Buffer,             TRUE,
      FRM_DefaultType,       FRTYPE_BUTTON,
      FRM_Recessed,          TRUE,

      TAG_PREFID,            BGUI_LISTVIEW_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_Type,              FRTYPE_BUTTON,

      TAG_PREFID,            BGUI_MX_GADGET,
      LAB_NoPlaceIn,         TRUE,
      LAB_Place,             PLACE_ABOVE,
      MX_Spacing,            GRSPACE_NARROW,
      MX_LabelPlace,         PLACE_RIGHT,

      TAG_PREFID,            BGUI_PAGE_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_Type,              FRTYPE_NONE,
      BT_Buffer,             TRUE,

      TAG_PREFID,            BGUI_PROGRESS_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_Type,              FRTYPE_BUTTON,
      FRM_Recessed,          TRUE,
      BT_LeftOffset,         1,
      BT_RightOffset,        1,
      BT_TopOffset,          1,
      BT_BottomOffset,       1,

      TAG_PREFID,            BGUI_PROP_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_Type,              FRTYPE_BUTTON,

      TAG_PREFID,            BGUI_RADIOBUTTON_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_Type,              FRTYPE_RADIOBUTTON,
      BUTTON_ScaleMinWidth,  8,
      BUTTON_ScaleMinHeight, 8,
      BUTTON_SelectOnly,     TRUE,

      TAG_PREFID,            BGUI_SLIDER_GADGET,
      PGA_Slider,            TRUE,

      TAG_PREFID,            BGUI_STRING_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_Type,              FRTYPE_RIDGE,
      BT_LeftOffset,         1,
      BT_RightOffset,        1,
      BT_TopOffset,          1,
      BT_BottomOffset,       1,

      TAG_PREFID,            BGUI_VIEW_GADGET,
      LAB_NoPlaceIn,         TRUE,
      FRM_Type,              FRTYPE_BUTTON,
      FRM_Recessed,          TRUE,
      BT_LeftOffset,         1,
      BT_RightOffset,        1,
      BT_TopOffset,          1,
      BT_BottomOffset,       1,

      TAG_PREFID,            BGUI_WINDOW_OBJECT,
      WINDOW_ToolTicks,      10,
      WINDOW_PreBufferRP,    FALSE,

      TAG_PREFID,            BGUI_FILEREQ_OBJECT,
      ASLREQ_Type,           ASL_FileRequest,

      TAG_PREFID,            BGUI_FONTREQ_OBJECT,
      ASLREQ_Type,           ASL_FontRequest,

      TAG_PREFID,            BGUI_SCREENREQ_OBJECT,
      ASLREQ_Type,           ASL_ScreenModeRequest,

      TAG_DONE
   };

   ObtainSemaphore(&TaskLock);
   if (tm = FindTaskMember())
   {
      t1 = deftags;
      do
      {
         id = t1->ti_Data;
         t2 = ++t1;
         while (t2->ti_Tag & TAG_USER) t2++;

         type = t2->ti_Tag;
         t2->ti_Tag = TAG_DONE;

         NewPrefInfo(tm, id, t1);

         t2->ti_Tag = type;
         t1 = t2;
      }  while (type == TAG_PREFID);
   };
   ReleaseSemaphore(&TaskLock);
}

static void LoadPrefs(UBYTE *name)
{
   struct IFFHandle        *iff;
   struct CollectionItem   *ci;
   ULONG                   *data, type;
   TM                      *tm;

   if (tm = FindTaskMember())
   {
      if (iff = AllocIFF())
      {
         if (iff->iff_Stream = Open(name, MODE_OLDFILE))
         {
            InitIFFasDOS(iff);
            if (OpenIFF(iff, IFFF_READ) == 0)
            {
               CollectionChunk(iff, ID_BGUI, ID_DTAG);
               StopOnExit(iff, ID_BGUI, ID_FORM);
               ParseIFF(iff, IFFPARSE_SCAN);
               
               ci = FindCollection(iff, ID_BGUI, ID_DTAG);
               while (ci)
               {
                  data = ci->ci_Data;
                  type = *data++;
                  NewPrefInfo(tm, type, (struct TagItem *)data);

                  ci = ci->ci_Next;
               };
            };
            Close(iff->iff_Stream);
         };
         FreeIFF(iff);
      };
   };
}

makeproto struct TagItem *DefTagList(ULONG id, struct TagItem *tags)
{
   struct TagItem *deftags, *newtags, *t, *tag, *tstate1, *tstate2;
   int             fr_deftype = 0;

   int n1, n2;

   if (deftags = BGUI_GetDefaultTags(id))
   {
      tstate1 = deftags, tstate2 = tags;

      n1 = BGUI_CountTagItems(deftags);
      n2 = BGUI_CountTagItems(tags);

      if (newtags = AllocVec((n1 + n2 + 1) * sizeof(struct TagItem), 0))
      {
         t = newtags;

         while (tag = NextTagItem(&tstate1))
         {
            if (FindTagItem(tag->ti_Tag, tags))
               continue;

            if (tag->ti_Tag == FRM_DefaultType)
            {
               fr_deftype = tag->ti_Data;
               continue;
            };
            *t++ = *tag;
         };

         while (tag = NextTagItem(&tstate2))
         {
            if (fr_deftype && (tag->ti_Tag == FRM_Type) && (tag->ti_Data == FRTYPE_DEFAULT))
            {
               tag->ti_Data = fr_deftype;
            };
            *t++ = *tag;
         };

         t->ti_Tag = TAG_DONE;

         tags = CloneTagItems(newtags);

         FreeVec(newtags);

         return tags;
      };
   };
   return CloneTagItems(tags);
}

makeproto SAVEDS ASM ULONG BGUI_CountTagItems(REG(a0) struct TagItem *tags)
{
   struct TagItem *tstate = tags;
   ULONG n = 0;

   while (NextTagItem(&tstate)) n++;

   return n;
}

makeproto SAVEDS ASM struct TagItem *BGUI_MergeTagItems(REG(a0) struct TagItem *tags1, REG(a1) struct TagItem *tags2)
{
   struct TagItem *tags, *t, *tag, *tstate1 = tags1, *tstate2 = tags2;

   int n1 = BGUI_CountTagItems(tags1);
   int n2 = BGUI_CountTagItems(tags2);

   if (tags = AllocVec((n1 + n2 + 1) * sizeof(struct TagItem), 0))
   {
      t = tags;

      while (tag = NextTagItem(&tstate1)) *t++ = *tag;
      while (tag = NextTagItem(&tstate2)) *t++ = *tag;
      t->ti_Tag = TAG_DONE;

      t = CloneTagItems(tags);

      FreeVec(tags);

      return t;
   };
   return NULL;
}

makeproto SAVEDS ASM struct TagItem *BGUI_CleanTagItems(REG(a0) struct TagItem *tags, REG(d0) LONG dir)
{
   struct TagItem *tstate = tags, *tag, *tag2;

   if (dir)
   {
      while (tag = NextTagItem(&tstate))
      {
         while (tag2 = FindTagItem(tag->ti_Tag, tag + 1))
         {
            if (dir > 0)
            {
               tag->ti_Tag = TAG_IGNORE;
               break;
            }
            else if (dir < 0)
            {
               tag2->ti_Tag = TAG_IGNORE;
            };
         };
      };
   };
   tag = CloneTagItems(tags);
   FreeTagItems(tags);
   return tag;
}

makeproto SAVEDS ASM struct TagItem *BGUI_GetDefaultTags(REG(d0) ULONG id)
{
   PI             *pi;
   TM             *tm;
   struct TagItem *tags = NULL;

   /*
    * Lock the list.
    */
   ObtainSemaphore(&TaskLock);

   if (tm = FindTaskMember())
   {
      if (pi = FindPrefInfo(tm, id))
         tags = pi->pi_Defaults;
   };

   /*
    * Release the lock.
    */
   ReleaseSemaphore(&TaskLock);

   return tags;
}

makeproto SAVEDS ASM VOID BGUI_DefaultPrefs(VOID)
{
   PI        *pi;
   TM        *tm;

   ObtainSemaphore(&TaskLock);
   if (tm = FindTaskMember())
   {
      while (pi = (PI *)RemHead((struct List *)&tm->tm_Prefs))
         FreeTL_PI(pi);
   };
   ReleaseSemaphore(&TaskLock);

   DefPrefs();
}

makeproto SAVEDS ASM VOID BGUI_LoadPrefs(REG(a0) UBYTE *name)
{
   BGUI_DefaultPrefs();
   LoadPrefs(name);
}
