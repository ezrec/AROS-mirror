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

#if defined(WIN32)
# ifdef _MSC_VER
#  if _MSC_VER >= 1100
/* Stupid MSC can't compile own headers without warning at least in VC 5.0 */
#   pragma warning(disable: 4115 4201 4214)
#  endif
# endif
# include <windows.h>
# ifdef _MSC_VER
#  if _MSC_VER >= 1100
#   pragma warning(default: 4115 4201 4214)
#  endif
# endif
#endif

#include "rexx.h"
/*#include <limits.h> */
#include <stdio.h>

#include <string.h>
#include <signal.h>
/* #include <ctype.h>   */
#include <errno.h>
#ifdef HAVE_ASSERT_H
# include <assert.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#if defined(VMS)
# define fork() vfork()
# ifdef  posix_do_command
#  undef posix_do_command
# endif
# define posix_do_command __regina_vms_do_command
#endif

#if defined(__WINS__) || defined(__EPOC32__)
# define REGINA_MAX_BUFFER_LENGTH 256
#else
# define REGINA_MAX_BUFFER_LENGTH 4096
#endif

#define STD_IO    0x00
#define simQUEUE  0x01
#define simLIFO   0x02
#define simFIFO   0x04
#define STREAM    0x08
#define STEM      0x10
#define STRING    0x20
#define QUEUE     0x40
#define LIFO      0x80
#define FIFO      0x100

#if defined(_POSIX_PIPE_BUF) && !defined(PIPE_BUF)
# define PIPE_BUF _POSIX_PIPE_BUF
#endif

typedef struct { /* shl_tsd: static variables of this module (thread-safe) */
   char         *cbuff ;
   int           child ;
   int           status ;
   int           running ;
   void         *AsyncInfo ;
   unsigned char IObuf[4096]; /* write cache */
   unsigned      IOBused;
} shl_tsd_t; /* thread-specific but only needed by this module. see
              * init_shell
              */

/* init_shell initializes the module.
 * Currently, we set up the thread specific data.
 * The function returns 1 on success, 0 if memory is short.
 */
int init_shell( tsd_t *TSD )
{
   shl_tsd_t *st;

   if (TSD->shl_tsd != NULL)
      return(1);

   if ((st = TSD->shl_tsd = MallocTSD(sizeof(shl_tsd_t))) == NULL)
      return(0);
   memset(st,0,sizeof(shl_tsd_t));  /* correct for all values */
   return(1);
}

const streng *stem_access( tsd_t *TSD, environpart *e, int pos,
                                                           const streng *value)
/* puts e->name+"."+itoa(pos) to e->currname and accesses this variable.
 * value is NULL to access the current value or non-NULL to set the new value.
 * The return value is NULL if a new value is set or the old one.
 */
{
   int stemlen, leaflen ;

   stemlen = Str_len( e->name ) ;
   leaflen = sprintf( e->currname->value + stemlen, "%d", pos ) ;

   e->currname->len = stemlen + leaflen ;

   if (value == NULL)
#ifndef NON_ANSI_BEFORE_NOV_2001
      return( get_it_anyway_compound( TSD, e->currname ) ) ;
#else
      return( getdirvalue_compound( TSD, e->currname ) ) ;
#endif

   setdirvalue_compound( TSD, e->currname, Str_dupTSD( value ) ) ;
   return( NULL ) ;
}

