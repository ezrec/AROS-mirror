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
#include <limits.h>
#include <stdio.h>
#if defined(VMS) || defined(MAC) || defined(__AROS__)
# include <time.h>
#else
# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif
# if (defined(__WATCOMC__) && !defined(__QNX__)) || defined(_MSC_VER) || defined(__MINGW32__) || defined(__BORLANDC__)         /* MH 10-06-96 */
#  include <process.h>                                  /* MH 10-06-96 */
# else                                                  /* MH 10-06-96 */
#  include <sys/wait.h>                                 /* MH 10-06-96 */
#  include <sys/time.h>                                 /* MH 10-06-96 */
# endif                                                 /* MH 10-06-96 */
# include <fcntl.h>
#endif

#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef HAVE_ASSERT_H
# include <assert.h>
#endif
#include <signal.h>

#if defined(VMS)
# define fork() vfork()
# ifdef  posix_do_command
#  undef posix_do_command
# endif
# define posix_do_command __regina_vms_do_command
#endif

#if defined(DOS) || defined(MAC) || (defined(__WATCOMC__) && !defined(__QNX__)) || defined(_MSC_VER) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(_AMIGA) || defined(__AROS__) /* MH 10-06-96 */
# ifdef  posix_do_command
#  undef posix_do_command
# endif
# define posix_do_command __regina_dos_do_command
#endif

#if defined(_POSIX_PIPE_BUF) && !defined(PIPE_BUF)
# define PIPE_BUF _POSIX_PIPE_BUF
#endif

