#ifndef lint
static char *RCSid = "$Id$";
#endif

/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1992-1994  Anders Christensen <anders@pvv.unit.no>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * This process runs as a daemon (or NT service). It maintains multiple,
 * named Rexx queues.
 * All communication is done via TCP/IP sockets.
 * This process waits on a known port; 5656 by default for connections
 * from clients. A client is any process that respects the Interface
 * defined below. The "normal" clients are regina and rxqueue.
 * Details about each client is kept, like current queue name.
 *
 * Structure
 * ---------
 * startup
 * - set signal handler for SIGTERM
 * initialise socket interface
 * - socket()
 * - bind()
 * - listen()
 * loop until killed
 * - setup read FDs
 * - select()
 * - if listen socket, add new client
 * - otherwise read command
 * cleanup
 * - disconnect all clients
 * - free up resources
 *
 * Interface.
 * ---------
 * Once a client connects, it sends commands
 * Commands are single character, followed by optional 6 hex character
 * length and optional data.
 *   F - queue data onto client's current queue (FIFO)
 *       in -> FFFFFFFxxx--data--xxx
 *       out-> 0000000 (if successful)
 *       out-> 2xxxxxx (if error, eg queue deleted)
 *       regina QUEUE, rxqueue /fifo
 *   L - push data onto client's current queue (LIFO)
 *       in->  LFFFFFFxxx--data--xxx
 *       out-> 0000000 (if successful)
 *       out-> 2xxxxxx (if error, eg queue deleted)
 *       regina PUSH, rxqueue /lifo
 *   C - create queue
 *       in->  CFFFFFFxxx--queue name--xxx (if length 0, create name)
 *       out-> 0xxxxxx (if queue name created - length ignored)
 *       out-> 1FFFFFFxxx--queue name--xxx (if queue name existed or was not specified)
 *       out-> 2xxxxxx (if error)
 *       regina RXQUEUE('C'), rxqueue N/A
 *   D - delete queue
 *       in->  DFFFFFFxxx--queue name--xxx
 *       out-> 0000000 (if queue name deleted)
 *       out-> 2xxxxxx (if error, eg queue already deleted)
 *       out-> 9xxxxxx (trying to delete 'SESSION' queue)
 *       regina RXQUEUE('D'), rxqueue N/A
 *   E - empty data from specified queue
 *       in->  KFFFFFFxxx--queue name--xxx
 *       out-> 0000000 (if queue emptied)
 *       out-> 2xxxxxx (if error, eg queue deleted)
 *       regina N/A, rxqueue /clear
 *   P - pop item off client's default queue
 *       in->  P000000
 *       out-> 0FFFFFFxxx--data--xxx (if queue name existed)
 *       out-> 1000000 (if queue empty)
 *       out-> 2xxxxxx (if queue name deleted - length ignored)
 *       out-> 4xxxxxx (if timeout on queue exceeded - length ignored)
 *       regina PULL, rxqueue N/A
 *   S - set default queue name (allow false queues)
 *       in->  SFFFFFFxxx--queue name--xxx
 *       out-> 0000000 (if successful)
 *       out-> 2xxxxxx (if error)
 *       regina RXQUEUE('S'), rxqueue N/A
 *   G - get default queue name
 *       in->  G000000
 *       out-> 0FFFFFFxxx--queue name--xxx
 *       regina RXQUEUE('G'), rxqueue N/A
 *   N - return number of lines on stack
 *       in->  N000000
 *       out-> 0FFFFFF (if queue exists)
 *       out-> 2xxxxxx (if error or queue doesn't exist - length ignored)
 *       regina QUEUED(), rxqueue N/A
 *   T - set timeout on queue pull
 *       in->  DFFFFFFTTTTTT
 *       out-> 0000000 (if queue timeout set)
 *       regina RXQUEUE('T'), rxqueue N/A
 *   W - write client's temporary stack to "real" stack
 *       in->  W000000
 *       out-> 0FFFFFF (if queue exists)
 *       out-> 2xxxxxx (if error or queue doesn't exist - length ignored)
 *       regina QUEUED(), rxqueue N/A
 *   X - client disconnect
 *       in->  X000000
 *       out-> 
 *   Z - client requests shutdown - should only be called by ourselves!!
 *       in->  Z000000
 *       out-> 
 */
#define MAX_CLIENTS 100

/*
#define FD_SETSIZE MAX_CLIENTS+1
*/
#include "rexx.h"

#ifdef WIN32
# if defined(_MSC_VER)
#  if _MSC_VER >= 1100
/* Stupid MSC can't compile own headers without warning at least in VC 5.0 */
#   pragma warning(disable: 4115 4201 4214)
#  endif
#  include <windows.h>
#  if _MSC_VER >= 1100
#   pragma warning(default: 4115 4201 4214)
#  endif
# else
#  include <windows.h>
# endif
# include <io.h>
#else
# ifdef HAVE_SYS_SOCKET_H
#  include <sys/socket.h>
# endif
# ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#  endif
# ifdef HAVE_SYS_SELECT_H
#  include <sys/select.h>
# endif
# ifdef HAVE_NETDB_H
#  include <netdb.h>
# endif
# ifdef HAVE_ARPA_INET_H
#  include <arpa/inet.h>
# endif
#endif
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_PROCESS_H
# include <process.h>
#endif

#if defined(TIME_WITH_SYS_TIME)
# include <sys/time.h>
# include <time.h>
#else
# if defined(HAVE_SYS_TIME_H)
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#define HAVE_FORK
#if !defined(__WATCOMC__) && !defined(_MSC_VER)  && !(defined(__IBMC__) && defined(WIN32)) && !defined(__SASC) && !defined(__MINGW32__) && !defined(__BORLANDC__)
# undef HAVE_FORK
#endif
#if defined(__WATCOMC__) && defined(__QNX__)
# define HAVE_FORK
#endif

#include "extstack.h"

#ifndef NDEBUG
# define DEBUGDUMP(x) {x;}
#else
# define DEBUGDUMP(x) {}
#endif

#ifdef BUILD_NT_SERVICE
# include "service.h"
/*
 * this event is signalled when the
 * service should end
 */
HANDLE  hServerStopEvent = NULL;
#endif

#define NUMBER_QUEUES 100

/*
 * Structure for multiple queues
 * All clients share the queues. Do they also share the
 * temporary stack ? yes I think so TBD
 */
typedef struct stacklinestruct *stacklineptr ;

typedef struct stacklinestruct
{
   stacklineptr next, prev ;
   streng *contents ;
} stackline ;

/*
 * Pointers to queue names
 */
streng *queuename[NUMBER_QUEUES] ;
/*
 * Indicates if the queue is a "real" queue
 * or a false queue as a result of a rxqueue('set', 'qname')
 * on a queue that doesn't exist. This is crap behaviour!
 * but that's how Object Rexx works :-(
 */
int real_queue[NUMBER_QUEUES];
/*
 * Pointers to first and last line in the stack.
 */
stacklineptr lastline[NUMBER_QUEUES] ;
stacklineptr firstline[NUMBER_QUEUES] ;

/*
 * Structure for multiple clients
 */
typedef struct
{
   int socket;
   int default_queue;
   /*
    * Pointers to first and last entry in the temporary stack
    * don't use this yet; if ever
    */
   stacklineptr firstbox;
   stacklineptr lastbox;
   long queue_timeout;
} CLIENT;
/*
 * Linked list would be better for client - TBD
 */
CLIENT clients[MAX_CLIENTS];

int running = 1;
int allclean = 0;
time_t base_secs; /* the time the process started */

#if !defined(HAVE_STRERROR)
/*
 * Sigh! This must probably be done this way, although it's incredibly
 * backwards. Some versions of gcc comes with a complete set of ANSI C
 * include files, which contains the definition of strerror(). However,
 * that function does not exist in the default libraries of SunOS.
 * To circumvent that problem, strerror() is #define'd to get_sys_errlist()
 * in config.h, and here follows the definition of that function.
 * Originally, strerror() was #defined to sys_errlist[x], but that does
 * not work if string.h contains a declaration of the (non-existing)
 * function strerror().
 *
 * So, this is a mismatch between the include files and the library, and
 * it should not create problems for Regina. However, the _user_ will not
 * encounter any problems until he compiles Regina, so we'll have to
 * clean up after a buggy installation of the C compiler!
 */
const char *get_sys_errlist( int num )
{
   extern char *sys_errlist[] ;
   return sys_errlist[num] ;
}
#endif

time_t getmsecs( void )
{
   time_t secs,usecs;
#if defined(HAVE_GETTIMEOFDAY)
   struct timeval times ;

   gettimeofday(&times, NULL) ;
   secs = times.tv_sec - base_secs;
   usecs = times.tv_usec ;

   DEBUGDUMP(printf("getmsecs() secs: %ld usecs: %ld\n", secs, usecs););

   if (times.tv_usec < 0)
   {
      usecs = (times.tv_usec + 1000000) ;
      secs = times.tv_sec - 1 ;
   }

#elif defined(HAVE_FTIME)
   struct timeb timebuffer;

   ftime(&timebuffer);
   secs = timebuffer.time - base_secs;
   usecs = timebuffer.millitm * 1000;
#else
   secs = time(NULL) - base_secs;
   usecs = 0 ;
#endif
   return ( (secs*1000000)+usecs) / 1000;
}

streng *Streng_upper( streng *str )
{
   int i;

   for ( i = 0; i < PSTRENGLEN( str ); i++ )
   {
      if ( islower( str->value[i] ) )
         str->value[i] = (char)toupper( str->value[i] );
   }
   return str;
}

int Str_ccmp( const streng *first, const streng *second )
{
   int tmp=0 ;

   if ( PSTRENGLEN( first ) != PSTRENGLEN( second ) )
      return 1 ;

   for (tmp=0; tmp < PSTRENGLEN( first ); tmp++ )
      if ( tolower( first->value[tmp] ) != tolower( second->value[tmp] ) )
         return 1 ;

   return 0 ;
}

void delete_a_queue( int idx )
{
   stacklineptr ptr=firstline[idx],nptr;
   int i;

   while ( ptr )
   {
      if (ptr->contents)
         DROPSTRENG(ptr->contents) ;
      nptr = ptr->next ;
      free( ptr );
      ptr = nptr;
   }
   DROPSTRENG( queuename[idx] );
   real_queue[idx] = 0;
   firstline[idx] = lastline[idx] = NULL;
   queuename[idx] = NULL;
   /*
    * OS/2 Rexx seems to reset the queue to SESSION for each client
    * that has the queue we are just deleting as their current
    * queue
    */
   for( i = 0; i < MAX_CLIENTS; i++ )
   {
      if ( clients[i].default_queue == idx )
         clients[i].default_queue = 0;
   }
}

void delete_all_queues( void )
{
   int i;
   for ( i = 0; i < NUMBER_QUEUES; i++ )
   {
      if ( firstline[i] )
         delete_a_queue( i );
   }
}

char *get_unspecified_queue( void )
{
   char *rxq = getenv( "RXQUEUE" );

   if ( rxq == NULL )
      rxq = "SESSION";
   return rxq;
}

int suicide( void )
{
   int sock;
   streng *queue;
   char *in_queue=get_unspecified_queue();
   streng *server_name;
   int server_address,portno;
#ifdef WIN32
   if ( init_external_queue( NULL ) )
      return 1;
#endif

   queue = MAKESTRENG( strlen( in_queue ) );
   if ( queue  == NULL )
   {
      showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
      return(ERR_STORAGE_EXHAUSTED);
   }
   memcpy( PSTRENGVAL( queue ), in_queue, PSTRENGLEN( queue ) );

   if ( parse_queue( NULL, queue, &server_name, &server_address, &portno ) == 0 )
   {
      sock = connect_to_rxstack( NULL, portno, server_name, server_address );
      if ( sock < 0 )
      {
         showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_CANT_CONNECT, ERR_RXSTACK_CANT_CONNECT_TMPL, server_name, portno, strerror( errno) );
         return(ERR_RXSTACK_CANT_CONNECT);
      }
      send_command_to_rxstack( NULL, sock, RXSTACK_KILL_STR, NULL, 0 );
      read_result_from_rxstack( NULL, sock, RXSTACK_HEADER_SIZE );
      close(sock);
   }
   return 0;
}

int rxstack_cleanup( void )
{
   if ( !allclean )
   {
      DEBUGDUMP(printf("Cleaning up\n"););
      /*
       * Disconnect all clients
       * Delete all clients
       */
#ifdef WIN32
      term_external_queue();
#endif
      delete_all_queues();
      DEBUGDUMP(printf("Finished Cleaning up\n"););
      allclean = 1;
   }
   return 0;
}

#ifdef BUILD_NT_SERVICE
BOOL report_service_start( void )
{
   /* 
    * report the status to the service control manager.
    */
   return (ReportStatusToSCMgr(
           SERVICE_RUNNING,       /* service state */
           NO_ERROR,              /* exit code */
           0));                   /* wait hint */
}

BOOL report_service_pending_start( void )
{
   /* 
    * report the status to the service control manager.
    */
   return (ReportStatusToSCMgr(
           SERVICE_START_PENDING, /* service state */
           NO_ERROR,              /* exit code */
           3000));                /* wait hint */
}

int nt_service_start( void )
{
   /*
    * code copied from sample NT Service code. The goto's are
    * not mine!!
    * report the status to the service control manager.
    */
   if ( !report_service_pending_start() )
      goto cleanupper;

   /*
    * create the event object. The control handler function signals
    * this event when it receives the "stop" control code.
    */
   hServerStopEvent = CreateEvent(
        NULL,    /* no security attributes */
        TRUE,    /* manual reset event */
        FALSE,   /* not-signalled */
        NULL);   /* no name */

   if ( hServerStopEvent == NULL)
      goto cleanupper;

   /* 
    * report the status to the service control manager.
    */
   if ( !report_service_pending_start() )
      goto cleanupper;

    return 0;
cleanupper:
    return 1;
}

VOID ServiceStop()
{
   DEBUGDUMP(printf("In ServiceStop()\n"););
   suicide();
/*
   running = 0;
*/
}
#endif

void rxstack_signal_handler( int sig )
{
   running = 0;
}

int find_free_client( )
{
   int i;
   for ( i = 0; i < MAX_CLIENTS; i++ )
   {
      if ( clients[i].socket == 0 )
         return i;
   }
   return -1;
}

/*
 * Find the named queue - case insensitive
 */
int find_queue( streng *queue_name )
{
   int i;

   for ( i = 0; i < NUMBER_QUEUES; i++ ) /* inefficient !! */
   {
      if ( queuename[i]
      &&   Str_ccmp( queuename[i], queue_name ) == 0 )
         return i;
   }
   return UNKNOWN_QUEUE;
}

int find_free_slot( void )
{
   int i,idx = UNKNOWN_QUEUE;

   for ( i = 0; i < NUMBER_QUEUES; i++ )
   {
      if ( queuename[i] == NULL )
      {
         idx = i;
         break;
      }
   }
   return idx;
}

int rxstack_delete_queue( CLIENT *client, streng *queue_name )
{
   int idx,rc;

   if ( ( idx = find_queue( queue_name ) ) == UNKNOWN_QUEUE )
   {
      rc = 9;
   }
   else
   {
      if ( idx == 0 )
         rc = 5;
      else
      {
         /*
          * If we found a false queue, return 9
          */
         if ( real_queue[idx] == 0 )
            rc = 9;
         else
         {
            /*
             * Delete the contents of the queue
             * and mark it as gone.
             */
            delete_a_queue( idx );
            rc = 0;
         }
      }
   }
   return 0;
}

int rxstack_create_client( int socket )
{
   int i;
   i = find_free_client();
   if ( i == -1 )
   {
      /* TBD */
   }
   else
   {
      clients[i].default_queue = 0;
      clients[i].socket = socket;
      clients[i].queue_timeout = 0;
   }
   return 0;
}

int rxstack_send_return( int sock, char *action, char *str, int len )
{
   streng *qlen, *header;
   int rc;

   DEBUGDUMP(printf("Sending to %d Result: %s <%s>\n", sock, action, (str) ? str : ""););
   qlen = REXX_D2X( len );
   if ( qlen )
   {
      header = REXX_RIGHT( qlen, RXSTACK_HEADER_SIZE, '0');
      DROPSTRENG( qlen );
      if ( header )
      {
         header->value[0] = action[0];
         rc = send( sock, PSTRENGVAL(header), PSTRENGLEN(header), 0 );
         DEBUGDUMP(printf("Send length: %s(%d) rc %d\n", PSTRENGVAL(header),PSTRENGLEN(header),rc););
         if ( str )
         {
            rc = send( sock, str, len, 0 );
            DEBUGDUMP(printf("Send str <%s> length %d rc: %d\n", str, len, rc););
         }
         DROPSTRENG( header );
      }
   }
   return 0;
}

int rxstack_delete_client( CLIENT *client )
{
   close( client->socket );
   client->socket = 0;
   client->default_queue = 0;
   return 0;
}

int rxstack_set_default_queue( CLIENT *client, streng *data )
{
   int idx,rc;
   streng *new_queue;

   rc = client->default_queue;
   if ( ( idx = find_queue( data ) ) == UNKNOWN_QUEUE )
   {
      /*
       * We didn't find a real or a false queue, so create
       * a false queue
       */
      idx = find_free_slot( );
      if ( idx == UNKNOWN_QUEUE )
         rc = -1;
      else
      {
         new_queue = MAKESTRENG( PSTRENGLEN( data ) );
         if ( new_queue )
         {
            memcpy( PSTRENGVAL( new_queue ), PSTRENGVAL( data ), PSTRENGLEN( new_queue ) );
         }
         else
         {
            showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
            exit( ERR_STORAGE_EXHAUSTED );
         }
         queuename[idx] = Streng_upper( new_queue ) ;
         client->default_queue = idx;
         real_queue[idx] = 0; /* false queue */
      }
   }
   else
   {
      client->default_queue = idx;
   }

   if ( rc == -1 )
   {
      DEBUGDUMP(printf("No FREE SLOTS when setting default queue for client: <%s>\n", PSTRENGVAL(data) ););
   }
   else
   {
      DEBUGDUMP(printf("Setting default queue for client: <%s> Prev: %d <%s>\n", PSTRENGVAL(data), rc, PSTRENGVAL(queuename[rc]) ););
   }
   return rc;
}

int rxstack_timeout_queue( CLIENT *client, streng *data )
{
   streng *timeout;
   /*
    * Extract the timeout value from the incoming buffer
    */
   timeout = MakeStreng( PSTRENGLEN( data ) );
   if ( timeout == NULL )
   {
      showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
      exit( ERR_STORAGE_EXHAUSTED );
   }
   memcpy( PSTRENGVAL(timeout), PSTRENGVAL( data ), PSTRENGLEN( data ) );
   /*
    * Convert the timeout
    * If the supplied timeout is 0 (infinite wait), set the client->queue_timeout
    * to -1.
    */
   client->queue_timeout = REXX_X2D( timeout );
   if ( client->queue_timeout == 0 )
   {
      client->queue_timeout = -1;
   }
   DEBUGDUMP(printf("Timeout on queue: %ld\n", client->queue_timeout ););
   DROPSTRENG( timeout );

   return 0;
}

int rxstack_create_queue( CLIENT *client, streng *data )
{
   int rc=0,length;
   int idx=UNKNOWN_QUEUE,false_queue_idx=UNKNOWN_QUEUE;
   char buf[50];
   streng *new_queue;

   if ( data )
   {
      DEBUGDUMP(printf("Creating new user-specified queue: <%s>\n", PSTRENGVAL(data) ););
      if ( ( idx = find_queue( data ) ) == UNKNOWN_QUEUE )
      {
         /*
          * No queue of that name, so use it.
          */
         DEBUGDUMP(printf("Couldn't find <%s>; so creating it\n", PSTRENGVAL(data) ););
         new_queue = MAKESTRENG( PSTRENGLEN( data ));
         if ( new_queue )
         {
            memcpy( PSTRENGVAL( new_queue ), PSTRENGVAL( data ), PSTRENGLEN( new_queue ) );
         }
         else
         {
            showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
            exit( ERR_STORAGE_EXHAUSTED );
         }
      }
      else
      {
         /*
          * If the queue we found is a false queue, we can still
          * use the supplied name and the slot
          */
         if ( real_queue[idx] == 0 )
         {
            DEBUGDUMP(printf("Found false queue\n" ););
            new_queue = MAKESTRENG( PSTRENGLEN( data ));
            if ( new_queue )
            {
               memcpy( PSTRENGVAL( new_queue ), PSTRENGVAL( data ), PSTRENGLEN( new_queue ) );
            }
            else
            {
               showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
               exit( ERR_STORAGE_EXHAUSTED );
            }
            false_queue_idx = idx;
         }
         else
         {
            /*
             * Create a unique queue name
             */
            sprintf(buf,"S%d%ld%d", getpid(), clock(), find_free_slot( ) );
            DEBUGDUMP(printf("Having to create unique queue <%s>\n", buf ););
            length = strlen( buf );
            new_queue = MAKESTRENG( length );
            if ( new_queue )
            {
               memcpy( PSTRENGVAL( new_queue ), buf, length );
               new_queue->len = length;
            }
            else
            {
               showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
               exit( ERR_STORAGE_EXHAUSTED );
            }
         }
      }
      rc = 0;
   }
   else
   {
      DEBUGDUMP(printf("Creating system generated queue.\n"););
      /*
       * Create a unique queue name
       */
      sprintf(buf,"S%d%ld%d", getpid(), clock(), find_free_slot( ) );
      length = strlen( buf );
      new_queue = MAKESTRENG( length );
      if ( new_queue )
      {
         memcpy( PSTRENGVAL( new_queue ), buf, length );
         new_queue->len = length;
         rc = 1;
      }
      else
      {
         showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
         exit( ERR_STORAGE_EXHAUSTED );
      }
   }
   /*
    * Find the first slot that hasn't been used, or use
    * the false queue
    */
   if ( false_queue_idx == UNKNOWN_QUEUE )
      idx = find_free_slot( );
   else
      idx = false_queue_idx;
   
   if ( idx == UNKNOWN_QUEUE )
   {
      showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_TOO_MANY_QUEUES, ERR_RXSTACK_TOO_MANY_QUEUES_TMPL, NUMBER_QUEUES );
      return ERR_RXSTACK_TOO_MANY_QUEUES;
   }
   if ( queuename[idx] )
      DROPSTRENG( queuename[idx] );
   /*
    * Uppercase the queue name 
    */
   queuename[idx] = Streng_upper( new_queue );
   client->default_queue = idx;
   real_queue[idx] = 1;

   return rc;
}

