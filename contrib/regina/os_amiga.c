#include "rexx.h"

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <utility/tagitem.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <dos/filesystem.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <exec/lists.h>
#include <exec/nodes.h>

#include <assert.h>
#include <errno.h>

#include <stdlib.h>

/* On amiga we store for each connection the input and output
   filehandle.
   We use as filehandle the number of the slot, so we have filehandles
   from 0 to 2
*/  
typedef struct {
  BPTR fhin, fhout;
  struct MsgPort *replyPort;
  struct DosPacket *pendingread;
  ULONG bytesread;
  struct DosPacket *pendingwrite;
  ULONG flags;
} FileHandleInfo;

#define FHI_WAIT          (1<<0)
#define FHI_ISINPUT       (1<<1)
#define FHI_READRETURNED  (1<<2)

typedef struct {
  const tsd_t *TSD;
  struct Task *parent, *child;
  BYTE psigbit, csigbit;
  environment *parentenv;
  const char *childcmd;
  int retval;
  FileHandleInfo files[3];
} AsyncInfo;

/* SupreplyPort function for AmigaDOS asynchronous file IO
 */
static struct DosPacket *CreateDosPacket(void)
{
    return AllocDosObject(DOS_STDPKT, NULL);
}

static void DeleteDosPacket(struct DosPacket *dp)
{
    return FreeDosObject(DOS_STDPKT, dp);
}

/* Allocate and setup AsyncInfo */
static void *Amiga_create_async_info(const tsd_t *TSD)
{
  AsyncInfo *ai = MallocTSD(sizeof(AsyncInfo));
  int i;
  
  ai->TSD = TSD;
  __amiga_set_ai((tsd_t *)TSD, ai);
  
  for(i=0; i<3; i++)
  {
    ai->files[i].fhin = BNULL;
    ai->files[i].fhout = BNULL;
    ai->files[i].replyPort = CreatePort(NULL, 0);
  }

  return (void *)ai;
}

/* Deallocate AsyncInfo */
static void Amiga_delete_async_info(void *async_info)
{
  AsyncInfo *ai = (AsyncInfo *)async_info;
  int i;
 
  if (ai == NULL)
    return;

  for(i=0; i < 3; i++)
  {
    assert(ai->files[i].fhin == BNULL);
    assert(ai->files[i].fhout == BNULL);
    DeletePort(ai->files[i].replyPort);
  }

  __amiga_set_ai((tsd_t *)ai->TSD, NULL);

  Free_TSD(ai->TSD, ai);
}

/* Reset FHI_WAIT flag */
static void Amiga_reset_async_info(void *async_info)
{
  AsyncInfo *ai = (AsyncInfo *)async_info;
  int i;

  for(i = 0; i < 3; i++)
    ai->files[i].flags &= ~FHI_WAIT;
}

/* Mark handle to wait for it */
static void Amiga_add_async_waiter(void *async_info, int handle, int add_as_read_handle)
{
  AsyncInfo *ai = (AsyncInfo *)async_info;

  ai->files[handle].flags |= FHI_WAIT;
}

/* For a subprocess connection on amiga a pipe will be opened
   and both read and write ends will be stored.
   Only a read or write handle will be returned depending on wether
   this connection is input or output.
*/
static int Amiga_open_subprocess_connection(const tsd_t *TSD, environpart *ep)
{
  AsyncInfo *ai = __amiga_get_ai(TSD);
  int slot;
  char buff[5 + 5 + 16 + 1];

  for (slot=0; slot<3 && ai->files[slot].fhin!=BNULL; slot++)
    ;
    
  if (slot==3)
  {
    errno = EMFILE;
    return -1;
  }

  /* Reset flags */
  ai->files[slot].flags = 0;

  snprintf(buff, sizeof(buff), "PIPE:rexx-%016llx", (unsigned long long)(IPTR)FindTask(NULL));

  if (Pipe(buff, &(ai->files[slot].fhin), &(ai->files[slot].fhout)) != DOSTRUE)
  {
    errno = EACCES;
    return -1;
  }
  
  ai->files[slot].bytesread = 0;
  
  if (ep->flags.isinput)
  {
    ai->files[slot].flags |= FHI_ISINPUT;
    ep->hdls[0] = -1;
    ep->hdls[1] = slot;
  }
  else
  {
    ep->hdls[0] = slot;
    ep->hdls[1] = -1;
  }
    
  return 0;
}

