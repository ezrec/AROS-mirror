/*
**	Chat.c
**
**	Chat support code
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Maximum number of characters to enter per line */

#define CHAT_LINE_SIZE	512

	/* Local data */

STATIC struct List	*ChatList;
STATIC struct Node	*ChatNode;
STATIC STRPTR		 ChatBuffer,
					 ChatUndo,
					 ChatWork,
					 ChatTemp;
STATIC IPTR		 ChatPosition;
STATIC Object		*ChatGadget;
STATIC BOOL			 ChatWasActive;

	/* HideChatGadget():
	 *
	 *	Remove the chat gadget, but don't free the buffers.
	 */

VOID
HideChatGadget()
{
	if(ChatGadget)
	{
		GetAttr(STRINGA_BufferPos,ChatGadget,&ChatPosition);

		RemoveGList(Window,(struct Gadget *)ChatGadget,1);

		DisposeObject(ChatGadget);

		ChatGadget = NULL;
	}
}

	/* DeleteChatGadget():
	 *
	 *	Remove the chat gadget, also take care of the buffers.
	 */

VOID
DeleteChatGadget()
{
	HideChatGadget();

	FreeVecPooled(ChatBuffer);
	ChatBuffer = NULL;

	DeleteList(ChatList);
	ChatList = NULL;
}

	/* UpdateChatGadget():
	 *
	 *	Redraw the chat gadget imagery.
	 */

VOID
UpdateChatGadget()
{
	if(ChatGadget)
	{
		struct RastPort	*RPort;
		LONG Left,Right,Top;

		RPort = Window->RPort;

			/* Draw the separator bar */

		if(StatusWindow || Config->ScreenConfig->StatusLine == STATUSLINE_DISABLED)
			Top = UserFontHeight;
		else
			Top = StatusDisplayHeight + UserFontHeight;

		Top		= Window->Height - (Window->BorderBottom + Top + 2);
		Left	= Window->BorderLeft;
		Right	= Window->Width - (Window->BorderRight + 1);

		SetAPen(RPort,Pens[SHADOWPEN]);
		RectFill(RPort,Left,Top,Right,Top);

		Top++;

		SetAPen(RPort,Pens[SHINEPEN]);
		RectFill(RPort,Left,Top,Right,Top);

			/* Redraw the gadget */

		RefreshGList((struct Gadget *)ChatGadget,Window,NULL,1);
	}
}

	/* ActivateChat(BOOL Reactivate):
	 *
	 *	Activate the chat gadget.
	 */

VOID
ActivateChat(BOOL Reactivate)
{
	if(ChatGadget)
	{
		if(Reactivate)
			Reactivate = ChatWasActive;
		else
			Reactivate = TRUE;

		if(Reactivate)
			ActivateGadget((struct Gadget *)ChatGadget,Window,NULL);
	}
}

	/* CreateChatGadget():
	 *
	 *	Create the chat gadget and add it to the main window.
	 */

BOOL
CreateChatGadget()
{
	ChatWasActive = TRUE;

		/* Allocate the history list */

	if(!ChatList)
	{
		if(!(ChatList = CreateList()))
			return(FALSE);

		ChatNode = NULL;
	}

		/* Allocate the undo/work/editing buffers */

	if(!ChatBuffer)
	{
		if(!(ChatBuffer = (STRPTR)AllocVecPooled(5 * CHAT_LINE_SIZE,MEMF_ANY | MEMF_CLEAR)))
		{
			DeleteChatGadget();

			return(FALSE);
		}
		else
		{
			ChatUndo = ChatBuffer	+ CHAT_LINE_SIZE;
			ChatWork = ChatUndo		+ CHAT_LINE_SIZE;
			ChatTemp = ChatWork		+ CHAT_LINE_SIZE;
		}
	}

		/* Finally create the gadget */

	if(!ChatGadget)
	{
		STATIC struct Hook ChatHook;

		LONG Bottom = Window->BorderBottom + UserFontHeight - 1;

		InitHook(&ChatHook,(HOOKFUNC)CommonEditRoutine,NULL);

		if(!StatusWindow && Config->ScreenConfig->StatusLine != STATUSLINE_DISABLED && !Config->ScreenConfig->SplitStatus)
			Bottom += StatusDisplayHeight;

		if(!(ChatGadget = NewObject(NULL,STRGCLASS,
			GA_Left,				Window->BorderLeft,
			GA_Height,				UserFontHeight,
			GA_RelBottom,			-Bottom,
			GA_RelWidth,			-(Window->BorderLeft + Window->BorderRight),
			GA_TabCycle,			FALSE,
			GA_ID,					CHAT_GadgetID,
			GA_RelVerify,			TRUE,
			GA_Immediate,			TRUE,
			STRINGA_TextVal,		ChatBuffer,
			STRINGA_MaxChars,		CHAT_LINE_SIZE-1,
			STRINGA_Buffer,			ChatBuffer,
			STRINGA_BufferPos,		ChatPosition,
			STRINGA_UndoBuffer,		ChatUndo,
			STRINGA_WorkBuffer,		ChatWork,
			STRINGA_NoFilterMode,	TRUE,
			STRINGA_EditHook,		&ChatHook,

				/* NOTE: This should really look like below, */
				/*       according to the BOOPSI documentation. */
				/*       Even worse, the pen numbers are in reality */
				/*       truncated to a maximum of colour # 15. */

	/*		STRINGA_Pens,			((LONG)Pens[BACKGROUNDPEN] << 16) | Pens[TEXTPEN], */
	/*		STRINGA_ActivePens,		((LONG)Pens[BACKGROUNDPEN] << 16) | Pens[TEXTPEN], */

			STRINGA_Pens,			(Pens[BACKGROUNDPEN] << 8) | Pens[TEXTPEN],
			STRINGA_ActivePens,		(Pens[BACKGROUNDPEN] << 8) | Pens[TEXTPEN],
		TAG_DONE)))
		{
			DeleteChatGadget();

			return(FALSE);
		}
		else
			AddGList(Window,(struct Gadget *)ChatGadget,(UWORD)~0,1,NULL);
	}

	return(TRUE);
}

	/* HandleChatGadget(UWORD GadgetCode):
	 *
	 *	Process the input forwarded by the chat line gadget.
	 */

