/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
extern APTR AllocCopyString( char *string );
extern void FreeString( APTR string );

extern ULONG get(Object *obj, ULONG id);

extern APTR AllocNode(struct MinList *list, ULONG size);
extern void FreeNode(APTR node);
extern void FreeAllNodes(struct MinList *list);

extern void OrRectRegionArgs(struct Region *region, WORD minx, WORD miny, WORD maxx, WORD maxy);
extern void XorRectRegionArgs(struct Region *region, WORD minx, WORD miny, WORD maxx, WORD maxy);
extern void AndRectRegionArgs(struct Region *region, WORD minx, WORD miny, WORD maxx, WORD maxy);
extern void ClearRectRegionArgs(struct Region *region, WORD minx, WORD miny, WORD maxx, WORD maxy);
