/*
**	Accountant.c
**
**	Connection cost accounting
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

/****************************************************************************/

enum	{ STATE_Idle, STATE_Waiting };

/****************************************************************************/

enum	{ ACMSG_Start,ACMSG_Stop,ACMSG_Query };

typedef struct AccountantMsg
{
	struct Message	Message;

	UWORD			Type;
	struct timeval	Time;
} AccountantMsg;

/****************************************************************************/

STATIC struct MsgPort			*AccountantPort;
STATIC struct Task				*AccountantTask;

STATIC struct SignalSemaphore	 AccountantSemaphore;
STATIC ULONG					 AccountantCost;

/****************************************************************************/

	/* AccountantEntry(VOID):
	 *
	 *	The rates accounting task.
	 */

STATIC VOID SAVE_DS
AccountantEntry(VOID)
{
	struct MsgPort *TimePort;

		/* Set up the timer resources */

	if(TimePort = CreateMsgPort())
	{
		struct timerequest *TimeRequest;

		if(TimeRequest = (struct timerequest *)CreateIORequest(TimePort,sizeof(struct timerequest)))
		{
			if(!OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)TimeRequest,0))
			{
					/* Now create the communications port */

				if(AccountantPort = CreateMsgPort())
				{
					struct timeval StopTime;
					struct timeval Now;
					ULONG Signals;
					WORD State;

						/* That's me */

					AccountantTask = FindTask(NULL);

						/* Wake up the main program */

					Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

						/* Doing nothing yet... */

					State = STATE_Idle;

					FOREVER
					{
							/* Wait for a sign */

						Signals = Wait(PORTMASK(TimePort) | PORTMASK(AccountantPort) | SIG_KILL);

							/* Terminate this task? */

						if(Signals & SIG_KILL)
							break;

							/* The timer has elapsed? */

						if((Signals & PORTMASK(TimePort)) && State == STATE_Waiting)
						{
							PhoneEntry *ChosenEntry;
							struct List *ChosenPattern;
							ULONG Cost;

								/* Terminate the timer request */

							WaitIO((struct IORequest *)TimeRequest);

								/* We are no longer waiting */

							State = STATE_Idle;

								/* Start with a clean slate */

							Cost = 0;

								/* We want to read the accounting data */

							ChosenPattern = LockActivePattern();
							ChosenEntry = LockActiveEntry(GlobalPhoneHandle);

							if(ChosenEntry || ChosenPattern)
							{
									/* Look up the current time */

								GetSysTime(&Now);

									/* Check which rate data is current */

								SelectTime(ChosenEntry,ChosenPattern,&Now);

									/* Store the cost for the next unit */

								Cost = PayPerUnit[DT_NEXT_UNIT];

									/* Does this unit last for a certain time? */

								if(SecPerUnit[DT_NEXT_UNIT] > 0)
								{
									struct timeval TimeVal;

										/* This is the time it takes for the */
										/* unit to elapse */

									TimeVal.tv_secs		= SecPerUnit[DT_NEXT_UNIT] / 10000;
									TimeVal.tv_micro	= (SecPerUnit[DT_NEXT_UNIT] % 10000) * 100;

										/* Remember when the unit will have elapsed */

									GetSysTime(&StopTime);
									AddTime(&StopTime,&TimeVal);

										/* Start waiting again */

									TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
									TimeRequest->tr_time			= TimeVal;

									SendIO((struct IORequest *)TimeRequest);

									State = STATE_Waiting;
								}
							}

							UnlockActiveEntry(GlobalPhoneHandle);
							UnlockActivePattern();

								/* Add up the connection cost */

							ObtainSemaphore(&AccountantSemaphore);
							AccountantCost += Cost;
							ReleaseSemaphore(&AccountantSemaphore);
						}

							/* Somebody wants us to do something... */

						if(Signals & PORTMASK(AccountantPort))
						{
							AccountantMsg *Message;

								/* For each message... */

							while(Message = (AccountantMsg *)GetMsg(AccountantPort))
							{
								switch(Message->Type)
								{
										/* Start waiting? */

									case ACMSG_Start:

											/* Remember the time when this unit */
											/* will have elapsed */

										GetSysTime(&StopTime);
										AddTime(&StopTime,&Message->Time);

											/* Start waiting */

										TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
										TimeRequest->tr_time			= Message->Time;

										SendIO((struct IORequest *)TimeRequest);

										State = STATE_Waiting;

										break;

										/* Stop waiting? */

									case ACMSG_Stop:

											/* Are we currently waiting? */

										if(State == STATE_Waiting)
										{
											if(!CheckIO((struct IORequest *)TimeRequest))
												AbortIO((struct IORequest *)TimeRequest);

											WaitIO((struct IORequest *)TimeRequest);

											State = STATE_Idle;
										}

										break;

										/* Return the time that still has to elapse? */

									case ACMSG_Query:

										if(State == STATE_Waiting)
										{
												/* This is now */

											GetSysTime(&Now);

												/* Is there still time left */
												/* before the unit elapses? */

											if(CmpTime(&StopTime,&Now) < 0)	/* i.e. "StopTime > Now" */
											{
													/* Return the remaining time */

												Message->Time = StopTime;

												SubTime(&Message->Time,&Now);

												break;
											}
										}

											/* Clear the time value */

										memset(&Message->Time,0,sizeof(Message->Time));

										break;
								}

									/* Return the message */

								ReplyMsg((struct Message *)Message);
							}
						}
					}

						/* Are we still waiting? */

					if(State == STATE_Waiting)
					{
						if(!CheckIO((struct IORequest *)TimeRequest))
							AbortIO((struct IORequest *)TimeRequest);

						WaitIO((struct IORequest *)TimeRequest);
					}

						/* Start cleaning up... */

					DeleteMsgPort(AccountantPort);
				}

				CloseDevice((struct IORequest *)TimeRequest);
			}

			DeleteIORequest((struct IORequest *)TimeRequest);
		}

		DeleteMsgPort(TimePort);
	}

		/* Finished; wave goodbye and exit */

	Forbid();

	AccountantTask = NULL;
	Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
}

