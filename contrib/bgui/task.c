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
 * Revision 42.13  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.12  2003/01/18 19:10:01  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.11  2001/01/28 04:53:21  bergers
 * Fixed some compiler complaints (some casts were missing.).
 *
 * Revision 42.10  2000/08/09 14:54:19  stegerg
 * fixed a bug which was introduced by one of us AROS guys and which caused
 * enforcer hits on Amiga.
 *
 * Revision 42.9  2000/08/09 11:45:57  chodorowski
 * Removed a lot of #ifdefs that disabled the AROS_LIB* macros when not building on AROS. This is now handled in contrib/bgui/include/bgui_compilerspecific.h.
 *
 * Revision 42.8  2000/08/08 18:16:12  stegerg
 * bug fix in BGUI_FreePoolMem/FreePoolMemDebug.
 *
 * Revision 42.7  2000/08/08 14:17:30  chodorowski
 * Fixes to compile on Amiga. There was some wrongly nested #ifdefs here,
 * around the BGUI_FreePooled and BGUI_FreePooledDebug functions.
 * Now it compiles, but I'm a bit dubiuos if it works as it should.
 * Could someone have a look please?
 *
 * Revision 42.6  2000/07/06 16:44:03  stegerg
 * AddTaskMember can now be called. Problem was Cli()->cli_CommandName
 * which BGUI expected to be a BSTR with size in first byte.
 *
 * Revision 42.5  2000/06/01 01:41:37  bergers
 * Only 2 linker problems left: stch_l & stcu_d. Somebody might want to replace them (embraced by #ifdef __AROS__), please.
 *
 * Revision 42.4  2000/05/31 01:23:10  bergers
 * Changes to make BGUI compilable and linkable.
 *
 * Revision 42.3  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.2  2000/05/15 19:27:02  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:48  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:10:28  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:18  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.10  1998/12/07 14:47:28  mlemos
 * Made font tracking code use a copy of the TextFont structure to allow the
 * opened fonts be properly tracked.
 *
 * Revision 41.10.2.9  1998/12/07 04:30:04  mlemos
 * Made the code that lists all unclosed fonts to list all probable source
 * locations where the fonts were opened.
 *
 * Revision 41.10.2.8  1998/12/07 03:04:37  mlemos
 * Added conditional debug code to track fonts opened by BGUI classes.
 *
 * Revision 41.10.2.7  1998/06/18 22:06:36  mlemos
 * Made BGUI fetch the preferences from the appropriate file when running from
 * CLI.
 *
 * Revision 41.10.2.6  1998/03/02 03:14:39  mlemos
 * Corrected the display of the memory leak address.
 *
 * Revision 41.10.2.5  1998/03/01 23:09:19  mlemos
 * Added code to trash memory allocations before being freed.
 * Added code to warn when a NULL pointer is passed to memory release.
 *
 * Revision 41.10.2.4  1998/03/01 18:42:45  mlemos
 * Corrected memory wall address verifications.
 *
 * Revision 41.10.2.3  1998/03/01 18:25:00  mlemos
 * Added support to track memory overwriting outside the allocated space.
 *
 * Revision 41.10.2.2  1998/03/01 04:25:23  mlemos
 * Added support to track memory freeing mismatches and memory leaks.
 *
 * Revision 41.10.2.1  1998/03/01 02:23:56  mlemos
 * Added new memory allocation debugging functions to the library
 *
 * Revision 41.10  1998/02/25 21:13:21  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:56  mlemos
 * Ian sources
 *
 *
 */

#define NO_MEMORY_ALLOCATION_DEBUG_ALIASING

#include "include/classdefs.h"

/*
 * Global data.
 */

makeproto TL                     TaskList;   /* List of openers.         */
makeproto struct SignalSemaphore TaskLock;   /* For exclusive task-list access. */

static APTR MemPool = NULL;

static void LoadPrefs(UBYTE *name);
static void DefPrefs(void);

#ifdef DEBUG_BGUI

static struct BlockHeader
{
	struct BlockHeader *NextBlock;
	ULONG BlockSize;
	APTR Address;
	ULONG Size;
	APTR PoolHeader;
	STRPTR File;
	ULONG Line;
}
*MemoryBlocks=NULL;

union TypesUnion
{
	long int LongInteger;
	long double LongDouble;
	void *DataPointer;
	void (*FunctionPointer)(void);
};

#define AlignMemory(offset) (((offset)+sizeof(union TypesUnion)-1)/sizeof(union TypesUnion))*sizeof(union TypesUnion)

#define MEMORY_WALL_SIZE 8

#define MEMORY_WALL_TRASH 'W'
#define MEMORY_FREE_TRASH 'Y'

#define BLOCK_HEADER_OFFSET AlignMemory(sizeof(struct BlockHeader)+MEMORY_WALL_SIZE)

#define TrashMemory(memory,size,character) memset(memory,character,size)

//static ASM VOID FreeVecMemDebug(REG(a0) APTR pool, REG(a1) APTR memptr, REG(a2) STRPTR file, REG(d0) ULONG line);
static ASM REGFUNCPROTO4(VOID, FreeVecMemDebug,
	REGPARAM(A0, APTR, pool),
	REGPARAM(A1, APTR, memptr),
	REGPARAM(A2, STRPTR, file),
	REGPARAM(D0, ULONG, line));

//static ASM APTR AllocVecMemDebug(REG(a0) APTR mempool, REG(d0) ULONG size,REG(a1) STRPTR file, REG(d1) ULONG line);
static ASM REGFUNCPROTO4(APTR, AllocVecMemDebug,
	REGPARAM(A0, APTR, mempool),
	REGPARAM(D0, ULONG, size),
	REGPARAM(A1, STRPTR, file),
	REGPARAM(D1, ULONG, line));

#define AllocVecMem(mempool,size) AllocVecMemDebug(mempool,size,__FILE__,__LINE__)
#define FreeVecMem(pool,memptr) FreeVecMemDebug(pool,memptr,__FILE__,__LINE__)

static BOOL VerifyMemoryWall(char *memory)
{
	ULONG size;

	for(size=MEMORY_WALL_SIZE;size;size--,memory++)
	{
		if(*memory!=MEMORY_WALL_TRASH)
			return(FALSE);
	}
	return(TRUE);
}

#else
//static ASM APTR AllocVecMem(REG(a0) APTR mempool, REG(d0) ULONG size);
static ASM REGFUNCPROTO2(APTR, AllocVecMem,
	REGPARAM(A0, APTR, mempool),
	REGPARAM(D0, ULONG, size));

//static ASM VOID FreeVecMem(REG(a0) APTR pool, REG(a1) APTR memptr);
static ASM REGFUNCPROTO2(VOID, FreeVecMem,
	REGPARAM(A0, APTR, pool),
	REGPARAM(A1, APTR, memptr));
#endif

//static SAVEDS ASM APTR BGUI_CreatePool(REG(d0) ULONG memFlags, REG(d1) ULONG puddleSize, REG(d2) ULONG threshSize);
static SAVEDS ASM REGFUNCPROTO3(APTR, BGUI_CreatePool,
	REGPARAM(D0, ULONG, memFlags),
	REGPARAM(D1, ULONG, puddleSize),
	REGPARAM(D2, ULONG, threshSize));


//static SAVEDS ASM VOID BGUI_DeletePool(REG(a0) APTR poolHeader);
static SAVEDS ASM REGFUNCPROTO1(VOID, BGUI_DeletePool,
	REGPARAM(A0, APTR, poolHeader));

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

#ifdef DEBUG_BGUI

static struct FontEntry
{
	struct FontEntry *NextFont;
	struct TextFont *Font;
	BOOL Closed;
	STRPTR File;
	ULONG Line;
	struct TextFont Copy;
}
*FontEntries=NULL;

static void BGUI_CloseAllFonts(void)
{
	struct FontEntry *font_entry;

	for(font_entry=FontEntries;font_entry;font_entry=font_entry->NextFont)
	{
		if(font_entry->Closed==FALSE)
		{
	    D(bug("***Font leak (%lx) \"%s\" %s,%lu:\n",font_entry->Font,font_entry->Font->tf_Message.mn_Node.ln_Name ? font_entry->Font->tf_Message.mn_Node.ln_Name : "Unnamed font",font_entry->File ? font_entry->File : (STRPTR)"Unknown source",font_entry->Line));
			CloseFont(font_entry->Font);
			font_entry->Closed=TRUE;
		}
	}
	while((font_entry=FontEntries))
	{
		FontEntries=font_entry->NextFont;
		BGUI_FreePoolMemDebug(font_entry,__FILE__,__LINE__);
	}
}

makeproto struct TextFont *BGUI_OpenFontDebug(struct TextAttr *textAttr,STRPTR file,ULONG line)
{
	struct TextFont *font;

	if((font=OpenFont(textAttr)))
	{
		struct FontEntry *font_entry;

		if((font_entry=BGUI_AllocPoolMemDebug(sizeof(*font_entry),__FILE__,__LINE__)))
		{
			font_entry->Font=font;
			font_entry->Closed=FALSE;
			font_entry->File=file;
			font_entry->Line=line;
			font_entry->Copy= *font;
			ObtainSemaphore(&TaskLock);
			font_entry->NextFont=FontEntries;
			FontEntries=font_entry;
			ReleaseSemaphore(&TaskLock);
			font= &font_entry->Copy;
		}
		else
		{
			CloseFont(font);
			font=NULL;
		}
	}
	return(font);
}

makeproto void BGUI_CloseFontDebug(struct TextFont *font,STRPTR file,ULONG line)
{
	struct FontEntry **font_entry,*free_font_entry;

	if(font==NULL)
	{
		D(bug("***Attempt to close an NULL font (%s,%lu)\n",file ? file : (STRPTR)"Unknown source",line));
		return;
	}
	ObtainSemaphore(&TaskLock);
	for(font_entry=&FontEntries;*font_entry && (&((*font_entry)->Copy)!=font || (*font_entry)->Closed);font_entry=&(*font_entry)->NextFont);
	if((free_font_entry=*font_entry)
	&& &free_font_entry->Copy==font)
	{
		CloseFont(free_font_entry->Font);
		free_font_entry->Closed=TRUE;
	}
	else
		D(bug("***Attempt to close an unknown font (%lx) \"%s\" (%s,%lu)\n",font,font->tf_Message.mn_Node.ln_Name ? font->tf_Message.mn_Node.ln_Name : "Unnamed font",file ? file : (STRPTR)"Unknown source",line));
	ReleaseSemaphore(&TaskLock);
}

#else
#define BGUI_CloseAllFonts() 
#endif

/*
 * Free task list.
 */
makeproto void FreeTaskList(void)
{
   BGUI_CloseAllFonts();
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
      if(Cli())
      {
	 STRPTR command_name,insert;
	 size_t offset;
	 UWORD command_len;
	 
	 command_name=BADDR(Cli()->cli_CommandName);
	 strcpy(buffer,"ENV:BGUI/");
	 insert=buffer+sizeof("ENV:BGUI/")-1;
	 #ifdef __AROS__
	 strcpy(insert,command_name);
	 command_len = strlen(command_name);
	 #else
	 memcpy(insert,command_name+1,*command_name);
	 command_len = *command_name;
	 *(insert+ command_len)='\0';
	 #endif
	 if((offset=FilePart(insert)-insert)!=0)
	   memcpy(insert,command_name+1+offset,command_len-offset);
	 insert+= command_len-offset;
	 strcpy(insert,".prefs");
      }
      else
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
//extern ASM APTR AsmCreatePool  ( REG(d0) ULONG, REG(d1) ULONG, REG(d2) ULONG, REG(a6) struct ExecBase * );
extern ASM REGFUNCPROTO4(APTR, AsmCreatePool,
	REGPARAM(D0, ULONG, a),
	REGPARAM(D1, ULONG, b),
	REGPARAM(D2, ULONG, c),
	REGPARAM(A6, struct ExecBase *, sysbase));


//extern ASM APTR AsmAllocPooled ( REG(a0) APTR,  REG(d0) ULONG,                REG(a6) struct ExecBase * );
extern ASM REGFUNCPROTO3(APTR, AsmAllocPooled,
	REGPARAM(A0, APTR, pool),
	REGPARAM(D0, ULONG, size),
	REGPARAM(A6, struct ExecBase *, sysbase));


//extern ASM APTR AsmFreePooled  ( REG(a0) APTR,  REG(a1) APTR,  REG(d0) ULONG, REG(a6) struct ExecBase * );
extern ASM REGFUNCPROTO4(APTR, AsmFreePooled,
	REGPARAM(A0, APTR, pool),
	REGPARAM(A1, APTR, mem),
	REGPARAM(D0, ULONG, size),
	REGPARAM(A6, struct ExecBase *, sysbase));


//extern ASM APTR AsmDeletePool  ( REG(a0) APTR,                                REG(a6) struct ExecBase * );
extern ASM REGFUNCPROTO2(APTR, AsmDeletePool,
	REGPARAM(A0, APTR, pool),
	REGPARAM(A6, struct ExecBase *, sysbase));

/*
 * Create a memory pool.
 */
//static SAVEDS ASM APTR BGUI_CreatePool(REG(d0) ULONG memFlags, REG(d1) ULONG puddleSize, REG(d2) ULONG threshSize)
static SAVEDS ASM REGFUNC3(APTR, BGUI_CreatePool,
	REGPARAM(D0, ULONG, memFlags),
	REGPARAM(D1, ULONG, puddleSize),
	REGPARAM(D2, ULONG, threshSize))
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
REGFUNC_END

/*
 * Delete a memory pool.
 */
//static SAVEDS ASM VOID BGUI_DeletePool(REG(a0) APTR poolHeader)
static SAVEDS ASM REGFUNC1(VOID, BGUI_DeletePool,
	REGPARAM(A0, APTR, poolHeader))
{
#ifdef DEBUG_BGUI
   struct BlockHeader **header;

   for(header= &MemoryBlocks;*header;)
   {
      if((*header)->PoolHeader==poolHeader)
      {
	 D(bug("***Memory leak (%lx) (%s,%lu)\n",(*header)->Address,(*header)->File ? (*header)->File : (STRPTR)"Unknown file", (*header)->Line));
	 *header=(*header)->NextBlock;
      }
      else
	 header= &(*header)->NextBlock;
   }
#endif
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
REGFUNC_END

/*
 * Allocate pooled memory.
 */

#ifdef DEBUG_BGUI
//static SAVEDS ASM APTR BGUI_AllocPooledDebug(REG(a0) APTR poolHeader, REG(d0) ULONG memSize,REG(a1) STRPTR file,REG(d1) ULONG line)
static SAVEDS ASM REGFUNC4(APTR, BGUI_AllocPooledDebug,
	REGPARAM(A0, APTR, poolHeader),
	REGPARAM(D0, ULONG, memSize),
	REGPARAM(A1, STRPTR, file),
	REGPARAM(D1, ULONG, line))
#else
//static SAVEDS ASM APTR BGUI_AllocPooled(REG(a0) APTR poolHeader, REG(d0) ULONG memSize)
static SAVEDS ASM REGFUNC2(APTR, BGUI_AllocPooled,
	REGPARAM(A0, APTR, poolHeader),
	REGPARAM(D0, ULONG, memSize))
#endif
{
   APTR memory;
#ifdef DEBUG_BGUI
   ULONG size;

   size=memSize;
   memSize=AlignMemory(memSize+BLOCK_HEADER_OFFSET+MEMORY_WALL_SIZE);
#endif
   #ifdef ENHANCED
   /*
    * V39 Exec function.
    */
   memory=AllocPooled(poolHeader, memSize);

   #else
   /*
    * pools.lib function
    */
   memory=AsmAllocPooled(poolHeader, memSize, SysBase);

   #endif

#ifdef DEBUG_BGUI
   if(memory)
   {
		struct BlockHeader *header=memory;

		memory=((char *)memory)+BLOCK_HEADER_OFFSET;
		header->NextBlock=MemoryBlocks;
		header->Address=memory;
		header->Size=size;
		header->PoolHeader=poolHeader;
		header->File=file;
		header->Line=line;
		MemoryBlocks=header;
		TrashMemory(((char *)memory)-MEMORY_WALL_SIZE,MEMORY_WALL_SIZE,MEMORY_WALL_TRASH);
		TrashMemory(((char *)memory)+size,MEMORY_WALL_SIZE,MEMORY_WALL_TRASH);
   }
#endif
   return(memory);
}
REGFUNC_END

/*
 * Free pooled memory.
 */
#ifdef DEBUG_BGUI
//static SAVEDS ASM VOID BGUI_FreePooledDebug(REG(a0) APTR poolHeader, REG(a1) APTR memory, REG(d1) ULONG memSize,REG(a2) STRPTR file,REG(d2) ULONG line)
static SAVEDS ASM REGFUNC5(VOID, BGUI_FreePooledDebug,
	REGPARAM(A0, APTR, poolHeader),
	REGPARAM(A1, APTR, memory),
	REGPARAM(D1, ULONG, memSize),
	REGPARAM(A2, STRPTR, file),
	REGPARAM(D2, ULONG, line))
#else
//static SAVEDS ASM VOID BGUI_FreePooled(REG(a0) APTR poolHeader, REG(a1) APTR memory, REG(d1) ULONG memSize)
static SAVEDS ASM REGFUNC3(VOID, BGUI_FreePooled,
	REGPARAM(A0, APTR, poolHeader),
	REGPARAM(A1, APTR, memory),
	REGPARAM(D1, ULONG, memSize))
#endif
{
#ifdef DEBUG_BGUI
   struct BlockHeader **header;

   for(header= &MemoryBlocks;*header;header= &(*header)->NextBlock)
   {
      if((*header)->Address==memory)
	break;
   }
   if(*header==NULL)
   {
      D(bug("***Attempt to free an unknown memory block (%s,%lu)\n",file ? file : (STRPTR)"Unknown file", line));
      return;
   }
   else
   {
	  if((*header)->PoolHeader!=poolHeader)
      {
	 D(bug("***Attempt to free a memory block from a wrong memory pool (%lx) (%s,%lu)\n",poolHeader,file ? file : (STRPTR)"Unknown file", line));
	 D(bug("***The original memory pool was (%lx) (%s,%lu)\n",(*header)->PoolHeader,(*header)->File ? (*header)->File : (STRPTR)"Unknown file", (*header)->Line));
      }
      if(!VerifyMemoryWall(((char *)(*header)->Address)-MEMORY_WALL_SIZE))
      {
	 D(bug("***Lower memory wall violation (%s,%lu)\n",file ? file : (STRPTR)"Unknown file", line));
	 D(bug("***Memory originally allocated from (%s,%lu)\n",(*header)->File ? (*header)->File : (STRPTR)"Unknown file", (*header)->Line));
      }
      if(!VerifyMemoryWall(((char *)(*header)->Address)+(*header)->Size))
      {
	 D(bug("***Upper memory wall violation (%s,%lu)\n",file ? file : (STRPTR)"Unknown file", line));
	 D(bug("***Memory originally allocated from (%s,%lu)\n",(*header)->File ? (*header)->File : (STRPTR)"Unknown file", (*header)->Line));
      }
      {
	 ULONG block_size;

	 block_size=(*header)->BlockSize;
	 memory= *header;
	 *header=(*header)->NextBlock;
	 TrashMemory(memory,block_size,MEMORY_FREE_TRASH);
      }
   }
#endif
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
REGFUNC_END

/*
 * Allocate memory.
 */
#ifdef DEBUG_BGUI
//static ASM APTR AllocVecMemDebug(REG(a0) APTR mempool, REG(d0) ULONG size,REG(a1) STRPTR file, REG(d1) ULONG line)
static ASM REGFUNC4(APTR, AllocVecMemDebug,
	REGPARAM(A0, APTR, mempool),
	REGPARAM(D0, ULONG, size),
	REGPARAM(A1, STRPTR, file),
	REGPARAM(D1, ULONG, line))
#else
//static ASM APTR AllocVecMem(REG(a0) APTR mempool, REG(d0) ULONG size)
static ASM REGFUNC2(APTR, AllocVecMem,
	REGPARAM(A0, APTR, mempool),
	REGPARAM(D0, ULONG, size))
#endif
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
#ifdef DEBUG_BGUI
      if (mem = (ULONG *)BGUI_AllocPooledDebug(mempool, size,file,line))
#else
      if (mem = (ULONG *)BGUI_AllocPooled(mempool, size))
#endif
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
REGFUNC_END

/*
 * Free memory.
 */
#ifdef DEBUG_BGUI
//static ASM VOID FreeVecMemDebug(REG(a0) APTR pool, REG(a1) APTR memptr, REG(a2) STRPTR file, REG(d0) ULONG line)
static ASM REGFUNC4(VOID, FreeVecMemDebug,
	REGPARAM(A0, APTR, pool),
	REGPARAM(A1, APTR, memptr),
	REGPARAM(A2, STRPTR, file),
	REGPARAM(D0, ULONG, line))
#else
//static ASM VOID FreeVecMem(REG(a0) APTR pool, REG(a1) APTR memptr)
static ASM REGFUNC2(VOID, FreeVecMem,
	REGPARAM(A0, APTR, pool),
	REGPARAM(A1, APTR, memptr))
#endif
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
#ifdef DEBUG_BGUI
      BGUI_FreePooledDebug(pool, (APTR)mem, *mem,file,line);
#else
      BGUI_FreePooled(pool, (APTR)mem, *mem);
#endif
   } else
      /*
       * Normal system de-allocation.
       */
      FreeVec(mem);
}
REGFUNC_END

/*
 * Allocate memory from the pool.
 */
#ifdef DEBUG_BGUI
   #ifdef __AROS__
   AROS_LH1(APTR, BGUI_AllocPoolMem,
       AROS_LHA(ULONG, size, D0),
       struct Library *, BGUIBase, 12, BGUI)
   #else
   SAVEDS ASM APTR BGUI_AllocPoolMem(REG(d0) ULONG size)
   #endif
   {
      AROS_LIBFUNC_INIT
      AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

     D(bug("BGUI_AllocPoolMem is being called from unknown location\n"));
	   return(BGUI_AllocPoolMemDebug(size,__FILE__,__LINE__));

      AROS_LIBFUNC_EXIT
   }

   #ifdef __AROS__
   makearosproto
   AROS_LH3(APTR, BGUI_AllocPoolMemDebug,
       AROS_LHA(ULONG, size, D0),
       AROS_LHA(STRPTR, file, A0),
       AROS_LHA(ULONG, line, D1),
       struct Library *BGUIBase, 31, BGUI)
   #else
   makeproto SAVEDS ASM APTR BGUI_AllocPoolMemDebug(REG(d0) ULONG size, REG(a0) STRPTR file, REG(d1) ULONG line)
   #endif

#else

   #ifdef __AROS__
   makearosproto
   AROS_LH3(APTR, BGUI_AllocPoolMemDebug,
       AROS_LHA(ULONG, size, D0),
       AROS_LHA(STRPTR, file, A0),
       AROS_LHA(ULONG, line, D1),
       struct Library *, BGUIBase, 31, BGUI)
   #else
   SAVEDS ASM APTR BGUI_AllocPoolMemDebug(REG(d0) ULONG size, REG(a0) STRPTR file, REG(d1) ULONG line)
   #endif
   {
      AROS_LIBFUNC_INIT
      AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

     bug("BGUI_AllocPoolMemDebug is being called from (%s,%lu)\n",file ? file : (STRPTR)"unknown",line);
	   return(BGUI_AllocPoolMem(size));

      AROS_LIBFUNC_EXIT
   }

   #ifdef __AROS__
   makearosproto
   AROS_LH1(APTR, BGUI_AllocPoolMem,
       AROS_LHA(ULONG, size, D0),
       struct Library *, BGUIBase, 12, BGUI)
   #else
   makeproto SAVEDS ASM APTR BGUI_AllocPoolMem(REG(d0) ULONG size)
   #endif
#endif

{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   APTR        memPtr;

   /*
    * Lock the list.
    */
   ObtainSemaphore(&TaskLock);

   /*
    * Allocate memory.
    */
#ifdef DEBUG_BGUI
   memPtr = AllocVecMemDebug(MemPool, size, file, line);
#else
   memPtr = AllocVecMem(MemPool, size);
#endif

   /*
    * Unlock the list.
    */
   ReleaseSemaphore(&TaskLock);

   return memPtr;

   AROS_LIBFUNC_EXIT
}

/*
 * Free memory from the pool.
 */
#ifdef DEBUG_BGUI
   #ifdef __AROS__
   AROS_LH1(VOID, BGUI_FreePoolMem,
       AROS_LHA(APTR, memPtr, A0),
       struct Library *, BGUIBase, 13, BGUI)
   #else
   SAVEDS ASM VOID BGUI_FreePoolMem(REG(a0) APTR memPtr)
   #endif
   {
      AROS_LIBFUNC_INIT
      AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

      D(bug("BGUI_FreePoolMem is being called from unknown location\n"));
	   BGUI_FreePoolMemDebug(memPtr,__FILE__,__LINE__);

      AROS_LIBFUNC_EXIT
   }

   #ifdef __AROS__
   makearosproto
   AROS_LH3(VOID, BGUI_FreePoolMemDebug,
       AROS_LHA(APTR, memPtr, A0),
       AROS_LHA(STRPTR, file, A1),
       AROS_LHA(ULONG, line, D0),
       struct Library *, BGUIBase, 32, BGUI)
   #else
   makeproto SAVEDS ASM VOID BGUI_FreePoolMemDebug(REG(a0) APTR memPtr, REG(a1) STRPTR file, REG(d0) ULONG line)
   #endif
    
#else

   #ifdef __AROS__
   makearosproto
   AROS_LH3(VOID, BGUI_FreePoolMemDebug,
       AROS_LHA(APTR, memPtr, A0),
       AROS_LHA(STRPTR, file, A1),
       AROS_LHA(ULONG, line, D0),
       struct Library *, BGUIBase, 32, BGUI)
   #else
   makeproto SAVEDS ASM VOID BGUI_FreePoolMemDebug(REG(a0) APTR memPtr, REG(a1) STRPTR file, REG(d0) ULONG line)
   #endif
   {
      AROS_LIBFUNC_INIT
      AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

      bug("BGUI_FreePoolMemDebug is being called from (%s,%lu)\n",file ? file : (STRPTR)"unknown",line);
	   BGUI_FreePoolMem(memPtr);

      AROS_LIBFUNC_EXIT
   }

   #ifdef __AROS__
   makearosproto
   AROS_LH1(VOID, BGUI_FreePoolMem,
       AROS_LHA(APTR, memPtr, A0),
       struct Library *, BGUIBase, 13, BGUI)
   #else
   makeproto SAVEDS ASM VOID BGUI_FreePoolMem(REG(a0) APTR memPtr)
   #endif

#endif

{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   if (memPtr)
   {
      /*
       * Lock the list.
       */
      ObtainSemaphore(&TaskLock);

      /*
       * Free the memory.
       */
#ifdef DEBUG_BGUI
      FreeVecMemDebug(MemPool, memPtr, file, line);
#else
      FreeVecMem(MemPool, memPtr);
#endif

      /*
       * Unlock the list.
       */
      ReleaseSemaphore(&TaskLock);
   }
#ifdef DEBUG_BGUI
   else
      D(bug("*** Attempt to free memory with a NULL pointer\n"));
#endif

   AROS_LIBFUNC_EXIT
}

/*
 *  Add an ID to the list.
 *
 *  Changes made by T.Herold
 *
 *  - Added reg(a1) Task param
 *  - Uses InternalFindTaskMember to find destination task.
 */

//makeproto ASM BOOL AddIDReport(REG(a0) struct Window *window, REG(d0) ULONG id, REG(a1) struct Task *task)
makeproto ASM REGFUNC3(BOOL, AddIDReport,
	REGPARAM(A0, struct Window *, window),
	REGPARAM(D0, ULONG, id),
	REGPARAM(A1, struct Task *, task))
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
REGFUNC_END

/*
 * Get next ID for the window.
 */
//makeproto ASM ULONG GetIDReport(REG(a0) struct Window *window)
makeproto ASM REGFUNC1(ULONG, GetIDReport,
	REGPARAM(A0, struct Window *, window))
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
REGFUNC_END

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
//makeproto ASM VOID RemoveIDReport(REG(a0) struct Window *window)
makeproto ASM REGFUNC1(VOID, RemoveIDReport,
	REGPARAM(A0, struct Window *, window))
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
REGFUNC_END

/*
 * Add an open window to the list.
 */
//makeproto ASM VOID AddWindow(REG(a0) Object *wo, REG(a1) struct Window *win)
makeproto ASM REGFUNC2(VOID, AddWindow,
	REGPARAM(A0, Object *, wo),
	REGPARAM(A1, struct Window *, win))
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
REGFUNC_END

/*
 * Remove a window from the list.
 */
//makeproto ASM VOID RemWindow(REG(a0) Object *wo)
makeproto ASM REGFUNC1(VOID, RemWindow,
	REGPARAM(A0, Object *, wo))
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
REGFUNC_END

/*
 * Find the window located under the mouse.
 */
makeproto ASM REGFUNC1(Object *, WhichWindow,
	REGPARAM(A0, struct Screen *, screen))
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
REGFUNC_END

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
	 if (iff->iff_Stream = Open((char *)name, MODE_OLDFILE))
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
	    Close((BPTR)iff->iff_Stream);
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

//makeproto SAVEDS ASM ULONG BGUI_CountTagItems(REG(a0) struct TagItem *tags)
makeproto SAVEDS ASM REGFUNC1(ULONG, BGUI_CountTagItems,
	REGPARAM(A0, struct TagItem *, tags))
{
   struct TagItem *tstate = tags;
   ULONG n = 0;

   while (NextTagItem(&tstate)) n++;

   return n;
}
REGFUNC_END

//makeproto SAVEDS ASM struct TagItem *BGUI_MergeTagItems(REG(a0) struct TagItem *tags1, REG(a1) struct TagItem *tags2)
makeproto SAVEDS ASM REGFUNC2(struct TagItem *, BGUI_MergeTagItems,
	REGPARAM(A0, struct TagItem *, tags1),
	REGPARAM(A1, struct TagItem *, tags2))
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
REGFUNC_END

//makeproto SAVEDS ASM struct TagItem *BGUI_CleanTagItems(REG(a0) struct TagItem *tags, REG(d0) LONG dir)
makeproto SAVEDS ASM REGFUNC2(struct TagItem *, BGUI_CleanTagItems,
	REGPARAM(A0, struct TagItem *, tags),
	REGPARAM(D0, LONG, dir))
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
REGFUNC_END

#ifdef __AROS__
makearosproto
AROS_LH1(struct TagItem *, BGUI_GetDefaultTags,
    AROS_LHA(ULONG, id, D0),
    struct Library *, BGUIBase, 28, BGUI)
#else
makeproto SAVEDS ASM struct TagItem *BGUI_GetDefaultTags(REG(d0) ULONG id)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

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

   AROS_LIBFUNC_EXIT
}

#ifdef __AROS__
makearosproto
AROS_LH0(VOID, BGUI_DefaultPrefs,
    struct Library *, BGUIBase, 29, BGUI)
#else
makeproto SAVEDS ASM VOID BGUI_DefaultPrefs(VOID)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

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

   AROS_LIBFUNC_EXIT
}

#ifdef __AROS__
makearosproto
AROS_LH1(VOID, BGUI_LoadPrefs,
    AROS_LHA(UBYTE *, name, A0),
    struct Library *, BGUIBase, 30, BGUI)
#else
makeproto SAVEDS ASM VOID BGUI_LoadPrefs(REG(a0) UBYTE *name)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   BGUI_DefaultPrefs();
   LoadPrefs(name);

   AROS_LIBFUNC_EXIT
}
