/* Copyright (c) 1999 Carlo Wood.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef INDENT_GLOBS_H
#define INDENT_GLOBS_H

RCSTAG_H (globs, "$Id$");

/* Warning messages:  indent continues */
#define WARNING(s,a,b) \
    message ("Warning", s, (unsigned int *)(a), (unsigned int *)(b))

/* Error messages: indent stops processing the current file. */
#define ERROR(s,a,b) \
    message ("Error", s, (unsigned int *)(a), (unsigned int *)(b))

extern char *xmalloc PARAMS ((unsigned int size));
extern char *xrealloc PARAMS ((char *ptr, unsigned int size));
extern void fatal PARAMS ((char *string, char *a0));
extern void message PARAMS ((char *kind, char *string, unsigned int *a0, unsigned int *a1));

#endif /* INDENT_GLOBS_H */