/****************************************************************************/

	/* DeleteAccountant():
	 *
	 *	Stop the accounting task.
	 */

VOID
DeleteAccountant()
{
	ShakeHands(AccountantTask,SIG_KILL);
}

	/* CreateAccountant():
	 *
	 *	Launch the accounting task.
	 */

BOOL
CreateAccountant()
{
	InitSemaphore(&AccountantSemaphore);

	Forbid();

	if(LocalCreateTask("term Accountant Task",ThisProcess->pr_Task.tc_Node.ln_Pri + 10,(TASKENTRY)AccountantEntry,4096,0))
		WaitForHandshake();

	Permit();

	return((BOOL)(AccountantTask != NULL));
}

/****************************************************************************/

	/* QueryAccountantTime(struct timeval *Time):
	 *
	 *	Query the time that will have to elapse before
	 *	the next unit will begin.
	 */

ULONG
QueryAccountantTime(struct timeval *Time)
{
	AccountantMsg Message;

	memset(&Message,0,sizeof(Message));

	Message.Message.mn_Length	= sizeof(Message);
	Message.Type				= ACMSG_Query;

	SendMessageGetReply(AccountantPort,(struct Message *)&Message);

	if(Time)
	{
		Time->tv_secs	= Message.Time.tv_secs;
		Time->tv_micro	= Message.Time.tv_micro;
	}

	return(Message.Time.tv_secs);
}

	/* QueryAccountantCost():
	 *
	 *	Query the cost of the current connection.
	 */

ULONG
QueryAccountantCost()
{
	ULONG Cost;

	SafeObtainSemaphoreShared(&AccountantSemaphore);
	Cost = AccountantCost;
	ReleaseSemaphore(&AccountantSemaphore);

	return(Cost);
}

	/* StopAccountant():
	 *
	 *	Stop cost accounting and return the current
	 *	connection cost.
	 */