void open_env_io( tsd_t *TSD, environpart *e )
/* Prepares the WITH-IO-redirection from envpart and sets *flag to either
 * STREAM or STEM. Nothing happens if there isn't a redirection.
 */
{
   const streng *h ;
   int error ;
   char code ;

   e->SameAsOutput = 0;
   e->FileRedirected = 0;
   e->tempname = NULL ; /* none as default, might become char* RedirTempFile */
   e->type = STD_IO ;
   e->hdls[0] = e->hdls[1] = -1 ;

   if (e->name == NULL)
      return ;

   switch (e->flags.awt) {
      case isSTREAM:
         /*
          * For a STREAM input/output redirection, set the file reopen
          * flag, and reopen the file.
          */
         e->type = STREAM ;
         if (e->flags.isinput)
            code = 'r' ;
         else if (e->flags.append)
            code = 'A' ;
         else /* REPLACE */
            code = 'R' ;
         e->file = addr_reopen_file( TSD, get_it_anyway( TSD, e->name ), code ) ;
         break;

      case isSTEM:
         /*
          * For a STEM input/output redirection, check that existing state of
          * the stem if appropriate and initialise the stem
          */
         e->type  = STEM ;

         if (e->flags.isinput || e->flags.append)
         {
            /*
             * For a STEM input redirection, the stem must already exist and be
             * a valid Rexx "array". This happens to an existing output stem in
             * the append mode, too.
             */
            h = stem_access( TSD, e, 0, NULL ) ;
            /* h must be a whole positive number */
            e->maxnum = streng_to_int( TSD, h, &error ) ;
            if (error || (e->maxnum < 0))
               exiterror( ERR_INVALID_STEM_OPTION, /* needs stem.0 and      */
                          1,                       /* (stem.0) as arguments */
                          tmpstr_of( TSD, e->currname ),
                          tmpstr_of( TSD, h ) )  ;
            e->currnum = (e->flags.isinput) ? 1 : e->maxnum + 1;
         }
         else /* must be REPLACE */
         {
            e->maxnum = 0 ;
            e->currnum = 1 ;
            e->base->value[0] = '0' ;
            e->base->len = 1 ;
            stem_access( TSD, e, 0, e->base ) ;
         }
         break;

      case isLIFO:
         if (Str_len(e->name) == 0)
         {
            if (e->flags.isinput)
               e->type = QUEUE;
            else
               e->type = LIFO;
            break;
         }
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "LIFO isn't implemented yet" )  ;
         break;

      case isFIFO:
         if (Str_len(e->name) == 0)
         {
            if (e->flags.isinput)
               e->type = QUEUE;
            else
               e->type = FIFO;
            break;
         }
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "FIFO isn't implemented yet" )  ;
         break;

      default:
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "Illegal address code in open_env_io()" )  ;
   }
}

void put_stem( tsd_t *TSD, environpart *e, streng *str )
/* Adds one line to the stem specified in e. */
{
   /*
    * FGC: 2 lines commented on 27.02.2002 17:08:46 FIXME
    * const streng *h;
    * int dummy;
    */

   e->maxnum = e->currnum ;
   e->currnum++ ;

   /* FGC: FIXME: Do we still need the following two lines?
    * Needed by doscmd.c?
    */
   /*
    * FGC: 2 lines commented on 27.02.2002 17:08:46 FIXME
    * h = stem_access( TSD, e, 0, NULL ) ;
    * e->maxnum = e->currnum = streng_to_int( TSD, h, &dummy ) + 1 ;
    */
   e->base->len = sprintf( e->base->value, "%d", e->maxnum ) ;
   stem_access( TSD, e, 0, e->base ) ;
   stem_access( TSD, e, e->maxnum, str ) ;
}

static int write_buffered(const tsd_t *TSD, int hdl, const void *buf,
                                               unsigned size, void *async_info)
{
   int rc, done;
   unsigned todo;
   shl_tsd_t *st = TSD->shl_tsd;

   if ((buf == NULL) || (size == 0)) /* force flush buffers */
   {
      if (st->IOBused)
         rc = __regina_write(hdl, st->IObuf, st->IOBused, async_info);
      else
         rc = 0;
      if (rc >= 0)
         rc = __regina_write(hdl, NULL, 0, async_info);
      else
         __regina_write(hdl, NULL, 0, async_info);
      return(rc);
   }

   done = 0;
   while (size) {
      todo = size;
      if (todo > sizeof(st->IObuf) - st->IOBused)
         todo = sizeof(st->IObuf) - st->IOBused;
      if (todo > 0)
      {
         memcpy(st->IObuf + st->IOBused, buf, todo);
         st->IOBused += todo;
      }

      done += todo;
      if (st->IOBused < sizeof(st->IObuf))
         return(done);

      /* buffer full */
      rc = __regina_write(hdl, st->IObuf, st->IOBused, async_info);
      if (rc <= 0)
      {
         if (done)
            break; /* something done sucessfully */
         return(rc);
      }
      if (rc == (int) st->IOBused)
         st->IOBused = 0;
      else
      {
         memmove(st->IObuf, st->IObuf + rc, st->IOBused - rc);
         st->IOBused -= rc;
      }

      buf = (const char *) buf + todo;
      size -= todo;
   }

   return(done); /* something done sucessfully */
}

