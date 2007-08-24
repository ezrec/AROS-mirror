
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <dos/notify.h>
#include <libraries/feelin.h>

#include <proto/feelin.h>
#include <proto/exec.h>
#include <proto/dos.h>

struct FeelinBase *FeelinBase;

///Thread_Main
F_THREAD_ENTRY(Thread_Main)
{
   struct Process *proc = (struct Process *) FindTask(NULL);
   FThreadMsg *msg;

   for (;;)
   {
      if (msg = (FThreadMsg *) GetMsg(&proc -> pr_MsgPort))
      {
         F_Log(0,"Msg 0x%08lx - Action 0x%08lx",msg,msg -> Action);

         switch (msg -> Action)
         {
            case FV_Thread_Hello:
            {
               msg -> Return = TRUE;
            }
            break;

            case FV_Thread_Bye:
            {
               F_Log(0,"bye");

               msg -> Return = TRUE;

               ReplyMsg(msg); return;
            }
            break;
         }

         ReplyMsg(msg);
      }

      WaitPort(&proc -> pr_MsgPort);
   }
   return;
}
//+

int32 main(void)
{
    if (F_FEELIN_OPEN)
    {
        FObject thread = ThreadObject,
            "FA_Thread_Entry",     Thread_Main,
            "FA_Thread_Name",      "test.thread",
            "FA_Thread_Priority",  "Low",

            End;
           
        if (thread)
        {
            F_Log(0,"Thread 0x%08lx - Priority %ld",thread,F_Get(thread,(ULONG) "FA_Thread_Priority"));

            Wait(0x1000);

            F_DisposeObj(thread);
        }
        
        F_FEELIN_CLOSE;
    }
    return 0;
}

