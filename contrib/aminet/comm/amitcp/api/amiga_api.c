/*
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * Created: Tue Jan 26 20:44:28 1993 too
 * Last modified: Mon Feb 14 16:48:27 1994 jraja
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:21:39  henrik
 * lots of assembler in api/amiga_api.h that needs to be fixed
 * better not do it since my knowledge of assembler i almost zero
 * think one need to change adresses to something like eax,ebx or something?
 *
 * Revision 3.7  1994/04/02  11:12:59  jraja
 * Added initialization of the resolver variables.
 *
 * Revision 3.6  1994/03/22  07:36:28  jraja
 * Added initialization of the fdCallback to NULL. Also clearing the
 * fdCallback pointer before the final CloseSocket sweep on the library
 * close.
 * Removed f_void definition, already defined in apicalls.h.
 *
 * Revision 3.5  1994/02/14  14:49:20  jraja
 * Changed the default log tag to be NULL (no tag).
 *
 * Revision 3.4  1994/01/20  02:23:44  jraja
 * Changed writeErrnoValue() to use only errno sizes of 1, 2 or 4 bytes.
 *
 * Revision 3.3  1994/01/12  07:17:58  jraja
 * Added initialization of the Syslog() related variables of the library base.
 *
 * Revision 3.2  1994/01/07  15:40:29  too
 * Bug fixes after revision 3.1. Now tested.
 *
 * Revision 3.1  1994/01/04  14:24:05  too
 * Addeed compile time fd_mask and long sizeof checks.
 * Added allocation of socket usage bitmask (at then end of dTable)
 *
 * Revision 1.50  1993/11/26  16:23:42  too
 * Added sendbreaktotasks() function
 *
 * Revision 1.49  1993/08/09  21:28:22  ppessi
 * Added revision headers and release string.
 *
 * Revision 1.48  1993/08/06  08:42:44  jraja
 * Removed the version check, since it does not work.
 *
 * Revision 1.47  1993/08/05  10:35:52  jraja
 * Added version check to the ELL_Open(). Now all Open requests with
 * version less than 2 will be rejected.
 * No requester or alert is set up yet (Add later).
 *
 * Revision 1.46  1993/07/15  20:17:25  too
 * Changed library revision to VERSION 2 REVISION 1
 *
 * Revision 1.45  1993/06/12  22:56:08  too
 * Added freeDataBuffer() calls to UL_Close so now selitem and netdb
 * buffers are freed at CloseLIbrary()
 *
 * Revision 1.44  1993/06/07  12:37:20  too
 * Changed inet_ntoa, netdatabase functions and WaitSelect() use
 * separate buffers for their dynamic buffers
 *
 */

#include <bsdsocket.library_rev.h>
#define RELEASESTRING "AmiTCP/IP release 3 "

/*
 * NOTE: Exec has turned off task switching while in Open, Close, Expunge and
 *	 Reserved functions (via Forbid()/Permit()) so we should not take
 *	 too long in them.
 */

#include <conf.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/syslog.h>

#include <kern/amiga_includes.h>

#include <api/amiga_api.h>
#include <api/allocdatabuffer.h>
#include <api/amiga_libcallentry.h>
#include <api/apicalls.h>
#include <api/resolv.h>

#include <kern/amiga_subr.h>
#include <kern/amiga_log.h>


/*
#if sizeof (fd_mask) != 4 || sizeof (long) != 4
#error "AmiTCP/IP currently depends on fd_mask and longword size of 32 bits."
#endif
*/

#define SOCLIBNAME "bsdsocket.library\0\0" /* space for ".n" at the end */

/*
 *  Semaphore to prevent simultaneous access to library functions.
 */
struct SignalSemaphore syscall_semaphore = { 0 };

/*
 *  some globals.
 */
struct Library *MasterSocketBase = NULL;
struct List	socketBaseList;	     /* list of opened socket library bases */
struct List	garbageSocketBaseList; /* list of libray bases not active
				      anymore (NOT YET IMPLEMENTED) */
struct List	releasedSocketList; /* List for sockets that are in no-one's
				       context, waiting for Obtain */

extern struct Task * AmiTCP_Task; /* reference to AmiTCP/IP task information */

/*
 * Declaration of variable to hold message format string when one
 * task tries to use other tasks' library base pointer. moved here
 * from amiga_libcallentry.h so it doens't generate code.
 */
