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
 
#ifdef EXTERNAL_TO_REGINA
# include <assert.h>
/* tmpstr_of must be defined before the inclusion of extstack.h since it
 * redefines tsd_t (grrr) */
volatile char *tmpstr_of( tsd_t *TSD, const streng *input )
{
   /* even exiterror isn't permitted at this point. */
   assert(0); /* hint while debugging */
   return((volatile char *) input->value);
}
#endif


#if defined(WIN32)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>

#include "extstack.h"

#ifndef NDEBUG
# define DEBUGDUMP(x) {x;}
#else
# define DEBUGDUMP(x) {}
#endif

void showerror( int err, int suberr, char *tmpl, ...)
{
   va_list argptr;
   if ( suberr )
      fprintf( stderr, "Error:%d.%d - ", err, suberr );
   else
      fprintf( stderr, "Error:%d - ", err );
   va_start( argptr, tmpl );
   vfprintf( stderr, tmpl, argptr );
   va_end( argptr );
   fprintf( stderr, "\n" );
}

int init_external_queue( const tsd_t *TSD )
{
   int rc=0;
#ifdef WIN32
   WORD wsver = (WORD)MAKEWORD(1,1);
   WSADATA wsaData;
   if ( WSAStartup( wsver, &wsaData ) != 0 )
   {
      /* TSD can be NULL when called from rxqueue or rxstack */
      if ( TSD == NULL || TSD->called_from_saa )
         showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_NO_WINSOCK, ERR_RXSTACK_NO_WINSOCK_TMPL, WSAGetLastError() );
      else
         exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_NO_WINSOCK, WSAGetLastError() );
      rc = 1;
   }
#else
   TSD = TSD; /* keep compiler happy */
#endif
   return rc;
}

void term_external_queue( void )
{
#ifdef WIN32
   WSACleanup();
#endif
}

int get_default_port_number( void )
{
   int portno;
   char *port = getenv("RXSTACK");
   if ( port == NULL )
   {
      portno = RXSOCKET;
   }
   else
   {
      portno = atoi(port);

   }
   return portno;
}

int get_length_from_header( const tsd_t *TSD, streng *header )
{
   int length=0;
   streng *result=NULL;

   result = MAKESTRENG( RXSTACK_HEADER_SIZE - 1 );
   if ( result )
   {
      result->len = RXSTACK_HEADER_SIZE - 1;
      memcpy( result->value, header->value+1, RXSTACK_HEADER_SIZE-1 );
      DEBUGDUMP(printf("Hex value: %s\n", result->value););
      length = REXX_X2D( result );
      DROPSTRENG( result );
   }
   return length;
}

#if !defined(NO_EXTERNAL_QUEUES)

int get_default_server_address( void )
{
   return inet_addr( "127.0.0.1" );
}

int connect_to_rxstack( tsd_t *TSD, int portno, streng *server_name, int server_address )
{
   struct sockaddr_in server;
   int sock;
   /*
    * Connect to external rxstack process/daemon/service
    */
   memset( &server, 0, sizeof(server) );
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = server_address;
   server.sin_port = htons((unsigned short) portno);

   sock = socket( AF_INET, SOCK_STREAM, 0 );
   if ( sock < 0 )
      return sock;
   if ( connect( sock, (struct sockaddr *)&server, sizeof(server) ) < 0 )
   {
      /* TSD can be NULL when called from rxqueue or rxstack */
      if ( TSD == NULL || TSD->called_from_saa )
         showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_CANT_CONNECT, ERR_RXSTACK_CANT_CONNECT_TMPL, server_name->value, portno, strerror ( errno ) );
      else
         exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_CANT_CONNECT, tmpstr_of( (void *) TSD, server_name), portno, strerror ( errno ) );
      return -1;
   }
   return sock;
}
#endif

#if !defined(NO_EXTERNAL_QUEUES)
int disconnect_from_rxstack( const tsd_t *TSD, int sock )
{
   int rc;

   DEBUGDUMP(printf("Diconnecting from socket %d\n", sock ););
   rc = send_command_to_rxstack( TSD, sock, RXSTACK_EXIT_STR, NULL, 0 );
   close( sock );
   return rc;
}
#endif

#if !defined(NO_EXTERNAL_QUEUES)
int send_command_to_rxstack( const tsd_t *TSD, int sock, char *action, char *str, int len )
{
   streng *qlen, *header;
   int rc=-1;

   DEBUGDUMP(printf("Sending to %d Action: %s <%s> Len:%d\n", sock, action, (str) ? str : "", len););
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
         if ( str && rc != -1 )
         {
            rc = send( sock, str, len, 0 );
            DEBUGDUMP(printf("Send str length: %d\n", rc););
         }
         DROPSTRENG( header );
      }
   }
   return rc;
}
#endif

