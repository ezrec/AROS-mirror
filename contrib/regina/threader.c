/* Asynchroneous thread multiplexer with a parallel use of the REXXSAA API.
 * This module is under construction.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <time.h>

#define INCL_RXSHV
#define INCL_RXFUNC
#define INCL_RXSYSEXIT
#define INCL_RXSUBCOM

#ifdef POSIX_THREADS
# include <sys/time.h>
# include <unistd.h>
# include <pthread.h>
#endif

#ifdef _MSC_VER
/* This picky compiler claims about unused formal parameters.
 * This is correct but hides (for human eyes) other errors since they
 * are many and we can't reduce them all.
 * Error 4100 is "unused formal parameter".
 * Error 4115 is generated in the rameter".
 */
# pragma warning(disable:4100)
#endif

#ifdef WIN32_THREADS
# define WIN32_LEAN_AND_MEAN
# include <process.h>
# include <windows.h>
# include <io.h>
#endif

#ifdef _MSC_VER
# pragma warning(default:4115)
#endif

#include "rexxsaa.h"

#define MAX_THREADS 6  /* parallel starting threads, 20 is a good maximum */
#define MAX_RUN (sizeof(unsigned) * CHAR_BIT)
#define TOTAL_THREADS 2000  /* total count of threads */

#ifdef POSIX_THREADS
#define ThreadIndexType pthread_t
#define my_threadidx() pthread_self()
#define my_threadid() pthread_self()
#define THREAD_RETURN void *
#define THREAD_CONVENTION
static pthread_t thread[MAX_THREADS];
#endif

#ifdef WIN32_THREADS
#define ThreadIndexType DWORD
#define my_threadidx() GetCurrentThreadId()
#define my_threadid() GetCurrentThread()
#define THREAD_RETURN unsigned
#define THREAD_CONVENTION __stdcall
static HANDLE thread[MAX_THREADS];
#endif

static ThreadIndexType threadx[MAX_THREADS];
static enum {Ready = 0,Running,Stopped} State[MAX_THREADS];
static unsigned found[MAX_THREADS]; /* which lines we have seen (bitwise). */
static int GlobalError = 0; /* int should be threadsafe by default */
static int stdout_is_tty = 0;
static enum {UnUsed = 0, FirstRun, DoInstore} UseInstore = UnUsed;
static void *InstoreBuf = NULL;
static unsigned InstoreLen;

static void ThreadHasStopped(unsigned position);

/* We redirect the output. This is the redirection handler. */
LONG APIENTRY instore_exit( LONG ExNum, LONG Subfun, PEXIT PBlock )
{
   RXSIOSAY_PARM *psiosay;
   char buf[256]; /* enough to hold the data */
   RXSTRING rx;
   unsigned len,tid,loop;
   int rc,idx;

   if ((ExNum != RXSIO) || (Subfun != RXSIOSAY)) /* unexpected? */
      return RXEXIT_NOT_HANDLED;

   if (GlobalError)
      return RXEXIT_HANDLED;

   psiosay = (RXSIOSAY_PARM *)PBlock;
   rx = psiosay->rxsio_string;
   if (!RXVALIDSTRING(rx))
      {
         fprintf(stderr,"\n"
                        "Thread %lu gives an invalid string for a SAY output.\n",
                        (unsigned long) my_threadidx());
         GlobalError = 1;
         return RXEXIT_HANDLED ;
      }

   len = RXSTRLEN(rx);
   if (len >= sizeof(buf))
      len = sizeof(buf) - 1;
   memcpy(buf,RXSTRPTR(rx),len);
   buf[len] = '\0'; /* We have a sscanf-able string */

   rc = sscanf(buf,"Loop %u in thread %u",&loop,&tid);
   if (rc != 2)
      {
         fprintf(stderr,"\n"
                        "Thread %lu gives an unexpected SAY output \"%s\".\n",
                        (unsigned long) my_threadidx(),buf);
         GlobalError = 1;
         return RXEXIT_HANDLED ;
      }

   if ((ThreadIndexType) tid != my_threadidx())
      {
         fprintf(stderr,"\n"
                        "Thread %lu claims an incorrect thread identifier %lu.\n",
                        (unsigned long) my_threadidx(),
                        (unsigned long) tid);
         GlobalError = 1;
         return RXEXIT_HANDLED ;
      }

   for (idx = 0;idx < MAX_THREADS;idx++)
      if (threadx[idx] == (ThreadIndexType) tid)
         break;

   if (idx >= MAX_THREADS)
      {
         fprintf(stderr,"\n"
                        "Thread %lu can't be found in the thread table.\n",
                        (unsigned long) my_threadidx());
         GlobalError = 1;
         return RXEXIT_HANDLED ;
      }

   /* Check the loop number */
   rc = 0; /* OK */
   if ((loop < 1) || (loop > MAX_RUN))
      rc = 1;
   else
   {
      loop = 1 << (loop - 1); /* Bitmask for the loop */
      if (found[idx] & loop)
         rc = 1;
      found[idx] |= loop;
   }
   if (rc)
      {
         fprintf(stderr,"\n"
                        "Thread %lu's loop doesn't run continuously.\n",
                        (unsigned long) my_threadidx());
         GlobalError = 1;
      }

   return RXEXIT_HANDLED ;
}

