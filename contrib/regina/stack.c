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

/* Concept of implementation:

 * In this description, the stack is described as growing upwards

 * The stack has two pointers to the ends of a double-linked list,
 * which contains entries consisting of just a pointer to the text (in
 * a struct streng) in addition to pointers to next and previous
 * entry.  Last box' next, and first box' prev is both NIL. The text
 * pointer points to the text of that line in the stack.

 * If the text pointer is NIL, then that box in the stack-structure is
 * just symbolizing a stack buffer mark. The integer variable
 * 'buffers' contains the current number of buffermarks in the stack.
 * A buffer is the lines between two buffermarks. The first buffer mark
 * (for buffer 0) is not marked, but is implicit the bottom of the
 * stack. The last buffer is the lines between the uppermost
 * buffermark and the top of the stack.

 * Initialy all lines are put in buffer 0. When a new buffer is
 * created, lines pushed lifo is put on top of that buffer, and lines
 * queued fifo are put in the bottom of that buffer (i.e. not in the
 * bottom of buffer 0.) Lines pushed on the stack are 'eaten' and
 * there must not be any referances to them in other parts of the
 * program after they have been pushed.

 * When reading lines from the stack, and all lines in the current
 * buffer has been read, the buffer-mark will be removed, and lines
 * are read from the buffer underneath. Lines read from the stack must
 * be free'ed by the routine that popped them from the stack.

 * When the whole stack is empty, lines are read from standard input.

 * Buffer 0 is contains the lines between the bottom of the stack, and
 * the first buffermark on the stack, the first buffer, is the lines
 * between buffermark 1 and buffer mark 2.

 * When creating a buffer, the value returned will be the number of
 * buffermarks in the stack after the new buffermark is created.
 * When destroying buffers, the parameter given will equal the number
 * of the lowest buffermark to be destroyed (i.e dropbuf 4) will leave
 * 3 buffermarks on the stack).

 * Possible extentions:
 *  o A new 'hard-buffer', that is not removed when a line is popped
 *    from it while it is empty
 */

#include "rexx.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#ifdef HAVE_PROCESS_H
# include <process.h>
#endif

#if !defined(NO_EXTERNAL_QUEUES)
# include "extstack.h"
#endif

#ifndef NDEBUG
# define DEBUGDUMP(x) {x;}
#else
# define DEBUGDUMP(x) {}
#endif

#define NUMBER_QUEUES 100
/*
 * Queue 0 is SESSION and cannot be added/deleted
 */
/*
 * Hide the definition of the stack from the rest of the program
 */
typedef struct stacklinestruct *stacklineptr ;

typedef struct stacklinestruct
{
   stacklineptr next, prev ;
   streng *contents ;
} stackline ;

typedef struct { /* stk_tsd: static variables of this module (thread-safe) */
#if !defined(NOEXTERNAL_QUEUES)
   /*
    * The port number for the current connection to an external rxstack
    */
   int external_queue_portno;
   /*
    * The socket fd for the current connection to an external rxstack
    */
   int external_queue_socket;
   /*
    * The server_address of the current connection. Used to determine if
    * we need to disconnect from one rxstack server and connect to
    * another
    */
   int external_server_address;
   /*
    * The server_name of the current connection.
    */
   streng *external_server_name;
#endif
   /*
    * Pointers to queue names
    */
   streng *queuename[NUMBER_QUEUES] ;
   /*
    * The index into the current queue
    */
   int current_queue;
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
    * The number of buffermarks in the stack
    */
   int buffers[NUMBER_QUEUES] ;
   /*
    * Pointers to first and last entry in the temporary stack
    */
   stacklineptr firstbox ;
   stacklineptr lastbox ;
} stk_tsd_t; /* thread-specific but only needed by this module. see
              * init_stacks
              */

#if !defined(NOEXTERNAL_QUEUES)
static int get_socket_details_and_connect( tsd_t *TSD, streng *server_name, int server_address, int portno );
#endif

/* init_stacks initializes the module.
 * Currently, we set up the thread specific data.
 * The function returns 1 on success, 0 if memory is short.
 */
