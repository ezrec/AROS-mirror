#include "Private.h"

//#define DB_NOTIFY
//#define DB_ADDNOTIFY
//#define DB_REMNOTIFY

/*

GOFROMIEL: Attention guillaume, il y a UN SEUL Thread pour une multitude de
notifications,  et  pas  un Thread par notification. Ca m'inquiétait que tu
garde une trace de la derniere 'NotifyRequest'...

*/

///Thread_Main
F_THREAD_ENTRY(Thread_Main)
{
    struct Process *proc = (struct Process *) FindTask(NULL);
    struct MsgPort *port = CreateMsgPort();
            
    uint32 signals = (1 << proc -> pr_MsgPort.mp_SigBit);
 
    BOOL read_again = FALSE;
    
    if (port)
    {
        signals |= (1 << port -> mp_SigBit);
    }

    for (;;)
    {
        FThreadMsg *msg = (FThreadMsg *) GetMsg(&proc -> pr_MsgPort);

/*** read messages *********************************************************/

        if (msg)
        {
            read_again = TRUE;

            switch (msg->Action)
            {
                case FV_Thread_Hello:
                {
                    if (port)
                    {
                        msg->Return = TRUE;
                    }
                }
                break;

                case FV_Thread_Bye:
                {
                    
                    /* We get the 'bye' message, when there is not a single
                    DOSNotify object alive, thus, no notify request either.
                    To be safe, we flush the message port and  destroy  it.
                    There shall be no notify request alive. */
 
                    if (port)
                    {
                        struct Message *pending;
                        
                        while ((pending = GetMsg(port)) != NULL)
                        {
                            ReplyMsg(pending);
                        }

                        DeleteMsgPort(port);
                    }

                    msg -> Return = TRUE;
                    
                    ReplyMsg((struct Message *) msg);
                    
                    return;
                }
                break;

                case FV_Thread_AddNotify:
                {
                    struct FS_Thread_AddNotify *par = msg -> Params;
                    struct NotifyRequest *nr = par -> NReq;

                    if (nr && port)
                    {
///DB_ADDNOTIFY
                        #ifdef DB_ADDNOTIFY
                        F_Log(0,"Adding NR 0x%08lx - Name '%s'",nr,nr -> nr_Name);
                        #endif
//+
                        nr -> nr_Flags = NRF_SEND_MESSAGE;
                        nr -> nr_UserData = (uint32)(par -> Object);
                        nr -> nr_stuff.nr_Msg.nr_Port = port;

                        if (StartNotify(nr))
                        {
                            msg -> Return = TRUE;
                        }
                    }
                }
                break;

                case FV_Thread_RemNotify:
                {
                    struct FS_Thread_RemNotify *par = msg -> Params;
                    struct NotifyRequest *nr = par -> NReq;

                    if (nr)
                    {
///DB_REMNOTIFY
#ifdef DB_REMNOTIFY
                        F_Log(0,"Removing NR 0x%08lx - Name '%s'",nr,nr -> nr_Name);
#endif
//+
                        EndNotify(nr);
                    }
                }
                break;
            }

            ReplyMsg((struct Message *) msg);
        }
                
/*** dos notify port *******************************************************/
    
        if (port)
        {
            msg = (FThreadMsg *) GetMsg(port);

            if (msg)
            {
                read_again = TRUE;
///DB_NOTIFY
            #ifdef DB_NOTIFY
            F_Log(0,"NOTIFY: Request 0x%08lx - File (%s)",((struct NotifyMessage *)(msg)) -> nm_NReq,((struct NotifyMessage *)(msg)) -> nm_NReq -> nr_Name);
            #endif
//+
                ReplyMsg((struct Message *) msg);

                F_Set((FObject)(((struct NotifyMessage *)(msg)) -> nm_NReq -> nr_UserData),CUD -> id_Update,(uint32)(((struct NotifyMessage *)(msg)) -> nm_NReq -> nr_Name));
            }
        }

/*** waiting ***************************************************************/
 
        if (!read_again)
        {
            Wait(signals);
        }
        else
        {
            read_again = FALSE;
        }
    }
    return;
}
//+