/*
 * Pushes 'line' onto the REXX stack, lifo, and sets 'lastline' to
 *    point to the new line. The line is put on top of the current
 *    buffer.
 */
stacklineptr rxstack_stack_lifo( int current_queue, streng *line )
{
   stacklineptr newbox=NULL ;

   newbox = (stacklineptr)malloc( sizeof(stackline) ) ;
   if ( newbox )
   {
      if ( lastline[current_queue])
      {
         lastline[current_queue]->next = newbox ;
         newbox->prev = lastline[current_queue] ;
      }
      else
      {
         newbox->prev = NULL ;
         firstline[current_queue] = newbox ;
      }
   
      newbox->next = NULL ;
      newbox->contents = line ;
      lastline[current_queue] = newbox ;
   }
   return newbox;
}


/*
 * Puts 'line' on the REXX stack, fifo. This routine is similar to
 *    stack_lifo but the differences are big enough to have a separate
 *    routine. The line is put in the bottom of the current buffer,
 *    that is just above the uppermost buffer mark, or at the bottom
 *    of the stack, if there are no buffer marks.
 */
stacklineptr rxstack_stack_fifo( int current_queue, streng *line )
{
   stacklineptr newbox=NULL, ptr=NULL ;

   newbox = (stacklineptr)malloc( sizeof(stackline) ) ;
   if ( newbox )
   {
      newbox->prev = newbox->next = NULL ;
      newbox->contents = line ;
   
      for (ptr=lastline[current_queue];(ptr)&&(ptr->contents);ptr=ptr->prev) ;
   
      if (ptr)
      {
         newbox->prev = ptr ;
         newbox->next = ptr->next ;
         if (ptr->next)
            ptr->next->prev = newbox ;
         else
            lastline[current_queue] = newbox ;
         ptr->next = newbox ;
      }
      else
      {
         newbox->next = firstline[current_queue] ;
         firstline[current_queue] = newbox ;
         if (newbox->next)
            newbox->next->prev = newbox ;
         if (!lastline[current_queue])
            lastline[current_queue] = newbox ;
      }
   }
   return newbox;
}