static int feed( const tsd_t *TSD, streng **string, int hdl, void *async_info )
/* Writes the content of *string into the file associated with hdl. The
 * string is shortened and, after the final write, deleted and *string set
 * to NULL.
 * async_info is both a structure and a flag. If set, asynchronous IO shall
 * be used, otherwise blocked IO has to be used.
 * feed returns 0 on success or an errno value on error.
 * A return value of EAGAIN is set if we have to wait.
 */
{
   unsigned total ;
   int done ;

   if ((string == NULL) || (*string == NULL))
      return( 0 ) ;

   total = Str_len( *string ) ;
   if (total == 0)
      return( 0 ) ;

   done = write_buffered(TSD, hdl, (*string)->value, total, async_info);
   if (done <= 0)
   {
      if (done == 0)      /* no error set? */
         done = ENOSPC ;  /* good assumption */
      else
         done = -done;
      if ((done != EAGAIN) && (done != EPIPE))
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, strerror(done) )  ;
      return( done ) ; /* error */
   }

   if ((unsigned) done < total)
   {
      (*string)->len -= done ;
      memmove((*string)->value, (*string)->value + done, (*string)->len);
   }
   else
   {
      assert((unsigned)done==total);
      Free_stringTSD(*string);
      *string = NULL;
   }
   return(0);
}

static int reap( const tsd_t *TSD, streng **string, int hdl, void *async_info )
/* Reads data from the file associated with hdl and returns it in *string.
 * *string may be NULL or valid, in which case it is expanded.
 * reap returns 0 on success or an errno value on error. The value -1 indicates
 * EOF.
 * async_info is both a structure and a flag. If set, asynchronous IO shall
 * be used, otherwise blocked IO has to be used.
 * A maximum chunk of REGINA_MAX_BUFFER_LENGTH (usually 4096) bytes is read in one operation.
 * A return value of EAGAIN is set if we have to wait.
 */
{
   char buf[REGINA_MAX_BUFFER_LENGTH] ;
   unsigned len, total ;
   streng *s ;
   int done ;

   if (string == NULL)
      return( 0 ) ;

   done = __regina_read( hdl, buf, sizeof(buf), async_info ) ;
   if (done <= 0)
   {
      if (done == 0)
         return( -1 ); /* EOF */
      else
         done = -done;
      /* FGC, FIXME: Not sure, this is the right processing. Setting RS, etc? */
      if ( done != EAGAIN )
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, strerror(done) )  ;
      return( done ) ; /* error */
   }

   if (( s = *string ) == NULL)
   {
      len = 0 ;
      s = Str_makeTSD( done ) ;
   }
   else
   {
      len = Str_len( s ) ;
      total = Str_max( s ) ;
      if (len + done > total)
      {
         s = Str_makeTSD( len + done ) ;
         s->len = len ;
         memcpy( s->value, (*string)->value, len ) ;
         Free_stringTSD( *string ) ;
      }
   }
   memcpy( s->value + len, buf, done ) ;
   s->len += done ;
   *string = s ;
   return( 0 ) ;
}

void cleanup_envirpart(const tsd_t *TSD, environpart *ep)
/* Closes the associated file handles of ep and deletes a temporary file
 * if necessary.
 */
{
   shl_tsd_t *st = TSD->shl_tsd;

   if (ep->hdls[0] != -1)
   {
      __regina_close(ep->hdls[0], (ep->FileRedirected) ? NULL : st->AsyncInfo);
      ep->hdls[0] = -1;
   }
   if (ep->hdls[1] != -1)
   {
      __regina_close(ep->hdls[1], (ep->FileRedirected) ? NULL : st->AsyncInfo);
      ep->hdls[1] = -1;
   }
   if (ep->tempname)
   {
      unlink(ep->tempname);
      FreeTSD(ep->tempname);
      ep->tempname = NULL;
   }
}

static void cleanup( tsd_t *TSD, environment *env )
/* Closes all open handles in env and deletes temporary files. Already closed
 * handles are marked by a value of -1.
 * -1 is set to each handle after closing.
 */
{
   shl_tsd_t *st = TSD->shl_tsd;

   cleanup_envirpart(TSD, &env->input);
   cleanup_envirpart(TSD, &env->output);
   cleanup_envirpart(TSD, &env->error);
   purge_input_queue(TSD);

   if (st->AsyncInfo)
      delete_async_info(st->AsyncInfo);
   st->AsyncInfo = NULL;
   st->IOBused = 0;
}

