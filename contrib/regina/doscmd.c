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

#if defined(MAC) || defined(DOS) || defined(WIN32) || defined(OS2) || defined(_AMIGA) || defined(__AROS__) /* MH 10-06-96 */

#include "rexx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(DOS)                                        /* MH 10-06-96 */
# include <dos.h>
#endif                                                  /* MH 10-06-96 */

#include <errno.h>

#if defined(HAVE_ASSERT_H)
# include <assert.h>
#endif

#if defined(HAVE_UNISTD_H)
# include <unistd.h>
#endif

#if defined(WIN32)
# if defined(__BORLANDC__)
#  include <time.h>
#  include <process.h>
# endif
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

#if defined(MAC) || defined(__WATCOMC__) || defined(_MSC_VER) || defined(__SASC) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__AROS__)
# include "utsname.h"                                   /* MH 10-06-96 */
# define MAXPATHLEN  _MAX_PATH                          /* MH 10-06-96 */
#else                                                   /* MH 10-06-96 */
# if defined(WIN32) && defined(__IBMC__)                /* LM 26-02-99 */
#  include "utsname.h"
#  define MAXPATHLEN (8192)
#  include <io.h>
# else
#  include <sys/param.h>                                 /* MH 10-06-96 */
#  include <sys/utsname.h>                               /* MH 10-06-96 */
# endif
#endif

#if defined(MAC) || defined (__EMX__) || defined(__WATCOMC__) || defined(_MSC_VER) || defined(DJGPP) || defined(__CYGWIN32__) || defined(__BORLANDC__) || defined(__MINGW32__) || defined(__AROS__)
# define HAVE_BROKEN_TMPNAM
# define PATH_DELIMS ":\\/"
# if defined(__EMX__) || defined(__CYGWIN32__) || defined(__AROS__)
#  define ISTR_SLASH "/"  /* This is not a must, \\ works, too */
#  define I_SLASH '/'  /* This is not a must, \\ works, too */
# elif defined(MAC)
#  define ISTR_SLASH ":"
#  define I_SLASH ':'
# else
#  define ISTR_SLASH "\\" /* This is not a must, / works at least for MSC, too */
#  define I_SLASH '\\'    /* This is not a must, / works at least for MSC, too */
                          /* FGC: system depending: DOS, OS/2, Win32 work! */
# endif
#if !defined(MAC) && !defined(__AROS__)
#  ifndef HAVE_UNISTD_H
#   include <io.h> /* access() */
#  endif
#  include <process.h> /* FGC, 5.1.00 for getpid at least under MSC/W32 */
# endif
# include <time.h> /* FGC, 5.1.00 for clock at least under MSC/W32 */
#endif

#if !defined(WIN32)
# define mysystem( tsd, cmd) system( cmd )
#endif

#define BUF_SIZE 512

#define RC_OUT_OF_MEMORY (-1)
#define RC_EOF           (-2)