const char wrongTaskErrorFmt[] =
  "Task %ld (%s) attempted to use library base of Task %ld (%s).";

/*
 * Instead of using exec/initializers.h we looked it as a reference
 * and wrote InitTable by hand
 */

/*
 * OFFSET needed to be casted LONG so compiler doesn't give warning
 * about casting pointer to UWORD
 */
#define OFFSET(structName, structEntry) \
  ((LONG)(&(((struct structName *) 0)->structEntry)))

/*
 * original initTable of only UWORD items doesn't work, since compiler
 * doesn't know address of SOCNAME and VSTRING at compile time, and
 * those are broken to 2 WORDS. therefore initTable is a structure
 * constructed by hand, and those (LONG) values are set longword aligned.
 */
#define id_byte 0xe000
#define id_word 0xd000
#define id_long 0xc000

struct {
  UWORD byte1; UWORD offset1; UWORD ln_type;
  UWORD byte2; UWORD offset2; UWORD lib_flags;
  UWORD long3; UWORD offset3; ULONG ln_Name;
  UWORD word4; UWORD offset4; UWORD lib_Version;
  UWORD word5; UWORD offset5; UWORD lib_Revision;
  UWORD long6; UWORD offset6; ULONG lib_IdString;
  UWORD end7; 
  } Library_initTable = {
    id_byte, OFFSET(Node, ln_Type), NT_LIBRARY << 8,
    id_byte, OFFSET(Library, lib_Flags), (LIBF_SUMUSED|LIBF_CHANGED) << 8,
    id_long, OFFSET(Node, ln_Name), (ULONG)SOCLIBNAME,
    id_word, OFFSET(Library, lib_Version), VERSION,
    id_word, OFFSET(Library, lib_Revision), REVISION,
    id_long, OFFSET(Library, lib_IdString), (ULONG)RELEASESTRING VSTRING,
    0x0000
    };
#undef id_byte
#undef id_word
#undef id_long

/*
 * Api show and hide functions.. during these calls system is not
 * inside FOrbid()/Permit() pair
 */

enum {	API_SCRATCH,		/* api not initialized */
	API_INITIALIZED,	/* librarybase created */
	API_SHOWN,		/* librarybase made visible */
	API_HIDDEN,		/* librarybase hidden */
	API_FUNCTIONPATCHED	/* Api functions set to return -1 */
} api_state = API_SCRATCH;

  /*
   * Setting the following variable to FALSE just before making
   * new socket Library base prevents ELL_Expunge, the final
   * expunging function to remove library base from memory
   */
BOOL AmiTCP_lets_you_do_expunge = FALSE;

BOOL SB_Expunged = FALSE; /* boolean value set by ELL_Expunge */

/* This was inside ELL_Open, but doesn't make any sense as a 
   local variable, since it is declared extern.  Also, GCC
   doesn't like that use of extern on a local declaration. */

extern f_void UserLibrary_funcTable[];

struct Library * SAVEDS RAF2(_ELL_Open,
		 struct Library *,	libPtr,		a6,
		 ULONG,		version,	d0)
