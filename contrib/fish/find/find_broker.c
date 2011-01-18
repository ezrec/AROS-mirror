
#include "find.h"

#define EVT_HOTKEY 1L

int OpenBroker(int argc,char **argv)
{
 UBYTE yn[8];
 char **ttypes;
 if(info.broker.nbrk.nb_Port = CreateMsgPort())
 {
  ttypes = ArgArrayInit(argc,argv);
  info.broker.nbrk.nb_Version = NB_VERSION;
  info.broker.nbrk.nb_Name    = "Find V1.0";
  info.broker.nbrk.nb_Title   = "Pattern Matching Utility";
  info.broker.nbrk.nb_Descr   = " ";
  info.broker.nbrk.nb_Unique  = NBU_UNIQUE | NBU_NOTIFY;
  info.broker.nbrk.nb_Flags   = COF_SHOW_HIDE;
  info.broker.nbrk.nb_Pri = (BYTE)ArgInt(ttypes,"CX_PRIORITY",0);
  strcpy(info.broker.hotkey,(char *)ArgString(ttypes,"CX_POPKEY","control alt f"));  
  if(info.broker.broker = CxBroker(&info.broker.nbrk,NULL))
  {
   if(info.broker.key = CxFilter(info.broker.hotkey))
   {
    AttachCxObj(info.broker.broker,info.broker.key);
    if(info.broker.sender = CxSender(info.broker.nbrk.nb_Port,EVT_HOTKEY))
    {
     AttachCxObj(info.broker.key,info.broker.sender);
     if(info.broker.translate = (CxTranslate(NULL)))
     {
      AttachCxObj(info.broker.key,info.broker.translate);
      if(!CxObjError(info.broker.key))
      {
       ActivateCxObj(info.broker.broker,1L);
       strcpy(yn,(char *)ArgString(ttypes,"CX_POPUP","NO"));
       if(!stricmp(yn,"yes"))
       {
        OpenProject0Window();
       }
       SetSignals();
       return(TRUE);
      }
     }
    }
   }
  }
 }
 return (FALSE);
}

void CloseBroker()
{
 struct Message *msg;
 if(info.broker.broker)
 {
  DeleteCxObjAll(info.broker.broker);
  while(msg = GetMsg(info.broker.nbrk.nb_Port))ReplyMsg(msg);
  DeletePort(info.broker.nbrk.nb_Port);
  ArgArrayDone();
 }
}

int HandleBroker()
{
 ULONG id,type;
 int rc = TRUE;
 CxMsg *msg;
 if(msg = (CxMsg *)GetMsg(info.broker.nbrk.nb_Port))
 {
  id    = CxMsgID(msg);
  type  = CxMsgType(msg);
  ReplyMsg((struct Message *)msg);
  switch(type)
  {
   case CXM_IEVENT   :
        switch(id)
        {
         case EVT_HOTKEY      : if(!Project0Wnd)
                                {
                                 OpenProject0Window();
                                }
                                break;
         default              : break;
        }
        break;
   case CXM_COMMAND  :
        switch(id)
        {
         case CXCMD_DISABLE   : ActivateCxObj(info.broker.broker,0L);
                                break;
         case CXCMD_ENABLE    : ActivateCxObj(info.broker.broker,1L);
                                break;
         case CXCMD_KILL      : rc = FALSE;
                                break;        
         case CXCMD_APPEAR    : if(!Project0Wnd)
                                {
                                 OpenProject0Window();
                                }
                                break;     
         case CXCMD_DISAPPEAR : if(Project0Wnd)
                                {
                                 CloseProject0Window();
                                }
                                break;     
         case CXCMD_UNIQUE    : if(!Project0Wnd)
                                 OpenProject0Window();
                                else
                                 WindowToFront(Project0Wnd);
                                break;
         default     : break;
        }
        break;     
   default           : break;
  }
 }
 return rc;
}