VOID
HandleChatGadget(UWORD GadgetCode)
{
	BOOL Activate,NeedChange,AddIt;
	UWORD Code,Qualifier;

	Activate	= TRUE;
	NeedChange	= FALSE;
	Code		= GadgetCode & 0xFF;
	Qualifier	= GadgetCode >> 8;

	switch(Qualifier)
	{
		case 0xFF:

			switch(Code)
			{
				case CONTROL_('S'):
				case CONTROL_('Q'):

					ChatTemp[0] = Code;
					SendInputTextBuffer(ChatTemp,1,TRUE,TRUE);
					break;

				case '\r':

					LimitedStrcpy(CHAT_LINE_SIZE,ChatTemp,ChatBuffer);

						/* Add the line unless it is already in the history list. */

					AddIt = TRUE;

					if(ChatNode)
					{
						if(!Stricmp(ChatNode->ln_Name,ChatTemp))
							AddIt = FALSE;
					}

					if(AddIt)
					{
						struct Node *Node;

						if(Node = CreateNode(ChatTemp))
							AddTail(ChatList,Node);
					}

					ChatNode = NULL;
					NeedChange = TRUE;

					LimitedStrcat(CHAT_LINE_SIZE,ChatTemp,"\\r");
					SerialCommand(ChatTemp);
					break;

				case '\n':

					Activate = FALSE;
					break;
			}

			break;

		default:

				/* Clear the history list when pressing Amiga+Del/Amiga+Backspace */

			if((Qualifier & (AMIGARIGHT | AMIGALEFT)) && (Code == DEL_CODE || Code == BACKSPACE_CODE))
			{
				FreeList(ChatList);

				ChatNode = NULL;

				NeedChange = TRUE;

				break;
			}

				/* This looks like a function key. Send the corresponding macro. */

			if(Code >= F01_CODE && Code <= F10_CODE)
			{
				STRPTR String;
				LONG Index;

				Index = Code - F01_CODE;

					/* Pick the right macro */

				if(Qualifier & CONTROL_KEY)
					String = MacroKeys->Keys[3][Index];
				else
				{
					if(Qualifier & ALT_KEY)
						String = MacroKeys->Keys[2][Index];
					else
					{
						if(Qualifier & SHIFT_KEY)
							String = MacroKeys->Keys[1][Index];
						else
							String = MacroKeys->Keys[0][Index];
					}
				}

					/* Anything to do? */

				if(String[0])
				{
						/* This should really insert the macro at the current
						 * cursor position, but due to some strange reasons
						 * we can't get the position reliably.
						 */

					LimitedStrcpy(CHAT_LINE_SIZE,ChatTemp,ChatBuffer);
					LimitedStrcat(CHAT_LINE_SIZE,ChatTemp,String);

					SetGadgetAttrs((struct Gadget *)ChatGadget,Window,NULL,
						STRINGA_TextVal,	ChatTemp,
						STRINGA_BufferPos,	strlen(ChatTemp),
					TAG_DONE);
				}

				break;
			}

				/* The user pressed the cursor-up key to
				 * scroll through the command history.
				 */

			if(Code == CURSORUP)
			{
					/* Shift key: jump to first command
					 * history entry.
					 */

				if(Qualifier & SHIFT_KEY)
				{
					NeedChange = TRUE;

					if(!IsListEmpty(ChatList))
						ChatNode = ChatList->lh_Head;
					else
						ChatNode = NULL;
				}
				else
				{
					if(ChatNode)
					{
						if(ChatNode->ln_Pred->ln_Pred)
						{
							NeedChange = TRUE;

							ChatNode = ChatNode->ln_Pred;
						}
					}
					else
					{
						if(!IsListEmpty(ChatList))
						{
							NeedChange = TRUE;

							ChatNode = ChatList->lh_TailPred;
						}
					}
				}
			}

				/* The user pressed the cursor-down key to
				 * scroll through the command history.
				 */

			if(Code == CURSORDOWN)
			{
				NeedChange = TRUE;

					/* Shift key: jump to last command
					 * history entry.
					 */

				if(Qualifier & SHIFT_KEY)
				{
					if(!IsListEmpty(ChatList))
						ChatNode = ChatList->lh_TailPred;
					else
						ChatNode = NULL;
				}
				else
				{
					if(ChatNode)
					{
						if(ChatNode->ln_Succ->ln_Succ)
							ChatNode = ChatNode->ln_Succ;
						else
							ChatNode = NULL;
					}
				}
			}

			break;
	}

		/* Check if we have to update the string gadget contents. */

	if(NeedChange)
	{
		SetGadgetAttrs((struct Gadget *)ChatGadget,Window,NULL,
			STRINGA_TextVal,ChatNode ? ChatNode->ln_Name : "",
		TAG_DONE);
	}

	ChatWasActive = Activate;

	if(Activate)
		ActivateGadget((struct Gadget *)ChatGadget,Window,NULL);
}

	/* MarkChatGadgetAsActive():
	 *
	 *	Remember that the chat gadget was activated.
	 */

VOID
MarkChatGadgetAsActive()
{
	ChatWasActive = TRUE;
}
