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

#include "rexx.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# include "configur.h"
#endif

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

#include "extstack.h"

#define SUCCESS(a) ((a)&&(a)->value[0] == '0')

#ifndef NDEBUG
# define DEBUGDUMP(x) {x;}
#else
# define DEBUGDUMP(x) {}
#endif

char *buff=NULL;
unsigned int bufflen=0;

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

int send_all( int sock, char *action )
{
   /*
    * Send everything in stdin to the socket. Each LF encountered
    * results in a send.
    * This code borrowed from rxstack.c in REXX/imc
    */
   int c,rc;
   unsigned int len;
   streng *result;

   for ( ; ; )
   {
      len = 0;
      while ( ( c = getchar() ) != EOF && c != '\n' )
      {
         if ( len >= bufflen
         && (( buff = realloc( buff, bufflen <<= 1 ) ) == NULL ) )
         {
           showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
           exit( ERR_STORAGE_EXHAUSTED );
         }
         buff[len++] = (char)c;
      }
      if ( (c == EOF || feof(stdin) )
      &&  !len)
         break;
      DEBUGDUMP(printf("--- Queue %s ---\n", action[0] == RXSTACK_QUEUE_FIFO ? "FIFO" : "LIFO"););
      rc = send_command_to_rxstack( NULL, sock, action, buff, len );
      if ( rc != -1 )
      {
         result = read_result_from_rxstack( NULL, sock, RXSTACK_HEADER_SIZE );
         DROPSTRENG( result );
      }
   }
   return 0;
}

char *get_unspecified_queue( void )
{
   char *rxq = getenv( "RXQUEUE" );

   if ( rxq == NULL )
      rxq = "SESSION";
   return rxq;
}

char *get_action( char *parm )
{
   char *action=NULL;

   if ( strcmp( parm, "/fifo" ) == 0 )
      action = RXSTACK_QUEUE_FIFO_STR;
   else if ( strcmp( parm, "/lifo" ) == 0 )
      action = RXSTACK_QUEUE_LIFO_STR;
   else if ( strcmp( parm, "/clear" ) == 0 )
      action = RXSTACK_EMPTY_QUEUE_STR;
   else if ( strcmp( parm, "/pull" ) == 0 )
      action = RXSTACK_PULL_STR;
   else if ( strcmp( parm, "/queued" ) == 0 )
      action = RXSTACK_NUMBER_IN_QUEUE_STR;
   else
   {
      showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_SWITCH, ERR_RXSTACK_INVALID_SWITCH_TMPL, "/fifo, /lifo, /clear, /queued, /pull" );
      /* not setting action will exit */
   }
   return action;
}

void junk_return_from_rxstack( int sock, streng *header )
{
   int length;
   streng *result;

   length = get_length_from_header( NULL, header );
   if ( length )
   {
      result = read_result_from_rxstack( NULL, sock, length );
      if ( result )
         DROPSTRENG( result );
   
   }
}

int cleanup( void )
{
#ifdef WIN32
   WSACleanup();
#endif
   if ( buff )
      free( buff );
   return 0;
}

int main( int argc, char *argv[])
{
   int sock,portno,rc=0,i,num;
   char *action;
   streng *queue=NULL,*server_name=NULL;
   char *in_queue=NULL;
   int server_address;
   streng *result;
#ifdef WIN32
   WORD wsver = (WORD)MAKEWORD(1,1);
   WSADATA wsaData;
#endif

#ifdef WIN32
   if ( WSAStartup( wsver, &wsaData ) != 0 )
   {
      showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_NO_WINSOCK, ERR_RXSTACK_NO_WINSOCK_TMPL, WSAGetLastError() );
      exit(ERR_RXSTACK_NO_WINSOCK);
   }