int rxstack_queue_data( CLIENT *client, streng *data, char order )
{
   int rc = 0;
   streng *line;

   if ( data == NULL )
      line = MakeStreng ( 0 );
   else
      line = data;
   DEBUGDUMP(printf("Queueing: <%s> Order: %c\n", (PSTRENGVAL(line)) ? PSTRENGVAL(line) : "", order ););
   if ( order == RXSTACK_QUEUE_FIFO )
   {
      if ( rxstack_stack_fifo( client->default_queue, line ) == NULL )
         rc = 1;
   }
   else
   {
      if ( rxstack_stack_lifo( client->default_queue, line ) == NULL )
         rc = 1;
   }
   return rc;
}

int rxstack_empty_queue( CLIENT *client, streng *data )
{
   int rc;
   stacklineptr tmp,line;
   streng *contents;

   DEBUGDUMP(printf("Emptying queue: <%s>\n", (PSTRENGVAL(data)) ? PSTRENGVAL(data) : "" ););
   rc = rxstack_set_default_queue( client, data );
   if ( rc == -1 )
      rc = 2;
   else
   {
      for ( line = firstline[client->default_queue]; line != NULL; )
      {
         contents = line->contents ;
         DROPSTRENG( contents );
         tmp = line->next;
         free( line ) ;
         line = tmp->next;
      }
      firstline[client->default_queue] = lastline[client->default_queue] = NULL;
   }
   return rc;
}