static struct DosPacket *wait_pkt(struct MsgPort *port)
{
  WaitPort(port);
  return (struct DosPacket *)(GetMsg(port)->mn_Node.ln_Name);
}


/* Get possible returned messages for a certain file handle */
static void handle_msgs(FileHandleInfo *fhi)
{
  struct DosPacket *dp;

  while ((dp = wait_pkt(fhi->replyPort)) != NULL) {
    if (dp == fhi->pendingread)
      fhi->flags |= FHI_READRETURNED;
    else if (dp == fhi->pendingwrite)
    {
      FreeVec((APTR)dp->dp_Arg2);
      DeleteDosPacket(dp);
      fhi->pendingwrite = NULL;
    }
    else {
      /* TODO: This is impossible - we should die here */
    }
  }
}

/* First abort pending IOs, then close filehandle */
static int Amiga_close(int handle, void *async_info)
{
  AsyncInfo *ai = async_info;
  FileHandleInfo *fhi;

  assert(handle<3 && handle>=0);

  fhi = &ai->files[handle];

  while (fhi->pendingread != NULL && !(fhi->flags & FHI_READRETURNED))
  {
    handle_msgs(fhi);
  }
  if (fhi->pendingread != NULL) {
    FreeVec((APTR)fhi->pendingread->dp_Arg2);
    DeleteDosPacket(fhi->pendingread);
    fhi->pendingread = NULL;
  }
  while (fhi->pendingwrite != NULL)
  {
    handle_msgs(fhi);
  }
  if (fhi->flags & FHI_ISINPUT)
    Close(fhi->fhout);
  else
    Close(fhi->fhin);
  fhi->fhout = fhi->fhin = BNULL;
  DeletePort(fhi->replyPort);

  return 0;
}

/* We don't have special filehandles */
static void Amiga_close_special( int handle )
{
  assert( handle == -1 );
}


/* Amiga uses pipes so cannot restart file */
static void Amiga_restart_file(int handle)
{
  assert(handle == -1);
}

static void Amiga_unblock_handle(int *handle, void *async_info)
{
  /* All handles are non-blocking on AROS => do nothing */
}

static struct MsgPort *file_port(BPTR file)
{
  struct FileHandle *fh = BADDR(file);
  return fh->fh_Type;
}

static int Amiga_read(int handle, void *buf, unsigned size, void *async_info)
{
  AsyncInfo *ai = async_info;
  FileHandleInfo *fhi;
  struct DosPacket *dp;

  assert(handle<3);
  
  fhi = &ai->files[handle];
    
  if (fhi->fhin == BNULL)
    return -EBADF;
  
  if (fhi->pendingread == NULL)
  {
    fhi->pendingread = CreateDosPacket();
    if (fhi->pendingread == NULL)
      return -ENOMEM;

    dp = fhi->pendingread;
    dp->dp_Type = ACTION_READ;
    dp->dp_Arg1 = (IPTR)fhi->fhin;
    dp->dp_Arg2 = (IPTR)AllocVec(size, MEMF_PUBLIC);
    dp->dp_Arg3 = size;
    
    SendPkt(dp, file_port(fhi->fhin), fhi->replyPort);
  }
    
  handle_msgs(fhi);
    
  if (fhi->flags & FHI_READRETURNED)
  {
    if (fhi->bytesread + size > fhi->pendingread->dp_Res1)
      size = fhi->pendingread->dp_Res1 - fhi->bytesread;

    memcpy(buf, (APTR)fhi->pendingread->dp_Arg2, size);
    fhi->bytesread += size;
    
    if (fhi->bytesread == fhi->pendingread->dp_Res1)
    {
      FreeVec((APTR)fhi->pendingread->dp_Arg2);
      DeleteDosPacket(fhi->pendingread);
      fhi->pendingread = NULL;
      fhi->bytesread = 0;
      fhi->flags &= ~FHI_READRETURNED;
    }
      
    return size;
  }
  else
    return -EAGAIN;
}


