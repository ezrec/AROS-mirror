/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include "Assert.h"

VOID
LTP_AddAndRefreshGadgets(struct Window *Window,struct Gadget *Gadgets)
{
	AddGList(Window,Gadgets,(UWORD)-1,(UWORD)-1,NULL);
	RefreshGList(Gadgets,Window,NULL,(UWORD)-1);
}

VOID
LTP_StripGadgets(LayoutHandle *Handle,struct Gadget *Gadgets)
{
	if(Handle->Window && Gadgets)
		RemoveGList(Handle->Window,Gadgets,(UWORD)-1);
}

VOID
LTP_AddGadgets(LayoutHandle *handle)
{
	if(handle)
	{
		#ifdef DO_BOOPSI_KIND
		{
			if(handle->BOOPSIList)
				LTP_AddAndRefreshGadgets(handle->Window,(struct Gadget *)handle->BOOPSIList);
		}
		#endif	/* DO_BOOPSI_KIND */

		LTP_AddAndRefreshGadgets(handle->Window,handle->List);

		GT_RefreshWindow(handle -> Window,NULL);

		LTP_DrawGroup(handle,handle -> TopGroup);
	}
}

VOID
LTP_AddGadgetsDontRefresh(LayoutHandle *handle)
{
	if(handle)
	{
		#ifdef DO_BOOPSI_KIND
		{
			if(handle->BOOPSIList)
				AddGList(handle->Window,(struct Gadget *)handle->BOOPSIList,(UWORD)-1,(UWORD)-1,NULL);
		}
		#endif	/* DO_BOOPSI_KIND */

		AddGList(handle->Window,(struct Gadget *)handle->List,(UWORD)-1,(UWORD)-1,NULL);
	}
}