THREAD_RETURN THREAD_CONVENTION instore( void *data )
{
   RXSTRING Instore[2] ;
   RXSYSEXIT Exits[2] ;
   char instore_buf[256];
   int rc ;

   threadx[(unsigned) data] = my_threadidx();
   RexxRegisterExitExe( "ExitHandler",
#ifdef RX_STRONGTYPING
                        instore_exit,
#else
                        (PFN) instore_exit,
#endif
                        NULL ) ;

   Exits[0].sysexit_name = "ExitHandler" ;
   Exits[0].sysexit_code = RXSIO ;
   Exits[1].sysexit_code = RXENDLST ;

   sprintf(instore_buf,"Do i = 1 To %u;"
                       "say 'Loop' i 'in thread' gettid();"
                       "End;"
                       "Return 0",
                       MAX_RUN);
   Instore[0].strptr = instore_buf;
   Instore[0].strlength = strlen( Instore[0].strptr ) ;
   if (UseInstore == DoInstore)
   {
      Instore[1].strptr = InstoreBuf ;
      Instore[1].strlength = InstoreLen ;
   }
   else
      Instore[1].strptr = NULL ;
   rc = RexxStart( 0, NULL, "Testing", Instore, "Foo", RXCOMMAND,
                   Exits, NULL, NULL ) ;
   switch (UseInstore) {
      case UnUsed:
         if ( Instore[1].strptr )
            RexxFreeMemory( Instore[1].strptr ) ;
         else
         {
            GlobalError = 1;
            fprintf(stderr,"\n"
                           "Not got the instore macro.\n");
         }
         break;
      case FirstRun:
         if ( Instore[1].strptr )
         {
            InstoreBuf = Instore[1].strptr ;
            InstoreLen = Instore[1].strlength ;
         }
         else
         {
            GlobalError = 1;
            fprintf(stderr,"\n"
                           "Not got the instore macro.\n");
         }
         break;
      default:
         /* I don't know if the standard allows a success and a return value
          * of NULL in Instore[1]. Ignore it. It will be detected later.
          * True application should check the return code of RexxStart().
          */
         break;
   }
   RexxDeregisterExit("ExitHandler",NULL);
   ThreadHasStopped((unsigned) data);
#ifndef THREAD_RETURN_VOID
   return((THREAD_RETURN) 0);
#endif
}

void reap(unsigned position)
{
   if (found[position] != ~((unsigned) 0))
   {
      fprintf(stderr,"\n"
                     "Thread %lu has stopped without completing its loop.\n",
                     threadx[position]);
      GlobalError = 1;
   }
   found[position] = 0;
}

