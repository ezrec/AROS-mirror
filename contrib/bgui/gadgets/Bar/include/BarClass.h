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
 * Revision 42.0  2000/05/09 22:20:44  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:33  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:36  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:16  mlemos
 * Initial revision.
 *
 *
 */

#include <intuition/classes.h>

#define BAR_TB                 BGUI_TB+0x30000
#define BAR_Horizontal         BAR_TB+1
#define BAR_Vertical           BAR_TB+2
#define BAR_HorizontalPosition BAR_TB+3
#define BAR_HorizontalSize     BAR_TB+4
#define BAR_HorizontalTotal    BAR_TB+5
#define BAR_HorizontalPage     BAR_TB+6
#define BAR_VerticalPosition   BAR_TB+7
#define BAR_VerticalSize       BAR_TB+8
#define BAR_VerticalTotal      BAR_TB+9
#define BAR_VerticalPage       BAR_TB+10
#define BAR_MinimumKnobWidth   BAR_TB+11
#define BAR_MinimumKnobHeight  BAR_TB+12