#if 0
{
#endif
  
  struct SocketBase * newBase;
  LONG error;
  WORD * i;

  /*
   * One task may open socket library more than once. In that case caller
   * receives the base it has opened already.
   */
  if ((newBase = FindSocketBase(SysBase->ThisTask)) != NULL) 
      newBase->libNode.lib_OpenCnt++;
      return (struct Library *) newBase;
  /*
   * Create new library base.
   * All fields in the base will first be initialized to zero and then
   * modified by initializers in initTable.
   */
  /* LW - the first argument of MakeLibrary is supposed to be 
     an APTR.  <exec/types.h> says ULONG* should be used instead
     if any pointer arithmetic is to be used.  But I suppose we 
     want compiler checking to know they are really functions
   */
  newBase = (struct SocketBase *)MakeLibrary(UserLibrary_funcTable,
					     (UWORD *)&Library_initTable,
					     NULL,
					     sizeof(struct SocketBase),
					     NULL);
  if (newBase == NULL)
      return NULL;

  /*
   * add this newly allocated library base to our list of opened
   * socket libraries
   */	
  AddTail(&socketBaseList, (struct Node *)newBase); 

  /*
   * Modify some MASTER library base fields
   */
  libPtr->lib_OpenCnt++;		/* mark us as having another opener */
  libPtr->lib_Flags&= ~LIBF_DELEXP;	/* prevent delayed expunges */

  /*
   * Initialize new library base
   */
  for (i = (WORD *)((struct Library *)newBase + 1);
       i < (WORD *)(newBase + 1);
       i++)
    *i = 0L;
  newBase->libNode.lib_OpenCnt = 1;
  newBase->errnoPtr = (VOID *)&newBase->defErrno;
  newBase->errnoSize = sizeof newBase->defErrno;
  newBase->thisTask = SysBase->ThisTask;
  newBase->sigIntrMask = SIGBREAKF_CTRL_C;
  
  /* initialize syslog variables */
#if 0 /* initialization to zero is implicit */
  newBase->LogTag = NULL; /* no tag by default, old apps print a tag already */
#endif
  newBase->LogFacility = LOG_USER;
  newBase->LogMask = 0xff;

  /* initialize resolver variables */
  newBase->hErrnoPtr = &newBase->defHErrno;
  newBase->res_socket = -1;
  res_init(&newBase->res_state);

  /* initialize dtable variables */
#if 0 /* initialization to zero is implicit */
  newBase->fdCallback = NULL;
#endif
  newBase->dTableSize = FD_SETSIZE;
  if ((newBase->dTable =
       AllocMem(newBase->dTableSize * sizeof (struct socket *) +
		((newBase->dTableSize - 1) / NFDBITS + 1) * sizeof (fd_mask),
		MEMF_CLEAR|MEMF_PUBLIC)) != NULL) {
    /*	
     * allocate and initialize the timer message reply port
     */
    newBase->timerPort = CreateMsgPort();
    if (newBase->timerPort != NULL) {
      /*
       * Disable signalling for now
       */
      newBase->timerPort->mp_Flags = PA_IGNORE;
      /*
       * allocate and initialize the timerequest
       */
      newBase->tsleep_timer = (struct timerequest *)
	CreateIORequest(newBase->timerPort, sizeof(struct timerequest));
      if (newBase->tsleep_timer != NULL) {
	error = OpenDevice(TIMERNAME, UNIT_VBLANK, 
			   (struct IORequest *)newBase->tsleep_timer, 0);
	if (error == 0) 
	  {
	    /*
	     * Initialize some fields of the IO request to common values
	     */
	    newBase->tsleep_timer->tr_node.io_Command = TR_ADDREQUEST;
	    newBase->tsleep_timer->tr_node.io_Message.mn_Node.ln_Type = NT_UNKNOWN;
	    return (struct Library *) newBase;
	  }
      }
    }
  }
  /*
   * There was some error if we reached here. Call Close to clean up.
   */
  {
#ifdef __GNUC__
/*  UL_Close is now defined in apicalls, and _UL_Close is in Exec_Library_list */
    UL_Close(newBase);
#elif defined __SASC
    extern ULONG* REGARGFUN UL_Close(register __a6 struct SocketBase *);
    UL_Close(newBase);
#else 
#error Compiler not supported!
#endif
  }
    return NULL;
}


ULONG * SAVEDS RAF1(_ELL_Expunge,
		 struct Library *,	libPtr, a6)
#if 0
{
#endif
  /*
   * Since every user gets her own library base, Major library base
   * can be removed immediately after 
   */ 
  if (libPtr->lib_OpenCnt == 0 && AmiTCP_lets_you_do_expunge) 
    {
      VOID * freestart;
      ULONG  size;

#if 0 /* Currently done already  */
      /*
       * unlink SocketBase from System Library list
       */
      Remove((struct Node *)libPtr);
#endif
    
      freestart = (void *)((ULONG)libPtr - (ULONG)libPtr->lib_NegSize);
      size = libPtr->lib_NegSize + libPtr->lib_PosSize;
      FreeMem(freestart, size);
      MasterSocketBase = NULL; 
      
      SB_Expunged = TRUE;
      Signal(AmiTCP_Task, SIGBREAKF_CTRL_F);
      return NULL; /* no AmigaDos seglist there (for system use) */
    }
  /*
   * here if someone still have us open, or AmiTCP don't let us expunge yet
   */
  libPtr->lib_Flags |= LIBF_DELEXP;	/* set delayed expunge flag */
  SB_Expunged = FALSE;
  return NULL; 
}


LONG __saveds Null(void)
{
  return 0L;
}

LONG __saveds Garbage(void)
{     
  return -1L;
}


