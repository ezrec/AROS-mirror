/* - cfn -  completes filenames when pressing the "TAB" key */

/*  written in 1993 by Andreas Günther                      */

/*  ( compiled with aztec-C 5.2 )                           */
/*  for more information, please read the doc-file          */


/*  this is NOT great artwork, so                           */

/*           FEEL FREE TO IMPROVE THIS CODE !               */



/* note: this piece of code is real public domain, do with  */
/*       it whatever you want, but I would be happy if you  */
/*       left my name in it :)                              */



#include <string.h>

#include <exec/types.h>
#include <exec/interrupts.h>
#include <exec/memory.h>
#include <intuition/intuitionbase.h>
//#include "functions.h"
#include <devices/input.h>
#include <dos/dosextens.h>
#include <dos/exall.h>
#include <proto/keymap.h>
#include <proto/utility.h>

#include <aros/oldprograms.h>

#define BUF_LEN   256   /* number of buffered characters from input stream */
#define COMPL_LEN  64   /* maximum length of filename completion */
#define FILTER "~(#?.info)"
#define FILTER_LEN 10
#pragma amicall(SysBase, 0, observe_input(a0))

char *ver="$VER: cfn 1.0 (21.6.93)  by Andreas Günther";

struct IntuitionBase *IntuitionBase=NULL;
struct Library *KeymapBase=NULL;
struct UtilityBase *UtilityBase=NULL;
ULONG ilock,sig;
BOOL ready;
struct InputEvent event, *cur_ev;

struct Task *task;
char buf[BUF_LEN];    /* recently typed characters */
struct InputEvent ie_buffer[2*BUF_LEN];    /* recently typed characters as InputEvents*/
int bufpos=0;
BOOL buf_busy=FALSE;
char compl[COMPL_LEN];   /* actual completion */
struct Interrupt *handler;
struct MsgPort *port;
struct IOStdReq *io_req;


BOOL open_libs()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",37);
  KeymapBase = OpenLibrary("keymap.library",37);
  UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library",37);
  return(IntuitionBase!=NULL && KeymapBase!=NULL && UtilityBase!=NULL);
}

void close_libs()
{
  if(NULL!=IntuitionBase)
    CloseLibrary(IntuitionBase);
  if(NULL!=KeymapBase)
    CloseLibrary(KeymapBase);
  if(NULL!=UtilityBase)
    CloseLibrary((struct Library *)UtilityBase);
}



void filter_equal_part(char *file)
{
/* leaves in compl[] only the first chars that are equal with "file" */
  int i=0;
  while(ToUpper(file[i])==ToUpper(compl[i]) && compl[i]!=0 && file[i]!=0)
    i++;
  compl[i]=0;
}

void find_completion(char *part, BPTR dir)
{
/* scans the given dir and finds the longest unique completion */
  char pat[COMPL_LEN*2+6];
  struct ExAllControl *eac;
  struct ExAllData *alldata, *ead;
  BPTR lock;

  compl[0]=0;
  if(NULL!=(alldata=(struct ExAllData *)malloc(3000)))
   {
    strcat(part,FILTER);
    ParsePatternNoCase(part,pat,COMPL_LEN*2+6);
    eac=AllocDosObject(DOS_EXALLCONTROL,NULL);
    if(eac!=NULL)
     {
      eac->eac_MatchString=pat;
      eac->eac_LastKey=0;
      ExAll(dir,alldata,3000,ED_NAME,eac);
      if(eac->eac_Entries!=0)
       {
        strcpy(compl,&alldata->ed_Name[strlen(part)-FILTER_LEN]);
        if(eac->eac_Entries==1)
         {
          struct FileInfoBlock *fileinfo;
          if(NULL!=(fileinfo=AllocDosObject(DOS_FIB,NULL)))
           {
            CurrentDir(dir);
            lock=Lock(alldata->ed_Name,ACCESS_READ);
            Examine(lock,fileinfo);
            if(fileinfo->fib_DirEntryType>0)  /* Directory ? */
              strcat(compl,"/");
            UnLock(lock);
            FreeDosObject(DOS_FIB,fileinfo);
           }
         }
        else
          for(ead=alldata->ed_Next; ead!=NULL; ead=ead->ed_Next)
           {
            filter_equal_part(&ead->ed_Name[strlen(part)-FILTER_LEN]);
           }
       }
      FreeDosObject(DOS_EXALLCONTROL,eac);
     }
    free(alldata);
   }
}



struct InputEvent *observe_input(struct InputEvent *oldevent)
{
/* This is the handler itself.
   It puts every rawkey into the buffer, clears the buffer when a
   SPACE comes along and signals on arrival of a TAB */

