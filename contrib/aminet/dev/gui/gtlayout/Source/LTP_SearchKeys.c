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

#ifdef DO_PICKSHORTCUTS	/* Support code */

VOID
LTP_SearchKeys(LayoutHandle *handle,ObjectNode *group)
{
	ObjectNode	*node;
	ULONG		 page;

	SCANPAGE(group,node,page)
	{
		if(node->Type == GROUP_KIND)
			LTP_SearchKeys(handle,node);
		else
		{
			if(node->Key)
			{
				LONG i;

				for(i = 0 ; i < 256 ; i++)
				{
					if(LTP_Keys[0][i] == node->Key || LTP_Keys[1][i] == node->Key)
					{
						handle->Keys[LTP_Keys[0][i]] = TRUE;
						handle->Keys[LTP_Keys[1][i]] = TRUE;

						break;
					}
				}

				if(LTP_Keys[1][node->Key])
					node->Key = LTP_Keys[0][node->Key];
			}
			else
			{
				if((handle->CursorKey == node) || (handle->ReturnKey == node) || (handle->EscKey == node) || (handle->TabKey == node))
					node->NoKey = TRUE;
			}
		}
	}
}

#endif	/* DO_PICKSHORTCUTS */
