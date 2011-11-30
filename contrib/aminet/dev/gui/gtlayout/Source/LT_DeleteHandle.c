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

#ifdef DO_BOOPSI_KIND	/* Support code */
STATIC VOID
LTP_BOOPSI_Cleanup(ObjectNode *group)
{
	ObjectNode *node;

	SCANGROUP(group,node)
	{
		switch(node->Type)
		{
			case BOOPSI_KIND:

				DB(kprintf("2) disposing boopsi object\n"));

				CloseLibrary(node->Special.BOOPSI.ClassBase);
				node->Special.BOOPSI.ClassBase = NULL;

				FreeTagItems(node->Special.BOOPSI.ClassTags);
				node->Special.BOOPSI.ClassTags = NULL;

				break;

			case GROUP_KIND:

				LTP_BOOPSI_Cleanup(node);
				break;
		}
	}
}
#endif /* DO_BOOPSI_KIND */

VOID
LTP_DisposeGadgets(struct LayoutHandle *Handle)
{
	if(Handle->List)
	{
		struct Gadget	*Gadget;
		ObjectNode		*Node;

		LTP_StripGadgets(Handle,Handle->List);

		Gadget = Handle->List;

		if(Handle->ExitFlush)
		{
			while(Gadget)
			{
				if(GETOBJECT(Gadget,Node))
				{
					LTP_PutStorage(Node);

					LTP_DeleteObjectNode(Handle,Node);
				}

				Gadget = Gadget->NextGadget;
			}
		}
		else
		{
			while(Gadget)
			{
				if(GETOBJECT(Gadget,Node))
					LTP_DeleteObjectNode(Handle,Node);

				Gadget = Gadget->NextGadget;
			}
		}

		FreeGadgets(Handle->List);

		Handle->List = NULL;
	}

	#ifdef DO_BOOPSI_KIND
	{
		if(Handle->BOOPSIList)
		{
			struct Gadget *gadget = (struct Gadget *)Handle->BOOPSIList,*next;
			ObjectNode *Node;

			LTP_StripGadgets(Handle,(struct Gadget *)Handle->BOOPSIList);

			while(gadget)
			{
				next = gadget->NextGadget;

				if(GETOBJECT(gadget,Node))
				{
					DB(kprintf("1) disposing boopsi object\n"));

					DisposeObject(gadget);
					Node->Host = NULL;
				}

				gadget = next;
			}

			Handle->BOOPSIList = NULL;
			Handle->BOOPSIPrevious = NULL;
		}

			// Clean up all BOOPSI objects. This includes even those
			// which have never been displayed.

		LTP_BOOPSI_Cleanup(Handle->TopGroup);
	}
	#endif	/* DO_BOOPSI_KIND */
}


/*****************************************************************************/


/****** gtlayout.library/LT_DeleteHandle ******************************************
*
*   NAME
*	LT_DeleteHandle -- Release storage space allocated by
*	                   LT_CreateHandleTagList, closing windows,
*	                   removing gadgets, etc.
*
*   SYNOPSIS
*	LT_DeleteHandle(Handle);
*	                  A0
*
*	VOID LT_DeleteHandle(LayoutHandle *);
*
*   FUNCTION
*	Windows and gadgets created by LT_CreateHandleTagList()
*	are removed, any associated memory is deallocated.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure created
*	    by LT_CreateHandleTaglist(). Passing NULL is
*	    harmless.
*
*   RESULT
*	none
*
*   SEE ALSO
*	gtlayout.library/CreateHandleTagList
*
******************************************************************************
*
*/

VOID LIBENT
LT_DeleteHandle(REG(a0) LayoutHandle *Handle)
{
	if(Handle)
	{
		if(Handle->UnlockPubScreen)
			UnlockPubScreen(NULL,Handle->PubScreen);

		#ifdef DO_CLONING
		{
			if(Handle->CloneExtra && Handle->CloneExtra->Screen)
				ScreenToBack(Handle->CloneExtra->Screen);
		}
		#endif

		LTP_DisposeGadgets(Handle);

		LT_UnlockWindow(Handle->Parent);

		if(Handle->Window)
		{
			LT_DeleteWindowLock(Handle->Window);

			if(Handle->MsgPort && Handle->Window->UserPort)
			{
				struct IntuiMessage	*intuiMsg;
				struct Node			*next;

				Forbid();

				for(intuiMsg = (struct IntuiMessage *)Handle->Window->UserPort->mp_MsgList.lh_Head ; next = intuiMsg->ExecMessage.mn_Node.ln_Succ ; intuiMsg = (struct IntuiMessage *)next)
				{
					if(intuiMsg->IDCMPWindow == Handle->Window)
					{
						Remove((struct Node *)intuiMsg);

						ReplyMsg((struct Message *)intuiMsg);
					}
				}

				Handle->Window->UserPort = NULL;

				ModifyIDCMP(Handle->Window, 0);

				Permit();
			}

			ClearMenuStrip(Handle->Window);
			CloseWindow(Handle->Window);
		}

		#ifdef DO_MENUS
		{
			LT_DisposeMenu(Handle->Menu);
		}
		#endif	// DO_MENUS

		DisposeObject(Handle->AmigaGlyph);
		DisposeObject(Handle->CheckGlyph);

		FreeScreenDrawInfo(Handle->Screen,Handle->DrawInfo);

		FreeVisualInfo(Handle->VisualInfo);

		DisposeObject(Handle->BevelImage);
		DisposeObject(Handle->GrooveImage);

		if(Handle->RPort.Font && Handle->CloseFont)
			CloseFont(Handle->RPort.Font);

		#ifdef DO_CLONING
		{
			if(Handle->CloneExtra && Handle->CloneExtra->Screen)
				CloseScreen(Handle->CloneExtra->Screen);
		}
		#endif

		AsmDeletePool(Handle->Pool,SysBase);
	}
}
