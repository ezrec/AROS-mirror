#ifndef lint
static char *RCSid = "$Id$";
#endif

/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1993-1994  Anders Christensen <anders@pvv.unit.no>
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
#include <string.h>

#ifdef HAVE_CTYPE_H
# include <ctype.h>
#endif

#if defined(VMS)
# define fork() vfork()
# ifdef posix_do_command
#  undef posix_do_command
# endif
# define posix_do_command __regina_vms_do_command
#endif

#if defined(DOS) || defined(WIN32) || defined(OS2) || defined(_AMIGA) || defined(__AROS__) || defined(MAC)
# ifdef posix_do_command
#  undef posix_do_command
# endif
# define posix_do_command __regina_dos_do_command
#endif


struct envir
{
   streng *name ;
   int type ;
   int subtype ;
   struct envir *next, *prev ;
};

void add_envir( tsd_t *TSD, streng *name, int type, int subtype )
{
   struct envir *ptr=NULL ;

   ptr = MallocTSD( sizeof(struct envir)) ;
   ptr->name = name ;
   ptr->type = type ;
   ptr->subtype = subtype ;
   ptr->prev = TSD->firstenvir ;
   ptr->next = NULL ;
   TSD->firstenvir = ptr ;
   if (ptr->prev)
      ptr->prev->next = ptr ;
}


#ifdef TRACEMEM
static void markenvir( const tsd_t *TSD )
{
   struct envir *eptr=NULL ;

   eptr = TSD->firstenvir ;
   for (; eptr; eptr=eptr->prev )
   {
      markmemory( eptr, TRC_ENVIRBOX ) ;
      markmemory( eptr->name, TRC_ENVIRNAME ) ;
   }
}
#endif /* TRACEMEM */


static struct envir *find_envir( const tsd_t *TSD, const streng *name )
{
   struct envir *ptr=NULL ;

   for (ptr=TSD->firstenvir; ptr; ptr=ptr->prev)
      if (!Str_cmp(ptr->name, name))
         return ptr ;

   return NULL ;
}

/*
 * This function is used to determine if an environment exists. It is the
 * external equivalent of find_envir()
 */
int envir_exists( const tsd_t *TSD, const streng *name )
{
   if ( find_envir( TSD, name ) == NULL )
      return 0;
   else
      return 1;
}


void del_envir( tsd_t *TSD, const streng *name )
{
   struct envir *ptr=NULL ;

   ptr = find_envir( TSD, name ) ;
   if (ptr)
   {
      if (ptr->prev)
         ptr->prev->next = ptr->next ;
      if (ptr->next)
         ptr->next->prev = ptr->prev ;
      if (TSD->firstenvir==ptr)
         TSD->firstenvir = ptr->prev ;
   }
}


int init_envir( tsd_t *TSD )
{
   add_envir( TSD, Str_creTSD("COMMAND"), ENVIR_SHELL, SUBENVIR_COMMAND ) ;
   add_envir( TSD, Str_creTSD("SYSTEM"), ENVIR_SHELL, SUBENVIR_SYSTEM ) ;
   add_envir( TSD, Str_creTSD("OS2ENVIRONMENT"), ENVIR_SHELL, SUBENVIR_SYSTEM ) ;
   add_envir( TSD, Str_creTSD("ENVIRONMENT"), ENVIR_SHELL, SUBENVIR_SYSTEM ) ;
   add_envir( TSD, Str_creTSD("CMD"), ENVIR_SHELL, SUBENVIR_COMMAND ) ;
   add_envir( TSD, Str_creTSD("PATH"), ENVIR_SHELL, SUBENVIR_PATH ) ;
#ifdef TRACEMEM
   regmarker( TSD, markenvir ) ;
#endif
   return(1);
}


