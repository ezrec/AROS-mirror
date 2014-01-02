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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <proto/dos.h>

void extract_catalog(char *filename)
{
    char *aux;

    aux = strrchr(filename, '/');
    if (!aux)
        aux = strrchr(filename, ':');
    if (aux)
    {
        aux++;
        *aux = '\0';
    }
    else
        filename[0] = '\0';
}

void extract_file(char *path, char *filename)
{
    strcpy(filename, FilePart(path));
}

void add_extend(char *filename, char *extend)
{
    char *aux, *aux2;
    char temp[512];
    char temp2[20];

    strncpy(temp2, extend, 20);
    aux2 = temp2;
    while (*aux2 != '\0')
    {
        if (islower(*aux2))
            *aux2 = toupper(*aux2);
        aux2++;
    }

    aux = strrchr(filename, '.');
    if (!aux)
        strcat(filename, extend);
    else
    {
        strncpy(temp, aux, 512);
        aux2 = temp;
        while (*aux2 != '\0')
        {
            if (islower(*aux2))
                *aux2 = toupper(*aux2);
            aux2++;
        }
        if (strcmp(temp, temp2) != 0)
            strcat(filename, extend);
    }
}

void remove_extend(char *filename)
{
    char *aux;

    aux = strrchr(filename, '.');
    if (aux)
        *aux = '\0';
}

void change_extend(char *filename, char *extend)
{
    remove_extend(filename);
    add_extend(filename, extend);
}