int init_stacks( tsd_t *TSD )
{
   stk_tsd_t *st;

   if (TSD->stk_tsd != NULL)
      return(1);

   if ((st = TSD->stk_tsd = MallocTSD(sizeof(stk_tsd_t))) == NULL)
      return(0);
   memset(st,0,sizeof(stk_tsd_t));  /* correct for all values */
   /*
    * Create the default internal SESSION queue
    */
   st->queuename[0] = Str_cre_TSD( TSD, "SESSION" ) ;
   st->external_server_name = Str_cre_TSD( TSD, "localhost" ) ;
   st->real_queue[0] = 1;
   st->external_queue_socket = 0;
   st->external_server_address = 0;
   st->external_queue_portno = 0;
   return(1);
}
/*
 * Returns 1 if we have started using external queues, 0 if we haven't
 * started using external queues
 */
int external_queues_used( const tsd_t *TSD )
{
   stk_tsd_t *st;

#if defined(NOEXTERNAL_QUEUES)
   return 0;
#else
   st = TSD->stk_tsd;
   return(st->external_queue_portno != 0 );
#endif
}

void delete_an_internal_queue( const tsd_t *TSD, int idx )
{
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   if ( st->queuename[idx] != NULL )
   {
      drop_buffer( TSD, 0 );
      Free_stringTSD( st->queuename[idx] );
      st->queuename[idx] = NULL;
      st->real_queue[idx] = 0;
      st->current_queue = 0;
   }
}

/*
 * This routine clears all the internal queues we have made
 * and if we have a connection to rxstack, disconnetcs from it
 */
void purge_stacks( const tsd_t *TSD )
{
   stk_tsd_t *st;
   int i;

   st = TSD->stk_tsd;
   for ( i = 0; i < NUMBER_QUEUES; i++ )
      delete_an_internal_queue( TSD, i );
#if !defined(NO_EXTERNAL_QUEUES)
   if ( st->external_queue_socket != 0 )
   {
      disconnect_from_rxstack( TSD, st->external_queue_socket );
      st->external_queue_socket = 0;
      st->external_queue_portno = 0;
      st->external_server_address = 0;
   }
#endif
}

/*
 * Find the named queue - case insensitive
 */
static int find_queue( const tsd_t *TSD, streng *queue_name )
{
   int i;
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   for ( i = 0; i < NUMBER_QUEUES; i++ ) /* inefficient !! */
   {
      if ( st->queuename[i]
      &&   Str_ccmp( st->queuename[i], queue_name ) == 0 )
         return i;
   }
   return UNKNOWN_QUEUE;
}

/*
 * This routines accumulates stackentries, but without pushing them
 * on the actual stack, used when stacking the output of an external
 * command. If such a command also reads from the stack, the output
 * can not be flushed to the stack before the command has finished
 * reading (else, the command might read its own output.)
 *
 * Parameter is the streng to stack.
 */
void tmp_stack( tsd_t *TSD, streng *value, int is_fifo )
{
   stacklineptr ptr=NULL ;
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   assert( ((st->lastbox)&&(st->firstbox)) || ((!st->lastbox)&&(!st->firstbox)) ) ;
   assert( (value) && (Str_max(value) >= Str_len(value)) ) ;

   if ( get_options_flag( TSD->currlevel, EXT_FLUSHSTACK ) )
   {
      if (is_fifo)
         stack_fifo( TSD, value, NULL ) ;
      else
         stack_lifo( TSD, value, NULL ) ;
   }
   else
   {
      ptr = MallocTSD( sizeof(struct stacklinestruct) ) ;
      ptr->contents = value ;
      ptr->next = NULL ;
      ptr->prev = st->lastbox ;


      if (st->firstbox)
         st->lastbox->next = ptr ;
      else
         st->firstbox = ptr ;
      st->lastbox = ptr ;
   }
}


/*
 * Flushes the content of the temporary stack created by (possibly
 * multiple) calls to tmp_stack().
 *
 * If parameter is true, lines are stacked fifo, which requires their
 * order to be reversed first.
 */
