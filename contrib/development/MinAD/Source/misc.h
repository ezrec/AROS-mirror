/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#ifndef MISC_H
#define MISC_H 1

/*------------------------------------------------------------------------*/

void RemPart(UBYTE *path);

/*------------------------------------------------------------------------*/

LONG ListSize(struct List *list);
struct Node *GetNode(struct List *list, LONG num);

/*------------------------------------------------------------------------*/

BOOL AssignAvail(UBYTE *assign, UBYTE *realpath);

/*------------------------------------------------------------------------*/

UBYTE *ReadFileMagic(UBYTE *filename, LONG *size, UBYTE *magic_id, LONG magic_len);

/*------------------------------------------------------------------------*/

UBYTE *LowerString(UBYTE *string, UBYTE *buffer);

/*------------------------------------------------------------------------*/

void ErrorMsg(UBYTE *fmt, ...);

/*------------------------------------------------------------------------*/

#endif /* MISC_H */