static int get_io_flag( tsd_t *TSD, streng *command, streng **rxqueue )
{
   int length=0, i=0, pos=0 ;
   int flag=0,have_space=0 ;
   int qname_start,qname_end;
   streng *tmpq=NULL;

   flag = REDIR_NONE ;
   /*
    * All I/O redirection valid with INTERNAL queues only
    */
   if ((length=Str_len(command)) > 5
   &&  (get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES )
      ||   external_queues_used( TSD ) == 0 ) )
   {
      if ((!memcmp(command->value,"lifo>",5)) ||
          (!memcmp(command->value,"LIFO>",5)))
               flag |= REDIR_INPUT ;

      if ((!memcmp(command->value+length-5,">lifo",5))
      ||  (!memcmp(command->value+length-5,">LIFO",5)))
      {
         flag |= REDIR_OUTLIFO ;
         command->len -= 5;
      }
      else
      {
         if ( ( !memcmp( command->value+length-5, ">fifo" ,5 ) )
         ||   ( !memcmp( command->value+length-5, ">FIFO", 5 ) ) )
         {
            flag |= REDIR_OUTFIFO ;
            command->len -= 5;
         }
         else
         {
            if ( length >= 8 )
            {
               int j = 0;

               for ( i = 0, pos = -1; i < length; i++ )
               {
                  if ( *(command->value + i ) == '|' )
                  {
                     pos = i;
                     /* don't break here, as we want the last '|' */
                  }
               }
               if ( pos != -1 )
               {
                  /* allow "|" [whitespace] "rxqueue" [whitespace[args]] */
                  /* "|" already checked */
                  for ( i = pos + 1, j = 0; i < length; i++ )
                  {
                     if ( !isspace(command->value[i] ) )
                        break;
                  }
                  if ( i+7 <= length )
                  {
                     if (mem_cmpic( command->value + i, "RXQUEUE", 7 ) == 0 )
                     {
                        i += 7;
                        for ( ; i < length; i++ )
                        {
                           if ( !isspace( command->value[i] ) )
                              break;
                           have_space = 1;
                        }
                        if ( i == length )
                        {
                            flag |= REDIR_OUTFIFO ;
                            command->len = pos;
                        }
                        else if ( have_space )
                        {
                            if ( *(command->value + i) == '/' )
                            {
                               /*
                                * Only a switch, not a queuename
                                */
                               if ( i+6 <= length
                               &&   mem_cmpic( command->value + i, "/CLEAR", 6 ) == 0 )
                               {
                                  flag |= REDIR_CLEAR;
                                  flag |= REDIR_OUTFIFO ;
                               }
                               else if ( i+5 <= length )
                               {
                                  if (mem_cmpic( command->value + i, "/FIFO", 5 ) == 0 )
                                     flag |= REDIR_OUTFIFO ;
                                  else if (mem_cmpic( command->value + i, "/LIFO", 5 ) == 0 )
                                     flag |= REDIR_OUTLIFO ;
                                  else
                                     flag |= REDIR_OUTFIFO ;
                                  /*
                                   * Let the queue name be determined by the caller
                                   */
                               }
                            }
                            else
                            {
                               /*
                                * First word must be a queue name, optionally
                                * followed by a switch
                                */
                               have_space = 0;
                               qname_start = i;
                               for ( ; i < length; i++ )
                               {
                                  if ( isspace( command->value[i] ) )
                                  {
                                     have_space = 1;
                                     qname_end = i;
                                     /*
                                      * Get queuename from RXQUEUE env
                                      * variable, or if not set, use SESSION
                                      */
                                     tmpq = Str_make_TSD( TSD, qname_end - qname_start );
                                     tmpq->len = qname_end - qname_start;
                                     memcpy( tmpq->value, command->value + qname_start, qname_end - qname_start );
                                     break;
                                  }
                               }
                               if ( have_space )
                               {
                                  /*
                                   * Eat up all spaces. If we have
                                   * any non-spaces left, it should be
                                   * a switch
                                   */
                                  for ( ; i < length; i++ )
                                  {
                                     if ( !isspace( command->value[i] ) )
                                        break;
                                  }
                                  if ( i+6 <= length
                                  &&   mem_cmpic( command->value + i, "/CLEAR", 6 ) == 0 )
                                  {
                                     flag |= REDIR_CLEAR;
                                     flag |= REDIR_OUTFIFO ;
                                  }
                                  else if ( i+5 <= length )
                                  {
                                     if (mem_cmpic( command->value + i, "/FIFO", 5 ) == 0 )
                                        flag |= REDIR_OUTFIFO ;
                                     else if (mem_cmpic( command->value + i, "/LIFO", 5 ) == 0 )
                                        flag |= REDIR_OUTLIFO ;
                                     else
                                        flag |= REDIR_OUTFIFO ;
                                  }
                               }
                               else
                               {
                                  /*
                                   * Only have a queue name
                                   */
                                  tmpq = Str_make_TSD( TSD, length - qname_start );
                                  tmpq->len = length - qname_start;
                                  memcpy( tmpq->value, command->value + qname_start, length - qname_start );
                                  flag |= REDIR_OUTFIFO ;
                               }
                            }
                            command->len = pos;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   if (flag & REDIR_INPUT)
   {
      for(i=5; i<Str_len(command); i++ ) /* avoid buffer overrun */
         command->value[i-5]=command->value[i] ;
      command->len -= 5 ;
   }
   *rxqueue = tmpq;
   return flag ;
}


streng *perform( tsd_t *TSD, const streng *command, const streng *envir, cnodeptr this )
{
   int rc=0, io_flag=0, clearq=0;
   struct envir *eptr=NULL ;
   streng *retstr=NULL ;
   streng *rxqueue=NULL;
   streng *cmd=Str_dupTSD(command);
   streng *saved_queue=NULL;
   char *rxq=NULL;

   eptr = find_envir( TSD, envir ) ;

   if (eptr)
   {
      switch (eptr->type)
      {
         case ENVIR_PIPE:
            retstr = SubCom( TSD, cmd, envir, &rc ) ;
            break ;

         case ENVIR_SHELL:
            io_flag = get_io_flag( TSD, cmd, &rxqueue ) ;
            /*
             * Save the current queue name
             * Then change it to the value of rxqueue
             * Only for internal queues
             */
            if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) )
            {
               /*
                * If no queuename specified,
                * get queuename from RXQUEUE env
                * variable, or if not set, use SESSION
                */
               if ( rxqueue == NULL )
               {
                  if ( ( rxq = getenv("RXQUEUE") ) == NULL )
                  {
                     rxqueue = Str_cre_TSD( TSD, "SESSION" );
                  }
                  else
                  {
                     rxqueue = Str_cre_TSD( TSD, rxq );
                  }
               }
               saved_queue = set_queue( TSD, rxqueue );
               if ( io_flag & REDIR_CLEAR )
               {
                  clearq = 1;
                  io_flag -= REDIR_CLEAR;
               }
            }
            rc = posix_do_command( TSD, cmd, io_flag, eptr->subtype ) ;
            /*
             * Change the current queue name back
             * to the saved name
             */
            if ( get_options_flag( TSD->currlevel, EXT_INTERNAL_QUEUES ) )
            {
               if ( clearq )
                  drop_buffer( TSD, 0);
               set_queue( TSD, saved_queue );
               if ( rxqueue != NULL )
                  Free_stringTSD( rxqueue );
            }
            retstr = int_to_streng( TSD, rc ) ;
            break ;

         default:
            exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__ )  ;
      }
   }
   else
   {
#if 0
      rc = -3 ;
      retstr = nullstringptr() ;
#else
      retstr = SubCom( TSD, cmd, envir, &rc ) ;
#endif
   }

   setvalue(TSD,&RC_name,Str_dupTSD(retstr)) ;

   if (rc && this)
   {
      trap *traps ;
      int type ;

      traceerror( TSD, this, rc ) ;
      traps = gettraps( TSD, TSD->currlevel ) ;
      type = (rc>0) ? SIGNAL_ERROR : SIGNAL_FAILURE ;

      if ((type==SIGNAL_FAILURE) && (traps[type].on_off))
         condition_hook( TSD, type, rc, 0, this->lineno, Str_dupTSD(cmd), NULL ) ;
      else if (traps[SIGNAL_ERROR].on_off)
         condition_hook( TSD, SIGNAL_ERROR, rc, 0, this->lineno, Str_dupTSD(cmd), NULL ) ;
   }

   Free_stringTSD( cmd ) ;
   return retstr ;
}