ULONG * __saveds RAF1(_UL_Close, struct SocketBase *, libPtr ,a6)

#ifdef 0
{
#endif
  VOID * freestart;
  ULONG  size;
  int	 i;

  /*
   * one task may have SocketLibrary opened more than once.
   */
  if (--libPtr->libNode.lib_OpenCnt > 0)
    return NULL; 

#ifdef DEBUG
  log(LOG_DEBUG, "Closing proc 0x%lx base 0x%lx\n", 
      libPtr->thisTask, libPtr);
#endif
  /*
   * Since library base is to be closed, all sockets referenced by this
   * library base must be closed too. Next piece of code searches for open
   * sockets and calls CloseSocket() on our own library base. It is safe
   * to call since Forbid() state is broken if semaphore needs to be waited.
   *
   * Note that the close may linger. In such case the linger time will be
   * waited. The linger may be interrupted by any signal in sigIntrMask.
   */
  libPtr->fdCallback = NULL; /* don't call the callback any more */
  for (i = 0; i < libPtr->dTableSize; i++)
    if (libPtr->dTable[i] != NULL)
      CloseSocket(libPtr, i);
  
  Remove((struct Node *)libPtr); /* remove this librarybase from our list
				    of opened library bases */
  
  if (libPtr->tsleep_timer) 
    {
      if (libPtr->tsleep_timer->tr_node.io_Device != NULL) 
	{
	  AbortIO((struct IORequest *)(libPtr->tsleep_timer));
	  CloseDevice((struct IORequest *)libPtr->tsleep_timer);
	}
      DeleteIORequest((struct IORequest *)libPtr->tsleep_timer);
    }
  if (libPtr->timerPort)
    DeleteMsgPort(libPtr->timerPort);

  freeDataBuffer(&libPtr->selitems);
  freeDataBuffer(&libPtr->hostents);
  freeDataBuffer(&libPtr->netents);
  freeDataBuffer(&libPtr->protoents);
  freeDataBuffer(&libPtr->servents);
  
  if (libPtr->dTable) 
    FreeMem(libPtr->dTable, libPtr->dTableSize * sizeof (struct socket *) +
	    ((libPtr->dTableSize - 1) / NFDBITS + 1) * sizeof (fd_mask));
  
  freestart = (void *)((ULONG)libPtr - (ULONG)libPtr->libNode.lib_NegSize);
  size = libPtr->libNode.lib_NegSize + libPtr->libNode.lib_PosSize;
  bzero(freestart, size);
  FreeMem(freestart, size);
  
  MasterSocketBase->lib_OpenCnt--;
  /*
   * If no more libraries are open and delayed expunge is asked,
   * ELL_expunge() is called.
   */
  if (MasterSocketBase->lib_OpenCnt == 0 &&
      (MasterSocketBase->lib_Flags & LIBF_DELEXP)) 
    {
#if __GNUC__
      /*    register struct Library *a6 __asm("a6") = MasterSocketBase;*/
      /*Now ELL_Expunge is inline "stub" to jump to library _ELL_Expunge*/
      return ELL_Expunge(MasterSocketBase);

#elif __SASC
      return ELL_Expunge(MasterSocketBase);
#else 
#error Compiler not supported!
#endif
    }
    return NULL; 
}

BOOL api_init()
{
  extern void select_init(void);
  extern f_void ExecLibraryList_funcTable[];
  
  if (api_state != API_SCRATCH)
    return TRUE;

  AmiTCP_lets_you_do_expunge = FALSE;
  
  /* Hmm, MasterSocketBase only has open and expunge (that's
     all that's listed in ExecLibraryList _funcTable). */

  MasterSocketBase = MakeLibrary(ExecLibraryList_funcTable,
				 (UWORD *)&Library_initTable,
				 NULL,
				 sizeof(struct Library),
				 NULL);
  if (MasterSocketBase == NULL)
    return FALSE;

  InitSemaphore(&syscall_semaphore);
  select_init(); /* initializes data Select() needs */
  NewList(&socketBaseList);
  NewList(&garbageSocketBaseList);
  NewList(&releasedSocketList);

  api_state = API_INITIALIZED;
  return TRUE;
}

LONG nthLibrary = 0;

