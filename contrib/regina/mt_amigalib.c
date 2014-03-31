/* This is the file to support single-threading.
 * We initialize the global data structure and the global access variable.
 */

#if !defined(__AROS__) && !defined(_AMIGA)
# error mt_amiga.c only works on Amiga or AROS
#endif

#if !defined(RXLIB)
# error compiling mt_amiga.c without being a library does not make sense
#endif

#include "rexx.h"

#include <proto/alib.h>
#include <exec/memory.h>

#define DEBUG 0
#include <aros/debug.h>
#include <aros/symbolsets.h>

#include <assert.h>

APTR __regina_semaphorepool;

typedef struct _mt_tsd_t {
   APTR mempool;
} mt_tsd_t;


/* Lowest level memory allocation function for normal circumstances. */
static void *MTMalloc(const tsd_t *TSD,size_t size)
{
   mt_tsd_t *mt = (mt_tsd_t *)TSD->mt_tsd;
   void *mem;

   size += sizeof(size_t);
   mem = AllocPooled( mt->mempool, size);
   if ( mem == NULL )
      return NULL;

   *((size_t*)mem)=size;
   return (void *)(((char *)mem)+sizeof(size_t));
}

/* Lowest level memory deallocation function for normal circumstances. */
static void MTFree(const tsd_t *TSD,void *chunk)
{
   mt_tsd_t *mt = (mt_tsd_t *)TSD->mt_tsd;
   APTR mem = (APTR)(((char *)chunk)-sizeof(size_t));
  
   FreePooled( mt->mempool, (APTR)mem, *(size_t *)mem );
}

/* Lowest level exit handler. Use this indirection to prevent errors. */
static void MTExit(int code)
{
   D(bug("[mt_amigalib::MTExit] exiting with code=%d\n", code));

   exit(code);
}

void exit_amigaf( APTR ); /* In amifuncs.c */

static void cleanup(int dummy, void *ptr)
{
   tsd_node_t *node = (tsd_node_t *)ptr;
   mt_tsd_t *mt = (mt_tsd_t *)node->TSD->mt_tsd;

   D(bug("[mt_amigalib::cleanup] node=%p\n"));

   exit_amigaf( node->TSD->ami_tsd );

   DeletePool( mt->mempool );

   node->TSD = NULL; /* Node is cleared */
}

int IfcReginaCleanup( VOID )
{
   /* Do nothing, currently cleanup is done through on_exit function */
   return 1;
}

