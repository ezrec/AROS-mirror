#include "find.h"

#define RXPORTNAME "FIND"

#define CMD_SET         1
#define CMD_MATCHFILE   2
#define CMD_MATCHSINGLE 3
#define CMD_QUIT        4
#define CMD_FILLCLIP    5
#define CMD_MATCHLINE   6
#define CMD_SIZE        7
#define CMD_INS         8
#define CMD_SUB         9
#define CMD_DEL        10
#define CMD_DIST       11
#define CMD_FROM       12
#define CMD_TO         13
#define CMD_FILENAME   14
#define CMD_DELIM      15
#define CMD_SEARCH     16
#define CMD_OUTPUT     17
#define CMD_OUTFILE    18
#define CMD_FWNUM      19
#define CMD_FLNUM      20
#define CMD_FDIST      21
#define CMD_FWORD      22
#define CMD_STARTB     23
#define CMD_FREQ       24
#define CMD_RESETB     25
#define CMD_LOADCONFIG 26
#define CMD_SAVECONFIG 27
#define CMD_SHOWWINDOW 28
#define CMD_HIDEWINDOW 29

struct Library *RexxSysBase;

struct RxCmd rexxCmd[] =
       {
        "matchfile"   ,CMD_MATCHFILE,
        "matchsingle" ,CMD_MATCHSINGLE,
        "quit"        ,CMD_QUIT,
        "makecliplist",CMD_FILLCLIP,
        "matchline"   ,CMD_MATCHLINE,
        "datasize"    ,CMD_SIZE,
        "insert"      ,CMD_INS,
        "substitute"  ,CMD_SUB,
        "delete"      ,CMD_DEL,
        "filter"      ,CMD_DIST,
        "fromline"    ,CMD_FROM,
        "toline"      ,CMD_TO,  
        "filename"    ,CMD_FILENAME,
        "delimiter"   ,CMD_DELIM,
        "pattern"     ,CMD_SEARCH,  
        "output"      ,CMD_OUTPUT,  
        "outfilename" ,CMD_OUTFILE, 
        "wordnumber"  ,CMD_FWNUM, 
        "reset"       ,CMD_RESETB,
        "linenumber"  ,CMD_FLNUM,
        "distance"    ,CMD_FDIST,   
        "word"        ,CMD_FWORD,   
        "start"       ,CMD_STARTB,   
        "requestfile" ,CMD_FREQ,   
        "showwindow"  ,CMD_SHOWWINDOW,
        "hidewindow"  ,CMD_HIDEWINDOW,
        NULL,0
       };

int OpenFRX(void)
{
 Forbid();
 if(FindPort(RXPORTNAME))
 {
  Permit();
  return FALSE;
 }
 else
 {
  Permit();
  if(RexxSysBase = OpenLibrary("rexxsyslib.library",0))
  {
   if(info.Rx.Port = CreatePort(RXPORTNAME,0))
   {
    if(info.Rx.Reply = CreateMsgPort())
    {
     info.Rx.ourmsg.rm_Node.mn_ReplyPort = info.Rx.Reply;
     info.Rx.ourmsg.rm_Node.mn_Length = sizeof(struct RexxMsg);
     info.Rx.ourmsg.rm_Action = RXFUNC | RXFF_STRING; 
     SetSignals();     
     return(TRUE);
    }
   }
  }
 } 
}

void CloseFRX(void)
{
 struct Message *msg;
 if(info.Rx.Port)
 {
  while(msg = GetMsg(info.Rx.Port))ReplyMsg(msg);
  DeletePort(info.Rx.Port);
 }
 if(info.Rx.Reply)
 {
  DeleteMsgPort(info.Rx.Reply);
 }
 if(RexxSysBase)CloseLibrary(RexxSysBase);
}