void flush_stack( const tsd_t *TSD, int is_fifo )
{
   stacklineptr ptr=NULL, tptr=NULL ;
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   if (st->firstbox==NULL)
   {
      /* nothing in temporary stack to flush */
      assert(st->lastbox==NULL) ;
      return ;
   }

   /* stack it either fifo or lifo */
   if (is_fifo)
   {
      /* if fifo, temporary stack must be reversed first */
      /* travelling in 'prev' direction, since stack is reversed */
      for (ptr=st->firstbox; ptr; ptr=ptr->prev )
      {
         tptr = ptr->prev ;
         ptr->prev = ptr->next ;
         ptr->next = tptr ;
      }

      /* temporary stack now in right order, link to top of real stack */
      st->firstbox->next = st->firstline[st->current_queue] ;
      if (st->firstline[st->current_queue])
         st->firstline[st->current_queue]->prev = st->firstbox ;
      else
         st->lastline[st->current_queue] = st->firstbox ;
      st->firstline[st->current_queue] = st->lastbox ;
   }
   else
   {
      /* everything is in right order, just link them together */
      st->firstbox->prev = st->lastline[st->current_queue] ;
      if (st->lastline[st->current_queue])
         st->lastline[st->current_queue]->next = st->firstbox ;
      else
         st->firstline[st->current_queue] = st->firstbox ;
      st->lastline[st->current_queue] = st->lastbox ;
   }

   /* reset the pointers, to signify that temporary stack is empty */
   st->lastbox = st->firstbox = NULL ;
}



/*
 * Pushes 'line' onto the REXX stack, lifo, and sets 'lastline' to
 *    point to the new line. The line is put on top of the current
 *    buffer.
 */
int stack_lifo( tsd_t *TSD, streng *line, streng *queue_name )
{
   stacklineptr newbox=NULL ;
   stk_tsd_t *st;
   int idx;
   streng *server_name;
   int server_address,portno;

   st = TSD->stk_tsd;
   if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) 
   ||    st->external_queue_portno == 0 )
   {
      if ( queue_name )
      {
         if ( ( idx = find_queue( TSD, queue_name ) ) == UNKNOWN_QUEUE )
         {
            return 9;
         }
      }
      else
         idx = st->current_queue;
      /*
       * we are to use internal queues
       */
      newbox = (stacklineptr)MallocTSD(sizeof(stackline)) ;
      if (st->lastline[idx])
      {
         st->lastline[idx]->next = newbox ;
         newbox->prev = st->lastline[idx] ;
      }
      else
      {
         newbox->prev = NULL ;
         st->firstline[idx] = newbox ;
      }
      if (!line)
         st->buffers[idx]++ ;
   
      newbox->next = NULL ;
      newbox->contents = line ;
      st->lastline[idx] = newbox ;
   }
#if !defined(NO_EXTERNAL_QUEUES)
   else
   {
      if ( queue_name )
      {
         if ( parse_queue( TSD, queue_name, &server_name, &server_address, &portno ) != 0 )
            exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
         st->external_queue_socket = get_socket_details_and_connect( TSD, server_name, server_address, portno );
         DROPSTRENG( st->external_server_name );
         st->external_server_name = server_name;
      }
      else
      {
         st->external_queue_socket = get_socket_details_and_connect( TSD, st->external_server_name, st->external_server_address, st->external_queue_portno );
      }
      queue_line_lifo_to_rxstack( TSD, st->external_queue_socket, line ) ;
   }
#else
   server_name = server_name; /* keep compiler happy */
   server_address = server_address; /* keep compiler happy */
   portno = portno; /* keep compiler happy */
#endif
   return 0;
}


/*
 * Puts 'line' on the REXX stack, fifo. This routine is similar to
 *    stack_lifo but the differences are big enough to have a separate
 *    routine. The line is put in the bottom of the current buffer,
 *    that is just above the uppermost buffer mark, or at the bottom
 *    of the stack, if there are no buffer marks.
 */
int stack_fifo( tsd_t *TSD, streng *line, streng *queue_name )
{
   stacklineptr newbox=NULL, ptr=NULL ;
   stk_tsd_t *st;
   int idx;
   streng *server_name;
   int server_address,portno;

   st = TSD->stk_tsd;
   if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) 
   ||    st->external_queue_portno == 0 )
   {
      if ( queue_name )
      {
         if ( ( idx = find_queue( TSD, queue_name ) ) == UNKNOWN_QUEUE )
         {
            return 9;
         }
      }
      else
         idx = st->current_queue;
      /*
       * we are to use internal queues
       */
      if (!line)
         st->buffers[idx]++ ;
   
      /* Bug: inserts into bottom of stack, not bottom of current buffer */
      newbox = (stacklineptr)MallocTSD(sizeof(stackline)) ;
      newbox->prev = newbox->next = NULL ;
      newbox->contents = line ;
   
      for (ptr=st->lastline[idx];(ptr)&&(ptr->contents);ptr=ptr->prev) ;
   
      if (ptr)
      {
         newbox->prev = ptr ;
         newbox->next = ptr->next ;
         if (ptr->next)
            ptr->next->prev = newbox ;
         else
            st->lastline[idx] = newbox ;
         ptr->next = newbox ;
      }
      else
      {
         newbox->next = st->firstline[idx] ;
         st->firstline[idx] = newbox ;
         if (newbox->next)
            newbox->next->prev = newbox ;
         if (!st->lastline[idx])
            st->lastline[idx] = newbox ;
      }
   }