int rxstack_number_in_queue( CLIENT *client )
{
   stacklineptr ptr ;
   int lines=0 ;

   ptr = firstline[client->default_queue] ;
   for ( lines = 0; ptr; ptr = ptr->next )
   {
      lines++ ;
   }

   DEBUGDUMP(printf("Querying number in queue: %d\n", lines ););
   return lines ;
}

/*
 * Gets the line off the queue but doesn't delete it
 */
int rxstack_get_line_off_queue( CLIENT *client, streng **result )
{
   /*
    * Return 0 if line OK, 1 if queue empty, 2 if error, 3 if timeout, 4 if still waiting
    */
   int rc;
   stacklineptr line=NULL ;

   /*
    * test here to see if client's default queue is valid
    */
   if ( ( line = lastline[client->default_queue] ) != NULL )
   {
      *result = MAKESTRENG( line->contents->len );
      if ( *result )
      {
         memcpy( (*result)->value, line->contents->value, line->contents->len );
         rc = 0;
      }
      else
         rc = 2;
   }
   else
   {
      if ( client->queue_timeout )
         rc = 3; /* waiting */
      else
         rc = 1; /* queue empty */
   }
   DEBUGDUMP(printf("Pulling line off queue; rc %d\n", rc ););
   return rc;
}

int rxstack_delete_line_off_queue( CLIENT *client )
{
   /*
    * Return 0 if line OK, 1 if queue empty, 2 if error
    */
   int rc;
   streng *contents=NULL ;
   stacklineptr line=NULL ;

   /*
    * test here to see if client's default queue is valid
    */
   DEBUGDUMP(printf("Deleting line off queue\n" ););
   if ( ( line = lastline[client->default_queue] ) != NULL )
   {
      contents = line->contents ;
      DROPSTRENG( contents );
      lastline[client->default_queue] = line->prev ;
      if ( !line->prev )
         firstline[client->default_queue] = NULL ;
      else
         line->prev->next = NULL ;

      free( line ) ;
      rc = 0;
   }
   else
      rc = 1;
   return rc;
}