#ifdef WIN32_THREADS
int start_a_thread( unsigned position )
{
   int rc;
   unsigned threadID;

   State[position] = Running;
   /* Avoid some race conditions. I don't know if this is a problem of the
    * runtime system or the kernel. If the systems runs into severe swapping
    * the threads seems to run before the thread id is known which is used
    * in instore_exit. We suspend the thread until all details of the new
    * thread are known before we continue. This gives a little bit worser
    * performance.
    */
   thread[position] = (HANDLE) _beginthreadex( NULL,
                                               0,
                                               instore,
                                               (void *) position,
                                               CREATE_SUSPENDED,
                                               &threadID );
   rc = (long) thread[position] != 0l;
   if (!rc)
   {
      fprintf(stderr,"\n"
                     "Error starting thread, error code is %ld\n",
                     GetLastError() );
      GlobalError = 1;
      ThreadHasStopped(position);
   }
   ResumeThread(thread[position]);
   return(rc);
}

static void ThreadHasStopped(unsigned position)
{
   State[position] = Stopped;
}

void wait_for_threads( void )
{
   unsigned i,j,done,running;
   DWORD rc;
   HANDLE compressed[MAX_THREADS];
   unsigned BackSort[MAX_THREADS];

   running = done = MAX_THREADS;
   if (stdout_is_tty)
      printf("%u\r",MAX_THREADS); /* already started */
   if (GlobalError)
      return;

   for (;;)
   {
      for (i = 0,j = 0;i < MAX_THREADS;i++)
         if (State[i] != Ready)
         {
            compressed[j] = thread[i];
            BackSort[j] = i;
            j++;
         }
      rc = WaitForMultipleObjects(running, compressed, FALSE, 3000);
      if (rc == 0xFFFFFFFF)
      { /* Failure or dead thread, look for a stopped one */
         for (i = 0;i < running;i++)
            if (State[BackSort[i]] == Stopped)
               rc = WAIT_OBJECT_0 + i;
      }
      if ((rc < WAIT_OBJECT_0) || (rc >= running + WAIT_OBJECT_0))
      {
         fprintf(stderr,"\n"
                        "At least one thread won't finish normally within 3 seconds (rc=%u,error=%lu).\n",
                        rc,GetLastError());
         GlobalError = 1;
      }
      if (GlobalError)
         break;
      i = BackSort[rc - WAIT_OBJECT_0];
      if (State[i] != Stopped)
      {
         fprintf(stderr,"\n"
                        "Thread %u hasn't finished normally.\n");
         GlobalError = 1;
         break;
      }
      CloseHandle(thread[i]);
      State[i] = Ready;
      running--;
      reap(i);
      if (done < TOTAL_THREADS)
      {
         if (!start_a_thread(i))
            break;
         done++;
         running++;
      }
      if (stdout_is_tty)
         printf("%u(%u)\r",done,running);
      if (GlobalError || !running)
         break;
   }
}
#endif

#ifdef POSIX_THREADS
static pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t something_stopped = PTHREAD_COND_INITIALIZER;

int start_a_thread( unsigned position )
{
   int rc;

   State[position] = Running;
   rc = pthread_create( &thread[position], NULL, instore, (void *) position );
   if (rc)
   {
      fprintf(stderr,"\n"
                     "Error starting thread, error code is %d\n",
                     rc );
      GlobalError = 1;
      ThreadHasStopped(position);
   }
   return(!rc);
}

static void ThreadHasStopped(unsigned position)
{
   pthread_mutex_lock(&thread_lock);
   State[position] = Stopped;
   pthread_cond_signal(&something_stopped);
   pthread_mutex_unlock(&thread_lock);
}