#if !defined(NO_EXTERNAL_QUEUES)
streng *read_result_from_rxstack( const tsd_t *TSD, int sock, int result_size )
{
  /*
   * Creates a streng of length 'result_size' and if non-zero,
   * reads this many characters from the socket.
   * The caller MUST DROPSTRENG this streng.
   */
   streng *result;
   int rc;

   result = MAKESTRENG( result_size );
   if ( result )
   {
      result->len = result_size;
      if ( result_size )
      {
         rc = recv( sock, PSTRENGVAL(result), PSTRENGLEN(result), 0 );
         DEBUGDUMP(printf("Recv result: %s(%d) length: %d\n", PSTRENGVAL(result),PSTRENGLEN(result), rc););
      }
   }
   return result;
}
#endif

#if !defined(NO_EXTERNAL_QUEUES)
int parse_queue( tsd_t *TSD, streng *queue, streng **server_name, int *server_address, int *portno )
{
   /*
    * Validate the queue name. Format is [queue][@host[:port]]
    */
   int num_colons=0,num_ats=0,num_dots=0,i,colon_pos=-1,at_pos=-1;
   char *tmp_server=NULL,*tmp_port=NULL;
   int len=PSTRENGLEN( queue );
   struct hostent *host;
   struct in_addr *ptr;

   /*
    * Find the optional '@' and ':'...
    */
   for ( i = 0; i < len; i++ )
   {
      if ( queue->value[i] == '@' )
      {
         num_ats++;
         at_pos = i;
         tmp_server = queue->value+at_pos+1;
         queue->value[at_pos] = '\0';
      }
      else if ( queue->value[i] == ':' )
      {
         num_colons++;
         colon_pos = i;
         tmp_port = queue->value+colon_pos+1;
         queue->value[colon_pos] = '\0';
      }
   }
   if ( num_colons > 1 
   ||   num_ats > 1 
   || ( num_colons == 1 
      &&   num_ats == 1
      &&   at_pos > colon_pos ) )
   {
      /* TSD can be NULL when called from rxqueue or rxstack */
      if ( TSD == NULL || TSD->called_from_saa )
         showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, ERR_RXSTACK_INVALID_QUEUE_TMPL, queue->value );
      else
         exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of( (void *) TSD, queue ) );
      return 1;
   }
   /*
    * Sort out the port number portion...
    */
   if ( num_colons == 1 )
   {
      *portno = atoi( tmp_port );
      queue->len = colon_pos;
   }
   else
   {
      *portno = get_default_port_number();
   }
   DEBUGDUMP(printf("port <%d>\n", *portno ););
   /*
    * Sort out server_name portion...
    */
   if ( num_ats == 1 )
   {
      /*
       * Get the length of the server portion of the specified queue
       * If 3 dots, we have an IP address for the server address and queue name
       */
      len = strlen( tmp_server );
      for( i = 0, num_dots = 0; i < len; i++ )
      {
         if ( tmp_server[i] == '.' )
            num_dots++;
      }
      if ( num_dots == 0 )
      {
         host = gethostbyname( tmp_server );
         if ( host )
         {
            ptr = (struct in_addr *)host->h_addr;
            *server_address = ptr->s_addr;
            *server_name = MAKESTRENG( len+1 );
            if ( *server_name == NULL )
            {
               /* TSD can be NULL when called from rxqueue or rxstack */
               if ( TSD == NULL || TSD->called_from_saa )
                  showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
               else
                  exiterror( ERR_STORAGE_EXHAUSTED, 0 );
               return 1;
            }
            (*server_name)->len = len;
            memcpy( (*server_name)->value, tmp_server, len );
            (*server_name)->value[len] = '\0';
         }
         else
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_NO_IP, ERR_RXSTACK_NO_IP_TMPL, tmp_server );
            else
               exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_NO_IP, tmp_server );
            return 1;
         }
      }
      else if ( num_dots == 3 )
      {
         *server_address = inet_addr( tmp_server );
         *server_name = MAKESTRENG( len+1 );
         if ( *server_name == NULL )
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
            else
               exiterror( ERR_STORAGE_EXHAUSTED, 0 );
            return 1;
         }
         (*server_name)->len = len;
         memcpy( (*server_name)->value, tmp_server, len );
         (*server_name)->value[len] = '\0';
      }
      else
      {
         /* TSD can be NULL when called from rxqueue or rxstack */
         if ( TSD == NULL || TSD->called_from_saa )
            showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_SERVER, ERR_RXSTACK_INVALID_SERVER_TMPL, tmp_server );
         else
            exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_SERVER, tmp_server );
         return 1;
      }
      queue->len = at_pos;
   }
   else 
   {
      *server_address = inet_addr("127.0.0.1");
      *server_name = MAKESTRENG( 10 );
      if ( *server_name == NULL )
      {
         /* TSD can be NULL when called from rxqueue or rxstack */
         if ( TSD == NULL || TSD->called_from_saa )
            showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
         else
            exiterror( ERR_STORAGE_EXHAUSTED, 0 );
         return 1;
      }
      (*server_name)->len = 10;
      memcpy( (*server_name)->value, "127.0.0.1", 9 );
      (*server_name)->value[9] = '\0';
   }
   DEBUGDUMP(printf("server <%s>\n", (*server_name)->value ););
   return 0;
}
#endif