int rxstack_process_command( CLIENT *client )
{
   char cheader[RXSTACK_HEADER_SIZE];
   streng *header;
   streng *buffer;
   streng *result=NULL;
   int rc,length;
   char rcode[2];

   rcode[1] = '\0';
   memset( cheader, 0, sizeof(cheader) );
   DEBUGDUMP(printf("reading from socket %d\n", client->socket););
   rc = recv( client->socket, cheader, RXSTACK_HEADER_SIZE, 0 );
   if ( rc < 0 )
   {
      showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_READING_SOCKET, ERR_RXSTACK_READING_SOCKET_TMPL, strerror( errno ) );
      /*
       * Assume client has been lost
       */
      rxstack_delete_client( client );
   }
   else if ( rc == 0 )
   {
      DEBUGDUMP(printf("read empty header\n"););
      /*
       * Assume client has been lost
       */
      rxstack_delete_client( client );
   }
   else
   {
      header = MakeStreng( RXSTACK_HEADER_SIZE - 1 );
      if ( header == NULL )
      {
         showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
         exit( ERR_STORAGE_EXHAUSTED );
      }
      memcpy( PSTRENGVAL(header), cheader+1, RXSTACK_HEADER_SIZE-1 );
      buffer = NULL;
      /*
       * Convert the data length
       */
      length = REXX_X2D( header );
      DROPSTRENG( header );
      DEBUGDUMP(printf("Header: <%s> length: %d\n", header->value, length););
      if ( length )
      {
         /*
          * Allocate a streng big enough for the expected data
          * string, based on the length just read; even if the length
          * is zero
          */
         buffer = MakeStreng ( length );
         if ( buffer == NULL )
         {
            showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
            exit( ERR_STORAGE_EXHAUSTED );
         }
         rc = recv( client->socket, PSTRENGVAL(buffer), PSTRENGLEN(buffer), 0 );
         if ( rc < 0 )
         {
            showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_READING_SOCKET, ERR_RXSTACK_READING_SOCKET_TMPL, strerror( errno ) );
         }
         else if ( rc == 0 )
         {
            /*
             * All we can assume here is that the client has been lost
             */
            DEBUGDUMP(printf("read empty header\n"););
            rxstack_delete_client( client );
            cheader[0] = '?';
         }
      }

      switch( cheader[0] )
      {
         case RXSTACK_QUEUE_FIFO:
         case RXSTACK_QUEUE_LIFO:
            DEBUGDUMP(printf("--- Queue %s ---\n", cheader[0] == RXSTACK_QUEUE_FIFO ? "FIFO" : "LIFO"););
            rxstack_queue_data( client, buffer, cheader[0] );
            rxstack_send_return( client->socket, "0", NULL, 0 );
            break;
         case RXSTACK_EXIT:
            DEBUGDUMP(printf("--- Exit ---\n"););
            /*
             * Client has requested disconnect, so remove all
             * references to the client
             */
            rxstack_send_return( client->socket, "0", NULL, 0 );
            rxstack_delete_client( client );
            break;
         case RXSTACK_KILL:
            DEBUGDUMP(printf("--- Kill ---\n"););
            /*
             * Client has requested server to stop
             */
            rxstack_send_return( client->socket, "0", NULL, 0 );
            rxstack_delete_client( client );
            running = 0;
            return 0;
         case RXSTACK_SET_QUEUE:
            DEBUGDUMP(printf("--- Set Queue ---\n"););
            /*
             * Set the default queue for the client
             */
            rc = rxstack_set_default_queue( client, buffer );
            if ( rc == -1 )
               rxstack_send_return( client->socket, "2", NULL, 0 );
            else
               rxstack_send_return( client->socket, "0", (queuename[rc])->value, (queuename[rc])->len );
            DROPSTRENG( buffer );
            break;
         case RXSTACK_EMPTY_QUEUE:
            DEBUGDUMP(printf("--- Empty Queue ---\n"););
            rxstack_empty_queue( client, buffer );
            rxstack_send_return( client->socket, "0", NULL, 0 );
            break;
         case RXSTACK_NUMBER_IN_QUEUE:
            DEBUGDUMP(printf("--- Number in Queue ---\n"););
            length = rxstack_number_in_queue( client );
            rxstack_send_return( client->socket, "0", NULL, length );
            break;
         case RXSTACK_PULL:
            DEBUGDUMP(printf("--- Pull ---\n"););
            rc = rxstack_get_line_off_queue( client, &result );
            switch( rc )
            {
               case 0: /* all OK */
                  rxstack_send_return( client->socket, "0", PSTRENGVAL( result ), PSTRENGLEN( result ) );
                  DROPSTRENG( result );
                  rc = rxstack_delete_line_off_queue( client );
                  break;
               case 1: /* empty */
               case 2: /* error */
                  rcode[0] = (char)(rc+'0');
                  rxstack_send_return( client->socket, rcode, NULL, 0 );
                  DROPSTRENG( result );
                  break;
               default: /* 3 - still waiting; don't return */
                  break;
            }
            break;
         case RXSTACK_GET_QUEUE:
            DEBUGDUMP(printf("--- Get Queue ---\n"););
            rxstack_send_return( client->socket, "0", (queuename[client->default_queue])->value, (queuename[client->default_queue])->len );
            break;
         case RXSTACK_CREATE_QUEUE:
            DEBUGDUMP(printf("--- Create Queue ---\n"););
            /*
             * Create a new queue
             */
            rc = rxstack_create_queue( client, buffer );
            if ( rc == 0 )
               rxstack_send_return( client->socket, "0", NULL, 0 );
            else if ( rc == 1 )
            {
               rcode[0] = (char)(rc+'0');
               rxstack_send_return( client->socket, rcode, (queuename[client->default_queue])->value, (queuename[client->default_queue])->len );
            }
            DROPSTRENG( buffer );
            break;
         case RXSTACK_DELETE_QUEUE:
            DEBUGDUMP(printf("--- Delete Queue ---\n"););
            /*
             * Delete the queue
             */
            rc = rxstack_delete_queue( client, buffer );
            rcode[0] = (char)(rc+'0');
            rxstack_send_return( client->socket, rcode, NULL, 0 );
            DROPSTRENG( buffer );
            break;
         case RXSTACK_TIMEOUT_QUEUE:
            DEBUGDUMP(printf("--- Timeout Queue ---\n"););
            /*
             * Set timeout for pull from queue
             */
            rc = rxstack_timeout_queue( client, buffer );
            rcode[0] = (char)(rc+'0');
            rxstack_send_return( client->socket, rcode, NULL, 0 );
            DROPSTRENG( buffer );
            break;
         case RXSTACK_UNKNOWN:
            /* do nothing */
            break;
         default:
            rxstack_send_return( client->socket, "9", NULL, 0 );
            break;
      }
   }
   return 0;
}

