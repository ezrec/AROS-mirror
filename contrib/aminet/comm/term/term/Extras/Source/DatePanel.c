/*
**	DatePanel.c
**
**	Editing panel for date selection
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_MONTH=1,GAD_DAY,GAD_COMMENT,GAD_USE,GAD_CANCEL };

BOOL
DatePanel(struct Window *Parent,struct TimeDateNode *Node)
{
	STATIC BYTE MaxDays[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	struct TimeDateNode LocalNode;
	LayoutHandle *Handle;
	BOOL Result;

	Result = FALSE;;

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
					LA_Type,			CYCLE_KIND,
					LA_LabelID,			MSG_DATEPANEL_MONTH_GAD,
					LA_BYTE,			&LocalNode.Header.Month,
					LA_ID,				GAD_MONTH,
					LACY_FirstLabel,	MSG_DATEPANEL_JANUARY_TXT,
					LACY_LastLabel,		MSG_DATEPANEL_DECEMBER_TXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			SliderType,
					LA_LabelID,			MSG_DATEPANEL_DAY_GAD,
					LA_BYTE,			&LocalNode.Header.Day,
					LA_ID,				GAD_DAY,
					GTSL_Min,			1,
					GTSL_Max,			MaxDays[(WORD)LocalNode.Header.Month],
					LASL_FullCheck,		TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			XBAR_KIND,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			STRING_KIND,
					LA_LabelID,			MSG_GLOBAL_COMMENT_GAD,
					LA_STRPTR,			LocalNode.Header.Comment,
					GTST_MaxChars,		sizeof(LocalNode.Header.Comment) - 1,
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
			LAWN_TitleID,		MSG_DATEPANEL_DATE_SETTINGS_TXT,
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
			BOOL Done;

			Done = FALSE;

			GuideContext(CONTEXT_DATE);

			LT_ShowWindow(Handle,TRUE);

			PushWindow(PanelWindow);

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_MONTH:

								LT_SetAttributes(Handle,GAD_DAY,
									GTSL_Max,MaxDays[(WORD)LocalNode.Header.Month],
								TAG_DONE);

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								CopyMem(&LocalNode,Node,sizeof(struct TimeDateNode));

								Result = Done = TRUE;

								break;

							case GAD_CANCEL:

								Done = TRUE;
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
