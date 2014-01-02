/***************************************************************************

  muibuilder.library - Support library for MUIBuilder's code generators
  Copyright (C) 1990-2009 by Eric Totel
  Copyright (C) 2010-2011 by MUIBuilder Open Source Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  MUIBuilder Support Site: http://sourceforge.net/projects/muibuilder/

  $Id$

***************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/* MB_GetA Tags */
#define MUIB_FileName           (TAG_USER + 200)
#define MUIB_CatalogName        (TAG_USER + 201)
#define MUIB_GetStringName      (TAG_USER + 202)
#define MUIB_Code               (TAG_USER + 203)
#define MUIB_Environment        (TAG_USER + 204)
#define MUIB_Locale             (TAG_USER + 205)
#define MUIB_Declarations       (TAG_USER + 206)
#define MUIB_VarNumber          (TAG_USER + 207)
#define MUIB_Notifications      (TAG_USER + 212)
#define MUIB_Application        (TAG_USER + 213)

/* MB_GetVarInfoA Tags */
#define MUIB_VarSize            (TAG_USER + 208)
#define MUIB_VarName            (TAG_USER + 209)
#define MUIB_VarType            (TAG_USER + 210)
#define MUIB_VarInitPtr         (TAG_USER + 211)

/* types of datas */

        /* variables types */
#define TYPEVAR_BOOL            1
#define TYPEVAR_INT             2
#define TYPEVAR_STRING          3
#define TYPEVAR_TABSTRING       4
#define TYPEVAR_PTR             5
#define TYPEVAR_HOOK            6

        /* Types of Code */
#define TC_CREATEOBJ            1
#define TC_ATTRIBUT             2
#define TC_END                  3
#define TC_FUNCTION             4
#define TC_STRING               5
#define TC_INTEGER              6
#define TC_CHAR                 7
#define TC_VAR_AFFECT           8
#define TC_VAR_ARG              9
#define TC_END_FUNCTION         10
#define TC_BOOL                 11
#define TC_MUIARG               12
#define TC_OBJFUNCTION          13
#define TC_LOCALESTRING         14
#define TC_EXTERNAL_CONSTANT    15
#define TC_EXTERNAL_FUNCTION    16
#define TC_MUIARG_ATTRIBUT      17
#define TC_MUIARG_FUNCTION      18
#define TC_MUIARG_OBJ           19
#define TC_BEGIN_NOTIFICATION   20
#define TC_END_NOTIFICATION     21
#define TC_EXTERNAL_VARIABLE    22
#define TC_MUIARG_OBJFUNCTION   23

#endif