void check_for_waiting( CLIENT *client, time_t inc )
{
   streng *result=NULL;
   int rc;
   char rcode[2];

   rcode[1] = '\0';
   /*
    * If the client is waiting infinitely (clinet->queue_timeout = -1, then
    * simply ignore the decrement of the timeout counter
    */
   if ( client->queue_timeout != -1 )
   {
      if ( inc >= client->queue_timeout )
         client->queue_timeout = 0;
      else
         client->queue_timeout -= inc;
   }
   DEBUGDUMP(printf("Checking for wait. Remaining time: %ld Value decremented: %ld\n", client->queue_timeout, inc ););
   /*
    * Check if there is anything in the queue...
    */
   rc = rxstack_get_line_off_queue( client, &result );
   if ( rc == 1 /* empty */
   &&   client->queue_timeout == 0 )
   {
      rc = 4;
   }
   switch( rc )
   {
      case 0: /* all OK */
         rxstack_send_return( client->socket, "0", PSTRENGVAL( result ), PSTRENGLEN( result ) );
         DROPSTRENG( result );
         rc = rxstack_delete_line_off_queue( client );
         break;
      case 1: /* empty */
      case 2: /* error */
      case 4: /* timeout */
         rcode[0] = (char)(rc+'0');
         rxstack_send_return( client->socket, rcode, NULL, 0 );
         DROPSTRENG( result );
         client->queue_timeout = 0;
         break;
      default: /* 3 - still waiting; don't return */
         break;
   }
}