  int_start();
  if(!buf_busy)
    for(cur_ev=oldevent; cur_ev!=NULL; cur_ev=cur_ev->ie_NextEvent)
     {
      if(cur_ev->ie_Class==IECLASS_RAWKEY/* && !(cur_ev->ie_Code&0x80)*/)
       {
        if(cur_ev->ie_Code==0x42)    /* TAB */
         {
          event=*cur_ev;
          buf_busy=TRUE;
          Signal(task,sig);
          cur_ev->ie_Class=IECLASS_NULL;
         }
        else if(cur_ev->ie_Code==0x40 ||    /* SPACE */
                cur_ev->ie_Code==0x44 ||    /* RETURN */
                cur_ev->ie_Code==0x4c ||    /* Arrow Up */
                cur_ev->ie_Code==0x4d ||    /* Arrow Down */
                cur_ev->ie_Code==0x4e)      /* Arrow Right */
          bufpos=0;
        else
         {
          ie_buffer[bufpos++]=*cur_ev;
          if(bufpos==BUF_LEN)
            bufpos=0;
         }
       }
     }
  int_end();
  return(oldevent);
}


void put_into_stream(char *str)
{
  /* puts the given string into the input stream */
  int i;
  struct cq_pair {UBYTE code, qual;} raw_char;

  event.ie_NextEvent=NULL;    /* recycle received event */
  for(i=0; str[i]!=0; i++)
   {
    MapANSI(&str[i],1,&raw_char,1,NULL);
    event.ie_Code=raw_char.code;
    event.ie_Qualifier=raw_char.qual;
    io_req->io_Data=(APTR)&event;
    io_req->io_Length=sizeof(struct InputEvent);
    io_req->io_Command=IND_WRITEEVENT;
    DoIO((struct IORequest *)io_req);
   }
}

serve_handler()
{
  /* Waits for a Signal from the handler, finds the appropriate completion
     for the given part of the path and puts it into the input stream
     (sends it to the input device) */

  struct Process *proc;   /* process of the current CLI */
  BPTR old_lock,path_lock;
  int i,strpos;
  char tmp;

  sig=1<<AllocSignal(-1);
  task=FindTask(NULL);    /* for signaling from interrupt */
  for(;;)
   {
    Wait(sig);
    ilock=LockIBase(0);
    proc=IntuitionBase->ActiveWindow->UserData; /* set by cfn_newshell */
    UnlockIBase(ilock);
    if(proc!=NULL)    /* set by "cfn_newshell" (should be started first) */
     {
      strpos=0;
      for(i=0; i<bufpos; i++)
       {
        ie_buffer[i].ie_NextEvent=NULL;
        if(ie_buffer[i].ie_Code==0x41)  /* Backspace */
          buf[strpos ? strpos--:0]=0;
        else
          strpos+=MapRawKey(&ie_buffer[i],&buf[strpos],20,NULL);
       }
      buf_busy=FALSE;
      buf[strpos]=0;  /* buf is now the incomplete filename */
      old_lock=CurrentDir(proc->pr_CurrentDir);   /* CDir of the input shell */
      strpos=(int)((int)PathPart(buf)-(int)buf);
      tmp=buf[strpos];
      buf[strpos]=0;
      path_lock=Lock(buf,ACCESS_READ);
      buf[strpos]=tmp;
      find_completion(FilePart(buf),path_lock);
      put_into_stream(compl);
      CurrentDir(old_lock);
      UnLock(path_lock);
     }
   }
}


main()
{
  if(open_libs())
   {
    if(port=CreatePort(NULL,0))
     {
      if(handler=AllocMem(sizeof(struct Interrupt),MEMF_PUBLIC|MEMF_CLEAR))
       {
        if(io_req=(struct IOStdReq *)CreateExtIO(port,sizeof(struct IOStdReq)))
         {
          if(!OpenDevice("input.device",0,(struct IORequest *)io_req,0))
           {
            handler->is_Code=observe_input;
            handler->is_Data=NULL;
            handler->is_Node.ln_Pri=1;  /* just before console.device */
            handler->is_Node.ln_Name="cfn";
            io_req->io_Data=(APTR)handler;
            io_req->io_Command=IND_ADDHANDLER;
            DoIO((struct IORequest *)io_req);
            serve_handler();  /* never returns... */
           }
          else
            puts("cfn: ERROR: could not open input.device\n");
         }
        else
          puts("cfn: ERROR: could not create IO-Request\n");
       }
      else
        puts("cfn: ERROR: could not allocate memory for interrupt structure\n");
     }
    else
      puts("cfn: ERROR: could not create port \n");
   }
  else
    puts("cfn: ERROR: could not open intuition.library !\n");
  close_libs();
}
