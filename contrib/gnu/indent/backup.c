/* Copyright (c) 1993,1994, Joseph Arceneaux.  All rights reserved.

   This file is subject to the terms of the GNU General Public License as
   published by the Free Software Foundation.  A copy of this license is
   included with this software distribution in the file COPYING.  If you
   do not have a copy, you may obtain a copy by writing to the Free
   Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details. */

/* GNU/Emacs style backups --
   This behaviour is controlled by two environment variables,
   VERSION_CONTROL and SIMPLE_BACKUP_SUFFIX.

   VERSION_CONTROL determines what kinds of backups are made.  If it's
   value is "numbered", then the first modification of some file
   "eraserhead.c" will yield a backup file "eraserhead.c.~1~", the
   second modification will yield "eraserhead.c.~2~", and so on.  It
   does not matter if the version numbers are not a sequence;  the next
   version will be one greater than the highest in that directory.

   If the value of VERSION_CONTROL is "numbered_existing", then such
   numbered backups will be made if there are already numbered backup
   versions of the file.  Otherwise, the backup name will be that of
   the original file with "~" (tilde) appended.  E.g., "eraserhead.c~".

   If the value of VERSION_CONTROL is "simple", then the backup name
   will be that of the original file with "~" appended, regardless of
   whether or not there exist numbered versions in the directory.

   For simple backups, the value of SIMPLE_BACKUP_SUFFIX will be used
   rather than "~" if it is set.

   If VERSION_CONTROL is unset, "numbered_existing" is assumed.  For
   Emacs lovers, "nil" is equivalent to "numbered_existing" and "t" is
   equivalent to "numbered".

   Finally, if VERSION_CONTROL is "none" or "never", backups are not
   made.  I suggest you avoid this behaviour.

   Added, october 1999 (by Chris F.A. Johnson):

   If VERSION_WIDTH is set, then it controls zero padding of a numbered
   suffix. */

/* Written by jla, based on code from djm (see `patch') */

#include "sys.h"
#include <ctype.h>
#include <stdlib.h>
#if defined (HAVE_UNISTD_H)
#include <unistd.h>
#endif
#ifdef PRESERVE_MTIME
#include <time.h>
#ifdef HAVE_UTIME_H
#include <utime.h>
#elif defined(HAVE_SYS_UTIME_H)
#include <sys/utime.h>
#endif
#endif
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <fcntl.h>
#endif
#include <string.h>
#ifndef isascii
#define ISDIGIT(c) (isdigit ((unsigned char) (c)))
#else
#define ISDIGIT(c) (isascii (c) && isdigit (c))
#endif
#include <sys/types.h>
#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
# if !defined(HAVE_SYS_NDIR_H) && !defined(HAVE_SYS_DIR_H) && !defined(HAVE_NDIR_H)
#  define NODIR 1
# endif
#endif
#include "backup.h"
#include "indent.h"
#include "globs.h"
#include "io.h"

RCSTAG_CC ("$Id$");

#ifndef NODIR
#if defined (_POSIX_VERSION)	/* Might be defined in unistd.h.  */
/* POSIX does not require that the d_ino field be present, and some
   systems do not provide it. */
#define REAL_DIR_ENTRY(dp) 1
#else
#define REAL_DIR_ENTRY(dp) ((dp)->d_ino != 0)
#endif
#else /* NODIR */
#define generate_backup_filename(v,f) simple_backup_name((f))
#endif /* NODIR */

#ifndef BACKUP_SUFFIX_STR
#define BACKUP_SUFFIX_STR    "~"
#endif

#ifndef BACKUP_SUFFIX_CHAR
#define BACKUP_SUFFIX_CHAR   '~'
#endif

#ifndef BACKUP_SUFFIX_FORMAT
#define BACKUP_SUFFIX_FORMAT "%s.~%0*d~"
#endif

/* Default backup file suffix to use */
static char *simple_backup_suffix = BACKUP_SUFFIX_STR;

/* What kinds of backup files to make -- see
   table `version_control_values' below. */
enum backup_mode version_control = unknown;
int version_width = 1;

/* Construct a simple backup name for PATHNAME by appending
   the value of `simple_backup_suffix'. */

static char *
simple_backup_name (pathname)
     char *pathname;
{
  char *backup_name;

  backup_name = xmalloc (strlen (pathname) + strlen (simple_backup_suffix) + 2);
  sprintf (backup_name, "%s%s", pathname, simple_backup_suffix);
  return backup_name;
}

#ifndef NODIR
/* If DIRENTRY is a numbered backup version of file BASE, return
   that number.  BASE_LENGTH is the string length of BASE. */

static int
version_number (base, direntry, base_length)
     char *base;
     char *direntry;
     int base_length;
{
  int version;
  char *p;

  version = 0;
  if (!strncmp (base, direntry, base_length) && ISDIGIT (direntry[base_length + 2]))
    {
      for (p = &direntry[base_length + 2]; ISDIGIT (*p); ++p)
	version = version * 10 + *p - '0';
      if (p[0] != BACKUP_SUFFIX_CHAR || p[1])
	version = 0;
    }

  return version;
}


/* Return the highest version of file FILENAME in directory
   DIRNAME.  Return 0 if there are no numbered versions. */

