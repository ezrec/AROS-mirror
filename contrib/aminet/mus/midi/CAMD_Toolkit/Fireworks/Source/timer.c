
#include "fireworks.h"
#include "fireworks_protos.h"

#include <proto/timer.h>

//INTERRUPT 
SAVEDS int VBlankInterface( void );

struct EClockVal eclock;
ULONG E_Freq;
UWORD E_Freq_1000;



/* Interrupt stuff */

struct hoh{
  APTR a;APTR b;UBYTE c;BYTE d;char *e;
  APTR f;
  APTR g;
};

struct hoh huffoghuff={
  NULL,NULL,NT_INTERRUPT,-60,"Fireworks VBlank",	/* node, pri = -60 */
  NULL,											/* data ptr, same as inputevent */
  (void *)VBlankInterface							/* code ptr */
};

struct Interrupt *VertBlank = (struct Interrupt *)&huffoghuff;

struct EClockVal stampeclock;
BOOL Watch;
ULONG IdleCount;


/*-------------------*/
/* Open timer device */
/*-------------------*/

struct timerequest *OpenTimer(void)
{
	BOOL Success = FALSE;
	
	struct MsgPort *reply = NULL;
	struct timerequest *treq = NULL;
	
	kprintf("About to open Timer\n");
	if (!(reply = CreateMsgPort()))
	{
		Message("Unable to create timer port!",NULL);
	}
	else
	{
		if (!(treq = (struct timerequest *)CreateIORequest(reply, (ULONG)sizeof(struct timerequest))))
		{
			Message("Unable to create timer request!",NULL);
		}
		else
		{
			if (OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest*)treq,0))
			{
				Message("Timer: Cannot open timer.device (UNIT_VBLANK).",NULL);
			}
			else
			{
				TimerBase = (struct Library*)treq->tr_node.io_Device;
				
				E_Freq      = ReadEClock(&eclock);
				E_Freq_1000 = E_Freq / 1000;
				
				Success = TRUE;
			}
		}
	}
	
	if (!Success)
	{
		if (TimerBase)
		{
			CloseDevice((struct IORequest*)treq);
			TimerBase = NULL;
		}
		
		if (treq)
		{
			DeleteIORequest((struct IORequest *)treq);
			treq = NULL;
		}
		
		if (reply)
		{
			DeleteMsgPort(reply);
			reply = NULL;
		}
	}
	
	return(treq);
}



/*--------------------*/
/* Close timer device */
/*--------------------*/

void CloseTimer(struct timerequest *treq)
{
	struct MsgPort *reply = NULL;
	
	if (treq)
	{
		if (treq->tr_node.io_Device)
		{
			CloseDevice((struct IORequest*)treq);
			TimerBase = NULL;
		}
		
		reply = treq->tr_node.io_Message.mn_ReplyPort;
		
		DeleteIORequest((struct IORequest *)treq);
		treq = NULL;
	}
	
	if (reply)
	{
		DeleteMsgPort(reply);
		reply = NULL;
	}
}


/*----------------*/
/* Get Time Delta */
/*----------------*/

/* This function returns the time difference in milliseconds */
/* to the time it was called last                            */

ULONG GetTimeDelta(void)
{
	ULONG delta;
	ULONG old_lo = eclock.ev_lo;
	
	ReadEClock(&eclock);
	
	delta = 1000 * (eclock.ev_lo - old_lo) / E_Freq;
	
	return(delta);
}


/*------------------------*/
/* How Old Is Time Stamp? */
/*------------------------*/

/* This function returns the age of a time stamp in milliseconds */
/* relatively to the time GetTimeDelta() was called last         */

LONG HowOldIsTimestamp(ULONG timestamp)
{
	LONG delta = ((LONG)eclock.ev_lo - (LONG)timestamp) / (WORD)E_Freq_1000;
	
	return(delta);
}



/*----------------------------------------*/
/* VBR Interrupt handler (Watchdog timer) */
/*----------------------------------------*/

//INTERRUPT 
SAVEDS int VBlankInterface( void )
{
	ReadEClock(&stampeclock);		/* for timestamping */
	
	kprintf("I'm actually inside an interrupt, or something\n");

	if (Watch)
	{
		if (IdleCount++ >= (50/MIN_FPS))
		{
			SetTaskPri(MyTask, HIGHPRI);
			IdleCount = 0;
		}
	}
	
	return 0;                      /* server chain continues */
}
