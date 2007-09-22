#ifndef _plan9_
#include <string.h>
#ifdef __EMX__
#include <sys/types.h>
#endif
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#else
#include <u.h>
#include <libc.h>
#endif
#include <config.h>
#include <filter.h>
#include <fractal.h>
#include <ui_helper.h>
xio_path xio_getdirectory(xio_path filename)
{
  int i;
  xio_pathdata directory;
  for (i = strlen (filename); i && filename[i] != '/' &&
      filename[i] != '\\'; i--);
  if (filename[i] == '/' || filename[i] == '\\')
    i++;
  directory[i] = 0;
  i--;
  for (; i >= 0; i--)
    directory[i] =
      filename[i];
  return(strdup(directory));
}

xio_path 
xio_getfilename (char *basename, char *extension)
{
  static char name[40];
  int nimage = 0;
#ifdef _plan9_
  char edir[DIRLEN];
#else
  struct stat sb;
#endif
  do
    {
      sprintf (name, "%s%i%s", basename, nimage++, extension);
#ifndef _plan9_
    }
  while (stat (name, &sb) != -1);
#else
    }
  while (stat (name, edir) != -1);
#endif
  return (name);
}
xio_file
xio_getrandomexample(xio_path name)
{
#ifndef _plan9_
  static char *paths[] =
  {				/*Where examples should be located? */
    EXAMPLESPATH,		/*Data path when XaoS is propertly installed */
    "./examples/",		/*XaoS was started from root of source tree */
    "../examples/",		/*XaoS was started from bin directory in source tree */
    "./",			/*Oops...it's not. Try curent directory */
  };
  int i, p;
  DIR *d = NULL;
  xio_file f;
  struct dirent *dir;
  int n;
  int max;

  for (p = 0; p < (int) (sizeof (paths) / sizeof (char *)) && d == NULL; p++)
    {
      d = opendir (paths[p]);
      if (d != NULL)
	break;
    }

  if (d == NULL)
    {
      /*uih->errstring = "Can not open examples directory";*/
      return NULL;
    }
  max = 800 - strlen (paths[p]);

  for (i = 0; (dir = readdir (d)) != NULL; i++)
    {
      int s = strlen (dir->d_name);
      if (s > max || s < 4 || strcmp (".xpf", dir->d_name + s - 4))
	i--;
      /*free(dir); */
    }
  if (!i)
    {
      /*uih->errstring = "No *.xpf files found";*/
      closedir (d);
      return NULL;
    }

  rewinddir (d);
  dir = NULL;
  n = ((number_t) i * rand () / (RAND_MAX + 1.0));

  for (i = 0; i <= n; i++)
    {
      int s;
      do
	{
	  /*if(dir!=NULL) free(dir); */
	  dir = readdir (d);
	  if (dir == NULL)
	    {
	      /*uih->errstring = "Reading of examples directory failed";*/
	      closedir (d);
	      return NULL;
	    }
	  s = strlen (dir->d_name);
	}
      while (s > max || s < 4 || strcmp (".xpf", dir->d_name + s - 4));
    }
  if (dir == NULL)
    {
      /*uih->errstring = "Reading of examples directory failed";*/
      closedir (d);
      return NULL;
    }
  strcpy (name, paths[p]);
  strcat (name, dir->d_name);
  closedir (d);
  /*free(dir); */

  f = fopen (name, "r");
  return(f);
#endif
}
xio_file xio_getcatalog(char *name)
{
  static xio_path paths[] =
  {                             /*Where catalogs should be located? */
    CATALOGSPATH,               /*Data path when XaoS is propertly installed */
    "./catalogs/",              /*XaoS was started from root of source tree */
    "../catalogs/",             /*XaoS was started from bin directory in source tree */
    "./",                       /*Oops...it's not. Try curent directory */
  };
  int i;
  xio_file f = XIO_FAILED;
  xio_pathdata tmp;
  for (i = 0; i < (int) (sizeof (paths) / sizeof (char *)) && f == XIO_FAILED; i++)
  {
    xio_addfname(tmp,paths[i],name);
    f = xio_ropen (tmp);
    if (f == XIO_FAILED)
    {
      xio_addextension(tmp,".cat");
      f = xio_ropen (tmp);
    }
  }
  return(f);
}
xio_file xio_gettutorial(char *name,xio_path tmp)
{
  int i;
  xio_file f = XIO_FAILED;
  static xio_path paths[] =
  {                             /*Where tutorials should be located? */
    TUTORIALPATH,               /*Data path when XaoS is propertly installed */
    "./",                       /*Oops...it's not. Try curent directory */
    "./tutorial/",              /*XaoS was started from root of source tree */
    "../tutorial/",             /*XaoS was started from bin directory in source tree */
  };
  for (i = 0; i < (int) (sizeof (paths) / sizeof (char *)) && f == XIO_FAILED; i++)
  {
    xio_addfname(tmp, paths[i],name);
    f = xio_ropen (tmp);
  }
  return(f);
}
