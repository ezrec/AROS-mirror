/*
**	PickScreen.c
**
**	Simplified public screen selection routine
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LIST=1,GAD_USE,GAD_CANCEL };

	/* BuildScreenList():
	 *
	 *	Build a private copy of the public screen list.
	 */

STATIC struct List *
BuildScreenList(VOID)
{
	struct List *List,*PubScreenList;

		/* Get the list body. */

	if(List = CreateList())
	{
			/* Get access to the public screen list. */

		if(PubScreenList = LockPubScreenList())
		{
			struct Node *Next,*Node;

				/* Scan the list. */

			for(Node = PubScreenList->lh_Head ; Next = Node->ln_Succ ; Node = Next)
			{
					/* Don't include the current `term' public
					 * screen name in it.
					 */

				if(strcmp(Node->ln_Name,TermIDString))
				{
					struct Node *New;

						/* Got a new node? */

					if(New = CreateNode(Node->ln_Name))
						AddTail(List,New);
				}
			}

			UnlockPubScreenList();
		}

			/* In case the list happens to remain empty,
			 * include the Workbench screen in it.
			 */

		if(IsListEmpty(List))
		{
			struct Node *New;

			if(New = CreateNode("Workbench"))
				AddTail(List,New);
			else
			{
				FreeVecPooled(List);
				List = NULL;
			}
		}
	}

	return(List);
}

	/* PickScreen(STRPTR Name):
	 *
	 *	Your nice public screen selection routine.
	 */

BOOL
PickScreen(struct Window *Window,STRPTR Name)
{
	struct List	*ScreenList;
	BOOL Result;

	Result = FALSE;

	if(ScreenList = BuildScreenList())
	{
		LayoutHandle *Handle;

		if(Handle = LT_CreateHandleTags(Window->WScreen,
			LAHN_LocaleHook,	&LocaleHook,
		TAG_DONE))
		{
			LONG MaxWidth,MaxHeight,Len;
			struct Window *PanelWindow;
			struct Node	*Node;
			ULONG Index,i;

			Index = (ULONG)~0;

			MaxWidth	= 0;
			MaxHeight	= 0;

			for(Node = ScreenList->lh_Head, i = 0 ; Node->ln_Succ ; Node = Node->ln_Succ, i++)
			{
				if(!Stricmp(Node->ln_Name,Name))
					Index = i;

				Len = strlen(Node->ln_Name);

				if(Len > MaxWidth)
					MaxWidth = Len;

				MaxHeight++;
			}

			if(MaxWidth < 30)
				MaxWidth = 30;

			if(MaxHeight < 5)
				MaxHeight = 5;
			else
			{
				if(MaxHeight > 20)
					MaxHeight = 20;
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		LISTVIEW_KIND,
						LA_Chars,		30,
						LA_ID,			GAD_LIST,
						LALV_Lines,		5,
						LALV_MaxGrowX,	MaxWidth,
						LALV_MaxGrowY,	MaxHeight,
						LALV_ResizeY,	TRUE,
						GTLV_Labels,	ScreenList,
						GTLV_Selected,	Index,
						LALV_Link,		NIL_LINK,
						LALV_CursorKey,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		XBAR_KIND,
						LAXB_FullSize,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,LA_Type,HORIZONTAL_KIND,
					LAGR_SameSize,	TRUE,
					LAGR_Spread,	TRUE,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_GLOBAL_USE_GAD,
						LA_ID,			GAD_USE,
						LABT_ReturnKey,	TRUE,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
						LA_ID,			GAD_CANCEL,
						LABT_EscKey,	TRUE,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			if(PanelWindow = LT_Build(Handle,
				LAWN_TitleID,		MSG_TERMPICKSCREEN_SCREENS_TXT,
				LAWN_HelpHook,		&GuideHook,
				LAWN_Parent,		Window,
				WA_DepthGadget,		TRUE,
				WA_DragBar,			TRUE,
				WA_RMBTrap,			TRUE,
				WA_Activate,		TRUE,
				WA_SimpleRefresh,	TRUE,
			TAG_DONE))
			{
				struct IntuiMessage	*Message;
				struct Gadget *MsgGadget;
				ULONG MsgClass;
				UWORD MsgCode;
				BOOL Done;

				LT_ShowWindow(Handle,TRUE);

				PushWindow(PanelWindow);

				Done = FALSE;

				do
				{
					if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
						break;

					while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
					{
						MsgClass	= Message->Class;
						MsgCode		= Message->Code;
						MsgGadget	= (struct Gadget *)Message->IAddress;

						LT_ReplyIMsg(Message);

						if(MsgClass == IDCMP_GADGETUP)
						{
							switch(MsgGadget->GadgetID)
							{
								case GAD_USE:

									if(Index != (ULONG)~0)
									{
										struct Node *Node;

										if(Node = GetListNode(Index,ScreenList))
										{
											strcpy(Name,Node->ln_Name);

											Result = TRUE;
										}
									}

									Done = TRUE;
									break;

								case GAD_CANCEL:

									Done = TRUE;
									break;

								case GAD_LIST:

									Index = MsgCode;
									break;
							}
						}

						if(MsgClass == IDCMP_IDCMPUPDATE)
						{
							struct Node *Node;

							if(Node = GetListNode(Index = MsgCode,ScreenList))
							{
								strcpy(Name,Node->ln_Name);

								Done = Result = TRUE;

								LT_PressButton(Handle,GAD_USE);
							}
						}
					}
				}
				while(!Done);

				PopWindow();
			}

			LT_DeleteHandle(Handle);
		}

		DeleteList(ScreenList);
	}
	else
		DisplayBeep(Window->WScreen);

	return(Result);
}