static int Amiga_write(int handle, const void *buf, unsigned size, void *async_info)
{
  AsyncInfo *ai = (AsyncInfo *)async_info;
  FileHandleInfo *fhi;
  struct DosPacket *dp;
  
  assert(handle<3);

  fhi = &ai->files[handle];

  if (buf==NULL && size==0)
  { /* Flush write */
    while (fhi->pendingwrite!=NULL) {
      handle_msgs(fhi);
    }
    return 0;
  }

  if (fhi->fhin == BNULL)
    return -EBADF;
  
  handle_msgs(fhi);
  
  if (fhi->pendingwrite == NULL)
  {
    fhi->pendingwrite = dp = CreateDosPacket();
    if (fhi->pendingwrite == NULL)
      return -ENOMEM;
   
    dp->dp_Type = ACTION_WRITE;
    dp->dp_Arg1 = (IPTR)fhi->fhout;
    dp->dp_Arg2 = (IPTR)AllocVec(size, MEMF_PUBLIC);
    CopyMem(buf, (APTR)dp->dp_Arg2, size);
    dp->dp_Arg3 = size;
    
    SendPkt(dp, file_port(fhi->fhout), fhi->replyPort);
    return (int)size;
  }
  else
    return -EAGAIN;
}
  

static void Amiga_wait_async_info(void *async_info)
{
  AsyncInfo *ai = (AsyncInfo *)async_info;
  int i;
  ULONG mask = SIGBREAKF_CTRL_C;

  for (i = 0; i < 3; i++)
  {
    if (ai->files[i].fhin != BNULL && ai->files[i].flags & FHI_WAIT &&
	(ai->files[i].pendingwrite != NULL || (ai->files[i].pendingread != NULL && !(ai->files[i].flags & FHI_READRETURNED)))
       )
      mask |= 1<<ai->files[i].replyPort->mp_SigBit;
  }

  if (mask != SIGBREAKF_CTRL_C)
    Wait(mask);
}

GLOBAL_PROTECTION_VAR(startcommand)
static AsyncInfo *childai;

static void StartCommand(void)
{
  AsyncInfo *ai = childai;
  char *cmd;
  struct Library *UtilityBase;
  struct TagItem *tags;
  struct DosLibrary *DOSBase;

  DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
  
  ai->child = FindTask(NULL);
  ai->csigbit = AllocSignal(-1);
  cmd = AllocVec(strlen(ai->childcmd)+1, MEMF_PUBLIC|MEMF_CLEAR);
  strcpy(cmd, ai->childcmd);
  
  Signal(ai->parent, 1<<ai->psigbit);
  Wait(1<<ai->csigbit);
  FreeSignal(ai->csigbit);
    
  UtilityBase = OpenLibrary("utility.library", 0);
  if (UtilityBase == NULL)
  {
    FreeVec(cmd);
    ai->retval = 20;
    Signal(ai->parent, 1<<ai->psigbit);
    CloseLibrary((struct Library *)DOSBase);
    return;
  }
  
  tags = AllocateTagItems(5);
  if (tags == NULL)
  {
    FreeVec(cmd);
    CloseLibrary(UtilityBase);
    ai->retval = 20;
    Signal(ai->parent, 1<<ai->psigbit);
    CloseLibrary((struct Library *)DOSBase);
    return;
  }

  ai->retval = SystemTagList(cmd, NULL);
  FreeVec(cmd);

  FreeTagItems(tags);
  CloseLibrary(UtilityBase);
  
  Signal(ai->parent, 1<<ai->psigbit);
  
  CloseLibrary((struct Library *)DOSBase);
}

