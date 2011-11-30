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

#include <devices/timer.h>

#include <clib/alib_protos.h>	/* For NewList */

/*****************************************************************************/

#include "Assert.h"

VOID
LTP_Delay(ULONG Seconds,ULONG Micros)
{
	struct MsgPort 		TimePort;
	struct timerequest	TimeRequest;

	memset(&TimePort,0,sizeof(TimePort));

	NewList(&TimePort.mp_MsgList);

	TimePort.mp_Flags	= PA_SIGNAL;
	TimePort.mp_SigBit	= SIGB_SINGLE;
	TimePort.mp_SigTask	= FindTask(NULL);

	memset(&TimeRequest,0,sizeof(TimeRequest));

	TimeRequest.tr_node.io_Message.mn_ReplyPort	= &TimePort;
	TimeRequest.tr_node.io_Message.mn_Length	= sizeof(struct timerequest);

	if(!OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)&TimeRequest,0))
	{
		TimeRequest.tr_node.io_Command	= TR_ADDREQUEST;
		TimeRequest.tr_time.tv_secs 	= Seconds;
		TimeRequest.tr_time.tv_micro	= Micros;

		SetSignal(0,SIGF_SINGLE);
		SendIO((struct IORequest *)&TimeRequest);
		WaitIO((struct IORequest *)&TimeRequest);

		CloseDevice((struct IORequest *)&TimeRequest);
	}
}
