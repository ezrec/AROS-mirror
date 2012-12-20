#include "rexx.h"

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <utility/tagitem.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <exec/lists.h>
#include <exec/nodes.h>

#include <assert.h>
#include <errno.h>

#include <stdlib.h>

#ifdef __AROS__

#define DEBUG 0
#include <aros/debug.h>

#else

#define D(x)
#ifndef BNULL
#define BNULL ((BPTR)NULL)
#endif

#endif

typedef struct {
   const tsd_t *TSD;
   struct Task *parent, *child;
   BYTE psigbit, csigbit;
   environment *parentenv;
   const char *childcmd;
   int retval;
} ChildInfo;

typedef enum {
   PIPE_READ, PIPE_WRITE
} AmigaPipeType;

typedef struct {
   AmigaPipeType type;
   BPTR file;
} AmigaPipeInfo;

/* On amiga(-like) systems the pipe used for communicating
   is asynchronous from itself and does not to be handled
   by custom code.
*/
static void *Amiga_create_async_info(const tsd_t *TSD)
{
   D(bug("[Amiga_create_async_info] TSD=%p\n", TSD));

   /* Amiga pipes are async */
   return NULL;
}

static void Amiga_delete_async_info(void *async_info)
{
   D(bug("[Amiga_delete_async_info] async_info=%p\n", async_info));
   /* Amiga pipes are async */
}

/* Reset FHI_WAIT flag */
static void Amiga_reset_async_info(void *async_info)
{
   D(bug("[Amiga_reset_async_info] async_info=%p\n", async_info));
   /* Amiga pipes are async */
}

/* Mark handle to wait for it */
static void Amiga_add_async_waiter(void *async_info, int handle, int add_as_read_handle)
{
   /* Amiga pipes are async */
}

/* For a subprocess connection on amiga a pipe will be opened
   and both read and write ends will be stored.
   Only a read or write handle will be returned depending on wether
   this connection is input or output.
   We can't just cast a BPTR to int in this function as on some archs
   sizeof(int) < sizeof(void *).
*/
static int Amiga_open_subprocess_connection(const tsd_t *TSD, environpart *ep)
{
   AmigaPipeInfo *pipein, *pipeout;
   int hndlin, hndlout;

   D(bug("[Amiga_open_subprocess_connection] Entering\n"));

   pipein = malloc(sizeof(AmigaPipeInfo));
   if(pipein == NULL)
   {
      errno = ENOMEM;
      return -1;
   }
   pipein->type = PIPE_READ;
   pipein->file = BNULL;

   pipeout = malloc(sizeof(AmigaPipeInfo));
   if(pipeout == NULL)
   {
      errno = ENOMEM;
      return -1;
   }
   pipeout->type = PIPE_WRITE;
   pipeout->file = BNULL;

   hndlin = __amiga_ptr2int(TSD, pipein);
   hndlout = __amiga_ptr2int(TSD, pipeout);
   if (hndlin == -1 || hndlout == -1)
   {
      free(pipein);
      free(pipeout);
      errno = ENOMEM;
      return -1;
   }

   ep->hdls[0] = hndlin;
   ep->hdls[1] = hndlout;

   return 0;
}

/* First abort pending IOs, then close filehandle */
static int Amiga_close(int handle, void *async_info)
{
   const tsd_t *TSD = GLOBAL_ENTRY_POINT();
   AmigaPipeInfo *pipe = __amiga_getptr( TSD, handle );

   D(bug("[Amiga_close] handle=%d\n", handle));

   if(pipe->file != BNULL)
      Close(pipe->file);
   __amiga_clearptr(TSD, handle);
   free(pipe);

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
   assert( handle == -1 );
}

static void Amiga_unblock_handle(int *handle, void *async_info)
{
  /* All handles are non-blocking on AROS => do nothing */
}

static int Amiga_read(int handle, void *buf, unsigned size, void *async_info)
{
   const tsd_t *TSD = GLOBAL_ENTRY_POINT();
   AmigaPipeInfo *pipe = __amiga_getptr( TSD, handle );
   int retval;

   D(bug("[Amiga_read] hndl=%d\n"));

   assert(pipe->type == PIPE_READ);
   assert(pipe->file != BNULL);

   retval = Read( pipe->file, buf, size );
   if( retval < 0 )
      retval = -EACCES;

   D(bug("[Amiga_read] retval=%d\n", retval));

   return retval;
}

