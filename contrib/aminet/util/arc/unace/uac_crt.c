/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Creates/Replaces files or directories.                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#include "os.h"

#include <fcntl.h>     // AROS: open()
#include <stdio.h>     // printf() remove()
#include <string.h>    // strncpy()
#include <sys/stat.h>  // struct stat

#ifdef UNIX
#include <unistd.h>
#else
 #if !defined(VA_CPP)
  #include <dos/dos.h>     // AROS: mkdir() rmdir() DOS: _dos_*()
 #endif
#endif
#if defined(DOS) || defined(WINNT) || defined(WIN16) || defined(VA_CPP)
 #include <io.h>       // access()
#if defined(WATCOM_C) || defined(VA_CPP)
 #include <direct.h>   // mkdir()
#else
 #include <dir.h>      // mkdir()
#endif
#endif

#include "globals.h"
#include "uac_crt.h"
#include "uac_sys.h"

/* gets file name from header
 */
CHAR *ace_fname(CHAR * s, thead * head, INT nopath)
{
   INT  i;
   char *cp;

   strncpy(s, (*(tfhead *) head).FNAME, i = (*(tfhead *) head).FNAME_SIZE);
   s[i] = 0;

   if (nopath)
   {
      cp=strrchr(s, '\\');
      if (cp)
         memmove(s, cp+1, strlen(cp));
   }
#if (DIRSEP!='\\')                  // replace msdos directory seperator
   else
   {                                // by current OS seperator
      cp=s;
      while ((cp=strchr(cp, '\\'))!=NULL)
         *cp++=DIRSEP;
   }
#endif

   return s;
}

void check_ext_dir(CHAR * f)        // checks/creates path of file
{
   CHAR *cp,
        d[PATH_MAX];
   INT  i;

   d[0] = 0;

   for (;;)
   {
      if ((cp = (CHAR *) strchr(&f[strlen(d) + 1], DIRSEP))!=NULL)
      {
         i = cp - f;
         strncpy(d, f, i);
         d[i] = 0;
      }
      else
         return;

      if (!fileexists(d))
         if (mkdir(d))
         {
            f_err = ERR_WRITE;
            printf("\n    Error while creating directory.\n");
         }
   }
}

INT  ovr_delete(CHAR * n)           // deletes directory or file
{
   if (remove(n) && rmdir(n))
   {
      printf("\n    Could not delete file or directory. Access denied.\n");
      return (1);
   }
   return (0);
}

INT  create_dest_file(CHAR * file, INT a)  // creates file or directory
{
   INT  han,
        i  = 0,
        ex = fileexists(file);
   struct stat st;

   check_ext_dir(file);
   if (f_err)
      return (-1);
   if (a & _A_SUBDIR)
   {                                // create dir or file?
      if (ex) stat(file, &st);
      if (ex ? (st.st_mode & S_IFDIR) : mkdir(file))
      {
         printf("\n    Could not create directory.\n");
         return (-1);
      }
#ifdef DOS
      _dos_setfileattr(file, a);    // set directory attributes
#endif
      return (-1);
   }
   else
   {
      if (ex)
      {                             // does the file already exist
         if (!f_ovrall)
         {
            i = wrask("Overwrite existing file?");  // prompt for overwrite
            f_ovrall = (i == 1);
            if (i == 3)
               f_err = ERR_USER;
         }
         if ((i && !f_ovrall) || ovr_delete(file))
            return (-1);            // delete?
      }
      if ((han = open(file, O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,
#ifdef __AROS__
                            S_IEXEC | S_IDELETE |
#else
S_IREAD | S_IWRITE | S_IEXEC | S_IDELETE |
#endif
                            S_IRGRP | S_IWGRP  | S_IROTH | S_IWOTH )) 
< 0)
         printf("\n    Could not create destination file.\n");
      return (han);
   }
}