int rxstack_doit( )
{
   int listen_sock,i,msgsock;
   struct sockaddr_in server,client;
   fd_set ready;
   int max_sock,client_size;
   int portno,rc;
   struct timeval to;
   time_t now;
#ifdef BUILD_NT_SERVICE
   char buf[30];
#endif
#if defined(SO_REUSEADDR) && defined(SOL_SOCKET)
   int on = 1;
#endif

   base_secs = time(NULL);
   client_size = sizeof( struct sockaddr );
#ifdef WIN32
   if ( init_external_queue( NULL ) )
      return 1;
#endif

#ifdef BUILD_NT_SERVICE
   if ( IsItNT()
   &&   !report_service_pending_start() )
      goto notrunning;
#endif
   /*
    * Set up signal handler
    */
#ifdef SIGTERM
   signal( SIGTERM, rxstack_signal_handler );
#endif
#ifdef SIGINT
   signal( SIGINT, rxstack_signal_handler );
#endif
#ifdef SIGPIPE
   signal( SIGPIPE, SIG_IGN );
#endif
   memset( clients, 0, sizeof(clients) );
   memset( queuename, 0, sizeof(queuename) );

   /*
    * Initialise default "SESSION" queue
    */
   queuename[0] = MakeStreng( 7 );
   memcpy( queuename[0]->value, "SESSION", 7 ) ;
   real_queue[0] = 1;

#ifdef BUILD_NT_SERVICE
   if ( IsItNT()
   &&   !report_service_pending_start() )
      goto notrunning;
#endif
   /* 
    * Create listener socket 
    */
   listen_sock = socket(AF_INET, SOCK_STREAM, 0);
   if (listen_sock < 0) 
   {
      showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_GENERAL, ERR_RXSTACK_GENERAL_TMPL, "Listening on socket", strerror( errno ) );
      rxstack_cleanup();
      exit(ERR_RXSTACK_GENERAL);
   }
   memset( &server, 0, sizeof(server) );
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(INADDR_ANY);
   portno = get_default_port_number();
   server.sin_port = htons((unsigned short) portno);

