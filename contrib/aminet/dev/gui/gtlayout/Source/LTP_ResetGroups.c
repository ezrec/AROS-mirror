/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include "Assert.h"

VOID
LTP_ResetGroups(ObjectNode *group)
{
	ObjectNode *node;

	group->Special.Group.ExtraLeft	= 0;
	group->Special.Group.ExtraTop	= 0;
	group->Special.Group.MaxOffset	= 0;
	group->Special.Group.MaxSize	= 0;
	group->Special.Group.Visible	= FALSE;
	group->Special.Group.MiscFlags	= 0;
	group->GroupIndent				= FALSE;
	group->Left						= 0;
	group->Top						= 0;
	group->Width					= 0;
	group->Height 					= 0;

	SCANGROUP(group,node)
	{
		if(node->Type == GROUP_KIND)
			LTP_ResetGroups(node);
	}
}