#if defined(WIN32)
/*********************************************************/
static int mysystem( const tsd_t *TSD, const char *command )
/*********************************************************/
{
   PROCESS_INFORMATION pinfo;
   STARTUPINFO         sinfo;
   DWORD               exitCode;
   DWORD               version;

   ZeroMemory(&sinfo, sizeof(sinfo));
   sinfo.cb = sizeof(sinfo);
#if !defined(HAVE_WIN32GUI)
   version = GetVersion();
   if (version < 0x80000000)
   {  /* WinNT system */
/*      printf("mysystem: WinNT\n"); */
      return system(command);
   }

   /* Win95, Win98 system */
/*   printf("mysystem: Win9x\n");*/

   /* test for redirections or pipes */
   if (command == NULL || strpbrk(command, "<>|"))
      return system(command);
/*   printf("no redirection\n");*/
#else
   sinfo.dwFlags=STARTF_USESHOWWINDOW;
   sinfo.wShowWindow=SW_HIDE;
#endif
   /* first try: assume it is a EXE-program like pkzip.exe */
   if (!CreateProcess(NULL,    /* pointer to name of executable module    */
             (char *) command, /* pointer to command line string          */
                      NULL,    /* pointer to process security attributes  */
                      NULL,    /* pointer to thread security attributes   */
                      TRUE,    /* handle inheritance flag                 */
                      0,       /* creation flags                          */
                      NULL,    /* pointer to new environment block        */
                      NULL,    /* pointer to current directory name       */
                      &sinfo,  /* pointer to STARTUPINFO                  */
                      &pinfo)) /* pointer to PROCESS_INFORMATION          */
   {  /* second try: assume a command like dir, copy, BAT-program which needs command.com */
      char *p, szTemp[4096];
      p = mygetenv(TSD, "COMSPEC", szTemp, 4096);
      if (!p)
         strcpy(szTemp, "COMMAND.COM");
      strcat(szTemp, " /C ");
      strcat(szTemp, command);
      assert(strlen(szTemp) < 4096);
/*      printf("mysystem: 2nd try: <%s>\n", szTemp);*/

      if (!CreateProcess(NULL,    /* pointer to name of executable module    */
                         szTemp,  /* pointer to command line string          */
                         NULL,    /* pointer to process security attributes  */
                         NULL,    /* pointer to thread security attributes   */
                         TRUE,    /* handle inheritance flag                 */
                         0,       /* creation flags                          */
                         NULL,    /* pointer to new environment block        */
                         NULL,    /* pointer to current directory name       */
                         &sinfo,  /* pointer to STARTUPINFO                  */
                         &pinfo)) /* pointer to PROCESS_INFORMATION          */
      {
/*       printf("mysystem: 2nd try: cannot launch child process\n"); */
      return -1;
      }
   }

   /* wait until child process exits */
   WaitForSingleObject(pinfo.hProcess, INFINITE);
   GetExitCodeProcess(pinfo.hProcess, &exitCode);
   CloseHandle(pinfo.hProcess);
   CloseHandle(pinfo.hThread);
/*   printf("mysystem: exitCode %i\n", exitCode);*/
   return exitCode;
}
/*********************************************************/
int my_win32_setenv( const char *name, const char *value )
/*********************************************************/
{
   return (SetEnvironmentVariable( name, value ) );
}
#endif

/***********************************************************************/
static char *get_a_line(FILE *fp,int *length,int *rcode)
/***********************************************************************/
{
   int ch=' ';
   int bufs = 1;
   register int i=0;
   char *string;
/*---------------------------------------------------------------------*/
/* Allocate the first block of memory.                                 */
/*---------------------------------------------------------------------*/
   if ((string = (char *)malloc(BUF_SIZE+1)) == NULL) /* realloc allowed! */
   {
      *rcode = RC_OUT_OF_MEMORY;
      return(NULL);
   }

   for(;;)
   {
/*---------------------------------------------------------------------*/
/* Read a character form the stream...                                 */
/*---------------------------------------------------------------------*/
      if ((ch = fgetc(fp)) == EOF)
      {
/*---------------------------------------------------------------------*/
/* If EOF is reached, check that it really is end of file.             */
/*---------------------------------------------------------------------*/
         if (feof(fp))
         {
            *length = i;
            *rcode = RC_EOF;
            return(string);
         }
      }
/*---------------------------------------------------------------------*/
/* If end of line is reached, nul terminate string and return.         */
/*---------------------------------------------------------------------*/
      if ((char)ch == '\n')
      {
         *(string+i) = '\0';
         break;
      }
/*---------------------------------------------------------------------*/
/* All other characters, copy to string.                               */
/*---------------------------------------------------------------------*/
      *(string+i++) = (char)ch;
/*---------------------------------------------------------------------*/
/* If we have got to the end of the allocated memory, realloc some more*/
/*---------------------------------------------------------------------*/
      if (i == BUF_SIZE*bufs)
      {
         if ((string = (char *)realloc(string,(BUF_SIZE*(++bufs))+1)) == NULL)
         {
            *rcode = RC_OUT_OF_MEMORY;
            return(NULL);
         }
      }
   }
/*---------------------------------------------------------------------*/
/* Return a line read form the temporary file.                         */
/*---------------------------------------------------------------------*/
   *length = i;
   *rcode = 0;
   return(string);
}

