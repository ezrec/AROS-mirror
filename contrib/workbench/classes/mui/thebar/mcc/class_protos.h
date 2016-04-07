/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

/* utils.c */
#if !defined(__MORPHOS__)
#if defined(__AROS__)
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, Tag tag1, ...);
#else
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...);
#endif
#endif

BOOL CreateSharedPool(void);
void DeleteSharedPool(void);
APTR SharedAlloc(ULONG size);
void SharedFree(APTR mem);

/* brc1.c */
int BRCUnpack(APTR pSource, APTR pDest, LONG srcBytes0, LONG dstBytes0);

/* spacer.c */
BOOL initSpacerClass(void);

/* dragbar.c */
BOOL initDragBarClass(void);


