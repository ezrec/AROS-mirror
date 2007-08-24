#include "Private.h"

//#define DB_SEND

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

/// thread_main
F_THREAD_ENTRY(thread_main)
{
    struct Process *proc = (struct Process *) FindTask(NULL);
    
    for (;;)
    {
        FThreadMsg *msg = (FThreadMsg *) GetMsg(&proc->pr_MsgPort);
    
        if (msg)
        {
            if (msg->Action == FV_Thread_Init)
            {
                struct FS_Thread_Init *params = msg->Params;
               
                struct FeelinClass *Class = params->Class;
                FObject Obj = params->Object;
                F_THREAD_ENTRY_PROTO((*entry)) = params->UserEntry;
                APTR userdata = params->UserData;
                struct FeelinBase *FeelinBase = params->Feelin;
               
                struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
                msg->Return = TRUE;
               
                ReplyMsg((struct Message *) msg);

                /* call user function */

                /*GOFROMIEL: 'entry' ne peut pas être  NULL,  nous  l'avons
                déjà testé lors de la création de l'objet (Thread_New)*/
               
                entry(Obj,userdata,FeelinBase);

                /* Terminate all messages sent and waiting for a reply */
               
                if (!AttemptSemaphore(&LOD->arbitrer))
                {
                    while ((msg = (FThreadMsg *) GetMsg(&proc->pr_MsgPort)) != NULL)
                    {
                        ReplyMsg((APTR) msg);
                    }
               
                    ObtainSemaphore(&LOD->arbitrer);
                }
           
                LOD->process = NULL; /* it's the signal that this process does no longer exist */

                ReleaseSemaphore(&LOD->arbitrer);
                
                return;
            }
            else
            {
                /* re-queue message */
               
                PutMsg(&proc->pr_MsgPort, (struct Message *) msg);
            }
        }
    
        Wait(1 << proc -> pr_MsgPort.mp_SigBit);
    };
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Thread_New
F_METHOD(FObject, Thread_New)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = Msg, item;
    
    uint32 entry = 0, stacksize = 8192, pri = 0, name = NULL;
    APTR data = NULL;

    while (F_DynamicNTI(&Tags, &item, Class) != NULL)
    {
        switch (item.ti_Tag)
        {
            case FA_Thread_Entry: entry = item.ti_Data; break;
            case FA_Thread_StackSize: stacksize = item.ti_Data; break;
            case FA_Thread_Priority: pri = item.ti_Data; break;
            case FA_Thread_Name: name = item.ti_Data; break;
            case FA_Thread_UserData: data = (APTR) item.ti_Data; break;
        }
    }

    InitSemaphore(&LOD->arbitrer);

    if (entry)
    {
        ObtainSemaphore(&LOD->arbitrer);
        
        LOD->process = CreateNewProcTags(
            
            NP_Entry,       (uint32) thread_main,
            NP_StackSize,   stacksize,
            NP_Priority,    pri,
            NP_Name,        (uint32) name,
            
            TAG_DONE);

        ReleaseSemaphore(&LOD->arbitrer);

        if (LOD->process)
        {
            uint32 rc;

//            F_Log(0, "Process 0x%08lx - FM_Thread_Send 0x%08lx (%ld)", (uint32) LOD->process, F_IDM(FM_Thread_Send), FM_Thread_Send);

            rc = F_Do(Obj, F_IDM(FM_Thread_Send), FV_Thread_Init, Class, Obj, entry, data, FeelinBase);
            
            if (rc)
            {
                /* User init msg */
                
                rc = F_Do(Obj, F_IDM(FM_Thread_Send), FV_Thread_Hello);
                
                if (rc)
                {
                    return Obj;
                }
                else
                {
                    F_Log(0, "Process returns %ld to FV_Thread_Hello", rc);
                }
            }
            else
            {
                F_Log(0, "Process (%s) returns %ld to FV_Thread_Init", name ? name : (uint32) "private", rc);
            }
        }
    }
    return NULL;
}
//+
///Thread_Dispose
F_METHOD(void, Thread_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);

    if (!F_Do(Obj, F_IDM(FM_Thread_Send), FV_Thread_Bye))
    {
        /* process is not dead, but doesn't respond */
        
        if (LOD->process)
        {
            F_Alert("Thread.Dispose",
                    "Thread didn't respond to FV_Thread_Bye.\n"
                    "The process 0x%08lx will be killed", (uint32) LOD->process);

            Forbid();
            
            RemTask((struct Task *) (LOD->process));
            
            LOD->process = NULL;
            
            Permit();
        }
    }

    ObtainSemaphore(&LOD->arbitrer);

    F_SUPERDO();
}
//+
///Thread_Get
F_METHOD(void, Thread_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = Msg, item;

    while (F_DynamicNTI(&Tags, &item, Class))
    switch (item.ti_Tag)
    {
        case FA_Thread_Priority: F_STORE((LOD->process) ? LOD->process->pr_Task.tc_Node.ln_Pri : 0); break;
        case FA_Thread_Process: F_STORE(LOD->process); break;
    }
}
//+
///Thread_Set
F_METHOD(void, Thread_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = Msg, item;

    while (F_DynamicNTI(&Tags, &item, Class) != NULL)
    switch (item.ti_Tag)
    {
        case FA_Thread_Priority:
        {
            if (LOD->process)
            {
                SetTaskPri((struct Task *) LOD->process, item.ti_Data);
            }
        }
        break;
    }
}
//+
///Thread_Send
F_METHODM(uint32, Thread_Send, FS_Thread_Send)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    FThreadMsg msg; 

    //   F_Log(0,"SEND 0x%08lx 0x%08lx",((ULONG *)(Msg))[0],((ULONG *)(Msg))[1]);

    msg.Return = 0; 

    /* prevent process death during a send */  
    
    ObtainSemaphoreShared(&LOD->arbitrer);
    
    if (LOD->process)
    {
        msg.SysMsg.mn_Node.ln_Succ = NULL;
        msg.SysMsg.mn_Node.ln_Pred = NULL;
        msg.SysMsg.mn_Node.ln_Type = NT_MESSAGE;
        msg.SysMsg.mn_Node.ln_Pri = 0;
        msg.SysMsg.mn_Node.ln_Name = NULL;
        msg.SysMsg.mn_ReplyPort = CreateMsgPort();
        msg.SysMsg.mn_Length = sizeof(FThreadMsg);
        msg.Action = Msg->Cmd;
        msg.Params = ++Msg;

#ifdef DB_SEND
        F_Log(0, "Process 0x%08lx - Port 0x%08lx - Message 0x%08lx (0x%08lx)",
              (ULONG) LOD->process, (ULONG) &LOD->process->pr_MsgPort, (ULONG) &msg, msg.Action);
#endif

        //GOFROMIEL: ajouter un "time out"

        if (msg.SysMsg.mn_ReplyPort)
        {
            uint32 i;
            
            #ifdef DB_SEND
            F_Log(0,"Reply 0x%08lx", (uint32) msg.SysMsg.mn_ReplyPort);
            #endif

            #ifdef DB_SEND
            F_Log(0,"PutMsg >>");
            #endif

            PutMsg(&LOD -> process -> pr_MsgPort, (struct Message *) &msg);
            
            for (i = 0 ; i < 200 ; i++)
            {
                struct Message *replied = GetMsg(msg.SysMsg.mn_ReplyPort);
                    
                if (replied)
                {
                    if (replied == &msg)
                    {
                        #ifdef DB_SEND
                        F_Log(0,"message replied");
                        #endif
                        
                        break;
                    }
                    else
                    {
                        F_Log(0,"Got unexpected message 0x%08lx, waiting 0x%08lx",replied,&msg);
                    }
                }
                else
                {
                    #ifdef DB_SEND
                    F_Log(0,"waiting (%ld)",i);
                    #endif
 
                    WaitTOF();
                }
            }
        
            if (i == 199)
            {
                F_Log(0,"Time's out !!");
            }
/*
            #ifdef DB_SEND
            F_Log(0,"WaitPort >>");
            #endif

            WaitPort(msg.SysMsg.mn_ReplyPort);

            #ifdef DB_SEND
            F_Log(0,"GetMsg >>");
            #endif

            GetMsg(msg.SysMsg.mn_ReplyPort);
*/
            #ifdef DB_SEND
            F_Log(0,"DeleteMsgPort >>");
            #endif

            DeleteMsgPort(msg.SysMsg.mn_ReplyPort);

            #ifdef DB_SEND
            F_Log(0," DONE");
            #endif
        }
    }
    else
    {
        F_Log(0, "Process 0x%08lx is dead (Message = 0x%08lx)!", (ULONG) LOD->process, msg.Action);
    }

    ReleaseSemaphore(&LOD->arbitrer);

    return msg.Return;
}
//+