static int setup_io( tsd_t *TSD, int io_flags, environment *env )
/* Sets up the IO-redirections based on the values in io_flags and env.
 * Each environpart (env->input, env->output, env->error) is set up as follows:
 * a) The enviroment-based streams and stems are set up if used or not.
 *    env->input.type (or output or error) is set to STREAM, STEM or STD_IO.
 * b) The io_flags overwrite the different settings and may have
 *    values QUEUE, simLIFO, simFIFO.
 * c) If a redirection takes place (type != STD_IO) a pipe() or temporary
 *    file is opened and used.
 * This function returns 1 on success, 0 on error, in which case an error is
 * already reported..
 */
{
   shl_tsd_t *st = TSD->shl_tsd;

   cleanup( TSD, env ); /* Useful in case of an undetected previous error */

   /*
    * Determine which ANSI redirections are in effect
    */
   open_env_io( TSD, &env->input ) ;
   open_env_io( TSD, &env->output ) ;
   open_env_io( TSD, &env->error ) ;

   if ((env->output.type == STEM) && (env->error.type == STEM))
   {
      /* env->output.name and env->error.name must exist here
       *
       * We have to take special care if output and error are
       * redirected to the same stem. We neither want to overwrite
       * stem values twice nor want to read "stem.0" for every
       * stem on every access to prevent it.
       */
      if (Str_ccmp(env->output.name, env->error.name) == 0)
      {
         env->error.SameAsOutput = 1;
         if (env->error.maxnum == 0)
         {
            /* error may has the REPLACE option while output has not.
             * Force a silent replace in this case.
             */
            env->output.maxnum = 0;
            env->output.currnum = 1 ;
         }
      }
   }

   if (env->input.type == STEM)
   {
      /* Same procedure. To prevent overwriting variables while
       * outputting to a stem wherefrom we have to read, buffer
       * the input stem of the names do overlap.
       */

      if ((env->output.type == STEM) &&
          (Str_ccmp(env->input.name, env->output.name) == 0))
         env->input.SameAsOutput = 1;

      if ((env->error.type == STEM) &&
          (Str_ccmp(env->input.name, env->error.name) == 0))
         env->input.SameAsOutput = 1;

      if (env->input.SameAsOutput)
         fill_input_queue(TSD, env->input.name, env->input.maxnum);
   }
   /*
    * Override any Regina redirections
    */
   if (io_flags & REDIR_INPUT)
      env->input.type = QUEUE ;
   if (io_flags & REDIR_OUTLIFO)
      env->output.type  = simLIFO ;
   else if (io_flags & REDIR_OUTFIFO)
      env->output.type = simFIFO ;
   else if (io_flags & REDIR_OUTSTRING)
      env->output.type = STRING ;

   if (env->input.type != STD_IO)
   {
      if (open_subprocess_connection(TSD, &env->input) != 0)
      {
         cleanup( TSD, env ) ;
         exiterror( ERR_SYSTEM_FAILURE, 920, "creating redirection", "for input", strerror(errno) )  ;
         return( 0 ) ;
      }
   }
   if (env->output.type != STD_IO)
   {
      if (open_subprocess_connection(TSD, &env->output) != 0)
      {
         cleanup( TSD, env ) ;
         exiterror( ERR_SYSTEM_FAILURE, 920, "creating redirection", "for output", strerror(errno) )  ;
         return( 0 ) ;
      }
   }
   else
      fflush(stdout);
   if (env->error.type != STD_IO)
   {
      if (open_subprocess_connection(TSD, &env->error) != 0)
      {
         cleanup( TSD, env ) ;
         exiterror( ERR_SYSTEM_FAILURE, 920, "creating redirection", "for error", strerror(errno) )  ;
         return( 0 ) ;
      }
   }
   else
      fflush(stderr);
   st->AsyncInfo = create_async_info(TSD);
   return( 1 ) ;
}

