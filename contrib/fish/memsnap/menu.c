/*
*	menu.c
*
*	NewMenus for MemSnap
*
*	MWS, 2/93.
*/

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <proto/gadtools.h>
#include "menu.h"

static struct NewMenu gt_items[] = {
	{ NM_TITLE, "MemSnapII",	NULL, 0, 0, NULL },
	{ NM_ITEM,  "Small window",     ".", 0, 0, NULL },
	{ NM_ITEM,  "Large window",	"/",0, 0, NULL },
	{ NM_ITEM,  "About...",		"A",0, 0, NULL },
	{ NM_ITEM,  "Quit",		"Q", 0, 0, NULL },

	{ NM_END }
};

static APTR vi;			/* visual info for gadtools */
static struct Menu *menu;

void FreeMemSnapMenu()		/* free all menustrips and related data */
{
	if (menu) FreeMenus(menu);
	if (vi) FreeVisualInfo(vi);
}

struct Menu *
AllocMemSnapMenu(struct Window *window)	/* allocate menustrips required by program */
{
	if ((menu = CreateMenusA(gt_items, NULL)) &&
	    (vi = GetVisualInfoA(window->WScreen, NULL)) &&
 	    (LayoutMenusA(menu, vi, NULL)))
		return menu;

	FreeMemSnapMenu();	/* something failed... */
	return NULL;
}
