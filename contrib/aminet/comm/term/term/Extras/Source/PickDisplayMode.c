/*
**	PickDisplayMode.c
**
**	User interface for selecting screen display modes
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{ GAD_USE=1000,GAD_CANCEL };

STATIC ULONG
FilterModeID(ULONG ID,APTR UserData)
{
	struct DimensionInfo DimensionInfo;

	if(GetDisplayInfoData(NULL,(APTR)&DimensionInfo,sizeof(struct DimensionInfo),DTAG_DIMS,ID))
	{
		if(DimensionInfo.MaxDepth >= (LONG)(IPTR)UserData && DimensionInfo.MaxDepth <= 8)
			return(TRUE);
	}

	return(FALSE);
}

	/* PickDisplayMode(struct Window *Parent,ULONG *CurrentMode,struct Configuration *Config):
	 *
	 *	Pick a screen display mode from a list. If the Config pointer is not NULL,
	 *	will filter display modes out which cannot be used for the `term' main
	 *	screen, given the current colour restrictions.
	 */

BOOL
PickDisplayMode(struct Window *Parent,ULONG *CurrentMode,struct Configuration *Config)
{
	LONG MinDepth = 1;
	BOOL Success;

	Success = FALSE;

	if(Config)
	{
		switch(Config->ScreenConfig->ColourMode)
		{
			case COLOUR_AMIGA:

				MinDepth = 2;
				break;

			case COLOUR_EIGHT:

				MinDepth = 3;
				break;

			case COLOUR_SIXTEEN:

				MinDepth = 4;
				break;

			case COLOUR_MONO:

				MinDepth = 1;
				break;
		}
	}

		/* For poor Workbench 2.04 users... */

	if(AslBase->lib_Version < 38)
	{
		struct List	*ModeList;
		LONG Index;

		if(ModeList = BuildModeList(&Index,*CurrentMode,Config ? (MODEFILTER)FilterModeID : (MODEFILTER)NULL,(APTR)(IPTR)MinDepth))
		{
			struct LayoutHandle *Handle;

			if(Handle = LT_CreateHandleTags(Parent->WScreen,
				LAHN_LocaleHook,	&LocaleHook,
			TAG_DONE))
			{
				struct Window *LocalWindow;

				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,VERTICAL_KIND,
					TAG_DONE);
					{
						LONG MaxWidth,MaxHeight,Len;
						struct Node *Node;

						MaxWidth	= -1;
						MaxHeight	= 0;

						for(Node = ModeList->lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
						{
							Len = strlen(Node->ln_Name);

							if(Len > MaxWidth)
								MaxWidth = Len;

							MaxHeight++;
						}

						MaxHeight++;

						LT_New(Handle,
							LA_Type,		LISTVIEW_KIND,
							LA_LabelID,		MSG_V36_0160,
							GTLV_Labels,	ModeList,
							LA_LONG,		&Index,
							LALV_CursorKey,	TRUE,
							LALV_Link,		NIL_LINK,
							LALV_MaxGrowY,	MaxHeight,
							LALV_ResizeY,	TRUE,
							LALV_Lines,		MaxHeight > 10 ? 10 : MaxHeight,
							LA_Chars,		MaxWidth,
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

				if(LocalWindow = LT_Build(Handle,
					LAWN_TitleID,		MSG_V36_0161,
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
					struct ModeNode *ModeNode;
					struct Gadget *MsgGadget;
					ULONG MsgClass;
					BOOL Done;
					LONG i;

					Done = FALSE;

					do
					{
						WaitPort(LocalWindow->UserPort);

						while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
						{
							MsgClass	= Message->Class;
							MsgGadget	= (struct Gadget *)Message->IAddress;

							LT_ReplyIMsg(Message);

							if(MsgClass == IDCMP_GADGETUP)
							{
								switch(MsgGadget->GadgetID)
								{
									case GAD_USE:

										for(ModeNode = (struct ModeNode *)ModeList->lh_Head, i = 0; ModeNode->Node.ln_Succ ; ModeNode = (struct ModeNode *)ModeNode->Node.ln_Succ, i++)
										{
											if(Index == i)
											{
												*CurrentMode = ModeNode->DisplayID;

												break;
											}
										}

										Success = Done = TRUE;
										break;

									case GAD_CANCEL:

										Done = TRUE;
										break;
								}
							}

							if(MsgClass == IDCMP_IDCMPUPDATE)
							{
								for(ModeNode = (struct ModeNode *)ModeList->lh_Head, i = 0; ModeNode->Node.ln_Succ ; ModeNode = (struct ModeNode *)ModeNode->Node.ln_Succ, i++)
								{
									if(Index == i)
									{
										*CurrentMode = ModeNode->DisplayID;

										Success = Done = TRUE;

										LT_PressButton(Handle,GAD_USE);

										break;
									}
								}
							}
						}
					}
					while(!Done);

					if(Success && Config)
					{
						Config->ScreenConfig->Depth			= 0;
						Config->ScreenConfig->OverscanType	= OSCAN_TEXT;
						Config->ScreenConfig->DisplayWidth	= 0;
						Config->ScreenConfig->DisplayHeight	= 0;
					}
				}

				LT_DeleteHandle(Handle);
			}

			DeleteList(ModeList);
		}
	}
	else
	{
		LONG DisplayWidth,DisplayHeight,Depth = 0,OverscanType = 0;
		struct ScreenModeRequester *Request;
		struct TagItem DimensionTags[5];
		struct Rectangle DisplayClip;

		if(Config)
		{
			Depth = Config->ScreenConfig->Depth;

			if(Depth < MinDepth)
				Depth = MinDepth;

			OverscanType = Config->ScreenConfig->OverscanType;

			if((!Config->ScreenConfig->DisplayWidth || !Config->ScreenConfig->DisplayHeight) && QueryOverscan(Config->ScreenConfig->DisplayMode,&DisplayClip,OverscanType))
			{
				DisplayWidth	= DisplayClip.MaxX - DisplayClip.MinX + 1;
				DisplayHeight	= DisplayClip.MaxY - DisplayClip.MinY + 1;
			}
			else
			{
				DisplayWidth	= Config->ScreenConfig->DisplayWidth;
				DisplayHeight	= Config->ScreenConfig->DisplayHeight;
			}
		}
		else
			DisplayWidth = DisplayHeight = 0;

		if(Request = (struct ScreenModeRequester *)AllocAslRequestTags(ASL_ScreenModeRequest,
			ASLSM_Window,				Parent,
			ASLSM_InitialDisplayID,		*CurrentMode,
			ASLSM_PrivateIDCMP,			TRUE,

			Config ? ASLSM_InitialDisplayDepth			: TAG_IGNORE,	Depth,
			Config ? ASLSM_MinDepth						: TAG_IGNORE,	MinDepth,
			Config ? ASLSM_MaxDepth						: TAG_IGNORE,	8,
			Config ? ASLSM_DoDepth						: TAG_IGNORE,	TRUE,
			Config ? ASLSM_DoWidth						: TAG_IGNORE,	TRUE,
			Config ? ASLSM_DoHeight						: TAG_IGNORE,	TRUE,
			Config ? ASLSM_DoOverscanType				: TAG_IGNORE,	TRUE,
			Config ? ASLSM_InitialOverscanType			: TAG_IGNORE,	OverscanType,

			DisplayWidth  ? ASLSM_InitialDisplayWidth	: TAG_IGNORE,	DisplayWidth,
			DisplayHeight ? ASLSM_InitialDisplayHeight	: TAG_IGNORE,	DisplayHeight,
		TAG_MORE,GetDimensionTags(Parent,DimensionTags)))
		{
			if(AslRequest(Request,NULL))
			{
				PutDimensionTags(Parent,Request->sm_LeftEdge,Request->sm_TopEdge,Request->sm_Width,Request->sm_Height);

				*CurrentMode = Request->sm_DisplayID;

				if(Config)
				{
					if(Request->sm_DisplayDepth == MinDepth)
						Depth = 0;
					else
						Depth = Request->sm_DisplayDepth;

					Config->ScreenConfig->Depth			= Depth;
					Config->ScreenConfig->OverscanType	= Request->sm_OverscanType;
					Config->ScreenConfig->DisplayWidth	= Request->sm_DisplayWidth;
					Config->ScreenConfig->DisplayHeight	= Request->sm_DisplayHeight;
				}

				Success = TRUE;
			}

			FreeAslRequest(Request);
		}
	}

	return(Success);
}