int delete_queue_from_rxstack( const tsd_t *TSD, int sock, streng *queue_name )
{
   int rc;
   streng *result;

   rc = send_command_to_rxstack( TSD, sock, RXSTACK_DELETE_QUEUE_STR, PSTRENGVAL( queue_name ), PSTRENGLEN( queue_name ) );
   if ( rc != -1 )
   {
      result = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
      if ( result )
      {
         rc = result->value[0]-'0';
         DROPSTRENG( result );
      }
   }
   return rc;
}

int timeout_queue_on_rxstack( const tsd_t *TSD, int sock, long timeout )
{
   int rc=0;
   streng *result,*qtimeout, *hex_timeout;

   qtimeout = REXX_D2X( timeout );
   if ( qtimeout )
   {
      hex_timeout = REXX_RIGHT( qtimeout, RXSTACK_TIMEOUT_SIZE, '0');
      DROPSTRENG( qtimeout );
      if ( hex_timeout )
      {
         DEBUGDUMP(printf("Send timeout: %s(%d) rc %d\n", PSTRENGVAL(hex_timeout),PSTRENGLEN(hex_timeout),rc););
         rc = send_command_to_rxstack( TSD, sock, RXSTACK_TIMEOUT_QUEUE_STR, PSTRENGVAL(hex_timeout), PSTRENGLEN(hex_timeout) );
         DROPSTRENG( hex_timeout );
         if ( rc != -1 )
         {
            result = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
            if ( result )
            {
               rc = result->value[0]-'0';
               DROPSTRENG( result );
            }
         }
      }
   }
   return rc;
}

int get_number_in_queue_from_rxstack( const tsd_t *TSD, int sock )
{
   int rc,length=0;
   streng *header;

   rc = send_command_to_rxstack( TSD, sock, RXSTACK_NUMBER_IN_QUEUE_STR, NULL, 0 );
   if ( rc != -1 )
   {
      header = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
      if ( header )
      {
         rc = header->value[0]-'0';
         if ( rc == 0 )
         {
            /* 
             * now get the length from the header
             */
            DEBUGDUMP(printf("before get_length_from_header: %s\n", header->value););
            length = get_length_from_header( TSD, header );
         }
         else
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Getting number in queue" );
            else
               exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, rc, "Getting number in queue"  );
         }
         DROPSTRENG( header );
      }
   }
   return length;
}

int get_queue_from_rxstack( const tsd_t *TSD, int sock, streng **result )
{
   int rc,length;
   streng *header;

   rc = send_command_to_rxstack( TSD, sock, RXSTACK_GET_QUEUE_STR, NULL, 0 );
   if ( rc != -1 )
   {
      header = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
      if ( header )
      {
         rc = header->value[0]-'0';
         if ( rc == 0 )
         {
            /* 
             * now get the length from the header and get that many characters...
             */
            length = get_length_from_header( TSD, header );
            *result = read_result_from_rxstack( TSD, sock, length );
         }
         else
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Getting queue from stack" );
            else
               exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, rc, "Getting queue from stack" );
         }
         DROPSTRENG( header );
      }
   }
   return rc;
}