static int Amiga_write(int handle, const void *buf, unsigned size, void *async_info)
{
   const tsd_t *TSD = GLOBAL_ENTRY_POINT();
   AmigaPipeInfo *pipe = __amiga_getptr( TSD, handle );
   int retval;

   D(bug("[Amiga_write] hndl=%d\n"));

   assert(pipe->type == PIPE_WRITE);
   assert(pipe->file != BNULL);

   retval = Write( pipe->file, buf, size );
   if( retval < 0 )
      retval = -EACCES;

   D(bug("[Amiga_write] retval=%d\n", retval));

   return retval;
}
  

static void Amiga_wait_async_info(void *async_info)
{
   /* Amiga pipes are async */
}

GLOBAL_PROTECTION_VAR(startcommand)
static ChildInfo *childinfo;

static void StartCommand(void)
{
   ChildInfo *info = childinfo;
   char *cmd;
   struct Library *UtilityBase;
   struct DosLibrary *DOSBase;

   D(bug("[Startcommand]: Entering\n"));
   
   DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
  
   info->child = FindTask(NULL);
   info->csigbit = AllocSignal(-1);
   cmd = AllocVec(strlen(info->childcmd)+1, MEMF_PUBLIC|MEMF_CLEAR);
   strcpy(cmd, info->childcmd);
   D(bug("[Startcommand]: cmd='%s'\n", cmd));
   
   D(bug("[Startcommand]: Signaling parent\n"));
   Signal(info->parent, 1<<info->psigbit);
   D(bug("[Startcommand]: Waiting for parent\n"));
   Wait(1<<info->csigbit);
   FreeSignal(info->csigbit);
    
   UtilityBase = OpenLibrary("utility.library", 0);
   if (UtilityBase == NULL)
   {
      FreeVec(cmd);
      info->retval = 20;
      Signal(info->parent, 1<<info->psigbit);
      CloseLibrary((struct Library *)DOSBase);
      return;
   }
  
   info->retval = SystemTags(cmd, NP_Synchronous, TRUE, TAG_DONE, NULL);
   D(bug("[Startcommand]: retval=%d\n", info->retval));
   FreeVec(cmd);

   CloseLibrary(UtilityBase);
  
   D(bug("[Startcommand]: Signaling parent\n"));
   Signal(info->parent, 1<<info->psigbit);
  
   CloseLibrary((struct Library *)DOSBase);
}

