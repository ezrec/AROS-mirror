#include "Global.h"
#include "protos.h"
#include "GTPopUp_text.h"

enum {	BUTTON_NEWMESSAGE, BUTTON_DELMESSAGE, BUTTON_FIRSTMESSAGE, BUTTON_PREVMESSAGE, BUTTON_NEXTMESSAGE, BUTTON_LASTMESSAGE, BOX_OVERVIEW, BOX_FROM, RADIO_TOWHOM, 
		BUTTON_SENDMESSAGE, BUTTON_CANCEL_MESSAGE, STRING_MESSAGE_LABEL , LISTVIEW_MESSAGE};

struct Library *GTLayoutBase;
struct Library *GadToolsBase;

struct Hook PopHook;
char UpdateString[80];
struct List *MessageList;

void main(void)		//GTPopUpGUI()
{
	struct LayoutHandle *Handle;

	if(OpenLibs())
	{
		InitHook(&PopHook,(HOOKFUNC)LocaleHookFunc,NULL);

		if(MessageList = (struct List*)AllocVec(sizeof(struct List), MEMF_ANY|MEMF_CLEAR))
		{
			NewList(MessageList);
		}

		if(Handle = LT_CreateHandleTags(NULL,
			LAHN_AutoActivate, FALSE,
			LAHN_LocaleHook,	&PopHook,
			TAG_DONE))
		{
			struct Window *MessageWindow; 
			
			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
					LAGR_SameSize,	TRUE,
					TAG_DONE);
				{

					LT_New(Handle,
						LA_Type,		HORIZONTAL_KIND,
//						LAGR_SameSize,	TRUE,
						TAG_DONE);
					{

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		BUTTON_NEWMESSAGE_TEXT,
							LA_ID,			BUTTON_NEWMESSAGE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		BUTTON_KIND,
							LA_LabelID,		BUTTON_DELMESSAGE_TEXT,
							LA_ID,			BUTTON_DELMESSAGE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		END_KIND,
						TAG_DONE);
					}

					LT_New(Handle,
						LA_Type,		HORIZONTAL_KIND,
						TAG_DONE);
					{

						LT_New(Handle,
							LA_Type,			TAPEDECK_KIND,
							LATD_ButtonType,	TDBT_Previous,
							LA_ID,				BUTTON_FIRSTMESSAGE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			TAPEDECK_KIND,
							LATD_ButtonType,	TDBT_Rewind,
							LA_ID,				BUTTON_PREVMESSAGE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			TAPEDECK_KIND,
							LATD_ButtonType,	TDBT_Play,
							LA_ID,				BUTTON_NEXTMESSAGE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			TAPEDECK_KIND,
							LATD_ButtonType,	TDBT_Next,
							LA_ID,				BUTTON_LASTMESSAGE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		END_KIND,
						TAG_DONE);
					}

					LT_New(Handle,
						LA_Type,		END_KIND,
					TAG_DONE);
				}

				LT_New(Handle,
					LA_Type,		BOX_KIND,
					LABX_DrawBox,	TRUE,
					LA_ID,			BOX_FROM,
					LABX_LineID,	BOX_FROM_TEXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		LISTVIEW_KIND,
					LA_ID,			LISTVIEW_MESSAGE,
					GTLV_Labels,	MessageList,
					LALV_ResizeY,	TRUE,
					LALV_Lines,		8,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BOX_KIND,
					LA_ID,			BOX_OVERVIEW,
					LABX_DrawBox,	TRUE,
					LABX_AlignText,	ALIGNTEXT_Centered,
					LABX_LineID,	BOX_OVERVIEW_TEXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		END_KIND,
				TAG_DONE);
			}

			if(MessageWindow = LT_Build(Handle,
						LAWN_TitleID,	TITLE_GTPOPUP,
						LAWN_IDCMP,		IDCMP_CLOSEWINDOW,
						WA_DragBar,		TRUE,
						WA_DepthGadget,	TRUE,
						WA_CloseGadget,	TRUE,
						WA_Activate,		TRUE,
						TAG_DONE))
			{
				struct IntuiMessage		*popup_message;
				ULONG					MsgQualifier, MsgClass;
				UWORD					MsgCode;
				struct Gadget			*MsgGadget;
				BOOL					Done = FALSE;

				UpdateMessageWindow(Handle);

				do
				{
					WaitPort(MessageWindow->UserPort);

					while(popup_message = GT_GetIMsg(MessageWindow->UserPort))
					{
						MsgClass     = popup_message->Class;
						MsgCode      = popup_message->Code;
						MsgQualifier = popup_message->Qualifier;
						MsgGadget    = popup_message->IAddress;

						GT_ReplyIMsg(popup_message);

						LT_HandleInput(Handle, MsgQualifier, &MsgClass, &MsgCode, &MsgGadget);

						switch(MsgClass)
						{
							case	IDCMP_CLOSEWINDOW:
								Done = TRUE;
								break;
							case	IDCMP_GADGETUP:
								switch(MsgGadget->GadgetID)
								{
									case BUTTON_NEWMESSAGE:
										NewMessageDialog();
										break;

									case BUTTON_DELMESSAGE:
										Message("Delete Message");
										UpdateMessageWindow(Handle);
										break;

									case BUTTON_FIRSTMESSAGE:
										Message("First Message");
										UpdateMessageWindow(Handle);
										break;

									case BUTTON_PREVMESSAGE:
										Message("Previous Message");
										UpdateMessageWindow(Handle);
										break;

									case BUTTON_LASTMESSAGE:
										Message("Last Message");
										UpdateMessageWindow(Handle);
										break;

									case BUTTON_NEXTMESSAGE:
										Message("Next Message");
										break;
								}
						}
					}
				}
				while(!Done);
			} else Message(LocaleString(GTLAYOUT_CONTEXT_ERROR));
		}
		LT_DeleteHandle(Handle);
		GlobalCleanup();
	}
}