int get_line_from_rxstack( const tsd_t *TSD, int sock, streng **result )
{
   int rc,length;
   streng *header;

   rc = send_command_to_rxstack( TSD, sock, RXSTACK_PULL_STR, NULL, 0 );
   if ( rc != -1 )
   {
      header = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
      if ( header )
      {
         rc = header->value[0]-'0';
         if ( rc == 0 )
         {
            /* 
             * now get the length from the header and get that many characters...
             */
            length = get_length_from_header( TSD, header );
            *result = read_result_from_rxstack( TSD, sock, length );
         }
         else if ( rc == 1 || rc == 4 )
         {
            /*
             * queue is empty - return a NULL
             */
            *result = NULL;
         }
         else
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Getting line from queue" );
            else
               exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, rc, "Getting line from queue" );
         }
         DROPSTRENG( header );
      }
   }
   return rc;
}

int create_queue_on_rxstack( const tsd_t *TSD, int sock, streng *queue, streng **result )
{
   int rc,length;
   streng *header;

   rc = send_command_to_rxstack( TSD, sock, RXSTACK_CREATE_QUEUE_STR, (queue) ? PSTRENGVAL( queue ) : NULL, (queue) ? PSTRENGLEN( queue ) : 0 );
   if ( rc != -1 )
   {
      header = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
      if ( header )
      {
         rc = header->value[0]-'0';
         if ( rc == 0 )
         {
            /* 
             * the requested queue name was created, so put the input
             * value into the result
             */
            *result = MAKESTRENG( PSTRENGLEN( queue ) );
            if ( *result == NULL )
            {
               /* TSD can be NULL when called from rxqueue or rxstack */
               if ( TSD == NULL || TSD->called_from_saa )
                  showerror( ERR_STORAGE_EXHAUSTED, 0, ERR_STORAGE_EXHAUSTED_TMPL );
               else
                  exiterror( ERR_STORAGE_EXHAUSTED, 0 );
            }
            else
            {
               (*result)->len = PSTRENGLEN( queue );
               memcpy( (*result)->value, PSTRENGVAL( queue ), PSTRENGLEN( queue ) );
            }
         }
         else if ( rc == 1 )
         {
            /* 
             * now get the length from the header and get that many characters...
             */
            length = get_length_from_header( TSD, header );
            *result = read_result_from_rxstack( TSD, sock, length );
         }
         else
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Creating queue" );
            else
               exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, rc, "Creating queue" );
         }
         DROPSTRENG( header );
      }
   }
   return rc;
}

int set_queue_in_rxstack( const tsd_t *TSD, int sock, streng *queue_name, streng **result )
{
   int rc,length;
   streng *header;

   rc = send_command_to_rxstack( TSD, sock, RXSTACK_SET_QUEUE_STR, PSTRENGVAL( queue_name ), PSTRENGLEN( queue_name ) );
   if ( rc != -1 )
   {
      header = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
      if ( header )
      {
         rc = header->value[0]-'0';
         if ( rc == 0 )
         {
            /* 
             * now get the length from the header and get that many characters...
             */
            length = get_length_from_header( TSD, header );
            *result = read_result_from_rxstack( TSD, sock, length );
         }
         else
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Setting quueue" );
            else
               exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, rc, "Setting quueue" );
         }
         DROPSTRENG( header );
      }
   }
   return rc;
}

int queue_line_fifo_to_rxstack( const tsd_t *TSD, int sock, streng *line )
{
   int rc;
   streng *header;

   rc = send_command_to_rxstack( TSD, sock, RXSTACK_QUEUE_FIFO_STR, PSTRENGVAL( line ), PSTRENGLEN( line ) );
   if ( rc != -1 )
   {
      header = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
      if ( header )
      {
         rc = header->value[0]-'0';
         if ( rc != 0 )
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Queueing line" );
            else
               exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, rc, "Queueing line" );
         }
         DROPSTRENG( header );
      }
   }
   return rc;
}

int queue_line_lifo_to_rxstack( const tsd_t *TSD, int sock, streng *line )
{
   int rc;
   streng *header;

   rc = send_command_to_rxstack( TSD, sock, RXSTACK_QUEUE_LIFO_STR, PSTRENGVAL( line ), PSTRENGLEN( line ) );
   if ( rc != -1 )
   {
      header = read_result_from_rxstack( TSD, sock, RXSTACK_HEADER_SIZE );
      if ( header )
      {
         rc = header->value[0]-'0';
         if ( rc != 0 )
         {
            /* TSD can be NULL when called from rxqueue or rxstack */
            if ( TSD == NULL || TSD->called_from_saa )
               showerror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, ERR_RXSTACK_INTERNAL_TMPL, rc, "Queueing line" );
            else
               exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INTERNAL, rc, "Queueing line" );
         }
         DROPSTRENG( header );
      }
   }
   return rc;
}
