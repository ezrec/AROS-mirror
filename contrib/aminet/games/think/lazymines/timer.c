/*
 * timer.c
 * =======
 * Implementation of a timer.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/memory.h>
#include <proto/exec.h>
#include <devices/timer.h>

#include "timer.h"


/* Timer data */
struct timer
{
   struct MsgPort      *mp;
   struct timerequest  *io;
   BOOL                 used;
};


/* Creates a new timer object */
timer_ptr             /* created timer */
timer_create (void)
{
   timer_ptr   timer;
   
   if (timer = AllocVec (sizeof (*timer), MEMF_PUBLIC))
   {
      timer->mp = NULL;
      timer->io = NULL;
      timer->used = FALSE;
      if (timer->mp = CreateMsgPort ())
      {
         if (timer->io = (struct timerequest *)
                         CreateIORequest (timer->mp, sizeof (*timer->io)))
         {
            if (0 == OpenDevice (TIMERNAME, UNIT_VBLANK,
                                 (struct IORequest *)timer->io, 0L))
            {
               return timer;
            }
            DeleteIORequest (timer->io);
         }
         DeleteMsgPort (timer->mp);
      }
      FreeVec (timer);
   }
   
   return NULL;
}


/* Frees a timer object */
void
timer_free (
   timer_ptr   timer)   /* timer to free */
{
   if (timer)
   {
      timer_stop (timer);
      CloseDevice ((struct IORequest *)timer->io);
      DeleteIORequest ((struct IORequest *)timer->io);
      DeleteMsgPort (timer->mp);
      FreeVec (timer);
   }
}


/* Calculates the signal of a timer */
ULONG          /* calculated signal */
timer_signal (
   timer_ptr   timer)   /* timer to calculate signal for */
{
   return (ULONG)(1L << timer->mp->mp_SigBit);
}


/* Starts a timer */
void
timer_start (
   timer_ptr   timer,   /* timer to start */
   ULONG       secs,    /* number of seconds timer will count */
   ULONG       micro)   /* number of microseconds the timer will count */
{
   timer->io->tr_node.io_Command = TR_ADDREQUEST;
   timer->io->tr_time.tv_secs = secs;
   timer->io->tr_time.tv_micro = micro;
   SendIO ((struct IORequest *)timer->io);
   timer->used = TRUE;
}


/* Replies a timer */
void
timer_reply (
   timer_ptr   timer)   /* timer to reply */
{
   while (GetMsg (timer->mp))
      ;
}


/* Stops a timer */
void
timer_stop (
   timer_ptr   timer)   /* timer to stop */
{
   if (timer->used)
   {
      AbortIO ((struct IORequest *)timer->io);
      WaitIO ((struct IORequest *)timer->io);
      SetSignal (0L, timer_signal (timer));
   }
}