static streng *fetch_food( tsd_t *TSD, environment *env )
/* returns one streng fetched either from a queue (env->input.type == QUEUE) or
 * from a stem or stream.
 * Returns NULL if there is no more input to feed the child process.
 */
{
   const streng *c ;
   streng *retval ;
   int delflag = 0 ;

   switch (env->input.type)
   {
      case QUEUE:
         if (stack_empty( TSD ))
            return( NULL ) ;
         delflag = 1 ;
         c = popline( TSD, NULL, NULL, 0 ) ;
         break;

      case STREAM:
         if (env->input.file == NULL)
            return( NULL ) ;
         delflag = 1 ;
         c = addr_io_file( TSD, env->input.file, NULL ) ;
         if (!c)
            return( NULL ) ;
         break;

      case STEM:
         if (!env->input.SameAsOutput)
         {
            if (env->input.currnum > env->input.maxnum)
               return( NULL ) ;
            c = stem_access( TSD, &env->input, env->input.currnum++, NULL ) ;
         }
         else
         {
            delflag = 1 ;
            c = get_input_queue( TSD ) ;
         }
         if (!c)
            return( NULL ) ;
         break;

      default:
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "Illegal feeder in fetch_food()" )  ;
         return( NULL ) ;
         break ;
   }

   /* Append a newline to the end of the line before returning */
   retval = Str_makeTSD( c->len + 1 ) ;
   memcpy(retval->value, c->value, c->len);
   retval->value[c->len] = REGINA_EOL;
   retval->len = c->len + 1;
   if (delflag)
      Free_stringTSD( (streng *) c ) ;
   return( retval ) ;
}

static void drop_crop_line( tsd_t *TSD, environment *env, const char *data,
                                                unsigned length, int is_error )
/* Called while reading the output of the child. The output is in data and
 * contains length bytes without the line terminator.
 * which may be empty or not yet completed. The exact destination is determined
 * by env->x.type, where x is either output or error depending on is_error.
 * type may have one of the values simLIFO, simFIFO, STRING, STREAM or STEM.
 * is_error is set if the error redirection should happen.
 */
{
   streng *string ;
   int type;

   string = Str_makeTSD( length + 1 ) ; /* We need a terminating 0 in some */
                                        /* cases                           */
   memcpy( string->value, data, length ) ;
   string->len = length ;
   string->value[length] = '\0' ;

   if (is_error)
      type = env->error.type;
   else
      type = env->output.type;

   switch (type)
   {
      case simLIFO:
         tmp_stack(TSD, string, 0 ) ;
         return;  /* consumes the new string */

      case simFIFO:
      case STRING:
         tmp_stack(TSD, string, 1 ) ;
         return;  /* consumes the new string */

      case STREAM:
         if (is_error)
         {
            if (env->error.file)
               addr_io_file( TSD, env->error.file, string ) ;
         }
         else
         {
            if (env->output.file)
               addr_io_file( TSD, env->output.file, string ) ;
         }
         break;

      case STEM:
         if (is_error && !env->error.SameAsOutput)
            put_stem( TSD, &env->error, string ) ;
         else
            put_stem( TSD, &env->output, string ) ;
         return;  /* consumes the new string */

      default:
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "Illegal crop in drop_crop_line()" )  ;
         break ;
   }

   Free_stringTSD( string ) ;
}

static void drop_crop( tsd_t *TSD, environment *env, streng **string,
                                                  int EOFreached, int is_error)
