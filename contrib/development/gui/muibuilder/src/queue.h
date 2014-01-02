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

#ifndef QUEUE_H
#define QUEUE_H

#include <exec/types.h>

typedef struct chainon1
{
    APTR element;
    int ident;
    struct chainon1 *next;
} chainon;

typedef struct queue1
{
    chainon *head;
    chainon *tail;
    int nb_elements;
} queue;

extern void add(queue *, APTR);
extern void delete_nth(queue *, int);
extern void delete_all(queue *);
extern APTR nth(queue *, int);
extern queue *create(void);
extern void exchange(queue *, int, int);
extern void up(queue *, int);
extern void down(queue *, int);

#endif
