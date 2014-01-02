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

  $Id$

***************************************************************************/

#include <string.h>
#include <proto/dos.h>

#include "file.h"

void extract_dir( char *filename )
{
	char *aux;

	aux = strrchr( filename, '/' );
	if (!aux) aux = strrchr( filename, ':' );
	if (aux)
	{
		aux++;
		*aux = '\0';
	}
	else filename[0] = '\0';
}

void extract_file( char * path, char * filename )
{
	strcpy( filename, FilePart( path ) );
}

void add_extend( char *filename, char * extend )
{
	char *aux;

	aux = strrchr( filename, '.' );
	if (!aux) strcat( filename, extend );
	else if (strcmp( aux, extend )!=0) strcat( filename, extend );
}

void remove_extend( char *filename )
{
	char *aux;

	aux = strrchr( filename, '.' );
	if (aux) *aux='\0';
}

void change_extend( char *filename, char * extend )
{
	remove_extend( filename );
	add_extend( filename, extend );
}
		
