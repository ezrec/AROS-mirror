#include <dos/dos.h>
#include <proto/dos.h>
#include <string.h>

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
  BPTR lock = 0;
  
  do
  {
    if (lock) UnLock(lock);
    sprintf (name, "%s%i%s", basename, nimage++, extension);
    lock = Lock(name, SHARED_LOCK);
  }
  while (lock != 0);

  return (name);
}

xio_file
xio_getrandomexample(xio_path name)
{
  return 0;
}
xio_file xio_getcatalog(char *name)
{
  static xio_path paths[] =
  {                             /*Where catalogs should be located? */
    CATALOGSPATH,               /*Data path when XaoS is propertly installed */
    "",                       /*Oops...it's not. Try curent directory */
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
    "",                         /*Oops...it's not. Try curent directory */
  };
  for (i = 0; i < (int) (sizeof (paths) / sizeof (char *)) && f == XIO_FAILED; i++)
  {
    xio_addfname(tmp, paths[i],name);
    f = xio_ropen (tmp);
  }
  return(f);
}
