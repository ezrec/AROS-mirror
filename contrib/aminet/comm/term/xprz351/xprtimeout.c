/** timeout.c
*
*   Roll-yer-own Delay() function.
*
**/
#include "xprzmodem_all.h"

#define TRSIZE ((long) sizeof(struct timerequest))

#ifdef zedzap
void XprTimeOut(long ticks)
{
  long secs, micros;
  struct timerequest *Timereq = NULL;
  struct MsgPort *Timerport;

  if (ticks == 0L) return;

  Timerport = CreatePort(0L, 0L);
  if (Timerport == NULL) goto cleanup;

  Timereq = (struct timerequest *) AllocMem(TRSIZE, MEMF_PUBLIC | MEMF_CLEAR);
  if (Timereq == NULL) goto cleanup;

  if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)Timereq, 0L) != NULL) goto cleanup;
/*
*  Set up timer request.
*/
  secs = ticks / 50L;
  micros = (ticks % 50L) * 20000L;

  Timereq->tr_node.io_Message.mn_ReplyPort = Timerport;
  Timereq->tr_node.io_Command = TR_ADDREQUEST;
  Timereq->tr_node.io_Flags = 0;
  Timereq->tr_node.io_Error = 0;
  Timereq->tr_time.tv_secs = secs;
  Timereq->tr_time.tv_micro = micros;
/*
*  Time out
*/
  SendIO((struct IORequest *)Timereq);
  Wait(1L << Timerport->mp_SigBit);
/*
*  Handle timer events.
*/
  GetMsg(Timerport);
/*
*  Clean up
*/
cleanup:
  if (Timereq)
  {
    if (Timereq->tr_node.io_Message.mn_ReplyPort)
      CloseDevice((struct IORequest *)Timereq);
    FreeMem(Timereq, TRSIZE);
  }
  if (Timerport) DeletePort(Timerport);

  return;
}
#endif
