/* Copyright (c) 1999, Carlo Wood.  All rights reserved.
   Copyright (c) 1993,1994, Joseph Arceneaux.  All rights reserved.

   This file is subject to the terms of the GNU General Public License as
   published by the Free Software Foundation.  A copy of this license is
   included with this software distribution in the file COPYING.  If you
   do not have a copy, you may obtain a copy by writing to the Free
   Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details. */

#ifndef INDENT_BACKUP_H
#define INDENT_BACKUP_H

RCSTAG_H (backup, "$Id$");

#include "io.h"

/* When to make backup files.  Analagous to 'version-control' in Emacs. */
enum backup_mode
{
  /* Uninitialized or indeterminate value */
  unknown,

  /* Never make backups. */
  none,

  /* Make simple backups of every file. */
  simple,

  /* Make numbered backups of files that already have numbered backups,
     and simple backups of the others. */
  numbered_existing,

  /* Make numbered backups of every file. */
  numbered
};

struct version_control_values
{
  enum backup_mode value;
  char *name;
};

/* Determine the value of `version_control' by looking in the environment
   variable "VERSION_CONTROL".  Defaults to numbered_existing. */
extern enum backup_mode version_control_value PARAMS ((void));

/* Initialize information used in determining backup filenames. */
extern void initialize_backups PARAMS ((void));

/* Make a backup copy of FILE, taking into account version-control.
   See the description at the beginning of the file for details. */
struct stat;
extern void make_backup PARAMS ((struct file_buffer * file, const struct stat * file_stats));

#endif /* INDENT_BACKUP_H */