ULONG
StopAccountant()
{
	AccountantMsg Message;
	ULONG Cost;

	memset(&Message,0,sizeof(Message));

	Message.Message.mn_Length	= sizeof(Message);
	Message.Type				= ACMSG_Stop;

	SendMessageGetReply(AccountantPort,(struct Message *)&Message);

	SafeObtainSemaphoreShared(&AccountantSemaphore);
	Cost = AccountantCost;
	ReleaseSemaphore(&AccountantSemaphore);

	return(Cost);
}

	/* StartAccountant(ULONG OnlineSeconds):
	 *
	 *	Start cost accounting for the current connection.
	 */

VOID
StartAccountant(ULONG OnlineSeconds)
{
	PhoneEntry *ChosenEntry;
	struct List *ChosenPattern;
	struct timeval Remain;
	struct timeval Now;
	ULONG Cost;

	ChosenPattern = LockActivePattern();
	ChosenEntry = LockActiveEntry(GlobalPhoneHandle);

		/* Forget the old data */

	StopAccountant();

		/* Now is the time for all good men... */

	GetSysTime(&Now);

		/* Did the connection happen a few seconds earlier? */

	if(OnlineSeconds > 0)
	{
		struct timeval Lead;

			/* Take care of the leading time */

		Lead.tv_secs	= OnlineSeconds;
		Lead.tv_micro	= 0;

			/* Technically, the connection happened a bit earlier. */

		SubTime(&Now,&Lead);

			/* Choose the right rate accounting time. */

		SelectTime(ChosenEntry,ChosenPattern,&Now);

			/* Enter the first rate. */

		Cost = PayPerUnit[DT_FIRST_UNIT];

		Remain.tv_secs	= SecPerUnit[DT_FIRST_UNIT] / 10000;
		Remain.tv_micro	= (SecPerUnit[DT_FIRST_UNIT] % 10000) * 100;

			/* Did the first unit elapse already? */

		while(-CmpTime(&Lead,&Remain) >= 0 && (Remain.tv_secs || Remain.tv_micro))	/* "Lead >= Remain" */
		{
				/* Subtract the unit time */

			SubTime(&Lead,&Remain);

				/* Update the "current" time so SelectTime */
				/* can do something sensible. */

			AddTime(&Now,&Remain);

			SelectTime(ChosenEntry,ChosenPattern,&Now);

			Remain.tv_secs	= SecPerUnit[DT_NEXT_UNIT] / 10000;
			Remain.tv_micro	= (SecPerUnit[DT_NEXT_UNIT] % 10000) * 100;

				/* In any case, a new unit starts here */

			Cost += PayPerUnit[DT_NEXT_UNIT];
		}

			/* Adjust the remaining time it takes for the unit */
			/* to elapse. */

		SubTime(&Remain,&Lead);
	}
	else
	{
			/* Choose the right rate accounting time. */

		SelectTime(ChosenEntry,ChosenPattern,&Now);

			/* Enter the first rate. */

		Cost = PayPerUnit[DT_FIRST_UNIT];

		Remain.tv_secs	= SecPerUnit[DT_FIRST_UNIT] / 10000;
		Remain.tv_micro	= (SecPerUnit[DT_FIRST_UNIT] % 10000) * 100;
	}

		/* Store the cost */

	ObtainSemaphore(&AccountantSemaphore);
	AccountantCost = Cost;
	ReleaseSemaphore(&AccountantSemaphore);

		/* That's it; "Remain" now holds the time to wait before */
		/* the first unit elapses. Careful here, we must not */
		/* launch a `zero' time wait request. */

	if(Remain.tv_secs > 0 || Remain.tv_micro > 0)
	{
		AccountantMsg Message;

		memset(&Message,0,sizeof(Message));

		Message.Message.mn_Length	= sizeof(Message);
		Message.Type				= ACMSG_Start;
		Message.Time				= Remain;

		SendMessageGetReply(AccountantPort,(struct Message *)&Message);
	}

	UnlockActiveEntry(GlobalPhoneHandle);
	UnlockActivePattern();
}
