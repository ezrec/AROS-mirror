/*
**	Rendezvous.c
**
**	External program interface for Amiga telecommunications software
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* TaskDestructor(struct DataMsg *Item):
	 *
	 *	Msg destructor for the routines below.
	 */

STATIC VOID
TaskDestructor(struct DataMsg *Item)
{
	Signal((struct Task *)Item->Data,SIGBREAKF_CTRL_F);
}

#ifndef __AROS__
struct RendezvousData * SAVE_DS ASM
RendezvousLogin(REG(a0) struct MsgPort *ReadPort,REG(a1) struct MsgPort *WritePort,REG(a2) struct TagItem *UnusedTagList)
#else
AROS_UFH3(struct RendezvousData *, RendezvousLogin,
 AROS_UFHA(struct MsgPort * , ReadPort , A0),
 AROS_UFHA(struct MsgPort * , WritePort, A2),
 AROS_UFHA(struct TagItem * , UnusedTagList, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	struct RendezvousData *Data;

	if(Data = (struct RendezvousData *)AllocVecPooled(sizeof(struct RendezvousData),MEMF_ANY | MEMF_CLEAR))
	{
		struct DataMsg Msg;

		InitMsgItem(&Msg,(DESTRUCTOR)TaskDestructor);

		Msg.Type = DATAMSGTYPE_RENDEZVOUS;
		Msg.Data = (UBYTE *)FindTask(NULL);

		Forbid();

		ClrSignal(SIGBREAKF_CTRL_F);

		PutMsgItem(SpecialQueue,(struct MsgItem *)&Msg);

		Wait(SIGBREAKF_CTRL_F);

		Permit();

		if(ReadRequest && WriteRequest)
		{
			struct Node *Node;

			CopyMem(ReadRequest,&Data->rd_ReadRequest,sizeof(struct IOExtSer));

			Data->rd_ReadRequest.IOSer.io_Message.mn_ReplyPort = ReadPort;

			CopyMem(WriteRequest,&Data->rd_WriteRequest,sizeof(struct IOExtSer));

			Data->rd_WriteRequest.IOSer.io_Message.mn_ReplyPort = WritePort;

			if(Window)
				Data->rd_Screen = Window->WScreen;

			NewList(&Data->rd_UploadList);
			NewList(&Data->rd_DownloadList);
			NewList(&Data->rd_SentList);
			NewList(&Data->rd_ReceivedList);

			while(Node = RemoveFirstGenericListNode(GenericListTable[GLIST_UPLOAD]))
				AddTail(&Data->rd_UploadList,Node);

			Data->rd_SendPath		= Config->PathConfig->BinaryUploadPath;
			Data->rd_ReceivePath	= Config->PathConfig->BinaryDownloadPath;
			Data->rd_Options		= "";
		}
		else
		{
			FreeVecPooled(Data);

			Data = NULL;
		}
	}

	return(Data);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

VOID SAVE_DS ASM
RendezvousLogoff(REG(a0) struct RendezvousData *Data)
{
	if(Data)
	{
		struct Node *Node,*Next;

		for(Node = Data->rd_UploadList.lh_Head ; Next = Node->ln_Succ ; Node = Next)
			FreeVecPooled(Node);

		for(Node = Data->rd_DownloadList.lh_Head ; Next = Node->ln_Succ ; Node = Next)
			FreeVecPooled(Node);

		for(Node = Data->rd_SentList.lh_Head ; Next = Node->ln_Succ ; Node = Next)
			FreeVecPooled(Node);

		for(Node = Data->rd_ReceivedList.lh_Head ; Next = Node->ln_Succ ; Node = Next)
			FreeVecPooled(Node);

		FreeVecPooled(Data);

		Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
	}
}

struct Node * SAVE_DS ASM
RendezvousNewNode(REG(a0) STRPTR Name)
{
	if(Name)
	{
		LONG Len;

		if(Len = strlen(Name))
		{
			struct Node *Node;

			if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + Len + 1,MEMF_ANY))
			{
				memset(Node,0,sizeof(struct Node));

				strcpy(Node->ln_Name = (STRPTR)(Node + 1),Name);

				return(Node);
			}
		}
	}

	return(NULL);
}

VOID
RendezvousSetup()
{
	InitSemaphore((struct SignalSemaphore *)&RendezvousSemaphore);

#ifdef USE_GLUE
	RendezvousSemaphore.rs_Login	= RendezvousLoginGlue;
	RendezvousSemaphore.rs_Logoff	= RendezvousLogoffGlue;
	RendezvousSemaphore.rs_NewNode	= RendezvousNewNodeGlue;
#else
	RendezvousSemaphore.rs_Login	= RendezvousLogin;
	RendezvousSemaphore.rs_Logoff	= RendezvousLogoff;
	RendezvousSemaphore.rs_NewNode	= RendezvousNewNode;
#endif	/* USE_GLUE */
}

VOID
AddRendezvousSemaphore(STRPTR Name)
{
	Forbid();

	RendezvousSemaphore.rs_Semaphore.ss_Link.ln_Name = Name;
	RendezvousSemaphore.rs_Semaphore.ss_Link.ln_Pri  = -127;

	AddSemaphore((struct SignalSemaphore *)&RendezvousSemaphore);

	Permit();
}

VOID
RemoveRendezvousSemaphore()
{
	Forbid();

	if(RendezvousSemaphore.rs_Semaphore.ss_Link.ln_Name)
		RemSemaphore((struct SignalSemaphore *)&RendezvousSemaphore);

	Permit();
}