#if !defined(NO_EXTERNAL_QUEUES)
   else
   {
      if ( queue_name )
      {
         if ( parse_queue( TSD, queue_name, &server_name, &server_address, &portno ) != 0 )
            exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
         st->external_queue_socket = get_socket_details_and_connect( TSD, server_name, server_address, portno );
         DROPSTRENG( st->external_server_name );
         st->external_server_name = server_name;
      }
      else
      {
         st->external_queue_socket = get_socket_details_and_connect( TSD, st->external_server_name, st->external_server_address, st->external_queue_portno );
      }
      queue_line_fifo_to_rxstack( TSD, st->external_queue_socket, line ) ;
   }
#else
   server_name = server_name; /* keep compiler happy */
   server_address = server_address; /* keep compiler happy */
   portno = portno; /* keep compiler happy */
#endif
   return 0;
}


/*
 * Removes one (or several) buffers from the stack. The number of
 *    buffers to remove is decided by 'number', which is the number of
 *    buffers to remain on the stack. The global variable buffer
 *    contains the number of buffermarks currently in the stack.
 *
 * When number==2, buffer 0 (the implisit), and buffer 1 (the first
 *    buffer created by the user) are the only that remain. All lines
 *    from buffermark 2 (inclusive) and above are removed. Remember
 *    that buffer N is the lines following the N'th buffermark, until
 *    end-of-stack or another buffermark is reached.
 *
 * If number is less than zero, then abs(number) buffers is removed
 *    from the top of the stack (or until the stack is empty).

 * If the number is zero, the stack is emptied. If a buffer with a
 *    number that is higher than the current buffer is spesified,
 *    errorcode is retured.
 */

int drop_buffer( const tsd_t *TSD, int number )
{
   stacklineptr ptr=NULL, nptr=NULL ;
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   if (number<0)
      number = (st->buffers[st->current_queue] + number + 1) ;

   assert(st->buffers[st->current_queue]>=0) ;
   for (ptr=st->lastline[st->current_queue]; (ptr) && (number<=st->buffers[st->current_queue]); ptr=nptr)
   {
      nptr = ptr->prev ;
      if (ptr->contents)
         Free_stringTSD(ptr->contents) ;
      else
         st->buffers[st->current_queue]-- ;
      FreeTSD(ptr) ;
      if (nptr)
         nptr->next = NULL ;
   }
   if ((st->lastline[st->current_queue]=ptr) == NULL)
      st->firstline[st->current_queue] = NULL ;

   if (st->buffers[st->current_queue]<0)
      st->buffers[st->current_queue] = 0 ;

   return st->buffers[st->current_queue] ;
}


/*
 * Fetches a line from the top of the stack. If the current buffer
 *    does not contain any lines, it is removed and the second current
 *    buffer is search for a line etc. If there isn't any lines in the
 *    stack, a line is read from the standard input.
 */
streng *popline( tsd_t *TSD, streng *queue_name, int *result, unsigned long waitflag )
{
   streng *contents=NULL ;
   stacklineptr line=NULL ;
   stk_tsd_t *st;
   int need_line_from_stdin=0,idx;
   streng *server_name;
   int server_address,portno;

   st = TSD->stk_tsd;

   if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) 
   ||    st->external_queue_portno == 0 )
   {
      if ( queue_name )
      {
         if ( ( idx = find_queue( TSD, queue_name ) ) == UNKNOWN_QUEUE )
         {
            if ( result ) *result = -9;
            return NULL;
         }
      }
      else
         idx = st->current_queue;
      /*
       * we are to use internal queues
       */
      if ((line=st->lastline[idx]) != NULL)
      {
         contents = line->contents ;
         st->lastline[idx] = line->prev ;
         if (!line->prev)
            st->firstline[idx] = NULL ;
         else
            line->prev->next = NULL ;
   
         FreeTSD(line) ;
         if (!contents)
         {
            st->buffers[idx]-- ;
            contents = popline( TSD, NULL, NULL, waitflag ) ;
         }
      }
      else
         need_line_from_stdin = 1;
   }
