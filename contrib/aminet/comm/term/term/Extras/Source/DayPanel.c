/*
**	DayPanel.c
**
**	Editing panel for day of week selection
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_USE=1,GAD_CANCEL,GAD_TEXT };

BOOL
DayPanel(struct Window *Parent,struct TimeDateNode *Node)
{
	struct TimeDateNode LocalNode;
	LayoutHandle *Handle;
	BOOL Result;

	Result = FALSE;

	CopyMem(Node,&LocalNode,sizeof(struct TimeDateNode));

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExitFlush,		FALSE,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LONG i;

					for(i = 0 ; i < 7 ; i++)
					{
						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,		MSG_DAYPANEL_MONDAY_GAD + i,
							LA_ID,			GAD_TEXT + i,
							GTCB_Checked,	LocalNode.Header.Day & (1L << i),
						TAG_DONE);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	XBAR_KIND,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_GLOBAL_COMMENT_GAD,
						LA_STRPTR,		LocalNode.Header.Comment,
						LA_Chars,		15,
						GTST_MaxChars,	sizeof(LocalNode.Header.Comment) - 1,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

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
			LAWN_TitleID,		MSG_DAYPANEL_DAY_SETTINGS_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
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

			GuideContext(CONTEXT_DAY);

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

								LT_UpdateStrings(Handle);

								CopyMem(&LocalNode,Node,sizeof(struct TimeDateNode));

								Result = Done = TRUE;

								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;

							default:

								if(MsgGadget->GadgetID >= GAD_TEXT && MsgGadget->GadgetID < GAD_TEXT + 7)
								{
									LONG Index = MsgGadget->GadgetID - GAD_TEXT;

									if(MsgCode)
										LocalNode.Header.Day |= (1L << Index);
									else
										LocalNode.Header.Day &= ~(1L << Index);
								}

								break;
						}
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	return(Result);
}