/* Called while reading the output of the child. The output is in *string,
 * which may be empty or not yet completed. The exact destination is determined
 * by env->x.type, where x is either output or error depending on is_error.
 * type may have one of the values simLIFO, simFIFO, STRING, STREAM or STEM.
 * If EOFreached is set and some data is in *string, this data is interpreted
 * as a completed line.
 * Completed lines are cut of the string. The string itself isn't deleted.
 * is_error is set if the error redirection should happen.
 */
{
   streng *s ;
   char *ptr ;
   char *ccr, *clf ;
   int len=0, max, found, termlen=0 ;


   s = *string;
   if (s == NULL) /* might happen on a first call */
      return;

   ptr = s->value ;
   max = Str_len( s ) ;

   for (;;)
   {
      /* We have to find the line end. This is painful because we don't
       * know the used line style. Allow '\r', '\n', "\r\n" ,and "\n\r".
       */
      /* memchr calling twice is much faster than a locally defined loop */
      ccr = memchr( ptr, '\r', max ) ;
      clf = memchr( ptr, '\n', max ) ;
      found = 0 ;
      if (ccr)
      {
         if (clf == ccr + 1)
         {
            len = ccr - ptr ;
            termlen = 2 ;
            found = 1 ;
         }
         else if (ccr == clf + 1)
         {
            len = clf - ptr ;
            termlen = 2 ;
            found = 1 ;
         }
         else /* '\r' found, but we must know if it terminates */
         {
            len = ccr - ptr ;
            if ((len < max) || EOFreached)
            {
               termlen = 1 ;
               found = 1 ;
            }
         }
      }
      else if (clf) /* simple line feed */
      {
         len = clf - ptr ;
         if ((len < max) || EOFreached)
         {
            termlen = 1 ;
            found = 1 ;
         }
      }
      else if (EOFreached)
      {
         len = max ;
         termlen = 0 ;
         if (len)
            found = 1;
      }
      if (!found)
         break;

      drop_crop_line( TSD, env, ptr, (unsigned) len, is_error ) ;
      len += termlen ;
      max -= len ;

      memcpy( s->value, s->value + len, max ) ;
   }
   s->len = max ;
   *string = s ;
}

