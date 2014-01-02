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

#ifndef NOTIFYCONSTANTS_H
#define NOTIFYCONSTANTS_H

#define TY_NOTHING      0       /* No extra argument to consider */
#define TY_WINOBJ       1       /*
                                   notification on an element of a window (so window is needed)
                                   used only with "Activate" because we need to know the window
                                   in which is the object
                                 */
#define TY_FUNCTION     2       /* argument is a call hook */
#define TY_ID           3       /* argument is a constant identifier */
#define TY_VARIABLE     4       /* argument is a variable name */
#define TY_CONS_INT     5       /* argument is a constant integer value */
#define TY_CONS_STRING  6       /* argument is a constant string */
#define TY_CONS_CHAR    7       /* argument is a constant character */
#define TY_CONS_BOOL    8       /* argument is a constant Boolean */

#define Current_Max     24

#endif