/* create_tmpname create a temporary filename. It returns 1 on success and 0
 * if an error occurs. The argument should point to a buffer which is at least
 * REXX_PATH_MAX character long (incl. the term. 0). This buffer is filled on
 * success, only.
 * There are some limitations: All processes share the same name space.
 * (Well, maybe Regina only). If a new name is created this name will be
 * detected by another process as free, too. A new tmpname should be used
 * as fast as possible. Note, that the filename part of the tmpname is
 * limited to 8.3 since there might be problems with "higher" operating systems
 * when they access "lower" filesystems, e.g. OS/2 and FAT.
 */
int create_tmpname( const tsd_t *TSD, char *name )
{
   char *slash;
   char buf[REXX_PATH_MAX]; /* enough space for largest path name */
   unsigned i;
   unsigned long start,run;

   if ( mygetenv( TSD, "TMP", buf, sizeof(buf) ) == NULL)
   {
      if ( mygetenv( TSD, "TEMP", buf, sizeof(buf) ) == NULL)
      {
         if ( mygetenv( TSD, "TMPDIR", buf, sizeof(buf) ) == NULL)
         {
#ifdef UNIX
            strcpy(buf,"/tmp");
#else
            strcpy(buf,"C:");
#endif
         }
      }
   }

   if (strlen(buf) > REXX_PATH_MAX - 14 /* 8.3 + "\0" + ISLASH */)
      buf[REXX_PATH_MAX - 14] = '\0';

   if ( buf[strlen(buf)-1] != I_SLASH )
      slash = ISTR_SLASH;
   else
      slash = "";

   /* algorithm:
    * select a random number, e.g. a mixture of pid, tid and time.
    * increment this number by a fixed amount until we reach the starting
    * value again. Do a wrap around and an increment which is a unique prime.
    * The number 100000000 has the primes 2 and 5. We may use all primes
    * except 2 and 5; 9901 (which is prime) will give a good distribution.
    */
   start = TSD->thread_id;
   if (start == 0)
      start = 0xFFFFFFFFul;
   start *= (unsigned long) getpid();
   start *= (unsigned long) (clock() + 1);
   start %= 100000000ul;

   run = start;
   for (i = 0;i <= 10000;i++)            /* Not more than 10000 tries        */
   {
      /* form a name like "c:\temp\34503302._rx" or "/tmp/34503302._rx" */
      sprintf(name,"%s%s%08lu._rx", buf, slash, run );
      if (access(name,0) != 0)
         return(1);
      run += 9901;
      run %= 100000000ul;
      if (run == start)
         break;
   }
   return( 0 );
}