typedef struct { /* shl_tsd: static variables of this module (thread-safe) */
   char *cbuff ;
   int   child ;
   int   status ;
   int   running ;
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


static int getenvir( const streng *envir )
{
   if (envir->len==4 && !strncmp( envir->value, "PATH", 4 ))
      return SUBENVIR_PATH ;
   else if (envir->len==3 && !strncmp( envir->value, "CMD", 3 ))
      return SUBENVIR_COMMAND ;
   else if (envir->len==7 && !strncmp( envir->value, "COMMAND", 7 ))
      return SUBENVIR_COMMAND ;
   else
      return SUBENVIR_SYSTEM ;
}

static char **makeargs( const tsd_t *TSD, const char *string )
{
   int i, words = 0, j, k ;
   char **ccptr ;
   size_t len = strlen(string);

   if ( len == 0 )
      return NULL;
   for (i=0; i<len; i++)
   {
      words += ((isgraph(string[i])) &&
                (i>=(len-1) || !isgraph(string[i+1]))) ;
   }

   ccptr = MallocTSD((words+2)*sizeof(char*)) ;
   for (j=1,i=0; j<=words; j++)
   {
      for(k=i; i<len && isgraph(string[i]); i++) ;
      strncpy(ccptr[j]=MallocTSD(i-k+1),&string[k],i-k) ;
      (ccptr[j])[i-k] = 0x00 ;
      for (; i<len &&(!isgraph(string[i])); i++) ;
   }
   ccptr[j] = NULL ;

   ccptr[0] = ccptr[1] ;
   for (i=strlen(ccptr[0])-1; (i>=0)&&((ccptr[0])[i]!='/'); i--) ;
   strcpy( ccptr[1]=MallocTSD(strlen(&((ccptr[0])[i+1]))+1), &(ccptr[0])[i+1]) ;

   return ccptr ;
}

static void destroyargs( const tsd_t *TSD, char **args )
{
   char **ccptr=NULL ;

   ccptr = args ;
   for (; *ccptr; ccptr++)
      FreeTSD( *ccptr ) ;

   FreeTSD( args ) ;
}

#if defined(__QNX__) || (!defined(MAC) && !defined(VMS) && !defined(DOS) && !defined(WIN32) && !defined(__WATCOMC__) && !defined(_MSC_VER) && !defined(_AMIGA) && !defined(__AROS__))
int posix_do_command( tsd_t *TSD, const streng *command, int io_flags, int envir )
{
   char **args ;
   int child, rc, i, fdin[2], fdout[2] ;
   char *cmd, *cmdline ;
   int in, out, fout ;
   streng *string = NULL;
   streng *istring = NULL;
   int pipe_buf = 0;
   int length, rcode, j, flush, status ;
   streng *result ;
   char flags=' ' ;
   int count ;
   shl_tsd_t *st;

   st = TSD->shl_tsd;

   in = io_flags & REDIR_INPUT ;
   out = io_flags & REDIR_OUTLIFO ;
   fout = io_flags & REDIR_OUTFIFO ;

   cmdline = str_ofTSD( command ) ;
   flush = (out!=0) + ((fout!=0)*2) ;

   if ((!in)&&(!out)&&(!fout))
   {
      ;       /* maybe this should not be allowed. ... */
   }

   cmd = NULL ;
   if (envir != SUBENVIR_SYSTEM
   && strlen(cmdline))
   {
      args = makeargs( TSD, cmdline ) ;
      cmd = *args ;
   }
   else
      args = NULL ;

   fflush( stdout ) ;
   fflush( stderr ) ;

   if ((in)&&(pipe( fdin )))     /* for info from stack to child */
      perror("While opening input pipe") ;

   if (((out)||(fout))&&(pipe( fdout )))   /* for info from child to stack */
      perror("While opening output pipe") ;

   if ((child=fork()))
   {
      regina_signal( SIGPIPE, SIG_IGN ) ;

      if (out || fout)
      {
#if defined(PIPE_BUF)
         pipe_buf = PIPE_BUF ;
#else
# ifndef _PC_PIPE_BUF
         /* Some vendors just don't get it right ... sigh! The number */
         /* 512 is my 'invention' ... it might not be what you want */
         pipe_buf = 512 ;
# else
         pipe_buf = fpathconf( fdout[0], _PC_PIPE_BUF ) ;
         assert( pipe_buf > 0 ) ;
# endif
#endif
      }

      if (st->cbuff==NULL && (out||fout))
         st->cbuff = MallocTSD( pipe_buf+1 ) ;

      if (child<0)
         exiterror( ERR_SYSTEM_FAILURE, 0 )  ;

      if (in)
         close( fdin[0]) ;

      if ((out)||(fout))
         close( fdout[1] ) ;

      if ((in)&&((out)||(fout)))
      {
         fcntl( fdin[1], F_SETFL, O_NONBLOCK ) ;
         fcntl( fdout[0], F_SETFL, O_NONBLOCK ) ;
      }

      for (;(in)||(out)||(fout);)
      {
         if (in)
         {
            if (((!string)&&(stack_empty( TSD ))))
            {
               if (close(fdin[1]))
                  perror("During close") ;
               in = 0 ;
               if ((out)||(fout))
               {
#if !defined(VMS) && !defined(DOS) && !defined(__WATCOMC__) && !defined(_MSC_VER) && !defined(_AMIGA) && !defined(__BORLANDC__)
/*
 * Kludge for OS/2 ... see below
 */
                  flags = fcntl( fdout[0], F_GETFL, NULL ) ;
                  fcntl( fdout[0], F_SETFL, flags & (~O_NONBLOCK)) ;
#endif
               }

            }
            else  /* nothing left in string, but more in the stack */
            {
               if (!string)
               {
                  streng *h;
                  /* pop a line and append a newline. no term. zero needed */
                  string = popline( TSD, NULL, NULL, 0 ) ;
                  h = Str_make( string->len + 1 );
                  memcpy(h->value,string->value,string->len);
                  h->value[string->len] = '\n';
                  h->len = string->len + 1;
                  Free_stringTSD( string ) ;
                  string = h ;
               }

               rcode = write( fdin[1], string->value, string->len ) ;

#ifdef EWOULDBLOCK
               /* POSIX says EAGAIN, but BSD trad. uses EWOULDBLOCK, */
               /* so if EWOULDBLOCK is defined, check for that, too  */
               if ((rcode== -1)&&((errno==EAGAIN)||(errno==EWOULDBLOCK)))
#else
               if ((rcode== -1)&&(errno==EAGAIN))
#endif
                  continue ;

               if ((rcode== -1)&&(errno==EPIPE))
               {
                  Free_stringTSD( string ) ;
                  for (; (!stack_empty( TSD )); )
                     Free_stringTSD( popline( TSD, NULL, NULL, 0 ) ) ;
                  string = NULL ;
                  continue ;
               }

               if (rcode < 0)
               {
                  perror("While writing") ;
                  continue ;
               }

               if (rcode<string->len)
               {
                  string->len -= rcode ;
                  memmove(string->value,string->value + rcode,string->len) ;
               }
               else
               {
                  assert( rcode==string->len ) ;
                  Free_stringTSD( string ) ;
                  string = NULL ;
               }
            }
         }

         if ((out)||(fout))
         {
            rcode = read( fdout[0], st->cbuff, pipe_buf ) ;
            if (rcode>0)
               st->cbuff[rcode] = 0x00 ;

            if ((rcode==(-1))&&(errno==EAGAIN))
               continue ;

            if ((rcode==(0)))
            {
               close( fdout[0] ) ;
               if (in)
               {
#if !defined(VMS) && !defined(DOS) && !defined(__WATCOMC__) && !defined(_MSC_VER) && !defined(_AMIGA) && !defined(__BORLANDC__)
/*
 * OS/2 haven't defined fcntl() correctly, so we try to make it
 * work, by adding an extra parameter ... sigh!
 */
                  flags = fcntl( fdin[1], F_GETFL, NULL ) ;
                  fcntl( fdin[1], F_SETFL, flags & (~O_NONBLOCK)) ;
#endif
               }

               out = fout = 0 ;
               break ;
            }

            for (i=j=0; (i<rcode)&&(st->cbuff[i]); i++)
            {
               if ( st->cbuff[i] == REGINA_CR )
                  st->cbuff[i] = ' ' ;

               if ( st->cbuff[i] != '\n' )
                  continue ;

               if (istring)
               {
                  result = Str_makeTSD( (length=Str_len(istring)) + i-j + 1 ) ;
                  result = Str_catTSD( result, istring ) ;
                  for (count=0; count<(i-j); count++)
                     result->value[count+length] = st->cbuff[j+count] ;
/*                result->value[length+count] = 0x00 ;
 *                FreeTSD( istring ) ;
 *
 * Previous two lines changed to next to lines, after bugfix from
 * G. Fuchs <fuchs@vax1.rz.uni-regensburg.dbp.de>, 7th July 92
 */
                  result->value[result->len=length+count] = 0x00 ;
                  Free_stringTSD( istring ) ; istring=NULL ;
               }
               else
               {
                  result = Str_makeTSD( (i-j) + 1 ) ;
                  for (count=0; count<(i-j); count++ )
                     result->value[count] = st->cbuff[j+count] ;
                  result->value[result->len=(i-j)] = 0x00 ;
               }

               tmp_stack(TSD, result, flush==2) ;
               j = i + 1 ;
            }
            assert((j<=i)&&(j>=0)&&(i>=0)) ;
            if ((j<i)&&(!istring)) /* some leftover chars ... */
            {
               istring = Str_makeTSD( i-j+1 ) ;
               for (count=0; count<(i-j); count++ )
                  istring->value[count] = st->cbuff[j+count] ;
               istring->value[ istring->len=(i-j) ] = 0x00 ;
            }
            else if ((j!=i)&&(istring)) /* leftover chars but no lf */
            {
               result = Str_makeTSD( (length=Str_len(istring)) + (i-j) + 1 ) ;
               result = Str_catTSD( result, istring ) ;
               result = Str_catstrTSD( result, st->cbuff ) ;
               Free_stringTSD( istring );  /* bja */
               istring = result ;
            }
            else if ((i==j)) /* read ended with a lf */
            {
               assert( !istring ) ;
            }

         }
      }

      if (flush)
         flush_stack( TSD, (flush==2) ) ;

      if (in) {
         close( fdin[0] ) ;
         close( fdin[1] ) ;
      }
      if ((out)||(fout)) {
         close( fdout[0] ) ;
         close( fdout[1] ) ;
      }
      if ( istring )              /* bja */
         Free_stringTSD( istring );  /* bja */

#ifdef VMS
      wait( &status ) ;
      rc = status & 0xff ;
#else
#ifdef NEXT
      /*
       * According to Paul F. Kunz, NeXTSTEP 3.1 Prerelease doesn't have
       * the waitpid() function, so wait() must be used instead. The
       * following klugde will remain until NeXTSTEP gets waitpid().
       */
      wait( &status ) ;
#else
# ifdef DOS
      wait( &status ) ;
# else
      waitpid( child, &status, 0 ) ;
# endif
#endif
      if (WIFEXITED(status))
         rc = ( int ) WEXITSTATUS(status) ;
      else if (WIFSIGNALED(status))
         rc = -100 - WTERMSIG(status) ;
      else
         rc = -1 ;
#endif
      regina_signal( SIGPIPE, SIG_DFL ) ;
   }
   else /* child part of the fork */
   {
     if (in)
     {
        dup2( fdin[0], 0 ) ;
        if (fdin[0] != 0)
         close( fdin[0] ) ;
        if (fdin[1] != 0)
        close( fdin[1] ) ;
     }

     if ((out)||(fout))
     {
        dup2( fdout[1], 1 ) ;
        if (fdin[0] != 1)
        close( fdout[0] ) ;
        if (fdin[1] != 1)
        close( fdout[1] ) ;
     }

     for (i=3; i<32; i++)
       close( i ) ;

     if (envir==SUBENVIR_PATH)
        execvp( cmd, ++args ) ;
     else if (envir==SUBENVIR_COMMAND)
        execv( cmd, ++args ) ;
     else if (envir==SUBENVIR_SYSTEM)
     {
#if defined(HAVE_WIN32GUI)
        int foo = mysystem( cmdline ) ;
#else
        int foo = system( cmdline ) ;
#endif
#ifdef VMS
        exit (foo) ; /* This is a separate process, exit() is allowed */
#else
        if (WIFEXITED(foo))
        {
           exit( ( int )WEXITSTATUS(foo) ) ; /* This is a separate process, exit() is allowed */
        }
        else if (WIFSIGNALED(foo))
        {
           exit( -100 - WTERMSIG(foo) ); /* This is a separate process, exit() is allowed */
        }
        else
        {
           exit( 0 ) ; /* This is a separate process, exit() is allowed */
        }
#endif

     }
     else
        exit( -1 ) ; /* This is a separate process, exit() is allowed */

     exit( -2 ) ; /* This is a separate process, exit() is allowed */
  }

  if (args)
     destroyargs( TSD, args ) ;

  FreeTSD( cmdline );
  return rc ;
}
#endif

#if defined(WIN32)                                      /* MH 10-06-96 */
streng *run_popen( const tsd_t *TSD, const streng *command, const streng *envir )
{
   char *cbuffer, *sum = NULL, *h ;
   char comspec[1024] ;
   int pipe_buf = 512;
   long rc,i,chars_read;
   size_t length = 0;
   int envirno ;
   char *cmdline;
   HANDLE read_handle, write_handle ;
   STARTUPINFO sinfo;
   PROCESS_INFORMATION pinfo;
   SECURITY_ATTRIBUTES sattr;
   streng *result;

   fflush(stdout) ;
   fflush(stderr) ;

   envirno = getenvir( envir ) ;

   sattr.nLength = sizeof(sattr);
   sattr.lpSecurityDescriptor = NULL;
   sattr.bInheritHandle = TRUE;

   if (!CreatePipe(&read_handle, &write_handle, &sattr, (DWORD)0))
   {
      return NULL ;
   }

   if (envirno==SUBENVIR_SYSTEM)
   {
      char *p;

      if (mygetenv( TSD, "COMSPEC", comspec, sizeof(comspec)) == NULL)
         strcpy(comspec, "CMD.EXE");
      cmdline = MallocTSD(command->len+strlen(comspec)+20);
      sprintf(cmdline, "%s /c ", comspec) ;
      p = cmdline + strlen(cmdline);
      memcpy(p,command->value,command->len);
      p[command->len] = '\0';
   }
   else
      cmdline = str_ofTSD(command);

   memset(&sinfo, '\0', sizeof(sinfo));

   sinfo.cb = sizeof(sinfo);
   sinfo.hStdInput = GetStdHandle( STD_INPUT_HANDLE ) ;
   sinfo.hStdOutput = write_handle ;
   sinfo.hStdError = GetStdHandle( STD_ERROR_HANDLE ) ;
#if defined(HAVE_WIN32GUI)
   sinfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
   sinfo.wShowWindow=SW_HIDE;
#else
   sinfo.dwFlags = STARTF_USESTDHANDLES ;
#endif

   if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, &pinfo))
   {
      FreeTSD( cmdline ) ;
      return NULL ;
   }

   FreeTSD( cmdline ) ;
   CloseHandle(write_handle);
   cbuffer = MallocTSD(pipe_buf+1) ;

   for (;;)
   {
      if (!ReadFile( read_handle, cbuffer, pipe_buf, (DWORD*)&chars_read, NULL ) )
      {
         if (GetLastError() != ERROR_BROKEN_PIPE)
         {
            FreeTSD( cbuffer ) ;
            if (sum)
               FreeTSD( sum ) ;
            return NULL;
         }
         break;
      }

      for (i=0; (i<chars_read)&&(cbuffer[i]); i++)
        if ((cbuffer[i]=='\n')||(cbuffer[i]=='\r'))
           cbuffer[i] = ' ' ;

      h = MallocTSD( length + chars_read + 1 /* Allow a zero length string */ ) ;
      if (sum)
      {
         memcpy( h, sum, length ) ;
         FreeTSD( sum ) ;
      }
      sum = h;
      memcpy( sum+length, cbuffer, chars_read ) ;

      length += chars_read;

      if (chars_read==0)
         break ;
   }
   FreeTSD( cbuffer ) ;

   CloseHandle(read_handle);

   while(length && sum[length-1] == ' ')
      length--;

   WaitForSingleObject( pinfo.hProcess, INFINITE ) ;

   if (!GetExitCodeProcess(pinfo.hProcess,(DWORD*)&rc ))
      rc = -1;

   CloseHandle(read_handle);

   /* should this be set? */
   setvalue( TSD, &RC_name, int_to_streng( TSD,rc) ) ;

   if (rc < 0)
   {
      if (sum)
         FreeTSD( sum ) ;
      return( NULL );
   }

   /* Note: until Feb. 17th 2000 the result was 0-terminated. I think it was useless.
    * FGC
    */
   result = Str_makeTSD( length ) ;
   if ( sum )
   {
      memcpy( result->value, sum, length ) ;
      result->len = length ;
      FreeTSD( sum ) ;
   }
   else
      result->len = 0 ;

   return( result ) ;
}
#elif defined(DOS) || defined(OS2) || defined(_AMIGA) || defined(MAC) || defined(__AROS__)
/* FIXME, FGC:
 * 1) OS/2 can handle real pipes! OS/2 should get an own section.
 * 2) At least DOS has an execv-statement. We may try to load the first
 *    parameter. Lets have a look at "make" for an alternative.
 */