static int Amiga_fork_exec(tsd_t *TSD, environment *env, const char *cmdline, int *rcode)
{
   switch (env->subtype)
   {
   case SUBENVIR_PATH:
   case SUBENVIR_COMMAND:
   case SUBENVIR_SYSTEM:
      {
         ChildInfo *info = malloc( sizeof(ChildInfo) );
         char buff[32];
         int inhndl = env->input.hdls[1],
            outhndl = env->output.hdls[0],
            errhndl = env->error.hdls[0];
         BPTR sub_input = BNULL, sub_output = BNULL, sub_error = BNULL;
         BOOL close_input, close_output, close_error;
         AmigaPipeInfo *pipe = NULL;
         int proc;
         struct Process *me = (struct Process *)FindTask(NULL);

         D(bug("[Amiga_fork_exec]: Entering cmdline='%s'\n", cmdline));

         if( info == NULL )
         {
            errno = ENOMEM;
            return -1;
         }

         proc = __amiga_ptr2int( TSD, info );
         D(bug("[Amiga_fork_exec]: Got proc=%d\n", proc));
         if ( proc < 0 )
         {
            errno = ENOMEM;
            free( info );
            return -1;
         }

         info->psigbit = AllocSignal(-1);
         if (info->psigbit < 0)
         {
            errno = ECHILD;
            return -1;
         }
         info->parent = FindTask(NULL);
         info->parentenv = env;
         info->childcmd = cmdline;

         if(inhndl != -1)
         {
            pipe = (AmigaPipeInfo *)__amiga_getptr(TSD, inhndl);
            /* Regina will write into input of subcommand */
            assert(pipe->type == PIPE_WRITE);
            
            snprintf(buff, sizeof(buff), "PIPE:rexx-%p-in", me);
            pipe->file = Open(buff, MODE_NEWFILE);
            if (pipe->file != BNULL)
            {
               /* Input() of subcommand with read from pipe */
               sub_input = Open(buff, MODE_OLDFILE);
               if(sub_input == BNULL)
               {
                  errno = ENOMEM;
                  Close(pipe->file);
                  pipe->file = BNULL;
                  return -1;
               }
            }
            else
            {
               errno = EACCES;
               return -1;
            }

            close_input = TRUE;
         }
         else
         {
            sub_input = Input();
            close_input = FALSE;
         }

         if(outhndl != -1)
         {
            pipe = (AmigaPipeInfo *)__amiga_getptr(TSD, outhndl);
            /* Regina will read output of subcommand */
            assert(pipe->type == PIPE_READ);
            
            snprintf(buff, sizeof(buff), "PIPE:rexx-%p-out", me);
            /* Output() of subcommand will write in pipe */
            sub_output = Open(buff, MODE_NEWFILE);
            if (sub_output != BNULL)
            {
               pipe->file = Open(buff, MODE_OLDFILE);
               if(pipe->file == BNULL)
               {
                  errno = ENOMEM;
                  Close(pipe->file);
                  pipe->file = BNULL;
                  return -1;
               }
            }
            else
            {
               errno = EACCES;
               return -1;
            }

            close_output = TRUE;
         }
         else
         {
            sub_output = Output();
            close_output = FALSE;
         }

         if(errhndl != -1)
         {
            pipe = (AmigaPipeInfo *)__amiga_getptr(TSD, errhndl);
            /* Regina will read error output of subcommand */
            assert(pipe->type == PIPE_READ);
            
            snprintf(buff, sizeof(buff), "PIPE:rexx-%p-err", me);
            /* Error() of subcommand will write in pipe */
            sub_error = Open(buff, MODE_NEWFILE);
            if (sub_error != BNULL)
            {
               pipe->file = Open(buff, MODE_OLDFILE);
               if(pipe->file == BNULL)
               {
                  errno = ENOMEM;
                  Close(pipe->file);
                  pipe->file = BNULL;
                  return -1;
               }
            }
            else
            {
               errno = EACCES;
               return -1;
            }

            close_error = TRUE;
         }
         else
         {
            sub_error = me->pr_CES;
            close_error = FALSE;
         }

         THREAD_PROTECT(startcommand)
         childinfo = info;

         D(bug("[Amiga_fork_exec]: Starting command\n"));

         CreateNewProcTags
         (
            NP_Entry, StartCommand,
            NP_Input, sub_input, NP_CloseInput, close_input,
            NP_Output, sub_output, NP_CloseOutput, close_output,
            NP_Error, sub_error, NP_CloseError, close_error,
            NP_Cli, TRUE,
            TAG_DONE, NULL
         );
         D(bug("[Amiga_fork_exec]: Waiting child cache of info\n"));
         Wait(1<<info->psigbit);
         THREAD_UNPROTECT(startcommand)
         D(bug("[Amiga_fork_exec]: Signaling child\n"));
         Signal(info->child, 1<<info->csigbit);

         D(bug("[Amiga_fork_exec]: Returning proc=%d\n", proc));
         return proc;
      }
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
   const tsd_t *TSD = GLOBAL_ENTRY_POINT();
   ChildInfo *info = __amiga_getptr( TSD, process );
   int retval;

   D(bug("[Amiga_wait] Waiting for child\n"));

   Wait( 1<<info->psigbit );
   retval = info->retval;
   FreeSignal( info->psigbit );
   info->psigbit = -1;
   __amiga_clearptr( TSD, process );
   free( info );

   D(bug("[Amiga_wait] retval=%d\n", retval));

   return retval;
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
#ifdef __PPC__
   strcpy( name->machine, "ppc" );
#else
   strcpy( name->machine, "i386" );
#endif
   
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