#endif

   action = NULL;
   /*
    * Process the command line
    */
   if ( argc == 1 )
   {
      /*
       * "rxqueue"
       */
      action = RXSTACK_QUEUE_FIFO_STR;
      in_queue = get_unspecified_queue();
   }
   else if ( argc == 2 )
   {
      /*
       * "rxqueue queue"
       * or
       * "rxqueue /switch"
       */
      if ( argv[1][0] == '/' )
      {
         /*
          * Only parameter is a switch
          */
         in_queue = get_unspecified_queue();
         action = get_action( argv[1] );
      }
      else
      {
         /*
          * Only parameter is a queue name
          */
         in_queue = argv[1];
         action = RXSTACK_QUEUE_FIFO_STR;
      }
   }
   else if ( argc == 3 )
   {
      /*
       * "rxqueue queue /switch"
       */
      in_queue = argv[1];
      if ( argv[2][0] == '/' )
      {
         /*
          * Parameter is a switch
          */
         action = get_action( argv[2] );
      }
   }
   else
   {
      /*
       * ERROR
       */
      fprintf(stderr, "Invalid number of parameters\n");
      rc = 1;
   }
   if ( action )
   {
      queue = MAKESTRENG( strlen( in_queue ) );
      if ( queue  == NULL )
      {
         showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
         exit(ERR_STORAGE_EXHAUSTED);
      }
      memcpy( PSTRENGVAL( queue ), in_queue, PSTRENGLEN( queue ) );
      /*
       * Parse the queue to determine server address
       * and queue name
       */
      if ( parse_queue( NULL, queue, &server_name, &server_address, &portno ) == 0 )
      {
         if ( PSTRENGLEN( queue ) == 0 )
         {
            DROPSTRENG( queue );
            queue = MAKESTRENG( strlen( in_queue ) );
            if ( queue  == NULL )
            {
               showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
               exit(ERR_STORAGE_EXHAUSTED);
            }
            memcpy( PSTRENGVAL( queue ), "SESSION", PSTRENGLEN( queue ) );
         }
         DEBUGDUMP(printf( "queue: <%s> server: %s<%d> Port:%d\n", PSTRENGVAL( queue ), PSTRENGVAL( server_name ), server_address,portno ););
         sock = connect_to_rxstack( NULL, portno, server_name, server_address );
         if ( sock < 0 )
         {
            cleanup();
            exit(ERR_RXSTACK_CANT_CONNECT);
         }
         /*
          * Now process the actual command
          */
         switch( action[0] )
         {
            case RXSTACK_QUEUE_FIFO:
            case RXSTACK_QUEUE_LIFO:
               DEBUGDUMP(printf("--- Queue %s ", action[0] == RXSTACK_QUEUE_FIFO ? "FIFO" : "LIFO"););
               /*
                * Allocate the initial buffer
                */
               if (( buff = (char *)malloc( bufflen = 256 ) ) == NULL )
               {
                  showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
                  rc = ERR_STORAGE_EXHAUSTED;
                  break;
               }
               /*
                * Set the current queue
                */
               DEBUGDUMP(printf("(Set Queue) ---\n"););
               rc = send_command_to_rxstack( NULL, sock, RXSTACK_SET_QUEUE_STR, PSTRENGVAL( queue) , PSTRENGLEN( queue ) );
               if ( rc == -1 )
               {
                  showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Setting default queue" );
                  rc = ERR_RXSTACK_INTERNAL;
                  break;
               }
               result = read_result_from_rxstack( NULL, sock, RXSTACK_HEADER_SIZE );
               if ( !SUCCESS( result ) )
               {
                  showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Setting default queue" );
                  DROPSTRENG( result );
                  rc = ERR_RXSTACK_INTERNAL;
                  break;
               }
               /*
                * Success. We need to throw away the returned queue name. We
                * don't need it.
                */
               junk_return_from_rxstack( sock, result );
               send_all( sock, action );
               DROPSTRENG( result );
               rc = 0;
               break;
            case RXSTACK_EMPTY_QUEUE:
               DEBUGDUMP(printf("--- Empty Queue ---\n"););
               rc = send_command_to_rxstack( NULL, sock, action, PSTRENGVAL( queue) , PSTRENGLEN( queue ) );
               if ( rc == -1 )
               {
                  showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Emptying queue" );
                  rc = ERR_RXSTACK_INTERNAL;
                  break;
               }
               result = read_result_from_rxstack( NULL, sock, RXSTACK_HEADER_SIZE );
               if ( !SUCCESS( result ) )
               {
                  showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Emptying queue" );
                  DROPSTRENG( result );
                  rc = ERR_RXSTACK_INTERNAL;
                  break;
               }
               DROPSTRENG( result );
               rc = 0;
               break;
            case RXSTACK_NUMBER_IN_QUEUE:
               /*
                * Set the current queue
                */
               DEBUGDUMP(printf("--- Set Queue ---\n"););
               rc = send_command_to_rxstack( NULL, sock, RXSTACK_SET_QUEUE_STR, PSTRENGVAL( queue ), PSTRENGLEN( queue ) );
               if ( rc == -1 )
               {
                  showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Setting default queue" );
                  rc = ERR_RXSTACK_INTERNAL;
                  break;
               }
               /*
                * Read the return response header
                */
               result = read_result_from_rxstack( NULL, sock, RXSTACK_HEADER_SIZE );
               if ( !SUCCESS( result ) )
               {
                  showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Setting default queue" );
                  DROPSTRENG( result );
                  rc = ERR_RXSTACK_INTERNAL;
                  break;
               }
               /*
                * Success. We need to throw away the returned queue name. We
                * don't need it.
                */
               junk_return_from_rxstack( sock, result );
               DROPSTRENG( result );
               DEBUGDUMP(printf("--- Number in Queue ---\n"););
               num = get_number_in_queue_from_rxstack( NULL, sock );
               printf("%d\n", num );
               rc = 0;
               break;
            case RXSTACK_PULL:
               /*
                * Set the current queue
                */
               DEBUGDUMP(printf("--- Set Queue ---\n"););
               rc = send_command_to_rxstack( NULL, sock, RXSTACK_SET_QUEUE_STR, PSTRENGVAL( queue ), PSTRENGLEN( queue ) );
               if ( rc == -1 )
               {
                  showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Setting default queue" );
                  rc = ERR_RXSTACK_INTERNAL;
                  break;
               }
               result = read_result_from_rxstack( NULL, sock, RXSTACK_HEADER_SIZE );
               if ( !SUCCESS( result ) )
               {
                  showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Setting default queue" );
                  DROPSTRENG( result );
                  rc = ERR_RXSTACK_INTERNAL;
                  break;
               }
               /*
                * Success. We need to throw away the returned queue name. We
                * don't need it.
                */
               junk_return_from_rxstack( sock, result );
               DROPSTRENG( result );
               /*
                * Loop until the number of lines on the queue is zero
                */
               for ( ; ; )
               {
                  DEBUGDUMP(printf("--- Pull ---\n"););
                  rc = get_line_from_rxstack( NULL, sock, &result );
                  if ( rc == 0 )
                  {
                     for ( i = 0; i < PSTRENGLEN( result ); i++ )
                     {
                        putchar( result->value[i] );
                     }
                     putchar( '\n' );
                  }
                  else if ( rc == 2 )
                  {
                     showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Getting line from queue" );
                     rc = ERR_RXSTACK_INTERNAL;
                     break;
                  }
                  else
                  {
                     /*
                      * Queue empty.
                      */
                     rc = 0;
                     break;
                  }
                  DROPSTRENG( result );
               }
               break;
         }
         /*
          * Tell the server we want to disconnect
          * Don't expect any return result.
          */
         DEBUGDUMP(printf("--- Exit ---\n"););
         send_command_to_rxstack( NULL, sock, RXSTACK_EXIT_STR, NULL, 0 );
         close(sock);
      }
      else
      {
         DEBUGDUMP(printf( "queue: <%s> server: %s<%d> Port:%d\n", PSTRENGVAL( queue ), PSTRENGVAL( server_name ), server_address,portno ););
      }
   }
   DROPSTRENG( server_name );
   DROPSTRENG( queue );
   cleanup();
   return rc;
}