static int
highest_version (filename, dirname)
     char *filename, *dirname;
{
  DIR *dirp;
  struct dirent *dp;
  int highest_version;
  int this_version;
  int file_name_length;

  dirp = opendir (dirname);
  if (!dirp)
    return 0;

  highest_version = 0;
  file_name_length = strlen (filename);

  while ((dp = readdir (dirp)) != 0)
    {
      if (!REAL_DIR_ENTRY (dp) || NAMLEN (dp) <= file_name_length + 2)
	continue;

      this_version = version_number (filename, dp->d_name, file_name_length);
      if (this_version > highest_version)
	highest_version = this_version;
    }

  closedir (dirp);
  return highest_version;
}


/* Return the highest version number for file PATHNAME.  If there
   are no backups, or only a simple backup, return 0. */

static int
max_version (pathname)
     char *pathname;
{
  char *p;
  char *filename;
  int pathlen = strlen (pathname);
  int version;

  p = pathname + pathlen - 1;
  while (p > pathname && *p != '/')
    p--;

  if (*p == '/')
    {
      int dirlen = p - pathname;
      char *dirname;

      filename = p + 1;
      dirname = xmalloc (dirlen + 1);
      strncpy (dirname, pathname, (dirlen));
      dirname[dirlen] = '\0';
      version = highest_version (filename, dirname);
      free (dirname);
      return version;
    }

  filename = pathname;
  version = highest_version (filename, ".");
  return version;
}


/* Generate a backup filename for PATHNAME, dependent on the
   value of VERSION_CONTROL. */

static char *
generate_backup_filename (version_control, pathname)
     enum backup_mode version_control;
     char *pathname;
{
  int last_numbered_version;
  char *backup_name;

  if (version_control == none)
    return 0;

  if (version_control == simple)
    return simple_backup_name (pathname);

  last_numbered_version = max_version (pathname);
  if (version_control == numbered_existing && last_numbered_version == 0)
    return simple_backup_name (pathname);

  last_numbered_version++;
  backup_name = xmalloc (strlen (pathname) + 16);
  if (!backup_name)
    return 0;

  sprintf (backup_name, BACKUP_SUFFIX_FORMAT, pathname, version_width, (int) last_numbered_version);

  return backup_name;
}

#endif /* !NODIR */

static struct version_control_values values[] = {
  {none, "never"},		/* Don't make backups. */
  {none, "none"},		/* Ditto */
  {simple, "simple"},		/* Only simple backups */
  {numbered_existing, "existing"},	/* Numbered if they already exist */
  {numbered_existing, "nil"},	/* Ditto */
  {numbered, "numbered"},	/* Numbered backups */
  {numbered, "t"},		/* Ditto */
  {unknown, 0}			/* Initial, undefined value. */
};


/* Determine the value of `version_control' by looking in the
   environment variable "VERSION_CONTROL".  Defaults to
   numbered_existing. */

enum backup_mode
version_control_value ()
{
  char *version;
  struct version_control_values *v;

  version = getenv ("VERSION_CONTROL");
  if (version == 0 || *version == 0)
    return numbered_existing;

  v = &values[0];
  while (v->name)
    {
      if (strcmp (version, v->name) == 0)
	return v->value;
      v++;
    }

  return unknown;
}


/* Initialize information used in determining backup filenames. */

void
set_version_width (void)
{
  char *v = getenv ("VERSION_WIDTH");

  if (v && ISDIGIT (*v))
    version_width = atoi (v);
  if (version_width > 16)
    version_width = 16;
}

void
initialize_backups ()
{
  char *v = getenv ("SIMPLE_BACKUP_SUFFIX");

  if (v && *v)
    simple_backup_suffix = v;
#ifdef NODIR
  version_control = simple;
#else /* !NODIR */
  version_control = version_control_value ();
  if (version_control == unknown)
    {
      fprintf (stderr, "indent:  Strange version-control value\n");
      fprintf (stderr, "indent:  Using numbered-existing\n");
      version_control = numbered_existing;
    }
#endif /* !NODIR */
  set_version_width ();
}


/* Make a backup copy of FILE, taking into account version-control.
   See the description at the beginning of the file for details. */

void
make_backup (file, file_stats)
     struct file_buffer *file;
     const struct stat *file_stats;
{
  FILE *bf;
  char *backup_filename;
  unsigned int size;

  if (version_control == none)
    return;

  backup_filename = generate_backup_filename (version_control, file->name);
  if (!backup_filename)
    {
      fprintf (stderr, "indent: Can't make backup filename of %s\n", file->name);
      exit (system_error);
    }

  bf = fopen (backup_filename, "w");
  if (!bf)
    fatal ("Can't open backup file %s", backup_filename);
  size = fwrite (file->data, file->size, 1, bf);
  if (size != 1)
    fatal ("Can't write to backup file %s", backup_filename);

  fclose (bf);
#ifdef PRESERVE_MTIME
  {
    struct utimbuf buf;

    buf.actime = time (NULL);
    buf.modtime = file_stats->st_mtime;
    if (utime (backup_filename, &buf) != 0)
      WARNING ("Can't preserve modification time on backup file %s", backup_filename, 0);
  }
#endif
  free (backup_filename);
}