streng *run_popen( const tsd_t *TSD, const streng *command, const streng *envir )
{
   FILE *fptr;
   streng *result=NULL ; /* Keep the compiler happy */
   int i ;
   int rcode ;
   int c ;
   char *cmdline ;
   char outfile[REXX_PATH_MAX];

   envir = envir; /* keep compiler happy */
   /*
    * Create a temporary file to accept the stdout from the
    * command to be run
    */
   if ( !create_tmpname( TSD, outfile ) )
   {
      perror("While getting temporary out-file name") ;
      return (NULL);
   }
   cmdline = MallocTSD(command->len+strlen(outfile)+20);
   memcpy(cmdline,command->value,command->len);
   cmdline[command->len] = '\0';
   strcat(cmdline," > ");
   strcat(cmdline, str_trans(outfile,'/','\\'));

   /*
    * Run the command, with all stdout going to the temporary file
    */
#if defined(HAVE_WIN32GUI)
   rcode = mysystem(cmdline);
#else
   rcode = system(cmdline);
#endif
   FreeTSD(cmdline);
   /*
    * Read the temporary file, and set RC to the content
    * of the file.
    */
   if (rcode != -1)
   {
      fptr = fopen(outfile, "r");
      if (fptr != NULL)
      {
         result = Str_makeTSD( 500 );
         for (i=0; i < 500; i++)
         {
            c = fgetc(fptr);
            if (c == EOF)
               break;
            if ((c=='\n')||(c=='\r'))
               result->value[i] = ' ' ;
            else
               result->value[i] = c;
         }
         result->value[i] = 0x00;
         result->len = i;
         fclose(fptr);
      }
   }
   /*
    * Delete the temporary file and free up resources for it
    */
   unlink(outfile);

   setvalue( TSD, &RC_name, int_to_streng( TSD,rcode) ) ;

   return((rcode==-1)?NULL:result);
}
#else

