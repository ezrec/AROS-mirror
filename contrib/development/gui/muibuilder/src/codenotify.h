/***************************************************************************

  MUIBuilder - MUI interface builder
  Copyright (C) 1990-2009 by Eric Totel
  Copyright (C) 2010-2011 by MUIBuilder Open Source Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  MUIBuilder Support Site: http://sourceforge.net/projects/muibuilder/

  $Id$$

***************************************************************************/

#ifndef CODENOTIFY_H
#define CODENOTIFY_H

#include "notifyconstants.h"

/* constantes */
#define ID_evt
#define ID_obj
#define ID_action

/* Type of the argument of the notification */
#define ARG_BOOL        0
#define ARG_INT         1
#define ARG_OBJ         2
#define ARG_STRING      3
#define ARG_MUI         4

extern int *TYActions[];
extern int *ArgEVT[];
extern int *CActions[];
extern int *CEvenements[];

#endif