#if !defined(NO_EXTERNAL_QUEUES)
   else
   {
      if ( queue_name )
      {
         if ( parse_queue( TSD, queue_name, &server_name, &server_address, &portno ) != 0 )
            exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
         st->external_queue_socket = get_socket_details_and_connect( TSD, server_name, server_address, portno );
         DROPSTRENG( st->external_server_name );
         st->external_server_name = server_name;
      }
      else
      {
         st->external_queue_socket = get_socket_details_and_connect( TSD, st->external_server_name, st->external_server_address, st->external_queue_portno );
      }
      /*
       * contents created by get_queue_from_stack(); up to caller to free it
       * rc can be 0; line pulled off stack, or 1; queue empty. Error; 2
       * handled by get_line_from_rxstack()
       */
      if ( get_line_from_rxstack( TSD, st->external_queue_socket, &contents ) == 1 )
         need_line_from_stdin = 1;
   }
#else
   server_name = server_name; /* keep compiler happy */
   server_address = server_address; /* keep compiler happy */
   portno = portno; /* keep compiler happy */
#endif

   if ( need_line_from_stdin )
   {
      if ( TSD->called_from_saa )
         contents = NULL; /* indicate that queue is empty */
      else
      {
         int rc = HOOK_GO_ON ;
         if (TSD->systeminfo->hooks & HOOK_MASK(HOOK_PULL))
            rc = hookup_input( TSD, HOOK_PULL, &contents ) ;
   
         if (rc==HOOK_GO_ON)
            contents = readkbdline( TSD ) ;
   
         assert( contents ) ;
      }
   }
   if ( result ) *result = 0 ;
   return contents;
}


/*
 * Counts the lines in the stack.
 */
int lines_in_stack( tsd_t *TSD, streng *queue_name )
{
   streng *server_name;
   int server_address,portno;
   stacklineptr ptr=NULL ;
   int lines=0,idx ;
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) 
   ||    st->external_queue_portno == 0 )
   {
      if ( queue_name )
      {
         if ( ( idx = find_queue( TSD, queue_name ) ) == UNKNOWN_QUEUE )
         {
            return -9;
         }
      }
      else
         idx = st->current_queue;
      /*
       * we are to use internal queues
       */
      ptr = st->firstline[idx] ;
      for (lines=0;ptr;ptr=ptr->next)
      {
         if (ptr->contents)
            lines++ ;
      }
   }
#if !defined(NO_EXTERNAL_QUEUES)
   else
   {
      if ( queue_name )
      {
         if ( parse_queue( TSD, queue_name, &server_name, &server_address, &portno ) != 0 )
            exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
         st->external_queue_socket = get_socket_details_and_connect( TSD, server_name, server_address, portno );
         DROPSTRENG( st->external_server_name );
         st->external_server_name = server_name;
      }
      else
      {
         st->external_queue_socket = get_socket_details_and_connect( TSD, st->external_server_name, st->external_server_address, st->external_queue_portno );
      }
      /*
       * Any errors are handled by get_number_in_queue_from_rxstack()
       */
      lines = get_number_in_queue_from_rxstack( TSD, st->external_queue_socket );
   }
#else
   server_name = server_name; /* keep compiler happy */
   server_address = server_address; /* keep compiler happy */
   portno = portno; /* keep compiler happy */
#endif
   return lines ;
}


/*
 * Returns boolean expression: is the stack empty?
 */
int stack_empty( const tsd_t *TSD )
{
   stacklineptr ptr=NULL ;
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   for (ptr=st->firstline[st->current_queue];ptr;ptr=ptr->next)
      if (ptr->contents)
         return 0 ;

   return 1 ;
}


#ifdef TRACEMEM
/*
 * Marks all chunks of dynamic allocated memory that are allocated
 *   to the stack subsystem.
 */