/* run_as_child is called after a fork in run_popen. The function has no
 * input. ouput must be done into the outhandle channel. The functions
 * executes command in the "environment" envirno.
 */
static void run_as_child(const tsd_t *TSD,int outhandle,const streng *command,int envirno)
{
   int i;
   char *cmdline = str_of(TSD,command);

   /* Make outhandle the handle of stdout. stdin is closed already. */
   dup2(outhandle, 1) ;
   if (outhandle != 1)
      close(outhandle);

   /* stderr is still bound to the original value! */

   for (i = 3;i < 1024;i++)
      close(i);

   if ((envirno==SUBENVIR_PATH)||(envirno==SUBENVIR_COMMAND))
   {
      char *cmd, **args;

      args = makeargs( TSD, cmdline ) ;
      cmd = *args ;
      args++ ; /* FIXME, FGC: Why ??? argv[0] should be cmd! */

      if (envirno==SUBENVIR_PATH)
         execvp( cmd, args ) ;
      else /* must be SUBENVIR_COMMAND */
         execv( cmd, args ) ;
   }
   else if (envirno==SUBENVIR_SYSTEM)
   {
#if defined(VMS)
      int foo = system( cmdline ) ;
      exit ( foo ) ; /* This is a separate process, exit() is allowed */
#else
#if defined(HAVE_WIN32GUI)
      int foo = mysystem( cmdline ) ;
#else
      int foo = system( cmdline ) ;
#endif
      if (WIFEXITED(foo))
      {
         if (WEXITSTATUS(foo)==1)
            exit( -2 ) ; /* sh returns 2 if cmd could not be executed */
         exit( ( int )WEXITSTATUS(foo) ) ; /* This is a separate process, exit() is allowed */
      }
      else if (WIFSIGNALED(foo))
         exit( -100 - WTERMSIG(foo) ); /* This is a separate process, exit() is allowed */
      else
         exit( 0 ) ; /* This is a separate process, exit() is allowed */
#endif
   }
   else /* unknown envirno */
      exit( -1 ) ; /* This is a separate process, exit() is allowed */

   /* error in calling system or execv */
   exit( -2 ) ; /* We (the child) MUST die! */
}