/***********************************************************************/
int dos_do_command( tsd_t *TSD, const streng *command, int io_flags, int dummy )
/***********************************************************************/
{
   int rc=0;
   streng *cmd=NULL ;
   int in=0, out=0, fout=0 ;
   streng *inredir=NULL,*outredir=NULL ;
   int length=0, rcode=0, flush=0 ;
   streng *result=NULL ;
   char *string=NULL;

   FILE *infp,*outfp;
   char infile[REXX_PATH_MAX];
   char outfile[REXX_PATH_MAX];

   in = io_flags & REDIR_INPUT ;
   out = io_flags & REDIR_OUTLIFO ;
   fout = io_flags & REDIR_OUTFIFO ;

   flush = (out!=0) + ((fout!=0)*2) ;

   if ((!in)&&(!out)&&(!fout))
   {
      ;       /* maybe this should not be allowed. ... */
   }
   dummy = dummy; /* keep compiler happy */
   cmd = NULL ;
/*---------------------------------------------------------------------*/
/* If redirecting stdin, create a temporary file and write the contents*/
/* of the stack to the file.                                           */
/*---------------------------------------------------------------------*/
   if (in)
   {
      if ( !create_tmpname( TSD, infile ) )
      {
         perror( "While getting temporary in-file name" ) ;
         return (-1);
      }
      if ( ( infp = fopen( infile, "w" ) ) == NULL )
      {
         perror( "While opening input file" ) ;
         return (-1);
      }
      while( !stack_empty( TSD ) )
      {
         result = popline( TSD, NULL, NULL, 0 ) ;
         fwrite( result->value, result->len, 1, infp ) ;
         fwrite( "\n", 1, 1, infp ) ;
         Free_stringTSD( result ) ;
      }
      if ( fclose( infp ) )
      {
         perror( "While closing input file" ) ;
         return (-1);
      }
      inredir = Str_makeTSD( strlen( infile ) + 4 );
      sprintf( inredir->value, "< %s ", str_trans( infile, '/', '\\' ) );
      inredir->len = strlen( inredir->value );
   }
/*---------------------------------------------------------------------*/
/* If redirecting stdout, create the name of a temporary file for the  */
/* output.                                                             */
/*---------------------------------------------------------------------*/
   if ((out)||(fout))
   {
      if ( !create_tmpname( TSD, outfile ) )
      {
         perror( "While getting temporary out-file name" ) ;
         return (-1);
      }
      outredir = Str_makeTSD( strlen( outfile ) + 3 );
      sprintf( outredir->value, "> %s", str_trans( outfile, '/', '\\' ) );
      outredir->len = strlen( outredir->value );
   }
/*---------------------------------------------------------------------*/
/* Build up the command to be passed to the system() command. The      */
/* command will contain the command to be executed, any arguments and  */
/* the redirection commands and file names.                            */
/*---------------------------------------------------------------------*/
   length = command->len + ((in) ? inredir->len : 0) + (((out)||(fout)) ? outredir->len : 0) + 2;
   cmd = Str_makeTSD(length);
   memcpy(cmd->value,command->value,command->len);
   cmd->len = command->len;
   if (in)
   {
      cmd = Str_catTSD(cmd,inredir);
      Free_stringTSD(inredir);
   }
   if (out||fout)
   {
      cmd = Str_catTSD(cmd,outredir);
      Free_stringTSD(outredir);
   }
/*---------------------------------------------------------------------*/
/* Execute the command.                                                */
/*---------------------------------------------------------------------*/
   string = str_of(TSD,cmd);
   rc = mysystem(TSD, string);
   FreeTSD(string);
   Free_stringTSD(cmd);
/*---------------------------------------------------------------------*/
/* If redirecting stdout, we now have to read the file and push each   */
/* line onto the stack.                                                */
/*---------------------------------------------------------------------*/
   if ((out)||(fout))
   {
      str_trans(outfile,'\\','/');
      if ((outfp = fopen(outfile,"r")) == NULL)
      {
         perror("While opening output file") ;
         return(-1);
      }

/*---------------------------------------------------------------------*/
/* If redirecting stdin, create a temporary file and write the contents*/
/* of the stack to the file.                                           */
/*---------------------------------------------------------------------*/
      for (;;)
      {
         string = get_a_line(outfp,&length,&rcode);
         if (rcode == RC_OUT_OF_MEMORY)
         {
            perror("While reading output file");
            return (-1);
         }
         if (rcode == RC_EOF && length == 0)
         {
            free(string);
            break;
         }
         result = Str_ncreTSD(string,length);
         tmp_stack(TSD,result,flush==2);
         free(string);
         if (rcode == RC_EOF)
            break;
      }
      if (flush)
         flush_stack(TSD, flush==2);

      if (fclose(outfp))
      {
         perror("While closing output file") ;
         return(-1);
      }
   }
/*---------------------------------------------------------------------*/
/* Delete the temporary file(s) and free up any memory.                */
/*---------------------------------------------------------------------*/
  if (in)
     unlink(str_trans(infile,'\\','/'));
  if ((out)||(fout))
     unlink(str_trans(outfile,'\\','/'));
/*---------------------------------------------------------------------*/
/* Return with, hopefully, return code from system() command.          */
/*---------------------------------------------------------------------*/
  return rc ;
}
# if defined(__WATCOMC__) || defined(_MSC_VER) || defined(__SASC) || defined(__MINGW32__) || defined(__IBMC__) || defined(__BORLANDC__) || defined(MAC) || defined(__AROS__)
/********************************************************* MH 10-06-96 */
int uname(struct utsname *name)                         /* MH 10-06-96 */
/********************************************************* MH 10-06-96 */
{                                                       /* MH 10-06-96 */
#  if defined (WIN32)
 SYSTEM_INFO sysinfo;
 OSVERSIONINFO osinfo;
 char computername[MAX_COMPUTERNAME_LENGTH+1];
 char *pComputerName=computername;
 DWORD namelen=MAX_COMPUTERNAME_LENGTH+1;
#  endif
/*-------------------------------------------------------- MH 10-06-96 */
/* Set up values for utsname structure...                  MH 10-06-96 */
/*-------------------------------------------------------- MH 10-06-96 */
#  if defined(OS2)                                      /* MH 10-06-96 */
 strcpy(name->sysname,"OS2");                           /* MH 10-06-96 */
 sprintf(name->version,"%d",_osmajor);                  /* MH 10-06-96 */
 sprintf(name->release,"%d",_osminor);                  /* MH 10-06-96 */
 strcpy(name->nodename,"standalone");
 strcpy(name->machine,"i386");
#  elif defined(_AMIGA) || defined(__AROS__)
 strcpy(name->sysname,"AMIGA");
 sprintf(name->version,"%d",0);
 sprintf(name->release,"%d",0);
 strcpy(name->nodename,"standalone");
 strcpy(name->machine,"m68k");
#  elif defined(MAC)
 strcpy(name->sysname,"MAC");
 sprintf(name->version,"%d",0);
 sprintf(name->release,"%d",0);
 strcpy(name->nodename,"standalone");
 strcpy(name->machine,"m68k");
#  elif defined(WIN32)
 osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
 GetVersionEx(&osinfo);
 sprintf(name->version,"%d",osinfo.dwMajorVersion);
 sprintf(name->release,"%d",osinfo.dwMinorVersion);

 /* get specific OS version... */
 switch(osinfo.dwPlatformId)
 {
    case VER_PLATFORM_WIN32s:
       strcpy(name->sysname,"WIN32S");
       break;
    case VER_PLATFORM_WIN32_WINDOWS:
       if ( osinfo.dwMinorVersion >= 90 )
          strcpy(name->sysname,"WINME");
       else if ( osinfo.dwMinorVersion >= 10 )
          strcpy(name->sysname,"WIN98");
       else
          strcpy(name->sysname,"WIN95");
       break;
    case VER_PLATFORM_WIN32_NT:
       if ( osinfo.dwMajorVersion == 4 )
          strcpy(name->sysname,"WINNT");
       else if ( osinfo.dwMajorVersion == 5 )
          strcpy(name->sysname,"WIN2K");
       else
          strcpy(name->sysname,"UNKNOWN");
       break;
    default:
       strcpy(name->sysname,"UNKNOWN");
       break;
 }
 /* 
  * get name of computer if possible.
  */
 if ( GetComputerName(pComputerName,&namelen) )
    strcpy(name->nodename,pComputerName);
 else
    strcpy(name->nodename,"UNKNOWN");
 GetSystemInfo(&sysinfo);
 switch(sysinfo.dwProcessorType)
 {
    case PROCESSOR_INTEL_386:
       strcpy(name->machine,"i386");
       break;
    case PROCESSOR_INTEL_486:
       strcpy(name->machine,"i486");
       break;
    case PROCESSOR_INTEL_PENTIUM:
       strcpy(name->machine,"i586");
       break;
#if 0
    case PROCESSOR_INTEL_MIPS_R4000:
       strcpy(name->machine,"mipsR4000");
       break;
    case PROCESSOR_INTEL_ALPHA_21064:
       strcpy(name->machine,"alpha21064");
       break;
#endif

 }
#  endif /* WIN32 */

 return(0);
}
# endif
#endif
