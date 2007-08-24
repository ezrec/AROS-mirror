#include "Private.h"

///thread_init
int32 thread_init(void)
{
   return TRUE;
}
//+
///thread_exit
void thread_exit(void)
{
}
//+

///Thread_Main
F_THREAD_ENTRY(Thread_Main)
{
   CUD -> process = (struct Process *) FindTask(NULL);
   CUD -> Signals = (1 << CUD -> process -> pr_MsgPort.mp_SigBit);

   for (;;)
   {
      FThreadMsg *msg = (FThreadMsg *) GetMsg(&CUD -> process -> pr_MsgPort);

      if (msg)
      {
         switch (msg -> Method)
         {
            case FV_Thread_Hello:
            {
               msg -> Return = thread_init();
            }
            break;

            case FV_Thread_Bye:
            {
               thread_exit();

               msg -> Return = TRUE;

               ReplyMsg((struct Message *) msg);
               
               return 0;
            }
            break;
         }
         
         if (msg)
         {
            ReplyMsg((struct Message *) msg);
         }
      }

      Wait(CUD -> Signals);
   }
}
//+