BOOL api_show()
{
  struct Node * libNode;
  STRPTR libName = (STRPTR)Library_initTable.ln_Name;

  if (api_state == API_SHOWN)
    return TRUE;
  if (api_state == API_SCRATCH)
    return FALSE;

  Forbid();
  for (libNode = SysBase->LibList.lh_Head; libNode->ln_Succ;
       libNode = libNode->ln_Succ) 
    {
      if (!strncmp(libNode->ln_Name, libName, sizeof (SOCLIBNAME) - 3)) 
	{
#ifdef DEBUG
	  int i;
	  if (libNode->ln_Name[sizeof (SOCLIBNAME) - 3] == '\0') 
	    i = 1;
	  else 
	    i = (BYTE)(libNode->ln_Name[sizeof (SOCLIBNAME) - 2] - '0' + 1);
	  if (nthLibrary < i)
	    nthLibrary = i;
#else
	  Permit();
	  return FALSE;
#endif
	}
    }
  Permit();
#ifdef DEBUG
  if (nthLibrary > 8)
    return FALSE;
  if (nthLibrary) {
    libName[sizeof (SOCLIBNAME) - 3] = '.'; 
    libName[sizeof (SOCLIBNAME) - 2] = '0' + nthLibrary;
    libName[sizeof (SOCLIBNAME) - 1] = '\0';
    MasterSocketBase->lib_Node.ln_Name = libName;
  }
#endif
  AddLibrary(MasterSocketBase);
  api_state = API_SHOWN;
  
  return TRUE;
}

VOID api_hide()
{
  if (api_state != API_SHOWN)
    return;
  Forbid();
  /* unlink Master SocketBase from System Library list */
  Remove((struct Node*)MasterSocketBase);	
  Permit();
  api_state = API_HIDDEN;
}

VOID api_setfunctions() /* DOES NOTHING NOW */
{
/*  struct Node *node2move; */
  
  if (api_state == API_SCRATCH)
    return;
  Forbid();
  if (api_state == API_SHOWN)
    /* unlink Master SocketBase from System Library list */
    Remove((struct Node*)MasterSocketBase);	

  /* here SetFunction()s to patch libray calls (forbid()/permit()) */
  /*  while(node2move = RemHead(&socketBaseList))
      AddTail(&garbageSocketBaseList, node2move); */
  Permit();
  api_state = API_FUNCTIONPATCHED;
}

/*
 * Send CTRL_C to all tasks having socketbase open. 
 */
VOID api_sendbreaktotasks()
{
  extern struct List socketBaseList; /* :/ */
  struct Node * libNode;

  Forbid();
  for (libNode = socketBaseList.lh_Head; libNode->ln_Succ;
       libNode = libNode->ln_Succ)
    if (((struct SocketBase *)libNode)->thisTask != Nettrace_Task)
      Signal(((struct SocketBase *)libNode)->thisTask, SIGBREAKF_CTRL_C);

  Permit();
}


VOID api_deinit()
{
#if DIAGNOSTIC
  if (FindTask(NULL) != AmiTCP_Task)
    log(LOG_ERR, "The calling task of api_deinit() was not AmiTCP/IP");
#endif  
  if (api_state == API_SHOWN || api_state == API_HIDDEN)
    api_setfunctions();
  if (api_state == API_SCRATCH)
    return;
  
  Forbid();
  AmiTCP_lets_you_do_expunge = TRUE;
  {
#if __GNUC__
/*    register volatile struct Library *a6 __asm("a6") = MasterSocketBase; */
    ELL_Expunge(MasterSocketBase);
#elif __SASC
    ELL_Expunge(MasterSocketBase);
#else 
#error Compiler not supported!
#endif
  }
  Permit();

  /*
   * if SB_Expunged == FALSE, waiting until last UL_Close() expunges
   * our library.
   */
  while(SB_Expunged == FALSE)
    Wait(SIGBREAKF_CTRL_F);

  api_state = API_SCRATCH;
}

void writeErrnoValue(struct SocketBase * libPtr, int errno)
{
  /*
   * errnoSize is now restricted to 1, 2 or 4
   */
  BYTE erri = libPtr->errnoSize;

  if (erri == 4) {
    *(ULONG *)libPtr->errnoPtr = (ULONG)errno;
    return;
  }
  if (erri == 2) {
    *(UWORD *)libPtr->errnoPtr = (UWORD)errno;
    return;
  }
  /* size must be 1 */
  *(UBYTE *)libPtr->errnoPtr = (UBYTE)errno;
  return;
}
