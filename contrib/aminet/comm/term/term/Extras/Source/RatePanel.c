/*
**	RatePanel.c
**
**	Editing panel for phone units configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_TIME=1,GAD_ADDTIME,GAD_REMOVETIME,GAD_EDITTIME,
			GAD_PAY1,GAD_MINUTES1,
			GAD_PAY2,GAD_MINUTES2,
			GAD_LIST,
			GAD_ADDDATE,GAD_ADDDAY,GAD_EDIT,
			GAD_CLONE,GAD_REMOVE,GAD_IMPORT
		};

struct DoubleInfo
{
	LONG	GlyphWidth,
			HeaderLen;
};

STATIC VOID
Rate2String(ULONG Rate,STRPTR String,LONG StringSize)
{
	if(Rate % 10000)
	{
		LONG i,Len;

		LimitedSPrintf(StringSize,String,"%ld%s%04ld",Rate / 10000,DecimalPoint,Rate % 10000);

		Len = strlen(String);

		for(i = Len - 1 ; i >= Len - 3 ; i--)
		{
			if(String[i] == '0')
				String[i] = 0;
			else
				break;
		}
	}
	else
		LimitedSPrintf(StringSize,String,"%ld",Rate / 10000);
}

STATIC ULONG
String2Rate(STRPTR String)
{
	ULONG Lead;
	ULONG Follow;
	ULONG Factor;
	BOOL Decimal;
	LONG i;

	Lead = 0;
	Follow = 0;
	Factor = 1000;
	Decimal = FALSE;

	for(i = 0 ; i < strlen(String) ; i++)
	{
		if(String[i] == DecimalPoint[0])
			Decimal = TRUE;
		else
		{
			ULONG v = String[i] & 0xf;

			if(Decimal)
			{
				Follow += v * Factor;

				Factor /= 10;
			}
			else
				Lead = (10 * Lead) + v;
		}
	}

	return(Lead * 10000 + Follow);
}

#ifndef __AROS__
STATIC ULONG SAVE_DS ASM
RateEditRoutine(REG(a0) struct Hook *UnusedHook,REG(a2) struct SGWork *Work,REG(a1) Msg msg)
#else
AROS_UFH3(STATIC ULONG, RateEditRoutine,
 AROS_UFHA(struct Hook *  , UnusedHook, A0),
 AROS_UFHA(struct SGWork *, Work, A2),
 AROS_UFHA(Msg            , msg, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	switch(msg->MethodID)
	{
		case SGH_KEY:

			if(Work->EditOp == EO_INSERTCHAR || Work->EditOp == EO_REPLACECHAR)
			{
				ULONG Integral;
				BOOL Decimal;
				WORD Follow;
				BOOL UseIt;
				UBYTE c;
				LONG i;

				Integral	= 0;
				Decimal		= FALSE;
				Follow		= 0;
				UseIt		= TRUE;

				for(i = 0 ; i < strlen(Work->WorkBuffer) ; i++)
				{
					c = Work->WorkBuffer[i];

					if(c == '.')
						c = Work->WorkBuffer[i] = DecimalPoint[0];

					if(c == DecimalPoint[0])
					{
						if(Decimal)
						{
							UseIt = FALSE;
							break;
						}
						else
							Decimal = TRUE;
					}
					else
					{
						if(c >= '0' && c <= '9')
						{
							if(Decimal)
								Follow++;
							else
								Integral = (Integral * 10) + (c & 0xf);
						}
						else
						{
							UseIt = FALSE;
							break;
						}
					}
				}

				if(Follow > 4 || !UseIt || Integral > 429496)
				{
					Work->EditOp	= EO_BADFORMAT;
					Work->Actions	= SGA_BEEP;
				}
			}

		/* Falls through to... */

		case SGH_CLICK:

			return(TRUE);

		default:

			return(FALSE);
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

	/* RateListViewRender():
	 *
	 *	This callback routine is responsible for rendering
	 *	the single listview entries.
	 */

