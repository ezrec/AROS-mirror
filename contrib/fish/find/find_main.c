
#include "find.h"

/* ------------------------------------------------------------------- */
/*      for cback.o startup */
/* ------------------------------------------------------------------- */

long                      __stack        = 8192;
char                      *__procname    = "*Find V1.0*";
long                      __priority      = 0L;
long                      __BackGroundIO = 0;
extern BPTR               _Backstdout;


struct find_info info;
       
long __OSlibversion = 37;

void __stdargs main(int argc,union wbstart argv)
{
 int   end = FALSE;
 ULONG sig;     
 if(!SetupScreen())
 {
  if(InitInfo())
  {
   if(SetDefaults())
   {
    strcpy(info.strings.load_dir,"env:");
    strcpy(info.strings.load_file,"find.config");
    ReqFile(MODE_LOADCONFIG);
    loadconfig();
    LoadIcon(argc,argv);
    if(OpenBroker(argc,argv.args))
    {
     if(OpenFRX())
     {
      while(!end)
      {
       sig = Wait(info.misc.waitsig);
       if(sig & (1 << info.App.port->mp_SigBit))
       {
        if(HandleApp() == FALSE) end = TRUE;
       }
       if(sig & (1 << Project0Wnd->UserPort->mp_SigBit))
       {
        if(HandleProject0IDCMP() == FALSE) end = TRUE;
       }
       if(sig & (1 << info.broker.nbrk.nb_Port->mp_SigBit))
       {
        if(HandleBroker() == FALSE) end = TRUE;
       }
       if(sig & (1 << info.Rx.Port->mp_SigBit))
       {
        if(HandleRx() == FALSE) end = TRUE;
       }
      }
     }
    }
   }  
  }
  CloseFRX();
  CloseBroker();
  FreeInfo();
  CloseProject0Window();
  CloseDownScreen();
 }
 exit(0);
}

void SetSignals()
{
 info.misc.waitsig    = 0L;
 if(info.App.port)            info.misc.waitsig |= (1 << info.App.port->mp_SigBit); 
 if(Project0Wnd)              info.misc.waitsig |= (1 << Project0Wnd->UserPort->mp_SigBit);
 if(info.broker.nbrk.nb_Port) info.misc.waitsig |= (1 << info.broker.nbrk.nb_Port->mp_SigBit);
 if(info.Rx.Port)             info.misc.waitsig |= (1 << info.Rx.Port->mp_SigBit);
 if(info.Rx.Reply)            info.misc.waitsig |= (1 << info.Rx.Reply->mp_SigBit);
}

int HandleApp()
{
 int rc = TRUE;
 int i;
 if(info.App.amsg = (struct AppMessage *)GetMsg(info.App.port))
 {
  switch(info.App.amsg->am_Type)
  {
   case MTYPE_APPMENUITEM : switch(info.App.amsg->am_ID)
        {
         case 1 : if(Project0Wnd)
                    CloseProject0Window();
                  else
                    OpenProject0Window();
                  SetSignals();
                  break;                    
         case 2 : rc = FALSE;
                  break;
        }
        break;
   case MTYPE_APPWINDOW   :
         info.misc.from_rx = TRUE;
         for(i = 0; i < info.App.amsg->am_NumArgs;i++)
         {
          NameFromLock(info.App.amsg->am_ArgList[i].wa_Lock,info.strings.filename,STRINGSIZE);
          AddPart(info.strings.filename,info.App.amsg->am_ArgList[i].wa_Name,STRINGSIZE);
          filestringClicked();
         }
         info.misc.from_rx = FALSE;
         break;
   default      : break;
  }
  ReplyMsg((struct Message *)info.App.amsg);
 }
 return rc;
}

int InitInfo(void)
{
 if(info.App.port = CreateMsgPort())
 {
  info.App.port->mp_Node.ln_Pri  = 0;
  if(addappmenus())
  {
   NewList(&info.match.list);
   NewList(&info.misc.lines);
   return (TRUE);
  }
 }
 return (FALSE);
}

void FreeInfo(void)
{
 struct Node *n;
 if(info.App.port)
 {
  remappmenus();
   while(info.App.amsg = (struct AppMessage *)GetMsg(info.App.port))
   ReplyMsg((struct Message *)info.App.amsg);
  DeleteMsgPort(info.App.port);
 }
 if(info.data.data)FreeVec(info.data.data);
 while(n = RemHead(&info.match.list))
 {
  free(n->ln_Name);
  free(n);
 }
 Freefile();
 if(info.misc.inp)fclose(info.misc.inp);
}

int SetDefaults()
{
 info.weight.ins      = 1;
 info.weight.sub      = 2;
 info.weight.del      = 3;
 info.match.filter    = 5;
 info.data.size       = 40;
 if(info.data.data)
 {
  FreeVec(info.data.data);
  info.data.data = NULL;
 }
 if(info.data.data = (ULONG *)AllocVec(((info.data.size + 1) * (info.data.size + 1) * sizeof(ULONG)),MEMF_ANY | MEMF_CLEAR))
 {
  info.flags.dist     = 1;
  info.flags.linenum  = 1;
  info.flags.word     = 1;
  info.flags.wnum     = 1;
  info.misc.out      = 0;
  strcpy(info.strings.save_dir,"");
  strcpy(info.strings.save_file,"find.config");
  strcpy(info.strings.outfile,"");
  strcpy(info.strings.search,"");
  strcpy(info.strings.filename,"");
  strcpy(info.strings.delim,"( ){}[]/+-*<>.\"',;:!?$&=#");
  strcpy(info.strings.error,"0:OK");
  strcpy(info.strings.buffer,"");
  info.misc.from_rx = FALSE;
  if(info.misc.inp)
  {
   fclose(info.misc.inp);
   info.misc.inp = NULL;
  }
  Freematchlist();
  info.misc.lasterror = 0;
  info.misc.fromline  = 1;
  info.misc.toline    = 99999;
  info.misc.writeicon = TRUE;
  SetSignals();
  if(Project0Wnd)Project0Render();
  return (TRUE);
 }
 return (FALSE);
}

void Freematchlist()
{
 struct Node *n;
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_Gadget00],Project0Wnd,NULL,GTLV_Labels,-1L,TAG_DONE);
 while(n = RemHead(&info.match.list))
 {
  free(n->ln_Name);
  free(n);
 }
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_Gadget00],Project0Wnd,NULL,GTLV_Labels,&info.match.list,TAG_DONE);
}

int addappmenus()
{
 if(info.App.men_w = AddAppMenuItem(1,0,(UBYTE *)"Find : Toggle Window",info.App.port,TAG_DONE))
 {
  if(info.App.men_q = AddAppMenuItem(2,0,(UBYTE *)"Find : Quit Program",info.App.port,TAG_DONE))
  {
   return (TRUE);
  }
 }
 return (FALSE); 
}

void remappmenus()
{
 if(info.App.men_w)
 {
  RemoveAppMenuItem(info.App.men_w);
  info.App.men_w = NULL;
 }
 if(info.App.men_q)
 {
  RemoveAppMenuItem(info.App.men_q);
  info.App.men_q = NULL;
 }
}

void LoadIcon(int argc, union wbstart argv)
{
 long i;
 if(!argc)
 {
  if(argv.msg->sm_NumArgs > 1)
  {
   for(i = 1; i < argv.msg->sm_NumArgs;i++)
   {
    NameFromLock(argv.msg->sm_ArgList[i].wa_Lock,info.strings.load_dir,256);
    strcpy(info.strings.load_file,argv.msg->sm_ArgList[i].wa_Name);
    loadconfig();
   }
  }
 }
}