static int Amiga_fork_exec(tsd_t *TSD, environment *env, const char *cmdline, int *rcode)
{
  AsyncInfo *ai = (void *)__amiga_get_ai(TSD);

  switch (env->subtype)
  {
  case SUBENVIR_PATH:
  case SUBENVIR_COMMAND:
  case SUBENVIR_SYSTEM:
    {
      int inhndl  = env->input.hdls[1],
	  outhndl = env->output.hdls[0],
	  errhndl = env->error.hdls[0];
      struct Process *me = (struct Process *)FindTask(NULL);

      ai->psigbit = AllocSignal(-1);
      if (ai->psigbit < 0)
      {
	errno = ECHILD;
	return -1;
      }
      ai->parent = FindTask(NULL);
      ai->parentenv = env;
      ai->childcmd = cmdline;

      THREAD_PROTECT(startcommand)
      childai = ai;

      CreateNewProcTags(NP_Entry, StartCommand,
			NP_Input, (inhndl == -1) ? Input() : ai->files[inhndl].fhin,
			NP_CloseInput, inhndl != -1,
			NP_Output, (outhndl == -1) ? Output() : ai->files[outhndl].fhout,
			NP_CloseOutput, outhndl != -1,
			NP_Error, (errhndl == -1) ? me->pr_CES : ai->files[errhndl].fhout,
			NP_CloseError, errhndl != -1,
			NP_Cli, TRUE,
			TAG_DONE, NULL);
      Wait(1<<ai->psigbit);
      THREAD_UNPROTECT(startcommand)
      Signal(ai->child, 1<<ai->csigbit);
    }
    return (int)ai;
    break;
    
  case SUBENVIR_REXX:
    errno = ENOSYS;
    return -1;
  }
    
  errno = ENOSYS;
  return -1;
}

static int Amiga_wait(int process)
{
  AsyncInfo *ai = (AsyncInfo *)process;

  Wait(1<<ai->psigbit);
  FreeSignal(ai->psigbit);
  ai->psigbit = -1;
    
  return ai->retval;
}

static void Amiga_init(void)
{
}

static int Amiga_setenv(const char *name, const char *value)
{
  return setenv(name, value, 1);
}

#include "utsname.h"

static int Amiga_uname(struct regina_utsname *name)
{
  strcpy( name->sysname, "AMIGA" );
  sprintf( name->version, "%d", 0 );
  sprintf( name->release, "%d", 0 );
  strcpy( name->nodename, "standalone" );
  strcpy( name->machine, "i386" );

  return 0;
}

OS_Dep_funcs __regina_OS_Amiga =
{
   Amiga_init,                         /* init                       */
   Amiga_setenv,                       /* setenv                     */
   Amiga_fork_exec,                    /* fork_exec                  */
   Amiga_wait,                         /* wait                       */
   Amiga_open_subprocess_connection,   /* open_subprocess_connection */
   Amiga_unblock_handle,               /* unblock_handle             */
   Amiga_restart_file,                 /* restart_file               */
   Amiga_close,                        /* close                      */
   Amiga_close_special,                /* close_special              */
   Amiga_read,                         /* read                       */
   Amiga_write,                        /* write                      */
   Amiga_create_async_info,            /* create_async_info          */
   Amiga_delete_async_info,            /* delete_async_info          */
   Amiga_reset_async_info,             /* reset_async_info           */
   Amiga_add_async_waiter,             /* add_async_waiter           */
   Amiga_wait_async_info,              /* wait_async_info            */
   Amiga_uname                         /* uname                      */
};