#ifndef __AROS__
STATIC ULONG SAVE_DS ASM
RateListViewRender(REG(a0) struct Hook *Hook,REG(a2) struct Node *Node,REG(a1) struct LVDrawMsg *Msg)
#else
AROS_UFH3(STATIC ULONG, RateListViewRender,
 AROS_UFHA(struct Hook *     , Hook, A0),
 AROS_UFHA(struct Node *     , Node, A2),
 AROS_UFHA(struct LVDrawMsg *, Msg , A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
		/* We only know how to redraw lines. */

	if(Msg->lvdm_MethodID == LV_DRAW)
	{
		struct RastPort		*RPort	= Msg->lvdm_RastPort;
		LONG				 Left	= Msg->lvdm_Bounds.MinX,
							 Top	= Msg->lvdm_Bounds.MinY,
							 Width	= Msg->lvdm_Bounds.MaxX - Msg->lvdm_Bounds.MinX + 1,
							 Height	= Msg->lvdm_Bounds.MaxY - Msg->lvdm_Bounds.MinY + 1,
							 Delta,
							 FgPen,BgPen,
							 Len,LabelLen,Template;
		UBYTE				*String,Header[8];
		struct DoubleInfo	*Info;
		UWORD				*Pens;

		Pens = Msg->lvdm_DrawInfo->dri_Pens;

		Info = Hook->h_Data;

		String = &Node->ln_Name[Info->HeaderLen];

		CopyMem(Node->ln_Name,Header,Info->HeaderLen);

		Header[Info->HeaderLen] = 0;

			/* Determine the rendering pens. */

		if(Msg->lvdm_State == LVR_SELECTED)
		{
			FgPen = FILLTEXTPEN;
			BgPen = FILLPEN;
		}
		else
		{
			FgPen = TEXTPEN;
			BgPen = BACKGROUNDPEN;
		}

		FgPen = Pens[FgPen];
		BgPen = Pens[BgPen];

			/* Set the rendering pens. */

		SetABPenDrMd(RPort,BgPen,BgPen,JAM2);

		RectFill(RPort,Left,Top,Left + 1,Top + Height - 1);

		Left	+= 2;
		Width	-= 2;

		Template = Info->HeaderLen * Info->GlyphWidth;

			/* Determine header length. */

		Len = TextLength(RPort,Header,Info->HeaderLen);

			/* Fill the space to precede the header. */

		if((Delta = Template - Len) > 0)
		{
			SetAPen(RPort,BgPen);
			FillBox(RPort,Left,Top,Delta,Height);
		}

			/* Render the header, right-justified. */

		SetAPen(RPort,FgPen);

		PlaceText(RPort,Left + Delta,Top,Header,Info->HeaderLen);

			/* Adjust width and area left edge. */

		Left	+= Template;
		Width	-= Template;

			/* Determine length of vanilla name. */

		LabelLen = strlen(String);

			/* Try to make it fit. */

		LabelLen = FitText(RPort,Width,String,LabelLen);

			/* Print the vanilla name if possible. */

		if(LabelLen)
		{
			Len = TextLength(RPort,String,LabelLen);

			PlaceText(RPort,Left,Top,String,LabelLen);

			Left	+= Len;
			Width	-= Len;
		}

			/* Fill the area to follow the vanilla name. */

		if(Width > 0)
		{
			SetAPen(RPort,BgPen);
			FillBox(RPort,Left,Top,Width,Height);
		}

			/* If the item happens to be disabled, draw the cross-hatch
			 * pattern across it.
			 */

		ListViewStateFill(Msg);

		return(LVCB_OK);
	}
	else
		return(LVCB_UNKNOWN);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

STATIC VOID
ChangeLocalState(LayoutHandle *Handle,struct List *TimeDateList,struct TimeDateNode *SelectedTimeDateNode,struct List *TimeList,LONG SelectedTime)
{
	BOOL IsDefaultEntry,IsInvalid,HasNoData,IsEmpty,IsSingle;
	Tag StringTag;

	if(IsListEmpty(TimeDateList))
		IsEmpty = IsDefaultEntry = IsInvalid = IsSingle = HasNoData = TRUE;
	else
	{
		IsDefaultEntry	= (SelectedTimeDateNode == NULL) || (BOOL)(TimeDateList->lh_Head == (struct Node *)SelectedTimeDateNode);
		IsEmpty			= FALSE;
		IsInvalid		= FALSE;
		HasNoData		= (BOOL)(SelectedTime < 0);

		if(TimeList)
			IsSingle = (BOOL)(TimeList->lh_Head == TimeList->lh_TailPred);
		else
			IsSingle = TRUE;
	}

	StringTag = (IsInvalid || HasNoData) ? GTST_String : TAG_IGNORE;

	LT_SetAttributes(Handle,GAD_PAY1,
		GA_Disabled,	IsInvalid || HasNoData,
		StringTag,		"",
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_MINUTES1,
		GA_Disabled,	IsInvalid || HasNoData,
		StringTag,		"",
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_PAY2,
		GA_Disabled,	IsInvalid || HasNoData,
		StringTag,		"",
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_MINUTES2,
		GA_Disabled,	IsInvalid || HasNoData,
		StringTag,		"",
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_EDIT,
		GA_Disabled,	HasNoData || IsDefaultEntry,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_CLONE,
		GA_Disabled,	HasNoData || IsDefaultEntry,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_REMOVE,
		GA_Disabled,	HasNoData || IsDefaultEntry,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_LIST,
		GA_Disabled,	IsEmpty,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_TIME,
		GA_Disabled,	HasNoData || IsInvalid,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_ADDTIME,
		GA_Disabled,	IsInvalid,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_REMOVETIME,
		GA_Disabled,	HasNoData || IsInvalid || IsSingle,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_EDITTIME,
		GA_Disabled,	HasNoData || IsInvalid,
	TAG_DONE);
}

STATIC VOID
ShowSelectedTime(LayoutHandle *Handle,struct TimeDateNode *SelectedTimeDateNode,LONG SelectedTime)
{
	UBYTE String[20];

	Rate2String(SelectedTimeDateNode->Table[SelectedTime].PayPerUnit[DT_FIRST_UNIT],String,sizeof(String));

	LT_SetAttributes(Handle,GAD_PAY1,
		GTST_String,	String,
	TAG_DONE);

	Rate2String(SelectedTimeDateNode->Table[SelectedTime].SecPerUnit[DT_FIRST_UNIT],String,sizeof(String));
	LT_SetAttributes(Handle,GAD_MINUTES1,
		GTST_String,	String,
	TAG_DONE);

	Rate2String(SelectedTimeDateNode->Table[SelectedTime].PayPerUnit[DT_NEXT_UNIT],String,sizeof(String));

	LT_SetAttributes(Handle,GAD_PAY2,
		GTST_String,	String,
	TAG_DONE);

	Rate2String(SelectedTimeDateNode->Table[SelectedTime].SecPerUnit[DT_NEXT_UNIT],String,sizeof(String));

	LT_SetAttributes(Handle,GAD_MINUTES2,
		GTST_String,	String,
	TAG_DONE);
}

BOOL
RatePanel(struct Window *Parent,PhonebookHandle *PhoneHandle,PhoneEntry *Entry,struct List *TimeDateList)
{
	STATIC struct DoubleInfo TimeInfo;
	STATIC struct DoubleInfo DateInfo;

	STATIC struct Hook TimeHook;
	STATIC struct Hook DateHook;
	STATIC struct Hook RateHook;

	struct LayoutHandle	*Handle;
	BOOL				 MadeChanges = FALSE;

	InitHook(&TimeHook,(HOOKFUNC)RateListViewRender,&TimeInfo);
	InitHook(&DateHook,(HOOKFUNC)RateListViewRender,&DateInfo);
	InitHook(&RateHook,(HOOKFUNC)RateEditRoutine,NULL);

	if(!TimeDateList)
		TimeDateList = (struct List *)&Entry->TimeDateList;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		STATIC WORD ButtonLabels[] =
		{
			MSG_RATEPANEL_ADD_TIME_GAD,
			MSG_RATEPANEL_REMOVE_TIME_GAD,
			MSG_RATEPANEL_EDIT_TIME_GAD,

			MSG_RATEPANEL_ADD_DATE_GAD,
			MSG_RATEPANEL_ADD_DAYS_GAD,
			MSG_RATEPANEL_EDIT_GAD,

			MSG_RATEPANEL_CLONE_GAD,
			MSG_GLOBAL_REMOVE_GAD,
			MSG_RATEPANEL_IMPORT_GAD
		};

		struct Window	*PanelWindow;
		LONG			 i,Max = 0,Size;
		UBYTE			 SmallString[2];
		UBYTE			 Currency[20];

		SmallCurrency(Currency,sizeof(Currency));

		SmallString[1] = 0;

		for(i = '0' ; i <= '9' ; i++)
		{
			SmallString[0] = i;

			if((Size = LT_LabelWidth(Handle,SmallString)) > Max)
				Max = Size;
		}

		TimeInfo.GlyphWidth	= Max;
		TimeInfo.HeaderLen	= 2;

		DateInfo.GlyphWidth	= Max;
		DateInfo.HeaderLen	= 7;

		for(i = Max = 0 ; i < NUM_ELEMENTS(ButtonLabels) ; i++)
		{
			if((Size = LT_LabelChars(Handle,LocaleString(ButtonLabels[i]))) > Max)
				Max = Size;
		}

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	HORIZONTAL_KIND,
				LA_LabelID,	MSG_V36_0157,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		LISTVIEW_KIND,
						LA_LabelID,		MSG_RATEPANEL_DAYS_AND_DATES_GAD,
						LA_Chars,		30,
						LA_ID,			GAD_LIST,
						LALV_Lines,		10,
						GTLV_CallBack,	&DateHook,
						GTLV_MaxPen,	GetListMaxPen(Handle->DrawInfo->dri_Pens),
						GTLV_Labels,	TimeDateList,
						LALV_Link,		NIL_LINK,

						Kick30 ? TAG_IGNORE : LALV_TextAttr, ~0,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		LISTVIEW_KIND,
						LA_LabelID,		MSG_RATEPANEL_TIME_GAD,
						LA_Chars,		10,
						LA_ID,			GAD_TIME,
						GA_Disabled,	TRUE,
						LALV_Lines,		10,
						GTLV_CallBack,	&TimeHook,
						GTLV_MaxPen,	GetListMaxPen(Handle->DrawInfo->dri_Pens),
						LALV_Link,		NIL_LINK,

						Kick30 ? TAG_IGNORE : LALV_TextAttr, ~0,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		VERTICAL_KIND,
						LA_LabelID,		MSG_RATEPANEL_FIRST_UNIT_GAD,
						LAGR_SameSize,	TRUE,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			STRING_KIND,
							LA_LabelText,		Currency,
							LA_ID,				GAD_PAY1,
							LA_Chars,			6,
							GTST_MaxChars,		11,
							GTST_EditHook,		&RateHook,
							GA_Disabled,		TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			STRING_KIND,
							LA_LabelID,			MSG_RATEPANEL_SECONDS_PER_UNIT_GAD,
							LA_ID,				GAD_MINUTES1,
							GTST_MaxChars,		11,
							GTST_EditHook,		&RateHook,
							GA_Disabled,		TRUE,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,		VERTICAL_KIND,
						LA_LabelID,		MSG_RATEPANEL_FOLLOWING_UNITS_GAD,
						LAGR_SameSize,	TRUE,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			STRING_KIND,
							LA_LabelText,		Currency,
							LA_ID,				GAD_PAY2,
							GA_Disabled,		TRUE,
							GTST_MaxChars,		11,
							GTST_EditHook,		&RateHook,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			STRING_KIND,
							LA_LabelID,			MSG_RATEPANEL_SECONDS_PER_UNIT_GAD,
							LA_ID,				GAD_MINUTES2,
							LA_Chars,			6,
							GTST_MaxChars,		11,
							GTST_EditHook,		&RateHook,
							GA_Disabled,		TRUE,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullSize,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,LA_Type,HORIZONTAL_KIND,
				LAGR_Spread,	TRUE,
				LAGR_SameSize,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_RATEPANEL_ADD_TIME_GAD,
					LA_ID,			GAD_ADDTIME,
					LA_Chars,		Max,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_RATEPANEL_REMOVE_TIME_GAD,
					LA_ID,			GAD_REMOVETIME,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_RATEPANEL_EDIT_TIME_GAD,
					LA_ID,			GAD_EDITTIME,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,LA_Type,HORIZONTAL_KIND,
				LAGR_Spread,	TRUE,
				LAGR_SameSize,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_RATEPANEL_ADD_DATE_GAD,
					LA_ID,			GAD_ADDDATE,
					LA_Chars,		Max,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_RATEPANEL_ADD_DAYS_GAD,
					LA_ID,			GAD_ADDDAY,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_RATEPANEL_EDIT_GAD,
					LA_ID,			GAD_EDIT,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,LA_Type,HORIZONTAL_KIND,
				LAGR_Spread,	TRUE,
				LAGR_SameSize,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_RATEPANEL_CLONE_GAD,
					LA_ID,			GAD_CLONE,
					LA_Chars,		Max,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_REMOVE_GAD,
					LA_ID,			GAD_REMOVE,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_RATEPANEL_IMPORT_GAD,
					LA_ID,			GAD_IMPORT,
					GA_Disabled,	GlobalPhoneHandle->NumPhoneEntries == 0,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_RATEPANEL_RATES_PREFERENCES_TXT,
			LAWN_IDCMP,			IDCMP_CLOSEWINDOW,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_CloseGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage	*Message;
			BOOL				 Done = FALSE;
			ULONG				 MsgClass;
			UWORD				 MsgCode;
			struct Gadget		*MsgGadget;
			LONG				 MsgGadgetID;

			BOOL				 Redisplay;

			struct TimeDateNode	*SelectedTimeDateNode;
			LONG				 SelectedTime;

			LONG				 TimeDateListSize;
			struct List			*TimeList;

			struct TimeDateNode	*NewNode;

			GuideContext(CONTEXT_RATES);

			TimeDateListSize = GetListSize(TimeDateList);

			TimeList = NULL;

			SelectedTimeDateNode = NULL;
			SelectedTime = -1;

			Redisplay = FALSE;

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

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

					if(MsgClass == IDCMP_CLOSEWINDOW)
						Done = TRUE;

					MsgGadgetID = -1;

					if(MsgClass == IDCMP_GADGETUP)
						MsgGadgetID = MsgGadget->GadgetID;

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_LIST:

								if(SelectedTimeDateNode != NULL && TimeDateList->lh_Head != (struct Node *)SelectedTimeDateNode)
									MsgGadgetID = GAD_EDIT;

								break;

							case GAD_TIME:

								if(SelectedTimeDateNode != NULL && SelectedTime != -1)
									MsgGadgetID = GAD_EDITTIME;

								break;
						}

						if(MsgGadgetID != -1)
							LT_PressButton(Handle,MsgGadgetID);
					}

					if(MsgGadgetID != -1)
					{
						switch(MsgGadgetID)
						{
							case GAD_TIME:

								if(SelectedTimeDateNode != NULL && TimeList != NULL)
								{
									SelectedTime = MsgCode;

									ShowSelectedTime(Handle,SelectedTimeDateNode,SelectedTime);

									ChangeLocalState(Handle,TimeDateList,SelectedTimeDateNode,TimeList,SelectedTime);
								}

								break;

							case GAD_EDITTIME:

								if(SelectedTimeDateNode != NULL && SelectedTime != -1)
								{
									LT_LockWindow(PanelWindow);

										/* Edit the time */

									if(TimePanel(PanelWindow,&SelectedTimeDateNode->Table[SelectedTime].Time))
									{
											/* Make a mark so we can find it later */

										SelectedTimeDateNode->Table[SelectedTime].Mark = TRUE;

											/* Sort the times in ascending order */

										SortTimeTable(SelectedTimeDateNode);

											/* Find the previously selected item */

										for(i = 0 ; i < SelectedTimeDateNode->Table[0].Count ; i++)
										{
											if(SelectedTimeDateNode->Table[i].Mark)
											{
													/* Remove the mark */

												SelectedTimeDateNode->Table[i].Mark = FALSE;

												SelectedTime = i;
												break;
											}
										}

										Redisplay = TRUE;
										MadeChanges = TRUE;
									}

									LT_UnlockWindow(PanelWindow);

									LT_ShowWindow(Handle,TRUE);
								}

								break;

							case GAD_REMOVETIME:

								if(SelectedTimeDateNode != NULL && SelectedTime != -1)
								{
									LT_LockWindow(PanelWindow);

									LT_SetAttributes(Handle,GAD_TIME,
										GTLV_Labels,	~0,
									TAG_DONE);

									if(DeleteTimeDateNode(SelectedTimeDateNode,SelectedTime))
									{
										if(SelectedTime == SelectedTimeDateNode->Table[0].Count)
											SelectedTime--;

										Redisplay = TRUE;
										MadeChanges = TRUE;
									}
									else
									{
										LT_SetAttributes(Handle,GAD_TIME,
											GTLV_Labels,	TimeList,
										TAG_DONE);

										DisplayBeep(PanelWindow->WScreen);
									}

									LT_UnlockWindow(PanelWindow);
								}

								break;

							case GAD_ADDTIME:

								if(SelectedTimeDateNode != NULL)
								{
									UBYTE Time = DT_GET_TIME(12,0);

									LT_LockWindow(PanelWindow);

										/* Get the new time */

									if(TimePanel(PanelWindow,&Time))
									{
										BOOL AddIt = TRUE;

											/* Check if this moment already exists */

										for(i = 0 ; i < SelectedTimeDateNode->Table[0].Count ; i++)
										{
											if(Time == SelectedTimeDateNode->Table[i].Time)
											{
												AddIt = FALSE;
												break;
											}
										}

											/* Don't add it if it's already in there */

										if(AddIt)
										{
											LT_SetAttributes(Handle,GAD_TIME,
												GTLV_Labels,	~0,
											TAG_DONE);

												/* Make a mark so we can find this entry later */

											SelectedTimeDateNode->Table[SelectedTime].Mark = TRUE;

												/* Add one new entry */

											if(ResizeTimeDateNode(SelectedTimeDateNode,SelectedTimeDateNode->Table[0].Count + 1,Time))
											{
													/* Restore the proper order */

												SortTimeTable(SelectedTimeDateNode);

													/* Find the originally selected item */

												for(i = 0 ; i < SelectedTimeDateNode->Table[0].Count ; i++)
												{
													if(SelectedTimeDateNode->Table[i].Mark)
													{
															/* Remove the mark. */

														SelectedTimeDateNode->Table[i].Mark = FALSE;

														SelectedTime = i;
														break;
													}
												}

												MadeChanges = TRUE;
												Redisplay = TRUE;
											}
											else
											{
													/* Remove the mark. */

												SelectedTimeDateNode->Table[SelectedTime].Mark = FALSE;

													/* Restore the list */

												LT_SetAttributes(Handle,GAD_TIME,
													GTLV_Labels,	TimeList,
												TAG_DONE);

												DisplayBeep(PanelWindow->WScreen);
											}
										}
									}

									LT_UnlockWindow(PanelWindow);

									LT_ShowWindow(Handle,TRUE);
								}

								break;

							case GAD_PAY1:

								SelectedTimeDateNode->Table[SelectedTime].PayPerUnit[DT_FIRST_UNIT] = String2Rate(LT_GetString(Handle,MsgGadgetID));
								MadeChanges = TRUE;

								break;

							case GAD_PAY2:

								SelectedTimeDateNode->Table[SelectedTime].PayPerUnit[DT_NEXT_UNIT] = String2Rate(LT_GetString(Handle,MsgGadgetID));
								MadeChanges = TRUE;

								break;

							case GAD_MINUTES1:

								SelectedTimeDateNode->Table[SelectedTime].SecPerUnit[DT_FIRST_UNIT] = String2Rate(LT_GetString(Handle,MsgGadgetID));
								MadeChanges = TRUE;

								break;

							case GAD_MINUTES2:

								SelectedTimeDateNode->Table[SelectedTime].SecPerUnit[DT_NEXT_UNIT] = String2Rate(LT_GetString(Handle,MsgGadgetID));
								MadeChanges = TRUE;

								break;

							case GAD_LIST:

								SelectedTimeDateNode = (struct TimeDateNode *)GetListNode(MsgCode,(struct List *)TimeDateList);
								SelectedTime = 0;

								Redisplay = TRUE;

								break;

							case GAD_IMPORT:

								LT_LockWindow(PanelWindow);

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								if(ImportPanel(PanelWindow,PhoneHandle,Entry,TimeDateList))
								{
									if(SelectedTimeDateNode)
									{
										LONG Offset;

										Offset = GetNodeOffset((struct Node *)SelectedTimeDateNode,TimeDateList);

										if(Offset < 0)
											SelectedTimeDateNode = NULL;
										else
										{
											LT_SetAttributes(Handle,GAD_LIST,
												GTLV_Labels,	TimeDateList,
												LALV_Selected,	Offset,
											TAG_DONE);

											Redisplay = TRUE;
										}
									}

									if(SelectedTimeDateNode == NULL)
									{
										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,	TimeDateList,
											LALV_Selected,	~0,
										TAG_DONE);

										SelectedTimeDateNode = NULL;

										LT_SetAttributes(Handle,GAD_TIME,
											GTLV_Labels,	TimeList,
											LALV_Selected,	~0,
										TAG_DONE);

										SelectedTime = -1;
									}

									MadeChanges = TRUE;
								}
								else
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	TimeDateList,
									TAG_DONE);
								}

								if(!Redisplay)
									ChangeLocalState(Handle,TimeDateList,SelectedTimeDateNode,TimeList,SelectedTime);

								LT_UnlockWindow(PanelWindow);

								LT_ShowWindow(Handle,TRUE);

								break;

							case GAD_ADDDAY:
							case GAD_ADDDATE:

								LT_LockWindow(PanelWindow);

								if(NewNode = CreateTimeDateNode(0,1,"",1))
								{
									BOOL Result;

									if(MsgGadgetID == GAD_ADDDAY)
									{
										NewNode->Header.Month	= -1;
										NewNode->Header.Day		= 0;

										Result = DayPanel(PanelWindow,NewNode);
									}
									else
										Result = DatePanel(PanelWindow,NewNode);

									if(Result)
									{
										MadeChanges = TRUE;

										AdaptTimeDateNode(NewNode);

										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,	~0,
										TAG_DONE);

										AddTail((struct List *)TimeDateList,&NewNode->Node);

										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,		TimeDateList,
											LALV_Selected,		TimeDateListSize++,
										TAG_DONE);

										SelectedTimeDateNode = NewNode;
										SelectedTime = -1;

										Redisplay = TRUE;
									}
									else
										FreeVecPooled(NewNode);
								}

								if(!Redisplay)
									ChangeLocalState(Handle,TimeDateList,SelectedTimeDateNode,TimeList,SelectedTime);

								LT_UnlockWindow(PanelWindow);

								LT_ShowWindow(Handle,TRUE);

								break;

							case GAD_REMOVE:

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								NewNode = (struct TimeDateNode *)RemoveGetNext((struct Node *)SelectedTimeDateNode);
								FreeVecPooled(SelectedTimeDateNode);

								MadeChanges = TRUE;

								SelectedTimeDateNode = NewNode;
								TimeDateListSize--;

								SelectedTime = -1;

								if(NewNode)
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	TimeDateList,
										LALV_Selected,	GetNodeOffset((struct Node *)NewNode,TimeDateList),
									TAG_DONE);

									Redisplay = TRUE;
								}
								else
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	TimeDateList,
										LALV_Selected,	~0,
									TAG_DONE);

									ChangeLocalState(Handle,TimeDateList,SelectedTimeDateNode,TimeList,SelectedTime);
								}

								break;

							case GAD_CLONE:

								if(NewNode = CreateTimeDateNode(0,1,"",SelectedTimeDateNode->Table[0].Count))
								{
									MadeChanges = TRUE;

									strcpy(NewNode->Node.ln_Name = NewNode->Buffer,SelectedTimeDateNode->Buffer);

									NewNode->Header = SelectedTimeDateNode->Header;

									CopyMem(SelectedTimeDateNode->Table,NewNode->Table,sizeof(struct TimeDate) * SelectedTimeDateNode->Table[0].Count);

									AdaptTimeDateNode(NewNode);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									AddTail((struct List *)TimeDateList,(struct Node *)NewNode);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	TimeDateList,
										LALV_Selected,	TimeDateListSize++,
									TAG_DONE);

									SelectedTimeDateNode = NewNode;

									ChangeLocalState(Handle,TimeDateList,SelectedTimeDateNode,TimeList,SelectedTime);
								}

								break;

							case GAD_EDIT:

								LT_LockWindow(PanelWindow);

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								if(SelectedTimeDateNode->Header.Month == -1)
									MadeChanges |= DayPanel(PanelWindow,SelectedTimeDateNode);
								else
									MadeChanges |= DatePanel(PanelWindow,SelectedTimeDateNode);

								AdaptTimeDateNode(SelectedTimeDateNode);

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	TimeDateList,
								TAG_DONE);

								LT_UnlockWindow(PanelWindow);

								LT_ShowWindow(Handle,TRUE);

								break;
						}
					}

					if(Redisplay)
					{
						Redisplay = FALSE;

						if(SelectedTimeDateNode)
						{
							LT_SetAttributes(Handle,GAD_TIME,
								GTLV_Labels,~0,
							TAG_DONE);

							FreeVecPooled(TimeList);

							if(TimeList = BuildTimeList(SelectedTimeDateNode))
							{
								if(SelectedTime < 0)
									SelectedTime = 0;

								LT_SetAttributes(Handle,GAD_TIME,
									GTLV_Labels,	TimeList,
									LALV_Selected,	SelectedTime,
								TAG_DONE);

								ShowSelectedTime(Handle,SelectedTimeDateNode,SelectedTime);
							}
							else
							{
								LT_SetAttributes(Handle,GAD_LIST,
									LALV_Selected,	~0,
								TAG_DONE);

								SelectedTimeDateNode = NULL;

								LT_SetAttributes(Handle,GAD_TIME,
									GTLV_Labels,	NULL,
									LALV_Selected,	~0,
								TAG_DONE);

								SelectedTime = -1;

								DisplayBeep(PanelWindow->WScreen);
							}
						}

						ChangeLocalState(Handle,TimeDateList,SelectedTimeDateNode,TimeList,SelectedTime);
					}
				}
			}
			while(!Done);

			PopWindow();

			FreeVecPooled(TimeList);
		}

		LT_DeleteHandle(Handle);
	}

	return(MadeChanges);
}