tsd_t *ReginaInitializeThread(void)
{
   int OK;

   tsd_t *__regina_tsd = malloc(sizeof(tsd_t));
   mt_tsd_t *mt;

   D(bug("[mt_amigalib::ReginaInitializeThread] TSD=%p\n", __regina_tsd));

   /* Default all values to zero        */
   memset(__regina_tsd,0,sizeof(tsd_t));
   __regina_tsd->MTMalloc = MTMalloc;
   __regina_tsd->MTFree = MTFree;
   __regina_tsd->MTExit = MTExit;

   OK = ( __regina_tsd->mt_tsd = malloc(sizeof(mt_tsd_t))) != NULL;
   mt = (mt_tsd_t *)__regina_tsd->mt_tsd;
   OK &= ( mt->mempool = CreatePool(MEMF_PUBLIC, 8192, 1024) ) != NULL;

   OK &= init_memory(__regina_tsd);     /* Initialize the memory module FIRST*/

   D(bug("[mt_amigalib::ReginaInitializeThread] TSD->mem_tsd=%p\n", __regina_tsd->mem_tsd));

   /* Without the initial memory we don't have ANY chance! */
   if (!OK)
      return(NULL);

   {
      extern OS_Dep_funcs __regina_OS_Amiga;
      __regina_tsd->OS = &__regina_OS_Amiga;
   }

   OK &= init_vars(__regina_tsd);      /* Initialize the variable module    */
   OK &= init_stacks(__regina_tsd);    /* Initialize the stack module       */
   OK &= init_filetable(__regina_tsd); /* Initialize the files module       */
   OK &= init_math(__regina_tsd);      /* Initialize the math module        */
   OK &= init_spec_vars(__regina_tsd); /* Initialize the interprt module    */
   OK &= init_tracing(__regina_tsd);   /* Initialize the tracing module     */
   OK &= init_builtin(__regina_tsd);   /* Initialize the builtin module     */
   OK &= init_client(__regina_tsd);    /* Initialize the client module      */
   OK &= init_library(__regina_tsd);   /* Initialize the library module     */
   OK &= init_rexxsaa(__regina_tsd);   /* Initialize the rexxsaa module     */
   OK &= init_shell(__regina_tsd);     /* Initialize the shell module       */
   OK &= init_envir(__regina_tsd);     /* Initialize the envir module       */
   OK &= init_expr(__regina_tsd);      /* Initialize the expr module        */
   OK &= init_error(__regina_tsd);     /* Initialize the error module       */
#ifdef VMS
   OK &= init_vms(__regina_tsd);       /* Initialize the vmscmd module      */
   OK &= init_vmf(__regina_tsd);       /* Initialize the vmsfuncs module    */
#endif
   OK &= init_arexxf(__regina_tsd);    /* Initialize the arexxfuncs module  */
   OK &= init_amigaf(__regina_tsd);    /* Initialize the amigafuncs module  */
   __regina_tsd->loopcnt = 1;            /* stupid r2perl-module              */
   __regina_tsd->traceparse = -1;
   __regina_tsd->thread_id = 1;

   /* Initiliaze thread specific data */
   if (!OK)
      exiterror( ERR_STORAGE_EXHAUSTED, 0 ) ;

   return(__regina_tsd);
}

void AmigaLockSemaphore(struct SignalSemaphore **semaphoreptr)
{
   if (*semaphoreptr == NULL)
   {
      Forbid();

      if (*semaphoreptr == NULL)
      {
         *semaphoreptr = AllocPooled (__regina_semaphorepool, sizeof(struct SignalSemaphore));
         InitSemaphore(*semaphoreptr);
      }

      Permit();
   }

   ObtainSemaphore(*semaphoreptr);
}

void AmigaUnlockSemaphore(struct SignalSemaphore *semaphore)
{
   assert(semaphore!=NULL);
   ReleaseSemaphore(semaphore);
}


tsd_t *__regina_get_tsd(void)
{
   struct Task *thistask = FindTask(NULL);
   tsd_node_t *node;

   D(bug("[mt_amigalib::__regina_get_tsd] thistask=%p\n", thistask));

   node = (tsd_node_t *)GetHead(__regina_tsdlist);
   while (node!=NULL && node->task!=thistask)
      node = (tsd_node_t *)GetSucc(node);
  
   D(bug("[mt_amigalib::__regina_get_tsd] node=%p\n", node));

   if (node==NULL)
   {
      /* taskdata not found */
      node = (tsd_node_t *)AllocPooled(__regina_semaphorepool, sizeof(tsd_node_t));
      node->task = thistask;
      node->TSD = ReginaInitializeThread();
      AddTail((struct List *)__regina_tsdlist, (struct Node *)node);
      D(bug("[mt_amigalib::__regina_get_tsd] new node=%p, TSD=%p\n", node, node->TSD));
   }
   else if (node->TSD==NULL) /* Was MTExit called on this task ? */
   {
      node->TSD = ReginaInitializeThread();
      D(bug("[mt_amigalib::__regina_get_tsd] new TSD=%p\n", node->TSD));
   }

   return node->TSD;
}

/* Run cleanup on closing of per-task library base */
void CloseLib(APTR base)
{
    struct Task *thistask = FindTask(NULL);
    tsd_node_t *node;

    node = (tsd_node_t *)GetHead(__regina_tsdlist);
    while (node!=NULL && node->task!=thistask)
       node = (tsd_node_t *)GetSucc(node);

    if (node!=NULL)
        cleanup(0, node);
}

ADD2CLOSELIB(CloseLib, 0);
