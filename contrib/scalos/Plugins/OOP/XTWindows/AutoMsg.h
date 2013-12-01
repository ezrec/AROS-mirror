// AutoMsg.h
// $Date$
// $Revision$

struct AutoReplyPort
{
	struct MsgPort		msgPort;
	struct Interrupt	inter;
	struct ScalosBase	*ScalosBase;		// 56
	short			numMsgs;
};


#define InitAutoReplyPort( rp, sb )				\
	{							\
	(rp)->inter.is_Node.ln_Type = NT_INTERRUPT;		\
	(rp)->inter.is_Node.ln_Pri = 32;			\
	(rp)->inter.is_Data = rp;				\
	(rp)->inter.is_Code = (VOID (*)(VOID)) DISPATCHER_REF(FreeMsgInter);	 \
	(rp)->msgPort.mp_Flags = PA_SOFTINT;			\
	(rp)->msgPort.mp_SigTask = &(rp)->inter;		\
	(rp)->ScalosBase = sb;					\
	(rp)->numMsgs = 0;					\
	NewList( &(rp)->msgPort.mp_MsgList );			\
	}                                                       

#define FreeAutoReplyPort( rp )					\
	{							\
	while( (rp)->numMsgs > 0 )				\
		;						\
	}                                                       

#define PutAutoMsg( mp, msg, rp )				\
	{							\
	(rp)->numMsgs ++;					\
	(msg)->mn_ReplyPort = &(rp)->msgPort;			\
	PutMsg( mp, msg );					\
	}