void mark_stack( const tsd_t *TSD )
{
   stacklineptr ptr=NULL ;
   stk_tsd_t *st;
   int i;

   st = TSD->stk_tsd;
   for ( i = 0; i < NUMBER_QUEUES; i++ )
   {
      if ( st->queuename[i] )
      {
         markmemory(st->queuename[i],TRC_STACKBOX) ;
         if ( st->real_queue[i] == 1 )
         {
            for (ptr=st->firstline[i];ptr;ptr=ptr->next)
            {
               markmemory(ptr,TRC_STACKBOX) ;
               if (ptr->contents)
                  markmemory(ptr->contents,TRC_STACKLINE) ;
            }
         }
      }
   }
}
#endif


/*
 * Creates a new buffer, by putting a buffer mark at the top of the
 *    stack. (This could be implemented as {push_line(NULL)} and a
 *    special test for (line==NULL) in push_line())
 */
int make_buffer( tsd_t *TSD )
{
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   stack_lifo( TSD, NULL, NULL ) ;
   return st->buffers[st->current_queue] ;
}



/*
 * Dumps the contents of the stack to standard error. Buffer marks are
 *    indicated in the printout.
 */
void type_buffer( tsd_t *TSD )
{
   stacklineptr ptr=NULL ;
   char *cptr=NULL, *stop=NULL ;
   int counter=0 ;
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   if (TSD->stddump == NULL)
      return;
   fprintf(TSD->stddump,"==> Lines: %d\n", lines_in_stack( TSD, NULL )) ;
   fprintf(TSD->stddump,"==> Buffer: %d\n", counter=st->buffers[st->current_queue]) ;
   for (ptr=st->lastline[st->current_queue]; ptr; ptr=ptr->prev)
   {
      if (ptr->contents)
      {
         putc( '"', TSD->stddump ) ;
         stop = Str_end(ptr->contents) ;
         for (cptr=ptr->contents->value; cptr<stop; cptr++)
            putc( (isprint(*cptr) ? (*cptr) : '?'), TSD->stddump ) ;

         putc( '"', TSD->stddump ) ;
         putc( REGINA_EOL, TSD->stddump ) ;
      }
      else
         fprintf(TSD->stddump,"==> Buffer: %d\n",--counter) ; }

   fprintf(TSD->stddump,"==> End of Stack\n") ;
   fflush(TSD->stddump) ;
}

#if !defined(NO_EXTERNAL_QUEUES)
/*
 * The following functions allow interfacing to the external
 * queue process; rxstack
 */
static int get_socket_details_and_connect( tsd_t *TSD, streng *server_name, int server_address, int portno )
{
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   DEBUGDUMP(printf("In get_socket_details_and_connect: st->external_server_address %d st->external_queue_portno %d server_address %d portno %d\n",st->external_server_address,st->external_queue_portno,server_address,portno  ););

   if ( st->external_server_address == 0
   &&   st->external_queue_portno == 0 )
   {
      /*
       * If the current values for server_address and portno
       * are zero, we don't have a connection or an explicit
       * queue, so use the defaults: 127.0.0.1 and 5656
       */
      portno = get_default_port_number();
      server_address = get_default_server_address();
   }
   DEBUGDUMP(printf("In get_socket_details_and_connect: server_address %d portno %d\n",server_address,portno  ););

   if ( portno != st->external_queue_portno
   ||   server_address != st->external_server_address )
   {
      /*
       * If the current portno is zero, don't disconnect because
       * we don't have a current connection
       */
      if ( st->external_queue_portno == 0 )
      {
         init_external_queue(TSD);
      }
      else
         disconnect_from_rxstack( TSD, st->external_queue_socket );
      DEBUGDUMP(printf("In get_socket_details_and_connect: connecting...%s\n", ( st->external_queue_portno != 0 ) ? "disconnect first" : "" ););
      st->external_queue_socket = connect_to_rxstack( TSD, portno, server_name, server_address );
      st->external_queue_portno = portno;
      st->external_server_address = server_address;
   }
   if ( st->external_queue_socket < 0 )
      exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_CANT_CONNECT, tmpstr_of(TSD, server_name), portno, strerror( errno ) );
   return st->external_queue_socket;
}
#endif

/*
 * The following functions control multiple, internal queues
 */


