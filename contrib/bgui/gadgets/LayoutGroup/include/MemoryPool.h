/*
 * @(#) $Header$
 *
 * BGUI library
 *
 * (C) Copyright 2000 BGUI Developers Team.
 * (C) Copyright 1998 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 1.2  2000/05/09 19:59:56  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:56  mlemos
 * Initial revision.
 *
 *
 */

typedef void *oBGUI_POOL;

void *oBGUIPoolAllocate(oBGUI_POOL pool,unsigned long size);
void *oBGUIPoolReallocate(oBGUI_POOL pool,void *memory,unsigned long size);
int oBGUIPoolFree(oBGUI_POOL pool,void *memory);
oBGUI_POOL oBGUICreatePool(void);
void oBGUIDestroyPool(oBGUI_POOL pool);
