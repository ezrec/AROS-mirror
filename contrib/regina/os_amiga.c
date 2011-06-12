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
  struct MsgPort *port;
  struct IOFileSys *pendingread;
  ULONG bytesread;
  struct IOFileSys *pendingwrite;
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

/* Support function for AROS IOFS DOS asynchronous file IO
   TODO: switch or add DOS packets asynchronous handling
 */
static struct IOFileSys *CreateIOFS(ULONG type, struct MsgPort *port, struct FileHandle *fh)
{
  struct IOFileSys *iofs = (struct IOFileSys *)AllocMem(sizeof(struct IOFileSys), MEMF_PUBLIC|MEMF_CLEAR);

  if (iofs == NULL)
    return NULL;
  
  iofs->IOFS.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
  iofs->IOFS.io_Message.mn_ReplyPort    = port;
  iofs->IOFS.io_Message.mn_Length       = sizeof(struct IOFileSys);
  iofs->IOFS.io_Command                 = type;
  iofs->IOFS.io_Flags                   = 0;
#if defined(__AROS__) && !defined(AROS_DOS_PACKETS)
  iofs->IOFS.io_Device                  = fh->fh_Device;
  iofs->IOFS.io_Unit                    = fh->fh_Unit;
#endif
  
  return iofs;
}

static void DeleteIOFS(struct IOFileSys *iofs)
{
  FreeMem(iofs, sizeof(struct IOFileSys));
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
    ai->files[i].fhin = NULL;
    ai->files[i].fhout = NULL;
    ai->files[i].port = CreatePort(NULL, 0);
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
    DeletePort(ai->files[i].port);
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

  for (slot=0; slot<3 && ai->files[slot].fhin!=NULL; slot++)
    ;
    
  if (slot==3)
  {
    errno = EMFILE;
    return -1;
  }

  /* Reset flags */
  ai->files[slot].flags = 0;

  if (Pipe("PIPEFS:", &(ai->files[slot].fhin), &(ai->files[slot].fhout)) != DOSTRUE)
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


/* Get possible returned messages for a certain file handle */
static void handle_msgs(FileHandleInfo *fhi)
{
  struct IOFileSys *iofs;
    
  while ((iofs = (struct IOFileSys *)GetMsg(fhi->port)) != NULL)
  {
    if (iofs == fhi->pendingread)
      fhi->flags |= FHI_READRETURNED;
    else if (iofs == fhi->pendingwrite)
    {
      FreeVec(iofs->io_Union.io_WRITE.io_Buffer);
      DeleteIOFS(iofs);
      fhi->pendingwrite = NULL;
    }
    else
      ReplyMsg((struct Message *)iofs);
  }
}

/* First abort pending IOs, then close filehandle */
static int Amiga_close(int handle, void *async_info)
{
  AsyncInfo *ai = async_info;
  FileHandleInfo *fhi;

  assert(handle<3 && handle>=0);

  fhi = &ai->files[handle];

  handle_msgs(fhi);
  if (fhi->pendingread != NULL && !(fhi->flags & FHI_READRETURNED))
  {
    AbortIO((struct IORequest *)fhi->pendingread);
    WaitIO((struct IORequest *)fhi->pendingread);
    FreeVec(fhi->pendingread->io_Union.io_READ.io_Buffer);
    DeleteIOFS(fhi->pendingread);
  }
  if (fhi->pendingwrite != NULL)
  {
    AbortIO((struct IORequest *)fhi->pendingwrite);
    WaitIO((struct IORequest *)fhi->pendingwrite);
    FreeVec(fhi->pendingwrite->io_Union.io_WRITE.io_Buffer);
    DeleteIOFS(fhi->pendingwrite);
  }
  if (fhi->flags & FHI_ISINPUT)
    Close(fhi->fhout);
  else
    Close(fhi->fhin);
  fhi->fhout = fhi->fhin = BNULL;
  DeletePort(fhi->port);

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


static int Amiga_read(int handle, void *buf, unsigned size, void *async_info)
{
  AsyncInfo *ai = async_info;
  FileHandleInfo *fhi;

  assert(handle<3);
  
  fhi = &ai->files[handle];
    
  if (fhi->fhin == NULL)
    return -EBADF;
  
  if (fhi->pendingread == NULL)
  {
    struct IOFileSys *iofs;
    
    fhi->pendingread = iofs = CreateIOFS(FSA_READ, fhi->port, fhi->fhin);
    if (iofs == NULL)
      return -ENOMEM;
    
    iofs->io_Union.io_READ.io_Buffer = AllocVec(size, MEMF_PUBLIC);
    iofs->io_Union.io_READ.io_Length = size;
 
    SendIO(&iofs->IOFS);
  }
    
  handle_msgs(fhi);
    
  if (fhi->flags & FHI_READRETURNED)
  {
    if (fhi->bytesread + size > fhi->pendingread->io_Union.io_READ.io_Length)
      size = fhi->pendingread->io_Union.io_READ.io_Length - fhi->bytesread;

    memcpy(buf, fhi->pendingread->io_Union.io_READ.io_Buffer, size);
    fhi->bytesread += size;
    
    if (fhi->bytesread == fhi->pendingread->io_Union.io_READ.io_Length)
    {
      FreeVec(fhi->pendingread->io_Union.io_READ.io_Buffer);
      DeleteIOFS(fhi->pendingread);
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
  
  assert(handle<3);

  fhi = &ai->files[handle];

  if (buf==NULL && size==0)
  { /* Flush write */
    if (fhi->pendingwrite!=NULL)
    {
      WaitIO((struct IORequest *)fhi->pendingwrite);
      FreeVec(fhi->pendingwrite->io_Union.io_WRITE.io_Buffer);
      DeleteIOFS(fhi->pendingwrite);
      fhi->pendingwrite = NULL;
    }
    return 0;
  }

  if (fhi->fhin == NULL)
    return -EBADF;
  
  handle_msgs(fhi);
  
  if (fhi->pendingwrite == NULL)
  {
    struct IOFileSys *iofs;
    
    fhi->pendingwrite = iofs = CreateIOFS(FSA_WRITE, fhi->port, fhi->fhout);
    if (iofs == NULL)
      return -ENOMEM;
    
    iofs->io_Union.io_WRITE.io_Buffer = AllocVec(size, MEMF_PUBLIC);
    memcpy(iofs->io_Union.io_WRITE.io_Buffer, buf, size);
    iofs->io_Union.io_WRITE.io_Length = size;
    
    SendIO(&iofs->IOFS);
      
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
    if (ai->files[i].fhin != NULL && ai->files[i].flags & FHI_WAIT &&
	(ai->files[i].pendingwrite != NULL || (ai->files[i].pendingread != NULL && !(ai->files[i].flags & FHI_READRETURNED)))
       )
      mask |= 1<<ai->files[i].port->mp_SigBit;
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
			NP_Error, (errhndl == -1) ? Error() : ai->files[errhndl].fhout,
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
