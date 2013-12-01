//
// pmshadow.h
//
// PopupMenu Library - Shadows
//
// Copyright (C)2000 Henrik Isaksson <henrik@boing.nu>
// All Rights Reserved.
//
// $Date$
// $Revision$
//

#ifndef PM_SHADOW_H
#define PM_SHADOW_H

#ifndef PM_DLIST_H
#include "pmdlist.h"
#endif

#ifndef MAX
#define MAX(a,b)	(a>b?a:b)
#endif
#ifndef MIN
#define MIN(a,b)	(a<b?a:b)
#endif

#define	SHADOWFLAG_LEFT		(1L << 0)
#define	SHADOWFLAG_BOTTOM	(1L << 1)
#define	SHADOWFLAG_TOP		(1L << 2)
#define	SHADOWFLAG_RIGHT	(1L << 3)

struct PMShadowRect {
	PMGLN		n;
	WORD		Left;
	WORD		Top;
	WORD		Right;
	WORD		Bottom;
	UBYTE		pmsr_Type;
};

typedef struct PMShadowRect PMSR;
typedef struct MinList PMSRList;

#define PM_InitShadowList		(PMSRList *)PM_InitList
#define PM_FreeShadowList(l)		PM_FreeList((PMDList *)l)
#define PM_CopyShadowList(l)		(PMSRList *)PM_CopyList((PMDList *)l)	
#define PM_AddShadowToList(l, A)	PM_AddToList((PMDList *)l, (PMNode *)A)
#define PM_UnlinkShadow(l, A)		PM_Unlink((PMDList *)l, (PMNode *)A)
#define PM_FreeShadowNode(A)		PM_FreeNode((PMNode *)A)
//#define PM_CopyShadowNode(A)		(PMSR *)PM_CopyNode((PMNode *)A)
/*
void PM_AddShadowToList(PMSRList *l, PMSR *A);	// Add A to l. *
void PM_UnlinkShadow(PMSRList *l, PMSR *A);	// Remove A from l. *
void PM_FreeShadowNode(PMSR *A);		// Free a shadow node. *
*/
PMSR *PM_CopyShadowNode(PMSR *A);		// Copy a shadow node. *
	

//PMSRList *PM_InitShadowList(void);		// Create a new list header *
//void PM_FreeShadowList(PMSRList *list);		// Free a list of shadow rects *
//PMSRList *PM_CopyShadowList(PMSRList *list);	// Copy a list of shadow rects *

BOOL PM_AddShadow(PMSRList *sigma,		// Add shadows in delta to
		PMSRList *delta);		// sigma, and modify shadows
						// in delta so they will not
						// overlap shadows in sigma. *

BOOL PM_SubMenuRect(PMSRList *sigma,		// Subract a rectangle from
	WORD l, WORD t, WORD w, WORD h);	// list. *


//
// Support functions.
//

BOOL PM_ShadowOverlap(PMSR *A, PMSR *B);	// Does A touch B anywhere? *

#endif