static int find_free_slot( const tsd_t *TSD )
{
   int i,idx = UNKNOWN_QUEUE;
   stk_tsd_t *st;

   st = TSD->stk_tsd;
   for ( i = 0; i < NUMBER_QUEUES; i++ )
   {
      if ( st->queuename[i] == NULL )
      {
         idx = i;
         break;
      }
   }
   return idx;
}

/*
 * Create a new queue
 * RXQUEUE( 'Create' )
 */
int create_queue( tsd_t *TSD, streng *queue_name, streng **result )
{
   int idx=UNKNOWN_QUEUE,false_queue_idx=UNKNOWN_QUEUE;
   streng *new_queue=NULL;
   stk_tsd_t *st;
   char buf[50];
   streng *server_name;
   int server_address,portno;
   int rc=0;

   st = TSD->stk_tsd;
#if defined(NO_EXTERNAL_QUEUES)
   server_name = server_name; /* keep compiler happy */
   server_address = server_address; /* keep compiler happy */
   portno = portno; /* keep compiler happy */
#else
   if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) )
#endif
   {
      if ( queue_name == NULL )
      {
         /*
          * Create a unique queue name
          */
         sprintf(buf,"S%d%ld%d", getpid(), clock(), find_free_slot( TSD ) );
         new_queue = Str_cre_TSD( TSD, buf );
      }
      else
      {
         if ( ( idx = find_queue( TSD, queue_name ) ) == UNKNOWN_QUEUE )
         {
            /*
             * No queue of that name, so use it.
             */
            new_queue = queue_name;
         }
         else
         {
            /*
             * If we tried to create SESSION, then return 5
             */
            if ( idx == 0 )
               rc = 5;
            else
            {
               /*
                * If the queue we found is a false queue, we can still
                * use the supplied name and the slot
                */
               if ( st->real_queue[idx] == 0 )
               {
                  new_queue = queue_name;
                  false_queue_idx = idx;
               }
               else
               {
                  /*
                   * Create a unique queue name
                   */
                  sprintf(buf,"S%d%ld%d", getpid(), clock(), find_free_slot( TSD ) );
                  new_queue = Str_cre_TSD( TSD, buf );
               }
            }
         }
      }
      /*
       * Find the first slot that hasn't been used, or use
       * the false queue; provided we haven't tried to create SESSION
       */
      if ( rc == 0 )
      {
         if ( false_queue_idx == UNKNOWN_QUEUE )
            idx = find_free_slot( TSD );
         else
            idx = false_queue_idx;
      
         if ( idx == UNKNOWN_QUEUE )
         {
            exiterror( ERR_STORAGE_EXHAUSTED, 0 );
         }
         st->queuename[idx] = Str_upper( Str_dupTSD( new_queue ) ) ;
         st->current_queue = idx;
         st->real_queue[idx] = 1;
         /*
          * result created here; up to caller to free it
          */
         *result = Str_dupTSD( st->queuename[idx] );
      }
   }
#if !defined(NO_EXTERNAL_QUEUES)
   else
   {
      /*
       * Validate the supplied queue name. We only do this for
       * external queues
       */
      if ( queue_name )
      {
         if ( parse_queue( TSD, queue_name, &server_name, &server_address, &portno ) != 0 )
            exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
         st->external_queue_socket = get_socket_details_and_connect( TSD, server_name, server_address, portno );
         DROPSTRENG( st->external_server_name );
         st->external_server_name = server_name;
      }
      else
      {
         st->external_queue_socket = get_socket_details_and_connect( TSD, st->external_server_name, st->external_server_address, st->external_queue_portno );
      }
      /*
       * Can't get here if st->external_queue_socket is an error
       */
      rc = create_queue_on_rxstack( TSD, st->external_queue_socket, queue_name, result );
   }
#endif
   return rc;
}

/*
 * Delete a queue
 * RXQUEUE( 'Delete' )
 */
int delete_queue( tsd_t *TSD, streng *queue_name )
{
   int idx,rc=0;
   stk_tsd_t *st;
   streng *server_name;
   int server_address,portno;

   st = TSD->stk_tsd;
#if defined(NO_EXTERNAL_QUEUES)
   server_name = server_name; /* keep compiler happy */
   server_address = server_address; /* keep compiler happy */
   portno = portno; /* keep compiler happy */
#else
   if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) )