streng *run_popen( const tsd_t *TSD, const streng *command, const streng *envir )
{
   char *cbuffer ;
   int pipe_buf ;
   streng *result = NULL ; /* This is the "sum" of the output of the command */
   int rc, rsize, rcode, i ;
   int length ;
   int fd[2] ;
   shl_tsd_t *st;

   st = TSD->shl_tsd;

   fflush(stdout) ;
   fflush(stderr) ;

   /*
    * first, kill any old st->children, the execution of them must have
    * been interrupted, leaving them un-waited.
    */
   if (st->running)
   {
      kill( st->child, SIGKILL ) ;
#if defined(VMS) || defined(NEXT) || defined (DOS)
      wait( &st->status ) ;
#else
      waitpid( st->child, &st->status, WNOHANG ) ;
#endif
   }

   /*
    * Create ourselves a pipe on which we communicate with the
    * st->child process
    */
   pipe(fd) ;
   /*
    * Create the st->child process...
    * No error checking if st->child process cannot be started!!
    */
   if ( ( st->child = fork() ) == 0 )
   {
      /*
       * This is the new child of the process.
       *
       * Close the read end of the pipe; the st->child only
       * writes to the pipe.
       */
      close(fd[0]) ;
      run_as_child(TSD,fd[1],command,getenvir( envir ));
      /* run_as_child will always die! */
   }

   /*
    * We are still the parent process
    *
    * Close the write end of the pipe; the parent only
    * reads from the pipe.
    */
   close(fd[1]) ; /* Close the write pipe */
   if (st->child == -1) /* An error occurs while forking */
      rc = -1;
   else
   {
      /* The child has started the execution */
      rsize = 0 ;
#if defined(PIPE_BUF)
      pipe_buf = PIPE_BUF ;
#else
# if !defined(_PC_PIPE_BUF) || defined(WIN32)           /* MH 10-06-96 */
      /* See comment on PIPE_BUF above */
      pipe_buf = 512 ;
# else
      pipe_buf = fpathconf( fd[0], _PC_PIPE_BUF ) ;
      if (pipe_buf < 1)
         pipe_buf = 512 ;
# endif
#endif
      cbuffer = MallocTSD(pipe_buf) ;

      for (;;)
      {
         rcode = read( fd[0], cbuffer, pipe_buf ) ;

         for (i=0; i<rcode; i++) /* Change CR and LF to blanks */
            if ((cbuffer[i]=='\n')||(cbuffer[i]=='\r'))
               cbuffer[i] = ' ' ;

         if ((rcode>=0) || ((rcode==(-1)) && (errno==EINTR))) /* MH 10-06-96 */
         {
            if (rcode>=0)
            {
               streng *nresult ;

               length = (result) ? Str_len(result) : 0; /* current length */
               nresult = Str_makeTSD(length+rcode+1) ;
               if (result)
                  memcpy(nresult->value,result->value,length);
               memcpy(nresult->value+length,cbuffer,rcode);
               /* FIXME, FGC: Why do we terminate the string? */
               nresult->value[length+rcode] = '\0';
               nresult->len = length+rcode;
               if (result)
                  Free_stringTSD(result);
               result = nresult;
            }

            if (rcode==0)
               break ;
         }
         else if (rcode<=0)
         {
            if (rcode<0)
               perror( "While reading from pipe" ) ;
            break ;
         }
      }
      FreeTSD( cbuffer ) ;

      if (result)
      {
         /* Strip blanks at the end of the string AND TERMINATE THE STRING! */
         while (Str_len(result))
            if (result->value[Str_len(result) - 1] == ' ')
               Str_len(result)--;
            else
               break;
         result->value[Str_len(result)] = '\0';
      }
      /*
       * Wait for the st->child process to finish...
       */
#if defined(VMS)
      wait( &st->status ) ;
      rc = st->status & 0xff ;
#else
#ifdef NEXT
      wait( &st->status ) ;
#else
# ifdef DOS
      wait( &st->status ) ;
# else
      waitpid( st->child, &st->status, 0 ) ;
# endif
#endif

      if (WIFEXITED(st->status))
         rc = ( int ) WEXITSTATUS(st->status) ;
      else if (WIFSIGNALED(st->status))
         rc = -100 - WTERMSIG(st->status) ;
      else
         rc = -1 ;
#endif
   }
   close(fd[0]);

   setvalue( TSD, &RC_name, int_to_streng( TSD,rc) ) ;
   return(((rc>=0))?result:NULL) ;

}
#endif                                                  /* MH 10-06-96 */