BOOL OpenLibs(void)
{
	BOOL Success=FALSE;

	if(GTLayoutBase = OpenLibrary("gtlayout.library", 47))
	{
		if(GadToolsBase = OpenLibrary("gadtools.library", 39))
		{
			Success=TRUE;
		}
		else
		{
			Message(LocaleString(OPEN_GADTOOLS_ERROR));
		}
	}
	else
	{
		Message(LocaleString(OPEN_GTLAYOUT_ERROR));
	}

	if (!Success) CloseLibs();
	
	return(Success);
}

void CloseLibs(void)
{
	if(GTLayoutBase)
	{
		CloseLibrary(GTLayoutBase);
		GTLayoutBase=NULL;
	}

	if(GadToolsBase)
	{
		CloseLibrary(GadToolsBase);
		GadToolsBase=NULL;
	}
}

void NewMessageDialog(void)
{
	struct LayoutHandle *Handle;
	char MessageString[512] = "";

	STATIC LONG		ToWhomTableEntryID[] =
	{
		RADIO_TOWHOM_COMPUTER_TEXT,
		RADIO_TOWHOM_WORKGROUP_TEXT,
		-1
	};

	if(Handle = LT_CreateHandleTags(NULL,
		LAHN_AutoActivate, FALSE,
		LAHN_LocaleHook,	&PopHook,
		TAG_DONE))
	{
		struct Window *NewMessageWindow; 
		
		LT_New(Handle,
			LA_Type,		VERTICAL_KIND,
			TAG_DONE);
		{

			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
			{

				LT_New(Handle,
					LA_Type,		VERTICAL_KIND,
					LA_LabelID,		FRAME_MESSAGE_TEXT,
					TAG_DONE);
				{

					LT_New(Handle,
						LA_Type,		MX_KIND,
						LA_ID,			RADIO_TOWHOM,
						LA_LabelPlace,	PLACE_Right,
						LAMX_LabelTable,ToWhomTableEntryID,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		END_KIND,
					TAG_DONE);
				}

				LT_New(Handle,
					LA_Type,		VERTICAL_KIND,
					TAG_DONE);
				{

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		BUTTON_SENDMESSAGE_TEXT,
						LA_ID,			BUTTON_SENDMESSAGE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		BUTTON_CANCEL_TEXT,
						LA_ID,			BUTTON_CANCEL_MESSAGE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		END_KIND,
					TAG_DONE);
				}

				LT_New(Handle,
					LA_Type,		END_KIND,
				TAG_DONE);
			}

			LT_New(Handle,
				LA_Type,		XBAR_KIND,
			TAG_DONE);

#if 0
			LT_New(Handle,
				LA_Type,		TEXTEDIT_KIND,
				LA_ID,			STRING_MESSAGE_LABEL,
				LA_LabelPlace,	PLACE_Above,
				LA_LabelID,		STRING_MESSAGE_LABEL_TEXT,
				LATE_String,	MessageString,
				LATE_Chars,		30,
				LATE_Lines,		5,
			TAG_DONE);
#endif

			LT_New(Handle,
				LA_Type,		END_KIND,
			TAG_DONE);
		}

		if(NewMessageWindow = LT_Build(Handle,
					LAWN_TitleID,	TITLE_MESSAGE,
					LAWN_IDCMP,		IDCMP_CLOSEWINDOW,
					WA_DragBar,		TRUE,
					WA_DepthGadget,	TRUE,
					WA_SizeGadget,	TRUE,
					WA_SizeBBottom,	TRUE,
					WA_CloseGadget,	TRUE,
					WA_Activate,		TRUE,
					TAG_DONE))
		{
			struct IntuiMessage		*newpopup_message;
			ULONG					MsgQualifier, MsgClass;
			UWORD					MsgCode;
			struct Gadget			*MsgGadget;
			BOOL					Done = FALSE;

			do
			{
				WaitPort(NewMessageWindow->UserPort);

				while(newpopup_message = GT_GetIMsg(NewMessageWindow->UserPort))
				{
					MsgClass     = newpopup_message->Class;
					MsgCode      = newpopup_message->Code;
					MsgQualifier = newpopup_message->Qualifier;
					MsgGadget    = newpopup_message->IAddress;

					GT_ReplyIMsg(newpopup_message);

					LT_HandleInput(Handle, MsgQualifier, &MsgClass, &MsgCode, &MsgGadget);

					switch(MsgClass)
					{
						case	IDCMP_CLOSEWINDOW:
							Done = TRUE;
							break;
						case	IDCMP_GADGETUP:
							switch(MsgGadget->GadgetID)
							{
								case BUTTON_SENDMESSAGE:
									Message("Sent!!");
									break;

								case BUTTON_CANCEL_MESSAGE:
									Done = TRUE;
									break;
							}
					}
				}
			}
			while(!Done);
		} else Message(LocaleString(GTLAYOUT_CONTEXT_ERROR));
	}
	LT_DeleteHandle(Handle);
}

int GetMessageCount(void)
{
	struct FileInfoBlock *mailblock;
	char *dirstring = "samba:mail";
	int EntryCount = NULL;
	BPTR locky;

	if((mailblock = AllocVec(sizeof(*mailblock), 0)) && (locky = Lock(dirstring, SHARED_LOCK)))
	{
		if(Examine(locky, mailblock))
		{
			while(ExNext(locky, mailblock))
			{
				EntryCount++;
			}
		}
		else
		{
			Message(LocaleString(EXAMINE_ERROR));
		}
	}
	else
	{
		Message(LocaleString(LOCK_MAILDIR_ERROR));
	}
	
	if(mailblock) FreeVec(mailblock);
	if(locky) UnLock(locky);
	return (EntryCount);
}

void UpdateMessageWindow(struct LayoutHandle *Handle)
{
	sprintf(UpdateString, "current message %ld - count of messages %ld", 0, GetMessageCount());
	
	LT_SetAttributes(Handle, BOX_OVERVIEW,
		LABX_Index,	0,
		LABX_Text,	UpdateString,
	TAG_DONE);
}

void GlobalCleanup(void)
{
	if(MessageList)
		FreeVec(MessageList);

	CloseLibs();
}