#endif
   {
      if ( ( idx = find_queue( TSD, queue_name ) ) == UNKNOWN_QUEUE )
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
            if ( st->real_queue[idx] == 0 )
               rc = 9;
            else
            {
               /*
                * Delete the contents of the queue
                * and mark it as gone.
                */
               delete_an_internal_queue( TSD, idx );
               rc = 0;
            }
         }
      }
   }
#if !defined(NO_EXTERNAL_QUEUES)
   else
   {
      /*
       * Validate the supplied queue name. We only do this for
       * external queues
       */
      if ( parse_queue( TSD, queue_name, &server_name, &server_address, &portno ) != 0 )
         exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
      if ( PSTRENGLEN( queue_name ) )
      {
         st->external_queue_socket = get_socket_details_and_connect( TSD, server_name, server_address, portno );
         rc = delete_queue_from_rxstack( TSD, st->external_queue_socket, queue_name );
      }
      else
         exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
      DROPSTRENG( st->external_server_name );
      st->external_server_name = server_name;
   }
#endif
   return rc;
}

/*
 * Return the name of the current queue
 * RXQUEUE( 'Get' )
 * Returns queue name even if it is a false queue
 */
streng *get_queue( tsd_t *TSD )
{
   streng *result;
   stk_tsd_t *st;
   int rc=0;

   st = TSD->stk_tsd;
#if !defined(NO_EXTERNAL_QUEUES)
   if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) )
#endif
   {
      /*
       * result created here; up to caller to free it
       */
      result = Str_dupTSD( st->queuename[st->current_queue] );
   }
#if !defined(NO_EXTERNAL_QUEUES)
   else
   {
      st->external_queue_socket = get_socket_details_and_connect( TSD, st->external_server_name, st->external_server_address, st->external_queue_portno );
      /*
       * result created by get_queue_from_stack(); up to caller to free it
       */
      rc = get_queue_from_rxstack( TSD, st->external_queue_socket, &result );
   }
#endif
   return result;
}

/*
 * Set the current queue
 * RXQUEUE( 'Set' )
 * Object Rexx allows the queue to be set to an unknown queue
 * This is stupid; the user should be told its not valid, and
 * return an empty string.
 * But we validly set the false queue :-(
 */
streng *set_queue( tsd_t *TSD, streng *queue_name )
{
   stk_tsd_t *st;
   streng *result=NULL;
   int idx, prev_idx;
   streng *server_name;
   int server_address,portno;

   st = TSD->stk_tsd;
#if defined(NO_EXTERNAL_QUEUES)
   server_name = server_name; /* keep compiler happy */
   server_address = server_address; /* keep compiler happy */
   portno = portno; /* keep compiler happy */
#else
   if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) )
#endif
   {
      /*
       * result created here; up to caller to free it
       */
      prev_idx = st->current_queue;
      if ( ( idx = find_queue( TSD, queue_name ) ) == UNKNOWN_QUEUE )
      {
         /*
          * We didn't find a real or a false queue, so create
          * a false queue
          */
         idx = find_free_slot( TSD );
         if ( idx == UNKNOWN_QUEUE )
            exiterror( ERR_STORAGE_EXHAUSTED, 0 );
         else
         {
            st->queuename[idx] = Str_upper( Str_dupTSD( queue_name ) ) ;
            st->current_queue = idx;
            st->real_queue[idx] = 0; /* false queue */
         }
      }
      st->current_queue = idx;
      result = st->queuename[prev_idx] ;
   }
#if !defined(NO_EXTERNAL_QUEUES)
   else
   {
      /*
       * Validate the supplied queue name. We only do this for
       * external queues
       */
      if ( parse_queue( TSD, queue_name, &server_name, &server_address, &portno ) != 0 )
         exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
      if ( PSTRENGLEN( queue_name ) )
      {
         st->external_queue_socket = get_socket_details_and_connect( TSD, server_name, server_address, portno );
         set_queue_in_rxstack( TSD, st->external_queue_socket, queue_name, &result );
      }
      else
         exiterror( ERR_EXTERNAL_QUEUE, ERR_RXSTACK_INVALID_QUEUE, tmpstr_of(TSD, queue_name ) ) ;
      DROPSTRENG( st->external_server_name );
      st->external_server_name = server_name;
   }
#endif
   return result;
}