#ifdef BUILD_NT_SERVICE
   if ( IsItNT()
   &&   !report_service_pending_start() )
      goto notrunning;
#endif

#if defined(SO_REUSEADDR) && defined(SOL_SOCKET)
   setsockopt( listen_sock, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof( on ) );
#endif
   if ( bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) < 0)
   {
      showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_GENERAL, ERR_RXSTACK_GENERAL_TMPL, "Error binding socket", strerror( errno ) );
      rxstack_cleanup();
      exit( ERR_RXSTACK_GENERAL );
   }
#ifdef BUILD_NT_SERVICE
   sprintf(buf, "Listening on port: %d", portno );
   if ( IsItNT() )
   {
      if ( !report_service_start() )
         goto notrunning;
      AddToMessageLog(TEXT(buf));
   }
   else
   {
      printf( "%s\n", buf );
      fflush(stdout);
   }
#else
   printf( "rxstack listening on port: %d\n", portno );
   fflush(stdout);
#endif
   /* 
    * Start accepting connections 
    */
   listen(listen_sock, 5);
   while ( running )
   {
      FD_ZERO(&ready);
      FD_SET(listen_sock, &ready);
      DEBUGDUMP(printf("FD_SET for %d\n", listen_sock););
      max_sock = listen_sock;
      /*
       * For each connected client, allow its socket
       * to be triggered
       */
      for ( i = 0; i < MAX_CLIENTS; i++ )
      {

         if ( clients[i].socket )
         {
            DEBUGDUMP(printf("FD_SET for %d\n", clients[i].socket););
            FD_SET( clients[i].socket, &ready );
            if ( clients[i].socket > max_sock )
               max_sock = clients[i].socket;
         }
      }
      to.tv_usec = 100000; /* 100000 is 100 milliseconds */
      to.tv_sec = 0; 
      now = getmsecs();
      DEBUGDUMP(printf("*********** before select() max_sock %d time: %ld\n", max_sock, now););
      if ( ( rc = select(max_sock+1, &ready, (fd_set *)0, (fd_set *)0, &to) ) < 0)
      {
         if ( errno != EINTR )
            showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_GENERAL, ERR_RXSTACK_GENERAL_TMPL, "Calling select", strerror( errno ) );
         break;
      }
      DEBUGDUMP(printf("*********** after select() rc %d time: %ld\n", rc, getmsecs()););
      if ( rc )
      {
         if ( FD_ISSET(listen_sock, &ready ) )
         {
            msgsock =  accept(listen_sock, (struct  sockaddr *)&client, (int *)&client_size);
            if (msgsock == -1) 
            {
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_GENERAL, ERR_RXSTACK_GENERAL_TMPL, "Calling listen", strerror( errno ) );
               rxstack_cleanup();
               exit( ERR_RXSTACK_GENERAL );
            } 
            else
            {
               /*
                * A client has connected, create a client entry
                * and set their default queue to SESSION
                */
               /*
                * Validate the client here...TBD
                * use details in client sockaddr struct
                */
               DEBUGDUMP(printf("Client connecting from %s\n", inet_ntoa( client.sin_addr) ););
               rxstack_create_client( msgsock );
            }
         }
         else
         {
            for ( i = 0; i < MAX_CLIENTS; i++ )
            {
               if ( clients[i].socket )
               {
                  if ( FD_ISSET( clients[i].socket, &ready ) )
                  {
                     /*
                      * Process the client's command...
                      */
                     rxstack_process_command(&clients[i]);
                  }
               }
            }
         }
      }
      /*
       * If select() timed out or received input, check all connected clients who
       * may be waiting for input on one of the queues.
       */
      if ( rc != -1 )
      {
         /*
          * now contains the time between the start of select() call and now
          * in milliseconds
          */
         now = getmsecs() - now;
         for ( i = 0; i < MAX_CLIENTS; i++ )
         {
            if ( clients[i].socket
            &&   clients[i].queue_timeout )
            {
               check_for_waiting( &clients[i], now );
            }
         }
      }
   }
#ifdef BUILD_NT_SERVICE
notrunning:
#endif
   return 0;
}

int runNormal( int argc, char **argv )
{
   int rc=0;

   if ( argc == 2 )
   {
      if ( strcmp(argv[1], "-k") == 0 )
      {
         rc = suicide();
      }
      else if ( strcmp(argv[1], "-d") == 0 )
      {
#if defined(HAVE_FORK)
         if ( ( rc = fork() ) != 0 )
            exit(rc < 0);
         rc = rxstack_doit();
#else
         fprintf(stderr,"-d option invalid on this platform\nUsage: %s [-d|-k]\n", argv[0] );
         exit(1);
#endif
      }
      else
      {
         fprintf(stderr,"Usage: %s [-d|-k]\n", argv[0] );
         exit(1);
      }
   }
   else
   {
      rc = rxstack_doit();
   }
   rxstack_cleanup();
   printf( "%s terminated.\n", argv[0] );
   fflush(stdout);
   return rc;
}

#ifdef BUILD_NT_SERVICE
VOID ServiceStart(DWORD argc, LPTSTR *argv)
#else
int main(int argc, char *argv[])
#endif
{
#ifdef BUILD_NT_SERVICE
   if ( IsItNT() )
   {
      if ( !nt_service_start() )
      {
         rxstack_doit();
      }
      rxstack_cleanup();
      return;
   }
   else
   {
      runNormal(argc, argv);
      return;
   }
#else
   return runNormal( argc, argv );
#endif
}