void wait_for_threads( void )
{
   unsigned i,done,running;
   int rc;
   struct timeval now;
   struct timespec timeout;

   running = done = MAX_THREADS;
   if (stdout_is_tty)
      printf("%u\r",MAX_THREADS); /* already started */

   if (GlobalError)
      return;

   pthread_mutex_lock(&thread_lock);
   for (;;)
   {
      gettimeofday(&now,NULL);
      timeout.tv_sec = now.tv_sec + 3;
      timeout.tv_nsec = now.tv_usec * 1000;
      rc = pthread_cond_timedwait(&something_stopped,&thread_lock,&timeout);
      if (rc == ETIMEDOUT)
      {
         fprintf(stderr,"\n"
                        "At least one thread won't finish within 3 seconds.\n");
         GlobalError = 1;
      }
      if (GlobalError)
         break;
      for ( i = 0; i < MAX_THREADS; i++ )
      {
         if (State[i] != Stopped)
            continue;
         State[i] = Ready;
         running--;
         rc = pthread_join( thread[i], NULL );
         if (rc != 0)
         {
            fprintf(stderr,"\n"
                           "A thread can't be found in the internal table.\n");
            GlobalError = 1;
            break;
         }
         reap(i);
         if (done < TOTAL_THREADS)
         {
            if (!start_a_thread(i))
               break;
            done++;
            running++;
         }
      }
      if (stdout_is_tty)
         printf("%u(%u)\r",done,running);
      if (GlobalError || !running)
         break;
      usleep(10);
   }
   pthread_mutex_unlock(&thread_lock);
}
#endif

static void usage(void)
{
   printf("usage: threader [-p]\n"
          "\n"
          "Options:\n"
          "-p\tLoad the macro only once and then use the generated instore\n"
          "\tmacro. Default: Always load the macro new.\n"
          "\n"
          "threader uses a macro which generated lines with numbers which can\n"
          "be parsed to detect problems in the multi-threading implementation.\n"
          "A loop counter runs until %u. The test should run within a few\n"
          "seconds until a few minutes. You should hit ^C to abort the program\n"
          "if you think your harddisk is working heavy.\n"
          "\n"
          "This program is for testing purpose only.\n"
          ,TOTAL_THREADS);
   exit(1);
}

int main( int argc, char *argv[] )
{
   RXSTRING version;
   ULONG versioncode;
   int i;
   time_t start;

   if (isatty(fileno(stdout)))
      stdout_is_tty = 1;

   if (argc > 2)
      usage();
   if (argc > 1)
   {
      if (strcmp(argv[1],"-p") == 0)
         UseInstore = FirstRun;
        else
         usage();
   }

   memset(found,0,sizeof(found));
   memset(State,0,sizeof(State));
   /* We want to see surprisings at once: */
   setvbuf(stdout,NULL,_IONBF,0);
   setvbuf(stderr,NULL,_IONBF,0);
   printf( "Regina Rexx Thread Tester\n" );
   printf( "-------------------------\n" );

   version.strlength = 0;
   version.strptr = NULL;
   /* This might not work if we check another Rexx: */
   versioncode = ReginaVersion(&version);
   printf("Regina's version is %lu.%02lu",
          versioncode >> 8,
          versioncode & 0xFF);
   if (version.strptr)
   {
      printf(" (in complete \"%s\")",version.strptr);
      RexxFreeMemory(version.strptr);
   }
   printf("\n");

   if (UseInstore)
   {
      State[0] = Running;
      thread[0] = my_threadid();
      threadx[0] = my_threadidx();
      instore(NULL);
      State[0] = Stopped;
      found[0] = 0;
      UseInstore = DoInstore;
   }

   if (stdout_is_tty)
      printf("\n"
             "You should see a loop counter which stops at %u.\n\n",
             TOTAL_THREADS);

   start = time(NULL);
   for ( i = 0; i < MAX_THREADS; i++ )
      if (!start_a_thread(i) || GlobalError)
         break;

   if (!GlobalError)
      wait_for_threads();

   if (GlobalError)
   {
      fprintf(stderr,"\n"
                     "An error encountered. Do you use the right shared "
                                                            "libs or DLLs?\n");
      return 1;
   }

   printf("\n"
          "Thread tester passed without an error.\n"
          "About %u seconds used for %u cyles.\n",
          (unsigned) (time(NULL) - start),TOTAL_THREADS);

   printf("Press ENTER to continue...\n");
   {
      char buf[128];
      fgets(buf,sizeof(buf),stdin);
   }
   return 0 ;
}
