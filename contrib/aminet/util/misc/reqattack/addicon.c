#include <exec/types.h>

#include <proto/wb.h>

#include "reqlist.h"

void AddIcon(struct ReqNode *reqnode)
{
	if (!(reqnode->appicon))
	{
		reqnode->appicon = AddAppIconA(1,(ULONG)reqnode, reqnode->title, reqnode->win->UserPort, NULL,reqnode->appicondiskobject, NULL);
	}
}
