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
 * Revision 42.0  2000/05/09 22:21:07  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:55  mlemos
 * Bumped to revision 41.11
 *
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