int posix_do_command( tsd_t *TSD, const streng *command, int io_flags, environment *env )
{
   int child, rc ;
   int in, out, err;
   streng *istring = NULL, *ostring = NULL, *estring = NULL ;
   char *cmdline ;
   shl_tsd_t *st = TSD->shl_tsd;

   fflush( stdout ) ;
   fflush( stderr ) ;

   if (!setup_io(TSD, io_flags, env))
      exiterror( ERR_SYSTEM_FAILURE, 0 )  ;

   if (env->input.FileRedirected)
   {
      /* fill up the input file without closing the stream. */

      while ((istring = fetch_food(TSD, env)) != NULL) {
         if (feed(TSD, &istring, env->input.hdls[1], NULL) != 0)
            break; /* shall not happen! */
      }
      /* seek positions of both fdin may have been destroyed */
      restart_file(env->input.hdls[0]);
      __regina_close(env->input.hdls[1], NULL);
      env->input.hdls[1] = -1;
   }

   cmdline = str_ofTSD( command ) ;
   child = fork_exec(TSD, env, cmdline);
   FreeTSD( cmdline ) ;
   if ((child == -1) || (child == -2))
   {
      cleanup( TSD, env ) ;
      exiterror( ERR_SYSTEM_FAILURE, 1, strerror(errno) ) ;
   }

   /* Close the child part of the handles */
   if (env->input.hdls[0]  != -1) __regina_close(env->input.hdls[0], NULL) ;
   if (env->output.hdls[1] != -1) __regina_close(env->output.hdls[1], NULL) ;
   if (env->error.hdls[1]  != -1) __regina_close(env->error.hdls[1], NULL) ;
   env->input.hdls[0] = env->output.hdls[1] = env->error.hdls[1] = -1;

   /* Force our own handles to become nonblocked */
   if (!env->input.FileRedirected)
   {
      in = env->input.hdls[1];
      unblock_handle( &in, st->AsyncInfo ) ;
   }
   else
      in = -1;
   if (!env->output.FileRedirected)
   {
      out = env->output.hdls[0];
      unblock_handle( &out, st->AsyncInfo ) ;
   }
   else
      out = -1;
   if (!env->error.FileRedirected)
   {
      err = env->error.hdls[0];
      unblock_handle( &err, st->AsyncInfo ) ;
   }
   else
      err = -1;

#ifdef SIGPIPE
   regina_signal( SIGPIPE, SIG_IGN ) ;
#endif

   while ((in != -1) || (out != -1) || (err != -1))
   {
      reset_async_info(st->AsyncInfo);
      if (in != -1)
      {
         do {
            if (!istring)
               istring = fetch_food( TSD, env ) ;
            if (!istring)
            {
               rc = write_buffered(TSD, in, NULL, 0, st->AsyncInfo);
               if (rc == -EAGAIN)
                  add_async_waiter(st->AsyncInfo, in, 0);
               else
               {
                  if (rc < 0)
                  {
                     errno = -rc;
                     exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, strerror(errno) ) ;
                  }
                  if (__regina_close(in, st->AsyncInfo))
                     exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, strerror(errno) ) ;
                  env->input.hdls[1] = in = -1 ;
                  rc = -1 ; /* indicate a closed stream */
               }
            }
            else  /* nothing left in string, but more in the stack */
            {
               rc = feed( TSD, &istring, in, st->AsyncInfo ) ;
               if (rc)
               {
                  if (rc == EAGAIN)
                     add_async_waiter(st->AsyncInfo, in, 0);
                  else
                  {
                     __regina_close(in, st->AsyncInfo) ;
                     env->input.hdls[1] = in = -1 ;
                  }
               }
               else if (istring != NULL)
               {
                  /* hasn't written all at once, therefore is blocked.
                   * do a little performance boost and don't try to write
                   * once more, perform the wait instead.
                   */
                  rc = -1;
                  add_async_waiter(st->AsyncInfo, in, 0);
               }
            }
         } while (rc == 0); /* It is best for performance and no penalty for */
                            /* security to write as much as possible         */

      } /* if (in != -1) */

      if (out != -1)
      {
         do {
            rc = reap( TSD, &ostring, out, st->AsyncInfo );
            if (rc)
            {
               if (rc == EAGAIN)
                  add_async_waiter(st->AsyncInfo, out, 1);
               else
               {
                  __regina_close(out, st->AsyncInfo) ;
                  env->output.hdls[0] = out = -1 ;
               }
            }
            else if (ostring != NULL)
               drop_crop( TSD, env, &ostring, 0, 0 ) ;
         } while (rc == 0); /* It is best for performance and no penalty for */
                            /* security to write as much as possible         */
      } /* if (out != -1) */

      if (err != -1)
      {
         do {
            rc = reap( TSD, &estring, err, st->AsyncInfo );
            if (rc)
            {
               if (rc == EAGAIN)
                  add_async_waiter(st->AsyncInfo, err, 1);
               else
               {
                  __regina_close(err, st->AsyncInfo) ;
                  env->error.hdls[0] = err = -1 ;
               }
            }
            else if (estring != NULL)
               drop_crop( TSD, env, &estring, 0, 1 ) ;
         } while (rc == 0); /* It is best for performance and no penalty for */
                            /* security to write as much as possible         */
      } /* if (err != -1) */

      wait_async_info(st->AsyncInfo); /* wait for any more IO */
   } /* end of IO */

   if (istring)
      Free_stringTSD( istring );

   if (ostring && Str_len(ostring))
   {
      drop_crop( TSD, env, &ostring, 1, 0 ) ;
      Free_stringTSD( ostring );
   }

   if (estring && Str_len(estring))
   {
      drop_crop( TSD, env, &estring, 1, 1 ) ;
      Free_stringTSD( estring );
   }

   if (env->input.type == QUEUE)
   {
      while (!stack_empty( TSD ))
      {
         Free_stringTSD( popline( TSD, NULL, NULL, 0 ) ) ;
      }
   }

   rc = __regina_wait(child);

#ifdef SIGPIPE
   regina_signal( SIGPIPE, SIG_DFL ) ;
#endif

   if (env->output.FileRedirected)
   {
      /* The file position is usually at the end: */
      restart_file(env->output.hdls[0]);
      while (reap( TSD, &ostring, env->output.hdls[0], NULL ) == 0) {
         if (ostring != NULL)
            drop_crop( TSD, env, &ostring, 0, 0 ) ;
      }
      if (ostring != NULL)
         drop_crop( TSD, env, &ostring, 1, 0 ) ;

      /* use the automatted closing feature of cleanup */
   }
   if (env->error.FileRedirected)
   {
      /* The file position is usually at the end: */
      restart_file(env->error.hdls[0]);
      while (reap( TSD, &estring, env->error.hdls[0], NULL ) == 0) {
         if (estring != NULL)
            drop_crop( TSD, env, &estring, 0, 1 ) ;
      }
      if (estring != NULL)
         drop_crop( TSD, env, &estring, 1, 1 ) ;
      /* use the automatted closing feature of cleanup */
   }

   if ((env->output.type == simLIFO) || (env->output.type == simFIFO))
      flush_stack( TSD, env->output.type == simFIFO ) ;

   cleanup( TSD, env ) ;

   return rc ;
}