int HandleRx(void)
{
 int rc = TRUE;
 UBYTE *line;
 long  i,found;
 UBYTE  cmd_string[STRINGSIZE],arg1[STRINGSIZE],arg2[STRINGSIZE],arg3[STRINGSIZE];
 while(info.Rx.rmsg = (struct RexxMsg *)GetMsg(info.Rx.Port))
 {
   line = info.Rx.rmsg->rm_Args[0];
   sscanf(line,"%s %s %s %s",cmd_string,arg1,arg2,arg3);
   found = FALSE;
   i = 0L;
   while(!found)
   {
    if(rexxCmd[i].cmd == NULL){found = TRUE; i = -1L;}
    else
    {
     if(!Stricmp(rexxCmd[i].cmd,cmd_string))
     {
      found = TRUE;
      i=rexxCmd[i].code;
     }
     else i++;
    }
   }
   if(i != -1L)
   {
    info.misc.from_rx = TRUE;
    switch(i)
    {
     case CMD_QUIT      : rc = FALSE;
                          break;
     case CMD_MATCHSINGLE : if(*arg1 && *arg2)
                          {
                           UBYTE t[STRINGSIZE];
                           BYTE d;
                           strcpy(t,info.strings.search);
                           strcpy(info.strings.search,arg1);
                           strcpy(info.strings.buffer,arg2);
                           d = wld();
                           strcpy(info.strings.search,t);
                           info.Rx.rmsg->rm_Result1 = (LONG)d;
                          }
                          break;
     case CMD_MATCHFILE : if(*arg1 && *arg2)
                          {
                           strcpy(info.strings.filename,arg1);
                           filestringClicked();
                           strcpy(info.strings.search,arg2);
                           patternClicked();
                           startClicked();
                          }
                          break;
     case CMD_FILLCLIP  : FillClip();
                          break;
     case CMD_MATCHLINE : if(*arg1)
                          {
                           struct Node n;
                           n.ln_Name = line + strlen(cmd_string) + 1;
                           Freematchlist();
                           matchline(&n,0);
                           outputlist();
                          }
                          break;
     case CMD_SIZE      : if(*arg1)
                          {
                           ULONG n = atol(arg1);
                           if(n >= 20)
                           {
                            info.data.size = n;
                            sizeClicked();
                           }
                          }
                          break;
     case CMD_INS       : if(*arg1)
                          {
                           ULONG n = atol(arg1);
                           if(n >= 0)
                           {
                            info.weight.ins = n;
                            insertClicked();
                           }
                          }
                          break;
     case CMD_SUB       : if(*arg1)
                          {
                           ULONG n = atol(arg1);
                           if(n >= 0)
                           {
                            info.weight.sub = n;
                            subClicked();
                           }
                          }
                          break;
     case CMD_DEL       : if(*arg1)
                          {
                           ULONG n = atol(arg1);
                           if(n >= 0)
                           {
                            info.weight.del = n;
                            delClicked();
                           }
                          }
                          break;
     case CMD_DIST      : if(*arg1)
                          {
                           ULONG n = atol(arg1);
                           if((n >= 0)&&(n<=127))
                           {
                            info.match.filter = n;
                            filterClicked();
                           }
                          }
                          break;
     case CMD_FROM      : if(*arg1)
                          {
                           ULONG n = atol(arg1);
                           if((n >= 0)&&(n < info.misc.toline))
                           {
                            info.misc.fromline = n;
                            fromlineClicked();
                           }
                          }
                          break;
     case CMD_TO        : if(*arg1)
                          {
                           ULONG n = atol(arg1);
                           if((n >= 0)&&(n > info.misc.fromline))
                           {
                            info.misc.toline = n;
                            tolineClicked();
                           }
                          }
                          break;
     case CMD_FILENAME  : if(*arg1)
                          {
                           strncpy(info.strings.filename,arg1,STRINGSIZE);
                           filestringClicked();
                          }
                          break;
     case CMD_DELIM     : if(*arg1)
                          {
                           strncpy(info.strings.delim,line + strlen(cmd_string) + 1,STRINGSIZE);
                           delimClicked();
                          }

                          break;
     case CMD_SEARCH    : if(*arg1)
                          {
                           strncpy(info.strings.search,arg1,STRINGSIZE);
                           patternClicked();
                          }
                          break;
     case CMD_OUTPUT    : if(*arg1)
                          {
                           if(!stricmp(arg1,"none"))info.misc.out = 0;
                           if(!stricmp(arg1,"file"))info.misc.out = 1;
                           if(!stricmp(arg1,"printer"))info.misc.out = 2;
                           outputClicked();
                          }
                          break;
     case CMD_OUTFILE   : if(*arg1)
                          {
                           strncpy(info.strings.outfile,arg1,STRINGSIZE);
                           outfilenameClicked();
                          }
                          break;
     case CMD_FWNUM     : if(*arg1)
                          {
                           if(!stricmp(arg1,"on"))info.flags.wnum = FALSE;
                            else info.flags.wnum = TRUE;    
                           flag_wnumClicked();
                          }
                          break;
     case CMD_FLNUM     : if(*arg1)
                          {
                           if(!stricmp(arg1,"on"))info.flags.linenum = FALSE;
                            else info.flags.linenum = TRUE;    
                           flag_linenumberClicked();
                          }
                          break;
     case CMD_FDIST     : if(*arg1)
                          {
                           if(!stricmp(arg1,"on"))info.flags.dist = FALSE;
                            else info.flags.dist = TRUE;    
                           flag_distClicked();
                          }

                          break;
     case CMD_FWORD     : if(*arg1)
                          {
                           if(!stricmp(arg1,"on"))info.flags.word = FALSE;
                            else info.flags.word = TRUE;    
                           flag_wordClicked();
                          }
                          break;
     case CMD_STARTB    : startClicked();
                          break;
     case CMD_FREQ      : ReqFile(MODE_INSERTNAME);
                          filestringClicked();  
                          break;
     case CMD_RESETB    : Project0reset();
                          break;
     case CMD_SHOWWINDOW: if(!Project0Wnd)
                          {
                           OpenProject0Window();
                          }
                          break;
     case CMD_HIDEWINDOW: if(Project0Wnd)
                          {
                           CloseProject0Window();
                          }
                          break;
     default            : break;
    }
    info.misc.from_rx = FALSE;
   }
  ReplyMsg((struct Message *)info.Rx.rmsg);
 }
 return rc;
}

void SendAClip(UBYTE *clip)
{
 struct MsgPort *rx;
 if(info.Rx.ourmsg.rm_Args[0] = CreateArgstring(clip,strlen(clip)))
 {
  Forbid();
  if(rx = FindPort("REXX"))
  {
   PutMsg(rx,&info.Rx.ourmsg);
   Permit();
   WaitPort(info.Rx.Reply);
   (void)GetMsg(info.Rx.Reply);
  }
  DeleteArgstring(info.Rx.ourmsg.rm_Args[0]);
 }  
}

void FillClip(void)
{
 struct Node    *n;
 ULONG           c = 0;     
 UBYTE  clip[STRINGSIZE + 32];
 sprintf(clip,"setclip('MATCHCOUNT','%ld')",info.match.count);
 SendAClip(clip);
 putmsg(23);
 for(n = info.match.list.lh_Head;n->ln_Succ;n = n->ln_Succ,c++)
 {
  sprintf(clip,"setclip('MATCH.%ld','%s')",c,n->ln_Name);
  SendAClip(clip);
 }
 putmsg(24);
}
