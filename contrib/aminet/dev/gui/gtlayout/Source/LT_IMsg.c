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

/*****************************************************************************/

#include <exec/memory.h>

/*****************************************************************************/

#include "Assert.h"

#define MESSAGE_COOKIE 0x19138AFB

/****** gtlayout.library/LT_GetIMsg ******************************************
*
*   NAME
*	LT_GetIMsg -- Retrieve the next pending IntuiMessage
*	              from the window associated with a
*	              LayoutHandle.
*
*   SYNOPSIS
*	IntuiMessage = LT_GetIMsg(Handle);
*	     D0                     A0
*
*	struct IntuiMessage *LT_GetIMsg(LayoutHandle *);
*
*   FUNCTION
*	This routine will handle most of the input loop for
*	you. Just pass the pointer to the layout handle in,
*	check if the result code is non-null, copy the data
*	you need and reply the message via LT_ReplyIMsg().
*
*	You will still need to wait for new input, as LT_GetIMsg
*	will poll the MsgPort of the window.
*
*	LT_GetIMsg() will try its best to distinguish between
*	different LayoutHandles sharing the same Window->UserPort.
*	If it finds that the window the message was sent to is
*	using a different LayoutHandle, it will switch to using
*	this handle (V13).
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure
*
*   RESULT
*	IntuiMessage - Pointer to IntuiMessage structure
*
*   NOTES
*	You *must not* make any assumptions about the contents
*	of the IntuiMessage structure except for the following
*	entries:
*
*	   Class
*	   Code
*	   Qualifier
*	   IAddress
*
*	When finished, you must dispose the IntuiMessage via
*	LT_ReplyIMsg or memory will be lost which can never
*	be reclaimed.
*
*	DO NOT CALL LT_HandleInput() ON THE DATA YOU RECEIVE
*	FROM LT_GetIMsg() AS LT_GetIMsg() ALREADY DOES ALL THE
*	MAGIC LT_HandleInput() OTHERWISE WOULD NEED TO DO!
*	IF YOU STILL DO CALL LT_HandleInput() ON THE DATA YOU
*	WILL RECEIVE `GHOST' EVENTS.
*
*   SEE ALSO
*	gtlayout.library/LT_ReplyIMsg
*
******************************************************************************
*
*/

struct IntuiMessage * LIBENT
LT_GetIMsg(REG(a0) struct LayoutHandle *Handle)
{
	if(Handle)
	{
		struct IntuiMessage *Msg = GT_GetIMsg(Handle->Window->UserPort);

		if(Msg)
		{
			struct IntuiMessage *Clone;

				// For multiple handles sharing the same UserPort

			if(Msg->IDCMPWindow->UserData && !((IPTR)Msg->IDCMPWindow->UserData & 1))
			{
				LayoutHandle *Local = (LayoutHandle *)Msg->IDCMPWindow->UserData;

				if(Local->PointBack == Local)
					Handle = Local;
				else
					return(Msg);
			}
			else
				return(Msg);

			if(Msg->Class == IDCMP_SIZEVERIFY && Handle->ResizeObject != NULL)
			{
					// They're coming to take me away, HAHA!

				Handle->SizeVerified	= TRUE;
				Handle->SizeWidth		= Handle->Window->Width;
				Handle->SizeHeight		= Handle->Window->Height;

				LTP_StripGadgets(Handle,Handle->List);

				#ifdef DO_BOOPSI_KIND
				{
					LTP_StripGadgets(Handle,(struct Gadget *)Handle->BOOPSIList);
				}
				#endif	/* DO_BOOPSI_KIND */

					// Fake a null event, otherwise the following
					// IDCMP_NEWSIZE would probably not get through

				if(Clone = (struct IntuiMessage *)AllocMem(sizeof(struct IntuiMessage),MEMF_ANY))
				{
					CopyMem(Msg,Clone,sizeof(struct IntuiMessage));

					Clone->Class						= 0;
					Clone->ExecMessage.mn_Node.ln_Name	= (char *)MESSAGE_COOKIE;
					Clone->ExecMessage.mn_Node.ln_Pri 	= -114;
					Clone->ExecMessage.mn_ReplyPort		= (struct MsgPort *)Clone;
					Clone->SpecialLink					= (struct IntuiMessage *)Clone;

					GT_ReplyIMsg(Msg);
				}
			}
			else
			{
				if(Clone = (struct IntuiMessage *)AllocMem(sizeof(struct IntuiMessage),MEMF_ANY))
				{
					CopyMem(Msg,Clone,sizeof(struct IntuiMessage));

					Clone->ExecMessage.mn_Node.ln_Name	= (char *)MESSAGE_COOKIE;
					Clone->ExecMessage.mn_Node.ln_Pri 	= -114;
					Clone->ExecMessage.mn_ReplyPort		= (struct MsgPort *)Clone;
					Clone->SpecialLink					= (struct IntuiMessage *)Clone;

					GT_ReplyIMsg(Msg);

					LT_HandleInput(Handle,Clone->Qualifier,&Clone->Class,&Clone->Code,(struct Gadget **)&Clone->IAddress);
				}
			}

			if(Clone)
				return(Clone);
			else
				GT_ReplyIMsg(Msg);
		}
	}

	return(NULL);
}


/*****************************************************************************/


/****** gtlayout.library/LT_ReplyIMsg ******************************************
*
*   NAME
*	LT_ReplyIMsg -- Dispose of an IntuiMessage received
*
*   SYNOPSIS
*	LT_ReplyIMsg(IntuiMessage);
*	                 A0
*
*	VOID LT_ReplyIMsg(struct IntuiMessage *);
*
*   FUNCTION
*	This routine complements LT_GetIMsg().
*
*   INPUTS
*	IntuiMessage - Pointer to IntuiMessage structure,
*	    passing NULL is harmless.
*
*   RESULT
*	none
*
*   NOTES
*	Only pass IntuiMessages you received via LT_GetIMsg,
*	or things will get tough.
*
*   SEE ALSO
*	gtlayout.library/LT_GetIMsg
*
******************************************************************************
*
*/

VOID LIBENT
LT_ReplyIMsg(REG(a0) struct IntuiMessage *Msg)
{
	if(Msg)
	{
		if(Msg->SpecialLink == (struct IntuiMessage *)Msg && Msg->ExecMessage.mn_Node.ln_Name == (char *)MESSAGE_COOKIE && Msg->ExecMessage.mn_Node.ln_Pri == -114 && Msg->ExecMessage.mn_ReplyPort == (struct MsgPort *)Msg)
			FreeMem(Msg,sizeof(struct IntuiMessage));
		else
			GT_ReplyIMsg(Msg);
	}
}
